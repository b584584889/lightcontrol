/*
 *  CenterOutgoingService.cpp
 *
 *  Created on: 2017/02/21
 *      Author: ikki
 */

#include "MQTTSubscribeService.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <math.h>

#include <iostream>
#include <list>
#include "../json/json.h"

using namespace std;

wqueue<DataItem*> * glmqttMainJobQueue = NULL;
wqueue<MQTTDataItem*> * glqueue = NULL;
wqueue<MQTTDataItem*> * glqueuePuslish = NULL;
ITPS::Publisher<DataItem*> * glTagActivityPublisher = NULL;

DeviceManager * glMQTTDeviceManager = NULL;

bool glUpdateProject = false;
std::list<int> * glPrjidList = NULL;
std::string glSubTopic;
std::string glReportTopic;
std::string glSubReportTopic;

void messageArrived(MQTT::MessageData & md);
void messageArrived_wentaitek(MQTT::MessageData & md);
std::string processMessage(std::string msg_type, std::string topicname, Json::Value & item, std::string LoRAMacaddress, std::string gwid);
std::string processLoRAGatewayMessage(std::string cur_type, std::string topicname, Json::Value & item);
std::string processNBIoTMessage(std::string cur_type, std::string topicname, Json::Value & item, std::string LoRAMacaddress, std::string gwid, int rssi, std::string timestamp);

int do_parse_data(int projectid, const char * LoRAMacAddr, Json::Value & JSONLora, const char * gatewayid, int rssi, const char * timestamp);
int do_parse_data(int projectid, const char * LoRAMacAddr, const char * data, const char * gatewayid, int rssi, const char * timestamp);
int do_parse_compress_data(int projectid, const char * LoRAMacAddr, const char * data, const char * gatewayid, int rssi, const char * timestamp);
int do_parse_compress_multiple_data(int projectid, const char * LoRAMacAddr, const char * data, const char * gatewayid, int rssi, const char * timestamp);
int do_parse_compress_AOA_data(int projectid, const char * LoRAMacAddr, const char * data, const char * gatewayid, int rssi, const char * timestamp);
int do_parse_compress_AOA_IMU_data(int projectid, const char * LoRAMacAddr, const char * data, const char * gatewayid, int rssi, const char * timestamp);

int do_parse_wentaitek_s3_watch(Json::Value & item);
int do_parse_wentaitek_uw_sensor_1(Json::Value & item);
int do_parse_wentaitek_light(Json::Value & item);
int do_parse_wentaitek_power_meter(Json::Value & item);

int do_parse_wentaitek_s3_watch_device(Json::Value & item);
int do_parse_wentaitek_uw_sensor_1_device(Json::Value & item);
int do_parse_wentaitek_power_meter_device(Json::Value & item);

std::shared_ptr<wentaitek_s3_watch_deviceStruct> parse_wentaitek_s3_watch_device(Json::Value & item);
std::shared_ptr<wentaitek_uw_sensor_1_deviceStruct> parse_wentaitek_uw_sensor_1_device(Json::Value & item);
std::shared_ptr<wentaitek_power_meter_deviceStruct> parse_wentaitek_power_meter_device(Json::Value & item);


void send_loragateway_to_projects(LoRaGatewayInfoStruct * geteway);
void send_MQTTgateway_to_projects(std::string gatewayid);
void send_alert_to_projects(const char * LoRAMacAddr, const char * macaddress, const char * gatewayid, const char * timestamp);
void send_cancel_to_projects(const char * LoRAMacAddr, const char * macaddress, const char * gatewayid, const char * timestamp);
void send_ranging_to_projects(const char * LoRAMacAddr, const char * macaddress, int ranging, const char * voltage, const char * gatewayid, int rssi, const char * timestamp);
void send_gpsinfo_to_projects(const char * type, const char * LoRAMacAddr, const char * macaddress, double Longitude, double Latitude, const char * gatewayid, int rssi, const char * timestamp);
void send_lidarinfo_to_projects(const char * LoRAMacAddr, const char * value, const char * gatewayid, int rssi, const char * timestamp);
void send_LoRaDeviceALIVE_to_projects(const char * LoRAMacAddr, const char * gatewayid, int rssi, int AoAGridSize);
void send_LoRaDeviceBOOT_to_projects(const char * LoRAMacAddr, const char * macaddress, std::string PANID, std::string MODE, std::string VERSION, const char * gatewayid);
void send_LoRaDevicePOWEROFF_to_projects(const char * macaddress, const char * gatewayid);
void send_LoRaDevicePOWERON_to_projects(const char * macaddress, const char * gatewayid);
void send_NFCLocatorPOWERON_to_projects(const char * LoRAMacAddr, const char * macaddress, const char * gatewayid);
void send_Heart_to_projects(const char * LoRAMacAddr, std::string HeartData, const char * gatewayid);
void send_Weight_to_projects(const char * LoRAMacAddr, std::string WeightData, const char * gatewayid);
void send_Tag_Heart_to_projects(const char * LoRAMacAddr, const char * tagmacAddr, std::string HeartData);
void send_Tag_Weight_to_projects(const char * LoRAMacAddr, const char * tagmacAddr, std::string WeightData);
void send_rangingcompress_to_projects(const char * LoRAMacAddr, const char * macaddress, int distance, const char * voltage, int velocity, const char * gatewayid, int rssi, const char * timestamp);
void send_rangingcompressMultiDistance_to_projects(const char * LoRAMacAddr, const char * macaddress, int distInterval, std::list<int> * distList, const char * voltage, int velocity, const char * gatewayid, int rssi, const char * timestamp);
void send_rangingcompressaoa_to_projects(const char * LoRAMacAddr, const char * macaddress, int distance, const char * voltage, int velocity, float angleFloat, const char * gatewayid, int rssi, int * IMU, const char * timestamp);
void send_MQTT_resp_to_projects(std::string gatewayid, std::string dataId, std::string resp, int status);
void send_message_to_projects(std::string gatewayid, const char * LoRAMacAddr, std::string jsondata);
void send_OPENDOOR_to_projects(const char * LoRAMacAddr, const char * macaddress, const char * gatewayid, const char * timestamp);
void send_CLOSEDOOR_to_projects(const char * LoRAMacAddr, const char * gatewayid, const char * timestamp);
void send_GPIO_ON_to_projects(const char * LoRAMacAddr, const char * gatewayid, const char * timestamp);
bool send_MQTT_cmd(int projectid, std::string cmd, NFCDeviceStruct * device);
void send_aoa_class_to_projects(const char * LoRAMacAddr, const char * macaddress, const char * lv, const char * time, const char * gatewayid, int rssi, const char * timestamp);
void send_PowerSwitch_to_projects(const char * LoRAMacAddr, const char * value, const char * gatewayid, int rssi, const char * timestamp);
void send_beacon_to_projects(const char * LoRAMacAddr, const char * macaddress, const char * value, const char * gatewayid, int rssi, const char * timestamp);
void send_iCover_to_projects(const char * LoRAMacAddr, const char * macaddress, const char * value, const char * gatewayid, int rssi, const char * timestamp);

MQTTSubscribeService::MQTTSubscribeService(wqueue<DataItem*> & MainJobQueue, wqueue<MQTTDataItem*> & queueSub, wqueue<MQTTDataItem*> & queuePub, DeviceManager * pDevManager, DBManager * dbManager)
    : mMainJobQueue(MainJobQueue), m_queueSub(queueSub), m_queuePub(queuePub), mDeviceManager(pDevManager), mDbManager(dbManager)
{
    glmqttMainJobQueue = &mMainJobQueue;
    glqueue = &m_queueSub;
    glqueuePuslish = &m_queuePub;
    glMQTTDeviceManager = mDeviceManager;
    glTagActivityPublisher = new ITPS::Publisher<DataItem*>("Tag", "Activity");
}

bool MQTTSubscribeService::status()
{
    return mInitial;
}

std::string MQTTSubscribeService::statusString()
{
    return mStatus;
}

void MQTTSubscribeService::updateProject()
{
    glUpdateProject = true;
}

