/*
 * EventChecker.cpp
 *
 *  Created on: 2019/04/23
 *      Author: ikki
 */


#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <time.h>

#include "../common.h"
#include "EventChecker.h"

using namespace std;

EventChecker::EventChecker(wqueue<SubscribeDataItem*> & pWebSocketqueue, wqueue<NotificationItem*> & NotifiactionQueue, wqueue<WorkItem*> & MySQLJobQueue, DeviceManager * pDevManager, DBManager * dbManager)
    : mWebSocketqueue(pWebSocketqueue), mNotifiactionQueue(NotifiactionQueue), mMySQLJobQueue(MySQLJobQueue), mDeviceManager(pDevManager), mDbManager(dbManager)
{
    //mEventList = NULL;
    //mUpdateEventList = false;

    init();
}

EventChecker::~EventChecker()
{
    //clear_AreaInOut_vector(&mAreaInOutDataVector);
    //clear_event_list(mEventList);
    //delete mEventList;
    clear_ProjectEventInfoListStruct_vector(&mProjectEventInfoVector);
}

int EventChecker::init()
{
    //clear_AreaInOut_vector(&mAreaInOutDataVector);
    //updateEventList();

    clear_ProjectEventInfoListStruct_vector(&mProjectEventInfoVector);

    std::list<int> * prjidList = mDbManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);

        ProjectEventInfoListStruct * newInfo = createProjectEventInfo(projectid);

        mProjectEventInfoVector.push_back(newInfo);
    }
    prjidList->clear();
    delete prjidList;

    return 0;
}

ProjectEventInfoListStruct * EventChecker::createProjectEventInfo(int projectid)
{
    ProjectEventInfoListStruct * newInfo = new ProjectEventInfoListStruct();
    newInfo->projectid = projectid;
    newInfo->mEventList = NULL;
    clear_AreaInOut_vector(&newInfo->mAreaInOutDataVector);

    updateEventList(newInfo);

    return newInfo;
}

ProjectEventInfoListStruct * EventChecker::getProjectEventInfo(int projectid)
{
    ProjectEventInfoListStruct * eventInfo = NULL;
    for (unsigned int i=0; i< mProjectEventInfoVector.size(); i++)
    {
        if (mProjectEventInfoVector[i]->projectid == projectid)
        {
            eventInfo = mProjectEventInfoVector[i];
            break;
        }
    }

    if( eventInfo == NULL )
    {
        eventInfo = createProjectEventInfo(projectid);

        mProjectEventInfoVector.push_back(eventInfo);
    }

    return eventInfo;
}

int EventChecker::setUpdateEventList()
{
    mUpdateEventList = true;
    return 0;
}

int EventChecker::updateEventList()
{
    //printf("EventChecker::updateEventList()\n");
    std::list<int> * prjidList = mDbManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);

        bool bFound = false;
        for (std::vector<ProjectEventInfoListStruct*>::iterator i = mProjectEventInfoVector.begin(); i != mProjectEventInfoVector.end(); i++)
        {
            if ( (*i)->projectid == projectid )
            {
                updateEventList((*i));
                bFound = true;
                break;
            }
        }
        if (!bFound)
        {
            ProjectEventInfoListStruct * newInfo = createProjectEventInfo(projectid);
            mProjectEventInfoVector.push_back(newInfo);
        }
    }

    prjidList->clear();
    delete prjidList;

    // if (mEventList != NULL)
    // {
    //     clear_event_list(mEventList);
    //     delete mEventList;
    // }
    // MySQLDBAccess * baseDb = mDbManager->getBaseDB(1);
    //
    // mEventList = baseDb->read_events();
    //
    // //vilslog::printf("EventChecker::updateEventList() mEventList size[%d]\n", (int)mEventList->size());
    //
    // return (int)mEventList->size();
    return 0;
}

int EventChecker::updateEventList(ProjectEventInfoListStruct * eventInfo)
{
    if (eventInfo->mEventList != NULL)
    {
        clear_event_list(eventInfo->mEventList);
        delete eventInfo->mEventList;
    }
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(eventInfo->projectid);
    if (baseDb == NULL)
        return 0;

    eventInfo->mEventList = baseDb->read_events();

    //vilslog::printf("EventChecker::updateEventList() mEventList size[%d]\n", (int)mEventList->size());

    return (int)eventInfo->mEventList->size();
}

