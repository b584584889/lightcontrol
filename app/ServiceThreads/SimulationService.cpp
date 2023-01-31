/*
 *  SimulationService.cpp
 *
 *  Created on: 2018/10/19
 *      Author: ikki
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include <math.h>
#include <sys/time.h>
#include <time.h>
#include <set>
#include <filesystem>

#include "SimulationService.h"
#include "../PosCalculate/algorithm.h"
#include "../Simulation/SimulationMoving.h"
#include "../Simulation/SimulationPowerTag.h"
#include "../Simulation/SimulationSitting.h"
#include "../Filters/SmoothingFilter/SmoothingFilter.h"

using namespace std;

SimulationService::SimulationService(wqueue<DataItem*> & pUDPJobQueue, wqueue<MQTTDataItem*> & pMQTTqueue, wqueue<SubscribeDataItem*> & pWebSocketqueue, wqueue<DataItem*> & PosCalJobQueue, wqueue<NotificationItem*> & NotifiactionQueue, DeviceManager * pDevManager)
    : mUDPJobQueue(pUDPJobQueue), mMQTTqueue(pMQTTqueue), mWebSocketqueue(pWebSocketqueue), mPosCalJobQueue(PosCalJobQueue), mNotifiactionQueue(NotifiactionQueue), mDeviceManager(pDevManager)
{
}

bool SimulationService::status()
{
    return mInitial;
}

std::string SimulationService::statusString()
{
    return mStatus;
}

void * SimulationService::run()
{
    try
    {
        runloop();
    }
    catch(const std::exception &exc)
    {
        // catch anything thrown within try block that derives from std::exception
        std::cerr << exc.what();
        vilslog::printf("*********************************************\n");
        vilslog::printf("SimulationService::run() exception \n");
        vilslog::printf(exc.what());
        vilslog::printf("\n*********************************************\n");
    }
    return NULL;
}

void SimulationService::runloop()
{
    mInitial = false;

    mStatus = "SimulationService()::run() Initial successfully!";
    vilslog::printf ("SimulationService()::run() Initial successfully!\n");

    mInitial = true;

    waitloop();

}

void SimulationService::waitloop()
{
    vilslog::printf ("\n\n**************************\n**** Start Simulation data\n");

    // SimulationPowerTag * simulationPTag = new SimulationPowerTag(mDeviceManager);
    // simulationPTag->start();

    SimulationMoving * simulation = new SimulationMoving(mDeviceManager, mUDPJobQueue, mPosCalJobQueue);
    simulation->start();

    //test_smoothing_filter();

    // sleep(15);

    //test_simulation_GPIO_ON();

    //sleep(10);
    while ( 1 )
    {


        //test_simulation_Inspection();

        // test_Calculate_poistion();

        // test_PowerTag();

        // test_Door();

        // test_AoA();

        // test_tag_bodyinfo();

        //test_tag_voltage();

        // test_sos_cancel();

        //test_simulation_geteway();

        // test_simulation_checkpoint();

        // test_simulation_lora_0D();

        // test_simulation_lora_0D_exclude_rollcall_and_GPS();

        // test_simulation_lora_rollcall();

        //test_simulation_lora();

        // test_simulation_lora_GPS();

        //test_simulation_WebSocket();

        // test_simulation_MQTT_data();

        sleep(20);
    }
}

void SimulationService::test_simulation_GPIO_ON()
{
    char gatewayid[64];
    snprintf(gatewayid, 64, "00001c497bcc6f73");
    char timestamp[64];
    snprintf(timestamp, 64, "%s", currentDateTimeForSQL().c_str());
    int project_id = 1;

    MQTTDataItem * mqttitem = new MQTTDataItem("GPIO_ON", project_id, "0000000000050003", "", gatewayid, timestamp);
    mMQTTqueue.add(mqttitem);
}

void SimulationService::test_simulation_Inspection()
{

    simulation_point_behavior_normal();

    //simulation_point_behavior_unnormal();

    //simulation_point_behavior_unnormal_1();
}

void SimulationService::test_Calculate_poistion()
{
    // add_action_join("0320000000000055", NODE_TYPE_ANCHOR, -1, "4.12.10");
    // add_action_join("0320000000000056", NODE_TYPE_ANCHOR, -1, "4.12.10");
    // add_action_join("0320000000000066", NODE_TYPE_ANCHOR, -1, "4.12.10");
    // add_action_join("0320000000000075", NODE_TYPE_ANCHOR, -1, "4.12.10");
    // add_action_join("3131000000000040", NODE_TYPE_ANCHOR, -1, "4.12.10");

    mDeviceManager->setAnchorInfoS(1, "An0055", 84, 480, 250);
    mDeviceManager->setAnchorInfoS(1, "An0056", 890, 170, 250);
    mDeviceManager->setAnchorInfoS(1, "An0066", 84, 55, 250);
    mDeviceManager->setAnchorInfoS(1, "An0075", 950, 470, 250);
    mDeviceManager->setAnchorInfoS(1, "An0040", 455, 482, 245);

    mDeviceManager->setLocatorInfoS(1, "Locator0008", 1095, 824, 100);

    int IMU[6] = {0,0,-9,0,0,0};

    char gatewayid[64];
    snprintf(gatewayid, 64, "00001c497bcc6f73");
    char timestamp[64];
    snprintf(timestamp, 64, "%s", currentDateTimeForSQL().c_str());
    int velocity = 2;
    char voltage[256];
    memset(voltage, 0, 256);
    snprintf(voltage, 256, "3.9");
    int rssi = 0;

    // All (519,229,6)
    // 75/40/008 (570,834,-40)
    // 66/40/008 (571,834,-40)
    // 56/40/008 (852,544,7)  ->
    // 55/40/008 (388,927,115)
    // 55/75/008 (492,856,213)
    // 55/66/008 (477,230,1422)
    // 55/56/008 (695,689,467)
    // 56/66/008 (695,689,467) -> no result
    // 56/75/008 (998,35,-163)

    // 56/75/40 (554,125,-42)


    while(1)
    {
        mPosCalJobQueue.add(new DataItem("rangingdiag_1", 1, "0320000000000055", "0310000000000002", 1, 558, IMU, 1, 1, -90, -90, -90, -90, 1, 1, 1, "127.0.0.1"));
        mPosCalJobQueue.add(new DataItem("rangingdiag_1", 1, "0320000000000056", "0310000000000002", 1, 447, IMU, 1, 1, -90, -90, -90, -90, 1, 1, 1, "127.0.0.1"));
        //mPosCalJobQueue.add(new DataItem("rangingdiag_1", 1, "0320000000000066", "0310000000000002", 1, 529, IMU, 1, 1, -90, -90, -90, -90, 1, 1, 1, "127.0.0.1"));
        //mPosCalJobQueue.add(new DataItem("rangingdiag_1", 1, "0320000000000075", "0310000000000002", 1, 600, IMU, 1, 1, -90, -90, -90, -90, 1, 1, 1, "127.0.0.1"));
        mPosCalJobQueue.add(new DataItem("rangingdiag_1", 1, "3131000000000040", "0310000000000002", 1, 468, IMU, 1, 1, -90, -90, -90, -90, 1, 1, 1, "127.0.0.1"));

        // mPosCalJobQueue.add(new DataItem("rangingdiag_1", 1, "0320000000000055", "0310000000000002", 1, 526, IMU, 1, 1, -90, -90, -90, -90, 1, 1, 1, "127.0.0.1"));
        // mPosCalJobQueue.add(new DataItem("rangingdiag_1", 1, "0320000000000056", "0310000000000002", 1, 453, IMU, 1, 1, -90, -90, -90, -90, 1, 1, 1, "127.0.0.1"));
        // mPosCalJobQueue.add(new DataItem("rangingdiag_1", 1, "0320000000000066", "0310000000000002", 1, 505, IMU, 1, 1, -90, -90, -90, -90, 1, 1, 1, "127.0.0.1"));
        // mPosCalJobQueue.add(new DataItem("rangingdiag_1", 1, "0320000000000075", "0310000000000002", 1, 590, IMU, 1, 1, -90, -90, -90, -90, 1, 1, 1, "127.0.0.1"));
        // mPosCalJobQueue.add(new DataItem("rangingdiag_1", 1, "3131000000000040", "0310000000000002", 1, 337, IMU, 1, 1, -90, -90, -90, -90, 1, 1, 1, "127.0.0.1"));

        MQTTDataItem * mqttitem = new MQTTDataItem("rangingcompress", 1, "0000000000050008", "0310000000000002", 1012, voltage, velocity, gatewayid, rssi, timestamp);
        mMQTTqueue.add(mqttitem);

        sleep(1);
    }


}


void SimulationService::test_tag_voltage()
{
    char macaddress9913[32]; // Tag
    sprintf(macaddress9913, "0610000000009913");
    int projectid = 1;
    float voltage_f = 4.3f;

    char voltage[10];

    for(int i=0;i<14;i++)
    {
        sleep(10);
        snprintf(voltage, 10, "%.1f", voltage_f);
        mDeviceManager->setTagVoltage(projectid, macaddress9913, voltage);

        voltage_f -= 0.1f;
    }
}

void SimulationService::test_PowerTag()
{
    char LoRAMacaddress0001[32];
    sprintf(LoRAMacaddress0001, "0000000002070001");

    char AoAmacaddress0001[32];
    sprintf(AoAmacaddress0001, "0761010000000001");

    char macaddress0001[32]; // Tag
    char macaddress0002[32]; // Tag
    char macaddress0003[32]; // Tag

    sprintf(macaddress0001, "0410000000000001");
    sprintf(macaddress0002, "0410000000000002");
    sprintf(macaddress0003, "0410000000000003");

    char gatewayid[64];
    snprintf(gatewayid, 64, "00001c497bcc6f73");
    char timestamp[64];
    // snprintf(timestamp, 64, "2019-05-23T07:15:51");
    snprintf(timestamp, 64, "%s", currentDateTimeForSQL().c_str());


    int rssi = 0;
    float angle1 = 0.0f;
    float angle2 = 0.0f;
    float angle3 = 0.0f;
    int ranging1 = 0;
    int ranging2 = 0;
    int ranging3 = 0;
    int velocity = 2;

    int projectid = 1;
    char voltage[256];
    memset(voltage, 0, 256);
    snprintf(voltage, 256, "3.6");

    for(int i=0;i<1200;i++)
    {
        angle1 = angle_to_rad(-45.0f); ranging1 = 100;

        //vilslog::printf("SimulationService::test_AoA() [%s] angle1[%f]\n", LoRAMacaddress0001, rad_to_angle(angle1));
        DataItem * senditem1 = new DataItem("lorarangingcompressaoa", projectid, "LoRa", LoRAMacaddress0001, macaddress0001, ranging1, velocity, angle1, rssi, timestamp);
        mPosCalJobQueue.add(senditem1);

        angle2 = angle_to_rad(46.0f); ranging2 = 100;

        //vilslog::printf("SimulationService::test_AoA() [%s] angle2[%f]\n", LoRAMacaddress0002, rad_to_angle(angle2));
        DataItem * senditem2 = new DataItem("lorarangingcompressaoa", projectid, "LoRa", LoRAMacaddress0001, macaddress0002, ranging2, velocity, angle2, rssi, timestamp);
        mPosCalJobQueue.add(senditem2);

        angle3 = angle_to_rad(44.0f); ranging3 = 100;

        //vilslog::printf("SimulationService::test_AoA() [%s] angle1[%f]\n", LoRAMacaddress0003, rad_to_angle(angle3));
        DataItem * senditem3 = new DataItem("lorarangingcompressaoa", projectid, "LoRa", LoRAMacaddress0001, macaddress0003, ranging3, velocity, angle3, rssi, timestamp);
        mPosCalJobQueue.add(senditem3);

        sleep(1);
    }

}


void SimulationService::test_Door()
{
    char LoRAMacaddress0111[32];
    sprintf(LoRAMacaddress0111, "0000000000050111");

    char macaddress0022[32]; // Tag
    sprintf(macaddress0022, "0410000000000022");

    int projectid = 1;
    int rssi = 0;
    int velocity = 2;
    char voltage[256];
    memset(voltage, 0, 256);
    snprintf(voltage, 256, "3.6");
    char gatewayid[64];
    snprintf(gatewayid, 64, "00001c497bcc6f73");
    char timestamp[64];
    // snprintf(timestamp, 64, "2019-05-23T07:15:51");
    snprintf(timestamp, 64, "%s", currentDateTimeForSQL().c_str());


    sleep(10);

    for(int i=0;i<1;i++)
    {
        MQTTDataItem * mqttitem1 = new MQTTDataItem("OPENDOOR", projectid, LoRAMacaddress0111, macaddress0022, gatewayid, timestamp);
        mMQTTqueue.add(mqttitem1);

        for(int j=0;j<10;j++)
        {
            MQTTDataItem * mqttitem = new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0111, macaddress0022, 55, voltage, velocity, gatewayid, rssi, timestamp);
            mMQTTqueue.add(mqttitem);
            sleep(1);
        }

        sleep(10);

        MQTTDataItem * mqttitem2 = new MQTTDataItem("CLOSEDOOR", projectid, LoRAMacaddress0111);
        mMQTTqueue.add(mqttitem2);
    }

    sleep(60);
}

void SimulationService::test_AoA()
{
    char LoRAMacaddress0001[32];
    sprintf(LoRAMacaddress0001, "0000000002070001");
    char LoRAMacaddress0002[32];
    sprintf(LoRAMacaddress0002, "0000000002070002");
    char LoRAMacaddress0003[32];
    sprintf(LoRAMacaddress0003, "0000000002070003");
    char LoRAMacaddress0004[32];
    sprintf(LoRAMacaddress0004, "0000000002070004");

    char AoAmacaddress0001[32];
    sprintf(AoAmacaddress0001, "0761010000000001");
    char AoAmacaddress0002[32];
    sprintf(AoAmacaddress0002, "0761010000000002");
    char AoAmacaddress0003[32];
    sprintf(AoAmacaddress0003, "0761010000000003");
    char AoAmacaddress0004[32];
    sprintf(AoAmacaddress0004, "0761010000000004");

    char AoANodeId0001[32];
    sprintf(AoANodeId0001, "AOA0001");
    char AoANodeId0002[32];
    sprintf(AoANodeId0002, "AOA0002");
    char AoANodeId0003[32];
    sprintf(AoANodeId0003, "AOA0003");
    char AoANodeId0004[32];
    sprintf(AoANodeId0004, "AOA0004");


    char macaddress0022[32]; // Tag
    sprintf(macaddress0022, "0410000000000022");

    char gatewayid[64];
    snprintf(gatewayid, 64, "00001c497bcc6f73");
    char timestamp[64];
    // snprintf(timestamp, 64, "2019-05-23T07:15:51");
    snprintf(timestamp, 64, "%s", currentDateTimeForSQL().c_str());


    int rssi = 0;
    float angle1 = 0.01f;
    float angle2 = 0.3f;
    float angle3 = 0.0f;
    int ranging1 = 0;
    int ranging2 = 0;
    int ranging3 = 0;
    int velocity = 2;

    int projectid = 1;
    char PANID[256];
    char MODE[256];
    char VERSION[256];
    char voltage[256];
    memset(PANID, 0, 256);
    memset(MODE, 0, 256);
    memset(VERSION, 0, 256);
    memset(voltage, 0, 256);
    snprintf(PANID, 256, "0214");
    snprintf(MODE, 256, "0");
    snprintf(VERSION, 256, "V5.30.2-10");
    snprintf(voltage, 256, "3.6");

    //mDeviceManager->updateAoAList(1, LoRAMacaddress0001);

    // create AOA node
    mDeviceManager->setLocatorIndex(projectid, AoAmacaddress0004, -1, "");
    mDeviceManager->setLocatorInfoLoRA(projectid, AoAmacaddress0004, LoRAMacaddress0004);
    mDeviceManager->updateAoAList(projectid, LoRAMacaddress0004);


    mDeviceManager->setLocatorInfoS(1, AoANodeId0001, 120,540,170);
    mDeviceManager->setLocatorInfoS(1, AoANodeId0002, 300,0,180);
    mDeviceManager->setLocatorInfoS(1, AoANodeId0003, 0,0,195);
    mDeviceManager->setLocatorInfoS(1, AoANodeId0004, 10,10,10);

    mDeviceManager->setAoAInfo(1, AoAmacaddress0001, 120,540,170);
    mDeviceManager->setAoAInfo(1, AoAmacaddress0002, 300,0,180);
    mDeviceManager->setAoAInfo(1, AoAmacaddress0003, 0,0,195);
    mDeviceManager->setAoAInfo(1, AoAmacaddress0004, 10,10,10);

    int pitch = -25;
    int roll = 90;
    int yaw = -70;
    mDeviceManager->setAoAInfoS(1, AoAmacaddress0001, pitch, roll, yaw);
    mDeviceManager->setAoAInfoS(1, AoAmacaddress0002, -11, 34, 139);
    mDeviceManager->setAoAInfoS(1, AoAmacaddress0003, -28, 0, 56);
    mDeviceManager->setAoAInfoS(1, AoAmacaddress0004, 0, 0, 90);

    MQTTDataItem * mqttitem1 = new MQTTDataItem("LoRaDeviceBOOT", projectid, LoRAMacaddress0001, AoAmacaddress0001, PANID, MODE, VERSION);
    mMQTTqueue.add(mqttitem1);

    MQTTDataItem * mqttitem2 = new MQTTDataItem("LoRaDeviceBOOT", projectid, LoRAMacaddress0002, AoAmacaddress0002, PANID, MODE, VERSION);
    mMQTTqueue.add(mqttitem2);

    MQTTDataItem * mqttitem3 = new MQTTDataItem("LoRaDeviceBOOT", projectid, LoRAMacaddress0003, AoAmacaddress0003, PANID, MODE, VERSION);
    mMQTTqueue.add(mqttitem3);

    for(int i=0;i<1200;i++)
    {
        //angle1 = angle_to_rad(-22.35f); ranging1 = 422;
        angle1 = angle_to_rad(-15.47f); ranging1 = 184;

        vilslog::printf("SimulationService::test_AoA() [%s] angle1[%f]\n", LoRAMacaddress0001, rad_to_angle(angle1));
        DataItem * senditem1 = new DataItem("lorarangingcompressaoa", projectid, "LoRa", LoRAMacaddress0001, macaddress0022, ranging1, velocity, angle1, rssi, timestamp);
        mPosCalJobQueue.add(senditem1);

        //angle2 = angle_to_rad(12.03f); ranging2 = 162;
        angle2 = angle_to_rad(29.79f); ranging2 = 363;

        vilslog::printf("SimulationService::test_AoA() [%s] angle2[%f]\n", LoRAMacaddress0002, rad_to_angle(angle2));
        DataItem * senditem2 = new DataItem("lorarangingcompressaoa", projectid, "LoRa", LoRAMacaddress0002, macaddress0022, ranging2, velocity, angle2, rssi, timestamp);
        mPosCalJobQueue.add(senditem2);

        //angle3 = angle_to_rad(26.36f); ranging3 = 211;
        angle3 = angle_to_rad(-8.59f); ranging3 = 395;

        vilslog::printf("SimulationService::test_AoA() [%s] angle1[%f]\n", LoRAMacaddress0003, rad_to_angle(angle3));
        DataItem * senditem3 = new DataItem("lorarangingcompressaoa", projectid, "LoRa", LoRAMacaddress0003, macaddress0022, ranging3, velocity, angle3, rssi, timestamp);
        mPosCalJobQueue.add(senditem3);

        // vilslog::printf("SimulationService::test_AoA() [%s] angle1[%f]\n", LoRAMacaddress0004, rad_to_angle(angle3));
        // DataItem * senditem4 = new DataItem("lorarangingcompressaoa", projectid, "LoRa", LoRAMacaddress0004, macaddress0022, ranging3, velocity, angle3, rssi, timestamp);
        // mPosCalJobQueue.add(senditem4);

        sleep(1);
    }

    sleep(1200);
/*
    for(int i=0;i<20;i++)
    {
        angle1 = 0.3f;
        DataItem * senditem1 = new DataItem("lorarangingcompressaoa", projectid, "LoRa", LoRAMacaddress0001, macaddress0022, 600, velocity, angle1, rssi, timestamp);
        mPosCalJobQueue.add(senditem1);

        angle2 = 0.1f;
        DataItem * senditem2 = new DataItem("lorarangingcompressaoa", projectid, "LoRa", LoRAMacaddress0002, macaddress0022, 600, velocity, angle2, rssi, timestamp);
        mPosCalJobQueue.add(senditem2);
        sleep(1);
    }
    for(int i=0;i<20;i++)
    {
        angle1 = -0.3f;
        DataItem * senditem1 = new DataItem("lorarangingcompressaoa", 1, "LoRa", LoRAMacaddress0001, macaddress0022, 600, velocity, angle1, rssi, timestamp);
        mPosCalJobQueue.add(senditem1);

        angle2 = -0.1f;
        DataItem * senditem2 = new DataItem("lorarangingcompressaoa", 1, "LoRa", LoRAMacaddress0002, macaddress0022, 600, velocity, angle2, rssi, timestamp);
        mPosCalJobQueue.add(senditem2);

        sleep(1);
    }
    */
}


