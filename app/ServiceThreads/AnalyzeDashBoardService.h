/*
 *  AnalyzeDashBoardService.h
 *
 *  Created on: 2019/07/30
 *      Author: ikki
 */
#ifndef __AnalyzeDashBoardService_h__
#define __AnalyzeDashBoardService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Thread.h"
#include "../network/WorkItem.h"
#include "../mysql/DBManager.h"
#include "../sqlite3/AnalyzeDashBoardDBAccess.h"
#include "../DeviceManager/DeviceManager.h"

#include "../config.h"

using namespace std;

// #define ANALYZE_DASHBOARD_PRINT_MSG

#define USING_SQLITE_DB_MEMORY_CACHE

typedef struct
{
    int projectid;
    std::list<DayRecordStruct*> * m_day_records;
    std::list<ActiveTagRecordStruct*> * m_ActiveTag_records;
#ifdef USING_SQLITE_DB_MEMORY_CACHE
    AnalyzeDashBoardDBAccess * m_CacheDB;
#endif
} ProjectDayRecordStruct;

class AnalyzeDashBoardService : public Thread
{
public:
    AnalyzeDashBoardService(wqueue<WorkItem*> & analyzedashboardqueue, wqueue<WorkItem*> & trackdbqueue, DeviceManager * pDevManager, DBManager * dbManager);
    AnalyzeDashBoardService();
    ~AnalyzeDashBoardService();
    bool status();
    std::string statusString();
    void updateProject();
    bool finished();
    void dump_cache_data();
#ifdef USING_SQLITE_DB_MEMORY_CACHE
    void clear_old_data();
#endif

    void * run();

private:

    std::string mStatus;
    bool mInitial;
    bool mFinished; // before system exit(1);

    wqueue<WorkItem*> & mAnalyzeDashBoardqueue;
    wqueue<WorkItem*> & mTrackDBqueue;
    DeviceManager * mDeviceManager;
    DBManager * mDbManager;

    std::list<OperationModeRecordStruct*> mOperationModeRecordList;

    bool mUpdateProject;
    std::list<int> * mPrjidList;

    //std::list<DayRecordStruct*> * m_day_records;
    std::list<ProjectDayRecordStruct*> m_ProjectDayRecordList;

    void runloop();
    void prepare_project_record();

    void updateAliveCount();
    // void updateActivityTag();
    // void updateActivityTag(int projectid);
    // std::list<ActiveTagRecordStruct*> * prepair_ActiveTag_record(int projectid);

    void updateActivityUser();
    void updateActivityUser(int projectid);
    std::list<DayRecordStruct*> * prepair_day_record(int projectid);

    void updateAreaActivityUserCount();
    void updateAreaActivityUserCount(int projectid);
    void updateActivityOperationMode();
    void updateActivityOperationMode(int projectid);
    void updateNodeMoveOffset();
    void updateNodeMoveOffset(int projectid);

    void updateDeviceAliveCount(int projectid, std::list<NodeStruct*> * deviceList, std::string today, std::string time);
    void updateGatewayAliveCount(int projectid, std::list<LoRaGatewayInfoStruct*> * deviceList, std::string today, std::string time);
    int CheckOperationMode(int diffMS);
    int CheckOperationMode(int projectid, int diffMS);

    bool save_operationmode(int projectid, NodeStruct * node, int diffMS);

#ifdef USING_SQLITE_DB_MEMORY_CACHE
    int do_deviceactivityrecord(WorkItem * item);
    int do_arearollcallrecord(WorkItem * item);
    int do_TagMoveOffsetInfoRecord(WorkItem * item);
    int do_operationmoderecord(WorkItem * item);
#endif
};

#endif
