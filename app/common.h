
#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <list>
#include <vector>

#include "mysql/MySQLDBAccess.h"
#include "mysql/ConfigDBAccess.h"
#include "mysql/TrackDBAccess.h"
#include "mysql/TimeSerialsDBAccess.h"
#include "PosCalculate/AnchorTagCoupleDataClass.h"
#include "DeviceManager/AreaInOutDataClass.h"
#include "DeviceManager/TagInOutDataClass.h"
#include "Inspection/InspectionAnchorRangingDataClass.h"
#include "Courses/CourseAnchorRangingDataClass.h"
#include "network/MulticastClient.h"
#include "json/json.h"
#include "json/JSONAgent.h"

#include "config.h"
#include "logger/log.h"

#include "QRcode/qrcode.h"
#include "bmp/qdbmp.h"

#define STATE_CAL_ANCHOR_COORD 	0
#define STATE_CAL_TAG_COORD 	1
#define STATE_GET_ANCHOR_LIST	2
#define STATE_GET_TAG_LIST	3

#define NODE_TYPE_ANCHOR    0
#define NODE_TYPE_TAG       1
#define NODE_TYPE_COORD     2
#define NODE_TYPE_LOCATOR   3
#define NODE_TYPE_GATEWAY   4
#define NODE_TYPE_AOA       5
#define NODE_TYPE_SENSOR    6
#define NODE_TYPE_USER      2222
#define NODE_TYPE_CARD      500
#define NODE_TYPE_NFC_LOCATOR  100
#define NODE_TYPE_UHF_READER   101

#define OPERTAION_MODE_NONE    0
#define OPERTAION_MODE_UWB     1
#define OPERTAION_MODE_LORA    2
#define OPERTAION_MODE_GPS     3

typedef unsigned long uint32;

typedef struct
{
    int TagCount;
    std::string areaid;
    PointStruct center;
} AreaCenterStruct;

typedef struct
{
    int AnchorIndex;
    int FailCount;
} AnchorRangingCountStruct;

typedef struct
{
    std::string interface;
    std::string ip;
    std::string broadcastAddr;
} NetworkInfoStruct;

typedef struct
{
    std::string parameter;
    std::string value;
} HttpHeader;

typedef struct
{
    std::string filepath;
    time_t createtime;
} QRCodeStruct;

typedef struct
{
    int projectid;
    int status;// 0:wait finish 1:success 2:failed!
    std::string waitkey;
    std::string message;
    std::string macaddress;
    time_t createtime;
} SendNotifyStatusStruct;

typedef struct
{
    int distInterval;
    int baseDist;
    std::string macaddress;
    std::list<int> distList;
} LoRaCompressMultipleDistanceStruct;

typedef struct
{
    int projectid;
    std::string NodeId;
    std::string macaddress;
    std::string message;
    int function;
    time_t issuetime;
} FunctionSendTimeStruct;

typedef struct
{
    unsigned long long TotalDiskSize;// in MB
    unsigned long long TotalUsedSize;// in MB
    unsigned long long TotalFreeSize;// in MB
    unsigned long long DBUsedSize;      // MB

    int SystemRunHour;
    int SystemRunMin;
    int SystemRunSec;
    int CPURunHour;
    int CPURunMin;
    int CPURunSec;
    int MemoryUsage;

    time_t updateTimestamp;
} SystemInfoStruct;

