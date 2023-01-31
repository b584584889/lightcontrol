/*
 *  AreaRollCallService.cpp
 *
 *  Created on: 2019/07/30
 *      Author: ikki
 */
#include <string.h>

#include "AreaRollCallService.h"

using namespace std;

AreaRollCallService::AreaRollCallService(wqueue<AreaInOutEventStruct*> & mareaInOutEventQueue, wqueue<AreaInOutEventStruct*> & mcourseAreaInOutEventQueue, wqueue<AreaInOutEventStruct*> & pWebSocketAreaInOutEventQueue, DeviceManager * pDevManager, wqueue<WorkItem*> & trackdbqueue, DBManager * dbManager)
    : mAreaInOutEventQueue(mareaInOutEventQueue), mCourseAreaInOutEventQueue(mcourseAreaInOutEventQueue), mWebSocketAreaInOutEventQueue(pWebSocketAreaInOutEventQueue), mDeviceManager(pDevManager), mTrackDBqueue(trackdbqueue), mDbManager(dbManager)
{
}

AreaRollCallService::~AreaRollCallService()
{
}

bool AreaRollCallService::status()
{
    return mInitial;
}

std::string AreaRollCallService::statusString()
{
    return mStatus;
}

void AreaRollCallService::updateProject()
{
    mUpdateProject = true;
}

