/*
 *  AnalyzeTrackService.cpp
 *
 *  Created on: 2019/04/17
 *      Author: ikki
 */

#include <string.h>
#include <ctime>
#include <math.h>
#include "AnalyzeTrackService.h"
#include "../common.h"
#include "../mysql/struct.h"

using namespace std;

AnalyzeTrackService::AnalyzeTrackService(wqueue<WorkItem*> & trackdbqueue, DeviceManager * pDevManager, DBManager * dbManager)
    : mTrackDBqueue(trackdbqueue), mDeviceManager(pDevManager), mDbManager(dbManager)
{

    // if ( mMysqlDB == NULL || mMysqlTSDB == NULL || mTrackDB == NULL)
    // {
    //     mInitial = false;
    //     mStatus = "AnalyzeTrackService() Initial failed!!";
    //     vilslog::printf ("AnalyzeTrackService() Initial failed!\n");
    // }
}

AnalyzeTrackService::~AnalyzeTrackService()
{
}

bool AnalyzeTrackService::status()
{
    return mInitial;
}

std::string AnalyzeTrackService::statusString()
{
    return mStatus;
}

void AnalyzeTrackService::updateProject()
{
    mUpdateProject = true;
}

void * AnalyzeTrackService::run()
{
    while (1)
    {
        mInitial = false;
        mStatus = "AnalyzeTrackService() Initial failed!!";

        try
        {
            runloop();
            vilslog::printf ("AnalyzeTrackService() Initial failed!\n");
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("AnalyzeTrackService::run() exception m_CurAnalyzeType[%d] m_CurAnalyzeTag[%s] m_dayInfoList_NotReleased[%d]\n",
                m_CurAnalyzeType, m_CurAnalyzeTag.c_str(), m_dayInfoList_NotReleased);
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");
            mInitial = false;
            mStatus = "AnalyzeTrackService::run() exception:";
            mStatus.append(exc.what());
        }
    }

    return NULL;
}

void AnalyzeTrackService::runloop()
{
    long long start1 = getSystemTime();
    long long updateTime = 590000;
    long long updateOneDayTime = 0;
    long long updatePrjIdTime = 0;

    m_CurAnalyzeTag = "";
    m_CurAnalyzeSensor = "";
    m_CurAnalyzeUserId = 0;
    m_CurAnalyzeType = TDT_POS;

    mPrjidList = mDbManager->getAllProjectConfig();

    mInitial = true;
    mStatus = "AnalyzeTrackService() Initial successfully!";
    vilslog::printf ("AnalyzeTrackService() Initial successfully!\n");


    // available to process.
    while (1)
    {
        long long end1 = getSystemTime();
        long long difft = end1 - start1;
        updateTime += difft;
        updateOneDayTime += difft;
        updatePrjIdTime += difft;

        if(updateTime >= 600000) // 10 minutes
        {
            //==================================
            // test only
            // m_CurAnalyzeTag = "Tag1181";
            // analyzeTagTrack(1, "Tag1181", end1);
            //==================================


            bool done = analyzeTrack(end1);

#ifdef ANALYZE_TRACK_PRINT_MSG
            long long difft_analyzeTrack = getSystemTime() - end1;

            vilslog::printf("AnalyzeTrackService::runloop() analyzeTrack Total Time[%lld ms] done[%d]\n",
                difft_analyzeTrack, done);
#endif

            if (done)
            {
                m_CurAnalyzeTag = "";
                m_CurAnalyzeType = TDT_POS;
                updateTime = 0;
            }
        }

        if(updateOneDayTime >= 60000) // 60 seconds
        {
            analyzeTrack_OneDay();
            updateOneDayTime = 0;
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
        sleep(10);
    }
}

bool AnalyzeTrackService::analyzeTrack(long long t_start)
{
    bool done = true;
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        if (m_CurAnalyzeSensor.length() == 0 && m_CurAnalyzeUserId == 0)
            done = analyzeTrack(projectid, t_start);
        if ( done)
            done = analyzeSensorTrack(projectid, t_start);
        if ( done)
            done = analyzeUserTrack(projectid, t_start);
    }
    return done;
}

bool AnalyzeTrackService::analyzeTrack(int projectid, long long t_start)
{
    bool done = false;
    std::list<NodeStruct*> * TagNodeList = mDeviceManager->get_Tag_list(projectid);

    for (std::list<NodeStruct*>::iterator i = TagNodeList->begin(); i != TagNodeList->end(); i++)
    {
#ifdef ANALYZE_TRACK_PRINT_MSG
        vilslog::printf("\n*********\nAnalyzeTrackService::analyzeTrack() nodeid[%s]\n", (*i)->nodeid);
        long long start_time = getSystemTime();
#endif

//#ifdef ANALYZE_TRACK_PRINT_MSG
//#endif

        if (m_CurAnalyzeTag.length() > 0 && !done)
        {
            if (m_CurAnalyzeTag.compare((*i)->nodeid) != 0)
            {
                continue;
            }
        }

        m_CurAnalyzeTag = (*i)->nodeid;

        done = analyzeTagTrack(projectid, NODE_TYPE_TAG, (*i)->nodeid, (*i)->macaddress, t_start);

#ifdef ANALYZE_TRACK_PRINT_MSG
        long long difft_analyzeTrack = getSystemTime() - start_time;
        vilslog::printf("AnalyzeTrackService::analyzeTrack() nodeid[%s]  time[%lld ms]\n", (*i)->nodeid, difft_analyzeTrack);
#endif

        if (!done)
            break;

        m_CurAnalyzeTag = "";
    }

    if ( (int)TagNodeList->size() == 0 )
    {
        done = true;
    }

    return done;
}

bool AnalyzeTrackService::analyzeSensorTrack(int projectid, long long t_start)
{
    bool done = false;
    std::list<SensorDeviceStruct*> * SensorNodeList = mDeviceManager->get_SensorDevice_list(projectid);

    for (std::list<SensorDeviceStruct*>::iterator i = SensorNodeList->begin(); i != SensorNodeList->end(); i++)
    {
        // vilslog::printf("AnalyzeTrackService::analyzeSensorTrack() SensorDevice id[%s]\n", (*i)->id.c_str());

        if (m_CurAnalyzeSensor.length() > 0 && !done)
        {
            if (m_CurAnalyzeSensor.compare((*i)->id) != 0)
            {
                continue;
            }
        }

        m_CurAnalyzeSensor = (*i)->id;

        done = analyzeTagTrack(projectid, NODE_TYPE_SENSOR, (*i)->id.c_str(), (*i)->id.c_str(), t_start);

        if (!done)
            break;

        m_CurAnalyzeSensor = "";
    }

    if ( (int)SensorNodeList->size() == 0 )
    {
        done = true;
    }

    return done;
}

