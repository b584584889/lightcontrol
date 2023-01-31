/*
 *  SimulationService.h
 *
 *  Created on: 2018/10/19
 *      Author: ikki
 */
#ifndef __SimulationService_h__
#define __SimulationService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../common.h"
#include "../config.h"
#include "../network/wqueue.h"
#include "../network/DataItem.h"
#include "../network/NotificationItem.h"
#include "../MQTT/MQTTDataItem.h"
#include "../websocket/SubscribeDataItem.h"
#include "../DeviceManager/DeviceManager.h"

using namespace std;

class SimulationService : public Thread
{
public:
    SimulationService(wqueue<DataItem*> & pUDPJobQueue, wqueue<MQTTDataItem*> & pMQTTqueue, wqueue<SubscribeDataItem*> & pWebSocketqueue, wqueue<DataItem*> & PosCalJobQueue, wqueue<NotificationItem*> & NotifiactionQueue, DeviceManager * pDevManager);
    bool status();
    std::string statusString();

    void * run();

private:

    std::string mStatus;
    bool mInitial;

    wqueue<DataItem*> & mUDPJobQueue;
    wqueue<MQTTDataItem*> & mMQTTqueue;
    wqueue<SubscribeDataItem*> & mWebSocketqueue;
    wqueue<DataItem*> & mPosCalJobQueue;
    wqueue<NotificationItem*> & mNotifiactionQueue;
    DeviceManager * mDeviceManager;

    void runloop();
    void waitloop();

    void test_simulation_GPIO_ON();
    void test_simulation_checkpoint();
    void simulation_point_behavior_normal();
    void simulation_point_behavior_unnormal();
    void simulation_point_behavior_unnormal_1();
    void simulation_point_behavior(const char * LoRaMac, const char * TagMac,
                                                      int start_distance, int approaching_time, int approaching_step_distance,
                                                      int leaving_time, int leaving_step_distance);

    void test_simulation_Inspection();
    void test_Calculate_poistion();
    void test_PowerTag();
    void test_Door();
    void test_AoA();
    void test_tag_bodyinfo();
    void test_tag_voltage();
    void test_sos_cancel();
    void test_smoothing_filter();
    void test_simulation_lora();
    void test_simulation_lora_0D();
    void test_simulation_lora_rollcall();
    void test_simulation_lora_GPS();
    void test_simulation_lora_0D_exclude_rollcall_and_GPS();
    void test_simulation_WebSocket();
    void test_simulation_MQTT_data();

    void test_simulation_geteway();
    //void test_mysql_ts();
    void send_MQTT_LoRA_GPS(const char * LoRAMacaddress, const char * macaddress1, double Longitude, double Latitude);
    void create_alert_event(const char * tagmacadddress, const char * tagid);
    void create_alert_event_no_area(int projectid, const char * tagmacadddress, const char * tagid);
    void add_action_join(const char* macaddress, int nodetype, int idx, const char* version);

    int do_parse_compress_AOA_file(std::string path, std::string filename);
    int do_parse_compress_AOA_data(int projectid, const char * LoRAMacAddr, const char * data, const char * gatewayid, int rssi, const char * timestamp);
    void send_LoRaDeviceALIVE_to_projects(const char * LoRAMacAddr, const char * gatewayid, int rssi, int AoAGridSize);
    void send_rangingcompressaoa_to_projects(const char * LoRAMacAddr, const char * macaddress, int distance, const char * voltage, int velocity, float angleFloat, const char * gatewayid, int rssi, const char * timestamp);

};


#endif
