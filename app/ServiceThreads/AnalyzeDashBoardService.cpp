/*
 *  AnalyzeDashBoardService.cpp
 *
 *  Created on: 2019/07/30
 *      Author: ikki
 */

#include <string.h>
#include <ctime>
#include "AnalyzeDashBoardService.h"
#include "../common.h"
#include "../mysql/struct.h"

using namespace std;

AnalyzeDashBoardService::AnalyzeDashBoardService(wqueue<WorkItem*> & analyzedashboardqueue, wqueue<WorkItem*> & trackdbqueue, DeviceManager * pDevManager, DBManager * dbManager)
    : mAnalyzeDashBoardqueue(analyzedashboardqueue), mTrackDBqueue(trackdbqueue), mDeviceManager(pDevManager), mDbManager(dbManager)
{
    // if ( mMysqlDB == NULL || mMysqlTSDB == NULL || mTrackDB == NULL)
    // {
    //     mInitial = false;
    //     mStatus = "AnalyzeDashBoardService() Initial failed!!";
    //     vilslog::printf ("AnalyzeDashBoardService() Initial failed!\n");
    // }
}

AnalyzeDashBoardService::~AnalyzeDashBoardService()
{
}

bool AnalyzeDashBoardService::status()
{
    return mInitial;
}

std::string AnalyzeDashBoardService::statusString()
{
    return mStatus;
}

void AnalyzeDashBoardService::updateProject()
{
    mUpdateProject = true;
}

bool AnalyzeDashBoardService::finished()
{
    return mFinished;
}

void * AnalyzeDashBoardService::run()
{
    while (1)
    {
        mInitial = false;
        mFinished = false;
        mStatus = "AnalyzeDashBoardService() Initial failed!!";
        try
        {
            runloop();
            vilslog::printf ("AnalyzeDashBoardService() Initial failed!\n");
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("AnalyzeDashBoardService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");
            mInitial = false;
            mStatus = "AnalyzeDashBoardService::run() exception:";
            mStatus.append(exc.what());
        }
    }

    return NULL;
}

void AnalyzeDashBoardService::runloop()
{
    long long start1 = getSystemTime();
    long long updateAliveTime = 55000;
    // long long updateActivityTagTime = 35000;
    long long updateActivityUserTime = 55000;
    long long updateAreaActivityUserCountTime = 55000;
    long long updateActivityOperationModeTime = 55000;
    long long updateNodeMoveOffsetTime = 55000;
    long long checkTime = 0;
    long long updatePrjIdTime = 0;
#ifdef USING_SQLITE_DB_MEMORY_CACHE
    long long removeolddataTime = 0;
    long long dumpcacheTime = 0;
    char type[64];
#endif

    mPrjidList = mDbManager->getAllProjectConfig();

    mInitial = true;
    mStatus = "AnalyzeDashBoardService() Initial successfully!";
    vilslog::printf ("AnalyzeDashBoardService() Initial successfully!\n");


    prepare_project_record();

    // available to process.
    while (1)
    {

        WorkItem * item = mAnalyzeDashBoardqueue.removetimed(100);
        if ( item != NULL )
        {
// #ifdef ANALYZE_DASHBOARD_PRINT_MSG
//             long long doStart = getSystemTime();
// #endif

#ifdef USING_SQLITE_DB_MEMORY_CACHE
            sprintf(type, "%s", item->getType());

            if ( strcmp(type, "deviceactivityrecord") == 0)
            {
                do_deviceactivityrecord(item);

// #ifdef ANALYZE_DASHBOARD_PRINT_MSG
//                 long long doUsing = getSystemTime() - doStart;
//                 vilslog::printf ("AnalyzeDashBoardService() do_deviceactivityrecord doUsing[%d]\n", (int)doUsing);
// #endif
            }
            else
            if ( strcmp(type, "arearollcallrecord") == 0)
            {
                do_arearollcallrecord(item);

// #ifdef ANALYZE_DASHBOARD_PRINT_MSG
//                 long long doUsing = getSystemTime() - doStart;
//                 vilslog::printf ("AnalyzeDashBoardService() do_arearollcallrecord doUsing[%d]\n", (int)doUsing);
// #endif
            }
            else
            if ( strcmp(type, "TagMoveOffsetInfoRecord") == 0)
            {
                do_TagMoveOffsetInfoRecord(item);

// #ifdef ANALYZE_DASHBOARD_PRINT_MSG
//                 long long doUsing = getSystemTime() - doStart;
//                 vilslog::printf ("AnalyzeDashBoardService() do_TagMoveOffsetInfoRecord doUsing[%d]\n", (int)doUsing);
// #endif
            }
            else
            if ( strcmp(type, "operationmoderecord") == 0)
            {
                do_operationmoderecord(item);

// #ifdef ANALYZE_DASHBOARD_PRINT_MSG
//                 long long doUsing = getSystemTime() - doStart;
//                 vilslog::printf ("AnalyzeDashBoardService() operationmoderecord doUsing[%d]\n", (int)doUsing);
// #endif
            }
            else
            if ( strcmp(type, "savestate") == 0)
            {
                mFinished = false;
                printf("AnalyzeDashBoardService::run() type[%s]\n", type);

                dump_cache_data();

                printf("AnalyzeDashBoardService::run() type[%s] finished!\n", type);
                mFinished = true;
            }
#endif

            delete item;
        }

        long long end1 = getSystemTime();
        long long difft = end1 - start1;

        updateAliveTime += difft;
        if (updateAliveTime >= 60000)
        {
#ifdef ANALYZE_DASHBOARD_PRINT_MSG
            long long updateAliveStart = getSystemTime();
#endif
            updateAliveCount();

#ifdef ANALYZE_DASHBOARD_PRINT_MSG
            long long updateAliveEnd = getSystemTime();
            long long updateAliveUsing = updateAliveEnd - updateAliveStart;
            vilslog::printf ("AnalyzeDashBoardService() updateAliveTime[%d] using[%d]\n", updateAliveTime, (int)updateAliveUsing);
#endif
            difft = getSystemTime() - start1;
            updateAliveTime = updateAliveTime - 60000;
        }

//         updateActivityTagTime += difft;
//         if (updateActivityTagTime >= 60000)
//         {
// #ifdef ANALYZE_DASHBOARD_PRINT_MSG
//             long long updateActivityTagStart = getSystemTime();
// #endif
//             updateActivityTag();
// #ifdef ANALYZE_DASHBOARD_PRINT_MSG
//             long long updateActivityTagUsing = getSystemTime() - updateActivityTagStart;
//             vilslog::printf ("AnalyzeDashBoardService() updateActivityTagTime[%d] using[%d]\n", updateActivityTagTime, (int)updateActivityTagUsing);
// #endif
//
//             difft = getSystemTime() - start1;
//             updateActivityTagTime = 0;
//         }

        updateActivityUserTime += difft;
        if (updateActivityUserTime >= 60000)
        {
#ifdef ANALYZE_DASHBOARD_PRINT_MSG
            long long updateActivityUserStart = getSystemTime();
#endif
            updateActivityUser();
#ifdef ANALYZE_DASHBOARD_PRINT_MSG
            long long updateActivityUserUsing = getSystemTime() - updateActivityUserStart;
            vilslog::printf ("AnalyzeDashBoardService() updateActivityUser[%d] using[%d]\n", updateActivityUserTime, (int)updateActivityUserUsing);
#endif

            difft = getSystemTime() - start1;
            updateActivityUserTime = updateActivityUserTime - 60000;
        }

        updateAreaActivityUserCountTime += difft;
        if (updateAreaActivityUserCountTime >= 60000)
        {
#ifdef ANALYZE_DASHBOARD_PRINT_MSG
            long long updateAreaActivityUserCountStart = getSystemTime();
#endif
            updateAreaActivityUserCount();
#ifdef ANALYZE_DASHBOARD_PRINT_MSG
            long long updateAreaActivityUserCountUsing = getSystemTime() - updateAreaActivityUserCountStart;
            vilslog::printf ("AnalyzeDashBoardService() updateAreaActivityUserCount[%d] using[%d]\n", updateAreaActivityUserCountTime, (int)updateAreaActivityUserCountUsing);
#endif
            difft = getSystemTime() - start1;
            updateAreaActivityUserCountTime = updateAreaActivityUserCountTime - 60000;
        }

        updateActivityOperationModeTime += difft;
        if (updateActivityOperationModeTime >= 60000)
        {
#ifdef ANALYZE_DASHBOARD_PRINT_MSG
            long long updateActivityOperationModeStart = getSystemTime();
#endif
            updateActivityOperationMode();
#ifdef ANALYZE_DASHBOARD_PRINT_MSG
            long long updateActivityOperationModeUsing = getSystemTime() - updateActivityOperationModeStart;
            vilslog::printf ("AnalyzeDashBoardService() updateActivityOperationMode[%d] using[%d]\n", updateActivityOperationModeTime, (int)updateActivityOperationModeUsing);
#endif
            difft = getSystemTime() - start1;
            updateActivityOperationModeTime = updateActivityOperationModeTime - 60000;
        }

        updateNodeMoveOffsetTime += difft;
        if (updateNodeMoveOffsetTime >= 60000)
        {
#ifdef ANALYZE_DASHBOARD_PRINT_MSG
            long long updateNodeMoveOffsetStart = getSystemTime();
#endif
            updateNodeMoveOffset();
#ifdef ANALYZE_DASHBOARD_PRINT_MSG
            long long updateNodeMoveOffsetUsing = getSystemTime() - updateNodeMoveOffsetStart;
            vilslog::printf ("AnalyzeDashBoardService() updateNodeMoveOffset[%d] using[%d]\n", updateNodeMoveOffsetTime, (int)updateNodeMoveOffsetUsing);
#endif
            difft = getSystemTime() - start1;
            updateNodeMoveOffsetTime = updateNodeMoveOffsetTime - 60000;
        }

        checkTime += difft;

#ifdef USING_SQLITE_DB_MEMORY_CACHE
        removeolddataTime += difft;
        dumpcacheTime += difft;
#endif

        if ( checkTime >= 500) // 500 miliseconds
        {
            CheckOperationMode((int)checkTime);
            difft = getSystemTime() - start1;
            checkTime = 0;
        }

        updatePrjIdTime += difft;
        if (updatePrjIdTime > 3600000 || mUpdateProject)
        {
            mPrjidList->clear();
            delete mPrjidList;
            mPrjidList = mDbManager->getAllProjectConfig();

            updatePrjIdTime = 0;
            mUpdateProject = false;
        }

#ifdef USING_SQLITE_DB_MEMORY_CACHE
        if (dumpcacheTime >= 120000) // 2 minute
        {
#ifdef ANALYZE_DASHBOARD_PRINT_MSG
            long long dumpcacheStart = getSystemTime();
#endif

            dump_cache_data();

#ifdef ANALYZE_DASHBOARD_PRINT_MSG
            long long dumpcacheUsing = getSystemTime() - dumpcacheStart;
            vilslog::printf ("AnalyzeDashBoardService() dump_cache_data dumpcacheUsing[%d]\n", (int)dumpcacheUsing);
#endif
            dumpcacheTime = 0;
        }

        //if (removeolddataTime >= 600000) // 10 minutes
        if (removeolddataTime >= 120000) // 2 minutes
        {
#ifdef ANALYZE_DASHBOARD_PRINT_MSG
            long long clearoldStart = getSystemTime();
#endif
            // std::string time = currentTimeForSQL();

            clear_old_data();

#ifdef ANALYZE_DASHBOARD_PRINT_MSG
            long long  clearoldUsing = getSystemTime() - clearoldStart;
            vilslog::printf ("AnalyzeDashBoardService() clear_old_data clearoldUsing[%d]\n", (int)clearoldUsing);
#endif
            removeolddataTime = 0;
        }
#endif

        start1 = end1;

        usleep(100000); // 100 miliseconds
    }
}