bool AnalyzeTrackService::analyzeUserTrack(int projectid, long long t_start)
{
    bool done = false;
    std::list<UserStruct*> * UserList = mDeviceManager->get_User_list(projectid);

    for (std::list<UserStruct*>::iterator i = UserList->begin(); i != UserList->end(); i++)
    {
        // vilslog::printf("AnalyzeTrackService::analyzeSensorTrack() SensorDevice id[%s]\n", (*i)->id.c_str());

        if (m_CurAnalyzeUserId != 0 && !done)
        {
            if (m_CurAnalyzeUserId != (*i)->userid)
            {
                continue;
            }
        }

        m_CurAnalyzeUserId = (*i)->userid;

        std::string useridStr = to_string((*i)->userid);
        done = analyzeTagTrack(projectid, NODE_TYPE_USER, useridStr.c_str(), (*i)->account.c_str(), t_start);

        if (!done)
            break;

        m_CurAnalyzeUserId = 0;
    }

    if ( (int)UserList->size() == 0 )
    {
        done = true;
    }

    return done;
}

bool AnalyzeTrackService::analyzeTrack_OneDay()
{
    bool done = false;
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        done = analyzeTrack_OneDay(projectid);
    }
    return done;
}

bool AnalyzeTrackService::analyzeTrack_OneDay(int projectid)
{
    bool done = true;
    std::string todayDate  = TodayDateType1(); // YYYY-MM-DD
#ifdef ANALYZE_TRACK_PRINT_MSG
    long long start_time = getSystemTime();
#endif

    std::list<NodeStruct*> * TagNodeList = mDeviceManager->get_Tag_list(projectid);
    std::list<NodeStruct*> * LocatorNodeList = mDeviceManager->get_Locator_list(projectid);
    std::list<SensorDeviceStruct*> * SensorNodeList = mDeviceManager->get_SensorDevice_list(projectid);
    std::list<UserStruct*> * UserList = mDeviceManager->get_User_list(projectid);

    for (std::list<NodeStruct*>::iterator i = TagNodeList->begin(); i != TagNodeList->end(); i++)
    {
        // vilslog::printf("AnalyzeTrackService::analyzeTrack_OneDay() nodeid[%s]\n", (*i)->nodeid);

        //=====================
        //analyzeTagSingleTypeTrack_OneDay(projectid, (*i)->nodeid, (*i)->macaddress, TDT_POS, todayDate);
        //analyzeTagSingleTypeTrack_OneDay(projectid, (*i)->nodeid, (*i)->macaddress, TDT_AREA, todayDate);
        //analyzeTagSingleTypeTrack_OneDay(projectid, (*i)->nodeid, (*i)->macaddress, TDT_GPS, todayDate);
        analyzeTagSingleTypeTrack_OneDay(projectid, NODE_TYPE_TAG, (*i)->nodeid, (*i)->macaddress, TDT_COURSE, todayDate);
        //=====================
        analyzeTagSingleTypeTrack_OneDay(projectid, NODE_TYPE_TAG, (*i)->nodeid, (*i)->macaddress, TDT_ALERT, todayDate);
        analyzeTagSingleTypeTrack_OneDay(projectid, NODE_TYPE_TAG, (*i)->nodeid, (*i)->macaddress, TDT_EVENT, todayDate);
        //=====================

// #ifdef ANALYZE_TRACK_PRINT_MSG
//         vilslog::printf("AnalyzeTrackService::analyzeTrack_OneDay() nodeid[%s] todayDate[%s]\n",
//             (*i)->nodeid, todayDate.c_str());
// #endif
    }

    for (std::list<NodeStruct*>::iterator i = LocatorNodeList->begin(); i != LocatorNodeList->end(); i++)
    {
        //vilslog::printf("AnalyzeTrackService::analyzeTrack_OneDay() nodeid[%s]\n", (*i)->nodeid);

        //int projectid = mDeviceManager->getTagProjectId((*i)->macaddress);
        analyzeTagSingleTypeTrack_OneDay(projectid, NODE_TYPE_LOCATOR, (*i)->nodeid, (*i)->macaddress, TDT_ALERT, todayDate);
        analyzeTagSingleTypeTrack_OneDay(projectid, NODE_TYPE_LOCATOR, (*i)->nodeid, (*i)->macaddress, TDT_EVENT, todayDate);
    }

    for (std::list<SensorDeviceStruct*>::iterator i = SensorNodeList->begin(); i != SensorNodeList->end(); i++)
    {
        // vilslog::printf("AnalyzeTrackService::analyzeTrack_OneDay() SensorDevice id[%s]\n", (*i)->id.c_str());

        analyzeTagSingleTypeTrack_OneDay(projectid, NODE_TYPE_SENSOR, (*i)->id.c_str(), (*i)->id.c_str(), TDT_ALERT, todayDate);
        analyzeTagSingleTypeTrack_OneDay(projectid, NODE_TYPE_SENSOR, (*i)->id.c_str(), (*i)->id.c_str(), TDT_EVENT, todayDate);
    }

    for (std::list<UserStruct*>::iterator i = UserList->begin(); i != UserList->end(); i++)
    {
        // vilslog::printf("AnalyzeTrackService::analyzeTrack_OneDay() SensorDevice id[%s]\n", (*i)->id.c_str());

        std::string useridStr = to_string((*i)->userid);

        analyzeTagSingleTypeTrack_OneDay(projectid, NODE_TYPE_USER, useridStr.c_str(), (*i)->account.c_str(), TDT_AREA, todayDate);
        analyzeTagSingleTypeTrack_OneDay(projectid, NODE_TYPE_USER, useridStr.c_str(), (*i)->account.c_str(), TDT_ALERT, todayDate);
        analyzeTagSingleTypeTrack_OneDay(projectid, NODE_TYPE_USER, useridStr.c_str(), (*i)->account.c_str(), TDT_EVENT, todayDate);
    }

#ifdef ANALYZE_TRACK_PRINT_MSG
        long long difft_analyzeTrack = getSystemTime() - start_time;
        vilslog::printf("AnalyzeTrackService::analyzeTrack_OneDay() Done total time[%lld ms]\n", difft_analyzeTrack);
#endif

    return done;
}