#define MULTI_PUSHNOTIFY_FUNCTION_1 1111  // 手動群發
#define MULTI_PUSHNOTIFY_FUNCTION_2222 2222  // 請假相關申請
#define MULTI_PUSHNOTIFY_FUNCTION_3333 3333  // 選課相關申請
#define MULTI_PUSHNOTIFY_FUNCTION_201 201  // 手動群發 -> 聯絡簿
#define MULTI_PUSHNOTIFY_FUNCTION_202 202  // 手動群發 -> 師生交流
#define MULTI_PUSHNOTIFY_FUNCTION_203 203  // 手動群發 -> 電子投票
#define MULTI_PUSHNOTIFY_FUNCTION_204 204  // 手動群發 -> 工作指示
#define MULTI_PUSHNOTIFY_FUNCTION_205 205  // 手動群發 -> 不分類
#define MULTI_PUSHNOTIFY_FUNCTION_206 206  // 手動群發 -> 協助事項回報
#define MULTI_PUSHNOTIFY_FUNCTION_207 207  // 手動群發 -> 公布欄

typedef enum
{
    TAG_PUSHNOTIFY_FUNCTION_1 = 1, // 非設備文字訊息, 如醫囑等, 手動發送
    TAG_PUSHNOTIFY_FUNCTION_2,     // 緊急求助與取消通知
    TAG_PUSHNOTIFY_FUNCTION_3,     // 低電量通知
    TAG_PUSHNOTIFY_FUNCTION_4,     // 進入(離開)危險區通知, 身份為工作人員時不通知
    TAG_PUSHNOTIFY_FUNCTION_5,     // 巡檢相關通知, 身份為巡檢員時才有通知
    TAG_PUSHNOTIFY_FUNCTION_6,     // 未正常配戴通知
    TAG_PUSHNOTIFY_FUNCTION_7,     // 進出區域事件, 需要在事件列表設定才有通知
    TAG_PUSHNOTIFY_FUNCTION_8,     // 課程點名通知, 包含上課中進出通知
    TAG_PUSHNOTIFY_FUNCTION_9,       // GPS活動超過區域範圍通知
    TAG_PUSHNOTIFY_FUNCTION_10,       // 不特定對象推播
    TAG_PUSHNOTIFY_FUNCTION_11,       // 周邊模組訊息，如不明物體移動、煙霧探測
    TAG_PUSHNOTIFY_FUNCTION_12       // 人員點名
} TAG_PUSHNOTIFY_FUNCTION_TYPE;

typedef enum
{
    LOCATOR_PUSHNOTIFY_FUNCTION_1 = 101, // 非設備文字訊息, 如醫囑等, 手動發送
    LOCATOR_PUSHNOTIFY_FUNCTION_2,     // 緊急求助與取消通知
    LOCATOR_PUSHNOTIFY_FUNCTION_3,     // 呼喚與取消通知
    LOCATOR_PUSHNOTIFY_FUNCTION_4,     // 醫生進出病房與病床區域通知
    LOCATOR_PUSHNOTIFY_FUNCTION_5,     // 斷電警報
    LOCATOR_PUSHNOTIFY_FUNCTION_6     // 周邊模組訊息，如不明物體移動、煙霧探測
    // LOCATOR_PUSHNOTIFY_FUNCTION_4     // 不特定對象推播, 巡檢或病房相關通知
} LOCATOR_PUSHNOTIFY_FUNCTION_TYPE;

typedef enum
{
    CARD_PUSHNOTIFY_FUNCTION_1 = 500 // 卡片點名
} CARD_PUSHNOTIFY_FUNCTION_TYPE;

typedef enum
{
    USER_PUSHNOTIFY_FUNCTION_1 = 800 // 人員緊急求助
} USER_PUSHNOTIFY_FUNCTION_TYPE;

typedef struct
{
    std::string TargetTime;
    float NormDist;
    int Triggered;
    int ReportMean;
} CARDRollCallTriggerItemStruct;

typedef struct
{
    std::string TargetRollCallName;
    int TotalMean;
    int ReportTotalMean;
    std::list<CARDRollCallTriggerItemStruct*> * triggeritems;
} CARDRollCallTriggerStruct;

NodeStruct * createNodeStruct();
NodeStruct * createBaseNodeStruct(int index, int nodetype,
        const char * nodeid, const char *macaddress,
        double posX, double posY, double posZ, const char * version,
        int tafid, int buildingid, int buildingfloor);
