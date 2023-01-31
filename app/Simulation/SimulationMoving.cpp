/*
 * SimulationMoving.cpp
 *
 *  Created on: 2018/05/09
 *      Author: ikki
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>

#include "SimulationMoving.h"
#include "../common.h"
#include "../CSV/CSVRow.h"
#include "../PosCalculate/algorithm.h"


#define MAX_RANGING_INDEX 0xFF

SimulationMoving::SimulationMoving(DeviceManager * pDevManager, wqueue<DataItem*> & queue, wqueue<DataItem*> & PosCalJobQueue)
    : mDevManager(pDevManager), m_queue(queue), mPosCalJobQueue(PosCalJobQueue)
{
    //m_tagIndex = 0;
}

SimulationMoving::~SimulationMoving()
{
}

std::istream& operator>>(std::istream& str, CSVRow& data)
{
    data.readNextRow(str);
    return str;
}

void SimulationMoving::start()
{

    //test_AUO_loop();

    // taipei tag node
    // add_action_join("0410000000000025", NODE_TYPE_TAG, -1, "4.13.0");
    // add_action_join("0410000000000007", NODE_TYPE_TAG, -1, "4.13.0");
    // add_action_join("4210000000001311", NODE_TYPE_TAG, -1, "4.13.0");
    // add_action_join("4210000000000013", NODE_TYPE_TAG, -1, "4.13.0");
    // add_action_join("0310000000000135", NODE_TYPE_TAG, -1, "4.12.12");
    // add_action_join("0310000000000002", NODE_TYPE_TAG, -1, "4.13.0");
    // add_action_join("0410000000000029", NODE_TYPE_TAG, -1, "4.13.0");
    // add_action_join("4210000000000022", NODE_TYPE_TAG, -1, "4.13.0");
    // add_action_join("0410000000000018", NODE_TYPE_TAG, -1, "4.13.0");
    // add_action_join("0610000000000128", NODE_TYPE_TAG, -1, "4.13.0");
    // add_action_join("0310000000000145", NODE_TYPE_TAG, -1, "4.13.0");
    // add_action_join("0310000000000015", NODE_TYPE_TAG, -1, "4.13.0");
    // add_action_join("0310000000000150", NODE_TYPE_TAG, -1, "4.13.0");
    // add_action_join("4210000000001195", NODE_TYPE_TAG, -1, "4.13.0");
    // add_action_join("4210000000001200", NODE_TYPE_TAG, -1, "4.13.0");

    // add_action_join("4210000000000004", NODE_TYPE_TAG, -1, "4.12.12");
    // add_action_join("4210000000000010", NODE_TYPE_TAG, -1, "4.12.12");
    // add_action_join("4210000000000314", NODE_TYPE_TAG, -1, "4.12.12");
    // add_action_join("4210000000001196", NODE_TYPE_TAG, -1, "4.12.12");

    // add_action_join("0320000000000055", NODE_TYPE_ANCHOR, -1, "4.12.10");
    // add_action_join("0320000000000056", NODE_TYPE_ANCHOR, -1, "4.12.10");
    // add_action_join("0320000000000066", NODE_TYPE_ANCHOR, -1, "4.12.10");
    // add_action_join("0320000000000075", NODE_TYPE_ANCHOR, -1, "4.12.10");
    // add_action_join("3131000000000014", NODE_TYPE_ANCHOR, -1, "4.12.10");
    // add_action_join("3131000000000040", NODE_TYPE_ANCHOR, -1, "4.12.10");

    // add_action_join("4210000000001300", NODE_TYPE_TAG, -1, "42.12.12");
    // add_action_join("4210000000001312", NODE_TYPE_TAG, -1, "42.12.12");
    // add_action_join("4210000000000003", NODE_TYPE_TAG, -1, "42.12.12");
    //
    // add_action_join("4210000000001181", NODE_TYPE_TAG, -1, "42.12.12");
    // add_action_join("4210000000001182", NODE_TYPE_TAG, -1, "42.12.12");
    // add_action_join("4210000000001183", NODE_TYPE_TAG, -1, "42.12.12");
    // add_action_join("4210000000001184", NODE_TYPE_TAG, -1, "42.12.12");
    // add_action_join("4210000000001185", NODE_TYPE_TAG, -1, "42.12.12");
    // add_action_join("4210000000001186", NODE_TYPE_TAG, -1, "42.12.12");
    // add_action_join("4210000000001187", NODE_TYPE_TAG, -1, "42.12.12");
    // add_action_join("4210000000001188", NODE_TYPE_TAG, -1, "42.12.12");
    // add_action_join("4210000000001189", NODE_TYPE_TAG, -1, "42.12.12");
    // add_action_join("4210000000001190", NODE_TYPE_TAG, -1, "42.12.12");

    //add_action_join("0410000000000340", NODE_TYPE_TAG, 2, "2.10.0");
    //add_action_join("0410000000000002", NODE_TYPE_TAG, TAG_INDEX+1, "2.10.0");
    //test_ranging_loop();

    //mDevManager->setTagVoltage(1, "0610000000009914", "3.3");
    //test_doctor_enter_room();

    //test_1d_ranging_loop();
    //test_fixes_ranging_loop();

    //test_multiple_tag_loop_ihome();
    //test_ranging_loop_ihome();

    //test_ranging_loop_kaohsiungOffice();
    //test_ranging_loop_kaohsiungOffice(0, 1);

    //test_ranging_loop_checkpoint();
    //test_ranging_loop_taipei_checkpoint();
    // test_ranging_loop_taf_checkpoint();

    //test_ranging_loop_taipei_1D();
    // int totalTime = 10;//  seconds
    // int marginTime = 100;//100 ms
    // int thresholds = 30;//5 meter

    do_soccer_random_move();

    sleep(10);

    while(1)
    {
        // sleep(60);

        // 0923 中油林園 Video 1
        // read_anchor_csv_File("/Volumes/MacintoshHD-Data/Data/DecaWave/SMIMS/中油林園/RangingLog/Log_2021-09-29_12-47-53_anchors.csv");
        // read_from_csv_File("/Volumes/MacintoshHD-Data/Data/DecaWave/SMIMS/中油林園/RangingLog/Log_2021-09-29_13-00-00_ranging.csv");
        sleep(10);

        // 0923 中油林園 Video 2
        // read_anchor_csv_File("/Volumes/MacintoshHD-Data/Data/DecaWave/SMIMS/中油林園/RangingLog/Log_2021-09-29_12-47-53_anchors.csv");
        // read_from_csv_File("/Volumes/MacintoshHD-Data/Data/DecaWave/SMIMS/中油林園/RangingLog/Log_2021-09-29_12-47-53_ranging.csv");
        // sleep(10);



        // // 中油林園 Video 1
        // read_anchor_csv_File("/Volumes/MacintoshHD-Data/Data/DecaWave/SMIMS/中油林園/video/Video1/log/Log_2021-08-23_10-48-08_anchors.csv");
        // read_from_csv_File("/Volumes/MacintoshHD-Data/Data/DecaWave/SMIMS/中油林園/video/Video1/log/Log_2021-08-23_10-48-08_ranging.csv");
        // sleep(10);

        // 中油林園 Video 2
        // read_anchor_csv_File("/Volumes/MacintoshHD-Data/Data/DecaWave/SMIMS/中油林園/video/Video2/log/Log_2021-08-23_16-33-57_anchors.csv");
        // read_from_csv_File("/Volumes/MacintoshHD-Data/Data/DecaWave/SMIMS/中油林園/video/Video2/log/Log_2021-08-23_16-33-57_ranging.csv");
        // sleep(10);

        // 中油林園 Video 3
        // read_anchor_csv_File("/Volumes/MacintoshHD-Data/Data/DecaWave/SMIMS/中油林園/video/Video3/log/Log_2021-08-24_11-28-56_anchors.csv");
        // read_from_csv_File("/Volumes/MacintoshHD-Data/Data/DecaWave/SMIMS/中油林園/video/Video3/log/Log_2021-08-24_11-28-56_ranging.csv");
        // sleep(10);

        // tapei office
        // read_anchor_csv_File("/Users/ikki/DecaWave/SMIMS/MacOS/test_data/Log_2020-02-21_09-39-45_anchors.csv");
        // read_from_csv_File("/Users/ikki/DecaWave/SMIMS/MacOS/test_data/Log_2020-02-21_09-39-46_ranging.csv");

        // sleep(180);

        // tapei office
        // read_anchor_csv_File("/Users/ikki/DecaWave/SMIMS/MacOS/test_data/Log_2020-02-07_13-12-58_anchors.csv");
        // read_from_csv_File("/Users/ikki/DecaWave/SMIMS/MacOS/test_data/Log_2020-02-07_13-12-58_ranging.csv");

        // tapei office
        // read_anchor_csv_File("/Users/ikki/DecaWave/SMIMS/MacOS/test_data/Log_2019-12-05_10-55-39_anchors.csv");
        // read_from_csv_File("/Users/ikki/DecaWave/SMIMS/MacOS/test_data/Log_2019-12-05_10-55-39_ranging.csv");
        // sleep(1200);


        // tainan
        //read_anchor_csv_File("/Users/ikki/DecaWave/SMIMS/MacOS/test_data/tainan/Log_2019-07-12_09-04-30_anchors.csv");
        //read_from_csv_File("/Users/ikki/DecaWave/SMIMS/MacOS/test_data/tainan/Log_2019-07-12_09-04-30_ranging.csv");

        // fill_ranging_floor("0410000000000007", 1000,1000,0, 1100,1100,0, totalTime, marginTime, thresholds, 1);

        // tapei office test 1D
        //read_anchor_csv_File("/Users/ikki/DecaWave/SMIMS/MacOS/test_data/Log_2019-08-07_17-56-57_anchors.csv");
        // read_from_csv_File("/Users/ikki/DecaWave/SMIMS/MacOS/test_data/Log_2019-08-07_17-56-57_ranging.csv");

        // tainan 小火車
        // read_anchor_csv_File("/Users/ikki/DecaWave/SMIMS/MacOS/test_data/Log_2019-08-08_13-59-23_anchors.csv");
        // read_from_csv_File("/Users/ikki/DecaWave/SMIMS/MacOS/test_data/Log_2019-08-08_13-59-23_ranging.csv");
        //
        // // tainan 小火車
        // read_anchor_csv_File("/Users/ikki/DecaWave/SMIMS/MacOS/test_data/Log_2019-08-08_14-09-43_anchors.csv");
        // read_from_csv_File("/Users/ikki/DecaWave/SMIMS/MacOS/test_data/Log_2019-08-08_14-09-43_ranging.csv");
        //
        // // tainan 小火車
        // read_anchor_csv_File("/Users/ikki/DecaWave/SMIMS/MacOS/test_data/Log_2019-08-08_14-18-01_anchors.csv");
        // read_from_csv_File("/Users/ikki/DecaWave/SMIMS/MacOS/test_data/Log_2019-08-08_14-18-01_ranging.csv");
        //
        // // tainan 小火車
        // read_anchor_csv_File("/Users/ikki/DecaWave/SMIMS/MacOS/test_data/Log_2019-08-08_14-28-45_anchors.csv");
        // read_from_csv_File("/Users/ikki/DecaWave/SMIMS/MacOS/test_data/Log_2019-08-08_14-28-45_ranging.csv");
        //
        // // tainan 小火車
        // read_anchor_csv_File("/Users/ikki/DecaWave/SMIMS/MacOS/test_data/Log_2019-08-08_14-40-40_anchors.csv");
        // read_from_csv_File("/Users/ikki/DecaWave/SMIMS/MacOS/test_data/Log_2019-08-08_14-40-40_ranging.csv");
        //
        // // tainan 小火車
        // read_anchor_csv_File("/Users/ikki/DecaWave/SMIMS/MacOS/test_data/Log_2019-08-08_14-54-07_anchors.csv");
        // read_from_csv_File("/Users/ikki/DecaWave/SMIMS/MacOS/test_data/Log_2019-08-08_14-54-07_ranging.csv");


        // 空總
        // read_anchor_csv_File("/Users/ikki/DecaWave/SMIMS/MacOS/test_data/Log_2019-07-02_16-32-32_anchors.csv");
        // read_from_csv_File("/Users/ikki/DecaWave/SMIMS/MacOS/test_data/Log_2019-07-02_16-32-32_ranging.csv");

        // powertag test data
        //read_from_csv_File("/Library/WebServer/Documents/log/Log_Tag005_Pos_2019-03-15_09_10_35.csv");

        // test_ranging_loop_ihome();
    //      //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-08-16_11-50-03_ranging.csv");
    //      //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-08-16_13-22-25_ranging.csv");
    //      //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-08-24_11-01-52_ranging.csv");
    //      //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-08-24_11-26-28_ranging.csv");
    //
    //      //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-08-24_15-23-42_ranging.csv");
    //      //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-08-24_15-34-26_ranging.csv");
    //      //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-08-24_15-48-50_ranging.csv");
    //      //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-08-24_16-05-44_ranging.csv");
    //      //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-08-29_10-10-00_ranging.csv");
    //      //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-08-29_10-10-00_ranging_testY.csv");
    //      //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-08-30_10-41-15_ranging.csv");
    //      //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-08-30_17-03-50_ranging.csv");
    //      //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-08-30_17-48-00_ranging.csv");
    //      //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-08-31_10-13-12_ranging.csv");
    //      //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-09-03_12-48-20_ranging.csv");
    //      //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-09-05_11-16-18_ranging.csv");
    //      //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-09-04_17-31-48_ranging.csv");
    //
    //
    //      // tag011 走 ihome 12F 全區
    //      //read_anchor_csv_File("/Library/WebServer/Documents/log/Log_2018-09-05_11-04-15_anchors.csv");
    //      //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-09-05_11-04-15_ranging.csv");
    //
    //      // 1個 tag 左右走
    //      //read_anchor_csv_File("/Library/WebServer/Documents/log/Log_2018-09-05_11-16-18_anchors.csv");
    //      //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-09-05_11-16-18_ranging.csv");
    //
    //      // 3個 tag 左右走
    //      //read_anchor_csv_File("/Library/WebServer/Documents/log/Log_2018-09-05_13-25-05_anchors.csv");
    //      //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-09-05_13-25-05_ranging.csv");
    //
    //      // 3個 tag 左右走
    //      //read_anchor_csv_File("/Library/WebServer/Documents/log/Log_2018-09-05_13-26-10_anchors.csv");
    //      //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-09-05_13-26-10_ranging.csv");
    //
    //      // 3個 tag 左右走
    //      //read_anchor_csv_File("/Library/WebServer/Documents/log/Log_2018-09-05_13-30-02_anchors.csv");
    //      //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-09-05_13-30-02_ranging.csv");
    //
    //      // 3個 tag 左右走
    //      //read_anchor_csv_File("/Library/WebServer/Documents/log/Log_2018-09-05_13-32-37_anchors.csv");
    //      //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-09-05_13-32-37_ranging.csv");
    //
    //      //read_anchor_csv_File("/Library/WebServer/Documents/log/Log_2018-09-05_13-40-00_anchors.csv");
    //      //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-09-05_13-40-00_ranging.csv");
    //
    //     //read_anchor_csv_File("/Library/WebServer/Documents/log/Log_2018-09-05_13-42-04_anchors.csv");
    //     //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-09-05_13-42-04_ranging.csv");
    //
    //       // 3個 tag 左右走
    //       //read_anchor_csv_File("/Library/WebServer/Documents/log/Log_2018-09-05_13-45-23_anchors.csv");
    //       //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-09-05_13-45-23_ranging.csv");
    //       // 2個 tag
    //       //read_anchor_csv_File("/Library/WebServer/Documents/log/Log_2018-09-05_13-49-27_anchors.csv");
    //       //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-09-05_13-49-27_ranging.csv");
    //
    //       // 1 tag 007
    //       //read_anchor_csv_File("/Library/WebServer/Documents/log/Log_2018-09-05_13-52-08_anchors.csv");
    //       //read_from_csv_File("/Library/WebServer/Documents/log/Log_2018-09-05_13-52-08_ranging.csv");
    }

}

void SimulationMoving::test_doctor_enter_room()
{
    int marginTime = 100;//100 ms
    int thresholds = 20;//20 meter
    char macaddress9914[32];
    sprintf(macaddress9914, "0610000000009914");

    sleep(10);

    while(1)
    {
/*
        //=================================
        // room 病房 100 outside
        fill_ranging_floor(macaddress9914, 870,  2630, 220, 870,  2630, 220, 10, marginTime, thresholds, 1);

        // enter room 病房 100
        fill_ranging_floor(macaddress9914, 870,  2630, 220, 870,  3090, 220, 10, marginTime, thresholds, 1);

        // enter room 病床 101
        fill_ranging_floor(macaddress9914, 870,  3090, 220, 580,  3090, 220, 10, marginTime, thresholds, 1);

        // stay room 病床 101
        fill_ranging_floor(macaddress9914, 580,  3090, 220, 580,  3090, 220, 20, marginTime, thresholds, 1);

        // leave room 病床 101
        fill_ranging_floor(macaddress9914, 580,  3090, 220, 870,  3090, 220, 10, marginTime, thresholds, 1);
        */
        // stay room 病房 100
        fill_ranging_floor(macaddress9914, 870,  3090, 220, 870,  3360, 220, 10, marginTime, thresholds, 1, 0);


        // enter room 病床 102
        fill_ranging_floor(macaddress9914, 870,  3360, 220, 580,  3360, 220, 10, marginTime, thresholds, 1, 0);

        // stay room 病床 102
        fill_ranging_floor(macaddress9914, 580,  3360, 220, 580,  3360, 220, 20, marginTime, thresholds, 1, 0);

        sleep(120);

        // leave room 病床 102
        fill_ranging_floor(macaddress9914, 580,  3360, 220, 870,  3360, 220, 10, marginTime, thresholds, 1, 0);

        // room 病房 100 outside
        fill_ranging_floor(macaddress9914, 870,  3360, 220, 870,  2630, 220, 10, marginTime, thresholds, 1, 0);
        //=================================



        //=================================
        // room 病房 200 outside
        fill_ranging_floor(macaddress9914, 870,  2630, 220, 1590,  2630, 220, 20, marginTime, thresholds, 1, 0);