void AnalyzeDashBoardService::prepare_project_record()
{
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);

        bool bFound = false;
        for (std::list<ProjectDayRecordStruct*>::iterator buffer = m_ProjectDayRecordList.begin(); buffer != m_ProjectDayRecordList.end(); buffer++)
        {
            if( (*buffer)->projectid == projectid )
            {
                bFound = true;
                break;
            }
        }
        if (!bFound)
        {
            ProjectDayRecordStruct * curProjectDayRecord = new ProjectDayRecordStruct();
            curProjectDayRecord->projectid = projectid;
            curProjectDayRecord->m_day_records = new std::list<DayRecordStruct*>();
            curProjectDayRecord->m_ActiveTag_records = new std::list<ActiveTagRecordStruct*>();
            m_ProjectDayRecordList.push_back(curProjectDayRecord);

#ifdef USING_SQLITE_DB_MEMORY_CACHE
            curProjectDayRecord->m_CacheDB = new AnalyzeDashBoardDBAccess();
            curProjectDayRecord->m_CacheDB->init();

            // loda sqlite3 cache from file
            curProjectDayRecord->m_CacheDB->loadOrSaveDb(projectid, 0);

            bool bExisted = curProjectDayRecord->m_CacheDB->checkDBExist();

            vilslog::printf("AnalyzeDashBoardService::prepare_project_record() m_CacheDB projectid[%d] bExisted[%d]\n", projectid, bExisted);

            if (bExisted)
            {
                curProjectDayRecord->m_CacheDB->check_valid_table();
            }
            else
            if (!bExisted)
            {
                curProjectDayRecord->m_CacheDB->init();

                //
                // copy mysql data to sqlite in-memory database
                //
                TrackDBAccess * trackDb = mDbManager->getTrackDB(projectid);
                if (trackDb != NULL)
                {
                    std::string current_datetime = currentDateTimeForSQL();
                    std::string one_day_ago = currentDateTimeForSQL(1);

                    vilslog::printf("AnalyzeDashBoardService::prepare_project_record() start...\n");
                    long long tStart = getSystemTime();

                    // std::list<AreaRollCallRecordStruct *> * areaRollCallList = trackDb->getAreaRollCallList(one_day_ago, current_datetime);
                    std::list<AreaRollCallRecordStruct *> * areaRollCallList = new std::list<AreaRollCallRecordStruct *>();

                    long long getAreaRollCallListUsing = getSystemTime() - tStart;
                    vilslog::printf("AnalyzeDashBoardService::prepare_project_record() areaRollCallList->size()[%d] getAreaRollCallListUsing[%d]\n",
                        (int)areaRollCallList->size(), (int)getAreaRollCallListUsing);

                    tStart = getSystemTime();

                    //std::list<OperationModeRecordStruct *> * operationModeList = trackDb->getOperationModeRecordList(one_day_ago, current_datetime);
                    std::list<OperationModeRecordStruct *> * operationModeList = new std::list<OperationModeRecordStruct *>();

                    long long getOperationModeRecordListUsing = getSystemTime() - tStart;
                    vilslog::printf("AnalyzeDashBoardService::prepare_project_record() operationModeList->size()[%d] getOperationModeRecordListUsing[%d]\n",
                        (int)operationModeList->size(), (int)getOperationModeRecordListUsing);

                    tStart = getSystemTime();

                    //std::list<TagMoveOffsetRecordStruct *> * tagMoveOffsetList = trackDb->getTagMoveOffsetRecordList(one_day_ago, current_datetime);
                    std::list<TagMoveOffsetRecordStruct *> * tagMoveOffsetList = new std::list<TagMoveOffsetRecordStruct *>();

                    long long getTagMoveOffsetRecordListUsing = getSystemTime() - tStart;
                    vilslog::printf("AnalyzeDashBoardService::prepare_project_record() tagMoveOffsetList->size()[%d] getTagMoveOffsetRecordListUsing[%d]\n",
                        (int)tagMoveOffsetList->size(), (int)getTagMoveOffsetRecordListUsing);

                    tStart = getSystemTime();

                    int nodetype = NODE_TYPE_TAG;
                    std::string today_date = TodayDateType1();
                    //std::list<DeviceActivityRecordStruct *> * deviceActivityRecordList = trackDb->getDeviceActivityRecordList(nodetype, today_date, 30);
                    std::list<DeviceActivityRecordStruct*> * deviceActivityRecordList = new std::list<DeviceActivityRecordStruct*>();

                    long long getDeviceActivityRecordListUsing = getSystemTime() - tStart;
                    vilslog::printf("AnalyzeDashBoardService::prepare_project_record() deviceActivityRecordList->size()[%d] getDeviceActivityRecordListUsing[%d]\n",
                        (int)deviceActivityRecordList->size(), (int)getDeviceActivityRecordListUsing);


                    for (std::list<AreaRollCallRecordStruct*>::iterator record = areaRollCallList->begin(); record != areaRollCallList->end(); record++)
                    {
                        curProjectDayRecord->m_CacheDB->write_arearollcall_record_full((*record));
                    }
                    for (std::list<OperationModeRecordStruct*>::iterator record = operationModeList->begin(); record != operationModeList->end(); record++)
                    {
                        curProjectDayRecord->m_CacheDB->write_operationmode_record_full((*record));
                    }
                    for (std::list<TagMoveOffsetRecordStruct*>::iterator record = tagMoveOffsetList->begin(); record != tagMoveOffsetList->end(); record++)
                    {
                        curProjectDayRecord->m_CacheDB->write_TagMoveOffset_record_full((*record));
                    }
                    for (std::list<DeviceActivityRecordStruct*>::iterator record = deviceActivityRecordList->begin(); record != deviceActivityRecordList->end(); record++)
                    {
                        curProjectDayRecord->m_CacheDB->write_deviceactivity_record_full((*record));
                    }

                    vilslog::printf("AnalyzeDashBoardService::prepare_project_record() Done.\n");

                    clear_AreaRollCallRecordStruct_list(areaRollCallList);
                    delete areaRollCallList;
                    clear_OperationModeRecordStruct_list(operationModeList);
                    delete operationModeList;
                    clear_TagMoveOffsetRecordStruct_list(tagMoveOffsetList);
                    delete tagMoveOffsetList;
                    clear_DeviceActivityRecordStruct_list(deviceActivityRecordList);
                    delete deviceActivityRecordList;

                    int saveStatus = curProjectDayRecord->m_CacheDB->loadOrSaveDb(projectid, 1);
                    vilslog::printf("AnalyzeDashBoardService::prepare_project_record() temp.db saveStatus[%d]\n", saveStatus);
                }
            }
#endif
        }
    }
}

