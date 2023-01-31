/*
 *  MySQLServiceLoopThread.cpp
 *
 *  Created on: 2017/05/24
 *      Author: ikki
 */

#include <string.h>
#include "MySQLServiceLoopThread.h"
#include "../network/inter_thread_pubsub.hpp"
#include "../common.h"

using namespace std;

MySQLServiceLoopThread::MySQLServiceLoopThread(wqueue<DataItem*> & MainJobQueue, wqueue<WorkItem*> & queue, DBManager * dbManager)
    : mMainJobQueue(MainJobQueue), m_queue(queue), mDbManager(dbManager)
{
    for (int i=0;i<MAX_TAG_NUMBER;i++)
    {
        mTagData[i].projectid = 1;
        mTagData[i].idx = -1;
        memset(mTagData[i].TagId, 0, 32);
        memset(mTagData[i].voltage, 0, 32);
        memset(mTagData[i].temperature, 0, 32);
    }
}

MySQLServiceLoopThread::~MySQLServiceLoopThread()
{
}

bool MySQLServiceLoopThread::status()
{
    return mInitial;
}

std::string MySQLServiceLoopThread::statusString()
{
    return mStatus;
}

void MySQLServiceLoopThread::updateProject()
{
    mUpdateProject = true;
}

void * MySQLServiceLoopThread::run()
{
    while(1)
    {
        mInitial = false;
        mStatus = "MySQLServiceLoopThread() Initial failed!!";

        try
        {
            runloop();
            sleep(3);
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("MySQLServiceLoopThread::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "MySQLServiceLoopThread()::run() exception:";
            mStatus.append(exc.what());
        }
        mInitial = false;
        mStatus = "MySQLServiceLoopThread() Initial failed!!";
        vilslog::printf ("MySQLServiceLoopThread() Initial failed!\n");
    }
    return NULL;
}

int MySQLServiceLoopThread::getQueueSize()
{
    return m_queue_size;
}

void MySQLServiceLoopThread::runloop()
{
    char type[64];
    // Remove 1 item at a time and process it. Blocks if no items are
    long long start1 = getSystemTime();
    long long ProcessTime = 0;
    long long updateTime = 0;
    long long checkTime = 0;
    long long updatePrjIdTime = 0;
    int ProcessCount = 0;
    int updateCount = 0;

    vilslog::printf ("MySQLServiceLoopThread()::runloop()\n");

    // instantiate subscribers with message queues
    WorkItem * dft_rtn = NULL;
    int queue_size1 = 256;
    // ITPS::Subscriber<std::string> sub1("Topic1", "Msg1", queue_size1);
    mSQLJobSubscriber = new ITPS::Subscriber<WorkItem*>("SQLServer", "save", queue_size1);

    while(!mSQLJobSubscriber->subscribe())
    {
        vilslog::printf ("MySQLServiceLoopThread()::waitloop() mSQLJobSubscriber()\n");
        sleep(1);
    }

    // wait until the subscribers are initialized
    // while(!sub1.subscribe())
    // {
    //     vilslog::printf ("MySQLServiceLoopThread()::runloop() sub1.subscribe()\n");
    //     sleep(1);
    // }

    mPrjidList = mDbManager->getAllProjectConfig();

    mInitial = true;
    mStatus = "MySQLServiceLoopThread() Initial successfully!";
    vilslog::printf ("MySQLServiceLoopThread() Initial successfully!\n");

    // available to process.
    while (1)
    {
        // printf("thread MySQLServiceLoopThread() waiting for pop_msg...\n");
        // std::string rtn = sub1.pop_msg(1000, dft_rtn);
        // printf("thread MySQLServiceLoopThread()  pop_msg[%s]\n", rtn.c_str());

        //printf("thread MySQLServiceLoopThread, waiting for item...\n");
        //WorkItem * item = m_queue.remove();
        WorkItem * item = m_queue.removetimed(100);

        if (item == NULL)
        {
            item = mSQLJobSubscriber->pop_msg(100, dft_rtn);
        }

        long long end1 = getSystemTime();
        long long difft = end1 - start1;
        ProcessTime += difft;
        updateTime += difft;
        checkTime += difft;
        updatePrjIdTime += difft;

        m_queue_size = (int)m_queue.size();

        //if (updateTime >= 500 && updateCount > 0)
        if (updateTime >= 5000 || updateCount > 100)
        {
            //vilslog::printf("MySQLServiceLoopThread::waitloop() updateCount[%d] m_queue.size()[%d]\n",  updateCount, (int)m_queue.size());
            for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
            {
                int projectid = (*prj);
                MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
                if (baseDb != NULL)
                    baseDb->Transaction_commit();
            }

            updateTime = 0;
            updateCount = 0;
        }

        if (updatePrjIdTime > 3600000 || mUpdateProject)
        {
            mPrjidList->clear();
            delete mPrjidList;
            mPrjidList = mDbManager->getAllProjectConfig();

            updatePrjIdTime = 0;
            mUpdateProject = false;
        }

        if (checkTime >= 30000)
        {
            std::string time = currentTimeForSQL();

            if ( time.compare("01:00") == 0 )
            {
                clear_old_data();
            }

            checkTime = 0;
        }

        if ( item == NULL )
        {
            start1 = end1;
            continue;
        }

        updateCount++;
        ProcessCount++;
        if ((ProcessCount % 100) == 0)
        {
            // vilslog::printf("MySQLServiceLoopThread::waitloop() ProcessCount[%d] ProcessTime[%lld] m_queue.size()[%d]\n",
            //                 ProcessCount, ProcessTime, (int)m_queue.size());
            ProcessTime = 0;
        }

        mProcessingType = item->getType();
        sprintf(type, "%s", item->getType());

//        printf("MySQLServiceLoopThread::waitloop() type[%s] difft[%lld]\n", type, difft);

        if ( strcmp(type, "alive") == 0)
            send_alive(item);
        else
//        if ( strcmp(type, "updateS") == 0)
//            send_updateS(item);
//        else
//        if ( strcmp(type, "updateM") == 0)
//            send_updateM(item);
//        else
        if ( strcmp(type, "deletenode") == 0)
            send_DeleteNode(item);
        else
        if ( strcmp(type, "deleteaoanode") == 0)
            send_DeleteAoANode(item);
        else
        if ( strcmp(type, "updateInfo") == 0)
            updateInfo(item);
        else
        if ( strcmp(type, "updateFullInfo") == 0)
            updateFullInfo(item);
        else
        if ( strcmp(type, "updateAoANodeInfo") == 0)
            updateAoANodeInfo(item);
        else
        if ( strcmp(type, "alert") == 0)
            send_alert(item);
        else
        if ( strcmp(type, "fencealert") == 0)
            send_fencealert(item);
        else
        if ( strcmp(type, "alert0D") == 0)
            send_alert0D(item);
        else
        if ( strcmp(type, "alertcancel") == 0)
            send_alertcancel(item);
        else
        if ( strcmp(type, "event") == 0)
            send_event(item);
        else
        if ( strcmp(type, "deleteevent") == 0)
            delete_event(item);
        else
        if ( strcmp(type, "createevent") == 0)
            create_event(item);
        else
        // if ( strcmp(type, "CheckPointPathDailyRecord") == 0)
        //     updateCheckPointPathDailyRecord(item);
        // else
        // if ( strcmp(type, "CheckPointGroupDailyRecord") == 0)
        //     updateCheckPointGroupDailyRecord(item);
        // else
        // if ( strcmp(type, "CheckPointTagBehavior") == 0)
        //     updateCheckPointTagBehavior(item);
        // else
        if ( strcmp(type, "InspectionPointDailyRecord") == 0)
            updateInspectionPointDailyRecord(item);
        else
        if ( strcmp(type, "InspectionGroupDailyRecord") == 0)
            updateInspectionGroupDailyRecord(item);
        else
        if ( strcmp(type, "InspectorBehavior") == 0)
            updateInspectorBehavior(item);
        else
        if ( strcmp(type, "createsection") == 0)
            createsection(item);
        else
        if ( strcmp(type, "deletesection") == 0)
            deletesection(item);
        else
        if ( strcmp(type, "createfencealert") == 0)
            createfencealert(item);
        else
        if ( strcmp(type, "deletefencealert") == 0)
            deletefencealert(item);
        else
        if ( strcmp(type, "updatemaplayer") == 0)
            updatemaplayer(item);
        else
        if ( strcmp(type, "updatemaplayerarea") == 0)
            updatemaplayerarea(item);
        else
        if ( strcmp(type, "updatemaplayerareashape") == 0)
            updatemaplayerareashape(item);
        else
        if ( strcmp(type, "deletemaplayerarea") == 0)
            deletemaplayerarea(item);
        else
        // if ( strcmp(type, "createcheckpointgroup") == 0)
        //     createcheckpointgroup(item);
        // else
        // if ( strcmp(type, "updatecheckpointgroup") == 0)
        //     updatecheckpointgroup(item);
        // else
        // if ( strcmp(type, "deletecheckpointgroup") == 0)
        //     deletecheckpointgroup(item);
        // else
        // if ( strcmp(type, "createcheckpointpath") == 0)
        //     createcheckpointpath(item);
        // else
        // if ( strcmp(type, "updatecheckpointpathstartnode") == 0)
        //     updatecheckpointpathstartnode(item);
        // else
        // if ( strcmp(type, "updatecheckpointpathendnode") == 0)
        //     updatecheckpointpathendnode(item);
        // else
        // if ( strcmp(type, "updatecheckpointpathtraveltime") == 0)
        //     updatecheckpointpathtraveltime(item);
        // else
        // if ( strcmp(type, "deletecheckpointpath") == 0)
        //     deletecheckpointpath(item);
        // else
        // if ( strcmp(type, "createinspectiongroup") == 0)
        //     createinspectiongroup(item);
        // else
        // if ( strcmp(type, "updateinspectiongroup") == 0)
        //     updateinspectiongroup(item);
        // else
        if ( strcmp(type, "updateconfig") == 0)
            updateconfig(item);
        else
        if ( strcmp(type, "deleteconfig") == 0)
            deleteconfig(item);
        else
        if ( strcmp(type, "updatesystemconfigudpport") == 0)
            updatesystemconfigudpport(item);
        else
        if ( strcmp(type, "updateAlgorithmConfig") == 0)
            updateAlgorithmConfig(item);
        else
        if ( strcmp(type, "updateLoRAWANConfig") == 0)
            updateLoRAWANConfig(item);
        else
        if ( strcmp(type, "createcourse") == 0)
            createcourse(item);
        else
        if ( strcmp(type, "deletecourse") == 0)
            deletecourse(item);
        else
        if ( strcmp(type, "updateCourseRollCallRecord") == 0)
            updateCourseRollCallRecord(item);
        else
        if ( strcmp(type, "areainoutevent") == 0)
            updateAreaInOutEventRecord(item);
        else
        if ( strcmp(type, "taggpsinfo") == 0)
            updateTagGPSInfoRecord(item);
        else
        if ( strcmp(type, "sensorgpsinfo") == 0)
            updateTagGPSInfoRecord(item);
        else
        if ( strcmp(type, "loragateway") == 0)
            updateLoRaGatewayInfo(item);
        else
        if ( strcmp(type, "updateblacklist") == 0)
            updateBlacklist(item);
        else
        if ( strcmp(type, "updatewhitelist") == 0)
            updateWhitelist(item);
        else
        if ( strcmp(type, "updatejoinRangingRange") == 0)
            updatejoinRangingRange(item);
        else
        if ( strcmp(type, "updatejoinNotRangingRange") == 0)
            updatejoinNotRangingRange(item);
        else
        if ( strcmp(type, "UpdateResident") == 0)
            UpdateResident(item);
        else
        if ( strcmp(type, "DeleteResident") == 0)
            DeleteResident(item);
        else
        if ( strcmp(type, "UpdateEmployee") == 0)
            UpdateEmployee(item);
        else
        if ( strcmp(type, "DeleteEmployee") == 0)
            DeleteEmployee(item);
        else
        if ( strcmp(type, "updateNodeResident") == 0)
            updateNodeResident(item);
        else
        if ( strcmp(type, "updateNodeEmployee") == 0)
            updateNodeEmployee(item);
        else
        if ( strcmp(type, "updateLocatorButtontype") == 0)
            updateLocatorButtontype(item);
        else
        if ( strcmp(type, "updateLocatorSensorAlert") == 0)
            updateLocatorSensorAlert(item);
        else
        if ( strcmp(type, "EmployeeEventReport") == 0)
            EmployeeEventReport(item);
        else
        // if ( strcmp(type, "UpdatePushNotifyUser") == 0)
        //     UpdatePushNotifyUser(item);
        // else
        if ( strcmp(type, "PushNotifyMessage") == 0)
            PushNotifyMessage(item);
        else
        if ( strcmp(type, "PushNotifyMessageRecord") == 0)
            PushNotifyMessageRecord(item);
        else
        if ( strcmp(type, "callout") == 0)
            send_callout(item);
        else
        if ( strcmp(type, "doctorevent") == 0)
            send_doctorevent(item);
        else
        if ( strcmp(type, "PowerOff") == 0)
            send_PowerOff(item);
        else
        if ( strcmp(type, "PowerOn") == 0)
            send_PowerOn(item);
        else
        if ( strcmp(type, "OpenDoor") == 0)
            send_OpenDoor(item);
        else
        if ( strcmp(type, "CloseDoor") == 0)
            send_CloseDoor(item);
        else
        if ( strcmp(type, "EnterDoor") == 0)
            send_EnterDoor(item);
        else
        if ( strcmp(type, "ExitDoor") == 0)
            send_ExitDoor(item);
        else
        if ( strcmp(type, "bodyinfoweight") == 0)
            send_bodyinfoweight(item);
        else
        if ( strcmp(type, "bodyinfoheartbeat") == 0)
            send_bodyinfoheartbeat(item);
        else
        if ( strcmp(type, "bodyinfoall") == 0)
            send_bodyinfoall(item);
        else
        if ( strcmp(type, "SensorAlert") == 0)
            send_SensorAlert(item);
        else
        if ( strcmp(type, "updatePushNotifySetting") == 0)
            updatePushNotifySetting(item);
        else
        if ( strcmp(type, "GAESurveyResultData") == 0)
            updateGAESurveyResultData(item);
        else
        if ( strcmp(type, "GAEMessageStatusData") == 0)
            updateGAEMessageStatusData(item);
        else
        if ( strcmp(type, "TargetTopicStruct") == 0)
            updateTargetTopicRecord(item);
        else
        if ( strcmp(type, "RFIDCardStruct") == 0)
            updateRFIDCardRecord(item);
        else
        if ( strcmp(type, "RFIDCardRollCall") == 0)
            updateRFIDCardRollCallRecord(item);
        else
        if ( strcmp(type, "NFCCardStruct") == 0)
            updateNFCCardRecord(item);
        else
        // if ( strcmp(type, "NFCCardRollCall") == 0)
        //     updateNFCCardRollCallRecord(item);
        // else
        if ( strcmp(type, "CARDRollCallRecordStruct") == 0)
            updateCARDRollCallRecord(item);
        else
        if ( strcmp(type, "CARDRollCallEventStruct") == 0)
            updateCARDRollCallEvent(item);
        else
        if ( strcmp(type, "CARDRollCallSubGroupEventStruct") == 0)
            updateCARDRollCallSubGroupEvent(item);
        else
        if ( strcmp(type, "CARDScanRecordStruct") == 0)
            updateCARDScanRecord(item);
        else
        if ( strcmp(type, "RFIDDevice") == 0)
            updateRFIDDevice(item);
        else
        if ( strcmp(type, "NFCDevice") == 0)
            updateNFCDevice(item);
        else
        if ( strcmp(type, "EDGEDevice") == 0)
            updateEDGEDevice(item);
        else
        if ( strcmp(type, "checkcarduser") == 0)
        {
            checkcarduser(item);

            int userid = item->getUserId();
            DataItem * senditem = new DataItem("reloadcarduser", 1);
            senditem->setUserId(userid);
            mMainJobQueue.add(senditem);
        }
        else
        if ( strcmp(type, "removecarduser") == 0)
            removecarduser(item);
        else
        if ( strcmp(type, "adduseruwbcard") == 0)
            adduseruwbcard(item);
        else
        if ( strcmp(type, "updateUserStruct") == 0)
            updateUserStruct(item);
        else
        if ( strcmp(type, "updateUserPoistion") == 0)
            updateUserPoistion(item);
        else
        if ( strcmp(type, "updateUserLocation") == 0)
            updateUserLocation(item);
        else
        if ( strcmp(type, "updateUserPassword") == 0)
            updateUserPassword(item);
        else
        if ( strcmp(type, "updateusersubgroup") == 0)
            updateusersubgroup(item);
        else
        if ( strcmp(type, "updateusermaingroup") == 0)
            updateusermaingroup(item);
        else
        if ( strcmp(type, "deletegateway") == 0)
            deleteGateway(item);
        else
        if ( strcmp(type, "clearUnknownNFCCardAlive") == 0)
            clearUnknownNFCCardAlive(item);
        else
        if ( strcmp(type, "clearUnknownRFIDCardAlive") == 0)
            clearUnknownRFIDCardAlive(item);
        else
        if ( strcmp(type, "updateLoginToken") == 0)
            updateLoginToken(item);
        else
        if ( strcmp(type, "updateAssistEventStruct") == 0)
            updateAssistEventStruct(item);
        else
        if ( strcmp(type, "createAssistEventRecordStruct") == 0)
            createAssistEventRecordStruct(item);
        else
        if ( strcmp(type, "updateAssistEventRecordStruct") == 0)
            updateAssistEventRecordStruct(item);
        else
        if ( strcmp(type, "updateApplicationFormLeaveStruct") == 0)
            updateApplicationFormLeaveStruct(item);
        else
        if ( strcmp(type, "sensorinfo") == 0)
            updateSensorInfoStruct(item);
        else
        if ( strcmp(type, "sensordevice") == 0)
            updateTransactionSensorDeviceStruct(item);
        else
        if ( strcmp(type, "updatesensordevice") == 0)
            updateSensorDeviceStruct(item);
        else
        if ( strcmp(type, "sensorfiredevent") == 0)
            updateSensorFiredEventStruct(item);
        else
        if ( strcmp(type, "flyinfo") == 0)
            updateFlyInfoStruct(item);
        else
        if ( strcmp(type, "wcinfo") == 0)
            updateWCInfoStruct(item);
        else
        if ( strcmp(type, "updateCurriculumApplyStruct") == 0)
            updateCurriculumApplyStruct(item);
        else
        if ( strcmp(type, "firefightingevent") == 0)
            updateFireFightingEvent(item);
        else
        if ( strcmp(type, "UpdateSoccerRecord") == 0)
            updateSoccerRecord(item);
        else
        if ( strcmp(type, "CreateSoccerRecord") == 0)
            createSoccerRecord(item);

        start1 = end1;

        mProcessingType = "";
        delete item;
    }

}

void MySQLServiceLoopThread::clear_old_data()
{
    std::string outdatetime = currentDateTimeForSQL(MYSQLDB_CLEAR_DATA_DAYS);
    std::string outdatetime90days = currentDateTimeForSQL(90);

    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);

        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if (baseDb == NULL)
            continue;

        baseDb->clear_old_data("AlertRecord", "issuetime", outdatetime.c_str());
        baseDb->clear_old_data("AreaInOutRecord", "issuetime", outdatetime.c_str());
        //baseDb->clear_old_data("CheckPointGroupDailyRecord", "issuetime", outdatetime.c_str());
        //baseDb->clear_old_data("CheckPointPathDailyRecord", "issuetime", outdatetime.c_str());
        //baseDb->clear_old_data("CheckPointTagBehaviorData", "issuetime", outdatetime.c_str());
        baseDb->clear_old_data("CourseRollCallRecord", "issuetime", outdatetime.c_str());
        baseDb->clear_old_data("FiredEvents", "datetime", outdatetime.c_str());
        baseDb->clear_old_data("Footprint", "datetime", outdatetime.c_str());
        baseDb->clear_old_data("GPSInfoRecord", "issuetime", outdatetime.c_str());
        baseDb->clear_old_data("PushNotifyMessage", "datetime", outdatetime.c_str());

        baseDb->clear_old_data("PushNotifyMessageRecord", "datetime", outdatetime.c_str());
        baseDb->clear_old_data("InspectionGroupDailyRecord", "issuetime", outdatetime.c_str());
        baseDb->clear_old_data("InspectionPointDailyRecord", "issuetime", outdatetime.c_str());
        baseDb->clear_old_data("InspectorBehaviorData", "issuetime", outdatetime.c_str());

        baseDb->clear_old_data("SurveyEventRecord", "datetime", outdatetime.c_str());
        baseDb->clear_old_data("SurveyResult", "datetime", outdatetime.c_str());
        baseDb->clear_old_data("PushNotifyMessageStatus", "datetime", outdatetime.c_str());
        baseDb->clear_old_data("CARDRollCallEvent", "datetime", outdatetime.c_str());
        baseDb->clear_old_data("CARDRollCallRecord", "datetime", outdatetime.c_str());
        baseDb->clear_old_data("CARDScanRecord", "datetime", outdatetime.c_str());

        baseDb->clear_old_data("sensorinfo", "datetime", outdatetime90days.c_str());
    }
}

