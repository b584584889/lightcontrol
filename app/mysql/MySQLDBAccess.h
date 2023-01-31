/*
 * MySQLDBAccess.h
 *
 *  Created on: 2017/05/19
 *      Author: ikki
 */

#ifndef MySQLDBAccess_H
#define MySQLDBAccess_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include "../config.h"
#include "struct.h"
#include "../network/wqueue.h"

#define MAX_FOOTPRINT_INDEX 1000

#define MYSQLDB_CLEAR_DATA_DAYS 365

#define ERP_MD5_KEY "5kNAKSDPTmkKCueWFQynFtmuycb5r2Y84bVkBzxbVtXkWKrFgTKyaaCYBEMHrDX2"

typedef int (*CALLBACK) (sql::ResultSet *, void*);

class MySQLDBAccess
{
public:
	MySQLDBAccess(int projectID);
	~MySQLDBAccess();

    int init();
    int check_valid_table();
    int deletedb();
	int getProjectID();
	int getDatabaseSize();
	bool checkMySQLConnection();

//    int run_test();

    // footprint
//    int get_latest_idx(int nodetype, const char * nodeid);
    int write_node_footprint(
        int nodetype, const char * nodeid, int idx, const char * areaid,
        int posX, int posY, int posZ, const char * voltage, const char * temperature);

    int Transaction_commit();
    int Transaction_write_node_footprint(int nodetype, const char * nodeid, int idx, const char * areaid,
                                         int posX, int posY, int posZ, const char * voltage, const char * temperature);

    std::list<FootprintStruct*> * read_node_footprint(const char * nodeid, int max_size);
    // int write_node_footprintall(
    //     int nodetype, const char * nodeid, int idx, const char * areaid,
    //     int posX, int posY, int posZ, const char * voltage, const char * temperature);

    // int write_node_info(
    //     int nodetype, const char * macaddress, const char * nodeid, int tafid, int buildingid, int buildingfloor, const char * nodename, int idx,
    //     int posX, int posY, int posZ, double Longitude, double Latitude, const char * voltage, const char * temperature, const char * version, const char * AreaId, const char * AreaName);
    // int write_node_info(
    //     int nodetype, const char * macaddress, const char * LoRAMacaddress, const char * nodeid, int tafid, int buildingid, int buildingfloor, const char * nodename, int idx,
    //     int posX, int posY, int posZ, double Longitude, double Latitude, const char * voltage, const char * temperature, const char * version, const char * AreaId, const char * AreaName);

	int write_node_info(NodeStruct * node);
    int Transaction_write_node_info(NodeStruct * node);
    // int Transaction_write_node_info(
    //     int nodetype, const char * macaddress, const char * LoRAMacaddress, const char * nodeid, int tafid, int buildingid, int buildingfloor, const char * nodename, int idx,
    //     int posX, int posY, int posZ, double Longitude, double Latitude, const char * voltage, const char * temperature, const char * version, const char * AreaId, const char * AreaName);

	int write_AOA_node_info(AoANodeStruct * node);
	int Transaction_write_AOA_node_info(AoANodeStruct * node);


    int update_node_info_id(const char * macaddress, const char * nodeid);
    int update_node_info_name(const char * macaddress, const char * nodename);
    int update_node_info_pos(const char * macaddress, int posX, int posY, int posZ);
    int update_node_info_building(const char * macaddress, int buildingid, int buildingfloor);
    int update_node_info_blacklist(const char * macaddress, const char * anchorids);
    int update_node_info_whitelist(const char * macaddress, const char * anchorids);
    int update_node_info_joinRangingRange(const char * macaddress, int joinRangingRange);
    int update_node_info_joinNotRangingRange(const char * macaddress, int joinNotRangingRange);


    int delete_node_info(int nodetype, int idx);
    int Transaction_delete_node_info(int nodetype, int idx);

    int delete_node_info_bymac( int nodetype, const char * macaddress);
	int delete_aoa_node_info_bymac(const char * macaddress);
    int delete_all_node(int nodetype);

    // events
    int delete_event(int eventid);
    int create_event(EventStruct event);
    int clear_all_event(int projectID);
    int write_event(int projectID, int eventid, std::string nodeid, std::string action, std::string areaid, std::string issuetime);

    // FiredEvents
    std::list<FiredEventStruct*> * read_fired_events(int projectID, int unread, int prettystring, int count, int offset, int markasread);
    std::list<FiredEventStruct *> * read_fired_events_DayRange(const char * FromDatetime, const char * ToDatetime, const char * order);
    std::list<FiredEventStruct *> * read_fired_events_DayRange(const char * eventid, const char * FromDatetime, const char * ToDatetime, const char * order);
    int write_fired_events(int projectID, int eventid, int maingroupid, std::string timestart, std::string timeend, std::string eventjson);
	int write_fired_events(int projectID, int eventid, int maingroupid, std::string timestart, std::string timeend, std::string eventjson, std::string empid);
    int check_fired_events(int projectID, int eventid, std::string timestart);
    int markFiredEvent(int tableid);

    // Alert
    int write_alert_record(int nodetype, const char * macaddress);
    int write_alert0D_record(int nodetype, const char * macaddress);

    // config
    // int write_config(int projectID, std::string PROJECT_NAME, std::string PROJECT_DESCRIPTION,
    //                  std::string WGS48_Origin_X, std::string WGS48_Origin_Y, std::string MAP_North,
    //                  int BuildingID, int BuildingFloor);

