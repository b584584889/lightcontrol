/*
 *  CoursesService.cpp
 *
 *  Created on: 2019/04/08
 *      Author: ikki
 */
#include <string.h>

#include "CoursesService.h"

using namespace std;

CoursesService::CoursesService(wqueue<DataItem*> & CoursesJobQueue, wqueue<AreaInOutEventStruct*> & mcourseAreaInOutEventQueue, wqueue<WorkItem*> & MySQLJobQueue, DeviceManager * pDevManager, DBManager * dbManager)
    : mCoursesJobQueue(CoursesJobQueue), mCourseAreaInOutEventQueue(mcourseAreaInOutEventQueue), mMySQLJobQueue(MySQLJobQueue), mDeviceManager(pDevManager), mDbManager(dbManager)
{
    mInitial = false;

    //mCourseList = NULL;
    //mRollCallRecordList = NULL;
}

CoursesService::~CoursesService()
{
    // clear_CourseAnchorRanging_vector(&mAnchorRangingMapVector);
}

bool CoursesService::status()
{
    return mInitial;
}

std::string CoursesService::statusString()
{
    return mStatus;
}

void CoursesService::updateProject()
{
    mUpdateProject = true;
}

void * CoursesService::run()
{
    while(1)
    {
        try
        {
            runloop();
            sleep(5);
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("CoursesService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "CoursesService()::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void CoursesService::runloop()
{
    long long start1 = getSystemTime();
    long long end1 = getSystemTime();
    long long difft = end1 - start1;
    long long CheckCoursesTime = 0;
    long long updatePrjIdTime = 0;

    mPrjidList = mDbManager->getAllProjectConfig();

	initProjectCourseInfo();

    mInitial = true;

    if ( mInitial )
    {
        mStatus = "CoursesService()::run() Initial Courses successfully!";
        vilslog::printf ("CoursesService()::run() Initial Courses successfully!\n");
    }
    else
    {
        mStatus = "CoursesService()::run() Initial Courses failed!!";
        vilslog::printf ("CoursesService()::run() Initial Courses failed!\n");
        return;
    }

    while (1)
    {
        AreaInOutEventStruct * event = mCourseAreaInOutEventQueue.removetimed(1000);

        if ( event != NULL )
        {
            CheckCourses(event);
            delete event;
        }
        else
        {
            DataItem * item = mCoursesJobQueue.removetimed(10);
            if( item != NULL )
            {
                //printf("CoursesService::run() item:Type[%s]\n", item->getType());

                processDataItem(item);
                delete item;
            }
        }

        end1 = getSystemTime();
        difft = end1 - start1;
        //printf("wait_loop() time[%lld ms]\n", difft);

        CheckCoursesTime += difft;
        updatePrjIdTime += difft;

        if ( CheckCoursesTime >= 3000) // 3000 miliseconds
        {
            CheckCourses();
            CheckCoursesTime = 0;
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

void CoursesService::initProjectCourseInfo()
{
    clear_ProjectCourseInfoStruct_vector(&mProjectCourseInfoVector);

    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);

        ProjectCourseInfoStruct * newInfo = createProjectCourseInfo(projectid);

        mProjectCourseInfoVector.push_back(newInfo);
    }
}

ProjectCourseInfoStruct * CoursesService::createProjectCourseInfo(int projectid)
{
    ProjectCourseInfoStruct * newInfo = new ProjectCourseInfoStruct();
    newInfo->projectid = projectid;
    newInfo->mCourseList = NULL;
    newInfo->mRollCallRecordList = new std::list<CourseRollCallRecordStruct*>();

    reloadCourseList(newInfo);

    return newInfo;
}

ProjectCourseInfoStruct * CoursesService::getProjectCourseInfo(int projectid)
{
    ProjectCourseInfoStruct * courseInfo = NULL;
    for (unsigned int i=0; i< mProjectCourseInfoVector.size(); i++)
    {
        if (mProjectCourseInfoVector[i]->projectid == projectid)
        {
            courseInfo = mProjectCourseInfoVector[i];
            break;
        }
    }

    if( courseInfo == NULL )
    {
        courseInfo = createProjectCourseInfo(projectid);

        mProjectCourseInfoVector.push_back(courseInfo);
    }

    return courseInfo;
}

void CoursesService::reloadCourseList()
{
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);

        bool bFound = false;
        for (std::vector<ProjectCourseInfoStruct*>::iterator i = mProjectCourseInfoVector.begin(); i != mProjectCourseInfoVector.end(); i++)
        {
            if ( (*i)->projectid == projectid )
            {
                reloadCourseList((*i));
                bFound = true;
                break;
            }
        }
        if (!bFound)
        {
            ProjectCourseInfoStruct * newInfo = createProjectCourseInfo(projectid);
            mProjectCourseInfoVector.push_back(newInfo);
        }
    }
}

void CoursesService::reloadCourseList(ProjectCourseInfoStruct * courseInfo)
{
    vilslog::printf("CoursesService::reloadCourseList()\n");

    if (courseInfo->mCourseList != NULL)
    {
        vilslog::printf("CoursesService::reloadCourseList() projectid[%d] old mCourseList->size()[%d]\n", courseInfo->projectid, (int)courseInfo->mCourseList->size());

        clear_Course_list(courseInfo->mCourseList);
        delete courseInfo->mCourseList;
    }
    //
    // Load from database
    //
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(courseInfo->projectid);
    if (baseDb != NULL)
        courseInfo->mCourseList = baseDb->read_allCourses(-1);// read all projects
    //mCourseList = create_test_Courses();

    vilslog::printf("CoursesService::reloadCourseList() projectid[%d] new mCourseList->size()[%d]\n", courseInfo->projectid, (int)courseInfo->mCourseList->size());

}

int CoursesService::processDataItem(DataItem * dataItem)
{
    if ( strcmp(dataItem->getType(), "updatecourse") == 0 )
    {
        vilslog::printf ("CoursesService::processDataItem() updatecourse \n");

        reloadCourseList();
    }

    return 0;
}

int CoursesService::CheckCourses()
{
    int newCount = 0;
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);

        newCount += CheckCourses(projectid);
    }
    return newCount;
}

int CoursesService::CheckCourses(int projectid)
{
    int newCount = 0;
    std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);

    ProjectCourseInfoStruct * courseInfo = getProjectCourseInfo(projectid);

    std::string infoOfDate = TodayDate(); //YYYYMMDD
    for (std::list<CourseStruct*>::iterator checkCourse = courseInfo->mCourseList->begin(); checkCourse != courseInfo->mCourseList->end(); checkCourse++)
    {
        bool bInTime = CheckCourseInTime(infoOfDate, (*checkCourse));

        if (!bInTime)
        {

            // vilslog::printf ("CoursesService::CheckCourses() infoOfDate[%s] courseId[%s] resourceId[%s] !bInTime\n",
            //         infoOfDate.c_str(), (*checkCourse)->courseId.c_str(), (*checkCourse)->resourceId.c_str());

            continue;
        }

        for (std::list<NodeStruct*>::iterator node = tagNodeList->begin(); node != tagNodeList->end(); node++)
        {
            if ( (*checkCourse)->resourceId.compare((*node)->AreaId) == 0 )
            {
                bool bExisted = update_CourseRollCallRecord(projectid, infoOfDate, (*checkCourse), (*node)->nodeid);
                if (!bExisted)
                {
                    // vilslog::printf ("CoursesService::CheckCourses() create new record infoOfDate[%s] resourceId[%s] tagId[%s]\n",
                    //         infoOfDate.c_str(), (*checkCourse)->resourceId.c_str(), (*node)->nodeid);
                    newCount++;
                }
            }
        }
    }

    return newCount;
}

