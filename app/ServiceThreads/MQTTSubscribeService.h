/*
 *  MQTTSubscribeService.h
 *
 *  Created on: 2018/08/03
 *      Author: ikki
 */
#ifndef __MQTTSubscribeService_h__
#define __MQTTSubscribeService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../network/wqueue.h"
#include "../network/DataItem.h"
#include "../network/inter_thread_pubsub.hpp"

#include "../MQTT/MQTTClient/MQTTClient.h"
#include "../MQTT/MQTTClient/linux.h"
#include "../MQTT/MQTTDataItem.h"
#include "../mysql/DBManager.h"
#include "../DeviceManager/DeviceManager.h"
#include "../config.h"

using namespace std;

// #define PRINT_MQTT_SUB_MSG
// #define PRINT_MQTT_compress_multiple_data
// #define PRINT_MQTT_compress_AOA_data
// #define PRINT_MQTT_compress_AOA_IMU_data

class MQTTSubscribeService : public Thread
{
public:
    MQTTSubscribeService(wqueue<DataItem*> & MainJobQueue, wqueue<MQTTDataItem*> & queueSub, wqueue<MQTTDataItem*> & queuePub, DeviceManager * pDevManager, DBManager * dbManager);
    bool status();
    std::string statusString();
    void updateProject();

    void * run();

private:

    wqueue<DataItem*> & mMainJobQueue;
    wqueue<MQTTDataItem*> & m_queueSub;
    wqueue<MQTTDataItem*> & m_queuePub;
    DeviceManager * mDeviceManager;
    DBManager * mDbManager;

    VILSSystemConfig mSystemConfig;

    bool mInitial;
    std::string mStatus;

    int runloop();
    int connect_to_MQTT(MQTT::Client<IPStack, Countdown> * client, IPStack * ipstack);

};

#endif
