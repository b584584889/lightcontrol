
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <vector>
#include <ctime>
#include <sstream>
#include <iostream>
#include <pthread.h>
#include <signal.h>

#if defined(__unix__) || defined(linux)
#include <sys/resource.h>
#include <malloc.h>
#include <sys/vfs.h>
#endif

#include "VILS_RTLS.h"

#include "config.h"
#include "common.h"

#include "mysql/DBManager.h"

#include "PosCalculate/algorithm.h"
#include "MQTT/LoRAGateway.h"

#ifdef ENABLE_LOCAL_COORDINATOR
#include "ServiceThreads/CoordIncomingService.h"
#include "ServiceThreads/CoordCommandService.h"
#include "ServiceThreads/CoordMsgParserService.h"
#include "ServiceThreads/CenterOutgoingService.h"
#include "ServiceThreads/BroadcastIPService.h"
#include "ServiceThreads/BroadcastDevicesService.h"
#include "ServiceThreads/DataParserService.h"
#endif

#include "ServiceThreads/MQTTSubscribeService.h"
#include "ServiceThreads/MQTTPublishService.h"
#include "ServiceThreads/MQTTProcessingService.h"
#include "ServiceThreads/CenterWebSocketService.h"
#include "ServiceThreads/MySQLServiceLoopThread.h"
#include "ServiceThreads/WebUDPAPIService.h"
#include "ServiceThreads/WebUDPAPIService_DMATEK.h"
#include "ServiceThreads/WebUDPParserService.h"
#include "ServiceThreads/DeviceEventCheckService.h"
#include "ServiceThreads/PosCalculateService.h"
#include "ServiceThreads/WebSocketEventCheckService.h"
#include "ServiceThreads/InspectionGroupService.h"
#include "ServiceThreads/CoursesService.h"
#include "ServiceThreads/AnalyzeFootprintService.h"
#include "ServiceThreads/AnalyzeTrackService.h"
#include "ServiceThreads/AnalyzeNodePosOffsetService.h"
#include "ServiceThreads/AnalyzeDashBoardService.h"
#include "ServiceThreads/PingDeviceService.h"
#include "ServiceThreads/TrackDBService.h"
#include "ServiceThreads/AreaRollCallService.h"
#include "ServiceThreads/PushNotificationService.h"
#include "ServiceThreads/SyncERPService.h"
#include "ServiceThreads/FootprintDBService.h"
#include "ServiceThreads/CheckEmployeeActionService.h"
#include "ServiceThreads/ForacareService.h"
#include "ServiceThreads/GAEService.h"
#include "ServiceThreads/CARDRollCallService.h"
#include "ServiceThreads/AssistService.h"
#include "ServiceThreads/MQProcessingService.h"
#include "ServiceThreads/FireFightingService.h"
#ifdef ENABLE_CHT_IOT
#include "ServiceThreads/CHTIoTService.h"
#endif
#ifdef ENABLE_KIA_SERVICE
#include "ServiceThreads/SensorInfoService.h"
#include "ServiceThreads/KIAService.h"
#endif
#include "ServiceThreads/SimulationService.h"

using namespace std ;

//
// Global value
//
DeviceManager * glDeviceManager = NULL;
DBManager * glDBManager = NULL;

//
// Service Threads
//
#ifdef ENABLE_LOCAL_COORDINATOR
CoordIncomingService * glThreadCoordService = NULL;
CoordCommandService * glCoordCMDService = NULL;
CoordMsgParserService * glThreadCoordMsgService = NULL;
CenterOutgoingService * glThreadCenterService = NULL;
BroadcastIPService * glBroadcastIPService = NULL;
BroadcastDevicesService * glBroadcastDevicesService = NULL;
DataParserService * glDataParserService = NULL;
#endif

MQTTSubscribeService * glMQTTSubscribeService = NULL;
MQTTPublishService * glMQTTPublishService = NULL;
MQTTProcessingService * glMQTTProcessingService = NULL;
CenterWebSocketService * glCenterWebSocketService = NULL;
MySQLServiceLoopThread * glMySQLService = NULL;
WebUDPAPIService * glWebUDPAPIService = NULL;
WebUDPAPIService_DMATEK * glWebUDPAPIService_DMATEK = NULL;
WebUDPParserService * glWebUDPParserService = NULL;
DeviceEventCheckService * glDeviceEventCheckService = NULL;
PosCalculateService * glPosCalService = NULL;
WebSocketEventCheckService * glWebSocketEventCheckService = NULL;
//CheckPointGroupService * glCheckPointGroupService = NULL;
InspectionGroupService * glInspectionGroupService = NULL;
CoursesService * glCoursesService = NULL;
AnalyzeFootprintService * glAnalyzeFootprintService = NULL;
AnalyzeTrackService * glAnalyzeTrackService = NULL;
AnalyzeNodePosOffsetService * glAnalyzeNodePosOffsetService = NULL;
AnalyzeDashBoardService * glAnalyzeDashBoardService = NULL;
PingDeviceService * glPingDeviceService = NULL;
TrackDBService * glTrackDBService = NULL;
AreaRollCallService * glAreaRollCallService = NULL;
PushNotificationService * glPushNotificationService = NULL;
SyncERPService * glSyncERPService = NULL;
FootprintDBService * glFootprintDBService = NULL;
CheckEmployeeActionService * glCheckEmployeeActionService = NULL;
ForacareService * glForacareService = NULL;
GAEService * glGAEService = NULL;
CARDRollCallService * glCARDRollCallService = NULL;
AssistService * glAssistService = NULL;
MQProcessingService * glMQProcessingService = NULL;
FireFightingService * glFireFightingService = NULL;

#ifdef ENABLE_CHT_IOT
CHTIoTService * glCHTIoTService = NULL;
#endif
#ifdef ENABLE_KIA_SERVICE
SensorInfoService * glSensorInfoService = NULL;
KIAService * glKIAService = NULL;
#endif
SimulationService * glSimulationService = NULL;

// Create the queue and consumer (worker) threads
#ifdef ENABLE_LOCAL_COORDINATOR
wqueue<UDPDataItem*> glUDPJobQueue; // Coord incoming UDP
wqueue<DataItem*> glCoordJobQueue; // Coord incoming UDP
wqueue<DataItem*> glSendOutJobQueue; // outgoing to coord
#endif

wqueue<WorkItem*> glMySQLJobQueue; // save to local MySQL DB
wqueue<WorkItem*> glMySQLTSJobQueue; // For AnalyzeFootprintService
wqueue<WorkItem*> glTrackDBJobQueue; // For TrackDBService
wqueue<WorkItem*> glAnalyzeDashBoardJobQueue; // For AnalyzeDashBoardService
wqueue<UDPDataItem*> glUDPAPIJobQueue; //Web UDP API
wqueue<DataItem*> glPosCalJobQueue; // send to PosCalculate
wqueue<DataItem*> glInspectionJobQueue; // send to InspectionGroupService
// wqueue<DataItem*> glCheckPointJobQueue; // send to CheckPointService
wqueue<DataItem*> glCoursesJobQueue;//send to CoursesService
wqueue<DataItem*> glMainJobQueue; // Main peocess queue
wqueue<MQTTDataItem*> glMQTTJobQueue; // MQTT subscribe input data
wqueue<MQTTDataItem*> glMQTTPubJobQueue; // MQTT publish input data
wqueue<SubscribeDataItem*> glWebSocketJobQueue;// Webe socket
wqueue<AreaInOutEventStruct*> glAreaInOutEventQueue;
wqueue<AreaInOutEventStruct*> glCoursesAreaInOutEventQueue;//send to CoursesService
wqueue<AreaInOutEventStruct*> glWebSocketAreaInOutEventQueue;
wqueue<DataItem*> glSyncERPQueue;
wqueue<NotificationItem*> glNotificationItemQueue;//send to CoursesService
wqueue<WorkItem*> glNodePositionQueue;
wqueue<DataItem*> glDeviceEventJobQueue; // send to DeviceEventCheckService
wqueue<WorkItem*> glForacareServiceJobQueue; // send to ForacareService
wqueue<DataItem*> glCARDRollCallJobQueue;// send to CARDRollCallService
wqueue<DataItem*> glCHTIoTJobQueue;// send to CHTIoTService
wqueue<DataItem*> glGAEJobQueue;// send to GAEService
wqueue<DataItem*> glAssistJobQueue;// send to AssistService
wqueue<WorkItem*> glSensorInfoJobQueue;// send to SensorInfoService
wqueue<DataItem*> glMQJobQueue;// send to MQProcessingService
wqueue<DataItem*> glWebDMATEKJobQueue;// send to WebUDPAPIService_DMATEK

static int s_interrupted = 0;

int main(int argc, char* argv[])
{
    printf("main start...\n");
    vilslog::printf("main start...");
    vilslog::printf("main start3...");

    if (argc < 2)
    {
        printf("redirect printf to file\n");
        vilslog::printf("MODE_FILE\n");
        vilslog::setMode(MODE_FILE);
        vilslog::printf("MODE_FILE\n");
    }
    else
    {
        printf("Arguments argv[1][%s]\n", argv[1]);
        vilslog::printf("MODE_PRINTF\n");
        vilslog::setMode(MODE_PRINTF);
        vilslog::printf("MODE_PRINTF\n");
    }

    s_catch_signals();

    // PrintNetworkInterfaceInfos();

    vilslog::printf("wait MySql server wakeup....\n");

    glDBManager = new DBManager();
    glDBManager->init();

    //
    // print out define value
    //
    //printf_VILSSystemConfig(glVILSsysConfig);

    glDeviceManager = new DeviceManager(glDBManager);

    //=================
    // MySQL service
    //
    glMySQLService = new MySQLServiceLoopThread(glMainJobQueue, glMySQLJobQueue, glDBManager);
    glMySQLService->start();
    vilslog::printf("MySQLServiceLoopThread start()...\n");
    //=================

#ifdef ENABLE_LOCAL_COORDINATOR
    //
    // VILS Center outgoing packet
    //
    glThreadCenterService = new CenterOutgoingService(glSendOutJobQueue, glDBManager);
    glThreadCenterService->start();

    //
    // wait VILS Coord incoming packet
    //
    glCoordCMDService = new CoordCommandService(glDBManager);
    glCoordCMDService->start();

    glThreadCoordService = new CoordIncomingService(glUDPJobQueue, glDBManager);
    glThreadCoordService->start();

    glThreadCoordMsgService = new CoordMsgParserService(glCoordJobQueue, glUDPJobQueue, glDeviceManager);
    glThreadCoordMsgService->start();

    //
    // Broadcast Center IP service
    //
    glBroadcastIPService = new BroadcastIPService(glDBManager);
    glBroadcastIPService->start();

    //
    // Broadcast Devices service
    //
    glBroadcastDevicesService = new BroadcastDevicesService(glSendOutJobQueue, glDeviceManager, glDBManager);
    glBroadcastDevicesService->start();

    //
    // Coordinator incomming data parse service
    //
    glDataParserService = new DataParserService(glCoordJobQueue, glPosCalJobQueue, glWebSocketJobQueue, glNotificationItemQueue, glMySQLJobQueue, glDeviceManager);
    glDataParserService->start();

#endif

    glAnalyzeFootprintService = new AnalyzeFootprintService(glDeviceManager, glDBManager);
    glAnalyzeFootprintService->start();

    glAnalyzeTrackService = new AnalyzeTrackService(glTrackDBJobQueue, glDeviceManager, glDBManager);
    glAnalyzeTrackService->start();

    glAnalyzeNodePosOffsetService = new AnalyzeNodePosOffsetService(glTrackDBJobQueue, glNodePositionQueue, glDeviceManager);
    glAnalyzeNodePosOffsetService->start();

    glAnalyzeDashBoardService = new AnalyzeDashBoardService(glAnalyzeDashBoardJobQueue, glTrackDBJobQueue, glDeviceManager, glDBManager);
    glAnalyzeDashBoardService->start();

    //
    // wait MQTT message incoming
    //
    glMQTTSubscribeService = new MQTTSubscribeService(glMainJobQueue, glMQTTJobQueue, glMQTTPubJobQueue, glDeviceManager, glDBManager);
    glMQTTSubscribeService->start();

    //
    // send MQTT message, not working
    //
    glMQTTPublishService = new MQTTPublishService(glMQTTPubJobQueue, glDBManager);
    glMQTTPublishService->start();

    //
    // wait MQTT message incoming
    //
    glMQTTProcessingService = new MQTTProcessingService(glMainJobQueue, glMQTTJobQueue, glWebSocketJobQueue, glNotificationItemQueue, glSensorInfoJobQueue, glMySQLJobQueue, glPosCalJobQueue, glNodePositionQueue, glInspectionJobQueue, glCARDRollCallJobQueue, glDeviceManager);
    glMQTTProcessingService->start();

    //
    // create web socket service
    //
    glCenterWebSocketService = new CenterWebSocketService(glWebSocketJobQueue, glDBManager);
    glCenterWebSocketService->start();

    //
    //Web UDP API service
    //
    glWebUDPAPIService = new WebUDPAPIService(glUDPAPIJobQueue, glDBManager);
    glWebUDPAPIService->start();

    glWebUDPAPIService_DMATEK = new WebUDPAPIService_DMATEK(glUDPJobQueue, glWebDMATEKJobQueue);
    glWebUDPAPIService_DMATEK->start();

    vilslog::printf("WebUDPParserService::Start");
    glWebUDPParserService = new WebUDPParserService(glMySQLJobQueue, glMainJobQueue, glUDPAPIJobQueue, glNotificationItemQueue, glDeviceManager, glDBManager);
    glWebUDPParserService->start();

    //
    //Web UDP API service
    //
    glDeviceEventCheckService = new DeviceEventCheckService(glDeviceEventJobQueue, glAreaInOutEventQueue, glWebSocketJobQueue, glNotificationItemQueue, glMySQLJobQueue, glDeviceManager, glDBManager);
    glDeviceEventCheckService->start();

    //
    // Check Employee Action Service
    //
    glCheckEmployeeActionService = new CheckEmployeeActionService(glMySQLJobQueue, glDeviceManager, glDBManager);
    glCheckEmployeeActionService->start();

    //
    // PosCalculate service
    //
    glPosCalService = new PosCalculateService(glPosCalJobQueue, glMySQLJobQueue, glMySQLTSJobQueue, glInspectionJobQueue, glNodePositionQueue, glDeviceEventJobQueue, glDeviceManager, glDBManager);
    glPosCalService->start();

    //
    // WebSocket Event Check service
    //
    glWebSocketEventCheckService = new WebSocketEventCheckService(glWebSocketJobQueue, glWebSocketAreaInOutEventQueue, glNotificationItemQueue, glMySQLJobQueue, glDeviceManager, glDBManager);
    glWebSocketEventCheckService->start();

    //
    // CheckPoint  service
    //
    glInspectionGroupService = new InspectionGroupService(glWebSocketJobQueue, glInspectionJobQueue, glNotificationItemQueue, glMySQLJobQueue, glDeviceManager, glDBManager);
    glInspectionGroupService->start();

    //
    // CheckPoint  service
    //
    glCoursesService = new CoursesService(glCoursesJobQueue, glCoursesAreaInOutEventQueue, glMySQLJobQueue, glDeviceManager, glDBManager);
    glCoursesService->start();

    //
    // TrackDB Service
    //
    glTrackDBService = new TrackDBService(glTrackDBJobQueue, glAnalyzeDashBoardJobQueue, glDBManager);
    glTrackDBService->start();

    //
    // AreaRollCall Service
    //
    glAreaRollCallService = new AreaRollCallService(glAreaInOutEventQueue, glCoursesAreaInOutEventQueue, glWebSocketAreaInOutEventQueue, glDeviceManager, glTrackDBJobQueue, glDBManager);
    glAreaRollCallService->start();

    //
    // Ping Device  service
    //
    glPingDeviceService = new PingDeviceService();
    glPingDeviceService->start();

    //
    // Push Notification Service
    //
    glPushNotificationService = new PushNotificationService(glMainJobQueue, glNotificationItemQueue, glMySQLJobQueue, glDeviceManager, glDBManager);
    glPushNotificationService->start();

    //
    // Sync ERP Service
    //
    glSyncERPService = new SyncERPService(glMainJobQueue, glSyncERPQueue, glMySQLJobQueue, glDBManager);
    glSyncERPService->start();

    //
    //  Footprint DB Service
    //
    glFootprintDBService = new FootprintDBService(glMySQLTSJobQueue, glDBManager);
    glFootprintDBService->start();

    //
    // Fora care Service
    //
    glForacareService = new ForacareService(glForacareServiceJobQueue);
    glForacareService->start();

    //
    // Fora care Service
    //
    glGAEService = new GAEService(glGAEJobQueue, glMySQLJobQueue, glNotificationItemQueue, glDeviceManager, glDBManager);
    glGAEService->start();

    //
    // Fora RFID Reader Service
    //
    glCARDRollCallService = new CARDRollCallService(glNotificationItemQueue, glCHTIoTJobQueue, glMQTTPubJobQueue, glCARDRollCallJobQueue, glMySQLJobQueue, glDeviceManager, glDBManager);
    glCARDRollCallService->start();

    //
    // Assist Service
    //
    glAssistService = new AssistService(glNotificationItemQueue, glAssistJobQueue, glMySQLJobQueue, glDBManager);
    glAssistService->start();

    //
    // MQ Service
    //
    glMQProcessingService = new MQProcessingService(glMQJobQueue);
    glMQProcessingService->start();

    //
    // Fire Fighting Service
    //
    glFireFightingService = new FireFightingService(glDeviceManager, glDBManager);
    glFireFightingService->start();

    //
    // Fora RFID Reader Service
    //
#ifdef ENABLE_CHT_IOT
    glCHTIoTService = new CHTIoTService(glCHTIoTJobQueue);
    glCHTIoTService->start();
#endif

#ifdef ENABLE_KIA_SERVICE

    //
    // SensorInfo Service
    //
    glSensorInfoService = new SensorInfoService(glSensorInfoJobQueue, glMySQLJobQueue, glMainJobQueue, glDeviceManager, glDBManager);
    glSensorInfoService->start();

    glKIAService = new KIAService(glMySQLJobQueue, glNotificationItemQueue, glDBManager);
    glKIAService->start();
#endif


    //
    // Simulation Devices service
    //
    // glSimulationService = new SimulationService(glCoordJobQueue, glMQTTJobQueue, glWebSocketJobQueue, glPosCalJobQueue, glNotificationItemQueue, glDeviceManager);
    // glSimulationService->start();

#ifdef ENABLE_LOCAL_COORDINATOR
    //
    // reset all cordinator
    //
    DataItem * itemT = new DataItem("coordinatorreset", 1);
    glSendOutJobQueue.add(itemT);
#endif

    // std::string message = "Tag0001";
    // message.append(" 按下緊急按鈕(UWB)");
    // NotificationItem * item = new NotificationItem(TAG_PUSHNOTIFY_FUNCTION_2, NODE_TYPE_TAG, "Tag0001", "Tag0001", "061000123456790", "SMIMSVILS 緊急求助通知", message);
    // glNotificationItemQueue.add(item);

    //NotificationItem * item = new NotificationItem("update_GAE_Topic", "smims", "SMIMSkao_USER_smims", "1,2,4,6", 1);
    //glNotificationItemQueue.add(item);

    // time_t t = StringTimeZoneDateTimeTotime_t("2019-09-17T11:59:38+08:00");
    // vilslog::printf("\n****[%s]****\n\n", time_tToString(t).c_str());


    //double results[3] = {0,100,0};
    //double result_roll[3] = {0,0,0};
    //rotate_AoA_normal_vector(0, 0, 0, 0, 90, 0, results, result_roll);

    // std::string pwd = create_MD5_encrypt_salt(std::string(ERP_MD5_KEY), std::string("1"));
    // vilslog::printf("pwd[%s]\n", pwd.c_str());

    //
    // endless loop
    //
    runloop();

    vilslog::printf("main end\n");

	return 0;
}