void * MQTTSubscribeService::run()
{
    while(1)
    {
         try
         {
            int runloop_cnt = 0;
            while(1)
            {

                // do_parse_compress_multiple_data("0000000000050013", "5b0610000000009900990199029903990499059906990799089909ff2b442a506a81810003fe000101fc038050690081010103fe00fc030180507100ff0001fd03ff01fffc80506f0001ff00fd00000201fd8050690001010000810002fc0280506c00ff0103ff00ff01ffff80506d0001feff000401fefc0480507300fa03fd04fe03fe810280506f0000fe01fc03ff00810280507100ff00000100fc03fd03800102030405060708090A123456789A5d",
                //     "00001c497bcc6f73", -99, "1234567890");

                // do_parse_compress_multiple_data("0000000000050013", "5b0610000000000128ff041000000000002900220007ff2b442a50c1000000000081ff01fe018050c100c60100ff0100000000805051001002fe0001ff0002ff80507100f300ff028100ff01ff80000000000ede5d",
                //     "00001c497bcc6f73", -99, "1234567890");

                //sleep(10);
                // // do_parse_compress_AOA_data(1, "0000000002070001", "3c0410000000000320ff2b442a080003d02b412a803e0b", "00001c497bcc6f73", -99, "1234567890");
                // //do_parse_compress_AOA_data(1, "0000000002070001", "3c4210000000000003ff2b442a080000d02b412a8b3ecd", "00001c497bcc6f73", -99, "2019-11-22T15:47:14");
                //
                // do_parse_compress_AOA_data(1, "0000000002070001", "3c4210000000000314ff2b442a0ac000b02b412a853e01", "00001c497bcc6f73", -99, "2019-11-22T15:47:14");
                // sleep(3);
                // do_parse_compress_AOA_data(1, "0000000002070001", "3c4210000000000314ff2b442a0ac000b02b412a883e02", "00001c497bcc6f73", -99, "2019-11-22T15:47:14");
                // sleep(3);
                // do_parse_compress_AOA_data(1, "0000000002070001", "3c4210000000000314ff2b442a0ac000b02b412a863e03", "00001c497bcc6f73", -99, "2019-11-22T15:47:14");
                // sleep(3);
                // do_parse_compress_AOA_data(1, "0000000002070001", "3c4210000000000314ff2b442a0ac000b02b412a883e04", "00001c497bcc6f73", -99, "2019-11-22T15:47:14");
                // sleep(3);

                // do_parse_compress_AOA_data(1, "0000000002070013", "3c0410000000000013ff0310000000000135ff2b442a0d606a0000fc2b412a0070006b3e7e", "00001c497bcc6f73", -99, "2019-11-22T15:47:14");
                // sleep(3);

                // do_parse_compress_AOA_data(1, "0000000002070013", "3c4210000000000138ff2b442a001000002b412a00b83e", "00001c497bcc6f73", -99, "2019-11-22T15:47:14");
                // sleep(3);

                runloop();

                runloop_cnt++;
                vilslog::printf("MQTTSubscribeService::run() runloop_cnt[%d]\n", runloop_cnt);
                sleep(10);
            }
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("MQTTSubscribeService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "MQTTSubscribeService()::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

int MQTTSubscribeService::runloop()
{
    int rc = 0;
    mInitial = false;

    glPrjidList = mDeviceManager->getAllProjectConfig();

    IPStack ipstack = IPStack();

    MQTT::Client<IPStack, Countdown> mClient = MQTT::Client<IPStack, Countdown>(ipstack, 30000);

    rc = connect_to_MQTT(&mClient, &ipstack);

    while (rc == 0)
    {
        int ret = mClient.yield(100);
        //printf("client.yield(5000) end ret[%d]\n", ret);

        if (glUpdateProject)
        {
            glPrjidList->clear();
            delete glPrjidList;
            glPrjidList = mDbManager->getAllProjectConfig();

            glUpdateProject = false;
        }

        if (ret < 0)
        {
            vilslog::printf("MQTTSubscribeService::run() ret[%d] < 0 break while loop \n", ret);
            break;
        }
    }
    rc = mClient.unsubscribe(mSystemConfig.LoRAWANTPOIC.c_str());
    if (rc != 0)
        vilslog::printf("MQTTSubscribeService::run() rc from unsubscribe was %d\n", rc);

    //rc = client.unsubscribe("GIOT-GW/DL-report/#");
    std::string subReportTopic = glReportTopic;
    subReportTopic.append("#");
    rc = mClient.unsubscribe(subReportTopic.c_str());

    rc = mClient.disconnect();
    if (rc != 0)
        vilslog::printf("MQTTSubscribeService::run() rc from disconnect was %d\n", rc);

    ipstack.disconnect();

    return rc;
}

int MQTTSubscribeService::connect_to_MQTT(MQTT::Client<IPStack, Countdown> * client, IPStack * ipstack)
{
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(1);
    if (baseDb != NULL)
        baseDb->read_sys_config(&mSystemConfig);

    vilslog::printf("MQTTSubscribeService::connect_to_MQTT() Connecting to %s:%d\n", mSystemConfig.MQTTHostIp.c_str(), mSystemConfig.MQTTHostPort);
    int rc = ipstack->connect(mSystemConfig.MQTTHostIp.c_str(), mSystemConfig.MQTTHostPort);
    if (rc != 0)
    {
        vilslog::printf("MQTTSubscribeService::connect_to_MQTT() rc from ipstack.connect is %d\n", rc);
        mStatus = "MQTTSubscribeService::connect_to_MQTT() 1 rc from ipstack.connect is failed!!! ";

        rc = client->disconnect();
        if (rc != 0)
            vilslog::printf("MQTTSubscribeService::connect_to_MQTT() rc from disconnect was %d\n", rc);

        ipstack->disconnect();

        return rc;
    }
    else
    {
        vilslog::printf("MQTTSubscribeService::connect_to_MQTT() MQTT connecting...\n");
        mStatus = "MQTTSubscribeService::connect_to_MQTT() MQTT connecting...";

        std::string clientID = mSystemConfig.MQTTClientId.c_str();
        clientID.append("Subscribe");

        MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
        data.username.cstring = (char*)mSystemConfig.MQTTHostUsername.c_str();
        data.password.cstring = (char*)mSystemConfig.MQTTHostPassword.c_str();
        data.MQTTVersion = 3;
        //data.clientID.cstring = (char*)systemConfig.MQTTClientId.c_str();
        data.clientID.cstring = (char*)clientID.c_str();
        rc = client->connect(data);
        if (rc != 0)
        {
            vilslog::printf("MQTTSubscribeService::connect_to_MQTT() rc from MQTT connect is %d\n", rc);
            mStatus = "MQTTSubscribeService::connect_to_MQTT() 2 rc from MQTT connect is failed!!!";

            rc = client->disconnect();
            if (rc != 0)
                vilslog::printf("MQTTSubscribeService::connect_to_MQTT() 2 rc from disconnect was %d\n", rc);

            return rc;
        }
        else
        {
            vilslog::printf("MQTTSubscribeService::connect_to_MQTT() MQTT connected\n");
            mStatus = "MQTTSubscribeService::connect_to_MQTT() MQTT connected";
            //rc = client->subscribe(mSystemConfig.LoRAWANTPOIC.c_str(), MQTT::QOS0, messageArrived);
            rc = client->subscribe(mSystemConfig.LoRAWANTPOIC.c_str(), MQTT::QOS1, messageArrived);
            if (rc != 0)
            {
                vilslog::printf("MQTTSubscribeService::connect_to_MQTT() 3 rc from MQTT subscribe is %d\n", rc);
                mStatus = "MQTTSubscribeService::connect_to_MQTT() 3 rc from MQTT subscribe is failed!!!";

                rc = client->disconnect();
                if (rc != 0)
                    vilslog::printf("MQTTSubscribeService::connect_to_MQTT() 3 rc from disconnect was %d\n", rc);

                return rc;
            }
            else
            {
                mInitial = true;
            }

            rc = client->subscribe("get_data/#", MQTT::QOS1, messageArrived_wentaitek);
            if (rc != 0)
            {
                vilslog::printf("MQTTSubscribeService::connect_to_MQTT() wentaitek MQTT subscribe get_data failed! rc[%d]\n", rc);
            }

            rc = client->subscribe("norgay/get_data/#", MQTT::QOS1, messageArrived_wentaitek);
            if (rc != 0)
            {
                vilslog::printf("MQTTSubscribeService::connect_to_MQTT() wentaitek MQTT subscribe norgay/get_data failed! rc[%d]\n", rc);
            }

            //
            // split TPOIC GIOT-GW/DL-report/
            //
            std::list<std::string> * topicStringlist = convert_string_to_list(mSystemConfig.LoRAWANTPOIC, '/');
            glSubTopic = mSystemConfig.LoRAWANTPOIC;

            // unsigned int count = 0;
            glReportTopic = "";
            for (std::list<std::string>::iterator topicString = topicStringlist->begin(); topicString != topicStringlist->end(); topicString++)
            {
                if( (*topicString).compare("UL") == 0 )
                {
                    glReportTopic.append("DL-report/");
                }
                else
                if( (*topicString).compare("#") != 0 )
                {
                    glReportTopic.append((*topicString));
                    glReportTopic.append("/");
                }
                // if ( count == (topicStringlist->size() - 2))
                // {
                //     glReportTopic.append("DL-report/");
                // }
                // else
                // if ( count >= (topicStringlist->size() - 1))
                // {
                //     break;
                // }
                // else
                // {
                //     glReportTopic.append((*topicString));
                //     glReportTopic.append("/");
                // }
                // count++;
            }

            vilslog::printf("MQTTSubscribeService::connect_to_MQTT() glReportTopic[%s] topicStringlist->size()[%d]\n", glReportTopic.c_str(), (int)topicStringlist->size());

            topicStringlist->clear();
            delete topicStringlist;

            glSubReportTopic = glReportTopic;
            glSubReportTopic.append("#");

            //rc = client->subscribe(subReportTopic.c_str(), MQTT::QOS0, messageArrived);
            rc = client->subscribe(glSubReportTopic.c_str(), MQTT::QOS1, messageArrived);
            vilslog::printf("MQTTSubscribeService::connect_to_MQTT() client.subscribe(%s) rc[%d]\n", glSubReportTopic.c_str(), rc);
        }
    }

    return rc;
}


void messageArrived(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;

    // vilslog::printf("Message %d arrived: qos %d, retained %d, dup %d, packetid %d\n",
    //     ++arrivedcount, message.qos, message.retained, message.dup, message.id);
    //printf("MQTT messageArrived() Payload %.*s\n", (int)message.payloadlen, (char*)message.payload);

    char topicname[256];
    memset(topicname, 0, 256);
    strncpy(topicname, md.topicName.lenstring.data, md.topicName.lenstring.len);

    char payload[65536];
    memset(payload, 0, 65536);
    if ( (int)message.payloadlen < 65536 )
    {
        memcpy(payload, (char*)message.payload, (int)message.payloadlen);
    }
    else
    {
        vilslog::printf ("MQTTSubscribeService::messageArrived() payloadlen[%d]\n", (int)message.payloadlen);
        return;
    }
    // printf("MQTT messageArrived() Payload[%d]\n<%s>\n", (int)message.payloadlen, (char*)message.payload);
    // printf("MQTT messageArrived() Payload[%d]\n<%s>\n", (int)message.payloadlen, payload);

    // Payload
    // [{"channel":923200000, "sf":7, "time":"2018-07-24T06:44:44",
    // "gwip":"192.168.99.143", "gwid":"00001c497bcaae89", "repeater":"00000000ffffffff",
    // "systype":18, "rssi":-46.0, "snr":23.8, "snr_max":32.5, "snr_min":16.3,
    // "macAddr":"0000000012345678", "data":"3132333435363738", "frameCnt":0, "fport":1}]

// [[{"channel":925400000, "sf":10, "time":"2018-08-17T15:47:14",
// "gwip":"0.0.0.0",
// "gwid":"00001c497bcaae88",
// "repeater":"00000000ffffffff",
// "systype":0,
// "rssi":-92.0,
// "snr":18.8,
// "snr_max":35.8,
// "snr_min":9.5,
// "macAddr":"00000000000000fc",
// "data":"7b225470223a2245222c2246223a2233313330303030303030303030303231227d0d0a",
// "frameCnt":0,
// "fport":1}]]

// [{"type":"HAR","macaddress":"4210000000000313","activity":"LAYING"}]

    Json::Value JSONresult;
    try{

        Json::Reader * reader = new Json::Reader();
        bool parsingSuccessful = reader->parse( payload, JSONresult);
        delete reader;

#ifdef PRINT_MQTT_SUB_MSG
        vilslog::printf ("*********\nMQTTSubscribeService::messageArrived() topicName[%s] parsingSuccessful[%d] payload=%s\n*********\n",
            topicname, parsingSuccessful, payload);
#endif

        // vilslog::printf ("MQTTSubscribeService::messageArrived() parsingSuccessful[%d] isArray[%d]\n", parsingSuccessful, JSONresult.isArray());

        // [{"cmd":"ReportScanRFIDCard","time":"2020-05-21 15:01:44"}]

        /*
        MQTTSubscribeService::messageArrived() payload=[{"channel":923600000,
        "sf":10,
        "time":"2019-04-21T09:01:44",
        "time":"2019-04-21T09:01:44+08:00",
        "gwip":"10.1.1.14",
        "gwid":"00001c497bf88362",
        "repeater":"00000000ffffffff",
        "systype":0,
        "rssi":-66.0, "snr":23.3, "snr_max":34.5, "snr_min":15.8,
        "macAddr":"0000000000050177", "
        data":"7b225470223a2244222c2246223a2234323130303030303030303030333135222c2244223a223831227d0d0a",
        "frameCnt":0,
        "fport":1}]
        */

        /*
            AOA
            payload=[
                {"channel":923000000,
                "sf":10,
                "time":"2019-11-22T12:09:49+08:00",
                "gwip":"192.168.99.101",
                "gwid":"00001c497bcc6f73",
                "repeater":"00000000ffffffff",
                "systype":0,
                "rssi":-89.0,
                "snr":24.5,
                "snr_max":30.0,
                "snr_min":17.0,
                "macAddr":"0000000000050179",
                "data":"3c0410000000000320ff2b442a080003d02b412a803e0b",
                "frameCnt":0,
                "fport":1}
            ]
        */

        if (parsingSuccessful && JSONresult.isArray())
        {
            // vilslog::printf ("MQTTSubscribeService::messageArrived() JSONresult.size()[%d]\n", (int)JSONresult.size());

            std::string msg_type = "";
            std::string gwid = "";
            std::string macAddr = "";
            for(unsigned int i=0;i<JSONresult.size();i++)
            {
                Json::Value tmp;
                Json::Value item = JSONresult.get((Json::UInt)i, tmp);

                if ( item.isMember("gwid") )
                {
                    gwid = item["gwid"].asString();
                }
                if ( item.isMember("macAddr") )
                {
                    macAddr = item["macAddr"].asString();
                }

                std::string type = processMessage(msg_type, topicname, item, macAddr, gwid);
                if ( type.length() > 0 )
                {
                    msg_type = type;
                }
            }
        }
        else
        {
            //payload={"dataId":"ikkitest11", "resp":"2019-05-29T15:31:54Z", "status":3}


#ifdef PRINT_MQTT_SUB_MSG
            vilslog::printf ("MQTTSubscribeService::messageArrived() Not array!!!!! topicName[%s]\n", topicname);
#endif
            // topicName[GIOT-GW/DL-report/00001c497bcc6f73]

            //if(strstr(topicname, "GIOT-GW/DL-report/"))
            if(strstr(topicname, glReportTopic.c_str()))
            {
                // special case
                // GIOT-GW/DL-report/00001c497bf8836200001c{"downlink_data":[{"macAddr":"0000000012345678","data":"011eff017b81750736772e","id":"vilscenter14:53:45.8321","extra":{"port":2,"txpara":"22"}}]}

                // normal case
                // GIOT-GW/DL-report/00001c497bcc6fbd

                char gatewayid[256];
                memset(gatewayid, 0, 256);
                int gateway_id_length = 16; // 00001c497bcc6f73
                //int prefixLen = strlen("GIOT-GW/DL-report/");
                int prefixLen = strlen(glReportTopic.c_str());
                strncpy(gatewayid, topicname + prefixLen, gateway_id_length);
                //strncpy(gatewayid, topicname + prefixLen, md.topicName.lenstring.len - prefixLen);
#ifdef PRINT_MQTT_SUB_MSG
                vilslog::printf ("MQTTSubscribeService::messageArrived() gatewayid[%s] topicName.lenstring.len[%d]\n", gatewayid, md.topicName.lenstring.len);
#endif
                //LoRaGatewayInfoStruct geteway;
                //geteway.projectid = glMQTTDeviceManager->getLoRaGatewayProjectId(gatewayid);
                //geteway.gwid = gatewayid;

                // send MQTTgateway alive
                std::string gatewayidStr = gatewayid;
                send_MQTTgateway_to_projects(gatewayidStr);

                // send MQTTgateway alive
                //MQTTDataItem * mqttitem = new MQTTDataItem("MQTTgateway", geteway.projectid, &geteway);
                //glqueue->add(mqttitem);
                std::string dataId;
                std::string resp;
                int status = -3;
                if ( JSONresult.isMember("dataId") )
                    dataId = JSONresult["dataId"].asString();
                if ( JSONresult.isMember("resp") )
                    resp = JSONresult["resp"].asString();
                if ( JSONresult.isMember("status") )
                    status = JSONresult["status"].asInt();

#ifdef PRINT_MQTT_SUB_MSG
                vilslog::printf ("MQTTSubscribeService::messageArrived() gatewayid[%s] dataId[%s] resp[%s] status[%d]\n",
                    gatewayid, dataId.c_str(), resp.c_str(), status);
#endif
                send_MQTT_resp_to_projects(gatewayidStr, dataId, resp, status);

            }
            else
            {
                // not array data
                vilslog::printf ("MQTTSubscribeService::messageArrived() parsingSuccessful[%d] topicname[%s] payload[%s]\n",
                    parsingSuccessful, topicname, payload);

                // GIOT-GW/NORGAY/UL/868333030959270
                char IMEI[256];
                memset(IMEI, 0, 256);
                int IMEI_length = 15; // 8683-3303-0959-270
                int prefixLen = strlen(glSubTopic.c_str());
                strncpy(IMEI, topicname + prefixLen, IMEI_length);

                vilslog::printf ("MQTTSubscribeService::messageArrived() IMEI[%s] IMEI_length[%d] glSubTopic[%s] prefixLen[%d]\n",
                    IMEI, IMEI_length, glSubTopic.c_str(), prefixLen);

                processNBIoTMessage("iCover", topicname, JSONresult, IMEI, IMEI, 0, currentDateTimeForSQL());
            }
        }
    }
    catch(...)
    {
        vilslog::printf ("MQTTSubscribeService::messageArrived() catch exception...\n");
        vilslog::printf ("MQTTSubscribeService::messageArrived() topicname[%s]\n", topicname);
        vilslog::printf ("MQTTSubscribeService::messageArrived() payloadlen[%d] payload[%s]\n", (int)message.payloadlen, payload);
    }
    //printf ("MQTTSubscribeService::messageArrived() Finished!\n\n\n");
}

void messageArrived_wentaitek(MQTT::MessageData& md)
{
    MQTT::Message &message = md.message;

    // vilslog::printf("Message %d arrived: qos %d, retained %d, dup %d, packetid %d\n",
    //     ++arrivedcount, message.qos, message.retained, message.dup, message.id);
    //printf("MQTT messageArrived() Payload %.*s\n", (int)message.payloadlen, (char*)message.payload);

    char topicname[256];
    memset(topicname, 0, 256);
    strncpy(topicname, md.topicName.lenstring.data, md.topicName.lenstring.len);

    char payload[65536];
    memset(payload, 0, 65536);
    if ( (int)message.payloadlen < 65536 )
    {
        memcpy(payload, (char*)message.payload, (int)message.payloadlen);
    }
    else
    {
        vilslog::printf ("MQTTSubscribeService::messageArrived_wentaitek() payloadlen[%d]\n", (int)message.payloadlen);
        return;
    }

    // vilslog::printf ("MQTTSubscribeService::messageArrived_wentaitek() topicname[%s] payloadlen[%d]\n", topicname, (int)message.payloadlen);

    Json::Value JSONresult;
    try{

        Json::Reader * reader = new Json::Reader();
        bool parsingSuccessful = reader->parse( payload, JSONresult);
        std::string strTopic(topicname);
        delete reader;

        if (parsingSuccessful)
        {
            std::string::size_type s3 = strTopic.find("get_data/s3_watch", 0);
            if (s3 != std::string::npos)
            {
                do_parse_wentaitek_s3_watch(JSONresult);
            }
            std::string::size_type uw = strTopic.find("get_data/uw_sensor_1", 0);
            if (uw != std::string::npos)
            {
                do_parse_wentaitek_uw_sensor_1(JSONresult);
            }
            std::string::size_type light = strTopic.find("get_data/light", 0);
            if (light != std::string::npos)
            {
                do_parse_wentaitek_light(JSONresult);
            }
            std::string::size_type pm = strTopic.find("get_data/power_meter", 0);
            if (pm != std::string::npos)
            {
                do_parse_wentaitek_power_meter(JSONresult);
            }

            if (strcmp(topicname, "get_data/s3_watch") == 0 )
            {
                do_parse_wentaitek_s3_watch(JSONresult);
            }
            else
            if (strcmp(topicname, "get_data/uw_sensor_1") == 0 )
            {
                do_parse_wentaitek_uw_sensor_1(JSONresult);
            }
            else
            if (strcmp(topicname, "get_data/light") == 0 )
            {
                do_parse_wentaitek_light(JSONresult);
            }
            else
            if (strcmp(topicname, "get_data/power_meter") == 0 )
            {
                do_parse_wentaitek_power_meter(JSONresult);
            }
        }
    }
    catch(...)
    {
        vilslog::printf ("MQTTSubscribeService::messageArrived_wentaitek() catch exception...\n");
        vilslog::printf ("MQTTSubscribeService::messageArrived_wentaitek() topicname[%s]\n", topicname);
        vilslog::printf ("MQTTSubscribeService::messageArrived_wentaitek() payloadlen[%d] payload[%s]\n", (int)message.payloadlen, payload);
    }
}

int do_parse_wentaitek_s3_watch(Json::Value & item)
{
    //{"watchS3": [
    //    {"device": [
    //        {"Blood_Oxygen": 0,
    //         "DBP": 0,
    //         "Decive ID": "ED0B",
    //         "Heart_Rate": 0,
    //         "Rssi": -83,
    //         "SBP": 0,
    //         "SOS": 0,
    //         "Temperature": 35.19},
    //        {"Blood_Oxygen": 0,
    //         "DBP": 82,
    //         "Decive ID": "2EBD",
    //         "Heart_Rate": 79,
    //         "Rssi": -91,
    //         "SBP": 118,
    //         "SOS": 0,
    //         "Temperature": 36.18}
    //    ],
    //    "repeater_ip": "10.10.10.6",
    //    "repeater_mac": "00:08:dc:36:06:0b"}
    //  ]}

    if ( item.isMember("watchS3") )
    {
        Json::Value watchS3 = item["watchS3"];

        if(watchS3.isArray())
        {
            vilslog::printf ("MQTTSubscribeService::do_parse_wentaitek_s3_watch() watchS3.size()[%d]\n", (int)watchS3.size());

            for(unsigned int i=0;i<watchS3.size();i++)
            {
                Json::Value tmp;
                Json::Value item_watchS3 = watchS3.get((Json::UInt)i, tmp);

                do_parse_wentaitek_s3_watch_device(item_watchS3);
            }
        }
    }
    return 0;
}

int do_parse_wentaitek_uw_sensor_1(Json::Value & item)
{
    // {   "uw_sensor_1": [{
    //         "device": [{
    //             "Device ID": "8291",
    //             "Rssi": "-90"
    //         }, {
    //             "Device ID": "2C9D",
    //             "Rssi": "-78"
    //         }, {
    //             "Device ID": "66EE",
    //             "Rssi": "-88"
    //         }],
    //         "repeater_ip": "10.10.10.6",
    //         "repeater_mac": "00:08:dc:36:06:0b"
    //     }]
    // }

    if ( item.isMember("uw_sensor_1") )
    {
        Json::Value uw_sensor_1 = item["uw_sensor_1"];

        if(uw_sensor_1.isArray())
        {
            // vilslog::printf ("MQTTSubscribeService::do_parse_wentaitek_s3_watch() watchS3.size()[%d]\n", (int)watchS3.size());

            for(unsigned int i=0;i<uw_sensor_1.size();i++)
            {
                Json::Value tmp;
                Json::Value item_uw_sensor_1 = uw_sensor_1.get((Json::UInt)i, tmp);

                do_parse_wentaitek_uw_sensor_1_device(item_uw_sensor_1);
            }
        }
    }

    return 0;
}

int do_parse_wentaitek_light(Json::Value & item)
{
    // {
    //     "devices": [{
    //         "id": 1,
    //         "name": "DT8_1",
    //         "uniAddress": 4,
    //         "state": {
    //             "onOff": 1,
    //             "level": 0,
    //             "cct": 0
    //         }
    //     }, {
    //         "id": 2,
    //         "name": "DT8_2",
    //         "uniAddress": 8,
    //         "state": {
    //             "onOff": 1,
    //             "level": 0,
    //             "cct": 0
    //         }
    //     }, {
    //         "id": 3,
    //         "name": "DT8_3",
    //         "uniAddress": 12,
    //         "state": {
    //             "onOff": 1,
    //             "level": 0,
    //             "cct": 0
    //         }
    //     }]
    // }

    if ( item.isMember("devices") )
    {
        Json::Value devices = item["devices"];

        if(devices.isArray())
        {
            // vilslog::printf ("MQTTSubscribeService::do_parse_wentaitek_s3_watch() watchS3.size()[%d]\n", (int)watchS3.size());

            for(unsigned int i=0;i<devices.size();i++)
            {
                Json::Value tmp;
                Json::Value item_device = devices.get((Json::UInt)i, tmp);

                std::shared_ptr<wentaitek_lightStruct> light_device = std::make_shared<wentaitek_lightStruct>();

                if ( item_device.isMember("id") )
                {
                    light_device->id = item_device["id"].asInt();
                }
                if ( item_device.isMember("name") )
                {
                    light_device->name = item_device["name"].asString();
                }
                if ( item_device.isMember("uniAddress") )
                {
                    light_device->uniAddress = item_device["uniAddress"].asInt();
                }
                if ( item_device.isMember("state") )
                {
                    Json::Value light_state = item_device["state"];
                    if ( light_state.isMember("onOff") )
                    {
                        light_device->state_onOff = light_state["onOff"].asInt();
                    }
                    if ( light_state.isMember("level") )
                    {
                        light_device->state_level = light_state["level"].asInt();
                    }
                    if ( light_state.isMember("cct") )
                    {
                        light_device->state_cct = light_state["cct"].asInt();
                    }
                }

                MQTTDataItem * mqttitem = new MQTTDataItem("wentaitek_light", 1, light_device);
                glqueue->add(mqttitem);
            }
        }
    }
    return 0;
}

int do_parse_wentaitek_power_meter(Json::Value & item)
{
    // {
    //     "power_meter": [{
    //         "device": [{
    //             "Active_Energy": 450.2,
    //             "Active_Power": 1.3,
    //             "BT_version": "v4.7",
    //             "Current_RMS": 0.01,
    //             "Device_ID": "8291",
    //             "Frequency": 59.9,
    //             "Rssi": -89,
    //             "Voltage_RMS": 109.9,
    //             "relay_state": true
    //         }, {
    //             "Active_Energy": 570.5,
    //             "Active_Power": 0.0,
    //             "BT_version": "v4.6",
    //             "Current_RMS": 0.0,
    //             "Device_ID": "2C9D",
    //             "Frequency": 59.9,
    //             "Rssi": -78,
    //             "Voltage_RMS": 108.6,
    //             "relay_state": true
    //         }, {
    //             "Active_Energy": 570.5,
    //             "Active_Power": 1.3,
    //             "BT_version": "v4.7",
    //             "Current_RMS": 0.02,
    //             "Device_ID": "66EE",
    //             "Frequency": 59.9,
    //             "Rssi": -88,
    //             "Voltage_RMS": 110.7,
    //             "relay_state": true
    //         }, {
    //             "Active_Energy": 0.0,
    //             "Active_Power": 0.0,
    //             "BT_version": "v0.0",
    //             "Current_RMS": 0.0,
    //             "Device_ID": "2EBD",
    //             "Frequency": 0.0,
    //             "Rssi": 0,
    //             "Voltage_RMS": 0.0,
    //             "relay_state": false
    //         }],
    //         "repeater_ip": "10.10.10.6",
    //         "repeater_mac": "00:08:dc:36:06:0b"
    //     }]
    // }

    if ( item.isMember("power_meter") )
    {
        Json::Value power_meter = item["power_meter"];

        if(power_meter.isArray())
        {
            // vilslog::printf ("MQTTSubscribeService::do_parse_wentaitek_power_meter() power_meter.size()[%d]\n", (int)power_meter.size());

            for(unsigned int i=0;i<power_meter.size();i++)
            {
                Json::Value tmp;
                Json::Value item_power_meter = power_meter.get((Json::UInt)i, tmp);

                do_parse_wentaitek_power_meter_device(item_power_meter);
            }
        }
        else {
            vilslog::printf("MQTTSubscribeService::do_parse_wentaitek_power_meter power_meter is not array!");
        }

    }

    return 0;
}

int do_parse_wentaitek_s3_watch_device(Json::Value & item)
{
    std::string repeater_ip;
    std::string repeater_mac;

    if ( item.isMember("repeater_ip") )
    {
        repeater_ip = item["repeater_ip"].asString();
    }

    if ( item.isMember("repeater_mac") )
    {
        repeater_mac = item["repeater_mac"].asString();
    }

    if ( item.isMember("device") )
    {
        Json::Value watchS3_device = item["device"];
        if(watchS3_device.isArray())
        {
            // vilslog::printf ("MQTTSubscribeService::do_parse_wentaitek_s3_watch_device() watchS3_device.size()[%d]\n", (int)watchS3_device.size());

            for(unsigned int i=0;i<watchS3_device.size();i++)
            {
                Json::Value tmp;
                Json::Value item_device = watchS3_device.get((Json::UInt)i, tmp);

                std::shared_ptr<wentaitek_s3_watch_deviceStruct> s3_watch_device = parse_wentaitek_s3_watch_device(item_device);
                s3_watch_device->repeater_ip = repeater_ip;
                s3_watch_device->repeater_mac = repeater_mac;

                MQTTDataItem * mqttitem = new MQTTDataItem("wentaitek_s3_watch_device", 1, s3_watch_device);
                glqueue->add(mqttitem);

                // vilslog::printf ("MQTTSubscribeService::do_parse_wentaitek_s3_watch_device() Device_ID[%s] Blood_Oxygen[%d] Heart_Rate[%d] DBP[%d] Rssi[%d] SBP[%d] SOS[%d] Temperature[%f]\n",
                //     s3_watch_device->Device_ID.c_str(), s3_watch_device->Blood_Oxygen, s3_watch_device->Heart_Rate, s3_watch_device->DBP, s3_watch_device->Rssi, s3_watch_device->SBP, s3_watch_device->SOS, s3_watch_device->Temperature);

            }
        }
    }

    // vilslog::printf ("MQTTSubscribeService::do_parse_wentaitek_s3_watch_device() repeater_ip[%d] repeater_mac[%d]\n",
    //     repeater_ip.c_str(), repeater_mac.c_str());

    return 0;
}

int do_parse_wentaitek_uw_sensor_1_device(Json::Value & item)
{
    std::string repeater_ip;
    std::string repeater_mac;

    if ( item.isMember("repeater_ip") )
    {
        repeater_ip = item["repeater_ip"].asString();
    }

    if ( item.isMember("repeater_mac") )
    {
        repeater_mac = item["repeater_mac"].asString();
    }

    if ( item.isMember("device") )
    {
        Json::Value watchS3_device = item["device"];
        if(watchS3_device.isArray())
        {
            // vilslog::printf ("MQTTSubscribeService::do_parse_wentaitek_s3_watch_device() watchS3_device.size()[%d]\n", (int)watchS3_device.size());

            for(unsigned int i=0;i<watchS3_device.size();i++)
            {
                Json::Value tmp;
                Json::Value item_device = watchS3_device.get((Json::UInt)i, tmp);

                std::shared_ptr<wentaitek_uw_sensor_1_deviceStruct> wentaitek_uw_sensor_1_device = parse_wentaitek_uw_sensor_1_device(item_device);
                wentaitek_uw_sensor_1_device->repeater_ip = repeater_ip;
                wentaitek_uw_sensor_1_device->repeater_mac = repeater_mac;

                MQTTDataItem * mqttitem = new MQTTDataItem("wentaitek_uw_sensor_1_device", 1, wentaitek_uw_sensor_1_device);
                glqueue->add(mqttitem);

                // vilslog::printf ("MQTTSubscribeService::do_parse_wentaitek_s3_watch_device() Device_ID[%s] Blood_Oxygen[%d] Heart_Rate[%d] DBP[%d] Rssi[%d] SBP[%d] SOS[%d] Temperature[%f]\n",
                //     s3_watch_device->Device_ID.c_str(), s3_watch_device->Blood_Oxygen, s3_watch_device->Heart_Rate, s3_watch_device->DBP, s3_watch_device->Rssi, s3_watch_device->SBP, s3_watch_device->SOS, s3_watch_device->Temperature);

            }
        }
    }

    return 0;
}

int do_parse_wentaitek_power_meter_device(Json::Value & item)
{
    std::string repeater_ip;
    std::string repeater_mac;

        vilslog::printf("do_parse_wentaitek_power_meter_device() IN\n");

    if ( item.isMember("repeater_ip") )
    {
        repeater_ip = item["repeater_ip"].asString();
    }

    if ( item.isMember("repeater_mac") )
    {
        repeater_mac = item["repeater_mac"].asString();
    }

    if ( item.isMember("device") )
    {
        Json::Value watchS3_device = item["device"];
        if(watchS3_device.isArray())
        {
            for(unsigned int i=0;i<watchS3_device.size();i++)
            {
                Json::Value tmp;
                Json::Value item_device = watchS3_device.get((Json::UInt)i, tmp);

                std::shared_ptr<wentaitek_power_meter_deviceStruct> wentaitek_power_meter_device = parse_wentaitek_power_meter_device(item_device);
                wentaitek_power_meter_device->repeater_ip = repeater_ip;
                wentaitek_power_meter_device->repeater_mac = repeater_mac;

                MQTTDataItem * mqttitem = new MQTTDataItem("wentaitek_power_meter_device", 1, wentaitek_power_meter_device);
                glqueue->add(mqttitem);
            }
        }
    }

    return 0;
}

std::shared_ptr<wentaitek_s3_watch_deviceStruct> parse_wentaitek_s3_watch_device(Json::Value & item_device)
{
    std::shared_ptr<wentaitek_s3_watch_deviceStruct> s3_watch_device = std::make_shared<wentaitek_s3_watch_deviceStruct>();

    if ( item_device.isMember("Blood_Oxygen") )
    {
        s3_watch_device->Blood_Oxygen = item_device["Blood_Oxygen"].asInt();
    }
    if ( item_device.isMember("DBP") )
    {
        s3_watch_device->DBP = item_device["DBP"].asInt();
    }
    if ( item_device.isMember("Decive ID") )
    {
        s3_watch_device->Device_ID = item_device["Decive ID"].asString();
    }
    if ( item_device.isMember("Heart_Rate") )
    {
        s3_watch_device->Heart_Rate = item_device["Heart_Rate"].asInt();
    }
    if ( item_device.isMember("Rssi") )
    {
        s3_watch_device->Rssi = item_device["Rssi"].asInt();
    }
    if ( item_device.isMember("SBP") )
    {
        s3_watch_device->SBP = item_device["SBP"].asInt();
    }
    if ( item_device.isMember("SOS") )
    {
        s3_watch_device->SOS = item_device["SOS"].asInt();
    }
    if ( item_device.isMember("Temperature") )
    {
        s3_watch_device->Temperature = item_device["Temperature"].asDouble();
    }

    return s3_watch_device;
}

std::shared_ptr<wentaitek_uw_sensor_1_deviceStruct> parse_wentaitek_uw_sensor_1_device(Json::Value & item_device)
{
    std::shared_ptr<wentaitek_uw_sensor_1_deviceStruct> uw_sensor_1_device = std::make_shared<wentaitek_uw_sensor_1_deviceStruct>();
    if ( item_device.isMember("Device ID") )
    {
        uw_sensor_1_device->Device_ID = item_device["Device ID"].asString();
    }
    if ( item_device.isMember("Rssi") )
    {
        uw_sensor_1_device->Rssi = convert_string_to_int(item_device["Rssi"].asString());
    }

    return uw_sensor_1_device;
}

std::shared_ptr<wentaitek_power_meter_deviceStruct> parse_wentaitek_power_meter_device(Json::Value & item_device)
{
    std::shared_ptr<wentaitek_power_meter_deviceStruct> power_meter_device = std::make_shared<wentaitek_power_meter_deviceStruct>();

    if ( item_device.isMember("Active_Energy") )
    {
        power_meter_device->Active_Energy = item_device["Active_Energy"].asDouble();
    }
    if ( item_device.isMember("Active_Power") )
    {
        power_meter_device->Active_Power = item_device["Active_Power"].asDouble();
    }
    if ( item_device.isMember("Device_ID") )
    {
        power_meter_device->Device_ID = item_device["Device_ID"].asString();
    }
    if ( item_device.isMember("BT_version") )
    {
        power_meter_device->BT_version = item_device["BT_version"].asString();
    }
    if ( item_device.isMember("Current_RMS") )
    {
        power_meter_device->Current_RMS = item_device["Current_RMS"].asDouble();
    }
    if ( item_device.isMember("Frequency") )
    {
        power_meter_device->Frequency = item_device["Frequency"].asDouble();
    }
    if ( item_device.isMember("Rssi") )
    {
        power_meter_device->Rssi = item_device["Rssi"].asInt();
    }
    if ( item_device.isMember("Voltage_RMS") )
    {
        power_meter_device->Voltage_RMS = item_device["Voltage_RMS"].asDouble();
    }
    if ( item_device.isMember("relay_state") )
    {
        power_meter_device->relay_state = item_device["relay_state"].asBool();
    }

    return power_meter_device;
}

std::string processMessage(std::string cur_type, std::string topicname, Json::Value & item, std::string macAddr, std::string gwid)
{
    if ( item.isMember("data") )
    {
        return processLoRAGatewayMessage(cur_type, topicname, item);
    }
    else
    {
        // vilslog::printf ("MQTTSubscribeService::processMessage() processNBIoTMessage gwid[%s] macAddr[%s]\n", gwid.c_str(), macAddr.c_str());

        return processNBIoTMessage(cur_type, topicname, item, macAddr, gwid, 0, currentDateTimeForSQL());
    }
}

std::string processLoRAGatewayMessage(std::string cur_type, std::string topicname, Json::Value & item)
{
    int rssi = 0;
    std::string macAddr = "";
    std::string timestamp = currentDateTimeForSQL();

    int gatewayinfoCnt = 0;
    LoRaGatewayInfoStruct geteway;
    geteway.projectid = 1;
    geteway.sf = 10;
    geteway.gwip = "";
    geteway.gwid = "";
    geteway.repeater = "";
    geteway.maplayer = 0;
    geteway.maingroupid = 1;

    if ( item.isMember("macAddr") )
    {
        macAddr = item["macAddr"].asString();
    }

    if ( item.isMember("sf") )
    {
        geteway.sf = item["sf"].asInt();
        gatewayinfoCnt++;
    }

    if ( item.isMember("time") )
    {
        timestamp = item["time"].asString();
        time_t gateTime = StringUTCDateTimeTotime_t(timestamp.c_str());
        time_t curTime = getCurrentTimeInSec();
        time_t diff_t = curTime - gateTime;

        //vilslog::printf("messageArrived() timestamp[%s] diff_t[%d]\n", timestamp, (int)diff_t);

        if ( (int)diff_t > 60)
        {
            DataItem * senditem = new DataItem("resetLoRA", 1);
            glmqttMainJobQueue->add(senditem);
        }
    }

    if ( item.isMember("gwip") )
    {
        geteway.gwip = item["gwip"].asString();
        gatewayinfoCnt++;
    }

    if ( item.isMember("gwid") )
    {
        geteway.gwid = item["gwid"].asString();

        if (!validateID(geteway.gwid))
        {
            geteway.gwid = "";
        }

        gatewayinfoCnt++;
    }

    if ( item.isMember("repeater") )
    {
        geteway.repeater = item["repeater"].asString();
        gatewayinfoCnt++;
    }

    if ( item.isMember("channel") )
    {
        char channel[128];
        memset(channel, 0, 128);
        sprintf(channel, "%d", item["channel"].asInt());
        // geteway.channel = channel;
        geteway.channels.push_back(channel);
        gatewayinfoCnt++;
    }

    if ( item.isMember("rssi") )
    {
        rssi = item["rssi"].asInt();
    }

    int projectid = 1;
    if (gatewayinfoCnt >= 5)
    {
        if (geteway.gwid.length()  > 0)
        {
            projectid = glMQTTDeviceManager->getLoRaGatewayProjectId(geteway.gwid);
            geteway.projectid = projectid;
            send_loragateway_to_projects(&geteway);
        }
    }

    if ( item.isMember("data") )
    {
        const char * jsondata = item["data"].asCString();

        std::string jsonString = HEXStringToString(jsondata);

        // vilslog::printf ("processLoRAGatewayMessage() jsonString[%s]\n", jsonString.c_str());

        if (geteway.gwid.length()  > 0)
        {
            send_LoRaDeviceALIVE_to_projects(macAddr.c_str(), geteway.gwid.c_str(), rssi, 0);
        }

        Json::Value JSONLora;
        Json::Reader * readerData = new Json::Reader();
        bool parsingSuccessful = readerData->parse( jsonString, JSONLora);
        delete readerData;

        if (parsingSuccessful)
        {
            // vilslog::printf ("processLoRAGatewayMessage() parsingSuccessful[%d]\n", parsingSuccessful);
            if (JSONLora.isArray())
            {
                std::string msg_type = cur_type;

                for(unsigned int i=0;i<JSONLora.size();i++)
                {
                    Json::Value tmp;
                    Json::Value itemData = JSONLora.get((Json::UInt)i, tmp);

                    std::string ret_type = processNBIoTMessage(msg_type, topicname, itemData, macAddr, geteway.gwid, rssi, timestamp);

                    if (ret_type.length() > 0)
                        msg_type = ret_type;
                    else
                    {
                        // vilslog::printf ("processLoRAGatewayMessage() parsingSuccessful[%d]\n", parsingSuccessful);
                        // vilslog::printf ("processLoRAGatewayMessage() projectid[%d] macAddr[%s] gwid[%s] rssi[%d] timestamp[%s]\n",
                        //     projectid, macAddr.c_str(), geteway.gwid.c_str(), rssi, timestamp.c_str());
                        do_parse_data(projectid, macAddr.c_str(), itemData, geteway.gwid.c_str(), rssi, timestamp.c_str());
                    }

                    // if ( itemData.isMember("type") )
                    // {
                    //     //msg_type = JSONLora["type"].asString();
                    //     msg_type = processLoRAGatewayMessage(msg_type, topicname, itemData);
                    // }
                    // else
                    // {
                    //     vilslog::printf ("processLoRAGatewayMessage() parsingSuccessful[%d] not JSONLora.isMember(type)\n", parsingSuccessful);
                    // }
                }
                return msg_type;
            }
        }

        if (jsondata[0] == '2' && jsondata[1] == '8')
        {
            int ret = do_parse_compress_data(projectid, macAddr.c_str(), jsondata, geteway.gwid.c_str(), rssi, timestamp.c_str());
            if ( ret == 1 )
            {
#ifdef PRINT_MQTT_SUB_MSG
                vilslog::printf ("do_parse_compress_data() failed!\n");
#endif
            }
        }
        else
        if (jsondata[0] == '5' && jsondata[1] == 'b')
        {
            int ret = do_parse_compress_multiple_data(projectid, macAddr.c_str(), jsondata, geteway.gwid.c_str(), rssi, timestamp.c_str());
            if ( ret == 1 )
            {
#ifdef PRINT_MQTT_SUB_MSG
                vilslog::printf ("do_parse_compress_multiple_data() failed!\n");
#endif
            }
        }
        else
        if (jsondata[0] == '3' && jsondata[1] == 'c')
        {
            int ret = do_parse_compress_AOA_data(projectid, macAddr.c_str(), jsondata, geteway.gwid.c_str(), rssi, timestamp.c_str());
            if ( ret == 1 )
            {
#ifdef PRINT_MQTT_SUB_MSG
                vilslog::printf ("do_parse_compress_AOA_data() failed!\n");
#endif
            }
        }
        else
        if (jsondata[0] == '2' && jsondata[1] == '3')
        {
            int ret = do_parse_compress_AOA_IMU_data(projectid, macAddr.c_str(), jsondata, geteway.gwid.c_str(), rssi, timestamp.c_str());
            if ( ret == 1 )
            {
        #ifdef PRINT_MQTT_SUB_MSG
                vilslog::printf ("do_parse_compress_AOA_IMU_data() failed!\n");
        #endif
            }
        }
        else
        if (jsondata[0] == 'f' && jsondata[1] == '0' && jsondata[strlen(jsondata) - 2] == '0' && jsondata[strlen(jsondata) - 1] == 'f')
        {
            // check DMATEK packet
            // vilslog::printf ("processLoRAGatewayMessage() DMATEK data [%s]\n", jsondata);
            DataItem * senditem = new DataItem("DMATEKpacket", 1);
            senditem->setMessage(jsondata);
            glmqttMainJobQueue->add(senditem);
        }
        else
        {
            do_parse_data(projectid, macAddr.c_str(), jsondata, geteway.gwid.c_str(), rssi, timestamp.c_str());
        }
        //printf ("\n");
    }

    return cur_type;
}

std::string processNBIoTMessage(std::string cur_type, std::string topicname, Json::Value & item, std::string LoRAMacaddress, std::string gwid, int rssi, std::string timestamp)
{
    std::string msg_type = cur_type;
    std::string cardid = "";
    std::string macAddr = "";

    if ( item.isMember("type") )
    {
        msg_type = item["type"].asString();
    }

    if ( item.isMember("cardid") )
    {
        cardid = item["cardid"].asString();
    }

    if ( item.isMember("macAddr") )
    {
        macAddr = item["macAddr"].asString();
    }

    if ( item.isMember("macaddress") )
    {
        macAddr = item["macaddress"].asString();
    }

    if ( item.isMember("time") )
    {
        timestamp = item["time"].asString();
    }

    // if ( item.isMember("Tp") )
    // {
    //     vilslog::printf ("processNBIoTMessage() Tp[%s]\n", item["Tp"].asCString());
    // }

    if ( item.isMember("alive") )
    {
        vilslog::printf ("processNBIoTMessage() alive[%s]\n", item["alive"].asCString());
        // send LoRa Device alive
        //send_LoRaDeviceALIVE_to_projects(LoRAMacaddress.c_str(), gwid.c_str(), 0);
    }

    int AoAClass = 0;
    if ( item.isMember("Class") )
    {
        AoAClass = convert_string_to_int(item["Class"].asString());
    }

    // send LoRa Device alive
    send_LoRaDeviceALIVE_to_projects(LoRAMacaddress.c_str(), gwid.c_str(), 0, AoAClass);

    // vilslog::printf ("processNBIoTMessage() msg_type[%s] cur_type[%s]\n", msg_type.c_str(), cur_type.c_str());
    // vilslog::printf ("type[%s] macAddr[%s] LoRAMacaddress[%s] time[%s]\n", msg_type.c_str(), macAddr.c_str(), LoRAMacaddress.c_str(), timestamp.c_str());

    if (cardid.length() != 0)
    {
        //vilslog::printf ("cardid[%s] time[%s]\n", cardid.c_str(), timestamp.c_str());
        // vilslog::printf ("cardid[%s] type[%s] macAddr[%s] gwid[%s] LoRAMacaddress[%s] time[%s]\n",
        //     cardid.c_str(), msg_type.c_str(), macAddr.c_str(), gwid.c_str(), LoRAMacaddress.c_str(), timestamp.c_str());

        if (cur_type.compare("nfc") == 0)
        {
            if (validateID(cardid))
            {
                NFCCardStruct nfccard;
                nfccard.cardid = cardid;
                nfccard.datetime = timestamp;
                nfccard.updatetime = timestamp;
                MQTTDataItem * mqttitem = new MQTTDataItem("nfccard", 1, &nfccard);
                mqttitem->setLoRAMacaddress(LoRAMacaddress.c_str());
                mqttitem->setGatewayid(gwid.c_str());
                glqueue->add(mqttitem);
            }
        }
        else
        if (cur_type.compare("rfid") == 0)
        {
            if (validateID(cardid))
            {
                RFIDCardStruct rfidcard;
                rfidcard.cardid = cardid;
                rfidcard.datetime = timestamp;
                rfidcard.updatetime = timestamp;
                MQTTDataItem * mqttitem = new MQTTDataItem("rfidcard", 1, &rfidcard);
                mqttitem->setLoRAMacaddress(LoRAMacaddress.c_str());
                mqttitem->setGatewayid(gwid.c_str());
                glqueue->add(mqttitem);
            }
        }
    }

    if (msg_type.compare("raspberrypi") == 0)
    {
        // {"type":"raspberrypi","macAddr":"b827eba80335","msg":"boot","time":"2020-05-22 16:03:23"}
        // {"type":"raspberrypi","macAddr":"b827eba80335","msg":"alive","time":"2020-05-22 16:03:23"}
        std::string msg;
        std::string version;
        if ( item.isMember("msg") )
        {
            msg = item["msg"].asString();

            if ( item.isMember("v") )
                version = item["v"].asString();


            DataItem * senditem = new DataItem(msg.c_str(), 1);
            glmqttMainJobQueue->add(senditem);

            vilslog::printf ("processNBIoTMessage() type[%s] macAddr[%s] LoRAMacaddress[%s] time[%s] msg[%s] version[%s]\n",
                msg_type.c_str(), macAddr.c_str(), LoRAMacaddress.c_str(), timestamp.c_str(), msg.c_str(), version.c_str());
        }

        //processNBIoTMessage() type[raspberrypi] macAddr[5551000000000010] LoRAMacaddress[00000000035f0010] time[2020-09-07 12:20:36] msg[alive] version[1.1.0]

        EDGEDeviceStruct edgedevice;
        edgedevice.alive = 1;
        edgedevice.macaddress = macAddr;
        edgedevice.name = "Edge Control";
        edgedevice.type = "raspberrypi";
        edgedevice.LoRAMacaddress = LoRAMacaddress;
        edgedevice.gwid = gwid;
        edgedevice.version = version;
        edgedevice.datetime = timestamp;

        MQTTDataItem * mqttitem = new MQTTDataItem("EDGEDevice", 1, &edgedevice);
        glqueue->add(mqttitem);

    }
    else
    if (msg_type.compare("esp32") == 0)
    {
        // {"type":"esp32","macAddr":"c42c031ecc34","msg":"boot","time":"2020-05-22 16:03:23"}
        // {"type":"esp32","macAddr":"c42c031ecc34","msg":"alive","time":"2020-05-22 16:03:23"}
        std::string msg;
        std::string ssid;
        std::string wifimac;
        std::string devicetype = "ESP32";

        if ( item.isMember("msg") )
        {
            msg = item["msg"].asString();

            DataItem * senditem = new DataItem(msg.c_str(), 1);
            glmqttMainJobQueue->add(senditem);

            if ( msg.compare("poweron") == 0 )
            {
                send_NFCLocatorPOWERON_to_projects(LoRAMacaddress.c_str(), macAddr.c_str(), gwid.c_str());
            }
            else
            if ( msg.compare("wifiinfo") == 0 )
            {
                if ( item.isMember("ssid") )
                {
                    ssid = item["ssid"].asString();
                }
                if ( item.isMember("wifimac") )
                {
                    wifimac = item["wifimac"].asString();
                }
                devicetype = "WIFI";
            }
        }
        std::string version;
        if ( item.isMember("V") )
        {
            version = item["V"].asString();
        }

        //vilslog::printf ("type[%s] macAddr[%s] LoRAMacaddress[%s] msg[%s] time[%s]\n", msg_type.c_str(), macAddr.c_str(), LoRAMacaddress.c_str(), msg.c_str(), timestamp.c_str());

        NFCDeviceStruct nfcdevice;
        nfcdevice.alive = 1;
        nfcdevice.macaddress = macAddr;
        nfcdevice.name = "NFC Reader";
        nfcdevice.type = devicetype;
        nfcdevice.gwid = gwid;
        nfcdevice.LoRAMacaddress = LoRAMacaddress;
        nfcdevice.datetime = timestamp;
        nfcdevice.ssid = ssid;
        nfcdevice.wifimac = wifimac;
        nfcdevice.version = version;
        nfcdevice.maingroupid = 1;

        MQTTDataItem * mqttitem = new MQTTDataItem("NFCDevice", 1, &nfcdevice);
        glqueue->add(mqttitem);

        vilslog::printf ("processNBIoTMessage() type[%s] macAddr[%s] LoRAMacaddress[%s] time[%s] msg[%s] version[%s]\n",
            msg_type.c_str(), macAddr.c_str(), LoRAMacaddress.c_str(), timestamp.c_str(), msg.c_str(), version.c_str());

        // std::string time = currentDateTimeForESP32();
        // std::string cmd = "AT+DATETIME=";
        // cmd.append(time);
        // send_MQTT_cmd(1, cmd.c_str(), &nfcdevice);
        //send_MQTT_cmd(1, "AT+DATETIME=2020,05,22,16,03,23", &nfcDevice);
    }
    else
    if (msg_type.compare("ZebraRX7500") == 0)
    {
        // {"type":"ZebraRX7500","macAddr":"84248d801683","msg":"alive","time":"2020-05-22 16:03:23"}
        std::string msg;
        if ( item.isMember("msg") )
        {
            msg = item["msg"].asString();

            DataItem * senditem = new DataItem(msg.c_str(), 1);
            glmqttMainJobQueue->add(senditem);
        }

        //vilslog::printf ("type[%s] macAddr[%s] LoRAMacaddress[%s] msg[%s] time[%s]\n", msg_type.c_str(), macAddr.c_str(), LoRAMacaddress.c_str(), msg.c_str(), timestamp.c_str());

        RFIDDeviceStruct rfiddevice;
        rfiddevice.alive = 1;
        rfiddevice.macaddress = macAddr;
        rfiddevice.name = "RFID Reader";
        rfiddevice.type = "ZebraRX7500";
        rfiddevice.gwid = gwid;
        rfiddevice.LoRAMacaddress = LoRAMacaddress;
        rfiddevice.datetime = timestamp;

        MQTTDataItem * mqttitem = new MQTTDataItem("RFIDDevice", 1, &rfiddevice);
        glqueue->add(mqttitem);

        vilslog::printf ("processNBIoTMessage() type[%s] macAddr[%s] LoRAMacaddress[%s] time[%s] msg[%s]\n",
            msg_type.c_str(), macAddr.c_str(), LoRAMacaddress.c_str(), timestamp.c_str(), msg.c_str());

    }
    else
    if (msg_type.compare("rfid") == 0)
    {
        // [{"type":"rfid","macAddr":"b827eba80335"},{"cardid":"E2004462620E0168203041B9","time":"2020-05-22 16:03:23"}]
        // vilslog::printf ("type[%s] macAddr[%s]\n", msg_type.c_str(), macAddr.c_str());
        std::string msg;
        if ( item.isMember("msg") )
        {
            msg = item["msg"].asString();

            DataItem * senditem = new DataItem(msg.c_str(), 1);
            glmqttMainJobQueue->add(senditem);
        }
        //vilslog::printf ("type[%s] macAddr[%s] LoRAMacaddress[%s] msg[%s] time[%s]\n", msg_type.c_str(), macAddr.c_str(), LoRAMacaddress.c_str(), msg.c_str(), timestamp.c_str());
        if (macAddr.length() > 0 )
        {
            RFIDDeviceStruct rfiddevice;
            rfiddevice.alive = 1;
            rfiddevice.macaddress = macAddr;
            rfiddevice.name = "RFID Reader";
            rfiddevice.type = "ZebraRX7500";
            rfiddevice.gwid = gwid;
            rfiddevice.LoRAMacaddress = LoRAMacaddress;
            rfiddevice.datetime = timestamp;

            MQTTDataItem * mqttitem = new MQTTDataItem("RFIDDevice", 1, &rfiddevice);
            glqueue->add(mqttitem);

            if ( item.isMember("ack") )
            {
                std::string ack = item["ack"].asString();
                //vilslog::printf ("type[%s] macAddr[%s] LoRAMacaddress[%s] ack[%s]\n", msg_type.c_str(), macAddr.c_str(), LoRAMacaddress.c_str(), ack.c_str());

                MQTTDataItem * mqttitemack = new MQTTDataItem("ACK", 1, LoRAMacaddress.c_str(), ack);
                glqueue->add(mqttitemack);
            }
        }
    }
    else
    if (msg_type.compare("nfc") == 0)
    {
        //[{"type":"nfc","macAddr":"5352000000000240"},{"cardid":"E393BBE9","time":"2021-08-09 12:03:23"}]
        // vilslog::printf ("type[%s] macAddr[%s]\n", msg_type.c_str(), macAddr.c_str());
        std::string msg;
        if ( item.isMember("msg") )
        {
            msg = item["msg"].asString();

            DataItem * senditem = new DataItem(msg.c_str(), 1);
            glmqttMainJobQueue->add(senditem);
        }

        if (macAddr.length() > 0 )
        {
            NFCDeviceStruct nfcdevice;
            nfcdevice.alive = 1;
            nfcdevice.macaddress = macAddr;
            nfcdevice.name = "NFC Reader";
            nfcdevice.type = "";
            nfcdevice.gwid = gwid;
            nfcdevice.LoRAMacaddress = LoRAMacaddress;
            nfcdevice.datetime = timestamp;
            nfcdevice.version = "";
            nfcdevice.maingroupid = 1;

            MQTTDataItem * mqttitem = new MQTTDataItem("NFCDevice", 1, &nfcdevice);
            glqueue->add(mqttitem);
        }
    }
    else
    if (msg_type.compare("SMIMSLoRa") == 0)
    {
        // [{"type":"SMIMSLoRa","macAddr":"84248d801683","msg":"alive","time":"2020-05-22 16:03:23"}]
        // [{"type":"SMIMSLoRa","macAddr":"84248d801683","msg":"boot","time":"2020-05-22 16:03:23"}]
        std::string msg;
        if ( item.isMember("msg") )
        {
            msg = item["msg"].asString();

            sleep(1);

            DataItem * senditem = new DataItem(msg.c_str(), 1);
            glmqttMainJobQueue->add(senditem);

            if ( msg.compare("poweron") == 0 )
            {
                send_NFCLocatorPOWERON_to_projects(LoRAMacaddress.c_str(), macAddr.c_str(), gwid.c_str());
            }
        }
        std::string version;
        if ( item.isMember("V") )
        {
            version = item["V"].asString();
        }
        //vilslog::printf ("type[%s] macAddr[%s] LoRAMacaddress[%s] msg[%s] time[%s]\n", msg_type.c_str(), macAddr.c_str(), LoRAMacaddress.c_str(), msg.c_str(), timestamp.c_str());

        NFCDeviceStruct nfcdevice;
        nfcdevice.alive = 1;
        nfcdevice.macaddress = macAddr;
        nfcdevice.name = "NFC Reader";
        nfcdevice.type = "SMIMS LoRa";
        nfcdevice.gwid = gwid;
        nfcdevice.LoRAMacaddress = LoRAMacaddress;
        nfcdevice.datetime = timestamp;
        nfcdevice.version = version;
        nfcdevice.maingroupid = 1;

        MQTTDataItem * mqttitem = new MQTTDataItem("NFCDevice", 1, &nfcdevice);
        glqueue->add(mqttitem);

        vilslog::printf ("processNBIoTMessage() type[%s] macAddr[%s] LoRAMacaddress[%s] time[%s] msg[%s] version[%s]\n",
            msg_type.c_str(), macAddr.c_str(), LoRAMacaddress.c_str(), timestamp.c_str(), msg.c_str(), version.c_str());

        // std::string time = currentDateTimeForESP32();
        // std::string cmd = "AT+DATETIME=";
        // cmd.append(time);
        // send_MQTT_cmd(1, cmd.c_str(), &nfcdevice);
        //send_MQTT_cmd(1, "AT+DATETIME=2020,05,22,16,03,23", &nfcDevice);
    }
    else
    if (msg_type.compare("HAR") == 0)
    {
        std::string activity;
        if ( item.isMember("activity") )
        {
            activity = item["activity"].asString();
        }

        // vilslog::printf ("processNBIoTMessage() type[%s] macAddr[%s] LoRAMacaddress[%s] time[%s] activity[%s]\n",
        //     msg_type.c_str(), macAddr.c_str(), LoRAMacaddress.c_str(), timestamp.c_str(), activity.c_str());

        DataItem * senditem = new DataItem("HAR", 1, macAddr.c_str(), NODE_TYPE_TAG);
        senditem->setMessage(activity);
        glmqttMainJobQueue->add(senditem);

        //-------------------
        DataItem * senditem1 = new DataItem("HAR", 1, macAddr.c_str(), NODE_TYPE_TAG);
        senditem1->setMessage(activity);
        glTagActivityPublisher->publish(senditem1);
        //-------------------
    }
    else
    if (msg_type.compare("beacon") == 0)
    {
        // vilslog::printf("MQTTSubscribeService::processNBIoTMessage() beacon\n");

        // "mac":"00B0F424EC629EFC"
        //  00B0+(beacon上預設mac address)

        std::string minor = "0";
        std::string major = "0";
        std::string data = "";
        std::string mac = "";

        if ( item.isMember("minor") )
            minor = item["minor"].asString();
        if ( item.isMember("major") )
            major = item["major"].asString();
        if ( item.isMember("data") )
            data = item["data"].asString();
        if ( item.isMember("mac") )
            mac = item["mac"].asString();

        char value[256];
        memset(value, 0, 256);
        sprintf(value, "{\"major\":\"%s\",\"minor\":\"%s\",\"mac\":\"%s\",\"data\":\"%s\"}", major.c_str(), minor.c_str(), mac.c_str(), data.c_str());

        // vilslog::printf("MQTTSubscribeService::processNBIoTMessage() beacon mac[%s]\n", mac.c_str());

        std::string valueString = std::string(value);

        MQTTDataItem * mqttitem = new MQTTDataItem("beacon", 1, LoRAMacaddress.c_str(), valueString);
        mqttitem->setGatewayid(gwid.c_str());
        mqttitem->setRSSI(rssi);
        mqttitem->setTimestamp(timestamp.c_str());
        mqttitem->setTagMacaddress(mac.c_str());
        glqueue->add(mqttitem);
    }
    else
    if (msg_type.compare("Watch") == 0)
    {
        vilslog::printf("MQTTSubscribeService::processNBIoTMessage() Watch[%s]\n", macAddr.c_str());

        SensorDeviceStruct sensorDevice;
        sensorDevice.alive = 1;
        sensorDevice.id = macAddr;
        sensorDevice.name = "";
        sensorDevice.type = "Watch";
        sensorDevice.jsonvalue = "";
        sensorDevice.Longitude = "";
        sensorDevice.Latitude = "";
        sensorDevice.Battery = "";
        sensorDevice.Version = "";
        sensorDevice.datetime = "";
        sensorDevice.maingroupid = 1;

        std::string HR;
        std::string SPO2;

        if ( item.isMember("name") )
        {
            sensorDevice.name = item["name"].asString();
        }
        if ( item.isMember("lng") )
        {
            sensorDevice.Longitude = item["lng"].asString();
        }
        if ( item.isMember("lat") )
        {
            sensorDevice.Latitude = item["lat"].asString();
        }
        if ( item.isMember("Battery") )
        {
            sensorDevice.Battery = item["Battery"].asString();
        }
        if ( item.isMember("HR") )
        {
            HR = item["HR"].asString();
        }
        if ( item.isMember("SPO2") )
        {
            SPO2 = item["SPO2"].asString();
        }
        if ( item.isMember("Version") )
        {
            sensorDevice.Version = item["Version"].asString();
        }
        if ( item.isMember("time") )
        {
            sensorDevice.datetime = item["time"].asString();
        }

        char value[256];
        memset(value, 0, 256);
        sprintf(value, "{\"name\":\"%s\",\"lng\":\"%s\",\"lat\":\"%s\",\"Battery\":\"%s\",\"HR\":\"%s\",\"SPO2\":\"%s\",\"Version\":\"%s\",\"time\":\"%s\"}",
            sensorDevice.name.c_str(), sensorDevice.Longitude.c_str(), sensorDevice.Latitude.c_str(),
            sensorDevice.Battery.c_str(), HR.c_str(), SPO2.c_str(), sensorDevice.Version.c_str(), sensorDevice.datetime.c_str());

        sensorDevice.jsonvalue = std::string(value);

        MQTTDataItem * mqttitem = new MQTTDataItem("Watch", 1, &sensorDevice);
        glqueue->add(mqttitem);
    }
    else
    if (msg_type.compare("Network Gateway") == 0)
    {
        vilslog::printf("MQTTSubscribeService::processNBIoTMessage() Network Gateway[%s]\n", macAddr.c_str());

        SensorDeviceStruct sensorDevice;
        sensorDevice.alive = 1;
        sensorDevice.id = macAddr;
        sensorDevice.name = "";
        sensorDevice.type = "Network Gateway";
        sensorDevice.jsonvalue = "";
        sensorDevice.Longitude = "";
        sensorDevice.Latitude = "";
        sensorDevice.Battery = "";
        sensorDevice.Version = "";
        sensorDevice.datetime = "";
        sensorDevice.maingroupid = 1;

        std::string ip;
        std::string i;
        std::string v;
        std::string w;
        std::string wacc;
        std::string status;

        if ( item.isMember("name") )
        {
            sensorDevice.name = item["name"].asString();
        }
        if ( item.isMember("i") )
        {
            i = item["i"].asString();
        }
        if ( item.isMember("v") )
        {
            v = item["v"].asString();
        }
        if ( item.isMember("w") )
        {
            w = item["w"].asString();
        }
        if ( item.isMember("wacc") )
        {
            wacc = item["wacc"].asString();
        }
        if ( item.isMember("status") )
        {
            status = item["status"].asString();
        }
        if ( item.isMember("Version") )
        {
            sensorDevice.Version = item["Version"].asString();
        }
        if ( item.isMember("time") )
        {
            sensorDevice.datetime = item["time"].asString();
        }

        char value[256];
        memset(value, 0, 256);
        sprintf(value, "{\"name\":\"%s\",\"i\":\"%s\",\"v\":\"%s\",\"w\":\"%s\",\"wacc\":\"%s\",\"status\":\"%s\",\"Version\":\"%s\",\"time\":\"%s\"}",
            sensorDevice.name.c_str(), i.c_str(), v.c_str(), w.c_str(), wacc.c_str(), status.c_str(),
            sensorDevice.Version.c_str(), sensorDevice.datetime.c_str());

        sensorDevice.jsonvalue = std::string(value);
        sensorDevice.Battery = v;

        MQTTDataItem * mqttitem = new MQTTDataItem("Network Gateway", 1, &sensorDevice);
        glqueue->add(mqttitem);
    }
    else
    if (msg_type.compare("iCover") == 0)
    {
        //{"SW":"F","IMU":"0.07;0.22;-1.02;-0.42;0.28;-0.14","Volt":"3.44","RSSI","21"}

        std::string SW;
        std::string IMU;
        std::string WU;
        std::string Volt;
        std::string RSSI;

        if ( item.isMember("SW") )
            SW = item["SW"].asString();
        if ( item.isMember("IMU") )
            IMU = item["IMU"].asString();
        if ( item.isMember("WU") )
            WU = item["WU"].asString();
        if ( item.isMember("Volt") )
            Volt = item["Volt"].asString();
        if ( item.isMember("RSSI") )
            RSSI = item["RSSI"].asString();

        vilslog::printf("MQTTSubscribeService::do_parse_data() iCover LoRAMacaddress[%s] SW[%s] IMU[%s] WU[%s] Volt[%s] RSSI[%s]\n",
            LoRAMacaddress.c_str(), SW.c_str(), IMU.c_str(), WU.c_str(), Volt.c_str(), RSSI.c_str());

        char value[256];
        memset(value, 0, 256);
        sprintf(value, "{\"IMEI\":\"%s\",\"SW\":\"%s\",\"IMU\":\"%s\",\"WU\":\"%s\",\"Volt\":\"%s\",\"RSSI\":\"%s\"}",
            LoRAMacaddress.c_str(), SW.c_str(), IMU.c_str(), WU.c_str(), Volt.c_str(), RSSI.c_str());
        //{"ID":"868333030959270","Switch":"True","IMU":"AccelX;AccelY;AccelZ;GyroX;GyroY;GyroZ","Voltage":"4.2","Time":"2022-02-24T13:45:30","RSSI"="20"}
        // vilslog::printf("MQTTSubscribeService::do_parse_data() beacon mac[%s]\n", mac.c_str());

        send_iCover_to_projects(LoRAMacaddress.c_str(), LoRAMacaddress.c_str(), value, gwid.c_str(), rssi, timestamp.c_str());
    }

    return msg_type;
}

int do_parse_data(int projectid, const char * LoRAMacAddr, Json::Value & JSONLora, const char * gatewayid, int rssi, const char * timestamp)
{
    char type[256];
    char macaddress[256];
    char distance[256];
    char voltage[256];
    memset(type, 0, 256);
    memset(macaddress, 0, 256);
    memset(distance, 0, 256);
    memset(voltage, 0, 256);

    if ( JSONLora.isMember("Tp"))
    {
        strncpy(type, JSONLora["Tp"].asCString(), 256);
    }
    else
    if ( JSONLora.isMember("tp"))
    {
        strncpy(type, JSONLora["tp"].asCString(), 256);
    }
    else
    if ( JSONLora.isMember("Lat") )
    {
        strcpy(type, "GPS");
    }
    else
    if ( JSONLora.isMember("T") )
    {
        //vilslog::printf ("do_parse_data() T[%s]\n", JSONLora["T"].asCString());
        strncpy(type, JSONLora["T"].asCString(), 256);
    }
    else
    if ( JSONLora.isMember("type") )
    {
        //vilslog::printf ("do_parse_data() T[%s]\n", JSONLora["T"].asCString());
        strncpy(type, JSONLora["type"].asCString(), 256);
    }
    else
    if ( JSONLora.isMember("alive") )
    {
        vilslog::printf ("MQTTSubscribeService::do_parse_data() alive[%s] LoRAMacAddr[%s]\n", JSONLora["alive"].asCString(), LoRAMacAddr);

        int AoAClass = 0;
        if ( JSONLora.isMember("Class") )
        {
            AoAClass = convert_string_to_int(JSONLora["Class"].asString());
        }

        // send LoRa Device alive
        send_LoRaDeviceALIVE_to_projects(LoRAMacAddr, gatewayid, 0, AoAClass);
    }
    else
    if ( JSONLora.isMember("lidar") )
    {
        char value[256];
        memset(value, 0, 256);
        strncpy(value, JSONLora["lidar"].asCString(), 256);

        send_lidarinfo_to_projects(LoRAMacAddr, value, gatewayid, rssi, timestamp);
    }

    if (strcmp(type, "E") == 0)
    {
        if ( JSONLora.isMember("F") )
        {
            strncpy(macaddress, JSONLora["F"].asCString(), 256);
        }

        send_alert_to_projects(LoRAMacAddr, macaddress, gatewayid, timestamp);

        //MQTTDataItem * mqttitem = new MQTTDataItem("alert", projectid, LoRAMacAddr, macaddress, gatewayid, timestamp);
        //glqueue->add(mqttitem);

    }
    else
    if (strcmp(type, "D") == 0)
    {
        if ( JSONLora.isMember("F") )
        {
            strncpy(macaddress, JSONLora["F"].asCString(), 256);
        }
        if ( JSONLora.isMember("D") )
        {
            strncpy(distance, JSONLora["D"].asCString(), 256);
        }
        if ( JSONLora.isMember("P") )
        {
            strncpy(voltage, JSONLora["P"].asCString(), 256);
        }

        int ranging = atoi(distance);

        send_ranging_to_projects(LoRAMacAddr, macaddress, ranging, voltage, gatewayid, rssi, timestamp);

        //MQTTDataItem * mqttitem = new MQTTDataItem("ranging", projectid, LoRAMacAddr, macaddress, ranging, voltage, gatewayid, rssi, timestamp);
        //glqueue->add(mqttitem);
    }
    else
    if (strcmp(type, "CANCEL") == 0)
    {
        // "Tp":"CLOSE" cancel alert data
        // "F":"MAC ADDRESS"

        // "Tp":"CLOSE" cancel alert data
        // "F":"SELF"
        if ( JSONLora.isMember("F") )
        {
            strncpy(macaddress, JSONLora["F"].asCString(), 256);
        }

        send_cancel_to_projects(LoRAMacAddr, macaddress, gatewayid, timestamp);

        //MQTTDataItem * mqttitem = new MQTTDataItem("cancel", projectid, LoRAMacAddr, macaddress, gatewayid, timestamp);
        //glqueue->add(mqttitem);
    }
    else
    if (strcmp(type, "GPS") == 0)
    {
        //jsondata[{"Lat":"2239.8676","LatDir":"N","Lon":"12018.379","LonDir":"E"}
        // {"Lat":"0.0","LatDir":"N","Lon":"0.0","LonDir":"E"}
        // std::string s_Latitude = JSONLora["Lat"].asCString();
        // std::string s_Longitude = JSONLora["Lon"].asCString();
        std::string s_Latitude = filterValidNumberString(JSONLora["Lat"].asCString(), (int)JSONLora["Lat"].asString().length());
        std::string s_Longitude = filterValidNumberString(JSONLora["Lon"].asCString(), (int)JSONLora["Lon"].asString().length());
        std::string s_LatDir = JSONLora["LatDir"].asCString();
        std::string s_LonDir = JSONLora["LonDir"].asCString();

        // s_Latitude.erase(std::remove(s_Latitude.begin(), s_Latitude.end(), '\n'), s_Latitude.end());
        // s_Longitude.erase(std::remove(s_Longitude.begin(), s_Longitude.end(), '\n'), s_Longitude.end());

#ifdef PRINT_MQTT_SUB_MSG
        vilslog::printf ("s_Latitude[%s] s_Longitude[%s]\n", s_Latitude.c_str(), s_Longitude.c_str());
#endif
        double Longitude = convertLatLongToDeg(s_Longitude, s_LonDir);
        double Latitude = convertLatLongToDeg(s_Latitude, s_LatDir);

        //緯度，格式：度度分分.分分分分（ddmm.mmmm）。
        //緯度區分，北半球（N）或南半球（S）。
        //經度，格式：度度分分.分分分分 (dddmm.mmmm)。
        //經度區分，東（E）半球或西（W）半球。
        // 4807.038,N   Latitude 48 deg 07.038' N
        // 01131.000,E  Longitude 11 deg 31.000' E
        // 5133.82,N, -> 51.563667
        // 00042.24,W ->-0.704
        // 4807.038,N   Latitude 48 deg 07.038' N
      // 01131.000,E  Longitude 11 deg 31.000' E

        // //std::string GPRMC = "4807.038N";
        // //std::string GPRMC = "3137.36664N";
        // std::string GPRMC = "2239.8676N";
        // //double degree = DMS::DecimalDegrees(GPRMC);
        // double degree = convertLatLongToDeg("2239.8676", "N");
        // vilslog::printf("GPRMC[%s] -> %f\n", GPRMC.c_str(), degree);
        //
        // //GPRMC = "01131.000E";
        // GPRMC = "12018.379E";
        // degree = convertLatLongToDeg("12018.379", "E");
        // //degree = DMS::DecimalDegrees(GPRMC);
        // vilslog::printf("GPRMC[%s] -> %f\n", GPRMC.c_str(), degree);

        //
        // 檢查是不是 Locator
        //
        NodeStruct * node = glMQTTDeviceManager->get_Locator_node(projectid, LoRAMacAddr);
        if ( node != NULL )
        {
            send_gpsinfo_to_projects("locatorgps", LoRAMacAddr, macaddress, Longitude, Latitude, gatewayid, rssi, timestamp);

            //
            // 產生一個虛擬Tag
            //
            send_gpsinfo_to_projects("taggps", LoRAMacAddr, macaddress, Longitude, Latitude, gatewayid, rssi, timestamp);
        }
        else
        {
            send_gpsinfo_to_projects("taggps", LoRAMacAddr, macaddress, Longitude, Latitude, gatewayid, rssi, timestamp);
        }

        // MQTTDataItem * mqttitem = new MQTTDataItem("taggps", projectid, LoRAMacAddr, macaddress, Longitude, Latitude, gatewayid, timestamp);
        // glqueue->add(mqttitem);
    }
    else
    if (strcmp(type, "ALIVE") == 0)
    {
        // vilslog::printf ("MQTTSubscribeService::do_parse_data() ALIVE LoRAMacAddr[%s] gatewayid[%s]\n", LoRAMacAddr, gatewayid);

        // {"Tp":"ALIVE","Class":"50"}  AoA Class
        int AoAClass = 0;
        if ( JSONLora.isMember("Class") )
        {
            AoAClass = convert_string_to_int(JSONLora["Class"].asString());
        }

        // send LoRa Device alive
        send_LoRaDeviceALIVE_to_projects(LoRAMacAddr, gatewayid, 0, AoAClass);

        // send LoRa Device alive
        //MQTTDataItem * mqttitem = new MQTTDataItem("LoRaDeviceALIVE", projectid, LoRAMacAddr);
        //glqueue->add(mqttitem);
    }
    else
    if (strcmp(type, "S") == 0 || strcmp(type, "PWO") == 0)
    {
        // {"Tp":"PWO","P":"5656","Dev":"07610200","V":"5.80.1-1","Class":"50"} AoA Class
        // {"Tp":"PWO","P":"9999","Dev":"05520420","V":"5.1.3-7","clk":"21808"}
        // Tp : S不用動
        // P : 9999 這邊 9999留空
        // Dev :   這邊是mac address前面8碼
        // V:        版本號
        // clk:       留空

        std::string PANID = "0000";
        std::string MODE = "0";
        std::string Device = "";
        std::string VERSION = "0";
        if ( JSONLora.isMember("P") )
        {
            PANID = JSONLora["P"].asString();
        }
        if ( JSONLora.isMember("Dev") )
        {
            Device = JSONLora["Dev"].asString();
            //std::string serialNum = LoRAMacAddr.substr(6,10);
            sprintf(macaddress, "%s0000%s", Device.c_str(), &LoRAMacAddr[12]);
        }
        if ( JSONLora.isMember("M") )
        {
            MODE = JSONLora["M"].asString();
        }
        if ( JSONLora.isMember("V") )
        {
            VERSION = JSONLora["V"].asString();
        }
        if ( JSONLora.isMember("F") )
        {
            strncpy(macaddress, JSONLora["F"].asCString(), 256);
        }

        if (strcmp(type, "PWO") == 0)
        {
            vilslog::printf("MQTTSubscribeService::do_parse_data() LoRaDevicePOWERON type[%s] gatewayid[%s] LoRAMacAddr[%s] macaddress[%s]\n",
                type, gatewayid, LoRAMacAddr, macaddress);

            send_LoRaDevicePOWERON_to_projects(macaddress, gatewayid);
        }

        send_LoRaDeviceBOOT_to_projects(LoRAMacAddr, macaddress, PANID, MODE, VERSION, gatewayid);

        // MQTTDataItem * mqttitem = new MQTTDataItem("LoRaDeviceBOOT", projectid, LoRAMacAddr, macaddress, PANID, MODE, VERSION);
        // glqueue->add(mqttitem);
    }
    else
    if (strcmp(type, "H") == 0)
    {
        std::string HeartData = "";
        HeartData = JSONLora["D"].asString();

        if ( JSONLora.isMember("F") )
        {
            char hexmacaddress[256];
            memset(hexmacaddress, 0, 256);
            strncpy(hexmacaddress, JSONLora["F"].asCString(), 256);
            vilslog::printf("do_parse_data() Heart hexmacaddress[%s]\n", hexmacaddress);

            send_Tag_Heart_to_projects(LoRAMacAddr, hexmacaddress, HeartData);
        }

        send_Heart_to_projects(LoRAMacAddr, HeartData, gatewayid);

        //MQTTDataItem * mqttitem = new MQTTDataItem("Heart", projectid, LoRAMacAddr, HeartData);
        //glqueue->add(mqttitem);
    }
    else
    if (strcmp(type, "Weight") == 0)
    {
        std::string WeightData = "";
        WeightData = JSONLora["D"].asString();

        if ( JSONLora.isMember("F") )
        {
            char hexmacaddress[256];
            memset(hexmacaddress, 0, 256);
            strncpy(hexmacaddress, JSONLora["F"].asCString(), 256);
            vilslog::printf("do_parse_data() Weight hexmacaddress[%s]\n", hexmacaddress);

            send_Tag_Weight_to_projects(LoRAMacAddr, hexmacaddress, WeightData);
        }

        send_Weight_to_projects(LoRAMacAddr, WeightData, gatewayid);

        //MQTTDataItem * mqttitem = new MQTTDataItem("Weight", projectid, LoRAMacAddr, WeightData);
        //glqueue->add(mqttitem);
    }
    else
    if (strcmp(type, "POWEROFF") == 0)
    {
        if ( JSONLora.isMember("F") )
        {
            strncpy(macaddress, JSONLora["F"].asCString(), 256);
        }

        send_LoRaDevicePOWEROFF_to_projects(macaddress, gatewayid);
    }
    else
    if (strcmp(type, "POWERON") == 0)
    {
        if ( JSONLora.isMember("F") )
        {
            strncpy(macaddress, JSONLora["F"].asCString(), 256);
        }

        send_LoRaDevicePOWERON_to_projects(macaddress, gatewayid);
    }
    else
    if (strcmp(type, "OPENDOOR") == 0)
    {
        if ( JSONLora.isMember("F") )
        {
            strncpy(macaddress, JSONLora["F"].asCString(), 256);
        }

        send_OPENDOOR_to_projects(LoRAMacAddr, macaddress, gatewayid, timestamp);
    }
    else
    if (strcmp(type, "CLOSEDOOR") == 0)
    {
        send_CLOSEDOOR_to_projects(LoRAMacAddr, gatewayid, timestamp);
    }
    else
    if (strcmp(type, "GPIO_ON") == 0)
    {
        send_GPIO_ON_to_projects(LoRAMacAddr, gatewayid, timestamp);
    }
    else
    if (strcmp(type, "AOAfence") == 0)
    {
        char lv[256];
        char time[256];
        memset(lv, 0, 256);
        memset(time, 0, 256);

        if ( JSONLora.isMember("mac") )
        {
            strncpy(macaddress, JSONLora["mac"].asCString(), 256);
        }
        if ( JSONLora.isMember("lv") )
        {
            strncpy(lv, JSONLora["lv"].asCString(), 256);
        }
        if ( JSONLora.isMember("time") )
        {
            strncpy(time, JSONLora["time"].asCString(), 256);
        }

        // vilslog::printf("do_parse_data() AOAfence macaddress[%s] lv[%s] time[%s]\n",
        //   macaddress, lv, time);

        send_aoa_class_to_projects(LoRAMacAddr, macaddress, lv, time, gatewayid, rssi, timestamp);
    }
    else
    if (strcmp(type, "sw") == 0)
    {
        // {"tp":"sw","v":"123.29","i":"0.04","w":"5.70","wacc":"12658843.10"}
        std::string v = "";
        std::string i = "";
        std::string w = "";
        std::string wacc = "";

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

        char value[256];
        memset(value, 0, 256);
        sprintf(value, "{\"v\":\"%s\",\"i\":\"%s\",\"w\":\"%s\",\"wacc\":\"%s\"}", v.c_str(), i.c_str(), w.c_str(), wacc.c_str());

        // vilslog::printf("do_parse_data() sw LoRAMacAddr[%s] v[%s] i[%s] w[%s] wacc[%s]\n",
        //     LoRAMacAddr, v.c_str(), i.c_str(), w.c_str(), wacc.c_str());

        send_PowerSwitch_to_projects(LoRAMacAddr, value, gatewayid, rssi, timestamp);
    }
    else
    if (strcmp(type, "beacon") == 0)
    {
        // vilslog::printf("MQTTSubscribeService::do_parse_data() beacon\n");
        // [{"type":"beacon","minor":"2","mac":"00B0123B6A1AC2CE","data":"153:153"}]
        // "mac":"00B0F424EC629EFC"
        //  00B0+(beacon上預設mac address)

        std::string minor = "0";
        std::string major = "0";
        std::string data = "";
        std::string mac = "";

        if ( JSONLora.isMember("minor") )
            minor = JSONLora["minor"].asString();
        if ( JSONLora.isMember("major") )
            major = JSONLora["major"].asString();
        if ( JSONLora.isMember("data") )
            data = JSONLora["data"].asString();
        if ( JSONLora.isMember("mac") )
            mac = JSONLora["mac"].asString();

        char value[256];
        memset(value, 0, 256);
        sprintf(value, "{\"major\":\"%s\",\"minor\":\"%s\",\"mac\":\"%s\",\"data\":\"%s\"}", major.c_str(), minor.c_str(), mac.c_str(), data.c_str());

        // vilslog::printf("MQTTSubscribeService::do_parse_data() beacon mac[%s]\n", mac.c_str());

        send_beacon_to_projects(LoRAMacAddr, mac.c_str(), value, gatewayid, rssi, timestamp);
    }


    // else
    // if (strcmp(type, "Beacon") == 0)
    // {
    //     vilslog::printf("MQTTSubscribeService::do_parse_data() Beacon\n");
    //
    //     char mac[256];
    //     char distance[256];
    //     char voltage[256];
    //     memset(mac, 0, 256);
    //     memset(distance, 0, 256);
    //     memset(voltage, 0, 256);
    //
    //     // {"Tp":"Beacon","mac":"00B0F424EC629EFC","D","32"}
    //     // 00B0+(beacon上預設mac address)
    //     if ( JSONLora.isMember("mac") )
    //     {
    //         strncpy(mac, JSONLora["mac"].asCString(), 256);
    //     }
    //     if ( JSONLora.isMember("D") )
    //     {
    //         strncpy(distance, JSONLora["D"].asCString(), 256);
    //     }
    //
    //     int ranging = atoi(distance);
    //
    //     send_ranging_to_projects(LoRAMacAddr, macaddress, ranging, voltage, gatewayid, rssi, timestamp);
    // }

    return 0;
}

int do_parse_data(int projectid, const char * LoRAMacAddr, const char * jsondataASCII, const char * gatewayid, int rssi, const char * timestamp)
{
    //"data":"7b225470223a2245222c2246223a2233313330303030303030303030303231227d0d0a"

    // char jsondata[512];
    // int dataCount = 0;
    // int length = (int)strlen(jsondataASCII);
    //
    // memset(jsondata, 0, 512);
    //
    // for(int i=0; i<length;i+=2)
    // {
    //     char dataHex[3];
    //     dataHex[0] = jsondataASCII[i + 0];
    //     dataHex[1] = jsondataASCII[i + 1];
    //     dataHex[2] = '\0';
    //     jsondata[dataCount++] = strtol(dataHex, NULL, 16);
    //     //printf ("dataHex[%s] -> %c\n", dataHex, strtol(dataHex, NULL, 16));
    // }

    char jsondata[512];
    memset(jsondata, 0, 512);
    std::string jsonString = HEXStringToString(jsondataASCII);

    snprintf(jsondata, 512, "%s", jsonString.c_str());

#ifdef PRINT_MQTT_SUB_MSG
    vilslog::printf ("do_parse_data() LoRAMacAddr[%s] jsondata[%s] jsondataASCII[%s]\n", LoRAMacAddr, jsondata, jsondataASCII);
#endif
    if ( strlen(jsondata) < 10)
    {
        // vilslog::printf ("do_parse_data() LoRAMacAddr[%s] jsondata[%s] jsondataASCII[%s]\n", LoRAMacAddr, jsondata, jsondataASCII);

        // if ( strlen(jsondata) == 1 && strcmp(jsondata, "0") == 0)
        // {
        //     //
        //     // Locator bug , 員工卡片按下取消鈕, 只傳 "0"
        //     //
        //     send_cancel_to_projects(LoRAMacAddr, "0000000000000000", gatewayid, timestamp);
        //
        //     //MQTTDataItem * mqttitem = new MQTTDataItem("cancel", projectid, LoRAMacAddr, "0000000000000000", gatewayid, timestamp);
        //     //glqueue->add(mqttitem);
        // }
        // else
        // {
            //vilslog::printf ("Not Valid Data!\n");

            // vilslog::printf ("MQTTSubscribeService::do_parse_data() gatewayid[%s] LoRAMacAddr[%s] jsondata[%s]\n",
            //     gatewayid, LoRAMacAddr, jsondata);

            send_message_to_projects(gatewayid, LoRAMacAddr, jsondata);

            return 1;
        // }
    }
    //printf ("jsondata[%s]\n", jsondata);
    // "Tp":"E" alert data
    // "F":"MAC ADDRESS"

    // "Tp":"D" ranging data
    // "F":"MAC ADDRESS"
    // "D":"DISTANCE" ranging distance

    // alive訊息 {"Tp":"ALIVE"}
    // {"alive":"5252000000050002"}

    // {"Tp":"POWEROFF","F":"0530000000000166"}

    // 開機訊息 {"Tp":"S","P":"0903","M":"1","V":"5.0.9"}
    // M=locator模式 1.累積傳送 2.循環 3.一般
    // V=firmware version
    Json::Value JSONLora;
    Json::Reader * readerData = new Json::Reader();
    bool parsingSuccessful = readerData->parse( jsondata, JSONLora);
    delete readerData;

    if (parsingSuccessful)
    {
        do_parse_data(projectid, LoRAMacAddr, JSONLora, gatewayid, rssi, timestamp);
    }
    else
    {
        // vilslog::printf ("MQTTSubscribeService::do_parse_data() parsingSuccessful[%d] gatewayid[%s] LoRAMacAddr[%s] jsondata[%s]\n",
        //     parsingSuccessful, gatewayid, LoRAMacAddr, jsondata);

        send_message_to_projects(gatewayid, LoRAMacAddr, jsondata);
    }

    return 0;
}

//
//  28
//  0410000000000001001000130019002100240033ff
//  03100000000000
//  2b442a
//  10e10b10a11912110210b1472000000000000000000000effeeefec02900000000
//
int do_parse_compress_data(int projectid, const char * LoRAMacAddr, const char * data, const char * gatewayid, int rssi, const char * timestamp)
{
    std::string inputData = data;

    std::string rawData = "";
    std::string dataStartPattern("28"); // '('
    std::string::size_type dataStartPos = inputData.find(dataStartPattern);
    if (dataStartPos != std::string::npos)
    {
        rawData = inputData.substr(dataStartPos);
        dataStartPos = 0;
    }
    else
    {
        return 1;
    }

    // vilslog::printf("do_parse_compress_data() rawData[%s]\n", rawData.c_str());

    std::string macEndPattern("ff");
    std::string::size_type ftMacStIdx = dataStartPos + 2;
    std::string::size_type ftMacEndIdx = rawData.find(macEndPattern);

    if (ftMacEndIdx == std::string::npos)
        return 1;

    std::vector<std::string::size_type> macStIdx;
    std::vector<std::string::size_type> macEndIdx;

    macStIdx.push_back(ftMacStIdx);
    macEndIdx.push_back(ftMacEndIdx);

    int commonMacCnt = 1;
    std::string::size_type start_pos = macEndIdx[commonMacCnt - 1] + 2;
    std::string nextValue = rawData.substr(start_pos, 2);
    //printf("start_pos[%lu] nextValue[%s]\n", start_pos, nextValue.c_str());

    while (nextValue.compare("2b") != 0)// 0x2b = '+'
    {
        start_pos = macEndIdx[commonMacCnt - 1] + 2;
        macStIdx.push_back(start_pos);
        commonMacCnt += 1;
        std::string::size_type tmpEndIdx = rawData.find(macEndPattern, start_pos);

        if (tmpEndIdx == std::string::npos)
            return 1;

        macEndIdx.push_back(tmpEndIdx);

        //printf("tag[%s]\n", rawData.substr(start_pos + (5*2), tmpEndIdx - (start_pos + (5*2))).c_str());

        start_pos = macEndIdx[commonMacCnt - 1] + 2;
        nextValue = rawData.substr(start_pos, 2);
        //printf("start_pos[%lu] nextValue[%s]\n", start_pos, nextValue.c_str());
    }

    int tagAmount = 0;
    for(int i=0;i<commonMacCnt;i++)
    {
        std::string::size_type start_idx = macStIdx[i];
        std::string::size_type end_idx = macEndIdx[i];
        std::string tagValue = rawData.substr(start_idx, end_idx - start_idx);
        int tagCnt = (int)(tagValue.length()/2 - 8)/2;
        tagAmount += 1;
        tagAmount += tagCnt;
        //printf("i[%d] start_idx[%lu] end_idx[%lu] tagValue[%s] tagAmount[%d]\n", i, start_idx, end_idx, tagValue.c_str(), tagAmount);
    }

    int distanceStIdx = (int)macEndIdx[commonMacCnt - 1] + (4*2);
    int distanceEndIdx = distanceStIdx + (int)(ceil((float)(tagAmount*12)/8.0)*2);
    // std::string distanceList = rawData.substr(distanceStIdx, distanceEndIdx - distanceStIdx);
    // int distanceOffset = 0;
	// if((tagAmount*12)%8 > 0)
    // {
    //     distanceOffset = 4;
    // }
    // int64_t distanceIntValue = parseInt(distanceList, 16);
    //printf("distanceStIdx[%d] distanceEndIdx[%d]\n", distanceStIdx, distanceEndIdx);
    //vilslog::printf("do_parse_compress_data() distanceList[%s] distanceIntValue[%lld] distanceOffset[%d]\n", distanceList.c_str(), distanceIntValue, distanceOffset);

    int velStIdx = distanceEndIdx;
    int velEndIdx = velStIdx + ((tagAmount * 1) * 2);
    std::string velList = rawData.substr(velStIdx, velEndIdx - velStIdx);

    //printf("imuStIdx[%d] imuEndIdx[%d]\n", imuStIdx, imuEndIdx);
    //vilslog::printf("imuList[%s]\n", imuList.c_str());

    int voltageStIdx = velEndIdx;
    //int voltageEndIdx = voltageStIdx + (int)(ceil((float)(tagAmount*4)/8.0)*2);
    // std::string voltageList = rawData.substr(voltageStIdx, voltageEndIdx - voltageStIdx);
    // int64_t voltageIntValue = parseInt(voltageList, 16);

    //printf("voltageStIdx[%d] voltageEndIdx[%d]\n", voltageStIdx, voltageEndIdx);
    // vilslog::printf("voltageList[%s] voltageIntValue[%lld]\n", voltageList.c_str(), voltageIntValue);

    // vilslog::printf("tagAmount:%d\n", tagAmount);

    int tagIdx = 0;
    for(int i=0;i<commonMacCnt;i++)
    {
        std::string::size_type start_idx = macStIdx[i];
        std::string::size_type end_idx = macEndIdx[i];
        std::string tagValue = rawData.substr(start_idx, end_idx - start_idx);
        int tagCnt = (int)(tagValue.length()/2 - 8)/2;
        tagCnt++;

        std::string tagPrefixMac = rawData.substr(start_idx, 6*2);
        //printf("i[%d] tagCnt[%d] tagValue[%s] tagPrefixMac[%s]\n", i, tagCnt, tagValue.c_str(), tagPrefixMac.c_str());

        for(int j=0;j<tagCnt;j++)
        {
            std::string tagMacaddress = tagPrefixMac;
            tagMacaddress.append(rawData.substr(start_idx + (6*2) + (4*j), 4));
#ifdef PRINT_MQTT_SUB_MSG
            vilslog::printf("do_parse_compress_data() [%d] tagmac[%s]", tagIdx, tagMacaddress.c_str());
#endif
            // int distanceShiftBit = ((tagAmount - tagIdx - 1)*12) + distanceOffset;
            // int distance = (distanceIntValue >> distanceShiftBit) & 0xfff;

            std::string distanceStr = rawData.substr(distanceStIdx + tagIdx*3, 3);
            int distance = (int)parseInt(distanceStr, 16);

#ifdef PRINT_MQTT_SUB_MSG
            vilslog::printf(" distance[%d]", distance);
#endif
            // int voltageShiftBit = (tagAmount - tagIdx - 1) * 4 + distanceOffset;
            // int voltageInt = ((voltageIntValue >> voltageShiftBit) & 0xf)+29;
            // float voltageFloat = (float)(voltageInt)/10.0;

            std::string voltageStr = rawData.substr(voltageStIdx + tagIdx*1, 1);
            int voltageInt = (int)parseInt(voltageStr, 16)+29;
            float voltageFloat = (float)(voltageInt)/10.0;

            char voltage[256];
            memset(voltage, 0, 256);
            sprintf(voltage, "%.01f", voltageFloat);

#ifdef PRINT_MQTT_SUB_MSG
            vilslog::printf(" voltage[%s]", voltage);
#endif
            std::string velValue = rawData.substr(velStIdx + (tagIdx*2), 2);
            int velocity = (int)strtol(velValue.c_str(), NULL, 16);

#ifdef PRINT_MQTT_SUB_MSG
            vilslog::printf(" velocity[%d]", velocity);
            vilslog::printf(" timestamp[%s]", timestamp);
            vilslog::printf("\n");
#endif
            //
            // send out data
            //
            send_rangingcompress_to_projects(LoRAMacAddr, tagMacaddress.c_str(), distance, voltage, velocity, gatewayid, rssi, timestamp);

            //MQTTDataItem * mqttitem = new MQTTDataItem("rangingcompress", projectid, LoRAMacAddr, tagMacaddress.c_str(), distance, voltage, velocity, gatewayid, rssi, timestamp);
            //glqueue->add(mqttitem);

            tagIdx++;
        }
    }

    return 0;
}

//
// 5b
//  0610000000009900990199029903990499059906990799089909ff
//  2b442a
//  506a81810003fe000101fc0380
//  50690081010103fe00fc030180
//  507100ff0001fd03ff01fffc80
//  506f0001ff00fd00000201fd80
//  50690001010000810002fc0280
//  506c00ff0103ff00ff01ffff80
//  506d0001feff000401fefc0480
//  507300fa03fd04fe03fe810280
//  506f0000fe01fc03ff00810280
//  507100ff00000100fc03fd0380
//  000000000000000000000000000000
//  5d
//
// Tag amount : 10.0
// 0 - 0610000000009900
// 500 N N 106 112 110 110 112 114 108 114     2.9(0 or 2.9)   00
// 1 - 0610000000009901
// 500 105 N 107 109 115 113 113 107 113 115   2.9(0 or 2.9)   00
// 2 - 0610000000009902
// 500 113 113 113 115 111 117 117 119 119 113     2.9(0 or 2.9)   00
// 3 - 0610000000009903
// 500 111 113 113 113 109 109 109 113 115 111     2.9(0 or 2.9)   00
// 4 - 0610000000009904
// 500 105 107 109 109 109 N 109 113 107 111   2.9(0 or 2.9)   00
// 5 - 0610000000009905
// 500 108 108 110 116 116 116 116 118 118 118     2.9(0 or 2.9)   00
// 6 - 0610000000009906
// 500 109 111 109 109 109 117 119 117 111 119     2.9(0 or 2.9)   00
// 7 - 0610000000009907
// 500 115 105 111 107 115 113 119 117 N 121   2.9(0 or 2.9)   00
// 8 - 0610000000009908
// 500 111 111 109 111 105 111 111 111 N 115   2.9(0 or 2.9)   00
// 9 - 0610000000009909
// 500 113 113 113 113 115 115 109 115 111 117     2.9(0 or 2.9)   00
int do_parse_compress_multiple_data(int projectid, const char * LoRAMacAddr, const char * data, const char * gatewayid, int rssi, const char * timestamp)
{
    std::string inputData = data;
    std::string rawData = "";
    std::string dataStartPattern("5b"); // '['
    std::string::size_type dataStartPos = inputData.find(dataStartPattern);
    if (dataStartPos != std::string::npos)
    {
        rawData = inputData.substr(dataStartPos);
        dataStartPos = 0;
    }
    else
    {
        return 1;
    }

#ifdef PRINT_MQTT_compress_multiple_data
    // vilslog::printf("do_parse_compress_multiple_data() rawData[%s]\n", rawData.c_str());
#endif

    std::string macEndPattern("ff");
    std::string::size_type ftMacStIdx = dataStartPos + 2;
    std::string::size_type ftMacEndIdx = rawData.find(macEndPattern);

    if (ftMacEndIdx == std::string::npos)
    {
        vilslog::printf("do_parse_compress_multiple_data() ERROR! ftMacEndIdx == std::string::npos\n");
        return 1;
    }

    std::vector<std::string::size_type> macStIdx;
    std::vector<std::string::size_type> macEndIdx;

    macStIdx.push_back(ftMacStIdx);
    macEndIdx.push_back(ftMacEndIdx);

    int commonMacCnt = 1;
    std::string::size_type start_pos = macEndIdx[commonMacCnt - 1] + 2;
    std::string nextValue = rawData.substr(start_pos, 2);

#ifdef PRINT_MQTT_compress_multiple_data
    // vilslog::printf("do_parse_compress_multiple_data() start_pos[%lu] nextValue[%s]\n", start_pos, nextValue.c_str());
#endif
    while (nextValue.compare("2b") != 0)// 0x2b = '+'
    {
        start_pos = macEndIdx[commonMacCnt - 1] + 2;
        macStIdx.push_back(start_pos);
        commonMacCnt += 1;
        std::string::size_type tmpEndIdx = rawData.find(macEndPattern, start_pos);

        if (tmpEndIdx == std::string::npos)
            return 1;

        macEndIdx.push_back(tmpEndIdx);

#ifdef PRINT_MQTT_compress_multiple_data
        // vilslog::printf("do_parse_compress_multiple_data() tag[%s]\n", rawData.substr(start_pos + (5*2), tmpEndIdx - (start_pos + (5*2))).c_str());
#endif
        start_pos = macEndIdx[commonMacCnt - 1] + 2;
        nextValue = rawData.substr(start_pos, 2);

        // vilslog::printf("do_parse_compress_multiple_data() start_pos[%lu] nextValue[%s]\n", start_pos, nextValue.c_str());
    }

    int tagAmount = 0;
    for(int i=0;i<commonMacCnt;i++)
    {
        std::string::size_type start_idx = macStIdx[i];
        std::string::size_type end_idx = macEndIdx[i];
        std::string tagValue = rawData.substr(start_idx, end_idx - start_idx);
        int tagCnt = (int)(tagValue.length()/2 - 8)/2;
        tagAmount += 1;
        tagAmount += tagCnt;
#ifdef PRINT_MQTT_compress_multiple_data
        // vilslog::printf("do_parse_compress_multiple_data() i[%d] start_idx[%lu] end_idx[%lu] tagValue[%s] tagAmount[%d]\n",
        //     i, start_idx, end_idx, tagValue.c_str(), tagAmount);
#endif
    }

    int distanceStIdx = (int)macEndIdx[commonMacCnt - 1] + (4*2);
    int lastEndIdx = distanceStIdx;

#ifdef PRINT_MQTT_compress_multiple_data
    // vilslog::printf("do_parse_compress_multiple_data() distanceStIdx[%d] lastEndIdx[%d]\n", distanceStIdx, lastEndIdx);
#endif

    //int lastValidIdx = 0;
    std::list<LoRaCompressMultipleDistanceStruct *> tagDataList;
    int tagIdx = 0;
    for(int i=0;i<commonMacCnt;i++)
    {
        std::string::size_type start_idx = macStIdx[i];
        std::string::size_type end_idx = macEndIdx[i];
        std::string tagValue = rawData.substr(start_idx, end_idx - start_idx);
        int tagCnt = (int)(tagValue.length()/2 - 8)/2;
        tagCnt++;

        std::string tagPrefixMac = rawData.substr(start_idx, 6*2);

#ifdef PRINT_MQTT_compress_multiple_data
        // vilslog::printf("do_parse_compress_multiple_data() i[%d] tagCnt[%d] tagValue[%s] tagPrefixMac[%s]\n", i, tagCnt, tagValue.c_str(), tagPrefixMac.c_str());
#endif

        for(int j=0;j<tagCnt;j++)
        {
            std::string tagMacaddress = tagPrefixMac;
            tagMacaddress.append(rawData.substr(start_idx + (6*2) + (4*j), 4));

#ifdef PRINT_MQTT_compress_multiple_data
            // vilslog::printf("do_parse_compress_multiple_data() [%d] tagmac[%s]\n", tagIdx, tagMacaddress.c_str());
#endif
            LoRaCompressMultipleDistanceStruct * newTagData = new LoRaCompressMultipleDistanceStruct();
            newTagData->macaddress = tagMacaddress;
            tagDataList.push_back(newTagData);

            //
            // find string '506a81810003fe000101fc0380'
            //
            std::string::size_type currentEndIdx = rawData.find("80", lastEndIdx+4);
            if ( currentEndIdx == std::string::npos )
            {
                vilslog::printf("do_parse_compress_multiple_data() find(80) npos \n");
                continue;
            }
            std::string curTagDistanceStr = rawData.substr(lastEndIdx, currentEndIdx - lastEndIdx + 2); // include '80'

#ifdef PRINT_MQTT_compress_multiple_data
            // vilslog::printf("do_parse_compress_multiple_data() currentEndIdx[%d] curTagDistanceStr[%s]\n",
            //     currentEndIdx, curTagDistanceStr.c_str());
#endif

            std::string distIntervalStr = curTagDistanceStr.substr(0, 1);
            int distInterval = (int)parseInt(distIntervalStr, 16) * 100;// * 100 ms
            newTagData->distInterval = distInterval;

            std::string baseDistStr = curTagDistanceStr.substr(1, 3);
            int baseDist = (int)parseInt(baseDistStr, 16);
            newTagData->baseDist = baseDist;

#ifdef PRINT_MQTT_compress_multiple_data
            // vilslog::printf("do_parse_compress_multiple_data() distInterval[%d ms] baseDist[%d cm]\n", distInterval, baseDist);
#endif
            std::string::size_type dist_start_pos = 4;// skip distInterval and baseDist
            std::string dist_cur_Value = curTagDistanceStr.substr(dist_start_pos, 2);

            int pre_distance = baseDist;
#ifdef PRINT_MQTT_compress_multiple_data
            // vilslog::printf("do_parse_compress_multiple_data() distance\n");
#endif
            while (dist_cur_Value.compare("80") != 0 && dist_start_pos < (curTagDistanceStr.length() - 2))
            {
                if ( dist_cur_Value.compare("81") == 0 )
                {
#ifdef PRINT_MQTT_compress_multiple_data
                    // vilslog::printf("[N][%d]\n", -1);
#endif
                    newTagData->distList.push_back(-1);
                }
                else
                {
                    int distance = (int)parseInt(dist_cur_Value, 16);
                    if ( distance >= 128 )
                        distance -= 255;

                    int true_distance = pre_distance + (distance * 2);
#ifdef PRINT_MQTT_compress_multiple_data
                    // vilslog::printf("[%d][%d]\n", distance, true_distance);
#endif
                    newTagData->distList.push_back(true_distance);
                    pre_distance = true_distance;
                }
                dist_start_pos += 2;
                dist_cur_Value = curTagDistanceStr.substr(dist_start_pos, 2);
            }
#ifdef PRINT_MQTT_compress_multiple_data
            // vilslog::printf("\n");
            // vilslog::printf("do_parse_compress_multiple_data() newTagData->distList.size()[%d]\n", newTagData->distList.size());
#endif
            lastEndIdx = (int)currentEndIdx + 2;
            tagIdx++;
        }
    }

    int velStIdx = lastEndIdx;
    int velEndIdx = velStIdx + ((tagAmount * 1) * 2);
    std::string velStr = rawData.substr(velStIdx, velEndIdx - velStIdx);

#ifdef PRINT_MQTT_compress_multiple_data
    // vilslog::printf("velStIdx[%d] velEndIdx[%d]\n", velStIdx, velEndIdx);
    // vilslog::printf("velStr[%s]\n", velStr.c_str());
#endif

    int voltageStIdx = velEndIdx;
    int voltageEndIdx = voltageStIdx + tagAmount;
    std::string voltageStr = rawData.substr(voltageStIdx, voltageEndIdx - voltageStIdx);

#ifdef PRINT_MQTT_compress_multiple_data
    // printf("voltageStIdx[%d] voltageStr[%s]\n", voltageStIdx, voltageStr.c_str());
#endif

    tagIdx = 0;
    for (std::list<LoRaCompressMultipleDistanceStruct*>::iterator i = tagDataList.begin(); i != tagDataList.end(); i++)
    {
        LoRaCompressMultipleDistanceStruct * curTagData = (*i);

        std::string volValue = voltageStr.substr(tagIdx, 1);
        int voltageInt = (int)parseInt(volValue, 16) + 29;
        float voltageFloat = (float)(voltageInt)/10.0;
        char voltage[256];
        memset(voltage, 0, 256);
        sprintf(voltage, "%.01f", voltageFloat);

        std::string velValue = velStr.substr(tagIdx*2, 2);
        int velocity = (int)strtol(velValue.c_str(), NULL, 16);

#ifdef PRINT_MQTT_compress_multiple_data
        // printf("macaddress[%s] voltageFloat[%f] velocity[%d]\n", curTagData->macaddress.c_str(), voltageFloat, velocity);
#endif

        tagIdx++;

        //
        // send out data
        //
        send_rangingcompressMultiDistance_to_projects(LoRAMacAddr, curTagData->macaddress.c_str(), curTagData->distInterval, &curTagData->distList, voltage, velocity, gatewayid, rssi, timestamp);

        //MQTTDataItem * mqttitem = new MQTTDataItem("rangingcompressMultiDistance", projectid, LoRAMacAddr, curTagData->macaddress.c_str(), curTagData->distInterval, &curTagData->distList, voltage, velocity, gatewayid, rssi, timestamp);
        //glqueue->add(mqttitem);
    }

    clear_LoRaCompressMultipleDistanceStruct_list(&tagDataList);
    tagDataList.clear();

    return 0;
}

//
// 3c0410000000000320ff2b442a080000d02b412a8b3ecd
// 0 - 0410000000000320    128 0.11    4.2             00
//
// 3c4210000000000138ff2b442a001000002b412a00b83e
//
int do_parse_compress_AOA_data(int projectid, const char * LoRAMacAddr, const char * data, const char * gatewayid, int rssi, const char * timestamp)
{
    std::string inputData = data;
    std::string rawData = "";
    std::string dataStartPattern("3c"); // '<'
    std::string::size_type dataStartPos = inputData.find(dataStartPattern);
    if (dataStartPos != std::string::npos)
    {
        rawData = inputData.substr(dataStartPos);
        dataStartPos = 0;
    }
    else
    {
        return 1;
    }

#ifdef PRINT_MQTT_compress_AOA_data
    vilslog::printf("do_parse_compress_AOA_data() rawData[%s]\n", rawData.c_str());
#endif

    std::string macEndPattern("ff");
    std::string::size_type ftMacStIdx = dataStartPos + 2;
    std::string::size_type ftMacEndIdx = rawData.find(macEndPattern);

    if (ftMacEndIdx == std::string::npos)
    {
        vilslog::printf("do_parse_compress_AOA_data() ERROR! ftMacEndIdx == std::string::npos\n");
        return 1;
    }

    std::vector<std::string::size_type> macStIdx;
    std::vector<std::string::size_type> macEndIdx;

    macStIdx.push_back(ftMacStIdx);
    macEndIdx.push_back(ftMacEndIdx);

    int commonMacCnt = 1;
    std::string::size_type start_pos = macEndIdx[commonMacCnt - 1] + 2;
    std::string nextValue = rawData.substr(start_pos, 2);

#ifdef PRINT_MQTT_compress_AOA_data
    // vilslog::printf("do_parse_compress_AOA_data() start_pos[%lu] nextValue[%s]\n", start_pos, nextValue.c_str());
#endif
    while (nextValue.compare("2b") != 0)// 0x2b = '+'
    {
        start_pos = macEndIdx[commonMacCnt - 1] + 2;
        macStIdx.push_back(start_pos);
        commonMacCnt += 1;
        std::string::size_type tmpEndIdx = rawData.find(macEndPattern, start_pos);

        if (tmpEndIdx == std::string::npos)
            return 1;

        macEndIdx.push_back(tmpEndIdx);

#ifdef PRINT_MQTT_compress_AOA_data
        vilslog::printf("do_parse_compress_AOA_data() tag[%s]\n", rawData.substr(start_pos + (5*2), tmpEndIdx - (start_pos + (5*2))).c_str());
#endif
        start_pos = macEndIdx[commonMacCnt - 1] + 2;
        nextValue = rawData.substr(start_pos, 2);

        // vilslog::printf("do_parse_compress_AOA_data() start_pos[%lu] nextValue[%s]\n", start_pos, nextValue.c_str());
    }

    int tagAmount = 0;
    for(int i=0;i<commonMacCnt;i++)
    {
        std::string::size_type start_idx = macStIdx[i];
        std::string::size_type end_idx = macEndIdx[i];
        std::string tagValue = rawData.substr(start_idx, end_idx - start_idx);
        int tagCnt = (int)(tagValue.length()/2 - 8)/2;
        tagAmount += 1;
        tagAmount += tagCnt;
#ifdef PRINT_MQTT_compress_AOA_data
        vilslog::printf("do_parse_compress_AOA_data() i[%d] start_idx[%lu] end_idx[%lu] tagValue[%s] tagAmount[%d]\n",
            i, start_idx, end_idx, tagValue.c_str(), tagAmount);
#endif
    }

    int distanceStIdx = (int)macEndIdx[commonMacCnt - 1] + (4*2);
    int distanceEndIdx = distanceStIdx + (int)(ceil((float)(tagAmount*12)/8.0)*2);
    std::string distanceStr = rawData.substr(distanceStIdx, distanceEndIdx - distanceStIdx);

#ifdef PRINT_MQTT_compress_AOA_data
    vilslog::printf("do_parse_compress_AOA_data() distanceStIdx[%d] distanceEndIdx[%d] distanceStr[%s]\n", distanceStIdx, distanceEndIdx, distanceStr.c_str());
#endif

    int velStIdx = distanceEndIdx;
    int velEndIdx = velStIdx + ((tagAmount * 1) * 2);
    std::string velList = rawData.substr(velStIdx, velEndIdx - velStIdx);

#ifdef PRINT_MQTT_compress_AOA_data
    vilslog::printf("do_parse_compress_AOA_data() velStIdx[%d] velEndIdx[%d] velList[%s]\n", velStIdx, velEndIdx, velList.c_str());
#endif

    int voltageStIdx = velEndIdx;
    int voltageEndIdx = voltageStIdx + (int)(ceil((float)(tagAmount*4)/8.0)*2);
    std::string voltageStr = rawData.substr(voltageStIdx, voltageEndIdx - voltageStIdx);
    //int voltageInt = (int)parseInt(voltageStr, 16)+29;
    //float voltageFloat = (float)(voltageInt)/10.0;

#ifdef PRINT_MQTT_compress_AOA_data
    vilslog::printf("do_parse_compress_AOA_data() voltageStIdx[%d] voltageEndIdx[%d] velList[%s]\n", voltageStIdx, voltageEndIdx, voltageStr.c_str());
#endif

    int angleStIdx = voltageEndIdx + (3*2);
    int angleEndIdx = angleStIdx + ((tagAmount * 2) * 2);
    std::string angleStr = rawData.substr(angleStIdx, angleEndIdx - angleStIdx);

#ifdef PRINT_MQTT_compress_AOA_data
    vilslog::printf("do_parse_compress_AOA_data() angleStIdx[%d] angleEndIdx[%d] angleStr[%s]\n", angleStIdx, angleEndIdx, angleStr.c_str());
#endif

    int tagIdx = 0;
    for(int i=0;i<commonMacCnt;i++)
    {
        std::string::size_type start_idx = macStIdx[i];
        std::string::size_type end_idx = macEndIdx[i];
        std::string tagValue = rawData.substr(start_idx, end_idx - start_idx);
        int tagCnt = (int)(tagValue.length()/2 - 8)/2;
        tagCnt++;

        std::string tagPrefixMac = rawData.substr(start_idx, 6*2);

#ifdef PRINT_MQTT_compress_AOA_data
        vilslog::printf("do_parse_compress_AOA_data() i[%d] tagCnt[%d] tagValue[%s] tagPrefixMac[%s]\n", i, tagCnt, tagValue.c_str(), tagPrefixMac.c_str());
#endif

        for(int j=0;j<tagCnt;j++)
        {
            std::string tagMacaddress = tagPrefixMac;
            tagMacaddress.append(rawData.substr(start_idx + (6*2) + (4*j), 4));

#ifdef PRINT_MQTT_compress_AOA_data
            vilslog::printf("do_parse_compress_AOA_data() [%d] tagmac[%s]\n", tagIdx, tagMacaddress.c_str());
#endif

            std::string distanceStr = rawData.substr(distanceStIdx + tagIdx*3, 3);
            int distance = (int)parseInt(distanceStr, 16);

#ifdef PRINT_MQTT_compress_AOA_data
            vilslog::printf(" distance[%d]", distance);
#endif

            std::string voltageStr = rawData.substr(voltageStIdx + tagIdx*1, 1);
            int voltageInt = (int)parseInt(voltageStr, 16)+29;
            float voltageFloat = (float)(voltageInt)/10.0;

            char voltage[256];
            memset(voltage, 0, 256);
            sprintf(voltage, "%.01f", voltageFloat);

#ifdef PRINT_MQTT_compress_AOA_data
            vilslog::printf(" voltage[%s]", voltage);
#endif

            std::string velValue = rawData.substr(velStIdx + (tagIdx*2), 2);
            int velocity = (int)strtol(velValue.c_str(), NULL, 16);

#ifdef PRINT_MQTT_compress_AOA_data
            vilslog::printf(" velocity[%d]", velocity);
#endif

            std::string angleStr = rawData.substr(angleStIdx + (tagIdx*4), 4);
            int angleRawValue = (int)strtol(angleStr.c_str(), NULL, 16);
            angleRawValue -= 157;
            float angleFloat = (float)(angleRawValue)/100.0;
            // float angleFloat = (float)(angleRawValue);

#ifdef PRINT_MQTT_compress_AOA_data
            vilslog::printf(" angleStr[%s] angleRawValue[%d] angleFloat[%f]", angleStr.c_str(), angleRawValue, angleFloat);
            vilslog::printf(" timestamp[%s]", timestamp);
            vilslog::printf("\n");
#endif

            //
            // send out data
            //
            int IMU[6] = {0};
            send_rangingcompressaoa_to_projects(LoRAMacAddr, tagMacaddress.c_str(), distance, voltage, velocity, angleFloat, gatewayid, rssi, IMU, timestamp);

            // MQTTDataItem * mqttitem = new MQTTDataItem("rangingcompressaoa", projectid, LoRAMacAddr, tagMacaddress.c_str(), distance, voltage, velocity, angleFloat, gatewayid, rssi, timestamp);
            // glqueue->add(mqttitem);

            tagIdx++;
        }
    }

    return 0;
}

//
// 234210000000000313ff2b442a0640fd00fa000700d02b412afe5d2431
// 23 = '#'  24 = '$'
//
int do_parse_compress_AOA_IMU_data(int projectid, const char * LoRAMacAddr, const char * data, const char * gatewayid, int rssi, const char * timestamp)
{
    std::string inputData = data;
    std::string rawData = "";
    std::string dataStartPattern("23"); // '#'
    std::string::size_type dataStartPos = inputData.find(dataStartPattern);
    if (dataStartPos != std::string::npos)
    {
        rawData = inputData.substr(dataStartPos);
        dataStartPos = 0;
    }
    else
    {
        return 1;
    }

#ifdef PRINT_MQTT_compress_AOA_IMU_data
    vilslog::printf("do_parse_compress_AOA_IMU_data() rawData[%s]\n", rawData.c_str());
#endif

    std::string macEndPattern("ff");
    std::string::size_type ftMacStIdx = dataStartPos + 2;
    std::string::size_type ftMacEndIdx = rawData.find(macEndPattern);

    if (ftMacEndIdx == std::string::npos)
    {
        vilslog::printf("do_parse_compress_AOA_IMU_data() ERROR! ftMacEndIdx == std::string::npos\n");
        return 1;
    }

    std::vector<std::string::size_type> macStIdx;
    std::vector<std::string::size_type> macEndIdx;

    macStIdx.push_back(ftMacStIdx);
    macEndIdx.push_back(ftMacEndIdx);

    int commonMacCnt = 1;
    std::string::size_type start_pos = macEndIdx[commonMacCnt - 1] + 2;
    std::string nextValue = rawData.substr(start_pos, 2);

#ifdef PRINT_MQTT_compress_AOA_IMU_data
    // vilslog::printf("do_parse_compress_AOA_IMU_data() start_pos[%lu] nextValue[%s]\n", start_pos, nextValue.c_str());
#endif

    while (nextValue.compare("2b") != 0)// 0x2b = '+'
    {
        start_pos = macEndIdx[commonMacCnt - 1] + 2;
        macStIdx.push_back(start_pos);
        commonMacCnt += 1;
        std::string::size_type tmpEndIdx = rawData.find(macEndPattern, start_pos);

        if (tmpEndIdx == std::string::npos)
            return 1;

        macEndIdx.push_back(tmpEndIdx);

#ifdef PRINT_MQTT_compress_AOA_IMU_data
        // vilslog::printf("do_parse_compress_AOA_IMU_data() tag[%s]\n", rawData.substr(start_pos + (5*2), tmpEndIdx - (start_pos + (5*2))).c_str());
#endif
        start_pos = macEndIdx[commonMacCnt - 1] + 2;
        nextValue = rawData.substr(start_pos, 2);

#ifdef PRINT_MQTT_compress_AOA_IMU_data
        // vilslog::printf("do_parse_compress_AOA_data() start_pos[%lu] nextValue[%s]\n", start_pos, nextValue.c_str());
#endif
    }

    int tagAmount = 0;
    for(int i=0;i<commonMacCnt;i++)
    {
        std::string::size_type start_idx = macStIdx[i];
        std::string::size_type end_idx = macEndIdx[i];
        std::string tagValue = rawData.substr(start_idx, end_idx - start_idx);
        int tagCnt = (int)(tagValue.length()/2 - 8)/2;
        tagAmount += 1;
        tagAmount += tagCnt;
#ifdef PRINT_MQTT_compress_AOA_IMU_data
        vilslog::printf("do_parse_compress_AOA_IMU_data() i[%d] start_idx[%lu] end_idx[%lu] tagValue[%s] tagAmount[%d]\n",
            i, start_idx, end_idx, tagValue.c_str(), tagAmount);
#endif
    }

    int distanceStIdx = (int)macEndIdx[commonMacCnt - 1] + (4*2);
    int distanceEndIdx = distanceStIdx + (int)(ceil((float)(tagAmount*12)/8.0)*2);
    std::string distanceStr = rawData.substr(distanceStIdx, distanceEndIdx - distanceStIdx);

#ifdef PRINT_MQTT_compress_AOA_IMU_data
    vilslog::printf("do_parse_compress_AOA_IMU_data() distanceStIdx[%d] distanceEndIdx[%d] distanceStr[%s]\n", distanceStIdx, distanceEndIdx, distanceStr.c_str());
#endif

    int IMUStIdx = distanceEndIdx;
    int IMUEndIdx = IMUStIdx + ((tagAmount * 1) * ( 2 * 6));
    std::string IMUList = rawData.substr(IMUStIdx, IMUEndIdx - IMUStIdx);

#ifdef PRINT_MQTT_compress_AOA_IMU_data
    vilslog::printf("do_parse_compress_AOA_IMU_data() IMUStIdx[%d] IMUEndIdx[%d] IMUList[%s]\n", IMUStIdx, IMUEndIdx, IMUList.c_str());
#endif

    int voltageStIdx = IMUEndIdx;
    int voltageEndIdx = voltageStIdx + (int)(ceil((float)(tagAmount*4)/8.0)*2);
    std::string voltageStr = rawData.substr(voltageStIdx, voltageEndIdx - voltageStIdx);
    //int voltageInt = (int)parseInt(voltageStr, 16)+29;
    //float voltageFloat = (float)(voltageInt)/10.0;

#ifdef PRINT_MQTT_compress_AOA_IMU_data
    vilslog::printf("do_parse_compress_AOA_IMU_data() voltageStIdx[%d] voltageEndIdx[%d] velList[%s]\n", voltageStIdx, voltageEndIdx, voltageStr.c_str());
#endif

    int angleStIdx = voltageEndIdx + (3*2);
    int angleEndIdx = angleStIdx + ((tagAmount * 2) * 2);
    std::string angleStr = rawData.substr(angleStIdx, angleEndIdx - angleStIdx);

#ifdef PRINT_MQTT_compress_AOA_IMU_data
        vilslog::printf("do_parse_compress_AOA_IMU_data() angleStIdx[%d] angleEndIdx[%d] angleStr[%s]\n", angleStIdx, angleEndIdx, angleStr.c_str());
#endif

    int tagIdx = 0;
    for(int i=0;i<commonMacCnt;i++)
    {
        std::string::size_type start_idx = macStIdx[i];
        std::string::size_type end_idx = macEndIdx[i];
        std::string tagValue = rawData.substr(start_idx, end_idx - start_idx);
        int tagCnt = (int)(tagValue.length()/2 - 8)/2;
        tagCnt++;

        std::string tagPrefixMac = rawData.substr(start_idx, 6*2);

#ifdef PRINT_MQTT_compress_AOA_IMU_data
        vilslog::printf("do_parse_compress_AOA_IMU_data() i[%d] tagCnt[%d] tagValue[%s] tagPrefixMac[%s]\n", i, tagCnt, tagValue.c_str(), tagPrefixMac.c_str());
#endif

        for(int j=0;j<tagCnt;j++)
        {
            std::string tagMacaddress = tagPrefixMac;
            tagMacaddress.append(rawData.substr(start_idx + (6*2) + (4*j), 4));

#ifdef PRINT_MQTT_compress_AOA_IMU_data
            vilslog::printf("do_parse_compress_AOA_IMU_data() [%d] tagmac[%s]\n", tagIdx, tagMacaddress.c_str());
#endif

            std::string distanceStr = rawData.substr(distanceStIdx + tagIdx*3, 3);
            int distance = (int)parseInt(distanceStr, 16);

#ifdef PRINT_MQTT_compress_AOA_IMU_data
            vilslog::printf(" distance[%d]", distance);
#endif

            std::string voltageStr = rawData.substr(voltageStIdx + tagIdx*1, 1);
            int voltageInt = (int)parseInt(voltageStr, 16)+29;
            float voltageFloat = (float)(voltageInt)/10.0;

            char voltage[256];
            memset(voltage, 0, 256);
            sprintf(voltage, "%.01f", voltageFloat);

#ifdef PRINT_MQTT_compress_AOA_IMU_data
            vilslog::printf(" voltage[%s]", voltage);
#endif

            int IMU[6] = {0};
            for(int k=0;k<6;k++)
            {
                // std::string IMUValue = rawData.substr(IMUStIdx + (tagIdx*2), (2*6));
                std::string IMUValue = rawData.substr(IMUStIdx + (tagIdx*1) + k*2, 2);
                IMU[k] = (int)strtol(IMUValue.c_str(), NULL, 16);
            }

#ifdef PRINT_MQTT_compress_AOA_IMU_data
            vilslog::printf(" IMU[%d,%d,%d,%d,%d,%d]", IMU[0], IMU[1], IMU[2], IMU[3], IMU[4], IMU[5]);
#endif

            std::string angleStr = rawData.substr(angleStIdx + (tagIdx*4), 4);
            int angleRawValue = (int)strtol(angleStr.c_str(), NULL, 16);
            angleRawValue -= 157;
            float angleFloat = (float)(angleRawValue)/100.0;
            // float angleFloat = (float)(angleRawValue);

#ifdef PRINT_MQTT_compress_AOA_IMU_data
            vilslog::printf(" angleStr[%s] angleRawValue[%d] angleFloat[%f]", angleStr.c_str(), angleRawValue, angleFloat);
            vilslog::printf(" timestamp[%s]", timestamp);
            vilslog::printf("\n");
#endif

            int velocity = 1;

            //
            // send out data
            //
            send_rangingcompressaoa_to_projects(LoRAMacAddr, tagMacaddress.c_str(), distance, voltage, velocity, angleFloat, gatewayid, rssi, IMU, timestamp);

            tagIdx++;
        }
    }

    return 0;
}

void send_loragateway_to_projects(LoRaGatewayInfoStruct * geteway)
{
    // static time_t previousTime = getCurrentTimeInSec();
    // time_t curTime = getCurrentTimeInSec();
    // int diff = (int)(curTime - previousTime);
    // if ( diff <= 60 )
    // {
    //     return;
    // }
    // previousTime = curTime;

    bool bFound = false;
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        LoRaGatewayInfoStruct * gatwayExisted =  glMQTTDeviceManager->getLoRaGateway(projectid, geteway->gwid);
        if (gatwayExisted != NULL )
        {
            MQTTDataItem * mqttitem = new MQTTDataItem("loragateway", projectid, geteway);
            glqueue->add(mqttitem);
            bFound = true;
        }
    }
    if (!bFound)
    {
        MQTTDataItem * mqttitem = new MQTTDataItem("loragateway", 1, geteway);
        glqueue->add(mqttitem);
    }
}

void send_MQTTgateway_to_projects(std::string gatewayid)
{
    bool bFound = false;
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        LoRaGatewayInfoStruct * gatwayExisted =  glMQTTDeviceManager->getLoRaGateway(projectid, gatewayid);
        if (gatwayExisted != NULL )
        {
            // send MQTTgateway alive
            MQTTDataItem * mqttitem = new MQTTDataItem("MQTTgateway", projectid, gatwayExisted);
            glqueue->add(mqttitem);
            bFound = true;
        }
    }
    if (!bFound)
    {

    }
}

void send_alert_to_projects(const char * LoRAMacAddr, const char * macaddress, const char * gatewayid, const char * timestamp)
{
    std::string gatewayidString = gatewayid;

    bool bFound = false;
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        LoRaGatewayInfoStruct * gatwayExisted =  glMQTTDeviceManager->getLoRaGateway(projectid, gatewayidString);
        if (gatwayExisted != NULL )
        {
            MQTTDataItem * mqttitem = new MQTTDataItem("alert", projectid, LoRAMacAddr, macaddress, gatewayid, timestamp);
            glqueue->add(mqttitem);
            bFound = true;
        }
    }
    if (!bFound)
    {
        MQTTDataItem * mqttitem = new MQTTDataItem("alert", 1, LoRAMacAddr, macaddress, gatewayid, timestamp);
        glqueue->add(mqttitem);
    }
}

void send_cancel_to_projects(const char * LoRAMacAddr, const char * macaddress, const char * gatewayid, const char * timestamp)
{
    std::string gatewayidString = gatewayid;

    bool bFound = false;
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        LoRaGatewayInfoStruct * gatwayExisted =  glMQTTDeviceManager->getLoRaGateway(projectid, gatewayidString);
        if (gatwayExisted != NULL )
        {
            MQTTDataItem * mqttitem = new MQTTDataItem("cancel", projectid, LoRAMacAddr, macaddress, gatewayid, timestamp);
            glqueue->add(mqttitem);
            bFound = true;
        }
    }
    if (!bFound)
    {
        MQTTDataItem * mqttitem = new MQTTDataItem("cancel", 1, LoRAMacAddr, macaddress, gatewayid, timestamp);
        glqueue->add(mqttitem);
    }
}

