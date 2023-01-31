
#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sstream>
#include <iostream>

#define CENTER_APP_VERSION "3.1.3"
#define APP_DIRECTORY "/Users/smims/soccer/app"
#define LOG_DIRECTORY "/Users/smims/soccer/www/log"
#define OUTPUT_DIRECTORY "/Users/smims/soccer/www"
#define DI_WEBPAGE "http://127.0.0.1:8080/di/"
#define CENTER_WEBPAGE "http://127.0.0.1:8080/"

#define PSUH_NOTIFY_SERVER "103.17.9.146"
#define PSUH_NOTIFY_SERVER_PORT "8080"

#define MySQLDB_server "tcp://127.0.0.1:3306"
#define MySQLDB_username "soccer"
#define MySQLDB_password "soccer@soccers2022cJgyRd"
// #define configsqldbname  "soccer_vils_config_db"
// #define sqldbname_fp  "soccer_vils_fp_db"
// #define sqldbname  "soccer_vils_db"
// #define sqldbname_ts  "soccer_vils_ts_db"
// #define tracksqldbname  "soccer_vils_track_db"
// #define sqlite3db_prefix  "soccer_vils"

// #define MySQLDB_username "norgay"
// #define MySQLDB_password "norgay@vils2020K6GF91"
#define configsqldbname  "norgay_vils_config_db"
#define sqldbname_fp  "norgay_vils_fp_db"
#define sqldbname  "norgay_vils_db"
#define sqldbname_ts  "norgay_vils_ts_db"
#define tracksqldbname  "norgay_vils_track_db"
#define sqlite3db_prefix  "norgay_vils"

#define ApplicationForm_LEAVE_SURVEYID "88888888"
#define ApplicationForm_ASSIST_ID "99999999"
#define ApplicationForm_ASSISTEVENT_ID "99999998"
#define ApplicationForm_CURRICULUM_APPLYID "99999997"

#define CHT_IOT_DeviceID ""
#define CHT_IOT_DeviceKey ""
#define CHT_IOT_SensorID ""

#define ENABLE_LOCAL_COORDINATOR
// #define ENABLE_CHT_IOT
// #define ENABLE_KIA_SERVICE

//===============================
#define ENABLE_SEND_BODYINFO_ALERT
#define SPO2_WARNING_MIN 90
#define SPO2_WARNING_MAX 95
#define SPO2_DANGER_MIN 1
#define SPO2_DANGER_MAX 89
#define HR_WARNING_MIN 120
#define HR_WARNING_MAX 180
#define HR_DANGER_MIN 181
#define HR_DANGER_MAX 254
//===============================

#define INSPECTION_HEHAVIOR_RECORD_THRESHOLD 500 // cm
#define INSPECTION_HEHAVIOR_RECORD_LEAVE_TIMEOUT 600 // seconds
#define INSPECTION_HEHAVIOR_RECORD_MAX_TIMEOUT 1800 // seconds

#define MAX_ANCHOR_NUMBER 256 // MAX 100 anchor scenario
#define MAX_TAG_NUMBER 512  // MAX 200 Tag scenario
#define MAXUDPPACKET 65536

#define RANGING_BUFFER_SIZE 5
#define MAX_RANGING_RESULT 10

#define URI_MAX_LEN 128
#define DATA_MAX_LEN 4096

#define MAX_POSINFO_LIST_SIZE 128
#define MAX_GPSINFO_LIST_SIZE 128

#define UNASSIGN_RESIDENT_START_X 0
#define UNASSIGN_RESIDENT_START_Y (-1000)

#define SENSOR_VALUE_BUFFER_SIZE 1000

#define SECTION_0D_EXCLUDE_AREA

enum
{
    UDP = 1,
    MULTICAST = 2,
    TCP = 3
};

typedef struct
{
   int SolveTagPosition_Time;
   int SendTAFDB_Time;
   int SendTagAnchorList_Time;
   int SendDeviceStatus_Time;
   int UpdateNodeFromSMIMSIOTSERVER_Time;

   int web_udp_port;//  center app <-- center web
   int center_udp_port;//  center app <-- coord app
   int coord_udp_port;//   coord app <-- center app
   int center_broadcast_time;
   int center_broadcast_port;//   coord app <-- center app broadcast ip
   int web_socket_port;//  center app web socket port
   int UsingProtocol; // 1:UDP 2:Multicast 3:TCP
   std::string VILSServer;
   std::string VILSUID;
   std::string ERPServer;
   std::string SSOIP; // single sign on IP

   std::string PushNotifyPrefix;
   std::string PushNotifyWelcomeTitle;
   std::string PushNotifyWelcomeBody;
   int EnablePushNotify;

   int TagAlertTimeout;
   std::string TagAlertVoltageLow;

   //
   //VILS Center Algorithm Setting
   //

   //1.Ranging Data Preprocessing
   int ValidDataTime;
   int MaxRangindDistance;
   int EnableNoiseMakup;// 0:disable 1:enable
   float NoiseMakupRatio; // 2.0 實際測距值與平均測距值比例, 大於此數時計算合理測距值取代目前實際測距值
   float NoiseMakupWeight; //0.8 實際測距值併入平均測距值的比重.(0.1~1.0)
   int EnableKalman1D;// 0:disable 1:enable
   int EnableRangingReliability;// 0:disable 1:enable
   //int RangingDataMethod; // 0: kalman 1D, 1:RangingReliability
   int RangingReliabilityMaxDistance; // 最大可用測距值(cm), 大於此數將捨棄.
   int RangingReliabilityMinCount; // 特定時間內至少可用測距次數, 少於此數將捨棄.
   int RangingReliabilityAverageDiff; // 最大平均測距差(cm), 大於此數將捨棄.

   //2.Position Calculation
   int EnableAnchor1DMapping;// 0:disable 1:enable
   int EnableOneAnchorCalculation;// 0:disable 1:enable
   int EnableGDOP;// 0:disable 1:enable
   int GDOPThreshold; // default:60
   int CalculationMethod; // 0: kalman 3D, 1:SMOOTHING_FILTER, 2:Weight Moving Average
   int EnableIMUVelocity;// 0:disable 1:enable
   int IMUVelocityOffset;// default:100
   int SteadyStateOffset;// default:5
   float WeightMovingAverageValue1; //0.01
   float WeightMovingAverageValue2; //0.02
   float WeightMovingAverageValue3; //0.97
   int StayInWallTimeout;// default:5000
   int EnableFixHeight;// 判斷tag是否過高或過低於Anchor/Locator.

   //3.Position Result
   int SmoothingResult; // 0:disable 1:enable
   int SmoothingResultOffset; // centermeter
   int SmoothingResultStep; // default:6
   float SmoothingResultFirstStateOffsetRatio;
   int SmoothingResultFirstStateRunTime;
   float SmoothingResultSecondStateOffsetRatio;
   int SmoothingResultSecondStateRunTime;

   //
   // LoRAWAN
   //
   std::string LoRAWANTPOIC;
   std::string MQTTHostIp;
   int MQTTHostPort;
   std::string MQTTHostUsername;
   std::string MQTTHostPassword;
   std::string MQTTClientId;

   //
   // assist
   //
   int sendNotifyTime; // in minute, for AssistService

} VILSSystemConfig;

#endif
