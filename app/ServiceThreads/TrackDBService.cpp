/*
 *  TrackDBService.cpp
 *
 *  Created on: 2019/07/30
 *      Author: ikki
 */

#include <string.h>
#include "TrackDBService.h"
#include "../common.h"

using namespace std;

TrackDBService::TrackDBService(wqueue<WorkItem*> & queue, wqueue<WorkItem*> & analyzedashboardqueue, DBManager * dbManager)
    : m_queue(queue), mAnalyzeDashBoardqueue(analyzedashboardqueue), mDbManager(dbManager)
{
}

TrackDBService::~TrackDBService()
{
}

bool TrackDBService::status()
{
    return mInitial;
}

std::string TrackDBService::statusString()
{
    return mStatus;
}

void * TrackDBService::run()
{
    while(1)
    {
        mInitial = false;
        mStatus = "TrackDBService() Initial failed!!";
        try
        {
            runloop();
            sleep(3);
            vilslog::printf ("TrackDBService() Initial failed!\n");
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("TrackDBService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "TrackDBService::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void TrackDBService::runloop()
{
    // Remove 1 item at a time and process it. Blocks if no items are
    char type[64];
    long long start1 = getSystemTime();
    long long ProcessTime = 0;
    long long updateTime = 0;
    long long checkTime = 0;
    int update_count = 0;

    mInitial = true;
    mStatus = "TrackDBService() Initial successfully!";
    vilslog::printf ("TrackDBService() Initial successfully!\n");

    // available to process.
    while (1)
    {
        WorkItem * item = m_queue.removetimed(1000);

        long long end1 = getSystemTime();
        long long difft = end1 - start1;
        ProcessTime += difft;
        updateTime += difft;
        checkTime += difft;

        if (checkTime >= 30000)
        {
            std::string time = currentTimeForSQL();

            if ( time.compare("03:00") == 0 )
            {
                clear_old_data();
            }

            checkTime = 0;
        }

        if ((updateTime >= 2000) || (update_count > 100))
        {
            // long long doStart = getSystemTime();

            do_commit_db();

            // long long doUsing = getSystemTime() - doStart;
            // vilslog::printf ("TrackDBService::runloop()() do_commit_db doUsing[%d]\n", (int)doUsing);

            update_count = 0;
            updateTime = 0;
        }

        if ( item == NULL )
        {
            start1 = end1;
            continue;
        }

        update_count++;

        sprintf(type, "%s", item->getType());

        if ( strcmp(type, "analyzetrackrecord") == 0)
            do_analyzetrackrecord(item);
        else
        if ( strcmp(type, "daytrackrecord") == 0)
            do_daytrackrecord(item);
        else
        if ( strcmp(type, "deviceactivityrecord") == 0)
            do_deviceactivityrecord(item);
        else
        if ( strcmp(type, "arearollcallrecord") == 0)
            do_arearollcallrecord(item);
        else
        if ( strcmp(type, "operationmoderecord") == 0)
            do_operationmoderecord(item);
        else
        if ( strcmp(type, "deviceactivityalivecountrecord") == 0)
            do_deviceactivityalivecountrecord(item);
        else
        if ( strcmp(type, "areaactivityusercountrecord") == 0)
            do_areaactivityusercountrecord(item);
        else
        if ( strcmp(type, "OperationModeActivityRecord") == 0)
            do_OperationModeActivityRecord(item);
        else
        if ( strcmp(type, "TagMoveOffsetInfoRecord") == 0)
            do_TagMoveOffsetInfoRecord(item);
        else
        if ( strcmp(type, "NodeMoveOffsetInfoRecord") == 0)
            do_NodeMoveOffsetInfoRecord(item);

        start1 = end1;
        delete item;
    }

}

void TrackDBService::clear_old_data()
{
    std::string outdatetime = currentDateTimeForSQL(TRACKDB_CLEAR_DATA_DAYS);
    std::string outdate = TodayDateType1(TRACKDB_CLEAR_DATA_DAYS);

    std::list<int> * prjidList = mDbManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);

        TrackDBAccess * trackDb = mDbManager->getTrackDB(projectid);
        if (trackDb == NULL)
            continue;

        trackDb->clear_old_data("AnalyzeTrackRecord", "datetime", outdatetime.c_str());
        trackDb->clear_old_data("AreaActivityUserCount", "datetime", outdatetime.c_str());
        trackDb->clear_old_data("DayTrack", "datetime", outdatetime.c_str());

        trackDb->clear_old_data("DeviceActivity", "issueday", outdate.c_str());
        trackDb->clear_old_data("DeviceActivityAliveCount", "datetime", outdatetime.c_str());

        trackDb->clear_old_data("NodeMoveOffsetInfo", "datetime", outdatetime.c_str());
        trackDb->clear_old_data("NodeMoveOffsetRecord", "datetime", outdatetime.c_str());
        trackDb->clear_old_data("OperationMode", "datetime", outdatetime.c_str());
        trackDb->clear_old_data("OperationModeActivityCount", "datetime", outdatetime.c_str());
        trackDb->clear_old_data("AreaRollCall", "datetime", outdatetime.c_str());
    }

    prjidList->clear();
    delete prjidList;
}

int TrackDBService::do_commit_db()
{
    std::list<int> * prjidList = mDbManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);

        TrackDBAccess * trackDb = mDbManager->getTrackDB(projectid);
        if (trackDb == NULL)
            continue;

        trackDb->Transaction_commit();
    }
    return 0;
}

