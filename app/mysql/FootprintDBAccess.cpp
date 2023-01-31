/*
 * FootprintDBAccess.cpp
 *
 *  Created on: 2020/02/24
 *      Author: ikki
 */


#include <stdio.h>
#include <iostream>
#include <sstream>

#include <sys/time.h>
#include <sys/timeb.h>//timeb
#include <time.h>

#include "FootprintDBAccess.h"
#include "../common.h"

using namespace std;

//
// FootprintDBAccess
//
FootprintDBAccess::FootprintDBAccess(int projectID)
{
    mProjectID = projectID;
    init();
}

FootprintDBAccess::~FootprintDBAccess()
{
}

int FootprintDBAccess::init()
{
    mMySQLDriver = get_driver_instance();

    if (mProjectID == 1)
    {
        mSQLDBname = sqldbname_fp;
    }
    else
    {
        mSQLDBname = sqldbname_fp;
        mSQLDBname.append("_");
        mSQLDBname.append(std::to_string(mProjectID));
    }

    vilslog::printf("FootprintDBAccess::init() checkDBExist() \n");

    if (!checkDBExist())
    {
        vilslog::printf("FootprintDBAccess::init() database %s not exist!\n", mSQLDBname.c_str());
        create_DB();
        create_table();
    }

    return 0;
}

int FootprintDBAccess::check_valid_table()
{
    vilslog::printf("FootprintDBAccess::check_valid_table()\n");
    check_Footprint_table();
    vilslog::printf("FootprintDBAccess::check_valid_table() check_Footprint_table() Done.\n");
    return 0;
}

int FootprintDBAccess::getProjectID()
{
    return mProjectID;
}

AnalyzeDayInfoStruct * FootprintDBAccess::getAnalyzeDayInfo(const char * nodeid, const char * macaddress, const char * targetDate)
{
    AnalyzeDayInfoStruct * dayInfo = createAnalyzeDayInfoStruct();
    dayInfo->nodeid = nodeid;

    //
    // find list existed!
    //
    FootprintListStruct * nodeFootprintList = getNodeFootprintList(macaddress, targetDate);
    if ( nodeFootprintList == NULL )
    {
        vilslog::printf("FootprintDBAccess::getAnalyzeDayInfo() nodeFootprintList == NULL macaddress[%s]\n", macaddress);
        return dayInfo;
    }

    dayInfo->max_time = nodeFootprintList->max_time;
    dayInfo->min_time = nodeFootprintList->min_time;
    dayInfo->count = (int)nodeFootprintList->dataList->size();
    dayInfo->day = nodeFootprintList->order_day;

    // vilslog::printf("FootprintDBAccess::getAnalyzeDayInfo() nodeid[%s] macaddress[%s] max_time[%s] min_time[%s] count[%d] day[%s]\n",
    //     nodeid, macaddress, dayInfo->max_time.c_str(), dayInfo->min_time.c_str(), dayInfo->count, dayInfo->day.c_str());

    return dayInfo;
}