void * AreaRollCallService::run()
{
    while(1)
    {
        try
        {
            mPrjidList = mDbManager->getAllProjectConfig();

            initialAreaInfo();

            prepairTagBuffer();
            prepairUserBuffer();

            runloop();
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("AreaRollCallService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "AreaRollCallService::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void AreaRollCallService::runloop()
{
    long long start1 = getSystemTime();
    long long end1 = getSystemTime();
    long long difft = end1 - start1;
    long long checkTime = 0;
    long long checkSleepTime = 0;
    long long updatePrjIdTime = 0;

    mInitial = true;
    mStatus = "AreaRollCallService()::run() Initial successfully!";
    vilslog::printf ("AreaRollCallService()::run() Initial successfully!\n");

    while (1)
    {
        end1 = getSystemTime();
        difft = end1 - start1;

        checkTime += difft;
        checkSleepTime += difft;
        updatePrjIdTime += difft;

        if ( checkTime >= 500) // 500 miliseconds
        {
            if (mUpdateAreaInfo)
            {
                reloadAreaList();
                mUpdateAreaInfo = false;
            }

            checkTagArea();

            checkTagAreaBuffer((int)checkTime);

            checkTime = 0;
        }

        if (checkSleepTime > 2000)
        {
            checkTagSleep();
            checkSleepTime = 0;
        }

        if (updatePrjIdTime > 3600000 || mUpdateProject)
        {
            mPrjidList->clear();
            delete mPrjidList;
            mPrjidList = mDbManager->getAllProjectConfig();

            dump_cache_data();

            updatePrjIdTime = 0;
            mUpdateProject = false;
        }

        start1 = end1;
        usleep(500000); // 500 miliseconds
    }
}

void AreaRollCallService::updateAreaInfo()
{
    mUpdateAreaInfo = true;
}

void AreaRollCallService::dump_cache_data()
{
    for (std::vector<ProjectAreaInfoStruct*>::iterator areaInfo = mProjectAreaInfoVector.begin(); areaInfo != mProjectAreaInfoVector.end(); areaInfo++)
    {
        (*areaInfo)->m_CacheDB->loadOrSaveDb((*areaInfo)->projectid, 1);
    }
}

void AreaRollCallService::reloadAreaList()
{
    for (std::vector<ProjectAreaInfoStruct*>::iterator areaInfo = mProjectAreaInfoVector.begin(); areaInfo != mProjectAreaInfoVector.end(); areaInfo++)
    {
        reloadAreaList( (*areaInfo) );
    }
}

void AreaRollCallService::reloadAreaList(ProjectAreaInfoStruct * areaInfo)
{
    if (areaInfo->mRaycasting == NULL)
    {
        areaInfo->mRaycasting = new RayCasting();
    }
    areaInfo->mRaycasting->stop();

    if (areaInfo->mAreaList != NULL)
    {
        clear_maplayerarea_list(areaInfo->mAreaList);
        delete areaInfo->mAreaList;
    }

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(areaInfo->projectid);
    if (baseDb == NULL)
        return;

    areaInfo->mAreaList = baseDb->read_maplayerarea_not_wall();

    std::list<MapLayerStruct*> * maplayers = baseDb->read_maplayers();

    for (std::list <MapLayerAreaStruct *>::iterator i = areaInfo->mAreaList->begin (); i != areaInfo->mAreaList->end (); i++)
    {
        for (std::list <MapLayerStruct *>::iterator maplayer = maplayers->begin (); maplayer != maplayers->end (); maplayer++)
        {
            if ( (*maplayer)->layerid == (*i)->layerid )
            {
                (*i)->WGS48_Origin_X = std::stod((*maplayer)->WGS48OriginX);
                (*i)->WGS48_Origin_Y = std::stod((*maplayer)->WGS48OriginY);
                break;
            }
        }
    }

    clear_MapLayerStruct_list(maplayers);
    delete maplayers;

    areaInfo->mRaycasting->setAreaList(areaInfo->mAreaList);
    areaInfo->mRaycasting->start();

}

AreaRollCallInfoDBAccess * AreaRollCallService::create_cacheDB(int projectid, ProjectAreaInfoStruct * areainfo)
{
    AreaRollCallInfoDBAccess * CacheDB = new AreaRollCallInfoDBAccess();
    CacheDB->init();

    // loda sqlite3 cache from file
    CacheDB->loadOrSaveDb(projectid, 0);
    bool bExisted = CacheDB->checkDBExist();

    vilslog::printf("AreaRollCallService::create_cacheDB() m_CacheDB projectid[%d] bExisted[%d]\n", projectid, bExisted);

    if (bExisted)
    {
        CacheDB->check_valid_table();

        //
        // read cache data
        //
        std::list<TagAreaBufferStruct*> * tagAreaBufferList = CacheDB->getTagAreaBufferList();

        // vilslog::printf("AreaRollCallService::create_cacheDB() m_CacheDB projectid[%d] tagAreaBufferList->size()[%d]\n", projectid, (int)tagAreaBufferList->size());

        std::list<SleepPoolStruct*> * sleepPoolStructList = CacheDB->getSleepPoolList();

        // vilslog::printf("AreaRollCallService::create_cacheDB() m_CacheDB projectid[%d] sleepPoolStructList->size()[%d]\n", projectid, (int)sleepPoolStructList->size());

        assign_TagAreaBuffer_cache_data(projectid, tagAreaBufferList);
        assign_TagSleepPool_cache_data(projectid, sleepPoolStructList);

        clear_TagAreaBufferStruct_list(tagAreaBufferList);
        delete tagAreaBufferList;
        clear_SleepPoolStruct_list(sleepPoolStructList);
        delete sleepPoolStructList;
    }
    else
    if (!bExisted)
    {
        CacheDB->init();
    }
    return CacheDB;
}

void AreaRollCallService::assign_TagAreaBuffer_cache_data(int projectid, std::list<TagAreaBufferStruct*> * tagAreaBufferList)
{
    // vilslog::printf("assign_TagAreaBuffer_cache_data() tagAreaBufferList->size()[%d]\n", (int)tagAreaBufferList->size());

    for (std::list<TagAreaBufferStruct*>::iterator cache_buf = tagAreaBufferList->begin(); cache_buf != tagAreaBufferList->end(); cache_buf++)
    {
        //
        // find existed data
        //
        bool bFound = false;
        for (std::list<TagAreaBufferStruct*>::iterator glbuf = mTagAreaBufferList.begin(); glbuf != mTagAreaBufferList.end(); glbuf++)
        {
            if ( ((*glbuf)->projectid == projectid) && ((*glbuf)->nodeid.compare((*cache_buf)->nodeid) == 0) )
            {
                // existed!
                bFound = true;
                break;
            }
        }

        if (bFound)
        {
            remove_cache_TagAreaBuffer(projectid, (*cache_buf));

            // vilslog::printf("assign_TagAreaBuffer_cache_data() Found same projectid[%d] nodeid[%s] tafid[%d]\n",
            //     projectid, (*cache_buf)->nodeid.c_str(), (*cache_buf)->tafid);

            continue;
        }

        TagAreaBufferStruct * newBuffer = new TagAreaBufferStruct();
        newBuffer->projectid = projectid;
        newBuffer->nodeid = (*cache_buf)->nodeid;
        newBuffer->tafid = (*cache_buf)->tafid;
        newBuffer->isuser = (*cache_buf)->isuser;
        mTagAreaBufferList.push_back(newBuffer);

        // vilslog::printf("\nassign_TagAreaBuffer_cache_data() projectid[%d] nodeid[%s] (*cache_buf)->mBufferAreaList.size()[%d]\n",
        //     projectid, newBuffer->nodeid.c_str(), (int)(*cache_buf)->mBufferAreaList.size());

        newBuffer->mBufferAreaList.clear();
        for (std::list<BufferAreaInfoStruct*>::iterator cache_bufarea = (*cache_buf)->mBufferAreaList.begin(); cache_bufarea != (*cache_buf)->mBufferAreaList.end(); cache_bufarea++)
        {

            // vilslog::printf("assign_TagAreaBuffer_cache_data() (*cache_bufarea)->areaid[%s][%s] AreaType0D[%d] MapAreaType[%d]\n",
            //     (*cache_bufarea)->areaid.c_str(), (*cache_bufarea)->areaname.c_str(), (*cache_bufarea)->AreaType0D, (*cache_bufarea)->MapAreaType);
            //
            // vilslog::printf("assign_TagAreaBuffer_cache_data() checked_in[%d] velocity_in[%f] voltage_in[%f] timestamp_in[%s]\n",
            //     (*cache_bufarea)->checked_in, (*cache_bufarea)->velocity_in, (*cache_bufarea)->voltage_in, time_tToString((*cache_bufarea)->timestamp_in/1000).c_str());
            //
            // vilslog::printf("assign_TagAreaBuffer_cache_data() checked_out[%d] velocity_out[%f] voltage_out[%f] timestamp_out[%s]\n",
            //     (*cache_bufarea)->checked_out, (*cache_bufarea)->velocity_out, (*cache_bufarea)->voltage_out, time_tToString((*cache_bufarea)->timestamp_out/1000).c_str());

            // create new buffer
            BufferAreaInfoStruct * newBufferArea = new BufferAreaInfoStruct();
            newBufferArea->AreaType0D = (*cache_bufarea)->AreaType0D;
            newBufferArea->MapAreaType = (*cache_bufarea)->MapAreaType;
            newBufferArea->areaid = (*cache_bufarea)->areaid;
            newBufferArea->areaname = (*cache_bufarea)->areaname;

            // in action
            newBufferArea->checked_in = (*cache_bufarea)->checked_in;
            newBufferArea->velocity_in = (*cache_bufarea)->velocity_in;
            newBufferArea->voltage_in = (*cache_bufarea)->voltage_in;
            newBufferArea->timestamp_in = (*cache_bufarea)->timestamp_in;

            // out action
            newBufferArea->checked_out = (*cache_bufarea)->checked_out;
            newBufferArea->velocity_out = (*cache_bufarea)->velocity_out;
            newBufferArea->voltage_out = (*cache_bufarea)->voltage_out;
            newBufferArea->timestamp_out = (*cache_bufarea)->timestamp_out;

            newBuffer->mBufferAreaList.push_back(newBufferArea);

            if ( newBufferArea->checked_in && !newBufferArea->checked_out)
            {
                mDeviceManager->setTagInfoArea(projectid, newBuffer->nodeid.c_str(), newBufferArea->areaid.c_str(), newBufferArea->areaname.c_str());

                if ( newBufferArea->MapAreaType == 5 )
                {
                    mDeviceManager->setTagInfoAreaType5(projectid, newBuffer->nodeid.c_str(), newBufferArea->areaid.c_str(), newBufferArea->areaname.c_str());
                }
            }
            else
            {
                mDeviceManager->setTagInfoArea(projectid, newBuffer->nodeid.c_str(), "", "");
                if ( newBufferArea->MapAreaType == 5 )
                {
                    mDeviceManager->setTagInfoAreaType5(projectid, newBuffer->nodeid.c_str(), "", "");
                }
            }
        }
        // vilslog::printf("assign_TagAreaBuffer_cache_data() projectid[%d] nodeid[%s] tafid[%d] mBufferAreaList.size()[%d]\n",
        //     projectid, newBuffer->nodeid.c_str(), newBuffer->tafid, (int)newBuffer->mBufferAreaList.size());
    }
    // vilslog::printf("assign_TagAreaBuffer_cache_data() mTagAreaBufferList.size()[%d]\n", (int)mTagAreaBufferList.size());
}

void AreaRollCallService::assign_TagSleepPool_cache_data(int projectid, std::list<SleepPoolStruct*> * sleepPoolStructList)
{
    for (std::list<SleepPoolStruct*>::iterator cache_buf = sleepPoolStructList->begin(); cache_buf != sleepPoolStructList->end(); cache_buf++)
    {
        //
        // find existed data
        //
        bool bFound = false;
        for (std::list<SleepPoolStruct*>::iterator glbuf = mTagSleepPoolList.begin(); glbuf != mTagSleepPoolList.end(); glbuf++)
        {
            if ( ((*glbuf)->projectid == projectid) && ((*glbuf)->nodeid.compare((*cache_buf)->nodeid) == 0) )
            {
                // existed!
                bFound = true;
                break;
            }
        }

        if (bFound)
        {
            remove_cache_SleepPool(projectid, (*cache_buf));

            // vilslog::printf("assign_TagSleepPool_cache_data() Found same projectid[%d] nodeid[%s] tafid[%d]\n",
            //     projectid, (*cache_buf)->nodeid.c_str(), (*cache_buf)->tafid);

            continue;
        }

        SleepPoolStruct * newPool = new SleepPoolStruct();
        newPool->projectid = projectid;
        newPool->nodeid = (*cache_buf)->nodeid;
        newPool->tafid = (*cache_buf)->tafid; // 0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師
        newPool->isuser = (*cache_buf)->isuser;
        newPool->check_count = (*cache_buf)->check_count;
        newPool->slept = (*cache_buf)->slept;
        newPool->timestamp = (*cache_buf)->timestamp;

        //
        // assign to global list
        //
        mTagSleepPoolList.push_back(newPool);

        mDeviceManager->setTagSleep(projectid, (*cache_buf)->nodeid.c_str(), (*cache_buf)->slept);
    }
    // vilslog::printf("assign_TagSleepPool_cache_data() mTagSleepPoolList.size()[%d]\n", (int)mTagSleepPoolList.size());
}

void AreaRollCallService::initialAreaInfo()
{
    mTagAreaBufferList.clear();
    mTagSleepPoolList.clear();

    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);

        ProjectAreaInfoStruct * newInfo = new ProjectAreaInfoStruct();

        newInfo->projectid = projectid;
        newInfo->mRaycasting = NULL;
        newInfo->mAreaList = NULL;

        reloadAreaList(newInfo);

        newInfo->m_CacheDB = create_cacheDB(projectid, newInfo);

        mProjectAreaInfoVector.push_back(newInfo);
    }
}

void AreaRollCallService::prepairTagBuffer()
{
    mUpdateAreaBuffer = true;

    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);

        ProjectAreaInfoStruct * areaInfo = NULL;
        for (std::vector<ProjectAreaInfoStruct*>::iterator prjAreaInfo = mProjectAreaInfoVector.begin(); prjAreaInfo != mProjectAreaInfoVector.end(); prjAreaInfo++)
        {
            if ( (*prjAreaInfo)->projectid == projectid )
            {
                areaInfo = (*prjAreaInfo);
                break;
            }
        }

        if (areaInfo == NULL)
        {
            continue;
        }

        std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);

        for (std::list<NodeStruct*>::iterator node = tagNodeList->begin(); node != tagNodeList->end(); node++)
        {
            if ( strlen((*node)->AreaId) > 0 )
            {
#ifdef PRINT_AREA_ROLLCALL_LOG
                vilslog::printf("AreaRollCallService::prepairTagBuffer() [%s] AreaName[%s] AreaId0D[%s] AreaType5Name[%s] area_unknown[%d] exclude_rollcall_AreaId[%s]\n",
                    (*node)->nodeid, (*node)->AreaName, (*node)->AreaId0D, (*node)->AreaType5Name, (*node)->area_unknown, (*node)->exclude_rollcall_AreaId);
#endif
                //
                // 檢查是否在病房內 5:病房
                //
                int results[3];
                results[0] = (*node)->posX;
                results[1] = (*node)->posY;
                results[2] = (*node)->posZ;

                MapLayerAreaStruct * areaType5 = areaInfo->mRaycasting->findNodeAreaStructWithtype((*node)->maplayer, results[0], results[1], 5);
                if (areaType5 != NULL)
                {
                    // vilslog::printf("AreaRollCallService::prepairTagBuffer() [%s] AreaType5ID[%s] AreaType5Name[%s] areaType5 != NULL\n",
                    //     (*node)->nodeid, areaType5->areaid.c_str(), areaType5->areaname.c_str());

                    mDeviceManager->setTagInfoAreaType5(projectid, (*node)->nodeid, areaType5->areaid.c_str(), areaType5->areaname.c_str());
                    setTagInfoArea(projectid, areaType5, 0, (*node)->nodeid, (*node)->tafid, (*node)->AreaId0D, (*node)->AreaType0D, (*node)->velocity, (*node)->voltage, 1);// 1:in
                    move_to_sleeping_pool(projectid, (*node)->nodeid, (*node)->tafid);
                    mDeviceManager->setTagSleep(projectid, (*node)->nodeid, 1);
                }

                // find area
                for (std::list <MapLayerAreaStruct *>::iterator area = areaInfo->mAreaList->begin(); area != areaInfo->mAreaList->end(); area++)
                {
                    if ( (*area)->areaid.compare((*node)->AreaId) == 0 )
                    {
                        setTagInfoArea(projectid, (*area), 0, (*node)->nodeid, (*node)->tafid, (*node)->AreaId0D, (*node)->AreaType0D, (*node)->velocity, (*node)->voltage, 1);// 1:in
                        move_to_sleeping_pool(projectid, (*node)->nodeid, (*node)->tafid);
                        mDeviceManager->setTagSleep(projectid, (*node)->nodeid, 1);
                        break;
                    }
                }
            }
        }
    }

    mUpdateAreaBuffer = false;
}


