/*
 *  PushNotificationService.cpp
 *
 *  Created on: 2019/08/02
 *      Author: ikki
 */


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <iostream>

#include <netdb.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/errno.h>

#include "PushNotificationService.h"

using namespace std;

PushNotificationService::PushNotificationService(wqueue<DataItem*> & mainJobQueue, wqueue<NotificationItem*> & queue, wqueue<WorkItem*> & mysqlJobQueue, DeviceManager * pDevManager, DBManager * dbManager)
    : mMainJobQueue(mainJobQueue), m_queue(queue), mMySQLJobQueue(mysqlJobQueue), mDeviceManager(pDevManager), mDbManager(dbManager)
{
}

bool PushNotificationService::status()
{
    return mInitial;
}

std::string PushNotificationService::statusString()
{
    return mStatus;
}

void PushNotificationService::updateProject()
{
    mUpdateProject = true;
}

void * PushNotificationService::run()
{
    while(1)
    {
        try
        {
            mPrjidList = mDbManager->getAllProjectConfig();

            reloadPushNotifySetting();

            runloop();
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("PushNotificationService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "PushNotificationService()::run() exception:";
            mStatus.append(exc.what());
        }

        clear_FunctionSendTimeStruct_list(&mFunctionSendTimeList);
    }
    return NULL;
}

void PushNotificationService::runloop()
{
    mInitial = true;
    mStatus = "PushNotificationService()::run() Initial PushNotificationService successfully!";
    vilslog::printf ("PushNotificationService()::run() Initial PushNotificationService successfully!\n");

    waitloop();
}


// http://smimsvils.appspot.com/sendmessage?title=SMIMSVILS推播通知&body=Tag0025 進入巡檢點 Locator0005&topic=smimsvils-android&datetime=201908021104


void PushNotificationService::waitloop()
{
    long long start1 = getSystemTime();
    long long end1 = getSystemTime();
    long long difft = end1 - start1;
    long long updateTime = 3590000;
    long long updatePrjIdTime = 0;

    while ( 1 )
    {
        NotificationItem * item = m_queue.removetimed(5000);
        if (item != NULL)
        {
            if ( item->m_type.compare("updatePushNotify") == 0 )
            {
                vilslog::printf ("PushNotificationService()::waitloop() updatePushNotify \n");
                reloadPushNotifySetting();
            }
            else
            {
                VILSSystemConfig systemConfig;
                int projectid = item->m_projectID;

                VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);

                if (vilssysConfig->EnablePushNotify == 1)
                {
                    // vilslog::printf ("PushNotificationService()::waitloop() m_functiontype[%d] m_title[%s] m_message.length[%d]\n",
                    //     item->m_functiontype, item->m_title.c_str(), item->m_message.length());

                    do_check_function(item, vilssysConfig);

                }
                else
                {
                    // vilslog::printf ("PushNotificationService()::waitloop() m_type[%s] m_title[%s] m_topic[%s] m_message[%d] !!!! DISABLE !!!! Not Send!!!\n",
                    //     item->m_type.c_str(), item->m_title.c_str(), item->m_topic.c_str(), item->m_message.length());
                    // vilslog::printf ("PushNotificationService()::waitloop() m_type[%s] m_title[%s] m_topic[%s] \n\t m_message[%s]\n\t !!!! DISABLE !!!! Not Send!!!\n",
                    //     item->m_type.c_str(), item->m_title.c_str(), item->m_topic.c_str(), item->m_message.c_str());


                    SendNotifyStatusStruct newStatus;
                    newStatus.projectid = item->m_projectID;
                    newStatus.status = 2;// 0:wait finish 1:success 2:failed!
                    newStatus.waitkey = item->m_waitkey;
                    newStatus.message = "Push Notifiaction Disabled!";
                    DataItem * status = new DataItem("SendNotifityStatus", item->m_projectID, &newStatus);
                    mMainJobQueue.add(status);

                }
            }
            delete item;
        }

        end1 = getSystemTime();
        difft = end1 - start1;

        updateTime += difft;
        updatePrjIdTime += difft;

        if ( updateTime >= 3600000) // one hour
        {
            // check_User_Topic();
            reloadPushNotifySetting();
            updateTime = 0;
        }

        if (updatePrjIdTime > 3600000 || mUpdateProject)
        {
            mPrjidList->clear();
            delete mPrjidList;
            mPrjidList = mDbManager->getAllProjectConfig();

            updatePrjIdTime = 0;
            mUpdateProject = false;
        }

        start1 = end1;
    }
}

void PushNotificationService::do_check_function(NotificationItem * item, VILSSystemConfig * vilssysConfig)
{
    //
    // check valid send
    //
    bool validSend = checkValidSendNotifity(item);

    if (!validSend)
    {
        // vilslog::printf ("PushNotificationService()::do_check_function() function[%d] m_title[%s] m_message.length[%d] send time too shortly\n",
        //     item->m_functiontype, item->m_title.c_str(), item->m_message.length());

        SendNotifyStatusStruct newStatus;
        newStatus.projectid = item->m_projectID;
        newStatus.status = 2;// 0:wait finish 1:success 2:failed!
        newStatus.waitkey = item->m_waitkey;
        newStatus.message = "send time too shortly";
        DataItem * status = new DataItem("SendNotifityStatus", item->m_projectID, &newStatus);
        mMainJobQueue.add(status);

        return;
    }

    // vilslog::printf ("PushNotificationService()::do_check_function() function[%d] m_title[%s] m_message.length[%d]\n",
    //     item->m_functiontype, item->m_title.c_str(), item->m_message.length());

    //int count = 0;
    std::list<std::string> * failList;

    std::string messagehashkey = get_unique_hashksy_PushNotifyMessageRecord(item->m_projectID);

    if( item->m_functiontype == MULTI_PUSHNOTIFY_FUNCTION_1 ) // 手動發送
    {
        failList = do_function_MULTI_PUSHNOTIFY_FUNCTION_1(item, vilssysConfig->PushNotifyPrefix, vilssysConfig->VILSUID, messagehashkey);
    }
    else
    if( item->m_functiontype == MULTI_PUSHNOTIFY_FUNCTION_2222 ||  // 請假相關申請
        item->m_functiontype == MULTI_PUSHNOTIFY_FUNCTION_3333 ||  // 選課課程相關申請
        item->m_functiontype == MULTI_PUSHNOTIFY_FUNCTION_201 ||  // 聯絡簿
        item->m_functiontype == MULTI_PUSHNOTIFY_FUNCTION_202 ||  // 師生交流
        item->m_functiontype == MULTI_PUSHNOTIFY_FUNCTION_203 ||  // 電子投票
        item->m_functiontype == MULTI_PUSHNOTIFY_FUNCTION_204 ||  // 工作指示
        item->m_functiontype == MULTI_PUSHNOTIFY_FUNCTION_205 ||  // 不分類
        item->m_functiontype == MULTI_PUSHNOTIFY_FUNCTION_206 ||  // 協助事項回報
        item->m_functiontype == MULTI_PUSHNOTIFY_FUNCTION_207 ) // 公布欄
    {
        failList = do_function_MULTI_PUSHNOTIFY_FUNCTION_1(item, vilssysConfig->PushNotifyPrefix, vilssysConfig->VILSUID, messagehashkey);
    }
    else
    if( item->m_functiontype == TAG_PUSHNOTIFY_FUNCTION_5 ) // 巡檢相關通知, 身份為巡檢員時才有通知
    {
        failList = do_function_TAG_PUSHNOTIFY_FUNCTION_5(item, vilssysConfig->PushNotifyPrefix, vilssysConfig->VILSUID, messagehashkey);
    }
    else
    if( item->m_functiontype == TAG_PUSHNOTIFY_FUNCTION_10 ) // 不特定對象推播
    {
        failList = do_function_TAG_PUSHNOTIFY_FUNCTION_10(item, vilssysConfig->PushNotifyPrefix, vilssysConfig->VILSUID, messagehashkey);
    }
    else
    if( item->m_functiontype == TAG_PUSHNOTIFY_FUNCTION_12 ) // 卡片點名
    {
        failList = do_function_USER_PUSHNOTIFY_FUNCTION(item, TAG_PUSHNOTIFY_FUNCTION_12, vilssysConfig->PushNotifyPrefix, vilssysConfig->VILSUID, messagehashkey);
    }
    else
    if( item->m_functiontype == CARD_PUSHNOTIFY_FUNCTION_1 ) // 單一卡片點名
    {
        failList = do_function_CARD_PUSHNOTIFY_FUNCTION_1(item, vilssysConfig->PushNotifyPrefix, vilssysConfig->VILSUID, messagehashkey);
    }
    else
    if( item->m_functiontype == USER_PUSHNOTIFY_FUNCTION_1 ) // 人員
    {
        failList = do_function_USER_PUSHNOTIFY_FUNCTION(item, USER_PUSHNOTIFY_FUNCTION_1, vilssysConfig->PushNotifyPrefix, vilssysConfig->VILSUID, messagehashkey);
    }
    else
    {
        failList = do_function_other(item, vilssysConfig->PushNotifyPrefix, vilssysConfig->VILSUID, messagehashkey);
    }

    // vilslog::printf ("PushNotificationService()::do_check_function() failList->size()[%d]\n", (int)failList->size());

    SendNotifyStatusStruct newStatus;
    newStatus.projectid = item->m_projectID;
    newStatus.status = 1;// 0:wait finish 1:success 2:failed!
    newStatus.waitkey = item->m_waitkey;
    newStatus.message = "success";

    if ((int)failList->size() > 0)
    {
        std::ostringstream sendstream;
        sendstream<<"無法完成 ";
        for (std::list<std::string>::iterator targetid = failList->begin(); targetid != failList->end(); targetid++)
        {
            std::string nodename = mDeviceManager->getTagName(item->m_projectID, (*targetid).c_str());
            if (nodename.length() == 0)
                nodename = mDeviceManager->getTagName(item->m_projectID, (*targetid).c_str());
            if (nodename.length() == 0)
                nodename = (*targetid);
            sendstream<<" "<<nodename;
        }
        sendstream<<" 訊息發送，請檢查推播設定";
        newStatus.message = sendstream.str();
        newStatus.status = 2;
    }

    DataItem * status = new DataItem("SendNotifityStatus", item->m_projectID, &newStatus);
    mMainJobQueue.add(status);

    failList->clear();
    delete failList;
}

