/*
 * Moving.h
 *
 *  Created on: 2018/05/09
 *      Author: ikki
 */


#ifndef SIMULATIONMOVING_H
#define SIMULATIONMOVING_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <list>
#include <random>

#include "../PosCalculate/PosCalculate.h"
#include "../DeviceManager/DeviceManager.h"

#include "../network/wqueue.h"
#include "../network/DataItem.h"


// #define USE_NOISE
#define NOISE_MOVING_MAX 20
#define NOISE_MOVING_MIN 5
// #define NOISE_MOVING_MAX 10
// #define NOISE_MOVING_MIN 2

//#define RANDOM_LOST

typedef struct
{
    std::string macaddress;
	int curIndex;
	int startPosX;
	int startPosY;
	int startPosZ;
	int endPosX;
	int endPosY;
	int endPosZ;
	int totalTime;
} RandomTagStruct;

class SimulationMoving
{
public:
	SimulationMoving(DeviceManager * pDevManager, wqueue<DataItem*> & queue, wqueue<DataItem*> & PosCalJobQueue);
	~SimulationMoving();
    void start();
private:
    //PosCalculate * mPosCal;
    DeviceManager * mDevManager;
    wqueue<DataItem*> & m_queue;
    wqueue<DataItem*> & mPosCalJobQueue;

    //int m_tagIndex;
    int mRangingIdx;

	std::default_random_engine generator;

	void test_doctor_enter_room();
	void test_1d_ranging_loop();
    void test_fixes_ranging_loop();
    void test_AUO_loop();
	//void test_multiple_tag_loop_ihome();
    void test_ranging_loop_ihome();
    void test_ranging_loop_checkpoint();
    void test_ranging_loop_taipei_checkpoint();
    void test_ranging_loop_taipei_1D();
    void test_ranging_loop_taf_checkpoint();
    void test_ranging_loop_kaohsiungOffice();
    void test_ranging_loop_kaohsiungOffice(int tag_index_start, int tag_index_end);
    void test_ranging_loop();
	void do_soccer_random_move();
	RandomTagStruct * updateRandomTag(std::list<RandomTagStruct*> & randomTagList, const char * macaddress);

	void read_anchor_csv_File(const char * csvfilepath);
    void read_from_csv_File(const char * csvfilepath);
    int getDiffTime(const char * curTime, const char * preTime);

    void add_action_join(const char* macaddress, int nodetype, int idx, const char* version);
    //void add_action_ranging(int anchoridx, int tagidx, int rangingidx, int distance, int * imu);
    void add_action_ranging(const char* AnchorMacaddress, const char* TagMacaddress, int rangingidx, int distance, int * imu);
    void add_action_LoRa_ranging(const char* LoRAMacaddress, const char* macaddress, int distance, int rssi);

    int fill_ranging(const char* TagMacaddress, int StartPosX, int StartPosY, int StartPosZ,
                      int EndPosX, int EndPosY, int EndPosZ, int totaltimeinsec, int timemagininms, int thresholds, int currentIndex);
    // void fill_multiple_tag_ranging_floor(int tag_index_start, int tag_index_end, int StartPosX, int StartPosY, int StartPosZ,
    //                           int EndPosX, int EndPosY, int EndPosZ, int totaltimeinsec,
    //                           int timemagininms, int thresholds, int floor);
    int fill_ranging_floor(const char* TagMacaddress, int StartPosX, int StartPosY, int StartPosZ,
        int EndPosX, int EndPosY, int EndPosZ, int totaltimeinsec, int timemagininms, int thresholds, int floor, int currentIndex);

    int fill_ranging_with_list(const char* TagMacaddress, std::list<NodeStruct*> * nodeList, int curPtX, int curPtY, int curPtZ, int * imu, int thresholds, int floor);

    void fill_LoRa_ranging_floor(const char* macaddress, int StartPosX, int StartPosY, int StartPosZ,
        int EndPosX, int EndPosY, int EndPosZ, int totaltimeinsec, int timemagininms, int thresholds, int floor);

};

#endif