//
// endless loop
//

void runloop()
{
    try
    {
        wait_loop();
    }
    catch(const std::exception &exc)
    {
        // catch anything thrown within try block that derives from std::exception
        std::cerr << exc.what();
        vilslog::printf("*********************************************\n");
        vilslog::printf("main runloop() exception \n");
        vilslog::printf(exc.what());
        vilslog::printf("\n*********************************************\n");
    }
    return;
}

void wait_loop()
{
    int update_ms = 50;// 50 ms
    long long start1 = getSystemTime();
    long long updateTime = 0;
    long long dumpTime = 0;
    long long checkAlertTimeoutTime = 0;
    long long updateWebAPITime = 0;
    long long dumpTagSatusTime = 0;
    long long dumpAnchorSatusTime = 0;
    long long dumpCoordSatusTime = 0;
    long long dumpLocatorSatusTime = 0;
    long long dumpNFCandRFIDSatusTime = 0;
    long long updatePingSatusTime = 60000;
    long long updateNodeProjectIdTime = 10000; // run at first time
    long long updateSystemConfigTime = 0;
    long long sendBlackListTime = 0;
    long long sendWhiteListTime = 0;
    long long sendValidrangTime = 0;
    long long dumpMemoryUsageTime = 590000; // first 10 second dump
    long long printTagInfoTime = 0;
    long long clearBodyInfoTime = 0;
    long long sendSystemTime = 50000;

    int update_type = 0;
    int update_SystemConfig = 0;
    VILSSystemConfig * vilssysConfig = glDBManager->get_sys_config(1);

    printTagInfo();

    while (1)
    {
        //vilslog::printf("wait_loop() wait...\n");
        DataItem * mainitem = glMainJobQueue.removetimed(1000);
        if ( mainitem != NULL )
        {
            //vilslog::printf("wait_loop() item:Type[%s]\n", item->getType());
            update_type = 0;
            update_type = processData(mainitem);

            if (update_type == 1)
            {
                update_SystemConfig = 1;
                updateSystemConfigTime = 0;
                glDBManager->reload_sys_config();
                vilssysConfig = glDBManager->get_sys_config(1);
            }

            delete mainitem;
        }

        if (s_interrupted == 1)
        {
            vilslog::printf("\nmain wait_loop() s_interrupted[%d] exit loop...\n", s_interrupted);
            break;
        }
        if (s_interrupted == 2)
        {
            vilslog::printf("\nmain wait_loop() s_interrupted[%d] exit loop...\n", s_interrupted);
            break;
        }

        long long end1 = getSystemTime();
        long long difft = end1 - start1;
        //vilslog::printf("wait_loop() time[%lld ms]\n", difft);

        if ( difft >= update_ms )
        {
            updateTime += difft;
            dumpTime += difft;
            checkAlertTimeoutTime += difft;
            updateWebAPITime += difft;
            dumpTagSatusTime += difft;
            dumpAnchorSatusTime += difft;
            dumpCoordSatusTime += difft;
            dumpLocatorSatusTime += difft;
            dumpNFCandRFIDSatusTime += difft;
            updatePingSatusTime += difft;
            updateNodeProjectIdTime += difft;
            updateSystemConfigTime += difft;
            sendBlackListTime += difft;
            sendWhiteListTime += difft;
            sendValidrangTime += difft;
            dumpMemoryUsageTime += difft;
            printTagInfoTime += difft;
            clearBodyInfoTime += difft;
            sendSystemTime += difft;

            if ( updateWebAPITime > 300 ) // 300 miliseconds
            {
                update_service_WebAPI();

                updateWebAPITime = 0;
            }

            if (updateTime >= 60000) // 60 seconds
            {
                clear_anchor_alive();
                clear_tag_alive();
                clear_coord_alive();
                clear_locator_alive();
                clear_LoRaGateway_alive();
                clear_NFCDevice_alive();
                clear_RFIDDevice_alive();
                clear_EDGEDevice_alive();
                clear_NFCCard_alive();
                clear_MaingroupNFCCard_alive();
                clear_RFIDCard_alive();
                clear_unknown_NFCCard_alive();
                clear_unknown_RFIDCard_alive();
                clear_SensorDevice_alive();

                updateERPServerInfo();

                //dump_queue_status();

                updateTime = 0;
            }

            //if ( dumpTagSatusTime >= 10000 ) // every 10 seconds
            if ( dumpTagSatusTime >= 60000 ) // every 60 seconds
            {
                dump_tag_status_to_MySQL();
                dumpTagSatusTime = 0;
            }

            //if ( dumpAnchorSatusTime >= 60000 ) // every 60 seconds
            if ( dumpAnchorSatusTime >= 600000 ) // every 600 seconds
            {
                dump_anchor_status_to_MySQL();
                dumpAnchorSatusTime = 0;
            }

            //if ( dumpLocatorSatusTime >= 70000 ) // every 70 seconds
            if ( dumpLocatorSatusTime >= 600000 ) // every 600 seconds
            {
                dump_locator_status_to_MySQL();
                dumpLocatorSatusTime = 0;
            }

            //if ( dumpCoordSatusTime >= 80000 ) // every 80 seconds
            if ( dumpCoordSatusTime >= 600000 ) // every 600 seconds
            {
                dump_coord_status_to_MySQL();
                dumpCoordSatusTime = 0;
            }

            if ( dumpNFCandRFIDSatusTime >= 3600000 ) // every 1 hour
            {
                dump_NFC_RFID_status_to_MySQL();
                dumpNFCandRFIDSatusTime = 0;
            }

            if ( sendBlackListTime >= 55000 ) // every 55 seconds
            {
                send_all_BlackList_info();
                sendBlackListTime = 0;
            }
            if ( sendWhiteListTime >= 65000 ) // every 65 seconds
            {
                send_all_WhiteList_info();
                sendWhiteListTime = 0;
            }
            if ( sendValidrangTime >= 75000 ) // every 75 seconds
            {
                send_all_Validrang_info();
                sendValidrangTime = 0;
            }

            if ( dumpTime >= vilssysConfig->SendDeviceStatus_Time ) // every 10 seconds
            {
                //vilslog::printf("wait_loop() broadcast device list\n");
                //dump_device_status_to_MySQL();

                update_service_status();

                check_coordinator_alive();

                check_queue_status();

                //dump_queue_status();

                //dump_tag_area_inout_action();

                dumpTime = 0;
            }

            if (checkAlertTimeoutTime >= 3000)// every 3 seconds
            {
                check_alert_timeout();
                checkAlertTimeoutTime = 0;
            }

            if (updatePingSatusTime >= 60000)// every 60 seconds
            {
                update_ping_status();
                updatePingSatusTime = 0;
            }

            if (updateNodeProjectIdTime >= 10000)// every 10 seconds
            {
                //update_node_ProjectId();
                updateNodeProjectIdTime = 0;
            }

            if (update_SystemConfig == 1 && updateSystemConfigTime >= 2000)// every 2 seconds
            {
                glDBManager->reload_sys_config();

                int projectid = 1;
                vilssysConfig = glDBManager->get_sys_config(projectid);

                update_SystemConfig = 0;
                updateSystemConfigTime = 0;

#ifdef ENABLE_LOCAL_COORDINATOR
                glCoordCMDService->updateconfig();
#endif
                glSyncERPService->updateERPServer(projectid, vilssysConfig->ERPServer.c_str());

                glDeviceManager->reload_sys_config();

                // NotificationItem * item = new NotificationItem("check_User_Topic", projectid, NODE_TYPE_TAG, "", "", "", "", "");
                // glNotificationItemQueue.add(item);
            }

            if ( dumpMemoryUsageTime >= 600000 ) // 10 minutes
            //if ( dumpMemoryUsageTime >= 60000 ) // 1 minutes
            {
                //dump_memory_usage();
                dumpMemoryUsageTime = 0;
            }

            if ( clearBodyInfoTime >= 30000 ) // 30 seconds
            {
                clear_tag_BodyInfo();
                clear_locator_BodyInfo();
                clearBodyInfoTime = 0;
            }

            if ( printTagInfoTime >= 60000) // one minute
            {
                std::string time = currentTimeForSQL();

                if ( time.compare("10:00") == 0 )
                {
                    printTagInfo();
                }
                printTagInfoTime = 0;
            }

            if (sendSystemTime > 3600000 ) // 1 hour
            {
                sendSystemTimeToDevice();
                sendSystemTime = 0;
            }

            start1 = end1;
        }
    }

    wait_system_finished();
    vilslog::printf("main::wait_loop() stop All Service Done!\n");
}

