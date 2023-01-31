/*
 *  WebUDPParserService.cpp
 *
 *  Created on: 2019/04/15
 *      Author: ikki
 */

#include <string.h>
#include <sstream>
#include <iostream>
#include <math.h> // atan2
#include <boost/algorithm/string.hpp>
#include <curl/curl.h>

#include "WebUDPParserService.h"

#include "../PosCalculate/algorithm.h"
#include "../pdf/PDFCreater.h"
#include "../XLSX/openxlsx/interfaces/c++/headers/OpenXLSX.h"
#include "../network/inter_thread_pubsub.hpp"

using namespace std;

static size_t QueryLocationWriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

WebUDPParserService::WebUDPParserService(wqueue<WorkItem*> & MySQLJobQueue, wqueue<DataItem*> & MainJobQueue, wqueue<UDPDataItem*> & udpqueue, wqueue<NotificationItem*> & NotifiactionQueue, DeviceManager * pDevManager, DBManager * dbManager)
    : mMySQLJobQueue(MySQLJobQueue), mMainJobQueue(MainJobQueue), m_udpqueue(udpqueue), mNotifiactionQueue(NotifiactionQueue), mDeviceManager(pDevManager), mDbManager(dbManager)
{
}

bool WebUDPParserService::status()
{
    return mInitial;
}

std::string WebUDPParserService::statusString()
{
    return mStatus;
}

void * WebUDPParserService::run()
{
    mSystemStartTime = getCurrentTimeInSec();
    mSystemInfo.updateTimestamp = 0;

    while(1)
    {
        try
        {
            initQueue();
            runloop();
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("WebUDPParserService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = true;
            mStatus = "WebUDPParserService()::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void WebUDPParserService::initQueue()
{
    mFireFightingPublisher = new ITPS::Publisher<std::string>("FireFighting", "event");
    mSensorBodyInfoPublisher = new ITPS::Publisher<WorkItem*>("Sensor", "BodyInfo");
}

void WebUDPParserService::runloop()
{
    mInitial = true;
    mStatus = "WebUDPParserService()::run() Initial successfully!";
    vilslog::printf ("WebUDPParserService()::run() Initial successfully!\n");

    getSystemRunTime(mSystemInfo, mSystemStartTime);
    // test_create_QRCODE();

    waitloop();

}

void WebUDPParserService::waitloop()
{
    ITPS::Publisher<std::string> pub1("Topic1", "Msg1");
    while ( 1 )
    {
        //UDPDataItem * item = m_udpqueue.remove();
        UDPDataItem * item = m_udpqueue.removetimed(1000);

        // pub1.publish("Hello, I'm WebUDPParserService");

        if ( item == NULL )
        {
            continue;
        }

        // remove old image
        for (std::list<QRCodeStruct*>::iterator img = mQRCodeImageList.begin(); img != mQRCodeImageList.end(); img++)
        {
            time_t curTime = getCurrentTimeInSec();
            int diff = (int)(curTime - (*img)->createtime);

            if ( diff >= 30 )
            {
                vilslog::printf("WebUDPParserService::waitloop() remove QRCode Image file[%s] diff[%d]\n", (*img)->filepath.c_str(), diff);
                // remove file
                remove((*img)->filepath.c_str());
                mQRCodeImageList.remove((*img));
                break;
            }
        }

        // remove old sned notify
        for (std::list<SendNotifyStatusStruct*>::iterator status = mSendNotifyStatusList.begin(); status != mSendNotifyStatusList.end(); status++)
        {
            time_t curTime = getCurrentTimeInSec();
            int diff = (int)(curTime - (*status)->createtime);

            if ( diff >= 120 )
            {
                vilslog::printf("WebUDPParserService::waitloop() remove Send Notifytatus waitkey[%s] diff[%d]\n", (*status)->waitkey.c_str(), diff);
                mSendNotifyStatusList.remove((*status));
                break;
            }
        }

        for (std::list<SendNotifyStatusStruct*>::iterator status = mSendATCmdStatusList.begin(); status != mSendATCmdStatusList.end(); status++)
        {
            time_t curTime = getCurrentTimeInSec();
            int diff = (int)(curTime - (*status)->createtime);

            if ( diff >= 120 )
            {
                vilslog::printf("WebUDPParserService::waitloop() remove Send ATCmd waitkey[%s] diff[%d]\n", (*status)->waitkey.c_str(), diff);
                mSendATCmdStatusList.remove((*status));
                break;
            }
        }


        int length = item->getMsgLength();

        if (length <= 0)
        {
            delete item;
            continue;
        }
        char * buf = item->getMsg();
        std::string origString = item->getMsg();
        std::string type = item->getMsg();
        std::string payload = "";
        std::string::size_type pos = type.find('=');
        if (pos != std::string::npos)
        {
            type = origString.substr(0, pos);
            payload = origString.substr(pos+1);
        }
        else
        {
            type = origString;
        }

        // vilslog::printf("WebUDPParserService::waitloop() type[%s]\n", type.c_str());

        // ProcessCount++;
        // if ((ProcessCount % 100) == 0)
        // {
        //     vilslog::out <<currentDateTimeForSQL() << " WebUDPParserService::waitloop() ProcessCount:"<<ProcessCount<<endl;
        //     vilslog::flush() ;
        // }

        //
        // check command
        //
        if ( type.compare("systemstatus") == 0 )
        {

            //printf ("udp_wait_loop:[anchor] length[%d] buf[%s]\n", length, buf);
            sendSystemStatusToClient(item);
        }
        else
        if ( type.compare("single_tag_info") == 0 )
        {
            sendSingleTagStatusToClient(item);
        }
        else
        if ( type.compare("appversion") == 0 )
        {
            //printf ("udp_wait_loop:[anchor] length[%d] buf[%s]\n", length, buf);
            char showmsg[64];
            snprintf(showmsg, 64, "v %s", CENTER_APP_VERSION);
            std::string strReturn = showmsg;
            const char * sendBuf = strReturn.c_str();
            item->mUDPServer->sendToClient(sendBuf, (int)strReturn.length(), (struct sockaddr *)&item->m_client_addr);
        }
        else
        if ( type.compare("tagstatus") == 0 )
        {
            sendTagStatusToClient(item);
        }
        else
        if ( type.compare("tagrollcallstatus") == 0 )
        {
            sendTagRollCallStatusToClient(item);
        }
        else
        if ( type.compare("tagrollcallpdfreport") == 0 )
        {
            sendTagRollCallPdfReportToClient(item);
        }
        else
        if ( type.compare("tagrollcallxlsxreport") == 0 )
        {
            sendTagRollCallXlsxReportToClient(item);
        }
        else
        if ( type.compare("userinfoxlsxreport") == 0 )
        {
            sendUserInfoXlsxReportToClient(item);
        }
        else
        if ( type.compare("userrollcallresultxlsxreport") == 0 )
        {
            sendUserRollCallResultXlsxReportToClient(item);
        }
        else
        if ( type.compare("unknownnfccardsxlsxreport") == 0 )
        {
            sendUnknowNFCCardsXlsxReportToClient(item);
        }
        else
        if ( type.compare("unknownuhfcardsxlsxreport") == 0 )
        {
            sendUnknowUHFCardsXlsxReportToClient(item);
        }
        else
        if ( type.compare("anchorstatus") == 0 )
        {
            sendAnchorStatusToClient(item);
        }
        else
        if ( type.compare("coordstatus") == 0 )
        {
            sendCoordStatusToClient(item);
        }
        else
        if ( type.compare("locatorstatus") == 0 )
        {
            sendLocatorStatusToClient(item);
        }
        else
        if ( type.compare("aoastatus") == 0 )
        {
            sendAoAStatusToClient(item);
        }
        else
        if ( type.compare("loadloragateway") == 0 )
        {
            sendLoRaGatewayStatusToClient(item);
        }
        else
        if ( type.compare("nfcdevicetatus") == 0 )
        {
            sendNFCDeviceStatusToClient(item);
        }
        else
        if ( type.compare("nfcdevicecount") == 0 )
        {
            sendNFCDeviceCountToClient(item);
        }
        else
        if ( type.compare("wifidevicetatus") == 0 )
        {
            sendWIFIDeviceStatusToClient(item);
        }
        else
        if ( type.compare("celldevicetatus") == 0 )
        {
            sendCELLDeviceStatusToClient(item);
        }
        else
        if ( type.compare("edgedevicetatus") == 0 )
        {
            sendEDGEDeviceStatusToClient(item);
        }
        else
        if ( type.compare("sensordevicedata") == 0 )
        {
            sendSensorDeviceDataToClient(item);
        }
        else
        if ( type.compare("sensordevicetatus") == 0 )
        {
            sendSensorDeviceStatusToClient(item);
        }
        else
        if ( type.compare("sensordevicebaseinfo") == 0 )
        {
            sendSensorDeviceBaseInfoToClient(item);
        }
        else
        if ( type.compare("send_lightCommand") == 0 )
        {
            vilslog::printf("WebUDPParserService::wait_loop()  -> payload->%s\n", payload.c_str());

            send_lightCommand(payload);
        }
        else
        if ( type.compare("update_sensordevice") == 0 )
        {
            // printf ("WebUDPParserService::waitloop():[update_sensordevice]\n");
            DataItem * senditem = new DataItem("reloadsensordevice", 1);
            mMainJobQueue.add(senditem);
        }
        else
        if ( type.compare("nfccardtatus") == 0 )
        {
            sendNFCCardStatusToClient(item);
        }
        else
        if ( type.compare("rfiddevicetatus") == 0 )
        {
            sendRFIDDeviceStatusToClient(item);
        }
        else
        if ( type.compare("rfidcardtatus") == 0 )
        {
            sendRFIDCardStatusToClient(item);
        }
        else
        if ( type.compare("devicescount") == 0 )
        {
            //printf ("WebUDPParserService::waitloop():[tagstatus] length[%d] buf[%s]\n", length, buf);
            sendDevicesCountToClient(item);
        }
        else
        if ( type.compare("status") == 0 )
        {
            sendDeviceStatusToClient(item);
        }
        else
        if ( type.compare("startdumprangingresult") == 0 )
        {
            //vilslog::printf ("WebUDPParserService::waitloop():[startdumprangingresult]\n");
            startDumpRangingResult(buf, length);
        }
        else
        if ( type.compare("loadranging") == 0 )
        {
            sendRangingToClient(item);
        }
        else
        if ( type.compare("loadcardscan") == 0 )
        {
            sendCardScanResultToClient(item);
        }
        else
        if ( type.compare("loadaoarangingcloudpoint") == 0 )
        {
            sendAoARangingCloudPointToClient(item);
        }
        else
        if ( type.compare("loadaoaanglevectorcloudpoint") == 0 )
        {
            sendAoAAngleVectorCloudPointToClient(item);
        }
        else
        if ( type.compare("loadaoaanglecloudpoint") == 0 )
        {
            sendAoAAngleCloudPointToClient(item);
        }
        else
        if ( type.compare("PowerTagCalculateInfo") == 0 )
        {
            setPowerTagCalculateInfo(item);
        }
        else
        if ( type.compare("anchorinfo") == 0 )
        {
            //printf ("udp_wait_loop:[anchor] length[%d] buf[%s]\n", length, buf);
            updateNodeInfo("anchor", buf, length);
        }
        else
        if ( type.compare("taginfo") == 0 )
        {
            //printf ("udp_wait_loop:[anchor] length[%d] buf[%s]\n", length, buf);
            updateNodeInfo("tag", buf, length);
        }
        else
        if ( type.compare("coordinfo") == 0 )
        {
            //printf ("udp_wait_loop:[anchor] length[%d] buf[%s]\n", length, buf);
            updateNodeInfo("coord", buf, length);
        }
        else
        if ( type.compare("locatorinfo") == 0 )
        {
            //printf ("udp_wait_loop:[locator] length[%d] buf[%s]\n", length, buf);
            updateNodeInfo("locator", buf, length);
        }
        else
        if ( type.compare("gatewayinfo") == 0 )
        {
            // printf ("udp_wait_loop:[gateway] length[%d] buf[%s]\n", length, buf);
            updateNodeInfo("gateway", buf, length);
        }
        else
        if ( type.compare("nodeinfo") == 0 )
        {
            //rintf ("udp_wait_loop:[anchor] length[%d] buf[%s]\n", length, buf);
            updateMapNodeInfo(buf, length);
        }
        else
        if ( type.compare("deletenode") == 0 )
        {
           //printf ("udp_wait_loop: length[%d] buf[%s]\n", length, buf);
           deleteNodeInfo(buf, length);
        }
        else
        if ( type.compare("deletegateway") == 0 )
        {
           //printf ("udp_wait_loop: length[%d] buf[%s]\n", length, buf);
           deleteGatewayInfo(buf, length);
        }
        else
        if ( type.compare("deleteallofflinedevice") == 0 )
        {
           //printf ("udp_wait_loop: length[%d] buf[%s]\n", length, buf);
           deleteAllOfflineDevice(buf, length);
        }
        else
        if ( type.compare("updateconfig") == 0 )
        {
            updateConfig(buf, length);
        }
        else
        if ( type.compare("createconfig") == 0 )
        {
            createConfig(item);
        }
        else
        if ( type.compare("deleteconfig") == 0 )
        {
            deleteConfig(buf, length);
        }
        else
        if ( type.compare("loadsystemconfig") == 0 )
        {
            sendSystemConfigToClient(item);
        }
        else
        if ( type.compare("updatesystemconfigudpport") == 0 )
        {
            updateSystemConfigUDPPort(buf, length);
        }
        else
        if ( type.compare("updateAlgorithmConfig") == 0 )
        {
            updateAlgorithmConfig(buf, length);
        }
        else
        if ( type.compare("updateLoRAWANConfig") == 0 )
        {
            updateLoRAWANConfig(buf, length);
        }
        else
        if ( type.compare("resetLoRA") == 0 )
        {
            DataItem * senditem = new DataItem("resetLoRA", 1);
            mMainJobQueue.add(senditem);
        }
        else
        if ( type.compare("createsection") == 0 )
        {
            //printf ("WebUDPParserService::waitloop() [createsection] length[%d] buf[%s]\n", length, buf);
            createSection(buf, length);
        }
        else
        if ( type.compare("deletesection") == 0 )
        {
            //printf ("WebUDPParserService::waitloop() [deletesection] length[%d] buf[%s]\n", length, buf);
            deleteSection(buf, length);
        }
        else
        if ( type.compare("createfencealert") == 0 )
        {
            createFenceAlert(buf, length);
        }
        else
        if ( type.compare("deletefencealert") == 0 )
        {
            deleteFenceAlert(buf, length);
        }
        else
        if ( type.compare("updatearea") == 0 )
        {
            vilslog::printf ("udp_wait_loop:[updatearea] length[%d] buf[%s]\n", length, buf);
            updateArea(item);
        }
        else
        if ( type.compare("tagalert") == 0 )
        {
            //printf ("udp_wait_loop:[tagalert]\n");
            sendTagAlertToClient(item);
        }
        else
        if ( type.compare("getrangingdiagnosis") == 0 )
        {
            sendRangingDiagnosisToClient(item);
        }
        else
        if ( type.compare("getlocatorrssiinfo") == 0 )
        {
            sendLocatorRSSIInfoToClient(item);
        }
        else
        if ( type.compare("deleteevent") == 0 )
        {
            deleteEvent(buf, length);
        }
        else
        if ( type.compare("createevent") == 0 )
        {
            //printf ("WebUDPParserService::waitloop() [createevent] length[%d] buf[%s]\n", length, buf);
            createEvent(buf, length);
        }
        else
        if ( type.compare("updatemaplayer") == 0 )
        {
            //vilslog::printf ("WebUDPParserService::waitloop() [updatemaplayer] length[%d] buf[%s]\n", length, buf);
            updateMapLayer(buf, length);
        }
        else
        if ( type.compare("updatemaplayerarea") == 0 )
        {
            //printf ("WebUDPParserService::waitloop() [updatemaparea] length[%d] buf[%s]\n", length, buf);
            updateMapLayerArea(buf, length);
        }
        else
        if ( type.compare("updatemaplayerareashape") == 0 )
        {
            //printf ("WebUDPParserService::waitloop() [updatemaplayerareashape] length[%d] buf[%s]\n", length, buf);
            updateMapLayerAreaShape(buf, length);
        }
        else
        if ( type.compare("deletemaplayerarea") == 0 )
        {
            //printf ("WebUDPParserService::waitloop() [deletemaparea] length[%d] buf[%s]\n", length, buf);
            deleteMapLayerArea(buf, length);
        }
        else
        if ( type.compare("createinspectiongroup") == 0 )
        {
            createInspectionGroup(buf, length);
        }
        else
        if ( type.compare("updateinspectiongroup") == 0 )
        {
            updateInspectionGroup(buf, length);
        }
        else
        if ( type.compare("deleteinspectiongroup") == 0 )
        {
            deleteInspectionGroup(buf, length);
        }
        else
        if ( type.compare("createinspectionpoint") == 0 )
        {
            createInspectionPoint(buf, length);
        }
        else
        if ( type.compare("updateinspectionpoint") == 0 )
        {
            updateInspectionPoint(buf, length);
        }
        else
        if ( type.compare("deleteinspectionpoint") == 0 )
        {
            deleteInspectionPoint(buf, length);
        }
        else
        if ( type.compare("inspectionpdfreport") == 0 )
        {
            SendInspectionPDFReportToClient(item);
        }
        else
        if ( type.compare("bodyinfopdfreport") == 0 )
        {
            // vilslog::printf ("WebUDPParserService::waitloop() [bodyinfopdfreport]\n");
            SendBodyInfoPDFReportToClient(item);
        }
        else
        if ( type.compare("userbodyinfopdfreport") == 0 )
        {
            // vilslog::printf ("WebUDPParserService::waitloop() [bodyinfopdfreport]\n");
            SendUserBodyInfoPDFReportToClient(item);
        }
        else
        if ( type.compare("loadcoursepdfreport") == 0 )
        {
            SendCoursePDFReportToClient(item);
        }
        else
        if ( type.compare("createcourse") == 0 )
        {
            //printf ("WebUDPParserService::waitloop() [createcourse] length[%d] buf[%s]\n", length, buf);
            createCourse(buf, length);
        }
        else
        if ( type.compare("deletecourse") == 0 )
        {
            //printf ("WebUDPParserService::waitloop() [createcourse] length[%d] buf[%s]\n", length, buf);
            deleteCourse(buf, length);
        }
        else
        if ( type.compare("loadposnode") == 0 )
        {
            sendPOSNodeToClient(item);
        }
        else
        if ( type.compare("loadgpsnode") == 0 )
        {
            sendGPSNodeToClient(item);
        }
        else
        if ( type.compare("loaddevicealive") == 0 )
        {
            sendDeviceAliveToClient(item);
        }
        else
        if ( type.compare("loadsystemservice") == 0 )
        {
            sendSystemServiceToClient(item);
        }
        else
        if ( type.compare("resetm4") == 0 )
        {
            resetm4(item);
        }
        else
        if ( type.compare("resetm0") == 0 )
        {
            resetm0(item);
        }
        else
        if ( type.compare("blacklist") == 0 )
        {
            blacklist(buf, length);
        }
        else
        if ( type.compare("whitelist") == 0 )
        {
            whitelist(buf, length);
        }
        else
        if ( type.compare("clearblacklist") == 0 )
        {
            clearblacklist(buf, length);
        }
        else
        if ( type.compare("clearwhitelist") == 0 )
        {
            clearwhitelist(buf, length);
        }
        else
        if ( type.compare("joinRangingRange") == 0 )
        {
            joinRangingRange(buf, length);
        }
        else
        if ( type.compare("joinNotRangingRange") == 0 )
        {
            joinNotRangingRange(buf, length);
        }
        else
        if ( type.compare("nodeResident") == 0 )
        {
            nodeResident(buf, length);
        }
        else
        if ( type.compare("nodeEmployee") == 0 )
        {
            nodeEmployee(buf, length);
        }
        else
        if ( type.compare("locatorButtontype") == 0 )
        {
            locatorButtontype(buf, length);
        }
        else
        if ( type.compare("locatorSensorAlert") == 0 )
        {
            locatorSensorAlert(buf, length);
        }
        else
        if ( type.compare("checkaccountlogin") == 0 )
        {
            checkaccountlogin(item);
        }
        else
        if ( type.compare("checkaccountloginkey") == 0 )
        {
            checkaccountloginkey(item);
        }
        else
        if ( type.compare("queryaccountloginkey") == 0 )
        {
            queryaccountloginkey(item);
        }
        else
        if ( type.compare("syncERPaccount") == 0 )
        {
            syncERPaccount(item);
        }
        else
        if ( type.compare("loadqrcode") == 0 )
        {
            sendQRCodeToClient(item);
        }
        else
        if ( type.compare("loadpdfreport") == 0 )
        {
            sendPDFReportToClient(item);
        }
        else
        if ( type.compare("loadareainoutpdfreport") == 0 )
        {
            sendTagAreaInOutPDFReportToClient(item);
        }
        else
        if ( type.compare("loadareainoutxlsxreport") == 0 )
        {
            sendTagAreaInOutXLSXReportToClient(item);
        }
        else
        if ( type.compare("loadeventpdfreport") == 0 )
        {
            sendEventPDFReportToClient(item);
        }
        else
        if ( type.compare("loadtagdailypdfreport") == 0 )
        {
            sendTagDailyPDFReportToClient(item);
        }
        else
        if ( type.compare("updatePushNotify") == 0 )
        {
            updatePushNotify(buf, length);
        }
        else
        if ( type.compare("updatePushNotifyTriggerMessage") == 0 )
        {
            updatePushNotifyTriggerMessage(buf, length);
        }
        else
        if ( type.compare("sendmultinotification") == 0 )
        {
            sendmultinotification(item);
        }
        else
        if ( type.compare("waitpushnotify") == 0 )
        {
            waitpushnotify(item);
        }
        else
        if ( type.compare("load_dashboard_activity") == 0 )
        {
            sendDashboardActivityToClient(item);
        }
        else
        if ( type.compare("load_dashboard_systeminfo") == 0 )
        {
            sendDashboardSystemInfoToClient(item);
        }
        else
        if ( type.compare("loadtagbodyinfo") == 0 )
        {
            sendTagBodyInfoToClient(item);
        }
        else
        if ( type.compare("loadtagbodyinforecords") == 0 )
        {
            sendTagBodyInfoRecordsToClient(item);
        }
        else
        if ( type.compare("loadlocatorbodyinfo") == 0 )
        {
            sendLocatorBodyInfoToClient(item);
        }
        else
        if ( type.compare("loadtagprjinfo") == 0 )
        {
            sendTagPrjInfoToClient(item);
        }
        else
        if ( type.compare("loadanchorprjinfo") == 0 )
        {
            sendAnchorPrjInfoToClient(item);
        }
        else
        if ( type.compare("loadcoordprjinfo") == 0 )
        {
            sendCoordPrjInfoToClient(item);
        }
        else
        if ( type.compare("loadlocatorprjinfo") == 0 )
        {
            sendLocatorPrjInfoToClient(item);
        }
        else
        if ( type.compare("loadgatewayprjinfo") == 0 )
        {
            sendGatewayPrjInfoToClient(item);
        }
        else
        if ( type.compare("updatenodeprj") == 0 )
        {
            updatenodeprj(buf, length);
        }
        else
        if ( type.compare("updatenodeuser") == 0 )
        {
            updatenodeuser(buf, length);
        }
        else
        if ( type.compare("aoanormalvector") == 0 )
        {
            updateaoanormalvector(buf, length);
        }
        else
        if ( type.compare("sendatcmd") == 0 )
        {
            sendatcmd(item);
        }
        else
        if ( type.compare("waitatcmd") == 0 )
        {
            waitatcmd(item);
        }
        else
        if ( type.compare("printsystemlog") == 0 )
        {
            printsystemlog(buf, length);
        }
        else
        if ( type.compare("loadabbox") == 0 )
        {
            sendABBoxToClient(item);
        }
        else
        if ( type.compare("savebodyinfoweight") == 0 )
        {
            savebodyinfoweight(buf, length);
        }
        else
        if ( type.compare("savebodyinfoheartbeat") == 0 )
        {
            savebodyinfoheartbeat(buf, length);
        }
        else
        if ( type.compare("savebodyinfoall") == 0 )
        {
            savebodyinfoall(buf, length);
        }
        else
        if ( type.compare("loadforacaredata") == 0 )
        {
            sendForacaredataClient(item);
        }
        else
        if ( type.compare("loadunknownpositiontag") == 0 )
        {
            sendUnknownPositionTagToClient(item);
        }
        else
        if ( type.compare("fixposition") == 0 )
        {
            setFixPosition(buf, length);
        }
        else
        if ( type.compare("fixdevice") == 0 )
        {
            setFixDevice(buf, length);
        }
        else
        if ( type.compare("create_new_user") == 0 )
        {
            create_new_user(buf, length);
        }
        else
        if ( type.compare("update_user") == 0 )
        {
            update_user(buf, length);
        }
        else
        if ( type.compare("delete_user") == 0 )
        {
            delete_user(buf, length);
        }
        else
        if ( type.compare("reset_user_topic") == 0 )
        {
            reset_user_topic(item);
        }
        else
        if ( type.compare("reset_user_password") == 0 )
        {
            reset_user_password(item);
        }
        else
        if ( type.compare("updatesurveyresult") == 0 )
        {
            DataItem * senditem = new DataItem("updatesurveyresult", 1);
            mMainJobQueue.add(senditem);
        }
        else
        if ( type.compare("cmd") == 0 )
        {
            DataItem * senditem = new DataItem(payload.c_str(), 1);
            mMainJobQueue.add(senditem);
        }
        else
        if ( type.compare("updateCARDRollCall") == 0 )
        {
            DataItem * senditem = new DataItem("updateCARDRollCall", 1);
            mMainJobQueue.add(senditem);
        }
        else
        if ( type.compare("scanRFIDCard") == 0 )
        {
            DataItem * senditem = new DataItem("scanRFIDCard", 1);
            mMainJobQueue.add(senditem);
        }
        else
        if ( type.compare("scanNFCCard") == 0 )
        {
            DataItem * senditem = new DataItem("scanNFCCard", 1);
            mMainJobQueue.add(senditem);
        }
        else
        if ( type.compare("updaterollcallrecord") == 0 )
        {
            update_rollcall_record(buf, length);
        }
        else
        if ( type.compare("updaterollcallrecords") == 0 )
        {
            update_rollcall_records(buf, length);
        }
        else
        if ( type.compare("update_nfclocator") == 0 )
        {
            DataItem * senditem = new DataItem("reloadnfcdevice", 1);
            mMainJobQueue.add(senditem);

            DataItem * senditem1 = new DataItem("updateCARDRollCall", 1);
            mMainJobQueue.add(senditem1);
        }
        else
        if ( type.compare("update_rfiddevice") == 0 )
        {
            DataItem * senditem = new DataItem("reloadrfiddevice", 1);
            mMainJobQueue.add(senditem);

            DataItem * senditem1 = new DataItem("updateCARDRollCall", 1);
            mMainJobQueue.add(senditem1);
        }
        else
        if ( type.compare("update_rfidcard") == 0 )
        {
            DataItem * senditem = new DataItem("update_rfidcard", 1);
            mMainJobQueue.add(senditem);
        }
        else
        if ( type.compare("update_nfccard") == 0 )
        {
            DataItem * senditem = new DataItem("update_nfccard", 1);
            mMainJobQueue.add(senditem);
        }
        else
        if ( type.compare("update_edgecontrol") == 0 )
        {
            DataItem * senditem = new DataItem("reloadedgedevice", 1);
            mMainJobQueue.add(senditem);
        }
        else
        if ( type.compare("update_LoRaGatewayInfo") == 0 )
        {
            DataItem * senditem = new DataItem("reloadLoRaGatewayInfo", 1);
            mMainJobQueue.add(senditem);
        }
        else
        if ( type.compare("uploadxlsxfile") == 0 )
        {
            vilslog::printf ("udp_wait_loop:[uploadxlsxfile]\n");
            uploadxlsxfile(item);
        }
        else
        if ( type.compare("newleaveform") == 0 )
        {
            vilslog::printf ("udp_wait_loop:[newleaveform]\n");
            newleaveform(buf, length);
        }
        else
        if ( type.compare("leaveformsignoff") == 0 )
        {
            vilslog::printf ("udp_wait_loop:[leaveformsignoff]\n");
            leaveformsignoff(buf, length);
        }
        else
        if ( type.compare("newassistevent") == 0 )
        {
            vilslog::printf ("udp_wait_loop:[newassistevent]\n");
            newassistevent(buf, length);
        }
        else
        if ( type.compare("updateassistconfig") == 0 )
        {
            vilslog::printf ("udp_wait_loop:[updateassistconfig]\n");
            DataItem * senditem = new DataItem("updateAssist", 1);
            mMainJobQueue.add(senditem);
        }
        else
        if ( type.compare("update_usergroup") == 0 )
        {
            vilslog::printf ("udp_wait_loop:[update_usergroup]\n");

            mDeviceManager->reload_User();

            DataItem * senditem = new DataItem("updateUserGroup", 1);
            mMainJobQueue.add(senditem);
        }
        else
        if ( type.compare("update_usersubgroup") == 0 )
        {
            vilslog::printf ("udp_wait_loop:[update_usersubgroup]\n");

            mDeviceManager->reload_User();

            DataItem * senditem = new DataItem("updateUserSubGroup", 1);
            mMainJobQueue.add(senditem);
        }
        else
        if ( type.compare("update_usermaingroup") == 0 )
        {
            vilslog::printf ("udp_wait_loop:[update_usermaingroup]\n");

            mDeviceManager->reload_User();

            DataItem * senditem = new DataItem("updateUserMainGroup", 1);
            mMainJobQueue.add(senditem);
        }
        else
        if ( type.compare("update_nodemaingroup") == 0 )
        {
            vilslog::printf ("udp_wait_loop:[update_nodemaingroup]\n");

            update_nodemaingroup(buf, length);
        }
        else
        if ( type.compare("sensor_info") == 0 )
        {
            // vilslog::printf ("udp_wait_loop:[sensor_info]\n");
            sensor_info(buf, length);
        }
        else
        if ( type.compare("api_pushnotify") == 0 )
        {
            // vilslog::printf ("udp_wait_loop:[sensor_info]\n");
            api_pushnotify(buf, length);
        }
        else
        if ( type.compare("updatecurriculumrecord") == 0 )
        {
            updatecurriculumrecord(buf, length);

            mDeviceManager->reload_User();

            DataItem * senditem1 = new DataItem("updateCARDRollCall", 1);
            mMainJobQueue.add(senditem1);
        }
        else
        if ( type.compare("deletecurriculumrecord") == 0 )
        {
            deletecurriculumrecord(buf, length);

            DataItem * senditem1 = new DataItem("updateCARDRollCall", 1);
            mMainJobQueue.add(senditem1);
        }
        else
        if ( type.compare("applycurriculumrecord") == 0 )
        {
            applycurriculumrecord(buf, length);

            DataItem * senditem1 = new DataItem("updateCARDRollCall", 1);
            mMainJobQueue.add(senditem1);
        }
        else
        if ( type.compare("updatecurriculumapplystatus") == 0 )
        {
            updatecurriculumapplystatus(buf, length);

            DataItem * senditem1 = new DataItem("updateCARDRollCall", 1);
            mMainJobQueue.add(senditem1);
        }
        else
        if ( type.compare("get_air_remain") == 0 )
        {
            sendAirRemainToClient(item);
        }
        else
        if ( type.compare("reset_air_remain") == 0 )
        {
            resetAirRemain(buf, length);
        }
        else
        if ( type.compare("findlocation") == 0 )
        {
            findlocation(item, payload);
        }
        else
        if ( type.compare("createFireFighting") == 0 )
        {
            createFireFighting(item, payload);
        }
        else
        if ( type.compare("finishFireFighting") == 0 )
        {
            mFireFightingPublisher->publish(payload);
        }
        else
        if ( type.compare("updatefirefightingdeviceinfo") == 0 )
        {
            mFireFightingPublisher->publish(payload);
        }
        else
        if ( type.compare("updatefirefightinguserairremain") == 0 )
        {
            mFireFightingPublisher->publish(payload);
        }
        else
        if ( type.compare("updatefirefightinguserarea") == 0 )
        {
            mFireFightingPublisher->publish(payload);
        }
        // else
        // if ( type.compare("printAreaRollCallServiceTagAreaBuffer") == 0 )
        // {
        //     DataItem * senditem = new DataItem("printAreaRollCallServiceTagAreaBuffer", 1);
        //     mMainJobQueue.add(senditem);
        // }
        else
        if ( type.compare("restart_app") == 0 )
        {
            vilslog::printf ("udp_wait_loop:[restart_app]\n");
            DataItem * senditem = new DataItem("restart_app", 1);
            mMainJobQueue.add(senditem);
        }

        delete item;
    }
}

void WebUDPParserService::update_service_status(int index, bool status, const char * serviceString, std::string statusString)
{
    bool bFound = false;
    for (std::list<StatusStruct*>::iterator existed = mServiceStatusList.begin(); existed != mServiceStatusList.end(); existed++)
    {
        if ( (*existed)->index == index )
        {
            (*existed)->status = status;
            (*existed)->serviceString = serviceString;
            (*existed)->statusString = statusString;
            bFound = true;
            break;
        }
    }
    if (!bFound)
    {
        StatusStruct * newStatus = new StatusStruct();
        newStatus->index = index;
        newStatus->status = status;
        newStatus->serviceString = serviceString;
        newStatus->statusString = statusString;

        mServiceStatusList.push_back(newStatus);
    }
}

void WebUDPParserService::update_RangingDiagnosisString(int projectid, const char * tagmacaddress, std::string RangingDiagnosisString)
{
    bool bFound = false;
    for (std::list<RangingDiagnosisStruct*>::iterator existed = mRangingDiagnosisList.begin(); existed != mRangingDiagnosisList.end(); existed++)
    {
        if ( ((*existed)->tagmacaddress.compare(tagmacaddress) == 0) && ((*existed)->projectid == projectid) )
        {
            (*existed)->RangingDiagnosisString = RangingDiagnosisString;
            bFound = true;
            break;
        }
    }
    if (!bFound)
    {
        RangingDiagnosisStruct * newRanging = new RangingDiagnosisStruct();
        newRanging->projectid = projectid;
        newRanging->tagmacaddress = tagmacaddress;
        newRanging->RangingDiagnosisString = RangingDiagnosisString;

        mRangingDiagnosisList.push_back(newRanging);
    }
}

void WebUDPParserService::update_LastRangingString(int projectid, std::string LastRangingString)
{
    //mLastRangingString = LastRangingString;
    bool bFound = false;
    for (std::list<LastRangingStringStruct*>::iterator existed = mLastRangingStringList.begin(); existed != mLastRangingStringList.end(); existed++)
    {
        if ( (*existed)->projectId == projectid )
        {
            (*existed)->lastRangingString = LastRangingString;
            bFound = true;
            break;
        }
    }
    if (!bFound)
    {
        LastRangingStringStruct * newRanging = new LastRangingStringStruct();
        newRanging->projectId = projectid;
        newRanging->lastRangingString = LastRangingString;

        mLastRangingStringList.push_back(newRanging);
    }
}

void WebUDPParserService::update_LastRangingStringEx(int projectid, std::string LastRangingString)
{
    //mLastRangingStringEx = LastRangingString;
    bool bFound = false;
    for (std::list<LastRangingStringStruct*>::iterator existed = mLastRangingStringExList.begin(); existed != mLastRangingStringExList.end(); existed++)
    {
        if ( (*existed)->projectId == projectid )
        {
            (*existed)->lastRangingString = LastRangingString;
            bFound = true;
            break;
        }
    }
    if (!bFound)
    {
        LastRangingStringStruct * newRanging = new LastRangingStringStruct();
        newRanging->projectId = projectid;
        newRanging->lastRangingString = LastRangingString;

        mLastRangingStringExList.push_back(newRanging);
    }
}

void WebUDPParserService::update_PushNotifyStatus(int projectId, std::string waitkey, int statusvalue, std::string message)
{
    for (std::list<SendNotifyStatusStruct*>::iterator status = mSendNotifyStatusList.begin(); status != mSendNotifyStatusList.end(); status++)
    {
        if ( (*status)->waitkey.compare(waitkey) == 0 )
        {
            (*status)->status = statusvalue;
            (*status)->message = message;

            // vilslog::printf("WebUDPParserService::update_PushNotifyStatus statusvalue[%d] waitkey[%s] message[%s]\n",
            //     statusvalue, waitkey.c_str(), message.c_str());

            break;
        }
    }
}

void WebUDPParserService::update_SendATcmdStatus(int projectId, std::string waitkey, int statusvalue, std::string message)
{
    for (std::list<SendNotifyStatusStruct*>::iterator status = mSendATCmdStatusList.begin(); status != mSendATCmdStatusList.end(); status++)
    {
        if ( (*status)->waitkey.compare(waitkey) == 0 )
        {
            (*status)->status = statusvalue;
            (*status)->message = message;

            // vilslog::printf("WebUDPParserService::update_SendATcmdStatus statusvalue[%d] waitkey[%s] message[%s]\n",
            //     statusvalue, waitkey.c_str(), message.c_str());

            break;
        }
    }
}

void WebUDPParserService::update_MQTTMessage(int projectId, const char * macaddress, std::string message)
{
    // vilslog::printf("WebUDPParserService::update_MQTTMessage projectId[%d] macaddress[%s] message[%s]\n",
    //     projectId, macaddress, message.c_str());

    bool bFound = false;
    for (std::list<SendNotifyStatusStruct*>::iterator status = mSendATCmdStatusList.begin(); status != mSendATCmdStatusList.end(); status++)
    {
        if ( (*status)->macaddress.compare(macaddress) == 0 )
        {
            //vilslog::printf("WebUDPParserService::update_MQTTMessage Found!!!\n");
            (*status)->status = 1;
            (*status)->message = message;
            bFound = true;
        }
    }
    if (!bFound)
    {
        // vilslog::printf("WebUDPParserService::update_MQTTMessage Not Found!!! macaddress[%s] message[%s]\n", macaddress, message.c_str());
    }
}

void WebUDPParserService::send_lightCommand(std::string payload)
{
    // vilslog::printf("WebUDPParserService::send_lightCommand() payload->%s \n", payload.c_str());

    DataItem * senditemmain = new DataItem("send_light", 1);
    senditemmain->setMessage(payload);
    mMainJobQueue.add(senditemmain);
}

void WebUDPParserService::update_AoARangingCloudPoint(int projectId, std::string CloudPointString)
{
    bool bFound = false;
    for (std::list<AoACloudPointStringStruct*>::iterator existed = mAoARangingCloudPointList.begin(); existed != mAoARangingCloudPointList.end(); existed++)
    {
        if ( (*existed)->projectId == projectId )
        {
            //vilslog::printf("WebUDPParserService::update_AoARangingCloudPoint() CloudPointString.size()[%d]\n", (int)CloudPointString.size());

            (*existed)->CloudPointString = CloudPointString;
            bFound = true;
            break;
        }
    }
    if (!bFound)
    {
        AoACloudPointStringStruct * newString = new AoACloudPointStringStruct();
        newString->projectId = projectId;
        newString->CloudPointString = CloudPointString;

        //vilslog::printf("WebUDPParserService::update_AoARangingCloudPoint() projectId[%d] CloudPointString.size()[%d]\n", projectId, (int)CloudPointString.size());

        mAoARangingCloudPointList.push_back(newString);
    }
}

void WebUDPParserService::update_AoAAngleVectorCloudPoint(int projectId, std::string CloudPointString)
{
    bool bFound = false;
    for (std::list<AoACloudPointStringStruct*>::iterator existed = mAoAAngleVectorCloudPointList.begin(); existed != mAoAAngleVectorCloudPointList.end(); existed++)
    {
        if ( (*existed)->projectId == projectId )
        {
            // vilslog::printf("WebUDPParserService::update_AoAAngleVectorCloudPoint() CloudPointString.size()[%d]\n", (int)CloudPointString.size());

            (*existed)->CloudPointString = CloudPointString;
            bFound = true;
            break;
        }
    }
    if (!bFound)
    {
        AoACloudPointStringStruct * newString = new AoACloudPointStringStruct();
        newString->projectId = projectId;
        newString->CloudPointString = CloudPointString;

        // vilslog::printf("WebUDPParserService::update_AoAAngleVectorCloudPoint() projectId[%d] CloudPointString.size()[%d]\n", projectId, (int)CloudPointString.size());

        mAoAAngleVectorCloudPointList.push_back(newString);
    }
}

void WebUDPParserService::update_AoAAngleCloudPoint(int projectId, std::string CloudPointString)
{
    bool bFound = false;
    for (std::list<AoACloudPointStringStruct*>::iterator existed = mAoAAngleCloudPointList.begin(); existed != mAoAAngleCloudPointList.end(); existed++)
    {
        if ( (*existed)->projectId == projectId )
        {
            //vilslog::printf("WebUDPParserService::update_AoAAngleCloudPoint() CloudPointString.size()[%d]\n", (int)CloudPointString.size());

            (*existed)->CloudPointString = CloudPointString;
            bFound = true;
            break;
        }
    }
    if (!bFound)
    {
        AoACloudPointStringStruct * newString = new AoACloudPointStringStruct();
        newString->projectId = projectId;
        newString->CloudPointString = CloudPointString;

        //vilslog::printf("WebUDPParserService::update_AoAAngleCloudPoint() projectId[%d] CloudPointString.size()[%d]\n", projectId, (int)CloudPointString.size());

        mAoAAngleCloudPointList.push_back(newString);
    }
}

void WebUDPParserService::update_ForacareData(int projectId, std::string ForacareDataString)
{
    mForacareDataString = ForacareDataString;
}

void WebUDPParserService::update_UnknownPositionTagString(int projectId, std::string UnknownPositionTagString)
{
    bool bFound = false;

    // vilslog::printf("WebUDPParserService::update_UnknownPositionTagString() UnknownPositionTagString[%s]\n", UnknownPositionTagString.c_str());

    for (std::list<UnknownPositionTagStringStruct*>::iterator existed = mUnknownPositionTagList.begin(); existed != mUnknownPositionTagList.end(); existed++)
    {
        if ( (*existed)->projectId == projectId )
        {
            (*existed)->UnknownPositionTagString = UnknownPositionTagString;
            bFound = true;
            // vilslog::printf("WebUDPParserService::update_UnknownPositionTagString() 1\n");
            break;
        }
    }
    if (!bFound)
    {
        UnknownPositionTagStringStruct * newString = new UnknownPositionTagStringStruct();
        newString->projectId = projectId;
        newString->UnknownPositionTagString = UnknownPositionTagString;
        mUnknownPositionTagList.push_back(newString);

        // vilslog::printf("WebUDPParserService::update_UnknownPositionTagString() 2\n");
    }
}

void WebUDPParserService::update_LocatorRSSIString(int projectId, std::string LoRAMacaddress, std::string strRSSIInfo)
{
    bool bFound = false;
    for (std::list<LocatorRSSIInfoStringStruct*>::iterator existed = mLocatorRSSIInfoList.begin(); existed != mLocatorRSSIInfoList.end(); existed++)
    {
        if ( ((*existed)->LoRAMacaddress.compare(LoRAMacaddress) == 0) && ((*existed)->projectId == projectId) )
        {
            (*existed)->LocatorRSSIInfoString = strRSSIInfo;
            bFound = true;
            break;
        }
    }
    if (!bFound)
    {
        LocatorRSSIInfoStringStruct * newRSSIString = new LocatorRSSIInfoStringStruct();
        newRSSIString->projectId = projectId;
        newRSSIString->LoRAMacaddress = LoRAMacaddress;
        newRSSIString->LocatorRSSIInfoString = strRSSIInfo;

        mLocatorRSSIInfoList.push_back(newRSSIString);
    }
}

void WebUDPParserService::sendSystemStatusToClient(UDPDataItem * item)
{
    //char * buf = item->getMsg();
    //Json::Value result;
    //bool parsingSuccessful = parse_payload_JSON((const char *)buf, result);

    // int projectid = 1;
    // if ( parsingSuccessful )
    // {
    //     projectid = getProjectId(result);
    // }

    std::ostringstream sendstream;
    sendstream<<"STATUS_NUM="<<mServiceStatusList.size() + 2<<endl;

    int index = 1;

/*
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    long sec = usage.ru_stime.tv_sec;
    sec += usage.ru_stime.tv_usec/1000000;

    int hour = (int)(sec / 3600);
    int min = (int)(sec / 60);
    int hr_min = hour * 60;
    int hr_sec = hour * 3600;
    min -= hr_min;
    int min_sec = min * 60;
    sec -= (hr_sec + min_sec);


    //int hour = sec/(60*60);
    // 1024 * 1024 = 1048576
    //long long ram = CurrentRAMUsed();
    //vilslog::printf("WebUDPParserService::sendSystemStatusToClient() ram[%lu KBytes]\n", ram/(1024));

    char msg[256];

    time_t cutTime = getCurrentTimeInSec();
    int sys_sec = (int)(cutTime - mSystemStartTime);

    int sys_hour = (int)(sys_sec / 3600);
    int sys_min = (int)(sys_sec / 60);
    int sys_hr_min = sys_hour * 60;
    int sys_hr_sec = sys_hour * 3600;
    sys_min -= sys_hr_min;
    int sys_min_sec = sys_min * 60;
    sys_sec -= (sys_hr_sec + sys_min_sec);
*/

    getSystemRunTime(mSystemInfo, mSystemStartTime);

    char msg[256];

    snprintf(msg, 256, "System RunTime[%02d:%02d:%02d]", mSystemInfo.SystemRunHour, mSystemInfo.SystemRunMin, mSystemInfo.SystemRunSec);
    sendstream<<"STATUS_"<<index<<"="<<msg<<endl;;
    index++;

    snprintf(msg, 256, "MemoryUsage[%d MBytes] CPU RunTime[%02d:%02d:%02d]", mSystemInfo.MemoryUsage, mSystemInfo.CPURunHour, mSystemInfo.CPURunMin, mSystemInfo.CPURunSec);
    sendstream<<"STATUS_"<<index<<"="<<msg<<endl;;
    index++;

    for (std::list<StatusStruct*>::iterator existed = mServiceStatusList.begin(); existed != mServiceStatusList.end(); existed++)
    {
        sendstream<<"STATUS_"<<index<<"="<<(*existed)->serviceString<<":initial:"<<(*existed)->status;
        if (!(*existed)->status)
            sendstream<<" Msg:" << (*existed)->statusString;
        sendstream<<endl;
        index++;
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    // mWebUDPServer->sendToClient(sendBuf, (int)sendString.length());
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendSingleTagStatusToClient(UDPDataItem * item)
{
    //int projectid = item->getProjectid();
    int projectid = 1;

    std::string macaddress = "";

    std::string bufString = item->getMsg();
    std::string::size_type pos = bufString.find('=');
    if (pos != std::string::npos)
    {
        macaddress = bufString.substr(pos + 1);

        vilslog::printf("WebUDPParserService::sendSingleTagStatusToClient() projectid[%d] macaddress[%s]\n", projectid, macaddress.c_str());
    }
    std::ostringstream sendstream;

    NodeStruct * tagnode = mDeviceManager->get_Tag_node_ByMacaddress(projectid, macaddress.c_str());
    if (tagnode != NULL)
    {
        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        std::string MaplayerName = "";
        if (baseDb != NULL)
            MaplayerName = baseDb->fetchMaplayerName(tagnode->maplayer);

        sendstream << "{";
        sendstream << "\"nodename\":\""<<tagnode->nodename<<"\"";
        sendstream << ",\"alive\":"<<tagnode->alive;
        if (tagnode->Opertaion_Mode == 1 || tagnode->Opertaion_Mode == 2)
        {
            // 1:UWB Ranging 2:LoRa Ranging
            sendstream << ",\"uwb\":\"("<<tagnode->posX<<","<<tagnode->posY<<","<<tagnode->posZ<<")\"";
            sendstream << ",\"gps\":\"(0,0)\"";
        }
        else
        if (tagnode->Opertaion_Mode == 3)
        {
            // GPS
            sendstream << ",\"uwb\":\"(0,0,0)\"";
            sendstream << ",\"gps\":\"("<<tagnode->Latitude<<","<<tagnode->Longitude<<")\"";
        }
        sendstream << ",\"maplayer\":\""<< MaplayerName <<"\"";
        sendstream << ",\"areaname\":\""<< tagnode->AreaName <<"\"";
        sendstream << ",\"datetime\":\""<< time_tToString(tagnode->updateTimestamp/1000) <<"\"";
        sendstream << "}";

    }
    else
    {
        sendstream << "[]";
    }
    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

//
// send tag status to web
// tagstatus=start_index,count
void WebUDPParserService::sendTagStatusToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON((const char *)buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        int groupid = 3;
        int start_index = 0;
        int count = 0;
        std::string maingroups;
        std::string nodeid;

        if ( result.isMember("start") )
            start_index = atoi(result["start"].asCString());
        if ( result.isMember("count") )
            count = atoi(result["count"].asCString());
        if ( result.isMember("groupid") )
            groupid = convert_string_to_int(result["groupid"].asString());

        if ( result.isMember("maingroups") )
        {
            maingroups = result["maingroups"].asString();
        }

        if ( result.isMember("nodeid") )
        {
            nodeid = result["nodeid"].asString();
        }

        if (nodeid.length() > 0)
        {
            sendSingleTagStatusToClient(item, projectid, groupid, maingroups, nodeid);
        }
        else
        if ( start_index == 0 && count == 0 )
        {
            sendAllTagStatusToClient(item, projectid, groupid, maingroups);
        }
        else
        {
            vilslog::printf("WebUDPParserService::sendTagStatusToClient->sendTagStatusToClient\n");
            sendTagStatusToClient(item, projectid, groupid, maingroups, start_index, count);
        }
    }
}

void WebUDPParserService::sendTagStatusToClient(UDPDataItem * item, int projectid, int groupid, std::string maingroups, int start_index, int count)
{
    // local connected node list
    std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);

    //
    // filter by maingroups
    //
    std::list<NodeStruct*> * grouptagNodeList = new std::list<NodeStruct*>();
    std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');

    for (std::list<NodeStruct*>::iterator existed = tagNodeList->begin(); existed != tagNodeList->end(); existed++)
    {
        for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
        {
            int maingroupid = convert_string_to_int(*maingroup);

            if ((*existed)->maingroupid == maingroupid || groupid == 1 || groupid == 2)
            {
                grouptagNodeList->push_back((*existed));
                break;
            }
        }
    }

    std::ostringstream sendstream;
    if (grouptagNodeList != NULL)
    {
        //vilslog::printf("WebUDPParserService::sendTagStatusToClient() start_index[%d] count[%d] tagNodeList->size()[%d]\n", start_index, count, (int)tagNodeList->size());

        if ( (start_index+count) > (int)grouptagNodeList->size())
        {
            count = (int)grouptagNodeList->size() - start_index;
            // vilslog::printf("WebUDPParserService::sendTagStatusToClient() new count[%d]\n", count);
        }
        sendstream<<"LOC_TAG_NUM="<<count<<endl;
        dump_stream_nodelist_info(projectid, grouptagNodeList, sendstream, std::string("LOC_TAG"), start_index, count);

        delete grouptagNodeList;
    }
    else
    {
        sendstream<<"LOC_TAG_NUM=0"<<endl;
    }
    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    vilslog::printf("WebUDPParserService::sendTagStatusToClient sendBuf->%s sendString_length->%d\n", sendBuf, sendString.length());
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

//
// send tag status to web
//
void WebUDPParserService::sendAllTagStatusToClient(UDPDataItem * item, int projectid, int groupid, std::string maingroups)
{
    // local connected node list
    std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);

    //
    // filter by maingroups
    //
    std::list<NodeStruct*> * grouptagNodeList = new std::list<NodeStruct*>();
    std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');
    for (std::list<NodeStruct*>::iterator existed = tagNodeList->begin(); existed != tagNodeList->end(); existed++)
    {
        for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
        {
            int maingroupid = convert_string_to_int(*maingroup);

            if ((*existed)->maingroupid == maingroupid || groupid == 1 || groupid == 2)
            {
                grouptagNodeList->push_back((*existed));
                break;
            }
        }
    }

    std::ostringstream sendstream;
    if (grouptagNodeList != NULL)
    {
        sendstream<<"LOC_TAG_NUM="<<grouptagNodeList->size()<<endl;
        dump_stream_nodelist_info(projectid, grouptagNodeList, sendstream, std::string("LOC_TAG"));

        delete grouptagNodeList;
    }
    else
    {
        sendstream<<"LOC_TAG_NUM=0"<<endl;
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    vilslog::printf("WebUDPParserService::sendAllTagStatusToClient: sendBuf->%s sendString_length->%d \n", sendBuf, sendString.length());
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendSingleTagStatusToClient(UDPDataItem * item, int projectid, int groupid, std::string maingroups, std::string nodeid)
{
    // local connected node list
    std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);

    //
    // filter by maingroups
    //
    std::list<NodeStruct*> * grouptagNodeList = new std::list<NodeStruct*>();
    std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');

    for (std::list<NodeStruct*>::iterator existed = tagNodeList->begin(); existed != tagNodeList->end(); existed++)
    {
        for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
        {
            int maingroupid = convert_string_to_int(*maingroup);

            if ((*existed)->maingroupid == maingroupid || groupid == 1 || groupid == 2)
            {
                if ( strcmp((*existed)->nodeid, nodeid.c_str()) == 0 )
                    grouptagNodeList->push_back((*existed));
                break;
            }
        }
    }

    std::ostringstream sendstream;
    if (grouptagNodeList != NULL)
    {
        int count = (int)grouptagNodeList->size();
        sendstream<<"LOC_TAG_NUM="<<count<<endl;
        dump_stream_nodelist_info(projectid, grouptagNodeList, sendstream, std::string("LOC_TAG"), 0, count);

        delete grouptagNodeList;
    }
    else
    {
        sendstream<<"LOC_TAG_NUM=0"<<endl;
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    vilslog::printf("WebUDPParserService::sendSingleTagStatusToClient: sendBuf->%s sendString_length->%d \n", sendBuf, sendString.length());
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

//
// send tag status to web
// tagstatus=start_index,count
void WebUDPParserService::sendTagRollCallStatusToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON((const char *)buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        int start_index = 0;
        int count = 0;
        std::string areaid = "";

        if ( result.isMember("start") )
            start_index = atoi(result["start"].asCString());
        if ( result.isMember("count") )
            count = atoi(result["count"].asCString());
        if ( result.isMember("areaid") )
            areaid = result["areaid"].asString();

        if ( start_index == 0 && count == 0 && areaid.length() == 0)
        {
            sendAllTagRollCallStatusToClient(item, projectid);
        }
        else
        {
            sendTagRollCallStatusToClient(item, projectid, start_index, count, areaid);
        }
    }
}

void WebUDPParserService::sendTagRollCallStatusToClient(UDPDataItem * item, int projectid, int start_index, int count, std::string areaid)
{
    // local connected node list
    std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);

    std::ostringstream sendstream;
    if (tagNodeList != NULL)
    {
        if ( (start_index+count) > (int)tagNodeList->size())
        {
            count = (int)tagNodeList->size() - start_index;

            vilslog::printf("WebUDPParserService::sendTagRollCallStatusToClient() new count[%d]\n", count);
        }
        sendstream<<"LOC_TAG_NUM="<<count<<endl;
        dump_stream_tagrollcall_info(tagNodeList, sendstream, std::string("LOC_TAG"), start_index, count, areaid);
    }
    else
    {
        sendstream<<"LOC_TAG_NUM=0"<<endl;
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    //mWebUDPServer->sendToClient(sendBuf, (int)sendString.length());
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendAllTagRollCallStatusToClient(UDPDataItem * item, int projectid)
{
    // local connected node list
    std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);

    std::ostringstream sendstream;
    if (tagNodeList != NULL)
    {
        sendstream<<"LOC_TAG_NUM="<<tagNodeList->size()<<endl;
        dump_stream_tagrollcall_info(tagNodeList, sendstream, std::string("LOC_TAG"));
    }
    else
    {
        sendstream<<"LOC_TAG_NUM=0"<<endl;
    }
    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    //mWebUDPServer->sendToClient(sendBuf, (int)sendString.length());
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendTagRollCallPdfReportToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON((const char *)buf, result);

    std::ostringstream sendstream;

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        std::string areaid = "";

        if ( result.isMember("areaid") )
            areaid = result["areaid"].asString();

        vilslog::printf("WebUDPParserService::sendTagRollCallPdfReportToClient() projectid[%d] areaid[%s]\n", projectid, areaid.c_str());

        if(areaid.length() == 0)
        {
            sendstream<<"{\"pdfreport\":\"\",\"success\":\"false\"}";
        }
        else
        {
            PDFTableDataStruct pdfDataRollCallRecord1;
            pdfDataRollCallRecord1.type = "rollcall";
            pdfDataRollCallRecord1.startdate = TodayDateType1();
            pdfDataRollCallRecord1.enddate = TodayDateType1();

            PDFTableDataStruct pdfDataRollCallRecord2;
            pdfDataRollCallRecord2.type = "rollcall";
            pdfDataRollCallRecord2.startdate = TodayDateType1();
            pdfDataRollCallRecord2.enddate = TodayDateType1();

            pdfDataRollCallRecord1.headerlist.push_back(create_pdf_headerdata("應到", 100));
            pdfDataRollCallRecord1.headerlist.push_back(create_pdf_headerdata("實到", 100));
            pdfDataRollCallRecord1.headerlist.push_back(create_pdf_headerdata("未到", 100));
            pdfDataRollCallRecord1.headerlist.push_back(create_pdf_headerdata("點名時間", 100));

            pdfDataRollCallRecord2.headerlist.push_back(create_pdf_headerdata("狀態", 50));
            pdfDataRollCallRecord2.headerlist.push_back(create_pdf_headerdata("人數", 50));
            pdfDataRollCallRecord2.headerlist.push_back(create_pdf_headerdata("人員名單", 300));


            std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);

            int total_count = (int)tagNodeList->size();
            int in_count = 0;
            int out_count = 0;
            std::ostringstream stream_in;
            std::ostringstream stream_out;

            for (auto node = tagNodeList->begin(); node != tagNodeList->end(); node++)
            {
                if ( (areaid.compare((*node)->AreaId) == 0) || (areaid.compare((*node)->AreaType5Id) == 0) )
                {
                    if ( ((*node)->alive == 0) && ((*node)->asleep == 0) )
                    {
                        stream_out<<(*node)->nodename<<"  ";
                        out_count++;
                    }
                    else
                    {
                        stream_in<<(*node)->nodename<<"  ";
                        in_count++;
                    }
                }
                else
                {
                    stream_out<<(*node)->nodename<<"  ";
                    out_count++;
                }
            }

            PDFTableRowDataStruct * newRowData = new PDFTableRowDataStruct();
            newRowData->coldatalist.push_back(create_pdf_columndata("應到", std::to_string(total_count)));
            newRowData->coldatalist.push_back(create_pdf_columndata("實到", std::to_string(in_count)));
            newRowData->coldatalist.push_back(create_pdf_columndata("未到", std::to_string(out_count)));
            newRowData->coldatalist.push_back(create_pdf_columndata("點名時間", currentDateTimeForSQL()));
            pdfDataRollCallRecord1.rowdatalist.push_back(newRowData);

            PDFTableRowDataStruct * newRowData_out = new PDFTableRowDataStruct();
            newRowData_out->coldatalist.push_back(create_pdf_columndata("狀態", "實到"));
            newRowData_out->coldatalist.push_back(create_pdf_columndata("人數", std::to_string(in_count)));
            newRowData_out->coldatalist.push_back(create_pdf_columndata("人員名單", stream_in.str()));
            pdfDataRollCallRecord2.rowdatalist.push_back(newRowData_out);

            PDFTableRowDataStruct * newRowData_in = new PDFTableRowDataStruct();
            newRowData_in->coldatalist.push_back(create_pdf_columndata("狀態", "未到"));
            newRowData_in->coldatalist.push_back(create_pdf_columndata("人數", std::to_string(out_count)));
            newRowData_in->coldatalist.push_back(create_pdf_columndata("人員名單", stream_out.str()));
            pdfDataRollCallRecord2.rowdatalist.push_back(newRowData_in);

            // std::string randomFilename = random_string(16);

            std::string pdffilename = "/upload/";
            // pdffilename.append(randomFilename);
            pdffilename.append("電子點名報告");
            pdffilename.append("_");
            pdffilename.append(currentDateTimeForReport());
            pdffilename.append(".pdf");

            std::string pdfOutputfile = OUTPUT_DIRECTORY;
            pdfOutputfile.append(pdffilename);

            vilslog::printf("WebUDPParserService::sendTagRollCallPdfReportToClient() pdfOutputfile[%s]\n", pdfOutputfile.c_str());

            std::string report_date = TodayDateType1();
            std::string report_start_date = report_date;
            std::string report_end_date = report_date;

            MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
            std::string areaName = areaid;
            if (baseDb != NULL)
                areaName = baseDb->fetchAreaName(areaid);

            vilslog::printf("WebUDPParserService::sendTagRollCallPdfReportToClient() projectid[%d] areaName[%s]\n", projectid, areaName.c_str());

            PDFCreater * pdfCreater = new PDFCreater();
            pdfCreater->create_report_header(pdfOutputfile, "電子點名報告", report_date, report_start_date, report_end_date);
            pdfCreater->create_report_userinfo("區域", areaName);
            pdfCreater->create_report_table(&pdfDataRollCallRecord1);
            pdfCreater->create_report_table(&pdfDataRollCallRecord2);
            pdfCreater->generate_report();

            delete pdfCreater;

            // put to remove queue
            QRCodeStruct * newQRCcode = new QRCodeStruct();
            newQRCcode->filepath = pdfOutputfile;
            newQRCcode->createtime = getCurrentTimeInSec();
            mQRCodeImageList.push_back(newQRCcode);

            sendstream<<"{\"pdfreport\":\"";
            sendstream<<pdffilename;
            sendstream<<"\",\"success\":\"true\"}";
        }
    }
    else
    {
        sendstream<<"{\"pdfreport\":\"\",\"success\":\"false\"}";
    }
    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendTagRollCallXlsxReportToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON((const char *)buf, result);

    std::ostringstream sendstream;

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        std::string areaid = "";

        if ( result.isMember("areaid") )
            areaid = result["areaid"].asString();

        vilslog::printf("WebUDPParserService::sendTagRollCallXlsxReportToClient() projectid[%d] areaid[%s]\n", projectid, areaid.c_str());

        if(areaid.length() == 0)
        {
            sendstream<<"{\"xlsxreport\":\"\",\"success\":\"false\"}";
        }
        else
        {
            std::string report_date = TodayDateType1();
            std::string report_start_date = report_date;
            std::string report_end_date = report_date;

            std::string xlsxfilename = "/upload/";
            xlsxfilename.append("電子點名報告");
            xlsxfilename.append("_");
            xlsxfilename.append(currentDateTimeForReport());
            xlsxfilename.append(".xlsx");

            std::string xlsxOutputfile = OUTPUT_DIRECTORY;
            xlsxOutputfile.append(xlsxfilename);

            vilslog::printf("WebUDPParserService::sendTagRollCallXlsxReportToClient() xlsxOutputfile[%s]\n", xlsxOutputfile.c_str());

            std::ostringstream stream_report_date_range;
            stream_report_date_range<<report_start_date<<" 至 "<<report_end_date;

            MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
            std::string areaName = areaid;
            if (baseDb != NULL)
                areaName = baseDb->fetchAreaName(areaid);

            std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);

            int total_count = (int)tagNodeList->size();
            int in_count = 0;
            int out_count = 0;
            std::ostringstream stream_in;
            std::ostringstream stream_out;

            for (auto node = tagNodeList->begin(); node != tagNodeList->end(); node++)
            {
                if ( (areaid.compare((*node)->AreaId) == 0) || (areaid.compare((*node)->AreaType5Id) == 0) )
                {
                    if ( ((*node)->alive == 0) && ((*node)->asleep == 0) )
                    {
                        stream_out<<(*node)->nodename<<"  ";
                        out_count++;
                    }
                    else
                    {
                        stream_in<<(*node)->nodename<<"  ";
                        in_count++;
                    }
                }
                else
                {
                    stream_out<<(*node)->nodename<<"  ";
                    out_count++;
                }
            }

            OpenXLSX::XLDocument xlsxReport;
            xlsxReport.CreateDocument(xlsxOutputfile);
            auto wks1 = xlsxReport.Workbook().Worksheet("Sheet1");

            wks1.Cell("A1").Value() = "電子點名報告";
            wks1.Cell("A2").Value() = "報告產生日期";
            wks1.Cell("B2").Value() = report_date;
            wks1.Cell("A3").Value() = "報告資料區間";
            wks1.Cell("B3").Value() = stream_report_date_range.str();
            wks1.Cell("A4").Value() = "區域";
            wks1.Cell("B4").Value() = areaName;

            wks1.Cell("A6").Value() = "應到";
            wks1.Cell("A7").Value() = std::to_string(total_count);
            wks1.Cell("B6").Value() = "實到";
            wks1.Cell("B7").Value() = std::to_string(in_count);
            wks1.Cell("C6").Value() = "未到";
            wks1.Cell("C7").Value() = std::to_string(out_count);
            wks1.Cell("D6").Value() = "點名時間";
            wks1.Cell("D7").Value() = currentDateTimeForSQL();

            wks1.Cell("B9").Value() = "實到人員名單";
            wks1.Cell("C9").Value() = "未到人員名單";

            in_count = 10;
            out_count = 10;
            for (auto node = tagNodeList->begin(); node != tagNodeList->end(); node++)
            {
                if ( (areaid.compare((*node)->AreaId) == 0) || (areaid.compare((*node)->AreaType5Id) == 0) )
                {
                    if ( ((*node)->alive == 0) && ((*node)->asleep == 0) )
                    {
                        std::string col = "C";
                        col.append(std::to_string(out_count));
                        wks1.Cell(col).Value() = (*node)->nodename;

                        out_count++;
                    }
                    else
                    {
                        std::string col = "B";
                        col.append(std::to_string(in_count));
                        wks1.Cell(col).Value() = (*node)->nodename;

                        in_count++;
                    }
                }
                else
                {
                    std::string col = "C";
                    col.append(std::to_string(out_count));
                    wks1.Cell(col).Value() = (*node)->nodename;

                    out_count++;
                }
            }


            xlsxReport.SaveDocument();
            xlsxReport.CloseDocument();

            // put to remove queue
            QRCodeStruct * newQRCcode = new QRCodeStruct();
            newQRCcode->filepath = xlsxOutputfile;
            newQRCcode->createtime = getCurrentTimeInSec();
            mQRCodeImageList.push_back(newQRCcode);

            sendstream<<"{\"xlsxreport\":\"";
            sendstream<<xlsxfilename;
            sendstream<<"\",\"success\":\"true\"}";
        }
    }
    else
    {
        sendstream<<"{\"xlsxreport\":\"\",\"success\":\"false\"}";
    }
    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendUserInfoXlsxReportToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON((const char *)buf, result);

    std::ostringstream sendstream;

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        std::list<std::string> targetuserlist;
        std::string groupid;

        if ( result.isMember("users") )
        {
            if ( result["users"].isArray() )
            {
                for(int i=0;i<(int)result["users"].size();i++)
                {
                    Json::Value userid = result["users"][i];
                    targetuserlist.push_back(userid.asString());
                    // vilslog::printf("WebUDPParserService::sendUserInfoXlsxReportToClient() projectid[%d] userid[%s]\n", projectid, userid.asCString());
                }
            }
        }
        if ( result.isMember("groupid") )
            groupid = result["groupid"].asString();

        bool enableShowMainGroup = false;
        if ( (groupid.compare("1") == 0) || (groupid.compare("2") == 0) ||(groupid.compare("4") == 0) )
        {
            enableShowMainGroup = true;
        }

        // vilslog::printf("WebUDPParserService::sendUserInfoXlsxReportToClient() projectid[%d] targetuserlist.size()[%d]\n", projectid, (int)targetuserlist.size());

        std::string report_date = TodayDateType1();
        std::string report_start_date = report_date;
        std::string report_end_date = report_date;

        std::string xlsxfilename = "/upload/";
        xlsxfilename.append("人員列表");
        xlsxfilename.append("_");
        xlsxfilename.append(currentDateTimeForReport());
        xlsxfilename.append(".xlsx");

        std::string xlsxOutputfile = OUTPUT_DIRECTORY;
        xlsxOutputfile.append(xlsxfilename);

        vilslog::printf("WebUDPParserService::sendUserInfoXlsxReportToClient() projectid[%d] xlsxOutputfile[%s]\n", projectid, xlsxOutputfile.c_str());

        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        std::list<UserStruct*> * userList = NULL;
        std::list<UserGroupStruct*> * userGroupList = NULL;
        std::list<UserSubGroupStruct*> * userSubGroupList = NULL;
        std::list<UserMainGroupStruct*> * userMainGroupList = NULL;
        if (baseDb != NULL)
        {
            userList = baseDb->get_Users();
            userGroupList = baseDb->get_UserGroups();
        	userSubGroupList = baseDb->get_UserSubGroups();
            userMainGroupList = baseDb->get_UserMainGroups();
        }

        OpenXLSX::XLDocument xlsxReport;
        xlsxReport.CreateDocument(xlsxOutputfile);
        auto wks1 = xlsxReport.Workbook().Worksheet("Sheet1");
        wks1.Cell("A1").Value() = "名稱";
        wks1.Cell("B1").Value() = "帳號";
        wks1.Cell("C1").Value() = "職稱";
        wks1.Cell("D1").Value() = "功能權限群組";
        wks1.Cell("E1").Value() = "資料權限群組";
        wks1.Cell("F1").Value() = "子集合";
        wks1.Cell("G1").Value() = "RFID卡片";
        wks1.Cell("H1").Value() = "NFC卡片";
        wks1.Cell("I1").Value() = "UWB卡片";
        if (enableShowMainGroup)
        {
            wks1.Cell("J1").Value() = "主集合";
            wks1.Cell("K1").Value() = "傳感器";
        }
        else
        {
            wks1.Cell("J1").Value() = "傳感器";
        }

        if (userList != NULL)
        {
            int count = 2;
            for (std::list<std::string>::iterator useridIt = targetuserlist.begin(); useridIt != targetuserlist.end(); useridIt++)
            {
                int targetuserid = convert_string_to_int((*useridIt));

                for (std::list<UserStruct*>::iterator user = userList->begin(); user != userList->end(); user++)
                {
                    if ( targetuserid == (*user)->userid )
                    {
                        // vilslog::printf("WebUDPParserService::sendUserInfoXlsxReportToClient() targetuserid[%d]\n", targetuserid);

                        std::string colName = "A";
                        colName.append(std::to_string(count));
                        wks1.Cell(colName).Value() = (*user)->name;

                        colName = "B";
                        colName.append(std::to_string(count));
                        wks1.Cell(colName).Value() = (*user)->account;

                        colName = "C";
                        colName.append(std::to_string(count));
                        wks1.Cell(colName).Value() = (*user)->title;

                        colName = "D";
                        colName.append(std::to_string(count));
                        wks1.Cell(colName).Value() = convert_usergroup_id_to_name(userGroupList, (*user)->groupid);

                        colName = "E";
                        colName.append(std::to_string(count));
                        std::string datagroupnames = convert_subgroup_id_to_name(userSubGroupList, (*user)->datagroups);
                        if ( (*user)->datagroups->size() > 0 )
                            datagroupnames.append(",自己");
                        else
                            datagroupnames.append("自己");
                        wks1.Cell(colName).Value() = datagroupnames;

                        colName = "F";
                        colName.append(std::to_string(count));
                        wks1.Cell(colName).Value() = convert_subgroup_id_to_name(userSubGroupList, (*user)->subgroups);

                        colName = "G";
                        colName.append(std::to_string(count));
                        wks1.Cell(colName).Value() = convert_list_to_string((*user)->rfidcards);

                        colName = "H";
                        colName.append(std::to_string(count));
                        wks1.Cell(colName).Value() = convert_list_to_string((*user)->nfccards);

                        colName = "I";
                        colName.append(std::to_string(count));
                        wks1.Cell(colName).Value() = convert_list_to_string((*user)->uwbcards);

                        if (enableShowMainGroup)
                        {
                            colName = "J";
                            colName.append(std::to_string(count));
                            wks1.Cell(colName).Value() = convert_maingroup_id_to_name(userMainGroupList, (*user)->maingroups);

                            colName = "K";
                            colName.append(std::to_string(count));
                            wks1.Cell(colName).Value() = convert_list_to_string((*user)->sensors);
                        }
                        else
                        {
                            colName = "J";
                            colName.append(std::to_string(count));
                            wks1.Cell(colName).Value() = convert_list_to_string((*user)->sensors);
                        }

                        count++;

                        break;
                    }
                }
            }
        }

        xlsxReport.SaveDocument();
        xlsxReport.CloseDocument();

        if (userList != NULL)
        {
            clear_UserStruct_list(userList);
            delete userList;
        }
        if (userGroupList != NULL)
        {
            clear_UserGroupStruct_list(userGroupList);
            delete userGroupList;
        }
        if (userSubGroupList != NULL)
        {
            clear_UserSubGroupStruct_list(userSubGroupList);
            delete userSubGroupList;
        }

        targetuserlist.clear();

        // put to remove queue
        QRCodeStruct * newQRCcode = new QRCodeStruct();
        newQRCcode->filepath = xlsxOutputfile;
        newQRCcode->createtime = getCurrentTimeInSec();
        mQRCodeImageList.push_back(newQRCcode);

        sendstream<<"{\"xlsxreport\":\"";
        sendstream<<xlsxfilename;
        sendstream<<"\",\"success\":\"true\"}";
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendUserRollCallResultXlsxReportToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON((const char *)buf, result);

    std::ostringstream sendstream;

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        std::string accountid;
        int rollcallid = 0;
        int eventid = 0;

        if ( result.isMember("accountid") )
            accountid = result["accountid"].asString();
        if ( result.isMember("rollcallid") )
            rollcallid = convert_string_to_int(result["rollcallid"].asString());
        if ( result.isMember("eventid") )
            eventid = convert_string_to_int(result["eventid"].asString());

        vilslog::printf("WebUDPParserService::sendUserRollCallResultXlsxReportToClient() accountid[%s] rollcallid[%d] eventid[%d]\n",
            accountid.c_str(), rollcallid, eventid);

        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if (baseDb != NULL)
        {
            std::list<UserStruct*> * userList = NULL;
            std::list<UserSubGroupStruct*> * userSubGroupList = NULL;

            userList = baseDb->get_Users();
            userSubGroupList = baseDb->get_UserSubGroups();

            // 1.取得此帳號可管理 人員資料
            std::list<UserStruct*> * rollcallUserList = getSubGroupUser(accountid, userList);

            vilslog::printf("WebUDPParserService::sendUserRollCallResultXlsxReportToClient() rollcallUserList->size()[%d]\n", (int)rollcallUserList->size());

            // 2.取得此帳號可管理 子集合資料
            std::list<UserSubGroupStruct*> * rollcallSubGroupList = getViewAccessSubGroup(accountid, userList, userSubGroupList);

            vilslog::printf("WebUDPParserService::sendUserRollCallResultXlsxReportToClient() rollcallSubGroupList->size()[%d]\n", (int)rollcallSubGroupList->size());

            // 3.取得點名排程資料
            CARDRollCallStruct * rollcallinfo = baseDb->read_CARDRollCallInfo(rollcallid);

            vilslog::printf("WebUDPParserService::sendUserRollCallResultXlsxReportToClient() rollcallname[%s] StartTime[%s] EndTime[%s]\n",
                rollcallinfo->rollcallname.c_str(), rollcallinfo->StartTime.c_str(), rollcallinfo->EndTime.c_str());

            // 4.取得點名事件紀錄
            CARDRollCallEventStruct * rollcallEvent = baseDb->get_CARDRollCallEvent(rollcallid, eventid);

            vilslog::printf("WebUDPParserService::sendUserRollCallResultXlsxReportToClient() rollcallid[%d] eventid[%d] StartDateTime[%s] EndDateTime[%s] totalcount[%d] presentcount[%d]\n",
                rollcallid, eventid, rollcallEvent->StartDateTime.c_str(), rollcallEvent->EndDateTime.c_str(), rollcallEvent->totalcount, rollcallEvent->presentcount);

            // 5.取得點名結果紀錄
            std::list<CARDRollCallRecordStruct*> * rollcallRecordList = baseDb->get_CARDRollCallRecords(rollcallid, eventid);

            vilslog::printf("WebUDPParserService::sendUserRollCallResultXlsxReportToClient() rollcallRecordList->size()[%d]\n", (int)rollcallRecordList->size());

            // 7.統計點名結果
            std::list<USERRollCallRecordStruct*> * userRollCallList = getUserRollCallList(rollcallRecordList);

            vilslog::printf("WebUDPParserService::sendUserRollCallResultXlsxReportToClient() userRollCallList->size()[%d]\n", (int)userRollCallList->size());

            std::string filename = create_UserRollCallResultXlsxReport(projectid, rollcallinfo, rollcallSubGroupList, rollcallEvent, rollcallUserList, userRollCallList);

            if ( filename.length() > 0 )
            {
                sendstream<<"{\"xlsxreport\":\"";
                sendstream<<filename;
                sendstream<<"\",\"success\":\"true\"}";
            }
            else
            {
                sendstream<<"{\"xlsxreport\":\"\",\"success\":\"false\"}";
            }

            rollcallUserList->clear();
            delete rollcallUserList;
            rollcallSubGroupList->clear();
            delete rollcallSubGroupList;

            if (rollcallinfo != NULL)
            {
                rollcallinfo->rfiddeviceids.clear();
                rollcallinfo->nfclocators.clear();
                rollcallinfo->subgroups.clear();
                rollcallinfo->weekday.clear();
                rollcallinfo->validdate.clear();
                rollcallinfo->invaliddate.clear();
                delete rollcallinfo;
            }

            if (rollcallEvent != NULL)
                delete rollcallEvent;

            if (rollcallRecordList != NULL)
            {
                clear_CARDRollCallRecordStruct_list(rollcallRecordList);
                delete rollcallRecordList;
            }

            clear_USERRollCallRecordStruct_list(userRollCallList);
            delete userRollCallList;

            if (userList != NULL)
            {
                clear_UserStruct_list(userList);
                delete userList;
            }
            if (userSubGroupList != NULL)
            {
                clear_UserSubGroupStruct_list(userSubGroupList);
                delete userSubGroupList;
            }
        }
        else
        {
            sendstream<<"{\"xlsxreport\":\"\",\"success\":\"false\"}";
        }
    }
    else
    {
        sendstream<<"{\"xlsxreport\":\"\",\"success\":\"false\"}";
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

std::string WebUDPParserService::create_UserRollCallResultXlsxReport(int projectid, CARDRollCallStruct * rollcallinfo, std::list<UserSubGroupStruct*> * rollcallSubGroupList, CARDRollCallEventStruct * rollcallEvent, std::list<UserStruct*> * rollcallUserList, std::list<USERRollCallRecordStruct*> *userRollCallList)
{
    std::string report_date = TodayDateType1();
    std::string report_start_date = report_date;
    std::string report_end_date = report_date;

    std::string xlsxfilename = "/upload/";
    xlsxfilename.append("人員點名結果");
    xlsxfilename.append("_");
    xlsxfilename.append(currentDateTimeForReport());
    xlsxfilename.append(".xlsx");

    std::string xlsxOutputfile = OUTPUT_DIRECTORY;
    xlsxOutputfile.append(xlsxfilename);

    vilslog::printf("WebUDPParserService::create_UserRollCallResultXlsxReport() projectid[%d] xlsxOutputfile[%s]\n", projectid, xlsxOutputfile.c_str());

    OpenXLSX::XLDocument xlsxReport;
    xlsxReport.CreateDocument(xlsxOutputfile);

    xlsxReport.Workbook().AddWorksheet("點名紀錄");
    xlsxReport.Workbook().AddWorksheet("點名未到");
    xlsxReport.Workbook().DeleteSheet("Sheet1");

    auto wks1 = xlsxReport.Workbook().Worksheet("點名紀錄");
    wks1.Cell("A1").Value() = "排程名稱";
    wks1.Cell("B1").Value() = "點名開始時間";
    wks1.Cell("C1").Value() = "點名結束時間";
    wks1.Cell("D1").Value() = "應到總數";
    wks1.Cell("E1").Value() = "實到總數";
    wks1.Cell("F1").Value() = "未到總數";

    wks1.Cell("A2").Value() = rollcallEvent->rollcallname;
    wks1.Cell("B2").Value() = rollcallEvent->StartDateTime;
    wks1.Cell("C2").Value() = rollcallEvent->EndDateTime;
    wks1.Cell("D2").Value() = rollcallEvent->totalcount;
    wks1.Cell("E2").Value() = rollcallEvent->presentcount;
    wks1.Cell("F2").Value() = rollcallEvent->totalcount - rollcallEvent->presentcount;

    auto wks_notpresent = xlsxReport.Workbook().Worksheet("點名未到");
    wks_notpresent.Cell("A1").Value() = "子集合";
    wks_notpresent.Cell("B1").Value() = "應到總數";
    wks_notpresent.Cell("C1").Value() = "實到總數";
    wks_notpresent.Cell("D1").Value() = "未到總數";
    wks_notpresent.Cell("E1").Value() = "未到人員";

    int gl_all_subgroup_totalcount = 0;
    int gl_all_subgroup_presentcount = 0;
    int gl_all_subgroup_absentcount = 0;

    int group_row = 2;
    for (std::list<int>::iterator subgroupid = rollcallinfo->subgroups.begin(); subgroupid != rollcallinfo->subgroups.end(); subgroupid++)
    {
        for (std::list<UserSubGroupStruct*>::iterator subgroup = rollcallSubGroupList->begin(); subgroup != rollcallSubGroupList->end(); subgroup++)
        {
            if ( (*subgroup)->subgroupid == (*subgroupid) )
            {
                // 產生 子集合 sheet
                if ( !xlsxReport.Workbook().SheetExists((*subgroup)->subgroupname))
                {
                    xlsxReport.Workbook().AddWorksheet((*subgroup)->subgroupname);
                    auto wks = xlsxReport.Workbook().Worksheet((*subgroup)->subgroupname);
                    wks.Cell("A1").Value() = "序號";
                    wks.Cell("B1").Value() = "使用者";
                    wks.Cell("C1").Value() = "帳號";
                    wks.Cell("D1").Value() = "點名結果";
                    wks.Cell("E1").Value() = "點名時間";
                }

                auto wks = xlsxReport.Workbook().Worksheet((*subgroup)->subgroupname);

                int subgroup_presentcount = 0;
                int subgroup_absentcount = 0;
                int subgroup_totalcount = 0;

                //
                // 產生 子集合 點名資料
                //
                int user_index = 2;
                for (std::list<USERRollCallRecordStruct*>::iterator record = userRollCallList->begin(); record != userRollCallList->end(); record++)
                {
                    // 確定此使用者 為此子集合
                    bool valid = false;
                    UserStruct * targetuser = NULL;
                    for (std::list<UserStruct*>::iterator user = rollcallUserList->begin(); user != rollcallUserList->end(); user++)
                    {
                        if ( (*record)->userid == (*user)->userid )
                        {
                            for (std::list<std::string>::iterator user_subgroup = (*user)->subgroups->begin(); user_subgroup != (*user)->subgroups->end(); user_subgroup++)
                            {
                                int int_user_subgroup = convert_string_to_int(*user_subgroup);
                                if ( int_user_subgroup == (*subgroupid) )
                                {
                                    targetuser = (*user);
                                    valid = true;
                                    break;
                                }
                            }
                            break;
                        }
                    }

                    if (valid)
                    {
                        std::string colName = "A";
                        colName.append(std::to_string(user_index));
                        wks.Cell(colName).Value() = user_index - 1;

                        colName = "B";
                        colName.append(std::to_string(user_index));
                        wks.Cell(colName).Value() = targetuser->name;

                        colName = "C";
                        colName.append(std::to_string(user_index));
                        wks.Cell(colName).Value() = targetuser->account;

                        colName = "D";
                        colName.append(std::to_string(user_index));
                        if ((*record)->present == 1)
                            wks.Cell(colName).Value() = "點名已到";
                        else
                            wks.Cell(colName).Value() = "點名未到";

                        colName = "E";
                        colName.append(std::to_string(user_index));
                        wks.Cell(colName).Value() = (*record)->datetime;

                        user_index++;

                        if ((*record)->present == 1)
                        {
                            gl_all_subgroup_presentcount += 1;
                            subgroup_presentcount += 1;
                        }
                        else
                        {
                            //
                            // 點名未到 人員
                            //
                            wks_notpresent.Cell(group_row, 5 + subgroup_absentcount).Value() = targetuser->name;

                            gl_all_subgroup_absentcount += 1;
                            subgroup_absentcount += 1;
                        }

                        gl_all_subgroup_totalcount++;
                        subgroup_totalcount++;
                    }
                }

                //
                // 產生 點名未到 子集合紀錄
                //
                std::string colName = "A";
                colName.append(std::to_string(group_row));
                wks_notpresent.Cell(colName).Value() = (*subgroup)->subgroupname;

                colName = "B";
                colName.append(std::to_string(group_row));
                wks_notpresent.Cell(colName).Value() = subgroup_totalcount;

                colName = "C";
                colName.append(std::to_string(group_row));
                wks_notpresent.Cell(colName).Value() = subgroup_presentcount;

                colName = "D";
                colName.append(std::to_string(group_row));
                wks_notpresent.Cell(colName).Value() = subgroup_absentcount;

                group_row++;
                break;
            }
        }
    }

    wks1.Cell("D2").Value() = gl_all_subgroup_totalcount;
    wks1.Cell("E2").Value() = gl_all_subgroup_presentcount;
    wks1.Cell("F2").Value() = gl_all_subgroup_absentcount;

    xlsxReport.SaveDocument();
    xlsxReport.CloseDocument();

    return xlsxfilename;
}


void WebUDPParserService::sendUnknowNFCCardsXlsxReportToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON((const char *)buf, result);

    std::ostringstream sendstream;

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        std::string report_date = TodayDateType1();
        std::string report_start_date = report_date;
        std::string report_end_date = report_date;

        std::string xlsxfilename = "/upload/";
        xlsxfilename.append("NFC未註冊卡片");
        xlsxfilename.append("_");
        xlsxfilename.append(currentDateTimeForReport());
        xlsxfilename.append(".xlsx");

        std::string xlsxOutputfile = OUTPUT_DIRECTORY;
        xlsxOutputfile.append(xlsxfilename);

        vilslog::printf("WebUDPParserService::sendUnknowNFCCardsXlsxReportToClient() projectid[%d] xlsxOutputfile[%s]\n", projectid, xlsxOutputfile.c_str());

        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if ( baseDb != NULL)
        {
            std::list<NFCCardStruct*> * unknownNFCCardList = baseDb->get_UnknownNFCCards();

            OpenXLSX::XLDocument xlsxReport;
            xlsxReport.CreateDocument(xlsxOutputfile);
            auto wks1 = xlsxReport.Workbook().Worksheet("Sheet1");
            wks1.Cell("A1").Value() = "NFC卡片";
            wks1.Cell("B1").Value() = "時間";

            int count = 2;
            for (std::list<NFCCardStruct*>::iterator card = unknownNFCCardList->begin(); card != unknownNFCCardList->end(); card++)
            {
                std::string colName = "A";
                colName.append(std::to_string(count));
                wks1.Cell(colName).Value() = (*card)->cardid;

                colName = "B";
                colName.append(std::to_string(count));
                wks1.Cell(colName).Value() = (*card)->updatetime;

                count++;
            }

            xlsxReport.SaveDocument();
            xlsxReport.CloseDocument();

            clear_NFCCardStruct_list(unknownNFCCardList);
            delete unknownNFCCardList;

            // put to remove queue
            QRCodeStruct * newQRCcode = new QRCodeStruct();
            newQRCcode->filepath = xlsxOutputfile;
            newQRCcode->createtime = getCurrentTimeInSec();
            mQRCodeImageList.push_back(newQRCcode);

            sendstream<<"{\"xlsxreport\":\"";
            sendstream<<xlsxfilename;
            sendstream<<"\",\"success\":\"true\"}";
        }
        else
        {
            sendstream<<"{\"xlsxreport\":\"\"";
            sendstream<<"\",\"success\":\"false\"}";
        }
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendUnknowUHFCardsXlsxReportToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON((const char *)buf, result);

    std::ostringstream sendstream;

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        std::string report_date = TodayDateType1();
        std::string report_start_date = report_date;
        std::string report_end_date = report_date;

        std::string xlsxfilename = "/upload/";
        xlsxfilename.append("UHF未註冊卡片");
        xlsxfilename.append("_");
        xlsxfilename.append(currentDateTimeForReport());
        xlsxfilename.append(".xlsx");

        std::string xlsxOutputfile = OUTPUT_DIRECTORY;
        xlsxOutputfile.append(xlsxfilename);

        vilslog::printf("WebUDPParserService::sendUnknowUHFCardsXlsxReportToClient() projectid[%d] xlsxOutputfile[%s]\n", projectid, xlsxOutputfile.c_str());

        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if ( baseDb != NULL)
        {
            std::list<RFIDCardStruct*> * unknownRFIDCardList = baseDb->get_UnknownRFIDCards();

            OpenXLSX::XLDocument xlsxReport;
            xlsxReport.CreateDocument(xlsxOutputfile);
            auto wks1 = xlsxReport.Workbook().Worksheet("Sheet1");
            wks1.Cell("A1").Value() = "UHF卡片";
            wks1.Cell("B1").Value() = "時間";

            int count = 2;
            for (std::list<RFIDCardStruct*>::iterator card = unknownRFIDCardList->begin(); card != unknownRFIDCardList->end(); card++)
            {
                std::string colName = "A";
                colName.append(std::to_string(count));
                wks1.Cell(colName).Value() = (*card)->cardid;

                colName = "B";
                colName.append(std::to_string(count));
                wks1.Cell(colName).Value() = (*card)->updatetime;

                count++;
            }

            xlsxReport.SaveDocument();
            xlsxReport.CloseDocument();

            clear_RFIDCardStruct_list(unknownRFIDCardList);
            delete unknownRFIDCardList;

            // put to remove queue
            QRCodeStruct * newQRCcode = new QRCodeStruct();
            newQRCcode->filepath = xlsxOutputfile;
            newQRCcode->createtime = getCurrentTimeInSec();
            mQRCodeImageList.push_back(newQRCcode);

            sendstream<<"{\"xlsxreport\":\"";
            sendstream<<xlsxfilename;
            sendstream<<"\",\"success\":\"true\"}";
        }
        else
        {
            sendstream<<"{\"xlsxreport\":\"\"";
            sendstream<<"\",\"success\":\"false\"}";
        }
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

//
// send anchor status to web
//
void WebUDPParserService::sendAnchorStatusToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON((const char *)buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        int groupid = 3;
        int start_index = 0;
        int count = 0;

        if ( result.isMember("start") )
            start_index = atoi(result["start"].asCString());
        if ( result.isMember("count") )
            count = atoi(result["count"].asCString());
        if ( result.isMember("groupid") )
            groupid = convert_string_to_int(result["groupid"].asString());

        std::string maingroups;
        if ( result.isMember("maingroups") )
        {
            maingroups = result["maingroups"].asString();
        }

        if ( start_index == 0 && count == 0 )
        {
            sendAllAnchorStatusToClient(item, projectid, groupid, maingroups);
        }
        else
        {
            sendAnchorStatusToClient(item, projectid, groupid, maingroups, start_index, count);
        }
    }
}

void WebUDPParserService::sendAnchorStatusToClient(UDPDataItem * item, int projectid, int groupid, std::string maingroups, int start_index, int count)
{
    // local connected node list
    std::list<NodeStruct*> * anchorNodeList = mDeviceManager->get_Anchor_list(projectid);

    //
    // filter by maingroups
    //
    std::list<NodeStruct*> * groupNodeList = new std::list<NodeStruct*>();
    std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');
    for (std::list<NodeStruct*>::iterator existed = anchorNodeList->begin(); existed != anchorNodeList->end(); existed++)
    {
        for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
        {
            int maingroupid = convert_string_to_int(*maingroup);
            if ((*existed)->maingroupid == maingroupid || groupid == 1 || groupid == 2)
            {
                groupNodeList->push_back((*existed));
                break;
            }
        }
    }

    std::ostringstream sendstream;
    if(groupNodeList != NULL)
    {
        if ( (start_index+count) > (int)groupNodeList->size())
        {
            count = (int)groupNodeList->size() - start_index;
            //printf("WebUDPParserService::sendAnchorStatusToClient() new count[%d]\n", count);
        }
        sendstream<<"LOC_ANCHOR_NUM="<<count<<endl;
        dump_stream_nodelist_info(projectid, groupNodeList, sendstream, std::string("LOC_ANCHOR"), start_index, count);

        delete groupNodeList;
    }
    else
    {
        sendstream<<"LOC_ANCHOR_NUM=0"<<endl;
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

//
// send anchor status to web
//
void WebUDPParserService::sendAllAnchorStatusToClient(UDPDataItem * item, int projectid, int groupid, std::string maingroups)
{
    // local connected node list
    std::list<NodeStruct*> * anchorNodeList = mDeviceManager->get_Anchor_list(projectid);

    //
    // filter by maingroups
    //
    std::list<NodeStruct*> * groupNodeList = new std::list<NodeStruct*>();
    std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');
    for (std::list<NodeStruct*>::iterator existed = anchorNodeList->begin(); existed != anchorNodeList->end(); existed++)
    {
        for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
        {
            int maingroupid = convert_string_to_int(*maingroup);

            if ((*existed)->maingroupid == maingroupid || groupid == 1 || groupid == 2)
            {
                groupNodeList->push_back((*existed));
                break;
            }
        }
    }

    std::ostringstream sendstream;
    if (groupNodeList != NULL)
    {
        sendstream<<"LOC_ANCHOR_NUM="<<groupNodeList->size()<<endl;
        dump_stream_nodelist_info(projectid, groupNodeList, sendstream, std::string("LOC_ANCHOR"));

        delete groupNodeList;
    }
    else
    {
        sendstream<<"LOC_ANCHOR_NUM=0"<<endl;
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

//
// send coordinator status to web
//
void WebUDPParserService::sendCoordStatusToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON((const char *)buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        int groupid = 3;
        int start_index = 0;
        int count = 0;

        if ( result.isMember("start") )
            start_index = atoi(result["start"].asCString());
        if ( result.isMember("count") )
            count = atoi(result["count"].asCString());
        if ( result.isMember("groupid") )
            groupid = convert_string_to_int(result["groupid"].asString());

        std::string maingroups;
        if ( result.isMember("maingroups") )
        {
            maingroups = result["maingroups"].asString();
        }

        if ( start_index == 0 && count == 0 )
        {
            sendAllCoordStatusToClient(item, projectid, groupid, maingroups);
        }
        else
        {
            sendCoordStatusToClient(item, projectid, groupid, maingroups, start_index, count);
        }
    }
}

void WebUDPParserService::sendCoordStatusToClient(UDPDataItem * item, int projectid, int groupid, std::string maingroups, int start_index, int count)
{
    // local connected node list
    std::list<NodeStruct*> * coordNodeList = mDeviceManager->get_Coordinator_list(projectid);

    //
    // filter by maingroups
    //
    std::list<NodeStruct*> * groupNodeList = new std::list<NodeStruct*>();
    std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');
    for (std::list<NodeStruct*>::iterator existed = coordNodeList->begin(); existed != coordNodeList->end(); existed++)
    {
        for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
        {
            int maingroupid = convert_string_to_int(*maingroup);

            if ((*existed)->maingroupid == maingroupid || groupid == 1 || groupid == 2)
            {
                groupNodeList->push_back((*existed));
                break;
            }
        }
    }

    std::ostringstream sendstream;
    if (groupNodeList != NULL)
    {
        if ( (start_index+count) > (int)groupNodeList->size())
        {
            count = (int)groupNodeList->size() - start_index;
            //printf("WebUDPParserService::sendCoordStatusToClient() new count[%d]\n", count);
        }
        sendstream<<"LOC_COORD_NUM="<<count<<endl;
        dump_stream_nodelist_info(projectid, groupNodeList, sendstream, std::string("LOC_COORD"), start_index, count);

        delete groupNodeList;
    }
    else
    {
        sendstream<<"LOC_COORD_NUM=0"<<endl;
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

//
// send coordinator status to web
//
void WebUDPParserService::sendAllCoordStatusToClient(UDPDataItem * item, int projectid, int groupid, std::string maingroups)
{
    // local connected node list
    std::list<NodeStruct*> * coordNodeList = mDeviceManager->get_Coordinator_list(projectid);

    //
    // filter by maingroups
    //
    std::list<NodeStruct*> * groupNodeList = new std::list<NodeStruct*>();
    std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');
    for (std::list<NodeStruct*>::iterator existed = coordNodeList->begin(); existed != coordNodeList->end(); existed++)
    {
        for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
        {
            int maingroupid = convert_string_to_int(*maingroup);
            if ((*existed)->maingroupid == maingroupid || groupid == 1 || groupid == 2)
            {
                groupNodeList->push_back((*existed));
                break;
            }
        }
    }

    std::ostringstream sendstream;
    if (groupNodeList != NULL)
    {
        sendstream<<"LOC_COORD_NUM="<<groupNodeList->size()<<endl;
        dump_stream_nodelist_info(projectid, groupNodeList, sendstream, std::string("LOC_COORD"));

        delete groupNodeList;
    }
    else
    {
        sendstream<<"LOC_COORD_NUM=0"<<endl;
    }
    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

//
// send locator status to web
//
void WebUDPParserService::sendLocatorStatusToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        int groupid = 3;
        int start_index = 0;
        int count = 0;
        std::string nodeid;

        // vilslog::printf("WebUDPParserService::sendLocatorStatusToClient() projectid[%d]\n", projectid);

        if ( result.isMember("start") )
            start_index = atoi(result["start"].asCString());
        if ( result.isMember("count") )
            count = atoi(result["count"].asCString());
        if ( result.isMember("groupid") )
            groupid = convert_string_to_int(result["groupid"].asString());
        if ( result.isMember("nodeid") )
            nodeid = result["nodeid"].asString();

        std::string maingroups;
        if ( result.isMember("maingroups") )
        {
            maingroups = result["maingroups"].asString();
        }

        // vilslog::printf("WebUDPParserService::sendLocatorStatusToClient() start_index[%d] count[%d] groupid[%d] maingroups[%s]\n",
        //     start_index, count, groupid, maingroups.c_str());

        if ( nodeid.length() > 0 )
        {
            sendSingleLocatorStatusToClient(item, projectid, groupid, maingroups, nodeid);
        }
        else
        if ( start_index == 0 && count == 0 )
        {
            sendAllLocatorStatusToClient(item, projectid, groupid, maingroups);
        }
        else
        {
            sendLocatorStatusToClient(item, projectid, groupid, maingroups, start_index, count);
        }

        // vilslog::printf("WebUDPParserService::sendLocatorStatusToClient() end\n");

    }
}

void WebUDPParserService::sendLocatorStatusToClient(UDPDataItem * item, int projectid, int groupid, std::string maingroups, int start_index, int count)
{

    // local connected node list
    std::list<NodeStruct*> * locatorNodeList = mDeviceManager->get_Locator_list(projectid);

    //
    // filter by maingroups
    //
    std::list<NodeStruct*> * groupNodeList = new std::list<NodeStruct*>();
    std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');
    for (std::list<NodeStruct*>::iterator existed = locatorNodeList->begin(); existed != locatorNodeList->end(); existed++)
    {
        for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
        {
            int maingroupid = convert_string_to_int(*maingroup);

            if ((*existed)->maingroupid == maingroupid || groupid == 1 || groupid == 2)
            {
                groupNodeList->push_back((*existed));
                break;
            }
        }
    }

    std::ostringstream sendstream;
    if (groupNodeList != NULL)
    {
        if ( (start_index+count) > (int)groupNodeList->size())
        {
            count = (int)groupNodeList->size() - start_index;
            //printf("WebUDPParserService::sendLocatorStatusToClient() new count[%d]\n", count);
        }
        sendstream<<"LOC_LOCATOR_NUM="<<count<<endl;
        dump_stream_nodelist_info(projectid, groupNodeList, sendstream, std::string("LOC_LOCATOR"), start_index, count);

        delete groupNodeList;
    }
    else
    {
        sendstream<<"LOC_LOCATOR_NUM=0"<<endl;
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

//
// send locator status to web
//
void WebUDPParserService::sendAllLocatorStatusToClient(UDPDataItem * item, int projectid, int groupid, std::string maingroups)
{
    // local connected node list
    std::list<NodeStruct*> * locatorNodeList = mDeviceManager->get_Locator_list(projectid);

    //
    // filter by maingroups
    //
    std::list<NodeStruct*> * groupNodeList = new std::list<NodeStruct*>();
    std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');

    for (std::list<NodeStruct*>::iterator existed = locatorNodeList->begin(); existed != locatorNodeList->end(); existed++)
    {
        for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
        {
            int maingroupid = convert_string_to_int(*maingroup);

            if ((*existed)->maingroupid == maingroupid || groupid == 1 || groupid == 2)
            {
                groupNodeList->push_back((*existed));
                break;
            }
        }
    }

    std::ostringstream sendstream;
    if (groupNodeList != NULL)
    {
        sendstream<<"LOC_LOCATOR_NUM="<<groupNodeList->size()<<endl;
        dump_stream_nodelist_info(projectid, groupNodeList, sendstream, std::string("LOC_LOCATOR"));

        delete groupNodeList;
    }
    else
    {
        sendstream<<"LOC_LOCATOR_NUM=0"<<endl;
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendSingleLocatorStatusToClient(UDPDataItem * item, int projectid, int groupid, std::string maingroups, std::string nodeid)
{
    // local connected node list
    std::list<NodeStruct*> * locatorNodeList = mDeviceManager->get_Locator_list(projectid);

    //
    // filter by maingroups
    //
    std::list<NodeStruct*> * groupNodeList = new std::list<NodeStruct*>();
    std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');

    for (std::list<NodeStruct*>::iterator existed = locatorNodeList->begin(); existed != locatorNodeList->end(); existed++)
    {
        for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
        {
            int maingroupid = convert_string_to_int(*maingroup);

            if ((*existed)->maingroupid == maingroupid || groupid == 1 || groupid == 2)
            {
                if ( strcmp((*existed)->nodeid, nodeid.c_str()) == 0 )
                    groupNodeList->push_back((*existed));
                break;
            }
        }
    }

    std::ostringstream sendstream;
    if (groupNodeList != NULL)
    {
        sendstream<<"LOC_LOCATOR_NUM="<<groupNodeList->size()<<endl;
        dump_stream_nodelist_info(projectid, groupNodeList, sendstream, std::string("LOC_LOCATOR"));

        delete groupNodeList;
    }
    else
    {
        sendstream<<"LOC_LOCATOR_NUM=0"<<endl;
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

//
// send AoA status to web
//
void WebUDPParserService::sendAoAStatusToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        int groupid = 3;
        int start_index = 0;
        int count = 0;

        if ( result.isMember("start") )
            start_index = atoi(result["start"].asCString());
        if ( result.isMember("count") )
            count = atoi(result["count"].asCString());
        if ( result.isMember("groupid") )
            groupid = convert_string_to_int(result["groupid"].asString());

        std::string maingroups;
        if ( result.isMember("maingroups") )
        {
            maingroups = result["maingroups"].asString();
        }

        if ( start_index == 0 && count == 0 )
        {
            sendAllAoAStatusToClient(item, projectid, groupid, maingroups);
        }
        else
        {
            sendAoAStatusToClient(item, projectid, groupid, maingroups, start_index, count);
        }
    }
}

void WebUDPParserService::sendAoAStatusToClient(UDPDataItem * item, int projectid, int groupid, std::string maingroups, int start_index, int count)
{
    std::list<AoANodeStruct*> * aoaNodeList = mDeviceManager->get_AoA_list(projectid);

    //
    // filter by maingroups
    //
    std::list<AoANodeStruct*> * groupNodeList = new std::list<AoANodeStruct*>();
    std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');

    for (std::list<AoANodeStruct*>::iterator existed = aoaNodeList->begin(); existed != aoaNodeList->end(); existed++)
    {
        for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
        {
            int maingroupid = convert_string_to_int(*maingroup);

            if ((*existed)->maingroupid == maingroupid || groupid == 1 || groupid == 2)
            {
                groupNodeList->push_back((*existed));
                break;
            }
        }
    }

    std::ostringstream sendstream;
    if (groupNodeList != NULL)
    {
        if ( (start_index+count) > (int)groupNodeList->size())
        {
            count = (int)groupNodeList->size() - start_index;
        }

        sendstream<<"LOC_AOA_NUM="<<count<<endl;
        dump_stream_aoalist_info(projectid, groupNodeList, sendstream, std::string("LOC_AOA"), start_index, count);

        delete groupNodeList;
    }
    else
    {
        sendstream<<"LOC_AOA_NUM=0"<<endl;
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

//
// send AoA status to web
//
void WebUDPParserService::sendAllAoAStatusToClient(UDPDataItem * item, int projectid, int groupid, std::string maingroups)
{
    std::list<AoANodeStruct*> * aoaNodeList = mDeviceManager->get_AoA_list(projectid);

    //
    // filter by maingroups
    //
    std::list<AoANodeStruct*> * groupNodeList = new std::list<AoANodeStruct*>();
    std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');
    for (std::list<AoANodeStruct*>::iterator existed = aoaNodeList->begin(); existed != aoaNodeList->end(); existed++)
    {
        for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
        {
            int maingroupid = convert_string_to_int(*maingroup);

            if ((*existed)->maingroupid == maingroupid || groupid == 1 || groupid == 2)
            {
                groupNodeList->push_back((*existed));
                break;
            }
        }
    }

    std::ostringstream sendstream;
    if (groupNodeList != NULL)
    {
        sendstream<<"LOC_AOA_NUM="<<groupNodeList->size()<<endl;
        dump_stream_aoalist_info(projectid, groupNodeList, sendstream, std::string("LOC_AOA"));

        delete groupNodeList;
    }
    else
    {
        sendstream<<"LOC_AOA_NUM=0"<<endl;
    }
    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

//
// send lora gateway status to web
//
void WebUDPParserService::sendLoRaGatewayStatusToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    int projectid = 1;
    int groupid = 3;
    std::string maingroups = "1";
    if ( parsingSuccessful )
    {
        projectid = getProjectId(result);

        if ( result.isMember("maingroups") )
            maingroups = result["maingroups"].asString();

        if ( result.isMember("groupid") )
            groupid = convert_string_to_int(result["groupid"].asString());
    }

    std::list<LoRaGatewayInfoStruct*> * LoRaGatewayList = mDeviceManager->get_LoRaGateway_list(projectid);

    //
    // filter by maingroups
    //
    std::list<LoRaGatewayInfoStruct*> * groupNodeList = new std::list<LoRaGatewayInfoStruct*>();
    std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');
    for (std::list<LoRaGatewayInfoStruct*>::iterator existed = LoRaGatewayList->begin(); existed != LoRaGatewayList->end(); existed++)
    {
        for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
        {
            int maingroupid = convert_string_to_int(*maingroup);

            if ((*existed)->maingroupid == maingroupid || groupid == 1 || groupid == 2)
            {
                groupNodeList->push_back((*existed));
                break;
            }
        }
    }

// {"00001c497bf88362":{"gwid":"00001c497bf88362","PROJECT_ID":"1","sf":"10","gwip":"10.1.1.14","repeater":"00000000ffffffff","channel":"922800000"}}

    std::ostringstream sendstream;

    if (groupNodeList != NULL)
    {
        sendstream<<"{";
        for (std::list<LoRaGatewayInfoStruct *>::iterator gateway = groupNodeList->begin(); gateway != groupNodeList->end(); gateway++)
        {
            if (gateway != groupNodeList->begin())
                sendstream<<",";

            std::string channel = convertListToString(&(*gateway)->channels);

            sendstream<<"\""<<(*gateway)->gwid<<"\":";

            sendstream<<"{";
            sendstream<<"\"alive\":\""<<(*gateway)->alive<<"\",";
            sendstream<<"\"ping\":\""<<(*gateway)->pingCount<<"\",";
            sendstream<<"\"mqtt\":\""<<(*gateway)->MQTTalive<<"\",";
            sendstream<<"\"sf\":\""<<(*gateway)->sf<<"\",";
            sendstream<<"\"gwid\":\""<<(*gateway)->gwid<<"\",";
            sendstream<<"\"gwip\":\""<<(*gateway)->gwip<<"\",";
            sendstream<<"\"repeater\":\""<<(*gateway)->repeater<<"\",";
            sendstream<<"\"channel\":\""<<channel<<"\",";
            sendstream<<"\"maplayer\":\""<<(*gateway)->maplayer<<"\",";
            sendstream<<"\"posX\":\""<<(*gateway)->posX<<"\",";
            sendstream<<"\"posY\":\""<<(*gateway)->posY<<"\",";
            sendstream<<"\"posZ\":\""<<(*gateway)->posZ<<"\",";
            sendstream<<"\"verifykey\":\""<<(*gateway)->verifykey<<"\",";
            sendstream<<"\"issuetime\":\""<<(*gateway)->DataTime<<"\"";
            sendstream<<"}"<<endl;
        }
        sendstream<<"}"<<endl;

        delete groupNodeList;
    }
    else
    {
        sendstream<<"{}"<<endl;
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    vilslog::printf("WebUDPParserService::sendLoRaGatewayStatusToClient sendBuf->%s sendString_length->%d\n", sendBuf, sendString.length());
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendNFCDeviceStatusToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    // vilslog::printf("sendNFCDeviceStatusToClient() buf[%s]\n", buf);


    int projectid = 1;
    int groupid = 3;
    int start_index = 0;
    int count = 0;
    int showonline = 0;
    int showoffline = 0;
    std::string maingroups = "1";
    if ( parsingSuccessful )
    {
        projectid = getProjectId(result);

        if ( result.isMember("start") )
            start_index = result["start"].asInt();
        if ( result.isMember("count") )
            count = result["count"].asInt();
        if ( result.isMember("showonline") )
            showonline = result["showonline"].asInt();
        if ( result.isMember("showoffline") )
            showoffline = result["showoffline"].asInt();
        if ( result.isMember("groupid") )
            groupid = convert_string_to_int(result["groupid"].asString());
        if ( result.isMember("maingroups") )
            maingroups = result["maingroups"].asString();
    }

    // vilslog::printf("sendNFCDeviceStatusToClient() projectid[%d] start_index[%d] count[%d] maingroups[%s]\n",
    //     projectid, start_index, count, maingroups.c_str());

    std::list<NFCDeviceStruct*> * nfcDeciceList = mDeviceManager->get_NFCDevice_list(projectid);

    //
    // filter by maingroups
    //
    // bool enableShowMainGroup = false;
    // if ( (groupid.compare("1") == 0) || (groupid.compare("2") == 0) || (groupid.compare("4") == 0) )
    // {
    //     enableShowMainGroup = true;
    // }

    std::list<NFCDeviceStruct*> * groupNodeList = new std::list<NFCDeviceStruct*>();
    std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');
    for (std::list<NFCDeviceStruct*>::iterator existed = nfcDeciceList->begin(); existed != nfcDeciceList->end(); existed++)
    {
        for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
        {
            int maingroupid = convert_string_to_int(*maingroup);

            if ((*existed)->maingroupid == maingroupid || groupid == 1 || groupid == 2)
            {
                groupNodeList->push_back((*existed));
                break;
            }
        }
    }

    std::ostringstream sendstream;

    if (groupNodeList != NULL)
    {
        if (count == 0)
        {
            count = groupNodeList->size();
        }

        // vilslog::printf("sendNFCDeviceStatusToClient() projectid[%d]  nfcDeciceList->size()[%d] count[%d]\n", projectid, (int)nfcDeciceList->size(), count);

        int index = 0;
        int sendcount = 0;
        sendstream<<"{";
        for (std::list<NFCDeviceStruct *>::iterator nfcDecice = groupNodeList->begin(); nfcDecice != groupNodeList->end(); nfcDecice++)
        {
            if ( ((*nfcDecice)->deviceid < 0))
            {
                continue;
            }

            if ( showonline == 0 && (*nfcDecice)->alive != 0 )
            {
                continue;
            }
            if ( showoffline == 0 && (*nfcDecice)->alive == 0 )
            {
                continue;
            }

            // vilslog::printf("sendNFCDeviceStatusToClient() index[%d] start_index[%d] count[%d] sendcount[%d] deviceid[%d]\n",
            //     index, start_index, count, sendcount, (*nfcDecice)->deviceid);

            if ((index >= start_index) && (index < (start_index + count)))
            {
                if (sendcount > 0)
                    sendstream<<",";

                // vilslog::printf("sendNFCDeviceStatusToClient() add\n");

                sendstream<<"\""<<(*nfcDecice)->deviceid<<"\":";
                sendstream<<"{";
                sendstream<<"\"alive\":\""<<(*nfcDecice)->alive<<"\",";
                sendstream<<"\"deviceid\":\""<<(*nfcDecice)->deviceid<<"\",";
                sendstream<<"\"macaddress\":\""<<(*nfcDecice)->macaddress<<"\",";
                sendstream<<"\"name\":\""<<(*nfcDecice)->name<<"\",";
                sendstream<<"\"type\":\""<<(*nfcDecice)->type<<"\",";
                sendstream<<"\"gwid\":\""<<(*nfcDecice)->gwid<<"\",";
                sendstream<<"\"LoRAMacaddress\":\""<<(*nfcDecice)->LoRAMacaddress<<"\",";
                sendstream<<"\"ssid\":\""<<(*nfcDecice)->ssid<<"\",";
                sendstream<<"\"wifimac\":\""<<(*nfcDecice)->wifimac<<"\",";
                sendstream<<"\"version\":\""<<(*nfcDecice)->version<<"\",";
                sendstream<<"\"verifykey\":\""<<(*nfcDecice)->verifykey<<"\",";
                sendstream<<"\"datetime\":\""<<(*nfcDecice)->datetime<<"\"";
                sendstream<<"}"<<endl;
                sendcount++;
            }
            index++;
        }
        sendstream<<"}"<<endl;

        delete groupNodeList;
    }
    else
    {
        sendstream<<"{}"<<endl;
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();

    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);

}

void WebUDPParserService::sendNFCDeviceCountToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    int projectid = 1;
    int groupid = 3;
    std::string maingroups = "1";
    if ( parsingSuccessful )
    {
        projectid = getProjectId(result);

        if ( result.isMember("groupid") )
            groupid = convert_string_to_int(result["groupid"].asString());
        if ( result.isMember("maingroups") )
            maingroups = result["maingroups"].asString();
    }

    std::list<NFCDeviceStruct*> * nfcDeciceList = mDeviceManager->get_NFCDevice_list(projectid);

    std::list<NFCDeviceStruct*> * groupNodeList = new std::list<NFCDeviceStruct*>();
    std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');
    for (std::list<NFCDeviceStruct*>::iterator existed = nfcDeciceList->begin(); existed != nfcDeciceList->end(); existed++)
    {
        for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
        {
            int maingroupid = convert_string_to_int(*maingroup);

            if ((*existed)->maingroupid == maingroupid || groupid == 1 || groupid == 2)
            {
                groupNodeList->push_back((*existed));
                break;
            }
        }
    }

    std::ostringstream sendstream;

    sendstream<<"{\"count\":\""<<groupNodeList->size()<<"\"}";

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();

    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);

}

void WebUDPParserService::sendWIFIDeviceStatusToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    int projectid = 1;
    int groupid = 3;
    int start_index = 0;
    int count = 0;
    int showonline = 0;
    int showoffline = 0;

    std::string maingroups = "1";
    if ( parsingSuccessful )
    {
        projectid = getProjectId(result);

        if ( result.isMember("start") )
            start_index = result["start"].asInt();
        if ( result.isMember("count") )
            count = result["count"].asInt();
        if ( result.isMember("showonline") )
            showonline = result["showonline"].asInt();
        if ( result.isMember("showoffline") )
            showoffline = result["showoffline"].asInt();
        if ( result.isMember("groupid") )
            groupid = convert_string_to_int(result["groupid"].asString());
        if ( result.isMember("maingroups") )
            maingroups = result["maingroups"].asString();
    }

    std::list<NFCDeviceStruct*> * nfcDeciceList = mDeviceManager->get_NFCDevice_list(projectid);

    //
    // filter by maingroups
    //
    std::list<NFCDeviceStruct*> * groupNodeList = new std::list<NFCDeviceStruct*>();
    std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');

    for (std::list<NFCDeviceStruct*>::iterator existed = nfcDeciceList->begin(); existed != nfcDeciceList->end(); existed++)
    {
        for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
        {
            int maingroupid = convert_string_to_int(*maingroup);

            if ((*existed)->maingroupid == maingroupid || groupid == 1 || groupid == 2)
            {
                groupNodeList->push_back((*existed));
                break;
            }
        }
    }

    std::ostringstream sendstream;

    if (groupNodeList != NULL)
    {
        if (count == 0)
        {
            count = groupNodeList->size();
        }

        // vilslog::printf("sendNFCDeviceStatusToClient() projectid[%d]  nfcDeciceList->size()[%d] count[%d]\n", projectid, (int)nfcDeciceList->size(), count);

        int index = 0;
        int sendcount = 0;
        sendstream<<"{";
        for (std::list<NFCDeviceStruct *>::iterator nfcDecice = groupNodeList->begin(); nfcDecice != groupNodeList->end(); nfcDecice++)
        {
            if ( (*nfcDecice)->deviceid < 0 )
            {
                continue;
            }

            if ( showonline == 0 && (*nfcDecice)->alive != 0 )
            {
                continue;
            }
            if ( showoffline == 0 && (*nfcDecice)->alive == 0 )
            {
                continue;
            }

            if ( (*nfcDecice)->type.compare("WIFI") == 0 )
            {
                if (index >= start_index && index < (start_index + count))
                {
                    if (sendcount > 0)
                        sendstream<<",";

                    sendstream<<"\""<<(*nfcDecice)->deviceid<<"\":";
                    sendstream<<"{";
                    sendstream<<"\"alive\":\""<<(*nfcDecice)->alive<<"\",";
                    sendstream<<"\"deviceid\":\""<<(*nfcDecice)->deviceid<<"\",";
                    sendstream<<"\"macaddress\":\""<<(*nfcDecice)->macaddress<<"\",";
                    sendstream<<"\"name\":\""<<(*nfcDecice)->name<<"\",";
                    sendstream<<"\"type\":\""<<(*nfcDecice)->type<<"\",";
                    sendstream<<"\"ssid\":\""<<(*nfcDecice)->ssid<<"\",";
                    sendstream<<"\"wifimac\":\""<<(*nfcDecice)->wifimac<<"\",";
                    sendstream<<"\"version\":\""<<(*nfcDecice)->version<<"\",";
                    sendstream<<"\"verifykey\":\""<<(*nfcDecice)->verifykey<<"\",";
                    sendstream<<"\"datetime\":\""<<(*nfcDecice)->datetime<<"\"";
                    sendstream<<"}"<<endl;
                    sendcount++;
                }
                index++;
            }
        }
        sendstream<<"}"<<endl;

        delete groupNodeList;
    }
    else
    {
        sendstream<<"{}"<<endl;
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();

    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);

}

void WebUDPParserService::sendCELLDeviceStatusToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    int projectid = 1;
    int groupid = 3;
    int start_index = 0;
    int count = 0;
    int showonline = 0;
    int showoffline = 0;

    std::string maingroups = "1";
    if ( parsingSuccessful )
    {
        projectid = getProjectId(result);

        if ( result.isMember("start") )
            start_index = result["start"].asInt();
        if ( result.isMember("count") )
            count = result["count"].asInt();
        if ( result.isMember("showonline") )
            showonline = result["showonline"].asInt();
        if ( result.isMember("showoffline") )
            showoffline = result["showoffline"].asInt();
        if ( result.isMember("groupid") )
            groupid = convert_string_to_int(result["groupid"].asString());
        if ( result.isMember("maingroups") )
            maingroups = result["maingroups"].asString();
    }

    // local connected node list
    std::list<NodeStruct*> * locatorNodeList = mDeviceManager->get_Locator_list(projectid);

    //
    // filter by maingroups
    //
    std::list<NodeStruct*> * groupNodeList = new std::list<NodeStruct*>();
    std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');

    for (std::list<NodeStruct*>::iterator existed = locatorNodeList->begin(); existed != locatorNodeList->end(); existed++)
    {
        for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
        {
            int maingroupid = convert_string_to_int(*maingroup);

            if ((*existed)->maingroupid == maingroupid || groupid == 1 || groupid == 2)
            {
                groupNodeList->push_back((*existed));
                break;
            }
        }
    }

    std::ostringstream sendstream;
    if (groupNodeList != NULL)
    {
        if (count == 0)
        {
            count = groupNodeList->size();
        }
        int index = 0;
        int sendcount = 0;
        sendstream<<"{";
        for (std::list<NodeStruct *>::iterator node = groupNodeList->begin(); node != groupNodeList->end(); node++)
        {
            if ( (*node)->gwid.length() > 0 )
            {
                if (boost::iequals((*node)->LoRAMacaddress, (*node)->macaddress))
                {
                    // vilslog::printf("WebUDPParserService::sendCELLDeviceStatusToClient() (*node)->gwid[%s] LoRAMacaddress[%s] macaddress[%s]\n",
                    //     (*node)->gwid.c_str(), (*node)->LoRAMacaddress, (*node)->macaddress);

                    if ( showonline == 0 && (*node)->alive != 0 )
                    {
                        continue;
                    }
                    if ( showoffline == 0 && (*node)->alive == 0 )
                    {
                        continue;
                    }

                    if (index >= start_index && index < (start_index + count))
                    {
                        if (sendcount > 0)
                            sendstream<<",";

                        char Latitude[32];
                        char Longitude[32];
                        if ( (*node)->Latitude < 0.0 )
                            sprintf(Latitude,"%d", 0);
                        else
                            sprintf(Latitude,"%f", (*node)->Latitude);
                        if ( (*node)->Longitude < 0.0 )
                            sprintf(Longitude,"%d", 0);
                        else
                            sprintf(Longitude,"%f", (*node)->Longitude);

                        sendstream<<"\""<<(*node)->nodeid<<"\":";
                        sendstream<<"{";
                        sendstream<<"\"alive\":\""<<(*node)->alive<<"\",";
                        sendstream<<"\"nodeid\":\""<<(*node)->nodeid<<"\",";
                        sendstream<<"\"macaddress\":\""<<(*node)->macaddress<<"\",";
                        sendstream<<"\"name\":\""<<(*node)->nodename<<"\",";
                        sendstream<<"\"gwid\":\""<<(*node)->gwid<<"\",";
                        sendstream<<"\"panid\":\""<<(*node)->panid<<"\",";
                        sendstream<<"\"version\":\""<<(*node)->version<<"\",";
                        sendstream<<"\"mode\":\""<<(*node)->Opertaion_Mode<<"\","; // 1.累積傳送 2.循環 3.一般
                        sendstream<<"\"Longitude\":\""<<Longitude<<"\",";
                        sendstream<<"\"Latitude\":\""<<Latitude<<"\",";
                        sendstream<<"\"verifykey\":\""<<(*node)->verifykey<<"\"";
                        sendstream<<"}"<<endl;

                        sendcount++;
                    }
                    index++;
                }
            }
        }
        sendstream<<"}"<<endl;

        delete groupNodeList;
    }
    else
    {
        sendstream<<"{}"<<endl;
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();

    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);

}

void WebUDPParserService::sendEDGEDeviceStatusToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    int projectid = 1;
    int groupid = 3;
    int start_index = 0;
    int count = 0;
    int showonline = 0;
    int showoffline = 0;

    std::string maingroups = "1";
    if ( parsingSuccessful )
    {
        projectid = getProjectId(result);

        if ( result.isMember("start") )
            start_index = result["start"].asInt();
        if ( result.isMember("count") )
            count = result["count"].asInt();
        if ( result.isMember("showonline") )
            showonline = result["showonline"].asInt();
        if ( result.isMember("showoffline") )
            showoffline = result["showoffline"].asInt();
        if ( result.isMember("groupid") )
            groupid = convert_string_to_int(result["groupid"].asString());
        if ( result.isMember("maingroups") )
            maingroups = result["maingroups"].asString();
    }

    // local connected node list
    std::list<EDGEDeviceStruct*> * edgeDeviceList = mDeviceManager->get_EDGEDevice_list(projectid);

    //
    // filter by maingroups
    //
    std::list<EDGEDeviceStruct*> * groupNodeList = new std::list<EDGEDeviceStruct*>();
    std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');
    for (std::list<EDGEDeviceStruct*>::iterator existed = edgeDeviceList->begin(); existed != edgeDeviceList->end(); existed++)
    {
        for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
        {
            int maingroupid = convert_string_to_int(*maingroup);
            if ((*existed)->maingroupid == maingroupid || groupid == 1 || groupid == 2)
            {
                groupNodeList->push_back((*existed));
                break;
            }
        }
    }

    std::ostringstream sendstream;
    if (groupNodeList != NULL)
    {
        if (count == 0)
        {
            count = groupNodeList->size();
        }
        int index = 0;
        int sendcount = 0;
        sendstream<<"{";
        for (std::list<EDGEDeviceStruct *>::iterator device = groupNodeList->begin(); device != groupNodeList->end(); device++)
        {
            if ( (*device)->deviceid > 0 )
            {
                if ( showonline == 0 && (*device)->alive != 0 )
                {
                    continue;
                }
                if ( showoffline == 0 && (*device)->alive == 0 )
                {
                    continue;
                }

                if (index >= start_index && index < (start_index + count))
                {
                    if (sendcount > 0)
                        sendstream<<",";

                    sendstream<<"\""<<(*device)->deviceid<<"\":";
                    sendstream<<"{";
                    sendstream<<"\"alive\":\""<<(*device)->alive<<"\",";
                    sendstream<<"\"deviceid\":\""<<(*device)->deviceid<<"\",";
                    sendstream<<"\"macaddress\":\""<<(*device)->macaddress<<"\",";
                    sendstream<<"\"name\":\""<<(*device)->name<<"\",";
                    sendstream<<"\"type\":\""<<(*device)->type<<"\",";
                    sendstream<<"\"ip\":\""<<(*device)->ip<<"\",";
                    sendstream<<"\"gwid\":\""<<(*device)->gwid<<"\",";
                    sendstream<<"\"LoRAMacaddress\":\""<<(*device)->LoRAMacaddress<<"\",";
                    sendstream<<"\"version\":\""<<(*device)->version<<"\",";
                    sendstream<<"\"verifykey\":\""<<(*device)->verifykey<<"\"";
                    sendstream<<"}"<<endl;

                    sendcount++;
                }
                index++;
            }
        }
        sendstream<<"}"<<endl;

        delete groupNodeList;
    }
    else
    {
        sendstream<<"{}"<<endl;
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();

    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendSensorDeviceDataToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    int projectid = 1;
    std::string names = "";
    if ( parsingSuccessful )
    {
        projectid = getProjectId(result);

        if ( result.isMember("names") )
            names = result["names"].asString();
    }

    std::ostringstream sendstream;
    sendstream<<"[";

    int count = 0;
    std::list<std::string> * sensornameList = convert_string_to_list(names, ',');
    for (std::list<std::string>::iterator sensorname = sensornameList->begin(); sensorname != sensornameList->end(); sensorname++)
    {
        SensorDeviceStruct * sensorDevice = mDeviceManager->get_SensorDevice(projectid, (*sensorname));
        if (sensorDevice == NULL)
            continue;

        // vilslog::printf("WebUDPParserService::sendSensorDeviceDataToClient() sensorname[%s] value_buffer_size[%d]\n", (*sensorname).c_str(), sensorDevice->value_buffer_size);

        int sendCount = 0;
        //for(int i=0;i<sensorDevice->value_buffer_size;i++)
        for(int i=0;i<200 && i<sensorDevice->value_buffer_size;i++)
        {
            if ( count > 0 )
                sendstream<<",{";
            else
                sendstream<<"{";

            int dataindex = sensorDevice->value_buffer_index - sendCount - 1;
            if ( dataindex < 0 )
                dataindex = SENSOR_VALUE_BUFFER_SIZE - sendCount - 1;

            sendstream<<"\"name\""<<":\""<<(*sensorname)<<"\",";
            sendstream<<"\"value\""<<":\""<<sensorDevice->value_buffer[dataindex]<<"\",";
            sendstream<<"\"datetime\""<<":\""<<sensorDevice->value_datetime[dataindex]<<"\"";

            sendstream<<"}";
            sendCount++;
            count++;
        }
    }

    sendstream<<"]";
    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();

    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendSensorDeviceStatusToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    // vilslog::printf("WebUDPParserService::sendSensorDeviceStatusToClient buf->%s", buf);
    vilslog::printf("WebUDPParserService::sendSensorDeviceStatusToClient->IN \n");

    int projectid = 1;
    int groupid = 3;
    int start_index = 0;
    int count = 0;
    int showonline = 0;
    int showoffline = 0;

    std::string maingroups = "1";
    std::string nodeid = "";
    if ( parsingSuccessful )
    {
        projectid = getProjectId(result);

        if ( result.isMember("start") )
            start_index = result["start"].asInt();
        if ( result.isMember("count") )
            count = result["count"].asInt();
        if ( result.isMember("showonline") )
            showonline = result["showonline"].asInt();
        if ( result.isMember("showoffline") )
            showoffline = result["showoffline"].asInt();
        if ( result.isMember("groupid") )
            groupid = convert_string_to_int(result["groupid"].asString());
        if ( result.isMember("maingroups") )
            maingroups = result["maingroups"].asString();
        if ( result.isMember("nodeid") )
            nodeid = result["nodeid"].asString();
    }

    // local connected node list
    std::list<SensorDeviceStruct*> * sensorDeviceList = mDeviceManager->get_SensorDevice_list(projectid);

    // vilslog::printf("sendSensorDeviceStatusToClient() projectid[%d] sensorDeviceList->size()[%d] start_index[%d] count[%d] nodeid[%s]\n",
    //     projectid, (int)sensorDeviceList->size(), start_index, count, nodeid.c_str());

    //
    // filter by maingroups
    //
    std::list<SensorDeviceStruct*> * groupNodeList = new std::list<SensorDeviceStruct*>();
    std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');

    // vilslog::printf("sendSensorDeviceStatusToClient() maingroups->%s", maingroups);

    for (std::list<SensorDeviceStruct*>::iterator existed = sensorDeviceList->begin(); existed != sensorDeviceList->end(); existed++)
    {
        for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
        {
            int maingroupid = convert_string_to_int(*maingroup);

            if ((*existed)->maingroupid == maingroupid || (groupid == 1 || groupid == 2 ))
            {
                groupNodeList->push_back((*existed));
                break;
            }
        }
    }

    std::ostringstream sendstream;
    if (groupNodeList != NULL)
    {
        if (count == 0)
        {
            count = groupNodeList->size();
        }
        int index = 0;
        int sendcount = 0;
        sendstream<<"{";
        for (std::list<SensorDeviceStruct *>::iterator device = groupNodeList->begin(); device != groupNodeList->end(); device++)
        {
            if ( (*device)->id.length() )
            {
                if ( showonline == 0 && (*device)->alive != 0 )
                {
                    continue;
                }
                if ( showoffline == 0 && (*device)->alive == 0 )
                {
                    continue;
                }

                bool bValid = false;
                if (nodeid.length() > 0)
                {
                    if ( nodeid.compare((*device)->id) == 0 )
                    {
                        bValid = true;
                    }
                }
                else
                if (index >= start_index && index < (start_index + count))
                {
                    bValid = true;
                }

                if (bValid)
                {
                    if (sendcount > 0)
                        sendstream<<",";

                    std::string jsonvalue = (*device)->jsonvalue;
                    replaceAll(jsonvalue, "\"", "\\\"");

                    sendstream<<"\""<<(*device)->id<<"\":";
                    sendstream<<"{";
                    sendstream<<"\"alive\":\""<<(*device)->alive<<"\",";
                    sendstream<<"\"id\":\""<<(*device)->id<<"\",";
                    sendstream<<"\"name\":\""<<(*device)->name<<"\",";
                    sendstream<<"\"type\":\""<<(*device)->type<<"\",";
                    sendstream<<"\"Longitude\":\""<<(*device)->Longitude<<"\",";
                    sendstream<<"\"Latitude\":\""<<(*device)->Latitude<<"\",";
                    sendstream<<"\"Battery\":\""<<(*device)->Battery<<"\",";
                    sendstream<<"\"jsonvalue\":\""<<jsonvalue<<"\",";
                    sendstream<<"\"Version\":\""<<(*device)->Version<<"\",";
                    sendstream<<"\"unknownLocation\":\""<<(*device)->unknownLocation<<"\",";
                    sendstream<<"\"verifykey\":\""<<(*device)->verifykey<<"\",";
                    sendstream<<"\"datetime\":\""<<(*device)->datetime<<"\"";
                    sendstream<<"}"<<endl;

                    sendcount++;
                }
                index++;
            }

        }
        sendstream<<"}"<<endl;

        delete groupNodeList;
    }
    else
    {
        sendstream<<"{}"<<endl;
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();

    vilslog::printf("WebUDPParserService::sendSensorDeviceStatusToClient->sendBuf:%s\n", sendBuf);
    vilslog::printf("WebUDPParserService::sendSensorDeviceStatusToClient->sendBuf_langth:%d\n", sendString.length());
    vilslog::printf("WebUDPParserService::sendSensorDeviceStatusToClient->m_client_addr:%s\n", (struct sockaddr *)&item->m_client_addr);

    vilslog::printf("WebUDPParserService::sendSensorDeviceStatusToClient->OUT\n");

    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendSensorDeviceBaseInfoToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    vilslog::printf("WebUDPParserService::sendSensorDeviceBaseInfoToClient");

    int projectid = 1;
    int groupid = 3;
    std::string maingroups = "1";
    if ( parsingSuccessful )
    {
        projectid = getProjectId(result);

        if ( result.isMember("groupid") )
            groupid = convert_string_to_int(result["groupid"].asString());
        if ( result.isMember("maingroups") )
            maingroups = result["maingroups"].asString();
    }

    // local connected node list
    std::list<SensorDeviceStruct*> * sensorDeviceList = mDeviceManager->get_SensorDevice_list(projectid);

    //
    // filter by maingroups
    //
    std::list<SensorDeviceStruct*> * groupNodeList = new std::list<SensorDeviceStruct*>();
    std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');
    for (std::list<SensorDeviceStruct*>::iterator existed = sensorDeviceList->begin(); existed != sensorDeviceList->end(); existed++)
    {
        for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
        {
            int maingroupid = convert_string_to_int(*maingroup);

            if ((*existed)->maingroupid == maingroupid || (groupid == 1 || groupid == 2 ))
            {
                groupNodeList->push_back((*existed));
                break;
            }
        }
    }

    std::ostringstream sendstream;
    if (groupNodeList != NULL)
    {
        int sendcount = 0;
        sendstream<<"{";
        for (std::list<SensorDeviceStruct *>::iterator device = groupNodeList->begin(); device != groupNodeList->end(); device++)
        {
            if ( (*device)->id.length() )
            {
                if (sendcount > 0)
                    sendstream<<",";

                sendstream<<"\""<<(*device)->id<<"\":";
                sendstream<<"{";
                sendstream<<"\"name\":\""<<(*device)->name<<"\"";
                sendstream<<"}"<<endl;

                sendcount++;
            }
        }
        sendstream<<"}"<<endl;

        delete groupNodeList;
    }
    else
    {
        sendstream<<"{}"<<endl;
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();

    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendNFCCardStatusToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    int projectid = 1;
    int groupid = 3;
    int start_index = 0;
    int count = 0;
    int showonline = 0;
    int showoffline = 0;
    std::string maingroups = "1";

    if ( parsingSuccessful )
    {
        projectid = getProjectId(result);

        if ( result.isMember("start") )
            start_index = result["start"].asInt();
        if ( result.isMember("count") )
            count = result["count"].asInt();
        if ( result.isMember("showonline") )
            showonline = result["showonline"].asInt();
        if ( result.isMember("showoffline") )
            showoffline = result["showoffline"].asInt();
        if ( result.isMember("groupid") )
            groupid = convert_string_to_int(result["groupid"].asString());
        if ( result.isMember("maingroups") )
            maingroups = result["maingroups"].asString();
    }

    std::list<NFCCardStruct*> * nfccardList = mDeviceManager->get_NFCCard_list(projectid);

    //
    // filter by maingroups
    //
    std::list<NFCCardStruct*> * groupNodeList = new std::list<NFCCardStruct*>();
    std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');
    for (std::list<NFCCardStruct*>::iterator existed = nfccardList->begin(); existed != nfccardList->end(); existed++)
    {
        for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
        {
            int maingroupid = convert_string_to_int(*maingroup);

            if ((*existed)->maingroupid == maingroupid || groupid == 1 || groupid == 2)
            {
                groupNodeList->push_back((*existed));
                break;
            }
        }
    }

    // vilslog::printf("sendNFCCardStatusToClient() projectid[%d] nfccardList->size()[%d] start_index[%d] count[%d]\n",
    //     projectid, (int)nfccardList->size(), start_index, count);

    std::ostringstream sendstream;

    if (groupNodeList != NULL)
    {
        if (count == 0)
        {
            count = groupNodeList->size();
        }

        int index = 0;
        int sendcount = 0;
        sendstream<<"[";
        for (std::list<NFCCardStruct *>::iterator nfccard = groupNodeList->begin(); nfccard != groupNodeList->end(); nfccard++)
        {
            if ((*nfccard)->cardid.length() == 0)
            {
                continue;
            }

            if ( showonline == 0 && (*nfccard)->alive != 0 )
            {
                continue;
            }
            if ( showoffline == 0 && (*nfccard)->alive == 0 )
            {
                continue;
            }

            if (index >= start_index && index < (start_index + count))
            {
                if (sendcount > 0)
                    sendstream<<",";

                sendstream<<"{";
                sendstream<<"\"alive\":\""<<(*nfccard)->alive<<"\",";
                sendstream<<"\"cardid\":\""<<(*nfccard)->cardid<<"\",";
                sendstream<<"\"name\":\""<<(*nfccard)->name<<"\",";
                sendstream<<"\"type\":\""<<(*nfccard)->type<<"\",";
                sendstream<<"\"maplayer\":\""<<(*nfccard)->maplayer<<"\",";
                sendstream<<"\"userid\":\""<<(*nfccard)->userid<<"\",";
                sendstream<<"\"deviceid\":\""<<(*nfccard)->deviceid<<"\",";
                sendstream<<"\"datetime\":\""<<(*nfccard)->datetime<<"\",";
                sendstream<<"\"updatetime\":\""<<(*nfccard)->updatetime<<"\"";
                sendstream<<"}";
                sendcount++;
            }
            index++;
        }
        sendstream<<"]"<<endl;

        delete groupNodeList;
    }
    else
    {
        sendstream<<"[]"<<endl;
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();

    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);

}

void WebUDPParserService::sendRFIDDeviceStatusToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    int projectid = 1;
    int groupid = 3;
    int start_index = 0;
    int count = 0;
    int showonline = 0;
    int showoffline = 0;
    std::string maingroups = "1";

    if ( parsingSuccessful )
    {
        projectid = getProjectId(result);

        if ( result.isMember("start") )
            start_index = result["start"].asInt();
        if ( result.isMember("count") )
            count = result["count"].asInt();
        if ( result.isMember("showonline") )
            showonline = result["showonline"].asInt();
        if ( result.isMember("showoffline") )
            showoffline = result["showoffline"].asInt();
        if ( result.isMember("groupid") )
            groupid = convert_string_to_int(result["groupid"].asString());
        if ( result.isMember("maingroups") )
            maingroups = result["maingroups"].asString();
    }

    std::list<RFIDDeviceStruct*> * rfidDeciceList = mDeviceManager->get_RFIDDevice_list(projectid);

    //
    // filter by maingroups
    //
    std::list<RFIDDeviceStruct*> * groupNodeList = new std::list<RFIDDeviceStruct*>();
    std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');

    for (std::list<RFIDDeviceStruct*>::iterator existed = rfidDeciceList->begin(); existed != rfidDeciceList->end(); existed++)
    {
        for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
        {
            int maingroupid = convert_string_to_int(*maingroup);
            if ((*existed)->maingroupid == maingroupid || groupid == 1 || groupid == 2)
            {
                groupNodeList->push_back((*existed));
                break;
            }
        }
    }

    std::ostringstream sendstream;

    if (groupNodeList != NULL)
    {
        if (count == 0)
        {
            count = groupNodeList->size();
        }

        int index = 0;
        int sendcount = 0;
        sendstream<<"{";
        for (std::list<RFIDDeviceStruct *>::iterator rfidDecice = groupNodeList->begin(); rfidDecice != groupNodeList->end(); rfidDecice++)
        {
            if ( (*rfidDecice)->deviceid < 0 )
            {
                continue;
            }

            if ( showonline == 0 && (*rfidDecice)->alive != 0 )
            {
                continue;
            }
            if ( showoffline == 0 && (*rfidDecice)->alive == 0 )
            {
                continue;
            }

            if (index >= start_index && index < (start_index + count))
            {
                if (sendcount > 0)
                    sendstream<<",";

                sendstream<<"\""<<(*rfidDecice)->deviceid<<"\":";
                sendstream<<"{";
                sendstream<<"\"alive\":\""<<(*rfidDecice)->alive<<"\",";
                sendstream<<"\"deviceid\":\""<<(*rfidDecice)->deviceid<<"\",";
                sendstream<<"\"macaddress\":\""<<(*rfidDecice)->macaddress<<"\",";
                sendstream<<"\"name\":\""<<(*rfidDecice)->name<<"\",";
                sendstream<<"\"type\":\""<<(*rfidDecice)->type<<"\",";
                sendstream<<"\"ip\":\""<<(*rfidDecice)->ip<<"\",";
                sendstream<<"\"gwid\":\""<<(*rfidDecice)->gwid<<"\",";
                sendstream<<"\"LoRAMacaddress\":\""<<(*rfidDecice)->LoRAMacaddress<<"\",";
                sendstream<<"\"verifykey\":\""<<(*rfidDecice)->verifykey<<"\",";
                sendstream<<"\"datetime\":\""<<(*rfidDecice)->datetime<<"\"";
                sendstream<<"}"<<endl;

                sendcount++;
            }
            index++;
        }
        sendstream<<"}"<<endl;

        delete groupNodeList;
    }
    else
    {
        sendstream<<"{}"<<endl;
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();

    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);

}

void WebUDPParserService::sendRFIDCardStatusToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    int projectid = 1;
    int groupid = 3;
    int start_index = 0;
    int count = 0;
    int showonline = 0;
    int showoffline = 0;
    std::string maingroups = "1";

    if ( parsingSuccessful )
    {
        projectid = getProjectId(result);

        if ( result.isMember("start") )
            start_index = result["start"].asInt();
        if ( result.isMember("count") )
            count = result["count"].asInt();
        if ( result.isMember("showonline") )
            showonline = result["showonline"].asInt();
        if ( result.isMember("showoffline") )
            showoffline = result["showoffline"].asInt();
        if ( result.isMember("groupid") )
            groupid = convert_string_to_int(result["groupid"].asString());
        if ( result.isMember("maingroups") )
            maingroups = result["maingroups"].asString();
    }

    std::list<RFIDCardStruct*> * rfidcardList = mDeviceManager->get_RFIDCard_list(projectid);

    //
    // filter by maingroups
    //
    std::list<RFIDCardStruct*> * groupNodeList = new std::list<RFIDCardStruct*>();
    std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');
    for (std::list<RFIDCardStruct*>::iterator existed = rfidcardList->begin(); existed != rfidcardList->end(); existed++)
    {
        for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
        {
            int maingroupid = convert_string_to_int(*maingroup);
            if ((*existed)->maingroupid == maingroupid || groupid == 1 || groupid == 2)
            {
                groupNodeList->push_back((*existed));
                break;
            }
        }
    }

    std::ostringstream sendstream;

    if (groupNodeList != NULL)
    {
        if ( count == 0 )
        {
            count = groupNodeList->size();
        }

        // vilslog::printf("sendRFIDCardStatusToClient() projectid[%d] rfidcardList->size()[%d] start_index[%d] count[%d]\n",
        //     projectid, (int)rfidcardList->size(), start_index, count);

        int index = 0;
        int sendcount = 0;
        sendstream<<"[";
        for (std::list<RFIDCardStruct *>::iterator rfidcard = groupNodeList->begin(); rfidcard != groupNodeList->end(); rfidcard++)
        {
            // vilslog::printf("sendRFIDCardStatusToClient() index[%d] start_index[%d] (start_index + count)[%d] sendcount[%d]\n",
            //     index, start_index, (start_index + count), sendcount);

            if ((*rfidcard)->cardid.length() == 0)
            {
                continue;
            }

            if ( showonline == 0 && (*rfidcard)->alive != 0 )
            {
                continue;
            }
            if ( showoffline == 0 && (*rfidcard)->alive == 0 )
            {
                continue;
            }

            if (index >= start_index && index < (start_index + count))
            {
                // vilslog::printf("sendRFIDCardStatusToClient() index[%d] start_index[%d] (start_index + count)[%d] sendcount[%d] alive[%d]\n",
                //     index, start_index, (start_index + count), sendcount, (*rfidcard)->alive);

                if (sendcount > 0)
                    sendstream<<",";

                sendstream<<"{";
                sendstream<<"\"alive\":\""<<(*rfidcard)->alive<<"\",";
                sendstream<<"\"cardid\":\""<<(*rfidcard)->cardid<<"\",";
                sendstream<<"\"name\":\""<<(*rfidcard)->name<<"\",";
                sendstream<<"\"type\":\""<<(*rfidcard)->type<<"\",";
                sendstream<<"\"maplayer\":\""<<(*rfidcard)->maplayer<<"\",";
                sendstream<<"\"userid\":\""<<(*rfidcard)->userid<<"\",";
                sendstream<<"\"deviceid\":\""<<(*rfidcard)->deviceid<<"\",";
                sendstream<<"\"datetime\":\""<<(*rfidcard)->datetime<<"\",";
                sendstream<<"\"updatetime\":\""<<(*rfidcard)->updatetime<<"\"";
                sendstream<<"}";
                sendcount++;
            }
            index++;
        }
        sendstream<<"]"<<endl;

        delete groupNodeList;
    }
    else
    {
        sendstream<<"[]"<<endl;
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();

    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);

}

//
// send devices count to web api
//
void WebUDPParserService::sendDevicesCountToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    int projectid = 1;
    int groupid = 3;
    std::string maingroups = "";
    if ( parsingSuccessful )
    {
        projectid = getProjectId(result);

        if ( result.isMember("groupid") )
            groupid = convert_string_to_int(result["groupid"].asString());
        if ( result.isMember("maingroups") )
            maingroups = result["maingroups"].asString();
    }
    std::list<NodeStruct*> * anchorNodeList = mDeviceManager->get_Anchor_list(projectid);
    std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);
    std::list<NodeStruct*> * coordNodeList = mDeviceManager->get_Coordinator_list(projectid);
    std::list<NodeStruct*> * locatorNodeList = mDeviceManager->get_Locator_list(projectid);
    std::list<AoANodeStruct*> * aoaNodeList = mDeviceManager->get_AoA_list(projectid);
    std::list<SensorDeviceStruct*> * sensorNodeList = mDeviceManager->get_SensorDevice_list(projectid);

    std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');

    int anchor_num = 0;
    int tag_num = 0;
    int coord_num = 0;
    int locator_num = 0;
    int aoa_num = 0;
    int sensor_num = 0;

    if (groupid == 1 || groupid == 2)
    {
        anchor_num = (int)anchorNodeList->size();
        tag_num = (int)tagNodeList->size();
        coord_num = (int)coordNodeList->size();
        locator_num = (int)locatorNodeList->size();
        aoa_num = (int)aoaNodeList->size();
        sensor_num = (int)sensorNodeList->size();
    }
    else
    {
        for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
        {
            int maingroupid = convert_string_to_int(*maingroup);
            for (std::list<NodeStruct*>::iterator existed = anchorNodeList->begin(); existed != anchorNodeList->end(); existed++)
            {
                if ((*existed)->maingroupid == maingroupid)
                    anchor_num++;
            }
            for (std::list<NodeStruct*>::iterator existed = tagNodeList->begin(); existed != tagNodeList->end(); existed++)
            {
                if ((*existed)->maingroupid == maingroupid)
                    tag_num++;
            }
            for (std::list<NodeStruct*>::iterator existed = coordNodeList->begin(); existed != coordNodeList->end(); existed++)
            {
                if ((*existed)->maingroupid == maingroupid)
                    coord_num++;
            }
            for (std::list<NodeStruct*>::iterator existed = locatorNodeList->begin(); existed != locatorNodeList->end(); existed++)
            {
                if ((*existed)->maingroupid == maingroupid)
                    locator_num++;
            }
            for (std::list<AoANodeStruct*>::iterator existed = aoaNodeList->begin(); existed != aoaNodeList->end(); existed++)
            {
                if ((*existed)->maingroupid == maingroupid)
                    aoa_num++;
            }
            for (std::list<SensorDeviceStruct*>::iterator existed = sensorNodeList->begin(); existed != sensorNodeList->end(); existed++)
            {
                if ((*existed)->maingroupid == maingroupid)
                    sensor_num++;
            }
        }
    }

    std::ostringstream sendstream;
    sendstream<<"LOC_ANCHOR_NUM="<<anchor_num<<endl;
    sendstream<<"LOC_TAG_NUM="<<tag_num<<endl;
    sendstream<<"LOC_COORD_NUM="<<coord_num<<endl;
    sendstream<<"LOC_LOCATOR_NUM="<<locator_num<<endl;
    sendstream<<"LOC_AOA_NUM="<<aoa_num<<endl;
    sendstream<<"LOC_SENSOR_NUM="<<sensor_num<<endl;

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();

    maingroupList->clear();
    delete maingroupList;

    //printf("sendDevicesCountToClient() sendBuf[%s]\n", sendBuf);

    //mWebUDPServer->sendToClient(sendBuf, (int)sendString.length());
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendDeviceStatusToClient(UDPDataItem * item)
{
    //printf("sendDeviceStatusToClient()\n");
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    int projectid = 1;
    if ( parsingSuccessful )
    {
        projectid = getProjectId(result);
    }

    int static count = 0;
    std::ostringstream sendstream;

    if( count == 0 )
    {
        std::list<NodeStruct*> * anchorNodeList = mDeviceManager->get_Anchor_list(projectid);
        sendstream<<"LOC_ANCHOR_NUM="<<anchorNodeList->size()<<endl;
        dump_stream_nodelist_info(projectid, anchorNodeList, sendstream, std::string("LOC_ANCHOR"));
    }
    else
    if( count == 1 )
    {
        std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);
        sendstream<<"LOC_TAG_NUM="<<tagNodeList->size()<<endl;
        dump_stream_nodelist_info(projectid, tagNodeList, sendstream, std::string("LOC_TAG"));
    }
    else
    if( count == 2 )
    {
        std::list<NodeStruct*> * coordNodeList = mDeviceManager->get_Coordinator_list(projectid);
        sendstream<<"LOC_COORD_NUM="<<coordNodeList->size()<<endl;
        dump_stream_nodelist_info(projectid, coordNodeList, sendstream, std::string("LOC_COORD"));
    }
    else
    if( count == 3 )
    {
        std::list<NodeStruct*> * locatorNodeList = mDeviceManager->get_Locator_list(projectid);
        sendstream<<"LOC_LOCATOR_NUM="<<locatorNodeList->size()<<endl;
        dump_stream_nodelist_info(projectid, locatorNodeList, sendstream, std::string("LOC_LOCATOR"));
    }
    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    //mWebUDPServer->sendToClient(sendBuf, (int)sendString.length());
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);

    count++;
    if ( count >= 4 )
    {
        count = 0;
    }
}

void WebUDPParserService::startDumpRangingResult(char * buf, int length)
{
    vilslog::printf ("startDumpRangingResult() buf[%s]\n", buf);

    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
    // std::string bufString = buf;
    // std::string::size_type pos = bufString.find('=');
    // if (pos != std::string::npos)
    // {
    //     std::string num_string = bufString.substr(pos + 1);
    // int timeout = atoi(num_string.c_str());

        int projectid = getProjectId(result);
        int timeout = 10;
        if ( result.isMember("timeout") )
        {
            timeout = atoi(result["timeout"].asCString());
        }

        //
        // send to PosCalculate
        //
        DataItem * senditemmain = new DataItem("startdumprangingresult", projectid, timeout);
        mMainJobQueue.add(senditemmain);
    }
}

void WebUDPParserService::sendRangingToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        for (std::list<LastRangingStringStruct*>::iterator existed = mLastRangingStringList.begin(); existed != mLastRangingStringList.end(); existed++)
        {
            if( (*existed)->projectId == projectid)
            {
                const char * sendBuf = (*existed)->lastRangingString.c_str();
                item->mUDPServer->sendToClient(sendBuf, (int)(*existed)->lastRangingString.length(), (struct sockaddr *)&item->m_client_addr);
                break;
            }
        }
    }
}
void WebUDPParserService::sendCardScanResultToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    // vilslog::printf ("sendCardScanResultToClient() parsingSuccessful[%d]\n", parsingSuccessful);

    std::ostringstream sendstream;

    if ( parsingSuccessful )
    {
        //int projectid = getProjectId(result);

        // std::list<CARDScanRecordStruct*> * CARDScanList = mDeviceManager->get_CardScan_list(projectid);

        int count = 0;
        // for (std::list<CARDScanRecordStruct*>::iterator CARDScan = CARDScanList->begin(); CARDScan != CARDScanList->end(); CARDScan++)
        // {
        //     sendstream << (*CARDScan)->cardid << "=[{";
        //     int cnt = 0;
        //     for (std::list<CARDScanStruct*>::iterator record = (*CARDScan)->scanRecords.begin(); record != (*CARDScan)->scanRecords.end(); record++)
        //     {
        //         if (cnt > 0)
        //             sendstream<<"},{";
        //
        //         sendstream<<"\"loramac\":\""<<(*record)->LoRAMacaddress<<"\",";
        //         sendstream<<"\"time\":\""<<(*record)->datetime<<"\"";
        //
        //         cnt++;
        //     }
        //     sendstream<<"}]"<<endl;
        //     count++;
        // }

        if (count == 0)
        {
            sendstream << "=[]";
        }

        std::string sendString = sendstream.str();
        const char * sendBuf = sendString.c_str();

        // vilslog::printf ("sendCardScanResultToClient() 1 sendBuf[%s]\n", sendBuf);

        item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
    }
    else
    {
        std::string sendString = sendstream.str();
        const char * sendBuf = sendString.c_str();

        // vilslog::printf ("sendCardScanResultToClient() 2 sendBuf[%s]\n", sendBuf);

        item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
    }
}

void WebUDPParserService::sendAoARangingCloudPointToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    // vilslog::printf ("sendAoARangingCloudPointToClient() buf[%s]\n", buf);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        DataItem * senditem = new DataItem("DumpAoACirclePoint", projectid);
        mMainJobQueue.add(senditem);

        bool bFound = false;
        for (std::list<AoACloudPointStringStruct*>::iterator existed = mAoARangingCloudPointList.begin(); existed != mAoARangingCloudPointList.end(); existed++)
        {
            if( (*existed)->projectId == projectid)
            {
                if ((*existed)->CloudPointString.length() == 0)
                {
                    item->mUDPServer->sendToClient("[]", 2, (struct sockaddr *)&item->m_client_addr);
                }
                else
                {
                    const char * sendBuf = (*existed)->CloudPointString.c_str();
                    item->mUDPServer->sendToClient(sendBuf, (int)(*existed)->CloudPointString.length(), (struct sockaddr *)&item->m_client_addr);
                }
                bFound = true;
                break;
            }
        }
        if (!bFound)
        {
            item->mUDPServer->sendToClient("[]", 2, (struct sockaddr *)&item->m_client_addr);
        }
    }
    else
    {
        item->mUDPServer->sendToClient("[]", 2, (struct sockaddr *)&item->m_client_addr);
    }
}

void WebUDPParserService::sendAoAAngleVectorCloudPointToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    // vilslog::printf ("sendAoAAngleVectorCloudPointToClient() buf[%s]\n", buf);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        DataItem * senditem = new DataItem("DumpAoACirclePoint", projectid);
        mMainJobQueue.add(senditem);

        bool bFound = false;
        for (std::list<AoACloudPointStringStruct*>::iterator existed = mAoAAngleVectorCloudPointList.begin(); existed != mAoAAngleVectorCloudPointList.end(); existed++)
        {
            if( (*existed)->projectId == projectid)
            {
                // vilslog::printf ("sendAoAAngleVectorCloudPointToClient() (*existed)->projectId[%d] CloudPointString.length()[%d]\n", (*existed)->projectId, (*existed)->CloudPointString.length());

                if ((*existed)->CloudPointString.length() == 0)
                {
                    item->mUDPServer->sendToClient("[]", 2, (struct sockaddr *)&item->m_client_addr);
                }
                else
                {
                    // vilslog::printf ("sendAoAAngleVectorCloudPointToClient() 1\n");
                    const char * sendBuf = (*existed)->CloudPointString.c_str();
                    item->mUDPServer->sendToClient(sendBuf, (int)(*existed)->CloudPointString.length(), (struct sockaddr *)&item->m_client_addr);
                }
                bFound = true;
                break;
            }
        }
        if (!bFound)
        {
            // vilslog::printf ("sendAoAAngleVectorCloudPointToClient() 2\n");
            item->mUDPServer->sendToClient("[]", 2, (struct sockaddr *)&item->m_client_addr);
        }
    }
    else
    {
        // vilslog::printf ("sendAoAAngleVectorCloudPointToClient() 3\n");

        item->mUDPServer->sendToClient("[]", 2, (struct sockaddr *)&item->m_client_addr);
    }
}

void WebUDPParserService::sendAoAAngleCloudPointToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    //vilslog::printf ("sendAoAAngleCloudPointToClient() buf[%s]\n", buf);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        DataItem * senditem = new DataItem("DumpAoACirclePoint", projectid);
        mMainJobQueue.add(senditem);

        bool bFound = false;
        for (std::list<AoACloudPointStringStruct*>::iterator existed = mAoAAngleCloudPointList.begin(); existed != mAoAAngleCloudPointList.end(); existed++)
        {
            if( (*existed)->projectId == projectid)
            {
                //vilslog::printf ("sendAoAAngleCloudPointToClient() (*existed)->projectId[%d] CloudPointString.length()[%d]\n", (*existed)->projectId, (*existed)->CloudPointString.length());

                if ((*existed)->CloudPointString.length() == 0)
                {
                    item->mUDPServer->sendToClient("[]", 2, (struct sockaddr *)&item->m_client_addr);
                }
                else
                {
                    //vilslog::printf ("sendAoAAngleCloudPointToClient() 1\n");
                    const char * sendBuf = (*existed)->CloudPointString.c_str();
                    item->mUDPServer->sendToClient(sendBuf, (int)(*existed)->CloudPointString.length(), (struct sockaddr *)&item->m_client_addr);
                }
                bFound = true;
                break;
            }
        }
        if (!bFound)
        {
            //vilslog::printf ("sendAoAAngleCloudPointToClient() 2\n");
            item->mUDPServer->sendToClient("[]", 2, (struct sockaddr *)&item->m_client_addr);
        }
    }
    else
    {
        //vilslog::printf ("sendAoAAngleCloudPointToClient() 3\n");

        item->mUDPServer->sendToClient("[]", 2, (struct sockaddr *)&item->m_client_addr);
    }
}

void WebUDPParserService::setPowerTagCalculateInfo(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    //vilslog::printf ("sendAoAAngleCloudPointToClient() buf[%s]\n", buf);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        DataItem * senditem = new DataItem("PowerTagCalculateInfo", projectid);
        senditem->setMessage(buf);

        mMainJobQueue.add(senditem);
    }
}

void WebUDPParserService::updateNodeInfo(const char * node_type, char * buf, int length)
{
    //
    // anchor={"macaddress":"55BBCC4455667788","posX":"30","posY":"40","posZ":"20"}
    //
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        bool updatePos = false;
        double posX = 0;
        double posY = 0;
        double posZ = 0;
        int tafid = -1;
        int buildingid = -100;
        int buildingfloor = -100;
        int maplayerid = -1;
        char nodeid[64];
        char nodename[64];
        char loramacaddress[64];
        memset(nodeid, 0, 64);
        memset(nodename, 0, 64);
        memset(loramacaddress, 0, 64);

        int projectid = getProjectId(result);

        if ( result.isMember("posX") )
        {
            updatePos = true;
            posX = atof(result["posX"].asCString());
        }
        if ( result.isMember("posY") )
        {
            updatePos = true;
            posY = atof(result["posY"].asCString());
        }
        if ( result.isMember("posZ") )
        {
            updatePos = true;
            posZ = atof(result["posZ"].asCString());
        }
        if ( result.isMember("tafid") )
        {
            tafid = atoi(result["tafid"].asCString());
        }
        if ( result.isMember("buildingid") )
        {
            buildingid = atoi(result["buildingid"].asCString());
        }
        if ( result.isMember("buildingfloor") )
        {
            buildingfloor = atoi(result["buildingfloor"].asCString());
        }
        if ( result.isMember("nodeid") )
            snprintf(nodeid, 64, "%s", result["nodeid"].asCString());
        if ( result.isMember("nodename") )
            snprintf(nodename, 64, "%s", result["nodename"].asCString());
        if ( result.isMember("LoRAMacaddress") )
            snprintf(loramacaddress, 64, "%s", result["LoRAMacaddress"].asCString());
        if ( result.isMember("maplayerid") )
            maplayerid = atoi(result["maplayerid"].asCString());

        vilslog::printf ("WebUDPParserService::updateNodeInfo() nodeid[%s] maplayerid[%d]\n", nodeid, maplayerid);

        if ( result.isMember("macaddress") )
        {
            vilslog::printf ("WebUDPParserService::updateNodeInfo() node_type[%s] macaddress[%s]\n", node_type, result["macaddress"].asCString());

            if ( strcmp(node_type, "anchor") == 0 )
            {
                mDeviceManager->setAnchorInfo(projectid, result["macaddress"].asCString(), nodeid, tafid, buildingid, buildingfloor, nodename, -1, updatePos, posX, posY, posZ, "");
                mDeviceManager->setAnchorInfoMapLayer(projectid, result["macaddress"].asCString(), maplayerid);

                DataItem * senditem1 = new DataItem("dumpanchor", projectid);
                mMainJobQueue.add(senditem1);
            }
            else
            if ( strcmp(node_type, "tag") == 0 )
            {
                mDeviceManager->setTagInfo(projectid, result["macaddress"].asCString(), nodeid, tafid, buildingid, buildingfloor, nodename, -1, updatePos, posX, posY, posZ, "");
                mDeviceManager->setTagInfoMapLayer(projectid, result["macaddress"].asCString(), maplayerid);

                //
                // update to database
                //
                if ( tafid >= 0 )
                {
                    NodeStruct * tagnode = mDeviceManager->get_Tag_node_ByMacaddress(projectid, result["macaddress"].asCString());
                    if (tagnode != NULL)
                    {
                        WorkItem * item = new WorkItem("updatePushNotifySetting", projectid, NODE_TYPE_TAG, result["macaddress"].asCString());
                        item->setNodeId(tagnode->nodeid);
                        item->setRole(tafid);
                        mMySQLJobQueue.add(item);
                    }
                }

                DataItem * senditem1 = new DataItem("dumptag", projectid);
                mMainJobQueue.add(senditem1);

            }
            else
            if ( strcmp(node_type, "coord") == 0 )
            {
                mDeviceManager->setCoordInfo(projectid, result["macaddress"].asCString(), nodeid, tafid, buildingid, buildingfloor, nodename, -1, updatePos, posX, posY, posZ, "");
                mDeviceManager->setCoordInfoMapLayer(projectid, result["macaddress"].asCString(), maplayerid);

                DataItem * senditem1 = new DataItem("dumpcoord", projectid);
                mMainJobQueue.add(senditem1);
            }
            else
            if ( strcmp(node_type, "locator") == 0 )
            {
                mDeviceManager->setLocatorInfo(projectid, result["macaddress"].asCString(), nodeid, tafid, buildingid, buildingfloor, nodename, -1, updatePos, posX, posY, posZ, "");
                mDeviceManager->setLocatorInfoMapLayer(projectid, result["macaddress"].asCString(), maplayerid);
                if (strlen(loramacaddress) > 0)
                    mDeviceManager->setLocatorInfoLoRA(projectid, result["macaddress"].asCString(), loramacaddress);
                //
                vilslog::printf("WebUDPParserService::updateNodeInfo() locator macaddress[%s] updatePos[%d] posX[%f] posY[%f] posZ[%f] maplayerid[%d]\n",
                    result["macaddress"].asCString(), updatePos, posX, posY, posZ, maplayerid);

                // update AoA device only
                if ( updatePos && mDeviceManager->setAoAInfo(projectid, result["macaddress"].asCString(), posX, posY, posZ))
                {
                    vilslog::printf("WebUDPParserService::updateNodeInfo() mDeviceManager->setAoAInfo() ok\n");

                    AoANodeStruct * aoaNode = mDeviceManager->get_AoA_node(projectid, result["macaddress"].asCString());
                    if (aoaNode != NULL)
                    {
                        WorkItem * item = new WorkItem("updateAoANodeInfo", projectid, aoaNode);
                        mMySQLJobQueue.add(item);
                    }
                }

                //
                // update to database
                //
                if ( tafid >= 0 )
                {
                    NodeStruct * locatornode = mDeviceManager->get_Locator_node_ByMacaddress(projectid, result["macaddress"].asCString());
                    if (locatornode != NULL)
                    {
                        WorkItem * item = new WorkItem("updatePushNotifySetting", projectid, NODE_TYPE_LOCATOR, result["macaddress"].asCString());
                        item->setNodeId(locatornode->nodeid);
                        item->setRole(tafid);
                        mMySQLJobQueue.add(item);
                    }
                }

                DataItem * senditem1 = new DataItem("dumplocator", projectid);
                mMainJobQueue.add(senditem1);
            }
            else
            if ( strcmp(node_type, "gateway") == 0 )
            {
                // macaddress --> gateway id
                mDeviceManager->setLoRaGatewayMapLayer(projectid, result["macaddress"].asCString(), maplayerid);

                DataItem * senditem1 = new DataItem("dumpgateway", projectid);
                mMainJobQueue.add(senditem1);
            }
            //
            // updateto database
            //
            WorkItem * item = new WorkItem("updateInfo", projectid, result["macaddress"].asCString(), nodeid, nodename, buildingid, buildingfloor, posX, posY, posZ);
            mMySQLJobQueue.add(item);
        }

    }
}

//
// 2d map editor
//
void WebUDPParserService::updateMapNodeInfo(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        double posX = 0;
        double posY = 0;
        double posZ = 0;
        std::string Latitude;
        std::string Longitude;
        std::string type;

        int projectid = getProjectId(result);

        if ( result.isMember("posX") )
            posX = atof(result["posX"].asCString());
        if ( result.isMember("posY") )
            posY = atof(result["posY"].asCString());
        if ( result.isMember("posZ") )
            posZ = atof(result["posZ"].asCString());
        if ( result.isMember("Latitude") )
            Latitude = result["Latitude"].asString();
        if ( result.isMember("Longitude") )
            Longitude = result["Longitude"].asString();

        // vilslog::printf ("WebUDPParserService::updateMapNodeInfo() type[%s] nodeid[%s] Latitude[%s] Longitude[%s]\n",
        //     result["type"].asCString(), result["nodeid"].asCString(), Latitude.c_str(), Longitude.c_str());

        if ( result.isMember("nodeid") && result.isMember("type") )
        {
            type = result["type"].asString();
            if ( type.compare("user") == 0 )
            {
                int userid = convert_string_to_int(result["nodeid"].asString());

                update_user_location(projectid, userid, Longitude, Latitude);
            }
            else
            if ( type.compare("sensor") == 0 )
            {
                SensorDeviceStruct * sensorDevice = mDeviceManager->setSensorDeviceLocation(projectid, result["nodeid"].asString(), Latitude, Longitude);
                if ( sensorDevice != NULL )
                {
                    WorkItem * witem = new WorkItem("updatesensordevice", projectid, sensorDevice);
                    mMySQLJobQueue.add(witem);
                }
            }
            else
            {
                // vilslog::printf ("WebUDPParserService::updateMapNodeInfo() type[%s] nodeid[%s] pos(%d, %d, %d)\n",
                //     result["type"].asCString(), result["nodeid"].asCString(), (int)posX, (int)posY, (int)posZ);

                mDeviceManager->setNodeInfoByID(projectid, result["type"].asCString(), result["nodeid"].asCString(), posX, posY, posZ);
            }
        }
    }
}

//
// delete a node
//
void WebUDPParserService::deleteNodeInfo(char * buf, int length)
{
    //printf ("deleteNodeInfo() length[%d] buf[%s]\n", length, buf);

    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        std::string maingroups = "";
        int groupid = 0;

        if ( result.isMember("maingroups") )
            maingroups = result["maingroups"].asString();
        if ( result.isMember("groupid") )
            groupid = convert_string_to_int(result["groupid"].asString());

        if(groupid == 5 || groupid == 6)
        {
            if ( result.isMember("macaddress") )
            {
                std::string macaddress = result["macaddress"].asCString();
                vilslog::printf("WebUDPParserService::deleteNodeInfo() macaddress[%s] maingroupid_i[1]\n", macaddress.c_str());

                mDeviceManager->update_Node_maingroup(macaddress.c_str(), 1);

                int projectid = 1;
                DataItem * senditem1 = new DataItem("dumpanchor", projectid);
                mMainJobQueue.add(senditem1);
                DataItem * senditem2 = new DataItem("dumpcoord", projectid);
                mMainJobQueue.add(senditem2);
                DataItem * senditem3 = new DataItem("dumplocator", projectid);
                mMainJobQueue.add(senditem3);
            }
        }
        else
        if ( result.isMember("type") && result.isMember("macaddress") )
        {
            //printf ("deleteNodeInfo() type[%s] macaddress[%s]\n", result["type"].asCString(), result["macaddress"].asCString());

            //
            // remove from mysql
            //
            int nodetype = NODE_TYPE_ANCHOR;
            if ( strcmp(result["type"].asCString(), "anchor") == 0 )
            {
                nodetype = NODE_TYPE_ANCHOR;
                //projectid = mDeviceManager->getAnchorProjectId(result["macaddress"].asCString());
            }
            else
            if ( strcmp(result["type"].asCString(), "tag") == 0 )
            {
                nodetype = NODE_TYPE_TAG;
                //projectid = mDeviceManager->getTagProjectId(result["macaddress"].asCString());
            }
            else
            if ( strcmp(result["type"].asCString(), "coord") == 0 )
            {
                nodetype = NODE_TYPE_COORD;
                //projectid = mDeviceManager->getCoordinatorProjectId(result["macaddress"].asCString());
            }
            else
            if ( strcmp(result["type"].asCString(), "locator") == 0 )
            {

                nodetype = NODE_TYPE_LOCATOR;
                //projectid = mDeviceManager->getLocatorProjectId(result["macaddress"].asCString());
                bool bFound = mDeviceManager->deleteNodeByMAC(projectid, result["macaddress"].asCString(), "aoa");
                if (bFound)
                {
                    WorkItem * item = new WorkItem("deleteaoanode", projectid, NODE_TYPE_AOA, result["macaddress"].asCString());
                    mMySQLJobQueue.add(item);
                }
            }

            mDeviceManager->deleteNodeByMAC(projectid, result["macaddress"].asCString(), result["type"].asCString());

            WorkItem * item = new WorkItem("deletenode", projectid, nodetype, result["macaddress"].asCString());
            mMySQLJobQueue.add(item);
        }
    }
}

//
// delete a gateway
//
void WebUDPParserService::deleteGatewayInfo(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        if ( result.isMember("gatewayid"))
        {
            std::string maingroups = "";
            int groupid = 0;

            if ( result.isMember("maingroups") )
                maingroups = result["maingroups"].asString();
            if ( result.isMember("groupid") )
                groupid = convert_string_to_int(result["groupid"].asString());

            if(groupid == 5 || groupid == 6)
            {
                if ( result.isMember("gatewayid") )
                {
                    std::string gatewayid = result["gatewayid"].asCString();
                    vilslog::printf("WebUDPParserService::deleteGatewayInfo() gatewayid[%s] maingroupid_i[1]\n", gatewayid.c_str());

                    mDeviceManager->setLoRaGatewayMaingroup(projectid, gatewayid.c_str(), 1);

                    DataItem * senditem3 = new DataItem("dumpgateway", projectid);
                    mMainJobQueue.add(senditem3);
                }
            }
            else
            {
                mDeviceManager->deleteGatewayByID(projectid, result["gatewayid"].asString());

                WorkItem * item = new WorkItem("deletegateway", projectid, NODE_TYPE_GATEWAY, result["gatewayid"].asCString());
                mMySQLJobQueue.add(item);
            }
        }
    }
}

void WebUDPParserService::deleteAllOfflineDevice(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        if ( result.isMember("TYPE"))
        {
            std::list<NodeStruct*> * nodeList = NULL;

            int nodetype = NODE_TYPE_ANCHOR;
            if ( strcmp(result["TYPE"].asCString(), "anchor") == 0 )
            {
                nodetype = NODE_TYPE_ANCHOR;
                nodeList = mDeviceManager->get_Anchor_list(projectid);
            }
            else
            if ( strcmp(result["TYPE"].asCString(), "tag") == 0 )
            {
                nodetype = NODE_TYPE_TAG;
                nodeList = mDeviceManager->get_Tag_list(projectid);
            }
            else
            if ( strcmp(result["TYPE"].asCString(), "coord") == 0 )
            {
                nodetype = NODE_TYPE_COORD;
                nodeList = mDeviceManager->get_Coordinator_list(projectid);
            }
            else
            if ( strcmp(result["TYPE"].asCString(), "locator") == 0 )
            {
                nodetype = NODE_TYPE_LOCATOR;
                nodeList = mDeviceManager->get_Locator_list(projectid);
            }

            if ( nodeList == NULL )
                return;

            vilslog::printf("WebUDPParserService::deleteAllOfflineDevice() nodetype[%d] nodeList->size()[%d]\n", nodetype, nodeList->size());

            for (std::list<NodeStruct*>::iterator node = nodeList->begin(); node != nodeList->end();)
            {
                //mDeviceManager->deleteNodeByMAC((*node)->macaddress, result["type"].asCString());

                if ( (*node)->index == -1 || (*node)->alive == 0 )
                {
                    WorkItem * item = new WorkItem("deletenode", projectid, nodetype, (*node)->macaddress);
                    mMySQLJobQueue.add(item);

            		node = nodeList->erase(node);
                }
                else
                {
                    node++;
                }
            }
            vilslog::printf("WebUDPParserService::deleteAllOfflineDevice() Finished! nodetype[%d] nodeList->size()[%d]\n", nodetype, nodeList->size());
        }
    }
}

void WebUDPParserService::updateConfig(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        VILSConfig config;
        config.projectid = 1;
        config.projectname = "NEW_PROJECT";
        config.projectdesc = "PROJECT_DESCRIPTION";
        config.WGS48_Origin_X = "121.5379560";
        config.WGS48_Origin_Y = "25.0401453";
        config.MAP_North = "0.0";
        // config.Building_Origin_X = "121.5379560";
        // config.Building_Origin_Y = "25.0401453";
        // config.Building_X_Length = "0.0001";
        // config.Building_Y_Length = "0.0001";
        config.BuildingID = 1;
        config.BuildingFloor = 1;

        if ( result.isMember("project_id") )
            config.projectid = getProjectId(result);
        if ( result.isMember("PROJECT_NAME") )
            config.projectname = result["PROJECT_NAME"].asString();
        if ( result.isMember("PROJECT_DESCRIPTION") )
            config.projectdesc = result["PROJECT_DESCRIPTION"].asString();
        // if ( result.isMember("WGS48OriginX") )
        //     config.WGS48_Origin_X = result["WGS48OriginX"].asCString();
        // if ( result.isMember("WGS48OriginY") )
        //     config.WGS48_Origin_Y = result["WGS48OriginY"].asCString();
        // if ( result.isMember("MAP_North") )
        //     config.MAP_North = result["MAP_North"].asCString();
        // if ( result.isMember("BuildingOriginX") )
        //     config.Building_Origin_X = result["BuildingOriginX"].asCString();
        // if ( result.isMember("BuildingOriginY") )
        //     config.Building_Origin_Y = result["BuildingOriginY"].asCString();
        // if ( result.isMember("BuildingXLength") )
        //     config.Building_X_Length = result["BuildingXLength"].asCString();
        // if ( result.isMember("BuildingYLength") )
        //     config.Building_Y_Length = result["BuildingYLength"].asCString();
        // if ( result.isMember("BuildingID") )
        //     config.BuildingID = atoi(result["BuildingID"].asCString());
        // if ( result.isMember("BuildingFloor") )
        //     config.BuildingFloor = atoi(result["BuildingFloor"].asCString());

        vilslog::printf ("WebUDPParserService::updateConfig() projectid[%d] projectname[%s] projectdesc[%s]\n",
            config.projectid, config.projectname.c_str(), config.projectdesc.c_str());

        WorkItem * item = new WorkItem("updateconfig", config.projectid, config);
        mMySQLJobQueue.add(item);

        DataItem * senditem = new DataItem("updateconfig", config.projectid);
        mMainJobQueue.add(senditem);
    }
}

void WebUDPParserService::createConfig(UDPDataItem * item)
{
    char * buf = item->getMsg();
    vilslog::printf ("WebUDPParserService::createConfig() buf[%s]\n", buf);

    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    vilslog::printf ("WebUDPParserService::createConfig() parsingSuccessful[%d]\n", parsingSuccessful);

    if ( parsingSuccessful )
    {
        VILSConfig config;
        config.projectid = getProjectId(result);

        vilslog::printf ("WebUDPParserService::createConfig() config.projectid[%d]\n", config.projectid);

        //mDbManager->reload_prj_config();
        //vilslog::printf ("WebUDPParserService::createConfig() reload_prj_config() finished!\n");

        VILSSystemConfig * systemConfig = mDbManager->get_sys_config(config.projectid);

        std::ostringstream sendstream;
        if (systemConfig != NULL)
            sendstream<<"{\"create\":\"success\", \"PROJECT_ID\":\""<<config.projectid<<"\"}"<<endl;
        else
            sendstream<<"{\"create\":\"error\", \"PROJECT_ID\":\""<<config.projectid<<"\""<<endl;

        std::string sendString = sendstream.str();

        const char * sendBuf = sendString.c_str();

        vilslog::printf ("WebUDPParserService::createConfig() sendBuf[%s]\n", sendBuf);

        //item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);

        DataItem * senditem = new DataItem("updateAllconfig", config.projectid);
        mMainJobQueue.add(senditem);
    }
    else
    {
        std::ostringstream sendstream;

        sendstream<<"{\"create\":\"failed\", \"PROJECT_ID\":\""<<0<<"\""<<endl;

        std::string sendString = sendstream.str();

        const char * sendBuf = sendString.c_str();
        item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
    }
}

void WebUDPParserService::deleteConfig(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        VILSConfig config;
        config.projectid = getProjectId(result);
        WorkItem * item = new WorkItem("deleteconfig", config.projectid, config);
        mMySQLJobQueue.add(item);

        DataItem * senditem = new DataItem("updateAllconfig", config.projectid);
        mMainJobQueue.add(senditem);
    }
}

void WebUDPParserService::sendSystemConfigToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);
    int projectid = 1;
    if ( parsingSuccessful )
    {
        projectid = getProjectId(result);
    }

    VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);

    std::ostringstream sendstream;

    sendstream<<"{";
    sendstream<<"\"SolveTagPosition_Time\":\""<<vilssysConfig->SolveTagPosition_Time<<"\",";
    sendstream<<"\"SendTAFDB_Time\":\""<<vilssysConfig->SendTAFDB_Time<<"\",";
    sendstream<<"\"SendTagAnchorList_Time\":\""<<vilssysConfig->SendTagAnchorList_Time<<"\",";
    sendstream<<"\"SendDeviceStatus_Time\":\""<<vilssysConfig->SendDeviceStatus_Time<<"\",";
    sendstream<<"\"UpdateNodeFromSMIMSIOTSERVER_Time\":\""<<vilssysConfig->UpdateNodeFromSMIMSIOTSERVER_Time<<"\",";

    sendstream<<"\"web_udp_port\":\""<<vilssysConfig->web_udp_port<<"\",";
    sendstream<<"\"center_udp_port\":\""<<vilssysConfig->center_udp_port<<"\",";
    sendstream<<"\"coord_udp_port\":\""<<vilssysConfig->coord_udp_port<<"\",";
    sendstream<<"\"center_broadcast_time\":\""<<vilssysConfig->center_broadcast_time<<"\",";
    sendstream<<"\"center_broadcast_port\":\""<<vilssysConfig->center_broadcast_port<<"\",";
    sendstream<<"\"web_socket_port\":\""<<vilssysConfig->web_socket_port<<"\",";
    sendstream<<"\"UsingProtocol\":\""<<vilssysConfig->UsingProtocol<<"\",";
    sendstream<<"\"VILSServer\":\""<<vilssysConfig->VILSServer<<"\",";
    sendstream<<"\"ERPServer\":\""<<vilssysConfig->ERPServer<<"\",";
    sendstream<<"\"SSOIP\":\""<<vilssysConfig->SSOIP<<"\",";

    sendstream<<"\"PushNotifyPrefix\":\""<<vilssysConfig->PushNotifyPrefix<<"\",";
    sendstream<<"\"PushNotifyWelcomeTitle\":\""<<vilssysConfig->PushNotifyWelcomeTitle<<"\",";
    sendstream<<"\"PushNotifyWelcomeBody\":\""<<vilssysConfig->PushNotifyWelcomeBody<<"\",";
    sendstream<<"\"EnablePushNotify\":\""<<vilssysConfig->EnablePushNotify<<"\",";

    sendstream<<"\"TagAlertTimeout\":\""<<vilssysConfig->TagAlertTimeout<<"\",";
    sendstream<<"\"TagAlertVoltageLow\":\""<<vilssysConfig->TagAlertVoltageLow<<"\",";

    //1.Ranging Data Preprocessing
    sendstream<<"\"ValidDataTime\":\""<<vilssysConfig->ValidDataTime<<"\",";
    sendstream<<"\"MaxRangindDistance\":\""<<vilssysConfig->MaxRangindDistance<<"\",";
    sendstream<<"\"EnableNoiseMakup\":\""<<vilssysConfig->EnableNoiseMakup<<"\",";
    sendstream<<"\"NoiseMakupRatio\":\""<<vilssysConfig->NoiseMakupRatio<<"\",";
    sendstream<<"\"NoiseMakupWeight\":\""<<vilssysConfig->NoiseMakupWeight<<"\",";
    sendstream<<"\"EnableKalman1D\":\""<<vilssysConfig->EnableKalman1D<<"\",";
    sendstream<<"\"EnableRangingReliability\":\""<<vilssysConfig->EnableRangingReliability<<"\",";
    sendstream<<"\"RangingReliabilityMaxDistance\":\""<<vilssysConfig->RangingReliabilityMaxDistance<<"\",";
    sendstream<<"\"RangingReliabilityMinCount\":\""<<vilssysConfig->RangingReliabilityMinCount<<"\",";
    sendstream<<"\"RangingReliabilityAverageDiff\":\""<<vilssysConfig->RangingReliabilityAverageDiff<<"\",";

    //2.Position Calculation
    sendstream<<"\"EnableAnchor1DMapping\":\""<<vilssysConfig->EnableAnchor1DMapping<<"\",";
    sendstream<<"\"EnableOneAnchorCalculation\":\""<<vilssysConfig->EnableOneAnchorCalculation<<"\",";
    sendstream<<"\"EnableGDOP\":\""<<vilssysConfig->EnableGDOP<<"\",";
    sendstream<<"\"GDOPThreshold\":\""<<vilssysConfig->GDOPThreshold<<"\",";
    sendstream<<"\"CalculationMethod\":\""<<vilssysConfig->CalculationMethod<<"\",";
    sendstream<<"\"EnableIMUVelocity\":\""<<vilssysConfig->EnableIMUVelocity<<"\",";
    sendstream<<"\"IMUVelocityOffset\":\""<<vilssysConfig->IMUVelocityOffset<<"\",";
    sendstream<<"\"SteadyStateOffset\":\""<<vilssysConfig->SteadyStateOffset<<"\",";
    sendstream<<"\"WeightMovingAverageValue1\":\""<<vilssysConfig->WeightMovingAverageValue1<<"\",";
    sendstream<<"\"WeightMovingAverageValue2\":\""<<vilssysConfig->WeightMovingAverageValue2<<"\",";
    sendstream<<"\"WeightMovingAverageValue3\":\""<<vilssysConfig->WeightMovingAverageValue3<<"\",";
    sendstream<<"\"StayInWallTimeout\":\""<<vilssysConfig->StayInWallTimeout<<"\",";
    sendstream<<"\"EnableFixHeight\":\""<<vilssysConfig->EnableFixHeight<<"\",";

    //3.Position Result
    sendstream<<"\"SmoothingResult\":\""<<vilssysConfig->SmoothingResult<<"\",";
    sendstream<<"\"SmoothingResultOffset\":\""<<vilssysConfig->SmoothingResultOffset<<"\",";
    sendstream<<"\"SmoothingResultStep\":\""<<vilssysConfig->SmoothingResultStep<<"\",";
    sendstream<<"\"SmoothingResultFirstStateOffsetRatio\":\""<<vilssysConfig->SmoothingResultFirstStateOffsetRatio<<"\",";
    sendstream<<"\"SmoothingResultFirstStateRunTime\":\""<<vilssysConfig->SmoothingResultFirstStateRunTime<<"\",";
    sendstream<<"\"SmoothingResultSecondStateOffsetRatio\":\""<<vilssysConfig->SmoothingResultSecondStateOffsetRatio<<"\",";
    sendstream<<"\"SmoothingResultSecondStateRunTime\":\""<<vilssysConfig->SmoothingResultSecondStateRunTime<<"\",";
    //
    // LoRAWAN
    //
    sendstream<<"\"LoRAWANTPOIC\":\""<<vilssysConfig->LoRAWANTPOIC<<"\",";
    sendstream<<"\"MQTTHostIp\":\""<<vilssysConfig->MQTTHostIp<<"\",";
    sendstream<<"\"MQTTHostPort\":\""<<vilssysConfig->MQTTHostPort<<"\",";
    sendstream<<"\"MQTTHostUsername\":\""<<vilssysConfig->MQTTHostUsername<<"\",";
    sendstream<<"\"MQTTHostPassword\":\""<<vilssysConfig->MQTTHostPassword<<"\",";
    sendstream<<"\"MQTTClientId\":\""<<vilssysConfig->MQTTClientId<<"\"";
    sendstream<<"}"<<endl;

    std::string sendString = sendstream.str();
    const char * sendBuf = sendString.c_str();

    //printf("WebUDPParserService::sendSystemConfigToClient() sendBuf[%s]\n", sendBuf);

    //mWebUDPServer->sendToClient(sendBuf, (int)sendString.length());
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
    //delete config;
}

void WebUDPParserService::updateSystemConfigUDPPort(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);

        if ( result.isMember("web_udp_port") )
            vilssysConfig->web_udp_port = atoi(result["web_udp_port"].asCString());
        if ( result.isMember("center_udp_port") )
            vilssysConfig->center_udp_port = atoi(result["center_udp_port"].asCString());
        if ( result.isMember("coord_udp_port") )
            vilssysConfig->coord_udp_port = atoi(result["coord_udp_port"].asCString());
        if ( result.isMember("center_broadcast_port") )
            vilssysConfig->center_broadcast_port = atoi(result["center_broadcast_port"].asCString());
        if ( result.isMember("web_socket_port") )
            vilssysConfig->web_socket_port = atoi(result["web_socket_port"].asCString());
        if ( result.isMember("TagAlertTimeout") )
            vilssysConfig->TagAlertTimeout = atoi(result["TagAlertTimeout"].asCString());
        if ( result.isMember("TagAlertVoltageLow") )
            vilssysConfig->TagAlertVoltageLow = result["TagAlertVoltageLow"].asString();
        if ( result.isMember("VILSServer") )
            vilssysConfig->VILSServer = result["VILSServer"].asString();
        if ( result.isMember("ERPServer") )
            vilssysConfig->ERPServer = result["ERPServer"].asString();
        if ( result.isMember("SSOIP") )
            vilssysConfig->SSOIP = result["SSOIP"].asString();
        if ( result.isMember("PushNotifyPrefix") )
            vilssysConfig->PushNotifyPrefix = result["PushNotifyPrefix"].asString();
        if ( result.isMember("PushNotifyWelcomeTitle") )
            vilssysConfig->PushNotifyWelcomeTitle = result["PushNotifyWelcomeTitle"].asString();
        if ( result.isMember("PushNotifyWelcomeBody") )
            vilssysConfig->PushNotifyWelcomeBody = result["PushNotifyWelcomeBody"].asString();
        if ( result.isMember("EnablePushNotify") )
            vilssysConfig->EnablePushNotify = atoi(result["EnablePushNotify"].asCString());

        // vilslog::printf("WebUDPParserService::updateSystemConfigUDPPort() VILSServer[%s]\n", vilssysConfig->VILSServer.c_str());
        // vilslog::printf("WebUDPParserService::updateSystemConfigUDPPort() VILSUID[%s]\n", vilssysConfig->VILSUID.c_str());

        WorkItem * item = new WorkItem("updatesystemconfigudpport", projectid, vilssysConfig);
        mMySQLJobQueue.add(item);

        DataItem * senditem = new DataItem("updatesystemconfig", projectid);
        mMainJobQueue.add(senditem);
    }
}

void WebUDPParserService::updateAlgorithmConfig(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);

        if ( result.isMember("ValidDataTime") )
             vilssysConfig->ValidDataTime = atoi(result["ValidDataTime"].asCString());
        if ( result.isMember("MaxRangindDistance") )
            vilssysConfig->MaxRangindDistance = atoi(result["MaxRangindDistance"].asCString());
        if ( result.isMember("EnableNoiseMakup") )
            vilssysConfig->EnableNoiseMakup = atoi(result["EnableNoiseMakup"].asCString());
        if ( result.isMember("NoiseMakupRatio") )
            vilssysConfig->NoiseMakupRatio = atof(result["NoiseMakupRatio"].asCString());
        if ( result.isMember("NoiseMakupWeight") )
            vilssysConfig->NoiseMakupWeight = atof(result["NoiseMakupWeight"].asCString());
        if ( result.isMember("EnableKalman1D") )
            vilssysConfig->EnableKalman1D = atoi(result["EnableKalman1D"].asCString());
        if ( result.isMember("EnableRangingReliability") )
            vilssysConfig->EnableRangingReliability = atoi(result["EnableRangingReliability"].asCString());
        if ( result.isMember("RangingReliabilityMaxDistance") )
            vilssysConfig->RangingReliabilityMaxDistance = atoi(result["RangingReliabilityMaxDistance"].asCString());
        if ( result.isMember("RangingReliabilityMinCount") )
            vilssysConfig->RangingReliabilityMinCount = atoi(result["RangingReliabilityMinCount"].asCString());
        if ( result.isMember("RangingReliabilityAverageDiff") )
            vilssysConfig->RangingReliabilityAverageDiff = atoi(result["RangingReliabilityAverageDiff"].asCString());

        if ( result.isMember("EnableAnchor1DMapping") )
            vilssysConfig->EnableAnchor1DMapping = atof(result["EnableAnchor1DMapping"].asCString());
        if ( result.isMember("EnableOneAnchorCalculation") )
            vilssysConfig->EnableOneAnchorCalculation = atof(result["EnableOneAnchorCalculation"].asCString());
        if ( result.isMember("EnableGDOP") )
            vilssysConfig->EnableGDOP = atoi(result["EnableGDOP"].asCString());
        if ( result.isMember("GDOPThreshold") )
            vilssysConfig->GDOPThreshold = atoi(result["GDOPThreshold"].asCString());
        if ( result.isMember("CalculationMethod") )
            vilssysConfig->CalculationMethod = atoi(result["CalculationMethod"].asCString());
        if ( result.isMember("EnableIMUVelocity") )
            vilssysConfig->EnableIMUVelocity = atoi(result["EnableIMUVelocity"].asCString());
        if ( result.isMember("IMUVelocityOffset") )
            vilssysConfig->IMUVelocityOffset = atoi(result["IMUVelocityOffset"].asCString());
        if ( result.isMember("SteadyStateOffset") )
            vilssysConfig->SteadyStateOffset = atoi(result["SteadyStateOffset"].asCString());
        if ( result.isMember("WeightMovingAverageValue1") )
            vilssysConfig->WeightMovingAverageValue1 = atof(result["WeightMovingAverageValue1"].asCString());
        if ( result.isMember("WeightMovingAverageValue2") )
            vilssysConfig->WeightMovingAverageValue2 = atof(result["WeightMovingAverageValue2"].asCString());
        if ( result.isMember("WeightMovingAverageValue3") )
            vilssysConfig->WeightMovingAverageValue3 = atof(result["WeightMovingAverageValue3"].asCString());
        if ( result.isMember("StayInWallTimeout") )
            vilssysConfig->StayInWallTimeout = atoi(result["StayInWallTimeout"].asCString());
        if ( result.isMember("EnableFixHeight") )
            vilssysConfig->EnableFixHeight = atoi(result["EnableFixHeight"].asCString());

        if ( result.isMember("SmoothingResult") )
            vilssysConfig->SmoothingResult = atoi(result["SmoothingResult"].asCString());
        if ( result.isMember("SmoothingResultOffset") )
            vilssysConfig->SmoothingResultOffset = atoi(result["SmoothingResultOffset"].asCString());
        if ( result.isMember("SmoothingResultStep") )
            vilssysConfig->SmoothingResultStep = atoi(result["SmoothingResultStep"].asCString());
        if ( result.isMember("SmoothingResultFirstStateOffsetRatio") )
            vilssysConfig->SmoothingResultFirstStateOffsetRatio = atof(result["SmoothingResultFirstStateOffsetRatio"].asCString());
        if ( result.isMember("SmoothingResultFirstStateRunTime") )
            vilssysConfig->SmoothingResultFirstStateRunTime = atoi(result["SmoothingResultFirstStateRunTime"].asCString());
        if ( result.isMember("SmoothingResultSecondStateOffsetRatio") )
            vilssysConfig->SmoothingResultSecondStateOffsetRatio = atof(result["SmoothingResultSecondStateOffsetRatio"].asCString());
        if ( result.isMember("SmoothingResultSecondStateRunTime") )
            vilssysConfig->SmoothingResultSecondStateRunTime = atoi(result["SmoothingResultSecondStateRunTime"].asCString());

        WorkItem * item = new WorkItem("updateAlgorithmConfig", projectid, vilssysConfig);
        mMySQLJobQueue.add(item);

        DataItem * senditem = new DataItem("updateAlgorithmConfig", projectid);
        mMainJobQueue.add(senditem);
    }
}

void WebUDPParserService::updateLoRAWANConfig(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);

        if ( result.isMember("LoRAWANTPOIC") )
            vilssysConfig->LoRAWANTPOIC = result["LoRAWANTPOIC"].asString();
        if ( result.isMember("MQTTHostIp") )
            vilssysConfig->MQTTHostIp = result["MQTTHostIp"].asString();
        if ( result.isMember("MQTTHostPort") )
            vilssysConfig->MQTTHostPort = atoi(result["MQTTHostPort"].asCString());
        if ( result.isMember("MQTTHostUsername") )
            vilssysConfig->MQTTHostUsername = result["MQTTHostUsername"].asString();
        if ( result.isMember("MQTTHostPassword") )
            vilssysConfig->MQTTHostPassword = result["MQTTHostPassword"].asString();
        if ( result.isMember("MQTTClientId") )
            vilssysConfig->MQTTClientId = result["MQTTClientId"].asString();

        WorkItem * item = new WorkItem("updateLoRAWANConfig", projectid, vilssysConfig);
        mMySQLJobQueue.add(item);

        DataItem * senditem = new DataItem("updateLoRAWANConfig", projectid);
        mMainJobQueue.add(senditem);
    }
}

void WebUDPParserService::sendTagAlertToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    int projectid = 1;
    if ( parsingSuccessful )
    {
        projectid = getProjectId(result);
    }

    std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);

    VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);

    std::ostringstream sendstream;

    for (std::list<NodeStruct*>::iterator node = tagNodeList->begin(); node != tagNodeList->end(); node++)
    {
        if ((*node)->alert > 0 || (*node)->alert0D > 0)
        //if ((*node)->alert > 0)
        {
            vilslog::printf("sendTagAlertToClient() macaddress[%s] alert[%d] alert0D[%d]\n", (*node)->macaddress, (*node)->alert, (*node)->alert0D);
            sendstream<<"tagname="<<(*node)->nodename<<endl;
            sendstream<<"tagid="<<(*node)->nodeid<<endl;
            sendstream<<"area="<<(*node)->AreaName<<endl;
            sendstream<<"alert="<<(*node)->alert<<endl;
            sendstream<<"alert0D="<<(*node)->alert0D<<endl;

            // if ((*node)->alert > 0)
            //     (*node)->alert--;
            // if ((*node)->alert0D > 0)
            //     (*node)->alert0D--;

            if ( (*node)->alert == 0 && (*node)->alert0D == 0 )
            {
                vilslog::printf("sendTagAlertToClient() finished! macaddress[%s] (*node)->alert[%d] alert0D[%d]\n", (*node)->macaddress, (*node)->alert, (*node)->alert0D);
            }

            //
            // alert timeout
            //
            time_t currentTime = getCurrentTimeInMilliSec();
            time_t diffTime = currentTime -  (*node)->alertIssuTime;
            //printf("sendTagAlertToClient() alert timeout  diffTime(%.02f)\n", (float)diffTime);
            if ( (float)diffTime > (float)vilssysConfig->TagAlertTimeout)
            {
                (*node)->alert = 0;
                (*node)->alert0D = 0;
            }
            break;
        }
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    //mWebUDPServer->sendToClient(sendBuf, (int)sendString.length());
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendRangingDiagnosisToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();

    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        std::string tag_macaddress = "";
        if ( result.isMember("tagmacaddress") )
            tag_macaddress = result["tagmacaddress"].asString();

        std::string strRanging = "";

        for (std::list<RangingDiagnosisStruct*>::iterator existed = mRangingDiagnosisList.begin(); existed != mRangingDiagnosisList.end(); existed++)
        {
            if ( (*existed)->tagmacaddress.compare(tag_macaddress) == 0 && (*existed)->projectid == projectid)
            {
                strRanging = (*existed)->RangingDiagnosisString;
                break;
            }
        }
        const char * sendBuf = strRanging.c_str();
        //mWebUDPServer->sendToClient(sendBuf, (int)strRanging.length());
        item->mUDPServer->sendToClient(sendBuf, (int)strRanging.length(), (struct sockaddr *)&item->m_client_addr);
    }
    else
    {
        std::ostringstream sendstream;
        std::string sendString = sendstream.str();
        const char * sendBuf = sendString.c_str();
        //mWebUDPServer->sendToClient(sendBuf, (int)strlen(sendBuf));
        item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
    }

}

void WebUDPParserService::sendLocatorRSSIInfoToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();

    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        std::string LoRAMacaddress = "";
        if ( result.isMember("LoRAMacaddress") )
            LoRAMacaddress = result["LoRAMacaddress"].asString();

        std::string strRanging = "";

        for (std::list<LocatorRSSIInfoStringStruct*>::iterator existed = mLocatorRSSIInfoList.begin(); existed != mLocatorRSSIInfoList.end(); existed++)
        {
            if ( (*existed)->LoRAMacaddress.compare(LoRAMacaddress) == 0 && (*existed)->projectId == projectid)
            {
                strRanging = (*existed)->LocatorRSSIInfoString;
                break;
            }
        }
        const char * sendBuf = strRanging.c_str();
        item->mUDPServer->sendToClient(sendBuf, (int)strRanging.length(), (struct sockaddr *)&item->m_client_addr);
    }
    else
    {
        std::ostringstream sendstream;
        std::string sendString = sendstream.str();
        const char * sendBuf = sendString.c_str();
        item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
    }

}

void WebUDPParserService::createSection(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        SectionTypeEnum SectionType = SECTION_TYPE_ZERO; // 0:0D, 1:1D, 2:2D, 3:3D
        std::string AreaID = "";
        int Param1 = 0;// 0D using for distance
        int Param2 = 0;// 0D using for event type 0:normal 1:0D room  2:danger area
        int Param3 = 0;
        int Param4 = 0;
        int Param5 = 0;
        int Param6 = 0;
        std::string maingroups = "";

        int projectid = getProjectId(result);
        if ( result.isMember("SectionType") )
            SectionType = (SectionTypeEnum)result["SectionType"].asInt();
        if ( result.isMember("AreaID") )
            AreaID = result["AreaID"].asString();
        if ( result.isMember("Param1") )
            Param1 = result["Param1"].asInt();
        if ( result.isMember("Param2") )
            Param2 = result["Param2"].asInt();
        if ( result.isMember("Param3") )
            Param3 = result["Param3"].asInt();
        if ( result.isMember("Param4") )
            Param4 = result["Param4"].asInt();
        if ( result.isMember("Param5") )
            Param5 = result["Param5"].asInt();
        if ( result.isMember("Param6") )
            Param6 = result["Param6"].asInt();
        if ( result.isMember("maingroups") )
            maingroups = result["maingroups"].asString();

        //printf("WebUDPParserService::createSection() projectid[%d] SectionType[%d] AreaID[%s] Param1[%d] Param2[%d]\n",
        //    projectid, SectionType, AreaID.c_str(), Param1, Param2);

        SectionGroup * sectiongroup = new SectionGroup();
        sectiongroup->GroupID = -1;
        sectiongroup->projectid = projectid;
        sectiongroup->SectionType = SectionType;
        sectiongroup->AreaID = AreaID;
        sectiongroup->Param1 = Param1;
        sectiongroup->Param2 = Param2;
        sectiongroup->Param3 = Param3;
        sectiongroup->Param4 = Param4;
        sectiongroup->Param5 = Param5;
        sectiongroup->Param6 = Param6;
        sectiongroup->maingroupid = 0;
        sectiongroup->nodeIDList = NULL;

        if (maingroups.length() > 0)
        {
            std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');
            std::string maingroupidstr = maingroupList->front();
            sectiongroup->maingroupid = std::stoi(maingroupidstr);

            maingroupList->clear();
            delete maingroupList;
        }

        if ( result.isMember("nodeIDList") )
        {
            // vilslog::printf("WebUDPParserService::createSection() result.isMember(nodeIDList)\n");

            std::string nodeIDListStr = result["nodeIDList"].asString();
            sectiongroup->nodeIDList = new std::list<std::string>();
            istringstream f(nodeIDListStr.c_str());
            // vilslog::printf("WebUDPParserService::createSection() nodeIDListStr[%s]\n", nodeIDListStr.c_str());
            string s;
            while (getline(f, s, ','))
            {
                // vilslog::printf("WebUDPParserService::createSection() s[%s]\n", s.c_str());
                sectiongroup->nodeIDList->push_back(s);
            }
        }

        // vilslog::printf("WebUDPParserService::createSection() new WorkItem(createsection)\n");
        WorkItem * item = new WorkItem("createsection", projectid, sectiongroup);
        mMySQLJobQueue.add(item);

        if (SectionType == 0)
        {
            // 0D move locator to area center
            std::string id = sectiongroup->nodeIDList->front();
            std::string macaddress = mDeviceManager->getLocatorMacaddress(projectid, id.c_str());
            vilslog::printf("WebUDPParserService::createSection() AreaID[%s] id[%s] macaddress[%s]\n", AreaID.c_str(), id.c_str(), macaddress.c_str());
            if (macaddress.length() > 0)
            {
                DataItem * senditemmain = new DataItem("updatlocatorposition", projectid, id.c_str());
                senditemmain->setMessage(AreaID);
                mMainJobQueue.add(senditemmain);
            }
        }

        //
        // send to PosCalculate
        //
        DataItem * senditemmain = new DataItem("updatesection", projectid);
        mMainJobQueue.add(senditemmain);

        delete sectiongroup->nodeIDList;
        delete sectiongroup;
    }
}

void WebUDPParserService::deleteSection(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int GroupID = -1;
        if ( result.isMember("GroupID") )
            GroupID = result["GroupID"].asInt();

        int projectid = getProjectId(result);

        SectionGroup * sectiongroup = new SectionGroup();
        sectiongroup->GroupID = GroupID;
        sectiongroup->nodeIDList = NULL;

        WorkItem * item = new WorkItem("deletesection", projectid, sectiongroup);
        mMySQLJobQueue.add(item);

        //
        // send to PosCalculate
        //
        DataItem * senditemmain = new DataItem("updatesection", projectid);
        mMainJobQueue.add(senditemmain);

        delete sectiongroup;
    }
}

void WebUDPParserService::createFenceAlert(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        std::string nodeid;
        int distance = 0;// distance

        int projectid = getProjectId(result);
        if ( result.isMember("distance") )
            distance = result["distance"].asInt();
        if ( result.isMember("nodeid") )
            nodeid = result["nodeid"].asString();

        FenceAlertStruct * alertdata = new FenceAlertStruct();
        alertdata->GroupID = -1;
        alertdata->distance = distance;
        alertdata->nodeid = nodeid;

        WorkItem * item = new WorkItem("createfencealert", projectid, alertdata);
        mMySQLJobQueue.add(item);

        //
        // send to PosCalculate
        //
        DataItem * senditemmain = new DataItem("updatefencealert", projectid);
        mMainJobQueue.add(senditemmain);

        delete alertdata;
    }
}

void WebUDPParserService::deleteFenceAlert(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int GroupID = -1;
        if ( result.isMember("GroupID") )
            GroupID = result["GroupID"].asInt();

        int projectid = getProjectId(result);

        FenceAlertStruct * alertdata = new FenceAlertStruct();
        alertdata->GroupID = GroupID;

        WorkItem * item = new WorkItem("deletefencealert", projectid, alertdata);
        mMySQLJobQueue.add(item);

        //
        // send to PosCalculate
        //
        DataItem * senditemmain = new DataItem("updatefencealert", projectid);
        mMainJobQueue.add(senditemmain);

        delete alertdata;
    }
}

void WebUDPParserService::updateArea(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        //
        // send to PosCalculate
        //
        DataItem * senditemmain = new DataItem("updatearea", projectid);
        mMainJobQueue.add(senditemmain);
    }
}

void WebUDPParserService::deleteEvent(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        int eventid = -1;
        if ( result.isMember("eventid") )
            eventid = result["eventid"].asInt();

        EventStruct * event = new EventStruct();
        event->eventid = eventid;

        WorkItem * item = new WorkItem("deleteevent", projectid, event);
        mMySQLJobQueue.add(item);

        delete event;
    }
}

void WebUDPParserService::createEvent(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        EventStruct * event = new EventStruct();

        event->projectid = getProjectId(result);
        event->nodeid = "";
        event->action = "";
        event->areaid = "";
        event->issuetime = "";

        if ( result.isMember("nodeid") )
            event->nodeid = result["nodeid"].asString();
        if ( result.isMember("action") )
            event->action = result["action"].asString();
        if ( result.isMember("areaid") )
            event->areaid = result["areaid"].asString();
        if ( result.isMember("issuetime") )
            event->issuetime = result["issuetime"].asString();

        WorkItem * item = new WorkItem("createevent", event->projectid, event);
        mMySQLJobQueue.add(item);

        delete event;
    }
}

// void WebUDPParserService::deleteMapArea(char * buf, int length)
// {
//     Json::Value result;
//     bool parsingSuccessful = parse_payload_JSON(buf, result);
//
//     if ( parsingSuccessful )
//     {
//         std::string areaid = "";
//         int projectid = getProjectId(result);
//         if ( result.isMember("areaid") )
//             areaid = result["areaid"].asString();
//
//         AreaStruct * area = new AreaStruct();
//         area->areaid = areaid;
//         area->projectid = projectid;
//
//         WorkItem * item = new WorkItem("deletemaparea", projectid, area);
//         mMySQLJobQueue.add(item);
//
//         delete area;
//     }
// }

// void WebUDPParserService::updateMapAreaShape(char * buf, int length)
// {
//     Json::Value result;
//     bool parsingSuccessful = parse_payload_JSON(buf, result);
//
//     if ( parsingSuccessful )
//     {
//         AreaStruct * area = new AreaStruct();
//         area->projectid = getProjectId(result);
//         area->areaid = "";
//         area->areaname = "";
//         area->geojson = "";
//
//         if ( result.isMember("areaid") )
//             area->areaid = result["areaid"].asString();
//         if ( result.isMember("areaname") )
//             area->areaname = result["areaname"].asString();
//         if ( result.isMember("geojson") )
//             area->geojson = result["geojson"].asString();
//
//         replaceAll(area->geojson, "\\\"", "\"");
//
//         //printf("WebUDPParserService::updatemapareashape() geojson[%s]\n", area->geojson.c_str());
//
//         WorkItem * item = new WorkItem("updatemapareashape", area->projectid, area);
//         mMySQLJobQueue.add(item);
//
//         delete area;
//     }
// }

// void WebUDPParserService::updateMapArea(char * buf, int length)
// {
//     Json::Value result;
//     bool parsingSuccessful = parse_payload_JSON(buf, result);
//
//     if ( parsingSuccessful )
//     {
//         AreaStruct * area = new AreaStruct();
//         area->projectid = getProjectId(result);
//         area->areaid = "";
//         area->areaname = "";
//         area->geojson = "";
//         area->type = 0;
//
//         if ( result.isMember("areaid") )
//             area->areaid = result["areaid"].asString();
//         if ( result.isMember("areaname") )
//             area->areaname = result["areaname"].asString();
//         if ( result.isMember("geojson") )
//             area->geojson = result["geojson"].asString();
//         if ( result.isMember("type") )
//             area->type = result["type"].asInt();
//
//         replaceAll(area->geojson, "\\\"", "\"");
//
//         //printf("WebUDPParserService::updateMapArea() geojson[%s]\n", area->geojson.c_str());
//
//         WorkItem * item = new WorkItem("updatemaparea", area->projectid, area);
//         mMySQLJobQueue.add(item);
//
//         delete area;
//     }
// }

// void WebUDPParserService::updateMap(char * buf, int length)
// {
//     Json::Value result;
//     bool parsingSuccessful = parse_payload_JSON(buf, result);
//
//     if ( parsingSuccessful )
//     {
//         MapStruct * map = new MapStruct();
//         map->projectid = getProjectId(result);
//         map->mapid = -1;
//         map->mapname = "";
//         map->POSX = "";
//         map->POSY = "";
//         map->SCALE = "";
//         map->COEFF = "";
//         map->FILE = "";
//         map->MAPWIDTH = "";
//         map->MAPHEIGHT = "";
//
//         if ( result.isMember("PROJECT_ID") )
//             map->projectid = result["PROJECT_ID"].asInt();
//         if ( result.isMember("mapid") )
//             map->mapid = atoi(result["mapid"].asCString());
//         if ( result.isMember("mapname") )
//             map->mapname = result["mapname"].asString();
//         if ( result.isMember("POSX") )
//             map->POSX = result["POSX"].asString();
//         if ( result.isMember("POSY") )
//             map->POSY = result["POSY"].asString();
//         if ( result.isMember("SCALE") )
//             map->SCALE = result["SCALE"].asString();
//         if ( result.isMember("COEFF") )
//             map->COEFF = result["COEFF"].asString();
//         if ( result.isMember("FILE") )
//             map->FILE = result["FILE"].asString();
//         if ( result.isMember("MAPWIDTH") )
//             map->MAPWIDTH = result["MAPWIDTH"].asString();
//         if ( result.isMember("MAPHEIGHT") )
//             map->MAPHEIGHT = result["MAPHEIGHT"].asString();
//
//         WorkItem * item = new WorkItem("updatemap", map->projectid, map);
//         mMySQLJobQueue.add(item);
//
//         delete map;
//     }
// }

void WebUDPParserService::updateMapLayer(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        MapLayerStruct * maplayer = new MapLayerStruct();
        maplayer->projectid = getProjectId(result);
        maplayer->layerid = 1;
        maplayer->maingroupid = 1;
        maplayer->layer_name = "";
        maplayer->layer_description = "";
        maplayer->WGS48OriginX = "0.0";
        maplayer->WGS48OriginY = "0.0";
        maplayer->MAPNorth = "0.0";
        maplayer->BuildingID = 1;
        maplayer->BuildingFloor = 1;
        maplayer->POSX = "";
        maplayer->POSY = "";
        maplayer->SCALE = "";
        maplayer->COEFF = "";
        maplayer->FILE = "";
        maplayer->MAPWIDTH = "";
        maplayer->MAPHEIGHT = "";

        if ( result.isMember("layerid") )
            maplayer->layerid = atoi(result["layerid"].asCString());
        if ( result.isMember("maingroupid") )
            maplayer->maingroupid = atoi(result["maingroupid"].asCString());
        if ( result.isMember("layer_name") )
            maplayer->layer_name = result["layer_name"].asString();
        if ( result.isMember("layer_description") )
            maplayer->layer_description = result["layer_description"].asString();
        if ( result.isMember("WGS48OriginX") )
            maplayer->WGS48OriginX = result["WGS48OriginX"].asString();
        if ( result.isMember("WGS48OriginY") )
            maplayer->WGS48OriginY = result["WGS48OriginY"].asString();
        if ( result.isMember("MAP_North") )
            maplayer->MAPNorth = result["MAP_North"].asString();
        if ( result.isMember("BuildingID") )
            maplayer->BuildingID = atoi(result["BuildingID"].asCString());
        if ( result.isMember("BuildingFloor") )
            maplayer->BuildingFloor = atoi(result["BuildingFloor"].asCString());
        if ( result.isMember("POSX") )
            maplayer->POSX = result["POSX"].asString();
        if ( result.isMember("POSY") )
            maplayer->POSY = result["POSY"].asString();
        if ( result.isMember("SCALE") )
            maplayer->SCALE = result["SCALE"].asString();
        if ( result.isMember("COEFF") )
            maplayer->COEFF = result["COEFF"].asString();
        if ( result.isMember("FILE") )
            maplayer->FILE = result["FILE"].asString();
        if ( result.isMember("MAPWIDTH") )
            maplayer->MAPWIDTH = result["MAPWIDTH"].asString();
        if ( result.isMember("MAPHEIGHT") )
            maplayer->MAPHEIGHT = result["MAPHEIGHT"].asString();

        WorkItem * item = new WorkItem("updatemaplayer", maplayer->projectid, maplayer);
        mMySQLJobQueue.add(item);

        delete maplayer;
    }
}

void WebUDPParserService::updateMapLayerArea(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        MapLayerAreaStruct * area = new MapLayerAreaStruct();
        area->layerid = 0;
        area->areaid = "";
        area->areaname = "";
        area->geojson = "";
        area->type = 0;

        if ( result.isMember("layerid") )
            area->layerid = result["layerid"].asInt();
        if ( result.isMember("areaid") )
            area->areaid = result["areaid"].asString();
        if ( result.isMember("areaname") )
            area->areaname = result["areaname"].asString();
        if ( result.isMember("geojson") )
            area->geojson = result["geojson"].asString();
        if ( result.isMember("type") )
            area->type = result["type"].asInt();

        replaceAll(area->geojson, "\\\"", "\"");

        WorkItem * item = new WorkItem("updatemaplayerarea", projectid, area);
        mMySQLJobQueue.add(item);

        delete area;
    }
}

void WebUDPParserService::updateMapLayerAreaShape(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        MapLayerAreaStruct * area = new MapLayerAreaStruct();
        area->layerid = 0;
        area->areaid = "";
        area->areaname = "";
        area->geojson = "";
        area->type = 0;

        if ( result.isMember("layerid") )
            area->layerid = result["layerid"].asInt();
        if ( result.isMember("areaid") )
            area->areaid = result["areaid"].asString();
        if ( result.isMember("areaname") )
            area->areaname = result["areaname"].asString();
        if ( result.isMember("geojson") )
            area->geojson = result["geojson"].asString();

        replaceAll(area->geojson, "\\\"", "\"");

        WorkItem * item = new WorkItem("updatemaplayerareashape", projectid, area);
        mMySQLJobQueue.add(item);

        delete area;
    }
}

void WebUDPParserService::deleteMapLayerArea(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        MapLayerAreaStruct * area = new MapLayerAreaStruct();
        area->layerid = 0;
        area->areaid = "";
        area->areaname = "";
        area->geojson = "";
        area->type = 0;

        if ( result.isMember("layerid") )
            area->layerid = result["layerid"].asInt();
        if ( result.isMember("areaid") )
            area->areaid = result["areaid"].asString();

        replaceAll(area->geojson, "\\\"", "\"");

        WorkItem * item = new WorkItem("deletemaplayerarea", projectid, area);
        mMySQLJobQueue.add(item);

        delete area;
    }
}

void WebUDPParserService::createInspectionGroup(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    printf("WebUDPParserService::createInspectionGroup() parsingSuccessful[%d] buf[%s]\n", parsingSuccessful, buf);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        // std::string GroupName = "";
        // std::string StartTime = "";
        // std::string EndTime = "";
        // std::string issuetime = "";
        // int AlertTime = 0;
        //
        // if ( result.isMember("GroupName") )
        //     GroupName = result["GroupName"].asString();
        // if ( result.isMember("StartTime") )
        //     StartTime = result["StartTime"].asString();
        // if ( result.isMember("EndTime") )
        //     EndTime = result["EndTime"].asString();
        // if ( result.isMember("AlertTime") )
        //     AlertTime = atoi(result["AlertTime"].asCString());
        //
        // InspectionGroupStruct inspectionGroup;
        // inspectionGroup.GroupName = GroupName;
        // inspectionGroup.StartTime = StartTime;
        // inspectionGroup.EndTime = EndTime;
        // inspectionGroup.AlertTime = AlertTime;
        //
        // WorkItem * item = new WorkItem("createinspectiongroup", projectid, inspectionGroup);
        // mMySQLJobQueue.add(item);

        DataItem * senditem = new DataItem("updateinspection", projectid);
        mMainJobQueue.add(senditem);
    }
}

void WebUDPParserService::updateInspectionGroup(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    // printf("WebUDPParserService::updateInspectionGroup() parsingSuccessful[%d] buf[%s]\n", parsingSuccessful, buf);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        // int GroupID = -1;
        // std::string GroupName = "";
        // std::string StartTime = "";
        // std::string EndTime = "";
        // std::string issuetime = "";
        // int AlertTime = 0;
        //
        // if ( result.isMember("GroupID") )
        //     GroupID = atoi(result["GroupID"].asCString());
        // if ( result.isMember("GroupName") )
        //     GroupName = result["GroupName"].asString();
        // if ( result.isMember("StartTime") )
        //     StartTime = result["StartTime"].asString();
        // if ( result.isMember("EndTime") )
        //     EndTime = result["EndTime"].asString();
        // if ( result.isMember("AlertTime") )
        //     AlertTime = atoi(result["AlertTime"].asCString());
        //
        // InspectionGroupStruct inspectionGroup;
        // inspectionGroup.GroupID = GroupID;
        // inspectionGroup.GroupName = GroupName;
        // inspectionGroup.StartTime = StartTime;
        // inspectionGroup.EndTime = EndTime;
        // inspectionGroup.AlertTime = AlertTime;
        //
        // WorkItem * item = new WorkItem("updateinspectiongroup", projectid, inspectionGroup);
        // mMySQLJobQueue.add(item);

        DataItem * senditem = new DataItem("updateinspection", projectid);
        mMainJobQueue.add(senditem);
    }
}

void WebUDPParserService::deleteInspectionGroup(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    printf("WebUDPParserService::deleteInspectionGroup() parsingSuccessful[%d] buf[%s]\n", parsingSuccessful, buf);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        DataItem * senditem = new DataItem("updateinspection", projectid);
        mMainJobQueue.add(senditem);
    }
}

void WebUDPParserService::createInspectionPoint(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    printf("WebUDPParserService::createInspectionPoint() parsingSuccessful[%d] buf[%s]\n", parsingSuccessful, buf);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        DataItem * senditem = new DataItem("updateinspection", projectid);
        mMainJobQueue.add(senditem);
    }
}

void WebUDPParserService::updateInspectionPoint(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    printf("WebUDPParserService::updateInspectionPoint() parsingSuccessful[%d] buf[%s]\n", parsingSuccessful, buf);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        DataItem * senditem = new DataItem("updateinspection", projectid);
        mMainJobQueue.add(senditem);
    }
}

void WebUDPParserService::deleteInspectionPoint(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    printf("WebUDPParserService::deleteInspectionPoint() parsingSuccessful[%d] buf[%s]\n", parsingSuccessful, buf);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        DataItem * senditem = new DataItem("updateinspection", projectid);
        mMainJobQueue.add(senditem);
    }
}

void WebUDPParserService::SendInspectionPDFReportToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    printf("WebUDPParserService::SendInspectionPDFReportToClient() parsingSuccessful[%d]\n", parsingSuccessful);

    std::ostringstream sendstream;

    if ( parsingSuccessful )
    {
        //int projectid = getProjectId(result);
        std::string type = "";
        std::string id = "";
        PDFTableDataStruct pdfData;
        pdfData.type = "";
        //std::list<std::string> headerlist;

        if ( result.isMember("type") )
        {
            type = result["type"].asString();
            pdfData.type = result["type"].asString();
        }
        if ( result.isMember("startdate") )
        {
            pdfData.startdate = result["startdate"].asString();
        }
        if ( result.isMember("enddate") )
        {
            pdfData.enddate = result["enddate"].asString();
        }

        // vilslog::printf("WebUDPParserService::SendInspectionPDFReportToClient() projectid[%d] type[%s]\n", projectid, type.c_str());

        if ( result.isMember("header") )
        {
            //vilslog::printf("WebUDPParserService::SendInspectionPDFReportToClient() header\n");

            if ( result["header"].isArray() )
            {
                // vilslog::printf("WebUDPParserService::SendInspectionPDFReportToClient() header isArray\n");
                for(int i=0;i<(int)result["header"].size();i++)
                {
                    Json::Value header = result["header"][i];
                    //pdfData.headerlist.push_back(header.asString());
                    if (header.asString().compare("巡檢員") == 0)
                        pdfData.headerlist.push_back(create_pdf_headerdata(header.asString(), 80));
                    else
                    if (header.asString().compare("平均分") == 0)
                        pdfData.headerlist.push_back(create_pdf_headerdata(header.asString(), 30));
                    else
                    {
                        if (type.compare("daily") == 0)
                            pdfData.headerlist.push_back(create_pdf_headerdata(header.asString(), 50));
                        else
                            pdfData.headerlist.push_back(create_pdf_headerdata(header.asString(), 25));
                    }
                    //vilslog::printf("WebUDPParserService::SendInspectionPDFReportToClient() [%d] header[%s]\n", i, header.asCString());
                }
            }

        }

        if ( result.isMember("data") )
        {
            if ( result["data"].isObject() )
            {
                Json::Value resultData = result["data"];

                //vilslog::printf("WebUDPParserService::SendInspectionPDFReportToClient() data isObject size()[%d]\n", resultData.size());

                std::vector<std::string> Members = resultData.getMemberNames();

                //vilslog::printf("WebUDPParserService::SendInspectionPDFReportToClient() Members.size()[%d]\n", Members.size());

                for(unsigned int i=0;i<Members.size();i++)
                {
                    // vilslog::printf("WebUDPParserService::SendInspectionPDFReportToClient() member[%s]\n", Members[i].c_str());
                    PDFTableRowDataStruct * newRowData = new PDFTableRowDataStruct();

                    Json::Value member = resultData[Members[i].c_str()];
                    if (member.isObject() )
                    {
                        // vilslog::printf("WebUDPParserService::SendInspectionPDFReportToClient() isObject member.size()[%d]\n", (int)member.size());

                        std::vector<std::string> targetHeaders = member.getMemberNames();
                        for(unsigned int i=0;i<targetHeaders.size();i++)
                        {
                            // vilslog::printf("WebUDPParserService::SendInspectionPDFReportToClient() [%d] targetHeader[%s] ", i, targetHeaders[i].c_str());
                            Json::Value rowData = member[targetHeaders[i].c_str()];
                            if (rowData.isString() )
                            {
                                // vilslog::printf("rowData[%s]\n", rowData.asCString());

                                PDFTableColDataStruct * newData = new PDFTableColDataStruct();
                                newData->header = targetHeaders[i];
                                newData->data = rowData.asString();
                                newRowData->coldatalist.push_back(newData);
                            }
                        }
                    }
                    pdfData.rowdatalist.push_back(newRowData);
                }

                vilslog::printf("WebUDPParserService::SendInspectionPDFReportToClient() pdfData.type[%s] startdate[%s] enddate[%s]\n",
                    pdfData.type.c_str(), pdfData.startdate.c_str(), pdfData.enddate.c_str());
                vilslog::printf("WebUDPParserService::SendInspectionPDFReportToClient() pdfData.headerlist.size[%d]\n", (int)pdfData.headerlist.size());
                vilslog::printf("WebUDPParserService::SendInspectionPDFReportToClient() pdfData.rowdatalist.size[%d]\n", (int)pdfData.rowdatalist.size());
            }
        }

        if (type.length() == 0)
        {
            sendstream<<"{\"pdfreport\":\"\",\"success\":\"false\"}";
        }
        else
        {
            std::string randomFilename = random_string(16);

            std::string pdffilename = "/upload/";
            pdffilename.append(randomFilename);
            pdffilename.append(".pdf");

            std::string pdfOutputfile = OUTPUT_DIRECTORY;
            pdfOutputfile.append(pdffilename);

            vilslog::printf("WebUDPParserService::SendInspectionPDFReportToClient() pdfOutputfile[%s]\n", pdfOutputfile.c_str());

            std::string report_date = TodayDateType1();
            std::string report_start_date = convertToShowDate(pdfData.startdate);
            std::string report_end_date = convertToShowDate(pdfData.enddate);

            PDFCreater * pdfCreater = new PDFCreater();
            pdfCreater->create_inspection_report(pdfOutputfile, report_date, report_start_date, report_end_date, &pdfData);
            delete pdfCreater;

            // put to remove queue
            QRCodeStruct * newQRCcode = new QRCodeStruct();
            newQRCcode->filepath = pdfOutputfile;
            newQRCcode->createtime = getCurrentTimeInSec();
            mQRCodeImageList.push_back(newQRCcode);

            sendstream<<"{\"pdfreport\":\"";
            sendstream<<pdffilename;
            sendstream<<"\",\"success\":\"true\"}";

            clear_PDFTableDataStruct(&pdfData);

            // pdfData.headerlist.clear();
            // for (std::list<PDFTableRowDataStruct *>::iterator row = pdfData.rowdatalist.begin(); row != pdfData.rowdatalist.end(); row++)
            // {
            //     for (std::list<PDFTableColDataStruct *>::iterator col = (*row)->coldatalist.begin(); col != (*row)->coldatalist.end(); col++)
            //     {
            //         delete (*col);
            //     }
            //     (*row)->coldatalist.clear();
            //     delete (*row);
            // }
            // pdfData.rowdatalist.clear();
        }
    }
    else
    {
        sendstream<<"{\"pdfreport\":\"\",\"success\":\"false\"}";
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::SendBodyInfoPDFReportToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    vilslog::printf("WebUDPParserService::SendBodyInfoPDFReportToClient() parsingSuccessful[%d]\n", parsingSuccessful);

    std::ostringstream sendstream;

    if ( parsingSuccessful )
    {
        //int projectid = getProjectId(result);
        std::string nodeid = "";
        std::string nodename = "";
        std::string startdate = "";
        std::string enddate = "";
        PDFTableDataStruct pdfDataWeight;
        PDFTableDataStruct pdfDataHeartbeat;
        pdfDataWeight.type = "weight";
        pdfDataHeartbeat.type = "heartbeat";

        pdfDataWeight.headerlist.push_back(create_pdf_headerdata("日期", 120));
        pdfDataWeight.headerlist.push_back(create_pdf_headerdata("體重", 50));

        pdfDataHeartbeat.headerlist.push_back(create_pdf_headerdata("日期", 120));
        pdfDataHeartbeat.headerlist.push_back(create_pdf_headerdata("心率", 50));

        if ( result.isMember("nodeid") )
        {
            nodeid = result["nodeid"].asString();
        }
        if ( result.isMember("nodename") )
        {
            nodename = result["nodename"].asString();
        }
        if ( result.isMember("startdate") )
        {
            startdate = result["startdate"].asString();
            pdfDataWeight.startdate = result["startdate"].asString();
            pdfDataHeartbeat.startdate = result["startdate"].asString();
        }
        if ( result.isMember("enddate") )
        {
            enddate = result["enddate"].asString();
            pdfDataWeight.enddate = result["enddate"].asString();
            pdfDataHeartbeat.enddate = result["enddate"].asString();
        }

        vilslog::printf("WebUDPParserService::SendBodyInfoPDFReportToClient() nodeid[%s] nodename[%s] startdate[%s] enddate[%s]\n",
            nodeid.c_str(), nodename.c_str(), startdate.c_str(), enddate.c_str());

        if ( result["weight"].isArray() )
        {
            // vilslog::printf("WebUDPParserService::SendBodyInfoPDFReportToClient() weight isArray\n");
            for(int i=0;i<(int)result["weight"].size();i++)
            {
                Json::Value weightData = result["weight"][i];

                PDFTableRowDataStruct * newRowData = new PDFTableRowDataStruct();

                std::string weight;
                std::string datetime;
                if ( weightData.isMember("weight") )
                {
                    weight = weightData["weight"].asString();
                }
                if ( weightData.isMember("datetime") )
                {
                    datetime = weightData["datetime"].asString();
                }

                newRowData->coldatalist.push_back(create_pdf_columndata("體重", weight));
                newRowData->coldatalist.push_back(create_pdf_columndata("日期", datetime));

                // PDFTableColDataStruct * newData1 = new PDFTableColDataStruct();
                // newData1->header = "體重";
                // newData1->data = weight;
                // newRowData->coldatalist.push_back(newData1);
                //
                // PDFTableColDataStruct * newData2 = new PDFTableColDataStruct();
                // newData2->header = "日期";
                // newData2->data = datetime;
                // newRowData->coldatalist.push_back(newData2);

                //pdfDataWeight.rowdatalist.push_back(newRowData);
                pdfDataWeight.rowdatalist.push_front(newRowData);

                // vilslog::printf("WebUDPParserService::SendBodyInfoPDFReportToClient() [%d] weight[%s] datetime[%s]\n", i, weight.c_str(), datetime.c_str());
            }
        }

        if ( result["heartbeat"].isArray() )
        {
            // vilslog::printf("WebUDPParserService::SendBodyInfoPDFReportToClient() heartbeat isArray\n");
            for(int i=0;i<(int)result["heartbeat"].size();i++)
            {
                Json::Value heartbeatData = result["heartbeat"][i];

                PDFTableRowDataStruct * newRowData = new PDFTableRowDataStruct();

                std::string heartbeat;
                std::string datetime;
                if ( heartbeatData.isMember("heartbeat") )
                {
                    heartbeat = heartbeatData["heartbeat"].asString();
                }
                if ( heartbeatData.isMember("datetime") )
                {
                    datetime = heartbeatData["datetime"].asString();
                }

                newRowData->coldatalist.push_back(create_pdf_columndata("心率", heartbeat));
                newRowData->coldatalist.push_back(create_pdf_columndata("日期", datetime));

                // PDFTableColDataStruct * newData1 = new PDFTableColDataStruct();
                // newData1->header = "心率";
                // newData1->data = heartbeat;
                // newRowData->coldatalist.push_back(newData1);
                //
                // PDFTableColDataStruct * newData2 = new PDFTableColDataStruct();
                // newData2->header = "日期";
                // newData2->data = datetime;
                // newRowData->coldatalist.push_back(newData2);

                //pdfDataHeartbeat.rowdatalist.push_back(newRowData);
                pdfDataHeartbeat.rowdatalist.push_front(newRowData);

                // vilslog::printf("WebUDPParserService::SendBodyInfoPDFReportToClient() [%d] heartbeat[%s] datetime[%s]\n", i, heartbeat.c_str(), datetime.c_str());
            }
        }

        std::string randomFilename = random_string(16);

        std::string pdffilename = "/upload/";
        pdffilename.append(randomFilename);
        pdffilename.append(".pdf");

        std::string pdfOutputfile = OUTPUT_DIRECTORY;
        pdfOutputfile.append(pdffilename);

        vilslog::printf("WebUDPParserService::SendBodyInfoPDFReportToClient() pdfOutputfile[%s]\n", pdfOutputfile.c_str());
        vilslog::printf("WebUDPParserService::SendBodyInfoPDFReportToClient() pdfDataWeight.rowdatalist.size()[%d] pdfDataHeartbeat.rowdatalist.size()[%d]\n",
            (int)pdfDataWeight.rowdatalist.size(), (int)pdfDataHeartbeat.rowdatalist.size());


        std::string report_date = TodayDateType1();
        std::string report_start_date = pdfDataHeartbeat.startdate;
        std::string report_end_date = pdfDataHeartbeat.enddate;

        PDFCreater * pdfCreater = new PDFCreater();

        pdfCreater->create_report_header(pdfOutputfile, "生理量測紀錄", report_date, report_start_date, report_end_date);
        pdfCreater->create_report_userinfo("人員", nodename);
        pdfCreater->create_report_table(&pdfDataWeight);
        pdfCreater->create_report_table(&pdfDataHeartbeat);
        pdfCreater->generate_report();

        delete pdfCreater;

        // put to remove queue
        QRCodeStruct * newQRCcode = new QRCodeStruct();
        newQRCcode->filepath = pdfOutputfile;
        newQRCcode->createtime = getCurrentTimeInSec();
        mQRCodeImageList.push_back(newQRCcode);

        sendstream<<"{\"pdfreport\":\"";
        sendstream<<pdffilename;
        sendstream<<"\",\"success\":\"true\"}";

        clear_PDFTableDataStruct(&pdfDataWeight);
        clear_PDFTableDataStruct(&pdfDataHeartbeat);

        // pdfDataWeight.headerlist.clear();
        // for (std::list<PDFTableRowDataStruct *>::iterator row = pdfDataWeight.rowdatalist.begin(); row != pdfDataWeight.rowdatalist.end(); row++)
        // {
        //     for (std::list<PDFTableColDataStruct *>::iterator col = (*row)->coldatalist.begin(); col != (*row)->coldatalist.end(); col++)
        //     {
        //         delete (*col);
        //     }
        //     (*row)->coldatalist.clear();
        //     delete (*row);
        // }
        // pdfDataWeight.rowdatalist.clear();
        //
        // pdfDataHeartbeat.headerlist.clear();
        // for (std::list<PDFTableRowDataStruct *>::iterator row = pdfDataHeartbeat.rowdatalist.begin(); row != pdfDataHeartbeat.rowdatalist.end(); row++)
        // {
        //     for (std::list<PDFTableColDataStruct *>::iterator col = (*row)->coldatalist.begin(); col != (*row)->coldatalist.end(); col++)
        //     {
        //         delete (*col);
        //     }
        //     (*row)->coldatalist.clear();
        //     delete (*row);
        // }
        // pdfDataHeartbeat.rowdatalist.clear();
    }
    else
    {
        sendstream<<"{\"pdfreport\":\"\",\"success\":\"false\"}";
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::SendUserBodyInfoPDFReportToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    vilslog::printf("WebUDPParserService::SendUserBodyInfoPDFReportToClient() parsingSuccessful[%d]\n", parsingSuccessful);

    std::ostringstream sendstream;

    if ( parsingSuccessful )
    {
        std::string accountid = "";
        std::string username = "";
        std::string startdate = "";
        std::string enddate = "";
        PDFTableDataStruct pdfDataWeight;
        PDFTableDataStruct pdfDataHeartbeat;
        PDFTableDataStruct pdfDatabloodsugar;
        PDFTableDataStruct pdfDatasystolicbloodpressure;
        PDFTableDataStruct pdfDatadiastolicbloodpressure;
        PDFTableDataStruct pdfDatabodytemperature;
        PDFTableDataStruct pdfDatabodyfat;
        PDFTableDataStruct pdfDatabloodoxygen;

        pdfDataWeight.type = "weight";
        pdfDataHeartbeat.type = "heartbeat";
        pdfDatabloodsugar.type = "bloodsugar";
        pdfDatasystolicbloodpressure.type = "systolicbloodpressure";
        pdfDatadiastolicbloodpressure.type = "diastolicbloodpressure";
        pdfDatabodytemperature.type = "bodytemperature";
        pdfDatabodyfat.type = "bodyfat";
        pdfDatabloodoxygen.type = "bloodoxygen";

        pdfDataWeight.headerlist.push_back(create_pdf_headerdata("日期", 120));
        pdfDataWeight.headerlist.push_back(create_pdf_headerdata("體重", 50));

        pdfDataHeartbeat.headerlist.push_back(create_pdf_headerdata("日期", 120));
        pdfDataHeartbeat.headerlist.push_back(create_pdf_headerdata("心率", 50));

        pdfDatabloodsugar.headerlist.push_back(create_pdf_headerdata("日期", 120));
        pdfDatabloodsugar.headerlist.push_back(create_pdf_headerdata("血糖", 50));

        pdfDatasystolicbloodpressure.headerlist.push_back(create_pdf_headerdata("日期", 120));
        pdfDatasystolicbloodpressure.headerlist.push_back(create_pdf_headerdata("收縮壓", 50));

        pdfDatadiastolicbloodpressure.headerlist.push_back(create_pdf_headerdata("日期", 120));
        pdfDatadiastolicbloodpressure.headerlist.push_back(create_pdf_headerdata("舒張壓", 50));

        pdfDatabodytemperature.headerlist.push_back(create_pdf_headerdata("日期", 120));
        pdfDatabodytemperature.headerlist.push_back(create_pdf_headerdata("體溫", 50));

        pdfDatabodyfat.headerlist.push_back(create_pdf_headerdata("日期", 120));
        pdfDatabodyfat.headerlist.push_back(create_pdf_headerdata("體脂率", 50));

        pdfDatabloodoxygen.headerlist.push_back(create_pdf_headerdata("日期", 120));
        pdfDatabloodoxygen.headerlist.push_back(create_pdf_headerdata("血氧", 50));

        if ( result.isMember("accountid") )
        {
            accountid = result["accountid"].asString();
        }
        if ( result.isMember("username") )
        {
            username = result["username"].asString();
        }
        if ( result.isMember("startdate") )
        {
            startdate = result["startdate"].asString();
            pdfDataWeight.startdate = result["startdate"].asString();
            pdfDataHeartbeat.startdate = result["startdate"].asString();
            pdfDatabloodsugar.startdate = result["startdate"].asString();
            pdfDatasystolicbloodpressure.startdate = result["startdate"].asString();
            pdfDatadiastolicbloodpressure.startdate = result["startdate"].asString();
            pdfDatabodytemperature.startdate = result["startdate"].asString();
            pdfDatabodyfat.startdate = result["startdate"].asString();
            pdfDatabloodoxygen.startdate = result["startdate"].asString();
        }
        if ( result.isMember("enddate") )
        {
            enddate = result["enddate"].asString();
            pdfDataWeight.enddate = result["enddate"].asString();
            pdfDataHeartbeat.enddate = result["enddate"].asString();
            pdfDatabloodsugar.enddate = result["startdate"].asString();
            pdfDatasystolicbloodpressure.enddate = result["startdate"].asString();
            pdfDatadiastolicbloodpressure.enddate = result["startdate"].asString();
            pdfDatabodytemperature.enddate = result["startdate"].asString();
            pdfDatabodyfat.enddate = result["startdate"].asString();
            pdfDatabloodoxygen.enddate = result["startdate"].asString();
        }

        int datacount = fetchArrayValue(result, &pdfDataWeight, "weight", "體重");
        datacount += fetchArrayValue(result, &pdfDataHeartbeat, "heartbeat", "心率");
        datacount += fetchArrayValue(result, &pdfDatabloodsugar, "bloodsugar", "血糖");
        datacount += fetchArrayValue(result, &pdfDatasystolicbloodpressure, "systolicbloodpressure", "收縮壓");
        datacount += fetchArrayValue(result, &pdfDatadiastolicbloodpressure, "diastolicbloodpressure", "舒張壓");
        datacount += fetchArrayValue(result, &pdfDatabodytemperature, "bodytemperature", "體溫");
        datacount += fetchArrayValue(result, &pdfDatabodyfat, "bodyfat", "體脂率");
        datacount += fetchArrayValue(result, &pdfDatabloodoxygen, "bloodoxygen", "血氧");

        vilslog::printf("WebUDPParserService::SendUserBodyInfoPDFReportToClient() datacount[%d]\n", datacount);

        std::string randomFilename = random_string(16);

        std::string pdffilename = "/upload/";
        pdffilename.append(randomFilename);
        pdffilename.append(".pdf");

        std::string pdfOutputfile = OUTPUT_DIRECTORY;
        pdfOutputfile.append(pdffilename);

        vilslog::printf("WebUDPParserService::SendUserBodyInfoPDFReportToClient() pdfOutputfile[%s]\n", pdfOutputfile.c_str());
        vilslog::printf("WebUDPParserService::SendUserBodyInfoPDFReportToClient() pdfDataWeight.rowdatalist.size()[%d] pdfDataHeartbeat.rowdatalist.size()[%d]\n",
            (int)pdfDataWeight.rowdatalist.size(), (int)pdfDataHeartbeat.rowdatalist.size());

        std::string report_date = TodayDateType1();
        std::string report_start_date = pdfDataHeartbeat.startdate;
        std::string report_end_date = pdfDataHeartbeat.enddate;

        PDFCreater * pdfCreater = new PDFCreater();

        pdfCreater->create_report_header(pdfOutputfile, "生理量測紀錄", report_date, report_start_date, report_end_date);
        pdfCreater->create_report_userinfo("人員", username);
        pdfCreater->create_report_table(&pdfDataWeight);
        pdfCreater->create_report_table(&pdfDataHeartbeat);
        pdfCreater->create_report_table(&pdfDatabloodsugar);
        pdfCreater->create_report_table(&pdfDatasystolicbloodpressure);
        pdfCreater->create_report_table(&pdfDatadiastolicbloodpressure);
        pdfCreater->create_report_table(&pdfDatabodytemperature);
        pdfCreater->create_report_table(&pdfDatabodyfat);
        pdfCreater->create_report_table(&pdfDatabloodoxygen);
        pdfCreater->generate_report();

        delete pdfCreater;

        // put to remove queue
        QRCodeStruct * newQRCcode = new QRCodeStruct();
        newQRCcode->filepath = pdfOutputfile;
        newQRCcode->createtime = getCurrentTimeInSec();
        mQRCodeImageList.push_back(newQRCcode);

        sendstream<<"{\"pdfreport\":\"";
        sendstream<<pdffilename;
        sendstream<<"\",\"success\":\"true\"}";

        clear_PDFTableDataStruct(&pdfDataWeight);
        clear_PDFTableDataStruct(&pdfDataHeartbeat);
        clear_PDFTableDataStruct(&pdfDatabloodsugar);
        clear_PDFTableDataStruct(&pdfDatasystolicbloodpressure);
        clear_PDFTableDataStruct(&pdfDatadiastolicbloodpressure);
        clear_PDFTableDataStruct(&pdfDatabodytemperature);
        clear_PDFTableDataStruct(&pdfDatabodyfat);
        clear_PDFTableDataStruct(&pdfDatabloodoxygen);
    }
    else
    {
        sendstream<<"{\"pdfreport\":\"\",\"success\":\"false\"}";
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::SendCoursePDFReportToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    vilslog::printf("WebUDPParserService::SendCoursePDFReportToClient() parsingSuccessful[%d]\n", parsingSuccessful);

    std::ostringstream sendstream;

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        std::string courseId = "";
        std::string date = "";

        if ( result.isMember("courseId") )
        {
            courseId = result["courseId"].asString();
        }
        if ( result.isMember("date") )
        {
            date = result["date"].asString();
        }
        //vilslog::printf("WebUDPParserService::SendCoursePDFReportToClient() projectid[%d] courseId[%s] date[%s]\n", projectid, courseId.c_str(), date.c_str());

        TrackDBAccess * trackDb = mDbManager->getTrackDB(projectid);
        if (trackDb != NULL)
        {
            PDFTableDataStruct pdfDataCourseInfo;
            pdfDataCourseInfo.type = "courseinfo";
            pdfDataCourseInfo.startdate = date;
            pdfDataCourseInfo.enddate = date;

            MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
            if (baseDb != NULL)
            {
                CourseStruct * course = baseDb->read_Course(courseId.c_str());
                if (course != NULL)
                {
                    pdfDataCourseInfo.headerlist.push_back(create_pdf_headerdata("課程名稱", 120));
                    pdfDataCourseInfo.headerlist.push_back(create_pdf_headerdata("課程描述", 150));
                    pdfDataCourseInfo.headerlist.push_back(create_pdf_headerdata("日期時間", 150));

                    PDFTableRowDataStruct * newRowData = new PDFTableRowDataStruct();
                    newRowData->coldatalist.push_back(create_pdf_columndata("課程名稱", course->title));
                    newRowData->coldatalist.push_back(create_pdf_columndata("課程描述", course->description));
                    pdfDataCourseInfo.rowdatalist.push_back(newRowData);

                    std::ostringstream datetimestream;

                    if ((course->startRecur.length() > 0) && (course->endRecur.length() > 0) )
                    {
                        datetimestream<<"日期從 "<< course->startRecur << " 到 " << course->endRecur << endl;
                        if ( course->allDay == 1 )
                        {
                            datetimestream<<" 全天";
                        }
                        else
                        {
                            datetimestream<<"時間從 "<< course->startTime << " 到 " << course->endTime;
                        }
                    }
                    else
                    {
                        if (course->start.compare(course->end) == 0 )
                        {
                            datetimestream<<"日期 "<< course->start;
                        }
                        else
                        {
                            datetimestream<<"日期從 "<< course->start << " 到 " << course->end << endl;
                        }

                        if ( course->allDay == 1 )
                        {
                            datetimestream<<" 全天";
                        }
                        else
                        {
                            datetimestream<<"時間從 "<< course->startTime << " 到 " << course->endTime;
                        }
                    }
                    newRowData->coldatalist.push_back(create_pdf_columndata("日期時間", datetimestream.str()));

                    delete course;
                }
            }

            PDFTableDataStruct pdfDataCourseRecord;
            pdfDataCourseRecord.type = "courserecord";
            pdfDataCourseRecord.startdate = date;
            pdfDataCourseRecord.enddate = date;

            pdfDataCourseRecord.headerlist.push_back(create_pdf_headerdata("#", 20));
            pdfDataCourseRecord.headerlist.push_back(create_pdf_headerdata("人員名稱", 80));
            pdfDataCourseRecord.headerlist.push_back(create_pdf_headerdata("開始上課時間", 100));
            pdfDataCourseRecord.headerlist.push_back(create_pdf_headerdata("離開課程時間", 100));
            pdfDataCourseRecord.headerlist.push_back(create_pdf_headerdata("上課總時間", 100));

            std::list<CourseDayTrackRecordStruct *> * infoList = trackDb->getCourseRecordList(courseId, date);

            //vilslog::printf("WebUDPParserService::SendCoursePDFReportToClient() infoList->size()[%d]\n", (int)infoList->size());

            int index = 1;
            for (auto info = infoList->begin(); info != infoList->end(); info++)
            {
                PDFTableRowDataStruct * newRowData = new PDFTableRowDataStruct();
                newRowData->coldatalist.push_back(create_pdf_columndata("#", std::to_string(index)));
                newRowData->coldatalist.push_back(create_pdf_columndata("人員名稱", (*info)->nodeid));
                newRowData->coldatalist.push_back(create_pdf_columndata("開始上課時間", (*info)->min_time));
                newRowData->coldatalist.push_back(create_pdf_columndata("離開課程時間", (*info)->max_time));
                newRowData->coldatalist.push_back(create_pdf_columndata("上課總時間", (*info)->total_time));
                pdfDataCourseRecord.rowdatalist.push_back(newRowData);
                index++;
            }

            std::string randomFilename = random_string(16);

            std::string pdffilename = "/upload/";
            pdffilename.append(randomFilename);
            pdffilename.append(".pdf");

            std::string pdfOutputfile = OUTPUT_DIRECTORY;
            pdfOutputfile.append(pdffilename);

            vilslog::printf("WebUDPParserService::SendCoursePDFReportToClient() pdfOutputfile[%s]\n", pdfOutputfile.c_str());

            std::string report_date = TodayDateType1();
            std::string report_start_date = date;
            std::string report_end_date = date;

            PDFCreater * pdfCreater = new PDFCreater();
            pdfCreater->create_report_header(pdfOutputfile, "課程紀錄", report_date, report_start_date, report_end_date);
            pdfCreater->create_report_table(&pdfDataCourseInfo);
            pdfCreater->create_report_table(&pdfDataCourseRecord);
            pdfCreater->generate_report();

            delete pdfCreater;

            // put to remove queue
            QRCodeStruct * newQRCcode = new QRCodeStruct();
            newQRCcode->filepath = pdfOutputfile;
            newQRCcode->createtime = getCurrentTimeInSec();
            mQRCodeImageList.push_back(newQRCcode);

            sendstream<<"{\"pdfreport\":\"";
            sendstream<<pdffilename;
            sendstream<<"\",\"success\":\"true\"}";

            clear_CourseDayTrackRecordStruct_list(infoList);
            delete infoList;

            clear_PDFTableDataStruct(&pdfDataCourseInfo);
            clear_PDFTableDataStruct(&pdfDataCourseRecord);
        }
        else
        {
            sendstream<<"{\"pdfreport\":\"\",\"success\":\"false\"}";
        }
    }
    else
    {
        sendstream<<"{\"pdfreport\":\"\",\"success\":\"false\"}";
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);

}

void WebUDPParserService::createCourse(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    //printf("WebUDPParserService::createCourse() parsingSuccessful[%d]\n",parsingSuccessful);

    if ( parsingSuccessful )
    {
        CourseStruct * course = new CourseStruct();
        course->dbId = -1;
        course->projectid = 1;
        course->allDay = 0;
        course->resourceId = "";
        course->courseId = "";
        course->title = "";
        course->description = "";
        course->start = "";
        course->end ="";
        course->startTime = "";
        course->endTime = "";
        course->daysOfWeek = new std::list<int>();
        course->startRecur = "";
        course->endRecur = "";
        course->backgroundColor = "";
        course->borderColor = "";
        course->maingroupid = 1;

        if ( result.isMember("project_id") )
            course->projectid = getProjectId(result);
        if ( result.isMember("allDay") )
            course->allDay = result["allDay"].asInt();
        if ( result.isMember("resourceId") )
            course->resourceId = result["resourceId"].asString();
        if ( result.isMember("courseId") )
            course->courseId = result["courseId"].asString();
        if ( result.isMember("title") )
            course->title = result["title"].asString();
        if ( result.isMember("description") )
            course->description = result["description"].asString();
        if ( result.isMember("start") )
            course->start = result["start"].asString();
        if ( result.isMember("end") )
            course->end = result["end"].asString();
        if ( result.isMember("startTime") )
            course->startTime = result["startTime"].asString();
        if ( result.isMember("endTime") )
            course->endTime = result["endTime"].asString();
        if ( result.isMember("startRecur") )
            course->startRecur = result["startRecur"].asString();
        if ( result.isMember("endRecur") )
            course->endRecur = result["endRecur"].asString();
        if ( result.isMember("backgroundColor") )
            course->backgroundColor = result["backgroundColor"].asString();
        if ( result.isMember("borderColor") )
            course->borderColor = result["borderColor"].asString();
        if ( result.isMember("maingroupid") )
            course->maingroupid = convert_string_to_int(result["maingroupid"].asString());
        // std::string weekListStr = res->getString("daysOfWeek");

        WorkItem * item = new WorkItem("createcourse", course->projectid, course);
        mMySQLJobQueue.add(item);

        DataItem * senditem = new DataItem("updatecourse", course->projectid);
        mMainJobQueue.add(senditem);

        delete course->daysOfWeek;
        delete course;

    }
}

void WebUDPParserService::deleteCourse(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        CourseStruct * course = new CourseStruct();
        course->dbId = -1;
        course->projectid = 1;
        course->allDay = 0;
        course->resourceId = "";
        course->courseId = "";
        course->title = "";
        course->description = "";
        course->start = "";
        course->end ="";
        course->startTime = "";
        course->endTime = "";
        course->daysOfWeek = new std::list<int>();
        course->startRecur = "";
        course->endRecur = "";
        course->backgroundColor = "";
        course->borderColor = "";
        course->maingroupid = 1;

        if ( result.isMember("project_id") )
            course->projectid = getProjectId(result);
        if ( result.isMember("courseId") )
            course->courseId = result["courseId"].asString();

        WorkItem * item = new WorkItem("deletecourse", course->projectid, course);
        mMySQLJobQueue.add(item);

        DataItem * senditem = new DataItem("updatecourse", course->projectid);
        mMainJobQueue.add(senditem);

        delete course->daysOfWeek;
        delete course;
    }
}

void WebUDPParserService::sendPOSNodeToClient(UDPDataItem * item)
{
    std::string bufString = item->getMsg();
    std::string::size_type pos = bufString.find('=');
    int projectid = 1;
    if (pos != std::string::npos)
    {
        std::string prjstr = bufString.substr(pos + 1);
        projectid = atoi(prjstr.c_str());
        vilslog::printf("WebUDPParserService::sendPOSNodeToClient() projectid[%d]\n", projectid);
    }
    std::list<NodeItemStruct *> * plist = new std::list<NodeItemStruct *>();

    std::ostringstream sendstream;
    sendstream<<"[";

    int count = mDeviceManager->get_POSNode_list(plist, projectid);
    if (count > 0)
    {
        for (std::list<NodeItemStruct*>::iterator item = plist->begin(); item != plist->end(); item++)
        {
            if(item != plist->begin())
                sendstream<<",";
            sendstream<<"{";
            sendstream<<"\"nodeid\":\""<<(*item)->nodeid<<"\",";
            sendstream<<"\"nodename\":\""<<(*item)->nodename<<"\"";
            sendstream<<"}";
        }
    }
    sendstream<<"]\r\n";
    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);


    clear_NodeItemStruct_list(plist);
    delete plist;
}

// void WebUDPParserService::sendPOSNodeDataToClient(UDPDataItem * item)
// {
//     std::string bufString = item->getMsg();
//     std::string::size_type pos = bufString.find('=');
//
//     std::string nodeid = "";
//     if (pos != std::string::npos)
//     {
//         nodeid = bufString.substr(pos + 1);
//         vilslog::printf("WebUDPParserService::sendPOSNodeDataToClient() nodeid[%s]\n", nodeid.c_str());
//     }
//
//     std::ostringstream sendstream;
//     sendstream<<"[";
//
//     int projectid = item->getProjectid();
//     std::list<PositionInfoStruct *> * plist = mDeviceManager->get_POSNodeData_list(projectid, nodeid);
//     if (plist != NULL)
//     {
//         for (std::list<PositionInfoStruct*>::iterator item = plist->begin(); item != plist->end(); item++)
//         {
//             if(item != plist->begin())
//                 sendstream<<",";
//             sendstream<<"{";
//             sendstream<<"\"x\":\""<<(*item)->posX<<"\",";
//             sendstream<<"\"y\":\""<<(*item)->posY<<"\",";
//             sendstream<<"\"z\":\""<<(*item)->posZ<<"\",";
//             sendstream<<"\"time\":\""<<time_tToString((*item)->updateTimestamp)<<"\"";
//             sendstream<<"}";
//         }
//     }
//
//     sendstream<<"]\r\n";
//     std::string sendString = sendstream.str();
//
//     const char * sendBuf = sendString.c_str();
//     item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
// }

void WebUDPParserService::sendGPSNodeToClient(UDPDataItem * item)
{
    std::string bufString = item->getMsg();
    std::string::size_type pos = bufString.find('=');
    int projectid = 1;
    if (pos != std::string::npos)
    {
        std::string prjstr = bufString.substr(pos + 1);
        projectid = atoi(prjstr.c_str());
        vilslog::printf("WebUDPParserService::sendGPSNodeToClient() projectid[%d]\n", projectid);
    }
    std::list<NodeItemStruct *> * plist = new std::list<NodeItemStruct *>();

    std::ostringstream sendstream;
    sendstream<<"[";

    int count = mDeviceManager->get_GPSNode_list(plist, projectid);
    if (count > 0)
    {
        for (std::list<NodeItemStruct*>::iterator item = plist->begin(); item != plist->end(); item++)
        {
            if(item != plist->begin())
                sendstream<<",";
            sendstream<<"{";
            sendstream<<"\"nodeid\":\""<<(*item)->nodeid<<"\",";
            sendstream<<"\"nodename\":\""<<(*item)->nodename<<"\"";
            sendstream<<"}";
        }
    }
    sendstream<<"]\r\n";
    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);


    clear_NodeItemStruct_list(plist);
    delete plist;
}

// void WebUDPParserService::sendGPSNodeDataToClient(UDPDataItem * item)
// {
//     std::string bufString = item->getMsg();
//     std::string::size_type pos = bufString.find('=');
//     std::string nodeid = "";
//     if (pos != std::string::npos)
//     {
//         nodeid = bufString.substr(pos + 1);
//         vilslog::printf("WebUDPParserService::sendGPSNodeDataToClient() nodeid[%s]\n", nodeid.c_str());
//     }
//
//     std::ostringstream sendstream;
//     sendstream.precision(6);
//
//     sendstream<<"[";
//
//     int projectid = 1;
//     std::list<GPSInfoStruct *> * plist = mDeviceManager->get_GPSNodeData_list(projectid, nodeid);
//     if (plist != NULL)
//     {
//         for (std::list<GPSInfoStruct*>::const_reverse_iterator item = plist->rbegin(); item != plist->rend(); item++)
//         {
//             if(item != plist->rbegin())
//                 sendstream<<",";
//             sendstream<<"{";
//             sendstream<<"\"Lng\":\""<<fixed<<(*item)->Longitude<<"\",";
//             sendstream<<"\"Lat\":\""<<fixed<<(*item)->Latitude<<"\",";
//             sendstream<<"\"time\":\""<<time_tToString((*item)->updateTimestamp)<<"\"";
//             sendstream<<"}";
//         }
//     }
//
//     sendstream<<"]\r\n";
//     std::string sendString = sendstream.str();
//
//     const char * sendBuf = sendString.c_str();
//     item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
// }

void WebUDPParserService::sendDeviceAliveToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    int projectid = 1;
    if ( parsingSuccessful )
    {
        projectid = getProjectId(result);
    }

    std::ostringstream sendstream;
    sendstream<<"[";

    std::list<NodeStruct*> * anchorNodeList = mDeviceManager->get_Anchor_list(projectid);
    std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);
    std::list<NodeStruct*> * coordNodeList = mDeviceManager->get_Coordinator_list(projectid);
    std::list<NodeStruct*> * locatorNodeList = mDeviceManager->get_Locator_list(projectid);
    std::list<LoRaGatewayInfoStruct*> * gatewayList = mDeviceManager->get_LoRaGateway_list(projectid);

    int anchor_total_count = 0;
    int tag_total_count = 0;
    int coord_total_count = 0;
    int locator_total_count = 0;
    int gateway_total_count = 0;

    if (anchorNodeList != NULL)
        anchor_total_count = (int)anchorNodeList->size();
    if (tagNodeList != NULL)
        tag_total_count = (int)tagNodeList->size();
    if (coordNodeList != NULL)
        coord_total_count = (int)coordNodeList->size();
    if (locatorNodeList != NULL)
        locator_total_count = (int)locatorNodeList->size();
    if (gatewayList != NULL)
        gateway_total_count = (int)gatewayList->size();

    int anchor_alive_count = 0;
    int tag_alive_count = 0;
    int coord_alive_count = 0;
    int locator_alive_count = 0;
    int gateway_alive_count =0;

    if (anchorNodeList != NULL)
        anchor_alive_count = count_node_alive(anchorNodeList);
    if (tagNodeList != NULL)
        tag_alive_count = count_node_alive(tagNodeList);
    if (coordNodeList != NULL)
        coord_alive_count = count_node_alive(coordNodeList);
    if (locatorNodeList != NULL)
        locator_alive_count = count_node_alive(locatorNodeList);
    if (gatewayList != NULL)
        gateway_alive_count = count_gateway_alive(gatewayList);

    sendstream<<"{\"type\":"<<NODE_TYPE_ANCHOR<<",\"total\":"<<anchor_total_count<<",\"alive\":"<<anchor_alive_count<<"}";
    sendstream<<",";
    sendstream<<"{\"type\":"<<NODE_TYPE_TAG<<",\"total\":"<<tag_total_count<<",\"alive\":"<<tag_alive_count<<"}";
    sendstream<<",";
    sendstream<<"{\"type\":"<<NODE_TYPE_COORD<<",\"total\":"<<coord_total_count<<",\"alive\":"<<coord_alive_count<<"}";
    sendstream<<",";
    sendstream<<"{\"type\":"<<NODE_TYPE_LOCATOR<<",\"total\":"<<locator_total_count<<",\"alive\":"<<locator_alive_count<<"}";
    sendstream<<",";
    sendstream<<"{\"type\":"<<NODE_TYPE_GATEWAY<<",\"total\":"<<gateway_total_count<<",\"alive\":"<<gateway_alive_count<<"}";

    sendstream<<"]\r\n";
    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendSystemServiceToClient(UDPDataItem * item)
{
    std::ostringstream sendstream;

    int service_total_count = (int)mServiceStatusList.size();
    int service_alive_count = 0;

    for (std::list<StatusStruct*>::iterator existed = mServiceStatusList.begin(); existed != mServiceStatusList.end(); existed++)
    {
        if ((*existed)->status)
        {
            service_alive_count++;
        }
    }

    sendstream<<"{\"total\":"<<service_total_count<<",\"alive\":"<<service_alive_count<<"}";
    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendQRCodeToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();

    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    std::ostringstream sendstream;

    if ( parsingSuccessful )
    {
        std::string type = "";
        std::string id = "";
        std::string accountid = "";
        std::string maingroupid = "1";
        int projectid = getProjectId(result);

        if ( result.isMember("type") )
        {
            type = result["type"].asString();
        }
        if ( result.isMember("id") )
        {
            if ( result["id"].isInt() )
            {
                id = to_string(result["id"].asInt());
            }
            else
            {
                id = result["id"].asString();
            }
        }
        if ( result.isMember("accountid") )
        {
            accountid = result["accountid"].asString();
        }
        if ( result.isMember("maingroupid") )
        {
            if ( result["maingroupid"].isInt() )
            {
                maingroupid =to_string(result["maingroupid"].asInt());
            }
            else
            {
                maingroupid = result["maingroupid"].asString();
            }
        }

        vilslog::printf("WebUDPParserService::sendQRCodeToClient() projectid[%d] type[%s] id[%s] accountid[%s] maingroupid[%s]\n",
            projectid, type.c_str(), id.c_str(), accountid.c_str(), maingroupid.c_str());

        if (type.length() == 0 || id.length() == 0)
        {
            sendstream<<"{\"qrcode\":\"\",\"success\":\"false\"}";
        }
        else
        {

            //
            // check if use type tag
            //
            if ( type.compare("TAG") == 0 )
            {
                NodeStruct * tagnode = mDeviceManager->get_Tag_node_ByMacaddress(projectid, id.c_str());
                if ( tagnode->tafid == 4 )
                {
                    // 0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師
                    type = "USER";
                }
            }

            VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);

            std::string qrcodeText = vilssysConfig->PushNotifyPrefix;
            qrcodeText.append("_");
            qrcodeText.append(type);
            qrcodeText.append("_");
            if ( type.compare("USER") == 0 )
            {
                int userid = convert_string_to_int(id);
                qrcodeText.append(to_string(userid));
            }
            else
            {
                qrcodeText.append(id);
            }

            std::string randomFilename = random_string(16);

            std::string qrcodefilename = "/upload/";
            qrcodefilename.append(randomFilename);
            qrcodefilename.append(".bmp");

            std::string qrcodeOutputfile = OUTPUT_DIRECTORY;
            qrcodeOutputfile.append(qrcodefilename);

            TargetTopicStruct * target = NULL;
            MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
            if ( baseDb != NULL )
            {
                target = baseDb->get_TargetTopic(qrcodeText, type, id, accountid, maingroupid);
            }

            vilslog::printf("WebUDPParserService::sendQRCodeToClient() qrcodeText[%s]\n", qrcodeText.c_str());

            //
            // generate QR code bitmap file
            //
            create_qrcode(qrcodeText.c_str(), qrcodeOutputfile.c_str(), 10);

            // put to queue
            QRCodeStruct * newQRCcode = new QRCodeStruct();
            newQRCcode->filepath = qrcodeOutputfile;
            newQRCcode->createtime = getCurrentTimeInSec();
            mQRCodeImageList.push_back(newQRCcode);

            //sendstream<<"{\"qrcode\":\"USER:A001\",\"success\":\"true\"}";
            sendstream<<"{\"qrcode\":\""<<qrcodefilename<<"\"";
            sendstream<<",\"target\":\""<<target->target<<"\"";
            sendstream<<",\"topic\":\""<<target->topic<<"\"";
            sendstream<<",\"name\":\""<<target->name<<"\"";
            sendstream<<",\"datetime\":\""<<target->datetime<<"\"";
            sendstream<<",\"success\":\"true\"}";

            if (target != NULL)
            {
                delete target;
            }

        }
    }
    else
    {
        sendstream<<"{\"qrcode\":\"\",\"success\":\"false\"}";
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendPDFReportToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();

    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    std::ostringstream sendstream;

    if ( parsingSuccessful )
    {
        std::string type = "";
        std::string id = "";
        int projectid = getProjectId(result);

        if ( result.isMember("type") )
        {
            type = result["type"].asString();
        }
        if ( result.isMember("id") )
        {
            id = result["id"].asString();
        }

        vilslog::printf("WebUDPParserService::sendPDFReportToClient() projectid[%d] type[%s] id[%s]\n", projectid, type.c_str(), id.c_str());

        if (type.length() == 0 || id.length() == 0)
        {
            sendstream<<"{\"pdfreport\":\"\",\"success\":\"false\"}";
        }
        else
        {
            std::string randomFilename = random_string(16);

            std::string pdffilename = "/upload/";
            pdffilename.append(randomFilename);
            pdffilename.append(".pdf");

            std::string pdfOutputfile = OUTPUT_DIRECTORY;
            pdfOutputfile.append(pdffilename);

            vilslog::printf("WebUDPParserService::sendPDFReportToClient() pdfOutputfile[%s] id[%s]\n", pdfOutputfile.c_str(), id.c_str());

            //UserEmployeeStruct * employee = mDeviceManager->getEmployee(id.c_str());
            int userid = convert_string_to_int(id);

            UserStruct * user = mDeviceManager->getUser(projectid, userid);
            if (user != NULL)
            {
                std::string report_date = TodayDateType1();
                std::string report_start_date = TodayDateType1(30);
                std::string report_end_date = TodayDateType1();
                std::string report_start_datetime = currentDateTimeForSQL(30);
                std::string report_end_datetime = currentDateTimeForSQL();
                std::list<EmployeeEventReportStruct*> * reportlist = mDeviceManager->getEmployeeEventReport(projectid, id, report_start_datetime, report_end_datetime);

                // EmployeeEventReportStruct * event = new EmployeeEventReportStruct();
                // event->emp_empid = "A001";
                // event->emp_cname = "蔡孟偉";
                // event->emp_orgName = "營運管理部";
                // event->emp_jobTitle = "經理";
                // event->comment = "風雨過大, 導致窗戶滲水, 馬上清理並通知工程人員處理!";
                // event->action = "Cancel";
                // event->nodeid = "Locator0001";
                // event->locatorid = "Locator0001";
                // event->resident_serno = "13";
                // event->resident_name = "郭曹寶玉";
                // event->resident_roomNo = "0510(A11-12)";
                // event->eventtime = "2019/08/23 10:32:56";
                // event->reponsetime = "10:33:06";
                // eventlist->push_back(event);
                //
                // generate pdf report file
                //
                if (reportlist != NULL)
                {
                    PDFCreater * pdfCreater = new PDFCreater();
                    pdfCreater->create_event_report(pdfOutputfile, report_date, report_start_date, report_end_date, user, reportlist);
                    delete pdfCreater;

                    // put to remove queue
                    QRCodeStruct * newQRCcode = new QRCodeStruct();
                    newQRCcode->filepath = pdfOutputfile;
                    newQRCcode->createtime = getCurrentTimeInSec();
                    mQRCodeImageList.push_back(newQRCcode);

                    clear_EmployeeEventReportStruct_list(reportlist);
                    delete reportlist;

                    sendstream<<"{\"pdfreport\":\"";
                    sendstream<<pdffilename;
                    sendstream<<"\",\"success\":\"true\"}";
                }
                else
                {
                    sendstream<<"{\"pdfreport\":\"\",\"success\":\"false\"}";
                }

                // clear_UserStruct(user);
                // delete user;
            }
            else
            {
                sendstream<<"{\"pdfreport\":\"\",\"success\":\"false\"}";
            }
        }
    }
    else
    {
        sendstream<<"{\"pdfreport\":\"\",\"success\":\"false\"}";
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendTagAreaInOutPDFReportToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();

    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    std::ostringstream sendstream;

    if ( parsingSuccessful )
    {
        std::string date = "";
        std::string startdate = "";
        std::string enddate = "";
        std::string id = "";
        int projectid = getProjectId(result);

        if ( result.isMember("date") )
        {
            date = result["date"].asString();
        }
        if ( result.isMember("startdate") )
        {
            startdate = result["startdate"].asString();
        }
        if ( result.isMember("enddate") )
        {
            enddate = result["enddate"].asString();
        }
        if ( result.isMember("id") )
        {
            id = result["id"].asString();
        }

        vilslog::printf("WebUDPParserService::sendTagAreaInOutPDFReportToClient() projectid[%d] startdate[%s] enddate[%s] date[%s] id[%s]\n",
            projectid, startdate.c_str(), enddate.c_str(), date.c_str(), id.c_str());

        if (id.length() == 0)
        {
            sendstream<<"{\"pdfreport\":\"\",\"success\":\"false\"}";
        }
        else
        {
            //std::string randomFilename = random_string(16);

            std::string pdffilename = "/upload/";
            //pdffilename.append(randomFilename);
            pdffilename.append("進出區域報告");
            pdffilename.append("_");
            pdffilename.append(currentDateTimeForReport());
            pdffilename.append(".pdf");

            std::string pdfOutputfile = OUTPUT_DIRECTORY;
            pdfOutputfile.append(pdffilename);

            vilslog::printf("WebUDPParserService::sendTagAreaInOutPDFReportToClient() pdfOutputfile[%s]\n", pdfOutputfile.c_str());

            std::string report_date = TodayDateType1();
            std::string report_start_date = date;
            std::string report_end_date = date;
            std::string report_start_datetime = time_tToString(StringDateTotime_t(date, 0));
            std::string report_end_datetime = time_tToString(StringDateTotime_t(date, 1));

            if ( startdate.length() > 0 )
            {
                report_start_date = startdate;
                report_start_datetime = time_tToString(StringDateTotime_t(startdate, 0));
            }
            if ( enddate.length() > 0 )
            {
                report_end_date = enddate;
                report_end_datetime = time_tToString(StringDateTotime_t(enddate, 1));
            }

            vilslog::printf("WebUDPParserService::sendTagAreaInOutPDFReportToClient() report_start_date[%s] report_end_date[%s]\n",
                report_start_date.c_str(), report_end_date.c_str());
            vilslog::printf("WebUDPParserService::sendTagAreaInOutPDFReportToClient() report_start_datetime[%s] report_end_datetime[%s]\n",
                report_start_datetime.c_str(), report_end_datetime.c_str());

            std::list<AreaInOutEventStruct*> * nodeAreaInOutList = NULL;
            MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
            if (baseDb == NULL)
            {
                nodeAreaInOutList = new std::list<AreaInOutEventStruct*>();
            }
            else
            {
                nodeAreaInOutList = baseDb->getNodeAreaInOutList(id.c_str(), report_start_datetime.c_str(), report_end_datetime.c_str(), "ASC");
            }

            PDFTableDataStruct * pdfDataAreaInOut = prepareTagDailyAreaInOutReport(projectid, nodeAreaInOutList, id, report_start_datetime, report_end_datetime);

            std::string report_type = "人員";
            NodeStruct * node = mDeviceManager->get_Tag_node(projectid, id.c_str());
            if ( node == NULL )
            {
                report_type = "裝置";
                node = mDeviceManager->get_Locator_node_byid(projectid, id.c_str());
            }

            PDFCreater * pdfCreater = new PDFCreater();
            pdfCreater->create_report_header(pdfOutputfile, "進出區域紀錄", report_date, report_start_date, report_end_date);
            if ( node != NULL )
                pdfCreater->create_report_userinfo(report_type, node->nodename);
            pdfCreater->create_report_table(pdfDataAreaInOut);
            pdfCreater->generate_report();
            delete pdfCreater;

            // put to remove queue
            QRCodeStruct * newQRCcode = new QRCodeStruct();
            newQRCcode->filepath = pdfOutputfile;
            newQRCcode->createtime = getCurrentTimeInSec();
            mQRCodeImageList.push_back(newQRCcode);

            sendstream<<"{\"pdfreport\":\"";
            sendstream<<pdffilename;
            sendstream<<"\",\"success\":\"true\"}";

            clear_PDFTableDataStruct(pdfDataAreaInOut);

            delete pdfDataAreaInOut;

            clear_AreaInOutEventStruct_list(nodeAreaInOutList);
            delete nodeAreaInOutList;
        }
    }
    else
    {
        sendstream<<"{\"pdfreport\":\"\",\"success\":\"false\"}";
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendTagAreaInOutXLSXReportToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();

    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    std::ostringstream sendstream;

    if ( parsingSuccessful )
    {
        std::string date = "";
        std::string startdate = "";
        std::string enddate = "";
        std::string id = "";
        int projectid = getProjectId(result);

        if ( result.isMember("date") )
        {
            date = result["date"].asString();
        }
        if ( result.isMember("startdate") )
        {
            startdate = result["startdate"].asString();
        }
        if ( result.isMember("enddate") )
        {
            enddate = result["enddate"].asString();
        }
        if ( result.isMember("id") )
        {
            id = result["id"].asString();
        }

        // vilslog::printf("WebUDPParserService::sendTagAreaInOutXLSXReportToClient() projectid[%d] startdate[%s] enddate[%s] date[%s] id[%s]\n",
        //     projectid, startdate.c_str(), enddate.c_str(), date.c_str(), id.c_str());

        if (id.length() == 0)
        {
            sendstream<<"{\"xlsxreport\":\"\",\"success\":\"false\"}";
        }
        else
        {
            std::string report_date = TodayDateType1();
            std::string report_start_date = date;
            std::string report_end_date = date;
            std::string report_start_datetime = time_tToString(StringDateTotime_t(date, 0));
            std::string report_end_datetime = time_tToString(StringDateTotime_t(date, 1));

            if ( startdate.length() > 0 )
            {
                report_start_date = startdate;
                report_start_datetime = time_tToString(StringDateTotime_t(startdate, 0));
            }
            if ( enddate.length() > 0 )
            {
                report_end_date = enddate;
                report_end_datetime = time_tToString(StringDateTotime_t(enddate, 1));
            }

            std::ostringstream stream_report_date_range;
            stream_report_date_range<<report_start_date<<" 至 "<<report_end_date;

            std::list<AreaInOutEventStruct*> * nodeAreaInOutList = NULL;
            MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
            if (baseDb == NULL)
            {
                nodeAreaInOutList = new std::list<AreaInOutEventStruct*>();
            }
            else
            {
                nodeAreaInOutList = baseDb->getNodeAreaInOutList(id.c_str(), report_start_datetime.c_str(), report_end_datetime.c_str(), "ASC");
            }

            PDFTableDataStruct * pdfDataAreaInOut = prepareTagDailyAreaInOutReport(projectid, nodeAreaInOutList, id, report_start_datetime, report_end_datetime);

            std::string report_type = "人員";
            NodeStruct * node = mDeviceManager->get_Tag_node(projectid, id.c_str());
            if ( node == NULL )
            {
                report_type = "裝置";
                node = mDeviceManager->get_Locator_node_byid(projectid, id.c_str());
            }

            std::string xlsxfilename = "/upload/";
            xlsxfilename.append("進出區域報告");
            xlsxfilename.append("_");
            xlsxfilename.append(currentDateTimeForReport());
            xlsxfilename.append(".xlsx");

            std::string xlsxOutputfile = OUTPUT_DIRECTORY;
            xlsxOutputfile.append(xlsxfilename);

            // vilslog::printf("WebUDPParserService::sendTagRollCallXlsxReportToClient() xlsxOutputfile[%s]\n", xlsxOutputfile.c_str());

            OpenXLSX::XLDocument xlsxReport;
            xlsxReport.CreateDocument(xlsxOutputfile);
            auto wks1 = xlsxReport.Workbook().Worksheet("Sheet1");

            wks1.Cell("A1").Value() = "進出區域紀錄";
            wks1.Cell("A2").Value() = "報告產生日期";
            wks1.Cell("B2").Value() = report_date;
            wks1.Cell("A3").Value() = "報告資料區間";
            wks1.Cell("B3").Value() = stream_report_date_range.str();
            wks1.Cell("A4").Value() = report_type;
            wks1.Cell("B4").Value() = node->nodename;

            wks1.Cell("A6").Value() = "進入時間";
            wks1.Cell("B6").Value() = "離開時間";
            wks1.Cell("C6").Value() = "到訪區域";
            wks1.Cell("D6").Value() = "區域類別";
            wks1.Cell("E6").Value() = "停留時間";

            int col_count = 0;
            int row_count = 7;

            for (std::list<PDFTableRowDataStruct *>::iterator row = pdfDataAreaInOut->rowdatalist.begin(); row != pdfDataAreaInOut->rowdatalist.end(); row++)
            {
                PDFTableRowDataStruct * rowdata = (*row);

                col_count = 0;
                for (std::list<PDFTableHeaderDataStruct *>::iterator header = pdfDataAreaInOut->headerlist.begin(); header != pdfDataAreaInOut->headerlist.end(); header++)
                {
                    for (std::list<PDFTableColDataStruct *>::iterator col = rowdata->coldatalist.begin(); col != rowdata->coldatalist.end(); col++)
                    {
                        PDFTableColDataStruct * colata = (*col);

                        if ( colata->header.compare((*header)->header) == 0 )
                        {
                            std::string colName = "A";
                            if ( col_count == 0 )
                                colName = "A";
                            else
                            if ( col_count == 1 )
                                colName = "B";
                            else
                            if ( col_count == 2 )
                                colName = "C";
                            else
                            if ( col_count == 3 )
                                colName = "D";
                            else
                            if ( col_count == 4 )
                                colName = "E";

                            colName.append(std::to_string(row_count));
                            // vilslog::printf("sendTagRollCallXlsxReportToClient() header[%s] colName[%s] data[%s]\n", colata->header.c_str(), colName.c_str(), colata->data.c_str());
                            wks1.Cell(colName).Value() = colata->data;

                            col_count++;
                        }
                    }
                }
                row_count++;
            }

            xlsxReport.SaveDocument();
            xlsxReport.CloseDocument();

            // put to remove queue
            QRCodeStruct * newQRCcode = new QRCodeStruct();
            newQRCcode->filepath = xlsxOutputfile;
            newQRCcode->createtime = getCurrentTimeInSec();
            mQRCodeImageList.push_back(newQRCcode);

            sendstream<<"{\"xlsxreport\":\"";
            sendstream<<xlsxfilename;
            sendstream<<"\",\"success\":\"true\"}";


            clear_PDFTableDataStruct(pdfDataAreaInOut);
            delete pdfDataAreaInOut;

            clear_AreaInOutEventStruct_list(nodeAreaInOutList);
            delete nodeAreaInOutList;
        }
    }
    else
    {
        sendstream<<"{\"xlsxreport\":\"\",\"success\":\"false\"}";
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendEventPDFReportToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();

    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    std::ostringstream sendstream;

    if ( parsingSuccessful )
    {
        std::string date = "";
        std::string id = "";
        int projectid = getProjectId(result);

        if ( result.isMember("date") )
        {
            date = result["date"].asString();
        }
        if ( result.isMember("id") )
        {
            id = result["id"].asString();
        }

        vilslog::printf("WebUDPParserService::sendEventPDFReportToClient() projectid[%d] date[%s] id[%s]\n", projectid, date.c_str(), id.c_str());

        if (date.length() == 0 || id.length() == 0)
        {
            sendstream<<"{\"pdfreport\":\"\",\"success\":\"false\"}";
        }
        else
        {
            MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
            if (baseDb != NULL)
            {
                std::string randomFilename = random_string(16);

                std::string pdffilename = "/upload/";
                pdffilename.append(randomFilename);
                pdffilename.append(".pdf");

                std::string pdfOutputfile = OUTPUT_DIRECTORY;
                pdfOutputfile.append(pdffilename);

                vilslog::printf("WebUDPParserService::sendEventPDFReportToClient() pdfOutputfile[%s]\n", pdfOutputfile.c_str());

                std::string report_date = TodayDateType1();
                std::string report_start_date = date;
                std::string report_end_date = date;
                std::string report_start_datetime = time_tToString(StringDateTotime_t(date, 0));
                std::string report_end_datetime = time_tToString(StringDateTotime_t(date, 1));

                std::list<FiredEventStruct*> * firedEventList = baseDb->read_fired_events_DayRange(report_start_datetime.c_str(), report_end_datetime.c_str(), "ASC");
                std::list<FiredEventStruct*> * nodeFiredEventsList = new std::list<FiredEventStruct*>();

                vilslog::printf ("WebUDPParserService::sendEventPDFReportToClient() nodeid[%s] firedEventList->size()[%d]\n", id.c_str(), (int)firedEventList->size());

                for (std::list<FiredEventStruct *>::iterator event = firedEventList->begin(); event != firedEventList->end(); event++)
                {
                    Json::Value JSONresult;

                    Json::Reader * reader = new Json::Reader();
                    bool parsingSuccessful = reader->parse( (*event)->eventjson.c_str(), JSONresult);
                    delete reader;

                    if (!parsingSuccessful)
                    {
                        continue;
                    }

                    bool bFound = false;

                    if ( (*event)->eventid == 9999 )
                    {
                        if( JSONresult["nodeid"].asString().compare(id) == 0)
                        {
                            bFound = true;
                        }
                    }
                    else
                    if ( (*event)->eventid == 999 )
                    {
                        if( JSONresult["type"].asString().compare("inspectionpoint") == 0 ||
                            JSONresult["type"].asString().compare("inspectionpointtravel") == 0)
                        {
                            if( JSONresult["Inspector"].asString().compare(id) == 0)
                            {
                                bFound = true;
                            }
                        }
                    }
                    else
                    if ( JSONresult["nodeid"].asString().compare(id) == 0 )
                    {
                        bFound = true;
                    }
                    else
                    if ( JSONresult["locatorid"].asString().compare(id) == 0 )
                    {
                        bFound = true;
                    }

                    if(bFound)
                    {
                        FiredEventStruct * newEvent = copyFiredEventStruct((*event));
                        nodeFiredEventsList->push_back(newEvent);
                    }
                }

                vilslog::printf ("WebUDPParserService::sendEventPDFReportToClient() nodeid[%s] nodeFiredEventsList->size()[%d]\n", id.c_str(), (int)nodeFiredEventsList->size());

                //
                // generate pdf report file
                //
                NodeStruct * node = mDeviceManager->get_Tag_node(projectid, id.c_str());
                if ( node == NULL )
                {
                    node = mDeviceManager->get_Locator_node_byid(projectid, id.c_str());
                }

                PDFCreater * pdfCreater = new PDFCreater();
                if ( node != NULL )
                    pdfCreater->create_firedevent_report(pdfOutputfile, report_date, report_start_date, report_end_date, node, nodeFiredEventsList);
                delete pdfCreater;

                // put to remove queue
                QRCodeStruct * newQRCcode = new QRCodeStruct();
                newQRCcode->filepath = pdfOutputfile;
                newQRCcode->createtime = getCurrentTimeInSec();
                mQRCodeImageList.push_back(newQRCcode);

                clear_FiredEventStruct_list(firedEventList);
                delete firedEventList;
                clear_FiredEventStruct_list(nodeFiredEventsList);
                delete nodeFiredEventsList;

                sendstream<<"{\"pdfreport\":\"";
                sendstream<<pdffilename;
                sendstream<<"\",\"success\":\"true\"}";
            }
        }
    }
    else
    {
        sendstream<<"{\"pdfreport\":\"\",\"success\":\"false\"}";
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendTagDailyPDFReportToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();

    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    std::ostringstream sendstream;

    if ( parsingSuccessful )
    {
        std::string date = "";
        std::string id = "";
        int projectid = getProjectId(result);

        if ( result.isMember("date") )
        {
            date = result["date"].asString();
        }
        if ( result.isMember("id") )
        {
            id = result["id"].asString();
        }

        vilslog::printf("WebUDPParserService::sendTagDailyPDFReportToClient() projectid[%d] date[%s] id[%s]\n", projectid, date.c_str(), id.c_str());

        if (date.length() == 0 || id.length() == 0)
        {
            sendstream<<"{\"pdfreport\":\"\",\"success\":\"false\"}";
        }
        else
        {

            std::string report_start_datetime = time_tToString(StringDateTotime_t(date, 0));
            std::string report_end_datetime = time_tToString(StringDateTotime_t(date, 1));

            std::list<FiredEventStruct*> * firedEventList = NULL;
            std::list<AreaInOutEventStruct*> * nodeAreaInOutList1 = NULL;
            std::list<AreaInOutEventStruct*> * nodeAreaInOutList2 = NULL;
            MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
            if (baseDb == NULL)
            {
                firedEventList = new std::list<FiredEventStruct*>();
                nodeAreaInOutList1 = new std::list<AreaInOutEventStruct*>();
                nodeAreaInOutList2 = new std::list<AreaInOutEventStruct*>();
            }
            else
            {
                firedEventList = baseDb->read_fired_events_DayRange(report_start_datetime.c_str(), report_end_datetime.c_str(), "ASC");
                nodeAreaInOutList1 = baseDb->getNodeAreaInOutList(id.c_str(), report_start_datetime.c_str(), report_end_datetime.c_str(), "ASC");
                nodeAreaInOutList2 = new std::list<AreaInOutEventStruct*>();
                for (std::list<AreaInOutEventStruct*>::iterator event = nodeAreaInOutList1->begin(); event != nodeAreaInOutList1->end(); event++)
                {
                    AreaInOutEventStruct * dstEvent = copyAreaInOutEventStruct((*event));
                    nodeAreaInOutList2->push_back(dstEvent);
                }
            }

            std::string report_date = TodayDateType1();
            std::string report_start_date = date;
            std::string report_end_date = date;

            std::string AreaID = "";
            std::string ShowName = id;
            std::string report_type = "人員";
            NodeStruct * node = mDeviceManager->get_Tag_node(projectid, id.c_str());
            if ( node == NULL )
            {
                report_type = "裝置";
                node = mDeviceManager->get_Locator_node_byid(projectid, id.c_str());
            }

            if ( node == NULL )
            {
                report_type = "感測器";
                SensorDeviceStruct * sensor = mDeviceManager->get_SensorDevice(projectid, id);
                if (sensor != NULL)
                    ShowName = sensor->name;
            }
            else
            {
                ShowName = node->nodename;
                AreaID = getUseRoommAreaID(projectid, node);
            }
            vilslog::printf("WebUDPParserService::sendTagDailyPDFReportToClient() UseRoomm AreaID[%s]\n", AreaID.c_str());

            PDFTableDataStruct * pdfUserRoomInOut = prepareUserRoomInOutReport(projectid, nodeAreaInOutList1, id, AreaID, report_start_datetime, report_end_datetime);
            PDFTableDataStruct * pdfDataAreaInOut = prepareTagDailyAreaInOutReport(projectid, nodeAreaInOutList1, id, report_start_datetime, report_end_datetime);
            PDFTableDataStruct * pdfDataAlertEvent = prepareTagDailyAlertEventReport(projectid, firedEventList, id, report_start_datetime, report_end_datetime);
            PDFTableDataStruct * pdfDataEvent = prepareTagDailyEventReport(projectid, firedEventList, id, report_start_datetime, report_end_datetime);
            std::list<PDFTableDataStruct*> * pdfDataCourses = prepareTagDailyCourseReport(projectid, nodeAreaInOutList2, id, report_start_datetime, report_end_datetime);

            std::string randomFilename = random_string(16);

            std::string pdffilename = "/upload/";
            pdffilename.append(randomFilename);
            pdffilename.append(".pdf");

            std::string pdfOutputfile = OUTPUT_DIRECTORY;
            pdfOutputfile.append(pdffilename);

            vilslog::printf("WebUDPParserService::sendTagDailyPDFReportToClient() pdfOutputfile[%s]\n", pdfOutputfile.c_str());

            PDFCreater * pdfCreater = new PDFCreater();
            pdfCreater->create_report_header(pdfOutputfile, "標籤足跡紀錄", report_date, report_start_date, report_end_date);
            if ( node != NULL )
                pdfCreater->create_report_userinfo(report_type, ShowName);

            if(pdfUserRoomInOut->rowdatalist.size() > 0)
            {
                pdfCreater->create_report_tableinfo("進出居住者房間", 12);
                pdfCreater->create_report_tableinfo(pdfUserRoomInOut->tableinfo, 10);
                pdfCreater->create_report_table(pdfUserRoomInOut);
            }
            if(pdfDataAreaInOut->rowdatalist.size() > 0)
            {
                pdfCreater->create_report_tableinfo("區域進出", 12);
                pdfCreater->create_report_table(pdfDataAreaInOut);
            }
            if(pdfDataAlertEvent->rowdatalist.size() > 0)
            {
                pdfCreater->create_report_tableinfo("發出緊急求助事件", 12);
                pdfCreater->create_report_table(pdfDataAlertEvent);
            }
            if(pdfDataEvent->rowdatalist.size() > 0)
            {
                pdfCreater->create_report_tableinfo("觸發事件", 12);
                pdfCreater->create_report_table(pdfDataEvent);
            }

            for (std::list<PDFTableDataStruct*>::iterator course = pdfDataCourses->begin(); course != pdfDataCourses->end(); course++)
            {
                if((*course)->rowdatalist.size() > 0)
                {
                    pdfCreater->create_report_tableinfo("課程紀錄", 12);
                    pdfCreater->create_report_tableinfo((*course)->tableinfo, 10);
                    pdfCreater->create_report_table((*course));
                }
            }

            pdfCreater->generate_report();
            delete pdfCreater;

            // put to remove queue
            QRCodeStruct * newQRCcode = new QRCodeStruct();
            newQRCcode->filepath = pdfOutputfile;
            newQRCcode->createtime = getCurrentTimeInSec();
            mQRCodeImageList.push_back(newQRCcode);

            sendstream<<"{\"pdfreport\":\"";
            sendstream<<pdffilename;
            sendstream<<"\",\"success\":\"true\"}";

            clear_PDFTableDataStruct(pdfUserRoomInOut);
            clear_PDFTableDataStruct(pdfDataAreaInOut);
            clear_PDFTableDataStruct(pdfDataAlertEvent);
            clear_PDFTableDataStruct(pdfDataEvent);
            clear_PDFTableDataStruct_list(pdfDataCourses);

            delete pdfUserRoomInOut;
            delete pdfDataAreaInOut;
            delete pdfDataAlertEvent;
            delete pdfDataEvent;
            delete pdfDataCourses;

            clear_FiredEventStruct_list(firedEventList);
            delete firedEventList;
            clear_AreaInOutEventStruct_list(nodeAreaInOutList1);
            delete nodeAreaInOutList1;
            clear_AreaInOutEventStruct_list(nodeAreaInOutList2);
            delete nodeAreaInOutList2;
        }
    }
    else
    {
        sendstream<<"{\"pdfreport\":\"\",\"success\":\"false\"}";
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

PDFTableDataStruct * WebUDPParserService::prepareUserRoomInOutReport(int projectid, std::list<AreaInOutEventStruct*> * nodeAreaInOutList, std::string nodeid, std::string AreaID, std::string start_datetime, std::string end_datetime)
{
    PDFTableDataStruct * pdfUserRoomInOut = new PDFTableDataStruct();
    pdfUserRoomInOut->type = "UserRoomInOut";
    pdfUserRoomInOut->startdate = start_datetime;
    pdfUserRoomInOut->enddate = end_datetime;
    pdfUserRoomInOut->tableinfo = "";

    pdfUserRoomInOut->headerlist.push_back(create_pdf_headerdata("進入房間時間", 120));
    pdfUserRoomInOut->headerlist.push_back(create_pdf_headerdata("離開房間時間", 120));
    pdfUserRoomInOut->headerlist.push_back(create_pdf_headerdata("人員", 120));
    pdfUserRoomInOut->headerlist.push_back(create_pdf_headerdata("停留時間", 50));

    std::list<node_AreaInOutEventStruct*> * nodeAreaInOutEventList = new std::list<node_AreaInOutEventStruct*>();

    for (std::list<AreaInOutEventStruct*>::iterator event = nodeAreaInOutList->begin(); event != nodeAreaInOutList->end(); event++)
    {
        if ( ((*event)->DataTime.compare(start_datetime) >= 0) &&
             ((*event)->DataTime.compare(end_datetime) <= 0) &&
             ((*event)->AreaID.compare(AreaID) == 0) )
        {

            bool bFound = false;
            for (std::list<node_AreaInOutEventStruct *>::iterator nodevent = nodeAreaInOutEventList->begin(); nodevent != nodeAreaInOutEventList->end(); nodevent++)
            {
                if ( (*nodevent)->nodeid.compare((*event)->NodeId) == 0 )
                {
                    AreaInOutEventStruct * target_event = copyAreaInOutEventStruct((*event));
                    (*nodevent)->area_inout_events->push_back(target_event);
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
            {
                node_AreaInOutEventStruct * newNodeEvent = new node_AreaInOutEventStruct();
                newNodeEvent->nodeid = (*event)->NodeId;
                newNodeEvent->area_inout_events = new std::list<AreaInOutEventStruct*>();

                AreaInOutEventStruct * target_event = copyAreaInOutEventStruct((*event));
                newNodeEvent->area_inout_events->push_back(target_event);

                nodeAreaInOutEventList->push_back(newNodeEvent);
            }
        }
    }

    vilslog::printf ("WebUDPParserService::prepareUserRoomInOutReport() nodeid[%s] AreaID[%s] nodeAreaInOutEventList->size()[%d]\n",
        nodeid.c_str(), AreaID.c_str(), (int)nodeAreaInOutEventList->size());

    for (std::list<node_AreaInOutEventStruct*>::iterator event = nodeAreaInOutEventList->begin(); event != nodeAreaInOutEventList->end(); event++)
    {
        vilslog::printf ("WebUDPParserService::prepareUserRoomInOutReport() nodeid[%s] area_inout_events->size()[%d]\n",
            (*event)->nodeid.c_str(), (int)(*event)->area_inout_events->size());


        std::string user_name = "";
        NodeStruct * node = mDeviceManager->get_Tag_node(projectid, (*event)->nodeid.c_str());
        if (node != NULL)
        {
            user_name = node->nodename;
        }

        std::list<AreaInOutEventStruct*> * merge_events = merge_inout_event((*event)->area_inout_events);

        std::list<Pair_AreaInOutEventStruct*> * pair_inout_events = pair_inout_event(merge_events);

        std::string today_date = StringDateTimeToStringDate(start_datetime);

        std::string inarea_in_datetime = "";
        std::string inarea_out_datetime = "";
        time_t t_total_inarea = 0;

        for (std::list<Pair_AreaInOutEventStruct*>::iterator pairevent = pair_inout_events->begin(); pairevent != pair_inout_events->end(); pairevent++)
        {
            std::string area_in_datetime = "今日無此紀錄";
            std::string area_out_datetime = "今日無此紀錄";
            std::string total_time = "0";

            if ( (*pairevent)->type.compare("single") == 0 )
            {
                if ( (*pairevent)->event1->action == 1 )
                {
                    if (inarea_in_datetime.length() == 0)
                    {
                        inarea_in_datetime = (*pairevent)->event1->DataTime;
                    }

                    area_in_datetime = (*pairevent)->event1->DataTime;
                    time_t t_in = StringDateTimeTotime_t((*pairevent)->event1->DataTime);
                    time_t t_out = 0;

                    // 以當日 24:00:00 作結束 或是 目前時間
                    std::string end_date = StringDateTimeToStringDate(area_in_datetime);
                    if ( today_date.compare(end_date) == 0 )
                    {
                        // 以當日 目前時間 作結束 或是
                        std::string cur_datetime = currentDateTimeForSQL();
                        t_out = StringDateTimeTotime_t(cur_datetime);
                    }
                    else
                    {
                        // 以當日 24:00:00 作結束
                        // std::string endDatetime = end_date;
                        // endDatetime.append(" 23:59:59");
                        t_out = StringDateTimeTotime_t(end_datetime);
                    }
                    time_t t_diff = t_out - t_in;
                    total_time = time_tToTimerString(t_diff);
                }
                else
                {
                    if (inarea_out_datetime.compare((*pairevent)->event1->DataTime) < 0)
                    {
                        inarea_out_datetime = (*pairevent)->event1->DataTime;
                    }

                    area_out_datetime = (*pairevent)->event1->DataTime;
                    time_t t_in = 0;
                    time_t t_out = StringDateTimeTotime_t((*pairevent)->event1->DataTime);

                    // 以當日 00:00:00 作開始
                    //std::string start_date = StringDateTimeToStringDate(area_out_datetime);
                    // 以當日 00:00:00 作開始
                    //std::string startDatetime = start_date;
                    //startDatetime.append(" 00:00:00");

                    t_in = StringDateTimeTotime_t(start_datetime);
                    time_t t_diff = t_out - t_in;
                    total_time = time_tToTimerString(t_diff);
                }
            }
            else
            if ( (*pairevent)->type.compare("pair") == 0 )
            {
                area_in_datetime = (*pairevent)->event1->DataTime;
                area_out_datetime = (*pairevent)->event2->DataTime;

                time_t t_in = StringDateTimeTotime_t((*pairevent)->event1->DataTime);
                time_t t_out = StringDateTimeTotime_t((*pairevent)->event2->DataTime);
                time_t t_diff = t_out - t_in;
                t_total_inarea += t_diff;
                total_time = time_tToTimerString(t_diff);

                if (inarea_in_datetime.length() == 0)
                {
                    inarea_in_datetime = (*pairevent)->event1->DataTime;
                }
                if (inarea_out_datetime.compare((*pairevent)->event2->DataTime) < 0)
                {
                    inarea_out_datetime = (*pairevent)->event2->DataTime;
                }
            }

            std::string area_name = (*pairevent)->event1->AreaName;
            if (pdfUserRoomInOut->tableinfo.length() == 0)
            {
                pdfUserRoomInOut->tableinfo = "居住者房間:";
                pdfUserRoomInOut->tableinfo.append(area_name);
            }

            // std::string areatype_name = getAreaTypeName((*event)->event1->MapAreaType);
            //
            // vilslog::printf("PDFCreater::prepareUserRoomInOutReport() user_name[%s] area_in_datetime[%s] area_out_datetime[%s] area_name[%s] total_time[%s]\n",
            //     user_name.c_str(), area_in_datetime.c_str(), area_out_datetime.c_str(), area_name.c_str(), total_time.c_str());

            PDFTableRowDataStruct * newRowData = new PDFTableRowDataStruct();

            newRowData->coldatalist.push_back(create_pdf_columndata("進入房間時間", area_in_datetime));
            newRowData->coldatalist.push_back(create_pdf_columndata("離開房間時間", area_out_datetime));
            newRowData->coldatalist.push_back(create_pdf_columndata("人員", user_name));
            newRowData->coldatalist.push_back(create_pdf_columndata("停留時間", total_time));

            pdfUserRoomInOut->rowdatalist.push_back(newRowData);
        }

        std::string inarea_total_time = time_tToTimerString(t_total_inarea);

        vilslog::printf("PDFCreater::prepareUserRoomInOutReport() inarea_in_datetime[%s] inarea_out_datetime[%s] inarea_total_time[%s] \n",
            inarea_in_datetime.c_str(), inarea_out_datetime.c_str(), inarea_total_time.c_str());


        clear_AreaInOutEventStruct_list(merge_events);
        delete merge_events;
        clear_Pair_AreaInOutEventStruct_list(pair_inout_events);
        delete pair_inout_events;
    }

    clear_node_AreaInOutEventStruct_list(nodeAreaInOutEventList);
    delete nodeAreaInOutEventList;

    return pdfUserRoomInOut;
}

PDFTableDataStruct * WebUDPParserService::prepareTagDailyAreaInOutReport(int projectid, std::list<AreaInOutEventStruct*> * nodeAreaInOutList, std::string nodeid, std::string start_datetime, std::string end_datetime)
{
    PDFTableDataStruct * pdfDataAreaInOut = new PDFTableDataStruct();
    pdfDataAreaInOut->type = "areainout";
    pdfDataAreaInOut->startdate = start_datetime;
    pdfDataAreaInOut->enddate = end_datetime;

    std::list<AreaInOutEventStruct*> * merge_events = merge_inout_event(nodeAreaInOutList);

    std::list<Pair_AreaInOutEventStruct*> * pair_inout_events = pair_inout_event(merge_events);

    vilslog::printf ("WebUDPParserService::prepareTagDailyAreaInOutReport() nodeid[%s] nodeAreaInOutList->size()[%d]\n",
        nodeid.c_str(), (int)nodeAreaInOutList->size());

    pdfDataAreaInOut->headerlist.push_back(create_pdf_headerdata("進入時間", 120));
    pdfDataAreaInOut->headerlist.push_back(create_pdf_headerdata("離開時間", 120));
    pdfDataAreaInOut->headerlist.push_back(create_pdf_headerdata("到訪區域", 50));
    pdfDataAreaInOut->headerlist.push_back(create_pdf_headerdata("區域類別", 50));
    pdfDataAreaInOut->headerlist.push_back(create_pdf_headerdata("停留時間", 50));

    std::string today_date = TodayDateType1();

    for (std::list<Pair_AreaInOutEventStruct*>::iterator event = pair_inout_events->begin(); event != pair_inout_events->end(); event++)
    {
        std::string area_in_datetime = "今日無此紀錄";
        std::string area_out_datetime = "今日無此紀錄";
        std::string total_time = "0";

        if ( (*event)->type.compare("single") == 0 )
        {
            if ( (*event)->event1->action == 1 )
            {
                area_in_datetime = (*event)->event1->DataTime;
                time_t t_in = StringDateTimeTotime_t((*event)->event1->DataTime);
                time_t t_out = 0;

                // 以當日 24:00:00 作結束 或是 目前時間
                std::string end_date = StringDateTimeToStringDate(area_in_datetime);
                if ( today_date.compare(end_date) == 0 )
                {
                    // 以當日 目前時間 作結束 或是
                    std::string cur_datetime = currentDateTimeForSQL();
                    t_out = StringDateTimeTotime_t(cur_datetime);
                }
                else
                {
                    // 以當日 24:00:00 作結束
                    std::string endDatetime = end_date;
                    endDatetime.append(" 23:59:59");
                    t_out = StringDateTimeTotime_t(endDatetime);
                }
                time_t t_diff = t_out - t_in;
                total_time = time_tToTimerString(t_diff);
            }
            else
            {
                area_out_datetime = (*event)->event1->DataTime;
                time_t t_in = 0;
                time_t t_out = StringDateTimeTotime_t((*event)->event1->DataTime);

                // 以當日 00:00:00 作開始
                std::string start_date = StringDateTimeToStringDate(area_out_datetime);
                // 以當日 00:00:00 作開始
                std::string startDatetime = start_date;
                startDatetime.append(" 00:00:00");
                t_in = StringDateTimeTotime_t(startDatetime);
                time_t t_diff = t_out - t_in;
                total_time = time_tToTimerString(t_diff);
            }
        }
        else
        if ( (*event)->type.compare("pair") == 0 )
        {
            area_in_datetime = (*event)->event1->DataTime;
            area_out_datetime = (*event)->event2->DataTime;

            time_t t_in = StringDateTimeTotime_t((*event)->event1->DataTime);
            time_t t_out = StringDateTimeTotime_t((*event)->event2->DataTime);
            time_t t_diff = t_out - t_in;
            total_time = time_tToTimerString(t_diff);
        }
        std::string area_name = (*event)->event1->AreaName;

        std::string areatype_name = getAreaTypeName((*event)->event1->MapAreaType);

        // vilslog::printf("PDFCreater::prepareTagDailyAreaInOutReport() area_in_datetime[%s] area_out_datetime[%s] area_name[%s] total_time[%s]\n",
        //     area_in_datetime.c_str(), area_out_datetime.c_str(), area_name.c_str(), total_time.c_str());

        PDFTableRowDataStruct * newRowData = new PDFTableRowDataStruct();

        newRowData->coldatalist.push_back(create_pdf_columndata("進入時間", area_in_datetime));
        newRowData->coldatalist.push_back(create_pdf_columndata("離開時間", area_out_datetime));
        newRowData->coldatalist.push_back(create_pdf_columndata("到訪區域", area_name));
        newRowData->coldatalist.push_back(create_pdf_columndata("區域類別", areatype_name));
        newRowData->coldatalist.push_back(create_pdf_columndata("停留時間", total_time));

        pdfDataAreaInOut->rowdatalist.push_back(newRowData);
    }

    clear_AreaInOutEventStruct_list(merge_events);
    delete merge_events;

    clear_Pair_AreaInOutEventStruct_list(pair_inout_events);
    delete pair_inout_events;

    return pdfDataAreaInOut;
}

PDFTableDataStruct * WebUDPParserService::prepareTagDailyAlertEventReport(int projectid, std::list<FiredEventStruct*> * firedEventList, std::string nodeid, std::string start_datetime, std::string end_datetime)
{
    PDFTableDataStruct * pdfDataEvent = new PDFTableDataStruct();
    pdfDataEvent->type = "areainout";
    pdfDataEvent->startdate = start_datetime;
    pdfDataEvent->enddate = end_datetime;

    vilslog::printf ("WebUDPParserService::prepareTagDailyAlertEventReport() nodeid[%s] firedEventList->size()[%d]\n", nodeid.c_str(), (int)firedEventList->size());

    std::list<FiredEventStruct*> * alertCancelEventsList = getAlertCancelActionEventList(firedEventList, nodeid);

    vilslog::printf ("WebUDPParserService::prepareTagDailyAlertEventReport() nodeid[%s] alertCancelEventsList->size()[%d]\n", nodeid.c_str(), (int)alertCancelEventsList->size());

    std::list<Pair_FiredEventStruct*> * pair_events = getPairEvents(alertCancelEventsList);

    vilslog::printf ("WebUDPParserService::prepareTagDailyAlertEventReport() nodeid[%s] pair_events->size()[%d]\n", nodeid.c_str(), (int)pair_events->size());
    //
    // generate pdf report file
    //
    NodeStruct * node = mDeviceManager->get_Tag_node(projectid, nodeid.c_str());
    if ( node == NULL )
    {
        node = mDeviceManager->get_Locator_node_byid(projectid, nodeid.c_str());
    }

    pdfDataEvent->headerlist.push_back(create_pdf_headerdata("警急求助觸發時間", 120));
    pdfDataEvent->headerlist.push_back(create_pdf_headerdata("取消時間", 120));
    pdfDataEvent->headerlist.push_back(create_pdf_headerdata("處理工作人員", 80));

    for (auto pairevent = pair_events->begin(); pairevent != pair_events->end(); pairevent++)
    {
        std::string alert_datetime = (*pairevent)->event1->datetime;
        std::string cancel_datetime = "";
        int userid = 0;
        std::string empname = "";
        if ((*pairevent)->event2 != NULL)
        {
            cancel_datetime = (*pairevent)->event2->datetime;
            userid = convert_string_to_int((*pairevent)->event2->empid);
            empname = mDeviceManager->getUserName(projectid, userid);
        }


        // std::string empid = (*pairevent)->event2->empid;
        // std::string empname = empid;
        //
        // UserStruct * user = mDeviceManager->getUser(projectid, empid);
        // if (user != NULL)
        // {
        //     empname = user->name;
        // }

        PDFTableRowDataStruct * newRowData = new PDFTableRowDataStruct();

        newRowData->coldatalist.push_back(create_pdf_columndata("警急求助觸發時間", alert_datetime));
        newRowData->coldatalist.push_back(create_pdf_columndata("取消時間", cancel_datetime));
        newRowData->coldatalist.push_back(create_pdf_columndata("處理工作人員", empname));

        pdfDataEvent->rowdatalist.push_back(newRowData);
    }


    clear_Pair_FiredEventStruct_list(pair_events);
    delete pair_events;
    clear_FiredEventStruct_list(alertCancelEventsList);
    delete alertCancelEventsList;

    return pdfDataEvent;
}

PDFTableDataStruct * WebUDPParserService::prepareTagDailyEventReport(int projectid, std::list<FiredEventStruct*> * firedEventList, std::string nodeid, std::string start_datetime, std::string end_datetime)
{
    PDFTableDataStruct * pdfDataEvent = new PDFTableDataStruct();
    pdfDataEvent->type = "event";
    pdfDataEvent->startdate = start_datetime;
    pdfDataEvent->enddate = end_datetime;

    vilslog::printf ("WebUDPParserService::prepareTagDailyEventReport() nodeid[%s] firedEventList->size()[%d]\n", nodeid.c_str(), (int)firedEventList->size());

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);

    //
    // generate pdf report file
    //
    NodeStruct * node = mDeviceManager->get_Tag_node(projectid, nodeid.c_str());
    if ( node == NULL )
    {
        node = mDeviceManager->get_Locator_node_byid(projectid, nodeid.c_str());
    }

    pdfDataEvent->headerlist.push_back(create_pdf_headerdata("事件觸發時間", 120));
    pdfDataEvent->headerlist.push_back(create_pdf_headerdata("事件", 300));

    for (auto event = firedEventList->begin(); event != firedEventList->end(); event++)
    {
        std::string alert_datetime = (*event)->datetime;
        std::string event_detail = "";

        Json::Value JSONresult;

        Json::Reader * reader = new Json::Reader();
        bool parsingSuccessful = reader->parse( (*event)->eventjson.c_str(), JSONresult);
        delete reader;

        if (!parsingSuccessful)
        {
            continue;
        }

        if ((*event)->eventid == 999)
        {
            std::string Inspector = JSONresult["Inspector"].asString();
            std::string Action = JSONresult["action"].asString();
            std::string AnchorNodeid = JSONresult["nodeid"].asString();

            if ( Inspector.compare(nodeid) != 0)
            {
                continue;
            }

            NodeStruct * InspectorNode = mDeviceManager->get_Tag_node(projectid, Inspector.c_str());
            NodeStruct * AnchorNode = mDeviceManager->get_Anchor_node(projectid, AnchorNodeid.c_str());
            if (AnchorNode == NULL)
            {
                AnchorNode = mDeviceManager->get_Locator_node(projectid, AnchorNodeid.c_str());
            }

            std::string InspectorName = Inspector;
            std::string ActionName = getEventActionName(Action);
            std::string nodeName = "";

            if (InspectorNode != NULL)
            {
                InspectorName = InspectorNode->nodename;
            }
            if (AnchorNode != NULL)
            {
                nodeName = AnchorNode->nodename;
            }

            event_detail.append(InspectorName);
            event_detail.append("  ");
            event_detail.append(ActionName);
            event_detail.append("  巡檢點  ");
            event_detail.append(nodeName);
        }
        else
        {
            std::string tagNodeId = JSONresult["nodeid"].asString();
            std::string locatorNodeId = JSONresult["locatorid"].asString();
            std::string Action = JSONresult["action"].asString();
            std::string areaid = JSONresult["areaid"].asString();
            std::string issuetime = JSONresult["issuetime"].asString();

            if ( (nodeid.compare(tagNodeId) != 0) && (nodeid.compare(locatorNodeId) != 0) )
            {
                continue;
            }

            NodeStruct * tagNode = mDeviceManager->get_Tag_node(projectid, tagNodeId.c_str());

            std::string tagName = tagNodeId;
            std::string ActionName = getEventActionName(Action);
            std::string IssueTimeName = getEventIssueTimeName(issuetime);
            std::string areaName = areaid;

            if (tagNode != NULL)
            {
                tagName = tagNode->nodename;
            }

            if (baseDb != NULL)
            {
                areaName = baseDb->fetchAreaName(areaid);
            }

            event_detail.append(tagName);
            event_detail.append("  ");
            event_detail.append(ActionName);
            event_detail.append("  ");
            event_detail.append(areaName);
            event_detail.append("  ");
            event_detail.append(IssueTimeName);
        }

        PDFTableRowDataStruct * newRowData = new PDFTableRowDataStruct();

        newRowData->coldatalist.push_back(create_pdf_columndata("事件觸發時間", alert_datetime));
        newRowData->coldatalist.push_back(create_pdf_columndata("事件", event_detail));

        pdfDataEvent->rowdatalist.push_back(newRowData);
    }

    return pdfDataEvent;
}

std::list<PDFTableDataStruct*> * WebUDPParserService::prepareTagDailyCourseReport(int projectid, std::list<AreaInOutEventStruct*> * nodeAreaInOutList, std::string nodeid, std::string start_datetime, std::string end_datetime)
{
    std::list<PDFTableDataStruct*> * pdfDataCourses = new std::list<PDFTableDataStruct*>();
    //pdfDataCourse->type = "course";
    //pdfDataCourse->startdate = start_datetime;
    //pdfDataCourse->enddate = end_datetime;

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
        return pdfDataCourses;

    std::list<CourseRollCallRecordStruct *> * rollcallRecordList = baseDb->read_rollcallRecordDayRange(start_datetime.c_str(), end_datetime.c_str());

    if ( rollcallRecordList == NULL )
        return pdfDataCourses;

    // vilslog::printf ("WebUDPParserService::prepareTagDailyCourseReport() nodeid[%s] start_datetime[%s] end_datetime[%s] rollcallRecordList->size()[%d]\n",
    //     nodeid.c_str(), start_datetime.c_str(), end_datetime.c_str(), (int)rollcallRecordList->size());

    for (std::list<CourseRollCallRecordStruct *>::iterator rollcall = rollcallRecordList->begin(); rollcall != rollcallRecordList->end(); rollcall++)
    {
        for (std::list<std::string>::iterator tag = (*rollcall)->tags->begin(); tag != (*rollcall)->tags->end(); tag++)
        {
            if ( (*tag).compare(nodeid) == 0 )
            {
                // vilslog::printf ("WebUDPParserService::prepareTagDailyCourseReport() courseId[%s] infoOfDate[%s] issuetime[%s]\n",
                //     (*rollcall)->courseId.c_str(), (*rollcall)->infoOfDate.c_str(), (*rollcall)->issuetime.c_str());

                CourseStruct * course = baseDb->read_Course((*rollcall)->courseId.c_str());
                if (course != NULL)
                {
                    //std::string targetDate = (*rollcall)->issuetime.substr(0, 10);

                    // vilslog::printf ("WebUDPParserService::prepareTagDailyCourseReport() title[%s] start[%s] end[%s] startTime[%s] endTime[%s] startRecur[%s] endRecur[%s]\n",
                    //     course->title.c_str(), course->start.c_str(), course->end.c_str(), course->startTime.c_str(), course->endTime.c_str(),
                    //     course->startRecur.c_str(), course->endRecur.c_str());

                    PDFTableDataStruct * pdfDataCourse = prepareTagCourseReport(projectid, nodeAreaInOutList, course, nodeid, start_datetime, end_datetime);
                    if(pdfDataCourse != NULL)
                    {
                        pdfDataCourses->push_back(pdfDataCourse);
                    }
                }
            }
        }
    }

    clear_CourseRollCallRecordStruct_list(rollcallRecordList);
    delete rollcallRecordList;

    return pdfDataCourses;
}

PDFTableDataStruct * WebUDPParserService::prepareTagCourseReport(int projectid, std::list<AreaInOutEventStruct*> * nodeAreaInOutList, CourseStruct * course, std::string nodeid, std::string start_datetime, std::string end_datetime)
{
    PDFTableDataStruct * pdfDataCourse = new PDFTableDataStruct();
    pdfDataCourse->type = "course";
    pdfDataCourse->startdate = start_datetime;
    pdfDataCourse->enddate = end_datetime;
    pdfDataCourse->tableinfo = "";

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);

    std::string data_start_datetime = start_datetime;
    std::string data_end_datetime = end_datetime;

    vilslog::printf ("\nWebUDPParserService::prepareTagCourseReport() title[%s] allDay[%d] start[%s] end[%s] startTime[%s] endTime[%s] startRecur[%s] endRecur[%s]\n",
        course->title.c_str(), course->allDay, course->start.c_str(), course->end.c_str(), course->startTime.c_str(), course->endTime.c_str(),
        course->startRecur.c_str(), course->endRecur.c_str());

    if ( course->allDay == 1)
    {
        data_start_datetime = start_datetime;
        data_end_datetime = end_datetime;
    }
    else
    {
        if ( (course->startRecur.length() == 0) && (course->endRecur.length() == 0) )
        {
            //
            // single day
            //
            data_start_datetime = course->start;
            data_end_datetime = course->end;
        }
        else
        {
            //
            // long range
            //
            data_start_datetime = StringDateTimeToStringDate(start_datetime);
            data_start_datetime.append(" ");
            data_start_datetime.append(course->startTime);
            data_start_datetime.append(":00");

            data_end_datetime = StringDateTimeToStringDate(start_datetime);
            data_end_datetime.append(" ");
            data_end_datetime.append(course->endTime);
            data_end_datetime.append(":00");

        }
    }

    pdfDataCourse->headerlist.push_back(create_pdf_headerdata("進入課程時間", 120));
    pdfDataCourse->headerlist.push_back(create_pdf_headerdata("離開課程時間", 120));
    pdfDataCourse->headerlist.push_back(create_pdf_headerdata("停留時間", 50));

    vilslog::printf ("WebUDPParserService::prepareTagCourseReport() data_start_datetime[%s] data_end_datetime[%s] resourceId[%s]\n",
        data_start_datetime.c_str(), data_end_datetime.c_str(), course->resourceId.c_str());

    std::list<AreaInOutEventStruct*> * target_inout_events = new std::list<AreaInOutEventStruct*>();

    for (std::list<AreaInOutEventStruct*>::iterator event = nodeAreaInOutList->begin(); event != nodeAreaInOutList->end(); event++)
    {
        if ( ((*event)->DataTime.compare(data_start_datetime) >= 0) &&
             ((*event)->DataTime.compare(data_end_datetime) <= 0) &&
             ((*event)->AreaID.compare(course->resourceId) == 0) )
        {
            vilslog::printf ("WebUDPParserService::prepareTagCourseReport() Found (*event)->DataTime[%s] action[%d]\n",
                (*event)->DataTime.c_str(), (*event)->action);

            AreaInOutEventStruct * target_event = copyAreaInOutEventStruct((*event));
            target_inout_events->push_back(target_event);
        }
    }

    std::list<AreaInOutEventStruct*> * merge_events = merge_inout_event(target_inout_events);

    std::list<Pair_AreaInOutEventStruct*> * pair_inout_events = pair_inout_event(merge_events);

    vilslog::printf ("WebUDPParserService::prepareTagCourseReport() merge_events->size()[%d] pair_inout_events->size()[%d]\n",
        (int)merge_events->size(), (int)pair_inout_events->size());

    std::string today_date = StringDateTimeToStringDate(start_datetime);

    std::string inarea_in_datetime = "";
    std::string inarea_out_datetime = "";
    time_t t_total_inarea = 0;

    for (std::list<Pair_AreaInOutEventStruct*>::iterator event = pair_inout_events->begin(); event != pair_inout_events->end(); event++)
    {
        std::string area_in_datetime = "今日無此紀錄";
        std::string area_out_datetime = "今日無此紀錄";
        std::string total_time = "0";

        if ( (*event)->type.compare("single") == 0 )
        {
            if ( (*event)->event1->action == 1 )
            {
                if (inarea_in_datetime.length() == 0)
                {
                    inarea_in_datetime = (*event)->event1->DataTime;
                }

                area_in_datetime = (*event)->event1->DataTime;
                time_t t_in = StringDateTimeTotime_t((*event)->event1->DataTime);
                time_t t_out = 0;

                // 以當日 24:00:00 作結束 或是 目前時間
                std::string end_date = StringDateTimeToStringDate(area_in_datetime);
                if ( today_date.compare(end_date) == 0 )
                {
                    // 以當日 目前時間 作結束 或是
                    std::string cur_datetime = currentDateTimeForSQL();
                    t_out = StringDateTimeTotime_t(cur_datetime);
                }
                else
                {
                    // 以當日 24:00:00 作結束
                    // std::string endDatetime = end_date;
                    // endDatetime.append(" 23:59:59");
                    t_out = StringDateTimeTotime_t(data_end_datetime);
                }
                time_t t_diff = t_out - t_in;
                total_time = time_tToTimerString(t_diff);
            }
            else
            {
                if (inarea_out_datetime.compare((*event)->event1->DataTime) < 0)
                {
                    inarea_out_datetime = (*event)->event1->DataTime;
                }

                area_out_datetime = (*event)->event1->DataTime;
                time_t t_in = 0;
                time_t t_out = StringDateTimeTotime_t((*event)->event1->DataTime);

                // 以當日 00:00:00 作開始
                //std::string start_date = StringDateTimeToStringDate(area_out_datetime);
                // 以當日 00:00:00 作開始
                //std::string startDatetime = start_date;
                //startDatetime.append(" 00:00:00");

                t_in = StringDateTimeTotime_t(data_start_datetime);
                time_t t_diff = t_out - t_in;
                total_time = time_tToTimerString(t_diff);
            }
        }
        else
        if ( (*event)->type.compare("pair") == 0 )
        {
            area_in_datetime = (*event)->event1->DataTime;
            area_out_datetime = (*event)->event2->DataTime;

            time_t t_in = StringDateTimeTotime_t((*event)->event1->DataTime);
            time_t t_out = StringDateTimeTotime_t((*event)->event2->DataTime);
            time_t t_diff = t_out - t_in;
            t_total_inarea += t_diff;
            total_time = time_tToTimerString(t_diff);

            if (inarea_in_datetime.length() == 0)
            {
                inarea_in_datetime = (*event)->event1->DataTime;
            }
            if (inarea_out_datetime.compare((*event)->event2->DataTime) < 0)
            {
                inarea_out_datetime = (*event)->event2->DataTime;
            }
        }
        std::string area_name = (*event)->event1->AreaName;

        std::string areatype_name = getAreaTypeName((*event)->event1->MapAreaType);

        vilslog::printf("PDFCreater::prepareTagCourseReport() area_in_datetime[%s] area_out_datetime[%s] area_name[%s] total_time[%s]\n",
            area_in_datetime.c_str(), area_out_datetime.c_str(), area_name.c_str(), total_time.c_str());

        PDFTableRowDataStruct * newRowData = new PDFTableRowDataStruct();

        newRowData->coldatalist.push_back(create_pdf_columndata("進入課程時間", area_in_datetime));
        newRowData->coldatalist.push_back(create_pdf_columndata("離開課程時間", area_out_datetime));
        newRowData->coldatalist.push_back(create_pdf_columndata("停留時間", total_time));

        pdfDataCourse->rowdatalist.push_back(newRowData);
    }

    AreaInOutEventStruct * areaevent_in = NULL;
    AreaInOutEventStruct * areaevent_out = NULL;

    if ( inarea_in_datetime.length() == 0 )
    {
        // 沒有進入資料
        areaevent_in = baseDb->getNodeLastAreaInOutList(nodeid.c_str(), course->resourceId.c_str(), data_start_datetime.c_str(), 1);
        areaevent_out = baseDb->getNodeLastAreaInOutList(nodeid.c_str(), course->resourceId.c_str(), data_start_datetime.c_str(), 2);

        bool bStillInside = false;
        if ( areaevent_in->DataTime.compare(areaevent_out->DataTime) > 0)
        {
            bStillInside = true;
        }

        vilslog::printf("WebUDPParserService::prepareTagCourseReport() areaevent_in->DataTime[%s] areaevent_out->DataTime[%s] data_start_datetime[%s] bStillInside[%d]\n",
            areaevent_in->DataTime.c_str(), areaevent_out->DataTime.c_str(), data_start_datetime.c_str(), bStillInside);

        if(bStillInside)
        {
            if ( data_start_datetime.compare(areaevent_in->DataTime) >= 0)
            {
                inarea_in_datetime = data_start_datetime;
            }
            else
            {
                inarea_in_datetime = areaevent_in->DataTime;
            }
        }
        else
        {
            // 已離開始此區域
            inarea_in_datetime = "";
        }
    }

    if ( inarea_out_datetime.length() == 0 )
    {
        // 沒有離開資料
        std::string curDatetime = currentDateTimeForSQL();
        std::string targetDatetime = curDatetime;
        if (curDatetime.compare(data_end_datetime) >= 0 )
            targetDatetime = data_end_datetime;

        if(areaevent_in == NULL)
            areaevent_in = baseDb->getNodeLastAreaInOutList(nodeid.c_str(), course->resourceId.c_str(), data_start_datetime.c_str(), 1);
        if(areaevent_out == NULL)
            areaevent_out = baseDb->getNodeLastAreaInOutList(nodeid.c_str(), course->resourceId.c_str(), data_start_datetime.c_str(), 2);

        bool bStillInside = false;
        if ( areaevent_in->DataTime.compare(areaevent_out->DataTime) > 0)
        {
            bStillInside = true;
        }

        vilslog::printf("WebUDPParserService::prepareTagCourseReport() areaevent_in->DataTime[%s] areaevent_out->DataTime[%s] data_end_datetime[%s] bStillInside[%d]\n",
            areaevent_in->DataTime.c_str(), areaevent_out->DataTime.c_str(), data_end_datetime.c_str(), bStillInside);

        if(bStillInside)
        {
            inarea_out_datetime = targetDatetime;
        }
        else
        {
            // 已離開始此區域
            inarea_out_datetime = "";
        }
    }

    if (t_total_inarea == 0)
    {
        time_t t_in = StringDateTimeTotime_t(inarea_in_datetime);
        time_t t_out = StringDateTimeTotime_t(inarea_out_datetime);
        time_t t_diff = t_out - t_in;
        t_total_inarea += t_diff;
    }

    std::string inarea_total_time = time_tToTimerString(t_total_inarea);

    vilslog::printf("WebUDPParserService::prepareTagCourseReport() inarea_in_datetime[%s] inarea_out_datetime[%s] inarea_total_time[%s] \n",
        inarea_in_datetime.c_str(), inarea_out_datetime.c_str(), inarea_total_time.c_str());

    if ( (int)pdfDataCourse->rowdatalist.size() == 0)
    {
        PDFTableRowDataStruct * newRowData = new PDFTableRowDataStruct();

        newRowData->coldatalist.push_back(create_pdf_columndata("進入課程時間", inarea_in_datetime));
        newRowData->coldatalist.push_back(create_pdf_columndata("離開課程時間", inarea_out_datetime));
        newRowData->coldatalist.push_back(create_pdf_columndata("停留時間", inarea_total_time));

        pdfDataCourse->rowdatalist.push_back(newRowData);
    }

    std::ostringstream courseinfo;
    courseinfo<<"課程名稱:"<<course->title;

    if (baseDb != NULL)
        courseinfo<<" 在教室: "<<baseDb->get_area_name(course->resourceId)<<" 上課 ";
    courseinfo<<inarea_in_datetime<<" 開始上課";
    courseinfo<<inarea_out_datetime<<" 離開課程";
    courseinfo<<", 上課總時間:"<<inarea_total_time;

    pdfDataCourse->tableinfo = courseinfo.str();

    if (areaevent_in != NULL)
        delete areaevent_in;
    if (areaevent_out != NULL)
        delete areaevent_out;

    clear_AreaInOutEventStruct_list(merge_events);
    delete merge_events;

    clear_AreaInOutEventStruct_list(target_inout_events);
    delete target_inout_events;

    clear_Pair_AreaInOutEventStruct_list(pair_inout_events);
    delete pair_inout_events;

    return pdfDataCourse;
}

std::list<FiredEventStruct*> * WebUDPParserService::getAlertCancelActionEventList(std::list<FiredEventStruct*> * eventlist, std::string nodeid)
{
    std::list<FiredEventStruct*> * alertCancelEventsList = new std::list<FiredEventStruct*>();
    Json::Reader * reader = new Json::Reader();

    for (std::list<FiredEventStruct *>::iterator event = eventlist->begin(); event != eventlist->end(); event++)
    // for (auto event = eventlist->rbegin(); event != eventlist->rend(); event++)
    {
        // {"projectid":"1","eventid":"9999","nodeid":"Tag0003","action":"Cancel","areaid":"","issuetime":"0","locatorid":"Locator0001"}
        // {"projectid":"1","eventid":"9999","nodeid":"Tag0002","action":"Alert","areaid":"","issuetime":"0","locatorid":"Locator0001"}
        if ( (*event)->eventid == 9999 )
        {
            Json::Value JSONresult;

            bool parsingSuccessful = reader->parse( (*event)->eventjson.c_str(), JSONresult);

            if (!parsingSuccessful)
            {
                continue;
            }

            std::string curNodeid = JSONresult["nodeid"].asString();
            std::string action = JSONresult["action"].asString();

            if (action.compare("Alert") == 0 || (action.compare("CallOut") == 0))
            {
                if (nodeid.compare(curNodeid) != 0)
                {
                    continue;
                }

                FiredEventStruct * newEvent = copyFiredEventStruct((*event));
                alertCancelEventsList->push_back(newEvent);
            }

            if (action.compare("Cancel") == 0)
            {
                FiredEventStruct * newEvent = copyFiredEventStruct((*event));
                alertCancelEventsList->push_back(newEvent);
            }
        }
    }
    delete reader;

    return alertCancelEventsList;
}

std::list<Pair_FiredEventStruct*> * WebUDPParserService::getPairEvents(std::list<FiredEventStruct*> * eventlist)
{
    std::list<Pair_FiredEventStruct*> * pair_events = new std::list<Pair_FiredEventStruct*>();
    Pair_FiredEventStruct * pre_pairEvent = NULL;

    Json::Reader * reader = new Json::Reader();

    //for (auto it=eventlist->rbegin(); it!=eventlist->rend(); ++it)
    for (auto event = eventlist->begin(); event != eventlist->end(); event++)
    {
        // vilslog::printf ("WebUDPParserService::getPairEvents() eventjson[%s] empid[%s] comment[%s] datetime[%s]\n",
        //     (*event)->eventjson.c_str(), (*event)->empid.c_str(), (*event)->comment.c_str(), (*event)->datetime.c_str());

        time_t time_event = StringDateTimeTotime_t((*event)->datetime);

        if (pre_pairEvent != NULL)
        {
            // move to non-check event
            time_t time_pre_event = 0;
            if ( pre_pairEvent->event2 != NULL )
                time_pre_event = StringDateTimeTotime_t(pre_pairEvent->event2->datetime);
            else
            if ( pre_pairEvent->event1 != NULL )
                time_pre_event = StringDateTimeTotime_t(pre_pairEvent->event1->datetime);
            int diff_t = (int)(time_pre_event - time_event);
            if ( diff_t >= 0 )
            {
                // vilslog::printf ("WebUDPParserService::getPairEvents() \t pre_pairEvent != NULL diff_t[%d]\n", diff_t);
                continue;
            }
        }

        Json::Value JSONresult;

        bool parsingSuccessful = reader->parse( (*event)->eventjson.c_str(), JSONresult);

        if (!parsingSuccessful)
        {
            continue;
        }

        std::string action = JSONresult["action"].asString();
        if ((action.compare("Alert") == 0) || (action.compare("CallOut") == 0) )
        {
            // find Cancel event
            bool bStart = false;
            for (auto it = eventlist->begin(); it != eventlist->end(); it++)
            {
                if ( !bStart )
                {
                    if ( (*event)->firedeventid == (*it)->firedeventid)
                    {
                        bStart = true;
                    }
                    continue;
                }

                time_t time_it = StringDateTimeTotime_t((*it)->datetime);
                int diff_t = (int)(time_it - time_event);

                // vilslog::printf ("WebUDPParserService::getPairEvents() \t diff_t[%d]\n", diff_t);

                if ( diff_t <= 90 )
                {
                    Json::Value JSONresult_it;

                    bool parsingSuccessful_it = reader->parse( (*it)->eventjson.c_str(), JSONresult_it);

                    if (!parsingSuccessful_it)
                    {
                        continue;
                    }

                    std::string action = JSONresult_it["action"].asString();
                    if ((action.compare("Cancel") == 0))
                    {
                        // vilslog::printf ("WebUDPParserService::getPairEvents() \t it eventjson[%s] empid[%s] comment[%s] datetime[%s]\n",
                        //     (*it)->eventjson.c_str(), (*it)->empid.c_str(), (*it)->comment.c_str(), (*it)->datetime.c_str());

                        Pair_FiredEventStruct * pairEvent = new Pair_FiredEventStruct();
                        pairEvent->event1 = copyFiredEventStruct((*event));
                        pairEvent->event2 = copyFiredEventStruct((*it));
                        pairEvent->events = NULL;
                        pair_events->push_back(pairEvent);

                        pre_pairEvent = pairEvent;
                        break;
                    }
                    else
                    {
                        // vilslog::printf ("WebUDPParserService::getPairEvents() \t singel alert event eventjson[%s] empid[%s] comment[%s] datetime[%s]\n",
                        //     (*it)->eventjson.c_str(), (*it)->empid.c_str(), (*it)->comment.c_str(), (*it)->datetime.c_str());

                        // singel alert event
                        Pair_FiredEventStruct * singleEvent = new Pair_FiredEventStruct();
                        singleEvent->event1 = copyFiredEventStruct((*event));
                        singleEvent->event2 = NULL;
                        singleEvent->events = NULL;
                        pair_events->push_back(singleEvent);

                        pre_pairEvent = singleEvent;
                        break;
                    }
                }
                else
                {
                    // vilslog::printf ("WebUDPParserService::getPairEvents() \t singel alert event eventjson[%s] empid[%s] comment[%s] datetime[%s]\n",
                    //     (*it)->eventjson.c_str(), (*it)->empid.c_str(), (*it)->comment.c_str(), (*it)->datetime.c_str());

                    // singel alert event
                    Pair_FiredEventStruct * singleEvent = new Pair_FiredEventStruct();
                    singleEvent->event1 = copyFiredEventStruct((*event));
                    singleEvent->event2 = NULL;
                    singleEvent->events = NULL;
                    pair_events->push_back(singleEvent);

                    pre_pairEvent = singleEvent;
                    break;
                }
            }
        }
    }
    delete reader;

    return pair_events;
}

// void WebUDPParserService::sendPushNotifyUserToClient(UDPDataItem * item)
// {
//     char * buf = item->getMsg();
//
//     Json::Value result;
//     bool parsingSuccessful = parse_payload_JSON(buf, result);
//     int projectid = 1;
//     std::string account = "";
//     if ( parsingSuccessful )
//     {
//         projectid = getProjectId(result);
//
//         if ( result.isMember("accountid") )
//             account = result["accountid"].asString();
//     }
//
//     std::ostringstream sendstream;
//
//     if (account.length() > 0)
//     {
//
//         UserStruct * user = mDeviceManager->getUser(projectid, account);
//         if (user != NULL)
//         {
//             vilslog::printf("WebUDPParserService::sendPushNotifyUserToClient() projectid[%d] account[%s] groupid[%d]\n", projectid, account.c_str(), user->groupid);
//
//             if ( user->groupid == 1 )
//             {
//                 std::list<PushNotifyUserStruct*> * userList = mDeviceManager->get_PushNotifyUsers(projectid);
//
//                 sendstream<<"[";
//                 if ( userList != NULL )
//                 {
//                     for (std::list<PushNotifyUserStruct*>::iterator record = userList->begin(); record != userList->end(); record++)
//                     {
//                         if ( record != userList->begin() )
//                             sendstream<<",";
//
//                         sendstream<<"{\"user\":\""<<(*record)->account<<"\"";
//                         sendstream<<",\"topic\":\""<<(*record)->topic<<"\"";
//                         sendstream<<",\"functions\":\""<<(*record)->functions<<"\"";
//                         sendstream<<",\"success\":\"true\"}";
//                     }
//                     clear_PushNotifyUserStruct_list(userList);
//                     delete userList;
//                 }
//                 sendstream<<"]";
//             }
//             else
//             if ( user->groupid == 2 )
//             {
//                 PushNotifyUserStruct * record = mDeviceManager->getPushNotifyUser(projectid, account);
//                 if (record != NULL)
//                 {
//                     sendstream<<"[{\"user\":\""<<record->account<<"\"";
//                     sendstream<<",\"topic\":\""<<record->topic<<"\"";
//                     sendstream<<",\"functions\":\""<<record->functions<<"\"";
//                     sendstream<<",\"success\":\"true\"}]";
//
//                     delete record;
//                 }
//                 else
//                 {
//                      sendstream<<"[]";
//                 }
//             }
//             else
//             {
//                 sendstream<<"[{\"user\":\"";
//                 sendstream<<account;
//                 sendstream<<"\",\"success\":\"false\"}]";
//             }
//             delete user;
//         }
//         else
//         {
//             sendstream<<"[{\"user\":\"";
//             sendstream<<account;
//             sendstream<<"\",\"success\":\"false\"}]";
//         }
//     }
//     else
//     {
//         sendstream<<"[{\"user\":\"";
//         sendstream<<"NONE";
//         sendstream<<"\",\"success\":\"false\"}]";
//     }
//
//     std::string sendString = sendstream.str();
//
//     const char * sendBuf = sendString.c_str();
//     item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
// }

// dashboard data
void WebUDPParserService::sendDashboardActivityToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    std::ostringstream sendstream;

    //vilslog::printf("sendDashboardActivityToClient() buf[%s]\n", buf);

    int projectid = 1;
    int nodetype = NODE_TYPE_TAG;
    if ( parsingSuccessful )
    {
        projectid = getProjectId(result);

        if ( result.isMember("nodetype") )
            nodetype = result["nodetype"].asInt();

        //vilslog::printf("sendDashboardActivityToClient() projectid[%d] nodetype[%d]\n", projectid, nodetype);

        TrackDBAccess * trackDb = mDbManager->getTrackDB(projectid);
        if (trackDb != NULL)
        {
            DeviceActivityAliveCountStruct * data = trackDb->loadactivity(nodetype);

            sendstream<<"{";
            sendstream<<"\"devicecount\":\""<<data->devicecount<<"\",";
            sendstream<<"\"records\":\""<<data->aliveString<<"\",";
            sendstream<<"\"issueday\":\""<<data->issueday<<"\",";
            sendstream<<"\"issuetime\":\""<<data->issuetime<<"\"";
            sendstream<<"}";
            delete data;
        }
        else
        {
            sendstream<<"{}";
        }
    }
    else
    {
        sendstream<<"{}";
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendDashboardSystemInfoToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    std::ostringstream sendstream;

    //vilslog::printf("sendDashboardActivityToClient() buf[%s]\n", buf);

    int projectid = 1;
    if ( parsingSuccessful )
    {
        projectid = getProjectId(result);

        time_t t_cur = getCurrentTimeInSec();
        time_t t_diff = t_cur - mSystemInfo.updateTimestamp;
        if ( t_diff >= 60 )
        {
            getFreeDiskSpace("/", mSystemInfo);
            getSystemRunTime(mSystemInfo, mSystemStartTime);

            mSystemInfo.DBUsedSize = mDbManager->getDatabaseSize(projectid); // in MB
            mSystemInfo.updateTimestamp = getCurrentTimeInSec();
        }

        char msg[256];

        sendstream<<"{";
        sendstream<<"\"TotalDiskSize\":\""<<mSystemInfo.TotalDiskSize<<"\",";
        sendstream<<"\"TotalUsedSize\":\""<<mSystemInfo.TotalUsedSize<<"\",";
        sendstream<<"\"TotalFreeSize\":\""<<mSystemInfo.TotalFreeSize<<"\",";
        sendstream<<"\"DBUsedSize\":\""<<mSystemInfo.DBUsedSize<<"\",";

        snprintf(msg, 256, "%02d:%02d:%02d", mSystemInfo.SystemRunHour, mSystemInfo.SystemRunMin, mSystemInfo.SystemRunSec);

        sendstream<<"\"SystemRunTime\":\""<<msg<<"\",";

        snprintf(msg, 256, "%02d:%02d:%02d", mSystemInfo.CPURunHour, mSystemInfo.CPURunMin, mSystemInfo.CPURunSec);

        sendstream<<"\"CPURunTime\":\""<<msg<<"\",";
        sendstream<<"\"MemoryUsage\":\""<<mSystemInfo.MemoryUsage<<"\"";
        sendstream<<"}";
    }
    else
    {
        sendstream<<"{}";
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendTagBodyInfoToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    std::ostringstream sendstream;
    sendstream<<"[";

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        std::string targetid = "";
        if ( result.isMember("targetid") )
            targetid = result["targetid"].asString();

        bool getAllTag = true;
        if (targetid.length() > 0)
            getAllTag = false;

        int count = 0;
        std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);
        for (std::list<NodeStruct*>::iterator existed = tagNodeList->begin(); existed != tagNodeList->end(); existed++)
        {
            //if ( (*existed)->weight >= 0 || (*existed)->heartbeat >= 0 )
            if ( getAllTag || (*existed)->rangingTargetid.compare(targetid) == 0 )
            {
                float weightFloat = ((float)(*existed)->weight) / 100.0f;

                if(count > 0)
                    sendstream<<",";
                sendstream<<"{";
                sendstream<<"\"nodeid\":\""<<(*existed)->nodeid<<"\",";
                sendstream<<"\"nodename\":\""<<(*existed)->nodename<<"\",";
                sendstream<<"\"weight\":\""<<weightFloat<<"\",";
                sendstream<<"\"heartbeat\":\""<<(*existed)->heartbeat<<"\",";
                sendstream<<"\"weightTime\":\""<<time_tToString((*existed)->updateWeightTimestamp)<<"\",";
                sendstream<<"\"heartbeatTime\":\""<<time_tToString((*existed)->updateHeartBeatTimestamp)<<"\"";
                sendstream<<"}";
                count++;
            }
        }
    }

    sendstream<<"]\r\n";
    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendTagBodyInfoRecordsToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    std::ostringstream sendstream;
    sendstream<<"[";

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        std::string targetid = "";
        if ( result.isMember("targetid") )
            targetid = result["targetid"].asString();

        if (targetid.length() > 0)
        {
            MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
            if (baseDb != NULL)
            {
                std::list<BodyInfoStruct*> * WeightinfoList = baseDb->get_bodyinfo_Weight(targetid.c_str(), 10);
                std::list<BodyInfoStruct*> * HeartBeatinfoList = baseDb->get_bodyinfo_HeartBeat(targetid.c_str(), 10);

                // vilslog::printf("sendTagBodyInfoRecordsToClient() projectid[%d] WeightinfoList->size()[%d] HeartBeatinfoList->size()[%d]\n",
                //     projectid, (int)WeightinfoList->size(), (int)HeartBeatinfoList->size());

                sendstream<<"{\"weight\":[";

                int count = 0;
                for (std::list<BodyInfoStruct*>::iterator info = WeightinfoList->begin(); info != WeightinfoList->end(); info++)
                {
                    if(count > 0)
                        sendstream<<",";
                    sendstream<<"{";
                    sendstream<<"\"weight\":\""<<(*info)->weight<<"\",";
                    sendstream<<"\"datetime\":\""<<(*info)->datetime<<"\"";
                    sendstream<<"}";
                    count++;
                }
                sendstream<<"]},";
                sendstream<<"{\"heartbeat\":[";

                count = 0;
                for (std::list<BodyInfoStruct*>::iterator info = HeartBeatinfoList->begin(); info != HeartBeatinfoList->end(); info++)
                {
                    if(count > 0)
                        sendstream<<",";
                    sendstream<<"{";
                    sendstream<<"\"heartbeat\":\""<<(*info)->heartbeat<<"\",";
                    sendstream<<"\"datetime\":\""<<(*info)->datetime<<"\"";
                    sendstream<<"}";
                    count++;
                }

                sendstream<<"]}";

                clear_BodyInfoStruct_list(WeightinfoList);
                clear_BodyInfoStruct_list(HeartBeatinfoList);

                delete WeightinfoList;
                delete HeartBeatinfoList;
            }
        }
    }

    sendstream<<"]\r\n";
    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendLocatorBodyInfoToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    std::ostringstream sendstream;
    sendstream<<"[";

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        std::string targetid = "";
        if ( result.isMember("targetid") )
            targetid = result["targetid"].asString();

        bool getAllLocator = true;
        if (targetid.length() > 0)
            getAllLocator = false;

        int count = 0;
        std::list<NodeStruct*> * locatorNodeList = mDeviceManager->get_Locator_list(projectid);
        for (std::list<NodeStruct*>::iterator existed = locatorNodeList->begin(); existed != locatorNodeList->end(); existed++)
        {
            //if ( (*existed)->weight >= 0 || (*existed)->heartbeat >= 0 )
            if ( (getAllLocator && (*existed)->tafid == 3) || targetid.compare((*existed)->nodeid) == 0 )
            {
                float weightFloat = ((float)(*existed)->weight) / 100.0f;

                if(count > 0)
                    sendstream<<",";
                sendstream<<"{";
                sendstream<<"\"nodeid\":\""<<(*existed)->nodeid<<"\",";
                sendstream<<"\"nodename\":\""<<(*existed)->nodename<<"\",";
                sendstream<<"\"weight\":\""<<weightFloat<<"\",";
                sendstream<<"\"heartbeat\":\""<<(*existed)->heartbeat<<"\",";
                sendstream<<"\"weightTime\":\""<<time_tToString((*existed)->updateWeightTimestamp)<<"\",";
                sendstream<<"\"heartbeatTime\":\""<<time_tToString((*existed)->updateHeartBeatTimestamp)<<"\"";
                sendstream<<"}";
                count++;
            }
        }
    }

    sendstream<<"]\r\n";
    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendTagPrjInfoToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    std::ostringstream sendstream;

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        int groupid = 3;

        std::string maingroups = "";
        if ( result.isMember("maingroups") )
            maingroups = result["maingroups"].asString();
        if ( result.isMember("groupid") )
            groupid = convert_string_to_int(result["groupid"].asString());

        std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');

        sendstream<<"{\""<<projectid<<"\":[";

        int nodeCnt = 0;
        std::list<NodeStruct*> * nodeList = mDeviceManager->get_Tag_list(projectid);

        for (std::list<NodeStruct*>::iterator existed = nodeList->begin(); existed != nodeList->end(); existed++)
        {
            for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
            {
                int maingroupid = convert_string_to_int(*maingroup);

                if ((*existed)->maingroupid == maingroupid || groupid == 1 || groupid == 2)
                {
                    if(nodeCnt > 0)
                        sendstream<<",";
                    sendstream<<"{\"nodeid\":\""<<(*existed)->nodeid<<"\"}";
                    nodeCnt++;
                    break;
                }
            }
        }
        sendstream<<"]}\r\n";

        maingroupList->clear();
        delete maingroupList;
    }
    else
    {
        sendstream<<"{";
        std::list<int> * prjidList = mDbManager->getAllProjectConfig();
        int prjCnt = 0;
        for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
        {
            int projectid = (*prj);
            if(prjCnt > 0)
                sendstream<<",";

            sendstream<<"\""<<projectid<<"\":[";

            int nodeCnt = 0;
            std::list<NodeStruct*> * nodeList = mDeviceManager->get_Tag_list(projectid);
            for (std::list<NodeStruct*>::iterator existed = nodeList->begin(); existed != nodeList->end(); existed++)
            {
                if(nodeCnt > 0)
                    sendstream<<",";
                sendstream<<"{\"nodeid\":\""<<(*existed)->nodeid<<"\"}";
                nodeCnt++;
            }

            sendstream<<"]";
            prjCnt++;
        }
        sendstream<<"}\r\n";

        prjidList->clear();
        delete prjidList;
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendAnchorPrjInfoToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    std::ostringstream sendstream;

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        int groupid = 3;

        std::string maingroups = "";
        if ( result.isMember("maingroups") )
            maingroups = result["maingroups"].asString();
        if ( result.isMember("groupid") )
            groupid = convert_string_to_int(result["groupid"].asString());

        std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');

        sendstream<<"{\""<<projectid<<"\":[";

        int nodeCnt = 0;
        std::list<NodeStruct*> * nodeList = mDeviceManager->get_Anchor_list(projectid);

        for (std::list<NodeStruct*>::iterator existed = nodeList->begin(); existed != nodeList->end(); existed++)
        {
            for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
            {
                int maingroupid = convert_string_to_int(*maingroup);

                if ( (*existed)->maingroupid == maingroupid || groupid == 1 || groupid == 2)
                {
                    if(nodeCnt > 0)
                        sendstream<<",";
                    sendstream<<"{\"nodeid\":\""<<(*existed)->nodeid<<"\"}";
                    nodeCnt++;
                    break;
                }
            }
        }
        sendstream<<"]}\r\n";

        maingroupList->clear();
        delete maingroupList;
    }
    else
    {
        sendstream<<"{";

        std::list<int> * prjidList = mDbManager->getAllProjectConfig();
        int prjCnt = 0;
        for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
        {
            int projectid = (*prj);
            if(prjCnt > 0)
                sendstream<<",";

            sendstream<<"\""<<projectid<<"\":[";

            int nodeCnt = 0;
            std::list<NodeStruct*> * nodeList = mDeviceManager->get_Anchor_list(projectid);
            for (std::list<NodeStruct*>::iterator existed = nodeList->begin(); existed != nodeList->end(); existed++)
            {
                if(nodeCnt > 0)
                    sendstream<<",";
                sendstream<<"{\"nodeid\":\""<<(*existed)->nodeid<<"\"}";
                nodeCnt++;
            }

            sendstream<<"]";
            prjCnt++;
        }

        sendstream<<"}\r\n";

        prjidList->clear();
        delete prjidList;
    }

    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendCoordPrjInfoToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    std::ostringstream sendstream;

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        int groupid = 3;

        std::string maingroups = "";
        if ( result.isMember("maingroups") )
            maingroups = result["maingroups"].asString();
        if ( result.isMember("groupid") )
            groupid = convert_string_to_int(result["groupid"].asString());

        std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');

        sendstream<<"{\""<<projectid<<"\":[";

        int nodeCnt = 0;
        std::list<NodeStruct*> * nodeList = mDeviceManager->get_Coordinator_list(projectid);
        for (std::list<NodeStruct*>::iterator existed = nodeList->begin(); existed != nodeList->end(); existed++)
        {
            for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
            {
                int maingroupid = convert_string_to_int(*maingroup);

                if ( (*existed)->maingroupid == maingroupid || groupid == 1 || groupid == 2)
                {
                    if(nodeCnt > 0)
                        sendstream<<",";
                    sendstream<<"{\"nodeid\":\""<<(*existed)->nodeid<<"\"}";
                    nodeCnt++;
                    break;
                }
            }
        }
        sendstream<<"]}\r\n";

        maingroupList->clear();
        delete maingroupList;
    }
    else
    {
        sendstream<<"{";

        std::list<int> * prjidList = mDbManager->getAllProjectConfig();
        int prjCnt = 0;
        for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
        {
            int projectid = (*prj);
            if(prjCnt > 0)
                sendstream<<",";

            sendstream<<"\""<<projectid<<"\":[";

            int nodeCnt = 0;
            std::list<NodeStruct*> * nodeList = mDeviceManager->get_Coordinator_list(projectid);
            for (std::list<NodeStruct*>::iterator existed = nodeList->begin(); existed != nodeList->end(); existed++)
            {
                if(nodeCnt > 0)
                    sendstream<<",";
                sendstream<<"{\"nodeid\":\""<<(*existed)->nodeid<<"\"}";
                nodeCnt++;
            }

            sendstream<<"]";
            prjCnt++;
        }

        sendstream<<"}\r\n";

        prjidList->clear();
        delete prjidList;
    }
    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendLocatorPrjInfoToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    std::ostringstream sendstream;

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        int groupid = 3;

        std::string maingroups = "";
        if ( result.isMember("maingroups") )
            maingroups = result["maingroups"].asString();
        if ( result.isMember("groupid") )
            groupid = convert_string_to_int(result["groupid"].asString());

        std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');

        sendstream<<"{\""<<projectid<<"\":[";

        int nodeCnt = 0;
        std::list<NodeStruct*> * nodeList = mDeviceManager->get_Locator_list(projectid);
        for (std::list<NodeStruct*>::iterator existed = nodeList->begin(); existed != nodeList->end(); existed++)
        {
            for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
            {
                int maingroupid = convert_string_to_int(*maingroup);

                if ( (*existed)->maingroupid == maingroupid || groupid == 1 || groupid == 2)
                {
                    if(nodeCnt > 0)
                        sendstream<<",";
                    sendstream<<"{\"nodeid\":\""<<(*existed)->nodeid<<"\"}";
                    nodeCnt++;
                    break;
                }
            }
        }
        sendstream<<"]}\r\n";

        maingroupList->clear();
        delete maingroupList;
    }
    else
    {
        sendstream<<"{";

        std::list<int> * prjidList = mDbManager->getAllProjectConfig();
        int prjCnt = 0;
        for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
        {
            int projectid = (*prj);
            if(prjCnt > 0)
                sendstream<<",";

            sendstream<<"\""<<projectid<<"\":[";

            int nodeCnt = 0;
            std::list<NodeStruct*> * nodeList = mDeviceManager->get_Locator_list(projectid);
            for (std::list<NodeStruct*>::iterator existed = nodeList->begin(); existed != nodeList->end(); existed++)
            {
                if(nodeCnt > 0)
                    sendstream<<",";
                sendstream<<"{\"nodeid\":\""<<(*existed)->nodeid<<"\"}";
                nodeCnt++;
            }

            sendstream<<"]";
            prjCnt++;
        }

        sendstream<<"}\r\n";

        prjidList->clear();
        delete prjidList;
    }
    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::sendGatewayPrjInfoToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    std::ostringstream sendstream;

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        int groupid = 3;

        std::string maingroups = "";
        if ( result.isMember("maingroups") )
            maingroups = result["maingroups"].asString();

        std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');

        sendstream<<"{\""<<projectid<<"\":[";

        int nodeCnt = 0;
        std::list<LoRaGatewayInfoStruct*> * nodeList = mDeviceManager->get_LoRaGateway_list(projectid);
        for (std::list<LoRaGatewayInfoStruct*>::iterator existed = nodeList->begin(); existed != nodeList->end(); existed++)
        {
            for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
            {
                int maingroupid = convert_string_to_int(*maingroup);

                if ( (*existed)->maingroupid == maingroupid || groupid == 1 || groupid == 2)
                {
                    if(nodeCnt > 0)
                        sendstream<<",";
                    sendstream<<"{\"gwid\":\""<<(*existed)->gwid<<"\"}";
                    nodeCnt++;
                    break;
                }
            }
        }
        sendstream<<"]}\r\n";

        maingroupList->clear();
        delete maingroupList;
    }
    else
    {

        sendstream<<"{";

        std::list<int> * prjidList = mDbManager->getAllProjectConfig();
        int prjCnt = 0;
        for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
        {
            int projectid = (*prj);
            if(prjCnt > 0)
                sendstream<<",";

            sendstream<<"\""<<projectid<<"\":[";

            int nodeCnt = 0;
            std::list<LoRaGatewayInfoStruct*> * nodeList = mDeviceManager->get_LoRaGateway_list(projectid);
            for (std::list<LoRaGatewayInfoStruct*>::iterator existed = nodeList->begin(); existed != nodeList->end(); existed++)
            {
                if(nodeCnt > 0)
                    sendstream<<",";
                sendstream<<"{\"gwid\":\""<<(*existed)->gwid<<"\"}";
                nodeCnt++;
            }

            sendstream<<"]";
            prjCnt++;
        }

        sendstream<<"}\r\n";

        prjidList->clear();
        delete prjidList;
    }
    std::string sendString = sendstream.str();

    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::updatenodeprj(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        std::string macaddress = "";
        int target_projectid = 0;
        std::string type = "";

        int projectid = getProjectId(result);

        if ( result.isMember("mac") )
        {
            macaddress = result["mac"].asString();
        }
        if ( result.isMember("target_projectid") )
        {
            if ( result["target_projectid"].isInt() )
                target_projectid = result["target_projectid"].asInt();
            else
                target_projectid = atoi(result["target_projectid"].asCString());
        }
        if ( result.isMember("type") )
        {
            type = result["type"].asString();
        }

        vilslog::printf("WebUDPParserService::updatenodeprj() macaddress[%s] type[%s] target_projectid[%d]\n",
            macaddress.c_str(), type.c_str(), target_projectid);

        //
        // copy node from projectid to target_projectid
        //
        if ( type.compare("anchor") == 0 )
        {
            NodeStruct * node = mDeviceManager->get_Anchor_node_ByMacaddress(projectid, macaddress.c_str());
            mDeviceManager->add_Anchor_node(target_projectid, node);
        }
        else
        if ( type.compare("tag") == 0 )
        {
            NodeStruct * node = mDeviceManager->get_Tag_node_ByMacaddress(projectid, macaddress.c_str());
            mDeviceManager->add_Tag_node(target_projectid, node);
        }
        else
        if ( type.compare("coord") == 0 )
        {
            NodeStruct * node = mDeviceManager->get_Cooridinator_node_ByMacaddress(projectid, macaddress.c_str());
            mDeviceManager->add_Cooridinator_node(target_projectid, node);
        }
        else
        if ( type.compare("locator") == 0 )
        {
            NodeStruct * node = mDeviceManager->get_Locator_node_ByMacaddress(projectid, macaddress.c_str());
            mDeviceManager->add_Locator_node(target_projectid, node);
        }
        else
        if ( type.compare("gateway") == 0 )
        {
            LoRaGatewayInfoStruct * node = mDeviceManager->getLoRaGateway(projectid, macaddress);
            mDeviceManager->add_LoRaGateway_node(target_projectid, node);
        }



        // if (type.compare("locator") == 0)
        //     mDeviceManager->setLocatorButtonType(projectid, macaddress.c_str(), buttontype);
        //
        // WorkItem * item = new WorkItem("updateLocatorButtontype", projectid, macaddress, buttontype, 0);
        // mMySQLJobQueue.add(item);
    }
}

void WebUDPParserService::updatenodeuser(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        std::string macaddress = "";
        int userid = 0;
        std::string type = "";

        int projectid = getProjectId(result);

        if ( result.isMember("mac") )
        {
            macaddress = result["mac"].asString();
        }
        if ( result.isMember("userid") )
        {
            if ( result["userid"].isInt() )
                userid = result["userid"].asInt();
            else
                userid = atoi(result["userid"].asCString());
        }
        if ( result.isMember("type") )
        {
            type = result["type"].asString();
        }

        // vilslog::printf("WebUDPParserService::updatenodeuser() macaddress[%s] type[%s] userid[%d]\n",
        //     macaddress.c_str(), type.c_str(), userid);

        //
        // copy node from projectid to target_projectid
        //
        if ( type.compare("tag") == 0 )
        {
            mDeviceManager->setTagUser(projectid, macaddress.c_str(), userid);

            WorkItem * item = new WorkItem("adduseruwbcard", projectid);
            item->setUserId(userid);
            item->setUWBCards(macaddress);
            mMySQLJobQueue.add(item);
        }
        else
        if ( type.compare("locator") == 0 )
        {
            mDeviceManager->setLocatorUser(projectid, macaddress.c_str(), userid);
        }
    }
}

void WebUDPParserService::updateaoanormalvector(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        std::string macaddress = "";
        int posX = 0;
        int posY = 0;
        int posZ = 0;
        int roll = 0;

        int projectid = getProjectId(result);

        if ( result.isMember("mac") )
        {
            macaddress = result["mac"].asString();
        }
        if ( result.isMember("posX") )
            posX = atoi(result["posX"].asCString());
        if ( result.isMember("posY") )
            posY = atoi(result["posY"].asCString());
        if ( result.isMember("posZ") )
            posZ = atoi(result["posZ"].asCString());
        if ( result.isMember("roll") )
            roll = atoi(result["roll"].asCString());

        vilslog::printf("WebUDPParserService::updateaoanormalvector() macaddress[%s] posX[%d] posY[%d] posZ[%d] roll[%d]\n",
            macaddress.c_str(), posX, posY, posZ, roll);

        AoANodeStruct * aoaNode = mDeviceManager->get_AoA_node(projectid, macaddress.c_str());
        if (aoaNode == NULL)
        {
            vilslog::printf("WebUDPParserService::updateaoanormalvector() ERROR!!!! Can not find this AOA node!\n");
            return;
        }

        vilslog::printf("WebUDPParserService::updateaoanormalvector() aoaNode nodeid[%s] posX[%d] posY[%d] posZ[%d]\n",
            aoaNode->nodeid, aoaNode->posX, aoaNode->posY, aoaNode->posZ);

        int arrowX = posX - aoaNode->posX;
        int arrowY = posY - aoaNode->posY;
        int arrowZ = posZ - aoaNode->posZ;

        vilslog::printf("WebUDPParserService::updateaoanormalvector() arrowX[%d] arrowY[%d] arrowZ[%d]\n",
           arrowX, arrowY, arrowZ);

        double heading = atan2((double)arrowY, (double)arrowX);
        double angle = rad_to_angle(heading);

        vilslog::printf("WebUDPParserService::updateaoanormalvector() heading[%f] angle[%f]\n", heading, angle);

        double lenOnXY = sqrt(arrowX*arrowX + arrowY*arrowY);
        double pitchRaw = atan2(arrowZ, lenOnXY);

        int pitch = (int)rad_to_angle(pitchRaw);
        int yaw = (int)angle;

        vilslog::printf("WebUDPParserService::updateaoanormalvector() pitch[%d] roll[%d] yaw[%d]\n",
            pitch, roll, yaw);

        //mDeviceManager->setAoAInfoS(projectid, macaddress.c_str(), pitch, roll, yaw);
        aoaNode->pitch = pitch;
        aoaNode->roll = roll;
        aoaNode->yaw = yaw;

        WorkItem * item = new WorkItem("updateAoANodeInfo", projectid, aoaNode);
        mMySQLJobQueue.add(item);
    }
}

void WebUDPParserService::resetm4(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);
    if ( parsingSuccessful )
    {
        std::string macaddress = "";
        int projectid = getProjectId(result);

        if ( result.isMember("mac") )
            macaddress = result["mac"].asString();

        DataItem * senditem = new DataItem("resetm4", projectid, macaddress.c_str(), NODE_TYPE_COORD);
        mMainJobQueue.add(senditem);
    }
}

void WebUDPParserService::resetm0(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);
    if ( parsingSuccessful )
    {
        std::string macaddress = "";
        int projectid = getProjectId(result);

        if ( result.isMember("mac") )
            macaddress = result["mac"].asString();

        DataItem * senditem = new DataItem("resetm0", projectid, macaddress.c_str(), NODE_TYPE_COORD);
        mMainJobQueue.add(senditem);
    }
}

void WebUDPParserService::blacklist(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        std::string macaddress = "";
        std::string list = "";
        int projectid = getProjectId(result);
        if ( result.isMember("mac") )
        {
            macaddress = result["mac"].asString();
        }
        if ( result.isMember("list") )
        {
            list = result["list"].asString();
        }

        replaceAll(list, ",", ":");

        vilslog::printf("WebUDPParserService::blacklist() projectid[%d] macaddress[%s] list[%s]\n", projectid, macaddress.c_str(), list.c_str());

        mDeviceManager->setCoordinatorBlacklist(projectid, macaddress.c_str(), list.c_str());

        DataItem * senditem = new DataItem("blacklist", projectid, macaddress.c_str(), list);
        mMainJobQueue.add(senditem);

        WorkItem * item = new WorkItem("updateblacklist", projectid, macaddress, list);
        mMySQLJobQueue.add(item);
    }
}

void WebUDPParserService::whitelist(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        std::string macaddress = "";
        std::string list = "";
        int projectid = getProjectId(result);
        if ( result.isMember("mac") )
        {
            macaddress = result["mac"].asString();
        }
        if ( result.isMember("list") )
        {
            list = result["list"].asString();
        }

        replaceAll(list, ",", ":");

        vilslog::printf("WebUDPParserService::whitelist() macaddress[%s] list[%s]\n", macaddress.c_str(), list.c_str());

        mDeviceManager->setCoordinatorWhitelist(projectid, macaddress.c_str(), list.c_str());

        DataItem * senditem = new DataItem("whitelist", projectid, macaddress.c_str(), list);
        mMainJobQueue.add(senditem);

        WorkItem * item = new WorkItem("updatewhitelist", projectid, macaddress, list);
        mMySQLJobQueue.add(item);
    }
}

void WebUDPParserService::clearblacklist(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        std::string macaddress = "";
        int projectid = getProjectId(result);
        if ( result.isMember("mac") )
        {
            macaddress = result["mac"].asString();
        }

        //vilslog::printf("WebUDPParserService::clearblacklist() macaddress[%s]\n", macaddress.c_str());

        mDeviceManager->setCoordinatorBlacklist(projectid, macaddress.c_str(), "");

        DataItem * senditem = new DataItem("blacklist", projectid, macaddress.c_str(), "");
        mMainJobQueue.add(senditem);

        WorkItem * item = new WorkItem("updateblacklist", projectid, macaddress, "");
        mMySQLJobQueue.add(item);
    }
}

void WebUDPParserService::clearwhitelist(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        std::string macaddress = "";
        int projectid = getProjectId(result);
        if ( result.isMember("mac") )
        {
            macaddress = result["mac"].asString();
        }

        //vilslog::printf("WebUDPParserService::clearwhitelist() macaddress[%s]\n", macaddress.c_str());

        mDeviceManager->setCoordinatorWhitelist(projectid, macaddress.c_str(), "");

        DataItem * senditem = new DataItem("whitelist", projectid, macaddress.c_str(), "");
        mMainJobQueue.add(senditem);

        WorkItem * item = new WorkItem("updatewhitelist", projectid, macaddress, "");
        mMySQLJobQueue.add(item);
    }
}

void WebUDPParserService::joinRangingRange(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        std::string macaddress = "";
        std::string joinRangingRange = "";
        int i_joinRangingRange = 0;
        int i_joinNotRangingRange = 0;
        int projectid = getProjectId(result);
        if ( result.isMember("mac") )
        {
            macaddress = result["mac"].asString();
        }
        if ( result.isMember("joinRangingRange") )
        {
            joinRangingRange = result["joinRangingRange"].asString();
            i_joinRangingRange = atoi(joinRangingRange.c_str());
        }

        vilslog::printf("WebUDPParserService::joinRangingRange() macaddress[%s] i_joinRangingRange[%d]\n", macaddress.c_str(), i_joinRangingRange);

        mDeviceManager->setCoordinatorjoinRangingRange(projectid, macaddress.c_str(), i_joinRangingRange);

        i_joinNotRangingRange = mDeviceManager->getCoordinatorjoinNotRangingRange(projectid, macaddress.c_str());

        DataItem * senditem = new DataItem("setvalidrang", projectid, macaddress.c_str(), mDeviceManager->getCoordinatorIP(projectid, macaddress.c_str()), i_joinRangingRange, i_joinNotRangingRange);
        mMainJobQueue.add(senditem);

        WorkItem * item = new WorkItem("updatejoinRangingRange", projectid, macaddress, i_joinRangingRange);
        mMySQLJobQueue.add(item);
    }
}

void WebUDPParserService::joinNotRangingRange(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        std::string macaddress = "";
        std::string joinNotRangingRange = "";
        int i_joinRangingRange = 0;
        int i_joinNotRangingRange = 0;
        int projectid = getProjectId(result);
        if ( result.isMember("mac") )
        {
            macaddress = result["mac"].asString();
        }
        if ( result.isMember("joinNotRangingRange") )
        {
            joinNotRangingRange = result["joinNotRangingRange"].asString();
            i_joinNotRangingRange = atoi(joinNotRangingRange.c_str());
        }

        vilslog::printf("WebUDPParserService::joinNotRangingRange() macaddress[%s] i_joinNotRangingRange[%d]\n", macaddress.c_str(), i_joinRangingRange);

        mDeviceManager->setCoordinatorjoinNotRangingRange(projectid, macaddress.c_str(), i_joinNotRangingRange);

        i_joinRangingRange = mDeviceManager->getCoordinatorjoinRangingRange(projectid, macaddress.c_str());

        DataItem * senditem = new DataItem("setvalidrang", projectid, macaddress.c_str(), mDeviceManager->getCoordinatorIP(projectid, macaddress.c_str()), i_joinRangingRange, i_joinNotRangingRange);
        mMainJobQueue.add(senditem);

        WorkItem * item = new WorkItem("updatejoinNotRangingRange", projectid, macaddress, i_joinRangingRange);
        mMySQLJobQueue.add(item);
    }
}

void WebUDPParserService::nodeResident(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        std::string macaddress = "";
        std::string serno = "";
        std::string type = "";

        int projectid = getProjectId(result);

        if ( result.isMember("mac") )
        {
            macaddress = result["mac"].asString();
        }
        if ( result.isMember("serno") )
        {
            serno = result["serno"].asString();
        }
        if ( result.isMember("type") )
        {
            type = result["type"].asString();
        }

        vilslog::printf("WebUDPParserService::nodeResident() macaddress[%s] serno[%s]\n", macaddress.c_str(), serno.c_str());

        if (type.compare("tag") == 0)
            mDeviceManager->setTagResident(projectid, macaddress.c_str(), serno);
        else
        if (type.compare("locator") == 0)
            mDeviceManager->setLocatorResident(projectid, macaddress.c_str(), serno);

        WorkItem * item = new WorkItem("updateNodeResident", projectid, macaddress, serno, 0);
        mMySQLJobQueue.add(item);
    }
}

void WebUDPParserService::nodeEmployee(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        std::string macaddress = "";
        std::string empid = "";
        std::string type = "";

        int projectid = getProjectId(result);

        if ( result.isMember("mac") )
        {
            macaddress = result["mac"].asString();
        }
        if ( result.isMember("empid") )
        {
            empid = result["empid"].asString();
        }
        if ( result.isMember("type") )
        {
            type = result["type"].asString();
        }

        vilslog::printf("WebUDPParserService::nodeEmployee() macaddress[%s] empid[%s]\n", macaddress.c_str(), empid.c_str());

        if (type.compare("tag") == 0)
            mDeviceManager->setTagEmployee(projectid, macaddress.c_str(), empid);
        else
        if (type.compare("locator") == 0)
            mDeviceManager->setLocatorEmployee(projectid, macaddress.c_str(), empid);

        WorkItem * item = new WorkItem("updateNodeEmployee", projectid, macaddress, empid, 0);
        mMySQLJobQueue.add(item);
    }
}

void WebUDPParserService::locatorButtontype(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        std::string macaddress = "";
        std::string buttontype = "";
        std::string type = "";

        int projectid = getProjectId(result);

        if ( result.isMember("mac") )
        {
            macaddress = result["mac"].asString();
        }
        if ( result.isMember("buttontype") )
        {
            buttontype = result["buttontype"].asString();
        }
        if ( result.isMember("type") )
        {
            type = result["type"].asString();
        }

        vilslog::printf("WebUDPParserService::locatorButtontype() macaddress[%s] buttontype[%s]\n", macaddress.c_str(), buttontype.c_str());

        if (type.compare("locator") == 0)
            mDeviceManager->setLocatorButtonType(projectid, macaddress.c_str(), buttontype);

        WorkItem * item = new WorkItem("updateLocatorButtontype", projectid, macaddress, buttontype, 0);
        mMySQLJobQueue.add(item);
    }
}

void WebUDPParserService::locatorSensorAlert(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        std::string macaddress = "";
        std::string sensoralert = "";
        std::string type = "";

        int projectid = getProjectId(result);

        if ( result.isMember("mac") )
        {
            macaddress = result["mac"].asString();
        }
        if ( result.isMember("sensoralert") )
        {
            sensoralert = result["sensoralert"].asString();
        }
        if ( result.isMember("type") )
        {
            type = result["type"].asString();
        }

        vilslog::printf("WebUDPParserService::locatorSensorAlert() macaddress[%s] sensoralert[%s]\n", macaddress.c_str(), sensoralert.c_str());

        if (type.compare("locator") == 0)
            mDeviceManager->setLocatorInfoSensorAlert(projectid, macaddress.c_str(), atoi(sensoralert.c_str()));

        WorkItem * item = new WorkItem("updateLocatorSensorAlert", projectid, macaddress, sensoralert, 0);
        mMySQLJobQueue.add(item);
    }
}

void WebUDPParserService::checkaccountlogin(UDPDataItem * item)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(item->getMsg(), result);
    int projectid = 1;

    if ( parsingSuccessful )
    {
        std::string account = "";
        std::string password = "";
        std::string userid = "";

        if ( result.isMember("account") )
        {
            account = result["account"].asString();
        }
        if ( result.isMember("password") )
        {
            password = result["password"].asString();
        }
        if ( result.isMember("userid") )
        {
            userid = result["userid"].asString();
        }

        std::ostringstream sendstream;
        UserStruct * UserData = new UserStruct();

        if ( mDeviceManager->checkValidUser(projectid, account.c_str(), password.c_str(), userid.c_str(), UserData) )
        {

            // UserStruct.type 0: vilscenter user 1:已註冊, 2:已購買點名裝置
            int usertype = UserData->type;

            if ( UserData->groupid == 5 || UserData->groupid == 6 )
            {
                // 主集合管理員, 子集合管理員 可以共享點名器
                bool hasNFCLocator = mDeviceManager->checkNFCLocatorMainGroup(projectid, UserData->maingroups);
                if ( hasNFCLocator)
                {
                    usertype = 2;
                }
                else
                {
                    bool hasSensorDevice = mDeviceManager->checkSensorDeviceMainGroup(projectid, UserData->maingroups);
                    if ( hasSensorDevice )
                    {
                        usertype = 2;
                    }
                }
            }

            std::string maingroups = convertListToString(UserData->maingroups);

            vilslog::printf("WebUDPParserService::checkaccountlogin() account[%s] userid[%s] password[%s] maingroups[%s] groupid[%d] usertype[%d]\n",
                account.c_str(), userid.c_str(), password.c_str(), maingroups.c_str(), UserData->groupid, usertype);

            sendstream<<"{\"valid\":\"true\"";
            sendstream<<",\"name\":\""<<UserData->name<<"\"";
            sendstream<<",\"groupid\":\""<<UserData->groupid<<"\"";
            sendstream<<",\"type\":\""<<usertype<<"\"";
            sendstream<<",\"userid\":\""<<UserData->userid<<"\"";
            sendstream<<",\"maingroups\":\""<<maingroups<<"\"";
            sendstream<<",\"subgroups\":\""<<convertListToString(UserData->subgroups)<<"\"";
            if (UserData->empid.length() > 0 )
                sendstream<<",\"empid\":\""<<UserData->empid<<"\"";
            if (UserData->unit.length() > 0 )
                sendstream<<",\"unit\":\""<<UserData->unit<<"\"";
            if (UserData->title.length() > 0 )
                sendstream<<",\"title\":\""<<UserData->title<<"\"";
            sendstream<<",\"gender\":\""<<UserData->gender<<"\"";
            sendstream<<"}\r\n";
        }
        else
        {

            vilslog::printf("WebUDPParserService::checkaccountlogin() account[%s] userid[%s] password[%s]\n",
                account.c_str(), userid.c_str(), password.c_str());

            sendstream<<"{\"valid\":\"false\"}\r\n";

            DataItem * senditem = new DataItem("loginfailed", projectid);
            mMainJobQueue.add(senditem);
        }

        clear_UserStruct(UserData);
        delete UserData;

        std::string sendString = sendstream.str();

        const char * sendBuf = sendString.c_str();

        //vilslog::printf("WebUDPParserService::checkaccountlogin() sendBuf[%s]\n", sendBuf);

        item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
    }
}

void WebUDPParserService::checkaccountloginkey(UDPDataItem * item)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(item->getMsg(), result);
    int projectid = 1;

    if ( parsingSuccessful )
    {
        std::string account = "";
        std::string key = "";
        std::string token = "";

        if ( result.isMember("account") )
        {
            account = result["account"].asString();
        }
        if ( result.isMember("key") )
        {
            key = result["key"].asString();
        }
        // if ( result.isMember("token") )
        // {
        //     token = result["token"].asString();
        // }
        // vilslog::printf("WebUDPParserService::checkaccountloginkey() account[%s] key[%s]\n",
        //     account.c_str(), key.c_str());

        std::ostringstream sendstream;

        int userid = mDeviceManager->checkValidAccountLogin(projectid, account, key);

        if ( userid > 0)
        {
            UserStruct * UserData = mDeviceManager->getUser(projectid, userid);

            sendstream<<"{\"valid\":\"true\"";
            sendstream<<",\"name\":\""<<UserData->name<<"\"";
            sendstream<<",\"groupid\":\""<<UserData->groupid<<"\"";
            sendstream<<",\"type\":\""<<UserData->type<<"\"";
            sendstream<<",\"userid\":\""<<UserData->userid<<"\"";
            sendstream<<",\"maingroups\":\""<<convertListToString(UserData->maingroups)<<"\"";
            sendstream<<",\"subgroups\":\""<<convertListToString(UserData->subgroups)<<"\"";
            // sendstream<<"{\"valid\":\"true\"";
            // sendstream<<",\"name\":\""<<UserData->name<<"\"";
            // sendstream<<",\"groupid\":\""<<UserData->groupid<<"\"";
            sendstream<<"}\r\n";

            // clear_UserStruct(UserData);
            // delete UserData;
        }
        else
        {
            sendstream<<"{\"valid\":\"false\"}\r\n";

            DataItem * senditem = new DataItem("loginfailed", projectid);
            mMainJobQueue.add(senditem);
        }

        std::string sendString = sendstream.str();

        const char * sendBuf = sendString.c_str();

        // vilslog::printf("WebUDPParserService::checkaccountloginkey() sendBuf[%s]\n", sendBuf);

        item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
    }
}

void WebUDPParserService::queryaccountloginkey(UDPDataItem * item)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(item->getMsg(), result);
    int projectid = 1;

    if ( parsingSuccessful )
    {
        std::string userid = "";
        std::string account = "";
        std::string password = "";
        std::string token = "";

        if ( result.isMember("account") )
        {
            account = result["account"].asString();
        }
        if ( result.isMember("userid") )
        {
            userid = result["userid"].asString();
        }
        if ( result.isMember("password") )
        {
            password = result["password"].asString();
        }
        if ( result.isMember("token") )
        {
            token = result["token"].asString();
        }
        vilslog::printf("WebUDPParserService::queryaccountloginkey() account[%s] password[%s] token[%s] userid[%s]\n",
            account.c_str(), password.c_str(), token.c_str(), userid.c_str());

        std::ostringstream sendstream;

        bool valid = false;

        if ( userid.length() > 0 )
        {
            valid = mDeviceManager->checkValidUser(projectid, account.c_str(), password.c_str(), userid.c_str());
        }

        if ( valid )
        {
            // vilslog::printf("WebUDPParserService::queryaccountloginkey() valid\n");

            //
            // check exited login key
            //
            std::string key = mDeviceManager->queryLoginKey(projectid, account.c_str(), userid.c_str(), token.c_str());

            if (key.length() == 0)
            {
                std::string enc_string = token;
                enc_string.append(account);
                std::string salt = currentDateTimeForReport();

                vilslog::printf("WebUDPParserService::queryaccountloginkey() create new key salt[%s] enc_string[%s]\n", salt.c_str(), enc_string.c_str());

                key = create_MD5_encrypt_salt(salt, enc_string);

                LoginTokenStruct loginToken;
                loginToken.userid = convert_string_to_int(userid);
                loginToken.accountid = account;
                loginToken.token = token;
                loginToken.loginkey = key;
                WorkItem * item = new WorkItem("updateLoginToken", projectid, &loginToken);
                mMySQLJobQueue.add(item);
            }

            // vilslog::printf("WebUDPParserService::queryaccountloginkey() key[%s]\n", key.c_str());

            sendstream<<"{\"valid\":\"true\"";
            sendstream<<",\"userid\":\""<<userid<<"\"";
            sendstream<<",\"account\":\""<<account<<"\"";
            // sendstream<<",\"token\":\""<<token<<"\"";
            sendstream<<",\"key\":\""<<key<<"\"";
            sendstream<<"}\r\n";
        }
        else
        {
            sendstream<<"{\"valid\":\"false\"}\r\n";
        }

        std::string sendString = sendstream.str();

        const char * sendBuf = sendString.c_str();

        // vilslog::printf("WebUDPParserService::queryaccountloginkey() sendBuf[%s]\n", sendBuf);

        item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
    }
}

void WebUDPParserService::syncERPaccount(UDPDataItem * item)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(item->getMsg(), result);
    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        DataItem * senditem = new DataItem("syncERPaccount", projectid);
        mMainJobQueue.add(senditem);
    }
}

void WebUDPParserService::updatePushNotify(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        // std::string functions = "";
        std::string account = "";
        int projectid = getProjectId(result);

        //vilslog::printf("WebUDPParserService::updatePushNotify() buf[%s] length[%d]\n", buf, length);

        if ( result.isMember("ACCOUNT") )
        {
            account = result["ACCOUNT"].asString();
        }
        // if ( result.isMember("FUNCTIONS") )
        // {
        //     if ( result["FUNCTIONS"].isObject() )
        //     {
        //         vilslog::printf("WebUDPParserService::updatePushNotify() result[FUNCTIONS].isObject()\n");
        //     }
        //     else
        //         functions = result["FUNCTIONS"].asString();
        // }
        // if ( result.isMember("project_id") )
        // {
        //     projectid = getProjectId(result);
        // }

        vilslog::printf("WebUDPParserService::updatePushNotify() projectid[%d] account[%s] \n", projectid, account.c_str());

        DataItem * senditem = new DataItem("updatePushNotify", projectid, account.c_str());
        mMainJobQueue.add(senditem);
    }
}

void WebUDPParserService::updatePushNotifyTriggerMessage(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        // std::string functions = "";
        std::string account = "";
        int projectid = getProjectId(result);

        //vilslog::printf("WebUDPParserService::updatePushNotifyTriggerMessage() buf[%s] length[%d]\n", buf, length);

        if ( result.isMember("ACCOUNT") )
        {
            account = result["ACCOUNT"].asString();
        }

        vilslog::printf("WebUDPParserService::updatePushNotifyTriggerMessage() projectid[%d] account[%s] \n", projectid, account.c_str());

        DataItem * senditem = new DataItem("updatePushNotifyTriggerMessage", projectid, account.c_str());
        mMainJobQueue.add(senditem);
    }
}

void WebUDPParserService::sendmultinotification(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        std::list<std::string> * sendtargetlist = new std::list<std::string>();
        std::list<std::string> targetlist;
        std::list<std::string> subgrouptargetlist;
        std::list<std::string> subgrouplist;
        std::list<std::string> surveylist;
        std::list<std::string> deadlinelist;
        std::string accountid = "";
        std::string userid = "";
        std::string maingroups = "";
        std::string title = "";
        std::string context = "";
        std::string imageuid = "";
        int type = MULTI_PUSHNOTIFY_FUNCTION_205;
        int projectid = 1;
        // int nodetype = NODE_TYPE_TAG;

        if ( result.isMember("project_id") )
        {
            projectid = getProjectId(result);
        }
        if ( result.isMember("targetlist") )
        {
            if ( result["targetlist"].isArray() )
            {
                // vilslog::printf("WebUDPParserService::sendmultinotification() result[targetlist].size()[%d]\n", result["targetlist"].size());
                for(int i=0;i<(int)result["targetlist"].size();i++)
                {
                    Json::Value target = result["targetlist"][i];
                    sendtargetlist->push_back(target.asString());
                    targetlist.push_back(target.asString());
                    // vilslog::printf("WebUDPParserService::sendmultinotification() [%d] target[%s]\n", i, target.asCString());
                }
            }
            else
            {
                vilslog::printf("WebUDPParserService::sendmultinotification() targetlist is not array\n");
            }
        }
        else
        {
            vilslog::printf("WebUDPParserService::sendmultinotification() targetlist not found\n");
        }

        if ( result.isMember("subgrouptargetlist") )
        {
            if ( result["subgrouptargetlist"].isArray() )
            {
                for(int i=0;i<(int)result["subgrouptargetlist"].size();i++)
                {
                    Json::Value target = result["subgrouptargetlist"][i];
                    sendtargetlist->push_back(target.asString());
                    // vilslog::printf("WebUDPParserService::sendmultinotification() [%d] subgroutarget[%s]\n", i, target.asCString());
                }
            }
        }

        if ( result.isMember("subgrouplist") )
        {
            if ( result["subgrouplist"].isArray() )
            {
                for(int i=0;i<(int)result["subgrouplist"].size();i++)
                {
                    Json::Value subgroup = result["subgrouplist"][i];
                    subgrouplist.push_back(subgroup.asString());
                    // vilslog::printf("WebUDPParserService::sendmultinotification() [%d] subgroup[%s]\n", i, subgroup.asCString());
                }
            }
        }

        if ( result.isMember("surveylist") )
        {
            if ( result["surveylist"].isArray() )
            {
                for(int i=0;i<(int)result["surveylist"].size();i++)
                {
                    Json::Value surveyid = result["surveylist"][i];
                    surveylist.push_back(surveyid.asString());
                    // vilslog::printf("WebUDPParserService::sendnotification() [%d] surveyid[%s]\n",
                    //     i, surveyid.asCString());
                }
            }
        }
        if ( result.isMember("deadlinelist") )
        {
            if ( result["deadlinelist"].isArray() )
            {
                for(int i=0;i<(int)result["deadlinelist"].size();i++)
                {
                    Json::Value datetime = result["deadlinelist"][i];
                    deadlinelist.push_back(datetime.asString());
                    //vilslog::printf("WebUDPParserService::sendnotification() [%d] deadline[%s]\n", i, datetime.asCString());
                }
            }
        }
        if ( result.isMember("accountid") )
        {
            accountid = result["accountid"].asString();
        }
        if ( result.isMember("userid") )
        {
            userid = result["userid"].asString();
        }
        if ( result.isMember("maingroups") )
        {
            maingroups = result["maingroups"].asString();
        }
        if ( result.isMember("title") )
        {
            title = result["title"].asString();
        }
        if ( result.isMember("context") )
        {
            context = result["context"].asString();
        }
        if ( result.isMember("imageuid") )
        {
            imageuid = result["imageuid"].asString();
        }
        if ( result.isMember("type") )
        {
            if ( result["type"].isInt() )
                type = result["type"].asInt();
            else
                type = atoi(result["type"].asCString());
        }

        vilslog::printf("WebUDPParserService::sendmultinotification() projectid[%d] accountid[%s] userid[%s] maingroups[%s] title[%s] context.length[%d] imageuid[%s] type[%d]\n",
            projectid, accountid.c_str(), userid.c_str(), maingroups.c_str(), title.c_str(), context.length(), imageuid.c_str(), type);

        std::ostringstream sendstream;

        // sendstream<<"{\"success\":\"true\"}";

        std::string randomKey = random_string(16);

        sendstream<<"{\"waitkey\":\""<<randomKey<<"\"}";

        // put to queue
        SendNotifyStatusStruct * newStatus = new SendNotifyStatusStruct();
        newStatus->status = 0;// 0:wait finish 1:success 2:failed!
        newStatus->waitkey = randomKey;
        newStatus->message = "waiting";
        newStatus->createtime = getCurrentTimeInSec();
        mSendNotifyStatusList.push_back(newStatus);

        NotificationItem * notifactionitem = new NotificationItem(type, projectid, sendtargetlist, targetlist, subgrouplist, surveylist, deadlinelist, accountid, userid, maingroups, title, context, randomKey);
        notifactionitem->m_imageuid = imageuid;
        mNotifiactionQueue.add(notifactionitem);

        std::string sendString = sendstream.str();
        const char * sendBuf = sendString.c_str();
        item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);

        sendtargetlist->clear();
        delete sendtargetlist;
        targetlist.clear();
        subgrouplist.clear();
        surveylist.clear();
        deadlinelist.clear();
    }
    else
    {

        vilslog::printf("WebUDPParserService::sendnotification() parsingSuccessful[%d]\n", parsingSuccessful);
        std::ostringstream sendstream;

        sendstream<<"{\"success\":\"false\"}";

        std::string sendString = sendstream.str();
        const char * sendBuf = sendString.c_str();
        item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
    }
}

void WebUDPParserService::waitpushnotify(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        std::string waitkey = "";
        //int projectid = 1;

        if ( result.isMember("waitkey") )
        {
            waitkey = result["waitkey"].asString();
        }
        // if ( result.isMember("project_id") )
        // {
        //     projectid = getProjectId(result);
        // }

        // vilslog::printf("WebUDPParserService::waitpushnotify() projectid[%d] waitkey[%s] \n", projectid, waitkey.c_str());

        bool bFound = false;
        for (std::list<SendNotifyStatusStruct*>::iterator status = mSendNotifyStatusList.begin(); status != mSendNotifyStatusList.end(); status++)
        {
            // vilslog::printf("WebUDPParserService::waitpushnotify() (*status)->waitkey[%s] \n", (*status)->waitkey.c_str());

            if ( (*status)->waitkey.compare(waitkey) == 0 )
            {
                std::ostringstream sendstream;
                if ( (*status)->status == 0 )
                    sendstream<<"{\"success\":\"wait\"}";
                else
                if ( (*status)->status == 1 )
                    sendstream<<"{\"success\":\"true\"}";
                else
                if ( (*status)->status == 2 )
                    sendstream<<"{\"success\":\"false\",\"msg\":\""<<(*status)->message<<"\"}";


                std::string sendString = sendstream.str();
                const char * sendBuf = sendString.c_str();

                // vilslog::printf("WebUDPParserService::waitpushnotify() sendBuf[%s] \n", sendBuf);

                item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
                bFound = true;
                break;
            }
        }

        if(!bFound)
        {
            std::ostringstream sendstream;
            sendstream<<"{\"success\":\"false\",\"msg\":\"無此傳送紀錄！\"}";
            std::string sendString = sendstream.str();
            const char * sendBuf = sendString.c_str();
            item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
        }
    }
}

void WebUDPParserService::sendatcmd(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        std::string atcmd = "";
        std::string macaddress = "";
        int projectid = getProjectId(result);

        if ( result.isMember("atcmd") )
        {
            atcmd = result["atcmd"].asString();
        }
        atcmd.append("\r\n");

        if ( result.isMember("macaddress") )
        {
            macaddress = result["macaddress"].asString();
        }

        vilslog::printf("WebUDPParserService::sendatcmd() projectid[%d] atcmd[%s] macaddress[%d]\n",
            projectid, atcmd.c_str(), macaddress.length());

        std::ostringstream sendstream;

        // sendstream<<"{\"success\":\"true\"}";

        std::string randomKey = random_string(16);

        sendstream<<"{\"waitkey\":\""<<randomKey<<"\"}";

        // put to queue
        SendNotifyStatusStruct * newStatus = new SendNotifyStatusStruct();
        newStatus->status = 0;// 0:wait finish 1:success 2:failed!
        newStatus->waitkey = randomKey;
        newStatus->message = "waiting";
        newStatus->macaddress = macaddress;
        newStatus->createtime = getCurrentTimeInSec();
        mSendATCmdStatusList.push_back(newStatus);

        DataItem * senditem = new DataItem("sendATcmd", projectid, macaddress.c_str(), NODE_TYPE_LOCATOR);
        senditem->setATcommand(atcmd);
        senditem->setWaitkey(randomKey);
        mMainJobQueue.add(senditem);

        std::string sendString = sendstream.str();
        const char * sendBuf = sendString.c_str();
        item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);

    }
    else
    {

        vilslog::printf("WebUDPParserService::sendatcmd() parsingSuccessful[%d]\n", parsingSuccessful);
        std::ostringstream sendstream;

        sendstream<<"{\"success\":\"false\"}";

        std::string sendString = sendstream.str();
        const char * sendBuf = sendString.c_str();
        item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
    }
}

void WebUDPParserService::waitatcmd(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        std::string waitkey = "";
        //int projectid = getProjectId(result);

        if ( result.isMember("waitkey") )
        {
            waitkey = result["waitkey"].asString();
        }

        // vilslog::printf("WebUDPParserService::waitatcmd() projectid[%d] waitkey[%s] \n", projectid, waitkey.c_str());

        bool bFound = false;
        for (std::list<SendNotifyStatusStruct*>::iterator status = mSendATCmdStatusList.begin(); status != mSendATCmdStatusList.end(); status++)
        {
            // vilslog::printf("WebUDPParserService::waitatcmd() (*status)->waitkey[%s] \n", (*status)->waitkey.c_str());

            if ( (*status)->waitkey.compare(waitkey) == 0 )
            {
                std::ostringstream sendstream;
                if ( (*status)->status == 0 )
                    sendstream<<"{\"success\":\"wait\",\"msg\":\""<<(*status)->message<<"\"}";
                else
                if ( (*status)->status == 1 )
                    sendstream<<"{\"success\":\"true\",\"msg\":\""<<(*status)->message<<"\"}";
                else
                if ( (*status)->status == 3 || (*status)->status == -1 )
                    sendstream<<"{\"success\":\"false\",\"msg\":\""<<(*status)->message<<"\"}";


                std::string sendString = sendstream.str();
                const char * sendBuf = sendString.c_str();

                // vilslog::printf("WebUDPParserService::waitatcmd() sendBuf[%s] \n", sendBuf);

                item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
                bFound = true;
                break;
            }
        }

        if(!bFound)
        {
            std::ostringstream sendstream;
            sendstream<<"{\"success\":\"false\",\"msg\":\"無此傳送紀錄！\"}";
            std::string sendString = sendstream.str();
            const char * sendBuf = sendString.c_str();
            item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
        }
    }
}

void WebUDPParserService::printsystemlog(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        std::string logtype = "";
        int projectid = getProjectId(result);

        if ( result.isMember("logtype") )
        {
            logtype = result["logtype"].asString();
        }
        DataItem * senditem = new DataItem(logtype.c_str(), projectid);
        mMainJobQueue.add(senditem);
    }
}

void WebUDPParserService::sendABBoxToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        std::string waitkey = "";
        int projectid = getProjectId(result);

        std::ostringstream sendstream;

        sendstream<<"[";

        int nodeCnt = 0;
        std::list<NodeStruct*> * nodeList = mDeviceManager->get_Locator_list(projectid);
        for (std::list<NodeStruct*>::iterator existed = nodeList->begin(); existed != nodeList->end(); existed++)
        {
            if( (*existed)->tafid == 3 )
            {
                if(nodeCnt > 0)
                    sendstream<<",";
                sendstream<<"{\"nodeid\":\""<<(*existed)->nodeid<<"\",";
                sendstream<<"\"macaddress\":\""<<(*existed)->macaddress<<"\",";
                sendstream<<"\"nodename\":\""<<(*existed)->nodename<<"\"}";
                nodeCnt++;
            }
        }

        sendstream<<"]";

        std::string sendString = sendstream.str();
        const char * sendBuf = sendString.c_str();
        item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);

    }
}

void WebUDPParserService::savebodyinfoweight(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        std::string nodeid = "";
        std::string weight = "";

        if ( result.isMember("nodeid") )
            nodeid = result["nodeid"].asString();
        if ( result.isMember("weight") )
            weight = result["weight"].asString();

        DataItem * senditem = new DataItem("savebodyinfoweight", projectid, nodeid.c_str());
        senditem->setWeight(weight);
        mMainJobQueue.add(senditem);
    }
}

void WebUDPParserService::savebodyinfoheartbeat(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        std::string nodeid = "";
        std::string heartbeat = "";

        if ( result.isMember("nodeid") )
            nodeid = result["nodeid"].asString();
        if ( result.isMember("heartbeat") )
            heartbeat = result["heartbeat"].asString();

        DataItem * senditem = new DataItem("savebodyinfoheartbeat", projectid, nodeid.c_str());
        senditem->setHeartBeat(heartbeat);
        mMainJobQueue.add(senditem);
    }
}

void WebUDPParserService::savebodyinfoall(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        std::string accountid = "";
        std::string recorderid = "";
        std::string weight = "";
        std::string heartbeat = "";
        std::string bloodsugar = "";
        std::string systolicbloodpressure = "";
        std::string diastolicbloodpressure = "";
        std::string bodytemperature = "";
        std::string bodyfat = "";
        std::string bloodoxygen = "";
        std::string datetime = "";

        if ( result.isMember("accountid") )
            accountid = result["accountid"].asString();
        if ( result.isMember("recorderid") )
            recorderid = result["recorderid"].asString();
        if ( result.isMember("weight") )
            weight = result["weight"].asString();
        if ( result.isMember("heartbeat") )
            heartbeat = result["heartbeat"].asString();
        if ( result.isMember("bloodsugar") )
            bloodsugar = result["bloodsugar"].asString();
        if ( result.isMember("systolicbloodpressure") )
            systolicbloodpressure = result["systolicbloodpressure"].asString();
        if ( result.isMember("diastolicbloodpressure") )
            diastolicbloodpressure = result["diastolicbloodpressure"].asString();
        if ( result.isMember("bodytemperature") )
            bodytemperature = result["bodytemperature"].asString();
        if ( result.isMember("bodyfat") )
            bodyfat = result["bodyfat"].asString();
        if ( result.isMember("bloodoxygen") )
            bloodoxygen = result["bloodoxygen"].asString();
        if ( result.isMember("datetime") )
            datetime = result["datetime"].asString();

        DataItem * senditem = new DataItem("savebodyinfoall", projectid, accountid.c_str());
        senditem->setRecorderID(recorderid);
        senditem->setWeight(weight);
        senditem->setHeartBeat(heartbeat);
        senditem->setBloodSugar(bloodsugar);
        senditem->setSystolicBloodPressure(systolicbloodpressure);
        senditem->setDiastolicBloodPressure(diastolicbloodpressure);
        senditem->setBodyTemperature(bodytemperature);
        senditem->setBodyFat(bodyfat);
        senditem->setBloodOxygen(bloodoxygen);
        senditem->setDateTime(datetime);
        mMainJobQueue.add(senditem);
    }
}

void WebUDPParserService::sendForacaredataClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        DataItem * senditem = new DataItem("updateforacare", projectid);
        mMainJobQueue.add(senditem);

        const char * sendBuf = mForacareDataString.c_str();
        item->mUDPServer->sendToClient(sendBuf, (int)mForacareDataString.length(), (struct sockaddr *)&item->m_client_addr);
    }
}

void WebUDPParserService::sendUnknownPositionTagToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    // vilslog::printf("WebUDPParserService::sendUnknownPositionTagToClient() parsingSuccessful[%d]\n", parsingSuccessful);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        DataItem * senditem = new DataItem("UnknownPositionTag", projectid);
        mMainJobQueue.add(senditem);

        std::string strRanging = "[]";

        for (std::list<UnknownPositionTagStringStruct*>::iterator existed = mUnknownPositionTagList.begin(); existed != mUnknownPositionTagList.end(); existed++)
        {
            if ( (*existed)->projectId == projectid )
            {
                strRanging = (*existed)->UnknownPositionTagString;
                break;
            }
        }

        const char * sendBuf = strRanging.c_str();

        // vilslog::printf("WebUDPParserService::sendUnknownPositionTagToClient() sendBuf[%s]\n", sendBuf);

        item->mUDPServer->sendToClient(sendBuf, (int)strRanging.length(), (struct sockaddr *)&item->m_client_addr);
    }
}

void WebUDPParserService::uploadxlsxfile(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    // vilslog::printf("WebUDPParserService::uploadxlsxfile() parsingSuccessful[%d]\n", parsingSuccessful);

    std::ostringstream sendstream;

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        std::string filename;
        std::string fileurl;
        std::string target_Path;
        std::string fileinfo;
        std::string accountid;
        std::string userid;
        std::string groupid;
        std::string maingroups;

        if ( result.isMember("filename") )
            filename = result["filename"].asString();
        if ( result.isMember("fileurl") )
            fileurl = result["fileurl"].asString();
        if ( result.isMember("target_Path") )
            target_Path = result["target_Path"].asString();
        if ( result.isMember("fileinfo") )
            fileinfo = result["fileinfo"].asString();
        if ( result.isMember("accountid") )
            accountid = result["accountid"].asString();
        if ( result.isMember("userid") )
            userid = result["userid"].asString();
        if ( result.isMember("groupid") )
            groupid = result["groupid"].asString();
        if ( result.isMember("maingroups") )
            maingroups = result["maingroups"].asString();

        std::string maingroupid = "1";
        bool enableShowMainGroup = false;
        if ( (groupid.compare("1") == 0) || (groupid.compare("2") == 0) || (groupid.compare("4") == 0) )
        {
            enableShowMainGroup = true;
        }
        else
        {
            // 取得上傳人員 主集合
            std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');
            for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
            {
                maingroupid = (*maingroup);
                break;
            }
            maingroupList->clear();
            delete maingroupList;
        }

        vilslog::printf("WebUDPParserService::uploadxlsxfile() projectid[%d] filename[%s] fileurl[%s] target_Path[%s] fileinfo[%s] groupid[%s] maingroupid[%s]\n",
            projectid, filename.c_str(), fileurl.c_str(), target_Path.c_str(), fileinfo.c_str(), groupid.c_str(), maingroupid.c_str());

        OpenXLSX::XLDocument readXLSX;
        std::string issue_file = "";

        //unsigned int sheetCount = readXLSX.Workbook().SheetCount();
        bool bSuccessRead = false;
        try
        {
            readXLSX.OpenDocument(target_Path);

            auto wks1 = readXLSX.Workbook().Worksheet("Sheet1");

            unsigned int columnCount = wks1.ColumnCount();
            unsigned long rowCount = wks1.RowCount();

            vilslog::printf("WebUDPParserService::uploadxlsxfile() columnCount[%d] rowCount[%d]\n", columnCount, rowCount);

            std::vector<std::string> nameList;
            std::list<XLSXUserStruct*> xlsxUserList;

            for(unsigned int col=1;col<=columnCount;col++)
            {
                auto cell = wks1.Cell(1, col);
                nameList.push_back(cell.Value().Get<std::string>());
                //vilslog::printf("WebUDPParserService::uploadxlsxfile() i[%d] name[%s]\n", i, cell.Value().Get<std::string>().c_str());
            }

            for(unsigned int row=2;row<=rowCount;row++)
            {
                XLSXUserStruct * user = new XLSXUserStruct();

                if (!enableShowMainGroup)
                {
                    user->maingroups = maingroupid;
                }

                for(unsigned int col=1;col<=columnCount;col++)
                {
                    auto cell = wks1.Cell(row, col);

                    if (cell.ValueType() == OpenXLSX::XLValueType::String)
                    {
                        std::string nameLabel = nameList[col-1];
                        std::string stringValue = cell.Value().Get<std::string>();
                        // vilslog::printf("[%s][%s] ", nameLabel.c_str(), stringValue.c_str());

                        if (nameLabel.compare("名稱") == 0)
                        {
                            user->name = stringValue;
                        }
                        else
                        if (nameLabel.compare("帳號") == 0)
                        {
                            user->account = stringValue;
                        }
                        else
                        if (nameLabel.compare("職稱") == 0)
                        {
                            user->title = stringValue;
                        }
                        else
                        if (nameLabel.compare("功能權限群組") == 0)
                        {
                            user->group = stringValue;
                        }
                        else
                        if (nameLabel.compare("資料權限群組") == 0)
                        {
                            user->datagroups = stringValue;
                        }
                        else
                        if (nameLabel.compare("主集合") == 0 && enableShowMainGroup)
                        {
                            user->maingroups = stringValue;
                        }
                        else
                        if (nameLabel.compare("子集合") == 0)
                        {
                            user->subgroups = stringValue;
                        }
                        else
                        if (nameLabel.compare("RFID卡片") == 0)
                        {
                            user->rfidcards = stringValue;
                        }
                        else
                        if (nameLabel.compare("NFC卡片") == 0)
                        {
                            user->nfccards = stringValue;
                        }
                        else
                        if (nameLabel.compare("UWB卡片") == 0)
                        {
                            user->uwbcards = stringValue;
                        }
                        else
                        if (nameLabel.compare("傳感器") == 0)
                        {
                            // vilslog::printf("WebUDPParserService::uploadxlsxfile() nameLabel[%s] [%s]\n", nameLabel.c_str(), stringValue.c_str());
                            user->sensors = stringValue;
                        }
                    }
                }

                if (user->account.length() > 0)
                {
                    //vilslog::printf("WebUDPParserService::uploadxlsxfile() add [%s]\n", user->account.c_str());

                    xlsxUserList.push_back(user);
                }
                else
                {
                    delete user;
                }
                // vilslog::printf("\n");
            }
            //cout << "Cell A1: " << wks1.Cell("A1").Value().Get<std::string>() << endl;

            //vilslog::printf("WebUDPParserService::uploadxlsxfile() xlsxUserList->size()[%d]\n", (int)xlsxUserList.size());

            //
            // start check user
            //
            issue_file = check_xlsx_user(projectid, accountid, userid, &xlsxUserList, enableShowMainGroup);

            vilslog::printf("WebUDPParserService::uploadxlsxfile() issue_file[%s]\n", issue_file.c_str());

            nameList.clear();
            clear_XLSXUserStruct_list(&xlsxUserList);
            xlsxUserList.clear();
            bSuccessRead = true;
        }
        catch(const OpenXLSX::XLException &exc)
        {
            vilslog::printf("WebUDPParserService::uploadxlsxfile() OpenXLSX::XLException\n======\n");
            vilslog::printf(exc.what());
            vilslog::printf("\n======\n");
        }

        readXLSX.CloseDocument();

        if (bSuccessRead)
        {

            vilslog::printf("WebUDPParserService::uploadxlsxfile() issue_file.length()[%d]\n", issue_file.length());

            if ( issue_file.length() > 0)
                sendstream<<"{\"success\":\"true\",\"msg\":\"資料已讀取完畢!\", \"issuefile\":\""<<issue_file<<"\"}";
            else
                sendstream<<"{\"success\":\"true\",\"msg\":\"資料已讀取完畢!\"}";
        }
        else
            sendstream<<"{\"success\":\"false\",\"msg\":\"資料無法讀取!\"}";

    }
    else
    {
        sendstream<<"{\"success\":\"false\",\"msg\":\"資料無法讀取!\"}";
    }

    std::string sendString = sendstream.str();
    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);

    DataItem * senditem1 = new DataItem("updatesurveyresult", 1);
    mMainJobQueue.add(senditem1);

    sleep(2);

    DataItem * senditem2 = new DataItem("updateGAEuserinfo", 1);
    senditem2->setUserId(-1); // update all
    mMainJobQueue.add(senditem2);

    DataItem * senditem = new DataItem("reloadcarduser", 1);
    mMainJobQueue.add(senditem);

}

std::string WebUDPParserService::check_xlsx_user(int projectid, std::string accountid, std::string userid, std::list<XLSXUserStruct*> * xlsxUserList, bool enableShowMainGroup)
{
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    std::list<UserStruct*> * userList = NULL;
    std::list<UserGroupStruct*> * userGroupList = NULL;
    std::list<UserSubGroupStruct*> * userSubGroupList = NULL;
    std::list<UserMainGroupStruct*> * userMainGroupList = NULL;
    if (baseDb == NULL)
    {
        return "";
    }

    //=======================
    std::string report_date = TodayDateType1();
    std::string report_start_date = report_date;
    std::string report_end_date = report_date;

    std::string xlsxfilename = "/upload/";
    xlsxfilename.append("資料有誤");
    xlsxfilename.append("_");
    xlsxfilename.append(currentDateTimeForReport());
    xlsxfilename.append(".xlsx");

    std::string xlsxOutputfile = OUTPUT_DIRECTORY;
    xlsxOutputfile.append(xlsxfilename);

    OpenXLSX::XLDocument xlsxReport;
    xlsxReport.CreateDocument(xlsxOutputfile);
    auto wks1 = xlsxReport.Workbook().Worksheet("Sheet1");
    wks1.Cell("A1").Value() = "名稱";
    wks1.Cell("B1").Value() = "帳號";
    wks1.Cell("C1").Value() = "職稱";
    wks1.Cell("D1").Value() = "功能權限群組";
    wks1.Cell("E1").Value() = "資料權限群組";
    wks1.Cell("F1").Value() = "子集合";
    wks1.Cell("G1").Value() = "RFID卡片";
    wks1.Cell("H1").Value() = "NFC卡片";
    wks1.Cell("I1").Value() = "UWB卡片";
    if (enableShowMainGroup)
    {
        wks1.Cell("J1").Value() = "主集合";
        wks1.Cell("K1").Value() = "傳感器";
    }
    else
    {
        wks1.Cell("J1").Value() = "傳感器";
    }
    int issue_count = 2;
    //=======================

    userList = baseDb->get_Users();
    userGroupList = baseDb->get_UserGroups();
	userSubGroupList = baseDb->get_UserSubGroups();
    userMainGroupList = baseDb->get_UserMainGroups();

    for (std::list<XLSXUserStruct*>::iterator xlsxuser = xlsxUserList->begin(); xlsxuser != xlsxUserList->end(); xlsxuser++)
    {
        // vilslog::printf("[%s][%s][%s][%s][%s][%s][%s][%s]\n",
        //     (*xlsxuser)->name.c_str(), (*xlsxuser)->account.c_str(), (*xlsxuser)->group.c_str(), (*xlsxuser)->datagroups.c_str(), (*xlsxuser)->subgroups.c_str(), (*xlsxuser)->rfidcards.c_str(), (*xlsxuser)->nfccards.c_str(), (*xlsxuser)->uwbcards.c_str());

        UserStruct * newUser = new UserStruct();
        newUser->userid = -1;
        newUser->groupid = -1;
        newUser->empid = "";
        newUser->account = (*xlsxuser)->account;
        newUser->name = (*xlsxuser)->name;
        newUser->unit = "";
        newUser->title = (*xlsxuser)->title;
        newUser->gender = "";
        newUser->pwd = "";
        newUser->datetime = "";
        newUser->maingroups = NULL;
        newUser->datagroups = NULL;
        newUser->subgroups = NULL;
        newUser->rfidcards = convert_string_to_list((*xlsxuser)->rfidcards, ',');
        newUser->nfccards = convert_string_to_list((*xlsxuser)->nfccards, ',');
        newUser->uwbcards = convert_string_to_list((*xlsxuser)->uwbcards, ',');
        newUser->sensors = convert_string_to_list((*xlsxuser)->sensors, ',');
        newUser->imageuid = "";

        //
        // User Group
        //
        // vilslog::printf("WebUDPParserService::check_xlsx_user() (*xlsxuser)->group[%s]\n", (*xlsxuser)->group.c_str());
        if ((*xlsxuser)->group.length() > 0)
        {
            bool bFoundGroup = false;
            for (std::list<UserGroupStruct*>::iterator group = userGroupList->begin(); group != userGroupList->end(); group++)
            {
                if ( (*xlsxuser)->group.compare((*group)->groupname) == 0 )
                {
                    newUser->groupid = (*group)->groupid;
                    bFoundGroup = true;
                    break;
                }
            }
            if (!bFoundGroup)
            {
                // create new group
                UserGroupStruct * userGroup = new UserGroupStruct();
                userGroup->groupid = -1;
                userGroup->groupname = (*xlsxuser)->group;
                userGroup->permission = (*xlsxuser)->group;
                baseDb->update_UserGroup(userGroup);

                newUser->groupid = userGroup->groupid;
                vilslog::printf("check_xlsx_user() new UserGroup[%d][%s]\n", userGroup->groupid, userGroup->groupname.c_str());
                userGroupList->push_back(userGroup);
            }
        }

        //
        // Main group
        //
        if (enableShowMainGroup)
        {
            if ( (*xlsxuser)->maingroups.length() > 0 )
            {
                newUser->maingroups = new std::list<std::string>();
                check_xlsx_usermaingroup(projectid, accountid, userid, newUser->maingroups, (*xlsxuser)->maingroups, userMainGroupList);
            }
        }
        else
        {
            // 直接給予 上傳人員 main group
            newUser->maingroups = new std::list<std::string>();
            newUser->maingroups->push_back((*xlsxuser)->maingroups);
        }

        //
        // Data group
        //
        if ( (*xlsxuser)->datagroups.length() > 0 )
        {
            newUser->datagroups = new std::list<std::string>();
            check_xlsx_usersubgroup(projectid, accountid, userid, newUser->datagroups, (*xlsxuser)->datagroups, userSubGroupList, (*xlsxuser)->maingroups);
        }

        //
        // Sub group
        //
        if ( (*xlsxuser)->subgroups.length() > 0 )
        {
            // vilslog::printf("check_xlsx_user() (*xlsxuser)->subgroups[%s]\n", (*xlsxuser)->subgroups.c_str());
            newUser->subgroups = new std::list<std::string>();
            check_xlsx_usersubgroup(projectid, accountid, userid, newUser->subgroups, (*xlsxuser)->subgroups, userSubGroupList, (*xlsxuser)->maingroups);
        }

        //
        // 檢查 卡片重複
        //
        bool bIssue = false;
        for (std::list<UserStruct*>::iterator user = userList->begin(); user != userList->end(); user++)
        {
            // vilslog::printf("check_xlsx_user() 檢查 卡片重複  [%s] \n", (*user)->account.c_str());

            if ( (*xlsxuser)->account.compare((*user)->account) != 0 )
            {
                for (std::list<std::string>::iterator rfidcard = newUser->rfidcards->begin(); rfidcard != newUser->rfidcards->end(); rfidcard++)
                {
                    bool bExisted = checkCardExisted((*rfidcard), (*user)->rfidcards);

                    if (bExisted)
                    {
                        vilslog::printf("check_xlsx_user() [%s] RFID card[%s] is userd by [%s]\n", newUser->account.c_str(), (*rfidcard).c_str(), (*user)->account.c_str());

                        bIssue = true;

                        newUser->rfidcards->erase(rfidcard--);
                    }
                }

                for (std::list<std::string>::iterator nfccard = newUser->nfccards->begin(); nfccard != newUser->nfccards->end(); nfccard++)
                {
                    bool bExisted = checkCardExisted((*nfccard), (*user)->nfccards);
                    if (bExisted)
                    {
                        vilslog::printf("check_xlsx_user() [%s] NFC card[%s] is userd by [%s]\n", newUser->account.c_str(), (*nfccard).c_str(), (*user)->account.c_str());

                        bIssue = true;

                        newUser->nfccards->erase(nfccard--);
                    }
                }
            }
        }

        if (bIssue)
        {
            std::string colName = "A";
            colName.append(std::to_string(issue_count));
            wks1.Cell(colName).Value() = (*xlsxuser)->name;

            colName = "B";
            colName.append(std::to_string(issue_count));
            wks1.Cell(colName).Value() = (*xlsxuser)->account;

            colName = "C";
            colName.append(std::to_string(issue_count));
            wks1.Cell(colName).Value() = (*xlsxuser)->title;

            colName = "D";
            colName.append(std::to_string(issue_count));
            wks1.Cell(colName).Value() =(*xlsxuser)->group;

            colName = "E";
            colName.append(std::to_string(issue_count));
            wks1.Cell(colName).Value() = (*xlsxuser)->datagroups;

            colName = "F";
            colName.append(std::to_string(issue_count));
            wks1.Cell(colName).Value() = (*xlsxuser)->subgroups;

            colName = "G";
            colName.append(std::to_string(issue_count));
            wks1.Cell(colName).Value() = (*xlsxuser)->rfidcards;

            colName = "H";
            colName.append(std::to_string(issue_count));
            wks1.Cell(colName).Value() = (*xlsxuser)->nfccards;

            colName = "I";
            colName.append(std::to_string(issue_count));
            wks1.Cell(colName).Value() = (*xlsxuser)->uwbcards;

            if (enableShowMainGroup)
            {
                colName = "J";
                colName.append(std::to_string(issue_count));
                wks1.Cell(colName).Value() = (*xlsxuser)->maingroups;
            }

            colName = "K";
            colName.append(std::to_string(issue_count));
            wks1.Cell(colName).Value() = (*xlsxuser)->sensors;

            issue_count++;
        }


        // vilslog::printf("check_xlsx_user() 檢查 卡片重複  end\n");

        bool bFoundUser = false;
        for (std::list<UserStruct*>::iterator user = userList->begin(); user != userList->end(); user++)
        {
            if ( (*xlsxuser)->account.compare((*user)->account) == 0 )
            {
                newUser->userid = (*user)->userid;
                newUser->pwd = (*user)->pwd;
                newUser->imageuid = (*user)->imageuid;

                //
                // update cards
                //
                (*user)->rfidcards->clear();
                (*user)->nfccards->clear();

                for (std::list<std::string>::iterator rfidcard = newUser->rfidcards->begin(); rfidcard != newUser->rfidcards->end(); rfidcard++)
                {
                    (*user)->rfidcards->push_back((*rfidcard));
                }
                for (std::list<std::string>::iterator nfccard = newUser->nfccards->begin(); nfccard != newUser->nfccards->end(); nfccard++)
                {
                    (*user)->nfccards->push_back((*nfccard));
                }

                bFoundUser = true;
                break;
            }
        }
        if (!bFoundUser)
        {
            vilslog::printf("check_xlsx_user() new User[%s][%s]\n", newUser->name.c_str(), newUser->account.c_str());
        }
        else
        {
            // for (std::list<std::string>::iterator rfidcard = newUser->rfidcards->begin(); rfidcard != newUser->rfidcards->end(); rfidcard++)
            // {
            //     mDeviceManager->setRFIDCardUser(projectid, (*rfidcard), newUser->userid);
            // }
            // for (std::list<std::string>::iterator nfccard = newUser->nfccards->begin(); nfccard != newUser->nfccards->end(); nfccard++)
            // {
            //     mDeviceManager->setNFCCardUser(projectid, (*nfccard), newUser->userid);
            // }
        }

        // vilslog::printf("check_xlsx_user() User[%s][%s] groupid[%d]\n", newUser->name.c_str(), newUser->account.c_str(), newUser->groupid);
        //
        // if (newUser->datagroups != NULL)
        // {
        //     vilslog::printf("check_xlsx_user() datagroups.size()[%d]\n", (int)newUser->datagroups->size());
        // }
        // if (newUser->subgroups != NULL)
        // {
        //     vilslog::printf("check_xlsx_user() subgroups.size()[%d]\n", (int)newUser->subgroups->size());
        // }
        // vilslog::printf("check_xlsx_user() rfidcards.size()[%d] nfccards.size()[%d] uwbcards.size()[%d]\n",
        //     (int)newUser->rfidcards->size(), (int)newUser->nfccards->size(), (int)newUser->uwbcards->size());

        WorkItem * item = new WorkItem("updateUserStruct", projectid, newUser);
        mMySQLJobQueue.add(item);

        clear_UserStruct(newUser);
        delete newUser;
    }

    if (issue_count > 2)
        xlsxReport.SaveDocument();
    xlsxReport.CloseDocument();

    //
    // 更新上傳 人員 資料權限
    //
    int upload_userid = convert_string_to_int(userid);
    for (std::list<UserStruct*>::iterator user = userList->begin(); user != userList->end(); user++)
    {
        //if ( accountid.compare((*user)->account) == 0 )
        if ( upload_userid == (*user)->userid )
        {
            //
            // 檢查是否在主集合中有此帳號 為管理者
            //
            for (std::list<UserMainGroupStruct*>::iterator mainGroup = userMainGroupList->begin(); mainGroup != userMainGroupList->end(); mainGroup++)
            {
                bool bNeedCheck = false;
                //for (std::list<std::string>::iterator maingroupaccount = (*mainGroup)->accountidList->begin(); maingroupaccount != (*mainGroup)->accountidList->end(); maingroupaccount++)
                for (std::list<std::string>::iterator maingroupuserid = (*mainGroup)->useridList->begin(); maingroupuserid != (*mainGroup)->useridList->end(); maingroupuserid++)
                {
                    int maingroup_userid = convert_string_to_int((*maingroupuserid));
                    if ( upload_userid == maingroup_userid )
                    {
                        // 發現是管理者
                        bNeedCheck = true;
                        vilslog::printf("check_xlsx_user() userMainGroupList maingroup_userid[%d] bNeedCheck\n", maingroup_userid);
                        break;
                    }
                }

                if (bNeedCheck)
                {
                    // 檢查 是否尚未 更新使用者權限資料
                    bool bFound = false;
                    for (std::list<std::string>::iterator userMainGroup = (*user)->maingroups->begin(); userMainGroup != (*user)->maingroups->end(); userMainGroup++)
                    {
                        int maingroupid = convert_string_to_int(*userMainGroup);
                        if ( maingroupid == (*mainGroup)->id )
                        {
                            bFound = true;
                            break;
                        }
                    }
                    if (!bFound)
                    {
                        std::string newGroupid = to_string((*mainGroup)->id);
                        (*user)->maingroups->push_back(newGroupid);
                        vilslog::printf("check_xlsx_user() userMainGroupList 新增 newGroupid[%s] \n", newGroupid.c_str());
                    }
                }
            }

            //
            // 檢查是否在子集合中有此帳號 為管理者
            //
            for (std::list<UserSubGroupStruct*>::iterator subGroup = userSubGroupList->begin(); subGroup != userSubGroupList->end(); subGroup++)
            {
                bool bNeedCheck = false;
                //for (std::list<std::string>::iterator subgroupaccount = (*subGroup)->accountidList->begin(); subgroupaccount != (*subGroup)->accountidList->end(); subgroupaccount++)
                for (std::list<std::string>::iterator subgroupuserid = (*subGroup)->useridList->begin(); subgroupuserid != (*subGroup)->useridList->end(); subgroupuserid++)
                {
                    int subgroup_userid = convert_string_to_int((*subgroupuserid));
                    if ( upload_userid == subgroup_userid )
                    {
                        // 發現是管理者
                        bNeedCheck = true;
                        vilslog::printf("check_xlsx_user() userSubGroupList subgroup_userid[%d] bNeedCheck\n", subgroup_userid);
                        break;
                    }
                }

                if (bNeedCheck)
                {
                    // 檢查 是否尚未 更新使用者權限資料
                    bool bFound = false;
                    for (std::list<std::string>::iterator userDataGroup = (*user)->datagroups->begin(); userDataGroup != (*user)->datagroups->end(); userDataGroup++)
                    {
                        int subgroupid = convert_string_to_int(*userDataGroup);
                        if ( subgroupid == (*subGroup)->subgroupid )
                        {
                            bFound = true;
                            break;
                        }
                    }
                    if (!bFound)
                    {
                        std::string newSubGroupid = to_string((*subGroup)->subgroupid);
                        (*user)->datagroups->push_back(newSubGroupid);
                        vilslog::printf("check_xlsx_user() userSubGroupList 新增 newSubGroupid[%s] \n", newSubGroupid.c_str());
                    }
                }
            }

            WorkItem * item = new WorkItem("updateUserStruct", projectid, (*user));
            mMySQLJobQueue.add(item);

            break;
        }
    }


    if (userList != NULL)
    {
        clear_UserStruct_list(userList);
        delete userList;
    }
    if (userGroupList != NULL)
    {
        clear_UserGroupStruct_list(userGroupList);
        delete userGroupList;
    }
    if (userSubGroupList != NULL)
    {
        clear_UserSubGroupStruct_list(userSubGroupList);
        delete userSubGroupList;
    }
    if (userMainGroupList != NULL)
    {
        clear_UserMainGroupStruct_list(userMainGroupList);
        delete userMainGroupList;
    }

    if (issue_count > 2)
        return xlsxfilename;
    else
        return "";
}

void WebUDPParserService::check_xlsx_usersubgroup(int projectid, std::string accountid, std::string userid, std::list<std::string> * userDataGroups, std::string datagroups, std::list<UserSubGroupStruct*> * userSubGroupList, std::string maingroups)
{
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
    {
        return;
    }

    std::list<std::string> * datagroupList = convert_string_to_list(datagroups, ',');

    for (std::list<std::string>::iterator groupname = datagroupList->begin(); groupname != datagroupList->end(); groupname++)
    {

        vilslog::printf("check_xlsx_usersubgroup() groupname[%s]\n", (*groupname).c_str());

        if ( (*groupname).compare("自己") == 0 )
        {
            continue;
        }
        else
        if ( (*groupname).compare("全部") == 0 )
        {
            userDataGroups->push_back("99999");
            continue;
        }
        else
        {
            bool bFoundSubGroup = false;
            for (std::list<UserSubGroupStruct*>::iterator userSubGroup = userSubGroupList->begin(); userSubGroup != userSubGroupList->end(); userSubGroup++)
            {
                vilslog::printf("check_xlsx_usersubgroup() userSubGroupList subgroupname[%s]\n", (*userSubGroup)->subgroupname.c_str());
                if ( (*userSubGroup)->subgroupname.compare((*groupname)) == 0 )
                {
                    userDataGroups->push_back(std::to_string((*userSubGroup)->subgroupid));
                    bFoundSubGroup = true;
                    break;
                }
            }
            if (!bFoundSubGroup)
            {
                // create new sub group
                UserSubGroupStruct * newSubGroup = new UserSubGroupStruct();
                newSubGroup->subgroupid = -1;
                newSubGroup->subgroupname = (*groupname);
                newSubGroup->permission = (*groupname);
                newSubGroup->accountidList = new std::list<std::string>();
                newSubGroup->accountidList->push_back(accountid);
                newSubGroup->useridList = new std::list<std::string>();
                newSubGroup->useridList->push_back(userid);
                newSubGroup->type = 0;
                newSubGroup->curriculumrecordid = 0;
                newSubGroup->validdate = TodayDateType1();

                std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');
                for (std::list<std::string>::iterator groupid = maingroupList->begin(); groupid != maingroupList->end(); groupid++)
                {
                    // 取第一個
                    newSubGroup->maingroupid = convert_string_to_int((*groupid));
                    break;
                }
                maingroupList->clear();
                delete maingroupList;


                baseDb->update_UserSubGroup(newSubGroup);
                // WorkItem * item = new WorkItem("updateusersubgroup", projectid, newSubGroup);
                // mMySQLJobQueue.add(item);

                userDataGroups->push_back(std::to_string(newSubGroup->subgroupid));
                vilslog::printf("check_xlsx_usersubgroup() new UserSubGroup[%d][%s]\n", newSubGroup->subgroupid, newSubGroup->subgroupname.c_str());
                userSubGroupList->push_back(newSubGroup);
            }
        }
    }

    datagroupList->clear();
    delete datagroupList;
}

void WebUDPParserService::check_xlsx_usermaingroup(int projectid, std::string accountid, std::string userid, std::list<std::string> * userMainGroups, std::string maingroups, std::list<UserMainGroupStruct*> * userMainGroupList)
{
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
    {
        return;
    }

    std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');

    for (std::list<std::string>::iterator groupname = maingroupList->begin(); groupname != maingroupList->end(); groupname++)
    {
        if ( (*groupname).compare("自己") == 0 )
        {
            continue;
        }
        else
        if ( (*groupname).compare("全部") == 0 )
        {
            userMainGroups->push_back("99999");
            continue;
        }
        else
        {
            bool bFoundMainGroup = false;
            for (std::list<UserMainGroupStruct*>::iterator userMainGroup = userMainGroupList->begin(); userMainGroup != userMainGroupList->end(); userMainGroup++)
            {
                if ( (*userMainGroup)->name.compare((*groupname)) == 0 )
                {
                    userMainGroups->push_back(std::to_string((*userMainGroup)->id));
                    bFoundMainGroup = true;
                    break;
                }
            }
            if (!bFoundMainGroup)
            {
                // create new sub group
                UserMainGroupStruct * newMainGroup = new UserMainGroupStruct();
                newMainGroup->id = -1;
                newMainGroup->autojoin = 0; //0:不可自動加入 1:使用識別碼與帳號加入 2:自行加入
                newMainGroup->name = (*groupname);
                newMainGroup->permission = (*groupname);
                newMainGroup->welcommessage = "歡迎加入";
                newMainGroup->welcommessage.append((*groupname));
                newMainGroup->accountidList = new std::list<std::string>();
                newMainGroup->accountidList->push_back(accountid);
                newMainGroup->useridList = new std::list<std::string>();
                newMainGroup->useridList->push_back(userid);

                baseDb->update_UserMainGroup(newMainGroup);
                // WorkItem * item = new WorkItem("updateusermaingroup", projectid, newMainGroup);
                // mMySQLJobQueue.add(item);

                userMainGroups->push_back(std::to_string(newMainGroup->id));
                vilslog::printf("check_xlsx_usermaingroup() new UserMainGroup[%d][%s]\n", newMainGroup->id, newMainGroup->name.c_str());
                userMainGroupList->push_back(newMainGroup);
            }
        }
    }

    maingroupList->clear();
    delete maingroupList;
}

void WebUDPParserService::newleaveform(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        if ( result.isMember("userid") )
        {
            std::string formid = result["formid"].asString();
            //std::string accountid = result["accountid"].asString();
            std::string userid = result["userid"].asString();
            std::string type = result["type"].asString();
            std::string reason = result["reason"].asString();
            std::string startdate = result["startdate"].asString();
            std::string starttime = result["starttime"].asString();
            std::string enddate = result["enddate"].asString();
            std::string endtime = result["endtime"].asString();
            std::string applyusername = result["applyusername"].asString();
            std::string applyuserrelation = result["applyuserrelation"].asString();
            std::string imageids = result["imageids"].asString();
            std::string fileids = result["fileids"].asString();
            std::string maingroups = result["maingroups"].asString();

            std::list<std::string> * sendtargetlist = NULL;
            std::list<std::string> targetlist;
            std::string title = "已遞交請假單";
            std::string username = "";

            //
            // 推播給子集合管理者
            //
            int userid_int = convert_string_to_int(userid);
            UserStruct * user = mDeviceManager->getUser(projectid, userid_int);
            if (user != NULL)
            {
                username = user->name;
                title = user->name;
                title += " 已遞交請假單";

                sendtargetlist = getSubgGroupManager(projectid, user);
                for (std::list<std::string>::iterator target = sendtargetlist->begin(); target != sendtargetlist->end(); target++)
                {
                    targetlist.push_back((*target));
                }
                // clear_UserStruct(user);
                // delete user;
            }

            std::ostringstream sendstream;

            sendstream << "請假人員:"<<username<<endl;
            sendstream << "假勤項目:"<<type<<endl;
            sendstream << "請假事由:"<<reason<<endl;
            sendstream << "請假時間:"<<startdate<< " " <<starttime<<" 到 "<<enddate<< " " <<endtime<<endl;
            sendstream << "申請人:"<<applyusername<<" ";
            sendstream << "關係:"<<applyuserrelation<<endl;

            std::string context = sendstream.str();

            if (sendtargetlist != NULL)
            {
                // ===========================================
                // 傳送給 子集合管理者
                std::string randomKey1 = random_string(16);
                std::list<std::string> subgrouplist;
                std::list<std::string> surveylist;
                std::list<std::string> deadlinelist;
                std::string surveyid = ApplicationForm_LEAVE_SURVEYID;
                surveylist.push_back(surveyid);
                deadlinelist.push_back(currentDateTimeForSQL(-7));
                NotificationItem * notifactionitem1 = new NotificationItem(MULTI_PUSHNOTIFY_FUNCTION_2222, projectid, sendtargetlist, targetlist, subgrouplist, surveylist, deadlinelist, user->account, userid, maingroups, title, context, randomKey1);
                notifactionitem1->m_imageuid = imageids;
                notifactionitem1->m_formid = formid;
                mNotifiactionQueue.add(notifactionitem1);

                sendtargetlist->clear();
                delete sendtargetlist;
            }
            targetlist.clear();
            // ===========================================
            // 傳送給 申請人
            std::string randomKey2 = random_string(16);
            std::list<std::string> userlist;
            //userlist.push_back(accountid);
            userlist.push_back(userid);
            NotificationItem * notifactionitem2 = new NotificationItem(MULTI_PUSHNOTIFY_FUNCTION_2222, projectid, userlist, user->account, userid, maingroups, title, context, randomKey2);
            mNotifiactionQueue.add(notifactionitem2);
            // ===========================================
        }
    }
}

std::list<std::string> * WebUDPParserService::getSubgGroupManager(int projectid, UserStruct * user)
{
    std::list<std::string> * targetlist = new std::list<std::string>();

    if (user == NULL)
    {
        return targetlist;
    }

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    std::list<UserSubGroupStruct*> * userSubGroupList = NULL;
    if (baseDb != NULL)
    {
    	userSubGroupList = baseDb->get_UserSubGroups();

        for (std::list<std::string>::iterator userSubGroupid = user->subgroups->begin(); userSubGroupid != user->subgroups->end(); userSubGroupid++)
        {
            int curUserGroupid = convert_string_to_int((*userSubGroupid));

            for (std::list<UserSubGroupStruct *>::iterator userSubGroup = userSubGroupList->begin(); userSubGroup != userSubGroupList->end(); userSubGroup++)
            {
                if( (*userSubGroup)->subgroupid == curUserGroupid )
                {
                    // 找到 子集合管理者
                    for (std::list<std::string>::iterator userid_it = (*userSubGroup)->useridList->begin(); userid_it != (*userSubGroup)->useridList->end(); userid_it++)
                    {
                        //
                        // 不重複加入
                        //
                        bool bExisted = false;
                        for (std::list<std::string>::iterator userid_existed = targetlist->begin(); userid_existed != targetlist->end(); userid_existed++)
                        {
                            if ( (*userid_it).compare((*userid_existed)) == 0 )
                            {
                                bExisted = true;
                                break;
                            }
                        }
                        if (!bExisted)
                        {
                            targetlist->push_back((*userid_it));
                        }
                    }
                    // for (std::list<std::string>::iterator accountid_it = (*userSubGroup)->accountidList->begin(); accountid_it != (*userSubGroup)->accountidList->end(); accountid_it++)
                    // {
                    //     targetlist->push_back((*accountid_it));
                    // }
                    break;
                }
            }
        }
    }

    if (userSubGroupList != NULL)
    {
        clear_UserSubGroupStruct_list(userSubGroupList);
        delete userSubGroupList;
    }

    return targetlist;
}

void WebUDPParserService::leaveformsignoff(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        if ( result.isMember("formid") )
        {
            std::string formid = result["formid"].asString();
            std::string userid = result["userid"].asString();
            std::string type = result["type"].asString();
            std::string reason = result["reason"].asString();
            std::string startdate = result["startdate"].asString();
            std::string starttime = result["starttime"].asString();
            std::string enddate = result["enddate"].asString();
            std::string endtime = result["endtime"].asString();
            std::string applyusername = result["applyusername"].asString();
            std::string signoffstatus = result["signoffstatus"].asString();
            std::string signoffuserid = result["signoffuserid"].asString();
            std::string signoffaccountname = result["signoffaccountname"].asString();
            std::string maingroups = result["maingroups"].asString();

            std::list<std::string> targetlist;
            // targetlist.push_back(accountid);
            targetlist.push_back(userid);
            std::string randomKey = random_string(16);
            std::string title = "請假單審核結果通知";

            std::string status = "簽核中";
            if (signoffstatus.compare("1") == 0)
            {
                status = "已簽核";
            }
            else
            if (signoffstatus.compare("99") == 0)
            {
                status = "未核准";
            }

            std::ostringstream sendstream;

            sendstream << "審核結果:"<<status<<endl;
            sendstream << "假勤項目:"<<type<<endl;
            sendstream << "請假事由:"<<reason<<endl;
            sendstream << "請假時間:"<<startdate<< " " <<starttime<<" 到 "<<enddate<< " " <<endtime<<endl;
            sendstream << "申請人:"<<applyusername<<endl;

            std::string context = sendstream.str();

            NotificationItem * notifactionitem = new NotificationItem(MULTI_PUSHNOTIFY_FUNCTION_2222, projectid, targetlist, "", userid, maingroups, title, context, randomKey);
            mNotifiactionQueue.add(notifactionitem);
        }
    }
}

void WebUDPParserService::newassistevent(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        if ( result.isMember("userid") )
        {
            std::string userid = result["userid"].asString();
            std::string maingroups = result["maingroups"].asString();
            // std::string accountid = result["accountid"].asString();
            std::string assistid = result["assistid"].asString();
            std::string description = result["description"].asString();
            std::string eventtime = result["eventtime"].asString();
            std::string weekday = result["weekday"].asString();
            std::string validdate = result["validdate"].asString();
            std::string invaliddate = result["invaliddate"].asString();
            std::string deadline = result["deadline"].asString();
            std::string imageids = result["imageids"].asString();
            std::string fileids = result["fileids"].asString();
            std::string applyusername = result["applyusername"].asString();
            std::string applyuserrelation = result["applyuserrelation"].asString();
            int eventid = result["eventid"].asInt();

            std::list<std::string> * sendtargetlist = NULL;
            std::list<std::string> targetlist;
            std::string title = "已遞交申請協助事項";
            std::string username = userid;
            std::string accountid = "";

            vilslog::printf("WebUDPParserService::newassistevent() assistid[%s] title[%s] userid[%s] eventid[%d]\n",
                assistid.c_str(), title.c_str(), userid.c_str(), eventid);

            // 取得 assist
            MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
            AssistStruct * assistinfo = baseDb->read_Assist(convert_string_to_int(assistid));
            if (assistinfo == NULL)
            {
                return;
            }

            int userid_int = convert_string_to_int(userid);
            UserStruct * user = mDeviceManager->getUser(projectid, userid_int);
            if (user != NULL)
            {
                username = user->name;
                accountid = user->account;
            }

            if ( assistinfo->assignusers.length() == 0 )
            {
                //
                // 推播給子集合管理者
                //
                sendtargetlist = getSubgGroupManager(projectid, user);
            }
            else
            {
                //
                // 推播給指定人員
                //
                sendtargetlist = convert_string_to_list(assistinfo->assignusers, ',');
            }

            for (std::list<std::string>::iterator target = sendtargetlist->begin(); target != sendtargetlist->end(); target++)
            {
                targetlist.push_back((*target));
            }
            // vilslog::printf("WebUDPParserService::newassistevent() username[%s] targetlist.size()[%d]\n", username.c_str(), (int)targetlist->size());

            std::ostringstream sendstream;

            sendstream << "人員:"<<username<<endl;
            sendstream << "申請者:"<<applyusername<<"/"<<applyuserrelation<<endl;
            sendstream << "申請時間:"<<currentDateTimeForSQL()<<endl;
            sendstream << "事項名稱:"<<assistinfo->title<<endl;
            sendstream << "申請說明:"<<description<<endl;

            std::string context = sendstream.str();

            // vilslog::printf("WebUDPParserService::newassistevent() context[%s]\n", context.c_str());


            if (sendtargetlist != NULL)
            {
                // ===========================================
                // 傳送給 子集合管理者
                std::string randomKey1 = random_string(16);
                std::list<std::string> subgrouplist;
                std::list<std::string> surveylist;
                std::list<std::string> deadlinelist;
                std::string surveyid = ApplicationForm_ASSIST_ID;
                surveylist.push_back(surveyid);
                deadlinelist.push_back(currentDateTimeForSQL(-7));
                NotificationItem * notifactionitem1 = new NotificationItem(MULTI_PUSHNOTIFY_FUNCTION_204, projectid, sendtargetlist, targetlist, subgrouplist, surveylist, deadlinelist, accountid, userid, maingroups, title, context, randomKey1);
                notifactionitem1->m_imageuid = imageids;
                notifactionitem1->m_formid = to_string(eventid);
                mNotifiactionQueue.add(notifactionitem1);
            }
            // ===========================================
            // 傳送給 申請人
            std::string randomKey2 = random_string(16);
            std::list<std::string> userlist;
            //userlist.push_back(accountid);
            userlist.push_back(userid);
            NotificationItem * notifactionitem2 = new NotificationItem(MULTI_PUSHNOTIFY_FUNCTION_204, projectid, userlist, accountid, userid, maingroups, title, context, randomKey2);
            mNotifiactionQueue.add(notifactionitem2);
            // ===========================================

            delete assistinfo;

            // if (user != NULL)
            // {
            //     clear_UserStruct(user);
            //     delete user;
            // }

            targetlist.clear();
            if (sendtargetlist != NULL)
            {
                sendtargetlist->clear();
                delete sendtargetlist;
            }
        }
    }
}
//
// void WebUDPParserService::update_nodemaingroup(char * buf, int length)
// {
//     Json::Value result;
//     bool parsingSuccessful = parse_payload_JSON(buf, result);
//
//     if ( parsingSuccessful )
//     {
//         std::string macaddress = result["macaddress"].asString();
//         std::string maingroupid = result["maingroupid"].asString();
//         int maingroupid_i = stoi(maingroupid);
//
//         vilslog::printf("WebUDPParserService::update_nodemaingroup() macaddress[%s] maingroupid_i[%d]\n", macaddress.c_str(), maingroupid_i);
//
//         mDeviceManager->update_Node_maingroup(macaddress.c_str(), maingroupid_i);
//     }
// }

void WebUDPParserService::update_nodemaingroup(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        std::string macaddress = result["macaddress"].asString();
        std::string maingroupid = result["maingroupid"].asString();
        int maingroupid_i = stoi(maingroupid);

        vilslog::printf("WebUDPParserService::update_nodemaingroup() macaddress[%s] maingroupid_i[%d]\n", macaddress.c_str(), maingroupid_i);

        mDeviceManager->update_Node_maingroup(macaddress.c_str(), maingroupid_i);


        int projectid = 1;
        DataItem * senditem1 = new DataItem("dumpanchor", projectid);
        mMainJobQueue.add(senditem1);
        DataItem * senditem2 = new DataItem("dumpcoord", projectid);
        mMainJobQueue.add(senditem2);
        DataItem * senditem3 = new DataItem("dumplocator", projectid);
        mMainJobQueue.add(senditem3);

    }
}

void WebUDPParserService::sensor_info(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        std::string did = result["did"].asString();
        std::string lat = result["lat"].asString();
        std::string lng = result["lng"].asString();
        std::string HR = "";
        std::string SPO2 = "";
        std::string Battery = "";
        std::ostringstream jsonvalue;

        SensorDeviceStruct * sensorExisted = mDeviceManager->get_SensorDevice(projectid, did);

        std::string Latitude = lat;
        std::string Longitude = lng;
        std::string Latitude_pre = lat;
        std::string Longitude_pre = lng;

        double d_Longitude = 0.0;
        double d_Latitude = 0.0;
        double distanceOffset = 0;

        if (lng.length() > 0)
            d_Longitude = std::stof(lng);
        if (lat.length() > 0)
            d_Latitude = std::stof(lat);

        // vilslog::printf("WebUDPParserService::sensor_info() Latitude[%s] Longitude[%s]\n", Latitude.c_str(), Longitude.c_str());

        bool isAlert = true;
        bool sendAlert = true;
        if ( result.isMember("HR") )
        {
            HR = result["HR"].asString();
            SPO2 = result["SPO2"].asString();
            Battery = result["Battery"].asString();
            jsonvalue<<"{\"HR\":\""<<HR<<"\",\"SPO2\":\""<<SPO2<<"\",\"Battery\":\""<<Battery<<"\"}";

            sendAlert = false;
            isAlert = false;
        }
        else
        if ( result.isMember("content"))
        {
            std::string category = result["category"].asString();
            std::string data_source = result["data_source"].asString();
            std::string content = result["content"].asString();
            jsonvalue<<"{\"category\":\""<<category<<"\",\"data_source\":\""<<data_source<<"\",\"content\":\""<<content<<"\"}";

            sendAlert = true;
            isAlert = true;
        }

        UserStruct * userLink = NULL;
        bool bFoundUser = false;
        if (sensorExisted == NULL)
        {
            //========================================
            SensorDeviceStruct * sensorDevice = new SensorDeviceStruct();
            sensorDevice->alive = 1;
            sensorDevice->id = did;
            sensorDevice->name = did;
            sensorDevice->type = "ProMOS Watch";
            sensorDevice->jsonvalue = jsonvalue.str();
            sensorDevice->Longitude = lng;
            sensorDevice->Latitude = lat;
            sensorDevice->Battery = Battery;
            sensorDevice->Version = "";
            sensorDevice->datetime = "";
            sensorDevice->maingroupid = 1;

            WorkItem * witem = new WorkItem("sensordevice", projectid, sensorDevice);
            mMySQLJobQueue.add(witem);

            mDeviceManager->setSensorDeviceAlive(projectid, sensorDevice);
            mDeviceManager->setSensorDeviceValue(projectid, did, Battery, currentDateTimeForSQL());

            delete sensorDevice;
            //========================================
        }
        else
        {
            Latitude_pre = sensorExisted->Latitude;
            Longitude_pre = sensorExisted->Longitude;

            // vilslog::printf("WebUDPParserService::sensor_info() Latitude_pre[%s] Longitude_pre[%s]\n", Latitude_pre.c_str(), Longitude_pre.c_str());

            int i_Longitude = 0;
            int i_Latitude = 0;
            if (lng.length() > 0)
                i_Longitude = std::stoi(lng);
            if (lat.length() > 0)
                i_Latitude = std::stoi(lat);

            if ( i_Longitude == 0 || i_Latitude == 0)
            {
                Latitude = sensorExisted->Latitude;
                Longitude = sensorExisted->Longitude;
            }

            if (Longitude.length() > 0)
                d_Longitude = std::stof(Longitude);
            if (Latitude.length() > 0)
                d_Latitude = std::stof(Latitude);

            // vilslog::printf("WebUDPParserService::sensor_info() Latitude[%s] Longitude[%s] i_Longitude[%d] i_Latitude[%d]\n", Latitude.c_str(), Longitude.c_str(), i_Longitude, i_Latitude);

            //========================================
            SensorDeviceStruct * sensorDevice = new SensorDeviceStruct();
            sensorDevice->id = did;
            sensorDevice->name = sensorExisted->name;
            sensorDevice->type = "ProMOS Watch";
            sensorDevice->jsonvalue = jsonvalue.str();
            sensorDevice->Longitude = Longitude;
            sensorDevice->Latitude = Latitude;
            sensorDevice->Battery = Battery;
            sensorDevice->Version = sensorExisted->Version;
            sensorDevice->maingroupid = sensorExisted->maingroupid;

            mDeviceManager->setSensorDeviceAlive(projectid, sensorDevice);
            mDeviceManager->setSensorDeviceValue(projectid, sensorExisted->id, Battery, currentDateTimeForSQL());

            WorkItem * witem = new WorkItem("sensordevice", projectid, sensorDevice);
            mMySQLJobQueue.add(witem);

            //-------------------
            WorkItem * witem1 = new WorkItem("sensordevice", projectid, sensorDevice);
            mSensorBodyInfoPublisher->publish(witem1);
            //-------------------

            delete sensorDevice;
            //========================================

            // vilslog::printf("WebUDPParserService::sensor_info() sensorExisted->Latitude[%s] Longitude[%s]\n", sensorExisted->Latitude.c_str(), sensorExisted->Longitude.c_str());

            userLink = update_sensor_user_location(projectid, did, Longitude, Latitude);
            if ( userLink != NULL )
                bFoundUser = true;
        }

        if ( sendAlert == false )
        {
            if (bFoundUser && ((HR.compare("255") != 0 && HR.compare("0") != 0) || (SPO2.compare("255") != 0 && SPO2.compare("0") != 0)) )
            {
                int i_HR = 0;
                int i_SPO2 = 0;
                DataItem * senditem = new DataItem("savebodyinfoall", projectid, userLink->account.c_str());
                if (HR.compare("255") != 0 && HR.compare("0") != 0)
                {
                    i_HR = convert_string_to_int(HR);
                    senditem->setHeartBeat(HR);
                }
                if (SPO2.compare("255") != 0 && SPO2.compare("0") != 0)
                {
                    i_SPO2 = convert_string_to_int(SPO2);
                    senditem->setBloodOxygen(SPO2);
                }
                senditem->setDateTime(currentDateTimeForSQL());
                mMainJobQueue.add(senditem);

#ifdef ENABLE_SEND_BODYINFO_ALERT
                check_bodyinfo_alert(projectid, sensorExisted, userLink, i_HR, i_SPO2);
#endif
            }

            if (bFoundUser)
            {
                //
                // 再發送求救推播
                //
                time_t diffTime = getCurrentTimeInSec() - userLink->alertTime;
                if (diffTime <= 600 )
                {
                    // 計算距離
                    double d_Longitude_pre = 0.0;
                    double d_Latitude_pre = 0.0;

                    if (Longitude_pre.length() > 0)
                        d_Longitude_pre = std::stof(Longitude_pre);
                    if (Latitude_pre.length() > 0)
                        d_Latitude_pre = std::stof(Latitude_pre);

                    distanceOffset = calculateDistance(d_Latitude_pre, d_Longitude_pre, d_Latitude, d_Longitude);
                    if (distanceOffset >= 0.1)
                    {
                        sendAlert = true;

                        // vilslog::printf("WebUDPParserService::sensor_info() d_Latitude[%f] d_Longitude[%f] d_Latitude_pre[%f] d_Longitude_pre[%f] distanceOffset[%f] sendAlert[%d]\n",
                        //     d_Latitude, d_Longitude, d_Latitude_pre, d_Longitude_pre, distanceOffset, sendAlert);
                    }
                }
            }
        }

        if ( sendAlert)
        {
            send_sensor_user_Alert(projectid, sensorExisted, userLink, isAlert, Latitude, Longitude, distanceOffset);
        }

        GPSInfoStruct * gpsinfo = new GPSInfoStruct();
        gpsinfo->nodetype = NODE_TYPE_SENSOR;
        gpsinfo->nodeid = did;
        gpsinfo->Longitude = d_Longitude;
        gpsinfo->Latitude = d_Latitude;
        gpsinfo->updateTimestamp = getCurrentTimeInSec();
        WorkItem * witem = new WorkItem("sensorgpsinfo", projectid, gpsinfo);
        mMySQLJobQueue.add(witem);

        if (bFoundUser)
        {
            GPSInfoStruct * gpsinfo = new GPSInfoStruct();
            gpsinfo->nodetype = NODE_TYPE_USER;
            gpsinfo->nodeid = to_string(userLink->userid);
            gpsinfo->Longitude = d_Longitude;
            gpsinfo->Latitude = d_Latitude;
            gpsinfo->updateTimestamp = getCurrentTimeInSec();
            WorkItem * witem1 = new WorkItem("sensorgpsinfo", projectid, gpsinfo);
            mMySQLJobQueue.add(witem1);
        }

        // vilslog::printf("WebUDPParserService::sensor_info() did[%s] Longitude[%f] Longitude[%f]\n", did.c_str(), gpsinfo->Longitude, gpsinfo->Latitude);

        delete gpsinfo;
    }
}

#ifdef ENABLE_SEND_BODYINFO_ALERT
void WebUDPParserService::check_bodyinfo_alert(int projectid, SensorDeviceStruct * sensorExisted, UserStruct * user, int HR, int SPO2)
{
    std::string alert_msg = "";
    std::string action = "BodyInfoAlertNormal";

    int new_alert_type = 0; // 0: normal, 1: warning 2: danger
    int HR_alert_type = 0; // 0: normal, 1: warning 2: danger
    int SPO2_alert_type = 0; // 0: normal, 1: warning 2: danger
    if ( HR >= HR_WARNING_MIN && HR <= HR_WARNING_MAX )
    {
        HR_alert_type = 1;
        action = "BodyInfoAlertWarning";
        alert_msg = "心跳高於";
        alert_msg.append(std::to_string(HR_WARNING_MIN));
    }
    else
    if ( HR >= HR_DANGER_MIN && HR <= HR_DANGER_MAX )
    {
        HR_alert_type = 2;
        action = "BodyInfoAlertDanger";
        alert_msg = "心跳高於";
        alert_msg.append(std::to_string(HR_DANGER_MIN));
    }
    else
    {
        alert_msg = "心跳小於";
        alert_msg.append(std::to_string(HR_WARNING_MIN));
    }
    new_alert_type = HR_alert_type;

    if ( SPO2 >= SPO2_WARNING_MIN && SPO2 <= SPO2_WARNING_MAX )
    {
        SPO2_alert_type = 1;
        if (new_alert_type <= 1)
        {
            action = "BodyInfoAlertWarning";
            alert_msg = "血氧低於";
            alert_msg.append(std::to_string(SPO2_WARNING_MAX));
            alert_msg.append("%");
            new_alert_type = SPO2_alert_type;
        }
    }
    else
    if ( SPO2 >= SPO2_DANGER_MIN && SPO2 <= SPO2_DANGER_MAX )
    {
        if (new_alert_type <= 1)
        {
            action = "BodyInfoAlertDanger";
            alert_msg = "血氧低於";
            alert_msg.append(std::to_string(SPO2_DANGER_MAX));
            alert_msg.append("%");
            SPO2_alert_type = 2;
            new_alert_type = SPO2_alert_type;
        }
    }
    else
    {
        if (new_alert_type < 1)
        {
            alert_msg = "血氧高於";
            alert_msg.append(std::to_string(SPO2_WARNING_MAX));
            alert_msg.append("%");
        }
    }

    vilslog::printf("WebUDPParserService::check_bodyinfo_alert() userid[%d] sensorid[%s] SPO2[%d] HR[%d]\n",
        user->userid, sensorExisted->id.c_str(), SPO2, HR);

    // send alert
    bool bFound = false;
    BodyinfoAlertStruct * curAlert = NULL;
    for (std::list<BodyinfoAlertStruct *>::iterator alert = mBodyinfoAlertList.begin(); alert != mBodyinfoAlertList.end(); alert++)
    {
        if ( (projectid == (*alert)->projectId) && (sensorExisted->id.compare((*alert)->sensorid) == 0) && (user->userid == (*alert)->userid) )
        {
            curAlert = (*alert);
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        curAlert = new BodyinfoAlertStruct();
        curAlert->projectId = projectid;
        curAlert->userid = user->userid;
        curAlert->sensorid = sensorExisted->id;
        curAlert->HR_pre = 0;
        curAlert->HR = 0;
        curAlert->HR_alert_type = 0; // 0: normal, 1: warning 2: danger
        curAlert->SPO2_pre = 0;
        curAlert->SPO2 = 0;
        curAlert->SPO2_alert_type = 0; // 0: normal, 1: warning 2: danger
        curAlert->alerttime = 0;
        mBodyinfoAlertList.push_back(curAlert);
    }

    if ( (HR_alert_type != curAlert->HR_alert_type) || (SPO2_alert_type != curAlert->SPO2_alert_type) )
    {
        time_t diffTime = getCurrentTimeInSec() - curAlert->alerttime;
        // send alert
        vilslog::printf("WebUDPParserService::check_bodyinfo_alert() userid[%d] sensorid[%s] HR_alert_type[%d] SPO2_alert_type[%d] diffTime[%d] alert_msg[%s]\n",
            curAlert->userid, curAlert->sensorid.c_str(), HR_alert_type, SPO2_alert_type, (int)diffTime, alert_msg.c_str());

        if (diffTime >= 60)
        {
            std::string eventjson = convert_to_json_str(projectid, 7777, curAlert->sensorid.c_str(), action.c_str(), alert_msg.c_str(), "0", curAlert->sensorid.c_str());

            WorkItem * witem = new WorkItem("doctorevent", projectid, NODE_TYPE_SENSOR, curAlert->sensorid.c_str(), eventjson.c_str());
            witem->setMaingroupid(sensorExisted->maingroupid);
            mMySQLJobQueue.add(witem);

            curAlert->HR_pre = curAlert->HR;
            curAlert->SPO2_pre = curAlert->SPO2;
            curAlert->HR = HR;
            curAlert->SPO2 = SPO2;
            curAlert->HR_alert_type = HR_alert_type;
            curAlert->SPO2_alert_type = SPO2_alert_type;
            curAlert->alerttime = getCurrentTimeInSec();
        }
    }
}
#endif

void WebUDPParserService::send_sensor_user_Alert(int projectid, SensorDeviceStruct * sensorExisted, UserStruct * userLink, bool isAlert, std::string Latitude, std::string Longitude, double distanceOffset)
{
    std::string eventjson = "";
    std::string did = sensorExisted->id;
    bool bFoundUser = false;

    if (sensorExisted != NULL)
    {
        std::string userid = sensorExisted->id;

        if (userLink != NULL)
        {
            bFoundUser = true;
            userid = to_string(userLink->userid);
        }

        eventjson = convert_to_json_str(projectid, 9999, userid , "Alert", "", "0", did, Latitude, Longitude);

        if (bFoundUser)
        {
            // 人員 推播
            std::string message = "";
            message.append(userLink->name);
            message.append(" 在傳感器 ");
            message.append(sensorExisted->name);
            message.append("\n按下SOS按鈕\n位置:http://www.google.com/maps/place/");
            message.append(Latitude);
            message.append(",");
            message.append(Longitude);

            if (isAlert)
            {
                userLink->alertTime = getCurrentTimeInSec();
            }

            time_t diffTime = getCurrentTimeInSec() - sensorExisted->updateLocationTime;
            std::string lastLocationTime = time_tToHumanReadString(diffTime);
            message.append("\n資料時間:");
            if (lastLocationTime.length() > 0)
            {
                message.append(lastLocationTime);
                message.append("前");
            }
            else
            {
                message.append("剛剛");
            }

            if (distanceOffset > 0 )
            {
                message.append("\n 移動:");
                char distBuf[32];
                memset(distBuf, 0, 32);
                if (distanceOffset < 1.0)
                {
                    sprintf(distBuf, "%d", (int)(distanceOffset*1000.0));
                    message.append(std::string(distBuf));
                    message.append("公尺");
                }
                else
                {
                    sprintf(distBuf, "%.02f", distanceOffset);
                    message.append(std::string(distBuf));
                    message.append("公里");
                }
            }

            std::string maingroups = convert_list_to_string(userLink->maingroups);

            // vilslog::printf("WebUDPParserService::sensor_info() maingroups[%s]\n", maingroups.c_str());

            NotificationItem * item = new NotificationItem(USER_PUSHNOTIFY_FUNCTION_1, projectid, NODE_TYPE_USER, did, sensorExisted->name, userid, maingroups, "", "人員緊急求助", message);
            mNotifiactionQueue.add(item);
        }
    }
    else
    {
        eventjson = convert_to_json_str(projectid, 9999, did , "Alert", "", "0", did, Latitude, Longitude);
    }

    int maingroupid = 1;
    if (bFoundUser)
    {
        std::list<std::string>::iterator maingroupidIt = userLink->maingroups->begin();

        maingroupid = convert_string_to_int(*maingroupidIt);
    }

    WorkItem * witem = new WorkItem("alert0D", projectid, NODE_TYPE_SENSOR, did.c_str(), eventjson.c_str());
    witem->setMaingroupid(maingroupid);
    mMySQLJobQueue.add(witem);
}

void WebUDPParserService::api_pushnotify(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        std::string key = result["key"].asString();
        std::string target = result["target"].asString();
        std::string title = result["title"].asString();
        std::string content = result["content"].asString();

        vilslog::printf("WebUDPParserService::api_pushnotify() key[%s] target[%s] title[%s] content[%s]\n",
            key.c_str(), target.c_str(), title.c_str(), content.c_str());

        TargetTopicStruct * targetTopic = NULL;
        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if ( baseDb != NULL )
        {
            targetTopic = baseDb->get_TargetTopicFromTarget(target);
        }

        if (targetTopic->userid > 0)
        {
            std::string maingroups = to_string(targetTopic->maingroupid);
            std::string userid = to_string(targetTopic->userid);

            std::list<std::string> targetlist;
            targetlist.push_back(userid);
            std::string randomKey = random_string(16);

            vilslog::printf("WebUDPParserService::api_pushnotify() userid[%d] maingroupid[%d] accountid[%s] maingroups[%s]\n",
                targetTopic->userid, targetTopic->maingroupid, targetTopic->accountid.c_str(), maingroups.c_str());

            NotificationItem * notifactionitem = new NotificationItem(MULTI_PUSHNOTIFY_FUNCTION_1, projectid, targetlist, targetTopic->accountid, userid, maingroups, title, content, randomKey);
            mNotifiactionQueue.add(notifactionitem);
        }

        if (targetTopic != NULL)
        {
            delete targetTopic;
        }
    }
}

void WebUDPParserService::updatecurriculumrecord(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        // int projectid = getProjectId(result);
        //
        // int recordid = getIntResult(result, "id");
        // int userid = getIntResult(result, "userid");
        // int curriculumid = getIntResult(result, "curriculumid");
        // std::string number = result["number"].asString();
        // std::string curriculumtypename = result["curriculumtypename"].asString();
        // std::string enddate = result["enddate"].asString();
        // std::string usermaingroups = result["usermaingroups"].asString();
        // std::string accountid = result["accountid"].asString();
        //
        // vilslog::printf("WebUDPParserService::updatecurriculumrecord() projectid[%d] recordid[%d] userid[%d] usermaingroups[%s]\n",
        //     projectid, recordid, userid, usermaingroups.c_str());

        // MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        // if (baseDb != NULL)
        // {
        //     UserSubGroupStruct * userSubGroup = baseDb->get_UserSubGroupByCurriculumRecordID(recordid);
        //     if (userSubGroup->subgroupid == -1)
        //     {
        //         // not exist
        //
        //         std::string groupname = curriculumtypename;
        //         groupname.append("[");
        //         groupname.append(number);
        //         groupname.append("]");
        //
        //
        //         // create new subgroup
        //         UserSubGroupStruct * newSubGroup = new UserSubGroupStruct();
        //         newSubGroup->subgroupid = -1;
        //         newSubGroup->subgroupname = groupname;
        //         newSubGroup->permission = groupname;
        //         newSubGroup->accountidList = new std::list<std::string>();
        //         newSubGroup->accountidList->push_back(accountid);
        //         newSubGroup->useridList = new std::list<std::string>();
        //         newSubGroup->useridList->push_back(to_string(userid));
        //         newSubGroup->type = 1;
        //         newSubGroup->curriculumrecordid = recordid;
        //         newSubGroup->validdate = enddate;
        //
        //         std::list<std::string> * maingroupList = convert_string_to_list(usermaingroups, ',');
        //         for (std::list<std::string>::iterator groupid = maingroupList->begin(); groupid != maingroupList->end(); groupid++)
        //         {
        //             // 取第一個
        //             newSubGroup->maingroupid = convert_string_to_int((*groupid));
        //             break;
        //         }
        //         maingroupList->clear();
        //         delete maingroupList;
        //
        //         baseDb->update_UserSubGroup(newSubGroup);
        //
        //         // update user info
        //         newSubGroup->subgroupid;
        //
        //         clear_UserSubGroupStruct(newSubGroup);
        //         delete newSubGroup;
        //     }
        //
        // }
    }
}

void WebUDPParserService::deletecurriculumrecord(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        int recordid = getIntResult(result, "id");

        vilslog::printf("WebUDPParserService::deletecurriculumrecord() projectid[%d] recordid[%d]\n", projectid, recordid);
    }
}

void WebUDPParserService::applycurriculumrecord(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        int userid = getIntResult(result, "userid");
        int recordid = getIntResult(result, "recordid");
        int teacheruserid = getIntResult(result, "teacheruserid");
        std::string number = result["number"].asString();
        std::string curriculumname = result["curriculumname"].asString();
        std::string approval = result["approval"].asString();
        std::string maingroups = result["maingroups"].asString();

        vilslog::printf("WebUDPParserService::applycurriculumrecord() projectid[%d] recordid[%d] userid[%d] teacheruserid[%d] number[%s] curriculumname[%s] approval[%s]\n",
            projectid, recordid, userid, teacheruserid, number.c_str(), curriculumname.c_str(), approval.c_str());

        //if (approval.compare("false") == 0)
        //{
            std::list<std::string> * sendtargetlist = NULL;
            std::list<std::string> targetlist;
            std::string title = "學生申請選課";
            std::string username = "";

            UserStruct * user = mDeviceManager->getUser(projectid, userid);
            if (user != NULL)
            {
                username = user->name;
                title = user->name;
                if (approval.compare("false") == 0)
                    title += " 已遞交申請選課";
                else
                    title += " 已成功加入課程";
            }

            // 推播給任課老師
            sendtargetlist = new std::list<std::string>();
            if (approval.compare("false") == 0)
            {
                sendtargetlist->push_back(to_string(teacheruserid));
                targetlist.push_back(to_string(teacheruserid));
            }

            std::ostringstream sendstream;

            sendstream << "課程名稱:"<<curriculumname<<endl;
            sendstream << "課程編號:"<<number<<endl;
            sendstream << "申請人:"<<username<<" ";

            std::string context = sendstream.str();

            vilslog::printf("WebUDPParserService::applycurriculumrecord() context[%s]\n", context.c_str());

            if (approval.compare("false") == 0)
            {
                // ===========================================
                // 推播給任課老師
                std::string randomKey1 = random_string(16);
                std::list<std::string> subgrouplist;
                std::list<std::string> surveylist;
                std::list<std::string> deadlinelist;
                std::string surveyid = ApplicationForm_CURRICULUM_APPLYID;
                surveylist.push_back(surveyid);
                deadlinelist.push_back(currentDateTimeForSQL(-7));
                NotificationItem * notifactionitem1 = new NotificationItem(MULTI_PUSHNOTIFY_FUNCTION_3333, projectid, sendtargetlist, targetlist, subgrouplist, surveylist, deadlinelist, user->account, to_string(userid), maingroups, title, context, randomKey1);
                notifactionitem1->m_formid = to_string(recordid);
                mNotifiactionQueue.add(notifactionitem1);

                sendtargetlist->clear();
                delete sendtargetlist;
            }
            targetlist.clear();
            // ===========================================
            // 傳送給 申請人
            std::string randomKey2 = random_string(16);
            std::list<std::string> userlist;
            userlist.push_back(to_string(userid));
            NotificationItem * notifactionitem2 = new NotificationItem(MULTI_PUSHNOTIFY_FUNCTION_3333, projectid, userlist, user->account, to_string(userid), maingroups, title, context, randomKey2);
            mNotifiactionQueue.add(notifactionitem2);
            // ===========================================

        //}
    }
}

void WebUDPParserService::updatecurriculumapplystatus(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        int userid = getIntResult(result, "userid");
        int recordid = getIntResult(result, "recordid");
        int status = getIntResult(result, "status");
        std::string number = result["number"].asString();
        std::string curriculumname = result["curriculumname"].asString();
        std::string curriculumtypename = result["curriculumtypename"].asString();
        std::string maingroups = result["maingroups"].asString();

        vilslog::printf("WebUDPParserService::updatecurriculumapplystatus() projectid[%d] userid[%d] recordid[%d] status[%d] number[%s] curriculumname[%s] curriculumtypename[%s]\n",
            projectid, userid, recordid, status, number.c_str(), curriculumname.c_str(), curriculumtypename.c_str());

        std::list<std::string> targetlist;
        // targetlist.push_back(accountid);
        targetlist.push_back(to_string(userid));
        std::string randomKey = random_string(16);
        std::string title = "選課審核結果通知";
        std::string username = "";

        UserStruct * user = mDeviceManager->getUser(projectid, userid);
        if (user != NULL)
        {
            username = user->name;
        }

        std::string singoffstatus = "簽核中";
        if (status == 1)
        {
            singoffstatus = "已簽核";
        }
        else
        if (status == 99)
        {
            singoffstatus = "未核准";
        }

        std::ostringstream sendstream;

        sendstream << "審核結果:"<<singoffstatus<<endl;
        sendstream << "課程名稱:"<<curriculumname<<endl;
        sendstream << "課程類別:"<<curriculumtypename<<endl;
        sendstream << "課程編號:"<<number<<endl;
        sendstream << "申請人:"<<username<<" ";

        std::string context = sendstream.str();

        vilslog::printf("WebUDPParserService::updatecurriculumapplystatus() context[%s]\n", context.c_str());

        NotificationItem * notifactionitem = new NotificationItem(MULTI_PUSHNOTIFY_FUNCTION_3333, projectid, targetlist, "", to_string(userid), maingroups, title, context, randomKey);
        mNotifiactionQueue.add(notifactionitem);
    }
}

void WebUDPParserService::sendAirRemainToClient(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    // vilslog::printf("WebUDPParserService::sendAirRemainToClient() parsingSuccessful[%d]\n", parsingSuccessful);
    std::ostringstream sendstream;
    sendstream<< "[";
    int count = 0;
    if ( parsingSuccessful )
    {
        // int projectid = getProjectId(result);

        std::string areaid = "";
        if ( result.isMember("areaid") )
            areaid = result["areaid"].asString();

        for (std::list<UserAirRemainStruct *>::iterator remain = mUserAirRemainList.begin(); remain != mUserAirRemainList.end(); remain++)
        {
            if ( areaid.compare((*remain)->areaid) == 0 )
            {
                if (count > 0)
                    sendstream<<",";
                sendstream<<"{\"userid\":\""<<(*remain)->userid<<"\",\"airremain\":\""<<(*remain)->airremain<<"\"}";
                count++;
            }
        }
    }
    sendstream<< "]";

    std::string sendString = sendstream.str();
    const char * sendBuf = sendString.c_str();

    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::resetAirRemain(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        // int projectid = getProjectId(result);

        std::string userid = "";
        std::string areaid = "";
        std::string remain = "";
        if ( result.isMember("userid") )
            userid = result["userid"].asString();
        if ( result.isMember("areaid") )
            areaid = result["areaid"].asString();
        if ( result.isMember("remain") )
            remain = result["remain"].asString();

        bool bFound = false;
        for (std::list<UserAirRemainStruct *>::iterator airremain = mUserAirRemainList.begin(); airremain != mUserAirRemainList.end(); airremain++)
        {
            if ( areaid.compare((*airremain)->areaid) == 0 && userid.compare((*airremain)->userid) == 0)
            {
                (*airremain)->airremain = remain;
                bFound = true;
                break;
            }
        }

        if(!bFound)
        {
            UserAirRemainStruct * newremain = new UserAirRemainStruct();
            newremain->areaid = areaid;
            newremain->userid = userid;
            newremain->airremain = remain;
            mUserAirRemainList.push_back(newremain);
        }
    }
}

void WebUDPParserService::findlocation(UDPDataItem * item, std::string payload)
{
    // vilslog::printf("WebUDPParserService::findlocation() payload[%s]\n", payload.c_str());

    std::list<GeoCodeStruct*> * GeoCodeList = getAddressGeoCode(payload);

    std::ostringstream sendstream;
    sendstream<<"[";

    for (std::list<GeoCodeStruct *>::iterator geocode = GeoCodeList->begin(); geocode != GeoCodeList->end(); geocode++)
    {
        if ( geocode != GeoCodeList->begin())
            sendstream<<",";

        sendstream<< "{\"address\":\""<<(*geocode)->formatted_address<<"\"";
        sendstream<< ",\"location\":{\"lat\":\""<<(*geocode)->location_lat<<"\",\"lng\":\""<<(*geocode)->location_lng<<"\"}";
        sendstream<< ",\"northeast\":{\"lat\":\""<<(*geocode)->northeast_lat<<"\",\"lng\":\""<<(*geocode)->northeast_lng<<"\"}";
        sendstream<< ",\"southwest\":{\"lat\":\""<<(*geocode)->southwest_lat<<"\",\"lng\":\""<<(*geocode)->southwest_lng<<"\"}";
        sendstream<< "}";
    }

    sendstream<<"]";
    std::string sendString = sendstream.str();
    const char * sendBuf = sendString.c_str();

    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
}

void WebUDPParserService::createFireFighting(UDPDataItem * item, std::string payload)
{
    Json::Value result;
    bool parsingSuccessful = parse_JSON(payload, result);

    std::ostringstream sendstream;
    sendstream<<"{";

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        int maingroupid = 1;

        std::string address = "";
        std::string lat = "";
        std::string lng = "";
        if ( result.isMember("address") )
            address = result["address"].asString();
        if ( result.isMember("lat") )
            lat = result["lat"].asString();
        if ( result.isMember("lng") )
            lng = result["lng"].asString();
        if ( result.isMember("maingroupid") )
            maingroupid = result["maingroupid"].asInt();

        vilslog::printf("WebUDPParserService::createFireFighting() projectid[%d] maingroupid[%d] address[%s] lat[%s] lng[%s]\n",
            projectid, maingroupid, address.c_str(), lat.c_str(), lng.c_str());

        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if ( baseDb != NULL )
        {
            FireFightingStruct * newCase = baseDb->createFireFightingCase(address, lat, lng, maingroupid);

            sendstream<< "\"id\":\""<<newCase->id<<"\"";
            sendstream<< ",\"projectid\":\""<<projectid<<"\"";
            sendstream<< ",\"address\":\""<<address<<"\"";
            sendstream<< ",\"casenumber\":\""<<newCase->casenumber<<"\"";
            sendstream<< ",\"event\":\"newcase\"";
            sendstream<< ",\"lat\":\""<<newCase->lat<<"\"";
            sendstream<< ",\"lng\":\""<<newCase->lng<<"\"";
            sendstream<< ",\"maingroupid\":\""<<newCase->maingroupid<<"\"";
            sendstream<< ",\"caseclosed\":\""<<newCase->caseclosed<<"\"";
        }
    }

    sendstream<<"}";
    std::string sendString = sendstream.str();
    const char * sendBuf = sendString.c_str();
    item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);

    mFireFightingPublisher->publish(sendString);
}

UserStruct * WebUDPParserService::update_sensor_user_location(int projectid, std::string deviceid, std::string Longitude, std::string Latitude)
{
    UserStruct * userLink = NULL;

    //
    // find user sensor
    //
    std::list<UserStruct*> * userList = mDeviceManager->get_User_list(projectid);

    for (std::list<UserStruct*>::iterator user = userList->begin(); user != userList->end(); user++)
    {
        for (std::list<std::string>::iterator sensor = (*user)->sensors->begin(); sensor != (*user)->sensors->end(); sensor++)
        {
            if ( (*sensor).compare(deviceid) == 0)
            {
                userLink = (*user);
                //userid = to_string((*user)->userid);

                // vilslog::printf("WebUDPParserService::sensor_info() updateUserStruct userid[%d] Latitude[%s] Longitude[%s]\n",
                //     (*user)->userid, Latitude.c_str(), Longitude.c_str());

                userLink->Latitude = Latitude;
                userLink->Longitude = Longitude;
                WorkItem * witem = new WorkItem("updateUserStruct", projectid, userLink);
                mMySQLJobQueue.add(witem);
                break;
            }
        }
    }

    return userLink;
}

UserStruct * WebUDPParserService::update_user_location(int projectid, int userid, std::string Longitude, std::string Latitude)
{
    UserStruct * userLink = NULL;

    std::list<UserStruct*> * userList = mDeviceManager->get_User_list(projectid);

    for (std::list<UserStruct*>::iterator user = userList->begin(); user != userList->end(); user++)
    {
        if ( (*user)->userid == userid )
        {
            userLink = (*user);

            vilslog::printf("WebUDPParserService::update_user_location() userid[%d] Latitude[%s] Longitude[%s]\n", userid, Latitude.c_str(), Longitude.c_str());

            userLink->Latitude = Latitude;
            userLink->Longitude = Longitude;
            WorkItem * witem = new WorkItem("updateUserStruct", projectid, userLink);
            mMySQLJobQueue.add(witem);

            break;
        }
    }

    return userLink;
}

void WebUDPParserService::setFixPosition(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    //vilslog::printf("WebUDPParserService::setFixPosition() buf[%s]\n", buf);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        if ( result.isMember("updatenode") )
        {
            if ( result["updatenode"].isArray() )
            {
                Json::Value NodeListValue = result["updatenode"];
                for(unsigned int i=0;i<NodeListValue.size();i++)
                {
                    Json::Value item = NodeListValue[i];
                    //vilslog::printf("WebUDPParserService::setFixPosition() macaddress[%s]\n", item.asCString());

                    DataItem * senditem = new DataItem("FixPosition", projectid);
                    senditem->setMacaddress(item.asCString());
                    mMainJobQueue.add(senditem);
                }
            }
        }
    }
}

void WebUDPParserService::setFixDevice(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    //vilslog::printf("WebUDPParserService::setFixDevice() buf[%s]\n", buf);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);

        if ( result.isMember("updatenode") )
        {
            if ( result["updatenode"].isArray() )
            {
                Json::Value NodeListValue = result["updatenode"];
                for(unsigned int i=0;i<NodeListValue.size();i++)
                {
                    Json::Value item = NodeListValue[i];
                    //vilslog::printf("WebUDPParserService::setFixDevice() macaddress[%s]\n", item.asCString());

                    DataItem * senditem = new DataItem("FixDevice", projectid);
                    senditem->setMacaddress(item.asCString());
                    mMainJobQueue.add(senditem);
                }
            }
        }
    }
}

void WebUDPParserService::create_new_user(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    vilslog::printf("WebUDPParserService::create_new_user() buf[%s]\n", buf);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        int userid = -1;
        std::string strUserId = "";
        std::string account = "";
        std::string title = "";
        std::string rfidcards = "";
        std::string nfccards = "";
        std::string uwbcards = "";
        std::string maingroupid = "1";

        if ( result.isMember("userid") )
        {
            if ( result["userid"].isInt() )
            {
                userid = result["userid"].asInt();
                strUserId = to_string(userid);
            }
            else
            {
                strUserId = result["userid"].asString();
                userid = convert_string_to_int(strUserId);
            }
        }
        if ( result.isMember("account") )
        {
            account = result["account"].asString();
        }
        if ( result.isMember("title") )
        {
            title = result["title"].asString();
        }
        if ( result.isMember("rfidcards") )
        {
            rfidcards = result["rfidcards"].asString();
        }
        if ( result.isMember("nfccards") )
        {
            nfccards = result["nfccards"].asString();
        }
        if ( result.isMember("uwbcards") )
        {
            uwbcards = result["uwbcards"].asString();
        }
        if ( result.isMember("maingroupid") )
        {
            maingroupid = result["maingroupid"].asString();
        }

        vilslog::printf("WebUDPParserService::create_new_user() projectid[%d] userid[%d]\n", projectid, userid);
        vilslog::printf("WebUDPParserService::create_new_user() projectid[%d] rfidcards[%s] nfccards[%s]\n", projectid, rfidcards.c_str(), nfccards.c_str());
        vilslog::printf("WebUDPParserService::create_new_user() projectid[%d] userid[%d] uwbcards[%s]\n", projectid, userid, uwbcards.c_str());

        mDeviceManager->reload_RFIDCard();
        mDeviceManager->reload_NFCCard();
        mDeviceManager->reload_User();
        mDeviceManager->clearTagUser(projectid, userid);

        if ( uwbcards.length() > 0 )
        {
            std::list<std::string> * uwbcardList = convert_string_to_list(uwbcards, ',');
            for (std::list<std::string>::iterator uwbcard = uwbcardList->begin(); uwbcard != uwbcardList->end(); uwbcard++)
            {
                // vilslog::printf("WebUDPParserService::create_new_user() projectid[%d] userid[%d] uwbcard[%s]\n", projectid, userid, (*uwbcard).c_str());

                mDeviceManager->setTagUser(projectid, (*uwbcard).c_str(), userid);
            }
        }

        //預設密碼
        mDeviceManager->update_user_password(projectid, account, userid, std::string("1234"));

        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if ( baseDb != NULL )
        {
            VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);

            std::string qrcodeText = vilssysConfig->PushNotifyPrefix;
            qrcodeText.append("_USER_");
            qrcodeText.append(strUserId);
            TargetTopicStruct * target = baseDb->get_TargetTopic(qrcodeText, std::string("USER"), strUserId, account, maingroupid);
            if (target != NULL)
            {
                vilslog::printf("WebUDPParserService::create_new_user() qrcodeText[%s] topic[%s]\n",
                    qrcodeText.c_str(), target->topic.c_str());

                delete target;
            }
        }

        WorkItem * itemU = new WorkItem("updateUserPassword", projectid);
        itemU->setAccountId(account);
        itemU->setUserId(userid);
        itemU->setUserPwd(std::string("1234"));
        mMySQLJobQueue.add(itemU);

        WorkItem * item = new WorkItem("checkcarduser", projectid);
        item->setUserId(userid);
        item->setRFIDCards(rfidcards);
        item->setNFCCards(nfccards);
        item->setUWBCards(uwbcards);
        item->setMaingroups(maingroupid);
        mMySQLJobQueue.add(item);

        DataItem * senditem = new DataItem("updateGAEuserinfo", projectid);
        senditem->setUserId(userid);
        mMainJobQueue.add(senditem);

        // DataItem * senditem = new DataItem("updatesurveyresult", projectid);
        // mMainJobQueue.add(senditem);

    }
}

void WebUDPParserService::update_user(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        int userid = -1;
        std::string account = "";
        std::string rfidcards = "";
        std::string nfccards = "";
        std::string uwbcards = "";
        std::string sensors = "";
        std::string maingroups = "1";

        if ( result.isMember("userid") )
        {
            std::string strUserId = result["userid"].asString();
            userid = convert_string_to_int(strUserId);
        }
        if ( result.isMember("account") )
        {
            account = result["account"].asString();
        }
        if ( result.isMember("rfidcards") )
        {
            rfidcards = result["rfidcards"].asString();
        }
        if ( result.isMember("nfccards") )
        {
            nfccards = result["nfccards"].asString();
        }
        if ( result.isMember("uwbcards") )
        {
            uwbcards = result["uwbcards"].asString();
        }
        if ( result.isMember("sensors") )
        {
            sensors = result["sensors"].asString();
        }
        if ( result.isMember("maingroups") )
        {
            maingroups = result["maingroups"].asString();
        }
        //vilslog::printf("WebUDPParserService::update_user() projectid[%d] userid[%d] account[%s]\n", projectid, userid, account.c_str());
        //vilslog::printf("WebUDPParserService::update_user() projectid[%d] rfidcards[%s] nfccards[%s]\n", projectid, rfidcards, nfccards);
        // vilslog::printf("WebUDPParserService::update_user() projectid[%d] userid[%d] uwbcards[%s]\n", projectid, userid, uwbcards.c_str());

        mDeviceManager->reload_RFIDCard();
        mDeviceManager->reload_NFCCard();
        mDeviceManager->reload_User();
        mDeviceManager->clearTagUser(projectid, userid);

        if ( uwbcards.length() > 0 )
        {
            std::list<std::string> * uwbcardList = convert_string_to_list(uwbcards, ',');
            for (std::list<std::string>::iterator uwbcard = uwbcardList->begin(); uwbcard != uwbcardList->end(); uwbcard++)
            {
                // vilslog::printf("WebUDPParserService::update_user() projectid[%d] userid[%d] uwbcard[%s]\n", projectid, userid, (*uwbcard).c_str());

                mDeviceManager->setTagUser(projectid, (*uwbcard).c_str(), userid);
            }
        }

        WorkItem * item = new WorkItem("checkcarduser", projectid);
        item->setUserId(userid);
        item->setRFIDCards(rfidcards);
        item->setNFCCards(nfccards);
        item->setUWBCards(uwbcards);
        item->setMaingroups(maingroups);
        mMySQLJobQueue.add(item);

        DataItem * senditem = new DataItem("updateGAEuserinfo", projectid);
        senditem->setUserId(userid);
        mMainJobQueue.add(senditem);

    }
}

void WebUDPParserService::delete_user(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        int userid = -1;
        std::string account = "";

        if ( result.isMember("userid") )
        {
            if ( result["userid"].isInt() )
            {
                userid = result["userid"].asInt();
            }
            else
            {
                std::string strUserId = result["userid"].asString();
                userid = convert_string_to_int(strUserId);
            }
        }
        if ( result.isMember("account") )
        {
            account = result["account"].asString();
        }

        //vilslog::printf("WebUDPParserService::delete_user() projectid[%d] userid[%d] account[%s]\n", projectid, userid, account.c_str());

        mDeviceManager->reload_RFIDCard();
        mDeviceManager->reload_NFCCard();
        mDeviceManager->reload_User();

        //
        // 移除卡片連結使用者
        //
        WorkItem * item = new WorkItem("removecarduser", projectid);
        item->setUserId(userid);
        mMySQLJobQueue.add(item);

        DataItem * senditem = new DataItem("updatesurveyresult", projectid);
        mMainJobQueue.add(senditem);

    }
}

void WebUDPParserService::reset_user_topic(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    vilslog::printf("WebUDPParserService::reset_user_topic() buf[%s]\n", buf);

    std::ostringstream sendstream;

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        std::string accountid = "";
        std::string maingroups = "";

        int userid = -1;
        if ( result.isMember("userid") )
        {
            userid = result["userid"].asInt();
        }
        if ( result.isMember("account") )
        {
            accountid = result["account"].asString();
        }

        TargetTopicStruct * userTopic = mDeviceManager->getUserTopic(projectid, userid, accountid);

        vilslog::printf("WebUDPParserService::reset_user_topic() projectid[%d] userid[%d] accountid[%s] topic[%s]\n",
            projectid, userid, accountid.c_str(), userTopic->topic.c_str());

        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if (baseDb != NULL)
        {
            std::string newTopic = baseDb->createNewTopic();

            userTopic->topic = newTopic;

            vilslog::printf("WebUDPParserService::reset_user_topic() new topic[%s]\n", userTopic->topic.c_str());
        }

        WorkItem * work = new WorkItem("TargetTopicStruct", projectid, userTopic);
        mMySQLJobQueue.add(work);

        delete userTopic;

        sendstream<<"{\"userid\":\""<<userid<<"\",\"account\":\""<<accountid<<"\",\"topic\":\""<< userTopic->topic <<"\"}";

        std::string sendString = sendstream.str();
        const char * sendBuf = sendString.c_str();
        item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
    }
    else
    {
        std::string sendString = sendstream.str();
        const char * sendBuf = sendString.c_str();
        item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
    }
}

void WebUDPParserService::reset_user_password(UDPDataItem * item)
{
    char * buf = item->getMsg();
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    vilslog::printf("WebUDPParserService::reset_user_password() buf[%s]\n", buf);

    std::ostringstream sendstream;

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        std::string accountid = "";
        std::string password = "";
        std::string maingroups = "";

        int userid = -1;
        if ( result.isMember("userid") )
        {
            userid = result["userid"].asInt();
        }
        if ( result.isMember("account") )
        {
            accountid = result["account"].asString();
        }
        if ( result.isMember("password") )
        {
            password = result["password"].asString();
        }

        if ( password.length() == 0 )
        {
            password = random_string(4);
        }

        vilslog::printf("WebUDPParserService::reset_user_password() projectid[%d] userid[%d] accountid[%s] new password[%s]\n",
            projectid, userid, accountid.c_str(), password.c_str());

        mDeviceManager->update_user_password(projectid, accountid, userid, password);

        WorkItem * witem = new WorkItem("updateUserPassword", projectid);
        witem->setAccountId(accountid);
        witem->setUserId(userid);
        witem->setUserPwd(password);
        mMySQLJobQueue.add(witem);

        std::list<std::string> targetlist;
        //targetlist.push_back(accountid);
        targetlist.push_back(to_string(userid));
        std::string randomKey = random_string(16);
        std::string title = "更新密碼";
        std::string context = "新密碼:";
        context.append(password);

        NotificationItem * notifactionitem = new NotificationItem(MULTI_PUSHNOTIFY_FUNCTION_1, projectid, targetlist, accountid, to_string(userid), maingroups, title, context, randomKey);
        mNotifiactionQueue.add(notifactionitem);


        sendstream<<"{\"account\":\""<<accountid<<"\",\"password\":\""<< password <<"\"}";

        std::string sendString = sendstream.str();
        const char * sendBuf = sendString.c_str();
        item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
    }
    else
    {
        std::string sendString = sendstream.str();
        const char * sendBuf = sendString.c_str();
        item->mUDPServer->sendToClient(sendBuf, (int)sendString.length(), (struct sockaddr *)&item->m_client_addr);
    }
}

void WebUDPParserService::update_rollcall_record(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    // vilslog::printf("WebUDPParserService::update_rollcall_record() buf[%s]\n", buf);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        int rollcallid = -1;
        int rollcalleventid = -1;
        std::string cardid = "";
        int userid = -1;

        if ( result.isMember("rollcallid") )
        {
            if ( result["rollcallid"].isInt() )
                rollcallid = result["rollcallid"].asInt();
            else
                rollcallid = atoi(result["rollcallid"].asCString());
        }
        if ( result.isMember("eventid") )
        {
            if ( result["eventid"].isInt() )
                rollcalleventid = result["eventid"].asInt();
            else
                rollcalleventid = atoi(result["eventid"].asCString());
        }
        if ( result.isMember("userid") )
        {
            if ( result["userid"].isInt() )
                userid = result["userid"].asInt();
            else
                userid = atoi(result["userid"].asCString());
        }
        if ( result.isMember("cardid") )
        {
            cardid = result["cardid"].asString();
        }

        CARDRollCallRecordStruct * record = new CARDRollCallRecordStruct();
        record->rollcallid = rollcallid;
        record->rollcalleventid = rollcalleventid;
        record->cardid = cardid;
        record->userid = userid;
        // record->nodeid;
        // record->rfiddeviceid;
        // record->nfclocator;
        // record->present;
        // record->datetime;
        // record->updatetime;

        DataItem * senditem = new DataItem("updaterollcallrecord", projectid, record);
        mMainJobQueue.add(senditem);
    }
}

void WebUDPParserService::update_rollcall_records(char * buf, int length)
{
    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(buf, result);

    // vilslog::printf("WebUDPParserService::update_rollcall_records() buf[%s]\n", buf);

    if ( parsingSuccessful )
    {
        int projectid = getProjectId(result);
        int rollcallid = -1;
        int rollcalleventid = -1;
        std::string cardids = "";
        std::string userids = "";

        if ( result.isMember("rollcallid") )
        {
            if ( result["rollcallid"].isInt() )
                rollcallid = result["rollcallid"].asInt();
            else
                rollcallid = atoi(result["rollcallid"].asCString());
        }
        if ( result.isMember("eventid") )
        {
            if ( result["eventid"].isInt() )
                rollcalleventid = result["eventid"].asInt();
            else
                rollcalleventid = atoi(result["eventid"].asCString());
        }
        if ( result.isMember("userids") )
        {
            userids = result["userids"].asCString();
        }
        if ( result.isMember("cardids") )
        {
            cardids = result["cardids"].asString();
        }

        std::list<std::string> * useridlist = convert_string_to_list(userids, ',');
        std::list<std::string> * cardidlist = convert_string_to_list(cardids, ',');

        // vilslog::printf("WebUDPParserService::update_rollcall_records() useridlist->size()[%d] cardidlist->size()[%d]\n",
        //     (int)useridlist->size(), (int)cardidlist->size());

        if ( useridlist->size() == cardidlist->size() )
        {
            std::list<std::string>::iterator cardid = cardidlist->begin();
            for (std::list<std::string>::iterator userid = useridlist->begin(); userid != useridlist->end(); userid++, cardid++)
            {
                // vilslog::printf("WebUDPParserService::update_rollcall_records() userid[%s] cardid[%s]\n", (*userid).c_str(), (*cardid).c_str());

                CARDRollCallRecordStruct * record = new CARDRollCallRecordStruct();
                record->rollcallid = rollcallid;
                record->rollcalleventid = rollcalleventid;
                record->cardid = (*cardid);
                record->userid = convert_string_to_int((*userid));

                DataItem * senditem = new DataItem("updaterollcallrecord", projectid, record);
                mMainJobQueue.add(senditem);
            }
        }

        useridlist->clear();
        cardidlist->clear();

        delete useridlist;
        delete cardidlist;
    }
}

void WebUDPParserService::dump_stream_nodelist_info(int projectid, std::list<NodeStruct*> * nodeList, std::ostringstream & ostream, std::string prefix)
{
    int index = 0;
    for (std::list<NodeStruct*>::iterator node = nodeList->begin(); node != nodeList->end(); node++)
    {
        ostream<<prefix<<"_INDEX_"<<index++<<"=";
        dump_stream_node_info(projectid, (*node), ostream, prefix);
    }
}

void WebUDPParserService::dump_stream_nodelist_info(int projectid, std::list<NodeStruct*> * nodeList, std::ostringstream & ostream, std::string prefix, int start_index, int count)
{
    int index = 0;
    for (std::list<NodeStruct*>::iterator node = nodeList->begin(); node != nodeList->end(); node++)
    {
        if (index >= start_index && index < (start_index + count))
        {
            ostream<<prefix<<"_INDEX_"<<index<<"=";
            dump_stream_node_info(projectid, (*node), ostream, prefix);
        }

        index++;
    }
}

void WebUDPParserService::dump_stream_node_info(int projectid, NodeStruct * node, std::ostringstream & ostream, std::string prefix)
{

    if (prefix.compare("LOC_ANCHOR") == 0)
    {
        dump_anchor_node_info(node, ostream);
    }
    else
    if (prefix.compare("LOC_TAG") == 0)
    {
        dump_tag_node_info(projectid, node, ostream);
    }
    else
    if (prefix.compare("LOC_COORD") == 0)
    {
        dump_coord_node_info(node, ostream);
    }
    else
    if (prefix.compare("LOC_LOCATOR") == 0)
    {
        dump_locator_node_info(projectid, node, ostream);
    }
}

void WebUDPParserService::dump_stream_aoalist_info(int projectid, std::list<AoANodeStruct*> * nodeList, std::ostringstream & ostream, std::string prefix)
{
    int index = 0;
    for (std::list<AoANodeStruct*>::iterator node = nodeList->begin(); node != nodeList->end(); node++)
    {
        ostream<<prefix<<"_INDEX_"<<index++<<"=";
        dump_stream_aoa_info(projectid, (*node), ostream, prefix);
    }
}

void WebUDPParserService::dump_stream_aoalist_info(int projectid, std::list<AoANodeStruct*> * nodeList, std::ostringstream & ostream, std::string prefix, int start_index, int count)
{
    int index = 0;
    for (std::list<AoANodeStruct*>::iterator node = nodeList->begin(); node != nodeList->end(); node++)
    {
        if (index >= start_index && index < (start_index + count))
        {
            ostream<<prefix<<"_INDEX_"<<index<<"=";
            dump_stream_aoa_info(projectid, (*node), ostream, prefix);
        }

        index++;
    }
}

void WebUDPParserService::dump_stream_aoa_info(int projectid, AoANodeStruct * node, std::ostringstream & ostream, std::string prefix)
{
    ostream<<node->nodeid;
    ostream<<","<<node->nodename;
    ostream<<","<<node->panid;
    ostream<<","<<node->macaddress;
    ostream<<","<<node->maplayer;
    ostream<<","<<node->posX;// 5
    ostream<<","<<node->posY;
    ostream<<","<<node->posZ;
    ostream<<","<<node->pitch; // 8
    ostream<<","<<node->roll;
    ostream<<","<<node->yaw;
    ostream<<","<<node->verifykey;
    ostream<<endl;
}


void WebUDPParserService::dump_anchor_node_info(NodeStruct * node, std::ostringstream & ostream)
{
    string validAnchors = "00";
    ostream<<node->index; // 0
    ostream<<","<<node->alive;
    ostream<<","<<node->nodetype;
    ostream<<","<<node->nodeid;
    ostream<<","<<node->tafid;
    ostream<<","<<node->nodename;//5
    ostream<<","<<node->macaddress;//6
    ostream<<","<<(int)node->posX;
    ostream<<","<<(int)node->posY;
    ostream<<","<<(int)node->posZ;
    ostream<<","<<node->voltage; // 10
    ostream<<","<<node->temperature;
    ostream<<","<<node->version;
    ostream<<","<<(int)node->buildingid;
    ostream<<","<<(int)node->buildingfloor;
    ostream<<","<<validAnchors;//15
    ostream<<","<<node->IP;//16
    ostream<<","<<node->AppVersion;//17
    ostream<<","<<node->PID;//18
    ostream<<","<<node->panid;//19
    ostream<<","<<node->maplayer; // 20
    ostream<<","<<time_tToString(node->poweronTimestamp); // 21
    ostream<<","<<time_tToString(node->poweroffTimestamp); // 22
    ostream<<","<<node->verifykey;
    ostream<<endl;
}

void WebUDPParserService::dump_tag_node_info(int projectid, NodeStruct * node, std::ostringstream & ostream)
{
    string validAnchors = "00";

    VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);

    // vilslog::printf("WebUDPParserService::dump_tag_node_info() node->nodeid[%s] AreaId[%s] AreaName[%s] AreaType0D[%d]\n",
    //    node->nodeid, node->AreaId, node->AreaName,  node->AreaType0D);

    if (strlen(node->rangingList) > 0)
    {
        validAnchors = node->rangingList;
    }

    char Latitude[32];
    char Longitude[32];
    if ( node->Latitude < 0.0 )
        sprintf(Latitude,"%d", 0);
    else
        sprintf(Latitude,"%f", node->Latitude);
    if ( node->Longitude < 0.0 )
        sprintf(Longitude,"%d", 0);
    else
        sprintf(Longitude,"%f", node->Longitude);

    //
    // check alive time
    //
    // time_t t_cur = getCurrentTimeInMilliSec();
    // time_t t_diff = t_cur - node->updateTimestamp;
    // int area_unknown = node->area_unknown;
    // if ( t_diff >= 30000 )
    // {
    //     area_unknown = 1; // 1:unknown area
    // }

    ostream<<node->index; // 0
    ostream<<","<<node->alive;
    ostream<<","<<node->nodetype;
    ostream<<","<<node->nodeid;
    ostream<<","<<node->tafid;
    ostream<<","<<node->nodename;
    ostream<<","<<node->macaddress;//6
    if ( vilssysConfig->SmoothingResult == 1)
    {
        ostream<<","<<(int)node->smoothingPosX;
        ostream<<","<<(int)node->smoothingPosY;
        ostream<<","<<(int)node->smoothingPosZ;
    }
    else
    {
        ostream<<","<<(int)node->posX;
        ostream<<","<<(int)node->posY;
        ostream<<","<<(int)node->posZ;
    }
    ostream<<","<<node->voltage; // 10
    ostream<<","<<node->temperature;
    ostream<<","<<node->version;
    ostream<<","<<(int)node->buildingid;
    ostream<<","<<(int)node->buildingfloor;
    ostream<<","<<validAnchors;//15
    ostream<<","<<node->IP;//16
    ostream<<","<<node->AppVersion;//17
    ostream<<","<<node->PID;//18

    ostream<<","<<node->AreaId;//19
    ostream<<","<<node->AreaName;//20
    ostream<<","<<node->panid;
    if (node->alert > 0)
        ostream<<","<<1;//tag:22
    else
    if (node->alert0D > 0)
        ostream<<","<<2;//tag:22
    else
        ostream<<","<<0;//tag:22
    ostream<<","<<node->AreaType0D;//tag:23
    ostream<<","<<Longitude;//tag:24
    ostream<<","<<Latitude;//tag:25
    ostream<<","<<node->exclude_rollcall_AreaId;//tag:26
    ostream<<","<<node->Opertaion_Mode;//tag:27 // 0:None 1:UWB Ranging 2:LoRa Ranging 3:GPS
    ostream<<","<<node->area_unknown;//tag:28 0: 1:unknown area
    ostream<<","<<node->gwid; // tag:29
    ostream<<","<<node->resident_serno;// link to Table Resident.serno
    ostream<<","<<node->empid;// link to Table employee.empid
    ostream<<","<<node->voltage_low;//32
    ostream<<","<<node->AreaType5Id;//33
    ostream<<","<<node->AreaType5Name;//34
    ostream<<","<<node->asleep;//35
    ostream<<","<<node->maplayer; // 36
    ostream<<","<<time_tToString(node->poweronTimestamp); // 37
    ostream<<","<<time_tToString(node->poweroffTimestamp); // 38
    ostream<<","<<node->userid; // 39
    ostream<<","<<node->activity; // 40
    ostream<<","<<node->verifykey;// 41
    ostream<<endl;
}

void WebUDPParserService::dump_coord_node_info(NodeStruct * node, std::ostringstream & ostream)
{
    string validAnchors = "00";
    ostream<<node->index; // 0
    ostream<<","<<node->alive;
    ostream<<","<<node->nodetype;
    ostream<<","<<node->nodeid;
    ostream<<","<<node->tafid;
    ostream<<","<<node->nodename;//5
    ostream<<","<<node->macaddress;//6
    ostream<<","<<(int)node->posX;
    ostream<<","<<(int)node->posY;
    ostream<<","<<(int)node->posZ;
    ostream<<","<<node->voltage; // 10
    ostream<<","<<node->temperature;
    ostream<<","<<node->version;
    ostream<<","<<(int)node->buildingid;
    ostream<<","<<(int)node->buildingfloor;
    ostream<<","<<validAnchors;//15
    ostream<<","<<node->IP;//16
    ostream<<","<<node->AppVersion;//17
    ostream<<","<<node->PID;//18
    ostream<<","<<node->panid;//19
    ostream<<","<<node->pingCount;//20
    ostream<<","<<node->time;//21
    ostream<<","<<node->blacklist;//22
    ostream<<","<<node->whitelist;//23
    ostream<<","<<node->joinRangingRange;//24
    ostream<<","<<node->joinNotRangingRange;//25
    ostream<<","<<node->maplayer; // 26
    ostream<<","<<time_tToString(node->poweronTimestamp); // 27
    ostream<<","<<time_tToString(node->poweroffTimestamp); // 28
    ostream<<","<<node->verifykey;
    ostream<<endl;
}

void WebUDPParserService::dump_locator_node_info(int projectid, NodeStruct * node, std::ostringstream & ostream)
{
    char Latitude[32];
    char Longitude[32];
    if ( node->Latitude < 0.0 )
        sprintf(Latitude,"%d", 0);
    else
        sprintf(Latitude,"%f", node->Latitude);
    if ( node->Longitude < 0.0 )
        sprintf(Longitude,"%d", 0);
    else
        sprintf(Longitude,"%f", node->Longitude);

    string validAnchors = "00";
    ostream<<node->index; // 0
    ostream<<","<<node->alive;
    ostream<<","<<node->nodetype;
    ostream<<","<<node->nodeid;
    ostream<<","<<node->tafid;
    ostream<<","<<node->nodename;
    ostream<<","<<node->macaddress;//6
    ostream<<","<<(int)node->posX;
    ostream<<","<<(int)node->posY;
    ostream<<","<<(int)node->posZ;
    ostream<<","<<node->voltage; // 10
    ostream<<","<<node->temperature;
    ostream<<","<<node->version;
    ostream<<","<<(int)node->buildingid;
    ostream<<","<<(int)node->buildingfloor;
    ostream<<","<<validAnchors;//15
    ostream<<","<<node->IP;//16
    ostream<<","<<node->AppVersion;//17
    ostream<<","<<node->PID;//18
    ostream<<","<<node->panid;
    if (node->alert > 0)
        ostream<<","<<1;//Locator:20
    else
    if (node->alert0D > 0)
        ostream<<","<<2;//Locator:20  0D alert
    else
        ostream<<","<<0;//Locator:20
    ostream<<","<<node->AreaType0D;//Locator:21
    ostream<<","<<node->LoRAMacaddress; // Locator:22
    ostream<<","<<node->gwid; // Locator:23
    ostream<<","<<node->resident_serno;// link to Table Resident.serno
    ostream<<","<<node->Opertaion_Mode;//25 1.累積傳送 2.循環 3.一般
    ostream<<","<<node->maplayer; // Locator:26
    ostream<<","<<time_tToString(node->poweronTimestamp); // Locator:27
    ostream<<","<<time_tToString(node->poweroffTimestamp); // Locator:28
    ostream<<","<<time_tToString(node->aliveTime); // Locator:29

    //char version = node->macaddress[1];
    //if ( version == '7' )
    //{
        AoANodeStruct * aoaNode = mDeviceManager->get_AoA_nodeById(projectid, node->nodeid);
        if (aoaNode != NULL)
        {
            ostream<<",p:"<<aoaNode->pitch<<" r:"<<aoaNode->roll<<" y:"<<aoaNode->yaw;// Locator:30
        }
        else
        {
            ostream<<",p:0 r:0 y:0";// Locator:30
        }
    //}
    ostream<<","<<node->enableSensorAlert; // Locator:31
    ostream<<","<<node->userid; // Locator:32
    ostream<<","<<Longitude;//Locator:33
    ostream<<","<<Latitude;//Locator:34
    ostream<<","<<node->verifykey;

    ostream<<endl;
}

void WebUDPParserService::dump_stream_tagrollcall_info(std::list<NodeStruct*> * nodeList, std::ostringstream & ostream, std::string prefix)
{
    int index = 0;
    for (std::list<NodeStruct*>::iterator node = nodeList->begin(); node != nodeList->end(); node++)
    {
        ostream<<prefix<<"_INDEX_"<<index++<<"=";
        dump_stream_rollcall_info((*node), ostream, prefix, "");
    }
}

void WebUDPParserService::dump_stream_tagrollcall_info(std::list<NodeStruct*> * nodeList, std::ostringstream & ostream, std::string prefix, int start_index, int count, std::string areaid)
{
    int index = 0;
    for (std::list<NodeStruct*>::iterator node = nodeList->begin(); node != nodeList->end(); node++)
    {
        if (index >= start_index && index <= (start_index + count))
        {
            ostream<<prefix<<"_INDEX_"<<index<<"=";
            dump_stream_rollcall_info((*node), ostream, prefix, areaid);
        }

        index++;
    }
}

void WebUDPParserService::dump_stream_rollcall_info(NodeStruct * node, std::ostringstream & ostream, std::string prefix, std::string areaid)
{
    // AreaInOutStruct data;
    // data.action = 0;
    // data.DataTime = "";
    // if (areaid.length() > 0)
    // {
    //     mDeviceManager->getAreaTagLastAction(node->projectid, areaid, node->nodeid, data);
    // }
    //
    // //
    // // check alive time
    // //
    // time_t t_cur = getCurrentTimeInMilliSec();
    // time_t t_diff = t_cur - node->updateTimestamp;
    // int i_diff = (int)t_diff;
    //
    // time_t t_action_cur = getCurrentTimeInSec();
    // //time_t t_action = data.DataTime_t;
    // time_t t_action = data.UpdateDataTime_t;
    // time_t t_action_diff = t_action_cur - t_action;
    // int i_action_diff = (int)t_action_diff;
    //
    // int AreaAction = data.action;
    // if ( (i_diff >= 30000 && i_action_diff >= 30) || (node->area_unknown == 1))
    // {
    //     AreaAction = 2; // set leave this area
    // }


    // vilslog::printf("WebUDPParserService::dump_stream_rollcall_info() nodeid[%s] \n\t t_diff[%d] t_action_diff[%d] node->area_unknown[%d] data.action[%d] AreaAction[%d]\n",
    //                 node->nodeid, (int)t_diff, (int)t_action_diff, node->area_unknown, data.action, AreaAction);

    int AreaAction = 1;// in

    ostream<<node->index; // 0
    ostream<<","<<node->alive;
    ostream<<","<<node->nodetype;
    ostream<<","<<node->nodeid;
    ostream<<","<<node->nodename;
    ostream<<","<<node->macaddress;//5
    ostream<<","<<node->AreaId;
    ostream<<","<<node->AreaName;
    ostream<<","<<AreaAction;// AreaAction
    ostream<<","<<time_tToString(node->updateTimestamp / 1000);// time, millisecond -> second
    ostream<<","<<node->asleep;
    ostream<<","<<node->AreaType5Id;
    ostream<<","<<node->AreaType5Name;
    ostream<<endl;
}

int WebUDPParserService::count_node_alive(std::list<NodeStruct*> * nodeList)
{
    int count = 0;
    for (std::list<NodeStruct*>::iterator node = nodeList->begin(); node != nodeList->end(); node++)
    {
        if ((*node)->alive > 0)
            count++;
    }
    return count;
}

int WebUDPParserService::count_gateway_alive(std::list<LoRaGatewayInfoStruct*> * nodeList)
{
    int count = 0;
    for (std::list<LoRaGatewayInfoStruct*>::iterator node = nodeList->begin(); node != nodeList->end(); node++)
    {
        if ( ((*node)->alive > 0) || ((*node)->MQTTalive > 0) || ((*node)->pingCount > 0) )
        {
            count++;
        }
    }
    return count;
}

int WebUDPParserService::getProjectId(Json::Value result)
{
    int projectid = 1;

    if ( result.isMember("project_id") )
    {
        if ( result["project_id"].isInt() )
            projectid = result["project_id"].asInt();
        else
        if ( result["project_id"].isNull() )
            projectid = 1;
        else
            projectid = atoi(result["project_id"].asCString());
    }
    return projectid;
}

int WebUDPParserService::getIntResult(Json::Value result, const char * key)
{
    int retInt = 0;

    if ( result.isMember(key) )
    {
        if ( result[key].isInt() )
            retInt = result[key].asInt();
        else
        if ( result[key].isNull() )
            retInt = 1;
        else
            retInt = atoi(result[key].asCString());
    }
    return retInt;
}

int WebUDPParserService::test_create_QRCODE()
{
    // std::string qrcodeText = "https://play.google.com/store/apps/details?id=com.smims.smims_vils_subscriber";
    // std::string qrcodefilename = "/upload/";
    // qrcodefilename.append("VILS_Android_app.bmp");

    //std::string qrcodeText = "itms-apps://itunes.apple.com/app/apple-store/id1507769058?mt=8";
    std::string qrcodeText = "https://apps.apple.com/app/apple-store/id1507769058";
    std::string qrcodefilename = "/upload/";
    qrcodefilename.append("VILS_iOS_app_test.bmp");

    std::string qrcodeOutputfile = OUTPUT_DIRECTORY;
    qrcodeOutputfile.append(qrcodefilename);

    vilslog::printf("WebUDPParserService::test_create_QRCODE() qrcodeText[%s]\n", qrcodeText.c_str());

    //
    // generate QR code bitmap file
    //
    create_qrcode(qrcodeText.c_str(), qrcodeOutputfile.c_str(), 20);

    return 0;
}

std::string WebUDPParserService::getUseRoommAreaID(int projectid, NodeStruct * node)
{
    std::string areaid = "";

    if (node->userid <= 0)
        return areaid;

    NodeStruct * locatornode = mDeviceManager->get_Locator_node_byUserid(projectid, node->userid);

    if (locatornode == NULL)
        return areaid;

    // 從 OD 取得 maplayerarea
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        std::list<SectionGroup *> * loSectionGroupList = baseDb->read_Section0DGroupList();

        for (std::list<SectionGroup *>::iterator sectiongroup = loSectionGroupList->begin(); sectiongroup != loSectionGroupList->end(); sectiongroup++)
        {
            std::string nodeid = (*sectiongroup)->nodeIDList->front();
            if ( nodeid.compare(locatornode->nodeid) == 0)
            {
                areaid = (*sectiongroup)->AreaID;
                break;
            }
        }

        clear_SectionGroup_list(loSectionGroupList);
        delete loSectionGroupList;
    }
    return areaid;
}

std::list<UserStruct*> * WebUDPParserService::getSubGroupUser(std::string accountid, std::list<UserStruct*> * alluserList)
{
    std::list<UserStruct*> * rollcallUserList = new std::list<UserStruct*>();

    std::list<std::string> * account_datagroups = NULL;
    for (std::list<UserStruct*>::iterator user = alluserList->begin(); user != alluserList->end(); user++)
    {
        if ( accountid.compare((*user)->account) == 0 )
        {
            account_datagroups = (*user)->datagroups;
            break;
        }
    }

    // 取得 人員資料
    for (std::list<UserStruct*>::iterator user = alluserList->begin(); user != alluserList->end(); user++)
    {
        bool bFound = false;
        if( account_datagroups != NULL )
        {
            for (std::list<std::string>::iterator datagroupid = account_datagroups->begin(); datagroupid != account_datagroups->end(); datagroupid++)
            {
                if ( (*datagroupid).compare("99999") == 0)
                {
                    bFound = true;
                    break;
                }

                // 尋找此人員是否屬於這個子集合
                for (std::list<std::string>::iterator subgroupid = (*user)->subgroups->begin(); subgroupid != (*user)->subgroups->end(); subgroupid++)
                {
                    if ( (*datagroupid).compare((*subgroupid)) == 0)
                    {
                        bFound = true;
                        break;
                    }
                }
                if (bFound)
                {
                    break;
                }
            }
        }

        if (bFound)
        {
            rollcallUserList->push_back((*user));
        }
    }

    return rollcallUserList;
}

std::list<UserSubGroupStruct*> * WebUDPParserService::getViewAccessSubGroup(std::string accountid, std::list<UserStruct*> * alluserList, std::list<UserSubGroupStruct*> * allSubGroupList)
{
    std::list<UserSubGroupStruct*> * rollcallSubGroupList = new std::list<UserSubGroupStruct*>();

    std::list<std::string> * account_datagroups = NULL;
    std::list<std::string> * account_subgroups = NULL;
    for (std::list<UserStruct*>::iterator user = alluserList->begin(); user != alluserList->end(); user++)
    {
        if ( accountid.compare((*user)->account) == 0 )
        {
            account_datagroups = (*user)->datagroups;
            account_subgroups = (*user)->subgroups;
            break;
        }
    }

    for (std::list<UserSubGroupStruct*>::iterator subgroup = allSubGroupList->begin(); subgroup != allSubGroupList->end(); subgroup++)
    {
        bool bFound = false;
        if( account_datagroups != NULL )
        {
            for (std::list<std::string>::iterator datagroupid = account_datagroups->begin(); datagroupid != account_datagroups->end(); datagroupid++)
            {
                if ( (*datagroupid).compare("99999") == 0)
                {
                    bFound = true;
                    break;
                }
                int int_datagroupid = convert_string_to_int((*datagroupid));

                // 尋找此子集合
                if ( (*subgroup)->subgroupid == int_datagroupid)
                {
                    bFound = true;
                    break;
                }
            }
        }

        if( account_datagroups != NULL && !bFound)
        {
            for (std::list<std::string>::iterator subgroupid = account_subgroups->begin(); subgroupid != account_subgroups->end(); subgroupid++)
            {
                int int_subgroupid = convert_string_to_int((*subgroupid));

                // 尋找此子集合
                if ( (*subgroup)->subgroupid == int_subgroupid)
                {
                    bFound = true;
                    break;
                }
            }
        }

        if (bFound)
        {
            // vilslog::printf("WebUDPParserService::getViewAccessSubGroup() found subgroupid[%d] subgroupname[%s]\n",
            //     (*subgroup)->subgroupid, (*subgroup)->subgroupname.c_str());

            rollcallSubGroupList->push_back((*subgroup));
        }
    }

    return rollcallSubGroupList;
}

std::list<USERRollCallRecordStruct*> * WebUDPParserService::getUserRollCallList(std::list<CARDRollCallRecordStruct*> * rollcallRecordList)
{
    std::list<USERRollCallRecordStruct*> * userRollCallList = new std::list<USERRollCallRecordStruct*>();

    for (std::list<CARDRollCallRecordStruct*>::iterator record = rollcallRecordList->begin(); record != rollcallRecordList->end(); record++)
    {
        bool bFound = false;
        for (std::list<USERRollCallRecordStruct*>::iterator userrollcall = userRollCallList->begin(); userrollcall != userRollCallList->end(); userrollcall++)
        {
            if ( (*record)->userid == (*userrollcall)->userid )
            {
                if ( (*record)->present == 1 )
                {
                    (*userrollcall)->present = 1;

                    if ( (*userrollcall)->datetime.length() == 0 )
                        (*userrollcall)->datetime = (*record)->updatetime;
                }

                (*userrollcall)->recordlist.push_back((*record));

                bFound = true;
                break;
            }
        }

        if (!bFound)
        {
            USERRollCallRecordStruct * newrollcall = new USERRollCallRecordStruct();
            newrollcall->userid = (*record)->userid;
            newrollcall->present = (*record)->present;
            if ( (*record)->present == 1 )
            {
                newrollcall->datetime = (*record)->updatetime;
            }
            else
            {
                newrollcall->datetime = "";
            }
            newrollcall->recordlist.push_back((*record));
            newrollcall->maingroups = (*record)->maingroups;

            userRollCallList->push_back(newrollcall);
        }
    }

    return userRollCallList;
}

std::list<GeoCodeStruct*> * WebUDPParserService::getAddressGeoCode(std::string address)
{
    std::list<GeoCodeStruct*> * GeoCodeList = new std::list<GeoCodeStruct*>();

    CURL *curl;
    CURLcode res;
    std::string readBufferLocation;

    curl = curl_easy_init();
    if(curl)
    {
        // First set the URL that is about to receive our POST. This URL can
        // just as well be a https:// URL if that is what should receive the
        // data.
        string url = "https://maps.googleapis.com/maps/api/geocode/json?language=zh-TW&address=";
        url.append(address);
        url.append("&key=AIzaSyADi8iR2-R0qiiZ8rBUG8eXcpgJAE-z8LQ");

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, QueryLocationWriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBufferLocation);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 5);

        // Perform the request, res will get the return code
        res = curl_easy_perform(curl);

        // always cleanup
	    curl_easy_cleanup(curl);
    }

    // vilslog::printf("WebUDPParserService::getAddressGeoCode() readBufferLocation[%s]\n", readBufferLocation.c_str());
/*
    {
       "results" : [
          {
             "address_components" : [
                {
                   "long_name" : "30",
                   "short_name" : "30",
                   "types" : [ "street_number" ]
                },
                {
                   "long_name" : "榮譽街",
                   "short_name" : "榮譽街",
                   "types" : [ "route" ]
                },
                {
                   "long_name" : "大福里",
                   "short_name" : "大福里",
                   "types" : [ "administrative_area_level_4", "political" ]
                },
                {
                   "long_name" : "東區",
                   "short_name" : "東區",
                   "types" : [ "administrative_area_level_3", "political" ]
                },
                {
                   "long_name" : "台南市",
                   "short_name" : "台南市",
                   "types" : [ "administrative_area_level_1", "political" ]
                },
                {
                   "long_name" : "台灣",
                   "short_name" : "TW",
                   "types" : [ "country", "political" ]
                },
                {
                   "long_name" : "701",
                   "short_name" : "701",
                   "types" : [ "postal_code" ]
                }
             ],
             "formatted_address" : "701台灣台南市東區榮譽街30號",
             "geometry" : {
                "location" : {
                   "lat" : 22.9739768,
                   "lng" : 120.2157161
                },
                "location_type" : "ROOFTOP",
                "viewport" : {
                   "northeast" : {
                      "lat" : 22.9753257802915,
                      "lng" : 120.2170650802915
                   },
                   "southwest" : {
                      "lat" : 22.9726278197085,
                      "lng" : 120.2143671197085
                   }
                }
             },
             "place_id" : "ChIJTfNnkCh0bjQRbXB3D9ShLCQ",
             "plus_code" : {
                "compound_code" : "X6F8+H7 台灣台南市東區",
                "global_code" : "7QJ2X6F8+H7"
             },
             "types" : [ "street_address" ]
          }
       ],
       "status" : "OK"
    }
*/
    Json::Reader * reader = new Json::Reader();
    Json::Value JSONresult;
    bool parsingSuccessful = false;
    try{
        parsingSuccessful = reader->parse( readBufferLocation, JSONresult);

        std::string status;
        if ( JSONresult.isMember("status") )
        {
            status = JSONresult["status"].asString();
        }

        if (status.compare("OK") == 0)
        {
            if ( JSONresult.isMember("results") )
            {
                for(unsigned int i=0;i<JSONresult["results"].size();i++)
                {
                    Json::Value tmp;
                    Json::Value item = JSONresult["results"].get((Json::UInt)i, tmp);

                    GeoCodeStruct * geocode = new GeoCodeStruct();
                    GeoCodeList->push_back(geocode);

                    if ( item.isMember("formatted_address") )
                    {
                        geocode->formatted_address = item["formatted_address"].asString();
                        // vilslog::printf("WebUDPParserService::getAddressGeoCode() formatted_address[%s]\n", formatted_address.c_str());
                    }
                    if ( item.isMember("geometry") )
                    {
                        Json::Value geometry = item["geometry"];

                        // vilslog::printf("WebUDPParserService::getAddressGeoCode() isObject()[%d]\n", item["geometry"].isObject());

                        if ( geometry.isMember("location") )
                        {
                            Json::Value location = geometry["location"];

                            if ( location.isMember("lat") )
                            {
                                geocode->location_lat = location["lat"].asDouble();
                            }
                            if ( location.isMember("lng") )
                            {
                                geocode->location_lng = location["lng"].asDouble();
                            }
                            // vilslog::printf("WebUDPParserService::getAddressGeoCode() Latitude[%f] Longitude[%f]\n", Latitude, Longitude);
                        }

                        if ( geometry.isMember("viewport") )
                        {
                            Json::Value viewport = geometry["viewport"];

                            if ( viewport.isMember("northeast") )
                            {
                                Json::Value northeast = viewport["northeast"];
                                if ( northeast.isMember("lat") )
                                {
                                    geocode->northeast_lat = northeast["lat"].asDouble();
                                }
                                if ( northeast.isMember("lng") )
                                {
                                    geocode->northeast_lng = northeast["lng"].asDouble();
                                }
                            }

                            if ( viewport.isMember("southwest") )
                            {
                                Json::Value southwest = viewport["southwest"];
                                if ( southwest.isMember("lat") )
                                {
                                    geocode->southwest_lat = southwest["lat"].asDouble();
                                }
                                if ( southwest.isMember("lng") )
                                {
                                    geocode->southwest_lng = southwest["lng"].asDouble();
                                }
                            }
                        }
                        // vilslog::printf("WebUDPParserService::findlocation() geometry[%s]\n", geometry.c_str());
                    }
                }
            }
        }
    }
    catch(exception exc)
    {
        vilslog::printf("WebUDPParserService::getAddressGeoCode() reader->parse() catch exception[%s]\n", exc.what());
    }
    delete reader;

    return GeoCodeList;
}
