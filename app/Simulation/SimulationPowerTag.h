/*
 * SimulationPowerTag.h
 *
 *  Created on: 2020/02/10
 *      Author: ikki
 */


#ifndef SIMULATIONPOWERTAG_H
#define SIMULATIONPOWERTAG_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <list>

#include "../DeviceManager/DeviceManager.h"
#include "../PosCalculate/PosCalculate.h"

#include "../network/wqueue.h"
#include "../network/DataItem.h"

class SimulationPowerTag
{
public:
	SimulationPowerTag(DeviceManager * pDevManager);
	~SimulationPowerTag();
    void start();
private:

    DeviceManager * mDevManager;
	std::list<PointStruct *> * list_cicle_point(AoARangingInfoStruct * aoainfo);
    int calc_rotate_point(AoARangingInfoStruct * aoainfo, float Xoffset, int radius, float angle, double * results);
	int calc_AoA_result(AoARangingInfoStruct * aoainfo, double * normalVector, double * origAoA, double * results);
    std::list<PointStruct *> * find_cross_circle_point(AoANodeStruct * aoaNode, std::list<PointStruct *> * ptList, int z_height, bool dumplog);
	bool find_AoA_position(double * AoAAnchor1, double * AoAAnchor2, double * AoAAnchor3,
			std::list<PointStruct *> * ptList1, std::list<PointStruct *> * ptList2, std::list<PointStruct *> * ptList3, double * results);
};

#endif
