/*
 *  SyncERPService.h
 *
 *  Created on: 2019/08/15
 *      Author: ikki
 */
#ifndef __SyncERPService_h__
#define __SyncERPService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../common.h"
#include "../config.h"
#include "../network/DataItem.h"
#include "../network/WorkItem.h"
#include "../mysql/DBManager.h"

using namespace std;

class SyncERPService : public Thread
{
public:
    SyncERPService(wqueue<DataItem*> & mainJobQueue, wqueue<DataItem*> & ERPQueue, wqueue<WorkItem*> & MySQLJobQueue, DBManager * dbManager);
    ~SyncERPService();
    bool status();
    std::string statusString();

    void * run();
    void updateERPServer(int projectid, const char * server);

private:
    bool mInitial;
    std::string mStatus;
    //std::string mERPServer;

    wqueue<DataItem*> & mMainJobQueue;
    wqueue<DataItem*> & mSyncERPJobQueue;
    wqueue<WorkItem*> & mMySQLJobQueue;
    DBManager * mDbManager;

    long long mPreStartSyncTime;
    std::list<std::string> mResidentList1;
    std::list<std::string> mResidentList2;
    std::list<std::string> mResidentList3;
    std::list<std::string> mEmployeeList1;
    std::list<std::string> mEmployeeList2;
    std::list<std::string> mEmployeeList3;

    void runloop();
    int processDataItem(DataItem * dataItem);
    bool SyncAllProject();

    bool startSyncWithRetry(int projectid, int retry_Count);
    bool startSync(int projectid, int type, int listindex);

    std::list<HttpHeader*> * parse_http_header(const char * response, size_t n);
    int get_line(const char * databuffer, int data_len, int start_idx, char * linebuffer, int buffer_len);
    void errExit(const char * reason);
    void errPrint(const char *reason);
    bool update_data(int projectid, int type, Json::Value JSONresult, int listindex);
    void update_resident(int projectid, Json::Value JSONresult, int listindex);
    void update_employee(int projectid, Json::Value JSONresult, int listindex);
    std::string getJSONString(Json::Value JSONresult, const char * key);
    int getJSONInt(Json::Value JSONresult, const char * key);

    void test_parser();
    void test_md5();
};

#endif