    // systemconfig
    int write_sys_config(int configID, VILSSystemConfig * systemConfig);
	int get_sys_sendNotifyTime(int configID);
	//=====================================================
	// For support old project
    std::list<VILSConfig*> * read_all_config();
	// map
    int read_map(int projectID, MapStruct * map);
    std::list<AreaStruct*> * read_area(int projectID);
	//=====================================================

    //int write_map(int mapid, int projectID, std::string mapname, std::string POSX, std::string POSY, std::string SCALE, std::string COEFF,
    //      std::string mapfile, std::string MAPWIDTH, std::string MAPHEIGHT);

    // maparea
    //int delete_maparea(std::string areaid, int projectID);
    //int write_mapareashape(std::string areaid, int projectID, std::string areaname, std::string geojson);
    //int write_maparea(std::string areaid, int projectID, std::string areaname, std::string geojson, int type);


    // SectionGroup
    int create_SectionGroup(SectionGroup sectiongroup);
    int delete_SectionGroup(int GroupID);
    int write_SectionGroup(SectionGroup * sectiongroup);
    int write_SectionGroupList(std::list<SectionGroup *> * sectiongrouplist);
    bool read_SectionGroupList(int projectID, std::list<SectionGroup *> * sectiongrouplist);
	bool read_SectionGroupList(int projectID, SectionTypeEnum SectionType, std::list<SectionGroup *> * sectiongrouplist);
	std::list<SectionGroup *> * read_Section0DGroupList();

	// FenceAlert
	int create_FenceAlert(FenceAlertStruct fencealert);
	int delete_FenceAlert(int GroupID);

    // NodeInfo
    std::list<NodeStruct*> * read_nodes(int nodetype);
	std::list<NodeStruct*> * read_nodes_assigned_serno();
    NodeStruct * getNodeStruct(std::string nodeid);
    std::string fetchNodeName(std::string nodeid);
    std::string fetchLocatorName(std::string nodeid);

    // Events
    std::list<EventStruct*> * read_events();
    std::list<EventStruct*> * read_events(int projectID);
    std::list<EventStruct*> * read_events_forshow(int projectID);

	// Inspection
    std::list<InspectionGroupStruct*> * read_InspectionGroup();
	std::list<InspectionPointStruct*> * read_InspectionPoint(int GroupID);
	int write_InspectionGroupDailyRecord(InspectionGroupDailyRecord DailyRecord);
	int read_InspectionGroupDailyRecord(int GroupID, std::string Today, InspectionGroupDailyRecord & DailyRecord);
	int write_InspectionPointDailyRecord(InspectionPointDailyRecord DailyRecord);
	int read_InspectionPointDailyRecord(int GroupID, int PointID, std::string Today, InspectionPointDailyRecord & DailyRecord);
	int write_InspectorBehaviorData(InspectorBehaviorData InspectorBehaviorData);

    // systemconfig
    bool read_sys_config(VILSSystemConfig * sysConfig);
    bool update_systemconfigudpport(VILSSystemConfig sysConfig);
    bool update_AlgorithmConfig(VILSSystemConfig sysConfig);
    bool update_LoRAWANConfig(VILSSystemConfig sysConfig);

    // courses
    CourseStruct * read_Course(const char * courseId);
    std::list<CourseStruct*> * read_allCourses(int projectID);
    void write_Course(CourseStruct * curCourse);
    void delete_Course(CourseStruct * curCourse);

    // std::list<ResourceStruct*> * read_allResources(int projectID);
    // void write_Resource(ResourceStruct * curResource);

    CourseRollCallRecordStruct * read_rollcallRecord(int projectID, std::string infoOfDate, std::string courseId);
    std::list<CourseRollCallRecordStruct *> * read_rollcallRecordDayRange(const char * FromDatetime, const char * ToDatetime);
    void write_rollcallRecord(CourseRollCallRecordStruct * curRecord);

    void write_AreaInOutEvent(AreaInOutEventStruct * inoutEvent);
    void write_LoRaGatewayInfo(LoRaGatewayInfoStruct * gateway);
	void delete_LoRaGateway(std::string gwid);

    void write_TagGPSInfo(GPSInfoStruct * GPSInfo);

    void update_Resident(UserResidentStruct * Resident);
	void update_NodeResident(const char * macaddress, const char * serno);
	void update_NodeEmployee(const char * macaddress, const char * empid);
	void update_NodeButtontype(const char * macaddress, const char * Buttontype);
	void update_NodeSensorAlert(const char * macaddress, const char * SensorAlert);
	void delete_Resident(UserResidentStruct * Resident);
    bool check_exist_Resident(std::string serno);
    UserResidentStruct * getResident(const char * serno);
	std::list<UserResidentStruct *> * getResidentList();

	void update_Employee(UserEmployeeStruct * Employee);
	void delete_Employee(UserEmployeeStruct * Employee);
    bool check_exist_Employee(std::string empid);
    UserEmployeeStruct * getEmployee(std::string empid);
	std::list<UserEmployeeStruct *> * getEmployeeList();

    void update_EmployeeEventReport(EmployeeEventReportStruct * EmployeeEventReportRecord);
    std::list<EmployeeEventReportStruct*> * getEmployeeEventReport(std::string empid, std::string report_start_date, std::string report_end_date);

