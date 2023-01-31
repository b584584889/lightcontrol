/*
 *  InspectionGroupService.h
 *
 *  Created on: 2020/03/03
 *      Author: ikki
 */
#ifndef __InspectionGroupService_h__
#define __InspectionGroupService_h__


#include "Thread.h"
#include "../network/wqueue.h"
#include "../config.h"
#include "../network/DataItem.h"
#include "../network/WorkItem.h"
#include "../network/NotificationItem.h"
#include "../websocket/SubscribeDataItem.h"
#include "../DeviceManager/DeviceManager.h"
#include "../Inspection/InspectionAnchorRangingDataClass.h"
#include "../mysql/DBManager.h"
#include "../sqlite3/InspectionInfoDBAccess.h"

// #define INSPECTION_PRINT_LOG
#define INSPECTION_PRINT_LOG_RegisterInspectionTime

typedef enum
{
    BEHAVIOR_STATUS_NONE = 0,
    BEHAVIOR_STATUS_ENTER,
    BEHAVIOR_STATUS_IN_RANGE,
    BEHAVIOR_STATUS_LEAVE,
    BEHAVIOR_STATUS_START_RECORD,
    BEHAVIOR_STATUS_STOP_RECORD,
} TagBehaviorStatus;

typedef struct
{
    int GroupID;
    //int SendAlertMessage;
    std::string Inspector;//巡檢員
    // For Runtime Check
    InspectionGroupDailyRecord DailyData;
    std::list<InspectionPointStruct*> * InspectionPointList;
} InspectionGroupData;

typedef struct
{
    int GroupID;
    int PointID;
    int PointIndex;// InspectionPointStruct.PointIndex
    std::string AnchorNodeId;
    std::string Inspector;
    time_t LeaveTime;
    time_t RecordTime;
} InspectorActivityData;

typedef struct
{
    bool bStart;
    int GroupID;
    std::string AnchorNodeId;
    std::string Inspector;
    time_t InspectionStartTime;
    time_t GroupStartTime;
    time_t GroupEndTime;

    int setInspectionTime;//  設定 巡檢時間 0: in Tiime 1:過長 2:過短
    int setTravelTimeTime;//  設定 巡檢點轉換時間 0: in Tiime 1:過長 2:過短

    int InspectionPointDistance;
    int InspectionPointMinTime;// in minutes
    int InspectionPointMaxTime;// in minutes
    int InspectionPointTravelTime;
} InspectorSimulationData;

typedef struct
{
    int projectid;
    std::list<InspectionGroupStruct*> * mInspectionGroupList;
    std::vector<InspectionGroupData*> mInspectionGroupVector;

    std::list<InspectorActivityData*> mInspectorActivityList;
    std::vector<InspectionAnchorRangingDataClass *> mAnchorRangingMapVector;

    std::list<SendNotificationTimeData*> mNotificationTimeList;
    std::list<InspectorSimulationData*> mSimulationList;
    InspectionInfoDBAccess * m_CacheDB;
} ProjectInspectionInfoStruct;

class InspectionGroupService : public Thread
{
public:
    InspectionGroupService(wqueue<SubscribeDataItem*> & pWebSocketqueue, wqueue<DataItem*> & InspectionJobQueue, wqueue<NotificationItem*> & NotifiactionQueue, wqueue<WorkItem*> & MySQLJobQueue, DeviceManager * pDevManager, DBManager * dbManager);
    ~InspectionGroupService();
    bool status();
    std::string statusString();

    void * run();
    void dump_cache_data();
    void print_SimulationData();

private:
    wqueue<SubscribeDataItem*> & mWebSocketqueue;
    wqueue<DataItem*> & mInspectionJobQueue;
    wqueue<NotificationItem*> & mNotifiactionQueue;
    wqueue<WorkItem*> & mMySQLJobQueue;
    DeviceManager * mDeviceManager;
    DBManager * mDbManager;

    bool mInitial;
    std::string mStatus;

    std::vector<ProjectInspectionInfoStruct *> mProjectInspectionInfoVector;

    void runloop();
    int processDataItem(DataItem * dataItem);
    int do_RangingFiltered(DataItem * item);
    int do_RecordTagActivity(int projectid, std::string TagNodeId, std::string AnchorNodeId, int Distance);

    void initProjectInspectionInfo();
    ProjectInspectionInfoStruct * createProjectInspectionInfo(int projectid);
	ProjectInspectionInfoStruct * getProjectInspectionInfo(int projectid);
    InspectionInfoDBAccess * create_cacheDB(int projectid, ProjectInspectionInfoStruct * inspectionInfo);

    int InspectionSimulationEvent();
    int create_simulation_event(int projectid, InspectionGroupStruct * inspectionGroup, InspectionPointStruct * inspectionPoint);
    int update_simulation_event(int projectid, InspectionGroupStruct * inspectionGroup, InspectionPointStruct * inspectionPoint);
    int stop_simulation_event(int projectid, InspectionGroupStruct * inspectionGroup);