int CoursesService::CheckCourses(AreaInOutEventStruct * event)
{
    int newCount = 0;
    std::string infoOfDate = TodayDate(); //YYYYMMDD

    int projectid = event->projectid;
    ProjectCourseInfoStruct * courseInfo = getProjectCourseInfo(projectid);

    for (std::list<CourseStruct*>::iterator checkCourse = courseInfo->mCourseList->begin(); checkCourse != courseInfo->mCourseList->end(); checkCourse++)
    {
        if ((*checkCourse)->projectid != event->projectid)
        {
            continue;
        }

        if ((*checkCourse)->resourceId.compare(event->AreaID) == 0)
        {
            bool bInTime = CheckCourseInTime(infoOfDate, (*checkCourse));

            if (bInTime)
            {
                newCount += CheckRollCall(projectid, infoOfDate, (*checkCourse), event);

                if (newCount > 0)
                {
                    vilslog::printf ("CoursesService::CheckCourses() [CheckRollCall] dbId[%d] resourceId[%s] courseId[%s] newCount[%d]\n",
                            (*checkCourse)->dbId, (*checkCourse)->resourceId.c_str(), (*checkCourse)->courseId.c_str(), newCount);
                }
            }
        }
    }
    return newCount;
}

bool CoursesService::CheckCourseInTime(std::string infoOfDate, CourseStruct * curCourse)
{
    bool bInTime = false;

    //
    // Find the course at the current time
    //
    time_t t_cur = getCurrentTimeInSec();
    time_t t_check_start = 0;
    time_t t_check_end = 0;

    // vilslog::printf ("\n\n===============\nCoursesService::CheckCourse() dbId[%d] resourceId[%s] courseId[%s] allDay[%d] t_cur[%s]\n",
    //         curCourse->dbId, curCourse->resourceId.c_str(), curCourse->courseId.c_str(), curCourse->allDay, time_tToString(t_cur).c_str());

    if ( curCourse->allDay == 1)
    {
        //
        // check start and end
        // expand end to min-night
        time_t t_start = StringDateTotime_t(curCourse->start, 0);
        time_t t_end = StringDateTotime_t(curCourse->end, 1);

        t_check_start = t_start;
        t_check_end = t_end;

        // vilslog::printf ("CoursesService::CheckCourse() [allDay] start[%s] end[%s] t_start[%s] t_end[%s]\n",
        //         curCourse->start.c_str(), curCourse->end.c_str(), time_tToString(t_start).c_str(), time_tToString(t_end).c_str());
    }
    else
    {
        if (curCourse->startRecur.length() == 0 || curCourse->endRecur.length() == 0)
        {
            //
            // single day
            //
            time_t t_start = StringDateTimeTotime_t(curCourse->start);
            time_t t_end = StringDateTimeTotime_t(curCourse->end);
            t_check_start = t_start;
            t_check_end = t_end;

            // vilslog::printf ("CoursesService::CheckCourse() [single day] start[%s] end[%s] t_start[%s] t_end[%s]\n",
            //         curCourse->start.c_str(), curCourse->end.c_str(), time_tToString(t_start).c_str(), time_tToString(t_end).c_str());
        }
        else
        {
            //
            // long range
            //
            time_t t_startRecur = StringDateTotime_t(curCourse->startRecur, 0);
            time_t t_endRecur = StringDateTotime_t(curCourse->endRecur, 1);

            // vilslog::printf ("CoursesService::CheckCourse() [long range] startRecur[%s] endRecur[%s] t_startRecur[%s] t_endRecur[%s]\n",
            //         curCourse->startRecur.c_str(), curCourse->endRecur.c_str(), time_tToString(t_startRecur).c_str(), time_tToString(t_endRecur).c_str());

            //
            // check if today is in range
            //
            int t_diff_today_start = (int)(t_cur - t_startRecur);
            int t_diff_today_end = (int)(t_cur - t_endRecur);
            // vilslog::printf ("CoursesService::CheckCourse() [long range] t_diff_today_start[%d] t_diff_today_end[%d]\n",
            //     t_diff_today_start, t_diff_today_end);

            if( t_diff_today_start >= 0 && t_diff_today_end <= 0)
            {
                //printf ("CoursesService::CheckCourse() [long range] Found!\n");

                //
                // apply to today DateTime
                //
                time_t t_startTime = StringTimeTotime_t(curCourse->startTime, 0);
                time_t t_endTime = StringTimeTotime_t(curCourse->endTime, 1);
                // vilslog::printf ("CoursesService::CheckCourse() [long range] startTime[%s] endTime[%s] t_startTime[%s] t_endTime[%s]\n",
                //         curCourse->startTime.c_str(), curCourse->endTime.c_str(), time_tToString(t_startTime).c_str(), time_tToString(t_endTime).c_str());


                t_check_start = t_startTime;
                t_check_end = t_endTime;
            }
        }

        // if (curCourse->start.length() <= 10)
        //     t_start = StringDateTotime_t(curCourse->start);
        // if (curCourse->end.length() <= 10)
        //     t_end = StringDateTotime_t(curCourse->end);


        // vilslog::printf ("CoursesService::CheckCourse() t_startRecur[%d] t_endRecur[%d] resourceId[%s] courseId[%s]\n",
        //         (int)t_startRecur, (int)t_endRecur, curCourse->resourceId.c_str(), curCourse->courseId.c_str());
    }

    int t_diff_start = (int)(t_cur - t_check_start);
    int t_diff_end = (int)(t_cur - t_check_end);

    // vilslog::printf ("CoursesService::CheckCourse() t_check_start[%s] t_check_end[%s]\n",
    //         time_tToString(t_check_start).c_str(), time_tToString(t_check_end).c_str());

    if( t_diff_start >= 0 && t_diff_end <= 0)
    {
        // vilslog::printf ("CoursesService::CheckCourse() [CheckRollCall] dbId[%d] resourceId[%s] courseId[%s] \n",
        //         curCourse->dbId, curCourse->resourceId.c_str(), curCourse->courseId.c_str());
        bInTime = true;
    }
    else
    {
        bInTime = false;
    }

    return bInTime;
}