    void update_User(UserEmployeeStruct * Employee);
	void delete_User(UserEmployeeStruct * Employee);
    bool check_exist_User(std::string empid);
    bool check_valid_User(const char * account, const char * password);
	bool check_valid_User(const char * account, const char * password, UserStruct * UserData);
	bool check_valid_User(const char * account, const char * password, const char * userid, UserStruct * UserData);
	int check_Account_Login(std::string account, std::string key);
	bool check_Account_Login(std::string account, std::string userid, std::string key);
	bool update_Account_LoginKey(std::string account, int userid, std::string key, std::string token);
	std::string query_Account_LoginKey(std::string accountid, std::string userid, std::string token);
	// std::string query_Account_LoginKey(std::string account, std::string token);
	// bool update_user_password(const char * account, const char * password);
	bool update_user_password(std::string accountid, int userid, std::string password);
	bool update_user_uwbcards(int userid, const char * uwbcards);

	bool update_UserStruct(UserStruct * user);
	bool update_UserPoistion(int userid, double posX, double posY, double posZ);
	bool update_UserLocation(int userid, std::string Latitude, std::string Longitude);
	bool update_UserGroup(UserGroupStruct * usergroup);
	bool update_UserSubGroup(UserSubGroupStruct * usersubgroup);
	bool update_UserMainGroup(UserMainGroupStruct * usermaingroup);

    std::list<UserStruct*> * get_Users();
	UserStruct * getUser(std::string account);
	UserStruct * getUserByUserId(int userid);
	std::list<UserStruct*> * getUserByUWBCard(const char * uwbcard);
	std::string getUserName(int userid);
	std::list<UserGroupStruct*> * get_UserGroups();
	std::list<UserSubGroupStruct*> * get_UserSubGroups();
	std::list<UserMainGroupStruct*> * get_UserMainGroups();
	UserSubGroupStruct * get_UserSubGroup(int subgroupid);
	UserSubGroupStruct * get_UserSubGroupByCurriculumRecordID(int curriculumrecordid);
	UserMainGroupStruct * get_UserMainGroup(int maingroupid);

    int getUnassignResidentCount(int nodetype);

	// void update_PushNotifyUser(PushNotifyUserStruct * user);
    // bool check_exist_PushNotifyUser(std::string account);
	// std::list<PushNotifyUserStruct*> * get_PushNotifyUsers();
	// PushNotifyUserStruct * getPushNotifyUser(std::string account);
	// PushNotifyFunctionUserStruct * getNotiftyUserByFunction(int function);

	std::list<PushNotifySettingStruct*> * get_PushNotifySettings();
	std::list<TriggerPushMessageStruct*> * get_TriggerPushMessages();
	std::list<PushNotifyMessageRecordStruct*> * get_PushNotifyMessageRecords(const char * FromDatetime, const char * ToDatetime);
	PushNotifyMessageRecordStruct * get_PushNotifyMessageRecord(std::string hashkey);
	PushNotifyMessageRecordStruct * get_PushNotifyMessageRecord(int PushNotifyMessageRecorddbId);
	std::list<SurveyEventRecordStruct*> * get_LeaveForm_SurveyEventRecord();
	ApplicationFormLeaveStruct * get_LeaveForm(std::string formid);
	void update_LeaveForm(ApplicationFormLeaveStruct * record);
	void update_SensorInfo(SensorInfoStruct * record);
	void update_SensorDevice(SensorDeviceStruct * record);
	void update_SensorFiredEvent(SensorFiredEventStruct * record);
	void Transaction_update_FlyInfo(FlyInfoStruct * record);
	void Transaction_update_SensorInfo(SensorInfoStruct * record);
	void Transaction_update_SensorDevice(SensorDeviceStruct * record);
	void Transaction_update_WCInfo(WCInfoStruct * record);
	std::list<SensorDeviceStruct*> * get_SensorDevices();
	std::list<FlyInfoStruct*> * get_FlyInfos(std::string FDATE);
	std::list<FlyGateInfoStruct*> * get_FlyGateInfos();
	std::string get_SensorGateInfo(std::string sensorid);

	int get_WCInfoSendNotifyStatus(std::string GID);

	std::list<SurveyEventRecordStruct*> * get_Assist_SurveyEventRecords();
	SurveyEventRecordStruct * get_SurveyEventRecord(int PushNotifyMessageRecorddbId);
	AssistEventStruct * get_AssistEvent(std::string eventid);
	void update_AssistEvent(AssistEventStruct * event);

	std::list<SurveyEventRecordStruct*> * get_Curriculum_SurveyEventRecords();
	CurriculumRecordStruct * get_CurriculumRecord(std::string recordid);
	CurriculumStruct * get_Curriculum(int curriculumid);
	CurriculumTypeStruct * get_CurriculumType(int curriculumtypeid);
	CurriculumApplyStruct * get_CurriculumApply(std::string recordid, int userid);
	void update_CurriculumApply(CurriculumApplyStruct * record);

	void update_PushNotifyMessage(PushNotifyMessageStruct * msg);
	void Transaction_write_PushNotifyMessage(PushNotifyMessageStruct * msg);

	void update_PushNotifyMessageRecord(PushNotifyMessageRecordStruct * msgrecord);
	void Transaction_write_PushNotifyMessageRecord(PushNotifyMessageRecordStruct * msgrecord);