bool AnalyzeTrackService::analyzeTagTrack(int projectid, int nodetype, const char * nodeid, const char * macaddress, long long t_start)
{
    bool done = false;
#ifdef ANALYZE_TRACK_PRINT_MSG
    long long start_time = getSystemTime();
#endif

    if (m_CurAnalyzeType == TDT_POS)
    {
        done = analyzeTagSingleTypeTrack(projectid, nodetype, nodeid, macaddress, TDT_POS, t_start);
        if (!done)
            return false;

        m_CurAnalyzeType = TDT_AREA;
    }
#ifdef ANALYZE_TRACK_PRINT_MSG
    long long difft_1 = getSystemTime() - start_time;
#endif

    if (m_CurAnalyzeType == TDT_AREA)
    {
        done = analyzeTagSingleTypeTrack(projectid, nodetype, nodeid, macaddress, TDT_AREA, t_start);
        if (!done)
            return false;
        m_CurAnalyzeType = TDT_GPS;
    }
#ifdef ANALYZE_TRACK_PRINT_MSG
    long long difft_2 = getSystemTime() - start_time;
#endif

    if (m_CurAnalyzeType == TDT_GPS)
    {
        done = analyzeTagSingleTypeTrack(projectid, nodetype, nodeid, macaddress, TDT_GPS, t_start);
        if (!done)
            return false;
        m_CurAnalyzeType = TDT_COURSE;
    }
#ifdef ANALYZE_TRACK_PRINT_MSG
    long long difft_3 = getSystemTime() - start_time;
#endif

    if (m_CurAnalyzeType == TDT_COURSE)
    {
        done = analyzeTagSingleTypeTrack(projectid, nodetype, nodeid, macaddress, TDT_COURSE, t_start);
        if (!done)
            return false;
        m_CurAnalyzeType = TDT_ALERT;
    }
#ifdef ANALYZE_TRACK_PRINT_MSG
    long long difft_4 = getSystemTime() - start_time;
#endif

    if (m_CurAnalyzeType == TDT_ALERT)
    {
        done = analyzeTagSingleTypeTrack(projectid, nodetype, nodeid, macaddress, TDT_ALERT, t_start);
        if (!done)
            return false;
        m_CurAnalyzeType = TDT_EVENT;
    }
#ifdef ANALYZE_TRACK_PRINT_MSG
    long long difft_5 = getSystemTime() - start_time;
#endif

    if (m_CurAnalyzeType == TDT_EVENT)
    {
        done = analyzeTagSingleTypeTrack(projectid, nodetype, nodeid, macaddress, TDT_EVENT, t_start);
        if (!done)
            return false;
        m_CurAnalyzeType = TDT_POS;
    }

    return true;
}

bool AnalyzeTrackService::analyzeTagSingleTypeTrack(int projectid, int nodetype, const char * nodeid, const char * macaddress, TrackDataType datatype, long long t_start)
{
    bool success = false;

    // get unAnalyze Date
    std::list<std::string> * unAnalyzeList = getUnAnalyzeDateListFromBuffer(projectid, nodetype, nodeid, datatype);

    if (unAnalyzeList == NULL)
        return true;

// #ifdef ANALYZE_TRACK_PRINT_MSG
//     if (unAnalyzeList->size() > 0)
//         vilslog::printf("AnalyzeTrackService::analyzeTrack() datatype[%d] nodeid[%s] unAnalyzeList->size()[%d]\n", datatype, nodeid, unAnalyzeList->size());
// #endif

    for (std::list<std::string>::iterator targetDate = unAnalyzeList->begin(); targetDate != unAnalyzeList->end(); targetDate++)
    {
        success = false;
        //
        // check timeout
        //
        long long diff_time = getSystemTime() - t_start;

        if ( diff_time >= 30000 )
        {
// #ifdef ANALYZE_TRACK_PRINT_MSG
//         if (unAnalyzeList->size() > 0)
//             vilslog::printf("\n\nAnalyzeTrackService::analyzeTrack() TIMEOUT diff_time[%lld]\n\n", diff_time);
// #endif
            break;
        }

        success = analyzeTagSingleTypeTrack_OneDay(projectid, nodetype, nodeid, macaddress, datatype, (*targetDate));

        if (success)
        {
            //
            // today does not set analyzeddone
            //
            std::string todayDate  = TodayDateType1(); // YYYY-MM-DD

            if ( todayDate.compare((*targetDate)) != 0 )
            {
                //
                // fill AnalyzeTrackRecordStruct
                //
                //AnalyzeTrackRecordStruct * record = new AnalyzeTrackRecordStruct();
                AnalyzeTrackRecordStruct record;
                record.projectid = projectid;
                record.nodetype = nodetype;
                record.nodeid = nodeid;
                record.datatype = datatype;
                record.analyzeddone = 1; // 1:done
                record.analyzeddate = (*targetDate); // date for already analyzed, foramt: YYYY-MM-DD

                WorkItem * writem = new WorkItem("analyzetrackrecord", projectid, &record);
                mTrackDBqueue.add(writem);

                //if (record!= NULL)
               //     delete record;
            }
        }
        else
        {
            break;
        }
    }

    // if (unAnalyzeList != NULL)
    // {
    //     unAnalyzeList->clear();
    //     delete unAnalyzeList;
    // }
    return success;
}