FootprintListStruct * FootprintDBAccess::findNodeFootprintList(const char * macaddress, const char * targetDate)
{
    FootprintListStruct * FootprintList = new FootprintListStruct();
    FootprintList->macaddress = macaddress;
    FootprintList->order_day = targetDate;
    FootprintList->dataList = new std::list<FootprintDataStruct *>();

    char cmd[4096];

    std::string dbkey = macaddress;
    dbkey.append("_");
    dbkey.append(targetDate);

    sprintf(cmd, "SELECT dbdata, datetime from Footprint WHERE dbkey='%s' ", dbkey.c_str());
    FootprintRecordStruct * data = new FootprintRecordStruct();
    bool bSuccess = sql_executeQuery_new(cmd, get_FootprintRecord_callback, data);
    if (bSuccess)
    {
        // vilslog::printf("FootprintDBAccess::findNodeFootprintDataList() cmd[%s] Found Data!\n", cmd);

        // parse JSON data
        Json::Value JSONresult;
        try{
            Json::Reader * reader = new Json::Reader();
            bool parsingSuccessful = reader->parse( data->dbdata.c_str(), JSONresult);
            delete reader;

            if (parsingSuccessful && JSONresult.isArray())
            {
                // vilslog::printf("FootprintDBAccess::findNodeFootprintDataList() JSONresult.isArray() size()[%d]\n", JSONresult.size());


                for(unsigned int i=0;i<JSONresult.size();i++)
                {
                    FootprintDataStruct * newData = new FootprintDataStruct();

                    Json::Value item = JSONresult[i];
                    // {"p":"146,353","a":"areaid1234","t":"13:26:57"}
                    if ( item.isMember("p") )
                    {
                        std::string pos = item["p"].asString();
                        std::vector<std::string> posVector = convert_string_to_vector(pos, ',');
                        if ( posVector.size() >= 1 )
                            newData->posX = convert_string_to_int(posVector[0]);
                        if ( posVector.size() >= 2 )
                            newData->posY = convert_string_to_int(posVector[1]);
                        posVector.clear();
                    }
                    if ( item.isMember("m") )
                    {
                        newData->maplayer = convert_string_to_int(item["m"].asString());
                    }
                    if ( item.isMember("a") )
                    {
                        newData->areaid = item["a"].asString();
                    }
                    if ( item.isMember("t") )
                    {
                        newData->order_time = item["t"].asString();
                    }
                    newData->datetime = targetDate;
                    newData->datetime.append(" ");
                    newData->datetime.append(newData->order_time);

                    if ( (FootprintList->max_time.length() == 0) || (FootprintList->max_time.compare(newData->datetime) <= 0) )
                        FootprintList->max_time = newData->datetime;
                    if ( (FootprintList->min_time.length() == 0) || (FootprintList->min_time.compare(newData->datetime) >= 0) )
                        FootprintList->min_time = newData->datetime;

                    // vilslog::printf("FootprintDBAccess::findNodeFootprintDataList() newData[%d, %d] areaid[%s] order_time[%s] datetime[%s]\n",
                    //     newData->posX, newData->posY, newData->areaid.c_str(), newData->order_time.c_str(), newData->datetime.c_str());

                    // check if same time
                    if ( FootprintList->dataList->size() > 0)
                    {
                        // std::list<FootprintDataStruct*>::iterator lastData = FootprintList->dataList->end();

                        // vilslog::printf("FootprintDBAccess::findNodeFootprintDataList() datetime[%s]\n",
                        //     newData->datetime.c_str());
                        // vilslog::printf("FootprintDBAccess::findNodeFootprintDataList() (*lastData)->order_time[%s]\n",
                        //     FootprintList->dataList->back()->order_time.c_str());

                        if ( FootprintList->dataList->back()->order_time.compare(newData->order_time) == 0)
                        {
                            vilslog::printf("FootprintDBAccess::findNodeFootprintDataList() same time newData[%d, %d] areaid[%s] order_time[%s] datetime[%s]\n",
                                newData->posX, newData->posY, newData->areaid.c_str(), newData->order_time.c_str(), newData->datetime.c_str());
                            delete newData;
                        }
                        else
                        {
                            FootprintList->dataList->push_back(newData);
                        }
                    }
                    else
                    {
                        FootprintList->dataList->push_back(newData);
                    }
                }
            }
        }
        catch(...)
        {
            vilslog::printf("FootprintDBAccess::findNodeFootprintDataList() Json parse failed!\n");
            return FootprintList;
        }
    }

    return FootprintList;
}

