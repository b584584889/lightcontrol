/*
 *  WorkItem.h
 *
 *  Created on: 2017/02/10
 *      Author: ikki
 */
#ifndef __WorkItem_h__
#define __WorkItem_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "../mysql/MySQLDBAccess.h"
#include "../mysql/TrackDBAccess.h"

using namespace std;

class WorkItem
{
    string m_type;
    int m_project_id;
    int m_NodeType;// 0:Anchor 1:Tag 2:Coordinator
    string m_nodeid;
    string m_macaddress;
    string m_nodename;
    double    m_voltage;
    double    m_temperature;
    int    m_count;

    double    m_posX;
    double    m_posY;
    double    m_posZ;
    double    m_accelX;
    double    m_accelY;
    double    m_accelZ;
    double    m_gyroX;
    double    m_gyroY;
    double    m_gyroZ;

    // For event
    string m_action;
    string m_area_name;
    string m_issuetime;

    string m_area_id;

    int    m_tafid;
    int    m_buildingid;
    int    m_floor;
    int    m_maplayer;

    string m_jsondata;
    string m_anchorids;

    int    m_range;
    string   m_serno;
    string   m_empid;

    std::string m_recorderid;
    std::string m_weight;
    std::string m_heartbeat;
    std::string m_bloodsugar;
    std::string m_systolicbloodpressure;
    std::string m_diastolicbloodpressure;
    std::string m_bodytemperature;
    std::string m_bodyfat;
    std::string m_bloodoxygen;
    std::string m_datetime;

    int m_role;
    std::string m_accountid;
    int m_userid;
    std::string m_pwd;
    std::string m_rfidcards;
    std::string m_nfccards;
    std::string m_uwbcards;
    std::string m_Longitude;
    std::string m_Latitude;

    int m_maingroupid;
    std::string m_maingroups;
    std::string m_status;

public:
    WorkItem(const char* type, int projectID);
    WorkItem(const char* type, int projectID, const char* nodeid, int nodetype, double voltage, double temperature, int count);
    WorkItem(const char* type, int projectID, const char* nodeid, double posX, double posY, double posZ, const char* area_id);
    WorkItem(const char* type, int projectID, const char* nodeid, double posX, double posY, double posZ,
                    double accelX, double accelY, double accelZ,
                    double gyroX, double gyroY, double gyroZ, const char* area_id);
    WorkItem(const char* type, int projectID, int nodetype);
    WorkItem(const char* type, int projectID, int nodetype, const char* macaddress);
    WorkItem(const char* type, int projectID, int nodetype, const char* macaddress, const char * jsondata);
    WorkItem(const char* type, int projectID, int nodetype, const char* macaddress, const char * jsondata, std::string empid);
    WorkItem(const char* type, int projectID, const char* macaddress, const char* nodeid, const char* nodename, int buildingid, int floor, double posX, double posY, double posZ);
    WorkItem(const char* type, int projectID, int nodetype, int tafid, int buildingid, int floor, double posX, double posY, double posZ);
    WorkItem(const char* type, int projectID, int tafid, string nodename, string action, string area_name, string issuetime);
    WorkItem(const char* type, int projectID, std::string macaddress, std::string serno, int dummy);
    WorkItem(const char* type, int projectID, std::string macaddress, std::string anchorids);
    WorkItem(const char* type, int projectID, std::string macaddress, int range);
    WorkItem(const char* type, int projectID, NodeStruct * node);
    WorkItem(const char* type, int projectID, CheckPointPathStruct checkPointPath);
    WorkItem(const char* type, int projectID, CheckPointPathDailyRecord DailyRecord);
    WorkItem(const char* type, int projectID, CheckPointGroupStruct checkPointGroup);
    WorkItem(const char* type, int projectID, CheckPointGroupDailyRecord DailyRecord);
    WorkItem(const char* type, int projectID, CheckPointTagBehaviorData TagBehaviorData);

    WorkItem(const char* type, int projectID, InspectionGroupStruct inspectionGroup);
    WorkItem(const char* type, int projectID, InspectionPointDailyRecord DailyRecord);
    WorkItem(const char* type, int projectID, InspectionGroupDailyRecord DailyRecord);
    WorkItem(const char* type, int projectID, InspectorBehaviorData InspectorBehaviorData);

