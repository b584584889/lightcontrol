/*
 * struct.h
 *
 *  Created on: 2019/04/08
 *      Author: ikki
 */

#ifndef STRUCT_H
#define STRUCT_H

#include "../network/wqueue.h"
#include "../config.h"
#include "../json/json_struct.h"

typedef struct
{
    double x;
    double y;
    double z;
} PointStruct;

typedef struct
{
    std::list<PointStruct*> * coordinates;
} PolygonStruct;

typedef struct
{
    int projectid;
    int layerid;
    int maingroupid; // link to Table UserMainGroup.id
    std::string layer_name;
    std::string layer_description;
    std::string WGS48OriginX;
    std::string WGS48OriginY;
    std::string MAPNorth;
    int BuildingID;
    int BuildingFloor;
    std::string POSX;
    std::string POSY;
    std::string SCALE;
    std::string COEFF;
    std::string FILE;
    std::string MAPWIDTH;
    std::string MAPHEIGHT;
} MapLayerStruct;

typedef struct
{
    int layerid;
    std::string areaid;
    std::string areaname;
    std::string datetime;
    std::string geojson;
    std::list<PolygonStruct*> * Polygons;
    int type;// 0:普通 1:電子圍欄 2:電子點名 3:房間 4:教室 5:病房 6:病床 999:虛擬牆

    double WGS48_Origin_X; // not save on db
    double WGS48_Origin_Y; // not save on db

} MapLayerAreaStruct;

//=====================================================
// For support old project
typedef struct
{
    int projectid;
    std::string areaid;
    std::string areaname;
    std::string datetime;
    std::string geojson;
    std::list<PolygonStruct*> * Polygons;
    int type;// 0:普通 1:電子圍欄 2:電子點名 3:房間 4:教室 5:病房 6:病床 999:虛擬牆

    double WGS48_Origin_X; // not save on db
    double WGS48_Origin_Y; // not save on db

} AreaStruct;

typedef struct
{
    int projectid;
    int mapid;
    std::string mapname;
    std::string POSX;
    std::string POSY;
    std::string SCALE;
    std::string COEFF;
    std::string FILE;
    std::string MAPWIDTH;
    std::string MAPHEIGHT;
} MapStruct;
//=====================================================

typedef struct
{
    int projectid;
    int eventid;
    std::string nodeid;
    std::string action;
    std::string areaid;
    std::string issuetime;
    std::string nodename;
    std::string areaname;
    std::string issuename;
} EventStruct;

typedef struct
{
    int firedeventid;
    int projectid;
    int eventid;// Table EventStruct eventid
    int maingroupid; // link to Table UserMainGroup.id
    std::string empid;// Table User.empid or User.account
    std::string comment; //
    std::string prettyjson; // Not save to DB
    std::string eventjson; // Store tag trigger event in JSON string
    std::string timestart; // event start time
    std::string timeend; // event occur time
    std::string readdatetime; // User read this events datetime, for mark as read.
    std::string datetime;

    std::string firstdatetime; // Not save to DB, using in DeviceEventCheckService
} FiredEventStruct;

typedef struct
{
    std::string type;
    FiredEventStruct * event1; // Alert , CallOut
    FiredEventStruct * event2; //Cancel
    std::list<FiredEventStruct*> * events;
} Pair_FiredEventStruct;
//====================================
// Inspection
typedef struct
{
    int GroupID;
    std::string GroupName;
    std::string StartTime;
    std::string EndTime;
    std::list<std::string> * Inspectors;
    int AlertTime; // in minutes send push notification before start Inspection
    int AutoFeed; // 自動加入亂數巡檢資料
    std::string issuetime;
} InspectionGroupStruct;

typedef struct
{
    int DailyRecordID;
    int GroupID;
    int Score;

    // For Runtime Check
    int Checked; // Tag already enter ane leave
    int Finished;
    int Exceed;

    std::string DateTime;
} InspectionGroupDailyRecord;

typedef struct
{
    int DailyRecordID;
    int GroupID;
    int PointID;
    int Score;

    // For Runtime Check
    int Checked; // Tag already enter ane leave
    int Finished;
    int Exceed; // for travel time record

    std::string Inspector;
    std::string RecordNodeId;
    int Status; // 0:None 1:match 2:Too Short 3:Too Long
    time_t EnterTime;
    time_t LeaveTime;
    std::string DateTime;

    // run time check
    int SendAlertMessage;
} InspectionPointDailyRecord;

typedef struct
{
    int GroupID;
    int PointID;
    int PointIndex;// start from 1
    std::string AnchorNodeId;
    int Distance;
    int MinTime;// in minutes
    int MaxTime;// in minutes
    std::string Message;
    int TravelTime;// in minutes, First Point does not has this value
    std::string issuetime;

    // For Runtime Check
    InspectionPointDailyRecord DailyData;

} InspectionPointStruct;

// For Record Tag Behavior in Checkpoint
typedef struct
{
    int GroupID;
    int PointID;
    int PointIndex;// start from 1
    std::string AnchorNodeId;
    std::string Inspector;
    int Distance;
    std::string issuetime;
    std::string DateTime;
} InspectorBehaviorData;
//====================================


//====================================
// CheckPoint
typedef struct
{
    int projectid;
    int GroupID;
    std::string GroupName;
    std::string StartTime;
    std::string EndTime;
    std::string issuetime;
} CheckPointGroupStruct;