void AreaRollCallService::prepairUserBuffer()
{
    mUpdateAreaBuffer = true;

    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);

        ProjectAreaInfoStruct * areaInfo = NULL;
        for (std::vector<ProjectAreaInfoStruct*>::iterator prjAreaInfo = mProjectAreaInfoVector.begin(); prjAreaInfo != mProjectAreaInfoVector.end(); prjAreaInfo++)
        {
            if ( (*prjAreaInfo)->projectid == projectid )
            {
                areaInfo = (*prjAreaInfo);
                break;
            }
        }

        if (areaInfo == NULL)
        {
            continue;
        }

        std::list<UserStruct*> * userList = mDeviceManager->get_User_list(projectid);

        for (std::list<UserStruct*>::iterator user = userList->begin(); user != userList->end(); user++)
        {
            if ( (*user)->AreaId.length() > 0 )
            {
                vilslog::printf("AreaRollCallService::prepairUserBuffer() userid[%d] AreaName[%s] AreaId[%s]\n",
                    (*user)->userid, (*user)->AreaName.c_str(), (*user)->AreaId.c_str());

                //
                // 檢查是否在病房內 5:病房
                //
                int results[3];
                results[0] = (*user)->posX;
                results[1] = (*user)->posY;
                results[2] = (*user)->posZ;

                int tafid = 0;
                std::string nodeid = std::to_string((*user)->userid);
                std::string AreaId0D = (*user)->AreaId0D;
                int AreaType0D = (*user)->AreaType0D;
                double velocity = 0.0;
                std::string voltage = "0";

                MapLayerAreaStruct * areaType5 = areaInfo->mRaycasting->findNodeAreaStructWithtype((*user)->maplayer, results[0], results[1], 5);
                if (areaType5 != NULL)
                {
                    vilslog::printf("AreaRollCallService::prepairUserBuffer() userid[%d] AreaType5ID[%s] AreaType5Name[%s] areaType5 != NULL\n",
                        (*user)->userid, areaType5->areaid.c_str(), areaType5->areaname.c_str());

                    // mDeviceManager->setTagInfoAreaType5(projectid, (*node)->nodeid, areaType5->areaid.c_str(), areaType5->areaname.c_str());
                    setTagInfoArea(projectid, areaType5, 1, nodeid, tafid, AreaId0D, AreaType0D, velocity, voltage, 1);// 1:in
                    move_to_sleeping_pool(projectid, nodeid, tafid);
                    // mDeviceManager->setTagSleep(projectid, (*node)->nodeid, 1);
                }

                // find area
                for (std::list <MapLayerAreaStruct *>::iterator area = areaInfo->mAreaList->begin(); area != areaInfo->mAreaList->end(); area++)
                {
                    if ( (*area)->areaid.compare((*user)->AreaId) == 0 )
                    {
                        setTagInfoArea(projectid, (*area), 1, nodeid, tafid, AreaId0D, AreaType0D, velocity, voltage, 1);// 1:in
                        move_to_sleeping_pool(projectid, nodeid, tafid);
                        // mDeviceManager->setTagSleep(projectid, (*node)->nodeid, 1);
                        break;
                    }
                }
            }
        }
    }

    mUpdateAreaBuffer = false;
}

int AreaRollCallService::checkTagArea()
{
    int count = 0;
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);

        for (std::vector<ProjectAreaInfoStruct*>::iterator areaInfo = mProjectAreaInfoVector.begin(); areaInfo != mProjectAreaInfoVector.end(); areaInfo++)
        {
            if ( (*areaInfo)->projectid == projectid )
            {
                count += checkTagArea((*areaInfo));
                // break;
            }
        }
    }
    return count;
}

int AreaRollCallService::checkTagArea(ProjectAreaInfoStruct * areainfo)
{
    int count = 0;

    std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(areainfo->projectid);
    for (std::list<NodeStruct*>::iterator node = tagNodeList->begin(); node != tagNodeList->end(); node++)
    {
        if ( (*node)->alive > 0 )
        {
            // check alive time
            time_t t_cur = getCurrentTimeInMilliSec();
            time_t t_diff = t_cur - (*node)->updateTimestamp;
            int i_diff = (int)t_diff;
            if (i_diff < NODE_ALIVE_TIMEOUT)
            {
                count += checkTagArea(areainfo, 0, (*node)->nodeid, (*node)->tafid, (*node)->AreaId0D, (*node)->AreaType0D, (*node)->velocity, (*node)->voltage, (*node)->smoothingPosX, (*node)->smoothingPosY, (*node)->smoothingPosZ, (*node)->maplayer, (*node)->exclude_rollcall_AreaId, (*node)->RangingResultType, (*node)->Opertaion_Mode);
            }
        }
    }

    std::list<UserStruct*> * userList = mDeviceManager->get_User_list(areainfo->projectid);

    for (std::list<UserStruct*>::iterator user = userList->begin(); user != userList->end(); user++)
    {
        int tafid = 0;
        std::string nodeid = std::to_string((*user)->userid);
        std::string AreaId0D = (*user)->AreaId0D;
        int AreaType0D = (*user)->AreaType0D;
        double velocity = 0.0;
        std::string voltage = "0";
        int Opertaion_Mode = 0;

        count += checkTagArea(areainfo, 1, nodeid, tafid, AreaId0D, AreaType0D, velocity, voltage, (*user)->posX, (*user)->posY, (*user)->posZ, (*user)->maplayer, (*user)->exclude_rollcall_AreaId, (*user)->RangingResultType, Opertaion_Mode);
    }

    return count;
}

int AreaRollCallService::checkTagArea(ProjectAreaInfoStruct * areainfo, int isuser, std::string nodeid, int tafid, std::string AreaId0D, int AreaType0D, double velocity, std::string voltage, int posX, int posY, int posZ, int maplayer, std::string exclude_rollcall_AreaId, int RangingResultType, int Opertaion_Mode)
{
    int results[3];
    results[0] = posX;
    results[1] = posY;
    results[2] = posZ;

#ifdef PRINT_AREA_ROLLCALL_LOG
    vilslog::printf("\nAreaRollCallService::checkTagArea() TagId[%s] results[%d, %d]\n", nodeid.c_str(), results[0], results[1]);
#endif

    MapLayerAreaStruct * area = areainfo->mRaycasting->findNodeAreaStruct(maplayer, results[0], results[1]);
    if (area == NULL)
    {
#ifdef PRINT_AREA_ROLLCALL_LOG
        vilslog::printf("AreaRollCallService::checkTagArea() TagId[%s] area == NULL\n", nodeid.c_str());
#endif
        setTagInfoArea(areainfo->projectid, NULL, isuser, nodeid, tafid, AreaId0D, AreaType0D, velocity, voltage, 0);// 0:no action
    }
    else
    {

#ifdef PRINT_AREA_ROLLCALL_LOG
        vilslog::printf("AreaRollCallService::checkTagArea() TagId[%s] areaid[%s] areaname[%s] area->type[%d] area->layerid[%d] node->maplayer[%d]\n",
                nodeid.c_str(), area->areaid.c_str(), area->areaname.c_str(), area->type, area->layerid, maplayer);
#endif
        //
        // check map layer
        //
        if (area->layerid != maplayer )
        {
            setTagInfoArea(areainfo->projectid, NULL, isuser, nodeid, tafid, AreaId0D, AreaType0D, velocity, voltage, 0);// 0:no action
            return 0;
        }

        if ( area->areaid.compare(exclude_rollcall_AreaId) == 0 )
        {
            // vilslog::printf("AreaRollCallService::checkTagArea() Is exclude_AreaId[%s] TagId[%s] projectid[%d] areaid[%s] areaname[%s]\n",
            //     node->exclude_rollcall_AreaId, node->nodeid, areainfo->projectid, area->areaid.c_str(), area->areaname.c_str());

            setTagInfoArea(areainfo->projectid, area, isuser, nodeid, tafid, AreaId0D, AreaType0D, velocity, voltage, 2);// 2:out

            return 0;
        }

        if ( Opertaion_Mode == 3 )
        {
            // GPS mode
            setTagInfoArea(areainfo->projectid, area, isuser, nodeid, tafid, AreaId0D, AreaType0D, velocity, voltage, 2);// 2:out

            return 0;
        }

        if ( AreaId0D.length() > 0 )
        {
#ifdef PRINT_AREA_ROLLCALL_LOG
            vilslog::printf("AreaRollCallService::checkTagArea() TagId[%s] AreaId0D[%s] area->type[%d]\n", nodeid.c_str(), AreaId0D.c_str(), area->type);
#endif

            // 觸發 0D 區域, 其他區域一律不能進入. ex.飄過區域.
            if ( AreaId0D.compare(area->areaid) != 0 )
            {
                if ( area->type == 5 )
                {
                    // 可以ˋ進入病房區
                    vilslog::printf("AreaRollCallService::checkTagArea() 觸發 0D 區域, 其他區域一律不能進入. 可以進入病房區, TagId[%s] AreaId0D[%s] areaid[%s]\n",
                           nodeid.c_str(), AreaId0D.c_str(), area->areaid.c_str());
                }
                else
                {
#ifdef PRINT_AREA_ROLLCALL_LOG
                    vilslog::printf("AreaRollCallService::checkTagArea() 觸發 0D 區域, 其他區域一律不能進入. TagId[%s] AreaId0D[%s] areaid[%s]\n",
                        nodeid.c_str(), AreaId0D.c_str(), area->areaid.c_str());
#endif

                    return 0;
                }
            }
        }

        setTagInfoArea(areainfo->projectid, area, isuser, nodeid, tafid, AreaId0D, AreaType0D, velocity, voltage, 1);// 1:in

        if (RangingResultType != 0 )
        {
            // 如果是 1D, 2D 定位結果, 取消 exclude_rollcall_AreaId
            // clear RollCallNotInArea info
            mDeviceManager->setTagRollCallNotInArea(areainfo->projectid, nodeid.c_str(), "");
        }
    }
    return 1;
}