int MySQLServiceLoopThread::send_alive(WorkItem * item)
{
    int ret = 0;
    int projectid = item->getProjectId();

    bool bFound = false;
    for (int i=0;i<MAX_TAG_NUMBER;i++)
    {
        if( (mTagData[i].projectid == projectid) && (strcmp(item->getNodeId(), mTagData[i].TagId) == 0) )
        {
            snprintf(mTagData[i].voltage, 32, "%0.2f", item->getVoltage());
            snprintf(mTagData[i].temperature, 32, "%0.2f", item->getTemperature());

            //printf("thread MySQLServiceLoopThread, send_alive %s(%s,%s)\n",
            //     mTagData[i].TagId, mTagData[i].voltage, mTagData[i].temperature);

            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        for (int i=0;i<MAX_TAG_NUMBER;i++)
        {
            if(strlen(mTagData[i].TagId) == 0)
            {
                mTagData[i].projectid = projectid;
                strcpy(mTagData[i].TagId, item->getNodeId());
                snprintf(mTagData[i].voltage, 32, "%0.2f", item->getVoltage());
                snprintf(mTagData[i].temperature, 32, "%0.2f", item->getTemperature());

//                vilslog::printf("thread MySQLServiceLoopThread, new data send_alive %s(%s,%s)\n",
//                    mTagData[i].TagId, mTagData[i].voltage, mTagData[i].temperature);

                break;
            }
        }
    }
    return ret;
}

int MySQLServiceLoopThread::send_DeleteNode(WorkItem * item)
{
    int ret = 0;
    int projectid = item->getProjectId();

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    baseDb->delete_node_info_bymac(item->getNodeType(), item->getMacaddress());

    return ret;
}

int MySQLServiceLoopThread::send_DeleteAoANode(WorkItem * item)
{
    int ret = 0;
    int projectid = item->getProjectId();

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    baseDb->delete_aoa_node_info_bymac(item->getMacaddress());

    return ret;
}

void MySQLServiceLoopThread::updateInfo(WorkItem * item)
{
    int projectid = item->getProjectId();

    vilslog::printf("MySQLServiceLoopThread::updateInfo() NodeId[%s] NodeName[%s] (%f,%f,%f) BuildingId[%d] floor[%d]\n",
       item->getNodeId(), item->getNodeName(), item->getPosX(), item->getPosY(), item->getPosZ(),
       item->getBuildingId(), item->getFloor());

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
        return;


    if (strlen(item->getNodeId()) > 0)
    {
        vilslog::printf("MySQLServiceLoopThread::updateInfo() NodeId[%s]\n", item->getNodeId());
        baseDb->update_node_info_id(item->getMacaddress(), item->getNodeId());
    }
    else
    if (strlen(item->getNodeName()) > 0)
    {
        vilslog::printf("MySQLServiceLoopThread::updateInfo() NodeName[%s]\n", item->getNodeName());
        baseDb->update_node_info_name(item->getMacaddress(), item->getNodeName());
    }
    else
    if (item->getBuildingId() >= -100 || item->getFloor() >= -100)
    {
        vilslog::printf("MySQLServiceLoopThread::updateInfo()  BuildingId[%d] floor[%d]\n", item->getBuildingId(), item->getFloor());

        baseDb->update_node_info_building(item->getMacaddress(), item->getBuildingId(), item->getFloor());
    }
    else
    {
        vilslog::printf("MySQLServiceLoopThread::updateInfo() (%f,%f,%f)\n", item->getPosX(), item->getPosY(), item->getPosZ());
        baseDb->update_node_info_pos(item->getMacaddress(), item->getPosX(), item->getPosY(), item->getPosZ());
    }

}

void MySQLServiceLoopThread::updateFullInfo(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->Transaction_write_node_info(&item->m_node);
}

void MySQLServiceLoopThread::updateAoANodeInfo(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->Transaction_write_AOA_node_info(&item->mAoANodeRecord);
}

void MySQLServiceLoopThread::send_alert(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);

    if (baseDb != NULL)
    {
        baseDb->write_alert_record(item->getNodeType(), item->getMacaddress());
        baseDb->write_fired_events(projectid, 9999, item->getMaingroupid(),
            currentDateTimeForSQL().c_str(),
            currentDateTimeForSQL().c_str(),
            item->getJsonData());
    }
}

