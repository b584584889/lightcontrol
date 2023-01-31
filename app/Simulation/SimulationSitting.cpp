/*
 * SimulationSitting.cpp
 *
 *  Created on: 2018/08/08
 *      Author: ikki
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>

#include "SimulationSitting.h"
#include "../common.h"

#define MAX_RANGING_INDEX 0xFF

SimulationSitting::SimulationSitting(DeviceManager * pDevManager, wqueue<DataItem*> & queue, int tagIndex)
    : mDevManager(pDevManager), m_queue(queue), m_tagIndex(tagIndex)
{
}

SimulationSitting::~SimulationSitting()
{
}

void SimulationSitting::start()
{
     test_ranging_loop();
}

void SimulationSitting::test_ranging_loop()
{
    int count = 0;
    int totalTime = 4;//  seconds
    int marginTime = 200;//200 ms
    int thresholds = 20;//20 meter
    mRangingIdx = 0;

    srand((unsigned int)time(NULL) );

    double tagcoord[3] = {0,0,0};
    mDevManager->getTagCoord(1, "Tag0026", tagcoord);

    while(1)
    {
        fill_ranging(m_tagIndex, tagcoord[0]   , tagcoord[1]   , tagcoord[2], tagcoord[0]+20, tagcoord[1]+10, tagcoord[2], totalTime, marginTime, thresholds);
        fill_ranging(m_tagIndex, tagcoord[0]+20, tagcoord[1]+10, tagcoord[2], tagcoord[0]+10, tagcoord[1]+2 , tagcoord[2], totalTime, marginTime, thresholds);
        fill_ranging(m_tagIndex, tagcoord[0]+10, tagcoord[1]+2 , tagcoord[2], tagcoord[0]   , tagcoord[1]-5 , tagcoord[2], totalTime, marginTime, thresholds);
        fill_ranging(m_tagIndex, tagcoord[0]   , tagcoord[1]-5 , tagcoord[2], tagcoord[0]   , tagcoord[1]   , tagcoord[2], totalTime, marginTime, thresholds);

        count++;
    }

}

void SimulationSitting::add_action_ranging(int anchoridx, int tagidx, int rangingidx, int distance, int * imu)
{
    // DataItem * itemcc = new DataItem("ranging", 1, anchoridx, tagidx, rangingidx, distance, imu);
    // m_queue.add(itemcc);
}

//
//
// thresholds: in meter
//
void SimulationSitting::fill_ranging(int tag_index, int StartPosX, int StartPosY, int StartPosZ,
                            int EndPosX, int EndPosY, int EndPosZ, int totaltimeinsec,
                            int timemagininms, int thresholds)
{

    int imu[6]={0,0,-9,0,0,0};
    //get total points
    int totalPts = (totaltimeinsec*1000) / timemagininms;

    //printf ("Simulation::fill_ranging() totalPts = %d\n", totalPts);

    double x_diff = EndPosX - StartPosX;
    double y_diff = EndPosY - StartPosY;
    double z_diff = EndPosZ - StartPosZ;
    double totalDistance = sqrt(pow(x_diff, 2) + pow(y_diff, 2) + pow(z_diff, 2));
    double stepDistance = totalDistance / totalPts;

    //printf ("Simulation::fill_ranging() totalPts[%d] totalDistance[%f] stepDistance[%f]\n",
    //        totalPts, totalDistance, stepDistance);
    //printf ("Simulation::fill_ranging() x_diff[%f] y_diff[%f] z_diff[%f]\n", x_diff, y_diff, z_diff);
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

        //mDevManager->setTagInfoS(TAG_INDEX+1, curPtX, curPtY, curPtZ, 1);

        //printf ("Simulation::fill_ranging() curPt(%d,%d,%d)\n", curPtX, curPtY, curPtZ);

        int imuoffset0 = rand() % 10;
        int imuoffset1 = rand() % 5;
        imu[0] = imuoffset0;
        imu[1] = imuoffset1;

        //
        // get distance from each anchor
        //
        int validCount = 0;
        std::list<NodeStruct*> * anchorNodeList = mDevManager->get_Anchor_list(1);
        for (std::list<NodeStruct*>::iterator node = anchorNodeList->begin(); node != anchorNodeList->end(); node++)
        {
            int at_x_diff = (int)(*node)->posX - curPtX;
            int at_y_diff = (int)(*node)->posY - curPtY;
            int at_z_diff = (int)(*node)->posZ - curPtZ;
            double distance = sqrt(pow(at_x_diff, 2) + pow(at_y_diff, 2) + pow(at_z_diff, 2));

            //printf ("Simulation::fill_ranging() distance[%d] thresholds*100[%d]\n", (int)distance, (int)thresholds*100);

            if ( distance < thresholds*100 )
            {
#ifdef SIMU_SIT_USE_NOISE
                int noise = (rand()%(SIMU_SIT_NOISE_MOVING_MAX*2));
                if ( noise > SIMU_SIT_NOISE_MOVING_MAX)
                    noise = -1*(noise - SIMU_SIT_NOISE_MOVING_MAX + SIMU_SIT_NOISE_MOVING_MIN);
                else
                    noise += SIMU_SIT_NOISE_MOVING_MIN;
#else
                int noise = 0;
#endif
                mRangingIdx++;
                if (mRangingIdx > MAX_RANGING_INDEX)
                    mRangingIdx = 0;

                add_action_ranging((*node)->index, tag_index, mRangingIdx, (int)distance + noise, imu);
                //printf ("[%d] [%s] A[%d] D[%d]\n", rangingidx, (*node)->nodename, (*node)->index, (int)distance + noise);

                validCount++;

                if (validCount >= 4)
                    break;
            }
        }
        //printf ("Simulation::fill_ranging() validCount[%d]\n", validCount);
        usleep(timemagininms * 1000);
    }

}