/*
        // enter room 病房 200
        fill_ranging_floor(macaddress9902, 1590,  2630, 220, 1590,  3090, 220, 10, marginTime, thresholds, 1);

        // enter room 病床 201
        fill_ranging_floor(macaddress9902, 1590,  3090, 220, 1340,  3090, 220, 10, marginTime, thresholds, 1);

        // stay room 病床 201
        fill_ranging_floor(macaddress9902, 1340,  3090, 220, 1340,  3090, 220, 20, marginTime, thresholds, 1);

        // leave room 病床 201
        fill_ranging_floor(macaddress9902, 1340,  3090, 220, 1590,  3090, 220, 10, marginTime, thresholds, 1);
        fill_ranging_floor(macaddress9902, 1590,  3090, 220, 1590,  3360, 220, 10, marginTime, thresholds, 1);

        // enter room 病床 202
        fill_ranging_floor(macaddress9902, 1590,  3360, 220, 1340,  3360, 220, 10, marginTime, thresholds, 1);

        // stay room 病床 202
        fill_ranging_floor(macaddress9902, 1340,  3360, 220, 1340,  3360, 220, 20, marginTime, thresholds, 1);

        // leave room 病床 102
        fill_ranging_floor(macaddress9902, 1340,  3360, 220, 1590,  3360, 220, 10, marginTime, thresholds, 1);

        // room 病房 200 outside
        fill_ranging_floor(macaddress9902, 1590,  3360, 220, 1590,  2630, 220, 10, marginTime, thresholds, 1);
*/
        // room 病房 200 outside -> 病房 100 outside
        fill_ranging_floor(macaddress9914, 1590,  2630, 220, 870,  2630, 220, 10, marginTime, thresholds, 1, 0);
        //=================================

    }
}

void SimulationMoving::test_1d_ranging_loop()
{
    int IMU[6] = {0,0,-9,0,0,0};
    char TagMacaddress9905[32];
    char AnchorMacaddress0016[32];
    char AnchorMacaddress0019[32];

    sprintf(TagMacaddress9905, "0610000000009905");
    sprintf(AnchorMacaddress0016, "3131000000000016");
    sprintf(AnchorMacaddress0019, "0210000000000019");

    while(1)
    {
        for(int i=0;i<5;i++)
        {
            mPosCalJobQueue.add(new DataItem("rangingdiag_1", 1, AnchorMacaddress0016, TagMacaddress9905, 1, 900, IMU, 1, 1, -90, -90, -90, -90, 1, 1, 1, "127.0.0.1"));
            mPosCalJobQueue.add(new DataItem("rangingdiag_1", 1, AnchorMacaddress0019, TagMacaddress9905, 1, 100, IMU, 1, 1, -90, -90, -90, -90, 1, 1, 1, "127.0.0.1"));

            sleep(1);
        }


        for(int i=0;i<10;i++)
        {
            int offset = i * -100;
            mPosCalJobQueue.add(new DataItem("rangingdiag_1", 1, AnchorMacaddress0016, TagMacaddress9905, 1, 900 + offset, IMU, 1, 1, -90, -90, -90, -90, 1, 1, 1, "127.0.0.1"));
            mPosCalJobQueue.add(new DataItem("rangingdiag_1", 1, AnchorMacaddress0019, TagMacaddress9905, 1, 100 - offset, IMU, 1, 1, -90, -90, -90, -90, 1, 1, 1, "127.0.0.1"));

            sleep(1);
        }

        for(int i=0;i<5;i++)
        {
            mPosCalJobQueue.add(new DataItem("rangingdiag_1", 1, AnchorMacaddress0016, TagMacaddress9905, 1, 100, IMU, 1, 1, -90, -90, -90, -90, 1, 1, 1, "127.0.0.1"));
            mPosCalJobQueue.add(new DataItem("rangingdiag_1", 1, AnchorMacaddress0019, TagMacaddress9905, 1, 900, IMU, 1, 1, -90, -90, -90, -90, 1, 1, 1, "127.0.0.1"));

            sleep(1);
        }

        sleep(2);
    }

}

void SimulationMoving::test_fixes_ranging_loop()
{
    char macaddress0145[32];
    char LoRAMacaddress0001[32];
    char LoRAMacaddress0002[32];
    char LoRAMacaddress0003[32];
    char LoRAMacaddress0004[32];

    sprintf(macaddress0145, "0310000000000145");
    sprintf(LoRAMacaddress0001, "0000000000050001");
    sprintf(LoRAMacaddress0002, "0000000000050002");
    sprintf(LoRAMacaddress0003, "0000000000050003");
    sprintf(LoRAMacaddress0004, "0000000000050004");

    while(1)
    {
        DataItem * senditem1 = new DataItem("loraranging", 1, "LoRa", LoRAMacaddress0001, macaddress0145, 589, -39, currentDateTimeForSQL().c_str());
        mPosCalJobQueue.add(senditem1);
        DataItem * senditem2 = new DataItem("loraranging", 1, "LoRa", LoRAMacaddress0002, macaddress0145, 541, -39, currentDateTimeForSQL().c_str());
        mPosCalJobQueue.add(senditem2);
        DataItem * senditem3 = new DataItem("loraranging", 1, "LoRa", LoRAMacaddress0003, macaddress0145, 478, -39, currentDateTimeForSQL().c_str());
        mPosCalJobQueue.add(senditem3);
        //DataItem * senditem4 = new DataItem("loraranging", "LoRa", LoRAMacaddress0004, macaddress0145, ranging, -39, currentDateTimeForSQL());
        //m_queue.add(senditem4);
        sleep(2);
    }
}