std::list<std::string> * PushNotificationService::do_function_MULTI_PUSHNOTIFY_FUNCTION_1(NotificationItem * item, std::string PushNotifyPrefix, std::string VILSUID, std::string messagehashkey)
{
    std::list<std::string> * failList = new std::list<std::string>();

    std::list<std::string> sendList;
    std::string surveylist = "";
    std::string hashkeys = "";
    std::string deadlinelist = "";
    std::string todolist = "[]";

    // vilslog::printf ("PushNotificationService::do_function_MULTI_PUSHNOTIFY_FUNCTION_1() PushNotifyPrefix[%s] m_surveylist.size()[%d]\n",
    //     PushNotifyPrefix.c_str(), (int)item->m_surveylist.size());

    if (item->m_surveylist.size() > 0)
    {
        std::list<std::string> * hashkeylist = get_unique_hashksy_SurveyEventRecord(item->m_projectID, (int)item->m_surveylist.size());

        surveylist = convertListToString(&item->m_surveylist);
        hashkeys = convertListToString(hashkeylist);
        deadlinelist = convertListToString(&item->m_deadlinelist);
        todolist = convertSurveyToJson(item->m_projectID, &item->m_surveylist, hashkeylist, &item->m_deadlinelist);

        // vilslog::printf ("PushNotificationService::do_function_MULTI_PUSHNOTIFY_FUNCTION_1() surveylist[%s]\n", surveylist.c_str());
        // vilslog::printf ("PushNotificationService::do_function_MULTI_PUSHNOTIFY_FUNCTION_1() hashkeys[%s]\n", hashkeys.c_str());
        // vilslog::printf ("PushNotificationService::do_function_MULTI_PUSHNOTIFY_FUNCTION_1() deadlinelist[%s]\n", deadlinelist.c_str());
        // vilslog::printf ("PushNotificationService::do_function_MULTI_PUSHNOTIFY_FUNCTION_1() todolist[%s]\n", todolist.c_str());

        hashkeylist->clear();
        delete hashkeylist;
    }

    //
    // check tag target
    //
    for (std::list<std::string>::iterator targetid = item->m_sendtargetlist.begin(); targetid != item->m_sendtargetlist.end(); targetid++)
    {
        std::list<std::string> * successList = do_send_target(item->m_projectID, TAG_PUSHNOTIFY_FUNCTION_1, (*targetid), item->m_title, item->m_message, PushNotifyPrefix, VILSUID, item->m_waitkey, todolist, messagehashkey, item->m_imageuid);

        // vilslog::printf ("do_send_target() TAG_PUSHNOTIFY_FUNCTION_1 targetid[%s] successList->size()[%d]\n",(*targetid).c_str(), (int)successList->size());

        if ( successList->size() == 0 )
        {
            successList->clear();
            delete successList;

            successList = do_send_target(item->m_projectID, LOCATOR_PUSHNOTIFY_FUNCTION_1, (*targetid), item->m_title, item->m_message, PushNotifyPrefix, VILSUID, item->m_waitkey, todolist, messagehashkey, item->m_imageuid);

            // vilslog::printf ("do_send_target() LOCATOR_PUSHNOTIFY_FUNCTION_1 targetid[%s] successList->size()[%d]\n",(*targetid).c_str(), (int)successList->size());

            if ( successList->size() == 0 )
            {
                //
                // check if user account
                //
                int ret = do_send_user_pushnotify(item->m_projectID, TAG_PUSHNOTIFY_FUNCTION_1, (*targetid), item->m_title, item->m_message, PushNotifyPrefix, VILSUID, todolist, messagehashkey, item->m_imageuid);

                if (ret == 0)
                {
                    vilslog::printf ("do_send_user_pushnotify() TAG_PUSHNOTIFY_FUNCTION_1 targetid[%s] ret[%d]\n", (*targetid).c_str(), ret);
                }
                else
                {
                    successList->push_back((*targetid));
                }
            }
        }

        if ( successList->size() == 0 )
        {
            failList->push_back((*targetid));
        }
        else
        {
            //
            // add to sendList
            //
            for (std::list<std::string>::iterator successid = successList->begin(); successid != successList->end(); successid++)
            {
                bool bFound = false;
                for (std::list<std::string>::iterator sendid = sendList.begin(); sendid != sendList.end(); sendid++)
                {

                    // vilslog::printf ("put to sendList() successid[%s] sendid[%s]\n", (*successid).c_str(), (*sendid).c_str());

                    if ((*successid).compare((*sendid)) == 0)
                    {
                        bFound = true;
                        break;
                    }
                }
                if (!bFound)
                {
                    sendList.push_back((*successid));
                }
            }
        }
        successList->clear();
        delete successList;
    }

    std::string targetids = convertListToString(&sendList);
    sendList.clear();

    std::string origtargetids = convertListToString(&item->m_targetlist);
    std::string subgroupids = convertListToString(&item->m_subgrouplist);

    // vilslog::printf ("PushNotificationService()::do_function_MULTI_PUSHNOTIFY_FUNCTION_1() function[%d] sendList[%s] origtargetids[%s] m_userid[%s]\n",
    //     item->m_functiontype, targetids.c_str(), origtargetids.c_str(), item->m_userid.c_str());

    PushNotifyMessageRecordStruct pushmsgRecord;
    pushmsgRecord.type = item->m_functiontype;
    pushmsgRecord.nodeid = "";
    pushmsgRecord.title = item->m_title;
    pushmsgRecord.message = item->m_message;
    pushmsgRecord.messagehashkey = messagehashkey;
    //pushmsgRecord.targetids = targetids;
    pushmsgRecord.targetids = origtargetids;
    pushmsgRecord.subgroupids = subgroupids;
    pushmsgRecord.surveyids = surveylist;
    pushmsgRecord.hashkeys = hashkeys;
    pushmsgRecord.deadlines = deadlinelist;
    pushmsgRecord.userid = convert_string_to_int(item->m_userid);
    pushmsgRecord.imageuid = item->m_imageuid;
    pushmsgRecord.formid = item->m_formid;
    pushmsgRecord.eventday = item->m_eventday;
    pushmsgRecord.eventtime = item->m_eventtime;
    pushmsgRecord.maingroups = item->m_maingroups;

    WorkItem * work = new WorkItem("PushNotifyMessageRecord", item->m_projectID, &pushmsgRecord);
    mMySQLJobQueue.add(work);

    return failList;
}

