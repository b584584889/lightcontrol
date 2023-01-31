/*
 * AnalyzeDashBoardDBAccess.cpp
 *
 *  Created on: 2020/01/31
 *      Author: ikki
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

#include "AnalyzeDashBoardDBAccess.h"
#include "../common.h"

using namespace std;


//
// AnalyzeDashBoardDBAccess
//
AnalyzeDashBoardDBAccess::AnalyzeDashBoardDBAccess()
{
    sqlite3_initialize();

    sprintf(mCacheDBName, cachedbname);
    vilslog::printf("AnalyzeDashBoardDBAccess::AnalyzeDashBoardDBAccess() mCacheDBName[%s]\n", mCacheDBName);

    int flag = (SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
//    printf("AnalyzeDashBoardDBAccess::AnalyzeDashBoardDBAccess() flag[%d]\n", flag);
    int rc = sqlite3_open_v2(mCacheDBName, &mSqlitedb, flag, NULL);
    if( rc != SQLITE_OK )
    {
        printf("AnalyzeDashBoardDBAccess::AnalyzeDashBoardDBAccess() Can't open database[%s] errmsg[%s]\n", mCacheDBName, sqlite3_errmsg(mSqlitedb));
    }
    else
    {
        mSqlitedb = NULL;
    }
}

AnalyzeDashBoardDBAccess::~AnalyzeDashBoardDBAccess()
{
    sqlite3_close(mSqlitedb);
    sqlite3_shutdown();
}

int AnalyzeDashBoardDBAccess::init()
{
    int flag = (SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
    if (!checkDBExist(flag))
    {
        vilslog::printf("AnalyzeDashBoardDBAccess::init() database not exist!\n");
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
            vilslog::printf("AnalyzeDashBoardDBAccess::init() sqlite3 is thread safe.\n");
        }
        else
        {
            vilslog::printf("AnalyzeDashBoardDBAccess::init() sqlite3 is *** NOT *** thread safe!!!\n");
        }
    }

    check_valid_table();

    return 0;
}

int AnalyzeDashBoardDBAccess::check_valid_table()
{
    check_DeviceActivity_table();
    check_AreaRollCall_table();
    check_OperationMode_table();
    check_NodeMoveOffsetInfo_table();

    return 0;
}

//
// saving the data of your SQLite db (in memory) into a file call:
// loadOrSaveDb(db_con, target_file, 1);
//
// load a SQLite database file into memory, just run:
// loadOrSaveDb(db_con, target_file, 0);
//
int AnalyzeDashBoardDBAccess::loadOrSaveDb(int projectid, int isSave)
{
    int rc;                   /* Function return code */
    sqlite3 *pFile;           /* Database connection opened on zFilename */
    sqlite3_backup *pBackup;  /* Backup object used to copy data */
    sqlite3 *pTo;             /* Database to copy to (pFile or pInMemory) */
    sqlite3 *pFrom;           /* Database to copy from (pFile or pInMemory) */
    char zFilename[256];

    memset(zFilename, 0, 256);
    snprintf(zFilename, 256, "%s/tmp/%s_vilstemp%02d.db", APP_DIRECTORY, sqlite3db_prefix, projectid);

    //vilslog::printf("AnalyzeDashBoardDBAccess::loadOrSaveDb() isSave[%d] zFilename[%s]\n", isSave, zFilename);

    rc = sqlite3_open(zFilename, &pFile);
    if (rc == SQLITE_OK)
    {
        //vilslog::printf("AnalyzeDashBoardDBAccess::loadOrSaveDb() sqlite3_open rc == SQLITE_OK\n");

        pFrom = (isSave ? mSqlitedb : pFile);
        pTo = (isSave ? pFile : mSqlitedb);

        pBackup = sqlite3_backup_init(pTo, "main", pFrom, "main");

        //vilslog::printf("AnalyzeDashBoardDBAccess::loadOrSaveDb() isSave[%d] zFilename[%s]\n", isSave, zFilename);

        if (pBackup)
        {
            // vilslog::printf("AnalyzeDashBoardDBAccess::loadOrSaveDb() isSave[%d] zFilename[%s] start Backup...\n", isSave, zFilename);

            //===========================================
            //(void)sqlite3_backup_step(pBackup, -1);
            //(void)sqlite3_backup_finish(pBackup);
            //===========================================
            do {
                rc = sqlite3_backup_step(pBackup, 20);
                //int remaining = sqlite3_backup_remaining(pBackup);
                //int pagecount = sqlite3_backup_pagecount(pBackup);

                // vilslog::printf("AnalyzeDashBoardDBAccess::loadOrSaveDb() isSave[%d] zFilename[%s] pagecount[%d] remaining[%d]\n",
                //     isSave, zFilename, pagecount, remaining);

                // if( rc==SQLITE_OK || rc==SQLITE_BUSY || rc==SQLITE_LOCKED )
                // {
                //   sqlite3_sleep(250);
                // }
            } while( rc==SQLITE_OK || rc==SQLITE_BUSY || rc==SQLITE_LOCKED );
            //===========================================

            //vilslog::printf("AnalyzeDashBoardDBAccess::loadOrSaveDb() isSave[%d] zFilename[%s] finish Backup.\n", isSave, zFilename);
        }
        rc = sqlite3_errcode(pTo);
        //vilslog::printf("AnalyzeDashBoardDBAccess::loadOrSaveDb() isSave[%d] zFilename[%s] rc[%d]\n", isSave, zFilename, rc);
    }
    else
    {
        vilslog::printf("AnalyzeDashBoardDBAccess::loadOrSaveDb() isSave[%d] zFilename[%s] rc != SQLITE_OK\n", isSave, zFilename);
    }
    (void)sqlite3_close(pFile);
    return rc;
}