void SimulationMoving::test_AUO_loop()
{
    while(1)
    {
        // all tag in same place
        //read_from_csv_File("/Users/ikki/DecaWave/SMIMS/友達/智慧工廠/log/Log_2018-11-20_13-55-50_ranging.csv");

        // Tag013 2D -> 1D -> 2D
        read_from_csv_File("/Users/ikki/DecaWave/SMIMS/友達/智慧工廠/log/Log_2018-11-19_15-25-17_ranging.csv");
        // Tag015 2D -> 1D -> 2D
        //read_from_csv_File("/Users/ikki/DecaWave/SMIMS/友達/智慧工廠/log/Log_2018-11-19_15-30-25_ranging.csv");
    }
}
/*
void SimulationMoving::test_multiple_tag_loop_ihome()
{
    int totalTime = 30;//  seconds
    int marginTime = 100;//100 ms
    int thresholds = 20;//20 meter
    mRangingIdx = 0;

    // srand( time(NULL) );

    while(1)
    {
        fill_multiple_tag_ranging_floor(0, 9, 0, 0, 0, 3000, 0, 0, totalTime, marginTime, thresholds, 12);

        fill_multiple_tag_ranging_floor(0, 9, 3000, 0, 0, 0, 0, 0, totalTime, marginTime, thresholds, 12);

        fill_multiple_tag_ranging_floor(0, 9, 0, 0, 0, -3000, 0, 0, totalTime, marginTime, thresholds, 12);

        fill_multiple_tag_ranging_floor(0, 9, -3000, 0, 0, 0, 0, 0, totalTime, marginTime, thresholds, 12);

    }
}
*/

void SimulationMoving::test_ranging_loop_ihome()
{
    //int totalTime = 20;//  seconds
    int marginTime = 100;//100 ms
    //int thresholds = 20;//15 meter
    mRangingIdx = 0;
    char TagMacaddress[32];

    memset(TagMacaddress, 0, 32);
    sprintf(TagMacaddress, "0410000000000012");
  // srand( time(NULL) );

  while(1)
  {

      // 12F
      //fill_ranging_floor(TagMacaddress, 293,  -44, 100, 293,  -44, 100, 30, marginTime, 15, 12); //wait

      fill_ranging_floor(TagMacaddress, 293,  -44, 100, 460, -230, 100, 10, marginTime, 10, 12, 0);
      //mDevManager->setNodeAlertByType(1, "0410000000000011", NODE_TYPE_TAG);
      fill_ranging_floor(TagMacaddress, 460, -530, 100, 460, -530, 100, 12, marginTime, 10, 12, 0);


      // 13F
      //fill_ranging_floor(TagMacaddress, 460, -530, 100, 471, -222, 100, 15, marginTime, 15, 13); // wait
      fill_ranging_floor(TagMacaddress, 460, -530, 100, 471, -222, 100, 8, marginTime, 15, 13, 0);
      fill_ranging_floor(TagMacaddress, 471, -222, 100, 671, -105, 100, 14, marginTime, 15, 13, 0);
      fill_ranging_floor(TagMacaddress, 671, -105, 100, 1683, -76, 100, 12, marginTime, 15, 13, 0);
      fill_ranging_floor(TagMacaddress, 1683, -76, 100, 2247, -127, 100, 6, marginTime, 15, 13, 0);
      //mDevManager->setNodeAlertByType(1, "0410000000000011", NODE_TYPE_TAG);
      fill_ranging_floor(TagMacaddress, 2247, -127, 100, 2651, -73, 100, 7, marginTime, 15, 13, 0);
      fill_ranging_floor(TagMacaddress, 2651, -73, 100, 3051, -102, 100, 6, marginTime, 15, 13, 0);
      fill_ranging_floor(TagMacaddress, 3051, -102, 100, 3051, -102, 100, 11, marginTime, 15, 13, 0);

      // 12F
      //fill_ranging_floor(TagMacaddress, 3050, -107, 100, 3050, -107, 100, 10, marginTime, 15, 12); //wait
      fill_ranging_floor(TagMacaddress, 3050, -107, 100, 3050, -107, 100, 11, marginTime, 15, 12, 0);
      fill_ranging_floor(TagMacaddress, 3050, -107, 100, 2954, -121, 100, 9, marginTime, 15, 12, 0);
      fill_ranging_floor(TagMacaddress, 2954, -121, 100, 2105, -81, 100, 12, marginTime, 15, 12, 0);
      fill_ranging_floor(TagMacaddress, 2105, -81, 100, 1069, -81, 100, 9, marginTime, 15, 12, 0);
      fill_ranging_floor(TagMacaddress, 1069, -81, 100, 293, -44, 100, 5, marginTime, 15, 12, 0);

      fill_ranging_floor(TagMacaddress, 293, -44, 100, 293, -44, 100, 10, marginTime, 15, 12, 0);

    // fill_ranging(TagMacaddress,    0, -105,  100, 1000,  -105, 100, totalTime, marginTime, thresholds);
    // fill_ranging(TagMacaddress, 1000, -105,  100, 2000,  -105, 100, totalTime, marginTime, thresholds);
    // fill_ranging(TagMacaddress, 2000, -105,  100, 3200,  -105, 100, totalTime, marginTime, thresholds);
    // fill_ranging(TagMacaddress, 3200, -105,  100, 3200,  -105, 100, 2, marginTime, thresholds);
    // mDevManager->setNodeAlertByType(1, "0410000000000011", NODE_TYPE_TAG);
    // create_alert_event_no_area(m_tagIndex);
    // fill_ranging(TagMacaddress, 3200, -105,  100, 3200,  -105, 100, 10, marginTime, thresholds);
    // //mDevManager->setNodeAlertByType( "0410000000000009", NODE_TYPE_TAG);
    // fill_ranging(TagMacaddress, 3000, -105,  100, 2000,  -105, 100, totalTime, marginTime, thresholds);
    // fill_ranging(TagMacaddress, 2000, -105,  100, 1000,  -105, 100, totalTime, marginTime, thresholds);
    // fill_ranging(TagMacaddress, 1000, -105,  100,    0,  -105, 100, totalTime, marginTime, thresholds);




    //create_alert_event_no_area(m_tagIndex);

    // fill_ranging(TagMacaddress, 2055, -100, 100, 2903, -100, 100, 10, marginTime, thresholds);
    // fill_ranging(TagMacaddress, 2903, -100, 100,  2055, -100, 100, 10, marginTime, thresholds);
    // fill_ranging(TagMacaddress, 2055, -100, 100, -2319, -100, 100, totalTime, marginTime, thresholds);
    // fill_ranging(TagMacaddress, -2319, -100, 100, -2790, -100, 100, totalTime, marginTime, thresholds);
    // fill_ranging(TagMacaddress, -2790, -100, 100, -2790, -1500, 100, totalTime, marginTime, thresholds);
    // fill_ranging(TagMacaddress, -2790, -1500, 100, -2790, -100, 100, totalTime, marginTime, thresholds);
    // fill_ranging(TagMacaddress, -2790, -1500, 100, -2319, -100, 100, totalTime, marginTime, thresholds);

    //mDevManager->setNodeAlertByType( "0410000000000009", NODE_TYPE_TAG);
    //mDevManager->setNodeAlertByType( "0410000000000009", NODE_TYPE_TAG);

    //fill_ranging(TagMacaddress, -2319, -100, 100, 222, -100, 100, totalTime, marginTime, thresholds);

  }
}

void SimulationMoving::test_ranging_loop_checkpoint()
{
    // int count = 0;
    int totalTime = 10;//  seconds
    int marginTime = 100;//100 ms
    int thresholds = 12;//12 meter
    //m_tagIndex = 0;
    char TagMacaddress[32];

    memset(TagMacaddress, 0, 32);
    sprintf(TagMacaddress, "0410000000000012");

    while(1)
    {
        fill_ranging_floor(TagMacaddress,   0,   0, 100, 160, 100, 100, totalTime, marginTime, thresholds, 1, 0);

        //mDevManager->setNodeAlertByType(1, "0410000000000013", NODE_TYPE_TAG);

        fill_ranging_floor(TagMacaddress, 160, 100, 100, 160, -900, 100, totalTime * 3, marginTime, thresholds, 1, 0);

        //mDevManager->setNodeAlertByType(1, "0410000000000013", NODE_TYPE_TAG);

        fill_ranging_floor(TagMacaddress, 160, -900, 100, 1000, -800, 100, totalTime * 3, marginTime, thresholds, 1, 0);

        //mDevManager->setNodeAlertByType(1, "0410000000000013", NODE_TYPE_TAG);

        fill_ranging_floor(TagMacaddress, 1000, -800, 100, 1100, 100, 100, totalTime * 3, marginTime, thresholds, 1, 0);

        //mDevManager->setNodeAlertByType(1, "0410000000000013", NODE_TYPE_TAG);

        fill_ranging_floor(TagMacaddress, 1100, 100, 100, 160, 100, 100, totalTime * 3, marginTime, thresholds, 1, 0);

        //mDevManager->setNodeAlertByType(1, "0410000000000013", NODE_TYPE_TAG);

        fill_ranging_floor(TagMacaddress, 160, 100, 100,  0,  0, 100, totalTime, marginTime, thresholds, 1, 0);
    }
}

void SimulationMoving::test_ranging_loop_taipei_checkpoint()
{
    // int count = 0;
    int totalTime = 10;//  seconds
    int marginTime = 1000;//100 ms
    int thresholds = 10;//12 meter
    //m_tagIndex = 1;
    char TagMacaddress[32];

    memset(TagMacaddress, 0, 32);
    sprintf(TagMacaddress, "0410000000000012");

    while(1)
    {
        fill_ranging_floor(TagMacaddress,   0,   0, 100, 100, 100, 100, totalTime, marginTime, thresholds, 1, 0);

        //mDevManager->setNodeAlertByType(1, "0410000000000013", NODE_TYPE_TAG);

        fill_ranging_floor(TagMacaddress, 100, 100, 100, 100, 500, 100, totalTime * 3, marginTime, thresholds, 1, 0);

        //mDevManager->setNodeAlertByType(1, "0410000000000013", NODE_TYPE_TAG);

        fill_ranging_floor(TagMacaddress, 100, 500, 100, 450, 500, 100, totalTime * 3, marginTime, thresholds, 1, 0);

        //mDevManager->setNodeAlertByType(1, "0410000000000013", NODE_TYPE_TAG);

        fill_ranging_floor(TagMacaddress, 450, 500, 100, 900, 180, 100, totalTime * 3, marginTime, thresholds, 1, 0);

        //mDevManager->setNodeAlertByType(1, "0410000000000013", NODE_TYPE_TAG);

        fill_ranging_floor(TagMacaddress, 900, 180, 100, 950, 470, 100, totalTime * 3, marginTime, thresholds, 1, 0);


        fill_ranging_floor(TagMacaddress, 950, 470, 100, 450, 500, 100, totalTime * 3, marginTime, thresholds, 1, 0);


        fill_ranging_floor(TagMacaddress, 450, 500, 100, 100, 500, 100, totalTime * 3, marginTime, thresholds, 1, 0);


        fill_ranging_floor(TagMacaddress,100, 500, 100, 100, 100, 100, totalTime * 3, marginTime, thresholds, 1, 0);

        sleep(3600);

    }
}

