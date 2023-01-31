/*
 *  MQTTProcessingService.cpp
 *
 *  Created on: 2018/10/19
 *      Author: ikki
 */

#include <string.h>
#include <sstream>
#include <iostream>
#include <math.h>

#include "MQTTProcessingService.h"

using namespace std;

MQTTProcessingService::MQTTProcessingService(wqueue<DataItem*> & MainJobQueue, wqueue<MQTTDataItem*> & pMQTTqueue, wqueue<SubscribeDataItem*> & pWebSocketqueue, wqueue<NotificationItem*> & NotifiactionQueue, wqueue<WorkItem*> & SensorInfoJobQueue, wqueue<WorkItem*> & MySQLJobQueue, wqueue<DataItem*> & PosCalJobQueue, wqueue<WorkItem*> & nodePositionQueue, wqueue<DataItem*> & inspectionJobQueue, wqueue<DataItem*> & CARDRollCallJobQueue, DeviceManager * pDevManager)
    : mMainJobQueue(MainJobQueue), mMQTTqueue(pMQTTqueue), mWebSocketqueue(pWebSocketqueue), mNotifiactionQueue(NotifiactionQueue), mSensorInfoJobQueue(SensorInfoJobQueue), mMySQLJobQueue(MySQLJobQueue), mPosCalJobQueue(PosCalJobQueue), mNodePositionQueue(nodePositionQueue), mInspectionJobQueue(inspectionJobQueue), mCARDRollCallJobQueue(CARDRollCallJobQueue), mDeviceManager(pDevManager)
{
}

bool MQTTProcessingService::status()
{
    return mInitial;
}

std::string MQTTProcessingService::statusString()
{
    return mStatus;
}

