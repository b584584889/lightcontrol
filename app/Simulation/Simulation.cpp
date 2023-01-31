/*
 * Simulation.cpp
 *
 *  Created on: 2017/03/08
 *      Author: ikki
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <sys/time.h>
#include <time.h>

#include "Simulation.h"
#include "../common.h"

#define MAX_RANGING_INDEX 0xFF

// Simulation::Simulation(PosCalculate * pPosCal, DeviceManager * pDevManager, wqueue<DataItem*> & queue)
//     : mPosCal(pPosCal), mDevManager(pDevManager), m_queue(queue)
Simulation::Simulation(DeviceManager * pDevManager, wqueue<DataItem*> & queue)
    : mDevManager(pDevManager), m_queue(queue)
{
}

Simulation::~Simulation()
{
}

void Simulation::start()
{
    // add nodes
    add_action_join("0230000000000837", NODE_TYPE_COORD, 0, "2.10.0");
    add_action_join("0230000000000899", NODE_TYPE_COORD, 0, "2.10.0");

    add_action_join("0220000000000847", NODE_TYPE_ANCHOR, 0, "2.10.0");
    add_action_join("0220000000000848", NODE_TYPE_ANCHOR, 0, "2.10.0");
    add_action_join("0220000000000849", NODE_TYPE_ANCHOR, 0, "2.10.0");
    add_action_join("0220000000000850", NODE_TYPE_ANCHOR, 0, "2.10.0");

    add_action_join("0220000000000851", NODE_TYPE_ANCHOR, 0, "2.10.0");
    add_action_join("0220000000000852", NODE_TYPE_ANCHOR, 0, "2.10.0");
    add_action_join("0220000000000853", NODE_TYPE_ANCHOR, 0, "2.10.0");
    add_action_join("0220000000000854", NODE_TYPE_ANCHOR, 0, "2.10.0");
    add_action_join("0220000000000855", NODE_TYPE_ANCHOR, 0, "2.10.0");
    //add_action_join("0220000000000856", NODE_TYPE_ANCHOR, 0, "2.10.0");

    add_action_join("0210000000000819", NODE_TYPE_TAG, 0, "2.10.0");

    // set anchor info
    mDevManager->setAnchorInfo(1, "0220000000000847", "", -1, -1, -100, "", -1, true, 640,3567, 350, "");//index:0
    mDevManager->setAnchorInfo(1, "0220000000000848", "", -1, -1, -100, "", -1, true, 392,2060, 350, "");//1
    mDevManager->setAnchorInfo(1, "0220000000000849", "", -1, -1, -100, "", -1, true, 763,977, 350, "");//2
    mDevManager->setAnchorInfo(1, "0220000000000850", "", -1, -1, -100, "", -1, true, 2044, 977, 350, "");//3
    mDevManager->setAnchorInfo(1, "0220000000000851", "", -1, -1, -100, "", -1, true, 2100,2060, 350, "");//4
    mDevManager->setAnchorInfo(1, "0220000000000852", "", -1, -1, -100, "", -1, true, 1915,3569, 350, "");//5
    mDevManager->setAnchorInfo(1, "0220000000000853", "", -1, -1, -100, "", -1, true, 1595,2766, 350, "");//6
    mDevManager->setAnchorInfo(1, "0220000000000854", "", -1, -1, -100, "", -1, true, 1589,1321, 350, "");//7
    mDevManager->setAnchorInfo(1, "0220000000000855", "", -1, -1, -100, "", -1, true, 1131,479, 350, "");//8
    //mPosCal->setAnchorInfo(1, "0220000000000856", "", "", -1, true, 1915,3569, 350, "");//9

    // as real poistion
    mDevManager->setCoordInfo(1, "0230000000000899", "", -1, -1, -100, "", -1, true, 0, 0, 0, "");

    test_ranging_loop();
}

void Simulation::test_ranging_loop()
{
    int count = 0;
    int totalTime = 20;//20 seconds
    int marginTime = 200;//200 ms
    int thresholds = 20;//20 meter
    mRangingIdx = 0;

    while(1)
    {

        mDevManager->setCoordInfo(1, "0230000000000899", "", -1, -1, -100, "", -1, true, 2171, 1704, 144, "");
        fill_ranging(2430, 2363, 144, 2171, 1704, 144, totalTime, marginTime, thresholds);

        mDevManager->setCoordInfo(1, "0230000000000899", "", -1, -1, -100, "", -1, true, 2612, 955, 144, "");
        fill_ranging(2171, 1704, 144, 2612, 955, 144, totalTime, marginTime, thresholds);

        mDevManager->setCoordInfo(1, "0230000000000899", "", -1, -1, -100, "", -1, true, 2293, 160, 144, "");
        fill_ranging(2612, 955, 144, 2293, 160, 144, totalTime, marginTime, thresholds);

        mDevManager->setCoordInfo(1, "0230000000000899", "", -1, -1, -100, "", -1, true, 250, 172, 144, "");
        fill_ranging(2293, 160, 144, 250, 172, 144, totalTime, marginTime, thresholds);

        mDevManager->setCoordInfo(1, "0230000000000899", "", -1, -1, -100, "", -1, true, 675, 1392, 144, "");
        fill_ranging(250, 172, 144, 675, 1392, 144, totalTime, marginTime, thresholds);

        mDevManager->setCoordInfo(1, "0230000000000899", "", -1, -1, -100, "", -1, true, 34, 1955, 144, "");
        fill_ranging(675, 1392, 144, 34, 1955, 144, totalTime, marginTime, thresholds);

        mDevManager->setCoordInfo(1, "0230000000000899", "", -1, -1, -100, "", -1, true, 446, 3040, 144, "");
        fill_ranging(34, 1955, 144, 446, 3040, 144, totalTime, marginTime, thresholds);

        mDevManager->setCoordInfo(1, "0230000000000899", "", -1, -1, -100, "", -1, true, 948, 3292, 144, "");
        fill_ranging(446, 3040, 144, 948, 3292, 144, totalTime, marginTime, thresholds);

        mDevManager->setCoordInfo(1, "0230000000000899", "", -1, -1, -100, "", -1, true, 1223, 3549, 144, "");
        fill_ranging(948, 3292, 144, 1223, 3549, 144, totalTime, marginTime, thresholds);

        mDevManager->setCoordInfo(1, "0230000000000899", "", -1, -1, -100, "", -1, true, 1228, 2820, 144, "");
        fill_ranging(1223, 3549, 144, 1228, 2820, 144, totalTime, marginTime, thresholds);

        mDevManager->setCoordInfo(1, "0230000000000899", "", -1, -1, -100, "", -1, true, 1217, 1921, 144, "");
        fill_ranging(1228, 2820, 144, 1217, 1921, 144, totalTime, marginTime, thresholds);

        mDevManager->setCoordInfo(1, "0230000000000899", "", -1, -1, -100, "", -1, true, 2897, 829, 144, "");
        fill_ranging(1217, 1921, 144, 2897, 829, 144, totalTime, marginTime, thresholds);

        mDevManager->setCoordInfo(1, "0230000000000899", "", -1, -1, -100, "", -1, true, 2244, -114, 144, "");
        fill_ranging(2897, 829, 144, 2244, -114, 144, totalTime, marginTime, thresholds);

        mDevManager->setCoordInfo(1, "0230000000000899", "", -1, -1, -100, "", -1, true, 835, 190, 144, "");
        fill_ranging(2244, -114, 144, 835, 190, 144, totalTime, marginTime, thresholds);

        mDevManager->setCoordInfo(1, "0230000000000899", "", -1, -1, -100, "", -1, true, 181, 1368, 144, "");
        fill_ranging(835, 190, 144, 181, 1368, 144, totalTime, marginTime, thresholds);

        mDevManager->setCoordInfo(1, "0230000000000899", "", -1, -1, -100, "", -1, true, 375, 2979, 144, "");
        fill_ranging(181, 1368, 144, 375, 2979, 144, totalTime, marginTime, thresholds);

        mDevManager->setCoordInfo(1, "0230000000000899", "", -1, -1, -100, "", -1, true, 2430, 2363, 144, "");
        fill_ranging(375, 2979, 144, 2430, 2363, 144, totalTime, marginTime, thresholds);


/*
        int imu[6]={0,0,0,0,0,0};
        //53	950	52	1347	48	2087
        add_action_ranging(6, 0, 950, imu);//53
        add_action_ranging(5, 0, 1347, imu);//52
        add_action_ranging(1, 0, 2087, imu);//48
        // result( 2491, 2306, 350)   diff(3D) 222.275 diff(2D)  83.487  do_trilateration_3D()
        // result( N/A, N/A, N/A)   diff 37.202  do_leastsquares()
        //959		1335		2052
        mPosCal->setCoordInfo(1, "0230000000000899", "", "", -1, true, 2430, 2363, 144, "");
        sleep(1);


        //51	432	49	1615	53	1251	50	784
        add_action_ranging(4, 0, 432, imu);//51
        add_action_ranging(2, 0, 1615, imu);//49
        add_action_ranging(6, 0, 1251, imu);//53
        add_action_ranging(3, 0, 784, imu);//50
        // result( 2199, 1688, 153)   diff(3D) 33.481 diff(2D) 32.249   do_trilateration_3D()
        // result( 2192, 1703, 114)   diff(3D) 36.633 diff(2D) 21.024  do_leastsquares()
        //409		1599		1264
        mPosCal->setCoordInfo(1, "0230000000000899", "", "", -1, true, 2171, 1704, 144, "");
        sleep(1);

        //51	1243	49	1848	50	609
        add_action_ranging(4, 0, 1243, imu);//51
        add_action_ranging(2, 0, 1848, imu);//49
        add_action_ranging(3, 0, 609, imu);//50
        // result( 2591, 949, 85)   diff(3D) 62.913 diff(2D) 21.840 do_trilateration_3D()
        // result( 2591, 949, 83)   diff(3D) 64.792 diff(2D) 21.840 do_leastsquares()
        //1231		1847		587
        mPosCal->setCoordInfo(1, "0230000000000899", "", "", -1, true, 2612, 955, 144, "");
        sleep(1);

        //49	1763	50	871	51	1920
        add_action_ranging(2, 0, 1763, imu);//49
        add_action_ranging(3, 0, 871, imu);//50
        add_action_ranging(4, 0, 1920, imu);//51
        // result( 2320, 153, 281)   diff(3D) 139.811 diff(2D) 27.893  do_trilateration_3D()
        // result( 2320, 153, 273)   diff(3D) 131.981 diff(2D) 27.893  do_leastsquares()
        sleep(1);
        //1759		871		1922
        mPosCal->setCoordInfo(1, "0230000000000899", "", "", -1, true, 2293, 160, 144, "");


        //49	967	50	1965	48	1907
        add_action_ranging(2, 0, 967, imu);//49
        add_action_ranging(3, 0, 1965, imu);//50
        add_action_ranging(1, 0, 1907, imu);//48
        // result( 261, 162, 205)   diff(3D) 62.785 diff(2D) 14.866  do_trilateration_3D()
        // result( 260, 162, 205)   diff(3D) 62.618 diff(2D) 14.142  do_leastsquares()
        sleep(1);
        //975		1983		1914
        mPosCal->setCoordInfo(1, "0230000000000899", "", "", -1, true, 250, 172, 144, "");


        //49	485	50	1459	51	1587	48	745
        add_action_ranging(2, 0, 485, imu);//49
        add_action_ranging(3, 0, 1459, imu);//50
        add_action_ranging(4, 0, 1587, imu);//51
        add_action_ranging(1, 0, 745, imu);//48
        // result( 664, 1410, 135)   diff(3D) 22.935 diff(2D) 21.095  do_trilateration_3D()
        // result( 668, 1403, 137)   diff(3D) 14.799 diff(2D) 13.038  do_leastsquares()
        sleep(1);
        //483		1452		1489		751
        mPosCal->setCoordInfo(1, "0230000000000899", "", "", -1, true, 675, 1392, 144, "");


        //53	1802	49	1255	51	2130	48	442
        add_action_ranging(6, 0, 1802, imu);//53
        add_action_ranging(2, 0, 1255, imu);//49
        add_action_ranging(4, 0, 2130, imu);//51
        add_action_ranging(1, 0, 442, imu);//48
        // result( -6, 1955, 189)   diff(3D) 60.208 diff(2D) 40  do_trilateration_3D()
        // result( -17, 1955, 223)   diff(3D) 94.032 diff(2D) 51  do_leastsquares()
        sleep(1);
        //1806		1243		2090		440
        mPosCal->setCoordInfo(1, "0230000000000899", "", "", -1, true, 34, 1955, 144, "");


        //48	1017	51	1953	53	1205
        add_action_ranging(1, 0, 1017, imu);//48
        add_action_ranging(4, 0, 1953, imu);//51
        add_action_ranging(6, 0, 1205, imu);//53
        // result( 432, 3072, 72)   diff 80.025  mZPOS_up == 1
        // result( 432, 3072, 276)   diff 136.543  mZPOS_up == 0

        // result( 432, 3073, 276)   diff(3D) 136.781 diff(2D) 35.847  do_trilateration_3D()
        // result( 432, 3073, 270)   diff(3D) 57.533 diff(2D) 35.847  do_leastsquares()
        sleep(1);
        //995		1938		1195
        mPosCal->setCoordInfo(1, "0230000000000899", "", "", -1, true, 446, 3040, 144, "");


        //52	1104	47	472	51	1713	53	853
        add_action_ranging(5, 0, 1104, imu);//52
        add_action_ranging(0, 0, 472, imu);//47
        add_action_ranging(4, 0, 1713, imu);//51
        add_action_ranging(6, 0, 853, imu);//53
        // result( 887, 3245, 108)   diff(3D)  85.006 diff(2D) 77.006 do_trilateration_3D() 52,53,47
        // result( 1058, 3411, 197)   diff(3D)  170.499  do_trilateration_3D() 51,53,47
        // result( 888, 3228, 140)   diff(3D)  87.818 diff(2D) 87.727  do_leastsquares() 4 point
        // result( 887, 3207, 168)   diff(3D)  107.341 diff(2D) 104.623  do_leastsquares() 3 point 52,53,47
        sleep(1);
        //1054		471		1701		847
        mPosCal->setCoordInfo(1, "0230000000000899", "", "", -1, true, 948, 3292, 144, "");


        //48	1724	47	624	52	736	53	875
        add_action_ranging(1, 0, 1724, imu);//48
        add_action_ranging(0, 0, 624, imu);//47
        add_action_ranging(5, 0, 736, imu);//52
        add_action_ranging(6, 0, 875, imu);//53
        // result( 1242, 3550, 189)   diff(3D) 48.857      do_trilateration_3D()
        // result( 1217, 3554, 114)   diff(3D) 31  diff(2D)  7.810  do_trilateration_3D()
        // result( 1224, 3538, 122)   diff(3D) 24.617   diff(2D) 11.045 do_leastsquares() 4 point
        sleep(1);
        //1716		620		723		864
        mPosCal->setCoordInfo(1, "0230000000000899", "", "", -1, true, 1223, 3549, 144, "");


        //53	450	48	1120	51	1181
        add_action_ranging(6, 0, 450, imu);//53
        add_action_ranging(1, 0, 1120, imu);//48
        add_action_ranging(4, 0, 1181, imu);//51
        // result( 1204, 2797, 127)   diff(3D) 37.336  diff(2D) 33.242 do_trilateration_3D()
        // result( 1204, 2798, 126)   diff(3D) 37.202  diff(2D) 32.558  do_leastsquares()
        sleep(1);
        //438		1124		1187
        mPosCal->setCoordInfo(1, "0230000000000899", "", "", -1, true, 1228, 2820, 144, "");


        //49	1089	50	1316	51	941	53	971
        add_action_ranging(2, 0, 1089, imu);//49
        add_action_ranging(3, 0, 1316, imu);//50
        add_action_ranging(4, 0, 941, imu);//51
        add_action_ranging(6, 0, 971, imu);//53
        // result( 1190, 1934, 54)   diff(3D) 94.858  diff(2D) 29.967  do_trilateration_3D()
        // result( 1202, 1933, 59)   diff(3D) 87.144  diff(2D) 19.209  do_leastsquares()
        sleep(1);
        //1080		1303		940		971
        mPosCal->setCoordInfo(1, "0230000000000899", "", "", -1, true, 1217, 1921, 144, "");

        //  3/6 data

        //50[888]	51[1495]	54[1423]
        add_action_ranging(3, 0, 888, imu);//50
        add_action_ranging(4, 0, 1495, imu);//51
        add_action_ranging(7, 0, 1423, imu);//54
        // result( 2916, 807, 350)   diff(3D) 208.041  diff(2D) 29.069  do_trilateration_3D()
        sleep(1);
        mPosCal->setCoordInfo(1, "0230000000000899", "", "", -1, true, 2897, 829, 144, "");

        //49[1830]	50[1128]	54[1600]
        add_action_ranging(2, 0, 1830, imu);//49
        add_action_ranging(3, 0, 1128, imu);//50
        add_action_ranging(7, 0, 1600, imu);//54
        // result( 2214, -196, 350)   diff(3D) 223.741  diff(2D) 87.316  do_trilateration_3D()
        sleep(1);
        mPosCal->setCoordInfo(1, "0230000000000899", "", "", -1, true, 2244, -114, 144, "");

        //49[824]	50[1460]	54[1400]
        add_action_ranging(2, 0, 824, imu);//49
        add_action_ranging(3, 0, 1460, imu);//50
        add_action_ranging(7, 0, 1400, imu);//54
        // result( 836, 102, 350)   diff(3D) 224.011  diff(2D) 88.006  do_trilateration_3D()
        sleep(1);
        //Laser 49[810]	50[1450]	54[1386]
        mPosCal->setCoordInfo(1, "0230000000000899", "", "", -1, true, 835, 190, 144, "");

        //48[761]	49[748]	54[1439]
        add_action_ranging(1, 0, 761, imu);//48
        add_action_ranging(2, 0, 748, imu);//49
        add_action_ranging(7, 0, 1439, imu);//54
        // result( 169, 1369, 118)   diff(3D) 28.653  diff(2D) 12.042  do_leastsquares()
        sleep(1);
        //Laser 49[756]	49[742]	54[1432]
        mPosCal->setCoordInfo(1, "0230000000000899", "", "", -1, true, 181, 1368, 144, "");


        //47[704]	52[1721]	53[1266]
        add_action_ranging(0, 0, 704, imu);//47
        add_action_ranging(5, 0, 1721, imu);//52
        add_action_ranging(6, 0, 1266, imu);//53
        // result( 311, 2896, 350)   diff(3D) 231.130  diff(2D) 104.809  do_leastsquares()
        sleep(1);
        //Laser 47[712]	52[1673]	53[1257]
        mPosCal->setCoordInfo(1, "0230000000000899", "", "", -1, true, 375, 2979, 144, "");
*/

        count++;

    }

}