void SimulationMoving::test_ranging_loop_taipei_1D()
{
    // int count = 0;
    int totalTime = 10;//  seconds
    int marginTime = 1000;//100 ms
    int thresholds = 30;//32 meter
    //m_tagIndex = 1;
    char TagMacaddress[32];

    memset(TagMacaddress, 0, 32);
    sprintf(TagMacaddress, "0410000000000007");

    while(1)
    {
        fill_ranging_floor(TagMacaddress, 800,-505,605, -875,-595,160, totalTime, marginTime, thresholds, 1, 0);

        //mDevManager->setNodeAlertByType(1, "0410000000000013", NODE_TYPE_TAG);

        fill_ranging_floor(TagMacaddress, -875,-595,160, -2711,-595,160, totalTime * 3, marginTime, thresholds, 1, 0);

        //mDevManager->setNodeAlertByType(1, "0410000000000013", NODE_TYPE_TAG);

        fill_ranging_floor(TagMacaddress, -2711,-595,160, -3859,-690,160, totalTime * 3, marginTime, thresholds, 1, 0);

        //mDevManager->setNodeAlertByType(1, "0410000000000013", NODE_TYPE_TAG);

        fill_ranging_floor(TagMacaddress, -3859,-690,160, -2711,-595,160, totalTime * 3, marginTime, thresholds, 1, 0);

        //mDevManager->setNodeAlertByType(1, "0410000000000013", NODE_TYPE_TAG);

        fill_ranging_floor(TagMacaddress, -2711,-595,160, -875,-595,160, totalTime * 3, marginTime, thresholds, 1, 0);


        fill_ranging_floor(TagMacaddress, -875,-595,160, 800,-505,605, totalTime * 3, marginTime, thresholds, 1, 0);
        sleep(10);

    }
}

void SimulationMoving::test_ranging_loop_taf_checkpoint()
{
    // int count = 0;
    int totalTime = 10;//  seconds
    int marginTime = 1000;//100 ms
    int thresholds = 12;//12 meter
    //m_tagIndex = 1;
    char TagMacaddress[32];

    memset(TagMacaddress, 0, 32);
    sprintf(TagMacaddress, "0410000000000012");

    while(1)
    {
        fill_ranging_floor(TagMacaddress, 4980, 20, 100, 4580, 20, 100, totalTime, marginTime, thresholds, 1, 0);

        //mDevManager->setNodeAlertByType(1, "0410000000000013", NODE_TYPE_TAG);

        fill_ranging_floor(TagMacaddress, 4580, 20, 100, 4563, -214, 100, totalTime * 3, marginTime, thresholds, 1, 0);

        //mDevManager->setNodeAlertByType(1, "0410000000000013", NODE_TYPE_TAG);

        fill_ranging_floor(TagMacaddress, 4563, -214, 100, 5127, -214, 100, totalTime * 3, marginTime, thresholds, 1, 0);

        //mDevManager->setNodeAlertByType(1, "0410000000000013", NODE_TYPE_TAG);

        fill_ranging_floor(TagMacaddress, 5127, -214, 100, 5127, 570, 100, totalTime * 3, marginTime, thresholds, 1, 0);

        //mDevManager->setNodeAlertByType(1, "0410000000000013", NODE_TYPE_TAG);

        fill_ranging_floor(TagMacaddress, 5127, 570, 100, 4543, 505, 100, totalTime * 3, marginTime, thresholds, 1, 0);


        fill_ranging_floor(TagMacaddress, 4543, 505, 100, 4580, 20, 100, totalTime * 3, marginTime, thresholds, 1, 0);


        fill_ranging_floor(TagMacaddress, 4580, 20, 100, 4980, 20, 100, totalTime * 3, marginTime, thresholds, 1, 0);

        sleep(3600);

    }
}

void SimulationMoving::test_ranging_loop_kaohsiungOffice()
{
    // int count = 0;
    // int totalTime = 10;//  seconds
    int marginTime = 100;//100 ms
    int thresholds = 20;//20 meter
    mRangingIdx = 0;
    //m_tagIndex = 1;
    // srand( time(NULL) );
    char TagMacaddress[32];

    memset(TagMacaddress, 0, 32);
    sprintf(TagMacaddress, "0310000000000015");

    while(1)
    {
        // fill_ranging(TagMacaddress,   0,   0, 100,  30,   0, 100, totalTime, marginTime, thresholds);
        // fill_ranging(TagMacaddress,  30,   0, 100,  30,  60, 100, totalTime, marginTime, thresholds);
        // fill_ranging(TagMacaddress,  30,  60, 100,  60,  60, 100, totalTime, marginTime, thresholds);
        // fill_ranging(TagMacaddress,  60,  60, 100,   0,   0, 100, totalTime, marginTime, thresholds);



         // fill_ranging(TagMacaddress, 100, 100, 100, 200, 100, 100, totalTime, marginTime, thresholds);
         // fill_ranging(TagMacaddress, 200, 100, 100, 200, 200, 100, totalTime, marginTime, thresholds);
         // fill_ranging(TagMacaddress, 200, 200, 100, 100, 200, 100, totalTime, marginTime, thresholds);
         // fill_ranging(TagMacaddress, 100, 200, 100, 100, 100, 100, totalTime, marginTime, thresholds);

        // stay same location
        //fill_ranging(TagMacaddress, 1080, -800, 100, 1180, -820, 100, totalTime, marginTime, thresholds);
        //fill_ranging(TagMacaddress, 1180, -820, 100, 1010, -700, 100, totalTime, marginTime, thresholds);
        //fill_ranging(TagMacaddress, 1010, -700, 100, 1080, -800, 100, totalTime, marginTime, thresholds);

        fill_ranging(TagMacaddress, 3350, 1050, 100, 4600, 1050, 100, 20, marginTime, thresholds, 0);
        //mDevManager->setNodeAlertByType(TagMacaddress, NODE_TYPE_TAG);

        fill_ranging(TagMacaddress, 4600, 1050, 100, 4860,  100, 100, 20, marginTime, thresholds, 0);
        fill_ranging(TagMacaddress, 4860,  100, 100, 5050, -300, 100, 120, marginTime, thresholds, 0);
        fill_ranging(TagMacaddress, 5050, -300, 100, 4780, -700, 100, 60, marginTime, thresholds, 0);

        fill_ranging(TagMacaddress, 4780, -700, 100, 3760,  250, 100, 20, marginTime, thresholds, 0);
        fill_ranging(TagMacaddress, 3760,  250, 100, 3350, 1050, 100, 20, marginTime, thresholds, 0);

        sleep(3600);

        // fill_ranging(TagMacaddress, 100, 100, 100, 400, 100, 100, totalTime, marginTime, thresholds);
        // fill_ranging(TagMacaddress, 400, 100, 100, 400, 400, 100, totalTime, marginTime, thresholds);
        // fill_ranging(TagMacaddress, 400, 400, 100, 100, 400, 100, totalTime, marginTime, thresholds);
        // fill_ranging(TagMacaddress, 100, 400, 100, 100, 100, 100, totalTime, marginTime, thresholds);
    }
}

void SimulationMoving::test_ranging_loop_kaohsiungOffice(int tag_index_start, int tag_index_end)
{
    // int totalTime = 10;//  seconds
    // int marginTime = 100;//100 ms
    // int thresholds = 20;//20 meter

    while(1)
    {

        // stay same location
        //fill_multiple_tag_ranging_floor(tag_index_start, tag_index_end,   300,   20, 100, 400,   120, 100, totalTime, marginTime, thresholds, 4);
        //fill_multiple_tag_ranging_floor(tag_index_start, tag_index_end,   400,   120, 100, 280,   120, 100, totalTime, marginTime, thresholds, 4);
        //fill_multiple_tag_ranging_floor(tag_index_start, tag_index_end,   280,   120, 100, 300,   20, 100, totalTime, marginTime, thresholds, 4);

        // fill_multiple_tag_ranging_floor(tag_index_start, tag_index_end, 0,   0, 100, 300,   0, 100, totalTime, marginTime, thresholds, 4);
        //
        // fill_multiple_tag_ranging_floor(tag_index_start, tag_index_end, 300,   0, 100, 300, 300, 100, totalTime, marginTime, thresholds, 4);
        //
        // fill_multiple_tag_ranging_floor(tag_index_start, tag_index_end, 300, 300, 100,   0, 300, 100, totalTime, marginTime, thresholds, 4);
        //
        // fill_multiple_tag_ranging_floor(tag_index_start, tag_index_end, 0, 300, 100,   0,   0, 100, totalTime, marginTime, thresholds, 4);
    }
}

void SimulationMoving::test_ranging_loop()
{
    int count = 0;
    //int totalTime = 4;//  seconds
    int marginTime = 200;//200 ms
    int thresholds = 20;//20 meter
    mRangingIdx = 0;

    char macaddress2012[32];
    sprintf(macaddress2012, "0610000000002012");

    //m_tagIndex = 34;
    srand((unsigned int)time(NULL) );

    while(1)
    {
        //fill_LoRa_ranging_floor(macaddress2012, -1500, 1100, 270, -836, 519, 157, 10, marginTime, thresholds, -100);

        //fill_LoRa_ranging_floor(macaddress2012, -836, 519, 157, -1400, 1295, 270, 10, marginTime, thresholds, -100);

        fill_LoRa_ranging_floor(macaddress2012, -1400, 1295, 270, -1500, 1100, 270, 10, marginTime, thresholds, -100);

        mDevManager->setNodeAlertByType(1, "4210000000001302", NODE_TYPE_TAG);
        sleep(60);

        // fill_ranging(TagMacaddress, -757, 1769, 130, -816, 1637, 130, 10, marginTime, thresholds);
        //fill_ranging(TagMacaddress, -816, 1637, 130, -784, 1266, 130, totalTime, marginTime, thresholds);
        // //create_alert_event(m_tagIndex);
        // fill_ranging(TagMacaddress, -784, 1266, 130, -805, 1075, 130, totalTime, marginTime, thresholds);
        // //create_alert_event(m_tagIndex);
        // fill_ranging(TagMacaddress, -805, 1075, 130,-1208, 1028, 130, totalTime, marginTime, thresholds);
        // fill_ranging(TagMacaddress,-1208, 1028, 130,-1483,  741, 130, totalTime, marginTime, thresholds);
        // //create_alert_event(m_tagIndex);
        // fill_ranging(TagMacaddress,-1483, 741, 130, -1483, 392, 130, totalTime, marginTime, thresholds);
        // fill_ranging(TagMacaddress,-1483, 392, 130, -1197, 116, 130, totalTime, marginTime, thresholds);
        // fill_ranging(TagMacaddress,-1197, 116, 130, -927, -185, 130, totalTime, marginTime, thresholds);
        //
        // //create_alert_event(m_tagIndex);
        //
        // fill_ranging(TagMacaddress, -927, -185, 130, -482,  74, 130, totalTime, marginTime, thresholds);
        // fill_ranging(TagMacaddress, -482,   74, 130,  -42, 333, 130, totalTime, marginTime, thresholds);
        // //create_alert_event(m_tagIndex);
        //
        // fill_ranging(TagMacaddress,  -42,  333, 130,  270,  551, 130, totalTime, marginTime, thresholds);
        // fill_ranging(TagMacaddress,  270,  551, 130,  672,  635, 130, totalTime, marginTime, thresholds);
        // fill_ranging(TagMacaddress,  672,  635, 130,  519, 1006, 130, totalTime, marginTime, thresholds);
        // //create_alert_event(m_tagIndex);
        // fill_ranging(TagMacaddress, 519, 1006, 130, 307, 1192, 130, totalTime, marginTime, thresholds);
        // fill_ranging(TagMacaddress, 307, 1192, 130, 312, 1414, 130, totalTime, marginTime, thresholds);
        // fill_ranging(TagMacaddress, 312, 1414, 130, -84, 1515, 130, totalTime, marginTime, thresholds);
        //
        //
        // fill_ranging(TagMacaddress, -84, 1515, 130, -466, 1568, 130, totalTime, marginTime, thresholds);
        // fill_ranging(TagMacaddress, -466, 1568, 130, -757, 1769, 130, totalTime, marginTime, thresholds);

        //fill_ranging(TagMacaddress, -757, 1769, 130,  -757, 1769, 130, 600, marginTime, thresholds);
        //create_alert_event(m_tagIndex);

        count++;
    }
    /*
    while(1)
    {
        fill_ranging(TagMacaddress,  794, 3703, 130,  694, 3603, 130, 10, marginTime, thresholds);
        fill_ranging(TagMacaddress,  694, 3603, 130,  768, 2638, 130, totalTime, marginTime, thresholds);
        //create_alert_event(m_tagIndex);
        fill_ranging(TagMacaddress,  768, 2638, 130,  831, 2368, 130, totalTime, marginTime, thresholds);
        //create_alert_event(m_tagIndex);
        fill_ranging(TagMacaddress,  831, 2368, 130,  471, 2103, 130, totalTime, marginTime, thresholds);
        fill_ranging(TagMacaddress,  471, 2103, 130,  127, 2034, 130, totalTime, marginTime, thresholds);
        //create_alert_event(m_tagIndex);
        fill_ranging(TagMacaddress,  127, 2034, 130,   63, 1658, 130, totalTime, marginTime, thresholds);
        fill_ranging(TagMacaddress,   63, 1658, 130,  116, 1330, 130, totalTime, marginTime, thresholds);
        fill_ranging(TagMacaddress,  116, 1330, 130, -190, 1033, 130, totalTime, marginTime, thresholds);

        //create_alert_event(m_tagIndex);

        fill_ranging(TagMacaddress, -190, 1033, 130,  217,  826, 130, totalTime, marginTime, thresholds);
        fill_ranging(TagMacaddress,  217,  826, 130,  519,  365, 130, totalTime, marginTime, thresholds);
        create_alert_event(m_tagIndex);

        fill_ranging(TagMacaddress,  519,  365, 130,  911,  572, 130, totalTime, marginTime, thresholds);
        fill_ranging(TagMacaddress,  911,  572, 130, 1356,  672, 130, totalTime, marginTime, thresholds);
        fill_ranging(TagMacaddress, 1356,  672, 130, 1610, 1149, 130, totalTime, marginTime, thresholds);
        //create_alert_event(m_tagIndex);
        fill_ranging(TagMacaddress, 1610, 1149, 130, 2066, 1277, 130, totalTime, marginTime, thresholds);
        fill_ranging(TagMacaddress, 2066, 1277, 130, 2347, 1753, 130, totalTime, marginTime, thresholds);
        fill_ranging(TagMacaddress, 2347, 1753, 130, 2013, 2347, 130, totalTime, marginTime, thresholds);
        fill_ranging(TagMacaddress, 2013, 2347, 130, 2071, 2850, 130, totalTime, marginTime, thresholds);
        fill_ranging(TagMacaddress, 2071, 2850, 130, 1859, 3110, 130, totalTime, marginTime, thresholds);
        fill_ranging(TagMacaddress, 1859, 3110, 130,  990, 3290, 130, totalTime, marginTime, thresholds);
        fill_ranging(TagMacaddress,  990, 3290, 130,  900, 3492, 130, totalTime, marginTime, thresholds);
        fill_ranging(TagMacaddress,  900, 3492, 130,  794, 3703, 130, totalTime, marginTime, thresholds);

        fill_ranging(TagMacaddress,  794, 3703, 130,  794, 3703, 130, 600, marginTime, thresholds);
        create_alert_event(m_tagIndex);

        count++;
    }
*/
}

