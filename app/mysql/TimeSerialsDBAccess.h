/*
 * TimeSerialsDBAccess.h
 *
 *  Created on: 2018/02/02
 *      Author: ikki
 */

#ifndef TimeSerialsDBAccess_H
#define TimeSerialsDBAccess_H


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
    int nodetype;
    std::string nodeid;
    std::string areaid;
    int posX;
    int posY;
    int posZ;
    int sampling;// 1, 2, 3,...
    std::string voltage;
    std::string temperature;
    std::string date;
    std::string datetime;
} FootprintAllStruct;

typedef struct
{
    int nodetype;
    std::string nodeid;
    int datacount;
    int analyzeddone; // 1:done
    int regeneratedate; // 1:done
    std::string analyzeddate; // date for already analyzed
    std::string datetime; // analyze action update datetime
} AnalyzeRecordStruct;

typedef struct
{
    std::string recordnumber;
    std::string nodeid;
    std::string team;
    std::string areaid;
    int maplayer;
    int maingroupid;
    int posX;
    int posY;
    int posZ;
    std::string voltage;
    int IMU[6];// scale 0.01
    std::string status;
    std::string datetime;
} SoccerTagStruct;

class TimeSerialsDBAccess
{
public:
	TimeSerialsDBAccess(int projectID);
	~TimeSerialsDBAccess();

    int init();
    int check_valid_table();
    int deletedb();
    int getProjectID();

    // SoccerTag
    int Transaction_write_node_SoccerTag(
        std::string recordnumber, std::string team, std::string nodeid, std::string areaid,
        int maplayer, int maingroupid, int posX, int posY, int posZ, std::string voltage, int moveoffset,
        double velocity, double acceleration, double diffacceleration, std::string imu, std::string status, std::string datetime);

    int Transaction_commit();

	bool clear_old_data();
    bool clear_old_data(const char * table, const char * column, const char * datetime);

private:

	int mProjectID;
    sql::Driver * mMySQLDriver;
	std::string mSQLDBname;

    bool checkDBExist();
    bool create_DB();

    bool check_SoccerTag_table();
    bool create_SoccerTag_table();

    wqueue<std::string> mMysqlTransactionQueue;

    int sql_execute(const char * cmd);
    bool sql_executeQuery_new(const char * cmd, CALLBACK callback, void * retValue);

    bool tableAddIndex(const char * tableName, const char * columnName);
    bool tableExists(const char * tableName);
    bool createNonExistColumn(const char * tableName, const char * columnName, const char * colDefinition);
    bool columnExists(const char * tableName, const char * columnName);
    int create_column(const char * tableName, const char * columnName, const char * colDefinition);
    int get_datacount(const char * tableName);

    static int get_count_callback(sql::ResultSet *, void*);
    void printSQLException(sql::SQLException &e);
};


#endif