void AnalyzeDashBoardService::updateAliveCount()
{
    std::string today = TodayDateType1();
    std::string time = currentTimeForSQL();

    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);

        std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);
        std::list<NodeStruct*> * anchorNodeList = mDeviceManager->get_Anchor_list(projectid);
        std::list<NodeStruct*> * coordNodeList = mDeviceManager->get_Coordinator_list(projectid);
        std::list<NodeStruct*> * locatorNodeList = mDeviceManager->get_Locator_list(projectid);
        std::list<LoRaGatewayInfoStruct*> * gatewayNodeList = mDeviceManager->get_LoRaGateway_list(projectid);

        updateDeviceAliveCount(projectid, tagNodeList, today, time);
        updateDeviceAliveCount(projectid, anchorNodeList, today, time);
        updateDeviceAliveCount(projectid, coordNodeList, today, time);
        updateDeviceAliveCount(projectid, locatorNodeList, today, time);
        updateGatewayAliveCount(projectid, gatewayNodeList, today, time);
    }
}

// void AnalyzeDashBoardService::updateActivityTag()
// {
//     for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
//     {
//         int projectid = (*prj);
//         updateActivityTag(projectid);
//     }
// }
//
// void AnalyzeDashBoardService::updateActivityTag(int projectid)
// {
//     std::list<ActiveTagRecordStruct*> * m_ActiveTag_records = prepair_ActiveTag_record(projectid);
//     if (m_ActiveTag_records == NULL)
//         return;
//
//     int nodetype = NODE_TYPE_TAG;
//     std::string today_date = TodayDateType1();
//     std::string current_time = currentTimeForSQL();
//
//     int devicecount = 0;
//     std::string aliveString = "";
//     for (std::list<ActiveTagRecordStruct*>::iterator record = m_ActiveTag_records->begin(); record != m_ActiveTag_records->end(); record++)
//     {
//         if ( record == m_ActiveTag_records->begin() )
//         {
//             devicecount = (*record)->count;
//         }
//         else
//         {
//             aliveString.append(",");
//         }
//         aliveString.append( std::to_string((*record)->count));
//     }
//
//     DeviceActivityAliveCountStruct record;
//     record.nodetype = nodetype;
//     record.devicecount = devicecount;
//     record.aliveString = aliveString;
//     record.issueday = today_date;
//     record.issuetime = current_time;
//
//     WorkItem * writem = new WorkItem("deviceactivityalivecountrecord", projectid, &record);
//     mTrackDBqueue.add(writem);
//
// }
//
// std::list<ActiveTagRecordStruct*> * AnalyzeDashBoardService::prepair_ActiveTag_record(int projectid)
// {
//     ProjectDayRecordStruct * curProjectDayRecord = NULL;
//     for (std::list<ProjectDayRecordStruct*>::iterator buffer = m_ProjectDayRecordList.begin(); buffer != m_ProjectDayRecordList.end(); buffer++)
//     {
//         if( (*buffer)->projectid == projectid )
//         {
//             curProjectDayRecord = (*buffer);
//             break;
//         }
//     }
//
//     if (curProjectDayRecord == NULL)
//     {
//         return NULL;
//     }
//
//     int nodetype = NODE_TYPE_TAG;
//     std::string today_date = TodayDateType1();
//     if (curProjectDayRecord->m_ActiveTag_records->size() == 0)
//     {
//         // curProjectDayRecord = new ProjectDayRecordStruct();
//         // curProjectDayRecord->projectid = projectid;
//         // curProjectDayRecord->m_ActiveTag_records = new std::list<ActiveTagRecordStruct*>();
//         // curProjectDayRecord->m_CacheDB = new AnalyzeDashBoardDBAccess();
//         // curProjectDayRecord->m_CacheDB->init();
//         // m_ProjectDayRecordList.push_back(curProjectDayRecord);
//
//         //==========================
//         // test
//         // AreaRollCallRecordStruct * areaRollCall = new AreaRollCallRecordStruct();
//         // areaRollCall->projectid = 1;
//         // areaRollCall->areaid = "areaid";
//         // areaRollCall->nodeid = "nodeid";
//         // areaRollCall->issueday = "today"; // foramt: YYYY-MM-DD
//         // areaRollCall->issuetime = "time"; // foramt: HH:mm
//         // areaRollCall->totaltime = 0; // foramt: ss how many seconds stay in this minute at this area
//         // areaRollCall->totaltimeInMS = 0;
//         // curProjectDayRecord->m_CacheDB->write_arearollcall_record(areaRollCall);
//         // delete areaRollCall;
//         //
//         // std::string current_datetime = currentDateTimeForSQL();
//         // std::string one_day_ago = currentDateTimeForSQL(1);
//         //
//         // std::list<AreaActivityTimeStruct*> * area_records = curProjectDayRecord->m_CacheDB->getAreaActivityTime(one_day_ago, current_datetime);
//         //
//         // vilslog::printf("AnalyzeDashBoardService::prepair_ActiveTag_record() area_records->size()[%d]\n", (int)area_records->size());
//         //
//         // for (std::list<AreaActivityTimeStruct*>::iterator i = area_records->begin(); i != area_records->end(); i++)
//         // {
//         //     std::list<std::string> * area_users = curProjectDayRecord->m_CacheDB->getAreaActivityUser((*i)->areaid, one_day_ago, current_datetime);
//         //
//         //     vilslog::printf("AnalyzeDashBoardService::prepair_ActiveTag_record() areaid[%s] area_users->size()[%d]\n",
//         //         (*i)->areaid.c_str(), (int)area_users->size());
//         // }
//         //==========================
//
//
//         TrackDBAccess * trackDb = mDbManager->getTrackDB(projectid);
//         if (trackDb == NULL)
//         {
//             return NULL;
//         }
//
//         std::list<DeviceActivityAliveCountStruct *> * dataList = trackDb->loadactivitys(nodetype, today_date, 30);
//
//         // vilslog::printf("prepair_ActiveTag_record() dataList.size()[%d]\n", (int)dataList->size());
//
//         for(int i=1;i<=30;i++)
//         {
//             std::string currentTime = currentTimeForSQL(i);
//
//             bool bFound = false;
//             for (std::list<DeviceActivityAliveCountStruct*>::iterator data = dataList->begin(); data != dataList->end(); data++)
//             {
//                 if ( currentTime.compare((*data)->issuetime) == 0 )
//                 {
//                     ActiveTagRecordStruct * record = new ActiveTagRecordStruct();
//                     record->count = (*data)->devicecount;
//                     record->issuetime = (*data)->issuetime;
//
//                     curProjectDayRecord->m_ActiveTag_records->push_back(record);
//                     bFound = true;
//                     break;
//                 }
//
//                 // vilslog::printf("prepair_ActiveTag_record() (*data)->devicecount[%d] (*data)->issuetime[%s]\n",
//                 //     (*data)->devicecount, (*data)->issuetime.c_str());
//             }
//             if (!bFound)
//             {
//                 ActiveTagRecordStruct * record = new ActiveTagRecordStruct();
//                 if ( i == 1 )
//                     record->count = -1;
//                 else
//                     record->count = 0;
//                 record->issuetime = currentTime;
//                 curProjectDayRecord->m_ActiveTag_records->push_back(record);
//             }
//         }
//
//         clear_DeviceActivityAliveCountStruct_list(dataList);
//         delete dataList;
//     }
//
//     //
//     // prepair issuetime
//     //
//     std::string previous_min = currentTimeForSQL(1);
//
//     ActiveTagRecordStruct tmp_record;
//     ActiveTagRecordStruct pre_record;
//     pre_record.count = -1;
//     pre_record.issuetime = "";
//     for (std::list<ActiveTagRecordStruct*>::iterator record = curProjectDayRecord->m_ActiveTag_records->begin(); record != curProjectDayRecord->m_ActiveTag_records->end(); record++)
//     {
//         if ( pre_record.count == -1 )
//         {
//             // vilslog::printf("prepair_ActiveTag_record() 1 previous_min[%s] (*record)->issuetime[%s]\n", previous_min.c_str(), (*record)->issuetime.c_str());
//
//             if (previous_min.compare((*record)->issuetime) == 0)
//             {
//                 // already have data
//                 break;
//             }
//             pre_record.count = (*record)->count;
//             pre_record.issuetime = (*record)->issuetime;
//             continue;
//         }
//
//         // backup current data
//         tmp_record.count = (*record)->count;
//         tmp_record.issuetime = (*record)->issuetime;
//
//         // copy previous data to current
//         (*record)->count = pre_record.count;
//         (*record)->issuetime = pre_record.issuetime;
//
//         // put to previous
//         pre_record.count = tmp_record.count;
//         pre_record.issuetime = tmp_record.issuetime;
//     }
//
//     // update first record
//     std::list<ActiveTagRecordStruct*>::iterator record = curProjectDayRecord->m_ActiveTag_records->begin();
//
//     if ((*record) == NULL)
//     {
//         vilslog::printf("prepair_ActiveTag_record() 2 record == NULL\n");
//         return NULL;
//     }
//
//     // vilslog::printf("prepair_ActiveTag_record() 2 previous_min[%s] (*record)->issuetime[%s] (*record)->count[%d]\n",
//     //     previous_min.c_str(), (*record)->issuetime.c_str(), (*record)->count);
//
//     if ( (previous_min.compare((*record)->issuetime) != 0) || ((*record)->count == -1) )
//     {
//         std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);
//
//         int aliveCount = 0;
//         for (std::list<NodeStruct*>::iterator i = tagNodeList->begin(); i != tagNodeList->end(); i++)
//         {
//             if ((*i)->alive > 0)
//             {
//                 aliveCount++;
//             }
//         }
//
//         // vilslog::printf("prepair_ActiveTag_record() previous_min[%s] aliveCount[%d]\n", previous_min.c_str(), aliveCount);
//
//         (*record)->count = aliveCount;
//         (*record)->issuetime = previous_min;
//     }
//
//     return curProjectDayRecord->m_ActiveTag_records;
// }

