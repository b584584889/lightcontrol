/*
 *  WebUDPParserService.h
 *
 *  Created on: 2019/04/15
 *      Author: ikki
 */
#ifndef __WebUDPParserService_h__
#define __WebUDPParserService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../DeviceManager/DeviceManager.h"
#include "../network/wqueue.h"
#include "../network/WorkItem.h"
#include "../network/DataItem.h"
#include "../network/UDPDataItem.h"
#include "../network/NotificationItem.h"
#include "../network/inter_thread_pubsub.hpp"
#include "../config.h"
#include "../MQTT/MQTTDataItem.h"

typedef struct
{
    int index;
    bool status;
    std::string serviceString;
    std::string statusString;
} StatusStruct;

typedef struct
{
    int projectid;
    std::string tagmacaddress;
    std::string RangingDiagnosisString;
} RangingDiagnosisStruct;

typedef struct
{
    int projectId;
    std::string lastRangingString;
} LastRangingStringStruct;

typedef struct
{
    int projectId;
    std::string CloudPointString;
} AoACloudPointStringStruct;

typedef struct
{
    int projectId;
    std::string UnknownPositionTagString;
} UnknownPositionTagStringStruct;

typedef struct
{
    int projectId;
    std::string LoRAMacaddress;
    std::string LocatorRSSIInfoString;
} LocatorRSSIInfoStringStruct;

typedef struct
{
    std::string formatted_address;
    double location_lat;
    double location_lng;
    double northeast_lat;
    double northeast_lng;
    double southwest_lat;
    double southwest_lng;
} GeoCodeStruct;

#ifdef ENABLE_SEND_BODYINFO_ALERT
typedef struct
{
    int projectId;
    int userid;
    std::string sensorid;
    int HR_pre;
    int HR;
    int HR_alert_type; // 0: normal, 1: warning 2: danger
    int SPO2_pre;
    int SPO2;
    int SPO2_alert_type; // 0: normal, 1: warning 2: danger
    time_t alerttime;
} BodyinfoAlertStruct;
#endif

using namespace std;

class WebUDPParserService : public Thread
{
public:
    WebUDPParserService(wqueue<WorkItem*> & MySQLJobQueue, wqueue<DataItem*> & MainJobQueue, wqueue<UDPDataItem*> & udpqueue, wqueue<NotificationItem*> & NotifiactionQueue, DeviceManager * pDevManager, DBManager * dbManager);
    bool status();
    std::string statusString();

    void * run();
    void update_service_status(int index, bool status, const char * serviceString, std::string statusString);
    void update_RangingDiagnosisString(int projectId, const char * tagmacaddress, std::string RangingDiagnosisString);
    void update_LastRangingString(int projectId, std::string LastRangingString);
    void update_LastRangingStringEx(int projectId, std::string LastRangingString);
    void update_PushNotifyStatus(int projectId, std::string waitkey, int statusvalue, std::string message);
    void update_SendATcmdStatus(int projectId, std::string waitkey, int statusvalue, std::string message);
    void update_MQTTMessage(int projectId, const char * macaddress, std::string message);
    void send_lightCommand(std::string payload);
    void update_AoARangingCloudPoint(int projectId, std::string CloudPointString);
    void update_AoAAngleVectorCloudPoint(int projectId, std::string CloudPointString);
    void update_AoAAngleCloudPoint(int projectId, std::string CloudPointString);
    void update_ForacareData(int projectId, std::string ForacareDataString);
    void update_UnknownPositionTagString(int projectId, std::string UnknownPositionTagString);
    void update_LocatorRSSIString(int projectId, std::string LoRAMacaddress, std::string strRSSIInfo);
    //int mDataValidTime;

private:

    std::list<StatusStruct *> mServiceStatusList;
    std::list<RangingDiagnosisStruct *> mRangingDiagnosisList;
    std::list<LastRangingStringStruct *> mLastRangingStringList;
    std::list<LastRangingStringStruct *> mLastRangingStringExList;
    std::list<AoACloudPointStringStruct *> mAoARangingCloudPointList;
    std::list<AoACloudPointStringStruct *> mAoAAngleVectorCloudPointList;
    std::list<AoACloudPointStringStruct *> mAoAAngleCloudPointList;
    std::string mForacareDataString;
    std::list<UnknownPositionTagStringStruct *> mUnknownPositionTagList;
    std::list<LocatorRSSIInfoStringStruct *> mLocatorRSSIInfoList;

#ifdef ENABLE_SEND_BODYINFO_ALERT
    std::list<BodyinfoAlertStruct *> mBodyinfoAlertList;
#endif