void send_ranging_to_projects(const char * LoRAMacAddr, const char * macaddress, int ranging, const char * voltage, const char * gatewayid, int rssi, const char * timestamp)
{
    std::string gatewayidString = gatewayid;

    bool bFound = false;
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        LoRaGatewayInfoStruct * gatwayExisted =  glMQTTDeviceManager->getLoRaGateway(projectid, gatewayidString);
        if (gatwayExisted != NULL )
        {
            MQTTDataItem * mqttitem = new MQTTDataItem("ranging", projectid, LoRAMacAddr, macaddress, ranging, voltage, gatewayid, rssi, timestamp);
            glqueue->add(mqttitem);
            bFound = true;
        }
    }
    if (!bFound)
    {
        MQTTDataItem * mqttitem = new MQTTDataItem("ranging", 1, LoRAMacAddr, macaddress, ranging, voltage, gatewayid, rssi, timestamp);
        glqueue->add(mqttitem);
    }
}

void send_gpsinfo_to_projects(const char * type, const char * LoRAMacAddr, const char * macaddress, double Longitude, double Latitude, const char * gatewayid, int rssi, const char * timestamp)
{
    std::string gatewayidString = gatewayid;

    bool bFound = false;
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        LoRaGatewayInfoStruct * gatwayExisted =  glMQTTDeviceManager->getLoRaGateway(projectid, gatewayidString);
        if (gatwayExisted != NULL )
        {
            MQTTDataItem * mqttitem = new MQTTDataItem(type, projectid, LoRAMacAddr, macaddress, Longitude, Latitude, gatewayid, timestamp);
            glqueue->add(mqttitem);
            bFound = true;
        }
    }
    if (!bFound)
    {
        MQTTDataItem * mqttitem = new MQTTDataItem(type, 1, LoRAMacAddr, macaddress, Longitude, Latitude, gatewayid, timestamp);
        glqueue->add(mqttitem);
    }
}