int AreaRollCallService::checkTagSleep()
{
    int count = 0;

    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);

        std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);
        for (std::list<NodeStruct*>::iterator node = tagNodeList->begin(); node != tagNodeList->end(); node++)
        {
            time_t t_cur = getCurrentTimeInMilliSec();
            time_t t_diff = t_cur - (*node)->updateTimestamp;
            int check_TIME_OUT = NODE_ALIVE_TIMEOUT;
            if ( (*node)->tafid == 2 )
                check_TIME_OUT = NODE_TYPE_2_ALIVE_TIMEOUT;

            int alive = 0;
            if ( t_diff > 0 && t_diff < check_TIME_OUT )
                alive = 1;

            if ( alive > 0 && (*node)->velocity < 0.1 )
            {
                move_to_sleeping_pool(projectid, (*node)->nodeid, (*node)->tafid);
            }
            else
            if ( alive > 0 )
            {
                move_from_sleeping_pool(projectid, (*node)->nodeid);
            }

        }
    }
    return count;
}

void AreaRollCallService::move_to_sleeping_pool(int projectid, std::string nodeid, int tafid)
{
    bool bfound = false;
    for (std::list<SleepPoolStruct*>::iterator pool = mTagSleepPoolList.begin(); pool != mTagSleepPoolList.end(); pool++)
    {
        if( (*pool)->nodeid.compare(nodeid) == 0 )
        {
            if ((*pool)->slept == 1)
            {
                bfound = true;
                break;
            }

            time_t curtime = getCurrentTimeInMilliSec();
            int tdiff = (int)(curtime - (*pool)->timestamp);

            int check_TIME_OUT = NODE_SLEEP_POOL_TIMEOUT;
            int check_MAX_COUNT = NODE_SLEEP_POOL_COUNT;
            if ( tafid == 2 )
            {
                check_TIME_OUT = NODE_SLEEP_POOL_TYPE_2_TIMEOUT;
                check_MAX_COUNT = NODE_SLEEP_POOL_TYPE_2_COUNT;
            }

            if ( (*pool)->check_count >= check_MAX_COUNT )
            {
                (*pool)->slept = 1;

                mDeviceManager->setTagSleep(projectid, nodeid.c_str(), (*pool)->slept);

                // vilslog::printf("AreaRollCallService::move_to_sleeping_pool() [%s] check_count[%d] >= check_MAX_COUNT[%d] slept\n",
                //     (*pool)->nodeid.c_str(), (*pool)->check_count, check_MAX_COUNT);
            }
            else
            if ( tdiff >= check_TIME_OUT )
            {
                (*pool)->check_count++;
                (*pool)->timestamp = curtime;

                // vilslog::printf("AreaRollCallService::move_to_sleeping_pool() [%s] check_count[%d] tdiff[%d]\n",
                //     (*pool)->nodeid.c_str(), (*pool)->check_count, tdiff);
            }
            // vilslog::printf("AreaRollCallService::move_to_sleeping_pool() [%s] check_count[%d] tdiff[%d]\n",
            //     (*pool)->nodeid.c_str(), (*pool)->check_count, tdiff);

            bfound = true;

            update_cache_SleepPool(projectid, (*pool));

            break;
        }
    }

    if(!bfound)
    {
        SleepPoolStruct * newPool = new SleepPoolStruct();
        newPool->projectid = projectid;
        newPool->nodeid = nodeid;
        newPool->tafid = tafid; // 0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師
        newPool->isuser = 0;
        newPool->check_count = 1;
        newPool->slept = 0;
        newPool->timestamp = getCurrentTimeInMilliSec();
        mTagSleepPoolList.push_back(newPool);

        mDeviceManager->setTagSleep(projectid, nodeid.c_str(), newPool->slept);

        update_cache_SleepPool(projectid, newPool);
        // vilslog::printf("AreaRollCallService::move_to_sleeping_pool() [%s] new pool! \n", newPool->nodeid.c_str());
    }
}

void AreaRollCallService::move_from_sleeping_pool(int projectid, std::string nodeid)
{
    //bool bfound = false;
    for (std::list<SleepPoolStruct*>::iterator pool = mTagSleepPoolList.begin(); pool != mTagSleepPoolList.end(); pool++)
    {
        if( (*pool)->nodeid.compare(nodeid) == 0 )
        {
            (*pool)->slept = 0;
            (*pool)->check_count = 0;
            (*pool)->timestamp = getCurrentTimeInMilliSec();

            mDeviceManager->setTagSleep(projectid, nodeid.c_str(), (*pool)->slept);

            break;
        }
    }
}

bool AreaRollCallService::check_sleep_pool(int projectid, std::string nodeid)
{
    bool bfound = false;

    for (std::list<SleepPoolStruct*>::iterator pool = mTagSleepPoolList.begin(); pool != mTagSleepPoolList.end(); pool++)
    {
        if( (*pool)->nodeid.compare(nodeid) == 0 )
        {
            if ( ((*pool)->slept == 1) || ((*pool)->check_count >= 1) )
            {
                bfound = true;
            }
            break;
        }
    }

    return bfound;
}

bool AreaRollCallService::update_cache_TagAreaBuffer(TagAreaBufferStruct * tagarea, BufferAreaInfoStruct * bufferarea)
{
    bool bfound = false;
    for (std::vector<ProjectAreaInfoStruct*>::iterator areaInfo = mProjectAreaInfoVector.begin(); areaInfo != mProjectAreaInfoVector.end(); areaInfo++)
    {
        if ( (*areaInfo)->projectid == tagarea->projectid )
        {
            (*areaInfo)->m_CacheDB->write_TagAreaBuffer_record(tagarea, bufferarea);
            bfound = true;
            break;
        }
    }

    if (!bfound)
    {
        vilslog::printf("AreaRollCallService::update_cache_TagAreaBuffer() not found!\n");
    }

    return bfound;
}

bool AreaRollCallService::remove_cache_TagAreaBuffer(TagAreaBufferStruct * tagarea, BufferAreaInfoStruct * bufferarea)
{
    bool bfound = false;
    for (std::vector<ProjectAreaInfoStruct*>::iterator areaInfo = mProjectAreaInfoVector.begin(); areaInfo != mProjectAreaInfoVector.end(); areaInfo++)
    {
        if ((*areaInfo)->projectid == tagarea->projectid )
        {
            (*areaInfo)->m_CacheDB->remove_TagAreaBuffer_record(tagarea, bufferarea);
            bfound = true;
            break;
        }
    }
    return bfound;
}

bool AreaRollCallService::update_cache_SleepPool(int projectid, SleepPoolStruct * newPool)
{
    bool bfound = false;
    for (std::vector<ProjectAreaInfoStruct*>::iterator areaInfo = mProjectAreaInfoVector.begin(); areaInfo != mProjectAreaInfoVector.end(); areaInfo++)
    {
        if ((*areaInfo)->projectid == projectid )
        {
            (*areaInfo)->m_CacheDB->write_SleepPool_record(newPool);
            bfound = true;
            break;
        }
    }

    if (!bfound)
    {
        vilslog::printf("AreaRollCallService::update_cache_SleepPool() not found!\n");
    }

    return bfound;
}

bool AreaRollCallService::remove_cache_TagAreaBuffer(int projectid, TagAreaBufferStruct * tagarea)
{
    bool bfound = false;
    for (std::vector<ProjectAreaInfoStruct*>::iterator areaInfo = mProjectAreaInfoVector.begin(); areaInfo != mProjectAreaInfoVector.end(); areaInfo++)
    {
        if ((*areaInfo)->projectid == projectid )
        {
            (*areaInfo)->m_CacheDB->deleteTagAreaBuffer(tagarea);
            bfound = true;
            break;
        }
    }

    if (!bfound)
    {
        vilslog::printf("AreaRollCallService::remove_cache_TagAreaBuffer() not found!\n");
    }

    return bfound;
}

