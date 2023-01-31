/*
* CourseRangingDataClass.cpp
*
*  Created on: 2019/04/08
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
#include "CourseRangingDataClass.h"

CourseRangingDataClass::CourseRangingDataClass()
{
    mDataSize = 0;
    mLatestDataIndex = -1;
    initData();
}

CourseRangingDataClass::~CourseRangingDataClass()
{
}

int CourseRangingDataClass::initData()
{
    for (int i=0;i<COURSE_RANGING_DATA_HISTORY_SIZE;i++)
    {
        mRangingData[i].ranging = 0;
        mRangingData[i].DataTime = 0;
    }
    return 0;
}

void CourseRangingDataClass::init(const char * TagId)
{
    mTagId = TagId;
}

void CourseRangingDataClass::setRanging(int ranging)
{
    mLatestDataIndex++;

    if (mLatestDataIndex >= COURSE_RANGING_DATA_HISTORY_SIZE)
    {
        mLatestDataIndex = 0;
    }
    if (mDataSize < COURSE_RANGING_DATA_HISTORY_SIZE)
    {
        mDataSize++;
    }

    mRangingData[mLatestDataIndex].ranging = ranging;
    mRangingData[mLatestDataIndex].DataTime = time(0);
}

std::string CourseRangingDataClass::getTagId()
{
    return mTagId;
}

int CourseRangingDataClass::getDataSize()
{
    return mDataSize;
}

int CourseRangingDataClass::getRanging(int index, CourseRangingDataStruct & data)
{
    int QueryIndex = mLatestDataIndex - index;

    if (QueryIndex < 0)
        QueryIndex = QueryIndex + CHECKPOINT_RANGING_DATA_HISTORY_SIZE;

    data.ranging = mRangingData[QueryIndex].ranging;
    data.DataTime = mRangingData[QueryIndex].DataTime;
    return data.ranging;
}

int CourseRangingDataClass::getLastRanging(CourseRangingDataStruct & data)
{
    data.ranging = mRangingData[mLatestDataIndex].ranging;
    data.DataTime = mRangingData[mLatestDataIndex].DataTime;
    return data.ranging;
}
