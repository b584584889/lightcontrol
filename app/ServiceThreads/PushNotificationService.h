/*
 *  PushNotificationService.h
 *
 *  Created on: 2019/08/02
 *      Author: ikki
 */
#ifndef __PushNotificationService_h__
#define __PushNotificationService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../config.h"
#include "../common.h"
#include "../network/NotificationItem.h"
#include "../network/DataItem.h"
#include "../network/WorkItem.h"
#include "../mysql/DBManager.h"
#include "../DeviceManager/DeviceManager.h"
using namespace std;

#define VALID_SEND_TIME_OUT 10000

// #if defined(__APPLE__)
// #define MSG_NOSIGNAL 0
// #endif

typedef struct
{
    int projectid;
    std::list<PushNotifySettingStruct*> * mPushNotifyList;
} ProjectPushNotifySettingStruct;

class PushNotificationService : public Thread
{
public:
    PushNotificationService(wqueue<DataItem*> & mainJobQueue, wqueue<NotificationItem*> & queue, wqueue<WorkItem*> & mysqlJobQueue, DeviceManager * pDevManager, DBManager * dbManager);
    bool status();
    std::string statusString();
    void updateProject();

    void * run();

private:

    std::string mStatus;
    bool mInitial;

    wqueue<DataItem*> & mMainJobQueue;
    wqueue<NotificationItem*> & m_queue;
    wqueue<WorkItem*> & mMySQLJobQueue;
    DeviceManager * mDeviceManager;
    DBManager * mDbManager;

    bool mUpdateProject;
    std::list<int> * mPrjidList;

    std::list<FunctionSendTimeStruct*> mFunctionSendTimeList;

    std::list<ProjectPushNotifySettingStruct*> mProjectPushNotifyList;

    void runloop();
    void waitloop();

    void reloadPushNotifySetting();
    void do_check_function(NotificationItem * item, VILSSystemConfig * vilssysConfig);
    std::list<std::string> * do_function_MULTI_PUSHNOTIFY_FUNCTION_1(NotificationItem * item, std::string PushNotifyPrefix, std::string VILSUID, std::string messagehashkey);
    std::list<std::string> * do_function_TAG_PUSHNOTIFY_FUNCTION_5(NotificationItem * item, std::string PushNotifyPrefix, std::string VILSUID, std::string messagehashkey);
    std::list<std::string> * do_function_TAG_PUSHNOTIFY_FUNCTION_10(NotificationItem * item, std::string PushNotifyPrefix, std::string VILSUID, std::string messagehashkey);
    std::list<std::string> * do_function_USER_PUSHNOTIFY_FUNCTION(NotificationItem * item, int functiontype, std::string PushNotifyPrefix, std::string VILSUID, std::string messagehashkey);
    std::list<std::string> * do_function_CARD_PUSHNOTIFY_FUNCTION_1(NotificationItem * item, std::string PushNotifyPrefix, std::string VILSUID, std::string messagehashkey);
    std::list<std::string> * do_function_other(NotificationItem * item, std::string PushNotifyPrefix, std::string VILSUID, std::string messagehashkey);


    // void check_User_Topic();
    // void check_User_Topic(int projectid);
    // void update_GAE_Topic(int projectid, std::string account);
    // void update_GAE_Topic(NotificationItem * item);
    bool checkValidSendNotifity(NotificationItem * item);
    std::list<std::string> * do_send_trigger_target(int projectid, std::string targetid, TriggerPushMessageStruct * triggerData, std::string PushNotifyPrefix, std::string VILSUID, std::string waitkey, std::string messagehashkey);
    std::list<std::string> * do_send_source_target(int projectid, int functiontype, std::string nodeid, std::string send_macaddress, std::string title, std::string message, std::string PushNotifyPrefix, std::string VILSUID, std::string waitkey, std::string messagehashkey);
    std::list<std::string> * do_send_target(int projectid, int functiontype, std::string targetid, std::string title, std::string message, std::string PushNotifyPrefix, std::string VILSUID, std::string waitkey, std::string todolist, std::string messagehashkey, std::string imageuid);
    int do_send_pushnotify(int projectid, int functiontype, std::string nodeid, std::string targetid, std::string title, std::string message, std::string PushNotifyPrefix, std::string VILSUID, std::string waitkey, std::string todolist, std::string messagehashkey, std::string imageuid);
    int do_send_user_pushnotify(int projectid, int functiontype, std::string userid, std::string title, std::string message, std::string PushNotifyPrefix, std::string VILSUID, std::string todo, std::string messagehashkey, std::string imageuid);
    int do_send_card_pushnotify(int projectid, int functiontype, std::string nodeid, std::string cardid, std::string title, std::string message, std::string PushNotifyPrefix, std::string VILSUID, std::string messagehashkey);

    // void do_send_tag_function(NotificationItem * item, int function);
    std::string sendPushNotification(std::string title, std::string topic, std::string message, std::string todo, std::string curTime, std::string messagehashkey, std::string VILSUID, std::string imageuid);
    std::string sendRequest(const char * host, const char * port, const char * request);
    std::list<HttpHeader*> * parse_http_header(const char * response, size_t n);
    int get_line(const char * databuffer, int data_len, int start_idx, char * linebuffer, int buffer_len);
    // int getNotifyType(int function, int nodetype);
    void errExit(const char *reason);
    void errPrint(const char *reason);
    std::string convertSurveyToJson(int projectid, std::list<std::string> * surveylist, std::list<std::string> * hashkeylist, std::list<std::string> * deadlines);

    std::string get_unique_hashksy_PushNotifyMessageRecord(int projectid);
    std::list<std::string> * get_unique_hashksy_SurveyEventRecord(int projectid, int count);

};


#endif
