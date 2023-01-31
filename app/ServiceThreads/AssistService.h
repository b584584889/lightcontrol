/*
 *  AssistService.h
 *
 *  Created on: 2020/09/30
 *      Author: ikki
 */
#ifndef __AssistService_h__
#define __AssistService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../common.h"
#include "../config.h"
#include "../network/DataItem.h"
#include "../network/WorkItem.h"
#include "../network/NotificationItem.h"
#include "../mysql/DBManager.h"

using namespace std;

typedef struct
{
    int projectid;
    int sendNotifyTime; // in minute
    std::list<AssistStruct*> * mAssistList;
    std::list<AssistEventStruct*> * mAssistEventList;
} ProjectAssistInfoStruct;

class AssistService : public Thread
{
public:
    AssistService(wqueue<NotificationItem*> & NotifiactionQueue,wqueue<DataItem*> & AssistJobQueue, wqueue<WorkItem*> & MySQLJobQueue, DBManager * dbManager);
    ~AssistService();
    bool status();
    std::string statusString();

    void * run();

private:
    bool mInitial;
    std::string mStatus;

    wqueue<NotificationItem*> & mNotifiactionQueue;
    wqueue<DataItem*> & mAssistJobQueue;
    wqueue<WorkItem*> & mMySQLJobQueue;

    DBManager * mDbManager;

    bool mAssistRunning;
    std::vector<ProjectAssistInfoStruct *> mProjectAssistInfoVector;

    void runloop();
    void initProjectAssistInfo();

    int processDataItem(DataItem * dataItem);

    void CheckProjectAssistEvent();
    bool CheckTodayNeedAssist(AssistEventStruct * event, std::string todayDate, int todayWeekday);
    int CheckAssistEvent(ProjectAssistInfoStruct * prjAssistInfo, AssistEventStruct * event, std::string todayDate);
    int check_assist_event(ProjectAssistInfoStruct * prjAssistInfo, AssistEventStruct * event, std::string todayDate, std::string eventtime, bool createEvent);
    int check_assist_result(ProjectAssistInfoStruct * prjAssistInfo, AssistEventStruct * event, std::string todayDate, std::string eventtime, AssistEventRecordStruct * assisteventrecord);
    int create_assist_event(ProjectAssistInfoStruct * prjAssistInfo, AssistEventStruct * event, std::string todayDate, std::string eventtime, AssistEventRecordStruct * assisteventrecord);

    void reloadAssistList();
    void clear_ProjectAssistInfoStruct_vector(std::vector<ProjectAssistInfoStruct*> * plist);
    void clear_AssistInfoStruct(ProjectAssistInfoStruct * info);
    void clear_AssistEventStruct_list(std::list<AssistEventStruct*> * plist);
    std::list<std::string> * getSubgGroupManager(int projectid, UserStruct * user);
};

#endif