	int check_PushNotifyMessageRecord_hashkey(std::string hashkey);
	int check_SurveyEventRecord_hashkey(std::string hashkey);

	void update_node_PushNotifySetting(int nodetype, const char * nodeid, int role);

	void update_GAESurveyResultData(GAESurveyResultData * result);
	void update_GAEMessageStatusData(GAEMessageStatusData * status);
	GAESurveyResultData * get_SurveyResult(std::string hashkey, std::string messagehashkey);

	void update_TargetTopicRecord(TargetTopicStruct * result);
	void Transaction_write_TargetTopicRecord(TargetTopicStruct * result);

	std::string createNewTopic();
	std::list<TargetTopicStruct*> * get_TargetTopics();
	TargetTopicStruct * get_UserTargetTopic(int userid, std::string accountid);
	TargetTopicStruct * get_TargetTopic(std::string target, std::string type, std::string id, std::string accountid, std::string maingroupid);
	TargetTopicStruct * get_TargetTopicFromTopic(std::string topic);
	TargetTopicStruct * get_TargetTopicFromTarget(std::string target);

	void update_RFIDCardRecord(RFIDCardStruct * result);
	void Transaction_write_RFIDCardRecord(RFIDCardStruct * result);

	void update_RFIDCardRollCallRecord(std::string cardid);

	void update_NFCCardRecord(NFCCardStruct * result);
	void Transaction_write_NFCCardRecord(NFCCardStruct * result);
	void Transaction_write_MaingroupNFCCardRecord(NFCCardStruct * result);

	// void update_NFCCardRollCallRecord(std::string cardid);
	void clear_UnknownNFCCardAlive(std::string updatetime);
	void clear_UnknownRFIDCardAlive(std::string updatetime);
	void remove_RFIDCard_user(int userid);
	void remove_NFCCard_user(int userid);
	void remove_MaingroupNFCCard_user(int userid, std::string maingroups);
	void remove_UWBCard_user(int userid);
	void update_RFIDCard_user(std::string cardid, int userid);
	void update_NFCCard_user(std::string cardid, int userid);
	void update_MaingroupNFCCard_user(std::string cardid, int userid, std::string maingroups);
	void update_UWBCard_user(std::string macaddress, int userid);
	std::list<RFIDCardStruct*> * get_RFIDCards();
	std::list<RFIDCardStruct*> * get_UnknownRFIDCards();
	std::list<NFCCardStruct*> * get_NFCCards();
	std::list<NFCCardStruct*> * get_MaingroupNFCCards();
	std::list<NFCCardStruct*> * get_UnknownNFCCards();
	std::list<RFIDDeviceStruct*> * get_RFIDDevices();
	std::list<NFCDeviceStruct*> * get_NFCDevices();
	std::list<EDGEDeviceStruct*> * get_EDGEDevices();
	void update_RFIDDevice(RFIDDeviceStruct * device);
	void update_NFCDevice(NFCDeviceStruct * device);
	void update_EDGEDevice(EDGEDeviceStruct * device);

	std::string create_verifykey(std::string tablename);

	void update_CARDRollCallRecord(CARDRollCallRecordStruct * record);
	void Transaction_write_CARDRollCallRecord(CARDRollCallRecordStruct * record);

	void update_CARDRollCallEvent(CARDRollCallEventStruct * event);
	void Transaction_write_CARDRollCallEvent(CARDRollCallEventStruct * event);

	void update_CARDRollCallSubGroupEvent(CARDRollCallSubGroupEventStruct * event);
	void Transaction_write_CARDRollCallSubGroupEvent(CARDRollCallSubGroupEventStruct * event);

	CARDRollCallEventStruct * get_CARDRollCallEvent(int rollcallid, int eventid);
	CARDRollCallEventStruct * get_CARDRollCallEvent(int rollcallid, std::string StartDateTime, std::string EndDateTime, std::string maingroups, bool bCreateNew);
	std::list<CARDRollCallRecordStruct*> * get_CARDRollCallRecords(int rollcallid, int rollcalleventid);

	void update_CARDScanRecord(CARDScanRecordStruct * record);
	std::list<CARDScanRecordStruct*> * get_CARDScanRecord(std::string StartDateTime, std::string EndDateTime);

	//
	// maplayer
	//
	void create_maplayer(MapLayerStruct * maplayer);
	void update_maplayer(MapLayerStruct maplayer);
	int get_first_maplayerid();
	std::list<MapLayerStruct*> * read_maplayers();
	std::list<MapLayerAreaStruct*> * read_maplayerarea_not_wall();
	std::list<MapLayerAreaStruct*> * read_maplayerarea_type(int type);
	std::string fetchAreaName(std::string areaid);
	MapLayerAreaStruct * getAreaStruct(std::string areaid);
	std::string fetchMaplayerName(int layerid);
	MapLayerStruct * getMapLayerStruct(int layerid);

	void create_maplayerarea(MapLayerAreaStruct * maplayerarea);
	void update_maplayerarea(MapLayerAreaStruct maplayerarea);
	void update_maplayerareashape(MapLayerAreaStruct maplayerarea);
	void delete_maplayerarea(MapLayerAreaStruct maplayerarea);

	void write_bodyinfo_Weight(const char * nodeid, std::string Weight);
	void write_bodyinfo_HeartBeat(const char * nodeid, std::string HeartBeat);
	void write_bodyinfo_all(std::string accountid, std::string recorderid, std::string weight, std::string heartbeat, std::string bloodsugar, std::string systolicbloodpressure, std::string diastolicbloodpressure, std::string bodytemperature, std::string bodyfat, std::string bloodoxygen, std::string datetimet);