NodeStruct * copyNodeStruct(NodeStruct * srcNode);
void copyUserStruct(UserStruct * srcUser, UserStruct * dstUser);
AnalyzeDayInfoStruct * createAnalyzeDayInfoStruct();

VILSConfig * createVILSConfig();

std::string currentDateTimeForSQL();
std::string getCurrentDateTimeInMilliSec();
std::string currentDateTimeISO8601();
std::string currentDateTimeForReport();
std::string currentDateTimeForESP32();
std::string BeforeMinDateTimeForSQL(int beforeInMin);
std::string currentDateTimeForSQL(int beforeInDay);
std::string currentDateTimeForPushNotification();
std::string currentDateTimeForPushNotification(int beforeInDay);
std::string createDateTimeForSQL(struct timeval tv);
std::string currentTimeForSQL();
std::string currentTimeWithSecForSQL();
std::string currentTimeForSQL(int beforeInMin);
std::string currentSimpleTime();
std::string convertSimpleTime(time_t curtime);
std::string currentDateTime();
std::string TodayDate();
std::string TodayDateType1();
std::string TodayDateType1(int beforeInDay);
std::string TodayYear(int beforeInDay);
std::string TodayMonth(int beforeInDay);
std::string TodayDay(int beforeInDay);

int TodayDateWeekday();
time_t getCurrentTimeInSec();
time_t getCurrentTimeInMilliSec();
time_t StringToCheckPointDatetime(string str);
time_t StringToInspectionDatetime(string str);
std::string time_tToString(time_t t);
std::string time_tToTimeString(time_t t);
std::string time_tToTimerString(time_t t);
std::string time_tToHumanReadString(time_t t);
std::string convertToShowDate(std::string str_time);
std::string StringDateTimeToStringDate(std::string str_time);
time_t StringDateTimeTotime_t(std::string str_time);
time_t StringSimpleTimeTotime_t(std::string str_time);
time_t StringUTCDateTimeTotime_t(const char * str_time);
time_t StringTimeZoneDateTimeTotime_t(const char * str_time);
time_t StringDateTotime_t(std::string str_time, int addDays);
time_t StringTimeTotime_t(std::string str_time, int addMinutes);
void clear_ranging_vector(std::vector<AnchorTagCoupleDataClass*> * plist);
void clear_AreaInOut_vector(std::vector<AreaInOutDataClass*> * plist);
void clear_TagInOut_vector(std::vector<TagInOutDataClass*> * plist);
void clear_SectionDeviceInfo_vector(std::vector<SectionDeviceInfo*> * plist);
void clear_SectionGroup_list(std::list<SectionGroup*> * plist);
void clear_node_list(std::list<NodeStruct*> * plist);
void clear_LoRaGateway_list(std::list<LoRaGatewayInfoStruct*> * plist);
void clear_FootprintAll_list(std::list<FootprintAllStruct*> * plist);
void clear_Config_list(std::list<VILSConfig*> * plist);

void clear_CheckPointGroup_list(std::list<CheckPointGroupStruct*> * plist);
void clear_CheckPointPath_list(std::list<CheckPointPathStruct*> * plist);
void clear_CheckPointGroupDailyRecord_list(std::list<CheckPointGroupDailyRecord*> * pList);
void clear_CheckPointPathDailyRecord_list(std::list<CheckPointPathDailyRecord*> * pList);
void clear_CheckPointTagBehaviorData_list(std::list<CheckPointTagBehaviorData*> * pList);

void clear_InspectionGroup_list(std::list<InspectionGroupStruct*> * plist);
void clear_InspectionPoint_list(std::list<InspectionPointStruct*> * plist);