void AnalyzeDashBoardService::updateActivityUser()
{
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        updateActivityUser(projectid);
    }
}

void AnalyzeDashBoardService::updateActivityUser(int projectid)
{
    int nodetype = NODE_TYPE_TAG;
    std::string today_date = TodayDateType1();
    std::string current_time = currentTimeForSQL();
    // std::string half_hour_ago = currentTimeForSQL(30);

    // vilslog::printf("AnalyzeDashBoardService::updateActivityUser() projectid[%d] today_date[%s] current_time[%s]\n",
    //     projectid, today_date.c_str(), current_time.c_str());

    std::list<DayRecordStruct*> * m_day_records = prepair_day_record(projectid);
    if (m_day_records == NULL)
        return;
    // TrackDBAccess * trackDb = mDbManager->getTrackDB(projectid);
    // if (trackDb == NULL)
    //     return;


    // vilslog::printf("AnalyzeDashBoardService::updateActivityUser() projectid[%d] today_date[%s] current_time[%s] m_day_records->size()[%d]\n",
    //     projectid, today_date.c_str(), current_time.c_str(), (int)m_day_records->size());

    //
    // check different node count
    //
    //==============================
    //std::list<DeviceAliveCountStruct*> * alive_records = trackDb->getDeviceActivityAliveCount(nodetype, today_date, half_hour_ago, current_time);
    //int devicecount = trackDb->getDeviceActivityAliveCount(nodetype, today_date, half_hour_ago, current_time);
    //==============================

    std::list<std::string> NodeIDList;
    std::string aliveString = "";
    for (std::list<DayRecordStruct*>::iterator record = m_day_records->begin(); record != m_day_records->end(); record++)
    {
        if ( record != m_day_records->begin() )
        {
            aliveString.append(",");
        }
        aliveString.append( std::to_string((*record)->count) );

        //
        // check different node count
        //
        for (std::list<std::string>::iterator nodeid = (*record)->NodeIDList->begin(); nodeid != (*record)->NodeIDList->end(); nodeid++)
        {
            bool bFound = false;
            for (std::list<std::string>::iterator uninodeid = NodeIDList.begin(); uninodeid != NodeIDList.end(); uninodeid++)
            {
                if ( (*uninodeid).compare((*nodeid)) == 0 )
                {
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
            {
                NodeIDList.push_back((*nodeid));
            }
        }
    }

    int devicecount = (int)NodeIDList.size();
    NodeIDList.clear();

    // vilslog::printf("AnalyzeDashBoardService::updateActivityUser() aliveString[%s] devicecount()[%d]\n", aliveString.c_str(), devicecount);

    DeviceActivityAliveCountStruct record;
    record.nodetype = nodetype;
    record.devicecount = devicecount;
    record.aliveString = aliveString;
    record.issueday = today_date;
    record.issuetime = current_time;

    WorkItem * writem = new WorkItem("deviceactivityalivecountrecord", projectid, &record);
    mTrackDBqueue.add(writem);
}

std::list<DayRecordStruct*> * AnalyzeDashBoardService::prepair_day_record(int projectid)
{
    ProjectDayRecordStruct * curProjectDayRecord = NULL;
    for (std::list<ProjectDayRecordStruct*>::iterator buffer = m_ProjectDayRecordList.begin(); buffer != m_ProjectDayRecordList.end(); buffer++)
    {
        if( (*buffer)->projectid == projectid )
        {
            curProjectDayRecord = (*buffer);
            break;
        }
    }

    if (curProjectDayRecord == NULL)
    {
        vilslog::printf("AnalyzeDashBoardService::prepair_day_record() projectid[%d] curProjectDayRecord == NULL\n", projectid);

        return NULL;
    }

#ifdef USING_SQLITE_DB_MEMORY_CACHE
#else
    TrackDBAccess * trackDb = mDbManager->getTrackDB(projectid);
    if (trackDb == NULL)
    {
        return NULL;
    }
#endif

    int nodetype = NODE_TYPE_TAG;
    std::string today_date = TodayDateType1();
    if (curProjectDayRecord->m_day_records->size() == 0)
    {
        // curProjectDayRecord = new ProjectDayRecordStruct();
        // curProjectDayRecord->m_day_records = new std::list<DayRecordStruct*>();
        // m_ProjectDayRecordList.push_back(curProjectDayRecord);

        for(int i=2;i<=31;i++)
        {
            std::string currentTime = currentTimeForSQL(i);

#ifdef USING_SQLITE_DB_MEMORY_CACHE
            std::list<std::string> * NodeIDList = curProjectDayRecord->m_CacheDB->getDeviceActivityNodeIdList(nodetype, today_date, currentTime);
#else
            std::list<std::string> * NodeIDList = trackDb->getDeviceActivityNodeIdList(nodetype, today_date, currentTime);
#endif
            DayRecordStruct * record = new DayRecordStruct();
            record->count = (int)NodeIDList->size();
            record->issuetime = currentTime;
            record->NodeIDList = NodeIDList;

            // vilslog::printf("AnalyzeDashBoardService::prepair_day_record() projectid[%d] today_date[%s] currentTime[%s] count[%d]\n",
            //     projectid, today_date.c_str(), currentTime.c_str(), record->count);

            curProjectDayRecord->m_day_records->push_back(record);
        }
    }

    //
    // prepair issuetime
    //
    std::string previous_min = currentTimeForSQL(1);
    //int i_time = 1;

    DayRecordStruct tmp_record;
    DayRecordStruct pre_record;
    pre_record.count = -1;
    pre_record.issuetime = "";
    pre_record.NodeIDList = NULL;
    for (std::list<DayRecordStruct*>::iterator record = curProjectDayRecord->m_day_records->begin(); record != curProjectDayRecord->m_day_records->end(); record++)
    {
        if ( pre_record.count == -1 )
        {
            // vilslog::printf("AnalyzeDashBoardService::prepair_day_record() projectid[%d] previous_min[%s] (*record)->issuetime[%s]\n",
            //     projectid, previous_min.c_str(), (*record)->issuetime.c_str());

            if (previous_min.compare((*record)->issuetime) == 0)
            {
                // already have data
                break;
            }
            pre_record.count = (*record)->count;
            pre_record.issuetime = (*record)->issuetime;
            pre_record.NodeIDList = (*record)->NodeIDList;
            continue;
        }

        // backup current data
        tmp_record.count = (*record)->count;
        tmp_record.issuetime = (*record)->issuetime;
        tmp_record.NodeIDList = (*record)->NodeIDList;

        // copy previous data to current
        (*record)->count = pre_record.count;
        (*record)->issuetime = pre_record.issuetime;
        (*record)->NodeIDList = pre_record.NodeIDList;

        // put to previous
        pre_record.count = tmp_record.count;
        pre_record.issuetime = tmp_record.issuetime;
        pre_record.NodeIDList = tmp_record.NodeIDList;
    }

    if ( pre_record.NodeIDList != NULL )
    {
        pre_record.NodeIDList->clear();
        delete pre_record.NodeIDList;

        // vilslog::printf("AnalyzeDashBoardService::prepair_day_record() previous_min[%s] count[%d] released\n",
        //     pre_record.issuetime.c_str(), pre_record.count);
    }

    // update first record
    std::list<DayRecordStruct*>::iterator record = curProjectDayRecord->m_day_records->begin();

    // vilslog::printf("AnalyzeDashBoardService::prepair_day_record() projectid[%d] previous_min[%s] (*record)->issuetime[%s]\n",
    //     projectid, previous_min.c_str(), (*record)->issuetime.c_str());

    if (previous_min.compare((*record)->issuetime) != 0)
    {
#ifdef USING_SQLITE_DB_MEMORY_CACHE
        std::list<std::string> * NodeIDList = curProjectDayRecord->m_CacheDB->getDeviceActivityNodeIdList(nodetype, today_date, previous_min);
#else
        std::list<std::string> * NodeIDList = trackDb->getDeviceActivityNodeIdList(nodetype, today_date, previous_min);
#endif
        // vilslog::printf("AnalyzeDashBoardService::prepair_day_record() projectid[%d] previous_min[%s] (*record)->issuetime[%s] NodeIDList->size()[%d]\n",
        //     projectid, previous_min.c_str(), (*record)->issuetime.c_str(), (int)NodeIDList->size());

        (*record)->count = (int)NodeIDList->size();
        (*record)->issuetime = previous_min;
        (*record)->NodeIDList = NodeIDList;

    }

    return curProjectDayRecord->m_day_records;
}

void AnalyzeDashBoardService::updateAreaActivityUserCount()
{
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        updateAreaActivityUserCount(projectid);
    }
}

void AnalyzeDashBoardService::updateAreaActivityUserCount(int projectid)
{
    std::string today_date = TodayDateType1();
    std::string current_time = currentTimeForSQL();
    std::string current_datetime = currentDateTimeForSQL();
    std::string one_day_ago = currentDateTimeForSQL(1);

    // vilslog::printf("AnalyzeDashBoardService::updateAreaActivityUserCount() today_date[%s] current_time[%s] current_datetime[%s] one_day_ago[%s]\n",
    //     today_date.c_str(), current_time.c_str(), current_datetime.c_str(), one_day_ago.c_str());

    //=======================================
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
        return;

    ProjectDayRecordStruct * curProjectDayRecord = NULL;
    for (std::list<ProjectDayRecordStruct*>::iterator buffer = m_ProjectDayRecordList.begin(); buffer != m_ProjectDayRecordList.end(); buffer++)
    {
        if( (*buffer)->projectid == projectid )
        {
            curProjectDayRecord = (*buffer);
            break;
        }
    }

    if (curProjectDayRecord == NULL)
    {
        vilslog::printf("AnalyzeDashBoardService::updateActivityOperationMode() projectid[%d] curProjectDayRecord == NULL\n", projectid);

        return;
    }

#ifdef USING_SQLITE_DB_MEMORY_CACHE

    std::list<AreaActivityTimeStruct*> * area_records = curProjectDayRecord->m_CacheDB->getAreaActivityTime(one_day_ago, current_datetime);
    for (std::list<AreaActivityTimeStruct*>::iterator i = area_records->begin(); i != area_records->end(); i++)
    {
        std::list<std::string> * area_users = curProjectDayRecord->m_CacheDB->getAreaActivityUser((*i)->areaid, one_day_ago, current_datetime);

        // if ((*i)->areaid.compare("area1718") == 0)
        // {
        //     vilslog::printf("AnalyzeDashBoardService::updateAreaActivityUserCount() (*i)->areaid[%s] area_users->size()[%d] totaltime[%d]\n",
        //         (*i)->areaid.c_str(), (int)area_users->size(), (*i)->totaltime);
        // }
        AreaActivityUserCountStruct record;
        record.totaltime = (*i)->totaltime;
        record.count = (int)area_users->size();
        record.areaid = (*i)->areaid;
        record.areaname = baseDb->fetchAreaName((*i)->areaid);
        record.issueday = today_date;
        record.issuetime = current_time;
        record.start_datetime = one_day_ago;
        record.end_datetime = current_datetime;

        WorkItem * writem = new WorkItem("areaactivityusercountrecord", projectid, &record);
        mTrackDBqueue.add(writem);

        area_users->clear();
        delete area_users;
    }
#else
    //=======================================
    TrackDBAccess * trackDb = mDbManager->getTrackDB(projectid);
    if (trackDb == NULL)
        return;

    // vilslog::printf("AnalyzeDashBoardService::updateAreaActivityUserCount() today_date[%s] current_datetime[%s] one_day_ago[%s]\n",
    //     today_date.c_str(), current_datetime.c_str(), one_day_ago.c_str());

    std::list<AreaActivityTimeStruct*> * area_records = trackDb->getAreaActivityTime(one_day_ago, current_datetime);

    //vilslog::printf("AnalyzeDashBoardService::updateAreaActivityUserCount() area_records->size()[%d]\n", (int)area_records->size());

    for (std::list<AreaActivityTimeStruct*>::iterator i = area_records->begin(); i != area_records->end(); i++)
    {
        std::list<std::string> * area_users = trackDb->getAreaActivityUser((*i)->areaid, one_day_ago, current_datetime);

        // vilslog::printf("AnalyzeDashBoardService::updateAreaActivityUserCount() (*i)->areaid[%s] area_users->size()[%d]\n",
        //     (*i)->areaid.c_str(), (int)area_users->size());


        AreaActivityUserCountStruct record;
        record.totaltime = (*i)->totaltime;
        record.count = (int)area_users->size();
        record.areaid = (*i)->areaid;
        record.areaname = baseDb->fetchAreaName((*i)->areaid);
        record.issueday = today_date;
        record.issuetime = current_time;
        record.start_datetime = one_day_ago;
        record.end_datetime = current_datetime;

        WorkItem * writem = new WorkItem("areaactivityusercountrecord", projectid, &record);
        mTrackDBqueue.add(writem);

        area_users->clear();
        delete area_users;
    }
    //=======================================
#endif

    clear_AreaActivityTimeStruct_list(area_records);
    delete area_records;
}

void AnalyzeDashBoardService::updateActivityOperationMode()
{
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        updateActivityOperationMode(projectid);
    }
}

