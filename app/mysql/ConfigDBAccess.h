/*
 * ConfigDBAccess.h
 *
 *  Created on: 2019/09/17
 *      Author: ikki
 */

#ifndef ConfigDBAccess_H
#define ConfigDBAccess_H

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

typedef int (*CALLBACK) (sql::ResultSet *, void*);

typedef struct
{
   int projectid;
   std::string projectname;
   std::string projectdesc;
   std::string datetime;
} ProjectConfig;

class ConfigDBAccess
{
public:
	ConfigDBAccess();
	~ConfigDBAccess();

    int init();
    int check_valid_table();

    ProjectConfig * read_config(int projectID);
    std::list<ProjectConfig *> * read_all_config();
    int write_config(int projectID, std::string PROJECT_NAME, std::string PROJECT_DESCRIPTION);
    bool update_config(VILSConfig config);
    bool delete_config(int projectID);

private:
    sql::Driver * mMySQLDriver;
    std::string mSQLDBname;

    bool checkDBExist();
    int create_DB();
    bool tableExists(const char * tableName);

    int sql_execute(const char * cmd);
    bool sql_executeQuery_new(const char * cmd, CALLBACK callback, void * retValue);

    bool check_config_table();
    bool create_config_table();

    static int get_count_callback(sql::ResultSet *, void*);
    static int get_projectConfiglist_callback(sql::ResultSet *, void*);
    static int get_projectConfig_callback(sql::ResultSet *, void*);
};

#endif