void * MQTTProcessingService::run()
{
    while(1)
    {
        try
        {
            initQueue();
            runloop();
            vilslog::printf("MQTTProcessingService::run() escape from runloop()\n");
            sleep(10);
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("MQTTProcessingService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "MQTTProcessingService()::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void MQTTProcessingService::initQueue()
{
    std::string dft_rtn = "none";

    mNFCCardPublisher = new ITPS::Publisher<DataItem*>("NFCCard", "scan");
    mAoARangingPublisher = new ITPS::Publisher<DataItem*>("AoA", "ranging");
}

void MQTTProcessingService::runloop()
{
    mInitial = false;
    mStatus = "MQTTProcessingService()::run() Initial successfully!";
    vilslog::printf ("MQTTProcessingService()::run() Initial successfully!\n");

    mInitial = true;

    waitloop();

}

void MQTTProcessingService::waitloop()
{
    // long long start1 = getSystemTime();
    // long long end1 = getSystemTime();

    while ( 1 )
    {
        MQTTDataItem * item = mMQTTqueue.removetimed(1000);

        if ( item != NULL )
        {
            // end1 = getSystemTime();
            // long long difft = end1 - start1;

            // vilslog::printf("MQTTProcessingService() difft[%d]\n", (int)difft);

#ifdef MQTT_PRINT_INCOMMING_MSG
            vilslog::printf("MQTTProcessingService() item:Type[%s] getProjectID[%d]\n", item->getType(), item->getProjectID());
            vilslog::printf("MQTTProcessingService() item:loramacaddress[%s] tagmacaddress[%s]\n", item->getLoRAMacaddress(), item->getTagMacaddress());
            vilslog::printf("MQTTProcessingService() item:Gatewayid[%s]\n", item->getGatewayid());
#endif

            char loraaddr[64];
            char tagaddr[64];
            char gatewayid[64];
            char timestamp[64];

            memset(loraaddr, 0, 64);
            memset(tagaddr, 0, 64);
            memset(gatewayid, 0, 64);
            memset(timestamp, 0, 64);

            snprintf(loraaddr,64, "%s", item->getLoRAMacaddress());
            snprintf(tagaddr, 64, "%s", item->getTagMacaddress());
            snprintf(gatewayid, 64, "%s", item->getGatewayid());
            snprintf(timestamp, 64, "%s", item->getTimestamp());
            int rssi = item->getTagRSSI();
            int projectid = item->getProjectID();

            // int maplayerid = mDeviceManager->getLoRaGatewayMapLayer(projectid, gatewayid);
            // if (maplayerid != 0)
            // {
            //     mDeviceManager->setTagInfoMapLayer(projectid, tagaddr, maplayerid);
            //     mDeviceManager->setLocatorInfoMapLayerByLoRAmacaddress(projectid, loraaddr, maplayerid);
            // }

            if ( strcmp(item->getType(), "alert") == 0 )
            {
                // alert
                process_alert(projectid, loraaddr, tagaddr, gatewayid);
                //printf("MQTTProcessingService() process_alert() finished!\n");
            }
            else
            if ( strcmp(item->getType(), "ranging") == 0 )
            {
                // ranging
                process_ranging(projectid, loraaddr, tagaddr, item->getRanging(), item->getTagVoltage(), gatewayid, rssi, timestamp);
            }
            else
            if ( strcmp(item->getType(), "rangingcompress") == 0 )
            {
                // rangingcompress
                //int imu[6] = {0,0,0,0,0,0};
                //item->getTagIMU(imu);
                process_rangingcompress(projectid, loraaddr, tagaddr, item->getRanging(), item->getTagVoltage(), item->getTagVelocity(), gatewayid, rssi, timestamp);
            }
            else
            if ( strcmp(item->getType(), "rangingcompressMultiDistance") == 0 )
            {
                process_rangingcompressMultiDistance(projectid, loraaddr, tagaddr, item->getDistInterval(), &item->m_distList, item->getTagVoltage(), item->getTagVelocity(), gatewayid, rssi, timestamp);
            }
            else
            if ( strcmp(item->getType(), "rangingcompressaoa") == 0 )
            {
                int IMU[6];
                item->getIMU(IMU);
                process_rangingcompressaoa(projectid, loraaddr, tagaddr, item->getRanging(), item->getTagVoltage(), item->getTagVelocity(), item->getAngle(), gatewayid, rssi, IMU, timestamp);
            }
            else
            if ( strcmp(item->getType(), "cancel") == 0 )
            {
                process_cancel(projectid, loraaddr, tagaddr, gatewayid);
            }
            else
            if ( strcmp(item->getType(), "taggps") == 0 )
            {
                double Longitude = item->getLongitude();
                double Latitude = item->getLatitude();
                process_taggps(projectid, loraaddr, Longitude, Latitude, gatewayid, timestamp);
            }
            else
            if ( strcmp(item->getType(), "locatorgps") == 0 )
            {
                double Longitude = item->getLongitude();
                double Latitude = item->getLatitude();
                process_locatorgps(projectid, loraaddr, Longitude, Latitude, gatewayid, timestamp);
            }
            else
            if ( strcmp(item->getType(), "loragateway") == 0 )
            {
                process_loragateway(projectid, &item->mLoRaGateway);
            }
            else
            if ( strcmp(item->getType(), "MQTTgateway") == 0 )
            {
                process_MQTTgateway(projectid, &item->mLoRaGateway);
            }
            else
            if ( strcmp(item->getType(), "LoRaDeviceALIVE") == 0 )
            {
                //
                // check of locator exist
                //
                checkLocatorExisted(projectid, loraaddr);

                // vilslog::printf("MQTTProcessingService()::waitloop() LoRaDeviceALIVE loraaddr[%s]\n", loraaddr);

                char locatorID[64];
                snprintf(locatorID, 64, "%s", mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, loraaddr));
                mDeviceManager->setLocatorInfoGatewayID(projectid, loraaddr, gatewayid);
                mDeviceManager->setLocatorAliveByLoRAmacaddress(projectid, loraaddr);
                mDeviceManager->setLocatorInfoGatewayRSSI(projectid, loraaddr, gatewayid, item->getTagRSSI());
                mDeviceManager->setAoAInfoGridSize(projectid, locatorID, item->getAOA_gridsize());

                mDeviceManager->setRFIDDeviceAlive(projectid, loraaddr);
                mDeviceManager->setNFCDeviceAlive(projectid, loraaddr);
            }
            else
            if ( strcmp(item->getType(), "LoRaDeviceBOOT") == 0 )
            {
                char macaddress[64];
                snprintf(macaddress, 64, "%s", item->getMacaddress());

                //
                // check of locator exist
                //
                if ( strlen(macaddress) > 0)
                    checkLocatorExisted(projectid, loraaddr, macaddress);
                else
                    checkLocatorExisted(projectid, loraaddr);

                char locatorID[64];
                snprintf(locatorID, 64, "%s", mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, loraaddr));
                mDeviceManager->setLocatorInfoGatewayID(projectid, loraaddr, gatewayid);
                mDeviceManager->setLocatorInfoBoot(projectid, loraaddr, item->getPANID(), item->getMODE(), item->getVERSION());
                mDeviceManager->setLocatorAliveByLoRAmacaddress(projectid, loraaddr);
            }
            else
            if ( strcmp(item->getType(), "LoRaDevicePOWEROFF") == 0 )
            {
                char macaddress[64];
                snprintf(macaddress, 64, "%s", item->getMacaddress());

                //
                // check of locator exist
                //
                checkLocatorExisted(projectid, loraaddr, macaddress);

                char locatorID[64];
                snprintf(locatorID, 64, "%s", mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, loraaddr));
                mDeviceManager->setLocatorInfoGatewayID(projectid, loraaddr, gatewayid);

                mDeviceManager->setLocatorInfoPowerOff(projectid, loraaddr);

                //========================================================
                NodeStruct * node = mDeviceManager->get_Locator_node(projectid, loraaddr);
                if (node != NULL)
                {
                    std::string action = "PowerOff";
                    std::string eventjson = convert_to_json_str(projectid, 7777, node->nodeid, action, "", "0", node->nodeid);

                    WorkItem * witem = new WorkItem("PowerOff", projectid, NODE_TYPE_LOCATOR, node->macaddress, eventjson.c_str());
                    witem->setMaingroupid(node->maingroupid);
                    mMySQLJobQueue.add(witem);

                    DataItem * inspecitem = new DataItem("PowerOff", projectid, node->nodeid);
                    mInspectionJobQueue.add(inspecitem);

                    int alertType = 2; //1:人員 2:固定裝置
                    int alertAction = 5;// 1:In 2:Out 3:alert 4:cancel 5:poweroff
                    SubscribeDataItem * subitem = new SubscribeDataItem("event", projectid, 7777, node->nodeid, alertType, alertAction);
                    subitem->setMaingroupid(node->maingroupid);
                    mWebSocketqueue.add(subitem);

                    int nodetype = NODE_TYPE_LOCATOR;
                    std::string nodeid = locatorID;
                    std::string sendMessage = "[";
                    sendMessage.append(currentDateTimeForSQL());
                    sendMessage.append("]-[");
                    sendMessage.append(node->nodename);
                    sendMessage.append("]-裝置目前斷電！");

                    NotificationItem * item = new NotificationItem(LOCATOR_PUSHNOTIFY_FUNCTION_5, projectid, nodetype, nodeid, node->nodename, "0", "1", macaddress, "SMIMSVILS 斷電警報", sendMessage);
                    mNotifiactionQueue.add(item);
                }
                //========================================================

            }
            else
            if ( strcmp(item->getType(), "LoRaDevicePOWERON") == 0 )
            {
                char macaddress[64];
                snprintf(macaddress, 64, "%s", item->getMacaddress());

                //
                // check of locator exist
                //
                checkLocatorExisted(projectid, loraaddr, macaddress);

                char locatorID[64];
                snprintf(locatorID, 64, "%s", mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, loraaddr));
                mDeviceManager->setLocatorInfoGatewayID(projectid, loraaddr, gatewayid);
                mDeviceManager->setLocatorInfoPowerOn(projectid, loraaddr);

                //========================================================
                NodeStruct * node = mDeviceManager->get_Locator_node(projectid, loraaddr);
                if (node != NULL)
                {
                    vilslog::printf("MQTTProcessingService::waitloop() LoRaDevicePOWERON loraaddr[%s] Locator_node != NULL\n", loraaddr);

                    std::string action = "PowerOn";
                    std::string eventjson = convert_to_json_str(projectid, 7777, node->nodeid, action, "", "0", node->nodeid);

                    WorkItem * witem = new WorkItem("PowerOn", projectid, NODE_TYPE_LOCATOR, node->macaddress, eventjson.c_str());
                    witem->setMaingroupid(node->maingroupid);
                    mMySQLJobQueue.add(witem);

                    int alertType = 2; //1:人員 2:固定裝置
                    int alertAction = 5;// 1:In 2:Out 3:alert 4:cancel 5:poweroff
                    SubscribeDataItem * subitem = new SubscribeDataItem("event", projectid, 7777, node->nodeid, alertType, alertAction);
                    subitem->setMaingroupid(node->maingroupid);
                    mWebSocketqueue.add(subitem);
                }
                //========================================================

            }
            else
            if ( strcmp(item->getType(), "NFCLocatorPOWERON") == 0 )
            {
                char macaddress[64];
                snprintf(macaddress, 64, "%s", item->getMacaddress());

                // vilslog::printf("MQTTProcessingService::waitloop() NFCLocatorPOWERON macaddress[%s]\n", macaddress);

                //========================================================
                NFCDeviceStruct * nfcDevice = mDeviceManager->get_NFCDevice(projectid, macaddress);
                if ( nfcDevice != NULL )
                {
                    vilslog::printf("MQTTProcessingService::waitloop() NFCLocatorPOWERON macaddress[%s] nfcDevice != NULL\n", macaddress);

                    std::string action = "PowerOn";
                    std::string eventjson = convert_to_json_str(projectid, 7777, macaddress, action, "", "0", macaddress);

                    WorkItem * witem = new WorkItem("PowerOn", projectid, NODE_TYPE_NFC_LOCATOR, macaddress, eventjson.c_str());
                    witem->setMaingroupid(nfcDevice->maingroupid);
                    mMySQLJobQueue.add(witem);

                    int alertType = 2; //1:人員 2:固定裝置
                    int alertAction = 5;// 1:In 2:Out 3:alert 4:cancel 5:poweroff/on
                    SubscribeDataItem * subitem = new SubscribeDataItem("event", projectid, 7777, macaddress, alertType, alertAction);
                    subitem->setMaingroupid(nfcDevice->maingroupid);
                    mWebSocketqueue.add(subitem);
                }
                //========================================================
            }
            else
            if ( strcmp(item->getType(), "Heart") == 0 )
            {
                checkLocatorExisted(projectid, loraaddr);

                std::string HeartData = item->getData();
                //vilslog::printf("MQTTProcessingService::waitloop() setLocatorInfoHeartBeat loraaddr[%s] HeartData[%s]\n", loraaddr, HeartData.c_str());
                char locatorID[64];
                snprintf(locatorID, 64, "%s", mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, loraaddr));
                mDeviceManager->setLocatorInfoGatewayID(projectid, loraaddr, gatewayid);
                mDeviceManager->setLocatorInfoHeartBeat(projectid, loraaddr, HeartData.c_str());
                mDeviceManager->setLocatorAliveByLoRAmacaddress(projectid, loraaddr);
            }
            else
            if ( strcmp(item->getType(), "Weight") == 0 )
            {
                checkLocatorExisted(projectid, loraaddr);

                std::string WeightData = item->getData();
                //vilslog::printf("MQTTProcessingService::waitloop() setLocatorInfoWeight loraaddr[%s] WeightData[%s]\n", loraaddr, WeightData.c_str());

                char locatorID[64];
                snprintf(locatorID, 64, "%s", mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, loraaddr));
                mDeviceManager->setLocatorInfoGatewayID(projectid, loraaddr, gatewayid);
                mDeviceManager->setLocatorInfoWeight(projectid, loraaddr, WeightData.c_str());
                mDeviceManager->setLocatorAliveByLoRAmacaddress(projectid, loraaddr);
            }
            else
            if ( strcmp(item->getType(), "TagHeart") == 0 )
            {
                checkLocatorExisted(projectid, loraaddr);
                char locatorID[64];
                snprintf(locatorID, 64, "%s", mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, loraaddr));

                std::string HeartData = item->getData();
                int HeartBeat = atoi(HeartData.c_str());

                vilslog::printf("MQTTProcessingService::waitloop() setTagInfoHeartBeat TagMacaddress[%s] HeartData[%s] HeartBeat[%d] locatorID[%s]\n",
                    item->getTagMacaddress(), HeartData.c_str(), HeartBeat, locatorID);

                mDeviceManager->setTagHeartBeat(projectid, item->getTagMacaddress(), HeartBeat);
                mDeviceManager->setTagInfoRangingTarget(projectid, item->getTagMacaddress(), locatorID);
            }
            else
            if ( strcmp(item->getType(), "TagWeight") == 0 )
            {
                checkLocatorExisted(projectid, loraaddr);
                char locatorID[64];
                snprintf(locatorID, 64, "%s", mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, loraaddr));

                std::string WeightData = item->getData();
                int weight = (int)(atof(WeightData.c_str()) * 100.0f);

                vilslog::printf("MQTTProcessingService::waitloop() setTagInfoWeight TagMacaddress[%s] HeartData[%s] weight[%d] locatorID[%s]\n",
                    item->getTagMacaddress(), WeightData.c_str(), weight, locatorID);

                mDeviceManager->setTagWeight(projectid, item->getTagMacaddress(), weight);
                mDeviceManager->setTagInfoRangingTarget(projectid, item->getTagMacaddress(), locatorID);
            }
            else
            if ( strcmp(item->getType(), "OPENDOOR") == 0 )
            {
                vilslog::printf("MQTTProcessingService::waitloop() OPENDOOR loraaddr[%s] TagMacaddress[%s]\n", loraaddr, item->getTagMacaddress());

                checkLocatorExisted(projectid, loraaddr);

                NodeStruct * locatorNode = mDeviceManager->get_Locator_node(projectid, loraaddr);

                char tagID[64];
                snprintf(tagID, 64, "%s", mDeviceManager->getTagId(projectid, item->getTagMacaddress()));

                mDeviceManager->setTagAlive(projectid, item->getTagMacaddress());
                mDeviceManager->setLocatorInfoGatewayID(projectid, loraaddr, gatewayid);
                mDeviceManager->setLocatorAliveByLoRAmacaddress(projectid, loraaddr);

                std::string action = "OpenDoor";
                std::string eventjson = convert_to_json_str(projectid, 7777, tagID, action, "", "0", locatorNode->nodeid);

                WorkItem * witem = new WorkItem("OpenDoor", projectid, NODE_TYPE_LOCATOR, locatorNode->macaddress, eventjson.c_str());
                witem->setMaingroupid(locatorNode->maingroupid);
                mMySQLJobQueue.add(witem);

                int alertType = 2; //1:人員 2:固定裝置
                int alertAction = 1;// 1:In 2:Out 3:alert 4:cancel 5:poweroff
                SubscribeDataItem * subitem = new SubscribeDataItem("event", projectid, 7777, locatorNode->nodeid, alertType, alertAction);
                subitem->setMaingroupid(locatorNode->maingroupid);
                mWebSocketqueue.add(subitem);

            }
            else
            if ( strcmp(item->getType(), "CLOSEDOOR") == 0 )
            {
                vilslog::printf("MQTTProcessingService::waitloop() CLOSEDOOR loraaddr[%s]\n", loraaddr);

                checkLocatorExisted(projectid, loraaddr);

                NodeStruct * locatorNode = mDeviceManager->get_Locator_node(projectid, loraaddr);

                mDeviceManager->setLocatorInfoGatewayID(projectid, loraaddr, gatewayid);
                mDeviceManager->setLocatorAliveByLoRAmacaddress(projectid, loraaddr);

                std::string action = "CloseDoor";
                std::string eventjson = convert_to_json_str(projectid, 7777, locatorNode->nodeid, action, "", "0", locatorNode->nodeid);

                WorkItem * witem = new WorkItem("CloseDoor", projectid, NODE_TYPE_LOCATOR, locatorNode->macaddress, eventjson.c_str());
                witem->setMaingroupid(locatorNode->maingroupid);
                mMySQLJobQueue.add(witem);

                int alertType = 2; //1:人員 2:固定裝置
                int alertAction = 2;// 1:In 2:Out 3:alert 4:cancel 5:poweroff
                SubscribeDataItem * subitem = new SubscribeDataItem("event", projectid, 7777, locatorNode->nodeid, alertType, alertAction);
                subitem->setMaingroupid(locatorNode->maingroupid);
                mWebSocketqueue.add(subitem);
            }
            else
            if ( strcmp(item->getType(), "GPIO_ON") == 0 )
            {
                checkLocatorExisted(projectid, loraaddr);

                NodeStruct * locatorNode = mDeviceManager->get_Locator_node(projectid, loraaddr);

                vilslog::printf("MQTTProcessingService::waitloop() GPIO_ON loraaddr[%s] enableSensorAlert[%d]\n", loraaddr, locatorNode->enableSensorAlert);

                mDeviceManager->setLocatorInfoGatewayID(projectid, loraaddr, gatewayid);
                mDeviceManager->setLocatorAliveByLoRAmacaddress(projectid, loraaddr);

                if ( locatorNode->enableSensorAlert == 1 )
                {
                    std::string action = "SensorAlert";
                    std::string eventjson = convert_to_json_str(projectid, 7777, locatorNode->nodeid, action, "", "0", locatorNode->nodeid);

                    WorkItem * witem = new WorkItem("SensorAlert", projectid, NODE_TYPE_LOCATOR, locatorNode->macaddress, eventjson.c_str());
                    witem->setMaingroupid(locatorNode->maingroupid);
                    mMySQLJobQueue.add(witem);

                    DataItem * inspecitem = new DataItem("SensorAlert", projectid, locatorNode->nodeid);
                    mInspectionJobQueue.add(inspecitem);

                    int alertType = 2; //1:人員 2:固定裝置
                    int alertAction = 2;// 1:In 2:Out 3:alert 4:cancel 5:poweroff
                    SubscribeDataItem * subitem = new SubscribeDataItem("event", projectid, 7777, locatorNode->nodeid, alertType, alertAction);
                    subitem->setMaingroupid(locatorNode->maingroupid);
                    mWebSocketqueue.add(subitem);


                    int nodetype = NODE_TYPE_LOCATOR;
                    std::string nodeid = locatorNode->nodeid;
                    std::string sendMessage = "[";
                    sendMessage.append(currentDateTimeForSQL());
                    sendMessage.append("]-[");
                    sendMessage.append(locatorNode->nodename);
                    sendMessage.append("]-周邊模組觸發警報");

                    NotificationItem * item = new NotificationItem(LOCATOR_PUSHNOTIFY_FUNCTION_6, projectid, nodetype, locatorNode->nodeid, locatorNode->nodename, "0", "1", locatorNode->macaddress, "SMIMSVILS 周邊模組警報", sendMessage);
                    mNotifiactionQueue.add(item);
                }
            }
            else
            if ( strcmp(item->getType(), "NFCDevice") == 0 )
            {
                // vilslog::printf ("MQTTProcessingService::waitloop() NFCDevice macaddress[%s] name[%s] type[%s] datetime[%s]\n",
                //     item->m_NFCDevice.macaddress.c_str(), item->m_NFCDevice.name.c_str(), item->m_NFCDevice.type.c_str(), item->m_NFCDevice.datetime.c_str());

                NFCDeviceStruct * nfcdevice = mDeviceManager->get_NFCDevice(projectid, item->m_NFCDevice.macaddress.c_str());
                if (nfcdevice == NULL)
                {
                    item->m_NFCDevice.deviceid = -1;

                    mDeviceManager->setNFCDeviceAlive(projectid, &item->m_NFCDevice);

                    WorkItem * witem = new WorkItem("NFCDevice", projectid, &item->m_NFCDevice);
                    mMySQLJobQueue.add(witem);

                    sleep(1);
                    //mDeviceManager->reload_NFCDevice();
                    DataItem * senditem = new DataItem("reloadnfcdevice", projectid);
                    mMainJobQueue.add(senditem);
                }
                else
                {
                    item->m_NFCDevice.deviceid = nfcdevice->deviceid;
                    if (nfcdevice->name.length() > 0)
                        item->m_NFCDevice.name = nfcdevice->name;
                    if (item->m_NFCDevice.type.length() > 0)
                        nfcdevice->type = item->m_NFCDevice.type;
                    if (item->m_NFCDevice.version.length() > 0)
                        nfcdevice->version = item->m_NFCDevice.version;

                    item->m_NFCDevice.maingroupid = nfcdevice->maingroupid;
                    item->m_NFCDevice.verifykey = nfcdevice->verifykey;

                    // if (nfcdevice->name.length() > 0)
                    //     item->m_NFCDevice.name = nfcdevice->name;
                    // if (nfcdevice->type.length() > 0)
                    //     item->m_NFCDevice.type = nfcdevice->type;

                    // replace with old one
                    if ((nfcdevice->ssid.length() > 0) && (item->m_NFCDevice.ssid.length() == 0 ))
                    {
                        item->m_NFCDevice.ssid = nfcdevice->ssid;
                    }

                    if ((nfcdevice->wifimac.length() > 0) && (item->m_NFCDevice.wifimac.length() == 0 ))
                    {
                        item->m_NFCDevice.wifimac = nfcdevice->wifimac;
                    }

                    mDeviceManager->setNFCDeviceAlive(projectid, &item->m_NFCDevice);

                    WorkItem * witem = new WorkItem("NFCDevice", projectid, &item->m_NFCDevice);
                    mMySQLJobQueue.add(witem);
                }
            }
            else
            if ( strcmp(item->getType(), "RFIDDevice") == 0 )
            {
                // vilslog::printf ("MQTTProcessingService::waitloop() RFIDDevice macaddress[%s] name[%s] type[%s] datetime[%s]\n",
                //     item->m_RFIDDevice.macaddress.c_str(), item->m_RFIDDevice.name.c_str(), item->m_RFIDDevice.type.c_str(), item->m_RFIDDevice.datetime.c_str());

                RFIDDeviceStruct * rfiddevice = mDeviceManager->get_RFIDDevice(projectid, item->m_RFIDDevice.macaddress.c_str());
                if (rfiddevice == NULL)
                {
                    vilslog::printf ("MQTTProcessingService::waitloop() rfiddevice == NULL\n");

                    item->m_RFIDDevice.deviceid = -1;

                    mDeviceManager->setRFIDDeviceAlive(projectid, &item->m_RFIDDevice);

                    WorkItem * witem = new WorkItem("RFIDDevice", projectid, &item->m_RFIDDevice);
                    mMySQLJobQueue.add(witem);

                    sleep(1);
                    //mDeviceManager->reload_RFIDDevice();
                    DataItem * senditem = new DataItem("reloadrfiddevice", projectid);
                    mMainJobQueue.add(senditem);
                }
                else
                {
                    // vilslog::printf ("MQTTProcessingService::waitloop() rfiddevice->deviceid[%d]\n", rfiddevice->deviceid);

                    item->m_RFIDDevice.deviceid = rfiddevice->deviceid;
                    if (rfiddevice->name.length() > 0)
                        item->m_RFIDDevice.name = rfiddevice->name;
                    if (rfiddevice->type.length() > 0)
                        item->m_RFIDDevice.type = rfiddevice->type;

                    item->m_RFIDDevice.maingroupid = rfiddevice->maingroupid;

                    mDeviceManager->setRFIDDeviceAlive(projectid, &item->m_RFIDDevice);

                    WorkItem * witem = new WorkItem("RFIDDevice", projectid, &item->m_RFIDDevice);
                    mMySQLJobQueue.add(witem);
                }
            }
            else
            if ( strcmp(item->getType(), "rfidcard") == 0 )
            {
                RFIDDeviceStruct * rfiddevice = mDeviceManager->get_RFIDDeviceByLoRAmacaddress(projectid, item->getLoRAMacaddress());
                RFIDCardStruct * rfidcard = mDeviceManager->get_RFIDCard(projectid, item->m_RFIDCard.cardid.c_str());
                if (rfidcard == NULL)
                {
                    item->m_RFIDCard.alive = 1;
                    item->m_RFIDCard.maplayer = 0;
                    item->m_RFIDCard.userid = 0;
                    item->m_RFIDCard.unknown = 1;

                    //
                    // 發現未註冊卡片
                    //
                    //create_unknown_card_event(projectid, "rfidcard", item->m_RFIDCard.cardid, item->getLoRAMacaddress());
                }
                else
                {
                    mDeviceManager->setRFIDCardAlive(projectid, &item->m_RFIDCard);

                    item->m_RFIDCard.alive = rfidcard->alive;
                    item->m_RFIDCard.name = rfidcard->name;
                    item->m_RFIDCard.type = rfidcard->type;
                    item->m_RFIDCard.maplayer = rfidcard->maplayer;
                    item->m_RFIDCard.userid = rfidcard->userid;
                    item->m_RFIDCard.unknown = rfidcard->unknown;
                    item->m_RFIDCard.deviceid = rfidcard->deviceid;
                    item->m_RFIDCard.maingroupid = rfidcard->maingroupid;

                    DataItem * carditem = new DataItem("RFIDCard", projectid);
                    carditem->setCardid(item->m_RFIDCard.cardid);
                    carditem->setLoRAMacaddress(item->getLoRAMacaddress());
                    carditem->setGatewayid(item->getGatewayid());
                    mCARDRollCallJobQueue.add(carditem);

                    // DataItem * carditem1 = new DataItem("RFIDCard", projectid);
                    // carditem1->setCardid(item->m_RFIDCard.cardid);
                    // carditem1->setLoRAMacaddress(item->getLoRAMacaddress());
                    // carditem1->setGatewayid(item->getGatewayid());
                    // mRFIDCardPublisher->publish(carditem1);

                    // mDeviceManager->addCardScanRecord(projectid, item->m_RFIDCard.cardid, item->getLoRAMacaddress());
                }

                // vilslog::printf("MQTTProcessingService::waitloop() RFIDCard gwid[%s] loramacaddress[%s] datetime[%s] unknown[%d] cardid[%s]\n",
                //     item->getGatewayid(), item->getLoRAMacaddress(), item->m_RFIDCard.datetime.c_str(), item->m_RFIDCard.unknown, item->m_RFIDCard.cardid.c_str());

                int deviceid = mDeviceManager->setRFIDDeviceAlive(projectid, item->getLoRAMacaddress());
                item->m_RFIDCard.deviceid = deviceid;
                if ( rfiddevice != NULL )
                {
                    item->m_RFIDCard.maingroupid = rfiddevice->maingroupid;
                }
                // vilslog::printf ("MQTTProcessingService::waitloop() rfidcard cardid[%s] datetime[%s] LoRAMacaddress[%s] deviceid[%d] unknown[%d] maplayer[%d]\n",
                //     item->m_RFIDCard.cardid.c_str(), item->m_RFIDCard.datetime.c_str(), item->getLoRAMacaddress(), deviceid, item->m_RFIDCard.unknown, item->m_RFIDCard.maplayer);

                WorkItem * witem = new WorkItem("RFIDCardStruct", projectid, &item->m_RFIDCard);
                mMySQLJobQueue.add(witem);

                //====================================
                CARDScanRecordStruct cardscan;
                cardscan.cardid = item->m_RFIDCard.cardid;
                cardscan.cardtype = 1; // 0:NFC 1:UHF
                cardscan.LoRAMacaddress = item->getLoRAMacaddress();
                cardscan.ssid = "";
                cardscan.wifimac = "";
                if ( item->m_RFIDCard.datetime.length() > 0 )
                    cardscan.datetime = item->m_RFIDCard.datetime;
                else
                    cardscan.datetime = currentDateTimeForSQL();


                WorkItem * senditem = new WorkItem("CARDScanRecordStruct", projectid, &cardscan);
                senditem->setMaingroupid(item->m_RFIDCard.maingroupid);
                mMySQLJobQueue.add(senditem);
                //====================================

                //====================================
                SubscribeDataItem * subitem = new SubscribeDataItem("cardrollcallevent", projectid);
                subitem->m_readerid = deviceid;
                subitem->m_cardtype = 1; // 0:NFC 1:UHF
                subitem->m_cardid = item->m_RFIDCard.cardid;
                subitem->setMaingroupid(item->m_RFIDCard.maingroupid);
                mWebSocketqueue.add(subitem);
                //====================================
            }
            else
            if ( strcmp(item->getType(), "nfccard") == 0 )
            {
                // vilslog::printf ("MQTTProcessingService::waitloop() nfccard card cardid[%s] datetime[%s]\n",
                //     item->m_NFCCard.cardid.c_str(), item->m_NFCCard.datetime.c_str());

                NFCDeviceStruct * nfcdevice = mDeviceManager->get_NFCDeviceByLoRAmacaddress(projectid, item->getLoRAMacaddress());
                NFCCardStruct * nfccard = mDeviceManager->get_NFCCard(projectid, item->m_NFCCard.cardid.c_str());
                if (nfccard == NULL)
                {
                    item->m_NFCCard.alive = 1;
                    item->m_NFCCard.name = item->m_NFCCard.cardid;
                    item->m_NFCCard.type = "1";
                    item->m_NFCCard.maplayer = 0;
                    item->m_NFCCard.userid = 0;
                    item->m_NFCCard.unknown = 1;

                    // vilslog::printf ("MQTTProcessingService::waitloop() nfccard == NULL item->m_NFCCard.name[%s]\n",
                    //     item->m_NFCCard.name.c_str());


                    //
                    // 發現未註冊卡片
                    //
                    if ( nfcdevice != NULL )
                        create_unknown_card_event(projectid, "nfccard", item->m_NFCCard.cardid, item->getLoRAMacaddress(), nfcdevice->maingroupid);
                    else
                        create_unknown_card_event(projectid, "nfccard", item->m_NFCCard.cardid, item->getLoRAMacaddress(), 1);

                }
                else
                {
                    mDeviceManager->setNFCCardAlive(projectid, &item->m_NFCCard);

                    item->m_NFCCard.alive = nfccard->alive;
                    item->m_NFCCard.name = nfccard->name;
                    item->m_NFCCard.type = nfccard->type;
                    item->m_NFCCard.maplayer = nfccard->maplayer;
                    item->m_NFCCard.userid = nfccard->userid;
                    item->m_NFCCard.unknown = nfccard->unknown;
                    item->m_NFCCard.deviceid = nfccard->deviceid;
                    item->m_NFCCard.maingroupid = nfccard->maingroupid;

                    DataItem * carditem = new DataItem("NFCCard", projectid);
                    carditem->setCardid(item->m_NFCCard.cardid);
                    carditem->setLoRAMacaddress(item->getLoRAMacaddress());
                    carditem->setGatewayid(item->getGatewayid());
                    mCARDRollCallJobQueue.add(carditem);

                    DataItem * carditem1 = new DataItem("NFCCard", projectid);
                    carditem1->setCardid(item->m_NFCCard.cardid);
                    carditem1->setLoRAMacaddress(item->getLoRAMacaddress());
                    carditem1->setGatewayid(item->getGatewayid());
                    mNFCCardPublisher->publish(carditem1);

                    // mDeviceManager->addCardScanRecord(projectid, item->m_NFCCard.cardid, item->getLoRAMacaddress());
                }

                // vilslog::printf("MQTTProcessingService::waitloop() NFCCard gwid[%s] loramacaddress[%s] datetime[%s] unknown[%d] cardid[%s]\n",
                //     item->getGatewayid(), item->getLoRAMacaddress(), item->m_NFCCard.datetime.c_str(), item->m_NFCCard.unknown, item->m_NFCCard.cardid.c_str());

                int deviceid = mDeviceManager->setNFCDeviceAlive(projectid, item->getLoRAMacaddress());
                item->m_NFCCard.deviceid = deviceid;
                if ( nfcdevice != NULL )
                {
                    item->m_NFCCard.maingroupid = nfcdevice->maingroupid;
                }
                // vilslog::printf ("MQTTProcessingService::waitloop() nfccard alive[%d] cardid[%s] datetime[%s] LoRAMacaddress[%s] deviceid[%d] unknown[%d] maplayer[%d]\n",
                //     item->m_NFCCard.alive, item->m_NFCCard.cardid.c_str(), item->m_NFCCard.datetime.c_str(), item->getLoRAMacaddress(), deviceid, item->m_NFCCard.unknown, item->m_NFCCard.maplayer);

                WorkItem * witem = new WorkItem("NFCCardStruct", projectid, &item->m_NFCCard);
                witem->setMaingroupid(item->m_NFCCard.maingroupid);
                mMySQLJobQueue.add(witem);

                //====================================
                CARDScanRecordStruct cardscan;
                cardscan.cardid = item->m_NFCCard.cardid;
                cardscan.cardtype = 0; // 0:NFC 1:UHF
                cardscan.LoRAMacaddress = item->getLoRAMacaddress();
                if ( nfcdevice != NULL )
                {
                    cardscan.ssid = nfcdevice->ssid;
                    cardscan.wifimac = nfcdevice->wifimac;
                }
                if ( item->m_NFCCard.datetime.length() > 0 )
                    cardscan.datetime = item->m_RFIDCard.datetime;
                else
                    cardscan.datetime = currentDateTimeForSQL();
                WorkItem * senditem = new WorkItem("CARDScanRecordStruct", projectid, &cardscan);
                senditem->setMaingroupid(item->m_NFCCard.maingroupid);
                mMySQLJobQueue.add(senditem);
                //====================================

                //====================================
                SubscribeDataItem * subitem = new SubscribeDataItem("cardrollcallevent", projectid);
                subitem->m_readerid = deviceid;
                subitem->m_cardtype = 0; // 0:NFC 1:UHF
                subitem->m_cardid = item->m_NFCCard.cardid;
                subitem->setMaingroupid(item->m_NFCCard.maingroupid);
                mWebSocketqueue.add(subitem);
                //====================================
            }
            else
            if ( strcmp(item->getType(), "EDGEDevice") == 0 )
            {
                vilslog::printf ("MQTTProcessingService::waitloop() EDGEDevice macaddress[%s] name[%s] type[%s] datetime[%s]\n",
                    item->m_EdgeDevice.macaddress.c_str(), item->m_EdgeDevice.name.c_str(), item->m_EdgeDevice.type.c_str(), item->m_EdgeDevice.datetime.c_str());

                EDGEDeviceStruct * edgedevice = mDeviceManager->get_EDGEDevice(projectid, item->m_EdgeDevice.macaddress.c_str());
                if (edgedevice == NULL)
                {
                    vilslog::printf ("MQTTProcessingService::waitloop() edgedevice == NULL\n");

                    item->m_EdgeDevice.deviceid = -1;

                    mDeviceManager->setEDGEDeviceAlive(projectid, &item->m_EdgeDevice);

                    WorkItem * witem = new WorkItem("EDGEDevice", projectid, &item->m_EdgeDevice);
                    mMySQLJobQueue.add(witem);

                    sleep(1);

                    DataItem * senditem = new DataItem("reloadedgedevice", projectid);
                    mMainJobQueue.add(senditem);
                }
                else
                {
                    vilslog::printf ("MQTTProcessingService::waitloop() edgedevice->deviceid[%d]\n", edgedevice->deviceid);

                    item->m_EdgeDevice.deviceid = edgedevice->deviceid;
                    if (edgedevice->name.length() > 0)
                        item->m_EdgeDevice.name = edgedevice->name;
                    if (edgedevice->type.length() > 0)
                        item->m_EdgeDevice.type = edgedevice->type;

                    mDeviceManager->setEDGEDeviceAlive(projectid, &item->m_EdgeDevice);

                    WorkItem * witem = new WorkItem("EDGEDevice", projectid, &item->m_EdgeDevice);
                    mMySQLJobQueue.add(witem);
                }
            }
            else
            if ( strcmp(item->getType(), "ACK") == 0 )
            {
                // vilslog::printf ("MQTTProcessingService::waitloop() ACK LoRAMacaddress[%s] cmd[%s]\n",
                //     item->getLoRAMacaddress(), item->getData());
                DataItem * carditem = new DataItem(item->getData(), projectid);
                carditem->setLoRAMacaddress(item->getLoRAMacaddress());
                mCARDRollCallJobQueue.add(carditem);
            }
            else
            if ( strcmp(item->getType(), "AOAfence") == 0 )
            {
                process_AOAfence(projectid, loraaddr, tagaddr, item->getAOA_lv(), item->getAOA_time(), gatewayid, rssi, timestamp);
            }
            else
            if ( strcmp(item->getType(), "lidarinfo") == 0 )
            {
                process_LiDARInfo(projectid, loraaddr, item->getData(), gatewayid, rssi, timestamp);
            }
            else
            if ( strcmp(item->getType(), "PowerSwitch") == 0 )
            {
                process_PowerSwitchInfo(projectid, loraaddr, item->getData(), gatewayid, rssi, timestamp);
            }
            else
            if ( strcmp(item->getType(), "beacon") == 0 )
            {
                process_beacon(projectid, loraaddr, tagaddr, item->getData(), gatewayid, rssi, timestamp);
            }
            else
            if ( strcmp(item->getType(), "Watch") == 0 )
            {
                process_WatchInfo(projectid, &item->m_SensorDevice);
            }
            else
            if ( strcmp(item->getType(), "Network Gateway") == 0 )
            {
                process_NetworkGateway(projectid, &item->m_SensorDevice);
            }
            else
            if ( strcmp(item->getType(), "iCover") == 0 )
            {
                process_iCover(projectid, loraaddr, tagaddr, item->getData(), gatewayid, rssi, timestamp);
            }
            else
            if ( strcmp(item->getType(), "wentaitek_light") == 0 )
            {
                process_wentaitek_light(projectid, &item->m_wentaitek_light);
                // vilslog::printf ("MQTTProcessingService::waitloop() wentaitek_light id[%d] name[%s] uniAddress[%d] state_onOff[%d] state_level[%d] state_cct[%d]\n",
                //     item->m_wentaitek_light.id,
                //     item->m_wentaitek_light.name.c_str(),
                //     item->m_wentaitek_light.uniAddress,
                //     item->m_wentaitek_light.state_onOff,
                //     item->m_wentaitek_light.state_level,
                //     item->m_wentaitek_light.state_cct);
            }
            else
            if ( strcmp(item->getType(), "wentaitek_s3_watch_device") == 0 )
            {
                process_wentaitek_s3_watch_device(projectid, &item->m_wentaitek_s3_watch_device);

                // vilslog::printf ("MQTTProcessingService::waitloop() wentaitek_s3_watch_device Device_ID[%s] Blood_Oxygen[%d] Heart_Rate[%d] DBP[%d] Rssi[%d] SBP[%d] SOS[%d] Temperature[%f] repeater_ip[%s] repeater_mac[%s]\n",
                //     item->m_wentaitek_s3_watch_device.Device_ID.c_str(),
                //     item->m_wentaitek_s3_watch_device.Blood_Oxygen,
                //     item->m_wentaitek_s3_watch_device.Heart_Rate,
                //     item->m_wentaitek_s3_watch_device.DBP,
                //     item->m_wentaitek_s3_watch_device.Rssi,
                //     item->m_wentaitek_s3_watch_device.SBP,
                //     item->m_wentaitek_s3_watch_device.SOS,
                //     item->m_wentaitek_s3_watch_device.Temperature,
                //     item->m_wentaitek_s3_watch_device.repeater_ip.c_str(),
                //     item->m_wentaitek_s3_watch_device.repeater_mac.c_str());

            }
            else
            if ( strcmp(item->getType(), "wentaitek_uw_sensor_1_device") == 0 )
            {
                process_wentaitek_uw_sensor_1_device(projectid, &item->m_wentaitek_uw_sensor_1_device);

                // vilslog::printf ("MQTTProcessingService::waitloop() wentaitek_uw_sensor_1_device Device_ID[%s] Rssi[%d] repeater_ip[%s] repeater_mac[%s]\n",
                //     item->m_wentaitek_uw_sensor_1_device.Device_ID.c_str(),
                //     item->m_wentaitek_uw_sensor_1_device.Rssi,
                //     item->m_wentaitek_uw_sensor_1_device.repeater_ip.c_str(),
                //     item->m_wentaitek_uw_sensor_1_device.repeater_mac.c_str());
            }
            else
            if ( strcmp(item->getType(), "wentaitek_power_meter_device") == 0 )
            {
                process_wentaitek_power_meter_device(projectid, &item->m_wentaitek_power_meter_device);

                vilslog::printf ("MQTTProcessingService::waitloop() wentaitek_power_meter_device Device_ID[%s] BT_version[%s] Rssi[%d] Active_Energy[%f] Active_Power[%f] Current_RMS[%f] Frequency[%f] Voltage_RMS[%f] relay_state[%d] repeater_ip[%s] repeater_mac[%s]\n",
                    item->m_wentaitek_power_meter_device.Device_ID.c_str(),
                    item->m_wentaitek_power_meter_device.BT_version.c_str(),
                    item->m_wentaitek_power_meter_device.Rssi,
                    item->m_wentaitek_power_meter_device.Active_Energy,
                    item->m_wentaitek_power_meter_device.Active_Power,
                    item->m_wentaitek_power_meter_device.Current_RMS,
                    item->m_wentaitek_power_meter_device.Frequency,
                    item->m_wentaitek_power_meter_device.Voltage_RMS,
                    item->m_wentaitek_power_meter_device.relay_state,
                    item->m_wentaitek_power_meter_device.repeater_ip.c_str(),
                    item->m_wentaitek_power_meter_device.repeater_mac.c_str());
            }
            else
            {
                vilslog::printf ("MQTTProcessingService::waitloop() unknown message item->getType()[%s]\n", item->getType());
            }

            delete item;
        }
        // start1 = end1;
    }
}

