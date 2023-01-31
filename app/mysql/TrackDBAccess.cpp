/*
 * TrackDBAccess.cpp
 *
 *  Created on: 2019/07/22
 *      Author: ikki
 */


#include <stdlib.h>
#include <iostream>
#include <sstream>

#include "TrackDBAccess.h"
#include "../json/json.h"
#include "../common.h"
using namespace std;

TrackDBAccess::TrackDBAccess(int projectID)
{
    mProjectID = projectID;
    bEnableTransaction = true;
    init();
}

TrackDBAccess::~TrackDBAccess()
{
}

int TrackDBAccess::init()
{
    mMySQLDriver = get_driver_instance();

    if (mProjectID == 1)
    {
        mSQLDBname = tracksqldbname;
    }
    else
    {
        mSQLDBname = tracksqldbname;
        mSQLDBname.append("_");
        mSQLDBname.append(std::to_string(mProjectID));
    }

    if (!checkDBExist())
    {
        vilslog::printf("TrackDBAccess::init() database %s not exist!\n", mSQLDBname.c_str());
        create_DB();
        check_valid_table();
    }

    static int flag = 0;
    if (flag == 0)
    {
        char cmd[4096];
        memset(cmd, 0, 4096);

        //create_table();

        flag = 1;
    }

    return 0;
}

int TrackDBAccess::check_valid_table()
{
    check_AnalyzeTrackRecord_table();
    check_DayTrack_table();
    check_DeviceActivity_table();
    check_AreaRollCall_table();
    check_OperationMode_table();
    check_DeviceActivityAliveCount_table();
    check_AreaActivityUserCount_table();
    check_OperationModeActivityCount_table();
    check_NodeMoveOffsetInfo_table();
    check_NodeMoveOffsetRecord_table();

    return 0;
}

int TrackDBAccess::deletedb()
{
    char cmd[4096];
    sprintf(cmd, "DROP DATABASE IF EXISTS %s", mSQLDBname.c_str());
    sql_execute(cmd);
    return 0;
}

int TrackDBAccess::getProjectID()
{
    return mProjectID;
}

int TrackDBAccess::Transaction_commit()
{
    int ret = 0;
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());
        stmt->execute("START TRANSACTION");

        while (mMysqlTransactionQueue.size() > 0)
        {
            std::string trans = mMysqlTransactionQueue.remove();
            stmt->execute(trans.c_str());
        }
        stmt->execute("COMMIT");

        stmt->close();
    }
    catch (sql::SQLException &e) {
        vilslog::printf("TrackDBAccess::Transaction_commit() sql::SQLException\n");

        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;

        ret = -1;
    }

    return ret;

}

bool TrackDBAccess::clear_old_data(const char * table, const char * column, const char * datetime)
{
    char cmd[4096];

    sprintf(cmd, "DELETE FROM %s WHERE %s <= '%s';",
        table, column, datetime);

    int ret = sql_execute(cmd);

    vilslog::printf("TrackDBAccess::clear_old_data() cmd[%s] ret[%d]\n", cmd, ret);

    if (ret == 0)
        return true;
    else
        return false;
}

DeviceActivityAliveCountStruct * TrackDBAccess::loadactivity(int nodetype)
{
    char cmd[4096];
    DeviceActivityAliveCountStruct * data = new DeviceActivityAliveCountStruct();

    sprintf(cmd, "SELECT issuetime, issueday, devicecount, aliveString, datetime \
                  FROM DeviceActivityAliveCount  \
                  WHERE nodetype=%d ORDER BY datetime DESC limit 1 ", nodetype);

    sql_executeQuery(cmd, get_DeviceActivityAliveCount_callback, data);

    return data;
}

std::list<DeviceActivityAliveCountStruct *> * TrackDBAccess::loadactivitys(int nodetype, std::string issueday, int count)
{
    char cmd[4096];
    std::list<DeviceActivityAliveCountStruct *> * dataList = new std::list<DeviceActivityAliveCountStruct *>();

    sprintf(cmd, "SELECT issuetime, issueday, devicecount, aliveString, datetime \
                  FROM DeviceActivityAliveCount  \
                  WHERE nodetype=%d and issueday='%s' ORDER BY datetime DESC limit %d ", nodetype, issueday.c_str(), count);

    sql_executeQuery(cmd, get_DeviceActivityAliveCountList_callback, dataList);

    return dataList;
}

std::list<std::string> * TrackDBAccess::getAnalyzedDate(const char * nodeid, TrackDataType datatype, std::string StartDate)
{
    std::list<std::string> * analyzedList = new std::list<std::string>();
    char cmd[4096];
    sprintf(cmd, "SELECT * FROM AnalyzeTrackRecord WHERE nodeid='%s' AND datatype=%d AND analyzeddone=1 AND analyzeddate >='%s' ORDER BY datetime ASC",
        nodeid, datatype, StartDate.c_str());

    //vilslog::printf(cmd);

    sql_executeQuery(cmd, get_AnalyzedList_callback, analyzedList);

    return analyzedList;
}

std::list<DayRecordStruct*> * TrackDBAccess::getDeviceActivityCount(int nodetype, std::string target_date, int limit)
{
    std::list<DayRecordStruct*> * recordList = new std::list<DayRecordStruct*>();

    char cmd[4096];
    sprintf(cmd, "SELECT issuetime, count(nodeid) AS user_cnt FROM DeviceActivity \
                  WHERE nodetype=%d AND issueday='%s' GROUP BY issuetime ORDER BY issuetime DESC limit %d ",
                    nodetype, target_date.c_str(), limit);

    sql_executeQuery(cmd, get_DeviceActivityCountList_callback, recordList);

    return recordList;
}