void send_lidarinfo_to_projects(const char * LoRAMacAddr, const char * value, const char * gatewayid, int rssi, const char * timestamp)
{
    std::string gatewayidString = gatewayid;
    std::string valueString = std::string(value);

    bool bFound = false;
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        LoRaGatewayInfoStruct * gatwayExisted =  glMQTTDeviceManager->getLoRaGateway(projectid, gatewayidString);
        if (gatwayExisted != NULL )
        {
            MQTTDataItem * mqttitem = new MQTTDataItem("lidarinfo", projectid, LoRAMacAddr, valueString);
            mqttitem->setGatewayid(gatewayid);
            mqttitem->setRSSI(rssi);
            mqttitem->setTimestamp(timestamp);
            glqueue->add(mqttitem);
            bFound = true;
        }
    }
    if (!bFound)
    {
        MQTTDataItem * mqttitem = new MQTTDataItem("lidarinfo", 1, LoRAMacAddr, valueString);
        mqttitem->setGatewayid(gatewayid);
        mqttitem->setRSSI(rssi);
        mqttitem->setTimestamp(timestamp);
        glqueue->add(mqttitem);
    }
}

void send_LoRaDeviceALIVE_to_projects(const char * LoRAMacAddr, const char * gatewayid, int rssi, int AoAGridSize)
{
    std::string gatewayidString = gatewayid;

    bool bFound = false;
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        LoRaGatewayInfoStruct * gatwayExisted =  glMQTTDeviceManager->getLoRaGateway(projectid, gatewayidString);
        if (gatwayExisted != NULL )
        {
            // send LoRa Device alive
            MQTTDataItem * mqttitem = new MQTTDataItem("LoRaDeviceALIVE", projectid, LoRAMacAddr);
            mqttitem->setGatewayid(gatewayid);
            mqttitem->setRSSI(rssi);
            mqttitem->setAOA_gridsize(AoAGridSize);
            glqueue->add(mqttitem);;
            bFound = true;
        }
    }
    if (!bFound)
    {
        MQTTDataItem * mqttitem = new MQTTDataItem("LoRaDeviceALIVE", 1, LoRAMacAddr);
        mqttitem->setGatewayid(gatewayid);
        mqttitem->setRSSI(rssi);
        mqttitem->setAOA_gridsize(AoAGridSize);
        glqueue->add(mqttitem);
    }
}