int FootprintDBAccess::write_node_footprint(const char * macaddress, const char * targetDate, const char * targetTime, const char * areaid,
    int maplayer, int posX, int posY, int posZ, const char * datetime)
{
    int ret = 0;

    std::string dbkey = macaddress;
    dbkey.append("_");
    dbkey.append(targetDate);

    //
    // find list existed!
    //
    FootprintListStruct * nodeFootprintList = getNodeFootprintList(macaddress, targetDate);
    if ( nodeFootprintList == NULL )
    {
        vilslog::printf("FootprintDBAccess::write_node_footprint() nodeFootprintList == NULL macaddress[%s]\n", macaddress);
        return 0;
    }

    if (nodeFootprintList->dataList->size() > 0 )
    {
        if ( nodeFootprintList->dataList->back()->order_time.compare(targetTime) == 0)
        {
            // vilslog::printf("FootprintDBAccess::write_node_footprint() same time macaddress[%s] targetTime[%s]\n", macaddress, targetTime);
            return 0;
        }
    }

    FootprintDataStruct * newData = new FootprintDataStruct();
    newData->maplayer = maplayer;
    newData->posX = posX;
    newData->posY = posY;
    newData->areaid = areaid;
    newData->datetime = datetime;
    newData->order_time = targetTime;
    nodeFootprintList->dataList->push_back(newData);

    if ( (nodeFootprintList->max_time.length() == 0) || (nodeFootprintList->max_time.compare(datetime) <= 0) )
        nodeFootprintList->max_time = datetime;
    if ( (nodeFootprintList->min_time.length() == 0) || (nodeFootprintList->min_time.compare(datetime) >= 0) )
        nodeFootprintList->min_time = datetime;

    // vilslog::printf("FootprintDBAccess::write_node_footprint() macaddress[%s] dataList->size()[%d] max_time[%s] min_time[%s]\n",
    //     macaddress, nodeFootprintList->dataList->size(), nodeFootprintList->max_time.c_str(), nodeFootprintList->min_time.c_str());
/*
    std::string dbdata = convertFootprintDataListToString(nodeFootprintList->dataList);

    std::string cmd;
    cmd.append("INSERT INTO Footprint(dbkey, dbdata) VALUES ('");
    cmd.append(dbkey);
    cmd.append("', '");
    cmd.append(dbdata);
    cmd.append("') ON DUPLICATE KEY UPDATE dbdata=VALUES(dbdata)");
    // sprintf(cmd, "INSERT INTO Footprint(dbkey, dbdata) VALUES ('%s', '%s') ON DUPLICATE KEY UPDATE dbdata=VALUES(dbdata)",
    //     dbkey.c_str(), dbdata.c_str());

    ret = sql_execute(cmd.c_str());
*/
    // vilslog::printf("FootprintDBAccess::write_node_footprint() ret[%d]\n", ret);

    return ret;
}

int FootprintDBAccess::Transaction_write_node_footprint(const char * macaddress, const char * targetDate, const char * targetTime, const char * areaid,
    int maplayer, int posX, int posY, int posZ, const char * datetime)
{
    int ret = 0;

    std::string dbkey = macaddress;
    dbkey.append("_");
    dbkey.append(targetDate);

    //
    // find list existed!
    //
    FootprintListStruct * nodeFootprintList = getNodeFootprintList(macaddress, targetDate);
    if ( nodeFootprintList == NULL )
    {
        vilslog::printf("FootprintDBAccess::write_node_footprint() nodeFootprintList == NULL macaddress[%s]\n", macaddress);
        return 0;
    }

    FootprintDataStruct * newData = new FootprintDataStruct();
    newData->maplayer = maplayer;
    newData->posX = posX;
    newData->posY = posY;
    newData->areaid = areaid;
    newData->datetime = datetime;
    newData->order_time = targetTime;
    nodeFootprintList->dataList->push_back(newData);

    if ( (nodeFootprintList->max_time.length() == 0) || (nodeFootprintList->max_time.compare(datetime) <= 0) )
        nodeFootprintList->max_time = datetime;
    if ( (nodeFootprintList->min_time.length() == 0) || (nodeFootprintList->min_time.compare(datetime) >= 0) )
        nodeFootprintList->min_time = datetime;

    // vilslog::printf("FootprintDBAccess::write_node_footprint() macaddress[%s] dataList->size()[%d] max_time[%s] min_time[%s]\n",
    //     macaddress, nodeFootprintList->dataList->size(), nodeFootprintList->max_time.c_str(), nodeFootprintList->min_time.c_str());

    std::string dbdata = convertFootprintDataListToString(nodeFootprintList->dataList);

    std::string cmd;
    cmd.append("INSERT INTO Footprint(dbkey, dbdata) VALUES ('");
    cmd.append(dbkey);
    cmd.append("', '");
    cmd.append(dbdata);
    cmd.append("') ON DUPLICATE KEY UPDATE dbdata=VALUES(dbdata)");
    // sprintf(cmd, "INSERT INTO Footprint(dbkey, dbdata) VALUES ('%s', '%s') ON DUPLICATE KEY UPDATE dbdata=VALUES(dbdata)",
    //     dbkey.c_str(), dbdata.c_str());

    std::string trans = cmd;
    mMysqlTransactionQueue.add(trans);
    // vilslog::printf("FootprintDBAccess::write_node_footprint() ret[%d]\n", ret);

    return ret;
}

int FootprintDBAccess::Transaction_commit()
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
        //delete stmt;
        //delete con;
    }
    catch (sql::SQLException &e) {
        vilslog::printf("FootprintDBAccess::Transaction_commit() sql::SQLException\n");

        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;

        ret = -1;
    }

    return ret;

}