// All Tag
//QTTProcessingService() item:Type[cancel]
//MQTTProcessingService() item:loramacaddress[0000000001060029] tagmacaddress[0610000000000029]
//MQTTProcessingService() item:Type[alert]
//MQTTProcessingService() item:loramacaddress[0000000001060029] tagmacaddress[0610000000000029]

// Locator
//MQTTProcessingService() item:Type[alert]
//MQTTProcessingService() item:loramacaddress[0000000000050017] tagmacaddress[0530000000000017]
//MQTTProcessingService() item:Type[cancel]
//MQTTProcessingService() item:loramacaddress[0000000000050017] tagmacaddress[SELF]

void MQTTProcessingService::process_alert(int projectid, const char * loraaddr, const char * tagaddr, const char* gatewayid)
{
    mDeviceManager->setLocatorInfoGatewayID(projectid, loraaddr, gatewayid);
    mDeviceManager->setLocatorAliveByLoRAmacaddress(projectid, loraaddr);

    //
    // check device is Tag or Locator
    //
    int NodeType = CheckNodeType(projectid, tagaddr);

    if ( NodeType == NODE_TYPE_TAG )
    {
        process_alert_Tag(projectid, loraaddr, tagaddr, gatewayid);
    }
    else
    if ( NodeType == NODE_TYPE_NFC_LOCATOR )
    {
        process_alert_Locator(projectid, loraaddr, tagaddr, gatewayid);
    }
    else
    if ( NodeType == NODE_TYPE_UHF_READER )
    {
        process_alert_Locator(projectid, loraaddr, tagaddr, gatewayid);
    }
    else
    if ( NodeType == NODE_TYPE_LOCATOR )
    {
        process_alert_Locator(projectid, loraaddr, tagaddr, gatewayid);
    }
}

void MQTTProcessingService::process_alert_Tag(int projectid, const char * loraaddr, const char * tagaddr, const char* gatewayid)
{
    std::string tagID = "";
    std::string eventjson = "";
    char locatorID[64];
    snprintf(locatorID, 64, "%s", mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, loraaddr));
    int tagIndex = mDeviceManager->getTagIndex(projectid, tagaddr);

    if (tagIndex < 0)
    {
        mDeviceManager->setTagIndex(projectid, tagaddr, tagIndex, "");
        tagIndex = mDeviceManager->getTagIndex(projectid, tagaddr);
        tagID = mDeviceManager->getTagId(projectid, tagIndex);

        eventjson = convert_to_json_str(projectid, 9999, tagID , "Alert", "", "0", locatorID);
    }
    else
    {
        tagID = mDeviceManager->getTagId(projectid, tagIndex);
        eventjson = convert_to_json_str(projectid, 9999, tagID , "Alert", "", "0", locatorID);
    }

    int alertType = 1; //1:人員 2:固定裝置
    int alertAction = 3;// 1:In 2:Out 3:alert 4:cancel 5:callout
    time_t diffTime = 0;
    int maingroupid = 1;

    diffTime = mDeviceManager->getTagAlertDiffTimestamp(projectid, tagaddr);

    alertType = 1;//1:人員 2:固定裝置
    mDeviceManager->setTag0DAlert(projectid, tagaddr, loraaddr);
    mDeviceManager->setTagAlive(projectid, tagaddr);

