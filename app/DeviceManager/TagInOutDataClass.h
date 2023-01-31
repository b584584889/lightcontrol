/*
 * TagInOutDataClass.h
 *
 *  Created on: 2018/11/01
 *      Author: ikki
 */

#ifndef TagInOutDataClass_H
#define TagInOutDataClass_H

#include "../config.h"

typedef struct
{
    int action;
    int AreaType0D;
    std::string DataTime;   // data create time
    time_t DataTime_t;      // data create time
    //std::string UpdateDataTime;  // data update time
    time_t UpdateDataTime_t;  // data update time
} AreaInOutStruct;

class TagInOutDataClass
{
public:
	TagInOutDataClass(int dataBufferSize);
	~TagInOutDataClass();

    int initData();
    void init(const char * TagId);
    bool setNewAction(int tafid, int action, int AreaType0D);

    std::string getTagId();
    int getTafid();
    int getDataSize();
    int getAction(int index, AreaInOutStruct & data);
    int getLastAction(AreaInOutStruct & data);
    bool getLastAction(int action, AreaInOutStruct & data);
private:
	std::string mTagId;
    int mTafid;

    int mDataBufferSize;
    int mDataSize;
    int mLatestDataIndex;

    bool mSameActionDiffTimeCheck;

    AreaInOutStruct * mAreaInOutData;
};


#endif