	std::list<BodyInfoStruct*> * get_bodyinfo_Weight(const char * nodeid, int count);
	std::list<BodyInfoStruct*> * get_bodyinfo_HeartBeat(const char * nodeid, int count);


    // LoRa Gateway
    std::list<LoRaGatewayInfoStruct*> * read_LoRaGateway();

	std::list<AoANodeStruct*> * read_AoA_nodes();

    std::string get_area_name(std::string areaid);

	SurveyStruct * readSurveyFromTitle(std::string title);
	std::list<SurveyStruct*> * readSurveys(std::list<std::string> * surveys);
	std::list<SurveyItemStruct*> * readSurveyItems(int surveyid);

	std::list<CARDRollCallStruct*> * read_CARDRollCallInfo();
	CARDRollCallStruct * read_CARDRollCallInfo(int rollcallid);

	std::list<AreaInOutEventStruct*> * getNodeAreaInOutList(const char * nodeid, const char * FromDatetime, const char * ToDatetime, const char * order);
	AreaInOutEventStruct * getNodeLastAreaInOutList(const char * nodeid, const char * areaid, const char * FromDatetime, int action);
    AnalyzeDayInfoStruct * getAreaInOutAnalyzeDayInfo(const char * nodeid, const char * FromDatetime, const char * ToDatetime);
	AnalyzeDayInfoStruct * getAreaInOutAnalyzeDayInfoFromCache(const char * nodeid, const char * FromDatetime, const char * ToDatetime);
    AnalyzeDayInfoStruct * getAreaInOutAnalyzeDayInfoByAreaId(const char * nodeid, const char * areaid, int action, const char * FromDatetime, const char * ToDatetime);
    AnalyzeDayInfoStruct * getGPSAnalyzeDayInfo(int nodetype, const char * nodeid, const char * FromDatetime, const char * ToDatetime);
	AnalyzeDayInfoStruct * getGPSAnalyzeDayInfoFromCache(int nodetype, const char * nodeid, const char * FromDatetime, const char * ToDatetime);
    //AnalyzeDayInfoStruct * getCourseAnalyzeDayInfo(const char * nodeid, const char * FromDatetime, const char * ToDatetime);
    AnalyzeDayInfoStruct * getAlertAnalyzeDayInfo(const char * macaddress, const char * FromDatetime, const char * ToDatetime);
	AnalyzeDayInfoStruct * getAlertAnalyzeDayInfoFromCache(const char * macaddress, const char * FromDatetime, const char * ToDatetime);
    AnalyzeDayInfoStruct * getEventAnalyzeDayInfo(const char * FromDatetime, const char * ToDatetime);

	bool clear_old_data(const char * table, const char * column, const char * datetime);

	std::list<AssistStruct*> * read_Assists();
	AssistStruct * read_Assist(int assistid);
	std::list<AssistEventStruct*> * read_AssistEvents();

	AssistEventRecordStruct * get_AssistEventRecord(int assistid, int assisteventid,std::string eventday, std::string eventtime);
	bool createAssistEventRecord(AssistEventRecordStruct * record);
	bool updateAssistEventRecord(AssistEventRecordStruct * record);

	bool updateSoccerRecord(SoccerRecordingStruct * record);
	bool createSoccerRecord(SoccerRecordingStruct * record);

	std::list<std::string> * getStorageImagesFromMessagehashkey(std::string messagehashkey);

	FireFightingStruct * createFireFightingCase(std::string address, std::string lat, std::string lng, int maingroupid);
	bool updateFireFightingCase(FireFightingStruct * record);
	bool updateFireFightingUser(FireFightingUserStruct * record);
	bool updateFireFightingAirUsing(FireFightingAirUsingStruct * record);
	bool updateUserFireFightingAirUsing(int userid, float airusing);
	bool addFireFightingEvent(std::string eventtype, std::string casenumber, std::string jsonstring);
	std::list<FireFightingStruct*> * get_OpenFireFightingCases();
	std::list<FireFightingUserStruct*> * get_FireFightingUsers(std::string casenumber);
	std::list<FireFightingEventStruct*> * get_FireFightingEvents(std::string casenumber, std::string eventtype);
	std::list<SoccerRecordingStruct*> * get_SoccerRecording();
private:

	int mProjectID;
    sql::Driver * mMySQLDriver;
	std::string mSQLDBname;

    std::list<AnalyzeDayInfoStruct *> mAreaInOutAnalyzeDayInfoStructList;
	std::list<AnalyzeDayInfoStruct *> mGPSAnalyzeDayInfoStructList;
	std::list<AnalyzeDayInfoStruct *> mAlertAnalyzeDayInfoStructList;

//    bool bConnectd;
//    sql::Connection * mMySQLCon;
//    sql::Statement * mMySQLStmt;

//    bool bQueryConnectd;
//    sql::Connection * mMySQLQueryCon;
//    sql::Statement * mMySQLQueryStmt;

    wqueue<std::string> mMysqlTransactionQueue;

    bool checkDBExist();
    int create_table();

//    int sql_execute_new(const char * cmd);
//    bool sql_executeQuery_test(const char * cmd, CALLBACK callback, void * retValue);