void MySQLServiceLoopThread::send_fencealert(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);

    if (baseDb != NULL)
        baseDb->write_fired_events(projectid, 9999, item->getMaingroupid(),
        currentDateTimeForSQL().c_str(),
        currentDateTimeForSQL().c_str(),
        item->getJsonData());
}

void MySQLServiceLoopThread::send_alert0D(WorkItem * item)
{
    //printf("MySQLServiceLoopThread::send_alert0D() getNodeType[%d] getMacaddress[%s]\n", item->getNodeType(), item->getMacaddress());
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->write_alert0D_record(item->getNodeType(), item->getMacaddress());
        baseDb->write_fired_events(projectid, 9999, item->getMaingroupid(),
            currentDateTimeForSQL().c_str(),
            currentDateTimeForSQL().c_str(),
            item->getJsonData());
    }
}

void MySQLServiceLoopThread::send_alertcancel(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    std::string empid = item->getempid();

    if (baseDb != NULL)
        baseDb->write_fired_events(projectid, 9999, item->getMaingroupid(),
        currentDateTimeForSQL().c_str(),
        currentDateTimeForSQL().c_str(),
        item->getJsonData(), empid);
}

void MySQLServiceLoopThread::send_event(WorkItem * item)
{
    //printf("MySQLServiceLoopThread::send_event()\n");
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);

    if (baseDb != NULL)
        baseDb->write_fired_events(item->m_EventFired.projectid,
        item->m_EventFired.eventid, item->getMaingroupid(),
        item->m_EventFired.timestart,
        item->m_EventFired.timeend,
        item->m_EventFired.eventjson);
}