int processData(DataItem * item)
{
    // vilslog::printf("main::processData: Type[%s]\n", item->getType());

    int update_value = 0;

    if ( strcmp(item->getType(), "updatesystemconfig") == 0 )
    {
        // re-read system config
        glDBManager->reload_sys_config();
        update_value = 1;
    }
    else
    if ( strcmp(item->getType(), "updateconfig") == 0 )
    {
        // re-read config
        //glDeviceManager->reloadConfig();
    }
    else
    if ( strcmp(item->getType(), "updateAllconfig") == 0 )
    {
        // re-read all project list
#ifdef ENABLE_LOCAL_COORDINATOR
        glThreadCenterService->updateProject();
        glThreadCoordMsgService->updateProject();
#endif
        glAnalyzeDashBoardService->updateProject();
        glAnalyzeFootprintService->updateProject();
        glAnalyzeTrackService->updateProject();
        glAreaRollCallService->updateProject();
        glCheckEmployeeActionService->updateProject();
        glCoursesService->updateProject();
        glFootprintDBService->updateProject();
        glMQTTSubscribeService->updateProject();
        glMySQLService->updateProject();
        glPosCalService->updateProject();
    }
    else
    if ( strcmp(item->getType(), "updateAlgorithmConfig") == 0 )
    {
        // re-read system config
        update_value = 1;
    }
    else
    if ( strcmp(item->getType(), "updatlocatorposition") == 0 )
    {
        int projectid = item->getProjectID();
        std::string Message = item->getMessage();
        DataItem * senditem = new DataItem("updatlocatorposition", projectid, item->getNodeId());
        senditem->setMessage(Message);
        glPosCalJobQueue.add(senditem);
    }
    else
    if ( strcmp(item->getType(), "updatesection") == 0 )
    {
        int projectid = item->getProjectID();
        DataItem * senditem = new DataItem("updatesection", projectid);
        glPosCalJobQueue.add(senditem);

        DataItem * senditem1 = new DataItem("updatesection", projectid);
        glCARDRollCallJobQueue.add(senditem1);
    }
    else
    if ( strcmp(item->getType(), "updatearea") == 0 )
    {
        int projectid = item->getProjectID();
        DataItem * senditem = new DataItem("updatearea", projectid);
        glPosCalJobQueue.add(senditem);

        glAreaRollCallService->updateAreaInfo();
    }
    else
    if ( strcmp(item->getType(), "startdumprangingresult") == 0 )
    {
        int projectid = item->getProjectID();
        DataItem * senditem = new DataItem("startdumprangingresult", projectid, item->getTimeout());
        glPosCalJobQueue.add(senditem);
    }
    else
    if ( strcmp(item->getType(), "PrintAoACirclePoint") == 0 )
    {
        int projectid = item->getProjectID();
        DataItem * senditem = new DataItem("PrintAoACirclePoint", projectid);
        glPosCalJobQueue.add(senditem);
    }
    else
    if ( strcmp(item->getType(), "startrecordsoccer") == 0 )
    {
        DataItem * senditem = new DataItem("startrecordsoccer", item->getProjectID());
        senditem->setRecorderID(item->getRecorderID());
        glPosCalJobQueue.add(senditem);
    }
    else
    if ( strcmp(item->getType(), "stoprecordsoccer") == 0 )
    {
        DataItem * senditem = new DataItem("stoprecordsoccer", item->getProjectID());
        senditem->setRecorderID(item->getRecorderID());
        glPosCalJobQueue.add(senditem);
    }
    else
    if ( strcmp(item->getType(), "startdumpcheckpoint") == 0 )
    {
        // int projectid = item->getProjectID();
        // DataItem * senditem = new DataItem("startdumpcheckpoint", projectid, item->getNodeId());
        // glCheckPointJobQueue.add(senditem);
    }
    else
    if ( strcmp(item->getType(), "updatecheckpoint") == 0 )
    {
        //vilslog::printf("main::processData() [updatecheckpoint]\n");
        // int projectid = item->getProjectID();
        // DataItem * senditem = new DataItem("updatecheckpoint", projectid);
        // glCheckPointJobQueue.add(senditem);
    }
    else
    if ( strcmp(item->getType(), "updateinspection") == 0 )
    {
        vilslog::printf("main::processData() [updateinspection]\n");
        int projectid = item->getProjectID();
        DataItem * senditem = new DataItem("updateinspection", projectid);
        glInspectionJobQueue.add(senditem);
    }
    else
    if ( strcmp(item->getType(), "updatefencealert") == 0 )
    {
        vilslog::printf("main::processData() [updatefencealert]\n");

    }
    else
    if ( strcmp(item->getType(), "updatecourse") == 0 )
    {
        int projectid = item->getProjectID();
        DataItem * senditem = new DataItem("updatecourse", projectid);
        glCoursesJobQueue.add(senditem);
    }
    else
    if ( strcmp(item->getType(), "resetm4") == 0 )
    {
#ifdef ENABLE_LOCAL_COORDINATOR
        int projectid = item->getProjectID();
        DataItem * senditem = new DataItem("coordinatorreset", projectid, item->getmacaddress(), glDeviceManager->getCoordinatorIP(projectid, item->getmacaddress()));
        glSendOutJobQueue.add(senditem);
#endif
    }
    else
    if ( strcmp(item->getType(), "resetm0") == 0 )
    {
#ifdef ENABLE_LOCAL_COORDINATOR
        int projectid = item->getProjectID();
        DataItem * senditem = new DataItem("m0reset", projectid, item->getmacaddress(), glDeviceManager->getCoordinatorIP(projectid, item->getmacaddress()));
        glSendOutJobQueue.add(senditem);
#endif
    }
    else
    if ( strcmp(item->getType(), "blacklist") == 0 )
    {
#ifdef ENABLE_LOCAL_COORDINATOR
        int projectid = item->getProjectID();
        DataItem * senditem = new DataItem("setblacklist", projectid, item->getmacaddress(), glDeviceManager->getCoordinatorIP(projectid, item->getmacaddress()), item->getValidAnchors());
        glSendOutJobQueue.add(senditem);
#endif
    }
    else
    if ( strcmp(item->getType(), "whitelist") == 0 )
    {
#ifdef ENABLE_LOCAL_COORDINATOR
        int projectid = item->getProjectID();
        DataItem * senditem = new DataItem("setwhitelist", projectid, item->getmacaddress(), glDeviceManager->getCoordinatorIP(projectid, item->getmacaddress()), item->getValidAnchors());
        glSendOutJobQueue.add(senditem);
#endif
    }
    else
    if ( strcmp(item->getType(), "setvalidrang") == 0 )
    {
#ifdef ENABLE_LOCAL_COORDINATOR
        int projectid = item->getProjectID();
        DataItem * senditem = new DataItem("setvalidrang", projectid, item->getmacaddress(), glDeviceManager->getCoordinatorIP(projectid, item->getmacaddress()), item->getRange1(), item->getRange2());
        glSendOutJobQueue.add(senditem);
#endif
    }
    else
    if ( strcmp(item->getType(), "loginfailed") == 0 )
    {
        int projectid = item->getProjectID();
        DataItem * senditem = new DataItem("startsync", projectid);
        glSyncERPQueue.add(senditem);
    }
    else
    if ( strcmp(item->getType(), "syncERPaccount") == 0 )
    {
        int projectid = item->getProjectID();
        VILSSystemConfig * vilssysConfig = glDBManager->get_sys_config(projectid);
        glSyncERPService->updateERPServer(projectid, vilssysConfig->ERPServer.c_str());
        DataItem * senditem = new DataItem("startsync", projectid);
        glSyncERPQueue.add(senditem);
    }
    else
    if ( strcmp(item->getType(), "updatePushNotify") == 0 )
    {
        int projectid = item->getProjectID();
        NotificationItem * senditem = new NotificationItem("updatePushNotify", projectid, item->getNodeId(), "0", "1");
        glNotificationItemQueue.add(senditem);
    }
    else
    if ( strcmp(item->getType(), "updatePushNotifyTriggerMessage") == 0 )
    {
        // vilslog::printf("main::updatePushNotifyTriggerMessage\n");
        int projectid = item->getProjectID();

        DataItem * senditem = new DataItem("updatePushNotifyTriggerMessage", projectid);
        glDeviceEventJobQueue.add(senditem);

        //NotificationItem * senditem = new NotificationItem("updatePushNotify", projectid, item->getNodeId());
        //glNotificationItemQueue.add(senditem);
    }
    else
    if ( strcmp(item->getType(), "SendNotifityStatus") == 0 )
    {
        // vilslog::printf("SendNotifyStatusStruct status[%d] waitkey[%s] message[%s]\n",
        //     item->m_status.status, item->m_status.waitkey.c_str(), item->m_status.message.c_str());

        glWebUDPParserService->update_PushNotifyStatus(item->m_status.projectid, item->m_status.waitkey, item->m_status.status, item->m_status.message);
    }
    else
    if ( strcmp(item->getType(), "CheckResidentMoveOut") == 0 )
    {
        CheckResidentMoveOut(item);
    }
    else
    if ( strcmp(item->getType(), "CheckEmployeeMoveOut") == 0 )
    {
        CheckEmployeeMoveOut(item);
    }
    else
    if ( strcmp(item->getType(), "sendATcmd") == 0 )
    {
        sendATcmd(item);
    }
    else
    if ( strcmp(item->getType(), "MQTTResponse") == 0 )
    {
        int projectid = item->getProjectID();
        // vilslog::printf("main::processData() MQTTResponse status[%d] waitkey[%s]\n", item->getResponseStatus(), item->getWaitkey().c_str());
        int status = item->getResponseStatus();
        std::string message = "";
        if (status == 0)
            message = "已通知LoRA gateway, 等待裝置回應.";
        if (status == 3 || status == -1)
            message = "LoRA gateway回應傳送失敗!";

        glWebUDPParserService->update_SendATcmdStatus(projectid, item->getWaitkey(), status, message);

    }
    else
    if ( strcmp(item->getType(), "MQTTMessage") == 0 )
    {
        int projectid = item->getProjectID();

        // vilslog::printf("main::processData() MQTTMessage LoRAMacaddress[%s] Message[%s]\n",
        //     item->getLoRAMacaddress(), item->getMessage().c_str());

        NodeStruct * node = glDeviceManager->get_Locator_node(projectid, item->getLoRAMacaddress());
        if (node != NULL)
        {
            //vilslog::printf("main::processData() MQTTMessage node != NULL\n");
            std::string message = remove_newline(item->getMessage());
            //vilslog::printf("main::processData() MQTTMessage message[%s]\n", message.c_str());

            glWebUDPParserService->update_MQTTMessage(projectid, node->macaddress, message);
            glDeviceManager->setLocatorAliveByLoRAmacaddress(projectid, item->getLoRAMacaddress());
        }
        else
        {
            vilslog::printf("main::processData() MQTTMessage node == NULL\n");
        }
        glDeviceManager->setRFIDDeviceAlive(projectid, item->getLoRAMacaddress());
        glDeviceManager->setNFCDeviceAlive(projectid, item->getLoRAMacaddress());

        RFIDDeviceStruct * rfidDevice = glDeviceManager->get_RFIDDeviceByLoRAmacaddress(projectid, item->getLoRAMacaddress());
        if (rfidDevice != NULL)
        {
            WorkItem * witem = new WorkItem("RFIDDevice", projectid, rfidDevice);
            glMySQLJobQueue.add(witem);
        }

        NFCDeviceStruct * nfcDevice = glDeviceManager->get_NFCDeviceByLoRAmacaddress(projectid, item->getLoRAMacaddress());
        if (nfcDevice != NULL)
        {
            WorkItem * witem = new WorkItem("NFCDevice", projectid, nfcDevice);
            glMySQLJobQueue.add(witem);
        }
    }
    else
    if ( strcmp(item->getType(), "savebodyinfoweight") == 0 )
    {
        int projectid = item->getProjectID();

        vilslog::printf("main::processData() savebodyinfoweight nodeid[%s] Weight[%s]\n",
            item->getNodeId(), item->getWeight().c_str());

        WorkItem * witem = new WorkItem("bodyinfoweight", projectid);
        witem->setNodeId(item->getNodeId());
        witem->setWeight(item->getWeight());
        glMySQLJobQueue.add(witem);
    }
    else
    if ( strcmp(item->getType(), "savebodyinfoheartbeat") == 0 )
    {
        int projectid = item->getProjectID();

        vilslog::printf("main::processData() savebodyinfoheartbeat nodeid[%s] HeartBeat[%s]\n",
            item->getNodeId(), item->getHeartBeat().c_str());

        WorkItem * witem = new WorkItem("bodyinfoheartbeat", projectid);
        witem->setNodeId(item->getNodeId());
        witem->setHeartBeat(item->getHeartBeat());
        glMySQLJobQueue.add(witem);
    }
    else
    if ( strcmp(item->getType(), "savebodyinfoall") == 0 )
    {
        int projectid = item->getProjectID();

        // vilslog::printf("main::processData() savebodyinfoall accountid[%s] DateTime[%s]\n",
        //     item->getNodeId(), item->getDateTime().c_str());

        WorkItem * witem = new WorkItem("bodyinfoall", projectid);
        witem->setNodeId(item->getNodeId());
        witem->setRecorderID(item->getRecorderID());
        witem->setWeight(item->getWeight());
        witem->setHeartBeat(item->getHeartBeat());
        witem->setBloodSugar(item->getBloodSugar());
        witem->setSystolicBloodPressure(item->getSystolicBloodPressure());
        witem->setDiastolicBloodPressure(item->getDiastolicBloodPressure());
        witem->setBodyTemperature(item->getBodyTemperature());
        witem->setBodyFat(item->getBodyFat());
        witem->setBloodOxygen(item->getBloodOxygen());
        witem->setDateTime(item->getDateTime());
        glMySQLJobQueue.add(witem);
    }
    else
    if( strcmp(item->getType(), "DumpAoACirclePoint") == 0)
    {
        int projectid = item->getProjectID();

        glPosCalService->setDumpAoACirclePoint();

        std::string strRanging1 = glPosCalService->getAoARangingCloudPoint(projectid);
        glWebUDPParserService->update_AoARangingCloudPoint(projectid, strRanging1);

        std::string strRanging2 = glPosCalService->getAoAAngleVectorCloudPoint(projectid);
        glWebUDPParserService->update_AoAAngleVectorCloudPoint(projectid, strRanging2);

        std::string strRanging3 = glPosCalService->getAoAAngleCloudPoint(projectid);
        glWebUDPParserService->update_AoAAngleCloudPoint(projectid, strRanging3);

        // vilslog::printf("main::processData() DumpAoACirclePoint projectid[%d] strRanging1.length[%d] strRanging2.length[%d]\n",
        //     projectid, (int)strRanging1.length(), (int)strRanging2.length());

    }
    else
    if ( strcmp(item->getType(), "PowerTagCalculateInfo") == 0 )
    {
        int projectid = item->getProjectID();
        std::string message = item->getMessage();

        DataItem * senditem = new DataItem("PowerTagCalculateInfo", projectid);
        senditem->setMessage(message);

        glPosCalJobQueue.add(senditem);
    }
    else
    if ( strcmp(item->getType(), "resetLoRA") == 0 )
    {
/*
        vilslog::printf("main::processData() resetLoRA[192.168.55.1]\n");

        LoRAGateway * gateway = new LoRAGateway("192.168.55.1");
        bool ret = gateway->resetLoRA();
        //bool ret = gateway->getPage();
        delete gateway;
        vilslog::printf("main::processData() resetLoRA[192.168.55.1] ret[%d]\n", ret);
        */

    }
    else
    if ( strcmp(item->getType(), "updateforacare") == 0 )
    {
        int projectid = item->getProjectID();
        WorkItem * senditem = new WorkItem("update", projectid);
        glForacareServiceJobQueue.add(senditem);

        std::string strData = glForacareService->getForacareData();

        if (strData.length() < 20)
        {
            vilslog::printf("main::processData() updateforacare not valid strData[%s]\n", strData.c_str());
        }
        else
        {
            glWebUDPParserService->update_ForacareData(projectid, strData);
        }
    }
    else
    if ( strcmp(item->getType(), "UnknownPositionTag") == 0 )
    {
        int projectid = item->getProjectID();

        // vilslog::printf("main::processData() UnknownPositionTag\n");

        std::string strData = glPosCalService->getUnknownPositionTagString(projectid);

        // vilslog::printf("main::processData() UnknownPositionTag strData[%s]\n", strData.c_str());

        glWebUDPParserService->update_UnknownPositionTagString(projectid, strData);
    }
    else
    if ( strcmp(item->getType(), "FixPosition") == 0 )
    {
        int projectid = item->getProjectID();

        //vilslog::printf("main::processData() FixPosition[%s]\n", item->getmacaddress());

        DataItem * senditem = new DataItem("FixPosition", projectid);
        senditem->setMacaddress(item->getmacaddress());
        glPosCalJobQueue.add(senditem);
    }
    else
    if ( strcmp(item->getType(), "FixDevice") == 0 )
    {
        int projectid = item->getProjectID();

        //vilslog::printf("main::processData() FixDevice[%s]\n", item->getmacaddress());

        DataItem * senditem = new DataItem("FixDevice", projectid);
        senditem->setMacaddress(item->getmacaddress());
        glPosCalJobQueue.add(senditem);
    }
    else
    if ( strcmp(item->getType(), "printAreaRollCallServiceTagAreaBuffer") == 0 )
    {
        glAreaRollCallService->print_TagAreaBuffer();
    }
    else
    if ( strcmp(item->getType(), "printInspectionGroupServiceSimulationData") == 0 )
    {
        glInspectionGroupService->print_SimulationData();
    }
    else
    if ( strcmp(item->getType(), "updatesurveyresult") == 0 )
    {
        // vilslog::printf("main::processData() updatesurveyresult\n");

        int projectid = item->getProjectID();
        DataItem * senditem = new DataItem("updatesurveyresult", projectid);
        glGAEJobQueue.add(senditem);
    }
    else
    if ( strcmp(item->getType(), "updateGAEuserinfo") == 0 )
    {
        int projectid = item->getProjectID();
        int userid = item->getUserId();

        DataItem * senditem = new DataItem("updateGAEuserinfo", projectid);
        senditem->setUserId(userid);
        glGAEJobQueue.add(senditem);
    }
    else
    if ( strcmp(item->getType(), "updateUserGroup") == 0 )
    {
    }
    else
    if ( strcmp(item->getType(), "updateUserSubGroup") == 0 )
    {
        int projectid = item->getProjectID();
        DataItem * senditem = new DataItem("updateUserSubGroup", projectid);
        glGAEJobQueue.add(senditem);
    }
    else
    if ( strcmp(item->getType(), "updateUserMainGroup") == 0 )
    {
        int projectid = item->getProjectID();
        DataItem * senditem = new DataItem("updateUserMainGroup", projectid);
        glGAEJobQueue.add(senditem);
    }
    else
    if ( strcmp(item->getType(), "updateCARDRollCall") == 0 )
    {
        int projectid = item->getProjectID();
        DataItem * senditem = new DataItem("updateCARDRollCall", projectid);
        glCARDRollCallJobQueue.add(senditem);
    }
    else
    if ( strcmp(item->getType(), "scanRFIDCard") == 0 )
    {
        int projectid = item->getProjectID();
        DataItem * senditem = new DataItem("scanRFIDCard", projectid);
        glCARDRollCallJobQueue.add(senditem);
    }
    else
    if ( strcmp(item->getType(), "scanNFCCard") == 0 )
    {
        int projectid = item->getProjectID();
        DataItem * senditem = new DataItem("scanNFCCard", projectid);
        glCARDRollCallJobQueue.add(senditem);
    }
    // else
    // if ( strcmp(item->getType(), "cardscanresult") == 0 )
    // {
    //     int projectid = item->getProjectID();
    //
    //     vilslog::printf("main::processData() cardscanresult projectid[%d] cardid[%s] LoRAMacaddress[%s] datetime[%s]\n",
    //         projectid, item->mCARDScanRecord.cardid.c_str(), item->mCARDScanRecord.LoRAMacaddress.c_str(), item->mCARDScanRecord.datetime.c_str());
    //
    // }
    else
    if ( strcmp(item->getType(), "updaterollcallrecord") == 0 )
    {
        int projectid = item->getProjectID();
        DataItem * senditem = new DataItem("updaterollcallrecord", projectid, &item->mCARDRollCallRecord);
        glCARDRollCallJobQueue.add(senditem);
    }
    else
    if ( (strcmp(item->getType(), "request_time") == 0) || (strcmp(item->getType(), "boot") == 0) || (strcmp(item->getType(), "poweron") == 0))
    {
        sendSystemTimeToDevice();
    }
    else
    if ( strcmp(item->getType(), "reloadcarduser") == 0 )
    {
        vilslog::printf("main::processData() reloadcarduser\n");

        glDeviceManager->reload_RFIDCard();
        glDeviceManager->reload_NFCCard();
    }
    else
    if ( strcmp(item->getType(), "update_rfidcard") == 0 )
    {
        glDeviceManager->reload_RFIDCard();
    }
    else
    if ( strcmp(item->getType(), "update_nfccard") == 0 )
    {
        glDeviceManager->reload_NFCCard();
    }
    else
    if ( strcmp(item->getType(), "reloadrfiddevice") == 0 )
    {
        glDeviceManager->reload_RFIDDevice();
    }
    else
    if ( strcmp(item->getType(), "reloadnfcdevice") == 0 )
    {
        glDeviceManager->reload_NFCDevice();
    }
    else
    if ( strcmp(item->getType(), "reloadsensordevice") == 0 )
    {
        glDeviceManager->reload_SensorDevice();
    }
    else
    if ( strcmp(item->getType(), "reloadedgedevice") == 0 )
    {
        glDeviceManager->reload_EDGEDevice();
    }
    else
    if ( strcmp(item->getType(), "reloadLoRaGatewayInfo") == 0 )
    {
        glDeviceManager->reload_LoRaGateway();
    }
    else
    if ( strcmp(item->getType(), "dumpanchor") == 0 )
    {
        dump_anchor_status_to_MySQL();
    }
    else
    if ( strcmp(item->getType(), "dumptag") == 0 )
    {
        dump_tag_status_to_MySQL();
    }
    else
    if ( strcmp(item->getType(), "dumpcoord") == 0 )
    {
        dump_coord_status_to_MySQL();
    }
    else
    if ( strcmp(item->getType(), "dumplocator") == 0 )
    {
        vilslog::printf ("main::processData() dumplocator\n");

        dump_locator_status_to_MySQL();
    }
    else
    if ( strcmp(item->getType(), "dumpgateway") == 0 )
    {
        dump_LoRaGateway_status_to_MySQL();
    }
    else
    if ( strcmp(item->getType(), "updateAssist") == 0 )
    {
        int projectid = item->getProjectID();
        DataItem * senditem = new DataItem("updateAssist", projectid);
        glAssistJobQueue.add(senditem);
    }
    else
    if ( strcmp(item->getType(), "HAR") == 0 )
    {
        int projectid = item->getProjectID();

        std::string message = item->getMessage();

        glDeviceManager->setTagInfoActivity(projectid, item->getmacaddress(), message);

        // vilslog::printf ("main::processData() type[%s] macAddr[%s] activity[%s]\n",
        //     item->getType(), item->getmacaddress(), message.c_str());
    }
    else
    if ( strcmp(item->getType(), "DMATEKpacket") == 0 )
    {
        DataItem * senditem = new DataItem("DMATEKpacket", item->getProjectID());
        senditem->setMessage(item->getMessage());
        glWebDMATEKJobQueue.add(senditem);
    }
    else
    if ( strcmp(item->getType(), "restart_app") == 0 )
    {
        wait_system_finished();

        vilslog::printf("main::processData() restart_app All Service Done!\n");

        exit(0);
    }
    else
    if ( strcmp(item->getType(), "send_light") == 0 )
    {
        std::string payload = item->getMessage();
        send_MQTT_cmd_wentaitek(payload);
    }

    return update_value;
}

