/*
 *  KIAService.h
 *
 *  Created on: 2021/04/27
 *      Author: ikki
 */
#ifndef __KIAService_h__
#define __KIAService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../common.h"
#include "../config.h"
#include "../network/WorkItem.h"
#include "../network/NotificationItem.h"
#include "../mysql/DBManager.h"

using namespace std;

class KIAService : public Thread
{
public:
    KIAService(wqueue<WorkItem*> & MySQLJobQueue, wqueue<NotificationItem*> & NotifiactionQueue, DBManager * dbManager);
    ~KIAService();
    bool status();
    std::string statusString();

    void * run();

private:
    bool mInitial;
    std::string mStatus;

    wqueue<WorkItem*> & mMySQLJobQueue;
    wqueue<NotificationItem*> & mNotifiactionQueue;
    DBManager * mDbManager;

    std::list<WCInfoStruct*> * mWCInfoList;

    void runloop();
    void updateWCInfo();
    void sendNotify();

    void updateWCInfoArr(std::string content);
    void putToWCInfoList(WCInfoStruct * newInfo);
    void putWCInfoToDB(WCInfoStruct * newInfo);
};

#endif
