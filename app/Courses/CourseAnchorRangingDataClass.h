/*
* CourseAnchorRangingDataClass.h
*
*  Created on: 2019/04/08
*      Author: ikki
*/


#ifndef _CourseAnchorRangingDataClass_H
#define _CourseAnchorRangingDataClass_H

#include "../config.h"
#include "CourseRangingDataClass.h"

class CourseAnchorRangingDataClass
{
public:
	CourseAnchorRangingDataClass();
	~CourseAnchorRangingDataClass();

    void init(const char * AnchorID);
    std::string getAnchorId();
    int getTagCount();
    std::string getTagId(int tagIndex);
    int getTagDataSize(int tagIndex);
    int getRangingData(int tagIndex, int dataIndex, CourseRangingDataStruct & data);
    int getLastRangingData(int tagIndex, CourseRangingDataStruct & data);
    void putRangingData(const char * TagId, int ranging);
private:
    std::string mAnchorID;
    std::vector<CourseRangingDataClass *> mTagRangingDataVector;
};


#endif