void SimulationMoving::do_soccer_random_move()
{
     add_action_join("0410000000009901", NODE_TYPE_TAG, -1, "1.0.0");
     add_action_join("0410000000009902", NODE_TYPE_TAG, -1, "1.0.0");
     add_action_join("0410000000009903", NODE_TYPE_TAG, -1, "1.0.0");
     add_action_join("0410000000009904", NODE_TYPE_TAG, -1, "1.0.0");
     add_action_join("0410000000009905", NODE_TYPE_TAG, -1, "1.0.0");
     add_action_join("0410000000009906", NODE_TYPE_TAG, -1, "1.0.0");
     add_action_join("0410000000009907", NODE_TYPE_TAG, -1, "1.0.0");
     add_action_join("0410000000009908", NODE_TYPE_TAG, -1, "1.0.0");
     add_action_join("0410000000009909", NODE_TYPE_TAG, -1, "1.0.0");
     add_action_join("0410000000009910", NODE_TYPE_TAG, -1, "1.0.0");
     add_action_join("0410000000009911", NODE_TYPE_TAG, -1, "1.0.0");

     add_action_join("0410000000008801", NODE_TYPE_TAG, -1, "1.0.0");
     add_action_join("0410000000008802", NODE_TYPE_TAG, -1, "1.0.0");
     add_action_join("0410000000008803", NODE_TYPE_TAG, -1, "1.0.0");
     add_action_join("0410000000008804", NODE_TYPE_TAG, -1, "1.0.0");
     add_action_join("0410000000008805", NODE_TYPE_TAG, -1, "1.0.0");
     add_action_join("0410000000008806", NODE_TYPE_TAG, -1, "1.0.0");
     add_action_join("0410000000008807", NODE_TYPE_TAG, -1, "1.0.0");
     add_action_join("0410000000008808", NODE_TYPE_TAG, -1, "1.0.0");
     add_action_join("0410000000008809", NODE_TYPE_TAG, -1, "1.0.0");
     add_action_join("0410000000008810", NODE_TYPE_TAG, -1, "1.0.0");
     add_action_join("0410000000008811", NODE_TYPE_TAG, -1, "1.0.0");

     add_action_join("0320000000007701", NODE_TYPE_ANCHOR, -1, "1.0.0");
     add_action_join("0320000000007702", NODE_TYPE_ANCHOR, -1, "1.0.0");
     add_action_join("0320000000007703", NODE_TYPE_ANCHOR, -1, "1.0.0");
     add_action_join("0320000000007704", NODE_TYPE_ANCHOR, -1, "1.0.0");
     add_action_join("0320000000007705", NODE_TYPE_ANCHOR, -1, "1.0.0");
     add_action_join("0320000000007706", NODE_TYPE_ANCHOR, -1, "1.0.0");

     mDevManager->setTagVoltage(1, "0410000000009901", "3.9");
     mDevManager->setTagVoltage(1, "0410000000009902", "3.9");
     mDevManager->setTagVoltage(1, "0410000000009903", "3.9");
     mDevManager->setTagVoltage(1, "0410000000009904", "3.9");
     mDevManager->setTagVoltage(1, "0410000000009905", "3.9");
     mDevManager->setTagVoltage(1, "0410000000009906", "3.9");
     mDevManager->setTagVoltage(1, "0410000000009907", "3.9");
     mDevManager->setTagVoltage(1, "0410000000009908", "3.9");
     mDevManager->setTagVoltage(1, "0410000000009909", "3.9");
     mDevManager->setTagVoltage(1, "0410000000009910", "3.9");
     mDevManager->setTagVoltage(1, "0410000000009911", "3.9");

     mDevManager->setTagVoltage(1, "0410000000008801", "3.9");
     mDevManager->setTagVoltage(1, "0410000000008802", "3.9");
     mDevManager->setTagVoltage(1, "0410000000008803", "3.9");
     mDevManager->setTagVoltage(1, "0410000000008804", "3.9");
     mDevManager->setTagVoltage(1, "0410000000008805", "3.9");
     mDevManager->setTagVoltage(1, "0410000000008806", "3.9");
     mDevManager->setTagVoltage(1, "0410000000008807", "3.9");
     mDevManager->setTagVoltage(1, "0410000000008808", "3.9");
     mDevManager->setTagVoltage(1, "0410000000008809", "3.9");
     mDevManager->setTagVoltage(1, "0410000000008810", "3.9");
     mDevManager->setTagVoltage(1, "0410000000008811", "3.9");

     int marginTime = 200;// ms
     int thresholds = 100;// meter

     std::list<RandomTagStruct*> mRandomTagList;

     mRandomTagList.push_back( updateRandomTag(mRandomTagList, "0410000000009901"));
     mRandomTagList.push_back( updateRandomTag(mRandomTagList, "0410000000009902"));
     mRandomTagList.push_back( updateRandomTag(mRandomTagList, "0410000000009903"));
     mRandomTagList.push_back( updateRandomTag(mRandomTagList, "0410000000009904"));
     mRandomTagList.push_back( updateRandomTag(mRandomTagList, "0410000000009905"));
     mRandomTagList.push_back( updateRandomTag(mRandomTagList, "0410000000009906"));
     mRandomTagList.push_back( updateRandomTag(mRandomTagList, "0410000000009907"));
     mRandomTagList.push_back( updateRandomTag(mRandomTagList, "0410000000009908"));
     mRandomTagList.push_back( updateRandomTag(mRandomTagList, "0410000000009909"));
     mRandomTagList.push_back( updateRandomTag(mRandomTagList, "0410000000009910"));
     mRandomTagList.push_back( updateRandomTag(mRandomTagList, "0410000000009911"));

     mRandomTagList.push_back( updateRandomTag(mRandomTagList, "0410000000008801"));
     mRandomTagList.push_back( updateRandomTag(mRandomTagList, "0410000000008802"));
     mRandomTagList.push_back( updateRandomTag(mRandomTagList, "0410000000008803"));
     mRandomTagList.push_back( updateRandomTag(mRandomTagList, "0410000000008804"));
     mRandomTagList.push_back( updateRandomTag(mRandomTagList, "0410000000008805"));
     mRandomTagList.push_back( updateRandomTag(mRandomTagList, "0410000000008806"));
     mRandomTagList.push_back( updateRandomTag(mRandomTagList, "0410000000008807"));
     mRandomTagList.push_back( updateRandomTag(mRandomTagList, "0410000000008808"));
     mRandomTagList.push_back( updateRandomTag(mRandomTagList, "0410000000008809"));
     mRandomTagList.push_back( updateRandomTag(mRandomTagList, "0410000000008810"));
     mRandomTagList.push_back( updateRandomTag(mRandomTagList, "0410000000008811"));

     while(1)
     {
         for (std::list<RandomTagStruct*>::iterator ranTag = mRandomTagList.begin(); ranTag != mRandomTagList.end(); ranTag++)
         {
             (*ranTag)->curIndex = fill_ranging((*ranTag)->macaddress.c_str(), (*ranTag)->startPosX, (*ranTag)->startPosY, (*ranTag)->startPosZ, (*ranTag)->endPosX, (*ranTag)->endPosY, (*ranTag)->endPosZ, (*ranTag)->totalTime, marginTime, thresholds, (*ranTag)->curIndex);

             if ((*ranTag)->curIndex == 0)
             {
                 updateRandomTag(mRandomTagList, (*ranTag)->macaddress.c_str());
             }
         }
         // vilslog::printf("SimulationMoving::do_soccer_random_move() curIndex[%d]\n", curIndex);

         usleep(marginTime * 1000);
         //sleep(1);
     }
}

RandomTagStruct * SimulationMoving::updateRandomTag(std::list<RandomTagStruct*> & randomTagList, const char * macaddress)
{
    RandomTagStruct * curTag = NULL;

    // std::default_random_engine generator;
    std::uniform_int_distribution<int> distributionPosX(100, 10000);
    std::uniform_int_distribution<int> distributionPosY(100, 6600);
    std::uniform_int_distribution<int> distributionTotalTime(10, 30);

    for (std::list<RandomTagStruct*>::iterator ranTag = randomTagList.begin(); ranTag != randomTagList.end(); ranTag++)
    {
        if ( (*ranTag)->macaddress.compare(macaddress) == 0)
        {
            curTag = (*ranTag);

            curTag->curIndex = 0;
            curTag->startPosX = curTag->endPosX;
            curTag->startPosY = curTag->endPosY;
            curTag->endPosX = distributionPosX(generator);
            curTag->endPosY = distributionPosY(generator);
            curTag->totalTime = distributionTotalTime(generator);

            // vilslog::printf("SimulationMoving::updateRandomTag() macaddress[%s] startPosX[%d] startPosY[%d] -> endPosX[%d] endPosY[%d] totalTime[%d]\n",
            //     curTag->macaddress.c_str(), curTag->startPosX, curTag->startPosY, curTag->endPosX, curTag->endPosY, curTag->totalTime);

            break;
        }
    }
    if (curTag == NULL)
    {
        curTag = new RandomTagStruct();
        curTag->macaddress = macaddress;
        curTag->startPosX = 100;
        curTag->startPosY = 100;
        curTag->startPosZ = 100;
        curTag->endPosX = distributionPosX(generator);
        curTag->endPosY = distributionPosY(generator);
        curTag->endPosZ = 100;
        curTag->totalTime = distributionTotalTime(generator);

        // vilslog::printf("SimulationMoving::updateRandomTag() macaddress[%s] startPosX[%d] startPosY[%d] -> endPosX[%d] endPosY[%d] totalTime[%d]\n",
        //     curTag->macaddress.c_str(), curTag->startPosX, curTag->startPosY, curTag->endPosX, curTag->endPosY, curTag->totalTime);

    }
    return curTag;
}

