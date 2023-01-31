/*
 *  GAEService.h
 *
 *  Created on: 2020/04/20
 *      Author: ikki
 */
#ifndef __GAEService_h__
#define __GAEService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../common.h"
#include "../config.h"
#include "../network/DataItem.h"
#include "../network/WorkItem.h"
#include "../network/NotificationItem.h"
#include "../mysql/DBManager.h"
#include "../DeviceManager/DeviceManager.h"
#include "WebSocketClient.h"

using namespace std;

class GAEService : public Thread
{
public:
    GAEService(wqueue<DataItem*> & GAEJobQueue, wqueue<WorkItem*> & MySQLJobQueue, wqueue<NotificationItem*> & NotifiactionQueue, DeviceManager * pDevManager, DBManager * dbManager);
    ~GAEService();
    bool status();
    std::string statusString();

    void * run();
    // void updatesurveyresult();
private:
    bool mInitial;
    std::string mStatus;

    // bool mUpdateSurveyResult;
    time_t mSyncGAETopicInfo_time;

    wqueue<DataItem*> & mGAEJobQueue;
    //wqueue<DataItem*> & mMainJobQueue;
    wqueue<WorkItem*> & mMySQLJobQueue;
    wqueue<NotificationItem*> & mNotifiactionQueue;
    DeviceManager * mDeviceManager;
    DBManager * mDbManager;

    WebSocketClient * m_client;

    void runloop();
    int processDataItem(DataItem * dataItem);
    bool SyncAllProject();

    int SyncMessageStatus(int projectid);
    int SyncSurveyResult(int projectid);
    std::list<GAESurveyResultData*> * fetchSurveyResult(int projectid);

    bool check_applicationform_leave(int projectid, std::list<SurveyEventRecordStruct*> * recordList, GAESurveyResultData * oldResult, GAESurveyResultData * surveyData);
    bool check_applicationform_assist(int projectid, std::list<SurveyEventRecordStruct*> * recordList, GAESurveyResultData * oldResult, GAESurveyResultData * surveyData);
    bool check_applicationform_curriculum(int projectid, std::list<SurveyEventRecordStruct*> * recordList, GAESurveyResultData * oldResult, GAESurveyResultData * surveyData);
#ifdef ENABLE_KIA_SERVICE
    bool check_KIA_WCInfo(int projectid, GAESurveyResultData * oldResult, GAESurveyResultData * surveyData);
#endif
    // int SyncTopicInfo(int projectid);
    int updateGAETopicInfo();
    int updateGAETopicInfo(int projectid);
    int updateGAEUserInfo(int projectid, int userid);
    int updateGAEUserCard(int projectid, std::string prefix, std::string VILSUID, UserStruct * user, TargetTopicStruct * topic);
    int updateGAECardUser(int projectid, std::string prefix, std::string VILSUID, UserStruct * user, TargetTopicStruct * topic);
    int updateGAECardUser_uwbcard(int projectid, std::string prefix, std::string VILSUID, std::list<NodeStruct*> * tagNodeList, UserStruct * user, TargetTopicStruct * topic);
    int updateGAECardUser_nfccard(int projectid, std::string prefix, std::string VILSUID, std::list<NFCCardStruct*> * nfccardList, UserStruct * user, TargetTopicStruct * topic);
    int updateGAECardUser_rfidcard(int projectid, std::string prefix, std::string VILSUID, std::list<RFIDCardStruct*> * rfidcardList, UserStruct * user, TargetTopicStruct * topic);

    int updateGAEUserCard();
    int updateGAECardUser();
    int updateGAECardUser_uwbcard(int projectid, std::string prefix, std::string VILSUID, std::list<NodeStruct*> * tagNodeList, std::list<UserStruct*> * userList, std::list<TargetTopicStruct*> * recordList);
    int updateGAECardUser_nfccard(int projectid, std::string prefix, std::string VILSUID, std::list<NFCCardStruct*> * nfccardList, std::list<UserStruct*> * userList, std::list<TargetTopicStruct*> * recordList);
    int updateGAECardUser_rfidcard(int projectid, std::string prefix, std::string VILSUID, std::list<RFIDCardStruct*> * rfidcardList, std::list<UserStruct*> * userList, std::list<TargetTopicStruct*> * recordList);
    int updateGAEDIInfo();


    //bool startSyncWithRetry(int projectid, int retry_Count);
    std::string startSync(int projectid, const char * type, const char * parameter);
    // bool updateGAETargetTopicInfo(TargetTopicStruct * newTopic);
    int updateGAETargetTopicInfoList(std::string prefix, std::string VILSUID, std::list<TargetTopicStruct*> * topiclist);
    int updateGAE_TAG_TopicInfo(int projectid, std::string PushNotifyPrefix, std::string VILSUID, std::list<TargetTopicStruct*> * recordList);
    int updateGAE_LOCATOR_TopicInfo(int projectid, std::string PushNotifyPrefix, std::string VILSUID, std::list<TargetTopicStruct*> * recordList);
    int updateGAE_TAG_USER_TopicInfo(int projectid, std::string PushNotifyPrefix, std::string VILSUID, std::list<TargetTopicStruct*> * recordList);
    int updateGAE_USER_TopicInfo(int projectid, std::string PushNotifyPrefix, std::string VILSUID, std::list<TargetTopicStruct*> * recordList, std::list<UserStruct*> * userList);
    int updateGAE_UHFCARD_TopicInfo(int projectid, std::string PushNotifyPrefix, std::string VILSUID, std::list<TargetTopicStruct*> * recordList, std::list<UserStruct*> * userList);
    int updateGAE_SUBGROUP_TopicInfo(int projectid, std::string PushNotifyPrefix, std::string VILSUID, std::list<TargetTopicStruct*> * recordList);
    int updateGAE_MAINGROUP_TopicInfo(int projectid, std::string PushNotifyPrefix, std::string VILSUID, std::list<TargetTopicStruct*> * recordList);


    // int update_to_GAE(int projectid, std::string target, std::string name, std::list<TargetTopicStruct*> * recordList);
    TargetTopicStruct * get_existTopic(int projectid, std::string target, std::list<TargetTopicStruct*> * recordList);
    TargetTopicStruct * get_newTopic(int projectid, std::string target, std::string name, std::string accountid, int userid, int maingroup, std::list<TargetTopicStruct*> * recordList);
    TargetTopicStruct * get_USER_newTopic(int projectid, std::string PushNotifyPrefix, UserStruct * user, int maingroup, std::list<TargetTopicStruct*> * recordList);

    bool sendPOSTRequest(const char * type, const char * sendbody);
    std::string sendRequest(const char * host, const char * port, const char * request);

    std::list<HttpHeader*> * parse_http_header(const char * response, size_t n);
    int get_line(const char * databuffer, int data_len, int start_idx, char * linebuffer, int buffer_len);
    void errExit(const char * reason);
    void errPrint(const char *reason);
    bool update_data(int projectid, Json::Value JSONresult);
    std::string getJSONString(Json::Value JSONresult, const char * key);
    int getJSONInt(Json::Value JSONresult, const char * key);
    NodeStruct * parseNodeInfo(int projectid, std::string prefix, std::string target);
    std::list<std::string> * getSubgGroupManager(int projectid, UserStruct * user);
};

#endif
