/*
 * InspectionInfoDBAccess.cpp
 *
 *  Created on: 2020/03/09
 *      Author: ikki
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "InspectionInfoDBAccess.h"
#include "../common.h"

using namespace std;


//
// InspectionInfoDBAccess
//
InspectionInfoDBAccess::InspectionInfoDBAccess()
{
    sqlite3_initialize();

    sprintf(mCacheDBName, cachedbname);
    vilslog::printf("InspectionInfoDBAccess::InspectionInfoDBAccess() mCacheDBName[%s]\n", mCacheDBName);

    int flag = (SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
//    printf("InspectionInfoDBAccess::InspectionInfoDBAccess() flag[%d]\n", flag);
    int rc = sqlite3_open_v2(mCacheDBName, &mSqlitedb, flag, NULL);
    if( rc != SQLITE_OK )
    {
        printf("InspectionInfoDBAccess::InspectionInfoDBAccess() Can't open database[%s] errmsg[%s]\n", mCacheDBName, sqlite3_errmsg(mSqlitedb));
    }
    else
    {
        mSqlitedb = NULL;
    }
}

InspectionInfoDBAccess::~InspectionInfoDBAccess()
{
    sqlite3_close(mSqlitedb);
    sqlite3_shutdown();
}

int InspectionInfoDBAccess::init()
{
    int flag = (SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
    if (!checkDBExist(flag))
    {
        vilslog::printf("InspectionInfoDBAccess::init() database not exist!\n");
        create_table();
    }

    //cout << "SQLiteDBAccess::init() database exist!" << endl;

    sqlite3_busy_timeout(mSqlitedb, 5);// wait 5 ms at busy mode

    static int flagtest = 0;
    if (flagtest == 0)
    {
        flagtest = 1;

        if ( sqlite3_threadsafe() == 1 || sqlite3_threadsafe() == 2)
        {
            vilslog::printf("InspectionInfoDBAccess::init() sqlite3 is thread safe.\n");
        }
        else
        {
            vilslog::printf("InspectionInfoDBAccess::init() sqlite3 is *** NOT *** thread safe!!!\n");
        }
    }

    check_valid_table();

    return 0;
}

int InspectionInfoDBAccess::check_valid_table()
{
    check_SendNotificationTime_table();

    return 0;
}

//
// saving the data of your SQLite db (in memory) into a file call:
// loadOrSaveDb(db_con, target_file, 1);
//
// load a SQLite database file into memory, just run:
// loadOrSaveDb(db_con, target_file, 0);
//
int InspectionInfoDBAccess::loadOrSaveDb(int projectid, int isSave)
{
    int rc;                   /* Function return code */
    sqlite3 *pFile;           /* Database connection opened on zFilename */
    sqlite3_backup *pBackup;  /* Backup object used to copy data */
    sqlite3 *pTo;             /* Database to copy to (pFile or pInMemory) */
    sqlite3 *pFrom;           /* Database to copy from (pFile or pInMemory) */
    char zFilename[256];

    memset(zFilename, 0, 256);
    snprintf(zFilename, 256, "%s/tmp/%s_vilstemp%02d%s.db", APP_DIRECTORY, sqlite3db_prefix, projectid, Inspection_prefix);

    //vilslog::printf("InspectionInfoDBAccess::loadOrSaveDb() isSave[%d] zFilename[%s]\n", isSave, zFilename);

    rc = sqlite3_open(zFilename, &pFile);
    if (rc == SQLITE_OK)
    {
        //vilslog::printf("InspectionInfoDBAccess::loadOrSaveDb() sqlite3_open rc == SQLITE_OK\n");

        pFrom = (isSave ? mSqlitedb : pFile);
        pTo = (isSave ? pFile : mSqlitedb);

        pBackup = sqlite3_backup_init(pTo, "main", pFrom, "main");

        //vilslog::printf("InspectionInfoDBAccess::loadOrSaveDb() isSave[%d] zFilename[%s]\n", isSave, zFilename);

        if (pBackup)
        {
            // vilslog::printf("InspectionInfoDBAccess::loadOrSaveDb() isSave[%d] zFilename[%s] start Backup...\n", isSave, zFilename);

            //===========================================
            //(void)sqlite3_backup_step(pBackup, -1);
            //(void)sqlite3_backup_finish(pBackup);
            //===========================================
            do {
                rc = sqlite3_backup_step(pBackup, 20);
                //int remaining = sqlite3_backup_remaining(pBackup);
                //int pagecount = sqlite3_backup_pagecount(pBackup);

                // vilslog::printf("InspectionInfoDBAccess::loadOrSaveDb() isSave[%d] zFilename[%s] pagecount[%d] remaining[%d]\n",
                //     isSave, zFilename, pagecount, remaining);

                // if( rc==SQLITE_OK || rc==SQLITE_BUSY || rc==SQLITE_LOCKED )
                // {
                //   sqlite3_sleep(250);
                // }
            } while( rc==SQLITE_OK || rc==SQLITE_BUSY || rc==SQLITE_LOCKED );
            //===========================================

            //vilslog::printf("InspectionInfoDBAccess::loadOrSaveDb() isSave[%d] zFilename[%s] finish Backup.\n", isSave, zFilename);
        }
        rc = sqlite3_errcode(pTo);
        //vilslog::printf("InspectionInfoDBAccess::loadOrSaveDb() isSave[%d] zFilename[%s] rc[%d]\n", isSave, zFilename, rc);
    }
    else
    {
        vilslog::printf("InspectionInfoDBAccess::loadOrSaveDb() isSave[%d] zFilename[%s] rc != SQLITE_OK\n", isSave, zFilename);
    }
    (void)sqlite3_close(pFile);
    return rc;
}