void SimulationService::test_tag_bodyinfo()
{
    char LoRAMacaddress0007[32];
    sprintf(LoRAMacaddress0007, "0000000000050007");

    char macaddress0001[32]; // Tag
    char macaddress0002[32]; // Tag
    char macaddress0003[32]; // Tag
    char macaddress0022[32]; // Tag
    sprintf(macaddress0001, "0410000000000001");
    sprintf(macaddress0002, "0410000000000002");
    sprintf(macaddress0003, "0410000000000003");
    sprintf(macaddress0022, "0410000000000022");

    char gatewayid[64];
    snprintf(gatewayid, 64, "00001c497bcc6f73");
    char timestamp[64];
    // snprintf(timestamp, 64, "2019-05-23T07:15:51");
    snprintf(timestamp, 64, "%s", currentDateTimeForSQL().c_str());

    int rssi = 0;
    int velocity = 2;

    int projectid = 1;
    char voltage[256];
    memset(voltage, 0, 256);
    snprintf(voltage, 256, "3.6");
    int iHeartData = 72;
    float fWeightData = 65;

    for(int i=0;i<10;i++)
    {
        std::string HeartData = std::to_string(iHeartData);
        std::string WeightData = std::to_string(fWeightData);

        MQTTDataItem * mqttitem1 = new MQTTDataItem("Heart", projectid, LoRAMacaddress0007, HeartData);
        mMQTTqueue.add(mqttitem1);

        MQTTDataItem * mqttitem2 = new MQTTDataItem("Weight", projectid, LoRAMacaddress0007, WeightData);
        mMQTTqueue.add(mqttitem2);
    }

    for(int i=0;i<10;i++)
    {
        MQTTDataItem * mqttitem = new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0007, macaddress0022, 55, voltage, velocity, gatewayid, rssi, timestamp);
        mMQTTqueue.add(mqttitem);
        MQTTDataItem * mqttitem1 = new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0007, macaddress0001, 55, voltage, velocity, gatewayid, rssi, timestamp);
        mMQTTqueue.add(mqttitem1);
        MQTTDataItem * mqttitem2 = new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0007, macaddress0002, 55, voltage, velocity, gatewayid, rssi, timestamp);
        mMQTTqueue.add(mqttitem2);
        MQTTDataItem * mqttitem3 = new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0007, macaddress0003, 55, voltage, velocity, gatewayid, rssi, timestamp);
        mMQTTqueue.add(mqttitem3);
    }

    for(int i=0;i<10;i++)
    {
        int noise = rand()%5;
        int direction = rand()%1;
        if (direction == 1)
            noise *= -1;

        iHeartData = 72 + noise;
        fWeightData = 65.3 + noise;

        std::string HeartData = std::to_string(iHeartData);
        std::string WeightData = std::to_string(fWeightData);

        MQTTDataItem * mqttitem = new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0007, macaddress0022, 55, voltage, velocity, gatewayid, rssi, timestamp);
        mMQTTqueue.add(mqttitem);

        MQTTDataItem * mqttitemH = new MQTTDataItem("Heart", projectid, LoRAMacaddress0007, HeartData);
        mMQTTqueue.add(mqttitemH);

        MQTTDataItem * mqttitemW = new MQTTDataItem("Weight", projectid, LoRAMacaddress0007, WeightData);
        mMQTTqueue.add(mqttitemW);


        MQTTDataItem * mqttitem1 = new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0007, macaddress0001, 155, voltage, velocity, gatewayid, rssi, timestamp);
        mMQTTqueue.add(mqttitem1);
        MQTTDataItem * mqttitem2 = new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0007, macaddress0002, 155, voltage, velocity, gatewayid, rssi, timestamp);
        mMQTTqueue.add(mqttitem2);
        MQTTDataItem * mqttitem3 = new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0007, macaddress0003, 155, voltage, velocity, gatewayid, rssi, timestamp);
        mMQTTqueue.add(mqttitem3);


        sleep(1);
    }

    for(int i=0;i<100;i++)
    {
        int noise = rand()%5;
        int direction = rand()%1;
        if (direction == 1)
            noise *= -1;

        iHeartData = 72 + noise;
        fWeightData = 65.3 + noise;

        std::string HeartData = std::to_string(iHeartData);
        std::string WeightData = std::to_string(fWeightData);

        MQTTDataItem * mqttitem = new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0007, macaddress0022, 45, voltage, velocity, gatewayid, rssi, timestamp);
        mMQTTqueue.add(mqttitem);

        MQTTDataItem * mqttitemH = new MQTTDataItem("Heart", projectid, LoRAMacaddress0007, HeartData);
        mMQTTqueue.add(mqttitemH);

        MQTTDataItem * mqttitemW = new MQTTDataItem("Weight", projectid, LoRAMacaddress0007, WeightData);
        mMQTTqueue.add(mqttitemW);


        MQTTDataItem * mqttitem1 = new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0007, macaddress0001, 145, voltage, velocity, gatewayid, rssi, timestamp);
        mMQTTqueue.add(mqttitem1);
        MQTTDataItem * mqttitem2 = new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0007, macaddress0002, 145, voltage, velocity, gatewayid, rssi, timestamp);
        mMQTTqueue.add(mqttitem2);
        MQTTDataItem * mqttitem3 = new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0007, macaddress0003, 145, voltage, velocity, gatewayid, rssi, timestamp);
        mMQTTqueue.add(mqttitem3);

        sleep(1);
    }