    WorkItem(const char* type, int projectID, EventStruct * event);
    WorkItem(const char* type, int projectID, FiredEventStruct * eventFired);
    WorkItem(const char* type, int projectID, SectionGroup * sectiongroup);
    WorkItem(const char* type, int projectID, VILSConfig config);
    WorkItem(const char* type, int projectID, VILSSystemConfig * sysConfig);
    WorkItem(const char* type, int projectID, CourseStruct * course);
    WorkItem(const char* type, int projectID, CourseRollCallRecordStruct * courseRollCallRecord);
    WorkItem(const char* type, int projectID, AreaInOutEventStruct * inoutEvent);
    WorkItem(const char* type, int projectID, GPSInfoStruct * GPSInfo);
    WorkItem(const char* type, int projectID, LoRaGatewayInfoStruct * gateway);
    WorkItem(const char* type, int projectID, AnalyzeTrackRecordStruct * record);
    WorkItem(const char* type, int projectID, TagDayTrackRecordStruct * record);
    WorkItem(const char* type, int projectID, DeviceActivityRecordStruct * record);
    WorkItem(const char* type, int projectID, AreaRollCallRecordStruct * record);
    WorkItem(const char* type, int projectID, OperationModeRecordStruct * record);
    WorkItem(const char* type, int projectID, UserResidentStruct * record);
    WorkItem(const char* type, int projectID, UserEmployeeStruct * record);
    WorkItem(const char* type, int projectID, DeviceActivityAliveCountStruct * record);
    WorkItem(const char* type, int projectID, AreaActivityUserCountStruct * record);
    WorkItem(const char* type, int projectID, EmployeeEventReportStruct * record);
    WorkItem(const char* type, int projectID, OperationModeActivityStruct * record);
    WorkItem(const char* type, int projectID, TagMoveOffsetInfoStruct * record);
    WorkItem(const char* type, int projectID, NodeMoveOffsetInfoStruct * record);
    WorkItem(const char* type, int projectID, PushNotifyMessageStruct * record);
    WorkItem(const char* type, int projectID, PushNotifyMessageRecordStruct * record);
    WorkItem(const char* type, int projectID, MapLayerStruct * record);
    WorkItem(const char* type, int projectID, MapLayerAreaStruct * record);
    WorkItem(const char* type, int projectID, AoANodeStruct * record);
    WorkItem(const char* type, int projectID, FenceAlertStruct * record);
    WorkItem(const char* type, int projectID, GAESurveyResultData * record);
    WorkItem(const char* type, int projectID, GAEMessageStatusData * record);
    WorkItem(const char* type, int projectID, TargetTopicStruct * record);
    WorkItem(const char* type, int projectID, CARDRollCallRecordStruct * record);
    WorkItem(const char* type, int projectID, CARDRollCallEventStruct * record);
    WorkItem(const char* type, int projectID, CARDRollCallSubGroupEventStruct * record);
    WorkItem(const char* type, int projectID, CARDScanRecordStruct * record);
    WorkItem(const char* type, int projectID, RFIDDeviceStruct * device);
    WorkItem(const char* type, int projectID, NFCDeviceStruct * device);
    WorkItem(const char* type, int projectID, RFIDCardStruct * card);
    WorkItem(const char* type, int projectID, NFCCardStruct * card);
    WorkItem(const char* type, int projectID, EDGEDeviceStruct * device);
    WorkItem(const char* type, int projectID, UserStruct * record);
    WorkItem(const char* type, int projectID, UserSubGroupStruct * record);
    WorkItem(const char* type, int projectID, UserMainGroupStruct * record);
    WorkItem(const char* type, int projectID, LoginTokenStruct * record);
    WorkItem(const char* type, int projectID, AssistEventStruct * record);
    WorkItem(const char* type, int projectID, AssistEventRecordStruct * record);
    WorkItem(const char* type, int projectID, ApplicationFormLeaveStruct * record);
    WorkItem(const char* type, int projectID, SensorInfoStruct * info);
    WorkItem(const char* type, int projectID, SensorDeviceStruct * device);
    WorkItem(const char* type, int projectID, SensorFiredEventStruct * event);
    WorkItem(const char* type, int projectID, FlyInfoStruct * info);
    WorkItem(const char* type, int projectID, CurriculumApplyStruct * info);
    WorkItem(const char* type, int projectID, WCInfoStruct * info);
    WorkItem(const char* type, int projectID, SoccerRecordingStruct * record);

    ~WorkItem();
    void init();

    void setNodeId(const char* nodeid);
    void setMacaddress(const char* macaddress);
    void setMaplayer(int maplayer);
    void setRole(int role);
    void setAccountId(std::string accountid);
    void setUserId(int userid);
    void setUserPwd(std::string pwd);
    void setRFIDCards(std::string rfidcards);
    void setNFCCards(std::string nfccards);
    void setUWBCards(std::string uwbcards);
    void setLongitudeLatitude(std::string Longitude, std::string Latitude);
    void setPosition(double posX, double posY, double posZ);

    void setRecorderID(std::string recorderid);
    void setWeight(std::string weight);
    void setHeartBeat(std::string heartbeat);
    void setBloodSugar(std::string bloodsugar);
    void setSystolicBloodPressure(std::string systolicbloodpressure);
    void setDiastolicBloodPressure(std::string diastolicbloodpressure);
    void setBodyTemperature(std::string bodytemperature);
    void setBodyFat(std::string bodyfat);
    void setBloodOxygen(std::string bloodoxygen);
    void setDateTime(std::string datetime);
    void setMaingroupid(int maingroupid);
    void setMaingroups(std::string maingroups);
    void setStatus(std::string status);

    const char* getType();
    int getNodeType();
    int getProjectId();
    int getTAFID();
    int getBuildingId();
    int getFloor();
    int getMaplayer();

    const char* getMacaddress();
    const char* getNodeId();
    double getVoltage();
    double getTemperature();
    int getCount();