void AnalyzeDashBoardService::updateActivityOperationMode(int projectid)
{
    std::string today_date = TodayDateType1();
    std::string current_time = currentTimeForSQL();
    std::string current_datetime = currentDateTimeForSQL();
    std::string one_day_ago = currentDateTimeForSQL(1);


    //=======================================
    ProjectDayRecordStruct * curProjectDayRecord = NULL;
    for (std::list<ProjectDayRecordStruct*>::iterator buffer = m_ProjectDayRecordList.begin(); buffer != m_ProjectDayRecordList.end(); buffer++)
    {
        if( (*buffer)->projectid == projectid )
        {
            curProjectDayRecord = (*buffer);
            break;
        }
    }

    if (curProjectDayRecord == NULL)
    {
        vilslog::printf("AnalyzeDashBoardService::updateActivityOperationMode() projectid[%d] curProjectDayRecord == NULL\n", projectid);

        return;
    }

#ifdef USING_SQLITE_DB_MEMORY_CACHE
    std::list<OperationModeActivityStruct *> * operationModeList = curProjectDayRecord->m_CacheDB->getOperationModeActivityList(one_day_ago, current_datetime);
#else
    //=======================================
    TrackDBAccess * trackDb = mDbManager->getTrackDB(projectid);
    if (trackDb == NULL)
        return;

    std::list<OperationModeActivityStruct *> * operationModeList = trackDb->getOperationModeActivityList(one_day_ago, current_datetime);
    //=======================================
#endif
    for (std::list<OperationModeActivityStruct *>::iterator ptoperationMode = operationModeList->begin(); ptoperationMode != operationModeList->end(); ptoperationMode++)
    {
        if ( (*ptoperationMode)->mode == 1 || (*ptoperationMode)->mode == 2 || (*ptoperationMode)->mode == 3)
        {
            (*ptoperationMode)->issueday = today_date;
            (*ptoperationMode)->issuetime = current_time;
            WorkItem * mode1item = new WorkItem("OperationModeActivityRecord", projectid, (*ptoperationMode));
            mTrackDBqueue.add(mode1item);

            // vilslog::printf("AnalyzeDashBoardService::updateActivityOperationMode() mode[%d] count[%d] totaltime[%d] issuetime[%s]\n",
            //     (*ptoperationMode)->mode, (*ptoperationMode)->count, (*ptoperationMode)->totaltime, (*ptoperationMode)->issuetime.c_str());
        }
    }

    clear_OperationModeActivityStruct_list(operationModeList);
    delete operationModeList;
}