bool EventChecker::addNewEvent(AreaInOutEventStruct * event)
{
    bool bFound = false;
    bool bNewAction = false;

    ProjectEventInfoListStruct * eventInfo = getProjectEventInfo(event->projectid);

    for (unsigned int i=0; i< eventInfo->mAreaInOutDataVector.size(); i++)
    {
        // check AreaId
        if ( eventInfo->mAreaInOutDataVector[i]->getAreaId().compare(event->AreaID) == 0 )
        {
            // vilslog::printf ("EventChecker::addNewEvent() NodeId[%s] AreaID[%s] action[%d]\n",
            //      event->NodeId.c_str(), event->AreaID.c_str(), event->action);

            bNewAction = eventInfo->mAreaInOutDataVector[i]->setNewAction(event->NodeId.c_str(), event->tafid, event->action, event->AreaType0D);
            eventInfo->mAreaInOutDataVector[i]->updateAreaInfo(event->AreaName.c_str(), event->MapAreaType);
            bFound = true;
            break;
        }
    }
    if (!bFound)
    {
        // vilslog::printf ("EventChecker::addNewEvent() create new Vector NodeId[%s] AreaID[%s] action[%d]\n",
        //      event->NodeId.c_str(), event->AreaID.c_str(), event->action);

        AreaInOutDataClass * newData = new AreaInOutDataClass(AreaInOutData_BUFFER_SIZE);
        newData->init(event->AreaID.c_str(), event->AreaName.c_str(), event->MapAreaType);
        bNewAction = newData->setNewAction(event->NodeId.c_str(), event->tafid, event->action, event->AreaType0D);
        eventInfo->mAreaInOutDataVector.push_back(newData);
    }

    if (event->AreaType0D == 2) // 0:normal 1:room 2:danger
    {
        // vilslog::printf ("\n*****\nEventChecker::addNewEvent() NodeId[%s] AreaType0D[%d] AreaID[%s] AreaName[%s] action[%d]\n*****\n",
        //      event->NodeId.c_str(), event->AreaType0D, event->AreaID.c_str(), event->AreaName.c_str(), event->action);

        check_area_fenceAlert(event);
    }

    return bNewAction;
}

bool EventChecker::getTagAreaLastAction(int projectid, std::string NodeId, std::string AreaID, AreaInOutStruct & data)
{
    bool bFound = false;

    for (unsigned int i=0; i< mProjectEventInfoVector.size(); i++)
    {
        ProjectEventInfoListStruct * eventInfo = mProjectEventInfoVector[i];
        if (eventInfo->projectid == projectid)
        {
            for (unsigned int j=0; j< eventInfo->mAreaInOutDataVector.size(); j++)
            {
                if ( AreaID.compare(eventInfo->mAreaInOutDataVector[j]->getAreaId()) == 0 )
                {
                    bFound = eventInfo->mAreaInOutDataVector[j]->getTagLastAction(NodeId, data);
                    break;
                }
            }
            break;
        }
    }

    return bFound;
}

int EventChecker::check()
{
    if (mUpdateEventList)
    {
        updateEventList();
        mUpdateEventList = false;
    }

    check_allProject();

    return 0;
}

void EventChecker::check_allProject()
{
    for (unsigned int i=0; i< mProjectEventInfoVector.size(); i++)
    {
        ProjectEventInfoListStruct * eventInfo = mProjectEventInfoVector[i];
        check_Project(eventInfo);
    }
}

