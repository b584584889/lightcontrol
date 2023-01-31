/*
 *  TrackDBService.h
 *
 *  Created on: 2019/07/30
 *      Author: ikki
 */
#ifndef __TrackDBService_h__
#define __TrackDBService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Thread.h"
#include "../network/wqueue.h"
#include "../network/WorkItem.h"
#include "../mysql/DBManager.h"

#include "../config.h"

using namespace std;

class TrackDBService : public Thread
{
public:
    TrackDBService(wqueue<WorkItem*> & queue, wqueue<WorkItem*> & analyzedashboardqueue, DBManager * dbManager);
    ~TrackDBService();
    bool status();
    std::string statusString();

    void * run();

private:

    std::string mStatus;
    bool mInitial;

    wqueue<WorkItem*> & m_queue;
    wqueue<WorkItem*> & mAnalyzeDashBoardqueue;
    DBManager * mDbManager;
    // TrackDBAccess * mTrackDB;

    void runloop();
    void clear_old_data();
    int do_commit_db();

    int do_analyzetrackrecord(WorkItem * item);
    int do_daytrackrecord(WorkItem * item);
    int do_deviceactivityrecord(WorkItem * item);
    int do_arearollcallrecord(WorkItem * item);
    int do_operationmoderecord(WorkItem * item);
    int do_deviceactivityalivecountrecord(WorkItem * item);
    int do_areaactivityusercountrecord(WorkItem * item);
    int do_OperationModeActivityRecord(WorkItem * item);
    int do_TagMoveOffsetInfoRecord(WorkItem * item);
    int do_NodeMoveOffsetInfoRecord(WorkItem * item);
};

#endif