bool AnalyzeTrackService::analyzeTagSingleTypeTrack_OneDay(int projectid, int nodetype, const char * nodeid, const char * macaddress, TrackDataType datatype, std::string targetDate)
{
    // prepare one datetime, 20180101 -> 20180101 00:00:00 , 20180102 00:00:00
    struct tm fromtm = {0, 0, 0};
    strptime(targetDate.c_str(), "%Y-%m-%d", &fromtm);
    fromtm.tm_isdst = 0; //without daylight savings

    struct tm totm = {0, 0, 0};
    strptime(targetDate.c_str(), "%Y-%m-%d", &totm);
    totm.tm_mday += 1;
    totm.tm_isdst = 0; //without daylight savings

    time_t t_From = mktime(&fromtm);
    time_t t_To = mktime(&totm);

    std::string FromDatetime = time_tToString(t_From);
    std::string ToDatetime = time_tToString(t_To);

// #ifdef ANALYZE_TRACK_PRINT_MSG
    // if ( datatype == 3 && strcmp(nodeid,"Tag0117") == 0 )
    // {
    //     vilslog::printf("AnalyzeTrackService::analyzeTagSingleTypeTrack_OneDay() nodeid[%s] FromDatetime[%s] ToDatetime[%s]\n",
    //         nodeid, FromDatetime.c_str(), ToDatetime.c_str());
    // }
// #endif
    std::list<AnalyzeDayInfoStruct*> * dayInfoList = getAnalyzeDayInfo(projectid, nodetype, nodeid, macaddress, datatype, targetDate, FromDatetime, ToDatetime);

    if ( dayInfoList == NULL )
    {
// #ifdef ANALYZE_TRACK_PRINT_MSG
        // if ( datatype == 3 && strcmp(nodeid,"Tag0117") == 0  )
        // {
        //     vilslog::printf("AnalyzeTrackService::analyzeTagSingleTypeTrack_OneDay() dayInfo == NULL nodeid[%s] datatype[%d] targetDate[%s]\n", nodeid, datatype, targetDate.c_str());
        // }
// #endif
        return true;
    }

    m_dayInfoList_NotReleased = true;


// #ifdef ANALYZE_TRACK_PRINT_MSG
    // if ( datatype == 3 && strcmp(nodeid,"Tag0117") == 0  )
    // {
    //     vilslog::printf("AnalyzeTrackService::analyzeTagSingleTypeTrack_OneDay() nodeid[%s] datatype[%d] targetDate[%s] dayInfoList->size[%d]\n",
    //         nodeid, datatype, targetDate.c_str(), dayInfoList->size());
    // }
// #endif

    for (std::list<AnalyzeDayInfoStruct *>::iterator i = dayInfoList->begin(); i != dayInfoList->end(); i++)
    {
        AnalyzeDayInfoStruct * dayInfo = (*i);
        time_t t_max = StringDateTimeTotime_t(dayInfo->max_time);
        time_t t_min = StringDateTimeTotime_t(dayInfo->min_time);
        time_t t_diff = t_max - t_min;

        if ( dayInfo->count > 0 )
        {

#ifdef ANALYZE_TRACK_PRINT_MSG
            vilslog::printf("AnalyzeTrackService::analyzeTagSingleTypeTrack_OneDay() write_daytrack_record nodeid[%s] datatype[%d] count[%d] ",
                nodeid, datatype, dayInfo->count);
            if (dayInfo->day.length() > 0)
                vilslog::printf("day[%s] ", dayInfo->day.c_str());
            if (dayInfo->course_id.length() > 1)
                vilslog::printf("course_id[%s] ", dayInfo->course_id.c_str());
            vilslog::printf("\n");
#endif

            //
            // fill TagDayTrackRecordStruct
            //
            //TagDayTrackRecordStruct * record = new TagDayTrackRecordStruct();
            TagDayTrackRecordStruct record;
            record.projectid = projectid;
            record.nodeid = nodeid;
            record.nodetype = nodetype;
            record.datatype = datatype;
            record.count = dayInfo->count;
            record.day = dayInfo->day;
            record.max_time = dayInfo->max_time;
            record.min_time = dayInfo->min_time;
            record.total_time = time_tToTimerString(t_diff);
            record.area_ids = dayInfo->area_ids;
            record.area_names = dayInfo->area_names;
            record.course_id = dayInfo->course_id;
            record.course_info = dayInfo->course_info;

            WorkItem * writem = new WorkItem("daytrackrecord", projectid, &record);
            mTrackDBqueue.add(writem);

            //if (record!= NULL)
            //    delete record;
        }
    }

    clear_AnalyzeDayInfoStruct_list(dayInfoList);
    delete dayInfoList;

    m_dayInfoList_NotReleased = false;

    return true;
}

std::list<AnalyzeDayInfoStruct*> * AnalyzeTrackService::getAnalyzeDayInfo(int projectid, int nodetype, const char * nodeid, const char * macaddress, TrackDataType datatype, std::string targetDate, std::string FromDatetime, std::string ToDatetime)
{
    std::list<AnalyzeDayInfoStruct*> * dayInfoList = NULL;
    if (datatype == TDT_POS)
    {
        dayInfoList = new std::list<AnalyzeDayInfoStruct*>();

        FootprintDBAccess * fpDb = mDbManager->getFootprintDB(projectid);
        if (fpDb == NULL)
            return dayInfoList;

        AnalyzeDayInfoStruct * dayInfo = fpDb->getAnalyzeDayInfo(nodeid, macaddress, targetDate.c_str());
        if ( dayInfo != NULL )
        {
            // vilslog::printf ("AnalyzeTrackService::getAnalyzeDayInfo() nodeid[%s] getAnalyzeDayInfoFromCache()\n", nodeid);
            dayInfoList->push_back(dayInfo);
        }
    }
    else
    if (datatype == TDT_AREA)
    {
        dayInfoList = new std::list<AnalyzeDayInfoStruct*>();
        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if (baseDb != NULL)
        {
            AnalyzeDayInfoStruct * dayInfo = baseDb->getAreaInOutAnalyzeDayInfoFromCache(nodeid, FromDatetime.c_str(), ToDatetime.c_str());
            if (dayInfo == NULL)
            {
                //if ( strcmp(nodeid, "Tag1181")  == 0)
                    // vilslog::printf ("AnalyzeTrackService::getAnalyzeDayInfo() nodeid[%s] getAreaInOut cahce not found!\n", nodeid);
                dayInfo = baseDb->getAreaInOutAnalyzeDayInfo(nodeid, FromDatetime.c_str(), ToDatetime.c_str());
            }
            else
            {
                //if ( strcmp(nodeid, "Tag1181")  == 0)
                    // vilslog::printf ("AnalyzeTrackService::getAnalyzeDayInfo() nodeid[%s] getAreaInOutAnalyzeDayInfoFromCache()\n", nodeid);
            }

            if (dayInfo != NULL)
                dayInfoList->push_back(dayInfo);
        }
    }
    else
    if (datatype == TDT_GPS)
    {
        dayInfoList = new std::list<AnalyzeDayInfoStruct*>();
        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if (baseDb != NULL)
        {
            AnalyzeDayInfoStruct * dayInfo = baseDb->getGPSAnalyzeDayInfoFromCache(nodetype, nodeid, FromDatetime.c_str(), ToDatetime.c_str());
            if (dayInfo == NULL)
            {
                dayInfo = baseDb->getGPSAnalyzeDayInfo(nodetype, nodeid, FromDatetime.c_str(), ToDatetime.c_str());
            }
            if (dayInfo != NULL)
                dayInfoList->push_back(dayInfo);
        }
    }
    else
    if (datatype == TDT_COURSE)
    {
        dayInfoList = getCourseAnalyzeDayInfo(projectid, nodeid, FromDatetime.c_str(), ToDatetime.c_str());
    }
    else
    if (datatype == TDT_ALERT)
    {
        std::string macaddress = "";
        if (nodetype == NODE_TYPE_TAG)
            macaddress = mDeviceManager->getTagMacaddress(projectid, nodeid);
        else
        if (nodetype == NODE_TYPE_LOCATOR)
            macaddress = mDeviceManager->getLocatorMacaddress(projectid, nodeid);

        if (macaddress.length() > 0)
        {
            dayInfoList = new std::list<AnalyzeDayInfoStruct*>();
            MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
            if (baseDb != NULL)
            {
                AnalyzeDayInfoStruct * dayInfo = baseDb->getAlertAnalyzeDayInfoFromCache(macaddress.c_str(), FromDatetime.c_str(), ToDatetime.c_str());
                if (dayInfo == NULL)
                {
                    // vilslog::printf ("AnalyzeTrackService::getAnalyzeDayInfo() macaddress[%s] Alert cache not found!\n", macaddress.c_str());
                    dayInfo = baseDb->getAlertAnalyzeDayInfo(macaddress.c_str(), FromDatetime.c_str(), ToDatetime.c_str());
                }
                else
                {
                    // vilslog::printf ("AnalyzeTrackService::getAnalyzeDayInfo() macaddress[%s] getAlertAnalyzeDayInfoFromCache\n", macaddress.c_str());
                }

                if (dayInfo != NULL)
                {
                    // vilslog::printf ("AnalyzeTrackService::getAnalyzeDayInfo() macaddress[%s] getAlertAnalyzeDayInfoFromCache count[%d]\n",
                    //     macaddress.c_str(), dayInfo->count);

                    dayInfoList->push_back(dayInfo);
                }
            }
        }
    }
    else
    if (datatype == TDT_EVENT)
    {
        dayInfoList = getEventAnalyzeDayInfo(projectid, nodeid, FromDatetime.c_str(), ToDatetime.c_str());
    }
    return dayInfoList;
}

