/*
 * MySQLDBAccess.cpp
 *
 *  Created on: 2017/05/19
 *      Author: ikki
 */


#include <stdlib.h>
#include <iostream>
#include <sstream>

#include "MySQLDBAccess.h"
#include "../json/json.h"
#include "../common.h"

using namespace std;

//
// AnchorRangingDataClass
//
MySQLDBAccess::MySQLDBAccess(int projectID)
{
    mProjectID = projectID;
    init();
}

MySQLDBAccess::~MySQLDBAccess()
{
}

int MySQLDBAccess::init()
{
    mMySQLDriver = get_driver_instance();

    if (mProjectID == 1)
    {
        mSQLDBname = sqldbname;
    }
    else
    {
        mSQLDBname = sqldbname;
        mSQLDBname.append("_");
        mSQLDBname.append(std::to_string(mProjectID));
    }

    if (!checkMySQLConnection())
    {
        vilslog::printf("MySQLDBAccess::init() checkMySQLConnection() falied!\n");
        return 1;
    }

    vilslog::printf("MySQLDBAccess::init() checkDBExist() \n");


    if (!checkDBExist())
    {
        vilslog::printf("MySQLDBAccess::init() database %s not exist!\n", mSQLDBname.c_str());

        create_table();

        create_FiredEvents_table();

        create_resident_table();
        create_employee_table();
        create_UserGroup_table();
        create_UserSubGroup_table();
        create_UserMainGroup_table();
        // create_UserMainGroupConnection_table();

        create_User_table();

        create_SectionGroup_table();
        create_AlertRecord_table();
        // create_config_table();
        create_systemconfig_table();
        // create_MapArea_table();
        create_InspectionGroup_table();
        create_InspectionPoint_table();
        create_InspectionGroupDailyRecord_table();
        create_InspectionPointDailyRecord_table();
        create_InspectorBehaviorData_table();
        create_AreaInOutRecord_table();

        create_Course_table();
        create_CourseRollCallRecord_table();
        create_GPSInfoRecord_table();
        create_LoRaGatewayInfo_table();
        create_EmployeeEventReport_table();
        create_PushNotifyUser_table();
        create_maplayer_table();
        create_maplayerarea_table();
        create_PushNotifyMessage_table();
        create_PushNotifyMessageRecord_table();
        create_CannedMessage_table();
        create_PushNotifySetting_table();
        create_TriggerPushMessage_table();
        create_AoANodeInfo_table();
        create_BodyInfo_table();

        create_User_data(1, 1, "smims", "北瀚科技", "北瀚科技", "工程師", "1", "smims2017");
        create_User_data(2, 3, "user", "user", "生活管理部", "生活管家", "1", "user");
        create_User_data(3, 1, "demo", "Demo帳號", "北瀚科技", "demo", "1", "demo");

        create_UserGroup_data(1, "工程人員", "能看全部網頁");
        create_UserGroup_data(2, "Power User", "能看全部網頁");
        create_UserGroup_data(3, "普通用戶", "只能看DI, 不能看網頁");
        create_UserGroup_data(4, "工作人員", "能看部份網頁, 無法編輯(坐標..etc), 可以發送推播, 可以記錄生理量測, 可以新增課程表");
        create_UserGroup_data(5, "子集合管理員", "可以操作推播, 待辦事項, 請假單, 團體活動表, 班級功課表, 檔案庫");
        create_UserGroup_data(6, "主集合管理員", "擁有子集合管理員權限, 可新增/刪除/編輯子集合");

        create_UserMainGroup_data("北瀚科技", "北瀚科技公司主集合");
        // create_UserMainGroup_data("測試", "測試主集合");
        create_UserSubGroup_data("北瀚科技", "北瀚科技公司子集合");

        create_FenceAlert_table();
        create_DeviceMaintenanceRecord_table();

        create_Survey_table();
        create_SurveyItem_table();
        create_SurveyEventRecord_table();
        // create_SurveyRecord_table();

        create_Survey_data(ApplicationForm_LEAVE_SURVEYID, "請假單簽核", "請假單簽核", 0, 1, "", "99999");
        create_Survey_data(ApplicationForm_ASSIST_ID, "協助事項簽核", "協助事項簽核", 0, 1, "", "99999");
        create_Survey_data(ApplicationForm_ASSISTEVENT_ID, "工作指示回報", "請回報完成狀況", 0, 1, "", "99999");
        create_SurveyItem_data(ApplicationForm_LEAVE_SURVEYID, "簽核中");
        create_SurveyItem_data(ApplicationForm_LEAVE_SURVEYID, "核准");
        create_SurveyItem_data(ApplicationForm_LEAVE_SURVEYID, "不核准");
        create_SurveyItem_data(ApplicationForm_ASSIST_ID, "簽核中");
        create_SurveyItem_data(ApplicationForm_ASSIST_ID, "核准");
        create_SurveyItem_data(ApplicationForm_ASSIST_ID, "不核准");
        create_SurveyItem_data(ApplicationForm_ASSISTEVENT_ID, "已完成");
        create_SurveyItem_data(ApplicationForm_ASSISTEVENT_ID, "未完成");

        create_SurveyResult_table();
        create_PushNotifyMessageStatus_table();
        create_TargetTopic_table();

        create_RFIDDevice_table();
        create_RFIDCard_table();
        create_CARDRollCall_table();
        create_CARDRollCallEvent_table();
        create_CARDRollCallSubGroupEvent_table();
        create_CARDRollCallRecord_table();
        create_CARDScanRecord_table();

        create_NFCDevice_table();
        create_NFCCard_table();
        create_MaingroupNFCCard_table();
        create_EDGEDevice_table();

        create_uiblock_table();
        create_subgroupuiblock_table();
        create_weekschedule_table();

        create_StorageFile_table();
        create_StorageImage_table();
        // create_PhotoAlbumImage_table();

        create_applicationformleave_table();

        create_logintoken_table();

        create_assist_table();
        create_assistEvent_table();
        create_assistEventRecord_table();

        create_billboardRecord_table();

        create_register_table();

        create_Curriculum_table();
        // create_CurriculumShared_table();
        create_CurriculumRecord_table();
        create_CurriculumType_table();
        create_CurriculumApply_table();
        create_CurriculumPrefix_table();

        create_sensorinfo_table();
        create_sensordevice_table();
        create_sensorevent_table();

        create_FlyInfo_table();
        create_APIKey_table();

        create_KIAWCInfo_table();
        create_FireFighting_table();
        create_FireFightingUser_table();
        create_FireFightingEvent_table();
        create_FireFightingAirUsing_table();

        create_SoccerRecord_table();
    }

    static int flag = 0;
    if (flag == 0)
    {
        char cmd[4096];
        memset(cmd, 0, 4096);

        //create_table();

        flag = 1;
    }
    return 0;
}

int MySQLDBAccess::check_valid_table()
{
    // check_config_table();
    check_FiredEvents_table();
    check_SectionGroup_table();

    check_systemconfig_table();
    check_InspectionGroup_table();
    check_InspectionPoint_table();
    check_InspectionGroupDailyRecord_table();
    check_InspectionPointDailyRecord_table();
    check_InspectorBehaviorData_table();
    check_NodeInfo_table();
    check_FenceAlert_table();
    check_DeviceMaintenanceRecord_table();

    check_Course_table();

    check_maplayer_table();
    check_maplayerarea_table();
    check_AreaInOutRecord_table();

    check_Survey_table();
    check_SurveyItem_table();
    check_SurveyEventRecord_table();
    // check_SurveyRecord_table();
    check_SurveyResult_table();

    check_PushNotifyMessage_table();
    check_PushNotifyMessageRecord_table();
    check_CannedMessage_table();

    check_PushNotifyMessageStatus_table();
    check_TargetTopic_table();
    check_TriggerPushMessage_table();

    create_UserGroup_data(1, "工程人員", "能看全部網頁");
    create_UserGroup_data(2, "Power User", "能看全部網頁");
    create_UserGroup_data(3, "普通用戶", "只能看DI, 不能看網頁");
    create_UserGroup_data(4, "工作人員", "能看部份網頁, 無法編輯(坐標..etc), 可以發送推播, 可以記錄生理量測, 可以新增課程表");
    create_UserGroup_data(5, "子集合管理員", "可以操作推播, 待辦事項, 請假單, 團體活動表, 班級功課表, 檔案庫");
    create_UserGroup_data(6, "主集合管理員", "擁有子集合管理員權限, 可新增/刪除/編輯子集合");

    // delete_UserGroup_data(5);

    check_UserSubGroup_table();
    check_UserMainGroup_table();
    // check_UserMainGroupConnection_table();

    check_User_table();

    check_CARDRollCall_table();
    check_CARDRollCallEvent_table();
    check_CARDRollCallSubGroupEvent_table();
    check_CARDRollCallRecord_table();
    check_CARDScanRecord_table();

    check_RFIDDevice_table();
    check_RFIDCard_table();
    check_NFCDevice_table();
    check_NFCCard_table();
    check_MaingroupNFCCard_table();
    check_EDGEDevice_table();

    add_canned_message(1, 204, "管理室通知1(工作指示)", "管理室通知", "管理室通知待辦事項，請完成後回報!");
    add_canned_message(2, 204, "管理室通知2(工作指示)", "管理室通知", "請同仁完成並互相回報.");
    add_canned_message(3, 201, "聯絡簿", "聯絡簿", "請同學家長回報.");
    add_canned_message(4, 202, "師生交流", "師生交流", "請同學家長提供意見");
    add_canned_message(5, 203, "電子投票", "電子投票", "請同仁完成投票");
    add_canned_message(6, 204, "工作指示", "工作指示", "請同仁完成並互相回報.");
    add_canned_message(7, 205, "不分類", "不分類訊息", "請詳閱訊息");

    check_uiblock_table();
    check_subgroupuiblock_table();
    check_weekschedule_table();

    check_StorageFile_table();
    check_StorageImage_table();
    // check_PhotoAlbumImage_table();

    check_applicationformleave_table();
    check_BodyInfo_table();

    check_LoRaGatewayInfo_table();

    check_AoANodeInfo_table();
    check_PushNotifySetting_table();

    check_logintoken_table();

    check_assist_table();
    check_assistEvent_table();
    check_assistEventRecord_table();

    check_billboardRecord_table();
    check_register_table();

    check_Curriculum_table();
    // check_CurriculumShared_table();
    check_CurriculumRecord_table();
    check_CurriculumType_table();
    check_CurriculumApply_table();
    check_CurriculumPrefix_table();

    check_sensorinfo_table();
    check_sensordevice_table();
    check_sensorevent_table();

    check_FlyInfo_table();
    check_APIKey_table();

    check_KIAWCInfo_table();
    check_FireFighting_table();
    check_FireFightingUser_table();
    check_FireFightingEvent_table();
    check_FireFightingAirUsing_table();

    check_SoccerRecord_table();

    add_APIKey(1, 1, "JtrXGqk1UE", "國泰建設");
    add_APIKey(1, 1, "1ecOtRFkzt", "主集合 eric.ting@compasschartered.com");
    add_APIKey(1, 1, "sotA6cY54U", "小港機場");

    check_GPSInfoRecord_table();

    // create_UserMainGroup_data("北瀚科技", "北瀚科技公司主集合");

    /*
    create_Survey_data(ApplicationForm_LEAVE_SURVEYID, "請假單簽核", "請假單簽核", "smims", 0, "99999");
    create_SurveyItem_data(ApplicationForm_LEAVE_SURVEYID, "簽核中");
    create_SurveyItem_data(ApplicationForm_LEAVE_SURVEYID, "核准");
    create_SurveyItem_data(ApplicationForm_LEAVE_SURVEYID, "不核准");

    create_Survey_data(ApplicationForm_ASSIST_ID, "協助事項簽核", "協助事項簽核", "smims", 0, "99999");
    create_SurveyItem_data(ApplicationForm_ASSIST_ID, "簽核中");
    create_SurveyItem_data(ApplicationForm_ASSIST_ID, "核准");
    create_SurveyItem_data(ApplicationForm_ASSIST_ID, "不核准");

    create_Survey_data(ApplicationForm_ASSISTEVENT_ID, "工作指示回報", "請回報完成狀況", "smims", 0, "99999");
    create_SurveyItem_data(ApplicationForm_ASSISTEVENT_ID, "已完成");
    create_SurveyItem_data(ApplicationForm_ASSISTEVENT_ID, "未完成");
    */


    /*
    convert_utf8_to_utf8mb4("config");
    convert_utf8_to_utf8mb4("NodeInfo");
    */

    return 0;
}

int MySQLDBAccess::deletedb()
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "DROP DATABASE IF EXISTS %s", mSQLDBname.c_str());
    sql_execute(cmd);
    return 0;
}

int MySQLDBAccess::getProjectID()
{
    return mProjectID;
}

int MySQLDBAccess::getDatabaseSize()
{
    int size = 0;

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT table_schema AS 'Database', ROUND(SUM(data_length + index_length) / 1024 / 1024, 2) AS 'size' \
    FROM information_schema.TABLES GROUP BY table_schema;");

    sql_executeQuery_new(cmd, get_size_callback, &size);

    return size;
}

bool MySQLDBAccess::checkMySQLConnection()
{
    try {
        //sql::Driver *driver;
        //sql::Connection *con;
        //sql::Statement *stmt;

        /* Create a connection */
        //driver = get_driver_instance();

        vilslog::printf("MySQLDBAccess::Server->%s \n", MySQLDB_server);
        vilslog::printf("MySQLDBAccess::username->%s \n", MySQLDB_username);
        vilslog::printf("MySQLDBAccess::password->%s \n", MySQLDB_password);

        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        //con = driver->connect(MySQLDB_server, username, password); //IP Address, user name, password
        //con = mMySQLDriver->connect("tcp://localhost:3306", MySQLDB_username, MySQLDB_password); //IP Address, user name, password
        //delete con;
    } catch (sql::SQLException &e) {
        printSQLException(e);

        return false;
    }

    return true;
}

bool MySQLDBAccess::clear_old_data(const char * table, const char * column, const char * datetime)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "DELETE FROM %s WHERE %s <= '%s';",
        table, column, datetime);

    int ret = sql_execute(cmd);

    vilslog::printf("MySQLDBAccess::clear_old_data() cmd[%s] ret[%d]\n", cmd, ret);

    if (ret == 0)
        return true;
    else
        return false;
}

int MySQLDBAccess::write_node_footprint(
    int nodetype, const char * nodeid, int idx, const char * areaid,
    int posX, int posY, int posZ, const char * voltage, const char * temperature)
{
    int ret = 0;

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO Footprint(nodetype, nodeid, idx, areaid, posX, posY, posZ, voltage, temperature) \
                    VALUES (%d, '%s', %d, '%s', %d, %d, %d, '%s', '%s') \
                    ON DUPLICATE KEY UPDATE idx=VALUES(idx), areaid=VALUES(areaid), posX=VALUES(posX), posY=VALUES(posY), posZ=VALUES(posZ), \
                    voltage=VALUES(voltage), temperature=VALUES(temperature)",
            nodetype, nodeid, idx, areaid, posX, posY, posZ, voltage, temperature);

    //printf(cmd);
    //printf("\nstrlen(cmd)=%d\n",strlen(cmd));

    ret = sql_execute(cmd);

    return ret;
}

int MySQLDBAccess::Transaction_write_node_footprint(
                                        int nodetype, const char * nodeid, int idx, const char * areaid,
                                        int posX, int posY, int posZ, const char * voltage, const char * temperature)
{
    int ret = 0;

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO Footprint(nodetype, nodeid, idx, areaid, posX, posY, posZ, voltage, temperature) \
            VALUES (%d, '%s', %d, '%s', %d, %d, %d, '%s', '%s') \
            ON DUPLICATE KEY UPDATE idx=VALUES(idx), areaid=VALUES(areaid), posX=VALUES(posX), posY=VALUES(posY), posZ=VALUES(posZ), \
            voltage=VALUES(voltage), temperature=VALUES(temperature)",
            nodetype, nodeid, idx, areaid, posX, posY, posZ, voltage, temperature);

    std::string trans = cmd;
    mMysqlTransactionQueue.add(trans);

    return ret;
}

int MySQLDBAccess::Transaction_commit()
{
    int ret = 0;
    std::string trans = "";

    if (mMysqlTransactionQueue.size() == 0)
    {
        return ret;
    }

    try {

        // Create a connection
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        con->setAutoCommit(true);

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        stmt->execute("START TRANSACTION");

        while (mMysqlTransactionQueue.size() > 0)
        {
            trans = mMysqlTransactionQueue.remove();

            // vilslog::printf ("MySQLDBAccess::Transaction_commit() trans[%s] \n", trans.c_str());

            stmt->execute(trans.c_str());
        }
        stmt->execute("COMMIT");

        stmt->close();
    }
    catch (sql::SQLException &e) {
        vilslog::printf("MySQLDBAccess::Transaction_commit() sql::SQLException\n");
        vilslog::printf("trans=[%s]\n", trans.c_str());

        printSQLException(e);

        ret = -1;
    }

    return ret;
}

std::list<FootprintStruct*> * MySQLDBAccess::read_node_footprint(const char * nodeid, int max_size)
{
    std::list<FootprintStruct*> * ftList = new std::list<FootprintStruct*>();

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM Footprint where nodeid='%s' ORDER BY datetime DESC LIMIT %d",
            nodeid, max_size);

    sql_executeQuery_new(cmd, get_footprintlist_callback, ftList);

    return ftList;
}

int MySQLDBAccess::write_node_info(NodeStruct * node)
{
    int ret = 0;

    delete_node_info(node->nodetype, node->index);

    std::string poweronTimestamp = time_tToString(node->poweronTimestamp);
    std::string poweroffTimestamp = time_tToString(node->poweroffTimestamp);

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO NodeInfo(nodetype, macaddress, LoRAMacaddress, nodeid, tafid, buildingid, buildingfloor, \
            nodename, idx, posX, posY, posZ, Longitude, Latitude, \
            voltage, temperature, version, AreaId, AreaName, \
            blacklist, whitelist, joinRangingRange, joinNotRangingRange, resident_serno, empid, panid, opmode, gwid, alive, \
            maplayer, poweronTimestamp, poweroffTimestamp, enableSensorAlert, userid, IP, maingroupid, verifykey) \
            VALUES (%d, '%s', '%s', '%s', %d, %d, %d, '%s', %d, %d, %d, %d, '%f', '%f', '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d, %d, '%s', '%s', '%s', %d, '%s', %d, \
            %d, '%s', '%s', %d, %d, '%s', %d, '%s') \
            ON DUPLICATE KEY UPDATE nodeid=VALUES(nodeid), LoRAMacaddress=VALUES(LoRAMacaddress), tafid=VALUES(tafid), buildingid=VALUES(buildingid), buildingfloor=VALUES(buildingfloor), \
            nodename=VALUES(nodename), idx=VALUES(idx), \
            posX=VALUES(posX), posY=VALUES(posY), posZ=VALUES(posZ), Longitude=VALUES(Longitude), Latitude=VALUES(Latitude), \
            voltage=VALUES(voltage), temperature=VALUES(temperature), version=VALUES(version), AreaId=VALUES(AreaId), AreaName=VALUES(AreaName), \
            blacklist=VALUES(blacklist), whitelist=VALUES(whitelist), joinRangingRange=VALUES(joinRangingRange), \
            joinNotRangingRange=VALUES(joinNotRangingRange), resident_serno=VALUES(resident_serno), empid=VALUES(empid), \
            panid=VALUES(panid), opmode=VALUES(opmode), gwid=VALUES(gwid), alive=VALUES(alive), \
            maplayer=VALUES(maplayer), poweronTimestamp=VALUES(poweronTimestamp), poweroffTimestamp=VALUES(poweroffTimestamp), \
            enableSensorAlert=VALUES(enableSensorAlert), userid=VALUES(userid), IP=VALUES(IP), maingroupid=VALUES(maingroupid), verifykey=VALUES(verifykey)",
            node->nodetype, node->macaddress, node->LoRAMacaddress, node->nodeid, node->tafid, node->buildingid, node->buildingfloor,
            node->nodename, node->index, (int)node->posX, (int)node->posY, (int)node->posZ, node->Longitude, node->Latitude,
            node->voltage, node->temperature, node->version, node->AreaId, node->AreaName,
            node->blacklist.c_str(), node->whitelist.c_str(), node->joinRangingRange, node->joinNotRangingRange, node->resident_serno.c_str(),
            node->empid.c_str(), node->panid, node->Opertaion_Mode, node->gwid.c_str(), node->alive,
            node->maplayer, poweronTimestamp.c_str(), poweroffTimestamp.c_str(), node->enableSensorAlert, node->userid, node->IP, node->maingroupid, node->verifykey.c_str());

    ret = sql_execute(cmd);

    return ret;
}

int MySQLDBAccess::Transaction_write_node_info(NodeStruct * node)
{
    int ret = 0;

    Transaction_delete_node_info(node->nodetype, node->index);

    std::string poweronTimestamp = time_tToString(node->poweronTimestamp);
    std::string poweroffTimestamp = time_tToString(node->poweroffTimestamp);

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO NodeInfo(nodetype, macaddress, LoRAMacaddress, nodeid, tafid, buildingid, buildingfloor, \
            nodename, idx, posX, posY, posZ, Longitude, Latitude, \
            voltage, temperature, version, AreaId, AreaName, \
            blacklist, whitelist, joinRangingRange, joinNotRangingRange, resident_serno, empid, panid, opmode, gwid, alive, \
            maplayer, poweronTimestamp, poweroffTimestamp, enableSensorAlert, userid, IP, maingroupid, verifykey) \
            VALUES (%d, '%s', '%s', '%s', %d, %d, %d, '%s', %d, %d, %d, %d, '%f', '%f', '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d, %d, '%s', '%s', '%s', %d, '%s', %d, \
            %d, '%s', '%s', %d, %d, '%s', %d, '%s') \
            ON DUPLICATE KEY UPDATE nodeid=VALUES(nodeid), LoRAMacaddress=VALUES(LoRAMacaddress), tafid=VALUES(tafid), buildingid=VALUES(buildingid), buildingfloor=VALUES(buildingfloor), \
            nodename=VALUES(nodename), idx=VALUES(idx), \
            posX=VALUES(posX), posY=VALUES(posY), posZ=VALUES(posZ), Longitude=VALUES(Longitude), Latitude=VALUES(Latitude), \
            voltage=VALUES(voltage), temperature=VALUES(temperature), version=VALUES(version), AreaId=VALUES(AreaId), AreaName=VALUES(AreaName), \
            blacklist=VALUES(blacklist), whitelist=VALUES(whitelist), joinRangingRange=VALUES(joinRangingRange), \
            joinNotRangingRange=VALUES(joinNotRangingRange), resident_serno=VALUES(resident_serno), empid=VALUES(empid), \
            panid=VALUES(panid), opmode=VALUES(opmode), gwid=VALUES(gwid), alive=VALUES(alive), \
            maplayer=VALUES(maplayer), poweronTimestamp=VALUES(poweronTimestamp), poweroffTimestamp=VALUES(poweroffTimestamp), \
            enableSensorAlert=VALUES(enableSensorAlert), userid=VALUES(userid), IP=VALUES(IP), maingroupid=VALUES(maingroupid), verifykey=VALUES(verifykey)",
            node->nodetype, node->macaddress, node->LoRAMacaddress, node->nodeid, node->tafid, node->buildingid, node->buildingfloor,
            node->nodename, node->index, (int)node->posX, (int)node->posY, (int)node->posZ, node->Longitude, node->Latitude,
            node->voltage, node->temperature, node->version, node->AreaId, node->AreaName,
            node->blacklist.c_str(), node->whitelist.c_str(), node->joinRangingRange, node->joinNotRangingRange, node->resident_serno.c_str(),
            node->empid.c_str(), node->panid, node->Opertaion_Mode, node->gwid.c_str(), node->alive,
            node->maplayer, poweronTimestamp.c_str(), poweroffTimestamp.c_str(), node->enableSensorAlert, node->userid, node->IP, node->maingroupid, node->verifykey.c_str());

    //vilslog::printf(cmd);
    //printf("\nstrlen(cmd)=%d\n",strlen(cmd));

    std::string trans = cmd;
    mMysqlTransactionQueue.add(trans);

    return ret;
}

int MySQLDBAccess::write_AOA_node_info(AoANodeStruct * node)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO AoANodeInfo(nodeid, nodename, panid, macaddress, maplayer, posX, posY, posZ, pitch, roll, yaw, gridsize, maingroupid, verifykey) \
        VALUES ('%s', '%s', '%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s') \
        ON DUPLICATE KEY UPDATE nodename=VALUES(nodename), panid=VALUES(panid), macaddress=VALUES(macaddress), maplayer=VALUES(maplayer), \
        posX=VALUES(posX), posY=VALUES(posY), posZ=VALUES(posZ), pitch=VALUES(pitch), roll=VALUES(roll), yaw=VALUES(yaw), gridsize=VALUES(gridsize), maingroupid=VALUES(maingroupid), verifykey=VALUES(verifykey) ",
        node->nodeid, node->nodename, node->panid, node->macaddress, node->maplayer,
        node->posX, node->posY, node->posZ, node->pitch, node->roll, node->yaw, node->gridsize, node->maingroupid, node->verifykey.c_str());

    ret = sql_execute(cmd);

    return ret;
}

int MySQLDBAccess::Transaction_write_AOA_node_info(AoANodeStruct * node)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO AoANodeInfo(nodeid, nodename, panid, macaddress, maplayer, posX, posY, posZ, pitch, roll, yaw, gridsize, maingroupid, verifykey) \
        VALUES ('%s', '%s', '%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s') \
        ON DUPLICATE KEY UPDATE nodename=VALUES(nodename), panid=VALUES(panid), macaddress=VALUES(macaddress), maplayer=VALUES(maplayer), \
        posX=VALUES(posX), posY=VALUES(posY), posZ=VALUES(posZ), pitch=VALUES(pitch), roll=VALUES(roll), yaw=VALUES(yaw), gridsize=VALUES(gridsize), maingroupid=VALUES(maingroupid), verifykey=VALUES(verifykey) ",
        node->nodeid, node->nodename, node->panid, node->macaddress, node->maplayer,
        node->posX, node->posY, node->posZ, node->pitch, node->roll, node->yaw, node->gridsize, node->maingroupid, node->verifykey.c_str());

    std::string trans = cmd;
    mMysqlTransactionQueue.add(trans);

    return ret;
}

int MySQLDBAccess::update_node_info_id(const char * macaddress, const char * nodeid)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE NodeInfo set nodeid='%s' WHERE macaddress='%s';", nodeid, macaddress);

    ret = sql_execute(cmd);
    return ret;
}

int MySQLDBAccess::update_node_info_name(const char * macaddress, const char * nodename)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE NodeInfo set nodename='%s'WHERE macaddress='%s';", nodename, macaddress);

    ret = sql_execute(cmd);
    return ret;
}

int MySQLDBAccess::update_node_info_pos(const char * macaddress, int posX, int posY, int posZ)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE NodeInfo set posX=%d, posY=%d, posZ=%d WHERE macaddress='%s';", posX, posY, posZ, macaddress);

    ret = sql_execute(cmd);
    return ret;
}

int MySQLDBAccess::update_node_info_building(const char * macaddress, int buildingid, int buildingfloor)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE NodeInfo set buildingid=%d, buildingfloor=%d WHERE macaddress='%s';", buildingid, buildingfloor, macaddress);

    ret = sql_execute(cmd);
    return ret;
}

int MySQLDBAccess::update_node_info_blacklist(const char * macaddress, const char * anchorids)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE NodeInfo set blacklist='%s' WHERE macaddress='%s';", anchorids, macaddress);

    ret = sql_execute(cmd);
    return ret;
}

int MySQLDBAccess::update_node_info_whitelist(const char * macaddress, const char * anchorids)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE NodeInfo set whitelist='%s' WHERE macaddress='%s';", anchorids, macaddress);

    //vilslog::printf("MySQLDBAccess::update_node_info_whitelist() cmd[%s]\n", cmd);

    ret = sql_execute(cmd);
    return ret;
}

int MySQLDBAccess::update_node_info_joinRangingRange(const char * macaddress, int joinRangingRange)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE NodeInfo set joinRangingRange=%d WHERE macaddress='%s';", joinRangingRange, macaddress);

    ret = sql_execute(cmd);
    return ret;
}

int MySQLDBAccess::update_node_info_joinNotRangingRange(const char * macaddress, int joinNotRangingRange)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE NodeInfo set joinNotRangingRange=%d WHERE macaddress='%s';", joinNotRangingRange, macaddress);

    ret = sql_execute(cmd);
    return ret;
}

int MySQLDBAccess::delete_node_info( int nodetype, int idx)
{
    int ret = 0;

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "DELETE from NodeInfo WHERE nodetype=%d and idx=%d", nodetype, idx);

    ret = sql_execute(cmd);
    //printf("delete_node_info, ret[%d] nodetype[%d] idx[%d]\n", ret, nodetype, idx);
    return ret;
}

int MySQLDBAccess::Transaction_delete_node_info( int nodetype, int idx)
{
    int ret = 0;

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "DELETE from NodeInfo WHERE nodetype=%d and idx=%d", nodetype, idx);

    std::string trans = cmd;
    mMysqlTransactionQueue.add(trans);

    return ret;
}

int MySQLDBAccess::delete_node_info_bymac( int nodetype, const char * macaddress)
{
    int ret = 0;

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "DELETE from NodeInfo WHERE nodetype=%d and macaddress='%s';", nodetype, macaddress);

    ret = sql_execute(cmd);
    //printf("delete_node_info_bymac, ret[%d] nodetype[%d] macaddress[%s]\n", ret, nodetype, macaddress);
    return ret;
}

int MySQLDBAccess::delete_aoa_node_info_bymac(const char * macaddress)
{
    int ret = 0;

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "DELETE from AoANodeInfo WHERE macaddress='%s';", macaddress);

    ret = sql_execute(cmd);

    vilslog::printf("cmd[%s] ret[%d] macaddress[%s]\n", cmd, ret, macaddress);

    return ret;
}

int MySQLDBAccess::delete_all_node( int nodetype)
{
    int ret = 0;

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "DELETE from NodeInfo WHERE nodetype=%d", nodetype);

    ret = sql_execute(cmd);
    //printf("delete_node_info_bymac, ret[%d] nodetype[%d] macaddress[%s]\n", ret, nodetype, macaddress);
    return ret;
}
//
// event
//
int MySQLDBAccess::delete_event(int eventid)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "DELETE FROM events WHERE id=%d", eventid);

    //vilslog::printf("MySQLDBAccess::delete_event()cmd[%s]\n", cmd);

    ret = sql_execute(cmd);

    return ret;
}

int MySQLDBAccess::create_event(EventStruct event)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO events(PROJECT_ID, nodeid, action, areaid, issuetime) \
                    VALUES (%d, '%s', '%s', '%s', '%s')",
                    event.projectid, event.nodeid.c_str(), event.action.c_str(), event.areaid.c_str(), event.issuetime.c_str());


    //printf("MySQLDBAccess::create_event() cmd[%s]\n", cmd);

    ret = sql_execute(cmd);

    return ret;
}

int MySQLDBAccess::clear_all_event(int projectID)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "DELETE FROM events WHERE PROJECT_ID = %d", projectID);

    //printf("%s\n", cmd);
    //printf("\nstrlen(cmd)=%lu\n",strlen(cmd));

    ret = sql_execute(cmd);

    return ret;
}

int MySQLDBAccess::write_event(int projectID, int eventid, std::string nodeid, std::string action, std::string areaid, std::string issuetime)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO events(PROJECT_ID, id, nodeid, action, areaid, issuetime) \
                    VALUES (%d, %d, '%s', '%s', '%s', '%s') \
                    ON DUPLICATE KEY UPDATE PROJECT_ID=VALUES(PROJECT_ID), nodeid=VALUES(nodeid), action=VALUES(action), areaid=VALUES(areaid), issuetime=VALUES(issuetime)",
                    projectID, eventid, nodeid.c_str(), action.c_str(), areaid.c_str(), issuetime.c_str());

    //printf("%s\n", cmd);
    //printf("\nstrlen(cmd)=%lu\n",strlen(cmd));

    ret = sql_execute(cmd);

    return ret;
}

//
// FiredEvents
//
std::list<FiredEventStruct*> * MySQLDBAccess::read_fired_events(int projectID, int unread, int prettystring, int count, int offset, int markasread)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::list<FiredEventStruct*> * FiredEventsList = new std::list<FiredEventStruct*>();

    // vilslog::printf("MySQLDBAccess::read_fired_events() projectID[%d] unread[%d] prettystring[%d] count[%d] offset[%d] markasread[%d]\n",
    //     projectID, unread, prettystring, count, offset, markasread);

    if (projectID < 0)
    {
        //sprintf(cmd, "SELECT * FROM FiredEvents ORDER BY datetime DESC LIMIT %d,%d", offset, count);
        if (unread == 1)
        {
            sprintf(cmd, "SELECT * FROM FiredEvents WHERE readdatetime IS NULL OR readdatetime = '' ORDER BY datetime DESC LIMIT %d,%d", offset, count);
        }
        else
        {
            sprintf(cmd, "SELECT * FROM FiredEvents ORDER BY datetime DESC LIMIT %d,%d", offset, count);
        }
    }
    else
    {
        if (unread == 1)
        {
            //sprintf(cmd, "SELECT * FROM FiredEvents WHERE PROJECT_ID=%d AND length(readdatetime)=0 ORDER BY datetime DESC LIMIT %d,%d", projectID, offset, count);
            sprintf(cmd, "SELECT * FROM FiredEvents WHERE PROJECT_ID=%d AND readdatetime IS NULL ORDER BY datetime DESC LIMIT %d,%d", projectID, offset, count);
        }
        else
        {
            //sprintf(cmd, "SELECT * FROM FiredEvents WHERE PROJECT_ID=%d AND length(readdatetime)>0 ORDER BY datetime DESC LIMIT %d,%d", projectID, offset, count);
            sprintf(cmd, "SELECT * FROM FiredEvents WHERE PROJECT_ID=%d ORDER BY datetime DESC LIMIT %d,%d", projectID, offset, count);
        }
    }

    //printf("MySQLDBAccess::read_fired_events() cmd[%s]\n", cmd);

    sql_executeQuery_new(cmd, get_FiredEventlist_callback, FiredEventsList);

    //printf("MySQLDBAccess::read_fired_events() FiredEventsList->size()[%d]\n", (int)FiredEventsList->size());

    for (std::list<FiredEventStruct*>::iterator firedevent = FiredEventsList->begin(); firedevent != FiredEventsList->end(); firedevent++)
    {
        if (prettystring == 1)
        {
            std::string prettyjson = getPrettyJSON((*firedevent)->eventjson);
            (*firedevent)->prettyjson = prettyjson;
        }
        if (markasread == 1)
        {
            markFiredEvent((*firedevent)->firedeventid);
        }
    }

    return FiredEventsList;
}

std::list<FiredEventStruct*> * MySQLDBAccess::read_fired_events_DayRange(const char * FromDatetime, const char * ToDatetime, const char * order)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::list<FiredEventStruct*> * FiredEventsList = new std::list<FiredEventStruct*>();

    sprintf(cmd, "SELECT * FROM FiredEvents WHERE datetime BETWEEN '%s' AND '%s' order by datetime %s ", FromDatetime, ToDatetime, order);

    sql_executeQuery_new(cmd, get_FiredEventlist_callback, FiredEventsList);

    return FiredEventsList;
}

std::list<FiredEventStruct*> * MySQLDBAccess::read_fired_events_DayRange(const char * eventid, const char * FromDatetime, const char * ToDatetime, const char * order)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::list<FiredEventStruct*> * FiredEventsList = new std::list<FiredEventStruct*>();

    sprintf(cmd, "SELECT * FROM FiredEvents WHERE eventid='%s' AND datetime BETWEEN '%s' AND '%s' order by datetime %s ",
        eventid, FromDatetime, ToDatetime, order);

    sql_executeQuery_new(cmd, get_FiredEventlist_callback, FiredEventsList);

    return FiredEventsList;
}

int MySQLDBAccess::write_fired_events(int projectID, int eventid, int maingroupid, std::string timestart, std::string timeend, std::string eventjson)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO FiredEvents(PROJECT_ID, eventid, maingroupid, eventjson, timestart, timeend) \
                    VALUES (%d, %d, %d, '%s', '%s', '%s') ", projectID, eventid, maingroupid, eventjson.c_str(), timestart.c_str(), timeend.c_str());
    ret = sql_execute(cmd);
    return ret;
}

int MySQLDBAccess::write_fired_events(int projectID, int eventid, int maingroupid, std::string timestart, std::string timeend, std::string eventjson, std::string empid)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO FiredEvents(PROJECT_ID, eventid, maingroupid, eventjson, timestart, timeend, empid) \
                    VALUES (%d, %d, %d, '%s', '%s', '%s', '%s') ", projectID, eventid, maingroupid, eventjson.c_str(), timestart.c_str(), timeend.c_str(), empid.c_str());
    ret = sql_execute(cmd);
    return ret;
}

int MySQLDBAccess::check_fired_events(int projectID, int eventid, std::string timestart)
{
    char cmd[4096];
    memset(cmd, 0, 4096);


    //
    // check if already fired
    //
    sprintf(cmd, "SELECT count(*) FROM FiredEvents where PROJECT_ID=%d and eventid=%d and timeend>'%s'",
            projectID, eventid, timestart.c_str());

    int count = 0;
    sql_executeQuery_new(cmd, get_count_callback, &count);

//    if (count > 0 )
//    {
//        vilslog::printf("MySQLDBAccess::check_fired_events() count()[%d]\n", count);
//    }

    return count;
}

int MySQLDBAccess::markFiredEvent(int tableid)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE FiredEvents SET readdatetime='%s' WHERE id=%d;", currentDateTimeForSQL().c_str(), tableid);

    //printf("MySQLDBAccess::markFiredEvent() tableid[%d]\n", tableid);

    ret = sql_execute(cmd);

    return ret;
}

// Alert
int MySQLDBAccess::write_alert_record(int nodetype, const char * macaddress)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO AlertRecord(nodetype, macaddress) VALUES (%d, '%s')", nodetype, macaddress);

    ret = sql_execute(cmd);

    std::string datetime = currentDateTimeForSQL();
    update_to_alert_cache(macaddress, 1, datetime.c_str(), datetime.c_str(), datetime.c_str(), datetime.c_str());

    return ret;
}

int MySQLDBAccess::write_alert0D_record(int nodetype, const char * macaddress)
{
    int ret = 0;

    vilslog::printf("MySQLDBAccess::write_alert0D_record() nodetype[%d] macaddress[%s]\n", nodetype, macaddress);

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO AlertRecord(nodetype, macaddress) VALUES (%d, '%s')", nodetype, macaddress);

    ret = sql_execute(cmd);

    std::string datetime = currentDateTimeForSQL();
    update_to_alert_cache(macaddress, 1, datetime.c_str(), datetime.c_str(), datetime.c_str(), datetime.c_str());

    return ret;
}

int MySQLDBAccess::write_sys_config(int configID, VILSSystemConfig * systemConfig)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO systemconfig(CONFIG_ID, SOLVETAGPOSITION_TIME, SENDTAFDB_TIME, SENDTAGANCHORLIST_TIME, SENDDEVICESTATUS_TIME, UPDATENODEFROMSMIMSIOTSERVER_TIME) \
                    VALUES (%d, %d, %d, %d, %d, %d) \
                    ON DUPLICATE KEY UPDATE SOLVETAGPOSITION_TIME=VALUES(SOLVETAGPOSITION_TIME), SENDTAFDB_TIME=VALUES(SENDTAFDB_TIME), \
                    SENDTAGANCHORLIST_TIME=VALUES(SENDTAGANCHORLIST_TIME), SENDDEVICESTATUS_TIME=VALUES(SENDDEVICESTATUS_TIME), UPDATENODEFROMSMIMSIOTSERVER_TIME=VALUES(UPDATENODEFROMSMIMSIOTSERVER_TIME) ",
                    configID, systemConfig->SolveTagPosition_Time , systemConfig->SendTAFDB_Time, systemConfig->SendTagAnchorList_Time, systemConfig->SendDeviceStatus_Time, systemConfig->UpdateNodeFromSMIMSIOTSERVER_Time);

    int ret = sql_execute(cmd);

    sprintf(cmd, "INSERT INTO systemconfig(CONFIG_ID, web_udp_port, center_udp_port, coord_udp_port, \
                    center_broadcast_time, center_broadcast_port, web_socket_port, USING_PROTOCOL, VILSServer, VILSUID, ERPServer, SSOIP, \
                    PushNotifyPrefix, PushNotifyWelcomeTitle, PushNotifyWelcomeBody, EnablePushNotify, TagAlertTimeout, TagAlertVoltageLow) \
                    VALUES (%d, %d, %d, %d, %d, %d, %d, %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d, %d, '%s') \
                    ON DUPLICATE KEY UPDATE  web_udp_port=VALUES(web_udp_port), center_udp_port=VALUES(center_udp_port), coord_udp_port=VALUES(coord_udp_port), \
                    center_broadcast_time=VALUES(center_broadcast_time), center_broadcast_port=VALUES(center_broadcast_port), web_socket_port=VALUES(web_socket_port), \
                    USING_PROTOCOL=VALUES(USING_PROTOCOL), VILSServer=VALUES(VILSServer), VILSUID=VALUES(VILSUID), ERPServer=VALUES(ERPServer), SSOIP=VALUES(SSOIP), \
                    PushNotifyPrefix=VALUES(PushNotifyPrefix), PushNotifyWelcomeTitle=VALUES(PushNotifyWelcomeTitle), PushNotifyWelcomeBody=VALUES(PushNotifyWelcomeBody), EnablePushNotify=VALUES(EnablePushNotify), \
                    TagAlertTimeout=VALUES(TagAlertTimeout), TagAlertVoltageLow=VALUES(TagAlertVoltageLow) ",
                    configID, systemConfig->web_udp_port, systemConfig->center_udp_port, systemConfig->coord_udp_port,
                    systemConfig->center_broadcast_time, systemConfig->center_broadcast_port, systemConfig->web_socket_port,
                    systemConfig->UsingProtocol, systemConfig->VILSServer.c_str(), systemConfig->VILSUID.c_str(), systemConfig->ERPServer.c_str(), systemConfig->SSOIP.c_str(),
                    systemConfig->PushNotifyPrefix.c_str(), systemConfig->PushNotifyWelcomeTitle.c_str(), systemConfig->PushNotifyWelcomeBody.c_str(),
                    systemConfig->EnablePushNotify, systemConfig->TagAlertTimeout, systemConfig->TagAlertVoltageLow.c_str());

    ret = sql_execute(cmd);

    //1.Ranging Data Preprocessing
    sprintf(cmd, "INSERT INTO systemconfig(CONFIG_ID, VALIDDATATIME, MAX_RANGE_DISTANCE, EnableNoiseMakup, \
                    NoiseMakupRatio, NoiseMakupWeight, EnableKalman1D, \
                    EnableRangingReliability, RangingReliabilityMaxDistance, \
                    RangingReliabilityMinCount, RangingReliabilityAverageDiff) \
                    VALUES (%d, %d, %d, %d, %f, %f, %d, %d, %d, %d, %d) \
                    ON DUPLICATE KEY UPDATE VALIDDATATIME=VALUES(VALIDDATATIME), MAX_RANGE_DISTANCE=VALUES(MAX_RANGE_DISTANCE), EnableNoiseMakup=VALUES(EnableNoiseMakup), \
                    NoiseMakupRatio=VALUES(NoiseMakupRatio), NoiseMakupWeight=VALUES(NoiseMakupWeight), EnableKalman1D=VALUES(EnableKalman1D), \
                    EnableRangingReliability=VALUES(EnableRangingReliability), RangingReliabilityMaxDistance=VALUES(RangingReliabilityMaxDistance), \
                    RangingReliabilityMinCount=VALUES(RangingReliabilityMinCount), RangingReliabilityAverageDiff=VALUES(RangingReliabilityAverageDiff) ",
                    configID, systemConfig->ValidDataTime, systemConfig->MaxRangindDistance, systemConfig->EnableNoiseMakup ,
                    systemConfig->NoiseMakupRatio, systemConfig->NoiseMakupWeight, systemConfig->EnableKalman1D,
                    systemConfig->EnableRangingReliability, systemConfig->RangingReliabilityMaxDistance,
                    systemConfig->RangingReliabilityMinCount, systemConfig->RangingReliabilityAverageDiff);

    ret = sql_execute(cmd);

    //2.Position Calculation
    sprintf(cmd, "INSERT INTO systemconfig(CONFIG_ID, EnableAnchor1DMapping, EnableOneAnchorCalculation, EnableGDOP, \
                   GDOPThreshold, CalculationMethod, EnableIMUVelocity, IMUVelocityOffset, SteadyStateOffset, \
                   WeightMovingAverageValue1, WeightMovingAverageValue2, WeightMovingAverageValue3, StayInWallTimeout, EnableFixHeight) \
                   VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, %f, %f, %f, %d, %d) \
                   ON DUPLICATE KEY UPDATE EnableAnchor1DMapping=VALUES(EnableAnchor1DMapping), EnableOneAnchorCalculation=VALUES(EnableOneAnchorCalculation), EnableGDOP=VALUES(EnableGDOP), \
                   GDOPThreshold=VALUES(GDOPThreshold), CalculationMethod=VALUES(CalculationMethod), EnableIMUVelocity=VALUES(EnableIMUVelocity), IMUVelocityOffset=VALUES(IMUVelocityOffset), SteadyStateOffset=VALUES(SteadyStateOffset), \
                   WeightMovingAverageValue1=VALUES(WeightMovingAverageValue1), WeightMovingAverageValue2=VALUES(WeightMovingAverageValue2), WeightMovingAverageValue3=VALUES(WeightMovingAverageValue3), \
                   StayInWallTimeout=VALUES(StayInWallTimeout),EnableFixHeight=VALUES(EnableFixHeight) ",
                   configID, systemConfig->EnableAnchor1DMapping, systemConfig->EnableOneAnchorCalculation, systemConfig->EnableGDOP ,
                   systemConfig->GDOPThreshold, systemConfig->CalculationMethod, systemConfig->EnableIMUVelocity, systemConfig->IMUVelocityOffset, systemConfig->SteadyStateOffset,
                   systemConfig->WeightMovingAverageValue1, systemConfig->WeightMovingAverageValue2, systemConfig->WeightMovingAverageValue3,
                   systemConfig->StayInWallTimeout, systemConfig->EnableFixHeight);

    ret = sql_execute(cmd);

    //3.Position Result
    sprintf(cmd, "INSERT INTO systemconfig(CONFIG_ID, SmoothingResult, SmoothingResultOffset, SmoothingResultStep, \
                   SmoothingResultFirstStateOffsetRatio, SmoothingResultFirstStateRunTime, \
                   SmoothingResultSecondStateOffsetRatio, SmoothingResultSecondStateRunTime) \
                   VALUES (%d, %d, %d, %d, %f, %d, %f, %d) \
                   ON DUPLICATE KEY UPDATE SmoothingResult=VALUES(SmoothingResult), SmoothingResultOffset=VALUES(SmoothingResultOffset), SmoothingResultStep=VALUES(SmoothingResultStep), \
                   SmoothingResultFirstStateOffsetRatio=VALUES(SmoothingResultFirstStateOffsetRatio), SmoothingResultFirstStateRunTime=VALUES(SmoothingResultFirstStateRunTime), \
                   SmoothingResultSecondStateOffsetRatio=VALUES(SmoothingResultSecondStateOffsetRatio), SmoothingResultSecondStateRunTime=VALUES(SmoothingResultSecondStateRunTime) ",
                   configID, systemConfig->SmoothingResult, systemConfig->SmoothingResultOffset, systemConfig->SmoothingResultStep ,
                   systemConfig->SmoothingResultFirstStateOffsetRatio, systemConfig->SmoothingResultFirstStateRunTime,
                   systemConfig->SmoothingResultSecondStateOffsetRatio, systemConfig->SmoothingResultSecondStateRunTime);

    ret = sql_execute(cmd);

    //
    // LoRAWAN
    //
    sprintf(cmd, "INSERT INTO systemconfig(CONFIG_ID, LoRAWANTPOIC, MQTTHostIp, MQTTHostPort, \
                   MQTTHostUsername, MQTTHostPassword, MQTTClientId) \
                   VALUES (%d, '%s','%s', %d, '%s', '%s', '%s') \
                   ON DUPLICATE KEY UPDATE LoRAWANTPOIC=VALUES(LoRAWANTPOIC), MQTTHostIp=VALUES(MQTTHostIp), MQTTHostPort=VALUES(MQTTHostPort), \
                   MQTTHostUsername=VALUES(MQTTHostUsername), MQTTHostPassword=VALUES(MQTTHostPassword), MQTTClientId=VALUES(MQTTClientId) ",
                   configID, systemConfig->LoRAWANTPOIC.c_str(), systemConfig->MQTTHostIp.c_str(), systemConfig->MQTTHostPort,
                   systemConfig->MQTTHostUsername.c_str(), systemConfig->MQTTHostPassword.c_str(), systemConfig->MQTTClientId.c_str());

    ret = sql_execute(cmd);

    return ret;
}

int MySQLDBAccess::get_sys_sendNotifyTime(int configID)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT sendNotifyTime as count FROM systemconfig WHERE CONFIG_ID=%d ", configID);

    int sendNotifyTime = 480;
    sql_executeQuery_new(cmd, get_count_callback, &sendNotifyTime);

    // vilslog::printf("MySQLDBAccess::get_sys_sendNotifyTime() configID[%d] sendNotifyTime[%d]\n", configID, sendNotifyTime);

    return sendNotifyTime;
}

//=====================================================
// For support old project
std::list<VILSConfig*> * MySQLDBAccess::read_all_config()
{
    list<VILSConfig*> * configList = new list<VILSConfig*>();
    char cmd[4096];
    sprintf(cmd, "SELECT * FROM config");

    sql_executeQuery_new(cmd, get_VILSConfiglist_callback, configList);
    return configList;
}

int MySQLDBAccess::read_map(int projectID, MapStruct * map)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM Map WHERE PROJECT_ID=%d", projectID);

    sql_executeQuery_new(cmd, get_map_callback, map);

    return ret;
}

std::list<AreaStruct*> * MySQLDBAccess::read_area(int projectID)
{
    std::list<AreaStruct*> * areaList = new std::list<AreaStruct*>();

    char cmd[4096];
    if (projectID >= 0)
        sprintf(cmd, "SELECT * FROM MapArea WHERE PROJECT_ID=%d", projectID);
    else
        sprintf(cmd, "SELECT * FROM MapArea");

    sql_executeQuery_new(cmd, get_arealist_callback, areaList);

    return areaList;
}
//=====================================================

//
// SectionGroup
//
int MySQLDBAccess::create_SectionGroup(SectionGroup sectiongroup)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::string nodeIDListStr = convertListToString(sectiongroup.nodeIDList);
    sprintf(cmd, "INSERT INTO SectionGroup(PROJECT_ID, SectionType, nodeIDList, AreaID, Param1, Param2, Param3, Param4, Param5, Param6, maingroupid) \
                    VALUES (%d, %d, '%s', '%s', %d, %d, %d, %d, %d, %d, %d)",
                    sectiongroup.projectid, sectiongroup.SectionType, nodeIDListStr.c_str(),
                    sectiongroup.AreaID.c_str(), sectiongroup.Param1, sectiongroup.Param2, sectiongroup.Param3,
                    sectiongroup.Param4, sectiongroup.Param5, sectiongroup.Param6, sectiongroup.maingroupid);

    ret = sql_execute(cmd);

    return ret;
}

int MySQLDBAccess::delete_SectionGroup(int GroupID)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "DELETE from SectionGroup WHERE GroupID=%d", GroupID);

    ret = sql_execute(cmd);

    return ret;
}

int MySQLDBAccess::write_SectionGroup(SectionGroup * sectiongroup)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::string nodeIDListStr = convertListToString(sectiongroup->nodeIDList);
    sprintf(cmd, "INSERT INTO SectionGroup(GroupID, PROJECT_ID, SectionType, nodeIDList, AreaID, Param1, Param2, Param3, Param4, Param5, Param6, maingroupid) \
                    VALUES (%d, %d, %d, '%s', '%s', %d, %d, %d, %d, %d, %d, %d) \
                    ON DUPLICATE KEY UPDATE PROJECT_ID=VALUES(PROJECT_ID), SectionType=VALUES(SectionType), \
                    nodeIDList=VALUES(nodeIDList), AreaID=VALUES(AreaID), Param1=VALUES(Param1), \
                    Param2=VALUES(Param2), Param3=VALUES(Param3), Param4=VALUES(Param4), Param5=VALUES(Param5), Param6=VALUES(Param6), maingroupid=VALUES(maingroupid)",
                    sectiongroup->GroupID, sectiongroup->projectid, sectiongroup->SectionType, nodeIDListStr.c_str(),
                    sectiongroup->AreaID.c_str(), sectiongroup->Param1, sectiongroup->Param2, sectiongroup->Param3,
                    sectiongroup->Param4, sectiongroup->Param5, sectiongroup->Param6, sectiongroup->maingroupid);

    ret = sql_execute(cmd);

    return ret;
}

int MySQLDBAccess::write_SectionGroupList(std::list<SectionGroup *> * sectiongrouplist)
{
    for (std::list<SectionGroup*>::iterator sectiongroup = sectiongrouplist->begin(); sectiongroup != sectiongrouplist->end(); sectiongroup++)
    {
        write_SectionGroup((*sectiongroup));
    }
    return (int)sectiongrouplist->size();
}

bool MySQLDBAccess::read_SectionGroupList(int projectID, std::list<SectionGroup *> * sectiongrouplist)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    if (projectID >= 0)
        sprintf(cmd, "SELECT * FROM SectionGroup WHERE PROJECT_ID=%d order by Param1 ASC", projectID);
    else
        sprintf(cmd, "SELECT * FROM SectionGroup order by Param1 ASC");

    sql_executeQuery_new(cmd, get_sectiongrouplist_callback, sectiongrouplist);
    return true;
}

bool MySQLDBAccess::read_SectionGroupList(int projectID, SectionTypeEnum SectionType, std::list<SectionGroup *> * sectiongrouplist)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM SectionGroup WHERE SectionType=%d order by Param1 ASC", SectionType);

    sql_executeQuery_new(cmd, get_sectiongrouplist_callback, sectiongrouplist);
    return true;
}

std::list<SectionGroup *> * MySQLDBAccess::read_Section0DGroupList()
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::list<SectionGroup *> * sectiongrouplist = new std::list<SectionGroup *>();

    sprintf(cmd, "SELECT * FROM SectionGroup WHERE SectionType=%d order by Param1 ASC", SECTION_TYPE_ZERO);

    sql_executeQuery_new(cmd, get_sectiongrouplist_callback, sectiongrouplist);

    return sectiongrouplist;
}

//
// FenceAlert
//
int MySQLDBAccess::create_FenceAlert(FenceAlertStruct fencealert)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO FenceAlert(nodeid, distance) VALUES ('%s', %d)", fencealert.nodeid.c_str(), fencealert.distance);

    ret = sql_execute(cmd);

    return ret;
}

int MySQLDBAccess::delete_FenceAlert(int GroupID)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "DELETE from FenceAlert WHERE GroupID=%d", GroupID);

    // printf("MySQLDBAccess::delete_FenceAlert() \ncmd[%s]\n", cmd);

    ret = sql_execute(cmd);

    return ret;
}

// NodeInfo
std::list<NodeStruct*> * MySQLDBAccess::read_nodes(int nodetype)
{
    std::list<NodeStruct*> * nodeList = new std::list<NodeStruct*>();

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM NodeInfo WHERE nodetype=%d", nodetype);

    sql_executeQuery_new(cmd, get_nodelist_callback, nodeList);

    //printf("MySQLDBAccess::read_nodes() nodeList->size()[%u]\n", nodeList->size());

    return nodeList;
}

std::list<NodeStruct*> * MySQLDBAccess::read_nodes_assigned_serno()
{
    std::list<NodeStruct*> * nodeList = new std::list<NodeStruct*>();

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM NodeInfo WHERE resident_serno IS NOT NULL AND resident_serno != '-1' AND resident_serno != '' ");

    sql_executeQuery_new(cmd, get_nodelist_callback, nodeList);

    return nodeList;
}

NodeStruct * MySQLDBAccess::getNodeStruct(std::string nodeid)
{
    NodeStruct * node = new NodeStruct();

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM NodeInfo WHERE nodeid='%s'", nodeid.c_str());

    sql_executeQuery_new(cmd, get_node_callback, node);

    return node;
}

std::string MySQLDBAccess::fetchNodeName(std::string nodeid)
{
    std::string nodename = nodeid;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM NodeInfo WHERE nodeid='%s'", nodeid.c_str());

    sql_executeQuery_new(cmd, get_nodename_callback, &nodename);
    //printf("MySQLDBAccess::fetchNodeName() nodename[%s]\n", nodename.c_str());

    return nodename;
}

std::string MySQLDBAccess::fetchLocatorName(std::string nodeid)
{
    std::string nodename = nodeid;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM NodeInfo WHERE nodeid='%s' AND nodetype=%d", nodeid.c_str(), NODE_TYPE_LOCATOR);

    sql_executeQuery_new(cmd, get_nodename_callback, &nodename);
    //printf("MySQLDBAccess::fetchLocatorName() nodename[%s]\n", nodename.c_str());

    return nodename;
}

// Events
std::list<EventStruct*> * MySQLDBAccess::read_events()
{
    return read_events(-1);
}

std::list<EventStruct*> * MySQLDBAccess::read_events(int projectID)
{
    std::list<EventStruct*> * eventList = new std::list<EventStruct*>();

    char cmd[4096];
    memset(cmd, 0, 4096);

    if (projectID >= 0 )
        sprintf(cmd, "SELECT * FROM events WHERE PROJECT_ID=%d ORDER BY datetime DESC", projectID);
    else
        sprintf(cmd, "SELECT * FROM events ORDER BY datetime DESC");

    sql_executeQuery_new(cmd, get_eventlist_callback, eventList);

    return eventList;
}

std::list<EventStruct*> * MySQLDBAccess::read_events_forshow(int projectID)
{
    std::list<EventStruct*> * eventList = read_events(projectID);

    for (std::list<EventStruct*>::iterator event = eventList->begin(); event != eventList->end(); event++)
    {
        (*event)->nodename = fetchNodeName((*event)->nodeid);
        (*event)->areaname = fetchAreaName((*event)->areaid);
        (*event)->issuename = get_time_name((*event)->issuetime);
    }
    return eventList;
}

// VILSSystemConfig
bool MySQLDBAccess::read_sys_config(VILSSystemConfig * sysConfig)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM systemconfig");

    //printf("MySQLDBAccess::read_sys_config() cmd[%s]\n", cmd);

    bool success = sql_executeQuery_new(cmd, get_SystemConfig_callback, sysConfig);

    //printf("MySQLDBAccess::read_sys_config() success[%d]\n", success);

    if (!success)
    {
        // create new one
        vilslog::printf("MySQLDBAccess::read_sys_config() create new one\n");

        sysConfig->SolveTagPosition_Time = 100; // solve TAG position every 100 milliseconds
        sysConfig->SendTAFDB_Time = 1000;// send position to TAF DB every 1 seconds
        sysConfig->SendTagAnchorList_Time = 3000; // broadcast TagAnchorValidTable every 3 seconds
        sysConfig->SendDeviceStatus_Time = 10000; // broadcast Device status every 10 seconds
        sysConfig->UpdateNodeFromSMIMSIOTSERVER_Time = 60000; // update loca node list from SMIMS IOT GAE server every 60 seconds

        sysConfig->web_udp_port = 18613; // VILS CGI-web UDP server, vils app <-- CGI
        sysConfig->center_udp_port = 18614;// VILS Center UDP server, vils center app <-- vils coord app
        sysConfig->coord_udp_port = 18615;// VILS Coord UDP server, vils coord app <-- vils center app
        sysConfig->center_broadcast_time = 5000;// Center broadcast IP
        sysConfig->center_broadcast_port = 12317;
        sysConfig->web_socket_port = 8083; // Web Socket server Port
        sysConfig->UsingProtocol = UDP;

        sysConfig->TagAlertTimeout = 5000; // Alert timeout in milli seconds
        sysConfig->TagAlertVoltageLow = "3.8";

        //1.Ranging Data Preprocessing
        sysConfig->ValidDataTime = 1000; // valid data in millisecond
        sysConfig->MaxRangindDistance = 3500; // MAX distance for tag and anchor ranging. In centmeters.
        sysConfig->EnableNoiseMakup = 0;// 0:disable 1:enable
        sysConfig->NoiseMakupRatio = 2.0f; // 2.0 實際測距值與平均測距值比例, 大於此數時計算合理測距值取代目前實際測距值
        sysConfig->NoiseMakupWeight = 0.8f; //0.8 實際測距值併入平均測距值的比重.(0.1~1.0)
        //sysConfig->RangingDataMethod = 0; // 0: kalman 1D, 1:RangingReliability
        sysConfig->EnableKalman1D = 0; // 0:disable 1:enable
        sysConfig->EnableRangingReliability = 0; // 0:disable 1:enable
        sysConfig->RangingReliabilityMaxDistance = 1300; // 最大可用測距值(cm), 大於此數將捨棄.
        sysConfig->RangingReliabilityMinCount = 1; // 特定時間內至少可用測距次數, 少於此數將捨棄.
        sysConfig->RangingReliabilityAverageDiff = 50; // 最大平均測距差(cm), 大於此數將捨棄.

        //2.Position Calculation
        sysConfig->EnableAnchor1DMapping = 0;// 0:disable 1:enable
        sysConfig->EnableOneAnchorCalculation = 0;// 0:disable 1:enable
        sysConfig->EnableGDOP = 0;// 0:disable 1:enable
        sysConfig->GDOPThreshold = 60; // default:60
        sysConfig->CalculationMethod = 0; // 0: kalman 3D, 1:SMOOTHING_FILTER, 2:Weight Moving Average
        sysConfig->EnableIMUVelocity = 0;// 0:disable 1:enable
        sysConfig->IMUVelocityOffset = 100;// default:100
        sysConfig->SteadyStateOffset = 5;// default:5
        sysConfig->WeightMovingAverageValue1 = 0.01f; //0.01
        sysConfig->WeightMovingAverageValue2 = 0.02f; //0.02
        sysConfig->WeightMovingAverageValue3 = 0.97f; //0.97
        sysConfig->StayInWallTimeout = 5000;// default:5000
        sysConfig->EnableFixHeight = 1;

        //3.Position Result
        sysConfig->SmoothingResult = 1;
        sysConfig->SmoothingResultOffset = 10; //in centermeter
        sysConfig->SmoothingResultStep = 6; // default:6
        sysConfig->SmoothingResultFirstStateOffsetRatio = 0.3f;
        sysConfig->SmoothingResultFirstStateRunTime = 200; // ms
        sysConfig->SmoothingResultSecondStateOffsetRatio = 0.8f;
        sysConfig->SmoothingResultSecondStateRunTime = 600; // ms

        //
        // LoRAWAN
        //
        sysConfig->LoRAWANTPOIC = "GIOT-GW/UL/#";
        sysConfig->MQTTHostIp = "127.0.0.1";
        sysConfig->MQTTHostPort = 1883;
        sysConfig->MQTTHostUsername = "admin";
        sysConfig->MQTTHostPassword = "smims2017";
        //sysConfig->MQTTClientId = "200cecd38e824e6388827552939fe4d1";
        sysConfig->MQTTClientId = random_string(32);

        write_sys_config(1, sysConfig);

    }
    return true;
}

bool MySQLDBAccess::update_systemconfigudpport(VILSSystemConfig sysConfig)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE systemconfig SET web_udp_port=%d, center_udp_port=%d, coord_udp_port=%d, center_broadcast_port=%d, \
                    web_socket_port=%d, TagAlertTimeout=%d, TagAlertVoltageLow='%s', VILSServer='%s', VILSUID='%s', ERPServer='%s', SSOIP='%s', \
                    PushNotifyPrefix='%s', PushNotifyWelcomeTitle='%s', PushNotifyWelcomeBody='%s', EnablePushNotify=%d WHERE CONFIG_ID=1 ",
                sysConfig.web_udp_port, sysConfig.center_udp_port, sysConfig.coord_udp_port, sysConfig.center_broadcast_port,
                sysConfig.web_socket_port, sysConfig.TagAlertTimeout, sysConfig.TagAlertVoltageLow.c_str(),
                sysConfig.VILSServer.c_str(), sysConfig.VILSUID.c_str(), sysConfig.ERPServer.c_str(), sysConfig.SSOIP.c_str(),
                sysConfig.PushNotifyPrefix.c_str(), sysConfig.PushNotifyWelcomeTitle.c_str(), sysConfig.PushNotifyWelcomeBody.c_str(), sysConfig.EnablePushNotify);

    sql_execute(cmd);

    // vilslog::printf("MySQLDBAccess::update_systemconfigudpport() cmd[%s]\n", cmd);

    return true;
}

bool MySQLDBAccess::update_AlgorithmConfig(VILSSystemConfig sysConfig)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    //1.Ranging Data Preprocessing
    sprintf(cmd, "INSERT INTO systemconfig(CONFIG_ID, VALIDDATATIME, MAX_RANGE_DISTANCE, EnableNoiseMakup, \
                    NoiseMakupRatio, NoiseMakupWeight, EnableKalman1D, \
                    EnableRangingReliability, RangingReliabilityMaxDistance, \
                    RangingReliabilityMinCount, RangingReliabilityAverageDiff) \
                    VALUES (%d, %d, %d, %d, %f, %f, %d, %d, %d, %d, %d) \
                    ON DUPLICATE KEY UPDATE VALIDDATATIME=VALUES(VALIDDATATIME), MAX_RANGE_DISTANCE=VALUES(MAX_RANGE_DISTANCE), EnableNoiseMakup=VALUES(EnableNoiseMakup), \
                    NoiseMakupRatio=VALUES(NoiseMakupRatio), NoiseMakupWeight=VALUES(NoiseMakupWeight), EnableKalman1D=VALUES(EnableKalman1D), \
                    EnableRangingReliability=VALUES(EnableRangingReliability), RangingReliabilityMaxDistance=VALUES(RangingReliabilityMaxDistance), \
                    RangingReliabilityMinCount=VALUES(RangingReliabilityMinCount), RangingReliabilityAverageDiff=VALUES(RangingReliabilityAverageDiff) ",
                    1, sysConfig.ValidDataTime, sysConfig.MaxRangindDistance, sysConfig.EnableNoiseMakup ,
                    sysConfig.NoiseMakupRatio, sysConfig.NoiseMakupWeight, sysConfig.EnableKalman1D,
                    sysConfig.EnableRangingReliability, sysConfig.RangingReliabilityMaxDistance,
                    sysConfig.RangingReliabilityMinCount, sysConfig.RangingReliabilityAverageDiff);

    sql_execute(cmd);

    //2.Position Calculation
    sprintf(cmd, "INSERT INTO systemconfig(CONFIG_ID, EnableAnchor1DMapping, EnableOneAnchorCalculation, EnableGDOP, \
                   GDOPThreshold, CalculationMethod, EnableIMUVelocity, IMUVelocityOffset, SteadyStateOffset, \
                   WeightMovingAverageValue1, WeightMovingAverageValue2, WeightMovingAverageValue3, StayInWallTimeout, EnableFixHeight) \
                   VALUES (%d, %d, %d, %d, %d, %d, %d, %d, %d, %f, %f, %f, %d, %d) \
                   ON DUPLICATE KEY UPDATE EnableAnchor1DMapping=VALUES(EnableAnchor1DMapping), EnableOneAnchorCalculation=VALUES(EnableOneAnchorCalculation), EnableGDOP=VALUES(EnableGDOP), \
                   GDOPThreshold=VALUES(GDOPThreshold), CalculationMethod=VALUES(CalculationMethod), EnableIMUVelocity=VALUES(EnableIMUVelocity), IMUVelocityOffset=VALUES(IMUVelocityOffset), SteadyStateOffset=VALUES(SteadyStateOffset), \
                   WeightMovingAverageValue1=VALUES(WeightMovingAverageValue1), WeightMovingAverageValue2=VALUES(WeightMovingAverageValue2), WeightMovingAverageValue3=VALUES(WeightMovingAverageValue3), \
                   StayInWallTimeout=VALUES(StayInWallTimeout), EnableFixHeight=VALUES(EnableFixHeight) ",
                   1, sysConfig.EnableAnchor1DMapping, sysConfig.EnableOneAnchorCalculation, sysConfig.EnableGDOP ,
                   sysConfig.GDOPThreshold, sysConfig.CalculationMethod, sysConfig.EnableIMUVelocity, sysConfig.IMUVelocityOffset, sysConfig.SteadyStateOffset,
                   sysConfig.WeightMovingAverageValue1, sysConfig.WeightMovingAverageValue2, sysConfig.WeightMovingAverageValue3,
                   sysConfig.StayInWallTimeout, sysConfig.EnableFixHeight);

    sql_execute(cmd);

    //3.Position Result
    sprintf(cmd, "INSERT INTO systemconfig(CONFIG_ID, SmoothingResult, SmoothingResultOffset, SmoothingResultStep, \
                   SmoothingResultFirstStateOffsetRatio, SmoothingResultFirstStateRunTime, \
                   SmoothingResultSecondStateOffsetRatio, SmoothingResultSecondStateRunTime) \
                   VALUES (%d, %d, %d, %d, %f, %d, %f, %d) \
                   ON DUPLICATE KEY UPDATE SmoothingResult=VALUES(SmoothingResult), SmoothingResultOffset=VALUES(SmoothingResultOffset), SmoothingResultStep=VALUES(SmoothingResultStep), \
                   SmoothingResultFirstStateOffsetRatio=VALUES(SmoothingResultFirstStateOffsetRatio), SmoothingResultFirstStateRunTime=VALUES(SmoothingResultFirstStateRunTime), \
                   SmoothingResultSecondStateOffsetRatio=VALUES(SmoothingResultSecondStateOffsetRatio), SmoothingResultSecondStateRunTime=VALUES(SmoothingResultSecondStateRunTime) ",
                   1, sysConfig.SmoothingResult, sysConfig.SmoothingResultOffset, sysConfig.SmoothingResultStep ,
                   sysConfig.SmoothingResultFirstStateOffsetRatio, sysConfig.SmoothingResultFirstStateRunTime,
                   sysConfig.SmoothingResultSecondStateOffsetRatio, sysConfig.SmoothingResultSecondStateRunTime);

    sql_execute(cmd);

    return true;
}

bool MySQLDBAccess::update_LoRAWANConfig(VILSSystemConfig sysConfig)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE systemconfig SET LoRAWANTPOIC='%s', MQTTHostIp='%s', MQTTHostPort=%d, \
                    MQTTHostUsername='%s', MQTTHostPassword='%s', MQTTClientId='%s' WHERE CONFIG_ID=1 ",
                sysConfig.LoRAWANTPOIC.c_str(), sysConfig.MQTTHostIp.c_str(), sysConfig.MQTTHostPort,
                sysConfig.MQTTHostUsername.c_str(), sysConfig.MQTTHostPassword.c_str(), sysConfig.MQTTClientId.c_str());

    sql_execute(cmd);
    return true;
}

std::list<InspectionGroupStruct*> * MySQLDBAccess::read_InspectionGroup()
{
    std::list<InspectionGroupStruct*> * InspectionGroupList = new std::list<InspectionGroupStruct*>();

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM InspectionGroup");

    sql_executeQuery_new(cmd, get_InspectionGrouplist_callback, InspectionGroupList);

    return InspectionGroupList;
}

std::list<InspectionPointStruct*> * MySQLDBAccess::read_InspectionPoint(int GroupID)
{
    list<InspectionPointStruct*> * InspectionPointList = new list<InspectionPointStruct*>();

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM InspectionPoint WHERE GroupID=%d ORDER BY PointIndex ASC", GroupID);

    sql_executeQuery_new(cmd, get_InspectionPointlist_callback, InspectionPointList);

    std::string today = TodayDate();
    for (std::list<InspectionPointStruct *>::iterator ispt = InspectionPointList->begin(); ispt != InspectionPointList->end(); ispt++)
    {
        InspectionPointStruct * InspectionPoint = (*ispt);
        int ret = read_InspectionPointDailyRecord(InspectionPoint->GroupID, InspectionPoint->PointID, today, InspectionPoint->DailyData);
        if (ret == 0)
        {
            // empty record
            // create new one
            InspectionPoint->DailyData.GroupID = InspectionPoint->GroupID;
            InspectionPoint->DailyData.PointID = InspectionPoint->PointID;
            InspectionPoint->DailyData.DateTime = today;
            write_InspectionPointDailyRecord(InspectionPoint->DailyData);
            //read again
            read_InspectionPointDailyRecord(InspectionPoint->GroupID, InspectionPoint->PointID, today, InspectionPoint->DailyData);
        }
    }

    //printf("MySQLDBAccess::read_InspectionPoint(%d) Finished!\n", GroupID);

    return InspectionPointList;
}

int MySQLDBAccess::write_InspectionGroupDailyRecord(InspectionGroupDailyRecord DailyRecord)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    if (DailyRecord.DailyRecordID < 0)
    {
        sprintf(cmd, "INSERT INTO InspectionGroupDailyRecord(GroupID, Score, Checked, Finished, Exceed, DateTime) \
                        VALUES (%d, %d, %d, %d, %d, '%s') ",
                        DailyRecord.GroupID, DailyRecord.Score, DailyRecord.Checked, DailyRecord.Finished, DailyRecord.Exceed, DailyRecord.DateTime.c_str());
    }
    else
    {
        sprintf(cmd, "UPDATE InspectionGroupDailyRecord SET GroupID=%d, Score=%d, Checked=%d, Finished=%d, Exceed=%d, DateTime='%s' WHERE DailyRecordID=%d;",
                        DailyRecord.GroupID, DailyRecord.Score, DailyRecord.Checked, DailyRecord.Finished, DailyRecord.Exceed,
                        DailyRecord.DateTime.c_str(), DailyRecord.DailyRecordID );

    }

    // printf("MySQLDBAccess::write_InspectionGroupDailyRecord() cmd[%s]!\n", cmd);

    ret = sql_execute(cmd);

    return ret;
}

int MySQLDBAccess::read_InspectionGroupDailyRecord(int GroupID, std::string Today, InspectionGroupDailyRecord & DailyRecord)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM InspectionGroupDailyRecord WHERE GroupID=%d AND DateTime='%s' ", GroupID, Today.c_str());

    bool succeess = sql_executeQuery_new(cmd, get_InspectionGroupDailyRecord_callback, &DailyRecord);
    if (succeess)
        ret = 1;

    if (ret == 0)
    {
        // empty record
        // create new one
        vilslog::printf("read_InspectionGroupDailyRecord() empty record, create new one\n");

        DailyRecord.DailyRecordID = -1;
        DailyRecord.GroupID = GroupID;
        DailyRecord.Score = 0;
        DailyRecord.Checked = 0;
        DailyRecord.Finished = 0;
        DailyRecord.Exceed = 0;
        DailyRecord.DateTime = Today;
        write_InspectionGroupDailyRecord(DailyRecord);

        sql_executeQuery_new(cmd, get_InspectionGroupDailyRecord_callback, &DailyRecord);

        ret = 1;
    }

    return ret;
}

int MySQLDBAccess::write_InspectionPointDailyRecord(InspectionPointDailyRecord DailyRecord)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::string EnterTime = time_tToString(DailyRecord.EnterTime);
    std::string LeaveTime = time_tToString(DailyRecord.LeaveTime);

    if (DailyRecord.DailyRecordID < 0)
    {
        sprintf(cmd, "INSERT INTO InspectionPointDailyRecord(GroupID, PointID, Score, Checked, Finished, Exceed, \
                        Inspector, RecordNodeId, EnterTime, LeaveTime, Status, DateTime) \
                        VALUES (%d, %d, %d, %d, %d, %d, '%s', '%s', '%s', '%s', %d, '%s') ",
                        DailyRecord.GroupID, DailyRecord.PointID, DailyRecord.Score, DailyRecord.Checked, DailyRecord.Finished, DailyRecord.Exceed,
                        DailyRecord.Inspector.c_str(), DailyRecord.RecordNodeId.c_str(), EnterTime.c_str(), LeaveTime.c_str(),
                        DailyRecord.Status, DailyRecord.DateTime.c_str());
    }
    else
    {
        sprintf(cmd, "UPDATE InspectionPointDailyRecord SET GroupID=%d, PointID=%d, Score=%d, Checked=%d, Finished=%d, Exceed=%d, \
                        Inspector='%s', RecordNodeId='%s', EnterTime='%s', LeaveTime='%s', Status=%d, DateTime='%s' WHERE DailyRecordID=%d;",
                        DailyRecord.GroupID, DailyRecord.PointID, DailyRecord.Score, DailyRecord.Checked, DailyRecord.Finished, DailyRecord.Exceed,
                        DailyRecord.Inspector.c_str(), DailyRecord.RecordNodeId.c_str(), EnterTime.c_str(), LeaveTime.c_str(),
                        DailyRecord.Status, DailyRecord.DateTime.c_str(), DailyRecord.DailyRecordID);
    }

    ret = sql_execute(cmd);

    //printf("write_InspectionPointDailyRecord() \ncmd[%s]\n", cmd);

    return ret;
}

int MySQLDBAccess::read_InspectionPointDailyRecord(int GroupID, int PointID, std::string Today, InspectionPointDailyRecord & DailyRecord)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM InspectionPointDailyRecord WHERE GroupID=%d AND PointID=%d AND DateTime='%s' ", GroupID, PointID, Today.c_str());

    bool succeess = sql_executeQuery_new(cmd, get_InspectionPointDailyRecord_callback, & DailyRecord);
    if (succeess)
        ret = 1;

    return ret;
}

int MySQLDBAccess::write_InspectorBehaviorData(InspectorBehaviorData InspectorBehaviorData)
{
    int ret = 0;
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO InspectorBehaviorData(GroupID, PointID, PointIndex, AnchorNodeId, Inspector, Distance, DateTime) \
                    VALUES (%d, %d, %d, '%s', '%s', %d, '%s')",
                    InspectorBehaviorData.GroupID, InspectorBehaviorData.PointID, InspectorBehaviorData.PointIndex,
                    InspectorBehaviorData.AnchorNodeId.c_str(), InspectorBehaviorData.Inspector.c_str(), InspectorBehaviorData.Distance,
                    InspectorBehaviorData.DateTime.c_str());

    ret = sql_execute(cmd);

    return ret;
}

std::string MySQLDBAccess::get_area_name(std::string areaid)
{
    std::string area_name = "";
    char cmd[4096];
    memset(cmd, 0, 4096);

    //sprintf(cmd, "SELECT * FROM MapArea WHERE areaid='%s'", areaid.c_str());
    sprintf(cmd, "SELECT * FROM maplayerarea WHERE areaid='%s'", areaid.c_str());

    sql_executeQuery_new(cmd, get_areaname_callback, &area_name);
    //printf("MySQLDBAccess::get_area_name() area_name[%s]\n", area_name.c_str());
    return area_name;
}

SurveyStruct * MySQLDBAccess::readSurveyFromTitle(std::string title)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM Survey WHERE title='%s' ", title.c_str());

    SurveyStruct * survey = new SurveyStruct();

    sql_executeQuery_new(cmd, get_Survey_callback, survey);

    return survey;
}

std::list<SurveyStruct*> * MySQLDBAccess::readSurveys(std::list<std::string> * surveys)
{
    std::ostringstream sendstream;
    sendstream << "SELECT * FROM Survey WHERE ";
    for (std::list<std::string>::iterator survey = surveys->begin(); survey != surveys->end(); survey++)
    {
        if ( survey != surveys->begin() )
            sendstream << "OR ";
        sendstream << "surveyid="<<(*survey)<<" ";
    }

    // printf("MySQLDBAccess::readSurveys() sendstream.c_str()[%s]\n", sendstream.str().c_str());

    std::list<SurveyStruct*> * surveyList = new std::list<SurveyStruct*>();

    sql_executeQuery_new(sendstream.str().c_str(), get_SurveyList_callback, surveyList);

    return surveyList;
}

std::list<SurveyItemStruct*> * MySQLDBAccess::readSurveyItems(int surveyid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM SurveyItem WHERE surveyid=%d order by surveyitemid ASC ", surveyid);

    std::list<SurveyItemStruct*> * surveyItemList = new std::list<SurveyItemStruct*>();

    sql_executeQuery_new(cmd, get_SurveyItemList_callback, surveyItemList);

    return surveyItemList;
}

std::list<CARDRollCallStruct*> * MySQLDBAccess::read_CARDRollCallInfo()
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM CARDRollCall ");

    std::list<CARDRollCallStruct*> * rollcallList = new std::list<CARDRollCallStruct*>();

    sql_executeQuery_new(cmd, get_CARDRollCallStructList_callback, rollcallList);

    return rollcallList;
}

CARDRollCallStruct * MySQLDBAccess::read_CARDRollCallInfo(int rollcallid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM CARDRollCall where rollcallid=%d ", rollcallid);

    CARDRollCallStruct * rollcallinfo = new CARDRollCallStruct();

    sql_executeQuery_new(cmd, get_CARDRollCallStruct_callback, rollcallinfo);

    return rollcallinfo;
}

std::list<AreaInOutEventStruct*> * MySQLDBAccess::getNodeAreaInOutList(const char * nodeid, const char * FromDatetime, const char * ToDatetime, const char * order)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM AreaInOutRecord WHERE NodeId='%s' AND issuetime between '%s' and '%s' order by issuetime %s ",
                nodeid, FromDatetime, ToDatetime, order);

    // vilslog::printf("MySQLDBAccess::getNodeAreaInOutList() cmd[%s]\n", cmd);

    list<AreaInOutEventStruct*> * nodeAreaInOutList = new list<AreaInOutEventStruct*>();

    sql_executeQuery_new(cmd, get_NodeAreaInOutList_callback, nodeAreaInOutList);

    return nodeAreaInOutList;
}

AreaInOutEventStruct * MySQLDBAccess::getNodeLastAreaInOutList(const char * nodeid, const char * areaid, const char * FromDatetime, int action)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM AreaInOutRecord WHERE NodeId='%s' AND AreaID='%s' AND DataTime<='%s' AND action=%d ORDER BY issuetime DESC limit 1 ",
        nodeid, areaid, FromDatetime, action);

    // vilslog::printf("MySQLDBAccess::getNodeAreaInOutList() cmd[%s]\n", cmd);

    AreaInOutEventStruct * nodeAreaInOut = new AreaInOutEventStruct();

    sql_executeQuery_new(cmd, get_NodeAreaInOut_callback, nodeAreaInOut);

    return nodeAreaInOut;
}

AnalyzeDayInfoStruct * MySQLDBAccess::getAreaInOutAnalyzeDayInfo(const char * nodeid, const char * FromDatetime, const char * ToDatetime)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT COUNT(dbId) as order_count, \
                max(issuetime) as max_time, \
                min(issuetime) as min_time, \
                SUBSTR( issuetime, 1, 10 ) as order_day \
                FROM AreaInOutRecord WHERE \
                NodeId='%s' AND issuetime between '%s' and '%s' GROUP BY order_day",
                nodeid, FromDatetime, ToDatetime);

    AnalyzeDayInfoStruct * dayInfo = createAnalyzeDayInfoStruct();

    bool bSuccess = sql_executeQuery_new(cmd, get_AreaInOutAnalyzeDayInfo_callback, dayInfo);

    if (!bSuccess)
    {
        //vilslog::printf("MySQLDBAccess::getAreaInOutAnalyzeDayInfo() nodeid[%s] [%s][%s] Failed!\n", nodeid, FromDatetime, ToDatetime);
        std::string curDatetime = currentDateTimeForSQL();
        update_to_AreaInOut_cache(nodeid, 0, FromDatetime, ToDatetime, FromDatetime, curDatetime.c_str(), "", "");

        delete dayInfo;
        return NULL;
    }

    sprintf(cmd, "SELECT AreaID as stringvalue FROM AreaInOutRecord WHERE \
                NodeId='%s' AND issuetime between '%s' and '%s' GROUP BY stringvalue",
                nodeid, FromDatetime, ToDatetime);

    std::list<std::string> * AreaIDList = new std::list<std::string>();
    bSuccess = sql_executeQuery_new(cmd, get_StringValueList_callback, AreaIDList);

    sprintf(cmd, "SELECT AreaName as stringvalue FROM AreaInOutRecord WHERE \
                NodeId='%s' AND issuetime between '%s' and '%s' GROUP BY stringvalue",
                nodeid, FromDatetime, ToDatetime);

    std::list<std::string> * AreaNameList = new std::list<std::string>();
    bSuccess = sql_executeQuery_new(cmd, get_StringValueList_callback, AreaNameList);

    dayInfo->area_ids = convertListToString(AreaIDList);
    dayInfo->area_names = convertListToString(AreaNameList);

    //vilslog::printf("MySQLDBAccess::getAreaInOutAnalyzeDayInfo() nodeid[%s] [%s][%s] count[%d]\n", nodeid, FromDatetime, ToDatetime, dayInfo->count);

    AreaIDList->clear();
    AreaNameList->clear();
    delete AreaIDList;
    delete AreaNameList;

    if (bSuccess)
    {
        update_to_AreaInOut_cache(nodeid, dayInfo->count, FromDatetime, ToDatetime, dayInfo->min_time.c_str(), dayInfo->max_time.c_str(), dayInfo->area_ids.c_str(), dayInfo->area_names.c_str());

        return dayInfo;
    }
    else
    {
        delete dayInfo;
        return NULL;
    }
}

AnalyzeDayInfoStruct * MySQLDBAccess::getAreaInOutAnalyzeDayInfoFromCache(const char * nodeid, const char * FromDatetime, const char * ToDatetime)
{
    std::string datetime_str = FromDatetime;
    std::string date_str = datetime_str.substr(0, 10);

    AnalyzeDayInfoStruct * dayInfo = NULL;

    for (std::list<AnalyzeDayInfoStruct *>::iterator dayInfoCache = mAreaInOutAnalyzeDayInfoStructList.begin(); dayInfoCache != mAreaInOutAnalyzeDayInfoStructList.end(); dayInfoCache++)
    {
        if ( ((*dayInfoCache)->nodeid.compare(nodeid) == 0) && ((*dayInfoCache)->day.compare(date_str) == 0))
        {
            // if ( strcmp(nodeid, "Tag1181")  == 0)
            // {
            //     vilslog::printf("MySQLDBAccess::getAreaInOutAnalyzeDayInfoFromCache() nodeid[%s] [%s][%s]\n",
            //         nodeid, FromDatetime, ToDatetime);
            //     vilslog::printf("MySQLDBAccess::getAreaInOutAnalyzeDayInfoFromCache() nodeid[%s] min_time[%s] max_time[%s] count[%d]\n",
            //         nodeid, (*dayInfoCache)->min_time.c_str(), (*dayInfoCache)->max_time.c_str(), (*dayInfoCache)->count);
            // }

            if ( (*dayInfoCache)->cache_min_time.compare(FromDatetime) <= 0 &&
                 (*dayInfoCache)->cache_max_time.compare(ToDatetime)   >= 0  )
            {
                dayInfo = createAnalyzeDayInfoStruct();
                dayInfo->nodeid = (*dayInfoCache)->nodeid;
                dayInfo->max_time = (*dayInfoCache)->max_time;
                dayInfo->min_time = (*dayInfoCache)->min_time;
                dayInfo->count = (*dayInfoCache)->count;
                dayInfo->area_ids = (*dayInfoCache)->area_ids;
                dayInfo->area_names = (*dayInfoCache)->area_names;
                dayInfo->day = (*dayInfoCache)->day;
                dayInfo->course_id = (*dayInfoCache)->course_id;
                dayInfo->cache_max_time = (*dayInfoCache)->cache_max_time;
                dayInfo->cache_min_time = (*dayInfoCache)->cache_min_time;
            }
            break;
        }
    }
    return dayInfo;
}

AnalyzeDayInfoStruct * MySQLDBAccess::getAreaInOutAnalyzeDayInfoByAreaId(const char * nodeid, const char * areaid, int action, const char * FromDatetime, const char * ToDatetime)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT COUNT(dbId) as order_count, \
                max(issuetime) as max_time, \
                min(issuetime) as min_time, \
                SUBSTR( issuetime, 1, 10 ) as order_day \
                FROM AreaInOutRecord WHERE \
                NodeId='%s' AND AreaID='%s' AND action=%d AND issuetime between '%s' and '%s' GROUP BY order_day",
                nodeid, areaid, action, FromDatetime, ToDatetime);

    AnalyzeDayInfoStruct * dayInfo = createAnalyzeDayInfoStruct();

    bool bSuccess = sql_executeQuery_new(cmd, get_AreaInOutAnalyzeDayInfo_callback, dayInfo);

    if (!bSuccess)
    {
        delete dayInfo;
        return NULL;
    }
    return dayInfo;
}

AnalyzeDayInfoStruct * MySQLDBAccess::getGPSAnalyzeDayInfo(int nodetype, const char * nodeid, const char * FromDatetime, const char * ToDatetime)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    // if ( strcmp(nodeid,"Tag0117") == 0 )
    // {
    //     vilslog::printf ("MySQLDBAccess::getGPSAnalyzeDayInfo() nodeid[%s] FromDatetime[%s] ToDatetime[%s] \n",
    //             nodeid, FromDatetime, ToDatetime);
    // }

    sprintf(cmd, "SELECT COUNT(dbId) as order_count, \
                max(issuetime) as max_time, \
                min(issuetime) as min_time, \
                SUBSTR( issuetime, 1, 10 ) as order_day \
                from GPSInfoRecord WHERE \
                nodetype=%d AND NodeId='%s' AND issuetime between '%s' and '%s' GROUP BY order_day",
                nodetype, nodeid, FromDatetime, ToDatetime);

    AnalyzeDayInfoStruct * dayInfo = createAnalyzeDayInfoStruct();

    bool bSuccess = sql_executeQuery_new(cmd, get_AnalyzeDayInfo_callback, dayInfo);

    // if ( strcmp(nodeid,"Tag0117") == 0 )
    // {
    //     vilslog::printf ("MySQLDBAccess::getGPSAnalyzeDayInfo() bSuccess[%d]\n", bSuccess);
    // }

    if (bSuccess)
    {
        // if ( strcmp(nodeid,"Tag0117") == 0 )
        // {
        //     vilslog::printf ("MySQLDBAccess::getGPSAnalyzeDayInfo() dayInfo->count[%d]\n", dayInfo->count);
        // }

        update_to_GPS_cache(nodetype, nodeid, dayInfo->count, FromDatetime, ToDatetime, dayInfo->min_time.c_str(), dayInfo->max_time.c_str());

        return dayInfo;
    }
    else
    {
        std::string curDatetime = currentDateTimeForSQL();
        update_to_GPS_cache(nodetype, nodeid, 0, FromDatetime, ToDatetime, curDatetime.c_str(), curDatetime.c_str());

        delete dayInfo;
        return NULL;
    }
}

AnalyzeDayInfoStruct * MySQLDBAccess::getGPSAnalyzeDayInfoFromCache(int nodetype, const char * nodeid, const char * FromDatetime, const char * ToDatetime)
{
    std::string datetime_str = FromDatetime;
    std::string date_str = datetime_str.substr(0, 10);

    AnalyzeDayInfoStruct * dayInfo = NULL;

    // if ( strcmp(nodeid,"Tag0117") == 0 )
    // {
    //     vilslog::printf ("MySQLDBAccess::getGPSAnalyzeDayInfoFromCache() nodeid[%s] FromDatetime[%s] ToDatetime[%s] mGPSAnalyzeDayInfoStructList.size()[%d]\n",
    //         nodeid, FromDatetime, ToDatetime, (int)mGPSAnalyzeDayInfoStructList.size());
    // }

    for (std::list<AnalyzeDayInfoStruct *>::iterator dayInfoCache = mGPSAnalyzeDayInfoStructList.begin(); dayInfoCache != mGPSAnalyzeDayInfoStructList.end(); dayInfoCache++)
    {
        if ( ((*dayInfoCache)->nodeid.compare(nodeid) == 0) && ((*dayInfoCache)->day.compare(date_str) == 0) && ((*dayInfoCache)->nodetype == nodetype))
        {
            if ( (*dayInfoCache)->cache_min_time.compare(FromDatetime) <= 0 &&
                 (*dayInfoCache)->cache_max_time.compare(ToDatetime)   >= 0  )
            {

                // if ( strcmp(nodeid,"Tag0117") == 0 )
                // {
                //     vilslog::printf ("MySQLDBAccess::getGPSAnalyzeDayInfoFromCache() Found\n");
                // }

                dayInfo = createAnalyzeDayInfoStruct();
                dayInfo->nodetype = nodetype;
                dayInfo->nodeid = (*dayInfoCache)->nodeid;
                dayInfo->max_time = (*dayInfoCache)->max_time;
                dayInfo->min_time = (*dayInfoCache)->min_time;
                dayInfo->count = (*dayInfoCache)->count;
                dayInfo->day = (*dayInfoCache)->day;
                dayInfo->course_id = (*dayInfoCache)->course_id;
                dayInfo->cache_max_time = (*dayInfoCache)->cache_max_time;
                dayInfo->cache_min_time = (*dayInfoCache)->cache_min_time;
            }
            break;
        }
    }
    return dayInfo;
}

AnalyzeDayInfoStruct * MySQLDBAccess::getAlertAnalyzeDayInfo(const char * macaddress, const char * FromDatetime, const char * ToDatetime)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT COUNT(alertid) as order_count, \
                max(issuetime) as max_time, \
                min(issuetime) as min_time, \
                SUBSTR( issuetime, 1, 10 ) as order_day \
                from AlertRecord WHERE \
                macaddress='%s' AND issuetime between '%s' and '%s' GROUP BY order_day",
                macaddress, FromDatetime, ToDatetime);

    AnalyzeDayInfoStruct * dayInfo = createAnalyzeDayInfoStruct();

    bool bSuccess = sql_executeQuery_new(cmd, get_AnalyzeDayInfo_callback, dayInfo);
    if (bSuccess)
    {
        update_to_alert_cache(macaddress, dayInfo->count, FromDatetime, ToDatetime, dayInfo->min_time.c_str(), dayInfo->max_time.c_str());

        return dayInfo;
    }
    else
    {
        std::string curDatetime = currentDateTimeForSQL();
        update_to_alert_cache(macaddress, 0, FromDatetime, ToDatetime, curDatetime.c_str(), curDatetime.c_str());

        delete dayInfo;
        return NULL;
    }
}

AnalyzeDayInfoStruct * MySQLDBAccess::getAlertAnalyzeDayInfoFromCache(const char * macaddress, const char * FromDatetime, const char * ToDatetime)
{
    std::string datetime_str = FromDatetime;
    std::string date_str = datetime_str.substr(0, 10);

    AnalyzeDayInfoStruct * dayInfo = NULL;

    for (std::list<AnalyzeDayInfoStruct *>::iterator dayInfoCache = mAlertAnalyzeDayInfoStructList.begin(); dayInfoCache != mAlertAnalyzeDayInfoStructList.end(); dayInfoCache++)
    {
        if ( ((*dayInfoCache)->nodeid.compare(macaddress) == 0) && ((*dayInfoCache)->day.compare(date_str) == 0))
        {
            if ( (*dayInfoCache)->cache_min_time.compare(FromDatetime) <= 0 &&
                 (*dayInfoCache)->cache_max_time.compare(ToDatetime)   >= 0  )
            {
                dayInfo = createAnalyzeDayInfoStruct();
                dayInfo->nodeid = (*dayInfoCache)->nodeid;
                dayInfo->max_time = (*dayInfoCache)->max_time;
                dayInfo->min_time = (*dayInfoCache)->min_time;
                dayInfo->count = (*dayInfoCache)->count;
                dayInfo->day = (*dayInfoCache)->day;
                dayInfo->course_id = (*dayInfoCache)->course_id;
                dayInfo->cache_max_time = (*dayInfoCache)->cache_max_time;
                dayInfo->cache_min_time = (*dayInfoCache)->cache_min_time;
            }
            break;
        }
    }
    return dayInfo;
}

AnalyzeDayInfoStruct * MySQLDBAccess::getEventAnalyzeDayInfo(const char * FromDatetime, const char * ToDatetime)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT SUBSTR( issuetime, 1, 10 ) as order_day, \
                max(datetime) as max_time, \
                min(datetime) as min_time \
                from FiredEvents WHERE \
                datetime between '%s' and '%s' GROUP BY order_day",
                FromDatetime, ToDatetime);

    AnalyzeDayInfoStruct * dayInfo = createAnalyzeDayInfoStruct();

    bool bSuccess = sql_executeQuery_new(cmd, get_AnalyzeDayInfo_callback, dayInfo);
    if (bSuccess)
        return dayInfo;
    else
    {
        delete dayInfo;
        return NULL;
    }
}

std::list<AssistStruct*> * MySQLDBAccess::read_Assists()
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM assist ");

    std::list<AssistStruct*> * assistlist = new std::list<AssistStruct*>();

    sql_executeQuery_new(cmd, get_AssistStructList_callback, assistlist);

    return assistlist;
}

AssistStruct * MySQLDBAccess::read_Assist(int assistid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM assist where id = %d ", assistid);

    AssistStruct * assistinfo = new AssistStruct();

    sql_executeQuery_new(cmd, get_AssistStruct_callback, assistinfo);

    return assistinfo;
}

std::list<AssistEventStruct*> * MySQLDBAccess::read_AssistEvents()
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM assistevent where signoffstatus=1 ");

    std::list<AssistEventStruct*> * assisteventlist = new std::list<AssistEventStruct*>();

    sql_executeQuery_new(cmd, get_AssistEventStructList_callback, assisteventlist);

    return assisteventlist;
}

AssistEventRecordStruct * MySQLDBAccess::get_AssistEventRecord(int assistid, int assisteventid, std::string eventday, std::string eventtime)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM assisteventrecord WHERE assistid=%d AND assisteventid=%d AND eventday='%s' AND eventtime='%s' ",
        assistid, assisteventid, eventday.c_str(), eventtime.c_str());

    AssistEventRecordStruct * assisteventrecord = new AssistEventRecordStruct();
    assisteventrecord->id = -1;
    assisteventrecord->PushNotifyMessageRecorddbId = -1;

    sql_executeQuery_new(cmd, get_AssistEventRecordStruct_callback, assisteventrecord);

    return assisteventrecord;
}

bool MySQLDBAccess::createAssistEventRecord(AssistEventRecordStruct * record)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO assisteventrecord(assistid, assisteventid, PushNotifyMessageRecorddbId, userid, messagehashkey, results, eventday, eventtime, maingroups) \
            VALUES (%d, %d, %d, %d, '%s', '%s', '%s', '%s', '%s')",
            record->assistid, record->assisteventid, record->PushNotifyMessageRecorddbId, record->userid, record->messagehashkey.c_str(), "",
            record->eventday.c_str(), record->eventtime.c_str(), record->maingroups.c_str());

    // vilslog::printf("MySQLDBAccess::createAssistEventRecord() cmd[%s]\n", cmd);

    sql_execute(cmd);
    return true;

}

bool MySQLDBAccess::updateAssistEventRecord(AssistEventRecordStruct * record)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::string results = convertListToString(&record->results);

    sprintf(cmd, "UPDATE assisteventrecord SET results='%s', userid=%d WHERE assistid=%d AND assisteventid=%d AND eventday='%s' AND eventtime='%s' ",
                results.c_str(), record->userid, record->assistid, record->assisteventid, record->eventday.c_str(), record->eventtime.c_str());

    // vilslog::printf("MySQLDBAccess::updateAssistEventRecord() cmd[%s]\n", cmd);

    sql_execute(cmd);
    return true;
}

bool MySQLDBAccess::updateSoccerRecord(SoccerRecordingStruct * record)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE SoccerRecord SET maingroupid=%d, starttime='%s', endtime='%s' WHERE recordnumber='%s';",
                    record->maingroupid, time_tToString(record->starttime).c_str(), time_tToString(record->endtime).c_str(), record->recordnumber.c_str());

    sql_execute(cmd);
    return true;
}

bool MySQLDBAccess::createSoccerRecord(SoccerRecordingStruct * record)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO SoccerRecord(recordnumber, maingroupid, starttime, endtime) VALUES ('%s', %d, '%s', '%s') ",
                    record->recordnumber.c_str(), record->maingroupid, time_tToString(record->starttime).c_str(), time_tToString(record->endtime).c_str());

    sql_execute(cmd);
    return true;
}

std::list<std::string> * MySQLDBAccess::getStorageImagesFromMessagehashkey(std::string messagehashkey)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::list<std::string> * imageuids = new std::list<std::string>();

    sprintf(cmd, "SELECT imageuid as stringvalue FROM StorageImage WHERE messagehashkey='%s' ", messagehashkey.c_str());

    sql_executeQuery_new(cmd, get_StringValueList_callback, imageuids);

    // vilslog::printf("MySQLDBAccess::getStorageImagesFromMessagehashkey() cmd[%s] imageuids->size()[%d]\n", cmd, (int)imageuids->size());

    return imageuids;
}

FireFightingStruct * MySQLDBAccess::createFireFightingCase(std::string address, std::string lat, std::string lng, int maingroupid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    FireFightingStruct * newCase = new FireFightingStruct();
    newCase->id = -1;
    newCase->address = address;
    newCase->lat = lat;
    newCase->lng = lng;
    newCase->maingroupid = maingroupid;
    newCase->StandbyDeviceIds = new std::list<std::string>();
    newCase->NFCDeviceIdList = new std::list<int>();

    int caseindex = 1;
    std::string today = TodayDate();

    bool bFound = false;
    while (!bFound)
    {
        int count = 0;

        // check if existed
        sprintf(cmd, "SELECT count(*) FROM FireFighting WHERE casenumber='%s%03d' ", today.c_str(), caseindex);

        sql_executeQuery_new(cmd, get_count_callback, &count);
        if ( count == 0 )
        {
            bFound = true;
        }
        else
        {
            caseindex++;
        }
    }

    memset(cmd, 0, 4096);
    sprintf(cmd, "%s%03d", today.c_str(), caseindex);

    newCase->caseclosed = 0;
    newCase->casenumber = cmd;
    newCase->starttime = currentDateTimeForSQL();

    updateFireFightingCase(newCase);

    return newCase;
}

bool MySQLDBAccess::updateFireFightingCase(FireFightingStruct * record)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::string StandbyDeviceIds = convert_list_to_string(record->StandbyDeviceIds);
    std::string NFCDeviceIds = convert_listint_to_string(record->NFCDeviceIdList);

    if (record->id < 0)
    {
        sprintf(cmd, "INSERT INTO FireFighting(casenumber, address, lat, lng, maingroupid, caseclosed, starttime, closetime, NFCDeviceIds, StandbyDeviceIds) \
                        VALUES ('%s', '%s', '%s', '%s', %d, %d, '%s', '%s', '%s', '%s') ",
                        record->casenumber.c_str(), record->address.c_str(), record->lat.c_str(), record->lng.c_str(),
                        record->maingroupid, record->caseclosed, record->starttime.c_str(), record->closetime.c_str(),
                        NFCDeviceIds.c_str(), StandbyDeviceIds.c_str());

        int dbId = sql_execute_get_insert_id(cmd);
        record->id = dbId;
    }
    else
    {
        sprintf(cmd, "UPDATE FireFighting SET casenumber='%s', address='%s', lat='%s', lng='%s', maingroupid=%d, caseclosed=%d, starttime='%s', closetime='%s', \
                        NFCDeviceIds='%s', StandbyDeviceIds='%s' WHERE id=%d;",
                        record->casenumber.c_str(), record->address.c_str(), record->lat.c_str(), record->lng.c_str(),
                        record->maingroupid, record->caseclosed, record->starttime.c_str(), record->closetime.c_str(),
                        NFCDeviceIds.c_str(), StandbyDeviceIds.c_str(), record->id );

        sql_execute(cmd);
    }

    return true;
}

bool MySQLDBAccess::updateFireFightingUser(FireFightingUserStruct * record)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::string enterTime = time_tToString(record->enterTime);

    sprintf(cmd, "INSERT INTO FireFightingUser(casenumber, userid, maingroupid, areaType, airremain, enterTime, activity, HR, SPO2, battery) \
            VALUES ('%s', %d, %d, %d, '%0.02f', '%s', '%s', '%s', '%s', '%s') \
            ON DUPLICATE KEY UPDATE maingroupid=VALUES(maingroupid), areaType=VALUES(areaType),airremain=VALUES(airremain), \
            enterTime=VALUES(enterTime), activity=VALUES(activity), HR=VALUES(HR), SPO2=VALUES(SPO2), battery=VALUES(battery)",
            record->casenumber.c_str(), record->userid, record->maingroupid, record->areaType, record->airremain,
            enterTime.c_str(), record->activity.c_str(), record->HR.c_str(), record->SPO2.c_str(), record->battery.c_str());

    sql_execute(cmd);

    return true;
}

bool MySQLDBAccess::updateFireFightingAirUsing(FireFightingAirUsingStruct * record)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::string starttime = time_tToString(record->starttime);
    std::string endtime = time_tToString(record->endtime);

    sprintf(cmd, "INSERT INTO FireFightingAirUsing(casenumber, userid, startairremain, starttime, endairremain, endtime, airusing, totaltime) \
            VALUES ('%s', %d, '%0.02f', '%s', '%0.02f', '%s', '%0.02f', '%d')",
            record->casenumber.c_str(), record->userid, record->startairremain, starttime.c_str(),
            record->endairremain, endtime.c_str(), record->airusing, (int)record->totaltime);

    sql_execute(cmd);

    return true;
}

bool MySQLDBAccess::updateUserFireFightingAirUsing(int userid, float airusing)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    FireFightingAirUsingStruct * record = new FireFightingAirUsingStruct();

    sprintf(cmd, "SELECT * FROM FireFightingAirUsing WHERE userid=%d AND casenumber='1';", userid);

    int ret = sql_executeQuery_new(cmd, get_FireFightingAirUsing_callback, record);

    vilslog::printf ("MySQLDBAccess::updateUserFireFightingAirUsing() userid[%d] airusing[%.02f] ret[%d]\n", userid, airusing, ret);

    if (ret == 0)
    {
        sprintf(cmd, "INSERT INTO FireFightingAirUsing(casenumber, userid, startairremain, starttime, endairremain, endtime, airusing, totaltime) \
                VALUES ('1', %d, '0', '0', '0', '0', '%0.02f', '0')", userid, airusing);
    }
    else
    {
        record->airusing = (record->airusing * 0.8f) + (airusing * 0.2f);

        sprintf(cmd, "UPDATE FireFightingAirUsing SET airusing='%0.02f' WHERE userid=%d AND casenumber='1';", record->airusing, userid);
    }

    sql_execute(cmd);

    delete record;

    return true;
}

bool MySQLDBAccess::addFireFightingEvent(std::string eventtype, std::string casenumber, std::string jsonstring)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO FireFightingEvent(casenumber, eventtype, jsonstring) VALUES ('%s', '%s', '%s')",
            casenumber.c_str(), eventtype.c_str(), jsonstring.c_str());

    sql_execute(cmd);

    return true;
}

std::list<FireFightingStruct*> * MySQLDBAccess::get_OpenFireFightingCases()
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::list<FireFightingStruct*> * pList = new std::list<FireFightingStruct*>();

    sprintf(cmd, "SELECT * FROM FireFighting where caseclosed=0;");

    sql_executeQuery_new(cmd, get_FireFightingList_callback, pList);

    return pList;
}

std::list<FireFightingUserStruct*> * MySQLDBAccess::get_FireFightingUsers(std::string casenumber)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::list<FireFightingUserStruct*> * pList = new std::list<FireFightingUserStruct*>();

    sprintf(cmd, "SELECT * FROM FireFightingUser where casenumber='%s';", casenumber.c_str());

    sql_executeQuery_new(cmd, get_FireFightingUserList_callback, pList);

    return pList;
}

std::list<FireFightingEventStruct*> * MySQLDBAccess::get_FireFightingEvents(std::string casenumber, std::string eventtype)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::list<FireFightingEventStruct*> * pList = new std::list<FireFightingEventStruct*>();

    sprintf(cmd, "SELECT * FROM FireFightingEvent WHERE casenumber='%s' AND eventtype='%s' ORDER BY datetime ASC;",
        casenumber.c_str(), eventtype.c_str());

    sql_executeQuery_new(cmd, get_FireFightingEventList_callback, pList);

    return pList;
}

std::list<SoccerRecordingStruct*> * MySQLDBAccess::get_SoccerRecording()
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::list<SoccerRecordingStruct*> * pList = new std::list<SoccerRecordingStruct*>();

    // sprintf(cmd, "SELECT * FROM SoccerRecord WHERE endtime='0' ORDER BY datetime DESC;");
    sprintf(cmd, "SELECT * FROM SoccerRecord ORDER BY datetime DESC;");

    sql_executeQuery_new(cmd, get_SoccerRecordList_callback, pList);

    return pList;
}

int MySQLDBAccess::sql_execute(const char * cmd)
{
    int ret = 0;
    try {
        //sql::Driver *driver;
        //sql::Connection *con;
        //sql::Statement *stmt;

        /* Create a connection */
        //driver = get_driver_instance();
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        //con = driver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password); //IP Address, user name, password
        con->setAutoCommit(true);

        //stmt = con->createStatement();
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());
        stmt->execute(cmd);
        // ret = stmt->executeUpdate(cmd);

        stmt->close();
        //delete stmt;
        //delete con;
    }
    catch (sql::SQLException &e) {

        vilslog::printf("MySQLDBAccess::sql_execute() sql::SQLException cmd[%s]\n", cmd);
        printSQLException(e);

        ret = -1;
    }

    return ret;
}

int MySQLDBAccess::sql_executeUpdate(const char * cmd)
{
    int ret = 0;
    try {
        //sql::Driver *driver;
        //sql::Connection *con;
        //sql::Statement *stmt;

        /* Create a connection */
        //driver = get_driver_instance();
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        //con = driver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password); //IP Address, user name, password
        con->setAutoCommit(true);

        //stmt = con->createStatement();
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());
        // stmt->execute(cmd);
        ret = stmt->executeUpdate(cmd);

        stmt->close();
        //delete stmt;
        //delete con;
    }
    catch (sql::SQLException &e) {

        vilslog::printf("MySQLDBAccess::sql_executeUpdate() sql::SQLException cmd[%s]\n", cmd);
        printSQLException(e);

        ret = -1;
    }

    return ret;
}

int MySQLDBAccess::sql_execute_get_insert_id(const char * cmd)
{
    int insert_id = 0;
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        con->setAutoCommit(true);

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());
        stmt->execute(cmd);

        sql::ResultSet * res = stmt->executeQuery("SELECT LAST_INSERT_ID() AS id");
        if ( res != NULL)
        {
            res->next();
            insert_id = res->getInt64("id");
            // vilslog::printf("MySQLDBAccess::sql_execute() insert_id[%d]\n", insert_id);
            delete res;
        }

        stmt->close();
    }
    catch (sql::SQLException &e) {

        vilslog::printf("MySQLDBAccess::sql_execute() sql::SQLException cmd[%s]\n", cmd);
        printSQLException(e);

        insert_id = -1;
    }

    return insert_id;
}

bool MySQLDBAccess::sql_executeQuery_new(const char * cmd, CALLBACK callback, void * retValue)
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
        printSQLException(e);
        return false;
    }

    return bRet;
}

bool MySQLDBAccess::checkDBExist()
{
    //cout << "checkDBExist(): " << endl;

    try {
        //sql::Driver *driver;
        //sql::Connection *con;
        //sql::Statement *stmt;

        /* Create a connection */
        //driver = get_driver_instance();
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        //con = driver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password); //IP Address, user name, password
        //stmt = con->createStatement();
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());
        stmt->close();
        //delete stmt;
        //delete con;
    } catch (sql::SQLException &e) {

        printSQLException(e);

        return false;
    }

    return true;
}

int MySQLDBAccess::create_table()
{
    // cout << "MySQLDBAccess::create_table() Started" << endl;

    try {
        //sql::Driver *driver;
        //sql::Connection *con;
        //sql::Statement *stmt;

        /* Create a connection */
        //driver = get_driver_instance();
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        //con = driver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password); //IP Address, user name, password

        //stmt = con->createStatement();
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

        //===============
        // create a new table
        // events
        //stmt->execute("DROP TABLE IF EXISTS events"); //drop if  table exists
        stmt->execute("CREATE TABLE events( \
                    id INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
                    PROJECT_ID INT(3) unsigned zerofill, \
                    nodeid CHAR(30), \
                    action CHAR(30), \
                    areaid CHAR(30), \
                    issuetime CHAR(30), \
                    datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
                    ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci"); //create table with (column name as id accepting INT) and (column name as label accepting CHAR(1))

        //stmt->execute("INSERT INTO test(id, label) VALUES (1, 'a')"); //insert into 'test' table with (1 and 'a')
        //===============

        //===============
        // create a new table
        // FiredEvents
        // stmt->execute("DROP TABLE IF EXISTS FiredEvents"); //drop if table exists
        // stmt->execute("CREATE TABLE FiredEvents( \
        //      id INT(6) UNSIGNED AUTO_INCREMENT, \
        //      PROJECT_ID INT(3) unsigned zerofill, \
        //      empid char(30), \
        //      comment TEXT, \
        //      eventid CHAR(30) NOT NULL, \
        //      eventjson TEXT NOT NULL, \
        //      timestart DATETIME NOT NULL, \
        //      timeend DATETIME NOT NULL, \
        //      readdatetime DATETIME, \
        //      datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
        //      PRIMARY KEY (id, eventid, timestart, timeend)) \
        //      ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");
        // ===============

        //===============
        // create a new table
        // Footprint
        // stmt->execute("DROP TABLE IF EXISTS Footprint"); //drop if table exists
        stmt->execute("CREATE TABLE Footprint( \
            nodetype INT(3) NOT NULL, \
            nodeid CHAR(30) NOT NULL, \
            idx INT(3) NOT NULL, \
            areaid CHAR(30) NOT NULL, \
            posX INT(3), \
            posY INT(3), \
            posZ INT(3), \
            voltage CHAR(30), \
            temperature CHAR(30), \
            datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
            CONSTRAINT PK_nodeft PRIMARY KEY (nodetype, nodeid, idx)) \
            ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");
        //===============

        //===============
        // create a new table
        // NodeInfo
        // stmt->execute("DROP TABLE IF EXISTS NodeInfo"); //drop if table exists
        stmt->execute("CREATE TABLE NodeInfo( \
            nodetype INT(3) NOT NULL, \
            macaddress CHAR(30) NOT NULL, \
            loramacaddress CHAR(30) NOT NULL, \
            nodeid CHAR(30) NOT NULL, \
            tafid INT(3), \
            buildingid INT(3), \
            buildingfloor INT(3), \
            nodename CHAR(30) NOT NULL, \
            idx INT(3) NOT NULL, \
            posX INT(3), \
            posY INT(3), \
            posZ INT(3), \
            Longitude CHAR(30), \
            Latitude CHAR(30), \
            voltage CHAR(30), \
            temperature CHAR(30), \
            IP CHAR(30), \
            version CHAR(30), \
            AreaId CHAR(30), \
            AreaName CHAR(30), \
            blacklist TEXT, \
            whitelist TEXT, \
            joinRangingRange INT(5), \
            joinNotRangingRange INT(5), \
            resident_serno CHAR(30), \
            empid CHAR(30), \
            panid CHAR(30), \
            opmode INT(3), \
            gwid CHAR(30), \
            alive INT(3), \
            maplayer INT(3), \
            poweronTimestamp CHAR(30), \
            poweroffTimestamp CHAR(30), \
            enableSensorAlert INT(3), \
            userid INT(3), \
            maingroupid INT(3) default 1, \
            verifykey CHAR(8), \
            datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
            PRIMARY KEY (nodetype, macaddress)) \
            ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");
        //===============

        stmt->close();
        //delete stmt;
        //delete con;
        /*According to documentation,
        You must free the sql::Statement and sql::Connection objects explicitly using delete
        But do not explicitly free driver, the connector object. Connector/C++ takes care of freeing that. */

    } catch (sql::SQLException &e) {

        printSQLException(e);

    }

    // cout << "MySQLDBAccess::create_table() Successfully ended" << endl;

    return 0;
}

bool MySQLDBAccess::check_resident_table()
{
    if (tableExists("resident"))
    {
    }
    else
    {
        vilslog::printf("check_resident_table() resident NOT existed!\n");
        create_resident_table();
    }
    return true;
}

bool MySQLDBAccess::create_resident_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        stmt->execute("CREATE TABLE resident(" \
              "id INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY," \
              "serno CHAR(30)," \
              "gender CHAR(30)," \
              "name CHAR(128)," \
              "roomNo CHAR(128)," \
              "residentType CHAR(30)," \
              "nurseNo CHAR(30)," \
              "datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
              ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");
            //===============


        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_employee_table()
{
    if (tableExists("employee"))
    {
    }
    else
    {
        vilslog::printf("check_employee_table() employee NOT existed!\n");
        create_employee_table();
    }
    return true;
}

bool MySQLDBAccess::create_employee_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        stmt->execute("CREATE TABLE employee(" \
              "id INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY," \
              "empid CHAR(30)," \
              "cname CHAR(30)," \
              "orgid CHAR(30)," \
              "orgName CHAR(128)," \
              "area CHAR(30)," \
              "jobTitle CHAR(30)," \
              "gender CHAR(30)," \
              "pwd CHAR(128)," \
              "datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
              ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");
            //===============


        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_User_table()
{
    if (tableExists("User"))
    {
        createNonExistColumn("User", "AreaId", "CHAR(30)");
        createNonExistColumn("User", "AreaName", "CHAR(30)");
        createNonExistColumn("User", "AreaId0D", "CHAR(30)");
        createNonExistColumn("User", "AreaType0D", "INT(3) NOT NULL DEFAULT 0");
        createNonExistColumn("User", "AreaType5Id", "CHAR(30)");
        createNonExistColumn("User", "AreaType5Name", "CHAR(30)");


        // createNonExistColumn("User", "maplayer", "INT(3) NOT NULL DEFAULT 0");
        // createNonExistColumn("User", "posX", "INT(3) NOT NULL DEFAULT 0");
        // createNonExistColumn("User", "posY", "INT(3) NOT NULL DEFAULT 0");
        // createNonExistColumn("User", "posZ", "INT(3) NOT NULL DEFAULT 0");
        // createNonExistColumn("User", "Longitude", "CHAR(30)");
        // createNonExistColumn("User", "Latitude", "CHAR(30)");
        // createNonExistColumn("User", "sensors", "TEXT");
        // createNonExistColumn("User", "maingroups", "TEXT");
        // createNonExistColumn("User", "type", "INT(3) default 0");
        // createNonExistColumn("User", "imageuid", "TEXT");
    }
    else
    {
        vilslog::printf("check_User_table() User NOT existed!\n");
        create_User_table();
    }
    return true;
}

bool MySQLDBAccess::create_User_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS User"); //drop if table exists
        stmt->execute("CREATE TABLE User(" \
              "userid INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY," \
              "type INT(3) default 0," \
              "groupid INT(3)," \
              "empid CHAR(30)," \
              "cardid TEXT," \
              "account CHAR(30)," \
              "name CHAR(30)," \
              "unit CHAR(30)," \
              "title CHAR(30)," \
              "gender CHAR(30)," \
              "pwd CHAR(128)," \
              "maingroups TEXT," \
              "datagroups TEXT," \
              "subgroups TEXT," \
              "rfidcards TEXT," \
              "nfccards TEXT," \
              "uwbcards TEXT," \
              "sensors TEXT," \
              "imageuid TEXT," \
              "maplayer INT(3) NOT NULL DEFAULT 0," \
              "posX INT(3) NOT NULL DEFAULT 0," \
              "posY INT(3) NOT NULL DEFAULT 0," \
              "posZ INT(3) NOT NULL DEFAULT 0," \
              "Longitude CHAR(30)," \
              "Latitude CHAR(30)," \
              "AreaId CHAR(30)," \
              "AreaName CHAR(30)," \
              "AreaId0D CHAR(30)," \
              "AreaType0D INT(3) NOT NULL DEFAULT 0," \
              "AreaType5Id CHAR(30)," \
              "AreaType5Name CHAR(30)," \
              "datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
              ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");
            //===============

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::create_User_data(int userid, int groupid, const char * account, const char * name, const char * unit, const char * title, const char * gender, const char * password)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    //std::string pwd = create_MD5_encrypt(std::string(ERP_MD5_KEY), std::string(password));
    std::string pwd = create_MD5_encrypt_salt(std::string(ERP_MD5_KEY), std::string(password));

    sprintf(cmd, "INSERT INTO User(userid, groupid, account, name, unit, title, gender, pwd, maingroups) \
            VALUES (%d, %d, '%s', '%s', '%s', '%s', '%s', '%s', '1') \
            ON DUPLICATE KEY UPDATE groupid=VALUES(groupid), account=VALUES(account), name=VALUES(name), \
            unit=VALUES(unit), title=VALUES(title), gender=VALUES(gender), pwd=VALUES(pwd), maingroups=VALUES(maingroups)",
            userid, groupid, account, name, unit, title, gender, pwd.c_str());

    sql_execute(cmd);


    return true;
}

bool MySQLDBAccess::check_UserGroup_table()
{
    if (tableExists("UserGroup"))
    {
    }
    else
    {
        vilslog::printf("check_UserGroup_table() UserGroup NOT existed!\n");
        create_UserGroup_table();
    }
    return true;
}

bool MySQLDBAccess::create_UserGroup_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        stmt->execute("CREATE TABLE UserGroup(" \
              "groupid INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY," \
              "groupname CHAR(30)," \
              "permission TEXT," \
              "datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
              ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");
            //===============


        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::create_UserGroup_data(int groupid, const char * groupname, const char * permission)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO UserGroup(groupid, groupname, permission) \
            VALUES (%d, '%s', '%s') ON DUPLICATE KEY UPDATE groupname=VALUES(groupname), permission=VALUES(permission)",
            groupid, groupname, permission);

    int ret = sql_execute(cmd);

    if (ret < 0)
    {
        printf("MySQLDBAccess::create_UserGroup_data() 1 cmd[%s] ret[%d]\n", cmd, ret);
    }
    return true;
}

bool MySQLDBAccess::delete_UserGroup_data(int groupid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "DELETE FROM UserGroup WHERE groupid >= %d ", groupid);

    int ret = sql_execute(cmd);

    if (ret < 0)
    {
        printf("MySQLDBAccess::delete_UserGroup_data() 1 cmd[%s] ret[%d]\n", cmd, ret);
    }
    return true;
}

bool MySQLDBAccess::check_UserSubGroup_table()
{
    if (tableExists("UserSubGroup"))
    {
        //
        // type 0:normal 1:curriculum
        //
        createNonExistColumn("UserSubGroup", "type", "INT(3) default 0");
        createNonExistColumn("UserSubGroup", "validdate", "DATE");
        createNonExistColumn("UserSubGroup", "curriculumrecordid", "INT(3) default 0");

        //createNonExistColumn("UserSubGroup", "imageuid", "TEXT");
        //createNonExistColumn("UserSubGroup", "maingroupid", "INT(3) default 1");
        //createNonExistColumn("UserSubGroup", "userids", "TEXT");
        // createNonExistColumn("UserSubGroup", "accountids", "TEXT");
    }
    else
    {
        vilslog::printf("check_UserSubGroup_table() UserSubGroup NOT existed!\n");
        create_UserSubGroup_table();
    }
    return true;
}

bool MySQLDBAccess::create_UserSubGroup_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        stmt->execute("CREATE TABLE UserSubGroup(" \
              "subgroupid INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY," \
              "subgroupname CHAR(30)," \
              "maingroupid INT(3) default 1," \
              "permission TEXT," \
              "accountids TEXT," \
              "userids TEXT," \
              "imageuid TEXT," \
              "type INT(3) default 0," \
              "validdate DATE," \
              "curriculumrecordid INT(3) default 0," \
              "datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
              ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");
            //===============

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::create_UserSubGroup_data( const char * subgroupname, const char * permission)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO UserSubGroup(subgroupname, permission) \
            VALUES ('%s', '%s') ON DUPLICATE KEY UPDATE subgroupname=VALUES(subgroupname), permission=VALUES(permission)",
            subgroupname, permission);

    int ret = sql_execute(cmd);

    if (ret < 0)
    {
        printf("MySQLDBAccess::create_UserSubGroup_data() cmd[%s] ret[%d]\n", cmd, ret);
    }
    return true;
}

bool MySQLDBAccess::check_UserMainGroup_table()
{
    if (tableExists("UserMainGroup"))
    {
        createNonExistColumn("UserMainGroup", "welcommessage", "TEXT");
        createNonExistColumn("UserMainGroup", "autojoin", "INT(3) default 0");
        createNonExistColumn("UserMainGroup", "imageuid", "TEXT");
        createNonExistColumn("UserMainGroup", "userids", "TEXT");
    }
    else
    {
        vilslog::printf("check_UserMainGroup_table() UserMainGroup NOT existed!\n");
        create_UserMainGroup_table();
    }
    return true;
}

bool MySQLDBAccess::create_UserMainGroup_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        stmt->execute("CREATE TABLE UserMainGroup(" \
              "id INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY," \
              "name CHAR(30)," \
              "permission TEXT," \
              "accountids TEXT," \
              "userids TEXT," \
              "imageuid TEXT," \
              "autojoin INT(3) default 0," \
              "welcommessage TEXT," \
              "datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
              ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");
            //===============

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::create_UserMainGroup_data( const char * maingroupname, const char * permission)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO UserMainGroup(name, permission) \
            VALUES ('%s', '%s') ON DUPLICATE KEY UPDATE name=VALUES(name), permission=VALUES(permission)",
            maingroupname, permission);

    int ret = sql_execute(cmd);

    if (ret < 0)
    {
        printf("MySQLDBAccess::create_UserMainGroup_data() cmd[%s] ret[%d]\n", cmd, ret);
    }
    return true;
}

bool MySQLDBAccess::create_Survey_data(const char * surveyid , const char * title, const char * description, int type, int userid, const char * maingroups, const char * subgroups)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO Survey(surveyid, title, description, type, userid, maingroups, subgroups) \
            VALUES (%s, '%s', '%s', %d, %d, '%s', '%s') ON DUPLICATE KEY UPDATE title=VALUES(title), \
            description=VALUES(description), type=VALUES(type), userid=VALUES(userid), maingroups=VALUES(maingroups), subgroups=VALUES(subgroups)",
            surveyid, title, description, type, userid, maingroups, subgroups);

    int ret = sql_execute(cmd);

    if (ret < 0)
    {
        printf("MySQLDBAccess::create_Survey_data() cmd[%s] ret[%d]\n", cmd, ret);
    }
    return true;
}

bool MySQLDBAccess::create_SurveyItem_data(const char * surveyid , const char * description)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO SurveyItem(surveyid, description) VALUES (%s, '%s') ON DUPLICATE KEY UPDATE description=VALUES(description)",
            surveyid, description);

    int ret = sql_execute(cmd);

    if (ret < 0)
    {
        printf("MySQLDBAccess::create_Survey_data() cmd[%s] ret[%d]\n", cmd, ret);
    }
    return true;
}

bool MySQLDBAccess::check_FiredEvents_table()
{
    if (tableExists("FiredEvents"))
    {
        createNonExistColumn("FiredEvents", "maingroupid", "INT(3) default 1");
    }
    else
    {
        vilslog::printf("check_FiredEvents_table() FiredEvents NOT existed!\n");
        create_FiredEvents_table();
    }
    return true;
}

bool MySQLDBAccess::create_FiredEvents_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        stmt->execute("CREATE TABLE FiredEvents( \
             id INT(6) UNSIGNED AUTO_INCREMENT, \
             PROJECT_ID INT(3) unsigned zerofill, \
             empid char(30), \
             comment TEXT, \
             eventid CHAR(30) NOT NULL, \
             eventjson TEXT NOT NULL, \
             timestart DATETIME NOT NULL, \
             timeend DATETIME NOT NULL, \
             readdatetime DATETIME, \
             datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
             maingroupid INT(3) default 1, \
             PRIMARY KEY (id, eventid, timestart, timeend)) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_NodeInfo_table()
{
    //printf("SQLiteDBAccess::check_NodeInfo_table() start...\n");

    if (tableExists("NodeInfo"))
    {
        createNonExistColumn("NodeInfo","verifykey","CHAR(8)");
        createNonExistColumn("NodeInfo", "maingroupid", "INT(3) default 1");
    }
    else
    {
        vilslog::printf("check_NodeInfo_table() NodeInfo NOT existed!\n");
    }
    //printf("SQLiteDBAccess::check_NodeInfo_table() finished!\n");
    return true;
}

bool MySQLDBAccess::check_SectionGroup_table()
{
    if (tableExists("SectionGroup"))
    {
        createNonExistColumn("SectionGroup", "maingroupid", "INT(3) default 0");
    }
    else
    {
        vilslog::printf("check_sectiongroup_table() SectionGroup NOT existed!\n");
        create_SectionGroup_table();
    }
    return true;
}

bool MySQLDBAccess::create_SectionGroup_table()
{
    try {
        //sql::Driver *driver;
        //sql::Connection *con;
        //sql::Statement *stmt;

        /* Create a connection */
        //driver = get_driver_instance();
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        //con = driver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password); //IP Address, user name, password

        //stmt = con->createStatement();
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        stmt->execute("CREATE TABLE SectionGroup(" \
              "GroupID INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY," \
              "PROJECT_ID INT(3)," \
              "SectionType INT(3)," \
              "nodeIDList CHAR(128)," \
              "AreaID CHAR(30)," \
              "Param1 INT(3)," \
              "Param2 INT(3)," \
              "Param3 INT(3)," \
              "Param4 INT(3)," \
              "Param5 INT(3)," \
              "Param6 INT(3)," \
              "maingroupid INT(3) default 0," \
              "datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
              ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");
            //===============


        stmt->close();
        //delete stmt;
        //delete con;
        /*According to documentation,
        You must free the sql::Statement and sql::Connection objects explicitly using delete
        But do not explicitly free driver, the connector object. Connector/C++ takes care of freeing that. */

    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_AlertRecord_table()
{
    if (tableExists("AlertRecord"))
    {
    }
    else
    {
        vilslog::printf("check_AlertRecord_table() AlertRecord NOT existed!\n");
        create_AlertRecord_table();
    }

    return true;
}

bool MySQLDBAccess::create_AlertRecord_table()
{
    try {
        //sql::Driver *driver;
        //sql::Connection *con;
        //sql::Statement *stmt;

        /* Create a connection */
        //driver = get_driver_instance();
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        //con = driver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password); //IP Address, user name, password

        //stmt = con->createStatement();
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        stmt->execute("CREATE TABLE AlertRecord( \
             alertid INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
             nodetype INT(3) NOT NULL, \
             macaddress CHAR(30) NOT NULL, \
             issuetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
        //delete stmt;
        //delete con;
        /*According to documentation,
        You must free the sql::Statement and sql::Connection objects explicitly using delete
        But do not explicitly free driver, the connector object. Connector/C++ takes care of freeing that. */

    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_systemconfig_table()
{
    if (tableExists("systemconfig"))
    {
        createNonExistColumn("systemconfig", "sendNotifyTime", "INT(6) default 480");
        // createNonExistColumn("systemconfig", "PushNotifyWelcomeTitle", "TEXT");
        // createNonExistColumn("systemconfig", "PushNotifyWelcomeBody", "TEXT");
    }
    else
    {
        vilslog::printf("check_systemconfig_table() systemconfig table NOT existed!\n");
        create_systemconfig_table();
    }

    return true;
}

bool MySQLDBAccess::create_systemconfig_table()
{
    try {
        //sql::Driver *driver;
        //sql::Connection *con;
        //sql::Statement *stmt;

        /* Create a connection */
        //driver = get_driver_instance();
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
        //con = driver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password); //IP Address, user name, password

        //stmt = con->createStatement();
        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        stmt->execute("CREATE TABLE systemconfig( \
            CONFIG_ID INT(3) unsigned zerofill PRIMARY KEY, \
            SOLVETAGPOSITION_TIME INT(6), \
            SENDTAFDB_TIME INT(6), \
            SENDTAGANCHORLIST_TIME INT(6), \
            SENDDEVICESTATUS_TIME INT(6), \
            UPDATENODEFROMSMIMSIOTSERVER_TIME INT(6), \
            web_udp_port INT(6), \
            center_udp_port INT(6), \
            coord_udp_port INT(6), \
            center_broadcast_time INT(6), \
            center_broadcast_port INT(6), \
            web_socket_port INT(6), \
            USING_PROTOCOL INT(3), \
            VILSServer CHAR(30), \
            VILSUID CHAR(8), \
            ERPServer CHAR(30), \
            SSOIP CHAR(30), \
            PushNotifyPrefix CHAR(30), \
            PushNotifyWelcomeTitle TEXT, \
            PushNotifyWelcomeBody TEXT, \
            EnablePushNotify INT(1), \
            TagAlertTimeout INT(6), \
            TagAlertVoltageLow CHAR(30), \
            VALIDDATATIME INT(6), \
            MAX_RANGE_DISTANCE INT(6), \
            EnableNoiseMakup INT(6), \
            NoiseMakupRatio FLOAT(20,12), \
            NoiseMakupWeight FLOAT(20,12), \
            EnableKalman1D INT(6), \
            EnableRangingReliability INT(6), \
            RangingReliabilityMaxDistance INT(6), \
            RangingReliabilityMinCount INT(6), \
            RangingReliabilityAverageDiff INT(6), \
            EnableAnchor1DMapping INT(6), \
            EnableOneAnchorCalculation INT(6), \
            EnableGDOP INT(6), \
            GDOPThreshold INT(6), \
            CalculationMethod INT(6), \
            EnableIMUVelocity INT(6), \
            IMUVelocityOffset INT(6), \
            SteadyStateOffset INT(6), \
            WeightMovingAverageValue1 FLOAT(20,12), \
            WeightMovingAverageValue2 FLOAT(20,12), \
            WeightMovingAverageValue3 FLOAT(20,12), \
            StayInWallTimeout INT(6), \
            EnableFixHeight INT(6), \
            SmoothingResult INT(6), \
            SmoothingResultOffset INT(6), \
            SmoothingResultStep INT(6), \
            SmoothingResultFirstStateOffsetRatio FLOAT(20,12), \
            SmoothingResultFirstStateRunTime INT(6), \
            SmoothingResultSecondStateOffsetRatio FLOAT(20,12), \
            SmoothingResultSecondStateRunTime INT(6), \
            LoRAWANTPOIC CHAR(64), \
            MQTTHostIp CHAR(30), \
            MQTTHostPort INT(6), \
            MQTTHostUsername CHAR(30), \
            MQTTHostPassword CHAR(30), \
            MQTTClientId CHAR(64), \
            sendNotifyTime INT(6) default 480, \
            datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
            ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
        //delete stmt;
        //delete con;
        /*According to documentation,
        You must free the sql::Statement and sql::Connection objects explicitly using delete
        But do not explicitly free driver, the connector object. Connector/C++ takes care of freeing that. */

    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_Course_table()
{
    if (tableExists("Course"))
    {
        createNonExistColumn("Course", "maingroupid", "INT(3) default 1");
    }
    else
    {
        vilslog::printf("check_Course_table() Course table NOT existed!\n");
        create_Course_table();
    }

    return true;
}

bool MySQLDBAccess::create_Course_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS Course"); //drop if  table exists
        stmt->execute("CREATE TABLE Course( \
                      dbId INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
                      PROJECT_ID INT(3) NOT NULL, \
                      allDay INT(3) NOT NULL, \
                      resourceId CHAR(30) NOT NULL, \
                      courseId CHAR(30) NOT NULL, \
                      title CHAR(30) NOT NULL, \
                      description TEXT NOT NULL, \
                      start CHAR(30), \
                      end CHAR(30), \
                      startTime CHAR(30), \
                      endTime CHAR(30), \
                      daysOfWeek CHAR(30), \
                      startRecur CHAR(30), \
                      endRecur CHAR(30), \
                      backgroundColor CHAR(30), \
                      borderColor CHAR(30), \
                      maingroupid INT(3) default 1, \
                      issuetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP) \
                      ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_CourseRollCallRecord_table()
{
    if (tableExists("CourseRollCallRecord"))
    {
        //create_CourseRollCallRecord_table();
    }
    else
    {
        vilslog::printf("check_CourseRollCallRecord_table() CourseRollCallRecord table NOT existed!\n");
        create_CourseRollCallRecord_table();
    }

    return true;
}

bool MySQLDBAccess::create_CourseRollCallRecord_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS CourseRollCallRecord"); //drop if  table exists

        stmt->execute("CREATE TABLE CourseRollCallRecord( \
                      PROJECT_ID INT(3) NOT NULL, \
                      courseId CHAR(30) NOT NULL, \
                      infoOfDate CHAR(30) NOT NULL, \
                      tags TEXT NOT NULL, \
                      issuetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
                      PRIMARY KEY (courseId, infoOfDate) ) \
                      ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_AreaInOutRecord_table()
{
    if (tableExists("AreaInOutRecord"))
    {
    }
    else
    {
        vilslog::printf("check_AreaInOutRecord_table() AreaInOutRecord table NOT existed!\n");
        create_AreaInOutRecord_table();
    }

    return true;
}

bool MySQLDBAccess::create_AreaInOutRecord_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS AreaInOutRecord"); //drop if  table exists

        stmt->execute("CREATE TABLE AreaInOutRecord( \
                      dbId INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
                      PROJECT_ID INT(3) NOT NULL, \
                      AreaType INT(3) default 0, \
                      action INT(3) default 0, \
                      tafid INT(3) default 0, \
                      MapAreaType INT(3) default 0, \
                      NodeId CHAR(30) NOT NULL, \
                      NodeName CHAR(30) NOT NULL, \
                      AreaID CHAR(30) NOT NULL, \
                      AreaName CHAR(30) NOT NULL, \
                      DataTime CHAR(30) NOT NULL, \
                      issuetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP)\
                      ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_GPSInfoRecord_table()
{
    if (tableExists("GPSInfoRecord"))
    {
        createNonExistColumn("GPSInfoRecord", "nodetype", "INT(3) NOT NULL DEFAULT 1");
    }
    else
    {
        vilslog::printf("check_GPSInfoRecord_table() GPSInfoRecord table NOT existed!\n");
        create_GPSInfoRecord_table();
    }

    return true;
}

bool MySQLDBAccess::create_GPSInfoRecord_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS GPSInfoRecord"); //drop if  table exists

        stmt->execute("CREATE TABLE GPSInfoRecord( \
                      dbId INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
                      PROJECT_ID INT(3) NOT NULL, \
                      nodetype INT(3) NOT NULL DEFAULT 1, \
                      NodeId CHAR(30) NOT NULL, \
                      Longitude CHAR(30), \
                      Latitude CHAR(30), \
                      DataTime CHAR(30) NOT NULL, \
                      issuetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP)\
                      ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_LoRaGatewayInfo_table()
{
    if (tableExists("LoRaGatewayInfo"))
    {
        createNonExistColumn("LoRaGatewayInfo","verifykey","CHAR(8)");
        createNonExistColumn("LoRaGatewayInfo", "maingroupid", "INT(3) NOT NULL default 1");
    }
    else
    {
        vilslog::printf("check_LoRaGatewayInfo_table() LoRaGatewayInfo table NOT existed!\n");
        create_LoRaGatewayInfo_table();
    }

    return true;
}

bool MySQLDBAccess::create_LoRaGatewayInfo_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS LoRaGatewayInfo"); //drop if  table exists

        stmt->execute("CREATE TABLE LoRaGatewayInfo( \
                      gwid CHAR(30) PRIMARY KEY, \
                      PROJECT_ID INT(3) NOT NULL, \
                      sf INT(3) NOT NULL, \
                      gwip CHAR(30) NOT NULL, \
                      repeater CHAR(30) NOT NULL, \
                      channel TEXT NOT NULL, \
                      maplayer INT(3) NOT NULL, \
                      posX INT(3) NOT NULL, \
                      posY INT(3) NOT NULL, \
                      posZ INT(3) NOT NULL, \
                      maingroupid INT(3) NOT NULL default 1, \
                      verifykey CHAR(8), \
                      issuetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP)\
                      ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_EmployeeEventReport_table()
{
    if (tableExists("EmployeeEventReport"))
    {
    }
    else
    {
        vilslog::printf("check_EmployeeEventReport_table() EmployeeEventReport table NOT existed!\n");
        create_EmployeeEventReport_table();
    }

    return true;
}

bool MySQLDBAccess::create_EmployeeEventReport_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS EmployeeEventReport"); //drop if  table exists

        stmt->execute("CREATE TABLE EmployeeEventReport( \
                      eventid_alert INT(3) NOT NULL, \
                      eventid_cancel INT(3) NOT NULL, \
                      emp_empid CHAR(30) NOT NULL, \
                      emp_cname CHAR(30) NOT NULL, \
                      emp_orgName CHAR(30) NOT NULL, \
                      emp_jobTitle CHAR(30) NOT NULL, \
                      comment TEXT NOT NULL, \
                      action CHAR(30) NOT NULL, \
                      nodeid CHAR(30) NOT NULL, \
                      locatorid CHAR(30) NOT NULL, \
                      resident_serno CHAR(30) NOT NULL, \
                      resident_name CHAR(30) NOT NULL, \
                      resident_roomNo CHAR(30) NOT NULL, \
                      eventtime CHAR(30) NOT NULL, \
                      reponsetime CHAR(30) NOT NULL, \
                      datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
                      PRIMARY KEY (eventid_alert, eventid_cancel, emp_empid, resident_serno) ) \
                      ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_PushNotifyUser_table()
{
    if (tableExists("PushNotifyUser"))
    {
    }
    else
    {
        vilslog::printf("check_PushNotifyUser_table() PushNotifyUser table NOT existed!\n");
        create_PushNotifyUser_table();
    }

    return true;
}

bool MySQLDBAccess::create_PushNotifyUser_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS PushNotifyUser"); //drop if  table exists

        stmt->execute("CREATE TABLE PushNotifyUser( \
                    dbId INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
                    account CHAR(30) NOT NULL, \
                    topic CHAR(30), \
                    functions TEXT, \
                    datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
                    ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_maplayer_table()
{
    if (tableExists("maplayer"))
    {
        createNonExistColumn("maplayer", "maingroupid", "INT(3) NOT NULL default 1");
    }
    else
    {
        vilslog::printf("check_maplayer_table() maplayer table NOT existed!\n");
        create_maplayer_table();
    }

    return true;
}

bool MySQLDBAccess::create_maplayer_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS maplayer"); //drop if  table exists

        // map->POSX = "0.0";
        // map->POSY = "0.0";
        // map->SCALE = "51.933455519444976";
        // map->COEFF = "100000.0";
        // map->FILE = "/upload/2dgrid.png";
        // map->MAPWIDTH = "600";
        // map->MAPHEIGHT = "600";

        stmt->execute("CREATE TABLE maplayer( \
                    layerid INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
                    layer_name varchar(50) COLLATE utf8mb4_unicode_ci NOT NULL, \
                    layer_description varchar(50) COLLATE utf8mb4_unicode_ci NOT NULL, \
                    WGS48OriginX CHAR(30), \
                    WGS48OriginY CHAR(30), \
                    MAP_North CHAR(30), \
                    BuildingID INT(3), \
                    BuildingFloor INT(3), \
                    POSX CHAR(30), \
                    POSY CHAR(30), \
                    SCALE CHAR(30), \
                    COEFF CHAR(30), \
                    FILE TEXT, \
                    MAPWIDTH CHAR(30), \
                    MAPHEIGHT CHAR(30), \
                    maingroupid INT(3) NOT NULL default 1, \
                    datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
                    ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_maplayerarea_table()
{
    if (tableExists("maplayerarea"))
    {
    }
    else
    {
        vilslog::printf("check_maplayerarea_table() maplayerarea table NOT existed!\n");
        create_maplayerarea_table();
    }

    return true;
}

bool MySQLDBAccess::create_maplayerarea_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS maplayerarea"); //drop if  table exists
        stmt->execute("CREATE TABLE maplayerarea( \
                    layerid INT(3) unsigned zerofill, \
                    areaid CHAR(30) NOT NULL, \
                    areaname CHAR(30), \
                    geojson TEXT, \
                    type INT(3) unsigned zerofill, \
                    datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
                    PRIMARY KEY (layerid, areaid)) \
                    ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_PushNotifyMessage_table()
{
    if (tableExists("PushNotifyMessage"))
    {
        createNonExistColumn("PushNotifyMessage", "maingroups", "TEXT NOT NULL");
    }
    else
    {
        vilslog::printf("check_PushNotifyMessage_table() PushNotifyMessage table NOT existed!\n");
        create_PushNotifyMessage_table();
    }

    return true;
}

bool MySQLDBAccess::create_PushNotifyMessage_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS PushNotifyMessage"); //drop if  table exists
        stmt->execute("CREATE TABLE PushNotifyMessage( \
                    dbId INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
                    type INT(3), \
                    nodeid CHAR(30), \
                    targetid CHAR(30), \
                    topic TEXT, \
                    messagehashkey TEXT, \
                    messageid TEXT, \
                    maingroups TEXT NOT NULL, \
                    datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
                    ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_PushNotifyMessageRecord_table()
{
    if (tableExists("PushNotifyMessageRecord"))
    {
        createNonExistColumn("PushNotifyMessageRecord", "userid", "INT(4) default 0");
        createNonExistColumn("PushNotifyMessageRecord", "maingroups", "TEXT NOT NULL");
        createNonExistColumn("PushNotifyMessageRecord", "subgroupids", "TEXT");
        //createNonExistColumn("PushNotifyMessageRecord", "imageuid", "TEXT");
    }
    else
    {
        vilslog::printf("check_PushNotifyMessageRecord_table() PushNotifyMessageRecord table NOT existed!\n");
        create_PushNotifyMessageRecord_table();
    }

    return true;
}

bool MySQLDBAccess::create_PushNotifyMessageRecord_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS PushNotifyMessageRecord"); //drop if  table exists
        stmt->execute("CREATE TABLE PushNotifyMessageRecord( \
                    dbId INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
                    type INT(3), \
                    nodeid CHAR(30), \
                    title TEXT, \
                    message TEXT, \
                    messagehashkey TEXT, \
                    targetids TEXT, \
                    subgroupids TEXT, \
                    surveyids TEXT, \
                    hashkeys TEXT, \
                    deadlines TEXT, \
                    userid INT(4) default 0, \
                    imageuid TEXT,\
                    maingroups TEXT NOT NULL,\
                    datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
                    ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_CannedMessage_table()
{
    if (tableExists("CannedMessage"))
    {
        // createNonExistColumn("CannedMessage", "type", "INT(3) default 204");
    }
    else
    {
        vilslog::printf("check_CannedMessage_table() CannedMessage table NOT existed!\n");
        create_CannedMessage_table();
    }

    return true;
}

bool MySQLDBAccess::create_CannedMessage_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS CannedMessage"); //drop if  table exists
        stmt->execute("CREATE TABLE CannedMessage( \
                    dbId INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
                    menutitle TEXT, \
                    title TEXT, \
                    message TEXT, \
                    type INT(3) default 204, \
                    datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
                    ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_PushNotifySetting_table()
{
    if (tableExists("PushNotifySetting"))
    {
    }
    else
    {
        vilslog::printf("check_PushNotifySetting_table() PushNotifySetting table NOT existed!\n");
        create_PushNotifySetting_table();
    }

    return true;
}

bool MySQLDBAccess::create_PushNotifySetting_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS PushNotifySetting"); //drop if  table exists
        stmt->execute("CREATE TABLE PushNotifySetting( \
                    nodeid CHAR(30), \
                    targetid CHAR(30), \
                    functions TEXT, \
                    datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
                    PRIMARY KEY (nodeid, targetid) ) \
                    ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_TriggerPushMessage_table()
{
    if (tableExists("TriggerPushMessage"))
    {
    }
    else
    {
        vilslog::printf("check_TriggerPushMessage_table() TriggerPushMessage table NOT existed!\n");
        create_TriggerPushMessage_table();
    }

    return true;
}

bool MySQLDBAccess::create_TriggerPushMessage_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS TriggerPushMessage"); //drop if  table exists
        stmt->execute("CREATE TABLE TriggerPushMessage( \
                    enable INT(3) , \
                    nodeid CHAR(30) , \
                    targettype INT(3) , \
                    distance INT(3) , \
                    title TEXT NOT NULL, \
                    message TEXT NOT NULL, \
                    datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,  \
                    PRIMARY KEY (nodeid, targettype) ) \
                    ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_AoANodeInfo_table()
{
    if (tableExists("AoANodeInfo"))
    {
        createNonExistColumn("AoANodeInfo", "verifykey", "CHAR(8)");
        createNonExistColumn("AoANodeInfo", "maingroupid", "INT(3) default 1");
        createNonExistColumn("AoANodeInfo", "gridsize", "INT(3) default 50");
    }
    else
    {
        vilslog::printf("check_AoANodeInfo_table() AoANodeInfo table NOT existed!\n");
        create_AoANodeInfo_table();
    }

    return true;
}

bool MySQLDBAccess::create_AoANodeInfo_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS AoANodeInfo"); //drop if  table exists
        stmt->execute("CREATE TABLE AoANodeInfo( \
                    nodeid CHAR(30) PRIMARY KEY, \
                    nodename CHAR(30) , \
                    panid CHAR(30) , \
                    macaddress CHAR(30) , \
                    maplayer INT(3) , \
                    posX INT(3) , \
                    posY INT(3) , \
                    posZ INT(3) , \
                    pitch INT(3) , \
                    roll INT(3) , \
                    yaw INT(3), \
                    gridsize INT(3) default 50, \
                    maingroupid INT(3) default 1, \
                    verifykey CHAR(8), \
                    datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
                    ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_BodyInfo_table()
{
    if (tableExists("BodyInfo"))
    {
    }
    else
    {
        vilslog::printf("check_BodyInfo_table() BodyInfo table NOT existed!\n");
        create_BodyInfo_table();
    }

    return true;
}

bool MySQLDBAccess::create_BodyInfo_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS BodyInfo"); //drop if  table exists
        stmt->execute("CREATE TABLE BodyInfo( \
                    dbId INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
                    nodeid CHAR(30) NOT NULL, \
                    accountid CHAR(30), \
                    recorderid CHAR(30), \
                    weight CHAR(30) , \
                    heartbeat CHAR(30) , \
                    bloodsugar CHAR(30) , \
                    systolicbloodpressure CHAR(30) , \
                    diastolicbloodpressure CHAR(30) , \
                    bodytemperature CHAR(30) , \
                    bodyfat CHAR(30) , \
                    bloodoxygen CHAR(30) , \
                    datadatetime CHAR(30) , \
                    datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
                    ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_InspectionGroup_table()
{
    if (tableExists("InspectionGroup"))
    {

    }
    else
    {
        vilslog::printf("check_InspectionGroup_table() InspectionGroup table NOT existed!\n");
        create_InspectionGroup_table();
    }

    return true;
}

bool MySQLDBAccess::create_InspectionGroup_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS InspectionGroup"); //drop if  table exists
        stmt->execute("CREATE TABLE InspectionGroup( \
                    GroupID INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
                    GroupName CHAR(30) NOT NULL, \
                    StartTime CHAR(30) NOT NULL, \
                    EndTime CHAR(30) NOT NULL, \
                    Inspectors TEXT, \
                    AlertTime INT(3) default 0, \
                    AutoFeed INT(3) default 0, \
                    issuetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
                    ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_InspectionPoint_table()
{
    if (tableExists("InspectionPoint"))
    {
    }
    else
    {
        vilslog::printf("check_InspectionPoint_table() InspectionPoint table NOT existed!\n");
        create_InspectionPoint_table();
    }

    return true;
}

bool MySQLDBAccess::create_InspectionPoint_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        // stmt->execute("DROP TABLE IF EXISTS InspectionPoint"); //drop if  table exists
        stmt->execute("CREATE TABLE InspectionPoint( \
                    PointID INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
                    GroupID INT(6) UNSIGNED NOT NULL, \
                    PointIndex INT(6) default 0, \
                    AnchorNodeId TEXT NOT NULL, \
                    Distance INT(6) default 0, \
                    MinTime INT(6) default 0, \
                    MaxTime INT(6) default 0, \
                    Message TEXT, \
                    TravelTime INT(6) default 0, \
                    issuetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
                    ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_InspectionGroupDailyRecord_table()
{
    if (tableExists("InspectionGroupDailyRecord"))
    {
    }
    else
    {
        vilslog::printf("check_InspectionGroupDailyRecord_table() InspectionGroupDailyRecord table NOT existed!\n");
        create_InspectionGroupDailyRecord_table();
    }

    return true;
}

bool MySQLDBAccess::create_InspectionGroupDailyRecord_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        // stmt->execute("DROP TABLE IF EXISTS InspectionGroupDailyRecord"); //drop if  table exists
        stmt->execute("CREATE TABLE InspectionGroupDailyRecord( \
             DailyRecordID INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
             GroupID INT(3) NOT NULL, \
             Score INT(3) default 0, \
             Finished INT(3) default 0, \
             Exceed INT(3) default 0, \
             Checked INT(3) default 0, \
             DateTime CHAR(30), \
             issuetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_InspectionPointDailyRecord_table()
{
    if (tableExists("InspectionPointDailyRecord"))
    {
    }
    else
    {
        vilslog::printf("check_InspectionPointDailyRecord_table() InspectionPointDailyRecord table NOT existed!\n");
        create_InspectionPointDailyRecord_table();
    }

    return true;
}

bool MySQLDBAccess::create_InspectionPointDailyRecord_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS InspectionPointDailyRecord"); //drop if  table exists
        stmt->execute("CREATE TABLE InspectionPointDailyRecord( \
             DailyRecordID INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
             GroupID INT(3) NOT NULL, \
             PointID INT(3) NOT NULL, \
             Score INT(3) default 0, \
             Checked INT(3) default 0, \
             Finished INT(3) default 0, \
             Exceed INT(3) default 0, \
             Inspector CHAR(30) , \
             RecordNodeId CHAR(30) , \
             EnterTime CHAR(30) , \
             LeaveTime CHAR(30) , \
             Status INT(3) default 0, \
             DateTime CHAR(30) , \
             issuetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_InspectorBehaviorData_table()
{
    if (tableExists("InspectorBehaviorData"))
    {

    }
    else
    {
        vilslog::printf("check_InspectorBehaviorData_table() InspectorBehaviorData table NOT existed!\n");
        create_InspectorBehaviorData_table();
    }

    return true;
}

bool MySQLDBAccess::create_InspectorBehaviorData_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        stmt->execute("CREATE TABLE InspectorBehaviorData( \
             BehaviorRecordID INT(6) UNSIGNED AUTO_INCREMENT, \
             GroupID INT(3) NOT NULL, \
             PointID INT(3) NOT NULL, \
             PointIndex INT(3) NOT NULL, \
             Distance INT(3) NOT NULL, \
             AnchorNodeId CHAR(30) NOT NULL, \
             Inspector CHAR(30) NOT NULL, \
             DateTime CHAR(30) NOT NULL, \
             issuetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
             PRIMARY KEY (BehaviorRecordID, issuetime), \
             INDEX (GroupID, PointID, PointIndex, AnchorNodeId, DateTime) ) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_FenceAlert_table()
{
    if (tableExists("FenceAlert"))
    {

    }
    else
    {
        vilslog::printf("check_FenceAlert_table() FenceAlert table NOT existed!\n");
        create_FenceAlert_table();
    }

    return true;
}

bool MySQLDBAccess::create_FenceAlert_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        stmt->execute("CREATE TABLE FenceAlert( \
             GroupID INT(3) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
             nodeid CHAR(30) NOT NULL, \
             distance INT(3) default 0, \
             DateTime TIMESTAMP DEFAULT CURRENT_TIMESTAMP) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_DeviceMaintenanceRecord_table()
{
    if (tableExists("DeviceMaintenanceRecord"))
    {

    }
    else
    {
        vilslog::printf("check_DeviceMaintenanceRecord_table() DeviceMaintenanceRecord table NOT existed!\n");
        create_DeviceMaintenanceRecord_table();
    }

    return true;
}

bool MySQLDBAccess::create_DeviceMaintenanceRecord_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        stmt->execute("CREATE TABLE DeviceMaintenanceRecord( \
             dbid INT(3) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
             nodetype INT(3) default 0, \
             nodeid CHAR(30) NOT NULL, \
             name CHAR(30) NOT NULL, \
             type CHAR(30) , \
             unit CHAR(30) , \
             manager CHAR(30) , \
             assetlife CHAR(30) , \
             maintenancedate CHAR(30) , \
             status CHAR(30) ,\
             DateTime TIMESTAMP DEFAULT CURRENT_TIMESTAMP) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_Survey_table()
{
    if (tableExists("Survey"))
    {
        createNonExistColumn("Survey", "userid", "INT(4) default 0");
        createNonExistColumn("Survey", "maingroups", "TEXT NOT NULL");

        // createNonExistColumn("Survey", "subgroups", "TEXT NOT NULL");
    }
    else
    {
        vilslog::printf("check_Survey_table() Survey table NOT existed!\n");
        create_Survey_table();
    }

    return true;
}

bool MySQLDBAccess::create_Survey_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        stmt->execute("CREATE TABLE Survey( \
             surveyid INT(3) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
             title CHAR(30) NOT NULL, \
             description CHAR(30) NOT NULL, \
             type INT(3) default 0, \
             userid INT(4) default 0, \
             maingroups TEXT NOT NULL, \
             subgroups TEXT NOT NULL, \
             DateTime TIMESTAMP DEFAULT CURRENT_TIMESTAMP) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_SurveyItem_table()
{
    if (tableExists("SurveyItem"))
    {

    }
    else
    {
        vilslog::printf("check_SurveyItem_table() SurveyItem table NOT existed!\n");
        create_SurveyItem_table();
    }

    return true;
}

bool MySQLDBAccess::create_SurveyItem_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        stmt->execute("CREATE TABLE SurveyItem( \
             surveyitemid INT(3) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
             surveyid INT(3) NOT NULL, \
             description CHAR(30) NOT NULL, \
             DateTime TIMESTAMP DEFAULT CURRENT_TIMESTAMP) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_SurveyEventRecord_table()
{
    if (tableExists("SurveyEventRecord"))
    {
        createNonExistColumn("SurveyEventRecord", "userid", "INT(4) default 0");
        createNonExistColumn("SurveyEventRecord", "maingroups", "TEXT NOT NULL");
    }
    else
    {
        vilslog::printf("check_SurveyEventRecord_table() SurveyEventRecord table NOT existed!\n");
        create_SurveyEventRecord_table();
    }

    return true;
}

bool MySQLDBAccess::create_SurveyEventRecord_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        stmt->execute("CREATE TABLE SurveyEventRecord( \
             eventid INT(6) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
             PushNotifyMessageRecorddbId INT(6) NOT NULL, \
             surveyid INT(3) NOT NULL, \
             hashkey CHAR(32), \
             deadline CHAR(32), \
             formid CHAR(32), \
             userid INT(4) default 0, \
             maingroups TEXT NOT NULL, \
             DateTime TIMESTAMP DEFAULT CURRENT_TIMESTAMP) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");



        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

// bool MySQLDBAccess::check_SurveyRecord_table()
// {
//     if (tableExists("SurveyRecord"))
//     {
//     }
//     else
//     {
//         vilslog::printf("check_SurveyRecord_table() SurveyRecord table NOT existed!\n");
//         create_SurveyRecord_table();
//     }
//
//     return true;
// }

// bool MySQLDBAccess::create_SurveyRecord_table()
// {
//     try {
//         std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
//
//         std::unique_ptr<sql::Statement> stmt(con->createStatement());
//         con->setAutoCommit(true);
//
//         std::string usedb = "USE ";
//         usedb += mSQLDBname;
//         stmt->execute(usedb.c_str());
//
//         // stmt->execute("DROP TABLE IF EXISTS SurveyRecord"); //drop if  table exists
//         stmt->execute("CREATE TABLE SurveyRecord( \
//              recordid INT(3) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
//              eventid INT(6) NOT NULL, \
//              userid INT(3) NOT NULL, \
//              value TEXT NOT NULL, \
//              comment TEXT, \
//              DateTime TIMESTAMP DEFAULT CURRENT_TIMESTAMP) \
//              ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");
//
//         stmt->close();
//     } catch (sql::SQLException &e) {
//         printSQLException(e);
//     }
//
//     return true;
// }

bool MySQLDBAccess::check_SurveyResult_table()
{
    if (tableExists("SurveyResult"))
    {
        // createNonExistColumn("SurveyResult", "token", "TEXT NOT NULL ");
        // sql_execute("UPDATE SurveyResult SET token='' WHERE token IS NULL ");
        // sql_execute("ALTER TABLE SurveyResult DROP PRIMARY KEY, ADD PRIMARY KEY(hashkey(256), token(256), topic(64), messagehashkey(64)) ");
    }
    else
    {
        vilslog::printf("check_SurveyResult_table() SurveyResult table NOT existed!\n");
        create_SurveyResult_table();
    }

    return true;
}

bool MySQLDBAccess::create_SurveyResult_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS SurveyResult"); //drop if  table exists
        stmt->execute("CREATE TABLE SurveyResult( \
             hashkey TEXT NOT NULL, \
             token TEXT NOT NULL, \
             topic TEXT NOT NULL, \
             messagehashkey TEXT NOT NULL, \
             comment TEXT, \
             results TEXT, \
             datetime CHAR(30), \
             updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
             PRIMARY KEY (hashkey(256), token(256), topic(64), messagehashkey(64)) ) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_PushNotifyMessageStatus_table()
{
    if (tableExists("PushNotifyMessageStatus"))
    {
    }
    else
    {
        vilslog::printf("check_PushNotifyMessageStatus_table() PushNotifyMessageStatus table NOT existed!\n");
        create_PushNotifyMessageStatus_table();
    }

    return true;
}

bool MySQLDBAccess::create_PushNotifyMessageStatus_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS PushNotifyMessageStatus"); //drop if  table exists
        stmt->execute("CREATE TABLE PushNotifyMessageStatus( \
             token TEXT NOT NULL, \
             messageid TEXT NOT NULL, \
             topic TEXT NOT NULL, \
             messagehashkey TEXT NOT NULL, \
             readed INT(6) NOT NULL, \
             finished INT(6) NOT NULL, \
             datetime CHAR(30), \
             updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
             PRIMARY KEY (token(256), topic(64), messageid(64)) ) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_TargetTopic_table()
{
    if (tableExists("TargetTopic"))
    {
        createNonExistColumn("TargetTopic", "userid", "INT(4) default 0");
        createNonExistColumn("TargetTopic", "maingroupid", "INT(3) default 1");
    }
    else
    {
        vilslog::printf("check_TargetTopic_table() TargetTopic table NOT existed!\n");
        create_TargetTopic_table();
    }

    return true;
}

bool MySQLDBAccess::create_TargetTopic_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS TargetTopic"); //drop if  table exists
        stmt->execute("CREATE TABLE TargetTopic( \
             target TEXT NOT NULL, \
             topic TEXT NOT NULL, \
             name TEXT NOT NULL, \
             accountid TEXT NOT NULL, \
             maingroupid INT(3) default 1, \
             userid INT(4) default 0, \
             updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
             PRIMARY KEY (target(128)) ) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_RFIDDevice_table()
{
    if (tableExists("RFIDDevice"))
    {
        createNonExistColumn("RFIDDevice","verifykey","CHAR(8)");
        createNonExistColumn("RFIDDevice","maingroupid","INT(3) default 1");
    }
    else
    {
        vilslog::printf("check_RFIDDevice_table() RFIDDevice table NOT existed!\n");
        create_RFIDDevice_table();
    }

    return true;
}

bool MySQLDBAccess::create_RFIDDevice_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS RFIDDevice"); //drop if  table exists
        stmt->execute("CREATE TABLE RFIDDevice( \
             deviceid INT(3) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
             macaddress TEXT NOT NULL, \
             name TEXT NOT NULL, \
             type TEXT NOT NULL, \
             ip TEXT NOT NULL, \
             gwid CHAR(30), \
             LoRAMacaddress CHAR(30), \
             alive INT(3), \
             maingroupid INT(3) default 1, \
             verifykey CHAR(8), \
             datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
             updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_RFIDCard_table()
{
    if (tableExists("RFIDCard"))
    {
        createNonExistColumn("RFIDCard","maingroupid","INT(3) default 1");
    }
    else
    {
        vilslog::printf("check_RFIDCard_table() RFIDCard table NOT existed!\n");
        create_RFIDCard_table();
    }

    return true;
}

bool MySQLDBAccess::create_RFIDCard_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS RFIDCard"); //drop if  table exists
        stmt->execute("CREATE TABLE RFIDCard( \
             cardid TEXT NOT NULL, \
             name TEXT NOT NULL, \
             type TEXT NOT NULL, \
             maplayer INT(3) DEFAULT 0, \
             userid INT(3) DEFAULT 0, \
             unknown INT(3) DEFAULT 0, \
             deviceid INT(3) DEFAULT 0, \
             alive INT(3) DEFAULT 0, \
             datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
             updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
             maingroupid INT(3) default 1, \
             PRIMARY KEY (cardid(32)) ) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_CARDRollCall_table()
{
    if (tableExists("CARDRollCall"))
    {
        //
        // type 0:normal 1:curriculum
        //
        createNonExistColumn("CARDRollCall", "type", "INT(3) default 0");
        createNonExistColumn("CARDRollCall", "curriculumrecordid", "INT(3) default 0");
        createNonExistColumn("CARDRollCall", "StartDate", "CHAR(30) NOT NULL DEFAULT ''");
        createNonExistColumn("CARDRollCall", "EndDate", "CHAR(30) NOT NULL DEFAULT ''");

        // createNonExistColumn("CARDRollCall", "maingroups", "TEXT NOT NULL");
        // createNonExistColumn("CARDRollCall", "resend", "INT(1) DEFAULT 0");
    }
    else
    {
        vilslog::printf("check_CARDRollCall_table() CARDRollCall table NOT existed!\n");
        create_CARDRollCall_table();
    }

    return true;
}

bool MySQLDBAccess::create_CARDRollCall_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS CARDRollCall"); //drop if  table exists
        stmt->execute("CREATE TABLE CARDRollCall( \
             rollcallid INT(3) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
             rollcallname CHAR(30) NOT NULL, \
             StartTime CHAR(30) NOT NULL, \
             EndTime CHAR(30) NOT NULL, \
             StartDate CHAR(30) NOT NULL DEFAULT '', \
             EndDate CHAR(30) NOT NULL DEFAULT '', \
             rfiddeviceids TEXT NOT NULL, \
             nfclocators TEXT NOT NULL, \
             subgroups TEXT NOT NULL, \
             weekday TEXT, \
             validdate TEXT, \
             invaliddate TEXT, \
             presentnotify INT(1) DEFAULT 0, \
             presentcomment CHAR(30) NOT NULL DEFAULT '點名已到', \
             absentnotify INT(1) DEFAULT 0, \
             absentcomment CHAR(30) NOT NULL DEFAULT '點名未到', \
             resend INT(1) DEFAULT 0, \
             maingroups TEXT NOT NULL, \
             type INT(3) default 0, \
             curriculumrecordid INT(3) default 0, \
             datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
             updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_CARDRollCallEvent_table()
{
    if (tableExists("CARDRollCallEvent"))
    {
        createNonExistColumn("CARDRollCallEvent", "maingroups", "TEXT NOT NULL");
        //create_CARDRollCallEvent_table();
    }
    else
    {
        vilslog::printf("check_CARDRollCallEvent_table() CARDRollCallEvent table NOT existed!\n");
        create_CARDRollCallEvent_table();
    }

    return true;
}

bool MySQLDBAccess::create_CARDRollCallEvent_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS CARDRollCallEvent"); //drop if  table exists
        stmt->execute("CREATE TABLE CARDRollCallEvent( \
             eventid INT(3) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
             rollcallid INT(3) NOT NULL, \
             rollcallname CHAR(30) NOT NULL, \
             StartDateTime CHAR(30) NOT NULL, \
             EndDateTime CHAR(30) NOT NULL, \
             totalcount INT(5) DEFAULT 0, \
             presentcount INT(5) DEFAULT 0, \
             maingroups TEXT NOT NULL, \
             datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
             updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_CARDRollCallSubGroupEvent_table()
{
    if (tableExists("CARDRollCallSubGroupEvent"))
    {
        createNonExistColumn("CARDRollCallSubGroupEvent", "maingroups", "TEXT NOT NULL");
    }
    else
    {
        vilslog::printf("check_CARDRollCallSubGroupEvent_table() CARDRollCallSubGroupEvent table NOT existed!\n");
        create_CARDRollCallSubGroupEvent_table();
    }

    return true;
}

bool MySQLDBAccess::create_CARDRollCallSubGroupEvent_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS CARDRollCallSubGroupEvent"); //drop if  table exists
        stmt->execute("CREATE TABLE CARDRollCallSubGroupEvent( \
             eventid INT(3) NOT NULL, \
             rollcallid INT(3) NOT NULL, \
             subgroupid INT(3) NOT NULL, \
             rollcallname CHAR(30) NOT NULL, \
             StartDateTime CHAR(30) NOT NULL, \
             EndDateTime CHAR(30) NOT NULL, \
             totalcount INT(5) DEFAULT 0, \
             presentcount INT(5) DEFAULT 0, \
             maingroups TEXT NOT NULL, \
             datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
             updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
             PRIMARY KEY (eventid, rollcallid, subgroupid) ) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_CARDRollCallRecord_table()
{
    if (tableExists("CARDRollCallRecord"))
    {
        // char cmd[4096];
        // memset(cmd, 0, 4096);
        // sprintf(cmd, "ALTER TABLE CARDRollCallRecord DROP PRIMARY KEY, ADD PRIMARY KEY (rollcallid, rollcalleventid, userid, cardid(32)); ");
        // sql_execute(cmd);

        createNonExistColumn("CARDRollCallRecord", "maingroups", "TEXT NOT NULL");
    }
    else
    {
        vilslog::printf("check_CARDRollCallRecord_table() CARDRollCallRecord table NOT existed!\n");
        create_CARDRollCallRecord_table();
    }

    return true;
}

bool MySQLDBAccess::create_CARDRollCallRecord_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //rfiddeviceid INT(3),
        //nfclocator INT(3),

        //stmt->execute("DROP TABLE IF EXISTS CARDRollCallRecord"); //drop if  table exists
        stmt->execute("CREATE TABLE CARDRollCallRecord( \
             rollcallid INT(3) NOT NULL, \
             rollcalleventid INT(3) NOT NULL, \
             cardid TEXT NOT NULL, \
             userid INT(3) DEFAULT 0, \
             nodeid CHAR(30), \
             rfiddeviceids TEXT NOT NULL, \
             nfclocators TEXT NOT NULL, \
             present INT(1) DEFAULT 0, \
             maingroups TEXT NOT NULL, \
             datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
             updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
             PRIMARY KEY (rollcallid, rollcalleventid, userid, cardid(32)) ) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_CARDScanRecord_table()
{
    if (tableExists("CARDScanRecord"))
    {
        // createNonExistColumn("CARDScanRecord","ssid","CHAR(30)");
        // createNonExistColumn("CARDScanRecord","wifimac","CHAR(30)");
    }
    else
    {
        vilslog::printf("check_CARDScanRecord_table() CARDScanRecord table NOT existed!\n");
        create_CARDScanRecord_table();
    }

    return true;
}

bool MySQLDBAccess::create_CARDScanRecord_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS CARDScanRecord"); //drop if  table exists
        stmt->execute("CREATE TABLE CARDScanRecord( \
             id INT(5) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
             cardid TEXT NOT NULL, \
             cardtype INT(3) DEFAULT 0, \
             LoRAMacaddress CHAR(30) NOT NULL, \
             ssid CHAR(30), \
             wifimac CHAR(30), \
             datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_NFCDevice_table()
{
    if (tableExists("NFCDevice"))
    {
        createNonExistColumn("NFCDevice","verifykey","CHAR(8)");
        createNonExistColumn("NFCDevice","maingroupid","INT(3) default 1");
        // createNonExistColumn("NFCDevice","version","CHAR(30)");
    }
    else
    {
        vilslog::printf("check_NFCDevice_table() NFCDevice table NOT existed!\n");
        create_NFCDevice_table();
    }

    return true;
}

bool MySQLDBAccess::create_NFCDevice_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS NFCDevice"); //drop if  table exists
        stmt->execute("CREATE TABLE NFCDevice( \
             deviceid INT(3) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
             macaddress TEXT NOT NULL, \
             name CHAR(30) NOT NULL, \
             type CHAR(30), \
             gwid CHAR(30), \
             alive INT(3), \
             LoRAMacaddress CHAR(30), \
             ssid CHAR(30), \
             wifimac CHAR(30), \
             version CHAR(30), \
             verifykey CHAR(8), \
             maingroupid INT(3) default 1, \
             datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
             updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_NFCCard_table()
{
    if (tableExists("NFCCard"))
    {
        createNonExistColumn("NFCCard","maingroupid","INT(3) default 1");
    }
    else
    {
        vilslog::printf("check_NFCCard_table() NFCCard table NOT existed!\n");
        create_NFCCard_table();
    }

    return true;
}

bool MySQLDBAccess::create_NFCCard_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS NFCCard"); //drop if  table exists
        stmt->execute("CREATE TABLE NFCCard( \
             cardid TEXT NOT NULL, \
             name TEXT NOT NULL, \
             type TEXT NOT NULL, \
             maplayer INT(3) DEFAULT 0, \
             userid INT(3) DEFAULT 0, \
             unknown INT(3) DEFAULT 0, \
             deviceid INT(3) DEFAULT 0, \
             alive INT(3) DEFAULT 0, \
             datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
             updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
             maingroupid INT(3) default 1, \
             PRIMARY KEY (cardid(32)) ) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_MaingroupNFCCard_table()
{
    if (tableExists("MaingroupNFCCard"))
    {
    }
    else
    {
        vilslog::printf("check_MaingroupNFCCard_table() MaingroupNFCCard table NOT existed!\n");
        create_MaingroupNFCCard_table();
    }

    return true;
}

bool MySQLDBAccess::create_MaingroupNFCCard_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS MaingroupNFCCard"); //drop if  table exists
        stmt->execute("CREATE TABLE MaingroupNFCCard( \
             cardid TEXT NOT NULL, \
             name TEXT NOT NULL, \
             type TEXT NOT NULL, \
             maplayer INT(3) DEFAULT 0, \
             userid INT(3) DEFAULT 0, \
             unknown INT(3) DEFAULT 0, \
             deviceid INT(3) DEFAULT 0, \
             alive INT(3) DEFAULT 0, \
             datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
             updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
             maingroupid INT(3) default 1, \
             PRIMARY KEY (cardid(32), maingroupid) ) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_EDGEDevice_table()
{
    if (tableExists("EDGEDevice"))
    {
        createNonExistColumn("EDGEDevice", "verifykey", "CHAR(8)");
        createNonExistColumn("EDGEDevice", "maingroupid","INT(3) default 1");
    }
    else
    {
        vilslog::printf("check_EDGEDevice_table() EDGEDevice table NOT existed!\n");
        create_EDGEDevice_table();
    }

    return true;
}

bool MySQLDBAccess::create_EDGEDevice_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS EDGEDevice"); //drop if  table exists
        stmt->execute("CREATE TABLE EDGEDevice( \
             deviceid INT(3) UNSIGNED AUTO_INCREMENT PRIMARY KEY, \
             alive INT(3), \
             macaddress TEXT NOT NULL, \
             name CHAR(30) NOT NULL, \
             type CHAR(30), \
             ip CHAR(30), \
             LoRAMacaddress CHAR(30), \
             gwid CHAR(30), \
             version CHAR(30), \
             maingroupid INT(3) default 1, \
             verifykey CHAR(8), \
             datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
             updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_uiblock_table()
{
    if (tableExists("uiblock"))
    {
    }
    else
    {
        vilslog::printf("check_uiblock_table() uiblock table NOT existed!\n");
        create_uiblock_table();
    }

    return true;
}

bool MySQLDBAccess::create_uiblock_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        stmt->execute("CREATE TABLE uiblock( \
             accountid TEXT NOT NULL, \
             jsonstring TEXT NOT NULL, \
             datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
             updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
             PRIMARY KEY (accountid(32)) ) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_subgroupuiblock_table()
{
    if (tableExists("subgroupuiblock"))
    {
    }
    else
    {
        vilslog::printf("check_subgroupuiblock_table() subgroupuiblock table NOT existed!\n");
        create_subgroupuiblock_table();
    }

    return true;
}

bool MySQLDBAccess::create_subgroupuiblock_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        stmt->execute("CREATE TABLE subgroupuiblock( \
             subgroupid INT(3) NOT NULL PRIMARY KEY, \
             jsonstring TEXT NOT NULL, \
             datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
             updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }
    return true;
}

bool MySQLDBAccess::check_weekschedule_table()
{
    if (tableExists("weekschedule"))
    {
    }
    else
    {
        vilslog::printf("check_weekschedule_table() weekschedule table NOT existed!\n");
        create_weekschedule_table();
    }

    return true;
}

bool MySQLDBAccess::create_weekschedule_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        stmt->execute("CREATE TABLE weekschedule( \
             subgroupid INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY, \
             jsonstring TEXT NOT NULL, \
             datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
             updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_StorageFile_table()
{
    if (tableExists("StorageFile"))
    {
        createNonExistColumn("StorageFile", "userid", "INT(4) NOT NULL DEFAULT 0");
        // createNonExistColumn("StorageFile", "uploadtype", "INT(3) default 0");
        // createNonExistColumn("StorageFile", "messagehashkey", "TEXT");
        // createNonExistColumn("StorageFile", "topic", "TEXT");
    }
    else
    {
        vilslog::printf("check_StorageFile_table() StorageFile table NOT existed!\n");
        create_StorageFile_table();
    }

    return true;
}

bool MySQLDBAccess::create_StorageFile_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        stmt->execute("CREATE TABLE StorageFile( \
             id INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY, \
             name TEXT NOT NULL, \
             description TEXT NOT NULL, \
             filename TEXT NOT NULL, \
             fileuid TEXT NOT NULL, \
             filetype TEXT NOT NULL, \
             filesize INT(3) default 0, \
             filepath TEXT NOT NULL, \
             accountid CHAR(30) NOT NULL, \
             userid INT(4) NOT NULL DEFAULT 0, \
             subgroups TEXT NOT NULL, \
             uploadtype INT(3) DEFAULT 0, \
             messagehashkey TEXT, \
             topic TEXT, \
             datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
             updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_StorageImage_table()
{
    if (tableExists("StorageImage"))
    {
        createNonExistColumn("StorageImage", "userid", "INT(4) NOT NULL DEFAULT 0");
        // createNonExistColumn("StorageImage", "uploadtype", "INT(3) default 0");
        // createNonExistColumn("StorageImage", "messagehashkey", "TEXT");
        // createNonExistColumn("StorageImage", "topic", "TEXT");
    }
    else
    {
        vilslog::printf("check_StorageImage_table() StorageImage table NOT existed!\n");
        create_StorageImage_table();
    }

    return true;
}

bool MySQLDBAccess::create_StorageImage_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        stmt->execute("CREATE TABLE StorageImage( \
             id INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY, \
             name TEXT NOT NULL, \
             description TEXT NOT NULL, \
             imagename TEXT NOT NULL, \
             imageuid TEXT NOT NULL, \
             imagetype TEXT NOT NULL, \
             imagewidth INT(3) default 0, \
             imageheight INT(3) default 0, \
             imagesize INT(3) default 0, \
             imagepath TEXT NOT NULL, \
             thumbpath TEXT NOT NULL, \
             accountid CHAR(30) NOT NULL, \
             userid INT(4) NOT NULL DEFAULT 0, \
             subgroups TEXT NOT NULL, \
             albumid INT(3) default 0, \
             uploadtype INT(3) default 0, \
             messagehashkey TEXT, \
             topic TEXT, \
             datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
             updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_applicationformleave_table()
{
    if (tableExists("applicationformleave"))
    {
        createNonExistColumn("applicationformleave", "userid", "INT(4) default 0");
        createNonExistColumn("applicationformleave", "signoffuserid", "INT(4) default 0");
        createNonExistColumn("applicationformleave", "maingroups", "TEXT NOT NULL");
    }
    else
    {
        vilslog::printf("check_applicationformleave_table() applicationformleave table NOT existed!\n");
        create_applicationformleave_table();
    }

    return true;
}

bool MySQLDBAccess::create_applicationformleave_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        stmt->execute("CREATE TABLE applicationformleave( \
             formid CHAR(30) NOT NULL PRIMARY KEY, \
             applytime TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
             userid INT(4) default 0, \
             subgroups TEXT NOT NULL, \
             type TEXT NOT NULL, \
             reason TEXT NOT NULL, \
             startdate TEXT NOT NULL, \
             starttime TEXT NOT NULL, \
             enddate TEXT NOT NULL, \
             endtime TEXT NOT NULL, \
             reissue INT(1) default 0, \
             applyusername TEXT NOT NULL, \
             applyuserrelation TEXT NOT NULL, \
             imageids TEXT, \
             fileids TEXT, \
             signoffstatus INT(1) default 0, \
             signoffuserid INT(4) default 0, \
             maingroups TEXT NOT NULL, \
             datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
             updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_logintoken_table()
{
    if (tableExists("logintoken"))
    {
        createNonExistColumn("logintoken", "userid", "INT(4) default 0");
    }
    else
    {
        vilslog::printf("check_logintoken_table() logintoken table NOT existed!\n");
        create_logintoken_table();
    }

    return true;
}

bool MySQLDBAccess::create_logintoken_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        stmt->execute("CREATE TABLE logintoken( \
             id INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY, \
             accountid CHAR(30) NOT NULL, \
             token TEXT NOT NULL, \
             loginkey TEXT NOT NULL, \
             userid INT(4) default 0, \
             datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
             updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_assist_table()
{
    if (tableExists("assist"))
    {
        createNonExistColumn("assist", "userid", "INT(4) default 0");
        createNonExistColumn("assist", "assignusers", "TEXT NOT NULL");
        createNonExistColumn("assist", "maingroups", "TEXT NOT NULL");
    }
    else
    {
        vilslog::printf("check_assist_table() assist table NOT existed!\n");
        create_assist_table();
    }

    return true;
}

bool MySQLDBAccess::create_assist_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        //stmt->execute("DROP TABLE IF EXISTS assist"); //drop if  table exists
        stmt->execute("CREATE TABLE assist( \
             id INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY, \
             userid INT(4) default 0, \
             title CHAR(30) NOT NULL, \
             description CHAR(30) NOT NULL, \
             subgroups TEXT NOT NULL, \
             assignusers TEXT NOT NULL, \
             maingroups TEXT NOT NULL, \
             datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_assistEvent_table()
{
    if (tableExists("assistevent"))
    {
        createNonExistColumn("assistevent", "userid", "INT(4) default 0");
        createNonExistColumn("assistevent", "signoffuserid", "INT(4) default 0");
        createNonExistColumn("assistevent", "maingroups", "TEXT NOT NULL");
    }
    else
    {
        vilslog::printf("check_assistEvent_table() assistevent table NOT existed!\n");
        create_assistEvent_table();
    }

    return true;
}

bool MySQLDBAccess::create_assistEvent_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        // stmt->execute("DROP TABLE IF EXISTS assistevent"); //drop if  table exists
        stmt->execute("CREATE TABLE assistevent( \
             id INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY, \
             assistid INT(3) NOT NULL, \
             userid INT(4) default 0, \
             description CHAR(128) NOT NULL, \
             eventtime TEXT NOT NULL, \
             weekday TEXT, \
             validdate TEXT, \
             invaliddate TEXT, \
             deadline TEXT, \
             imageids TEXT, \
             fileids TEXT, \
             applyusername TEXT NOT NULL, \
             applyuserrelation TEXT NOT NULL, \
             signoffstatus INT(1) default 0, \
             signoffuserid INT(4) default 0, \
             maingroups TEXT NOT NULL, \
             datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP) \
             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_assistEventRecord_table()
{
    if (tableExists("assisteventrecord"))
    {
        createNonExistColumn("assisteventrecord", "userid", "INT(4) default 0");
        createNonExistColumn("assisteventrecord", "maingroups", "TEXT NOT NULL");
        //createNonExistColumn("assisteventrecord", "accountid", "CHAR(30)");
    }
    else
    {
        vilslog::printf("check_assistEventRecord_table() assisteventrecord table NOT existed!\n");
        create_assistEventRecord_table();
    }

    return true;
}

bool MySQLDBAccess::create_assistEventRecord_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        // stmt->execute("DROP TABLE IF EXISTS assisteventrecord"); //drop if  table exists
        stmt->execute("CREATE TABLE assisteventrecord( \
            id INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY, \
            assistid INT(3) NOT NULL, \
            assisteventid INT(6) NOT NULL, \
            PushNotifyMessageRecorddbId INT(6) NOT NULL, \
            userid INT(4) default 0, \
            accountid CHAR(30), \
            messagehashkey TEXT NOT NULL, \
            results TEXT NOT NULL, \
            eventday TEXT NOT NULL, \
            eventtime TEXT NOT NULL, \
            maingroups TEXT NOT NULL, \
            datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
            updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
            ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_billboardRecord_table()
{
    if (tableExists("billboardrecord"))
    {
        createNonExistColumn("billboardrecord", "userid", "INT(4) default 0");
        createNonExistColumn("billboardrecord", "maingroups", "TEXT NOT NULL");
    }
    else
    {
        vilslog::printf("check_billboardRecord_table() billboardrecord table NOT existed!\n");
        create_billboardRecord_table();
    }

    return true;
}

bool MySQLDBAccess::create_billboardRecord_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        // stmt->execute("DROP TABLE IF EXISTS billboardrecord"); //drop if  table exists
        stmt->execute("CREATE TABLE billboardrecord( \
            id INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY, \
            userid INT(4) default 0, \
            sendpushnotify INT(6) NOT NULL, \
            PushNotifyMessageRecorddbId INT(6) NOT NULL, \
            subgroupids TEXT NOT NULL, \
            title TEXT NOT NULL, \
            content TEXT NOT NULL, \
            imageids TEXT NOT NULL, \
            fileids TEXT NOT NULL, \
            maingroups TEXT NOT NULL, \
            datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP) \
            ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_register_table()
{
    if (tableExists("register"))
    {
    }
    else
    {
        vilslog::printf("check_register_table() register table NOT existed!\n");
        create_register_table();
    }

    return true;
}

bool MySQLDBAccess::create_register_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        // stmt->execute("DROP TABLE IF EXISTS register"); //drop if  table exists
        stmt->execute("CREATE TABLE register( \
            email CHAR(64) PRIMARY KEY, \
            pwd CHAR(128) NOT NULL, \
            token TEXT NOT NULL, \
            subgroupname TEXT NOT NULL, \
            send INT(3) DEFAULT 0, \
            valid INT(3) DEFAULT 0, \
            updatetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
            datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP) \
            ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_Curriculum_table()
{
    if (tableExists("Curriculum"))
    {
        createNonExistColumn("Curriculum", "recordid", "INT(3) NOT NULL DEFAULT 0");
        createNonExistColumn("Curriculum", "shared", "INT(3) NOT NULL DEFAULT 0");
        createNonExistColumn("Curriculum", "sharedmaingroups", "TEXT NOT NULL");
    }
    else
    {
        vilslog::printf("check_Curriculum_table() Curriculum table NOT existed!\n");
        create_Curriculum_table();
    }

    return true;
}

bool MySQLDBAccess::create_Curriculum_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        // stmt->execute("DROP TABLE IF EXISTS Curriculum"); //drop if  table exists
        stmt->execute("CREATE TABLE Curriculum( \
            id INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY, \
            recordid INT(3) NOT NULL DEFAULT 0, \
            userid INT(4) NOT NULL DEFAULT 0, \
            typeid INT(3) NOT NULL DEFAULT 0, \
            subgroups TEXT NOT NULL, \
            maingroups TEXT NOT NULL, \
            number TEXT NOT NULL, \
            shared INT(3) NOT NULL DEFAULT 0, \
            sharedmaingroups TEXT NOT NULL, \
            datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP) \
            ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

// bool MySQLDBAccess::check_CurriculumShared_table()
// {
//     if (tableExists("CurriculumShared"))
//     {
//     }
//     else
//     {
//         vilslog::printf("check_CurriculumShared_table() CurriculumShared table NOT existed!\n");
//         create_CurriculumShared_table();
//     }
//
//     return true;
// }

// bool MySQLDBAccess::create_CurriculumShared_table()
// {
//     try {
//         std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));
//
//         std::unique_ptr<sql::Statement> stmt(con->createStatement());
//         con->setAutoCommit(true);
//
//         std::string usedb = "USE ";
//         usedb += mSQLDBname;
//         stmt->execute(usedb.c_str());
//
//         // stmt->execute("DROP TABLE IF EXISTS CurriculumShared"); //drop if  table exists
//         stmt->execute("CREATE TABLE CurriculumShared( \
//             id INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY, \
//             curriculumid INT(4) DEFAULT 0, \
//             userid INT(4) DEFAULT 0, \
//             typeid INT(3) NOT NULL DEFAULT 0, \
//             subgroups TEXT NOT NULL, \
//             maingroups TEXT NOT NULL, \
//             number TEXT NOT NULL, \
//             datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP) \
//             ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");
//
//         stmt->close();
//     } catch (sql::SQLException &e) {
//         printSQLException(e);
//     }
//
//     return true;
// }

bool MySQLDBAccess::check_CurriculumRecord_table()
{
    if (tableExists("CurriculumRecord"))
    {
        // char cmd[4096];
        // memset(cmd, 0, 4096);
        // sprintf(cmd, "ALTER TABLE CurriculumRecord MODIFY COLUMN starttime TEXT");
        // sql_execute(cmd);
        //
        // memset(cmd, 0, 4096);
        // sprintf(cmd, "ALTER TABLE CurriculumRecord MODIFY COLUMN endtime TEXT");
        // sql_execute(cmd);
        //
        // memset(cmd, 0, 4096);
        // sprintf(cmd, "ALTER TABLE CurriculumRecord MODIFY COLUMN signinstarttime TEXT");
        // sql_execute(cmd);
        //
        // memset(cmd, 0, 4096);
        // sprintf(cmd, "ALTER TABLE CurriculumRecord MODIFY COLUMN signinendtime TEXT");
        // sql_execute(cmd);

        createNonExistColumn("CurriculumRecord", "name", "TEXT NOT NULL");
        createNonExistColumn("CurriculumRecord", "shared", "INT(3) NOT NULL DEFAULT 0");
        createNonExistColumn("CurriculumRecord", "subgroups", "TEXT NOT NULL");
        // createNonExistColumn("CurriculumRecord", "teacheruserid", "INT(4) default 0");
    }
    else
    {
        vilslog::printf("check_CurriculumRecord_table() CurriculumRecord table NOT existed!\n");
        create_CurriculumRecord_table();
    }

    return true;
}

bool MySQLDBAccess::create_CurriculumRecord_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        // stmt->execute("DROP TABLE IF EXISTS CurriculumRecord"); //drop if  table exists
        stmt->execute("CREATE TABLE CurriculumRecord( \
            id INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY, \
            userid INT(4) default 0, \
            curriculumid INT(3) NOT NULL, \
            shared INT(3) NOT NULL DEFAULT 0, \
            name TEXT NOT NULL, \
            maingroups TEXT NOT NULL, \
            subgroups TEXT NOT NULL, \
            number TEXT NOT NULL, \
            opendate DATE NOT NULL, \
            closedate DATE NOT NULL, \
            startdate DATE NOT NULL, \
            enddate DATE NOT NULL, \
            starttime TEXT NOT NULL, \
            endtime TEXT NOT NULL, \
            signinstarttime TEXT NOT NULL, \
            signinendtime TEXT NOT NULL, \
            weekday TEXT NOT NULL, \
            totalhours TEXT NOT NULL, \
            location TEXT NOT NULL, \
            description TEXT NOT NULL, \
            teacher TEXT NOT NULL, \
            teacheruserid INT(4) default 0, \
            maxnumber INT(4) default 0, \
            autoapproval INT(1) default 0, \
            cost TEXT NOT NULL, \
            note TEXT NOT NULL, \
            files TEXT NOT NULL, \
            status INT(1) default 0, \
            datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP) \
            ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_CurriculumType_table()
{
    if (tableExists("CurriculumType"))
    {
    }
    else
    {
        vilslog::printf("check_CurriculumType_table() CurriculumType table NOT existed!\n");
        create_CurriculumType_table();
    }

    return true;
}

bool MySQLDBAccess::create_CurriculumType_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        // stmt->execute("DROP TABLE IF EXISTS CurriculumType"); //drop if  table exists
        stmt->execute("CREATE TABLE CurriculumType( \
            id INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY, \
            name TEXT NOT NULL, \
            maingroups TEXT NOT NULL, \
            datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP) \
            ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_CurriculumPrefix_table()
{
    if (tableExists("CurriculumPrefix"))
    {
    }
    else
    {
        vilslog::printf("check_CurriculumPrefix_table() CurriculumPrefix table NOT existed!\n");
        create_CurriculumPrefix_table();
    }

    return true;
}

bool MySQLDBAccess::create_CurriculumPrefix_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        // stmt->execute("DROP TABLE IF EXISTS CurriculumPrefix"); //drop if  table exists
        stmt->execute("CREATE TABLE CurriculumPrefix( \
            maingroupid INT(3) unsigned zerofill NOT NULL PRIMARY KEY, \
            prefix TEXT NOT NULL, \
            userid INT(4) default 0, \
            datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
            ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_CurriculumApply_table()
{
    if (tableExists("CurriculumApply"))
    {
    }
    else
    {
        vilslog::printf("check_CurriculumApply_table() CurriculumApply table NOT existed!\n");
        create_CurriculumApply_table();
    }

    return true;
}

bool MySQLDBAccess::create_CurriculumApply_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        // stmt->execute("DROP TABLE IF EXISTS CurriculumApply"); //drop if  table exists
        stmt->execute("CREATE TABLE CurriculumApply( \
            userid INT(4) NOT NULL, \
            curriculumrecordid INT(3) NOT NULL, \
            status INT(1) default 0, \
            datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
            PRIMARY KEY (userid, curriculumrecordid) ) \
            ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_sensorinfo_table()
{
    if (tableExists("sensorinfo"))
    {
        // createNonExistColumn("sensorinfo", "maingroupid", "INT(3) default 1");
    }
    else
    {
        vilslog::printf("check_sensorinfo_table() sensorinfo table NOT existed!\n");
        create_sensorinfo_table();
    }

    return true;
}

bool MySQLDBAccess::create_sensorinfo_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        // stmt->execute("DROP TABLE IF EXISTS sensorinfo"); //drop if  table exists
        stmt->execute("CREATE TABLE sensorinfo( \
            id INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY, \
            name VARCHAR(30) NOT NULL, \
            day date NOT NULL, \
            value TEXT NOT NULL, \
            jsonvalue TEXT, \
            Longitude VARCHAR(30), \
            Latitude VARCHAR(30), \
            maingroupid INT(3) default 1, \
            datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP, \
            INDEX(name(30), day)) \
            ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_sensordevice_table()
{
    if (tableExists("sensordevice"))
    {
        createNonExistColumn("sensordevice", "verifykey", "CHAR(8)");
        createNonExistColumn("sensordevice", "type", "TEXT");
    }
    else
    {
        vilslog::printf("check_sensordevice_table() sensordevice table NOT existed!\n");
        create_sensordevice_table();
    }

    return true;
}

bool MySQLDBAccess::create_sensordevice_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        // stmt->execute("DROP TABLE IF EXISTS sensordevice"); //drop if  table exists
        stmt->execute("CREATE TABLE sensordevice( \
            alive INT(3) default 0, \
            id VARCHAR(30) PRIMARY KEY, \
            name VARCHAR(30) NOT NULL, \
            type TEXT, \
            jsonvalue TEXT, \
            Longitude VARCHAR(30), \
            Latitude VARCHAR(30), \
            Battery VARCHAR(30), \
            Version VARCHAR(30), \
            maingroupid INT(3) default 1, \
            verifykey CHAR(8), \
            datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
            ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_sensorevent_table()
{
    if (tableExists("sensorevent"))
    {
    }
    else
    {
        vilslog::printf("check_sensorevent_table() sensorevent table NOT existed!\n");
        create_sensorevent_table();
    }

    return true;
}

bool MySQLDBAccess::create_sensorevent_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        // stmt->execute("DROP TABLE IF EXISTS sensorevent"); //drop if  table exists
        stmt->execute("CREATE TABLE sensorevent( \
            id INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY, \
            sensorid VARCHAR(30) NOT NULL, \
            type TEXT, \
            jsonvalue TEXT, \
            maingroupid INT(3) default 1, \
            datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
            ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_FlyInfo_table()
{
    if (tableExists("FlyInfo"))
    {
        // char cmd[4096];
        // memset(cmd, 0, 4096);
        // sprintf(cmd, "ALTER TABLE FlyInfo MODIFY COLUMN sensorid varchar(30) DEFAULT ''");
        // sql_execute(cmd);
        //
        // memset(cmd, 0, 4096);
        // sprintf(cmd, "ALTER TABLE FlyInfo MODIFY COLUMN airBoardingGate varchar(30) DEFAULT ''");
        // sql_execute(cmd);

        // createNonExistColumn("FlyInfo", "sensorid", "varchar(30) DEFAULT ''");
        // createNonExistColumn("FlyInfo", "airBoardingGate", "varchar(30) DEFAULT ''");
        // createNonExistColumn("FlyInfo", "expectTime", "VARCHAR(30) NOT NULL");
    }
    else
    {
        vilslog::printf("check_FlyInfo_table() FlyInfo table NOT existed!\n");
        create_FlyInfo_table();
    }

    return true;
}

bool MySQLDBAccess::create_FlyInfo_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        // stmt->execute("DROP TABLE IF EXISTS FlyInfo"); //drop if  table exists
        stmt->execute("CREATE TABLE FlyInfo( \
            FDATE VARCHAR(30) NOT NULL, \
            expectTime VARCHAR(30) NOT NULL, \
            airLineNum VARCHAR(30) NOT NULL, \
            airLineCode VARCHAR(30) NOT NULL, \
            type TEXT, \
            jsonvalue TEXT, \
            triggerTime TEXT, \
            maingroupid INT(3) default 1, \
            sensorid VARCHAR(30) DEFAULT '', \
            airBoardingGate VARCHAR(30) DEFAULT '', \
            datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
            PRIMARY KEY (FDATE, airLineNum, airLineCode)) \
            ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_APIKey_table()
{
    if (tableExists("APIKey"))
    {
    }
    else
    {
        vilslog::printf("check_APIKey_table() APIKey table NOT existed!\n");
        create_APIKey_table();
    }

    return true;
}

bool MySQLDBAccess::create_APIKey_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        // stmt->execute("DROP TABLE IF EXISTS APIKey"); //drop if  table exists
        stmt->execute("CREATE TABLE APIKey( \
            apikey VARCHAR(30) NOT NULL, \
            type TEXT, \
            descrption TEXT, \
            enable INT(3) default 1, \
            maingroupid INT(3) default 1, \
            subgroupid INT(3) default 1, \
            enableTime TEXT, \
            expireTime TEXT, \
            datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
            ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_KIAWCInfo_table()
{
    if (tableExists("KIAWCInfo"))
    {
    }
    else
    {
        vilslog::printf("check_KIAWCInfo_table() KIAWCInfo table NOT existed!\n");
        create_KIAWCInfo_table();
    }

    return true;
}

bool MySQLDBAccess::create_KIAWCInfo_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        // stmt->execute("DROP TABLE IF EXISTS KIAWCInfo"); //drop if  table exists
        stmt->execute("CREATE TABLE KIAWCInfo( \
            GID CHAR(30) PRIMARY KEY, \
            place TEXT, \
            item TEXT, \
            notetime TEXT, \
            sendNotify INT(3) default 0, \
            maingroupid INT(3) default 1, \
            datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
            ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_FireFighting_table()
{
    if (tableExists("FireFighting"))
    {
        // create_FireFighting_table();
        createNonExistColumn("FireFighting", "StandbyDeviceIds", "TEXT");
    }
    else
    {
        vilslog::printf("check_FireFighting_table() FireFighting table NOT existed!\n");
        create_FireFighting_table();
    }

    return true;
}

bool MySQLDBAccess::create_FireFighting_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        // stmt->execute("DROP TABLE IF EXISTS FireFighting"); //drop if  table exists
        stmt->execute("CREATE TABLE FireFighting( \
            id INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY, \
            casenumber TEXT, \
            address TEXT, \
            lat TEXT, \
            lng TEXT, \
            caseclosed INT(3) default 0, \
            maingroupid INT(3) default 1, \
            starttime TEXT, \
            closetime TEXT, \
            NFCDeviceIds TEXT, \
            StandbyDeviceIds TEXT, \
            datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
            ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_FireFightingUser_table()
{
    if (tableExists("FireFightingUser"))
    {
    }
    else
    {
        vilslog::printf("check_FireFightingUser_table() FireFightingUser table NOT existed!\n");
        create_FireFightingUser_table();
    }

    return true;
}

bool MySQLDBAccess::create_FireFightingUser_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        // stmt->execute("DROP TABLE IF EXISTS FireFightingUser"); //drop if  table exists
        stmt->execute("CREATE TABLE FireFightingUser( \
            casenumber VARCHAR(30) NOT NULL, \
            userid INT(3) NOT NULL, \
            maingroupid INT(3) NOT NULL, \
            areaType INT(3) NOT NULL, \
            airremain TEXT, \
            enterTime TEXT, \
            activity TEXT, \
            HR TEXT, \
            SPO2 TEXT, \
            battery TEXT, \
            datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
            PRIMARY KEY (casenumber, userid)) \
            ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_FireFightingEvent_table()
{
    if (tableExists("FireFightingEvent"))
    {
    }
    else
    {
        vilslog::printf("check_FireFightingUser_table() FireFightingEvent table NOT existed!\n");
        create_FireFightingEvent_table();
    }

    return true;
}

bool MySQLDBAccess::create_FireFightingEvent_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        // stmt->execute("DROP TABLE IF EXISTS FireFightingEvent"); //drop if  table exists
        stmt->execute("CREATE TABLE FireFightingEvent( \
            id INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY, \
            casenumber TEXT, \
            eventtype TEXT, \
            jsonstring TEXT, \
            datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
            ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_FireFightingAirUsing_table()
{
    if (tableExists("FireFightingAirUsing"))
    {
    }
    else
    {
        vilslog::printf("check_FireFightingAirUsing_table() FireFightingAirUsing table NOT existed!\n");
        create_FireFightingAirUsing_table();
    }

    return true;
}

bool MySQLDBAccess::create_FireFightingAirUsing_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        // stmt->execute("DROP TABLE IF EXISTS FireFightingAirUsing"); //drop if  table exists
        stmt->execute("CREATE TABLE FireFightingAirUsing( \
            id INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY, \
            casenumber TEXT, \
            userid INT(3) NOT NULL, \
            startairremain TEXT, \
            starttime TEXT, \
            endairremain TEXT, \
            endtime TEXT, \
            airusing TEXT, \
            totaltime TEXT, \
            datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP) \
            ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

bool MySQLDBAccess::check_SoccerRecord_table()
{
    if (tableExists("SoccerRecord"))
    {
        // create_SoccerRecord_table();
    }
    else
    {
        vilslog::printf("check_SoccerRecord_table() SoccerRecord table NOT existed!\n");
        create_SoccerRecord_table();
    }

    return true;
}

bool MySQLDBAccess::create_SoccerRecord_table()
{
    try {
        std::unique_ptr< sql::Connection > con( mMySQLDriver->connect(MySQLDB_server, MySQLDB_username, MySQLDB_password));

        std::unique_ptr<sql::Statement> stmt(con->createStatement());
        con->setAutoCommit(true);

        std::string usedb = "USE ";
        usedb += mSQLDBname;
        stmt->execute(usedb.c_str());

        // stmt->execute("DROP TABLE IF EXISTS SoccerRecord"); //drop if  table exists
        stmt->execute("CREATE TABLE SoccerRecord( \
            recordid INT(3) unsigned zerofill NOT NULL AUTO_INCREMENT PRIMARY KEY, \
            recordnumber VARCHAR(30) NOT NULL, \
            maingroupid INT(3) default 0, \
            starttime CHAR(30) default '0', \
            endtime CHAR(30) default '0', \
            datetime TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP, \
            INDEX (recordnumber)) \
            ENGINE=INNODB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci");

        stmt->close();
    } catch (sql::SQLException &e) {
        printSQLException(e);
    }

    return true;
}

std::list<AoANodeStruct*> * MySQLDBAccess::read_AoA_nodes()
{
    std::list<AoANodeStruct*> * aoaList = new std::list<AoANodeStruct*>();
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM AoANodeInfo");

    sql_executeQuery_new(cmd, get_AoANodeList_callback, aoaList);

    return aoaList;
}

CourseStruct * MySQLDBAccess::read_Course(const char * courseId)
{
    CourseStruct * course = new CourseStruct();
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM Course WHERE courseId='%s'", courseId);

    int retCount = sql_executeQuery_new(cmd, get_Course_callback, course);
    if (retCount == 0 )
    {
        delete course;
        return NULL;
    }
    return course;
}

std::list<CourseStruct*> * MySQLDBAccess::read_allCourses(int projectID)
{
    std::list<CourseStruct*> * courseList = new std::list<CourseStruct*>();
    char cmd[4096];
    memset(cmd, 0, 4096);

    if (projectID >= 0)
        sprintf(cmd, "SELECT * FROM Course WHERE PROJECT_ID=%d", projectID);
    else
        sprintf(cmd, "SELECT * FROM Course");

    sql_executeQuery_new(cmd, get_Courselist_callback, courseList);

    return courseList;
}

void MySQLDBAccess::write_Course(CourseStruct * curCourse)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::string daysOfWeekString = "";
    if ( curCourse->daysOfWeek != NULL )
    {
        daysOfWeekString = convertIntListToString(curCourse->daysOfWeek);
    }

    if (curCourse->dbId < 0)
    {
        sprintf(cmd, "INSERT INTO Course(PROJECT_ID, allDay, resourceId, courseId, title, description, \
                start, end, startTime, endTime, daysOfWeek, startRecur, endRecur, backgroundColor, borderColor, maingroupid) \
                VALUES (%d, %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d)",
                curCourse->projectid, curCourse->allDay, curCourse->resourceId.c_str(), curCourse->courseId.c_str(), curCourse->title.c_str(),
                curCourse->description.c_str(), curCourse->start.c_str(),
                curCourse->end.c_str(), curCourse->startTime.c_str(), curCourse->endTime.c_str(), daysOfWeekString.c_str(),
                curCourse->startRecur.c_str(), curCourse->endRecur.c_str(),
                curCourse->backgroundColor.c_str(), curCourse->borderColor.c_str(), curCourse->maingroupid);
    }
    else
    {
        sprintf(cmd, "UPDATE Course SET PROJECT_ID=%d, allDay=%d, resourceId='%s', courseId='%s', title='%s', description='%s', \
                start='%s', end='%s', startTime='%s', endTime='%s', daysOfWeek='%s', startRecur='%s', endRecur='%s', \
                backgroundColor='%s', borderColor='%s', maingroupid=%d WHERE dbId=%d;",
                curCourse->projectid, curCourse->allDay, curCourse->resourceId.c_str(), curCourse->courseId.c_str(), curCourse->title.c_str(),
                curCourse->description.c_str(), curCourse->start.c_str(),
                curCourse->end.c_str(), curCourse->startTime.c_str(), curCourse->endTime.c_str(), daysOfWeekString.c_str(),
                curCourse->startRecur.c_str(), curCourse->endRecur.c_str(),
                curCourse->backgroundColor.c_str(), curCourse->borderColor.c_str(), curCourse->maingroupid, curCourse->dbId);
    }

    //printf("MySQLDBAccess::write_Course() cmd[%s]\n", cmd);

    sql_execute(cmd);

}

void MySQLDBAccess::delete_Course(CourseStruct * curCourse)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "DELETE from Course WHERE PROJECT_ID=%d AND courseId='%s'", curCourse->projectid, curCourse->courseId.c_str());

    vilslog::printf("MySQLDBAccess::delete_Course() cmd[%s]\n", cmd);

    sql_execute(cmd);
}

CourseRollCallRecordStruct * MySQLDBAccess::read_rollcallRecord(int projectID, std::string infoOfDate, std::string courseId)
{
    CourseRollCallRecordStruct * rollcallRecord = new CourseRollCallRecordStruct();
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM CourseRollCallRecord WHERE infoOfDate='%s' AND courseId='%s' ", infoOfDate.c_str(), courseId.c_str());

    bool bSuccess = sql_executeQuery_new(cmd, get_CourseRollCallRecord_callback, rollcallRecord);
    if (!bSuccess)
    {
        //vilslog::printf("MySQLDBAccess::read_rollcallRecord() failed!!!!   \n");
        delete rollcallRecord;
        return NULL;
    }
    return rollcallRecord;
}

std::list<CourseRollCallRecordStruct *> * MySQLDBAccess::read_rollcallRecordDayRange(const char * FromDatetime, const char * ToDatetime)
{
    std::list<CourseRollCallRecordStruct *> * rollcallRecordList = new std::list<CourseRollCallRecordStruct *>();
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM CourseRollCallRecord WHERE issuetime BETWEEN '%s' AND '%s' ORDER BY issuetime DESC",
                FromDatetime, ToDatetime);

    //vilslog::printf("read_rollcallRecordDayRange() FromDatetime[%s] ToDatetime[%s]\n", FromDatetime, ToDatetime);

    bool bSuccess = sql_executeQuery_new(cmd, get_CourseRollCallRecordList_callback, rollcallRecordList);
    if (!bSuccess)
    {
        //vilslog::printf("MySQLDBAccess::read_rollcallRecordDayRange() failed!!!!\n");
    }
    return rollcallRecordList;
}

void MySQLDBAccess::write_rollcallRecord(CourseRollCallRecordStruct * curRecord)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::string tags = convertListToString(curRecord->tags);

    sprintf(cmd, "INSERT INTO CourseRollCallRecord(PROJECT_ID, courseId, infoOfDate, tags) \
            VALUES (%d, '%s', '%s', '%s')",
            curRecord->projectid, curRecord->courseId.c_str(), curRecord->infoOfDate.c_str(), tags.c_str());

    int ret = sql_execute(cmd);

    //printf("MySQLDBAccess::write_rollcallRecord() 1 cmd[%s] ret[%d]\n", cmd, ret);

    if (ret < 0)
    {

        sprintf(cmd, "UPDATE CourseRollCallRecord SET PROJECT_ID=%d, tags='%s' WHERE courseId='%s' AND infoOfDate='%s';",
                curRecord->projectid, tags.c_str(), curRecord->courseId.c_str(), curRecord->infoOfDate.c_str());

        ret = sql_execute(cmd);

        //printf("MySQLDBAccess::write_rollcallRecord() 2 cmd[%s] ret[%d]\n", cmd, ret);
    }
}

void MySQLDBAccess::write_AreaInOutEvent(AreaInOutEventStruct * inoutEvent)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO AreaInOutRecord(PROJECT_ID, AreaType, action, tafid, MapAreaType, NodeId, NodeName, AreaID, AreaName, DataTime) \
            VALUES (%d, %d, %d, %d, %d, '%s', '%s', '%s', '%s', '%s')",
            inoutEvent->projectid, inoutEvent->AreaType0D, inoutEvent->action, inoutEvent->tafid, inoutEvent->MapAreaType,
            inoutEvent->NodeId.c_str(), inoutEvent->NodeName.c_str(), inoutEvent->AreaID.c_str(), inoutEvent->AreaName.c_str(),
            inoutEvent->DataTime.c_str());

    sql_execute(cmd);

    update_to_AreaInOut_cache(inoutEvent->NodeId.c_str(), 1, inoutEvent->DataTime.c_str(), inoutEvent->DataTime.c_str(), inoutEvent->DataTime.c_str(), inoutEvent->DataTime.c_str(),
        inoutEvent->AreaID.c_str(), inoutEvent->AreaName.c_str());

    // vilslog::printf("MySQLDBAccess::write_AreaInOutEvent() cmd[%s]\n", cmd);
}

void MySQLDBAccess::write_LoRaGatewayInfo(LoRaGatewayInfoStruct * gateway)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::string channel = convertListToString(&gateway->channels);

    sprintf(cmd, "INSERT INTO LoRaGatewayInfo(gwid, PROJECT_ID, sf, gwip, repeater, channel, maplayer, posX, posY, posZ, maingroupid, verifykey) \
            VALUES ('%s', %d, %d, '%s', '%s', '%s', %d, %d, %d, %d, %d, '%s') \
            ON DUPLICATE KEY UPDATE PROJECT_ID=VALUES(PROJECT_ID), sf=VALUES(sf), gwip=VALUES(gwip), \
            repeater=VALUES(repeater), channel=VALUES(channel), maplayer=VALUES(maplayer), posX=VALUES(posX), posY=VALUES(posY), posZ=VALUES(posZ), \
            maingroupid=VALUES(maingroupid), verifykey=VALUES(verifykey)",
            gateway->gwid.c_str(), gateway->projectid, gateway->sf, gateway->gwip.c_str(),
            gateway->repeater.c_str(), channel.c_str(), gateway->maplayer, gateway->posX, gateway->posY, gateway->posZ,
            gateway->maingroupid, gateway->verifykey.c_str());

    sql_execute(cmd);

}

void MySQLDBAccess::delete_LoRaGateway(std::string gwid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "DELETE from LoRaGatewayInfo WHERE gwid='%s'", gwid.c_str());

    sql_execute(cmd);
}

void MySQLDBAccess::write_TagGPSInfo(GPSInfoStruct * GPSInfo)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    int i_Longitude = (int)GPSInfo->Longitude;
    int i_Latitude = (int)GPSInfo->Latitude;

    if ( i_Longitude != 0 && i_Latitude != 0)
    {
        std::string DataTime = time_tToString(GPSInfo->updateTimestamp);

        sprintf(cmd, "INSERT INTO GPSInfoRecord(PROJECT_ID, nodetype, NodeId, Longitude, Latitude, DataTime) \
                VALUES (1, %d, '%s', '%f', '%f', '%s')",
                GPSInfo->nodetype, GPSInfo->nodeid.c_str(), GPSInfo->Longitude, GPSInfo->Latitude, DataTime.c_str());

        sql_execute(cmd);

        update_to_GPS_cache(GPSInfo->nodetype, GPSInfo->nodeid.c_str(), 1, DataTime.c_str(), DataTime.c_str(), DataTime.c_str(), DataTime.c_str());
    }

    // vilslog::printf("MySQLDBAccess::write_TagGPSInfo() cmd[%s]\n", cmd);
}

void MySQLDBAccess::update_Resident(UserResidentStruct * Resident)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    bool existed = check_exist_Resident(Resident->serno);

    if (existed)
    {
        sprintf(cmd, "UPDATE resident SET gender='%s', name='%s', roomNo='%s', residentType='%s', nurseNo='%s' WHERE serno='%s';",
                Resident->gender.c_str(), Resident->name.c_str(), Resident->roomNo.c_str(),
                Resident->residentType.c_str(), Resident->nurseNo.c_str(), Resident->serno.c_str());

        sql_execute(cmd);

        //printf("MySQLDBAccess::update_Resident() 1 cmd[%s]\n", cmd);
    }
    else
    {
        sprintf(cmd, "INSERT INTO resident(serno, gender, name, roomNo, residentType, nurseNo) \
                VALUES ('%s', '%s', '%s', '%s', '%s', '%s')",
                Resident->serno.c_str(), Resident->gender.c_str(), Resident->name.c_str(),
                Resident->roomNo.c_str(), Resident->residentType.c_str(), Resident->nurseNo.c_str());

        sql_execute(cmd);

        //printf("MySQLDBAccess::update_Resident() 2 cmd[%s]\n", cmd);
    }
}

void MySQLDBAccess::update_NodeResident(const char * macaddress, const char * serno)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    UserResidentStruct * resident = new UserResidentStruct();
    sprintf(cmd, "SELECT * FROM resident WHERE serno='%s'", serno);
    sql_executeQuery_new(cmd, get_resident_callback, resident);    //

    std::string nodename = resident->name;
    nodename.append(" ");
    nodename.append(resident->roomNo);

    delete resident;

    sprintf(cmd, "UPDATE NodeInfo set resident_serno='%s', nodename='%s' WHERE macaddress='%s';",
        serno, nodename.c_str(), macaddress);

    printf("MySQLDBAccess::update_NodeResident() cmd[%s]\n", cmd);

    sql_execute(cmd);

}

void MySQLDBAccess::update_NodeEmployee(const char * macaddress, const char * empid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    UserEmployeeStruct * employee = getEmployee(empid);

    std::string nodename = employee->cname;
    nodename.append(" ");
    nodename.append(employee->jobTitle);

    delete employee;

    sprintf(cmd, "UPDATE NodeInfo set empid='%s', nodename='%s' WHERE macaddress='%s';",
        empid, nodename.c_str(), macaddress);

    printf("MySQLDBAccess::update_NodeEmployee() cmd[%s]\n", cmd);

    sql_execute(cmd);

}

void MySQLDBAccess::update_NodeButtontype(const char * macaddress, const char * Buttontype)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE NodeInfo set tafid=%d WHERE macaddress='%s';", atoi(Buttontype), macaddress);

    printf("MySQLDBAccess::update_NodeButtontype() cmd[%s]\n", cmd);

    sql_execute(cmd);

}

void MySQLDBAccess::update_NodeSensorAlert(const char * macaddress, const char * SensorAlert)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE NodeInfo set enableSensorAlert=%d WHERE macaddress='%s';", atoi(SensorAlert), macaddress);

    printf("MySQLDBAccess::update_NodeSensorAlert() cmd[%s]\n", cmd);

    sql_execute(cmd);

}

void MySQLDBAccess::delete_Resident(UserResidentStruct * Resident)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "DELETE from resident WHERE serno='%s'", Resident->serno.c_str());

    vilslog::printf("MySQLDBAccess::delete_Resident() cmd[%s]\n", cmd);

    sql_execute(cmd);
}

bool MySQLDBAccess::check_exist_Resident(std::string serno)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT count(*) FROM resident WHERE serno='%s'", serno.c_str());

    int count = 0;
    sql_executeQuery_new(cmd, get_count_callback, &count);

    if (count == 0)
        return false;
    else
        return true;
}

UserResidentStruct * MySQLDBAccess::getResident(const char * serno)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM resident WHERE serno='%s'", serno);

    UserResidentStruct * resident = new UserResidentStruct();
    sql_executeQuery_new(cmd, get_resident_callback, resident);

    return resident;
}

std::list<UserResidentStruct *> * MySQLDBAccess::getResidentList()
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::list<UserResidentStruct *> * residentList = new std::list<UserResidentStruct *>();

    sprintf(cmd, "SELECT * FROM resident ");

    sql_executeQuery_new(cmd, get_residentList_callback, residentList);

    return residentList;
}

void MySQLDBAccess::update_Employee(UserEmployeeStruct * Employee)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    bool existed = check_exist_Employee(Employee->empid);

    if (existed)
    {
        sprintf(cmd, "UPDATE employee SET cname='%s', orgid='%s', orgName='%s', area='%s', jobTitle='%s', gender='%s', pwd='%s' WHERE empid='%s';",
                Employee->cname.c_str(), Employee->orgid.c_str(), Employee->orgName.c_str(), Employee->area.c_str(),
                Employee->jobTitle.c_str(), Employee->gender.c_str(), Employee->pwd.c_str(), Employee->empid.c_str());

        sql_execute(cmd);

        //vilslog::printf("MySQLDBAccess::update_Employee() 1 cmd[%s]\n", cmd);
    }
    else
    {
        sprintf(cmd, "INSERT INTO employee(empid, cname, orgid, orgName, area, jobTitle, gender, pwd) \
                VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')",
                Employee->empid.c_str(), Employee->cname.c_str(), Employee->orgid.c_str(), Employee->orgName.c_str(),
                Employee->area.c_str(), Employee->jobTitle.c_str(), Employee->gender.c_str(), Employee->pwd.c_str());

        sql_execute(cmd);

        //vilslog::printf("MySQLDBAccess::update_Employee() 2 cmd[%s]\n", cmd);
    }
}

void MySQLDBAccess::delete_Employee(UserEmployeeStruct * Employee)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "DELETE from employee WHERE empid='%s'", Employee->empid.c_str());

    vilslog::printf("MySQLDBAccess::delete_Employee() cmd[%s]\n", cmd);

    sql_execute(cmd);
}

bool MySQLDBAccess::check_exist_Employee(std::string empid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT count(*) FROM employee WHERE empid='%s'", empid.c_str());

    int count = 0;
    sql_executeQuery_new(cmd, get_count_callback, &count);

    if (count == 0)
        return false;
    else
        return true;
}

UserEmployeeStruct * MySQLDBAccess::getEmployee(std::string empid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM employee WHERE empid='%s'", empid.c_str());

    UserEmployeeStruct * employee = new UserEmployeeStruct();

    sql_executeQuery_new(cmd, get_employee_callback, employee);

    return employee;
}

std::list<UserEmployeeStruct *> * MySQLDBAccess::getEmployeeList()
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::list<UserEmployeeStruct *> * employeeList = new std::list<UserEmployeeStruct *>();

    sprintf(cmd, "SELECT * FROM employee");

    sql_executeQuery_new(cmd, get_employeeList_callback, employeeList);

    return employeeList;
}

void MySQLDBAccess::update_EmployeeEventReport(EmployeeEventReportStruct * record)
{
    // vilslog::printf("MySQLDBAccess::update_EmployeeEventReport() eventid_alert[%d] eventid_cancel[%d]\n", record->eventid_alert, record->eventid_cancel);
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO EmployeeEventReport(eventid_alert, eventid_cancel, emp_empid, emp_cname, emp_orgName, emp_jobTitle, \
                   comment, action, nodeid, locatorid, resident_serno, resident_name, resident_roomNo, eventtime, reponsetime) \
                   VALUES (%d, %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s') \
                   ON DUPLICATE KEY UPDATE emp_cname=VALUES(emp_cname), emp_orgName=VALUES(emp_orgName), emp_jobTitle=VALUES(emp_jobTitle), \
                   comment=VALUES(comment), action=VALUES(action), nodeid=VALUES(nodeid), locatorid=VALUES(locatorid), \
                   resident_name=VALUES(resident_name), resident_roomNo=VALUES(resident_roomNo), eventtime=VALUES(eventtime), \
                   reponsetime=VALUES(reponsetime) ",
                   record->eventid_alert, record->eventid_cancel, record->emp_empid.c_str(), record->emp_cname.c_str(), record->emp_orgName.c_str(), record->emp_jobTitle.c_str(),
                   record->comment.c_str(), record->action.c_str(), record->nodeid.c_str(), record->locatorid.c_str(),
                   record->resident_serno.c_str(), record->resident_name.c_str(), record->resident_roomNo.c_str(), record->eventtime.c_str(), record->reponsetime.c_str());

    //vilslog::printf("MySQLDBAccess::update_EmployeeEventReport() cmd[%s]\n", cmd);

    sql_execute(cmd);
}

std::list<EmployeeEventReportStruct*> * MySQLDBAccess::getEmployeeEventReport(std::string empid, std::string report_start_date, std::string report_end_date)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::list<EmployeeEventReportStruct*> * reportlist = new std::list<EmployeeEventReportStruct*>();

    sprintf(cmd, "SELECT * FROM EmployeeEventReport WHERE emp_empid='%s' AND eventtime BETWEEN '%s' AND '%s' order by eventtime DESC ",
        empid.c_str(), report_start_date.c_str(), report_end_date.c_str());

    sql_executeQuery_new(cmd, get_EmployeeEventReportList_callback, reportlist);

    return reportlist;
}

void MySQLDBAccess::update_User(UserEmployeeStruct * Employee)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    bool existed = check_exist_User(Employee->empid);

    if (existed)
    {
        sprintf(cmd, "UPDATE User SET account='%s', name='%s', unit='%s', title='%s', gender='%s', pwd='%s', groupid=2 WHERE empid='%s';",
                Employee->empid.c_str(), Employee->cname.c_str(), Employee->orgName.c_str(), Employee->jobTitle.c_str(),
                Employee->gender.c_str(), Employee->pwd.c_str(), Employee->empid.c_str());

        sql_execute(cmd);

        //printf("MySQLDBAccess::update_User() 1 cmd[%s]\n", cmd);
    }
    else
    {
        sprintf(cmd, "INSERT INTO User(groupid, empid, account, name, unit, title, gender, pwd) \
                VALUES ('2', '%s', '%s', '%s', '%s', '%s', '%s', '%s')",
                Employee->empid.c_str(), Employee->empid.c_str(), Employee->cname.c_str(), Employee->orgName.c_str(),
                Employee->jobTitle.c_str(), Employee->gender.c_str(), Employee->pwd.c_str());

        sql_execute(cmd);

        //printf("MySQLDBAccess::update_User() 2 cmd[%s]\n", cmd);
    }
}

void MySQLDBAccess::delete_User(UserEmployeeStruct * Employee)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "DELETE from User WHERE empid='%s'", Employee->empid.c_str());

    vilslog::printf("MySQLDBAccess::delete_User() cmd[%s]\n", cmd);

    sql_execute(cmd);
}

bool MySQLDBAccess::check_exist_User(std::string empid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT count(*) FROM User WHERE empid='%s'", empid.c_str());

    int count = 0;
    sql_executeQuery_new(cmd, get_count_callback, &count);

    if (count == 0)
        return false;
    else
        return true;
}

bool MySQLDBAccess::check_valid_User(const char * account, const char * password)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT pwd as stringvalue FROM User WHERE account='%s'", account);

    std::string stringvalue;
    sql_executeQuery_new(cmd, get_stringvalue_callback, &stringvalue);

    std::string pwd = create_MD5_encrypt_salt(std::string(ERP_MD5_KEY), std::string(password));

    // vilslog::printf("MySQLDBAccess::check_valid_User() account[%s] password[%s] stringvalue[%s] pwd[%s]\n", account, password, stringvalue.c_str(), pwd.c_str());

    if ( pwd.compare(stringvalue) == 0 )
        return true;
    else
        return false;
}

bool MySQLDBAccess::check_valid_User(const char * account, const char * password, UserStruct * UserData)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM User WHERE account='%s'", account);

    sql_executeQuery_new(cmd, get_User_callback, UserData);

    //std::string pwd = create_MD5_encrypt(std::string(ERP_MD5_KEY), std::string(password));
    std::string pwd = create_MD5_encrypt_salt(std::string(ERP_MD5_KEY), std::string(password));

    // vilslog::printf("MySQLDBAccess::check_valid_User() UserData->pwd[%s]\n", UserData->pwd.c_str());
    // vilslog::printf("MySQLDBAccess::check_valid_User()           pwd[%s]\n", pwd.c_str());

    if ( pwd.compare(UserData->pwd) == 0 )
        return true;
    else
        return false;
}

bool MySQLDBAccess::check_valid_User(const char * account, const char * password, const char * userid, UserStruct * UserData)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM User WHERE account='%s' and userid=%s ", account, userid);

    sql_executeQuery_new(cmd, get_User_callback, UserData);

    //std::string pwd = create_MD5_encrypt(std::string(ERP_MD5_KEY), std::string(password));
    std::string pwd = create_MD5_encrypt_salt(std::string(ERP_MD5_KEY), std::string(password));

    // vilslog::printf("MySQLDBAccess::check_valid_User() UserData->pwd[%s]\n", UserData->pwd.c_str());
    // vilslog::printf("MySQLDBAccess::check_valid_User() iUserid[%d] UserData->userid[%d]\n", iUserid, UserData->userid);
    // vilslog::printf("MySQLDBAccess::check_valid_User()           pwd[%s]\n", pwd.c_str());

    if ( pwd.compare(UserData->pwd) == 0)
        return true;
    else
        return false;
}

int MySQLDBAccess::check_Account_Login(std::string account, std::string key)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT userid as count FROM logintoken WHERE accountid='%s' AND loginkey='%s' ",
        account.c_str(), key.c_str());

    int userid = 0;
    sql_executeQuery_new(cmd, get_count_callback, &userid);

    // vilslog::printf("MySQLDBAccess::check_Account_Login() accountid[%s] loginkey[%s] count[%d]\n",
    //     account.c_str(), key.c_str(), count);

    return userid;
}

bool MySQLDBAccess::check_Account_Login(std::string account, std::string userid, std::string key)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT count(*) FROM logintoken WHERE accountid='%s' AND userid=%s AND loginkey='%s' ",
        account.c_str(), userid.c_str(), key.c_str());

    int count = 0;
    sql_executeQuery_new(cmd, get_count_callback, &count);

    // vilslog::printf("MySQLDBAccess::check_Account_Login() accountid[%s] loginkey[%s] count[%d]\n",
    //     account.c_str(), key.c_str(), count);

    return count;
}

bool MySQLDBAccess::update_Account_LoginKey(std::string account, int userid, std::string key, std::string token)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE logintoken SET loginkey='%s' WHERE accountid='%s' AND userid=%d AND token='%s'",
        key.c_str(), account.c_str(), userid, token.c_str());

    int ret = sql_executeUpdate(cmd);

    // vilslog::printf("MySQLDBAccess::update_Account_LoginKey() cmd[%s]\n", cmd);

    vilslog::printf("MySQLDBAccess::update_Account_LoginKey() UPDATE accountid[%s] userid[%d] loginkey[%s] token[%s] ret[%d]\n",
        account.c_str(), userid, key.c_str(), token.c_str(), ret);

    if ( ret <= 0 )
    {
        sprintf(cmd, "INSERT INTO logintoken(accountid, userid, loginkey, token) \
                VALUES ('%s', %d, '%s', '%s')", account.c_str(), userid, key.c_str(), token.c_str());

        ret = sql_execute(cmd);

        vilslog::printf("MySQLDBAccess::update_Account_LoginKey() INSERT accountid[%s] userid[%d] loginkey[%s] token[%s] ret[%d]\n",
            account.c_str(), userid, key.c_str(), token.c_str(), ret);
    }

    return true;
}

std::string MySQLDBAccess::query_Account_LoginKey(std::string accountid, std::string userid, std::string token)
{
    std::string loginkey = "";

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT loginkey as stringvalue FROM logintoken WHERE accountid='%s' AND userid=%s AND token='%s' ",
        accountid.c_str(), userid.c_str(), token.c_str());

    sql_executeQuery_new(cmd, get_stringvalue_callback, &loginkey);

    return loginkey;
}

bool MySQLDBAccess::update_user_password(std::string accountid, int userid, std::string password)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::string pwd = create_MD5_encrypt_salt(std::string(ERP_MD5_KEY), password);

    sprintf(cmd, "UPDATE User SET pwd='%s' WHERE userid=%d ", pwd.c_str(), userid);

    sql_execute(cmd);

    // clear logintoken
    sprintf(cmd, "UPDATE logintoken SET loginkey='' WHERE userid=%d AND accountid='%s' ", userid, accountid.c_str());
    sql_execute(cmd);

    sprintf(cmd, "UPDATE logintoken SET loginkey='' WHERE userid=0 AND accountid='%s' ", accountid.c_str());
    sql_execute(cmd);

    return true;
}

bool MySQLDBAccess::update_user_uwbcards(int userid, const char * uwbcards)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE User SET uwbcards='%s' WHERE userid=%d ", uwbcards, userid);

    sql_execute(cmd);

    return true;
}

bool MySQLDBAccess::update_UserStruct(UserStruct * user)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::string maingroups = convert_list_to_string(user->maingroups);
    std::string datagroups = convert_list_to_string(user->datagroups);
    std::string subgroups = convert_list_to_string(user->subgroups);
    std::string rfidcards = convert_list_to_string(user->rfidcards);
    std::string nfccards = convert_list_to_string(user->nfccards);
    std::string uwbcards = convert_list_to_string(user->uwbcards);
    std::string sensors = convert_list_to_string(user->sensors);
    std::string pwd = create_MD5_encrypt_salt(std::string(ERP_MD5_KEY), std::string("1234"));

    if (user->userid <= 0 )
    {
        sprintf(cmd, "INSERT INTO User(groupid, account, name, title, pwd, maingroups, datagroups, subgroups, rfidcards, nfccards, uwbcards, sensors, \
                      imageuid, maplayer, posX, posY, posZ, Longitude, Latitude, AreaId, AreaName, AreaId0D, AreaType0D, AreaType5Id, AreaType5Name) \
                      VALUES(%d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', \
                      '%s', %d, %d, %d, %d, '%s', '%s', '%s', '%s', '%s', %d, '%s', '%s') ",
            user->groupid, user->account.c_str(), user->name.c_str(), user->title.c_str(), pwd.c_str(), maingroups.c_str(), datagroups.c_str(),
            subgroups.c_str(), rfidcards.c_str(), nfccards.c_str(), uwbcards.c_str(), sensors.c_str(),
            user->imageuid.c_str(), user->maplayer, user->posX, user->posY, user->posZ,
            user->Longitude.c_str(), user->Latitude.c_str(), user->AreaId.c_str(), user->AreaName.c_str(),
            user->AreaId0D.c_str(), user->AreaType0D, user->AreaType5Id.c_str(), user->AreaType5Name.c_str());


        int dbId = sql_execute_get_insert_id(cmd);
        user->userid = dbId;

        // vilslog::printf("MySQLDBAccess::update_UserStruct() cmd[%s] dbId[%d]\n", cmd, dbId);
    }
    else
    {
        sprintf(cmd, "UPDATE User SET groupid=%d, account='%s', name='%s', title='%s', pwd='%s', maingroups='%s', datagroups='%s', \
                      subgroups='%s', rfidcards='%s', nfccards='%s', uwbcards='%s', sensors='%s', imageuid='%s', maplayer=%d, \
                      posX=%d, posY=%d, posZ=%d, Longitude='%s', Latitude='%s', AreaId='%s', AreaName='%s', AreaId0D='%s', AreaType0D=%d, \
                      AreaType5Id='%s', AreaType5Name='%s' WHERE userid=%d ",
            user->groupid, user->account.c_str(), user->name.c_str(), user->title.c_str(), user->pwd.c_str(), maingroups.c_str(), datagroups.c_str(),
            subgroups.c_str(), rfidcards.c_str(), nfccards.c_str(), uwbcards.c_str(), sensors.c_str(), user->imageuid.c_str(),
            user->maplayer, user->posX, user->posY, user->posZ, user->Longitude.c_str(), user->Latitude.c_str(), user->AreaId.c_str(), user->AreaName.c_str(),
            user->AreaId0D.c_str(), user->AreaType0D, user->AreaType5Id.c_str(), user->AreaType5Name.c_str(), user->userid);
        sql_execute(cmd);
        // vilslog::printf("MySQLDBAccess::update_UserStruct() cmd[%s]\n", cmd);
    }

    return true;
}

bool MySQLDBAccess::update_UserPoistion(int userid, double posX, double posY, double posZ)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE User SET posX=%d, posY=%d, posZ=%d WHERE userid=%d ", (int)posX, (int)posY, (int)posZ, userid);

    sql_execute(cmd);

    return true;
}

bool MySQLDBAccess::update_UserLocation(int userid, std::string Latitude, std::string Longitude)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE User SET Latitude='%s', Longitude='%s' WHERE userid=%d ", Latitude.c_str(), Longitude.c_str(), userid);

    sql_execute(cmd);

    return true;
}

bool MySQLDBAccess::update_UserGroup(UserGroupStruct * usergroup)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    if (usergroup->groupid <= 0 )
    {
        sprintf(cmd, "INSERT INTO UserGroup(groupname, permission) VALUES('%s', '%s') ",
            usergroup->groupname.c_str(), usergroup->permission.c_str());

        int dbId = sql_execute_get_insert_id(cmd);
        usergroup->groupid = dbId;
    }
    else
    {
        sprintf(cmd, "UPDATE UserGroup SET groupname='%s', permission='%s' WHERE groupid=%d ",
            usergroup->groupname.c_str(), usergroup->permission.c_str(), usergroup->groupid);
        sql_execute(cmd);
    }

    return true;
}

bool MySQLDBAccess::update_UserSubGroup(UserSubGroupStruct * usersubgroup)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::string accountids = convert_list_to_string(usersubgroup->accountidList);
    std::string userids = convert_list_to_string(usersubgroup->useridList);

    if (usersubgroup->subgroupid <= 0 )
    {
        sprintf(cmd, "INSERT INTO UserSubGroup(subgroupname, permission, accountids, userids, maingroupid, imageuid, type, validdate, curriculumrecordid) VALUES('%s', '%s', '%s', '%s', %d, '%s', %d, '%s', %d) ",
            usersubgroup->subgroupname.c_str(), usersubgroup->permission.c_str(), accountids.c_str(), userids.c_str(), usersubgroup->maingroupid,
            usersubgroup->imageuid.c_str(), usersubgroup->type, usersubgroup->validdate.c_str(), usersubgroup->curriculumrecordid);

        int dbId = sql_execute_get_insert_id(cmd);
        usersubgroup->subgroupid = dbId;

        // vilslog::printf("MySQLDBAccess::update_UserSubGroup() cmd[%s]\n", cmd);
        // vilslog::printf("MySQLDBAccess::update_UserSubGroup() dbId[%d]\n", dbId);
    }
    else
    {
        sprintf(cmd, "UPDATE UserSubGroup SET subgroupname='%s', permission='%s', accountids='%s', userids='%s', maingroupid=%d, imageuid='%s', type=%d, validdate='%s', curriculumrecordid=%d WHERE subgroupid=%d ",
            usersubgroup->subgroupname.c_str(), usersubgroup->permission.c_str(), accountids.c_str(), userids.c_str(), usersubgroup->maingroupid,
            usersubgroup->imageuid.c_str(), usersubgroup->type, usersubgroup->validdate.c_str(), usersubgroup->curriculumrecordid, usersubgroup->subgroupid);
        sql_execute(cmd);
        // vilslog::printf("MySQLDBAccess::update_UserSubGroup() cmd[%s]\n", cmd);
    }

    return true;
}

bool MySQLDBAccess::update_UserMainGroup(UserMainGroupStruct * usermaingroup)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::string accountids = convert_list_to_string(usermaingroup->accountidList);
    std::string userids = convert_list_to_string(usermaingroup->useridList);

    if (usermaingroup->id <= 0 )
    {
        sprintf(cmd, "INSERT INTO UserMainGroup(name, permission, accountids, userids, imageuid, autojoin, welcommessage) VALUES('%s', '%s', '%s', '%s', '%s', %d, '%s') ",
            usermaingroup->name.c_str(), usermaingroup->permission.c_str(), accountids.c_str(), userids.c_str(), usermaingroup->imageuid.c_str(), usermaingroup->autojoin, usermaingroup->welcommessage.c_str());

        int dbId = sql_execute_get_insert_id(cmd);
        usermaingroup->id = dbId;
    }
    else
    {
        sprintf(cmd, "UPDATE UserMainGroup SET name='%s', permission='%s', accountids='%s', userids='%s', imageuid='%s', autojoin=%d, welcommessage='%s' WHERE id=%d ",
            usermaingroup->name.c_str(), usermaingroup->permission.c_str(), accountids.c_str(), userids.c_str(), usermaingroup->imageuid.c_str(), usermaingroup->autojoin, usermaingroup->welcommessage.c_str(), usermaingroup->id);
        sql_execute(cmd);
    }

    return true;
}

std::list<UserStruct*> * MySQLDBAccess::get_Users()
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM User");

    std::list<UserStruct*> * userList = new std::list<UserStruct*>();

    sql_executeQuery_new(cmd, get_UserList_callback, userList);

    return userList;
}

UserStruct * MySQLDBAccess::getUser(std::string account)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM User where account='%s' ", account.c_str());

    UserStruct * user = new UserStruct();

    sql_executeQuery_new(cmd, get_User_callback, user);

    return user;
}

UserStruct * MySQLDBAccess::getUserByUserId(int userid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM User where userid=%d ", userid);

    UserStruct * user = new UserStruct();

    sql_executeQuery_new(cmd, get_User_callback, user);

    return user;
}

std::list<UserStruct*> * MySQLDBAccess::getUserByUWBCard(const char * uwbcard)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM User where uwbcards LIKE '%%%s%%' ", uwbcard);

    std::list<UserStruct*> * userList = new std::list<UserStruct*>();

    sql_executeQuery_new(cmd, get_UserList_callback, userList);

    return userList;
}

std::string MySQLDBAccess::getUserName(int userid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT name as nodename FROM User where userid=%d ", userid);

    std::string UserName = std::to_string(userid);
    sql_executeQuery_new(cmd, get_nodename_callback, &UserName);

    return UserName;
}

std::list<UserGroupStruct*> * MySQLDBAccess::get_UserGroups()
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM UserGroup");

    std::list<UserGroupStruct*> * userGroupList = new std::list<UserGroupStruct*>();

    sql_executeQuery_new(cmd, get_UserGroupList_callback, userGroupList);

    return userGroupList;
}

std::list<UserSubGroupStruct*> * MySQLDBAccess::get_UserSubGroups()
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM UserSubGroup");

    std::list<UserSubGroupStruct*> * userSubGroupList = new std::list<UserSubGroupStruct*>();

    sql_executeQuery_new(cmd, get_UserSubGroupList_callback, userSubGroupList);

    return userSubGroupList;
}

std::list<UserMainGroupStruct*> * MySQLDBAccess::get_UserMainGroups()
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM UserMainGroup");

    std::list<UserMainGroupStruct*> * userMainGroupList = new std::list<UserMainGroupStruct*>();

    sql_executeQuery_new(cmd, get_UserMainGroupList_callback, userMainGroupList);

    return userMainGroupList;
}

UserSubGroupStruct * MySQLDBAccess::get_UserSubGroup(int subgroupid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM UserSubGroup WHERE subgroupid=%d ", subgroupid);

    UserSubGroupStruct * userSubGroup = new UserSubGroupStruct();

    sql_executeQuery_new(cmd, get_UserSubGroup_callback, userSubGroup);

    return userSubGroup;
}

UserSubGroupStruct * MySQLDBAccess::get_UserSubGroupByCurriculumRecordID(int curriculumrecordid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM UserSubGroup WHERE curriculumrecordid=%d ", curriculumrecordid);

    UserSubGroupStruct * userSubGroup = new UserSubGroupStruct();
    userSubGroup->subgroupid = -1;
    userSubGroup->maingroupid = -1;
    userSubGroup->curriculumrecordid = -1;

    sql_executeQuery_new(cmd, get_UserSubGroup_callback, userSubGroup);

    return userSubGroup;
}

UserMainGroupStruct * MySQLDBAccess::get_UserMainGroup(int maingroupid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM UserMainGroup WHERE id=%d ", maingroupid);

    UserMainGroupStruct * userMainGroup = new UserMainGroupStruct();

    sql_executeQuery_new(cmd, get_UserMainGroup_callback, userMainGroup);

    return userMainGroup;
}

int MySQLDBAccess::getUnassignResidentCount(int nodetype)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT count(*) FROM NodeInfo WHERE nodetype=%d and resident_serno='-1' ", nodetype);

    int count = 0;
    sql_executeQuery_new(cmd, get_count_callback, &count);
    return count;
}

std::list<PushNotifySettingStruct*> * MySQLDBAccess::get_PushNotifySettings()
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM PushNotifySetting");

    std::list<PushNotifySettingStruct*> * settingList = new std::list<PushNotifySettingStruct*>();

    sql_executeQuery_new(cmd, get_PushNotifySettingList_callback, settingList);

    return settingList;
}

std::list<TriggerPushMessageStruct*> * MySQLDBAccess::get_TriggerPushMessages()
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM TriggerPushMessage");

    std::list<TriggerPushMessageStruct*> * settingList = new std::list<TriggerPushMessageStruct*>();

    sql_executeQuery_new(cmd, get_TriggerPushMessageList_callback, settingList);
    //
    // vilslog::printf("MySQLDBAccess::get_TriggerPushMessages() settingList->size()[%d]\n", (int)settingList->size());

    return settingList;
}

std::list<PushNotifyMessageRecordStruct*> * MySQLDBAccess::get_PushNotifyMessageRecords(const char * FromDatetime, const char * ToDatetime)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM PushNotifyMessageRecord WHERE datetime BETWEEN '%s' AND '%s' ORDER BY datetime DESC ;",
                FromDatetime, ToDatetime);

    // vilslog::printf("MySQLDBAccess::get_PushNotifyMessageRecords() cmd[%s]\n", cmd);

    std::list<PushNotifyMessageRecordStruct*> * recordList = new std::list<PushNotifyMessageRecordStruct*>();

    sql_executeQuery_new(cmd, get_PushNotifyMessageRecordList_callback, recordList);

    return recordList;
}

PushNotifyMessageRecordStruct * MySQLDBAccess::get_PushNotifyMessageRecord(std::string hashkey)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM PushNotifyMessageRecord WHERE messagehashkey='%s' ", hashkey.c_str());

    // vilslog::printf("MySQLDBAccess::get_PushNotifyMessageRecords() cmd[%s]\n", cmd);

    PushNotifyMessageRecordStruct * record = new PushNotifyMessageRecordStruct();

    sql_executeQuery_new(cmd, get_PushNotifyMessageRecord_callback, record);

    return record;
}

PushNotifyMessageRecordStruct * MySQLDBAccess::get_PushNotifyMessageRecord(int PushNotifyMessageRecorddbId)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM PushNotifyMessageRecord WHERE dbId=%d ", PushNotifyMessageRecorddbId);

    vilslog::printf("MySQLDBAccess::get_PushNotifyMessageRecords() cmd[%s]\n", cmd);

    PushNotifyMessageRecordStruct * record = new PushNotifyMessageRecordStruct();

    sql_executeQuery_new(cmd, get_PushNotifyMessageRecord_callback, record);

    return record;
}

std::list<SurveyEventRecordStruct*> * MySQLDBAccess::get_LeaveForm_SurveyEventRecord()
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM SurveyEventRecord WHERE surveyid = %s AND deadline >= '%s' ORDER BY datetime DESC ;",
        ApplicationForm_LEAVE_SURVEYID, currentDateTimeForSQL().c_str());

    std::list<SurveyEventRecordStruct*> * recordList = new std::list<SurveyEventRecordStruct*>();

    sql_executeQuery_new(cmd, get_SurveyEventRecordList_callback, recordList);

    return recordList;
}

ApplicationFormLeaveStruct * MySQLDBAccess::get_LeaveForm(std::string formid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM applicationformleave WHERE formid = '%s' ;", formid.c_str());

    ApplicationFormLeaveStruct * formRecord = new ApplicationFormLeaveStruct();

    sql_executeQuery_new(cmd, get_ApplicationFormLeave_callback, formRecord);

    return formRecord;
}

void MySQLDBAccess::update_LeaveForm(ApplicationFormLeaveStruct * record)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE applicationformleave SET signoffstatus=%d, signoffuserid=%d where formid = '%s' ;",
            record->signoffstatus, record->signoffuserid, record->formid.c_str());

    // sprintf(cmd, "UPDATE applicationformleave SET signoffstatus=%d, signoffaccountid='%s' where formid = '%s' ;",
    //         record->signoffstatus, record->signoffaccountid.c_str(), record->formid.c_str());

    sql_execute(cmd);
}

void MySQLDBAccess::update_SensorInfo(SensorInfoStruct * record)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO sensorinfo(name, day, value, jsonvalue, Longitude, Latitude) VALUES ('%s', '%s', '%s', '%s', '%s', '%s') ",
            record->name.c_str(), record->day.c_str(), record->value.c_str(), record->jsonvalue.c_str(), record->Longitude.c_str(), record->Latitude.c_str());

    sql_execute(cmd);
}

void MySQLDBAccess::update_SensorDevice(SensorDeviceStruct * record)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    int i_Longitude = 0;
    int i_Latitude = 0;
    if (record->Longitude.length() > 0)
    {
        i_Longitude = convert_string_to_int(record->Longitude);
    }
    if (record->Latitude.length() > 0)
    {
        i_Latitude = convert_string_to_int(record->Latitude);
    }

    // vilslog::printf ("MySQLDBAccess::update_SensorDevice() record->Latitude[%s] record->Longitude[%s] i_Longitude[%d] i_Latitude[%d]\n",
    //     record->Latitude.c_str(), record->Longitude.c_str(), i_Longitude, i_Latitude);


    if ( i_Longitude == 0 || i_Latitude == 0)
    {
        sprintf(cmd, "INSERT INTO sensordevice(alive, maingroupid, id, name, type, jsonvalue, Battery, Version, verifykey) VALUES (%d, %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s') \
                        ON DUPLICATE KEY UPDATE alive=VALUES(alive), type=VALUES(type), maingroupid=VALUES(maingroupid), jsonvalue=VALUES(jsonvalue), Battery=VALUES(Battery), Version=VALUES(Version), verifykey=VALUES(verifykey)",
                        record->alive, record->maingroupid, record->id.c_str(), record->name.c_str(), record->type.c_str(), record->jsonvalue.c_str(), record->Battery.c_str(), record->Version.c_str(), record->verifykey.c_str());
    }
    else
    {
        sprintf(cmd, "INSERT INTO sensordevice(alive, maingroupid, id, name, type, jsonvalue, Longitude, Latitude, Battery, Version, verifykey) VALUES (%d, %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s') \
                        ON DUPLICATE KEY UPDATE alive=VALUES(alive), type=VALUES(type), maingroupid=VALUES(maingroupid), jsonvalue=VALUES(jsonvalue), Longitude=VALUES(Longitude), Latitude=VALUES(Latitude), Battery=VALUES(Battery), Version=VALUES(Version), verifykey=VALUES(verifykey)",
                        record->alive, record->maingroupid, record->id.c_str(), record->name.c_str(), record->type.c_str(), record->jsonvalue.c_str(), record->Longitude.c_str(), record->Latitude.c_str(), record->Battery.c_str(), record->Version.c_str(), record->verifykey.c_str());
    }

    // vilslog::printf("MySQLDBAccess::update_SensorDevice() cmd[%s]\n", cmd);

    sql_execute(cmd);
}

void MySQLDBAccess::update_SensorFiredEvent(SensorFiredEventStruct * record)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO sensorevent(sensorid, type, jsonvalue, maingroupid) VALUES ('%s', '%s', '%s', %d) ",
                    record->sensorid.c_str(), record->type.c_str(), record->jsonvalue.c_str(), record->maingroupid);

    sql_execute(cmd);
}

void MySQLDBAccess::Transaction_update_FlyInfo(FlyInfoStruct * record)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    // PRIMARY KEY (FDATE, airLineNum, airLineCode)

    sprintf(cmd, "INSERT INTO FlyInfo(FDATE, expectTime, airLineNum, airLineCode, type, jsonvalue, maingroupid) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', %d) \
                    ON DUPLICATE KEY UPDATE expectTime=VALUES(expectTime), type=VALUES(type), jsonvalue=VALUES(jsonvalue), maingroupid=VALUES(maingroupid)",
                    record->FDATE.c_str(), record->expectTime.c_str(), record->airLineNum.c_str(), record->airLineCode.c_str(),
                    record->type.c_str(), record->jsonvalue.c_str(), record->maingroupid);

    std::string trans = cmd;
    mMysqlTransactionQueue.add(trans);

    if (record->triggerTime.length() > 0)
    {
        memset(cmd, 0, 4096);
        sprintf(cmd, "UPDATE FlyInfo SET triggerTime='%s' WHERE FDATE='%s' AND airLineNum='%s' AND airLineCode='%s' ",
                        record->triggerTime.c_str(), record->FDATE.c_str(), record->airLineNum.c_str(), record->airLineCode.c_str());
        std::string trans = cmd;
        mMysqlTransactionQueue.add(trans);
    }

    if (record->sensorid.length() > 0)
    {
        memset(cmd, 0, 4096);
        sprintf(cmd, "UPDATE FlyInfo SET sensorid='%s' WHERE FDATE='%s' AND airLineNum='%s' AND airLineCode='%s' ",
                        record->sensorid.c_str(), record->FDATE.c_str(), record->airLineNum.c_str(), record->airLineCode.c_str());
        std::string trans = cmd;
        mMysqlTransactionQueue.add(trans);
    }

    if (record->airBoardingGate.length() > 0)
    {
        memset(cmd, 0, 4096);
        sprintf(cmd, "UPDATE FlyInfo SET airBoardingGate='%s' WHERE FDATE='%s' AND airLineNum='%s' AND airLineCode='%s' ",
                        record->airBoardingGate.c_str(), record->FDATE.c_str(), record->airLineNum.c_str(), record->airLineCode.c_str());
        std::string trans = cmd;
        mMysqlTransactionQueue.add(trans);
    }
}

void MySQLDBAccess::Transaction_update_SensorInfo(SensorInfoStruct * record)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO sensorinfo(name, day, value, jsonvalue, Longitude, Latitude) VALUES ('%s', '%s', '%s', '%s', '%s', '%s') ",
            record->name.c_str(), record->day.c_str(), record->value.c_str(), record->jsonvalue.c_str(), record->Longitude.c_str(), record->Latitude.c_str());

    std::string trans = cmd;
    mMysqlTransactionQueue.add(trans);
}

void MySQLDBAccess::Transaction_update_SensorDevice(SensorDeviceStruct * record)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    int i_Longitude = 0;
    int i_Latitude = 0;
    if (record->Longitude.length() > 0)
    {
        i_Longitude = convert_string_to_int(record->Longitude);
    }
    if (record->Latitude.length() > 0)
    {
        i_Latitude = convert_string_to_int(record->Latitude);
    }

    if ( i_Longitude == 0 || i_Latitude == 0)
    {
        sprintf(cmd, "INSERT INTO sensordevice(alive, maingroupid, id, name, type, jsonvalue, Battery, Version, verifykey) VALUES (%d, %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s') \
                        ON DUPLICATE KEY UPDATE alive=VALUES(alive), type=VALUES(type), maingroupid=VALUES(maingroupid), jsonvalue=VALUES(jsonvalue), Battery=VALUES(Battery), Version=VALUES(Version), verifykey=VALUES(verifykey)",
                        record->alive, record->maingroupid, record->id.c_str(), record->name.c_str(), record->type.c_str(), record->jsonvalue.c_str(), record->Battery.c_str(), record->Version.c_str(), record->verifykey.c_str());
    }
    else
    {
        sprintf(cmd, "INSERT INTO sensordevice(alive, maingroupid, id, name, type, jsonvalue, Longitude, Latitude, Battery, Version, verifykey) VALUES (%d, %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s') \
                        ON DUPLICATE KEY UPDATE alive=VALUES(alive), type=VALUES(type), maingroupid=VALUES(maingroupid), jsonvalue=VALUES(jsonvalue), Longitude=VALUES(Longitude), Latitude=VALUES(Latitude), Battery=VALUES(Battery), Version=VALUES(Version), verifykey=VALUES(verifykey)",
                        record->alive, record->maingroupid, record->id.c_str(), record->name.c_str(), record->type.c_str(), record->jsonvalue.c_str(), record->Longitude.c_str(), record->Latitude.c_str(), record->Battery.c_str(), record->Version.c_str(), record->verifykey.c_str());
    }

    std::string trans = cmd;
    mMysqlTransactionQueue.add(trans);

}

void MySQLDBAccess::Transaction_update_WCInfo(WCInfoStruct * record)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO KIAWCInfo(GID, place, item, notetime, sendNotify, maingroupid) VALUES ('%s', '%s', '%s', '%s', %d, %d) \
            ON DUPLICATE KEY UPDATE place=VALUES(place), item=VALUES(item), notetime=VALUES(notetime), sendNotify=VALUES(sendNotify), maingroupid=VALUES(maingroupid)",
            record->GID.c_str(), record->place.c_str(), record->item.c_str(), record->notetime.c_str(), record->sendNotify, record->maingroupid);

    std::string trans = cmd;
    mMysqlTransactionQueue.add(trans);
}

std::list<SensorDeviceStruct*> * MySQLDBAccess::get_SensorDevices()
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM sensordevice ORDER BY datetime DESC ;");

    std::list<SensorDeviceStruct*> * recordList = new std::list<SensorDeviceStruct*>();

    sql_executeQuery_new(cmd, get_SensorDeviceList_callback, recordList);

    return recordList;
}

std::list<FlyInfoStruct*> * MySQLDBAccess::get_FlyInfos(std::string FDATE)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM FlyInfo WHERE FDATE='%s' ORDER BY datetime DESC ;", FDATE.c_str());

    std::list<FlyInfoStruct*> * recordList = new std::list<FlyInfoStruct*>();

    sql_executeQuery_new(cmd, get_FlyInfoList_callback, recordList);

    return recordList;
}

std::list<FlyGateInfoStruct*> * MySQLDBAccess::get_FlyGateInfos()
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT max(airLineCode) as airLineCode, max(airLineNum) as airLineNum, max(airBoardingGate) as airBoardingGate FROM FlyInfo WHERE LENGTH(airBoardingGate) > 0 GROUP BY airLineNum ");

    std::list<FlyGateInfoStruct*> * recordList = new std::list<FlyGateInfoStruct*>();

    sql_executeQuery_new(cmd, get_FlyGateInfoList_callback, recordList);

    return recordList;
}

std::string MySQLDBAccess::get_SensorGateInfo(std::string sensorid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    //sprintf(cmd, "SELECT airBoardingGate as stringvalue FROM FlyInfo WHERE sensorid='%s' AND LENGTH(airBoardingGate) > 0 LIMIT 1 ", sensorid.c_str());
    sprintf(cmd, "SELECT airBoardingGate as stringvalue, count(airBoardingGate) as valuecount  FROM FlyInfo WHERE sensorid='%s' GROUP BY airBoardingGate ORDER BY valuecount DESC LIMIT 1 ", sensorid.c_str());

    std::string gate;
    sql_executeQuery_new(cmd, get_stringvalue_callback, &gate);

    return gate;
}

int MySQLDBAccess::get_WCInfoSendNotifyStatus(std::string GID)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT sendNotify as count FROM KIAWCInfo WHERE GID='%s' ", GID.c_str());

    int sendNotifyStatus = 0;
    sql_executeQuery_new(cmd, get_count_callback, &sendNotifyStatus);

    return sendNotifyStatus;
}

std::list<SurveyEventRecordStruct*> * MySQLDBAccess::get_Assist_SurveyEventRecords()
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM SurveyEventRecord WHERE surveyid = %s AND deadline >= '%s' ORDER BY datetime DESC ;",
        ApplicationForm_ASSIST_ID, currentDateTimeForSQL().c_str());

    std::list<SurveyEventRecordStruct*> * recordList = new std::list<SurveyEventRecordStruct*>();

    sql_executeQuery_new(cmd, get_SurveyEventRecordList_callback, recordList);

    return recordList;
}

SurveyEventRecordStruct * MySQLDBAccess::get_SurveyEventRecord(int PushNotifyMessageRecorddbId)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM SurveyEventRecord WHERE PushNotifyMessageRecorddbId=%d ", PushNotifyMessageRecorddbId);

    SurveyEventRecordStruct * record = new SurveyEventRecordStruct();

    sql_executeQuery_new(cmd, get_SurveyEventRecord_callback, record);

    // vilslog::printf("MySQLDBAccess::get_SurveyEventRecord() cmd=\n%s\n", cmd);

    return record;
}

AssistEventStruct * MySQLDBAccess::get_AssistEvent(std::string eventid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM assistevent WHERE id = %s ;", eventid.c_str());

    AssistEventStruct * event = new AssistEventStruct();

    sql_executeQuery_new(cmd, get_AssistEventStruct_callback, event);

    return event;
}

void MySQLDBAccess::update_AssistEvent(AssistEventStruct * event)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE assistevent SET signoffstatus=%d, signoffuserid=%d where id = %d ;",
            event->signoffstatus, event->signoffuserid, event->id);

    // sprintf(cmd, "UPDATE assistevent SET signoffstatus=%d, signoffaccountid='%s' where id = %d ;",
    //         event->signoffstatus, event->signoffaccountid.c_str(), event->id);

    sql_execute(cmd);
}

std::list<SurveyEventRecordStruct*> * MySQLDBAccess::get_Curriculum_SurveyEventRecords()
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM SurveyEventRecord WHERE surveyid = %s AND deadline >= '%s' ORDER BY datetime DESC ;",
        ApplicationForm_CURRICULUM_APPLYID, currentDateTimeForSQL().c_str());

    std::list<SurveyEventRecordStruct*> * recordList = new std::list<SurveyEventRecordStruct*>();

    sql_executeQuery_new(cmd, get_SurveyEventRecordList_callback, recordList);

    return recordList;
}

CurriculumRecordStruct * MySQLDBAccess::get_CurriculumRecord(std::string recordid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM CurriculumRecord WHERE id=%s ", recordid.c_str());

    CurriculumRecordStruct * record = new CurriculumRecordStruct();

    sql_executeQuery_new(cmd, get_CurriculumRecord_callback, record);

    // vilslog::printf("MySQLDBAccess::get_CurriculumRecord() cmd=\n%s\n", cmd);

    return record;
}

CurriculumStruct * MySQLDBAccess::get_Curriculum(int curriculumid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM Curriculum WHERE id=%d ", curriculumid);

    CurriculumStruct * record = new CurriculumStruct();

    sql_executeQuery_new(cmd, get_Curriculum_callback, record);

    // vilslog::printf("MySQLDBAccess::get_Curriculum() cmd=\n%s\n", cmd);

    return record;
}

CurriculumTypeStruct * MySQLDBAccess::get_CurriculumType(int curriculumtypeid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM CurriculumType WHERE id=%d ", curriculumtypeid);

    CurriculumTypeStruct * record = new CurriculumTypeStruct();

    sql_executeQuery_new(cmd, get_CurriculumType_callback, record);

    // vilslog::printf("MySQLDBAccess::get_CurriculumType() cmd=\n%s\n", cmd);

    return record;
}

CurriculumApplyStruct * MySQLDBAccess::get_CurriculumApply(std::string recordid, int userid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM CurriculumApply WHERE userid=%d AND curriculumrecordid=%s ", userid, recordid.c_str());

    CurriculumApplyStruct * record = new CurriculumApplyStruct();

    sql_executeQuery_new(cmd, get_CurriculumApply_callback, record);

    // vilslog::printf("MySQLDBAccess::get_CurriculumType() cmd=\n%s\n", cmd);

    return record;
}

void MySQLDBAccess::update_CurriculumApply(CurriculumApplyStruct * record)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE CurriculumApply SET status=%d WHERE userid=%d AND curriculumrecordid=%d",
            record->status, record->userid, record->curriculumrecordid);

    sql_execute(cmd);
}

void MySQLDBAccess::update_PushNotifyMessage(PushNotifyMessageStruct * msg)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO PushNotifyMessage(type, nodeid, targetid, topic, messagehashkey, messageid, maingroups) VALUES (%d, '%s', '%s', '%s', '%s', '%s', '%s')",
            msg->type, msg->nodeid.c_str(), msg->targetid.c_str(), msg->topic.c_str(), msg->messagehashkey.c_str(), msg->messageid.c_str(), msg->maingroups.c_str());

    sql_execute(cmd);

}

void MySQLDBAccess::Transaction_write_PushNotifyMessage(PushNotifyMessageStruct * msg)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::string maingroups = msg->maingroups;
    if ( maingroups.length() == 0 )
    {
        maingroups = "1";
    }

    sprintf(cmd, "INSERT INTO PushNotifyMessage(type, nodeid, targetid, topic, messagehashkey, messageid, maingroups) VALUES (%d, '%s', '%s', '%s', '%s', '%s', '%s')",
            msg->type, msg->nodeid.c_str(), msg->targetid.c_str(), msg->topic.c_str(), msg->messagehashkey.c_str(), msg->messageid.c_str(), maingroups.c_str());

    std::string trans = cmd;
    mMysqlTransactionQueue.add(trans);
}

void MySQLDBAccess::update_PushNotifyMessageRecord(PushNotifyMessageRecordStruct * msgrecord)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::string maingroups = msgrecord->maingroups;
    if ( maingroups.length() == 0 )
    {
        maingroups = "1";
    }

    sprintf(cmd, "INSERT INTO PushNotifyMessageRecord(type, nodeid, title, message, messagehashkey, targetids, subgroupids, \
                  surveyids, hashkeys, deadlines, userid, imageuid, maingroups) VALUES (%d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d, '%s', '%s')",
            msgrecord->type, msgrecord->nodeid.c_str(), msgrecord->title.c_str(), msgrecord->message.c_str(),
            msgrecord->messagehashkey.c_str(), msgrecord->targetids.c_str(), msgrecord->subgroupids.c_str(), msgrecord->surveyids.c_str(),
            msgrecord->hashkeys.c_str(), msgrecord->deadlines.c_str(), msgrecord->userid, msgrecord->imageuid.c_str(), maingroups.c_str());

    if(msgrecord->surveyids.length() > 0)
    {
        int dbId = sql_execute_get_insert_id(cmd);

        if (dbId > 0)
        {
            std::list<std::string> * surveyidList = convert_string_to_list(msgrecord->surveyids, ',');
            std::list<std::string> * hashkeyList = convert_string_to_list(msgrecord->hashkeys, ',');
            std::list<std::string> * deadlineList = convert_string_to_list(msgrecord->deadlines, ',');

            std::list<std::string>::iterator hashkey = hashkeyList->begin();
            std::list<std::string>::iterator deadline = deadlineList->begin();

            for (std::list<std::string>::iterator id = surveyidList->begin(); id != surveyidList->end(); id++, hashkey++, deadline++)
            {
                int surveyid = atoi((*id).c_str());
                sprintf(cmd, "INSERT INTO SurveyEventRecord(PushNotifyMessageRecorddbId, surveyid, hashkey, deadline, formid, userid, maingroups) VALUES (%d, %d, '%s', '%s', '%s', %d, '%s')",
                    dbId, surveyid, (*hashkey).c_str(), (*deadline).c_str(), msgrecord->formid.c_str(), msgrecord->userid, maingroups.c_str());

                // vilslog::printf("MySQLDBAccess::update_PushNotifyMessageRecord() cmd[%s]\n", cmd);

                sql_execute(cmd);

                //
                // 檢查是否是 協助事項
                //
                if ( (*id).compare(ApplicationForm_ASSISTEVENT_ID) == 0 && msgrecord->formid.length() > 0)
                {
                    // vilslog::printf("MySQLDBAccess::update_PushNotifyMessageRecord() (*id).c_str()[%s] ApplicationForm_ASSISTEVENT_ID[%s]\n", (*id).c_str(), ApplicationForm_ASSISTEVENT_ID);

                    sprintf(cmd, "UPDATE assisteventrecord SET PushNotifyMessageRecorddbId=%d, messagehashkey='%s' WHERE assisteventid=%d AND eventday='%s' AND eventtime='%s' ",
                                dbId, msgrecord->messagehashkey.c_str(), convert_string_to_int(msgrecord->formid), msgrecord->eventday.c_str(), msgrecord->eventtime.c_str());

                    sql_execute(cmd);
                }
            }

            surveyidList->clear();
            delete surveyidList;
            hashkeyList->clear();
            delete hashkeyList;
            deadlineList->clear();
            delete deadlineList;
        }
    }
    else
    {
        sql_execute(cmd);
    }
}


void MySQLDBAccess::Transaction_write_PushNotifyMessageRecord(PushNotifyMessageRecordStruct * msgrecord)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO PushNotifyMessageRecord(type, nodeid, title, message, messagehashkey, targetids, subgroupids, \
                  surveyids, hashkeys, deadlines, userid, imageuid, maingroups) VALUES (%d, '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', %d, '%s', '%s')",
            msgrecord->type, msgrecord->nodeid.c_str(), msgrecord->title.c_str(), msgrecord->message.c_str(),
            msgrecord->messagehashkey.c_str(), msgrecord->targetids.c_str(), msgrecord->subgroupids.c_str(), msgrecord->surveyids.c_str(),
            msgrecord->hashkeys.c_str(), msgrecord->deadlines.c_str(), msgrecord->userid, msgrecord->imageuid.c_str(), msgrecord->maingroups.c_str());

    std::string trans = cmd;
    mMysqlTransactionQueue.add(trans);
}

int MySQLDBAccess::check_PushNotifyMessageRecord_hashkey(std::string hashkey)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT count(*) FROM PushNotifyMessageRecord WHERE messagehashkey='%s' ", hashkey.c_str());

    int count = 0;
    sql_executeQuery_new(cmd, get_count_callback, &count);

    // vilslog::printf("check_PushNotifyMessageRecord_hashkey() cmd[%s] count[%d]\n", cmd, count);

    return count;
}

int MySQLDBAccess::check_SurveyEventRecord_hashkey(std::string hashkey)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT count(*) FROM SurveyEventRecord WHERE hashkey='%s' ", hashkey.c_str());

    int count = 0;
    sql_executeQuery_new(cmd, get_count_callback, &count);

    // vilslog::printf("check_SurveyEventRecord_hashkey() cmd[%s] count[%d]\n", cmd, count);

    return count;
}

void MySQLDBAccess::update_node_PushNotifySetting(int nodetype, const char * nodeid, int role)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::string functions = get_role_default_functions(nodetype, role);

    sprintf(cmd, "INSERT INTO PushNotifySetting(nodeid, targetid, functions) VALUES ('%s', '%s', '%s') \
                    ON DUPLICATE KEY UPDATE functions=VALUES(functions)",
                    nodeid, nodeid, functions.c_str());

    sql_execute(cmd);
}

void MySQLDBAccess::update_GAESurveyResultData(GAESurveyResultData * result)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::string comment = result->comment;
    replaceAll(comment,"'","\\'");

    std::string results = convertListToString(&result->results);

    sprintf(cmd, "INSERT INTO SurveyResult(hashkey, token, topic, messagehashkey, comment, results, datetime) VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s') \
                    ON DUPLICATE KEY UPDATE topic=VALUES(topic), comment=VALUES(comment), results=VALUES(results), datetime=VALUES(datetime)",
                    result->hashkey.c_str(), result->token.c_str(), result->topic.c_str(), result->messagehashkey.c_str(), comment.c_str(), results.c_str(), result->datetime.c_str());

    // vilslog::printf("MySQLDBAccess::update_GAESurveyResultData() cmd[%s]\n", cmd);

    sql_execute(cmd);

}

GAESurveyResultData * MySQLDBAccess::get_SurveyResult(std::string hashkey, std::string messagehashkey)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * From SurveyResult WHERE hashkey='%s' AND messagehashkey='%s' ORDER BY updatetime DESC LIMIT 1  ", hashkey.c_str(), messagehashkey.c_str());

    GAESurveyResultData * record = new GAESurveyResultData();

    // vilslog::printf("MySQLDBAccess::get_SurveyResult() cmd[%s]\n", cmd);

    sql_executeQuery_new(cmd, get_SurveyResult_callback, record);

    return record;
}

void MySQLDBAccess::update_GAEMessageStatusData(GAEMessageStatusData * status)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO PushNotifyMessageStatus(token, messageid, topic, messagehashkey, readed, finished, datetime) VALUES ('%s', '%s', '%s', '%s', %s, %s, '%s') \
                    ON DUPLICATE KEY UPDATE readed=VALUES(readed), topic=VALUES(topic), messagehashkey=VALUES(messagehashkey), finished=VALUES(finished), datetime=VALUES(datetime)",
                    status->token.c_str(), status->messageid.c_str(), status->topic.c_str(), status->messagehashkey.c_str(), status->read.c_str(), status->finished.c_str(), status->datetime.c_str());

    // vilslog::printf("MySQLDBAccess::update_GAEMessageStatusData() cmd[%s]\n", cmd);

    sql_execute(cmd);
}

void MySQLDBAccess::update_TargetTopicRecord(TargetTopicStruct * record)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO TargetTopic(target, topic, name, accountid, maingroupid, userid) VALUES ('%s', '%s', '%s', '%s', %d, %d) \
                    ON DUPLICATE KEY UPDATE topic=VALUES(topic), name=VALUES(name), accountid=VALUES(accountid), maingroupid=VALUES(maingroupid), userid=VALUES(userid)",
                    record->target.c_str(), record->topic.c_str(), record->name.c_str(), record->accountid.c_str(), record->maingroupid, record->userid);

    sql_execute(cmd);
}

void MySQLDBAccess::Transaction_write_TargetTopicRecord(TargetTopicStruct * record)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO TargetTopic(target, topic, name, accountid, maingroupid, userid) VALUES ('%s', '%s', '%s', '%s', %d, %d) \
                    ON DUPLICATE KEY UPDATE topic=VALUES(topic), name=VALUES(name), accountid=VALUES(accountid), maingroupid=VALUES(maingroupid), userid=VALUES(userid)",
                    record->target.c_str(), record->topic.c_str(), record->name.c_str(), record->accountid.c_str(), record->maingroupid, record->userid);

    std::string trans = cmd;
    mMysqlTransactionQueue.add(trans);
}

std::string MySQLDBAccess::createNewTopic()
{
    std::string newTopic = "";
    char cmd[4096];
    memset(cmd, 0, 4096);

    int count = 0;
    do
    {
        newTopic = random_string(16);

        sprintf(cmd, "SELECT count(*) FROM TargetTopic WHERE topic='%s'", newTopic.c_str());

        sql_executeQuery_new(cmd, get_count_callback, &count);

    }while(count > 0);

    return newTopic;
}

std::list<TargetTopicStruct*> * MySQLDBAccess::get_TargetTopics()
{
    std::list<TargetTopicStruct*> * recordList = new std::list<TargetTopicStruct*>();
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM TargetTopic");

    sql_executeQuery_new(cmd, get_TargetTopicStructList_callback, recordList);
    return recordList;
}

TargetTopicStruct * MySQLDBAccess::get_UserTargetTopic(int userid, std::string accountid)
{
    TargetTopicStruct * record = new TargetTopicStruct();
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM TargetTopic WHERE userid=%d AND accountid='%s' ", userid, accountid.c_str());

    bool bExisted = sql_executeQuery_new(cmd, get_TargetTopicStruct_callback, record);

    vilslog::printf("get_UserTargetTopic() userid[%d] bExisted[%d]\n", userid, bExisted);

    return record;
}

TargetTopicStruct * MySQLDBAccess::get_TargetTopic(std::string target, std::string type, std::string id, std::string accountid, std::string maingroupid)
{
    TargetTopicStruct * record = new TargetTopicStruct();
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM TargetTopic WHERE target='%s' ", target.c_str());

    bool bExisted = sql_executeQuery_new(cmd, get_TargetTopicStruct_callback, record);

    vilslog::printf("MySQLDBAccess::get_TargetTopic() target[%s] bExisted[%d]\n", target.c_str(), bExisted);

    if (!bExisted)
    {
        memset(cmd, 0, 4096);

        record->target = target;
        record->topic = random_string(16);
        record->datetime = currentDateTimeForSQL();
        record->userid = 0;
        if ( type.compare("USER") == 0 )
        {
            record->name = accountid;
            record->accountid = accountid;
            if (id.length() > 0)
                record->userid = convert_string_to_int(id);
        }
        else
        if ( type.compare("SUBGROUP") == 0 )
        {
            if (id.length() > 0)
            {
                int subgroupid = convert_string_to_int(id);
                UserSubGroupStruct * userSubGroup = get_UserSubGroup(subgroupid);
                record->name = userSubGroup->subgroupname;
                record->accountid = accountid;

                clear_UserSubGroupStruct(userSubGroup);
                delete userSubGroup;
            }
        }
        else
        if ( type.compare("MAINGROUP") == 0 )
        {
            if (id.length() > 0)
            {
                int maingroupid = convert_string_to_int(id);
                UserMainGroupStruct * userMainGroup = get_UserMainGroup(maingroupid);
                record->name = userMainGroup->name;
                record->accountid = accountid;

                clear_UserMainGroupStruct(userMainGroup);
                delete userMainGroup;
            }
        }
        else
        {
            record->name = accountid;
            record->accountid = accountid;
        }

        sprintf(cmd, "INSERT INTO TargetTopic(target, topic, name, accountid, maingroupid, userid) VALUES ('%s', '%s', '%s', '%s', %s, %d) \
                        ON DUPLICATE KEY UPDATE topic=VALUES(topic), name=VALUES(name), accountid=VALUES(accountid), maingroupid=VALUES(maingroupid), userid=VALUES(userid)",
                        record->target.c_str(), record->topic.c_str(), record->name.c_str(), record->accountid.c_str(), maingroupid.c_str(), record->userid);


        vilslog::printf("MySQLDBAccess::get_TargetTopic() cmd[%s]\n", cmd);

        sql_execute(cmd);
    }

    return record;
}

TargetTopicStruct * MySQLDBAccess::get_TargetTopicFromTopic(std::string topic)
{
    TargetTopicStruct * record = new TargetTopicStruct();
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM TargetTopic WHERE topic='%s' ", topic.c_str());

    bool bExisted = sql_executeQuery_new(cmd, get_TargetTopicStruct_callback, record);

    vilslog::printf("get_TargetTopicFromTopic() topic[%s] bExisted[%d]\n", topic.c_str(), bExisted);

    return record;
}

TargetTopicStruct * MySQLDBAccess::get_TargetTopicFromTarget(std::string target)
{
    TargetTopicStruct * record = new TargetTopicStruct();
    record->userid = 0;
    record->maingroupid = 0;

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM TargetTopic WHERE target='%s' ", target.c_str());

    bool bExisted = sql_executeQuery_new(cmd, get_TargetTopicStruct_callback, record);

    vilslog::printf("get_TargetTopicFromTarget() target[%s] bExisted[%d]\n", target.c_str(), bExisted);

    return record;
}

void MySQLDBAccess::update_RFIDCardRecord(RFIDCardStruct * record)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO RFIDCard(alive, cardid, name, type, maplayer, userid, unknown, deviceid, maingroupid) VALUES (%d, '%s', '%s', '%s', %d, %d, %d, %d, %d) \
                  ON DUPLICATE KEY UPDATE alive=VALUES(alive), name=VALUES(name), type=VALUES(type), maplayer=VALUES(maplayer), userid=VALUES(userid), unknown=VALUES(unknown), deviceid=VALUES(deviceid), maingroupid=VALUES(maingroupid)",
        record->alive, record->cardid.c_str(), record->name.c_str(), record->type.c_str(), record->maplayer, record->userid, record->unknown, record->deviceid, record->maingroupid);

    sql_execute(cmd);
}

void MySQLDBAccess::Transaction_write_RFIDCardRecord(RFIDCardStruct * record)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO RFIDCard(alive, cardid, name, type, maplayer, userid, unknown, deviceid, maingroupid) VALUES (%d, '%s', '%s', '%s', %d, %d, %d, %d, %d) \
                  ON DUPLICATE KEY UPDATE alive=VALUES(alive), name=VALUES(name), type=VALUES(type), maplayer=VALUES(maplayer), userid=VALUES(userid), unknown=VALUES(unknown), deviceid=VALUES(deviceid), maingroupid=VALUES(maingroupid)",
        record->alive, record->cardid.c_str(), record->name.c_str(), record->type.c_str(), record->maplayer, record->userid, record->unknown, record->deviceid, record->maingroupid);

    std::string trans = cmd;
    mMysqlTransactionQueue.add(trans);
}

void MySQLDBAccess::update_RFIDCardRollCallRecord(std::string cardid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO RFIDCard(cardid) VALUES ('%s') ON DUPLICATE KEY UPDATE cardid=VALUES(cardid)", cardid.c_str());

    sql_execute(cmd);
}

void MySQLDBAccess::update_NFCCardRecord(NFCCardStruct * record)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO NFCCard(alive, cardid, name, type, maplayer, userid, unknown, deviceid, maingroupid) VALUES (%d, '%s', '%s', '%s', %d, %d, %d, %d, %d) \
                  ON DUPLICATE KEY UPDATE alive=VALUES(alive), name=VALUES(name), type=VALUES(type), maplayer=VALUES(maplayer), userid=VALUES(userid), unknown=VALUES(unknown), deviceid=VALUES(deviceid), maingroupid=VALUES(maingroupid)",
        record->alive, record->cardid.c_str(), record->name.c_str(), record->type.c_str(), record->maplayer, record->userid, record->unknown, record->deviceid, record->maingroupid);

    sql_execute(cmd);
}

void MySQLDBAccess::Transaction_write_NFCCardRecord(NFCCardStruct * record)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO NFCCard(alive, cardid, name, type, maplayer, userid, unknown, deviceid, maingroupid) VALUES (%d, '%s', '%s', '%s', %d, %d, %d, %d, %d) \
                  ON DUPLICATE KEY UPDATE alive=VALUES(alive), name=VALUES(name), type=VALUES(type), maplayer=VALUES(maplayer), userid=VALUES(userid), unknown=VALUES(unknown), deviceid=VALUES(deviceid), maingroupid=VALUES(maingroupid)",
        record->alive, record->cardid.c_str(), record->name.c_str(), record->type.c_str(), record->maplayer, record->userid, record->unknown, record->deviceid, record->maingroupid);

    std::string trans = cmd;
    mMysqlTransactionQueue.add(trans);
}

void MySQLDBAccess::Transaction_write_MaingroupNFCCardRecord(NFCCardStruct * record)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO MaingroupNFCCard(alive, cardid, name, type, maplayer, userid, unknown, deviceid, maingroupid) VALUES (%d, '%s', '%s', '%s', %d, %d, %d, %d, %d) \
                  ON DUPLICATE KEY UPDATE alive=VALUES(alive), name=VALUES(name), type=VALUES(type), maplayer=VALUES(maplayer), userid=VALUES(userid), unknown=VALUES(unknown), deviceid=VALUES(deviceid)",
        record->alive, record->cardid.c_str(), record->name.c_str(), record->type.c_str(), record->maplayer, record->userid, record->unknown, record->deviceid, record->maingroupid);

    std::string trans = cmd;
    mMysqlTransactionQueue.add(trans);
}

// void MySQLDBAccess::update_NFCCardRollCallRecord(std::string cardid)
// {
//     char cmd[4096];
//     memset(cmd, 0, 4096);
//
//     sprintf(cmd, "INSERT INTO NFCCard(cardid) VALUES ('%s') ON DUPLICATE KEY UPDATE cardid=VALUES(cardid)", cardid.c_str());
//
//     sql_execute(cmd);
// }

void MySQLDBAccess::clear_UnknownNFCCardAlive(std::string updatetime)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE NFCCard SET alive=0 WHERE unknown=1 AND updatetime <= '%s' ", updatetime.c_str());
    sql_execute(cmd);

    sprintf(cmd, "UPDATE MaingroupNFCCard SET alive=0 WHERE unknown=1 AND updatetime <= '%s' ", updatetime.c_str());
    sql_execute(cmd);
}

void MySQLDBAccess::clear_UnknownRFIDCardAlive(std::string updatetime)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE RFIDCard SET alive=0 WHERE unknown=1 AND  updatetime <= '%s' ", updatetime.c_str());

    // vilslog::printf("MySQLDBAccess::clear_UnknownRFIDCardAlive() cmd[%s]\n", cmd);

    sql_execute(cmd);
}

void MySQLDBAccess::remove_RFIDCard_user(int userid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE RFIDCard SET userid=0 WHERE userid=%d", userid);

    sql_execute(cmd);
}

void MySQLDBAccess::remove_NFCCard_user(int userid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE NFCCard SET userid=0 WHERE userid=%d", userid);

    sql_execute(cmd);
}

void MySQLDBAccess::remove_MaingroupNFCCard_user(int userid, std::string maingroups)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    if (maingroups.length() > 0)
    {
        sprintf(cmd, "UPDATE MaingroupNFCCard SET userid=0 WHERE userid=%d AND maingroupid in(%s) ", userid, maingroups.c_str());

        sql_execute(cmd);
    }
}

void MySQLDBAccess::remove_UWBCard_user(int userid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE NodeInfo SET userid=0 WHERE userid=%d AND nodetype=%d ", userid, NODE_TYPE_TAG);

    sql_execute(cmd);
}

void MySQLDBAccess::update_RFIDCard_user(std::string cardid, int userid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE RFIDCard SET userid=%d, type=1, unknown=0 WHERE cardid='%s'", userid, cardid.c_str());

    // vilslog::printf("MySQLDBAccess::update_RFIDCard_user() cmd[%s]\n", cmd);

    sql_execute(cmd);

    sprintf(cmd, "INSERT INTO RFIDCard(cardid, name, userid, type, unknown) VALUES ('%s', '%s', %d, 1, 0) ON DUPLICATE KEY UPDATE userid=VALUES(userid), name=VALUES(name), type=VALUES(type), unknown=VALUES(unknown)",
        cardid.c_str(), cardid.c_str(), userid);

    // vilslog::printf("MySQLDBAccess::update_RFIDCard_user() cmd[%s]\n", cmd);

    sql_execute(cmd);
}

void MySQLDBAccess::update_NFCCard_user(std::string cardid, int userid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE NFCCard SET userid=%d, type=1, unknown=0 WHERE cardid='%s'", userid, cardid.c_str());

    // vilslog::printf("MySQLDBAccess::update_NFCCard_user() cmd[%s]\n", cmd);

    sql_execute(cmd);

    sprintf(cmd, "INSERT INTO NFCCard(cardid, name, userid, type, unknown) VALUES ('%s', '%s', %d, 1, 0) ON DUPLICATE KEY UPDATE userid=VALUES(userid), name=VALUES(name), type=VALUES(type), unknown=VALUES(unknown)",
        cardid.c_str(), cardid.c_str(), userid);

    // vilslog::printf("MySQLDBAccess::update_NFCCard_user() cmd[%s]\n", cmd);

    sql_execute(cmd);
}

void MySQLDBAccess::update_MaingroupNFCCard_user(std::string cardid, int userid, std::string maingroups)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE MaingroupNFCCard SET userid=%d, type=1, unknown=0 WHERE cardid='%s' AND maingroupid in(%s) ", userid, cardid.c_str(), maingroups.c_str());

    // vilslog::printf("MySQLDBAccess::update_MaingroupNFCCard_user() cmd[%s]\n", cmd);

    sql_execute(cmd);

    std::list<std::string> * maingroupList = convert_string_to_list(maingroups, ',');
    for (std::list<std::string>::iterator maingroup = maingroupList->begin(); maingroup != maingroupList->end(); maingroup++)
    {
        sprintf(cmd, "INSERT INTO MaingroupNFCCard(cardid, name, userid, type, unknown, maingroupid) VALUES ('%s', '%s', %d, 1, 0, %d) ON DUPLICATE KEY UPDATE userid=VALUES(userid), name=VALUES(name), type=VALUES(type), unknown=VALUES(unknown)",
            cardid.c_str(), cardid.c_str(), userid, convert_string_to_int(*maingroup));

        // vilslog::printf("MySQLDBAccess::update_NFCCard_user() cmd[%s]\n", cmd);

        sql_execute(cmd);
    }

    maingroupList->clear();
    delete maingroupList;
}

void MySQLDBAccess::update_UWBCard_user(std::string macaddress, int userid)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "UPDATE NodeInfo SET userid=%d WHERE macaddress='%s' AND nodetype=%d ", userid, macaddress.c_str(), NODE_TYPE_TAG);

    // vilslog::printf("MySQLDBAccess::update_UWBCard_user() cmd[%s]\n", cmd);

    sql_execute(cmd);
}

std::list<RFIDCardStruct*> * MySQLDBAccess::get_RFIDCards()
{
    std::list<RFIDCardStruct*> * cardList = new std::list<RFIDCardStruct*>();
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM RFIDCard WHERE unknown=0 ORDER BY datetime ASC ");

    // vilslog::printf("MySQLDBAccess::get_RFIDCards() cmd[%s]\n", cmd);

    sql_executeQuery_new(cmd, get_RFIDCardList_callback, cardList);

    return cardList;
}

std::list<RFIDCardStruct*> * MySQLDBAccess::get_UnknownRFIDCards()
{
    std::list<RFIDCardStruct*> * cardList = new std::list<RFIDCardStruct*>();
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM RFIDCard WHERE unknown=1 ORDER BY datetime ASC ");

    // vilslog::printf("MySQLDBAccess::get_UnknownRFIDCards() cmd[%s]\n", cmd);

    sql_executeQuery_new(cmd, get_RFIDCardList_callback, cardList);

    return cardList;
}

std::list<NFCCardStruct*> * MySQLDBAccess::get_NFCCards()
{
    std::list<NFCCardStruct*> * cardList = new std::list<NFCCardStruct*>();
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM NFCCard WHERE unknown=0 ORDER BY datetime ASC ");

    sql_executeQuery_new(cmd, get_NFCCardList_callback, cardList);

    return cardList;
}

std::list<NFCCardStruct*> * MySQLDBAccess::get_MaingroupNFCCards()
{
    std::list<NFCCardStruct*> * cardList = new std::list<NFCCardStruct*>();
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM MaingroupNFCCard WHERE unknown=0 ORDER BY datetime ASC ");

    sql_executeQuery_new(cmd, get_NFCCardList_callback, cardList);

    return cardList;
}

std::list<NFCCardStruct*> * MySQLDBAccess::get_UnknownNFCCards()
{
    std::list<NFCCardStruct*> * cardList = new std::list<NFCCardStruct*>();
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM NFCCard WHERE unknown=1 ORDER BY datetime ASC ");

    sql_executeQuery_new(cmd, get_NFCCardList_callback, cardList);

    return cardList;
}

std::list<RFIDDeviceStruct*> * MySQLDBAccess::get_RFIDDevices()
{
    std::list<RFIDDeviceStruct*> * deviceList = new std::list<RFIDDeviceStruct*>();
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM RFIDDevice");

    sql_executeQuery_new(cmd, get_RFIDDeviceList_callback, deviceList);

    return deviceList;
}

std::list<NFCDeviceStruct*> * MySQLDBAccess::get_NFCDevices()
{
    std::list<NFCDeviceStruct*> * deviceList = new std::list<NFCDeviceStruct*>();
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM NFCDevice");

    sql_executeQuery_new(cmd, get_NFCDeviceList_callback, deviceList);

    return deviceList;
}

std::list<EDGEDeviceStruct*> * MySQLDBAccess::get_EDGEDevices()
{
    std::list<EDGEDeviceStruct*> * deviceList = new std::list<EDGEDeviceStruct*>();
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM EDGEDevice");

    sql_executeQuery_new(cmd, get_EDGEDeviceList_callback, deviceList);

    return deviceList;
}

void MySQLDBAccess::update_RFIDDevice(RFIDDeviceStruct * device)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    if ( device->deviceid > 0)
    {
        sprintf(cmd, "UPDATE RFIDDevice SET name='%s', type='%s', ip='%s', macaddress='%s', gwid='%s', LoRAMacaddress='%s', alive=%d, maingroupid=%d, verifykey='%s' WHERE deviceid=%d ",
                      device->name.c_str(), device->type.c_str(), device->ip.c_str(), device->macaddress.c_str(),
                      device->gwid.c_str(), device->LoRAMacaddress.c_str(), device->alive, device->maingroupid, device->verifykey.c_str(), device->deviceid);

        sql_execute(cmd);

        // vilslog::printf("MySQLDBAccess::update_RFIDDevice() cmd[%s]\n", cmd);

    }
    else
    {

        sprintf(cmd, "SELECT count(*) FROM RFIDDevice WHERE LoRAMacaddress='%s'", device->LoRAMacaddress.c_str());

        int count = 0;
        sql_executeQuery_new(cmd, get_count_callback, &count);

        if ( count > 0)
        {
            sprintf(cmd, "UPDATE RFIDDevice SET name='%s', type='%s', ip='%s', macaddress='%s', gwid='%s', alive=%d, maingroupid=%d, verifykey='%s' WHERE LoRAMacaddress='%s' ",
                          device->name.c_str(), device->type.c_str(), device->ip.c_str(), device->macaddress.c_str(),
                          device->gwid.c_str(), device->alive, device->maingroupid, device->verifykey.c_str(), device->LoRAMacaddress.c_str());

            sql_execute(cmd);

        }
        else
        {
            // create new device
            sprintf(cmd, "INSERT INTO RFIDDevice(name, type, ip, macaddress, gwid, LoRAMacaddress, alive, maingroupid, verifykey) VALUES ('%s','%s','%s','%s','%s','%s', %d, %d,'%s') ",
                          device->name.c_str(), device->type.c_str(), device->ip.c_str(), device->macaddress.c_str(),
                          device->gwid.c_str(), device->LoRAMacaddress.c_str(), device->alive, device->maingroupid, device->verifykey.c_str());

            sql_execute(cmd);
        }
        // vilslog::printf("MySQLDBAccess::update_RFIDDevice() cmd[%s]\n", cmd);
    }
}

void MySQLDBAccess::update_NFCDevice(NFCDeviceStruct * device)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    if ( device->deviceid > 0 )
    {
        sprintf(cmd, "UPDATE NFCDevice SET name='%s', type='%s', macaddress='%s', gwid='%s', LoRAMacaddress='%s', ssid='%s', wifimac='%s', version='%s', alive=%d, maingroupid=%d, verifykey='%s' WHERE deviceid=%d ",
                      device->name.c_str(), device->type.c_str(), device->macaddress.c_str(), device->gwid.c_str(),
                      device->LoRAMacaddress.c_str(), device->ssid.c_str(), device->wifimac.c_str(), device->version.c_str(), device->alive, device->maingroupid, device->verifykey.c_str(), device->deviceid);

        sql_execute(cmd);

        // vilslog::printf("MySQLDBAccess::update_NFCDevice() cmd[%s] ret[%d] \n", cmd, ret);

    }
    else
    {

        sprintf(cmd, "SELECT count(*) FROM NFCDevice WHERE LoRAMacaddress='%s'", device->LoRAMacaddress.c_str());

        int count = 0;
        sql_executeQuery_new(cmd, get_count_callback, &count);

        if ( count > 0)
        {
            sprintf(cmd, "UPDATE NFCDevice SET name='%s', type='%s', macaddress='%s', gwid='%s', ssid='%s', wifimac='%s', version='%s', alive=%d, maingroupid=%d, verifykey='%s'  WHERE LoRAMacaddress='%s' ",
                          device->name.c_str(), device->type.c_str(), device->macaddress.c_str(), device->gwid.c_str(),
                          device->ssid.c_str(), device->wifimac.c_str(), device->version.c_str(), device->alive, device->maingroupid, device->verifykey.c_str(), device->LoRAMacaddress.c_str());

            sql_execute(cmd);
        }
        else
        {
            // create new device
            sprintf(cmd, "INSERT INTO NFCDevice(name, type, macaddress, gwid, LoRAMacaddress, ssid, wifimac, version, alive, maingroupid, verifykey) VALUES ('%s','%s','%s','%s','%s','%s','%s','%s', %d, %d,'%s') ",
                          device->name.c_str(), device->type.c_str(), device->macaddress.c_str(), device->gwid.c_str(),
                          device->LoRAMacaddress.c_str(), device->ssid.c_str(), device->wifimac.c_str(), device->version.c_str(), device->alive, device->maingroupid, device->verifykey.c_str());

            sql_execute(cmd);
        }

        // vilslog::printf("MySQLDBAccess::update_NFCDevice() cmd[%s] ret[%d] \n", cmd, ret);
    }
}

void MySQLDBAccess::update_EDGEDevice(EDGEDeviceStruct * device)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    if ( device->deviceid > 0 )
    {
        sprintf(cmd, "UPDATE EDGEDevice SET name='%s', type='%s', macaddress='%s', ip='%s', LoRAMacaddress='%s', gwid='%s', version='%s', alive=%d, maingroupid=%d, verifykey='%s' WHERE deviceid=%d ",
                      device->name.c_str(), device->type.c_str(), device->macaddress.c_str(), device->ip.c_str(),
                      device->LoRAMacaddress.c_str(), device->gwid.c_str(), device->version.c_str(), device->alive,
                      device->maingroupid, device->verifykey.c_str(), device->deviceid);

        sql_execute(cmd);

        // vilslog::printf("MySQLDBAccess::update_EDGEDevice() cmd[%s]\n", cmd);

    }
    else
    {

        sprintf(cmd, "SELECT count(*) FROM EDGEDevice WHERE macaddress='%s'", device->macaddress.c_str());

        int count = 0;
        sql_executeQuery_new(cmd, get_count_callback, &count);

        if ( count > 0)
        {
            sprintf(cmd, "UPDATE EDGEDevice SET name='%s', type='%s', ip='%s', LoRAMacaddress='%s', gwid='%s', version='%s', alive=%d, maingroupid=%d, verifykey='%s' WHERE macaddress='%s' ",
                          device->name.c_str(), device->type.c_str(), device->ip.c_str(), device->LoRAMacaddress.c_str(), device->gwid.c_str(),
                          device->version.c_str(), device->alive, device->maingroupid, device->verifykey.c_str(), device->macaddress.c_str());

            sql_execute(cmd);
        }
        else
        {
            // create new device
            sprintf(cmd, "INSERT INTO EDGEDevice(name, type, macaddress, ip, LoRAMacaddress, gwid, version, alive, maingroupid, verifykey) VALUES ('%s','%s','%s','%s','%s','%s','%s', %d, %d,'%s') ",
                          device->name.c_str(), device->type.c_str(), device->macaddress.c_str(), device->ip.c_str(),
                          device->LoRAMacaddress.c_str(), device->gwid.c_str(), device->version.c_str(), device->alive, device->maingroupid, device->verifykey.c_str());

            sql_execute(cmd);
        }

        // vilslog::printf("MySQLDBAccess::update_NFCDevice() cmd[%s] count[%d] \n", cmd, count);
    }

}

std::string MySQLDBAccess::create_verifykey(std::string tablename)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::string key = random_string(8);

    bool bFound = false;
    while (!bFound)
    {
        int count = 0;

        // check if existed
        sprintf(cmd, "SELECT count(*) FROM %s WHERE verifykey='%s' ", tablename.c_str(), key.c_str());

        sql_executeQuery_new(cmd, get_count_callback, &count);
        if ( count == 0 )
        {
            bFound = true;
        }
        else
        {
            key = random_string(8);
        }
    }

    return key;
}

void MySQLDBAccess::update_CARDRollCallRecord(CARDRollCallRecordStruct * record)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::string rfiddeviceids = convert_listint_to_string(&record->rfiddeviceids);
    std::string nfclocators = convert_list_to_string(&record->nfclocators);

    sprintf(cmd, "INSERT INTO CARDRollCallRecord(rollcallid, rollcalleventid, cardid, userid, nodeid, rfiddeviceids, nfclocators, present, maingroups, datetime) \
                  VALUES (%d, %d, '%s', %d, '%s', '%s', '%s', %d, '%s', '%s') ON DUPLICATE KEY UPDATE userid=VALUES(userid), nodeid=VALUES(nodeid), \
                  rfiddeviceids=VALUES(rfiddeviceids), nfclocators=VALUES(nfclocators), present=VALUES(present), maingroups=VALUES(maingroups), datetime=VALUES(datetime)",
                  record->rollcallid, record->rollcalleventid, record->cardid.c_str(), record->userid, record->nodeid.c_str(),
                  rfiddeviceids.c_str(), nfclocators.c_str(), record->present, record->maingroups.c_str(), record->datetime.c_str());

    sql_execute(cmd);
}

void MySQLDBAccess::Transaction_write_CARDRollCallRecord(CARDRollCallRecordStruct * record)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::string rfiddeviceids = convert_listint_to_string(&record->rfiddeviceids);
    std::string nfclocators = convert_list_to_string(&record->nfclocators);

    sprintf(cmd, "INSERT INTO CARDRollCallRecord(rollcallid, rollcalleventid, cardid, userid, nodeid, rfiddeviceids, nfclocators, present, maingroups, datetime) \
                  VALUES (%d, %d, '%s', %d, '%s', '%s', '%s', %d, '%s', '%s') ON DUPLICATE KEY UPDATE userid=VALUES(userid), nodeid=VALUES(nodeid), \
                  rfiddeviceids=VALUES(rfiddeviceids), nfclocators=VALUES(nfclocators), present=VALUES(present), maingroups=VALUES(maingroups), datetime=VALUES(datetime)",
                  record->rollcallid, record->rollcalleventid, record->cardid.c_str(), record->userid, record->nodeid.c_str(),
                  rfiddeviceids.c_str(), nfclocators.c_str(), record->present, record->maingroups.c_str(), record->datetime.c_str());

    std::string trans = cmd;
    mMysqlTransactionQueue.add(trans);
}

void MySQLDBAccess::update_CARDRollCallEvent(CARDRollCallEventStruct * event)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO CARDRollCallEvent(eventid, rollcallid, rollcallname, StartDateTime, EndDateTime, totalcount, presentcount, maingroups) \
                  VALUES (%d, %d, '%s', '%s', '%s', %d, %d, '%s') ON DUPLICATE KEY UPDATE rollcallid=VALUES(rollcallid), rollcallname=VALUES(rollcallname), \
                  StartDateTime=VALUES(StartDateTime), EndDateTime=VALUES(EndDateTime), totalcount=VALUES(totalcount), presentcount=VALUES(presentcount), maingroups=VALUES(maingroups)",
                  event->eventid, event->rollcallid, event->rollcallname.c_str(), event->StartDateTime.c_str(), event->EndDateTime.c_str(),
                  event->totalcount, event->presentcount, event->maingroups.c_str());

    sql_execute(cmd);
}

void MySQLDBAccess::Transaction_write_CARDRollCallEvent(CARDRollCallEventStruct * event)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO CARDRollCallEvent(eventid, rollcallid, rollcallname, StartDateTime, EndDateTime, totalcount, presentcount, maingroups) \
                  VALUES (%d, %d, '%s', '%s', '%s', %d, %d, '%s') ON DUPLICATE KEY UPDATE rollcallid=VALUES(rollcallid), rollcallname=VALUES(rollcallname), \
                  StartDateTime=VALUES(StartDateTime), EndDateTime=VALUES(EndDateTime), totalcount=VALUES(totalcount), presentcount=VALUES(presentcount), maingroups=VALUES(maingroups)",
                  event->eventid, event->rollcallid, event->rollcallname.c_str(), event->StartDateTime.c_str(), event->EndDateTime.c_str(),
                  event->totalcount, event->presentcount, event->maingroups.c_str());

      std::string trans = cmd;
      mMysqlTransactionQueue.add(trans);
}

void MySQLDBAccess::update_CARDRollCallSubGroupEvent(CARDRollCallSubGroupEventStruct * event)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO CARDRollCallSubGroupEvent(eventid, rollcallid, subgroupid, rollcallname, StartDateTime, EndDateTime, totalcount, presentcount, maingroups) \
                  VALUES (%d, %d, %d, '%s', '%s', '%s', %d, %d, '%s') ON DUPLICATE KEY UPDATE rollcallname=VALUES(rollcallname), \
                  StartDateTime=VALUES(StartDateTime), EndDateTime=VALUES(EndDateTime), totalcount=VALUES(totalcount), presentcount=VALUES(presentcount), maingroups=VALUES(maingroups)",
                  event->eventid, event->rollcallid, event->subgroupid, event->rollcallname.c_str(), event->StartDateTime.c_str(), event->EndDateTime.c_str(),
                  event->totalcount, event->presentcount, event->maingroups.c_str());

    sql_execute(cmd);
}

void MySQLDBAccess::Transaction_write_CARDRollCallSubGroupEvent(CARDRollCallSubGroupEventStruct * event)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO CARDRollCallSubGroupEvent(eventid, rollcallid, subgroupid, rollcallname, StartDateTime, EndDateTime, totalcount, presentcount, maingroups) \
                  VALUES (%d, %d, %d, '%s', '%s', '%s', %d, %d, '%s') ON DUPLICATE KEY UPDATE rollcallname=VALUES(rollcallname), \
                  StartDateTime=VALUES(StartDateTime), EndDateTime=VALUES(EndDateTime), totalcount=VALUES(totalcount), presentcount=VALUES(presentcount), maingroups=VALUES(maingroups)",
                  event->eventid, event->rollcallid, event->subgroupid, event->rollcallname.c_str(), event->StartDateTime.c_str(), event->EndDateTime.c_str(),
                  event->totalcount, event->presentcount, event->maingroups.c_str());

      std::string trans = cmd;
      mMysqlTransactionQueue.add(trans);
}

CARDRollCallEventStruct * MySQLDBAccess::get_CARDRollCallEvent(int rollcallid, int eventid)
{
    CARDRollCallEventStruct * rollcallEvent = new CARDRollCallEventStruct();
    rollcallEvent->eventid = -1;

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM CARDRollCallEvent WHERE eventid=%d AND rollcallid=%d ", eventid, rollcallid);

    sql_executeQuery_new(cmd, get_CARDRollCallEvent_callback, rollcallEvent);

    return rollcallEvent;
}

CARDRollCallEventStruct * MySQLDBAccess::get_CARDRollCallEvent(int rollcallid, std::string StartDateTime, std::string EndDateTime, std::string maingroups, bool bCreateNew)
{
    CARDRollCallEventStruct * rollcallEvent = new CARDRollCallEventStruct();
    rollcallEvent->eventid = -1;

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM CARDRollCallEvent WHERE rollcallid=%d AND StartDateTime='%s' AND EndDateTime='%s' ",
        rollcallid, StartDateTime.c_str(), EndDateTime.c_str());

    sql_executeQuery_new(cmd, get_CARDRollCallEvent_callback, rollcallEvent);

    if (bCreateNew && rollcallEvent->eventid == -1)
    {
        // create new event
        sprintf(cmd, "INSERT INTO CARDRollCallEvent (rollcallid, rollcallname, StartDateTime, EndDateTime, totalcount, presentcount, maingroups) \
                     VALUES (%d, '', '%s', '%s', 0, 0, '%s') ",
            rollcallid, StartDateTime.c_str(), EndDateTime.c_str(), maingroups.c_str());

        int eventid = sql_execute_get_insert_id(cmd);

        rollcallEvent->eventid = eventid;
        rollcallEvent->rollcallid = rollcallid;
        rollcallEvent->rollcallname = "";
        rollcallEvent->StartDateTime = StartDateTime;
        rollcallEvent->EndDateTime = EndDateTime;
        rollcallEvent->totalcount = 0;
        rollcallEvent->presentcount = 0;
        rollcallEvent->maingroups = maingroups;
    }
    return rollcallEvent;
}

std::list<CARDRollCallRecordStruct*> * MySQLDBAccess::get_CARDRollCallRecords(int rollcallid, int rollcalleventid)
{
    std::list<CARDRollCallRecordStruct*> * recordList = new std::list<CARDRollCallRecordStruct*>();

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM CARDRollCallRecord WHERE rollcallid=%d AND rollcalleventid=%d ", rollcallid, rollcalleventid);

    sql_executeQuery_new(cmd, get_CARDRollCallRecordStructList_callback, recordList);

    return recordList;
}

void MySQLDBAccess::update_CARDScanRecord(CARDScanRecordStruct * record)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO CARDScanRecord(cardtype, cardid, LoRAMacaddress, ssid, wifimac) VALUES (%d, '%s', '%s', '%s', '%s')",
                  record->cardtype, record->cardid.c_str(), record->LoRAMacaddress.c_str(), record->ssid.c_str(), record->wifimac.c_str());

    sql_execute(cmd);
}

std::list<CARDScanRecordStruct*> * MySQLDBAccess::get_CARDScanRecord(std::string StartDateTime, std::string EndDateTime)
{
    std::list<CARDScanRecordStruct*> * recordList = new std::list<CARDScanRecordStruct*>();

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM CARDScanRecord WHERE datetime between '%s' AND '%s' ", StartDateTime.c_str(), EndDateTime.c_str());

    sql_executeQuery_new(cmd, get_CARDScanRecordStructList_callback, recordList);

    return recordList;
}

void MySQLDBAccess::create_maplayer(MapLayerStruct * maplayer)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO maplayer(maingroupid, layer_name, layer_description, WGS48OriginX, WGS48OriginY, MAP_North, \
                  BuildingID, BuildingFloor, POSX, POSY, SCALE, COEFF, FILE, MAPWIDTH, MAPHEIGHT) \
                  VALUES (%d', %s', '%s', '%s', '%s', '%s', %d, %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s')",
                  maplayer->maingroupid, maplayer->layer_name.c_str(), maplayer->layer_description.c_str(), maplayer->WGS48OriginX.c_str(),
                  maplayer->WGS48OriginY.c_str(), maplayer->MAPNorth.c_str(), maplayer->BuildingID, maplayer->BuildingFloor,
                  maplayer->POSX.c_str(), maplayer->POSY.c_str(), maplayer->SCALE.c_str(), maplayer->COEFF.c_str(),
                  maplayer->FILE.c_str(),maplayer->MAPWIDTH.c_str(), maplayer->MAPHEIGHT.c_str() );

    sql_execute(cmd);
}

void MySQLDBAccess::update_maplayer(MapLayerStruct maplayer)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO maplayer(layerid, maingroupid, layer_name, layer_description, WGS48OriginX, WGS48OriginY, MAP_North, \
                  BuildingID, BuildingFloor, POSX, POSY, SCALE, COEFF, FILE, MAPWIDTH, MAPHEIGHT) \
                    VALUES (%d, %d, '%s', '%s', '%s', '%s', '%s', %d, %d, '%s', '%s', '%s', '%s', '%s', '%s', '%s') \
                    ON DUPLICATE KEY UPDATE maingroupid=VALUES(maingroupid), layer_name=VALUES(layer_name), layer_description=VALUES(layer_description), WGS48OriginX=VALUES(WGS48OriginX), \
                    WGS48OriginY=VALUES(WGS48OriginY), MAP_North=VALUES(MAP_North), BuildingID=VALUES(BuildingID), BuildingFloor=VALUES(BuildingFloor), \
                    POSX=VALUES(POSX), POSY=VALUES(POSY), SCALE=VALUES(SCALE), COEFF=VALUES(COEFF), FILE=VALUES(FILE), MAPWIDTH=VALUES(MAPWIDTH), MAPHEIGHT=VALUES(MAPHEIGHT)",
                    maplayer.layerid, maplayer.maingroupid, maplayer.layer_name.c_str(), maplayer.layer_description.c_str(), maplayer.WGS48OriginX.c_str(),
                    maplayer.WGS48OriginY.c_str(), maplayer.MAPNorth.c_str(), maplayer.BuildingID, maplayer.BuildingFloor,
                    maplayer.POSX.c_str(), maplayer.POSY.c_str(), maplayer.SCALE.c_str(), maplayer.COEFF.c_str(),
                    maplayer.FILE.c_str(),maplayer.MAPWIDTH.c_str(), maplayer.MAPHEIGHT.c_str());

    //vilslog::printf("MySQLDBAccess::update_maplayer() cmd[%s]\n", cmd);

    sql_execute(cmd);
}

int MySQLDBAccess::get_first_maplayerid()
{
    int layerid = 1;

    char cmd[4096];
    memset(cmd, 0, 4096);

    // sprintf(cmd, "SELECT layerid FROM maplayer ORDER BY datetime ASC limit 1 ");
    sprintf(cmd, "SELECT layerid FROM maplayer WHERE maingroupid=1 ORDER BY datetime ASC limit 1 ");

    sql_executeQuery_new(cmd, get_maplayerid_callback, &layerid);

    return layerid;
}

std::list<MapLayerStruct*> * MySQLDBAccess::read_maplayers()
{
    std::list<MapLayerStruct*> * maplayerList = new std::list<MapLayerStruct*>();
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM maplayer");

    sql_executeQuery_new(cmd, get_maplayerList_callback, maplayerList);

    return maplayerList;
}

std::list<MapLayerAreaStruct*> * MySQLDBAccess::read_maplayerarea_not_wall()
{
    std::list<MapLayerAreaStruct*> * maplayerareaList = new std::list<MapLayerAreaStruct*>();
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM maplayerarea WHERE type != 999");

    sql_executeQuery_new(cmd, get_maplayerareaList_callback, maplayerareaList);

    return maplayerareaList;
}

std::list<MapLayerAreaStruct*> * MySQLDBAccess::read_maplayerarea_type(int type)
{
    std::list<MapLayerAreaStruct*> * maplayerareaList = new std::list<MapLayerAreaStruct*>();
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM maplayerarea WHERE type=%d ", type);

    sql_executeQuery_new(cmd, get_maplayerareaList_callback, maplayerareaList);

    return maplayerareaList;
}

std::string MySQLDBAccess::fetchAreaName(std::string areaid)
{
    std::string AreaName = areaid;

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM maplayerarea WHERE areaid='%s'", areaid.c_str());

    sql_executeQuery_new(cmd, get_maplayerareaname_callback, &AreaName);

    return AreaName;
}

MapLayerAreaStruct * MySQLDBAccess::getAreaStruct(std::string areaid)
{
    MapLayerAreaStruct * area = new MapLayerAreaStruct();

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM maplayerarea WHERE areaid='%s'", areaid.c_str());

    sql_executeQuery_new(cmd, get_maplayerarea_callback, area);

    return area;
}

std::string MySQLDBAccess::fetchMaplayerName(int layerid)
{
    std::string MaplayerName = "";

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT layer_name FROM maplayer WHERE layerid=%d", layerid);

    sql_executeQuery_new(cmd, get_maplayername_callback, &MaplayerName);

    return MaplayerName;
}

MapLayerStruct * MySQLDBAccess::getMapLayerStruct(int layerid)
{
    MapLayerStruct * maplayer = new MapLayerStruct();

    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM maplayer WHERE layerid=%d", layerid);

    sql_executeQuery_new(cmd, get_maplayer_callback, maplayer);

    return maplayer;
}

void MySQLDBAccess::create_maplayerarea(MapLayerAreaStruct * maplayerarea)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO maplayerarea(layerid, areaid, areaname, geojson, type) \
                  VALUES (%d, '%s', '%s', '%s', %d)",
                  maplayerarea->layerid, maplayerarea->areaid.c_str(), maplayerarea->areaname.c_str(),
                  maplayerarea->geojson.c_str(), maplayerarea->type);

    sql_execute(cmd);
}

void MySQLDBAccess::update_maplayerarea(MapLayerAreaStruct maplayerarea)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO maplayerarea(layerid, areaid, areaname, geojson, type) \
                    VALUES (%d, '%s', '%s', '%s', %d) \
                    ON DUPLICATE KEY UPDATE areaname=VALUES(areaname), geojson=VALUES(geojson), type=VALUES(type)",
                    maplayerarea.layerid, maplayerarea.areaid.c_str(), maplayerarea.areaname.c_str(), maplayerarea.geojson.c_str(), maplayerarea.type);

    sql_execute(cmd);
}

void MySQLDBAccess::update_maplayerareashape(MapLayerAreaStruct maplayerarea)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO maplayerarea(layerid, areaid, areaname, geojson) \
                    VALUES (%d, '%s', '%s', '%s') \
                    ON DUPLICATE KEY UPDATE areaname=VALUES(areaname), geojson=VALUES(geojson)",
                    maplayerarea.layerid, maplayerarea.areaid.c_str(), maplayerarea.areaname.c_str(), maplayerarea.geojson.c_str());

    //vilslog::printf("MySQLDBAccess::update_maplayerareashape() cmd[%s]\n", cmd);

    sql_execute(cmd);
}

void MySQLDBAccess::delete_maplayerarea(MapLayerAreaStruct maplayerarea)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "DELETE FROM maplayerarea WHERE layerid=%d AND areaid='%s' ", maplayerarea.layerid, maplayerarea.areaid.c_str());

    sql_execute(cmd);
}

void MySQLDBAccess::write_bodyinfo_Weight(const char * nodeid, std::string Weight)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO BodyInfo(nodeid, weight) VALUES ('%s', '%s')", nodeid, Weight.c_str());

    sql_execute(cmd);
}

void MySQLDBAccess::write_bodyinfo_HeartBeat(const char * nodeid, std::string HeartBeat)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO BodyInfo(nodeid, heartbeat) VALUES ('%s', '%s')", nodeid, HeartBeat.c_str());

    sql_execute(cmd);
}

void MySQLDBAccess::write_bodyinfo_all(std::string accountid, std::string recorderid, std::string weight, std::string heartbeat, std::string bloodsugar, std::string systolicbloodpressure, std::string diastolicbloodpressure, std::string bodytemperature, std::string bodyfat, std::string bloodoxygen, std::string datetimet)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO BodyInfo(nodeid, accountid, recorderid, weight, heartbeat, bloodsugar, systolicbloodpressure, diastolicbloodpressure, bodytemperature, bodyfat, bloodoxygen, datadatetime) \
                  VALUES ('%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s')",
                  accountid.c_str(), accountid.c_str(), recorderid.c_str(), weight.c_str(), heartbeat.c_str(), bloodsugar.c_str(), systolicbloodpressure.c_str(), diastolicbloodpressure.c_str(), bodytemperature.c_str(), bodyfat.c_str(), bloodoxygen.c_str(), datetimet.c_str());

    sql_execute(cmd);
}

std::list<BodyInfoStruct*> * MySQLDBAccess::get_bodyinfo_Weight(const char * nodeid, int count)
{
    std::list<BodyInfoStruct*> * bodyinfoList = new std::list<BodyInfoStruct*>();
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM BodyInfo WHERE nodeid='%s' AND weight IS NOT NULL ORDER BY datetime DESC LIMIT %d ", nodeid, count);

    sql_executeQuery_new(cmd, get_bodyinfoWeightList_callback, bodyinfoList);

    return bodyinfoList;
}

std::list<BodyInfoStruct*> * MySQLDBAccess::get_bodyinfo_HeartBeat(const char * nodeid, int count)
{
    std::list<BodyInfoStruct*> * bodyinfoList = new std::list<BodyInfoStruct*>();
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM BodyInfo WHERE nodeid='%s' AND heartbeat IS NOT NULL ORDER BY datetime DESC LIMIT %d ", nodeid, count);

    sql_executeQuery_new(cmd, get_bodyinfoHeartBeatList_callback, bodyinfoList);

    return bodyinfoList;
}

std::list<LoRaGatewayInfoStruct*> * MySQLDBAccess::read_LoRaGateway()
{
    std::list<LoRaGatewayInfoStruct*> * gatewayList = new std::list<LoRaGatewayInfoStruct*>();
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT * FROM LoRaGatewayInfo");

    sql_executeQuery_new(cmd, get_LoRaGatewayList_callback, gatewayList);

    return gatewayList;
}

void MySQLDBAccess::convert_utf8_to_utf8mb4(const char * tablename)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "ALTER TABLE %s DEFAULT CHARACTER SET utf8mb4 COLLATE = utf8mb4_unicode_ci;", tablename);
    sql_execute(cmd);
}

bool MySQLDBAccess::tableExists(const char * tableName)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "SELECT count(*) FROM information_schema.tables WHERE table_schema='%s' AND table_name='%s' LIMIT 1 ", mSQLDBname.c_str(), tableName);

    int count = 0;
    sql_executeQuery_new(cmd, get_count_callback, &count);

    //printf("MySQLDBAccess::tableExists() count[%d]\n", count);

    if (count == 0)
        return false;
    else
        return true;

    return false;
}

bool MySQLDBAccess::createNonExistColumn(const char * tableName, const char * columnName, const char * colDefinition)
{
    bool bExist = columnExists(tableName, columnName);
    if (!bExist)
        create_column(tableName, columnName, colDefinition);
    return bExist;
}

bool MySQLDBAccess::columnExists(const char * tableName, const char * columnName)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

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

int MySQLDBAccess::create_column(const char * tableName, const char * columnName, const char * colDefinition)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "ALTER TABLE %s ADD %s %s", tableName, columnName, colDefinition);
    int res = sql_execute(cmd);
    return res;
}

bool MySQLDBAccess::update_to_alert_cache(const char * macaddress, int count, const char * FromDatetime, const char * ToDatetime, const char * min_time, const char * max_time)
{
    bool bFound = false;

    std::string datetime_str = FromDatetime;
    std::string date_str = datetime_str.substr(0, 10);

    // vilslog::printf("MySQLDBAccess::update_to_alert_cache() macaddress[%s] count[%d] FromDatetime[%s] ToDatetime[%s] min_time[%s] max_time[%s]\n",
    //     macaddress, count, FromDatetime, ToDatetime, min_time, max_time);

    for (std::list<AnalyzeDayInfoStruct *>::iterator dayInfoCache = mAlertAnalyzeDayInfoStructList.begin(); dayInfoCache != mAlertAnalyzeDayInfoStructList.end(); dayInfoCache++)
    {
        if ( (*dayInfoCache)->nodeid.compare(macaddress) == 0 && (*dayInfoCache)->day.compare(date_str) == 0)
        {
            // check datetime
            if ( (*dayInfoCache)->max_time.compare(max_time) < 0 )
            {
                //vilslog::printf("MySQLDBAccess::update_to_alert_cache() max_time[%s] -> [%s] count[%d]\n",
                //    (*dayInfoCache)->max_time.c_str(), ToDatetime, (*dayInfoCache)->count);
                (*dayInfoCache)->max_time = max_time;
            }
            else
            if ( (*dayInfoCache)->max_time.compare(max_time) == 0 )
            {
                //vilslog::printf("TMySQLDBAccess::update_to_alert_cache() max_time[%s] same [%s] count[%d]\n",
                //    (*dayInfoCache)->max_time.c_str(), ToDatetime, (*dayInfoCache)->count);
            }

            if ( (*dayInfoCache)->min_time.compare(min_time) > 0 )
            {
                //vilslog::printf("MySQLDBAccess::update_to_alert_cache() min_time[%s] -> [%s] count[%d]\n",
                //    (*dayInfoCache)->min_time.c_str(), FromDatetime, (*dayInfoCache)->count);
                (*dayInfoCache)->min_time = min_time;
            }
            else
            if ( (*dayInfoCache)->min_time.compare(min_time) == 0 )
            {
                //vilslog::printf("MySQLDBAccess::update_to_alert_cache() min_time[%s] same [%s] count[%d]\n",
                //    (*dayInfoCache)->min_time.c_str(), FromDatetime, (*dayInfoCache)->count);
            }

            // if ( (*dayInfoCache)->cache_max_time.compare(max_time) < 0 )
            // {
            //     (*dayInfoCache)->cache_max_time = max_time;
            // }
            //
            // if ( (*dayInfoCache)->cache_min_time.compare(min_time) > 0 )
            // {
            //     (*dayInfoCache)->cache_min_time = min_time;
            // }

            if ( (*dayInfoCache)->cache_max_time.compare(ToDatetime) < 0 )
            {
                (*dayInfoCache)->cache_max_time = ToDatetime;
            }

            if ( (*dayInfoCache)->cache_min_time.compare(FromDatetime) > 0 )
            {
                (*dayInfoCache)->cache_min_time = FromDatetime;
            }

            (*dayInfoCache)->count += count;

            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        AnalyzeDayInfoStruct * dayInfoCache = createAnalyzeDayInfoStruct();
        dayInfoCache->count = count;
        dayInfoCache->nodeid = macaddress;
        dayInfoCache->max_time = max_time;
        dayInfoCache->min_time = min_time;
        dayInfoCache->day = date_str;
        // dayInfoCache->cache_max_time = max_time;
        // dayInfoCache->cache_min_time = min_time;
        dayInfoCache->cache_max_time = ToDatetime;
        dayInfoCache->cache_min_time = FromDatetime;

        mAlertAnalyzeDayInfoStructList.push_back(dayInfoCache);
    }

    return true;
}

bool MySQLDBAccess::update_to_AreaInOut_cache(const char * nodeid, int count, const char * FromDatetime, const char * ToDatetime, const char * min_time, const char * max_time, const char * area_ids, const char * area_names)
{
    bool bFound = false;

    std::string datetime_str = FromDatetime;
    std::string date_str = datetime_str.substr(0, 10);

    // if ( strcmp(nodeid, "Tag1181")  == 0)
    // {
    //     vilslog::printf("MySQLDBAccess::update_to_AreaInOut_cache() nodeid[%s] count[%d] FromDatetime[%s] ToDatetime[%s]\n",
    //         nodeid, count, FromDatetime, ToDatetime);
    // }

    for (std::list<AnalyzeDayInfoStruct *>::iterator dayInfoCache = mAreaInOutAnalyzeDayInfoStructList.begin(); dayInfoCache != mAreaInOutAnalyzeDayInfoStructList.end(); dayInfoCache++)
    {
        if ( (*dayInfoCache)->nodeid.compare(nodeid) == 0 && (*dayInfoCache)->day.compare(date_str) == 0)
        {
            // check datetime
            if ( (*dayInfoCache)->max_time.compare(max_time) < 0 )
            {
                // if ( strcmp(nodeid, "Tag1181")  == 0)
                // {
                //     vilslog::printf("MySQLDBAccess::update_to_AreaInOut_cache() [%s] max_time[%s] -> [%s] count[%d]\n",
                //        nodeid, (*dayInfoCache)->max_time.c_str(), ToDatetime, (*dayInfoCache)->count);
                // }
                (*dayInfoCache)->max_time = max_time;
            }
            else
            if ( (*dayInfoCache)->max_time.compare(max_time) == 0 )
            {
                // if ( strcmp(nodeid, "Tag1181")  == 0)
                // {
                //     vilslog::printf("TMySQLDBAccess::update_to_AreaInOut_cache() max_time[%s] same [%s] count[%d]\n",
                //         (*dayInfoCache)->max_time.c_str(), ToDatetime, (*dayInfoCache)->count);
                // }
            }

            if ( (*dayInfoCache)->min_time.compare(min_time) > 0 )
            {
                // if ( strcmp(nodeid, "Tag1181")  == 0)
                // {
                //     vilslog::printf("MySQLDBAccess::update_to_AreaInOut_cache() [%s] min_time[%s] -> [%s] count[%d]\n",
                //         nodeid, (*dayInfoCache)->min_time.c_str(), FromDatetime, (*dayInfoCache)->count);
                // }

                (*dayInfoCache)->min_time = min_time;
            }
            else
            if ( (*dayInfoCache)->min_time.compare(min_time) == 0 )
            {
                // if ( strcmp(nodeid, "Tag1181")  == 0)
                // {
                //     vilslog::printf("MySQLDBAccess::update_to_AreaInOut_cache() min_time[%s] same [%s] count[%d]\n",
                //         (*dayInfoCache)->min_time.c_str(), FromDatetime, (*dayInfoCache)->count);
                // }
            }

            // if ( (*dayInfoCache)->cache_max_time.compare(max_time) < 0 )
            //     (*dayInfoCache)->cache_max_time = max_time;
            //
            // if ( (*dayInfoCache)->cache_min_time.compare(min_time) > 0 )
            //     (*dayInfoCache)->cache_min_time = min_time;

            if ( (*dayInfoCache)->cache_max_time.compare(ToDatetime) < 0 )
                (*dayInfoCache)->cache_max_time = ToDatetime;

            if ( (*dayInfoCache)->cache_min_time.compare(FromDatetime) > 0 )
                (*dayInfoCache)->cache_min_time = FromDatetime;


            (*dayInfoCache)->count += count;

            //
            // check difference areaid
            //
            std::vector<std::string> AreaIdVector = convert_string_to_vector(area_ids);
            std::vector<std::string> AreaNameVector = convert_string_to_vector(area_names);

            std::vector<std::string> AreaIdSingleVector = convert_string_to_vector((*dayInfoCache)->area_ids);
            std::vector<std::string> AreaNameSingleVector = convert_string_to_vector((*dayInfoCache)->area_names);

            for (unsigned int i=0; i< AreaIdVector.size(); i++)
            {
                std::string AreaId = AreaIdVector[i];
                std::string AreaName = AreaNameVector[i];

                // find exited
                bool bFoundSameID = false;
                for (unsigned int j=0; j< AreaIdSingleVector.size(); j++)
                {
                    if ( AreaId.compare(AreaIdSingleVector[j]) == 0 )
                    {
                        bFoundSameID = true;
                        break;
                    }
                }
                if (!bFoundSameID)
                {
                    AreaIdSingleVector.push_back(AreaId);
                    AreaNameSingleVector.push_back(AreaName);

                    // if ( strcmp(nodeid, "Tag1181")  == 0)
                    // {
                    //     vilslog::printf("MySQLDBAccess::update_to_AreaInOut_cache() new area [%s][%s][%s]\n", nodeid, AreaId.c_str(), AreaName.c_str());
                    // }
                }
            }

            (*dayInfoCache)->area_ids = convertVectorToString(&AreaIdSingleVector);
            (*dayInfoCache)->area_names = convertVectorToString(&AreaNameSingleVector);

            AreaIdVector.clear();
            AreaNameVector.clear();
            AreaIdSingleVector.clear();
            AreaNameSingleVector.clear();

            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        AnalyzeDayInfoStruct * dayInfoCache = createAnalyzeDayInfoStruct();
        dayInfoCache->count = count;
        dayInfoCache->nodeid = nodeid;
        dayInfoCache->max_time = max_time;
        dayInfoCache->min_time = min_time;
        dayInfoCache->day = date_str;
        dayInfoCache->area_ids = area_ids;
        dayInfoCache->area_names = area_names;
        // dayInfoCache->cache_max_time = max_time;
        // dayInfoCache->cache_min_time = min_time;
        dayInfoCache->cache_max_time = ToDatetime;
        dayInfoCache->cache_min_time = FromDatetime;

        // if ( strcmp(nodeid, "Tag1181")  == 0)
        // {
        //     vilslog::printf("MySQLDBAccess::update_to_AreaInOut_cache() new record nodeid[%s] count[%d] FromDatetime[%s] ToDatetime[%s]\n",
        //         nodeid, count, FromDatetime, ToDatetime);
        // }

        mAreaInOutAnalyzeDayInfoStructList.push_back(dayInfoCache);
    }

    return true;
}

bool MySQLDBAccess::update_to_GPS_cache(int nodetype, const char * nodeid, int count, const char * FromDatetime, const char * ToDatetime, const char * min_time, const char * max_time)
{
    bool bFound = false;

    std::string datetime_str = FromDatetime;
    std::string date_str = datetime_str.substr(0, 10);

    for (std::list<AnalyzeDayInfoStruct *>::iterator dayInfoCache = mGPSAnalyzeDayInfoStructList.begin(); dayInfoCache != mGPSAnalyzeDayInfoStructList.end(); dayInfoCache++)
    {
        if ( ((*dayInfoCache)->nodeid.compare(nodeid) == 0) && ((*dayInfoCache)->day.compare(date_str) == 0) && ( (*dayInfoCache)->nodetype == nodetype))
        {
            if ( (*dayInfoCache)->count == 0 )
            {
                // first data comming
                (*dayInfoCache)->max_time = max_time;
                (*dayInfoCache)->min_time = min_time;
            }

            // check datetime
            if ( (*dayInfoCache)->max_time.compare(max_time) < 0 )
            {
                (*dayInfoCache)->max_time = max_time;
            }

            if ( (*dayInfoCache)->min_time.compare(min_time) > 0 )
            {
                (*dayInfoCache)->min_time = min_time;
            }

            // if ( (*dayInfoCache)->cache_max_time.compare(max_time) < 0 )
            //     (*dayInfoCache)->cache_max_time = max_time;
            //
            // if ( (*dayInfoCache)->cache_min_time.compare(min_time) > 0 )
            //     (*dayInfoCache)->cache_min_time = min_time;

            if ( (*dayInfoCache)->cache_max_time.compare(ToDatetime) < 0 )
                (*dayInfoCache)->cache_max_time = ToDatetime;

            if ( (*dayInfoCache)->cache_min_time.compare(FromDatetime) > 0 )
                (*dayInfoCache)->cache_min_time = FromDatetime;

            (*dayInfoCache)->count += count;

            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        AnalyzeDayInfoStruct * dayInfoCache = createAnalyzeDayInfoStruct();
        dayInfoCache->count = count;
        dayInfoCache->nodetype = nodetype;
        dayInfoCache->nodeid = nodeid;
        dayInfoCache->max_time = max_time;
        dayInfoCache->min_time = min_time;
        dayInfoCache->day = date_str;
        dayInfoCache->cache_max_time = ToDatetime;
        dayInfoCache->cache_min_time = FromDatetime;

        mGPSAnalyzeDayInfoStructList.push_back(dayInfoCache);
    }

    return true;
}

bool MySQLDBAccess::add_canned_message(int dbid, int type, std::string menutitle, std::string title, std::string message)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    sprintf(cmd, "INSERT INTO CannedMessage(dbId, type, menutitle, title, message) VALUES (%d, %d, '%s', '%s', '%s') \
                    ON DUPLICATE KEY UPDATE type=VALUES(type), menutitle=VALUES(menutitle), title=VALUES(title), message=VALUES(message)",
                    dbid, type, menutitle.c_str(), title.c_str(), message.c_str());

    sql_execute(cmd);
    return true;
}

bool MySQLDBAccess::add_APIKey(int maingroupid, int subgroupid, std::string key, std::string description)
{
    char cmd[4096];
    memset(cmd, 0, 4096);

    std::string empty("");

    sprintf(cmd, "INSERT INTO APIKey(apikey, type, descrption, enable, maingroupid, subgroupid, enableTime, expireTime) VALUES ('%s','%s','%s',%d,%d,%d,'%s','%s') \
                    ON DUPLICATE KEY UPDATE type=VALUES(type), descrption=VALUES(descrption), enable=VALUES(enable), maingroupid=VALUES(maingroupid), subgroupid=VALUES(subgroupid), enableTime=VALUES(enableTime), expireTime=VALUES(expireTime)",
                    key.c_str(), empty.c_str(), description.c_str(), 1, maingroupid, subgroupid, empty.c_str(), empty.c_str());

    sql_execute(cmd);
    return true;
}

std::string MySQLDBAccess::getPrettyJSON(std::string eventjson)
{
    std::string prettyjson = "";

    //printf("MySQLDBAccess::getPrettyJSON() eventjson[%s]\n", eventjson.c_str());

    Json::Value result;
    bool parsingSuccessful = parse_JSON(eventjson, result);
    if (!parsingSuccessful)
        return prettyjson;

    if (result.isMember("type"))
    {
        std::string type = result["type"].asCString();
        if ( type.compare("inspectionpoint") == 0)
        {
            return getInspectionPointPrettyJSON(eventjson);
        }
        else
        if ( type.compare("inspectionpointtravel") == 0)
        {
            return getInspectionPointTravelPrettyJSON(eventjson);
        }
        else
        if ( type.compare("inspectionGroup") == 0)
        {
            return getInspectionGroupPrettyJSON(eventjson);
        }
    }

    std::string nodeid = result["nodeid"].asCString();
    std::string node_name = fetchNodeName(nodeid);

    std::string action_name = get_action_name(result["action"].asString());

    std::string areaid = result["areaid"].asCString();
    std::string area_name = areaid;
    if (areaid.length() > 0)
    {
        area_name = fetchAreaName(areaid);
        //printf("MySQLDBAccess::getPrettyJSON() areaid[%s] area_name[%s] \n", areaid.c_str(), area_name.c_str());
    }

    std::string time_name = get_time_name(result["issuetime"].asString());
    //printf("MySQLDBAccess::getPrettyJSON() issuetime[%s] time_name[%s] \n", result["issuetime"].asCString(), time_name.c_str());

    std::string locator_name = "";
    if (result.isMember("locatorid"))
    {
        std::string locatorid = result["locatorid"].asCString();
        locator_name = locatorid;
        if (locatorid.length() > 0)
        {
            locator_name = fetchLocatorName(locatorid);
            //printf("MySQLDBAccess::getPrettyJSON() locatorid[%s] locator_name[%s] \n", locatorid.c_str(), locator_name.c_str());
        }
    }

    std::ostringstream sendstream;

    sendstream<<"{";
    sendstream<<"\"nodename\":\""<<node_name<<"\",";
    sendstream<<"\"actionname\":\""<<action_name<<"\",";
    sendstream<<"\"areaname\":\""<<area_name<<"\",";
    sendstream<<"\"timename\":\""<<time_name<<"\",";
    sendstream<<"\"locatorname\":\""<<locator_name<<"\"";
    sendstream<<"}";

    prettyjson = sendstream.str();

    //printf("MySQLDBAccess::getPrettyJSON() prettyjson[%s]\n", prettyjson.c_str());
    return prettyjson;
}

std::string MySQLDBAccess::getInspectionPointPrettyJSON(std::string eventjson)
{
    std::string prettyjson = "";

    Json::Value result;
    bool parsingSuccessful = parse_JSON(eventjson, result);
    if (!parsingSuccessful)
        return prettyjson;

    // Inspector name
    std::string tag_nodeid = result["Inspector"].asCString();
    std::string tag_node_name = fetchNodeName(tag_nodeid);

    std::string action_name = get_action_name(result["action"].asString());

    // anchor action name
    std::string anchor_nodeid = result["NodeId"].asCString();
    std::string anchor_node_name = fetchNodeName(anchor_nodeid);

    std::ostringstream sendstream;

    sendstream<<"{";
    sendstream<<"\"type\":\"inspectionpoint\",";
    sendstream<<"\"tagname\":\""<<tag_node_name<<"\",";
    sendstream<<"\"actionname\":\""<<action_name<<"\",";
    sendstream<<"\"inspectionpointname\":\""<<anchor_node_name<<"\",";
    sendstream<<"\"locatorname\":\"\"";
    sendstream<<"}";

    prettyjson = sendstream.str();

    return prettyjson;
}

std::string MySQLDBAccess::getInspectionPointTravelPrettyJSON(std::string eventjson)
{
    std::string prettyjson = "";

    Json::Value result;
    bool parsingSuccessful = parse_JSON(eventjson, result);
    if (!parsingSuccessful)
        return prettyjson;

    // Inspector name
    std::string tag_nodeid = result["Inspector"].asCString();
    std::string tag_node_name = fetchNodeName(tag_nodeid);

    std::string action_name = get_action_name(result["action"].asString());

    // start anchor action name
    std::string start_anchor_nodeid = "";
    if ( result.isMember("StartNodeId") )
        start_anchor_nodeid = result["StartNodeId"].asCString();
    if ( result.isMember("AnchorNodeId") )
        start_anchor_nodeid = result["AnchorNodeId"].asCString();
    std::string start_anchor_node_name = fetchNodeName(start_anchor_nodeid);

    // end anchor action name
    std::string end_anchor_nodeid = "";
    if ( result.isMember("EndNodeId") )
        start_anchor_nodeid = result["EndNodeId"].asCString();
    if ( result.isMember("AnchorNodeId") )
        start_anchor_nodeid = result["AnchorNodeId"].asCString();
    std::string end_anchor_node_name = fetchNodeName(end_anchor_nodeid);

    std::ostringstream sendstream;

    sendstream<<"{";
    sendstream<<"\"type\":\"inspectionpointtravel\",";
    sendstream<<"\"tagname\":\""<<tag_node_name<<"\",";
    sendstream<<"\"actionname\":\""<<action_name<<"\",";
    sendstream<<"\"startinspectionpointname\":\""<<start_anchor_node_name<<"\",";
    sendstream<<"\"endinspectionpointname\":\""<<end_anchor_node_name<<"\"";
    sendstream<<"}";

    prettyjson = sendstream.str();

    return prettyjson;
}

std::string MySQLDBAccess::getInspectionGroupPrettyJSON(std::string eventjson)
{
    std::string prettyjson = "";

    Json::Value result;
    bool parsingSuccessful = parse_JSON(eventjson, result);
    if (!parsingSuccessful)
        return prettyjson;

    std::string action_name = get_action_name(result["action"].asString());

    std::ostringstream sendstream;

    sendstream<<"{";
    sendstream<<"\"type\":\"inspectionGroup\",";
    sendstream<<"\"groupname\":\""<<result["GroupName"].asString()<<"\",";
    sendstream<<"\"actionname\":\""<<action_name<<"\"";
    sendstream<<"}";

    prettyjson = sendstream.str();

    return prettyjson;
}

int MySQLDBAccess::get_count_callback(sql::ResultSet * res, void * pvalue)
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

int MySQLDBAccess::get_size_callback(sql::ResultSet * res, void * pvalue)
{
   int * size = (int *)pvalue;
   if ( res != NULL)
   {
       while (res->next())
       {
           *size += (int)res->getDouble("size");
       }
   }
   // printf("MySQLDBAccess::get_size_callback() size[%d]\n", *size);

   return *size;
}

int MySQLDBAccess::get_stringvalue_callback(sql::ResultSet * res, void * pvalue)
{
   std::string * stringvalue = (std::string *)pvalue;
   if ( res != NULL)
   {
       while (res->next())
       {
           (*stringvalue) = res->getString("stringvalue");
           return 1;
       }
   }
   return 0;
}

int MySQLDBAccess::get_PROJECT_ID_callback(sql::ResultSet * res, void * pvalue)
{
   int * PROJECT_ID = (int *)pvalue;
   if ( res != NULL)
   {
       while (res->next())
       {
           *PROJECT_ID = res->getInt("PROJECT_ID");
           break;
       }
   }
   return *PROJECT_ID;
}

int MySQLDBAccess::get_nodename_callback(sql::ResultSet * res, void * pvalue)
{
   std::string * nodename = (std::string *)pvalue;
   if ( res != NULL)
   {
       while (res->next())
       {
           (*nodename) = res->getString("nodename");
           return 1;
       }
   }
   return 0;
}

int MySQLDBAccess::get_areaname_callback(sql::ResultSet * res, void * pvalue)
{
   std::string * areaname = (std::string *)pvalue;
   if ( res != NULL)
   {
       while (res->next())
       {
           (*areaname) = res->getString("areaname");
           return 1;
       }
   }
   return 0;
}

int MySQLDBAccess::get_SystemConfig_callback(sql::ResultSet * res, void * pvalue)
{
    VILSSystemConfig * sysConfig = (VILSSystemConfig *)pvalue;

    if ( res != NULL)
    {
        //vilslog::printf("MySQLDBAccess::get_SystemConfig_callback() res != NULL\n");

        if(res->next())
        {
            //vilslog::printf("MySQLDBAccess::get_SystemConfig_callback() res->next()\n");

            sysConfig->SolveTagPosition_Time = res->getInt("SOLVETAGPOSITION_TIME");
            sysConfig->SendTAFDB_Time = res->getInt("SENDTAFDB_TIME");
            sysConfig->SendTagAnchorList_Time = res->getInt("SENDTAGANCHORLIST_TIME");
            sysConfig->SendDeviceStatus_Time = res->getInt("SENDDEVICESTATUS_TIME");
            sysConfig->UpdateNodeFromSMIMSIOTSERVER_Time = res->getInt("UPDATENODEFROMSMIMSIOTSERVER_TIME");

            sysConfig->web_udp_port = res->getInt("web_udp_port");
            sysConfig->center_udp_port = res->getInt("center_udp_port");
            sysConfig->coord_udp_port = res->getInt("coord_udp_port");
            sysConfig->center_broadcast_time = res->getInt("center_broadcast_time");
            sysConfig->center_broadcast_port = res->getInt("center_broadcast_port");
            sysConfig->web_socket_port = res->getInt("web_socket_port");
            sysConfig->UsingProtocol = res->getInt("USING_PROTOCOL");

            sysConfig->VILSServer = "";
            if ( res->findColumn("VILSServer") )
                sysConfig->VILSServer = res->getString("VILSServer");

            sysConfig->VILSUID = "";
            if ( res->findColumn("VILSUID") )
                sysConfig->VILSUID = res->getString("VILSUID");

            sysConfig->ERPServer = "";
            int ret = res->findColumn("ERPServer");
            if ( ret > 0 )
                sysConfig->ERPServer = res->getString("ERPServer");
            sysConfig->SSOIP = "";
            ret = res->findColumn("SSOIP");
            if ( ret > 0 )
                sysConfig->SSOIP = res->getString("SSOIP");

            sysConfig->PushNotifyPrefix = "SMIMS";
            ret = res->findColumn("PushNotifyPrefix");
            if ( ret > 0 )
                sysConfig->PushNotifyPrefix = res->getString("PushNotifyPrefix");

            sysConfig->PushNotifyWelcomeTitle = "歡迎使用";
            ret = res->findColumn("PushNotifyWelcomeTitle");
            if ( ret > 0 )
                sysConfig->PushNotifyWelcomeTitle = res->getString("PushNotifyWelcomeTitle");

            if( sysConfig->PushNotifyWelcomeTitle.length() == 0 )
            {
                sysConfig->PushNotifyWelcomeTitle = "歡迎使用";
            }

            sysConfig->PushNotifyWelcomeBody = "感謝您使用北瀚雲端推播系統";
            ret = res->findColumn("PushNotifyWelcomeBody");
            if ( ret > 0 )
                sysConfig->PushNotifyWelcomeBody = res->getString("PushNotifyWelcomeBody");

            if( sysConfig->PushNotifyWelcomeBody.length() == 0 )
            {
                sysConfig->PushNotifyWelcomeBody = "感謝您使用北瀚雲端推播系統";
            }

            sysConfig->EnablePushNotify = 0;
            ret = res->findColumn("EnablePushNotify");
            if ( ret > 0 )
                sysConfig->EnablePushNotify = res->getInt("EnablePushNotify");

            // vilslog::printf("MySQLDBAccess::get_SystemConfig_callback() PushNotifyWelcomeTitle[%s] PushNotifyWelcomeBody[%s]\n",
                // sysConfig->PushNotifyWelcomeTitle.c_str(), sysConfig->PushNotifyWelcomeBody.c_str());

            sysConfig->TagAlertTimeout = res->getInt("TagAlertTimeout");
            sysConfig->TagAlertVoltageLow = res->getString("TagAlertVoltageLow");

            //1.Ranging Data Preprocessing
            sysConfig->ValidDataTime = res->getInt("VALIDDATATIME");
            sysConfig->MaxRangindDistance = res->getInt("MAX_RANGE_DISTANCE");
            sysConfig->EnableNoiseMakup = res->getInt("EnableNoiseMakup");
            sysConfig->NoiseMakupRatio = res->getDouble("NoiseMakupRatio");
            sysConfig->NoiseMakupWeight = res->getDouble("NoiseMakupWeight");
            //sysConfig->RangingDataMethod = res->getInt("RangingDataMethod");
            sysConfig->EnableKalman1D = res->getInt("EnableKalman1D");
            sysConfig->EnableRangingReliability = res->getInt("EnableRangingReliability");
            sysConfig->RangingReliabilityMaxDistance = res->getInt("RangingReliabilityMaxDistance");
            sysConfig->RangingReliabilityMinCount = res->getInt("RangingReliabilityMinCount");
            sysConfig->RangingReliabilityAverageDiff = res->getInt("RangingReliabilityAverageDiff");

            //2.Position Calculation
            sysConfig->EnableAnchor1DMapping = res->getInt("EnableAnchor1DMapping");
            sysConfig->EnableOneAnchorCalculation = res->getInt("EnableOneAnchorCalculation");
            sysConfig->EnableGDOP = res->getInt("EnableGDOP");
            sysConfig->GDOPThreshold = res->getInt("GDOPThreshold");
            sysConfig->CalculationMethod = res->getInt("CalculationMethod");
            sysConfig->EnableIMUVelocity = res->getInt("EnableIMUVelocity");
            sysConfig->IMUVelocityOffset = res->getInt("IMUVelocityOffset");
            sysConfig->SteadyStateOffset = res->getInt("SteadyStateOffset");
            sysConfig->WeightMovingAverageValue1 = res->getDouble("WeightMovingAverageValue1");
            sysConfig->WeightMovingAverageValue2 = res->getDouble("WeightMovingAverageValue2");
            sysConfig->WeightMovingAverageValue3 = res->getDouble("WeightMovingAverageValue3");
            sysConfig->StayInWallTimeout = res->getInt("StayInWallTimeout");
            sysConfig->EnableFixHeight = res->getInt("EnableFixHeight");

            //3.Position Result
            sysConfig->SmoothingResult = res->getInt("SmoothingResult");
            sysConfig->SmoothingResultOffset = res->getInt("SmoothingResultOffset");
            sysConfig->SmoothingResultStep = res->getInt("SmoothingResultStep");
            sysConfig->SmoothingResultFirstStateOffsetRatio = res->getDouble("SmoothingResultFirstStateOffsetRatio");
            sysConfig->SmoothingResultFirstStateRunTime = res->getInt("SmoothingResultFirstStateRunTime");
            sysConfig->SmoothingResultSecondStateOffsetRatio = res->getDouble("SmoothingResultSecondStateOffsetRatio");
            sysConfig->SmoothingResultSecondStateRunTime = res->getInt("SmoothingResultSecondStateRunTime");

            //
            // LoRAWAN
            //
            sysConfig->LoRAWANTPOIC = res->getString("LoRAWANTPOIC");
            sysConfig->MQTTHostIp = res->getString("MQTTHostIp");
            sysConfig->MQTTHostPort = res->getInt("MQTTHostPort");
            sysConfig->MQTTHostUsername = res->getString("MQTTHostUsername");
            sysConfig->MQTTHostPassword = res->getString("MQTTHostPassword");
            sysConfig->MQTTClientId = res->getString("MQTTClientId");

            if ( sysConfig->UsingProtocol == 0 )
                sysConfig->UsingProtocol = UDP;
            if ( sysConfig->web_udp_port <= 0 || sysConfig->web_udp_port >= 65535)
                sysConfig->web_udp_port = 18613;
            if ( sysConfig->center_udp_port <= 0 || sysConfig->center_udp_port >= 65535)
                sysConfig->center_udp_port = 18614;
            if ( sysConfig->coord_udp_port <= 0 || sysConfig->coord_udp_port >= 65535)
                sysConfig->coord_udp_port = 18615;
            if ( sysConfig->center_broadcast_port <= 0 || sysConfig->center_broadcast_port >= 65535)
                sysConfig->center_broadcast_port = 12317;
            if ( sysConfig->web_socket_port <= 0 || sysConfig->web_socket_port >= 65535)
                sysConfig->web_socket_port = 8083;
            if ( sysConfig->SmoothingResultOffset == 0 )
                sysConfig->SmoothingResultOffset = 10;

            if ( sysConfig->center_broadcast_time == 0 )
                sysConfig->center_broadcast_time = 5000;
            if ( sysConfig->TagAlertTimeout == 0 )
                sysConfig->TagAlertTimeout = 5000;
            if (sysConfig->TagAlertVoltageLow.length() == 0)
                sysConfig->TagAlertVoltageLow = "3.8";

            if ( sysConfig->EnableNoiseMakup < 0 || sysConfig->EnableNoiseMakup >= 65535)
                sysConfig->EnableNoiseMakup = 0;

            if ( sysConfig->NoiseMakupRatio <= 0.0f)
                sysConfig->NoiseMakupRatio = 2.0f;

            if ( sysConfig->NoiseMakupWeight <= 0.0f || sysConfig->NoiseMakupWeight > 1.0f)
                sysConfig->NoiseMakupWeight = 0.8f;

            // if ( sysConfig->RangingDataMethod < 0 || sysConfig->RangingDataMethod > 1)
            //     sysConfig->RangingDataMethod = 0;

            if ( sysConfig->EnableKalman1D < 0 || sysConfig->EnableKalman1D >= 65535)
                sysConfig->EnableKalman1D = 0;

            if ( sysConfig->EnableRangingReliability < 0 || sysConfig->EnableRangingReliability >= 65535)
                sysConfig->EnableRangingReliability = 0;

            if ( sysConfig->RangingReliabilityMaxDistance < 0)
                sysConfig->RangingReliabilityMaxDistance = 1300;

            if ( sysConfig->RangingReliabilityMinCount < 0)
                sysConfig->RangingReliabilityMinCount = 1;
            if ( sysConfig->RangingReliabilityAverageDiff < 0)
                sysConfig->RangingReliabilityAverageDiff = 50;

            //2.Position Calculation
            if ( sysConfig->EnableAnchor1DMapping < 0)
                sysConfig->EnableAnchor1DMapping = 0;// 0:disable 1:enable
            if ( sysConfig->EnableOneAnchorCalculation < 0)
                sysConfig->EnableOneAnchorCalculation = 0;// 0:disable 1:enable
            if ( sysConfig->EnableGDOP < 0)
                sysConfig->EnableGDOP = 0;// 0:disable 1:enable
            if ( sysConfig->GDOPThreshold <= 0)
                sysConfig->GDOPThreshold = 60; // default:60
            if ( sysConfig->CalculationMethod < 0)
                sysConfig->CalculationMethod = 0; // 0: kalman 3D, 1:SMOOTHING_FILTER, 2:Weight Moving Average
            if ( sysConfig->EnableIMUVelocity < 0)
                sysConfig->EnableIMUVelocity = 0;// 0:disable 1:enable
            if ( sysConfig->IMUVelocityOffset <= 0)
                sysConfig->IMUVelocityOffset = 100;// default:100
            if ( sysConfig->SteadyStateOffset < 0)
                sysConfig->SteadyStateOffset = 5;// default:5
            if ( sysConfig->WeightMovingAverageValue1 <= 0)
                sysConfig->WeightMovingAverageValue1 = 0.01f; //0.01
            if ( sysConfig->WeightMovingAverageValue2 <= 0)
                sysConfig->WeightMovingAverageValue2 = 0.02f; //0.02
            if ( sysConfig->WeightMovingAverageValue3 <= 0)
                sysConfig->WeightMovingAverageValue3 = 0.97f; //0.97
            if ( sysConfig->StayInWallTimeout <= 0)
                sysConfig->StayInWallTimeout = 5000;// default:5000
            if ( sysConfig->EnableFixHeight < 0)
                sysConfig->EnableFixHeight = 1;// default:1

            //3.Position Result
            if ( sysConfig->SmoothingResult < 0)
                sysConfig->SmoothingResult = 1;
            if ( sysConfig->SmoothingResultOffset < 0)
                sysConfig->SmoothingResultOffset = 10; //in centermeter
            if ( sysConfig->SmoothingResultStep <= 0)
                sysConfig->SmoothingResultStep = 6; // default:6
            if ( sysConfig->SmoothingResultFirstStateOffsetRatio <= 0)
                sysConfig->SmoothingResultFirstStateOffsetRatio = 0.3f;
            if ( sysConfig->SmoothingResultFirstStateRunTime <= 0)
                sysConfig->SmoothingResultFirstStateRunTime = 200; // ms
            if ( sysConfig->SmoothingResultSecondStateOffsetRatio <= 0)
                sysConfig->SmoothingResultSecondStateOffsetRatio = 0.8f;
            if ( sysConfig->SmoothingResultSecondStateRunTime <= 0)
                sysConfig->SmoothingResultSecondStateRunTime = 600; // ms

            //
            // LoRAWAN
            //
            if (sysConfig->LoRAWANTPOIC.length() == 0)
                sysConfig->LoRAWANTPOIC = "GIOT-GW/UL/#";
            if (sysConfig->MQTTHostIp.length() == 0)
                sysConfig->MQTTHostIp = "192.168.99.103";
            if ( sysConfig->MQTTHostPort <= 0)
                sysConfig->MQTTHostPort = 1883;
            if (sysConfig->MQTTHostUsername.length() == 0)
                sysConfig->MQTTHostUsername = "admin";
            if (sysConfig->MQTTHostPassword.length() == 0)
                sysConfig->MQTTHostPassword = "smims2017";
            if (sysConfig->MQTTClientId.length() == 0)
                sysConfig->MQTTClientId = "200cecd38e824e6388827552939fe4d1";

            sysConfig->sendNotifyTime = 480;
            if ( res->findColumn("sendNotifyTime") )
                sysConfig->sendNotifyTime = res->getInt("sendNotifyTime");

            //vilslog::printf("MySQLDBAccess::get_SystemConfig_callback() Finished! web_udp_port[%d]\n", sysConfig->web_udp_port);
            return 1;
        }
        else
        {
            vilslog::printf("MySQLDBAccess::get_SystemConfig_callback() res->next() false\n");
            return 0;
        }
    }
    else
    {
        vilslog::printf("MySQLDBAccess::get_SystemConfig_callback() res == NULL\n");
    }
    return 0;
}

int MySQLDBAccess::get_nodelist_callback(sql::ResultSet * res, void * pvalue)
{
   std::list<NodeStruct*> * nodeList = (std::list<NodeStruct*> *)pvalue;

   int count = 0;
   if ( res != NULL)
   {
        while (res->next())
        {
            NodeStruct * node = createNodeStruct();
            node->nodetype = res->getInt("nodetype");
            snprintf(node->nodeid, 32, "%s", res->getString("nodeid").c_str());
            snprintf(node->macaddress, 32, "%s", res->getString("macaddress").c_str());
            snprintf(node->LoRAMacaddress, 32, "%s", res->getString("loramacaddress").c_str());

            //printf("MySQLDBAccess::get_nodelist_callback() macaddress[%s] LoRAMacaddress[%s]\n", node->macaddress, node->LoRAMacaddress);

            node->tafid = res->getInt("tafid");
            node->buildingid = res->getInt("buildingid");
            node->buildingfloor = res->getInt("buildingfloor");
            if ( res->findColumn("maplayer") > 0 )
            {
                node->maplayer = res->getInt("maplayer");
            }
            if (node->maplayer == 0)
                node->maplayer = 1;

            snprintf(node->nodename, 32, "%s", res->getString("nodename").c_str());
            node->index = res->getInt("idx");
            node->posX = res->getInt("posX");
            node->posY = res->getInt("posY");
            node->posZ = res->getInt("posZ");

            if ( res->findColumn("Longitude") > 0 )
            {
                std::string Longitude = res->getString("Longitude");
                node->Longitude = atof(Longitude.c_str());
            }
            if ( res->findColumn("Latitude") > 0 )
            {
                std::string Latitude = res->getString("Latitude");
                node->Latitude = atof(Latitude.c_str());
            }
            /*
            std::string Longitude = res->getString("Longitude");
            std::string Latitude = res->getString("Latitude");
            node->Longitude = atof(Longitude.c_str());
            node->Latitude = atof(Latitude.c_str());
            */
            if ( (int)node->Longitude == 0 )
                node->Longitude = -1;
            if ( (int)node->Latitude == 0 )
                node->Latitude = -1;

            snprintf(node->voltage, 32, "%s", res->getString("voltage").c_str());
            snprintf(node->temperature, 32, "%s", res->getString("temperature").c_str());
            snprintf(node->version, 32, "%s", res->getString("version").c_str());

            if ( res->findColumn("IP") > 0 )
            {
                snprintf(node->IP, 32, "%s", res->getString("IP").c_str());
            }

            if ( res->findColumn("AreaId") > 0 )
            {
                snprintf(node->AreaId, 32, "%s", res->getString("AreaId").c_str());
            }
            if ( res->findColumn("AreaName") > 0 )
            {
                snprintf(node->AreaName, 256, "%s", res->getString("AreaName").c_str());
            }
            int ret = res->findColumn("blacklist");
            if ( ret > 0 )
                node->blacklist = res->getString("blacklist");

            ret = res->findColumn("whitelist");
            if ( ret > 0 )
                node->whitelist = res->getString("whitelist");

            ret = res->findColumn("joinRangingRange");
            if ( ret > 0 )
                node->joinRangingRange = res->getInt("joinRangingRange");

            ret = res->findColumn("joinNotRangingRange");
            if ( ret > 0 )
                node->joinNotRangingRange = res->getInt("joinNotRangingRange");

            ret = res->findColumn("resident_serno");
            if ( ret > 0 )
                node->resident_serno = res->getString("resident_serno");

            ret = res->findColumn("empid");
            if ( ret > 0 )
                node->empid = res->getString("empid");

            ret = res->findColumn("panid");
            if ( ret > 0 )
                snprintf(node->panid, 32, "%s", res->getString("panid").c_str());

            ret = res->findColumn("opmode");
            if ( ret > 0 )
                node->Opertaion_Mode = res->getInt("opmode");

            ret = res->findColumn("gwid");
            if ( ret > 0 )
                node->gwid = res->getString("gwid");

            ret = res->findColumn("alive");
            if ( ret > 0 )
            {
                node->alive = res->getInt("alive");
                node->aliveTime = getCurrentTimeInSec();
            }
            else
            {
                node->alive = 0;
                node->aliveTime = 0;
            }

            if ( res->findColumn("poweronTimestamp") > 0 )
            {
                node->poweronTimestamp = StringDateTimeTotime_t(res->getString("poweronTimestamp"));
            }
            if ( res->findColumn("poweroffTimestamp") > 0 )
            {
                node->poweroffTimestamp = StringDateTimeTotime_t(res->getString("poweroffTimestamp"));
            }
            if ( res->findColumn("enableSensorAlert") > 0 )
            {
                node->enableSensorAlert =  res->getInt("enableSensorAlert");
            }

            if (strlen(node->AreaId) == 0)
                node->area_unknown = 1; // 1:unknown area
            else
                node->area_unknown = 0;// 0:known area

            if ( res->findColumn("userid") > 0 )
            {
                node->userid =  res->getInt("userid");
            }
            // if(node->nodetype == NODE_TYPE_LOCATOR )
            // {
            //     printf("MySQLDBAccess::get_nodelist_callback() nodeid[%s] maplayer[%d]\n",
            //         node->nodeid, node->maplayer);
            // }
            if ( res->findColumn("maingroupid") > 0 )
            {
                node->maingroupid = res->getInt("maingroupid");
            }

            node->verifykey = "";
            if ( res->findColumn("verifykey") > 0 )
                node->verifykey = res->getString("verifykey");

            nodeList->push_back(node);
            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_node_callback(sql::ResultSet * res, void * pvalue)
{
   NodeStruct * node = (NodeStruct *)pvalue;

   int count = 0;
   if ( res != NULL)
   {
        if (res->next())
        {
            node->nodetype = res->getInt("nodetype");
            snprintf(node->nodeid, 32, "%s", res->getString("nodeid").c_str());
            snprintf(node->macaddress, 32, "%s", res->getString("macaddress").c_str());
            snprintf(node->LoRAMacaddress, 32, "%s", res->getString("loramacaddress").c_str());

            node->tafid = res->getInt("tafid");
            node->buildingid = res->getInt("buildingid");
            node->buildingfloor = res->getInt("buildingfloor");
            if ( res->findColumn("maplayer") > 0 )
            {
                node->maplayer = res->getInt("maplayer");
            }
            if (node->maplayer == 0)
                node->maplayer = 1;

            snprintf(node->nodename, 32, "%s", res->getString("nodename").c_str());
            node->index = res->getInt("idx");
            node->posX = res->getInt("posX");
            node->posY = res->getInt("posY");
            node->posZ = res->getInt("posZ");

            if ( res->findColumn("Longitude") > 0 )
            {
                std::string Longitude = res->getString("Longitude");
                node->Longitude = atof(Longitude.c_str());
            }
            if ( res->findColumn("Latitude") > 0 )
            {
                std::string Latitude = res->getString("Latitude");
                node->Latitude = atof(Latitude.c_str());
            }
            if ( (int)node->Longitude == 0 )
                node->Longitude = -1;
            if ( (int)node->Latitude == 0 )
                node->Latitude = -1;

            snprintf(node->voltage, 32, "%s", res->getString("voltage").c_str());
            snprintf(node->temperature, 32, "%s", res->getString("temperature").c_str());
            snprintf(node->version, 32, "%s", res->getString("version").c_str());

            if ( res->findColumn("IP") > 0 )
            {
                snprintf(node->IP, 32, "%s", res->getString("IP").c_str());
            }

            if ( res->findColumn("AreaId") > 0 )
            {
                snprintf(node->AreaId, 32, "%s", res->getString("AreaId").c_str());
            }
            if ( res->findColumn("AreaName") > 0 )
            {
                snprintf(node->AreaName, 256, "%s", res->getString("AreaName").c_str());
            }
            int ret = res->findColumn("blacklist");
            if ( ret > 0 )
                node->blacklist = res->getString("blacklist");

            ret = res->findColumn("whitelist");
            if ( ret > 0 )
                node->whitelist = res->getString("whitelist");

            ret = res->findColumn("joinRangingRange");
            if ( ret > 0 )
                node->joinRangingRange = res->getInt("joinRangingRange");

            ret = res->findColumn("joinNotRangingRange");
            if ( ret > 0 )
                node->joinNotRangingRange = res->getInt("joinNotRangingRange");

            ret = res->findColumn("resident_serno");
            if ( ret > 0 )
                node->resident_serno = res->getString("resident_serno");

            ret = res->findColumn("empid");
            if ( ret > 0 )
                node->empid = res->getString("empid");

            ret = res->findColumn("panid");
            if ( ret > 0 )
                snprintf(node->panid, 32, "%s", res->getString("panid").c_str());

            ret = res->findColumn("opmode");
            if ( ret > 0 )
                node->Opertaion_Mode = res->getInt("opmode");

            ret = res->findColumn("gwid");
            if ( ret > 0 )
                node->gwid = res->getString("gwid");

            ret = res->findColumn("alive");
            if ( ret > 0 )
            {
                node->alive = res->getInt("alive");
                node->aliveTime = getCurrentTimeInSec();
            }
            else
            {
                node->alive = 0;
                node->aliveTime = 0;
            }

            if ( res->findColumn("poweronTimestamp") > 0 )
            {
                node->poweronTimestamp = StringDateTimeTotime_t(res->getString("poweronTimestamp"));
            }
            if ( res->findColumn("poweroffTimestamp") > 0 )
            {
                node->poweroffTimestamp = StringDateTimeTotime_t(res->getString("poweroffTimestamp"));
            }
            if ( res->findColumn("enableSensorAlert") > 0 )
            {
                node->enableSensorAlert =  res->getInt("enableSensorAlert");
            }

            if (strlen(node->AreaId) == 0)
                node->area_unknown = 1; // 1:unknown area
            else
                node->area_unknown = 0;// 0:known area

            if ( res->findColumn("userid") > 0 )
            {
                node->userid =  res->getInt("userid");
            }
            if ( res->findColumn("maingroupid") > 0 )
            {
                node->maingroupid = res->getInt("maingroupid");
            }
            node->verifykey = "";
            if ( res->findColumn("verifykey") > 0 )
                node->verifykey = res->getString("verifykey");

            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_footprintlist_callback(sql::ResultSet * res, void * pvalue)
{
   std::list<FootprintStruct*> * ftList = (std::list<FootprintStruct*> *)pvalue;

   int count = 0;
   if ( res != NULL)
   {
       while (res->next())
       {
           FootprintStruct * pt = new FootprintStruct();
           pt->nodetype = res->getInt("nodetype");
           pt->nodeid = res->getString("nodeid");
           pt->idx = res->getInt("idx");
           pt->areaid = res->getString("areaid");
           pt->posX = res->getInt("posX");
           pt->posY = res->getInt("posY");
           pt->posZ = res->getInt("posZ");
           pt->voltage = res->getString("voltage");
           pt->temperature = res->getString("temperature");
           pt->datetime = res->getString("datetime");

           ftList->push_back(pt);
           count++;
       }
   }

   return count;
}

int MySQLDBAccess::get_InspectionGrouplist_callback(sql::ResultSet * res, void * pvalue)
{
   std::list<InspectionGroupStruct*> * InspectionGroupList = (std::list<InspectionGroupStruct*> *)pvalue;

   int count = 0;
   if ( res != NULL)
   {
       while (res->next())
       {
           InspectionGroupStruct * InspectionGroup = new InspectionGroupStruct();
           InspectionGroup->GroupID = res->getInt("GroupID");
           InspectionGroup->GroupName = res->getString("GroupName");
           InspectionGroup->StartTime = res->getString("StartTime");
           InspectionGroup->EndTime = res->getString("EndTime");
           InspectionGroup->AlertTime = res->getInt("AlertTime");
           InspectionGroup->AutoFeed = res->getInt("AutoFeed");
           InspectionGroup->issuetime = res->getString("issuetime");
           InspectionGroup->Inspectors = new std::list<std::string>();

           std::string Inspectors_str = res->getString("Inspectors");
           // vilslog::printf("get_InspectionGrouplist_callback() Inspectors_str[%s]\n", Inspectors_str.c_str());

           std::vector<std::string> Inspector_vector = convert_string_to_vector(Inspectors_str);

           // vilslog::printf("get_InspectionGrouplist_callback() Inspector_vector.size()[%d]\n", Inspector_vector.size());

           for (unsigned int i=0; i<Inspector_vector.size(); i++)
           {
               // vilslog::printf("get_InspectionGrouplist_callback() i[%d] [%s]\n", i, Inspector_vector[i].c_str());
               InspectionGroup->Inspectors->push_back(Inspector_vector[i]);
           }

           InspectionGroupList->push_back(InspectionGroup);
           Inspector_vector.clear();
           count++;
       }
   }

   return count;
}

int MySQLDBAccess::get_InspectionPointlist_callback(sql::ResultSet * res, void * pvalue)
{
   std::list<InspectionPointStruct*> * InspectionPointList = (std::list<InspectionPointStruct*> *)pvalue;

   int count = 0;
   if ( res != NULL)
   {
       while (res->next())
       {
           InspectionPointStruct * inspectionPoint = new InspectionPointStruct();
           inspectionPoint->GroupID = res->getInt("GroupID");
           inspectionPoint->PointID = res->getInt("PointID");
           inspectionPoint->PointIndex = res->getInt("PointIndex");
           inspectionPoint->AnchorNodeId = res->getString("AnchorNodeId");
           inspectionPoint->Distance = res->getInt("Distance");
           inspectionPoint->MinTime = res->getInt("MinTime");
           inspectionPoint->MaxTime = res->getInt("MaxTime");
           inspectionPoint->Message = "";
           if ( res->findColumn("Message") > 0 )
           {
               inspectionPoint->Message = res->getString("Message");
           }
           inspectionPoint->TravelTime = res->getInt("TravelTime");
           inspectionPoint->issuetime = res->getString("issuetime");

           if ( inspectionPoint->PointIndex < 0 || inspectionPoint->PointIndex >= 65535)
               inspectionPoint->PointIndex = 0;

           // For Runtime Check
           initialInspectionPointDailyRecord(inspectionPoint->DailyData);

           std::string today = TodayDate();

           inspectionPoint->DailyData.GroupID = inspectionPoint->GroupID;
           inspectionPoint->DailyData.PointID = inspectionPoint->PointID;
           inspectionPoint->DailyData.DateTime = today;

           InspectionPointList->push_back(inspectionPoint);
           count++;
       }
   }

   return count;
}

int MySQLDBAccess::get_InspectionGroupDailyRecord_callback(sql::ResultSet * res, void * pvalue)
{
    InspectionGroupDailyRecord * DailyRecord = (InspectionGroupDailyRecord *)pvalue;

    if ( res != NULL)
    {
        //printf("MySQLDBAccess::get_InspectionGroupDailyRecord_callback() res != NULL\n");

        if(res->next())
        {
            DailyRecord->DailyRecordID = res->getInt("DailyRecordID");
            DailyRecord->GroupID = res->getInt("GroupID");
            DailyRecord->Score = 0;
            if ( res->findColumn("Score") > 0 )
            {
                DailyRecord->Score = res->getInt("Score");
            }
            DailyRecord->Finished = res->getInt("Finished");
            DailyRecord->Exceed = res->getInt("Exceed");
            DailyRecord->Checked = res->getInt("Checked");
            DailyRecord->DateTime = res->getString("DateTime");
            return 1;
        }
        else
        {
            vilslog::printf("MySQLDBAccess::get_InspectionGroupDailyRecord_callback() res->next() false\n");
            return 0;
        }
    }
    else
    {
        vilslog::printf("MySQLDBAccess::get_InspectionGroupDailyRecord_callback() res == NULL\n");
    }
    return 0;
}

int MySQLDBAccess::get_InspectionPointDailyRecord_callback(sql::ResultSet * res, void * pvalue)
{
    InspectionPointDailyRecord * DailyRecord = (InspectionPointDailyRecord *)pvalue;

    if ( res != NULL)
    {
        //printf("MySQLDBAccess::get_InspectionPointDailyRecord_callback() res != NULL\n");

        if(res->next())
        {
            DailyRecord->DailyRecordID = res->getInt("DailyRecordID");
            DailyRecord->GroupID = res->getInt("GroupID");
            DailyRecord->PointID = res->getInt("PointID");
            DailyRecord->Score = res->getInt("Score");
            DailyRecord->Checked = res->getInt("Checked");
            DailyRecord->Finished = res->getInt("Finished");
            DailyRecord->Exceed = res->getInt("Exceed");
            DailyRecord->Inspector = res->getString("Inspector");
            DailyRecord->RecordNodeId = res->getString("RecordNodeId");
            DailyRecord->Status = res->getInt("Status");
            DailyRecord->EnterTime = StringDateTimeTotime_t(res->getString("EnterTime"));
            DailyRecord->LeaveTime = StringDateTimeTotime_t(res->getString("LeaveTime"));
            DailyRecord->DateTime = res->getString("DateTime");
            return 1;
        }
        else
        {
            vilslog::printf("MySQLDBAccess::get_InspectionPointDailyRecord_callback() res->next() false\n");
            return 0;
        }
    }
    else
    {
        vilslog::printf("MySQLDBAccess::get_InspectionPointDailyRecord_callback() res == NULL\n");
    }
    return 0;
}

int MySQLDBAccess::get_eventlist_callback(sql::ResultSet * res, void * pvalue)
{
   std::list<EventStruct*> * eventList = (std::list<EventStruct*> *)pvalue;

   int count = 0;
   if ( res != NULL)
   {
       while (res->next())
       {
           EventStruct * event = new EventStruct();
           event->projectid = res->getInt("PROJECT_ID");
           event->eventid = res->getInt("id");
           event->nodeid = res->getString("nodeid");
           event->action = res->getString("action");
           event->areaid = res->getString("areaid");
           event->issuetime = res->getString("issuetime");

           if ( event->nodeid.length() == 0 || event->areaid.length() == 0 )
           {
               delete event;
           }
           else
           {
               eventList->push_back(event);
               count++;
           }
       }
   }

   return count;
}

int MySQLDBAccess::get_sectiongrouplist_callback(sql::ResultSet * res, void * pvalue)
{
   std::list<SectionGroup*> * sectiongrouplist = (std::list<SectionGroup*> *)pvalue;

   int count = 0;
   if ( res != NULL)
   {
       while (res->next())
       {
           SectionGroup * sectiongroup = new SectionGroup();
           sectiongroup->GroupID = res->getInt("GroupID");
           sectiongroup->projectid = res->getInt("PROJECT_ID");
           //sectiongroup->projectid = 1;
           sectiongroup->SectionType = (SectionTypeEnum)res->getInt("SectionType");
           sectiongroup->AreaID = res->getString("AreaID");
           sectiongroup->Param1 = res->getInt("Param1");
           sectiongroup->Param2 = res->getInt("Param2");
           sectiongroup->Param3 = res->getInt("Param3");
           sectiongroup->Param4 = res->getInt("Param4");
           sectiongroup->Param5 = res->getInt("Param5");
           sectiongroup->Param6 = res->getInt("Param6");
           sectiongroup->maingroupid = res->getInt("maingroupid");

           // vilslog::printf("MySQLDBAccess::get_sectiongrouplist_callback() AreaID[%s] maingroupid[%d]\n", sectiongroup->AreaID.c_str(), sectiongroup->maingroupid);

           // std::list<std::string> * nodeIDList;
           std::string nodeIDListStr = res->getString("nodeIDList");

           sectiongroup->nodeIDList = convert_string_to_list(nodeIDListStr, ',');

           sectiongrouplist->push_back(sectiongroup);
           count++;
       }
   }

   return count;
}

int MySQLDBAccess::get_FiredEventlist_callback(sql::ResultSet * res, void * pvalue)
{
   std::list<FiredEventStruct*> * FiredEventsList = (std::list<FiredEventStruct*> *)pvalue;

   int count = 0;
   if ( res != NULL)
   {
       while (res->next())
       {
           std::string readdatetime = res->getString("readdatetime");
           std::string eventjson = res->getString("eventjson");
           std::string prettyjson = "";

           // if (unread == 1)
           // {
           //     if (readdatetime.length() > 0)
           //     {
           //         continue;
           //     }
           // }

           // if (prettystring == 1)
           // {
           //     prettyjson = getPrettyJSON(eventjson);
           // }
           //
           // if (markasread == 1)
           // {
           //     markFiredEvent(res->getInt("id"));
           // }

           FiredEventStruct * event = new FiredEventStruct();
           event->firedeventid = res->getInt("id");
           event->projectid = res->getInt("PROJECT_ID");
           event->eventid = res->getInt("eventid");
           event->prettyjson = prettyjson;
           event->eventjson = eventjson;
           event->timestart = res->getString("timestart");
           event->timeend = res->getString("timeend");
           event->readdatetime = res->getString("readdatetime");
           event->datetime = res->getString("datetime");

            if ( res->findColumn("empid") > 0 )
            {
                event->empid = res->getString("empid");
            }

            if ( res->findColumn("comment") > 0 )
            {
                event->comment = res->getString("comment");
            }

            event->maingroupid = 1;
            if ( res->findColumn("maingroupid") > 0 )
            {
                event->maingroupid = res->getInt("maingroupid");
            }

           // printf("MySQLDBAccess::read_fired_events() projectid[%d] id[%d] eventjson[%s] timestart[%s] timeend[%s] readdatetime[%s]\n",
           //    event->projectid, event->eventid, event->eventjson.c_str(), event->timestart.c_str(), event->timeend.c_str(), event->readdatetime.c_str());
           // printf("MySQLDBAccess::read_fired_events() prettyjson[%s]\n", event->prettyjson.c_str());

           FiredEventsList->push_back(event);
           count++;
       }
   }

   return count;
}

int MySQLDBAccess::get_Course_callback(sql::ResultSet * res, void * pvalue)
{
    CourseStruct* course = (CourseStruct*)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            course->dbId = res->getInt("dbId");
            course->projectid = res->getInt("PROJECT_ID");
            course->allDay = res->getInt("allDay");
            course->resourceId = res->getString("resourceId");
            course->courseId = res->getString("courseId");
            course->title = res->getString("title");
            course->description = res->getString("description");
            course->start = res->getString("start");
            course->end = res->getString("end");
            course->startTime = res->getString("startTime");
            course->endTime = res->getString("endTime");
            course->startRecur = res->getString("startRecur");
            course->endRecur = res->getString("endRecur");
            course->backgroundColor = res->getString("backgroundColor");
            course->borderColor = res->getString("borderColor");
            course->maingroupid = res->getInt("maingroupid");
            course->issuetime = res->getString("issuetime");

            std::string weekListStr = res->getString("daysOfWeek");

            course->daysOfWeek = new std::list<int>();

            // split "1,2,3" to std::list<int>
            istringstream f(weekListStr.c_str());
            string s;
            while (getline(f, s, ','))
            {
                course->daysOfWeek->push_back(atoi(s.c_str()));
            }
            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_Courselist_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<CourseStruct*> * courseList = (std::list<CourseStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            CourseStruct * course = new CourseStruct();
            course->dbId = res->getInt("dbId");
            course->projectid = res->getInt("PROJECT_ID");
            course->allDay = res->getInt("allDay");
            course->resourceId = res->getString("resourceId");
            course->courseId = res->getString("courseId");
            course->title = res->getString("title");
            course->description = res->getString("description");
            course->start = res->getString("start");
            course->end = res->getString("end");
            course->startTime = res->getString("startTime");
            course->endTime = res->getString("endTime");
            course->startRecur = res->getString("startRecur");
            course->endRecur = res->getString("endRecur");
            course->backgroundColor = res->getString("backgroundColor");
            course->borderColor = res->getString("borderColor");
            course->maingroupid = res->getInt("maingroupid");
            course->issuetime = res->getString("issuetime");

            std::string weekListStr = res->getString("daysOfWeek");
            course->daysOfWeek = new std::list<int>();

            // split "1,2,3" to std::list<int>
            istringstream f(weekListStr.c_str());
            string s;
            while (getline(f, s, ','))
            {
                course->daysOfWeek->push_back(atoi(s.c_str()));
            }

            courseList->push_back(course);
            count++;
        }
    }

    return count;
}

int MySQLDBAccess::get_CourseRollCallRecord_callback(sql::ResultSet * res, void * pvalue)
{
    CourseRollCallRecordStruct * courseRollCall = (CourseRollCallRecordStruct *)pvalue;
    if ( res != NULL)
    {
        if(res->next())
        {
            courseRollCall->projectid = res->getInt("PROJECT_ID");
            courseRollCall->courseId = res->getString("courseId");
            courseRollCall->infoOfDate = res->getString("infoOfDate");
            courseRollCall->issuetime = res->getString("issuetime");

            std::string tagListStr = res->getString("tags");

            courseRollCall->tags = convert_string_to_list(tagListStr, ',');

            // courseRollCall->tags = new std::list<std::string>();
            // // split "tag001,tag002,tag003" to std::list<std::string>
            // istringstream f(tagListStr.c_str());
            // string s;
            // while (getline(f, s, ','))
            // {
            //     courseRollCall->tags->push_back(s);
            // }

            return 1;
        }
        else
        {
            //vilslog::printf("MySQLDBAccess::get_CourseRollCallRecord_callback() res->next() false\n");
            return 0;
        }
    }
    else
    {
        vilslog::printf("MySQLDBAccess::get_CourseRollCallRecord_callback() res == NULL\n");
    }

    return 0;
}

int MySQLDBAccess::get_CourseRollCallRecordList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<CourseRollCallRecordStruct *> * rollcallRecordList = (std::list<CourseRollCallRecordStruct *> *)pvalue;
    if ( res != NULL)
    {
        while(res->next())
        {
            CourseRollCallRecordStruct * courseRollCall = new CourseRollCallRecordStruct();

            courseRollCall->projectid = res->getInt("PROJECT_ID");
            courseRollCall->courseId = res->getString("courseId");
            courseRollCall->infoOfDate = res->getString("infoOfDate");
            courseRollCall->issuetime = res->getString("issuetime");

            std::string tagListStr = res->getString("tags");
            courseRollCall->tags = convert_string_to_list(tagListStr, ',');

            // courseRollCall->tags = new std::list<std::string>();
            //
            // // split "tag001,tag002,tag003" to std::list<std::string>
            // istringstream f(tagListStr.c_str());
            // string s;
            // while (getline(f, s, ','))
            // {
            //     courseRollCall->tags->push_back(s);
            // }

            rollcallRecordList->push_back(courseRollCall);
        }
    }
    else
    {
        vilslog::printf("MySQLDBAccess::get_CourseRollCallRecordList_callback() res == NULL\n");
    }

    return (int)rollcallRecordList->size();
}

int MySQLDBAccess::get_LoRaGatewayList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<LoRaGatewayInfoStruct*> * gatewayList = (std::list<LoRaGatewayInfoStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            LoRaGatewayInfoStruct * gateway = new LoRaGatewayInfoStruct();
            gateway->MQTTalive = 0;
            gateway->pingCount = 0;
            gateway->sf = res->getInt("sf");
            gateway->projectid = res->getInt("PROJECT_ID");
            gateway->gwip = res->getString("gwip");
            gateway->gwid = res->getString("gwid");
            gateway->repeater = res->getString("repeater");
            // gateway->channel = res->getString("channel");
            std::string channel = res->getString("channel");
            gateway->DataTime = res->getString("issuetime");
            gateway->maplayer = 1;

            // split "x,x,x" to std::list<std:string>
            istringstream f(channel.c_str());
            string s;
            while (getline(f, s, ','))
            {
                //cout << s << endl;
                gateway->channels.push_back(s);
            }

            int ret = res->findColumn("alive");
            if ( ret > 0 )
            {
                gateway->alive = res->getInt("alive");
                gateway->aliveTime = getCurrentTimeInSec();
            }
            else
            {
                gateway->alive = 0;
                gateway->aliveTime = 0;
            }

            // ret = res->findColumn("maplayer");
            // if ( ret > 0 )
            // {
            //     gateway->maplayer = res->getInt("maplayer");
            // }

            gateway->maplayer = res->getInt("maplayer");
            gateway->posX = res->getInt("posX");
            gateway->posY = res->getInt("posY");
            gateway->posZ = res->getInt("posZ");
            gateway->maingroupid = res->getInt("maingroupid");

            gateway->verifykey = "";
            if ( res->findColumn("verifykey") > 0 )
                gateway->verifykey = res->getString("verifykey");

            if (gateway->maplayer == 0)
                gateway->maplayer = 1;

            gatewayList->push_back(gateway);
            count++;
        }
    }

    return count;
}

int MySQLDBAccess::get_AreaInOutAnalyzeDayInfo_callback(sql::ResultSet * res, void * pvalue)
{
   AnalyzeDayInfoStruct * dayInfo = (AnalyzeDayInfoStruct *)pvalue;

   int count = 0;
   if ( res != NULL)
   {
       if (res->next())
       {
           // int iarea_id = res->findColumn("area_id");
           // int iarea_name = res->findColumn("area_name");
           // int iorder_count = res->findColumn("order_count");
           // int iorder_day = res->findColumn("order_day");
           // int imax_time = res->findColumn("max_time");
           // int imin_time = res->findColumn("min_time");
           // vilslog::printf("================\n\n");
           // vilslog::printf("get_AreaInOutAnalyzeDayInfo_callback() iarea_id[%d] iarea_name[%d] iorder_count[%d] iorder_day[%d] imax_time[%d] imin_time[%d]\n",
           //      iarea_id, iarea_name, iorder_count, iorder_day, imax_time, imin_time);
           // vilslog::printf("================\n\n");

           dayInfo->max_time = res->getString("max_time");
           dayInfo->min_time = res->getString("min_time");
           dayInfo->count = res->getInt("order_count");
           dayInfo->day = res->getString("order_day");
           dayInfo->area_ids = "";
           dayInfo->area_names = "";

           //vilslog::printf("area_ids[%s] area_names[%s]\n",
           //     area_ids.c_str(), area_names.c_str());

           count++;
       }
   }

   return count;
}

int MySQLDBAccess::get_AnalyzeDayInfo_callback(sql::ResultSet * res, void * pvalue)
{
   AnalyzeDayInfoStruct * dayInfo = (AnalyzeDayInfoStruct *)pvalue;

   int count = 0;
   if ( res != NULL)
   {
       if (res->next())
       {
           dayInfo->max_time = res->getString("max_time");
           dayInfo->min_time = res->getString("min_time");
           dayInfo->count = res->getInt("order_count");
           dayInfo->day = res->getString("order_day");

           count++;
       }
   }

   return count;
}

int MySQLDBAccess::get_User_callback(sql::ResultSet * res, void * pvalue)
{
    UserStruct * user = (UserStruct *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        if (res->next())
        {
            user->userid = res->getInt("userid");
            user->groupid = res->getInt("groupid");
            user->type = res->getInt("type");
            if ( res->findColumn("empid") > 0 )
                user->empid = res->getString("empid");
            else
                user->empid = "";
            // if ( res->findColumn("cardid") > 0 )
            //     user->cardid = res->getString("cardid");
            // else
            //     user->cardid = "";
            user->account = res->getString("account");
            user->name = res->getString("name");
            user->unit = res->getString("unit");
            user->title = res->getString("title");
            user->gender = res->getString("gender");
            user->pwd = res->getString("pwd");
            user->datetime = res->getString("datetime");

            if ( res->findColumn("maingroups") > 0 )
            {
                user->maingroups = convert_string_to_list(res->getString("maingroups"), ',');
            }
            else
            {
                // default maingroup is 1
                user->maingroups = convert_string_to_list("1", ',');
            }

            user->datagroups = convert_string_to_list(res->getString("datagroups"), ',');
            user->subgroups = convert_string_to_list(res->getString("subgroups"), ',');

            user->rfidcards = convert_string_to_list(res->getString("rfidcards"), ',');
            // if ( res->findColumn("cardid") > 0 )
            //     user->rfidcards->push_back(res->getString("cardid"));

            user->nfccards = convert_string_to_list(res->getString("nfccards"), ',');
            user->uwbcards = convert_string_to_list(res->getString("uwbcards"), ',');

            if ( res->findColumn("sensors") > 0 )
            {
                user->sensors = convert_string_to_list(res->getString("sensors"), ',');
            }
            else
            {
                user->sensors = new std::list<std::string>();
            }

            if ( res->findColumn("imageuid") > 0 )
                user->imageuid = res->getString("imageuid");
            else
                user->imageuid = "";

            user->maplayer = res->getInt("maplayer");
            user->posX = res->getInt("posX");
            user->posY = res->getInt("posY");
            user->posZ = res->getInt("posZ");
            if ( res->findColumn("Longitude") > 0 )
                user->Longitude = res->getString("Longitude");
            else
                user->Longitude = "";
            if ( res->findColumn("Latitude") > 0 )
                user->Latitude = res->getString("Latitude");
            else
                user->Latitude = "";

            user->AreaId = "";
            user->AreaName = "";
            user->AreaId0D = "";
            user->AreaType0D = 0;
            user->AreaType5Id = "";
            user->AreaType5Name = "";
            user->RangingResultType = 0;
            user->area_unknown = 0;// 0:known area

            if ( res->findColumn("AreaId") > 0 )
                user->AreaId = res->getString("AreaId");
            if ( res->findColumn("AreaName") > 0 )
                user->AreaName = res->getString("AreaName");
            if ( res->findColumn("AreaId0D") > 0 )
                user->AreaId0D = res->getString("AreaId0D");
            if ( res->findColumn("AreaType0D") > 0 )
                user->AreaType0D = res->getInt("AreaType0D");
            if ( res->findColumn("AreaType5Id") > 0 )
                user->AreaType5Id = res->getString("AreaType5Id");
            if ( res->findColumn("AreaType5Name") > 0 )
                user->AreaType5Name = res->getString("AreaType5Name");

            if (user->AreaId.length() == 0)
                user->area_unknown = 1; // 1:unknown area

            count++;
        }
    }

    return count;
}

int MySQLDBAccess::get_UserList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<UserStruct*> * userList = (std::list<UserStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            UserStruct * user = new UserStruct();
            user->userid = res->getInt("userid");
            user->groupid = res->getInt("groupid");
            user->type = res->getInt("type");
            if ( res->findColumn("empid") > 0 )
                user->empid = res->getString("empid");
            else
                user->empid = "";
            // if ( res->findColumn("cardid") > 0 )
            //     user->cardid = res->getString("cardid");
            // else
            //     user->cardid = "";
            user->account = res->getString("account");
            user->name = res->getString("name");
            user->unit = res->getString("unit");
            user->title = res->getString("title");
            user->gender = res->getString("gender");
            user->pwd = res->getString("pwd");
            user->datetime = res->getString("datetime");

            if ( res->findColumn("maingroups") > 0 )
            {
                user->maingroups = convert_string_to_list(res->getString("maingroups"), ',');
            }
            else
            {
                user->maingroups = convert_string_to_list("", ',');
            }

            user->datagroups = convert_string_to_list(res->getString("datagroups"), ',');
            user->subgroups = convert_string_to_list(res->getString("subgroups"), ',');

            user->rfidcards = convert_string_to_list(res->getString("rfidcards"), ',');
            // if ( res->findColumn("cardid") > 0 )
            //     user->rfidcards->push_back(res->getString("cardid"));

            user->nfccards = convert_string_to_list(res->getString("nfccards"), ',');
            user->uwbcards = convert_string_to_list(res->getString("uwbcards"), ',');

            if ( res->findColumn("sensors") > 0 )
            {
                user->sensors = convert_string_to_list(res->getString("sensors"), ',');
            }
            else
            {
                user->sensors = new std::list<std::string>();
            }

            if ( res->findColumn("imageuid") > 0 )
                user->imageuid = res->getString("imageuid");
            else
                user->imageuid = "";

            user->maplayer = res->getInt("maplayer");
            user->posX = res->getInt("posX");
            user->posY = res->getInt("posY");
            user->posZ = res->getInt("posZ");
            if ( res->findColumn("Longitude") > 0 )
                user->Longitude = res->getString("Longitude");
            else
                user->Longitude = "";
            if ( res->findColumn("Latitude") > 0 )
                user->Latitude = res->getString("Latitude");
            else
                user->Latitude = "";

            user->AreaId = "";
            user->AreaName = "";
            user->AreaId0D = "";
            user->AreaType0D = 0;
            user->AreaType5Id = "";
            user->AreaType5Name = "";
            user->RangingResultType = 0;
            user->area_unknown = 0;

            if ( res->findColumn("AreaId") > 0 )
                user->AreaId = res->getString("AreaId");
            if ( res->findColumn("AreaName") > 0 )
                user->AreaName = res->getString("AreaName");
            if ( res->findColumn("AreaId0D") > 0 )
                user->AreaId0D = res->getString("AreaId0D");
            if ( res->findColumn("AreaType0D") > 0 )
                user->AreaType0D = res->getInt("AreaType0D");
            if ( res->findColumn("AreaType5Id") > 0 )
                user->AreaType5Id = res->getString("AreaType5Id");
            if ( res->findColumn("AreaType5Name") > 0 )
                user->AreaType5Name = res->getString("AreaType5Name");

            if (user->AreaId.length() == 0)
                user->area_unknown = 1; // 1:unknown area

            userList->push_back(user);
            count++;
        }
    }

    return count;
}

int MySQLDBAccess::get_UserGroupList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<UserGroupStruct*> * userGroupList = (std::list<UserGroupStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            UserGroupStruct * usergroup = new UserGroupStruct();
            usergroup->groupid = res->getInt("groupid");
            usergroup->groupname = res->getString("groupname");
            usergroup->permission = res->getString("permission");
            usergroup->datetime = res->getString("datetime");
            userGroupList->push_back(usergroup);
            count++;
        }
    }

    return count;
}

int MySQLDBAccess::get_UserSubGroupList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<UserSubGroupStruct*> * userSubGroupList = (std::list<UserSubGroupStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            UserSubGroupStruct * usersubgroup = new UserSubGroupStruct();
            usersubgroup->subgroupid = res->getInt("subgroupid");
            usersubgroup->subgroupname = res->getString("subgroupname");
            usersubgroup->maingroupid = res->getInt("maingroupid");
            usersubgroup->permission = res->getString("permission");
            if ( res->findColumn("imageuid") > 0 )
                usersubgroup->imageuid = res->getString("imageuid");

            std::string accountids = res->getString("accountids");
            usersubgroup->accountidList = convert_string_to_list(accountids, ',');

            if ( res->findColumn("userids") > 0 )
            {
                std::string userids = res->getString("userids");
                usersubgroup->useridList = convert_string_to_list(userids, ',');
            }
            else
            {
                usersubgroup->useridList = new std::list<std::string>();
            }

            usersubgroup->type = 0;
            usersubgroup->curriculumrecordid = 0;
            if ( res->findColumn("type") > 0 )
                usersubgroup->type = res->getInt("type");
            if ( res->findColumn("validdate") > 0 )
                usersubgroup->validdate = res->getString("validdate");
            if ( res->findColumn("curriculumrecordid") > 0 )
                usersubgroup->curriculumrecordid = res->getInt("curriculumrecordid");

            usersubgroup->datetime = res->getString("datetime");

            userSubGroupList->push_back(usersubgroup);
            count++;
        }
    }

    return count;
}

int MySQLDBAccess::get_UserMainGroupList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<UserMainGroupStruct*> * userMainGroupList = (std::list<UserMainGroupStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            UserMainGroupStruct * usermaingroup = new UserMainGroupStruct();
            usermaingroup->id = res->getInt("id");
            usermaingroup->name = res->getString("name");
            usermaingroup->permission = res->getString("permission");

            std::string accountids = res->getString("accountids");
            usermaingroup->accountidList = convert_string_to_list(accountids, ',');

            if ( res->findColumn("userids") > 0 )
            {
                std::string userids = res->getString("userids");
                usermaingroup->useridList = convert_string_to_list(userids, ',');
            }
            else
            {
                usermaingroup->useridList = new std::list<std::string>();
            }
            if ( res->findColumn("imageuid") > 0 )
                usermaingroup->imageuid = res->getString("imageuid");

            usermaingroup->datetime = res->getString("datetime");

            usermaingroup->autojoin = 0;
            if ( res->findColumn("autojoin") > 0 )
                usermaingroup->autojoin = res->getInt("autojoin");

            usermaingroup->welcommessage = "歡迎加入";
            if ( res->findColumn("welcommessage") > 0 )
                usermaingroup->welcommessage = res->getString("welcommessage");
            else
                usermaingroup->welcommessage.append(usermaingroup->name);

            userMainGroupList->push_back(usermaingroup);
            count++;
        }
    }

    return count;
}

int MySQLDBAccess::get_UserSubGroup_callback(sql::ResultSet * res, void * pvalue)
{
    UserSubGroupStruct * usersubgroup = (UserSubGroupStruct *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        if (res->next())
        {
            usersubgroup->subgroupid = res->getInt("subgroupid");
            usersubgroup->subgroupname = res->getString("subgroupname");
            usersubgroup->maingroupid = res->getInt("maingroupid");
            usersubgroup->permission = res->getString("permission");
            if ( res->findColumn("imageuid") > 0 )
                usersubgroup->imageuid = res->getString("imageuid");

            std::string accountids = res->getString("accountids");
            usersubgroup->accountidList = convert_string_to_list(accountids, ',');

            if ( res->findColumn("userids") > 0 )
            {
                std::string userids = res->getString("userids");
                usersubgroup->useridList = convert_string_to_list(userids, ',');
            }
            else
            {
                usersubgroup->useridList = new std::list<std::string>();
            }

            usersubgroup->type = 0;
            usersubgroup->curriculumrecordid = 0;
            if ( res->findColumn("type") > 0 )
                usersubgroup->type = res->getInt("type");
            if ( res->findColumn("validdate") > 0 )
                usersubgroup->validdate = res->getString("validdate");
            if ( res->findColumn("curriculumrecordid") > 0 )
                usersubgroup->curriculumrecordid = res->getInt("curriculumrecordid");

            usersubgroup->datetime = res->getString("datetime");

            count++;
        }
    }

    return count;
}

int MySQLDBAccess::get_UserMainGroup_callback(sql::ResultSet * res, void * pvalue)
{
    UserMainGroupStruct * usermaingroup = (UserMainGroupStruct *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            usermaingroup->id = res->getInt("id");
            usermaingroup->name = res->getString("name");
            usermaingroup->permission = res->getString("permission");

            std::string accountids = res->getString("accountids");
            usermaingroup->accountidList = convert_string_to_list(accountids, ',');

            if ( res->findColumn("userids") > 0 )
            {
                std::string userids = res->getString("userids");
                usermaingroup->useridList = convert_string_to_list(userids, ',');
            }
            else
            {
                usermaingroup->useridList = new std::list<std::string>();
            }
            if ( res->findColumn("imageuid") > 0 )
                usermaingroup->imageuid = res->getString("imageuid");

            usermaingroup->datetime = res->getString("datetime");

            count++;
        }
    }

    return count;
}

int MySQLDBAccess::get_resident_callback(sql::ResultSet * res, void * pvalue)
{
    UserResidentStruct * resident = (UserResidentStruct *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        if (res->next())
        {
            resident->id = res->getInt("id");
            resident->serno = res->getString("serno");
            resident->gender = res->getString("gender");
            resident->name = res->getString("name");
            resident->roomNo = res->getString("roomNo");
            resident->residentType = res->getString("residentType");
            resident->nurseNo = res->getString("nurseNo");
            resident->datetime = res->getString("datetime");
            count++;
        }
    }

    return count;
}

int MySQLDBAccess::get_residentList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<UserResidentStruct *> * residentList = (std::list<UserResidentStruct *> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            UserResidentStruct * resident = new UserResidentStruct();
            resident->id = res->getInt("id");
            resident->serno = res->getString("serno");
            resident->gender = res->getString("gender");
            resident->name = res->getString("name");
            resident->roomNo = res->getString("roomNo");
            resident->residentType = res->getString("residentType");
            resident->nurseNo = res->getString("nurseNo");
            resident->datetime = res->getString("datetime");

            residentList->push_back(resident);
            count++;
        }
    }

    return count;
}

int MySQLDBAccess::get_employee_callback(sql::ResultSet * res, void * pvalue)
{
    UserEmployeeStruct * employee = (UserEmployeeStruct *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        if (res->next())
        {
            employee->id = res->getInt("id");
            employee->empid = res->getString("empid");
            employee->cname = res->getString("cname");
            employee->orgid = res->getString("orgid");
            employee->orgName = res->getString("orgName");
            employee->area = res->getString("area");
            employee->jobTitle = res->getString("jobTitle");
            employee->gender = res->getString("gender");
            employee->pwd = res->getString("pwd");
            employee->datetime = res->getString("datetime");
            count++;
        }
    }

    return count;
}

int MySQLDBAccess::get_employeeList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<UserEmployeeStruct *> * employeeList = (std::list<UserEmployeeStruct *> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            UserEmployeeStruct * employee = new UserEmployeeStruct();
            employee->id = res->getInt("id");
            employee->empid = res->getString("empid");
            employee->cname = res->getString("cname");
            employee->orgid = res->getString("orgid");
            employee->orgName = res->getString("orgName");
            employee->area = res->getString("area");
            employee->jobTitle = res->getString("jobTitle");
            employee->gender = res->getString("gender");
            employee->pwd = res->getString("pwd");
            employee->datetime = res->getString("datetime");

            employeeList->push_back(employee);

            count++;
        }
    }

    return count;
}

int MySQLDBAccess::get_EmployeeEventReportList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<EmployeeEventReportStruct*> * reportlist = (std::list<EmployeeEventReportStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            EmployeeEventReportStruct * report = new EmployeeEventReportStruct();

            report->eventid_alert = res->getInt("eventid_alert");
            report->eventid_cancel = res->getInt("eventid_cancel");
            report->emp_empid = res->getString("emp_empid");
            report->emp_cname = res->getString("emp_cname");
            report->emp_orgName = res->getString("emp_orgName");
            report->emp_jobTitle = res->getString("emp_jobTitle");
            report->comment = res->getString("comment");
            report->action = res->getString("action");
            report->nodeid = res->getString("nodeid");
            report->locatorid = res->getString("locatorid");
            report->resident_serno = res->getString("resident_serno");
            report->resident_name = res->getString("resident_name");
            report->resident_roomNo = res->getString("resident_roomNo");
            report->eventtime = res->getString("eventtime");
            report->reponsetime = res->getString("reponsetime");

            reportlist->push_back(report);
            count++;
        }
    }

    return count;
}

int MySQLDBAccess::get_PushNotifySettingList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<PushNotifySettingStruct *> * settingList = (std::list<PushNotifySettingStruct *> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            PushNotifySettingStruct * setting = new PushNotifySettingStruct();
            setting->nodeid = res->getString("nodeid");
            setting->targetid = res->getString("targetid");
            setting->functions = res->getString("functions");
            setting->functionList = convert_string_to_list(setting->functions, ',');
            settingList->push_back(setting);
            count++;
        }
    }

    return count;
}

int MySQLDBAccess::get_TriggerPushMessageList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<TriggerPushMessageStruct *> * settingList = (std::list<TriggerPushMessageStruct *> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            TriggerPushMessageStruct * setting = new TriggerPushMessageStruct();
            setting->enable = res->getInt("enable");
            setting->nodeid = res->getString("nodeid");
            setting->targettype = res->getInt("targettype");
            setting->distance = res->getInt("distance");
            setting->title = res->getString("title");
            setting->message = res->getString("message");
            settingList->push_back(setting);
            count++;
        }
    }

    return count;
}

int MySQLDBAccess::get_StringValueList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<std::string> * stringValueList = (std::list<std::string> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            stringValueList->push_back(res->getString("stringvalue"));
            count++;
        }
    }

    return count;
}

int MySQLDBAccess::get_NodeAreaInOut_callback(sql::ResultSet * res, void * pvalue)
{
    AreaInOutEventStruct * nodeAreaInOut = (AreaInOutEventStruct*)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        if (res->next())
        {
            nodeAreaInOut->AreaType0D = res->getInt("AreaType"); // 0:normal 1:room 2:danger
            nodeAreaInOut->action = res->getInt("action"); //1:in 2:out
            nodeAreaInOut->NodeId = res->getString("NodeId");
            nodeAreaInOut->AreaID = res->getString("AreaID");
            nodeAreaInOut->AreaName = res->getString("AreaName");
            nodeAreaInOut->DataTime = res->getString("DataTime");

            if ( res->findColumn("tafid") > 0 )
                nodeAreaInOut->tafid = res->getInt("tafid");
            if ( res->findColumn("MapAreaType") > 0 )
                nodeAreaInOut->MapAreaType = res->getInt("MapAreaType");
            if ( res->findColumn("NodeName") > 0 )
                nodeAreaInOut->NodeName = res->getString("NodeName");

            count++;
        }
    }

    return count;
}

int MySQLDBAccess::get_NodeAreaInOutList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<AreaInOutEventStruct*> * nodeAreaInOutList = (std::list<AreaInOutEventStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            AreaInOutEventStruct * event = new AreaInOutEventStruct();
            event->projectid = res->getInt("PROJECT_ID");
            event->AreaType0D = res->getInt("AreaType"); // 0:normal 1:room 2:danger
            event->action = res->getInt("action"); //1:in 2:out
            event->NodeId = res->getString("NodeId");
            event->AreaID = res->getString("AreaID");
            event->AreaName = res->getString("AreaName");
            event->DataTime = res->getString("DataTime");

            if ( res->findColumn("tafid") > 0 )
                event->tafid = res->getInt("tafid");
            if ( res->findColumn("MapAreaType") > 0 )
                event->MapAreaType = res->getInt("MapAreaType");
            if ( res->findColumn("NodeName") > 0 )
                event->NodeName = res->getString("NodeName");

            nodeAreaInOutList->push_back(event);
            count++;
        }
    }

    return count;
}

int MySQLDBAccess::get_maplayer_callback(sql::ResultSet * res, void * pvalue)
{
    MapLayerStruct * maplayer = (MapLayerStruct *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        if (res->next())
        {
            maplayer->layerid = res->getInt("layerid");
            maplayer->layer_name = res->getString("layer_name");
            maplayer->layer_description = res->getString("layer_description");
            maplayer->WGS48OriginX = res->getString("WGS48OriginX");
            maplayer->WGS48OriginY = res->getString("WGS48OriginY");
            maplayer->MAPNorth = res->getString("MAP_North");
            maplayer->BuildingID = res->getInt("BuildingID");
            maplayer->BuildingFloor = res->getInt("BuildingFloor");
            maplayer->POSX = res->getString("POSX");
            maplayer->POSY = res->getString("POSY");
            maplayer->SCALE = res->getString("SCALE");
            maplayer->COEFF = res->getString("COEFF");
            maplayer->FILE = res->getString("FILE");
            maplayer->MAPWIDTH = res->getString("MAPWIDTH");
            maplayer->MAPHEIGHT = res->getString("MAPHEIGHT");
            count++;
        }
    }

    return count;
}

int MySQLDBAccess::get_maplayerList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<MapLayerStruct*> * maplayerList = (std::list<MapLayerStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            MapLayerStruct * maplayer = new MapLayerStruct();
            maplayer->layerid = res->getInt("layerid");
            maplayer->layer_name = res->getString("layer_name");
            maplayer->layer_description = res->getString("layer_description");
            maplayer->WGS48OriginX = res->getString("WGS48OriginX");
            maplayer->WGS48OriginY = res->getString("WGS48OriginY");
            maplayer->MAPNorth = res->getString("MAP_North");
            maplayer->BuildingID = res->getInt("BuildingID");
            maplayer->BuildingFloor = res->getInt("BuildingFloor");
            maplayer->POSX = res->getString("POSX");
            maplayer->POSY = res->getString("POSY");
            maplayer->SCALE = res->getString("SCALE");
            maplayer->COEFF = res->getString("COEFF");
            maplayer->FILE = res->getString("FILE");
            maplayer->MAPWIDTH = res->getString("MAPWIDTH");
            maplayer->MAPHEIGHT = res->getString("MAPHEIGHT");

            maplayerList->push_back(maplayer);
            count++;
        }
    }

    return count;
}

int MySQLDBAccess::get_AoANodeList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<AoANodeStruct*> * aoaList = (std::list<AoANodeStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            AoANodeStruct * aoaNode = new AoANodeStruct();
            aoaNode->projectid = 0;
            snprintf(aoaNode->nodeid, 32, "%s", res->getString("nodeid").c_str());
            snprintf(aoaNode->nodename, 32, "%s", res->getString("nodename").c_str());
            snprintf(aoaNode->panid, 32, "%s", res->getString("panid").c_str());
            snprintf(aoaNode->macaddress, 32, "%s", res->getString("macaddress").c_str());
            aoaNode->maplayer = res->getInt("maplayer");
            aoaNode->posX = res->getInt("posX");
            aoaNode->posY = res->getInt("posY");
            aoaNode->posZ = res->getInt("posZ");
            aoaNode->pitch = res->getInt("pitch");
            aoaNode->roll = res->getInt("roll");
            aoaNode->yaw = res->getInt("yaw");
            if ( res->findColumn("gridsize") > 0 )
                aoaNode->gridsize = res->getInt("gridsize");
            aoaNode->maingroupid = res->getInt("maingroupid");

            aoaNode->verifykey = "";
            if ( res->findColumn("verifykey") > 0 )
                aoaNode->verifykey = res->getString("verifykey");

            if (aoaNode->maplayer == 0)
                aoaNode->maplayer = 1;

            aoaList->push_back(aoaNode);
            count++;
        }
    }

    return count;
}

int MySQLDBAccess::get_maplayerareaList_callback(sql::ResultSet * res, void * pvalue)
{
   std::list<MapLayerAreaStruct*> * areaList = (std::list<MapLayerAreaStruct*> *)pvalue;

   int count = 0;
   if ( res != NULL)
   {
        while (res->next())
        {
            MapLayerAreaStruct * area = new MapLayerAreaStruct();
            area->layerid = res->getInt("layerid");
            area->areaid = res->getString("areaid");
            area->areaname = res->getString("areaname");
            area->datetime = res->getString("datetime");
            area->geojson = res->getString("geojson");
            area->Polygons = getPolygonPoints(res->getString("geojson"));
            area->type = res->getInt("type");
            area->WGS48_Origin_X = 0.0;
            area->WGS48_Origin_Y = 0.0;
            areaList->push_back(area);
            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_maplayerarea_callback(sql::ResultSet * res, void * pvalue)
{
   MapLayerAreaStruct * area = (MapLayerAreaStruct *)pvalue;

   int count = 0;
   if ( res != NULL)
   {
        if (res->next())
        {
            area->layerid = res->getInt("layerid");
            area->areaid = res->getString("areaid");
            area->areaname = res->getString("areaname");
            area->datetime = res->getString("datetime");
            area->geojson = res->getString("geojson");
            area->Polygons = getPolygonPoints(res->getString("geojson"));
            area->type = res->getInt("type");
            area->WGS48_Origin_X = 0.0;
            area->WGS48_Origin_Y = 0.0;
            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_maplayerareaname_callback(sql::ResultSet * res, void * pvalue)
{
   std::string * areaname = (std::string *)pvalue;
   if ( res != NULL)
   {
       while (res->next())
       {
           (*areaname) = res->getString("areaname");
           return 1;
       }
   }
   return 0;
}

int MySQLDBAccess::get_maplayername_callback(sql::ResultSet * res, void * pvalue)
{
   std::string * MaplayerName = (std::string *)pvalue;
   if ( res != NULL)
   {
       while (res->next())
       {
           (*MaplayerName) = res->getString("layer_name");
           return 1;
       }
   }
   return 0;
}

int MySQLDBAccess::get_bodyinfoWeightList_callback(sql::ResultSet * res, void * pvalue)
{
   std::list<BodyInfoStruct*> * bodyinfoList = (std::list<BodyInfoStruct*> *)pvalue;

   int count = 0;
   if ( res != NULL)
   {
        while (res->next())
        {
            BodyInfoStruct * bodyinfo = new BodyInfoStruct();
            bodyinfo->nodeid = res->getString("nodeid");
            bodyinfo->gender = res->getInt("gender");
            bodyinfo->weight = res->getString("weight");
            bodyinfo->datetime = res->getString("datetime");
            bodyinfoList->push_back(bodyinfo);
            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_bodyinfoHeartBeatList_callback(sql::ResultSet * res, void * pvalue)
{
   std::list<BodyInfoStruct*> * bodyinfoList = (std::list<BodyInfoStruct*> *)pvalue;

   int count = 0;
   if ( res != NULL)
   {
        while (res->next())
        {
            BodyInfoStruct * bodyinfo = new BodyInfoStruct();
            bodyinfo->nodeid = res->getString("nodeid");
            bodyinfo->gender = res->getInt("gender");
            bodyinfo->heartbeat = res->getString("heartbeat");
            bodyinfo->datetime = res->getString("datetime");
            bodyinfoList->push_back(bodyinfo);
            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_maplayerid_callback(sql::ResultSet * res, void * pvalue)
{
    int * layerid = (int *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
         if (res->next())
         {
             *layerid = res->getInt("layerid");
             count++;
         }
     }
     return count;
}

int MySQLDBAccess::get_Survey_callback(sql::ResultSet * res, void * pvalue)
{
   SurveyStruct * surveyinfo = (SurveyStruct *)pvalue;

   int count = 0;
   if ( res != NULL)
   {
        if (res->next())
        {
            surveyinfo->surveyid = res->getInt("surveyid");
            surveyinfo->type = res->getInt("type");
            surveyinfo->title = res->getString("title");
            surveyinfo->description = res->getString("description");
            surveyinfo->userid = res->getInt("userid");
            surveyinfo->maingroups = res->getString("maingroups");
            if ( res->findColumn("subgroups") > 0 )
                surveyinfo->subgroups = res->getString("subgroups");
            else
                surveyinfo->subgroups = "99999";
            surveyinfo->DateTime = res->getString("DateTime");

            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_SurveyList_callback(sql::ResultSet * res, void * pvalue)
{
   std::list<SurveyStruct*> * surveyList = (std::list<SurveyStruct*> *)pvalue;

   int count = 0;
   if ( res != NULL)
   {
        while (res->next())
        {
            SurveyStruct * surveyinfo = new SurveyStruct();
            surveyinfo->surveyid = res->getInt("surveyid");
            surveyinfo->type = res->getInt("type");
            surveyinfo->title = res->getString("title");
            surveyinfo->description = res->getString("description");
            surveyinfo->userid = res->getInt("userid");
            surveyinfo->maingroups = res->getString("maingroups");
            if ( res->findColumn("subgroups") > 0 )
                surveyinfo->subgroups = res->getString("subgroups");
            else
                surveyinfo->subgroups = "99999";
            surveyinfo->DateTime = res->getString("DateTime");
            surveyList->push_back(surveyinfo);
            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_SurveyItemList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<SurveyItemStruct*> * surveyItemList = (std::list<SurveyItemStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            SurveyItemStruct * surveyiteminfo = new SurveyItemStruct();
            surveyiteminfo->surveyid = res->getInt("surveyid");
            surveyiteminfo->surveyitemid = res->getInt("surveyitemid");
            surveyiteminfo->description = res->getString("description");
            surveyiteminfo->DateTime = res->getString("DateTime");
            surveyItemList->push_back(surveyiteminfo);
            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_PushNotifyMessageRecord_callback(sql::ResultSet * res, void * pvalue)
{
    PushNotifyMessageRecordStruct * record = (PushNotifyMessageRecordStruct *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        if (res->next())
        {
            record->id = res->getInt("dbId");
            record->type = res->getInt("type");
            record->nodeid = res->getString("nodeid");
            record->title = res->getString("title");
            record->message = res->getString("message");
            record->messagehashkey = res->getString("messagehashkey");
            record->targetids = res->getString("targetids");
            record->subgroupids = res->getString("subgroupids");
            record->surveyids = res->getString("surveyids");
            record->hashkeys = res->getString("hashkeys");
            if ( res->findColumn("deadlines") > 0 )
                record->deadlines = res->getString("deadlines");
            // if ( res->findColumn("accountid") > 0 )
            //     record->accountid = res->getString("accountid");
            if ( res->findColumn("imageuid") > 0 )
                record->imageuid = res->getString("imageuid");

            record->userid = res->getInt("userid");
            record->maingroups = res->getString("maingroups");

            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_PushNotifyMessageRecordList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<PushNotifyMessageRecordStruct*> * recordList = (std::list<PushNotifyMessageRecordStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            PushNotifyMessageRecordStruct * record = new PushNotifyMessageRecordStruct();
            record->type = res->getInt("type");
            record->nodeid = res->getString("nodeid");
            record->title = res->getString("title");
            record->message = res->getString("message");
            record->messagehashkey = res->getString("messagehashkey");
            record->targetids = res->getString("targetids");
            record->subgroupids = res->getString("subgroupids");
            record->surveyids = res->getString("surveyids");
            record->hashkeys = res->getString("hashkeys");
            if ( res->findColumn("deadlines") > 0 )
                record->deadlines = res->getString("deadlines");
            // if ( res->findColumn("accountid") > 0 )
            //     record->accountid = res->getString("accountid");
            if ( res->findColumn("imageuid") > 0 )
                record->imageuid = res->getString("imageuid");

            record->userid = res->getInt("userid");
            record->maingroups = res->getString("maingroups");

            recordList->push_back(record);
            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_SurveyEventRecord_callback(sql::ResultSet * res, void * pvalue)
{
    SurveyEventRecordStruct * record = (SurveyEventRecordStruct*)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            if ( res->findColumn("formid") > 0 )
            {
                record->eventid = res->getInt("eventid");
                record->PushNotifyMessageRecorddbId = res->getInt("PushNotifyMessageRecorddbId");
                record->surveyid = res->getInt("surveyid");
                record->deadline = res->getString("deadline");
                record->hashkey = res->getString("hashkey");
                // record->accountid = res->getString("accountid");
                record->formid = res->getString("formid");
                record->datetime = res->getString("datetime");
                record->userid = res->getInt("userid");
                record->maingroups = res->getString("maingroups");

                count++;
            }
        }
    }
    return count;
}

int MySQLDBAccess::get_CurriculumRecord_callback(sql::ResultSet * res, void * pvalue)
{
    CurriculumRecordStruct * record = (CurriculumRecordStruct*)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        if (res->next())
        {

            record->id = res->getInt("id");
            record->userid = res->getInt("userid"); // 發起人
            record->curriculumid = res->getInt("curriculumid");// Curriculum.id
            record->shared = res->getInt("shared");
            record->name = res->getString("name");
            record->maingroups = res->getString("maingroups"); // UserMainGroup.id
            record->number = res->getString("number");
            record->opendate = res->getString("opendate");
            record->closedate = res->getString("closedate");
            record->startdate = res->getString("startdate");
            record->enddate = res->getString("enddate");
            record->starttime = res->getString("starttime");
            record->endtime = res->getString("endtime");
            record->signinstarttime = res->getString("signinstarttime");
            record->signinendtime = res->getString("signinendtime");
            record->weekday = res->getString("weekday");
            record->totalhours = res->getString("totalhours");
            record->location = res->getString("location");
            record->description = res->getString("description");
            record->teacher = res->getString("teacher");
            record->teacheruserid = res->getInt("teacheruserid");
            record->maxnumber = res->getInt("maxnumber");
            record->autoapproval = res->getInt("autoapproval");
            record->cost = res->getString("cost");
            record->note = res->getString("note");
            record->files = res->getString("files");
            record->status = res->getInt("status");
            record->datetime = res->getString("datetime");

            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_Curriculum_callback(sql::ResultSet * res, void * pvalue)
{
    CurriculumStruct * record = (CurriculumStruct*)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        if (res->next())
        {

            record->id = res->getInt("id");
            record->userid = res->getInt("userid");
            record->curriculumtypeid = res->getInt("curriculumtypeid");
            record->recordid = res->getInt("recordid");
            record->prefix = res->getString("prefix");
            record->subgroups = res->getString("subgroups");
            record->maingroups = res->getString("maingroups");
            record->number = res->getString("number");
            record->shared = res->getInt("shared");
            record->sharedmaingroups = res->getString("sharedmaingroups");
            record->datetime = res->getString("datetime");

            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_CurriculumType_callback(sql::ResultSet * res, void * pvalue)
{
    CurriculumTypeStruct * record = (CurriculumTypeStruct*)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        if (res->next())
        {

            record->id = res->getInt("id");
            record->name = res->getString("name");
            record->maingroups = res->getString("maingroups");
            record->datetime = res->getString("datetime");

            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_CurriculumApply_callback(sql::ResultSet * res, void * pvalue)
{
    CurriculumApplyStruct * record = (CurriculumApplyStruct*)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        if (res->next())
        {

            record->userid = res->getInt("userid");
            record->curriculumrecordid = res->getInt("curriculumrecordid");
            record->status = res->getInt("status");
            record->datetime = res->getString("datetime");

            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_SurveyEventRecordList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<SurveyEventRecordStruct*> * recordList = (std::list<SurveyEventRecordStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            if ( res->findColumn("formid") > 0 )
            {
                SurveyEventRecordStruct * record = new SurveyEventRecordStruct();
                record->eventid = res->getInt("eventid");
                record->PushNotifyMessageRecorddbId = res->getInt("PushNotifyMessageRecorddbId");
                record->surveyid = res->getInt("surveyid");
                record->deadline = res->getString("deadline");
                record->hashkey = res->getString("hashkey");
                // record->accountid = res->getString("accountid");
                record->formid = res->getString("formid");
                record->datetime = res->getString("datetime");
                record->userid = res->getInt("userid");
                record->maingroups = res->getString("maingroups");

                recordList->push_back(record);
                count++;
            }
        }
    }
    return count;
}

int MySQLDBAccess::get_ApplicationFormLeave_callback(sql::ResultSet * res, void * pvalue)
{
    ApplicationFormLeaveStruct * record = (ApplicationFormLeaveStruct *) pvalue;

    int count = 0;
    if ( res != NULL)
    {
        if (res->next())
        {
            record->formid = res->getString("formid");
            record->applytime = res->getString("applytime");
            // record->accountid = res->getString("accountid");
            record->userid = res->getInt("userid");
            record->subgroups = res->getString("subgroups");
            record->type = res->getString("type");
            record->reason = res->getString("reason");
            record->startdate = res->getString("startdate");
            record->starttime = res->getString("starttime");
            record->enddate = res->getString("enddate");
            record->endtime = res->getString("endtime");
            record->reissue = res->getInt("reissue");
            record->applyusername = res->getString("applyusername");
            record->applyuserrelation = res->getString("applyuserrelation");
            record->imageids = res->getString("imageids");
            record->fileids = res->getString("fileids");
            record->signoffstatus = res->getInt("signoffstatus");
            // record->signoffaccountid = res->getString("signoffaccountid");
            record->signoffuserid = res->getInt("signoffuserid");
            record->maingroups = res->getString("maingroups");
            record->datetime = res->getString("datetime");
            record->updatetime = res->getString("updatetime");

            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_RFIDCardList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<RFIDCardStruct*> * cardList = (std::list<RFIDCardStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            RFIDCardStruct * card = new RFIDCardStruct();
            card->cardid = res->getString("cardid");
            card->userid = res->getInt("userid");

            if ( res->findColumn("name") > 0 )
                card->name = res->getString("name");
            else
                card->name = card->cardid;

            if ( res->findColumn("type") > 0 )
                card->type = res->getString("type");
            else
                card->type = "卡片";

            if ( res->findColumn("maplayer") > 0 )
                card->maplayer = res->getInt("maplayer");
            else
                card->maplayer = 0;

            if ( res->findColumn("unknown") > 0 )
                card->unknown = res->getInt("unknown");
            else
                card->unknown = 0;

            if ( res->findColumn("deviceid") > 0 )
                card->deviceid = res->getInt("deviceid");
            else
                card->deviceid = 0;

            if ( res->findColumn("alive") > 0 )
                card->alive = res->getInt("alive");
            else
                card->alive = 0;

            if(card->userid > 0)
            {
                card->unknown = 0;
            }

            // vilslog::printf ("MySQLDBAccess::get_RFIDCardList_callback() rfidcard cardid[%s] deviceid[%d] unknown[%d] maplayer[%d]\n",
            //     card->cardid.c_str(), card->deviceid, card->unknown, card->maplayer);

            card->datetime = res->getString("datetime");
            card->updatetime = res->getString("updatetime");

            if ( res->findColumn("maingroupid") > 0 )
                card->maingroupid = res->getInt("maingroupid");
            else
                card->maingroupid = 1;

            cardList->push_back(card);
            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_NFCCardList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<NFCCardStruct*> * cardList = (std::list<NFCCardStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            NFCCardStruct * card = new NFCCardStruct();
            card->cardid = res->getString("cardid");
            card->userid = res->getInt("userid");

            if ( res->findColumn("name") > 0 )
                card->name = res->getString("name");
            else
                card->name = card->cardid;

            if ( res->findColumn("type") > 0 )
                card->type = res->getString("type");
            else
                card->type = "卡片";

            if ( res->findColumn("maplayer") > 0 )
                card->maplayer = res->getInt("maplayer");
            else
                card->maplayer = 0;

            if ( res->findColumn("unknown") > 0 )
                card->unknown = res->getInt("unknown");
            else
                card->unknown = 0;

            if ( res->findColumn("deviceid") > 0 )
                card->deviceid = res->getInt("deviceid");
            else
                card->deviceid = 0;

            if ( res->findColumn("alive") > 0 )
                card->alive = res->getInt("alive");
            else
                card->alive = 0;

            if(card->userid > 0)
            {
                card->unknown = 0;
            }

            // vilslog::printf ("MySQLDBAccess::get_NFCCardList_callback() nfccard cardid[%s] deviceid[%d] unknown[%d] maplayer[%d]\n",
            //     card->cardid.c_str(), card->deviceid, card->unknown, card->maplayer);

            card->datetime = res->getString("datetime");
            card->updatetime = res->getString("updatetime");

            if ( res->findColumn("maingroupid") > 0 )
                card->maingroupid = res->getInt("maingroupid");
            else
                card->maingroupid = 1;

            cardList->push_back(card);
            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_RFIDDeviceList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<RFIDDeviceStruct*> * deviceList = (std::list<RFIDDeviceStruct*> *)pvalue;
    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            RFIDDeviceStruct * record = new RFIDDeviceStruct();
            record->deviceid = res->getInt("deviceid");
            record->macaddress = res->getString("macaddress");
            record->type = res->getString("type");
            record->name = res->getString("name");
            record->ip = res->getString("ip");
            record->gwid = res->getString("gwid");
            record->LoRAMacaddress = res->getString("loramacaddress");
            record->datetime = res->getString("datetime");

            if ( res->findColumn("alive") > 0 )
            {
                record->alive = res->getInt("alive");
                record->aliveTime = getCurrentTimeInSec();
            }
            else
            {
                record->alive = 0;
                record->aliveTime = 0;
            }

            if ( res->findColumn("maingroupid") > 0 )
                record->maingroupid = res->getInt("maingroupid");
            else
                record->maingroupid = 1;

            record->verifykey = "";
            if ( res->findColumn("verifykey") > 0 )
                record->verifykey = res->getString("verifykey");

            deviceList->push_back(record);
            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_NFCDeviceList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<NFCDeviceStruct*> * deviceList = (std::list<NFCDeviceStruct*> *)pvalue;
    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            NFCDeviceStruct * record = new NFCDeviceStruct();
            record->deviceid = res->getInt("deviceid");
            record->macaddress = res->getString("macaddress");
            record->name = res->getString("name");
            record->type = res->getString("type");
            record->gwid = res->getString("gwid");
            record->LoRAMacaddress = res->getString("loramacaddress");
            record->datetime = res->getString("datetime");

            if ( res->findColumn("alive") > 0 )
            {
                record->alive = res->getInt("alive");
                record->aliveTime = getCurrentTimeInSec();
            }
            else
            {
                record->alive = 0;
                record->aliveTime = 0;
            }

            if ( res->findColumn("ssid") > 0 )
            {
                record->ssid = res->getString("ssid");
            }
            if ( res->findColumn("wifimac") > 0 )
            {
                record->wifimac = res->getString("wifimac");
            }
            if ( res->findColumn("version") > 0 )
            {
                record->version = res->getString("version");
            }
            if ( res->findColumn("maingroupid") > 0 )
                record->maingroupid = res->getInt("maingroupid");
            else
                record->maingroupid = 1;

            record->verifykey = "";
            if ( res->findColumn("verifykey") > 0 )
                record->verifykey = res->getString("verifykey");

            // vilslog::printf ("MySQLDBAccess::get_NFCDeviceList_callback() deviceid[%d] macaddress[%s] LoRAMacaddress[%s] name[%s]\n",
            //     record->deviceid, record->macaddress.c_str(), record->LoRAMacaddress.c_str(), record->name.c_str());

            deviceList->push_back(record);
            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_EDGEDeviceList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<EDGEDeviceStruct*> * deviceList = (std::list<EDGEDeviceStruct*> *)pvalue;
    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            EDGEDeviceStruct * record = new EDGEDeviceStruct();
            record->alive = res->getInt("alive");
            record->deviceid = res->getInt("deviceid");
            record->macaddress = res->getString("macaddress");
            record->name = res->getString("name");
            record->type = res->getString("type");
            record->ip = res->getString("ip");
            record->LoRAMacaddress = res->getString("loramacaddress");
            record->gwid = res->getString("gwid");
            record->datetime = res->getString("datetime");
            record->version = res->getString("version");

            record->aliveTime = 0;
            if (record->alive > 0)
                record->aliveTime = getCurrentTimeInSec();

            record->maingroupid = 1;
            if ( res->findColumn("maingroupid") > 0 )
                record->maingroupid = res->getInt("maingroupid");

            record->verifykey = "";
            if ( res->findColumn("verifykey") > 0 )
                record->verifykey = res->getString("verifykey");

            deviceList->push_back(record);
            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_CARDRollCallStruct_callback(sql::ResultSet * res, void * pvalue)
{
    CARDRollCallStruct * rollcall = (CARDRollCallStruct *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        if (res->next())
        {
            rollcall->rollcallid = res->getInt("rollcallid");
            rollcall->rollcallname = res->getString("rollcallname");
            rollcall->StartTime = res->getString("StartTime");
            rollcall->EndTime = res->getString("EndTime");
            rollcall->StartDate = res->getString("StartDate");
            rollcall->EndDate = res->getString("EndDate");

            std::string rfiddeviceids = res->getString("rfiddeviceids");
            std::list<std::string> * rfiddeviceidList = convert_string_to_list(rfiddeviceids, ',');
            for (std::list<std::string>::iterator rfiddeviceid = rfiddeviceidList->begin(); rfiddeviceid != rfiddeviceidList->end(); rfiddeviceid++)
            {
                rollcall->rfiddeviceids.push_back(convert_string_to_int((*rfiddeviceid)));
            }
            rfiddeviceidList->clear();
            delete rfiddeviceidList;

            std::string nfclocators = res->getString("nfclocators");
            std::list<std::string> * nfclocatorList = convert_string_to_list(nfclocators, ',');
            for (std::list<std::string>::iterator nfclocator = nfclocatorList->begin(); nfclocator != nfclocatorList->end(); nfclocator++)
            {
                rollcall->nfclocators.push_back((*nfclocator));
            }
            nfclocatorList->clear();
            delete nfclocatorList;

            std::string subgroups = res->getString("subgroups");
            std::list<std::string> * subgroupList = convert_string_to_list(subgroups, ',');
            for (std::list<std::string>::iterator subgroup = subgroupList->begin(); subgroup != subgroupList->end(); subgroup++)
            {
                rollcall->subgroups.push_back(convert_string_to_int((*subgroup)));
            }
            subgroupList->clear();
            delete subgroupList;

            std::string weekdays = res->getString("weekday");
            std::list<std::string> * weekdayList = convert_string_to_list(weekdays, ',');
            for (std::list<std::string>::iterator weekday = weekdayList->begin(); weekday != weekdayList->end(); weekday++)
            {
                rollcall->weekday.push_back(convert_string_to_int((*weekday)));
            }
            weekdayList->clear();
            delete weekdayList;

            std::string validdates = res->getString("validdate");
            std::list<std::string> * validdateList = convert_string_to_list(validdates, ',');
            for (std::list<std::string>::iterator validdate = validdateList->begin(); validdate != validdateList->end(); validdate++)
            {
                rollcall->validdate.push_back((*validdate));
            }
            validdateList->clear();
            delete validdateList;

            std::string invaliddates = res->getString("invaliddate");
            std::list<std::string> * invaliddateList = convert_string_to_list(invaliddates, ',');
            for (std::list<std::string>::iterator invaliddate = invaliddateList->begin(); invaliddate != invaliddateList->end(); invaliddate++)
            {
                rollcall->invaliddate.push_back((*invaliddate));
            }
            invaliddateList->clear();
            delete invaliddateList;

            rollcall->presentnotify = res->getInt("presentnotify");
            rollcall->presentcomment = res->getString("presentcomment");
            rollcall->absentnotify = res->getInt("absentnotify");
            rollcall->absentcomment = res->getString("absentcomment");
            rollcall->datetime = res->getString("datetime");
            rollcall->updatetime = res->getString("updatetime");
            rollcall->resend = res->getInt("resend");
            rollcall->maingroups = res->getString("maingroups");
            rollcall->type = res->getInt("type");
            rollcall->curriculumrecordid = res->getInt("curriculumrecordid");

            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_CARDRollCallStructList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<CARDRollCallStruct*> * rollcallList = (std::list<CARDRollCallStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            CARDRollCallStruct * rollcall = new CARDRollCallStruct();
            rollcall->rollcallid = res->getInt("rollcallid");
            rollcall->rollcallname = res->getString("rollcallname");
            rollcall->StartTime = res->getString("StartTime");
            rollcall->EndTime = res->getString("EndTime");
            rollcall->StartDate = res->getString("StartDate");
            rollcall->EndDate = res->getString("EndDate");

            std::string rfiddeviceids = res->getString("rfiddeviceids");
            std::list<std::string> * rfiddeviceidList = convert_string_to_list(rfiddeviceids, ',');
            for (std::list<std::string>::iterator rfiddeviceid = rfiddeviceidList->begin(); rfiddeviceid != rfiddeviceidList->end(); rfiddeviceid++)
            {
                rollcall->rfiddeviceids.push_back(convert_string_to_int((*rfiddeviceid)));
            }
            rfiddeviceidList->clear();
            delete rfiddeviceidList;

            std::string nfclocators = res->getString("nfclocators");
            std::list<std::string> * nfclocatorList = convert_string_to_list(nfclocators, ',');
            for (std::list<std::string>::iterator nfclocator = nfclocatorList->begin(); nfclocator != nfclocatorList->end(); nfclocator++)
            {
                rollcall->nfclocators.push_back((*nfclocator));
            }
            nfclocatorList->clear();
            delete nfclocatorList;

            std::string subgroups = res->getString("subgroups");
            std::list<std::string> * subgroupList = convert_string_to_list(subgroups, ',');
            for (std::list<std::string>::iterator subgroup = subgroupList->begin(); subgroup != subgroupList->end(); subgroup++)
            {
                rollcall->subgroups.push_back(convert_string_to_int((*subgroup)));
            }
            subgroupList->clear();
            delete subgroupList;

            std::string weekdays = res->getString("weekday");
            std::list<std::string> * weekdayList = convert_string_to_list(weekdays, ',');
            for (std::list<std::string>::iterator weekday = weekdayList->begin(); weekday != weekdayList->end(); weekday++)
            {
                rollcall->weekday.push_back(convert_string_to_int((*weekday)));
            }
            weekdayList->clear();
            delete weekdayList;

            std::string validdates = res->getString("validdate");
            std::list<std::string> * validdateList = convert_string_to_list(validdates, ',');
            for (std::list<std::string>::iterator validdate = validdateList->begin(); validdate != validdateList->end(); validdate++)
            {
                rollcall->validdate.push_back((*validdate));
            }
            validdateList->clear();
            delete validdateList;

            std::string invaliddates = res->getString("invaliddate");
            std::list<std::string> * invaliddateList = convert_string_to_list(invaliddates, ',');
            for (std::list<std::string>::iterator invaliddate = invaliddateList->begin(); invaliddate != invaliddateList->end(); invaliddate++)
            {
                rollcall->invaliddate.push_back((*invaliddate));
            }
            invaliddateList->clear();
            delete invaliddateList;

            rollcall->presentnotify = res->getInt("presentnotify");
            rollcall->presentcomment = res->getString("presentcomment");
            rollcall->absentnotify = res->getInt("absentnotify");
            rollcall->absentcomment = res->getString("absentcomment");
            rollcall->datetime = res->getString("datetime");
            rollcall->updatetime = res->getString("updatetime");
            rollcall->resend = res->getInt("resend");
            rollcall->maingroups = res->getString("maingroups");
            rollcall->type = res->getInt("type");
            rollcall->curriculumrecordid = res->getInt("curriculumrecordid");

            rollcallList->push_back(rollcall);
            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_CARDRollCallEvent_callback(sql::ResultSet * res, void * pvalue)
{
    CARDRollCallEventStruct * rollcallEvent = (CARDRollCallEventStruct *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        if (res->next())
        {
            rollcallEvent->eventid = res->getInt("eventid");
            rollcallEvent->rollcallid = res->getInt("rollcallid");
            rollcallEvent->rollcallname = res->getString("rollcallname");
            rollcallEvent->StartDateTime = res->getString("StartDateTime");
            rollcallEvent->EndDateTime = res->getString("EndDateTime");
            rollcallEvent->totalcount = res->getInt("totalcount");
            rollcallEvent->presentcount = res->getInt("presentcount");
            rollcallEvent->datetime = res->getString("datetime");
            rollcallEvent->updatetime = res->getString("updatetime");
            rollcallEvent->maingroups = res->getString("maingroups");

            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_CARDRollCallRecordStructList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<CARDRollCallRecordStruct*> * recordList = (std::list<CARDRollCallRecordStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            CARDRollCallRecordStruct * record = new CARDRollCallRecordStruct();
            record->rollcallid = res->getInt("rollcallid");
            record->rollcalleventid = res->getInt("rollcalleventid");
            record->cardid = res->getString("cardid");
            record->userid = res->getInt("userid");
            record->nodeid = res->getString("nodeid");

            if ( res->findColumn("rfiddeviceids") > 0 )
            {
                std::string rfiddeviceids = res->getString("rfiddeviceids");
                std::list<std::string> * rfiddeviceidList = convert_string_to_list(rfiddeviceids, ',');
                for (std::list<std::string>::iterator rfiddeviceid = rfiddeviceidList->begin(); rfiddeviceid != rfiddeviceidList->end(); rfiddeviceid++)
                {
                    record->rfiddeviceids.push_back(convert_string_to_int((*rfiddeviceid)));
                }
                rfiddeviceidList->clear();
                delete rfiddeviceidList;
            }

            if ( res->findColumn("nfclocators") > 0 )
            {
                std::string nfclocators = res->getString("nfclocators");
                std::list<std::string> * nfclocatorList = convert_string_to_list(nfclocators, ',');
                for (std::list<std::string>::iterator nfclocator = nfclocatorList->begin(); nfclocator != nfclocatorList->end(); nfclocator++)
                {
                    record->nfclocators.push_back((*nfclocator));
                }
                nfclocatorList->clear();
                delete nfclocatorList;
            }

            //record->rfiddeviceid = res->getInt("rfiddeviceid");
            //record->nfclocator = res->getString("nfclocator");

            record->present = res->getInt("present");
            record->datetime = res->getString("datetime");
            record->updatetime = res->getString("updatetime");
            record->maingroups = res->getString("maingroups");

            recordList->push_back(record);
            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_CARDScanRecordStructList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<CARDScanRecordStruct*> * recordList = (std::list<CARDScanRecordStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            CARDScanRecordStruct * record = new CARDScanRecordStruct();
            record->id = res->getInt("id");
            record->cardtype = res->getInt("cardtype");
            record->cardid = res->getString("cardid");
            record->LoRAMacaddress = res->getString("loramacaddress");
            record->ssid = res->getString("ssid");
            record->wifimac = res->getString("wifimac");
            record->datetime = res->getString("datetime");

            recordList->push_back(record);
            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_TargetTopicStructList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<TargetTopicStruct*> * recordList = (std::list<TargetTopicStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            TargetTopicStruct * record = new TargetTopicStruct();

            record->target = res->getString("target");
            record->topic = res->getString("topic");
            record->name = res->getString("name");
            record->accountid = res->getString("accountid");
            record->datetime = res->getString("updatetime");
            record->maingroupid = res->getInt("maingroupid");
            record->userid = res->getInt("userid");

            recordList->push_back(record);
            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_TargetTopicStruct_callback(sql::ResultSet * res, void * pvalue)
{
    TargetTopicStruct * record = (TargetTopicStruct *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        if (res->next())
        {
            record->target = res->getString("target");
            record->topic = res->getString("topic");
            record->name = res->getString("name");
            record->accountid = res->getString("accountid");
            record->datetime = res->getString("updatetime");
            record->maingroupid = res->getInt("maingroupid");
            record->userid = res->getInt("userid");

            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_SurveyResult_callback(sql::ResultSet * res, void * pvalue)
{
    GAESurveyResultData * record = (GAESurveyResultData *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        if (res->next())
        {
            record->hashkey = "";
            record->token = "";
            record->topic = "";
            record->messagehashkey = "";
            record->comment = "";
            record->datetime = "";

            if ( res->findColumn("hashkey") > 0 )
                record->hashkey = res->getString("hashkey");
            if ( res->findColumn("token") > 0 )
                record->token = res->getString("token");
            if ( res->findColumn("topic") > 0 )
                record->topic = res->getString("topic");
            if ( res->findColumn("messagehashkey") > 0 )
                record->messagehashkey = res->getString("messagehashkey");
            if ( res->findColumn("comment") > 0 )
                record->comment = res->getString("comment");
            if ( res->findColumn("updatetime") > 0 )
                record->datetime = res->getString("updatetime");
            std::string results = res->getString("results");

            std::list<std::string> * resultList = convert_string_to_list(results, ',');
            for (std::list<std::string>::iterator result = resultList->begin(); result != resultList->end(); result++)
            {
                record->results.push_back((*result));
            }
            resultList->clear();
            delete resultList;

            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_VILSConfiglist_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<VILSConfig*> * configList = (std::list<VILSConfig*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            VILSConfig * config = new VILSConfig();
            config->projectid = res->getInt("PROJECT_ID");
            config->projectname = res->getString("PROJECT_NAME");
            config->projectdesc = res->getString("PROJECT_DESCRIPTION");
            // config->iot_userid = res->getString("IOT_USER_ID");
            // config->iot_projectid = res->getString("IOT_PROJECT_ID");
            // config->iot_serverurl = res->getString("IOT_SERVER");
            // config->iot_serverport = res->getString("IOT_SERVER_PORT");
            // config->iot_access_token = res->getString("IOT_PROJECT_ACCESS_TOKEN");
            // config->uploadresult = res->getString("UPLOAD_RESULT");
            config->WGS48_Origin_X = res->getString("WGS48OriginX");
            config->WGS48_Origin_Y = res->getString("WGS48OriginY");
            config->MAP_North = res->getString("MAP_North");
            // config->Building_Origin_X = res->getString("BuildingOriginX");
            // config->Building_Origin_Y = res->getString("BuildingOriginY");
            // config->Building_X_Length = res->getString("BuildingXLength");
            // config->Building_Y_Length = res->getString("BuildingYLength");
            config->BuildingID = res->getInt("BuildingID");
            config->BuildingFloor = res->getInt("BuildingFloor");
            config->datetime = res->getString("datetime");

            configList->push_back(config);
            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_map_callback(sql::ResultSet * res, void * pvalue)
{
    MapStruct * map = (MapStruct *)pvalue;

    if ( res != NULL)
    {
        if(res->next())
        {
            map->projectid = res->getInt("PROJECT_ID");
            map->mapid = res->getInt("mapid");
            map->mapname = res->getString("mapname");
            map->POSX = res->getString("POSX");
            map->POSY = res->getString("POSY");
            map->SCALE = res->getString("SCALE");
            map->COEFF = res->getString("COEFF");
            map->FILE = res->getString("FILE");
            map->MAPWIDTH = res->getString("MAPWIDTH");
            map->MAPHEIGHT = res->getString("MAPHEIGHT");
            return 1;
        }
        else
        {
            vilslog::printf("MySQLDBAccess::get_map_callback() res->next() false\n");
            return 0;
        }
    }
    else
    {
        vilslog::printf("MySQLDBAccess::get_map_callback() res == NULL\n");
    }
    return 0;
}

int MySQLDBAccess::get_arealist_callback(sql::ResultSet * res, void * pvalue)
{
   std::list<AreaStruct*> * areaList = (std::list<AreaStruct*> *)pvalue;

   int count = 0;
   if ( res != NULL)
   {
        while (res->next())
        {
            AreaStruct * area = new AreaStruct();
            area->areaid = res->getString("areaid");
            area->projectid = res->getInt("PROJECT_ID");
            area->areaname = res->getString("areaname");
            area->datetime = res->getString("datetime");
            area->geojson = res->getString("geojson");
            area->Polygons = getPolygonPoints(res->getString("geojson"));
            area->type = res->getInt("type");
            area->WGS48_Origin_X = 0.0;
            area->WGS48_Origin_Y = 0.0;
            areaList->push_back(area);
            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_AssistStruct_callback(sql::ResultSet * res, void * pvalue)
{
   AssistStruct * assistinfo = (AssistStruct *)pvalue;

   int count = 0;
   if ( res != NULL)
   {
        if (res->next())
        {
            assistinfo->id = res->getInt("id");
            assistinfo->userid = res->getInt("userid");
            assistinfo->title = res->getString("title");
            assistinfo->description = res->getString("description");
            assistinfo->subgroups = res->getString("subgroups");
            assistinfo->assignusers = res->getString("assignusers");
            assistinfo->maingroups = res->getString("maingroups");
            // assistinfo->accountid = res->getString("accountid");
            // assistinfo->assignaccounts = res->getString("assignaccounts");
            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_AssistStructList_callback(sql::ResultSet * res, void * pvalue)
{
   std::list<AssistStruct*> * assistlist = (std::list<AssistStruct*> *)pvalue;

   int count = 0;
   if ( res != NULL)
   {
        while(res->next())
        {
            AssistStruct * assistinfo = new AssistStruct();
            assistinfo->id = res->getInt("id");
            assistinfo->userid = res->getInt("userid");
            // assistinfo->accountid = res->getString("accountid");
            assistinfo->title = res->getString("title");
            assistinfo->description = res->getString("description");
            assistinfo->subgroups = res->getString("subgroups");
            assistinfo->assignusers = res->getString("assignusers");
            assistinfo->maingroups = res->getString("maingroups");
            // assistinfo->assignaccounts = res->getString("assignaccounts");
            assistlist->push_back(assistinfo);
            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_AssistEventStruct_callback(sql::ResultSet * res, void * pvalue)
{
   AssistEventStruct * event = (AssistEventStruct *)pvalue;

   int count = 0;
   if ( res != NULL)
   {
        if (res->next())
        {
            event->id = res->getInt("id");
            event->assistid = res->getInt("assistid");
            event->userid = res->getInt("userid");
            event->description = res->getString("description");
            // event->accountid = res->getString("accountid");
            // event->eventtime = res->getString("eventtime");
            // event->weekday = res->getString("weekday");
            // event->validdate = res->getString("validdate");
            // event->invaliddate = res->getString("invaliddate");
            std::string eventtime = res->getString("eventtime");
            std::list<std::string> * eventtimeList = convert_string_to_list(eventtime, ',');
            for (std::list<std::string>::iterator daytime = eventtimeList->begin(); daytime != eventtimeList->end(); daytime++)
            {
                event->eventtime.push_back((*daytime));
            }
            eventtimeList->clear();
            delete eventtimeList;

            std::string weekdays = res->getString("weekday");
            std::list<std::string> * weekdayList = convert_string_to_list(weekdays, ',');
            for (std::list<std::string>::iterator weekday = weekdayList->begin(); weekday != weekdayList->end(); weekday++)
            {
                event->weekday.push_back(convert_string_to_int((*weekday)));
            }
            weekdayList->clear();
            delete weekdayList;

            std::string validdates = res->getString("validdate");
            std::list<std::string> * validdateList = convert_string_to_list(validdates, ',');
            for (std::list<std::string>::iterator validdate = validdateList->begin(); validdate != validdateList->end(); validdate++)
            {
                event->validdate.push_back((*validdate));
            }
            validdateList->clear();
            delete validdateList;

            std::string invaliddates = res->getString("invaliddate");
            std::list<std::string> * invaliddateList = convert_string_to_list(invaliddates, ',');
            for (std::list<std::string>::iterator invaliddate = invaliddateList->begin(); invaliddate != invaliddateList->end(); invaliddate++)
            {
                event->invaliddate.push_back((*invaliddate));
            }
            invaliddateList->clear();
            delete invaliddateList;

            event->deadline = res->getString("deadline");
            event->imageids = res->getString("imageids");
            event->fileids = res->getString("fileids");
            event->applyusername = res->getString("applyusername");
            event->applyuserrelation = res->getString("applyuserrelation");
            // event->signoffaccountid = res->getString("signoffaccountid");
            event->signoffuserid = res->getInt("signoffuserid");
            event->maingroups = res->getString("maingroups");
            event->signoffstatus = res->getInt("signoffstatus");
            event->datetime = res->getString("datetime");
            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_AssistEventStructList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<AssistEventStruct*> * assisteventlist = (std::list<AssistEventStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while(res->next())
        {
            AssistEventStruct * event = new AssistEventStruct();
            event->id = res->getInt("id");
            event->assistid = res->getInt("assistid");
            event->userid = res->getInt("userid");
            event->description = res->getString("description");
            // event->accountid = res->getString("accountid");
            // event->eventtime = res->getString("eventtime");
            // event->weekday = res->getString("weekday");
            // event->validdate = res->getString("validdate");
            // event->invaliddate = res->getString("invaliddate");

            std::string eventtime = res->getString("eventtime");
            std::list<std::string> * eventtimeList = convert_string_to_list(eventtime, ',');
            for (std::list<std::string>::iterator daytime = eventtimeList->begin(); daytime != eventtimeList->end(); daytime++)
            {
                event->eventtime.push_back((*daytime));
            }
            eventtimeList->clear();
            delete eventtimeList;

            std::string weekdays = res->getString("weekday");
            std::list<std::string> * weekdayList = convert_string_to_list(weekdays, ',');
            for (std::list<std::string>::iterator weekday = weekdayList->begin(); weekday != weekdayList->end(); weekday++)
            {
                event->weekday.push_back(convert_string_to_int((*weekday)));
            }
            weekdayList->clear();
            delete weekdayList;

            std::string validdates = res->getString("validdate");
            std::list<std::string> * validdateList = convert_string_to_list(validdates, ',');
            for (std::list<std::string>::iterator validdate = validdateList->begin(); validdate != validdateList->end(); validdate++)
            {
                event->validdate.push_back((*validdate));
            }
            validdateList->clear();
            delete validdateList;

            std::string invaliddates = res->getString("invaliddate");
            std::list<std::string> * invaliddateList = convert_string_to_list(invaliddates, ',');
            for (std::list<std::string>::iterator invaliddate = invaliddateList->begin(); invaliddate != invaliddateList->end(); invaliddate++)
            {
                event->invaliddate.push_back((*invaliddate));
            }
            invaliddateList->clear();
            delete invaliddateList;

            event->deadline = res->getString("deadline");
            event->imageids = res->getString("imageids");
            event->fileids = res->getString("fileids");
            event->applyusername = res->getString("applyusername");
            event->applyuserrelation = res->getString("applyuserrelation");
            // event->signoffaccountid = res->getString("signoffaccountid");
            event->signoffuserid = res->getInt("signoffuserid");
            event->maingroups = res->getString("maingroups");
            event->signoffstatus = res->getInt("signoffstatus");
            event->datetime = res->getString("datetime");

            assisteventlist->push_back(event);

            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_AssistEventRecordStruct_callback(sql::ResultSet * res, void * pvalue)
{
  AssistEventRecordStruct * assisteventrecord  = (AssistEventRecordStruct *)pvalue;

   int count = 0;
   if ( res != NULL)
   {
        if (res->next())
        {
            assisteventrecord->id = res->getInt("id");
            assisteventrecord->assistid = res->getInt("assistid");
            assisteventrecord->assisteventid = res->getInt("assisteventid");
            assisteventrecord->PushNotifyMessageRecorddbId = res->getInt("PushNotifyMessageRecorddbId");
            assisteventrecord->userid = res->getInt("userid");

            // if ( res->findColumn("accountid") > 0 )
            //     assisteventrecord->accountid = res->getString("accountid");
            assisteventrecord->messagehashkey = res->getString("messagehashkey");

            std::string results = res->getString("results");
            std::list<std::string> * resultList = convert_string_to_list(results, ',');
            for (std::list<std::string>::iterator result = resultList->begin(); result != resultList->end(); result++)
            {
                assisteventrecord->results.push_back((*result));
            }
            resultList->clear();
            delete resultList;

            assisteventrecord->eventday = res->getString("eventday");
            assisteventrecord->eventtime = res->getString("eventtime");
            assisteventrecord->maingroups = res->getString("maingroups");
            assisteventrecord->datetime = res->getString("datetime");
            assisteventrecord->updatetime = res->getString("updatetime");
            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_SensorDeviceList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<SensorDeviceStruct*> * recordList = (std::list<SensorDeviceStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            SensorDeviceStruct * device = new SensorDeviceStruct();

            device->alive = res->getInt("alive");
            device->maingroupid = res->getInt("maingroupid");
            device->id = res->getString("id");
            device->name = res->getString("name");
            device->type = res->getString("type");
            device->jsonvalue = res->getString("jsonvalue");
            device->Longitude = res->getString("Longitude");
            device->Latitude = res->getString("Latitude");
            device->Battery = res->getString("Battery");
            device->Version = res->getString("Version");
            device->datetime = res->getString("datetime");
            device->unknownLocation = 1;
            device->updateLocationTime = getCurrentTimeInSec();

            device->aliveTime = 0;
            if (device->alive > 0)
                device->aliveTime = getCurrentTimeInSec();

            device->verifykey = "";
            if ( res->findColumn("verifykey") > 0 )
                device->verifykey = res->getString("verifykey");

            device->value_buffer_size = 0;
            device->value_buffer_index = 0;
            for(int i=0;i<SENSOR_VALUE_BUFFER_SIZE;i++)
            {
                device->value_buffer[i] = "0";
                device->value_datetime[i] = "";
            }

            recordList->push_back(device);

            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_FlyInfoList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<FlyInfoStruct*> * recordList = (std::list<FlyInfoStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            FlyInfoStruct * record = new FlyInfoStruct();

            record->FDATE = res->getString("FDATE");
            record->expectTime = res->getString("expectTime");
            record->airLineNum = res->getString("airLineNum");
            record->airLineCode = res->getString("airLineCode");
            record->type = res->getString("type");
            record->jsonvalue = res->getString("jsonvalue");
            record->triggerTime = res->getString("triggerTime");
            record->datetime = res->getString("datetime");
            record->maingroupid = res->getInt("maingroupid");
            record->sensorid = res->getString("sensorid");
            record->airBoardingGate = res->getString("airBoardingGate");

            recordList->push_back(record);

            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_FlyGateInfoList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<FlyGateInfoStruct*> * recordList = (std::list<FlyGateInfoStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            FlyGateInfoStruct * record = new FlyGateInfoStruct();

            record->airLineNum = res->getString("airLineNum");
            record->airLineCode = res->getString("airLineCode");
            record->airBoardingGate = res->getString("airBoardingGate");

            recordList->push_back(record);

            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_FireFightingList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<FireFightingStruct*> * pList = (std::list<FireFightingStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            FireFightingStruct * record = new FireFightingStruct();

            record->id = res->getInt("id");
            record->maingroupid = res->getInt("maingroupid");
            record->caseclosed = res->getInt("caseclosed");
            record->casenumber = res->getString("casenumber");
            record->address = res->getString("address");
            record->lat = res->getString("lat");
            record->lng = res->getString("lng");
            record->starttime = res->getString("starttime");
            record->closetime = res->getString("closetime");
            record->datetime = res->getString("datetime");

            std::string StandbyDeviceIds = res->getString("StandbyDeviceIds");
            std::string NFCDeviceIds = res->getString("NFCDeviceIds");

            record->StandbyDeviceIds = convert_string_to_list(StandbyDeviceIds, ',');
            record->NFCDeviceIdList = convert_string_to_intlist(NFCDeviceIds, ',');

            pList->push_back(record);

            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_FireFightingUserList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<FireFightingUserStruct*> * pList = (std::list<FireFightingUserStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            FireFightingUserStruct * record = new FireFightingUserStruct();
            record->userid = res->getInt("userid");
            record->casenumber = res->getString("casenumber");
            record->maingroupid = res->getInt("maingroupid");
            record->areaType = res->getInt("areaType");
            std::string airremain = res->getString("airremain");
            std::string enterTime = res->getString("enterTime");
            record->airremain = parseFloat(airremain);
            record->enterTime = StringDateTimeTotime_t(enterTime);
            record->activity = res->getString("activity");
            record->HR = res->getString("HR");
            record->SPO2 = res->getString("SPO2");
            record->battery = res->getString("battery");
            pList->push_back(record);

            count++;
        }

    }
    return count;
}

int MySQLDBAccess::get_FireFightingEventList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<FireFightingEventStruct*> * pList = (std::list<FireFightingEventStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            FireFightingEventStruct * record = new FireFightingEventStruct();
            record->id = res->getInt("id");
            record->casenumber = res->getString("casenumber");
            record->eventtype = res->getString("eventtype");
            record->jsonstring = res->getString("jsonstring");
            record->datetime = res->getString("datetime");
            pList->push_back(record);

            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_FireFightingAirUsing_callback(sql::ResultSet * res, void * pvalue)
{
    FireFightingAirUsingStruct * record = (FireFightingAirUsingStruct *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            record->userid = res->getInt("userid");
            record->casenumber = res->getString("casenumber");
            record->startairremain = parseFloat(res->getString("startairremain"));
            record->starttime = StringDateTimeTotime_t(res->getString("starttime"));
            record->endairremain = parseFloat(res->getString("endairremain"));
            record->endtime = StringDateTimeTotime_t(res->getString("endtime"));
            record->airusing = parseFloat(res->getString("airusing"));
            record->totaltime = convert_string_to_int(res->getString("totaltime"));

            count++;
        }
    }
    return count;
}

int MySQLDBAccess::get_SoccerRecordList_callback(sql::ResultSet * res, void * pvalue)
{
    std::list<SoccerRecordingStruct*> * pList = (std::list<SoccerRecordingStruct*> *)pvalue;

    int count = 0;
    if ( res != NULL)
    {
        while (res->next())
        {
            SoccerRecordingStruct * record = new SoccerRecordingStruct();
            record->recordid = res->getInt("recordid");
            record->maingroupid = res->getInt("maingroupid");
            record->recordnumber = res->getString("recordnumber");
            record->starttime = StringDateTimeTotime_t(res->getString("starttime"));
            record->endtime = StringDateTimeTotime_t(res->getString("endtime"));
            pList->push_back(record);

            vilslog::printf("MySQLDBAccess::get_SoccerRecordList_callback() recordid[%d] maingroupid[%d] recordnumber[%s] starttime[%s] endtime[%s] starttime[%d] endtime[%d]\n",
                record->recordid, record->maingroupid, record->recordnumber.c_str(),
                res->getString("starttime").c_str(), res->getString("endtime").c_str(), record->starttime, record->endtime);

            count++;
        }
    }
    return count;
}

void MySQLDBAccess::printSQLException(sql::SQLException &e)
{
    std::ostringstream sendstream;
    sendstream << "# ERR: " << e.what();
    sendstream << " (MySQL error code: " << e.getErrorCode();
    sendstream << ", SQLState: " << e.getSQLState() << " )" << endl;
    vilslog::printf("sql::SQLException [%s]\n", sendstream.str().c_str());

    // cout << "# ERR: " << e.what();
    // cout << " (MySQL error code: " << e.getErrorCode();
    // cout << ", SQLState: " << e.getSQLState() << " )" << endl;

}

std::string MySQLDBAccess::get_role_default_functions(int nodetype, int role)
{
    std::string functions = "";

    if (nodetype == NODE_TYPE_TAG)
    {
        // Tag role 0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師

        if ( role == 0 || role == 3 || role == 4 || role == 5 || role == 6 )
        {
            functions = "1,2,3,4,5,7,8,9,10";
        }
        else
        if ( role == 1 )
        {
            functions = "1,2,3,5,10";
        }
        else
        if ( role == 2 )
        {
            functions = "1,3,7,11";
        }
    }
    else
    if (nodetype == NODE_TYPE_LOCATOR)
    {
        // Locator Button Type 0:Alert 1:CallOut 2:Door 3:訊號採集器(AB box) 4:巡檢器
        if ( role == 0 || role == 1 )
        {
            functions = "101,102,103,104";
        }
        else
        {
            functions = "101,102,103";
        }
    }

    return functions;
}