// void clear_AnchorRanging_vector(std::vector<CheckPointAnchorRangingDataClass*> * plist);
void clear_InspectionAnchorRanging_vector(std::vector<InspectionAnchorRangingDataClass*> * plist);
void clear_AnchorRangingDataClass_vector(std::vector<AnchorRangingDataClass*> * plist);
void clear_FiredEventStruct_list(std::list<FiredEventStruct*> * plist);
void clear_Pair_FiredEventStruct_list(std::list<Pair_FiredEventStruct*> * plist);
void clear_Pair_FiredEventStruct(Pair_FiredEventStruct * event);
void clear_Course_list(std::list<CourseStruct*> * plist);
void clear_NodeGPSInfo_list(std::list<NodeGPSInfoStruct*> * plist);
void clear_NodePositionInfo_list(std::list<NodePositionInfoStruct*> * plist);
void clear_NodeItemStruct_list(std::list<NodeItemStruct*> * plist);
void clear_CourseRollCallRecordStruct_list(std::list<CourseRollCallRecordStruct*> * plist);
void clear_AnalyzeDayInfoStruct_list(std::list<AnalyzeDayInfoStruct*> * plist);
void clear_HttpHeader_list(std::list<HttpHeader*> * plist);
void clear_DayRecordStruct_list(std::list<DayRecordStruct*> * plist);
void clear_DeviceAliveCountStruct_list(std::list<DeviceAliveCountStruct*> * plist);
void clear_AreaActivityTimeStruct_list(std::list<AreaActivityTimeStruct*> * plist);
void clear_EmployeeEventReportStruct_list(std::list<EmployeeEventReportStruct*> * plist);
void clear_UserStruct(UserStruct * user);
void clear_UserStruct_list(std::list<UserStruct*> * plist);
void clear_XLSXUserStruct_list(std::list<XLSXUserStruct*> * plist);
void clear_UserGroupStruct_list(std::list<UserGroupStruct*> * plist);
void clear_UserSubGroupStruct(UserSubGroupStruct * item);
void clear_UserSubGroupStruct_list(std::list<UserSubGroupStruct*> * plist);
void clear_UserMainGroupStruct(UserMainGroupStruct * item);
void clear_UserMainGroupStruct_list(std::list<UserMainGroupStruct*> * plist);
void clear_LoRaCompressMultipleDistanceStruct_list(std::list<LoRaCompressMultipleDistanceStruct*> * plist);
// void clear_PushNotifyUserStruct_list(std::list<PushNotifyUserStruct*> * plist);
void clear_NodeMoveOffsetInfoStruct_list(std::list<NodeMoveOffsetInfoStruct*> * plist);
void clear_UserResidentStruct_list(std::list<UserResidentStruct*> * plist);
void clear_UserEmployeeStruct_list(std::list<UserEmployeeStruct*> * plist);
void clear_FunctionSendTimeStruct_list(std::list<FunctionSendTimeStruct*> * plist);
void clear_ProjectConfig_list(std::list<ProjectConfig*> * plist);
void clear_AreaInOutEventStruct_list(std::list<AreaInOutEventStruct*> * plist);
void clear_node_AreaInOutEventStruct_list(std::list<node_AreaInOutEventStruct*> * plist);
void clear_Pair_AreaInOutEventStruct_list(std::list<Pair_AreaInOutEventStruct*> * plist);
void clear_Pair_AreaInOutEventStruct(Pair_AreaInOutEventStruct * event);
void clear_OperationModeActivityStruct_list(std::list<OperationModeActivityStruct*> * plist);
void clear_PushNotifySettingStruct_list(std::list<PushNotifySettingStruct*> * plist);
void clear_TriggerPushMessageStruct_list(std::list<TriggerPushMessageStruct*> * plist);
void clear_MapLayerStruct_list(std::list<MapLayerStruct*> * plist);
void clear_AoANodeStruct_list(std::list<AoANodeStruct*> * plist);
void clear_PointStruct_list(std::list<PointStruct*> * plist);
void clear_BodyInfoStruct_list(std::list<BodyInfoStruct*> * plist);
void clear_DeviceActivityAliveCountStruct_list(std::list<DeviceActivityAliveCountStruct*> * plist);
void clear_AreaRollCallRecordStruct_list(std::list<AreaRollCallRecordStruct*> * plist);
void clear_OperationModeRecordStruct_list(std::list<OperationModeRecordStruct*> * plist);
void clear_TagMoveOffsetRecordStruct_list(std::list<TagMoveOffsetRecordStruct*> * plist);
void clear_DeviceActivityRecordStruct_list(std::list<DeviceActivityRecordStruct*> * plist);
void clear_PDFTableDataStruct(PDFTableDataStruct * pdfData);
void clear_CourseDayTrackRecordStruct_list(std::list<CourseDayTrackRecordStruct*> * plist);
void clear_PDFTableDataStruct_list(std::list<PDFTableDataStruct*> * plist);
void clear_SurveyStruct_list(std::list<SurveyStruct*> * plist);
void clear_SurveyItemStruct_list(std::list<SurveyItemStruct*> * plist);
void clear_PushNotifyMessageRecordStruct_list(std::list<PushNotifyMessageRecordStruct*> * plist);
void clear_SurveyEventRecordStruct_list(std::list<SurveyEventRecordStruct*> * plist);
void clear_RFIDCardStruct_list(std::list<RFIDCardStruct*> * plist);
void clear_RFIDDeviceStruct_list(std::list<RFIDDeviceStruct*> * plist);
void clear_NFCCardStruct_list(std::list<NFCCardStruct*> * plist);
void clear_NFCDeviceStruct_list(std::list<NFCDeviceStruct*> * plist);
void clear_EDGEDeviceStruct_list(std::list<EDGEDeviceStruct*> * plist);
void clear_CARDRollCallRecordStruct_list(std::list<CARDRollCallRecordStruct*> * plist);
void clear_USERRollCallRecordStruct_list(std::list<USERRollCallRecordStruct*> * plist);
void clear_CARDScanRecordStruct_list(std::list<CARDScanRecordStruct*> * plist);
void clear_TargetTopicStruct_list(std::list<TargetTopicStruct*> * plist);
void clear_LocatorRSSIInfoStruct_list(std::list<LocatorRSSIInfoStruct*> * plist);
void clear_GatewayRSSIInfoStruct_list(std::list<GatewayRSSIInfoStruct*> * plist);
void clear_RSSIInfoStruct_list(std::list<RSSIInfoStruct*> * plist);
void clear_GAESurveyResultData_list(std::list<GAESurveyResultData*> * plist);
void clear_CARDRollCallTriggerItemStruct_list(std::list<CARDRollCallTriggerItemStruct*> * plist);
void clear_CARDRollCallTriggerStruct_list(std::list<CARDRollCallTriggerStruct*> * plist);
void clear_CARDRollCallSubGroupEventStruct_list(std::list<CARDRollCallSubGroupEventStruct*> * plist);
void clear_FireFightingUserAirInfoStruct_list(std::list<FireFightingUserAirInfoStruct*> * plist);

