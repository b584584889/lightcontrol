/*
 *  CoursesService.h
 *
 *  Created on: 2019/04/08
 *      Author: ikki
 */
#ifndef __CoursesService_h__
#define __CoursesService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../common.h"
#include "../config.h"
#include "../network/DataItem.h"
#include "../network/WorkItem.h"
#include "../DeviceManager/DeviceManager.h"
#include "../mysql/DBManager.h"

using namespace std;

typedef struct
{
    int projectid;
    std::list<CourseStruct*> * mCourseList;
    std::list<CourseRollCallRecordStruct*> * mRollCallRecordList;
} ProjectCourseInfoStruct;

class CoursesService : public Thread
{
public:
    CoursesService(wqueue<DataItem*> & CoursesJobQueue, wqueue<AreaInOutEventStruct*> & mcourseAreaInOutEventQueue, wqueue<WorkItem*> & MySQLJobQueue, DeviceManager * pDevManager, DBManager * dbManager);
    ~CoursesService();
    bool status();
    std::string statusString();
    void updateProject();

    void * run();

private:
    bool mInitial;
    std::string mStatus;

    wqueue<DataItem*> & mCoursesJobQueue;
    wqueue<AreaInOutEventStruct*> & mCourseAreaInOutEventQueue;
    wqueue<WorkItem*> & mMySQLJobQueue;
    DeviceManager * mDeviceManager;
    DBManager * mDbManager;

    std::vector<ProjectCourseInfoStruct *> mProjectCourseInfoVector;

    bool mUpdateProject;
    std::list<int> * mPrjidList;

    void runloop();
	void initProjectCourseInfo();
    ProjectCourseInfoStruct * createProjectCourseInfo(int projectid);
	ProjectCourseInfoStruct * getProjectCourseInfo(int projectid);

    void reloadCourseList();
    void reloadCourseList(ProjectCourseInfoStruct * courseInfo);
    int processDataItem(DataItem * dataItem);
    int CheckCourses();
    int CheckCourses(int projectid);
    int CheckCourses(AreaInOutEventStruct * event);
    bool CheckCourseInTime(std::string infoOfDate, CourseStruct * curCourse);
    int CheckRollCall(int projectid, std::string infoOfDate, CourseStruct * curCourse, AreaInOutEventStruct * event);
    bool update_CourseRollCallRecord(int projectid, std::string infoOfDate, CourseStruct * curCourse, std::string tagId);

    void clear_ProjectCourseInfoStruct_vector(std::vector<ProjectCourseInfoStruct*> * plist);
};

#endif
