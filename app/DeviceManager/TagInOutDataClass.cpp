/*
 * TagInOutDataClass.cpp
 *
 *  Created on: 2018/11/01
 *      Author: ikki
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "string.h"

#include "../common.h"
#include "TagInOutDataClass.h"

TagInOutDataClass::TagInOutDataClass(int dataBufferSize)
{
    mDataBufferSize = dataBufferSize;
    mDataSize = 0;
    mLatestDataIndex = -1;
    mSameActionDiffTimeCheck = false;
    initData();
}

TagInOutDataClass::~TagInOutDataClass()
{
    delete [] mAreaInOutData;
}

int TagInOutDataClass::initData()
{
    mAreaInOutData = new AreaInOutStruct[mDataBufferSize];

    for (int i=0;i<mDataBufferSize;i++)
    {
        mAreaInOutData[i].action = 0;
        mAreaInOutData[i].AreaType0D = 0;
        mAreaInOutData[i].DataTime = "";
        mAreaInOutData[i].DataTime_t = 0;
        mAreaInOutData[i].UpdateDataTime_t = 0;
    }
    return 0;
}

void TagInOutDataClass::init(const char * TagId)
{
    mTagId = TagId;
}

bool TagInOutDataClass::setNewAction(int tafid, int action, int AreaType0D)
{
    mTafid = tafid;

    time_t previousDataTime_t = mAreaInOutData[mLatestDataIndex].UpdateDataTime_t;

    //
    // check last action
    //
    if ( mAreaInOutData[mLatestDataIndex].action == action )
    {
        // same action , update time.
        mAreaInOutData[mLatestDataIndex].UpdateDataTime_t = getCurrentTimeInSec();

        time_t diff_t = mAreaInOutData[mLatestDataIndex].UpdateDataTime_t - previousDataTime_t;

        // vilslog::printf("TagInOutDataClass::setNewAction() same action[%d] mTagId[%s] diff_t[%d]\n", action, mTagId.c_str(), (int)diff_t);

        if ( (mSameActionDiffTimeCheck == false) && (int)diff_t >= 3 )
        {
            mSameActionDiffTimeCheck = true;
            return true;
        }
        else
        {
            return false;
        }
    }

    mLatestDataIndex++;

    if (mLatestDataIndex >= mDataBufferSize)
    {
        mLatestDataIndex = 0;
    }
    if (mDataSize < mDataBufferSize)
    {
        mDataSize++;
    }

    mAreaInOutData[mLatestDataIndex].action = action;
    mAreaInOutData[mLatestDataIndex].AreaType0D = AreaType0D;
    mAreaInOutData[mLatestDataIndex].DataTime = currentDateTimeForSQL();
    mAreaInOutData[mLatestDataIndex].DataTime_t = getCurrentTimeInSec();
    mAreaInOutData[mLatestDataIndex].UpdateDataTime_t = getCurrentTimeInSec();

    time_t diff_t = mAreaInOutData[mLatestDataIndex].UpdateDataTime_t - previousDataTime_t;

    // vilslog::printf("TagInOutDataClass::setNewAction() diff action[%d] mTagId[%s] diff_t[%d]\n", action, mTagId.c_str(), (int)diff_t);

    if ( (int)diff_t >= 3 )
    {
        return true;
    }
    else
    {
        return false;
    }
}

std::string TagInOutDataClass::getTagId()
{
    return mTagId;
}

int TagInOutDataClass::getTafid()
{
    return mTafid;
}

int TagInOutDataClass::getDataSize()
{
    return mDataSize;
}

int TagInOutDataClass::getAction(int index, AreaInOutStruct & data)
{
    //printf("TagInOutDataClass::getAction() index[%d] mLatestDataIndex[%d]\n", index, mLatestDataIndex);

    int QueryIndex = mLatestDataIndex - index;

    //printf("TagInOutDataClass::getAction() QueryIndex[%d] mDataSize[%d]\n", QueryIndex, mDataSize);

    if (QueryIndex < 0)
        QueryIndex = QueryIndex + mDataBufferSize;

    //printf("TagInOutDataClass::getAction() QueryIndex[%d]\n", QueryIndex);

    data.action = mAreaInOutData[QueryIndex].action;
    data.AreaType0D = mAreaInOutData[QueryIndex].AreaType0D;
    data.DataTime = mAreaInOutData[QueryIndex].DataTime;
    data.DataTime_t = mAreaInOutData[QueryIndex].DataTime_t;
    data.UpdateDataTime_t = mAreaInOutData[QueryIndex].UpdateDataTime_t;

    return data.action;
}

int TagInOutDataClass::getLastAction(AreaInOutStruct & data)
{
    data.action = mAreaInOutData[mLatestDataIndex].action;
    data.AreaType0D = mAreaInOutData[mLatestDataIndex].AreaType0D;
    data.DataTime = mAreaInOutData[mLatestDataIndex].DataTime;
    data.DataTime_t = mAreaInOutData[mLatestDataIndex].DataTime_t;
    data.UpdateDataTime_t = mAreaInOutData[mLatestDataIndex].UpdateDataTime_t;
    return data.action;
}

bool TagInOutDataClass::getLastAction(int action, AreaInOutStruct & data)
{
    // serach forward
    int target_index = mLatestDataIndex;
    for (int i=0;i<mDataBufferSize;i++)
    {
        target_index = mLatestDataIndex - i;
        if (target_index < 0)
            target_index = target_index + mDataBufferSize;

//        vilslog::printf("TagInOutDataClass::getLastAction() action[%d] target_index[%d]\n", action, target_index);

        if ( mAreaInOutData[target_index].action == action )
        {
            // same action return
            data.action = mAreaInOutData[target_index].action;
            data.AreaType0D = mAreaInOutData[target_index].AreaType0D;
            data.DataTime = mAreaInOutData[target_index].DataTime;
            data.DataTime_t = mAreaInOutData[target_index].DataTime_t;
            data.UpdateDataTime_t = mAreaInOutData[target_index].UpdateDataTime_t;
            return true;
        }
    }

    return false;
}
