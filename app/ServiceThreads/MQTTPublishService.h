/*
 *  MQTTPublishService.h
 *
 *  Created on: 2019/05/29
 *      Author: ikki
 */
#ifndef __MQTTPublishService_h__
#define __MQTTPublishService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../network/wqueue.h"
#include "../network/DataItem.h"

#include "../MQTT/MQTTClient/MQTTClient.h"
#include "../MQTT/MQTTClient/linux.h"
#include "../MQTT/MQTTDataItem.h"
#include "../mysql/DBManager.h"
#include "../config.h"

using namespace std;

//#define MQTT_PRINT_PUBLISH_MSG

class MQTTPublishService : public Thread
{
public:
    MQTTPublishService(wqueue<MQTTDataItem*> & queue, DBManager * dbManager);
    bool status();
    std::string statusString();

    void * run();

private:

    wqueue<MQTTDataItem*> & mMQTTqueue;
    DBManager * mDbManager;

    VILSSystemConfig mSystemConfig;

    bool mInitial;
    std::string mStatus;

    std::string mPublishTopic;

    int runloop();
    int connect_to_MQTT(MQTT::Client<IPStack, Countdown> * client, IPStack * ipstack);
    int process_publish(MQTT::Client<IPStack, Countdown> * client, const char * gatewayid, const char * payload, int payloadLength);
    int process_publish_wentaitek(MQTT::Client<IPStack, Countdown> * client, const char * gatewayid, const char * payload, int payloadLength);

};

#endif