#ifdef MQTT_PRINT_INCOMMING_MSG
    vilslog::printf("MQTTProcessingService::process_alert_Tag() setTag0DAlert[%s]\n", tagaddr);
#endif

    int alertTimeDiff = (int)diffTime; // in ms

    printf("MQTTProcessingService::process_alert_Tag() alertTimeDiff[%d] eventjson[%s]\n", alertTimeDiff, eventjson.c_str());

    if ( alertTimeDiff > 10000) // 10000 ms
    {
        WorkItem * witem = new WorkItem("alert0D", projectid, NODE_TYPE_TAG, tagaddr, eventjson.c_str());
        witem->setMaingroupid(maingroupid);
        mMySQLJobQueue.add(witem);

        SubscribeDataItem * sitem = new SubscribeDataItem("event", projectid, 999, tagID.c_str(), alertType, alertAction);
        sitem->setMaingroupid(maingroupid);
        mWebSocketqueue.add(sitem);

        int nodetype = NODE_TYPE_TAG;
        std::string nodeid = tagID;
        std::string nodename = mDeviceManager->getTagName(projectid, tagID.c_str());
        std::string message = "";
        int functiontype = TAG_PUSHNOTIFY_FUNCTION_2;
        std::string locatorname = mDeviceManager->getLocatorName(projectid, locatorID);

        message = nodename;
        message.append(" 在固定裝置 ");
        message.append(locatorname);
        message.append(" 按下緊急按鈕(UWB->LoRa)");

        NotificationItem * item = new NotificationItem(functiontype, projectid, nodetype, nodeid, nodename, "0", "1", tagaddr, "SMIMSVILS 人員區域求助", message);
        mNotifiactionQueue.add(item);
    }
}

void MQTTProcessingService::process_alert_Locator(int projectid, const char * loraaddr, const char * macaddress, const char* gatewayid)
{
    int buttontype = 0; // Alert
    std::string eventjson = "";
    char locatorID[64];
    snprintf(locatorID, 64, "%s", mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, loraaddr));

    printf("MQTTProcessingService::process_alert_Locator() loraaddr[%s] macaddress[%s] locatorID[%s]\n", loraaddr, macaddress, locatorID);

    mDeviceManager->setLocatorInfoLoRA(projectid, macaddress, loraaddr);

    // check button type
    buttontype = mDeviceManager->getLocatorButtonType(projectid, locatorID);

#ifdef MQTT_PRINT_INCOMMING_MSG
    vilslog::printf("MQTTProcessingService::process_alert_Locator() macaddress[%s] loraaddr[%s] locatorID[%s] buttontype[%d]\n",
        macaddress, loraaddr, locatorID, buttontype);
#endif

    if ( buttontype == 0 )
    {
        eventjson = convert_to_json_str(projectid, 9999, locatorID , "Alert", "", "0", locatorID);
    }
    else
    if ( buttontype == 1 )
    {
        vilslog::printf("MQTTProcessingService::process_alert_Locator() macaddress[%s] loraaddr[%s] locatorID[%s] buttontype[%d] CallOut\n",
            macaddress, loraaddr, locatorID, buttontype);

        eventjson = convert_to_json_str(projectid, 8888, locatorID , "CallOut", "", "0", locatorID);
    }

    int maingroupid = 1;
    int alertType = 2; //1:人員 2:固定裝置
    int alertAction = 3;// 1:In 2:Out 3:alert 4:cancel 5:callout
    time_t diffTime = mDeviceManager->getLocatorAlertDiffTimestamp(projectid, macaddress);
    if (diffTime < 0)
    {
        diffTime = mDeviceManager->getLocatorAlertDiffTimestampByLoRAmacaddress(projectid, loraaddr);
        if (diffTime < 0)
        {
            diffTime = 0;
        }
    }

    if (buttontype == 0)
    {
        mDeviceManager->setLocator0DAlert(projectid, macaddress, loraaddr);

#ifdef MQTT_PRINT_INCOMMING_MSG
        vilslog::printf("MQTTProcessingService::process_alert_Locator() setLocator0DAlert[%s]\n", macaddress);
#endif
    }
    else
    if ( buttontype == 1 )
    {
        mDeviceManager->updateLocatorAlertIssuTime(projectid, macaddress);
    }

    int alertTimeDiff = (int)diffTime; // in ms

    printf("MQTTProcessingService::process_alert_Locator() alertTimeDiff[%d] eventjson[%s]\n", alertTimeDiff, eventjson.c_str());

    // if ( (alertTimeDiff > 2500) && (buttontype == 0) ) // 2500 ms
    if ( alertTimeDiff > 10000) // 10000 ms
    {
        if ( buttontype == 0 )
        {
            WorkItem * witem = new WorkItem("alert0D", projectid, NODE_TYPE_LOCATOR, macaddress, eventjson.c_str());
            witem->setMaingroupid(maingroupid);
            mMySQLJobQueue.add(witem);

            SubscribeDataItem * sitem = new SubscribeDataItem("event", projectid, 999, "", alertType, alertAction);
            sitem->setMaingroupid(maingroupid);
            mWebSocketqueue.add(sitem);

            int nodetype = NODE_TYPE_LOCATOR;
            std::string nodeid = locatorID;
            std::string nodename = mDeviceManager->getLocatorName(projectid, locatorID);
            int functiontype = TAG_PUSHNOTIFY_FUNCTION_2;

            std::string message = "";
            message.append("固定裝置 ");
            message.append(nodename);
            message.append(" 按下緊急按鈕(LoRa)");

            NotificationItem * item = new NotificationItem(functiontype, projectid, nodetype, nodeid, nodename, "0", "1", macaddress, "SMIMSVILS 人員區域求助", message);
            mNotifiactionQueue.add(item);
        }
        else
        if (buttontype == 1)
        {
            // callout event
            WorkItem * witem = new WorkItem("callout", projectid, NODE_TYPE_LOCATOR, macaddress, eventjson.c_str());
            witem->setMaingroupid(maingroupid);
            mMySQLJobQueue.add(witem);

            alertType = 2; //1:人員 2:固定裝置
            alertAction = 5;// 1:In 2:Out 3:alert 4:cancel 5:callout
            std::string nodename = mDeviceManager->getLocatorName(projectid, locatorID);
            SubscribeDataItem * sitem = new SubscribeDataItem("callout", projectid, 888, locatorID, nodename.c_str(), alertType, alertAction);
            sitem->setMaingroupid(maingroupid);
            mWebSocketqueue.add(sitem);

            int nodetype = NODE_TYPE_LOCATOR;
            std::string nodeid = locatorID;
            std::string message = "";
            message.append("病房 ");
            message.append(nodename);
            message.append(" 已使用呼叫鈴/看護鈴，請盡快回病房查看。謝謝配合！");

            NotificationItem * item = new NotificationItem(LOCATOR_PUSHNOTIFY_FUNCTION_3, projectid, nodetype, nodeid, nodename, "0", "1", macaddress, "SMIMSVILS 病房呼喚鈴", message);
            mNotifiactionQueue.add(item);
        }
    }
}

void MQTTProcessingService::process_ranging(int projectid, const char * loraaddr, const char * tagaddr, int ranging, const char * voltage, const char* gatewayid, int rssi, const char*timestamp)
{
#ifdef MQTT_PRINT_INCOMMING_MSG
    vilslog::printf("MQTTProcessingService::process_ranging() tagaddr[%s]\n", tagaddr);
    vilslog::printf("MQTTProcessingService::process_ranging() item:ranging[%d]\n", ranging);
    vilslog::printf("MQTTProcessingService::process_ranging() voltage[%s]\n", voltage);
    vilslog::printf("MQTTProcessingService::process_ranging() rssi[%d]\n", rssi);
    vilslog::printf("MQTTProcessingService::process_ranging() timestamp[%s]\n", timestamp);
#endif

    // int projectid = mDeviceManager->getTagProjectId(tagaddr);
    int tagIndex = mDeviceManager->getTagIndex(projectid, tagaddr);
    if (tagIndex < 0)
    {
        mDeviceManager->setTagIndex(projectid, tagaddr, tagIndex, "");
        tagIndex = mDeviceManager->getTagIndex(projectid, tagaddr);
    }

    mDeviceManager->setTagAlive(projectid, tagaddr);
    mDeviceManager->setTagVoltage(projectid, tagaddr, voltage);
    mDeviceManager->setTagInfoRangingLoRAmac(projectid, tagaddr, loraaddr);
    mDeviceManager->setLocatorInfoGatewayID(projectid, loraaddr, gatewayid);

    DataItem * senditem = new DataItem("loraranging", projectid, "LoRa", loraaddr, tagaddr, ranging, rssi, timestamp);
    mPosCalJobQueue.add(senditem);
}

void MQTTProcessingService::process_rangingcompress(int projectid, const char * loraaddr, const char * tagaddr, int ranging, const char * voltage, int velocity, const char* gatewayid, int rssi, const char*timestamp)
{
#ifdef MQTT_PRINT_INCOMMING_MSG
    vilslog::printf("MQTTProcessingService::process_rangingcompress() tagaddr[%s] projectid[%d]\n", tagaddr, projectid);
    vilslog::printf("MQTTProcessingService::process_rangingcompress() item:ranging[%d]\n", ranging);
    vilslog::printf("MQTTProcessingService::process_rangingcompress() voltage[%s]\n", voltage);
    vilslog::printf("MQTTProcessingService::process_rangingcompress() velocity[%d]\n", velocity);
    vilslog::printf("MQTTProcessingService::process_rangingcompress() rssi[%d]\n", rssi);
    vilslog::printf("MQTTProcessingService::process_rangingcompress() timestamp[%s]\n", timestamp);
#endif
    // int projectid = mDeviceManager->getTagProjectId(tagaddr);

    int tagIndex = mDeviceManager->getTagIndex(projectid, tagaddr);
    if (tagIndex < 0)
    {
        mDeviceManager->setTagIndex(projectid, tagaddr, tagIndex, "");
        tagIndex = mDeviceManager->getTagIndex(projectid, tagaddr);
    }

    mDeviceManager->setTagAlive(projectid, tagaddr);
    mDeviceManager->setTagVoltage(projectid, tagaddr, voltage);
    mDeviceManager->setTagInfoRangingLoRAmac(projectid, tagaddr, loraaddr);

    //
    // check of locator exist
    //
    checkLocatorExisted(projectid, loraaddr);

    mDeviceManager->setLocatorInfoGatewayID(projectid, loraaddr, gatewayid);

    DataItem * senditem = new DataItem("lorarangingcompress", projectid, "LoRa", loraaddr, tagaddr, ranging, velocity, rssi, timestamp);
    mPosCalJobQueue.add(senditem);
}

void MQTTProcessingService::process_rangingcompressMultiDistance(int projectid, const char * loraaddr, const char * tagaddr, int distInterval, std::list<int> * distList, const char * voltage, int velocity, const char* gatewayid, int rssi, const char*timestamp)
{
    // vilslog::printf("MQTTProcessingService::process_rangingcompressMultiDistance() loraaddr[%s] tagaddr[%s] distInterval[%d] distList->size()[%d] voltage[%s] velocity[%d]\n",
    //     loraaddr, tagaddr, distInterval, (int)distList->size(), voltage, velocity);
    //

#ifdef MQTT_PRINT_INCOMMING_MSG
    vilslog::printf("MQTTProcessingService::process_rangingcompressMultiDistance() tagaddr[%s]\n", tagaddr);
    vilslog::printf("MQTTProcessingService::process_rangingcompressMultiDistance() voltage[%s]\n", voltage);
    vilslog::printf("MQTTProcessingService::process_rangingcompressMultiDistance() velocity[%d]\n", velocity);
    vilslog::printf("MQTTProcessingService::process_rangingcompressMultiDistance() rssi[%d]\n", rssi);
    vilslog::printf("MQTTProcessingService::process_rangingcompressMultiDistance() timestamp[%s]\n", timestamp);
#endif
    // int projectid = mDeviceManager->getTagProjectId(tagaddr);
    int tagIndex = mDeviceManager->getTagIndex(projectid, tagaddr);
    if (tagIndex < 0)
    {
        mDeviceManager->setTagIndex(projectid, tagaddr, tagIndex, "");
        tagIndex = mDeviceManager->getTagIndex(projectid, tagaddr);
    }

    mDeviceManager->setTagAlive(projectid, tagaddr);
    mDeviceManager->setTagVoltage(projectid, tagaddr, voltage);
    mDeviceManager->setTagInfoRangingLoRAmac(projectid, tagaddr, loraaddr);

    //
    // check of locator exist
    //
    checkLocatorExisted(projectid, loraaddr);

    mDeviceManager->setLocatorInfoGatewayID(projectid, loraaddr, gatewayid);

    DataItem * senditem = new DataItem("lorarangingcompressMultiDistance", 1, loraaddr, tagaddr, distInterval, distList, velocity, rssi, timestamp);
    mPosCalJobQueue.add(senditem);
}

void MQTTProcessingService::process_rangingcompressaoa(int projectid, const char * loraaddr, const char * tagaddr, int ranging, const char * voltage, int velocity, float angle, const char* gatewayid, int rssi, int * IMU, const char*timestamp)
{
    // vilslog::printf("MQTTProcessingService::process_rangingcompressaoa() loraadd[%s] tagaddr[%s] ranging[%d] angle[%f]\n", loraaddr, tagaddr, ranging, angle);
    // vilslog::printf("MQTTProcessingService::process_rangingcompressaoa() loraadd[%s] rssi[%d] timestamp[%s] voltage[%s] velocity[%d]\n", loraaddr, rssi, timestamp, voltage, velocity);
    // long long start1 = getSystemTime();

    int tagIndex = mDeviceManager->getTagIndex(projectid, tagaddr);
    if (tagIndex < 0)
    {
        mDeviceManager->setTagIndex(projectid, tagaddr, tagIndex, "");
        tagIndex = mDeviceManager->getTagIndex(projectid, tagaddr);
    }

    mDeviceManager->setTagAlive(projectid, tagaddr);
    mDeviceManager->setTagVoltage(projectid, tagaddr, voltage);
    mDeviceManager->setTagInfoRangingLoRAmac(projectid, tagaddr, loraaddr);

    // long long start2 = getSystemTime();
    //
    // check of locator exist
    //
    checkLocatorExisted(projectid, loraaddr);

    mDeviceManager->setLocatorInfoGatewayID(projectid, loraaddr, gatewayid);

    // 長映 Anchor + LoRA 使用此Address
    if (strncmp(loraaddr, "5252", 4) != 0)
    {
        mDeviceManager->updateAoAList(projectid, loraaddr);
    }

    // long long start3 = getSystemTime();

    DataItem * senditem = new DataItem("lorarangingcompressaoa", projectid, "LoRa", loraaddr, tagaddr, ranging, velocity, angle, rssi, timestamp);
    senditem->setIMU(IMU);
    mPosCalJobQueue.add(senditem);

    // long long start4 = getSystemTime();

    DataItem * senditem1 = new DataItem("lorarangingcompressaoa", projectid, "LoRa", loraaddr, tagaddr, ranging, velocity, angle, rssi, timestamp);
    senditem1->setIMU(IMU);
    mAoARangingPublisher->publish(senditem1);

    // long long end1 = getSystemTime();
    // long long difft1 = start2 - start1;
    // long long difft2 = start3 - start2;
    // long long difft3 = start4 - start3;
    // long long difft4 = end1 - start4;
    // vilslog::printf("MQTTProcessingService::process_rangingcompressaoa() difft1[%d] difft2[%d] difft3[%d] difft4[%d]\n",
    //     (int)difft1, (int)difft2, (int)difft3, (int)difft4);
}

