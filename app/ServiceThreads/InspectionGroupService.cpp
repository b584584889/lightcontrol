/*
 *  InspectionGroupService.cpp
 *
 *  Created on: 2020/03/03
 *      Author: ikki
 */

#include "InspectionGroupService.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <time.h>
#include <ctime>

#include <string.h>
#include <list>
#include <ifaddrs.h>

#include "../common.h"
#include "../logger/Logger.h"

InspectionGroupService::InspectionGroupService(wqueue<SubscribeDataItem*> & pWebSocketqueue, wqueue<DataItem*> & InspectionJobQueue, wqueue<NotificationItem*> & NotifiactionQueue, wqueue<WorkItem*> & MySQLJobQueue, DeviceManager * pDevManager, DBManager * dbManager)
    : mWebSocketqueue(pWebSocketqueue), mInspectionJobQueue(InspectionJobQueue), mNotifiactionQueue(NotifiactionQueue), mMySQLJobQueue(MySQLJobQueue), mDeviceManager(pDevManager), mDbManager(dbManager)
{
}

InspectionGroupService::~InspectionGroupService()
{
}

bool InspectionGroupService::status()
{
    return mInitial;
}

std::string InspectionGroupService::statusString()
{
    return mStatus;
}

void * InspectionGroupService::run()
{
    while(1)
    {
        try
        {
            runloop();
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("InspectionGroupService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "InspectionGroupService()::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void InspectionGroupService::runloop()
{
    mInitial = true;
    mStatus = "InspectionGroupService()::run() Initial successfully!";
    vilslog::printf ("InspectionGroupService()::run() Initial successfully!\n");

    long long start1 = getSystemTime();
    long long end1 = getSystemTime();
    long long difft = end1 - start1;
    long long SimulationTime = 0;
    long long InspectionTime = 0;
    long long reloadInspectionTime = 0;

	initProjectInspectionInfo();

    vilslog::printf("InspectionGroupService::run() mProjectInspectionInfoVector.size()[%d]\n", (int)mProjectInspectionInfoVector.size());

    // available to process.
    while (1)
    {
        DataItem * item = mInspectionJobQueue.removetimed(1000);
        if( item != NULL )
        {
            //printf("InspectionGroupService::run() item:m_msgType[%s]\n", item->m_msgType.c_str());

            processDataItem(item);
            delete item;
        }

        end1 = getSystemTime();
        difft = end1 - start1;
        //printf("wait_loop() time[%lld ms]\n", difft);

        SimulationTime += difft;
        InspectionTime += difft;
        reloadInspectionTime += difft;

        if ( SimulationTime >= 1000) // 1 seconds
        {
            InspectionSimulationEvent();
            SimulationTime = 0;
        }

        if ( InspectionTime >= 5000) // 5 seconds
        {
            InspectionGroupEvent();
            InspectionTime = 0;
        }

        if ( reloadInspectionTime >= 600000) // 600 seconds
        {
            dump_cache_data();

            reloadInspectionList();
            reloadInspectionTime = 0;
        }
        start1 = end1;
    }
}

void InspectionGroupService::dump_cache_data()
{
    for (unsigned int i=0; i< mProjectInspectionInfoVector.size(); i++)
    {
        mProjectInspectionInfoVector[i]->m_CacheDB->loadOrSaveDb(mProjectInspectionInfoVector[i]->projectid, 1);
    }
}

void InspectionGroupService::print_SimulationData()
{
    vilslog::printf("\nInspectionGroupService::print_SimulationData() mProjectInspectionInfoVector.size()[%d]\n", (int)mProjectInspectionInfoVector.size());

    for (std::vector<ProjectInspectionInfoStruct*>::iterator i = mProjectInspectionInfoVector.begin(); i != mProjectInspectionInfoVector.end(); i++)
    {
        vilslog::printf("\nInspectionGroupService::print_SimulationData() ProjectID[%d] mSimulationList.size()[%d]\n",
            (*i)->projectid, (int)(*i)->mSimulationList.size());

        for (std::list<InspectorSimulationData*>::iterator simu = (*i)->mSimulationList.begin(); simu != (*i)->mSimulationList.end(); simu++)
        {
            vilslog::printf("\nInspectionGroupService::print_SimulationData() GroupID[%d] bStart[%d] AnchorNodeId[%s] Inspector[%s]\n",
                (*simu)->GroupID, (*simu)->bStart, (*simu)->AnchorNodeId.c_str(), (*simu)->Inspector.c_str());
            vilslog::printf("InspectionGroupService::print_SimulationData() InspectionStartTime[%s] GroupStartTime[%s] GroupEndTime[%s]\n",
                time_tToString((*simu)->InspectionStartTime).c_str(), time_tToString((*simu)->GroupStartTime).c_str(), time_tToString((*simu)->GroupEndTime).c_str());
            vilslog::printf("InspectionGroupService::print_SimulationData() setInspectionTime[%d] setTravelTimeTime[%d] InspectionPointDistance[%d] InspectionPointMinTime[%d] InspectionPointMaxTime[%d] InspectionPointTravelTime[%d]\n",
                (*simu)->setInspectionTime, (*simu)->setTravelTimeTime, (*simu)->InspectionPointDistance, (*simu)->InspectionPointMinTime, (*simu)->InspectionPointMaxTime, (*simu)->InspectionPointTravelTime);
        }
    }
}

int InspectionGroupService::processDataItem(DataItem * dataItem)
{
    if ( strcmp(dataItem->getType(), "RangingFiltered") == 0 )
    {
        do_RangingFiltered(dataItem);
    }
    else
    if ( strcmp(dataItem->getType(), "updateinspection") == 0 )
    {
        vilslog::printf("InspectionGroupService::processDataItem() updateinspection\n");
        reloadInspectionList();
    }
    else
    if ( strcmp(dataItem->getType(), "PowerOff") == 0 )
    {
        vilslog::printf("InspectionGroupService::processDataItem() PowerOff ProjectID[%d] NodeId[%s]\n", dataItem->getProjectID(), dataItem->getNodeId());
        send_All_AlertMessage(dataItem->getProjectID(), dataItem->getNodeId(), "巡檢裝置目前斷電，請盡快處理！");
    }
    else
    if ( strcmp(dataItem->getType(), "SensorAlert") == 0 )
    {
        vilslog::printf("InspectionGroupService::processDataItem() SensorAlert ProjectID[%d] NodeId[%s]\n", dataItem->getProjectID(), dataItem->getNodeId());
        send_All_AlertMessage(dataItem->getProjectID(), dataItem->getNodeId(), "附近有異常人物入侵, 請儘速巡視!");
    }
    else
    if ( strcmp(dataItem->getType(), "restartInspectionGroup") == 0 )
    {
        vilslog::printf("InspectionGroupService::processDataItem() restartInspectionGroup ProjectID[%d] GroupID[%d]\n",
            dataItem->getProjectID(), dataItem->getGroupID());

    }
    else
    if ( strcmp(dataItem->getType(), "restartInspectionPoint") == 0 )
    {
        vilslog::printf("InspectionGroupService::processDataItem() restartInspectionPoint ProjectID[%d] GroupID[%d] PointID[%d]\n",
            dataItem->getProjectID(), dataItem->getGroupID(), dataItem->getPointID());

    }
    return 0;
}

int InspectionGroupService::do_RangingFiltered(DataItem * item)
{
    char Tag_id[64];
    char Anchor_id[64];
    sprintf(Tag_id, "%s", item->getTagId());
    sprintf(Anchor_id, "%s", item->getAnchorId());
    int projectid = item->getProjectID();

    time_t cutDataTimeStamp = item->getTimestamp_t(); // ms -> second
    if ( strlen(item->getTimestamp()) > 0 )
    {
        cutDataTimeStamp = StringTimeZoneDateTimeTotime_t(item->getTimestamp());
    }

    do_RecordTagActivity(projectid, Tag_id, Anchor_id, item->getDistance());

    ProjectInspectionInfoStruct * inspectionInfo = getProjectInspectionInfo(projectid);

    bool bFound = false;
    for (unsigned int i=0; i< inspectionInfo->mAnchorRangingMapVector.size(); i++)
    {
        // check AnchorId
        if ( inspectionInfo->mAnchorRangingMapVector[i]->getAnchorId().compare(Anchor_id) == 0 )
        {
            inspectionInfo->mAnchorRangingMapVector[i]->putRangingData(Tag_id, item->getDistance(), cutDataTimeStamp);
            bFound = true;
            break;
        }
    }
    if (!bFound)
    {
        InspectionAnchorRangingDataClass * newData = new InspectionAnchorRangingDataClass();
        newData->init(Anchor_id);
        newData->putRangingData(Tag_id, item->getDistance(), cutDataTimeStamp);
        inspectionInfo->mAnchorRangingMapVector.push_back(newData);
    }
    return 1;
}

int InspectionGroupService::do_RecordTagActivity(int projectid, std::string TagNodeId, std::string AnchorNodeId, int Distance)
{
    std::string today = TodayDate();

    ProjectInspectionInfoStruct * inspectionInfo = getProjectInspectionInfo(projectid);

    for (std::list<InspectorActivityData*>::iterator activityData = inspectionInfo->mInspectorActivityList.begin(); activityData != inspectionInfo->mInspectorActivityList.end(); )
    {
        if ( ((*activityData)->AnchorNodeId.compare(AnchorNodeId) == 0) &&
             ((*activityData)->Inspector.compare(TagNodeId) == 0) )
        {
            //
            // check timeout
            //
            if ( (*activityData)->LeaveTime > 0 )
            {
                int t_cur = (int)time(0);
                int t_diff = (int)(t_cur - (*activityData)->LeaveTime);
                if ( t_diff > INSPECTION_HEHAVIOR_RECORD_LEAVE_TIMEOUT )
                {
#ifdef INSPECTION_PRINT_LOG
                    vilslog::printf ("InspectionGroupService::do_RecordTagActivity() INSPECTION_HEHAVIOR_RECORD_LEAVE_TIMEOUT t_diff[%d] > [%d]\n", t_diff, INSPECTION_HEHAVIOR_RECORD_LEAVE_TIMEOUT);
#endif

                    InspectorActivityData * old_activityData = (*activityData);
                    activityData = inspectionInfo->mInspectorActivityList.erase(activityData);
                    delete old_activityData;

                    continue;
                }
            }
            else
            {
                if ( (*activityData)->RecordTime > 0 )
                {
                    int t_cur = (int)time(0);
                    int t_diff = (int)(t_cur - (*activityData)->RecordTime);
                    if ( t_diff > INSPECTION_HEHAVIOR_RECORD_MAX_TIMEOUT )
                    {
#ifdef INSPECTION_PRINT_LOG
                        vilslog::printf ("InspectionGroupService::do_RecordTagActivity() INSPECTION_HEHAVIOR_RECORD_MAX_TIMEOUT t_diff[%d] > [%d]\n", t_diff, INSPECTION_HEHAVIOR_RECORD_MAX_TIMEOUT);
#endif
                        InspectorActivityData * old_activityData = (*activityData);
                        activityData = inspectionInfo->mInspectorActivityList.erase(activityData);
                        delete old_activityData;

                        continue;
                    }
                }
            }

            InspectorBehaviorData behavior;
            behavior.GroupID = (*activityData)->GroupID;
            behavior.PointID = (*activityData)->PointID;
            behavior.PointIndex = (*activityData)->PointIndex;
            behavior.Distance = Distance;
            behavior.AnchorNodeId = AnchorNodeId;
            behavior.Inspector = TagNodeId;
            behavior.DateTime = today;

            WorkItem * witem = new WorkItem("InspectorBehavior", projectid, behavior);
            mMySQLJobQueue.add(witem);
        }
        activityData++;
    }
    return 0;
}

void InspectionGroupService::initProjectInspectionInfo()
{
    clear_ProjectInspectionInfoStruct_vector(&mProjectInspectionInfoVector);

    std::list<int> * prjidList = mDbManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);

        ProjectInspectionInfoStruct * newInfo = createProjectInspectionInfo(projectid);

        mProjectInspectionInfoVector.push_back(newInfo);
    }

    prjidList->clear();
    delete prjidList;
}

ProjectInspectionInfoStruct * InspectionGroupService::createProjectInspectionInfo(int projectid)
{
    ProjectInspectionInfoStruct * newInfo = new ProjectInspectionInfoStruct();
    newInfo->projectid = projectid;
    newInfo->mInspectionGroupList = NULL;
    newInfo->mInspectionGroupVector.clear();
    newInfo->m_CacheDB = create_cacheDB(projectid, newInfo);

    reloadInspectionList(newInfo);

    return newInfo;
}

ProjectInspectionInfoStruct * InspectionGroupService::getProjectInspectionInfo(int projectid)
{
    ProjectInspectionInfoStruct * inspectionInfo = NULL;
    for (unsigned int i=0; i< mProjectInspectionInfoVector.size(); i++)
    {
        if (mProjectInspectionInfoVector[i]->projectid == projectid)
        {
            inspectionInfo = mProjectInspectionInfoVector[i];
            break;
        }
    }

    if( inspectionInfo == NULL )
    {
        vilslog::printf("InspectionGroupService::getProjectInspectionInfo() projectid[%d] inspectionInfo == NULL\n", projectid);

        inspectionInfo = createProjectInspectionInfo(projectid);

        mProjectInspectionInfoVector.push_back(inspectionInfo);
    }

    return inspectionInfo;
}

InspectionInfoDBAccess * InspectionGroupService::create_cacheDB(int projectid, ProjectInspectionInfoStruct * inspectionInfo)
{
    InspectionInfoDBAccess * CacheDB = new InspectionInfoDBAccess();
    CacheDB->init();

    // loda sqlite3 cache from file
    CacheDB->loadOrSaveDb(projectid, 0);
    bool bExisted = CacheDB->checkDBExist();

    vilslog::printf("InspectionGroupService::create_cacheDB() m_CacheDB projectid[%d] bExisted[%d]\n", projectid, bExisted);

    if (bExisted)
    {
        CacheDB->check_valid_table();

        //
        // read cache data
        //
        std::list<SendNotificationTimeData*> * notificationTimeList = CacheDB->getSendNotificationTimeList();
        for (std::list<SendNotificationTimeData*>::iterator cache_ntime = notificationTimeList->begin(); cache_ntime != notificationTimeList->end(); cache_ntime++)
        {
            bool bFound = false;
            for (std::list<SendNotificationTimeData*>::iterator ntime = inspectionInfo->mNotificationTimeList.begin(); ntime != inspectionInfo->mNotificationTimeList.end(); ntime++)
            {
                if ( ((*cache_ntime)->GroupID == (*ntime)->GroupID) && ((*cache_ntime)->PointID == (*ntime)->PointID) && ((*cache_ntime)->MessageType == (*ntime)->MessageType) )
                {
                    (*ntime)->SendTime = (*cache_ntime)->SendTime;
                    (*ntime)->timeout = (*cache_ntime)->timeout;

                    bFound = true;
                    break;
                }
            }

            if (!bFound)
            {
                SendNotificationTimeData * ntime = new SendNotificationTimeData();
                ntime->GroupID = (*cache_ntime)->GroupID;
                ntime->PointID = (*cache_ntime)->PointID;
                ntime->SendTime = (*cache_ntime)->SendTime;;
                ntime->MessageType = (*cache_ntime)->MessageType;
                ntime->timeout = (*cache_ntime)->timeout;
                inspectionInfo->mNotificationTimeList.push_back(ntime);

                vilslog::printf("InspectionGroupService::create_cacheDB()  copy cache data  projectid[%d] GroupID[%d] PointID[%d] SendTime[%d] MessageType[%d]\n",
                    projectid, ntime->GroupID, ntime->PointID, ntime->SendTime, ntime->MessageType);
            }
        }

        clear_SendNotificationTimeData_list(notificationTimeList);
        delete notificationTimeList;
    }
    else
    if (!bExisted)
    {
        CacheDB->init();
    }
    return CacheDB;
}

int InspectionGroupService::InspectionSimulationEvent()
{
    time_t t_Current = time(0);

    for (std::vector<ProjectInspectionInfoStruct*>::iterator i = mProjectInspectionInfoVector.begin(); i != mProjectInspectionInfoVector.end(); i++)
    {
        for (std::list<InspectorSimulationData*>::iterator simu = (*i)->mSimulationList.begin(); simu != (*i)->mSimulationList.end(); simu++)
        {
            if( (*simu)->bStart )
            {
                if ( (t_Current >= (*simu)->GroupStartTime) && (t_Current <= (*simu)->GroupEndTime) )
                {
                    int runtime = (int)(t_Current - (*simu)->InspectionStartTime);

                    //
                    // travel time
                    //
                    // if ( runtime < ((*simu)->InspectionPointTravelTime + 3) )
                    // {
                    //     // vilslog::printf("InspectionSimulationEvent() GroupID[%d] runtime[%d] InspectionPointTravelTime[%d]\n",
                    //     //     (*simu)->GroupID, runtime, (*simu)->InspectionPointTravelTime);
                    //
                    //
                    //     //  設定 巡檢點轉換時間 0: in Tiime 1:過長 2:過短
                    //     if ( (*simu)->setTravelTimeTime == 1)
                    //     {
                    //         continue;
                    //     }
                    // }

                    if ( runtime < (*simu)->InspectionPointMinTime )
                    {
                        int randDist = (rand() % 50) + 5;
                        int curDistance = (*simu)->InspectionPointDistance - randDist;

                        DataItem * sendItemckp = new DataItem("RangingFiltered", (*i)->projectid, (*simu)->Inspector.c_str(), (*simu)->AnchorNodeId.c_str(), curDistance, 1, t_Current);
                        mInspectionJobQueue.add(sendItemckp);

                        //  設定 巡檢時間 0: in Tiime 1:過長 2:過短
                        if ( (*simu)->setInspectionTime == 2 )
                        {
                            if ( runtime > ((*simu)->InspectionPointMinTime - 5) )
                            {
                                int randDist = (rand() % 50) + 5;
                                int curDistance = (*simu)->InspectionPointDistance + randDist;

                                DataItem * sendItemckp = new DataItem("RangingFiltered", (*i)->projectid, (*simu)->Inspector.c_str(), (*simu)->AnchorNodeId.c_str(), curDistance, 1, t_Current);
                                mInspectionJobQueue.add(sendItemckp);
                            }
                            continue;
                        }
                    }
                    else
                    {
                        if ( runtime < (*simu)->InspectionPointMaxTime )
                        {
                            int randDist = (rand() % ((*simu)->InspectionPointDistance/2)) + 5;
                            int curDistance = (*simu)->InspectionPointDistance - randDist;

                            DataItem * sendItemckp = new DataItem("RangingFiltered", (*i)->projectid, (*simu)->Inspector.c_str(), (*simu)->AnchorNodeId.c_str(), curDistance, 1, t_Current);
                            mInspectionJobQueue.add(sendItemckp);

                            //  設定 巡檢時間 0: in Tiime 1:過長 2:過短
                            if ( (*simu)->setInspectionTime == 0 )
                            {
                                if ( runtime > ((*simu)->InspectionPointMinTime - 5) )
                                {
                                    int randDist = (rand() % 50) + 5;
                                    int curDistance = (*simu)->InspectionPointDistance + randDist;

                                    DataItem * sendItemckp = new DataItem("RangingFiltered", (*i)->projectid, (*simu)->Inspector.c_str(), (*simu)->AnchorNodeId.c_str(), curDistance, 1, t_Current);
                                    mInspectionJobQueue.add(sendItemckp);
                                }
                                continue;
                            }
                        }
                        else
                        {
                            //  設定 巡檢時間 0: in Tiime 1:過長 2:過短
                            if ( (*simu)->setInspectionTime == 1 )
                            {
                                if ( runtime < ((*simu)->InspectionPointMaxTime + 30) )
                                {
                                    int randDist = (rand() % 50) + 5;
                                    int curDistance = (*simu)->InspectionPointDistance - randDist;

                                    DataItem * sendItemckp = new DataItem("RangingFiltered", (*i)->projectid, (*simu)->Inspector.c_str(), (*simu)->AnchorNodeId.c_str(), curDistance, 1, t_Current);
                                    mInspectionJobQueue.add(sendItemckp);
                                }
                                else
                                if ( runtime < ((*simu)->InspectionPointMaxTime + 50) )
                                {
                                    int randDist = (rand() % 50) + 5;
                                    int curDistance = (*simu)->InspectionPointDistance + randDist;

                                    DataItem * sendItemckp = new DataItem("RangingFiltered", (*i)->projectid, (*simu)->Inspector.c_str(), (*simu)->AnchorNodeId.c_str(), curDistance, 1, t_Current);
                                    mInspectionJobQueue.add(sendItemckp);
                                }
                                else
                                {
                                    // (*simu)->bStart = false; // move to stop_simulation_event()
                                }
                            }
                            else
                            {
                                // (*simu)->bStart = false; // move to stop_simulation_event()
                            }
                        }
                    }
                    //int dist = 0;
// #ifdef INSPECTION_PRINT_LOG
                    // vilslog::printf("InspectionSimulationEvent() GroupID[%d] GroupStartTime[%d] GroupEndTime[%d]\n",
                    //     (*simu)->GroupID, (*simu)->GroupStartTime, (*simu)->GroupEndTime);
                    // vilslog::printf("InspectionSimulationEvent() GroupID[%d] Inspector[%s] AnchorNodeId[%s] runtime[%d] InspectionPointMinTime[%d] InspectionPointMaxTime[%d]\n",
                    //     (*simu)->GroupID, (*simu)->Inspector.c_str(), (*simu)->AnchorNodeId.c_str(), runtime, (*simu)->InspectionPointMinTime, (*simu)->InspectionPointMaxTime);
                    // vilslog::printf("InspectionSimulationEvent() GroupID[%d] Inspector[%s] AnchorNodeId[%s] dist[%d]\n",
                    //     (*simu)->GroupID, (*simu)->Inspector.c_str(), (*simu)->AnchorNodeId.c_str(), dist);
// #endif
                }
                else
                {
                    //(*simu)->bStart = false; // move to stop_simulation_event()
                }
            }
            else
            {
                // remove from list
                // InspectorSimulationData * old_simu = (*simu);
                // simu = (*i)->mSimulationList.erase(simu);
                // delete old_simu;
                //
                // vilslog::printf("InspectionGroupService::InspectionSimulationEvent() remove InspectorSimulationData GroupID[%d] GroupStartTime[%d] GroupEndTime[%d]\n",
                //     (*simu)->GroupID, (*simu)->GroupStartTime, (*simu)->GroupEndTime);

            }
        }
    }
    return 0;
}

int InspectionGroupService::create_simulation_event(int projectid, InspectionGroupStruct * inspectionGroup, InspectionPointStruct * inspectionPoint)
{
    // vilslog::printf("create_simulation_event() projectid[%d] GroupID[%d]\n", projectid, inspectionGroup->GroupID);

    ProjectInspectionInfoStruct * inspectionInfo = getProjectInspectionInfo(projectid);

    bool bFound = false;
    InspectorSimulationData * curSimu = NULL;
    for (std::list<InspectorSimulationData*>::iterator simu = inspectionInfo->mSimulationList.begin(); simu != inspectionInfo->mSimulationList.end(); simu++)
    {
        if ( (*simu)->GroupID == inspectionGroup->GroupID )
        {
            curSimu = (*simu);
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        curSimu = new InspectorSimulationData();
        inspectionInfo->mSimulationList.push_back(curSimu);
        curSimu->bStart = false;
    }

    if ( !curSimu->bStart )
        curSimu->InspectionStartTime = time(0);
    if ( curSimu->AnchorNodeId.compare(inspectionPoint->AnchorNodeId) != 0  )
        curSimu->InspectionStartTime = time(0);

    curSimu->bStart = true;
    curSimu->GroupID = inspectionGroup->GroupID;
    curSimu->AnchorNodeId = inspectionPoint->AnchorNodeId;
    curSimu->GroupStartTime = StringToInspectionDatetime(inspectionGroup->StartTime);
    curSimu->GroupEndTime = StringToInspectionDatetime(inspectionGroup->EndTime);

    if ( (inspectionGroup->Inspectors != NULL) )
    {
        if ( (int)inspectionGroup->Inspectors->size() > 0 )
        {
            std::list<std::string>::iterator inspectorIt = inspectionGroup->Inspectors->begin();
            curSimu->Inspector = (*inspectorIt);
        }
    }

    //  設定 巡檢時間 0: in Tiime 1:過長 2:過短
    curSimu->setInspectionTime = (rand() % 3);

    //  設定 巡檢點轉換時間 0: in Tiime 1:過長 2:過短
    curSimu->setTravelTimeTime = (rand() % 3);

    curSimu->InspectionPointDistance = inspectionPoint->Distance;
    curSimu->InspectionPointMinTime = inspectionPoint->MinTime;
    curSimu->InspectionPointMaxTime = inspectionPoint->MaxTime;
    curSimu->InspectionPointTravelTime = inspectionPoint->TravelTime;

    // vilslog::printf("InspectionGroupService::create_simulation_event() projectid[%d] GroupID[%d] PointID[%d] AnchorNodeId[%s] setInspectionTime[%d] setTravelTimeTime[%d] InspectionStartTime[%s]\n",
    //     projectid, inspectionGroup->GroupID, inspectionPoint->PointID, curSimu->AnchorNodeId.c_str(), curSimu->setInspectionTime, curSimu->setTravelTimeTime, time_tToString(curSimu->InspectionStartTime).c_str());

    return 0;
}

int InspectionGroupService::update_simulation_event(int projectid, InspectionGroupStruct * inspectionGroup, InspectionPointStruct * inspectionPoint)
{
    // vilslog::printf("update_simulation_event() projectid[%d] GroupID[%d]\n", projectid, inspectionGroup->GroupID);

    ProjectInspectionInfoStruct * inspectionInfo = getProjectInspectionInfo(projectid);

    bool bFound = false;
    for (std::list<InspectorSimulationData*>::iterator simu = inspectionInfo->mSimulationList.begin(); simu != inspectionInfo->mSimulationList.end(); simu++)
    {
        if ( (*simu)->GroupID == inspectionGroup->GroupID )
        {
            if ( (*simu)->AnchorNodeId.compare(inspectionPoint->AnchorNodeId) == 0 )
            {
                // same InspectionPoint
                // vilslog::printf("update_simulation_event() projectid[%d] GroupID[%d] same InspectionPoint\n", projectid, inspectionGroup->GroupID);
            }
            else
            {
                (*simu)->bStart = true;
                (*simu)->AnchorNodeId = inspectionPoint->AnchorNodeId;
                (*simu)->InspectionStartTime = time(0);
                (*simu)->InspectionPointDistance = inspectionPoint->Distance;
                (*simu)->InspectionPointMinTime = inspectionPoint->MinTime;
                (*simu)->InspectionPointMaxTime = inspectionPoint->MaxTime;
                (*simu)->InspectionPointTravelTime = inspectionPoint->TravelTime;
            }
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        vilslog::printf("InspectionGroupService::update_simulation_event() projectid[%d] GroupID[%d] create_simulation_event\n",
            projectid, inspectionGroup->GroupID);

        create_simulation_event(projectid, inspectionGroup, inspectionPoint);
    }

    return 0;
}

int InspectionGroupService::stop_simulation_event(int projectid, InspectionGroupStruct * inspectionGroup)
{
    vilslog::printf("stop_simulation_event() projectid[%d] GroupID[%d]\n", projectid, inspectionGroup->GroupID);

    ProjectInspectionInfoStruct * inspectionInfo = getProjectInspectionInfo(projectid);

    for (std::list<InspectorSimulationData*>::iterator simu = inspectionInfo->mSimulationList.begin(); simu != inspectionInfo->mSimulationList.end(); simu++)
    {
        if ( (*simu)->GroupID == inspectionGroup->GroupID )
        {
            vilslog::printf("stop_simulation_event() projectid[%d] GroupID[%d] Found!\n", projectid, inspectionGroup->GroupID);
            (*simu)->bStart = false;
            break;
        }
    }
    return 0;
}

int InspectionGroupService::InspectionGroupEvent()
{
    for (std::vector<ProjectInspectionInfoStruct*>::iterator i = mProjectInspectionInfoVector.begin(); i != mProjectInspectionInfoVector.end(); i++)
    {
        for (std::list<InspectionGroupStruct*>::iterator inspectionGroup = (*i)->mInspectionGroupList->begin(); inspectionGroup != (*i)->mInspectionGroupList->end(); inspectionGroup++)
        {
#ifdef INSPECTION_PRINT_LOG
            vilslog::printf("\n=============================\n");
            vilslog::printf("InspectionGroupEvent() GroupID[%d] GroupName[%s] time[%s -> %s]\n",
                (*inspectionGroup)->GroupID, (*inspectionGroup)->GroupName.c_str(), (*inspectionGroup)->StartTime.c_str(), (*inspectionGroup)->EndTime.c_str());
#endif

            InspectionGroupEvent((*i)->projectid, (*inspectionGroup));

#ifdef INSPECTION_PRINT_LOG
            vilslog::printf("=============================\n");
#endif
        }
    }
    return 0;
}

int InspectionGroupService::InspectionGroupEvent(int projectid, InspectionGroupStruct * inspectionGroup)
{
    int status = 0;
    time_t t_Start = StringToInspectionDatetime(inspectionGroup->StartTime);
    time_t t_End = StringToInspectionDatetime(inspectionGroup->EndTime);
    time_t t_Current = time(0);

    // vilslog::printf("InspectionGroupEvent() GroupID[%d] GroupName[%s] t_Start[%lu] t_End[%lu] t_Current[%lu]\n",
    //     inspectionGroup->GroupID, inspectionGroup->GroupName.c_str(), t_Start, t_End, t_Current);

    ProjectInspectionInfoStruct * inspectionInfo = getProjectInspectionInfo(projectid);

    InspectionGroupData * groupData = NULL;
    for (unsigned int i=0; i< inspectionInfo->mInspectionGroupVector.size(); i++)
    {
        if ( inspectionInfo->mInspectionGroupVector[i]->GroupID == inspectionGroup->GroupID )
        {
            groupData = inspectionInfo->mInspectionGroupVector[i];
            break;
        }
    }

    if ( groupData == NULL )
    {
        return 0;
    }

    if (t_Current < t_Start)
    {
        // not yet!
        time_t t_diff = t_Start - t_Current;
        status = InspectionGroupEvent_outtime(projectid, inspectionGroup, groupData, t_diff);

        if (status == 1)
        {
            // vilslog::printf("InspectionGroupService::InspectionGroupEvent() GroupID[%d] GroupName[%s] clear_SendNotificationTimeData_list\n",
            //     inspectionGroup->GroupID, inspectionGroup->GroupName.c_str());

            clear_Group_SendNotificationTimeData_list(inspectionGroup->GroupID, &inspectionInfo->mNotificationTimeList);

            // for (std::list<SendNotificationTimeData*>::iterator ntime = inspectionInfo->mNotificationTimeList.begin(); ntime != inspectionInfo->mNotificationTimeList.end(); ntime++)
            // {
            //     delete (*ntime);
            // }
            // inspectionInfo->mNotificationTimeList.clear();
        }
    }
    else
    if (t_Current >= t_Start && t_Current <= t_End)
    {
        // in check time interval
        time_t t_diff = t_Current - t_Start;
        status = InspectionGroupEvent_intime(projectid, inspectionGroup, groupData, t_diff);
    }
    else
    if (t_Current > t_End)
    {
        // check excess time event
        status = InspectionGroupEvent_exceedtime(projectid, inspectionGroup, groupData);
    }

    return status;
}

int InspectionGroupService::InspectionGroupEvent_outtime(int projectid, InspectionGroupStruct * inspectionGroup, InspectionGroupData * inspectiongroupdata, time_t t_diff)
{
    int status = 1;
    std::string today = TodayDate();
    int diff_min = ((int)t_diff/60);

#ifdef INSPECTION_PRINT_LOG
    vilslog::printf("InspectionGroupEvent_outtime()  today[%s] diff_min[%d] AlertTime[%d]\n", today.c_str(), diff_min, inspectionGroup->AlertTime);
#endif
    //
    // check push notification
    //
    if ( diff_min <= inspectionGroup->AlertTime )
    {
        bool bSend = check_Group_AlertMessage(projectid, inspectionGroup, 1, 3600);
        //if (inspectiongroupdata->SendAlertMessage == 0)
        if (bSend)
        {
#ifdef INSPECTION_PRINT_LOG
            vilslog::printf("InspectionGroupEvent_outtime()  Send Notification!\n");
#endif
            std::ostringstream sendstream1;
            sendstream1<<std::to_string(inspectionGroup->AlertTime)<<"分鐘後將開始巡檢";

            std::ostringstream sendstream2;
            sendstream2<<"表定 "<<inspectionGroup->StartTime<<" ~ "<<inspectionGroup->EndTime;

            send_InspectionGroup_alert_notification(projectid, inspectionGroup, sendstream1.str(), sendstream2.str(), "");
        }
        return 0;
    }


    //
    // clear record
    //
    for (std::list<InspectionPointStruct*>::iterator inspt = inspectiongroupdata->InspectionPointList->begin(); inspt != inspectiongroupdata->InspectionPointList->end(); inspt++)
    {
        initialInspectionPointDailyRecord((*inspt)->DailyData);
        (*inspt)->DailyData.GroupID = (*inspt)->GroupID;
        (*inspt)->DailyData.PointID = (*inspt)->PointID;
        (*inspt)->DailyData.DateTime = today;
    }
    //inspectiongroupdata->SendAlertMessage = 0;
    inspectiongroupdata->Inspector = "";//巡檢員

    return status;
}

int InspectionGroupService::InspectionGroupEvent_intime(int projectid, InspectionGroupStruct * inspectionGroup, InspectionGroupData * inspectiongroupdata, time_t t_diff)
{
    int status = BEHAVIOR_STATUS_NONE;
    bool bUpdateToDatabase = false;

    int total_score = 0;
    int Checked_Count = 0;
    int InspectionPointCount = (int)inspectiongroupdata->InspectionPointList->size();
    time_t t_Start = StringToInspectionDatetime(inspectionGroup->StartTime);

    InspectionPointStruct * pre_inspt = NULL;
    for (std::list<InspectionPointStruct*>::iterator inspt = inspectiongroupdata->InspectionPointList->begin(); inspt != inspectiongroupdata->InspectionPointList->end(); inspt++)
    {
#ifdef INSPECTION_PRINT_LOG
        vilslog::printf("\n\tGroupID[%d] PointID[%d] PointIndex[%d]  (%d/%d)\n",
            (*inspt)->GroupID, (*inspt)->PointID, (*inspt)->PointIndex, Checked_Count + 1, (int)inspectiongroupdata->InspectionPointList->size());
#endif

        bUpdateToDatabase = false;

        if ( (*inspt)->DailyData.Score > 0 )
            total_score += (*inspt)->DailyData.Score;

        if ((*inspt)->DailyData.Checked == 1)
        {
#ifdef INSPECTION_PRINT_LOG
            vilslog::printf("\t --------- This InspectionPoint is checked! ---------\n\n");
#endif
            Checked_Count++;
            pre_inspt = (*inspt);
            continue;
        }

        if ( (Checked_Count == 0) && (inspectiongroupdata->Inspector.length() == 0))
        {
            int diff_min = ((int)t_diff/60);

            if ( diff_min >= 1 )
            {
                if (inspectionGroup->AutoFeed == 1)
                {
                    create_simulation_event(projectid, inspectionGroup, (*inspt));
                }
            }

            if ( diff_min >= 10 )
            {
                bool bSend = check_Group_AlertMessage(projectid, inspectionGroup, 2, 600);
                //if (inspectiongroupdata->SendAlertMessage != 2)
                if (bSend)
                {
                    // [現在時間]-[巡檢路線名稱]-[巡檢點名稱]-[巡檢員名稱]-巡檢延宕，請盡速巡檢！

                    (*inspt)->DailyData.Score -= 5; // 巡檢延宕 -5% (每發送推播一次扣一次) 

                    send_InspectionPoint_AllInspector_notification(projectid, inspectionGroup, (*inspt), "", "巡檢延宕，請盡速巡檢！");

                    //inspectiongroupdata->SendAlertMessage = 2;
                }
            }
        }
        else
        {
            if (inspectionGroup->AutoFeed == 1)
            {
                update_simulation_event(projectid, inspectionGroup, (*inspt));
            }
        }

        int status_Behavior = RegisterInspectionBehavior(projectid, t_Start, inspectionGroup->Inspectors, inspectiongroupdata->Inspector, (*inspt));
        if ( status_Behavior != BEHAVIOR_STATUS_NONE )
        {
            // 找到巡檢Tag
            //if ( (status_Behavior == BEHAVIOR_STATUS_ENTER) && (inspt == inspectiongroupdata->InspectionPointList->begin()) )
            if ( status_Behavior == BEHAVIOR_STATUS_ENTER)
            {
                inspectiongroupdata->Inspector = (*inspt)->DailyData.Inspector;

                // std::string nodename = mDeviceManager->getTagName(projectid, inspectiongroupdata->Inspector.c_str());
                // std::string macadress = mDeviceManager->getTagMacaddress(projectid, inspectiongroupdata->Inspector.c_str());

                (*inspt)->DailyData.Score += 30; // 巡檢開始 +30% 

                (*inspt)->DailyData.Score += 40; // 正在巡檢 +40% 

                //
                // Enter Notification
                //
                std::string sendMessage = "巡檢開始！ ";
                if ( (*inspt)->Message.length() > 0 )
                {
                    sendMessage.append((*inspt)->Message);
                }
                send_InspectionPoint_notification(projectid, inspectionGroup, (*inspt), sendMessage, "", "", "");
            }
            else
            if (status_Behavior == BEHAVIOR_STATUS_LEAVE)
            {
                /*
                std::string nodename = mDeviceManager->getTagName(projectid, inspectiongroupdata->Inspector.c_str());
                std::string macadress = mDeviceManager->getTagMacaddress(projectid, inspectiongroupdata->Inspector.c_str());

                //
                // Leave Notification
                //
                std::string message = nodename;
                message.append(" 離開巡檢點 ");
                message.append((*inspt)->DailyData.Inspector);

                NotificationItem * item = new NotificationItem(TAG_PUSHNOTIFY_FUNCTION_5, projectid, NODE_TYPE_TAG, inspectiongroupdata->Inspector, nodename, macadress, "SMIMSVILS巡檢點通知", message);
                mNotifiactionQueue.add(item);
                */
            }

            bUpdateToDatabase = true;
        }

#ifdef INSPECTION_PRINT_LOG
        vilslog::printf("\n\tGroupID[%d] DailyData.Inspector[%s]\n", (*inspt)->GroupID, (*inspt)->DailyData.Inspector.c_str());
#endif
        //
        // check behavior time at CheckPoint
        //
        int status_BehaviorTime_pre = (*inspt)->DailyData.Status;
        int status_BehaviorTime = CheckBehaviorTime((*inspt)->MaxTime, (*inspt)->MinTime, (int)(*inspt)->DailyData.EnterTime, (int)(*inspt)->DailyData.LeaveTime);
        if ( status_BehaviorTime != 0 )
        {

#ifdef INSPECTION_PRINT_LOG
            vilslog::printf("\tCheckBehaviorTime() GroupID[%d] PointID[%d] PointIndex[%d] status_BehaviorTime[%d] \n\n",
                (*inspt)->GroupID, (*inspt)->PointID, (*inspt)->PointIndex, status_BehaviorTime);
#endif

            // 0:None 1:match 2:Too Short 3:Too Long
            if (status_BehaviorTime == 2 && status_BehaviorTime_pre != 2)
            {
                (*inspt)->DailyData.Score -= 30; // 巡檢時長不足 -30% 

                create_InspectionPoint_action_event(projectid, (*inspt)->DailyData.Inspector, "shorttime", (*inspt)->AnchorNodeId, (*inspt));

                send_InspectionPoint_notification(projectid, inspectionGroup, (*inspt), "巡檢時長不足，請確實巡檢！", "", "", "");

                //(*inspt)->DailyData.SendAlertMessage = 3;
            }
            else
            if (status_BehaviorTime == 3 && status_BehaviorTime_pre != 3)
            {
                bool bSend = check_Point_AlertMessage(projectid, inspectionGroup, (*inspt), 4, 600);
                if (bSend)
                {
                    (*inspt)->DailyData.Score -= 5; // 巡檢逾時 -5% (每發送推播一次扣一次) 

                    create_InspectionPoint_action_event(projectid, (*inspt)->DailyData.Inspector, "longtime", (*inspt)->AnchorNodeId, (*inspt));

                    send_InspectionPoint_notification(projectid, inspectionGroup, (*inspt), "巡檢逾時，請加快動作！", "", "", "");

                    //(*inspt)->DailyData.SendAlertMessage = 4;
                }
            }
            (*inspt)->DailyData.Status = status_BehaviorTime;
        }

        //
        // check TravelTime
        //
        int status_check = CheckRegisterTime(pre_inspt, (*inspt));

#ifdef INSPECTION_PRINT_LOG
        vilslog::printf("\tGroupID[%d] PointID[%d] PointIndex[%d] DailyData.Exceed[%d] status_check[%d] \n\n",
            (*inspt)->GroupID, (*inspt)->PointID, (*inspt)->PointIndex, (*inspt)->DailyData.Exceed, status_check);
#endif

        if (status_check == 0)
        {
            //printf("\t This InspectionPoint not checked!\n\n");
        }
        else
        if (status_check == 1 )
        {
            // 1: This InspectionPoint is checked and in travel time!
        }
        else
        if (status_check == 2)
        {
            // 2: This InspectionPoint is checked and over travel time!
            bool bSend = check_Point_AlertMessage(projectid, inspectionGroup, (*inspt), 5, 600);
            if (bSend)
            {
                (*inspt)->DailyData.Score -= 5; // 巡檢點轉換逾時 -5% (每發送推播一次扣一次) 
                send_InspectionPoint_notification(projectid, inspectionGroup, pre_inspt, "巡檢點轉換逾時，請加快移動！", "", "", "");
            }

            //OVER TRAVEL TIME
            if ((*inspt)->DailyData.Exceed == 0)
            {
                create_InspectionTravel_action_event(projectid, inspectiongroupdata->Inspector, "exceed", pre_inspt, (*inspt));
                (*inspt)->DailyData.Exceed = 1;
            }
            bUpdateToDatabase = true;
        }
        else
        if (status_check == 3)
        {
            // Tag leave InspectionPoint
            if ((*inspt)->DailyData.Finished == 0)
            {
                (*inspt)->DailyData.Score += 30; // 巡檢結束 +30% 

                create_InspectionTravel_action_event(projectid, inspectiongroupdata->Inspector, "finish", pre_inspt, (*inspt));
                (*inspt)->DailyData.Finished = 1;

                if ( Checked_Count < (InspectionPointCount - 1) )
                {
                    std::ostringstream sendstream1;
                    sendstream1<<"巡檢結束！";
                    std::ostringstream sendstream2;
                    sendstream2<<"本點評分 "<<(*inspt)->DailyData.Score;

                    send_InspectionPoint_notification(projectid, inspectionGroup, (*inspt), sendstream1.str(), sendstream2.str(), "", "");
                }
            }

            (*inspt)->DailyData.Checked = 1;
            bUpdateToDatabase = true;
        }
        else
        if (status_check == 4)
        {
            //OVER TRAVEL TIME
            bool bSend = check_Point_AlertMessage(projectid, inspectionGroup, (*inspt), 7, 600);
            if (bSend)
            {
                (*inspt)->DailyData.Score -= 5; // 巡檢點轉換逾時 -5% (每發送推播一次扣一次) 
                send_InspectionPoint_notification(projectid, inspectionGroup, pre_inspt, "巡檢點轉換逾時，請加快移動！", "", "", "");
            }
            if ((*inspt)->DailyData.Exceed == 0)
            {
                create_InspectionTravel_action_event(projectid, "", "exceed", pre_inspt, (*inspt));
                (*inspt)->DailyData.Exceed = 1;
            }
        }
        else
        if (status_check == 5)
        {
            // Tag Enter InspectionPoint
        }
        else
        {
            // status_check = 0
            // No Tag enter InspectionPoint
        }

        if (bUpdateToDatabase)
        {
            if ( (*inspt)->DailyData.Score < 0 )
                (*inspt)->DailyData.Score = 0;

            WorkItem * witem = new WorkItem("InspectionPointDailyRecord", projectid, (*inspt)->DailyData);
            mMySQLJobQueue.add(witem);
        }

        if ( ((*inspt)->DailyData.Finished == 0) && ((*inspt)->DailyData.Exceed == 0))
        {
            break;
        }

        // if (status_check == 0)
        // {
        //     // status_check = 0
        //     // No Tag enter InspectionPoint
        //     // We doed not need to check next InspectionPoint.
        //     //printf("\t status_check == 0, We doed not need to check next InspectionPoint. \n\n");
        //     break;
        // }
    }

    if ( (InspectionPointCount > 0) && (Checked_Count > 0))
    {
        inspectiongroupdata->DailyData.Score = total_score / Checked_Count;
    }

    if ( (InspectionPointCount > 0) && (Checked_Count >= InspectionPointCount))
    {
        if (inspectiongroupdata->DailyData.Finished == 0)
        {
            //inspectiongroupdata->DailyData.Score = total_score / Checked_Count;

            send_Inspection_Finish_notification(projectid, inspectionGroup, inspectiongroupdata);

            create_InspectionGroup_action_event(projectid, "finish", inspectionGroup, inspectiongroupdata);
            inspectiongroupdata->DailyData.Finished = 1;
            inspectiongroupdata->DailyData.Checked = 1;

            WorkItem * witem = new WorkItem("InspectionGroupDailyRecord", projectid, inspectiongroupdata->DailyData);
            mMySQLJobQueue.add(witem);

            if (inspectionGroup->AutoFeed == 1)
            {
                stop_simulation_event(projectid, inspectionGroup);
            }
        }
    }

    return status;
}

int InspectionGroupService::InspectionGroupEvent_exceedtime(int projectid, InspectionGroupStruct * inspectionGroup, InspectionGroupData * inspectiongroupdata)
{
    int status = 0;

    unsigned int check_count = 0;
    for (std::list<InspectionPointStruct*>::iterator inspt = inspectiongroupdata->InspectionPointList->begin(); inspt != inspectiongroupdata->InspectionPointList->end(); inspt++)
    {
        if ((*inspt)->DailyData.Checked == 1)
        {
            check_count++;
        }
    }

    if (check_count != inspectiongroupdata->InspectionPointList->size() )
    {
        if (inspectiongroupdata->DailyData.Exceed == 0)
        {
            create_InspectionGroup_action_event(projectid, "exceed", inspectionGroup, inspectiongroupdata);
            inspectiongroupdata->DailyData.Exceed = 1;
            inspectiongroupdata->DailyData.Checked = 1;

            WorkItem * witem = new WorkItem("InspectionGroupDailyRecord", projectid, inspectiongroupdata->DailyData);
            mMySQLJobQueue.add(witem);

            bool bSend = check_Group_AlertMessage(projectid, inspectionGroup, 3, 600);
            //if (inspectiongroupdata->SendAlertMessage == 0)
            if (bSend)
            {
#ifdef INSPECTION_PRINT_LOG
                vilslog::printf("InspectionGroupEvent_exceedtime() Send Notification!\n");
#endif

                send_InspectionGroup_exceed_notification(projectid, inspectionGroup, "巡檢路線已逾時");

                //inspectiongroupdata->SendAlertMessage = 3;
            }
        }
    }

    //
    // remove simulation data
    //
    ProjectInspectionInfoStruct * inspectionInfo = getProjectInspectionInfo(projectid);

    for (std::list<InspectorSimulationData*>::iterator simu = inspectionInfo->mSimulationList.begin(); simu != inspectionInfo->mSimulationList.end(); simu++)
    {
        if ( (*simu)->GroupID == inspectionGroup->GroupID )
        {
            vilslog::printf("InspectionGroupEvent_exceedtime() remove simulation data projectid[%d] GroupID[%d]\n",
                projectid, inspectionGroup->GroupID);

            // remove from list
            InspectorSimulationData * old_simu = (*simu);
            simu = inspectionInfo->mSimulationList.erase(simu);
            delete old_simu;
        }
    }
    return status;
}

int InspectionGroupService::RegisterInspectionBehavior(int projectid, time_t t_InspectionGroupStartTime, std::list<std::string> * Inspectors, std::string Inspector, InspectionPointStruct * inspectionPoint)
{
    int status = RegisterInspectionTime(projectid, t_InspectionGroupStartTime, Inspectors, Inspector, inspectionPoint);

    if ( status != BEHAVIOR_STATUS_NONE )
    {
        if ( status == BEHAVIOR_STATUS_START_RECORD )
        {
            bool bFound = addTo_BehaviorRecord(projectid, inspectionPoint->DailyData.RecordNodeId, inspectionPoint->AnchorNodeId, inspectionPoint);
            if (!bFound)
            {
#ifdef INSPECTION_PRINT_LOG
                vilslog::printf("\t **** BEHAVIOR_STATUS_START_RECORD **** GroupID[%d] PointIndex[%d] RecordNodeId[%s] inspectionPoint->AnchorNodeId[%s]\n",
                    inspectionPoint->GroupID, inspectionPoint->PointIndex, inspectionPoint->DailyData.RecordNodeId.c_str(), inspectionPoint->AnchorNodeId.c_str());
#endif
            }
        }
        else
        if ( status == BEHAVIOR_STATUS_ENTER )
        {
#ifdef INSPECTION_PRINT_LOG
            vilslog::printf("\t **** BEHAVIOR_STATUS_ENTER **** GroupID[%d] PointIndex[%d] Inspector[%s] inspectionPoint->AnchorNodeId[%s]\n",
                inspectionPoint->GroupID, inspectionPoint->PointIndex, Inspector.c_str(), inspectionPoint->AnchorNodeId.c_str());
#endif
            create_InspectionPoint_action_event(projectid, inspectionPoint->DailyData.Inspector, "enter", inspectionPoint->AnchorNodeId, inspectionPoint);

            bool bFound = addTo_BehaviorRecord(projectid, inspectionPoint->DailyData.RecordNodeId, inspectionPoint->AnchorNodeId, inspectionPoint);
            if (!bFound)
            {
#ifdef INSPECTION_PRINT_LOG
                vilslog::printf("\t **** BEHAVIOR_STATUS_START_RECORD **** GroupID[%d] PointIndex[%d] RecordNodeId[%s] inspectionPoint->AnchorNodeId[%s]\n",
                    inspectionPoint->GroupID, inspectionPoint->PointIndex, inspectionPoint->DailyData.RecordNodeId.c_str(), inspectionPoint->AnchorNodeId.c_str());
#endif
            }
            else
            {
#ifdef INSPECTION_PRINT_LOG
                vilslog::printf("\t **** BEHAVIOR_STATUS_ENTER But already addTo_BehaviorRecord()\n");
#endif
            }
        }
        else
        if ( status == BEHAVIOR_STATUS_LEAVE )
        {
#ifdef INSPECTION_PRINT_LOG
            vilslog::printf("\t **** BEHAVIOR_STATUS_LEAVE **** GroupID[%d] PointIndex[%d] Inspector[%s] inspectionPoint->AnchorNodeId[%s]\n",
                inspectionPoint->GroupID, inspectionPoint->PointIndex, Inspector.c_str(), inspectionPoint->AnchorNodeId.c_str());
#endif

            create_InspectionPoint_action_event(projectid, inspectionPoint->DailyData.Inspector, "leave", inspectionPoint->AnchorNodeId, inspectionPoint);

            markLeave_BehaviorRecord(projectid, Inspector, inspectionPoint->AnchorNodeId, inspectionPoint);
        }
        else
        if ( status == BEHAVIOR_STATUS_STOP_RECORD )
        {
            bool bFound = removeFrom_BehaviorRecord(projectid, inspectionPoint->DailyData.Inspector, inspectionPoint->AnchorNodeId, inspectionPoint);
            if (bFound)
            {
#ifdef INSPECTION_PRINT_LOG
                vilslog::printf("\t **** BEHAVIOR_STATUS_STOP_RECORD **** GroupID[%d] PointIndex[%d] Inspector[%s] inspectionPoint->AnchorNodeId[%s]\n",
                    inspectionPoint->GroupID, inspectionPoint->PointIndex, Inspector.c_str(), inspectionPoint->AnchorNodeId.c_str());
#endif
            }
        }
    }

    return status;
}

int InspectionGroupService::RegisterInspectionTime(int projectid, time_t t_InspectionGroupStartTime, std::list<std::string> * Inspectors, std::string Inspector, InspectionPointStruct * inspectionPoint)
{
    int status = BEHAVIOR_STATUS_NONE;
    time_t start_check_time = inspectionPoint->DailyData.EnterTime;
    std::string point_issuetime = inspectionPoint->issuetime;
    time_t t_point_issuetime = StringDateTimeTotime_t(point_issuetime);
    int Distance = inspectionPoint->Distance;

#ifdef INSPECTION_PRINT_LOG
    vilslog::printf("\tRegisterInspectionTime()\n\tInspector[%s] DailyData.Inspector[%s] AnchorNodeId[%s] Distance[%d]\n",
        Inspector.c_str(), inspectionPoint->DailyData.Inspector.c_str(), inspectionPoint->AnchorNodeId.c_str(), inspectionPoint->Distance);

    vilslog::printf("\tEnterTime[%s] point_issuetime[%s]\n", time_tToString(start_check_time).c_str(), point_issuetime.c_str());
#endif

    ProjectInspectionInfoStruct * inspectionInfo = getProjectInspectionInfo(projectid);

    for (unsigned int index = 0; index < inspectionInfo->mAnchorRangingMapVector.size(); index++)
    {
        // check AnchorId
        if ( inspectionInfo->mAnchorRangingMapVector[index]->getAnchorId().compare(inspectionPoint->AnchorNodeId) == 0 )
        {
            int TagCount = inspectionInfo->mAnchorRangingMapVector[index]->getTagCount();

#ifdef INSPECTION_PRINT_LOG_RegisterInspectionTime
            // vilslog::printf("\tRegisterInspectionTime() index[%d] TagCount[%d] t_InspectionGroupStartTime[%s]\n", index, TagCount, time_tToString(t_InspectionGroupStartTime).c_str());
#endif

            for(int i=0;i<TagCount;i++)
            {
                InspectionRangingDataStruct rangingdata;
                rangingdata.ranging = 9999999;
                rangingdata.DataTime = time(0);
                inspectionInfo->mAnchorRangingMapVector[index]->getLastRangingData(i, rangingdata);

                //check if same tag
                std::string TagId = inspectionInfo->mAnchorRangingMapVector[index]->getTagId(i);

                int t_group_diff = (int)(t_InspectionGroupStartTime - rangingdata.DataTime);
                if (t_group_diff >= 0)
                {
                    // rangingdata is too old
// #ifdef INSPECTION_PRINT_LOG_RegisterInspectionTime
//                     vilslog::printf("\t ************ t_group_diff[%d] rangingdata is too old ************\n", TagId.c_str(), t_group_diff);
//                     vilslog::printf("\t ************ TagId[%s] rangingdata.DataTime[%s] t_InspectionGroupStartTime[%s] ************\n",
//                         TagId.c_str(), time_tToString(rangingdata.DataTime).c_str(), time_tToString(t_InspectionGroupStartTime).c_str());
// #endif
                    continue;
                }

                //
                // check if rangingdata is too old, before the start_check_time.
                //
                int t_old_diff = (int)(start_check_time - rangingdata.DataTime);
                if (t_old_diff >= 0)
                {
                    // rangingdata is too old
#ifdef INSPECTION_PRINT_LOG_RegisterInspectionTime
                    vilslog::printf("\t ************ TagId[%s] t_old_diff[%d] rangingdata is too old ************\n", TagId.c_str(), t_old_diff);
                    vilslog::printf("\t ************ TagId[%s] rangingdata.DataTime[%s] CheckPoint start_check_time[%s] ************\n",
                        TagId.c_str(), time_tToString(rangingdata.DataTime).c_str(), time_tToString(start_check_time).c_str());
#endif
                    continue;
                }

                int t_ptah_diff = (int)(t_point_issuetime - rangingdata.DataTime);
                if (t_ptah_diff >= 0)
                {
                    // rangingdata is too old
#ifdef INSPECTION_PRINT_LOG_RegisterInspectionTime
                    vilslog::printf("\t ************ TagId[%s] t_ptah_diff[%d] rangingdata is too old ************\n", TagId.c_str(), t_ptah_diff);
                    vilslog::printf("\t ************ TagId[%s] rangingdata.DataTime[%s] point_issuetime[%s] ************\n",
                        TagId.c_str(), time_tToString(rangingdata.DataTime).c_str(), point_issuetime.c_str());
#endif
                    continue;
                }

                //
                // Found the current Tag
                //
                if ( (TagId.compare(Inspector) == 0) && (TagId.compare(inspectionPoint->DailyData.Inspector) == 0) )
                {
                    // check if distance is over range
                    int t_cur = (int)time(0);
                    int t_diff = (int)(t_cur - (int)rangingdata.DataTime);

#ifdef INSPECTION_PRINT_LOG_RegisterInspectionTime
                    vilslog::printf ("\tRegisterInspectionTime() Inspector[%s] TagId[%s] rangingdata.ranging[%d] DataTime[%s] start_check_time[%s] t_old_diff[%d]\n",
                        Inspector.c_str(), TagId.c_str(), rangingdata.ranging, time_tToString(rangingdata.DataTime).c_str(), time_tToString(start_check_time).c_str(), t_old_diff);
#endif
                    //
                    // check if rangingdata is too old, before the TagData.TagEnterTime.
                    //
                    int t_old_diff = (int)(inspectionPoint->DailyData.EnterTime - rangingdata.DataTime);
                    if (t_old_diff >= 0)
                    {
                        // rangingdata is too old

#ifdef INSPECTION_PRINT_LOG_RegisterInspectionTime
                         vilslog::printf ("\t ************ Inspector[%s] TagId[%s] DailyData.Inspector[%s] t_old_diff[%d] rangingdata is too old ************\n",
                             Inspector.c_str(), TagId.c_str(), inspectionPoint->DailyData.Inspector.c_str(), t_old_diff);
#endif
                    }
                    else
                    if ( (rangingdata.ranging <= Distance) && (t_diff < 10))
                    {
                        // timeout in 10 seconds
                        // still in range

#ifdef INSPECTION_PRINT_LOG_RegisterInspectionTime
                        vilslog::printf ("\t ************ Inspector[%s] TagId[%s] ranging[%d] still in range ************\n",
                            Inspector.c_str(), TagId.c_str(), rangingdata.ranging);
#endif
                        status = BEHAVIOR_STATUS_NONE;
                    }
                    else
                    if ( rangingdata.ranging > (Distance + INSPECTION_HEHAVIOR_RECORD_THRESHOLD) || (t_diff > 20))
                    {

#ifdef INSPECTION_PRINT_LOG_RegisterInspectionTime
                        vilslog::printf ("\tRegisterInspectionTime() 1 rangingdata.ranging[%d] t_diff[%d]\n",
                                rangingdata.ranging, t_diff);
#endif
                        status = BEHAVIOR_STATUS_STOP_RECORD;
                    }
                    else
                    {

#ifdef INSPECTION_PRINT_LOG_RegisterInspectionTime
                        vilslog::printf ("\tRegisterInspectionTime() 2 rangingdata.ranging[%d] t_diff[%d]\n",
                                rangingdata.ranging, t_diff);
#endif
                        // leave
                        inspectionPoint->DailyData.Inspector = TagId;
                        inspectionPoint->DailyData.RecordNodeId = TagId;
                        if (inspectionPoint->DailyData.LeaveTime == 0)
                        {
                            inspectionPoint->DailyData.LeaveTime = time(0);

                            status = BEHAVIOR_STATUS_LEAVE;
#ifdef INSPECTION_PRINT_LOG_RegisterInspectionTime
                            vilslog::printf ("\t ************ TagId[%s] RecordNodeId[%s] enter DataTime[%s] -> leave DataTime[%s] ************\n",
                               TagId.c_str(), inspectionPoint->DailyData.RecordNodeId.c_str(),
                               time_tToString(inspectionPoint->DailyData.EnterTime).c_str(),
                               time_tToString(inspectionPoint->DailyData.LeaveTime).c_str());
#endif
                        }
                        else
                        {
                            status = BEHAVIOR_STATUS_NONE;
                        }
                    }
                }
                else
                if ((Inspector.length() == 0) || (TagId.compare(Inspector) == 0) )
                {
                    // 尚未有Tag進入此巡檢路線 或是
                    // 找到此巡檢路線的Tag, 如果之前有其他巡檢員進入此Checkpoint 將會被覆蓋

                    //
                    // 檢查是否是此巡檢點 指定的 巡檢員
                    //
                    bool bFound = false;
                    for (std::list<std::string>::iterator inspectorIt = Inspectors->begin(); inspectorIt != Inspectors->end(); inspectorIt++)
                    {
                        if ( TagId.compare((*inspectorIt)) == 0 )
                        {
                            bFound = true;
                            break;
                        }
                    }

                    // check if distance is in range
                    if ( bFound && (rangingdata.ranging <= (Distance + INSPECTION_HEHAVIOR_RECORD_THRESHOLD)) )
                    {
                        if ( rangingdata.ranging <= Distance )
                        {
                            // enter 巡檢員進入此Checkpoint
                            inspectionPoint->DailyData.Inspector = TagId;
                            inspectionPoint->DailyData.RecordNodeId = TagId;
                            inspectionPoint->DailyData.EnterTime = time(0);
                            inspectionPoint->DailyData.LeaveTime = 0;
                            status = BEHAVIOR_STATUS_ENTER;
#ifdef INSPECTION_PRINT_LOG_RegisterInspectionTime
                             vilslog::printf ("\t ************ TagId[%s] enter DataTime[%s] ************ Inspector[%s]\n",
                                              TagId.c_str(), time_tToString(inspectionPoint->DailyData.EnterTime).c_str(), Inspector.c_str(), Inspector.c_str());
#endif
                        }
                        else
                        {
                            // 有巡檢員接近此Checkpoint
                            inspectionPoint->DailyData.RecordNodeId = TagId;
                            // 開始記錄測距, 可能會記錄到非此巡檢點巡檢員
                            status = BEHAVIOR_STATUS_START_RECORD;
#ifdef INSPECTION_PRINT_LOG_RegisterInspectionTimea
                             vilslog::printf ("\t ************ BEHAVIOR_STATUS_START_RECORD TagId[%s] RecordNodeId[%s] Inspector[%s]\n",
                                              TagId.c_str(), inspectionPoint->DailyData.RecordNodeId.c_str(), inspectionPoint->DailyData.Inspector.c_str());
#endif
                        }
                    }
                    else
                    {
                        // not in range
                        status = BEHAVIOR_STATUS_NONE;
                    }
                }
                else
                {
#ifdef INSPECTION_PRINT_LOG_RegisterInspectionTime
                     vilslog::printf ("\t ************ TagId[%s] is not this Inspector tag ************ real Inspector[%s]\n",
                                      TagId.c_str(), Inspector.c_str());
#endif
                    // already have tag enter this checkpoint
                    status = BEHAVIOR_STATUS_NONE;
                }
            }
            break;
        }
    }

    return status;
}

int InspectionGroupService::CheckBehaviorTime(int MaxTime, int MinTime, int InspectorEnterTime, int InspectorLeaveTime)
{
    int InspectorStatus = 0;

    if ( InspectorEnterTime == 0 )
    {
        // not enter
        InspectorStatus = 0;
    }
    else
    if ( InspectorLeaveTime == 0 )
    {
        // not leave yet, but check MaxTime
        int t_cur = (int)time(0);
        int t_diff = (int)(t_cur - InspectorEnterTime);

        if ( t_diff > MaxTime )
        {
#ifdef INSPECTION_PRINT_LOG
            vilslog::printf ("\t ************ CheckBehaviorTime() TagLeaveTime == 0 t_diff[%d] > MaxTime[%d] ************ \n",
                t_diff, MaxTime);
#endif

            InspectorStatus = 3;// 0:None 1:match 2:Too Short 3:Too Long
        }
    }
    else
    {
        int t_diff = InspectorLeaveTime - InspectorEnterTime;

        if ( t_diff < MinTime )
        {
#ifdef INSPECTION_PRINT_LOG
            vilslog::printf ("\t ************ CheckBehaviorTime() t_diff[%d] < MinTime[%d] ************ \n", t_diff, MinTime);
#endif

            InspectorStatus = 2;// 0:None 1:match 2:Too Short 3:Too Long
        }
        else
        if ( t_diff > MaxTime )
        {
#ifdef INSPECTION_PRINT_LOG
            vilslog::printf ("\t ************ CheckBehaviorTime() t_diff[%d] > MaxTime[%d] ************ \n", t_diff, MaxTime);
#endif

            InspectorStatus = 3;// 0:None 1:match 2:Too Short 3:Too Long
        }
        else
        {
#ifdef INSPECTION_PRINT_LOG
            vilslog::printf ("\t ************ CheckBehaviorTime() t_diff[%d] Match ************ \n", t_diff);
#endif

            InspectorStatus = 1;// 0:None 1:match 2:Too Short 3:Too Long
        }
    }

    return InspectorStatus;
}

int InspectionGroupService::CheckRegisterTime(InspectionPointStruct * pre_inspt, InspectionPointStruct * cur_inspt)
{
    int status = 0;

    if (pre_inspt == NULL)
    {
#ifdef INSPECTION_PRINT_LOG
        vilslog::printf ("\t\t CheckRegisterTime() pre_inspt == NULL\n");
#endif
        if ( cur_inspt->DailyData.Inspector.length() > 0 )
        {
            //
            // check leave time and enter time
            //
            if ( cur_inspt->DailyData.EnterTime > 0 )
            {
#ifdef INSPECTION_PRINT_LOG
                vilslog::printf ("\t\t CheckRegisterTime() status = 1 *********** --- IN TRAVEL TIME --- ***********\n");
#endif
                status = 1;// This checkpoint is checked and intime!
            }

            //
            // Tag leave InspectionPoint
            //
            if (cur_inspt->DailyData.LeaveTime > 0)
            {
#ifdef INSPECTION_PRINT_LOG
                vilslog::printf ("\t\t CheckRegisterTime() status = 3 ***********  Tag leave InspectionPoint  ***********\n");
#endif
                status = 3; // Tag leave InspectionPoint
            }
        }
        return status;
    }
    else
    {
#ifdef INSPECTION_PRINT_LOG
        vilslog::printf("\t\t CheckRegisterTime() pre_inspt->DailyData.inspector[%s] cur_inspt->DailyData.inspector[%s]\n",
            pre_inspt->DailyData.Inspector.c_str(), cur_inspt->DailyData.Inspector.c_str());
#endif
        if ( (pre_inspt->DailyData.Inspector.length() > 0) &&
             (cur_inspt->DailyData.Inspector.length() > 0) )
        {
            if ( pre_inspt->DailyData.Inspector.compare(cur_inspt->DailyData.Inspector) == 0 )
            {

#ifdef INSPECTION_PRINT_LOG
                vilslog::printf("\t\t pre_inspt->DailyData.LeaveTime[%d] cur_inspt->DailyData.EnterTime[%d]\n", pre_inspt->DailyData.LeaveTime, cur_inspt->DailyData.EnterTime);
#endif

                //
                // check leave time and enter time
                //
                if ( (pre_inspt->DailyData.LeaveTime > 0) && (cur_inspt->DailyData.EnterTime > 0) )
                {
                    int time_diff = (int)(cur_inspt->DailyData.EnterTime - pre_inspt->DailyData.LeaveTime);

#ifdef INSPECTION_PRINT_LOG
                    vilslog::printf("\t\t time_diff[%d] TravelTime[%d]\n", time_diff, cur_inspt->TravelTime);
#endif
                    if ( time_diff > cur_inspt->TravelTime)
                    {
#ifdef INSPECTION_PRINT_LOG
                        vilslog::printf ("\t\t CheckRegisterTime() status = 2 *********** !!! OVER TRAVEL TIME !!! ***********\n");
                        vilslog::printf ("\t\t CheckRegisterTime() time_diff[%d] TravelTime[%d]\n", time_diff, cur_inspt->TravelTime);
#endif
                        status = 2;// This InspectionPoint is checked and over travel time!
                    }
                    else
                    {
#ifdef INSPECTION_PRINT_LOG
                        vilslog::printf ("\t\t CheckRegisterTime() status = 1 *********** --- IN TRAVEL TIME --- ***********\n");
#endif
                        status = 1;// This InspectionPoint is checked and intime!
                    }

                    //
                    // Tag leave InspectionPoint
                    //
                    if (cur_inspt->DailyData.LeaveTime > 0)
                    {
#ifdef INSPECTION_PRINT_LOG
                        vilslog::printf ("\t\t CheckRegisterTime() status = 3 ***********  Tag leave InspectionPoint  ***********\n");
#endif
                        status = 3; // Tag leave InspectionPoint
                    }
                }
            }
        }
        else
        if ( (pre_inspt->DailyData.Inspector.length() > 0) )
        {
            //
            // Tag does not enter current InspectionPoint
            //
            //
            // check LeaveTime
            //
            if (pre_inspt->DailyData.LeaveTime > 0 )
            {
                int t_cur = (int)time(0);
                int time_diff = (int)(t_cur - (int)pre_inspt->DailyData.LeaveTime);
#ifdef INSPECTION_PRINT_LOG
                vilslog::printf("\t\t time_diff[%d] TravelTime[%d]\n", time_diff, cur_inspt->TravelTime);
#endif
                if ( time_diff > cur_inspt->TravelTime)
                {
#ifdef INSPECTION_PRINT_LOG
                    vilslog::printf("\t\t CheckRegisterTime() status = 4 *********** !!! OVER TRAVEL TIME !!! ***********\n");
                    vilslog::printf("\t\t CheckRegisterTime() TravelTime[%d] time_diff[%d]\n", cur_inspt->TravelTime, time_diff);

                    vilslog::printf("\n\t\t\t *********** !!! OVER TRAVEL TIME !!! ***********\n\n");
#endif
                    status = 4;// This InspectionPoint is OVER TRAVEL TIME !
                }
                else
                {
#ifdef INSPECTION_PRINT_LOG
                    vilslog::printf("\t\t CheckRegisterTime() status = 5 *********** --- IN TRAVEL TIME --- ***********\n");
                    vilslog::printf("\n\t\t\t *********** --- IN TRAVEL TIME --- ***********\n\n");
#endif
                    status = 5;// Tag Enter InspectionPoint
                }
            }
        }
    }

    return status;
}

int InspectionGroupService::create_InspectionGroup_action_event(int projectid, std::string Action, InspectionGroupStruct * inspectionGroup, InspectionGroupData * inspectiongroupdata)
{
#ifdef INSPECTION_PRINT_LOG
    vilslog::printf("\n\t[%s] create_InspectionGroup_action_event() Action[%s]\n", currentDateTime().c_str(), Action.c_str());
#endif
    std::string eventJsonString = convert_inspectionGroup_action_event_to_json_str("inspectionGroup", projectid, Action, inspectionGroup, inspectiongroupdata);

    int maingroupid = 1;

    //================================
    FiredEventStruct * newFiredEvents = new FiredEventStruct();
    newFiredEvents->projectid = projectid;
    newFiredEvents->eventid = 999;
    newFiredEvents->eventjson = eventJsonString;
    newFiredEvents->timestart = currentDateTimeForSQL().c_str();
    newFiredEvents->timeend = currentDateTimeForSQL().c_str();

    WorkItem * witem = new WorkItem("event", projectid, newFiredEvents);
    witem->setMaingroupid(maingroupid);
    mMySQLJobQueue.add(witem);

    delete newFiredEvents;

    int alertType = 1; //1:人員 2:固定裝置
    int alertAction = 1;// 1:In 2:Out 3:alert 4:cancel
    SubscribeDataItem * item = new SubscribeDataItem("event", projectid, 999, inspectiongroupdata->Inspector.c_str(), alertType, alertAction);
    mWebSocketqueue.add(item);
    //================================

    return 0;
}

//
// action: "finish"
//         "exceed"
//
std::string InspectionGroupService::convert_inspectionGroup_action_event_to_json_str(std::string type, int projectid, std::string Action, InspectionGroupStruct * inspectionGroup, InspectionGroupData * inspectiongroupdata)
{
    std::string json_str = "{";

    char tmp[64];
    json_str += "\"type\":\"" + type + "\",";

    snprintf(tmp, 64, "%d", projectid);
    json_str += "\"projectid\":\"" + std::string(tmp) + "\",";

    snprintf(tmp, 64, "%d", inspectionGroup->GroupID);
    json_str += "\"GroupID\":\"" + std::string(tmp) + "\",";

    json_str += "\"GroupName\":\"" + inspectionGroup->GroupName + "\",";
    json_str += "\"action\":\"" + Action + "\",";
    json_str += "\"issuetime\":\"" + currentDateTimeForSQL() + "\"";
    json_str += "}";

    return json_str;
}

std::string InspectionGroupService::convert_inspectionPoint_action_event_to_json_str(std::string type, int projectid, std::string Inspector, std::string Action, std::string AnchorNodeId, InspectionPointStruct * inspectionPoint)
{
    std::string json_str = "{";

    char tmp[64];
    json_str += "\"type\":\"" + type + "\",";

    snprintf(tmp, 64, "%d", projectid);
    json_str += "\"projectid\":\"" + std::string(tmp) + "\",";

    snprintf(tmp, 64, "%d", inspectionPoint->GroupID);
    json_str += "\"GroupID\":\"" + std::string(tmp) + "\",";

    snprintf(tmp, 64, "%d", inspectionPoint->PointIndex);
    json_str += "\"PointIndex\":\"" + std::string(tmp) + "\",";

    json_str += "\"Inspector\":\"" + Inspector + "\",";
    json_str += "\"action\":\"" + Action + "\",";
    json_str += "\"NodeId\":\"" + AnchorNodeId + "\",";
    json_str += "\"issuetime\":\"" + currentDateTimeForSQL() + "\"";
    json_str += "}";

    return json_str;
}

std::string InspectionGroupService::convert_inspectionTravel_action_event_to_json_str(std::string type, int projectid, std::string Inspector, std::string Action, InspectionPointStruct * pre_inspectionPoint, InspectionPointStruct * inspectionPoint)
{
    std::string json_str = "{";

    char tmp[64];
    json_str += "\"type\":\"" + type + "\",";

    snprintf(tmp, 64, "%d", projectid);
    json_str += "\"projectid\":\"" + std::string(tmp) + "\",";

    snprintf(tmp, 64, "%d", inspectionPoint->GroupID);
    json_str += "\"GroupID\":\"" + std::string(tmp) + "\",";

    snprintf(tmp, 64, "%d", inspectionPoint->PointIndex);
    json_str += "\"PointIndex\":\"" + std::string(tmp) + "\",";

    json_str += "\"Inspector\":\"" + Inspector + "\",";
    json_str += "\"action\":\"" + Action + "\",";
    //json_str += "\"AnchorNodeId\":\"" + inspectionPoint->AnchorNodeId + "\",";
    if (pre_inspectionPoint != NULL)
        json_str += "\"StartNodeId\":\"" + pre_inspectionPoint->AnchorNodeId + "\",";
    json_str += "\"EndNodeId\":\"" + inspectionPoint->AnchorNodeId + "\",";
    json_str += "\"issuetime\":\"" + currentDateTimeForSQL() + "\"";
    json_str += "}";

    return json_str;
}

int InspectionGroupService::create_InspectionPoint_action_event(int projectid, std::string Inspector, std::string Action, std::string AnchorNodeId, InspectionPointStruct * inspectionPoint)
{
    std::string eventJsonString = convert_inspectionPoint_action_event_to_json_str("inspectionpoint", projectid, Inspector, Action, AnchorNodeId, inspectionPoint);

    int maingroupid = 1;

    //================================
    FiredEventStruct * newFiredEvents = new FiredEventStruct();
    newFiredEvents->projectid = projectid;
    newFiredEvents->eventid = 999;
    newFiredEvents->eventjson = eventJsonString;
    newFiredEvents->timestart = currentDateTimeForSQL().c_str();
    newFiredEvents->timeend = currentDateTimeForSQL().c_str();

    WorkItem * witem = new WorkItem("event", projectid, newFiredEvents);
    witem->setMaingroupid(maingroupid);
    mMySQLJobQueue.add(witem);

    delete newFiredEvents;

    int alertType = 1; //1:人員 2:固定裝置
    int alertAction = 1;// 1:In 2:Out 3:alert 4:cancel
    SubscribeDataItem * item = new SubscribeDataItem("event", projectid, 999, Inspector.c_str(), alertType, alertAction);
    mWebSocketqueue.add(item);
    //================================

    return 0;
}

int InspectionGroupService::create_InspectionTravel_action_event(int projectid, std::string Inspector, std::string Action, InspectionPointStruct * pre_inspectionPoint, InspectionPointStruct * inspectionPoint)
{
    std::string eventJsonString = convert_inspectionTravel_action_event_to_json_str("inspectionpointtravel", projectid, Inspector, Action, pre_inspectionPoint, inspectionPoint);

    int maingroupid = 1;

    //================================
    FiredEventStruct * newFiredEvents = new FiredEventStruct();
    newFiredEvents->projectid = projectid;
    newFiredEvents->eventid = 999;
    newFiredEvents->eventjson = eventJsonString;
    newFiredEvents->timestart = currentDateTimeForSQL().c_str();
    newFiredEvents->timeend = currentDateTimeForSQL().c_str();

    WorkItem * witem = new WorkItem("event", projectid, newFiredEvents);
    witem->setMaingroupid(maingroupid);
    mMySQLJobQueue.add(witem);

    delete newFiredEvents;

    int alertType = 1; //1:人員 2:固定裝置
    int alertAction = 1;// 1:In 2:Out 3:alert 4:cancel
    SubscribeDataItem * item = new SubscribeDataItem("event", projectid, 999, Inspector.c_str(), alertType, alertAction);
    mWebSocketqueue.add(item);
    //================================

    return 0;
}

bool InspectionGroupService::addTo_BehaviorRecord(int projectid, std::string Inspector, std::string AnchorNodeId, InspectionPointStruct * inspectionPoint)
{
    // check if already add
    bool bFound = false;

    ProjectInspectionInfoStruct * inspectionInfo = getProjectInspectionInfo(projectid);

    for (std::list<InspectorActivityData*>::iterator activityData = inspectionInfo->mInspectorActivityList.begin(); activityData != inspectionInfo->mInspectorActivityList.end(); activityData++)
    {
        if ( ((*activityData)->GroupID == inspectionPoint->GroupID) &&
             ((*activityData)->PointIndex == inspectionPoint->PointIndex) &&
             ((*activityData)->AnchorNodeId.compare(AnchorNodeId) == 0) &&
             ((*activityData)->Inspector.compare(Inspector) == 0) )
        {
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        InspectorActivityData * newData = new InspectorActivityData();
        newData->GroupID = inspectionPoint->GroupID;
        newData->PointID = inspectionPoint->PointID;
        newData->PointIndex = inspectionPoint->PointIndex;
        newData->AnchorNodeId = AnchorNodeId;
        newData->Inspector = Inspector;
        newData->LeaveTime = 0;
        newData->RecordTime = time(0);
        inspectionInfo->mInspectorActivityList.push_back(newData);
    }

    return bFound;
}

bool InspectionGroupService::removeFrom_BehaviorRecord(int projectid, std::string Inspector, std::string AnchorNodeId, InspectionPointStruct * inspectionPoint)
{
    bool bFound = false;

    ProjectInspectionInfoStruct * inspectionInfo = getProjectInspectionInfo(projectid);

    for (std::list<InspectorActivityData*>::iterator activityData = inspectionInfo->mInspectorActivityList.begin(); activityData != inspectionInfo->mInspectorActivityList.end(); activityData++)
    {
        if ( ((*activityData)->GroupID == inspectionPoint->GroupID) &&
             ((*activityData)->PointIndex == inspectionPoint->PointIndex) &&
             ((*activityData)->AnchorNodeId.compare(AnchorNodeId) == 0) &&
             ((*activityData)->Inspector.compare(Inspector) == 0) )
        {
            InspectorActivityData * old_activityData = (*activityData);
            inspectionInfo->mInspectorActivityList.erase(activityData);
            delete old_activityData;

            bFound = true;
            break;
        }
    }

    return bFound;
}

bool InspectionGroupService::markLeave_BehaviorRecord(int projectid, std::string Inspector, std::string AnchorNodeId, InspectionPointStruct * inspectionPoint)
{
    bool bFound = false;

    ProjectInspectionInfoStruct * inspectionInfo = getProjectInspectionInfo(projectid);

    for (std::list<InspectorActivityData*>::iterator activityData = inspectionInfo->mInspectorActivityList.begin(); activityData != inspectionInfo->mInspectorActivityList.end(); activityData++)
    {
        if ( ((*activityData)->GroupID == inspectionPoint->GroupID) &&
             ((*activityData)->PointIndex == inspectionPoint->PointIndex) &&
             ((*activityData)->AnchorNodeId.compare(AnchorNodeId) == 0) &&
             ((*activityData)->Inspector.compare(Inspector) == 0) )
        {
            //
            // for timeout
            //
            (*activityData)->LeaveTime = time(0);
            bFound = true;
            break;
        }
    }

    return bFound;
}

void InspectionGroupService::reloadInspectionList()
{
    std::list<int> * prjidList = mDbManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);

        bool bFound = false;
        for (std::vector<ProjectInspectionInfoStruct*>::iterator i = mProjectInspectionInfoVector.begin(); i != mProjectInspectionInfoVector.end(); i++)
        {
            if ( (*i)->projectid == projectid )
            {
                reloadInspectionList((*i));
                bFound = true;
                break;
            }
        }
        if (!bFound)
        {
            ProjectInspectionInfoStruct * newInfo = createProjectInspectionInfo(projectid);
            mProjectInspectionInfoVector.push_back(newInfo);
        }
    }

    // printf("InspectionGroupService::reloadInspectionList() mProjectInspectionInfoVector.size()[%d]\n", (int)mProjectInspectionInfoVector.size());

    prjidList->clear();
    delete prjidList;
}

bool InspectionGroupService::check_Group_AlertMessage(int projectid, InspectionGroupStruct * inspectionGroup, int MessageType, int timeout_sec)
{
    bool bSend = false;

    ProjectInspectionInfoStruct * inspectionInfo = getProjectInspectionInfo(projectid);

    // vilslog::printf("InspectionGroupService::check_Group_AlertMessage() projectid[%d] GroupID[%d] inspectionInfo->mNotificationTimeList.size()[%d]\n",
    //     projectid, inspectionGroup->GroupID, (int)inspectionInfo->mNotificationTimeList.size());

    bool bFound = false;
    for (std::list<SendNotificationTimeData*>::iterator ntime = inspectionInfo->mNotificationTimeList.begin(); ntime != inspectionInfo->mNotificationTimeList.end(); ntime++)
    {
        // vilslog::printf("InspectionGroupService::check_Group_AlertMessage() projectid[%d] GroupID[%d] PointID[%d] MessageType[%d]\n",
        //     projectid, inspectionGroup->GroupID, (*ntime)->PointID, (*ntime)->MessageType);

        if ( (inspectionGroup->GroupID == (*ntime)->GroupID) && (0 == (*ntime)->PointID) && (MessageType == (*ntime)->MessageType) )
        {
            int t_diff = (int)(time(0) - (*ntime)->SendTime);

            // vilslog::printf("InspectionGroupService::check_Group_AlertMessage() projectid[%d] GroupID[%d] t_diff[%d] (*ntime)->timeout[%d] \n",
            //     projectid, inspectionGroup->GroupID, t_diff, (*ntime)->timeout);

            if ( t_diff >= (*ntime)->timeout )
            {
                bSend = true;
                (*ntime)->SendTime = time(0);

                inspectionInfo->m_CacheDB->write_SendNotificationTime_record((*ntime));
            }
            (*ntime)->timeout = timeout_sec;

            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        vilslog::printf("InspectionGroupService::check_Group_AlertMessage() projectid[%d] GroupID[%d] MessageType[%d] timeout_sec[%d] !bFound\n",
            projectid, inspectionGroup->GroupID, MessageType, timeout_sec);

        SendNotificationTimeData * ntime = new SendNotificationTimeData();
        ntime->GroupID = inspectionGroup->GroupID;
        ntime->PointID = 0;
        ntime->SendTime = time(0);
        ntime->MessageType = MessageType;
        ntime->timeout = timeout_sec;
        inspectionInfo->mNotificationTimeList.push_back(ntime);
        inspectionInfo->m_CacheDB->write_SendNotificationTime_record(ntime);

        // vilslog::printf("InspectionGroupService::check_Group_AlertMessage() projectid[%d] GroupID[%d] create new inspectionInfo->mNotificationTimeList.size()[%d]\n",
        //     projectid, inspectionGroup->GroupID, (int)inspectionInfo->mNotificationTimeList.size());

        bSend = true;
    }

    return bSend;
}

bool InspectionGroupService::check_Point_AlertMessage(int projectid, InspectionGroupStruct * inspectionGroup, InspectionPointStruct* inspectionPoint, int MessageType, int timeout_sec)
{
    bool bSend = false;

    ProjectInspectionInfoStruct * inspectionInfo = getProjectInspectionInfo(projectid);

    bool bFound = false;
    for (std::list<SendNotificationTimeData*>::iterator ntime = inspectionInfo->mNotificationTimeList.begin(); ntime != inspectionInfo->mNotificationTimeList.end(); ntime++)
    {
        if ( (inspectionGroup->GroupID == (*ntime)->GroupID) && (inspectionPoint->PointID == (*ntime)->PointID) && (MessageType == (*ntime)->MessageType) )
        {
            int t_diff = (int)(time(0) - (*ntime)->SendTime);

            if ( t_diff >= (*ntime)->timeout )
            {
                bSend = true;
                (*ntime)->SendTime = time(0);
            }
            (*ntime)->timeout = timeout_sec;

            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        SendNotificationTimeData * ntime = new SendNotificationTimeData();
        ntime->GroupID = inspectionGroup->GroupID;
        ntime->PointID = inspectionPoint->PointID;
        ntime->SendTime = time(0);
        ntime->MessageType = MessageType;
        ntime->timeout = timeout_sec;
        inspectionInfo->mNotificationTimeList.push_back(ntime);
        bSend = true;
    }

    return bSend;
}

bool InspectionGroupService::send_All_AlertMessage(int projectid, const char * locatorID, std::string message)
{
    bool bSend = false;

    ProjectInspectionInfoStruct * inspectionInfo = getProjectInspectionInfo(projectid);

    for (unsigned int i=0; i< inspectionInfo->mInspectionGroupVector.size(); i++)
    {
        InspectionGroupData * groupData = inspectionInfo->mInspectionGroupVector[i];
        InspectionGroupStruct * inspectionGroup = NULL;
        for (std::list<InspectionGroupStruct*>::iterator insGroup = inspectionInfo->mInspectionGroupList->begin(); insGroup != inspectionInfo->mInspectionGroupList->end(); insGroup++)
        {
            if ( (*insGroup)->GroupID == groupData->GroupID )
            {
                inspectionGroup = (*insGroup);
                break;
            }
        }

        if( inspectionGroup == NULL )
            continue;

        for (std::list<InspectionPointStruct*>::iterator inspt = groupData->InspectionPointList->begin(); inspt != groupData->InspectionPointList->end(); inspt++)
        {
            if ( (*inspt)->AnchorNodeId.compare(locatorID) == 0 )
            {
                send_InspectionPoint_AllInspector_notification(projectid, inspectionGroup, (*inspt), locatorID, message);
            }
        }
    }

    return bSend;
}

void InspectionGroupService::reloadInspectionList(ProjectInspectionInfoStruct * inspectionInfo)
{
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(inspectionInfo->projectid);
    if (baseDb == NULL)
        return;

    if (inspectionInfo->mInspectionGroupList != NULL)
    {
        clear_InspectionGroup_list(inspectionInfo->mInspectionGroupList);
        delete inspectionInfo->mInspectionGroupList;
    }

    inspectionInfo->mInspectionGroupList = baseDb->read_InspectionGroup();

    for (unsigned int i=0; i< inspectionInfo->mInspectionGroupVector.size(); i++)
    {
        clear_InspectionPoint_list(inspectionInfo->mInspectionGroupVector[i]->InspectionPointList);
        delete inspectionInfo->mInspectionGroupVector[i]->InspectionPointList;
    }
    inspectionInfo->mInspectionGroupVector.clear();

    std::string today = TodayDate();

    for (std::list<InspectionGroupStruct*>::iterator inspectionGroup = inspectionInfo->mInspectionGroupList->begin(); inspectionGroup != inspectionInfo->mInspectionGroupList->end(); inspectionGroup++)
    {
        InspectionGroupData * groupdata = new InspectionGroupData();
        groupdata->GroupID = (*inspectionGroup)->GroupID;
        //groupdata->SendAlertMessage = 0;
        groupdata->Inspector = "";

        initialInspectionGroupDailyRecord(groupdata->DailyData);
        groupdata->DailyData.GroupID = groupdata->GroupID;

        baseDb->read_InspectionGroupDailyRecord((*inspectionGroup)->GroupID, today, groupdata->DailyData);

        groupdata->InspectionPointList = baseDb->read_InspectionPoint((*inspectionGroup)->GroupID);
        groupdata->Inspector = findInspector(groupdata->InspectionPointList);
        inspectionInfo->mInspectionGroupVector.push_back(groupdata);
    }
}

std::string InspectionGroupService::findInspector(std::list<InspectionPointStruct*> * inspectionPointList)
{
    std::string Inspector = "";
    std::list<InspectionPointStruct*>::iterator ispt = inspectionPointList->begin();

    if ((*ispt) != NULL)
        Inspector = (*ispt)->DailyData.Inspector;

    return Inspector;
}

void InspectionGroupService::clear_ProjectInspectionInfoStruct_vector(std::vector<ProjectInspectionInfoStruct*> * plist)
{
    for (std::vector<ProjectInspectionInfoStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        if ((*i)->mInspectionGroupList != NULL)
        {
            clear_InspectionGroup_list((*i)->mInspectionGroupList);
            delete (*i)->mInspectionGroupList;
        }

        for (std::list<InspectorActivityData*>::iterator j = (*i)->mInspectorActivityList.begin(); j != (*i)->mInspectorActivityList.end(); j++)
        {
            delete (*j);
        }
        (*i)->mInspectorActivityList.clear();

        clear_InspectionAnchorRanging_vector(&(*i)->mAnchorRangingMapVector);

        delete (*i);
    }
    plist->clear();
}

void InspectionGroupService::clear_SendNotificationTimeData_list(std::list<SendNotificationTimeData*> * plist)
{
    for (std::list<SendNotificationTimeData*>::iterator ntime = plist->begin(); ntime != plist->end(); ntime++)
    {
        delete (*ntime);
    }
    plist->clear();
}

void InspectionGroupService::clear_Group_SendNotificationTimeData_list(int GroupID, std::list<SendNotificationTimeData*> * plist)
{
    for (std::list<SendNotificationTimeData*>::iterator ntime = plist->begin(); ntime != plist->end(); ntime++)
    {
        if ( GroupID == (*ntime)->GroupID )
        {
            SendNotificationTimeData * old_data = (*ntime);
            ntime = plist->erase(ntime);
            delete old_data;
        }
    }
}

void InspectionGroupService::send_InspectionGroup_alert_notification(int projectid, InspectionGroupStruct * inspectionGroup, std::string message1, std::string message2, std::string message3)
{
    std::string sendMessage = "[";
    sendMessage.append(currentDateTimeForSQL());
    sendMessage.append("]-[");
    sendMessage.append(inspectionGroup->GroupName);
    sendMessage.append("]");
    if (message1.length() > 0)
    {
        sendMessage.append("-[");
        sendMessage.append(message1);
        sendMessage.append("]");
    }
    if (message2.length() > 0)
    {
        sendMessage.append("-[");
        sendMessage.append(message2);
        sendMessage.append("]");
    }
    if (message3.length() > 0)
    {
        sendMessage.append("-[");
        sendMessage.append(message3);
        sendMessage.append("]");
    }

    send_push_notification(projectid, inspectionGroup, sendMessage);
}

void InspectionGroupService::send_InspectionGroup_exceed_notification(int projectid, InspectionGroupStruct * inspectionGroup, std::string message)
{
    std::string sendMessage = "[";
    sendMessage.append(currentDateTimeForSQL());
    sendMessage.append("]-[");
    sendMessage.append(inspectionGroup->GroupName);
    sendMessage.append("]-[");
    sendMessage.append(message);
    sendMessage.append("]");

    send_push_notification(projectid, inspectionGroup, sendMessage);
}

void InspectionGroupService::send_InspectionPoint_AllInspector_notification(int projectid, InspectionGroupStruct * inspectionGroup, InspectionPointStruct * inspectionPoint, const char * locatorID, std::string message)
{
    std::string sendMessage = "[";
    sendMessage.append(currentDateTimeForSQL());
    sendMessage.append("]-[");
    sendMessage.append(inspectionGroup->GroupName);
    sendMessage.append("]-[");
    sendMessage.append(getAnchorNodeName(projectid, inspectionPoint->AnchorNodeId));
    sendMessage.append("]-[");

    for (std::list<std::string>::iterator inspectorIt = inspectionGroup->Inspectors->begin(); inspectorIt != inspectionGroup->Inspectors->end(); inspectorIt++)
    {
        std::string tagmessage = sendMessage;
        //tagmessage.append(mDeviceManager->getTagName(projectid, (*inspectorIt).c_str()));
        tagmessage.append(message);
        tagmessage.append("]");

        send_push_notification(projectid, locatorID, (*inspectorIt), tagmessage);
    }
}

void InspectionGroupService::send_InspectionPoint_notification(int projectid, InspectionGroupStruct * inspectionGroup, InspectionPointStruct * inspectionPoint, std::string message1, std::string message2, std::string message3, std::string message4)
{
    std::string nodename = mDeviceManager->getTagName(projectid, inspectionPoint->DailyData.Inspector.c_str());

    std::string sendMessage = "[";
    sendMessage.append(currentDateTimeForSQL());
    sendMessage.append("]-[");
    sendMessage.append(inspectionGroup->GroupName);
    sendMessage.append("]-[");
    sendMessage.append(getAnchorNodeName(projectid, inspectionPoint->AnchorNodeId));
    sendMessage.append("]-[");
    sendMessage.append(nodename);
    sendMessage.append("]");
    if (message1.length() > 0)
    {
        sendMessage.append("-[");
        sendMessage.append(message1);
        sendMessage.append("]");
    }
    if (message2.length() > 0)
    {
        sendMessage.append("-[");
        sendMessage.append(message2);
        sendMessage.append("]");
    }
    if (message3.length() > 0)
    {
        sendMessage.append("-[");
        sendMessage.append(message3);
        sendMessage.append("]");
    }
    if (message4.length() > 0)
    {
        sendMessage.append("-[");
        sendMessage.append(message4);
        sendMessage.append("]");
    }

    send_push_notification(projectid, inspectionPoint->AnchorNodeId, inspectionPoint->DailyData.Inspector, sendMessage);
}

// [巡檢結束！] - [本點評分-95] - [本次路線平均分數 95] - [各點分數為60/80/70/80]
void InspectionGroupService::send_Inspection_Finish_notification(int projectid, InspectionGroupStruct * inspectionGroup, InspectionGroupData * inspectiongroupdata)
{
    std::ostringstream sendstream1;
    sendstream1<<"巡檢結束！";

    std::ostringstream sendstream3;
    sendstream3<<"本次路線平均分數 "<<inspectiongroupdata->DailyData.Score;

    std::ostringstream sendstream4;
    sendstream4<<"各點分數為 ";

    InspectionPointStruct * preinspt = NULL;
    for (std::list<InspectionPointStruct*>::iterator inspt = inspectiongroupdata->InspectionPointList->begin(); inspt != inspectiongroupdata->InspectionPointList->end(); inspt++)
    {
        if ( inspt != inspectiongroupdata->InspectionPointList->begin())
            sendstream4<<"/";
        sendstream4<<(*inspt)->DailyData.Score;
        preinspt = (*inspt);
    }

    std::ostringstream sendstream2;
    if (preinspt != NULL)
    {
        sendstream2<<"本點評分 "<<preinspt->DailyData.Score;
        send_InspectionPoint_notification(projectid, inspectionGroup, preinspt, sendstream1.str(), sendstream2.str(), sendstream3.str(), sendstream4.str());
    }
}

void InspectionGroupService::send_push_notification(int projectid, InspectionGroupStruct * inspectionGroup, std::string message)
{
    for (std::list<std::string>::iterator inspectorIt = inspectionGroup->Inspectors->begin(); inspectorIt != inspectionGroup->Inspectors->end(); inspectorIt++)
    {
        send_push_notification(projectid, (*inspectorIt), (*inspectorIt), message);
    }
}

void InspectionGroupService::send_push_notification(int projectid, std::string locatorID, std::string NodeId, std::string message)
{
    NodeStruct * tagNode = mDeviceManager->get_Tag_node(projectid, NodeId.c_str());
    if (tagNode != NULL)
    {
        std::string source_nodeid = locatorID;
        std::string source_name = tagNode->nodename;
        std::string target_macaddress = tagNode->macaddress;

        NotificationItem * item = new NotificationItem(TAG_PUSHNOTIFY_FUNCTION_5, projectid, NODE_TYPE_TAG, source_nodeid, source_name, to_string(tagNode->userid), "1", target_macaddress, "SMIMSVILS巡檢點通知", message);
        mNotifiactionQueue.add(item);

        vilslog::printf("InspectionGroupService::send_push_notification() message[%s]\n", message.c_str());
    }
}

std::string InspectionGroupService::getAnchorNodeName(int projectid, std::string AnchorNodeId)
{
    std::string AnchorNodeName = mDeviceManager->getAnchorName(projectid, AnchorNodeId.c_str());
    if ( AnchorNodeName.length() == 0 )
        AnchorNodeName = mDeviceManager->getLocatorName(projectid, AnchorNodeId.c_str());
    return AnchorNodeName;
}