int TrackDBAccess::getDeviceActivityCount(int nodetype, std::string target_date, std::string issuetime)
{
    //std::list<DayRecordStruct*> * recordList = new std::list<DayRecordStruct*>();

    char cmd[4096];
    std::list<std::string> * NodeIDList = new std::list<std::string>();

    sprintf(cmd, "SELECT nodeid as stringvalue FROM DeviceActivity \
                  WHERE nodetype=%d AND issueday='%s' AND issuetime='%s' ",
                    nodetype, target_date.c_str(), issuetime.c_str());

    int count = 0;
    //sql_executeQuery(cmd, get_count_callback, &count);
    bool bSuccess = sql_executeQuery(cmd, get_StringValueList_callback, NodeIDList);

    if(bSuccess)
        count = (int)NodeIDList->size();

    NodeIDList->clear();
    delete NodeIDList;

    //vilslog::printf(cmd);
    //vilslog::printf("TrackDBAccess::getDeviceActivityCount() count[%d]\n", count);

    return count;
}

std::list<DeviceActivityRecordStruct*> * TrackDBAccess::getDeviceActivityRecordList(int nodetype, std::string target_date, int limit)
{
    std::list<DeviceActivityRecordStruct*> * recordList = new std::list<DeviceActivityRecordStruct*>();

    char cmd[4096];
    sprintf(cmd, "SELECT * FROM DeviceActivity \
                  WHERE nodetype=%d AND issueday='%s' ORDER BY issuetime DESC limit %d ",
                    nodetype, target_date.c_str(), limit);

    sql_executeQuery(cmd, get_DeviceActivityRecordList_callback, recordList);

    return recordList;
}

std::list<std::string> * TrackDBAccess::getDeviceActivityNodeIdList(int nodetype, std::string target_date, std::string issuetime)
{
    char cmd[4096];
    std::list<std::string> * NodeIDList = new std::list<std::string>();

    sprintf(cmd, "SELECT nodeid as stringvalue FROM DeviceActivity \
                  WHERE nodetype=%d AND issueday='%s' AND issuetime='%s' ",
                  nodetype, target_date.c_str(), issuetime.c_str());

    sql_executeQuery(cmd, get_StringValueList_callback, NodeIDList);

    //vilslog::printf("TrackDBAccess::getDeviceActivityNodeIdList() bSuccess[%d]\n", bSuccess);

    return NodeIDList;
}

int TrackDBAccess::getDeviceActivityAliveCount(int nodetype, std::string target_date, std::string start_time, std::string end_time)
{
    std::list<DeviceAliveCountStruct*> * recordList = new std::list<DeviceAliveCountStruct*>();

    char cmd[4096];
    sprintf(cmd, "SELECT nodeid, max(alive) AS max_alive, min(alive) AS min_alive \
                  FROM DeviceActivity WHERE nodetype=%d AND issueday='%s' AND issuetime between '%s' AND '%s' GROUP BY nodeid ",
                    nodetype, target_date.c_str(), start_time.c_str(), end_time.c_str());

    sql_executeQuery(cmd, get_DeviceAliveCountList_callback, recordList);

    int count = (int)recordList->size();

    clear_DeviceAliveCountStruct_list(recordList);
    delete recordList;

    return count;
}

std::list<AreaActivityTimeStruct*> * TrackDBAccess::getAreaActivityTime(std::string start_datetime, std::string end_datetime)
{
    std::list<AreaActivityTimeStruct*> * recordList = new std::list<AreaActivityTimeStruct*>();

    char cmd[4096];
    sprintf(cmd, "SELECT areaid, max(datetime) as max_time, sum(totaltime) as totaltime from AreaRollCall \
                  WHERE datetime BETWEEN '%s' AND '%s' GROUP BY areaid ",
                  //WHERE datetime BETWEEN '%s' AND '%s' GROUP BY areaid ORDER BY max_time DESC ",
                  start_datetime.c_str(), end_datetime.c_str());

    sql_executeQuery(cmd, get_AreaActivityTimeList_callback, recordList);

    return recordList;
}

std::list<std::string> * TrackDBAccess::getAreaActivityUser(std::string areaid, std::string start_datetime, std::string end_datetime)
{
    std::list<std::string> * recordList = new std::list<std::string>();

    char cmd[4096];
    sprintf(cmd, "SELECT nodeid from AreaRollCall WHERE areaid='%s' AND datetime BETWEEN '%s' AND '%s' ",
                  areaid.c_str(), start_datetime.c_str(), end_datetime.c_str());

    //vilslog::printf("TrackDBAccess::getAreaActivityUser() cmd[%s]\n", cmd);

    sql_executeQuery(cmd, get_AreaActivityUserList_callback, recordList);

    return recordList;
}

std::list<AreaRollCallRecordStruct *> * TrackDBAccess::getAreaRollCallList(std::string start_datetime, std::string end_datetime)
{
    std::list<AreaRollCallRecordStruct *> * recordList = new std::list<AreaRollCallRecordStruct *>();

    char cmd[4096];
    sprintf(cmd, "SELECT * from AreaRollCall WHERE datetime BETWEEN '%s' AND '%s' ", start_datetime.c_str(), end_datetime.c_str());

    //vilslog::printf("TrackDBAccess::getAreaRollCallList() cmd[%s]\n", cmd);

    sql_executeQuery(cmd, get_AreaRollCallList_callback, recordList);

    return recordList;
}

OperationModeActivityStruct * TrackDBAccess::getOperationModeActivity(int mode, std::string start_datetime, std::string end_datetime)
{
    char cmd[4096];
    sprintf(cmd, "SELECT * from OperationMode WHERE mode=%d AND datetime BETWEEN '%s' AND '%s' ",
            mode, start_datetime.c_str(), end_datetime.c_str());

    OperationModeActivityStruct * operationMode = new OperationModeActivityStruct();
    operationMode->mode = mode;
    operationMode->totaltime = 0;
    operationMode->count = 0;
    operationMode->nodeidList.clear();
    sql_executeQuery(cmd, get_OperationModeActivity_callback, operationMode);

    operationMode->count = (int)operationMode->nodeidList.size();

    return operationMode;
}

