/*
 *  CARDRollCallService.h
 *
 *  Created on: 2020/05/04
 *      Author: ikki
 */
#ifndef __CARDRollCallService_h__
#define __CARDRollCallService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../common.h"
#include "../config.h"
#include "../network/DataItem.h"
#include "../network/WorkItem.h"
#include "../network/NotificationItem.h"
#include "../MQTT/MQTTDataItem.h"
#include "../mysql/DBManager.h"
#include "../LLRP/LLRPController.h"
#include "../DeviceManager/DeviceManager.h"

#define ZEBRA_RX7500_LOGIN_NAME "admin"
#define ZEBRA_RX7500_LOGIN_PASSWORD "WmKA8#vjRJE2qJ"

using namespace std;

typedef struct
{
    std::string cardid;
    std::string gwid;
    std::string loramacaddress;
    int rollcallcount;
    std::string datetime;
} RollCallCardStruct;

typedef struct
{
    int userid;
    std::list<std::string> rfidcards;
    std::list<std::string> nfccards;
    std::string name;
    std::string account;
    int present;
} UserCardStruct;

typedef struct
{
    int rollcallid;
    CARDRollCallEventStruct * mRollCallEvent;
    CARDRollCallTriggerStruct * mTrigger;
    std::list<UserCardStruct*> * mUserCardList;
    std::list<CARDRollCallRecordStruct*> * mRollCallRecordList;
    std::list<CARDRollCallSubGroupEventStruct*> * mRollCallSubGroupList;
} RollCallInstance;

typedef struct
{
    int projectid;
    std::list<RollCallInstance*> * mRollCallInstanceList;
    std::list<CARDRollCallStruct*> * mRollCallList;
    std::list<RFIDDeviceStruct*> * mRFIDDeviceList;
    std::list<NFCDeviceStruct*> * mNFCDeviceList;
    std::list<CARDRollCallTriggerStruct*> * mTriggerList;
    std::list<SectionGroup *> * mSection0DGroupList;

} ProjectRollCallInfoStruct;

class CARDRollCallService : public Thread
{
public:
    CARDRollCallService(wqueue<NotificationItem*> & NotifiactionQueue, wqueue<DataItem*> & CHTIoTJobQueue, wqueue<MQTTDataItem*> & MQTTPublishQueue, wqueue<DataItem*> & CARDRollCallJobQueue, wqueue<WorkItem*> & MySQLJobQueue, DeviceManager * pDevManager, DBManager * dbManager);
    ~CARDRollCallService();
    bool status();
    std::string statusString();

    void * run();

private:
    bool mInitial;
    std::string mStatus;

    bool mRollCallRunning;
    std::vector<ProjectRollCallInfoStruct *> mProjectRollCallInfoVector;

    // bool mWaitAck_StopScanRFIDCard;
    // int mWaitAck_projectid;
    // int mWaitAck_trycount;
    // RFIDDeviceStruct mWaitAck_device;

    wqueue<NotificationItem*> & mNotifiactionQueue;
    wqueue<DataItem*> & mCHTIoTJobQueue;
    wqueue<MQTTDataItem*> & mMQTTPublishQueue;
    wqueue<DataItem*> & mCARDRollCallJobQueue;
    wqueue<WorkItem*> & mMySQLJobQueue;
    DeviceManager * mDeviceManager;
    DBManager * mDbManager;

    std::list<RollCallCardStruct*> mTempRFIDCardList;
    std::list<RollCallCardStruct*> mTempNFCCardList;

    void runloop();
    void initProjectRollCallInfo();
    void reCheckRollCall(int projectid, std::string day, int rollcallid);

    int processDataItem(DataItem * dataItem);

    void check_LLRPInstance();
    void CheckProjectRollCallEvent();
    bool CheckTodayNeedRollCall(CARDRollCallStruct * rollcall, std::string todayDate, int todayWeekday);
    int CheckRollCallEvent(ProjectRollCallInfoStruct * prjRollCallInfo, CARDRollCallStruct * rollcall);
    void RollCallEvent_outtime(ProjectRollCallInfoStruct * prjRollCallInfo, CARDRollCallStruct * rollcall, time_t t_diff);
    void RollCallEvent_intime(ProjectRollCallInfoStruct * prjRollCallInfo, CARDRollCallStruct * rollcall, time_t t_diff);
    int check_RFID_Card_Present(ProjectRollCallInfoStruct * prjRollCallInfo, RollCallInstance * rollcallInstance, CARDRollCallStruct * rollcall);
    int check_NFC_Card_Present(ProjectRollCallInfoStruct * prjRollCallInfo, RollCallInstance * rollcallInstance, CARDRollCallStruct * rollcall);
    void RollCallEvent_exceedtime(ProjectRollCallInfoStruct * prjRollCallInfo, CARDRollCallStruct * rollcall, time_t t_diff);