void AnalyzeDashBoardService::updateNodeMoveOffset()
{
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        updateNodeMoveOffset(projectid);
    }
}

void AnalyzeDashBoardService::updateNodeMoveOffset(int projectid)
{
    std::string today_date = TodayDateType1();
    std::string current_time = currentTimeForSQL();
    std::string current_datetime = currentDateTimeForSQL();
    std::string one_day_ago = currentDateTimeForSQL(1);


    //=======================================
    ProjectDayRecordStruct * curProjectDayRecord = NULL;
    for (std::list<ProjectDayRecordStruct*>::iterator buffer = m_ProjectDayRecordList.begin(); buffer != m_ProjectDayRecordList.end(); buffer++)
    {
        if( (*buffer)->projectid == projectid )
        {
            curProjectDayRecord = (*buffer);
            break;
        }
    }

    if (curProjectDayRecord == NULL)
    {
        vilslog::printf("AnalyzeDashBoardService::updateNodeMoveOffset() projectid[%d] curProjectDayRecord == NULL\n", projectid);

        return;
    }

#ifdef USING_SQLITE_DB_MEMORY_CACHE
    std::list<NodeMoveOffsetInfoStruct *> * infoList = curProjectDayRecord->m_CacheDB->getNodeMoveOffset(one_day_ago, current_datetime);
#else
    //=======================================
    TrackDBAccess * trackDb = mDbManager->getTrackDB(projectid);
    if (trackDb == NULL)
        return;

    std::list<NodeMoveOffsetInfoStruct *> * infoList = trackDb->getNodeMoveOffset(one_day_ago, current_datetime);
    //=======================================
#endif

    for (std::list<NodeMoveOffsetInfoStruct*>::iterator info = infoList->begin(); info != infoList->end(); info++)
    {
        (*info)->nodetype = NODE_TYPE_TAG;
        (*info)->issueday = today_date;
        (*info)->issuetime = current_time;
        (*info)->start_datetime = one_day_ago;
        (*info)->end_datetime = current_datetime;
        WorkItem * item = new WorkItem("NodeMoveOffsetInfoRecord", projectid, (*info));
        mTrackDBqueue.add(item);
    }

    clear_NodeMoveOffsetInfoStruct_list(infoList);
    delete infoList;
}