/*
    for(int i=0;i<10;i++)
    {
        MQTTDataItem * mqttitem = new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0007, macaddress0022, 55, voltage, velocity, gatewayid, rssi, timestamp);
        mMQTTqueue.add(mqttitem);
    }
*/
}

void SimulationService::test_sos_cancel()
{
    char LoRAMacaddress0005[32];
    char LoRAMacaddress0001[32];
    char LoRAMacaddress0002[32];
    char LoRAMacaddress0006[32];
    char LoRAMacaddress0007[32];
    char tagmacaddress0001[32];
    char tagmacaddress0002[32];
    char tagmacaddress0006[32];
    char tagmacaddress0007[32];
    char macaddress0002[32]; // Tag
    char macaddress0025[32]; // Tag
    char macaddress0003[32]; // 員工卡


    sprintf(LoRAMacaddress0005, "0000000000050005");
    sprintf(LoRAMacaddress0006, "0000000000050006");
    sprintf(LoRAMacaddress0001, "0000000000050001");
    sprintf(LoRAMacaddress0002, "0000000000050002");
    sprintf(LoRAMacaddress0007, "0000000000050007");
    sprintf(tagmacaddress0001, "0551040000000001"); // Locator UWB Macaddress
    sprintf(tagmacaddress0002, "0530000000000002"); // Locator UWB Macaddress
    sprintf(tagmacaddress0006, "0530000000000006"); // Locator UWB Macaddress
    sprintf(tagmacaddress0007, "0530000000000007"); // Locator UWB Macaddress
    sprintf(macaddress0002, "0310000000000002");
    sprintf(macaddress0025, "0410000000000025");
    sprintf(macaddress0003, "0610000000000003");
    char gatewayid[64];
    snprintf(gatewayid, 64, "00001c497bcc6f73");
    char timestamp[64];
    // snprintf(timestamp, 64, "2019-05-23T07:15:51");
    snprintf(timestamp, 64, "%s", currentDateTimeForSQL().c_str());

    sleep(15);

    // //=========================================
    // 同一員工卡 在 不同 locator 取消
    // MQTTDataItem * mqttitem1 = new MQTTDataItem("cancel", 1, LoRAMacaddress0001, macaddress0014, gatewayid, timestamp);
    // mMQTTqueue.add(mqttitem1);
    //
    // sleep(5);
    // MQTTDataItem * mqttitem2 = new MQTTDataItem("cancel", 1, LoRAMacaddress0005, macaddress0014, gatewayid, timestamp);
    // mMQTTqueue.add(mqttitem2);
    //
    // sleep(5);
    // MQTTDataItem * mqttitem3 = new MQTTDataItem("cancel", 1, LoRAMacaddress0001, macaddress0014, gatewayid, timestamp);
    // mMQTTqueue.add(mqttitem3);
    //
    // sleep(5);
    // MQTTDataItem * mqttitem4 = new MQTTDataItem("cancel", 1, LoRAMacaddress0005, macaddress0014, gatewayid, timestamp);
    // mMQTTqueue.add(mqttitem4);
    //
    // sleep(5);
    // MQTTDataItem * mqttitem5 = new MQTTDataItem("alert", 1, LoRAMacaddress0002, macaddress0014, gatewayid, timestamp);
    // mMQTTqueue.add(mqttitem5);
    //
    //sleep(20);
    // //=========================================

    // //=========================================
    // // Locator SOS ,  Locator 自己取消
    // MQTTDataItem * mqttitem1 = new MQTTDataItem("alert", 1, LoRAMacaddress0002, tagmacaddress0002, gatewayid, timestamp);
    // mMQTTqueue.add(mqttitem1);

    // // 重發
    // for(int i=0;i<1;i++)
    // {
    //     sleep(5);
    //     MQTTDataItem * mqttitem1x = new MQTTDataItem("alert", 1, LoRAMacaddress0002, tagmacaddress0002, gatewayid, timestamp);
    //     mMQTTqueue.add(mqttitem1x);
    // }
    //sleep(60);

    // SELF cancel alert
    //MQTTDataItem * mqttitem2 = new MQTTDataItem("cancel", 1, LoRAMacaddress0002, "SELF", gatewayid, timestamp);
    //mMQTTqueue.add(mqttitem2);

    // 重發
    // sleep(5);
    // MQTTDataItem * mqttitem2x = new MQTTDataItem("cancel", 1, LoRAMacaddress0002, "SELF", gatewayid, timestamp);
    // mMQTTqueue.add(mqttitem2x);

    // sleep(600);
    // //=========================================

    // //=========================================
    // Locator SOS , 員工卡取消
    // MQTTDataItem * mqttitem3 = new MQTTDataItem("alert", 1, LoRAMacaddress0001, tagmacaddress0001, gatewayid, timestamp);
    // mMQTTqueue.add(mqttitem3);
    //
    // // 重發
    // sleep(5);
    // MQTTDataItem * mqttitem3x = new MQTTDataItem("alert", 1, LoRAMacaddress0001, tagmacaddress0001, gatewayid, timestamp);
    // mMQTTqueue.add(mqttitem3x);
    //
    // sleep(10);
    //
    // MQTTDataItem * mqttitem4 = new MQTTDataItem("cancel", 1, LoRAMacaddress0001, macaddress0003, gatewayid, timestamp);
    // mMQTTqueue.add(mqttitem4);
    //
    // // 重發
    // sleep(5);
    // MQTTDataItem * mqttitem4x = new MQTTDataItem("cancel", 1, LoRAMacaddress0001, macaddress0003, gatewayid, timestamp);
    // mMQTTqueue.add(mqttitem4x);
    //
    // sleep(20);
    // //=========================================


    // //=========================================
    // Tag 在 Locator 發出 SOS , 員工卡取消
    MQTTDataItem * mqttitem5 = new MQTTDataItem("alert", 1, LoRAMacaddress0007, macaddress0025, gatewayid, timestamp);
    mMQTTqueue.add(mqttitem5);

    // 重發
    // sleep(5);
    MQTTDataItem * mqttitem5x = new MQTTDataItem("alert", 1, LoRAMacaddress0007, macaddress0025, gatewayid, timestamp);
    mMQTTqueue.add(mqttitem5x);

    sleep(10);

    MQTTDataItem * mqttitem6 = new MQTTDataItem("cancel", 1, LoRAMacaddress0007, macaddress0003, gatewayid, timestamp);
    mMQTTqueue.add(mqttitem6);

    // 重發
    // sleep(5);
    MQTTDataItem * mqttitem6x = new MQTTDataItem("cancel", 1, LoRAMacaddress0007, macaddress0003, gatewayid, timestamp);
    mMQTTqueue.add(mqttitem6x);

    sleep(20);
    // //=========================================

    //=========================================
    // Tag 在 Locator 發出 SOS , 員工卡 在另一個 Locator 取消
    // MQTTDataItem * mqttitem5 = new MQTTDataItem("alert", 1, LoRAMacaddress0001, macaddress0003, gatewayid, timestamp);
    // mMQTTqueue.add(mqttitem5);
    //
    // // 重發
    // sleep(5);
    // MQTTDataItem * mqttitem5x = new MQTTDataItem("alert", 1, LoRAMacaddress0001, macaddress9913, gatewayid, timestamp);
    // mMQTTqueue.add(mqttitem5x);
    //
    // sleep(10);
    //
    // MQTTDataItem * mqttitem6 = new MQTTDataItem("cancel", 1, LoRAMacaddress0005, macaddress0014, gatewayid, timestamp);
    // mMQTTqueue.add(mqttitem6);
    //
    // // 重發
    // sleep(5);
    // MQTTDataItem * mqttitem6x = new MQTTDataItem("cancel", 1, LoRAMacaddress0005, macaddress0014, gatewayid, timestamp);
    // mMQTTqueue.add(mqttitem6x);
    //
    // sleep(20);
    //=========================================

    // //=========================================
    // // Tag 在 Coordinator 發出 SOS, 員工卡 在同一個Coordinator 取消
    // int projectid = 1;
    //
    // DataItem * jobitem1 = new DataItem("alert", projectid, macaddress9913, NODE_TYPE_TAG);
    // jobitem1->setIPAddress("192.168.99.107");
    // mUDPJobQueue.add(jobitem1);
    //
    // // 重發
    // sleep(5);
    // DataItem * jobitem1x = new DataItem("alert", projectid, macaddress9913, NODE_TYPE_TAG);
    // jobitem1x->setIPAddress("192.168.99.107");
    // mUDPJobQueue.add(jobitem1x);
    //
    // sleep(10);
    //
    // DataItem * jobitem = new DataItem("cancel", projectid, macaddress0014, NODE_TYPE_TAG);
    // jobitem->setIPAddress("192.168.99.107");
    // mUDPJobQueue.add(jobitem);
    //
    // sleep(20);
    // //=========================================

    // //=========================================
    // // Tag 在 Coordinator 發出 SOS, 員工卡 在另一個Coordinator 取消
    // int projectid = 1;
    // mDeviceManager->setCoordinatorInfo(projectid, "3130000000000021", "192.168.99.109", "vilscoord - 1.3.11-UDP","60000","0820");
    //
    // DataItem * jobitem1 = new DataItem("alert", projectid, macaddress9913, NODE_TYPE_TAG);
    // jobitem1->setIPAddress("192.168.99.107");
    // mUDPJobQueue.add(jobitem1);
    //
    // // 重發
    // sleep(5);
    // DataItem * jobitem1x = new DataItem("alert", projectid, macaddress9913, NODE_TYPE_TAG);
    // jobitem1x->setIPAddress("192.168.99.107");
    // mUDPJobQueue.add(jobitem1x);
    //
    // sleep(10);
    //
    // DataItem * jobitem2 = new DataItem("cancel", projectid, macaddress0014, NODE_TYPE_TAG);
    // jobitem2->setIPAddress("192.168.99.109");
    // mUDPJobQueue.add(jobitem2);
    //
    // // 重發
    // sleep(5);
    // DataItem * jobitem2x = new DataItem("cancel", projectid, macaddress0014, NODE_TYPE_TAG);
    // jobitem2x->setIPAddress("192.168.99.109");
    // mUDPJobQueue.add(jobitem2x);
    //
    // sleep(20);
    // //=========================================

    sleep(90);
}