void send_LoRaDeviceBOOT_to_projects(const char * LoRAMacAddr, const char * macaddress, std::string PANID, std::string MODE, std::string VERSION, const char * gatewayid)
{
    std::string gatewayidString = gatewayid;

    bool bFound = false;
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        LoRaGatewayInfoStruct * gatwayExisted = glMQTTDeviceManager->getLoRaGateway(projectid, gatewayidString);
        if (gatwayExisted != NULL )
        {
            MQTTDataItem * mqttitem = new MQTTDataItem("LoRaDeviceBOOT", projectid, LoRAMacAddr, macaddress, PANID, MODE, VERSION);
            mqttitem->setGatewayid(gatewayid);
            glqueue->add(mqttitem);
            bFound = true;
        }
    }
    if (!bFound)
    {
        MQTTDataItem * mqttitem = new MQTTDataItem("LoRaDeviceBOOT", 1, LoRAMacAddr, macaddress, PANID, MODE, VERSION);
        mqttitem->setGatewayid(gatewayid);
        glqueue->add(mqttitem);
    }
}

void send_LoRaDevicePOWEROFF_to_projects(const char * macaddress, const char * gatewayid)
{
    std::string gatewayidString = gatewayid;

    bool bFound = false;
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        LoRaGatewayInfoStruct * gatwayExisted =  glMQTTDeviceManager->getLoRaGateway(projectid, gatewayidString);
        if (gatwayExisted != NULL )
        {
            NodeStruct * node = glMQTTDeviceManager->get_Locator_node_ByMacaddress(projectid, macaddress);
            if ( node != NULL )
            {
                MQTTDataItem * mqttitem = new MQTTDataItem("LoRaDevicePOWEROFF", projectid, node->LoRAMacaddress, macaddress);
                mqttitem->setGatewayid(gatewayid);
                glqueue->add(mqttitem);
            }
            bFound = true;
        }
    }
    if (!bFound)
    {
        NodeStruct * node = glMQTTDeviceManager->get_Locator_node_ByMacaddress(1, macaddress);
        if ( node != NULL )
        {
            MQTTDataItem * mqttitem = new MQTTDataItem("LoRaDevicePOWEROFF", 1, node->LoRAMacaddress, macaddress);
            mqttitem->setGatewayid(gatewayid);
            glqueue->add(mqttitem);
        }
    }
}