std::list<std::string> * PushNotificationService::do_function_TAG_PUSHNOTIFY_FUNCTION_5(NotificationItem * item, std::string PushNotifyPrefix, std::string VILSUID, std::string messagehashkey)
{
    std::list<std::string> * failList = new std::list<std::string>();

    std::list<std::string> * sendList = do_send_source_target(item->m_projectID, item->m_functiontype, item->m_nodeid, item->m_macaddress, item->m_title, item->m_message, PushNotifyPrefix, VILSUID, item->m_waitkey, messagehashkey);
    if ( sendList->size() == 0 )
    {
        failList->push_back(item->m_nodeid);
    }
    else
    {
        std::string targetids = convertListToString(sendList);
        std::string subgroupids = convertListToString(&item->m_subgrouplist);

        PushNotifyMessageRecordStruct pushmsgRecord;
        pushmsgRecord.type = item->m_functiontype;
        pushmsgRecord.nodeid = item->m_nodeid;
        pushmsgRecord.title = item->m_title;
        pushmsgRecord.message = item->m_message;
        pushmsgRecord.messagehashkey = messagehashkey;
        pushmsgRecord.targetids = targetids;
        pushmsgRecord.subgroupids = subgroupids;
        pushmsgRecord.surveyids = "";
        pushmsgRecord.hashkeys = "";
        pushmsgRecord.deadlines = "";
        pushmsgRecord.userid = convert_string_to_int(item->m_userid);
        pushmsgRecord.maingroups = item->m_maingroups;

        WorkItem * work = new WorkItem("PushNotifyMessageRecord", item->m_projectID, &pushmsgRecord);
        mMySQLJobQueue.add(work);
    }

    sendList->clear();
    delete sendList;

    return failList;
}

std::list<std::string> * PushNotificationService::do_function_TAG_PUSHNOTIFY_FUNCTION_10(NotificationItem * item, std::string PushNotifyPrefix, std::string VILSUID, std::string messagehashkey)
{
    std::list<std::string> * failList = new std::list<std::string>();

    std::list<std::string> * sendList = do_send_trigger_target(item->m_projectID, item->m_nodeid, &item->mTriggerData, PushNotifyPrefix, VILSUID, item->m_waitkey, messagehashkey);
    if ( sendList->size() == 0 )
    {
        failList->push_back(item->m_nodeid);
    }
    else
    {

        std::string targetids = convertListToString(sendList);
        std::string subgroupids = convertListToString(&item->m_subgrouplist);

        PushNotifyMessageRecordStruct pushmsgRecord;
        pushmsgRecord.type = TAG_PUSHNOTIFY_FUNCTION_10;
        pushmsgRecord.nodeid = item->mTriggerData.nodeid;
        pushmsgRecord.title = item->mTriggerData.title;
        pushmsgRecord.message = item->mTriggerData.message;
        pushmsgRecord.messagehashkey = messagehashkey;
        pushmsgRecord.targetids = targetids;
        pushmsgRecord.subgroupids = subgroupids;
        pushmsgRecord.surveyids = "";
        pushmsgRecord.hashkeys = "";
        pushmsgRecord.deadlines = "";
        pushmsgRecord.userid = convert_string_to_int(item->m_userid);
        pushmsgRecord.maingroups = item->m_maingroups;

        WorkItem * work = new WorkItem("PushNotifyMessageRecord", item->m_projectID, &pushmsgRecord);
        mMySQLJobQueue.add(work);
    }

    sendList->clear();
    delete sendList;

    return failList;
}

std::list<std::string> * PushNotificationService::do_function_USER_PUSHNOTIFY_FUNCTION(NotificationItem * item, int functiontype, std::string PushNotifyPrefix, std::string VILSUID, std::string messagehashkey)
{
    std::list<std::string> * failList = new std::list<std::string>();

    // vilslog::printf("do_function_TAG_PUSHNOTIFY_FUNCTION_12() item->m_userid[%s] m_title[%s] m_message[%s]\n", item->m_userid.c_str(), item->m_title.c_str(), item->m_message.c_str());

    int ret = do_send_user_pushnotify(item->m_projectID, functiontype, item->m_userid, item->m_title, item->m_message, PushNotifyPrefix, VILSUID, "", messagehashkey, "");
    if (ret == 0)
    {
        failList->push_back(item->m_nodeid);
    }
    else
    {
        std::string subgroupids = convertListToString(&item->m_subgrouplist);

        PushNotifyMessageRecordStruct pushmsgRecord;
        pushmsgRecord.type = functiontype;
        pushmsgRecord.nodeid = item->m_nodeid;
        pushmsgRecord.title = item->m_title;
        pushmsgRecord.message = item->m_message;
        pushmsgRecord.messagehashkey = messagehashkey;
        pushmsgRecord.targetids = item->m_userid;
        pushmsgRecord.subgroupids = subgroupids;
        pushmsgRecord.surveyids = "";
        pushmsgRecord.hashkeys = "";
        pushmsgRecord.deadlines = "";
        pushmsgRecord.userid = convert_string_to_int(item->m_userid);
        pushmsgRecord.maingroups = item->m_maingroups;

        WorkItem * work = new WorkItem("PushNotifyMessageRecord", item->m_projectID, &pushmsgRecord);
        mMySQLJobQueue.add(work);
    }
    return failList;
}

std::list<std::string> * PushNotificationService::do_function_CARD_PUSHNOTIFY_FUNCTION_1(NotificationItem * item, std::string PushNotifyPrefix, std::string VILSUID, std::string messagehashkey)
{
    std::list<std::string> * failList = new std::list<std::string>();

    // vilslog::printf("do_function_CARD_PUSHNOTIFY_FUNCTION_1() item->m_nodeid[%s] m_macaddress[%s] m_title[%s] m_message[%s]\n",
    //     item->m_nodeid.c_str(), item->m_macaddress.c_str(), item->m_title.c_str(), item->m_message.c_str());

    int ret = do_send_card_pushnotify(item->m_projectID, CARD_PUSHNOTIFY_FUNCTION_1, item->m_nodeid, item->m_macaddress, item->m_title, item->m_message, PushNotifyPrefix, VILSUID, messagehashkey);
    if (ret == 0)
    {
        failList->push_back(item->m_nodeid);
    }
    else
    {
        std::string subgroupids = convertListToString(&item->m_subgrouplist);

        PushNotifyMessageRecordStruct pushmsgRecord;
        pushmsgRecord.type = CARD_PUSHNOTIFY_FUNCTION_1;
        pushmsgRecord.nodeid = item->m_nodeid;
        pushmsgRecord.title = item->m_title;
        pushmsgRecord.message = item->m_message;
        pushmsgRecord.messagehashkey = messagehashkey;
        pushmsgRecord.targetids = item->m_nodeid;
        pushmsgRecord.subgroupids = subgroupids;
        pushmsgRecord.surveyids = "";
        pushmsgRecord.hashkeys = "";
        pushmsgRecord.deadlines = "";
        pushmsgRecord.userid = convert_string_to_int(item->m_userid);
        pushmsgRecord.maingroups = item->m_maingroups;

        WorkItem * work = new WorkItem("PushNotifyMessageRecord", item->m_projectID, &pushmsgRecord);
        mMySQLJobQueue.add(work);
    }
    return failList;
}

std::list<std::string> * PushNotificationService::do_function_other(NotificationItem * item, std::string PushNotifyPrefix, std::string VILSUID, std::string messagehashkey)
{
    std::list<std::string> * failList = new std::list<std::string>();

    std::list<std::string> * sendList = do_send_target(item->m_projectID, item->m_functiontype, item->m_nodeid, item->m_title, item->m_message, PushNotifyPrefix, VILSUID, item->m_waitkey, "", messagehashkey, "");
    if ( sendList->size() == 0 )
    {
        failList->push_back(item->m_nodeid);
    }
    else
    {
        std::string targetids = convertListToString(sendList);
        std::string subgroupids = convertListToString(&item->m_subgrouplist);

        PushNotifyMessageRecordStruct pushmsgRecord;
        pushmsgRecord.type = item->m_functiontype;
        pushmsgRecord.nodeid = item->m_nodeid;
        pushmsgRecord.title = item->m_title;
        pushmsgRecord.message = item->m_message;
        pushmsgRecord.messagehashkey = messagehashkey;
        pushmsgRecord.targetids = targetids;
        pushmsgRecord.subgroupids = subgroupids;
        pushmsgRecord.surveyids = "";
        pushmsgRecord.hashkeys = "";
        pushmsgRecord.deadlines = "";
        pushmsgRecord.userid = convert_string_to_int(item->m_userid);
        pushmsgRecord.maingroups = item->m_maingroups;

        WorkItem * work = new WorkItem("PushNotifyMessageRecord", item->m_projectID, &pushmsgRecord);
        mMySQLJobQueue.add(work);
    }

    sendList->clear();
    delete sendList;

    return failList;
}