void EventChecker::check_Project(ProjectEventInfoListStruct * eventInfo)
{
    for (list <EventStruct *>::iterator event = eventInfo->mEventList->begin(); event != eventInfo->mEventList->end(); event++)
    {
//        vilslog::printf("===============\n");
//        vilslog::printf("EventChecker::check() eventid[%d] nodeid[%s] action[%s]\n",
//                        (*event)->eventid, (*event)->nodeid.c_str(), (*event)->action.c_str());
//        vilslog::printf("===============\n");

        FiredEventStruct * eventFired = NULL;

        //check action type
        if ( (*event)->action == "Enter" )
        {
            // eventFired = do_action_Enter(*event);
            eventFired = do_check_action(eventInfo, *event, 1);// 1:Enter 2:Leave
        }
        else
        if ( (*event)->action == "Leave" )
        {
            //eventFired = do_action_Leave(*event);
            eventFired = do_check_action(eventInfo, *event, 2);// 1:Enter 2:Leave
        }
        else
        if ( (*event)->action == "Stay" )
        {
            //eventFired = do_action_Stay(*event);
            eventFired = do_check_action(eventInfo, *event, 1);// 1:Enter 2:Leave
        }

        //cout << "EventChecker::check() eventFired=" << eventFired;

        if (eventFired != NULL)
        {
            //================================
            //
            // check if already fired in MySQL
            //
            MySQLDBAccess * baseDb = mDbManager->getBaseDB((*event)->projectid);
            if (baseDb == NULL)
                return;

            int ret = baseDb->check_fired_events((*event)->projectid, eventFired->eventid, eventFired->timestart);
            if ( ret == 0 )
            {
                int maingroupid = 1;

                WorkItem * witem = new WorkItem("event", (*event)->projectid, eventFired);
                witem->setMaingroupid(maingroupid);
                mMySQLJobQueue.add(witem);

                int alertType = 1; //1:人員 2:固定裝置
                int alertAction = 3;// 1:In 2:Out 3:alert 4:cancel
                SubscribeDataItem * item = new SubscribeDataItem("event", (*event)->projectid, eventFired->eventid, (*event)->nodeid.c_str(), alertType, alertAction);
                mWebSocketqueue.add(item);

                NodeStruct * tagnode = mDeviceManager->get_Tag_node((*event)->projectid, (*event)->nodeid.c_str());
                if ( tagnode != NULL )
                {
                    std::string message = convert_event_to_show_string(tagnode, (*event));
                    NotificationItem * item = new NotificationItem(TAG_PUSHNOTIFY_FUNCTION_7, (*event)->projectid, NODE_TYPE_TAG, tagnode->nodeid, tagnode->nodename, "0", to_string(maingroupid), tagnode->macaddress, "SMIMSVILS 觸發事件通知", message);
                    mNotifiactionQueue.add(item);
                }
            }
            //================================
            delete eventFired;
        }
    }
}

FiredEventStruct * EventChecker::do_check_action(ProjectEventInfoListStruct * eventInfo, EventStruct* event, int action)
{
    FiredEventStruct * newFiredEvents = NULL;

    for (unsigned int i=0; i< eventInfo->mAreaInOutDataVector.size(); i++)
    {
        // check area id
        // find interest area
        if ( event->areaid.compare(eventInfo->mAreaInOutDataVector[i]->getAreaId()) == 0 )
        {
            AreaInOutStruct data;
            //
            // check the last action , only enter is we want.
            //
            bool bFound = eventInfo->mAreaInOutDataVector[i]->getTagLastAction(event->nodeid, data);

            if ( data.action != action )
            {
                // maybe is out action, does not match stay action.
//                vilslog::printf ("EventChecker::do_action_Stay() data.action[%d] != action[%d] maybe is out action, does not match stay action.\n",
//                                 data.action, action);
//                vilslog::printf ("EventChecker::do_action_Stay() NodeId[%s] no Stay Action at AreaID[%s]\n",
//                                 event->nodeid.c_str(), event->areaid.c_str());
                break;
            }

            if (!bFound)
            {
                // vilslog::printf ("EventChecker::do_check_action() NodeId[%s] no valid Action at AreaID[%s]\n",
                //                  event->nodeid.c_str(), event->areaid.c_str());
                break;
            }

            newFiredEvents = check_event_mach(event, &data);

            break;
        }
    }

    return newFiredEvents;
}