std::list<AnalyzeDayInfoStruct*> * AnalyzeTrackService::getCourseAnalyzeDayInfo(int projectid, const char * nodeid, std::string FromDatetime, std::string ToDatetime)
{
    std::list<AnalyzeDayInfoStruct*> * dayInfoList = new std::list<AnalyzeDayInfoStruct*>();

#ifdef ANALYZE_TRACK_PRINT_MSG
    vilslog::printf("AnalyzeTrackService::getCourseAnalyzeDayInfo() nodeid[%s] FromDatetime[%s] ToDatetime[%s]\n",
        nodeid, FromDatetime.c_str(), ToDatetime.c_str());
#endif

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
        return dayInfoList;

    std::list<CourseRollCallRecordStruct *> * rollcallRecordList = baseDb->read_rollcallRecordDayRange(FromDatetime.c_str(), ToDatetime.c_str());

    if ( rollcallRecordList == NULL )
        return dayInfoList;

#ifdef ANALYZE_TRACK_PRINT_MSG
    vilslog::printf("AnalyzeTrackService::getCourseAnalyzeDayInfo() nodeid[%s] rollcallRecordList->size()[%d]\n",
        nodeid, rollcallRecordList->size());
#endif

    for (std::list<CourseRollCallRecordStruct *>::iterator rollcall = rollcallRecordList->begin(); rollcall != rollcallRecordList->end(); rollcall++)
    {
        for (std::list<std::string>::iterator tag = (*rollcall)->tags->begin(); tag != (*rollcall)->tags->end(); tag++)
        {
            if ( (*tag).compare(nodeid) == 0 )
            {
                AnalyzeDayInfoStruct * dayInfo = getCourseAnalyzeDayRollCallInfo(projectid, nodeid, (*rollcall));
                if (dayInfo != NULL)
                    dayInfoList->push_back(dayInfo);
            }
        }
    }

    clear_CourseRollCallRecordStruct_list(rollcallRecordList);
    delete rollcallRecordList;

    return dayInfoList;
}

std::list<AnalyzeDayInfoStruct*> * AnalyzeTrackService::getEventAnalyzeDayInfo(int projectid, const char * nodeid, std::string FromDatetime, std::string ToDatetime)
{
    std::list<AnalyzeDayInfoStruct*> * dayInfoList = new std::list<AnalyzeDayInfoStruct*>();

//#ifdef ANALYZE_TRACK_PRINT_MSG
    // if( strcmp(nodeid, "Locator0006") == 0)
    // {
    //     vilslog::printf("AnalyzeTrackService::getEventAnalyzeDayInfo() nodeid[%s] FromDatetime[%s] ToDatetime[%s]\n",
    //         nodeid, FromDatetime.c_str(), ToDatetime.c_str());
    // }
//#endif

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
        return dayInfoList;

    std::list<FiredEventStruct*> * firedEventList = baseDb->read_fired_events_DayRange(FromDatetime.c_str(), ToDatetime.c_str(), "DESC");

    if ( firedEventList == NULL )
        return dayInfoList;

// #ifdef ANALYZE_TRACK_PRINT_MSG
    // if( strcmp(nodeid, "Locator0006") == 0)
    // {
    //     vilslog::printf("AnalyzeTrackService::getEventAnalyzeDayInfo() firedEventList->size()[%d]\n", firedEventList->size());
    // }
// #endif

    int event_count = 0;
    std::string max_time = "0000-00-00 00:00:00";
    std::string min_time = "9999-99-99 99:99:99";

    for (std::list<FiredEventStruct *>::iterator event = firedEventList->begin(); event != firedEventList->end(); event++)
    {
// #ifdef ANALYZE_TRACK_PRINT_MSG
        // if( strcmp(nodeid, "Locator0006") == 0)
        // {
        //     vilslog::printf("AnalyzeTrackService::getEventAnalyzeDayInfo() eventid[%d] datetime[%s]\n",
        //         (*event)->eventid, (*event)->datetime.c_str());
        // }
// #endif
        Json::Value JSONresult;

        Json::Reader * reader = new Json::Reader();
        bool parsingSuccessful = reader->parse( (*event)->eventjson.c_str(), JSONresult);
        delete reader;

        if (!parsingSuccessful)
        {
#ifdef ANALYZE_TRACK_PRINT_MSG
        vilslog::printf("AnalyzeTrackService::getEventAnalyzeDayInfo() !jsonAgent->get(JSONresult)\n");
#endif
            continue;
        }

        if ( max_time.compare((*event)->datetime) <= 0 )
            max_time = (*event)->datetime;
        if ( min_time.compare((*event)->datetime) >= 0 )
            min_time = (*event)->datetime;

        if ( (*event)->eventid == 9999 )
        {
            // eventid=9999 {"projectid":"1","eventid":"9999","nodeid":"Locator0169","action":"Alert","areaid":"","issuetime":"0","locatorid":"Locator0169"}
            // eventid=9999 {"projectid":"1","eventid":"9999","nodeid":"Tag0145","action":"Enter","areaid":"area6330","issuetime":"0","locatorid":""}

            if( JSONresult["nodeid"].asString().compare(nodeid) == 0)
            {
                event_count++;
            }
        }
        else
        if ( (*event)->eventid == 999 )
        {
            // {"type":"checkpoint","projectid":"1","GroupID":"85","PathIndex":"1","TagNodeId":"Tag0026","action":"enter","NodeId":"Locator1002","issuetime":"2019-05-23 16:58:34"}
            // {"type":"checkpointpath","projectid":"1","GroupID":"85","PathIndex":"1","TagNodeId":"Tag0026","action":"finish","StartNodeId":"Locator1000","EndNodeId":"Locator1002","issuetime":"2019-05-23 16:58:34"}

            if( JSONresult["type"].asString().compare("inspectionpoint") == 0 ||
                JSONresult["type"].asString().compare("inspectionpointtravel") == 0)
            {
                if( JSONresult["Inspector"].asString().compare(nodeid) == 0)
                {
                    event_count++;
                }
            }
        }
        else
        if ( JSONresult["nodeid"].asString().compare(nodeid) == 0 )
        {
            event_count++;
        }
        else
        if ( JSONresult["locatorid"].asString().compare(nodeid) == 0 )
        {
            event_count++;
        }
    }

    if ( event_count > 0 )
    {

#ifdef ANALYZE_TRACK_PRINT_MSG
    vilslog::printf("AnalyzeTrackService::getEventAnalyzeDayInfo()  max_time[%s] min_time[%s] event_count[%d]\n",
        max_time.c_str(), min_time.c_str(), event_count);
#endif

        AnalyzeDayInfoStruct * dayInfo = createAnalyzeDayInfoStruct();
        dayInfo->max_time = max_time;
        dayInfo->min_time = min_time;
        dayInfo->count = event_count;
        dayInfo->day = FromDatetime.substr(0, 10);

        dayInfoList->push_back(dayInfo);
    }

    clear_FiredEventStruct_list(firedEventList);
    delete firedEventList;

    return dayInfoList;
}

