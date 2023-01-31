/*
 * InspectionRangingDataClass.h
 *
 *  Created on: 2020/03/05
 *      Author: ikki
 */

#ifndef InspectionRangingDataClass_H
#define InspectionRangingDataClass_H

#include "../config.h"

#define CHECKPOINT_RANGING_DATA_HISTORY_SIZE 500

typedef struct
{
    int ranging;
    time_t DataTime;
} InspectionRangingDataStruct;

class InspectionRangingDataClass
{
public:
	InspectionRangingDataClass();
	~InspectionRangingDataClass();

    int initData();
    void init(const char * TagId);
    void setRanging(int ranging, time_t timestamp);

    std::string getTagId();
    int getDataSize();
    int getRanging(int dataIndex, InspectionRangingDataStruct & data);
    int getLastRanging(InspectionRangingDataStruct & data);
private:
	//int mTagIndex;
    std::string mTagId;

    int mDataSize;
    int mLatestDataIndex;

    InspectionRangingDataStruct mRangingData[CHECKPOINT_RANGING_DATA_HISTORY_SIZE];
};


#endif
