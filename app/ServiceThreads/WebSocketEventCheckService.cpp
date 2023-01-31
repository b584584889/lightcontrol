/*
 *  WebSocketEventCheckService.cpp
 *
 *  Created on: 2018/10/31
 *      Author: ikki
 */

#include "WebSocketEventCheckService.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <string.h>
#include <list>
#include <ifaddrs.h>

WebSocketEventCheckService::WebSocketEventCheckService(wqueue<SubscribeDataItem*> & pWebSocketqueue, wqueue<AreaInOutEventStruct*> & pWebSocketAreaInOutEventQueue, wqueue<NotificationItem*> & NotifiactionQueue, wqueue<WorkItem*> & MySQLJobQueue, DeviceManager * pDevManager, DBManager * dbManager)
    : mWebSocketqueue(pWebSocketqueue), mWebSocketAreaInOutEventQueue(pWebSocketAreaInOutEventQueue), mNotifiactionQueue(NotifiactionQueue), mMySQLJobQueue(MySQLJobQueue), mDeviceManager(pDevManager), mDbManager(dbManager)
{

}

bool WebSocketEventCheckService::status()
{
    return mInitial;
}

std::string WebSocketEventCheckService::statusString()
{
    return mStatus;
}

void WebSocketEventCheckService::updateProject()
{
    mUpdateProject = true;
}

