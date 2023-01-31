/*
 * AreaRollCallInfoDBAccess.cpp
 *
 *  Created on: 2020/03/09
 *      Author: ikki
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "AreaRollCallInfoDBAccess.h"
#include "../common.h"

using namespace std;


//
// AreaRollCallInfoDBAccess
//
AreaRollCallInfoDBAccess::AreaRollCallInfoDBAccess()
{
    sqlite3_initialize();

    sprintf(mCacheDBName, cachedbname);
    vilslog::printf("AreaRollCallInfoDBAccess::AreaRollCallInfoDBAccess() mCacheDBName[%s]\n", mCacheDBName);

    int flag = (SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
//    printf("AreaRollCallInfoDBAccess::AreaRollCallInfoDBAccess() flag[%d]\n", flag);
    int rc = sqlite3_open_v2(mCacheDBName, &mSqlitedb, flag, NULL);
    if( rc != SQLITE_OK )
    {
        vilslog::printf("AreaRollCallInfoDBAccess::AreaRollCallInfoDBAccess() Can't open database[%s] errmsg[%s]\n", mCacheDBName, sqlite3_errmsg(mSqlitedb));
    }
    else
    {
        mSqlitedb = NULL;
    }
}

AreaRollCallInfoDBAccess::~AreaRollCallInfoDBAccess()
{
    sqlite3_close(mSqlitedb);
    sqlite3_shutdown();
}

int AreaRollCallInfoDBAccess::init()
{
    int flag = (SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
    if (!checkDBExist(flag))
    {
        vilslog::printf("AreaRollCallInfoDBAccess::init() database not exist!\n");
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
            vilslog::printf("AreaRollCallInfoDBAccess::init() sqlite3 is thread safe.\n");
        }
        else
        {
            vilslog::printf("AreaRollCallInfoDBAccess::init() sqlite3 is *** NOT *** thread safe!!!\n");
        }
    }

    check_valid_table();

    return 0;
}

int AreaRollCallInfoDBAccess::check_valid_table()
{
    check_TagAreaBuffer_table();
    check_SleepPool_table();

    return 0;
}

//
// saving the data of your SQLite db (in memory) into a file call:
// loadOrSaveDb(db_con, target_file, 1);
//
// load a SQLite database file into memory, just run:
// loadOrSaveDb(db_con, target_file, 0);
//
int AreaRollCallInfoDBAccess::loadOrSaveDb(int projectid, int isSave)
{
    int rc;                   /* Function return code */
    sqlite3 *pFile;           /* Database connection opened on zFilename */
    sqlite3_backup *pBackup;  /* Backup object used to copy data */
    sqlite3 *pTo;             /* Database to copy to (pFile or pInMemory) */
    sqlite3 *pFrom;           /* Database to copy from (pFile or pInMemory) */
    char zFilename[256];

    mProjectid = projectid;

    memset(zFilename, 0, 256);
    snprintf(zFilename, 256, "%s/tmp/%s_vilstemp%02d%s.db", APP_DIRECTORY, sqlite3db_prefix, projectid, AreaRollCall_prefix);

    //vilslog::printf("AreaRollCallInfoDBAccess::loadOrSaveDb() isSave[%d] zFilename[%s]\n", isSave, zFilename);

    rc = sqlite3_open(zFilename, &pFile);
    if (rc == SQLITE_OK)
    {
        //vilslog::printf("AreaRollCallInfoDBAccess::loadOrSaveDb() sqlite3_open rc == SQLITE_OK\n");

        pFrom = (isSave ? mSqlitedb : pFile);
        pTo = (isSave ? pFile : mSqlitedb);

        pBackup = sqlite3_backup_init(pTo, "main", pFrom, "main");

        //vilslog::printf("AreaRollCallInfoDBAccess::loadOrSaveDb() isSave[%d] zFilename[%s]\n", isSave, zFilename);

        if (pBackup)
        {
            // vilslog::printf("AreaRollCallInfoDBAccess::loadOrSaveDb() isSave[%d] zFilename[%s] start Backup...\n", isSave, zFilename);

            //===========================================
            //(void)sqlite3_backup_step(pBackup, -1);
            //(void)sqlite3_backup_finish(pBackup);
            //===========================================
            do {
                rc = sqlite3_backup_step(pBackup, 20);
                //int remaining = sqlite3_backup_remaining(pBackup);
                //int pagecount = sqlite3_backup_pagecount(pBackup);

                // vilslog::printf("AreaRollCallInfoDBAccess::loadOrSaveDb() isSave[%d] zFilename[%s] pagecount[%d] remaining[%d]\n",
                //     isSave, zFilename, pagecount, remaining);

                // if( rc==SQLITE_OK || rc==SQLITE_BUSY || rc==SQLITE_LOCKED )
                // {
                //   sqlite3_sleep(250);
                // }
            } while( rc==SQLITE_OK || rc==SQLITE_BUSY || rc==SQLITE_LOCKED );
            //===========================================

            //vilslog::printf("AreaRollCallInfoDBAccess::loadOrSaveDb() isSave[%d] zFilename[%s] finish Backup.\n", isSave, zFilename);
        }
        rc = sqlite3_errcode(pTo);
        //vilslog::printf("AreaRollCallInfoDBAccess::loadOrSaveDb() isSave[%d] zFilename[%s] rc[%d]\n", isSave, zFilename, rc);
    }
    else
    {
        vilslog::printf("AreaRollCallInfoDBAccess::loadOrSaveDb() isSave[%d] zFilename[%s] rc != SQLITE_OK\n", isSave, zFilename);
    }
    (void)sqlite3_close(pFile);
    return rc;
}

