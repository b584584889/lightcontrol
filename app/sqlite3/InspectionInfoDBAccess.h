/*
 * InspectionInfoDBAccess.h
 *
 *  Created on: 2020/03/09
 *      Author: ikki
 */

#ifndef InspectionInfoDBAccess_H
#define InspectionInfoDBAccess_H

#include <list>
#include <vector>

#include "sqlite3.h"
#include "../mysql/struct.h"

#define cachedbname ":memory:"
#define Inspection_prefix "inspectioninfodb"
//#define cachedbname "file::memory:?cache=shared"
// #define cachedbname "file:memdb%d?mode=memory&cache=shared"

class InspectionInfoDBAccess
{
public:
	InspectionInfoDBAccess();
	~InspectionInfoDBAccess();

    int init();
	bool checkDBExist();
    int check_valid_table();
	int loadOrSaveDb(int projectid, int isSave);

    bool write_SendNotificationTime_record(SendNotificationTimeData * record);

    std::list<SendNotificationTimeData*> * getSendNotificationTimeList();

    bool clear_old_data(const char * datetime);
    bool clear_old_data(const char * table, const char * column, const char * datetime);

private:

    sqlite3 * mSqlitedb;
    char mCacheDBName[256];

    bool checkDBExist(int flag);
	int getTableDataCount(const char * tableName);
	int create_table();

    bool check_SendNotificationTime_table();
    bool create_SendNotificationTime_table();

    std::list<std::string> * get_column_list(const char * table_name);
    bool create_column(const char * tableName, const char * colName, const char * colDefinition);
    bool check_column_Exists(const char * tableName, const char * colName, const char * colDefinition);
    bool tableExists(const char * tableName);

    static int check_column_callback(void*, int, char** , char**);
    static int get_count_callback(void*, int, char**, char**);
    static int get_StringValueList_callback(void*, int, char**, char**);

    static int get_SendNotificationTimeList_callback(void*, int, char**, char**);
    static int callback_busy_handler(void *ptr,int count);
};

#endif