typedef struct
{
    int DailyRecordID;
    int projectid;
    int GroupID;

    // For Runtime Check
    int Checked; // Tag already enter ane leave
    int Finished;
    int Exceed;

    std::string DateTime;
} CheckPointGroupDailyRecord;


// For Record Tag Behavior in Checkpoint
typedef struct
{
    int projectid;
    int GroupID;
    int PathID;
    int PathIndex;// CheckPointPathStruct.PathIndex
    std::string AnchorNodeId;
    std::string TagNodeId;
    int Distance;
    std::string issuetime;
    std::string DateTime;
} CheckPointTagBehaviorData;

// For Runtime Check
typedef struct
{
    std::string TagRecordNodeId;
    std::string TagNodeId;
    int TagStatus; // 0:None 1:match 2:Too Short 3:Too Long
    time_t TagEnterTime;
    time_t TagLeaveTime;
} TagRegisterTimeStruct;

typedef struct
{
    int DailyRecordID;
    int projectid;
    int GroupID;
    int PathID;

    // For Runtime Check
    int Checked; // Tag already enter ane leave
    int Finished;
    int Exceed;
    TagRegisterTimeStruct StartNodeTagData;
    TagRegisterTimeStruct EndNodeTagData;

    std::string DateTime;
} CheckPointPathDailyRecord;

typedef struct
{
    int projectid;
    int GroupID;
    int PathID;
    int PathIndex;// start from 1
    std::string StartNodeId;
    int StartNodeDistance;
    int StartNodeMinTime;
    int StartNodeMaxTime;
    std::string EndNodeId;
    int EndNodeDistance;
    int EndNodeMinTime;
    int EndNodeMaxTime;
    int TravelTime;
    std::string issuetime;

    // For Runtime Check
    CheckPointPathDailyRecord DailyData;

} CheckPointPathStruct;
//====================================



typedef struct
{
    int alertid;
    int nodetype;
    char macaddress[32];
    std::string issuetime;
} AlertRecord;

typedef struct
{
    int nodetype;
    std::string nodeid;
    int idx;
    std::string areaid;
    int posX;
    int posY;
    int posZ;
    std::string voltage;
    std::string temperature;
    std::string datetime;
} FootprintStruct;

typedef struct
{
   int projectid;
   std::string projectname;
   std::string projectdesc;
   // std::string iot_userid;
   // std::string iot_projectid;
   // std::string iot_serverurl;
   // std::string iot_serverport;
   // std::string iot_access_token;
   std::string WGS48_Origin_X;
   std::string WGS48_Origin_Y;
   std::string MAP_North;
   // std::string Building_Origin_X;
   // std::string Building_Origin_Y;
   // std::string Building_X_Length;
   // std::string Building_Y_Length;

   int BuildingID;
   int BuildingFloor;
   // std::string uploadresult;
   std::string datetime;

} VILSConfig;

typedef enum
{
    SECTION_TYPE_ZERO = 0,
    SECTION_TYPE_ONE = 1,
    SECTION_TYPE_TWO = 2,
    SECTION_TYPE_THREE = 3
} SectionTypeEnum;

typedef struct
{
   int GroupID;
   int projectid;
   SectionTypeEnum SectionType; // 0:0D, 1:1D, 2:2D, 3:3D
   std::list<std::string> * nodeIDList;
   std::string AreaID;
   int Param1;// 0D using for distance
   int Param2;// 0D using for event type 0:normal 1:0D room  2:danger area
   int Param3;
   int Param4;
   int Param5;
   int Param6;
   int maingroupid; // link to Table UserMainGroup.id
} SectionGroup;

typedef struct
{
    int GroupID;
    std::list<std::string> nodeIDList;
} SectionDeviceInfo;