bool AnalyzeDashBoardDBAccess::clear_old_data(const char * datetime)
{
    //char issueday[20];

    //memset(issueday, 0, 20);
    //strncpy(issueday, datetime, 10);
    //clear_old_data("DeviceActivity", "issueday", issueday);

    // int count = getTableDataCount("DeviceActivity");
    // printf("AnalyzeDashBoardDBAccess::clear_old_data() before DeviceActivity count[%d]\n", count);
    // count = getTableDataCount("AreaRollCall");
    // printf("AnalyzeDashBoardDBAccess::clear_old_data() before AreaRollCall count[%d]\n", count);
    // count = getTableDataCount("OperationMode");
    // printf("AnalyzeDashBoardDBAccess::clear_old_data() before OperationMode count[%d]\n", count);
    // count = getTableDataCount("NodeMoveOffsetInfo");
    // printf("AnalyzeDashBoardDBAccess::clear_old_data() before NodeMoveOffsetInfo count[%d]\n", count);

    std::string olddatetime = BeforeMinDateTimeForSQL(40);

    clear_old_data("DeviceActivity", "datetime", olddatetime.c_str());
    clear_old_data("AreaRollCall", "datetime", datetime);
    clear_old_data("OperationMode", "datetime", datetime);
    clear_old_data("NodeMoveOffsetInfo", "datetime", datetime);

    // count = getTableDataCount("DeviceActivity");
    // printf("AnalyzeDashBoardDBAccess::clear_old_data() after DeviceActivity count[%d]\n", count);
    // count = getTableDataCount("AreaRollCall");
    // printf("AnalyzeDashBoardDBAccess::clear_old_data() after AreaRollCall count[%d]\n", count);
    // count = getTableDataCount("OperationMode");
    // printf("AnalyzeDashBoardDBAccess::clear_old_data() after OperationMode count[%d]\n", count);
    // count = getTableDataCount("NodeMoveOffsetInfo");
    // printf("AnalyzeDashBoardDBAccess::clear_old_data() after NodeMoveOffsetInfo count[%d]\n", count);

    return true;
}

