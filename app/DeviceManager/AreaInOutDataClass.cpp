/*
 * AreaInOutDataClass.cpp
 *
 *  Created on: 2018/11/01
 *      Author: ikki
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "string.h"

#include "../common.h"
#include "AreaInOutDataClass.h"

AreaInOutDataClass::AreaInOutDataClass(int dataBufferSize)
{
    mDataBufferSize = dataBufferSize;
    clear_TagInOut_vector(&mTagInOutDataVector);
}

AreaInOutDataClass::~AreaInOutDataClass()
{
    clear_TagInOut_vector(&mTagInOutDataVector);
}

void AreaInOutDataClass::init(const char * AreaID, const char * AreaName, int MapAreaType)
{
    mAreaID = AreaID;
    mAreaName = AreaName;
    mMapAreaType = MapAreaType;
}

void AreaInOutDataClass::updateAreaInfo(const char * AreaName, int MapAreaType)
{
    mAreaName = AreaName;
    mMapAreaType = MapAreaType;
}

bool AreaInOutDataClass::setNewAction(const char * TagId, int tafid, int action, int AreaType0D)
{
    bool bNewAction = false;
    bool bFound = false;
    for (unsigned int i=0; i< mTagInOutDataVector.size(); i++)
    {
        // check index
        if ( mTagInOutDataVector[i]->getTagId().compare(TagId) == 0 )
        {
            // vilslog::printf ("AreaInOutDataClass::setNewAction() TagId[%s] action[%d] mAreaID[%s] mAreaName[%s]\n",
            //     TagId, action, mAreaID.c_str(), mAreaName.c_str());
            bNewAction = mTagInOutDataVector[i]->setNewAction(tafid, action, AreaType0D);
            bFound = true;
            break;
        }
    }
    if (!bFound)
    {
       // vilslog::printf ("AreaInOutDataClass::setNewAction() create new Vector TagId[%s] action[%d] mAreaID[%s] mAreaName[%s]\n",
       //     TagId, action, mAreaID.c_str(), mAreaName.c_str());

        TagInOutDataClass * newData = new TagInOutDataClass(mDataBufferSize);
        newData->init(TagId);
        bNewAction = newData->setNewAction(tafid, action, AreaType0D);
        mTagInOutDataVector.push_back(newData);
    }
    return bNewAction;
}

std::string AreaInOutDataClass::getAreaId()
{
    return mAreaID;
}

std::string AreaInOutDataClass::getAreaName()
{
    return mAreaName;
}

int AreaInOutDataClass::getMapAreaType()
{
    return mMapAreaType;
}

void AreaInOutDataClass::dump_tag_area_inout_action()
{
    for (unsigned int i=0; i< mTagInOutDataVector.size(); i++)
    {
        vilslog::printf ("\tTagId[%s] --- \n", mTagInOutDataVector[i]->getTagId().c_str());

        int dataSize = mTagInOutDataVector[i]->getDataSize();
        for(int j=0;j<dataSize;j++)
        {
            AreaInOutStruct data;
            mTagInOutDataVector[i]->getAction(j, data);
            vilslog::printf ("\t\t[%d] action[%d] Time[%s]\n", j, data.action, data.DataTime.c_str());
        }
    }
}

bool AreaInOutDataClass::getTagLastAction(std::string TagId, AreaInOutStruct & data)
{
    bool bFound = false;
    for (unsigned int i=0; i< mTagInOutDataVector.size(); i++)
    {
        // check AreaId
        if ( mTagInOutDataVector[i]->getTagId().compare(TagId) == 0 )
        {
            mTagInOutDataVector[i]->getLastAction(data);
            bFound = true;
            break;
        }
    }
    return bFound;
}

bool AreaInOutDataClass::getTagLastAction(std::string TagId, int action, AreaInOutStruct & data)
{
    bool bFound = false;
    for (unsigned int i=0; i< mTagInOutDataVector.size(); i++)
    {
        // check AreaId
        if ( mTagInOutDataVector[i]->getTagId().compare(TagId) == 0 )
        {
            bFound = mTagInOutDataVector[i]->getLastAction(action, data);
            break;
        }
    }
    return bFound;
}