void SimulationService::test_smoothing_filter()
{
    SmoothingFilter * mDataRateSmoothingFilter;

    mDataRateSmoothingFilter = new SmoothingFilter();

    double mDataRate_results[3];
    mDataRate_results[0] = 0;
    mDataRate_results[1] = 0;
    mDataRate_results[2] = 0;
    mDataRateSmoothingFilter->initial(mDataRate_results);
    mDataRateSmoothingFilter->update(mDataRate_results, mDataRate_results);

    int dir = 1;
    float datarate = 0.0;

    srand((unsigned int)time(NULL));

    for(int i=0; i<100; i++)
    {
        //=============================

        int ioffset = rand() % 100;
        float foffset = (float)ioffset / 1000.0f;

        if ( dir == 1 )
        {
            datarate += foffset;
        }
        else
        {
            datarate -= foffset;
        }
        if ( datarate >= 2.0 )
        {
            dir = 0;
        }
        if ( datarate <= 0.3)
        {
            dir = 1;
        }
        //=============================
        mDataRate_results[0] = datarate;
        mDataRateSmoothingFilter->update(mDataRate_results, mDataRate_results);

        vilslog::printf("SimulationService::test_smoothing_filter() [%d] [%.02f] -> [%.02f]\n", i, datarate, mDataRate_results[0]);

    }
}




//  clear test data
//
//   delete from CheckPointGroupDailyRecord where GroupID=14 and DateTime='20190418';
//   delete from CheckPointPathDailyRecord where GroupID=14 and DateTime='20190418';
//   delete from CheckPointTagBehaviorData where GroupID=14 and DateTime='20190418';
//
void SimulationService::test_simulation_checkpoint()
{

    simulation_point_behavior_normal();

    //simulation_point_behavior_unnormal();

    //simulation_point_behavior_unnormal_1();

}

void SimulationService::simulation_point_behavior_normal()
{

    char macaddress0302[32];
    char macaddress0025[32];
    char LoRAMacaddress0001[32];
    char LoRAMacaddress0002[32];
    char LoRAMacaddress0003[32];
    char LoRAMacaddress0004[32];

    sprintf(macaddress0302, "0421000000000302");
    sprintf(macaddress0025, "0410000000000025");
    sprintf(LoRAMacaddress0001, "0000000000050001");
    sprintf(LoRAMacaddress0002, "0000000000050002");
    sprintf(LoRAMacaddress0003, "0000000000050003");
    sprintf(LoRAMacaddress0004, "0000000000050004");

    sleep(20);

    simulation_point_behavior(LoRAMacaddress0001, macaddress0025, 510, 40, 5, 40, 5);
    sleep(15);

    simulation_point_behavior(LoRAMacaddress0002, macaddress0025, 550, 20, 10, 20, 15);
    sleep(15);

    // simulation_point_behavior(LoRAMacaddress0003, macaddress0025, 510, 40, 5, 40, 5);
    // sleep(180);

    // simulation_point_behavior(LoRAMacaddress0004, macaddress0025, 600, 30, 10, 30, 15);
    // sleep(15);
}

void SimulationService::simulation_point_behavior_unnormal()
{
    char macaddress0026[32];
    char LoRAMacaddress0013[32];
    char LoRAMacaddress1001[32];
    char LoRAMacaddress1004[32];
    char LoRAMacaddress1005[32];

    sprintf(macaddress0026, "0410000000000026");
    sprintf(LoRAMacaddress0013, "0000000000050013");
    sprintf(LoRAMacaddress1001, "0000000000051001");
    sprintf(LoRAMacaddress1004, "0000000000051004");
    sprintf(LoRAMacaddress1005, "0000000000051005");

    sleep(20);


    simulation_point_behavior(LoRAMacaddress1001, macaddress0026, 300, 10, 2, 0, 0);
    sleep(15);

    simulation_point_behavior(LoRAMacaddress0013, macaddress0026, 300, 10, 2, 0, 0);
    sleep(15);

    simulation_point_behavior(LoRAMacaddress1001, macaddress0026, 300, 10, 2, 0, 0);
    sleep(15);

    simulation_point_behavior(LoRAMacaddress1004, macaddress0026, 300, 20, 5, 0, 0);
    sleep(15);

    simulation_point_behavior(LoRAMacaddress1001, macaddress0026, 300, 10, 2, 0, 0);
    sleep(15);

    simulation_point_behavior(LoRAMacaddress1005, macaddress0026, 300, 20, 5, 0, 0);
    sleep(15);

}

void SimulationService::simulation_point_behavior_unnormal_1()
{
    char macaddress0026[32];
    char LoRAMacaddress0013[32];
    char LoRAMacaddress1001[32];
    char LoRAMacaddress1004[32];
    char LoRAMacaddress1005[32];

    sprintf(macaddress0026, "0410000000000026");
    sprintf(LoRAMacaddress0013, "0000000000050013");
    sprintf(LoRAMacaddress1001, "0000000000051001");
    sprintf(LoRAMacaddress1004, "0000000000051004");
    sprintf(LoRAMacaddress1005, "0000000000051005");

    sleep(20);

    simulation_point_behavior(LoRAMacaddress1001, macaddress0026, 300, 10, 2, 0, 0);
    sleep(15);

//    simulation_point_behavior(LoRAMacaddress1004, macaddress0026, 300, 20, 5, 0, 0);
//    sleep(15);

    simulation_point_behavior(LoRAMacaddress0013, macaddress0026, 300, 10, 2, 0, 0);
    sleep(15);

//    simulation_point_behavior(LoRAMacaddress1005, macaddress0026, 300, 20, 5, 0, 0);
//    sleep(15);

    simulation_point_behavior(LoRAMacaddress1001, macaddress0026, 300, 10, 2, 0, 0);
    sleep(15);


}

void SimulationService::simulation_point_behavior(const char * LoRaMac, const char * TagMac,
                                                  int start_distance, int approaching_time, int approaching_step_distance,
                                                  int leaving_time, int leaving_step_distance)
{
    char voltage[256];
    memset(voltage, 0, 256);
    snprintf(voltage, 256, "3.0");
    char gatewayid[64];
    snprintf(gatewayid, 64, "00001c497bcc6f73");
    char timestamp[64];
    // snprintf(timestamp, 64, "2019-05-23T07:15:51");
    snprintf(timestamp, 64, "%s", currentDateTimeForSQL().c_str());

    int rssi = 0;

    int rangingValue = start_distance;
    for(int i=0;i<approaching_time;i++)
    {
        // approaching
        rangingValue -= approaching_step_distance;
        snprintf(timestamp, 64, "%s", currentDateTimeForSQL().c_str());

        MQTTDataItem * mqttitemRanging = new MQTTDataItem("ranging", 1, LoRaMac, TagMac, rangingValue, voltage, gatewayid, rssi, timestamp);
        mMQTTqueue.add(mqttitemRanging);
        sleep(1);
    }
    for(int i=0;i<leaving_time;i++)
    {
        // leaveing
        rangingValue += leaving_step_distance;
        snprintf(timestamp, 64, "%s", currentDateTimeForSQL().c_str());

        MQTTDataItem * mqttitemRanging = new MQTTDataItem("ranging", 1, LoRaMac, TagMac, rangingValue, voltage, gatewayid, rssi, timestamp);
        mMQTTqueue.add(mqttitemRanging);
        sleep(1);
    }
}