    int sql_execute(const char * cmd);
	int sql_executeUpdate(const char * cmd);
	int sql_execute_get_insert_id(const char * cmd);
	bool sql_executeQuery_new(const char * cmd, CALLBACK callback, void * retValue);
    // sql::ResultSet * sql_executeQuery(const char * cmd);

    bool check_resident_table();
    bool create_resident_table();
    bool check_employee_table();
    bool create_employee_table();
    bool check_User_table();
    bool create_User_table();
    bool create_User_data(int userid, int groupid, const char * account, const char * name, const char * unit, const char * title, const char * gender, const char * password);

    bool check_UserGroup_table();
    bool create_UserGroup_table();

    bool create_UserGroup_data(int groupid, const char * groupname, const char * permission);
	bool delete_UserGroup_data(int groupid);

	bool check_UserSubGroup_table();
    bool create_UserSubGroup_table();
	bool create_UserSubGroup_data( const char * subgroupname, const char * permission);

	bool check_UserMainGroup_table();
    bool create_UserMainGroup_table();
	bool create_UserMainGroup_data( const char * maingroupname, const char * permission);

	// bool check_UserMainGroupConnection_table();
    // bool create_UserMainGroupConnection_table();

	bool create_Survey_data(const char * surveyid , const char * title, const char * description, int type, int userid, const char * maingroups, const char * subgroups);
	bool create_SurveyItem_data(const char * surveyid , const char * description);

    bool check_FiredEvents_table();
    bool create_FiredEvents_table();

    bool check_NodeInfo_table();
    bool check_SectionGroup_table();
    bool create_SectionGroup_table();
    bool check_AlertRecord_table();
    bool create_AlertRecord_table();
    //bool check_config_table();
    //bool create_config_table();
    bool check_systemconfig_table();
    bool create_systemconfig_table();
    // bool check_MapArea_table();
    // bool create_MapArea_table();

    bool check_Course_table();
    bool create_Course_table();
    bool check_CourseRollCallRecord_table();
    bool create_CourseRollCallRecord_table();

    bool check_AreaInOutRecord_table();
    bool create_AreaInOutRecord_table();

    bool check_GPSInfoRecord_table();
    bool create_GPSInfoRecord_table();

    bool check_LoRaGatewayInfo_table();
    bool create_LoRaGatewayInfo_table();

    bool check_EmployeeEventReport_table();
    bool create_EmployeeEventReport_table();

    bool check_PushNotifyUser_table();
    bool create_PushNotifyUser_table();

    bool check_maplayer_table();
    bool create_maplayer_table();

	bool check_maplayerarea_table();
    bool create_maplayerarea_table();

    bool check_PushNotifyMessage_table();
    bool create_PushNotifyMessage_table();

    bool check_PushNotifyMessageRecord_table();
    bool create_PushNotifyMessageRecord_table();

	bool check_CannedMessage_table();
    bool create_CannedMessage_table();

    bool check_PushNotifySetting_table();
    bool create_PushNotifySetting_table();

    bool check_TriggerPushMessage_table();
    bool create_TriggerPushMessage_table();

    bool check_AoANodeInfo_table();
    bool create_AoANodeInfo_table();

    bool check_BodyInfo_table();
    bool create_BodyInfo_table();

    bool check_InspectionGroup_table();
    bool create_InspectionGroup_table();

    bool check_InspectionPoint_table();
    bool create_InspectionPoint_table();

	bool check_InspectionGroupDailyRecord_table();
    bool create_InspectionGroupDailyRecord_table();

	bool check_InspectionPointDailyRecord_table();
    bool create_InspectionPointDailyRecord_table();

	bool check_InspectorBehaviorData_table();
    bool create_InspectorBehaviorData_table();

	bool check_FenceAlert_table();
    bool create_FenceAlert_table();

	bool check_DeviceMaintenanceRecord_table();
    bool create_DeviceMaintenanceRecord_table();

	bool check_Survey_table();
    bool create_Survey_table();

	bool check_SurveyItem_table();
    bool create_SurveyItem_table();

	bool check_SurveyEventRecord_table();
    bool create_SurveyEventRecord_table();

	// bool check_SurveyRecord_table();
    // bool create_SurveyRecord_table();

	bool check_SurveyResult_table();
    bool create_SurveyResult_table();

	bool check_PushNotifyMessageStatus_table();
    bool create_PushNotifyMessageStatus_table();

	bool check_TargetTopic_table();
    bool create_TargetTopic_table();

	bool check_RFIDDevice_table();
    bool create_RFIDDevice_table();

	bool check_RFIDCard_table();
    bool create_RFIDCard_table();

	bool check_NFCDevice_table();
    bool create_NFCDevice_table();

	bool check_NFCCard_table();
    bool create_NFCCard_table();

	bool check_MaingroupNFCCard_table();
    bool create_MaingroupNFCCard_table();

	bool check_EDGEDevice_table();
    bool create_EDGEDevice_table();

	bool check_CARDRollCall_table();
    bool create_CARDRollCall_table();

	bool check_CARDRollCallEvent_table();
    bool create_CARDRollCallEvent_table();

	bool check_CARDRollCallSubGroupEvent_table();
    bool create_CARDRollCallSubGroupEvent_table();

	bool check_CARDRollCallRecord_table();
    bool create_CARDRollCallRecord_table();

	bool check_CARDScanRecord_table();
    bool create_CARDScanRecord_table();