void send_LoRaDevicePOWERON_to_projects(const char * macaddress, const char * gatewayid)
{
    std::string gatewayidString = gatewayid;

    bool bFound = false;
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        LoRaGatewayInfoStruct * gatwayExisted =  glMQTTDeviceManager->getLoRaGateway(projectid, gatewayidString);
        if (gatwayExisted != NULL )
        {
            NodeStruct * node = glMQTTDeviceManager->get_Locator_node_ByMacaddress(projectid, macaddress);
            if ( node != NULL )
            {
                MQTTDataItem * mqttitem = new MQTTDataItem("LoRaDevicePOWERON", projectid, node->LoRAMacaddress, macaddress);
                mqttitem->setGatewayid(gatewayid);
                glqueue->add(mqttitem);
            }
            bFound = true;
        }
    }
    if (!bFound)
    {
        NodeStruct * node = glMQTTDeviceManager->get_Locator_node_ByMacaddress(1, macaddress);
        if ( node != NULL )
        {
            MQTTDataItem * mqttitem = new MQTTDataItem("LoRaDevicePOWERON", 1, node->LoRAMacaddress, macaddress);
            mqttitem->setGatewayid(gatewayid);
            glqueue->add(mqttitem);
            bFound = true;
        }
    }
    if (!bFound)
    {
        vilslog::printf("send_LoRaDevicePOWERON_to_projects() macaddress[%s] gatewayid[%s]\n", macaddress, gatewayid);
    }
}