void SimulationService::test_simulation_lora()
{
    char LoRAMacaddressNoExist[32];
    char LoRAMacaddress0001[32];
    char LoRAMacaddress1000[32];
    char LoRAMacaddress1001[32];
    char LoRAMacaddress1002[32];
    char LoRAMacaddress1006[32];
    char LoRAMacaddress1007[32];
    char macaddress0005[32];
    char macaddress0022[32];
    char macaddress9913[32];
    char tagmacaddress0001[32];
    // static int rangingValue = 100;

    sprintf(LoRAMacaddressNoExist, "0000000000059999");
    sprintf(LoRAMacaddress0001, "0000000000050001");
    sprintf(LoRAMacaddress1000, "0000000000051000");
    sprintf(LoRAMacaddress1001, "0000000000051001");
    sprintf(LoRAMacaddress1002, "0000000000051002");
    sprintf(LoRAMacaddress1006, "0000000000051006");
    sprintf(LoRAMacaddress1007, "0000000000051007");
    sprintf(macaddress0005, "0410000000000005");
    sprintf(macaddress0022, "0410000000000022");
    sprintf(macaddress9913, "0610000000009913");
    sprintf(tagmacaddress0001, "0530000000000001"); // Locator UWB Macaddress
    char gatewayid[64];
    snprintf(gatewayid, 64, "00001c497bcc6f73");
    char timestamp[64];
    // snprintf(timestamp, 64, "2019-05-23T07:15:51");
    snprintf(timestamp, 64, "%s", currentDateTimeForSQL().c_str());


    sleep(15);

    //=========================================
    // locator alert
    //
    vilslog::printf("SimulationService::test_simulation_lora() locator alert, lora[%s] tag[%s]\n", LoRAMacaddress0001, tagmacaddress0001);
    MQTTDataItem * mqttitem3 = new MQTTDataItem("alert", 1, LoRAMacaddress0001, tagmacaddress0001, gatewayid, timestamp);
    mMQTTqueue.add(mqttitem3);

    sleep(10);

    // tag LoRa alert
    MQTTDataItem * mqttitem1 = new MQTTDataItem("alert", 1, LoRAMacaddress0001, macaddress9913, gatewayid, timestamp);
    mMQTTqueue.add(mqttitem1);

    sleep(5);

    // SELF cancel alert
    vilslog::printf("SimulationService::test_simulation_lora() locator cancel alert, lora[%s] tag[%s]\n", LoRAMacaddress0001, tagmacaddress0001);
    MQTTDataItem * mqttitem4 = new MQTTDataItem("cancel", 1, LoRAMacaddress0001, "SELF", gatewayid, timestamp);
    mMQTTqueue.add(mqttitem4);

    sleep(5);

    // tag UWB alert
    create_alert_event_no_area(1, macaddress9913, "Tag9913");

    sleep(10);

    // unknown Tag cancel alert
    vilslog::printf("SimulationService::test_simulation_lora() locator cancel alert, lora[%s] tag[%s]\n", LoRAMacaddress0001, "0000000000000000");
    MQTTDataItem * mqttitem5 = new MQTTDataItem("cancel", 1, LoRAMacaddress0001, "0000000000000000", gatewayid, timestamp);
    mMQTTqueue.add(mqttitem5);

    sleep(5);

    // Tag cancel alert
    // vilslog::printf("SimulationService::test_simulation_lora() locator cancel alert, lora[%s] tag[%s]\n", LoRAMacaddress0001, macaddress0005);
    // MQTTDataItem * mqttitem4 = new MQTTDataItem("cancel", 1, LoRAMacaddress0001, macaddress0005, gatewayid, timestamp);
    // mMQTTqueue.add(mqttitem4);
    //
    // sleep(5);

    //tag alert
    // create_alert_event_no_area(1, macaddress9913, "Tag9913");

    sleep(10);

    //=========================================
    //sleep(20);

    // // locator cancel alert by himself
    //MQTTDataItem * mqttitem5 = new MQTTDataItem("cancel", LoRAMacaddress, "SELF");
    //mMQTTqueue.add(mqttitem5);

    //sleep(10);

    //DataItem * item = new DataItem("alert", macaddress0005, NODE_TYPE_TAG);
    //mUDPJobQueue.add(item);

    sleep(120);

    //
    // int ranging = rangingValue++;
    // char voltage[256];
    // memset(voltage, 0, 256);
    // snprintf(voltage, 256, "3.0");
    // MQTTDataItem * mqttitemRanging = new MQTTDataItem("ranging", LoRAMacaddress0013, macaddress0026, ranging, voltage);
    // mMQTTqueue.add(mqttitemRanging);
    // sleep(5);
    //
    // MQTTDataItem * mqttitemRanging2 = new MQTTDataItem("ranging", LoRAMacaddress1000, macaddress0340, ranging, voltage);
    // mMQTTqueue.add(mqttitemRanging2);
    // sleep(5);

    // MQTTDataItem * mqttitemRanging3 = new MQTTDataItem("ranging", LoRAMacaddress1006, macaddress0026, ranging, voltage);
    // mMQTTqueue.add(mqttitemRanging3);
    // sleep(5);
    //
    // MQTTDataItem * mqttitemRanging4 = new MQTTDataItem("ranging", LoRAMacaddress1007, macaddress0340, ranging, voltage);
    // mMQTTqueue.add(mqttitemRanging4);
    //sleep(5);

    // sleep(10);
    //
    //
    // MQTTDataItem * mqttitemRanging5 = new MQTTDataItem("ranging", LoRAMacaddress1001, macaddress0026, ranging, voltage);
    // mMQTTqueue.add(mqttitemRanging5);
    // sleep(5);
    //
    // MQTTDataItem * mqttitemRanging6 = new MQTTDataItem("ranging", LoRAMacaddress1002, macaddress0340, ranging, voltage);
    // mMQTTqueue.add(mqttitemRanging6);
    // sleep(5);

    // MQTTDataItem * mqttitemRanging7 = new MQTTDataItem("ranging", LoRAMacaddress1006, macaddress0026, ranging, voltage);
    // mMQTTqueue.add(mqttitemRanging7);
    // sleep(5);
    //
    // MQTTDataItem * mqttitemRanging8 = new MQTTDataItem("ranging", LoRAMacaddress1007, macaddress0340, ranging, voltage);
    // mMQTTqueue.add(mqttitemRanging8);
    //sleep(5);

    // sleep(10);
    //
    // if (rangingValue >= 350)
    //     rangingValue = 100;

    //
    // GPS Ingo
    //
    // double Longitude = 121.538597106934;
    // double Latitude = 25.040199279785;
    // MQTTDataItem * mqttitem = new MQTTDataItem("taggps", LoRAMacaddress, macaddress1, Longitude, Latitude);
    // mMQTTqueue.add(mqttitem);
    //
    // vilslog::printf ("SimulationService::test_simulation_lora() taggps LoRAMacaddress[%s] macaddress[%s] Longitude[%f] Latitude[%f] \n",
    //     LoRAMacaddress, macaddress1, Longitude, Latitude);


    //return NULL;
}

void SimulationService::test_simulation_lora_0D()
{
    char macaddress0001[32];
    char macaddress0002[32];
    char macaddress0003[32];
    char macaddress0004[32];
    char macaddress0005[32];
    char macaddress0006[32];
    char macaddress0007[32];
    char macaddress0008[32];
    char macaddress0009[32];
    char macaddress0010[32];

    char LoRAMacaddress0001[32];
    char LoRAMacaddress0002[32];
    char LoRAMacaddress1000[32];
    char LoRAMacaddress0177[32];
    char LoRAMacaddress0006[32];
    char LoRAMacaddress0007[32];
    char voltage[256];
    memset(voltage, 0, 256);
    snprintf(voltage, 256, "3.9");

    sprintf(macaddress0001, "0610000000000001");
    sprintf(macaddress0002, "0610000000000002");
    sprintf(macaddress0003, "0610000000000003");
    sprintf(macaddress0004, "0610000000000004");
    sprintf(macaddress0005, "0610000000000005");
    sprintf(macaddress0006, "0610000000000006");
    sprintf(macaddress0007, "0610000000000007");
    sprintf(macaddress0008, "0610000000000008");
    sprintf(macaddress0009, "0610000000000009");
    sprintf(macaddress0010, "0610000000000010");

    sprintf(LoRAMacaddress0001, "0000000000050001");
    sprintf(LoRAMacaddress0002, "0000000000050002");
    sprintf(LoRAMacaddress1000, "0000000000051000");
    sprintf(LoRAMacaddress0177, "0000000000050177");
    sprintf(LoRAMacaddress0006, "0000000000050006");
    sprintf(LoRAMacaddress0007, "0000000000050007");

    char gatewayid[64];
    snprintf(gatewayid, 64, "00001c497bcc6f73");
    char timestamp[64];
    // snprintf(timestamp, 64, "2019-05-23T07:15:51");
    snprintf(timestamp, 64, "%s", currentDateTimeForSQL().c_str());

    add_action_join(macaddress0001, NODE_TYPE_TAG, -1, "4.13.0");
    add_action_join(macaddress0002, NODE_TYPE_TAG, -1, "4.13.0");
    add_action_join(macaddress0003, NODE_TYPE_TAG, -1, "4.13.0");
    add_action_join(macaddress0004, NODE_TYPE_TAG, -1, "4.13.0");
    add_action_join(macaddress0005, NODE_TYPE_TAG, -1, "4.13.0");
    add_action_join(macaddress0006, NODE_TYPE_TAG, -1, "4.13.0");
    add_action_join(macaddress0007, NODE_TYPE_TAG, -1, "4.13.0");
    add_action_join(macaddress0008, NODE_TYPE_TAG, -1, "4.13.0");
    add_action_join(macaddress0009, NODE_TYPE_TAG, -1, "4.13.0");
    add_action_join(macaddress0010, NODE_TYPE_TAG, -1, "4.13.0");

    int rssi = 0;
    int velocity = 2;
    int projectid = 1;

    // for(int i=0;i<10;i++)
    // {
    //     MQTTDataItem * mqttitem = new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0007, macaddress9914, 350, voltage, velocity, gatewayid, rssi, timestamp);
    //     mMQTTqueue.add(mqttitem);
    //
    //     sleep(1);
    // }

    velocity = 0;
    for(int i=0;i<10;i++)
    {
        mMQTTqueue.add(new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0007, macaddress0001, 50, voltage, velocity, gatewayid, rssi, timestamp));
        mMQTTqueue.add(new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0007, macaddress0002, 50, voltage, velocity, gatewayid, rssi, timestamp));
        mMQTTqueue.add(new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0007, macaddress0003, 50, voltage, velocity, gatewayid, rssi, timestamp));
        mMQTTqueue.add(new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0007, macaddress0004, 50, voltage, velocity, gatewayid, rssi, timestamp));
        mMQTTqueue.add(new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0007, macaddress0005, 50, voltage, velocity, gatewayid, rssi, timestamp));
        mMQTTqueue.add(new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0007, macaddress0006, 50, voltage, velocity, gatewayid, rssi, timestamp));
        mMQTTqueue.add(new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0007, macaddress0007, 50, voltage, velocity, gatewayid, rssi, timestamp));
        mMQTTqueue.add(new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0007, macaddress0008, 50, voltage, velocity, gatewayid, rssi, timestamp));
        mMQTTqueue.add(new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0007, macaddress0009, 50, voltage, velocity, gatewayid, rssi, timestamp));
        mMQTTqueue.add(new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0007, macaddress0010, 50, voltage, velocity, gatewayid, rssi, timestamp));

        sleep(1);
    }

    // sleep(10);
    //
    // //
    // // test poweroff
    // //
    // MQTTDataItem * mqttitem = new MQTTDataItem("LoRaDevicePOWEROFF", projectid, LoRAMacaddress0002);
    // mMQTTqueue.add(mqttitem);
    //
    // sleep(30);
    //
    // velocity = 2;
    // for(int i=0;i<20;i++)
    // {
    //     MQTTDataItem * mqttitem = new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0002, macaddress9914, 350, voltage, velocity, gatewayid, rssi, timestamp);
    //     mMQTTqueue.add(mqttitem);
    //
    //     sleep(1);
    // }

    // for(int i=0;i<1;i++)
    // {
    //     MQTTDataItem * mqttitem = new MQTTDataItem("rangingcompress", 2, LoRAMacaddress0177, macaddress9914, 550, voltage, velocity, gatewayid, rssi, timestamp);
    //     mMQTTqueue.add(mqttitem);
    //     sleep(1);
    // }
    //
    // for(int i=0;i<2;i++)
    // {
    //     MQTTDataItem * mqttitem = new MQTTDataItem("rangingcompress", 2, LoRAMacaddress0177, macaddress9914, 350, voltage, velocity, gatewayid, rssi, timestamp);
    //     mMQTTqueue.add(mqttitem);
    //     sleep(1);
    // }
    //
    // for(int i=0;i<5;i++)
    // {
    //     MQTTDataItem * mqttitem = new MQTTDataItem("rangingcompress", 2, LoRAMacaddress0177, macaddress9914, 550, voltage, velocity, gatewayid, rssi, timestamp);
    //     mMQTTqueue.add(mqttitem);
    //     sleep(1);
    // }

    //sleep(5);

    // for(int i=0;i<10;i++)
    // {
    //     MQTTDataItem * mqttitemRanging0 = new MQTTDataItem("ranging", 1, LoRAMacaddress0177, macaddress9914, 650, voltage, gatewayid, rssi, timestamp);
    //     mMQTTqueue.add(mqttitemRanging0);
    //     sleep(1);
    // }
    //
    // sleep(10);

    // for(int i=0;i<20;i++)
    // {
    //     MQTTDataItem * mqttitem = new MQTTDataItem("rangingcompress", 2, LoRAMacaddress0006, macaddress9914, 350, voltage, velocity, gatewayid, rssi, timestamp);
    //     mMQTTqueue.add(mqttitem);
    //     sleep(1);
    // }
    //
    // velocity = 1;
    // for(int i=0;i<10;i++)
    // {
    //     MQTTDataItem * mqttitem = new MQTTDataItem("rangingcompress", 2, LoRAMacaddress0006, macaddress9914, 350, voltage, velocity, gatewayid, rssi, timestamp);
    //     mMQTTqueue.add(mqttitem);
    //     sleep(1);
    // }

    //sleep(180);