void SimulationMoving::read_anchor_csv_File(const char * csvfilepath)
{
    std::ifstream       file(csvfilepath);

    CSVRow prerow;
    CSVRow row;

    // firsrt row is header info
    file >> row;

    while(file >> row)
    {
        row[0]; // name
        row[1]; // id
        row[2]; // macaddress
        row[3]; // x
        row[4]; // y
        row[5]; // z
        row[6]; // version

        //
        // check if anchor or locator
        //
        int AnchorIndex = mDevManager->getAnchorIndexFromId(1, row[1].c_str());
        if (AnchorIndex >= 0 )
        {
            mDevManager->setAnchorInfoS(1, row[1].c_str(), ::atoi(row[3].c_str()), ::atoi(row[4].c_str()), ::atoi(row[5].c_str()));
        }
        else
        {
            int LocatorIndex = mDevManager->getLocatorIndex(1, row[2].c_str());
            if (LocatorIndex >= 0 )
            {
                mDevManager->setLocatorInfoS(1, row[1].c_str(), ::atoi(row[3].c_str()), ::atoi(row[4].c_str()), ::atoi(row[5].c_str()));
            }
            else
            {
                // check if is a Locator
                std::size_t found_Locator = row[1].find("Locator");
                std::size_t found_AOA = row[1].find("AOA");
                if (found_Locator)
                {
                    // is a new locator
                    mDevManager->setLocatorIndex(1, row[2].c_str(), -1, row[6].c_str());
                    mDevManager->setLocatorInfoS(1, row[1].c_str(), ::atoi(row[3].c_str()), ::atoi(row[4].c_str()), ::atoi(row[5].c_str()));
                }
                else
                if (found_AOA)
                {
                    // is a new AOA locator
                    mDevManager->setLocatorIndex(1, row[2].c_str(), -1, row[6].c_str());
                    mDevManager->setLocatorInfoS(1, row[1].c_str(), ::atoi(row[3].c_str()), ::atoi(row[4].c_str()), ::atoi(row[5].c_str()));
                }
                else
                {
                    // set it as new anchor
                    mDevManager->setAnchorIndex(1, row[2].c_str(), -1, row[6].c_str());
                    mDevManager->setAnchorInfoS(1, row[1].c_str(), ::atoi(row[3].c_str()), ::atoi(row[4].c_str()), ::atoi(row[5].c_str()));
                }
           }
        }

        vilslog::printf("read_anchor_csv_File() %s(%d, %d, %d)\n", row[1].c_str(), ::atoi(row[3].c_str()), ::atoi(row[4].c_str()), ::atoi(row[5].c_str()));
    }


}

void SimulationMoving::read_from_csv_File(const char * csvfilepath)
{
    std::ifstream       file(csvfilepath);

    CSVRow prerow;
    CSVRow row;
    std::vector<std::string> Anchors;
    //std::vector<int> AnchorIndex;
    std::vector<std::string> AnchorMacaddress;

    srand((unsigned int)time(NULL));

    int validIndex = -1;
    int startAnchor = 0;
    // firsrt row is header info
    file >> row;
    for(unsigned int i=0;i<row.size();i++)
    {
        //std::cout <<row[i]<<", ";
        if (row[i].compare("tag") == 0)
        {
            startAnchor = 1;
            continue;
        }
        else
        if (row[i].compare("valid") == 0)
        {
            validIndex = i;
            startAnchor = 0;
            continue;
        }

        if (startAnchor == 1)
        {
            Anchors.push_back(row[i]);
        }
    }
    std::cout <<endl;
    //vilslog::printf("Anchors.size()[%lu]\n", Anchors.size());

    std::list<NodeStruct*> * anchorNodeList = mDevManager->get_Anchor_list(1);
    std::list<NodeStruct*> * locatorNodeList = mDevManager->get_Locator_list(1);
    for(unsigned int i=0;i<Anchors.size();i++)
    {
        bool bFound = false;
        for (std::list<NodeStruct*>::iterator node = anchorNodeList->begin(); node != anchorNodeList->end(); node++)
        {
            if (strcmp(Anchors[i].c_str(), (*node)->nodeid) == 0)
            {
                //AnchorIndex.push_back((*node)->index);
                AnchorMacaddress.push_back((*node)->macaddress);
                vilslog::printf("%s[%s]\n", Anchors[i].c_str(),(*node)->macaddress);
                bFound = true;
            }
        }
        if (!bFound)
        {
            for (std::list<NodeStruct*>::iterator node = locatorNodeList->begin(); node != locatorNodeList->end(); node++)
            {
                if (strcmp(Anchors[i].c_str(), (*node)->nodeid) == 0)
                {
                    //AnchorIndex.push_back((*node)->index);
                    AnchorMacaddress.push_back((*node)->macaddress);
                    vilslog::printf("%s[%s]\n", Anchors[i].c_str(), (*node)->macaddress);
                    bFound = true;
                }
            }
        }

        if (!bFound)
        {
            vilslog::printf("SimulationMoving::read_from_csv_File() startAnchor[%d][%s] Not Found!!!!\n", startAnchor, Anchors[i].c_str());

            // push a empty macaddress
            AnchorMacaddress.push_back("");
        }
    }

    vilslog::printf("SimulationMoving::read_from_csv_File() startAnchor[%d] validIndex[%d]\n", startAnchor, validIndex);

    std::list<NodeStruct*> * tagNodeList = mDevManager->get_Tag_list(1);


    time_t startTime = getCurrentTimeInSec();

    int count = 0;
    int mRangingIdx = 0;
    // read first row
    file >> prerow;
    while(file >> row)
    {
        // vilslog::printf("SimulationMoving::read_from_csv_File() row.size()[%d] \n", row.size());

        if ( row.size() <= 1 )
        {
            continue;
        }

        // show current ranging
        //row[1]; // tag
        int imu[6]={0,0,-9,0,0,0};
        float angle = 0.0f;
        //int tag_index = 0;
        std::string tagMacaddress = "";

        time_t totalTime = getCurrentTimeInSec() - startTime;

        //
        // only view this tag
        //
        // if (strcmp(row[1].c_str(), "Tag1181") != 0)
        // {
        //     // wait next time
        //     int timeOffset = getDiffTime(row[0].c_str(), prerow[0].c_str());
        //
        //     count++;
        //
        //     if (timeOffset > 0)
        //         usleep(timeOffset * 1000);
        //
        //     prerow = row;
        //
        //     continue;
        // }

        for (std::list<NodeStruct*>::iterator node = tagNodeList->begin(); node != tagNodeList->end(); node++)
        {
            if (strcmp(row[1].c_str(), (*node)->nodeid) == 0)
            {
                tagMacaddress = (*node)->macaddress;
                break;
            }
        }

        if ( tagMacaddress.length() == 0 )
        {
            //vilslog::printf("SimulationMoving::read_from_csv_File() Tag[%s] not found!\n", row[1].c_str());

        }

        if ( row.size() > 2 + Anchors.size() )
        {
            //int IMUidx = 2 + (int)Anchors.size() + 6;
            int IMUidx = validIndex + 9;
            //int IMUidx = validIndex + 6; // powertag data
            imu[0] = ::atoi(row[IMUidx + 1].c_str());
            imu[1] = ::atoi(row[IMUidx + 2].c_str());
            imu[2] = ::atoi(row[IMUidx + 3].c_str());
            imu[3] = ::atoi(row[IMUidx + 4].c_str());
            imu[4] = ::atoi(row[IMUidx + 5].c_str());
            imu[5] = ::atoi(row[IMUidx + 6].c_str());

            int angleidx = IMUidx + 7;
            angle = ::stof(row[angleidx].c_str());

            // vilslog::printf("SimulationMoving::read_from_csv_File() angleidx[%d] angle[%s]\n", angleidx, row[angleidx].c_str());
        }
        else
        {
            //int imuoffset0 = rand() % 10;
            //int imuoffset1 = rand() % 5;
            int imuoffset0 = rand() % 20;
            int imuoffset1 = rand() % 5;
            imu[0] = imuoffset0;
            imu[1] = imuoffset1;
        }

        for(unsigned int i=0;i<Anchors.size();i++)
        {
            int distance = ::atoi(row[2+i].c_str());
            if (distance > 0)
            {
                // vilslog::printf("SimulationMoving::read_from_csv_File() add_action_ranging AnchorMacaddress[%s] tagMacaddress[%s]\n",
                //     AnchorMacaddress[i].c_str(), tagMacaddress.c_str());

                //if (strcmp(AnchorMacaddress[i].c_str(), "0530000000000003") != 0)
                if ( AnchorMacaddress[i].length() > 0 && tagMacaddress.length() > 0)
                {

                    // vilslog::printf("add_action_ranging AnchorMacaddress[%s] tagMacaddress[%s] distance[%d]\n",
                    //     AnchorMacaddress[i].c_str(), tagMacaddress.c_str(), distance);

                    if (angle != 0.0f)
                    {
                        vilslog::printf("add_action_ranging time[%d] AnchorMacaddress[%s] tagMacaddress[%s] distance[%d] angle[%f]\n",
                            (int)totalTime, AnchorMacaddress[i].c_str(), tagMacaddress.c_str(), distance, angle);

                        int projectid = 1;
                        int rssi = 0;
                        int velocity = 2;
                        char timestamp[64];
                        snprintf(timestamp, 64, "%s", currentDateTimeForSQL().c_str());

                        float angle1 = angle_to_rad(angle);

                        DataItem * senditem1 = new DataItem("lorarangingcompressaoa", projectid, "LoRa", AnchorMacaddress[i].c_str(), tagMacaddress.c_str(), distance, velocity, angle1, rssi, timestamp);
                        mPosCalJobQueue.add(senditem1);
                    }
                    else
                    {
                        add_action_ranging(AnchorMacaddress[i].c_str(), tagMacaddress.c_str(), mRangingIdx++, distance, imu);
                    }
                }
            }
        }

        // wait next time
        int timeOffset = getDiffTime(row[0].c_str(), prerow[0].c_str());

        count++;

        if (timeOffset > 0)
            usleep(timeOffset * 1000);

        prerow = row;
    }
}

int SimulationMoving::getDiffTime(const char * curTime, const char * preTime)
{
    int retdifftime = 0;

    // 2018/08/16 09:57:07.693
    //const char * time_details = row[0].c_str();
    struct tm pre_tm;
    strptime(preTime, "%Y/%m/%d %H:%M:%S.", &pre_tm);
    time_t pre_t = mktime(&pre_tm);  // t is now your desired time_t
    // millisecond
    int YYYY, mm, DD, HH, MM, SS;
    int pre_millisecond;
    sscanf(preTime, "%d/%d/%d %d:%d:%d.%d", &YYYY, &mm, &DD, &HH, &MM, &SS, &pre_millisecond);

    struct tm cur_tm;
    strptime(curTime, "%Y/%m/%d %H:%M:%S.", &cur_tm);
    time_t cur_t = mktime(&cur_tm);  // t is now your desired time_t
    // millisecond
    int cur_millisecond;
    sscanf(curTime, "%d/%d/%d %d:%d:%d.%d", &YYYY, &mm, &DD, &HH, &MM, &SS, &cur_millisecond);

    long int seconds = (long int)difftime(cur_t, pre_t);
    int milliseconds = cur_millisecond - pre_millisecond;

    if (seconds >= 1000 )
        seconds = 0;

    if ( seconds> 10 )
        vilslog::printf("getDiffTime() seconds[%ld] milliseconds[%d]\n", seconds, milliseconds);

    retdifftime = (int)seconds * 1000 + milliseconds;

    return retdifftime;
}