void MySQLServiceLoopThread::delete_event(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->delete_event(item->m_Event.eventid);
}

void MySQLServiceLoopThread::create_event(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->create_event(item->m_Event);
}

// void MySQLServiceLoopThread::updateCheckPointPathDailyRecord(WorkItem * item)
// {
//     //printf("MySQLServiceLoopThread::updateCheckPointPathDailyRecord()\n");
//     int projectid = item->getProjectId();
//     MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
//
//     if (baseDb != NULL)
//         baseDb->write_CheckPointPathDailyRecord(item->m_DailyRecord);
// }
//
// void MySQLServiceLoopThread::updateCheckPointGroupDailyRecord(WorkItem * item)
// {
//     //printf("MySQLServiceLoopThread::updateCheckPointPathDailyRecord()\n");
//     int projectid = item->getProjectId();
//     MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
//
//     if (baseDb != NULL)
//         baseDb->write_CheckPointGroupDailyRecord(item->m_GroupDailyRecord);
// }
//
// void MySQLServiceLoopThread::updateCheckPointTagBehavior(WorkItem * item)
// {
//     //printf("MySQLServiceLoopThread::updateCheckPointPathDailyRecord()\n");
//     int projectid = item->getProjectId();
//     MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
//
//     if (baseDb != NULL)
//         baseDb->write_CheckPointTagBehavior(item->m_TagBehaviorData);
// }

void MySQLServiceLoopThread::updateInspectionPointDailyRecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);

    if (baseDb != NULL)
        baseDb->write_InspectionPointDailyRecord(item->m_InspectionPointDailyRecord);
}

void MySQLServiceLoopThread::updateInspectionGroupDailyRecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);

    if (baseDb != NULL)
        baseDb->write_InspectionGroupDailyRecord(item->m_InspectionGroupDailyRecord);
}

