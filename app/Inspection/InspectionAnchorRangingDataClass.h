/*
 * InspectionAnchorRangingDataClass.h
 *
 *  Created on: 2020/03/05
 *      Author: ikki
 */


#ifndef _InspectionAnchorRangingDataClass_H
#define _InspectionAnchorRangingDataClass_H

#include "../config.h"
#include "InspectionRangingDataClass.h"

class InspectionAnchorRangingDataClass
{
public:
	InspectionAnchorRangingDataClass();
	~InspectionAnchorRangingDataClass();

    void init(const char * AnchorID);
    std::string getAnchorId();
    int getTagCount();
    std::string getTagId(int vectorIndex);
    int getTagDataSize(int vectorIndex);
    int getRangingData(int vectorIndex, int dataIndex, InspectionRangingDataStruct & data);
    int getLastRangingData(int vectorIndex, InspectionRangingDataStruct & data);
    void putRangingData(const char * TagId, int ranging, time_t timestamp);
private:
    std::string mAnchorID;
    std::vector<InspectionRangingDataClass *> mTagRangingDataVector;
};


#endif