/*
    MQTTDataItem * mqttitemRanging1 = new MQTTDataItem("ranging", LoRAMacaddress1002, macaddress0026, 50, voltage, gatewayid);
    mMQTTqueue.add(mqttitemRanging1);

    sleep(1);
    MQTTDataItem * mqttitemRanging2 = new MQTTDataItem("ranging", LoRAMacaddress1002, macaddress0340, 55, voltage, gatewayid);
    mMQTTqueue.add(mqttitemRanging2);

    sleep(1);
    MQTTDataItem * mqttitemRanging3 = new MQTTDataItem("ranging", LoRAMacaddress1002, macaddress0035, 60, voltage, gatewayid);
    mMQTTqueue.add(mqttitemRanging3);

    sleep(1);
    MQTTDataItem * mqttitemRanging4 = new MQTTDataItem("ranging", LoRAMacaddress1002, macaddress2012, 65, voltage, gatewayid);
    mMQTTqueue.add(mqttitemRanging4);

    sleep(1);
    MQTTDataItem * mqttitemRanging5 = new MQTTDataItem("ranging", LoRAMacaddress1002, macaddress0020, 70, voltage, gatewayid);
    mMQTTqueue.add(mqttitemRanging5);

    sleep(1);
    MQTTDataItem * mqttitemRanging6 = new MQTTDataItem("ranging", LoRAMacaddress1002, macaddress1302, 75, voltage, gatewayid);
    mMQTTqueue.add(mqttitemRanging6);

    sleep(15);

    MQTTDataItem * mqttitemRanging7 = new MQTTDataItem("ranging", LoRAMacaddress1002, macaddress0340, 555, voltage, gatewayid);
    mMQTTqueue.add(mqttitemRanging7);

    sleep(1);
    MQTTDataItem * mqttitemRanging8 = new MQTTDataItem("ranging", LoRAMacaddress1002, macaddress0035, 560, voltage, gatewayid);
    mMQTTqueue.add(mqttitemRanging8);
*/

}

void SimulationService::test_simulation_lora_rollcall()
{
    char macaddress0302[32];
    char macaddress0025[32];
    char macaddress0135[32];
    char LoRAMacaddress0001[32];
    char LoRAMacaddress0002[32];
    char LoRAMacaddress0004[32];
    char LoRAMacaddress0007[32];
    char LoRAMacaddress2008[32];
    char voltage[256];
    memset(voltage, 0, 256);
    snprintf(voltage, 256, "3.0");
    //int ranging = 50;

    sprintf(macaddress0302, "0421000000000302");
    sprintf(macaddress0025, "0410000000000025");
    sprintf(macaddress0135, "0310000000000135");


    sprintf(LoRAMacaddress0001, "0000000000050001");
    sprintf(LoRAMacaddress0002, "0000000000050002");
    sprintf(LoRAMacaddress0004, "0000000000050004");
    sprintf(LoRAMacaddress0007, "0000000000050007");
    sprintf(LoRAMacaddress2008, "0000000000052008");

    int projectid = 1;
    LoRaGatewayInfoStruct geteway;
    char gatewayid[64];
    snprintf(gatewayid, 64, "00001c497bcc6f73");

    geteway.projectid = projectid;
    geteway.sf = 10;
    geteway.gwip = "192.168.99.111";
    geteway.gwid = gatewayid;
    geteway.repeater = "00000000ffffffff";
    geteway.channels.push_back("923400000");
    geteway.channels.push_back("923800000");
    geteway.channels.push_back("924200000");
    geteway.channels.push_back("924000000");
    geteway.channels.push_back("923000000");
    geteway.channels.push_back("923600000");
    geteway.channels.push_back("922800000");

    char timestamp[64];
    // snprintf(timestamp, 64, "2019-05-23T07:15:51");
    snprintf(timestamp, 64, "%s", currentDateTimeForSQL().c_str());

    int rssi = 0;
    int velocity = 1;

    MQTTDataItem * mqttitem = new MQTTDataItem("loragateway", 1, &geteway);
    mMQTTqueue.add(mqttitem);

    sleep(5);

    for(int i=0;i<10;i++)
    {
        //ranging = 50;
        //mMQTTqueue.add(new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0004, macaddress0302, 50, voltage, velocity, gatewayid, rssi, timestamp));
        //mMQTTqueue.add(new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0004, macaddress0025, 50, voltage, velocity, gatewayid, rssi, timestamp));

        // 病房
        // mMQTTqueue.add(new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0001, macaddress0135, 250, voltage, velocity, gatewayid, rssi, timestamp));
        // sleep(1);

        // 電子圍欄
        mMQTTqueue.add(new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress2008, macaddress0135, 110, voltage, velocity, gatewayid, rssi, timestamp));
        sleep(1);

    }

    // simulation_point_behavior(LoRAMacaddress0013, macaddress0026, 320, 30, 5, 30, 5);

    sleep(30);

    for(int i=0;i<10;i++)
    {
        // mMQTTqueue.add(new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0007, macaddress0025, 110, voltage, velocity, gatewayid, rssi, timestamp));
        // mMQTTqueue.add(new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0007, macaddress0302, 110, voltage, velocity, gatewayid, rssi, timestamp));

        // 病床
        // mMQTTqueue.add(new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress0002, macaddress0135, 110, voltage, velocity, gatewayid, rssi, timestamp));
        // sleep(1);

        // 電子圍欄
        mMQTTqueue.add(new MQTTDataItem("rangingcompress", projectid, LoRAMacaddress2008, macaddress0135, 350, voltage, velocity, gatewayid, rssi, timestamp));
        sleep(1);
    }
    //sleep(15);

    //MQTTDataItem * mqttitemRanging33 = new MQTTDataItem("ranging", LoRAMacaddress0013, macaddress0026, 50, voltage);
    //mMQTTqueue.add(mqttitemRanging33);

    sleep(30);

    //
    // MQTTDataItem * mqttitemRanging3 = new MQTTDataItem("ranging", LoRAMacaddress0054, macaddress0035, 250, voltage);
    // mMQTTqueue.add(mqttitemRanging3);
    //
    // sleep(5);
    //
    // MQTTDataItem * mqttitemRanging4 = new MQTTDataItem("ranging", LoRAMacaddress0054, macaddress0035, 350, voltage);
    // mMQTTqueue.add(mqttitemRanging4);
    //
    // sleep(5);

}


void SimulationService::test_simulation_lora_GPS()
{
    char LoRAMacaddress0013[32];
    char macaddress0002[32];

    sprintf(LoRAMacaddress0013, "0000000000050013");
    sprintf(macaddress0002, "0310000000000002");

    vilslog::printf("SimulationService::test_simulation_lora_GPS()\n");
    //
    // GPS Ingo
    //
    double Latitude = 22.664460;
    double Longitude = 120.306317;
    //https://maps.google.com/?ll=22.664460,120.306317
    //https://www.google.com/maps/@22.66446,120.306317
    //http://www.google.com/maps/place/22.664460,120.306317

    sleep(5);

    while(1)
    {
        Latitude = 22.664291;
        Longitude = 120.306833;
        send_MQTT_LoRA_GPS(LoRAMacaddress0013, macaddress0002, Longitude, Latitude);
        sleep(5);

        // Latitude = 0.0;
        // Longitude = 0.0;
        // send_MQTT_LoRA_GPS(LoRAMacaddress0013, macaddress0002, Longitude, Latitude);
        // sleep(15);
        //
        // Latitude = -1.0;
        // Longitude = -1.0;
        // send_MQTT_LoRA_GPS(LoRAMacaddress0013, macaddress0002, Longitude, Latitude);
        // sleep(35);

        Latitude = 22.664244;
        Longitude = 120.307021;
        send_MQTT_LoRA_GPS(LoRAMacaddress0013, macaddress0002, Longitude, Latitude);
        sleep(5);

        Latitude = 22.664105;
        Longitude = 120.307120;
        send_MQTT_LoRA_GPS(LoRAMacaddress0013, macaddress0002, Longitude, Latitude);
        sleep(5);

        Latitude = 22.663954;
        Longitude = 120.307195;
        send_MQTT_LoRA_GPS(LoRAMacaddress0013, macaddress0002, Longitude, Latitude);
        sleep(5);

        Latitude = 22.663848;
        Longitude = 120.307201;
        send_MQTT_LoRA_GPS(LoRAMacaddress0013, macaddress0002, Longitude, Latitude);
        sleep(5);

        Latitude = 22.663764;
        Longitude = 120.307083;
        send_MQTT_LoRA_GPS(LoRAMacaddress0013, macaddress0002, Longitude, Latitude);
        sleep(5);

        Latitude = 22.664761;
        Longitude = 120.306919;
        send_MQTT_LoRA_GPS(LoRAMacaddress0013, macaddress0002, Longitude, Latitude);
        sleep(5);

        //tag alert
        // MQTTDataItem * mqttitem1 = new MQTTDataItem("alert", LoRAMacaddress, macaddress1);
        // mMQTTqueue.add(mqttitem1);
        //DataItem * item = new DataItem("alert", macaddress1, NODE_TYPE_TAG);
        //mUDPJobQueue.add(item);


        Latitude = 22.664768;
        Longitude = 120.306747;
        send_MQTT_LoRA_GPS(LoRAMacaddress0013, macaddress0002, Longitude, Latitude);
        sleep(5);

        Latitude = 22.663964;
        Longitude = 120.306779;
        send_MQTT_LoRA_GPS(LoRAMacaddress0013, macaddress0002, Longitude, Latitude);
        sleep(5);

        Latitude = 22.664164;
        Longitude = 120.306817;
        send_MQTT_LoRA_GPS(LoRAMacaddress0013, macaddress0002, Longitude, Latitude);
        sleep(5);

        // Latitude = 0.0;
        // Longitude = 0.0;
        // send_MQTT_LoRA_GPS(LoRAMacaddress0013, macaddress2012, Longitude, Latitude);
        //mDeviceManager->setTagInfoS(18, 100, 100, 100, 1);
        sleep(20);
    }
}

