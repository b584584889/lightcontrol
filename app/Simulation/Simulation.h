/*
 * Simulation.h
 *
 *  Created on: 2017/03/08
 *      Author: ikki
 */


#ifndef SIMULATION_H
#define SIMULATION_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <list>

#include "../PosCalculate/PosCalculate.h"
#include "../DeviceManager/DeviceManager.h"

#include "../network/wqueue.h"
#include "../network/DataItem.h"


#define USE_NOISE
#define NOISE_MAX 10
#define NOISE_MIN 5 // at least 5 cm noise


class Simulation
{
public:
	//Simulation(PosCalculate * pPosCal, DeviceManager * pDevManager, wqueue<DataItem*> & queue);
    Simulation(DeviceManager * pDevManager, wqueue<DataItem*> & queue);
	~Simulation();
    void start();
private:
    //PosCalculate * mPosCal;
    DeviceManager * mDevManager;

    wqueue<DataItem*> & m_queue;
    int mRangingIdx;

    void test_ranging_loop();

    void add_action_join(const char* macaddress, int nodetype, int idx, const char* version);
    void add_action_ranging(int anchoridx, int tagidx, int rangingidx, int distance, int * imu);

    void fill_ranging(int StartPosX, int StartPosY, int StartPosZ,
                      int EndPosX, int EndPosY, int EndPosZ, int totaltimeinsec, int timemagininms, int thresholds);

};

#endif