int TrackDBService::do_analyzetrackrecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    TrackDBAccess * trackDb = mDbManager->getTrackDB(projectid);
    if (trackDb != NULL)
        trackDb->write_record(&item->mAnalyzeTrackRecord);
    return 0;
}

int TrackDBService::do_daytrackrecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    TrackDBAccess * trackDb = mDbManager->getTrackDB(projectid);
    if (trackDb != NULL)
        trackDb->write_daytrack_record(&item->mDayTrackRecord);
    return 0;
}

int TrackDBService::do_deviceactivityrecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    TrackDBAccess * trackDb = mDbManager->getTrackDB(projectid);
    if (trackDb != NULL)
        trackDb->write_deviceactivity_record(&item->mDeviceActivityRecord);

    WorkItem * writem = new WorkItem("deviceactivityrecord", projectid, &item->mDeviceActivityRecord);
    mAnalyzeDashBoardqueue.add(writem);

    return 0;
}

int TrackDBService::do_arearollcallrecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    TrackDBAccess * trackDb = mDbManager->getTrackDB(projectid);
    if (trackDb != NULL)
        trackDb->write_arearollcall_record(&item->mAreaRollCallRecord);

    WorkItem * writem = new WorkItem("arearollcallrecord", projectid, &item->mAreaRollCallRecord);
    mAnalyzeDashBoardqueue.add(writem);

    return 0;
}

int TrackDBService::do_operationmoderecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    TrackDBAccess * trackDb = mDbManager->getTrackDB(projectid);
    if (trackDb != NULL)
        trackDb->write_operationmode_record(&item->mOperationModeRecord);

    WorkItem * writem = new WorkItem("operationmoderecord", projectid, &item->mOperationModeRecord);
    mAnalyzeDashBoardqueue.add(writem);

    return 0;
}

int TrackDBService::do_deviceactivityalivecountrecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    TrackDBAccess * trackDb = mDbManager->getTrackDB(projectid);
    if (trackDb != NULL)
        trackDb->write_DeviceActivityAliveCount_record(&item->mDeviceActivityAliveRecord);
    return 0;
}

int TrackDBService::do_areaactivityusercountrecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    TrackDBAccess * trackDb = mDbManager->getTrackDB(projectid);
    if (trackDb != NULL)
        trackDb->write_AreaActivityUserCount_record(&item->mAreaActivityUserCountRecord);
    return 0;
}

int TrackDBService::do_OperationModeActivityRecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    TrackDBAccess * trackDb = mDbManager->getTrackDB(projectid);
    if (trackDb != NULL)
        trackDb->write_OperationModeActivityCount_record(&item->mOperationModeActivityRecord);
    return 0;
}

int TrackDBService::do_TagMoveOffsetInfoRecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    TrackDBAccess * trackDb = mDbManager->getTrackDB(projectid);
    if (trackDb != NULL)
        trackDb->write_TagMoveOffsetInfo_record(NODE_TYPE_TAG, item->mTagMoveOffsetInfoRecord.TagId, item->mTagMoveOffsetInfoRecord.moveoffset);

    WorkItem * writem = new WorkItem("TagMoveOffsetInfoRecord", projectid, &item->mTagMoveOffsetInfoRecord);
    mAnalyzeDashBoardqueue.add(writem);

    return 0;
}

int TrackDBService::do_NodeMoveOffsetInfoRecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    TrackDBAccess * trackDb = mDbManager->getTrackDB(projectid);
    if (trackDb != NULL)
        trackDb->write_NodeMoveOffsetInfo_record(&item->mNodeMoveOffsetInfoRecord);
    return 0;
}
