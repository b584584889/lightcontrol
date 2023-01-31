/*
 * AreaInOutDataClass.h
 *
 *  Created on: 2018/11/01
 *      Author: ikki
 */

#ifndef AreaInOutDataClass_H
#define AreaInOutDataClass_H

#include "../config.h"
#include "TagInOutDataClass.h"

class AreaInOutDataClass
{
public:
	AreaInOutDataClass(int dataBufferSize);
	~AreaInOutDataClass();

    void init(const char * AreaID, const char * AreaName, int MapAreaType);
	void updateAreaInfo(const char * AreaName, int MapAreaType);
    bool setNewAction(const char * TagId, int tafid, int action, int AreaType0D);

    std::string getAreaId();
    std::string getAreaName();
	int getMapAreaType();
    void dump_tag_area_inout_action();
    bool getTagLastAction(std::string TagId, AreaInOutStruct & data);
    bool getTagLastAction(std::string TagId, int action, AreaInOutStruct & data);
private:
    int mDataBufferSize;
    std::string mAreaID;
    std::string mAreaName;
	int mMapAreaType;// 0:普通 1:電子圍欄 2:電子點名 3:房間 4:教室 5:病房 6:病床 999:虛擬牆
    std::vector<TagInOutDataClass *> mTagInOutDataVector;
};


#endif