typedef struct
{
    int projectid;
    int nodetype;
    char nodeid[32];
    char nodename[32];
    char panid[32];
    char time[32];
    char macaddress[32];

    //For TAF
    int tafid; // Tag role 0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師
                // Locator Button Type 0:Alert 1:CallOut 2:Door 3:訊號採集器(AB box) 4:巡檢器
    int buildingid;
    int buildingfloor;
    int maplayer; // MapLayerStruct.layerid

    double posX;
    double posY;
    double posZ;
    double prePosX;
    double prePosY;
    double prePosZ;
    double smoothingPosX;
    double smoothingPosY;
    double smoothingPosZ;
    double momentumX;
    double momentumY;
    double momentumZ;

    double velocity;

    int index;
    int alive;// alive count
    int asleep;// alive count
    char voltage[32];
    char temperature[32];
    char version[32];
    char rangingList[128];
    std::string blacklist;
    std::string whitelist;
    int joinRangingRange; // join且納入測距範圍(cm)
    int joinNotRangingRange;// join但不納入測距範圍(cm)

    time_t aliveTime;// latest alive time
    //int waitRangingCount;// when this calue countdown to 0, start send RangingAnchor_list

    char IP[32];
    char AppVersion[32];
    char PID[32];

    //=================================================
    char AreaId[32];
    char AreaName[256];

    char AreaId0D[32];
    int AreaType0D; // 0D using 0:normal 1:room 2:danger

    char AreaType5Id[32]; // 5: 病房
    char AreaType5Name[256]; // 5: 病房

    int RangingResultType; // -1:unknown 0:0D 1:1D 2:2D 3:3D

    // int area_changed; //1:in 2:out
    // char area_AreaId[32];
    // char area_AreaName[256];
    int area_unknown; // 0: 1:unknown area
    // int area_type; // AreaStruct.type 0:普通 1:電子圍欄 2:電子點名 3:房間 4:教室 5:病房 6:病床 999:虛擬牆
    // time_t area_checkTimestamp;

    char exclude_rollcall_AreaId[32];
    //=================================================

    int alert;
    int alert0D;
    time_t alertIssuTime; // alert occur time
    char LoRAMacaddress[32];

    // for websocket event
    int voltage_changed;
    int voltage_low;
    int coord_changed;

    //SectionGroup * curSectionGroup;
	//std::vector<std::string> * curSectionOneAnchors;
    time_t updateTimestamp;
    time_t poweronTimestamp;
    time_t poweroffTimestamp;

    // for Wall
    //AreaStruct * wallArea;
    MapLayerAreaStruct * wallArea;
    double wallPosX;
    double wallPosY;
    double wallPosZ;
    time_t wallTimeStamp;

    double Longitude;
    double Latitude;

    int Opertaion_Mode;// Tga: 0:None 1:UWB Ranging 2:LoRa Ranging 3:GPS
                       // Locator:  1.累積傳送 2.循環 3.一般
    int pingCount;

    int weight;
    int heartbeat;
    std::string rangingTargetid;// 與誰在測距

    time_t updateWeightTimestamp;
    time_t updateHeartBeatTimestamp;

    int enableSensorAlert;

    std::string gwid;
    std::string resident_serno;// link to Table Resident.serno
    std::string empid;// link to Table employee.empid

    int userid; // link to Table User.userid
    int maingroupid; // link to Table UserMainGroup.id
    std::string verifykey;

    std::string activity;

} NodeStruct;

typedef struct
{
    int projectid;
    char nodeid[32];
    char nodename[32];
    char panid[32];
    char macaddress[32];

    int maplayer; // MapLayerStruct.layerid

    int posX;
    int posY;
    int posZ;

    int pitch;
    int roll;
    int yaw;
    int gridsize;
    int maingroupid; // link to Table UserMainGroup.id
    std::string verifykey;

} AoANodeStruct;

//==========================================================================
// Courses
//
// typedef struct
// {
//     int dbId;
//     int projectid;
//     std::string resourceId; // '1'
//     std::string locatorId; // 'Locator0013'
//     std::string title; // 'Birthday Party'
//     std::string description; // 'description for Birthday Party Event'
//     std::string issuetime;
// } ResourceStruct;

typedef struct
{
    int dbId;
    int projectid;
    std::string resourceId; // 'Locator001' locatorId
    std::string courseId; // '1007'
    std::string title; // 'Birthday Party'
    std::string description; // 'description for Birthday Party Event'
    int allDay; // 0:false 1:true
    std::string start;// Date(y, m, d + 1, 19, 0),
    std::string end;// Date(y, m, d + 1, 22, 30),

    // repeat event
    std::string startTime;  // '06:00'
    std::string endTime;    // '07:00'
    std::list<int> * daysOfWeek;// 0:sunday
    std::string startRecur;// Date(y, m, 1),
    std::string endRecur; // Date(y, m, 30),

    std::string backgroundColor; // "#3c8dbc"  //Primary (light-blue)
    std::string borderColor; // "#3c8dbc" //Primary (light-blue)
    int maingroupid; // link to Table UserMainGroup.id
    std::string issuetime;
} CourseStruct;

typedef struct
{
    int projectid;
    std::string courseId; // '1007'
    std::string infoOfDate;
    std::list<std::string> * tags;
    std::string issuetime;
} CourseRollCallRecordStruct;
//==========================================================================

typedef struct
{
    int idx;
    int projectid;
    char TagId[32];
    char voltage[32];
    char temperature[32];
    int prePosX;
    int prePosY;
    int prePosZ;
    std::string recordnumber;
    double velocity;
    double acceleration;
    double diffacceleration;
    double preIMU[6];
    int moveoffset;// cm
    std::string status;
    time_t issuetime;
} TagIndexDataClass;

typedef struct
{
    int action; //1:in 2:out
    int AreaType0D; // 0:normal 1:room 2:danger
    int projectid;
    int tafid; // 0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師
    int MapAreaType;// 0:普通 1:電子圍欄 2:電子點名 3:房間 4:教室 5:病房 6:病床 999:虛擬牆
    std::string NodeId;
    std::string NodeName;
    std::string NodeMacaddress;
    std::string AreaID;
    std::string AreaName;
    std::string DataTime;
} AreaInOutEventStruct;

typedef struct
{
    std::string type;
    AreaInOutEventStruct * event1;
    AreaInOutEventStruct * event2;
} Pair_AreaInOutEventStruct;

typedef struct
{
    std::string nodeid;
    std::list<AreaInOutEventStruct*> * area_inout_events;
} node_AreaInOutEventStruct;

