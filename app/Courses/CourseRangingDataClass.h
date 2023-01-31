/*
 * CourseRangingDataClass.h
 *
 *  Created on: 2019/04/08
 *      Author: ikki
 */

#ifndef CourseRangingDataClass_H
#define CourseRangingDataClass_H

#include "../config.h"

#define COURSE_RANGING_DATA_HISTORY_SIZE 10

typedef struct
{
    int ranging;
    time_t DataTime;
} CourseRangingDataStruct;

class CourseRangingDataClass
{
public:
	CourseRangingDataClass();
	~CourseRangingDataClass();

    int initData();
    void init(const char * TagId);
    void setRanging(int ranging);

    std::string getTagId();
    int getDataSize();
    int getRanging(int index, CourseRangingDataStruct & data);
    int getLastRanging(CourseRangingDataStruct & data);
private:
	std::string mTagId;

    int mDataSize;
    int mLatestDataIndex;

    CourseRangingDataStruct mRangingData[COURSE_RANGING_DATA_HISTORY_SIZE];
};


#endif