void send_NFCLocatorPOWERON_to_projects(const char * LoRAMacAddr, const char * macaddress, const char * gatewayid)
{
    std::string gatewayidString = gatewayid;

    bool bFound = false;
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        LoRaGatewayInfoStruct * gatwayExisted = glMQTTDeviceManager->getLoRaGateway(projectid, gatewayidString);
        if (gatwayExisted != NULL )
        {
            NFCDeviceStruct * nfcDevice = glMQTTDeviceManager->get_NFCDevice(projectid, macaddress);
            if ( nfcDevice != NULL )
            {
                MQTTDataItem * mqttitem = new MQTTDataItem("NFCLocatorPOWERON", projectid, LoRAMacAddr, macaddress);
                mqttitem->setGatewayid(gatewayid);
                glqueue->add(mqttitem);
            }
            bFound = true;
        }
    }
    if (!bFound)
    {
        NFCDeviceStruct * nfcDevice = glMQTTDeviceManager->get_NFCDevice(1, macaddress);
        if ( nfcDevice != NULL )
        {
            MQTTDataItem * mqttitem = new MQTTDataItem("NFCLocatorPOWERON", 1, LoRAMacAddr, macaddress);
            mqttitem->setGatewayid(gatewayid);
            glqueue->add(mqttitem);
        }
    }
}