bool AnalyzeDashBoardDBAccess::clear_old_data(const char * table, const char * column, const char * datetime)
{
    char cmd[4096];

    sprintf(cmd, "DELETE FROM %s WHERE %s <= '%s';",
        table, column, datetime);


    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    // vilslog::printf("AnalyzeDashBoardDBAccess::clear_old_data() cmd[%s]\n", cmd);

    int nResult = sqlite3_exec(mSqlitedb, cmd, NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        vilslog::printf("AnalyzeDashBoardDBAccess::clear_old_data() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
        return false;
    }

    return true;
}

bool AnalyzeDashBoardDBAccess::checkDBExist()
{
    if (mSqlitedb == NULL)
        return false;
    if (!tableExists("DeviceActivity"))
        return false;
    if (!tableExists("AreaRollCall"))
        return false;
    if (!tableExists("OperationMode"))
        return false;
    if (!tableExists("NodeMoveOffsetInfo"))
        return false;

    int count = getTableDataCount("DeviceActivity");

    vilslog::printf("AnalyzeDashBoardDBAccess::checkDBExist() database[%s] count[%d]\n", mCacheDBName, count);

    if (count == 0)
        return false;
    else
        return true;
}

bool AnalyzeDashBoardDBAccess::checkDBExist(int flag)
{
    int rc;
    rc = sqlite3_open_v2(mCacheDBName, &mSqlitedb, flag, NULL);
    if( rc != SQLITE_OK )
    {
        vilslog::printf("AnalyzeDashBoardDBAccess::checkDBExist() Can't open database[%s] errmsg[%s]\n", mCacheDBName, sqlite3_errmsg(mSqlitedb));
        return false;
    }

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    // int nResult = sqlite3_exec(mSqlitedb,"select count(*) from AreaRollCall;", NULL, NULL, NULL);
    // if (nResult == SQLITE_BUSY)
    // {
    //     printf("AnalyzeDashBoardDBAccess::checkDBExist() SQLITE_BUSY sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
    // }
    // else
    // if (nResult == SQLITE_LOCKED)
    // {
    //     printf("AnalyzeDashBoardDBAccess::checkDBExist() SQLITE_LOCKED sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
    // }
    // else
    // if (nResult != SQLITE_OK)
    // {
    //     printf("AnalyzeDashBoardDBAccess::checkDBExist() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
    //     return false;
    // }

    return true;
}

int AnalyzeDashBoardDBAccess::getTableDataCount(const char * tableName)
{
    char cmd[4096];
    sprintf(cmd, "SELECT count(*) FROM %s;", tableName);

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    int count = 0;
    int nResult = sqlite3_exec(mSqlitedb, cmd, get_count_callback, &count, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AnalyzeDashBoardDBAccess::getTableDataCount() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
    }

    //printf("AnalyzeDashBoardDBAccess::getTableDataCount() tableName[%s] count[%d]\n", tableName, count);

    return count;
}

int AnalyzeDashBoardDBAccess::create_table()
{
    create_AreaRollCall_table();
    create_DeviceActivity_table();
    create_OperationMode_table();
    create_NodeMoveOffsetInfo_table();

    return 0;
}

bool AnalyzeDashBoardDBAccess::check_DeviceActivity_table()
{
    if (tableExists("DeviceActivity"))
    {
        //create_AreaRollCall_table();
        // bool bFound = check_column_Exists("DeviceActivity", "datetime", "DATETIME DEFAULT CURRENT_TIMESTAMP");
        // vilslog::printf("check_DeviceActivity_table() DeviceActivity datetime bFound[%d]\n", bFound);
    }
    else
    {
        // vilslog::printf("check_DeviceActivity_table() DeviceActivity NOT existed!\n");
        create_DeviceActivity_table();
    }
    return true;
}

bool AnalyzeDashBoardDBAccess::create_DeviceActivity_table()
{
    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    std::string sql;
    int nResult;

    sql = "CREATE TABLE DeviceActivity( \
          nodetype INT(3) NOT NULL DEFAULT 0, \
          nodeid CHAR(128) NOT NULL, \
          alive INT(3) NOT NULL DEFAULT 0, \
          issueday CHAR(30) NOT NULL, \
          issuetime CHAR(30) NOT NULL, \
          datetime DATETIME DEFAULT CURRENT_TIMESTAMP, \
          PRIMARY KEY (nodeid, issueday, issuetime));";
    nResult = sqlite3_exec(mSqlitedb, sql.c_str(), NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AnalyzeDashBoardDBAccess::create_DeviceActivity_table() AreaRollCall sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
    }

    return true;
}

bool AnalyzeDashBoardDBAccess::check_AreaRollCall_table()
{
    if (tableExists("AreaRollCall"))
    {
        //create_AreaRollCall_table();
    }
    else
    {
        //vilslog::printf("check_AreaRollCall_table() AreaRollCall NOT existed!\n");
        create_AreaRollCall_table();
    }
    return true;
}

bool AnalyzeDashBoardDBAccess::create_AreaRollCall_table()
{
    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    std::string sql;
    int nResult;

    sql = "CREATE TABLE AreaRollCall( \
          areaid CHAR(128) NOT NULL, \
          nodeid CHAR(128) NOT NULL, \
          issueday CHAR(30) NOT NULL, \
          issuetime CHAR(30) NOT NULL, \
          totaltime INT(3) NOT NULL DEFAULT 0, \
          datetime DATETIME DEFAULT CURRENT_TIMESTAMP, \
          PRIMARY KEY (areaid, nodeid, issueday, issuetime));";
    nResult = sqlite3_exec(mSqlitedb, sql.c_str(), NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AnalyzeDashBoardDBAccess::create_table() AreaRollCall sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
    }

    return true;
}

bool AnalyzeDashBoardDBAccess::check_OperationMode_table()
{
    if (tableExists("OperationMode"))
    {
        //create_OperationMode_table();
    }
    else
    {
        //vilslog::printf("check_OperationMode_table() OperationMode NOT existed!\n");
        create_OperationMode_table();
    }
    return true;
}

bool AnalyzeDashBoardDBAccess::create_OperationMode_table()
{
    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    std::string sql;
    int nResult;

    sql = "CREATE TABLE OperationMode( \
          mode INT(3) NOT NULL DEFAULT 0, \
          nodeid CHAR(128) NOT NULL, \
          issueday CHAR(30) NOT NULL, \
          issuetime CHAR(30) NOT NULL, \
          totaltime INT(3) NOT NULL DEFAULT 0, \
          datetime DATETIME DEFAULT CURRENT_TIMESTAMP, \
          PRIMARY KEY (mode, nodeid, issueday, issuetime));";
    nResult = sqlite3_exec(mSqlitedb, sql.c_str(), NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AnalyzeDashBoardDBAccess::create_table() OperationMode sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
    }

    return true;
}

bool AnalyzeDashBoardDBAccess::check_NodeMoveOffsetInfo_table()
{
    if (tableExists("NodeMoveOffsetInfo"))
    {
        //create_OperationModeActivityCount_table();
    }
    else
    {
        //vilslog::printf("check_NodeMoveOffsetInfo_table() NodeMoveOffsetInfo NOT existed!\n");
        create_NodeMoveOffsetInfo_table();
    }
    return true;
}

bool AnalyzeDashBoardDBAccess::create_NodeMoveOffsetInfo_table()
{
    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    std::string sql;
    int nResult;

    sql = "CREATE TABLE NodeMoveOffsetInfo( \
          nodetype INT(3) NOT NULL DEFAULT 0, \
          nodeid CHAR(30) NOT NULL, \
          moveoffset INT(8) NOT NULL DEFAULT 0, \
          issueday CHAR(30) NOT NULL, \
          issuetime CHAR(30) NOT NULL, \
          datetime DATETIME DEFAULT CURRENT_TIMESTAMP, \
          PRIMARY KEY (nodetype, nodeid, issueday, issuetime));";
    nResult = sqlite3_exec(mSqlitedb, sql.c_str(), NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AnalyzeDashBoardDBAccess::create_table() NodeMoveOffsetInfo sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
    }

    return true;
}

bool AnalyzeDashBoardDBAccess::write_deviceactivity_record(DeviceActivityRecordStruct * record)
{
    char cmd[4096];

    std::string datetime = currentDateTimeForSQL();

    sprintf(cmd, "INSERT OR IGNORE INTO DeviceActivity(nodeid, nodetype, alive, issueday, issuetime, datetime) \
                    VALUES ('%s', %d, %d, '%s', '%s', '%s'); \
                    UPDATE DeviceActivity SET nodetype=%d, alive=%d, datetime='%s' WHERE \
                    nodeid='%s' AND issueday='%s' AND issuetime='%s';",
                    record->nodeid.c_str(), record->nodetype, record->alive, record->issueday.c_str(), record->issuetime.c_str(), datetime.c_str(),
                    record->nodetype, record->alive, datetime.c_str(), record->nodeid.c_str(), record->issueday.c_str(), record->issuetime.c_str());

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    // vilslog::printf("AnalyzeDashBoardDBAccess::write_deviceactivity_record() cmd[%s]\n", cmd);

    int nResult = sqlite3_exec(mSqlitedb, cmd, NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AnalyzeDashBoardDBAccess::write_deviceactivity_record() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
        return false;
    }

    return true;
}

bool AnalyzeDashBoardDBAccess::write_deviceactivity_record_full(DeviceActivityRecordStruct * record)
{
    char cmd[4096];

    std::string datetime = record->issueday;
    datetime.append(" ");
    datetime.append(record->issuetime);
    datetime.append(":00");

    sprintf(cmd, "INSERT OR IGNORE INTO DeviceActivity(nodeid, nodetype, alive, issueday, issuetime, datetime) \
                    VALUES ('%s', %d, %d, '%s', '%s', '%s'); \
                    UPDATE DeviceActivity SET nodetype=%d, alive=%d, datetime='%s' WHERE \
                    nodeid='%s' AND issueday='%s' AND issuetime='%s';",
                    record->nodeid.c_str(), record->nodetype, record->alive, record->issueday.c_str(), record->issuetime.c_str(), datetime.c_str(),
                    record->nodetype, record->alive, datetime.c_str(), record->nodeid.c_str(), record->issueday.c_str(), record->issuetime.c_str());

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    // vilslog::printf("AnalyzeDashBoardDBAccess::write_deviceactivity_record_full() cmd[%s]\n", cmd);

    int nResult = sqlite3_exec(mSqlitedb, cmd, NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AnalyzeDashBoardDBAccess::write_deviceactivity_record() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
        return false;
    }

    return true;
}

bool AnalyzeDashBoardDBAccess::write_arearollcall_record(AreaRollCallRecordStruct * record)
{
    char cmd[4096];

    sprintf(cmd, "INSERT OR IGNORE INTO AreaRollCall(areaid, nodeid, issueday, issuetime, totaltime, datetime) \
                    VALUES ('%s', '%s', '%s', '%s', %d, '%s'); \
                    UPDATE AreaRollCall SET totaltime=%d, datetime='%s' WHERE \
                    areaid='%s' AND nodeid='%s' AND issueday='%s' AND issuetime='%s';",
                    record->areaid.c_str(), record->nodeid.c_str(), record->issueday.c_str(), record->issuetime.c_str(), record->totaltime,
                    record->datetime.c_str(), record->totaltime, record->datetime.c_str(), record->areaid.c_str(), record->nodeid.c_str(),
                    record->issueday.c_str(), record->issuetime.c_str());

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    // if (record->areaid.compare("area1718") == 0)
    // {
    //     vilslog::printf("AnalyzeDashBoardDBAccess::write_arearollcall_record() cmd[%s]\n", cmd);
    // }

    int nResult = sqlite3_exec(mSqlitedb, cmd, NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AnalyzeDashBoardDBAccess::write_arearollcall_record() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
        return false;
    }

    return true;
}

bool AnalyzeDashBoardDBAccess::write_arearollcall_record_full(AreaRollCallRecordStruct * record)
{
    char cmd[4096];

    sprintf(cmd, "INSERT OR IGNORE INTO AreaRollCall(areaid, nodeid, issueday, issuetime, totaltime, datetime) \
                    VALUES ('%s', '%s', '%s', '%s', %d, '%s');",
                    record->areaid.c_str(), record->nodeid.c_str(), record->issueday.c_str(),
                    record->issuetime.c_str(), record->totaltime, record->datetime.c_str());

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    // vilslog::printf("AnalyzeDashBoardDBAccess::write_arearollcall_record() cmd[%s]\n", cmd);

    int nResult = sqlite3_exec(mSqlitedb, cmd, NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AnalyzeDashBoardDBAccess::write_arearollcall_record_full() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
        return false;
    }

    return true;
}

bool AnalyzeDashBoardDBAccess::write_operationmode_record(OperationModeRecordStruct * record)
{
    char cmd[4096];

    std::string datetime = currentDateTimeForSQL();

    sprintf(cmd, "INSERT OR IGNORE INTO OperationMode(mode, nodeid, issueday, issuetime, totaltime, datetime) \
                    VALUES (%d, '%s', '%s', '%s', %d, '%s'); \
                    UPDATE OperationMode SET totaltime=%d, datetime='%s' WHERE \
                    mode=%d AND nodeid='%s' AND issueday='%s' AND issuetime='%s';",
                    record->mode, record->nodeid.c_str(), record->issueday.c_str(), record->issuetime.c_str(), record->totaltime, datetime.c_str(),
                    record->totaltime, datetime.c_str(), record->mode, record->nodeid.c_str(), record->issueday.c_str(), record->issuetime.c_str());

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    // vilslog::printf("AnalyzeDashBoardDBAccess::write_operationmode_record() cmd[%s]\n", cmd);

    int nResult = sqlite3_exec(mSqlitedb, cmd, NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AnalyzeDashBoardDBAccess::write_operationmode_record() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
        return false;
    }

    return true;
}

bool AnalyzeDashBoardDBAccess::write_operationmode_record_full(OperationModeRecordStruct * record)
{
    char cmd[4096];

    sprintf(cmd, "INSERT OR IGNORE INTO OperationMode(mode, nodeid, issueday, issuetime, totaltime, datetime) \
                    VALUES (%d, '%s', '%s', '%s', %d, '%s');",
                    record->mode, record->nodeid.c_str(), record->issueday.c_str(),
                    record->issuetime.c_str(), record->totaltime, record->datetime.c_str());

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    // vilslog::printf("AnalyzeDashBoardDBAccess::write_operationmode_record_full() cmd[%s]\n", cmd);

    int nResult = sqlite3_exec(mSqlitedb, cmd, NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AnalyzeDashBoardDBAccess::write_operationmode_record_full() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
        return false;
    }

    return true;
}

bool AnalyzeDashBoardDBAccess::write_TagMoveOffsetInfo_record(int nodetype, std::string nodeid, int moveoffset)
{
    char cmd[4096];

    std::string issueday = TodayDateType1();
    std::string issuetime = currentTimeForSQL();
    std::string datetime = currentDateTimeForSQL();

    sprintf(cmd, "INSERT OR IGNORE INTO NodeMoveOffsetInfo(nodetype, nodeid, moveoffset, issueday, issuetime, datetime) \
                    VALUES (%d, '%s', %d, '%s', '%s', '%s'); \
                    UPDATE NodeMoveOffsetInfo SET moveoffset=%d, datetime='%s' WHERE \
                    nodetype=%d AND nodeid='%s' AND issueday='%s' AND issuetime='%s';",
                    nodetype, nodeid.c_str(), moveoffset, issueday.c_str(), issuetime.c_str(), datetime.c_str(),
                    moveoffset, datetime.c_str(), nodetype, nodeid.c_str(), issueday.c_str(), issuetime.c_str());

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    // vilslog::printf("AnalyzeDashBoardDBAccess::write_TagMoveOffsetInfo_record() cmd[%s]\n", cmd);

    int nResult = sqlite3_exec(mSqlitedb, cmd, NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AnalyzeDashBoardDBAccess::write_TagMoveOffsetInfo_record() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
        return false;
    }

    return true;
}

bool AnalyzeDashBoardDBAccess::write_TagMoveOffset_record_full(TagMoveOffsetRecordStruct * record)
{
    char cmd[4096];

    std::string issueday = TodayDateType1();
    std::string issuetime = currentTimeForSQL();

    sprintf(cmd, "INSERT OR IGNORE INTO NodeMoveOffsetInfo(nodetype, nodeid, moveoffset, issueday, issuetime, datetime) \
                    VALUES (%d, '%s', %d, '%s', '%s', '%s'); ",
                    record->nodetype, record->nodeid.c_str(), record->moveoffset,
                    record->issueday.c_str(), record->issuetime.c_str(), record->datetime.c_str());

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    // vilslog::printf("AnalyzeDashBoardDBAccess::write_TagMoveOffset_record_full() cmd[%s]\n", cmd);

    int nResult = sqlite3_exec(mSqlitedb, cmd, NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AnalyzeDashBoardDBAccess::write_TagMoveOffset_record_full() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
        return false;
    }

    return true;
}

std::list<std::string> * AnalyzeDashBoardDBAccess::getDeviceActivityNodeIdList(int nodetype, std::string target_date, std::string issuetime)
{
    char cmd[4096];
    std::list<std::string> * NodeIDList = new std::list<std::string>();

    sprintf(cmd, "SELECT nodeid as stringvalue FROM DeviceActivity \
                  WHERE nodetype=%d AND issueday='%s' AND issuetime='%s' ",
                  nodetype, target_date.c_str(), issuetime.c_str());

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    // printf("AnalyzeDashBoardDBAccess::getDeviceActivityNodeIdList() cmd[%s]\n", cmd);

    int nResult = sqlite3_exec(mSqlitedb, cmd, get_StringValueList_callback, NodeIDList, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AnalyzeDashBoardDBAccess::getDeviceActivityNodeIdList() sqlite3_errmsg[%s]\n", sqlite3_errmsg(mSqlitedb));
    }

    return NodeIDList;
}

std::list<AreaActivityTimeStruct*> * AnalyzeDashBoardDBAccess::getAreaActivityTime(std::string start_datetime, std::string end_datetime)
{
    std::list<AreaActivityTimeStruct*> * recordList = new std::list<AreaActivityTimeStruct*>();

    char cmd[4096];
    sprintf(cmd, "SELECT areaid, min(datetime) as min_time, max(datetime) as max_time, sum(totaltime) as totaltime from AreaRollCall \
                  WHERE datetime BETWEEN '%s' AND '%s' GROUP BY areaid ",
                  start_datetime.c_str(), end_datetime.c_str());

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    int nResult = sqlite3_exec(mSqlitedb, cmd, get_AreaActivityTimeList_callback, recordList, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AnalyzeDashBoardDBAccess::getAreaActivityTime() sqlite3_errmsg[%s]\n", sqlite3_errmsg(mSqlitedb));
    }

    return recordList;
}

std::list<std::string> * AnalyzeDashBoardDBAccess::getAreaActivityUser(std::string areaid, std::string start_datetime, std::string end_datetime)
{
    std::list<std::string> * recordList = new std::list<std::string>();

    char cmd[4096];
    sprintf(cmd, "SELECT nodeid from AreaRollCall WHERE areaid='%s' AND datetime BETWEEN '%s' AND '%s' ",
                  areaid.c_str(), start_datetime.c_str(), end_datetime.c_str());

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    int nResult = sqlite3_exec(mSqlitedb, cmd, get_AreaActivityUserList_callback, recordList, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AnalyzeDashBoardDBAccess::getAreaActivityUser() sqlite3_errmsg[%s]\n", sqlite3_errmsg(mSqlitedb));
    }

    return recordList;
}

std::list<OperationModeActivityStruct *> * AnalyzeDashBoardDBAccess::getOperationModeActivityList(std::string start_datetime, std::string end_datetime)
{
    char cmd[4096];
    sprintf(cmd, "SELECT * from OperationMode WHERE datetime BETWEEN '%s' AND '%s' ",
            start_datetime.c_str(), end_datetime.c_str());

    std::list<OperationModeActivityStruct *> * operationModeList = new std::list<OperationModeActivityStruct *>();

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    int nResult = sqlite3_exec(mSqlitedb, cmd, get_OperationModeActivityList_callback, operationModeList, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AnalyzeDashBoardDBAccess::getOperationModeActivityList() sqlite3_errmsg[%s]\n", sqlite3_errmsg(mSqlitedb));
    }

    return operationModeList;
}

std::list<NodeMoveOffsetInfoStruct *> * AnalyzeDashBoardDBAccess::getNodeMoveOffset(std::string start_datetime, std::string end_datetime)
{
    char cmd[4096];
    sprintf(cmd, "SELECT nodeid, count(moveoffset) as minutes, sum(moveoffset) as moveoffset from NodeMoveOffsetInfo \
                  WHERE datetime BETWEEN '%s' AND '%s' GROUP BY nodeid ",
                  start_datetime.c_str(), end_datetime.c_str());

    std::list<NodeMoveOffsetInfoStruct *> * infoList = new std::list<NodeMoveOffsetInfoStruct *>();

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    int nResult = sqlite3_exec(mSqlitedb, cmd, get_NodeMoveOffsetInfolist_callback, infoList, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AnalyzeDashBoardDBAccess::getNodeMoveOffset() sqlite3_errmsg[%s]\n", sqlite3_errmsg(mSqlitedb));
    }

    return infoList;
}

std::list<std::string> * AnalyzeDashBoardDBAccess::get_column_list(const char * table_name)
{
    char cmd[4096];
    sprintf(cmd, "PRAGMA table_info (%s);", table_name);

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    std::list<std::string> * colList = new std::list<std::string>();
    int nResult = sqlite3_exec(mSqlitedb, cmd, check_column_callback, colList, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AnalyzeDashBoardDBAccess::get_column_list() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
    }
    return colList;
}

bool AnalyzeDashBoardDBAccess::create_column(const char * tableName, const char * colName, const char * colDefinition)
{
    char cmd[4096];
    //sprintf(cmd, "SELECT COUNT(*) AS CNTREC FROM pragma_table_info(webconfig) WHERE name='enable_3dmap';");
    sprintf(cmd, "ALTER TABLE %s ADD %s %s;", tableName, colName, colDefinition);

    sqlite3_busy_handler(mSqlitedb, callback_busy_handler, (void *)mSqlitedb);

    int nResult = sqlite3_exec(mSqlitedb, cmd, NULL, NULL, NULL);
    if (nResult != SQLITE_OK)
    {
        printf("AnalyzeDashBoardDBAccess::create_column() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
    }

    return true;
}

bool AnalyzeDashBoardDBAccess::check_column_Exists(const char * tableName, const char * colName, const char * colDefinition)
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
        printf("AnalyzeDashBoardDBAccess::check_column_Exists() tableName[%s] colName[%s] not found!!!\n", tableName, colName);
        create_column(tableName, colName, colDefinition);
    }

    delete colList;
    return bFound;
}