int wait_system_finished()
{
    // WorkItem * itemts = new WorkItem("savestate", 0);
    // glMySQLTSJobQueue.add(itemts);

    // WorkItem * writem = new WorkItem("savestate", 0);
    // glAnalyzeDashBoardJobQueue.add(writem);

    // int count = 0;
    // while( !glFootprintDBService->finished() )
    // {
    //     sleep(1);
    //     count++;
    //     if (count >= 10)
    //         break;
    // }

    glFootprintDBService->savealldata();

    vilslog::printf("wait_system_finished() glAnalyzeDashBoardService->dump_cache_data() start \n");
    glAnalyzeDashBoardService->dump_cache_data();
    vilslog::printf("wait_system_finished() glAnalyzeDashBoardService->dump_cache_data() finished!\n");

    vilslog::printf("wait_system_finished() glInspectionGroupService->dump_cache_data() start \n");
    glInspectionGroupService->dump_cache_data();
    vilslog::printf("wait_system_finished() glInspectionGroupService->dump_cache_data() finished!\n");

    vilslog::printf("wait_system_finished() glAreaRollCallService->dump_cache_data() start \n");
    glAreaRollCallService->dump_cache_data();
    vilslog::printf("wait_system_finished() glAreaRollCallService->dump_cache_data() finished!\n");

    // count = 0;
    // while( !glAnalyzeDashBoardService->finished() )
    // {
    //     sleep(1);
    //     if (count >= 10)
    //         break;
    // }

    return 0;
}


int CheckResidentMoveOut(DataItem * item)
{

    //
    // remove resident from DB
    //
    int projectid = item->getProjectID();
    MySQLDBAccess * baseDb = glDBManager->getBaseDB(projectid);
    if (baseDb == NULL)
        return 0;
    std::list<UserResidentStruct *> * residentList = baseDb->getResidentList();
    vilslog::printf("main::CheckResidentMoveOut() existed  residentList->size()[%d]\n", (int)residentList->size());
    vilslog::printf("main::CheckResidentMoveOut() new item->m_stringList.size()[%d]\n", (int)item->m_stringList.size());

    for (std::list<UserResidentStruct *>::iterator resident = residentList->begin(); resident != residentList->end(); resident++)
    {
        std::string serno = (*resident)->serno;

        bool bfound = false;
        for (std::list<std::string>::iterator curserno = item->m_stringList.begin(); curserno != item->m_stringList.end(); curserno++)
        {
            if ( (*curserno).compare(serno) == 0 )
            {
                bfound = true;
                break;
            }
        }
        if (!bfound)
        {
            vilslog::printf("main::CheckResidentMoveOut() serno[%s] name[%s] roomNo[%s] Move Out !! \n",
                serno.c_str(), (*resident)->name.c_str(), (*resident)->roomNo.c_str());

            WorkItem * witem = new WorkItem("DeleteResident", projectid, (*resident));
            glMySQLJobQueue.add(witem);
        }
    }

    clear_UserResidentStruct_list(residentList);
    delete residentList;

    sleep(1);

    // find resident_serno > 0 in NodeInfo
    std::list<NodeStruct*> * nodeList = baseDb->read_nodes_assigned_serno();

    //printf("main::processData() nodeList->size()[%lu]\n", nodeList->size());

    for (std::list<NodeStruct *>::iterator node = nodeList->begin(); node != nodeList->end(); node++)
    {

        // check resident still exist
        UserResidentStruct * resident = baseDb->getResident((*node)->resident_serno.c_str());

        //printf("resident->serno[%s]\n", resident->serno.c_str());

        if ( resident->serno.compare((*node)->resident_serno) != 0 || (resident->residentType.compare("1") != 0) )
        {
            vilslog::printf("main::CheckResidentMoveOut() nodeid[%s] nodename[%s] resident_serno[%s] residentType[%s] Not existd!\n",
                (*node)->nodeid, (*node)->nodename, (*node)->resident_serno.c_str(), resident->residentType.c_str());

            if ( (*node)->nodetype == NODE_TYPE_TAG )
                glDeviceManager->setTagResident(projectid, (*node)->macaddress, "-1");
            else
            if ( (*node)->nodetype == NODE_TYPE_LOCATOR )
                glDeviceManager->setLocatorResident(projectid, (*node)->macaddress, "-1");

            WorkItem * witem = new WorkItem("updateNodeResident", projectid, (*node)->macaddress, "-1", 0);
            glMySQLJobQueue.add(witem);
        }

        delete resident;
    }

    clear_node_list(nodeList);
    delete nodeList;

    return 0;
}

int CheckEmployeeMoveOut(DataItem * item)
{
    //
    // remove resident from DB
    //
    int projectid = item->getProjectID();
    MySQLDBAccess * baseDb = glDBManager->getBaseDB(projectid);
    if (baseDb == NULL)
        return 0;

    std::list<UserEmployeeStruct *> * employeeList = baseDb->getEmployeeList();

    vilslog::printf("main::CheckEmployeeMoveOut() existed  employeeList->size()[%d]\n", (int)employeeList->size());
    vilslog::printf("main::CheckEmployeeMoveOut() new item->m_stringList.size()[%d]\n", (int)item->m_stringList.size());

    for (std::list<UserEmployeeStruct *>::iterator employee = employeeList->begin(); employee != employeeList->end(); employee++)
    {
        std::string empid = (*employee)->empid;

        bool bfound = false;
        for (std::list<std::string>::iterator curempid = item->m_stringList.begin(); curempid != item->m_stringList.end(); curempid++)
        {
            if ( (*curempid).compare(empid) == 0 )
            {
                bfound = true;
                break;
            }
        }
        if (!bfound)
        {
            vilslog::printf("main::CheckEmployeeMoveOut() empid[%s] Move Out !! \n", empid.c_str());

            WorkItem * witem = new WorkItem("DeleteEmployee", projectid, (*employee));
            glMySQLJobQueue.add(witem);
        }
    }

    clear_UserEmployeeStruct_list(employeeList);
    delete employeeList;

    return 0;
}

