/*
 *  AnalyzeTrackService.h
 *
 *  Created on: 2019/07/22
 *      Author: ikki
 */
#ifndef __AnalyzeTrackService_h__
#define __AnalyzeTrackService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Thread.h"
#include "../network/WorkItem.h"
#include "../mysql/DBManager.h"
#include "../DeviceManager/DeviceManager.h"

#include "../config.h"

using namespace std;

// #define ANALYZE_TRACK_PRINT_MSG

typedef struct
{
    int projectid;
    int nodetype;
    int unAnalyzeDateDatatype;
    std::string nodeid;
    std::string bufferDataDate;
    std::list<std::string> * unAnalyzeDateList;
} UnAnalyzeDateDataBufferStruct;

typedef struct
{
    int projectid;
    int nodetype;
    TrackDataType datatype;// TDT_POS, TDT_AREA...
    std::string nodeid;
    std::string FromDatetime;
    std::string ToDatetime;
    AnalyzeDayInfoStruct * dayInfo;
} AnalyzeDayInfoDataBufferStruct;

class AnalyzeTrackService : public Thread
{
public:
    AnalyzeTrackService(wqueue<WorkItem*> & trackdbqueue, DeviceManager * pDevManager, DBManager * dbManager);
    AnalyzeTrackService();
    ~AnalyzeTrackService();
    bool status();
    std::string statusString();
    void updateProject();

    void * run();

private:

    std::string mStatus;
    bool mInitial;

    wqueue<WorkItem*> & mTrackDBqueue;
    DeviceManager * mDeviceManager;
    DBManager * mDbManager;

    bool mUpdateProject;
    std::list<int> * mPrjidList;

    std::list<UnAnalyzeDateDataBufferStruct*> m_UnAnalyzeDateDataBufferList;

    std::string m_CurAnalyzeTag;
    std::string m_CurAnalyzeSensor;
    int m_CurAnalyzeUserId;
    int m_CurAnalyzeType;

    int m_dayInfoList_NotReleased;

    void runloop();
    bool analyzeTrack(long long t_start);
    bool analyzeTrack(int projectid, long long t_start);
    bool analyzeSensorTrack(int projectid, long long t_start);
    bool analyzeUserTrack(int projectid, long long t_start);
    bool analyzeTrack_OneDay();
    bool analyzeTrack_OneDay(int projectid);
    bool analyzeTagTrack(int projectid, int nodetype, const char * nodeid, const char * macaddress, long long t_start);
    bool analyzeTagSingleTypeTrack(int projectid, int nodetype, const char * nodeid, const char * macaddress, TrackDataType datatype, long long t_start);
    bool analyzeTagSingleTypeTrack_OneDay(int projectid, int nodetype, const char * nodeid, const char * macaddress, TrackDataType datatype, std::string targetDate);
    std::list<AnalyzeDayInfoStruct*> * getAnalyzeDayInfo(int projectid, int nodetype, const char * nodeid, const char * macaddress, TrackDataType datatype, std::string targetDate, std::string FromDatetime, std::string ToDatetime);
    std::list<AnalyzeDayInfoStruct*> * getCourseAnalyzeDayInfo(int projectid, const char * nodeid, std::string FromDatetime, std::string ToDatetime);
    std::list<AnalyzeDayInfoStruct*> * getEventAnalyzeDayInfo(int projectid, const char * nodeid, std::string FromDatetime, std::string ToDatetime);
    AnalyzeDayInfoStruct * getCourseAnalyzeDayRollCallInfo(int projectid, const char * nodeid, CourseRollCallRecordStruct * curRollCall);
    TagCourseDayRecordStruct * build_CourseDayRecord(int projectid, const char * nodeid, CourseStruct * course, const char * targetDate);
    std::string pack_json_course_info(TagCourseDayRecordStruct * courseDayRecord);
    std::list<std::string> * getUnAnalyzeDateList(int projectid, const char * nodeid, TrackDataType datatype);
    std::list<std::string> * prepareDateList(int StartYear, int StartMonth, int StartDay);

    std::list<std::string> * getUnAnalyzeDateListFromBuffer(int projectid, int nodetype, const char * nodeid, TrackDataType datatype);

};

#endif