std::list<std::string> * PushNotificationService::do_send_trigger_target(int projectid, std::string targetid, TriggerPushMessageStruct * triggerData, std::string PushNotifyPrefix, std::string VILSUID, std::string waitkey, std::string messagehashkey)
{
    //int count = 0;
    std::list<std::string> * successList = new std::list<std::string>();
    std::string function = "10";
    for (std::list<ProjectPushNotifySettingStruct*>::iterator prjSetting = mProjectPushNotifyList.begin(); prjSetting != mProjectPushNotifyList.end(); prjSetting++)
    {
        if (((*prjSetting)->projectid == projectid) && ((*prjSetting)->mPushNotifyList != NULL))
        {
            for (std::list<PushNotifySettingStruct*>::iterator setting = (*prjSetting)->mPushNotifyList->begin(); setting != (*prjSetting)->mPushNotifyList->end(); setting++)
            {
                if ( targetid.compare((*setting)->targetid) == 0 )
                {
                    // vilslog::printf ("PushNotificationService()::do_send_trigger_target() function[%d] targetid[%s] nodeid[%s] functions[%s] Found!\n",
                    //     functiontype, targetid.c_str(), (*setting)->nodeid.c_str(), (*setting)->functions.c_str());

                    std::list<std::string>::iterator pos = std::find((*setting)->functionList->begin(), (*setting)->functionList->end(), function);
                    if(pos != (*setting)->functionList->end() )
                    {
                        // vilslog::printf ("PushNotificationService()::do_send_trigger_target() function[%d] targetid[%s] nodeid[%s] functions[%s] Enabled!\n",
                        //     functiontype, (*setting)->targetid.c_str(), (*setting)->nodeid.c_str(), (*setting)->functions.c_str());

                        int ret = do_send_pushnotify(projectid, TAG_PUSHNOTIFY_FUNCTION_10, triggerData->nodeid, targetid, triggerData->title, triggerData->message, PushNotifyPrefix, VILSUID, waitkey, "", messagehashkey, "");
                        if (ret == 1)
                        {
                            successList->push_back((*setting)->nodeid);
                            //count++;
                        }
                    }
                    else
                    {
                        // vilslog::printf ("PushNotificationService()::do_send_trigger_target() function[%d] targetid[%s] nodeid[%s] functions[%s] Not Found!!!!\n",
                        //     functiontype, (*setting)->targetid.c_str(), (*setting)->nodeid.c_str(), (*setting)->functions.c_str());
                    }

                    // check function enable
                    // if ( (*setting)->functions.length() > 0 )
                    // {
                    //     std::string::size_type pos = (*setting)->functions.find(function);
                    //     if (pos != std::string::npos)
                    //     {
                    //         // vilslog::printf ("PushNotificationService()::do_send_trigger_target() function[%d] targetid[%s] nodeid[%s] functions[%s] Enabled!\n",
                    //         //     functiontype, (*setting)->targetid.c_str(), (*setting)->nodeid.c_str(), (*setting)->functions.c_str());
                    //
                    //         int ret = do_send_pushnotify(projectid, TAG_PUSHNOTIFY_FUNCTION_10, triggerData->nodeid, targetid, triggerData->title, triggerData->message, PushNotifyPrefix, waitkey, "", messagehashkey);
                    //         if (ret == 1)
                    //         {
                    //             count++;
                    //         }
                    //     }
                    // }
                }
            }
            break;
        }
    }

    return successList;
}

std::list<std::string> * PushNotificationService::do_send_source_target(int projectid, int functiontype, std::string nodeid, std::string send_macaddress, std::string title, std::string message, std::string PushNotifyPrefix, std::string VILSUID, std::string waitkey, std::string messagehashkey)
{
    //int count = 0;
    std::list<std::string> * successList = new std::list<std::string>();
    std::string function = std::to_string(functiontype);

    // check node still exist
    NodeStruct * node = mDeviceManager->get_Tag_node_ByMacaddress(projectid, send_macaddress.c_str());
    if ( node == NULL )
    {
        node = mDeviceManager->get_Locator_node_ByMacaddress(projectid, send_macaddress.c_str());
    }

    if ( node == NULL )
    {
        return successList;
    }

    std::string targetid = node->nodeid;

    for (std::list<ProjectPushNotifySettingStruct*>::iterator prjSetting = mProjectPushNotifyList.begin(); prjSetting != mProjectPushNotifyList.end(); prjSetting++)
    {
        if (((*prjSetting)->projectid == projectid) && ((*prjSetting)->mPushNotifyList != NULL))
        {
            for (std::list<PushNotifySettingStruct*>::iterator setting = (*prjSetting)->mPushNotifyList->begin(); setting != (*prjSetting)->mPushNotifyList->end(); setting++)
            {
                if ( targetid.compare((*setting)->targetid) == 0 )
                {
                    // vilslog::printf ("PushNotificationService()::do_send_source_target() function[%d] targetid[%s] nodeid[%s] functions[%s] Found!\n",
                    //     functiontype, targetid.c_str(), (*setting)->nodeid.c_str(), (*setting)->functions.c_str());

                    std::list<std::string>::iterator pos = std::find((*setting)->functionList->begin(), (*setting)->functionList->end(), function);
                    if(pos != (*setting)->functionList->end() )
                    {
                        // vilslog::printf ("PushNotificationService()::do_send_source_target() function[%d] targetid[%s] nodeid[%s] functions[%s] Enabled!\n",
                        //     functiontype, (*setting)->targetid.c_str(), (*setting)->nodeid.c_str(), (*setting)->functions.c_str());

                        int ret = do_send_pushnotify(projectid, functiontype, (*setting)->nodeid, nodeid, title, message, PushNotifyPrefix, VILSUID, waitkey, "", messagehashkey, "");
                        if (ret == 1)
                        {
                            successList->push_back((*setting)->nodeid);
                            //count++;
                        }
                    }
                    else
                    {
                        // vilslog::printf ("PushNotificationService()::do_send_source_target() function[%d] targetid[%s] nodeid[%s] functions[%s] Not Found!!!!\n",
                        //     functiontype, (*setting)->targetid.c_str(), (*setting)->nodeid.c_str(), (*setting)->functions.c_str());
                    }

                    // check function enable
                    // if ( (*setting)->functions.length() > 0 )
                    // {
                    //     std::string::size_type pos = (*setting)->functions.find(function);
                    //     if (pos != std::string::npos)
                    //     {
                    //         // vilslog::printf ("PushNotificationService()::do_send_source_target() function[%d] targetid[%s] nodeid[%s] functions[%s] Enabled!\n",
                    //         //     functiontype, (*setting)->targetid.c_str(), (*setting)->nodeid.c_str(), (*setting)->functions.c_str());
                    //
                    //         int ret = do_send_pushnotify(projectid, functiontype, (*setting)->nodeid, nodeid, title, message, PushNotifyPrefix, waitkey, "", messagehashkey);
                    //         if (ret == 1)
                    //         {
                    //             count++;
                    //         }
                    //     }
                    // }
                }
            }
            break;
        }
    }

    return successList;
}

