/*
 *  DeviceEventCheckService.h
 *
 *  Created on: 2018/10/19
 *      Author: ikki
 */
#ifndef __DeviceEventCheckService_h__
#define __DeviceEventCheckService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../Events/EventChecker.h"
#include "../network/NotificationItem.h"
#include "../DeviceManager/DeviceManager.h"
#include "../network/wqueue.h"
#include "../network/WorkItem.h"
#include "../network/DataItem.h"
#include "../mysql/DBManager.h"
#include "../config.h"

using namespace std;

typedef struct
{
    int projectid;
    std::list<TriggerPushMessageStruct*> * mTriggerPushMessageList;
} ProjectTriggerPushMessageStruct;

typedef struct
{
    int projectid;
    std::list<TriggerRangingStruct*> * mTriggerRangingList;
} ProjectTriggerRangingStruct;

typedef struct
{
    int projectid;
    std::list<TriggerDoorEventStruct*> * mTriggerDoorEventList;
} ProjectTriggerDoorEventStruct;

class DeviceEventCheckService : public Thread
{
public:
    DeviceEventCheckService(wqueue<DataItem*> & DeviceEventJobQueue, wqueue<AreaInOutEventStruct*> & mareaInOutEventQueue, wqueue<SubscribeDataItem*> & pWebSocketqueue, wqueue<NotificationItem*> & NotifiactionQueue, wqueue<WorkItem*> & MySQLJobQueue, DeviceManager * pDevManager, DBManager * dbManager);
    bool status();
    std::string statusString();

    void * run();

private:

    std::string mStatus;
    bool mInitial;

    wqueue<DataItem*> & mDeviceEventJobQueue;
    wqueue<AreaInOutEventStruct*> & mAreaInOutEventQueue;
    wqueue<SubscribeDataItem*> & mWebSocketqueue;
    wqueue<NotificationItem*> & mNotifiactionQueue;
    wqueue<WorkItem*> & mMySQLJobQueue;

    DeviceManager * mDeviceManager;
    DBManager * mDbManager;

    EventChecker * mEventChecker;

    std::list<ProjectTriggerPushMessageStruct*> mProjectTriggerPushMessageList;
    std::list<ProjectTriggerRangingStruct*> mProjectTriggerRangingList;
    std::list<ProjectTriggerDoorEventStruct*> mProjectTriggerDoorEventList;

    void runloop();
    void waitloop();

    void reloadTriggerPushMessage();
    void reloadTriggerPushMessage(int projectid);

    int check_doctor_event(AreaInOutEventStruct * event);
    int check_device_event(DataItem * item);
    int checkTriggerMessage();
    int checkSingleTriggerMessage(int projectid, TriggerPushMessageStruct * triggerData);
    bool checkSingleRangingTriggerMessage(int projectid, TriggerPushMessageStruct * triggerData, TriggerRangingStruct * rangingData);

    int checkDoorEvent();


    int do_RangingFiltered(DataItem * item);
    void put_to_ranginglist(int projectid, const char * Anchor_id, const char * Tag_id, int distance);
    void put_to_dooreventlist(int projectid, const char * Anchor_id, const char * Tag_id, int distance);

    int sendNotification(AreaInOutEventStruct * event);
};


#endif