    bool mInitial;
    std::string mStatus;

    ITPS::Publisher<std::string> * mFireFightingPublisher;
    ITPS::Publisher<WorkItem*> * mSensorBodyInfoPublisher;

    time_t mSystemStartTime;
    SystemInfoStruct mSystemInfo;

    wqueue<WorkItem*> & mMySQLJobQueue;
    wqueue<DataItem*> & mMainJobQueue;
    wqueue<UDPDataItem*> & m_udpqueue;
    wqueue<NotificationItem*> & mNotifiactionQueue;

    DeviceManager * mDeviceManager;
    DBManager * mDbManager;

    std::list<QRCodeStruct*> mQRCodeImageList;
    std::list<SendNotifyStatusStruct*> mSendNotifyStatusList;
    std::list<SendNotifyStatusStruct*> mSendATCmdStatusList;
    std::list<UserAirRemainStruct*> mUserAirRemainList;
    std::list<SoccerRecordingStruct*> * mSoccerRecordingList;

    void initQueue();
    void runloop();
    void waitloop();
    void sendSystemStatusToClient(UDPDataItem * item);
    void sendSingleTagStatusToClient(UDPDataItem * item);
    void sendTagStatusToClient(UDPDataItem * item);
    void sendTagStatusToClient(UDPDataItem * item, int projectid, int groupid, std::string maingroups, int start_index, int count);
    void sendAllTagStatusToClient(UDPDataItem * item, int projectid, int groupid, std::string maingroups);
    void sendSingleTagStatusToClient(UDPDataItem * item, int projectid, int groupid, std::string maingroups, std::string nodeid);
    void sendCheckTagStatusToClient(UDPDataItem * item);
    void sendTagRollCallStatusToClient(UDPDataItem * item);
    void sendTagRollCallStatusToClient(UDPDataItem * item, int projectid, int start_index, int count, std::string areaid);
    void sendAllTagRollCallStatusToClient(UDPDataItem * item, int projectid);
    void sendTagRollCallPdfReportToClient(UDPDataItem * item);
    void sendTagRollCallXlsxReportToClient(UDPDataItem * item);
    void sendUserInfoXlsxReportToClient(UDPDataItem * item);
    void sendUserRollCallResultXlsxReportToClient(UDPDataItem * item);
    std::string create_UserRollCallResultXlsxReport(int projectid, CARDRollCallStruct * rollcallinfo, std::list<UserSubGroupStruct*> * rollcallSubGroupList, CARDRollCallEventStruct * rollcallEvent, std::list<UserStruct*> * rollcallUserList, std::list<USERRollCallRecordStruct*> *userRollCallList);
    void sendUnknowNFCCardsXlsxReportToClient(UDPDataItem * item);
    void sendUnknowUHFCardsXlsxReportToClient(UDPDataItem * item);
    void sendAnchorStatusToClient(UDPDataItem * item);
    void sendAnchorStatusToClient(UDPDataItem * item, int projectid, int groupid, std::string maingroups, int start_index, int count);
    void sendAllAnchorStatusToClient(UDPDataItem * item, int projectid, int groupid, std::string maingroups);
    void sendCoordStatusToClient(UDPDataItem * item);
    void sendCoordStatusToClient(UDPDataItem * item, int projectid, int groupid, std::string maingroups, int start_index, int count);
    void sendAllCoordStatusToClient(UDPDataItem * item, int projectid, int groupid, std::string maingroups);
    void sendLocatorStatusToClient(UDPDataItem * item);
    void sendLocatorStatusToClient(UDPDataItem * item, int projectid, int groupid, std::string maingroups, int start_index, int count);
    void sendAllLocatorStatusToClient(UDPDataItem * item, int projectid, int groupid, std::string maingroups);
    void sendSingleLocatorStatusToClient(UDPDataItem * item, int projectid, int groupid, std::string maingroups, std::string nodeid);
    void sendAoAStatusToClient(UDPDataItem * item);
    void sendAoAStatusToClient(UDPDataItem * item, int projectid, int groupid, std::string maingroups, int start_index, int count);
    void sendAllAoAStatusToClient(UDPDataItem * item, int projectid, int groupid, std::string maingroups);
    void sendLoRaGatewayStatusToClient(UDPDataItem * item);
    void sendNFCDeviceStatusToClient(UDPDataItem * item);
    void sendNFCDeviceCountToClient(UDPDataItem * item);
    void sendWIFIDeviceStatusToClient(UDPDataItem * item);
    void sendCELLDeviceStatusToClient(UDPDataItem * item);
    void sendEDGEDeviceStatusToClient(UDPDataItem * item);
    void sendSensorDeviceDataToClient(UDPDataItem * item);
    void sendSensorDeviceStatusToClient(UDPDataItem * item);
    void sendSensorDeviceBaseInfoToClient(UDPDataItem * item);
    void sendNFCCardStatusToClient(UDPDataItem * item);
    void sendRFIDDeviceStatusToClient(UDPDataItem * item);
    void sendRFIDCardStatusToClient(UDPDataItem * item);
    void sendDevicesCountToClient(UDPDataItem * item);
    void sendDeviceStatusToClient(UDPDataItem * item);
    void startDumpRangingResult(char * buf, int length);
    void sendRangingToClient(UDPDataItem * item);
    void sendCardScanResultToClient(UDPDataItem * item);
    void sendAoARangingCloudPointToClient(UDPDataItem * item);
    void sendAoAAngleVectorCloudPointToClient(UDPDataItem * item);
    void sendAoAAngleCloudPointToClient(UDPDataItem * item);
    void setPowerTagCalculateInfo(UDPDataItem * item);
    void updateNodeInfo(const char * node_type, char * buf, int length);
    void updateMapNodeInfo(char * buf, int length);
    void deleteNodeInfo(char * buf, int length);
    void deleteGatewayInfo(char * buf, int length);
    void deleteAllOfflineDevice(char * buf, int length);
    void updateConfig(char * buf, int length);
    void createConfig(UDPDataItem * item);
    void deleteConfig(char * buf, int length);
    void sendSystemConfigToClient(UDPDataItem * item);
    void updateSystemConfigUDPPort(char * buf, int length);
    void updateAlgorithmConfig(char * buf, int length);
    void updateLoRAWANConfig(char * buf, int length);
    void sendTagAlertToClient(UDPDataItem * item);
    void sendRangingDiagnosisToClient(UDPDataItem * item);
    void sendLocatorRSSIInfoToClient(UDPDataItem * item);
    void createSection(char * buf, int length);
    void deleteSection(char * buf, int length);
    void createFenceAlert(char * buf, int length);
    void deleteFenceAlert(char * buf, int length);
    void updateArea(UDPDataItem * item);
    void deleteEvent(char * buf, int length);
    void createEvent(char * buf, int length);
    void updateMapLayer(char * buf, int length);
    void updateMapLayerArea(char * buf, int length);
    void updateMapLayerAreaShape(char * buf, int length);
    void deleteMapLayerArea(char * buf, int length);