bool AreaRollCallInfoDBAccess::clear_old_data(const char * datetime)
{
    //std::string olddatetime = BeforeMinDateTimeForSQL(40);

    //clear_old_data("SendNotificationTime", "datetime", olddatetime.c_str());

    return true;
}

bool AreaRollCallInfoDBAccess::clear_old_data(const char * table, const char * column, const char * datetime)
{
    char cmd[4096];

    sprintf(cmd, "DELETE FROM %s WHERE %s <= '%s';",
        table, column, datetime);


    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    // vilslog::printf("AreaRollCallInfoDBAccess::clear_old_data() cmd[%s]\n", cmd);

    int nResult = sqlite3_exec(mSqlitedb, cmd, NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AreaRollCallInfoDBAccess::clear_old_data() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
        return false;
    }

    return true;
}

bool AreaRollCallInfoDBAccess::checkDBExist()
{
    if (mSqlitedb == NULL)
        return false;
    if (!tableExists("TagAreaBuffer"))
        return false;
    if (!tableExists("SleepPool"))
        return false;

    return true;
}

bool AreaRollCallInfoDBAccess::checkDBExist(int flag)
{
    int rc;
    rc = sqlite3_open_v2(mCacheDBName, &mSqlitedb, flag, NULL);
    if( rc != SQLITE_OK )
    {
        printf("AreaRollCallInfoDBAccess::checkDBExist() Can't open database[%s] errmsg[%s]\n", mCacheDBName, sqlite3_errmsg(mSqlitedb));
        return false;
    }

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    // int nResult = sqlite3_exec(mSqlitedb,"select count(*) from AreaRollCall;", NULL, NULL, NULL);
    // if (nResult == SQLITE_BUSY)
    // {
    //     printf("AreaRollCallInfoDBAccess::checkDBExist() SQLITE_BUSY sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
    // }
    // else
    // if (nResult == SQLITE_LOCKED)
    // {
    //     printf("AreaRollCallInfoDBAccess::checkDBExist() SQLITE_LOCKED sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
    // }
    // else
    // if (nResult != SQLITE_OK)
    // {
    //     printf("AreaRollCallInfoDBAccess::checkDBExist() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
    //     return false;
    // }

    return true;
}

int AreaRollCallInfoDBAccess::getTableDataCount(const char * tableName)
{
    char cmd[4096];
    sprintf(cmd, "SELECT count(*) FROM %s;", tableName);

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    int count = 0;
    int nResult = sqlite3_exec(mSqlitedb, cmd, get_count_callback, &count, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AreaRollCallInfoDBAccess::getTableDataCount() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
    }

    //printf("AreaRollCallInfoDBAccess::getTableDataCount() tableName[%s] count[%d]\n", tableName, count);

    return count;
}

int AreaRollCallInfoDBAccess::create_table()
{
    create_TagAreaBuffer_table();
    create_SleepPool_table();

    return 0;
}