void * WebSocketEventCheckService::run()
{
    while(1)
    {
        try
        {
            mPrjidList = mDbManager->getAllProjectConfig();

            runloop();
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("WebSocketEventCheckService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "WebSocketEventCheckService::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void WebSocketEventCheckService::runloop()
{
    mInitial = true;
    mStatus = "WebSocketEventCheckService()::run() Initial successfully!";
    vilslog::printf ("WebSocketEventCheckService()::run() Initial successfully!\n");

    long long start1 = getSystemTime();
    long long sendTagEventToWebSocketTime = 0;
    long long updatePrjIdTime = 0;

    // available to process.
    while (1)
    {
        AreaInOutEventStruct * event = mWebSocketAreaInOutEventQueue.removetimed(1000);

        if ( event != NULL )
        {
            CheckEvent(event);
            delete event;
        }

        long long end1 = getSystemTime();
        long long difft = end1 - start1;
        //printf("wait_loop() time[%lld ms]\n", difft);

        sendTagEventToWebSocketTime += difft;
        //reloadAreaTime += difft;
        updatePrjIdTime += difft;

        if ( sendTagEventToWebSocketTime >= 100) // 100 miliseconds
        {
            sendTagEventToWebSocket();
            sendLocatorEventToWebSocket();
            sendTagEventToWebSocketTime = 0;
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
        usleep(10000);
    }
}

void WebSocketEventCheckService::CheckEvent(AreaInOutEventStruct * event)
{
    int alertAction = event->action;
    int maingroupid = 1;

    // 0:普通 1:電子圍欄 2:電子點名 3:房間 4:教室 5:病房 6:病床 999:虛擬牆
    if ( event->MapAreaType == 1 )
    {
        // 1:電子圍欄

        // check previous alert time
        int alertTimeDiff = check_node_AlertTime(event->projectid, event->NodeId.c_str(), event->AreaID.c_str(), alertAction); // in ms

        if ( alertTimeDiff > SAME_EVENT_TIMEOUT )
        {
            vilslog::printf ("WebSocketEventCheckService::CheckEvent() [%s] action[%d] area[%s] alertTimeDiff[%d] > SAME_EVENT_TIMEOUT[%d]\n",
                event->NodeId.c_str(), alertAction, event->AreaName.c_str(), alertTimeDiff, SAME_EVENT_TIMEOUT);

            // 工作人員 -> 可以進出電子圍籬 不發警告
            NodeStruct * tagNode = mDeviceManager->get_Tag_node(event->projectid, event->NodeId.c_str());
            if( tagNode != NULL )
            {
                if ( tagNode->tafid == 4 )
                {
                    vilslog::printf ("WebSocketEventCheckService::CheckEvent() 工作人員 -> 可以進出電子圍籬 不發警告!\n");

                    // Tag role 0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師
                    return;
                }
            }

            send_fenceAlert_event(event);
        }

    }
    else
    if ( event->MapAreaType == 2 )
    {
        // 2:電子點名
    }
    else
    if ( event->MapAreaType == 5 )
    {
        // 5:病房
        // tafid 5:醫師
        if ( event->tafid == 5 )
        {
            vilslog::printf("\nWebSocketEventCheckService::CheckEvent() 5 nodename[%s] alertAction[%d] area_AreaName[%s]\n\n",
                event->NodeName.c_str(), alertAction, event->AreaName.c_str());

            std::string action = "Enter";
            if (alertAction == 1)
                action = "Enter";
            else
            if (alertAction == 2)
                action = "Leave";

            std::string eventjson = convert_to_json_str(event->projectid, 9999, event->NodeId.c_str(), action, event->AreaID.c_str(), "0", "");
            WorkItem * witem = new WorkItem("fencealert", event->projectid, NODE_TYPE_TAG, event->NodeMacaddress.c_str(), eventjson.c_str());
            witem->setMaingroupid(maingroupid);
            mMySQLJobQueue.add(witem);
        }
    }
    else
    if ( event->MapAreaType == 6 )
    {
        // 6:病床
        // tafid 5:醫師
        if ( event->tafid == 5 )
        {
            vilslog::printf("\nWebSocketEventCheckService::CheckEvent() 6 nodename[%s] alertAction[%d] area_AreaName[%s]\n\n",
                event->NodeName.c_str(), alertAction, event->AreaName.c_str());

            std::string action = "Enter";
            if (alertAction == 1)
                action = "Enter";
            else
            if (alertAction == 2)
                action = "Leave";

            std::string eventjson = convert_to_json_str(event->projectid, 9999, event->NodeId.c_str(), action, event->AreaID.c_str(), "0", "");
            WorkItem * witem = new WorkItem("fencealert", event->projectid, NODE_TYPE_TAG, event->NodeMacaddress.c_str(), eventjson.c_str());
            witem->setMaingroupid(maingroupid);
            mMySQLJobQueue.add(witem);
        }
    }
}

void WebSocketEventCheckService::send_fenceAlert_event(AreaInOutEventStruct * event)
{
    int alertType = 1;
    int alertAction = event->action;
    int maingroupid = 1;

    SubscribeDataItem * item4 = new SubscribeDataItem("fenceAlert", 0, event->NodeName.c_str(), event->NodeMacaddress.c_str(), event->NodeId.c_str(), event->projectid,
        alertType, alertAction, event->AreaID.c_str(), event->AreaName.c_str());
    mWebSocketqueue.add(item4);

    std::string actionCh = "進入";
    std::string action = "Enter";
    if (alertAction == 1)
    {
        actionCh = "進入";
        action = "Enter";
    }
    else
    if (alertAction == 2)
    {
        actionCh = "離開";
        action = "Leave";
    }
    std::string eventjson = convert_to_json_str(event->projectid, 9999, event->NodeId.c_str(), action, event->AreaID.c_str(), "0", "");

    WorkItem * witem = new WorkItem("fencealert", event->projectid, NODE_TYPE_TAG, event->NodeMacaddress.c_str(), eventjson.c_str());
    witem->setMaingroupid(maingroupid);
    mMySQLJobQueue.add(witem);

    std::string message = event->NodeName;
    message.append(" ");
    message.append(actionCh);
    message.append(" 危險區域 ");
    message.append(event->AreaName);
    NotificationItem * item = new NotificationItem(TAG_PUSHNOTIFY_FUNCTION_4, event->projectid, NODE_TYPE_TAG, event->NodeId.c_str(), event->NodeName.c_str(), "0", "1", event->NodeMacaddress.c_str(), "SMIMSVILS 危險區域通知", message);
    mNotifiactionQueue.add(item);

    //
    // update alert time
    //
    update_node_AlertTime(event->projectid, event->NodeId.c_str(), event->AreaID.c_str(), alertAction); // in ms

}

void WebSocketEventCheckService::sendTagEventToWebSocket()
{
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        sendTagEventToWebSocket(projectid);
    }
}

//void WebSocketEventCheckService::sendTagEventToWebSocket(ProjectAreaTypeStruct * areaTypeInfo)
void WebSocketEventCheckService::sendTagEventToWebSocket(int projectid)
{
    VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);
    std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);
    for (std::list<NodeStruct*>::iterator existed = tagNodeList->begin(); existed != tagNodeList->end(); existed++)
    {
        //int tagFloor = mDeviceManager->getTagFloor(projectid, (*existed)->nodeid);
        //int projectID = resolveProjectId(tagFloor);

        if ( (*existed)->alive > 0 )
        {
            int pos[3];
            if (vilssysConfig->SmoothingResult == 1)
            {
                //glSmoothingResult->getResult((*existed)->index, pos);
                //glPosCalService->getSmoothingResult((*existed)->index, pos);
                pos[0] = (int)(*existed)->smoothingPosX;
                pos[1] = (int)(*existed)->smoothingPosY;
                pos[2] = (int)(*existed)->smoothingPosZ;
            }
            else
            {
                pos[0] = (int)(*existed)->posX;
                pos[1] = (int)(*existed)->posY;
                pos[2] = (int)(*existed)->posZ;
            }
            SubscribeDataItem * item = new SubscribeDataItem("coord", (*existed)->index, (*existed)->nodename, (*existed)->macaddress, (*existed)->nodeid, projectid,
                        pos[0], pos[1], pos[2], (*existed)->voltage,
                        (*existed)->temperature, (*existed)->AreaId, (*existed)->AreaName);
            mWebSocketqueue.add(item);

            if ( (*existed)->voltage_changed > 0 )
            {
                SubscribeDataItem * item2 = new SubscribeDataItem("power", (*existed)->index, (*existed)->nodename, (*existed)->macaddress, (*existed)->nodeid, projectid,
                    (*existed)->voltage, (*existed)->temperature);
                mWebSocketqueue.add(item2);
            }

            if ( ((*existed)->voltage_low > 0) && ((*existed)->voltage_changed > 0) )
            {
                SubscribeDataItem * item3 = new SubscribeDataItem("powerAlert", (*existed)->index, (*existed)->nodename, (*existed)->macaddress, (*existed)->nodeid, projectid,
                    (*existed)->voltage, (*existed)->temperature);
                mWebSocketqueue.add(item3);

                vilslog::printf("WebSocketEventCheckService::sendTagEventToWebSocket() powerAlert macaddress[%s] voltage[%s]\n",
                    (*existed)->macaddress, (*existed)->voltage);

                std::string message = (*existed)->nodename;
                message.append(" 電量過低");
                message.append((*existed)->voltage);
                message.append("V");
                NotificationItem * item = new NotificationItem(TAG_PUSHNOTIFY_FUNCTION_3, projectid, (*existed)->nodetype, (*existed)->nodeid, (*existed)->nodename, to_string((*existed)->userid), "1", (*existed)->macaddress, "SMIMSVILS 低電量通知", message);
                mNotifiactionQueue.add(item);
            }

            // reset voltage value
            (*existed)->voltage_changed = 0;

        }

        if ((*existed)->alert > 0 || (*existed)->alert0D > 0)
        {
            std::string locatorName = "";
            std::string locatorId = "";

            vilslog::printf("sendTagEventToWebSocket() (*existed)->alert[%d] (*existed)->alert0D[%d]\n", (*existed)->alert, (*existed)->alert0D);

            int alertType = 1; //1:人員 2:固定裝置
            if ((*existed)->alert > 0)
                alertType = 1;//1:人員
            if ((*existed)->alert0D > 0)
            {
                alertType = 2;// 2:固定裝置
                if ( strlen((*existed)->LoRAMacaddress) > 0)
                {
                    NodeStruct * LocatorNode = mDeviceManager->get_Locator_node(projectid, (*existed)->LoRAMacaddress);
                    if (LocatorNode != NULL)
                    {
                        locatorName = LocatorNode->nodename;
                        locatorId = LocatorNode->nodeid;
                    }
                }
            }
            //printf("sendTagEventToWebSocket() sos nodename[%s] nodename[%s] locatorName[%s] locatorId[%s]\n",
            //            (*existed)->nodename, (*existed)->nodeid, locatorName.c_str(), locatorId.c_str());

            SubscribeDataItem * item1 = new SubscribeDataItem("sos", (*existed)->index, (*existed)->nodename, (*existed)->macaddress, (*existed)->nodeid, projectid,
                locatorName.c_str(), locatorId.c_str(),
                alertType, (*existed)->AreaId, (*existed)->AreaName);
            mWebSocketqueue.add(item1);

            //
            // alert timeout
            //
            time_t currentTime = getCurrentTimeInMilliSec();
            time_t diffTime = currentTime -  (*existed)->alertIssuTime;
            //printf("sendTagAlertToClient() alert timeout  diffTime(%.02f)\n", (float)diffTime);
            if ( (float)diffTime > (float)vilssysConfig->TagAlertTimeout)
            {
                // vilslog::printf("WebSocketEventCheckService::sendTagEventToWebSocket() alert timeout diffTime(%.02f) TagAlertTimeout(%.02f)\n",
                //     (float)diffTime, (float)systemConfig.TagAlertTimeout);
                (*existed)->alert = 0;
                (*existed)->alert0D = 0;
            }
        }

    }
}


void WebSocketEventCheckService::sendLocatorEventToWebSocket()
{
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        sendLocatorEventToWebSocket(projectid);
    }
}

void WebSocketEventCheckService::sendLocatorEventToWebSocket(int projectid)
{
    VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);
    std::list<NodeStruct*> * LocatorNodeList = mDeviceManager->get_Locator_list(projectid);
    for (std::list<NodeStruct*>::iterator existed = LocatorNodeList->begin(); existed != LocatorNodeList->end(); existed++)
    {
        if ((*existed)->alert > 0 || (*existed)->alert0D > 0)
        {
            //printf("sendLocatorEventToWebSocket() [%d] alive[%d] alert[%d] alert0D[%d]\n",
            //            (*existed)->index, (*existed)->alive, (*existed)->alert, (*existed)->alert0D);

            int alertType = 1; //1:人員 2:固定裝置
            if ((*existed)->alert > 0)
                alertType = 1;//1:人員
            if ((*existed)->alert0D > 0)
                alertType = 2;// 2:固定裝置
            SubscribeDataItem * item1 = new SubscribeDataItem("sos", (*existed)->index, (*existed)->nodename, (*existed)->macaddress, (*existed)->nodeid, projectid,
                (*existed)->nodename, (*existed)->nodeid,
                alertType, (*existed)->AreaId, (*existed)->AreaName);
            mWebSocketqueue.add(item1);

            //
            // alert timeout
            //
            time_t currentTime = getCurrentTimeInMilliSec();
            time_t diffTime = currentTime -  (*existed)->alertIssuTime;
            //printf("sendTagAlertToClient() alert timeout  diffTime(%.02f)\n", (float)diffTime);
            if ( (float)diffTime > (float)vilssysConfig->TagAlertTimeout)
            {
                vilslog::printf("WebSocketEventCheckService::sendLocatorEventToWebSocket() alert timeout  diffTime(%.02f)\n", (float)diffTime);
                (*existed)->alert = 0;
                (*existed)->alert0D = 0;
            }
        }
    }
}