char * convertToSimpleAddress(char * inString);
long long getSystemTime();
// bool read_sys_config(VILSSystemConfig * config);
// bool write_sys_config(VILSSystemConfig * config);
// bool read_vils_config(int project_id, VILSConfig * config);

int copyFile(const char * source_file, const char * target_file);
double calcVelocity(int * oldIMU, int * newIMU);
//void mac_eth0(char * MAC_str);
int getMyCenterIP(char * ip);
int getMyCenterNetworkInfo(std::list<NetworkInfoStruct * > & CenterNetworkInfoList);
int getMyIP(char * ip);
int getIPfromName(const char * hostname, char * ip);
void clear_area_list(std::list<AreaStruct*> * areaList);
void clear_maplayerarea_list(std::list<MapLayerAreaStruct*> * areaList);
void clear_areacenter_list(std::list<AreaCenterStruct*> * areacenterList);
void clear_PolygonStruct_list(std::list<PolygonStruct*> * polyList);
void clear_event_list(std::list<EventStruct*> * eventList);
void clear_Footprint_list(std::list<FootprintStruct*> * footprintList);
void clear_NetworkInfo_list(std::list<NetworkInfoStruct*> * NetworkInfoList);
void clear_MulticastClient_list(std::list<MulticastClient*> * MulticastClientList);
double centimeterToLatitude(int distenceincm);
double centimeterToLongitude(double latitude, int distenceincm);
double LatitudeToCentimeter(double latitude);
double LongitudeToCentimeter(double latitude, double longitude);
double centimeterTolatLng(int distenceincm);
double latLngTocentimeter(double distenceinlatLng);
double calculateDistance(double lat1, double long1, double lat2, double long2);
double toRad(double degree);
std::string convertToStringlist(int anchor_count, int * anchors);
std::string convertListToString(std::list<std::string> * stringlist);
std::string convertListToString(std::list<std::string> * stringlist, std::string sp);
std::string convertVectorToString(std::vector<std::string> * stringlist);
std::string convertIntListToString(std::list<int> * intlist);
void printout(std::list<SectionGroup *> * sectiongrouplist);
uint32 SockAddrToUint32(struct sockaddr * a);
void Inet_NtoA(uint32 addr, char * ipbuf);
uint32 Inet_AtoN(const char * buf);
void PrintNetworkInterfaceInfos();
//std::string convert_to_json_str(int projectid, int eventid, std::string nodeid, std::string action, std::string areaid, std::string issuetime);
std::string convert_to_json_str(int projectid, int eventid, std::string nodeid, std::string action, std::string areaid, std::string issuetime, std::string locatorid);
std::string convert_to_json_str(int projectid, int eventid, std::string nodeid, std::string action, std::string areaid, std::string issuetime, std::string locatorid, std::string lat, std::string lng);
std::string convert_FireFightingStruct_to_json_str(FireFightingStruct * fireFighting);
std::string convert_FireFightingUserStruct_to_json_str(FireFightingUserStruct * fireFightingUser);
std::list<AnchorRangingCountStruct *> * parse_anchor_ranging_table(const char * AnchorRangingTable);
void clear_AnchorRangingCount_list(std::list<AnchorRangingCountStruct*> * plist);
void replaceAll(std::string& str, const std::string& from, const std::string& to);
void printf_VILSSystemConfig(VILSSystemConfig * systemconfig);
void initialCheckPointGroupDailyRecord(CheckPointGroupDailyRecord & data);
void initialCheckPointPathDailyRecord(CheckPointPathDailyRecord & data);
void initialInspectionGroupDailyRecord(InspectionGroupDailyRecord & data);
void initialInspectionPointDailyRecord(InspectionPointDailyRecord & data);
double convertLatLongToDeg(std::string llstr, std::string dir);
double parseDouble(std::string s);
float parseFloat(std::string s);
int64_t parseInt(std::string s, int radix);
int convert_string_to_int(std::string inputMsg);
bool isZero(double value, double threshold);
bool parse_payload_JSON(const char * buf, Json::Value & result);
bool parse_JSON(std::string json_string, Json::Value & result);
std::string get_action_name(std::string action);
std::string get_time_name(std::string issuetime);
std::list<PolygonStruct*> * getPolygonPoints(string geojson);
void process_mem_usage(double& vm_usage, double& resident_set);
long getMemoryUsage();
// long long CurrentSystemRAMInfo();
// long long CurrentRAMUsed();
std::string filterValidNumberString(const char * inString, int length);
void test_Locator_compress();
void test_Locator_compress_1();
int convertHexIMUtoInt(const char * imustr, int * imu);
void DatePlusDays( struct tm* date, int days );
int enc_unicode_to_utf8_one(unsigned long unic, unsigned char *pOutput, int outSize);
int enc_utf8_to_unicode_one(const unsigned char* pInput, unsigned long *Unic);
int enc_get_utf8_size(const unsigned char pInput);
int create_qrcode(const char * message, const char * outputfile, int pixel_size);
std::string create_MD5_encrypt(std::string inkey, std::string password);
std::string create_MD5_encrypt_salt(std::string inkey, std::string password);
std::string random_string( size_t length );
FiredEventStruct * copyFiredEventStruct(FiredEventStruct * srcEvent);
AreaInOutEventStruct * copyAreaInOutEventStruct(AreaInOutEventStruct * srcEvent);
bool parseJSON(const char * inputJSON, Json::Value & JSONresult);
std::vector<std::string> convert_string_to_vector(std::string srcStr);
std::vector<std::string> convert_string_to_vector(std::string srcStr, char pattern);
std::list<std::string> * convert_string_to_list(std::string srcStr, char pattern);
std::list<int> * convert_string_to_intlist(std::string srcStr, char pattern);
std::string convert_list_to_string(std::list<std::string> * plist);
std::string convert_listint_to_string(std::list<int> * plist);
std::string StringToHEXString(std::string inputString);
std::string HEXStringToString(const char * dataASCII);
int HEXStringToBuffer(const char * dataASCII, int dataLength, char * dataBuffer);
std::string remove_newline(std::string inputMsg);
std::string remove_nonASCII(const char * inputString);
std::string remove_nonJSON(const char * inputString);
std::string remove_EOF(const char * inputString, int msgLen);