std::list<OperationModeRecordStruct *> * TrackDBAccess::getOperationModeRecordList(std::string start_datetime, std::string end_datetime)
{
    char cmd[4096];
    sprintf(cmd, "SELECT * from OperationMode WHERE datetime BETWEEN '%s' AND '%s' ",
            start_datetime.c_str(), end_datetime.c_str());

    std::list<OperationModeRecordStruct *> * operationModeRecordList = new std::list<OperationModeRecordStruct *>();

    sql_executeQuery(cmd, get_OperationModeRecordList_callback, operationModeRecordList);

    return operationModeRecordList;
}

std::list<OperationModeActivityStruct *> * TrackDBAccess::getOperationModeActivityList(std::string start_datetime, std::string end_datetime)
{
    char cmd[4096];
    //sprintf(cmd, "SELECT * from OperationMode WHERE mode=%d AND datetime BETWEEN '%s' AND '%s' ",
    sprintf(cmd, "SELECT * from OperationMode WHERE datetime BETWEEN '%s' AND '%s' ",
            start_datetime.c_str(), end_datetime.c_str());

    std::list<OperationModeActivityStruct *> * operationModeList = new std::list<OperationModeActivityStruct *>();

    sql_executeQuery(cmd, get_OperationModeActivityList_callback, operationModeList);

    return operationModeList;
}

std::list<NodeMoveOffsetInfoStruct *> * TrackDBAccess::getNodeMoveOffset(std::string start_datetime, std::string end_datetime)
{
    char cmd[4096];
    sprintf(cmd, "SELECT nodeid, count(moveoffset) as minutes, sum(moveoffset) as moveoffset from NodeMoveOffsetInfo \
                  WHERE datetime BETWEEN '%s' AND '%s' GROUP BY nodeid ",
                  start_datetime.c_str(), end_datetime.c_str());

    std::list<NodeMoveOffsetInfoStruct *> * infoList = new std::list<NodeMoveOffsetInfoStruct *>();
    sql_executeQuery(cmd, get_NodeMoveOffsetInfolist_callback, infoList);

    return infoList;
}

std::list<TagMoveOffsetRecordStruct *> * TrackDBAccess::getTagMoveOffsetRecordList(std::string start_datetime, std::string end_datetime)
{
    char cmd[4096];
    sprintf(cmd, "SELECT * from NodeMoveOffsetInfo WHERE datetime BETWEEN '%s' AND '%s' ",
                  start_datetime.c_str(), end_datetime.c_str());

    std::list<TagMoveOffsetRecordStruct *> * infoList = new std::list<TagMoveOffsetRecordStruct *>();
    sql_executeQuery(cmd, get_TagMoveOffsetRecordlist_callback, infoList);

    return infoList;
}

std::list<CourseDayTrackRecordStruct *> * TrackDBAccess::getCourseRecordList(std::string course_id, std::string targetday)
{
    char cmd[4096];
    sprintf(cmd, "SELECT nodeid, count, max_time, min_time, total_time, datetime from DayTrack \
                  WHERE datatype=4 AND course_id='%s' AND day='%s' ORDER BY datetime DESC ",
                  course_id.c_str(), targetday.c_str());

    std::list<CourseDayTrackRecordStruct *> * infoList = new std::list<CourseDayTrackRecordStruct *>();

    // vilslog::printf("TrackDBAccess::getCourseRecordList() cmd=[%s]\n", cmd);

    sql_executeQuery(cmd, get_CourseDayTrackRecordlist_callback, infoList);

    return infoList;
}

bool TrackDBAccess::write_record(AnalyzeTrackRecordStruct * record)
{
    char cmd[4096];
    sprintf(cmd, "INSERT INTO AnalyzeTrackRecord(PROJECT_ID, nodetype, nodeid, datatype, analyzeddone, analyzeddate) \
                    VALUES (%d, %d, '%s', %d, %d, '%s') \
                    ON DUPLICATE KEY UPDATE PROJECT_ID=VALUES(PROJECT_ID), nodetype=VALUES(nodetype), \
                    analyzeddone=VALUES(analyzeddone)",
                    record->projectid, record->nodetype, record->nodeid.c_str(),
                    record->datatype, record->analyzeddone, record->analyzeddate.c_str());

    if (bEnableTransaction)
        mMysqlTransactionQueue.add(std::string(cmd));
    else
        sql_execute(cmd);
    return true;
}

bool TrackDBAccess::write_daytrack_record(TagDayTrackRecordStruct * record)
{
    char cmd[4096];
    sprintf(cmd, "INSERT INTO DayTrack(PROJECT_ID, nodeid, nodetype, datatype, day, count, max_time, min_time, total_time, area_ids, area_names, course_id, course_info) \
                    VALUES (%d, '%s', %d, %d, '%s', %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s') \
                    ON DUPLICATE KEY UPDATE PROJECT_ID=VALUES(PROJECT_ID), nodetype=VALUES(nodetype), count=VALUES(count), \
                    max_time=VALUES(max_time), min_time=VALUES(min_time), total_time=VALUES(total_time), \
                    total_time=VALUES(total_time), area_ids=VALUES(area_ids), area_names=VALUES(area_names), total_time=VALUES(total_time), course_info=VALUES(course_info)",
                    record->projectid, record->nodeid.c_str(), record->nodetype, record->datatype, record->day.c_str(),
                    record->count, record->max_time.c_str(), record->min_time.c_str(), record->total_time.c_str(),
                    record->area_ids.c_str(), record->area_names.c_str(), record->course_id.c_str(), record->course_info.c_str());

    if (bEnableTransaction)
        mMysqlTransactionQueue.add(std::string(cmd));
    else
        sql_execute(cmd);

    return true;
}