bool AreaRollCallService::remove_cache_SleepPool(int projectid, SleepPoolStruct * pool)
{
    bool bfound = false;
    for (std::vector<ProjectAreaInfoStruct*>::iterator areaInfo = mProjectAreaInfoVector.begin(); areaInfo != mProjectAreaInfoVector.end(); areaInfo++)
    {
        if ((*areaInfo)->projectid == projectid )
        {
            (*areaInfo)->m_CacheDB->deleteTagSleepPool(pool);
            bfound = true;
            break;
        }
    }

    if (!bfound)
    {
        vilslog::printf("AreaRollCallService::remove_cache_SleepPool() not found!\n");
    }

    return bfound;
}

bool AreaRollCallService::save_rollcall(int projectid, std::string nodeid, std::string areaid, int diffMS)
{
    std::string today = TodayDateType1();
    std::string time = currentTimeForSQL();

    AreaRollCallRecordStruct * curRollCall = NULL;
    for (std::list<AreaRollCallRecordStruct*>::iterator rollcall = mRollCallRecordList.begin(); rollcall != mRollCallRecordList.end(); rollcall++)
    {
        if ( ((*rollcall)->projectid == projectid) &&
             ((*rollcall)->nodeid.compare(nodeid) == 0) &&
             ((*rollcall)->areaid.compare(areaid) == 0) )
        {
            curRollCall = (*rollcall);
            break;
        }
    }
    if (curRollCall == NULL)
    {
        AreaRollCallRecordStruct * areaRollCall = new AreaRollCallRecordStruct();
        areaRollCall->projectid = projectid;
        areaRollCall->areaid = areaid;
        areaRollCall->nodeid = nodeid;
        areaRollCall->issueday = today; // foramt: YYYY-MM-DD
        areaRollCall->issuetime = time; // foramt: HH:mm
        areaRollCall->totaltime = 0; // foramt: ss how many seconds stay in this minute at this area
        areaRollCall->totaltimeInMS = 0;

        mRollCallRecordList.push_back(areaRollCall);

        curRollCall = areaRollCall;
    }

    curRollCall->totaltimeInMS += diffMS;
    curRollCall->totaltime = (int)(curRollCall->totaltimeInMS / 1000);
    if ((curRollCall->totaltimeInMS % 1000) > 0)
    {
        curRollCall->totaltime += 1;
    }
    // if ( nodeid.compare("1166") == 0 || nodeid.compare("1166") == 0)
    // {
        // vilslog::printf("AreaRollCallService::save_rollcall() nodeid[%s] areaid[%s] curRollCall->areaid[%s] time[%d] curRollCall->issuetime[%d]\n",
        //     nodeid.c_str(), areaid.c_str(), curRollCall->areaid.c_str(), time.c_str(), curRollCall->issuetime.c_str());
        // vilslog::printf("AreaRollCallService::save_rollcall() totaltime[%d] totaltimeInMS[%d]\n",
        //     curRollCall->totaltime, curRollCall->totaltimeInMS);
    //
    // }

    if ( time.compare(curRollCall->issuetime) != 0 )
    {
        if (curRollCall->totaltime > 60)
            curRollCall->totaltime = 60;

        curRollCall->datetime = currentDateTimeForSQL();
        WorkItem * writem = new WorkItem("arearollcallrecord", projectid, curRollCall);
        mTrackDBqueue.add(writem);

        // vilslog::printf("AreaRollCallService::save_rollcall() nodeid[%s] areaid[%s] issueday[%s] issuetime[%s] totaltime[%d] totaltimeInMS[%d]\n",
        //                nodeid.c_str(), curRollCall->areaid.c_str(), curRollCall->issueday.c_str(), curRollCall->issuetime.c_str(), curRollCall->totaltime, curRollCall->totaltimeInMS);

        curRollCall->issueday = today;
        curRollCall->issuetime = time;
        curRollCall->totaltimeInMS = 0;
        curRollCall->totaltime = 0;
    }

    return true;
}