AnalyzeDayInfoStruct * AnalyzeTrackService::getCourseAnalyzeDayRollCallInfo(int projectid, const char * nodeid, CourseRollCallRecordStruct * curRollCall)
{
    AnalyzeDayInfoStruct * dayInfo = NULL;

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
    {
        return dayInfo;
    }

    CourseStruct * course = baseDb->read_Course(curRollCall->courseId.c_str());
    if (course == NULL)
    {
        return dayInfo;
    }

    std::string targetDate = curRollCall->issuetime.substr(0, 10);
    TagCourseDayRecordStruct * courseDayRecord = build_CourseDayRecord(projectid, nodeid, course, targetDate.c_str());
    if (courseDayRecord == NULL)
    {
        // vilslog::printf("AnalyzeTrackService::getCourseAnalyzeDayRollCallInfo() projectid[%d] nodeid[%s] targetDate[%s] courseDayRecord == NULL\n",
        //     projectid, nodeid, targetDate.c_str());

        return dayInfo;
    }

    std::string course_info = pack_json_course_info(courseDayRecord);

    // vilslog::printf("AnalyzeTrackService::getCourseAnalyzeDayRollCallInfo() projectid[%d] nodeid[%s] course_info[%s]\n",
    //     projectid, nodeid, course_info.c_str());

    dayInfo = createAnalyzeDayInfoStruct();
    dayInfo->max_time = courseDayRecord->out_max_time;
    dayInfo->min_time = courseDayRecord->in_min_time;
    dayInfo->count = courseDayRecord->in_count + courseDayRecord->out_count;
    dayInfo->day = targetDate;
    dayInfo->course_id = course->courseId;
    dayInfo->course_info = course_info;

    delete course;
    delete courseDayRecord;

    return dayInfo;
}