typedef struct
{
    int sf;
    int projectid;
    std::string gwip;
    std::string gwid;
    std::string repeater;
    std::list<std::string> channels;
    std::string DataTime;

    int maplayer; // MapLayerStruct.layerid
    int posX;
    int posY;
    int posZ;

    // for show info
    int alive;// alive count
    int MQTTalive;// MQTT alive count
    int pingCount;
    time_t aliveTime;

    int maingroupid;// link to Table UserMainGroup.id
    std::string verifykey;

} LoRaGatewayInfoStruct;

typedef struct
{
    std::string IP;
    int aliveCount;
    int pingCount;
} PingStatusStruct;

typedef struct
{
    int posX;
    int posY;
    int posZ;
    time_t updateTimestamp;
} PositionInfoStruct;

typedef struct
{
    std::string nodeid;
    std::string nodename;
    int projectid;
    std::list<PositionInfoStruct*> POSInfoList;
} NodePositionInfoStruct;

typedef struct
{
    int nodetype;
    std::string nodeid;
    double Longitude;
    double Latitude;
    time_t updateTimestamp;
} GPSInfoStruct;

typedef struct
{
    std::string nodeid;
    std::string nodename;
    int nodetype;
    std::list<GPSInfoStruct*> GPSInfoList;
} NodeGPSInfoStruct;

typedef struct
{
    std::string nodeid;
    std::string nodename;
} NodeItemStruct;

typedef struct
{
    int projectid;
    int alertAction;// 1:in 2:out
    std::string nodeid;
    std::string areaid;
    time_t alertIssueTime; // alert occur time
} NodeEventTimeStruct;

typedef struct
{
    int count;
    int nodetype;
    std::string nodeid;
    std::string max_time;
    std::string min_time;
    std::string day;
    std::string area_ids;
    std::string area_names;
    std::string course_id;
    std::string course_info;
    std::string cache_max_time;
    std::string cache_min_time;
} AnalyzeDayInfoStruct;

typedef struct
{
    int id;
    std::string serno;
    std::string gender;
    std::string name;
    std::string roomNo;
    std::string residentType;
    std::string nurseNo;
    std::string datetime;
} UserResidentStruct;

typedef struct
{
    int id;
    std::string empid;
    std::string cname;
    std::string orgid;
    std::string orgName;
    std::string area;
    std::string jobTitle;
    std::string gender;
    std::string pwd;
    std::string datetime;
} UserEmployeeStruct;

typedef struct
{
    int userid;
    int groupid;
    int type;// 0: vilscenter user 1:已註冊, 2:已購買點名裝置
    std::string empid;
    std::string account;
    std::string name;
    std::string unit;
    std::string title;
    std::string gender;
    std::string pwd;
    std::string datetime;
    std::list<std::string> * maingroups;
    std::list<std::string> * datagroups;
    std::list<std::string> * subgroups;
    std::list<std::string> * rfidcards;
    std::list<std::string> * nfccards;
    std::list<std::string> * uwbcards;
    std::list<std::string> * sensors;
    std::string imageuid;
    int maplayer; // MapLayerStruct.layerid
    int posX;
    int posY;
    int posZ;
    std::string Longitude;
    std::string Latitude;
    time_t alertTime;

    //=============================
    std::string AreaId;
    std::string AreaName;
    std::string AreaId0D;
    int AreaType0D; // 0D using 0:normal 1:room 2:danger
    std::string AreaType5Id; // 5: 病房
    std::string AreaType5Name; // 5: 病房

    // no save in db
    int RangingResultType; // -1:unknown 0:0D 1:1D 2:2D 3:3D
    int area_unknown; // 0: 1:unknown area
    std::string exclude_rollcall_AreaId;
    //=============================

} UserStruct;

typedef struct
{
    std::string name;
    std::string account;
    std::string title;
    std::string group;
    std::string maingroups;
    std::string datagroups;
    std::string subgroups;
    std::string rfidcards;
    std::string nfccards;
    std::string uwbcards;
    std::string sensors;
} XLSXUserStruct;

typedef struct
{
    int groupid;
    std::string groupname;
    std::string permission;
    std::string datetime;
} UserGroupStruct;

typedef struct
{
    int subgroupid;
    int maingroupid;
    std::string subgroupname;
    std::string permission;
    std::string imageuid;
    std::list<std::string> * accountidList;
    std::list<std::string> * useridList;
    int type;// 0:normal 1:curriculum
    std::string validdate;
    int curriculumrecordid; // CurriculumRecord.id
    std::string datetime;
} UserSubGroupStruct;

typedef struct
{
    int id;
    int autojoin; //0:不可自動加入 1:使用識別碼與帳號加入 2:自行加入
    std::string name;
    std::string permission;
    std::string welcommessage;
    std::list<std::string> * accountidList;
    std::list<std::string> * useridList;
    std::string imageuid;
    std::string datetime;
} UserMainGroupStruct;

typedef struct
{
    int count;
    std::string issuetime;
    std::list<std::string> * NodeIDList;
} DayRecordStruct;

typedef struct
{
    int count;
    std::string issuetime;
} ActiveTagRecordStruct;

typedef struct
{
    std::string nodeid;
    int max_alive;
    int min_alive;
} DeviceAliveCountStruct;

typedef struct
{
    int totaltime;
    std::string areaid;
    std::string areaname;
} AreaActivityTimeStruct;