void SimulationService::test_simulation_lora_0D_exclude_rollcall_and_GPS()
{
    char LoRAMacaddress0013[32];
    char LoRAMacaddress0020[32];
    char macaddress0020[32];
    static int rangingValue = 400;
    char voltage[256];
    memset(voltage, 0, 256);
    snprintf(voltage, 256, "3.0");

    sprintf(LoRAMacaddress0013, "0000000000050013");
    sprintf(LoRAMacaddress0020, "0000000000050020");
    sprintf(macaddress0020, "0610000000000020");
    //
    // GPS Ingo
    //
    double Latitude = 22.664460;
    double Longitude = 120.306317;

    char gatewayid[64];
    snprintf(gatewayid, 64, "00001c497bcc6f73");
    char timestamp[64];
    // snprintf(timestamp, 64, "2019-05-23T07:15:51");
    snprintf(timestamp, 64, "%s", currentDateTimeForSQL().c_str());

    int rssi = 0;

    sleep(5);

    MQTTDataItem * mqttitemRanging1 = new MQTTDataItem("ranging", 1, LoRAMacaddress0013, macaddress0020, 50, voltage, gatewayid, rssi, timestamp);
    mMQTTqueue.add(mqttitemRanging1);

    sleep(15);

    MQTTDataItem * mqttitemRanging2 = new MQTTDataItem("ranging", 1, LoRAMacaddress0013, macaddress0020, rangingValue, voltage, gatewayid, rssi, timestamp);
    mMQTTqueue.add(mqttitemRanging2);

    sleep(10);

    Latitude = 22.664291;
    Longitude = 120.306833;
    send_MQTT_LoRA_GPS(LoRAMacaddress0020, macaddress0020, Longitude, Latitude);
    sleep(15);

}

void SimulationService::test_simulation_WebSocket()
{
    // while(1)
    // {
//         // sleep(10);
//         // glDeviceManager->setTagAlive(0, "3.95", "37.8");
//         // sleep(1);
//         // glDeviceManager->setTagAlive(0, "3.92", "37.8");
//         // sleep(1);
//         // glDeviceManager->setTagAlive(0, "3.83", "37.8");
//         // sleep(1);
//         // glDeviceManager->setTagAlive(0, "3.81", "37.8");
//         // sleep(1);
//         // glDeviceManager->setTagAlive(0, "3.75", "37.8");
//         // sleep(1);
//         // glDeviceManager->setTagAlive(0, "3.7", "37.8");
//         // sleep(1);
//         // glDeviceManager->setTagAlive(0, "3.6", "37.8");
//         // sleep(1);
//         // glDeviceManager->setTagAlive(0, "3.60", "37.8");
//         // sleep(1);
//         // glDeviceManager->setTagAlive(0, "3.62", "37.8");
//
//         sleep(10);
//         glDeviceManager->setTagAlive(0, "3.95", "37.8");
//         glDeviceManager->setTagInfoArea(0, "area4567", "4F辦公室125", 0);
//         sleep(1);
//         glDeviceManager->setTagAlive(0, "3.95", "37.8");
//         glDeviceManager->setTagInfoArea(0, "area4567", "4F辦公室125", 0);
//         sleep(1);
//         glDeviceManager->setTagAlive(0, "3.95", "37.8");
//         glDeviceManager->setTagInfoArea(0, "area4567", "4F辦公室125", 0);
//         sleep(1);
//         glDeviceManager->setTagAlive(0, "3.95", "37.8");
//         glDeviceManager->setTagInfoArea(0, "", "", 0);
//         sleep(1);
//         glDeviceManager->setTagAlive(0, "3.95", "37.8");
//         glDeviceManager->setTagInfoArea(0, "", "", 0);
//         sleep(1);
//         glDeviceManager->setTagAlive(0, "3.95", "37.8");
//         glDeviceManager->setTagInfoArea(0, "", "", 0);
//         sleep(1);
//         glDeviceManager->setTagAlive(0, "3.95", "37.8");
//         glDeviceManager->setTagInfoArea(0, "area7890", "4F辦公室010", 0);
//         sleep(1);
//         glDeviceManager->setTagAlive(0, "3.95", "37.8");
//         glDeviceManager->setTagInfoArea(0, "area7890", "4F辦公室010", 0);
//         sleep(1);
//         glDeviceManager->setTagAlive(0, "3.95", "37.8");
//         glDeviceManager->setTagInfoArea(0, "", "", 0);
//
//         // sleep(10);
        int projectID = 1;
//        int alertType = 1; //1:人員 2:固定裝置
//         // SubscribeDataItem * item1 = new SubscribeDataItem("sos", "0410000000000011", "Tag011", projectID, alertType, "area4567", "4F辦公室125");
//         // glWebSocketJobQueue.add(item1);
//         //
//         // sleep(3);
//         // SubscribeDataItem * item2 = new SubscribeDataItem("power", "0410000000000011", "Tag011", projectID, "3.9", "37.8");
//         // glWebSocketJobQueue.add(item2);
//         //
//         // sleep(3);
//         // SubscribeDataItem * item3 = new SubscribeDataItem("powerAlert", "0410000000000011", "Tag011", projectID, "37.0", "37.1");
//         // glWebSocketJobQueue.add(item3);
//         //
        // sleep(10);
        // alertType = 1; //1:人員 2:固定裝置
        // int alertAction = 1; // 1:in 2:out
        // SubscribeDataItem * item4 = new SubscribeDataItem("fenceAlert", "0410000000000012", "Tag012", projectID, alertType, alertAction, "area2107", "Area 4");
        // mWebSocketqueue.add(item4);

        sleep(3);
        SubscribeDataItem * item5 = new SubscribeDataItem("coord", 0, "Tag0026", "0410000000000026", "Tag0026", projectID, 100, 200, 300, "3.6", "37.8", "area4567", "4F辦公室125");
        mWebSocketqueue.add(item5);

        sleep(5);
        mDeviceManager->setTagAlive(1, 0, "3.7", "37.8");
        //mDeviceManager->setTagInfoArea(0, "", "", 0);
//
    // }
}

void SimulationService::test_simulation_MQTT_data()
{
    //--- filenames are unique so we can use a set
    std::set<std::filesystem::path> sorted_by_name;

    sleep(2);

    // std::string path = "/Volumes/MacintoshHD-Data/Data/DecaWave/SMIMS/中油林園/LoRA";
    // std::string path = "/Volumes/MacintoshHD-Data/Data/DecaWave/SMIMS/中油林園/video/LoRA1"; // 20210820_112522.mp4
    // std::string path = "/Volumes/MacintoshHD-Data/Data/DecaWave/SMIMS/中油林園/video/LoRA2";
    // std::string path = "/Volumes/MacintoshHD-Data/Data/DecaWave/SMIMS/中油林園/video/LoRA3";
    // std::string path = "/Volumes/MacintoshHD-Data/Data/DecaWave/SMIMS/MacOS/AoAWifi";
    // std::string path = "/Volumes/MacintoshHD-Data/Data/DecaWave/SMIMS/MacOS/AoAWifi_0006/AoA0006";
    // std::string path = "/Volumes/MacintoshHD-Data/Data/DecaWave/SMIMS/中油林園/AoA0923_1240";
    std::string path = "/Volumes/MacintoshHD-Data/Data/DecaWave/SMIMS/中油林園/23-09-2021_12_38";


    vilslog::printf("SimulationService::test_simulation_MQTT_data() Satrt...\n");
    vilslog::printf("SimulationService::test_simulation_MQTT_data() file path[%s]\n", path.c_str());

    for (const auto & entry : std::filesystem::directory_iterator(path))
    {
        if(entry.path().extension() == ".bin")
        {
            sorted_by_name.insert(entry.path().filename());
        }
        // std::cout << entry.path().filename() << std::endl;
        // vilslog::printf("SimulationService::test_simulation_MQTT_data() filename[%s]\n", entry.path().filename().c_str());
    }

    int pre_time = 0;
    int pre_time_ms = 0;
    int count = 0;
    //--- print the files sorted by filename
    for (auto &filename : sorted_by_name)
    {
        // 20-08-2021_11_26_09.41169184.bin
        vilslog::printf("SimulationService::test_simulation_MQTT_data() filename[%s]\n", filename.c_str());


        do_parse_compress_AOA_file(path, filename);


        std::vector<std::string> strVector1 = convert_string_to_vector(filename, '.');

        int cur_time = 0;
        int cur_time_ms = 0;

        if (strVector1.size() >= 3)
        {
            std::vector<std::string> strVector2 = convert_string_to_vector(strVector1[0], '_');

            if (strVector2.size() >= 3)
            {
                int hour = convert_string_to_int(strVector2[1]);
                int min = convert_string_to_int(strVector2[2]);
                int second = convert_string_to_int(strVector2[3]);
                int ms = convert_string_to_int(strVector1[1])/100000;

                cur_time = hour * 3600 + min * 60 + second;
                cur_time_ms = ms;

                vilslog::printf("SimulationService::test_simulation_MQTT_data() [%d:%d:%d.%d]\n",
                    hour, min, second, ms);
            }
            strVector2.clear();
        }
        strVector1.clear();

        if (pre_time == 0)
        {
            pre_time = cur_time;
            pre_time_ms = cur_time_ms;
        }

        int diff = cur_time - pre_time;
        int diff_ms = cur_time_ms - pre_time_ms;

        pre_time = cur_time;
        pre_time_ms = cur_time_ms;

        // wait next time
        int timeOffset = diff * 1000 + diff_ms;

        vilslog::printf("SimulationService::test_simulation_MQTT_data() wait timeOffset %d ms \n", timeOffset);

        if (timeOffset > 0)
            usleep(timeOffset * 1000);

        count++;
    }

    vilslog::printf("SimulationService::test_simulation_MQTT_data() Finished!\n");

    sleep(30);
}

void SimulationService::test_simulation_geteway()
{

    LoRaGatewayInfoStruct geteway;
    geteway.sf = 10;
    geteway.projectid = 1;
    geteway.gwip = "10.1.1.14";
    geteway.gwid = "00001c497bf88362";
    geteway.repeater = "00000000ffffffff";
    geteway.channels.push_back("922800000");
    geteway.DataTime = "2019-04-21T09:01:48";

    MQTTDataItem * mqttitem = new MQTTDataItem("loragateway", 1, &geteway);
    mMQTTqueue.add(mqttitem);

    sleep(10);

    geteway.channels.clear();
    geteway.channels.push_back("924000000");
    MQTTDataItem * mqttitem1 = new MQTTDataItem("loragateway", 1, &geteway);
    mMQTTqueue.add(mqttitem1);

    sleep(10);

    geteway.channels.clear();
    geteway.channels.push_back("923200000");
    MQTTDataItem * mqttitem2 = new MQTTDataItem("loragateway", 1, &geteway);
    mMQTTqueue.add(mqttitem2);
}