void MySQLServiceLoopThread::updateInspectorBehavior(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);

    if (baseDb != NULL)
        baseDb->write_InspectorBehaviorData(item->m_InspectorBehaviorData);
}

void MySQLServiceLoopThread::createsection(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->create_SectionGroup(item->m_Sectiongroup);
}

void MySQLServiceLoopThread::deletesection(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->delete_SectionGroup(item->m_Sectiongroup.GroupID);
}

void MySQLServiceLoopThread::createfencealert(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->create_FenceAlert(item->mFenceAlertRecord);
}

void MySQLServiceLoopThread::deletefencealert(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->delete_FenceAlert(item->mFenceAlertRecord.GroupID);
}

// void MySQLServiceLoopThread::deletemaparea(WorkItem * item)
// {
//     int projectid = item->getProjectId();
//     MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
//     if (baseDb != NULL)
//         baseDb->delete_maparea(item->m_MapArea.areaid, item->m_MapArea.projectid);
// }
//
// void MySQLServiceLoopThread::updatemaparea(WorkItem * item)
// {
//     int projectid = item->getProjectId();
//     MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
//     if (baseDb != NULL)
//         baseDb->write_maparea(item->m_MapArea.areaid, item->m_MapArea.projectid, item->m_MapArea.areaname, item->m_MapArea.geojson, item->m_MapArea.type);
// }
//
// void MySQLServiceLoopThread::updatemapareashape(WorkItem * item)
// {
//     int projectid = item->getProjectId();
//     MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
//     if (baseDb != NULL)
//         baseDb->write_mapareashape(item->m_MapArea.areaid, item->m_MapArea.projectid, item->m_MapArea.areaname, item->m_MapArea.geojson);
// }
//
// void MySQLServiceLoopThread::updatemap(WorkItem * item)
// {
//     int projectid = item->getProjectId();
//     MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
//     if (baseDb != NULL)
//         baseDb->write_map(item->m_Map.mapid, item->m_Map.projectid, item->m_Map.mapname, item->m_Map.POSX, item->m_Map.POSY, item->m_Map.SCALE, item->m_Map.COEFF,
//           item->m_Map.FILE, item->m_Map.MAPWIDTH, item->m_Map.MAPHEIGHT);
// }

void MySQLServiceLoopThread::updatemaplayer(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->update_maplayer(item->mMapLayerRecord);
}

void MySQLServiceLoopThread::updatemaplayerarea(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->update_maplayerarea(item->mMapLayerAreaRecord);
}

void MySQLServiceLoopThread::updatemaplayerareashape(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->update_maplayerareashape(item->mMapLayerAreaRecord);
}

void MySQLServiceLoopThread::deletemaplayerarea(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->delete_maplayerarea(item->mMapLayerAreaRecord);
}

// void MySQLServiceLoopThread::createcheckpointgroup(WorkItem * item)
// {
//     int projectid = item->getProjectId();
//     MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
//     if (baseDb != NULL)
//         baseDb->create_CheckPointGroup(item->m_CheckPointGroup.projectid, item->m_CheckPointGroup.GroupName, item->m_CheckPointGroup.StartTime, item->m_CheckPointGroup.EndTime);
// }
//
// void MySQLServiceLoopThread::updatecheckpointgroup(WorkItem * item)
// {
//     int projectid = item->getProjectId();
//     MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
//     if (baseDb != NULL)
//         baseDb->update_CheckPointGroup(item->m_CheckPointGroup.projectid, item->m_CheckPointGroup.GroupID, item->m_CheckPointGroup.GroupName, item->m_CheckPointGroup.StartTime, item->m_CheckPointGroup.EndTime);
// }
//
// void MySQLServiceLoopThread::deletecheckpointgroup(WorkItem * item)
// {
//     int projectid = item->getProjectId();
//     MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
//     if (baseDb != NULL)
//         baseDb->delete_CheckPointGroup(item->m_CheckPointGroup.projectid, item->m_CheckPointGroup.GroupID);
// }
//
// void MySQLServiceLoopThread::createcheckpointpath(WorkItem * item)
// {
//     int projectid = item->getProjectId();
//     MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
//     if (baseDb != NULL)
//         baseDb->create_CheckPointPath(item->m_CheckPointPath);
// }
//
// void MySQLServiceLoopThread::updatecheckpointpathstartnode(WorkItem * item)
// {
//     int projectid = item->getProjectId();
//     MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
//     if (baseDb != NULL)
//         baseDb->update_CheckPointPathStartNode(item->m_CheckPointPath);
// }
//
// void MySQLServiceLoopThread::updatecheckpointpathendnode(WorkItem * item)
// {
//     int projectid = item->getProjectId();
//     MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
//     if (baseDb != NULL)
//         baseDb->update_CheckPointPathEndNode(item->m_CheckPointPath);
// }
//
// void MySQLServiceLoopThread::updatecheckpointpathtraveltime(WorkItem * item)
// {
//     int projectid = item->getProjectId();
//     MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
//     if (baseDb != NULL)
//         baseDb->update_CheckPointPathTravelTime(item->m_CheckPointPath);
// }
//
// void MySQLServiceLoopThread::deletecheckpointpath(WorkItem * item)
// {
//     int projectid = item->getProjectId();
//     MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
//     if (baseDb != NULL)
//         baseDb->delete_CheckPointPath(item->m_CheckPointPath);
// }

// void MySQLServiceLoopThread::createinspectiongroup(WorkItem * item)
// {
//     int projectid = item->getProjectId();
//     MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
//     if (baseDb != NULL)
//         baseDb->create_InspectionGroup(projectid, item->m_InspectionGroup);
// }
//
// void MySQLServiceLoopThread::updateinspectiongroup(WorkItem * item)
// {
//     int projectid = item->getProjectId();
//     MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
//     if (baseDb != NULL)
//         baseDb->update_InspectionGroup(projectid, item->m_InspectionGroup);
// }

void MySQLServiceLoopThread::updateconfig(WorkItem * item)
{
    // int projectid = item->getProjectId();
    // MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    // baseDb->update_config(item->m_Config);

    mDbManager->update_config(item->m_Config);
}

void MySQLServiceLoopThread::deleteconfig(WorkItem * item)
{
    // int projectid = item->getProjectId();
    // MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    // baseDb->delete_config(item->m_Config);

    mDbManager->delete_config(item->m_Config.projectid);

    //mDbManager->reload_sys_config();
}

void MySQLServiceLoopThread::updatesystemconfigudpport(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->update_systemconfigudpport(item->m_SysConfig);
}

void MySQLServiceLoopThread::updateAlgorithmConfig(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->update_AlgorithmConfig(item->m_SysConfig);
}

void MySQLServiceLoopThread::updateLoRAWANConfig(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->update_LoRAWANConfig(item->m_SysConfig);
}

void MySQLServiceLoopThread::createcourse(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->write_Course(&item->m_Course);
}

void MySQLServiceLoopThread::deletecourse(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->delete_Course(&item->m_Course);
}

void MySQLServiceLoopThread::updateCourseRollCallRecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->write_rollcallRecord(&item->m_CourseRollCallRecord);
}

void MySQLServiceLoopThread::updateAreaInOutEventRecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->write_AreaInOutEvent(&item->mInOutEvent);
}