void Simulation::add_action_join(const char* macaddress, int nodetype, int idx, const char* version)
{
    DataItem * itemT = new DataItem("join", 1, macaddress, nodetype, version);
    m_queue.add(itemT);
    sleep(1);
}

void Simulation::add_action_ranging(int anchoridx, int tagidx, int rangingidx, int distance, int * imu)
{
    // DataItem * itemcc = new DataItem("ranging", 1, anchoridx, tagidx, rangingidx, distance, imu);
    // m_queue.add(itemcc);
}

//
//
// thresholds: in meter
//
void Simulation::fill_ranging(int StartPosX, int StartPosY, int StartPosZ,
                            int EndPosX, int EndPosY, int EndPosZ, int totaltimeinsec,
                            int timemagininms, int thresholds)
{

    int imu[6]={0,0,0,0,0,0};
    //get total points
    int totalPts = (totaltimeinsec*1000) / timemagininms;

    //printf ("Simulation::fill_ranging() totalPts = %d\n", totalPts);

    double x_diff = EndPosX - StartPosX;
    double y_diff = EndPosY - StartPosY;
    double z_diff = EndPosZ - StartPosZ;
    double totalDistance = sqrt(pow(x_diff, 2) + pow(y_diff, 2) + pow(z_diff, 2));
    double stepDistance = totalDistance / totalPts;

    //printf ("Simulation::fill_ranging() totalPts[%d] totalDistance[%f] stepDistance[%f]\n",
     //       totalPts, totalDistance, stepDistance);
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

        printf ("Simulation::fill_ranging() curPt(%d,%d,%d)\n", curPtX, curPtY, curPtZ);

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
            if ( distance < thresholds*100 )
            {
#ifdef USE_NOISE
                int noise = (rand()%(NOISE_MAX*2));
                if ( noise > NOISE_MAX)
                    noise = -1*(noise - NOISE_MAX + NOISE_MIN);
                else
                    noise += NOISE_MIN;
#else
                int noise = 0;
#endif
                mRangingIdx++;
                if (mRangingIdx > MAX_RANGING_INDEX)
                    mRangingIdx = 0;

                add_action_ranging((*node)->index, 0, mRangingIdx, (int)distance + noise, imu);
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