int sendATcmd(DataItem * item)
{
    int projectid = item->getProjectID();

    // vilslog::printf("main::sendATcmd() projectid[%d] macaddress[%s] atcmd[%s]\n", projectid, item->getmacaddress(), remove_newline(item->getATcommand()).c_str());

    NodeStruct * node = glDeviceManager->get_Locator_node_ByMacaddress(projectid, item->getmacaddress());
    if (node == NULL)
    {
        // 0:wait finish 1:success 2:failed!
        glWebUDPParserService->update_SendATcmdStatus(projectid, item->getWaitkey(), 2, "This node does not existed!");
    }
    else
    {

        std::string gwid = node->gwid;
        std::string hexString = StringToHEXString(item->getATcommand());
        int delaytime = item->getDelayTime();
        //vilslog::printf ("main::sendATcmd() hexString[%s]\n", hexString.c_str());

        std::string payload = "[{\"macAddr\":\"";
        payload.append(node->LoRAMacaddress);
        payload.append("\",\"data\":\"");
        payload.append(hexString);
        payload.append("\",\"id\":\"");
        payload.append(item->getWaitkey());
        payload.append("\",\"extra\":{\"port\":0,\"txpara\":\"22\"}}]");

        // vilslog::printf ("main::sendATcmd() payload[%s] delaytime[%d]\n", payload.c_str(), delaytime);

        // check If NBIOT gateway
        if (node->macaddress[0] == 'A')
        {
            vilslog::printf ("main::sendATcmd() macaddress[%s] gwid[%s]\n", node->macaddress, gwid.c_str());
            if (gwid.at(0) != 'N')
            {
                std::string NBIOTgwid = "N";
                NBIOTgwid.append(gwid);
                gwid = NBIOTgwid;

                vilslog::printf ("main::sendATcmd() macaddress[%s] NBIOTgwid[%s]\n", node->macaddress, NBIOTgwid.c_str());
            }
        }

        if (delaytime > 0)
        {
            if (delaytime >= 10)
                sleep(10);
            else
                sleep(delaytime);
        }

        MQTTDataItem * mqttitem = new MQTTDataItem("MQTTpublish", projectid, gwid.c_str(), payload.c_str(), (int)payload.length());
        glMQTTPubJobQueue.add(mqttitem);

//{"dataId":"vilscenter_5PN2qmWqBlQ9wQj9", "resp":"2020-01-09T08:34:06+08:00", "status":0}
//[{"channel":923600000, "sf":10, "time":"2020-01-09T16:34:15+08:00", "gwip":"192.168.99.104", "gwid":"00001c497bcc6f73", "repeater":"00000000ffffffff", "systype":0, "rssi":-37.0, "snr":25.8, "snr_max":35.8, "snr_min":20.0, "macAddr":"0000000000050165", "data":"0d0a4f4b0d0a", "frameCnt":0, "fport":1}]
    }

    return 0;
}

void update_service_status()
{
    int index = 0;

#ifdef ENABLE_LOCAL_COORDINATOR
    glWebUDPParserService->update_service_status(index++, glThreadCenterService->status(), "CenterOutgoingService", glThreadCenterService->statusString());
    glWebUDPParserService->update_service_status(index++, glThreadCoordService->status(), "CoordIncomingService", glThreadCoordService->statusString());
    glWebUDPParserService->update_service_status(index++, glCoordCMDService->status(), "CoordCommandService", glCoordCMDService->statusString());
    glWebUDPParserService->update_service_status(index++, glBroadcastDevicesService->status(), "BroadcastDevicesService", glBroadcastDevicesService->statusString());
    glWebUDPParserService->update_service_status(index++, glThreadCoordMsgService->status(), "CoordMsgParserService", glThreadCoordMsgService->statusString());

    glWebUDPParserService->update_service_status(index++, glDataParserService->status(), "DataParserService", glDataParserService->statusString());
    glWebUDPParserService->update_service_status(index++, glBroadcastIPService->status(), "BroadcastIPService", glBroadcastIPService->statusString());

#endif
    glWebUDPParserService->update_service_status(index++, glMQTTSubscribeService->status(), "MQTTSubscribeService", glMQTTSubscribeService->statusString());
    glWebUDPParserService->update_service_status(index++, glCenterWebSocketService->status(), "CenterWebSocketService", glCenterWebSocketService->statusString());
    glWebUDPParserService->update_service_status(index++, glWebUDPAPIService->status(), "WebUDPAPIService", glWebUDPAPIService->statusString());

    glWebUDPParserService->update_service_status(index++, glMQTTProcessingService->status(), "MQTTProcessingService", glMQTTProcessingService->statusString());
    glWebUDPParserService->update_service_status(index++, glMySQLService->status(), "MySQLServiceLoopThread", glMySQLService->statusString());
    glWebUDPParserService->update_service_status(index++, glDeviceEventCheckService->status(), "DeviceEventCheckService", glDeviceEventCheckService->statusString());
    glWebUDPParserService->update_service_status(index++, glPosCalService->status(), "PosCalculateService", glPosCalService->statusString());
    glWebUDPParserService->update_service_status(index++, glInspectionGroupService->status(), "InspectionGroupService", glInspectionGroupService->statusString());

    glWebUDPParserService->update_service_status(index++, glCoursesService->status(), "CoursesService", glCoursesService->statusString());
    glWebUDPParserService->update_service_status(index++, glWebUDPParserService->status(), "WebUDPParserService", glWebUDPParserService->statusString());
    glWebUDPParserService->update_service_status(index++, glAnalyzeFootprintService->status(), "AnalyzeFootprintService", glAnalyzeFootprintService->statusString());
    glWebUDPParserService->update_service_status(index++, glPingDeviceService->status(), "PingDeviceService", glPingDeviceService->statusString());
    glWebUDPParserService->update_service_status(index++, glMQTTPublishService->status(), "MQTTPublishService", glMQTTPublishService->statusString());

    glWebUDPParserService->update_service_status(index++, glAnalyzeTrackService->status(), "AnalyzeTrackService", glAnalyzeTrackService->statusString());
    glWebUDPParserService->update_service_status(index++, glAnalyzeDashBoardService->status(), "AnalyzeDashBoardService", glAnalyzeDashBoardService->statusString());
    glWebUDPParserService->update_service_status(index++, glTrackDBService->status(), "TrackDBService", glTrackDBService->statusString());
    glWebUDPParserService->update_service_status(index++, glAreaRollCallService->status(), "AreaRollCallService", glAreaRollCallService->statusString());
    glWebUDPParserService->update_service_status(index++, glWebSocketEventCheckService->status(), "WebSocketEventCheckService", glWebSocketEventCheckService->statusString());

    glWebUDPParserService->update_service_status(index++, glPushNotificationService->status(), "PushNotificationService", glPushNotificationService->statusString());
    glWebUDPParserService->update_service_status(index++, glSyncERPService->status(), "SyncERPService", glSyncERPService->statusString());
    glWebUDPParserService->update_service_status(index++, glAnalyzeNodePosOffsetService->status(), "AnalyzeNodePosOffsetService", glAnalyzeNodePosOffsetService->statusString());
    glWebUDPParserService->update_service_status(index++, glFootprintDBService->status(), "FootprintDBService", glFootprintDBService->statusString());
    glWebUDPParserService->update_service_status(index++, glCheckEmployeeActionService->status(), "CheckEmployeeActionService", glCheckEmployeeActionService->statusString());

    glWebUDPParserService->update_service_status(index++, glForacareService->status(), "ForacareService", glForacareService->statusString());
    glWebUDPParserService->update_service_status(index++, glGAEService->status(), "GAEService", glGAEService->statusString());
    glWebUDPParserService->update_service_status(index++, glCARDRollCallService->status(), "CARDRollCallService", glCARDRollCallService->statusString());
#ifdef ENABLE_CHT_IOT
    glWebUDPParserService->update_service_status(index++, glCHTIoTService->status(), "CHTIoTService", glCHTIoTService->statusString());
#endif
#ifdef ENABLE_KIA_SERVICE
    glWebUDPParserService->update_service_status(index++, glSensorInfoService->status(), "SensorInfoService", glSensorInfoService->statusString());
    glWebUDPParserService->update_service_status(index++, glKIAService->status(), "KIAService", glKIAService->statusString());
#endif

}

void update_service_WebAPI()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        VILSSystemConfig * vilssysConfig = glDBManager->get_sys_config(projectid);

        std::string strRanging = glPosCalService->getRangingString(projectid, vilssysConfig->ValidDataTime);
        glWebUDPParserService->update_LastRangingString(projectid, strRanging);

        std::string strRangingEx = glPosCalService->getRangingStringEx(projectid, vilssysConfig->ValidDataTime);
        glWebUDPParserService->update_LastRangingStringEx(projectid, strRangingEx);

        std::list<NodeStruct*> * tagNodeList = glDeviceManager->get_Tag_list(projectid);
        for (std::list<NodeStruct*>::iterator existed = tagNodeList->begin(); existed != tagNodeList->end(); existed++)
        {
            std::string strRanging = glPosCalService->getRangingDiagnosisString(projectid, (*existed)->macaddress, vilssysConfig->ValidDataTime);
            glWebUDPParserService->update_RangingDiagnosisString(projectid, (*existed)->macaddress, strRanging);
        }

        std::list<LocatorRSSIInfoStruct*> * locatorRSSIInfoList = glDeviceManager->get_LocatorRSSIInfo_list(projectid);
        for (std::list<LocatorRSSIInfoStruct*>::iterator existed = locatorRSSIInfoList->begin(); existed != locatorRSSIInfoList->end(); existed++)
        {
            std::string strRSSIInfo = prepareLocatoRSSIInfoString((*existed));

            glWebUDPParserService->update_LocatorRSSIString(projectid, (*existed)->LoRAMacaddress, strRSSIInfo);
        }

    }

    prjidList->clear();
    delete prjidList;
}

void check_coordinator_alive()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::list<NodeStruct*> * coordNodeList = glDeviceManager->get_Coordinator_list(projectid);

        struct timeval tv;
        gettimeofday(&tv, NULL);

        for (std::list<NodeStruct*>::iterator existed = coordNodeList->begin(); existed != coordNodeList->end(); existed++)
        {
            if ( (*existed)->alive == 0 )
            {
                int timeout = (int)(tv.tv_sec - (*existed)->aliveTime);
                if ( timeout > 60 )
                {
                    if ( (*existed)->pingCount > 0)
                    {
                        glDeviceManager->setCoordinatorAlive(projectid, (*existed)->macaddress);
                        continue;
                    }

                    //vilslog::printf("check_coordinator_alive() nodeid[%s] timeout[%d]\n", (*existed)->nodeid, timeout);
                    (*existed)->aliveTime = tv.tv_sec;

#ifdef ENABLE_LOCAL_COORDINATOR
                    //
                    // send reset to this Coordinator
                    //
                    DataItem * itemT = new DataItem("coordinatorreset", projectid, (*existed)->macaddress, (*existed)->IP);
                    glSendOutJobQueue.add(itemT);
#endif
                }
            }
        }
    }

    prjidList->clear();
    delete prjidList;
}

void check_queue_status()
{
    int queue_size = glMySQLService->getQueueSize();

#ifdef ENABLE_LOCAL_COORDINATOR
    queue_size += (int)glUDPJobQueue.size();
    queue_size += (int)glCoordJobQueue.size();
    queue_size += (int)glSendOutJobQueue.size();
#endif
    queue_size += (int)glMySQLTSJobQueue.size();
    queue_size += (int)glTrackDBJobQueue.size();

    queue_size += (int)glAnalyzeDashBoardJobQueue.size();
    queue_size += (int)glUDPAPIJobQueue.size();
    queue_size += (int)glPosCalJobQueue.size();
    queue_size += (int)glInspectionJobQueue.size();

    queue_size += (int)glCoursesJobQueue.size();
    queue_size += (int)glMainJobQueue.size();
    queue_size += (int)glMQTTJobQueue.size();
    queue_size += (int)glMQTTPubJobQueue.size();
    queue_size += (int)glWebSocketJobQueue.size();

    queue_size += (int)glAreaInOutEventQueue.size();
    queue_size += (int)glCoursesAreaInOutEventQueue.size();
    queue_size += (int)glWebSocketAreaInOutEventQueue.size();
    queue_size += (int)glSyncERPQueue.size();
    queue_size += (int)glNotificationItemQueue.size();

    queue_size += (int)glNodePositionQueue.size();
    queue_size += (int)glDeviceEventJobQueue.size();
    queue_size += (int)glForacareServiceJobQueue.size();
    queue_size += (int)glCARDRollCallJobQueue.size();
    queue_size += (int)glCHTIoTJobQueue.size();

    queue_size += (int)glGAEJobQueue.size();
    queue_size += (int)glAssistJobQueue.size();
    queue_size += (int)glSensorInfoJobQueue.size();
    queue_size += (int)glMQJobQueue.size();

    if ( queue_size >= 500 )
    {
        vilslog::printf("check_queue_status() queue_size[%d] glMySQLService->mProcessingType[%s]\n", queue_size, glMySQLService->mProcessingType.c_str());
        dump_queue_status();
    }
}

//
// clear Node alive
//
void clear_anchor_alive()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::list<NodeStruct*> * anchorNodeList = glDeviceManager->get_Anchor_list(projectid);
        // vilslog::printf("clear_anchor_alive() anchorNodeList->size[%d]\n", (int)anchorNodeList->size());
        clear_node_alive(anchorNodeList, NODE_TYPE_ANCHOR);
    }

    prjidList->clear();
    delete prjidList;
}

void clear_tag_alive()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::list<NodeStruct*> * tagNodeList = glDeviceManager->get_Tag_list(projectid);
        // vilslog::printf("clear_tag_alive() tagNodeList->size[%d]\n", (int)tagNodeList->size());
        clear_node_alive(tagNodeList, NODE_TYPE_TAG);
    }

    prjidList->clear();
    delete prjidList;
}