// void SimulationMoving::create_alert_event(int tagidx)
// {
//
//     const char * areaid = mDevManager->getTagAreaId(tagidx);
//     if ( strlen(areaid) > 0 )
//     {
//         vilslog::printf("SimulationMoving::create_alert_event() TagAreaId[%s]\n", areaid);
//         mDevManager->setTagAlertByIndex(tagidx);
//
//         MySQLDBAccess * mysqldb = new MySQLDBAccess();
//         int projectid = 1;
//         std::string eventjson = convert_to_json_str(projectid, 9999, mDevManager->getTagId(tagidx), "Alert", areaid, "0", "");
//         mysqldb->write_fired_events(projectid, 9999,
//             currentDateTimeForSQL().c_str(),
//             currentDateTimeForSQL().c_str(),
//             eventjson);
//         delete mysqldb;
//     }
// }

// void SimulationMoving::create_alert_event_no_area(int tagidx)
// {
//     mDevManager->setTagAlertByIndex(tagidx);
//
//     MySQLDBAccess * mysqldb = new MySQLDBAccess();
//     int projectid = 1;
//     std::string eventjson = convert_to_json_str(projectid, 9999, mDevManager->getTagId(tagidx), "Alert", "", "0", "");
//     mysqldb->write_fired_events(projectid, 9999,
//         currentDateTimeForSQL().c_str(),
//         currentDateTimeForSQL().c_str(),
//         eventjson);
//     delete mysqldb;
//
// }

void SimulationMoving::add_action_join(const char* macaddress, int nodetype, int idx, const char* version)
{
    DataItem * itemT = new DataItem("join", 1, macaddress, nodetype, version);
    m_queue.add(itemT);

    DataItem * jobitem = new DataItem("alive", 1, macaddress, nodetype, "3.9", "", 1, "");
    m_queue.add(jobitem);

    sleep(1);
}

// void SimulationMoving::add_action_ranging(int anchoridx, int tagidx, int rangingidx, int distance, int * imu)
// {
//     //DataItem * item = new DataItem("ranging", anchoridx, tagidx, rangingidx, distance, imu);
//     //m_queue.add(item);
//
//     DataItem * itemcc = new DataItem("rangingdiag", anchoridx, tagidx, rangingidx, distance, imu,
//                     80, 70, -105, -95, -106, -90, 0, 0, 0, "127.0.0.1");
//     m_queue.add(itemcc);
// }

void SimulationMoving::add_action_ranging(const char* AnchorMacaddress, const char* TagMacaddress, int rangingidx, int distance, int * imu)
{

    // vilslog::printf("add_action_ranging AnchorMacaddress[%s] tagMacaddress[%s] distance[%d] rangingidx[%d]\n",
    //     AnchorMacaddress, TagMacaddress, distance, rangingidx);

    DataItem * itemcc = new DataItem("rangingdiag_1", 1, AnchorMacaddress, TagMacaddress, rangingidx, distance, imu,
                    80, 70, -105, -95, -106, -90, 0, 0, 0, "127.0.0.1");
    m_queue.add(itemcc);
}

void SimulationMoving::add_action_LoRa_ranging(const char* LoRAMacaddress, const char* macaddress, int distance, int rssi)
{
    DataItem * senditem1 = new DataItem("loraranging", 1, "LoRa", LoRAMacaddress, macaddress, distance, rssi, currentDateTimeForSQL().c_str());
    mPosCalJobQueue.add(senditem1);
}



//
//
// thresholds: in meter
//
int SimulationMoving::fill_ranging(const char* TagMacaddress, int StartPosX, int StartPosY, int StartPosZ,
                            int EndPosX, int EndPosY, int EndPosZ, int totaltimeinsec,
                            int timemagininms, int thresholds, int currentIndex)
{
    return fill_ranging_floor(TagMacaddress, StartPosX, StartPosY, StartPosZ,
                                EndPosX, EndPosY, EndPosZ, totaltimeinsec,
                                timemagininms, thresholds, -100, currentIndex);
}


// void SimulationMoving::fill_multiple_tag_ranging_floor(int tag_index_start, int tag_index_end, int StartPosX, int StartPosY, int StartPosZ,
//                             int EndPosX, int EndPosY, int EndPosZ, int totaltimeinsec,
//                             int timemagininms, int thresholds, int floor)
// {
//
//     int imu[6]={0,0,-9,0,0,0};
//     //get total points
//     int totalPts = (totaltimeinsec*1000) / timemagininms;
//
//     //vilslog::printf ("Simulation::fill_ranging() totalPts = %d\n", totalPts);
//
//     double x_diff = EndPosX - StartPosX;
//     double y_diff = EndPosY - StartPosY;
//     double z_diff = EndPosZ - StartPosZ;
//     double totalDistance = sqrt(pow(x_diff, 2) + pow(y_diff, 2) + pow(z_diff, 2));
//     double stepDistance = totalDistance / totalPts;
//
//     //vilslog::printf ("Simulation::fill_ranging() totalPts[%d] totalDistance[%f] stepDistance[%f]\n",
//     //        totalPts, totalDistance, stepDistance);
//     //vilslog::printf ("Simulation::fill_ranging() x_diff[%f] y_diff[%f] z_diff[%f]\n", x_diff, y_diff, z_diff);
//     srand((unsigned int)time(NULL));
//
//     int curPtX;
//     int curPtY;
//     int curPtZ;
//     for(int i=0; i<=totalPts; i++)
//     {
//         float movoffset = stepDistance * (i);
//         float ratef = fabs( movoffset / (float)totalDistance);
//         // get point coordinator
//         //
//         // interpolation
//         //
//         curPtX = (int)(StartPosX + x_diff * ratef);
//         curPtY = (int)(StartPosY + y_diff * ratef);
//         curPtZ = (int)(StartPosZ + z_diff * ratef);
//
//         //vilslog::printf ("Simulation::fill_ranging() curPt(%d,%d,%d)\n", curPtX, curPtY, curPtZ);
//
//         int imuoffset0 = rand() % 10;
//         int imuoffset1 = rand() % 5;
//         imu[0] = imuoffset0;
//         imu[1] = imuoffset1;
//
//         //
//         // get distance from each anchor
//         //
//         int validCount = 0;
//         std::list<NodeStruct*> * anchorNodeList = mDevManager->get_Anchor_list();
//         for (std::list<NodeStruct*>::iterator node = anchorNodeList->begin(); node != anchorNodeList->end(); node++)
//         {
//             int at_x_diff = (int)(*node)->posX - curPtX;
//             int at_y_diff = (int)(*node)->posY - curPtY;
//             int at_z_diff = (int)(*node)->posZ - curPtZ;
//             double distance = sqrt(pow(at_x_diff, 2) + pow(at_y_diff, 2) + pow(at_z_diff, 2));
//
//             //vilslog::printf ("Simulation::fill_ranging() distance[%d] thresholds*100[%d]\n", (int)distance, (int)thresholds*100);
//
//             if (floor != -100)
//             {
//                 if ((*node)->buildingfloor != floor)
//                 {
//                     continue;
//                 }
//             }
//
//             mRangingIdx++;
//             if (mRangingIdx > MAX_RANGING_INDEX)
//                 mRangingIdx = 0;
//
//             for(int j=tag_index_start;j<=tag_index_end;j++)
//             {
//
//                 if ( distance < thresholds*100 )
//                 {
//     #ifdef USE_NOISE
//                     int noise = (rand()%(NOISE_MOVING_MAX*2));
//                     if ( noise > NOISE_MOVING_MAX)
//                         noise = -1*(noise - NOISE_MOVING_MAX + NOISE_MOVING_MIN);
//                     else
//                         noise += NOISE_MOVING_MIN;
//     #else
//                     int noise = 0;
//     #endif
//
//
//                     add_action_ranging((*node)->index, j, mRangingIdx, (int)distance + noise, imu);
//
//                     validCount++;
//                 }
//             }
//         }
//         //vilslog::printf ("Simulation::fill_ranging() validCount[%d]\n", validCount);
//         usleep(timemagininms * 1000);
//     }
//
// }

int SimulationMoving::fill_ranging_floor(const char* TagMacaddress, int StartPosX, int StartPosY, int StartPosZ,
                            int EndPosX, int EndPosY, int EndPosZ, int totaltimeinsec,
                            int timemagininms, int thresholds, int floor, int currentIndex)
{

    int imu[6]={0,0,-9,0,0,0};
    //get total points
    int totalPts = (totaltimeinsec*1000) / timemagininms;

    // vilslog::printf ("Simulation::fill_ranging() [%d, %d, %d] -> [%d, %d, %d]\n", StartPosX, StartPosY, StartPosZ, EndPosX, EndPosY, EndPosZ);

    double x_diff = EndPosX - StartPosX;
    double y_diff = EndPosY - StartPosY;
    double z_diff = EndPosZ - StartPosZ;
    double totalDistance = sqrt(pow(x_diff, 2) + pow(y_diff, 2) + pow(z_diff, 2));
    double stepDistance = totalDistance / totalPts;

    // vilslog::printf ("Simulation::fill_ranging() totalPts = %d\n", totalPts);
    // vilslog::printf ("Simulation::fill_ranging() totalPts[%d] totalDistance[%f] stepDistance[%f]\n",
    //        totalPts, totalDistance, stepDistance);
    //vilslog::printf ("Simulation::fill_ranging() x_diff[%f] y_diff[%f] z_diff[%f]\n", x_diff, y_diff, z_diff);
    srand((unsigned int)time(NULL));

    int curPtX;
    int curPtY;
    int curPtZ;
    for(int i=currentIndex; i<=totalPts; i++)
    {
        float movoffset = stepDistance * (i);

        float ratef = 1.0f;
        if ( (int)totalDistance != 0)
            ratef = fabs( movoffset / (float)totalDistance);
        // get point coordinator
        //
        // interpolation
        //
        curPtX = (int)(StartPosX + x_diff * ratef);
        curPtY = (int)(StartPosY + y_diff * ratef);
        curPtZ = (int)(StartPosZ + z_diff * ratef);

        //
        // real poistion
        //
        //mDevManager->setTagInfoS(tag_index+1, curPtX, curPtY, curPtZ, 1);

        //vilslog::printf ("Simulation::fill_ranging() curPt(%d,%d,%d)\n", curPtX, curPtY, curPtZ);

        int imuoffset0 = rand() % 20;
        int imuoffset1 = rand() % 5;
        //int imuoffset0 = rand() % 20;
        //int imuoffset1 = rand() % 5;
        imu[0] = imuoffset0;
        imu[1] = imuoffset1;

        //
        // get distance from each anchor
        //
        int validCount = 0;
        std::list<NodeStruct*> * anchorNodeList = mDevManager->get_Anchor_list(1);
        validCount += fill_ranging_with_list(TagMacaddress, anchorNodeList, curPtX, curPtY, curPtZ, imu, thresholds, floor);

        std::list<NodeStruct*> * locatorNodeList = mDevManager->get_Locator_list(1);
        validCount += fill_ranging_with_list(TagMacaddress, locatorNodeList, curPtX, curPtY, curPtZ, imu, thresholds, floor);


//         int validCount = 0;
//         std::list<NodeStruct*> * anchorNodeList = mDevManager->get_Anchor_list();
//         for (std::list<NodeStruct*>::iterator node = anchorNodeList->begin(); node != anchorNodeList->end(); node++)
//         {
//             // if ( (*node)->buildingfloor != 12)
//             //     continue;
//
//             int at_x_diff = (int)(*node)->posX - curPtX;
//             int at_y_diff = (int)(*node)->posY - curPtY;
//             int at_z_diff = (int)(*node)->posZ - curPtZ;
//             double distance = sqrt(pow(at_x_diff, 2) + pow(at_y_diff, 2) + pow(at_z_diff, 2));
//
//             //vilslog::printf ("Simulation::fill_ranging() distance[%d] thresholds*100[%d]\n", (int)distance, (int)thresholds*100);
//
//             if (floor != -100)
//             {
//                 if ((*node)->buildingfloor != floor)
//                 {
//                     continue;
//                 }
//             }
//
//             // if ( (*node)->index == 1 )
//             //     continue;
//
//
//             //============================
//             // random   anchor
//             //=====
//             // int anchor1or2 = (rand() % 2 );
//             // //vilslog::printf ("validCount[%d] anchor1or2[%d]\n", validCount, anchor1or2);
//             // if ( anchor1or2 == 1 )
//             // {
//             //     //vilslog::printf ("validCount[%d]\n", validCount);
//             //     continue;
//             // }
//             //============================
//
//             if ( distance < thresholds*100 )
//             {
// #ifdef USE_NOISE
//                 int noise = (rand()%(NOISE_MOVING_MAX*2));
//                 if ( noise > NOISE_MOVING_MAX)
//                     noise = -1*(noise - NOISE_MOVING_MAX + NOISE_MOVING_MIN);
//                 else
//                     noise += NOISE_MOVING_MIN;
//
//                 // large nodeise test
//                 // int noiseLarge = (rand()%(100));
//                 // if ( noiseLarge == 33 )
//                 // {
//                 //     //vilslog::printf ("Simulation::fill_ranging() noiseLarge[%d]\n", noiseLarge);
//                 //     noise += NOISE_MOVING_MAX * 4;
//                 // }
// #else
//                 int noise = 0;
// #endif
//                 mRangingIdx++;
//                 if (mRangingIdx > MAX_RANGING_INDEX)
//                     mRangingIdx = 0;
//
//                 add_action_ranging((*node)->macaddress, TagMacaddress, mRangingIdx, (int)distance + noise, imu);
//                 //vilslog::printf ("[%d] [%s] A[%d] D[%d]\n", rangingidx, (*node)->nodename, (*node)->index, (int)distance + noise);
//
//                 validCount++;
//
//
//                 // random  3 or 4 anchor
//                 // int anchor3or4 = (rand() % 2 );
//                 // if ( validCount == 3 && anchor3or4 == 1 )
//                 // {
//                 //     //vilslog::printf ("validCount[%d]\n", validCount);
//                 //     break;
//                 // }
//
//             }
//
//             // int anchorStop = (rand() % 2 );
//             // if (validCount >= 2 || anchorStop == 1)
//             // //if (validCount >= 3)
//             // {
//             //     //vilslog::printf ("validCount[%d]\n", validCount);
//             //     break;
//             // }
//         }
        //vilslog::printf ("Simulation::fill_ranging() validCount[%d]\n", validCount);
        //===========================
        currentIndex++;
        break;
        //===========================
        // usleep(timemagininms * 1000);
        //===========================
    }

    if (currentIndex > totalPts)
    {
        currentIndex = 0;
    }

    return currentIndex;
}

