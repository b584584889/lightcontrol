
/*
 * PowerTagCalculate.h
 *
 *  Created on: 2020/02/12
 *      Author: ikki
 */

#ifndef POWERTAG_CALCULATE_H
#define POWERTAG_CALCULATE_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <list>

#include "../config.h"
#include "../common.h"
#include "../logger/Logger.h"
#include "../DeviceManager/DeviceManager.h"
#include "../PosCalculate/PosCalculate.h"

#define MAX_AOA_CALC_COUNT 3

typedef struct
{
    int projectid;
    std::string tagId;
    time_t issuetime;
} PowerTagInfoStruct;

typedef struct
{
	int validCount;
	std::string tagId[MAX_AOA_CALC_COUNT];
	float tagAngle[MAX_AOA_CALC_COUNT];
} AoAAngleInfoStruct;

class PowerTagCalculate
{
public:
	PowerTagCalculate(DeviceManager * pDevManager, DBManager * dbManager);
	~PowerTagCalculate();

    int setLoRARangingAOA(int projectid, const char * LoRA_macaddress, const char * Tag_macaddress, int Distance, int velocity, float angle, int rssi, const char * timestamp);
	bool setAoATag(int projectid, std::string tagId);
	int aoaResult(int projectid, std::string aoaId);
private:

	float mTestAngle;

    DeviceManager * mDeviceManager;
    DBManager * mDbManager;
	std::vector<ProjectRangingNodeInfoStruct *> mProjectRangingNodeInfoVector;
    std::list<PowerTagInfoStruct *> mPowerTagInfoList;

    void initProjectRangingNodeInfo();
    ProjectRangingNodeInfoStruct * createRangingNodeInfo(int projectid);
	ProjectRangingNodeInfoStruct * getProjectRangingNodeInfo(int projectid);
    bool loadAnchorNode(ProjectRangingNodeInfoStruct * rangingNodeInfo);

    void clear_ProjectRangingNodeInfoStruct_vector(std::vector<ProjectRangingNodeInfoStruct*> * plist);
	int prepareAoAAngleData(int projectid, std::string aoaId, AoAAngleInfoStruct * aoAAngleInfoData);
	int checkAoAResult(int projectid, std::string aoaId, AoAAngleInfoStruct * aoAAngleInfoData);
	AoARangingInfoStruct * prepareAoARangingInfo();

	std::list<PointStruct *> * list_cicle_point(AoARangingInfoStruct * aoainfo);
    int calc_rotate_point(AoARangingInfoStruct * aoainfo, float Xoffset, int radius, float angle, double * results);
	int calc_AoA_result(AoARangingInfoStruct * aoainfo, double * normalVector, double * origAoA, double * results);
    std::list<PointStruct *> * find_cross_circle_point(AoANodeStruct * aoaNode, std::list<PointStruct *> * ptList, int z_height, bool dumplog);
	bool find_AoA_position(double * AoAAnchor1, double * AoAAnchor2, double * AoAAnchor3,
			std::list<PointStruct *> * ptList1, std::list<PointStruct *> * ptList2, std::list<PointStruct *> * ptList3, double * results);

};

#endif