int FootprintDBAccess::save_to_db()
{
    for (std::list<FootprintListStruct *>::iterator nodeFP = mAllFootprintDataList.begin(); nodeFP != mAllFootprintDataList.end(); nodeFP++)
    {
        std::string dbkey = (*nodeFP)->macaddress;
        dbkey.append("_");
        dbkey.append((*nodeFP)->order_day);

        std::string dbdata = convertFootprintDataListToString((*nodeFP)->dataList);

        std::string cmd;
        cmd.append("INSERT INTO Footprint(dbkey, dbdata) VALUES ('");
        cmd.append(dbkey);
        cmd.append("', '");
        cmd.append(dbdata);
        cmd.append("') ON DUPLICATE KEY UPDATE dbdata=VALUES(dbdata)");
        // sprintf(cmd, "INSERT INTO Footprint(dbkey, dbdata) VALUES ('%s', '%s') ON DUPLICATE KEY UPDATE dbdata=VALUES(dbdata)",
        //     dbkey.c_str(), dbdata.c_str());

        sql_execute(cmd.c_str());

    }
    return 0;
}

int FootprintDBAccess::clear_old_data()
{
    std::string todayDate = TodayDateType1();
    int count = 0;
    for (std::list<FootprintListStruct *>::iterator nodeFP = mAllFootprintDataList.begin(); nodeFP != mAllFootprintDataList.end(); nodeFP++)
    {
        if ( todayDate.compare((*nodeFP)->order_day) != 0 )
        {
            vilslog::printf("FootprintDBAccess::clear_old_data() mProjectID[%d] macaddress[%s] order_day[%s]\n", mProjectID, (*nodeFP)->macaddress.c_str(), (*nodeFP)->order_day.c_str());

            // remove from list
            FootprintListStruct * old_fp = (*nodeFP);
            nodeFP = mAllFootprintDataList.erase(nodeFP);
            delete old_fp;
            count++;

            if ( (*nodeFP) == NULL)
            {
                vilslog::printf("FootprintDBAccess::clear_old_data() mProjectID[%d] nodeFP == NULL\n", mProjectID);
                break;
            }
        }
    }

    vilslog::printf("FootprintDBAccess::clear_old_data() mProjectID[%d] remove count[%d]\n", mProjectID, count);

    return count;
}

FootprintListStruct * FootprintDBAccess::getNodeFootprintList(const char * macaddress, const char * targetDate)
{
    FootprintListStruct * nodeFootprintList = NULL;
    for (std::list<FootprintListStruct *>::iterator nodeFP = mAllFootprintDataList.begin(); nodeFP != mAllFootprintDataList.end(); nodeFP++)
    {
        if ( ((*nodeFP)->macaddress.compare(macaddress) == 0) && ((*nodeFP)->order_day.compare(targetDate) == 0) )
        {
            nodeFootprintList = (*nodeFP);
            break;
        }
    }

    // vilslog::printf("FootprintDBAccess::getNodeFootprintList() macaddress[%s] targetDate[%s]\n", macaddress, targetDate);

    if ( nodeFootprintList == NULL )
    {
        //nodeFootprintList = new FootprintListStruct();
        //nodeFootprintList->macaddress = macaddress;
        //nodeFootprintList->order_day = targetDate;

        //
        // read from database
        //
        //nodeFootprintList->dataList = findNodeFootprintDataList(macaddress, targetDate);

        //
        // read from database
        //
        nodeFootprintList = findNodeFootprintList(macaddress, targetDate);

        // vilslog::printf("FootprintDBAccess::getNodeFootprintList() read from database macaddress[%s] targetDate[%s] max_time[%s] min_time[%s] order_day[%s] dataList->size[%d]\n",
        //     macaddress, targetDate, nodeFootprintList->max_time.c_str(), nodeFootprintList->min_time.c_str(),
        //     nodeFootprintList->order_day.c_str(), (int)nodeFootprintList->dataList->size());

        mAllFootprintDataList.push_back(nodeFootprintList);
    }

    return nodeFootprintList;
}