bool AreaRollCallInfoDBAccess::check_TagAreaBuffer_table()
{
    if (tableExists("TagAreaBuffer"))
    {
        //vilslog::printf("check_TagAreaBuffer_table() SendNotificationTime datetime bFound[%d]\n", bFound);
        // check_column_Exists("TagAreaBuffer", "isuser", "INT(3) NOT NULL DEFAULT 0");
    }
    else
    {
        create_TagAreaBuffer_table();
    }
    return true;
}

bool AreaRollCallInfoDBAccess::create_TagAreaBuffer_table()
{
    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    std::string sql;
    int nResult;

    sql = "CREATE TABLE TagAreaBuffer( \
          nodeid CHAR(30) NOT NULL, \
          tafid INT(3) NOT NULL DEFAULT 0, \
          isuser INT(3) NOT NULL DEFAULT 0, \
          areaid CHAR(30) NOT NULL, \
          areaname CHAR(30) NOT NULL, \
          AreaType0D INT(3) DEFAULT 0, \
          MapAreaType INT(3) DEFAULT 0, \
          checked_in INT(3) DEFAULT 0, \
          velocity_in CHAR(30), \
          voltage_in CHAR(30), \
          timestamp_in CHAR(30) , \
          checked_out INT(3) DEFAULT 0, \
          velocity_out CHAR(30), \
          voltage_out CHAR(30), \
          timestamp_out CHAR(30) , \
          datetime DATETIME DEFAULT CURRENT_TIMESTAMP, \
          PRIMARY KEY (nodeid, tafid, areaid));";
    nResult = sqlite3_exec(mSqlitedb, sql.c_str(), NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AreaRollCallInfoDBAccess::create_TagAreaBuffer_table() TagAreaBuffer sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
    }

    return true;
}

bool AreaRollCallInfoDBAccess::check_SleepPool_table()
{
    if (tableExists("SleepPool"))
    {
        //vilslog::printf("check_SleepPool_table() SendNotificationTime datetime bFound[%d]\n", bFound);
        check_column_Exists("SleepPool", "isuser", "INT(3) NOT NULL DEFAULT 0");
    }
    else
    {
        create_SleepPool_table();
    }
    return true;
}

bool AreaRollCallInfoDBAccess::create_SleepPool_table()
{
    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    std::string sql;
    int nResult;

    sql = "CREATE TABLE SleepPool( \
          nodeid CHAR(30) NOT NULL, \
          tafid INT(3) NOT NULL DEFAULT 0, \
          isuser INT(3) NOT NULL DEFAULT 0, \
          check_count INT(3) NOT NULL DEFAULT 0, \
          slept INT(3) NOT NULL DEFAULT 0 , \
          datetime DATETIME DEFAULT CURRENT_TIMESTAMP, \
          PRIMARY KEY (nodeid, tafid));";
    nResult = sqlite3_exec(mSqlitedb, sql.c_str(), NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AreaRollCallInfoDBAccess::create_SleepPool_table() SleepPool sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
    }

    return true;
}