typedef struct
{
    int eventid_alert;
    int eventid_cancel;
    std::string emp_empid;
    std::string emp_cname;
    std::string emp_orgName;
    std::string emp_jobTitle;
    std::string comment;
    std::string action;
    std::string nodeid;
    std::string locatorid;
    std::string resident_serno;
    std::string resident_name;
    std::string resident_roomNo;
    std::string eventtime;
    std::string reponsetime;
} EmployeeEventReportStruct;

// typedef struct
// {
//     int dbId;
//     std::string account; // UserStruct.account
//     std::string topic;
//     std::string functions;
// } PushNotifyUserStruct;

// typedef struct
// {
//     int function;
//     std::list<std::string> userList;
// } PushNotifyFunctionUserStruct;

typedef struct
{
    std::string nodeid;
    std::string targetid;
    std::string functions;
    std::list<std::string> * functionList;
} PushNotifySettingStruct;

typedef struct
{
    int type;               // 觸發類型 1.手動發送 2.Tag事件觸發 3.Locator事件觸發
    std::string nodeid;     // 觸發裝置id  [type:1 發送對象]  [type:2 Tag]  [type:3 Locator]
    std::string targetid;   // 被觸發裝置id  [type:1 發送對象]  [type:2 Tag]  [type:3 Tag(特定類型)]
    std::string topic;
    std::string messagehashkey;
    std::string messageid;
    std::string maingroups; // UserMainGroup.id
} PushNotifyMessageStruct;

typedef struct
{
    int id;
    int type;               // 觸發類型 1.手動發送 2.Tag事件觸發 3.Locator事件觸發
    std::string nodeid;     // 觸發裝置id  [type:1 發送對象]  [type:2 Tag]  [type:3 Locator]
    std::string title;
    std::string message;
    std::string messagehashkey;
    std::string targetids;
    std::string subgroupids;
    std::string surveyids;
    std::string hashkeys;
    std::string deadlines;
    // std::string accountid;
    int userid;
    std::string imageuid;
    std::string formid; // applicationformleave.formid char(30)
    std::string eventday;       // for assisteventrecord
    std::string eventtime;      // for assisteventrecord
    std::string maingroups; // UserMainGroup.id
} PushNotifyMessageRecordStruct;

typedef struct
{
    std::string formid;
    std::string applytime;
    // std::string accountid;
    int userid; // creator
    std::string subgroups;
    std::string type;
    std::string reason;
    std::string startdate;
    std::string starttime;
    std::string enddate;
    std::string endtime;
    int reissue;
    std::string applyusername;
    std::string applyuserrelation;
    std::string imageids;
    std::string fileids;
    int signoffstatus;// 0:簽核中 1:已簽核 99:未核准
    // std::string signoffaccountid;
    int signoffuserid;
    std::string maingroups; // UserMainGroup.id
    std::string datetime;
    std::string updatetime;
} ApplicationFormLeaveStruct;

typedef struct
{
    int eventid;
    int PushNotifyMessageRecorddbId;
    int surveyid;
    std::string hashkey;
    std::string deadline;
    // std::string accountid;
    std::string formid;
    std::string datetime;
    int userid; // link to Table User.id
    std::string maingroups; // UserMainGroup.id
} SurveyEventRecordStruct;

typedef struct
{
    int enable;
    std::string nodeid;
    int targettype; // NodeStruct->tafid,  0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師
    int distance;
    std::string title;
    std::string message;
} TriggerPushMessageStruct;

typedef struct
{
    int triggered;
    std::string nodeid;
    std::string tagid;
    int distance;
    time_t dataTime;
} TriggerRangingStruct;

typedef struct
{
    int triggered;
    int action; // 0:none 1:enter 2:exit
    std::string nodeid;
    std::string nodemacaddress;
    std::string tagid;
    int distance;
    time_t dataTime;
} TriggerDoorEventStruct;

typedef struct
{
    int projectid;
    std::string TagId;
    int moveoffset;// cm
    double prePosX;
    double prePosY;
    double prePosZ;
    time_t issuetime;
} TagMoveOffsetInfoStruct;

typedef struct
{
    int nodetype;
    std::string nodeid;
    int moveoffset;// cm
    int minutes; // in minutes
    std::string issueday; // foramt: YYYY-MM-DD
    std::string issuetime; // foramt: HH:mm
    std::string start_datetime; // foramt: YYYY-MM-DD HH:mm:ss
    std::string end_datetime;
} NodeMoveOffsetInfoStruct;

typedef struct
{
    int mode;
    int totaltime;
    int count;
    std::string issueday; // foramt: YYYY-MM-DD
    std::string issuetime; // foramt: HH:mm
    std::list<std::string> nodeidList;
} OperationModeActivityStruct;

typedef struct
{
    std::string nodeid;
    int gender;
    std::string weight;
    std::string heartbeat;
    std::string datetime; // foramt: YYYY-MM-DD HH:mm:ss
} BodyInfoStruct;

typedef struct
{
    int GroupID;
    std::string nodeid;
    int distance;
    std::string datetime; // foramt: YYYY-MM-DD HH:mm:ss
} FenceAlertStruct;

typedef struct
{
    int GroupID;
    int PointID;
    int MessageType;
    int timeout;
    time_t SendTime;
} SendNotificationTimeData;