std::string FootprintDBAccess::convertFootprintDataListToString(std::list<FootprintDataStruct *> * dataList)
{
    std::ostringstream outputstream;

    outputstream<<"[";
    for (std::list<FootprintDataStruct *>::iterator data = dataList->begin(); data != dataList->end(); data++)
    {
        if(data != dataList->begin())
            outputstream<<",";
        outputstream<<"{\"p\":\""<<(*data)->posX <<","<<(*data)->posY <<"\",";
        outputstream<<"\"m\":\""<<(*data)->maplayer<<"\",";
        if ((*data)->areaid.length() > 0)
            outputstream<<"\"a\":\""<<(*data)->areaid<<"\",";
        outputstream<<"\"t\":\""<<(*data)->order_time<<"\"}";

    }
    outputstream<<"]";

    std::string dbdata = outputstream.str();

    // vilslog::printf("FootprintDBAccess::convertFootprintDataListToString() dataList->size()[%d] dbdata.length()[%d]\n",
    //     (int)dataList->size(), dbdata.length());

    return dbdata;
}

int FootprintDBAccess::sql_execute(const char * cmd)
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

bool FootprintDBAccess::sql_executeQuery_new(const char * cmd, CALLBACK callback, void * retValue)
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
        vilslog::printf("FootprintDBAccess::sql_executeQuery_new() sql::SQLException\n");

        cout << "FootprintDBAccess::sql_executeQuery_new()  # ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
        bRet = false;
    }

    return bRet;
}

bool FootprintDBAccess::checkDBExist()
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
        vilslog::printf("FootprintDBAccess::sql_executeQuery_new() sql::SQLException\n");

        cout << "FootprintDBAccess::checkDBExist() # ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;

        return false;
    }

    return true;
}

int FootprintDBAccess::create_DB()
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


        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        stmt->close();
    } catch (sql::SQLException &e) {
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

    cout << "FootprintDBAccess::create_DB() Successfully ended" << endl;
    return 0;
}

int FootprintDBAccess::create_table()
{
    vilslog::printf("FootprintDBAccess::create_table() Started\n");

    bool bRet = create_Footprint_table();

    if (bRet)
        vilslog::printf("FootprintDBAccess::create_table() Successfully ended!\n");
    else
        vilslog::printf("FootprintDBAccess::create_table() Failed! ended!\n");

    return 0;
}

bool FootprintDBAccess::check_Footprint_table()
{
    if (tableExists("Footprint"))
    {
        //createNonExistColumn("FootprintAll", "date", "CHAR(30)");
    }
    else
    {
        create_Footprint_table();
    }
    return true;
}

bool FootprintDBAccess::create_Footprint_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        // dbkey : macaddress_2020:02:24
        // dbdata : JSON format

        stmt->execute("CREATE TABLE Footprint( \
            dbid INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
            dbkey VARCHAR(100) NOT NULL, \
            dbdata MEDIUMTEXT NOT NULL, \
            datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
            UNIQUE KEY unique_dbkey (dbkey) ) \
            ENGINE=INNODB ROW_FORMAT=COMPRESSED DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci ");

        stmt->close();
    } catch (sql::SQLException &e) {
        cout << "# ERR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << " )" << endl;
    }

    return true;
}

bool FootprintDBAccess::tableExists(const char * tableName)
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

bool FootprintDBAccess::createNonExistColumn(const char * tableName, const char * columnName, const char * colDefinition)
{
    bool bExist = columnExists(tableName, columnName);
    if (!bExist)
        create_column(tableName, columnName, colDefinition);
    return bExist;
}

bool FootprintDBAccess::columnExists(const char * tableName, const char * columnName)
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

int FootprintDBAccess::create_column(const char * tableName, const char * columnName, const char * colDefinition)
{
    char cmd[4096];
    sprintf(cmd, "ALTER TABLE %s ADD %s %s", tableName, columnName, colDefinition);
    int res = sql_execute(cmd);
    return res;
}

int FootprintDBAccess::get_datacount(const char * tableName)
{
    char cmd[4096];
    sprintf(cmd, "SELECT count(*) FROM %s", tableName);

    int count = 0;
    sql_executeQuery_new(cmd, get_count_callback, &count);

    return count;
}

int FootprintDBAccess::get_count_callback(sql::ResultSet * res, void * pvalue)
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

int FootprintDBAccess::get_FootprintRecord_callback(sql::ResultSet * res, void * pvalue)
{
    FootprintRecordStruct * dayInfo = (FootprintRecordStruct *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            dayInfo->dbdata = res->getString("dbdata");
            dayInfo->datetime = res->getString("datetime");

            count++;
        }
    }
   //printf("FootprintDBAccess::get_FootprintRecord_callback() count[%d]\n", *count);

   return count;
}