bool AreaRollCallInfoDBAccess::write_TagAreaBuffer_record(TagAreaBufferStruct * record, BufferAreaInfoStruct * buffer)
{
    char cmd[4096];

    time_t t_timestamp_in = buffer->timestamp_in / 1000; // millisecond -> second
    if ((buffer->timestamp_in % 1000) > 0)
        t_timestamp_in += 1;

    time_t t_timestamp_out = buffer->timestamp_out / 1000; // millisecond -> second
    if ((buffer->timestamp_out % 1000) > 0)
        t_timestamp_out += 1;

    std::string timestamp_in = time_tToString(t_timestamp_in);
    std::string timestamp_out = time_tToString(t_timestamp_out);
    std::string datetime = currentDateTimeForSQL();

    sprintf(cmd, "INSERT OR IGNORE INTO TagAreaBuffer(nodeid, tafid, isuser, areaid, areaname, AreaType0D, MapAreaType, \
                    checked_in, velocity_in, voltage_in, timestamp_in, \
                    checked_out, velocity_out, voltage_out, timestamp_out, datetime) \
                    VALUES ('%s', %d, %d, '%s', '%s', %d, %d, %d, '%0.2f', '%0.2f', '%s', %d, '%0.2f', '%0.2f', '%s', '%s'); \
                    UPDATE TagAreaBuffer SET areaname='%s', AreaType0D=%d, MapAreaType=%d, \
                    checked_in=%d, velocity_in='%0.2f', voltage_in='%0.2f', timestamp_in='%s', \
                    checked_out=%d, velocity_out='%0.2f', voltage_out='%0.2f', timestamp_out='%s', datetime='%s' \
                    WHERE nodeid='%s' AND tafid=%d AND isuser=%d AND areaid='%s';",
                    record->nodeid.c_str(), record->tafid, record->isuser, buffer->areaid.c_str(), buffer->areaname.c_str(),
                    buffer->AreaType0D, buffer->MapAreaType,
                    buffer->checked_in, buffer->velocity_in, buffer->voltage_in, timestamp_in.c_str(),
                    buffer->checked_out, buffer->velocity_out, buffer->voltage_out, timestamp_out.c_str(), datetime.c_str(),
                    buffer->areaname.c_str(), buffer->AreaType0D, buffer->MapAreaType,
                    buffer->checked_in, buffer->velocity_in, buffer->voltage_in, timestamp_in.c_str(),
                    buffer->checked_out, buffer->velocity_out, buffer->voltage_out, timestamp_out.c_str(), datetime.c_str(),
                    record->nodeid.c_str(), record->tafid, record->isuser, buffer->areaid.c_str());

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    // vilslog::printf("AreaRollCallInfoDBAccess::write_TagAreaBuffer_record() cmd[%s]\n", cmd);

    int nResult = sqlite3_exec(mSqlitedb, cmd, NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        vilslog::printf("AreaRollCallInfoDBAccess::write_TagAreaBuffer_record() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
        return false;
    }

    return true;
}

bool AreaRollCallInfoDBAccess::remove_TagAreaBuffer_record(TagAreaBufferStruct * record, BufferAreaInfoStruct * buffer)
{
    char cmd[4096];

    sprintf(cmd, "DELETE from TagAreaBuffer WHERE nodeid='%s' AND tafid=%d AND isuser=%d AND areaid='%s';",
        record->nodeid.c_str(), record->tafid, record->isuser, buffer->areaid.c_str());

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    // vilslog::printf("AreaRollCallInfoDBAccess::remove_TagAreaBuffer_record() cmd[%s]\n", cmd);

    int nResult = sqlite3_exec(mSqlitedb, cmd, NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        vilslog::printf("AreaRollCallInfoDBAccess::remove_TagAreaBuffer_record() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
        return false;
    }

    return true;
}

bool AreaRollCallInfoDBAccess::write_SleepPool_record(SleepPoolStruct * record)
{
    char cmd[4096];
    std::string datetime = currentDateTimeForSQL();

    sprintf(cmd, "INSERT OR IGNORE INTO SleepPool(nodeid, tafid, isuser, check_count, slept, datetime) \
                    VALUES ('%s', %d, %d, %d, %d, '%s'); \
                    UPDATE SleepPool SET check_count=%d, slept=%d, datetime='%s' WHERE \
                    nodeid='%s' AND tafid=%d AND isuser=%d;",
                    record->nodeid.c_str(), record->tafid, record->isuser, record->check_count, record->slept, datetime.c_str(),
                    record->check_count, record->slept, datetime.c_str(), record->nodeid.c_str(), record->tafid, record->isuser);

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    // vilslog::printf("AreaRollCallInfoDBAccess::write_SleepPool_record() cmd[%s]\n", cmd);

    int nResult = sqlite3_exec(mSqlitedb, cmd, NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        vilslog::printf("AreaRollCallInfoDBAccess::write_SleepPool_record() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
        return false;
    }

    return true;
}

bool AreaRollCallInfoDBAccess::deleteTagAreaBuffer(TagAreaBufferStruct * record)
{
    char cmd[4096];

    sprintf(cmd, "DELETE from TagAreaBuffer WHERE nodeid='%s' AND tafid=%d AND isuser=%d;",
        record->nodeid.c_str(), record->tafid, record->isuser);

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    // vilslog::printf("AreaRollCallInfoDBAccess::deleteTagAreaBuffer() cmd[%s]\n", cmd);

    int nResult = sqlite3_exec(mSqlitedb, cmd, NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        vilslog::printf("AreaRollCallInfoDBAccess::deleteTagAreaBuffer() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
        return false;
    }

    return true;
}

bool AreaRollCallInfoDBAccess::deleteTagSleepPool(SleepPoolStruct * record)
{
    char cmd[4096];

    sprintf(cmd, "DELETE from SleepPool WHERE nodeid='%s' AND tafid=%d AND isuser=%d;",
        record->nodeid.c_str(), record->tafid, record->isuser);

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    // vilslog::printf("AreaRollCallInfoDBAccess::deleteTagAreaBuffer() cmd[%s]\n", cmd);

    int nResult = sqlite3_exec(mSqlitedb, cmd, NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        vilslog::printf("AreaRollCallInfoDBAccess::deleteTagSleepPool() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
        return false;
    }

    return true;
}

std::list<TagAreaBufferStruct *> * AreaRollCallInfoDBAccess::getTagAreaBufferList()
{
    char cmd[4096];
    std::list<TagAreaBufferStruct*> * tagAreaBufferList = new std::list<TagAreaBufferStruct*>();

    sprintf(cmd, "SELECT * from TagAreaBuffer;");

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    //printf("AreaRollCallInfoDBAccess::getTagAreaBufferList() cmd[%s]\n", cmd);

    int nResult = sqlite3_exec(mSqlitedb, cmd, get_TagAreaBufferList_callback, tagAreaBufferList, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AreaRollCallInfoDBAccess::getTagAreaBufferList() sqlite3_errmsg[%s]\n", sqlite3_errmsg(mSqlitedb));
    }

    return tagAreaBufferList;
}

std::list<SleepPoolStruct *> * AreaRollCallInfoDBAccess::getSleepPoolList()
{
    char cmd[4096];
    std::list<SleepPoolStruct*> * sleepPoolStructList = new std::list<SleepPoolStruct*>();

    sprintf(cmd, "SELECT * from SleepPool;");

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    //printf("AreaRollCallInfoDBAccess::getSleepPoolList() cmd[%s]\n", cmd);

    int nResult = sqlite3_exec(mSqlitedb, cmd, get_SleepPoolList_callback, sleepPoolStructList, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AreaRollCallInfoDBAccess::getSleepPoolList() sqlite3_errmsg[%s]\n", sqlite3_errmsg(mSqlitedb));
    }

    return sleepPoolStructList;
}

std::list<std::string> * AreaRollCallInfoDBAccess::get_column_list(const char * table_name)
{
    char cmd[4096];
    sprintf(cmd, "PRAGMA table_info (%s);", table_name);

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    std::list<std::string> * colList = new std::list<std::string>();
    int nResult = sqlite3_exec(mSqlitedb, cmd, check_column_callback, colList, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AreaRollCallInfoDBAccess::get_column_list() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
    }
    return colList;
}

bool AreaRollCallInfoDBAccess::create_column(const char * tableName, const char * colName, const char * colDefinition)
{
    char cmd[4096];
    //sprintf(cmd, "SELECT COUNT(*) AS CNTREC FROM pragma_table_info(webconfig) WHERE name='enable_3dmap';");
    sprintf(cmd, "ALTER TABLE %s ADD %s %s;", tableName, colName, colDefinition);

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    int nResult = sqlite3_exec(mSqlitedb, cmd, NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AreaRollCallInfoDBAccess::create_column() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
    }

    return true;
}

bool AreaRollCallInfoDBAccess::check_column_Exists(const char * tableName, const char * colName, const char * colDefinition)
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
        printf("AreaRollCallInfoDBAccess::check_column_Exists() tableName[%s] colName[%s] not found!!!\n", tableName, colName);
        create_column(tableName, colName, colDefinition);
    }

    delete colList;
    return bFound;
}

bool AreaRollCallInfoDBAccess::tableExists(const char * tableName)
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
        printf("AreaRollCallInfoDBAccess::tableExists() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
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

int AreaRollCallInfoDBAccess::check_column_callback(void* pcolList, int nCount,char** pValue,char** pName)
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

int AreaRollCallInfoDBAccess::get_count_callback(void* nret,int nCount,char** pValue,char** pName)
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

int AreaRollCallInfoDBAccess::get_StringValueList_callback(void* pList,int nCount,char** pValue,char** pName)
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

int AreaRollCallInfoDBAccess::get_TagAreaBufferList_callback(void* pList, int nCount,char** pValue,char** pName)
{
    std::list<TagAreaBufferStruct*> * records = (std::list<TagAreaBufferStruct*> *)pList;

    TagAreaBufferStruct * tagarea = new TagAreaBufferStruct();
    BufferAreaInfoStruct * areainfo = new BufferAreaInfoStruct();

    for(int i=0;i<nCount;i++)
    {
        std::string s = pName[i];

        if ( s.compare("nodeid") == 0 )
            tagarea->nodeid = pValue[i];
        else
        if ( s.compare("tafid") == 0 )
            tagarea->tafid = atoi(pValue[i]);
        else
        if ( s.compare("isuser") == 0 )
            tagarea->isuser = atoi(pValue[i]);
        else
        if ( s.compare("areaid") == 0 )
            areainfo->areaid = pValue[i];
        else
        if ( s.compare("areaname") == 0 )
            areainfo->areaname = pValue[i];
        else
        if ( s.compare("AreaType0D") == 0 )
            areainfo->AreaType0D = atoi(pValue[i]);
        else
        if ( s.compare("MapAreaType") == 0 )
            areainfo->MapAreaType = atoi(pValue[i]);
        else
        if ( s.compare("checked_in") == 0 )
            areainfo->checked_in = atoi(pValue[i]);
        else
        if ( s.compare("velocity_in") == 0 )
            areainfo->velocity_in = atof(pValue[i]);
        else
        if ( s.compare("voltage_in") == 0 )
            areainfo->voltage_in = atof(pValue[i]);
        else
        if ( s.compare("timestamp_in") == 0 )
            areainfo->timestamp_in = StringDateTimeTotime_t(pValue[i]) * 1000;
        else
        if ( s.compare("checked_out") == 0 )
            areainfo->checked_out = atoi(pValue[i]);
        else
        if ( s.compare("velocity_out") == 0 )
            areainfo->velocity_out = atof(pValue[i]);
        else
        if ( s.compare("voltage_out") == 0 )
            areainfo->voltage_out = atof(pValue[i]);
        else
        if ( s.compare("timestamp_out") == 0 )
            areainfo->timestamp_out = StringDateTimeTotime_t(pValue[i]) * 1000;
    }

    TagAreaBufferStruct * record = NULL;
    bool bFound = false;
    for (std::list<TagAreaBufferStruct*>::iterator buffer = records->begin(); buffer != records->end(); buffer++)
    {
        if ( ((*buffer)->tafid == tagarea->tafid) && ((*buffer)->nodeid.compare(tagarea->nodeid) == 0) )
        {
            record = (*buffer);
            bFound = true;
            break;
        }
    }

    if(!bFound)
    {
        record = new TagAreaBufferStruct();
        record->nodeid = tagarea->nodeid;
        record->tafid = tagarea->tafid;
        record->isuser = tagarea->isuser;
        records->push_back(record);
    }

    record->mBufferAreaList.push_back(areainfo);

    delete tagarea;

    return 0;
}

int AreaRollCallInfoDBAccess::get_SleepPoolList_callback(void* pList, int nCount,char** pValue,char** pName)
{
    std::list<SleepPoolStruct*> * records = (std::list<SleepPoolStruct*> *)pList;

    SleepPoolStruct * record = new SleepPoolStruct();

    for(int i=0;i<nCount;i++)
    {
        std::string s = pName[i];

        if ( s.compare("nodeid") == 0 )
            record->nodeid = pValue[i];
        else
        if ( s.compare("tafid") == 0 )
            record->tafid = atoi(pValue[i]);
        else
        if ( s.compare("isuser") == 0 )
            record->isuser = atoi(pValue[i]);
        else
        if ( s.compare("check_count") == 0 )
            record->check_count = atoi(pValue[i]);
        else
        if ( s.compare("slept") == 0 )
            record->slept = atoi(pValue[i]);
    }
    records->push_back(record);

    return 0;
}

int AreaRollCallInfoDBAccess::callback_busy_handler(void *ptr,int count)
{
    //usleep(500000);   //如果获取不到锁，等待0.5秒
    usleep(5000);   //如果获取不到锁，等待 5ms
    //printf("database is locak now,can not write/read.\n");  //每次执行一次回调函数打印一次该信息
    return 1;    //回调函数返回值为1，则将不断尝试操作数据库。
}