int CoursesService::CheckRollCall(int projectid, std::string infoOfDate, CourseStruct * curCourse, AreaInOutEventStruct * event)
{
    int newCount = 0;

    //event->action;
    if ( event->action == 1 )//1:in 2:out
    {
        bool bExisted = update_CourseRollCallRecord(projectid, infoOfDate, curCourse, event->NodeId);
        if (!bExisted)
        {
            vilslog::printf ("CoursesService::CheckRollCall() create new record infoOfDate[%s] resourceId[%s] tagId[%s]\n",
                    infoOfDate.c_str(), curCourse->resourceId.c_str(), event->NodeId.c_str());
            newCount++;
        }
    }

    return newCount;

}

bool CoursesService::update_CourseRollCallRecord(int projectid, std::string infoOfDate, CourseStruct * curCourse, std::string tagId)
{
    ProjectCourseInfoStruct * courseInfo = getProjectCourseInfo(projectid);

    //
    // find the record from list
    //
    CourseRollCallRecordStruct * curRecord = NULL;
    for (std::list<CourseRollCallRecordStruct*>::iterator record = courseInfo->mRollCallRecordList->begin(); record != courseInfo->mRollCallRecordList->end(); record++)
    {
        if ( (*record)->courseId.compare(curCourse->courseId) == 0 && (*record)->infoOfDate.compare(infoOfDate) == 0)
        {
            curRecord = (*record);
            break;
        }
    }

    if ( curRecord == NULL )
    {
        // read from database
        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if (baseDb != NULL)
            curRecord = baseDb->read_rollcallRecord(projectid, infoOfDate, curCourse->courseId);
    }

    if ( curRecord == NULL )
    {
        // create new one
        curRecord = new CourseRollCallRecordStruct();
        curRecord->projectid = projectid;
        curRecord->courseId = curCourse->courseId;
        curRecord->infoOfDate = infoOfDate;
        curRecord->tags = new std::list<std::string>();

        courseInfo->mRollCallRecordList->push_back(curRecord);
    }

    //
    // skip duplicate tag
    //
    bool bFound = false;
    for (std::list<std::string>::iterator tag = curRecord->tags->begin(); tag != curRecord->tags->end(); tag++)
    {
        if ( (*tag).compare(tagId) == 0 )
        {
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        curRecord->tags->push_back(tagId);

        WorkItem * witem = new WorkItem("updateCourseRollCallRecord", curRecord->projectid, curRecord);
        mMySQLJobQueue.add(witem);
    }

    return bFound;
}

void CoursesService::clear_ProjectCourseInfoStruct_vector(std::vector<ProjectCourseInfoStruct*> * plist)
{
    for (std::vector<ProjectCourseInfoStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        if ((*i)->mCourseList != NULL)
        {
            clear_Course_list((*i)->mCourseList);
            delete (*i)->mCourseList;
        }
        if ((*i)->mRollCallRecordList != NULL)
        {
            clear_CourseRollCallRecordStruct_list((*i)->mRollCallRecordList);
            delete (*i)->mRollCallRecordList;
        }
    }
    plist->clear();
}
