/*
 * AreaRollCallInfoDBAccess.h
 *
 *  Created on: 2020/03/09
 *      Author: ikki
 */

#ifndef AreaRollCallInfoDBAccess_H
#define AreaRollCallInfoDBAccess_H

#include <list>
#include <vector>

#include "sqlite3.h"
#include "../mysql/struct.h"

#define cachedbname ":memory:"
#define AreaRollCall_prefix "arearollcallinfodb"
//#define cachedbname "file::memory:?cache=shared"
// #define cachedbname "file:memdb%d?mode=memory&cache=shared"

class AreaRollCallInfoDBAccess
{
public:
	AreaRollCallInfoDBAccess();
	~AreaRollCallInfoDBAccess();

    int init();
	bool checkDBExist();
    int check_valid_table();
	int loadOrSaveDb(int projectid, int isSave);

    bool write_TagAreaBuffer_record(TagAreaBufferStruct * record, BufferAreaInfoStruct * buffer);
	bool remove_TagAreaBuffer_record(TagAreaBufferStruct * record, BufferAreaInfoStruct * buffer);
	bool write_SleepPool_record(SleepPoolStruct * record);

	bool deleteTagAreaBuffer(TagAreaBufferStruct * record);
	bool deleteTagSleepPool(SleepPoolStruct * record);

    std::list<TagAreaBufferStruct*> * getTagAreaBufferList();
	std::list<SleepPoolStruct*> * getSleepPoolList();

    bool clear_old_data(const char * datetime);
    bool clear_old_data(const char * table, const char * column, const char * datetime);

private:

    sqlite3 * mSqlitedb;
    char mCacheDBName[256];
	int mProjectid;


    bool checkDBExist(int flag);
	int getTableDataCount(const char * tableName);
	int create_table();

    bool check_TagAreaBuffer_table();
    bool create_TagAreaBuffer_table();

	bool check_SleepPool_table();
    bool create_SleepPool_table();

    std::list<std::string> * get_column_list(const char * table_name);
    bool create_column(const char * tableName, const char * colName, const char * colDefinition);
    bool check_column_Exists(const char * tableName, const char * colName, const char * colDefinition);
    bool tableExists(const char * tableName);

    static int check_column_callback(void*, int, char** , char**);
    static int get_count_callback(void*, int, char**, char**);
    static int get_StringValueList_callback(void*, int, char**, char**);

    static int get_TagAreaBufferList_callback(void*, int, char**, char**);
	static int get_SleepPoolList_callback(void*, int, char**, char**);
    static int callback_busy_handler(void *ptr,int count);
};

#endif