    void createInspectionGroup(char * buf, int length);
    void updateInspectionGroup(char * buf, int length);
    void deleteInspectionGroup(char * buf, int length);
    void createInspectionPoint(char * buf, int length);
    void updateInspectionPoint(char * buf, int length);
    void deleteInspectionPoint(char * buf, int length);
    void SendInspectionPDFReportToClient(UDPDataItem * item);
    void SendBodyInfoPDFReportToClient(UDPDataItem * item);
    void SendUserBodyInfoPDFReportToClient(UDPDataItem * item);
    void SendCoursePDFReportToClient(UDPDataItem * item);


    // Course
    void createCourse(char * buf, int length);
    void deleteCourse(char * buf, int length);

    void sendPOSNodeToClient(UDPDataItem * item);
    void sendGPSNodeToClient(UDPDataItem * item);

    void sendDeviceAliveToClient(UDPDataItem * item);
    void sendSystemServiceToClient(UDPDataItem * item);
    void sendQRCodeToClient(UDPDataItem * item);
    void sendPDFReportToClient(UDPDataItem * item);
    void sendTagAreaInOutPDFReportToClient(UDPDataItem * item);
    void sendTagAreaInOutXLSXReportToClient(UDPDataItem * item);
    void sendEventPDFReportToClient(UDPDataItem * item);
    void sendTagDailyPDFReportToClient(UDPDataItem * item);
    PDFTableDataStruct * prepareUserRoomInOutReport(int projectid, std::list<AreaInOutEventStruct*> * nodeAreaInOutList, std::string nodeid, std::string AreaID, std::string start_datetime, std::string end_datetime);
    PDFTableDataStruct * prepareTagDailyAreaInOutReport(int projectid, std::list<AreaInOutEventStruct*> * nodeAreaInOutList, std::string nodeid, std::string start_datetime, std::string end_datetime);
    PDFTableDataStruct * prepareTagDailyAlertEventReport(int projectid, std::list<FiredEventStruct*> * firedEventList, std::string nodeid, std::string start_datetime, std::string end_datetime);
    PDFTableDataStruct * prepareTagDailyEventReport(int projectid, std::list<FiredEventStruct*> * firedEventList, std::string nodeid, std::string start_datetime, std::string end_datetime);
    std::list<PDFTableDataStruct*> * prepareTagDailyCourseReport(int projectid, std::list<AreaInOutEventStruct*> * nodeAreaInOutList, std::string nodeid, std::string start_datetime, std::string end_datetime);
    PDFTableDataStruct * prepareTagCourseReport(int projectid, std::list<AreaInOutEventStruct*> * nodeAreaInOutList, CourseStruct * course, std::string nodeid, std::string start_datetime, std::string end_datetime);
    std::list<FiredEventStruct*> * getAlertCancelActionEventList(std::list<FiredEventStruct*> * eventlist, std::string nodeid);
    std::list<Pair_FiredEventStruct*> * getPairEvents(std::list<FiredEventStruct*> * eventlist);

