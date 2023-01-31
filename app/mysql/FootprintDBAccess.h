/*
 * FootprintDBAccess.h
 *
 *  Created on: 2020/02/24
 *      Author: ikki
 */

#ifndef FootprintDBAccess_H
#define FootprintDBAccess_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>

#include "../config.h"
#include "../network/wqueue.h"
#include "struct.h"

typedef int (*CALLBACK) (sql::ResultSet *, void*);

typedef struct
{
    int maplayer;
    int posX;
    int posY;
    //int posZ;
    std::string areaid;
    //std::string voltage;
    //std::string temperature;
    std::string order_time;
    std::string datetime;
} FootprintDataStruct;

typedef struct
{
    std::string macaddress;
    std::string max_time;
    std::string min_time;
    std::string order_day;
    std::list<FootprintDataStruct *> * dataList;
} FootprintListStruct;

typedef struct
{
    std::string dbdata;
    std::string macaddress;
    std::string analyzeddate;
    std::string datetime;
} FootprintRecordStruct;

class FootprintDBAccess
{
public:
	FootprintDBAccess(int projectID);
	~FootprintDBAccess();

    int init();
    int check_valid_table();
    int getProjectID();

    AnalyzeDayInfoStruct * getAnalyzeDayInfo(const char * nodeid, const char * macaddress, const char * targetDate);

    // footprint
    int write_node_footprint(const char * macaddress, const char * targetDate, const char * targetTime, const char * areaid,
        int maplayer, int posX, int posY, int posZ, const char * datetime);

    int Transaction_write_node_footprint(const char * macaddress, const char * targetDate, const char * targetTime, const char * areaid,
        int maplayer, int posX, int posY, int posZ, const char * datetime);

    int Transaction_commit();

    int save_to_db();
    int clear_old_data();

private:

	int mProjectID;
    sql::Driver * mMySQLDriver;
	std::string mSQLDBname;

    std::list<FootprintListStruct *> mAllFootprintDataList;
    wqueue<std::string> mMysqlTransactionQueue;

    bool checkDBExist();
    int create_DB();
    int create_table();
    bool check_Footprint_table();
    bool create_Footprint_table();

    FootprintListStruct * getNodeFootprintList(const char * macaddress, const char * targetDate);
    FootprintListStruct * findNodeFootprintList(const char * macaddress, const char * targetDate);
    std::string convertFootprintDataListToString(std::list<FootprintDataStruct *> * dataList);

    int sql_execute(const char * cmd);
    bool sql_executeQuery_new(const char * cmd, CALLBACK callback, void * retValue);

    bool tableExists(const char * tableName);
    bool createNonExistColumn(const char * tableName, const char * columnName, const char * colDefinition);
    bool columnExists(const char * tableName, const char * columnName);
    int create_column(const char * tableName, const char * columnName, const char * colDefinition);
    int get_datacount(const char * tableName);

    static int get_count_callback(sql::ResultSet *, void*);
    static int get_FootprintRecord_callback(sql::ResultSet *, void*);

};

#endif
