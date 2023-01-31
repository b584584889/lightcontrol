/*
 *  AreaRollCallService.h
 *
 *  Created on: 2019/07/30
 *      Author: ikki
 */
#ifndef __AreaRollCallService_h__
#define __AreaRollCallService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../common.h"
#include "../config.h"
#include "../network/WorkItem.h"
#include "../DeviceManager/DeviceManager.h"
#include "../PosCalculate/RayCasting.h"
#include "../sqlite3/AreaRollCallInfoDBAccess.h"

using namespace std;

// #define PRINT_AREA_ROLLCALL_LOG

#define NODE_ALIVE_TIMEOUT 30000
#define NODE_TYPE_2_ALIVE_TIMEOUT 50000
#define AREA_INOUT_BUFFER_TIMEOUT 1500
#define AREA_INOUT_BUFFER_NOT_MOVING_TIMEOUT 1000

#define NODE_SLEEP_POOL_TIMEOUT 2000
#define NODE_SLEEP_POOL_COUNT 5
#define NODE_SLEEP_POOL_TYPE_2_TIMEOUT 5000
#define NODE_SLEEP_POOL_TYPE_2_COUNT 5

typedef struct
{
    int projectid;
    RayCasting * mRaycasting;
    std::list<MapLayerAreaStruct*> * mAreaList;
    //std::list<AreaStruct*> * mAreaList;
    AreaRollCallInfoDBAccess * m_CacheDB;
} ProjectAreaInfoStruct;

class AreaRollCallService : public Thread
{
public:
    AreaRollCallService(wqueue<AreaInOutEventStruct*> & mareaInOutEventQueue, wqueue<AreaInOutEventStruct*> & mcourseAreaInOutEventQueue, wqueue<AreaInOutEventStruct*> & pWebSocketAreaInOutEventQueue, DeviceManager * pDevManager, wqueue<WorkItem*> & trackdbqueue, DBManager * dbManager);
    ~AreaRollCallService();
    bool status();
    std::string statusString();
    void updateProject();

    void * run();
    void updateAreaInfo();
    void dump_cache_data();
    void print_TagAreaBuffer();

private:

    bool mInitial;
    std::string mStatus;

    wqueue<AreaInOutEventStruct*> & mAreaInOutEventQueue;
    wqueue<AreaInOutEventStruct*> & mCourseAreaInOutEventQueue;
    wqueue<AreaInOutEventStruct*> & mWebSocketAreaInOutEventQueue;

    DeviceManager * mDeviceManager;
    wqueue<WorkItem*> & mTrackDBqueue;
    DBManager * mDbManager;

    std::list<AreaRollCallRecordStruct*> mRollCallRecordList;

    bool mUpdateProject;
    std::list<int> * mPrjidList;

	bool mUpdateAreaInfo;
    std::vector<ProjectAreaInfoStruct *> mProjectAreaInfoVector;

    bool mUpdateAreaBuffer;
    std::list<TagAreaBufferStruct*> mTagAreaBufferList;
    std::list<SleepPoolStruct*> mTagSleepPoolList;

    void runloop();
    void initialAreaInfo();
    void reloadAreaList();
    void reloadAreaList(ProjectAreaInfoStruct * areainfo);
    AreaRollCallInfoDBAccess * create_cacheDB(int projectid, ProjectAreaInfoStruct * areainfo);
    void assign_TagAreaBuffer_cache_data(int projectid, std::list<TagAreaBufferStruct*> * tagAreaBufferList);
    void assign_TagSleepPool_cache_data(int projectid, std::list<SleepPoolStruct*> * sleepPoolStructList);

    void prepairTagBuffer();
    void prepairUserBuffer();

    int checkTagArea();
    int checkTagArea(ProjectAreaInfoStruct * areainfo);
    int checkTagArea(ProjectAreaInfoStruct * areainfo, int isuser, std::string nodeid, int tafid, std::string AreaId0D, int AreaType0D, double velocity, std::string voltage, int posX, int posY, int posZ, int maplayer, std::string exclude_rollcall_AreaId, int RangingResultType, int Opertaion_Mode);

    int checkTagSleep();
    void move_to_sleeping_pool(int projectid, std::string nodeid, int tafid);
    void move_from_sleeping_pool(int projectid, std::string nodeid);
    bool check_sleep_pool(int projectid, std::string nodeid);
    bool update_cache_TagAreaBuffer(TagAreaBufferStruct * tagarea, BufferAreaInfoStruct * bufferarea);
    bool remove_cache_TagAreaBuffer(TagAreaBufferStruct * tagarea, BufferAreaInfoStruct * bufferarea);
    bool update_cache_SleepPool(int projectid, SleepPoolStruct * newPool);
    bool remove_cache_TagAreaBuffer(int projectid, TagAreaBufferStruct * tagarea);
    bool remove_cache_SleepPool(int projectid, SleepPoolStruct * pool);

    bool save_rollcall(int projectid, std::string nodeid, std::string areaid, int diffMS);

    void setTagInfoArea(int projectid, MapLayerAreaStruct * area, int isuser, std::string nodeid, int tafid, std::string AreaId0D, int AreaType0D, double velocity, std::string voltage, int action);
    void setTagInfoArea(TagAreaBufferStruct * tagbuffer, MapLayerAreaStruct * area, std::string AreaId0D, int AreaType0D, double velocity, std::string Voltage, int action);
    void checkTagAreaBuffer(int diffMS);
    bool checkTagAreaBuffer(TagAreaBufferStruct* tagbuffer, int diffMS);
    int check_enter_area(TagAreaBufferStruct * tagbuffer, BufferAreaInfoStruct * bufferArea);
    int check_leave_area(TagAreaBufferStruct * tagbuffer, BufferAreaInfoStruct * bufferArea);
    bool set_tag_area_action(int projectid, int isuser, std::string nodeid, int action, std::string areaid, std::string areaname, int AreaType0D, int MapAreaType);

    void clear_TagAreaBufferStruct_list(std::list<TagAreaBufferStruct*> * plist);
    void clear_SleepPoolStruct_list(std::list<SleepPoolStruct*> * plist);
};

#endif