void MySQLServiceLoopThread::updateTagGPSInfoRecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->write_TagGPSInfo(&item->mGPSInfo);
}

void MySQLServiceLoopThread::updateLoRaGatewayInfo(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->write_LoRaGatewayInfo(&item->mLoRaGateway);
}

void MySQLServiceLoopThread::updateBlacklist(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->update_node_info_blacklist(item->getMacaddress(), item->getAnchorids());
}

void MySQLServiceLoopThread::updateWhitelist(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->update_node_info_whitelist(item->getMacaddress(), item->getAnchorids());
}

void MySQLServiceLoopThread::updatejoinRangingRange(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->update_node_info_joinRangingRange(item->getMacaddress(), item->getRange());
}

void MySQLServiceLoopThread::updatejoinNotRangingRange(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->update_node_info_joinNotRangingRange(item->getMacaddress(), item->getRange());
}

void MySQLServiceLoopThread::UpdateResident(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->update_Resident(&item->mResidentRecord);
}

void MySQLServiceLoopThread::DeleteResident(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->delete_Resident(&item->mResidentRecord);
}

void MySQLServiceLoopThread::UpdateEmployee(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->update_Employee(&item->mEmployeeRecord);
        baseDb->update_User(&item->mEmployeeRecord);
    }
}

void MySQLServiceLoopThread::DeleteEmployee(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->delete_Employee(&item->mEmployeeRecord);
        baseDb->delete_User(&item->mEmployeeRecord);
    }
}

void MySQLServiceLoopThread::updateNodeResident(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->update_NodeResident(item->getMacaddress(), item->getSerno());
}

void MySQLServiceLoopThread::updateNodeEmployee(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->update_NodeEmployee(item->getMacaddress(), item->getSerno()); // empid in Serno
}

void MySQLServiceLoopThread::updateLocatorButtontype(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->update_NodeButtontype(item->getMacaddress(), item->getSerno()); // Buttontype in Serno
}

void MySQLServiceLoopThread::updateLocatorSensorAlert(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->update_NodeSensorAlert(item->getMacaddress(), item->getSerno()); // SensorAlert in Serno
}

void MySQLServiceLoopThread::EmployeeEventReport(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->update_EmployeeEventReport(&item->mEmployeeEventReportRecord);
}

// void MySQLServiceLoopThread::UpdatePushNotifyUser(WorkItem * item)
// {
//     int projectid = item->getProjectId();
//     MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
//     if (baseDb != NULL)
//         baseDb->update_PushNotifyUser(&item->mPushNotifyUserRecord);
// }

void MySQLServiceLoopThread::PushNotifyMessage(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->Transaction_write_PushNotifyMessage(&item->mPushNotifyMessage);
    }
}

void MySQLServiceLoopThread::PushNotifyMessageRecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        if(item->mPushNotifyMessageRecord.surveyids.length() > 0)
        {
            baseDb->update_PushNotifyMessageRecord(&item->mPushNotifyMessageRecord);
        }
        else
        {
            baseDb->Transaction_write_PushNotifyMessageRecord(&item->mPushNotifyMessageRecord);
        }
    }
}

void MySQLServiceLoopThread::send_callout(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->write_fired_events(projectid, 8888, item->getMaingroupid(),
        currentDateTimeForSQL().c_str(),
        currentDateTimeForSQL().c_str(),
        item->getJsonData());
}

void MySQLServiceLoopThread::send_doctorevent(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->write_fired_events(projectid, 7777, item->getMaingroupid(),
        currentDateTimeForSQL().c_str(),
        currentDateTimeForSQL().c_str(),
        item->getJsonData());
}

void MySQLServiceLoopThread::send_PowerOff(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->write_fired_events(projectid, 7777, item->getMaingroupid(),
        currentDateTimeForSQL().c_str(),
        currentDateTimeForSQL().c_str(),
        item->getJsonData());
}

void MySQLServiceLoopThread::send_PowerOn(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->write_fired_events(projectid, 7777, item->getMaingroupid(),
        currentDateTimeForSQL().c_str(),
        currentDateTimeForSQL().c_str(),
        item->getJsonData());
}

void MySQLServiceLoopThread::send_OpenDoor(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->write_fired_events(projectid, 7777, item->getMaingroupid(),
        currentDateTimeForSQL().c_str(),
        currentDateTimeForSQL().c_str(),
        item->getJsonData());
}

void MySQLServiceLoopThread::send_CloseDoor(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->write_fired_events(projectid, 7777, item->getMaingroupid(),
        currentDateTimeForSQL().c_str(),
        currentDateTimeForSQL().c_str(),
        item->getJsonData());
}

void MySQLServiceLoopThread::send_EnterDoor(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->write_fired_events(projectid, 7777, item->getMaingroupid(),
        currentDateTimeForSQL().c_str(),
        currentDateTimeForSQL().c_str(),
        item->getJsonData());
}

void MySQLServiceLoopThread::send_ExitDoor(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->write_fired_events(projectid, 7777, item->getMaingroupid(),
        currentDateTimeForSQL().c_str(),
        currentDateTimeForSQL().c_str(),
        item->getJsonData());
}

void MySQLServiceLoopThread::send_bodyinfoweight(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->write_bodyinfo_Weight(item->getNodeId(), item->getWeight());
}

void MySQLServiceLoopThread::send_bodyinfoheartbeat(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->write_bodyinfo_HeartBeat(item->getNodeId(), item->getHeartBeat());

}

void MySQLServiceLoopThread::send_bodyinfoall(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        std::string accountid = item->getNodeId();
        std::string recorderid = item->getRecorderID();
        std::string weight = item->getWeight();
        std::string heartbeat = item->getHeartBeat();
        std::string bloodsugar = item->getBloodSugar();
        std::string systolicbloodpressure = item->getSystolicBloodPressure();
        std::string diastolicbloodpressure = item->getDiastolicBloodPressure();
        std::string bodytemperature = item->getBodyTemperature();
        std::string bodyfat = item->getBodyFat();
        std::string bloodoxygen = item->getBloodOxygen();
        std::string datetime = item->getDateTime();

        baseDb->write_bodyinfo_all(accountid, recorderid, weight, heartbeat, bloodsugar, systolicbloodpressure, diastolicbloodpressure, bodytemperature, bodyfat, bloodoxygen, datetime);

    }
}

void MySQLServiceLoopThread::send_SensorAlert(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
        baseDb->write_fired_events(projectid, 7777, item->getMaingroupid(),
        currentDateTimeForSQL().c_str(),
        currentDateTimeForSQL().c_str(),
        item->getJsonData());
}

void MySQLServiceLoopThread::updatePushNotifySetting(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        int role = item->getRole();
        int NodeType = item->getNodeType();
        baseDb->update_node_PushNotifySetting(NodeType, item->getNodeId(), role);
        vilslog::printf("MySQLServiceLoopThread::updatePushNotifySetting() NodeType[%d] NodeId[%s] role[%d]\n", NodeType, item->getNodeId(), role);
    }
}

void MySQLServiceLoopThread::updateGAESurveyResultData(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->update_GAESurveyResultData(&item->mGAESurveyResultData);
    }
}

void MySQLServiceLoopThread::updateGAEMessageStatusData(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->update_GAEMessageStatusData(&item->mGAEMessageStatusData);
    }
}