std::list<std::string> * PushNotificationService::do_send_target(int projectid, int functiontype, std::string targetid, std::string title, std::string message, std::string PushNotifyPrefix, std::string VILSUID, std::string waitkey, std::string surveylist, std::string messagehashkey, std::string imageuid)
{
    std::list<std::string> * successList = new std::list<std::string>();
    std::string function = std::to_string(functiontype);

    // vilslog::printf ("PushNotificationService()::do_send_target() projectid[%d] functiontype[%d] targetid[%s] title[%s] message[%s]\n",
    //     projectid, functiontype, targetid.c_str(), title.c_str(), message.c_str());

    for (std::list<ProjectPushNotifySettingStruct*>::iterator prjSetting = mProjectPushNotifyList.begin(); prjSetting != mProjectPushNotifyList.end(); prjSetting++)
    {
        if (((*prjSetting)->projectid == projectid) && ((*prjSetting)->mPushNotifyList != NULL))
        {
            // vilslog::printf ("PushNotificationService()::do_send_target() 1\n");
            for (std::list<PushNotifySettingStruct*>::iterator setting = (*prjSetting)->mPushNotifyList->begin(); setting != (*prjSetting)->mPushNotifyList->end(); setting++)
            {
                if ( targetid.compare((*setting)->targetid) == 0 )
                {
                    // vilslog::printf ("PushNotificationService()::do_send_target() function[%d] targetid[%s] nodeid[%s] functions[%s] Found!\n",
                    //     functiontype, targetid.c_str(), (*setting)->nodeid.c_str(), (*setting)->functions.c_str());

                    std::list<std::string>::iterator pos = std::find((*setting)->functionList->begin(), (*setting)->functionList->end(), function);
                    if(pos != (*setting)->functionList->end() )
                    {
                        // vilslog::printf ("PushNotificationService()::do_send_target() function[%d] targetid[%s] nodeid[%s] functions[%s] Enabled!\n",
                        //     functiontype, (*setting)->targetid.c_str(), (*setting)->nodeid.c_str(), (*setting)->functions.c_str());

                        int ret = do_send_pushnotify(projectid, functiontype, (*setting)->nodeid, targetid, title, message, PushNotifyPrefix, VILSUID, waitkey, surveylist, messagehashkey, imageuid);
                        if (ret == 1)
                        {
                            successList->push_back((*setting)->nodeid);
                            //count++;
                        }
                    }
                    else
                    {
                        // vilslog::printf ("PushNotificationService()::do_send_target() function[%d] targetid[%s] nodeid[%s] functions[%s] Not Found!!!!\n",
                        //     functiontype, (*setting)->targetid.c_str(), (*setting)->nodeid.c_str(), (*setting)->functions.c_str());
                    }

                    // check function enable
                    // if ( (*setting)->functions.length() > 0 )
                    // {
                    //     std::string::size_type pos = (*setting)->functions.find(function);
                    //     if (pos != std::string::npos)
                    //     {
                    //         // vilslog::printf ("PushNotificationService()::do_send_target() function[%d] targetid[%s] nodeid[%s] functions[%s] Enabled!\n",
                    //         //     functiontype, (*setting)->targetid.c_str(), (*setting)->nodeid.c_str(), (*setting)->functions.c_str());
                    //
                    //         int ret = do_send_pushnotify(projectid, functiontype, (*setting)->nodeid, targetid, title, message, PushNotifyPrefix, waitkey, surveylist, messagehashkey);
                    //         if (ret == 1)
                    //         {
                    //             count++;
                    //         }
                    //     }
                    // }
                }
            }
            break;
        }
    }

    return successList;
}

int PushNotificationService::do_send_pushnotify(int projectid, int functiontype, std::string nodeid, std::string targetid, std::string title, std::string message, std::string PushNotifyPrefix, std::string VILSUID, std::string waitkey, std::string todolist, std::string messagehashkey, std::string imageuid)
{
    // check node still exist
    NodeStruct * node = mDeviceManager->get_Tag_node(projectid, nodeid.c_str());
    if ( node == NULL )
    {
        node = mDeviceManager->get_Locator_node_byid(projectid, nodeid.c_str());
    }

    if ( node == NULL )
    {
        return 0;
    }

    std::string topic = PushNotifyPrefix;

    if(node->nodetype == NODE_TYPE_TAG)
    {
        if ( node->tafid == 4 )
            topic.append("_USER_");
        else
            topic.append("_TAG_");
    }
    else
    if(node->nodetype == NODE_TYPE_LOCATOR)
    {
        topic.append("_LOCATOR_");
    }
    topic.append(node->macaddress);

    // vilslog::printf ("PushNotificationService()::do_send_pushnotify() topic[%s] m_title[%s] m_message.length[%d]\n",
    //     topic.c_str(), title.c_str(), message.length());

    std::string curTime = currentDateTimeForPushNotification();

    std::string messageid = sendPushNotification(title, topic, message, todolist, curTime, messagehashkey, VILSUID, imageuid);
    if (messageid.length() > 0)
    {
        PushNotifyMessageStruct pushmsg;
        pushmsg.type = functiontype;
        pushmsg.nodeid = nodeid;
        pushmsg.targetid = targetid;
        pushmsg.topic = topic;
        pushmsg.messagehashkey = messagehashkey;
        pushmsg.messageid = messageid;
        pushmsg.maingroups = "";

        WorkItem * work = new WorkItem("PushNotifyMessage", projectid, &pushmsg);
        mMySQLJobQueue.add(work);

        return 1;
    }
    else
    {
        return 2;
    }
}

int PushNotificationService::do_send_user_pushnotify(int projectid, int functiontype, std::string userid, std::string title, std::string message, std::string PushNotifyPrefix, std::string VILSUID, std::string todo, std::string messagehashkey, std::string imageuid)
{
    // std::string nodeid_Encode = UriEncode((const unsigned char *)nodeid.c_str());

    int userid_int = convert_string_to_int(userid);

    std::string topic = PushNotifyPrefix;
    topic.append("_USER_");
    topic.append(to_string(userid_int));
    std::string curTime = currentDateTimeForPushNotification();

    // vilslog::printf("PushNotificationService::do_send_user_pushnotify() title[%s]\n", title.c_str());
    // vilslog::printf("PushNotificationService::do_send_user_pushnotify() topic[%s]\n", topic.c_str());
    // vilslog::printf("PushNotificationService::do_send_user_pushnotify() message[%s]\n", message.c_str());
    // vilslog::printf("PushNotificationService::do_send_user_pushnotify() todo[%s]\n", todo.c_str());
    // vilslog::printf("PushNotificationService::do_send_user_pushnotify() curTime[%s]\n", curTime.c_str());
    // vilslog::printf("PushNotificationService::do_send_user_pushnotify() imageuid[%s]\n", imageuid.c_str());

    std::string messageid = sendPushNotification(title, topic, message, todo, curTime, messagehashkey, VILSUID, imageuid);
    if (messageid.length() > 0)
    {
        PushNotifyMessageStruct pushmsg;
        pushmsg.type = functiontype;
        pushmsg.nodeid = userid;
        pushmsg.targetid = userid;
        pushmsg.topic = topic;
        pushmsg.messagehashkey = messagehashkey;
        pushmsg.messageid = messageid;
        pushmsg.maingroups = "";

        WorkItem * work = new WorkItem("PushNotifyMessage", projectid, &pushmsg);
        mMySQLJobQueue.add(work);

        return 1;
    }
    else
    {
        return 0;
    }
}

int PushNotificationService::do_send_card_pushnotify(int projectid, int functiontype, std::string nodeid, std::string cardid, std::string title, std::string message, std::string PushNotifyPrefix, std::string VILSUID, std::string messagehashkey)
{
    std::string topic = PushNotifyPrefix;
    topic.append("_CARD_");
    topic.append(cardid);
    std::string curTime = currentDateTimeForPushNotification();

    std::string messageid = sendPushNotification(title, topic, message, "", curTime, messagehashkey, VILSUID, "");
    if (messageid.length() > 0)
    {
        PushNotifyMessageStruct pushmsg;
        pushmsg.type = functiontype;
        pushmsg.nodeid = nodeid;
        pushmsg.targetid = cardid;
        pushmsg.topic = topic;
        pushmsg.messagehashkey = messagehashkey;
        pushmsg.messageid = messageid;
        pushmsg.maingroups = "";

        WorkItem * work = new WorkItem("PushNotifyMessage", projectid, &pushmsg);
        mMySQLJobQueue.add(work);

        return 1;
    }
    else
    {
        return 0;
    }
}

