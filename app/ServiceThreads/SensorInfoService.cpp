/*
 *  SensorInfoService.h
 *
 *  Created on: 2020/05/07
 *      Author: ikki
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include <math.h> // atan2

#include <netdb.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/errno.h>

#include <curl/curl.h>

#include "SensorInfoService.h"
#include "../network/inter_thread_pubsub.hpp"

using namespace std;

static size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

SensorInfoService::SensorInfoService(wqueue<WorkItem*> & SensorInfoJobQueue, wqueue<WorkItem*> & MySQLJobQueue, wqueue<DataItem*> & MainJobQueue, DeviceManager * pDevManager, DBManager * dbManager)
    : mSensorInfoJobQueue(SensorInfoJobQueue), mMySQLJobQueue(MySQLJobQueue), mMainJobQueue(MainJobQueue), mDeviceManager(pDevManager), mDbManager(dbManager)
{
}

SensorInfoService::~SensorInfoService()
{
}

bool SensorInfoService::status()
{
    return mInitial;
}

std::string SensorInfoService::statusString()
{
    return mStatus;
}

void * SensorInfoService::run()
{
    while(1)
    {
        try
        {
            runloop();
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("SensorInfoService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "SensorInfoService()::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void SensorInfoService::runloop()
{
    long long start1 = getSystemTime();
    long long end1 = getSystemTime();
    long long difft = end1 - start1;
    long long calcTime = 0;
    long long updateGateInfoTime = 0;
    long long updateFlyInfoTime = 60000;
    long long resetTime = 3550000;


    mLiDARInfoList = new std::list<LiDARSensorInfoStruct*>();
    mSensorEventList = new std::list<SensorEventStruct*>();
    mFlyInfoDepartureList = new std::list<FlyInfoDepartureStruct*>();
    mFlyInfoArrivalList = new std::list<FlyInfoArrivalStruct*>();
    mFlyScheduleDepartureList = new std::list<FlyScheduleDepartureStruct*>();
    mFlyScheduleArrivalList = new std::list<FlyScheduleArrivalStruct*>();
    mFlyGateInfoList = new std::list<FlyGateInfoStruct*>();

    mInitial = true;

    mStatus = "SensorInfoService()::run() Initial Courses successfully!";
    vilslog::printf ("SensorInfoService()::run() Initial Courses successfully!\n");

    update_last_trigger_event();
    updateGateInfo();

    ITPS::Publisher<std::string> pub1("Topic1", "Msg1");

    // int step = 0;

    while (1)
    {
        WorkItem * item = mSensorInfoJobQueue.removetimed(5000);
        if( item != NULL )
        {
            processDataItem(item);
            delete item;
        }

        // pub1.publish("Hello, I'm SensorInfoService");

        end1 = getSystemTime();
        difft = end1 - start1;

        calcTime += difft;
        updateGateInfoTime += difft;
        updateFlyInfoTime += difft;
        resetTime += difft;

        if ( updateGateInfoTime >= 43200000) //  43200 seconds, 12 hours
        {
            updateGateInfo();

            updateGateInfoTime = 0;
        }

        if ( calcTime >= 10000) //  10 seconds
        {
            calculate_LiDAR_value();

            // vilslog::printf ("SensorInfoService()::run() step[%d]\n", step);

            calcTime = 0;
        }

        if ( updateFlyInfoTime >= 60000) //  60 seconds
        {
            updateFlyInfo();
            updateFlySchedule();

            std::string time = currentTimeForSQL();

            if ( time.compare("02:30") == 0 )
            {
                release_old_info();
            }

            updateFlyInfoTime = 0;
        }

        if ( resetTime >= 3600000) //  3600 seconds, one hours
        {
            reset_sensor_device();
            resetTime = 0;
        }


        start1 = end1;
    }
}

void SensorInfoService::update_last_trigger_event()
{
    int projectid = 1;
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        std::list<FlyInfoStruct*> * recordList = NULL;
        //FlyInfoStruct * lastInfo = NULL;
        for(int i=0; i<10; i++)
        {
            std::string FDATE = TodayDateType1(i);
            recordList = baseDb->get_FlyInfos(FDATE);

            for (std::list<FlyInfoStruct*>::iterator flyinfo = recordList->begin(); flyinfo != recordList->end(); flyinfo++)
            {
                if( (*flyinfo)->triggerTime.length() > 0 )
                {
                    time_t triggerTime = StringDateTimeTotime_t((*flyinfo)->triggerTime);

                    bool bFound = false;
                    for (std::list<SensorEventStruct*>::iterator event = mSensorEventList->begin(); event != mSensorEventList->end(); event++)
                    {
                        if ( (*flyinfo)->sensorid.compare((*event)->sensorname) == 0 )
                        {
                            // update changeTime  triggerTime
                            if (triggerTime > (*event)->triggerTime)
                            {
                                (*event)->gate = (*flyinfo)->airBoardingGate;
                                (*event)->changeTime = triggerTime;
                                (*event)->triggerTime = triggerTime;
                                (*event)->pre_airLineCode = (*flyinfo)->airLineCode;
                                (*event)->pre_airLineNum = (*flyinfo)->airLineNum;
                            }

                            bFound = true;
                            break;
                        }
                    }
                    if (!bFound)
                    {
                        SensorEventStruct * eventRet = new SensorEventStruct();
                        eventRet->sensorname = (*flyinfo)->sensorid;
                        eventRet->eventname = convert_to_AirLineName((*flyinfo)->airLineCode);
                        eventRet->gate = (*flyinfo)->airBoardingGate;
                        eventRet->type = 0; // 0: enter 1: exit
                        eventRet->send = 0; // 0: not send , 1: already send  Notification
                        eventRet->triggerCount = 0;
                        eventRet->changeTime = triggerTime;
                        eventRet->triggerTime = triggerTime;
                        eventRet->startTime = 0;
                        eventRet->pre_airLineCode = (*flyinfo)->airLineCode;
                        eventRet->pre_airLineNum = (*flyinfo)->airLineNum;

                        // vilslog::printf ("SensorInfoService()::update_last_trigger_event() sensorid[%s] triggerTime[%s] airLineNum[%s] airLineCode[%s] airBoardingGate[%s]\n",
                        //     lastInfo->sensorid.c_str(), lastInfo->triggerTime.c_str(), lastInfo->airLineNum.c_str(), lastInfo->airLineCode.c_str(), lastInfo->airBoardingGate.c_str());

                        mSensorEventList->push_back(eventRet);
                    }
                }
            }
            recordList->clear();
            delete recordList;
            recordList = NULL;
        }
    }

    for (std::list<SensorEventStruct*>::iterator event = mSensorEventList->begin(); event != mSensorEventList->end(); event++)
    {
        vilslog::printf ("SensorInfoService()::update_last_trigger_event() sensorname[%s] changeTime[%s] triggerTime[%s] pre_airLineNum[%s] pre_airLineCode[%s] gate[%s]\n",
            (*event)->sensorname.c_str(), time_tToTimeString((*event)->changeTime).c_str(), time_tToTimeString((*event)->triggerTime).c_str(), (*event)->pre_airLineNum.c_str(), (*event)->pre_airLineCode.c_str(), (*event)->gate.c_str());

    }
}

void SensorInfoService::updateGateInfo()
{
    int projectid = 1;
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
    {
        return;
    }

    for (std::list<SensorEventStruct*>::iterator event = mSensorEventList->begin(); event != mSensorEventList->end(); event++)
    {
        //if ((*event)->gate.length() == 0 )
        //{
            std::string gate = baseDb->get_SensorGateInfo((*event)->sensorname);

            (*event)->gate = gate;
        //}
        vilslog::printf ("SensorInfoService()::updateGateInfo() sensorname[%s] gate[%s]\n", (*event)->sensorname.c_str(), (*event)->gate.c_str());
    }

    if (mFlyGateInfoList != NULL)
    {
        mFlyGateInfoList->clear();
        delete mFlyGateInfoList;
    }

    mFlyGateInfoList = baseDb->get_FlyGateInfos();

    vilslog::printf ("SensorInfoService()::updateGateInfo() mFlyGateInfoList->size[%d]\n", (int)mFlyGateInfoList->size());

}

int SensorInfoService::processDataItem(WorkItem * workItem)
{
    if ( strcmp(workItem->getType(), "LiDAR_sensorinfo") == 0 )
    {
        std::string timestamp = workItem->getDateTime();
        time_t timestamp_t = StringTimeZoneDateTimeTotime_t(timestamp.c_str());
        std::string distance = filterValidNumberString(workItem->mSensorInfo.value.c_str(), workItem->mSensorInfo.value.length());

        int iDistance = convert_string_to_int(distance);
        // vilslog::printf ("SensorInfoService()::processDataItem() name[%s] iDistance[%d] timestamp[%s] timestamp_t[%s] currentDateTime[%s]\n",
        //     workItem->mSensorInfo.name.c_str(), iDistance, timestamp.c_str(), time_tToString(timestamp_t).c_str(), currentDateTimeForSQL().c_str());

        put_to_LiDAR_list(workItem->mSensorInfo.name, iDistance, timestamp_t);
    }

    return 0;
}

int SensorInfoService::put_to_LiDAR_list(std::string name, int distance, time_t timestamp)
{
    bool bFound = false;

    int projectid = 1;
    mDeviceManager->setSensorDeviceValue(projectid, name, to_string(distance), time_tToString(timestamp));

    if (name.compare("LiDAR0f0032") == 0)
    {
        return 0;
    }

    for (std::list<LiDARSensorInfoStruct*>::iterator lidar = mLiDARInfoList->begin(); lidar != mLiDARInfoList->end(); lidar++)
    {
        if ( (*lidar)->name.compare(name) == 0 )
        {
            int put_index = (*lidar)->index;
            if ( (*lidar)->size >= MAX_VALUE_SIZE)
            {
                if ( put_index >= MAX_VALUE_SIZE)
                {
                    put_index = 0;
                    (*lidar)->index = 0;
                }
            }
            else
            {
                (*lidar)->size++;
            }

            // vilslog::printf ("SensorInfoService()::put_to_LiDAR_list() size[%d] put_index[%d] distance[%d]\n", (int)(*lidar)->size, put_index, distance);

            //=============================
            // mInput[0] = distance;
            // (*lidar)->smoothingFilter->update(mInput, mResult);
            // (*lidar)->value[put_index] = (int)mResult[0];
            //=============================
            (*lidar)->value[put_index] = distance;
            //=============================

            (*lidar)->index++;
            (*lidar)->updateTime = timestamp;
            // (*lidar)->updateTime = getCurrentTimeInSec();

            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        LiDARSensorInfoStruct * newInfo = new LiDARSensorInfoStruct();
        newInfo->name = name;
        newInfo->index = 0;
        newInfo->size = 0;
        newInfo->average = 0.0;
        newInfo->StandardDeviation = 0.0;
        newInfo->maxDistance = 0;
        newInfo->minDistance = 0;
        newInfo->smoothingFilter = new SmoothingFilter();
        for(int i=0;i<MAX_VALUE_SIZE;i++)
            newInfo->value[i] = 0;
        newInfo->updateTime = timestamp;
        newInfo->processTime = 0;

        // update first data
        mInput[0] = 0;
        mInput[1] = 0;
        mInput[2] = 0;
        newInfo->smoothingFilter->initial(mInput);

        mInput[0] = distance;
        newInfo->smoothingFilter->update(mInput, mResult);

        newInfo->value[newInfo->index] = (int)mResult[0];
        newInfo->index++;
        newInfo->size++;

        // vilslog::printf ("SensorInfoService()::put_to_LiDAR_list() name[%s]\n", name.c_str());

        mLiDARInfoList->push_back(newInfo);
    }

    return 0;
}

void SensorInfoService::calculate_LiDAR_value()
{
    for (std::list<LiDARSensorInfoStruct*>::iterator lidar = mLiDARInfoList->begin(); lidar != mLiDARInfoList->end(); lidar++)
    {
        if (((*lidar)->size >= MAX_VALUE_SIZE) && ((*lidar)->processTime != (*lidar)->updateTime))
        {
            SensorEventStruct * event = getEvent((*lidar)->name, std::string("華信航空 班機"));

            bool skipOne = false;
            //bool skipOne = true;
            //if ( event->type == 1 )
            //    skipOne = false;
            float average = averageCalc((*lidar)->value, (*lidar)->size, skipOne);
            float variance = varianceCalc(average, (*lidar)->value, (*lidar)->size, skipOne);
            float sd = sqrt(variance); // 得到標準差

            (*lidar)->average = average;
            (*lidar)->StandardDeviation = sd;

            (*lidar)->maxDistance = 0;
            (*lidar)->minDistance = 1000000;
            for (int i = 0; i < (*lidar)->size; i++) // 求最大與最小值
            {
                if ((*lidar)->maxDistance < (*lidar)->value[i])
                    (*lidar)->maxDistance = (*lidar)->value[i];
                if ((*lidar)->minDistance > (*lidar)->value[i])
                    (*lidar)->minDistance = (*lidar)->value[i];
            }

            // vilslog::printf("SensorInfoService::calculate_LiDAR_value() name[%s] size[%d] maxDistance[%d] minDistance[%d] average[%f] sd[%f]\n",
            //     (*lidar)->name.c_str(), (*lidar)->size, (*lidar)->maxDistance, (*lidar)->minDistance, average, sd);

            int type = check_LiDAR_event((*lidar), event->type);
            if (type >= 0)
            {
                if (event->type != type)
                {
                    if((type == 1 && (*lidar)->maxDistance > 1) ||
                       (type == 0 ))
                    {
                        vilslog::printf("SensorInfoService::calculate_LiDAR_value() name[%s] type[%d] -> type[%d] maxDistance[%d] minDistance[%d] average[%f] sd[%f]\n",
                            (*lidar)->name.c_str(), event->type, type, (*lidar)->maxDistance, (*lidar)->minDistance, average, sd);
                        // vilslog::printf("SensorInfoService::calculate_LiDAR_value() name[%s] average[%f] variance[%f] sd[%f]\n",
                        //     (*lidar)->name.c_str(), average, variance, sd);
                    }
                }
                check_Fly_event((*lidar), type, event, (*lidar)->updateTime);
            }

            (*lidar)->processTime = (*lidar)->updateTime;
            // vilslog::printf("SensorInfoService::calculate_LiDAR_value() \n\n");
        }
    }
}

int SensorInfoService::check_LiDAR_event(LiDARSensorInfoStruct * lidar, int curType)
{
    int type = -1; // not trigger

    if ( lidar->average > THRESHOD_AVG_ENTER &&
         lidar->maxDistance > THRESHOD_DISTANCE )
    {
        type = 0;
    }
    else
    if ( lidar->StandardDeviation < THRESHOD_SD &&
         lidar->average < THRESHOD_AVG &&
         lidar->maxDistance < THRESHOD_DISTANCE )
    {
        // leave
        type = 1;
    }
    else
    {
        // not trigger
        type = -1;
    }

    // vilslog::printf("SensorInfoService::check_LiDAR_event() name[%s] curType[%d] type[%d] StandardDeviation[%f] average[%f] maxDistance[%d] \n",
    //     lidar->name.c_str(), curType, type, lidar->StandardDeviation, lidar->average, lidar->maxDistance);

    return type;
}

void SensorInfoService::check_Fly_event(LiDARSensorInfoStruct * lidar, int type, SensorEventStruct * event, time_t updateTime)
{
    std::string today = TodayDateType1();

    // vilslog::printf("SensorInfoService::check_Fly_event() event->type[%d] type[%d] startTime[%d] \n", event->type, type, event->startTime);

    if ( event->startTime == 0 )
    {
        // assign current type
        event->type = type;
        event->send = 0;
        event->startTime = updateTime;

        vilslog::printf("SensorInfoService::check_Fly_event() name[%s] event->type[%d] type[%d] startTime[%s] \n",
            lidar->name.c_str(), event->type, type, time_tToString(event->startTime).c_str());
    }
    else
    //if ( event->send == 0 )
    {
        // vilslog::printf("SensorInfoService::check_Fly_event() event->type[%d] -> [%d] triggerCount[%d]\n", event->type, type, event->triggerCount);

        // check type changed
        if ( event->type != type )
        {
            int diffTime = (int)(updateTime - event->changeTime);
            // int diffTime = (int)(getCurrentTimeInSec() - event->changeTime);

            vilslog::printf("SensorInfoService::check_Fly_event() name[%s] event->type[%d] -> [%d] triggerCount[%d] currentDateTime[%s] updateTime[%s]\n",
                lidar->name.c_str(), event->type, type, event->triggerCount, currentDateTimeForSQL().c_str(), time_tToString(updateTime).c_str());
            vilslog::printf("SensorInfoService::check_Fly_event() name[%s] lidar->average[%f] StandardDeviation[%f] \n",
                lidar->name.c_str(), lidar->average, lidar->StandardDeviation);
            vilslog::printf("SensorInfoService::check_Fly_event() name[%s] triggerTime[%s] changeTime[%s] diffTime[%d]\n",
                lidar->name.c_str(), time_tToString(event->triggerTime).c_str(), time_tToString(event->changeTime).c_str(), diffTime);

            if ( diffTime >= EVENT_CHANGE_TIME )
            {
                if ( event->triggerCount == 0 )
                {
                    event->triggerTime = updateTime;
                    // event->triggerTime = getCurrentTimeInSec();
                    vilslog::printf("SensorInfoService::check_Fly_event() name[%s] event->type[%d] -> [%d] triggerTime[%s] \n",
                        lidar->name.c_str(), event->type, type, time_tToString(event->triggerTime).c_str());
                }
                event->triggerCount++;

                if ( event->triggerCount >= EVENT_TRIGGER_COUNT )
                {
                    int projectid = 1;
                    int maingroupid = 1;
                    std::string eventname = event->eventname;
                    std::string airLineNum = event->eventname;
                    std::string scheduleTime = "";
                    std::string realTime = "";
                    std::string state = "";
                    std::string action = "Enter";
                    std::string eventtype = "";
                    std::string fly_json = "";
                    bool validEvent = false;

                    // update info maybe airBoardingGate update
                    updateFlySchedule();

                    if (type == 0)
                    {
                        action = "Enter";
                        eventtype = "Arrival";

                        std::string curTime = convertSimpleTime(updateTime);
                        // std::string curTime = currentSimpleTime();
                        FlyInfoArrivalStruct * info = getFlyInfoArrival(today, curTime, event->gate);
                        if (info != NULL)
                        {
                            std::string triggerTime;
                            if ( info->send == 0 )
                            {
                                validEvent = true;
                                event->pre_airLineCode = info->airLineCode;
                                event->pre_airLineNum = info->airLineNum;
                                triggerTime = time_tToString(event->triggerTime);
                            }
                            else
                            {
                                validEvent = false;
                                vilslog::printf("SensorInfoService::check_Fly_event() Enter already send name[%s] airLineNum[%s] STA[%s] ritETA[%s]\n",
                                    lidar->name.c_str(), info->airLineNum.c_str(), info->STA.c_str(), info->ritETA.c_str());
                            }
                            eventname = convert_to_AirLineName(info->airLineCode);
                            eventname.append(" ");
                            eventname.append(info->airLineNum);
                            airLineNum = info->airLineNum;
                            scheduleTime = info->STA;
                            realTime = info->ritETA;
                            info->send = 1;
                            state = "到達";
                            if ( (info->airBoardingGate.length() == 0) && (event->gate.length() > 0) )
                            {
                                info->airBoardingGate = event->gate;
                            }

                            fly_json = convert_to_fly_json_str(info, triggerTime);
                            putFlyInfoToDB(info, triggerTime, lidar->name);
                        }

                        vilslog::printf("SensorInfoService::check_Fly_event() name[%s] event->type[%d] -> [%d] validEvent[%d] Enter\n",
                            lidar->name.c_str(), event->type, type, validEvent);
                    }
                    else
                    {
                        action = "Leave";
                        eventtype = "Departure";

                        std::string curTime = convertSimpleTime(updateTime);
                        // std::string curTime = currentSimpleTime();
                        FlyInfoDepartureStruct * info = getFlyInfoDeparture(today, curTime, event->gate);
                        if (info != NULL)
                        {
                            std::string triggerTime;

                            if ( info->send == 0 )
                            {
                                validEvent = true;

                                updateArrivalBoardingGate(info->FDATE, info->STD, event->pre_airLineCode, event->pre_airLineNum, info->airBoardingGate);

                                event->pre_airLineCode = info->airLineCode;
                                event->pre_airLineNum = info->airLineNum;
                                triggerTime = time_tToString(event->triggerTime);
                            }
                            else
                            {
                                validEvent = false;
                                vilslog::printf("SensorInfoService::check_Fly_event() Leave already send name[%s] airLineNum[%s] STD[%s] ritETD[%s]\n",
                                    lidar->name.c_str(), info->airLineNum.c_str(), info->STD.c_str(), info->ritETD.c_str());
                            }

                            eventname = convert_to_AirLineName(info->airLineCode);
                            eventname.append(" ");
                            eventname.append(info->airLineNum);
                            airLineNum = info->airLineNum;
                            scheduleTime = info->STD;
                            realTime = info->ritETD;
                            info->send = 1;
                            state = "起飛";
                            if ( (info->airBoardingGate.length() == 0) && (event->gate.length() > 0) )
                            {
                                info->airBoardingGate = event->gate;
                            }
                            else
                            if ( (info->airBoardingGate.length() > 0) )
                            {
                                if ( info->airBoardingGate.compare(event->gate) != 0 )
                                {
                                    vilslog::printf("SensorInfoService::check_Fly_event() change gate event->gate[%s] -> info->airBoardingGate[%s]\n",
                                        event->gate.c_str(), info->airBoardingGate.c_str());
                                }
                                event->gate = info->airBoardingGate;
                            }

                            fly_json = convert_to_fly_json_str(info, triggerTime);
                            putFlyInfoToDB(info, triggerTime, lidar->name);
                        }

                        vilslog::printf("SensorInfoService::check_Fly_event() name[%s] event->type[%d] -> [%d] validEvent[%d] Exit\n",
                            lidar->name.c_str(), event->type, type, validEvent);
                    }

                    if (validEvent)
                    {
                        std::string triggerTime = time_tToString(event->triggerTime);
                        std::string areaName = "停機坪 ";
                        areaName.append(triggerTime);
                        if (scheduleTime.length() > 0 )
                        {
                            areaName.append(" 表定");
                            areaName.append(scheduleTime);
                        }
                        if (realTime.length() > 0 )
                        {
                            areaName.append(" 實際");
                            areaName.append(realTime);
                        }
                        areaName.append(state);

                        // std::string eventjson = convert_to_json_str(projectid, 7777, event->eventname.c_str(), action, "停機坪", "0", triggerTime.c_str());
                        //=================================
                        std::string eventjson = convert_to_json_str(projectid, 7777, eventname.c_str(), action, areaName.c_str(), "0", event->sensorname.c_str());

                        WorkItem * witem = new WorkItem("doctorevent", projectid, NODE_TYPE_SENSOR, airLineNum.c_str(), eventjson.c_str());
                        // WorkItem * witem = new WorkItem("doctorevent", projectid, NODE_TYPE_SENSOR, airLineNum.c_str(), eventjson.c_str());
                        witem->setMaingroupid(maingroupid);
                        mMySQLJobQueue.add(witem);
                        //=================================


                        //=================================
                        SensorFiredEventStruct * event = new SensorFiredEventStruct();
                        event->id = -1;
                        event->sensorid = lidar->name;
                        event->type = eventtype;
                        event->jsonvalue = fly_json;
                        event->datetime = "";
                        event->maingroupid = 1; // UserMainGroup.id

                        WorkItem * wevent = new WorkItem("sensorfiredevent", projectid, event);
                        mMySQLJobQueue.add(wevent);

                        delete event;
                        //=================================

                        // vilslog::printf("SensorInfoService::check_Fly_event() lidar->average[%f] StandardDeviation[%f] triggerTime[%s]\n",
                        //     lidar->average, lidar->StandardDeviation, time_tToString(event->triggerTime).c_str());
                    }

                    event->send = 1;
                    event->type = type;
                    event->triggerCount = 0;
                    event->changeTime = updateTime;
                    // event->changeTime = getCurrentTimeInSec();
                }
            }
            else
            {
                // not valid trigger time
            }
        }
        else
        {
            if ( event->triggerCount > 0 )
                event->triggerCount--;
            //event->triggerCount = 0;
        }
    }
}

FlyInfoDepartureStruct * SensorInfoService::getFlyInfoDeparture(std::string day, std::string curTime, std::string gate)
{
    // vilslog::printf ("SensorInfoService::getFlyInfoDeparture() day[%s] curTime[%s]\n", day.c_str(), curTime.c_str());

    std::list<FlyInfoDepartureStruct*> * validInfoList = getFlyInfoDeparturePossible(day, curTime);

    // vilslog::printf ("SensorInfoService::getFlyInfoDeparture() validInfoList->size[%d]\n", (int)validInfoList->size());

    FlyInfoDepartureStruct * retInfo = NULL;

    for (std::list<FlyInfoDepartureStruct*>::iterator info = validInfoList->begin(); info != validInfoList->end(); info++)
    {
        std::string flyGate = getFlyBoardingGate((*info)->airLineCode, (*info)->airLineNum);

        bool valid = false;
        if ( flyGate.length() > 0 && gate.length() > 0 )
        {
            if ((*info)->airBoardingGate.length() > 0)
            {
                if ((*info)->airBoardingGate.compare(gate) == 0 )
                {
                    valid = true;
                    retInfo = NULL; // Found Perfect match, replace previous
                }
            }
            else
            {
                if (flyGate.compare(gate) == 0 )
                {
                    valid = true;
                }
            }
        }
        // else
        // if ( flyGate.length() == 0)
        // {
        //     valid = true;
        // }

        //===============================
        // check target airLine
        if (valid)
        {
            if ((*info)->airLineCode.compare("MDA") != 0 )
            {
                valid = false;
            }
        }
        else
        {
            if ((*info)->airLineCode.compare("MDA") == 0 )
            {
                valid = true;
            }
        }
        //===============================
        // valid = true;
        //===============================

        vilslog::printf ("SensorInfoService::getFlyInfoDeparture() flyGate[%s] gate[%s] (*info)->airBoardingGate[%s] airLineCode[%s] airLineNum[%s] valid[%d]\n",
            flyGate.c_str(), gate.c_str(), (*info)->airBoardingGate.c_str(), (*info)->airLineCode.c_str(), (*info)->airLineNum.c_str(), valid);

        if (valid)
        {
            if (retInfo == NULL)
            {
                retInfo = (*info);

                vilslog::printf ("SensorInfoService::getFlyInfoDeparture() [first] FDATE[%s] STD[%s] amhsETD[%s] ritETD[%s] ATD[%s] airLineCode[%s] airLineNum[%s] airBoardingGate[%s] flyGate[%s]\n",
                    retInfo->FDATE.c_str(), retInfo->STD.c_str(), retInfo->amhsETD.c_str(), retInfo->ritETD.c_str(),
                    retInfo->ATD.c_str(), retInfo->airLineCode.c_str(), retInfo->airLineNum.c_str(), retInfo->airBoardingGate.c_str(), flyGate.c_str());
            }
            else
            {
                // compare STD
                //if (retInfo->STD.compare((*info)->STD) > 0 )
                if (retInfo->amhsETD.length() == 0 )
                {
                    retInfo = (*info);

                    vilslog::printf ("SensorInfoService::getFlyInfoDeparture() [No amhsETD] FDATE[%s] STD[%s] amhsETD[%s] ritETD[%s] ATD[%s] airLineCode[%s] airLineNum[%s] airBoardingGate[%s] flyGate[%s]\n",
                        retInfo->FDATE.c_str(), retInfo->STD.c_str(), retInfo->amhsETD.c_str(), retInfo->ritETD.c_str(),
                        retInfo->ATD.c_str(), retInfo->airLineCode.c_str(), retInfo->airLineNum.c_str(), retInfo->airBoardingGate.c_str(), flyGate.c_str());
                }
                else
                if (retInfo->amhsETD.compare((*info)->amhsETD) > 0 )
                {
                    retInfo = (*info);

                    vilslog::printf ("SensorInfoService::getFlyInfoDeparture() [Use amhsETD] FDATE[%s] STD[%s] amhsETD[%s] ritETD[%s] ATD[%s] airLineCode[%s] airLineNum[%s] airBoardingGate[%s] flyGate[%s]\n",
                        retInfo->FDATE.c_str(), retInfo->STD.c_str(), retInfo->amhsETD.c_str(), retInfo->ritETD.c_str(),
                        retInfo->ATD.c_str(), retInfo->airLineCode.c_str(), retInfo->airLineNum.c_str(), retInfo->airBoardingGate.c_str(), flyGate.c_str());
                }
                else
                {
                    vilslog::printf ("SensorInfoService::getFlyInfoDeparture() [Not Valid] FDATE[%s] STD[%s] amhsETD[%s] ritETD[%s] ATD[%s] airLineCode[%s] airLineNum[%s] airBoardingGate[%s] flyGate[%s]\n",
                        retInfo->FDATE.c_str(), retInfo->STD.c_str(), retInfo->amhsETD.c_str(), retInfo->ritETD.c_str(),
                        retInfo->ATD.c_str(), retInfo->airLineCode.c_str(), retInfo->airLineNum.c_str(), retInfo->airBoardingGate.c_str(), flyGate.c_str());
                }
            }
        }
    }

    validInfoList->clear();
    delete validInfoList;

    if ( retInfo == NULL )
    {
        vilslog::printf ("SensorInfoService::getFlyInfoDeparture() retInfo == NULL day[%s] curTime[%s]\n", day.c_str(), curTime.c_str());
    }
    else
    {
        vilslog::printf ("SensorInfoService::getFlyInfoDeparture() FlyInfoDepartureStruct \n");
        vilslog::printf ("\t FlyInfoDepartureStruct FDATE[%s] STD[%s] amhsETD[%s] ritETD[%s] ATDDate[%s] ATD[%s]\n",
            retInfo->FDATE.c_str(), retInfo->STD.c_str(), retInfo->amhsETD.c_str(), retInfo->ritETD.c_str(), retInfo->ATDDate.c_str(), retInfo->ATD.c_str());
        vilslog::printf ("\t FlyInfoDepartureStruct airLineCode[%s] airLineNum[%s] ArrivalAirport[%s] airPlaneType[%s] REG_NO[%s] airBoardingGate[%s] APPLY_TYPE[%s] send[%d]\n",
            retInfo->airLineCode.c_str(), retInfo->airLineNum.c_str(), retInfo->ArrivalAirport.c_str(), retInfo->airPlaneType.c_str(), retInfo->REG_NO.c_str(),
            retInfo->airBoardingGate.c_str(), retInfo->APPLY_TYPE.c_str(), retInfo->send);
    }
    return retInfo;
}

std::list<FlyInfoDepartureStruct*> * SensorInfoService::getFlyInfoDeparturePossible(std::string day, std::string curTime)
{
    time_t t_curTime = StringSimpleTimeTotime_t(curTime);

    std::list<FlyInfoDepartureStruct*> * validInfoList = new std::list<FlyInfoDepartureStruct*>();

    // FlyInfoDepartureStruct * validInfo = NULL;
    for (std::list<FlyInfoDepartureStruct*>::iterator info = mFlyInfoDepartureList->begin(); info != mFlyInfoDepartureList->end(); info++)
    {
        // if ((*info)->FDATE.compare(day) == 0 && (*info)->STD.compare(curTime) >= 0 )
        // if ((*info)->FDATE.compare(day) == 0 && (*info)->amhsETD.compare(curTime) >= 0 )
        //if ((*info)->FDATE.compare(day) == 0 && (*info)->airLineCode.compare(airLineCode) == 0)
        if ((*info)->FDATE.compare(day) == 0 && (*info)->send == 0)
        {
            bool bValid = false;
            if ( (*info)->amhsETD.length() > 0 && (*info)->amhsETD.compare(curTime) >= 0 )
            {
                time_t t_amhsETD = StringSimpleTimeTotime_t((*info)->amhsETD);
                double diff = difftime(t_amhsETD, t_curTime);

                // 未來20分鐘之內都算有效
                if( diff <= 1200 && diff >= 0)
                {
                    bValid = true;
                }
            }
            else
            if ( (*info)->amhsETD.length() > 0 )
            {
                time_t t_amhsETD = StringSimpleTimeTotime_t((*info)->amhsETD);
                double diff = difftime(t_curTime, t_amhsETD);

                // 過去30分鐘之內都算有效
                if( diff <= 1800 && diff >= 0)
                {
                    bValid = true;

                    // vilslog::printf ("SensorInfoService::getFlyInfoDeparturePossible() t_curTime[%s] t_amhsETD[%s] diff[%s] bValid[%d]\n",
                    //     time_tToTimeString(t_curTime).c_str(), time_tToTimeString(t_amhsETD).c_str(), time_tToTimerString(diff).c_str(), bValid);
                    // vilslog::printf ("SensorInfoService::getFlyInfoDeparturePossible() (*info)->FDATE[%s] (*info)->STD[%s] (*info)->amhsETD[%s]\n",
                    //     (*info)->FDATE.c_str(), (*info)->STD.c_str(), (*info)->amhsETD.c_str());
                }
            }
            else
            if ( (*info)->amhsETD.length() == 0 )
            {
                time_t t_STD = StringSimpleTimeTotime_t((*info)->STD);
                if ( (*info)->STD.compare(curTime) >= 0 )
                {
                    double diff = difftime(t_STD, t_curTime);

                    // 未來20分鐘之內都算有效
                    if( diff <= 1200 && diff >= 0)
                    {
                        bValid = true;
                    }
                }
                else
                {
                    double diff = difftime(t_curTime, t_STD);

                    // 過去30分鐘之內都算有效
                    if( diff <= 1800 && diff >= 0)
                    {
                        bValid = true;
                    }
                }
            }

            if( bValid )
            {
                // vilslog::printf ("SensorInfoService::getFlyInfoDeparture() (*info)->FDATE[%s] (*info)->STD[%s] (*info)->amhsETD[%s]\n",
                //     (*info)->FDATE.c_str(), (*info)->STD.c_str(), (*info)->amhsETD.c_str());

                // 0:not exited, 1:valid, 2: existed but not valid
                int status = checkFlyDepartured((*info)->STD, (*info)->airLineCode, (*info)->airLineNum);

                // ritETD = ""
                // ATDDate = "0000-00-00"
                // ATD = ""
                if ( (status == 1) && ((*info)->ritETD.length() == 0) && ((*info)->ATDDate.compare("0000-00-00") == 0) && ((*info)->ATD.length() == 0) )
                {
                    validInfoList->push_back((*info));
                }
                else
                if (status == 1 || status == 2)
                {
                    vilslog::printf ("SensorInfoService::getFlyInfoDeparture() (*info)->airLineNum[%s] ritETD[%s] ATDDate[%s] ATD[%s] status[%d]\n",
                        (*info)->airLineNum.c_str(), (*info)->ritETD.c_str(), (*info)->ATDDate.c_str(), (*info)->ATD.c_str(), status);

                    validInfoList->push_back((*info));
                }
            }
        }
    }
    return validInfoList;
}

FlyInfoArrivalStruct * SensorInfoService::getFlyInfoArrival(std::string day, std::string curTime, std::string gate)
{
    vilslog::printf ("SensorInfoService::getFlyInfoArrival() day[%s] curTime[%s] gate[%s]\n", day.c_str(), curTime.c_str(), gate.c_str());

    std::list<FlyInfoArrivalStruct*> * validInfoList = getFlyInfoArrivalPossible(day, curTime);

    FlyInfoArrivalStruct * retInfo = NULL;
    FlyInfoArrivalStruct * possibleInfo = NULL;

    vilslog::printf ("SensorInfoService::getFlyInfoArrival() validInfoList->size()[%d]\n", (int)validInfoList->size());

    // vilslog::printf ("SensorInfoService::getFlyInfoArrival() validInfoList->size[%d]\n", (int)validInfoList->size());
    for (std::list<FlyInfoArrivalStruct*>::iterator info = validInfoList->begin(); info != validInfoList->end(); info++)
    {
        std::string flyGate = getFlyBoardingGate((*info)->airLineCode, (*info)->airLineNum);

        bool valid = true;
        // bool valid = false;
        // if ( flyGate.length() > 0 && gate.length() > 0 )
        // {
        //     if (flyGate.compare(gate) == 0 )
        //     {
        //         valid = true;
        //         retInfo = NULL; // Found Perfect match, replace previous
        //     }
        // }
        // else
        // if ( gate.length() > 0 )
        // {
        //     valid = true;
        // }

        //===============================
        // check target airLine
        if (valid)
        {
            if ((*info)->airLineCode.compare("MDA") != 0 )
            {
                valid = false;
            }
        }
        else
        {
            if ((*info)->airLineCode.compare("MDA") == 0 )
            {
                valid = true;
            }
        }
        //===============================
        // valid = true;
        //===============================

        vilslog::printf ("SensorInfoService::getFlyInfoArrival() airLineCode[%s] airLineNum[%s] flyGate[%s] airBoardingGate[%s] gate[%s] valid[%d]\n",
            (*info)->airLineCode.c_str(), (*info)->airLineNum.c_str(), flyGate.c_str(), (*info)->airBoardingGate.c_str(), gate.c_str(), valid);

        if (valid)
        {
            if (retInfo == NULL)
            {
                if ( (*info)->ritETA.length() > 0 )
                {
                    //
                    // 已有班次, 刪選掉 是未來時間
                    //
                    if ( (*info)->ritETA.compare(curTime) <= 0 )
                    {
                        retInfo = (*info);

                        vilslog::printf ("SensorInfoService::getFlyInfoArrival() first FDATE[%s] STA[%s] amhsETA[%s] ritETA[%s] ATADate[%s] ATA[%s] airLineCode[%s] airLineNum[%s] airBoardingGate[%s] flyGate[%s]\n",
                            retInfo->FDATE.c_str(), retInfo->STA.c_str(), retInfo->amhsETA.c_str(), retInfo->ritETA.c_str(),
                            retInfo->ATADate.c_str(), retInfo->ATA.c_str(), retInfo->airLineCode.c_str(), retInfo->airLineNum.c_str(), retInfo->airBoardingGate.c_str(), flyGate.c_str());
                    }
                    else
                    {
                        vilslog::printf ("SensorInfoService::getFlyInfoArrival() 已有班次, 排除掉 是未來時間 FDATE[%s] STA[%s] amhsETA[%s] ritETA[%s] ATADate[%s] ATA[%s] airLineCode[%s] airLineNum[%s] airBoardingGate[%s] flyGate[%s]\n",
                            (*info)->FDATE.c_str(), (*info)->STA.c_str(), (*info)->amhsETA.c_str(), (*info)->ritETA.c_str(),
                            (*info)->ATADate.c_str(), (*info)->ATA.c_str(), (*info)->airLineCode.c_str(), (*info)->airLineNum.c_str(), (*info)->airBoardingGate.c_str(), flyGate.c_str());
                    }
                }
            }
            else
            {
                // compare STA
                //if (retInfo->STA.compare((*info)->STA) < 0 )
                //if (retInfo->amhsETA.compare((*info)->amhsETA) < 0 )
                if (retInfo->ritETA.compare((*info)->ritETA) <= 0 )
                {
                    //
                    // 已有班次, 刪選掉 是未來時間
                    //
                    if ( (*info)->ritETA.compare(curTime) <= 0 )
                    {
                        retInfo = (*info);

                        vilslog::printf ("SensorInfoService::getFlyInfoArrival() update FDATE[%s] STA[%s] amhsETA[%s] ritETA[%s] ATADate[%s] ATA[%s] airLineCode[%s] airLineNum[%s] airBoardingGate[%s] flyGate[%s]\n",
                            retInfo->FDATE.c_str(), retInfo->STA.c_str(), retInfo->amhsETA.c_str(), retInfo->ritETA.c_str(),
                            retInfo->ATADate.c_str(), retInfo->ATA.c_str(), retInfo->airLineCode.c_str(), retInfo->airLineNum.c_str(), retInfo->airBoardingGate.c_str(), flyGate.c_str());
                    }
                    else
                    {
                        vilslog::printf ("SensorInfoService::getFlyInfoArrival() 刪選掉 是未來時間 FDATE[%s] STA[%s] amhsETA[%s] ritETA[%s] ATADate[%s] ATA[%s] airLineCode[%s] airLineNum[%s] airBoardingGate[%s] flyGate[%s]\n",
                            (*info)->FDATE.c_str(), (*info)->STA.c_str(), (*info)->amhsETA.c_str(), (*info)->ritETA.c_str(),
                            (*info)->ATADate.c_str(), (*info)->ATA.c_str(), (*info)->airLineCode.c_str(), (*info)->airLineNum.c_str(), retInfo->airBoardingGate.c_str(), flyGate.c_str());
                    }
                }

                if ( (*info)->amhsETA.length() > 0 )
                {
                    if (retInfo->amhsETA.compare((*info)->amhsETA) <= 0 )
                    {
                        possibleInfo = (*info);

                        vilslog::printf ("SensorInfoService::getFlyInfoArrival() possibleInfo FDATE[%s] STA[%s] amhsETA[%s] ritETA[%s] ATADate[%s] ATA[%s] airLineCode[%s] airLineNum[%s] flyGate[%s]\n",
                            retInfo->FDATE.c_str(), retInfo->STA.c_str(), retInfo->amhsETA.c_str(), retInfo->ritETA.c_str(),
                            retInfo->ATADate.c_str(), retInfo->ATA.c_str(), retInfo->airLineCode.c_str(), retInfo->airLineNum.c_str(), retInfo->airBoardingGate.c_str(), flyGate.c_str());
                    }
                }
            }
        }
    }

    validInfoList->clear();
    delete validInfoList;

    if ( retInfo == NULL )
    {
        vilslog::printf ("SensorInfoService::getFlyInfoArrival() retInfo == NULL day[%s] curTime[%s]\n", day.c_str(), curTime.c_str());

        //
        // using amhsETA
        //
        retInfo = possibleInfo;
    }
    else
    {
        vilslog::printf ("SensorInfoService::getFlyInfoArrival() FlyInfoArrivalStruct \n");
        vilslog::printf ("\t FlyInfoArrivalStruct FDATE[%s] STA[%s] amhsETA[%s] ritETA[%s] ATADate[%s] ATA[%s]\n",
            retInfo->FDATE.c_str(), retInfo->STA.c_str(), retInfo->amhsETA.c_str(), retInfo->ritETA.c_str(), retInfo->ATADate.c_str(), retInfo->ATA.c_str());
        vilslog::printf ("\t FlyInfoArrivalStruct airLineCode[%s] airLineNum[%s] DepartureAirport[%s] airPlaneType[%s] REG_NO[%s] airBoardingGate[%s] APPLY_TYPE[%s] send[%d]\n",
            retInfo->airLineCode.c_str(), retInfo->airLineNum.c_str(), retInfo->DepartureAirport.c_str(), retInfo->airPlaneType.c_str(), retInfo->REG_NO.c_str(),
            retInfo->airBoardingGate.c_str(), retInfo->APPLY_TYPE.c_str(), retInfo->send);

    }
    return retInfo;
}

std::list<FlyInfoArrivalStruct*> * SensorInfoService::getFlyInfoArrivalPossible(std::string day, std::string curTime)
{
    time_t t_curTime = StringSimpleTimeTotime_t(curTime);

    std::list<FlyInfoArrivalStruct*> * validInfoList = new std::list<FlyInfoArrivalStruct*>();
    std::list<FlyInfoArrivalStruct*> * possibleInfoList = new std::list<FlyInfoArrivalStruct*>();

    // FlyInfoArrivalStruct * validInfo = NULL;
    for (std::list<FlyInfoArrivalStruct*>::iterator info = mFlyInfoArrivalList->begin(); info != mFlyInfoArrivalList->end(); info++)
    {
        // if ((*info)->FDATE.compare(day) == 0 && (*info)->STA.compare(curTime) <= 0 )
        //if ((*info)->FDATE.compare(day) == 0 && (*info)->amhsETA.compare(curTime) <= 0 )
        // if ((*info)->FDATE.compare(day) == 0 && (*info)->ritETA.compare(curTime) <= 0 )
        //if ((*info)->FDATE.compare(day) == 0 && (*info)->airLineCode.compare(airLineCode) == 0)
        if ((*info)->FDATE.compare(day) == 0 && (*info)->send == 0)
        {
            // 10 mins range
            bool bValid = false;
            if ( (*info)->ritETA.length() > 0 && (*info)->ritETA.compare(curTime) <= 0 )
            {
                time_t t_amhsETD = StringSimpleTimeTotime_t((*info)->ritETA);
                double diff = difftime(t_curTime, t_amhsETD);
                // 過去1小時內才算
                if( diff <= 1800 && diff >= 0)
                {
                    bValid = true;

                    vilslog::printf ("SensorInfoService::getFlyInfoArrivalPossible() 過去1小時內才算 (*info)->airLineNum[%s] FDATE[%s] STA[%s] ritETA[%s] amhsETA[%s] diff[%d] < 1800\n",
                        (*info)->airLineNum.c_str(), (*info)->FDATE.c_str(), (*info)->STA.c_str(), (*info)->ritETA.c_str(), (*info)->amhsETA.c_str(), (int)diff);
                }
            }
            else
            if ( (*info)->ritETA.length() > 0 )
            {
                time_t t_ritETA = StringSimpleTimeTotime_t((*info)->ritETA);
                double diff = difftime(t_ritETA, t_curTime);
                // 未來20分鐘之內都算有效
                if( diff <= 1200 && diff > 0)
                {
                    bValid = true;

                    vilslog::printf ("SensorInfoService::getFlyInfoArrivalPossible() 未來20分鐘之內都算有效 (*info)->airLineNum[%s] FDATE[%s] STA[%s] ritETA[%s] amhsETA[%s] diff[%d] <= 1200\n",
                        (*info)->airLineNum.c_str(), (*info)->FDATE.c_str(), (*info)->STA.c_str(), (*info)->ritETA.c_str(), (*info)->amhsETA.c_str(), (int)diff);

                    // vilslog::printf ("SensorInfoService::getFlyInfoArrival() t_curTime[%s] t_ritETA[%s] diff[%s] bValid[%d]\n",
                    //     time_tToTimeString(t_curTime).c_str(), time_tToTimeString(t_ritETA).c_str(), time_tToTimerString(diff).c_str(), bValid);
                    // vilslog::printf ("SensorInfoService::getFlyInfoArrival() (*info)->FDATE[%s] STA[%s] ATA[%s] ritETA[%s]\n",
                    //     (*info)->FDATE.c_str(), (*info)->STA.c_str(), (*info)->ATA.c_str(), (*info)->ritETA.c_str());
                }
            }
            else
            if ( (*info)->ritETA.length() == 0 )
            {
                time_t t_amhsETA = StringSimpleTimeTotime_t((*info)->amhsETA);
                double diff = difftime(t_curTime, t_amhsETA);
                // 過去30分鐘之內都算有效
                if( diff <= 1800 && diff > 0)
                {
                    bValid = true;

                    vilslog::printf ("SensorInfoService::getFlyInfoArrivalPossible() 過去30分鐘之內都算有效 (*info)->airLineNum[%s] FDATE[%s] STA[%s] ritETA[%s] amhsETA[%s] diff[%d] <= 1800\n",
                        (*info)->airLineNum.c_str(), (*info)->FDATE.c_str(), (*info)->STA.c_str(), (*info)->ritETA.c_str(), (*info)->amhsETA.c_str(), (int)diff);
                }
            }

            if( bValid )
            {
                // vilslog::printf ("SensorInfoService::getFlyInfoArrival() (*info)->FDATE[%s] (*info)->STA[%s] (*info)->ritETA[%s]\n",
                //     (*info)->FDATE.c_str(), (*info)->STA.c_str(), (*info)->ritETA.c_str());

                // 0:not exited, 1:valid, 2: existed but not valid
                int status = checkFlyArrivaled((*info)->STA, (*info)->airLineCode, (*info)->airLineNum);

                // ritETA = "1230"
                // ATADate = "2021-01-19"
                // ATA = "1230"
                if ( (status != 0) && ((*info)->ATADate.compare(day) == 0) && ((*info)->ATA.length() > 0) && ((*info)->ATA.compare(curTime) <= 0) )
                {
                    validInfoList->push_back((*info));
                }
                else
                if (status == 1)
                {
                    validInfoList->push_back((*info));
                }
                else
                {
                    possibleInfoList->push_back((*info));
                }
            }
        }
    }

    if (validInfoList->size() == 0)
    {
        vilslog::printf ("SensorInfoService::getFlyInfoArrivalPossible() validInfoList->size() == 0 day[%s] curTime[%s]\n", day.c_str(), curTime.c_str());

        //
        // find at least one
        //
        for (std::list<FlyInfoArrivalStruct*>::iterator info = possibleInfoList->begin(); info != possibleInfoList->end(); info++)
        {
            validInfoList->push_back((*info));
        }
        delete possibleInfoList;
    }

    return validInfoList;
}

int SensorInfoService::checkFlyDepartured(std::string STD, std::string airLineCode, std::string airLineNum)
{
    bool status = 0;// 0:not exited, 1:valid, 2: existed but not valid

    // vilslog::printf ("SensorInfoService::checkFlyDepartured STD[%s] airLineCode[%s] airLineNum[%s]\n",
    //     STD.c_str(), airLineCode.c_str(), airLineNum.c_str());

    for (std::list<FlyScheduleDepartureStruct*>::iterator info = mFlyScheduleDepartureList->begin(); info != mFlyScheduleDepartureList->end(); info++)
    {

        // vilslog::printf ("SensorInfoService::checkFlyDepartured expectTime[%s] airLineCode[%s] airLineNum[%s] airFlyStatus[%s]\n",
        //     (*info)->expectTime.c_str(), (*info)->airLineCode.c_str(), (*info)->airLineNum.c_str(), (*info)->airFlyStatus.c_str());
        // if ((*info)->expectTime.compare(STD) == 0 &&
        //     (*info)->airLineCode.compare(airLineCode) == 0 &&
        //     (*info)->airLineNum.compare(airLineNum) == 0 )

        if ((*info)->airLineCode.compare(airLineCode) == 0 &&
            (*info)->airLineNum.compare(airLineNum) == 0 )
        {
            if ( (*info)->airFlyStatus.compare("離站Departed") == 0 )
            {
                // checkValid = true;
                status = 1;
            }
            else
            {
                status = 2;
            }
            break;
        }
    }

    return status;
}

int SensorInfoService::checkFlyArrivaled(std::string STA, std::string airLineCode, std::string airLineNum)
{
    int status = 0;// 0:not exited, 1:valid, 2: existed but not valid

    // vilslog::printf ("SensorInfoService::checkFlyArrivaled STA[%s] airLineCode[%s] airLineNum[%s]\n",
    //     STA.c_str(), airLineCode.c_str(), airLineNum.c_str());

    for (std::list<FlyScheduleArrivalStruct*>::iterator info = mFlyScheduleArrivalList->begin(); info != mFlyScheduleArrivalList->end(); info++)
    {
        // vilslog::printf ("SensorInfoService::checkFlyArrivaled expectTime[%s] airLineCode[%s] airLineNum[%s] airFlyStatus[%s]\n",
        //     (*info)->expectTime.c_str(), (*info)->airLineCode.c_str(), (*info)->airLineNum.c_str(), (*info)->airFlyStatus.c_str());

        // if ((*info)->expectTime.compare(STA) == 0 &&
        //     (*info)->airLineCode.compare(airLineCode) == 0 &&
        //     (*info)->airLineNum.compare(airLineNum) == 0 )
        if ((*info)->airLineCode.compare(airLineCode) == 0 &&
            (*info)->airLineNum.compare(airLineNum) == 0 )
        {
            if ( (*info)->airFlyStatus.compare("抵達Arrived") == 0)
            {
                status = 1;
            }
            else
            if ( (*info)->airFlyStatus.compare("延遲Delayed") == 0 ||
                 (*info)->airFlyStatus.compare("準時On Time") == 0 )
            {
                status = 2;
            }
            break;
        }
    }

    return status;
}

SensorEventStruct * SensorInfoService::getEvent(std::string sensorname, std::string eventname)
{
    SensorEventStruct * eventRet = NULL;
    for (std::list<SensorEventStruct*>::iterator event = mSensorEventList->begin(); event != mSensorEventList->end(); event++)
    {
        if ((*event)->sensorname.compare(sensorname) == 0)
        {
            eventRet = (*event);
            break;
        }
    }

    if ( eventRet != NULL )
    {
        return eventRet;
    }
    else
    {
        eventRet = new SensorEventStruct();
        eventRet->sensorname = sensorname;
        //eventRet->day = day;
        eventRet->eventname = eventname;
        eventRet->gate = "";
        eventRet->type = -1; //-1 : not trigger 0: enter 1: exit
        eventRet->send = 0; // 0: not send , 1: already send  Notification
        eventRet->triggerCount = 0;
        eventRet->changeTime = 0;
        eventRet->triggerTime = 0;
        eventRet->startTime = 0;
        eventRet->pre_airLineCode = "";
        eventRet->pre_airLineNum = "";

        mSensorEventList->push_back(eventRet);
        return eventRet;
    }
}

std::string SensorInfoService::getFlyBoardingGate(std::string airLineCode, std::string airLineNum)
{
    //FlyGateInfoStruct gateInfo;
    //std::string pretty_json = JS::serializeStruct(gateInfo);
    //vilslog::printf ("SensorInfoService::getFlyBoardingGatepretty_json[%s]\n", pretty_json.c_str());

    std::string gate;
    for (std::list<FlyGateInfoStruct*>::iterator gateInfo = mFlyGateInfoList->begin(); gateInfo != mFlyGateInfoList->end(); gateInfo++)
    {
        if ( (*gateInfo)->airLineCode.compare(airLineCode) == 0 && (*gateInfo)->airLineNum.compare(airLineNum) == 0)
        {
            gate = (*gateInfo)->airBoardingGate;

            //std::string pretty_json = JS::serializeStruct(*(*gateInfo));
            //vilslog::printf ("SensorInfoService::getFlyBoardingGatepretty_json[%s]\n", pretty_json.c_str());

            break;
        }
    }
    return gate;
}

void SensorInfoService::updateFlyInfo()
{
    CURL *curl;
  	CURLcode res;
    std::string readBufferArr;
    std::string readBufferDep;

	curl = curl_easy_init();
    if(curl)
	{
        // First set the URL that is about to receive our POST. This URL can
	    // just as well be a https:// URL if that is what should receive the
	    // data.
	    curl_easy_setopt(curl, CURLOPT_URL, "https://ccc.kia.gov.tw/fids/json/arr/");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBufferArr);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);

	    // Perform the request, res will get the return code
	    res = curl_easy_perform(curl);

        curl_easy_setopt(curl, CURLOPT_URL, "https://ccc.kia.gov.tw/fids/json/dep/");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBufferDep);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);

	    // Perform the request, res will get the return code
	    res = curl_easy_perform(curl);


	    // always cleanup
	    curl_easy_cleanup(curl);

        // vilslog::printf("SensorInfoService::updateFlyInfo() readBufferArr[%s]\n", readBufferArr.c_str());
        // vilslog::printf("SensorInfoService::updateFlyInfo() readBufferDep[%s]\n", readBufferDep.c_str());

        updateFlyInfoArr(readBufferArr);
        updateFlyInfoDep(readBufferDep);

    }
}

void SensorInfoService::updateFlyInfoArr(std::string content)
{
    Json::Reader * reader = new Json::Reader();
    Json::Value JSONresult;
    bool parsingSuccessful = false;

    // int strLen = content.length();
    // const char * docCont = content.c_str();
    // vilslog::printf ("SensorInfoService::updateFlyInfoArr() content[%d][%d][%d][%d]\n",
    //     (unsigned int)docCont[0], (unsigned int)docCont[1], (unsigned int)docCont[2], (unsigned int)docCont[3]);
    // vilslog::printf ("SensorInfoService::updateFlyInfoArr() content[%c][%c][%c][%c]\n",
    //     docCont[0], docCont[1], docCont[2], docCont[3]);
    // vilslog::printf ("SensorInfoService::updateFlyInfoArr() content[%d][%d][%d][%d]\n",
    //     docCont[strLen-4], docCont[strLen-3], docCont[strLen-2], docCont[strLen-1]);
    // vilslog::printf ("SensorInfoService::updateFlyInfoArr() content[%c][%c][%c][%c]\n",
    //     docCont[strLen-4], docCont[strLen-3], docCont[strLen-2], docCont[strLen-1]);


    std::string pureCont = remove_nonJSON(content.c_str());

    // int purestrLen = pureCont.length();
    // const char * puredocCont = pureCont.c_str();
    // vilslog::printf ("SensorInfoService::updateFlyInfoArr() puredocCont[%d][%d][%d][%d]\n",
    //     (unsigned int)puredocCont[0], (unsigned int)puredocCont[1], (unsigned int)puredocCont[2], (unsigned int)puredocCont[3]);
    // vilslog::printf ("SensorInfoService::updateFlyInfoArr() puredocCont[%c][%c][%c][%c]\n",
    //     puredocCont[0], puredocCont[1], puredocCont[2], puredocCont[3]);
    // vilslog::printf ("SensorInfoService::updateFlyInfoArr() puredocCont[%d][%d][%d][%d]\n",
    //     puredocCont[purestrLen-4], puredocCont[purestrLen-3], puredocCont[purestrLen-2], puredocCont[purestrLen-1]);
    // vilslog::printf ("SensorInfoService::updateFlyInfoArr() content[%c][%c][%c][%c]\n",
    //     puredocCont[purestrLen-4], puredocCont[purestrLen-3], puredocCont[purestrLen-2], puredocCont[purestrLen-1]);

    try{
        parsingSuccessful = reader->parse( pureCont, JSONresult);
        // vilslog::printf ("SensorInfoService::updateFlyInfoArr() parsingSuccessful[%d]\n", parsingSuccessful);

        if (parsingSuccessful)
        {
            // vilslog::printf ("SensorInfoService::updateFlyInfoArr() JSONresult.isArray()[%d]\n", JSONresult.isArray());
            if ( JSONresult.isArray() )
            {
                // vilslog::printf ("SensorInfoService::updateFlyInfoArr() JSONresult.size()[%d]\n", (int)JSONresult.size());

                for(unsigned int i=0;i<JSONresult.size();i++)
                {
                    Json::Value tmp;
                    Json::Value item = JSONresult.get((Json::UInt)i, tmp);

                    FlyInfoArrivalStruct * newInfo = new FlyInfoArrivalStruct();

                    if ( item.isMember("FDATE") )
                    {
                        std::string FDATE = item["FDATE"].asString();
                        if (FDATE.length() > 0)
                            newInfo->FDATE = FDATE;
                    }
                    if ( item.isMember("STA") )
                    {
                        std::string STA = item["STA"].asString();
                        if (STA.length() > 0)
                            newInfo->STA = STA;
                    }
                    if ( item.isMember("amhsETA") )
                    {
                        std::string amhsETA = item["amhsETA"].asString();
                        if (amhsETA.length() > 0)
                            newInfo->amhsETA = amhsETA;
                    }
                    if ( item.isMember("ritETA") )
                    {
                        std::string ritETA = item["ritETA"].asString();
                        if (ritETA.length() > 0)
                            newInfo->ritETA = ritETA;
                    }
                    if ( item.isMember("ATADate") )
                    {
                        std::string ATADate = item["ATADate"].asString();
                        if (ATADate.length() > 0)
                            newInfo->ATADate = ATADate;
                    }
                    if ( item.isMember("ATA") )
                    {
                        std::string ATA = item["ATA"].asString();
                        if (ATA.length() > 0)
                            newInfo->ATA = ATA;
                    }
                    if ( item.isMember("airLineCode") )
                    {
                        std::string airLineCode = item["airLineCode"].asString();
                        if (airLineCode.length() > 0)
                            newInfo->airLineCode = airLineCode;
                    }
                    if ( item.isMember("airLineNum") )
                    {
                        std::string airLineNum = item["airLineNum"].asString();
                        if (airLineNum.length() > 0)
                            newInfo->airLineNum = airLineNum;
                    }
                    if ( item.isMember("DepartureAirport") )
                    {
                        std::string DepartureAirport = item["DepartureAirport"].asString();
                        if (DepartureAirport.length() > 0)
                            newInfo->DepartureAirport = DepartureAirport;
                    }
                    if ( item.isMember("airPlaneType") )
                    {
                        std::string airPlaneType = item["airPlaneType"].asString();
                        if (airPlaneType.length() > 0)
                            newInfo->airPlaneType = airPlaneType;
                    }
                    if ( item.isMember("REG_NO") )
                    {
                        std::string REG_NO = item["REG_NO"].asString();
                        if (REG_NO.length() > 0)
                            newInfo->REG_NO = REG_NO;
                    }
                    if ( item.isMember("APPLY_TYPE") )
                    {
                        std::string APPLY_TYPE = item["APPLY_TYPE"].asString();
                        if (APPLY_TYPE.length() > 0)
                            newInfo->APPLY_TYPE = APPLY_TYPE;
                    }

                    putToFlyInfoArrivalList(newInfo);

                    delete newInfo;
                }
            }
        }
    }
    catch(exception exc)
    {
        vilslog::printf ("SensorInfoService::updateFlyInfoArr() reader->parse() catch exception[%s]\n", exc.what());
    }

    delete reader;
}

void SensorInfoService::updateFlyInfoDep(std::string content)
{
    Json::Reader * reader = new Json::Reader();
    Json::Value JSONresult;
    bool parsingSuccessful = false;

    //vilslog::printf ("SensorInfoService::updateFlyInfoDep() \n");

    std::string pureCont = remove_nonJSON(content.c_str());

    try{
        parsingSuccessful = reader->parse( pureCont, JSONresult);

        if (parsingSuccessful)
        {
            if ( JSONresult.isArray() )
            {
                // vilslog::printf ("SensorInfoService::updateFlyInfoDep() JSONresult.size()[%d]\n", (int)JSONresult.size());

                for(unsigned int i=0;i<JSONresult.size();i++)
                {
                    Json::Value tmp;
                    Json::Value item = JSONresult.get((Json::UInt)i, tmp);

                    FlyInfoDepartureStruct * newInfo = new FlyInfoDepartureStruct();

                    if ( item.isMember("FDATE") )
                    {
                        std::string FDATE = item["FDATE"].asString();
                        if (FDATE.length() > 0)
                            newInfo->FDATE = FDATE;
                    }
                    if ( item.isMember("STD") )
                    {
                        std::string STD = item["STD"].asString();
                        if (STD.length() > 0)
                            newInfo->STD = STD;
                    }
                    if ( item.isMember("amhsETD") )
                    {
                        std::string amhsETD = item["amhsETD"].asString();
                        if (amhsETD.length() > 0)
                            newInfo->amhsETD = amhsETD;
                    }
                    if ( item.isMember("ritETD") )
                    {
                        std::string ritETD = item["ritETD"].asString();
                        if (ritETD.length() > 0)
                            newInfo->ritETD = ritETD;
                    }
                    if ( item.isMember("ATDDate") )
                    {
                        std::string ATDDate = item["ATDDate"].asString();
                        if (ATDDate.length() > 0)
                            newInfo->ATDDate = ATDDate;
                    }
                    if ( item.isMember("ATD") )
                    {
                        std::string ATD = item["ATD"].asString();
                        if (ATD.length() > 0)
                            newInfo->ATD = ATD;
                    }
                    if ( item.isMember("airLineCode") )
                    {
                        std::string airLineCode = item["airLineCode"].asString();
                        if (airLineCode.length() > 0)
                            newInfo->airLineCode = airLineCode;
                    }
                    if ( item.isMember("airLineNum") )
                    {
                        std::string airLineNum = item["airLineNum"].asString();
                        if (airLineNum.length() > 0)
                            newInfo->airLineNum = airLineNum;
                    }
                    if ( item.isMember("ArrivalAirport") )
                    {
                        std::string ArrivalAirport = item["ArrivalAirport"].asString();
                        if (ArrivalAirport.length() > 0)
                            newInfo->ArrivalAirport = ArrivalAirport;
                    }
                    if ( item.isMember("airPlaneType") )
                    {
                        std::string airPlaneType = item["airPlaneType"].asString();
                        if (airPlaneType.length() > 0)
                            newInfo->airPlaneType = airPlaneType;
                    }
                    if ( item.isMember("REG_NO") )
                    {
                        std::string REG_NO = item["REG_NO"].asString();
                        if (REG_NO.length() > 0)
                            newInfo->REG_NO = REG_NO;
                    }
                    if ( item.isMember("APPLY_TYPE") )
                    {
                        std::string APPLY_TYPE = item["APPLY_TYPE"].asString();
                        if (APPLY_TYPE.length() > 0)
                            newInfo->APPLY_TYPE = APPLY_TYPE;
                    }

                    putToFlyInfoDepartureList(newInfo);

                    delete newInfo;
                }
            }
        }
    }
    catch(exception exc)
    {
        vilslog::printf ("SensorInfoService::updateFlyInfoDep() reader->parse() catch exception[%s]\n", exc.what());
    }

    delete reader;
}

void SensorInfoService::updateFlySchedule()
{
    CURL *curl;
  	CURLcode res;
    std::string readBufferArr;
    std::string readBufferDep;

	curl = curl_easy_init();
    if(curl)
	{
        // First set the URL that is about to receive our POST. This URL can
	    // just as well be a https:// URL if that is what should receive the
	    // data.
	    // curl_easy_setopt(curl, CURLOPT_URL, "https://www.kia.gov.tw/API/InstantSchedule.ashx?AirFlyLine=2&AirFlyIO=2&airLineCode=MDA");
        curl_easy_setopt(curl, CURLOPT_URL, "https://www.kia.gov.tw/API/InstantSchedule.ashx?AirFlyLine=2&AirFlyIO=2");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBufferArr);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);

	    // Perform the request, res will get the return code
	    res = curl_easy_perform(curl);

        // curl_easy_setopt(curl, CURLOPT_URL, "https://www.kia.gov.tw/API/InstantSchedule.ashx?AirFlyLine=2&AirFlyIO=1&airLineCode=MDA");
        curl_easy_setopt(curl, CURLOPT_URL, "https://www.kia.gov.tw/API/InstantSchedule.ashx?AirFlyLine=2&AirFlyIO=1");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBufferDep);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);

	    // Perform the request, res will get the return code
	    res = curl_easy_perform(curl);


	    // always cleanup
	    curl_easy_cleanup(curl);

        // vilslog::printf("SensorInfoService::updateFlySchedule() readBufferArr[%s]\n", readBufferArr.c_str());
        // vilslog::printf("SensorInfoService::updateFlySchedule() readBufferDep[%s]\n", readBufferDep.c_str());

        updateFlyScheduleArr(readBufferArr);
        updateFlyScheduleDep(readBufferDep);

    }
}

void SensorInfoService::updateFlyScheduleArr(std::string content)
{
    Json::Reader * reader = new Json::Reader();
    Json::Value JSONresult;
    bool parsingSuccessful = false;

    try{
        parsingSuccessful = reader->parse( content, JSONresult);

        if (parsingSuccessful)
        {
            if ( JSONresult.isMember("InstantSchedule") )
            {
                if ( JSONresult["InstantSchedule"].isArray() )
                {
                    for(unsigned int i=0;i<JSONresult["InstantSchedule"].size();i++)
                    {
                        Json::Value tmp;
                        Json::Value item = JSONresult["InstantSchedule"].get((Json::UInt)i, tmp);

                        FlyScheduleArrivalStruct * newInfo = new FlyScheduleArrivalStruct();

                        if ( item.isMember("expectTime") )
                            newInfo->expectTime = item["expectTime"].asString();
                        if ( item.isMember("realTime") )
                            newInfo->realTime = item["realTime"].asString();
                        if ( item.isMember("airLineName") )
                            newInfo->airLineName = item["airLineName"].asString();
                        if ( item.isMember("airLineCode") )
                            newInfo->airLineCode = item["airLineCode"].asString();
                        if ( item.isMember("airLineLogo") )
                            newInfo->airLineLogo = item["airLineLogo"].asString();
                        if ( item.isMember("airLineUrl") )
                            newInfo->airLineUrl = item["airLineUrl"].asString();
                        if ( item.isMember("airLineNum") )
                            newInfo->airLineNum = item["airLineNum"].asString();
                        if ( item.isMember("upAirportCode") )
                            newInfo->upAirportCode = item["upAirportCode"].asString();
                        if ( item.isMember("upAirportName") )
                            newInfo->upAirportName = item["upAirportName"].asString();
                        if ( item.isMember("airPlaneType") )
                            newInfo->airPlaneType = item["airPlaneType"].asString();
                        if ( item.isMember("airBoardingGate") )
                            newInfo->airBoardingGate = item["airBoardingGate"].asString();
                        if ( item.isMember("airFlyStatus") )
                            newInfo->airFlyStatus = item["airFlyStatus"].asString();
                        if ( item.isMember("airFlyDelayCause") )
                            newInfo->airFlyDelayCause = item["airFlyDelayCause"].asString();

                        // vilslog::printf ("SensorInfoService::updateFlyScheduleArr() airLineNum[%s]\n", newInfo->airLineNum.c_str());

                        putToFlyScheduleArrivalList(newInfo);

                        delete newInfo;
                    }
                }
            }
        }
    }
    catch(exception exc)
    {
        vilslog::printf ("SensorInfoService::updateFlyScheduleArr() reader->parse() catch exception[%s]\n", exc.what());
    }

    delete reader;
}

void SensorInfoService::updateFlyScheduleDep(std::string content)
{
    Json::Reader * reader = new Json::Reader();
    Json::Value JSONresult;
    bool parsingSuccessful = false;

    try{
        parsingSuccessful = reader->parse( content, JSONresult);

        if (parsingSuccessful)
        {
            if ( JSONresult.isMember("InstantSchedule") )
            {
                if ( JSONresult["InstantSchedule"].isArray() )
                {
                    for(unsigned int i=0;i<JSONresult["InstantSchedule"].size();i++)
                    {
                        Json::Value tmp;
                        Json::Value item = JSONresult["InstantSchedule"].get((Json::UInt)i, tmp);

                        FlyScheduleDepartureStruct * newInfo = new FlyScheduleDepartureStruct();

                        if ( item.isMember("expectTime") )
                            newInfo->expectTime = item["expectTime"].asString();
                        if ( item.isMember("realTime") )
                            newInfo->realTime = item["realTime"].asString();
                        if ( item.isMember("airLineName") )
                            newInfo->airLineName = item["airLineName"].asString();
                        if ( item.isMember("airLineCode") )
                            newInfo->airLineCode = item["airLineCode"].asString();
                        if ( item.isMember("airLineLogo") )
                            newInfo->airLineLogo = item["airLineLogo"].asString();
                        if ( item.isMember("airLineUrl") )
                            newInfo->airLineUrl = item["airLineUrl"].asString();
                        if ( item.isMember("airLineNum") )
                            newInfo->airLineNum = item["airLineNum"].asString();
                        if ( item.isMember("goalAirportCode") )
                            newInfo->goalAirportCode = item["goalAirportCode"].asString();
                        if ( item.isMember("goalAirportName") )
                            newInfo->goalAirportName = item["goalAirportName"].asString();
                        if ( item.isMember("airPlaneType") )
                            newInfo->airPlaneType = item["airPlaneType"].asString();
                        if ( item.isMember("airBoardingGate") )
                            newInfo->airBoardingGate = item["airBoardingGate"].asString();
                        if ( item.isMember("airFlyStatus") )
                            newInfo->airFlyStatus = item["airFlyStatus"].asString();
                        if ( item.isMember("airFlyDelayCause") )
                            newInfo->airFlyDelayCause = item["airFlyDelayCause"].asString();

                        // vilslog::printf ("SensorInfoService::updateFlyScheduleDep() airLineNum[%s]\n", newInfo->airLineNum.c_str());

                        putToFlyScheduleDepartureList(newInfo);

                        delete newInfo;
                    }
                }
            }
        }
    }
    catch(exception exc)
    {
        vilslog::printf ("SensorInfoService::updateFlyScheduleDep() reader->parse() catch exception[%s]\n", exc.what());
    }

    delete reader;
}

void SensorInfoService::putToFlyInfoDepartureList(FlyInfoDepartureStruct * newInfo)
{
    bool bFound = false;
    for (std::list<FlyInfoDepartureStruct*>::iterator info = mFlyInfoDepartureList->begin(); info != mFlyInfoDepartureList->end(); info++)
    {
        if ((*info)->FDATE.compare(newInfo->FDATE) == 0 &&
            (*info)->STD.compare(newInfo->STD) == 0 &&
            (*info)->airLineCode.compare(newInfo->airLineCode) == 0 &&
            (*info)->airLineNum.compare(newInfo->airLineNum) == 0 )
        {
            (*info)->FDATE = newInfo->FDATE;
            (*info)->STD = newInfo->STD;
            (*info)->amhsETD = newInfo->amhsETD;
            (*info)->ritETD = newInfo->ritETD;
            (*info)->ATDDate = newInfo->ATDDate;
            (*info)->ATD = newInfo->ATD;
            (*info)->airLineCode = newInfo->airLineCode;
            (*info)->airLineNum = newInfo->airLineNum;
            (*info)->ArrivalAirport = newInfo->ArrivalAirport;
            (*info)->airPlaneType = newInfo->airPlaneType;
            (*info)->REG_NO = newInfo->REG_NO;
            (*info)->APPLY_TYPE = newInfo->APPLY_TYPE;

            if ( newInfo->airBoardingGate.length() > 0 )
            {
                (*info)->airBoardingGate = newInfo->airBoardingGate;
            }

            putFlyInfoToDB((*info), std::string(""), std::string(""));

            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        FlyInfoDepartureStruct * info = new FlyInfoDepartureStruct();
        info->FDATE = newInfo->FDATE;
        info->STD = newInfo->STD;
        info->amhsETD = newInfo->amhsETD;
        info->ritETD = newInfo->ritETD;
        info->ATDDate = newInfo->ATDDate;
        info->ATD = newInfo->ATD;
        info->airLineCode = newInfo->airLineCode;
        info->airLineNum = newInfo->airLineNum;
        info->ArrivalAirport = newInfo->ArrivalAirport;
        info->airPlaneType = newInfo->airPlaneType;
        info->REG_NO = newInfo->REG_NO;
        info->APPLY_TYPE = newInfo->APPLY_TYPE;
        info->send = 0;
        info->airBoardingGate = newInfo->airBoardingGate;

        // vilslog::printf ("SensorInfoService::putToFlyInfoDepartureList() FlyInfoDepartureStruct \n");
        // vilslog::printf ("FDATE[%s] STD[%s] amhsETD[%s] ritETD[%s] ATDDate[%s] ATD[%s]\n",
        //     info->FDATE.c_str(), info->STD.c_str(), info->amhsETD.c_str(), info->ritETD.c_str(), info->ATDDate.c_str(), info->ATD.c_str());
        // vilslog::printf ("airLineCode[%s] airLineNum[%s] ArrivalAirport[%s] airPlaneType[%s] REG_NO[%s] APPLY_TYPE[%s]\n",
        //     info->airLineCode.c_str(), info->airLineNum.c_str(), info->ArrivalAirport.c_str(), info->airPlaneType.c_str(), info->REG_NO.c_str(), info->APPLY_TYPE.c_str());

        mFlyInfoDepartureList->push_back(info);

        putFlyInfoToDB(info, std::string(""), std::string(""));

    }
}

void SensorInfoService::putToFlyInfoArrivalList(FlyInfoArrivalStruct * newInfo)
{
    bool bFound = false;
    for (std::list<FlyInfoArrivalStruct*>::iterator info = mFlyInfoArrivalList->begin(); info != mFlyInfoArrivalList->end(); info++)
    {
        if ((*info)->FDATE.compare(newInfo->FDATE) == 0 &&
            (*info)->STA.compare(newInfo->STA) == 0 &&
            (*info)->airLineCode.compare(newInfo->airLineCode) == 0 &&
            (*info)->airLineNum.compare(newInfo->airLineNum) == 0 )
        {
            (*info)->FDATE = newInfo->FDATE;
            (*info)->STA = newInfo->STA;
            (*info)->amhsETA = newInfo->amhsETA;
            (*info)->ritETA = newInfo->ritETA;
            (*info)->ATADate = newInfo->ATADate;
            (*info)->ATA = newInfo->ATA;
            (*info)->airLineCode = newInfo->airLineCode;
            (*info)->airLineNum = newInfo->airLineNum;
            (*info)->DepartureAirport = newInfo->DepartureAirport;
            (*info)->airPlaneType = newInfo->airPlaneType;
            (*info)->REG_NO = newInfo->REG_NO;
            (*info)->APPLY_TYPE = newInfo->APPLY_TYPE;

            if ( newInfo->airBoardingGate.length() > 0 )
            {
                (*info)->airBoardingGate = newInfo->airBoardingGate;
            }

            putFlyInfoToDB((*info), std::string(""), std::string(""));

            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        FlyInfoArrivalStruct * info = new FlyInfoArrivalStruct();
        info->FDATE = newInfo->FDATE;
        info->STA = newInfo->STA;
        info->amhsETA = newInfo->amhsETA;
        info->ritETA = newInfo->ritETA;
        info->ATADate = newInfo->ATADate;
        info->ATA = newInfo->ATA;
        info->airLineCode = newInfo->airLineCode;
        info->airLineNum = newInfo->airLineNum;
        info->DepartureAirport = newInfo->DepartureAirport;
        info->airPlaneType = newInfo->airPlaneType;
        info->REG_NO = newInfo->REG_NO;
        info->APPLY_TYPE = newInfo->APPLY_TYPE;
        info->send = 0;
        info->airBoardingGate = newInfo->airBoardingGate;

        // vilslog::printf ("SensorInfoService::putToFlyInfoArrivalList() FlyInfoArrivalStruct \n");
        // vilslog::printf ("FDATE[%s] STA[%s] amhsETA[%s] ritETA[%s] ATADate[%s] ATA[%s]\n",
        //     info->FDATE.c_str(), info->STA.c_str(), info->amhsETA.c_str(), info->ritETA.c_str(), info->ATADate.c_str(), info->ATA.c_str());
        // vilslog::printf ("airLineCode[%s] airLineNum[%s] DepartureAirport[%s] airPlaneType[%s] REG_NO[%s] APPLY_TYPE[%s]\n",
        //     info->airLineCode.c_str(), info->airLineNum.c_str(), info->DepartureAirport.c_str(), info->airPlaneType.c_str(), info->REG_NO.c_str(), info->APPLY_TYPE.c_str());

        mFlyInfoArrivalList->push_back(info);

        putFlyInfoToDB(info, std::string(""), std::string(""));

    }
}

void SensorInfoService::putToFlyScheduleDepartureList(FlyScheduleDepartureStruct * newInfo)
{
    std::string day = TodayDateType1();

    bool bFound = false;
    for (std::list<FlyScheduleDepartureStruct*>::iterator info = mFlyScheduleDepartureList->begin(); info != mFlyScheduleDepartureList->end(); info++)
    {
        if ((*info)->expectTime.compare(newInfo->expectTime) == 0 &&
            (*info)->airLineCode.compare(newInfo->airLineCode) == 0 &&
            (*info)->airLineNum.compare(newInfo->airLineNum) == 0 )
        {
            (*info)->FDATE = day;
            (*info)->expectTime = newInfo->expectTime;
            (*info)->realTime = newInfo->realTime;
            (*info)->airLineName = newInfo->airLineName;
            (*info)->airLineCode = newInfo->airLineCode;
            (*info)->airLineLogo = newInfo->airLineLogo;
            (*info)->airLineUrl = newInfo->airLineUrl;
            (*info)->airLineNum = newInfo->airLineNum;
            (*info)->goalAirportCode = newInfo->goalAirportCode;
            (*info)->goalAirportName = newInfo->goalAirportName;
            (*info)->airPlaneType = newInfo->airPlaneType;

            if ((*info)->airBoardingGate.compare(newInfo->airBoardingGate) != 0)
            {
                vilslog::printf ("SensorInfoService::putToFlyScheduleDepartureList() airLineCode[%s] airLineNum[%s] airBoardingGate[%s] -> [%s]\n",
                    (*info)->airLineCode.c_str(), (*info)->airLineNum.c_str(), (*info)->airBoardingGate.c_str(), newInfo->airBoardingGate.c_str());
            }

            if ((*info)->airFlyStatus.compare(newInfo->airFlyStatus) != 0)
            {
                vilslog::printf ("SensorInfoService::putToFlyScheduleDepartureList() airLineCode[%s] airLineNum[%s] airFlyStatus[%s] -> [%s]\n",
                    (*info)->airLineCode.c_str(), (*info)->airLineNum.c_str(), (*info)->airFlyStatus.c_str(), newInfo->airFlyStatus.c_str());
            }

            (*info)->airBoardingGate = newInfo->airBoardingGate;
            (*info)->airFlyStatus = newInfo->airFlyStatus;
            (*info)->airFlyDelayCause = newInfo->airFlyDelayCause;

            if ( (*info)->airBoardingGate.length() > 0 )
            {
                updateDepartureBoardingGate((*info));
            }
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        FlyScheduleDepartureStruct * info = new FlyScheduleDepartureStruct();
        info->FDATE = day;
        info->expectTime = newInfo->expectTime;
        info->realTime = newInfo->realTime;
        info->airLineName = newInfo->airLineName;
        info->airLineCode = newInfo->airLineCode;
        info->airLineLogo = newInfo->airLineLogo;
        info->airLineUrl = newInfo->airLineUrl;
        info->airLineNum = newInfo->airLineNum;
        info->goalAirportCode = newInfo->goalAirportCode;
        info->goalAirportName = newInfo->goalAirportName;
        info->airPlaneType = newInfo->airPlaneType;
        info->airBoardingGate = newInfo->airBoardingGate;
        info->airFlyStatus = newInfo->airFlyStatus;
        info->airFlyDelayCause = newInfo->airFlyDelayCause;

        if ( info->airBoardingGate.length() > 0 )
        {
            updateDepartureBoardingGate(info);
        }

        mFlyScheduleDepartureList->push_back(info);
    }
}

void SensorInfoService::putToFlyScheduleArrivalList(FlyScheduleArrivalStruct * newInfo)
{
    std::string day = TodayDateType1();

    bool bFound = false;
    for (std::list<FlyScheduleArrivalStruct*>::iterator info = mFlyScheduleArrivalList->begin(); info != mFlyScheduleArrivalList->end(); info++)
    {
        if ((*info)->expectTime.compare(newInfo->expectTime) == 0 &&
            (*info)->airLineCode.compare(newInfo->airLineCode) == 0 &&
            (*info)->airLineNum.compare(newInfo->airLineNum) == 0 )
        {
            (*info)->FDATE = day;
            (*info)->expectTime = newInfo->expectTime;
            (*info)->realTime = newInfo->realTime;
            (*info)->airLineName = newInfo->airLineName;
            (*info)->airLineCode = newInfo->airLineCode;
            (*info)->airLineLogo = newInfo->airLineLogo;
            (*info)->airLineUrl = newInfo->airLineUrl;
            (*info)->airLineNum = newInfo->airLineNum;
            (*info)->upAirportCode = newInfo->upAirportCode;
            (*info)->upAirportName = newInfo->upAirportName;
            (*info)->airPlaneType = newInfo->airPlaneType;

            if ((*info)->airBoardingGate.compare(newInfo->airBoardingGate) != 0)
            {
                vilslog::printf ("SensorInfoService::putToFlyScheduleArrivalList() airLineCode[%s] airLineNum[%s] airBoardingGate[%s] -> [%s]\n",
                    (*info)->airLineCode.c_str(), (*info)->airLineNum.c_str(), (*info)->airBoardingGate.c_str(), newInfo->airBoardingGate.c_str());
            }

            if ((*info)->airFlyStatus.compare(newInfo->airFlyStatus) != 0)
            {
                vilslog::printf ("SensorInfoService::putToFlyScheduleArrivalList() airLineCode[%s] airLineNum[%s] airFlyStatus[%s] -> [%s]\n",
                    (*info)->airLineCode.c_str(), (*info)->airLineNum.c_str(), (*info)->airFlyStatus.c_str(), newInfo->airFlyStatus.c_str());
            }

            (*info)->airBoardingGate = newInfo->airBoardingGate;
            (*info)->airFlyStatus = newInfo->airFlyStatus;
            (*info)->airFlyDelayCause = newInfo->airFlyDelayCause;

            if ( (*info)->airBoardingGate.length() > 0 )
            {
                updateArrivalBoardingGate((*info));
            }

            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        FlyScheduleArrivalStruct * info = new FlyScheduleArrivalStruct();
        info->FDATE = day;
        info->expectTime = newInfo->expectTime;
        info->realTime = newInfo->realTime;
        info->airLineName = newInfo->airLineName;
        info->airLineCode = newInfo->airLineCode;
        info->airLineLogo = newInfo->airLineLogo;
        info->airLineUrl = newInfo->airLineUrl;
        info->airLineNum = newInfo->airLineNum;
        info->upAirportCode = newInfo->upAirportCode;
        info->upAirportName = newInfo->upAirportName;
        info->airPlaneType = newInfo->airPlaneType;
        info->airBoardingGate = newInfo->airBoardingGate;
        info->airFlyStatus = newInfo->airFlyStatus;
        info->airFlyDelayCause = newInfo->airFlyDelayCause;

        if ( info->airBoardingGate.length() > 0 )
        {
            updateArrivalBoardingGate(info);
        }

        mFlyScheduleArrivalList->push_back(info);
    }
}

std::string SensorInfoService::convert_to_AirLineName(std::string airLineCode)
{
    std::string AirLineName = airLineCode;

    if (airLineCode.compare("MDA") == 0)
    {
        AirLineName = "華信航空 班機";
    }

    return AirLineName;
}

std::string SensorInfoService::convert_to_fly_json_str(FlyInfoDepartureStruct * info, std::string triggerTime)
{
    std::ostringstream jsonstream;
    jsonstream<<"{";
    jsonstream<<"\"type\":\"Departure\",";
    jsonstream<<"\"FDATE\":\""<<info->FDATE<<"\",";
    jsonstream<<"\"STD\":\""<<info->STD<<"\",";
    jsonstream<<"\"amhsETD\":\""<<info->amhsETD<<"\",";
    jsonstream<<"\"ritETD\":\""<<info->ritETD<<"\",";
    jsonstream<<"\"ATD\":\""<<info->ATD<<"\",";
    jsonstream<<"\"airLineCode\":\""<<info->airLineCode<<"\",";
    jsonstream<<"\"airLineNum\":\""<<info->airLineNum<<"\",";
    jsonstream<<"\"ArrivalAirport\":\""<<info->ArrivalAirport<<"\",";
    if (triggerTime.length() > 0)
    {
        jsonstream<<"\"airPlaneType\":\""<<info->airPlaneType<<"\",";
        jsonstream<<"\"triggerTime\":\""<<triggerTime<<"\"";
    }
    else
    {
        jsonstream<<"\"airPlaneType\":\""<<info->airPlaneType<<"\"";
    }
    jsonstream<<"}";
    return jsonstream.str();
}

std::string SensorInfoService::convert_to_fly_json_str(FlyInfoArrivalStruct * info, std::string triggerTime)
{
    std::ostringstream jsonstream;
    jsonstream<<"{";
    jsonstream<<"\"type\":\"Arrival\",";
    jsonstream<<"\"FDATE\":\""<<info->FDATE<<"\",";
    jsonstream<<"\"STA\":\""<<info->STA<<"\",";
    jsonstream<<"\"amhsETA\":\""<<info->amhsETA<<"\",";
    jsonstream<<"\"ritETA\":\""<<info->ritETA<<"\",";
    jsonstream<<"\"ATA\":\""<<info->ATA<<"\",";
    jsonstream<<"\"airLineCode\":\""<<info->airLineCode<<"\",";
    jsonstream<<"\"airLineNum\":\""<<info->airLineNum<<"\",";
    jsonstream<<"\"DepartureAirport\":\""<<info->DepartureAirport<<"\",";
    if (triggerTime.length() > 0)
    {
        jsonstream<<"\"airPlaneType\":\""<<info->airPlaneType<<"\",";
        jsonstream<<"\"triggerTime\":\""<<triggerTime<<"\"";
    }
    else
    {
        jsonstream<<"\"airPlaneType\":\""<<info->airPlaneType<<"\"";
    }
    jsonstream<<"}";
    return jsonstream.str();
}

void SensorInfoService::putFlyInfoToDB(FlyInfoDepartureStruct * departureInfo, std::string triggerTime, std::string sensorid)
{
    int projectid = 1;
    int maingroupid = 1;

    FlyInfoStruct * newInfo = new FlyInfoStruct();
    newInfo->FDATE = departureInfo->FDATE;
    newInfo->expectTime = departureInfo->STD;
    newInfo->airLineCode = departureInfo->airLineCode;
    newInfo->airLineNum = departureInfo->airLineNum;
    newInfo->type = "Departure";
    newInfo->jsonvalue = convert_to_fly_json_str(departureInfo, triggerTime);
    newInfo->triggerTime = triggerTime;
    newInfo->maingroupid = maingroupid;
    newInfo->sensorid = sensorid;
    newInfo->airBoardingGate = departureInfo->airBoardingGate;

    // std::string pretty_json = JS::serializeStruct(*newInfo, JS::SerializerOptions(JS::SerializerOptions::Compact));
    // //std::string pretty_json = JS::serializeStruct(*newInfo);
    // vilslog::printf ("SensorInfoService::putFlyInfoToDB pretty_json[%s]\n", pretty_json.c_str());
    //
    // JS::ParseContext context(pretty_json);
    // FlyInfoStruct obj;
    // context.parseTo(obj);
    //
    // vilslog::printf ("SensorInfoService::putFlyInfoToDB airLineNum[%s]\n", obj.airLineNum.c_str());

    WorkItem * witem = new WorkItem("flyinfo", projectid, newInfo);
    mMySQLJobQueue.add(witem);
    delete newInfo;
}

void SensorInfoService::putFlyInfoToDB(FlyInfoArrivalStruct * arrivalInfo, std::string triggerTime, std::string sensorid)
{
    int projectid = 1;
    int maingroupid = 1;

    FlyInfoStruct * newInfo = new FlyInfoStruct();
    newInfo->FDATE = arrivalInfo->FDATE;
    newInfo->expectTime = arrivalInfo->STA;
    newInfo->airLineCode = arrivalInfo->airLineCode;
    newInfo->airLineNum = arrivalInfo->airLineNum;
    newInfo->type = "Arrival";
    newInfo->jsonvalue = convert_to_fly_json_str(arrivalInfo, triggerTime);
    newInfo->triggerTime = triggerTime;
    newInfo->maingroupid = maingroupid;
    newInfo->sensorid = sensorid;
    newInfo->airBoardingGate = arrivalInfo->airBoardingGate;

    WorkItem * witem = new WorkItem("flyinfo", projectid, newInfo);
    mMySQLJobQueue.add(witem);
    delete newInfo;
}

void SensorInfoService::updateDepartureBoardingGate(FlyScheduleDepartureStruct * newInfo)
{
    for (std::list<FlyInfoDepartureStruct*>::iterator info = mFlyInfoDepartureList->begin(); info != mFlyInfoDepartureList->end(); info++)
    {
        if ((*info)->FDATE.compare(newInfo->FDATE) == 0 &&
            (*info)->airLineCode.compare(newInfo->airLineCode) == 0 &&
            (*info)->airLineNum.compare(newInfo->airLineNum) == 0 )
        {
            if ((*info)->airBoardingGate.compare(newInfo->airBoardingGate) != 0)
            {
                vilslog::printf ("SensorInfoService::updateDepartureBoardingGate FDATE[%s] airLineCode[%s] airLineNum[%s] airBoardingGate[%s] -> [%s]\n",
                    (*info)->FDATE.c_str(), (*info)->airLineCode.c_str(), (*info)->airLineNum.c_str(), (*info)->airBoardingGate.c_str(), newInfo->airBoardingGate.c_str());
            }

            (*info)->airBoardingGate = newInfo->airBoardingGate;
            break;
        }
    }
}

void SensorInfoService::updateArrivalBoardingGate(FlyScheduleArrivalStruct * newInfo)
{
    for (std::list<FlyInfoArrivalStruct*>::iterator info = mFlyInfoArrivalList->begin(); info != mFlyInfoArrivalList->end(); info++)
    {
        if ((*info)->FDATE.compare(newInfo->FDATE) == 0 &&
            (*info)->airLineCode.compare(newInfo->airLineCode) == 0 &&
            (*info)->airLineNum.compare(newInfo->airLineNum) == 0 )
        {
            if ((*info)->airBoardingGate.compare(newInfo->airBoardingGate) != 0)
            {
                vilslog::printf ("SensorInfoService::updateArrivalBoardingGate FDATE[%s] airLineCode[%s] airLineNum[%s] airBoardingGate[%s] -> [%s]\n",
                    (*info)->FDATE.c_str(), (*info)->airLineCode.c_str(), (*info)->airLineNum.c_str(), (*info)->airBoardingGate.c_str(), newInfo->airBoardingGate.c_str());
            }

            (*info)->airBoardingGate = newInfo->airBoardingGate;
            break;
        }
    }
}

void SensorInfoService::updateArrivalBoardingGate(std::string FDATE, std::string STD, std::string airLineCode, std::string airLineNum, std::string airBoardingGate)
{
    for (std::list<FlyInfoArrivalStruct*>::iterator info = mFlyInfoArrivalList->begin(); info != mFlyInfoArrivalList->end(); info++)
    {
        if ((*info)->FDATE.compare(FDATE) == 0 &&
            (*info)->airLineCode.compare(airLineCode) == 0 &&
            (*info)->airLineNum.compare(airLineNum) == 0 )
        {
            if ((*info)->STA.compare(STD) < 0)
            {
                if ((*info)->airBoardingGate.compare(airBoardingGate) != 0)
                {
                    vilslog::printf ("SensorInfoService::updateArrivalBoardingGate          FDATE[%s] airLineCode[%s] airLineNum[%s] STD[%s]\n",
                        FDATE.c_str(), airLineCode.c_str(), airLineNum.c_str(), STD.c_str());
                    vilslog::printf ("SensorInfoService::updateArrivalBoardingGate (*info)->FDATE[%s] airLineCode[%s] airLineNum[%s] STA[%s] airBoardingGate[%s] -> [%s]\n",
                        (*info)->FDATE.c_str(), (*info)->airLineCode.c_str(), (*info)->airLineNum.c_str(), (*info)->STA.c_str(), (*info)->airBoardingGate.c_str(), airBoardingGate.c_str());
                }

                (*info)->airBoardingGate = airBoardingGate;
            }
            break;
        }
    }
}

void SensorInfoService::release_old_info()
{
    std::string day = TodayDateType1();

    for (std::list<FlyInfoDepartureStruct*>::iterator info = mFlyInfoDepartureList->begin(); info != mFlyInfoDepartureList->end(); info++)
    {
        if ((*info)->FDATE.compare(day) != 0)
        {
            FlyInfoDepartureStruct * tmp = (*info);
            info = mFlyInfoDepartureList->erase(info);
            delete tmp;
        }
    }
    for (std::list<FlyInfoArrivalStruct*>::iterator info = mFlyInfoArrivalList->begin(); info != mFlyInfoArrivalList->end(); info++)
    {
        if ((*info)->FDATE.compare(day) != 0)
        {
            FlyInfoArrivalStruct * tmp = (*info);
            info = mFlyInfoArrivalList->erase(info);
            delete tmp;
        }
    }
    for (std::list<FlyScheduleDepartureStruct*>::iterator info = mFlyScheduleDepartureList->begin(); info != mFlyScheduleDepartureList->end(); info++)
    {
        if ((*info)->FDATE.compare(day) != 0)
        {
            FlyScheduleDepartureStruct * tmp = (*info);
            info = mFlyScheduleDepartureList->erase(info);
            delete tmp;
        }
    }
    for (std::list<FlyScheduleArrivalStruct*>::iterator info = mFlyScheduleArrivalList->begin(); info != mFlyScheduleArrivalList->end(); info++)
    {
        if ((*info)->FDATE.compare(day) != 0)
        {
            FlyScheduleArrivalStruct * tmp = (*info);
            info = mFlyScheduleArrivalList->erase(info);
            delete tmp;
        }
    }
}

void SensorInfoService::reset_sensor_device()
{
    int projectid = 1;
    std::list<NodeStruct*> * locatorNodeList = mDeviceManager->get_Locator_list(projectid);

    std::string atcmd = "AT+CRST\r\n";

    // vilslog::printf("SensorInfoService::reset_sensor_device() atcmd[%s]", atcmd.c_str());

    for (std::list<NodeStruct*>::iterator existed = locatorNodeList->begin(); existed != locatorNodeList->end(); existed++)
    {
        // 0f30000000000032 LiDAR
        // vilslog::printf("SensorInfoService::reset_sensor_device() (*existed)->macaddress[%s]\n", (*existed)->macaddress);
        if ( strncmp((*existed)->macaddress, "0f30", 4) == 0 )
        {
            char macLastFour[5];
            memcpy(macLastFour, (*existed)->macaddress + strlen((*existed)->macaddress) - 4, 4);
            macLastFour[4] = '\0';

            vilslog::printf("SensorInfoService::reset_sensor_device() (*existed)->macaddress[%s]\n", (*existed)->macaddress);

            //
            // check LiDAR trigger
            //
            bool bValid = false;
            for (std::list<SensorEventStruct*>::iterator event = mSensorEventList->begin(); event != mSensorEventList->end(); event++)
            {
                if ((*event)->sensorname.length() > 0)
                {
                    std::string nameLastFour = (*event)->sensorname.substr((*event)->sensorname.length() - 4, 4);
                    // char nameLastFour[5];
                    // memcpy(nameLastFour, (*event)->sensorname + (*event)->sensorname) - 4, 4);
                    // nameLastFour[4] = '\0';

                    // vilslog::printf("SensorInfoService::reset_sensor_device() nameLastFour[%s]\n", nameLastFour.c_str());

                    if (nameLastFour.compare(macLastFour) == 0)
                    {
                        if ( (*event)->triggerCount == 0 )
                            bValid = true;
                        break;
                    }
                }
            }

            for(int i=0;i<RESET_SENSOR_COUNT && bValid;i++)
            {
                DataItem * senditem = new DataItem("sendATcmd", projectid, (*existed)->macaddress, NODE_TYPE_LOCATOR);
                senditem->setATcommand(atcmd);
                senditem->setWaitkey(random_string(16));
                senditem->setDelayTime((rand()%3) + 1);
                mMainJobQueue.add(senditem);
            }
        }
    }
}

float SensorInfoService::averageCalc(int * x, int len, bool skipOne)
{
    // vilslog::printf("SensorInfoService::averageCalc()");

    int validCnt = 0;
    float sum = 0;
    for (int i = 0; i < len; i++) // 求和
    {
        // vilslog::printf("%d, ", x[i]);
        if (skipOne && x[i] == 1)
        {
            continue;
        }

        sum += (float)x[i];
        validCnt++;
    }
    // vilslog::printf("\n");
    if (validCnt == 0)
        return 0.0f;
    return (sum/(float)validCnt); // 得到平均值
}

// 求方差
float SensorInfoService::varianceCalc(float average, int * x, int len, bool skipOne)
{
    // bool skipOne = false;
    // if ( average > THRESHOD_AVG )
    // {
    //     average = averageCalc(x, len, true);
    //     skipOne = true;
    // }

    int validCnt = 0;
    float sum = 0;
    for (int i = 0; i < len; i++) // 求和
    {
        if (skipOne)
        {
            // vilslog::printf("varianceCalc() skipOne average[%f] x[%d][%d]\n", average, i, x[i]);
            if (x[i] == 1)
            {
                continue;
            }
        }
        sum += pow((float)x[i] - average, 2);
        validCnt++;
    }
    // vilslog::printf("varianceCalc() average[%f] sum[%f] validCnt[%d]\n", average, sum, validCnt);
    if (validCnt == 0)
        return 0.0f;
    return (sum/(float)validCnt); // 得到平均值
}