typedef struct
{
    //int action; // 1:in 2:out
    int AreaType0D;// 0:normal 1:room 2:danger
    int MapAreaType;// 0:普通 1:電子圍欄 2:電子點名 3:房間 4:教室 5:病房 6:病床 999:虛擬牆
    std::string areaid;
    std::string areaname;

    bool checked_in;
    double velocity_in;
    double voltage_in;
    time_t timestamp_in;

    bool checked_out;
    double velocity_out;
    double voltage_out;
    time_t timestamp_out;
} BufferAreaInfoStruct;

typedef struct
{
    int projectid;
    std::string nodeid;
    int tafid; // 0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師
    int isuser; // 0: tag 1: User
    std::list<BufferAreaInfoStruct*> mBufferAreaList;
} TagAreaBufferStruct;

typedef struct
{
    int projectid;
    std::string nodeid;
    int tafid; // 0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師
    int isuser; // 0: tag 1: User
    int check_count;
    int slept;
    time_t timestamp;
} SleepPoolStruct;

typedef struct
{
    std::string header;
    int ShowWidth;
} PDFTableHeaderDataStruct;

typedef struct
{
    std::string header;
    std::string data;
} PDFTableColDataStruct;

typedef struct
{
    std::list<PDFTableColDataStruct *> coldatalist;
} PDFTableRowDataStruct;

typedef struct
{
    std::string type;// dailt, weekly, monthly
    std::string startdate;
    std::string enddate;
    std::string tableinfo;
    std::list<PDFTableHeaderDataStruct *> headerlist;
    std::list<PDFTableRowDataStruct *> rowdatalist;
} PDFTableDataStruct;

typedef struct
{
    int nodetype;
    std::string nodeid;
    std::string name;// 儀器名稱
    std::string type;// 廠牌型別
    std::string unit;// 保管單位
    std::string manager;// 管理人員
    std::string assetlife;// 資產年限
    std::string maintenancedate;// 保養校正(日期通知)
    std::string status;// 儀器設備狀況(良好、維修校正、故障)
    std::string datetime; // foramt: YYYY-MM-DD HH:mm:ss
} DeviceMaintenanceRecord;

typedef struct
{
    int surveyid;
    std::string title;
    std::string description;
    int type; // 0:單選 1:複選
    std::string subgroups;
    std::string DateTime;
    int userid; // link to Table User.id
    std::string maingroups; // UserMainGroup.id
} SurveyStruct;

typedef struct
{
    int surveyid;
    int surveyitemid;
    std::string description;
    std::string DateTime;
} SurveyItemStruct;

// http://smimsvils.appspot.com/vilsapi?type=getsurveyresult&hash=AhgXQ78urAI8
typedef struct
{
    std::string topic;
    std::string messagehashkey;
    std::string hashkey;
    std::string token;
    std::string comment;
    std::list<std::string> results; // "0,1,0,1" -> 每一個代表是否按下選擇
    std::string datetime;
} GAESurveyResultData;

// https://smimsvils.appspot.com/vilsapi?type=getmessagestatus&messageid=1587115188857821
typedef struct
{
    std::string messageid;
    std::string topic;
    std::string messagehashkey;
    std::string token;
    std::string read;
    std::string finished;
    std::string datetime;
} GAEMessageStatusData;

typedef struct
{
    std::string target;
    std::string topic;
    std::string name;
    std::string accountid;
    std::string datetime;
    int maingroupid; // link to Table UserMainGroup.id
    int userid; // link to Table User.id
} TargetTopicStruct;

typedef struct
{
    int alive;// alive count
    int deviceid;
    std::string macaddress;
    std::string name;
    std::string type;
    std::string gwid;
    std::string LoRAMacaddress;
    std::string ssid;
    std::string wifimac;
    std::string version;
    std::string verifykey;
    std::string datetime;
    time_t aliveTime;// latest alive time
    int maingroupid; // link to Table UserMainGroup.id
} NFCDeviceStruct;

typedef struct
{
    int alive;// alive count
    std::string cardid;
    std::string name;
    std::string type;
    int maplayer;
    int userid;
    int unknown;
    int deviceid;
    std::string datetime;
    std::string updatetime;
    time_t aliveTime;// latest alive time
    int maingroupid; // link to Table UserMainGroup.id
} NFCCardStruct;

typedef struct
{
    int alive;// alive count
    int deviceid;
    std::string macaddress;
    std::string name;
    std::string type;
    std::string ip;
    std::string gwid;
    std::string LoRAMacaddress;
    std::string datetime;
    time_t aliveTime;// latest alive time
    int maingroupid; // link to Table UserMainGroup.id
    std::string verifykey;
} RFIDDeviceStruct;

typedef struct
{
    int alive;// alive count
    std::string cardid;
    std::string name;
    std::string type;
    int maplayer;
    int userid;
    int unknown;
    int deviceid;
    std::string datetime;
    std::string updatetime;
    time_t aliveTime;// latest alive time
    int maingroupid; // link to Table UserMainGroup.id
} RFIDCardStruct;

typedef struct
{
    int alive;// alive count
    int deviceid;
    std::string macaddress;
    std::string name;
    std::string type;
    std::string ip;
    std::string gwid;
    std::string LoRAMacaddress;
    std::string version;
    std::string datetime;
    time_t aliveTime;// latest alive time
    int maingroupid; // link to Table UserMainGroup.id
    std::string verifykey;
} EDGEDeviceStruct;