void MySQLServiceLoopThread::updateTargetTopicRecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        //baseDb->update_TargetTopicRecord(&item->mTargetTopicRecord);
        baseDb->Transaction_write_TargetTopicRecord(&item->mTargetTopicRecord);
    }
}

void MySQLServiceLoopThread::updateRFIDCardRecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        //baseDb->update_RFIDCardRecord(&item->m_RFIDCard);
        baseDb->Transaction_write_RFIDCardRecord(&item->m_RFIDCard);
    }
}

void MySQLServiceLoopThread::updateRFIDCardRollCallRecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->update_RFIDCardRollCallRecord(item->m_RFIDCard.cardid);
    }
}

void MySQLServiceLoopThread::updateNFCCardRecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->Transaction_write_NFCCardRecord(&item->m_NFCCard);
        baseDb->Transaction_write_MaingroupNFCCardRecord(&item->m_NFCCard);
    }
}

// void MySQLServiceLoopThread::updateNFCCardRollCallRecord(WorkItem * item)
// {
//     int projectid = item->getProjectId();
//     MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
//     if (baseDb != NULL)
//     {
//         baseDb->update_NFCCardRollCallRecord(item->m_NFCCard.cardid);
//     }
// }

void MySQLServiceLoopThread::updateCARDRollCallRecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        // baseDb->update_CARDRollCallRecord(&item->mCARDRollCallRecord);
        baseDb->Transaction_write_CARDRollCallRecord(&item->mCARDRollCallRecord);
    }
}

void MySQLServiceLoopThread::updateCARDRollCallEvent(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        // baseDb->update_CARDRollCallEvent(&item->mCARDRollCallEvent);
        baseDb->Transaction_write_CARDRollCallEvent(&item->mCARDRollCallEvent);
    }
}

void MySQLServiceLoopThread::updateCARDRollCallSubGroupEvent(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        // baseDb->update_CARDRollCallSubGroupEvent(&item->mCARDRollCallSubGroupEvent);
        baseDb->Transaction_write_CARDRollCallSubGroupEvent(&item->mCARDRollCallSubGroupEvent);
    }
}

void MySQLServiceLoopThread::updateCARDScanRecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->update_CARDScanRecord(&item->mCARDScanRecord);
    }
}

void MySQLServiceLoopThread::updateRFIDDevice(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->update_RFIDDevice(&item->m_RFIDDevice);
    }
}

void MySQLServiceLoopThread::updateNFCDevice(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->update_NFCDevice(&item->m_NFCDevice);
    }
}

void MySQLServiceLoopThread::updateEDGEDevice(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->update_EDGEDevice(&item->m_EdgeDevice);
    }
}

void MySQLServiceLoopThread::checkcarduser(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        int userid = item->getUserId();
        std::string rfidcards = item->getRFIDCards();
        std::string nfccards = item->getNFCCards();
        std::string uwbcards = item->getUWBCards();
        std::string maingroups = item->getMaingroups();

        // vilslog::printf("MySQLServiceLoopThread::checkcarduser() userid[%d] rfidcards[%s] nfccards[%s] uwbcards[%s]\n",
        //     userid, rfidcards.c_str(), nfccards.c_str(), uwbcards.c_str());

        baseDb->remove_RFIDCard_user(userid);
        if(rfidcards.length() > 0)
        {
            std::list<std::string> * rfidcardList = convert_string_to_list(rfidcards, ',');

            for (std::list<std::string>::iterator rfidcard = rfidcardList->begin(); rfidcard != rfidcardList->end(); rfidcard++)
            {
                baseDb->update_RFIDCard_user((*rfidcard), userid);
            }

            rfidcardList->clear();
            delete rfidcardList;
        }

        baseDb->remove_NFCCard_user(userid);
        baseDb->remove_MaingroupNFCCard_user(userid, maingroups);
        if(nfccards.length() > 0)
        {
            std::list<std::string> * nfccardList = convert_string_to_list(nfccards, ',');

            for (std::list<std::string>::iterator nfccard = nfccardList->begin(); nfccard != nfccardList->end(); nfccard++)
            {
                baseDb->update_NFCCard_user((*nfccard), userid);
                baseDb->update_MaingroupNFCCard_user((*nfccard), userid, maingroups);
            }

            nfccardList->clear();
            delete nfccardList;
        }

        // baseDb->remove_UWBCard_user(userid);
        // if(uwbcards.length() > 0)
        // {
        //     std::list<std::string> * uwbcardList = convert_string_to_list(uwbcards, ',');
        //
        //     for (std::list<std::string>::iterator uwbcard = uwbcardList->begin(); uwbcard != uwbcardList->end(); uwbcard++)
        //     {
        //         baseDb->update_UWBCard_user((*uwbcard), userid);
        //     }
        //
        //     uwbcardList->clear();
        //     delete uwbcardList;
        // }
    }
}

void MySQLServiceLoopThread::removecarduser(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        int userid = item->getUserId();

        vilslog::printf("MySQLServiceLoopThread::removecarduser() userid[%d] \n", userid);

        baseDb->remove_RFIDCard_user(userid);
    }
}

void MySQLServiceLoopThread::adduseruwbcard(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        int userid = item->getUserId();
        UserStruct * user = baseDb->getUserByUserId(userid);

        //std::string macaddress = item->getUWBCards();
        std::string newuwbcards = item->getUWBCards();

        std::list<std::string> * newuwbcardList = convert_string_to_list(newuwbcards, ',');
        for (std::list<std::string>::iterator macaddress = newuwbcardList->begin(); macaddress != newuwbcardList->end(); macaddress++)
        {
            vilslog::printf("MySQLServiceLoopThread::adduseruwbcard() userid[%d] macaddress[%s]\n", userid, (*macaddress).c_str());

            bool bFound = false;
            for (std::list<std::string>::iterator uwbcard = user->uwbcards->begin(); uwbcard != user->uwbcards->end(); uwbcard++)
            {
                if ( (*macaddress).compare((*uwbcard)) == 0 )
                {
                    bFound = true;
                    break;
                }
            }

            if (!bFound)
            {
                user->uwbcards->push_back((*macaddress));
                //std::string uwbcards = convert_list_to_string(user->uwbcards);
                //baseDb->update_user_uwbcards(userid, uwbcards.c_str());
            }
        }

        std::string uwbcards = convert_list_to_string(user->uwbcards);
        baseDb->update_user_uwbcards(userid, uwbcards.c_str());

        if ( user->rfidcards != NULL )
        {
            user->rfidcards->clear();
            delete user->rfidcards;
        }
        if ( user->nfccards != NULL )
        {
            user->nfccards->clear();
            delete user->nfccards;
        }
        if ( user->uwbcards != NULL )
        {
            user->uwbcards->clear();
            delete user->uwbcards;
        }
        delete user;

        //
        // remove from another user
        //
        for (std::list<std::string>::iterator macaddress = newuwbcardList->begin(); macaddress != newuwbcardList->end(); macaddress++)
        {
            std::list<UserStruct*> * userList = baseDb->getUserByUWBCard((*macaddress).c_str());

            // vilslog::printf("MySQLServiceLoopThread::adduseruwbcard() userList->size()[%d] \n", userList->size());

            for (std::list<UserStruct*>::iterator user = userList->begin(); user != userList->end(); user++)
            {
                if ( (*user)->userid != userid )
                {
                    // vilslog::printf("MySQLServiceLoopThread::adduseruwbcard() (*user)->userid[%d] 1 uwbcards->size()[%d] \n", (*user)->userid, (*user)->uwbcards->size());

                    (*user)->uwbcards->remove((*macaddress));

                    // vilslog::printf("MySQLServiceLoopThread::adduseruwbcard() (*user)->userid[%d] 2 uwbcards->size()[%d] \n", (*user)->userid, (*user)->uwbcards->size());
                    std::string uwbcards = convert_list_to_string((*user)->uwbcards);
                    // vilslog::printf("MySQLServiceLoopThread::adduseruwbcard() (*user)->userid[%d] uwbcards[%s] \n",  (*user)->userid, uwbcards.c_str());
                    baseDb->update_user_uwbcards((*user)->userid, uwbcards.c_str());
                }
            }

            clear_UserStruct_list(userList);
            delete userList;
        }

        if ( newuwbcardList != NULL )
        {
            newuwbcardList->clear();
            delete newuwbcardList;
        }
    }
}

