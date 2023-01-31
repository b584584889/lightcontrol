/*
 *  FireFightingService.cpp
 *
 *  Created on: 2021/08/25
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

#include "FireFightingService.h"

using namespace std;

FireFightingService::FireFightingService(DeviceManager * pDevManager, DBManager * dbManager)
    : mDeviceManager(pDevManager), mDbManager(dbManager)
{
}

FireFightingService::~FireFightingService()
{
}

bool FireFightingService::status()
{
    return mInitial;
}

std::string FireFightingService::statusString()
{
    return mStatus;
}

void * FireFightingService::run()
{
    while(1)
    {
        try
        {
            initQueue();
            runloop();
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("FireFightingService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "FireFightingService()::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void FireFightingService::initQueue()
{
    std::string dft_rtn = "none";
    int queue_size1 = 10000;

    mSQLJobPublisher = new ITPS::Publisher<WorkItem*>("SQLServer", "save");
    mWebSocketEventPublisher = new ITPS::Publisher<SubscribeDataItem*>("WebSocket", "event");

    mFireFightingSubscriber = new ITPS::Subscriber<std::string>("FireFighting", "event", queue_size1);
    mNFCCardSubscriber = new ITPS::Subscriber<DataItem*>("NFCCard", "scan", queue_size1);
    mAoARangingSubscriber = new ITPS::Subscriber<DataItem*>("AoA", "ranging", queue_size1);
    mTagActivitySubscriber = new ITPS::Subscriber<DataItem*>("Tag", "Activity", queue_size1);
    mSensorBodyInfoSubscriber = new ITPS::Subscriber<WorkItem*>("Sensor", "BodyInfo", queue_size1);

    while(!mFireFightingSubscriber->subscribe())
    {
        vilslog::printf ("FireFightingService()::waitloop() mFireFightingSubscriber->subscribe()\n");
        usleep(10000);
    }

    while(!mNFCCardSubscriber->subscribe())
    {
        vilslog::printf ("FireFightingService()::waitloop() mNFCCardSubscriber->subscribe()\n");
        usleep(10000);
    }

    while(!mAoARangingSubscriber->subscribe())
    {
        vilslog::printf ("FireFightingService()::waitloop() mAoARangingSubscriber->subscribe()\n");
        usleep(10000);
    }

    while(!mTagActivitySubscriber->subscribe())
    {
        vilslog::printf ("FireFightingService()::waitloop() mTagActivitySubscriber->subscribe()\n");
        usleep(10000);
    }

    while( !mSensorBodyInfoSubscriber->subscribe() )
    {
        vilslog::printf ("FireFightingService()::waitloop() mSensorBodyInfoSubscriber->subscribe()\n");
        usleep(10000);
    }
}

void FireFightingService::runloop()
{
    long long start1 = getSystemTime();
    long long end1 = getSystemTime();
    long long difft = end1 - start1;
    long long reloadCaseTime = 0;

    std::string dft_event = "none";
    DataItem * dft_rtn = NULL;
    WorkItem * dft_rtnw = NULL;
    int count = 0;

    mInitial = true;
    mReloadAll = false;

    mStatus = "FireFightingService()::run() Initial Courses successfully!";
    vilslog::printf ("FireFightingService()::run() Initial Courses successfully!\n");

    initProjectCaseInfo();

    while (1)
    {
        DataItem * dataItem = mNFCCardSubscriber->pop_msg(1, dft_rtn);
        if ( dataItem != NULL )
        {
            count++;
            processData(dataItem);

            delete dataItem;
        }

        DataItem * dataItem1 = mAoARangingSubscriber->pop_msg(1, dft_rtn);
        if ( dataItem1 != NULL )
        {
            count++;
            processData(dataItem1);

            delete dataItem1;
        }

        DataItem * dataItem2 = mTagActivitySubscriber->pop_msg(1, dft_rtn);
        if ( dataItem2 != NULL )
        {
            count++;
            processData(dataItem2);

            delete dataItem2;
        }

        WorkItem * workItem = mSensorBodyInfoSubscriber->pop_msg(1, dft_rtnw);
        if ( workItem != NULL )
        {
            count++;
            processData(workItem);

            delete workItem;
        }

        std::string event = mFireFightingSubscriber->pop_msg(1, dft_event);
        if ( event.compare(dft_event) != 0 )
        {
            count++;
            processFireFightingEvent(event);
        }

        end1 = getSystemTime();
        difft = end1 - start1;

        reloadCaseTime += difft;

        if ( reloadCaseTime >= 600000 || mReloadAll) // 600 seconds
        {
            reloadCaseList();
            reloadCaseTime = 0;
            mReloadAll = false;
        }

        start1 = end1;

        if (count == 0)
        {
            usleep(100000);
        }
        count = 0;
    }
}

void FireFightingService::processData(DataItem * dataItem)
{
    // vilslog::printf ("FireFightingService::processData() type[%s]\n", dataItem->getType());

    if ( strcmp(dataItem->getType(), "NFCCard") == 0 )
    {
        int projectid = dataItem->getProjectID();
        std::string cardid = dataItem->getCardid();
        std::string gwid = dataItem->getGatewayid();
        std::string loramacaddress = dataItem->getLoRAMacaddress();

        // vilslog::printf("FireFightingService::processData() NFCCard cardid[%s] gwid[%s] loramacaddress[%s]\n",
        //     cardid.c_str(), gwid.c_str(), loramacaddress.c_str());

        updateCaseUserNFCCardAction(projectid, loramacaddress, cardid);

    }
    else
    if ( strcmp(dataItem->getType(), "lorarangingcompressaoa") == 0 )
    {
        int projectid = dataItem->getProjectID();
        int Distance = dataItem->getDistance();
        std::string macaddress = dataItem->getmacaddress();
        std::string loramacaddress = dataItem->getLoRAMacaddress();
        float angle = dataItem->getAngle();

        // vilslog::printf("FireFightingService::processData() AoA macaddress[%s] Distance[%d] loramacaddress[%s] angle[%f]\n",
        //     macaddress.c_str(), Distance, loramacaddress.c_str(), angle);

        updateCaseUserAoAAction(projectid, loramacaddress, macaddress, Distance, angle);
    }
    else
    if ( strcmp(dataItem->getType(), "HAR") == 0 )
    {
        int projectid = dataItem->getProjectID();
        std::string message = dataItem->getMessage();
        std::string macaddress = dataItem->getmacaddress();

        // vilslog::printf ("FireFightingService::processData() type[%s] projectid[%d] message[%s] macaddress[%s]\n",
        //     dataItem->getType(), projectid, message.c_str(), macaddress.c_str());

        updateCaseUserActivity(projectid, macaddress, message);
    }
}

void FireFightingService::processData(WorkItem* dataItem)
{
    // vilslog::printf ("FireFightingService::processData() type[%s]\n", dataItem->getType());

    if ( strcmp(dataItem->getType(), "sensordevice") == 0 )
    {
        int projectid = dataItem->getProjectId();
        std::string sensorid = dataItem->mSensorDevice.id;
        std::string jsonvalue = dataItem->mSensorDevice.jsonvalue;

        // vilslog::printf ("FireFightingService::processData() sensorid[%s] jsonvalue[%s]\n", sensorid.c_str(), jsonvalue.c_str());

        Json::Value result;
        bool parsingSuccessful = parse_JSON(jsonvalue.c_str(), result);

        if ( parsingSuccessful )
        {
            std::string jHR = result["HR"].asString();
            std::string jSPO2 = result["SPO2"].asString();
            std::string jBattery = result["Battery"].asString();

            updateCaseUserBodyInfo(projectid, sensorid, jHR, jSPO2, jBattery);
        }
    }
}

void FireFightingService::processFireFightingEvent(std::string event)
{
    Json::Value result;
    bool parsingSuccessful = parse_JSON(event, result);

    // vilslog::printf("FireFightingService::processFireFightingEvent() parsingSuccessful[%d]\n", parsingSuccessful);

    if ( parsingSuccessful )
    {
        std::string casenumber = "";
        std::string event = "";

        int projectid = getProjectId(result);
        if ( result.isMember("casenumber") )
            casenumber = result["casenumber"].asString();
        if ( result.isMember("event") )
            event = result["event"].asString();

        if( event.compare("updatedeviceinfo") == 0 )
        {
            std::string NFCDeviceIds = "";
            std::string StandbyDeviceIds = "";
            // std::string AoADeviceIds = "";
            if ( result.isMember("NFCDeviceIds") )
                NFCDeviceIds = result["NFCDeviceIds"].asString();
            if ( result.isMember("StandbyDeviceIds") )
                StandbyDeviceIds = result["StandbyDeviceIds"].asString();
            // if ( result.isMember("AoADeviceIds") )
            //     AoADeviceIds = result["AoADeviceIds"].asString();

            vilslog::printf("FireFightingService::processFireFightingEvent() updatedeviceinfo projectid[%d] casenumber[%s] StandbyDeviceIds[%s] NFCDeviceIds[%s]\n",
                projectid, casenumber.c_str(), StandbyDeviceIds.c_str(), NFCDeviceIds.c_str());

            updateCaseDeviceIds(projectid, casenumber, StandbyDeviceIds, NFCDeviceIds);
        }
        else
        if( event.compare("updateuserairremain") == 0 )
        {
            int userid = 0;
            float airremain = 0;
            if ( result.isMember("userid") )
                userid = stoi(result["userid"].asString());
            if ( result.isMember("airremain") )
                airremain = stof(result["airremain"].asString());

            vilslog::printf("FireFightingService::processFireFightingEvent() updatedeviceinfo projectid[%d] casenumber[%s] userid[%d] airremain[%.02f]\n",
                projectid, casenumber.c_str(), userid, airremain);

            updateCaseUserAirRemain(projectid, casenumber, userid, airremain);
        }
        else
        if( event.compare("updateuserarea") == 0 )
        {
            int userid = 0;
            int areaType = 0;
            if ( result.isMember("userid") )
                userid = stoi(result["userid"].asString());
            if ( result.isMember("areaType") )
                areaType = stoi(result["areaType"].asString());

            vilslog::printf("FireFightingService::processFireFightingEvent() updateuserarea projectid[%d] casenumber[%s] userid[%d] areaType[%d]\n",
                projectid, casenumber.c_str(), userid, areaType);

            updateCaseUserArea(projectid, casenumber, areaType, userid);
        }
        else
        if( event.compare("newcase") == 0 )
        {
            mReloadAll = true;
        }
        else
        if( event.compare("finishcase") == 0 )
        {
            mReloadAll = true;

            int calcairusing = 0;
            if ( result.isMember("calcairusing") )
                calcairusing = stoi(result["calcairusing"].asString());

            vilslog::printf("FireFightingService::processFireFightingEvent() finishcase projectid[%d] casenumber[%s] calcairusing[%d]\n",
                projectid, casenumber.c_str(), calcairusing);

            finishCase(projectid, casenumber, calcairusing);
        }
    }
}

void FireFightingService::updateCaseDeviceIds(int projectid, std::string casenumber, std::string StandbyDeviceIds, std::string NFCDeviceIds)
{
    std::list<FireFightingCaseStruct*> * caseList = getProjectCaseList(projectid);
    if (caseList == NULL)
    {
        return;
    }

    for (std::list<FireFightingCaseStruct*>::iterator ffCase = caseList->begin(); ffCase != caseList->end(); ffCase++)
    {
        if ( (*ffCase)->mFireFighting->casenumber.compare(casenumber) == 0 )
        {
            (*ffCase)->mFireFighting->StandbyDeviceIds = convert_string_to_list(StandbyDeviceIds, ',');
            (*ffCase)->mFireFighting->NFCDeviceIdList = convert_string_to_intlist(NFCDeviceIds, ',');

            sendOutInfo(projectid, "FireFightingInfo", (*ffCase)->mFireFighting);

            break;
        }
    }
}

void FireFightingService::updateCaseUserNFCCardAction(int projectid, std::string loramacaddress, std::string cardid)
{
    // vilslog::printf("FireFightingService::updateCaseUserNFCCardAction() nfcdevice loramacaddress[%s] cardid[%s]\n", loramacaddress.c_str(), cardid.c_str());

    NFCDeviceStruct * nfcdevice = mDeviceManager->get_NFCDeviceByLoRAmacaddress(projectid, loramacaddress.c_str());
    if (nfcdevice == NULL)
    {
        return;
    }

    NFCCardStruct * nfccard = mDeviceManager->get_NFCCard(projectid, cardid.c_str());
    if (nfccard == NULL)
    {
        return;
    }
    // vilslog::printf("FireFightingService::updateCaseUserNFCCardAction() nfcdevice deviceid[%d]\n", nfcdevice->deviceid);

    std::list<FireFightingCaseStruct*> * caseList = getProjectCaseList(projectid);
    if (caseList == NULL)
    {
        return;
    }

    for (std::list<FireFightingCaseStruct*>::iterator ffCase = caseList->begin(); ffCase != caseList->end(); ffCase++)
    {
        // vilslog::printf("FireFightingService::updateCaseUserNFCCardAction() casenumber[%s]\n", (*ffCase)->mFireFighting->casenumber.c_str());

        for (std::list<int>::iterator deviceid = (*ffCase)->mFireFighting->NFCDeviceIdList->begin(); deviceid != (*ffCase)->mFireFighting->NFCDeviceIdList->end(); deviceid++)
        {
            // vilslog::printf("FireFightingService::updateCaseUserNFCCardAction() (*deviceid)[%d]\n", (*deviceid));
            if ( nfcdevice->deviceid == (*deviceid) )
            {
                // vilslog::printf("FireFightingService::updateCaseUserNFCCardAction() found casenumber[%s] cardid[%s] userid[%d]\n",
                //     (*ffCase)->mFireFighting->casenumber.c_str(), cardid.c_str(), nfccard->userid);


                // add user to field
                updateCaseUserArea(projectid, (*ffCase)->mFireFighting->casenumber, (*ffCase)->mFireFighting->maingroupid, (*ffCase)->mUserInfoList, AREA_FIELD, nfccard->userid);

            }
        }

        // vilslog::printf("FireFightingService::updateCaseUserNFCCardAction() StandbyDeviceIds->size[%d]\n", (int)(*ffCase)->mFireFighting->StandbyDeviceIds->size());

        for (std::list<std::string>::iterator deviceid = (*ffCase)->mFireFighting->StandbyDeviceIds->begin(); deviceid != (*ffCase)->mFireFighting->StandbyDeviceIds->end(); deviceid++)
        {
            int standbydeviceid = convert_string_to_int((*deviceid));
            // vilslog::printf("FireFightingService::updateCaseUserAoAAction() found standbydeviceid[%d] deviceid[%d]\n", standbydeviceid, nfcdevice->deviceid);

            if ( nfcdevice->deviceid == standbydeviceid )
            {
                // vilslog::printf("FireFightingService::updateCaseUserAoAAction() found standbydeviceid[%d] userid[%d]\n", standbydeviceid, nfccard->userid);

                // add user to field
                updateCaseUserArea(projectid, (*ffCase)->mFireFighting->casenumber, (*ffCase)->mFireFighting->maingroupid, (*ffCase)->mUserInfoList, AREA_STANDBY1, nfccard->userid);
            }
        }

    }
}

void FireFightingService::updateCaseUserAoAAction(int projectid, std::string loramacaddress, std::string macaddress, int Distance, float angle)
{
    char * locatorID = mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, loramacaddress.c_str());

    // vilslog::printf("FireFightingService::updateCaseUserAoAAction() locatorID[%s]\n", locatorID);

    AoANodeStruct * aoaNode = mDeviceManager->get_AoA_nodeById(projectid, locatorID);
    if (aoaNode == NULL)
    {
        return;
    }

    // vilslog::printf("FireFightingService::updateCaseUserAoAAction() aoaNode nodename[%s]\n", aoaNode->nodename);

    NodeStruct * tagnode = mDeviceManager->get_Tag_node_ByMacaddress(projectid, macaddress.c_str());
    if (tagnode == NULL)
    {
        return;
    }

    if (tagnode->userid <= 0)
    {
        return;
    }

    std::list<FireFightingCaseStruct*> * caseList = getProjectCaseList(projectid);
    if (caseList == NULL)
    {
        return;
    }

    // vilslog::printf("FireFightingService::updateCaseUserAoAAction() caseList->size()[%d]\n", (int)caseList->size());

    for (std::list<FireFightingCaseStruct*>::iterator ffCase = caseList->begin(); ffCase != caseList->end(); ffCase++)
    {
        for (std::list<std::string>::iterator deviceid = (*ffCase)->mFireFighting->StandbyDeviceIds->begin(); deviceid != (*ffCase)->mFireFighting->StandbyDeviceIds->end(); deviceid++)
        {
            if ( (*deviceid).compare(locatorID) == 0 )
            {
                // vilslog::printf("FireFightingService::updateCaseUserAoAAction() found deviceid[%s] nodename[%s] userid[%d]\n",
                //     (*deviceid).c_str(), tagnode->nodename, tagnode->userid);

                // add user to field
                if (Distance >= 300)
                {
                    updateCaseUserArea(projectid, (*ffCase)->mFireFighting->casenumber, (*ffCase)->mFireFighting->maingroupid, (*ffCase)->mUserInfoList, AREA_STANDBY2, tagnode->userid);
                }
                else
                {
                    updateCaseUserArea(projectid, (*ffCase)->mFireFighting->casenumber, (*ffCase)->mFireFighting->maingroupid, (*ffCase)->mUserInfoList, AREA_STANDBY1, tagnode->userid);
                }
            }
        }
    }
}

void FireFightingService::updateCaseUserArea(int projectid, std::string casenumber, int areatype, int userid)
{
    std::list<FireFightingCaseStruct*> * caseList = getProjectCaseList(projectid);
    if (caseList == NULL)
    {
        return;
    }

    // vilslog::printf("FireFightingService::updateCaseUserArea() casenumber[%s] userid[%d] areatype[%d]\n", casenumber.c_str(), userid, areatype);

    for (std::list<FireFightingCaseStruct*>::iterator ffCase = caseList->begin(); ffCase != caseList->end(); ffCase++)
    {
        if ((*ffCase)->mFireFighting->casenumber.compare(casenumber) == 0)
        {
            bool bFound = false;
            for (std::list<FireFightingUserStruct*>::iterator userinfo = (*ffCase)->mUserInfoList->begin(); userinfo != (*ffCase)->mUserInfoList->end(); userinfo++)
            {
                // vilslog::printf("FireFightingService::updateCaseUserArea() (*userinfo)->userid[%d]\n", (*userinfo)->userid);

                if ( (*userinfo)->userid == userid)
                {
                    if ((*userinfo)->areaType != areatype)
                        (*userinfo)->enterTime = getCurrentTimeInSec();
                    (*userinfo)->areaType = areatype;
                    sendOutInfo(projectid, "FireFightingUser", (*userinfo));
                    bFound = true;
                    break;
                }
            }

            // vilslog::printf("FireFightingService::updateCaseUserArea() bFound[%d]\n", bFound);

            if (!bFound)
            {
                // add user to field
                updateCaseUserArea(projectid, casenumber, (*ffCase)->mFireFighting->maingroupid, (*ffCase)->mUserInfoList, AREA_FIELD, userid);
            }
            break;
        }
    }
}

void FireFightingService::updateCaseUserArea(int projectid, std::string casenumber, int maingroupid, std::list<FireFightingUserStruct*> * userInfoList, int areatype, int userid)
{
    bool bFound = false;
    for (std::list<FireFightingUserStruct*>::iterator userinfo = userInfoList->begin(); userinfo != userInfoList->end(); userinfo++)
    {
        if ( (*userinfo)->userid == userid)
        {
            (*userinfo)->maingroupid = maingroupid;
            (*userinfo)->casenumber = casenumber;
            if ((*userinfo)->areaType == areatype)
            {
                // already in the area
                (*userinfo)->enterTime = getCurrentTimeInSec();
                // vilslog::printf("FireFightingService::updateCaseUserArea() already in the area areatype[%d] userid[%d]\n",
                //     areatype, userid);

            }
            else
            {
                if ( (*userinfo)->areaType == AREA_FIELD && areatype != AREA_FIELD )
                {
                    // wait 1 minute to enter
                    time_t diffTime = getCurrentTimeInSec() - (*userinfo)->enterTime;

                    if ( diffTime < 60 )
                    {
                        // not yet
                        vilslog::printf("FireFightingService::updateCaseUserArea() not yet update area areatype[%d] userid[%d] enterTime[%s] diffTime[%d]\n",
                            areatype, userid, time_tToString((*userinfo)->enterTime).c_str(), (int)diffTime);
                        bFound = true;
                        break;
                    }
                }

                // update area type
                (*userinfo)->areaType = areatype;
                (*userinfo)->enterTime = getCurrentTimeInSec();

                // vilslog::printf("FireFightingService::updateCaseUserArea() update area type areatype[%d] userid[%d] enterTime[%s]\n",
                //     areatype, userid, time_tToString((*userinfo)->enterTime).c_str());
            }

            sendOutInfo(projectid, "FireFightingUser", (*userinfo));

            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        FireFightingUserStruct * newuser = new FireFightingUserStruct();
        newuser->userid = userid;
        newuser->maingroupid = maingroupid;
        newuser->areaType = areatype;
        newuser->airremain = 0;
        newuser->enterTime = getCurrentTimeInSec();
        newuser->casenumber = casenumber;

        // vilslog::printf("FireFightingService::updateCaseUserArea() new area user areatype[%d] userid[%d] enterTime[%s]\n",
        //     areatype, newuser->userid, time_tToString(newuser->enterTime).c_str());

        userInfoList->push_back(newuser);

        sendOutInfo(projectid, "FireFightingUser", newuser);
    }
}

void FireFightingService::updateCaseUserAirRemain(int projectid, std::string casenumber, int userid, float airremain)
{
    // vilslog::printf("FireFightingService::updateCaseUserAirRemain() projectid[%d] casenumber[%s] userid[%d] airremain[%.02f]\n",
    //     projectid, casenumber.c_str(), userid, airremain);

    std::list<FireFightingCaseStruct*> * caseList = getProjectCaseList(projectid);
    if (caseList == NULL)
    {
        return;
    }

    for (std::list<FireFightingCaseStruct*>::iterator ffCase = caseList->begin(); ffCase != caseList->end(); ffCase++)
    {
        if ((*ffCase)->mFireFighting->casenumber.compare(casenumber) == 0)
        {
            for (std::list<FireFightingUserStruct*>::iterator userinfo = (*ffCase)->mUserInfoList->begin(); userinfo != (*ffCase)->mUserInfoList->end(); userinfo++)
            {
                if ( (*userinfo)->userid == userid)
                {
                    (*userinfo)->airremain = airremain;

                    vilslog::printf("FireFightingService::updateCaseUserAirRemain() projectid[%d] casenumber[%s] userid[%d] airremain[%.02f]\n",
                        projectid, casenumber.c_str(), userid, airremain);

                    sendOutInfo(projectid, "FireFightingUser", (*userinfo));

                    break;
                }
            }
            break;
        }
    }
}

void FireFightingService::updateCaseUserActivity(int projectid, std::string macaddress, std::string activity)
{
    // vilslog::printf("FireFightingService::updateCaseUserActivity() projectid[%d] macaddress[%s] activity[%s]\n",
    //     projectid, macaddress.c_str(), activity.c_str());

    NodeStruct * tagnode = mDeviceManager->get_Tag_node_ByMacaddress(projectid, macaddress.c_str());
    if (tagnode == NULL)
    {
        return;
    }

    if (tagnode->userid <= 0)
    {
        return;
    }

    std::list<FireFightingCaseStruct*> * caseList = getProjectCaseList(projectid);
    if (caseList == NULL)
    {
        return;
    }

    // vilslog::printf("FireFightingService::updateCaseUserActivity() caseList->size()[%d] userid[%d]\n", (int)caseList->size(), tagnode->userid);

    for (std::list<FireFightingCaseStruct*>::iterator ffCase = caseList->begin(); ffCase != caseList->end(); ffCase++)
    {
        for (std::list<FireFightingUserStruct*>::iterator userinfo = (*ffCase)->mUserInfoList->begin(); userinfo != (*ffCase)->mUserInfoList->end(); userinfo++)
        {
            // vilslog::printf("FireFightingService::updateCaseUserActivity() (*userinfo)->userid[%d]\n", (*userinfo)->userid);

            if ( (*userinfo)->userid == tagnode->userid)
            {
                (*userinfo)->activity = activity;

                sendOutInfo(projectid, "FireFightingUser", (*userinfo));

                break;
            }
        }
        break;
    }
}

void FireFightingService::updateCaseUserBodyInfo(int projectid, std::string sensorid, std::string jHR, std::string jSPO2, std::string jBattery)
{
    // vilslog::printf("FireFightingService::updateCaseUserBodyInfo() projectid[%d] sensorid[%s] jHR[%s] jSPO2[%s] jBattery[%s]\n",
    //     projectid, sensorid.c_str(), jHR.c_str(), jSPO2.c_str(), jBattery.c_str());

    //
    // find user sensor
    //
    std::list<UserStruct*> * userList = mDeviceManager->get_User_list(projectid);

    for (std::list<UserStruct*>::iterator user = userList->begin(); user != userList->end(); user++)
    {
        for (std::list<std::string>::iterator sensor = (*user)->sensors->begin(); sensor != (*user)->sensors->end(); sensor++)
        {
            if ( (*sensor).compare(sensorid) == 0)
            {
                // vilslog::printf("FireFightingService::updateCaseUserBodyInfo() found! userid[%d]\n", (*user)->userid);

                updateCaseUserBodyInfo(projectid, (*user)->userid, jHR, jSPO2, jBattery);

                break;
            }
        }
    }
}

void FireFightingService::updateCaseUserBodyInfo(int projectid, int userid, std::string jHR, std::string jSPO2, std::string jBattery)
{
    // vilslog::printf("FireFightingService::updateCaseUserBodyInfo() projectid[%d] userid[%d] jHR[%s] jSPO2[%s] jBattery[%s]\n",
    //     projectid, userid, jHR.c_str(), jSPO2.c_str(), jBattery.c_str());

    std::list<FireFightingCaseStruct*> * caseList = getProjectCaseList(projectid);
    if (caseList == NULL)
        return;

    for (std::list<FireFightingCaseStruct*>::iterator ffCase = caseList->begin(); ffCase != caseList->end(); ffCase++)
    {
        for (std::list<FireFightingUserStruct*>::iterator userinfo = (*ffCase)->mUserInfoList->begin(); userinfo != (*ffCase)->mUserInfoList->end(); userinfo++)
        {
            if ( (*userinfo)->userid == userid)
            {
                // vilslog::printf("FireFightingService::updateCaseUserActivity() found! \n");

                (*userinfo)->HR = jHR;
                (*userinfo)->SPO2 = jSPO2;
                (*userinfo)->battery = jBattery;

                sendOutInfo(projectid, "FireFightingUser", (*userinfo));

                break;
            }
        }
        break;
    }
}

void FireFightingService::finishCase(int projectid, std::string casenumber, int calcairusing)
{
    std::list<FireFightingCaseStruct*> * caseList = getProjectCaseList(projectid);
    if (caseList == NULL)
    {
        return;
    }

    for (std::list<FireFightingCaseStruct*>::iterator ffCase = caseList->begin(); ffCase != caseList->end(); ffCase++)
    {
        if ((*ffCase)->mFireFighting->casenumber.compare(casenumber) == 0)
        {
            (*ffCase)->mFireFighting->caseclosed = 1;
            (*ffCase)->mFireFighting->closetime = currentDateTimeForSQL();

            sendOutInfo(projectid, "FireFightingInfo", (*ffCase)->mFireFighting);

            if (calcairusing == 1)
            {
                calcAirUsing(projectid, (*ffCase)->mFireFighting->casenumber);
            }

            break;
        }
    }
}

void FireFightingService::initProjectCaseInfo()
{
    // vilslog::printf ("FireFightingService()::initProjectCaseInfo() mProjectFireFightingCaseVector.size()[%d]\n",
    //     (int)mProjectFireFightingCaseVector.size());

    clear_ProjectFireFightingCaseStruct_vector(&mProjectFireFightingCaseVector);

    std::list<int> * prjidList = mDbManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);

        ProjectFireFightingCaseStruct * newCase = new ProjectFireFightingCaseStruct();
        newCase->projectid = projectid;
        newCase->mCaseList = readProjectCasesFromDB(projectid);

        mProjectFireFightingCaseVector.push_back(newCase);
    }

    prjidList->clear();
    delete prjidList;
}

void FireFightingService::reloadCaseList()
{
    std::list<int> * prjidList = mDbManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);

        bool bFound = false;
        for (std::vector<ProjectFireFightingCaseStruct*>::iterator i = mProjectFireFightingCaseVector.begin(); i != mProjectFireFightingCaseVector.end(); i++)
        {
            if ( (*i)->projectid == projectid )
            {
                clear_ProjectFireFightingCaseStruct((*i));
                (*i)->mCaseList = readProjectCasesFromDB(projectid);

                bFound = true;
                break;
            }
        }
        if (!bFound)
        {
            ProjectFireFightingCaseStruct * newCase = new ProjectFireFightingCaseStruct();
            newCase->projectid = projectid;
            newCase->mCaseList = readProjectCasesFromDB(projectid);
        }
    }

    prjidList->clear();
    delete prjidList;
}

std::list<FireFightingCaseStruct*> * FireFightingService::readProjectCasesFromDB(int projectid)
{
    std::list<FireFightingCaseStruct*> * caseList = new std::list<FireFightingCaseStruct*>();

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
        return caseList;

    std::list<FireFightingStruct*> * pCases = baseDb->get_OpenFireFightingCases();

    for (std::list<FireFightingStruct*>::iterator ffcase = pCases->begin(); ffcase != pCases->end(); ffcase++)
    {
        FireFightingCaseStruct * newFFcase = new FireFightingCaseStruct();
        newFFcase->mFireFighting = new FireFightingStruct();
        newFFcase->mFireFighting->StandbyDeviceIds = new std::list<std::string>();
        newFFcase->mFireFighting->NFCDeviceIdList = new std::list<int>();
        newFFcase->mUserInfoList = baseDb->get_FireFightingUsers((*ffcase)->casenumber);

        copyFireFightingStruct((*ffcase), newFFcase->mFireFighting);

        // vilslog::printf ("FireFightingService()::readProjectCasesFromDB() projectid[%d] newFFcase->mFireFighting->casenumber[%s] mUserInfoList->size()[%d]\n",
        //     projectid, newFFcase->mFireFighting->casenumber.c_str(), (int)newFFcase->mUserInfoList->size());

        caseList->push_back(newFFcase);
    }

    pCases->clear();
    delete pCases;

    return caseList;
}

std::list<FireFightingCaseStruct*> * FireFightingService::getProjectCaseList(int projectid)
{
    std::list<FireFightingCaseStruct*> * caseList = NULL;
    for (std::vector<ProjectFireFightingCaseStruct*>::iterator i = mProjectFireFightingCaseVector.begin(); i != mProjectFireFightingCaseVector.end(); i++)
    {
        if ( (*i)->projectid == projectid )
        {
            caseList = (*i)->mCaseList;
            break;
        }
    }
    return caseList;
}

void FireFightingService::sendOutInfo(int projectid, std::string infoType, FireFightingStruct * fireFighting)
{
    WorkItem * workitem = new WorkItem("firefightingevent", projectid);
    workitem->m_FireFightingType = infoType;
    workitem->setMaingroupid(fireFighting->maingroupid);
    copyFireFightingStruct(fireFighting, &workitem->m_FireFightingInfo);

    mSQLJobPublisher->publish(workitem);

    SubscribeDataItem * subitem = new SubscribeDataItem("firefightingevent", projectid);
    subitem->m_FireFightingType = infoType;
    subitem->m_maingroupid = fireFighting->maingroupid;
    copyFireFightingStruct(fireFighting, &subitem->m_FireFightingInfo);

    mWebSocketEventPublisher->publish(subitem);
}

void FireFightingService::sendOutInfo(int projectid, std::string infoType, FireFightingUserStruct * fireFightingUser)
{
    WorkItem * workitem = new WorkItem("firefightingevent", projectid);
    workitem->m_FireFightingType = infoType;
    workitem->setMaingroupid(fireFightingUser->maingroupid);

    copyFireFightingUserStruct(fireFightingUser, &workitem->m_FireFightingUser);

    mSQLJobPublisher->publish(workitem);

    SubscribeDataItem * subitem = new SubscribeDataItem("firefightingevent", projectid);
    subitem->m_FireFightingType = infoType;
    subitem->m_maingroupid = fireFightingUser->maingroupid;

    copyFireFightingUserStruct(fireFightingUser, &subitem->m_FireFightingUser);

    mWebSocketEventPublisher->publish(subitem);
}

void FireFightingService::calcAirUsing(int projectid, std::string casenumber)
{
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
        return;

    std::list<FireFightingEventStruct*> * eventList = baseDb->get_FireFightingEvents(casenumber, "FireFightingUser");

    std::list<FireFightingUserAirInfoStruct*> * userAirInfoList = new std::list<FireFightingUserAirInfoStruct*>();

    for (std::list<FireFightingEventStruct*>::iterator i = eventList->begin(); i != eventList->end(); i++)
    {
        // vilslog::printf ("FireFightingService()::calcAirUsing() projectid[%d] id[%d] casenumber[%s] eventtype[%s] datetime[%s]\n",
        //     projectid, (*i)->id, (*i)->casenumber.c_str(), (*i)->eventtype.c_str(), (*i)->datetime.c_str());

        putToUserAirInfoList(userAirInfoList, (*i)->jsonstring, (*i)->datetime);
    }

    vilslog::printf ("FireFightingService()::calcAirUsing() projectid[%d] casenumber[%s] userAirInfoList->size()[%d]\n",
        projectid, casenumber.c_str(), (int)userAirInfoList->size());

    for (std::list<FireFightingUserAirInfoStruct*>::iterator i = userAirInfoList->begin(); i != userAirInfoList->end(); i++)
    {
        calcUserAirUsing(projectid, casenumber, (*i)->userid, (*i)->AirInfoList);
    }

    clear_FireFightingUserAirInfoStruct_list(userAirInfoList);
    delete userAirInfoList;

    eventList->clear();
    delete eventList;
}

void FireFightingService::calcUserAirUsing(int projectid, std::string casenumber, int userid, std::list<FireFightingAirInfoStruct*> * AirInfoList)
{
    vilslog::printf ("\nFireFightingService()::calcUserAirUsing() userid[%d]\n", userid);

    float totalAirUsing = 0;
    time_t totalTimeUsing = 0;

    // int areaType_pre = 1;// 0:field 1,2:standby

    FireFightingAirInfoStruct * preInfo = NULL;
    FireFightingAirUsingStruct * newRecord = NULL;

    for (std::list<FireFightingAirInfoStruct*>::iterator i = AirInfoList->begin(); i != AirInfoList->end(); i++)
    {
        vilslog::printf ("FireFightingService()::calcUserAirUsing() areaType[%d] airremain_f[%.02f] enterTime_t[%d] datetime_t[%d]\n",
            (*i)->areaType, (*i)->airremain, (*i)->enterTime, (*i)->datetime);

        if(preInfo == NULL)
        {
            preInfo = (*i);
            continue;
        }

        if (preInfo->areaType == (*i)->areaType)
        {
            if (newRecord != NULL)
            {
                vilslog::printf ("FireFightingService()::calcUserAirUsing() newRecord airusing[%.02f] totaltime[%d]\n",
                    newRecord->airusing, newRecord->totaltime);

                //------------------------------------------
                WorkItem * workitem = new WorkItem("firefightingevent", projectid);
                workitem->m_FireFightingType = "FireFightingAirUsing";
                copyFireFightingAirUsingStruct(newRecord, &workitem->m_FireFightingAirUsing);
                mSQLJobPublisher->publish(workitem);
                //------------------------------------------

                delete newRecord;
                newRecord = NULL;
            }

            preInfo = (*i);
        }
        else
        {
            if (preInfo->areaType == 0)
            {
                if ( preInfo->airremain > (*i)->airremain )
                {
                    float diffAirUsing = preInfo->airremain - (*i)->airremain;
                    time_t diffTimeUsing = (*i)->enterTime - preInfo->enterTime;

                    if (newRecord == NULL)
                    {
                        newRecord = new FireFightingAirUsingStruct();
                    }

                    newRecord->userid = userid;
                    newRecord->casenumber = casenumber;
                    newRecord->startairremain = preInfo->airremain;
                    newRecord->starttime = preInfo->enterTime;
                    newRecord->endairremain = (*i)->airremain;
                    newRecord->endtime = (*i)->enterTime;
                    newRecord->airusing = diffAirUsing;
                    newRecord->totaltime = diffTimeUsing;

                    totalAirUsing += diffAirUsing;
                    totalTimeUsing += diffTimeUsing;

                    vilslog::printf ("FireFightingService()::calcUserAirUsing() diffAirUsing[%.02f] diffTimeUsing[%d]\n",
                        diffAirUsing, diffTimeUsing);
                }
            }
            else
            {
                preInfo = (*i);
            }
        }
    }

    if (newRecord != NULL)
    {
        vilslog::printf ("FireFightingService()::calcUserAirUsing() newRecord airusing[%.02f] totaltime[%d]\n",
            newRecord->airusing, newRecord->totaltime);

        //------------------------------------------
        WorkItem * workitem = new WorkItem("firefightingevent", projectid);
        workitem->m_FireFightingType = "FireFightingAirUsing";
        copyFireFightingAirUsingStruct(newRecord, &workitem->m_FireFightingAirUsing);
        mSQLJobPublisher->publish(workitem);
        //------------------------------------------

        delete newRecord;
        newRecord = NULL;
    }

    vilslog::printf ("FireFightingService()::calcUserAirUsing() totalAirUsing[%.02f] totalTimeUsing[%d] final.\n",
        totalAirUsing, totalTimeUsing);

    // Convert to  bar/min
    float airusing = (totalAirUsing / totalTimeUsing) * 60;

    //------------------------------------------
    WorkItem * workitem = new WorkItem("firefightingevent", projectid);
    workitem->m_FireFightingType = "UserFireFightingAirUsing";
    workitem->m_FireFightingAirUsing.userid = userid;
    workitem->m_FireFightingAirUsing.airusing = airusing;
    mSQLJobPublisher->publish(workitem);
    //------------------------------------------

}

void FireFightingService::putToUserAirInfoList(std::list<FireFightingUserAirInfoStruct*> * userAirInfoList, std::string jsonstring, std::string datetime)
{
    Json::Value result;
    bool parsingSuccessful = parse_JSON(jsonstring.c_str(), result);

    if ( parsingSuccessful )
    {
        // {"userid":1226,"areaType":2,"airremain":"360","enterTime":"2021-09-01 16:50:03","activity":"SITTING","HR":"105","SPO2":"98","battery":"90"}
        int userid = 0;
        int areaType = -1;
        std::string airremain;
        std::string enterTime;

        if ( result.isMember("userid") )
            userid = result["userid"].asInt();
        if ( result.isMember("areaType") )
            areaType = result["areaType"].asInt();
        if ( result.isMember("airremain") )
            airremain = result["airremain"].asString();
        if ( result.isMember("enterTime") )
            enterTime = result["enterTime"].asString();

        // vilslog::printf ("FireFightingService()::putToUserAirInfoList() userid[%d] areaType[%d] airremain[%s] enterTime[%s] datetime[%s]\n",
        //     userid, areaType, airremain.c_str(), enterTime.c_str(), datetime.c_str());

        if (userid <= 0)
        {
            return;
        }

        float airremain_f = parseFloat(airremain);
        time_t enterTime_t = StringDateTimeTotime_t(enterTime);
        time_t datetime_t = StringDateTimeTotime_t(datetime);

        bool bFound = false;
        for (std::list<FireFightingUserAirInfoStruct*>::iterator i = userAirInfoList->begin(); i != userAirInfoList->end(); i++)
        {
            if ((*i)->userid == userid)
            {
                FireFightingAirInfoStruct * newAirInfo = new FireFightingAirInfoStruct();
                newAirInfo->areaType = areaType;
                newAirInfo->airremain = airremain_f;
                newAirInfo->enterTime = enterTime_t;
                newAirInfo->datetime = datetime_t;

                // vilslog::printf ("FireFightingService()::putToUserAirInfoList() 2 newAirInfo userid[%d] areaType[%d] airremain_f[%.02f] enterTime_t[%d] datetime_t[%d]\n",
                //     userid, areaType, airremain_f, enterTime_t, datetime_t);

                (*i)->AirInfoList->push_back(newAirInfo);

                bFound = true;
                break;
            }
        }

        if (!bFound)
        {
            FireFightingUserAirInfoStruct * newInfo = new FireFightingUserAirInfoStruct();
            newInfo->userid = userid;
            newInfo->AirInfoList = new std::list<FireFightingAirInfoStruct*>();
            userAirInfoList->push_back(newInfo);

            FireFightingAirInfoStruct * newAirInfo = new FireFightingAirInfoStruct();
            newAirInfo->areaType = areaType;
            newAirInfo->airremain = airremain_f;
            newAirInfo->enterTime = enterTime_t;
            newAirInfo->datetime = datetime_t;

            // vilslog::printf ("FireFightingService()::putToUserAirInfoList() 1 newAirInfo userid[%d] areaType[%d] airremain_f[%.02f] enterTime_t[%d] datetime_t[%d]\n",
            //     userid, areaType, airremain_f, enterTime_t, datetime_t);

            newInfo->AirInfoList->push_back(newAirInfo);
        }
    }
}

void FireFightingService::clear_ProjectFireFightingCaseStruct_vector(std::vector<ProjectFireFightingCaseStruct*> * plist)
{
    for (std::vector<ProjectFireFightingCaseStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        clear_ProjectFireFightingCaseStruct((*i));
        delete (*i);
    }
    plist->clear();
}

void FireFightingService::clear_ProjectFireFightingCaseStruct(ProjectFireFightingCaseStruct * info)
{
    if (info->mCaseList != NULL)
    {
        clear_FireFightingCaseStruct_list(info->mCaseList);
        delete info->mCaseList;
        info->mCaseList = NULL;
    }
}

void FireFightingService::clear_FireFightingCaseStruct_list(std::list<FireFightingCaseStruct*> * plist)
{
    for (std::list<FireFightingCaseStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        clear_FireFightingCaseStruct((*i));
        delete (*i);
    }
    plist->clear();
}

void FireFightingService::clear_FireFightingCaseStruct(FireFightingCaseStruct * info)
{
    if (info->mFireFighting != NULL)
    {
        info->mFireFighting->StandbyDeviceIds->clear();
        info->mFireFighting->NFCDeviceIdList->clear();

        delete info->mFireFighting->StandbyDeviceIds;
        delete info->mFireFighting->NFCDeviceIdList;

        info->mFireFighting->StandbyDeviceIds = NULL;
        info->mFireFighting->NFCDeviceIdList = NULL;
    }

    if (info->mUserInfoList != NULL)
    {
        info->mUserInfoList->clear();
        delete info->mUserInfoList;
        info->mUserInfoList = NULL;
    }
}

int FireFightingService::getProjectId(Json::Value result)
{
    int projectid = 1;

    if ( result.isMember("project_id") )
    {
        if ( result["project_id"].isInt() )
            projectid = result["project_id"].asInt();
        else
        if ( result["project_id"].isNull() )
            projectid = 1;
        else
            projectid = atoi(result["project_id"].asCString());
    }
    return projectid;
}