void clear_coord_alive()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::list<NodeStruct*> * coordNodeList = glDeviceManager->get_Coordinator_list(projectid);
        // vilslog::printf("clear_coord_alive() coordNodeList->size[%d]\n", (int)coordNodeList->size());
        clear_node_alive(coordNodeList, NODE_TYPE_COORD);
    }

    prjidList->clear();
    delete prjidList;
}

void clear_locator_alive()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::list<NodeStruct*> * locatorNodeList = glDeviceManager->get_Locator_list(projectid);
        // vilslog::printf("clear_locator_alive() locatorNodeList->size[%d]\n", (int)locatorNodeList->size());
        clear_node_alive(locatorNodeList, NODE_TYPE_LOCATOR);
    }

    prjidList->clear();
    delete prjidList;
}

void clear_node_alive(std::list<NodeStruct*> * nodeList, int nodetype)
{
    time_t currentTime = getCurrentTimeInSec();

    for (std::list<NodeStruct*>::iterator existed = nodeList->begin(); existed != nodeList->end(); existed++)
    {
        if ( (*existed)->alive > 0 )
        {
            //
            // send to server
            //
            double voltage = atof((*existed)->voltage);
            double temperature = atof((*existed)->temperature);

            // MySQL workitem
            if (nodetype == NODE_TYPE_TAG)
            {
                WorkItem * item = new WorkItem("alive", (*existed)->projectid, (*existed)->nodeid, nodetype, voltage, temperature, (*existed)->alive);
                glMySQLJobQueue.add(item);

                WorkItem * itemts = new WorkItem("alive", (*existed)->projectid, (*existed)->nodeid, nodetype, voltage, temperature, (*existed)->alive);
                glMySQLTSJobQueue.add(itemts);
            }

            time_t diffTime = currentTime - (*existed)->aliveTime;

            if (nodetype == NODE_TYPE_LOCATOR )
            {
                time_t poweroffTime = currentTime - (*existed)->poweroffTimestamp;

                if ( poweroffTime < diffTime )
                {
                    (*existed)->alive = 0;
                    vilslog::printf ("VILS_RTLS::clear_node_alive() [%s] poweroffTime[%d] < diffTime[%d]\n",
                        (*existed)->LoRAMacaddress, poweroffTime, diffTime);
                }
                else
                //if( diffTime <= 660 ) // 11 mins
                if( diffTime <= 2460 ) // 41 mins
                {
                    (*existed)->alive = 1;
                }
                else
                {
                    (*existed)->alive = 0;
                    vilslog::printf ("VILS_RTLS::clear_node_alive() [%s]\n", (*existed)->LoRAMacaddress);
                }
            }
            else
            {
                if( diffTime <= 60 )
                {
                    (*existed)->alive = 1;
                }
                else
                {
                    (*existed)->alive = 0;
                }
            }
        }
    }
}

void clear_LoRaGateway_alive()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::list<LoRaGatewayInfoStruct*> * gatewayList = glDeviceManager->get_LoRaGateway_list(projectid);
        time_t currentTime = getCurrentTimeInSec();

        for (std::list<LoRaGatewayInfoStruct*>::iterator gateway = gatewayList->begin(); gateway != gatewayList->end(); gateway++)
        {
            if ( (*gateway)->alive > 0 )
            {
                time_t diffTime = currentTime - (*gateway)->aliveTime;
                if( diffTime <= 60 )
                {
                    (*gateway)->alive = 1;
                }
                else
                {
                    (*gateway)->alive = 0;
                }
            }

            (*gateway)->MQTTalive = 0;
        }
    }

    prjidList->clear();
    delete prjidList;
}

void clear_NFCDevice_alive()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::list<NFCDeviceStruct*> * nfcdeviceList = glDeviceManager->get_NFCDevice_list(projectid);
        time_t currentTime = getCurrentTimeInSec();

        for (std::list<NFCDeviceStruct*>::iterator nfcdevice = nfcdeviceList->begin(); nfcdevice != nfcdeviceList->end(); nfcdevice++)
        {
            if ( (*nfcdevice)->alive > 0 )
            {
                time_t diffTime = currentTime - (*nfcdevice)->aliveTime;
                //if( diffTime <= 3600 ) // one hour
                if( diffTime <= 2460 ) // 41 mins
                {
                    (*nfcdevice)->alive = 1;
                }
                else
                {
                    (*nfcdevice)->alive = 0;
                }
            }
        }
    }

    prjidList->clear();
    delete prjidList;
}

void clear_RFIDDevice_alive()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::list<RFIDDeviceStruct*> * rfiddeviceList = glDeviceManager->get_RFIDDevice_list(projectid);
        time_t currentTime = getCurrentTimeInSec();

        for (std::list<RFIDDeviceStruct*>::iterator rfiddevice = rfiddeviceList->begin(); rfiddevice != rfiddeviceList->end(); rfiddevice++)
        {
            if ( (*rfiddevice)->alive > 0 )
            {
                time_t diffTime = currentTime - (*rfiddevice)->aliveTime;
                if( diffTime <= 3600 ) // one hour
                {
                    (*rfiddevice)->alive = 1;
                }
                else
                {
                    (*rfiddevice)->alive = 0;
                }
            }
        }
    }

    prjidList->clear();
    delete prjidList;

}

void clear_EDGEDevice_alive()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::list<EDGEDeviceStruct*> * edgedeviceList = glDeviceManager->get_EDGEDevice_list(projectid);
        time_t currentTime = getCurrentTimeInSec();

        for (std::list<EDGEDeviceStruct*>::iterator edgedevice = edgedeviceList->begin(); edgedevice != edgedeviceList->end(); edgedevice++)
        {
            if ( (*edgedevice)->alive > 0 )
            {
                time_t diffTime = currentTime - (*edgedevice)->aliveTime;
                if( diffTime <= 3600 ) // one hour
                {
                    (*edgedevice)->alive = 1;
                }
                else
                {
                    (*edgedevice)->alive = 0;
                }
            }
        }
    }

    prjidList->clear();
    delete prjidList;
}

void clear_NFCCard_alive()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::list<NFCCardStruct*> * nfccardList = glDeviceManager->get_NFCCard_list(projectid);
        time_t currentTime = getCurrentTimeInSec();

        for (std::list<NFCCardStruct*>::iterator nfccard = nfccardList->begin(); nfccard != nfccardList->end(); nfccard++)
        {
            if ( (*nfccard)->alive > 0 )
            {
                time_t diffTime = currentTime - (*nfccard)->aliveTime;
                if( diffTime <= 60 )
                {
                    (*nfccard)->alive = 1;
                }
                else
                {
                    (*nfccard)->alive = 0;

                    WorkItem * witem = new WorkItem("NFCCardStruct", projectid, (*nfccard));
                    glMySQLJobQueue.add(witem);
                }
            }
        }
    }

    prjidList->clear();
    delete prjidList;

}

void clear_MaingroupNFCCard_alive()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::list<NFCCardStruct*> * nfccardList = glDeviceManager->get_MaingroupNFCCard_list(projectid);
        time_t currentTime = getCurrentTimeInSec();

        for (std::list<NFCCardStruct*>::iterator nfccard = nfccardList->begin(); nfccard != nfccardList->end(); nfccard++)
        {
            if ( (*nfccard)->alive > 0 )
            {
                time_t diffTime = currentTime - (*nfccard)->aliveTime;
                if( diffTime <= 60 )
                {
                    (*nfccard)->alive = 1;
                }
                else
                {
                    (*nfccard)->alive = 0;

                    WorkItem * witem = new WorkItem("NFCCardStruct", projectid, (*nfccard));
                    glMySQLJobQueue.add(witem);
                }
            }
        }
    }

    prjidList->clear();
    delete prjidList;

}

void clear_RFIDCard_alive()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::list<RFIDCardStruct*> * rfidcardList = glDeviceManager->get_RFIDCard_list(projectid);
        time_t currentTime = getCurrentTimeInSec();

        for (std::list<RFIDCardStruct*>::iterator rfidcard = rfidcardList->begin(); rfidcard != rfidcardList->end(); rfidcard++)
        {
            if ( (*rfidcard)->alive > 0 )
            {
                time_t diffTime = currentTime - (*rfidcard)->aliveTime;
                if( diffTime <= 60 )
                {
                    (*rfidcard)->alive = 1;
                }
                else
                {
                    (*rfidcard)->alive = 0;

                    WorkItem * witem = new WorkItem("RFIDCardStruct", projectid, (*rfidcard));
                    glMySQLJobQueue.add(witem);
                }
            }
        }
    }

    prjidList->clear();
    delete prjidList;
}

void clear_unknown_NFCCard_alive()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        WorkItem * witem = new WorkItem("clearUnknownNFCCardAlive", projectid);
        glMySQLJobQueue.add(witem);
    }
}

void clear_unknown_RFIDCard_alive()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        WorkItem * witem = new WorkItem("clearUnknownRFIDCardAlive", projectid);
        glMySQLJobQueue.add(witem);
    }
}

void clear_SensorDevice_alive()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::list<SensorDeviceStruct*> * sensordeviceList = glDeviceManager->get_SensorDevice_list(projectid);
        time_t currentTime = getCurrentTimeInSec();

        for (std::list<SensorDeviceStruct*>::iterator sensordevice = sensordeviceList->begin(); sensordevice != sensordeviceList->end(); sensordevice++)
        {
            if ( (*sensordevice)->alive > 0 )
            {
                time_t diffTime = currentTime - (*sensordevice)->aliveTime;
                if( diffTime <= 60 ) // one min
                {
                    (*sensordevice)->alive = 1;
                }
                else
                {
                    (*sensordevice)->alive = 0;
                }
            }
            WorkItem * witem = new WorkItem("sensordevice", projectid, (*sensordevice));
            glMySQLJobQueue.add(witem);
        }
    }

    prjidList->clear();
    delete prjidList;
}

void clear_tag_BodyInfo()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::list<NodeStruct*> * tagNodeList = glDeviceManager->get_Tag_list(projectid);
        clear_node_BodyInfo(tagNodeList);
    }

    prjidList->clear();
    delete prjidList;
}

void clear_locator_BodyInfo()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::list<NodeStruct*> * locatorNodeList = glDeviceManager->get_Locator_list(projectid);
        clear_node_BodyInfo(locatorNodeList);
    }

    prjidList->clear();
    delete prjidList;
}

void clear_node_BodyInfo(std::list<NodeStruct*> * nodeList)
{
    time_t currentTime = getCurrentTimeInSec();

    for (std::list<NodeStruct*>::iterator existed = nodeList->begin(); existed != nodeList->end(); existed++)
    {
        time_t diffTimeW = currentTime - (*existed)->updateWeightTimestamp;
        if( diffTimeW >= 60 )
        {
            (*existed)->weight = -1;
        }
        time_t diffTimeH = currentTime - (*existed)->updateHeartBeatTimestamp;
        if( diffTimeH >= 60 )
        {
            (*existed)->heartbeat = -1;
        }

        if ( (*existed)->weight == -1 && (*existed)->heartbeat == -1 )
        {
            (*existed)->rangingTargetid = "";
        }
    }
}

void updateERPServerInfo()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        VILSSystemConfig * vilssysConfig = glDBManager->get_sys_config(projectid);
        if (vilssysConfig != NULL)
        {
            glSyncERPService->updateERPServer(projectid, vilssysConfig->ERPServer.c_str());
        }
    }

    prjidList->clear();
    delete prjidList;
}

int check_alert_timeout()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::list<NodeStruct*> * tagNodeList = glDeviceManager->get_Tag_list(projectid);
        VILSSystemConfig * vilssysConfig = glDBManager->get_sys_config(projectid);

        for (std::list<NodeStruct*>::iterator node = tagNodeList->begin(); node != tagNodeList->end(); node++)
        {
            //
            // alert timeout
            //
            time_t currentTime = getCurrentTimeInMilliSec();
            time_t diffTime = currentTime -  (*node)->alertIssuTime;
            //vilslog::printf("check_alert_timeout() alert timeout  diffTime(%.02f) currentTime(%.02f)\n", (float)diffTime, (float)currentTime);
            if ( (float)diffTime > (float)vilssysConfig->TagAlertTimeout)
            {
                // if ( (*node)->alert > 0 || (*node)->alert0D > 0 )
                // {
                //     vilslog::printf("check_alert_timeout() alert timeout nodeid[%s] diffTime(%.02f)\n", (*node)->nodeid, (float)diffTime);
                // }
                // vilslog::printf("check_alert_timeout() alert timeout nodeid[%s] diffTime(%.02f)\n", (*node)->nodeid, (float)diffTime);
                (*node)->alert = 0;
                (*node)->alert0D = 0;
            }
            // else
            // {
            //     if ( (*node)->alert > 0 || (*node)->alert0D > 0 )
            //     {
            //         vilslog::printf("check_alert_timeout() nodeid[%s] diffTime(%.02f)\n", (*node)->nodeid, (float)diffTime);
            //     }
            // }
        }
    }

    prjidList->clear();
    delete prjidList;
    return 0;
}