FiredEventStruct * EventChecker::check_event_mach(EventStruct * event, AreaInOutStruct * data)
{
    FiredEventStruct * newFiredEvents = NULL;

    int event_issuetime = atoi(event->issuetime.c_str());

    time_t t_action_cur = getCurrentTimeInSec();
    time_t t_action = data->DataTime_t;
    int seconds = (int)(t_action_cur - t_action);
    int minutes = seconds/60;

    //
    // match issuetime
    //
    if (minutes >= event_issuetime && seconds > 1)
    {
        //
        // create fired event
        //
        newFiredEvents = new FiredEventStruct();
        newFiredEvents->projectid = event->projectid;
        newFiredEvents->eventid = event->eventid;
        newFiredEvents->eventjson = convert_event_to_json_str(event);
        newFiredEvents->timestart = data->DataTime;
        newFiredEvents->timeend = currentDateTimeForSQL();

//        vilslog::printf ("EventChecker::check_event_mach() New Event \n\tprojectid[%d] eventid[%d] timestart[%s] timeend[%s]\n",
//                         event->projectid, event->eventid, newFiredEvents->timestart.c_str(), newFiredEvents->timeend.c_str());
//        vilslog::printf ("EventChecker::check_event_mach() minutes[%d] event_issuetime[%d] seconds[%d]\n",
//                         minutes, event_issuetime, seconds);


    }
    return newFiredEvents;
}

std::string EventChecker::convert_event_to_json_str(EventStruct* event)
{
    std::string json_str = "{";

    char tmp[64];
    snprintf(tmp, 64, "%d", event->projectid);
    json_str += "\"projectid\":\"" + std::string(tmp) + "\"";
    json_str += ",";

    snprintf(tmp, 64, "%d", event->eventid);
    json_str += "\"eventid\":\"" + std::string(tmp) + "\"";
    json_str += ",";
    json_str += "\"nodeid\":\"" + event->nodeid + "\"";
    json_str += ",";
    json_str += "\"action\":\"" + event->action + "\"";
    json_str += ",";
    json_str += "\"areaid\":\"" + event->areaid + "\"";
    json_str += ",";
    json_str += "\"issuetime\":\"" + event->issuetime + "\"";
    json_str += "}";

    //printf("EventChecker::convert_to_json_str json_str[%s]\n", json_str.c_str());

    return json_str;
}

std::string EventChecker::convert_event_to_show_string(NodeStruct * tagnode, EventStruct* event)
{
    std::ostringstream sendstream;
    sendstream<<tagnode->nodename;
    sendstream<<" "<<get_Action_Name(event->action);

    // vilslog::printf("EventChecker::convert_event_to_show_string() nodeid[%s] action[%s] areaid[%s] nodename[%s] areaname[%s] issuename[%s]\n",
    //     event->nodeid.c_str(), event->action.c_str(), event->areaid.c_str(), event->nodename.c_str(), event->areaname.c_str(), event->issuename.c_str());

    std::string areaname = event->areaid;
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(event->projectid);
    if (baseDb != NULL)
    {
        areaname = baseDb->get_area_name(event->areaid.c_str());
    }
    sendstream<<" "<<areaname;
    sendstream<<" "<<get_Issue_Name(event->issuetime);

    return sendstream.str();
}

std::string EventChecker::get_Action_Name(std::string action)
{
    if ( action.compare("Enter") == 0 )
    {
        return std::string("進入");
    }
    else
    if ( action.compare("Leave") == 0 )
    {
        return std::string("離開");
    }
    else
    if ( action.compare("Stay") == 0 )
    {
        return std::string("停留");
    }
    return std::string("");
}

std::string EventChecker::get_Issue_Name(std::string issuetime)
{
    if ( issuetime.compare("0") == 0 )
    {
        return std::string("剛剛");
    }
    else
    if ( issuetime.compare("1") == 0 )
    {
        return std::string("1分鐘");
    }
    else
    if ( issuetime.compare("5") == 0 )
    {
        return std::string("5分鐘");
    }
    else
    if ( issuetime.compare("10") == 0 )
    {
        return std::string("10分鐘");
    }
    return std::string("");
}