	bool check_uiblock_table();
    bool create_uiblock_table();

	bool check_subgroupuiblock_table();
    bool create_subgroupuiblock_table();

	bool check_weekschedule_table();
    bool create_weekschedule_table();

	bool check_StorageFile_table();
    bool create_StorageFile_table();

	bool check_StorageImage_table();
    bool create_StorageImage_table();

	// bool check_PhotoAlbumImage_table();
    // bool create_PhotoAlbumImage_table();

	bool check_applicationformleave_table();
    bool create_applicationformleave_table();

	bool check_logintoken_table();
    bool create_logintoken_table();

	bool check_assist_table();
    bool create_assist_table();

	bool check_assistEvent_table();
    bool create_assistEvent_table();

	bool check_assistEventRecord_table();
    bool create_assistEventRecord_table();

	bool check_billboardRecord_table();
    bool create_billboardRecord_table();

	bool check_register_table();
    bool create_register_table();

	bool check_Curriculum_table();
    bool create_Curriculum_table();

	// bool check_CurriculumShared_table();
    // bool create_CurriculumShared_table();

	bool check_CurriculumRecord_table();
    bool create_CurriculumRecord_table();

	bool check_CurriculumType_table();
    bool create_CurriculumType_table();

	bool check_CurriculumPrefix_table();
    bool create_CurriculumPrefix_table();

	bool check_CurriculumApply_table();
    bool create_CurriculumApply_table();

	bool check_sensorinfo_table();
    bool create_sensorinfo_table();

	bool check_sensordevice_table();
    bool create_sensordevice_table();

	bool check_sensorevent_table();
    bool create_sensorevent_table();

	bool check_FlyInfo_table();
    bool create_FlyInfo_table();

	bool check_APIKey_table();
    bool create_APIKey_table();

	bool check_KIAWCInfo_table();
    bool create_KIAWCInfo_table();

	bool check_FireFighting_table();
    bool create_FireFighting_table();

	bool check_FireFightingUser_table();
    bool create_FireFightingUser_table();

	bool check_FireFightingEvent_table();
    bool create_FireFightingEvent_table();

	bool check_FireFightingAirUsing_table();
    bool create_FireFightingAirUsing_table();

	bool check_SoccerRecord_table();
    bool create_SoccerRecord_table();

    void convert_utf8_to_utf8mb4(const char * tablename);
    bool tableExists(const char * tableName);
    bool createNonExistColumn(const char * tableName, const char * columnName, const char * colDefinition);
    bool columnExists(const char * tableName, const char * columnName);
    int create_column(const char * tableName, const char * columnName, const char * colDefinition);

	bool update_to_alert_cache(const char * macaddress, int count, const char * FromDatetime, const char * ToDatetime, const char * min_time, const char * max_time);
	bool update_to_AreaInOut_cache(const char * nodeid, int count, const char * FromDatetime, const char * ToDatetime, const char * min_time, const char * max_time, const char * area_ids, const char * area_names);
	bool update_to_GPS_cache(int nodetype, const char * nodeid, int count, const char * FromDatetime, const char * ToDatetime, const char * min_time, const char * max_time);

	bool add_canned_message(int dbid, int type, std::string menutitle, std::string title, std::string message);
	bool add_APIKey(int maingroupid, int subgroupid, std::string key, std::string description);

    std::string getPrettyJSON(std::string eventjson);
	std::string getInspectionPointPrettyJSON(std::string eventjson);
    std::string getInspectionPointTravelPrettyJSON(std::string eventjson);
    std::string getInspectionGroupPrettyJSON(std::string eventjson);

    static int get_count_callback(sql::ResultSet *, void*);
	static int get_size_callback(sql::ResultSet *, void*);
	static int get_stringvalue_callback(sql::ResultSet *, void*);
    static int get_PROJECT_ID_callback(sql::ResultSet *, void*);
    static int get_nodename_callback(sql::ResultSet *, void*);
    static int get_areaname_callback(sql::ResultSet *, void*);
    static int get_SystemConfig_callback(sql::ResultSet *, void*);
    static int get_nodelist_callback(sql::ResultSet *, void*);
    static int get_node_callback(sql::ResultSet *, void*);
    static int get_footprintlist_callback(sql::ResultSet *, void*);

	static int get_InspectionGrouplist_callback(sql::ResultSet *, void*);
	static int get_InspectionPointlist_callback(sql::ResultSet *, void*);
	static int get_InspectionGroupDailyRecord_callback(sql::ResultSet *, void*);
	static int get_InspectionPointDailyRecord_callback(sql::ResultSet *, void*);

    static int get_eventlist_callback(sql::ResultSet *, void*);
    static int get_sectiongrouplist_callback(sql::ResultSet *, void*);
    static int get_unread_FiredEventlist_callback(sql::ResultSet *, void*);
    static int get_FiredEventlist_callback(sql::ResultSet *, void*);

    static int get_Course_callback(sql::ResultSet *, void*);
    static int get_Courselist_callback(sql::ResultSet *, void*);
    static int get_CourseRollCallRecord_callback(sql::ResultSet *, void*);
    static int get_CourseRollCallRecordList_callback(sql::ResultSet *, void*);