TagCourseDayRecordStruct * AnalyzeTrackService::build_CourseDayRecord(int projectid, const char * nodeid, CourseStruct * course, const char * targetDate)
{
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb == NULL)
        return NULL;

    TagCourseDayRecordStruct * courseDayRecord = new TagCourseDayRecordStruct();
    courseDayRecord->courseId = course->courseId;
    courseDayRecord->areaid = course->resourceId;
    courseDayRecord->areaname = baseDb->get_area_name(course->resourceId);
    courseDayRecord->course_title = course->title;
    courseDayRecord->course_start = course->start;
    courseDayRecord->course_end = course->end;
    courseDayRecord->course_startTime = course->startTime;
    courseDayRecord->course_endTime = course->endTime;
    courseDayRecord->course_startRecur = course->startRecur;
    courseDayRecord->course_endRecur = course->endRecur;

    std::string startTime = course->startTime;
    startTime.append(":00");
    if (course->startTime.length() == 0)
        startTime = "00:00:00";

    std::string endTime = course->endTime;
    endTime.append(":00");
    if (course->endTime.length() == 0)
        endTime = "23:59:59";

    // vilslog::printf("AnalyzeTrackService::build_CourseDayRecord() nodeid[%s] areaid[%s] targetDate[%s]\n",
    //     nodeid, courseDayRecord->areaid.c_str(), targetDate);
    //
    // vilslog::printf ("AnalyzeTrackService::build_CourseDayRecord() title[%s] start[%s] end[%s] startTime[%s] endTime[%s] startRecur[%s] endRecur[%s]\n",
    //     course->title.c_str(), course->start.c_str(), course->end.c_str(), course->startTime.c_str(), course->endTime.c_str(),
    //     course->startRecur.c_str(), course->endRecur.c_str());

    if((course->start.length() == 10) && (course->end.length() == 10))
    {
        // is all day
        // vilslog::printf("AnalyzeTrackService::build_CourseDayRecord() course->start.compare(targetDate)[%d] course->end.compare(targetDate)[%d]\n",
        //     course->start.compare(targetDate), course->end.compare(targetDate));

        if ( (course->start.compare(targetDate) <= 0) && (course->end.compare(targetDate) >= 0) )
        {
            // is in date range
        }
        else
        {
            delete courseDayRecord;
            return NULL;
        }
    }

    if( (course->startRecur.length() > 0) && (course->endRecur.length() > 0) )
    {
        // vilslog::printf("AnalyzeTrackService::build_CourseDayRecord() course->startRecur.compare(targetDate)[%d] course->endRecur.compare(targetDate)[%d]\n",
        //     course->startRecur.compare(targetDate), course->endRecur.compare(targetDate));
        if ( (course->startRecur.compare(targetDate) <= 0) && (course->endRecur.compare(targetDate) >= 0) )
        {
            // is in date range
        }
        else
        {
            delete courseDayRecord;
            return NULL;
        }
    }

    std::string range_startdate = targetDate;
    range_startdate.append(" ");
    range_startdate.append(startTime);

    std::string range_enddate = targetDate;
    range_enddate.append(" ");
    range_enddate.append(endTime);

    // vilslog::printf("AnalyzeTrackService::build_CourseDayRecord() range_startdate[%s] range_enddate[%s]\n",
    //     range_startdate.c_str(), range_enddate.c_str());

    //
    // find area inout between start and end
    //
    AreaInOutEventStruct * areaevent_in = NULL;
    AreaInOutEventStruct * areaevent_out = NULL;
    int action = 1; // in
    AnalyzeDayInfoStruct * areainout_records_in = baseDb->getAreaInOutAnalyzeDayInfoByAreaId(nodeid, courseDayRecord->areaid.c_str(), action, range_startdate.c_str(), range_enddate.c_str());
    if (areainout_records_in == NULL)
    {
        // no data

        areaevent_in = baseDb->getNodeLastAreaInOutList(nodeid, courseDayRecord->areaid.c_str(), range_startdate.c_str(), 1);
        areaevent_out = baseDb->getNodeLastAreaInOutList(nodeid, courseDayRecord->areaid.c_str(), range_startdate.c_str(), 2);

        bool bStillInside = false;
        if ( areaevent_in->DataTime.compare(areaevent_out->DataTime) > 0)
        {
            bStillInside = true;
        }

        if(bStillInside)
        {
            if ( range_startdate.compare(areaevent_in->DataTime) >= 0)
            {
                courseDayRecord->in_max_time = range_startdate;
                courseDayRecord->in_min_time = range_startdate;
                courseDayRecord->in_count = 1;
            }
            else
            {
                courseDayRecord->in_max_time = areaevent_in->DataTime;
                courseDayRecord->in_min_time = areaevent_in->DataTime;
                courseDayRecord->in_count = 1;
            }
        }
        else
        {
            // 已離開始此區域
            courseDayRecord->in_max_time = "";
            courseDayRecord->in_min_time = "";
            courseDayRecord->in_count = 0;
        }
    }
    else
    {
        courseDayRecord->in_max_time = areainout_records_in->max_time;
        courseDayRecord->in_min_time = areainout_records_in->min_time;
        courseDayRecord->in_count = areainout_records_in->count;

        delete areainout_records_in;
    }

    action = 2; // out
    AnalyzeDayInfoStruct * areainout_records_out = baseDb->getAreaInOutAnalyzeDayInfoByAreaId(nodeid, courseDayRecord->areaid.c_str(), action, range_startdate.c_str(), range_enddate.c_str());
    if (areainout_records_out == NULL)
    {
        // no data
        std::string curDatetime = currentDateTimeForSQL();
        std::string targetDatetime = curDatetime;
        if (curDatetime.compare(range_enddate) >= 0 )
            targetDatetime = range_enddate;

        if(areaevent_in == NULL)
            areaevent_in = baseDb->getNodeLastAreaInOutList(nodeid, courseDayRecord->areaid.c_str(), range_startdate.c_str(), 1);

        if(areaevent_out == NULL)
            areaevent_out = baseDb->getNodeLastAreaInOutList(nodeid, courseDayRecord->areaid.c_str(), range_startdate.c_str(), 2);

        bool bStillInside = false;
        if ( areaevent_in->DataTime.compare(areaevent_out->DataTime) > 0)
        {
            bStillInside = true;
        }

        if(bStillInside)
        {
            courseDayRecord->out_max_time = targetDatetime;
            courseDayRecord->out_min_time = targetDatetime;
            courseDayRecord->out_count = 1;
        }
        else
        {
            // 已離開始此區域
            courseDayRecord->out_max_time = "";
            courseDayRecord->out_min_time = "";
            courseDayRecord->out_count = 0;
        }
    }
    else
    {
        courseDayRecord->out_max_time = areainout_records_out->max_time;
        courseDayRecord->out_min_time = areainout_records_out->min_time;
        courseDayRecord->out_count = areainout_records_out->count;
        delete areainout_records_out;
    }

    if (areaevent_in != NULL)
        delete areaevent_in;
    if (areaevent_out != NULL)
        delete areaevent_out;

    // courseDayRecord->in_max_time = areainout_records_in->max_time;
    // courseDayRecord->in_min_time = areainout_records_in->min_time;
    // courseDayRecord->in_count = areainout_records_in->count;
    // if(areainout_records_in->max_time.length() == 0 && areainout_records_in->count == 0)
    // {
    //     // no data
    //     courseDayRecord->in_max_time = "";
    //     courseDayRecord->in_min_time = "";
    //     courseDayRecord->in_count = 0;
    // }

    // courseDayRecord->out_max_time = areainout_records_out->max_time;
    // courseDayRecord->out_min_time = areainout_records_out->min_time;
    // courseDayRecord->out_count = areainout_records_out->count;
    // if(areainout_records_out->max_time.length() == 0 && areainout_records_out->count == 0)
    // {
    //     // no data
    //     courseDayRecord->out_max_time = "";
    //     courseDayRecord->out_min_time = "";
    //     courseDayRecord->out_count = 0;
    // }


    return courseDayRecord;
}

std::string AnalyzeTrackService::pack_json_course_info(TagCourseDayRecordStruct * courseDayRecord)
{
    std::string course_info = "";

    course_info.append("{");
    course_info.append("\"courseId\":\"");
    course_info.append(courseDayRecord->courseId);
    course_info.append("\",\"areaid\":\"");
    course_info.append(courseDayRecord->areaid);
    course_info.append("\",\"areaname\":\"");
    course_info.append(courseDayRecord->areaname);
    course_info.append("\",\"course_title\":\"");
    course_info.append(courseDayRecord->course_title);
    course_info.append("\",\"course_start\":\"");
    course_info.append(courseDayRecord->course_start);
    course_info.append("\",\"course_end\":\"");
    course_info.append(courseDayRecord->course_end);
    course_info.append("\",\"course_startTime\":\"");
    course_info.append(courseDayRecord->course_startTime);
    course_info.append("\",\"course_endTime\":\"");
    course_info.append(courseDayRecord->course_endTime);
    course_info.append("\",\"course_startRecur\":\"");
    course_info.append(courseDayRecord->course_startRecur);
    course_info.append("\",\"course_endRecur\":\"");
    course_info.append(courseDayRecord->course_endRecur);
    course_info.append("\",\"in_max_time\":\"");
    course_info.append(courseDayRecord->in_max_time);
    course_info.append("\",\"in_min_time\":\"");
    course_info.append(courseDayRecord->in_min_time);
    course_info.append("\",\"in_count\":\"");
    course_info.append(std::to_string(courseDayRecord->in_count));
    course_info.append("\",\"out_max_time\":\"");
    course_info.append(courseDayRecord->out_max_time);
    course_info.append("\",\"out_min_time\":\"");
    course_info.append(courseDayRecord->out_min_time);
    course_info.append("\",\"out_count\":\"");
    course_info.append(std::to_string(courseDayRecord->out_count));
    course_info.append("\"}");

    return course_info;
}

