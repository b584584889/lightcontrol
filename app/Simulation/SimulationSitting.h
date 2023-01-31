/*
 * SimulationSitting.h
 *
 *  Created on: 2018/05/09
 *      Author: ikki
 */


#ifndef SIMULATIONSITTING_H
#define SIMULATIONSITTING_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <list>

#include "../PosCalculate/PosCalculate.h"
#include "../DeviceManager/DeviceManager.h"

#include "../network/wqueue.h"
#include "../network/DataItem.h"

#define SIMU_SIT_USE_NOISE
#define SIMU_SIT_NOISE_MOVING_MAX 5
#define SIMU_SIT_NOISE_MOVING_MIN 3

class SimulationSitting
{
public:
	SimulationSitting(DeviceManager * pDevManager, wqueue<DataItem*> & queue, int tagIndex);
	~SimulationSitting();
    void start();
private:
    //PosCalculate * mPosCal;
    DeviceManager * mDevManager;
    wqueue<DataItem*> & m_queue;
    int m_tagIndex;

    int mRangingIdx;

    void test_ranging_loop();

    void add_action_ranging(int anchoridx, int tagidx, int rangingidx, int distance, int * imu);

    void fill_ranging(int tag_index, int StartPosX, int StartPosY, int StartPosZ,
                      int EndPosX, int EndPosY, int EndPosZ, int totaltimeinsec, int timemagininms, int thresholds);
};

#endif