bool InspectionInfoDBAccess::clear_old_data(const char * datetime)
{
    //std::string olddatetime = BeforeMinDateTimeForSQL(40);

    //clear_old_data("SendNotificationTime", "datetime", olddatetime.c_str());

    return true;
}

bool InspectionInfoDBAccess::clear_old_data(const char * table, const char * column, const char * datetime)
{
    char cmd[4096];

    sprintf(cmd, "DELETE FROM %s WHERE %s <= '%s';",
        table, column, datetime);


    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    // vilslog::printf("InspectionInfoDBAccess::clear_old_data() cmd[%s]\n", cmd);

    int nResult = sqlite3_exec(mSqlitedb, cmd, NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("InspectionInfoDBAccess::clear_old_data() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
        return false;
    }

    return true;
}

bool InspectionInfoDBAccess::checkDBExist()
{
    if (mSqlitedb == NULL)
        return false;
    if (!tableExists("SendNotificationTime"))
        return false;

    return true;
}

bool InspectionInfoDBAccess::checkDBExist(int flag)
{
    int rc;
    rc = sqlite3_open_v2(mCacheDBName, &mSqlitedb, flag, NULL);
    if( rc != SQLITE_OK )
    {
        printf("InspectionInfoDBAccess::checkDBExist() Can't open database[%s] errmsg[%s]\n", mCacheDBName, sqlite3_errmsg(mSqlitedb));
        return false;
    }

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    // int nResult = sqlite3_exec(mSqlitedb,"select count(*) from AreaRollCall;", NULL, NULL, NULL);
    // if (nResult == SQLITE_BUSY)
    // {
    //     printf("InspectionInfoDBAccess::checkDBExist() SQLITE_BUSY sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
    // }
    // else
    // if (nResult == SQLITE_LOCKED)
    // {
    //     printf("InspectionInfoDBAccess::checkDBExist() SQLITE_LOCKED sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
    // }
    // else
    // if (nResult != SQLITE_OK)
    // {
    //     printf("InspectionInfoDBAccess::checkDBExist() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
    //     return false;
    // }

    return true;
}

int InspectionInfoDBAccess::getTableDataCount(const char * tableName)
{
    char cmd[4096];
    sprintf(cmd, "SELECT count(*) FROM %s;", tableName);

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    int count = 0;
    int nResult = sqlite3_exec(mSqlitedb, cmd, get_count_callback, &count, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("InspectionInfoDBAccess::getTableDataCount() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
    }

    //printf("InspectionInfoDBAccess::getTableDataCount() tableName[%s] count[%d]\n", tableName, count);

    return count;
}

int InspectionInfoDBAccess::create_table()
{
    create_SendNotificationTime_table();

    return 0;
}

bool InspectionInfoDBAccess::check_SendNotificationTime_table()
{
    if (tableExists("SendNotificationTime"))
    {
        //vilslog::printf("check_SendNotificationTime_table() SendNotificationTime datetime bFound[%d]\n", bFound);
        // check_column_Exists("SendNotificationTime", "timeout", "INT(3) NOT NULL DEFAULT 600");
    }
    else
    {
        create_SendNotificationTime_table();
    }
    return true;
}

bool InspectionInfoDBAccess::create_SendNotificationTime_table()
{
    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    std::string sql;
    int nResult;

    sql = "CREATE TABLE SendNotificationTime( \
          GroupID INT(3) NOT NULL DEFAULT 0, \
          PointID INT(3) NOT NULL DEFAULT 0, \
          MessageType INT(3) NOT NULL DEFAULT 0, \
          SendTime CHAR(30) , \
          timeout INT(3) NOT NULL DEFAULT 600, \
          datetime DATETIME DEFAULT CURRENT_TIMESTAMP, \
          PRIMARY KEY (GroupID, PointID, MessageType));";
    nResult = sqlite3_exec(mSqlitedb, sql.c_str(), NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("InspectionInfoDBAccess::create_SendNotificationTime_table() SendNotificationTime sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
    }

    return true;
}

bool InspectionInfoDBAccess::write_SendNotificationTime_record(SendNotificationTimeData * record)
{
    char cmd[4096];

    std::string SendTime = time_tToString(record->SendTime);
    std::string datetime = currentDateTimeForSQL();

    sprintf(cmd, "INSERT OR IGNORE INTO SendNotificationTime(GroupID, PointID, MessageType, SendTime, timeout, datetime) \
                    VALUES (%d, %d, %d, '%s', %d, '%s'); \
                    UPDATE SendNotificationTime SET SendTime='%s', timeout=%d, datetime='%s' WHERE \
                    GroupID=%d AND PointID=%d AND MessageType=%d;",
                    record->GroupID, record->PointID, record->MessageType, SendTime.c_str(), record->timeout, datetime.c_str(),
                    SendTime.c_str(), record->timeout, datetime.c_str(), record->GroupID, record->PointID, record->MessageType);

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    int nResult = sqlite3_exec(mSqlitedb, cmd, NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("InspectionInfoDBAccess::write_SendNotificationTime_record() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
        return false;
    }

    return true;
}

std::list<SendNotificationTimeData *> * InspectionInfoDBAccess::getSendNotificationTimeList()
{
    char cmd[4096];
    std::list<SendNotificationTimeData*> * notificationTimeList = new std::list<SendNotificationTimeData*>();

    sprintf(cmd, "SELECT * from SendNotificationTime;");

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    // printf("InspectionInfoDBAccess::getDeviceActivityNodeIdList() cmd[%s]\n", cmd);

    int nResult = sqlite3_exec(mSqlitedb, cmd, get_SendNotificationTimeList_callback, notificationTimeList, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("InspectionInfoDBAccess::getSendNotificationTimeList() sqlite3_errmsg[%s]\n", sqlite3_errmsg(mSqlitedb));
    }

    return notificationTimeList;
}

std::list<std::string> * InspectionInfoDBAccess::get_column_list(const char * table_name)
{
    char cmd[4096];
    sprintf(cmd, "PRAGMA table_info (%s);", table_name);

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    std::list<std::string> * colList = new std::list<std::string>();
    int nResult = sqlite3_exec(mSqlitedb, cmd, check_column_callback, colList, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("InspectionInfoDBAccess::get_column_list() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
    }
    return colList;
}

bool InspectionInfoDBAccess::create_column(const char * tableName, const char * colName, const char * colDefinition)
{
    char cmd[4096];
    //sprintf(cmd, "SELECT COUNT(*) AS CNTREC FROM pragma_table_info(webconfig) WHERE name='enable_3dmap';");
    sprintf(cmd, "ALTER TABLE %s ADD %s %s;", tableName, colName, colDefinition);

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    int nResult = sqlite3_exec(mSqlitedb, cmd, NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("InspectionInfoDBAccess::create_column() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
    }

    return true;
}

bool InspectionInfoDBAccess::check_column_Exists(const char * tableName, const char * colName, const char * colDefinition)
{
    std::list<std::string> * colList = get_column_list(tableName);

    // check column exist
    bool bFound = false;
    for (std::list<std::string>::iterator i = colList->begin(); i != colList->end(); i++)
    {
        if ( (*i).compare(colName) == 0 )
        {
            bFound = true;
            break;
        }
    }
    if (!bFound)
    {
        // alert a new column
        printf("InspectionInfoDBAccess::check_column_Exists() tableName[%s] colName[%s] not found!!!\n", tableName, colName);
        create_column(tableName, colName, colDefinition);
    }

    delete colList;
    return bFound;
}

bool InspectionInfoDBAccess::tableExists(const char * tableName)
{
    char cmd[4096];

    //
    // check if already fired
    //
    sprintf(cmd, "SELECT count(*) FROM sqlite_master where type='table' AND name='%s';", tableName);

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    //printf("\ncmd[%s]\n",cmd);

    int count = 0;
    int nResult = sqlite3_exec(mSqlitedb, cmd, get_count_callback, &count, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("InspectionInfoDBAccess::tableExists() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
        return false;
    }

    if ( count > 0 )
    {
        // exist
        //printf("SQLiteDBAccess::write_fired_events() already fired!!! count[%d]\n", count);
        return true;
    }
    return false;
}

int InspectionInfoDBAccess::check_column_callback(void* pcolList, int nCount,char** pValue,char** pName)
{
    std::list<std::string> * colList = (std::list<std::string> *)pcolList;
    //printf("SQLiteDBAccess::check_column_callback() nCount[%d]\n", nCount);

    for(int i=0;i<nCount;i++)
    {
        //printf("SQLiteDBAccess::check_column_callback() i[%d] pName[%s] pValue[%s]\n", i, pName[i], pValue[i]);
        std::string s = pName[i];
        if ( s.compare("name") == 0 )
            colList->push_back(pValue[i]);
    }

    return 0;
}

int InspectionInfoDBAccess::get_count_callback(void* nret,int nCount,char** pValue,char** pName)
{
    int * ret = (int *)nret;

    //printf("SQLiteDBAccess::get_count_callback() nCount[%d], *ret[%d]\n", nCount, *ret);

    for(int i=0;i<nCount;i++)
    {
        int cut_count = atoi(pValue[i]);
        if (cut_count > *ret)
            *ret = cut_count;
    }
    return 0;
}

int InspectionInfoDBAccess::get_StringValueList_callback(void* pList,int nCount,char** pValue,char** pName)
{
    std::list<std::string> * stringValueList = (std::list<std::string> *)pList;

    for(int i=0;i<nCount;i++)
    {
        std::string s = pName[i];

        if ( s.compare("stringvalue") == 0 )
        {
            stringValueList->push_back(pValue[i]);
        }
    }

    return 0;
}

int InspectionInfoDBAccess::get_SendNotificationTimeList_callback(void* pList, int nCount,char** pValue,char** pName)
{
    std::list<SendNotificationTimeData*> * records = (std::list<SendNotificationTimeData*> *)pList;

    SendNotificationTimeData * record = new SendNotificationTimeData();

    for(int i=0;i<nCount;i++)
    {
        std::string s = pName[i];

        if ( s.compare("GroupID") == 0 )
            record->GroupID = atoi(pValue[i]);
        else
        if ( s.compare("PointID") == 0 )
            record->PointID = atoi(pValue[i]);
        else
        if ( s.compare("MessageType") == 0 )
            record->MessageType = atoi(pValue[i]);
        else
        if ( s.compare("SendTime") == 0 )
            record->SendTime = StringDateTimeTotime_t(pValue[i]);
        else
        if ( s.compare("timeout") == 0 )
            record->timeout = atoi(pValue[i]);
    }
    records->push_back(record);

    return 0;
}

int InspectionInfoDBAccess::callback_busy_handler(void *ptr,int count)
{
    //usleep(500000);   //如果获取不到锁，等待0.5秒
    usleep(5000);   //如果获取不到锁，等待 5ms
    //printf("database is locak now,can not write/read.\n");  //每次执行一次回调函数打印一次该信息
    return 1;    //回调函数返回值为1，则将不断尝试操作数据库。
}