int WebSocketEventCheckService::check_node_AlertTime(int projectid, const char * nodeid, const char * AreaId, int alertAction)
{
    int alertTimeDiff = SAME_EVENT_TIMEOUT * 2;

    bool bFound = false;
    for (std::list<NodeEventTimeStruct*>::iterator event = mNodeEventTimeList.begin(); event != mNodeEventTimeList.end(); event++)
    {
        if( ((*event)->projectid == projectid) && ((*event)->nodeid.compare(nodeid) == 0) && ((*event)->areaid.compare(AreaId) == 0) )
        {
            if ( (*event)->alertAction == alertAction)
            {
                // same event
                //time_t currentTime = getCurrentTimeInMilliSec();
                //alertTimeDiff = currentTime -  (*event)->alertIssueTime;

                //
                // no timeout
                //
                alertTimeDiff = 0;
            }
            else
            {
                // different event in -> out or out -> in
                // return timeout
            }

            bFound = true;
            break;
        }
    }

    if(!bFound)
    {
        NodeEventTimeStruct * newEvent = new NodeEventTimeStruct();
        newEvent->projectid = projectid;
        newEvent->nodeid = nodeid;
        newEvent->areaid = AreaId;
        newEvent->alertAction = alertAction;
        newEvent->alertIssueTime = getCurrentTimeInMilliSec();

        mNodeEventTimeList.push_back(newEvent);
    }

    return alertTimeDiff;
}

int WebSocketEventCheckService::update_node_AlertTime(int projectid, const char * nodeid, const char * AreaId, int alertAction)
{
    for (std::list<NodeEventTimeStruct*>::iterator event = mNodeEventTimeList.begin(); event != mNodeEventTimeList.end(); event++)
    {
        if( ((*event)->projectid == projectid) && ((*event)->nodeid.compare(nodeid) == 0) && ((*event)->areaid.compare(AreaId) == 0) )
        {
            (*event)->alertIssueTime = getCurrentTimeInMilliSec();
            (*event)->alertAction = alertAction;
            break;
        }
    }

    return 0;
}