typedef struct
{
    int rollcallid;
    std::string rollcallname;
    std::string StartTime;
    std::string EndTime;
    std::string StartDate;
    std::string EndDate;
    std::list<int> rfiddeviceids;
    std::list<std::string> nfclocators;
    std::list<int> subgroups;
    std::list<int> weekday;
    std::list<std::string> validdate;
    std::list<std::string> invaliddate;
    int presentnotify;
    std::string presentcomment;
    int absentnotify;
    std::string absentcomment;
    int resend;
    std::string maingroups; // UserMainGroup.id
    int type;// 0:normal 1:curriculum
    int curriculumrecordid; // CurriculumRecord.id
    std::string datetime;
    std::string updatetime;
} CARDRollCallStruct;

typedef struct
{
    int eventid;
    int rollcallid;
    std::string rollcallname;
    std::string StartDateTime;
    std::string EndDateTime;
    int totalcount;
    int presentcount;
    std::string datetime;
    std::string updatetime;
    std::string maingroups; // UserMainGroup.id
} CARDRollCallEventStruct;

typedef struct
{
    int rollcallid;
    int rollcalleventid;
    std::string cardid;
    int userid;
    std::string nodeid;
    //int rfiddeviceid;
    //std::string nfclocator;
    std::list<int> rfiddeviceids;
    std::list<std::string> nfclocators;
    int present;
    std::string datetime;
    std::string updatetime;
    std::string maingroups; // UserMainGroup.id
} CARDRollCallRecordStruct;

typedef struct
{
    int userid;
    int present;
    std::string datetime;
    std::list<CARDRollCallRecordStruct *> recordlist;
    std::string maingroups; // UserMainGroup.id
} USERRollCallRecordStruct;

typedef struct
{
    int eventid;
    int rollcallid;
    int subgroupid;
    std::string rollcallname;
    std::string StartDateTime;
    std::string EndDateTime;
    int totalcount;
    int presentcount;
    std::list<int> userids;
    std::list<int> presentuserids;
    std::string datetime;
    std::string updatetime;
    std::string maingroups; // UserMainGroup.id
} CARDRollCallSubGroupEventStruct;

typedef struct
{
    int id;
    int cardtype; // 0:NFC 1:UHF
    std::string cardid;
    std::string LoRAMacaddress;// 感應裝置 lora macaddress
    std::string ssid;
    std::string wifimac;
    std::string datetime;// 感應時間
} CARDScanRecordStruct;

typedef struct
{
    int rssi;
    std::string datetime;// 資料時間
} RSSIInfoStruct;

typedef struct
{
    std::string gwid;
    std::list<RSSIInfoStruct*> * rssiRecords;
} GatewayRSSIInfoStruct;

typedef struct
{
    std::string LoRAMacaddress;
    std::list<GatewayRSSIInfoStruct*> * gatewayRecords;
} LocatorRSSIInfoStruct;

typedef struct
{
    int userid;
    std::string accountid;
    std::string token;
    std::string loginkey;
} LoginTokenStruct;

typedef struct
{
    int id;
    int userid; // creator
    // std::string accountid; // creator
    std::string title;
    std::string description;
    std::string subgroups;
    // std::string assignaccounts;
    std::string assignusers;
    std::string maingroups; // UserMainGroup.id
} AssistStruct;

typedef struct
{
    int id;
    int assistid;
    int userid; // creator
    // std::string accountid; // creator
    std::string description;
    std::list<std::string> eventtime;
    std::list<int> weekday;
    std::list<std::string> validdate;
    std::list<std::string> invaliddate;
    std::string deadline;
    std::string imageids;
    std::string fileids;
    std::string applyusername;
    std::string applyuserrelation;
    int signoffstatus;
    //std::string signoffaccountid;
    int signoffuserid;
    std::string maingroups; // UserMainGroup.id
    std::string datetime;
} AssistEventStruct;

typedef struct
{
    int id;
    int assistid;
    int assisteventid;
    int PushNotifyMessageRecorddbId;
    int userid; // 負責人
    // std::string accountid;
    std::string messagehashkey;
    std::list<std::string> results; // "0,1,0,1" -> 每一個代表是否按下選擇
    std::string eventday;
    std::string eventtime;
    std::string maingroups; // UserMainGroup.id
    std::string datetime;
    std::string updatetime;
} AssistEventRecordStruct;

typedef struct
{
    int id;
    int userid; // 發起人
    int curriculumid;// Curriculum.id
    int shared;
    std::string name;
    std::string maingroups; // UserMainGroup.id
    std::string number;
    std::string opendate;
    std::string closedate;
    std::string startdate;
    std::string enddate;
    std::string starttime;
    std::string endtime;
    std::string signinstarttime;
    std::string signinendtime;
    std::string weekday;
    std::string totalhours;
    std::string location;
    std::string description;
    std::string teacher;
    int teacheruserid;
    int maxnumber;
    int autoapproval;
    std::string cost;
    std::string note;
    std::string files;
    int status;
    std::string datetime;
} CurriculumRecordStruct;

typedef struct
{
    int id;
    int userid;
    int curriculumtypeid;
    int recordid; // CurriculumRecord.id
    std::string prefix;
    std::string subgroups;
    std::string maingroups;
    std::string number;
    int shared;
    std::string sharedmaingroups;
    std::string datetime;
} CurriculumStruct;

