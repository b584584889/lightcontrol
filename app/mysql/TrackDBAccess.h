/*
 * TrackDBAccess.h
 *
 *  Created on: 2019/07/22
 *      Author: ikki
 */

#ifndef TrackDBAccess_H
#define TrackDBAccess_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <vector>

#include "mysql_connection.h"
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include "../config.h"
#include "struct.h"
#include "../network/wqueue.h"

typedef int (*CALLBACK) (sql::ResultSet *, void*);

#define TRACKDB_CLEAR_DATA_DAYS 30

typedef enum
{
    TDT_POS = 1,
    TDT_AREA,
    TDT_GPS,
    TDT_COURSE,
    TDT_ALERT,
    TDT_EVENT
} TrackDataType;

typedef struct
{
    int projectid;
    int nodetype;
    std::string nodeid;
    int datatype;
    int analyzeddone; // 1:done
    std::string analyzeddate; // date for already analyzed, foramt: YYYY-MM-DD
    std::string datetime; // analyze action update datetime
} AnalyzeTrackRecordStruct;

typedef struct
{
    int projectid;
    std::string nodeid;
    int nodetype;
    int datatype;
    int count; // 1:done
    std::string day; // date for already analyzed, foramt: YYYY-MM-DD
    std::string max_time;
    std::string min_time;
    std::string total_time;
    std::string area_ids;
    std::string area_names;
    std::string course_id;
    std::string course_info;
    std::string datetime; // analyze action update datetime
} TagDayTrackRecordStruct;

typedef struct
{
    std::string areaid;
    std::string areaname;
    std::string courseId;
    std::string course_title;
    std::string course_start;
    std::string course_end;
    std::string course_startTime;
    std::string course_endTime;
    std::string course_startRecur;
    std::string course_endRecur;
    std::string in_max_time;
    std::string in_min_time;
    int in_count;
    std::string out_max_time;
    std::string out_min_time;
    int out_count;
} TagCourseDayRecordStruct;

typedef struct
{
    int nodetype;
    std::string nodeid;
    int alive;
    std::string issueday; // foramt: YYYY-MM-DD
    std::string issuetime; // foramt: HH:mm
} DeviceActivityRecordStruct;

typedef struct
{
    int projectid;
    std::string areaid;
    std::string nodeid;
    std::string issueday; // foramt: YYYY-MM-DD
    std::string issuetime; // foramt: HH:mm
    int totaltime; // foramt: seconds how many seconds stay in this minute at this area
    int totaltimeInMS; // no in database
    std::string datetime;
} AreaRollCallRecordStruct;

typedef struct
{
    int projectid;
    int mode; // 0:None 1:UWB Ranging 2:LoRa Ranging 3:GPS
    std::string nodeid;
    std::string issueday; // foramt: YYYY-MM-DD
    std::string issuetime; // foramt: HH:mm
    int totaltime; // foramt: seconds how many seconds stay in this minute at this area
    int totaltimeInMS; // no in database
    std::string datetime;
} OperationModeRecordStruct;

typedef struct
{
    int nodetype;
    std::string nodeid;
    int moveoffset;// cm
    std::string issueday; // foramt: YYYY-MM-DD
    std::string issuetime; // foramt: HH:mm
    std::string datetime;
} TagMoveOffsetRecordStruct;

typedef struct
{
    int nodetype;
    int devicecount;
    std::string aliveString;
    std::string issueday; // foramt: YYYY-MM-DD
    std::string issuetime; // foramt: HH:mm
} DeviceActivityAliveCountStruct;

typedef struct
{
    int count;
    int totaltime;
    std::string areaid;
    std::string areaname;
    std::string issueday; // foramt: YYYY-MM-DD
    std::string issuetime; // foramt: HH:mm
    std::string start_datetime; // foramt: YYYY-MM-DD HH:mm:ss
    std::string end_datetime;
} AreaActivityUserCountStruct;

typedef struct
{
    std::string nodeid;
    std::string nodename;
    int count;
    std::string max_time;
    std::string min_time;
    std::string total_time;

    // course base info
    //std::string courseId;
    //std::string course_title;
    //std::string resourceId; // locatorId
    //std::string day; // date foramt: YYYY-MM-DD
    //std::string tag_ids;
    //std::string tag_names;
    std::string datetime; // analyze action update datetime
} CourseDayTrackRecordStruct;

class TrackDBAccess
{
public:
	TrackDBAccess(int projectID);
	~TrackDBAccess();

    int init();
    int check_valid_table();
    int deletedb();
    int getProjectID();

    std::list<std::string> * getAnalyzedDate(const char * nodeid, TrackDataType datatype, std::string StartDate);
    std::list<DayRecordStruct*> * getDeviceActivityCount(int nodetype, std::string target_date, int limit);
    int getDeviceActivityCount(int nodetype, std::string target_date, std::string issuetime);
    std::list<DeviceActivityRecordStruct *> * getDeviceActivityRecordList(int nodetype, std::string target_date, int limit);

