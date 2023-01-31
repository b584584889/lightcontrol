/*
 *  PosCalculateService.h
 *
 *  Created on: 2018/10/23
 *      Author: ikki
 */
#ifndef __PosCalculateService_h__
#define __PosCalculateService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../common.h"
#include "../config.h"
#include "../network/wqueue.h"
#include "../network/DataItem.h"
#include "../network/WorkItem.h"
#include "../DeviceManager/DeviceManager.h"
#include "../PosCalculate/PosCalculate.h"
#include "../PowerTagCalculate/PowerTagCalculate.h"
#include "../mysql/DBManager.h"
using namespace std;

typedef struct
{
    int projectid;
    std::string TagId;
    time_t issuetime;
} TagUpdateInfoStruct;

typedef struct
{
    int ranging;
    int velocity;
    int rssi;
    std::string timestamp;
    std::string LoRAMacaddress;
    time_t issuetime;
} TagRangingTimeStruct;

typedef struct
{
    int projectid;
    std::string tagId;
    std::string tagMacaddress;
    wqueue<TagRangingTimeStruct*> rangingdataQueue;
} TagRangingQueueStruct;

typedef struct
{
    int projectid;
    std::string aoaId;
    time_t issuetime;
} PowerTagAoAInfoStruct;

typedef struct
{
    int projectid;
    int posX;
    int posY;
    int posZ;
    std::string macaddress;
    time_t issuetime;
} FixPositionInfoStruct;

class PosCalculateService : public Thread
{
public:
    PosCalculateService(wqueue<DataItem*> & JobQueue, wqueue<WorkItem*> & MySQLJobQueue, wqueue<WorkItem*> & MySQLTSJobQueue, wqueue<DataItem*> & CheckPointJobQueue, wqueue<WorkItem*> & nodePositionQueue, wqueue<DataItem*> & DeviceEventJobQueue, DeviceManager * pDevManager, DBManager * dbManager);
    bool status();
    std::string statusString();
    void updateProject();

    void * run();
    int solveAllTagPosition(int projectid);
    int solveAllAoAPosition(int projectid);
    int solveFixPosition(int projectid);
    // int solveAllTagArea(int projectid);
    int solveLocatorArea(int projectid);

    std::string getRangingString(int projectid, int validdatatime);
    std::string getRangingStringEx(int projectid, int validdatatime);
    std::string getRangingDiagnosisString(int projectid, const char * tag_macaddress, int validdatatime);

    void setDumpAoACirclePoint();
    std::string getAoARangingCloudPoint(int projectid);
    std::string getAoAAngleVectorCloudPoint(int projectid);
    std::string getAoAAngleCloudPoint(int projectid);
    std::string getUnknownPositionTagString(int projectid);

private:

    std::string mStatus;
    bool mInitial;

    wqueue<DataItem*> & mJobQueue;
    wqueue<WorkItem*> & mMySQLJobQueue;
    wqueue<WorkItem*> & mMySQLTSJobQueue;
    wqueue<DataItem*> & mCheckPointJobQueue;
    wqueue<WorkItem*> & mNodePositionQueue;
    wqueue<DataItem*> & mDeviceEventJobQueue;

    PosCalculate * mPosCal;
    PowerTagCalculate * mPowerTagCal;
    DeviceManager * mDeviceManager;
    DBManager * mDbManager;

    SmoothingResult  * mSmoothingResult;

    std::list<TagUpdateInfoStruct *> mTagUpdateList;
    std::list<TagRangingQueueStruct *> mTagRangingQueueList;

    std::list<PowerTagInfoStruct *> mPowerTagInfoList;
    std::list<PowerTagAoAInfoStruct *> mPowerTagAoAInfoList;

    std::list<FixPositionInfoStruct *> mFixPositionInfoList;
    std::list<FixPositionInfoStruct *> mDiffPositionInfoList;

    std::list<SoccerRecordingStruct*> mSoccerRecordingList;

    bool mUpdateData;
    bool mUpdateProject;
    std::list<int> * mPrjidList;

    bool bEnableDumpAoACirclePoint;

    void runloop();
    void waitloop();
    void checkRangingQueue();
    int do_rangingdiag(DataItem * item);
    int do_rangingdiag_1(DataItem * item);
    int do_loraranging(DataItem * item);
    int do_lorarangingcompress(DataItem * item);
    int do_lorarangingcompressMultiDistance(DataItem * item);
    int do_lorarangingcompressaoa(DataItem * item);
    int do_PowerTagCalculateInfo(DataItem * item);
    int do_FixPosition(DataItem * item);
    int do_FixDevice(DataItem * item);
    int do_AOAfence(DataItem * item);

    bool find_empty_parking_lot(int projectid, PositionInfoStruct * newPos, std::string macaddress);
    bool put_to_fixPositionList(int projectid, std::string macaddress, int posX, int posY, int posZ);
    bool update_diffPositionList(int projectid, char * macaddress, double distance, double * rawresult);

    int do_updatlocatorposition(DataItem * item);
    int loadSectionList();

    bool checkTagUpdateValidTime(int projectid, const char * tagid, int validtime);

    TagRangingQueueStruct * getTagRangingQueue(int projectid, std::string tagMacaddress);
    bool put_to_PowerTagInfoList(int projectid, std::string tagId);
    bool put_to_PowerTagAoAInfoList(int projectid, std::string aoaId);
    bool check_powertag_test(int projectid, const char * tagId);
    SoccerRecordingStruct * getCurrentSoccerRecording(int maingroupid);
    std::string getTagStatus(double * IMU);
};


#endif