void update_ping_status()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);

        std::list<NodeStruct*> * coordNodeList = glDeviceManager->get_Coordinator_list(projectid);
        std::list<LoRaGatewayInfoStruct*> * gatewayList = glDeviceManager->get_LoRaGateway_list(projectid);

        for (std::list<NodeStruct*>::iterator node = coordNodeList->begin(); node != coordNodeList->end(); node++)
        {
            if( strlen((*node)->IP) > 0 )
            {
                glPingDeviceService->addPingAddress((*node)->IP, (*node)->alive);
                int pingCount = glPingDeviceService->getPingCount((*node)->IP, false);// don't clear pingcount
                (*node)->pingCount = pingCount;
                if (pingCount > 0)
                    glDeviceManager->setCoordinatorAlive(projectid, (*node)->macaddress);
            }

            //vilslog::printf("update_ping_status() nodeid[%s]  pingCount[%d]\n", (*node)->nodeid, (*node)->pingCount);
        }

        int count = 0;
        for (std::list<LoRaGatewayInfoStruct*>::iterator gateway = gatewayList->begin(); gateway != gatewayList->end(); gateway++)
        {
            if( (*gateway)->gwip.length() > 0 )
            {
                glPingDeviceService->addPingAddress((*gateway)->gwip.c_str(), (*gateway)->alive);
                int pingCount = glPingDeviceService->getPingCount((*gateway)->gwip.c_str(), false);
                (*gateway)->pingCount = pingCount;
                if (pingCount > 0)
                    glDeviceManager->setLoRaGatewayAlive(projectid, (*gateway));
            }

            //
            // send MQTT publish to gateway
            //

            if ((*gateway)->gwid.length() == 0)
            {
                vilslog::printf("update_ping_status::run() (*gateway)->gwid.length() == 0)\n");
                continue;
            }

            // test publish
            //std::string payload = "[{\"macAddr\":\"0000000012345678\",\"data\":\"011eff017b81750736772e\",\"id\":\"ikkitest560\",\"extra\":{\"port\":2,\"txpara\":\"26\"}}]";
            // std::string payload = "[{\"macAddr\":\"0000000012345678\",\"data\":\"011eff017b81750736772e\",\"id\":\"ikkitest";
            // payload.append(currentDateTime());
            // payload.append("\",\"extra\":{\"port\":2,\"txpara\":\"26\"}}]");
            //
            // //rc = client.publish("GIOT-GW/DL/00001c497bcc6f73", (void*)payload.c_str(), payload.length(), MQTT::QOS0, false);
            // //rc = client.publish("SMIMS/VILS/TAG", (void*)payload.c_str(), payload.length(), MQTT::QOS0, false);
            // //vilslog::printf("MQTTSubscribeService::run() client.publish rc[%d]\n", rc);

            char cnt[10];
            snprintf(cnt, 10, "%d", count);

            std::string payload = "[{\"macAddr\":\"0000000012345678\",\"data\":\"011eff017b81750736772e\",\"id\":\"vilscenter";
            payload.append(currentDateTime());
            payload.append(cnt);
            payload.append("\",\"extra\":{\"port\":2,\"txpara\":\"22\"}}]");
            MQTTDataItem * mqttitem = new MQTTDataItem("MQTTpublish", projectid, (*gateway)->gwid.c_str(), payload.c_str(), (int)payload.length());
            glMQTTPubJobQueue.add(mqttitem);


            count++;
        }
    }

    //
    // clear pingcount
    //
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);

        std::list<NodeStruct*> * coordNodeList = glDeviceManager->get_Coordinator_list(projectid);
        std::list<LoRaGatewayInfoStruct*> * gatewayList = glDeviceManager->get_LoRaGateway_list(projectid);

        for (std::list<NodeStruct*>::iterator node = coordNodeList->begin(); node != coordNodeList->end(); node++)
        {
            if( strlen((*node)->IP) > 0 )
            {
                glPingDeviceService->getPingCount((*node)->IP, true);// clear pingcount
            }

            //vilslog::printf("update_ping_status() nodeid[%s]  pingCount[%d]\n", (*node)->nodeid, (*node)->pingCount);
        }

        for (std::list<LoRaGatewayInfoStruct*>::iterator gateway = gatewayList->begin(); gateway != gatewayList->end(); gateway++)
        {
            if( (*gateway)->gwip.length() > 0 )
            {
                glPingDeviceService->getPingCount((*gateway)->gwip.c_str(), true);// clear pingcount
            }
        }
    }

    prjidList->clear();
    delete prjidList;
}

// void update_node_ProjectId()
// {
//     MySQLDBAccess * baseDb = glDBManager->getBaseDB(1);
//
//     std::list<VILSConfig*> * allConfigList = baseDb->read_all_config();
//
//     std::list<NodeStruct*> * anchorNodeList = glDeviceManager->get_Anchor_list(1);
//     std::list<NodeStruct*> * tagNodeList = glDeviceManager->get_Tag_list(1);
//     std::list<NodeStruct*> * coordNodeList = glDeviceManager->get_Coordinator_list(1);
//     std::list<NodeStruct*> * locatorNodeList = glDeviceManager->get_Locator_list(1);
//
//     resolve_nodelist_projectid(anchorNodeList, allConfigList);
//     resolve_nodelist_projectid(tagNodeList, allConfigList);
//     resolve_nodelist_projectid(coordNodeList, allConfigList);
//     resolve_nodelist_projectid(locatorNodeList, allConfigList);
//
//     allConfigList->clear();
//     delete allConfigList;
// }

// void resolve_nodelist_projectid(std::list<NodeStruct*> * nodeList, std::list<VILSConfig*> * allConfigList)
// {
//     for (std::list<NodeStruct*>::iterator node = nodeList->begin(); node != nodeList->end(); node++)
//     {
//         int project_id = resolveProjectId(allConfigList, (*node)->buildingfloor);
//         (*node)->projectid = project_id;
//         //
//         // vilslog::printf("resolve_nodelist_projectid() nodeid[%s] buildingfloor[%d] projectid[%d]\n",
//         //     (*node)->nodeid, (*node)->buildingfloor, (*node)->projectid);
//     }
// }
//
// int resolveProjectId(std::list<VILSConfig*> * allConfigList, int floor)
// {
//     int project_id = 1;
//
//     for (std::list<VILSConfig*>::iterator config = allConfigList->begin(); config != allConfigList->end(); config++)
//     {
//         if( (*config)->BuildingFloor == floor )
//         {
//             project_id = (*config)->projectid;
//             break;
//         }
//     }
//
//     return project_id;
// }

bool dump_device_status_to_MySQL()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::list<NodeStruct*> * coordNodeList = glDeviceManager->get_Coordinator_list(projectid);
        std::list<NodeStruct*> * anchorNodeList = glDeviceManager->get_Anchor_list(projectid);
        std::list<NodeStruct*> * tagNodeList = glDeviceManager->get_Tag_list(projectid);
        std::list<NodeStruct*> * locatorNodeList = glDeviceManager->get_Locator_list(projectid);

        save_nodelist_info(projectid, coordNodeList);
        save_nodelist_info(projectid, anchorNodeList);
        save_nodelist_info(projectid, tagNodeList);
        save_nodelist_info(projectid, locatorNodeList);
    }

    prjidList->clear();
    delete prjidList;
    return true;
}

bool dump_tag_status_to_MySQL()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::list<NodeStruct*> * tagNodeList = glDeviceManager->get_Tag_list(projectid);
        save_nodelist_info(projectid, tagNodeList);
    }

    prjidList->clear();
    delete prjidList;
    return true;
}

bool dump_anchor_status_to_MySQL()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::list<NodeStruct*> * anchorNodeList = glDeviceManager->get_Anchor_list(projectid);
        save_nodelist_info(projectid, anchorNodeList);
    }

    prjidList->clear();
    delete prjidList;
    return true;
}

bool dump_coord_status_to_MySQL()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::list<NodeStruct*> * coordNodeList = glDeviceManager->get_Coordinator_list(projectid);
        save_nodelist_info(projectid, coordNodeList);
    }

    prjidList->clear();
    delete prjidList;
    return true;
}

bool dump_locator_status_to_MySQL()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();

    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);

        std::list<NodeStruct*> * locatorNodeList = glDeviceManager->get_Locator_list(projectid);
        save_nodelist_info(projectid, locatorNodeList);

        std::list<AoANodeStruct*> * aoaNodeList = glDeviceManager->get_AoA_list(projectid);
        save_aoanodelist_info(projectid, aoaNodeList);
    }

    prjidList->clear();
    delete prjidList;
    return true;
}

bool dump_NFC_RFID_status_to_MySQL()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();

    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::list<NFCDeviceStruct*> * nfcDeviceList = glDeviceManager->get_NFCDevice_list(projectid);
        for (std::list<NFCDeviceStruct*>::iterator device = nfcDeviceList->begin(); device != nfcDeviceList->end(); device++)
        {
            WorkItem * witem = new WorkItem("NFCDevice", projectid, (*device));
            glMySQLJobQueue.add(witem);
        }

        std::list<RFIDDeviceStruct*> * rfiddeviceList = glDeviceManager->get_RFIDDevice_list(projectid);
        for (std::list<RFIDDeviceStruct*>::iterator rfiddevice = rfiddeviceList->begin(); rfiddevice != rfiddeviceList->end(); rfiddevice++)
        {
            WorkItem * witem = new WorkItem("RFIDDevice", projectid, (*rfiddevice));
            glMySQLJobQueue.add(witem);
        }

        std::list<RFIDCardStruct*> * rfidcardList = glDeviceManager->get_RFIDCard_list(projectid);
        for (std::list<RFIDCardStruct*>::iterator rfidcard = rfidcardList->begin(); rfidcard != rfidcardList->end(); rfidcard++)
        {
            WorkItem * witem = new WorkItem("RFIDCardStruct", projectid, (*rfidcard));
            glMySQLJobQueue.add(witem);
        }

        std::list<NFCCardStruct*> * nfccardList = glDeviceManager->get_NFCCard_list(projectid);
        for (std::list<NFCCardStruct*>::iterator nfccard = nfccardList->begin(); nfccard != nfccardList->end(); nfccard++)
        {
            WorkItem * witem = new WorkItem("NFCCardStruct", projectid, (*nfccard));
            glMySQLJobQueue.add(witem);
        }
    }

    prjidList->clear();
    delete prjidList;

    return true;
}

bool dump_LoRaGateway_status_to_MySQL()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::list<LoRaGatewayInfoStruct*> * gatewayList = glDeviceManager->get_LoRaGateway_list(projectid);

        for (std::list<LoRaGatewayInfoStruct*>::iterator gateway = gatewayList->begin(); gateway != gatewayList->end(); gateway++)
        {
            WorkItem * witem = new WorkItem("loragateway", projectid, (*gateway));
            glMySQLJobQueue.add(witem);
        }
    }

    prjidList->clear();
    delete prjidList;

    return true;
}

void save_nodelist_info(int projectid, std::list<NodeStruct*> * nodeList)
{
    for (std::list<NodeStruct*>::iterator node = nodeList->begin(); node != nodeList->end(); node++)
    {
        WorkItem * item = new WorkItem("updateFullInfo", projectid, (*node));
        glMySQLJobQueue.add(item);
    }
}

void save_aoanodelist_info(int projectid, std::list<AoANodeStruct*> * nodeList)
{
    for (std::list<AoANodeStruct*>::iterator node = nodeList->begin(); node != nodeList->end(); node++)
    {
        WorkItem * item = new WorkItem("updateAoANodeInfo", projectid, (*node));
        glMySQLJobQueue.add(item);
    }
}

// void dump_tag_area_inout_action()
// {
//     glDeviceManager->dump_tag_area_inout_action();
// }

void dump_queue_status()
{
    if (glMySQLService->getQueueSize() > 0)
        vilslog::printf("dump_queue_status() glMySQLJobQueue.size()=%d\n", glMySQLService->getQueueSize());
    // vilslog::printf("dump_queue_status() glMySQLJobQueue.size()=%d\n", (int)glMySQLJobQueue.size());

#ifdef ENABLE_LOCAL_COORDINATOR
    if ((int)glUDPJobQueue.size() > 0)
        vilslog::printf("dump_queue_status() glUDPJobQueue.size()=%d\n", (int)glUDPJobQueue.size());
    if ((int)glCoordJobQueue.size() > 0)
        vilslog::printf("dump_queue_status() glCoordJobQueue()=%d\n", (int)glCoordJobQueue.size());
    if ((int)glSendOutJobQueue.size() > 0)
        vilslog::printf("dump_queue_status() glSendOutJobQueue.size()=%d\n", (int)glSendOutJobQueue.size());
#endif

    if ((int)glMySQLTSJobQueue.size() > 0)
        vilslog::printf("dump_queue_status() glMySQLTSJobQueue.size()=%d\n", (int)glMySQLTSJobQueue.size());
    if ((int)glTrackDBJobQueue.size() > 0)
        vilslog::printf("dump_queue_status() glTrackDBJobQueue.size()=%d\n", (int)glTrackDBJobQueue.size());
    if ((int)glAnalyzeDashBoardJobQueue.size() > 0)
        vilslog::printf("dump_queue_status() glAnalyzeDashBoardJobQueue.size()=%d\n", (int)glAnalyzeDashBoardJobQueue.size());
    if ((int)glUDPAPIJobQueue.size() > 0)
        vilslog::printf("dump_queue_status() glUDPAPIJobQueue.size()=%d\n", (int)glUDPAPIJobQueue.size());

    if ((int)glPosCalJobQueue.size() > 0)
        vilslog::printf("dump_queue_status() glPosCalJobQueue.size()=%d\n", (int)glPosCalJobQueue.size());
    if ((int)glInspectionJobQueue.size() > 0)
        vilslog::printf("dump_queue_status() glInspectionJobQueue.size()=%d\n", (int)glInspectionJobQueue.size());
    if ((int)glCoursesJobQueue.size() > 0)
        vilslog::printf("dump_queue_status() glCoursesJobQueue.size()=%d\n", (int)glCoursesJobQueue.size());
    if ((int)glMainJobQueue.size() > 0)
        vilslog::printf("dump_queue_status() glMainJobQueue.size()=%d\n", (int)glMainJobQueue.size());

    if ((int)glMQTTJobQueue.size() > 0)
        vilslog::printf("dump_queue_status() glMQTTJobQueue.size()=%d\n", (int)glMQTTJobQueue.size());
    if ((int)glMQTTPubJobQueue.size() > 0)
        vilslog::printf("dump_queue_status() glMQTTPubJobQueue.size()=%d\n", (int)glMQTTPubJobQueue.size());
    if ((int)glWebSocketJobQueue.size() > 0)
        vilslog::printf("dump_queue_status() glWebSocketJobQueue.size()=%d\n", (int)glWebSocketJobQueue.size());
    if ((int)glAreaInOutEventQueue.size() > 0)
        vilslog::printf("dump_queue_status() glAreaInOutEventQueue.size()=%d\n", (int)glAreaInOutEventQueue.size());
    if ((int)glCoursesAreaInOutEventQueue.size() > 0)
        vilslog::printf("dump_queue_status() glCoursesAreaInOutEventQueue.size()=%d\n", (int)glCoursesAreaInOutEventQueue.size());

    if ((int)glWebSocketAreaInOutEventQueue.size() > 0)
        vilslog::printf("dump_queue_status() glWebSocketAreaInOutEventQueue.size()=%d\n", (int)glWebSocketAreaInOutEventQueue.size());
    if ((int)glSyncERPQueue.size() > 0)
        vilslog::printf("dump_queue_status() glSyncERPQueue.size()=%d\n", (int)glSyncERPQueue.size());
    if ((int)glNotificationItemQueue.size() > 0)
        vilslog::printf("dump_queue_status() glNotificationItemQueue.size()=%d\n", (int)glNotificationItemQueue.size());
    if ((int)glNodePositionQueue.size() > 0)
        vilslog::printf("dump_queue_status() glNodePositionQueue.size()=%d\n", (int)glNodePositionQueue.size());
    if ((int)glDeviceEventJobQueue.size() > 0)
        vilslog::printf("dump_queue_status() glDeviceEventJobQueue.size()=%d\n", (int)glDeviceEventJobQueue.size());

    if ((int)glForacareServiceJobQueue.size() > 0)
        vilslog::printf("dump_queue_status() glForacareServiceJobQueue.size()=%d\n", (int)glForacareServiceJobQueue.size());
    if ((int)glCARDRollCallJobQueue.size() > 0)
        vilslog::printf("dump_queue_status() glCARDRollCallJobQueue.size()=%d\n", (int)glCARDRollCallJobQueue.size());
    if ((int)glCHTIoTJobQueue.size() > 0)
        vilslog::printf("dump_queue_status() glCHTIoTJobQueue.size()=%d\n", (int)glCHTIoTJobQueue.size());
    if ((int)glGAEJobQueue.size() > 0)
        vilslog::printf("dump_queue_status() glGAEJobQueue.size()=%d\n", (int)glGAEJobQueue.size());
    if ((int)glAssistJobQueue.size() > 0)
        vilslog::printf("dump_queue_status() glAssistJobQueue.size()=%d\n", (int)glAssistJobQueue.size());

    if ((int)glSensorInfoJobQueue.size() > 0)
        vilslog::printf("dump_queue_status() glSensorInfoJobQueue.size()=%d\n", (int)glSensorInfoJobQueue.size());
    if ((int)glMQJobQueue.size() > 0)
        vilslog::printf("dump_queue_status() glMQJobQueue.size()=%d\n", (int)glMQJobQueue.size());

    dump_memory_usage();
}

