/*
* CourseAnchorRangingDataClass.cpp
*
*  Created on: 2019/04/08
*      Author: ikki
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "string.h"

#include "../common.h"
#include "CourseAnchorRangingDataClass.h"
//
// CourseAnchorRangingDataClass
//
CourseAnchorRangingDataClass::CourseAnchorRangingDataClass()
{

}

CourseAnchorRangingDataClass::~CourseAnchorRangingDataClass()
{
}

void CourseAnchorRangingDataClass::init(const char * AnchorID)
{
    mAnchorID = AnchorID;
}

std::string CourseAnchorRangingDataClass::getAnchorId()
{
    return mAnchorID;
}

int CourseAnchorRangingDataClass::getTagCount()
{
    return (int)mTagRangingDataVector.size();
}

std::string CourseAnchorRangingDataClass::getTagId(int tagIndex)
{
    return mTagRangingDataVector[tagIndex]->getTagId();
}

int CourseAnchorRangingDataClass::getTagDataSize(int tagIndex)
{
    return (int)mTagRangingDataVector[tagIndex]->getDataSize();
}

int CourseAnchorRangingDataClass::getRangingData(int tagIndex, int dataIndex, CourseRangingDataStruct & data)
{
    return (int)mTagRangingDataVector[tagIndex]->getRanging(dataIndex, data);
}

int CourseAnchorRangingDataClass::getLastRangingData(int tagIndex, CourseRangingDataStruct & data)
{
    return (int)mTagRangingDataVector[tagIndex]->getLastRanging(data);
}

void CourseAnchorRangingDataClass::putRangingData(const char * TagId, int ranging)
{
    bool bFound = false;
    for (unsigned int i=0; i< mTagRangingDataVector.size(); i++)
    {
        // check index
        if ( mTagRangingDataVector[i]->getTagId().compare(TagId) == 0 )
        {
            //printf ("CourseAnchorRangingDataClass::putRangingData() tagIndex[%d] TagId[%s] ranging[%d]\n",
            //    tagIndex, TagId, ranging);
            mTagRangingDataVector[i]->setRanging(ranging);
            bFound = true;
            break;
        }
    }
    if (!bFound)
    {
        //printf ("CourseAnchorRangingDataClass::putRangingData() create new Vector tagIndex[%d] TagId[%s] ranging[%d]\n",
        //    tagIndex, TagId, ranging);

        CourseRangingDataClass * newData = new CourseRangingDataClass();
        newData->init(TagId);
        newData->setRanging(ranging);
        mTagRangingDataVector.push_back(newData);
    }
}