void MQTTProcessingService::process_cancel(int projectid, const char * loraaddr, const char * tagaddr, const char* gatewayid)
{
    char locatorID[64];

    //
    // check of locator exist
    //
    checkLocatorExisted(projectid, loraaddr);

    snprintf(locatorID, 64, "%s", mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, loraaddr));
    mDeviceManager->setLocatorInfoGatewayID(projectid, loraaddr, gatewayid);

// #ifdef MQTT_PRINT_INCOMMING_MSG
    vilslog::printf("MQTTProcessingService::process_cancel() loraaddr[%s] tagaddr[%s] locatorID[%s]\n", loraaddr, tagaddr, locatorID);
// #endif

    int maingroupid = 1;
    int alertType = 1; //1:人員 2:固定裝置
    int alertAction = 4;// 1:In 2:Out 3:alert 4:cancel
    std::string nodeid = "";
    std::string empid = "";
    if ( strcmp(tagaddr, "SELF") == 0)
    {
        // Locator 自己按下取消鈕
        nodeid = locatorID;
        alertType = 2; //1:人員 2:固定裝置

        projectid = mDeviceManager->getLocatorProjectIdByLoRAmacaddress(loraaddr);

        std::string eventjson = convert_to_json_str(projectid, 9999, locatorID, "Cancel", "", "0", locatorID);
        WorkItem * witem = new WorkItem("alertcancel", projectid, NODE_TYPE_LOCATOR, locatorID, eventjson.c_str());
        witem->setMaingroupid(maingroupid);
        mMySQLJobQueue.add(witem);

        mDeviceManager->setLocatorAliveByLoRAmacaddress(projectid, loraaddr);

#ifdef MQTT_PRINT_INCOMMING_MSG
    vilslog::printf("MQTTProcessingService::process_cancel() SELF eventjson[%s]\n", eventjson.c_str());
#endif

    }
//     else
//     if ( strcmp(tagaddr, "0000000000000000") == 0)
//     {
//         // Locator bug , 員工卡片按下取消鈕, 只傳 "0"
//         nodeid = "CARD";
//         alertType = 1; //1:人員 2:固定裝置
//         empid = mDeviceManager->getempidFromTagByAliveLoRAranging(projectid, loraaddr);
//
//         // 工作人員 simple tag 按下取消鈕
//         std::string eventjson = convert_to_json_str(projectid, 9999, nodeid, "Cancel", "", "0", locatorID);
//         WorkItem * witem = new WorkItem("alertcancel", projectid, NODE_TYPE_TAG, tagaddr, eventjson.c_str(), empid);
//         witem->setMaingroupid(maingroupid);
//         mMySQLJobQueue.add(witem);
//         mDeviceManager->setLocatorAliveByLoRAmacaddress(projectid, loraaddr);
//
// #ifdef MQTT_PRINT_INCOMMING_MSG
//         vilslog::printf("MQTTProcessingService::process_cancel() Tag eventjson[%s]\n", eventjson.c_str());
// #endif
//     }
    else
    {
        nodeid = mDeviceManager->getTagId(projectid, tagaddr);
        alertType = 1; //1:人員 2:固定裝置
        //empid = mDeviceManager->getTagempid(projectid, tagaddr);
        int userid = mDeviceManager->getTaguserid(projectid, tagaddr);
        empid = std::to_string(userid);
        //projectid = mDeviceManager->getTagProjectId(tagaddr);

        // 工作人員 simple tag 按下取消鈕
        std::string eventjson = convert_to_json_str(projectid, 9999, nodeid, "Cancel", "", "0", locatorID);
        WorkItem * witem = new WorkItem("alertcancel", projectid, NODE_TYPE_TAG, tagaddr, eventjson.c_str(), empid);
        witem->setMaingroupid(maingroupid);
        mMySQLJobQueue.add(witem);

        mDeviceManager->setTagAlive(projectid, tagaddr);
        mDeviceManager->setLocatorAliveByLoRAmacaddress(projectid, loraaddr);

#ifdef MQTT_PRINT_INCOMMING_MSG
    vilslog::printf("MQTTProcessingService::process_cancel() Tag eventjson[%s]\n", eventjson.c_str());
#endif

    }

    // info websocket alert
#ifdef MQTT_PRINT_INCOMMING_MSG
    vilslog::printf("MQTTProcessingService() cancel info websocket alert nodeid[%s]\n", nodeid.c_str());
#endif

    SubscribeDataItem * sitem = new SubscribeDataItem("event", projectid, 999, nodeid.c_str(), alertType, alertAction);
    sitem->setMaingroupid(maingroupid);
    mWebSocketqueue.add(sitem);

    SubscribeDataItem * sitem1 = new SubscribeDataItem("event", projectid, 999, locatorID, alertType, alertAction);
    sitem1->setMaingroupid(maingroupid);
    mWebSocketqueue.add(sitem1);


    //============================================================
    // 判斷 透過同一個locator發出的SOS所有tag都取消.
    //
    if (alertType == 1)
    {
        std::list<std::string> * noideidlist = new std::list<std::string>();
        mDeviceManager->getTag0DAlertList(projectid, loraaddr, noideidlist);

    // #ifdef MQTT_PRINT_INCOMMING_MSG
        vilslog::printf("MQTTProcessingService() cancel getTag0DAlertList count[%d]\n", (int)noideidlist->size());
    // #endif

        for (std::list<std::string>::iterator tagnodeid = noideidlist->begin(); tagnodeid != noideidlist->end(); tagnodeid++)
        {
            alertType = 1; //1:人員 2:固定裝置
            int alertAction = 4;// 1:In 2:Out 3:alert 4:cancel
            SubscribeDataItem * sitem = new SubscribeDataItem("event", projectid, 999, (*tagnodeid).c_str(), alertType, alertAction);
            sitem->setMaingroupid(maingroupid);
            mWebSocketqueue.add(sitem);

    // #ifdef MQTT_PRINT_INCOMMING_MSG
            vilslog::printf("MQTTProcessingService() cancel tagnodeid[%s]\n", (*tagnodeid).c_str());
    // #endif
        }
        noideidlist->clear();
        delete noideidlist;

        mDeviceManager->clearTag0DAlert(projectid, loraaddr);

    #ifdef MQTT_PRINT_INCOMMING_MSG
        vilslog::printf("MQTTProcessingService() clearTag0DAlert loraaddr[%s]\n", loraaddr);
    #endif
    }

    //============================================================


    //============================================================
    // 取消此 Locator 發出的 SOS
    //
    mDeviceManager->clearLocator0DAlert(projectid, loraaddr);

#ifdef MQTT_PRINT_INCOMMING_MSG
    vilslog::printf("MQTTProcessingService() cancel clearLocator0DAlert loraaddr[%s] bFound[%d]\n", loraaddr, bFound);
#endif
    //============================================================

    //============================================================
    // 判斷 10 公尺內所有 tag 的 SOS 都取消.
    //
    if (alertType == 1)
    {
        double Coord[3];
        mDeviceManager->getLocatorCoord(projectid, locatorID, Coord);

        vilslog::printf("MQTTProcessingService() cancel locatorID[%s] Coord[%d, %d, %d]\n", locatorID, (int)Coord[0], (int)Coord[1], (int)Coord[2]);

        std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);
        for (std::list<NodeStruct*>::iterator existed = tagNodeList->begin(); existed != tagNodeList->end(); existed++)
        {
            if ( (*existed)->alert0D > 0 || (*existed)->alert > 0)
            {
                vilslog::printf("MQTTProcessingService() nodeid[%s] alert0D[%d] alert[%d]\n", (*existed)->nodeid, (*existed)->alert0D, (*existed)->alert);

                int distance = sqrt(pow((*existed)->posX - Coord[0], 2) + pow((*existed)->posY - Coord[1], 2));
                if ( distance < 10000 ) // 10 meters
                {
                    int alertType = 1; //1:人員 2:固定裝置
                    int alertAction = 4;// 1:In 2:Out 3:alert 4:cancel
                    SubscribeDataItem * sitem = new SubscribeDataItem("event", projectid, 999, (*existed)->nodeid, alertType, alertAction);
                    sitem->setMaingroupid((*existed)->maingroupid);
                    mWebSocketqueue.add(sitem);

                    if ( (*existed)->alert > 0 )
                    {
                        //================================================================
                        // 設定工作人員 simple tag 在 Coordinator 按下取消鈕
                        std::string CoordinatorId = mDeviceManager->getCoordinatorIdFromIP(projectid, (*existed)->IP);
                        std::string eventjson = convert_to_json_str(projectid, 9999, nodeid, "Cancel", "", "0", CoordinatorId);
                        WorkItem * witem = new WorkItem("alertcancel", projectid, NODE_TYPE_TAG, tagaddr, eventjson.c_str(), empid);
                        witem->setMaingroupid((*existed)->maingroupid);
                        mMySQLJobQueue.add(witem);
                        //================================================================
                    }
                    if ( (*existed)->alert0D > 0 )
                    {
                        // Find tag alert locator info

                        std::string curLocatorID =  mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, (*existed)->LoRAMacaddress);
                        std::string eventjson = convert_to_json_str(projectid, 9999, nodeid, "Cancel", "", "0", curLocatorID);
                        WorkItem * witem = new WorkItem("alertcancel", projectid, NODE_TYPE_TAG, tagaddr, eventjson.c_str(), empid);
                        witem->setMaingroupid((*existed)->maingroupid);
                        mMySQLJobQueue.add(witem);
                    }

                    vilslog::printf("MQTTProcessingService() cancel distance[%d] nodeid[%s]\n", distance, (*existed)->nodeid);
                    mDeviceManager->clearTagAlertById(projectid, (*existed)->nodeid);
                    mDeviceManager->clearNodeAlertByType(projectid, (*existed)->macaddress, NODE_TYPE_TAG);
                }
            }
        }
    }
    //============================================================


    // if (!bFound)
    // {
    //     bFound = mDeviceManager->clearLocator0DAlert(loraaddr);
    // }
}

void MQTTProcessingService::process_taggps(int projectid, const char * loraaddr, double Longitude, double Latitude, const char* gatewayid, const char * timestamp)
{
    char tagaddr[64];
    snprintf(tagaddr, 64, "%s", "0610000000000000");

    tagaddr[12] = loraaddr[12];
    tagaddr[13] = loraaddr[13];
    tagaddr[14] = loraaddr[14];
    tagaddr[15] = loraaddr[15];

    // int projectid = mDeviceManager->getTagProjectId(tagaddr);
    int tagIndex = mDeviceManager->getTagIndex(projectid, tagaddr);
    if (tagIndex < 0)
    {
        mDeviceManager->setTagIndex(projectid, tagaddr, tagIndex, "");
        tagIndex = mDeviceManager->getTagIndex(projectid, tagaddr);
    }

#ifdef MQTT_PRINT_INCOMMING_MSG
    vilslog::printf("MQTTProcessingService::process_taggps() tagIndex[%d] tagaddr[%s] Longitude[%f] Latitude[%f]\n",
        tagIndex, tagaddr, Longitude, Latitude);
#endif

    bool found = mDeviceManager->setTagInfoGPS(projectid, tagaddr, Longitude, Latitude, timestamp);

    if( found )
    {
        GPSInfoStruct * gpsinfo = mDeviceManager->getTagInfoGPS(projectid, tagaddr);
        if(gpsinfo != NULL)
        {

            WorkItem * witem = new WorkItem("taggpsinfo", projectid, gpsinfo);
            mMySQLJobQueue.add(witem);

            WorkItem * witem1 = new WorkItem("taggpsinfo", projectid, gpsinfo);
            mNodePositionQueue.add(witem1);
        }
    }

    mDeviceManager->setTagAlive(projectid, tagaddr);
    mDeviceManager->setTagOperationMode(projectid, tagaddr, OPERTAION_MODE_GPS);
    mDeviceManager->setLocatorAliveByLoRAmacaddress(projectid, loraaddr);

    mDeviceManager->setTagInfoGatewayID(projectid, tagaddr, gatewayid);
}

void MQTTProcessingService::process_locatorgps(int projectid, const char * loraaddr, double Longitude, double Latitude, const char* gatewayid, const char * timestamp)
{
#ifdef MQTT_PRINT_INCOMMING_MSG
    vilslog::printf("MQTTProcessingService::process_locatorgps() loraaddr[%s] Longitude[%f] Latitude[%f]\n",
        loraaddr, Longitude, Latitude);
#endif

    mDeviceManager->setLocatorInfoGPS(projectid, loraaddr, Longitude, Latitude, timestamp);

    //mDeviceManager->setTagOperationMode(projectid, tagaddr, OPERTAION_MODE_GPS);
    mDeviceManager->setLocatorAliveByLoRAmacaddress(projectid, loraaddr);

    mDeviceManager->setLocatorInfoGatewayID(projectid, loraaddr, gatewayid);
}

int MQTTProcessingService::CheckNodeType(int projectid, const char * macaddress)
{
    int NodeType = NODE_TYPE_TAG;

    if ( strlen(macaddress) == 0)
    {
        return NODE_TYPE_LOCATOR;
    }

    if (macaddress[2] == '1')
    {
        NodeType = NODE_TYPE_TAG;
#ifdef MQTT_PRINT_INCOMMING_MSG
        vilslog::printf("MQTTProcessingService::CheckNodeType() macaddress[%s] NODE_TYPE_TAG\n", macaddress);
#endif
    }
    else
    {
        RFIDDeviceStruct * rfidDevice = mDeviceManager->get_RFIDDevice(projectid, macaddress);
        if (rfidDevice != NULL)
        {
            NodeType = NODE_TYPE_UHF_READER;
            vilslog::printf("MQTTProcessingService::CheckNodeType() macaddress[%s] NODE_TYPE_UHF_READER\n", macaddress);
        }
        else
        {
            NFCDeviceStruct * nfcDevice = mDeviceManager->get_NFCDevice(projectid, macaddress);
            if (nfcDevice != NULL)
            {
                NodeType = NODE_TYPE_NFC_LOCATOR;
                vilslog::printf("MQTTProcessingService::CheckNodeType() macaddress[%s] NODE_TYPE_NFC_LOCATOR\n", macaddress);
            }
            else
            {
                NodeType = NODE_TYPE_LOCATOR;
            }
        }

#ifdef MQTT_PRINT_INCOMMING_MSG
        vilslog::printf("MQTTProcessingService::CheckNodeType() macaddress[%s] NodeType[%d]\n", macaddress, NodeType);
#endif
    }

    return NodeType;
}

void MQTTProcessingService::process_loragateway(int projectid, LoRaGatewayInfoStruct * gateway)
{
#ifdef MQTT_PRINT_INCOMMING_MSG
    vilslog::printf("MQTTProcessingService::process_loragateway() sf[%d] gwip[%s] gwid[%s] repeater[%s] channels[%d]\n",
        gateway->sf, gateway->gwip.c_str(), gateway->gwid.c_str(), gateway->repeater.c_str(), (int)gateway->channels.size());
#endif

    std::string newChannel = "";
    for (std::list<std::string>::iterator channel = gateway->channels.begin(); channel != gateway->channels.end(); channel++)
    {
        newChannel = (*channel);
    }
    // int projectid = 1;

    //mDeviceManager->setLoRaGatewayAlive(gateway->gwid, newChannel);
    mDeviceManager->setLoRaGatewayAlive(projectid, gateway);
    mDeviceManager->setLoRaGatewayIP(projectid, gateway->gwid, gateway->gwip);

    LoRaGatewayInfoStruct * CurGateway = mDeviceManager->getLoRaGateway(projectid, gateway->gwid);
    if ( CurGateway == NULL )
    {
        WorkItem * witem = new WorkItem("loragateway", projectid, gateway);
        mMySQLJobQueue.add(witem);
    }
    else
    {
        WorkItem * witem = new WorkItem("loragateway", projectid, CurGateway);
        mMySQLJobQueue.add(witem);
    }
}

void MQTTProcessingService::process_MQTTgateway(int projectid, LoRaGatewayInfoStruct * gateway)
{
#ifdef MQTT_PRINT_INCOMMING_MSG
    vilslog::printf("MQTTProcessingService::process_MQTTgateway() gwid[%s]\n", gateway->gwid.c_str());
#endif
    // int projectid = 1;
    mDeviceManager->setLoRaGatewayMQTTAlive(projectid, gateway->gwid);
}