bool AnalyzeDashBoardDBAccess::tableExists(const char * tableName)
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
        printf("AnalyzeDashBoardDBAccess::tableExists() sqlite3_errmsg:%s\n", sqlite3_errmsg(mSqlitedb));
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

int AnalyzeDashBoardDBAccess::check_column_callback(void* pcolList, int nCount,char** pValue,char** pName)
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

int AnalyzeDashBoardDBAccess::get_count_callback(void* nret,int nCount,char** pValue,char** pName)
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

int AnalyzeDashBoardDBAccess::get_StringValueList_callback(void* pList,int nCount,char** pValue,char** pName)
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

int AnalyzeDashBoardDBAccess::get_AreaActivityTimeList_callback(void* pList, int nCount,char** pValue,char** pName)
{
    std::list<AreaActivityTimeStruct*> * records = (std::list<AreaActivityTimeStruct*> *)pList;

    AreaActivityTimeStruct * record = new AreaActivityTimeStruct();
    record->areaname = "";

    for(int i=0;i<nCount;i++)
    {
        std::string s = pName[i];

        if ( s.compare("totaltime") == 0 )
            record->totaltime = atoi(pValue[i]);
        else
        if ( s.compare("areaid") == 0 )
            record->areaid = pValue[i];

        // if (record->areaid.compare("area1718") == 0)
        // {
        //     std::string min_time;
        //     if ( s.compare("min_time") == 0 )
        //     {
        //         min_time = pValue[i];
        //         if (min_time.length() > 0)
        //         {
        //             vilslog::printf("AnalyzeDashBoardService::prepair_day_record() record->areaid[%s] totaltime[%d] min_time[%s]\n",
        //                 record->areaid.c_str(), record->totaltime, min_time.c_str());
        //         }
        //     }
        //     std::string max_time;
        //     if ( s.compare("max_time") == 0 )
        //     {
        //         max_time = pValue[i];
        //         if (max_time.length() > 0)
        //         {
        //             vilslog::printf("AnalyzeDashBoardService::prepair_day_record() record->areaid[%s] totaltime[%d] max_time[%s]\n",
        //                 record->areaid.c_str(), record->totaltime, max_time.c_str());
        //         }
        //     }
        // }
    }
    records->push_back(record);

    return 0;
}