// void EventChecker::dump_tag_area_inout_action()
// {
//     vilslog::printf ("DeviceManager::dump_tag_area_inout_action()\n===============\n");
//     for (int i=0; i< mAreaInOutDataVector.size(); i++)
//     {
//         vilslog::printf ("AreaId[%s] AreaName[%s] --- \n",
//                          mAreaInOutDataVector[i]->getAreaId().c_str(), mAreaInOutDataVector[i]->getAreaName().c_str());
//
//         mAreaInOutDataVector[i]->dump_tag_area_inout_action();
//     }
//     vilslog::printf ("===============\n");
// }

bool EventChecker::check_area_fenceAlert(AreaInOutEventStruct * event)
{
    bool bAlert = false;
    int alertType = 1; //1:人員 2:固定裝置
    int alertAction = event->action; // 1:in 2:out

    // check previous alert time
    int alertTimeDiff = check_node_AlertTime(event->projectid, event->NodeId.c_str(), event->AreaID.c_str(), alertAction); // in ms

    // vilslog::printf ("EventChecker::check_area_fenceAlert() alertTimeDiff[%d]\n", alertTimeDiff);

    if ( alertTimeDiff > SAME_FENCE_EVENT_TIMEOUT )
    {
        // vilslog::printf ("EventChecker::check_area_fenceAlert() alertTimeDiff[%d] > SAME_FENCE_EVENT_TIMEOUT area[%s] action[%d]\n",
        //     alertTimeDiff, event->AreaID.c_str(), alertAction);

        // 工作人員 -> 可以進出電子圍籬 不發警告
        NodeStruct * tagNode = mDeviceManager->get_Tag_node(event->projectid, event->NodeId.c_str());
        if( tagNode != NULL )
        {
            if ( tagNode->tafid == 4 )
            {
                vilslog::printf ("EventChecker::check_area_fenceAlert() 工作人員 -> 可以進出電子圍籬 不發警告!\n");
                // Tag role 0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師
                return false;
            }
        }

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
        std::string eventjson = convert_to_json_str(event->projectid, 9999, event->NodeId, action, event->AreaID, "0", "");

        WorkItem * witem = new WorkItem("fencealert", event->projectid, NODE_TYPE_TAG, event->NodeMacaddress.c_str(), eventjson.c_str());
        witem->setMaingroupid(tagNode->maingroupid);
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

        bAlert = true;
    }

    return bAlert;
}

int EventChecker::check_node_AlertTime(int projectid, const char * nodeid, const char * area_AreaId, int alertAction)
{
    int alertTimeDiff = SAME_FENCE_EVENT_TIMEOUT * 2;

    bool bFound = false;
    for (std::list<NodeEventTimeStruct*>::iterator event = mNodeEventTimeList.begin(); event != mNodeEventTimeList.end(); event++)
    {
        if( ((*event)->projectid == projectid) && ((*event)->nodeid.compare(nodeid) == 0) && ((*event)->areaid.compare(area_AreaId) == 0) )
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
        newEvent->areaid = area_AreaId;
        newEvent->alertAction = alertAction;
        newEvent->alertIssueTime = getCurrentTimeInMilliSec();

        mNodeEventTimeList.push_back(newEvent);
    }

    return alertTimeDiff;
}

int EventChecker::update_node_AlertTime(int projectid, const char * nodeid, const char * area_AreaId, int alertAction)
{
    for (std::list<NodeEventTimeStruct*>::iterator event = mNodeEventTimeList.begin(); event != mNodeEventTimeList.end(); event++)
    {
        if( ((*event)->projectid == projectid) && ((*event)->nodeid.compare(nodeid) == 0) && ((*event)->areaid.compare(area_AreaId) == 0) )
        {
            (*event)->alertIssueTime = getCurrentTimeInMilliSec();
            (*event)->alertAction = alertAction;
            break;
        }
    }

    return 0;
}

void EventChecker::clear_ProjectEventInfoListStruct_vector(std::vector<ProjectEventInfoListStruct*> * plist)
{
    for (std::vector<ProjectEventInfoListStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        if ((*i)->mEventList != NULL)
        {
            clear_event_list((*i)->mEventList);
            delete (*i)->mEventList;
        }
        clear_AreaInOut_vector(&(*i)->mAreaInOutDataVector);
    }
    plist->clear();
}
