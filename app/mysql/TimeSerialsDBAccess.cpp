/*
 * TimeSerialsDBAccess.cpp
 *
 *  Created on: 2018/02/02
 *      Author: ikki
 */


#include <stdio.h>
#include <iostream>
#include <sstream>

#include <sys/time.h>
#include <sys/timeb.h>//timeb
#include <time.h>

#include "TimeSerialsDBAccess.h"
#include "../common.h"

using namespace std;

//
// TimeSerialsDBAccess
//
TimeSerialsDBAccess::TimeSerialsDBAccess(int projectID)
{
    mProjectID = projectID;
    init();
}

TimeSerialsDBAccess::~TimeSerialsDBAccess()
{
}

int TimeSerialsDBAccess::init()
{
    mMySQLDriver = get_driver_instance();

    if (mProjectID == 1)
    {
        mSQLDBname = sqldbname_ts;
    }
    else
    {
        mSQLDBname = sqldbname_ts;
        mSQLDBname.append("_");
        mSQLDBname.append(std::to_string(mProjectID));
    }

    vilslog::printf("TimeSerialsDBAccess::init() checkDBExist() \n");

    if (!checkDBExist())
    {
        vilslog::printf("TimeSerialsDBAccess::init() database %s not exist!\n", mSQLDBname.c_str());
        create_DB();
        create_SoccerTag_table();
    }

    return 0;
}

int TimeSerialsDBAccess::check_valid_table()
{
    vilslog::printf("TimeSerialsDBAccess::check_valid_table()\n");
    check_SoccerTag_table();
    vilslog::printf("TimeSerialsDBAccess::check_valid_table() check_SoccerTag_table() Done.\n");

    return 0;
}

int TimeSerialsDBAccess::deletedb()
{
    char cmd[4096];
    sprintf(cmd, "DROP DATABASE IF EXISTS %s", mSQLDBname.c_str());
    sql_execute(cmd);
    return 0;
}

int TimeSerialsDBAccess::getProjectID()
{
    return mProjectID;
}

bool TimeSerialsDBAccess::check_SoccerTag_table()
{
    if (tableExists("SoccerTag"))
    {
        // create_SoccerTag_table();
        createNonExistColumn("SoccerTag", "moveoffset", "float NOT NULL default 0");
        createNonExistColumn("SoccerTag", "velocity", "float NOT NULL default 0");
        createNonExistColumn("SoccerTag", "acceleration", "float NOT NULL default 0");
        createNonExistColumn("SoccerTag", "diffacceleration", "float NOT NULL default 0");
        createNonExistColumn("SoccerTag", "status", "CHAR(30)");

    }
    else
    {
        vilslog::printf("check_SoccerTag_table() SoccerTag NOT existed!\n");

        create_SoccerTag_table();
    }
    return true;
}

bool TimeSerialsDBAccess::create_SoccerTag_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        // stmt->execute("DROP TABLE IF EXISTS SoccerTag"); //drop if table exists
        stmt->execute("CREATE TABLE SoccerTag( \
            id INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY, \
            recordnumber CHAR(30) NOT NULL, \
            nodeid CHAR(30) NOT NULL, \
            team CHAR(30) NOT NULL, \
            areaid CHAR(30), \
            maplayer INT(3) default 0, \
            maingroupid INT(3) default 0, \
            posX INT(3) NOT NULL default 0, \
            posY INT(3) NOT NULL default 0, \
            posZ INT(3) default 0, \
            voltage CHAR(30), \
            moveoffset float NOT NULL default 0, \
            velocity float NOT NULL default 0, \
            acceleration float NOT NULL default 0, \
            diffacceleration float NOT NULL default 0, \
            imu CHAR(30), \
            status CHAR(30), \
            datetime TIMESTAMP(3) , \
            INDEX game_record (recordnumber, datetime)) \
            ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");
            //===============

        stmt->close();
    } catch (sql::SQLException &e) {
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

    return true;
}

int TimeSerialsDBAccess::Transaction_write_node_SoccerTag(
    std::string recordnumber, std::string team, std::string nodeid, std::string areaid,
    int maplayer, int maingroupid, int posX, int posY, int posZ, std::string voltage,
    int moveoffset, double velocity, double acceleration, double diffacceleration, std::string imu, std::string status, std::string datetime)
{
    int ret = 0;

    char cmd[4096];
    sprintf(cmd, "INSERT INTO SoccerTag(recordnumber, team, nodeid, areaid, maplayer, maingroupid, posX, posY, posZ, voltage, moveoffset, velocity, acceleration, diffacceleration, imu, status, datetime) \
                    VALUES ('%s', '%s', '%s', '%s', %d, %d, %d, %d, %d, '%s', %f, %f, %f, %f, '%s', '%s', '%s') \
                    ON DUPLICATE KEY UPDATE team=VALUES(team), areaid=VALUES(areaid), maplayer=VALUES(maplayer), maingroupid=VALUES(maingroupid), \
                    posX=VALUES(posX), posY=VALUES(posY), posZ=VALUES(posZ),voltage=VALUES(voltage), moveoffset=VALUES(moveoffset), velocity=VALUES(velocity), \
                    acceleration=VALUES(acceleration), diffacceleration=VALUES(diffacceleration), imu=VALUES(imu), status=VALUES(status), datetime=VALUES(datetime)",
                    recordnumber.c_str(), team.c_str(), nodeid.c_str(), areaid.c_str(), maplayer, maingroupid,
                    posX, posY, posZ, voltage.c_str(), (float)moveoffset, (float)velocity, (float)acceleration, (float)diffacceleration, imu.c_str(), status.c_str(), datetime.c_str());

    std::string trans = cmd;
    mMysqlTransactionQueue.add(trans);

    // vilslog::printf("TimeSerialsDBAccess::Transaction_write_node_SoccerTag() cmd[%s]\n", cmd);

    return ret;
}