bool TrackDBAccess::write_deviceactivity_record(DeviceActivityRecordStruct * record)
{
    char cmd[4096];
    sprintf(cmd, "INSERT INTO DeviceActivity(nodeid, nodetype, alive, issueday, issuetime) \
                    VALUES ('%s', %d, %d, '%s', '%s') \
                    ON DUPLICATE KEY UPDATE nodetype=VALUES(nodetype), alive=VALUES(alive)",
                    record->nodeid.c_str(), record->nodetype, record->alive,
                    record->issueday.c_str(), record->issuetime.c_str());

    if (bEnableTransaction)
        mMysqlTransactionQueue.add(std::string(cmd));
    else
        sql_execute(cmd);

    return true;
}

bool TrackDBAccess::write_arearollcall_record(AreaRollCallRecordStruct * record)
{
    char cmd[4096];
    sprintf(cmd, "INSERT INTO AreaRollCall(projectid, areaid, nodeid, issueday, issuetime, totaltime) \
                    VALUES (%d, '%s', '%s', '%s', '%s', %d) \
                    ON DUPLICATE KEY UPDATE projectid=VALUES(projectid), totaltime=VALUES(totaltime)",
                    record->projectid, record->areaid.c_str(), record->nodeid.c_str(),
                    record->issueday.c_str(), record->issuetime.c_str(), record->totaltime);

    if (bEnableTransaction)
        mMysqlTransactionQueue.add(std::string(cmd));
    else
        sql_execute(cmd);

    return true;
}

bool TrackDBAccess::write_operationmode_record(OperationModeRecordStruct * record)
{
    char cmd[4096];
    sprintf(cmd, "INSERT INTO OperationMode(mode, nodeid, issueday, issuetime, totaltime) \
                    VALUES (%d, '%s', '%s', '%s', %d) \
                    ON DUPLICATE KEY UPDATE totaltime=VALUES(totaltime)",
                    record->mode, record->nodeid.c_str(),
                    record->issueday.c_str(), record->issuetime.c_str(), record->totaltime);

    if (bEnableTransaction)
        mMysqlTransactionQueue.add(std::string(cmd));
    else
        sql_execute(cmd);

    return true;
}

bool TrackDBAccess::write_DeviceActivityAliveCount_record(DeviceActivityAliveCountStruct * record)
{
    char cmd[4096];
    sprintf(cmd, "INSERT INTO DeviceActivityAliveCount(nodetype, devicecount, issueday, issuetime, aliveString) \
                    VALUES (%d, %d, '%s', '%s', '%s') \
                    ON DUPLICATE KEY UPDATE nodetype=VALUES(nodetype), devicecount=VALUES(devicecount), aliveString=VALUES(aliveString)",
                    record->nodetype, record->devicecount, record->issueday.c_str(), record->issuetime.c_str(), record->aliveString.c_str());

    if (bEnableTransaction)
        mMysqlTransactionQueue.add(std::string(cmd));
    else
        sql_execute(cmd);

    return true;
}

bool TrackDBAccess::write_AreaActivityUserCount_record(AreaActivityUserCountStruct * record)
{
    char cmd[4096];
    sprintf(cmd, "INSERT INTO AreaActivityUserCount(count, totaltime, areaid, areaname, issueday, issuetime, start_datetime, end_datetime) \
                    VALUES (%d, %d, '%s', '%s', '%s', '%s', '%s', '%s') \
                    ON DUPLICATE KEY UPDATE count=VALUES(count), totaltime=VALUES(totaltime), areaname=VALUES(areaname), \
                    start_datetime=VALUES(start_datetime), end_datetime=VALUES(end_datetime) ",
                    record->count, record->totaltime, record->areaid.c_str(), record->areaname.c_str(),
                    record->issueday.c_str(), record->issuetime.c_str(), record->start_datetime.c_str(), record->end_datetime.c_str());

    if (bEnableTransaction)
        mMysqlTransactionQueue.add(std::string(cmd));
    else
        sql_execute(cmd);

    return true;
}

bool TrackDBAccess::write_OperationModeActivityCount_record(OperationModeActivityStruct * record)
{
    char cmd[4096];
    sprintf(cmd, "INSERT INTO OperationModeActivityCount(mode, count, totaltime, issueday, issuetime) \
                    VALUES (%d, %d, %d, '%s', '%s') \
                    ON DUPLICATE KEY UPDATE count=VALUES(count), totaltime=VALUES(totaltime) ",
                    record->mode, record->count, record->totaltime, record->issueday.c_str(), record->issuetime.c_str());

    if (bEnableTransaction)
        mMysqlTransactionQueue.add(std::string(cmd));
    else
        sql_execute(cmd);

    return true;
}

bool TrackDBAccess::write_TagMoveOffsetInfo_record(int nodetype, std::string nodeid, int moveoffset)
{
    char cmd[4096];

    std::string issueday = TodayDateType1();
    std::string issuetime = currentTimeForSQL();

    sprintf(cmd, "INSERT INTO NodeMoveOffsetInfo(nodetype, nodeid, moveoffset, issueday, issuetime) \
                    VALUES (%d, '%s', %d, '%s', '%s') \
                    ON DUPLICATE KEY UPDATE moveoffset=VALUES(moveoffset) ",
                    nodetype, nodeid.c_str(), moveoffset, issueday.c_str(), issuetime.c_str());

    //vilslog::printf("TrackDBAccess::write_TagMoveOffsetInfo_record() cmd[%s]\n", cmd);

    if (bEnableTransaction)
        mMysqlTransactionQueue.add(std::string(cmd));
    else
        sql_execute(cmd);

    return true;
}