int SimulationMoving::fill_ranging_with_list(const char* TagMacaddress, std::list<NodeStruct*> * nodeList, int curPtX, int curPtY, int curPtZ, int * imu, int thresholds, int floor)
{
    //
    // get distance from each anchor
    //
    int validCount = 0;

    for (std::list<NodeStruct*>::iterator node = nodeList->begin(); node != nodeList->end(); node++)
    {
        // if ( (*node)->buildingfloor != 12)
        //     continue;

        int at_x_diff = (int)(*node)->posX - curPtX;
        int at_y_diff = (int)(*node)->posY - curPtY;
        int at_z_diff = (int)(*node)->posZ - curPtZ;
        double distance = sqrt(pow(at_x_diff, 2) + pow(at_y_diff, 2) + pow(at_z_diff, 2));

        // vilslog::printf ("Simulation::fill_ranging_with_list() [%s] [%d,%d,%d]  distance[%d] thresholds*100[%d]\n",
        //     (*node)->nodeid, curPtX, curPtY, curPtZ, (int)distance, (int)thresholds*100);

        if (floor != -100)
        {
            if ((*node)->buildingfloor != floor)
            {
                //vilslog::printf ("Simulation::fill_ranging_with_list() (*node)->buildingfloor != floor\n");
                continue;
            }
        }

        // if ( (*node)->index == 1 )
        //     continue;


        //============================
        // random   anchor
        //=====
        // int anchor1or2 = (rand() % 2 );
        // //vilslog::printf ("validCount[%d] anchor1or2[%d]\n", validCount, anchor1or2);
        // if ( anchor1or2 == 1 )
        // {
        //     //vilslog::printf ("validCount[%d]\n", validCount);
        //     continue;
        // }
        //============================

        if ( (distance < thresholds*100) && (distance > 0) )
        {
#ifdef USE_NOISE
            int noise = (rand()%(NOISE_MOVING_MAX*2));
            if ( noise > NOISE_MOVING_MAX)
                noise = -1*(noise - NOISE_MOVING_MAX + NOISE_MOVING_MIN);
            else
                noise += NOISE_MOVING_MIN;

            // large nodeise test
            // int noiseLarge = (rand()%(100));
            // if ( noiseLarge == 33 )
            // {
            //     //vilslog::printf ("Simulation::fill_ranging() noiseLarge[%d]\n", noiseLarge);
            //     noise += NOISE_MOVING_MAX * 4;
            // }
#else
            int noise = 0;
#endif

#ifdef RANDOM_LOST
                int random_lost = (rand()%(4));
                if ( random_lost == 1 )
                {
                    continue;
                }
#endif

            mRangingIdx++;
            if (mRangingIdx > MAX_RANGING_INDEX)
                mRangingIdx = 0;

            add_action_ranging((*node)->macaddress, TagMacaddress, mRangingIdx, (int)distance + noise, imu);
            //vilslog::printf ("[%d] [%s] A[%d] D[%d]\n", rangingidx, (*node)->nodename, (*node)->index, (int)distance + noise);

            validCount++;


            // random  3 or 4 anchor
            // int anchor3or4 = (rand() % 2 );
            // if ( validCount == 3 && anchor3or4 == 1 )
            // {
            //     //vilslog::printf ("validCount[%d]\n", validCount);
            //     break;
            // }

        }
        // else
        // {
        //     vilslog::printf ("Simulation::fill_ranging_with_list() Failed [%s] [%d,%d,%d]  distance[%d] thresholds*100[%d]\n",
        //         (*node)->nodeid, curPtX, curPtY, curPtZ, (int)distance, (int)thresholds*100);
        // }
        // int anchorStop = (rand() % 2 );
        // if (validCount >= 2 || anchorStop == 1)
        // //if (validCount >= 3)
        // {
        //     //vilslog::printf ("validCount[%d]\n", validCount);
        //     break;
        // }
    }
    return validCount;
}


void SimulationMoving::fill_LoRa_ranging_floor(const char* macaddress, int StartPosX, int StartPosY, int StartPosZ,
                            int EndPosX, int EndPosY, int EndPosZ, int totaltimeinsec,
                            int timemagininms, int thresholds, int floor)
{

    //int imu[6]={0,0,-9,0,0,0};
    //get total points
    int totalPts = (totaltimeinsec*1000) / timemagininms;

    //vilslog::printf ("Simulation::fill_ranging() totalPts = %d\n", totalPts);

    double x_diff = EndPosX - StartPosX;
    double y_diff = EndPosY - StartPosY;
    double z_diff = EndPosZ - StartPosZ;
    double totalDistance = sqrt(pow(x_diff, 2) + pow(y_diff, 2) + pow(z_diff, 2));
    double stepDistance = totalDistance / totalPts;

    //vilslog::printf ("Simulation::fill_ranging() totalPts[%d] totalDistance[%f] stepDistance[%f]\n",
    //        totalPts, totalDistance, stepDistance);
    //vilslog::printf ("Simulation::fill_ranging() x_diff[%f] y_diff[%f] z_diff[%f]\n", x_diff, y_diff, z_diff);
    srand((unsigned int)time(NULL));

    int curPtX;
    int curPtY;
    int curPtZ;
    for(int i=0; i<=totalPts; i++)
    {
        float movoffset = stepDistance * (i);
        float ratef = fabs( movoffset / (float)totalDistance);
        // get point coordinator
        //
        // interpolation
        //
        curPtX = (int)(StartPosX + x_diff * ratef);
        curPtY = (int)(StartPosY + y_diff * ratef);
        curPtZ = (int)(StartPosZ + z_diff * ratef);

        //
        // real poistion
        //
        //mDevManager->setTagInfoS(tag_index+1, curPtX, curPtY, curPtZ, 1);

        //vilslog::printf ("Simulation::fill_ranging() curPt(%d,%d,%d)\n", curPtX, curPtY, curPtZ);

        //int imuoffset0 = rand() % 10;
        //int imuoffset1 = rand() % 5;
        //int imuoffset0 = rand() % 20;
        //int imuoffset1 = rand() % 5;
        //imu[0] = imuoffset0;
        //imu[1] = imuoffset1;

        //
        // get distance from each anchor
        //
        int validCount = 0;
        std::list<NodeStruct*> * locatorNodeList = mDevManager->get_Locator_list(1);
        for (std::list<NodeStruct*>::iterator node = locatorNodeList->begin(); node != locatorNodeList->end(); node++)
        {
            // if ( (*node)->buildingfloor != 12)
            //     continue;

            int at_x_diff = (int)(*node)->posX - curPtX;
            int at_y_diff = (int)(*node)->posY - curPtY;
            int at_z_diff = (int)(*node)->posZ - curPtZ;
            double distance = sqrt(pow(at_x_diff, 2) + pow(at_y_diff, 2) + pow(at_z_diff, 2));

            //vilslog::printf ("Simulation::fill_LoRa_ranging_floor() distance[%d] thresholds*100[%d]\n", (int)distance, (int)thresholds*100);

            if (floor != -100)
            {
                if ((*node)->buildingfloor != floor)
                {
                    continue;
                }
            }

            if ( distance < thresholds*100 )
            {
#ifdef USE_NOISE
                int noise = (rand()%(NOISE_MOVING_MAX*2));
                if ( noise > NOISE_MOVING_MAX)
                    noise = -1*(noise - NOISE_MOVING_MAX + NOISE_MOVING_MIN);
                else
                    noise += NOISE_MOVING_MIN;

                // large nodeise test
                // int noiseLarge = (rand()%(100));
                // if ( noiseLarge == 33 )
                // {
                //     //vilslog::printf ("Simulation::fill_ranging() noiseLarge[%d]\n", noiseLarge);
                //     noise += NOISE_MOVING_MAX * 4;
                // }
#else
                int noise = 0;
#endif

                add_action_LoRa_ranging((*node)->LoRAMacaddress, macaddress, (int)distance + noise, -39);

                //vilslog::printf ("LoRAMacaddress[%s] Tag[%s] D[%d]\n", (*node)->LoRAMacaddress, macaddress, (int)distance + noise);

                validCount++;


                // random  3 or 4 anchor
                // int anchor3or4 = (rand() % 2 );
                // if ( validCount == 3 && anchor3or4 == 1 )
                // {
                //     //vilslog::printf ("validCount[%d]\n", validCount);
                //     break;
                // }

            }

            // int anchorStop = (rand() % 2 );
            // if (validCount >= 2 || anchorStop == 1)
            // //if (validCount >= 3)
            // {
            //     //vilslog::printf ("validCount[%d]\n", validCount);
            //     break;
            // }
        }
        //vilslog::printf ("Simulation::fill_ranging() validCount[%d]\n", validCount);
        usleep(timemagininms * 1000);
    }
}
