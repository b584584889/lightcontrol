/*
 *  DeviceEventCheckService.cpp
 *
 *  Created on: 2018/10/19
 *      Author: ikki
 */

#include <string.h>
#include <sstream>
#include <iostream>

#include "DeviceEventCheckService.h"

using namespace std;

DeviceEventCheckService::DeviceEventCheckService(wqueue<DataItem*> & DeviceEventJobQueue, wqueue<AreaInOutEventStruct*> & mareaInOutEventQueue, wqueue<SubscribeDataItem*> & pWebSocketqueue, wqueue<NotificationItem*> & NotifiactionQueue, wqueue<WorkItem*> & MySQLJobQueue, DeviceManager * pDevManager, DBManager * dbManager)
    : mDeviceEventJobQueue(DeviceEventJobQueue), mAreaInOutEventQueue(mareaInOutEventQueue), mWebSocketqueue(pWebSocketqueue), mNotifiactionQueue(NotifiactionQueue), mMySQLJobQueue(MySQLJobQueue), mDeviceManager(pDevManager), mDbManager(dbManager)
{

}

bool DeviceEventCheckService::status()
{
    return mInitial;
}

std::string DeviceEventCheckService::statusString()
{
    return mStatus;
}

void * DeviceEventCheckService::run()
{
    while(1)
    {
        try
        {
            reloadTriggerPushMessage();

            runloop();
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("DeviceEventCheckService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "DeviceEventCheckService()::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void DeviceEventCheckService::runloop()
{
    mInitial = false;

    mEventChecker = new EventChecker(mWebSocketqueue, mNotifiactionQueue, mMySQLJobQueue, mDeviceManager, mDbManager);

    if (mEventChecker != NULL)
    {
        mStatus = "DeviceEventCheckService()::run() Initial EventChecker successfully!";
        vilslog::printf ("DeviceEventCheckService()::run() Initial EventChecker successfully!\n");
    }
    else
    {
        mStatus = "DeviceEventCheckService()::run() Initial EventChecker failed!!";
        vilslog::printf ("DeviceEventCheckService()::run() Initial EventChecker failed!\n");
        return;
    }

    mInitial = true;

    waitloop();
}

void DeviceEventCheckService::waitloop()
{
    long long start1 = getSystemTime();
    long long checkTime = 0;
    long long checkTriggerMessageTime = 0;
    long long checkDoorEventTime = 0;
    long long dumpTime = 0;

    while ( 1 )
    {
        bool bSleepAWhile = true;
        long long end1 = getSystemTime();
        long long difft = end1 - start1;
        //printf("wait_loop() time[%lld ms]\n", difft);

        checkTime += difft;
        checkTriggerMessageTime += difft;
        checkDoorEventTime += difft;
        dumpTime += difft;

        AreaInOutEventStruct * event = mAreaInOutEventQueue.removetimed(1); // 1 ms

        if ( event != NULL )
        {
            WorkItem * witem = new WorkItem("areainoutevent", event->projectid, event);
            mMySQLJobQueue.add(witem);

            check_doctor_event(event);

            mEventChecker->addNewEvent(event);

            delete event;
            bSleepAWhile = false;
        }

        // do 100 at a time
        int count = 0;
        while(count <= 100)
        {
            DataItem * eventDevice = mDeviceEventJobQueue.removetimed(1); // 1 ms

            if ( eventDevice != NULL )
            {
                check_device_event(eventDevice);
                delete eventDevice;
                bSleepAWhile = false;
            }
            count++;
        }

        mEventChecker->check();

        if (checkTime >= 60000) // 60 seconds
        {
            mEventChecker->setUpdateEventList();
            checkTime = 0;
        }

        if (checkTriggerMessageTime >= 1000) // 1 seconds
        {
            checkTriggerMessage();
            checkTriggerMessageTime = 0;
        }

        if (checkDoorEventTime >= 1000) // 1 seconds
        {
            checkDoorEvent();
            checkDoorEventTime = 0;
        }

        if (dumpTime >= 10000) // 10 seconds
        {
            dumpTime = 0;
        }

        if (bSleepAWhile)
        {
            usleep(100000); // 100 ms
        }

        start1 = end1;
    }
}

void DeviceEventCheckService::reloadTriggerPushMessage()
{
    std::list<int> * prjidList = mDbManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);

        reloadTriggerPushMessage(projectid);
    }
    prjidList->clear();
    delete prjidList;
}

void DeviceEventCheckService::reloadTriggerPushMessage(int projectid)
{
    bool bFound = false;
    for (std::list<ProjectTriggerPushMessageStruct*>::iterator prjSetting = mProjectTriggerPushMessageList.begin(); prjSetting != mProjectTriggerPushMessageList.end(); prjSetting++)
    {
        if ((*prjSetting)->projectid == projectid)
        {
            if ( (*prjSetting)->mTriggerPushMessageList != NULL)
            {
                clear_TriggerPushMessageStruct_list((*prjSetting)->mTriggerPushMessageList);
                delete (*prjSetting)->mTriggerPushMessageList;
                (*prjSetting)->mTriggerPushMessageList = NULL;
            }

            MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
            if (baseDb == NULL)
                break;

            (*prjSetting)->mTriggerPushMessageList = baseDb->get_TriggerPushMessages();
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if (baseDb == NULL)
            return;

        ProjectTriggerPushMessageStruct * newSetting = new ProjectTriggerPushMessageStruct();
        newSetting->projectid = projectid;
        newSetting->mTriggerPushMessageList = baseDb->get_TriggerPushMessages();
        mProjectTriggerPushMessageList.push_back(newSetting);

        // vilslog::printf("DeviceEventCheckService::reloadTriggerPushMessage() projectid[%d] mTriggerPushMessageList->size()[%d]\n",
        //     projectid, (int)newSetting->mTriggerPushMessageList->size());
    }
}

int DeviceEventCheckService::check_doctor_event(AreaInOutEventStruct * event)
{
    if ( event->tafid == 5 ) // 5:醫師
    {
        vilslog::printf("\n******************\nDeviceEventCheckService::check_doctor_event() NodeId[%s] action[%d] AreaType0D[%d] MapAreaType[%d]\n",
                        event->NodeId.c_str(), event->action, event->AreaType0D, event->MapAreaType);

        if ( event->MapAreaType == 5 || event->MapAreaType == 6 ) // 5:病房  6:病床
        {
           // vilslog::printf("\nDeviceEventCheckService::check_doctor_event() NodeId[%s] action[%d] AreaID[%s] AreaName[%s] MapAreaType[%d]\n\n",
           //                 event->NodeId.c_str(), event->action,
           //                 event->AreaID.c_str(), event->AreaName.c_str(), event->MapAreaType);

           bool bSendNotification = false;
           AreaInOutStruct data;
           bool bFound = mEventChecker->getTagAreaLastAction(event->projectid, event->NodeId, event->AreaID, data);
           if (bFound)
           {
               if ( event->action == 1 )
               {
                   // 進入,  檢查是否已離開過
                   vilslog::printf("DeviceEventCheckService::check_doctor_event() 進入, 檢查是否已離開過...\n");

                   if ( data.action == event->action )
                   {
                       // 尚未離開, 不通知
                       bSendNotification = false;
                       vilslog::printf("DeviceEventCheckService::check_doctor_event() 尚未離開, 不通知.\n");
                   }
                   else
                   {
                       // 已離開, 可通知
                       bSendNotification = true;
                       vilslog::printf("DeviceEventCheckService::check_doctor_event() 已離開, 可通知.\n");

                       // time_t t_action_cur = getCurrentTimeInSec();
                       // time_t t_action = data.DataTime_t;
                       // int seconds = (int)(t_action_cur - t_action);
                       // int minutes = seconds/60;
                       // if ( minutes > 10 )
                       // {
                       //     bSendNotification = true;
                       // }
                       // else
                       // {
                       //     bSendNotification = false;
                       // }
                   }
               }
               else
               if ( event->action == 2 )
               {
                   // 離開
                   vilslog::printf("DeviceEventCheckService::check_doctor_event() 離開, 檢查是否已離開過...\n");

                   if ( data.action == event->action )
                   {
                       // 已離開, 不通知
                       bSendNotification = false;
                       vilslog::printf("DeviceEventCheckService::check_doctor_event() 已離開, 不通知.\n");
                   }
                   else
                   {
                       // 尚未離開, 可通知
                       bSendNotification = true;
                       vilslog::printf("DeviceEventCheckService::check_doctor_event() 尚未離開, 可通知.\n");

                       // time_t t_action_cur = getCurrentTimeInSec();
                       // time_t t_action = data.DataTime_t;
                       // int seconds = (int)(t_action_cur - t_action);
                       // int minutes = seconds/60;
                       // if ( minutes > 10 )
                       // {
                       //     bSendNotification = true;
                       // }
                       // else
                       // {
                       //     bSendNotification = false;
                       // }
                   }
               }
           }
           else
           {
               // 尚未有記錄, 可通知
               bSendNotification = true;
               vilslog::printf("DeviceEventCheckService::check_doctor_event() 尚未有記錄, 可通知.\n");
           }

           if (bSendNotification)
           {
               sendNotification(event);
           }
           else
           {
               vilslog::printf("\nDeviceEventCheckService::check_doctor_event() 不通知\n\n");
           }
        }
    }
    return 0;
}

int DeviceEventCheckService::check_device_event(DataItem * item)
{
    // vilslog::printf("DeviceEventCheckService::check_device_event() m_type[%s]\n", item->getType());

    if ( strcmp(item->getType(), "updatePushNotifyTriggerMessage") == 0 )
    {
        //vilslog::printf ("DeviceEventCheckService()::check_device_event() updatePushNotifyTriggerMessage \n");
        int projectid = item->getProjectID();
        reloadTriggerPushMessage(projectid);
    }
    else
    if ( strcmp(item->getType(), "RangingFiltered") == 0 )
    {
        do_RangingFiltered(item);
    }


    return 0;
}

int DeviceEventCheckService::checkTriggerMessage()
{
    for (std::list<ProjectTriggerPushMessageStruct*>::iterator prjSetting = mProjectTriggerPushMessageList.begin(); prjSetting != mProjectTriggerPushMessageList.end(); prjSetting++)
    {
        std::list<TriggerPushMessageStruct*> * triggerPushMessageList = (*prjSetting)->mTriggerPushMessageList;
        for (std::list<TriggerPushMessageStruct*>::iterator triggerData = triggerPushMessageList->begin(); triggerData != triggerPushMessageList->end(); triggerData++)
        {
            if ( (*triggerData)->enable == 1 )
            {
                checkSingleTriggerMessage((*prjSetting)->projectid, (*triggerData));
            }
        }
    }
    return 0;
}

int DeviceEventCheckService::checkSingleTriggerMessage(int projectid, TriggerPushMessageStruct * triggerData)
{
    for (std::list<ProjectTriggerRangingStruct*>::iterator prjSetting = mProjectTriggerRangingList.begin(); prjSetting != mProjectTriggerRangingList.end(); prjSetting++)
    {
        if ( (*prjSetting)->projectid == projectid )
        {
            std::list<TriggerRangingStruct*> * triggerRangingList = (*prjSetting)->mTriggerRangingList;
            for (std::list<TriggerRangingStruct*>::iterator rangingData = triggerRangingList->begin(); rangingData != triggerRangingList->end(); rangingData++)
            {
                bool bFound = checkSingleRangingTriggerMessage(projectid, triggerData, (*rangingData));
                if ( bFound )
                {
                    // check if already send
                    if ( (*rangingData)->triggered == 0 )
                    {
                        // vilslog::printf("DeviceEventCheckService::checkSingleTriggerMessage() Found!!! nodeid[%s] distance[%d] tagid[%s] targettype[%d]\n",
                        //     triggerData->nodeid.c_str(), (*rangingData)->distance, (*rangingData)->tagid.c_str(), triggerData->targettype);
                        // vilslog::printf("DeviceEventCheckService::checkSingleTriggerMessage() Found!!! title[%s] message[%s]\n",
                        //     triggerData->title.c_str(), triggerData->message.c_str());

                        NodeStruct * tagnode = mDeviceManager->get_Tag_node(projectid, (*rangingData)->tagid.c_str());
                        if ( tagnode != NULL )
                        {
                            NotificationItem * item = new NotificationItem(TAG_PUSHNOTIFY_FUNCTION_10, projectid, NODE_TYPE_TAG, tagnode->nodeid, tagnode->nodename, to_string(tagnode->userid), "1", tagnode->macaddress, triggerData);
                            mNotifiactionQueue.add(item);
                        }
                        // update triggered flag
                        (*rangingData)->triggered = 1;
                    }
                }
            }
        }
    }
    return 0;
}

bool DeviceEventCheckService::checkSingleRangingTriggerMessage(int projectid, TriggerPushMessageStruct * triggerData, TriggerRangingStruct * rangingData)
{
    bool bFound = false;
    if ( rangingData->nodeid.compare(triggerData->nodeid) == 0 )
    {
        //
        // check tag type
        //
        NodeStruct * tagnode = mDeviceManager->get_Tag_node(projectid, rangingData->tagid.c_str());
        if ( tagnode != NULL )
        {
            if ( tagnode->tafid == triggerData->targettype )
            {
                // vilslog::printf("DeviceEventCheckService::checkSingleRangingTriggerMessage() Found!!! nodeid[%s] distance[%d] tagid[%s] targettype[%d]\n",
                //     triggerData->nodeid.c_str(), rangingData->distance, rangingData->tagid.c_str(), triggerData->targettype);
                // vilslog::printf("DeviceEventCheckService::checkSingleRangingTriggerMessage() Found!!! title[%s] message[%s]\n",
                //     triggerData->title.c_str(), triggerData->message.c_str());
                if ( rangingData->distance <= triggerData->distance )
                {
                    bFound = true;
                }
                else
                {
                    // vilslog::printf("DeviceEventCheckService::checkSingleRangingTriggerMessage() rangingData->triggered = 0 nodeid[%s] distance[%d] tagid[%s] targettype[%d]\n",
                    //     triggerData->nodeid.c_str(), rangingData->distance, rangingData->tagid.c_str(), triggerData->targettype);

                    // update triggered flag
                    rangingData->triggered = 0;
                }
            }
        }
    }

    return bFound;
}

int DeviceEventCheckService::checkDoorEvent()
{
    for (std::list<ProjectTriggerDoorEventStruct*>::iterator prjSetting = mProjectTriggerDoorEventList.begin(); prjSetting != mProjectTriggerDoorEventList.end(); prjSetting++)
    {
        std::list<TriggerDoorEventStruct*> * triggerDoorEventList = (*prjSetting)->mTriggerDoorEventList;
        for (std::list<TriggerDoorEventStruct*>::iterator triggerDoorEvent = triggerDoorEventList->begin(); triggerDoorEvent != triggerDoorEventList->end(); triggerDoorEvent++)
        {
            if ((*triggerDoorEvent)->triggered == 0 && (*triggerDoorEvent)->action == 1)
            {
                // vilslog::printf ("DeviceEventCheckService()::checkDoorEvent() projectid[%d] Tag_id[%s] Anchor_id[%s] triggered[%d] action[%d]\n",
                //     (*prjSetting)->projectid, (*triggerDoorEvent)->tagid.c_str(), (*triggerDoorEvent)->nodeid.c_str(), (*triggerDoorEvent)->triggered, (*triggerDoorEvent)->action);

                int maingroupid = 1;
                std::string action = "EnterDoor";
                std::string eventjson = convert_to_json_str((*prjSetting)->projectid, 7777, (*triggerDoorEvent)->tagid.c_str(), action, "", "0", (*triggerDoorEvent)->nodeid.c_str());

                WorkItem * witem = new WorkItem("EnterDoor", (*prjSetting)->projectid, NODE_TYPE_TAG, (*triggerDoorEvent)->nodemacaddress.c_str(), eventjson.c_str());
                witem->setMaingroupid(maingroupid);
                mMySQLJobQueue.add(witem);

                int alertType = 1; //1:人員 2:固定裝置
                int alertAction = 1;// 1:In 2:Out 3:alert 4:cancel 5:poweroff
                SubscribeDataItem * subitem = new SubscribeDataItem("event", (*prjSetting)->projectid, 7777, (*triggerDoorEvent)->nodeid.c_str(), alertType, alertAction);
                mWebSocketqueue.add(subitem);

                (*triggerDoorEvent)->triggered = 1;
            }
            else
            if ((*triggerDoorEvent)->triggered == 1 && (*triggerDoorEvent)->action == 1)
            {
                int maingroupid = 1;
                // check timeout
                time_t curTime = getCurrentTimeInMilliSec();
                int diff_t = (int)(curTime - (*triggerDoorEvent)->dataTime);

                // vilslog::printf ("DeviceEventCheckService()::checkDoorEvent() projectid[%d] Tag_id[%s] Anchor_id[%s] triggered[%d] action[%d] diff_t[%d]\n",
                //     (*prjSetting)->projectid, (*triggerDoorEvent)->tagid.c_str(), (*triggerDoorEvent)->nodeid.c_str(),
                //     (*triggerDoorEvent)->triggered, (*triggerDoorEvent)->action, diff_t);

                if ( diff_t >= 30000 )
                {
                    std::string action = "ExitDoor";
                    std::string eventjson = convert_to_json_str((*prjSetting)->projectid, 7777, (*triggerDoorEvent)->tagid.c_str(), action, "", "0", (*triggerDoorEvent)->nodeid.c_str());

                    WorkItem * witem = new WorkItem("ExitDoor", (*prjSetting)->projectid, NODE_TYPE_TAG, (*triggerDoorEvent)->nodemacaddress.c_str(), eventjson.c_str());
                    witem->setMaingroupid(maingroupid);
                    mMySQLJobQueue.add(witem);

                    int alertType = 1; //1:人員 2:固定裝置
                    int alertAction = 2;// 1:In 2:Out 3:alert 4:cancel 5:poweroff
                    SubscribeDataItem * subitem = new SubscribeDataItem("event", (*prjSetting)->projectid, 7777, (*triggerDoorEvent)->nodeid.c_str(), alertType, alertAction);
                    mWebSocketqueue.add(subitem);


                    (*triggerDoorEvent)->action = 2;
                    (*triggerDoorEvent)->triggered = 1;
                }
            }
        }
    }
    return 0;
}

int DeviceEventCheckService::do_RangingFiltered(DataItem * item)
{
    char Tag_id[64];
    char Anchor_id[64];

    sprintf(Tag_id, "%s", item->getTagId());
    sprintf(Anchor_id, "%s", item->getAnchorId());
    int projectid = item->getProjectID();
    int distance = item->getDistance();

    // vilslog::printf ("DeviceEventCheckService()::do_RangingFiltered() projectid[%d] Tag_id[%s] Anchor_id[%s] distance[%d]\n",
    //     projectid, Tag_id, Anchor_id, distance);

    put_to_ranginglist(projectid, Anchor_id, Tag_id, distance);

    put_to_dooreventlist(projectid, Anchor_id, Tag_id, distance);

    return 0;
}

void DeviceEventCheckService::put_to_ranginglist(int projectid, const char * Anchor_id, const char * Tag_id, int distance)
{
    bool bFound = false;
    for (std::list<ProjectTriggerRangingStruct*>::iterator prjSetting = mProjectTriggerRangingList.begin(); prjSetting != mProjectTriggerRangingList.end(); prjSetting++)
    {
        if ((*prjSetting)->projectid == projectid)
        {
            bool bFoundData = false;
            std::list<TriggerRangingStruct*> * triggerRangingList = (*prjSetting)->mTriggerRangingList;
            for (std::list<TriggerRangingStruct*>::iterator rangingData = triggerRangingList->begin(); rangingData != triggerRangingList->end(); rangingData++)
            {
                if ( ((*rangingData)->nodeid.compare(Anchor_id) == 0) && ((*rangingData)->tagid.compare(Tag_id) == 0) )
                {
                    (*rangingData)->distance = distance;
                    (*rangingData)->dataTime = getCurrentTimeInMilliSec();

                    // vilslog::printf ("DeviceEventCheckService()::put_to_ranginglist() update projectid[%d] Tag_id[%s] Anchor_id[%s] distance[%d]\n",
                    //     projectid, Tag_id, Anchor_id, distance);

                    bFoundData = true;
                    break;
                }
            }
            if (!bFoundData)
            {
                TriggerRangingStruct * newData = new TriggerRangingStruct();
                newData->triggered = 0;
                newData->nodeid = Anchor_id;
                newData->tagid = Tag_id;
                newData->distance = distance;
                newData->dataTime = getCurrentTimeInMilliSec();
                triggerRangingList->push_back(newData);

                // vilslog::printf ("DeviceEventCheckService()::put_to_ranginglist() new TriggerRangingStruct projectid[%d] Tag_id[%s] Anchor_id[%s] distance[%d]\n",
                //     projectid, Tag_id, Anchor_id, distance);

            }
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        ProjectTriggerRangingStruct * newPrjData = new ProjectTriggerRangingStruct();
        newPrjData->projectid = projectid;
        newPrjData->mTriggerRangingList = new std::list<TriggerRangingStruct*>();
        mProjectTriggerRangingList.push_back(newPrjData);

        TriggerRangingStruct * newData = new TriggerRangingStruct();
        newData->triggered = 0;
        newData->nodeid = Anchor_id;
        newData->tagid = Tag_id;
        newData->distance = distance;
        newData->dataTime = getCurrentTimeInMilliSec();
        newPrjData->mTriggerRangingList->push_back(newData);

        // vilslog::printf ("DeviceEventCheckService()::put_to_ranginglist() new ProjectTriggerRangingStruct projectid[%d] Tag_id[%s] Anchor_id[%s] distance[%d]\n",
        //     projectid, Tag_id, Anchor_id, distance);

    }
}

void DeviceEventCheckService::put_to_dooreventlist(int projectid, const char * Anchor_id, const char * Tag_id, int distance)
{
    bool bFound = false;
    bool bRemove = false;

    // check Anchor_id device type
    // Locator Button Type 0:Alert 1:CallOut 2:Door
    NodeStruct * locatoNode = mDeviceManager->get_Locator_node_byid(projectid, Anchor_id);

    if (locatoNode == NULL)
    {
        return;
    }

    if ( locatoNode->tafid != 2 )
        bRemove = true;

    for (std::list<ProjectTriggerDoorEventStruct*>::iterator prjSetting = mProjectTriggerDoorEventList.begin(); prjSetting != mProjectTriggerDoorEventList.end(); prjSetting++)
    {
        if ((*prjSetting)->projectid == projectid)
        {
            bool bFoundData = false;
            std::list<TriggerDoorEventStruct*> * triggerRangingList = (*prjSetting)->mTriggerDoorEventList;
            for (std::list<TriggerDoorEventStruct*>::iterator rangingData = triggerRangingList->begin(); rangingData != triggerRangingList->end(); rangingData++)
            {
                if ( ((*rangingData)->nodeid.compare(Anchor_id) == 0) && ((*rangingData)->tagid.compare(Tag_id) == 0) )
                {
                    if (bRemove)
                    {
                        vilslog::printf ("DeviceEventCheckService()::put_to_dooreventlist() bRemove projectid[%d] Tag_id[%s] Anchor_id[%s] distance[%d]\n",
                            projectid, Tag_id, Anchor_id, distance);

                        TriggerDoorEventStruct * old_rangingData = (*rangingData);
                        triggerRangingList->erase(rangingData);
                        delete old_rangingData;
                    }
                    else
                    {
                        if ( (*rangingData)->action == 2 )
                            (*rangingData)->triggered = 0;

                        (*rangingData)->action = 1; // 0:none 1:enter 2:exit
                        (*rangingData)->distance = distance;
                        (*rangingData)->dataTime = getCurrentTimeInMilliSec();

                        // vilslog::printf ("DeviceEventCheckService()::put_to_dooreventlist() update projectid[%d] Tag_id[%s] Anchor_id[%s] distance[%d]\n",
                        //     projectid, Tag_id, Anchor_id, distance);

                    }
                    bFoundData = true;
                    break;
                }
            }
            if (!bFoundData && locatoNode->tafid == 2 )
            {
                TriggerDoorEventStruct * newData = new TriggerDoorEventStruct();
                newData->triggered = 0;
                newData->action = 1; // 0:none 1:enter 2:exit
                newData->nodeid = Anchor_id;
                newData->nodemacaddress = locatoNode->macaddress;
                newData->tagid = Tag_id;
                newData->distance = distance;
                newData->dataTime = getCurrentTimeInMilliSec();
                triggerRangingList->push_back(newData);

                // vilslog::printf ("DeviceEventCheckService()::put_to_dooreventlist() new TriggerDoorEventStruct projectid[%d] Tag_id[%s] Anchor_id[%s] distance[%d]\n",
                //     projectid, Tag_id, Anchor_id, distance);

            }
            bFound = true;
            break;
        }
    }

    if (!bFound && locatoNode->tafid == 2 )
    {
        ProjectTriggerDoorEventStruct * newPrjData = new ProjectTriggerDoorEventStruct();
        newPrjData->projectid = projectid;
        newPrjData->mTriggerDoorEventList = new std::list<TriggerDoorEventStruct*>();
        mProjectTriggerDoorEventList.push_back(newPrjData);

        TriggerDoorEventStruct * newData = new TriggerDoorEventStruct();
        newData->triggered = 0;
        newData->action = 1; // 0:none 1:enter 2:exit
        newData->nodeid = Anchor_id;
        newData->nodemacaddress = locatoNode->macaddress;
        newData->tagid = Tag_id;
        newData->distance = distance;
        newData->dataTime = getCurrentTimeInMilliSec();
        newPrjData->mTriggerDoorEventList->push_back(newData);

        vilslog::printf ("DeviceEventCheckService()::put_to_dooreventlist() new TriggerDoorEventStruct projectid[%d] Tag_id[%s] Anchor_id[%s] distance[%d]\n",
            projectid, Tag_id, Anchor_id, distance);

    }
}

int DeviceEventCheckService::sendNotification(AreaInOutEventStruct * event)
{
    vilslog::printf("\nDeviceEventCheckService::sendNotification() NodeId[%s] action[%d] AreaID[%s] AreaName[%s] MapAreaType[%d]\n\n",
                    event->NodeId.c_str(), event->action,
                    event->AreaID.c_str(), event->AreaName.c_str(), event->MapAreaType);
    std::string message = "";
    // message.append("醫師 ");
    message.append(event->NodeName);

    std::string action = "Enter";
    if (event->action == 1)
    {
        message.append(" 進入 ");
        action = "Enter";
    }
    else
    {
        message.append(" 離開 ");
        action = "Leave";
    }

    message.append(event->AreaName);

    // int maingroupid = 1;
    //
    // Find Locator in this area
    //
    std::list<NodeStruct*> * locatorNodeList = mDeviceManager->get_Locator_list(event->projectid);
    for (std::list<NodeStruct*>::iterator existed = locatorNodeList->begin(); existed != locatorNodeList->end(); existed++)
    {
        if ( strcmp(event->AreaID.c_str(), (*existed)->AreaId) == 0 )
        {
            vilslog::printf("\nDeviceEventCheckService::sendNotification() Send to Locator NodeId[%s] nodename[%s] event->MapAreaType[%d]\n\n",
                            (*existed)->nodeid, (*existed)->nodename, event->MapAreaType);

            if ( (event->MapAreaType == 5) || (event->MapAreaType == 6) ) // 5:病房  6:病床
            {
                NotificationItem * item = new NotificationItem(LOCATOR_PUSHNOTIFY_FUNCTION_4, event->projectid, NODE_TYPE_LOCATOR, (*existed)->nodeid, (*existed)->nodename, "0", "1", (*existed)->macaddress, "SMIMSVILS 事件通知", message);
                mNotifiactionQueue.add(item);
            }

            // std::string eventjson = convert_to_json_str(event->projectid, 7777, event->NodeId, action, event->AreaID, "0", (*existed)->nodeid);
            //
            // WorkItem * witem = new WorkItem("doctorevent", event->projectid, NODE_TYPE_LOCATOR, (*existed)->macaddress, eventjson.c_str());
            // witem->setMaingroupid((*existed)->maingroupid);
            // maingroupid = (*existed)->maingroupid;
            // mMySQLJobQueue.add(witem);

            int alertType = 1; //1:人員 2:固定裝置
            int alertAction = event->action;// 1:In 2:Out 3:alert 4:cancel
            SubscribeDataItem * subitem = new SubscribeDataItem("event", event->projectid, 7777, event->NodeId.c_str(), alertType, alertAction);
            mWebSocketqueue.add(subitem);

        }
        else
        if ( strcmp(event->AreaID.c_str(), (*existed)->AreaType5Id) == 0 )
        {
            vilslog::printf("\nDeviceEventCheckService::sendNotification() AreaType5Id Send to Locator NodeId[%s] nodename[%s] event->MapAreaType[%d]\n\n",
                            (*existed)->nodeid, (*existed)->nodename, event->MapAreaType);

            if ( (event->MapAreaType == 5) || (event->MapAreaType == 6) ) // 5:病房  6:病床
            {
                NotificationItem * item = new NotificationItem(LOCATOR_PUSHNOTIFY_FUNCTION_4, event->projectid, NODE_TYPE_LOCATOR, (*existed)->nodeid, (*existed)->nodename, "0", "1", (*existed)->macaddress, "SMIMSVILS 事件通知", message);
                mNotifiactionQueue.add(item);
            }

            // std::string eventjson = convert_to_json_str(event->projectid, 7777, event->NodeId, action, event->AreaID, "0", (*existed)->nodeid);
            //
            // WorkItem * witem = new WorkItem("doctorevent", event->projectid, NODE_TYPE_LOCATOR, (*existed)->macaddress, eventjson.c_str());
            // witem->setMaingroupid((*existed)->maingroupid);
            // maingroupid = (*existed)->maingroupid;
            // mMySQLJobQueue.add(witem);

            int alertType = 1; //1:人員 2:固定裝置
            int alertAction = event->action;// 1:In 2:Out 3:alert 4:cancel
            SubscribeDataItem * subitem = new SubscribeDataItem("event", event->projectid, 7777, event->NodeId.c_str(), alertType, alertAction);
            mWebSocketqueue.add(subitem);
        }
    }

    // doctor event
    // std::string eventjson = convert_to_json_str(event->projectid, 9999, event->NodeId.c_str(), action, event->AreaID.c_str(), "0", "");
    // WorkItem * witem = new WorkItem("fencealert", event->projectid, NODE_TYPE_TAG, event->NodeMacaddress.c_str(), eventjson.c_str());
    // witem->setMaingroupid(maingroupid);
    // mMySQLJobQueue.add(witem);

    return 0;
}
