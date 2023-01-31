/*
 * AnalyzeDashBoardDBAccess.h
 *
 *  Created on: 2020/01/31
 *      Author: ikki
 */

#ifndef AnalyzeDashBoardDBAccess_H
#define AnalyzeDashBoardDBAccess_H

#include <list>
#include <vector>

#include "sqlite3.h"
#include "../mysql/TrackDBAccess.h"

#define cachedbname ":memory:"
//#define cachedbname "file::memory:?cache=shared"
// #define cachedbname "file:memdb%d?mode=memory&cache=shared"


class AnalyzeDashBoardDBAccess
{
public:
	AnalyzeDashBoardDBAccess();
	~AnalyzeDashBoardDBAccess();

    int init();
	bool checkDBExist();
    int check_valid_table();
	int loadOrSaveDb(int projectid, int isSave);

    bool write_deviceactivity_record(DeviceActivityRecordStruct * record);
	bool write_deviceactivity_record_full(DeviceActivityRecordStruct * record);
    bool write_arearollcall_record(AreaRollCallRecordStruct * record);
    bool write_arearollcall_record_full(AreaRollCallRecordStruct * record);
    bool write_operationmode_record(OperationModeRecordStruct * record);
    bool write_operationmode_record_full(OperationModeRecordStruct * record);
    bool write_TagMoveOffsetInfo_record(int nodetype, std::string nodeid, int moveoffset);
    bool write_TagMoveOffset_record_full(TagMoveOffsetRecordStruct * record);

    std::list<std::string> * getDeviceActivityNodeIdList(int nodetype, std::string target_date, std::string issuetime);

    std::list<AreaActivityTimeStruct*> * getAreaActivityTime(std::string start_datetime, std::string end_datetime);
    std::list<std::string> * getAreaActivityUser(std::string areaid, std::string start_datetime, std::string end_datetime);

    std::list<OperationModeActivityStruct *> * getOperationModeActivityList(std::string start_datetime, std::string end_datetime);
    std::list<NodeMoveOffsetInfoStruct *> * getNodeMoveOffset(std::string start_datetime, std::string end_datetime);

    bool clear_old_data(const char * datetime);
    bool clear_old_data(const char * table, const char * column, const char * datetime);

private:

    sqlite3 * mSqlitedb;
    char mCacheDBName[256];

    bool checkDBExist(int flag);
	int getTableDataCount(const char * tableName);
	int create_table();

    bool check_DeviceActivity_table();
    bool create_DeviceActivity_table();

    bool check_AreaRollCall_table();
    bool create_AreaRollCall_table();

    bool check_OperationMode_table();
    bool create_OperationMode_table();

    bool check_NodeMoveOffsetInfo_table();
    bool create_NodeMoveOffsetInfo_table();

    std::list<std::string> * get_column_list(const char * table_name);
    bool create_column(const char * tableName, const char * colName, const char * colDefinition);
    bool check_column_Exists(const char * tableName, const char * colName, const char * colDefinition);
    bool tableExists(const char * tableName);

    static int check_column_callback(void*, int, char** , char**);
    static int get_count_callback(void*, int, char**, char**);
    static int get_StringValueList_callback(void*, int, char**, char**);

    static int get_AreaActivityTimeList_callback(void*, int, char**, char**);
    static int get_AreaActivityUserList_callback(void*, int, char**, char**);
    static int get_OperationModeActivityList_callback(void*, int, char**, char**);
    static int get_NodeMoveOffsetInfolist_callback(void*, int, char**, char**);
    static int callback_busy_handler(void *ptr,int count);
};

#endif