    double getPosX();
    double getPosY();
    double getPosZ();
    double getAccelX();
    double getAccelY();
    double getAccelZ();
    double getGyroX();
    double getGyroY();
    double getGyroZ();

    const char* getNodeName();
    const char* getAction();
    const char* getAreaName();
    const char* getIssueTime();

    const char* getAreaId();
    const char* getJsonData();
    const char* getAnchorids();

    int getRange();
    const char* getSerno();
    const char* getempid();

    std::string getRecorderID();
    std::string getWeight();
    std::string getHeartBeat();
    std::string getBloodSugar();
    std::string getSystolicBloodPressure();
    std::string getDiastolicBloodPressure();
    std::string getBodyTemperature();
    std::string getBodyFat();
    std::string getBloodOxygen();
    std::string getDateTime();

    int getRole();
    std::string getAccountId();
    int getUserId();
    std::string getUserPwd();
    std::string getRFIDCards();
    std::string getNFCCards();
    std::string getUWBCards();
    std::string getLongitude();
    std::string getLatitude();

    int getMaingroupid();
    std::string getMaingroups();
    std::string getStatus();

    NodeStruct m_node;
    CheckPointPathStruct m_CheckPointPath;
    CheckPointPathDailyRecord m_DailyRecord;
    CheckPointGroupStruct m_CheckPointGroup;
    CheckPointGroupDailyRecord m_GroupDailyRecord;

    InspectionGroupStruct m_InspectionGroup;
    InspectionPointDailyRecord m_InspectionPointDailyRecord;
    InspectionGroupDailyRecord m_InspectionGroupDailyRecord;
    InspectorBehaviorData m_InspectorBehaviorData;

    EventStruct m_Event;
    FiredEventStruct m_EventFired;
    CheckPointTagBehaviorData m_TagBehaviorData;
    SectionGroup m_Sectiongroup;
    VILSConfig m_Config;
    VILSSystemConfig m_SysConfig;
    CourseStruct m_Course;
    CourseRollCallRecordStruct m_CourseRollCallRecord;
    AreaInOutEventStruct mInOutEvent;
    GPSInfoStruct mGPSInfo;
    LoRaGatewayInfoStruct mLoRaGateway;
    AnalyzeTrackRecordStruct mAnalyzeTrackRecord;
    TagDayTrackRecordStruct mDayTrackRecord;
    DeviceActivityRecordStruct mDeviceActivityRecord;
    AreaRollCallRecordStruct mAreaRollCallRecord;
    OperationModeRecordStruct mOperationModeRecord;
    UserResidentStruct mResidentRecord;
    UserEmployeeStruct mEmployeeRecord;
    DeviceActivityAliveCountStruct mDeviceActivityAliveRecord;
    AreaActivityUserCountStruct mAreaActivityUserCountRecord;
    EmployeeEventReportStruct mEmployeeEventReportRecord;
    OperationModeActivityStruct mOperationModeActivityRecord;
    TagMoveOffsetInfoStruct mTagMoveOffsetInfoRecord;
    NodeMoveOffsetInfoStruct mNodeMoveOffsetInfoRecord;
    PushNotifyMessageStruct mPushNotifyMessage;
    PushNotifyMessageRecordStruct mPushNotifyMessageRecord;

    MapLayerStruct mMapLayerRecord;
    MapLayerAreaStruct mMapLayerAreaRecord;

    AoANodeStruct mAoANodeRecord;
    FenceAlertStruct mFenceAlertRecord;

    GAESurveyResultData mGAESurveyResultData;
    GAEMessageStatusData mGAEMessageStatusData;
    TargetTopicStruct mTargetTopicRecord;

    CARDRollCallRecordStruct mCARDRollCallRecord;
    CARDRollCallEventStruct mCARDRollCallEvent;
    CARDRollCallSubGroupEventStruct mCARDRollCallSubGroupEvent;
    CARDScanRecordStruct mCARDScanRecord;

    RFIDDeviceStruct m_RFIDDevice;
    NFCDeviceStruct m_NFCDevice;
    RFIDCardStruct m_RFIDCard;
    NFCCardStruct m_NFCCard;
    EDGEDeviceStruct m_EdgeDevice;

    UserStruct m_UserData;
    UserSubGroupStruct m_UserSubGroup;
    UserMainGroupStruct m_UserMainGroup;

    LoginTokenStruct m_LoginToken;

    AssistEventStruct mAssistEvent;
    AssistEventRecordStruct mAssistEventRecord;

    ApplicationFormLeaveStruct mApplicationFormLeave;

    SensorInfoStruct mSensorInfo;
    SensorDeviceStruct mSensorDevice;
    SensorFiredEventStruct mSensorFiredEvent;

    FlyInfoStruct mFlyInfo;
    WCInfoStruct mWCInfo;

    CurriculumApplyStruct mCurriculumApply;

    // firefightingevent
    std::string m_FireFightingType;
    FireFightingStruct m_FireFightingInfo;
    FireFightingUserStruct m_FireFightingUser;
    FireFightingAirUsingStruct m_FireFightingAirUsing;

    SoccerRecordingStruct m_SoccerRecording;
};

#endif
