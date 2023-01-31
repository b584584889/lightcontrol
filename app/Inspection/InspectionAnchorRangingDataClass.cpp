/*
 * InspectionAnchorRangingDataClass.cpp
 *
 *  Created on: 2020/03/05
 *      Author: ikki
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "string.h"

#include "../common.h"
#include "InspectionAnchorRangingDataClass.h"
//
// InspectionAnchorRangingDataClass
//
InspectionAnchorRangingDataClass::InspectionAnchorRangingDataClass()
{

}

InspectionAnchorRangingDataClass::~InspectionAnchorRangingDataClass()
{
}

void InspectionAnchorRangingDataClass::init(const char * AnchorID)
{
    mAnchorID = AnchorID;
}

std::string InspectionAnchorRangingDataClass::getAnchorId()
{
    return mAnchorID;
}

int InspectionAnchorRangingDataClass::getTagCount()
{
    return (int)mTagRangingDataVector.size();
}

std::string InspectionAnchorRangingDataClass::getTagId(int vectorIndex)
{
    return mTagRangingDataVector[vectorIndex]->getTagId();
}

int InspectionAnchorRangingDataClass::getTagDataSize(int vectorIndex)
{
    return (int)mTagRangingDataVector[vectorIndex]->getDataSize();
}

int InspectionAnchorRangingDataClass::getRangingData(int vectorIndex, int dataIndex, InspectionRangingDataStruct & data)
{
    return (int)mTagRangingDataVector[vectorIndex]->getRanging(dataIndex, data);
}

int InspectionAnchorRangingDataClass::getLastRangingData(int vectorIndex, InspectionRangingDataStruct & data)
{
    return (int)mTagRangingDataVector[vectorIndex]->getLastRanging(data);
}

void InspectionAnchorRangingDataClass::putRangingData(const char * TagId, int ranging, time_t timestamp)
{
    bool bFound = false;
    for (unsigned int i=0; i< mTagRangingDataVector.size(); i++)
    {
        // check index
        if ( mTagRangingDataVector[i]->getTagId().compare(TagId) == 0 )
        {
            //printf ("InspectionAnchorRangingDataClass::putRangingData() tagIndex[%d] TagId[%s] ranging[%d]\n",
            //    tagIndex, TagId, ranging);
            mTagRangingDataVector[i]->setRanging(ranging, timestamp);
            bFound = true;
            break;
        }
    }
    if (!bFound)
    {
        //printf ("InspectionAnchorRangingDataClass::putRangingData() create new Vector tagIndex[%d] TagId[%s] ranging[%d]\n",
        //    tagIndex, TagId, ranging);

        InspectionRangingDataClass * newData = new InspectionRangingDataClass();
        newData->init(TagId);
        newData->setRanging(ranging, timestamp);
        mTagRangingDataVector.push_back(newData);
    }
}
