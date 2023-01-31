/*
 * InspectionRangingDataClass.cpp
 *
 *  Created on: 2020/03/05
 *      Author: ikki
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <iostream>
#include <ctime>
#include "string.h"

#include "../common.h"
#include "InspectionRangingDataClass.h"

InspectionRangingDataClass::InspectionRangingDataClass()
{
    mDataSize = 0;
    mLatestDataIndex = -1;
    initData();
}

InspectionRangingDataClass::~InspectionRangingDataClass()
{
}

int InspectionRangingDataClass::initData()
{
    for (int i=0;i<CHECKPOINT_RANGING_DATA_HISTORY_SIZE;i++)
    {
        mRangingData[i].ranging = 0;
        mRangingData[i].DataTime = 0;
    }
    return 0;
}

void InspectionRangingDataClass::init(const char * TagId)
{
    //mTagIndex = TagIndex;
    mTagId = TagId;
}

void InspectionRangingDataClass::setRanging(int ranging, time_t timestamp)
{
    mLatestDataIndex++;

    if (mLatestDataIndex >= CHECKPOINT_RANGING_DATA_HISTORY_SIZE)
    {
        // vilslog::printf("InspectionRangingDataClass::setRanging() mTagId[%s] mLatestDataIndex[%d]\n", mTagId.c_str(), mLatestDataIndex);

        mLatestDataIndex = 0;
    }
    if (mDataSize < CHECKPOINT_RANGING_DATA_HISTORY_SIZE)
    {
        mDataSize++;
    }

    mRangingData[mLatestDataIndex].ranging = ranging;
    mRangingData[mLatestDataIndex].DataTime = timestamp;
}

std::string InspectionRangingDataClass::getTagId()
{
    return mTagId;
}

int InspectionRangingDataClass::getDataSize()
{
    return mDataSize;
}

int InspectionRangingDataClass::getRanging(int dataIndex, InspectionRangingDataStruct & data)
{
    //printf("TagInOutDataClass::getAction() index[%d] mLatestDataIndex[%d]\n", index, mLatestDataIndex);

    int QueryIndex = mLatestDataIndex - dataIndex;

    //printf("TagInOutDataClass::getAction() QueryIndex[%d] mDataSize[%d]\n", QueryIndex, mDataSize);

    if (QueryIndex < 0)
        QueryIndex = QueryIndex + CHECKPOINT_RANGING_DATA_HISTORY_SIZE;

    //printf("TagInOutDataClass::getAction() QueryIndex[%d]\n", QueryIndex);

    data.ranging = mRangingData[QueryIndex].ranging;
    data.DataTime = mRangingData[QueryIndex].DataTime;
    return data.ranging;
}

int InspectionRangingDataClass::getLastRanging(InspectionRangingDataStruct & data)
{
    data.ranging = mRangingData[mLatestDataIndex].ranging;
    data.DataTime = mRangingData[mLatestDataIndex].DataTime;
    return data.ranging;
}