void MySQLServiceLoopThread::updateUserStruct(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->update_UserStruct(&item->m_UserData);

        baseDb->remove_RFIDCard_user(item->m_UserData.userid);
        for (std::list<std::string>::iterator rfidcard = item->m_UserData.rfidcards->begin(); rfidcard != item->m_UserData.rfidcards->end(); rfidcard++)
        {
            baseDb->update_RFIDCard_user((*rfidcard), item->m_UserData.userid);
        }

        std::string maingroups = convert_list_to_string(item->m_UserData.maingroups);

        baseDb->remove_NFCCard_user(item->m_UserData.userid);
        baseDb->remove_MaingroupNFCCard_user(item->m_UserData.userid, maingroups);
        for (std::list<std::string>::iterator nfccard = item->m_UserData.nfccards->begin(); nfccard != item->m_UserData.nfccards->end(); nfccard++)
        {
            baseDb->update_NFCCard_user((*nfccard), item->m_UserData.userid);
            baseDb->update_MaingroupNFCCard_user((*nfccard), item->m_UserData.userid, maingroups);
        }
    }
}

void MySQLServiceLoopThread::updateUserPoistion(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        int userid = item->getUserId();
        double posX = item->getPosX();
        double posY = item->getPosY();
        double posZ = item->getPosZ();
        baseDb->update_UserPoistion(userid, posX, posY, posZ);
    }
}

void MySQLServiceLoopThread::updateUserLocation(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        int userid = item->getUserId();
        std::string Longitude = item->getLongitude();
        std::string Latitude = item->getLatitude();
        baseDb->update_UserLocation(userid, Latitude, Longitude);
    }
}

void MySQLServiceLoopThread::updateUserPassword(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        int userid = item->getUserId();
        std::string accountid = item->getAccountId();
        std::string password = item->getUserPwd();
        baseDb->update_user_password(accountid, userid, password);
    }
}

void MySQLServiceLoopThread::updateusersubgroup(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->update_UserSubGroup(&item->m_UserSubGroup);
    }
}

void MySQLServiceLoopThread::updateusermaingroup(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->update_UserMainGroup(&item->m_UserMainGroup);
    }
}

void MySQLServiceLoopThread::deleteGateway(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        std::string gwid = item->getMacaddress();

        baseDb->delete_LoRaGateway(gwid);
    }
}

void MySQLServiceLoopThread::clearUnknownNFCCardAlive(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        std::string updatetime = BeforeMinDateTimeForSQL(1);

        baseDb->clear_UnknownNFCCardAlive(updatetime);
    }
}

void MySQLServiceLoopThread::clearUnknownRFIDCardAlive(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        std::string updatetime = BeforeMinDateTimeForSQL(1);

        baseDb->clear_UnknownRFIDCardAlive(updatetime);
    }
}

void MySQLServiceLoopThread::updateLoginToken(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->update_Account_LoginKey(item->m_LoginToken.accountid, item->m_LoginToken.userid, item->m_LoginToken.loginkey, item->m_LoginToken.token);
    }
}

void MySQLServiceLoopThread::updateAssistEventStruct(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->update_AssistEvent(&item->mAssistEvent);
    }
}

void MySQLServiceLoopThread::createAssistEventRecordStruct(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->createAssistEventRecord(&item->mAssistEventRecord);
    }
}

void MySQLServiceLoopThread::updateAssistEventRecordStruct(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->updateAssistEventRecord(&item->mAssistEventRecord);
    }
}

void MySQLServiceLoopThread::updateApplicationFormLeaveStruct(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->update_LeaveForm(&item->mApplicationFormLeave);
    }
}

void MySQLServiceLoopThread::updateSensorInfoStruct(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->Transaction_update_SensorInfo(&item->mSensorInfo);
    }
}

void MySQLServiceLoopThread::updateTransactionSensorDeviceStruct(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->Transaction_update_SensorDevice(&item->mSensorDevice);
    }
}

void MySQLServiceLoopThread::updateSensorDeviceStruct(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->update_SensorDevice(&item->mSensorDevice);
    }
}

void MySQLServiceLoopThread::updateSensorFiredEventStruct(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->update_SensorFiredEvent(&item->mSensorFiredEvent);
    }
}

void MySQLServiceLoopThread::updateFlyInfoStruct(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->Transaction_update_FlyInfo(&item->mFlyInfo);
    }
}

void MySQLServiceLoopThread::updateWCInfoStruct(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->Transaction_update_WCInfo(&item->mWCInfo);
    }
}

void MySQLServiceLoopThread::updateCurriculumApplyStruct(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->update_CurriculumApply(&item->mCurriculumApply);
    }
}

void MySQLServiceLoopThread::updateFireFightingEvent(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        std::string jsonstring;
        std::string casenumber;

        if (item->m_FireFightingType.compare("FireFightingInfo") == 0)
        {
            baseDb->updateFireFightingCase(&item->m_FireFightingInfo);

            jsonstring = convert_FireFightingStruct_to_json_str(&item->m_FireFightingInfo);
            casenumber = item->m_FireFightingInfo.casenumber;
        }
        else
        if (item->m_FireFightingType.compare("FireFightingUser") == 0)
        {
            baseDb->updateFireFightingUser(&item->m_FireFightingUser);

            jsonstring = convert_FireFightingUserStruct_to_json_str(&item->m_FireFightingUser);
            casenumber = item->m_FireFightingUser.casenumber;
        }
        else
        if (item->m_FireFightingType.compare("FireFightingAirUsing") == 0)
        {
            baseDb->updateFireFightingAirUsing(&item->m_FireFightingAirUsing);
        }
        else
        if (item->m_FireFightingType.compare("UserFireFightingAirUsing") == 0)
        {
            baseDb->updateUserFireFightingAirUsing(item->m_FireFightingAirUsing.userid, item->m_FireFightingAirUsing.airusing);
        }

        if (jsonstring.length() > 0 && casenumber.length() > 0)
        {
            baseDb->addFireFightingEvent(item->m_FireFightingType, casenumber, jsonstring);
        }
    }
}

void MySQLServiceLoopThread::updateSoccerRecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->updateSoccerRecord(&item->m_SoccerRecording);
    }
}

void MySQLServiceLoopThread::createSoccerRecord(WorkItem * item)
{
    int projectid = item->getProjectId();
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        baseDb->createSoccerRecord(&item->m_SoccerRecording);
    }
}