    int InspectionGroupEvent();
    int InspectionGroupEvent(int projectid, InspectionGroupStruct * inspectionGroup);
    int InspectionGroupEvent_outtime(int projectid, InspectionGroupStruct * inspectionGroup, InspectionGroupData * inspectiongroupdata, time_t t_diff);
    int InspectionGroupEvent_intime(int projectid, InspectionGroupStruct * inspectionGroup, InspectionGroupData * inspectiongroupdata, time_t t_diff);
    int InspectionGroupEvent_exceedtime(int projectid, InspectionGroupStruct * inspectionGroup, InspectionGroupData * inspectiongroupdata);

    int RegisterInspectionBehavior(int projectid, time_t t_InspectionGroupStartTime, std::list<std::string> * Inspectors, std::string Inspector, InspectionPointStruct * inspectionPoint);
    int RegisterInspectionTime(int projectid, time_t t_InspectionGroupStartTime, std::list<std::string> * Inspectors, std::string Inspector, InspectionPointStruct * inspectionPoint);
    int CheckBehaviorTime(int MaxTime, int MinTime, int InspectorEnterTime, int InspectorLeaveTime);
    int CheckRegisterTime(InspectionPointStruct * pre_inspt, InspectionPointStruct * cur_inspt);

    int create_InspectionGroup_action_event(int projectid, std::string Action, InspectionGroupStruct * inspectionGroup, InspectionGroupData * inspectiongroupdata);
    int create_InspectionPoint_action_event(int projectid, std::string Inspector, std::string Action, std::string AnchorNodeId, InspectionPointStruct * inspectionPoint);
    int create_InspectionTravel_action_event(int projectid, std::string Inspector, std::string Action, InspectionPointStruct * pre_inspectionPoint, InspectionPointStruct * inspectionPoint);

    std::string convert_inspectionGroup_action_event_to_json_str(std::string type, int projectid, std::string Action, InspectionGroupStruct * inspectionGroup, InspectionGroupData * inspectiongroupdata);
    std::string convert_inspectionPoint_action_event_to_json_str(std::string type, int projectid, std::string Inspector, std::string Action, std::string AnchorNodeId, InspectionPointStruct * inspectionPoint);
    std::string convert_inspectionTravel_action_event_to_json_str(std::string type, int projectid, std::string Inspector, std::string Action, InspectionPointStruct * pre_inspectionPoint, InspectionPointStruct * inspectionPoint);

    bool addTo_BehaviorRecord(int projectid, std::string Inspector, std::string AnchorNodeId, InspectionPointStruct * inspectionPoint);
    bool removeFrom_BehaviorRecord(int projectid, std::string Inspector, std::string AnchorNodeId, InspectionPointStruct * inspectionPoint);
    bool markLeave_BehaviorRecord(int projectid, std::string Inspector, std::string AnchorNodeId, InspectionPointStruct * inspectionPoint);

    void reloadInspectionList();
    void reloadInspectionList(ProjectInspectionInfoStruct * checkPointInfo);

    bool check_Group_AlertMessage(int projectid, InspectionGroupStruct * inspectionGroup, int MessageType, int timeout_sec);
    bool check_Point_AlertMessage(int projectid, InspectionGroupStruct * inspectionGroup, InspectionPointStruct* inspectionPoint, int MessageType, int timeout_sec);
    bool send_All_AlertMessage(int projectid, const char * locatorID, std::string message);

    std::string findInspector(std::list<InspectionPointStruct*> * inspectionPointList);
    void clear_ProjectInspectionInfoStruct_vector(std::vector<ProjectInspectionInfoStruct*> * plist);
    void clear_SendNotificationTimeData_list(std::list<SendNotificationTimeData*> * plist);
    void clear_Group_SendNotificationTimeData_list(int GroupID, std::list<SendNotificationTimeData*> * plist);

    void send_InspectionGroup_alert_notification(int projectid, InspectionGroupStruct * inspectionGroup, std::string message1, std::string message2, std::string message3);
    void send_InspectionGroup_exceed_notification(int projectid, InspectionGroupStruct * inspectionGroup, std::string message);
    void send_InspectionPoint_AllInspector_notification(int projectid, InspectionGroupStruct * inspectionGroup, InspectionPointStruct * inspectionPoint, const char * locatorID, std::string message);
    void send_InspectionPoint_notification(int projectid, InspectionGroupStruct * inspectionGroup, InspectionPointStruct * inspectionPoint, std::string message1, std::string message2, std::string message3, std::string message4);
    void send_Inspection_Finish_notification(int projectid, InspectionGroupStruct * inspectionGroup, InspectionGroupData * inspectiongroupdata);
    void send_push_notification(int projectid, InspectionGroupStruct * inspectionGroup, std::string message);
    void send_push_notification(int projectid, std::string locatorID, std::string NodeId, std::string message);
    std::string getAnchorNodeName(int projectid, std::string AnchorNodeId);
};

#endif