void send_Heart_to_projects(const char * LoRAMacAddr, std::string HeartData, const char * gatewayid)
{
    std::string gatewayidString = gatewayid;

    bool bFound = false;
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        LoRaGatewayInfoStruct * gatwayExisted =  glMQTTDeviceManager->getLoRaGateway(projectid, gatewayidString);
        if (gatwayExisted != NULL )
        {
            MQTTDataItem * mqttitem = new MQTTDataItem("Heart", projectid, LoRAMacAddr, HeartData);
            mqttitem->setGatewayid(gatewayid);
            glqueue->add(mqttitem);
            bFound = true;
        }
    }
    if (!bFound)
    {
        MQTTDataItem * mqttitem = new MQTTDataItem("Heart", 1, LoRAMacAddr, HeartData);
        mqttitem->setGatewayid(gatewayid);
        glqueue->add(mqttitem);
    }
}

void send_Weight_to_projects(const char * LoRAMacAddr, std::string WeightData, const char * gatewayid)
{
    std::string gatewayidString = gatewayid;

    bool bFound = false;
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        LoRaGatewayInfoStruct * gatwayExisted =  glMQTTDeviceManager->getLoRaGateway(projectid, gatewayidString);
        if (gatwayExisted != NULL )
        {
            MQTTDataItem * mqttitem = new MQTTDataItem("Weight", projectid, LoRAMacAddr, WeightData);
            mqttitem->setGatewayid(gatewayid);
            glqueue->add(mqttitem);
            bFound = true;
        }
    }
    if (!bFound)
    {
        MQTTDataItem * mqttitem = new MQTTDataItem("Weight", 1, LoRAMacAddr, WeightData);
        mqttitem->setGatewayid(gatewayid);
        glqueue->add(mqttitem);
    }
}

void send_Tag_Heart_to_projects(const char * LoRAMacAddr, const char * tagmacAddr, std::string HeartData)
{
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        MQTTDataItem * mqttitem = new MQTTDataItem("TagHeart", projectid, LoRAMacAddr, HeartData);
        mqttitem->setTagMacaddress(tagmacAddr);
        glqueue->add(mqttitem);
    }
}

void send_Tag_Weight_to_projects(const char * LoRAMacAddr, const char * tagmacAddr, std::string WeightData)
{
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        MQTTDataItem * mqttitem = new MQTTDataItem("TagWeight", projectid, LoRAMacAddr, WeightData);
        mqttitem->setTagMacaddress(tagmacAddr);
        glqueue->add(mqttitem);
    }
}

void send_rangingcompress_to_projects(const char * LoRAMacAddr, const char * macaddress, int distance, const char * voltage, int velocity, const char * gatewayid, int rssi, const char * timestamp)
{
    std::string gatewayidString = gatewayid;

    bool bFound = false;
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        LoRaGatewayInfoStruct * gatwayExisted =  glMQTTDeviceManager->getLoRaGateway(projectid, gatewayidString);
        if (gatwayExisted != NULL )
        {
            MQTTDataItem * mqttitem = new MQTTDataItem("rangingcompress", projectid, LoRAMacAddr, macaddress, distance, voltage, velocity, gatewayid, rssi, timestamp);
            glqueue->add(mqttitem);
            bFound = true;
        }
    }
    if (!bFound)
    {
        MQTTDataItem * mqttitem = new MQTTDataItem("rangingcompress", 1, LoRAMacAddr, macaddress, distance, voltage, velocity, gatewayid, rssi, timestamp);
        glqueue->add(mqttitem);
    }
}

void send_rangingcompressMultiDistance_to_projects(const char * LoRAMacAddr, const char * macaddress, int distInterval, std::list<int> * distList, const char * voltage, int velocity, const char * gatewayid, int rssi, const char * timestamp)
{
    std::string gatewayidString = gatewayid;

    bool bFound = false;
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        LoRaGatewayInfoStruct * gatwayExisted =  glMQTTDeviceManager->getLoRaGateway(projectid, gatewayidString);
        if (gatwayExisted != NULL )
        {
            MQTTDataItem * mqttitem = new MQTTDataItem("rangingcompressMultiDistance", projectid, LoRAMacAddr, macaddress, distInterval, distList, voltage, velocity, gatewayid, rssi, timestamp);
            glqueue->add(mqttitem);
            bFound = true;
        }
    }
    if (!bFound)
    {
        MQTTDataItem * mqttitem = new MQTTDataItem("rangingcompressMultiDistance", 1, LoRAMacAddr, macaddress, distInterval, distList, voltage, velocity, gatewayid, rssi, timestamp);
        glqueue->add(mqttitem);
    }
}

void send_rangingcompressaoa_to_projects(const char * LoRAMacAddr, const char * macaddress, int distance, const char * voltage, int velocity, float angleFloat, const char * gatewayid, int rssi, int * IMU, const char * timestamp)
{
    std::string gatewayidString = gatewayid;

    bool bFound = false;
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        LoRaGatewayInfoStruct * gatwayExisted =  glMQTTDeviceManager->getLoRaGateway(projectid, gatewayidString);
        if (gatwayExisted != NULL )
        {
            MQTTDataItem * mqttitem = new MQTTDataItem("rangingcompressaoa", projectid, LoRAMacAddr, macaddress, distance, voltage, velocity, angleFloat, gatewayid, rssi, timestamp);
            mqttitem->setIMU(IMU);
            glqueue->add(mqttitem);

            bFound = true;
        }
    }
    if (!bFound)
    {
        MQTTDataItem * mqttitem = new MQTTDataItem("rangingcompressaoa", 1, LoRAMacAddr, macaddress, distance, voltage, velocity, angleFloat, gatewayid, rssi, timestamp);
        glqueue->add(mqttitem);
    }
}

void send_MQTT_resp_to_projects(std::string gatewayid, std::string dataId, std::string resp, int status)
{
    std::string gatewayidString = gatewayid;

    bool bFound = false;
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        LoRaGatewayInfoStruct * gatwayExisted =  glMQTTDeviceManager->getLoRaGateway(projectid, gatewayidString);
        if (gatwayExisted != NULL )
        {

            DataItem * senditem = new DataItem("MQTTResponse", projectid);
            senditem->setWaitkey(dataId);
            senditem->setResponseStatus(status);
            glmqttMainJobQueue->add(senditem);

            bFound = true;
        }
    }
    if (!bFound)
    {

        DataItem * senditem = new DataItem("MQTTResponse", 1);
        senditem->setWaitkey(dataId);
        senditem->setResponseStatus(status);
        glmqttMainJobQueue->add(senditem);

    }
}

void send_message_to_projects(std::string gatewayid, const char * LoRAMacAddr, std::string jsondata)
{
    std::string gatewayidString = gatewayid;

    bool bFound = false;
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        LoRaGatewayInfoStruct * gatwayExisted =  glMQTTDeviceManager->getLoRaGateway(projectid, gatewayidString);
        if (gatwayExisted != NULL )
        {

            DataItem * senditem = new DataItem("MQTTMessage", projectid);
            senditem->setLoRAMacaddress(LoRAMacAddr);
            senditem->setMessage(jsondata);
            glmqttMainJobQueue->add(senditem);

            bFound = true;
        }
    }
    if (!bFound)
    {

        DataItem * senditem = new DataItem("MQTTMessage", 1);
        senditem->setLoRAMacaddress(LoRAMacAddr);
        senditem->setMessage(jsondata);
        glmqttMainJobQueue->add(senditem);

    }
}

void send_OPENDOOR_to_projects(const char * LoRAMacAddr, const char * macaddress, const char * gatewayid, const char * timestamp)
{
    std::string gatewayidString = gatewayid;

    bool bFound = false;
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        LoRaGatewayInfoStruct * gatwayExisted =  glMQTTDeviceManager->getLoRaGateway(projectid, gatewayidString);
        if (gatwayExisted != NULL )
        {
            MQTTDataItem * mqttitem = new MQTTDataItem("OPENDOOR", projectid, LoRAMacAddr, macaddress, gatewayid, timestamp);
            glqueue->add(mqttitem);

            bFound = true;
        }
    }

    if (!bFound)
    {
        MQTTDataItem * mqttitem = new MQTTDataItem("OPENDOOR", 1, LoRAMacAddr, macaddress, gatewayid, timestamp);
        glqueue->add(mqttitem);
    }
}

void send_CLOSEDOOR_to_projects(const char * LoRAMacAddr, const char * gatewayid, const char * timestamp)
{
    std::string gatewayidString = gatewayid;

    bool bFound = false;
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        LoRaGatewayInfoStruct * gatwayExisted =  glMQTTDeviceManager->getLoRaGateway(projectid, gatewayidString);
        if (gatwayExisted != NULL )
        {
            MQTTDataItem * mqttitem = new MQTTDataItem("CLOSEDOOR", projectid, LoRAMacAddr, "", gatewayid, timestamp);
            glqueue->add(mqttitem);

            bFound = true;
        }
    }

    if (!bFound)
    {
        MQTTDataItem * mqttitem = new MQTTDataItem("CLOSEDOOR", 1, LoRAMacAddr, "", gatewayid, timestamp);
        glqueue->add(mqttitem);
    }
}

void send_GPIO_ON_to_projects(const char * LoRAMacAddr, const char * gatewayid, const char * timestamp)
{
    std::string gatewayidString = gatewayid;

    bool bFound = false;
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        LoRaGatewayInfoStruct * gatwayExisted =  glMQTTDeviceManager->getLoRaGateway(projectid, gatewayidString);
        if (gatwayExisted != NULL )
        {
            MQTTDataItem * mqttitem = new MQTTDataItem("GPIO_ON", projectid, LoRAMacAddr, "", gatewayid, timestamp);
            glqueue->add(mqttitem);

            bFound = true;
        }
    }

    if (!bFound)
    {
        MQTTDataItem * mqttitem = new MQTTDataItem("GPIO_ON", 1, LoRAMacAddr, "", gatewayid, timestamp);
        glqueue->add(mqttitem);
    }
}

bool send_MQTT_cmd(int projectid, std::string cmd, NFCDeviceStruct * device)
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

    // vilslog::printf("CARDRollCallService::send_MQTT_cmd() projectid[%d] payload[%s] gwid[%s]\n", projectid, payload.c_str(), device->gwid.c_str());

    MQTTDataItem * mqttitem = new MQTTDataItem("MQTTpublish", projectid, device->gwid.c_str(), payload.c_str(), (int)payload.length());
    glqueuePuslish->add(mqttitem);

    return true;
}

//
// {"mac": "4210000000000132", "Tp": "AOAfence", "lv": "1.-1;0.-1", "time": "0;27"}
//
void send_aoa_class_to_projects(const char * LoRAMacAddr, const char * macaddress, const char * lv, const char * time, const char * gatewayid, int rssi, const char * timestamp)
{
    std::string gatewayidString = gatewayid;

    bool bFound = false;
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        LoRaGatewayInfoStruct * gatwayExisted =  glMQTTDeviceManager->getLoRaGateway(projectid, gatewayidString);
        if (gatwayExisted != NULL )
        {
            MQTTDataItem * mqttitem = new MQTTDataItem("AOAfence", projectid, LoRAMacAddr, macaddress, gatewayid, timestamp);
            mqttitem->setAOA_lv(lv, time);
            mqttitem->setRSSI(rssi);
            glqueue->add(mqttitem);
            bFound = true;
        }
    }
    if (!bFound)
    {
        MQTTDataItem * mqttitem = new MQTTDataItem("AOAfence", 1, LoRAMacAddr, macaddress, gatewayid, timestamp);
        mqttitem->setAOA_lv(lv, time);
        mqttitem->setRSSI(rssi);
        glqueue->add(mqttitem);
    }
}

void send_PowerSwitch_to_projects(const char * LoRAMacAddr, const char * value, const char * gatewayid, int rssi, const char * timestamp)
{
    std::string gatewayidString = gatewayid;
    std::string valueString = std::string(value);

    bool bFound = false;
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        LoRaGatewayInfoStruct * gatwayExisted =  glMQTTDeviceManager->getLoRaGateway(projectid, gatewayidString);
        if (gatwayExisted != NULL )
        {
            MQTTDataItem * mqttitem = new MQTTDataItem("PowerSwitch", projectid, LoRAMacAddr, valueString);
            mqttitem->setGatewayid(gatewayid);
            mqttitem->setRSSI(rssi);
            mqttitem->setTimestamp(timestamp);
            glqueue->add(mqttitem);
            bFound = true;
        }
    }
    if (!bFound)
    {
        MQTTDataItem * mqttitem = new MQTTDataItem("PowerSwitch", 1, LoRAMacAddr, valueString);
        mqttitem->setGatewayid(gatewayid);
        mqttitem->setRSSI(rssi);
        mqttitem->setTimestamp(timestamp);
        glqueue->add(mqttitem);
    }
}

void send_beacon_to_projects(const char * LoRAMacAddr, const char * macaddress, const char * value, const char * gatewayid, int rssi, const char * timestamp)
{
    std::string gatewayidString = gatewayid;
    std::string valueString = std::string(value);

    bool bFound = false;
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        LoRaGatewayInfoStruct * gatwayExisted =  glMQTTDeviceManager->getLoRaGateway(projectid, gatewayidString);
        if (gatwayExisted != NULL )
        {
            MQTTDataItem * mqttitem = new MQTTDataItem("beacon", projectid, LoRAMacAddr, valueString);
            mqttitem->setGatewayid(gatewayid);
            mqttitem->setRSSI(rssi);
            mqttitem->setTimestamp(timestamp);
            mqttitem->setTagMacaddress(macaddress);
            glqueue->add(mqttitem);
            bFound = true;
        }
    }
    if (!bFound)
    {
        MQTTDataItem * mqttitem = new MQTTDataItem("beacon", 1, LoRAMacAddr, valueString);
        mqttitem->setGatewayid(gatewayid);
        mqttitem->setRSSI(rssi);
        mqttitem->setTimestamp(timestamp);
        mqttitem->setTagMacaddress(macaddress);
        glqueue->add(mqttitem);
    }
}

void send_iCover_to_projects(const char * LoRAMacAddr, const char * macaddress, const char * value, const char * gatewayid, int rssi, const char * timestamp)
{
    std::string gatewayidString = gatewayid;
    std::string valueString = std::string(value);

    bool bFound = false;
    for (std::list<int>::iterator prj = glPrjidList->begin(); prj != glPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        LoRaGatewayInfoStruct * gatwayExisted =  glMQTTDeviceManager->getLoRaGateway(projectid, gatewayidString);
        if (gatwayExisted != NULL )
        {
            MQTTDataItem * mqttitem = new MQTTDataItem("iCover", projectid, LoRAMacAddr, valueString);
            mqttitem->setGatewayid(gatewayid);
            mqttitem->setRSSI(rssi);
            mqttitem->setTimestamp(timestamp);
            mqttitem->setTagMacaddress(macaddress);
            glqueue->add(mqttitem);
            bFound = true;
        }
    }
    if (!bFound)
    {
        MQTTDataItem * mqttitem = new MQTTDataItem("iCover", 1, LoRAMacAddr, valueString);
        mqttitem->setGatewayid(gatewayid);
        mqttitem->setRSSI(rssi);
        mqttitem->setTimestamp(timestamp);
        mqttitem->setTagMacaddress(macaddress);
        glqueue->add(mqttitem);
    }
}