bool TrackDBAccess::write_NodeMoveOffsetInfo_record(NodeMoveOffsetInfoStruct * record)
{
    char cmd[4096];

    std::string issueday = TodayDateType1();
    std::string issuetime = currentTimeForSQL();

    sprintf(cmd, "INSERT INTO NodeMoveOffsetRecord(nodetype, nodeid, moveoffset, minutes, issueday, issuetime, start_datetime, end_datetime) \
                    VALUES (%d, '%s', %d, %d, '%s', '%s', '%s', '%s') \
                    ON DUPLICATE KEY UPDATE moveoffset=VALUES(moveoffset), minutes=VALUES(minutes) ",
                    record->nodetype, record->nodeid.c_str(), record->moveoffset, record->minutes,
                    record->issueday.c_str(), record->issuetime.c_str(), record->start_datetime.c_str(), record->end_datetime.c_str());

    //vilslog::printf("TrackDBAccess::write_TagMoveOffsetInfo_record() cmd[%s]\n", cmd);

    if (bEnableTransaction)
        mMysqlTransactionQueue.add(std::string(cmd));
    else
        sql_execute(cmd);


    return true;
}

bool TrackDBAccess::check_AnalyzeTrackRecord_table()
{
    if (tableExists("AnalyzeTrackRecord"))
    {
        //create_AnalyzeTrackRecord_table();
    }
    else
    {
        vilslog::printf("check_AnalyzeTrackRecord_table() AnalyzeTrackRecord NOT existed!\n");
        create_AnalyzeTrackRecord_table();
    }
    return true;
}

bool TrackDBAccess::create_AnalyzeTrackRecord_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS AnalyzeTrackRecord"); //drop if  table exists
        stmt->execute("CREATE TABLE AnalyzeTrackRecord( \
              PROJECT_ID INT(3) NOT NULL, \
              nodetype INT(3) NOT NULL, \
              nodeid CHAR(128) NOT NULL, \
              datatype INT(3) NOT NULL, \
              analyzeddone INT(3) NOT NULL, \
              analyzeddate CHAR(30) NOT NULL, \
              datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
              PRIMARY KEY (nodeid, datatype, analyzeddate)) \
              ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

    return true;
}

bool TrackDBAccess::check_DayTrack_table()
{
    if (tableExists("DayTrack"))
    {
        createNonExistColumn("DayTrack", "nodetype", "INT(3) NOT NULL DEFAULT 1");
    }
    else
    {
        vilslog::printf("check_DayTrack_table() DayTrack NOT existed!\n");
        create_DayTrack_table();
    }
    return true;
}

bool TrackDBAccess::create_DayTrack_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS DayTrack"); //drop if  table exists
        stmt->execute("CREATE TABLE DayTrack( \
              PROJECT_ID INT(3) NOT NULL, \
              nodeid CHAR(128), \
              nodetype INT(3) NOT NULL DEFAULT 1, \
              datatype INT(3), \
              day CHAR(128), \
              count INT(3) NOT NULL, \
              max_time CHAR(30) NOT NULL, \
              min_time CHAR(30) NOT NULL, \
              total_time CHAR(30) NOT NULL, \
              area_ids TEXT, \
              area_names TEXT, \
              course_id CHAR(128) NOT NULL, \
              course_info TEXT, \
              datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
              PRIMARY KEY (nodeid, datatype, day, course_id)) \
              ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

    return true;
}

bool TrackDBAccess::check_DeviceActivity_table()
{
    if (tableExists("DeviceActivity"))
    {
    }
    else
    {
        vilslog::printf("check_DeviceActivity_table() DeviceActivity NOT existed!\n");
        create_DeviceActivity_table();
    }
    return true;
}

bool TrackDBAccess::create_DeviceActivity_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS DeviceActivity"); //drop if  table exists
        stmt->execute("CREATE TABLE DeviceActivity( \
              nodetype INT(3) NOT NULL DEFAULT 0, \
              nodeid CHAR(128) NOT NULL, \
              alive INT(3) NOT NULL DEFAULT 0, \
              issueday CHAR(30) NOT NULL, \
              issuetime CHAR(30) NOT NULL, \
              PRIMARY KEY (nodeid, issueday, issuetime)) \
              ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

    return true;
}

bool TrackDBAccess::check_AreaRollCall_table()
{
    if (tableExists("AreaRollCall"))
    {
        //create_AreaRollCall_table();
    }
    else
    {
        vilslog::printf("check_AreaRollCall_table() AreaRollCall NOT existed!\n");
        create_AreaRollCall_table();
    }
    return true;
}

bool TrackDBAccess::create_AreaRollCall_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS AreaRollCall"); //drop if  table exists
        stmt->execute("CREATE TABLE AreaRollCall( \
              projectid INT(3) NOT NULL DEFAULT 0, \
              areaid CHAR(128) NOT NULL, \
              nodeid CHAR(128) NOT NULL, \
              issueday CHAR(30) NOT NULL, \
              issuetime CHAR(30) NOT NULL, \
              totaltime INT(3) NOT NULL DEFAULT 0, \
              datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
              PRIMARY KEY (areaid, nodeid, issueday, issuetime)) \
              ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

    return true;
}

bool TrackDBAccess::check_OperationMode_table()
{
    if (tableExists("OperationMode"))
    {
        //create_OperationMode_table();
    }
    else
    {
        vilslog::printf("check_OperationMode_table() OperationMode NOT existed!\n");
        create_OperationMode_table();
    }
    return true;
}

bool TrackDBAccess::create_OperationMode_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS OperationMode"); //drop if  table exists
        stmt->execute("CREATE TABLE OperationMode( \
              mode INT(3) NOT NULL DEFAULT 0, \
              nodeid CHAR(128) NOT NULL, \
              issueday CHAR(30) NOT NULL, \
              issuetime CHAR(30) NOT NULL, \
              totaltime INT(3) NOT NULL DEFAULT 0, \
              datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
              PRIMARY KEY (mode, nodeid, issueday, issuetime)) \
              ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

    return true;
}

