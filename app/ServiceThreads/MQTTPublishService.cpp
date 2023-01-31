/*
 *  MQTTPublishService.cpp
 *
 *  Created on: 2019/05/29
 *      Author: ikki
 */

#include "MQTTPublishService.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>

#include <string.h>
#include <iostream>
#include <list>
#include "../json/json.h"

using namespace std;

MQTTPublishService::MQTTPublishService(wqueue<MQTTDataItem*> & queue, DBManager * dbManager)
    : mMQTTqueue(queue), mDbManager(dbManager)
{
    //m_systemConfig = sysConfig;
}

bool MQTTPublishService::status()
{
    return mInitial;
}

std::string MQTTPublishService::statusString()
{
    return mStatus;
}

void * MQTTPublishService::run()
{
    while(1)
    {
        try
        {
            while(1)
            {
                runloop();

                vilslog::printf("MQTTPublishService::run() escape from runloop()\n");
                sleep(5);
            }
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("MQTTPublishService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "MQTTPublishService()::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

int MQTTPublishService::runloop()
{
    int rc = 0;
    mInitial = false;

    IPStack ipstack = IPStack();

    MQTT::Client<IPStack, Countdown> mClient = MQTT::Client<IPStack, Countdown>(ipstack, 30000);

    connect_to_MQTT(&mClient, &ipstack);

    while (1)
    {
        // client->yield(10);
        // int ret = client->yield(10);
        // if (ret < 0)
        // {
        //     vilslog::printf("MQTTPublishService::run() ret[%d] < 0 break while loop \n", ret);
        //     break;
        // }

        MQTTDataItem * item = mMQTTqueue.removetimed(1000);

        if ( item != NULL )
        {
            if ( strcmp(item->getType(), "MQTTpublish") == 0 )
            {
                rc = process_publish(&mClient, item->getGatewayid(), item->getPayload(), item->getPayloadLength());
                if (rc != 0)
                {
                    rc = connect_to_MQTT(&mClient, &ipstack);
                    if (rc != 0)
                    {
                        delete item;
                        break;
                    }
                    else
                    {
                        rc = process_publish(&mClient, item->getGatewayid(), item->getPayload(), item->getPayloadLength());
                        if (rc < 0)
                        {
                            vilslog::printf("MQTTPublishService::run() rc[%d] < 0 break while loop \n", rc);
                            delete item;
                            break;
                        }
                    }
                }
                // 正文可能無法處理
                sleep(1);
            }
            else if (strcmp(item->getType(), "MQTTWentaitek") == 0 )
            {
                vilslog::printf("MQTTPublishService::MQTTWentaitek  IN \n");
                rc = process_publish_wentaitek(&mClient, item->getGatewayid(), item->getPayload(), item->getPayloadLength());
                if (rc != 0)
                {
                    rc = connect_to_MQTT(&mClient, &ipstack);
                    if (rc != 0)
                    {
                        delete item;
                        break;
                    }
                    else
                    {
                        vilslog::printf("MQTTPublishService::MQTTWentaitek  false \n");
                        rc = process_publish_wentaitek(&mClient, item->getGatewayid(), item->getPayload(), item->getPayloadLength());
                        if (rc < 0)
                        {
                            vilslog::printf("MQTTPublishService::run() rc[%d] < 0 break while loop \n", rc);
                            delete item;
                            break;
                        }
                    }
                }
                vilslog::printf("MQTTPublishService::MQTTWentaitek  OUT \n");
                // 正文可能無法處理
                sleep(1);

            }

            delete item;
        }
    }
    return rc;
}

int MQTTPublishService::connect_to_MQTT(MQTT::Client<IPStack, Countdown> * client, IPStack * ipstack)
{
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(1);
    if (baseDb != NULL)
        baseDb->read_sys_config(&mSystemConfig);

    vilslog::printf("MQTTPublishService::run() Connecting to %s:%d\n", mSystemConfig.MQTTHostIp.c_str(), mSystemConfig.MQTTHostPort);
    int rc = ipstack->connect(mSystemConfig.MQTTHostIp.c_str(), mSystemConfig.MQTTHostPort);
    if (rc != 0)
    {
        vilslog::printf("MQTTPublishService::run() rc from ipstack->connect is %d\n", rc);
        mStatus = "MQTTPublishService::run() rc from ipstack->connect is failed!!!";

        rc = client->disconnect();
        if (rc != 0)
            vilslog::printf("MQTTPublishService::run() rc from disconnect was %d\n", rc);

        ipstack->disconnect();

        return rc;
    }
    else
    {
        vilslog::printf("MQTTPublishService::run() MQTT connecting...\n");
        mStatus = "MQTTPublishService::run() MQTT connecting...";

        std::string clientID = mSystemConfig.MQTTClientId.c_str();
        clientID.append("Publish");

        MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
        data.username.cstring = (char*)mSystemConfig.MQTTHostUsername.c_str();
        data.password.cstring = (char*)mSystemConfig.MQTTHostPassword.c_str();
        data.MQTTVersion = 3;
        //data.clientID.cstring = (char*)systemConfig.MQTTClientId.c_str();
        data.clientID.cstring = (char*)clientID.c_str();
        rc = client->connect(data);
        if (rc != 0)
        {
            vilslog::printf("MQTTPublishService::run() rc from MQTT connect is %d\n", rc);
            mStatus = "MQTTPublishService::run() rc from MQTT connect is failed!!!";

            return rc;
        }
        else
        {
            vilslog::printf("MQTTPublishService::run() MQTT connected\n");
            mStatus = "MQTTPublishService::run() MQTT connected";

            mInitial = true;

            //
            // split TPOIC GIOT-GW/KHH/UL/# to GIOT-GW/KHH/DL/
            //
            std::list<std::string> * topicStringlist = convert_string_to_list(mSystemConfig.LoRAWANTPOIC, '/');

            //unsigned int count = 0;
            mPublishTopic = "";
            for (std::list<std::string>::iterator topicString = topicStringlist->begin(); topicString != topicStringlist->end(); topicString++)
            {
                if( (*topicString).compare("UL") == 0 )
                {
                    mPublishTopic.append("DL/");
                }
                else
                if( (*topicString).compare("#") != 0 )
                {
                    mPublishTopic.append((*topicString));
                    mPublishTopic.append("/");
                }
                // if ( count == (topicStringlist->size() - 2))
                // {
                //     mPublishTopic.append("DL/");
                // }
                // else
                // if ( count >= (topicStringlist->size() - 1))
                // {
                //     break;
                // }
                // else
                // {
                //     mPublishTopic.append((*topicString));
                //     mPublishTopic.append("/");
                // }

                //count++;
            }

            vilslog::printf("MQTTPublishService::run() mPublishTopic[%s]\n", mPublishTopic.c_str());

            topicStringlist->clear();
            delete topicStringlist;

            // test publish
            // std::string payload = "[{\"macAddr\":\"0000000012345678\",\"data\":\"011eff017b81750736772e\",\"id\":\"ikkitest560\",\"extra\":{\"port\":2,\"txpara\":\"26\"}}]";
            //
            // rc = client->publish("GIOT-GW/DL/00001c497bcc6f73", (void*)payload.c_str(), payload.length(), MQTT::QOS0, false);
            // // rc = client->publish("SMIMS/VILS/TAG", (void*)payload.c_str(), payload.length(), MQTT::QOS0, false);
            // vilslog::printf("MQTTPublishService::run() client->publish rc[%d]\n", rc);
        }
    }

    return rc;
}

int MQTTPublishService::process_publish(MQTT::Client<IPStack, Countdown> * client, const char * gatewayid, const char * payload, int payloadLength)
{

#ifdef MQTT_PRINT_PUBLISH_MSG
    vilslog::printf("MQTTPublishService::process_publish()gatewayid[%s] payload[%s] payloadLength[%d]\n",
        gatewayid, payload, payloadLength);
#endif

    char topic[256];
    memset(topic, 0, 256);
    snprintf(topic, 256, "%s%s", mPublishTopic.c_str(), gatewayid);
    //int rc = client->publish(topic, (void*)payload, payloadLength, MQTT::QOS0, false);
    int rc = client->publish(topic, (void*)payload, payloadLength, MQTT::QOS1, false);
    if (rc != 0)
    {
        vilslog::printf("MQTTPublishService::process_publish() failed!!!! rc[%d] topic[%s] payload[%s]\n", rc, topic, payload);
    }
    else
    {
#ifdef MQTT_PRINT_PUBLISH_MSG
        vilslog::printf("MQTTPublishService::process_publish() gatewayid[%s] success!\n", gatewayid);
#endif
    }

    return rc;
}

int MQTTPublishService::process_publish_wentaitek(MQTT::Client<IPStack, Countdown> * client, const char * gatewayid, const char * payload, int payloadLength)
{

// #ifdef MQTT_PRINT_PUBLISH_MSG
    vilslog::printf("MQTTPublishService::process_publish_wentaitek()gatewayid[%s] payload[%s] payloadLength[%d]\n",
        gatewayid, payload, payloadLength);
// #endif

    char topic[256];
    memset(topic, 0, 256);
    vilslog::printf("MQTTPublishService::process_publish_wentaitek()->memset \n");
    snprintf(topic, 256, "%s", "control/light");
    // snprintf(topic, 256, "%s", "norgay/control/light");
    vilslog::printf("MQTTPublishService::process_publish_wentaitek()->snprintf \n");
    //int rc = client->publish(topic, (void*)payload, payloadLength, MQTT::QOS0, false);
    int rc = client->publish(topic, (void*)payload, payloadLength, MQTT::QOS1, false);
    vilslog::printf("MQTTPublishService::process_publish_wentaitek()->snprintf \n");

    if (rc != 0)
    {
        vilslog::printf("MQTTPublishService::process_publish_wentaitek() failed!!!! rc[%d] topic[%s] payload[%s]\n", rc, topic, payload);
    }
    else
    {
// #ifdef MQTT_PRINT_PUBLISH_MSG
        vilslog::printf("MQTTPublishService::process_publish_wentaitek() gatewayid[%s] success!\n", gatewayid);
// #endif
    }

    return rc;
}