void PushNotificationService::reloadPushNotifySetting()
{
    for (std::list<ProjectPushNotifySettingStruct*>::iterator prjSetting = mProjectPushNotifyList.begin(); prjSetting != mProjectPushNotifyList.end(); prjSetting++)
    {
        if ((*prjSetting)->mPushNotifyList != NULL)
        {
            clear_PushNotifySettingStruct_list((*prjSetting)->mPushNotifyList);
            delete (*prjSetting)->mPushNotifyList;
            (*prjSetting)->mPushNotifyList = NULL;
        }
    }

    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if (baseDb == NULL)
            continue;

        // vilslog::printf("PushNotificationService::reloadPushNotifySetting() projectid[%d]\n",
        //     projectid);

        bool bFound = false;
        for (std::list<ProjectPushNotifySettingStruct*>::iterator prjSetting = mProjectPushNotifyList.begin(); prjSetting != mProjectPushNotifyList.end(); prjSetting++)
        {
            if ( (*prjSetting)->projectid == projectid )
            {
                (*prjSetting)->mPushNotifyList = baseDb->get_PushNotifySettings();
                // vilslog::printf("PushNotificationService::reloadPushNotifySetting() projectid[%d] mPushNotifyList->size()[%d]\n",
                //     projectid, (int)(*prjSetting)->mPushNotifyList->size());

                bFound = true;
                break;
            }
        }
        if (!bFound)
        {
            ProjectPushNotifySettingStruct * newSetting = new ProjectPushNotifySettingStruct();
            newSetting->projectid = projectid;
            newSetting->mPushNotifyList = baseDb->get_PushNotifySettings();
            mProjectPushNotifyList.push_back(newSetting);

            // vilslog::printf("PushNotificationService::reloadPushNotifySetting() projectid[%d] mPushNotifyList->size()[%d]\n",
            //     projectid, (int)newSetting->mPushNotifyList->size());
        }

    }

    int count = 0;
    for (std::list<ProjectPushNotifySettingStruct*>::iterator prjSetting = mProjectPushNotifyList.begin(); prjSetting != mProjectPushNotifyList.end(); prjSetting++)
    {
        //
        // check default setting
        //
        std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list((*prjSetting)->projectid);
        for (std::list<NodeStruct*>::iterator existed = tagNodeList->begin(); existed != tagNodeList->end(); existed++)
        {
            bool bFound = false;
            for (std::list<PushNotifySettingStruct*>::iterator setting = (*prjSetting)->mPushNotifyList->begin(); setting != (*prjSetting)->mPushNotifyList->end(); setting++)
            {
                if ( (*setting)->nodeid.compare((*existed)->nodeid) == 0 )
                {
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
            {
                WorkItem * item = new WorkItem("updatePushNotifySetting", (*prjSetting)->projectid, NODE_TYPE_TAG, (*existed)->macaddress);
                item->setNodeId((*existed)->nodeid);
                item->setRole((*existed)->tafid);
                mMySQLJobQueue.add(item);
                count++;
            }
        }
        std::list<NodeStruct*> * locatorNodeList = mDeviceManager->get_Locator_list((*prjSetting)->projectid);
        for (std::list<NodeStruct*>::iterator existed = locatorNodeList->begin(); existed != locatorNodeList->end(); existed++)
        {
            bool bFound = false;
            for (std::list<PushNotifySettingStruct*>::iterator setting = (*prjSetting)->mPushNotifyList->begin(); setting != (*prjSetting)->mPushNotifyList->end(); setting++)
            {
                if ( (*setting)->nodeid.compare((*existed)->nodeid) == 0 )
                {
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
            {
                WorkItem * item = new WorkItem("updatePushNotifySetting", (*prjSetting)->projectid, NODE_TYPE_LOCATOR, (*existed)->macaddress);
                item->setNodeId((*existed)->nodeid);
                item->setRole((*existed)->tafid);
                mMySQLJobQueue.add(item);
                count++;
            }
        }

    }

    if (count > 0 )
    {
        vilslog::printf("PushNotificationService::reloadPushNotifySetting() new updatePushNotifySetting count[%d]\n", count);
    }
}

bool PushNotificationService::checkValidSendNotifity(NotificationItem * item)
{
    bool validSend = true;

    bool bfound = false;
    for (std::list<FunctionSendTimeStruct *>::iterator preTime = mFunctionSendTimeList.begin(); preTime != mFunctionSendTimeList.end(); preTime++)
    {
        if ( (*preTime)->projectid != item->m_projectID )
        {
            continue;
        }

        if ( ((*preTime)->macaddress.length() > 0) &&
             ((*preTime)->macaddress.compare(item->m_macaddress) == 0) &&
             ((*preTime)->function == item->m_functiontype))
        {

            if ((*preTime)->message.compare(item->m_message) == 0)
            {
                time_t currentTimestamp = getCurrentTimeInMilliSec();
                time_t diffTime = currentTimestamp - (*preTime)->issuetime;

                int valid_send_time_out = VALID_SEND_TIME_OUT;

                if (item->m_functiontype == TAG_PUSHNOTIFY_FUNCTION_3)
                {
                    //valid_send_time_out = 600000; //10 mintues
                    valid_send_time_out = 3600000; // 1 hour
                }
                else
                if (item->m_functiontype == LOCATOR_PUSHNOTIFY_FUNCTION_4)
                {
                    valid_send_time_out = 2000; //2 second
                }
                else
                if (item->m_functiontype == TAG_PUSHNOTIFY_FUNCTION_1 || item->m_functiontype == LOCATOR_PUSHNOTIFY_FUNCTION_1)
                {
                    valid_send_time_out = 2000; //2 second
                }

                if ( ((int)diffTime <= valid_send_time_out) && ((int)diffTime > 0) )
                {
                    validSend = false;
                }
                else
                {
                    (*preTime)->issuetime = currentTimestamp;
                }

                bfound = true;
                break;
            }
        }

    }

    if (!bfound)
    {
        FunctionSendTimeStruct * preTime = new FunctionSendTimeStruct();
        preTime->projectid = item->m_projectID;
        preTime->NodeId = item->m_accountid;
        preTime->macaddress = item->m_macaddress;
        preTime->message = item->m_message;
        preTime->function = item->m_functiontype;
        preTime->issuetime = getCurrentTimeInMilliSec();
        mFunctionSendTimeList.push_back(preTime);
    }

    return validSend;
}

//
// http://smimsvils.appspot.com/sendmessage?title=SMIMSVILS%E6%8E%A8%E6%92%AD%E9%80%9A%E7%9F%A5&body=Tag0025%20%E9%80%B2%E5%85%A5%E5%B7%A1%E6%AA%A2%E9%BB%9E%20Locator0005&topic=smimsvils-android&datetime=201908021104
//
std::string PushNotificationService::sendPushNotification(std::string title, std::string topic, std::string message, std::string todo, std::string curTime, std::string messagehashkey, std::string VILSUID, std::string imageuid)
{
    const char *host = PSUH_NOTIFY_SERVER; // 目標 URI
    //const char *host = "test-dot-smimsvils.appspot.com"; // 目標 URI
    //const char *host = "ios-dot-smimsvils.appspot.com"; // 目標 URI
    const char *command = "vilsapi/sendmessage"; // HTTP port
    const char *PORT_NUM = PSUH_NOTIFY_SERVER_PORT; // HTTP port

    char request[4096];
    memset(request, 0, 4096);

    //char request[0xfff], response[0xfff]; // 請求 與 回應訊息
    //const char *requestLine = "GET /sendmessage HTTP/1.1\r\n"; // 請求行
    const char *requestLineFmt = "POST /%s HTTP/1.1\r\n"; // 請求行
    const char *headerFmt = "Host: %s:%s\r\n"; // Host 表頭欄位
    const char *ContentType = "Content-Type: application/x-www-form-urlencoded\r\n";
    const char *ContentLengthFmt = "Content-Length: %d\r\n\r\n";
    const char *MessageFmt = "title=%s&topic=%s&body=%s&datetime=%s&todo=%s&messagehashkey=%s&vilsuid=%s&imageuid=%s";
    //const char *CRLF = "\r\n";  // 表頭後的 CRLF

    // int cfd; // Socket 檔案描述符 (File Descriptor)
    // int gaiStatus; // getaddrinfo 狀態碼
    // struct addrinfo hints; // hints 參數，設定 getaddrinfo() 的回傳方式
    // struct addrinfo *result; // getaddrinfo() 執行結果的 addrinfo 結構指標

    char buffer[4096];
    int bufferLen = 4096;
    memset(buffer, 0, bufferLen);

    //組裝請求訊息
    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, requestLineFmt, command);
    strcpy(request, buffer);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, headerFmt, host, PORT_NUM);
    strcat(request, buffer);

    strcat(request, ContentType);

    char sendmessage[4096];
    memset(sendmessage, 0, 4096);
    snprintf(sendmessage, 4096, MessageFmt, title.c_str(), topic.c_str(), message.c_str(), curTime.c_str(), todo.c_str(), messagehashkey.c_str(), VILSUID.c_str(), imageuid.c_str());

    memset(buffer, 0, 4096);
    snprintf(buffer, bufferLen, ContentLengthFmt, strlen(sendmessage));
    strcat(request, buffer);

    strcat(request, sendmessage);
    //strcat(request, CRLF);

    //
    // sen out request
    //
    std::string messageid = sendRequest(host, PORT_NUM, request);
    if ( messageid.length() > 0 )
    {
        return messageid;
    }
    else
    {
        vilslog::printf("sendPushNotification() failed!\n");
        return messageid;
    }
}

std::string PushNotificationService::sendRequest(const char * host, const char * port, const char * request)
{
    std::string messageid = "";
    int cfd; // Socket 檔案描述符 (File Descriptor)
    int gaiStatus; // getaddrinfo 狀態碼
    struct addrinfo hints; // hints 參數，設定 getaddrinfo() 的回傳方式
    struct addrinfo *result; // getaddrinfo() 執行結果的 addrinfo 結構指標
    char response[4096]; // 回應訊息
    size_t n = 0;

    memset(response, 0, 4096);

    // 以 memset 清空 hints 結構
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC; // 使用 IPv4 or IPv6
    //hints.ai_family = AF_INET; // 使用 IPv4
    hints.ai_socktype = SOCK_STREAM; // 串流 Socket
    hints.ai_flags = AI_NUMERICSERV; // 將 getaddrinfo() 第 2 參數 (PORT_NUM) 視為數字

    // 以 getaddrinfo 透過 DNS，取得 addrinfo 鏈結串列 (Linked List)
    // 以從中取得 Host 的 IP 位址
    if ((gaiStatus = getaddrinfo(host, port, &hints, &result)) != 0)
    {
        errPrint((char *) gai_strerror(gaiStatus));
        return messageid;
    }

    // 分別以 domain, type, protocol 建立 socket 檔案描述符
    cfd = socket(result->ai_family, result->ai_socktype, 0);

    if ( cfd == -1)
    {
        errPrint((char *) gai_strerror(gaiStatus));
        return messageid;
    }

    int set = 1;
    setsockopt(cfd, SOL_SOCKET, MSG_NOSIGNAL, (void *)&set, sizeof(int));

    // 以 socket 檔案描述符 (cfd), addr, addrlen 進行連線
    // 其中，result->ai_addr 為 gai 取得之 通用 socket 位址結構 -- sockaddr
    if (connect(cfd, result->ai_addr, result->ai_addrlen) < 0)
    {
        errPrint("Connect");
        return messageid;
    }


    // 釋放 getaddrinfo (Linked List) 記憶體空間
    freeaddrinfo(result);
    result = NULL;

    // 格式化輸出請求訊息
    //printf("----------\nRequest:\n----------\n%s\n", request);

    // 發送請求
    if (send(cfd, request, strlen(request), 0) <= 0)
    {
        errPrint("Send");
        return messageid;
    }

    // 接收回應
#if defined(__APPLE__)
    if ((n = recv(cfd, response, 4096, SO_NOSIGPIPE)) <= 0)
#else
    if ((n = recv(cfd, response, 4096, MSG_NOSIGNAL)) <= 0)
#endif
    {
        errPrint("Receive");
        return messageid;
    }


    // 格式化輸出回應訊息
    // printf("----------\nResponse:\n----------\n%s\n", response);

/*
    ----------
    Response:
    ----------
    HTTP/1.1 200 OK
    Content-Type: text/plain;charset=utf-8
    X-Cloud-Trace-Context: 252abfff9e7faacb4ddeb8eefb54770b;o=1
    Date: Fri, 02 Aug 2019 07:37:59 GMT
    Server: Google Frontend
    Content-Length: 63

    {  "name": "projects/smimsvils/messages/5888366962163071374"}
*/


    int header_end_index = 0;
    std::list<HttpHeader*> * httpHeaderlist = NULL;
    httpHeaderlist = parse_http_header(response, n);

    if (httpHeaderlist != NULL)
    {
        for (std::list<HttpHeader *>::iterator header = httpHeaderlist->begin(); header != httpHeaderlist->end(); header++)
        {
            //vilslog::printf("%s:%s\n", (*header)->parameter.c_str(), (*header)->value.c_str());

            if ((*header)->parameter.compare("header_end_index") == 0)
            {
                header_end_index = atoi((*header)->value.c_str());
            }
        }
    }

    std::string content = response + header_end_index;

    //vilslog::printf("PushNotificationService::sendRequest() return content[%s]\n", remove_newline(content).c_str());

    string::size_type found = content.find("{", 0);
    if (found != string::npos)
    {
        std::string http_response_json = content.substr(found);

        //vilslog::printf ("http_response_json.length[%d]\n", http_response_json.length());
        // printf ("\n-------------------\n%s\n-------------------\n", http_response_json.c_str());

        Json::Reader * reader = new Json::Reader();
        Json::Value JSONresult;
        bool parsingSuccessful = false;

        try{
            parsingSuccessful = reader->parse( http_response_json.c_str(), JSONresult);

            if (parsingSuccessful)
            {
                if ( JSONresult.isMember("name") )
                {
                    std::string prjMessageid = JSONresult["name"].asCString();
                    //vilslog::printf ("PushNotificationService::sendRequest() name[%s]\n", prjMessageid.c_str());
                    std::vector<std::string> msgs = convert_string_to_vector(prjMessageid, '/');

                    if (msgs.size() > 0)
                    {
                        messageid = msgs[msgs.size() - 1];

                        // vilslog::printf ("PushNotificationService::sendRequest() messageid[%s]\n", messageid.c_str());

                        msgs.clear();
                    }
                }
            }
            // else
            // {
            //     vilslog::printf ("PushNotificationService::sendRequest() http_response_json[%s]\n", http_response_json.c_str());
            // }
        }
        catch(exception exc)
        {
            vilslog::printf ("PushNotificationService::sendRequest() reader->parse() catch exception[%s]\n", exc.what());
        }

        delete reader;
    }

    if (httpHeaderlist != NULL)
    {
        clear_HttpHeader_list(httpHeaderlist);
        delete httpHeaderlist;
    }

    // 半雙工關閉 TCP Socket 連線
    // (i.e., 關閉寫入)
    shutdown(cfd, SHUT_WR);
    close(cfd);
    return messageid;
}

std::list<HttpHeader*> * PushNotificationService::parse_http_header(const char * response, size_t n)
{
    // HTTP/1.1 200 OK\r\n
    // Server: nginx/1.10.3 (Ubuntu)\r\n
    // Date: Fri, 16 Aug 2019 01:36:41 GMT\r\n
    // Content-Type: application/json; charset=utf-8\r\n
    // Transfer-Encoding: chunked\r\n
    // Connection: keep-alive\r\n
    // Access-Control-Allow-Origin: *\r\n
    // \r\n
    // f66\r\n
    // {"success"...

    char buffer[4096];
    std::list<HttpHeader*> * httpHeaderlist = NULL;

    memset(buffer, 0, 4096);
    int datalen = get_line(response, (int)n, 0, buffer, 4096);
    buffer[datalen] = 0;

    if (response[0] == 'H' && response[1] == 'T' && response[2] == 'T' && response[3] == 'P')
    {
        httpHeaderlist = new std::list<HttpHeader*>();
    }
    else
    {
        return NULL;
    }

    int start_idx = 0;
    bool bFinished = false;
    while(!bFinished)
    {
        memset(buffer, 0, 4096);
        int datalen = get_line(response, (int)n, start_idx, buffer, 4096);
        buffer[datalen] = 0;

        // pair 屬性名稱: 屬性值\r\n
        std::string bufString = buffer;
        string::size_type loc = bufString.find(":", 0);
        if (loc != string::npos)
        {
            HttpHeader * header = new HttpHeader();

            header->parameter = bufString.substr(0, loc);
            header->value = bufString.substr(loc+2, bufString.length() - 2 - 2);

            // vilslog::printf ("PushNotificationService::parse_http_header() parameter[%s] value[%s]\n",
            //     header->parameter.c_str(), header->value.c_str());

            httpHeaderlist->push_back(header);
        }

        start_idx += datalen;

        if(datalen == 0)
        {
            HttpHeader * header = new HttpHeader();
            header->parameter = "header_end_index";
            header->value = std::to_string(start_idx);
            httpHeaderlist->push_back(header);

            // vilslog::printf ("PushNotificationService::parse_http_header() parameter[%s] value[%s]\n",
            //     header->parameter.c_str(), header->value.c_str());

            bFinished = true;
        }
        else
        if(datalen == 2)
        {
            // header end \r\n\r\n
            HttpHeader * header = new HttpHeader();
            header->parameter = "header_end_index";
            header->value = std::to_string(start_idx);
            httpHeaderlist->push_back(header);

            // vilslog::printf ("PushNotificationService::parse_http_header() parameter[%s] value[%s]\n",
            //     header->parameter.c_str(), header->value.c_str());

            bFinished = true;
        }
        else
        {
            //vilslog::printf ("SyncERPService::parse_http_header() buffer[%s]\n", buffer);
        }
    }
    return httpHeaderlist;
}

int PushNotificationService::get_line(const char * databuffer, int data_len, int start_idx, char * linebuffer, int buffer_len)
{
    int length = 0;
    bool bFound = false;
    // find 0x10 0x13
    for (int i = start_idx; i < data_len; i++)
    {
        if(length < buffer_len)
        {
            linebuffer[length] = databuffer[i];
            length++;
        }

        if (databuffer[i] == '\r')
        {
            if ((i+1) < (data_len))
            {
                if (databuffer[i+1] == '\n')
                {
                    linebuffer[length] = databuffer[i];
                    length++;
                    bFound = true;
                }
            }
        }

        if(bFound)
            break;

    }

    return length;
}

void PushNotificationService::errExit(const char *reason)
{
    std::string buff = reason ? reason : strerror(errno);
    vilslog::printf("Error: %s", buff.c_str());
    exit(EXIT_FAILURE);
}

void PushNotificationService::errPrint(const char *reason)
{
    vilslog::printf("PushNotificationService::errPrint() enter.");
    std::string buff = reason ? reason : strerror(errno);
    vilslog::printf("PushNotificationService::errPrint() Error: %s", buff.c_str());
}

std::string PushNotificationService::convertSurveyToJson(int projectid, std::list<std::string> * surveys, std::list<std::string> * hashkeys, std::list<std::string> * deadlines)
{
    std::string todolist = "[]";
    //todolist = "[{\"surveyid\":1,\"text\":\"水槽巡檢注意事項確認\",\"type\":0,\"eventid\":111,\"items\":[\"item1\",\"item2\",\"item3\"],\"hash\":\"ABCDEFGH\"},{\"surveyid\":2,\"text\":\"戶外教學日期意願調查\",\"type\":1,\"eventid\":222,\"items\":[\"1111\",\"2222\",\"3333\"],\"hash\":\"ASDFGHJ\"}]";

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
    {
        return todolist;
    }

	std::list<SurveyStruct*> * surveyList = baseDb->readSurveys(surveys);

    std::ostringstream sendstream;
    sendstream<<"[";

    std::list<std::string>::iterator hashkey = hashkeys->begin();
    std::list<std::string>::iterator deadline = deadlines->begin();

    for (std::list<std::string>::iterator surveyid = surveys->begin(); surveyid != surveys->end(); surveyid++, hashkey++, deadline++)
    {
        if ((*surveyid).compare(ApplicationForm_LEAVE_SURVEYID) == 0 )
        {
            //
            // 請假單通知
            //
            if ( surveyid != surveys->begin() )
                sendstream<<",";
            sendstream<<"{";
            sendstream<<"\"type\":0,";
            sendstream<<"\"hash\":\""<<(*hashkey)<<"\",";
            sendstream<<"\"deadline\":\""<<(*deadline)<<"\",";
            sendstream<<"\"text\":\"請假單簽核\",";
            sendstream<<"\"items\":[\"簽核中\",\"核准\",\"不核准\"]";
            sendstream<<"}";

            continue;
        }
        else
        if ((*surveyid).compare(ApplicationForm_ASSIST_ID) == 0 )
        {
            //
            // 協助事項申請
            //
            if ( surveyid != surveys->begin() )
                sendstream<<",";
            sendstream<<"{";
            sendstream<<"\"type\":0,";
            sendstream<<"\"hash\":\""<<(*hashkey)<<"\",";
            sendstream<<"\"deadline\":\""<<(*deadline)<<"\",";
            sendstream<<"\"text\":\"協助事項簽核\",";
            sendstream<<"\"items\":[\"簽核中\",\"核准\",\"不核准\"]";
            sendstream<<"}";

            continue;
        }
        else
        if ((*surveyid).compare(ApplicationForm_ASSISTEVENT_ID) == 0 )
        {
            //
            // 協助事項 事件
            //
            if ( surveyid != surveys->begin() )
                sendstream<<",";
            sendstream<<"{";
            sendstream<<"\"type\":0,";
            sendstream<<"\"hash\":\""<<(*hashkey)<<"\",";
            sendstream<<"\"deadline\":\""<<(*deadline)<<"\",";
            sendstream<<"\"text\":\"請回報完成狀況\",";
            sendstream<<"\"items\":[\"已完成\",\"未完成\"]";
            sendstream<<"}";

            continue;
        }
        else
        if ((*surveyid).compare(ApplicationForm_CURRICULUM_APPLYID) == 0 )
        {
            //
            // 選課課程通知
            //
            if ( surveyid != surveys->begin() )
                sendstream<<",";
            sendstream<<"{";
            sendstream<<"\"type\":0,";
            sendstream<<"\"hash\":\""<<(*hashkey)<<"\",";
            sendstream<<"\"deadline\":\""<<(*deadline)<<"\",";
            sendstream<<"\"text\":\"選課課程簽核\",";
            sendstream<<"\"items\":[\"簽核中\",\"核准\",\"不核准\"]";
            sendstream<<"}";

            continue;
        }

        int isurveyid = convert_string_to_int((*surveyid));
        for (std::list<SurveyStruct*>::iterator survey = surveyList->begin(); survey != surveyList->end(); survey++)
        {
            if ( isurveyid == (*survey)->surveyid )
            {
                if ( surveyid != surveys->begin() )
                    sendstream<<",";
                sendstream<<"{";
                //sendstream<<"\"surveyid\":"<<(*survey)->surveyid<<",";
                //sendstream<<"\"eventid\":"<<(*survey)->eventid<<",";
                sendstream<<"\"type\":"<<(*survey)->type<<",";
                sendstream<<"\"hash\":\""<<(*hashkey)<<"\",";
                sendstream<<"\"deadline\":\""<<(*deadline)<<"\",";
                sendstream<<"\"text\":\""<<(*survey)->description<<"\",";
                sendstream<<"\"items\":[";

                std::list<SurveyItemStruct*> * surveyItemList = baseDb->readSurveyItems((*survey)->surveyid);

                //vilslog::printf ("PushNotificationService::convertSurveyToJson() surveyid[%d] title[%s] description[%s]\n", (*survey)->surveyid, (*survey)->title.c_str(), (*survey)->description.c_str());
                for (std::list<SurveyItemStruct*>::iterator surveyitem = surveyItemList->begin(); surveyitem != surveyItemList->end(); surveyitem++)
                {
                    //std::string enc = UriEncode((const unsigned char *)(*surveyitem)->description.c_str());
                    std::string enc = (*surveyitem)->description;
                    replaceAll(enc, "\"", "\\\"");

                    // vilslog::printf ("PushNotificationService::convertSurveyToJson() surveyitemid[%d] description[%s] enc[%s]\n",
                    //     (*surveyitem)->surveyitemid, (*surveyitem)->description.c_str(), enc.c_str());

                    if ( surveyitem != surveyItemList->begin() )
                        sendstream<<",";
                    sendstream<<"\""<<enc<<"\"";
                    // sendstream<<"\""<<(*surveyitem)->description<<"\"";
                }

                clear_SurveyItemStruct_list(surveyItemList);
                delete surveyItemList;

                sendstream<<"]}";
            }
        }
    }

    sendstream<<"]";
    todolist = sendstream.str();

    clear_SurveyStruct_list(surveyList);
    delete surveyList;

    // vilslog::printf ("PushNotificationService::convertSurveyToJson() todolist[%s]\n", todolist.c_str());

    return todolist;
}

std::string PushNotificationService::get_unique_hashksy_PushNotifyMessageRecord(int projectid)
{
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);

    std::string messagehashkey = random_string(16);

    // find unique hashkey
    if (baseDb != NULL)
    {
        int count = baseDb->check_PushNotifyMessageRecord_hashkey(messagehashkey);
        while (count > 0)
        {
            messagehashkey = random_string(16);
            count = baseDb->check_PushNotifyMessageRecord_hashkey(messagehashkey);

            vilslog::printf ("PushNotificationService()::get_unique_hashksy_PushNotifyMessageRecord() count[%d] messagehashkey[%s]\n",
                count, messagehashkey.c_str());
        }
    }
    return messagehashkey;
}