int TimeSerialsDBAccess::Transaction_commit()
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
            //printf("thread MySQLServiceLoopThread, waiting for item...\n");
            std::string trans = mMysqlTransactionQueue.remove();
            stmt->execute(trans.c_str());
        }
        stmt->execute("COMMIT");

        stmt->close();
    }
    catch (sql::SQLException &e) {
        vilslog::printf("TimeSerialsDBAccess::Transaction_commit() sql::SQLException\n");

        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;

        ret = -1;
    }

    return ret;

}

bool TimeSerialsDBAccess::clear_old_data()
{
    bool ret = clear_old_data("SoccerTag", "datetime", 0);
    return ret;
}

bool TimeSerialsDBAccess::clear_old_data(const char * table, const char * column, const char * datetime)
{
    char cmd[4096];

    sprintf(cmd, "DELETE FROM %s WHERE %s <= '%s';",
        table, column, datetime);

    int ret = sql_execute(cmd);

    printf("TimeSerialsDBAccess::clear_old_data() cmd[%s] ret[%d]\n", cmd, ret);

    if (ret == 0)
        return true;
    else
        return false;
}

int TimeSerialsDBAccess::sql_execute(const char * cmd)
{
    int ret = 0;
    try {

        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        std::unique_ptr<sql::Statement> stmt(con->createStatement());

        con->setAutoCommit(true);

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

bool TimeSerialsDBAccess::sql_executeQuery_new(const char * cmd, CALLBACK callback, void * retValue)
{
    bool bRet = false;

    try {

        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        con->setSchema(mSQLDBname);
        con->setAutoCommit(true);
        std::unique_ptr<sql::Statement> stmt(con->createStatement());

        sql::ResultSet * res = stmt->executeQuery(cmd);

        //printf("TimeSerialsDBAccess::sql_executeQuery_new() cmd[%s]\n", cmd);

        int ret = callback(res, retValue);

        //printf("TimeSerialsDBAccess::sql_executeQuery_new() ret[%d]\n", ret);

        if (ret > 0)
            bRet = true;

        delete res;
        stmt->close();
    }
    catch (sql::SQLException &e) {
        vilslog::printf("TimeSerialsDBAccess::sql_executeQuery_new() sql::SQLException\n");

        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
        bRet = false;
    }

    return bRet;
}

bool TimeSerialsDBAccess::checkDBExist()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        stmt->close();
    }
    catch (sql::SQLException &e)
    {
        printSQLException(e);
        return false;
    }

    return true;
}

bool TimeSerialsDBAccess::create_DB()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        // create a new database
        //stmt->execute("DROP DATABASE IF EXISTS vils_db"); //drop if 'test_db' exists
        std::string createdb = "CREATE DATABASE ";
        createdb += mSQLDBname;
        stmt->execute(createdb.c_str());// create  new  database
        stmt->close();
    }
    catch (sql::SQLException &e)
    {
        printSQLException(e);
        return false;
    }
    return true;
}

bool TimeSerialsDBAccess::tableAddIndex(const char * tableName, const char * columnName)
{
    char cmd[4096];
    sprintf(cmd, "ALTER TABLE %s ADD INDEX covering_index(%s)", tableName, columnName);
    sql_execute(cmd);
    return true;
}

bool TimeSerialsDBAccess::tableExists(const char * tableName)
{
    char cmd[4096];
    sprintf(cmd, "SELECT count(*) FROM information_schema.tables WHERE table_schema='%s' AND table_name='%s' LIMIT 1", mSQLDBname.c_str(), tableName);

    int count = 0;
    sql_executeQuery_new(cmd, get_count_callback, &count);

    //printf("MySQLDBAccess::tableExists() count[%d]\n", count);

    if (count == 0)
        return false;
    else
        return true;
}

bool TimeSerialsDBAccess::createNonExistColumn(const char * tableName, const char * columnName, const char * colDefinition)
{
    bool bExist = columnExists(tableName, columnName);
    if (!bExist)
        create_column(tableName, columnName, colDefinition);
    return bExist;
}

bool TimeSerialsDBAccess::columnExists(const char * tableName, const char * columnName)
{
    char cmd[4096];
    sprintf(cmd, "SELECT count(*) FROM information_schema.columns WHERE table_schema='%s' AND table_name='%s' AND column_name='%s'",
        mSQLDBname.c_str(), tableName, columnName);

    int count = 0;
    sql_executeQuery_new(cmd, get_count_callback, &count);

    //printf("MySQLDBAccess::columnExists() count[%d]\n", count);

    if (count == 0)
        return false;
    else
        return true;
}

int TimeSerialsDBAccess::create_column(const char * tableName, const char * columnName, const char * colDefinition)
{
    char cmd[4096];
    sprintf(cmd, "ALTER TABLE %s ADD %s %s", tableName, columnName, colDefinition);
    int res = sql_execute(cmd);
    return res;
}

int TimeSerialsDBAccess::get_datacount(const char * tableName)
{
    char cmd[4096];
    sprintf(cmd, "SELECT count(*) FROM %s", tableName);

    int count = 0;
    sql_executeQuery_new(cmd, get_count_callback, &count);

    return count;
}

int TimeSerialsDBAccess::get_count_callback(sql::ResultSet * res, void * pvalue)
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

void TimeSerialsDBAccess::printSQLException(sql::SQLException &e)
{
    std::ostringstream sendstream;
    sendstream << "# ERR: " << e.what();
    sendstream << " (MySQL error code: " << e.getErrorCode();
    sendstream << ", SQLState: " << e.getSQLState() << " )" << endl;
    vilslog::printf("sql::SQLException [%s]\n", sendstream.str().c_str());
}