void MQTTProcessingService::process_AOAfence(int projectid, const char * loraaddr, const char * tagaddr, const char * lv, const char * time, const char* gatewayid, int rssi, const char*timestamp)
{
    // "lv": "1.-1;0.-1", "time": "0;27"

    // vilslog::printf("MQTTProcessingService::process_AOAfence() loraaddr[%s] tagaddr[%s] lv[%s] time[%s] gatewayid[%s]\n",
    //     loraaddr, tagaddr, lv, time, gatewayid);

    int tagIndex = mDeviceManager->getTagIndex(projectid, tagaddr);
    if (tagIndex < 0)
    {
        mDeviceManager->setTagIndex(projectid, tagaddr, tagIndex, "");
        tagIndex = mDeviceManager->getTagIndex(projectid, tagaddr);
    }

    mDeviceManager->setTagAlive(projectid, tagaddr);
    mDeviceManager->setTagInfoRangingLoRAmac(projectid, tagaddr, loraaddr);

    //
    // check of locator exist
    //
    checkLocatorExisted(projectid, loraaddr);

    mDeviceManager->setLocatorInfoGatewayID(projectid, loraaddr, gatewayid);
    mDeviceManager->updateAoAList(projectid, loraaddr);


    std::list<std::string> * levelList = convert_string_to_list(std::string(lv), ';');
    std::list<std::string> * timeList = convert_string_to_list(std::string(time), ';');

    levelList->reverse();
    timeList->reverse();

    std::list<std::string>::iterator leveltime = timeList->begin();
    for (std::list<std::string>::iterator level = levelList->begin(); level != levelList->end(); level++, leveltime++)
    {
        // vilslog::printf("MQTTProcessingService::process_AOAfence() level[%s] leveltime[%s]\n", (*level).c_str(), (*leveltime).c_str());

        DataItem * senditem = new DataItem("AOAfence", projectid, "LoRa", loraaddr, tagaddr, 0, rssi, timestamp);
        senditem->setAOA_lv((*level), (*leveltime));
        mPosCalJobQueue.add(senditem);
    }

    levelList->clear();
    timeList->clear();
    delete levelList;
    delete timeList;
}

void MQTTProcessingService::process_LiDARInfo(int projectid, const char * loraaddr, const char * value, const char* gatewayid, int rssi, const char * timestamp)
{
    //
    // check of locator exist
    //
    checkLocatorExisted(projectid, loraaddr);

    mDeviceManager->setLocatorInfoGatewayID(projectid, loraaddr, gatewayid);

    std::string locatorid = mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, loraaddr);
    char locatormacaddress[64];
    snprintf(locatormacaddress, 64, "%s", mDeviceManager->getLocatorMacaddress(projectid, locatorid.c_str()));

    int adrLen = strlen(locatormacaddress);

    std::ostringstream lidarName;
    lidarName<<"LiDAR"<<locatormacaddress[0]<<locatormacaddress[1]<<locatormacaddress[adrLen-4]<<locatormacaddress[adrLen-3]<<locatormacaddress[adrLen-2]<<locatormacaddress[adrLen-1];

    std::ostringstream jsonvalue;
    jsonvalue<<"{\"D\":\""<<value<<"\"}";

    int maingroupid = 1;
    SensorInfoStruct * sensor = new SensorInfoStruct();
    sensor->id = -1;
    sensor->name = lidarName.str();
    sensor->day = TodayDateType1();
    sensor->value = value;
    sensor->jsonvalue = jsonvalue.str();
    sensor->Longitude = "";
    sensor->Latitude = "";
    sensor->datetime = "";
    sensor->maingroups = maingroupid;


    WorkItem * witem = new WorkItem("sensorinfo", projectid, sensor);
    witem->setMaingroupid(maingroupid);
    mMySQLJobQueue.add(witem);

    WorkItem * witem1 = new WorkItem("LiDAR_sensorinfo", projectid, sensor);
    witem1->setMaingroupid(maingroupid);
    witem1->setDateTime(timestamp);
    mSensorInfoJobQueue.add(witem1);

    delete sensor;

    SensorDeviceStruct * sensorDevice = mDeviceManager->get_SensorDevice(projectid, lidarName.str());
    if (sensorDevice == NULL)
    {
        sensorDevice = new SensorDeviceStruct();
        sensorDevice->alive = 1;
        sensorDevice->id = lidarName.str();
        sensorDevice->name = lidarName.str();
        sensorDevice->type = "LiDAR";
        sensorDevice->jsonvalue = jsonvalue.str();
        sensorDevice->Longitude = "";
        sensorDevice->Latitude = "";
        sensorDevice->Battery = "";
        sensorDevice->Version = "";
        sensorDevice->datetime = "";
        sensorDevice->maingroupid = maingroupid;

        WorkItem * witem = new WorkItem("sensordevice", projectid, sensorDevice);
        mMySQLJobQueue.add(witem);

        mDeviceManager->setSensorDeviceAlive(projectid, sensorDevice);

        delete sensorDevice;
    }
    else
    {
        sensorDevice->type = "LiDAR";
        sensorDevice->jsonvalue = jsonvalue.str();
        mDeviceManager->setSensorDeviceAlive(projectid, sensorDevice);
        WorkItem * witem = new WorkItem("sensordevice", projectid, sensorDevice);
        mMySQLJobQueue.add(witem);
    }
}

void MQTTProcessingService::process_PowerSwitchInfo(int projectid, const char * loraaddr, const char * value, const char* gatewayid, int rssi, const char * timestamp)
{
    //
    // check of locator exist
    //
    checkLocatorExisted(projectid, loraaddr);

    mDeviceManager->setLocatorInfoGatewayID(projectid, loraaddr, gatewayid);

    std::string locatorid = mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, loraaddr);

    char locatormacaddress[64];
    memset(locatormacaddress, 0, 64);
    snprintf(locatormacaddress, 64, "%s", mDeviceManager->getLocatorMacaddress(projectid, locatorid.c_str()));

    int adrLen = strlen(locatormacaddress);

    std::ostringstream swName;
    swName<<"PowerSwitch"<<locatormacaddress[0]<<locatormacaddress[1]<<locatormacaddress[adrLen-4]<<locatormacaddress[adrLen-3]<<locatormacaddress[adrLen-2]<<locatormacaddress[adrLen-1];

    Json::Value JSONLora;
    Json::Reader * readerData = new Json::Reader();
    bool parsingSuccessful = readerData->parse( value, JSONLora);
    delete readerData;

    std::string v = "";
    std::string i = "";
    std::string w = "";
    std::string wacc = "";

    if (parsingSuccessful)
    {
        if ( JSONLora.isMember("v") )
        {
            v = JSONLora["v"].asString();
        }
        if ( JSONLora.isMember("i") )
        {
            i = JSONLora["i"].asString();
        }
        if ( JSONLora.isMember("w") )
        {
            w = JSONLora["w"].asString();
        }
        if ( JSONLora.isMember("wacc") )
        {
            wacc = JSONLora["wacc"].asString();
        }
    }

    int maingroupid = 1;
    SensorInfoStruct * sensor = new SensorInfoStruct();
    sensor->id = -1;
    sensor->name = swName.str();
    sensor->day = TodayDateType1();
    sensor->value = v;
    sensor->jsonvalue = value;
    sensor->Longitude = "";
    sensor->Latitude = "";
    sensor->datetime = "";
    sensor->maingroups = maingroupid;

    WorkItem * witem = new WorkItem("sensorinfo", projectid, sensor);
    witem->setMaingroupid(maingroupid);
    mMySQLJobQueue.add(witem);

    delete sensor;

    SensorDeviceStruct * sensorDevice = mDeviceManager->get_SensorDevice(projectid, swName.str());
    if (sensorDevice == NULL)
    {
        sensorDevice = new SensorDeviceStruct();
        sensorDevice->alive = 1;
        sensorDevice->id = swName.str();
        sensorDevice->name = swName.str();
        sensorDevice->type = "PowerSwitch";
        sensorDevice->jsonvalue = value;
        sensorDevice->Longitude = "";
        sensorDevice->Latitude = "";
        sensorDevice->Battery = v;
        sensorDevice->Version = "";
        sensorDevice->datetime = "";
        sensorDevice->maingroupid = maingroupid;

        WorkItem * witem = new WorkItem("sensordevice", projectid, sensorDevice);
        mMySQLJobQueue.add(witem);

        mDeviceManager->setSensorDeviceAlive(projectid, sensorDevice);

        delete sensorDevice;
    }
    else
    {
        sensorDevice->type = "PowerSwitch";
        sensorDevice->jsonvalue = value;
        sensorDevice->Battery = v;
        mDeviceManager->setSensorDeviceAlive(projectid, sensorDevice);

        WorkItem * witem = new WorkItem("sensordevice", projectid, sensorDevice);
        mMySQLJobQueue.add(witem);
    }

    mDeviceManager->setSensorDeviceValue(projectid, swName.str(), v, currentDateTimeForSQL());

}

void MQTTProcessingService::process_beacon(int projectid, const char * loraaddr, const char * tagaddr, const char * value, const char* gatewayid, int rssi, const char * timestamp)
{
    //
    // check of locator exist
    //
    if (strlen(loraaddr) > 0)
    {
        checkLocatorExisted(projectid, loraaddr);

        mDeviceManager->setLocatorInfoGatewayID(projectid, loraaddr, gatewayid);
    }

    Json::Value JSONLora;
    Json::Reader * readerData = new Json::Reader();
    bool parsingSuccessful = readerData->parse( value, JSONLora);
    delete readerData;

    if (!parsingSuccessful)
    {
        return;
    }

    std::string minor = "0";
    std::string major = "0";
    std::string data = "";

    if ( JSONLora.isMember("minor") )
        minor = JSONLora["minor"].asString();
    if ( JSONLora.isMember("major") )
        major = JSONLora["major"].asString();
    if ( JSONLora.isMember("data") )
        data = JSONLora["data"].asString();

    std::ostringstream swName;
    swName<<"beacon_"<<major<<"_"<<minor;

    std::vector<std::string> distances = convert_string_to_vector(data, ':');

    std::string lastDistance = distances[distances.size() - 1];

    distances.clear();

    vilslog::printf("MQTTProcessingService::process_beacon() loraaddr[%s] tagaddr[%s] lastDistance[%s]\n", loraaddr, tagaddr, lastDistance.c_str());

    int maingroupid = 1;
    SensorInfoStruct * sensor = new SensorInfoStruct();
    sensor->id = -1;
    sensor->name = swName.str();
    sensor->day = TodayDateType1();
    sensor->value = lastDistance;
    sensor->jsonvalue = value;
    sensor->Longitude = "";
    sensor->Latitude = "";
    sensor->datetime = "";
    sensor->maingroups = maingroupid;

    WorkItem * witem = new WorkItem("sensorinfo", projectid, sensor);
    witem->setMaingroupid(maingroupid);
    mMySQLJobQueue.add(witem);

    delete sensor;

    SensorDeviceStruct * sensorDevice = mDeviceManager->get_SensorDevice(projectid, swName.str());
    if (sensorDevice == NULL)
    {
        sensorDevice = new SensorDeviceStruct();
        sensorDevice->alive = 1;
        sensorDevice->id = swName.str();
        sensorDevice->name = swName.str();
        sensorDevice->type = "beacon";
        sensorDevice->jsonvalue = value;
        sensorDevice->Longitude = "";
        sensorDevice->Latitude = "";
        sensorDevice->Battery = "";
        sensorDevice->Version = "";
        sensorDevice->datetime = "";
        sensorDevice->maingroupid = maingroupid;

        WorkItem * witem = new WorkItem("sensordevice", projectid, sensorDevice);
        mMySQLJobQueue.add(witem);

        mDeviceManager->setSensorDeviceAlive(projectid, sensorDevice);

        delete sensorDevice;
    }
    else
    {
        sensorDevice->type = "beacon";
        sensorDevice->jsonvalue = value;
        mDeviceManager->setSensorDeviceAlive(projectid, sensorDevice);

        WorkItem * witem = new WorkItem("sensordevice", projectid, sensorDevice);
        mMySQLJobQueue.add(witem);
    }

    mDeviceManager->setSensorDeviceValue(projectid, swName.str(), lastDistance, currentDateTimeForSQL());

    int ranging = parseInt(lastDistance, 10);

    // ==============================================
    // Ranging
    // loraaddr(BT_Lora_Locator)  tagaddr(beacon)
    int tagIndex = mDeviceManager->getTagIndex(projectid, tagaddr);
    if (tagIndex < 0)
    {
        mDeviceManager->setTagIndex(projectid, tagaddr, tagIndex, "");
        tagIndex = mDeviceManager->getTagIndex(projectid, tagaddr);
    }

    mDeviceManager->setTagAlive(projectid, tagaddr);
    // mDeviceManager->setTagVoltage(projectid, tagaddr, voltage);
    mDeviceManager->setTagInfoRangingLoRAmac(projectid, tagaddr, loraaddr);
    mDeviceManager->setLocatorInfoGatewayID(projectid, loraaddr, gatewayid);

    DataItem * senditem = new DataItem("loraranging", projectid, "LoRa", loraaddr, tagaddr, ranging, rssi, timestamp);
    mPosCalJobQueue.add(senditem);
    // ==============================================


    // ==============================================
    // Ranging
    // loraaddr(BT_Lora_Locator)  tagaddr(beacon)
    // std::string beacon = tagaddr;
    // std::string BT_Lora_Locator = loraaddr;
    //
    // //
    // // check of locator exist
    // //
    // checkLocatorExisted(projectid, beacon.c_str());
    //
    // int tagIndex = mDeviceManager->getTagIndex(projectid, BT_Lora_Locator.c_str());
    // if (tagIndex < 0)
    // {
    //     mDeviceManager->setTagIndex(projectid, BT_Lora_Locator.c_str(), tagIndex, "");
    //     tagIndex = mDeviceManager->getTagIndex(projectid, BT_Lora_Locator.c_str());
    // }
    //
    // mDeviceManager->setTagAlive(projectid, BT_Lora_Locator.c_str());
    // // mDeviceManager->setTagVoltage(projectid, tagaddr, voltage);
    // mDeviceManager->setTagInfoRangingLoRAmac(projectid, BT_Lora_Locator.c_str(), loraaddr);
    // mDeviceManager->setLocatorInfoGatewayID(projectid, beacon.c_str(), gatewayid);
    //
    // DataItem * senditem = new DataItem("loraranging", projectid, "LoRa", beacon.c_str(), BT_Lora_Locator.c_str(), ranging, rssi, timestamp);
    // mPosCalJobQueue.add(senditem);
    // ==============================================

}

void MQTTProcessingService::process_WatchInfo(int projectid, SensorDeviceStruct * sensorDevice)
{
    SensorDeviceStruct * sensorExisted = mDeviceManager->get_SensorDevice(projectid, sensorDevice->id);
    if (sensorExisted == NULL)
    {
        WorkItem * witem = new WorkItem("sensordevice", projectid, sensorDevice);
        mMySQLJobQueue.add(witem);

        mDeviceManager->setSensorDeviceAlive(projectid, sensorDevice);
        mDeviceManager->setSensorDeviceValue(projectid, sensorDevice->id, sensorDevice->Battery, currentDateTimeForSQL());
    }
    else
    {
        sensorDevice->name = sensorExisted->name;
        sensorDevice->type = "Watch";
        sensorDevice->maingroupid = sensorExisted->maingroupid;

        mDeviceManager->setSensorDeviceAlive(projectid, sensorDevice);
        mDeviceManager->setSensorDeviceValue(projectid, sensorDevice->id, sensorDevice->Battery, currentDateTimeForSQL());

        WorkItem * witem = new WorkItem("sensordevice", projectid, sensorDevice);
        mMySQLJobQueue.add(witem);

        //-------------------
        // WorkItem * witem1 = new WorkItem("sensordevice", projectid, sensorDevice);
        // mSensorBodyInfoPublisher->publish(witem1);
        //-------------------
    }

    // vilslog::printf("MQTTProcessingService::process_WatchInfo() id[%s] jsonvalue[%s]\n", sensorDevice->id.c_str(), sensorDevice->jsonvalue.c_str());

    UserStruct * userLink = NULL;
    bool bFoundUser = false;
    std::string HR = "";
    std::string SPO2 = "";
    std::string Battery = sensorDevice->Battery;

    Json::Value result;
    bool parsingSuccessful = parse_JSON(sensorDevice->jsonvalue, result);
    if ( parsingSuccessful )
    {
        if ( result.isMember("HR") )
        {
            HR = result["HR"].asString();
        }
        if ( result.isMember("SPO2") )
        {
            SPO2 = result["SPO2"].asString();
        }

        userLink = update_sensor_user_location(projectid, sensorDevice->id, sensorDevice->Longitude, sensorDevice->Latitude);
        if ( userLink != NULL )
            bFoundUser = true;
    }

    // vilslog::printf("MQTTProcessingService::process_WatchInfo() bFoundUser[%d]\n", bFoundUser);

    if (bFoundUser && ((HR.compare("255") != 0 && HR.compare("0") != 0) || (SPO2.compare("255") != 0 && SPO2.compare("0") != 0)) )
    {
        DataItem * senditem = new DataItem("savebodyinfoall", projectid, userLink->account.c_str());
        if (HR.compare("255") != 0 && HR.compare("0") != 0)
        {
            senditem->setHeartBeat(HR);
        }
        if (SPO2.compare("255") != 0 && SPO2.compare("0") != 0)
        {
            senditem->setBloodOxygen(SPO2);
        }
        senditem->setDateTime(currentDateTimeForSQL());
        mMainJobQueue.add(senditem);
    }
}