std::list<std::string> * PushNotificationService::get_unique_hashksy_SurveyEventRecord(int projectid, int count)
{
    std::list<std::string> * hashkeylist = new std::list<std::string>();

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);

    //for (std::list<std::string>::iterator survey = item->m_surveylist.begin(); survey != item->m_surveylist.end(); survey++)
    for (int i=0;i<count;i++)
    {
        std::string hashkey = random_string(12);
        bool bFound = true;
        while(bFound)
        {
            bFound = false;
            for (std::list<std::string>::iterator existedHashKey = hashkeylist->begin(); existedHashKey != hashkeylist->end(); existedHashKey++)
            {
                if ( hashkey.compare((*existedHashKey)) == 0 )
                {
                    vilslog::printf ("PushNotificationService()::get_unique_hashksy_SurveyEventRecord() hashkey[%s] existedHashKey[%s]\n",
                        hashkey.c_str(), (*existedHashKey).c_str());

                    bFound = true;
                    break;
                }
            }

            if (bFound)
            {
                hashkey = random_string(12);
                continue;
            }

            // find unique hashkey
            if (baseDb != NULL)
            {
                int count = baseDb->check_SurveyEventRecord_hashkey(hashkey);

                if ( count > 0 )
                {
                    bFound = true;
                    hashkey = random_string(12);
                    continue;
                }
            }
            else
            {
                bFound = false;
            }
        }

        hashkeylist->push_back(hashkey);
    }
    return hashkeylist;
}