    bool check_user_rfid_card_rollcall(ProjectRollCallInfoStruct * prjRollCallInfo, RollCallInstance * rollcallInstance, CARDRollCallStruct * rollcall, int deviceid, std::string cardid, UserCardStruct * UserCard, std::string datetime);
    bool check_user_nfc_card_rollcall(ProjectRollCallInfoStruct * prjRollCallInfo, RollCallInstance * rollcallInstance, CARDRollCallStruct * rollcall, std::string macaddress, std::string cardid, UserCardStruct * UserCard, std::string datetime);

    bool check_RollCallEvent_ready(ProjectRollCallInfoStruct * prjRollCallInfo, CARDRollCallStruct * rollcall);
    bool check_RollCallEvent_UserCardList(ProjectRollCallInfoStruct * prjRollCallInfo, CARDRollCallStruct * rollcall, RollCallInstance * rollcallInstance);
    bool check_match_subgroup(std::list<std::string> * src_subgroups, std::list<int> * dst_subgroups);
    int check_existed_CARDRollCallRecord(ProjectRollCallInfoStruct * prjRollCallInfo, CARDRollCallStruct * rollcall, RollCallInstance * rollcallInstance, int userid, std::string cardid, std::list<CARDRollCallRecordStruct*> * recordList);


    bool updateRollCallRecord(int projectid, CARDRollCallRecordStruct * record);
    int CheckRollCallEvent_time(CARDRollCallStruct * rollcall);

    RollCallInstance * getRollCallInstance(ProjectRollCallInfoStruct * prjRollCallInfo, int rollcallid);
    // LLRPController * getLLRPController(RollCallInstance * rollcallInstance, RFIDDeviceStruct * rfidDevice);
    RFIDDeviceStruct * getRFIDDevice(std::list<RFIDDeviceStruct*> * RFIDDeviceList, int deviceid);
    NFCDeviceStruct * getNFCDevice(std::list<NFCDeviceStruct*> * NFCDeviceList, std::string macaddress);
    int getRFIDDeviceid(std::list<RFIDDeviceStruct*> * RFIDDeviceList, std::string gwid, std::string loramacaddress, std::string maingroups);
    std::string getNFCDeviceMacaddress(std::list<NFCDeviceStruct*> * NFCDeviceList, std::string gwid, std::string loramacaddress, std::string maingroups);
    CARDRollCallRecordStruct * getRollCallRecord(std::list<CARDRollCallRecordStruct*> * RollCallRecordList, int rollcallid, int eventid, int userid, std::string cardid);

    bool put_to_TempRFIDCardlist(std::string cardid, std::string gwid, std::string loramacaddress, std::string datetime);
    bool put_to_TempNFCCardlist(std::string cardid, std::string gwid, std::string loramacaddress, std::string datetime);
    bool do_RFIDCard_section_0D_check(int projectid, std::string cardid, std::string loramacaddress);
    bool do_NFCCard_section_0D_check(int projectid, std::string cardid, std::string loramacaddress);
    int update_section0D(int projectid, int userid, int maingroupid, std::string AreaID, int Param2);

    void reloadRollCallList();
    bool reloadSectionList();

    bool send_MQTT_cmd(int projectid, std::string cmd, RFIDDeviceStruct * device);
    bool send_MQTT_cmd(int projectid, std::string cmd, NFCDeviceStruct * device);

    void clear_ProjectRollCallInfoStruct_vector(std::vector<ProjectRollCallInfoStruct*> * plist);
    void clear_ProjectRollCallInfoStruct(ProjectRollCallInfoStruct * info);
    void clear_RollCallInstance_list(std::list<RollCallInstance*> * plist);
    void clear_CARDRollCallStruct_list(std::list<CARDRollCallStruct*> * plist);
    void clear_RFIDDeviceStruct_list(std::list<RFIDDeviceStruct*> * plist);
    void clear_UserCardStruct_list(std::list<UserCardStruct*> * plist);
    void clear_RollCallCardStruct_list(std::list<RollCallCardStruct*> * plist);

    std::list<CARDRollCallTriggerStruct*> * create_Triggers();
    CARDRollCallTriggerStruct * create_Trigger_1();
    CARDRollCallTriggerStruct * create_Trigger_2();
    CARDRollCallTriggerStruct * create_Trigger_3();
    CARDRollCallTriggerItemStruct * create_Trigger_item(std::string TargetTime, float NormDist);

    CARDRollCallTriggerStruct * find_trigger(ProjectRollCallInfoStruct * prjRollCallInfo, std::string rollcallname);
    bool check_trigger(int projectid, RollCallInstance * rollcallInstance);

    bool set_subgroupevent(UserStruct * user, std::list<CARDRollCallSubGroupEventStruct*> * rollcallSubGroupList);
    bool set_present_subgroupevent(int userid, std::list<CARDRollCallSubGroupEventStruct*> * rollcallSubGroupList);

    AreaCenterStruct * getAreaCenter(int projectid, MapLayerStruct * maplayer, MapLayerAreaStruct * area, UserStruct * userRet);
    bool find_empty_0D_position(int projectid, int userid, int startX, int startY, int * newPosition);
};

#endif