int AnalyzeDashBoardDBAccess::get_AreaActivityUserList_callback(void* pList, int nCount,char** pValue,char** pName)
{
    std::list<std::string> * records = (std::list<std::string> *)pList;

    for(int i=0;i<nCount;i++)
    {
        std::string s = pName[i];

        if ( s.compare("nodeid") == 0 )
        {
            std::string nodeid = pValue[i];

            bool bfound = false;
            for (std::list<std::string>::iterator i = records->begin(); i != records->end(); i++)
            {
                if (nodeid.compare((*i)) == 0)
                {
                    bfound = true;
                }
            }
            if (!bfound)
            {
                records->push_back(nodeid);
            }
        }
    }

    return 0;
}

int AnalyzeDashBoardDBAccess::get_OperationModeActivityList_callback(void* pList, int nCount,char** pValue,char** pName)
{
    std::list<OperationModeActivityStruct *> * operationModeList = (std::list<OperationModeActivityStruct *> *)pList;

    OperationModeActivityStruct operationMode;
    for(int i=0;i<nCount;i++)
    {
        std::string s = pName[i];

        if ( s.compare("mode") == 0 )
        {
            operationMode.mode = atoi(pValue[i]);
        }
        else
        if ( s.compare("totaltime") == 0 )
        {
            operationMode.totaltime = atoi(pValue[i]);
        }
        else
        if ( s.compare("nodeid") == 0 )
        {
            operationMode.nodeidList.push_back(pValue[i]);
            operationMode.count = (int)operationMode.nodeidList.size();
        }
    }

    OperationModeActivityStruct * operationModeExisted = NULL;
    for (std::list<OperationModeActivityStruct *>::iterator ptoperationMode = operationModeList->begin(); ptoperationMode != operationModeList->end(); ptoperationMode++)
    {
        if ( (*ptoperationMode)->mode == operationMode.mode )
        {
            operationModeExisted = (*ptoperationMode);
            break;
        }
    }

    if ( operationModeExisted == NULL )
    {
        operationModeExisted = new OperationModeActivityStruct();
        operationModeExisted->mode = operationMode.mode;
        operationModeExisted->totaltime = operationMode.totaltime;
        operationModeExisted->count = operationMode.count;
        for (std::list<std::string>::iterator id = operationMode.nodeidList.begin(); id != operationMode.nodeidList.end(); id++)
        {
            operationModeExisted->nodeidList.push_back((*id));
        }
        operationModeList->push_back(operationModeExisted);
    }
    else
    {
        operationModeExisted->totaltime += operationMode.totaltime;

        for (std::list<std::string>::iterator id = operationMode.nodeidList.begin(); id != operationMode.nodeidList.end(); id++)
        {
            bool bFound = false;
            for (std::list<std::string>::iterator idExisted = operationModeExisted->nodeidList.begin(); idExisted != operationModeExisted->nodeidList.end(); idExisted++)
            {
                if ((*idExisted).compare((*id)) == 0)
                {
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
            {
                operationModeExisted->nodeidList.push_back((*id));
            }
        }
        operationModeExisted->count = (int)operationModeExisted->nodeidList.size();
    }

    return 0;
}

int AnalyzeDashBoardDBAccess::get_NodeMoveOffsetInfolist_callback(void* pList, int nCount,char** pValue,char** pName)
{
    std::list<NodeMoveOffsetInfoStruct *> * infoList = (std::list<NodeMoveOffsetInfoStruct *> *)pList;

    NodeMoveOffsetInfoStruct * info = new NodeMoveOffsetInfoStruct();
    for(int i=0;i<nCount;i++)
    {
        std::string s = pName[i];

        if ( s.compare("nodeid") == 0 )
        {
            info->nodeid = pValue[i];
        }
        else
        if ( s.compare("minutes") == 0 )
        {
            info->minutes = atoi(pValue[i]);
        }
        else
        if ( s.compare("moveoffset") == 0 )
        {
            info->moveoffset = atoi(pValue[i]);
        }
    }
    infoList->push_back(info);

    return 0;
}

int AnalyzeDashBoardDBAccess::callback_busy_handler(void *ptr,int count)
{
    //usleep(500000);   //如果获取不到锁，等待0.5秒
    usleep(5000);   //如果获取不到锁，等待 5ms
    //printf("database is locak now,can not write/read.\n");  //每次执行一次回调函数打印一次该信息
    return 1;    //回调函数返回值为1，则将不断尝试操作数据库。
}