std::list<std::string> * copyStringList(std::list<std::string> * pList);

#if defined(__APPLE__)
void freeAsMuchAsPossible();
void heapInUse( size_t * bytesInUse, size_t * blocksInUse, size_t * sizeAllocated);
void heapStats();
#endif

std::string UriEncode(const unsigned char * pSrc);
std::string UriDecode(const unsigned char * pSrc);
long getFreeDiskSpace(const char* absoluteFilePath, SystemInfoStruct & systemInfo);
int getSystemRunTime(SystemInfoStruct & systemInfo, int systemStartTime);

std::list<AreaInOutEventStruct*> * merge_inout_event(std::list<AreaInOutEventStruct*> * nodeAreaInOutList);
std::list<Pair_AreaInOutEventStruct*> * pair_inout_event(std::list<AreaInOutEventStruct*> * nodeAreaInOutList);
std::string getAreaTypeName(int areatype);
PDFTableHeaderDataStruct * create_pdf_headerdata(std::string header, int showwidth);
PDFTableColDataStruct * create_pdf_columndata(std::string header, std::string data);

std::string getEventActionName(std::string action);
std::string getEventIssueTimeName(std::string issuetime);

std::string convert_usergroup_id_to_name(std::list<UserGroupStruct*> * userGroupList, int groupid);
std::string convert_subgroup_id_to_name(std::list<UserSubGroupStruct*> * userSubGroupList, std::list<std::string> * subgroupidList);
std::string convert_maingroup_id_to_name(std::list<UserMainGroupStruct*> * userMainGroupList, std::list<std::string> * maingroupidList);

std::string getUserName(int userid, std::list<UserStruct*> * plist);

bool validateID(const std::string& s);
int fetchArrayValue(Json::Value result, PDFTableDataStruct * pdfDataWeight, const char * datalabel, const char * dataname);

bool checkCardExisted(std::string cardid, std::list<std::string> * cardlist);

void copyFireFightingStruct(FireFightingStruct * srcInfo, FireFightingStruct * dstInfo);
void copyFireFightingUserStruct(FireFightingUserStruct * srcInfo, FireFightingUserStruct * dstInfo);
void copyFireFightingAirUsingStruct(FireFightingAirUsingStruct * srcInfo, FireFightingAirUsingStruct * dstInfo);
int rssi_to_distance(int rssi);
#endif // COMMON_H