void MQTTProcessingService::process_NetworkGateway(int projectid, SensorDeviceStruct * sensorDevice)
{
    SensorDeviceStruct * sensorExisted = mDeviceManager->get_SensorDevice(projectid, sensorDevice->id);
    if (sensorExisted == NULL)
    {
        WorkItem * witem = new WorkItem("sensordevice", projectid, sensorDevice);
        mMySQLJobQueue.add(witem);

        mDeviceManager->setSensorDeviceAlive(projectid, sensorDevice);
        mDeviceManager->setSensorDeviceValue(projectid, sensorDevice->id, sensorDevice->Battery, currentDateTimeForSQL());
    }
    else
    {
        sensorDevice->name = sensorExisted->name;
        sensorDevice->type = "Network Gateway";
        sensorDevice->maingroupid = sensorExisted->maingroupid;

        mDeviceManager->setSensorDeviceAlive(projectid, sensorDevice);
        mDeviceManager->setSensorDeviceValue(projectid, sensorDevice->id, sensorDevice->Battery, currentDateTimeForSQL());

        WorkItem * witem = new WorkItem("sensordevice", projectid, sensorDevice);
        mMySQLJobQueue.add(witem);

        //-------------------
        // WorkItem * witem1 = new WorkItem("sensordevice", projectid, sensorDevice);
        // mSensorBodyInfoPublisher->publish(witem1);
        //-------------------
    }
}

void MQTTProcessingService::process_iCover(int projectid, const char * loraaddr, const char * tagaddr, const char * value, const char* gatewayid, int rssi, const char * timestamp)
{
    vilslog::printf("MQTTProcessingService::process_iCover() loraaddr[%s] tagaddr[%s]\n", loraaddr, tagaddr);

    Json::Value JSONLora;
    Json::Reader * readerData = new Json::Reader();
    bool parsingSuccessful = readerData->parse( value, JSONLora);
    delete readerData;

    if (!parsingSuccessful)
    {
        vilslog::printf("MQTTProcessingService::process_iCover() parsingSuccessful[%d]\n", parsingSuccessful);

        return;
    }

    std::string SW;
    std::string IMU;
    std::string WU;
    std::string Volt;
    std::string RSSI;

    if ( JSONLora.isMember("SW") )
        SW = JSONLora["SW"].asString();
    if ( JSONLora.isMember("IMU") )
        IMU = JSONLora["IMU"].asString();
    if ( JSONLora.isMember("WU") )
        WU = JSONLora["WU"].asString();
    if ( JSONLora.isMember("Volt") )
        Volt = JSONLora["Volt"].asString();
    if ( JSONLora.isMember("RSSI") )
        RSSI = JSONLora["RSSI"].asString();

    SensorDeviceStruct * sensorDevice = mDeviceManager->get_SensorDevice(projectid, tagaddr);
    if (sensorDevice == NULL)
    {
        int maingroupid = 1;
        sensorDevice = new SensorDeviceStruct();
        sensorDevice->alive = 1;
        sensorDevice->id = tagaddr;
        sensorDevice->name = tagaddr;
        sensorDevice->type = "iCover";
        sensorDevice->jsonvalue = value;
        sensorDevice->Longitude = "";
        sensorDevice->Latitude = "";
        sensorDevice->Battery = Volt;
        sensorDevice->Version = "";
        sensorDevice->datetime = currentDateTimeForSQL();
        sensorDevice->maingroupid = maingroupid;

        WorkItem * witem = new WorkItem("sensordevice", projectid, sensorDevice);
        mMySQLJobQueue.add(witem);

        mDeviceManager->setSensorDeviceAlive(projectid, sensorDevice);

        delete sensorDevice;
    }
    else
    {
        sensorDevice->type = "iCover";
        sensorDevice->jsonvalue = value;
        sensorDevice->Battery = Volt;
        sensorDevice->datetime = currentDateTimeForSQL();
        mDeviceManager->setSensorDeviceAlive(projectid, sensorDevice);

        WorkItem * witem = new WorkItem("sensordevice", projectid, sensorDevice);
        mMySQLJobQueue.add(witem);
    }

    mDeviceManager->setSensorDeviceValue(projectid, tagaddr, Volt, currentDateTimeForSQL());


    SensorInfoStruct * sensor = new SensorInfoStruct();
    sensor->id = -1;
    sensor->name = tagaddr;
    sensor->day = TodayDateType1();
    sensor->value = Volt;
    sensor->jsonvalue = value;
    sensor->Longitude = "";
    sensor->Latitude = "";
    sensor->datetime = currentDateTimeForSQL();
    sensor->maingroups = sensorDevice->maingroupid;

    WorkItem * witem = new WorkItem("sensorinfo", projectid, sensor);
    witem->setMaingroupid(sensorDevice->maingroupid);
    mMySQLJobQueue.add(witem);

    delete sensor;

    // {"SW":"F","IMU":"0.07;0.22;-1.02;-0.42;0.28;-0.14","Volt":"3.44","RSSI","21"}
    if (SW.compare("T") == 0)
    {
        std::string action = "iCoverOpenDoor";
        std::string eventjson = convert_to_json_str(projectid, 777777, tagaddr, action, "", "0", loraaddr);

        WorkItem * witem = new WorkItem("iCoverOpenDoor", projectid, NODE_TYPE_SENSOR, tagaddr, eventjson.c_str());
        witem->setMaingroupid(sensorDevice->maingroupid);
        mMySQLJobQueue.add(witem);

        // int alertType = 2; //1:人員 2:固定裝置
        // int alertAction = 1;// 1:In 2:Out 3:alert 4:cancel 5:poweroff
        // SubscribeDataItem * subitem = new SubscribeDataItem("event", projectid, 7777, "", alertType, alertAction);
        // subitem->setMaingroupid(sensorDevice->maingroupid);
        // mWebSocketqueue.add(subitem);
    }
    else
    {
        std::string action = "iCoverCloseDoor";
        std::string eventjson = convert_to_json_str(projectid, 777776, tagaddr, action, "", "0", loraaddr);

        WorkItem * witem = new WorkItem("iCoverCloseDoor", projectid, NODE_TYPE_SENSOR, tagaddr, eventjson.c_str());
        witem->setMaingroupid(sensorDevice->maingroupid);
        mMySQLJobQueue.add(witem);

        // int alertType = 2; //1:人員 2:固定裝置
        // int alertAction = 2;// 1:In 2:Out 3:alert 4:cancel 5:poweroff
        // SubscribeDataItem * subitem = new SubscribeDataItem("event", projectid, 7777, "", alertType, alertAction);
        // subitem->setMaingroupid(sensorDevice->maingroupid);
        // mWebSocketqueue.add(subitem);
    }
}

void MQTTProcessingService::process_wentaitek_s3_watch_device(int projectid, wentaitek_s3_watch_deviceStruct * watchDevice)
{
    std::string Device_ID = "wentaitek_watch_";
    Device_ID += watchDevice->Device_ID;

    std::string jsonvalue = pack_wentaitek_s3_watch_device(watchDevice);

    vilslog::printf("MQTTProcessingService::process_wentaitek_s3_watch_device() jsonvalue[%s]\n", jsonvalue.c_str());

    SensorDeviceStruct * sensorDevice = mDeviceManager->get_SensorDevice(projectid, Device_ID);
    if (sensorDevice == NULL)
    {
        int maingroupid = 1;
        sensorDevice = new SensorDeviceStruct();
        sensorDevice->alive = 1;
        sensorDevice->id = Device_ID;
        sensorDevice->name = Device_ID;
        sensorDevice->type = "wentaitek_watch_s3";
        sensorDevice->jsonvalue = jsonvalue;
        sensorDevice->Longitude = "";
        sensorDevice->Latitude = "";
        sensorDevice->Battery = "";
        sensorDevice->Version = "";
        sensorDevice->datetime = currentDateTimeForSQL();
        sensorDevice->maingroupid = maingroupid;

        WorkItem * witem = new WorkItem("sensordevice", projectid, sensorDevice);
        mMySQLJobQueue.add(witem);

        mDeviceManager->setSensorDeviceAlive(projectid, sensorDevice);

        delete sensorDevice;
    }
    else
    {
        sensorDevice->type = "wentaitek_watch_s3";
        sensorDevice->jsonvalue = jsonvalue;
        sensorDevice->datetime = currentDateTimeForSQL();
        mDeviceManager->setSensorDeviceAlive(projectid, sensorDevice);

        WorkItem * witem = new WorkItem("sensordevice", projectid, sensorDevice);
        mMySQLJobQueue.add(witem);
    }

    mDeviceManager->setSensorDeviceValue(projectid, Device_ID.c_str(), std::to_string(watchDevice->Heart_Rate), currentDateTimeForSQL());

    //=========================
    // save sensor data
    SensorInfoStruct * sensor = new SensorInfoStruct();
    sensor->id = -1;
    sensor->name = Device_ID;
    sensor->day = TodayDateType1();
    sensor->value = std::to_string(watchDevice->Heart_Rate);
    sensor->jsonvalue = jsonvalue;
    sensor->Longitude = "";
    sensor->Latitude = "";
    sensor->datetime = currentDateTimeForSQL();
    sensor->maingroups = sensorDevice->maingroupid;

    WorkItem * witem = new WorkItem("sensorinfo", projectid, sensor);
    witem->setMaingroupid(sensorDevice->maingroupid);
    mMySQLJobQueue.add(witem);

    delete sensor;
    //=========================

    //=========================
    // repeater to locator
    //
    std::string repeater_mac = watchDevice->repeater_mac;
    replaceAll(repeater_mac, ":", "");
    repeater_mac += "0000";
    checkLocatorExisted(projectid, repeater_mac.c_str());
    //=========================

    // ==============================================
    // Ranging
    int ranging = rssi_to_distance(watchDevice->Rssi);
    std::string tagaddr = Device_ID;
    std::string loraaddr = repeater_mac;
    int tagIndex = mDeviceManager->getTagIndex(projectid, tagaddr.c_str());
    if (tagIndex < 0)
    {
        mDeviceManager->setTagIndex(projectid, tagaddr.c_str(), tagIndex, "");
        tagIndex = mDeviceManager->getTagIndex(projectid, tagaddr.c_str());
    }

    mDeviceManager->setTagAlive(projectid, tagaddr.c_str());

    DataItem * senditem = new DataItem("loraranging", projectid, "LoRa", loraaddr.c_str(), tagaddr.c_str(), ranging, watchDevice->Rssi, currentTimeForSQL().c_str());
    mPosCalJobQueue.add(senditem);
    // ==============================================


    // ==============================================
    // Body Info
    UserStruct * userLink = get_sensor_user(projectid, Device_ID);

    if ( userLink != NULL )
    {
        // vilslog::printf("WebUDPParserService::process_wentaitek_s3_watch_device() Device_ID[%s] Heart_Rate[%d] Blood_Oxygen[%d]\n",
        //     Device_ID.c_str(), watchDevice->Heart_Rate, watchDevice->Blood_Oxygen);

        DataItem * senditem = new DataItem("savebodyinfoall", projectid, userLink->account.c_str());
        senditem->setHeartBeat(std::to_string(watchDevice->Heart_Rate));
        senditem->setBloodOxygen(std::to_string(watchDevice->Blood_Oxygen));
        senditem->setSystolicBloodPressure(std::to_string(watchDevice->SBP));
        senditem->setDiastolicBloodPressure(std::to_string(watchDevice->DBP));
        senditem->setBodyTemperature(std::to_string(watchDevice->Temperature));
        senditem->setDateTime(currentDateTimeForSQL());
        mMainJobQueue.add(senditem);
    }
    // ==============================================

}

void MQTTProcessingService::process_wentaitek_uw_sensor_1_device(int projectid, wentaitek_uw_sensor_1_deviceStruct * uwsensorDevice)
{
    std::string Device_ID = "wentaitek_uw_";
    Device_ID += uwsensorDevice->Device_ID;

    std::string jsonvalue = pack_wentaitek_uw_sensor_1_device(uwsensorDevice);

    SensorDeviceStruct * sensorDevice = mDeviceManager->get_SensorDevice(projectid, Device_ID);
    if (sensorDevice == NULL)
    {
        int maingroupid = 1;
        sensorDevice = new SensorDeviceStruct();
        sensorDevice->alive = 1;
        sensorDevice->id = Device_ID;
        sensorDevice->name = Device_ID;
        sensorDevice->type = "wentaitek_uw_sensor_1";
        sensorDevice->jsonvalue = jsonvalue;
        sensorDevice->Longitude = "";
        sensorDevice->Latitude = "";
        sensorDevice->Battery = "";
        sensorDevice->Version = "";
        sensorDevice->datetime = currentDateTimeForSQL();
        sensorDevice->maingroupid = maingroupid;

        WorkItem * witem = new WorkItem("sensordevice", projectid, sensorDevice);
        mMySQLJobQueue.add(witem);

        mDeviceManager->setSensorDeviceAlive(projectid, sensorDevice);

        delete sensorDevice;
    }
    else
    {
        sensorDevice->type = "wentaitek_uw_sensor_1";
        sensorDevice->jsonvalue = jsonvalue;
        sensorDevice->datetime = currentDateTimeForSQL();
        mDeviceManager->setSensorDeviceAlive(projectid, sensorDevice);

        WorkItem * witem = new WorkItem("sensordevice", projectid, sensorDevice);
        mMySQLJobQueue.add(witem);
    }

    mDeviceManager->setSensorDeviceValue(projectid, Device_ID.c_str(), std::to_string(uwsensorDevice->Rssi), currentDateTimeForSQL());


    //=========================
    // save sensor data
    SensorInfoStruct * sensor = new SensorInfoStruct();
    sensor->id = -1;
    sensor->name = Device_ID;
    sensor->day = TodayDateType1();
    sensor->value = std::to_string(uwsensorDevice->Rssi);
    sensor->jsonvalue = jsonvalue;
    sensor->Longitude = "";
    sensor->Latitude = "";
    sensor->datetime = currentDateTimeForSQL();
    sensor->maingroups = sensorDevice->maingroupid;

    WorkItem * witem = new WorkItem("sensorinfo", projectid, sensor);
    witem->setMaingroupid(sensorDevice->maingroupid);
    mMySQLJobQueue.add(witem);

    delete sensor;
    //=========================

    //=========================
    // repeater to locator
    //
    std::string repeater_mac = uwsensorDevice->repeater_mac;
    replaceAll(repeater_mac, ":", "");
    repeater_mac += "0000";
    checkLocatorExisted(projectid, repeater_mac.c_str());
    //=========================

}