    static int get_LoRaGatewayList_callback(sql::ResultSet *, void*);
    static int get_AreaInOutAnalyzeDayInfo_callback(sql::ResultSet *, void*);
    static int get_AnalyzeDayInfo_callback(sql::ResultSet *, void*);
    static int get_User_callback(sql::ResultSet *, void*);
    static int get_UserList_callback(sql::ResultSet *, void*);
	static int get_UserGroupList_callback(sql::ResultSet *, void*);
	static int get_UserSubGroupList_callback(sql::ResultSet *, void*);
	static int get_UserMainGroupList_callback(sql::ResultSet *, void*);
	static int get_UserSubGroup_callback(sql::ResultSet *, void*);
	static int get_UserMainGroup_callback(sql::ResultSet *, void*);
	static int get_resident_callback(sql::ResultSet *, void*);
	static int get_residentList_callback(sql::ResultSet *, void*);
    static int get_employee_callback(sql::ResultSet *, void*);
	static int get_employeeList_callback(sql::ResultSet *, void*);
    static int get_EmployeeEventReportList_callback(sql::ResultSet *, void*);
	static int get_PushNotifySettingList_callback(sql::ResultSet *, void*);
	static int get_TriggerPushMessageList_callback(sql::ResultSet *, void*);
	static int get_StringValueList_callback(sql::ResultSet *, void*);
	static int get_NodeAreaInOut_callback(sql::ResultSet *, void*);
	static int get_NodeAreaInOutList_callback(sql::ResultSet *, void*);
	static int get_maplayer_callback(sql::ResultSet *, void*);
	static int get_maplayerList_callback(sql::ResultSet *, void*);
	static int get_AoANodeList_callback(sql::ResultSet *, void*);
	static int get_maplayerareaList_callback(sql::ResultSet *, void*);
	static int get_maplayerarea_callback(sql::ResultSet *, void*);
	static int get_maplayerareaname_callback(sql::ResultSet *, void*);
	static int get_maplayername_callback(sql::ResultSet *, void*);
	static int get_bodyinfoWeightList_callback(sql::ResultSet *, void*);
	static int get_bodyinfoHeartBeatList_callback(sql::ResultSet *, void*);
	static int get_maplayerid_callback(sql::ResultSet *, void*);
	static int get_Survey_callback(sql::ResultSet *, void*);
	static int get_SurveyList_callback(sql::ResultSet *, void*);
	static int get_SurveyItemList_callback(sql::ResultSet *, void*);
	static int get_PushNotifyMessageRecord_callback(sql::ResultSet *, void*);
	static int get_PushNotifyMessageRecordList_callback(sql::ResultSet *, void*);
	static int get_SurveyEventRecord_callback(sql::ResultSet *, void*);
	static int get_CurriculumRecord_callback(sql::ResultSet *, void*);
	static int get_Curriculum_callback(sql::ResultSet *, void*);
	static int get_CurriculumType_callback(sql::ResultSet *, void*);
	static int get_CurriculumApply_callback(sql::ResultSet *, void*);
	static int get_SurveyEventRecordList_callback(sql::ResultSet *, void*);
	static int get_ApplicationFormLeave_callback(sql::ResultSet *, void*);
	static int get_RFIDCardList_callback(sql::ResultSet *, void*);
	static int get_NFCCardList_callback(sql::ResultSet *, void*);
	static int get_RFIDDeviceList_callback(sql::ResultSet *, void*);
	static int get_NFCDeviceList_callback(sql::ResultSet *, void*);
	static int get_EDGEDeviceList_callback(sql::ResultSet *, void*);
	static int get_CARDRollCallStruct_callback(sql::ResultSet *, void*);
	static int get_CARDRollCallStructList_callback(sql::ResultSet *, void*);
	static int get_CARDRollCallEvent_callback(sql::ResultSet *, void*);
	static int get_CARDRollCallRecordStructList_callback(sql::ResultSet *, void*);
	static int get_CARDScanRecordStructList_callback(sql::ResultSet *, void*);
	static int get_TargetTopicStructList_callback(sql::ResultSet *, void*);
	static int get_TargetTopicStruct_callback(sql::ResultSet *, void*);
	static int get_SurveyResult_callback(sql::ResultSet *, void*);
	static int get_VILSConfiglist_callback(sql::ResultSet * res, void * pvalue);
	static int get_map_callback(sql::ResultSet * res, void * pvalue);
	static int get_arealist_callback(sql::ResultSet *, void*);
	static int get_AssistStruct_callback(sql::ResultSet *, void*);
	static int get_AssistStructList_callback(sql::ResultSet *, void*);
	static int get_AssistEventStruct_callback(sql::ResultSet *, void*);
	static int get_AssistEventStructList_callback(sql::ResultSet *, void*);
	static int get_AssistEventRecordStruct_callback(sql::ResultSet *, void*);
	static int get_SensorDeviceList_callback(sql::ResultSet *, void*);
	static int get_FlyInfoList_callback(sql::ResultSet *, void*);
	static int get_FlyGateInfoList_callback(sql::ResultSet *, void*);
	static int get_FireFightingList_callback(sql::ResultSet *, void*);
	static int get_FireFightingUserList_callback(sql::ResultSet *, void*);
	static int get_FireFightingEventList_callback(sql::ResultSet *, void*);
	static int get_FireFightingAirUsing_callback(sql::ResultSet *, void*);
	static int get_SoccerRecordList_callback(sql::ResultSet *, void*);

	void printSQLException(sql::SQLException &e);
	std::string get_role_default_functions(int nodetype, int role);
};


#endif
