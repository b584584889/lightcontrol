/*
 *  CARDRollCallService.cpp
 *
 *  Created on: 2020/05/04
 *      Author: ikki
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <iostream>

#include <math.h>
#include <algorithm>    // std::max

#include <netdb.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/errno.h>

#include "CARDRollCallService.h"

using namespace std;

CARDRollCallService::CARDRollCallService(wqueue<NotificationItem*> & NotifiactionQueue, wqueue<DataItem*> & CHTIoTJobQueue, wqueue<MQTTDataItem*> & MQTTPublishQueue, wqueue<DataItem*> & CARDRollCallJobQueue, wqueue<WorkItem*> & MySQLJobQueue, DeviceManager * pDevManager, DBManager * dbManager)
    : mNotifiactionQueue(NotifiactionQueue), mCHTIoTJobQueue(CHTIoTJobQueue), mMQTTPublishQueue(MQTTPublishQueue), mCARDRollCallJobQueue(CARDRollCallJobQueue), mMySQLJobQueue(MySQLJobQueue), mDeviceManager(pDevManager), mDbManager(dbManager)
{
}

CARDRollCallService::~CARDRollCallService()
{
}

bool CARDRollCallService::status()
{
    return mInitial;
}

std::string CARDRollCallService::statusString()
{
    return mStatus;
}

void * CARDRollCallService::run()
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
            vilslog::printf("CARDRollCallService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "CARDRollCallService()::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void CARDRollCallService::runloop()
{
    long long start1 = getSystemTime();
    long long end1 = getSystemTime();
    long long difft = end1 - start1;
    long long CheclLLRPTime = 600000;
    long long RollCallTime = 0;
    long long reloadRollCallTime = 0;
    // long long waitAckTime = 0;

    mInitial = true;
    mRollCallRunning = false;
    // mWaitAck_StopScanRFIDCard = false;
    // mWaitAck_trycount = 0;

    mStatus = "CARDRollCallService()::run() Initial Courses successfully!";
    vilslog::printf ("CARDRollCallService()::run() Initial Courses successfully!\n");

    initProjectRollCallInfo();

    srand( time(NULL) );

    //printf("CARDRollCallService::runloop() mProjectRollCallInfoVector.size()[%d]\n", (int)mProjectRollCallInfoVector.size());

    // int projectid = 1;
    // int rollcallid = 3;
    // reCheckRollCall(projectid, std::string("2020-12-21"), rollcallid);

    while (1)
    {
        DataItem * item = mCARDRollCallJobQueue.removetimed(1000);
        if( item != NULL )
        {
            processDataItem(item);
            delete item;
        }

        end1 = getSystemTime();
        difft = end1 - start1;

        CheclLLRPTime += difft;
        RollCallTime += difft;
        reloadRollCallTime += difft;
        // waitAckTime += difft;

        if ( CheclLLRPTime >= 600000) // 10 minutes
        {
            check_LLRPInstance();
            CheclLLRPTime = 0;
        }
        if ( RollCallTime >= 5000) // 5 seconds
        {
            CheckProjectRollCallEvent();

            clear_RollCallCardStruct_list(&mTempRFIDCardList);
            mTempRFIDCardList.clear();

            clear_RollCallCardStruct_list(&mTempNFCCardList);
            mTempNFCCardList.clear();


            RollCallTime = 0;
        }

        if ( reloadRollCallTime >= 600000 ) // 600 seconds
        {
            if ( !mRollCallRunning )
            {
                reloadRollCallList();
            }
            reloadRollCallTime = 0;
        }

        start1 = end1;
    }
}

void CARDRollCallService::initProjectRollCallInfo()
{
    clear_ProjectRollCallInfoStruct_vector(&mProjectRollCallInfoVector);

    std::list<int> * prjidList = mDbManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);

        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if (baseDb == NULL)
            continue;

        ProjectRollCallInfoStruct * newInfo = new ProjectRollCallInfoStruct();
        newInfo->projectid = projectid;
        newInfo->mRollCallList = baseDb->read_CARDRollCallInfo();
        newInfo->mRFIDDeviceList = baseDb->get_RFIDDevices();
        newInfo->mNFCDeviceList = baseDb->get_NFCDevices();
        newInfo->mRollCallInstanceList = new std::list<RollCallInstance*>();
        newInfo->mTriggerList = create_Triggers();
        newInfo->mSection0DGroupList = new std::list<SectionGroup *>();
        baseDb->read_SectionGroupList(projectid, SECTION_TYPE_ZERO, newInfo->mSection0DGroupList);

        // vilslog::printf ("CARDRollCallService()::initProjectRollCallInfo() projectid[%d] mRollCallList->size()[%d]\n",
        //     projectid, (int)newInfo->mRollCallList->size());
        mProjectRollCallInfoVector.push_back(newInfo);
    }

    prjidList->clear();
    delete prjidList;
}

void CARDRollCallService::reCheckRollCall(int projectid, std::string day, int rollcallid)
{
    vilslog::printf ("CARDRollCallService()::reCheckRollCall() projectid[%d] day[%s] rollcallid[%d]\n", projectid, day.c_str(), rollcallid);

    for (std::vector<ProjectRollCallInfoStruct*>::iterator i = mProjectRollCallInfoVector.begin(); i != mProjectRollCallInfoVector.end(); i++)
    {
        for (std::list<CARDRollCallStruct*>::iterator rollcall = (*i)->mRollCallList->begin(); rollcall != (*i)->mRollCallList->end(); rollcall++)
        {
            if ( (*rollcall)->rollcallid == rollcallid )
            {
                MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
                if (baseDb == NULL)
                    break;

                vilslog::printf ("CARDRollCallService()::reCheckRollCall() projectid[%d] day[%s] rollcallid[%d] Found!\n", projectid, day.c_str(), rollcallid);

                RollCallInstance * rollcallInstance = new RollCallInstance();
                rollcallInstance->rollcallid = rollcallid;
                rollcallInstance->mRollCallEvent = NULL;
                rollcallInstance->mUserCardList = NULL;
                rollcallInstance->mRollCallRecordList = new std::list<CARDRollCallRecordStruct*>();
                rollcallInstance->mRollCallSubGroupList = NULL;
                rollcallInstance->mTrigger = NULL;

                time_t t_Start = StringToInspectionDatetime((*rollcall)->StartTime);
                time_t t_End = StringToInspectionDatetime((*rollcall)->EndTime);
                std::string StartTime = time_tToTimeString(t_Start);
                std::string EndTime = time_tToTimeString(t_End);

                std::string StartDateTime = day;
                StartDateTime.append(" ");
                StartDateTime.append(StartTime);

                std::string EndDateTime = day;
                EndDateTime.append(" ");
                EndDateTime.append(EndTime);

                rollcallInstance->mRollCallEvent = baseDb->get_CARDRollCallEvent((*rollcall)->rollcallid, StartDateTime, EndDateTime, (*rollcall)->maingroups, true);
                rollcallInstance->mRollCallEvent->rollcallname = (*rollcall)->rollcallname;
                rollcallInstance->mRollCallEvent->totalcount = 0;

                vilslog::printf ("CARDRollCallService()::reCheckRollCall() StartDateTime[%s] EndDateTime[%s] mRollCallEvent->eventid[%d]\n",
                    StartDateTime.c_str(), EndDateTime.c_str(), rollcallInstance->mRollCallEvent->eventid);

                rollcallInstance->mRollCallSubGroupList = new std::list<CARDRollCallSubGroupEventStruct*>();

                for (std::list<int>::iterator subgroup = (*rollcall)->subgroups.begin(); subgroup != (*rollcall)->subgroups.end(); subgroup++)
                {
                    int subgroupid = (*subgroup);

                    CARDRollCallSubGroupEventStruct * newEvent = new CARDRollCallSubGroupEventStruct();
                    newEvent->eventid = rollcallInstance->mRollCallEvent->eventid;
                    newEvent->rollcallid = rollcallInstance->mRollCallEvent->rollcallid;
                    newEvent->subgroupid = subgroupid;
                    newEvent->rollcallname = rollcallInstance->mRollCallEvent->rollcallname;
                    newEvent->StartDateTime = rollcallInstance->mRollCallEvent->StartDateTime;
                    newEvent->EndDateTime = rollcallInstance->mRollCallEvent->EndDateTime;
                    newEvent->totalcount = 0;
                    newEvent->presentcount = 0;
                    newEvent->maingroups = (*rollcall)->maingroups;

                    rollcallInstance->mRollCallSubGroupList->push_back(newEvent);
                }

                check_RollCallEvent_UserCardList((*i), (*rollcall), rollcallInstance);

                vilslog::printf ("CARDRollCallService()::reCheckRollCall() mRollCallEvent->eventid[%d] totalcount[%d] presentcount[%d]\n",
                    rollcallInstance->mRollCallEvent->eventid, rollcallInstance->mRollCallEvent->totalcount, rollcallInstance->mRollCallEvent->presentcount);

                rollcallInstance->mRollCallEvent->totalcount = (int)rollcallInstance->mUserCardList->size();

                //
                // get data from db
                //
                std::list<CARDScanRecordStruct*> * cardScanRecordList = baseDb->get_CARDScanRecord(StartDateTime, EndDateTime);

                vilslog::printf ("CARDRollCallService()::reCheckRollCall() cardScanRecordList->size()[%d]\n", (int)cardScanRecordList->size());

                for (std::list<CARDScanRecordStruct*>::iterator record = cardScanRecordList->begin(); record != cardScanRecordList->end(); record++)
                {
                    if ((*record)->cardtype == 0) // NFC
                    {
                        put_to_TempNFCCardlist((*record)->cardid, (*record)->wifimac, (*record)->LoRAMacaddress, (*record)->datetime);
                    }
                    else
                    {
                        put_to_TempRFIDCardlist((*record)->cardid, (*record)->wifimac, (*record)->LoRAMacaddress, (*record)->datetime);
                    }
                }

                vilslog::printf ("CARDRollCallService()::reCheckRollCall() mTempRFIDCardList.size()[%d]\n", (int)mTempRFIDCardList.size());
                vilslog::printf ("CARDRollCallService()::reCheckRollCall() mTempNFCCardList.size()[%d]\n", (int)mTempNFCCardList.size());

                check_RFID_Card_Present((*i), rollcallInstance, (*rollcall));
                check_NFC_Card_Present((*i), rollcallInstance, (*rollcall));

                //
                // update 實到 人數
                //
                if ( rollcallInstance->mUserCardList != NULL )
                {
                    int present_count = 0;
                    for (std::list<UserCardStruct*>::iterator UserCard = rollcallInstance->mUserCardList->begin(); UserCard != rollcallInstance->mUserCardList->end(); UserCard++)
                    {
                        if ((*UserCard)->present == 1)
                        {
                            present_count++;

                            //
                            // set subgroup event
                            //
                            set_present_subgroupevent((*UserCard)->userid, rollcallInstance->mRollCallSubGroupList);
                        }
                    }
                    if ( rollcallInstance->mRollCallEvent != NULL )
                    {
                        rollcallInstance->mRollCallEvent->totalcount = (int)rollcallInstance->mUserCardList->size();
                        rollcallInstance->mRollCallEvent->presentcount = present_count;

                        WorkItem * work = new WorkItem("CARDRollCallEventStruct", (*i)->projectid, rollcallInstance->mRollCallEvent);
                        mMySQLJobQueue.add(work);
                    }

                    for (std::list<CARDRollCallSubGroupEventStruct *>::iterator subgroupevent = rollcallInstance->mRollCallSubGroupList->begin(); subgroupevent != rollcallInstance->mRollCallSubGroupList->end(); subgroupevent++)
                    {
                        (*subgroupevent)->totalcount = (int)(*subgroupevent)->userids.size();
                        (*subgroupevent)->presentcount = (int)(*subgroupevent)->presentuserids.size();

                        // vilslog::printf("RollCallEvent_intime() eventid[%d] rollcallid[%d] subgroupid[%d] totalcount[%d] presentcount[%d]\n",
                        //     (*subgroupevent)->eventid, (*subgroupevent)->rollcallid, (*subgroupevent)->subgroupid, (*subgroupevent)->totalcount, (*subgroupevent)->presentcount);

                        WorkItem * work = new WorkItem("CARDRollCallSubGroupEventStruct", (*i)->projectid, (*subgroupevent));
                        mMySQLJobQueue.add(work);
                    }
                }

                cardScanRecordList->clear();
                delete cardScanRecordList;

                if ( rollcallInstance->mRollCallEvent != NULL )
                {
                    vilslog::printf("CARDRollCallService()::reCheckRollCall() rollcallid[%d] rollcallname[%s] \n",
                        (*rollcall)->rollcallid, (*rollcall)->rollcallname.c_str());

                    // release old one
                    delete rollcallInstance->mRollCallEvent;
                    rollcallInstance->mRollCallEvent = NULL;
                }

                if ( rollcallInstance->mUserCardList != NULL )
                {
                    // release old one
                    clear_UserCardStruct_list(rollcallInstance->mUserCardList);
                    delete rollcallInstance->mUserCardList;
                    rollcallInstance->mUserCardList = NULL;
                }

                if (rollcallInstance->mRollCallRecordList != NULL)
                {
                    for (std::list<CARDRollCallRecordStruct*>::iterator record = rollcallInstance->mRollCallRecordList->begin(); record != rollcallInstance->mRollCallRecordList->end(); record++)
                    {
                        (*record)->rfiddeviceids.clear();
                        (*record)->nfclocators.clear();
                        delete (*record);
                    }
                    rollcallInstance->mRollCallRecordList->clear();
                    delete rollcallInstance->mRollCallRecordList;
                }

                if (rollcallInstance->mRollCallSubGroupList != NULL)
                {
                    clear_CARDRollCallSubGroupEventStruct_list(rollcallInstance->mRollCallSubGroupList);
                    rollcallInstance->mRollCallSubGroupList->clear();
                    delete rollcallInstance->mRollCallSubGroupList;
                }
            }
        }
    }
}

int CARDRollCallService::processDataItem(DataItem * dataItem)
{
    if ( strcmp(dataItem->getType(), "updateCARDRollCall") == 0 )
    {
        vilslog::printf("CARDRollCallService::processDataItem() updateCARDRollCall\n");
        reloadRollCallList();
    }
    else
    if ( strcmp(dataItem->getType(), "updaterollcallrecord") == 0 )
    {
        vilslog::printf("CARDRollCallService::processDataItem() updaterollcallrecord\n");
        int projectid = dataItem->getProjectID();
        updateRollCallRecord(projectid, &dataItem->mCARDRollCallRecord);
    }
    else
    if ( strcmp(dataItem->getType(), "RFIDCard") == 0 )
    {
        int projectid = dataItem->getProjectID();
        std::string cardid = dataItem->getCardid();
        std::string gwid = dataItem->getGatewayid();
        std::string loramacaddress = dataItem->getLoRAMacaddress();

        // vilslog::printf("CARDRollCallService::processDataItem() RFIDCard cardid[%s] gwid[%s] loramacaddress[%s]\n",
        //     cardid.c_str(), gwid.c_str(), loramacaddress.c_str());

        if ( (cardid.length() >= 16) && (cardid.length() <= 32) )
        {
            put_to_TempRFIDCardlist(cardid, gwid, loramacaddress, currentDateTimeForSQL());

            do_RFIDCard_section_0D_check(projectid, cardid, loramacaddress);
        }
    }
    else
    if ( strcmp(dataItem->getType(), "NFCCard") == 0 )
    {
        int projectid = dataItem->getProjectID();
        std::string cardid = dataItem->getCardid();
        std::string gwid = dataItem->getGatewayid();
        std::string loramacaddress = dataItem->getLoRAMacaddress();

        // vilslog::printf("CARDRollCallService::processDataItem() NFCCard cardid[%s] gwid[%s] loramacaddress[%s]\n",
        //     cardid.c_str(), gwid.c_str(), loramacaddress.c_str());

        if ( (cardid.length() >= 6) && (cardid.length() <= 16) )
        {
            put_to_TempNFCCardlist(cardid, gwid, loramacaddress, currentDateTimeForSQL());

            do_NFCCard_section_0D_check(projectid, cardid, loramacaddress);
        }
    }
    else
    if ( strcmp(dataItem->getType(), "ackStartScanRFIDCard") == 0 )
    {
        std::string loramacaddress = dataItem->getLoRAMacaddress();
        vilslog::printf("CARDRollCallService::processDataItem() ackStartScanRFIDCard loramacaddress[%s]\n", loramacaddress.c_str());
    }
    else
    if ( strcmp(dataItem->getType(), "ackStopScanRFIDCard") == 0 )
    {
        std::string loramacaddress = dataItem->getLoRAMacaddress();
        vilslog::printf("CARDRollCallService::processDataItem() ackStopScanRFIDCard loramacaddress[%s]\n", loramacaddress.c_str());
        // mWaitAck_StopScanRFIDCard = false;
    }
    else
    if ( strcmp(dataItem->getType(), "ackReportScanRFIDCard") == 0 )
    {
        std::string loramacaddress = dataItem->getLoRAMacaddress();
        vilslog::printf("CARDRollCallService::processDataItem() ackReportScanRFIDCard loramacaddress[%s]\n", loramacaddress.c_str());
    }
    else
    if ( strcmp(dataItem->getType(), "updatesection") == 0 )
    {
        vilslog::printf ("CARDRollCallService()::waitloop() updatesection\n");
        reloadSectionList();
    }
    return 0;
}

void CARDRollCallService::check_LLRPInstance()
{
    for (std::vector<ProjectRollCallInfoStruct*>::iterator i = mProjectRollCallInfoVector.begin(); i != mProjectRollCallInfoVector.end(); i++)
    {
        for (std::list<RFIDDeviceStruct*>::iterator device = (*i)->mRFIDDeviceList->begin(); device != (*i)->mRFIDDeviceList->end(); device++)
        {
            RFIDDeviceStruct * rfidDevice = (*device);

            if (rfidDevice != NULL  )
            {
                // if ( rfidDevice->ip.length() > 0 )
                // {
                //     scan_with_ip_device(prjRollCallInfo, rollcallInstance, rollcall, rfidDevice);
                // }
                // else
                // {
                    if ( rfidDevice->gwid.length() > 0 )
                    {
                        send_MQTT_cmd((*i)->projectid, "StartScanRFIDCard", rfidDevice);
                    }
                    else
                    if ( rfidDevice->macaddress.length() > 0 )
                    {
                        std::ostringstream sendstreamStart;
                        sendstreamStart << "[{\"cmd\":\"StartScanRFIDCard\",\"time\":\"" << currentDateTimeForSQL() << "\"}]";
                        std::string payload = sendstreamStart.str();

                        MQTTDataItem * mqttitemStart = new MQTTDataItem("MQTTpublish", (*i)->projectid, rfidDevice->macaddress.c_str(), payload.c_str(), (int)payload.length());
                        mMQTTPublishQueue.add(mqttitemStart);
                    }
                // }
            }
        }
    }
}

void CARDRollCallService::CheckProjectRollCallEvent()
{
    int intime_count = 0;
    std::string todayDate = TodayDateType1();
    int todayWeekday = TodayDateWeekday();

    // vilslog::printf("CheckProjectRollCallEvent() todayDate[%s] todayWeekday[%d]\n", todayDate.c_str(), todayWeekday);

    for (std::vector<ProjectRollCallInfoStruct*>::iterator i = mProjectRollCallInfoVector.begin(); i != mProjectRollCallInfoVector.end(); i++)
    {
        for (std::list<CARDRollCallStruct*>::iterator rollcall = (*i)->mRollCallList->begin(); rollcall != (*i)->mRollCallList->end(); rollcall++)
        {
            bool todayNeedRollCall = CheckTodayNeedRollCall((*rollcall), todayDate, todayWeekday);

            // vilslog::printf("CheckProjectRollCallEvent() rollcallid[%d] rollcallname[%s] todayDate[%s] weekday.size()[%d] validdate.size()[%d] invaliddate.size()[%d]\n",
            //     (*rollcall)->rollcallid, (*rollcall)->rollcallname.c_str(), todayDate.c_str(), (int)(*rollcall)->weekday.size(), (int)(*rollcall)->validdate.size(), (int)(*rollcall)->invaliddate.size());
            //
            // vilslog::printf("CheckProjectRollCallEvent() todayNeedRollCall[%d]\n", todayNeedRollCall);
            if (todayNeedRollCall)
            {
                int ret = CheckRollCallEvent((*i), (*rollcall));
                if ( ret == 2 )
                {
                    intime_count++;
                }
            }
        }
    }
    if (intime_count > 0)
    {
        mRollCallRunning = true;
    }
    else
    {
        mRollCallRunning = false;
    }
}

bool CARDRollCallService::CheckTodayNeedRollCall(CARDRollCallStruct * rollcall, std::string todayDate, int todayWeekday)
{
    if ( rollcall->StartDate.length() > 0 )
    {
        if ( rollcall->StartDate.compare(todayDate) > 0 )
        {
            // 找到 排除點名 開始日期
            // vilslog::printf("CheckTodayNeedRollCall() 找到 排除點名 開始日期 rollcallname[%s] StartDate[%s] todayDate[%s]\n",
            //     rollcall->rollcallname.c_str(), rollcall->StartDate.c_str(), todayDate.c_str());
            return false;
        }
    }

    if ( rollcall->EndDate.length() > 0 )
    {
        if ( rollcall->EndDate.compare(todayDate) < 0 )
        {
            // 找到 排除點名 結束日期
            // vilslog::printf("CheckTodayNeedRollCall() 找到 排除點名 結束日期 rollcallname[%s] EndDate[%s] todayDate[%s]\n",
            //     rollcall->rollcallname.c_str(), rollcall->EndDate.c_str(), todayDate.c_str());
            return false;
        }
    }

    for (std::list<int>::iterator weekday = rollcall->weekday.begin(); weekday != rollcall->weekday.end(); weekday++)
    {
        if ( (*weekday) == todayWeekday)
        {
            //
            // 檢查 是否排除 今天
            //
            for (std::list<std::string>::iterator invaliddate = rollcall->invaliddate.begin(); invaliddate != rollcall->invaliddate.end(); invaliddate++)
            {
                if ( (*invaliddate).compare(todayDate) == 0 )
                {
                    // 找到 排除點名 日期
                    // vilslog::printf("CheckTodayNeedRollCall() todayNeedRollCall[%d] invaliddate[%s] todayDate[%s]\n", false, (*invaliddate).c_str(), todayDate.c_str());
                    return false;
                }
            }
            // 找不到 排除點名 日期
            // vilslog::printf("CheckTodayNeedRollCall() todayNeedRollCall[%d] todayDate[%s]\n", true, todayDate.c_str());
            return true;
        }
    }

    //
    // 檢查是否 需要點名
    //
    for (std::list<std::string>::iterator validdate = rollcall->validdate.begin(); validdate != rollcall->validdate.end(); validdate++)
    {
        if ( (*validdate).compare(todayDate) == 0 )
        {
            // 找到 需要點名 日期
            // vilslog::printf("CheckTodayNeedRollCall() todayNeedRollCall[%d] validdate[%s] todayDate[%s]\n", true, (*validdate).c_str(), todayDate.c_str());
            return true;
        }
    }

    // vilslog::printf("CheckTodayNeedRollCall() todayNeedRollCall[%d] todayDate[%s]\n", false, todayDate.c_str());
    return false;
}

int CARDRollCallService::CheckRollCallEvent(ProjectRollCallInfoStruct * prjRollCallInfo, CARDRollCallStruct * rollcall)
{
    int ret = 0;
    time_t t_Start = StringToInspectionDatetime(rollcall->StartTime);
    time_t t_End = StringToInspectionDatetime(rollcall->EndTime);
    time_t t_Current = time(0);

    // vilslog::printf("CheckRollCallEvent() rollcallid[%d] rollcallname[%s] t_Start[%lu] t_End[%lu] t_Current[%lu]\n",
    //     rollcall->rollcallid, rollcall->rollcallname.c_str(), t_Start, t_End, t_Current);

    if (t_Current < t_Start)
    {
        // not yet!
        time_t t_diff = t_Start - t_Current;
        RollCallEvent_outtime(prjRollCallInfo, rollcall, t_diff);
        ret = 1;
    }
    else
    if (t_Current >= t_Start && t_Current <= t_End)
    {
        // in check time interval
        time_t t_diff = t_Current - t_Start;
        RollCallEvent_intime(prjRollCallInfo, rollcall, t_diff);
        ret = 2;
    }
    else
    if (t_Current > t_End)
    {
        // check excess time event
        time_t t_diff = t_Current - t_End;
        RollCallEvent_exceedtime(prjRollCallInfo, rollcall, t_diff);
        ret = 3;
    }
    return ret;
}

void CARDRollCallService::RollCallEvent_outtime(ProjectRollCallInfoStruct * prjRollCallInfo, CARDRollCallStruct * rollcall, time_t t_diff)
{
    int diff_min = ((int)t_diff/60);
    //if ( diff_min <= 600 )
    if ( diff_min <= 1 )
    {
        // prepare event
        check_RollCallEvent_ready(prjRollCallInfo, rollcall);

        // clear_RollCallCardStruct_list(&mTempRFIDCardList);
        // mTempRFIDCardList.clear();
        //
        // clear_RollCallCardStruct_list(&mTempNFCCardList);
        // mTempNFCCardList.clear();
    }
}

void CARDRollCallService::RollCallEvent_intime(ProjectRollCallInfoStruct * prjRollCallInfo, CARDRollCallStruct * rollcall, time_t t_diff)
{
    RollCallInstance * rollcallInstance = getRollCallInstance(prjRollCallInfo, rollcall->rollcallid);

    if ( rollcallInstance == NULL )
    {
        return;
    }

    //
    // check all device is ready
    //
    check_RollCallEvent_ready(prjRollCallInfo, rollcall);

    check_RFID_Card_Present(prjRollCallInfo, rollcallInstance, rollcall);
    check_NFC_Card_Present(prjRollCallInfo, rollcallInstance, rollcall);

    check_trigger(prjRollCallInfo->projectid, rollcallInstance);

    //
    // update 實到 人數
    //
    if ( rollcallInstance->mUserCardList != NULL )
    {
        int present_count = 0;
        for (std::list<UserCardStruct*>::iterator UserCard = rollcallInstance->mUserCardList->begin(); UserCard != rollcallInstance->mUserCardList->end(); UserCard++)
        {
            if ((*UserCard)->present == 1)
            {
                present_count++;

                //
                // set subgroup event
                //
                set_present_subgroupevent((*UserCard)->userid, rollcallInstance->mRollCallSubGroupList);
            }
        }
        if ( rollcallInstance->mRollCallEvent != NULL )
        {
            rollcallInstance->mRollCallEvent->totalcount = (int)rollcallInstance->mUserCardList->size();
            rollcallInstance->mRollCallEvent->presentcount = present_count;

            WorkItem * work = new WorkItem("CARDRollCallEventStruct", prjRollCallInfo->projectid, rollcallInstance->mRollCallEvent);
            mMySQLJobQueue.add(work);
        }

        for (std::list<CARDRollCallSubGroupEventStruct *>::iterator subgroupevent = rollcallInstance->mRollCallSubGroupList->begin(); subgroupevent != rollcallInstance->mRollCallSubGroupList->end(); subgroupevent++)
        {
            (*subgroupevent)->totalcount = (int)(*subgroupevent)->userids.size();
            (*subgroupevent)->presentcount = (int)(*subgroupevent)->presentuserids.size();

            // vilslog::printf("RollCallEvent_intime() eventid[%d] rollcallid[%d] subgroupid[%d] totalcount[%d] presentcount[%d]\n",
            //     (*subgroupevent)->eventid, (*subgroupevent)->rollcallid, (*subgroupevent)->subgroupid, (*subgroupevent)->totalcount, (*subgroupevent)->presentcount);

            WorkItem * work = new WorkItem("CARDRollCallSubGroupEventStruct", prjRollCallInfo->projectid, (*subgroupevent));
            mMySQLJobQueue.add(work);
        }
    }
}

int CARDRollCallService::check_RFID_Card_Present(ProjectRollCallInfoStruct * prjRollCallInfo, RollCallInstance * rollcallInstance, CARDRollCallStruct * rollcall)
{
    int rollcallCount = 0;
    for (std::list<RollCallCardStruct*>::iterator rollcallcard = mTempRFIDCardList.begin(); rollcallcard != mTempRFIDCardList.end(); rollcallcard++)
    {
        std::string cardid = (*rollcallcard)->cardid;
        std::string gwid = (*rollcallcard)->gwid;
        std::string loramacaddress = (*rollcallcard)->loramacaddress;
        std::string datetime = (*rollcallcard)->datetime;

        //
        // 檢查 是否是要我們 指定的點名器
        //
        int card_rollcall_deviceid = getRFIDDeviceid(prjRollCallInfo->mRFIDDeviceList, gwid, loramacaddress, rollcall->maingroups);

        // vilslog::printf("CARDRollCallService::check_RFID_Card_Present() loramacaddress[%s] card_rollcall_deviceid[%d]\n", loramacaddress.c_str(), card_rollcall_deviceid);

        if ( card_rollcall_deviceid <= 0 )
        {
            continue;
        }

        bool bFound = false;
        for (std::list<int>::iterator deviceidIt = rollcall->rfiddeviceids.begin(); deviceidIt != rollcall->rfiddeviceids.end(); deviceidIt++)
        {
            if( (*deviceidIt) == card_rollcall_deviceid )
            {
                bFound = true;
                break;
            }
        }

        if (!bFound)
        {
            // vilslog::printf("CARDRollCallService::check_RFID_Card_Present() device not found!\n");
            continue;
        }

        if ( rollcallInstance->mUserCardList != NULL )
        {
            bool bFound = false;
            for (std::list<UserCardStruct*>::iterator UserCard = rollcallInstance->mUserCardList->begin(); UserCard != rollcallInstance->mUserCardList->end(); UserCard++)
            {
                bFound = check_user_rfid_card_rollcall(prjRollCallInfo, rollcallInstance, rollcall, card_rollcall_deviceid, cardid, (*UserCard), datetime);

                if ( bFound )
                {
                    break;
                }
            }
        }
        rollcallCount++;
    }

    // vilslog::printf("CARDRollCallService::check_RFID_Card_Present() rollcallCount[%d]\n", rollcallCount);

    return rollcallCount;
}

int CARDRollCallService::check_NFC_Card_Present(ProjectRollCallInfoStruct * prjRollCallInfo, RollCallInstance * rollcallInstance, CARDRollCallStruct * rollcall)
{
    int rollcallCount = 0;
    for (std::list<RollCallCardStruct*>::iterator rollcallcard = mTempNFCCardList.begin(); rollcallcard != mTempNFCCardList.end(); rollcallcard++)
    {
        std::string cardid = (*rollcallcard)->cardid;
        std::string gwid = (*rollcallcard)->gwid;
        std::string loramacaddress = (*rollcallcard)->loramacaddress;
        std::string datetime = (*rollcallcard)->datetime;

        //
        // 檢查 是否是要我們 指定的點名器
        //
        std::string card_rollcall_macaddress = getNFCDeviceMacaddress(prjRollCallInfo->mNFCDeviceList, gwid, loramacaddress, rollcall->maingroups);

        // vilslog::printf("CARDRollCallService::check_NFC_Card_Present() loramacaddress[%s] card_rollcall_macaddress[%d]\n", loramacaddress.c_str(), card_rollcall_macaddress.c_str());

        if ( card_rollcall_macaddress.length() <= 0 )
        {
            continue;
        }

        bool bFound = false;
        for (std::list<std::string>::iterator deviceMacIt = rollcall->nfclocators.begin(); deviceMacIt != rollcall->nfclocators.end(); deviceMacIt++)
        {
            if( (*deviceMacIt).compare(card_rollcall_macaddress) == 0 )
            {
                bFound = true;
                break;
            }
        }

        if (!bFound)
        {
            // vilslog::printf("CARDRollCallService::check_NFC_Card_Present() device not found!\n");
            continue;
        }

        if ( rollcallInstance->mUserCardList != NULL )
        {
            bool bFound = false;
            for (std::list<UserCardStruct*>::iterator UserCard = rollcallInstance->mUserCardList->begin(); UserCard != rollcallInstance->mUserCardList->end(); UserCard++)
            {
                bFound = check_user_nfc_card_rollcall(prjRollCallInfo, rollcallInstance, rollcall, card_rollcall_macaddress, cardid, (*UserCard), datetime);

                if ( bFound )
                {
                    break;
                }
            }
        }
    }

    // vilslog::printf("CARDRollCallService::check_NFC_Card_Present() rollcallCount[%d]\n", rollcallCount);

    return rollcallCount;
}

bool CARDRollCallService::check_trigger(int projectid, RollCallInstance * rollcallInstance)
{
    bool bFound = false;

    if (rollcallInstance->mTrigger == NULL)
        return false;

    std::string CurTime = currentTimeForSQL();

    //int totalMean = 0;

    for (std::list<CARDRollCallTriggerItemStruct*>::iterator item = rollcallInstance->mTrigger->triggeritems->begin(); item != rollcallInstance->mTrigger->triggeritems->end(); item++)
    {
        if ( CurTime.compare((*item)->TargetTime) == 0 && ((*item)->Triggered == 0))
        {

            float randv = (float) rand() / (RAND_MAX + 1.0f);

            // vilslog::printf("CARDRollCallService::check_trigger() randv[%f]\n", randv);
            // vilslog::printf("CARDRollCallService::check_trigger() rollcallInstance->mTrigger->TotalMean[%d] (*item)->NormDist[%f]\n",
            //     rollcallInstance->mTrigger->TotalMean, (*item)->NormDist);

            float LotMean = round( ((float)rollcallInstance->mTrigger->TotalMean * (*item)->NormDist)/100.0f );

            // vilslog::printf("CARDRollCallService::check_trigger() LotMean[%f]\n", LotMean);

            float random_sign = 1.0f;
            if(randv <= 0.5f)
            {
                random_sign = -1.0f;
            }

            // vilslog::printf("CARDRollCallService::check_trigger() random_sign[%f]\n", random_sign);

            float random_value = round((*item)->NormDist * 0.1f * randv);

            // vilslog::printf("CARDRollCallService::check_trigger() (*item)->NormDist * 0.1f * randv = [%f] random_value[%f]\n",
            //     (*item)->NormDist * 0.1f * randv, random_value);

            (*item)->ReportMean = max(LotMean + (random_sign * random_value), 0.0f);

            rollcallInstance->mTrigger->ReportTotalMean += (*item)->ReportMean;

            // vilslog::printf("CARDRollCallService::check_trigger()LotMean + (random_sign * random_value) = [%f]\n",
            //     LotMean + (random_sign * random_value));

            // vilslog::printf("check_trigger() [%s][%d]\n", rollcallInstance->mTrigger->TargetRollCallName.c_str(), (*item)->ReportMean);

#ifdef ENABLE_CHT_IOT
            if (rollcallInstance->mTrigger->TargetRollCallName.compare("到校點名") == 0)
            {
                DataItem * senditem = new DataItem("enterCounter", projectid);
                senditem->setMessage(std::to_string((*item)->ReportMean));
                mCHTIoTJobQueue.add(senditem);
            }
            else
            if (rollcallInstance->mTrigger->TargetRollCallName.compare("離校點名") == 0)
            {
                DataItem * senditem = new DataItem("exitCounter", projectid);
                senditem->setMessage(std::to_string((*item)->ReportMean));
                mCHTIoTJobQueue.add(senditem);
            }
#endif
            vilslog::printf("CARDRollCallService::check_trigger() found rollcallid[%d] CurTime[%s] NormDist[%f] ReportMean[%d] ReportTotalMean[%d]\n",
                rollcallInstance->rollcallid, CurTime.c_str(), (*item)->NormDist, (*item)->ReportMean, rollcallInstance->mTrigger->ReportTotalMean);

            (*item)->Triggered = 1;
            bFound = true;
            break;
        }
    }

    // vilslog::printf("CARDRollCallService::check_trigger() found rollcallid[%d] CurTime[%s] totalMean[%d]\n",
    //     rollcallInstance->rollcallid, CurTime.c_str(), totalMean);

    return bFound;
}

void CARDRollCallService::RollCallEvent_exceedtime(ProjectRollCallInfoStruct * prjRollCallInfo, CARDRollCallStruct * rollcall, time_t t_diff)
{
    int diff_min = ((int)t_diff/60);

    if ( (diff_min >= 1) && (diff_min <= 2) )
    {
        // vilslog::printf("RollCallEvent_exceedtime() rollcallid[%d] rollcallname[%s] diff_min[%d]\n",
        //     rollcall->rollcallid, rollcall->rollcallname.c_str(), diff_min);

        RollCallInstance * rollcallInstance = getRollCallInstance(prjRollCallInfo, rollcall->rollcallid);

        if ( rollcallInstance == NULL )
        {
            return;
        }

        // clear_RollCallCardStruct_list(&mTempRFIDCardList);
        // mTempRFIDCardList.clear();
        //
        // clear_RollCallCardStruct_list(&mTempNFCCardList);
        // mTempNFCCardList.clear();

        //
        // 檢查點名資料
        //
        if ( rollcallInstance->mUserCardList != NULL )
        {
            int present_count = 0;
            for (std::list<UserCardStruct*>::iterator UserCard = rollcallInstance->mUserCardList->begin(); UserCard != rollcallInstance->mUserCardList->end(); UserCard++)
            {
                if ((*UserCard)->present == 1)
                {
                    present_count++;
                }
                else
                {
                    //
                    // send notify
                    //
                    if (rollcall->absentnotify == 1)
                    {
                        std::string default_rfidcard = "";
                        for (std::list<std::string>::iterator rfidcard = (*UserCard)->rfidcards.begin(); rfidcard != (*UserCard)->rfidcards.end(); rfidcard++)
                        {
                            if (default_rfidcard.length() == 0 )
                            {
                                default_rfidcard = (*rfidcard);
                                break;
                            }
                        }
                        std::string default_nfccard = "";
                        for (std::list<std::string>::iterator nfccard = (*UserCard)->nfccards.begin(); nfccard != (*UserCard)->nfccards.end(); nfccard++)
                        {
                            if (default_nfccard.length() == 0 )
                            {
                                default_nfccard = (*nfccard);
                                break;
                            }
                        }

                        // save CARDRollCallEvent
                        CARDRollCallRecordStruct * newRecord = new CARDRollCallRecordStruct();
                        newRecord->rollcallid = rollcall->rollcallid;
                        newRecord->rollcalleventid = rollcallInstance->mRollCallEvent->eventid;
                        if (default_rfidcard.length() != 0 )
                            newRecord->cardid = default_rfidcard;
                        else
                        if (default_nfccard.length() != 0 )
                            newRecord->cardid = default_nfccard;
                        newRecord->userid = (*UserCard)->userid;
                        newRecord->nodeid = "";
                        //newRecord->rfiddeviceid = 0;
                        newRecord->present = 0;
                        newRecord->maingroups = rollcall->maingroups;
                        newRecord->datetime = currentDateTimeForSQL();

                        WorkItem * work2 = new WorkItem("CARDRollCallRecordStruct", prjRollCallInfo->projectid, newRecord);
                        mMySQLJobQueue.add(work2);

                        newRecord->rfiddeviceids.clear();
                        newRecord->nfclocators.clear();
                        delete newRecord;
                        //==================================
                        // 人員 推播
                        std::string message = (*UserCard)->name;
                        message.append(" ");
                        message.append(rollcall->absentcomment);
                        NotificationItem * item = new NotificationItem(TAG_PUSHNOTIFY_FUNCTION_12, prjRollCallInfo->projectid, NODE_TYPE_USER, (*UserCard)->account.c_str(), (*UserCard)->name.c_str(), to_string((*UserCard)->userid), rollcall->maingroups, "", "卡片點名通知", message);
                        mNotifiactionQueue.add(item);
                        //==================================

                        //==================================
                        // 卡片 推播
                        // NotificationItem * item = new NotificationItem(CARD_PUSHNOTIFY_FUNCTION_1, prjRollCallInfo->projectid, NODE_TYPE_CARD, default_rfidcard.c_str(), (*UserCard)->name.c_str(), "", "卡片點名通知", message);
                        // mNotifiactionQueue.add(item);
                        //==================================
                    }
                }
            }

            if ( rollcallInstance->mRollCallEvent != NULL )
            {
                rollcallInstance->mRollCallEvent->totalcount = (int)rollcallInstance->mUserCardList->size();
                rollcallInstance->mRollCallEvent->presentcount = present_count;

                WorkItem * work = new WorkItem("CARDRollCallEventStruct", prjRollCallInfo->projectid, rollcallInstance->mRollCallEvent);
                mMySQLJobQueue.add(work);

                vilslog::printf("RollCallEvent_exceedtime() rollcallid[%d] rollcallname[%s] present_count[%d]\n",
                    rollcall->rollcallid, rollcall->rollcallname.c_str(), present_count);

                // if (rollcallInstance->mRollCallEvent->rollcallname.compare("到校點名") == 0)
                // {
                //     vilslog::printf("RollCallEvent_exceedtime() 到校點名[%d]\n", present_count);
                //
                //     if ( present_count == 0 )
                //     {
                //         present_count = 10;
                //     }
                //
                //     DataItem * senditem = new DataItem("enterCounter", prjRollCallInfo->projectid);
                //     senditem->setMessage(std::to_string(present_count));
                //     mCHTIoTJobQueue.add(senditem);
                // }
                // else
                // if (rollcallInstance->mRollCallEvent->rollcallname.compare("離校點名") == 0)
                // {
                //     vilslog::printf("RollCallEvent_exceedtime() 離校點名[%d]\n", present_count);
                //
                //     if ( present_count == 0 )
                //     {
                //         present_count = 10;
                //     }
                //
                //     DataItem * senditem = new DataItem("exitCounter", prjRollCallInfo->projectid);
                //     senditem->setMessage(std::to_string(present_count));
                //     mCHTIoTJobQueue.add(senditem);
                // }
            }

            // release old one
            clear_UserCardStruct_list(rollcallInstance->mUserCardList);
            delete rollcallInstance->mUserCardList;
            rollcallInstance->mUserCardList = NULL;
        }

        //
        // close all device
        //
        // for (std::list<int>::iterator rfiddeviceid = rollcall->rfiddeviceids.begin(); rfiddeviceid != rollcall->rfiddeviceids.end(); rfiddeviceid++)
        // {
        //     int deviceid = (*rfiddeviceid);
        //     RFIDDeviceStruct * rfidDevice = getRFIDDevice(prjRollCallInfo->mRFIDDeviceList, deviceid);
        //
        //     if ( rfidDevice->ip.length() > 0 )
        //     {
        //         LLRPController * llrpCtrl = NULL;
        //         llrpCtrl = getLLRPController(rollcallInstance, rfidDevice);
        //
        //         if (llrpCtrl == NULL)
        //         {
        //             continue;
        //         }
        //
        //         if ( llrpCtrl->isReady() )
        //         {
        //             llrpCtrl->closeReader();
        //             vilslog::printf("RollCallEvent_exceedtime() projectid[%d] rollcallid[%d] closeReader deviceid[%d]\n",
        //                 prjRollCallInfo->projectid, rollcall->rollcallid, deviceid);
        //         }
        //     }
        //     else
        //     {
        //         if ( rfidDevice->gwid.length() > 0 )
        //         {
        //             send_MQTT_cmd(prjRollCallInfo->projectid, "StopScanRFIDCard", rfidDevice);
        //
        //             //
        //             // prepare wait ack StopScanRFIDCard
        //             //
        //             mWaitAck_projectid = prjRollCallInfo->projectid;
        //             mWaitAck_device.deviceid = rfidDevice->deviceid;
        //             mWaitAck_device.macaddress = rfidDevice->macaddress;
        //             mWaitAck_device.name = rfidDevice->name;
        //             mWaitAck_device.type = rfidDevice->type;
        //             mWaitAck_device.ip = rfidDevice->ip;
        //             mWaitAck_device.gwid = rfidDevice->gwid;
        //             mWaitAck_device.LoRAMacaddress = rfidDevice->LoRAMacaddress;
        //             mWaitAck_StopScanRFIDCard = true;
        //             mWaitAck_trycount = 0;
        //
        //         }
        //         else
        //         if ( rfidDevice->macaddress.length() > 0 )
        //         {
        //             std::ostringstream sendstreamStop;
        //             sendstreamStop << "[{\"cmd\":\"StopScanRFIDCard\",\"time\":\"" << currentDateTimeForSQL() << "\"}]";
        //             std::string payload = sendstreamStop.str();
        //
        //             MQTTDataItem * mqttitemStop = new MQTTDataItem("MQTTpublish", prjRollCallInfo->projectid, rfidDevice->macaddress.c_str(), payload.c_str(), (int)payload.length());
        //             mMQTTPublishQueue.add(mqttitemStop);
        //         }
        //     }
        // }

        // for (std::list<std::string>::iterator nfclocator = rollcall->nfclocators.begin(); nfclocator != rollcall->nfclocators.end(); nfclocator++)
        // {
        //     NFCDeviceStruct * nfcDevice = getNFCDevice(prjRollCallInfo->mNFCDeviceList, (*nfclocator));
        //     send_MQTT_cmd(prjRollCallInfo->projectid, "AT+SETMODE=ACTIVE", nfcDevice);
        // }
    }
    else
    if ( (diff_min >= 5) && (diff_min <= 10) )
    {
        RollCallInstance * rollcallInstance = getRollCallInstance(prjRollCallInfo, rollcall->rollcallid);

        if ( rollcallInstance == NULL )
        {
            return;
        }

        if ( rollcallInstance->mRollCallEvent != NULL )
        {
            vilslog::printf("RollCallEvent_exceedtime() rollcallid[%d] rollcallname[%s] diff_min[%d] release old one\n",
                rollcall->rollcallid, rollcall->rollcallname.c_str(), diff_min);

            // release old one
            delete rollcallInstance->mRollCallEvent;
            rollcallInstance->mRollCallEvent = NULL;
        }
        if ( rollcallInstance->mUserCardList != NULL )
        {
            // release old one
            clear_UserCardStruct_list(rollcallInstance->mUserCardList);
            delete rollcallInstance->mUserCardList;
            rollcallInstance->mUserCardList = NULL;
        }
        if (rollcallInstance->mTrigger != NULL)
        {
            clear_CARDRollCallTriggerItemStruct_list(rollcallInstance->mTrigger->triggeritems);
            delete rollcallInstance->mTrigger->triggeritems;
            delete rollcallInstance->mTrigger;
            rollcallInstance->mTrigger = NULL;
        }
    }
}

bool CARDRollCallService::check_user_rfid_card_rollcall(ProjectRollCallInfoStruct * prjRollCallInfo, RollCallInstance * rollcallInstance, CARDRollCallStruct * rollcall, int deviceid, std::string cardid, UserCardStruct * UserCard, std::string datetime)
{
    bool bFound = false;

    for (std::list<std::string>::iterator usercardid = UserCard->rfidcards.begin(); usercardid != UserCard->rfidcards.end(); usercardid++)
    {
        if ( (*usercardid).compare(cardid) == 0 )
        {
            //if ( UserCard->present == 0 )
            //{
                // vilslog::printf("check_user_rfid_card_rollcall() cardid[%s] userid[%d] save CARDRollCallEvent\n", cardid.c_str(), UserCard->userid);

                // save CARDRollCallEvent
                CARDRollCallRecordStruct * newRecord = new CARDRollCallRecordStruct();
                newRecord->rollcallid = rollcall->rollcallid;
                newRecord->rollcalleventid = rollcallInstance->mRollCallEvent->eventid;
                newRecord->cardid = cardid;
                newRecord->userid = UserCard->userid;
                newRecord->nodeid = "";
                newRecord->rfiddeviceids.push_back(deviceid);
                //newRecord->nfclocators.push_back();
                //newRecord->rfiddeviceid = deviceid;
                //newRecord->nfclocator = "";
                newRecord->present = 1;
                newRecord->maingroups = rollcall->maingroups;
                newRecord->datetime = datetime;

                WorkItem * work2 = new WorkItem("CARDRollCallRecordStruct", prjRollCallInfo->projectid, newRecord);
                mMySQLJobQueue.add(work2);

                newRecord->rfiddeviceids.clear();
                newRecord->nfclocators.clear();
                delete newRecord;

                //
                // send notify
                //
                if ((rollcall->presentnotify == 1) && ((UserCard->present == 0) || (rollcall->resend == 1)) )
                {
                    //==================================
                    // 人員 推播
                    std::string message = UserCard->name;
                    message.append(" ");
                    message.append(rollcall->presentcomment);
                    NotificationItem * item = new NotificationItem(TAG_PUSHNOTIFY_FUNCTION_12, prjRollCallInfo->projectid, NODE_TYPE_USER, UserCard->account.c_str(), UserCard->name.c_str(), to_string(UserCard->userid), rollcall->maingroups, cardid.c_str(), "卡片點名通知", message);
                    mNotifiactionQueue.add(item);
                    //==================================

                    //==================================
                    // 卡片 推播
                    NotificationItem * itemCard = new NotificationItem(CARD_PUSHNOTIFY_FUNCTION_1, prjRollCallInfo->projectid, NODE_TYPE_CARD, UserCard->account.c_str(), UserCard->name.c_str(), to_string(UserCard->userid), rollcall->maingroups, cardid.c_str(), "卡片點名通知", message);
                    mNotifiactionQueue.add(itemCard);
                    //==================================
                }

                UserCard->present = 1;
                bFound = true;
            //}
            //break; // 可能有多張
        }
    }

    return bFound;
}

bool CARDRollCallService::check_user_nfc_card_rollcall(ProjectRollCallInfoStruct * prjRollCallInfo, RollCallInstance * rollcallInstance, CARDRollCallStruct * rollcall, std::string macaddress, std::string cardid, UserCardStruct * UserCard, std::string datetime)
{
    bool bFound = false;

    for (std::list<std::string>::iterator usercardid = UserCard->nfccards.begin(); usercardid != UserCard->nfccards.end(); usercardid++)
    {
        if ( (*usercardid).compare(cardid) == 0 )
        {
            //if ( UserCard->present == 0 )
            //{
                // vilslog::printf("check_user_nfc_card_rollcall() cardid[%s] userid[%d] save CARDRollCallEvent\n", cardid.c_str(), UserCard->userid);

                // save CARDRollCallEvent
                CARDRollCallRecordStruct * newRecord = new CARDRollCallRecordStruct();
                newRecord->rollcallid = rollcall->rollcallid;
                newRecord->rollcalleventid = rollcallInstance->mRollCallEvent->eventid;
                newRecord->cardid = cardid;
                newRecord->userid = UserCard->userid;
                newRecord->nodeid = "";
                //newRecord->rfiddeviceids.push_back();
                newRecord->nfclocators.push_back(macaddress);
                //newRecord->rfiddeviceid = 0;
                //newRecord->nfclocator = macaddress;
                newRecord->present = 1;
                newRecord->maingroups = rollcall->maingroups;
                newRecord->datetime = datetime;

                WorkItem * work2 = new WorkItem("CARDRollCallRecordStruct", prjRollCallInfo->projectid, newRecord);
                mMySQLJobQueue.add(work2);

                newRecord->rfiddeviceids.clear();
                newRecord->nfclocators.clear();
                delete newRecord;

                //
                // send notify
                //
                //if ((rollcall->presentnotify == 1) && (UserCard->present == 0) )
                if ((rollcall->presentnotify == 1) && ((UserCard->present == 0) || (rollcall->resend == 1)) )
                {
                    //==================================
                    // 人員 推播
                    std::string message = UserCard->name;
                    message.append(" ");
                    message.append(rollcall->presentcomment);
                    NotificationItem * item = new NotificationItem(TAG_PUSHNOTIFY_FUNCTION_12, prjRollCallInfo->projectid, NODE_TYPE_USER, UserCard->account.c_str(), UserCard->name.c_str(), to_string(UserCard->userid), rollcall->maingroups, cardid.c_str(), "卡片點名通知", message);
                    mNotifiactionQueue.add(item);
                    //==================================

                    //==================================
                    // 卡片 推播
                    NotificationItem * itemCard = new NotificationItem(CARD_PUSHNOTIFY_FUNCTION_1, prjRollCallInfo->projectid, NODE_TYPE_CARD, UserCard->account.c_str(), UserCard->name.c_str(), to_string(UserCard->userid), rollcall->maingroups, cardid.c_str(), "卡片點名通知", message);
                    mNotifiactionQueue.add(itemCard);
                    //==================================
                }

                UserCard->present = 1;
                bFound = true;
            //}
            //break; // 可能有多張
        }
    }

    return bFound;
}

bool CARDRollCallService::check_RollCallEvent_ready(ProjectRollCallInfoStruct * prjRollCallInfo, CARDRollCallStruct * rollcall)
{
    RollCallInstance * rollcallInstance = getRollCallInstance(prjRollCallInfo, rollcall->rollcallid);

    if ( rollcallInstance == NULL )
    {
        return false;
    }

    time_t t_Start = StringToInspectionDatetime(rollcall->StartTime);
    time_t t_End = StringToInspectionDatetime(rollcall->EndTime);
    std::string StartDateTime = time_tToString(t_Start);
    std::string EndDateTime = time_tToString(t_End);

    if ( rollcallInstance->mRollCallEvent == NULL )
    {
        MySQLDBAccess * baseDb = mDbManager->getBaseDB(prjRollCallInfo->projectid);
        if (baseDb == NULL)
            return false;

        rollcallInstance->mRollCallEvent = baseDb->get_CARDRollCallEvent(rollcall->rollcallid, StartDateTime, EndDateTime, rollcall->maingroups, true);
        rollcallInstance->mRollCallEvent->rollcallname = rollcall->rollcallname;
        rollcallInstance->mRollCallEvent->totalcount = 0;

        vilslog::printf("check_RollCallEvent_ready() rollcallid[%d] eventid[%d] [%s]->[%s] 1 new RollCallEvent\n",
            rollcall->rollcallid, rollcallInstance->mRollCallEvent->eventid, StartDateTime.c_str(), EndDateTime.c_str());

        rollcallInstance->mTrigger = find_trigger(prjRollCallInfo, rollcall->rollcallname);
    }
    else
    {
        if ( (rollcallInstance->mRollCallEvent->StartDateTime.compare(StartDateTime) != 0) ||
             (rollcallInstance->mRollCallEvent->EndDateTime.compare(EndDateTime) != 0) )
        {
            vilslog::printf("check_RollCallEvent_ready() rollcallid[%d] eventid[%d] [%s]->[%s] 1 Found old RollCallEvent\n",
                rollcall->rollcallid, rollcallInstance->mRollCallEvent->eventid,
                rollcallInstance->mRollCallEvent->StartDateTime.c_str(), rollcallInstance->mRollCallEvent->EndDateTime.c_str());

            // release old one
            delete rollcallInstance->mRollCallEvent;
            rollcallInstance->mRollCallEvent = NULL;

            MySQLDBAccess * baseDb = mDbManager->getBaseDB(prjRollCallInfo->projectid);
            if (baseDb == NULL)
                return false;

            rollcallInstance->mRollCallEvent = baseDb->get_CARDRollCallEvent(rollcall->rollcallid, StartDateTime, EndDateTime, rollcall->maingroups, true);
            rollcallInstance->mRollCallEvent->rollcallname = rollcall->rollcallname;
            rollcallInstance->mRollCallEvent->totalcount = 0;

            vilslog::printf("check_RollCallEvent_ready() rollcallid[%d] eventid[%d] [%s]->[%s] 2 new RollCallEvent\n",
                rollcall->rollcallid, rollcallInstance->mRollCallEvent->eventid, StartDateTime.c_str(), EndDateTime.c_str());

            rollcallInstance->mTrigger = find_trigger(prjRollCallInfo, rollcall->rollcallname);

        }
        // else
        // {
        //     vilslog::printf("RollCallEvent_outtime() rollcallid[%d] eventid[%d] [%s]->[%s] 2 Found old RollCallEvent\n",
        //         rollcall->rollcallid, rollcallInstance->mRollCallEvent->eventid,
        //         rollcallInstance->mRollCallEvent->StartDateTime.c_str(), rollcallInstance->mRollCallEvent->EndDateTime.c_str());
        // }
    }

    if ( rollcallInstance->mRollCallSubGroupList == NULL )
    {
        rollcallInstance->mRollCallSubGroupList = new std::list<CARDRollCallSubGroupEventStruct*>();

        for (std::list<int>::iterator subgroup = rollcall->subgroups.begin(); subgroup != rollcall->subgroups.end(); subgroup++)
        {
            int subgroupid = (*subgroup);

            bool bFound = false;
            for (std::list<CARDRollCallSubGroupEventStruct *>::iterator subgroupevent = rollcallInstance->mRollCallSubGroupList->begin(); subgroupevent != rollcallInstance->mRollCallSubGroupList->end(); subgroupevent++)
            {
                if ( (*subgroupevent)->subgroupid == subgroupid )
                {
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
            {
                CARDRollCallSubGroupEventStruct * newEvent = new CARDRollCallSubGroupEventStruct();
                newEvent->eventid = rollcallInstance->mRollCallEvent->eventid;
                newEvent->rollcallid = rollcallInstance->mRollCallEvent->rollcallid;
                newEvent->subgroupid = subgroupid;
                newEvent->rollcallname = rollcallInstance->mRollCallEvent->rollcallname;
                newEvent->StartDateTime = rollcallInstance->mRollCallEvent->StartDateTime;
                newEvent->EndDateTime = rollcallInstance->mRollCallEvent->EndDateTime;
                newEvent->totalcount = 0;
                newEvent->presentcount = 0;
                newEvent->maingroups = rollcall->maingroups;

                // vilslog::printf("check_RollCallEvent_ready() newEvent rollcallid[%d] eventid[%d] subgroupid[%d] rollcallname[%s] maingroups[%s] \n",
                //     newEvent->rollcallid, newEvent->eventid, subgroupid, newEvent->rollcallname.c_str(), rollcall->maingroups.c_str());

                rollcallInstance->mRollCallSubGroupList->push_back(newEvent);
            }
        }
    }

    if ( rollcallInstance->mUserCardList == NULL )
    {
        check_RollCallEvent_UserCardList(prjRollCallInfo, rollcall, rollcallInstance);
    }

    if ( rollcallInstance->mRollCallEvent != NULL )
    {
        if ( rollcallInstance->mUserCardList != NULL )
        {
            rollcallInstance->mRollCallEvent->totalcount = (int)rollcallInstance->mUserCardList->size();
            // vilslog::printf("check_RollCallEvent_ready() rollcallInstance->mRollCallEvent->totalcount[%d]\n", rollcallInstance->mRollCallEvent->totalcount);
        }
    }

    return true;
}

bool CARDRollCallService::check_RollCallEvent_UserCardList(ProjectRollCallInfoStruct * prjRollCallInfo, CARDRollCallStruct * rollcall, RollCallInstance * rollcallInstance)
{
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(prjRollCallInfo->projectid);
    if (baseDb == NULL)
        return false;

    std::list<UserStruct*> * userList = baseDb->get_Users();

    //
    // chech if existed record
    //
    std::list<CARDRollCallRecordStruct*> * recordList = baseDb->get_CARDRollCallRecords(rollcallInstance->mRollCallEvent->rollcallid, rollcallInstance->mRollCallEvent->eventid);

    // vilslog::printf("check_RollCallEvent_UserCardList() userList->size()[%d] recordList->size()[%d]\n", (int)userList->size(), (int)recordList->size());

    rollcallInstance->mUserCardList = new std::list<UserCardStruct*>();

    for (std::list<UserStruct*>::iterator user = userList->begin(); user != userList->end(); user++)
    {
        //std::list<std::string> * subgroups = convert_string_to_list((*user)->subgroups, ',');

        // vilslog::printf("check_RollCallEvent_ready() (*user)->userid[%d] subgroups->size()[%d]\n", (*user)->userid, (int)(*user)->subgroups->size());

        bool bFound = check_match_subgroup((*user)->subgroups, &rollcall->subgroups);

        if (bFound)
        {
            // vilslog::printf("check_RollCallEvent_UserCardList() Found user[%d] rfidcards.size()[%d] nfccards.size()[%d]\n",
            //     (*user)->userid, (int)(*user)->rfidcards->size(), (int)(*user)->nfccards->size());

            // 可點名無卡片人員
            //if ( ((*user)->rfidcards->size() > 0) || ((*user)->nfccards->size() > 0) )
            //{
                UserCardStruct * newUserCard = new UserCardStruct();
                newUserCard->userid = (*user)->userid;
                newUserCard->name = (*user)->name;
                newUserCard->account = (*user)->account;
                newUserCard->present = 0;

                for (std::list<std::string>::iterator rfidcard = (*user)->rfidcards->begin(); rfidcard != (*user)->rfidcards->end(); rfidcard++)
                {
                    newUserCard->rfidcards.push_back((*rfidcard));

                    int present = check_existed_CARDRollCallRecord(prjRollCallInfo, rollcall, rollcallInstance, (*user)->userid, (*rfidcard), recordList);
                    if ( present > 0 )
                    {
                        newUserCard->present = 1;
                    }
                }
                for (std::list<std::string>::iterator nfccard = (*user)->nfccards->begin(); nfccard != (*user)->nfccards->end(); nfccard++)
                {
                    newUserCard->nfccards.push_back((*nfccard));

                    int present = check_existed_CARDRollCallRecord(prjRollCallInfo, rollcall, rollcallInstance, (*user)->userid, (*nfccard), recordList);
                    if ( present > 0 )
                    {
                        newUserCard->present = 1;
                    }
                }

                if ( ((*user)->rfidcards->size() == 0) && ((*user)->nfccards->size() == 0) )
                {
                    check_existed_CARDRollCallRecord(prjRollCallInfo, rollcall, rollcallInstance, (*user)->userid, "", recordList);

                    // vilslog::printf("check_RollCallEvent_UserCardList() eventid[%d] rollcallid[%d] (*user)->userid[%d]\n",
                    //     rollcallInstance->mRollCallEvent->eventid, rollcallInstance->mRollCallEvent->rollcallid, (*user)->userid);
                }

                rollcallInstance->mUserCardList->push_back(newUserCard);

                //
                // put to subgroup event
                //
                set_subgroupevent((*user), rollcallInstance->mRollCallSubGroupList);
            //}
        }
    }

    rollcallInstance->mRollCallEvent->totalcount = (int)rollcallInstance->mUserCardList->size();
    rollcallInstance->mRollCallEvent->presentcount = 0;

    // vilslog::printf("check_RollCallEvent_UserCardList() eventid[%d] rollcallid[%d] totalcount[%d] presentcount[%d]\n",
    //     rollcallInstance->mRollCallEvent->eventid, rollcallInstance->mRollCallEvent->rollcallid, rollcallInstance->mRollCallEvent->totalcount, rollcallInstance->mRollCallEvent->presentcount);


    WorkItem * work = new WorkItem("CARDRollCallEventStruct", prjRollCallInfo->projectid, rollcallInstance->mRollCallEvent);
    mMySQLJobQueue.add(work);

    for (std::list<CARDRollCallSubGroupEventStruct *>::iterator subgroupevent = rollcallInstance->mRollCallSubGroupList->begin(); subgroupevent != rollcallInstance->mRollCallSubGroupList->end(); subgroupevent++)
    {
        // vilslog::printf("check_RollCallEvent_UserCardList() eventid[%d] rollcallid[%d] subgroupid[%d] totalcount[%d] presentcount[%d]\n",
        //     (*subgroupevent)->eventid, (*subgroupevent)->rollcallid, (*subgroupevent)->subgroupid, (*subgroupevent)->totalcount, (*subgroupevent)->presentcount);

        (*subgroupevent)->totalcount = (int)(*subgroupevent)->userids.size();
        (*subgroupevent)->presentcount = (int)(*subgroupevent)->presentuserids.size();

        WorkItem * work = new WorkItem("CARDRollCallSubGroupEventStruct", prjRollCallInfo->projectid, (*subgroupevent));
        mMySQLJobQueue.add(work);

    }

    clear_UserStruct_list(userList);
    delete userList;

    clear_CARDRollCallRecordStruct_list(recordList);
    delete recordList;

    return true;
}

bool CARDRollCallService::check_match_subgroup(std::list<std::string> * src_subgroups, std::list<int> * dst_subgroups)
{
    bool bFound = false;
    for (std::list<std::string>::iterator subgroupstr = src_subgroups->begin(); subgroupstr != src_subgroups->end(); subgroupstr++)
    {
        int usersubgroupid = convert_string_to_int((*subgroupstr));

        // vilslog::printf("check_RollCallEvent_ready() usersubgroupid[%d] \n", usersubgroupid);

        for (std::list<int>::iterator subgroup = dst_subgroups->begin(); subgroup != dst_subgroups->end(); subgroup++)
        {
            int subgroupid = (*subgroup);
            // vilslog::printf("check_RollCallEvent_ready() subgroupid[%d] \n", subgroupid);

            // find user from sungroup
            if ( usersubgroupid == subgroupid )
            {
                bFound = true;
                break;
            }
        }
        if (bFound)
        {
            break;
        }
    }

    return bFound;
}

int CARDRollCallService::check_existed_CARDRollCallRecord(ProjectRollCallInfoStruct * prjRollCallInfo, CARDRollCallStruct * rollcall, RollCallInstance * rollcallInstance, int userid, std::string cardid, std::list<CARDRollCallRecordStruct*> * recordList)
{
    //
    // chech if existed record
    //
    int present = 0;
    CARDRollCallRecordStruct * existed_record = NULL;
    for (std::list<CARDRollCallRecordStruct*>::iterator record = recordList->begin(); record != recordList->end(); record++)
    {
        if ( ((*record)->userid == userid) && ((*record)->cardid.compare(cardid) == 0 ))
        {
            existed_record = (*record);
            present = (*record)->present;
            break;
        }
    }

    // save CARDRollCallEvent
    CARDRollCallRecordStruct * newRecord = getRollCallRecord(rollcallInstance->mRollCallRecordList, rollcall->rollcallid, rollcallInstance->mRollCallEvent->eventid, userid, cardid);

    newRecord->userid = userid;
    newRecord->nodeid = "";
    newRecord->present = present;
    newRecord->maingroups = rollcall->maingroups;

    if (existed_record != NULL)
    {
        newRecord->cardid = existed_record->cardid;
        for (std::list<int>::iterator value = existed_record->rfiddeviceids.begin(); value != existed_record->rfiddeviceids.end(); value++)
        {
            newRecord->rfiddeviceids.push_back((*value));
        }

        for (std::list<std::string>::iterator value = existed_record->nfclocators.begin(); value != existed_record->nfclocators.end(); value++)
        {
            newRecord->nfclocators.push_back((*value));
        }
    }

    WorkItem * work2 = new WorkItem("CARDRollCallRecordStruct", prjRollCallInfo->projectid, newRecord);
    mMySQLJobQueue.add(work2);

    return present;
}


bool CARDRollCallService::updateRollCallRecord(int projectid, CARDRollCallRecordStruct * record)
{
    int rollcallid = record->rollcallid;
    int rollcalleventid = record->rollcalleventid;
    std::string cardid = record->cardid;
    int userid = record->userid;

    printf("CARDRollCallService::updateRollCallRecord() rollcallid[%d] rollcalleventid[%d] cardid[%s] userid[%d]\n",
        rollcallid, rollcalleventid, cardid.c_str(), userid);

    int intime_count = 0;
    for (std::vector<ProjectRollCallInfoStruct*>::iterator i = mProjectRollCallInfoVector.begin(); i != mProjectRollCallInfoVector.end(); i++)
    {
        if ( (*i)->projectid != projectid )
        {
            continue;
        }

        for (std::list<CARDRollCallStruct*>::iterator rollcall = (*i)->mRollCallList->begin(); rollcall != (*i)->mRollCallList->end(); rollcall++)
        {
            if((*rollcall)->rollcallid == rollcallid)
            {
                int ret = CheckRollCallEvent_time((*rollcall));
                if ( ret == 1 )
                {
                    // do nothing
                }
                else
                if ( ret == 2 )
                {
                    // update current data

                    RollCallInstance * rollcallInstance = getRollCallInstance((*i), (*rollcall)->rollcallid);

                    if ( rollcallInstance == NULL )
                    {
                        break;
                    }

                    if ( rollcallInstance->mUserCardList != NULL )
                    {
                        for (std::list<UserCardStruct*>::iterator UserCard = rollcallInstance->mUserCardList->begin(); UserCard != rollcallInstance->mUserCardList->end(); UserCard++)
                        {
                            if( (*UserCard)->userid == userid )
                            {
                                if ( (*UserCard)->present == 0 )
                                {
                                    vilslog::printf("updateRollCallRecord() userid[%d] cardid[%s] save CARDRollCallEvent\n", (*UserCard)->userid, cardid.c_str());

                                    // save CARDRollCallEvent
                                    CARDRollCallRecordStruct * newRecord = new CARDRollCallRecordStruct();
                                    newRecord->rollcallid = (*rollcall)->rollcallid;
                                    newRecord->rollcalleventid = rollcalleventid;
                                    newRecord->cardid = cardid;
                                    newRecord->userid = (*UserCard)->userid;
                                    newRecord->nodeid = "";
                                    //newRecord->rfiddeviceid = 0;
                                    newRecord->present = 1;
                                    newRecord->maingroups = (*rollcall)->maingroups;
                                    newRecord->datetime = currentDateTimeForSQL();

                                    WorkItem * work2 = new WorkItem("CARDRollCallRecordStruct", projectid, newRecord);
                                    mMySQLJobQueue.add(work2);

                                    newRecord->rfiddeviceids.clear();
                                    newRecord->nfclocators.clear();
                                    delete newRecord;

                                    //
                                    // send notify
                                    //
                                    if ((*rollcall)->presentnotify == 1)
                                    {
                                        //==================================
                                        // 人員 推播
                                        std::string message = (*UserCard)->name;
                                        message.append(" ");
                                        message.append((*rollcall)->presentcomment);
                                        NotificationItem * item = new NotificationItem(TAG_PUSHNOTIFY_FUNCTION_12, projectid, NODE_TYPE_USER, (*UserCard)->account.c_str(), (*UserCard)->name.c_str(), to_string((*UserCard)->userid), record->maingroups, cardid.c_str(), "卡片點名通知", message);
                                        mNotifiactionQueue.add(item);
                                        //==================================

                                        //==================================
                                        // 卡片 推播
                                        // NotificationItem * itemCard = new NotificationItem(CARD_PUSHNOTIFY_FUNCTION_1, prjRollCallInfo->projectid, NODE_TYPE_CARD, cardid.c_str(), (*UserCard)->name.c_str(), "", "卡片點名通知", message);
                                        // mNotifiactionQueue.add(itemCard);
                                        //==================================
                                    }

                                    (*UserCard)->present = 1;
                                    //bFound = true;
                                }
                                // break; //可能多張
                            }
                        }

                        //
                        // update 實到 人數
                        //
                        int present_count = 0;
                        for (std::list<UserCardStruct*>::iterator UserCard = rollcallInstance->mUserCardList->begin(); UserCard != rollcallInstance->mUserCardList->end(); UserCard++)
                        {
                            if ((*UserCard)->present == 1)
                            {
                                present_count++;
                            }
                        }
                        if ( rollcallInstance->mRollCallEvent != NULL )
                        {
                            rollcallInstance->mRollCallEvent->totalcount = (int)rollcallInstance->mUserCardList->size();
                            rollcallInstance->mRollCallEvent->presentcount = present_count;

                            WorkItem * work = new WorkItem("CARDRollCallEventStruct", projectid, rollcallInstance->mRollCallEvent);
                            mMySQLJobQueue.add(work);
                        }
                    }

                    intime_count++;
                }
                else
                if ( ret == 3 )
                {
                    // update MySQL data
                }
            }
        }

        break;
    }

    return true;
}


int CARDRollCallService::CheckRollCallEvent_time(CARDRollCallStruct * rollcall)
{
    int ret = 0;
    time_t t_Start = StringToInspectionDatetime(rollcall->StartTime);
    time_t t_End = StringToInspectionDatetime(rollcall->EndTime);
    time_t t_Current = time(0);

    // vilslog::printf("CheckRollCallEvent_time() rollcallid[%d] rollcallname[%s] t_Start[%lu] t_End[%lu] t_Current[%lu]\n",
    //     rollcall->rollcallid, rollcall->rollcallname.c_str(), t_Start, t_End, t_Current);

    if (t_Current < t_Start)
    {
        // not yet!
        ret = 1;
    }
    else
    if (t_Current >= t_Start && t_Current <= t_End)
    {
        // in check time interval
        ret = 2;
    }
    else
    if (t_Current > t_End)
    {
        // check excess time event
        ret = 3;
    }
    return ret;
}

RollCallInstance * CARDRollCallService::getRollCallInstance(ProjectRollCallInfoStruct * prjRollCallInfo, int rollcallid)
{
    bool bFound = false;
    RollCallInstance * rollcallInstance = NULL;
    for (std::list<RollCallInstance*>::iterator llrpinst = prjRollCallInfo->mRollCallInstanceList->begin(); llrpinst != prjRollCallInfo->mRollCallInstanceList->end(); llrpinst++)
    {
        // vilslog::printf("getRollCallInstance() rollcallid[%d] rollcallname[%s] (*llrpinst)->rollcallid[%d]\n",
        //     rollcall->rollcallid, rollcall->rollcallname.c_str(), (*llrpinst)->rollcallid);

        if ( rollcallid == (*llrpinst)->rollcallid )
        {
            bFound = true;
            rollcallInstance = (*llrpinst);
            break;
        }
    }

    if (!bFound)
    {
        rollcallInstance = new RollCallInstance();
        rollcallInstance->rollcallid = rollcallid;
        rollcallInstance->mRollCallEvent = NULL;
        rollcallInstance->mUserCardList = NULL;
        //rollcallInstance->mLLRPContollerList = new std::list<LLRPController*>();
        rollcallInstance->mRollCallRecordList = new std::list<CARDRollCallRecordStruct*>();
        rollcallInstance->mRollCallSubGroupList = NULL;
        rollcallInstance->mTrigger = NULL;

        prjRollCallInfo->mRollCallInstanceList->push_back(rollcallInstance);
    }
    return rollcallInstance;
}

RFIDDeviceStruct * CARDRollCallService::getRFIDDevice(std::list<RFIDDeviceStruct*> * RFIDDeviceList, int deviceid)
{
    RFIDDeviceStruct * rfidDevice = NULL;
    for (std::list<RFIDDeviceStruct*>::iterator device = RFIDDeviceList->begin(); device != RFIDDeviceList->end(); device++)
    {
        if ( (*device)->deviceid == deviceid )
        {
            rfidDevice = (*device);
            break;
        }
    }
    return rfidDevice;
}

NFCDeviceStruct * CARDRollCallService::getNFCDevice(std::list<NFCDeviceStruct*> * NFCDeviceList, std::string macaddress)
{
    NFCDeviceStruct * nfcDevice = NULL;
    for (std::list<NFCDeviceStruct*>::iterator device = NFCDeviceList->begin(); device != NFCDeviceList->end(); device++)
    {
        if ( (*device)->macaddress.compare(macaddress) == 0 )
        {
            nfcDevice = (*device);
            break;
        }
    }
    return nfcDevice;
}

int CARDRollCallService::getRFIDDeviceid(std::list<RFIDDeviceStruct*> * RFIDDeviceList, std::string gwid, std::string loramacaddress, std::string maingroups)
{
    int deviceid = -1;
    for (std::list<RFIDDeviceStruct*>::iterator device = RFIDDeviceList->begin(); device != RFIDDeviceList->end(); device++)
    {
        //if ( ((*device)->gwid.compare(gwid) == 0) && ((*device)->LoRAMacaddress.compare(loramacaddress) == 0) )
        if ( (*device)->LoRAMacaddress.compare(loramacaddress) == 0 )
        {
            //
            // check device in maingroups
            //
            std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');

            bool bFound = false;
            for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
            {
                int maingroupid = convert_string_to_int(*maingroup);
                if ( maingroupid == (*device)->maingroupid )
                {
                    // vilslog::printf("CARDRollCallService::getNFCDeviceMacaddress() bFound maingroupid[%d]\n", maingroupid);

                    bFound = true;
                }
            }
            maingroupList->clear();
            delete maingroupList;

            if (bFound)
            {
                deviceid = (*device)->deviceid;
            }

            break;
        }
    }
    return deviceid;
}

std::string CARDRollCallService::getNFCDeviceMacaddress(std::list<NFCDeviceStruct*> * NFCDeviceList, std::string gwid, std::string loramacaddress, std::string maingroups)
{
    std::string macaddress = "";
    for (std::list<NFCDeviceStruct*>::iterator device = NFCDeviceList->begin(); device != NFCDeviceList->end(); device++)
    {
        //if ( ((*device)->gwid.compare(gwid) == 0) && ((*device)->LoRAMacaddress.compare(loramacaddress) == 0) )
        if ( (*device)->LoRAMacaddress.compare(loramacaddress) == 0 )
        {

            // vilslog::printf("CARDRollCallService::getNFCDeviceMacaddress() deviceid[%d] macaddress[%s] maingroupid[%d] maingroups[%s]\n",
            //     (*device)->deviceid, (*device)->macaddress.c_str(), (*device)->maingroupid, maingroups.c_str());

            //
            // check device in maingroups
            //
            std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');

            bool bFound = false;
            for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
            {
                int maingroupid = convert_string_to_int(*maingroup);
                if ( maingroupid == (*device)->maingroupid )
                {
                    // vilslog::printf("CARDRollCallService::getNFCDeviceMacaddress() bFound maingroupid[%d]\n", maingroupid);

                    bFound = true;
                }
            }
            maingroupList->clear();
            delete maingroupList;

            if (bFound)
            {
                macaddress = (*device)->macaddress;
            }
            // vilslog::printf("CARDRollCallService::getNFCDeviceMacaddress() bFound[%d] macaddress[%s]\n", bFound, macaddress.c_str());

            break;
        }
    }
    return macaddress;
}

CARDRollCallRecordStruct * CARDRollCallService::getRollCallRecord(std::list<CARDRollCallRecordStruct*> * RollCallRecordList, int rollcallid, int eventid, int userid, std::string cardid)
{
    bool bFound = false;
    CARDRollCallRecordStruct * retRecord = NULL;
    for (std::list<CARDRollCallRecordStruct*>::iterator record = RollCallRecordList->begin(); record != RollCallRecordList->end(); record++)
    {
        if ( (*record)->rollcallid == rollcallid && (*record)->rollcalleventid == eventid )
        {
            if ( (cardid.compare((*record)->cardid) == 0) && ((*record)->userid == userid))
            {
                retRecord = (*record);
                bFound = true;
                break;
            }
        }
    }

    if (!bFound)
    {
        // vilslog::printf("CARDRollCallService::getRollCallRecord() rollcallid[%d] eventid[%d] userid[%d] cardid[%s]\n", rollcallid, eventid, userid, cardid.c_str());

        retRecord = new CARDRollCallRecordStruct();
        retRecord->rollcallid = rollcallid;
        retRecord->rollcalleventid = eventid;
        retRecord->cardid = cardid;
        retRecord->userid = userid;
        retRecord->nodeid = "";
        //retRecord->rfiddeviceid = 0;
        //retRecord->nfclocator = "";
        retRecord->present = 0;
        retRecord->maingroups = "";
        retRecord->datetime = currentDateTimeForSQL();

        RollCallRecordList->push_back(retRecord);
    }

    return retRecord;
}

bool CARDRollCallService::put_to_TempRFIDCardlist(std::string cardid, std::string gwid, std::string loramacaddress, std::string datetime)
{
    // check existed
    bool bFound = false;
    for (std::list<RollCallCardStruct*>::iterator rollcallcard = mTempRFIDCardList.begin(); rollcallcard != mTempRFIDCardList.end(); rollcallcard++)
    {
        if ( cardid.compare((*rollcallcard)->cardid) == 0 )
        {
            (*rollcallcard)->gwid = gwid;
            (*rollcallcard)->loramacaddress = loramacaddress;
            (*rollcallcard)->datetime = datetime;

            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        RollCallCardStruct * rollcallCard = new RollCallCardStruct();
        rollcallCard->cardid = cardid;
        rollcallCard->gwid = gwid;
        rollcallCard->loramacaddress = loramacaddress;
        rollcallCard->rollcallcount = 0;
        rollcallCard->datetime = datetime;
        mTempRFIDCardList.push_back(rollcallCard);
    }

    return bFound;
}

bool CARDRollCallService::put_to_TempNFCCardlist(std::string cardid, std::string gwid, std::string loramacaddress, std::string datetime)
{
    // vilslog::printf ("CARDRollCallService::put_to_TempNFCCardlist() nfccard card cardid[%s] loramacaddress[%s]\n",
    //     cardid.c_str(), loramacaddress.c_str());

    // check existed
    bool bFound = false;
    for (std::list<RollCallCardStruct*>::iterator rollcallcard = mTempNFCCardList.begin(); rollcallcard != mTempNFCCardList.end(); rollcallcard++)
    {
        if ( cardid.compare((*rollcallcard)->cardid) == 0 )
        {
            (*rollcallcard)->gwid = gwid;
            (*rollcallcard)->loramacaddress = loramacaddress;
            (*rollcallcard)->datetime = datetime;

            bFound = true;
            break;
        }
    }

    // vilslog::printf ("CARDRollCallService::put_to_TempNFCCardlist() nfccard card bFound[%d]\n",  bFound);

    if (!bFound)
    {
        RollCallCardStruct * rollcallCard = new RollCallCardStruct();
        rollcallCard->cardid = cardid;
        rollcallCard->gwid = gwid;
        rollcallCard->loramacaddress = loramacaddress;
        rollcallCard->rollcallcount = 0;
        rollcallCard->datetime = datetime;
        mTempNFCCardList.push_back(rollcallCard);
    }

    return bFound;
}

bool CARDRollCallService::do_RFIDCard_section_0D_check(int projectid, std::string cardid, std::string loramacaddress)
{
    vilslog::printf ("CARDRollCallService::do_RFIDCard_section_0D_check() card cardid[%s] loramacaddress[%s]\n",
        cardid.c_str(), loramacaddress.c_str());

    for (std::vector<ProjectRollCallInfoStruct*>::iterator i = mProjectRollCallInfoVector.begin(); i != mProjectRollCallInfoVector.end(); i++)
    {
        for (std::list<SectionGroup *>::iterator sectiongroup = (*i)->mSection0DGroupList->begin(); sectiongroup != (*i)->mSection0DGroupList->end(); sectiongroup++)
        {
            std::string nodemacaddress = (*sectiongroup)->nodeIDList->front();

            if ( nodemacaddress.compare(loramacaddress) == 0)
            {
                RFIDCardStruct * rfidcard = mDeviceManager->get_RFIDCard(projectid, cardid.c_str());
                if (rfidcard != NULL)
                {
                    update_section0D(projectid, rfidcard->userid, (*sectiongroup)->maingroupid, (*sectiongroup)->AreaID, (*sectiongroup)->Param2);
                }
            }
        }
    }
    return true;
}

bool CARDRollCallService::do_NFCCard_section_0D_check(int projectid, std::string cardid, std::string loramacaddress)
{
    vilslog::printf ("CARDRollCallService::do_NFCCard_section_0D_check() card cardid[%s] loramacaddress[%s]\n",
        cardid.c_str(), loramacaddress.c_str());

    for (std::vector<ProjectRollCallInfoStruct*>::iterator i = mProjectRollCallInfoVector.begin(); i != mProjectRollCallInfoVector.end(); i++)
    {
        for (std::list<SectionGroup *>::iterator sectiongroup = (*i)->mSection0DGroupList->begin(); sectiongroup != (*i)->mSection0DGroupList->end(); sectiongroup++)
        {
            std::string nodemacaddress = (*sectiongroup)->nodeIDList->front();

            // vilslog::printf("CARDRollCallService::do_NFCCard_section_0D_check() AreaID[%s] maingroupid[%d]\n",
            //     (*sectiongroup)->AreaID.c_str(), (*sectiongroup)->maingroupid);

            if ( nodemacaddress.compare(loramacaddress) == 0)
            {
                NFCCardStruct * nfccard = mDeviceManager->get_NFCCard(projectid, cardid.c_str());
                if (nfccard != NULL)
                {
                    update_section0D(projectid, nfccard->userid, (*sectiongroup)->maingroupid, (*sectiongroup)->AreaID, (*sectiongroup)->Param2);
                }
            }
        }
    }

    return true;
}

int CARDRollCallService::update_section0D(int projectid, int userid, int maingroupid, std::string AreaID, int Param2)
{
    vilslog::printf ("CARDRollCallService::update_section0D() userid[%d] maingroupid[%d] AreaID[%s]\n",
        userid, maingroupid, AreaID.c_str());

    UserStruct * userRet = mDeviceManager->getUser(projectid, userid);
    if (userRet == NULL)
        return 1;

    bool bExisted = false;
    for (std::list<std::string>::iterator usermaingroupid = userRet->maingroups->begin(); usermaingroupid != userRet->maingroups->end(); usermaingroupid++)
    {
        int int_maingroupid = convert_string_to_int((*usermaingroupid));
        if ( maingroupid == int_maingroupid )
        {
            bExisted = true;
            break;
        }
    }

    vilslog::printf ("CARDRollCallService::update_section0D() user->maingroups->size()[%d] bExisted[%d]\n",
        userRet->maingroups->size(), bExisted);

    if(bExisted)
    {
        // check area
        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if (baseDb == NULL)
            return 0;

        MapLayerAreaStruct * area = baseDb->getAreaStruct(AreaID);
        if (area != NULL)
        {
            MapLayerStruct * maplayer = baseDb->getMapLayerStruct(area->layerid);
            AreaCenterStruct * areacenter = getAreaCenter(projectid, maplayer, area, userRet);

            // vilslog::printf ("CARDRollCallService::update_section0D() areacenter->center.x[%f] y[%f]\n",
            //     areacenter->center.x, areacenter->center.y);

            // WorkItem * witem = new WorkItem("updateUserPoistion", projectid);
            // witem->setUserId(userid);
            // witem->setPosition(areacenter->center.x, areacenter->center.y, 0);
            // mMySQLJobQueue.add(witem);
            // userRet->posX = areacenter->center.x;
            // userRet->posY = areacenter->center.y;
            // userRet->posZ = 0;
            // userRet->Latitude = "";
            // userRet->Longitude = "";
            // WorkItem * witem = new WorkItem("updateUserStruct", projectid, userRet);
            // mMySQLJobQueue.add(witem);

            mDeviceManager->setUserInfoArea0D(projectid, userid, area->areaid, Param2);
            // clear RollCallNotInArea info
            mDeviceManager->setUserRollCallNotInArea(projectid, userid, "");
            mDeviceManager->setUserInfoMaplayer(projectid, userid, area->layerid);
            mDeviceManager->setUserRangingResultType(projectid, userid, 0); // -1:unknown 0:0D 1:1D 2:2D 3:3D

            delete areacenter;
            delete maplayer;
            clear_PolygonStruct_list(area->Polygons);
            delete area->Polygons;
            delete area;
        }
    }

    return 0;
}

void CARDRollCallService::reloadRollCallList()
{
    std::list<int> * prjidList = mDbManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);

        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if (baseDb == NULL)
            continue;

        bool bFound = false;
        for (std::vector<ProjectRollCallInfoStruct*>::iterator i = mProjectRollCallInfoVector.begin(); i != mProjectRollCallInfoVector.end(); i++)
        {
            if ( (*i)->projectid == projectid )
            {
                clear_ProjectRollCallInfoStruct((*i));

                (*i)->mRollCallList = baseDb->read_CARDRollCallInfo();
                (*i)->mRFIDDeviceList = baseDb->get_RFIDDevices();
                (*i)->mNFCDeviceList = baseDb->get_NFCDevices();
                (*i)->mRollCallInstanceList = new std::list<RollCallInstance*>();
                (*i)->mTriggerList = create_Triggers();
                (*i)->mSection0DGroupList = new std::list<SectionGroup *>();
                baseDb->read_SectionGroupList(projectid, SECTION_TYPE_ZERO, (*i)->mSection0DGroupList);

                bFound = true;
                break;
            }
        }
        if (!bFound)
        {

            ProjectRollCallInfoStruct * newInfo = new ProjectRollCallInfoStruct();
            newInfo->projectid = projectid;
            newInfo->mRollCallList = baseDb->read_CARDRollCallInfo();
            newInfo->mRFIDDeviceList = baseDb->get_RFIDDevices();
            newInfo->mNFCDeviceList = baseDb->get_NFCDevices();
            newInfo->mRollCallInstanceList = new std::list<RollCallInstance*>();
            newInfo->mTriggerList = create_Triggers();
            newInfo->mSection0DGroupList = new std::list<SectionGroup *>();
            baseDb->read_SectionGroupList(projectid, SECTION_TYPE_ZERO, newInfo->mSection0DGroupList);
            // vilslog::printf ("CARDRollCallService()::initProjectRollCallInfo() projectid[%d] mRollCallList->size()[%d]\n",
            //     projectid, (int)newInfo->mRollCallList->size());

            mProjectRollCallInfoVector.push_back(newInfo);
        }
    }

    prjidList->clear();
    delete prjidList;
}

bool CARDRollCallService::reloadSectionList()
{
    std::list<int> * prjidList = mDbManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);

        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if (baseDb == NULL)
            continue;

        for (std::vector<ProjectRollCallInfoStruct*>::iterator i = mProjectRollCallInfoVector.begin(); i != mProjectRollCallInfoVector.end(); i++)
        {
            if ( (*i)->projectid == projectid )
            {
                std::list<SectionGroup *> * oldSection0DGroupList = (*i)->mSection0DGroupList;

                (*i)->mSection0DGroupList = new std::list<SectionGroup *>();
                baseDb->read_SectionGroupList(projectid, SECTION_TYPE_ZERO, (*i)->mSection0DGroupList);

                // release old one
                if (oldSection0DGroupList != NULL)
                {
                    clear_SectionGroup_list(oldSection0DGroupList);
                    delete oldSection0DGroupList;
                }

                break;
            }
        }
    }

    prjidList->clear();
    delete prjidList;

    return true;
}

bool CARDRollCallService::send_MQTT_cmd(int projectid, std::string cmd, RFIDDeviceStruct * device)
{
    std::ostringstream sendstreamData;
    sendstreamData << "[{\"cmd\":\""<< cmd <<"\",\"time\":\"" << currentDateTimeForSQL() << "\"}]";
    std::string payloadData = sendstreamData.str();
    std::string hexDataString = StringToHEXString(payloadData);

    std::ostringstream payloadstream;
    payloadstream << "[{\"macAddr\":\"";
    payloadstream << device->LoRAMacaddress;
    payloadstream << "\",\"data\":\"";
    payloadstream << hexDataString;
    payloadstream << "\",\"id\":\"";
    payloadstream << random_string(16);
    payloadstream << "\",\"extra\":{\"port\":0,\"txpara\":\"22\"}}]";

    std::string payload = payloadstream.str();

    // vilslog::printf("CARDRollCallService::send_MQTT_cmd() projectid[%d] payload[%s] gwid[%s]\n", projectid, payload.c_str(), device->gwid.c_str());

    MQTTDataItem * mqttitem = new MQTTDataItem("MQTTpublish", projectid, device->gwid.c_str(), payload.c_str(), (int)payload.length());
    mMQTTPublishQueue.add(mqttitem);

    return true;
}

bool CARDRollCallService::send_MQTT_cmd(int projectid, std::string cmd, NFCDeviceStruct * device)
{
    std::ostringstream sendstreamData;
    sendstreamData << "[{\"cmd\":\""<< cmd <<"\",\"time\":\"" << currentDateTimeForSQL() << "\"}]";
    std::string payloadData = sendstreamData.str();
    std::string hexDataString = StringToHEXString(payloadData);

    std::ostringstream payloadstream;
    payloadstream << "[{\"macAddr\":\"";
    payloadstream << device->LoRAMacaddress;
    payloadstream << "\",\"data\":\"";
    payloadstream << hexDataString;
    payloadstream << "\",\"id\":\"";
    payloadstream << random_string(16);
    payloadstream << "\",\"extra\":{\"port\":0,\"txpara\":\"22\"}}]";

    std::string payload = payloadstream.str();

    // vilslog::printf("CARDRollCallService::send_MQTT_cmd() projectid[%d] payload[%s] gwid[%s]\n", projectid, payload.c_str(), device->gwid.c_str());

    MQTTDataItem * mqttitem = new MQTTDataItem("MQTTpublish", projectid, device->gwid.c_str(), payload.c_str(), (int)payload.length());
    mMQTTPublishQueue.add(mqttitem);

    return true;
}

void CARDRollCallService::clear_ProjectRollCallInfoStruct_vector(std::vector<ProjectRollCallInfoStruct*> * plist)
{
    for (std::vector<ProjectRollCallInfoStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        clear_ProjectRollCallInfoStruct((*i));
        delete (*i);
    }
    plist->clear();
}

void CARDRollCallService::clear_ProjectRollCallInfoStruct(ProjectRollCallInfoStruct * info)
{
    if (info->mRollCallInstanceList != NULL)
    {
        clear_RollCallInstance_list(info->mRollCallInstanceList);
        delete info->mRollCallInstanceList;
        info->mRollCallInstanceList = NULL;
    }
    if (info->mRollCallList != NULL)
    {
        clear_CARDRollCallStruct_list(info->mRollCallList);
        delete info->mRollCallList;
        info->mRollCallList = NULL;
    }
    if (info->mRFIDDeviceList != NULL)
    {
        clear_RFIDDeviceStruct_list(info->mRFIDDeviceList);
        delete info->mRFIDDeviceList;
        info->mRFIDDeviceList = NULL;
    }
    if (info->mNFCDeviceList != NULL)
    {
        clear_NFCDeviceStruct_list(info->mNFCDeviceList);
        delete info->mNFCDeviceList;
        info->mNFCDeviceList = NULL;
    }
    if (info->mTriggerList != NULL)
    {
        clear_CARDRollCallTriggerStruct_list(info->mTriggerList);
        delete info->mTriggerList;
        info->mTriggerList = NULL;
    }
    if (info->mSection0DGroupList != NULL)
    {
        clear_SectionGroup_list(info->mSection0DGroupList);
        delete info->mSection0DGroupList;
        info->mSection0DGroupList = NULL;
    }
}

void CARDRollCallService::clear_RollCallInstance_list(std::list<RollCallInstance*> * plist)
{
    for (std::list<RollCallInstance*>::iterator i = plist->begin(); i != plist->end(); i++)
    {

        if ((*i)->mUserCardList != NULL)
        {
            (*i)->mUserCardList->clear();
            delete (*i)->mUserCardList;
        }

        // if ((*i)->mLLRPContollerList != NULL)
        // {
        //     for (std::list<LLRPController*>::iterator ctrl = (*i)->mLLRPContollerList->begin(); ctrl != (*i)->mLLRPContollerList->end(); ctrl++)
        //     {
        //         (*ctrl)->closeReader();
        //         delete (*ctrl);
        //     }
        //     (*i)->mLLRPContollerList->clear();
        //     delete (*i)->mLLRPContollerList;
        // }

        if ((*i)->mRollCallRecordList != NULL)
        {
            for (std::list<CARDRollCallRecordStruct*>::iterator record = (*i)->mRollCallRecordList->begin(); record != (*i)->mRollCallRecordList->end(); record++)
            {
                (*record)->rfiddeviceids.clear();
                (*record)->nfclocators.clear();
                delete (*record);
            }
            (*i)->mRollCallRecordList->clear();
            delete (*i)->mRollCallRecordList;
        }

        if ((*i)->mRollCallSubGroupList != NULL)
        {
            clear_CARDRollCallSubGroupEventStruct_list((*i)->mRollCallSubGroupList);
            (*i)->mRollCallSubGroupList->clear();
            delete (*i)->mRollCallSubGroupList;
        }

        delete (*i)->mRollCallEvent;

        if ((*i)->mTrigger != NULL)
        {
            clear_CARDRollCallTriggerItemStruct_list((*i)->mTrigger->triggeritems);
            delete (*i)->mTrigger->triggeritems;
            delete (*i)->mTrigger;
        }

        delete (*i);
    }
    plist->clear();
}

void CARDRollCallService::clear_CARDRollCallStruct_list(std::list<CARDRollCallStruct*> * plist)
{
    for (std::list<CARDRollCallStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        (*i)->rfiddeviceids.clear();
        (*i)->nfclocators.clear();
        (*i)->subgroups.clear();
        (*i)->weekday.clear();
        (*i)->validdate.clear();
        (*i)->invaliddate.clear();
        delete (*i);
    }
    plist->clear();
}

void CARDRollCallService::clear_RFIDDeviceStruct_list(std::list<RFIDDeviceStruct*> * plist)
{
    for (std::list<RFIDDeviceStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete (*i);
    }
    plist->clear();
}

void CARDRollCallService::clear_UserCardStruct_list(std::list<UserCardStruct*> * plist)
{
    for (std::list<UserCardStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        (*i)->rfidcards.clear();
        (*i)->nfccards.clear();
        delete (*i);
    }
    plist->clear();
}

void CARDRollCallService::clear_RollCallCardStruct_list(std::list<RollCallCardStruct*> * plist)
{
    for (std::list<RollCallCardStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete (*i);
    }
    plist->clear();
}

std::list<CARDRollCallTriggerStruct*> * CARDRollCallService::create_Triggers()
{
    std::list<CARDRollCallTriggerStruct*> * triggerList = new std::list<CARDRollCallTriggerStruct*>();

    CARDRollCallTriggerStruct * trigger_1 = create_Trigger_1();
    triggerList->push_back(trigger_1);

    CARDRollCallTriggerStruct * trigger_2 = create_Trigger_2();
    triggerList->push_back(trigger_2);

    // CARDRollCallTriggerStruct * trigger_3 = create_Trigger_3();
    // triggerList->push_back(trigger_3);

    return triggerList;
}

CARDRollCallTriggerStruct * CARDRollCallService::create_Trigger_1()
{
    CARDRollCallTriggerStruct * trigger = new CARDRollCallTriggerStruct();
    trigger->TargetRollCallName = "到校點名";
    trigger->TotalMean = 740;
    trigger->triggeritems = new std::list<CARDRollCallTriggerItemStruct*>();

    trigger->triggeritems->push_back(create_Trigger_item("06:30", 0.00f));
    trigger->triggeritems->push_back(create_Trigger_item("06:35", 0.03f));
    trigger->triggeritems->push_back(create_Trigger_item("06:40", 0.08f));
    trigger->triggeritems->push_back(create_Trigger_item("06:45", 0.39f));
    trigger->triggeritems->push_back(create_Trigger_item("06:50", 0.83f));
    trigger->triggeritems->push_back(create_Trigger_item("06:55", 1.46f));
    trigger->triggeritems->push_back(create_Trigger_item("07:00", 6.35f));
    trigger->triggeritems->push_back(create_Trigger_item("07:05", 8.58f));
    trigger->triggeritems->push_back(create_Trigger_item("07:10", 13.13f));
    trigger->triggeritems->push_back(create_Trigger_item("07:15", 16.16f));
    trigger->triggeritems->push_back(create_Trigger_item("07:20", 18.16f));
    trigger->triggeritems->push_back(create_Trigger_item("07:25", 15.13f));
    trigger->triggeritems->push_back(create_Trigger_item("07:30", 10.58f));
    trigger->triggeritems->push_back(create_Trigger_item("07:35", 4.35f));
    trigger->triggeritems->push_back(create_Trigger_item("07:40", 3.46f));
    trigger->triggeritems->push_back(create_Trigger_item("07:45", 1.03f));
    trigger->triggeritems->push_back(create_Trigger_item("07:50", 0.19f));
    trigger->triggeritems->push_back(create_Trigger_item("07:55", 0.06f));
    trigger->triggeritems->push_back(create_Trigger_item("08:00", 0.01f));

    return trigger;
}

CARDRollCallTriggerStruct * CARDRollCallService::create_Trigger_2()
{
    CARDRollCallTriggerStruct * trigger = new CARDRollCallTriggerStruct();
    trigger->TargetRollCallName = "離校點名";
    trigger->TotalMean = 630;
    trigger->triggeritems = new std::list<CARDRollCallTriggerItemStruct*>();

    trigger->triggeritems->push_back(create_Trigger_item("16:30", 0.00f));
    trigger->triggeritems->push_back(create_Trigger_item("16:35", 0.02f));
    trigger->triggeritems->push_back(create_Trigger_item("16:40", 0.07f));
    trigger->triggeritems->push_back(create_Trigger_item("16:45", 0.29f));
    trigger->triggeritems->push_back(create_Trigger_item("16:50", 0.93f));
    trigger->triggeritems->push_back(create_Trigger_item("16:55", 1.46f));
    trigger->triggeritems->push_back(create_Trigger_item("17:00", 4.35f));
    trigger->triggeritems->push_back(create_Trigger_item("17:05", 11.58f));
    trigger->triggeritems->push_back(create_Trigger_item("17:10", 17.13f));
    trigger->triggeritems->push_back(create_Trigger_item("17:15", 19.16f));
    trigger->triggeritems->push_back(create_Trigger_item("17:20", 15.16f));
    trigger->triggeritems->push_back(create_Trigger_item("17:25", 11.13f));
    trigger->triggeritems->push_back(create_Trigger_item("17:30", 9.58f));
    trigger->triggeritems->push_back(create_Trigger_item("17:35", 6.35f));
    trigger->triggeritems->push_back(create_Trigger_item("17:40", 1.46f));
    trigger->triggeritems->push_back(create_Trigger_item("17:45", 0.93f));
    trigger->triggeritems->push_back(create_Trigger_item("17:50", 0.29f));
    trigger->triggeritems->push_back(create_Trigger_item("17:55", 0.07f));
    trigger->triggeritems->push_back(create_Trigger_item("18:00", 0.02f));

    return trigger;
}

CARDRollCallTriggerStruct * CARDRollCallService::create_Trigger_3()
{
    CARDRollCallTriggerStruct * trigger = new CARDRollCallTriggerStruct();
    trigger->TargetRollCallName = "體育課";
    trigger->TotalMean = 630;
    trigger->triggeritems = new std::list<CARDRollCallTriggerItemStruct*>();

    trigger->triggeritems->push_back(create_Trigger_item("14:40", 0.00f));
    trigger->triggeritems->push_back(create_Trigger_item("14:41", 0.02f));
    trigger->triggeritems->push_back(create_Trigger_item("14:42", 0.07f));
    trigger->triggeritems->push_back(create_Trigger_item("14:43", 0.29f));
    trigger->triggeritems->push_back(create_Trigger_item("14:44", 0.93f));
    trigger->triggeritems->push_back(create_Trigger_item("14:45", 2.46f));
    trigger->triggeritems->push_back(create_Trigger_item("14:46", 5.35f));
    trigger->triggeritems->push_back(create_Trigger_item("14:47", 9.58f));
    trigger->triggeritems->push_back(create_Trigger_item("14:48", 14.13f));
    trigger->triggeritems->push_back(create_Trigger_item("14:49", 17.16f));
    trigger->triggeritems->push_back(create_Trigger_item("14:50", 17.16f));
    trigger->triggeritems->push_back(create_Trigger_item("14:51", 14.13f));
    trigger->triggeritems->push_back(create_Trigger_item("14:52", 9.58f));
    trigger->triggeritems->push_back(create_Trigger_item("14:53", 5.35f));
    trigger->triggeritems->push_back(create_Trigger_item("14:54", 2.46f));
    trigger->triggeritems->push_back(create_Trigger_item("14:55", 0.93f));
    trigger->triggeritems->push_back(create_Trigger_item("14:56", 0.29f));
    trigger->triggeritems->push_back(create_Trigger_item("14:57", 0.07f));
    trigger->triggeritems->push_back(create_Trigger_item("14:58", 0.02f));

    return trigger;
}

CARDRollCallTriggerItemStruct * CARDRollCallService::create_Trigger_item(std::string TargetTime, float NormDist)
{
    CARDRollCallTriggerItemStruct * item = new CARDRollCallTriggerItemStruct();
    item->TargetTime = TargetTime;
    item->NormDist = NormDist;
    item->Triggered = 0;
    item->ReportMean = 0;
    return item;
}

CARDRollCallTriggerStruct * CARDRollCallService::find_trigger(ProjectRollCallInfoStruct * prjRollCallInfo, std::string rollcallname)
{
    CARDRollCallTriggerStruct * retTrigger = NULL;

    for (std::list<CARDRollCallTriggerStruct*>::iterator trigger = prjRollCallInfo->mTriggerList->begin(); trigger != prjRollCallInfo->mTriggerList->end(); trigger++)
    {
        if ( rollcallname.compare((*trigger)->TargetRollCallName) == 0 )
        {
            //
            // copy
            //
            retTrigger = new CARDRollCallTriggerStruct();
            retTrigger->TargetRollCallName = (*trigger)->TargetRollCallName;
            retTrigger->TotalMean = (*trigger)->TotalMean;
            retTrigger->triggeritems = new std::list<CARDRollCallTriggerItemStruct*>();

            vilslog::printf("CARDRollCallService::find_trigger() found TargetRollCallName[%s] TotalMean[%d]\n", (*trigger)->TargetRollCallName.c_str(), (*trigger)->TotalMean);

            for (std::list<CARDRollCallTriggerItemStruct*>::iterator item = (*trigger)->triggeritems->begin(); item != (*trigger)->triggeritems->end(); item++)
            {
                retTrigger->triggeritems->push_back(create_Trigger_item((*item)->TargetTime, (*item)->NormDist));
            }

            break;
        }
    }
    return retTrigger;
}

bool CARDRollCallService::set_subgroupevent(UserStruct * user, std::list<CARDRollCallSubGroupEventStruct*> * rollcallSubGroupList)
{
    bool bFound = false;
    for (std::list<std::string>::iterator subgroupstr = user->subgroups->begin(); subgroupstr != user->subgroups->end(); subgroupstr++)
    {
        int usersubgroupid = convert_string_to_int((*subgroupstr));

        for (std::list<CARDRollCallSubGroupEventStruct *>::iterator subgroupevent = rollcallSubGroupList->begin(); subgroupevent != rollcallSubGroupList->end(); subgroupevent++)
        {
            if ( (*subgroupevent)->subgroupid == usersubgroupid )
            {
                (*subgroupevent)->userids.push_back(user->userid);
                (*subgroupevent)->totalcount++;
                bFound = true;
                break;
            }
        }

    }
    return bFound;
}

bool CARDRollCallService::set_present_subgroupevent(int userid, std::list<CARDRollCallSubGroupEventStruct*> * rollcallSubGroupList)
{
    bool bFound = false;
    for (std::list<CARDRollCallSubGroupEventStruct *>::iterator subgroupevent = rollcallSubGroupList->begin(); subgroupevent != rollcallSubGroupList->end(); subgroupevent++)
    {
        for (std::list<int>::iterator subgroup_userid = (*subgroupevent)->userids.begin(); subgroup_userid != (*subgroupevent)->userids.end(); subgroup_userid++)
        {
            if ( userid == (*subgroup_userid) )
            {
                //
                // check if exited
                //
                bool bExited = false;
                for (std::list<int>::iterator present_userid = (*subgroupevent)->presentuserids.begin(); present_userid != (*subgroupevent)->presentuserids.end(); present_userid++)
                {
                    if ( userid == (*present_userid) )
                    {
                        // already exited!
                        bExited = true;
                        break;
                    }
                }
                if(!bExited)
                {
                    (*subgroupevent)->presentuserids.push_back(userid);
                }

                bFound = true;
                break;
            }
        }
    }

    return bFound;
}

AreaCenterStruct * CARDRollCallService::getAreaCenter(int projectid, MapLayerStruct * maplayer, MapLayerAreaStruct * area, UserStruct * userRet)
{
    int count = 0;
    double WGS48_xi = 0.0;
    double WGS48_yi = 0.0;

    // vilslog::printf("CARDRollCallService::getAreaCenter() maplayer->WGS48OriginX[%s] WGS48OriginY[%s]\n",
    //     maplayer->WGS48OriginX.c_str(), maplayer->WGS48OriginY.c_str());

    double WGS48_Origin_X = std::stod(maplayer->WGS48OriginX);
    double WGS48_Origin_Y = std::stod(maplayer->WGS48OriginY);

    // vilslog::printf("CARDRollCallService::getAreaCenter() WGS48_Origin_X[%f] WGS48_Origin_Y[%f]\n",
    //     WGS48_Origin_X, WGS48_Origin_Y);

    for (list <PolygonStruct *>::iterator Polygon = area->Polygons->begin(); Polygon != area->Polygons->end(); Polygon++)
    {
        for (list <PointStruct *>::iterator point = (*Polygon)->coordinates->begin(); point != (*Polygon)->coordinates->end(); point++)
        {
            WGS48_xi += (*point)->x;
            WGS48_yi += (*point)->y;
            count++;
        }
    }

    // vilslog::printf("CARDRollCallService::getAreaCenter() WGS48_xi[%f] WGS48_yi[%f] count[%d]\n",
    //     WGS48_xi, WGS48_yi, count);

    double WGS48_xi_center = WGS48_xi / (double)count;
    double WGS48_yi_center = WGS48_yi / (double)count;

    double WGS48_xi_center_diff_orig = WGS48_xi_center - WGS48_Origin_X;
    double WGS48_yi_center_diff_orig = WGS48_yi_center - WGS48_Origin_Y;

    // vilslog::printf("CARDRollCallService::getAreaCenter() WGS48_xi_center[%f] WGS48_yi_center[%f]\n",
    //     WGS48_xi_center, WGS48_yi_center);
    // vilslog::printf("CARDRollCallService::getAreaCenter() WGS48_xi_center_diff_orig[%f] WGS48_yi_center_diff_orig[%f]\n",
    //     WGS48_xi_center_diff_orig, WGS48_yi_center_diff_orig);

    AreaCenterStruct * areacenter = new AreaCenterStruct();
    areacenter->TagCount = 0;
    areacenter->areaid = area->areaid;
    areacenter->center.x = LongitudeToCentimeter(WGS48_yi_center, WGS48_xi_center_diff_orig);
    areacenter->center.y = LatitudeToCentimeter(WGS48_yi_center_diff_orig);

    int newPosition[3] = {(int)areacenter->center.x, (int)areacenter->center.y, 0};
    find_empty_0D_position(projectid, userRet->userid, (int)areacenter->center.x, (int)areacenter->center.y, newPosition);

    int xDiff = newPosition[0] - areacenter->center.x;
    int yDiff = newPosition[1] - areacenter->center.y;

    double lat_diff = centimeterToLatitude(yDiff);
    double lon_diff = centimeterToLongitude(WGS48_yi_center_diff_orig, xDiff);


    userRet->posX = newPosition[0];
    userRet->posY = newPosition[1];
    userRet->posZ = newPosition[2];
    userRet->Latitude = std::to_string(WGS48_yi_center + lat_diff);
    userRet->Longitude = std::to_string(WGS48_xi_center + lon_diff);
    userRet->AreaId = area->areaid;
    userRet->AreaName = area->areaname;
    WorkItem * witem = new WorkItem("updateUserStruct", projectid, userRet);
    mMySQLJobQueue.add(witem);

    // vilslog::printf("CARDRollCallService::getAreaCenter() areacenter->center.x[%f] areacenter->center.y[%f] Latitude[%s] Longitude[%s]\n",
    //     areacenter->center.x, areacenter->center.y, userRet->Latitude.c_str(), userRet->Longitude.c_str());

    mDeviceManager->setUserInfoArea(projectid, userRet->userid, area->areaid, area->areaname);
    mDeviceManager->setUserInfoPos(projectid, userRet->userid, newPosition[0], newPosition[1], newPosition[2]);

    return areacenter;
}

bool CARDRollCallService::find_empty_0D_position(int projectid, int userid, int startX, int startY, int * newPosition)
{
    bool bFound = false;
    int showCol = 3;
    int starIndex = -1;

    // vilslog::printf("find_empty_0D_position() TagId[%s] startX[%d] startY[%d]\n", TagId, startX, startY);

    int ListIndex = 0;
    while (!bFound)
    {
        int offset_num = ListIndex / 9;
        int ShowIndex = ListIndex - (offset_num * 9);

        int rowIndex = ShowIndex / showCol;
        int show_row = starIndex + rowIndex; // col size = 3
        int show_col = starIndex + (ShowIndex - (rowIndex * showCol));

        int offset_x = offset_num * 10;
        int offset_row = show_row * 80; // 80 cm
        int offset_col = show_col * 80 + offset_x; // 80 cm
        int curX = startX + offset_col;
        int curY = startY + offset_row;

        // vilslog::printf("find_empty_0D_position() curX[%d] curY[%d] offset_col[%d] offset_row[%d]\n", curX, curY, offset_col, offset_row);

        bool bExist = false;
        std::list<UserStruct*> * userList = mDeviceManager->get_User_list(projectid);
        for (std::list<UserStruct*>::iterator user = userList->begin(); user != userList->end(); user++)
        {
            if ( (curX == (*user)->posX) && (curY == (*user)->posY) && ( (*user)->userid != userid ) )
            {
                bExist = true;
                break;
            }
        }

        if (!bExist)
        {
            newPosition[0] = curX;
            newPosition[1] = curY;
            bFound = true;
            break;
        }
        ListIndex++;
    }

    return bFound ;
}