    // dashboard adata
    void sendDashboardActivityToClient(UDPDataItem * item);
    void sendDashboardSystemInfoToClient(UDPDataItem * item);

    void sendTagBodyInfoToClient(UDPDataItem * item);
    void sendTagBodyInfoRecordsToClient(UDPDataItem * item);
    void sendLocatorBodyInfoToClient(UDPDataItem * item);
    void sendTagPrjInfoToClient(UDPDataItem * item);
    void sendAnchorPrjInfoToClient(UDPDataItem * item);
    void sendCoordPrjInfoToClient(UDPDataItem * item);
    void sendLocatorPrjInfoToClient(UDPDataItem * item);
    void sendGatewayPrjInfoToClient(UDPDataItem * item);
    void updatenodeprj(char * buf, int length);
    void updatenodeuser(char * buf, int length);
    void updateaoanormalvector(char * buf, int length);

    void resetm4(UDPDataItem * item);
    void resetm0(UDPDataItem * item);
    void blacklist(char * buf, int length);
    void whitelist(char * buf, int length);
    void clearblacklist(char * buf, int length);
    void clearwhitelist(char * buf, int length);
    void joinRangingRange(char * buf, int length);
    void joinNotRangingRange(char * buf, int length);
    void nodeResident(char * buf, int length);
    void nodeEmployee(char * buf, int length);
    void locatorButtontype(char * buf, int length);
    void locatorSensorAlert(char * buf, int length);
    void checkaccountlogin(UDPDataItem * item);
    void checkaccountloginkey(UDPDataItem * item);
    void queryaccountloginkey(UDPDataItem * item);
    void syncERPaccount(UDPDataItem * item);
    void updatePushNotify(char * buf, int length);
    void updatePushNotifyTriggerMessage(char * buf, int length);

    void sendmultinotification(UDPDataItem * item);
    void waitpushnotify(UDPDataItem * item);
    void sendatcmd(UDPDataItem * item);
    void waitatcmd(UDPDataItem * item);
    void printsystemlog(char * buf, int length);
    void sendABBoxToClient(UDPDataItem * item);
    void savebodyinfoweight(char * buf, int length);
    void savebodyinfoheartbeat(char * buf, int length);
    void savebodyinfoall(char * buf, int length);
    void sendForacaredataClient(UDPDataItem * item);
    void sendUnknownPositionTagToClient(UDPDataItem * item);
    void uploadxlsxfile(UDPDataItem * item);
    std::string check_xlsx_user(int projectid, std::string accountid, std::string userid, std::list<XLSXUserStruct*> * xlsxUserList, bool enableShowMainGroup);
    void check_xlsx_usersubgroup(int projectid, std::string accountid, std::string userid, std::list<std::string> * userDataGroups, std::string datagroups, std::list<UserSubGroupStruct*> * userSubGroupList, std::string maingroups);
    void check_xlsx_usermaingroup(int projectid, std::string accountid, std::string userid, std::list<std::string> * userMainGroups, std::string maingroups, std::list<UserMainGroupStruct*> * userMainGroupList);

    void newleaveform(char * buf, int length);
    std::list<std::string> * getSubgGroupManager(int projectid, UserStruct * user);

