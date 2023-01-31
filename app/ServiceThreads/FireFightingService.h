/*
 *  FireFightingService.h
 *
 *  Created on: 2021/08/25
 *      Author: ikki
 */
#ifndef __FireFightingService_h__
#define __FireFightingService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../network/inter_thread_pubsub.hpp"
#include "../common.h"
#include "../config.h"
#include "../DeviceManager/DeviceManager.h"
#include "../network/WorkItem.h"
#include "../network/DataItem.h"
#include "../websocket/SubscribeDataItem.h"
#include "../mysql/DBManager.h"

using namespace std;

enum
{
    AREA_FIELD = 0,
    AREA_STANDBY1 = 1,
    AREA_STANDBY2 = 2
};

typedef struct
{
    FireFightingStruct * mFireFighting;
    std::list<FireFightingUserStruct*> * mUserInfoList;
} FireFightingCaseStruct;

typedef struct
{
    int projectid;
    std::list<FireFightingCaseStruct*> * mCaseList;
} ProjectFireFightingCaseStruct;

class FireFightingService : public Thread
{
public:
    FireFightingService(DeviceManager * pDevManager, DBManager * dbManager);
    ~FireFightingService();
    bool status();
    std::string statusString();

    void * run();

private:
    bool mInitial;
    std::string mStatus;
    bool mReloadAll;

    ITPS::Subscriber<std::string> * mFireFightingSubscriber;
    ITPS::Subscriber<DataItem*> * mNFCCardSubscriber;
    ITPS::Subscriber<DataItem*> * mAoARangingSubscriber;
    ITPS::Subscriber<DataItem*> * mTagActivitySubscriber;
    ITPS::Subscriber<WorkItem*> * mSensorBodyInfoSubscriber;

    ITPS::Publisher<WorkItem*> * mSQLJobPublisher;
    ITPS::Publisher<SubscribeDataItem*> * mWebSocketEventPublisher;

    DeviceManager * mDeviceManager;
    DBManager * mDbManager;

    std::vector<ProjectFireFightingCaseStruct *> mProjectFireFightingCaseVector;

    void initQueue();
    void runloop();

    void processData(DataItem* data);
    void processData(WorkItem* data);
    void processFireFightingEvent(std::string event);
    void updateCaseDeviceIds(int projectid, std::string casenumber, std::string StandbyDeviceIds, std::string NFCDeviceIds);
    void updateCaseUserNFCCardAction(int projectid, std::string loramacaddress, std::string cardid);
    void updateCaseUserAoAAction(int projectid, std::string loramacaddress, std::string macaddress, int Distance, float angle);
    void updateCaseUserArea(int projectid, std::string casenumber, int areatype, int userid);
    void updateCaseUserArea(int projectid, std::string casenumber, int maingroupid, std::list<FireFightingUserStruct*> * userInfoList, int areatype, int userid);
    void updateCaseUserAirRemain(int projectid, std::string casenumber, int userid, float airremain);
    void updateCaseUserActivity(int projectid, std::string macaddress, std::string activity);
    void updateCaseUserBodyInfo(int projectid, std::string sensorid, std::string jHR, std::string jSPO2, std::string jBattery);
    void updateCaseUserBodyInfo(int projectid, int userid, std::string jHR, std::string jSPO2, std::string jBattery);
    void finishCase(int projectid, std::string casenumber, int calcairusing);

    void initProjectCaseInfo();
    void reloadCaseList();
    std::list<FireFightingCaseStruct*> * readProjectCasesFromDB(int projectid);
    std::list<FireFightingCaseStruct*> * getProjectCaseList(int projectid);

    void sendOutInfo(int projectid, std::string infoType, FireFightingStruct * fireFighting);
    void sendOutInfo(int projectid, std::string infoType, FireFightingUserStruct * fireFightingUser);
    void calcAirUsing(int projectid, std::string casenumber);
    void calcUserAirUsing(int projectid, std::string casenumber, int userid, std::list<FireFightingAirInfoStruct*> * AirInfoList);
    void putToUserAirInfoList(std::list<FireFightingUserAirInfoStruct*> * userAirInfoList, std::string jsonstring, std::string datetime);

    void clear_ProjectFireFightingCaseStruct_vector(std::vector<ProjectFireFightingCaseStruct*> * plist);
    void clear_ProjectFireFightingCaseStruct(ProjectFireFightingCaseStruct * info);
    void clear_FireFightingCaseStruct_list(std::list<FireFightingCaseStruct*> * plist);
    void clear_FireFightingCaseStruct(FireFightingCaseStruct * info);

    int getProjectId(Json::Value result);
};

#endif