#ifdef USING_SQLITE_DB_MEMORY_CACHE
void AnalyzeDashBoardService::dump_cache_data()
{
    ProjectDayRecordStruct * curProjectDayRecord = NULL;
    for (std::list<ProjectDayRecordStruct*>::iterator buffer = m_ProjectDayRecordList.begin(); buffer != m_ProjectDayRecordList.end(); buffer++)
    {
        curProjectDayRecord = (*buffer);
        curProjectDayRecord->m_CacheDB->loadOrSaveDb(curProjectDayRecord->projectid, 1);

        // int saveStatus = curProjectDayRecord->m_CacheDB->loadOrSaveDb(curProjectDayRecord->projectid, 1);
        // vilslog::printf("AnalyzeDashBoardService::dump_cache_data() projectid[%d] saveStatus[%d]\n", curProjectDayRecord->projectid, saveStatus);
    }
}
#else
void AnalyzeDashBoardService::dump_cache_data()
{
}
#endif

#ifdef USING_SQLITE_DB_MEMORY_CACHE
void AnalyzeDashBoardService::clear_old_data()
{
    ProjectDayRecordStruct * curProjectDayRecord = NULL;
    for (std::list<ProjectDayRecordStruct*>::iterator buffer = m_ProjectDayRecordList.begin(); buffer != m_ProjectDayRecordList.end(); buffer++)
    {
        curProjectDayRecord = (*buffer);
        std::string olddatetime = currentDateTimeForSQL(1);
        curProjectDayRecord->m_CacheDB->clear_old_data(olddatetime.c_str());
    }
}
#endif

void AnalyzeDashBoardService::updateDeviceAliveCount(int projectid, std::list<NodeStruct*> * deviceList, std::string today, std::string time)
{
    for (std::list<NodeStruct*>::iterator i = deviceList->begin(); i != deviceList->end(); i++)
    {
// #ifdef ANALYZE_TRACK_PRINT_MSG
//         vilslog::printf("AnalyzeDashBoardService::updateDeviceAliveCount() nodetype[%d] nodeid[%s] alive[%d]\n",
//             (*i)->nodetype, (*i)->nodeid, (*i)->alive);
// #endif

        if ((*i)->alive > 0)
        {
            DeviceActivityRecordStruct record;
            record.nodetype = (*i)->nodetype;
            record.nodeid = (*i)->nodeid;
            record.alive = (*i)->alive;
            record.issueday = today;
            record.issuetime = time;

            WorkItem * writem = new WorkItem("deviceactivityrecord", projectid, &record);
            mTrackDBqueue.add(writem);

            //mTrackDB->write_deviceactivity_record(&record);
        }
    }
}