void MQTTProcessingService::process_wentaitek_light(int projectid, wentaitek_lightStruct * lightDevice)
{
    std::string Device_ID = "wentaitek_light_";
    Device_ID += std::to_string(lightDevice->id);

    std::string jsonvalue = pack_wentaitek_light_device(lightDevice);

    SensorDeviceStruct * sensorDevice = mDeviceManager->get_SensorDevice(projectid, Device_ID);
    if (sensorDevice == NULL)
    {
        int maingroupid = 1;
        sensorDevice = new SensorDeviceStruct();
        sensorDevice->alive = 1;
        sensorDevice->id = Device_ID;
        sensorDevice->name = lightDevice->name;
        sensorDevice->type = "wentaitek_light";
        sensorDevice->jsonvalue = jsonvalue;
        sensorDevice->Longitude = "";
        sensorDevice->Latitude = "";
        sensorDevice->Battery = "";
        sensorDevice->Version = "";
        sensorDevice->datetime = currentDateTimeForSQL();
        sensorDevice->maingroupid = maingroupid;

        WorkItem * witem = new WorkItem("sensordevice", projectid, sensorDevice);
        mMySQLJobQueue.add(witem);

        mDeviceManager->setSensorDeviceAlive(projectid, sensorDevice);

        delete sensorDevice;
    }
    else
    {
        sensorDevice->type = "wentaitek_light";
        sensorDevice->jsonvalue = jsonvalue;
        sensorDevice->datetime = currentDateTimeForSQL();
        mDeviceManager->setSensorDeviceAlive(projectid, sensorDevice);

        WorkItem * witem = new WorkItem("sensordevice", projectid, sensorDevice);
        mMySQLJobQueue.add(witem);
    }

    mDeviceManager->setSensorDeviceValue(projectid, Device_ID.c_str(), std::to_string(lightDevice->state_level), currentDateTimeForSQL());

    //=========================
    // save sensor data
    SensorInfoStruct * sensor = new SensorInfoStruct();
    sensor->id = -1;
    sensor->name = Device_ID;
    sensor->day = TodayDateType1();
    sensor->value = std::to_string(lightDevice->state_level);
    sensor->jsonvalue = jsonvalue;
    sensor->Longitude = "";
    sensor->Latitude = "";
    sensor->datetime = currentDateTimeForSQL();
    sensor->maingroups = sensorDevice->maingroupid;

    WorkItem * witem = new WorkItem("sensorinfo", projectid, sensor);
    witem->setMaingroupid(sensorDevice->maingroupid);
    mMySQLJobQueue.add(witem);

    delete sensor;
}

void MQTTProcessingService::process_wentaitek_power_meter_device(int projectid, wentaitek_power_meter_deviceStruct * sensorPowerMeterDevice)
{
    std::string Device_ID = "wentaitek_pm_";
    Device_ID += sensorPowerMeterDevice->Device_ID;

    std::string jsonvalue = pack_wentaitek_power_meter_device(sensorPowerMeterDevice);

    SensorDeviceStruct * sensorDevice = mDeviceManager->get_SensorDevice(projectid, Device_ID);
    if (sensorDevice == NULL)
    {
        vilslog::printf("MQTTProcessingService::process_wentaitek_power_meter_device IN");

        int maingroupid = 1;
        sensorDevice = new SensorDeviceStruct();
        sensorDevice->alive = 1;
        sensorDevice->id = Device_ID;
        sensorDevice->name = Device_ID;
        sensorDevice->type = "wentaitek_power_meter";
        sensorDevice->jsonvalue = jsonvalue;
        sensorDevice->Longitude = "";
        sensorDevice->Latitude = "";
        sensorDevice->Battery = "";
        sensorDevice->Version = sensorPowerMeterDevice->BT_version;
        sensorDevice->datetime = currentDateTimeForSQL();
        sensorDevice->maingroupid = maingroupid;

        vilslog::printf("MQTTProcessingService::process_wentaitek_power_meter_device DataGet");

        WorkItem * witem = new WorkItem("sensordevice", projectid, sensorDevice);
        mMySQLJobQueue.add(witem);

        mDeviceManager->setSensorDeviceAlive(projectid, sensorDevice);

        delete sensorDevice;
    }
    else
    {
        sensorDevice->type = "wentaitek_power_meter";
        sensorDevice->jsonvalue = jsonvalue;
        sensorDevice->datetime = currentDateTimeForSQL();
        mDeviceManager->setSensorDeviceAlive(projectid, sensorDevice);

        WorkItem * witem = new WorkItem("sensordevice", projectid, sensorDevice);
        mMySQLJobQueue.add(witem);
    }

    mDeviceManager->setSensorDeviceValue(projectid, Device_ID.c_str(), std::to_string(sensorPowerMeterDevice->Frequency), currentDateTimeForSQL());

    //=========================
    // save sensor data
    SensorInfoStruct * sensor = new SensorInfoStruct();
    sensor->id = -1;
    sensor->name = Device_ID;
    sensor->day = TodayDateType1();
    sensor->value = sensorPowerMeterDevice->Frequency;
    sensor->jsonvalue = jsonvalue;
    sensor->Longitude = "";
    sensor->Latitude = "";
    sensor->datetime = currentDateTimeForSQL();
    sensor->maingroups = sensorDevice->maingroupid;

    WorkItem * witem = new WorkItem("sensorinfo", projectid, sensor);
    witem->setMaingroupid(sensorDevice->maingroupid);
    mMySQLJobQueue.add(witem);

    delete sensor;
    //=========================

    //=========================
    // repeater to locator
    //
    std::string repeater_mac = sensorPowerMeterDevice->repeater_mac;
    replaceAll(repeater_mac, ":", ""); // 0008dc36060b
    repeater_mac += "0000";
    checkLocatorExisted(projectid, repeater_mac.c_str());
    //=========================

}


bool MQTTProcessingService::checkLocatorExisted(int projectid, const char * loraaddr)
{
    //
    // check of locator exist
    //
    RFIDDeviceStruct * rfidDevice = mDeviceManager->get_RFIDDeviceByLoRAmacaddress(projectid, loraaddr);
    if (rfidDevice != NULL)
    {
        //vilslog::printf("MQTTProcessingService::checkLocatorExisted() loraaddr[%s] NODE_TYPE_UHF_READER\n", loraaddr);
        return true;
    }
    else
    {
        NFCDeviceStruct * nfcDevice = mDeviceManager->get_NFCDeviceByLoRAmacaddress(projectid, loraaddr);
        if (nfcDevice != NULL)
        {
            //vilslog::printf("MQTTProcessingService::checkLocatorExisted() loraaddr[%s] NODE_TYPE_NFC_LOCATOR\n", loraaddr);
            return true;
        }
        else
        {
            char locatorID[64];
            snprintf(locatorID, 64, "%s", mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, loraaddr));
            if (strlen(locatorID) == 0)
            {
                // 0000000002070001 or 0000000000050001 or 0000000001060001
                char version = loraaddr[11];

                char locatoraddr[64];
                snprintf(locatoraddr, 64, "%s", "0530000000000000");
                locatoraddr[1] = version;

                //vilslog::printf("MQTTProcessingService::checkLocatorExisted() version[%c] locatoraddr[%s]\n", version, locatoraddr);

                locatoraddr[12] = loraaddr[12];
                locatoraddr[13] = loraaddr[13];
                locatoraddr[14] = loraaddr[14];
                locatoraddr[15] = loraaddr[15];

                mDeviceManager->setLocatorIndex(projectid, locatoraddr, -1, "");
                mDeviceManager->setLocatorInfoLoRA(projectid, locatoraddr, loraaddr);
                snprintf(locatorID, 64, "%s", mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, loraaddr));

                if ( version == '7' )
                {
                    mDeviceManager->updateAoAList(projectid, loraaddr);
                }

                return false;
            }

            return true;
        }
    }
}

bool MQTTProcessingService::checkLocatorExisted(int projectid, const char * loraaddr, const char * macaddress)
{
    //
    // check of locator exist
    //
    RFIDDeviceStruct * rfidDevice = mDeviceManager->get_RFIDDevice(projectid, macaddress);
    if (rfidDevice != NULL)
    {
        vilslog::printf("MQTTProcessingService::checkLocatorExisted() macaddress[%s] NODE_TYPE_UHF_READER\n", macaddress);
        return true;
    }
    else
    {
        NFCDeviceStruct * nfcDevice = mDeviceManager->get_NFCDevice(projectid, macaddress);
        if (nfcDevice != NULL)
        {
            vilslog::printf("MQTTProcessingService::checkLocatorExisted() macaddress[%s] NODE_TYPE_NFC_LOCATOR\n", macaddress);
            return true;
        }
        else
        {
            char locatorID[64];
            snprintf(locatorID, 64, "%s", mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, loraaddr));
            if (strlen(locatorID) == 0)
            {
                vilslog::printf("MQTTProcessingService::checkLocatorExisted() locatorID[%s] loraaddr[%s] macaddress[%s]\n", locatorID, loraaddr, macaddress);

                mDeviceManager->setLocatorIndex(projectid, macaddress, -1, "");
                mDeviceManager->setLocatorInfoLoRA(projectid, macaddress, loraaddr);
                snprintf(locatorID, 64, "%s", mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, loraaddr));

                char version = macaddress[1];
                if ( version == '7' )
                {
                    mDeviceManager->updateAoAList(projectid, loraaddr);
                }

                return false;
            }
            else
            {
                vilslog::printf("MQTTProcessingService::checkLocatorExisted() loraaddr[%s] macaddress[%s]\n", loraaddr, macaddress);

                //
                // update macaddress
                //
                mDeviceManager->setLocatorInfoMacAddress(projectid, loraaddr, macaddress);
                mDeviceManager->setAoAInfoMacAddress(projectid, locatorID, macaddress);

                char version = macaddress[1];
                if ( version == '7' )
                {
                    char AoAID[64];
                    snprintf(AoAID, 64, "%s", mDeviceManager->getAoAId(projectid, macaddress));
                    if (strlen(AoAID) == 0)
                    {
                        mDeviceManager->updateAoAList(projectid, loraaddr);
                    }
                }
            }
            return true;
        }
    }
}

int MQTTProcessingService::create_unknown_card_event(int projectid, const char * cardtype, std::string cardid, std::string loramacaddress, int maingroupid)
{
    vilslog::printf("MQTTProcessingService::create_unknown_card_event() cardtype[%s] cardid[%s]\n", cardtype, cardid.c_str());

    std::string eventJsonString = convert_unknown_card_event_to_json_str(projectid, cardtype, cardid, loramacaddress);

    //================================
    FiredEventStruct * newFiredEvents = new FiredEventStruct();
    newFiredEvents->projectid = projectid;
    newFiredEvents->eventid = 111111;
    newFiredEvents->eventjson = eventJsonString;
    newFiredEvents->timestart = currentDateTimeForSQL().c_str();
    newFiredEvents->timeend = currentDateTimeForSQL().c_str();

    WorkItem * witem = new WorkItem("event", projectid, newFiredEvents);
    witem->setMaingroupid(maingroupid);
    mMySQLJobQueue.add(witem);

    delete newFiredEvents;

    int alertType = 1; //1:人員 2:固定裝置
    int alertAction = 1;// 1:In 2:Out 3:alert 4:cancel
    SubscribeDataItem * item = new SubscribeDataItem("event", projectid, 111111, cardid.c_str(), alertType, alertAction);
    item->setMaingroupid(maingroupid);
    mWebSocketqueue.add(item);
    //================================

    return 0;
}

std::string MQTTProcessingService::convert_unknown_card_event_to_json_str(int projectid, const char * cardtype, std::string cardid, std::string loramacaddress)
{
    std::string json_str = "{";

    char tmp[64];
    json_str += "\"type\":\"" + std::string(cardtype) + "\",";
    json_str += "\"eventid\":\"111111\",";

    snprintf(tmp, 64, "%d", projectid);
    json_str += "\"projectid\":\"" + std::string(tmp) + "\",";

    json_str += "\"cardid\":\"" + cardid + "\",";
    json_str += "\"loramacaddress\":\"" + loramacaddress + "\",";
    json_str += "\"action\":\"unknowncard\",";
    json_str += "\"issuetime\":\"" + currentDateTimeForSQL() + "\"";
    json_str += "}";

    return json_str;
}

UserStruct * MQTTProcessingService::update_sensor_user_location(int projectid, std::string deviceid, std::string Longitude, std::string Latitude)
{
    UserStruct * userLink = NULL;

    // vilslog::printf("MQTTProcessingService::update_sensor_user_location() projectid[%d] deviceid[%s] Latitude[%s] Longitude[%s]\n",
    //     projectid, deviceid.c_str(), Latitude.c_str(), Longitude.c_str());

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

                // vilslog::printf("MQTTProcessingService::update_sensor_user_location() updateUserStruct userid[%d] Latitude[%s] Longitude[%s]\n",
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

UserStruct * MQTTProcessingService::get_sensor_user(int projectid, std::string deviceid)
{
    UserStruct * userLink = NULL;
    std::list<UserStruct*> * userList = mDeviceManager->get_User_list(projectid);

    for (std::list<UserStruct*>::iterator user = userList->begin(); user != userList->end(); user++)
    {
        for (std::list<std::string>::iterator sensor = (*user)->sensors->begin(); sensor != (*user)->sensors->end(); sensor++)
        {
            if ( (*sensor).compare(deviceid) == 0)
            {
                userLink = (*user);
                break;
            }
        }
    }
    return userLink;
}

std::string MQTTProcessingService::pack_wentaitek_s3_watch_device(wentaitek_s3_watch_deviceStruct * watchDevice)
{
    std::ostringstream sendstream;

    sendstream << "{\"Device_ID\":\""<< watchDevice->Device_ID<<"\"";
    sendstream << ",\"Blood_Oxygen\":"<< watchDevice->Blood_Oxygen;
    sendstream << ",\"DBP\":"<< watchDevice->DBP;
    sendstream << ",\"Heart_Rate\":"<< watchDevice->Heart_Rate;
    sendstream << ",\"Rssi\":"<< watchDevice->Rssi;
    sendstream << ",\"SBP\":"<< watchDevice->SBP;
    sendstream << ",\"SOS\":"<< watchDevice->SOS;
    sendstream << ",\"Temperature\":"<< watchDevice->Temperature;
    sendstream << "}";

    std::string json = sendstream.str();

    return json;
}

std::string MQTTProcessingService::pack_wentaitek_uw_sensor_1_device(wentaitek_uw_sensor_1_deviceStruct * uwsensorDevice)
{
    std::ostringstream sendstream;

    sendstream << "{\"Device_ID\":\""<< uwsensorDevice->Device_ID<<"\"";
    sendstream << ",\"Rssi\":"<< uwsensorDevice->Rssi;
    sendstream << "}";

    std::string json = sendstream.str();

    return json;
}

std::string MQTTProcessingService::pack_wentaitek_light_device(wentaitek_lightStruct * lightDevice)
{
    std::ostringstream sendstream;

    sendstream << "{\"id\":"<< lightDevice->id;
    sendstream << ",\"name\":\""<< lightDevice->name<<"\"";
    sendstream << ",\"uniAddress\":"<< lightDevice->uniAddress;
    sendstream << ",\"state_onOff\":"<< lightDevice->state_onOff;
    sendstream << ",\"state_level\":"<< lightDevice->state_level;
    sendstream << ",\"state_cct\":"<< lightDevice->state_cct;
    sendstream << "}";

    std::string json = sendstream.str();

    return json;
}

std::string MQTTProcessingService::pack_wentaitek_power_meter_device(wentaitek_power_meter_deviceStruct * sensorPowerMeterDevice)
{
    std::ostringstream sendstream;

    sendstream << "{\"Device_ID\":\""<< sensorPowerMeterDevice->Device_ID<<"\"";
    sendstream << ",\"BT_version\":\""<< sensorPowerMeterDevice->BT_version<<"\"";
    sendstream << ",\"Rssi\":"<< sensorPowerMeterDevice->Rssi;
    sendstream << ",\"Active_Energy\":"<< sensorPowerMeterDevice->Active_Energy;
    sendstream << ",\"Active_Power\":"<< sensorPowerMeterDevice->Active_Power;
    sendstream << ",\"Current_RMS\":"<< sensorPowerMeterDevice->Current_RMS;
    sendstream << ",\"Frequency\":"<< sensorPowerMeterDevice->Frequency;
    sendstream << ",\"Voltage_RMS\":"<< sensorPowerMeterDevice->Voltage_RMS;
    sendstream << ",\"relay_state\":"<< sensorPowerMeterDevice->relay_state;
    sendstream << "}";

    std::string json = sendstream.str();

    return json;
}
