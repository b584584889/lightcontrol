/*
 *  MQTTProcessingService.h
 *
 *  Created on: 2018/10/19
 *      Author: ikki
 */
#ifndef __MQTTProcessingService_h__
#define __MQTTProcessingService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../mysql/MySQLDBAccess.h"
#include "../DeviceManager/DeviceManager.h"
#include "../websocket/SubscribeDataItem.h"
#include "../network/wqueue.h"
#include "../network/WorkItem.h"
#include "../network/DataItem.h"
#include "../network/NotificationItem.h"
#include "../network/inter_thread_pubsub.hpp"
#include "../MQTT/MQTTDataItem.h"
#include "../config.h"

using namespace std;

//#define MQTT_PRINT_INCOMMING_MSG

class MQTTProcessingService : public Thread
{
public:
    MQTTProcessingService(wqueue<DataItem*> & MainJobQueue, wqueue<MQTTDataItem*> & pMQTTqueue, wqueue<SubscribeDataItem*> & pWebSocketqueue, wqueue<NotificationItem*> & NotifiactionQueue, wqueue<WorkItem*> & SensorInfoJobQueue, wqueue<WorkItem*> & MySQLJobQueue, wqueue<DataItem*> & PosCalJobQueue, wqueue<WorkItem*> & nodePositionQueue, wqueue<DataItem*> & inspectionJobQueue, wqueue<DataItem*> & CARDRollCallJobQueue, DeviceManager * pDevManager);
    bool status();
    std::string statusString();

    void * run();

private:

    std::string mStatus;
    bool mInitial;

    wqueue<DataItem*> & mMainJobQueue;
    wqueue<MQTTDataItem*> & mMQTTqueue;
    wqueue<SubscribeDataItem*> & mWebSocketqueue;
    wqueue<NotificationItem*> & mNotifiactionQueue;
    wqueue<WorkItem*> & mSensorInfoJobQueue;
    wqueue<WorkItem*> & mMySQLJobQueue;
    wqueue<DataItem*> & mPosCalJobQueue;
    wqueue<WorkItem*> & mNodePositionQueue;
    wqueue<DataItem*> & mInspectionJobQueue;
    wqueue<DataItem*> & mCARDRollCallJobQueue;

    ITPS::Publisher<DataItem *> * mNFCCardPublisher;
    ITPS::Publisher<DataItem *> * mAoARangingPublisher;

    DeviceManager * mDeviceManager;

    void initQueue();
    void runloop();
    void waitloop();

    void process_alert(int projectid, const char * loraaddr, const char * tagaddr, const char* gatewayid);
    void process_alert_Tag(int projectid, const char * loraaddr, const char * tagaddr, const char* gatewayid);
    // void process_alert_NFCDevice(int projectid, const char * loraaddr, const char * macaddress, const char* gatewayid);
    // void process_alert_RFIDDevice(int projectid, const char * loraaddr, const char * macaddress, const char* gatewayid);
    void process_alert_Locator(int projectid, const char * loraaddr, const char * macaddress, const char* gatewayid);
    void process_ranging(int projectid, const char * loraaddr, const char * tagaddr, int ranging, const char * voltage, const char* gatewayid, int rssi, const char*timestamp);
    void process_rangingcompress(int projectid, const char * loraaddr, const char * tagaddr, int ranging, const char * voltage, int velocity, const char* gatewayid, int rssi, const char*timestamp);
    void process_rangingcompressMultiDistance(int projectid, const char * loraaddr, const char * tagaddr, int distInterval, std::list<int> * distList, const char * voltage, int velocity, const char* gatewayid, int rssi, const char*timestamp);
    void process_rangingcompressaoa(int projectid, const char * loraaddr, const char * tagaddr, int ranging, const char * voltage, int velocity, float angle, const char* gatewayid, int rssi, int * IMU, const char*timestamp);
    void process_cancel(int projectid, const char * loraaddr, const char * tagaddr, const char* gatewayid);
    void process_taggps(int projectid, const char * loraaddr, double Longitude, double Latitude, const char* gatewayid, const char * timestamp);
    void process_locatorgps(int projectid, const char * loraaddr, double Longitude, double Latitude, const char* gatewayid, const char * timestamp);
    void process_loragateway(int projectid, LoRaGatewayInfoStruct * gateway);
    void process_MQTTgateway(int projectid, LoRaGatewayInfoStruct * gateway);
    void process_AOAfence(int projectid, const char * loraaddr, const char * tagaddr, const char * lv, const char * time, const char* gatewayid, int rssi, const char*timestamp);
    void process_LiDARInfo(int projectid, const char * loraaddr, const char * value, const char* gatewayid, int rssi, const char * timestamp);
    void process_PowerSwitchInfo(int projectid, const char * loraaddr, const char * value, const char* gatewayid, int rssi, const char * timestamp);
    void process_beacon(int projectid, const char * loraaddr, const char * tagaddr, const char * value, const char* gatewayid, int rssi, const char * timestamp);
    void process_WatchInfo(int projectid, SensorDeviceStruct * sensorDevice);
    void process_NetworkGateway(int projectid, SensorDeviceStruct * sensorDevice);
    void process_iCover(int projectid, const char * loraaddr, const char * tagaddr, const char * value, const char* gatewayid, int rssi, const char * timestamp);
    void process_wentaitek_s3_watch_device(int projectid, wentaitek_s3_watch_deviceStruct * watchDevice);
    void process_wentaitek_uw_sensor_1_device(int projectid, wentaitek_uw_sensor_1_deviceStruct * uwsensorDevice);
    void process_wentaitek_light(int projectid, wentaitek_lightStruct * lightDevice);
    void process_wentaitek_power_meter_device(int projectid, wentaitek_power_meter_deviceStruct * sensorPowerMeterDevice);

    int CheckNodeType(int projectid, const char * macaddress);
    bool checkLocatorExisted(int projectid, const char * loraaddr);
    bool checkLocatorExisted(int projectid, const char * loraaddr, const char * macaddress);
    int create_unknown_card_event(int projectid, const char * cardtype, std::string cardid, std::string loramacaddress, int maingroupid);
    std::string convert_unknown_card_event_to_json_str(int projectid, const char * cardtype, std::string cardid, std::string loramacaddress);
    UserStruct * update_sensor_user_location(int projectid, std::string deviceid, std::string Longitude, std::string Latitude);
    UserStruct * get_sensor_user(int projectid, std::string deviceid);

    std::string pack_wentaitek_s3_watch_device(wentaitek_s3_watch_deviceStruct * watchDevice);
    std::string pack_wentaitek_uw_sensor_1_device(wentaitek_uw_sensor_1_deviceStruct * uwsensorDevice);
    std::string pack_wentaitek_light_device(wentaitek_lightStruct * lightDevice);
    std::string pack_wentaitek_power_meter_device(wentaitek_power_meter_deviceStruct * sensorPowerMeterDevice);
};


#endif