typedef struct
{
    int id;
    std::string name;
    std::string maingroups;
    std::string datetime;
} CurriculumTypeStruct;

typedef struct
{
    int userid;
    int curriculumrecordid;
    int status;
    std::string datetime;
} CurriculumApplyStruct;

typedef struct
{
    int id;
    std::string name;
    std::string day;
    std::string value;
    std::string jsonvalue;
    std::string Longitude;
    std::string Latitude;
    std::string datetime;
    std::string maingroups; // UserMainGroup.id
} SensorInfoStruct;

typedef struct
{
    int alive;// alive count
    int maingroupid; // UserMainGroup.id
    std::string id;
    std::string name;
    std::string type;
    std::string jsonvalue;
    std::string Longitude;
    std::string Latitude;
    std::string Battery;
    std::string Version;
    std::string datetime;
    int unknownLocation; // not save to db
    time_t updateLocationTime;// latest update GPS time
    time_t aliveTime;// latest alive time
    std::string verifykey;
    int value_buffer_size;
    int value_buffer_index;
    std::string value_buffer[SENSOR_VALUE_BUFFER_SIZE]; // not save to db
    std::string value_datetime[SENSOR_VALUE_BUFFER_SIZE]; // not save to db
} SensorDeviceStruct;

typedef struct
{
    int id;
    std::string sensorid;
    std::string type;
    std::string jsonvalue;
    std::string datetime;
    int maingroupid; // UserMainGroup.id
} SensorFiredEventStruct;

typedef struct
{
    std::string FDATE;
    std::string expectTime;
    std::string airLineCode;
    std::string airLineNum;
    std::string type;
    std::string jsonvalue;
    std::string triggerTime;
    std::string datetime;
    int maingroupid; // UserMainGroup.id
    std::string sensorid;
    std::string airBoardingGate;
} FlyInfoStruct;
JS_OBJ_EXT(FlyInfoStruct, FDATE, expectTime, airLineCode, airLineNum, type, jsonvalue, triggerTime, datetime, maingroupid, sensorid, airBoardingGate);

typedef struct
{
    std::string airLineNum;
    std::string airLineCode;
    std::string airBoardingGate;

    // JS_OBJ(airLineNum, airLineCode, airBoardingGate);
} FlyGateInfoStruct;
JS_OBJ_EXT(FlyGateInfoStruct, airLineNum, airLineCode, airBoardingGate);

typedef struct
{
    std::string GID;
    std::string place;
    std::string item;
    std::string notetime;
    int sendNotify;
    int maingroupid;
} WCInfoStruct;

typedef struct
{
    std::string userid;
    std::string areaid;
    std::string airremain;
} UserAirRemainStruct;

typedef struct
{
    int id;
    int maingroupid;
    int caseclosed;
    std::string casenumber;
    std::string address;
    std::string lat;
    std::string lng;
    std::string starttime;
    std::string closetime;
    std::string datetime;
    std::list<int> * NFCDeviceIdList;
    std::list<std::string> * StandbyDeviceIds;
} FireFightingStruct;

typedef struct
{
    int userid;
    int maingroupid;
    int areaType;
    float airremain;
    time_t enterTime;
    std::string casenumber;
    std::string activity;
    std::string HR;
    std::string SPO2;
    std::string battery;
} FireFightingUserStruct;

typedef struct
{
    int id;
    std::string casenumber;
    std::string eventtype;
    std::string jsonstring;
    std::string datetime;
} FireFightingEventStruct;

typedef struct
{
    int areaType;
    float airremain;
    time_t enterTime;
    time_t datetime;
} FireFightingAirInfoStruct;

typedef struct
{
    int userid;
    std::list<FireFightingAirInfoStruct*> * AirInfoList;
} FireFightingUserAirInfoStruct;

typedef struct
{
    int recordid;
    int maingroupid;
    std::string recordnumber;
    time_t starttime;
    time_t endtime;
} SoccerRecordingStruct;

typedef struct
{
    int userid;
    std::string casenumber;
    float startairremain;
    time_t starttime;
    float endairremain;
    time_t endtime;
    float airusing;
    time_t totaltime;
} FireFightingAirUsingStruct;

typedef struct
{
    std::string Device_ID;
    int Blood_Oxygen;
    int DBP;
    int Heart_Rate;
    int Rssi;
    int SBP;
    int SOS;
    double Temperature;
    std::string repeater_ip;
    std::string repeater_mac;
} wentaitek_s3_watch_deviceStruct;

typedef struct
{
    std::string Device_ID;
    int Rssi;
    std::string repeater_ip;
    std::string repeater_mac;
} wentaitek_uw_sensor_1_deviceStruct;

typedef struct
{
    int id;
    std::string name;
    int uniAddress;
    int state_onOff;
    int state_level;
    int state_cct;
} wentaitek_lightStruct;

typedef struct
{
    std::string Device_ID;
    std::string BT_version;
    int Rssi;
    double Active_Energy;
    double Active_Power;
    double Current_RMS;
    double Frequency;
    double Voltage_RMS;
    bool relay_state;
    std::string repeater_ip;
    std::string repeater_mac;
} wentaitek_power_meter_deviceStruct;

#endif