void AreaRollCallService::setTagInfoArea(int projectid, MapLayerAreaStruct * area, int isuser, std::string nodeid, int tafid, std::string AreaId0D, int AreaType0D, double velocity, std::string voltage, int action)
{
    bool bFound = false;
    for (std::list<TagAreaBufferStruct*>::iterator buffer = mTagAreaBufferList.begin(); buffer != mTagAreaBufferList.end(); buffer++)
    {
        if ( (*buffer)->projectid == projectid && ((*buffer)->nodeid.compare(nodeid) == 0) )
        {
            (*buffer)->tafid = tafid;

            setTagInfoArea((*buffer), area, AreaId0D, AreaType0D, velocity, voltage, action);
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        TagAreaBufferStruct * newBuffer = new TagAreaBufferStruct();
        newBuffer->projectid = projectid;
        newBuffer->nodeid = nodeid;
        newBuffer->tafid = tafid;
        newBuffer->isuser = isuser;
        mTagAreaBufferList.push_back(newBuffer);

        setTagInfoArea(newBuffer, area, AreaId0D, AreaType0D, velocity, voltage, action);
    }
}

void AreaRollCallService::setTagInfoArea(TagAreaBufferStruct * tagbuffer, MapLayerAreaStruct * area, std::string AreaId0D, int AreaType0D, double velocity, std::string Voltage, int action)
{

#ifdef PRINT_AREA_ROLLCALL_LOG
    if (area != NULL)
        vilslog::printf("AreaRollCallService::setTagInfoArea() [%s] action[%d] area->type[%d]\n", tagbuffer->nodeid.c_str(), action, area->type);
#endif

    double voltage = 0.0;
    if (Voltage.length() > 0)
        voltage = std::stod(Voltage);

    if ( tagbuffer->mBufferAreaList.size() == 0 )
    {
        if ( action == 0 || action == 2 )
        {
#ifdef PRINT_AREA_ROLLCALL_LOG
            if(area == NULL)
            {
                vilslog::printf("AreaRollCallService::setTagInfoArea() [%s][%d] area == NULL No In Action!!! \n",
                    tagbuffer->nodeid.c_str(), action);
            }
            else
            {
                vilslog::printf("AreaRollCallService::setTagInfoArea() [%s][%d][%s] No In Action!!! \n",
                    tagbuffer->nodeid.c_str(), action, area->areaname.c_str());
            }
#endif
        }
        else
        if (area != NULL)
        {

            // create new buffer
            BufferAreaInfoStruct * newBuffer = new BufferAreaInfoStruct();
            //newBuffer->action = action; // 1:in 2:out
            if ( area->areaid.compare(AreaId0D) == 0 )
                newBuffer->AreaType0D = AreaType0D;
            else
                newBuffer->AreaType0D = 0;
            newBuffer->MapAreaType = area->type;
            newBuffer->areaid = area->areaid;
            newBuffer->areaname = area->areaname;

            // in action
            newBuffer->checked_in = false;
            newBuffer->velocity_in = velocity;
            newBuffer->voltage_in = voltage;
            newBuffer->timestamp_in = getCurrentTimeInMilliSec();

            // out action
            newBuffer->checked_out = false;
            newBuffer->velocity_out = velocity;
            newBuffer->voltage_out = voltage;
            newBuffer->timestamp_out = 0;

            tagbuffer->mBufferAreaList.push_back(newBuffer);

            update_cache_TagAreaBuffer(tagbuffer, newBuffer);
        }
        return;
    }

#ifdef PRINT_AREA_ROLLCALL_LOG
    vilslog::printf("AreaRollCallService::setTagInfoArea() nodeid[%s] mBufferAreaList.size[%d]\n",
        tagbuffer->nodeid.c_str(), (int)tagbuffer->mBufferAreaList.size());
#endif

    // check last area info
    BufferAreaInfoStruct * lastBuffer = tagbuffer->mBufferAreaList.back();

    // vilslog::printf("AreaRollCallService::setTagInfoArea() nodeid[%s] mBufferAreaList.size[%d] projectid[%d]\n",
    //     tagbuffer->nodeid.c_str(), (int)tagbuffer->mBufferAreaList.size(), lastBuffer->projectid);

    // check if same area , same action
    if ( (action != 0) && (lastBuffer->areaid.compare(area->areaid) == 0) )
    {
#ifdef PRINT_AREA_ROLLCALL_LOG
        vilslog::printf("AreaRollCallService::setTagInfoArea() [%s][%d][%s] checked_in[%d] checked_out[%d] timestamp_in[%d] timestamp_out[%d]\n",
            tagbuffer->nodeid.c_str(), action, area->areaname.c_str(), lastBuffer->checked_in, lastBuffer->checked_out, (int)lastBuffer->timestamp_in, (int)lastBuffer->timestamp_out);
#endif
        bool bCreateNew = false;
        if ( action == 1 ) // in
        {
            if ( lastBuffer->checked_in && lastBuffer->checked_out )
            {
                // 已經 完成 進出, 新增進入紀錄
                bCreateNew = true;
#ifdef PRINT_AREA_ROLLCALL_LOG
                vilslog::printf("AreaRollCallService::setTagInfoArea() 已經 完成 進出, 新增進入紀錄\n");
#endif
            }
            else
            if ( lastBuffer->checked_in )
            {
                // 已經 進入, 尚未 出去
                // 修改記錄, 標示 尚未 出去,
                lastBuffer->timestamp_out = 0;
                update_cache_TagAreaBuffer(tagbuffer, lastBuffer);

#ifdef PRINT_AREA_ROLLCALL_LOG
                vilslog::printf("AreaRollCallService::setTagInfoArea() 已經 進入, 尚未 出去, 修改記錄, 標示 尚未 出去.\n");
#endif
            }
            else
            if ( !lastBuffer->checked_in )
            {
                //lastBuffer->timestamp_in = getCurrentTimeInMilliSec();
                //update_cache_TagAreaBuffer(tagbuffer, lastBuffer);
            }
        }
        else
        if ( action == 2 ) // out
        {
            if ( lastBuffer->checked_in && lastBuffer->checked_out )
            {
                // 已經 完成 進出, do nothing
#ifdef PRINT_AREA_ROLLCALL_LOG
                vilslog::printf("AreaRollCallService::setTagInfoArea() 已經 完成 進出, do nothing\n");
#endif
                return;
            }
            else
            if ( lastBuffer->checked_in )
            {
                // 已經 進入, 尚未 出去
                // 標示 已 出去,
                if (lastBuffer->timestamp_out == 0)
                {
                    lastBuffer->timestamp_out = getCurrentTimeInMilliSec();
                    update_cache_TagAreaBuffer(tagbuffer, lastBuffer);
                }
#ifdef PRINT_AREA_ROLLCALL_LOG
                vilslog::printf("AreaRollCallService::setTagInfoArea() 已經 進入, 尚未 出去, 標示 已 出去.\n");
#endif
            }
        }


        if (bCreateNew)
        {
#ifdef PRINT_AREA_ROLLCALL_LOG
            vilslog::printf("AreaRollCallService::setTagInfoArea() create new buffer\n");
#endif
            // create new buffer
            BufferAreaInfoStruct * newBuffer = new BufferAreaInfoStruct();
            if ( area->areaid.compare(AreaId0D) == 0 )
                newBuffer->AreaType0D = AreaType0D;
            else
                newBuffer->AreaType0D = 0;
            newBuffer->MapAreaType = area->type;
            newBuffer->areaid = area->areaid;
            newBuffer->areaname = area->areaname;

            // in action
            newBuffer->checked_in = false;
            newBuffer->velocity_in = velocity;
            newBuffer->voltage_in = voltage;
            newBuffer->timestamp_in = getCurrentTimeInMilliSec();

            // out action
            newBuffer->checked_out = false;
            newBuffer->velocity_out = velocity;
            newBuffer->voltage_out = voltage;
            newBuffer->timestamp_out = 0;

            tagbuffer->mBufferAreaList.push_back(newBuffer);

            update_cache_TagAreaBuffer(tagbuffer, newBuffer);
        }
    }
    else
    {
        //if ( lastBuffer->action == 1 ) // 1:in
        if ( lastBuffer->timestamp_out == 0 ) // 前一個Area尚未離開
        {
            // 檢查是不是在病房 5:病房 6:病床
            if ( (lastBuffer->MapAreaType == 5) && (action != 0) )
            {
                if ( area->type == 6 )
                {
                    if ( action == 1 ) // 1:in
                        vilslog::printf("AreaRollCallService::setTagInfoArea() 已經在病房, 要進入病床!\n");
                    else
                        vilslog::printf("AreaRollCallService::setTagInfoArea() 已經在病房, 要離開病床!\n");
                }
                else
                {
                    // 讓前一個Area先out
                    lastBuffer->timestamp_out = getCurrentTimeInMilliSec();
                }
            }
            else
            {
                // 讓前一個Area先out
                lastBuffer->timestamp_out = getCurrentTimeInMilliSec();
            }
            update_cache_TagAreaBuffer(tagbuffer, lastBuffer);
        }

        if ( action == 0 ) // 0:no action enter no area
        {
            // do nothing
        }
        else
        if ( action == 1 ) // 1:in
        {
            //
            // 檢查 buffer 裡面是否 有同一個 area, 可能是病房
            //
            bool bfound = false;
            for (std::list<BufferAreaInfoStruct*>::iterator buffer = tagbuffer->mBufferAreaList.begin(); buffer != tagbuffer->mBufferAreaList.end(); buffer++)
            {
                if ( (*buffer)->areaid.compare(area->areaid) == 0 )
                {
#ifdef PRINT_AREA_ROLLCALL_LOG
                    vilslog::printf("AreaRollCallService::setTagInfoArea() 檢查buffer裡面有同一個 area, 可能是病房!\n");
#endif
                    bfound = true;
                    break;
                }
            }
            if (!bfound)
            {
                BufferAreaInfoStruct * newBuffer = new BufferAreaInfoStruct();
                if ( area->areaid.compare(AreaId0D) == 0 )
                    newBuffer->AreaType0D = AreaType0D;
                else
                    newBuffer->AreaType0D = 0;
                newBuffer->MapAreaType = area->type;
                newBuffer->areaid = area->areaid;
                newBuffer->areaname = area->areaname;

                // in action
                newBuffer->checked_in = false;
                newBuffer->velocity_in = velocity;
                newBuffer->voltage_in = voltage;
                newBuffer->timestamp_in = getCurrentTimeInMilliSec();

                // out action
                newBuffer->checked_out = false;
                newBuffer->velocity_out = velocity;
                newBuffer->voltage_out = voltage;
                newBuffer->timestamp_out = 0;

                tagbuffer->mBufferAreaList.push_back(newBuffer);

                update_cache_TagAreaBuffer(tagbuffer, newBuffer);
            }
        }
        else
        if ( action == 2 ) // 2:out
        {
            // 沒有 in action
#ifdef PRINT_AREA_ROLLCALL_LOG
            vilslog::printf("AreaRollCallService::setTagInfoArea() [%s][%d][%s] No In Action!!!\n",
                tagbuffer->nodeid.c_str(), action, area->areaid.c_str());
#endif
        }
    }
}

void AreaRollCallService::checkTagAreaBuffer(int diffMS)
{
    for (std::list<TagAreaBufferStruct*>::iterator buffer = mTagAreaBufferList.begin(); buffer != mTagAreaBufferList.end(); buffer++)
    {
        bool bFinished = checkTagAreaBuffer((*buffer), diffMS);
        while(!bFinished)
        {
            bFinished = checkTagAreaBuffer((*buffer), diffMS);
        }
    }
}

bool AreaRollCallService::checkTagAreaBuffer(TagAreaBufferStruct* tagbuffer, int diffMS)
{
    bool bFinished = true;

#ifdef PRINT_AREA_ROLLCALL_LOG
    if ( tagbuffer->nodeid.compare("1166") == 0 || tagbuffer->nodeid.compare("1166") == 0)
    {
        vilslog::printf("\nAreaRollCallService::checkTagAreaBuffer() nodeid[%s] mBufferAreaList.size[%d]\n",
            tagbuffer->nodeid.c_str(), (int)tagbuffer->mBufferAreaList.size());
    }
#endif
    for (std::list<BufferAreaInfoStruct*>::iterator buffer = tagbuffer->mBufferAreaList.begin(); buffer != tagbuffer->mBufferAreaList.end(); buffer++)
    {
#ifdef PRINT_AREA_ROLLCALL_LOG
        if ( tagbuffer->nodeid.compare("1166") == 0 || tagbuffer->nodeid.compare("1166") == 0)
        {
            vilslog::printf("AreaRollCallService::checkTagAreaBuffer() nodeid[%s] areaname[%s] checked_in[%d] timestamp_in[%d] checked_out[%d] timestamp_out[%d]\n",
                tagbuffer->nodeid.c_str(), (*buffer)->areaname.c_str(), (*buffer)->checked_in, (*buffer)->timestamp_in, (*buffer)->checked_out, (*buffer)->timestamp_out);
        }
#endif
        bool bHasAction = false;
        if ( !(*buffer)->checked_in )
        {

#ifdef PRINT_AREA_ROLLCALL_LOG
            if ( tagbuffer->nodeid.compare("1157") == 0 )
            {
                vilslog::printf("AreaRollCallService::checkTagAreaBuffer() nodeid[%s] areaname[%s] checked_in[%d] checked_out[%d]\n",
                    tagbuffer->nodeid.c_str(), (*buffer)->areaname.c_str(), (*buffer)->checked_in, (*buffer)->checked_out);
            }
#endif

            //
            // 判斷是否 進入 區域
            //
            int actionType = check_enter_area(tagbuffer, (*buffer));
            if (actionType == 1)
                bHasAction = true;
            else
            if (actionType == 2)
            {
                break;
            }
        }
        else
        if ( !(*buffer)->checked_out )
        {
#ifdef PRINT_AREA_ROLLCALL_LOG
            if ( tagbuffer->nodeid.compare("1157") == 0 || tagbuffer->nodeid.compare("1166") == 0)
            {
                vilslog::printf("AreaRollCallService::checkTagAreaBuffer() nodeid[%s] areaname[%s] checked_in[%d] checked_out[%d]\n",
                    tagbuffer->nodeid.c_str(), (*buffer)->areaname.c_str(), (*buffer)->checked_in, (*buffer)->checked_out);
            }
#endif

            //
            // Save rollcall
            //
            save_rollcall(tagbuffer->projectid, tagbuffer->nodeid, (*buffer)->areaid, diffMS);


            //
            // 判斷是否 離開 區域
            //
            int actionType = check_leave_area(tagbuffer, (*buffer));
            if (actionType == 1)
                bHasAction = true;
            else
            if (actionType == 2)
            {
                break;
            }
        }

        // if ( tagbuffer->nodeid.compare("1166") == 0 || tagbuffer->nodeid.compare("1166") == 0)
        // {
        //     vilslog::printf("AreaRollCallService::checkTagAreaBuffer() nodeid[%s] areaname[%s] bHasAction[%d]\n",
        //         tagbuffer->nodeid.c_str(), (*buffer)->areaname.c_str(), bHasAction);
        // }


        if ( bHasAction )
        {
            //
            // output action
            //
            if ( !(*buffer)->checked_in )
            {
#ifdef PRINT_AREA_ROLLCALL_LOG
                if ( tagbuffer->nodeid.compare("1157") == 0 )
                {
                    vilslog::printf("\nAreaRollCallService::checkTagAreaBuffer() nodeid[%s][in][%s] MapAreaType[%d] size[%d]\n",
                        tagbuffer->nodeid.c_str(), (*buffer)->areaname.c_str(), (*buffer)->MapAreaType, (int)tagbuffer->mBufferAreaList.size());
                }
#endif
                if (tagbuffer->isuser == 1)
                {
                    int userid = std::stoi(tagbuffer->nodeid);
                    mDeviceManager->setUserInfoArea(tagbuffer->projectid, userid, (*buffer)->areaid, (*buffer)->areaname);

                    if ( (*buffer)->MapAreaType == 5 )
                    {
                        mDeviceManager->setUserInfoAreaType5(tagbuffer->projectid, userid, (*buffer)->areaid, (*buffer)->areaname);
                    }
                }
                else
                {
                    mDeviceManager->setTagInfoArea(tagbuffer->projectid, tagbuffer->nodeid.c_str(), (*buffer)->areaid.c_str(), (*buffer)->areaname.c_str());

                    if ( (*buffer)->MapAreaType == 5 )
                    {
                        mDeviceManager->setTagInfoAreaType5(tagbuffer->projectid, tagbuffer->nodeid.c_str(), (*buffer)->areaid.c_str(), (*buffer)->areaname.c_str());
                    }
                }

                set_tag_area_action(tagbuffer->projectid, tagbuffer->isuser, tagbuffer->nodeid, 1, (*buffer)->areaid, (*buffer)->areaname, (*buffer)->AreaType0D, (*buffer)->MapAreaType);

                (*buffer)->checked_in = true;
                update_cache_TagAreaBuffer(tagbuffer, (*buffer));
            }
            else
            if ( !(*buffer)->checked_out )
            {
#ifdef PRINT_AREA_ROLLCALL_LOG
                if ( tagbuffer->nodeid.compare("1157") == 0 )
                {
                    vilslog::printf("\nAreaRollCallService::checkTagAreaBuffer() nodeid[%s][out][%s] MapAreaType[%d] size[%d]\n",
                        tagbuffer->nodeid.c_str(), (*buffer)->areaname.c_str(), (*buffer)->MapAreaType, (int)tagbuffer->mBufferAreaList.size());
                }
#endif

                if (tagbuffer->isuser == 1)
                {
                    int userid = std::stoi(tagbuffer->nodeid);
                    mDeviceManager->setUserInfoArea(tagbuffer->projectid, userid, "", "");

                    if ( (*buffer)->MapAreaType == 5 )
                    {
                        mDeviceManager->setUserInfoAreaType5(tagbuffer->projectid, userid, "", "");
                    }
                }
                else
                {
                    mDeviceManager->setTagInfoArea(tagbuffer->projectid, tagbuffer->nodeid.c_str(), "", "");

                    if ( (*buffer)->MapAreaType == 5 )
                    {
                        mDeviceManager->setTagInfoAreaType5(tagbuffer->projectid, tagbuffer->nodeid.c_str(), "", "");
                    }
                }
                set_tag_area_action(tagbuffer->projectid, tagbuffer->isuser, tagbuffer->nodeid, 2, (*buffer)->areaid, (*buffer)->areaname, (*buffer)->AreaType0D, (*buffer)->MapAreaType);

                (*buffer)->checked_out = true;
                (*buffer)->timestamp_out = getCurrentTimeInMilliSec();
                update_cache_TagAreaBuffer(tagbuffer, (*buffer));
            }

            if ( (int)tagbuffer->mBufferAreaList.size() <= 1 )
            {
                bFinished = true;
            }
            else
            if ( (*buffer)->checked_in && (*buffer)->checked_out)
            {
#ifdef PRINT_AREA_ROLLCALL_LOG
                if ( tagbuffer->nodeid.compare("1157") == 0 )
                {
                    vilslog::printf("AreaRollCallService::checkTagAreaBuffer() nodeid[%s][%s] diffMS[%d] MapAreaType[%d] mBufferAreaList.size[%d] remove\n",
                        tagbuffer->nodeid.c_str(), (*buffer)->areaname.c_str(), diffMS, (*buffer)->MapAreaType, (int)tagbuffer->mBufferAreaList.size());
                }
#endif
                remove_cache_TagAreaBuffer(tagbuffer, (*buffer));

                // remove old data
                BufferAreaInfoStruct * old_buf = (*buffer);
                tagbuffer->mBufferAreaList.remove(old_buf);
                delete old_buf;

                bFinished = false;
            }
            if ( !bFinished )
                break;
        }
    }
    return bFinished;
}

int AreaRollCallService::check_enter_area(TagAreaBufferStruct * tagbuffer, BufferAreaInfoStruct * bufferArea)
{
    int actionType = 0;
    time_t current_timestamp = getCurrentTimeInMilliSec();

    double velocity = bufferArea->velocity_in;
    int t_ms_diff = (int)(current_timestamp - bufferArea->timestamp_in);

    if (t_ms_diff >= AREA_INOUT_BUFFER_TIMEOUT)
    {
        // if velocity < 0.1, maybe just drift
        if ( velocity <= 0.1 )
        {
            if ( t_ms_diff < AREA_INOUT_BUFFER_NOT_MOVING_TIMEOUT )
            {
                // wait AREA_INOUT_BUFFER_NOT_MOVING_TIMEOUT seconds
                actionType = 2;
            }
            else
            {
                actionType = 1;
            }
        }
        else
        {
            actionType = 1;
        }
    }

    return actionType;
}

int AreaRollCallService::check_leave_area(TagAreaBufferStruct * tagbuffer, BufferAreaInfoStruct * bufferArea)
{
    int actionType = 0;
    time_t current_timestamp = getCurrentTimeInMilliSec();

    int t_ms_diff = (int)(current_timestamp - bufferArea->timestamp_out);

    if (bufferArea->timestamp_out != 0)
    {
        if ( t_ms_diff >= AREA_INOUT_BUFFER_TIMEOUT )
        {
            // if velocity < 0.1, maybe just drift
            double velocity = bufferArea->velocity_out;
            if ( velocity <= 0.1 )
            {
                if ( t_ms_diff < AREA_INOUT_BUFFER_NOT_MOVING_TIMEOUT )
                {
                    // wait 5 seconds
                    actionType = 2;
                }
                else
                {
                    actionType = 1;
                }
            }
            else
            {
                actionType = 1;
            }
        }
    }
    else
    {
        // check alive time
        NodeStruct * node = mDeviceManager->get_Tag_node(tagbuffer->projectid, tagbuffer->nodeid.c_str());
        if ( node == NULL )
        {
            return actionType;
        }
        time_t t_cur = getCurrentTimeInMilliSec();
        time_t t_diff = t_cur - node->updateTimestamp;
        int i_diff = (int)t_diff;
        int target_timeout = NODE_ALIVE_TIMEOUT;

        bool bInSleepPool = check_sleep_pool(tagbuffer->projectid, tagbuffer->nodeid);

        if ( tagbuffer->tafid == 2 ) // 2:設備
        {
            target_timeout = NODE_TYPE_2_ALIVE_TIMEOUT;

            if (i_diff >= target_timeout)
            {
                if (bInSleepPool)
                {
                    // 可能是進入休眠, 不做離開區域.
                    actionType = 0;
                }
                else
                {
                    actionType = 1;

                    // vilslog::printf("AreaRollCallService::check_leave_area() nodeid[%s] tafid[%d] i_diff[%d] > [%d], 設備no alive 離開區域.\n",
                    //     tagbuffer->nodeid.c_str(), tagbuffer->tafid, i_diff, target_timeout);
                }
            }
        }
        else
        {
            // tafid  0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師
            if (i_diff >= target_timeout)
            {
                if (bInSleepPool)
                {
                    // 可能是進入休眠, 不做離開區域.
                    actionType = 0;
                    // vilslog::printf("AreaRollCallService::check_leave_area() nodeid[%s] tafid[%d] voltage[%s] 可能是進入休眠, 不做離開區域.\n",
                    //     tagbuffer->nodeid.c_str(), tagbuffer->tafid, node->voltage);
                }
                else
                {
                    //
                    // 判斷電壓, 是否是沒電.
                    //
                    double voltage = 0.0;
                    if (strlen(node->voltage) > 0)
                        voltage = std::stod(node->voltage);
                    if ( voltage <= 3.2 )
                    {
                        // 判斷 沒電, 不做離開區域.
                        actionType = 0;
                        // vilslog::printf("AreaRollCallService::check_leave_area() nodeid[%s] tafid[%d] voltage[%s] 判斷 沒電, 不做離開區域.\n",
                        //     tagbuffer->nodeid.c_str(), tagbuffer->tafid, node->voltage);
                    }
                    else
                    {
                        // 最後可能是移動離開
                        actionType = 1;
                        // vilslog::printf("AreaRollCallService::check_leave_area() nodeid[%s] tafid[%d] 最後可能是移動離開.\n",
                        //     tagbuffer->nodeid.c_str(), tagbuffer->tafid);
                    }
                }

/*
                //
                // 判斷移動, 是否是移動離開
                //
                double velocity = node->velocity;
                if ( velocity > 0.1 )
                {
                    // 最後可能是移動離開
                    actionType = 1;

                    // vilslog::printf("AreaRollCallService::check_leave_area() nodeid[%s] tafid[%d] velocity[%.01f] 最後可能是移動離開.\n",
                    //     tagbuffer->nodeid.c_str(), tagbuffer->tafid, velocity);
                }
                else
                {
                    //
                    // 判斷電壓, 是否是沒電.
                    //
                    double voltage = std::stod(node->voltage);
                    if ( voltage <= 3.2 )
                    {
                        // 判斷 沒電, 不做離開區域.
                        actionType = 0;

                        // vilslog::printf("AreaRollCallService::check_leave_area() nodeid[%s] tafid[%d] velocity[%.01f] voltage[%s] 判斷 沒電, 不做離開區域.\n",
                        //     tagbuffer->nodeid.c_str(), tagbuffer->tafid, velocity, node->voltage);
                    }
                    else
                    {
                        // 可能是進入休眠, 不做離開區域.
                        actionType = 0;

                        // vilslog::printf("AreaRollCallService::check_leave_area() nodeid[%s] tafid[%d] velocity[%.01f] voltage[%s] 可能是進入休眠, 不做離開區域.\n",
                        //     tagbuffer->nodeid.c_str(), tagbuffer->tafid, velocity, node->voltage);
                    }
                }
*/
            }
        }
    }

    return actionType;
}

bool AreaRollCallService::set_tag_area_action(int projectid, int isuser, std::string nodeid, int action, std::string areaid, std::string areaname, int AreaType0D, int MapAreaType)
{
    int tafid = 0;
    std::string nodename;
    std::string macaddress;

    if (isuser == 1)
    {
        int userid = std::stoi(nodeid);

        UserStruct * user = mDeviceManager->getUser(projectid, userid);
        if ( user == NULL )
        {
            return false;
        }
        nodename = user->name;
        macaddress = user->account;

        // vilslog::printf("AreaRollCallService::set_tag_area_action() userid[%d][%d][%s] mUpdateAreaBuffer[%d]\n",
        //     userid, action, areaname.c_str(), mUpdateAreaBuffer);
    }
    else
    {
        NodeStruct * node = mDeviceManager->get_Tag_node(projectid, nodeid.c_str());
        if ( node == NULL )
        {
            return false;
        }
        tafid = node->tafid;
        nodename = node->nodename;
        macaddress = node->macaddress;

        // vilslog::printf("AreaRollCallService::set_tag_area_action() nodeid[%s][%d][%s] mUpdateAreaBuffer[%d]\n",
        //     nodeid.c_str(), action, areaname.c_str(), mUpdateAreaBuffer);
    }

    if (mUpdateAreaBuffer)
    {
        return true;
    }

    AreaInOutEventStruct * newEvent = new AreaInOutEventStruct();
    newEvent->projectid = projectid; // area projectid
    newEvent->action = action; //1:in 2:out
    newEvent->AreaType0D = AreaType0D; // 0:normal 1:room 2:danger
    newEvent->MapAreaType = MapAreaType;// 0:普通 1:電子圍欄 2:電子點名 3:房間 4:教室 5:病房 6:病床 999:虛擬牆
    newEvent->tafid = tafid; // 0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師
    newEvent->NodeId = nodeid;
    newEvent->NodeName = nodename;
    newEvent->NodeMacaddress = macaddress;
    newEvent->AreaID = areaid;
    newEvent->AreaName = areaname;
    newEvent->DataTime = currentDateTimeForSQL();

    mAreaInOutEventQueue.add(newEvent);

    AreaInOutEventStruct * newEventCourse = new AreaInOutEventStruct();
    newEventCourse->projectid = projectid; // area projectid
    newEventCourse->action = action; //1:in 2:out
    newEventCourse->AreaType0D = AreaType0D; // 0:normal 1:room 2:danger
    newEventCourse->MapAreaType = MapAreaType;// 0:普通 1:電子圍欄 2:電子點名 3:房間 4:教室 5:病房 6:病床 999:虛擬牆
    newEventCourse->tafid = tafid; // 0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師
    newEventCourse->NodeId = nodeid;
    newEventCourse->NodeName = nodename;
    newEventCourse->NodeMacaddress = macaddress;
    newEventCourse->AreaID = areaid;
    newEventCourse->AreaName = areaname;
    newEventCourse->DataTime = currentDateTimeForSQL();

    mCourseAreaInOutEventQueue.add(newEventCourse);


    AreaInOutEventStruct * newWebSocketEvent = new AreaInOutEventStruct();
    newWebSocketEvent->projectid = projectid; // area projectid
    newWebSocketEvent->action = action; //1:in 2:out
    newWebSocketEvent->AreaType0D = AreaType0D; // 0:normal 1:room 2:danger
    newWebSocketEvent->MapAreaType = MapAreaType;// 0:普通 1:電子圍欄 2:電子點名 3:房間 4:教室 5:病房 6:病床 999:虛擬牆
    newWebSocketEvent->tafid = tafid; // 0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師
    newWebSocketEvent->NodeId = nodeid;
    newWebSocketEvent->NodeName = nodename;
    newWebSocketEvent->NodeMacaddress = macaddress;
    newWebSocketEvent->AreaID = areaid;
    newWebSocketEvent->AreaName = areaname;
    newWebSocketEvent->DataTime = currentDateTimeForSQL();

    mWebSocketAreaInOutEventQueue.add(newWebSocketEvent);


    return true;
}

void AreaRollCallService::clear_TagAreaBufferStruct_list(std::list<TagAreaBufferStruct*> * plist)
{
    for (std::list<TagAreaBufferStruct*>::iterator it = plist->begin(); it != plist->end(); it++)
    {
        delete (*it);
    }
    plist->clear();
}

void AreaRollCallService::clear_SleepPoolStruct_list(std::list<SleepPoolStruct*> * plist)
{
    for (std::list<SleepPoolStruct*>::iterator it = plist->begin(); it != plist->end(); it++)
    {
        delete (*it);
    }
    plist->clear();
}

void AreaRollCallService::print_TagAreaBuffer()
{
    vilslog::printf("====================================================\n");
    vilslog::printf("AreaRollCallService::print_TagAreaBuffer() mTagAreaBufferList.size()[%d]\n", (int)mTagAreaBufferList.size());

    for (std::list<TagAreaBufferStruct*>::iterator buffer = mTagAreaBufferList.begin(); buffer != mTagAreaBufferList.end(); buffer++)
    {
        vilslog::printf("\nprint_TagAreaBuffer() projectid[%d] nodeid[%s] (*buffer)->mBufferAreaList.size()[%d]\n",
            (*buffer)->projectid, (*buffer)->nodeid.c_str(), (int)(*buffer)->mBufferAreaList.size());

        for (std::list<BufferAreaInfoStruct*>::iterator bufarea = (*buffer)->mBufferAreaList.begin(); bufarea != (*buffer)->mBufferAreaList.end(); bufarea++)
        {
            vilslog::printf("print_TagAreaBuffer() (*bufarea)->areaid[%s][%s] AreaType0D[%d] MapAreaType[%d]\n",
                (*bufarea)->areaid.c_str(), (*bufarea)->areaname.c_str(), (*bufarea)->AreaType0D, (*bufarea)->MapAreaType);

            vilslog::printf("print_TagAreaBuffer() checked_in[%d] velocity_in[%f] voltage_in[%f] timestamp_in[%s]\n",
                (*bufarea)->checked_in, (*bufarea)->velocity_in, (*bufarea)->voltage_in, time_tToString((*bufarea)->timestamp_in/1000).c_str());

            vilslog::printf("print_TagAreaBuffer() checked_out[%d] velocity_out[%f] voltage_out[%f] timestamp_out[%s]\n",
                (*bufarea)->checked_out, (*bufarea)->velocity_out, (*bufarea)->voltage_out, time_tToString((*bufarea)->timestamp_out/1000).c_str());
        }

    }
    vilslog::printf("====================================================\n");
}