    std::list<std::string> * getDeviceActivityNodeIdList(int nodetype, std::string target_date, std::string issuetime);
    // std::list<DeviceAliveCountStruct*> * getDeviceActivityAliveCount(int nodetype, std::string target_date, std::string start_time, std::string end_time);
    int getDeviceActivityAliveCount(int nodetype, std::string target_date, std::string start_time, std::string end_time);
    std::list<AreaActivityTimeStruct*> * getAreaActivityTime(std::string start_datetime, std::string end_datetime);
    std::list<std::string> * getAreaActivityUser(std::string areaid, std::string start_datetime, std::string end_datetime);
    std::list<AreaRollCallRecordStruct *> * getAreaRollCallList(std::string start_datetime, std::string end_datetime);
	OperationModeActivityStruct * getOperationModeActivity(int mode, std::string start_datetime, std::string end_datetime);
    std::list<OperationModeRecordStruct *> * getOperationModeRecordList(std::string start_datetime, std::string end_datetime);
	std::list<OperationModeActivityStruct *> * getOperationModeActivityList(std::string start_datetime, std::string end_datetime);

    std::list<NodeMoveOffsetInfoStruct *> * getNodeMoveOffset(std::string start_datetime, std::string end_datetime);
    std::list<TagMoveOffsetRecordStruct *> * getTagMoveOffsetRecordList(std::string start_datetime, std::string end_datetime);

    std::list<CourseDayTrackRecordStruct *> * getCourseRecordList(std::string course_id, std::string targetday);

    bool write_record(AnalyzeTrackRecordStruct * record);
    bool write_daytrack_record(TagDayTrackRecordStruct * record);
    bool write_deviceactivity_record(DeviceActivityRecordStruct * record);
    bool write_arearollcall_record(AreaRollCallRecordStruct * record);
    bool write_operationmode_record(OperationModeRecordStruct * record);
    bool write_DeviceActivityAliveCount_record(DeviceActivityAliveCountStruct * record);
    bool write_AreaActivityUserCount_record(AreaActivityUserCountStruct * record);
    bool write_OperationModeActivityCount_record(OperationModeActivityStruct * record);
    bool write_TagMoveOffsetInfo_record(int nodetype, std::string nodeid, int moveoffset);
    bool write_NodeMoveOffsetInfo_record(NodeMoveOffsetInfoStruct * record);

	bool clear_old_data(const char * table, const char * column, const char * datetime);

    // show on dashboard
    DeviceActivityAliveCountStruct * loadactivity(int nodetype);
    std::list<DeviceActivityAliveCountStruct *> * loadactivitys(int nodetype, std::string issueday, int count);

    int Transaction_commit();

private:

	int mProjectID;
    sql::Driver * mMySQLDriver;
	std::string mSQLDBname;

    bool bEnableTransaction;

    wqueue<std::string> mMysqlTransactionQueue;

    bool checkDBExist();
    int create_DB();

    bool check_AnalyzeTrackRecord_table();
    bool create_AnalyzeTrackRecord_table();

    bool check_DayTrack_table();
    bool create_DayTrack_table();

    bool check_DeviceActivity_table();
    bool create_DeviceActivity_table();

    bool check_AreaRollCall_table();
    bool create_AreaRollCall_table();

    bool check_OperationMode_table();
    bool create_OperationMode_table();

    bool check_DeviceActivityAliveCount_table();
    bool create_DeviceActivityAliveCount_table();

    bool check_AreaActivityUserCount_table();
    bool create_AreaActivityUserCount_table();

    bool check_OperationModeActivityCount_table();
    bool create_OperationModeActivityCount_table();

    bool check_NodeMoveOffsetInfo_table();
    bool create_NodeMoveOffsetInfo_table();

    bool check_NodeMoveOffsetRecord_table();
    bool create_NodeMoveOffsetRecord_table();

    int sql_execute(const char * cmd);
    bool sql_executeQuery(const char * cmd, CALLBACK callback, void * retValue);

    bool tableExists(const char * tableName);
    bool createNonExistColumn(const char * tableName, const char * columnName, const char * colDefinition);
    bool columnExists(const char * tableName, const char * columnName);
    int create_column(const char * tableName, const char * columnName, const char * colDefinition);

    static int get_count_callback(sql::ResultSet *, void*);
    static int get_AnalyzedList_callback(sql::ResultSet *, void*);
    static int get_DeviceActivityCountList_callback(sql::ResultSet *, void*);
    static int get_DeviceActivityRecordList_callback(sql::ResultSet *, void*);
    static int get_DeviceAliveCountList_callback(sql::ResultSet *, void*);
    static int get_AreaActivityTimeList_callback(sql::ResultSet *, void*);
    static int get_AreaActivityUserList_callback(sql::ResultSet *, void*);
    static int get_AreaRollCallList_callback(sql::ResultSet * res, void * pvalue);
    static int get_OperationModeActivity_callback(sql::ResultSet *, void*);
    static int get_OperationModeRecordList_callback(sql::ResultSet *, void*);
    static int get_OperationModeActivityList_callback(sql::ResultSet *, void*);
    static int get_NodeMoveOffsetInfolist_callback(sql::ResultSet *, void*);
    static int get_TagMoveOffsetRecordlist_callback(sql::ResultSet *, void*);
    static int get_CourseDayTrackRecordlist_callback(sql::ResultSet *, void*);

    // show on dashboard
    static int get_DeviceActivityAliveCount_callback(sql::ResultSet *, void*);
    static int get_DeviceActivityAliveCountList_callback(sql::ResultSet * res, void * pvalue);
    static int get_StringValueList_callback(sql::ResultSet * res, void * pvalue);

};


#endif