bool TrackDBAccess::check_DeviceActivityAliveCount_table()
{
    if (tableExists("DeviceActivityAliveCount"))
    {
        //create_DeviceActivityAliveCount_table();
    }
    else
    {
        vilslog::printf("check_DeviceActivityAliveCount_table() DeviceActivityAliveCount NOT existed!\n");
        create_DeviceActivityAliveCount_table();
    }
    return true;
}

bool TrackDBAccess::create_DeviceActivityAliveCount_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS DeviceActivityAliveCount"); //drop if  table exists
        stmt->execute("CREATE TABLE DeviceActivityAliveCount( \
              devicecount INT(3) NOT NULL DEFAULT 0, \
              nodetype INT(3) NOT NULL DEFAULT 0, \
              issueday CHAR(30) NOT NULL, \
              issuetime CHAR(30) NOT NULL, \
              aliveString TEXT NOT NULL, \
              datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
              PRIMARY KEY (issueday, issuetime)) \
              ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

    return true;
}

bool TrackDBAccess::check_AreaActivityUserCount_table()
{
    if (tableExists("AreaActivityUserCount"))
    {
        //create_AreaActivityUserCount_table();
    }
    else
    {
        vilslog::printf("check_AreaActivityUserCount_table() AreaActivityUserCount NOT existed!\n");
        create_AreaActivityUserCount_table();
    }
    return true;
}

bool TrackDBAccess::create_AreaActivityUserCount_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS AreaActivityUserCount"); //drop if  table exists
        stmt->execute("CREATE TABLE AreaActivityUserCount( \
              count INT(3) NOT NULL DEFAULT 0, \
              totaltime INT(3) NOT NULL DEFAULT 0, \
              areaid CHAR(30) NOT NULL, \
              areaname CHAR(30) NOT NULL, \
              issueday CHAR(30) NOT NULL, \
              issuetime CHAR(30) NOT NULL, \
              start_datetime CHAR(30) NOT NULL, \
              end_datetime CHAR(30) NOT NULL, \
              datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
              PRIMARY KEY (areaid, issueday, issuetime)) \
              ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

    return true;
}

bool TrackDBAccess::check_OperationModeActivityCount_table()
{
    if (tableExists("OperationModeActivityCount"))
    {
        //create_OperationModeActivityCount_table();
    }
    else
    {
        vilslog::printf("check_OperationModeActivityCount_table() OperationModeActivityCount NOT existed!\n");
        create_OperationModeActivityCount_table();
    }
    return true;
}

bool TrackDBAccess::create_OperationModeActivityCount_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS OperationModeActivityCount"); //drop if  table exists
        stmt->execute("CREATE TABLE OperationModeActivityCount( \
              mode INT(3) NOT NULL DEFAULT 0, \
              count INT(3) NOT NULL DEFAULT 0, \
              totaltime INT(8) NOT NULL DEFAULT 0, \
              issueday CHAR(30) NOT NULL, \
              issuetime CHAR(30) NOT NULL, \
              datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
              PRIMARY KEY (mode, issueday, issuetime)) \
              ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

    return true;
}

bool TrackDBAccess::check_NodeMoveOffsetInfo_table()
{
    if (tableExists("NodeMoveOffsetInfo"))
    {
        //create_OperationModeActivityCount_table();
    }
    else
    {
        vilslog::printf("check_NodeMoveOffsetInfo_table() NodeMoveOffsetInfo NOT existed!\n");
        create_NodeMoveOffsetInfo_table();
    }
    return true;
}

bool TrackDBAccess::create_NodeMoveOffsetInfo_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS NodeMoveOffsetInfo"); //drop if  table exists
        stmt->execute("CREATE TABLE NodeMoveOffsetInfo( \
              nodetype INT(3) NOT NULL DEFAULT 0, \
              nodeid CHAR(30) NOT NULL, \
              moveoffset INT(8) NOT NULL DEFAULT 0, \
              issueday CHAR(30) NOT NULL, \
              issuetime CHAR(30) NOT NULL, \
              datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
              PRIMARY KEY (nodetype, nodeid, issueday, issuetime)) \
              ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

    return true;
}

bool TrackDBAccess::check_NodeMoveOffsetRecord_table()
{
    if (tableExists("NodeMoveOffsetRecord"))
    {
        //create_OperationModeActivityCount_table();
    }
    else
    {
        vilslog::printf("check_NodeMoveOffsetRecord_table() NodeMoveOffsetRecord NOT existed!\n");
        create_NodeMoveOffsetRecord_table();
    }
    return true;
}

bool TrackDBAccess::create_NodeMoveOffsetRecord_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS NodeMoveOffsetRecord"); //drop if  table exists
        stmt->execute("CREATE TABLE NodeMoveOffsetRecord( \
              nodetype INT(3) NOT NULL DEFAULT 0, \
              nodeid CHAR(30) NOT NULL, \
              moveoffset INT(8) NOT NULL DEFAULT 0, \
              minutes INT(8) NOT NULL DEFAULT 0, \
              issueday CHAR(30) NOT NULL, \
              issuetime CHAR(30) NOT NULL, \
              start_datetime CHAR(30) NOT NULL, \
              end_datetime CHAR(30) NOT NULL, \
              datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
              PRIMARY KEY (nodetype, nodeid, issueday, issuetime)) \
              ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

    return true;
}

int TrackDBAccess::sql_execute(const char * cmd)
{
    int ret = 0;
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        con->setAutoCommit(true);

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());
        stmt->execute(cmd);

        stmt->close();
    }
    catch (sql::SQLException &e) {
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;

        ret = -1;
    }

    return ret;
}

bool TrackDBAccess::sql_executeQuery(const char * cmd, CALLBACK callback, void * retValue)
{
    bool bRet = false;
    try {

        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        con->setSchema(mSQLDBname);
        con->setAutoCommit(true);
        std::unique_ptr<sql::Statement> stmt(con->createStatement());

        sql::ResultSet * res = stmt->executeQuery(cmd);

        int ret = callback(res, retValue);

        if (ret > 0)
            bRet = true;

        delete res;
        stmt->close();
    }
    catch (sql::SQLException &e) {
        vilslog::printf("TrackDBAccess::sql_executeQuery() sql::SQLException cmd[%s]\n", cmd);

        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
        return false;
    }

    return bRet;
}