#if defined(__APPLE__)
void dump_memory_usage()
{
    freeAsMuchAsPossible();

    struct rusage usage;
    if(0 == getrusage(RUSAGE_SELF, &usage))
    {
        //long sec = usage.ru_utime.tv_sec + usage.ru_stime.tv_sec;
        //long usec = usage.ru_utime.tv_usec + usage.ru_stime.tv_usec;
        //sec += usec/1000000;
        //usec %= 1000000;

        long sec = usage.ru_stime.tv_sec;
        sec += usage.ru_stime.tv_usec/1000000;
        // long sec = usage.ru_utime.tv_sec;
        // sec += usage.ru_utime.tv_usec/1000000;

        int hour = (int)(sec / 3600);
        int min = (int)(sec / 60);
        int hr_min = hour * 60;
        int hr_sec = hour * 3600;
        min -= hr_min;
        int min_sec = min * 60;
        sec -= (hr_sec + min_sec);
        //int hour = sec/(60*60);
        // 1024 * 1024 = 1048576
        vilslog::printf("dump_queue_status() MemoryUsage[%lu KBytes] RunTime[%02d:%02d:%02d]\n",
            (usage.ru_maxrss/(1024)), (int)hour, (int)min, (int)sec);
    }

    size_t bytesInUse = 0;
    size_t blocksInUse = 0;
    size_t sizeAllocated = 0;
    heapInUse(&bytesInUse, &blocksInUse, &sizeAllocated);
    vilslog::printf("in use: %zu KBytes, allocated: %zu, blocks: %zu\n", bytesInUse/(1024), sizeAllocated, blocksInUse);
    heapStats();

}
#elif defined(__unix__) || defined(linux)
void dump_memory_usage()
{

    struct rusage usage;
    if(0 == getrusage(RUSAGE_SELF, &usage))
    {
        //long sec = usage.ru_utime.tv_sec + usage.ru_stime.tv_sec;
        //long usec = usage.ru_utime.tv_usec + usage.ru_stime.tv_usec;
        //sec += usec/1000000;
        //usec %= 1000000;

        long sec = usage.ru_stime.tv_sec;
        sec += usage.ru_stime.tv_usec/1000000;
        // long sec = usage.ru_utime.tv_sec;
        // sec += usage.ru_utime.tv_usec/1000000;

        int hour = (int)(sec / 3600);
        int min = (int)(sec / 60);
        int hr_min = hour * 60;
        int hr_sec = hour * 3600;
        min -= hr_min;
        int min_sec = min * 60;
        sec -= (hr_sec + min_sec);
        //int hour = sec/(60*60);
        // 1024 * 1024 = 1048576
        vilslog::printf("dump_queue_status() MemoryUsage[%lu KBytes] RunTime[%02d:%02d:%02d]\n",
            (usage.ru_maxrss/(1024)), (int)hour, (int)min, (int)sec);
    }


#if defined(__APPLE__)
    size_t bytesInUse = 0;
    size_t blocksInUse = 0;
    size_t sizeAllocated = 0;
    heapInUse(&bytesInUse, &blocksInUse, &sizeAllocated);
    vilslog::printf("in use: %zu KBytes, allocated: %zu, blocks: %zu\n", bytesInUse/(1024), sizeAllocated, blocksInUse);
    heapStats();
#else
    double vm, rss;
    process_mem_usage(vm, rss);

    vilslog::printf("in use: %f, rss: %f\n", vm, rss);
#endif

}
#endif

void send_all_BlackList_info()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::list<NodeStruct*> * coordNodeList = glDeviceManager->get_Coordinator_list(projectid);
        send_BlackList_info(coordNodeList);
    }

    prjidList->clear();
    delete prjidList;
}

void send_all_WhiteList_info()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::list<NodeStruct*> * coordNodeList = glDeviceManager->get_Coordinator_list(projectid);
        send_WhiteList_info(coordNodeList);
    }

    prjidList->clear();
    delete prjidList;
}

void send_all_Validrang_info()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::list<NodeStruct*> * coordNodeList = glDeviceManager->get_Coordinator_list(projectid);
        send_Validrang_info(coordNodeList);
    }

    prjidList->clear();
    delete prjidList;
}

void printTagInfo()
{
    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);

        std::list<NodeStruct*> * tagNodeList = glDeviceManager->get_Tag_list(projectid);
        vilslog::printf("printTagInfo()[%d] tagNodeList->size()[%d]\n", projectid, (int)tagNodeList->size());

        for (std::list<NodeStruct*>::iterator node = tagNodeList->begin(); node != tagNodeList->end(); node++)
        {
            vilslog::printf("[%d] [%02d][%d] [%s][%s][%s] [%d][%s][%s] [%s][%s]\n", projectid, (*node)->index, (*node)->alive,
                (*node)->nodeid, (*node)->nodename, (*node)->macaddress, (*node)->tafid, (*node)->version,
                (*node)->voltage, (*node)->AreaId, (*node)->AreaName);
        }
    }
}

void send_BlackList_info(std::list<NodeStruct*> * nodeList)
{
    for (std::list<NodeStruct*>::iterator node = nodeList->begin(); node != nodeList->end(); node++)
    {
        if(strlen((*node)->IP) > 0)
        {
#ifdef ENABLE_LOCAL_COORDINATOR
            DataItem * senditem = new DataItem("setblacklist", (*node)->projectid, (*node)->macaddress, (*node)->IP, (*node)->blacklist);
            glSendOutJobQueue.add(senditem);
#endif
        }
    }
}

void send_WhiteList_info(std::list<NodeStruct*> * nodeList)
{
    for (std::list<NodeStruct*>::iterator node = nodeList->begin(); node != nodeList->end(); node++)
    {
        if(strlen((*node)->IP) > 0)
        {
#ifdef ENABLE_LOCAL_COORDINATOR
            DataItem * senditem = new DataItem("setwhitelist", (*node)->projectid, (*node)->macaddress, (*node)->IP, (*node)->whitelist);
            glSendOutJobQueue.add(senditem);
#endif
        }
    }
}

void send_Validrang_info(std::list<NodeStruct*> * nodeList)
{
    for (std::list<NodeStruct*>::iterator node = nodeList->begin(); node != nodeList->end(); node++)
    {
        if(strlen((*node)->IP) > 0)
        {
#ifdef ENABLE_LOCAL_COORDINATOR
            DataItem * senditem = new DataItem("setvalidrang", (*node)->projectid, (*node)->macaddress, (*node)->IP, (*node)->joinRangingRange, (*node)->joinNotRangingRange);
            glSendOutJobQueue.add(senditem);
#endif
        }
    }
}

void sendSystemTimeToDevice()
{
    std::string time = currentDateTimeForESP32();
    std::string cmd = "AT+DATETIME=";
    cmd.append(time);

    std::list<int> * prjidList = glDBManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);

        std::list<NFCDeviceStruct*> * nfcDeviceList = glDeviceManager->get_NFCDevice_list(projectid);
        for (std::list<NFCDeviceStruct*>::iterator device = nfcDeviceList->begin(); device != nfcDeviceList->end(); device++)
        {
            //==========================================
            // if ( (*device)->LoRAMacaddress.compare("00000000035f0023") == 0 )
            // {
            //     send_MQTT_cmd_todevice(1, cmd.c_str(), (*device));
            // }
            //==========================================
            send_MQTT_cmd_todevice(1, cmd.c_str(), (*device));
            //==========================================
        }
    }
}

bool send_MQTT_cmd_todevice(int projectid, std::string cmd, NFCDeviceStruct * device)
{
    std::ostringstream sendstreamData;
    sendstreamData << "[{\"cmd\":\""<< cmd <<"\",\"time\":\"" << currentDateTimeForSQL() << "\"}]";
    std::string payloadData = sendstreamData.str();
    std::string hexDataString = StringToHEXString(payloadData);

    std::ostringstream payloadstream;
    payloadstream << "[{\"macAddr\":\"";
    payloadstream << device->LoRAMacaddress;
    payloadstream << "\",\"data\":\"";
    payloadstream << hexDataString;
    payloadstream << "\",\"id\":\"";
    payloadstream << random_string(16);
    payloadstream << "\",\"extra\":{\"port\":0,\"txpara\":\"22\"}}]";

    std::string payload = payloadstream.str();

    // vilslog::printf("send_MQTT_cmd_todevice() projectid[%d] payload[%s] LoRAMacaddress[%s] gwid[%s]\n", projectid, payload.c_str(), device->LoRAMacaddress.c_str(), device->gwid.c_str());

    MQTTDataItem * mqttitem = new MQTTDataItem("MQTTpublish", projectid, device->gwid.c_str(), payload.c_str(), (int)payload.length());
    glMQTTPubJobQueue.add(mqttitem);

    return true;
}

bool send_MQTT_cmd_wentaitek(std::string payload)
{
    replaceAll(payload,"\\","");

    vilslog::printf("VILS_RTLS::send_MQTT_cmd_wentaitek() payload->%s \n", payload.c_str());

    MQTTDataItem * mqttitem = new MQTTDataItem("MQTTWentaitek", 1, "", payload.c_str(), (int)payload.length());
    glMQTTPubJobQueue.add(mqttitem);

    return true;
}

std::string prepareLocatoRSSIInfoString(LocatorRSSIInfoStruct * rssiInfo)
{
    std::ostringstream sendstream;

    for (std::list<GatewayRSSIInfoStruct*>::iterator existed = rssiInfo->gatewayRecords->begin(); existed != rssiInfo->gatewayRecords->end(); existed++)
    {
        int total_rssi = 0;
        int average_rssi = 0;
        std::string startdatetime = "";
        std::string enddatetime = "";

        int rssi_count = 0;
        for (std::list<RSSIInfoStruct*>::iterator rssi_it = (*existed)->rssiRecords->begin(); rssi_it != (*existed)->rssiRecords->end(); rssi_it++)
        {
            if ((*rssi_it)->rssi >= 0 )
            {
                continue;
            }

            total_rssi += (*rssi_it)->rssi;
            rssi_count += 1;

            if (rssi_count == 1)
            {
                startdatetime = (*rssi_it)->datetime;
            }
            enddatetime = (*rssi_it)->datetime;
        }

        if (rssi_count > 0)
        {
            average_rssi = total_rssi / rssi_count;
        }

        sendstream << (*existed)->gwid << "={";
        sendstream<<"\"rssi\":\""<<average_rssi<<"\"";
        sendstream << ",";
        sendstream<<"\"count\":\""<<rssi_count<<"\"";
        sendstream << ",";
        sendstream<<"\"startdatetime\":\""<<startdatetime<<"\"";
        sendstream << ",";
        sendstream<<"\"enddatetime\":\""<<enddatetime<<"\"";
        sendstream << "}" << endl;
    }

    return sendstream.str();
}

static void s_signal_handler (int signal_value)
{
    s_interrupted = 1;
}

static void s_SIGABRT_handler (int signal_value)
{
    vilslog::printf("s_SIGABRT_handler() signal_value[%d]\n", signal_value);
    s_interrupted = 2;
}

static void s_catch_signals (void)
{
    struct sigaction action;
    action.sa_handler = s_signal_handler;
    action.sa_flags = 0;
    sigemptyset (&action.sa_mask);
    sigaction (SIGINT, &action, NULL);
    sigaction (SIGTERM, &action, NULL);
    sigaction (SIGSTOP, &action, NULL);

    struct sigaction action1;
    action1.sa_handler = s_SIGABRT_handler;
    action1.sa_flags = 0;
    sigemptyset (&action1.sa_mask);
    sigaction (SIGABRT, &action1, NULL);
}


// int demo_XLSX()
// {
//
//     OpenXLSX::XLDocument doc1;
//     doc1.CreateDocument("./UnicodeTest.xlsx");
//     auto wks1 = doc1.Workbook().Worksheet("Sheet1");
//
//     wks1.Cell("A1").Value() = "안녕하세요 세계!";
//     wks1.Cell("A2").Value() = "你好，世界!";
//     wks1.Cell("A3").Value() = "こんにちは世界";
//     wks1.Cell("A4").Value() = "नमस्ते दुनिया!";
//     wks1.Cell("A5").Value() = "Привет, мир!";
//     wks1.Cell("A6").Value() = "Γειά σου Κόσμε!";
//
//     doc1.SaveDocument();
//     doc1.CloseDocument();
//
//     OpenXLSX::XLDocument doc2;
//     doc2.OpenDocument("./UnicodeTest.xlsx");
//     auto wks2 = doc2.Workbook().Worksheet("Sheet1");
//
//     cout << "Cell A1: " << wks2.Cell("A1").Value().Get<std::string>() << endl;
//     cout << "Cell A2: " << wks2.Cell("A2").Value().Get<std::string>() << endl;
//     cout << "Cell A3: " << wks2.Cell("A3").Value().Get<std::string>() << endl;
//     cout << "Cell A4: " << wks2.Cell("A4").Value().Get<std::string>() << endl;
//     cout << "Cell A5: " << wks2.Cell("A5").Value().Get<std::string>() << endl;
//     cout << "Cell A6: " << wks2.Cell("A6").Value().Get<std::string>() << endl;
//
//     doc2.CloseDocument();
//
//     return 0;
// }