    void leaveformsignoff(char * buf, int length);
    void newassistevent(char * buf, int length);
    void update_nodemaingroup(char * buf, int length);
    void sensor_info(char * buf, int length);
#ifdef ENABLE_SEND_BODYINFO_ALERT
    void check_bodyinfo_alert(int projectid, SensorDeviceStruct * sensorExisted, UserStruct * userLink, int HR, int SPO2);
#endif
    void send_sensor_user_Alert(int projectid, SensorDeviceStruct * sensorExisted, UserStruct * userLink, bool isAlert, std::string Latitude, std::string Longitude, double distanceOffset);
    void api_pushnotify(char * buf, int length);
    void updatecurriculumrecord(char * buf, int length);
    void deletecurriculumrecord(char * buf, int length);
    void applycurriculumrecord(char * buf, int length);
    void updatecurriculumapplystatus(char * buf, int length);

    void sendAirRemainToClient(UDPDataItem * item);
    void resetAirRemain(char * buf, int length);
    void findlocation(UDPDataItem * item, std::string payload);
    void createFireFighting(UDPDataItem * item, std::string payload);

    UserStruct * update_sensor_user_location(int projectid, std::string deviceid, std::string Longitude, std::string Latitude);
    UserStruct * update_user_location(int projectid, int userid, std::string Longitude, std::string Latitude);

    void setFixPosition(char * buf, int length);
    void setFixDevice(char * buf, int length);
    void create_new_user(char * buf, int length);
    void update_user(char * buf, int length);
    void delete_user(char * buf, int length);
    void reset_user_topic(UDPDataItem * item);
    void reset_user_password(UDPDataItem * item);
    void update_rollcall_record(char * buf, int length);
    void update_rollcall_records(char * buf, int length);

    void soccerStartRecord(std::string payload);
    void soccerStopRecord(std::string payload);
    void SendSoccerStatusToClient(UDPDataItem * item, std::string payload);

    void dump_stream_nodelist_info(int projectid, std::list<NodeStruct*> * nodeList, std::ostringstream & ostream, std::string prefix);
    void dump_stream_nodelist_info(int projectid, std::list<NodeStruct*> * nodeList, std::ostringstream & ostream, std::string prefix, int start_index, int count);
    void dump_stream_node_info(int projectid, NodeStruct * node, std::ostringstream & ostream, std::string prefix);
    void dump_stream_aoalist_info(int projectid, std::list<AoANodeStruct*> * nodeList, std::ostringstream & ostream, std::string prefix);
    void dump_stream_aoalist_info(int projectid, std::list<AoANodeStruct*> * nodeList, std::ostringstream & ostream, std::string prefix, int start_index, int count);
    void dump_stream_aoa_info(int projectid, AoANodeStruct * node, std::ostringstream & ostream, std::string prefix);
    void dump_anchor_node_info(NodeStruct * node, std::ostringstream & ostream);
    void dump_tag_node_info(int projectid, NodeStruct * node, std::ostringstream & ostream);
    void dump_coord_node_info(NodeStruct * node, std::ostringstream & ostream);
    void dump_locator_node_info(int projectid, NodeStruct * node, std::ostringstream & ostream);
    void dump_stream_tagrollcall_info(std::list<NodeStruct*> * nodeList, std::ostringstream & ostream, std::string prefix);
    void dump_stream_tagrollcall_info(std::list<NodeStruct*> * nodeList, std::ostringstream & ostream, std::string prefix, int start_index, int count, std::string areaid);
    void dump_stream_rollcall_info(NodeStruct * node, std::ostringstream & ostream, std::string prefix, std::string areaid);

    int count_node_alive(std::list<NodeStruct*> * nodeList);
    int count_gateway_alive(std::list<LoRaGatewayInfoStruct*> * nodeList);
    int getProjectId(Json::Value result);
    int getIntResult(Json::Value result, const char * key);
    int test_create_QRCODE();
    std::string getUseRoommAreaID(int projectid, NodeStruct * node);
    std::list<UserStruct*> * getSubGroupUser(std::string accountid, std::list<UserStruct*> * alluserList);
    std::list<UserSubGroupStruct*> * getViewAccessSubGroup(std::string accountid, std::list<UserStruct*> * alluserList, std::list<UserSubGroupStruct*> * allSubGroupList);
    std::list<USERRollCallRecordStruct*> * getUserRollCallList(std::list<CARDRollCallRecordStruct*> * rollcallRecordList);

    std::list<GeoCodeStruct*> * getAddressGeoCode(std::string address);

    SoccerRecordingStruct * getCurrentSoccerRecording(int maingroupid);
    void prepareSoccerRecording();
};


#endif