bool TrackDBAccess::tableExists(const char * tableName)
{
    char cmd[4096];
    sprintf(cmd, "SELECT count(*) FROM information_schema.tables WHERE table_schema='%s' AND table_name='%s' LIMIT 1 ", mSQLDBname.c_str(), tableName);

    int count = 0;
    sql_executeQuery(cmd, get_count_callback, &count);

    if (count == 0)
        return false;
    else
        return true;

    return false;
}

bool TrackDBAccess::createNonExistColumn(const char * tableName, const char * columnName, const char * colDefinition)
{
    bool bExist = columnExists(tableName, columnName);
    if (!bExist)
        create_column(tableName, columnName, colDefinition);
    return bExist;
}

bool TrackDBAccess::columnExists(const char * tableName, const char * columnName)
{
    char cmd[4096];
    sprintf(cmd, "SELECT count(*) FROM information_schema.columns WHERE table_schema='%s' AND table_name='%s' AND column_name='%s'",
        mSQLDBname.c_str(), tableName, columnName);

    int count = 0;
    sql_executeQuery(cmd, get_count_callback, &count);

    if (count == 0)
        return false;
    else
        return true;
}

int TrackDBAccess::create_column(const char * tableName, const char * columnName, const char * colDefinition)
{
    char cmd[4096];
    sprintf(cmd, "ALTER TABLE %s ADD %s %s", tableName, columnName, colDefinition);
    int res = sql_execute(cmd);
    return res;
}

bool TrackDBAccess::checkDBExist()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());
        stmt->close();
    } catch (sql::SQLException &e) {
        cout << "checkDBExist(): # ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;

        return false;
    }

    return true;
}

int TrackDBAccess::create_DB()
{
    cout << "TrackDBAccess::create_DB() Started" << endl;

    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        // create a new database
        std::string createdb = "CREATE DATABASE ";
        createdb += mSQLDBname;
        stmt->execute(createdb.c_str());// create  new  database
        stmt->close();

    } catch (sql::SQLException &e) {
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

    cout << "TrackDBAccess::create_DB() Successfully ended" << endl;

    return 0;
}

int TrackDBAccess::get_count_callback(sql::ResultSet * res, void * pvalue)
{
   int * count = (int *)pvalue;
   if ( res != NULL)
   {
       while (res->next())
       {
           *count = res->getInt(1);
           break;
       }
   }
   //printf("MySQLDBAccess::get_count_callback() count[%d]\n", *count);

   return *count;
}

int TrackDBAccess::get_AnalyzedList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<std::string> * analyzedList = (std::list<std::string> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            // AnalyzeTrackRecordStruct * record = new AnalyzeTrackRecordStruct();
            // record->projectid = res->getInt("PROJECT_ID");
            // record->nodetype = res->getInt("nodetype");
            // record->nodeid = res->getString("nodeid");
            // record->datatype = res->getInt("datatype");
            // record->analyzeddone = res->getInt("analyzeddone");
            // record->analyzeddate = res->getString("analyzeddate");
            // record->datetime = res->getString("datetime");

            analyzedList->push_back(res->getString("analyzeddate"));
            count++;
        }
    }

    return count;
}

int TrackDBAccess::get_DeviceActivityCountList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<DayRecordStruct*> * records = (std::list<DayRecordStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            DayRecordStruct * record = new DayRecordStruct();
            record->count = res->getInt("user_cnt");
            record->issuetime = res->getString("issuetime");
            record->NodeIDList = NULL;

            records->push_back(record);
            count++;
        }
    }

    return count;
}

int TrackDBAccess::get_DeviceActivityRecordList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<DeviceActivityRecordStruct*> * records = (std::list<DeviceActivityRecordStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            DeviceActivityRecordStruct * record = new DeviceActivityRecordStruct();
            record->nodetype = res->getInt("nodetype");
            record->nodeid = res->getString("nodeid");
            record->alive = res->getInt("alive");
            record->issueday = res->getString("issueday");
            record->issuetime = res->getString("issuetime");

            records->push_back(record);
            count++;
        }
    }

    return count;
}

int TrackDBAccess::get_DeviceAliveCountList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<DeviceAliveCountStruct*> * records = (std::list<DeviceAliveCountStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            DeviceAliveCountStruct * record = new DeviceAliveCountStruct();
            record->nodeid = res->getString("nodeid");
            record->max_alive = res->getInt("max_alive");
            record->min_alive = res->getInt("min_alive");

            records->push_back(record);
            count++;
        }
    }

    return count;
}

int TrackDBAccess::get_AreaActivityTimeList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<AreaActivityTimeStruct*> * records = (std::list<AreaActivityTimeStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            AreaActivityTimeStruct * record = new AreaActivityTimeStruct();
            record->totaltime = res->getInt("totaltime");
            record->areaid = res->getString("areaid");
            record->areaname = "";

            records->push_back(record);
            count++;
        }
    }

    return count;
}

int TrackDBAccess::get_AreaActivityUserList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<std::string> * records = (std::list<std::string> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            int ret = res->findColumn("nodeid");
            if ( ret > 0 )
            {
                std::string nodeid = res->getString("nodeid");

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
                count++;
            }
        }
    }

    return count;
}

int TrackDBAccess::get_AreaRollCallList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<AreaRollCallRecordStruct*> * records = (std::list<AreaRollCallRecordStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            AreaRollCallRecordStruct * record = new AreaRollCallRecordStruct();
            record->areaid = res->getString("areaid");
            record->nodeid = res->getString("nodeid");
            record->issueday = res->getString("issueday");
            record->issuetime = res->getString("issuetime");
            record->totaltime = res->getInt("totaltime");
            record->totaltimeInMS = 0;
            record->datetime = res->getString("datetime");

            records->push_back(record);
            count++;
        }
    }

    return count;
}