std::list<std::string> * AnalyzeTrackService::getUnAnalyzeDateList(int projectid, const char * nodeid, TrackDataType datatype)
{
    //std::string StartDate = "2020-01-01";
    std::string StartDate = TodayDateType1(30);
    TrackDBAccess * trackDb = mDbManager->getTrackDB(projectid);
    if (trackDb == NULL)
        return NULL;

    std::list<std::string> * analyzedList = trackDb->getAnalyzedDate(nodeid, datatype, StartDate);

    std::string StartYear = TodayYear(30);
    std::string StartMonth = TodayMonth(30);
    std::string StartDay = TodayDay(30);
    std::list<std::string> * unAnalyzeList = prepareDateList(convert_string_to_int(StartYear), convert_string_to_int(StartMonth), convert_string_to_int(StartDay));

// #ifdef ANALYZE_TRACK_PRINT_MSG
    // vilslog::printf("AnalyzeTrackService::getUnAnalyzeDateList() nodeid[%s] datatype[%d] analyzedList->size()[%d] unAnalyzeList->size()[%d]\n",
    //     nodeid, datatype, analyzedList->size(), unAnalyzeList->size());
// #endif

    // remove analyzed date
    for (std::list<std::string>::iterator doneDate = analyzedList->begin(); doneDate != analyzedList->end(); doneDate++)
    {
        for (std::list<std::string>::iterator targetDate = unAnalyzeList->begin(); targetDate != unAnalyzeList->end(); targetDate++)
        {
            if( (*doneDate).compare((*targetDate)) == 0 )
            {
                unAnalyzeList->remove((*targetDate));
                break;
            }
        }
    }

// #ifdef ANALYZE_TRACK_PRINT_MSG
    // if (unAnalyzeList->size() > 0)
    // {
    //     if (datatype == 3)
    //     {
    //         vilslog::printf("AnalyzeTrackService::getUnAnalyzeDateList() end nodeid[%s] datatype[%d] unAnalyzeList->size()[%d]\n",
    //             nodeid, datatype, unAnalyzeList->size());
    //     }
    // }
// #endif

    if (analyzedList != NULL)
    {
        analyzedList->clear();
        delete analyzedList;
    }
    return unAnalyzeList;
}

std::list<std::string> * AnalyzeTrackService::prepareDateList(int StartYear, int StartMonth, int StartDay)
{
    std::list<std::string> * dateList = new std::list<std::string>();

    struct tm date = { 0, 0, 12 } ;  // nominal time midday (arbitrary).

    int year = StartYear ;
    int month = StartMonth ;
    int day = StartDay ;

    // Set up the date structure
    date.tm_year = year - 1900 ;
    date.tm_mon = month - 1 ;  // note: zero indexed
    date.tm_mday = day ;       // note: not zero indexed

    std::string todayDate  = TodayDateType1(); // YYYY-MM-DD

    char tmpStr[32];
    memset(tmpStr, 0, 32);
    sprintf(tmpStr, "%d-%02d-%02d", year, month, day);
    std::string targetDate = tmpStr;
    //vilslog::printf("%s\n", tmpStr);

    while(todayDate.compare(targetDate) >= 0)
    {
        dateList->push_back(targetDate);

        DatePlusDays( &date, 1 ) ;

        memset(tmpStr, 0, 32);
        sprintf(tmpStr, "%d-%02d-%02d", date.tm_year + 1900, date.tm_mon + 1, date.tm_mday);
        targetDate = tmpStr;

        // vilslog::printf("AnalyzeTrackService::prepareDateList() StartYear[%d] StartMonth[%d] StartDay[%d] %s\n", StartYear, StartMonth, StartDay, tmpStr);
    }

    //vilslog::printf("dateList->size()[%d]\n", dateList->size());

    // Show time/date using default formatting
    //std::cout << asctime( &date ) << std::endl ;

    return dateList;
}

std::list<std::string> * AnalyzeTrackService::getUnAnalyzeDateListFromBuffer(int projectid, int nodetype, const char * nodeid, TrackDataType datatype)
{
    bool bFound = false;
    std::string todayDate = TodayDateType1(); // YYYY-MM-DD

    std::list<std::string> * unAnalyzeDateList = NULL;
    for (std::list<UnAnalyzeDateDataBufferStruct*>::iterator buffer = m_UnAnalyzeDateDataBufferList.begin(); buffer != m_UnAnalyzeDateDataBufferList.end(); buffer++)
    {
        if( ((*buffer)->projectid == projectid) && ((*buffer)->unAnalyzeDateDatatype == datatype) && ((*buffer)->nodeid.compare(nodeid) == 0) && ((*buffer)->nodetype == nodetype) )
        {
            if ( todayDate.compare((*buffer)->bufferDataDate) != 0 )
            {
                // get new data
                if ((*buffer)->unAnalyzeDateList != NULL)
                {
                    (*buffer)->unAnalyzeDateList->clear();
                    delete (*buffer)->unAnalyzeDateList;
                }

                (*buffer)->bufferDataDate = todayDate;
                (*buffer)->unAnalyzeDateList = getUnAnalyzeDateList(projectid, nodeid, datatype);
            }

            unAnalyzeDateList = (*buffer)->unAnalyzeDateList;

            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        UnAnalyzeDateDataBufferStruct * newBuffer = new UnAnalyzeDateDataBufferStruct();
        newBuffer->projectid = projectid;
        newBuffer->nodetype = nodetype;
        newBuffer->unAnalyzeDateDatatype = datatype;
        newBuffer->nodeid = nodeid;
        newBuffer->bufferDataDate = todayDate;
        newBuffer->unAnalyzeDateList = getUnAnalyzeDateList(projectid, nodeid, datatype);
        m_UnAnalyzeDateDataBufferList.push_back(newBuffer);

        unAnalyzeDateList = newBuffer->unAnalyzeDateList;
    }

    return unAnalyzeDateList;
}