void AnalyzeDashBoardService::updateGatewayAliveCount(int projectid, std::list<LoRaGatewayInfoStruct*> * deviceList, std::string today, std::string time)
{
    for (std::list<LoRaGatewayInfoStruct*>::iterator i = deviceList->begin(); i != deviceList->end(); i++)
    {
// #ifdef ANALYZE_TRACK_PRINT_MSG
//         vilslog::printf("AnalyzeDashBoardService::updateGatewayAliveCount() nodetype[%d] gwid[%s] alive[%d]\n",
//             NODE_TYPE_GATEWAY, (*i)->gwid.c_str(), (*i)->alive);
// #endif

        if ( ((*i)->alive > 0) || ((*i)->MQTTalive > 0) || ((*i)->pingCount > 0) )
        {
            DeviceActivityRecordStruct record;
            record.nodetype = NODE_TYPE_GATEWAY;
            record.nodeid = (*i)->gwid;
            record.alive = 1;
            record.issueday = today;
            record.issuetime = time;

            WorkItem * writem = new WorkItem("deviceactivityrecord", projectid, &record);
            mTrackDBqueue.add(writem);

            //mTrackDB->write_deviceactivity_record(&record);
        }
    }
}


int AnalyzeDashBoardService::CheckOperationMode(int diffMS)
{
    int count = 0;
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        count += CheckOperationMode(projectid, diffMS);
    }
    return count;
}

int AnalyzeDashBoardService::CheckOperationMode(int projectid, int diffMS)
{
    int count = 0;

    std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);

    for (std::list<NodeStruct*>::iterator node = tagNodeList->begin(); node != tagNodeList->end(); node++)
    {
        if ( (*node)->alive > 0 && (*node)->Opertaion_Mode != 0)
        {
            save_operationmode(projectid, (*node), diffMS);
        }

        count++;
    }

    return count;
}

bool AnalyzeDashBoardService::save_operationmode(int projectid, NodeStruct * node, int diffMS)
{
    std::string today = TodayDateType1();
    std::string time = currentTimeForSQL();

    OperationModeRecordStruct * curOperationMode = NULL;
    for (std::list<OperationModeRecordStruct*>::iterator operationmode = mOperationModeRecordList.begin(); operationmode != mOperationModeRecordList.end(); operationmode++)
    {
        if ( (projectid == (*operationmode)->projectid) &&  (*operationmode)->nodeid.compare(node->nodeid) == 0)
        {
            curOperationMode = (*operationmode);
            break;
        }
    }
    if (curOperationMode == NULL)
    {
        OperationModeRecordStruct * operationMode = new OperationModeRecordStruct();
        operationMode->projectid = node->projectid;
        operationMode->mode = node->Opertaion_Mode;
        operationMode->nodeid = node->nodeid;
        operationMode->issueday = today; // foramt: YYYY-MM-DD
        operationMode->issuetime = time; // foramt: HH:mm
        operationMode->totaltime = 0; // foramt: ss how many seconds stay in this minute at this area
        operationMode->totaltimeInMS = 0;

        mOperationModeRecordList.push_back(operationMode);

        curOperationMode = operationMode;
    }

    curOperationMode->totaltimeInMS += diffMS;
    curOperationMode->totaltime = (int)(curOperationMode->totaltimeInMS / 1000);

    if ( time.compare(curOperationMode->issuetime) != 0 )
    {
        WorkItem * writem = new WorkItem("operationmoderecord", projectid, curOperationMode);
        mTrackDBqueue.add(writem);

        // vilslog::printf("AnalyzeDashBoardService::save_operationmode() nodeid[%s] mode[%d] issueday[%s] issuetime[%s] totaltime[%d] totaltimeInMS[%d]\n",
        //                node->nodeid, curOperationMode->mode, curOperationMode->issueday.c_str(), curOperationMode->issuetime.c_str(), curOperationMode->totaltime, curOperationMode->totaltimeInMS);

        curOperationMode->issueday = today;
        curOperationMode->issuetime = time;
        curOperationMode->totaltimeInMS = 0;
        curOperationMode->totaltime = 0;
    }

    return true;
}

#ifdef USING_SQLITE_DB_MEMORY_CACHE
int AnalyzeDashBoardService::do_deviceactivityrecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    for (std::list<ProjectDayRecordStruct*>::iterator buffer = m_ProjectDayRecordList.begin(); buffer != m_ProjectDayRecordList.end(); buffer++)
    {
        if( (*buffer)->projectid == projectid )
        {
            //(*buffer)->m_CacheDB->write_deviceactivity_record(&item->mDeviceActivityRecord);
            (*buffer)->m_CacheDB->write_deviceactivity_record_full(&item->mDeviceActivityRecord);
            break;
        }
    }

    return 0;
}

int AnalyzeDashBoardService::do_arearollcallrecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    for (std::list<ProjectDayRecordStruct*>::iterator buffer = m_ProjectDayRecordList.begin(); buffer != m_ProjectDayRecordList.end(); buffer++)
    {
        if( (*buffer)->projectid == projectid )
        {
            (*buffer)->m_CacheDB->write_arearollcall_record(&item->mAreaRollCallRecord);
            break;
        }
    }

    // vilslog::printf("AnalyzeDashBoardService::do_arearollcallrecord() projectid[%d] nodeid[%s] areaid[%s] issueday[%s] issuetime[%s] totaltime[%d] totaltimeInMS[%d]\n",
    //                projectid, item->mAreaRollCallRecord.nodeid.c_str(), item->mAreaRollCallRecord.areaid.c_str(),
    //                item->mAreaRollCallRecord.issueday.c_str(), item->mAreaRollCallRecord.issuetime.c_str(),
    //                item->mAreaRollCallRecord.totaltime, item->mAreaRollCallRecord.totaltimeInMS);


    return 0;
}

int AnalyzeDashBoardService::do_TagMoveOffsetInfoRecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    for (std::list<ProjectDayRecordStruct*>::iterator buffer = m_ProjectDayRecordList.begin(); buffer != m_ProjectDayRecordList.end(); buffer++)
    {
        if( (*buffer)->projectid == projectid )
        {
            (*buffer)->m_CacheDB->write_TagMoveOffsetInfo_record(NODE_TYPE_TAG, item->mTagMoveOffsetInfoRecord.TagId, item->mTagMoveOffsetInfoRecord.moveoffset);
            break;
        }
    }

    return 0;
}

int AnalyzeDashBoardService::do_operationmoderecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    for (std::list<ProjectDayRecordStruct*>::iterator buffer = m_ProjectDayRecordList.begin(); buffer != m_ProjectDayRecordList.end(); buffer++)
    {
        if( (*buffer)->projectid == projectid )
        {
            (*buffer)->m_CacheDB->write_operationmode_record(&item->mOperationModeRecord);
            break;
        }
    }

    return 0;
}
#endif
