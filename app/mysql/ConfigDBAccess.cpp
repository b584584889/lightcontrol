/*
 * ConfigDBAccess.cpp
 *
 *  Created on: 2019/09/17
 *      Author: ikki
 */

#include <stdlib.h>
#include <iostream>
#include <sstream>

#include "ConfigDBAccess.h"
#include "../common.h"

using namespace std;

ConfigDBAccess::ConfigDBAccess()
{
    init();
}

ConfigDBAccess::~ConfigDBAccess()
{
}

int ConfigDBAccess::init()
{
    mMySQLDriver = get_driver_instance();

    mSQLDBname = configsqldbname;

    if (!checkDBExist())
    {
        vilslog::printf("ConfigDBAccess::init() database %s not exist!\n", mSQLDBname.c_str());

        create_DB();
        check_valid_table();
    }

    return 0;
}

bool ConfigDBAccess::checkDBExist()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        cout << "test";
        stmt->execute(usedb.c_str());
        stmt->close();
    } catch (sql::SQLException &e) {
        cout << "ConfigDBAccess::checkDBExist(): # ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;

        return false;
    }

    return true;
}

int ConfigDBAccess::create_DB()
{
    cout << "ConfigDBAccess::create_DB() Started" << endl;

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
        cout << "ConfigDBAccess::create_DB()  # ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

    cout << "ConfigDBAccess::create_DB() Successfully ended" << endl;

    return 0;
}

bool ConfigDBAccess::tableExists(const char * tableName)
{
    char cmd[4096];
    sprintf(cmd, "SELECT count(*) FROM information_schema.tables WHERE table_schema='%s' AND table_name='%s' LIMIT 1 ", mSQLDBname.c_str(), tableName);

    int count = 0;
    sql_executeQuery_new(cmd, get_count_callback, &count);

    if (count == 0)
        return false;
    else
        return true;

    return false;
}

int ConfigDBAccess::sql_execute(const char * cmd)
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
        //delete stmt;
        //delete con;
    }
    catch (sql::SQLException &e) {
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;

        ret = -1;
    }

    return ret;
}

bool ConfigDBAccess::sql_executeQuery_new(const char * cmd, CALLBACK callback, void * retValue)
{
    bool bRet = false;
    try {

        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        con->setSchema(mSQLDBname);
        con->setAutoCommit(true);
        std::unique_ptr<sql::Statement> stmt(con->createStatement());

        sql::ResultSet * res = stmt->executeQuery(cmd);

        //printf("MySQLDBAccess::sql_executeQuery_new() cmd[%s]\n", cmd);

        int ret = callback(res, retValue);

        //printf("MySQLDBAccess::sql_executeQuery_new() ret[%d]\n", ret);

        if (ret > 0)
            bRet = true;

        delete res;
        stmt->close();
    }
    catch (sql::SQLException &e) {
        vilslog::printf("MySQLDBAccess::sql_executeQuery_new() sql::SQLException cmd[%s]\n", cmd);

        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
        return false;
    }

    return bRet;
}

int ConfigDBAccess::check_valid_table()
{
    check_config_table();
    return 0;
}

bool ConfigDBAccess::check_config_table()
{
    if (tableExists("config"))
    {
    }
    else
    {
        vilslog::printf("check_config_table() config table NOT existed!\n");
        create_config_table();
    }

    return true;
}

bool ConfigDBAccess::create_config_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //===============
        // create a new table
        // config
        // stmt->execute("DROP TABLE IF EXISTS config"); //drop if table exists
        stmt->execute("CREATE TABLE config( \
             PROJECT_ID INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY, \
             PROJECT_NAME varchar(50) COLLATE utf8mb4_unicode_ci NOT NULL, \
             PROJECT_DESCRIPTION varchar(50) COLLATE utf8mb4_unicode_ci NOT NULL, \
             datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
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

ProjectConfig * ConfigDBAccess::read_config(int projectID)
{
    ProjectConfig * config = new ProjectConfig();

    char cmd[4096];
    sprintf(cmd, "SELECT * FROM config WHERE PROJECT_ID=%d ", projectID);

    sql_executeQuery_new(cmd, get_projectConfig_callback, config);

    return config;
}

std::list<ProjectConfig*> * ConfigDBAccess::read_all_config()
{
    list<ProjectConfig*> * configList = new list<ProjectConfig*>();
    char cmd[4096];
    sprintf(cmd, "SELECT * FROM config");

    sql_executeQuery_new(cmd, get_projectConfiglist_callback, configList);

    return configList;
}

int ConfigDBAccess::write_config(int projectID, std::string PROJECT_NAME, std::string PROJECT_DESCRIPTION)
{
    char cmd[4096];
    sprintf(cmd, "INSERT INTO config(PROJECT_ID, PROJECT_NAME, PROJECT_DESCRIPTION) \
                    VALUES (%d, '%s', '%s') \
                    ON DUPLICATE KEY UPDATE PROJECT_NAME=VALUES(PROJECT_NAME), PROJECT_DESCRIPTION=VALUES(PROJECT_DESCRIPTION) ",
                    projectID, PROJECT_NAME.c_str(), PROJECT_DESCRIPTION.c_str());

    int ret = sql_execute(cmd);

    return ret;
}

bool ConfigDBAccess::update_config(VILSConfig config)
{
    char cmd[4096];

    sprintf(cmd, "UPDATE config SET PROJECT_NAME='%s', PROJECT_DESCRIPTION='%s' WHERE PROJECT_ID=%d ",
                config.projectname.c_str(), config.projectdesc.c_str(), config.projectid);

    sql_execute(cmd);
    return true;
}

bool ConfigDBAccess::delete_config(int projectID)
{
    char cmd[4096];

    sprintf(cmd, "DELETE from config WHERE PROJECT_ID=%d ", projectID);
    //printf("ConfigDBAccess::delete_config() \ncmd[%s]\n", cmd);
    sql_execute(cmd);
    return true;
}

int ConfigDBAccess::get_count_callback(sql::ResultSet * res, void * pvalue)
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
   return *count;
}

int ConfigDBAccess::get_projectConfiglist_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<ProjectConfig*> * configList = (std::list<ProjectConfig*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            ProjectConfig * config = new ProjectConfig();
            config->projectid = res->getInt("PROJECT_ID");
            config->projectname = res->getString("PROJECT_NAME");
            config->projectdesc = res->getString("PROJECT_DESCRIPTION");

            config->datetime = res->getString("datetime");

            configList->push_back(config);
            count++;
        }
    }
    return count;
}

int ConfigDBAccess::get_projectConfig_callback(sql::ResultSet * res, void * pvalue)
{
    ProjectConfig* config = (ProjectConfig *) pvalue;

    int count = 0;
    if ( res != NULL)
    {
        if(res->next())
        {
            config->projectid = res->getInt("PROJECT_ID");
            config->projectname = res->getString("PROJECT_NAME");
            config->projectdesc = res->getString("PROJECT_DESCRIPTION");
            config->datetime = res->getString("datetime");
            count++;
        }
    }
    return count;
}