int TrackDBAccess::get_OperationModeActivity_callback(sql::ResultSet * res, void * pvalue)
{
    OperationModeActivityStruct * operationMode = (OperationModeActivityStruct *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            operationMode->totaltime += res->getInt("totaltime");
            std::string nodeid = res->getString("nodeid");

            bool bFound = false;
            for (std::list<std::string >::iterator id = operationMode->nodeidList.begin(); id != operationMode->nodeidList.end(); id++)
            {
                if ((*id).compare(nodeid) == 0)
                {
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
            {
                operationMode->nodeidList.push_back(nodeid);
            }

            count++;
        }
    }

    return count;
}

int TrackDBAccess::get_OperationModeRecordList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<OperationModeRecordStruct *> * operationModeRecordList = (std::list<OperationModeRecordStruct *> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            OperationModeRecordStruct * operationMode = new OperationModeRecordStruct();
            operationMode->mode = res->getInt("mode");
            operationMode->nodeid = res->getString("nodeid");
            operationMode->issueday = res->getString("issueday");
            operationMode->issuetime = res->getString("issuetime");
            operationMode->totaltime = res->getInt("totaltime");
            operationMode->totaltimeInMS = 0;
            operationMode->datetime = res->getString("datetime");

            operationModeRecordList->push_back(operationMode);

            count++;
        }
    }

    return count;
}

int TrackDBAccess::get_OperationModeActivityList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<OperationModeActivityStruct *> * operationModeList = (std::list<OperationModeActivityStruct *> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            int mode = res->getInt("mode");

            OperationModeActivityStruct * operationMode = NULL;
            for (std::list<OperationModeActivityStruct *>::iterator ptoperationMode = operationModeList->begin(); ptoperationMode != operationModeList->end(); ptoperationMode++)
            {
                if ( (*ptoperationMode)->mode == mode )
                {
                    operationMode = (*ptoperationMode);
                    break;
                }
            }

            if ( operationMode == NULL )
            {
                operationMode = new OperationModeActivityStruct();
                operationMode->mode = mode;
                operationMode->totaltime = 0;
                operationMode->count = 0;
                operationMode->nodeidList.clear();

                operationModeList->push_back(operationMode);
            }

            operationMode->totaltime += res->getInt("totaltime");
            std::string nodeid = res->getString("nodeid");

            bool bFound = false;
            for (std::list<std::string >::iterator id = operationMode->nodeidList.begin(); id != operationMode->nodeidList.end(); id++)
            {
                if ((*id).compare(nodeid) == 0)
                {
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
            {
                operationMode->nodeidList.push_back(nodeid);
            }
            operationMode->count = (int)operationMode->nodeidList.size();

            count++;
        }
    }

    return count;
}

int TrackDBAccess::get_NodeMoveOffsetInfolist_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<NodeMoveOffsetInfoStruct *> * infoList = (std::list<NodeMoveOffsetInfoStruct *> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            NodeMoveOffsetInfoStruct * info = new NodeMoveOffsetInfoStruct();
            info->nodeid = res->getString("nodeid");
            info->minutes = res->getInt("minutes");
            info->moveoffset = res->getInt("moveoffset");
            infoList->push_back(info);

            count++;
        }
    }

    return count;
}

int TrackDBAccess::get_TagMoveOffsetRecordlist_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<TagMoveOffsetRecordStruct *> * infoList = (std::list<TagMoveOffsetRecordStruct *> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            TagMoveOffsetRecordStruct * info = new TagMoveOffsetRecordStruct();
            info->nodetype = res->getInt("nodetype");
            info->nodeid = res->getString("nodeid");
            info->moveoffset = res->getInt("moveoffset");
            info->issueday = res->getString("issueday");
            info->issuetime = res->getString("issuetime");
            info->datetime = res->getString("datetime");
            infoList->push_back(info);

            count++;
        }
    }

    return count;
}

int TrackDBAccess::get_CourseDayTrackRecordlist_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<CourseDayTrackRecordStruct *> * infoList = (std::list<CourseDayTrackRecordStruct *> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            CourseDayTrackRecordStruct * info = new CourseDayTrackRecordStruct();
            info->nodeid = res->getString("nodeid");
            info->count = res->getInt("count");
            info->max_time = res->getString("max_time");
            info->min_time = res->getString("min_time");
            info->total_time = res->getString("total_time");
            info->datetime = res->getString("datetime");
            infoList->push_back(info);

            count++;
        }
    }

    return count;
}

int TrackDBAccess::get_DeviceActivityAliveCount_callback(sql::ResultSet * res, void * pvalue)
{
    DeviceActivityAliveCountStruct * data = (DeviceActivityAliveCountStruct *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        if (res->next())
        {
            data->devicecount = res->getInt("devicecount");
            data->aliveString = res->getString("aliveString");
            data->issueday = res->getString("issueday");
            data->issuetime = res->getString("issuetime");
            count++;
        }
    }

    return count;
}

int TrackDBAccess::get_DeviceActivityAliveCountList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<DeviceActivityAliveCountStruct *> * dataList = (std::list<DeviceActivityAliveCountStruct *> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            DeviceActivityAliveCountStruct * data = new DeviceActivityAliveCountStruct();
            data->devicecount = res->getInt("devicecount");
            data->aliveString = res->getString("aliveString");
            data->issueday = res->getString("issueday");
            data->issuetime = res->getString("issuetime");

            dataList->push_back(data);
            count++;
        }
    }

    return count;
}

int TrackDBAccess::get_StringValueList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<std::string> * stringValueList = (std::list<std::string> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            stringValueList->push_back(res->getString("stringvalue"));
            count++;
        }
    }

    return count;
}
