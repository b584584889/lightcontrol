/*
 *  MySQLServiceLoopThread.h
 *
 *  Created on: 2017/05/24
 *      Author: ikki
 */
#ifndef __MySQLServiceLoopThread_h__
#define __MySQLServiceLoopThread_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Thread.h"
#include "../network/wqueue.h"
#include "../network/DataItem.h"
#include "../network/WorkItem.h"
#include "../network/inter_thread_pubsub.hpp"
#include "../mysql/DBManager.h"

#include "../config.h"

using namespace std;

class MySQLServiceLoopThread : public Thread
{
public:
    MySQLServiceLoopThread(wqueue<DataItem*> & MainJobQueue, wqueue<WorkItem*> & queue, DBManager * dbManager);
    ~MySQLServiceLoopThread();
    bool status();
    std::string statusString();
    void updateProject();

    void * run();
    std::string mProcessingType;
    int m_queue_size;
    int getQueueSize();

private:

    std::string mStatus;
    bool mInitial;

    ITPS::Subscriber<WorkItem*> * mSQLJobSubscriber;

    wqueue<DataItem*> & mMainJobQueue;
    wqueue<WorkItem*> & m_queue;
    DBManager * mDbManager;

    TagIndexDataClass mTagData[MAX_TAG_NUMBER];

    bool mUpdateProject;
    std::list<int> * mPrjidList;

    void runloop();
    void clear_old_data();

    int send_alive(WorkItem * item);
    int send_DeleteNode(WorkItem * item);
    int send_DeleteAoANode(WorkItem * item);
    void updateInfo(WorkItem * item);
    void updateFullInfo(WorkItem * item);
    void updateAoANodeInfo(WorkItem * item);
    void send_alert(WorkItem * item);
    void send_fencealert(WorkItem * item);
    void send_alert0D(WorkItem * item);
    void send_alertcancel(WorkItem * item);
    void send_event(WorkItem * item);
    void delete_event(WorkItem * item);
    void create_event(WorkItem * item);
    // void updateCheckPointPathDailyRecord(WorkItem * item);
    // void updateCheckPointGroupDailyRecord(WorkItem * item);
    // void updateCheckPointTagBehavior(WorkItem * item);
    void updateInspectionPointDailyRecord(WorkItem * item);
    void updateInspectionGroupDailyRecord(WorkItem * item);
    void updateInspectorBehavior(WorkItem * item);
    void createsection(WorkItem * item);
    void deletesection(WorkItem * item);
    void createfencealert(WorkItem * item);
    void deletefencealert(WorkItem * item);
    void updatemaplayer(WorkItem * item);
    void updatemaplayerarea(WorkItem * item);
    void updatemaplayerareashape(WorkItem * item);
    void deletemaplayerarea(WorkItem * item);
    //void createcheckpointgroup(WorkItem * item);
    //void updatecheckpointgroup(WorkItem * item);
    //void deletecheckpointgroup(WorkItem * item);
    //void createcheckpointpath(WorkItem * item);
    //void updatecheckpointpathstartnode(WorkItem * item);
    //void updatecheckpointpathendnode(WorkItem * item);
    //void updatecheckpointpathtraveltime(WorkItem * item);
    //void deletecheckpointpath(WorkItem * item);
    void updateconfig(WorkItem * item);
    void deleteconfig(WorkItem * item);
    void updatesystemconfigudpport(WorkItem * item);
    void updateAlgorithmConfig(WorkItem * item);
    void updateLoRAWANConfig(WorkItem * item);
    void createcourse(WorkItem * item);
    void deletecourse(WorkItem * item);
    void updateCourseRollCallRecord(WorkItem * item);
    void updateAreaInOutEventRecord(WorkItem * item);
    void updateTagGPSInfoRecord(WorkItem * item);
    void updateLoRaGatewayInfo(WorkItem * item);
    void updateBlacklist(WorkItem * item);
    void updateWhitelist(WorkItem * item);
    void updatejoinRangingRange(WorkItem * item);
    void updatejoinNotRangingRange(WorkItem * item);
    void UpdateResident(WorkItem * item);
    void DeleteResident(WorkItem * item);
    void UpdateEmployee(WorkItem * item);
    void DeleteEmployee(WorkItem * item);
    void updateNodeResident(WorkItem * item);
    void updateNodeEmployee(WorkItem * item);
    void updateLocatorButtontype(WorkItem * item);
    void updateLocatorSensorAlert(WorkItem * item);
    void EmployeeEventReport(WorkItem * item);
    void UpdatePushNotifyUser(WorkItem * item);
    void PushNotifyMessage(WorkItem * item);
    void PushNotifyMessageRecord(WorkItem * item);
    void send_callout(WorkItem * item);
    void send_doctorevent(WorkItem * item);
    void send_PowerOff(WorkItem * item);
    void send_PowerOn(WorkItem * item);
    void send_OpenDoor(WorkItem * item);
    void send_CloseDoor(WorkItem * item);
    void send_EnterDoor(WorkItem * item);
    void send_ExitDoor(WorkItem * item);
    void send_bodyinfoweight(WorkItem * item);
    void send_bodyinfoheartbeat(WorkItem * item);
    void send_bodyinfoall(WorkItem * item);
    void send_SensorAlert(WorkItem * item);
    void updatePushNotifySetting(WorkItem * item);
    void updateGAESurveyResultData(WorkItem * item);
    void updateGAEMessageStatusData(WorkItem * item);
    void updateTargetTopicRecord(WorkItem * item);
    void updateRFIDCardRecord(WorkItem * item);
    void updateRFIDCardRollCallRecord(WorkItem * item);
    void updateNFCCardRecord(WorkItem * item);
    // void updateNFCCardRollCallRecord(WorkItem * item);
    void updateCARDRollCallRecord(WorkItem * item);
    void updateCARDRollCallEvent(WorkItem * item);
    void updateCARDRollCallSubGroupEvent(WorkItem * item);
    void updateCARDScanRecord(WorkItem * item);
    void updateRFIDDevice(WorkItem * item);
    void updateNFCDevice(WorkItem * item);
    void updateEDGEDevice(WorkItem * item);
    void checkcarduser(WorkItem * item);
    void removecarduser(WorkItem * item);
    void adduseruwbcard(WorkItem * item);
    void updateUserStruct(WorkItem * item);
    void updateUserPoistion(WorkItem * item);
    void updateUserLocation(WorkItem * item);
    void updateUserPassword(WorkItem * item);
    void updateusersubgroup(WorkItem * item);
    void updateusermaingroup(WorkItem * item);
    void deleteGateway(WorkItem * item);
    void clearUnknownNFCCardAlive(WorkItem * item);
    void clearUnknownRFIDCardAlive(WorkItem * item);
    void updateLoginToken(WorkItem * item);
    void updateAssistEventStruct(WorkItem * item);
    void createAssistEventRecordStruct(WorkItem * item);
    void updateAssistEventRecordStruct(WorkItem * item);
    void updateApplicationFormLeaveStruct(WorkItem * item);
    void updateSensorInfoStruct(WorkItem * item);
    void updateTransactionSensorDeviceStruct(WorkItem * item);
    void updateSensorDeviceStruct(WorkItem * item);
    void updateSensorFiredEventStruct(WorkItem * item);
    void updateFlyInfoStruct(WorkItem * item);
    void updateWCInfoStruct(WorkItem * item);
    void updateCurriculumApplyStruct(WorkItem * item);
    void updateFireFightingEvent(WorkItem * item);
    void updateSoccerRecord(WorkItem * item);
    void createSoccerRecord(WorkItem * item);

};

#endif