void SimulationService::send_MQTT_LoRA_GPS(const char * LoRAMacaddress, const char * macaddress1, double Longitude, double Latitude)
{

    char gatewayid[64];
    snprintf(gatewayid, 64, "00001c497bcc6f73");
    char timestamp[64];
    // snprintf(timestamp, 64, "2019-05-23T07:15:51");
    snprintf(timestamp, 64, "%s", currentDateTimeForSQL().c_str());


    MQTTDataItem * mqttitem = new MQTTDataItem("taggps", 1, LoRAMacaddress, macaddress1, Longitude, Latitude, gatewayid, timestamp);
    mMQTTqueue.add(mqttitem);

    vilslog::printf ("SimulationService::send_MQTT_LoRA_GPS() taggps LoRAMacaddress[%s] Longitude[%f] Latitude[%f] timestamp[%s]\n",
        LoRAMacaddress, Longitude, Latitude, timestamp);

}

void SimulationService::create_alert_event(const char * tagmacadddress, const char * tagid)
{
    int projectid = 1;
    const char * areaid = mDeviceManager->getTagAreaId(projectid, tagid);
    if ( strlen(areaid) > 0 )
    {
        vilslog::printf("SimulationService::create_alert_event() TagAreaId[%s]\n", areaid);
        mDeviceManager->setNodeAlertByType(projectid, tagmacadddress, NODE_TYPE_TAG);

        MySQLDBAccess * mysqldb = new MySQLDBAccess(projectid);
        std::string eventjson = convert_to_json_str(projectid, 9999, tagid, "Alert", areaid, "0", "");
        mysqldb->write_fired_events(projectid, 9999, 1,
            currentDateTimeForSQL().c_str(),
            currentDateTimeForSQL().c_str(),
            eventjson);
        delete mysqldb;
    }
}

void SimulationService::create_alert_event_no_area(int projectid, const char * tagmacadddress, const char * tagid)
{
    mDeviceManager->setNodeAlertByType(projectid, tagmacadddress, NODE_TYPE_TAG);

    MySQLDBAccess * mysqldb = new MySQLDBAccess(projectid);
    std::string eventjson = convert_to_json_str(projectid, 9999, tagid, "Alert", "", "0", "");
    mysqldb->write_fired_events(projectid, 9999, 1,
        currentDateTimeForSQL().c_str(),
        currentDateTimeForSQL().c_str(),
        eventjson);
    delete mysqldb;

    std::string message = tagid;
    message.append(" 按下緊急按鈕(UWB)");

    NotificationItem * item = new NotificationItem(TAG_PUSHNOTIFY_FUNCTION_2, projectid, NODE_TYPE_TAG, tagid, tagid, "0", "1", tagmacadddress, "SMIMSVILS 緊急求助通知", message);
    mNotifiactionQueue.add(item);
}

void SimulationService::add_action_join(const char* macaddress, int nodetype, int idx, const char* version)
{
    DataItem * itemT = new DataItem("join", 1, macaddress, nodetype, version);
    mUDPJobQueue.add(itemT);
}

int SimulationService::do_parse_compress_AOA_file(std::string path, std::string filename)
{
    std::string csvfilepath = path;
    csvfilepath.append("/");
    csvfilepath.append(filename);

    std::ifstream infile(csvfilepath);

    std::string line;

    std::getline(infile, line);

    // vilslog::printf ("SimulationService::do_parse_compress_AOA_file() line[%s]\n", line.c_str());

    Json::Value JSONresult;
    try{

        Json::Reader * reader = new Json::Reader();
        bool parsingSuccessful = reader->parse( line, JSONresult);
        delete reader;

        if (parsingSuccessful && JSONresult.isArray())
        {
            int projectid = 1;
            std::string msg_type = "";
            std::string gwid = "";
            std::string macAddr = "";
            std::string timestamp = currentDateTimeForSQL();
            for(unsigned int i=0;i<JSONresult.size();i++)
            {
                int rssi = 0;
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
                if ( item.isMember("rssi") )
                {
                    rssi = item["rssi"].asInt();
                }
                if ( item.isMember("time") )
                {
                    timestamp = item["time"].asString();
                }

                if ( item.isMember("data") )
                {
                    const char * jsondata = item["data"].asCString();

                    std::string jsonString = HEXStringToString(jsondata);

                    // vilslog::printf ("SimulationService::do_parse_compress_AOA_file() jsondata[%s]\n", jsondata);

                    if ( strlen(jsondata) == 1 && strcmp(jsondata, "0") == 0)
                    {
                        vilslog::printf ("\n\nSimulationService::do_parse_compress_AOA_file() jsondata[%s] jsonString[%s]\n\n", jsondata, jsonString.c_str());
                    }

                    // if (gwid.length()  > 0)
                    // {
                    //     send_LoRaDeviceALIVE_to_projects(macAddr.c_str(), gwid.c_str(), rssi, 0);
                    // }

                    // if (jsondata[0] == '3' && jsondata[1] == 'c')
                    // {
                        do_parse_compress_AOA_data(projectid, macAddr.c_str(), jsondata, gwid.c_str(), rssi, timestamp.c_str());
                    // }
                }

            }
        }
    }
    catch(...)
    {
        vilslog::printf ("SimulationService::do_parse_compress_AOA_file() catch exception...\n");
    }

    return 0;
}

int SimulationService::do_parse_compress_AOA_data(int projectid, const char * LoRAMacAddr, const char * data, const char * gatewayid, int rssi, const char * timestamp)
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

    while (nextValue.compare("2b") != 0)// 0x2b = '+'
    {
        start_pos = macEndIdx[commonMacCnt - 1] + 2;
        macStIdx.push_back(start_pos);
        commonMacCnt += 1;
        std::string::size_type tmpEndIdx = rawData.find(macEndPattern, start_pos);

        if (tmpEndIdx == std::string::npos)
            return 1;

        macEndIdx.push_back(tmpEndIdx);

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
    }

    int distanceStIdx = (int)macEndIdx[commonMacCnt - 1] + (4*2);
    int distanceEndIdx = distanceStIdx + (int)(ceil((float)(tagAmount*12)/8.0)*2);
    std::string distanceStr = rawData.substr(distanceStIdx, distanceEndIdx - distanceStIdx);

    int velStIdx = distanceEndIdx;
    int velEndIdx = velStIdx + ((tagAmount * 1) * 2);
    std::string velList = rawData.substr(velStIdx, velEndIdx - velStIdx);

    int voltageStIdx = velEndIdx;
    int voltageEndIdx = voltageStIdx + (int)(ceil((float)(tagAmount*4)/8.0)*2);
    std::string voltageStr = rawData.substr(voltageStIdx, voltageEndIdx - voltageStIdx);

    int angleStIdx = voltageEndIdx + (3*2);
    int angleEndIdx = angleStIdx + ((tagAmount * 2) * 2);
    std::string angleStr = rawData.substr(angleStIdx, angleEndIdx - angleStIdx);

    int tagIdx = 0;
    for(int i=0;i<commonMacCnt;i++)
    {
        std::string::size_type start_idx = macStIdx[i];
        std::string::size_type end_idx = macEndIdx[i];
        std::string tagValue = rawData.substr(start_idx, end_idx - start_idx);
        int tagCnt = (int)(tagValue.length()/2 - 8)/2;
        tagCnt++;

        std::string tagPrefixMac = rawData.substr(start_idx, 6*2);

        for(int j=0;j<tagCnt;j++)
        {
            std::string tagMacaddress = tagPrefixMac;
            tagMacaddress.append(rawData.substr(start_idx + (6*2) + (4*j), 4));

            std::string distanceStr = rawData.substr(distanceStIdx + tagIdx*3, 3);
            int distance = (int)parseInt(distanceStr, 16);

            std::string voltageStr = rawData.substr(voltageStIdx + tagIdx*1, 1);
            int voltageInt = (int)parseInt(voltageStr, 16)+29;
            float voltageFloat = (float)(voltageInt)/10.0;

            char voltage[256];
            memset(voltage, 0, 256);
            sprintf(voltage, "%.01f", voltageFloat);

            std::string velValue = rawData.substr(velStIdx + (tagIdx*2), 2);
            int velocity = (int)strtol(velValue.c_str(), NULL, 16);

            std::string angleStr = rawData.substr(angleStIdx + (tagIdx*4), 4);
            int angleRawValue = (int)strtol(angleStr.c_str(), NULL, 16);
            angleRawValue -= 157;
            float angleFloat = (float)(angleRawValue)/100.0;

            //
            // send out data
            //
            send_rangingcompressaoa_to_projects(LoRAMacAddr, tagMacaddress.c_str(), distance, voltage, velocity, angleFloat, gatewayid, rssi, timestamp);

            // MQTTDataItem * mqttitem = new MQTTDataItem("rangingcompressaoa", projectid, LoRAMacAddr, tagMacaddress.c_str(), distance, voltage, velocity, angleFloat, gatewayid, rssi, timestamp);
            // glqueue->add(mqttitem);

            tagIdx++;
        }
    }

    return 0;
}

void SimulationService::send_LoRaDeviceALIVE_to_projects(const char * LoRAMacAddr, const char * gatewayid, int rssi, int AoAGridSize)
{
    MQTTDataItem * mqttitem = new MQTTDataItem("LoRaDeviceALIVE", 1, LoRAMacAddr);
    mqttitem->setGatewayid(gatewayid);
    mqttitem->setRSSI(rssi);
    mqttitem->setAOA_gridsize(AoAGridSize);
    mMQTTqueue.add(mqttitem);
}

void SimulationService::send_rangingcompressaoa_to_projects(const char * LoRAMacAddr, const char * macaddress, int distance, const char * voltage, int velocity, float angleFloat, const char * gatewayid, int rssi, const char * timestamp)
{
    std::string gatewayidString = gatewayid;

    MQTTDataItem * mqttitem = new MQTTDataItem("rangingcompressaoa", 1, LoRAMacAddr, macaddress, distance, voltage, velocity, angleFloat, gatewayid, rssi, timestamp);
    mMQTTqueue.add(mqttitem);

}

// void SimulationService::test_mysql_ts()
// {
//     TimeSerialsDBAccess * mysqltsdb = new TimeSerialsDBAccess();
//
//     //mysqltsdb->run_test();
//     //mysqltsdb->analyzeFootprintall(NODE_TYPE_TAG, "Tag007");
//     //mysqltsdb->analyzeFootprintall(NODE_TYPE_TAG, "Tag008");
//     //mysqltsdb->analyzeFootprintall(NODE_TYPE_TAG, "Tag009");
//     //mysqltsdb->analyzeFootprintall(NODE_TYPE_TAG, "Tag340");
//
//     //mysqltsdb->analyze_all("Tag340");
//     //mysqltsdb->analyze_one_day("Tag007", "2018-02-02");
//
//     delete mysqltsdb;
// }
