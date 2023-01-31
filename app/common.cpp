


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>
#include <sstream>
#include <iostream>
#include <fstream>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/statvfs.h>
#include <sys/param.h>
#include <sys/mount.h>
#include <net/if.h>
#include <netinet/in.h>

#include <errno.h> //For errno - the error number
#include <netdb.h> //hostent
#include <arpa/inet.h>
#include <ifaddrs.h>

#include <sys/time.h>
#include <sys/timeb.h>//timeb
#include <sys/resource.h>
#include <time.h>

#if defined(__APPLE__)
#include <mach/mach.h>
#include <mach/vm_statistics.h>
#include <mach/mach_types.h>
#include <mach/mach_init.h>
#include <mach/mach_host.h>
#include <mach/task.h>
#include <malloc/malloc.h>
#endif

#if defined(__unix__) || defined(linux)
#include <malloc.h>
#include <sys/vfs.h>
#endif

#include <openssl/engine.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>
#include <openssl/md5.h>

#include "date/date/date.h"

#include "common.h"

const char HEX2DEC[256] =
{
    /*       0  1  2  3   4  5  6  7   8  9  A  B   C  D  E  F */
    /* 0 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 1 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 2 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 3 */  0, 1, 2, 3,  4, 5, 6, 7,  8, 9,-1,-1, -1,-1,-1,-1,

    /* 4 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 5 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 6 */ -1,10,11,12, 13,14,15,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 7 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,

    /* 8 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* 9 */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* A */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* B */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,

    /* C */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* D */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* E */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    /* F */ -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1
};

// Only alphanum is safe.
const char SAFE[256] =
{
    /*      0 1 2 3  4 5 6 7  8 9 A B  C D E F */
    /* 0 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 1 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 2 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 3 */ 1,1,1,1, 1,1,1,1, 1,1,0,0, 0,0,0,0,

    /* 4 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    /* 5 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,
    /* 6 */ 0,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1,
    /* 7 */ 1,1,1,1, 1,1,1,1, 1,1,1,0, 0,0,0,0,

    /* 8 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* 9 */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* A */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* B */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,

    /* C */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* D */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* E */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    /* F */ 0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};

NodeStruct * createNodeStruct()
{
    NodeStruct * newNode = new NodeStruct();
    newNode->projectid = 1;
    newNode->index = -1;
    newNode->alive = 0;
    newNode->asleep = 0;
    newNode->nodetype = 0;// 0:Anchor 1:Tag 2:Coordinator
    strcpy(newNode->panid, "");
    strcpy(newNode->time, "");
    strcpy(newNode->nodeid, "");
    strcpy(newNode->nodename, "");
    strcpy(newNode->panid, "");
    strcpy(newNode->macaddress, "");
    strcpy(newNode->version, "");
    strcpy(newNode->rangingList, "");

    newNode->blacklist = "";
    newNode->whitelist = "";
    newNode->joinRangingRange = 20000;
    newNode->joinNotRangingRange = 30000;

    newNode->posX = 0.0;
    newNode->posY = 0.0;
    newNode->posZ = 0.0;
    newNode->prePosX = 0.0;
    newNode->prePosY = 0.0;
    newNode->prePosZ = 0.0;
    // newNode->predictPosX = 0.0;
    // newNode->predictPosY = 0.0;
    // newNode->predictPosZ = 0.0;
    newNode->smoothingPosX = 0.0;
    newNode->smoothingPosY = 0.0;
    newNode->smoothingPosZ = 0.0;
    newNode->momentumX = 0.0;
    newNode->momentumY = 0.0;
    newNode->momentumZ = 0.0;

    newNode->velocity = 0.0;

    strcpy(newNode->voltage, "0.0");
    strcpy(newNode->temperature, "0.0");
    newNode->tafid = 100;
    newNode->buildingid = -100;
    newNode->buildingfloor = -100;
    newNode->maplayer = 1;
    newNode->aliveTime = 0;
    //newNode->waitRangingCount = 5;

    strcpy(newNode->IP, "");
    strcpy(newNode->AppVersion, "");
    strcpy(newNode->PID, "");

    strcpy(newNode->AreaId, "");
    strcpy(newNode->AreaName, "");

    strcpy(newNode->AreaId0D, "");
    newNode->AreaType0D = 0;

    strcpy(newNode->AreaType5Id, ""); // 5: 病房
    strcpy(newNode->AreaType5Name, "");

    newNode->RangingResultType = -1; // -1:unknown 0:0D 1:1D 2:2D 3:3D

    newNode->alert = 0;
    newNode->alert0D = 0;
    newNode->alertIssuTime = getCurrentTimeInMilliSec();
    strcpy(newNode->LoRAMacaddress, "");

    // for websocket event
    newNode->voltage_changed = 0;
    newNode->voltage_low = 0;
    newNode->coord_changed = 0;

    // newNode->area_changed = 0;
    // strcpy(newNode->area_AreaId, "");
    // strcpy(newNode->area_AreaName, "");

    newNode->area_unknown = 1;
    // newNode->area_type = 0;
    // newNode->area_checkTimestamp = 0;

    strcpy(newNode->exclude_rollcall_AreaId, "");

    //newNode->curSectionGroup = NULL;
    //newNode->curSectionOneAnchors = NULL;
    newNode->updateTimestamp = 0;
    newNode->poweronTimestamp = 0;
    newNode->poweroffTimestamp = 0;

    newNode->enableSensorAlert = 0;

    newNode->wallArea = NULL;
    newNode->wallPosX = 0;
    newNode->wallPosY = 0;
    newNode->wallPosZ = 0;
    newNode->wallTimeStamp = 0;

    newNode->Longitude = -1;
    newNode->Latitude = -1;

    newNode->Opertaion_Mode = OPERTAION_MODE_NONE;// 0:None 1:UWB Ranging 2:LoRa Ranging 3:GPS
    newNode->pingCount = 0;

    newNode->weight = -1;
    newNode->heartbeat = -1;

    newNode->updateWeightTimestamp = 0;
    newNode->updateHeartBeatTimestamp = 0;

    newNode->gwid = "";
    newNode->resident_serno = "";
    newNode->empid = "";

    newNode->userid = 0;
    newNode->maingroupid = 1;
    newNode->verifykey = "";

    return newNode;
}

NodeStruct * createBaseNodeStruct(int index, int nodetype,
        const char * nodeid, const char *macaddress,
        double posX, double posY, double posZ, const char * version,
        int tafid, int buildingid, int buildingfloor)
{
    NodeStruct * newNode = createNodeStruct();
    newNode->projectid = 1;
    newNode->index = index;
    newNode->nodetype = nodetype;// 0:Anchor 1:Tag 2:Coordinator
    strcpy(newNode->nodeid, nodeid);
    strcpy(newNode->nodename, nodeid);
    strcpy(newNode->macaddress, macaddress);
    strcpy(newNode->version, version);
    newNode->posX = posX;
    newNode->posY = posY;
    newNode->posZ = posZ;
    newNode->tafid = tafid;
    newNode->buildingid = buildingid;
    newNode->buildingfloor = buildingfloor;
    return newNode;
}

NodeStruct * copyNodeStruct(NodeStruct * srcNode)
{
    NodeStruct * newNode = createNodeStruct();

    newNode->nodetype = srcNode->nodetype;
    snprintf(newNode->nodeid, 32, "%s", srcNode->nodeid);
    snprintf(newNode->nodename, 32, "%s", srcNode->nodename);
    snprintf(newNode->panid, 32, "%s", srcNode->panid);
    snprintf(newNode->time, 32, "%s", srcNode->time);
    snprintf(newNode->macaddress, 32, "%s", srcNode->macaddress);

    newNode->tafid = srcNode->tafid;
    newNode->buildingid = srcNode->buildingid;
    newNode->buildingfloor = srcNode->buildingfloor;
    newNode->maplayer = srcNode->maplayer;

    newNode->posX = srcNode->posX;
    newNode->posY = srcNode->posY;
    newNode->posZ = srcNode->posZ;
    newNode->prePosX = srcNode->prePosX;
    newNode->prePosY = srcNode->prePosY;
    newNode->prePosZ = srcNode->prePosZ;
    newNode->smoothingPosX = srcNode->smoothingPosX;
    newNode->smoothingPosY = srcNode->smoothingPosY;
    newNode->smoothingPosZ = srcNode->smoothingPosZ;
    newNode->momentumX = srcNode->momentumX;
    newNode->momentumY = srcNode->momentumY;
    newNode->momentumZ = srcNode->momentumZ;
    newNode->velocity = srcNode->velocity;

    newNode->index = srcNode->index;
    newNode->alive = srcNode->alive;
    newNode->asleep = srcNode->asleep;

    snprintf(newNode->voltage, 32, "%s", srcNode->voltage);
    snprintf(newNode->temperature, 32, "%s", srcNode->temperature);
    snprintf(newNode->version, 32, "%s", srcNode->version);
    snprintf(newNode->rangingList, 128, "%s", srcNode->rangingList);
    newNode->blacklist = srcNode->blacklist;
    newNode->whitelist = srcNode->whitelist;
    newNode->joinRangingRange = srcNode->joinRangingRange;
    newNode->joinNotRangingRange = srcNode->joinNotRangingRange;
    newNode->aliveTime = srcNode->aliveTime;

    snprintf(newNode->IP, 32, "%s", srcNode->IP);
    snprintf(newNode->AppVersion, 32, "%s", srcNode->AppVersion);
    snprintf(newNode->PID, 32, "%s", srcNode->PID);

    //snprintf(newNode->AreaId, 32, "%s", srcNode->AreaId);
    //snprintf(newNode->AreaName, 256, "%s", srcNode->AreaName);

    //snprintf(newNode->AreaId0D, 32, "%s", srcNode->AreaId0D);
    //newNode->AreaType0D = srcNode->AreaType0D;

    //snprintf(newNode->AreaType5Id, 32, "%s", srcNode->AreaType5Id);
    //snprintf(newNode->AreaType5Name, 256, "%s", srcNode->AreaType5Name);

    newNode->RangingResultType = srcNode->RangingResultType;
    //newNode->area_unknown = srcNode->area_unknown;
    //snprintf(newNode->exclude_rollcall_AreaId, 32, "%s", srcNode->exclude_rollcall_AreaId);

    //newNode->alert = srcNode->alert;
    //newNode->alert0D = srcNode->alert0D;
    //newNode->alertIssuTime = srcNode->alertIssuTime;

    snprintf(newNode->LoRAMacaddress, 32, "%s", srcNode->LoRAMacaddress);

    // for websocket event
    //newNode->voltage_changed = srcNode->voltage_changed;
    //newNode->voltage_low = srcNode->voltage_low;
    //newNode->coord_changed = srcNode->coord_changed;

    newNode->updateTimestamp = srcNode->updateTimestamp;
    newNode->poweronTimestamp = srcNode->poweronTimestamp;
    newNode->poweroffTimestamp = srcNode->poweroffTimestamp;

    // for Wall
    //newNode->wallPosX;
    //newNode->wallPosY;
    //newNode->wallPosZ;
    //newNode->wallTimeStamp;

    newNode->Longitude = srcNode->Longitude;
    newNode->Latitude = srcNode->Latitude;
    if ( (int)newNode->Longitude == 0 )
        newNode->Longitude = -1;
    if ( (int)newNode->Latitude == 0 )
        newNode->Latitude = -1;

    newNode->Opertaion_Mode = srcNode->Opertaion_Mode;
    newNode->pingCount = srcNode->pingCount;

    //newNode->weight = srcNode->weight;
    //newNode->heartbeat = srcNode->heartbeat;
    newNode->rangingTargetid = srcNode->rangingTargetid;// 與誰在測距

    //newNode->updateWeightTimestamp = srcNode->updateWeightTimestamp;
    //newNode->updateHeartBeatTimestamp = srcNode->updateHeartBeatTimestamp;

    //newNode->enableSensorAlert = srcNode->enableSensorAlert;

    newNode->gwid = srcNode->gwid;

    //newNode->resident_serno = srcNode->resident_serno;
    //newNode->empid = srcNode->empid;

    //newNode->userid = srcNode->userid; // link to Table User.userid

    return newNode;
}

void copyUserStruct(UserStruct * srcUser, UserStruct * dstUser)
{
    dstUser->userid = srcUser->userid;
    dstUser->groupid = srcUser->groupid;
    dstUser->type = srcUser->type;
    dstUser->empid = srcUser->empid;
    dstUser->account = srcUser->account;
    dstUser->name = srcUser->name;
    dstUser->unit = srcUser->unit;
    dstUser->title = srcUser->title;
    dstUser->gender = srcUser->gender;
    dstUser->pwd = srcUser->pwd;
    dstUser->datetime = srcUser->datetime;

    dstUser->maingroups = copyStringList(srcUser->maingroups);
    dstUser->datagroups = copyStringList(srcUser->datagroups);
    dstUser->subgroups = copyStringList(srcUser->subgroups);
    dstUser->rfidcards = copyStringList(srcUser->rfidcards);
    dstUser->nfccards = copyStringList(srcUser->nfccards);
    dstUser->uwbcards = copyStringList(srcUser->uwbcards);
    dstUser->sensors = copyStringList(srcUser->sensors);
    dstUser->imageuid = srcUser->imageuid;

    dstUser->maplayer = srcUser->maplayer;
    dstUser->posX = srcUser->posX;
    dstUser->posY = srcUser->posY;
    dstUser->posZ = srcUser->posZ;
    dstUser->Longitude = srcUser->Longitude;
    dstUser->Latitude = srcUser->Latitude;
}

AnalyzeDayInfoStruct * createAnalyzeDayInfoStruct()
{
    AnalyzeDayInfoStruct * dayInfo = new AnalyzeDayInfoStruct();
    dayInfo->count = 0;
    dayInfo->nodeid = "";
    dayInfo->max_time = "";
    dayInfo->min_time = "";
    dayInfo->day = "";
    dayInfo->area_ids = "";
    dayInfo->area_names = "";
    dayInfo->course_id = "1";
    dayInfo->course_info = "";
    dayInfo->cache_max_time = "";
    dayInfo->cache_min_time = "";


    return dayInfo;
}

VILSConfig * createVILSConfig()
{
    VILSConfig * tmpVILSConfig = new VILSConfig();

    tmpVILSConfig->projectid = 1;
    tmpVILSConfig->projectname = "PROJECT_NAME";
    tmpVILSConfig->projectdesc = "PROJECT_DESCRIPTION";
    tmpVILSConfig->WGS48_Origin_X = "0.0";
    tmpVILSConfig->WGS48_Origin_Y = "0.0";
    tmpVILSConfig->MAP_North = "0.0";
    tmpVILSConfig->BuildingID = 1;
    tmpVILSConfig->BuildingFloor = 1;
    tmpVILSConfig->datetime = currentDateTimeForSQL();
    return tmpVILSConfig;
}

// Get current date/time, format is YYYY-MM-DD HH:mm:ss
std::string currentDateTimeForSQL()
{
    char buf[30];
    struct timeval tv;

    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime=tv.tv_sec;

    strftime(buf, 30,"%Y-%m-%d %H:%M:%S",localtime(&curtime));
    //strftime(buffer,30,"%T.",localtime(&curtime));
    //sprintf(buf, "%s%ld",buffer,tv.tv_usec/1000);

    //printf("currentDateTimeForSQL=%s\n",buf);

    return buf;
}

std::string getCurrentDateTimeInMilliSec()
{
    char buf[64];
    char milliSec[30];
    struct timeval tv;

    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime = tv.tv_sec;

    snprintf(milliSec, 30, "%03d", (int)(tv.tv_usec) / 1000);
    strftime(buf, 64,"%Y-%m-%d %H:%M:%S.", localtime(&curtime));

    strcat(buf, milliSec);

    return buf;
}

std::string currentDateTimeISO8601()
{
    char buf[30];
    struct timeval tv;

    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime = tv.tv_sec;

    strftime(buf, 30,"%Y-%m-%dT%T",gmtime(&curtime));

    return buf;
}

std::string currentDateTimeForReport()
{
    char buf[30];
    struct timeval tv;

    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime=tv.tv_sec;

    strftime(buf, 30,"%Y_%m_%d_%H_%M_%S",localtime(&curtime));

    return buf;
}

std::string currentDateTimeForESP32()
{
    char buf[30];
    struct timeval tv;

    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime=tv.tv_sec;

    strftime(buf, 30,"%Y,%m,%d,%H,%M,%S",localtime(&curtime));

    return buf;
}

std::string BeforeMinDateTimeForSQL(int beforeInMin)
{
    char buf[30];
    struct timeval tv;

    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime=tv.tv_sec - (beforeInMin * 60);

    strftime(buf, 30,"%Y-%m-%d %H:%M:%S",localtime(&curtime));

    return buf;
}

std::string currentDateTimeForSQL(int beforeInDay)
{
    char buf[30];
    struct timeval tv;

    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime=tv.tv_sec - (beforeInDay * 86400);

    strftime(buf, 30,"%Y-%m-%d %H:%M:%S",localtime(&curtime));

    return buf;
}

std::string currentDateTimeForPushNotification()
{
    char buf[30];
    struct timeval tv;

    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime=tv.tv_sec;

    strftime(buf, 30,"%Y%m%d%H%M%S",localtime(&curtime));

    return buf;
}

std::string currentDateTimeForPushNotification(int beforeInDay)
{
    char buf[30];
    struct timeval tv;

    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime=tv.tv_sec - (beforeInDay * 86400);

    strftime(buf, 30,"%Y%m%d%H%M%S",localtime(&curtime));

    return buf;
}

std::string createDateTimeForSQL(struct timeval tv)
{
    char buf[30];
    time_t curtime;
    curtime = tv.tv_sec;
    strftime(buf, 30,"%Y-%m-%d %H:%M:%S", localtime(&curtime));
    return buf;
}

std::string currentTimeForSQL()
{
    char buf[30];
    struct timeval tv;

    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime=tv.tv_sec;

    strftime(buf, 30,"%H:%M",localtime(&curtime));
    return buf;
}

std::string currentTimeWithSecForSQL()
{
    char buf[30];
    struct timeval tv;

    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime=tv.tv_sec;

    strftime(buf, 30,"%H:%M:%S",localtime(&curtime));
    return buf;
}

std::string currentTimeForSQL(int beforeInMin)
{
    char buf[30];
    struct timeval tv;

    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime=tv.tv_sec - (beforeInMin * 60);

    strftime(buf, 30,"%H:%M",localtime(&curtime));
    return buf;
}

std::string currentSimpleTime()
{
    char buf[30];
    struct timeval tv;

    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime=tv.tv_sec;

    strftime(buf, 30,"%H%M", localtime(&curtime));
    return buf;
}

std::string convertSimpleTime(time_t curtime)
{
    char buf[30];
    strftime(buf, 30,"%H%M", localtime(&curtime));
    return buf;
}

std::string currentDateTime()
{
    char       buf[80];
    char buffer[30];
    struct timeval tv;

    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime=tv.tv_sec;

    //strftime(buffer,30,"%m-%d-%Y  %T.",localtime(&curtime));
    strftime(buffer,30,"%T.",localtime(&curtime));
    sprintf(buf, "%s%d",buffer, (int)(tv.tv_usec/1000));

    return buf;
}

std::string TodayDate()
{
    char buf[30];
    struct timeval tv;

    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime = tv.tv_sec;
    strftime(buf, 30,"%Y%m%d", localtime(&curtime));
    return buf;
}

std::string TodayDateType1()
{
    char buf[30];
    struct timeval tv;

    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime = tv.tv_sec;
    strftime(buf, 30,"%Y-%m-%d", localtime(&curtime));
    return buf;
}

std::string TodayDateType1(int beforeInDay)
{
    char buf[30];
    struct timeval tv;

    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime = tv.tv_sec - (beforeInDay * 86400);
    strftime(buf, 30,"%Y-%m-%d", localtime(&curtime));
    return buf;
}

std::string TodayYear(int beforeInDay)
{
    char buf[30];
    struct timeval tv;

    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime = tv.tv_sec - (beforeInDay * 86400);
    strftime(buf, 30,"%Y", localtime(&curtime));
    return buf;
}

std::string TodayMonth(int beforeInDay)
{
    char buf[30];
    struct timeval tv;

    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime = tv.tv_sec - (beforeInDay * 86400);
    strftime(buf, 30,"%m", localtime(&curtime));
    return buf;
}

std::string TodayDay(int beforeInDay)
{
    char buf[30];
    struct timeval tv;

    time_t curtime;

    gettimeofday(&tv, NULL);
    curtime = tv.tv_sec - (beforeInDay * 86400);
    strftime(buf, 30,"%d", localtime(&curtime));
    return buf;
}

int TodayDateWeekday()
{
    time_t rawtime;
    tm * timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    int weekday = timeinfo->tm_wday;
    return weekday;
}

time_t getCurrentTimeInSec()
{
    time_t latest_time;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    latest_time = tv.tv_sec;
    return latest_time;
}

time_t getCurrentTimeInMilliSec()
{
    time_t latest_time;
    struct timeval tv;
    gettimeofday(&tv, NULL);
    latest_time = (tv.tv_sec) * 1000 + (tv.tv_usec) / 1000 ; // convert tv_sec & tv_usec to millisecond
    return latest_time;
}

time_t StringToCheckPointDatetime(string str)
{
    char *cha = (char*)str.data();
    tm tm_;
    int hour, minute;
    char AMPM[32];
    // 04:00 PM
    sscanf(cha, "%d:%d %s", &hour, &minute, &AMPM[0]);

    //printf("StringToDatetime() [%s] -> hour[%d] minute[%d] AMPM[%s]\n", str.c_str(), hour, minute, AMPM);

    time_t curtime;
    time(&curtime);
    //localtime(&curtime);
    struct tm * curtm = gmtime(&curtime);  //UTC time

    //printf("StringToDatetime() curtm->tm_mday[%d] curtm->tm_hour[%d]\n", curtm->tm_mday, curtm->tm_hour);

    if (curtm->tm_hour+8 > 23)
    {
        curtm->tm_mday += 1;
        curtm->tm_hour = 0;
    }

    // convert to 24 hours
    // AM 12:00  -> 00:00
    // AM 01:00  -> 01:00
    // AM 11:00  -> 11:00
    // PM 12:00  -> 12:00
    // PM 01:00  -> 13:00
    // PM 11:00  -> 23:00
    if (strcmp(AMPM, "PM") == 0)
    {
        if (hour != 12)
            hour += 12;
    }
    if (strcmp(AMPM, "AM") == 0)
    {
        if (hour == 12)
            hour = 0;
    }

    tm_.tm_year = curtm->tm_year;                 // 年，由于tm结构体存储的是从1900年开始的时间，所以tm_year为int临时变量减去1900。
    tm_.tm_mon = curtm->tm_mon;                    // 月，由于tm结构体的月份存储范围为0-11，所以tm_mon为int临时变量减去1。
    tm_.tm_mday = curtm->tm_mday;                         // 日。
    tm_.tm_hour = hour;                        // 时。 0 ~ 23
    tm_.tm_min = minute;                       // 分。
    tm_.tm_sec = 0;                       // 秒。
    tm_.tm_isdst = 0;                          // 非夏令时。

    //printf("StringToDatetime() [%s] -> hour[%d] minute[%d] AMPM[%s]\n", str.c_str(), hour, minute, AMPM);
    //printf("StringToDatetime() tm_year[%d] tm_mon[%d] tm_mday[%d]\n", tm_.tm_year, tm_.tm_mon, tm_.tm_mday);


    time_t t_ = mktime(&tm_);                  // 将tm结构体转换成time_t格式。
    return t_;                                 // 返回值。
}

time_t StringToInspectionDatetime(string str)
{
    char *cha = (char*)str.data();
    tm tm_;
    int hour, minute;
    char AMPM[32];
    // 04:00 PM
    sscanf(cha, "%d:%d %s", &hour, &minute, &AMPM[0]);

    // printf("StringToDatetime() [%s] -> hour[%d] minute[%d] AMPM[%s]\n", str.c_str(), hour, minute, AMPM);

    time_t curtime;
    time(&curtime);
    //localtime(&curtime);
    struct tm * curtm = gmtime(&curtime);  //UTC time

    // printf("StringToDatetime() curtm->tm_mday[%d] curtm->tm_hour[%d]\n", curtm->tm_mday, curtm->tm_hour);

    if (curtm->tm_hour+8 > 23)
    {
        curtm->tm_mday += 1;
        curtm->tm_hour = 0;
    }

    // convert to 24 hours
    // AM 12:00  -> 00:00
    // AM 01:00  -> 01:00
    // AM 11:00  -> 11:00
    // PM 12:00  -> 12:00
    // PM 01:00  -> 13:00
    // PM 11:00  -> 23:00
    if (strcmp(AMPM, "PM") == 0)
    {
        if (hour != 12)
            hour += 12;
    }
    if (strcmp(AMPM, "AM") == 0)
    {
        if (hour == 12)
            hour = 0;
    }

    // printf("StringToDatetime() hour[%d]\n", hour);

    tm_.tm_year = curtm->tm_year;                 // 年，由于tm结构体存储的是从1900年开始的时间，所以tm_year为int临时变量减去1900。
    tm_.tm_mon = curtm->tm_mon;                    // 月，由于tm结构体的月份存储范围为0-11，所以tm_mon为int临时变量减去1。
    tm_.tm_mday = curtm->tm_mday;                         // 日。
    tm_.tm_hour = hour;                        // 时。 0 ~ 23
    tm_.tm_min = minute;                       // 分。
    tm_.tm_sec = 0;                       // 秒。
    tm_.tm_isdst = 0;                          // 非夏令时。

    //printf("StringToDatetime() [%s] -> hour[%d] minute[%d] AMPM[%s]\n", str.c_str(), hour, minute, AMPM);
    //printf("StringToDatetime() tm_year[%d] tm_mon[%d] tm_mday[%d]\n", tm_.tm_year, tm_.tm_mon, tm_.tm_mday);


    time_t t_ = mktime(&tm_);                  // 将tm结构体转换成time_t格式。
    return t_;                                 // 返回值。
}

std::string time_tToString(time_t t)
{
    if ( t <= 0 )
       return "";

    char       buf[80];
    char buffer[30];

    tzset();

    //strftime(buffer,30,"%m-%d-%Y  %T.",localtime(&curtime));
    //strftime(buffer,30,"%Y/%m/%d  %T",localtime(&t));
    strftime(buffer, 30, "%Y-%m-%d %H:%M:%S", localtime(&t));
    sprintf(buf, "%s", buffer);
    return buf;
}

std::string time_tToTimeString(time_t t)
{
    if ( t <= 0 )
       return "";

    char       buf[80];
    char buffer[30];

    tzset();

    //strftime(buffer,30,"%m-%d-%Y  %T.",localtime(&curtime));
    //strftime(buffer,30,"%Y/%m/%d  %T",localtime(&t));
    strftime(buffer, 30, "%H:%M:%S", localtime(&t));
    sprintf(buf, "%s", buffer);
    return buf;
}

std::string time_tToTimerString(time_t t)
{
    char buffer[30];

    int ONE_MIN_SEC = 60;
    int ONE_HOUR_SEC = 3600;
    //int ONE_DAY_SEC = 86400;

    //int days = t / ONE_DAY_SEC;
    //t -= ONE_DAY_SEC * days;
    int hours = (int)(t / ONE_HOUR_SEC);
    t -= ONE_HOUR_SEC * hours;
    int mins = (int)(t / ONE_MIN_SEC);
    t -= ONE_MIN_SEC * mins;

    snprintf(buffer, 30, "%02d:%02d:%02d", hours, mins, (int)t);

    std::string timer_str = buffer;
    return timer_str;
}

std::string time_tToHumanReadString(time_t t)
{
    int ONE_MIN_SEC = 60;
    int ONE_HOUR_SEC = 3600;
    int ONE_DAY_SEC = 86400;

    int days = 0;
    int hours = 0;
    int mins = 0;

    if ( t > ONE_DAY_SEC )
    {
        days = (int)(t / ONE_DAY_SEC);
        t -= ONE_DAY_SEC * days;
    }

    if ( t > ONE_HOUR_SEC )
    {
        hours = (int)(t / ONE_HOUR_SEC);
        t -= ONE_HOUR_SEC * hours;
    }

    if ( t > ONE_MIN_SEC )
    {
        mins = (int)(t / ONE_MIN_SEC);
        t -= ONE_MIN_SEC * mins;
    }

    std::ostringstream sendstream;

    if ( days > 0 )
        sendstream<<days<< "天 ";
    if ( hours > 0 )
        sendstream<<hours<< "小時 ";
    if ( mins > 0 )
        sendstream<<mins<< "分鐘 ";
    if ( t > 0 )
        sendstream<<t<< "秒 ";

    std::string timer_str = sendstream.str();
    return timer_str;
}

std::string convertToShowDate(std::string str_time)
{
    char       buf[80];
    char buffer[30];

    if (str_time.length() == 0)
        return "";

    struct tm tm = {0};
    strptime(str_time.c_str(), "%Y%m%d", &tm);
    tm.tm_isdst = 0; //without daylight savings
    time_t t = mktime(&tm);

    tzset();
    strftime(buffer, 30, "%Y-%m-%d", localtime(&t));
    sprintf(buf, "%s", buffer);
    return buf;
}

std::string StringDateTimeToStringDate(std::string str_time)
{
    char       buf[80];
    char buffer[30];

    if (str_time.length() == 0)
        return "";

    struct tm tm = {0};
    strptime(str_time.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
    tm.tm_isdst = 0; //without daylight savings
    time_t t = mktime(&tm);

    strftime(buffer, 30, "%Y-%m-%d", localtime(&t));
    sprintf(buf, "%s", buffer);
    return buf;
}

time_t StringDateTimeTotime_t(std::string str_time)
{
    if (str_time.length() == 0)
        return 0;

    struct tm tm = {0};
    strptime(str_time.c_str(), "%Y-%m-%d %H:%M:%S", &tm);
    tm.tm_isdst = 0; //without daylight savings
    time_t t = mktime(&tm);
    return t;
}

time_t StringSimpleTimeTotime_t(std::string str_time)
{
    if (str_time.length() == 0)
        return 0;

    time_t curtime;
    time(&curtime);
    //localtime(&curtime);
    struct tm * curtm = gmtime(&curtime);  //UTC time

    strptime(str_time.c_str(), "%H%M", curtm);
    curtm->tm_sec = 0;

    curtm->tm_isdst = 0; //without daylight savings
    time_t t = mktime(curtm);
    return t;
}

time_t StringUTCDateTimeTotime_t(const char * str_time)
{
    if (strlen(str_time) == 0)
        return 0;

    struct tm tm = {0};
    strptime(str_time, "%Y-%m-%dT%H:%M:%S", &tm);

    tm.tm_isdst = -1;
    time_t t2 = mktime(&tm); // From localtime:
    //time_t t2 = timegm(&tm); // From UTC/GMT:

    return t2;
}

time_t StringTimeZoneDateTimeTotime_t(const char * str_time)
{
    if (strlen(str_time) == 0)
        return 0;

    istringstream in{str_time};
    date::sys_seconds tp;
    if ( strlen(str_time) > 19 )
        in >> date::parse("%Y-%m-%dT%H:%M:%S%z", tp);
    else
        in >> date::parse("%Y-%m-%dT%H:%M:%S", tp);
    std::time_t now_c = std::chrono::system_clock::to_time_t(tp);

    return now_c;
}

time_t StringDateTotime_t(std::string str_time, int addDays)
{
    if (str_time.length() == 0)
        return 0;

    struct tm tm = {0};
    strptime(str_time.c_str(), "%Y-%m-%d", &tm);
    tm.tm_mday += addDays;
    tm.tm_isdst = 0; //without daylight savings
    time_t t = mktime(&tm);
    return t;
}

time_t StringTimeTotime_t(std::string str_time, int addMinutes)
{
    if (str_time.length() == 0)
        return 0;

    //struct tm tm = {0};
    time_t curtime;
    time(&curtime);
    //localtime(&curtime);
    struct tm * curtm = gmtime(&curtime);  //UTC time

    strptime(str_time.c_str(), "%H:%M", curtm);
    if(addMinutes > 0)
    {
        curtm->tm_min += addMinutes;                       // 分。
    }
    curtm->tm_sec = 0;

    curtm->tm_isdst = 0; //without daylight savings
    time_t t = mktime(curtm);
    return t;
}

void clear_ranging_vector(std::vector<AnchorTagCoupleDataClass*> * plist)
{
    //printf("common::clear_ranging_vector() start\n");
    for (std::vector<AnchorTagCoupleDataClass*>::iterator i = plist->begin(); i != plist->end(); i++)
    	delete *i;
    plist->clear();
    //printf("common::clear_ranging_vector() end\n");
}

void clear_AreaInOut_vector(std::vector<AreaInOutDataClass*> * plist)
{
    //printf("common::clear_ranging_vector() start\n");
    for (std::vector<AreaInOutDataClass*>::iterator i = plist->begin(); i != plist->end(); i++)
    	delete *i;
    plist->clear();
    //printf("common::clear_ranging_vector() end\n");
}

void clear_TagInOut_vector(std::vector<TagInOutDataClass*> * plist)
{
    //printf("common::clear_ranging_vector() start\n");
    for (std::vector<TagInOutDataClass*>::iterator i = plist->begin(); i != plist->end(); i++)
    	delete *i;
    plist->clear();
    //printf("common::clear_ranging_vector() end\n");
}

void clear_SectionDeviceInfo_vector(std::vector<SectionDeviceInfo*> * plist)
{
    //printf("common::clear_ranging_vector() start\n");
    for (std::vector<SectionDeviceInfo*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        (*i)->nodeIDList.clear();
        delete *i;
    }
    plist->clear();
    //printf("common::clear_ranging_vector() end\n");
}

void clear_SectionGroup_list(std::list<SectionGroup*> * plist)
{
    //printf("common::clear_SectionGroup_list() start\n");
    for (std::list<SectionGroup*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        (*i)->nodeIDList->clear();
        delete (*i)->nodeIDList;
        delete *i;
    }
    plist->clear();
    //printf("common::clear_SectionGroup_list() end\n");
}

void clear_node_list(std::list<NodeStruct*> * plist)
{
    //printf("common::clear_node_list() start\n");
    if (plist == NULL)
        return;
    for (std::list<NodeStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        // if ((*i)->curSectionOneAnchors != NULL)
        // {
        //     (*i)->curSectionOneAnchors->clear();
        //     delete (*i)->curSectionOneAnchors;
        // }
        delete *i;
    }
    plist->clear();
    //printf("common::clear_node_list() end\n");
}

void clear_LoRaGateway_list(std::list<LoRaGatewayInfoStruct*> * plist)
{
    //printf("common::clear_node_list() start\n");
    if (plist == NULL)
        return;
    for (std::list<LoRaGatewayInfoStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        (*i)->channels.clear();
        delete *i;
    }
    plist->clear();
    //printf("common::clear_node_list() end\n");
}

void clear_FootprintAll_list(std::list<FootprintAllStruct*> * plist)
{
    //printf("common::clear_FootprintAll_list() start\n");
    for (std::list<FootprintAllStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    	delete *i;
    plist->clear();
    //printf("common::clear_FootprintAll_list() end\n");
}

void clear_Config_list(std::list<VILSConfig*> * plist)
{
    //printf("common::clear_FootprintAll_list() start\n");
    for (std::list<VILSConfig*>::iterator i = plist->begin(); i != plist->end(); i++)
    	delete *i;
    plist->clear();
    //printf("common::clear_FootprintAll_list() end\n");
}

void clear_CheckPointGroup_list(std::list<CheckPointGroupStruct*> * plist)
{
    //printf("common::clear_FootprintAll_list() start\n");
    for (std::list<CheckPointGroupStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    	delete *i;
    plist->clear();
    //printf("common::clear_FootprintAll_list() end\n");
}

void clear_CheckPointPath_list(std::list<CheckPointPathStruct*> * plist)
{
    //printf("common::clear_FootprintAll_list() start\n");
    for (std::list<CheckPointPathStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    	delete *i;
    plist->clear();
    //printf("common::clear_FootprintAll_list() end\n");
}

void clear_CheckPointGroupDailyRecord_list(std::list<CheckPointGroupDailyRecord*> * plist)
{
    //printf("common::clear_FootprintAll_list() start\n");
    for (std::list<CheckPointGroupDailyRecord*>::iterator i = plist->begin(); i != plist->end(); i++)
    	delete *i;
    plist->clear();
    //printf("common::clear_FootprintAll_list() end\n");
}

void clear_CheckPointPathDailyRecord_list(std::list<CheckPointPathDailyRecord*> * plist)
{
    //printf("common::clear_FootprintAll_list() start\n");
    for (std::list<CheckPointPathDailyRecord*>::iterator i = plist->begin(); i != plist->end(); i++)
    	delete *i;
    plist->clear();
    //printf("common::clear_FootprintAll_list() end\n");
}

void clear_CheckPointTagBehaviorData_list(std::list<CheckPointTagBehaviorData*> * plist)
{
    //printf("common::clear_FootprintAll_list() start\n");
    for (std::list<CheckPointTagBehaviorData*>::iterator i = plist->begin(); i != plist->end(); i++)
    	delete *i;
    plist->clear();
    //printf("common::clear_FootprintAll_list() end\n");
}

void clear_InspectionGroup_list(std::list<InspectionGroupStruct*> * plist)
{
    for (std::list<InspectionGroupStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        (*i)->Inspectors->clear();
        delete (*i)->Inspectors;
        delete *i;
    }
    plist->clear();
}

void clear_InspectionPoint_list(std::list<InspectionPointStruct*> * plist)
{
    for (std::list<InspectionPointStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

// void clear_AnchorRanging_vector(std::vector<CheckPointAnchorRangingDataClass*> * plist)
// {
//     //printf("common::clear_FootprintAll_list() start\n");
//     for (std::vector<CheckPointAnchorRangingDataClass*>::iterator i = plist->begin(); i != plist->end(); i++)
//     	delete *i;
//     plist->clear();
//     //printf("common::clear_FootprintAll_list() end\n");
// }

void clear_InspectionAnchorRanging_vector(std::vector<InspectionAnchorRangingDataClass*> * plist)
{
    //printf("common::clear_InspectionAnchorRanging_vector() start\n");
    for (std::vector<InspectionAnchorRangingDataClass*>::iterator i = plist->begin(); i != plist->end(); i++)
    	delete *i;
    plist->clear();
    //printf("common::clear_InspectionAnchorRanging_vector() end\n");
}

void clear_AnchorRangingDataClass_vector(std::vector<AnchorRangingDataClass*> * plist)
{
    //printf("common::clear_AnchorRangingDataClass_vector() start\n");
    for (std::vector<AnchorRangingDataClass*>::iterator i = plist->begin(); i != plist->end(); i++)
    	delete *i;
    plist->clear();
    //printf("common::clear_AnchorRangingDataClass_vector() end\n");
}

void clear_FiredEventStruct_list(std::list<FiredEventStruct*> * plist)
{
    //printf("common::clear_FiredEventStruct_list() start\n");
    for (std::list<FiredEventStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    	delete *i;
    plist->clear();
    //printf("common::clear_FiredEventStruct_list() end\n");
}

void clear_Pair_FiredEventStruct_list(std::list<Pair_FiredEventStruct*> * plist)
{
    for (std::list<Pair_FiredEventStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        clear_Pair_FiredEventStruct((*i));
        delete *i;
    }
    plist->clear();
}

void clear_Pair_FiredEventStruct(Pair_FiredEventStruct * event)
{
    if ( event->event1 != NULL )
        delete event->event1;
    if ( event->event2 != NULL )
        delete event->event2;
    if ( event->events != NULL )
    {
        clear_FiredEventStruct_list(event->events);
        delete event->events;
    }
}

void clear_Course_list(std::list<CourseStruct*> * plist)
{
    for (std::list<CourseStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        if ((*i)->daysOfWeek != NULL)
        {
            (*i)->daysOfWeek->clear();
            delete (*i)->daysOfWeek;
        }
        delete *i;
    }
    plist->clear();
}

void clear_NodeGPSInfo_list(std::list<NodeGPSInfoStruct*> * plist)
{
    for (std::list<NodeGPSInfoStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        for (std::list<GPSInfoStruct*>::iterator info = (*i)->GPSInfoList.begin(); info != (*i)->GPSInfoList.end(); info++)
        {
            delete *info;
        }
        (*i)->GPSInfoList.clear();
        delete *i;
    }
    plist->clear();
}

void clear_NodePositionInfo_list(std::list<NodePositionInfoStruct*> * plist)
{
    for (std::list<NodePositionInfoStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        for (std::list<PositionInfoStruct*>::iterator info = (*i)->POSInfoList.begin(); info != (*i)->POSInfoList.end(); info++)
        {
            delete *info;
        }
        (*i)->POSInfoList.clear();
        delete *i;
    }
    plist->clear();
}

void clear_NodeItemStruct_list(std::list<NodeItemStruct*> * plist)
{
    for (std::list<NodeItemStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_CourseRollCallRecordStruct_list(std::list<CourseRollCallRecordStruct*> * plist)
{
    for (std::list<CourseRollCallRecordStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        if ((*i)->tags != NULL)
        {
            (*i)->tags->clear();
            delete (*i)->tags;
        }
        delete *i;
    }
    plist->clear();
}

void clear_AnalyzeDayInfoStruct_list(std::list<AnalyzeDayInfoStruct*> * plist)
{
    for (std::list<AnalyzeDayInfoStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_HttpHeader_list(std::list<HttpHeader*> * plist)
{
    for (std::list<HttpHeader*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_DayRecordStruct_list(std::list<DayRecordStruct*> * plist)
{
    for (std::list<DayRecordStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        if ( (*i)->NodeIDList != NULL )
        {
            (*i)->NodeIDList->clear();
            delete (*i)->NodeIDList;
        }
        delete *i;
    }
    plist->clear();
}

void clear_DeviceAliveCountStruct_list(std::list<DeviceAliveCountStruct*> * plist)
{
    for (std::list<DeviceAliveCountStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_AreaActivityTimeStruct_list(std::list<AreaActivityTimeStruct*> * plist)
{
    for (std::list<AreaActivityTimeStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_EmployeeEventReportStruct_list(std::list<EmployeeEventReportStruct*> * plist)
{
    for (std::list<EmployeeEventReportStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_UserStruct(UserStruct * user)
{
    if ( user->maingroups != NULL )
    {
        user->maingroups->clear();
        delete user->maingroups;
    }
    if ( user->datagroups != NULL )
    {
        user->datagroups->clear();
        delete user->datagroups;
    }
    if ( user->subgroups != NULL )
    {
        user->subgroups->clear();
        delete user->subgroups;
    }
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
    if ( user->sensors != NULL )
    {
        user->sensors->clear();
        delete user->sensors;
    }
}

void clear_UserStruct_list(std::list<UserStruct*> * plist)
{
    for (std::list<UserStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        clear_UserStruct((*i));

        delete (*i);
    }
    plist->clear();
}

void clear_XLSXUserStruct_list(std::list<XLSXUserStruct*> * plist)
{
    for (std::list<XLSXUserStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete (*i);
    }
    plist->clear();
}

void clear_UserGroupStruct_list(std::list<UserGroupStruct*> * plist)
{
    for (std::list<UserGroupStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete (*i);
    }
    plist->clear();
}

void clear_UserSubGroupStruct(UserSubGroupStruct * item)
{
    if(item->accountidList != NULL)
    {
        item->accountidList->clear();
        delete item->accountidList;
    }
    if(item->useridList != NULL)
    {
        item->useridList->clear();
        delete item->useridList;
    }
}

void clear_UserSubGroupStruct_list(std::list<UserSubGroupStruct*> * plist)
{
    for (std::list<UserSubGroupStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        clear_UserSubGroupStruct(*i);
        delete (*i);
    }
    plist->clear();
}

void clear_UserMainGroupStruct(UserMainGroupStruct * item)
{
    if(item->accountidList != NULL)
    {
        item->accountidList->clear();
        delete item->accountidList;
    }
    if(item->useridList != NULL)
    {
        item->useridList->clear();
        delete item->useridList;
    }
}

void clear_UserMainGroupStruct_list(std::list<UserMainGroupStruct*> * plist)
{
    for (std::list<UserMainGroupStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        clear_UserMainGroupStruct(*i);
        delete (*i);
    }
    plist->clear();
}

void clear_LoRaCompressMultipleDistanceStruct_list(std::list<LoRaCompressMultipleDistanceStruct*> * plist)
{
    for (std::list<LoRaCompressMultipleDistanceStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        (*i)->distList.clear();
        delete *i;
    }
    plist->clear();
}

// void clear_PushNotifyUserStruct_list(std::list<PushNotifyUserStruct*> * plist)
// {
//     for (std::list<PushNotifyUserStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
//     {
//         delete *i;
//     }
//     plist->clear();
// }

void clear_NodeMoveOffsetInfoStruct_list(std::list<NodeMoveOffsetInfoStruct*> * plist)
{
    for (std::list<NodeMoveOffsetInfoStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_UserResidentStruct_list(std::list<UserResidentStruct*> * plist)
{
    for (std::list<UserResidentStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_UserEmployeeStruct_list(std::list<UserEmployeeStruct*> * plist)
{
    for (std::list<UserEmployeeStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_FunctionSendTimeStruct_list(std::list<FunctionSendTimeStruct*> * plist)
{
    for (std::list<FunctionSendTimeStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_ProjectConfig_list(std::list<ProjectConfig*> * plist)
{
    for (std::list<ProjectConfig*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_AreaInOutEventStruct_list(std::list<AreaInOutEventStruct*> * plist)
{
    for (std::list<AreaInOutEventStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_node_AreaInOutEventStruct_list(std::list<node_AreaInOutEventStruct*> * plist)
{
    for (std::list<node_AreaInOutEventStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        clear_AreaInOutEventStruct_list((*i)->area_inout_events);
        delete (*i)->area_inout_events;
        delete (*i);
    }
    plist->clear();
}

void clear_Pair_AreaInOutEventStruct_list(std::list<Pair_AreaInOutEventStruct*> * plist)
{
    for (std::list<Pair_AreaInOutEventStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        clear_Pair_AreaInOutEventStruct((*i));
        delete *i;
    }
    plist->clear();
}

void clear_Pair_AreaInOutEventStruct(Pair_AreaInOutEventStruct * event)
{
    if ( event->event1 != NULL )
        delete event->event1;
    if ( event->event2 != NULL )
        delete event->event2;
}

void clear_OperationModeActivityStruct_list(std::list<OperationModeActivityStruct*> * plist)
{
    for (std::list<OperationModeActivityStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        (*i)->nodeidList.clear();
        delete *i;
    }
    plist->clear();
}

void clear_PushNotifySettingStruct_list(std::list<PushNotifySettingStruct*> * plist)
{
    for (std::list<PushNotifySettingStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        if ((*i)->functionList != NULL)
        {
            (*i)->functionList->clear();
            delete (*i)->functionList;
        }
        delete *i;
    }
    plist->clear();
}

void clear_TriggerPushMessageStruct_list(std::list<TriggerPushMessageStruct*> * plist)
{
    for (std::list<TriggerPushMessageStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_MapLayerStruct_list(std::list<MapLayerStruct*> * plist)
{
    for (std::list<MapLayerStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_AoANodeStruct_list(std::list<AoANodeStruct*> * plist)
{
    for (std::list<AoANodeStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_PointStruct_list(std::list<PointStruct*> * plist)
{
    for (std::list<PointStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_BodyInfoStruct_list(std::list<BodyInfoStruct*> * plist)
{
    for (std::list<BodyInfoStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_DeviceActivityAliveCountStruct_list(std::list<DeviceActivityAliveCountStruct*> * plist)
{
    for (std::list<DeviceActivityAliveCountStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_AreaRollCallRecordStruct_list(std::list<AreaRollCallRecordStruct*> * plist)
{
    for (std::list<AreaRollCallRecordStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_OperationModeRecordStruct_list(std::list<OperationModeRecordStruct*> * plist)
{
    for (std::list<OperationModeRecordStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_TagMoveOffsetRecordStruct_list(std::list<TagMoveOffsetRecordStruct*> * plist)
{
    for (std::list<TagMoveOffsetRecordStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_DeviceActivityRecordStruct_list(std::list<DeviceActivityRecordStruct*> * plist)
{
    for (std::list<DeviceActivityRecordStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_PDFTableDataStruct(PDFTableDataStruct * pdfData)
{
    for (std::list<PDFTableHeaderDataStruct *>::iterator header = pdfData->headerlist.begin(); header != pdfData->headerlist.end(); header++)
    {
        delete (*header);
    }
    pdfData->headerlist.clear();

    for (std::list<PDFTableRowDataStruct *>::iterator row = pdfData->rowdatalist.begin(); row != pdfData->rowdatalist.end(); row++)
    {
        for (std::list<PDFTableColDataStruct *>::iterator col = (*row)->coldatalist.begin(); col != (*row)->coldatalist.end(); col++)
        {
            delete (*col);
        }
        (*row)->coldatalist.clear();
        delete (*row);
    }
    pdfData->rowdatalist.clear();
}

void clear_CourseDayTrackRecordStruct_list(std::list<CourseDayTrackRecordStruct*> * plist)
{
    for (std::list<CourseDayTrackRecordStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_PDFTableDataStruct_list(std::list<PDFTableDataStruct*> * plist)
{
    for (std::list<PDFTableDataStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_SurveyStruct_list(std::list<SurveyStruct*> * plist)
{
    for (std::list<SurveyStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_SurveyItemStruct_list(std::list<SurveyItemStruct*> * plist)
{
    for (std::list<SurveyItemStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_PushNotifyMessageRecordStruct_list(std::list<PushNotifyMessageRecordStruct*> * plist)
{
    for (std::list<PushNotifyMessageRecordStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_SurveyEventRecordStruct_list(std::list<SurveyEventRecordStruct*> * plist)
{
    for (std::list<SurveyEventRecordStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete *i;
    }
    plist->clear();
}

void clear_RFIDCardStruct_list(std::list<RFIDCardStruct*> * plist)
{
    for (std::list<RFIDCardStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    	delete *i;
    plist->clear();
}

void clear_RFIDDeviceStruct_list(std::list<RFIDDeviceStruct*> * plist)
{
    for (std::list<RFIDDeviceStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    	delete *i;
    plist->clear();
}

void clear_NFCCardStruct_list(std::list<NFCCardStruct*> * plist)
{
    for (std::list<NFCCardStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    	delete *i;
    plist->clear();
}

void clear_NFCDeviceStruct_list(std::list<NFCDeviceStruct*> * plist)
{
    for (std::list<NFCDeviceStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    	delete *i;
    plist->clear();
}

void clear_EDGEDeviceStruct_list(std::list<EDGEDeviceStruct*> * plist)
{
    for (std::list<EDGEDeviceStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    	delete *i;
    plist->clear();
}

void clear_CARDRollCallRecordStruct_list(std::list<CARDRollCallRecordStruct*> * plist)
{
    for (std::list<CARDRollCallRecordStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    	delete *i;
    plist->clear();
}

void clear_USERRollCallRecordStruct_list(std::list<USERRollCallRecordStruct*> * plist)
{
    for (std::list<USERRollCallRecordStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        (*i)->recordlist.clear();
        delete (*i);
    }
}

void clear_CARDScanRecordStruct_list(std::list<CARDScanRecordStruct*> * plist)
{
    for (std::list<CARDScanRecordStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        // (*i)->scanRecords.clear();
        delete (*i);
    }
    plist->clear();
}

void clear_TargetTopicStruct_list(std::list<TargetTopicStruct*> * plist)
{
    for (std::list<TargetTopicStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    	delete *i;
    plist->clear();
}

void clear_LocatorRSSIInfoStruct_list(std::list<LocatorRSSIInfoStruct*> * plist)
{
    for (std::list<LocatorRSSIInfoStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        clear_GatewayRSSIInfoStruct_list((*i)->gatewayRecords);
        delete (*i)->gatewayRecords;
        delete (*i);
    }
    plist->clear();
}

void clear_GatewayRSSIInfoStruct_list(std::list<GatewayRSSIInfoStruct*> * plist)
{
    for (std::list<GatewayRSSIInfoStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        clear_RSSIInfoStruct_list((*i)->rssiRecords);
        delete (*i)->rssiRecords;
        delete (*i);
    }
    plist->clear();
}

void clear_RSSIInfoStruct_list(std::list<RSSIInfoStruct*> * plist)
{
    for (std::list<RSSIInfoStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete (*i);
    }
    plist->clear();
}

void clear_GAESurveyResultData_list(std::list<GAESurveyResultData*> * plist)
{
    for (std::list<GAESurveyResultData*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        (*i)->results.clear();
        delete (*i);
    }
    plist->clear();
}

void clear_CARDRollCallTriggerItemStruct_list(std::list<CARDRollCallTriggerItemStruct*> * plist)
{
    for (std::list<CARDRollCallTriggerItemStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        delete (*i);
    }
    plist->clear();
}

void clear_CARDRollCallTriggerStruct_list(std::list<CARDRollCallTriggerStruct*> * plist)
{
    for (std::list<CARDRollCallTriggerStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        if ( (*i)->triggeritems != NULL )
        {
            clear_CARDRollCallTriggerItemStruct_list((*i)->triggeritems);
            delete (*i)->triggeritems;
        }
        delete (*i);
    }
    plist->clear();
}

void clear_CARDRollCallSubGroupEventStruct_list(std::list<CARDRollCallSubGroupEventStruct*> * plist)
{
    for (std::list<CARDRollCallSubGroupEventStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        (*i)->userids.clear();
        (*i)->presentuserids.clear();
        delete (*i);
    }
    plist->clear();
}

void clear_FireFightingUserAirInfoStruct_list(std::list<FireFightingUserAirInfoStruct*> * plist)
{
    for (std::list<FireFightingUserAirInfoStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        (*i)->AirInfoList->clear();
        delete (*i)->AirInfoList;
        delete (*i);
    }
    plist->clear();
}
// void clear_CourseAnchorRanging_vector(std::vector<CourseAnchorRangingDataClass*> * plist)
// {
//     for (std::vector<CourseAnchorRangingDataClass*>::iterator i = plist->begin(); i != plist->end(); i++)
//     	delete *i;
//     plist->clear();
// }

// void clear_Resource_list(std::list<ResourceStruct*> * plist)
// {
//     for (std::list<ResourceStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
//         delete *i;
//     plist->clear();
// }

// convert from AA-BB-CC-44-55-66-77-88 to AABBCC4455667788
char * convertToSimpleAddress(char * inString)
{
    char temp[128 + 1];
    int newLen = 0;
    int len = (int)strlen(inString);
    if (len > 128)
        len = 128;
    for(int i=0;i<len;i++)
    {
        if ( inString[i] == '-' )
            continue;
        temp[newLen++] = inString[i];
    }
    temp[newLen] = '\0';
    strcpy(inString, temp);
    return inString;
}

long long getSystemTime()
{
    struct timeb t;
    ftime(&t);
    return 1000 * t.time + t.millitm;
}

// bool read_sys_config(VILSSystemConfig * config)
// {
//     MySQLDBAccess * mysqldb = new MySQLDBAccess();
//     mysqldb->read_sys_config(config);
//     delete mysqldb;
//
//     return true;
// }
//
// bool write_sys_config(VILSSystemConfig * config)
// {
//     MySQLDBAccess * mysqldb = new MySQLDBAccess();
//     mysqldb->write_sys_config(1, config);
//     delete mysqldb;
//
//     return true;
// }
//
// bool read_vils_config(int project_id, VILSConfig * config)
// {
//     MySQLDBAccess * mysqldb = new MySQLDBAccess();
//     mysqldb->read_config(project_id, config);
//     delete mysqldb;
//
//     return true;
// }

int copyFile(const char * source_file, const char * target_file)
{
    FILE *source, *target;

    source = fopen(source_file, "r");

    if( source == NULL )
    {
        return -1;
    }

    target = fopen(target_file, "w");

    if( target == NULL )
    {
        fclose(source);
        return -2;
    }

    //while( ( ch = fgetc(source) ) != EOF )
    //    fputc(ch, target);
    char buffer[256];
    size_t n;

    while ((n = fread(buffer, sizeof(char), sizeof(buffer), source)) > 0)
    {
        if (fwrite(buffer, sizeof(char), n, target) != n)
        {
            break;
        }
    }

    fclose(source);
    fclose(target);

    return 0;
}

double calcVelocity(int * oldIMU, int * newIMU)
{
    double ret = 0;
	float Velocity = 0.0f;
	double IMUdiff[6];
	for(int j=0;j<6;j++)
		IMUdiff[j] = (double)(newIMU[j] - oldIMU[j]) / 10.0f; // Tag send IMU data x 10.0f

	// double caaX = IMUdiff[3] * IMUdiff[3] * (1.0f) ; // Centripetal Acceleration = Angular Acceleration * radius(1.0f)
	// double caaY = IMUdiff[4] * IMUdiff[4] * (1.0f) ; // Centripetal Acceleration = Angular Acceleration * radius(1.0f)
	// double caaZ = IMUdiff[5] * IMUdiff[5] * (1.0f) ; // Centripetal Acceleration = Angular Acceleration * radius(1.0f)

    double caaX = IMUdiff[3] * IMUdiff[3] * (1.0f) * 0.1f ; // Centripetal Acceleration = Angular Acceleration * radius(1.0f)
	double caaY = IMUdiff[4] * IMUdiff[4] * (1.0f) * 0.1f ; // Centripetal Acceleration = Angular Acceleration * radius(1.0f)
	double caaZ = IMUdiff[5] * IMUdiff[5] * (1.0f) * 0.1f; // Centripetal Acceleration = Angular Acceleration * radius(1.0f)

	double vxyz = IMUdiff[0] * IMUdiff[0] + IMUdiff[1] * IMUdiff[1] + IMUdiff[2] * IMUdiff[2];
	double ca = (caaY + caaZ) * (caaY + caaZ) + (caaX + caaZ) * (caaX + caaZ) + (caaX + caaY) * (caaX + caaY);

	//printf ("AnchorTagCoupleDataClass::putRanging() vxyz = %f, ca = %f\n", vxyz, ca);

	if ( vxyz - ca  > 0.0f)
	{
		Velocity = sqrt(vxyz - ca);
	}
	else
	{
		Velocity = 0.0f; // rotating
	}
    ret = (double)Velocity;
	return ret;
}

// void mac_eth0(char * MAC_str)
// {
//     #define HWADDR_len 6
//     int s,i;
//     struct ifreq ifr;
//     s = socket(AF_INET, SOCK_DGRAM, 0);
//     strcpy(ifr.ifr_name, "eth0");
//     ioctl(s, SIOCGIFHWADDR, &ifr);
//     char tmp[5];
//     for (i=0; i<HWADDR_len; i++)
//     {
//         sprintf(tmp,"%02X", ((unsigned char*)ifr.ifr_hwaddr.sa_data)[i]);
//         MAC_str[i*2 + 0] = tmp[0];
//         MAC_str[i*2 + 1] = tmp[1];
//         //sprintf(&MAC_str[i*2],"%02X",((unsigned char*)ifr.ifr_hwaddr.sa_data)[i]);
//     }
//     MAC_str[12]='\0';
// }

int getMyCenterIP(char * ip)
{
    // query all interface
    struct ifaddrs * ifap;
    if (getifaddrs(&ifap) == 0)
    {
       struct ifaddrs * p = ifap;
       while(p)
       {
          uint32 ifaAddr  = SockAddrToUint32(p->ifa_addr);
          uint32 maskAddr = SockAddrToUint32(p->ifa_netmask);
          uint32 dstAddr  = SockAddrToUint32(p->ifa_dstaddr);
          if (ifaAddr > 0)
          {
             char ifaAddrStr[32];
             Inet_NtoA(ifaAddr,  ifaAddrStr);
             char maskAddrStr[32];
             Inet_NtoA(maskAddr, maskAddrStr);
             char dstAddrStr[32];
             Inet_NtoA(dstAddr,  dstAddrStr);

             // printf("  Found interface:  name=[%s] desc=[%s] address=[%s] netmask=[%s] broadcastAddr=[%s]\n",
             //            p->ifa_name, "unavailable", ifaAddrStr, maskAddrStr, dstAddrStr);

             // skip local loop ip
             //if ( strcmp(dstAddrStr, "127.0.0.1") != 0)
             //strncmp(dstAddrStr, "192.168.88", 10) != 0)
             // if ( strcmp(maskAddrStr, "255.255.255.0") == 0 &&
             //      strcmp(dstAddrStr, "127.0.0.1") != 0)
             if ( strncmp(maskAddrStr, "255.255.255", 8) == 0 &&
                  strcmp(dstAddrStr, "127.0.0.1") != 0)
             {
                 printf("common() Found My Center IP[%s]\n", ifaAddrStr);
                 strcpy(ip, ifaAddrStr);
                 break;
             }
          }
          p = p->ifa_next;
       }
       freeifaddrs(ifap);
    }

    return 0;
}

int getMyCenterNetworkInfo(std::list<NetworkInfoStruct * > & CenterNetworkInfoList)
{
    int count = 0;
    // query all interface
    struct ifaddrs * ifap;
    if (getifaddrs(&ifap) == 0)
    {
       struct ifaddrs * p = ifap;
       while(p)
       {
          uint32 ifaAddr  = SockAddrToUint32(p->ifa_addr);
          uint32 maskAddr = SockAddrToUint32(p->ifa_netmask);
          uint32 dstAddr  = SockAddrToUint32(p->ifa_dstaddr);
          if (ifaAddr > 0)
          {
             char ifaAddrStr[32];
             Inet_NtoA(ifaAddr,  ifaAddrStr);
             char maskAddrStr[32];
             Inet_NtoA(maskAddr, maskAddrStr);
             char dstAddrStr[32];
             Inet_NtoA(dstAddr,  dstAddrStr);

             //printf("  Found interface:  name=[%s] desc=[%s] address=[%s] netmask=[%s] broadcastAddr=[%s]\n",
             //           p->ifa_name, "unavailable", ifaAddrStr, maskAddrStr, dstAddrStr);

             // skip local loop ip
             //if ( strcmp(dstAddrStr, "127.0.0.1") != 0)
             //strncmp(dstAddrStr, "192.168.88", 10) != 0)
             // if ( strcmp(maskAddrStr, "255.255.255.0") == 0 &&
             //      strcmp(dstAddrStr, "127.0.0.1") != 0)
             if ( strncmp(maskAddrStr, "255.255.255", 8) == 0 &&
                  strcmp(dstAddrStr, "127.0.0.1") != 0)
             {
                 // printf("common::getMyCenterNetworkInfo() Found My IP[%s] interface[%s] broadcastAddr[%s]\n",
                 //    ifaAddrStr, p->ifa_name, dstAddrStr);

                 NetworkInfoStruct * netif = new NetworkInfoStruct();
                 netif->interface = p->ifa_name;
                 netif->ip = ifaAddrStr;
                 netif->broadcastAddr = dstAddrStr;
                 CenterNetworkInfoList.push_back(netif);

                 //strcpy(ip, ifaAddrStr);
                 //CenterIPList.push_back(ifaAddrStr);
                 count++;
             }
          }
          p = p->ifa_next;
       }
       freeifaddrs(ifap);
    }
    return count;
}

int getMyIP(char * ip)
{
    int fd;
    struct ifreq ifr;

    fd = socket(AF_INET, SOCK_DGRAM, 0);

    if ( fd == -1 )
    {
        return 0;
    }

    /* I want to get an IPv4 IP address */
    ifr.ifr_addr.sa_family = AF_INET;

    /* I want IP address attached to "eth0" */
    //strncpy(ifr.ifr_name, "wlan0", IFNAMSIZ-1);
    strncpy(ifr.ifr_name, "en1", IFNAMSIZ-1);

    ioctl(fd, SIOCGIFADDR, &ifr);

    close(fd);

    /* display result */
    strcpy(ip , inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));

    return 0;
}

int getIPfromName(const char * hostname, char * ip)
{
    //int sockfd;
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_in *h;
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_UNSPEC; // use AF_INET6 to force IPv6
    hints.ai_socktype = SOCK_STREAM;

    if ( (rv = getaddrinfo( hostname , "http" , &hints , &servinfo)) != 0)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and connect to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next)
    {
        h = (struct sockaddr_in *) p->ai_addr;
        strcpy(ip , inet_ntoa( h->sin_addr ) );
    }

    freeaddrinfo(servinfo); // all done with this structure
    return 0;
}

void clear_area_list(std::list<AreaStruct*> * areaList)
{
    //printf("common::clear_area_list() start\n");
    if ( areaList == NULL )
        return;
    for (std::list <AreaStruct *>::iterator i = areaList->begin (); i != areaList->end (); i++)
    {
        clear_PolygonStruct_list((*i)->Polygons);
        delete (*i)->Polygons;
        delete *i;
    }
    areaList->clear();
    //printf("common::clear_area_list() end\n");
}

void clear_maplayerarea_list(std::list<MapLayerAreaStruct*> * areaList)
{
    //printf("common::clear_area_list() start\n");
    if ( areaList == NULL )
        return;
    for (std::list <MapLayerAreaStruct *>::iterator i = areaList->begin (); i != areaList->end (); i++)
    {
        clear_PolygonStruct_list((*i)->Polygons);
        delete (*i)->Polygons;
        delete *i;
    }
    areaList->clear();
    //printf("common::clear_area_list() end\n");
}

void clear_areacenter_list(std::list<AreaCenterStruct*> * areacenterList)
{
    //printf("common::clear_areacenter_list() start\n");
    if ( areacenterList == NULL )
        return;
    for (std::list <AreaCenterStruct *>::iterator i = areacenterList->begin (); i != areacenterList->end (); i++)
    {
        delete *i;
    }
    areacenterList->clear();
    //printf("common::clear_areacenter_list() end\n");
}

void clear_PolygonStruct_list(std::list<PolygonStruct*> * polyList)
{
    //printf("common::clear_PolygonStruct_list() start\n");
    if ( polyList == NULL )
        return;
    for (std::list <PolygonStruct *>::iterator i = polyList->begin (); i != polyList->end (); i++)
    {
        if ((*i)->coordinates != NULL)
        {
            (*i)->coordinates->clear();
            delete (*i)->coordinates;
        }
        delete *i;
    }
    polyList->clear();
    //printf("common::clear_PolygonStruct_list() end\n");
}

void clear_event_list(std::list<EventStruct*> * eventList)
{
    //printf("common::clear_event_list() start\n");
    if ( eventList == NULL )
        return;
    for (std::list <EventStruct *>::iterator i = eventList->begin (); i != eventList->end (); i++)
	   delete *i;
    eventList->clear();
    //printf("common::clear_event_list() end\n");
}

void clear_Footprint_list(std::list<FootprintStruct*> * footprintList)
{
    //printf("common::clear_Footprint_list() start\n");
    if ( footprintList == NULL )
        return;
    for (std::list <FootprintStruct *>::iterator i = footprintList->begin (); i != footprintList->end (); i++)
	   delete *i;
    footprintList->clear();
    //printf("common::clear_Footprint_list() end\n");
}

void clear_NetworkInfo_list(std::list<NetworkInfoStruct*> * NetworkInfoList)
{
    if ( NetworkInfoList == NULL )
        return;
    for (std::list <NetworkInfoStruct *>::iterator i = NetworkInfoList->begin (); i != NetworkInfoList->end (); i++)
	   delete *i;
    NetworkInfoList->clear();
}

void clear_MulticastClient_list(std::list<MulticastClient*> * MulticastClientList)
{
    if ( MulticastClientList == NULL )
        return;
    for (std::list <MulticastClient *>::iterator i = MulticastClientList->begin (); i != MulticastClientList->end (); i++)
	   delete *i;
    MulticastClientList->clear();
}

//
//    Latitude: 1 deg = 110.574 km
//    Longitude: 1 deg = 111.320*cos(latitude) km
//
double centimeterToLatitude(int distenceincm)
{
    //利用距離的比例來算出緯度上的比例
    double lat_diff = (double)distenceincm / (double)11057400.0;
	return lat_diff;
}

double centimeterToLongitude(double latitude, int distenceincm)
{
    // M_PI/180.0 = 0.017453
    double lon_distance = 11132000.0 * cos(latitude * 0.017453); //算出該緯度上的經度長度
    double lon_diff = (double)distenceincm / lon_distance; //利用距離的比例來算出經度上的比例
    return lon_diff;
}

double LatitudeToCentimeter(double latitude)
{
     return (latitude * 11057400.0);
     // return (latitude * 11112716.2812);
}

double LongitudeToCentimeter(double latitude, double longitude)
{
    // M_PI/180.0 = 0.017453
    double lon_distance = 11132000.0 * cos(latitude * 0.017453); //算出該緯度上的經度長度

    return (longitude * lon_distance);
}

double centimeterTolatLng(int distenceincm)
{
    return ((double)distenceincm * 0.000000089987);
    // return ((double)distenceincm / 100.0) * 0.0000089987;
}

double latLngTocentimeter(double distenceinlatLng)
{
    //double value = distenceinlatLng / 0.0000089987;
    // vilslog::printf("latLngTocentimeter() distenceinlatLng[%f] value[%f] [%f]\n", distenceinlatLng, value, value * 100.0);
    // 0.000097 / 0.0000089987 = 10.7793347928
	//return (value * 100.0);
    return (distenceinlatLng * 11112716.2812);
}

double calculateDistance(double lat1, double long1, double lat2, double long2)
{
    double dist;
    dist = sin(toRad(lat1)) * sin(toRad(lat2)) + cos(toRad(lat1)) * cos(toRad(lat2)) * cos(toRad(long1 - long2));
    dist = acos(dist);
//        dist = (6371 * pi * dist) / 180;
    //got dist in radian, no need to change back to degree and convert to rad again.
    dist = 6371 * dist;
    return dist;
}

double toRad(double degree)
{
    //#define PI 3.14159265358979323846
    return (degree/180) * (3.14159265358979323846);
}

std::string convertToStringlist(int anchor_count, int * anchors)
{
    std::string validAnchors = "";
    char tmp[32];
    int save = 1;
    int isFirst = 1;
    int startAnchorIndex = -1;
    int preAnchorIndex = -1;
    std::ostringstream sstream;
    for(int i=0;i<anchor_count;i++)
    {
        save = 0;
        if ( preAnchorIndex >= 0 )
        {
            if ( preAnchorIndex != anchors[i] - 1 )
            {
                // ok to save
                save = 1;
                if (startAnchorIndex == preAnchorIndex)
                    sprintf(tmp,"%02d", startAnchorIndex);
                else
                    sprintf(tmp,"%02d-%02d", startAnchorIndex, preAnchorIndex);
                startAnchorIndex = anchors[i];
                //preAnchorIndex = anchors[i];
            }
        }
        else
        {
            startAnchorIndex = anchors[i];
            //sprintf(tmp,"%d", anchors[i]);
        }
        //preAnchorIndex = anchors[i];

        if ( (i >= anchor_count - 1) && save == 0)
        {
            // ok to save
            save = 1;
            if (startAnchorIndex == anchors[i])
                sprintf(tmp,"%02d", anchors[i]);
            else
                sprintf(tmp,"%02d-%02d", startAnchorIndex, anchors[i]);
            preAnchorIndex = anchors[i];
        }

        if ( save == 1 )
        {
            if (isFirst != 1)
                sstream << ":";
            sstream << tmp;
            isFirst = 0;

            if (i >= anchor_count - 1)
            {
                if ( preAnchorIndex != anchors[i] )
                {
                    sprintf(tmp,":%02d", anchors[i]);
                    sstream << tmp;
                }
            }
        }
        preAnchorIndex = anchors[i];
    }

    validAnchors = sstream.str();

    return validAnchors;
}

std::string convertListToString(std::list<std::string> * stringlist)
{
    return convertListToString(stringlist, ",");
}

std::string convertListToString(std::list<std::string> * stringlist, std::string sp)
{
    std::string listStr = "";
    std::ostringstream outstream;
    for (std::list<std::string>::iterator str = stringlist->begin(); str != stringlist->end(); str++)
    {
        if (str != stringlist->begin())
            outstream<<sp;
        outstream<<(*str);
    }
    listStr = outstream.str();
    return listStr;
}

std::string convertVectorToString(std::vector<std::string> * stringlist)
{
    std::string listStr = "";
    std::ostringstream outstream;
    for (unsigned int i=0; i< stringlist->size(); i++)
    {
        if (i != 0 )
            outstream<<",";
        outstream<<(*stringlist)[i];
    }
    listStr = outstream.str();
    return listStr;
}

std::string convertIntListToString(std::list<int> * intlist)
{
    std::string listStr = "";
    for (std::list<int>::iterator weekday = intlist->begin(); weekday != intlist->end(); weekday++)
    {
        if (weekday != intlist->begin() )
            listStr.append(",");
        listStr.append(std::to_string((*weekday)));
    }
    return listStr;
}

void printout(std::list<SectionGroup *> * sectiongrouplist)
{
    for (std::list<SectionGroup *>::iterator sectiongroup = sectiongrouplist->begin(); sectiongroup != sectiongrouplist->end(); sectiongroup++)
    {
        std::string nodeIDListStr = convertListToString((*sectiongroup)->nodeIDList);
        printf("%d:%d:%d:%s:%s:%d:%d:%d:%d:%d:%d\n",
            (*sectiongroup)->GroupID, (*sectiongroup)->projectid, (*sectiongroup)->SectionType, nodeIDListStr.c_str(),
            (*sectiongroup)->AreaID.c_str(), (*sectiongroup)->Param1, (*sectiongroup)->Param2, (*sectiongroup)->Param3,
            (*sectiongroup)->Param4, (*sectiongroup)->Param5, (*sectiongroup)->Param6);
    }
}

uint32 SockAddrToUint32(struct sockaddr * a)
{
   return ((a)&&(a->sa_family == AF_INET)) ? ntohl(((struct sockaddr_in *)a)->sin_addr.s_addr) : 0;
}

// convert a numeric IP address into its string representation
void Inet_NtoA(uint32 addr, char * ipbuf)
{
   sprintf(ipbuf, "%li.%li.%li.%li", (addr>>24)&0xFF, (addr>>16)&0xFF, (addr>>8)&0xFF, (addr>>0)&0xFF);
}

// convert a string represenation of an IP address into its numeric equivalent
uint32 Inet_AtoN(const char * buf)
{
   // net_server inexplicably doesn't have this function; so I'll just fake it
   uint32 ret = 0;
   int shift = 24;  // fill out the MSB first
   bool startQuad = true;
   while((shift >= 0)&&(*buf))
   {
      if (startQuad)
      {
         unsigned char quad = (unsigned char) atoi(buf);
         ret |= (((uint32)quad) << shift);
         shift -= 8;
      }
      startQuad = (*buf == '.');
      buf++;
   }
   return ret;
}

void PrintNetworkInterfaceInfos()
{
    // BSD-style implementation
   struct ifaddrs * ifap;
   if (getifaddrs(&ifap) == 0)
   {
      struct ifaddrs * p = ifap;
      while(p)
      {
         uint32 ifaAddr  = SockAddrToUint32(p->ifa_addr);
         uint32 maskAddr = SockAddrToUint32(p->ifa_netmask);
         uint32 dstAddr  = SockAddrToUint32(p->ifa_dstaddr);
         if (ifaAddr > 0)
         {
            char ifaAddrStr[32];  Inet_NtoA(ifaAddr,  ifaAddrStr);
            char maskAddrStr[32]; Inet_NtoA(maskAddr, maskAddrStr);
            char dstAddrStr[32];  Inet_NtoA(dstAddr,  dstAddrStr);
            printf("  Found interface:  name=[%s] desc=[%s] address=[%s] netmask=[%s] broadcastAddr=[%s]\n", p->ifa_name, "unavailable", ifaAddrStr, maskAddrStr, dstAddrStr);
         }
         p = p->ifa_next;
      }
      freeifaddrs(ifap);
   }
}

// std::string convert_to_json_str(int projectid, int eventid, std::string nodeid, std::string action, std::string areaid, std::string issuetime)
// {
//     std::string json_str = "{";
//
//     char tmp[64];
//     snprintf(tmp, 64, "%d", projectid);
//     json_str += "\"projectid\":\"" + std::string(tmp) + "\"";
//     json_str += ",";
//
//     snprintf(tmp, 64, "%d", eventid);
//     json_str += "\"eventid\":\"" + std::string(tmp) + "\"";
//     json_str += ",";
//     json_str += "\"nodeid\":\"" + nodeid + "\"";
//     json_str += ",";
//     json_str += "\"action\":\"" + action + "\"";
//     json_str += ",";
//     json_str += "\"areaid\":\"" + areaid + "\"";
//     json_str += ",";
//     json_str += "\"issuetime\":\"" + issuetime + "\"";
//     json_str += "}";
//
//     //printf("EventsHandler::convert_to_json_str json_str[%s]\n", json_str.c_str());
//
//     return json_str;
// }

std::string convert_to_json_str(int projectid, int eventid, std::string nodeid, std::string action, std::string areaid, std::string issuetime, std::string locatorid)
{
    std::string json_str = "{";

    char tmp[64];
    snprintf(tmp, 64, "%d", projectid);
    json_str += "\"projectid\":\"" + std::string(tmp) + "\"";
    json_str += ",";

    snprintf(tmp, 64, "%d", eventid);
    json_str += "\"eventid\":\"" + std::string(tmp) + "\"";
    json_str += ",";
    json_str += "\"nodeid\":\"" + nodeid + "\"";
    json_str += ",";
    json_str += "\"action\":\"" + action + "\"";
    json_str += ",";
    json_str += "\"areaid\":\"" + areaid + "\"";
    json_str += ",";
    json_str += "\"issuetime\":\"" + issuetime + "\"";
    json_str += ",";
    json_str += "\"locatorid\":\"" + locatorid + "\"";
    json_str += "}";

    //printf("EventsHandler::convert_to_json_str json_str[%s]\n", json_str.c_str());

    return json_str;
}

std::string convert_to_json_str(int projectid, int eventid, std::string nodeid, std::string action, std::string areaid, std::string issuetime, std::string locatorid, std::string lat, std::string lng)
{
    std::string json_str = "{";

    char tmp[64];
    snprintf(tmp, 64, "%d", projectid);
    json_str += "\"projectid\":\"" + std::string(tmp) + "\"";
    json_str += ",";

    snprintf(tmp, 64, "%d", eventid);
    json_str += "\"eventid\":\"" + std::string(tmp) + "\"";
    json_str += ",";
    json_str += "\"nodeid\":\"" + nodeid + "\"";
    json_str += ",";
    json_str += "\"action\":\"" + action + "\"";
    json_str += ",";
    json_str += "\"areaid\":\"" + areaid + "\"";
    json_str += ",";
    json_str += "\"issuetime\":\"" + issuetime + "\"";
    json_str += ",";
    json_str += "\"locatorid\":\"" + locatorid + "\"";
    json_str += ",";
    json_str += "\"lat\":\"" + lat + "\"";
    json_str += ",";
    json_str += "\"lng\":\"" + lng + "\"";
    json_str += "}";

    //printf("EventsHandler::convert_to_json_str json_str[%s]\n", json_str.c_str());

    return json_str;
}

std::string convert_FireFightingStruct_to_json_str(FireFightingStruct * fireFighting)
{
    std::ostringstream sendstream;

    std::string StandbyDeviceIds = convert_list_to_string(fireFighting->StandbyDeviceIds);
    std::string NFCDeviceIds = convert_listint_to_string(fireFighting->NFCDeviceIdList);

    sendstream << "{\"id\":"<< fireFighting->id;
    // sendstream << ",\"maingroupid\":"<< fireFighting->maingroupid;
    sendstream << ",\"caseclosed\":"<< fireFighting->caseclosed;
    // sendstream << ",\"casenumber\":\""<< fireFighting->casenumber<<"\"";
    // sendstream << ",\"address\":\""<< fireFighting->address<<"\"";
    // sendstream << ",\"lat\":\""<< fireFighting->lat<<"\"";
    // sendstream << ",\"lng\":\""<< fireFighting->lng<<"\"";
    sendstream << ",\"starttime\":\""<< fireFighting->starttime<<"\"";
    sendstream << ",\"closetime\":\""<< fireFighting->closetime<<"\"";
    sendstream << ",\"StandbyDeviceIds\":\""<< StandbyDeviceIds<<"\"";
    sendstream << ",\"NFCDeviceIds\":\""<< NFCDeviceIds<<"\"";
    sendstream << "}" << endl;

    std::string json = sendstream.str();

    return json;
}

std::string convert_FireFightingUserStruct_to_json_str(FireFightingUserStruct * fireFightingUser)
{
    std::ostringstream sendstream;

    std::string enterTime = time_tToString(fireFightingUser->enterTime);

    sendstream << "{\"userid\":"<< fireFightingUser->userid;
    // sendstream << ",\"maingroupid\":"<< fireFightingUser->maingroupid;
    sendstream << ",\"areaType\":"<< fireFightingUser->areaType;
    sendstream << ",\"airremain\":\""<< fireFightingUser->airremain<<"\"";
    sendstream << ",\"enterTime\":\""<< enterTime<<"\"";
    // sendstream << ",\"casenumber\":\""<< fireFightingUser->casenumber<<"\"";
    sendstream << ",\"activity\":\""<< fireFightingUser->activity<<"\"";
    sendstream << ",\"HR\":\""<< fireFightingUser->HR<<"\"";
    sendstream << ",\"SPO2\":\""<< fireFightingUser->SPO2<<"\"";
    sendstream << ",\"battery\":\""<< fireFightingUser->battery<<"\"";
    sendstream << "}" << endl;

    std::string json = sendstream.str();
    return json;
}

std::list<AnchorRangingCountStruct *> * parse_anchor_ranging_table(const char * AnchorRangingTable)
{
    std::list<AnchorRangingCountStruct *> * AnchorRangingList = new std::list<AnchorRangingCountStruct *>();

    int length = (int)strlen(AnchorRangingTable);
    int dataIdx = 0;
    char SingleAnchorData[32];

    int i = 0;
    while( i < length )
    {
        int index = 0;
        while(AnchorRangingTable[i] != ':' && i < length)
        {
            SingleAnchorData[index++] = AnchorRangingTable[i++];
        }
        SingleAnchorData[index] = '\0';

        // 0000=0  失敗0次
        // 003a=2  失敗2次
        // 0003=2  失敗2次
        int dataLength = (int)strlen(SingleAnchorData);
        char AnIdx[10];
        char FaileCnt[10];
        memset(AnIdx, 0, 10);
        memset(FaileCnt, 0, 10);
        int j=0;
        while( j < dataLength )
        {
            int idx = 0;
            while(SingleAnchorData[j] != '=' && j < dataLength)
            {
                AnIdx[idx++] = SingleAnchorData[j++];
            }
            idx = 0;
            j++;
            while( j < dataLength)
            {
                FaileCnt[idx++] = SingleAnchorData[j++];
            }
        }

        // printf("parse_anchor_ranging_table() SingleAnchorData[%d][%s] -> AnIdx[%s] FaileCnt[%s]\n",
        //             dataIdx, SingleAnchorData, AnIdx, FaileCnt);
        AnchorRangingCountStruct * newStruct = new AnchorRangingCountStruct();
        newStruct->AnchorIndex = (int)strtol(AnIdx, NULL, 16); // hex value
        newStruct->FailCount = atoi(FaileCnt);
        AnchorRangingList->push_back(newStruct);

        // printf("parse_anchor_ranging_table() [%d] AnchorIndex[%d] FailCount[%d]\n",
        //             dataIdx, newStruct->AnchorIndex, newStruct->FailCount);

        dataIdx++;
        i++;
    }

    return AnchorRangingList;
}

void clear_AnchorRangingCount_list(std::list<AnchorRangingCountStruct*> * plist)
{
    //printf("common::clear_AnchorRangingCount_list() start\n");
    if ( plist == NULL )
        return;
    for (std::list<AnchorRangingCountStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
        delete *i;
    plist->clear();
    //printf("common::clear_AnchorRangingCount_list() end\n");
}

void replaceAll(std::string& str, const std::string& from, const std::string& to)
{
    if(from.empty())
        return;
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // In case 'to' contains 'from', like replacing 'x' with 'yx'
    }
}

void printf_VILSSystemConfig(VILSSystemConfig * systemconfig)
{
    printf ("\n\n================================\n");
    printf ("== VILSSystemConfig ===\n");

    printf ("SolveTagPosition_Time[%d]\n",systemconfig->SolveTagPosition_Time);
    printf ("SendTAFDB_Time[%d]\n",systemconfig->SendTAFDB_Time);
    printf ("SendTagAnchorList_Time[%d]\n",systemconfig->SendTagAnchorList_Time);
    printf ("SendDeviceStatus_Time[%d]\n",systemconfig->SendDeviceStatus_Time);
    printf ("UpdateNodeFromSMIMSIOTSERVER_Time[%d]\n",systemconfig->UpdateNodeFromSMIMSIOTSERVER_Time);

    printf ("web_udp_port[%d]\n",systemconfig->web_udp_port);
    printf ("center_udp_port[%d]\n",systemconfig->center_udp_port);
    printf ("coord_udp_port[%d]\n",systemconfig->coord_udp_port);
    printf ("center_broadcast_time[%d]\n",systemconfig->center_broadcast_time);
    printf ("center_broadcast_port[%d]\n",systemconfig->center_broadcast_port);
    printf ("web_socket_port[%d]\n",systemconfig->web_socket_port);
    printf ("UsingProtocol[%d]\n", systemconfig->UsingProtocol);
    printf ("ERPServer[%s]\n", systemconfig->ERPServer.c_str());
    printf ("SSOIP[%s]\n", systemconfig->SSOIP.c_str());

    printf ("TagAlertTimeout[%d]\n",systemconfig->TagAlertTimeout);
    printf ("TagAlertVoltageLow[%s]\n",systemconfig->TagAlertVoltageLow.c_str());

    //
    //VILS Center Algorithm Setting
    //

    //1.Ranging Data Preprocessing
    printf ("ValidDataTime[%d]\n", systemconfig->ValidDataTime);
    printf ("MaxRangindDistance[%d]\n", systemconfig->MaxRangindDistance);
    printf ("EnableNoiseMakup[%d]\n",systemconfig->EnableNoiseMakup);// 0:disable 1:enable
    printf ("NoiseMakupRatio[%f]\n",systemconfig->NoiseMakupRatio); // 2.0 實際測距值與平均測距值比例, 大於此數時計算合理測距值取代目前實際測距值
    printf ("NoiseMakupWeight[%f]\n",systemconfig->NoiseMakupWeight); //0.8 實際測距值併入平均測距值的比重.(0.1~1.0)
    printf ("EnableKalman1D[%d]\n",systemconfig->EnableKalman1D); // 0:disable 1:enable
    printf ("EnableRangingReliability[%d]\n",systemconfig->EnableRangingReliability); // 0:disable 1:enable
    printf ("RangingReliabilityMaxDistance[%d]\n",systemconfig->RangingReliabilityMaxDistance); // 最大可用測距值(cm), 大於此數將捨棄.
    printf ("RangingReliabilityMinCount[%d]\n",systemconfig->RangingReliabilityMinCount); // 特定時間內至少可用測距次數, 少於此數將捨棄.
    printf ("RangingReliabilityAverageDiff[%d]\n",systemconfig->RangingReliabilityAverageDiff); // 最大平均測距差(cm), 大於此數將捨棄.

    //2.Position Calculation
    printf ("EnableAnchor1DMapping[%d]\n",systemconfig->EnableAnchor1DMapping);// 0:disable 1:enable
    printf ("EnableOneAnchorCalculation[%d]\n",systemconfig->EnableOneAnchorCalculation);// 0:disable 1:enable
    printf ("EnableGDOP[%d]\n",systemconfig->EnableGDOP);// 0:disable 1:enable
    printf ("GDOPThreshold[%d]\n",systemconfig->GDOPThreshold); // default:60
    printf ("CalculationMethod[%d]\n",systemconfig->CalculationMethod); // 0: kalman 3D, 1:SMOOTHING_FILTER, 2:Weight Moving Average
    printf ("EnableIMUVelocity[%d]\n",systemconfig->EnableIMUVelocity);// 0:disable 1:enable
    printf ("IMUVelocityOffset[%d]\n",systemconfig->IMUVelocityOffset);// default:5
    printf ("SteadyStateOffset[%d]\n",systemconfig->SteadyStateOffset);// default:5
    printf ("WeightMovingAverageValue1[%f]\n",systemconfig->WeightMovingAverageValue1); //0.01
    printf ("WeightMovingAverageValue2[%f]\n",systemconfig->WeightMovingAverageValue2); //0.02
    printf ("WeightMovingAverageValue3[%f]\n",systemconfig->WeightMovingAverageValue3); //0.97
    printf ("StayInWallTimeout[%d]\n",systemconfig->StayInWallTimeout);// default:5000
    printf ("EnableFixHeight[%d]\n",systemconfig->EnableFixHeight);

    //3.Position Result
    printf ("SmoothingResult[%d]\n",systemconfig->SmoothingResult);
    printf ("SmoothingResultOffset[%d]\n",systemconfig->SmoothingResultOffset);
    printf ("SmoothingResultStep[%d]\n",systemconfig->SmoothingResultStep); // default:6
    printf ("SmoothingResultFirstStateOffsetRatio[%f]\n",systemconfig->SmoothingResultFirstStateOffsetRatio);
    printf ("SmoothingResultFirstStateRunTime[%d]\n",systemconfig->SmoothingResultFirstStateRunTime);
    printf ("SmoothingResultSecondStateOffsetRatio[%f]\n",systemconfig->SmoothingResultSecondStateOffsetRatio);
    printf ("SmoothingResultSecondStateRunTime[%d]\n",systemconfig->SmoothingResultSecondStateRunTime);

    //
    // LoRAWAN
    //
    printf ("LoRAWANTPOIC[%s]\n",systemconfig->LoRAWANTPOIC.c_str());
    printf ("MQTTHostIp[%s]\n",systemconfig->MQTTHostIp.c_str());
    printf ("MQTTHostPort[%d]\n",systemconfig->MQTTHostPort);
    printf ("MQTTHostUsername[%s]\n",systemconfig->MQTTHostUsername.c_str());
    printf ("MQTTHostPassword[%s]\n",systemconfig->MQTTHostPassword.c_str());
    printf ("MQTTClientId[%s]\n",systemconfig->MQTTClientId.c_str());


    printf ("\n================================\n\n");

}

void initialCheckPointGroupDailyRecord(CheckPointGroupDailyRecord & data)
{
    data.DailyRecordID = -1;
    data.projectid = -1;
    data.GroupID = -1;
    // For Runtime Check
    data.Checked = 0;
    data.Finished = 0;
    data.Exceed = 0;
}

void initialCheckPointPathDailyRecord(CheckPointPathDailyRecord & data)
{
    data.DailyRecordID = -1;
    data.projectid = -1;
    data.GroupID = -1;
    data.PathID = -1;

    // For Runtime Check
    data.Checked = 0; // Tag already enter ane leave
    data.Finished = 0;
    data.Exceed = 0;
    data.StartNodeTagData.TagRecordNodeId = "";
    data.StartNodeTagData.TagNodeId = "";
    data.StartNodeTagData.TagStatus = 0;
    data.StartNodeTagData.TagEnterTime = 0;
    data.StartNodeTagData.TagLeaveTime = 0;
    data.EndNodeTagData.TagRecordNodeId = "";
    data.EndNodeTagData.TagNodeId = "";
    data.EndNodeTagData.TagStatus = 0;
    data.EndNodeTagData.TagEnterTime = 0;
    data.EndNodeTagData.TagLeaveTime = 0;

    data.DateTime = "";
}

void initialInspectionGroupDailyRecord(InspectionGroupDailyRecord & data)
{
    data.DailyRecordID = -1;
    data.GroupID = -1;
    // For Runtime Check
    data.Checked = 0;
    data.Finished = 0;
    data.Exceed = 0;
}

void initialInspectionPointDailyRecord(InspectionPointDailyRecord & data)
{
    data.DailyRecordID = -1;
    data.GroupID = -1;
    data.PointID = -1;
    data.Score = 0;

    // For Runtime Check
    data.Checked = 0; // Tag already enter ane leave
    data.Finished = 0;
    data.Exceed = 0;
    data.Inspector = "";
    data.RecordNodeId = "";
    data.Status = 0;
    data.EnterTime = 0;
    data.LeaveTime = 0;

    data.DateTime = "";
    data.SendAlertMessage = 0;
}

//
// Takes the NMEA lat/long format (dddmm.mmmm, [N/S,E/W]) and converts to degrees N,E only
// https://github.com/ckgt/NemaTode/blob/master/src/GPSService.cpp
//
double convertLatLongToDeg(std::string llstr, std::string dir)
{

	double pd = parseDouble(llstr);
	double deg = trunc(pd / 100);				//get ddd from dddmm.mmmm
	double mins = pd - deg * 100;

	deg = deg + mins / 60.0;

	char hdg = 'x';
	if (dir.size() > 0)
    {
		hdg = dir[0];
	}

	//everything should be N/E, so flip S,W
	if (hdg == 'S' || hdg == 'W')
    {
		deg *= -1.0;
	}

	return deg;
}

double parseDouble(std::string s)
{
    char* p;
    double d = ::strtod(s.c_str(), &p);
    if ( *p != 0)
    {
        return 0;
    	// std::stringstream ss;
    	// ss << "NumberConversionError: parseDouble() error in argument \"" << s << "\", '"
    	// 	<< *p << "' is not a number.";
    	// throw NumberConversionError(ss.str());
    }
	return d;
}

float parseFloat(std::string s)
{
    char* p;
    float d = ::strtof(s.c_str(), &p);
    if ( *p != 0)
    {
        return 0;
    }
	return d;
}

int64_t parseInt(std::string s, int radix)
{
	char* p;

	int64_t d = ::strtoll(s.c_str(), &p, radix);

	if (*p != 0)
    {
        return 0;
		// std::stringstream ss;
		// ss << "NumberConversionError: parseInt() error in argument \"" << s << "\", '"
		// 	<< *p << "' is not a number.";
		// throw NumberConversionError(ss.str());
	}
	return d;
}

int convert_string_to_int(std::string inputMsg)
{
    // vilslog::printf("convert_string_to_int() inputMsg[%s]", inputMsg.c_str());
    return parseInt(inputMsg, 10);
}

bool isZero(double value, double threshold)
{
    return value >= -threshold && value <= threshold;
}

bool parse_payload_JSON(const char * buf, Json::Value & result)
{
    bool parsingSuccessful = false;
    std::string bufString = buf;
    std::string::size_type pos = bufString.find('=');
    if (pos != std::string::npos)
    {
        std::string json_string = bufString.substr(pos + 1);
        parsingSuccessful = parse_JSON(json_string, result);
    }
    return parsingSuccessful;
}

bool parse_JSON(std::string json_string, Json::Value & result)
{
    bool parsingSuccessful = false;
    try{
        Json::Reader * reader = new Json::Reader();
        parsingSuccessful = reader->parse( json_string.c_str(), result );
        delete reader;

        if ( !parsingSuccessful )
        {
            printf ("parse_JSON() parsing failed! json_string[%s]\n", json_string.c_str());
        }

    }
    catch(...)
    {
        printf ("parse_JSON() catch exception...\n");
    }
    return parsingSuccessful;
}

std::string get_action_name(std::string action)
{
    std::string action_name = action;

    if (action.compare("enter") == 0)
        action_name = "Enter";
    else
    if (action.compare("leave") == 0)
        action_name = "Leave";
    else
    if (action.compare("stay") == 0)
        action_name = "Stay";
    else
    if (action.compare("alert") == 0)
        action_name = "Alert";
    else
    if (action.compare("cancel") == 0)
        action_name = "Cancel";
    else
    if (action.compare("finish") == 0)
        action_name = "Finish";
    else
    if (action.compare("exceed") == 0)
        action_name = "Exceed";

    return action_name;
}

std::string get_time_name(std::string issuetime)
{
    std::string time_name = issuetime;

    if (issuetime.compare("0") == 0)
        time_name = "Immediately";
    else
    if (issuetime.compare("1") == 0)
        time_name = "1 minute";
    else
    if (issuetime.compare("5") == 0)
        time_name = "5 minutes";
    else
    if (issuetime.compare("10") == 0)
        time_name = "10 minutes";

    return time_name;
}

std::list<PolygonStruct*> * getPolygonPoints(string geojson)
{
    std::list<PolygonStruct*> * polygonList = new std::list<PolygonStruct*>();

    Json::Reader * reader = new Json::Reader();
    Json::Value result;
    bool parsingSuccessful = reader->parse( geojson.c_str(), result );
    delete reader;

    if ( parsingSuccessful )
    {
        if (result.isMember("geometry"))
        {
            //printf("MySQLDBAccess::getPolygonPoints result.isMember(geometry) ...\n");

            //printf ("geometry.type = %s\n", result["geometry"]["type"].asCString());
            if ( result["geometry"]["coordinates"].isArray() )
            {
                //printf("MySQLDBAccess::getPolygonPoints result[geometry][coordinates].isArray()  ...\n");

                Json::Value coordinates = result["geometry"]["coordinates"];
                //printf("MySQLDBAccess::getPolygonPoints coordinates.size() = %d\n", coordinates.size());

                for(unsigned int i=0;i<coordinates.size();i++)
                {
                    Json::Value coordinate = coordinates[i];
                    PolygonStruct * poly = new PolygonStruct();
                    poly->coordinates = new std::list<PointStruct*>();

                    if ( coordinate.isArray() )
                    {
                        //printf("MySQLDBAccess::getPolygonPoints coordinate.isArray() ...\n");
                        //printf("MySQLDBAccess::getPolygonPoints coordinate.size() = %d\n", coordinate.size());

                        for(unsigned int j=0;j<coordinate.size();j++)
                        {
                            PointStruct * point = new PointStruct();
                            point->x = coordinate[j][(unsigned int)0].asDouble();
                            point->y = coordinate[j][(unsigned int)1].asDouble();

                            poly->coordinates->push_back(point);
                        }
                    }
                    polygonList->push_back(poly);
                }
            }
        }
    }

    return polygonList;
}

void process_mem_usage(double& vm_usage, double& resident_set)
{
    vm_usage     = 0.0;
    resident_set = 0.0;

    // the two fields we want
    unsigned long vsize;
    long rss;
    {
        std::string ignore;
        std::ifstream ifs("/proc/self/stat", std::ios_base::in);
        ifs >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
                >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
                >> ignore >> ignore >> vsize >> rss;
    }

    long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
    vm_usage = vsize / 1024.0;
    resident_set = rss * page_size_kb;
}

long getMemoryUsage()
{
    // struct rusage {
    //     struct timeval ru_utime; /* user time used */
    //     struct timeval ru_stime; /* system time used */
    //     long   ru_maxrss;        /* maximum resident set size */
    //     long   ru_ixrss;         /* integral shared memory size */
    //     long   ru_idrss;         /* integral unshared data size */
    //     long   ru_isrss;         /* integral unshared stack size */
    //     long   ru_minflt;        /* page reclaims */
    //     long   ru_majflt;        /* page faults */
    //     long   ru_nswap;         /* swaps */
    //     long   ru_inblock;       /* block input operations */
    //     long   ru_oublock;       /* block output operations */
    //     long   ru_msgsnd;        /* messages sent */
    //     long   ru_msgrcv;        /* messages received */
    //     long   ru_nsignals;      /* signals received */
    //     long   ru_nvcsw;         /* voluntary context switches */
    //     long   ru_nivcsw;        /* involuntary context switches */
    // };


    struct rusage usage;
    if(0 == getrusage(RUSAGE_SELF, &usage))
        return usage.ru_maxrss; // bytes
    else
        return 0;
}

// long long CurrentSystemRAMInfo()
// {
//     vm_size_t page_size;
//     mach_port_t mach_port;
//     mach_msg_type_number_t count;
//     vm_statistics64_data_t vm_stats;
//
//     long long used_memory = 0;
//     long long free_memory = 0;
//
//     mach_port = mach_host_self();
//     count = sizeof(vm_stats) / sizeof(natural_t);
//     if (KERN_SUCCESS == host_page_size(mach_port, &page_size) &&
//         KERN_SUCCESS == host_statistics64(mach_port, HOST_VM_INFO,
//                                           (host_info64_t)&vm_stats, &count))
//     {
//         free_memory = (int64_t)vm_stats.free_count * (int64_t)page_size;
//
//         used_memory = ((int64_t)vm_stats.active_count +
//                                  (int64_t)vm_stats.inactive_count +
//                                  (int64_t)vm_stats.wire_count) *  (int64_t)page_size;
// //        vilslog::printf("free memory[%lld] used memory[%lld]\n", free_memory, used_memory);
//     }
//
//     return used_memory;
// }

// long long CurrentRAMUsed()
// {
//     long long used_memory = 0;
//     long long virtual_size = 0;
//     struct task_basic_info t_info;
//     mach_msg_type_number_t t_info_count = TASK_BASIC_INFO_COUNT;
//
//     if (KERN_SUCCESS != task_info(mach_task_self(),
//                                   TASK_BASIC_INFO, (task_info_t)&t_info,
//                                   &t_info_count))
//     {
//         return -1;
//     }
//     // resident size is in t_info.resident_size;
//     // virtual size is in t_info.virtual_size;
//
//     used_memory = t_info.resident_size;
//     virtual_size = t_info.virtual_size;
//
//     //vilslog::printf("used_memory[%lld] virtual_size[%lld]\n", used_memory, virtual_size);
//     return used_memory;
// }

std::string filterValidNumberString(const char * inString, int length)
{
    std::string validString = "";
    int stringLen = (int)strlen(inString);
    for(int i=0;i<stringLen;i++)
    {
        if ( (inString[i] >= '0' && inString[i] <= '9')  || inString[i] == '.'  || inString[i] == '-')
        {
            validString.push_back(inString[i]);
        }
    }
    return validString;
}

void test_Locator_compress()
{
    std::string testData = "12841302a0610000000000002ff2b41312a04100000000003210302ff2b442a1510a102a02b492a0000f700fcfefe0309ff000200fff703fe002b562a00b0290d0a";

    // Tag amount : 3.0
    //
    // 0 - 0610000000000002    337 2.9(0 or 2.9)   0000f700fcfe
    // 1 - 0410000000000321    161 2.9(0 or 2.9)   fe0309ff0002
    // 2 - 0410000000000302    42  4.0             00fff703fe00

    // find tags (A0*0610000000000002ff+A1*04100000000003210302ff

    std::string rawData = "";
    std::string dataStartPattern("28"); // '('
    std::string::size_type dataStartPos = testData.find(dataStartPattern);
    if (dataStartPos != std::string::npos)
    {
        rawData = testData.substr(dataStartPos);
        dataStartPos = 0;
    }

    printf("rawData[%s]\n", rawData.c_str());

    std::string macEndPattern("ff");
    std::string::size_type ftMacStIdx = dataStartPos + (4*2);
    std::string::size_type ftMacEndIdx = rawData.find(macEndPattern);

    std::vector<std::string::size_type> macStIdx;
    std::vector<std::string::size_type> macEndIdx;

    //printf("ftMacStIdx[%lu] ftMacEndIdx[%lu] tag[%s]\n", ftMacStIdx, ftMacEndIdx, rawData.substr(ftMacStIdx, ftMacEndIdx-ftMacStIdx).c_str());

    macStIdx.push_back(ftMacStIdx);
    macEndIdx.push_back(ftMacEndIdx);

    int commonMacCnt = 1;
    std::string::size_type start_pos = macEndIdx[commonMacCnt - 1] + (2*2);
    std::string nextValue = rawData.substr(start_pos, 2);
    //printf("start_pos[%lu] nextValue[%s]\n", start_pos, nextValue.c_str());

    while (nextValue.compare("44") != 0)
    {
        start_pos = macEndIdx[commonMacCnt - 1] + (5*2);
        macStIdx.push_back(start_pos);
        commonMacCnt += 1;
        std::string::size_type tmpEndIdx = rawData.find(macEndPattern, start_pos);
        macEndIdx.push_back(tmpEndIdx);

        //printf("tag[%s]\n", rawData.substr(start_pos + (5*2), tmpEndIdx - (start_pos + (5*2))).c_str());

        start_pos = macEndIdx[commonMacCnt - 1] + (2*2);
        nextValue = rawData.substr(start_pos, 2);
        //printf("start_pos[%lu] nextValue[%s]\n", start_pos, nextValue.c_str());
    }

    int tagAmount = 0;
    for(int i=0;i<commonMacCnt;i++)
    {
        std::string::size_type start_idx = macStIdx[i];
        std::string::size_type end_idx = macEndIdx[i];
        std::string tagValue = rawData.substr(start_idx, end_idx - start_idx);
        int tagCnt = (int)(tagValue.length()/2 - 8)/2;
        tagAmount += 1;
        tagAmount += tagCnt;
        //printf("i[%d] start_idx[%lu] end_idx[%lu] tagValue[%s] tagAmount[%d]\n", i, start_idx, end_idx, tagValue.c_str(), tagAmount);
    }

    int distanceStIdx = (int)macEndIdx[commonMacCnt - 1] + (4*2);
    int distanceEndIdx = distanceStIdx + (int)(ceil((float)(tagAmount*12)/8.0)*2);
    std::string distanceList = rawData.substr(distanceStIdx, distanceEndIdx - distanceStIdx);
    int distanceOffset = 0;
	if((tagAmount*12)%8 > 0)
    {
        distanceOffset = 4;
    }
    int64_t distanceIntValue = parseInt(distanceList, 16);
    //printf("distanceStIdx[%d] distanceEndIdx[%d]\n", distanceStIdx, distanceEndIdx);
    printf("distanceList[%s] distanceIntValue[%d] distanceOffset[%d]\n", distanceList.c_str(), (int)distanceIntValue, distanceOffset);

    int imuStIdx = distanceEndIdx + (3*2);
    int imuEndIdx = imuStIdx + ((tagAmount * 6) * 2);
    std::string imuList = rawData.substr(imuStIdx, imuEndIdx - imuStIdx);

    printf("imuStIdx[%d] imuEndIdx[%d]\n", imuStIdx, imuEndIdx);
    printf("imuList[%s]\n", imuList.c_str());

    int voltageStIdx = imuEndIdx + (3*2);
    int voltageEndIdx = voltageStIdx + (int)(ceil((float)(tagAmount*4)/8.0)*2);
    std::string voltageList = rawData.substr(voltageStIdx, voltageEndIdx - voltageStIdx);
    int64_t voltageIntValue = parseInt(voltageList, 16);

    printf("voltageStIdx[%d] voltageEndIdx[%d]\n", voltageStIdx, voltageEndIdx);
    printf("voltageList[%s] voltageIntValue[%d]\n", voltageList.c_str(), (int)voltageIntValue);

    printf("tagAmount:%d\n", tagAmount);

    int tagIdx = 0;
    for(int i=0;i<commonMacCnt;i++)
    {
        std::string::size_type start_idx = macStIdx[i];
        std::string::size_type end_idx = macEndIdx[i];
        std::string tagValue = rawData.substr(start_idx, end_idx - start_idx);
        int tagCnt = (int)(tagValue.length()/2 - 8)/2;
        tagCnt++;

        std::string tagPrefixMac = rawData.substr(start_idx, 6*2);
        //printf("i[%d] tagCnt[%d] tagValue[%s] tagPrefixMac[%s]\n", i, tagCnt, tagValue.c_str(), tagPrefixMac.c_str());

        for(int j=0;j<tagCnt;j++)
        {
            std::string tagMacaddress = tagPrefixMac;
            tagMacaddress.append(rawData.substr(start_idx + (6*2) + (4*j), 4));
            printf("[%d] tagmac[%s]", tagIdx, tagMacaddress.c_str());

            int distanceShiftBit = ((tagAmount - tagIdx - 1)*12) + distanceOffset;
            int distance = (distanceIntValue >> distanceShiftBit) & 0xfff;

            printf(" distance[%d]", distance);

            int voltageShiftBit = (tagAmount - tagIdx - 1) * 4 + distanceOffset;
            int voltageInt = ((voltageIntValue >> voltageShiftBit) & 0xf)+29;
            float voltageFloat = (float)(voltageInt)/10.0;

            printf(" voltage[%f]", voltageFloat);

            std::string imuValue = rawData.substr(imuStIdx + ((tagIdx*6)*2), 12);
            printf(" imu[%s]\n", imuValue.c_str());

            // int imu[6] = {0,0,0,0,0,0};
            // convertHexIMUtoInt(imuValue.c_str(), imu);

            tagIdx++;
        }
    }
}

void test_Locator_compress_1()
{
    //std::string testData = "01280610000000000002ff04100000000003210302ff2b442a1510a102a020fe03ff00b029";
    //std::string testData = "01280310000000000021ff0410000000000302ff2b442a0001c60001f929";
    //std::string testData = "280410000000000005002500040035ff03100000000001950135ff2b442a33817f2d01192e0176000000000000efbebb29";
    std::string testData = "280410000000000004003500050025ff0610000000000128ff031000000000019501350145ff2b442a2ca11c31b18000031332e3540000000000000100deef0ccb29";

    // Tag amount : 3.0
    //
    // 0 - 0610000000000002    337 2.9(0 or 2.9)   fe
    // 1 - 0410000000000321    161 2.9(0 or 2.9)   03
    // 2 - 0410000000000302    42  4.0             ff

    // find tags 0610000000000002ff04100000000003210302ff

    std::string rawData = "";
    std::string dataStartPattern("28"); // '('
    std::string::size_type dataStartPos = testData.find(dataStartPattern);
    if (dataStartPos != std::string::npos)
    {
        rawData = testData.substr(dataStartPos);
        dataStartPos = 0;
    }

    printf("rawData[%s]\n", rawData.c_str());

    std::string macEndPattern("ff");
    std::string::size_type ftMacStIdx = dataStartPos + 2;
    std::string::size_type ftMacEndIdx = rawData.find(macEndPattern);

    std::vector<std::string::size_type> macStIdx;
    std::vector<std::string::size_type> macEndIdx;

    printf("ftMacStIdx[%lu] ftMacEndIdx[%lu] tag[%s]\n", ftMacStIdx, ftMacEndIdx, rawData.substr(ftMacStIdx, ftMacEndIdx-ftMacStIdx).c_str());

    macStIdx.push_back(ftMacStIdx);
    macEndIdx.push_back(ftMacEndIdx);

    int commonMacCnt = 1;
    std::string::size_type start_pos = macEndIdx[commonMacCnt - 1] + 2;
    std::string nextValue = rawData.substr(start_pos, 2);

    printf("start_pos[%lu] nextValue[%s]\n", start_pos, nextValue.c_str());

    while (nextValue.compare("2b") != 0)// 0x2b = '+'
    {
        start_pos = macEndIdx[commonMacCnt - 1] + 2;
        macStIdx.push_back(start_pos);
        commonMacCnt += 1;
        std::string::size_type tmpEndIdx = rawData.find(macEndPattern, start_pos);
        macEndIdx.push_back(tmpEndIdx);

        printf("tag[%s]\n", rawData.substr(start_pos, tmpEndIdx - start_pos).c_str());

        start_pos = macEndIdx[commonMacCnt - 1] + 2;
        nextValue = rawData.substr(start_pos, 2);

        printf("start_pos[%lu] nextValue[%s]\n", start_pos, nextValue.c_str());
    }

    int tagAmount = 0;
    for(int i=0;i<commonMacCnt;i++)
    {
        std::string::size_type start_idx = macStIdx[i];
        std::string::size_type end_idx = macEndIdx[i];
        std::string tagValue = rawData.substr(start_idx, end_idx - start_idx);
        int tagCnt = (int)(tagValue.length()/2 - 8)/2;
        tagAmount += 1;
        tagAmount += tagCnt;
        printf("i[%d] start_idx[%lu] end_idx[%lu] tagValue[%s] tagAmount[%d]\n", i, start_idx, end_idx, tagValue.c_str(), tagAmount);
    }

    int distanceStIdx = (int)macEndIdx[commonMacCnt - 1] + (4*2);
    int distanceEndIdx = distanceStIdx + (int)(ceil((float)(tagAmount*12)/8.0)*2);
    std::string distanceList = rawData.substr(distanceStIdx, distanceEndIdx - distanceStIdx);
    // int distanceOffset = 0;
	// if((tagAmount*12)%8 > 0)
    // {
    //     distanceOffset = 1;
    // }
    // int64_t distanceIntValue = parseInt(distanceList, 16);
    printf("distanceStIdx[%d] distanceEndIdx[%d] distanceList[%s] \n", distanceStIdx, distanceEndIdx, distanceList.c_str());
    //printf("distanceList[%s] distanceOffset[%d]\n", distanceList.c_str(), distanceOffset);

    int velStIdx = distanceEndIdx;
    int velEndIdx = velStIdx + ((tagAmount * 1) * 2);
    std::string velList = rawData.substr(velStIdx, velEndIdx - velStIdx);

    printf("velStIdx[%d] velEndIdx[%d]\n", velStIdx, velEndIdx);
    printf("velList[%s]\n", velList.c_str());

    int voltageStIdx = velEndIdx;
    int voltageEndIdx = voltageStIdx + (int)(ceil((float)(tagAmount*4)/8.0)*2);
    std::string voltageList = rawData.substr(voltageStIdx, voltageEndIdx - voltageStIdx);
    //int64_t voltageIntValue = parseInt(voltageList, 16);

    printf("voltageStIdx[%d] voltageEndIdx[%d] voltageList[%s] \n", voltageStIdx, voltageEndIdx, voltageList.c_str());
    //printf("voltageList[%s] voltageIntValue[%lld]\n", voltageList.c_str(), voltageIntValue);

    printf("tagAmount:%d\n", tagAmount);

    int tagIdx = 0;
    for(int i=0;i<commonMacCnt;i++)
    {
        std::string::size_type start_idx = macStIdx[i];
        std::string::size_type end_idx = macEndIdx[i];
        std::string tagValue = rawData.substr(start_idx, end_idx - start_idx);
        int tagCnt = (int)(tagValue.length()/2 - 8)/2;
        tagCnt++;

        std::string tagPrefixMac = rawData.substr(start_idx, 6*2);
        //printf("i[%d] tagCnt[%d] tagValue[%s] tagPrefixMac[%s]\n", i, tagCnt, tagValue.c_str(), tagPrefixMac.c_str());

        for(int j=0;j<tagCnt;j++)
        {
            std::string tagMacaddress = tagPrefixMac;
            tagMacaddress.append(rawData.substr(start_idx + (6*2) + (4*j), 4));
            printf("[%d] tagmac[%s]", tagIdx, tagMacaddress.c_str());

            // int distanceShiftBit = ((tagAmount - tagIdx - 1)*12) + distanceOffset;
            // int64_t distance = (distanceIntValue >> (int64_t)distanceShiftBit) & 0xfff;
            //int distanceShiftChar = (tagAmount - tagIdx - 1)*3 + distanceOffset;
            std::string distanceStr = rawData.substr(distanceStIdx + tagIdx*3, 3);
            int distance = (int)parseInt(distanceStr, 16);

            printf(" distanceStr[%s] distance[%d]", distanceStr.c_str(), distance);

            //int voltageShiftBit = (tagAmount - tagIdx - 1) * 4 + distanceOffset;
            //int voltageInt = ((voltageIntValue >> voltageShiftBit) & 0xf)+29;
            std::string voltageStr = rawData.substr(voltageStIdx + tagIdx*1, 1);
            int voltageInt = (int)parseInt(voltageStr, 16)+29;
            float voltageFloat = (float)(voltageInt)/10.0;

            printf(" voltage[%f]", voltageFloat);

            std::string velValue = rawData.substr(velStIdx + (tagIdx * 2), 2);
            printf(" velValue[0x%s][%ld]\n", velValue.c_str(), strtol(velValue.c_str(), NULL, 16));

            // int imu[6] = {0,0,0,0,0,0};
            // convertHexIMUtoInt(imuValue.c_str(), imu);

            tagIdx++;
        }
    }

}

int convertHexIMUtoInt(const char * imustr, int * imu)
{
    int datalen = (int)strlen(imustr);
    if (datalen != 12)
    {
        return 1;
    }
    for(int i=0;i<6;i++)
    {
        char hexValue[3];
        hexValue[0] = imustr[i*2+0];
        hexValue[1] = imustr[i*2+1];
        hexValue[2] = '\0';
        imu[i] = (int)strtol(hexValue, NULL, 16);

        // printf("convertHexIMUtoInt() imu[%d] imu[0x%x] v[0x%x]\n", imu[i], imu[i], imu[i] & 0x80);
        if ((imu[i] & 0x80) == 0x80)
        {
            imu[i] -= 256;
        }

        // int signedIntvalue;
        // if (sscanf(hexValue, "%x", &signedIntvalue) == 1)
        // {
        //     imu[i] = signedIntvalue;
        // }

        // printf("convertHexIMUtoInt() hexValue[%s] imu[0x%x] imu[%d]\n", hexValue, imu[i], imu[i]);
    }
    return 0;
}
// Adjust date by a number of days +/-
void DatePlusDays( struct tm* date, int days )
{
    const time_t ONE_DAY = 24 * 60 * 60 ;

    // Seconds since start of epoch
    time_t date_seconds = mktime( date ) + (days * ONE_DAY) ;

    // Update caller's date
    // Use localtime because mktime converts to UTC so may change date
    *date = *localtime( &date_seconds ) ; ;
}

/*****************************************************************************
 * 將一個字符的Unicode(UCS-2和UCS-4)編碼轉換成UTF-8編碼.
 *
 * 参數:
 *    unic     字符的Unicode編碼值
 *    pOutput  指向輸出的用於存儲UTF8編碼值的緩沖區的指針
 *    outsize  pOutput緩沖的大小
 *
 * 返回值:
 *    返回轉換後的字符的UTF8編碼所占的字節數, 如果出錯則返回 0 .
 *
 * 注意:
 *     1. UTF8沒有字節序問題, 但是Unicode有字節序要求;
 *        字節序分为大端(Big Endian)和小端(Little Endian)兩種;
 *        在Intel處理器中采用小端法表示, 在此采用小端法表示. (低地址存低位)
 *     2. 請保證 pOutput 緩沖區有最少有 6 字節的空間大小!
 ****************************************************************************/
int enc_unicode_to_utf8_one(unsigned long unic, unsigned char *pOutput, int outSize)
{
    assert(pOutput != NULL);
    assert(outSize >= 6);

    if ( unic <= 0x0000007F )
    {
        // * U-00000000 - U-0000007F:  0xxxxxxx
        *pOutput     = (unic & 0x7F);
        return 1;
    }
    else if ( unic >= 0x00000080 && unic <= 0x000007FF )
    {
        // * U-00000080 - U-000007FF:  110xxxxx 10xxxxxx
        *(pOutput+1) = (unic & 0x3F) | 0x80;
        *pOutput     = ((unic >> 6) & 0x1F) | 0xC0;
        return 2;
    }
    else if ( unic >= 0x00000800 && unic <= 0x0000FFFF )
    {
        // * U-00000800 - U-0000FFFF:  1110xxxx 10xxxxxx 10xxxxxx
        *(pOutput+2) = (unic & 0x3F) | 0x80;
        *(pOutput+1) = ((unic >>  6) & 0x3F) | 0x80;
        *pOutput     = ((unic >> 12) & 0x0F) | 0xE0;
        return 3;
    }
    else if ( unic >= 0x00010000 && unic <= 0x001FFFFF )
    {
        // * U-00010000 - U-001FFFFF:  11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        *(pOutput+3) = (unic & 0x3F) | 0x80;
        *(pOutput+2) = ((unic >>  6) & 0x3F) | 0x80;
        *(pOutput+1) = ((unic >> 12) & 0x3F) | 0x80;
        *pOutput     = ((unic >> 18) & 0x07) | 0xF0;
        return 4;
    }
    else if ( unic >= 0x00200000 && unic <= 0x03FFFFFF )
    {
        // * U-00200000 - U-03FFFFFF:  111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        *(pOutput+4) = (unic & 0x3F) | 0x80;
        *(pOutput+3) = ((unic >>  6) & 0x3F) | 0x80;
        *(pOutput+2) = ((unic >> 12) & 0x3F) | 0x80;
        *(pOutput+1) = ((unic >> 18) & 0x3F) | 0x80;
        *pOutput     = ((unic >> 24) & 0x03) | 0xF8;
        return 5;
    }
    else if ( unic >= 0x04000000 && unic <= 0x7FFFFFFF )
    {
        // * U-04000000 - U-7FFFFFFF:  1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
        *(pOutput+5) = (unic & 0x3F) | 0x80;
        *(pOutput+4) = ((unic >>  6) & 0x3F) | 0x80;
        *(pOutput+3) = ((unic >> 12) & 0x3F) | 0x80;
        *(pOutput+2) = ((unic >> 18) & 0x3F) | 0x80;
        *(pOutput+1) = ((unic >> 24) & 0x3F) | 0x80;
        *pOutput     = ((unic >> 30) & 0x01) | 0xFC;
        return 6;
    }

    return 0;
}

/*****************************************************************************
 * 將一個字符的UTF8編碼轉換成Unicode(UCS-2和UCS-4)編碼.
 *
 * 参數:
 *    pInput      指向輸入緩沖區, 以UTF-8編碼
 *    Unic        指向輸出緩沖區, 其保存的數據即是Unicode編碼值,
 *                類型为unsigned long .
 *
 * 返回值:
 *    成功則返回該字符的UTF8編碼所占用的字節數; 失敗則返回0.
 *
 * 注意:
 *     1. UTF8沒有字節序問題, 但是Unicode有字節序要求;
 *        字節序分为大端(Big Endian)和小端(Little Endian)兩種;
 *        在Intel處理器中采用小端法表示, 在此采用小端法表示. (低地址存低位)
 ****************************************************************************/
int enc_utf8_to_unicode_one(const unsigned char* pInput, unsigned long *Unic)
{
    assert(pInput != NULL && Unic != NULL);

    // b1 表示UTF-8編碼的pInput中的高字節, b2 表示次高字節, ...
    char b1, b2, b3, b4, b5, b6;

    *Unic = 0x0; // 把 *Unic 初始化为全零
    int utfbytes = enc_get_utf8_size(*pInput);
    unsigned char *pOutput = (unsigned char *) Unic;

    switch ( utfbytes )
    {
        case 0:
            *pOutput     = *pInput;
            utfbytes    += 1;
            break;
        case 2:
            b1 = *pInput;
            b2 = *(pInput + 1);
            if ( (b2 & 0xE0) != 0x80 )
                return 0;
            *pOutput     = (b1 << 6) + (b2 & 0x3F);
            *(pOutput+1) = (b1 >> 2) & 0x07;
            break;
        case 3:
            b1 = *pInput;
            b2 = *(pInput + 1);
            b3 = *(pInput + 2);
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80) )
                return 0;
            *pOutput     = (b2 << 6) + (b3 & 0x3F);
            *(pOutput+1) = (b1 << 4) + ((b2 >> 2) & 0x0F);
            break;
        case 4:
            b1 = *pInput;
            b2 = *(pInput + 1);
            b3 = *(pInput + 2);
            b4 = *(pInput + 3);
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)
                    || ((b4 & 0xC0) != 0x80) )
                return 0;
            *pOutput     = (b3 << 6) + (b4 & 0x3F);
            *(pOutput+1) = (b2 << 4) + ((b3 >> 2) & 0x0F);
            *(pOutput+2) = ((b1 << 2) & 0x1C)  + ((b2 >> 4) & 0x03);
            break;
        case 5:
            b1 = *pInput;
            b2 = *(pInput + 1);
            b3 = *(pInput + 2);
            b4 = *(pInput + 3);
            b5 = *(pInput + 4);
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)
                    || ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80) )
                return 0;
            *pOutput     = (b4 << 6) + (b5 & 0x3F);
            *(pOutput+1) = (b3 << 4) + ((b4 >> 2) & 0x0F);
            *(pOutput+2) = (b2 << 2) + ((b3 >> 4) & 0x03);
            *(pOutput+3) = (b1 << 6);
            break;
        case 6:
            b1 = *pInput;
            b2 = *(pInput + 1);
            b3 = *(pInput + 2);
            b4 = *(pInput + 3);
            b5 = *(pInput + 4);
            b6 = *(pInput + 5);
            if ( ((b2 & 0xC0) != 0x80) || ((b3 & 0xC0) != 0x80)
                    || ((b4 & 0xC0) != 0x80) || ((b5 & 0xC0) != 0x80)
                    || ((b6 & 0xC0) != 0x80) )
                return 0;
            *pOutput     = (b5 << 6) + (b6 & 0x3F);
            *(pOutput+1) = (b5 << 4) + ((b6 >> 2) & 0x0F);
            *(pOutput+2) = (b3 << 2) + ((b4 >> 4) & 0x03);
            *(pOutput+3) = ((b1 << 6) & 0x40) + (b2 & 0x3F);
            break;
        default:
            return 0;
            break;
    }

    return utfbytes;
}

int enc_get_utf8_size(const unsigned char pInput)
{
    unsigned char c = pInput;
    // 0xxxxxxx 返回0
    // 10xxxxxx 不存在
    // 110xxxxx 返回2
    // 1110xxxx 返回3
    // 11110xxx 返回4
    // 111110xx 返回5
    // 1111110x 返回6
    if(c< 0x80) return 0;
    if(c>=0x80 && c<0xC0) return -1;
    if(c>=0xC0 && c<0xE0) return 2;
    if(c>=0xE0 && c<0xF0) return 3;
    if(c>=0xF0 && c<0xF8) return 4;
    if(c>=0xF8 && c<0xFC) return 5;
    if(c>=0xFC) return 6;
    return 0;
}

int create_qrcode(const char * message, const char * outputfile, int pixel_size)
{
    // Create the QR code
    QRCode qrcode;
    uint8_t qrcodeData[qrcode_getBufferSize(4)];
    qrcode_initText(&qrcode, qrcodeData, 4, 0, message);

    uint8_t top_bottom_margin = 20;
    uint8_t left_right_margin = 20;

    int bmp_width = qrcode.size * pixel_size + (left_right_margin * 2);
    int bmp_height = qrcode.size * pixel_size + (top_bottom_margin * 2);

    //vilslog::printf("create_qrcode() bmp_width[%d] bmp_height[%d]\n", bmp_width, bmp_height);

	//BMP * bmp = BMP_ReadFile( "qrcode.bmp" );
    BMP * bmp = BMP_Create( bmp_width, bmp_height, 24);
    BMP_CHECK_ERROR( stdout, -1 );

    // top space
    for (int x=0;x<bmp_width;x++)
    {
        for(int y=0;y<top_bottom_margin;y++)
        {
            BMP_SetPixelRGB( bmp, x, y, 255, 255, 255);
        }
    }

    for (int y=0;y<qrcode.size;y++)
    {
        // Left quiet zone
        int top_y = top_bottom_margin + (y*pixel_size);

        for (int marx=0;marx<left_right_margin;marx++)
        {
            for(int mary=0;mary<pixel_size;mary++)
            {
                BMP_SetPixelRGB( bmp, marx, top_y + mary, 255, 255, 255);
            }
        }

        for (int x=0;x<qrcode.size;x++)
        {
            //vilslog::printf("%s",qrcode_getModule(&qrcode, x, y) ? "*" : " ");
            int pixel = qrcode_getModule(&qrcode, x, y) ? 0 : 255;

            for(int i=0;i<pixel_size;i++)
            {
                for(int j=0;j<pixel_size;j++)
                {
                    BMP_SetPixelRGB( bmp, left_right_margin + (x*pixel_size)+i, top_y+j, pixel, pixel, pixel);
                }
            }

        }
        // eight quiet zone
        int left_x = left_right_margin + (qrcode.size*pixel_size);

        for (int marx=0;marx<left_right_margin;marx++)
        {
            for(int mary=0;mary<pixel_size;mary++)
            {
                BMP_SetPixelRGB( bmp,  left_x+marx, top_y+mary, 255, 255, 255);
            }
        }
    }

    //bottom space
    int bottom_space_top = qrcode.size * pixel_size + top_bottom_margin;

    for (int x=0;x<bmp_width;x++)
    {
        for(int y=0;y<top_bottom_margin;y++)
        {
            BMP_SetPixelRGB( bmp, x, bottom_space_top + y, 255, 255, 255);
        }
    }

    BMP_WriteFile( bmp, outputfile );
    BMP_CHECK_ERROR( stdout, -2 );
	BMP_Free( bmp );

    return 0;
}

//
// https://www.itread01.com/content/1548509606.html
// https://www.freeformatter.com/hmac-generator.html#ad-output
//
std::string create_MD5_encrypt(std::string inkey, std::string password)
{
    char key[128] = {'\0'};
    char data[128] = {'\0'};

    unsigned int result_len = 16;

    static char res_hexstring[40];
    unsigned char digest[EVP_MAX_MD_SIZE] = {'\0'};
    unsigned int digest_len = 0;

    sprintf(&key[0], "%s", inkey.c_str());
    sprintf(&data[0], "%s", password.c_str());
    // sprintf(&key[0], "%s", "2012121220121212201212122012121220121212201212122012121220121212");
    // sprintf(&data[0], "%s", "johndoejohndoejohndoejohndoejohndoejohndoejohndoejohndoejohndoejohndoejohndoejohndoe");
    //
    // inkey = "2012121220121212201212122012121220121212201212122012121220121212";
    // password = "johndoejohndoejohndoejohndoejohndoejohndoejohndoejohndoejohndoejohndoejohndoejohndoe";

    HMAC(EVP_md5(), (unsigned char*)key, (int)inkey.length(), (unsigned char*)data, password.length(), digest, &digest_len);
    for (unsigned int i = 0; i < result_len; i++)
    {
        sprintf(&(res_hexstring[i * 2]), "%02X", digest[i]);
    }

    std::string pwd = res_hexstring;

    //========================
    // Be careful of the length of string with the choosen hash engine. SHA1 needed 20 characters.
        // Change the length accordingly with your choosen hash engine.
        // unsigned char* result_test;
        // unsigned int len = 16;
        //
        // result_test = (unsigned char*)malloc(sizeof(char) * len);
        //
        // HMAC_CTX ctx;
        // HMAC_CTX_init(&ctx);
        //
        // // Using sha1 hash engine here.
        // // You may use other hash engines. e.g EVP_md5(), EVP_sha224, EVP_sha512, etc
        // //HMAC_Init_ex(&ctx, key, inkey.length(), EVP_sha1(), NULL);
        // //HMAC_Init_ex(&ctx, key, inkey.length(), EVP_md5(), NULL);
        // //HMAC_Update(&ctx, (unsigned char*)&data, password.length());
        // HMAC_Init_ex(&ctx, key, inkey.length(), EVP_md5(), NULL);
        // HMAC_Update(&ctx, (unsigned char*)&data, password.length());
        // HMAC_Final(&ctx, result_test, &len);
        // HMAC_CTX_cleanup(&ctx);
        //
        // printf("HMAC digest: ");
        //
        // for (int i = 0; i != len; i++)
        //     printf("%02X", (unsigned int)result_test[i]);
        //
        // printf("\n");
        //
        // free(result_test);
//========================


    return pwd;
}
std::string create_MD5_encrypt_salt(std::string salt, std::string password)
{
    char password_with_salt[512] = {'\0'};
    unsigned char digest[MD5_DIGEST_LENGTH] = {'\0'};
    unsigned int result_len = 16;
    static char res_hexstring[40];

    sprintf(&password_with_salt[0], "%s%s", password.c_str(), salt.c_str());

    MD5((unsigned char *)password_with_salt, strlen(password_with_salt), digest);
    for (unsigned int i = 0; i < result_len; i++)
    {
        sprintf(&(res_hexstring[i * 2]), "%02X", digest[i]);
    }

    std::string pwd = res_hexstring;
    return pwd;
}

std::string random_string( size_t length )
{
    static int firstIn = 1;

    if (firstIn == 1)
    {
        srand( (unsigned)time(NULL) );
        firstIn = 0;
    }

    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}

FiredEventStruct * copyFiredEventStruct(FiredEventStruct * srcEvent)
{
    FiredEventStruct * dstEvent = new FiredEventStruct();
    dstEvent->firedeventid = srcEvent->firedeventid;
    dstEvent->projectid = srcEvent->projectid;
    dstEvent->eventid = srcEvent->eventid;
    dstEvent->empid = srcEvent->empid;
    dstEvent->comment = srcEvent->comment;
    dstEvent->prettyjson = srcEvent->prettyjson;
    dstEvent->eventjson = srcEvent->eventjson;
    dstEvent->timestart = srcEvent->timestart;
    dstEvent->timeend = srcEvent->timeend;
    dstEvent->readdatetime = srcEvent->readdatetime;
    dstEvent->datetime = srcEvent->datetime;
    return dstEvent;
}

AreaInOutEventStruct * copyAreaInOutEventStruct(AreaInOutEventStruct * srcEvent)
{
    AreaInOutEventStruct * dstEvent = new AreaInOutEventStruct();
    dstEvent->action = srcEvent->action; //1:in 2:out
    dstEvent->AreaType0D = srcEvent->AreaType0D; // 0:normal 1:room 2:danger
    dstEvent->projectid = srcEvent->projectid;
    dstEvent->tafid = srcEvent->tafid; // 0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師
    dstEvent->MapAreaType = srcEvent->MapAreaType;// 0:普通 1:電子圍欄 2:電子點名 3:房間 4:教室 5:病房 6:病床 999:虛擬牆
    dstEvent->NodeId = srcEvent->NodeId;
    dstEvent->NodeName = srcEvent->NodeName;
    dstEvent->NodeMacaddress = srcEvent->NodeMacaddress;
    dstEvent->AreaID = srcEvent->AreaID;
    dstEvent->AreaName = srcEvent->AreaName;
    dstEvent->DataTime = srcEvent->DataTime;
    return dstEvent;
}

bool parseJSON(const char * inputJSON, Json::Value & JSONresult)
{
    Json::Reader * reader = new Json::Reader();
    bool parsingSuccessful = reader->parse( inputJSON, JSONresult);
    delete reader;
    return parsingSuccessful;
}

std::vector<std::string> convert_string_to_vector(std::string srcStr)
{
    std::vector<std::string> resultVector;
    istringstream ids(srcStr.c_str());
    string id;
    while (getline(ids, id, ','))
    {
        resultVector.push_back(id);
    }
    return resultVector;
}

std::vector<std::string> convert_string_to_vector(std::string srcStr, char pattern)
{
    std::vector<std::string> resultVector;
    istringstream ids(srcStr.c_str());
    string id;
    while (getline(ids, id, pattern))
    {
        resultVector.push_back(id);
    }
    return resultVector;
}

std::list<std::string> * convert_string_to_list(std::string srcStr, char pattern)
{
    std::list<std::string> * plist = new std::list<std::string>();

    // split "NodeIDA,NodeIDB,NodeIDC" to std::list<std:string>
    istringstream f(srcStr.c_str());
    string s;
    while (getline(f, s, pattern))
    {
        plist->push_back(s);
    }
    return plist;
}

std::list<int> * convert_string_to_intlist(std::string srcStr, char pattern)
{
    std::list<int> * plist = new std::list<int>();

    istringstream f(srcStr.c_str());
    string s;
    while (getline(f, s, pattern))
    {
        if (s.length() > 0)
        {
            // vilslog::printf("convert_string_to_intlist() s[%s]\n", s.c_str());
            int intValue = stoi(s);
            plist->push_back(intValue);
        }
    }
    return plist;
}

std::string convert_list_to_string(std::list<std::string> * plist)
{
    int count = 0;
    std::stringstream ss;
    for (std::list<std::string>::iterator value = plist->begin(); value != plist->end(); value++)
    {
      if(count != 0)
        ss << ",";
      ss << (*value);
      count++;
    }
    return ss.str();
}

std::string convert_listint_to_string(std::list<int> * plist)
{
    int count = 0;
    std::stringstream ss;
    for (std::list<int>::iterator value = plist->begin(); value != plist->end(); value++)
    {
      if(count != 0)
        ss << ",";
      ss << (*value);
      count++;
    }
    return ss.str();
}

std::string StringToHEXString(std::string inputString)
{
    std::string hexString;
    char input_string[256];
    char res_hexstring[512];

    memset(input_string, 0, 256);
    memset(res_hexstring, 0, 512);

    snprintf(input_string, 256, "%s", inputString.c_str());
    int result_len = strlen(input_string);

    for (int i = 0; i < result_len; i++)
    {
        sprintf(&(res_hexstring[i * 2]), "%02x", input_string[i]);
    }
    hexString = res_hexstring;

    return hexString;
}

std::string HEXStringToString(const char * dataASCII)
{
    char jsondata[512];
    int dataCount = 0;
    int length = (int)strlen(dataASCII);

    memset(jsondata, 0, 512);

    for(int i=0; i<length && (dataCount < 512); i+=2)
    {
        char dataHex[3];
        dataHex[0] = dataASCII[i + 0];
        dataHex[1] = dataASCII[i + 1];
        dataHex[2] = '\0';
        jsondata[dataCount++] = strtol(dataHex, NULL, 16);
        //printf ("dataHex[%s] -> %c\n", dataHex, strtol(dataHex, NULL, 16));
    }

    std::string retString = jsondata;

    return retString;
}

int HEXStringToBuffer(const char * dataASCII, int dataLength, char * dataBuffer)
{
    int dataCount = 0;
    int length = (int)strlen(dataASCII);

    for(int i=0; i<length && (dataCount < dataLength); i+=2)
    {
        char dataHex[3];
        dataHex[0] = dataASCII[i + 0];
        dataHex[1] = dataASCII[i + 1];
        dataHex[2] = '\0';
        dataBuffer[dataCount++] = strtol(dataHex, NULL, 16);
        //printf ("dataHex[%s] -> %c\n", dataHex, strtol(dataHex, NULL, 16));
    }
    return dataCount;
}

std::string remove_newline(std::string inputMsg)
{
    inputMsg.erase( std::remove(inputMsg.begin(), inputMsg.end(), '\r'), inputMsg.end() );
    inputMsg.erase( std::remove(inputMsg.begin(), inputMsg.end(), '\n'), inputMsg.end() );

    // if (!inputMsg.empty() && inputMsg[inputMsg.size() - 1] == '\n')
    //     inputMsg.erase(inputMsg.size() - 1);
    // if (!inputMsg.empty() && inputMsg[inputMsg.size() - 1] == '\r')
    //     inputMsg.erase(inputMsg.size() - 1);
    // if (!inputMsg.empty() && inputMsg[inputMsg.size() - 1] == '\n')
    //     inputMsg.erase(inputMsg.size() - 1);
    return inputMsg;
}

std::string remove_nonASCII(const char * inputString)
{
    char outputString[65536];
    memset(outputString, 0, 65536);

    int cnt = 0;
    int len = strlen(inputString);
    for(int i=0; i<len && i<65536 ;i++)
    {
        if ( (inputString[i] >= '0' && inputString[i] <= '9') ||
             (inputString[i] >= 'a' && inputString[i] <= 'z') ||
             (inputString[i] >= 'A' && inputString[i] <= 'Z') )
        {
            outputString[cnt] = inputString[i];
            cnt++;
        }
    }
    return std::string(outputString);
}

std::string remove_nonJSON(const char * inputString)
{
    char outputString[65536];
    memset(outputString, 0, 65536);

    int cnt = 0;
    int len = strlen(inputString);
    for(int i=0; i<len && i<65536 ;i++)
    {
        if ( (inputString[i] >= '0' && inputString[i] <= '9') ||
             (inputString[i] >= 'a' && inputString[i] <= 'z') ||
             (inputString[i] >= 'A' && inputString[i] <= 'Z') ||
             (inputString[i] == '[' || inputString[i] == ']') ||
             (inputString[i] == '{' || inputString[i] == '}') ||
             (inputString[i] == '"' || inputString[i] == '-') ||
             (inputString[i] == ':' || inputString[i] == ',') ||
             (inputString[i] == '=' || inputString[i] == ' ') )
        {
            outputString[cnt] = inputString[i];
            cnt++;
        }
    }
    return std::string(outputString);
}

std::string remove_EOF(const char * inputString, int msgLen)
{
    char outputString[65536];
    memset(outputString, 0, 65536);

    int cnt = 0;
    for(int i=0; i<msgLen && i<65536 ;i++)
    {
        if ( inputString[i] != '\0' )
        {
            outputString[cnt] = inputString[i];
            cnt++;
        }
    }

    vilslog::printf("remove_EOF() msgLen[%d] cnt[%d]\n", msgLen, cnt);

    return std::string(outputString);
}

#if defined(__APPLE__)
void freeAsMuchAsPossible()
{
    vm_address_t * zones;
    unsigned int count;
    unsigned int i;

    kern_return_t rc =
    malloc_get_all_zones(mach_task_self(), 0, &zones, &count);
    if (0 != rc)
    {
        fprintf(stderr, "rc was %d\n", rc);
    }
    for (i = 0; i < count; ++i)
    {
        malloc_zone_t * zone = (malloc_zone_t*)zones[i];
        char const * name = malloc_get_zone_name(zone);
        if (NULL == name)
        {
            continue;
        }
        malloc_zone_pressure_relief(zone, 0);
    }
}

// heapInUse()
// Gather the heap usage metrics from each zone, using
// malloc_zone_statistics().
void heapInUse( size_t * bytesInUse, size_t * blocksInUse, size_t * sizeAllocated)
{
    *bytesInUse = 0;
    *blocksInUse = 0;
    *sizeAllocated = 0;
    unsigned int i;
    vm_address_t * zones;
    unsigned int count;
    kern_return_t rc =
    malloc_get_all_zones(mach_task_self(), 0, &zones, &count);
    if (0 != rc)
    {
        fprintf(stderr, "rc was %d\n", rc);
    }
    for (i = 0; i < count; ++i)
    {
        malloc_zone_t * zone = (malloc_zone_t*)zones[i];
        char const * name = malloc_get_zone_name(zone);
        if (NULL == name)
        {
            continue;
        }
        malloc_statistics_t stats;
        stats.blocks_in_use = 0;
        stats.size_in_use = 0;
        stats.max_size_in_use = 0;
        stats.size_allocated = 0;
        malloc_zone_statistics(zone, &stats);
        *bytesInUse += stats.size_in_use;
        *blocksInUse += stats.blocks_in_use;
        *sizeAllocated += stats.size_allocated;
    }
}

// heapStats()
// print the output from the mstats() function: total heap bytes,
// used bytes, and free bytes.
void heapStats()
{
    struct mstats ms = mstats();
    malloc_printf("total: %d, used: %d, free: %d\n", ms.bytes_total, ms.bytes_used, ms.bytes_free);
}
#endif

std::string UriEncode(const unsigned char * pSrc)
{
    const char DEC2HEX[16 + 1] = "0123456789ABCDEF";
    //const unsigned char * pSrc = (const unsigned char *)sSrc.c_str();
    //const int SRC_LEN = sSrc.length();
    const int SRC_LEN = strlen((char *)pSrc);
    unsigned char * const pStart = new unsigned char[SRC_LEN * 3];
    unsigned char * pEnd = pStart;
    const unsigned char * const SRC_END = pSrc + SRC_LEN;

    for (; pSrc < SRC_END; ++pSrc)
	{
		if (SAFE[*pSrc])
            *pEnd++ = *pSrc;
        else
        {
            // escape this char
            *pEnd++ = '%';
            *pEnd++ = DEC2HEX[*pSrc >> 4];
            *pEnd++ = DEC2HEX[*pSrc & 0x0F];
        }
	}

    std::string sResult((char *)pStart, (char *)pEnd);
    delete [] pStart;
    return sResult;
}

std::string UriDecode(const unsigned char * pSrc)
{
    // Note from RFC1630:  "Sequences which start with a percent sign
    // but are not followed by two hexadecimal characters (0-9, A-F) are reserved
    // for future extension"

    //const unsigned char * pSrc = (const unsigned char *)sSrc.c_str();
    //const int SRC_LEN = sSrc.length();
    const int SRC_LEN = strlen((char *)pSrc);
    const unsigned char * const SRC_END = pSrc + SRC_LEN;
    const unsigned char * const SRC_LAST_DEC = SRC_END - 2;   // last decodable '%'

    char * const pStart = new char[SRC_LEN];
    char * pEnd = pStart;

    while (pSrc < SRC_LAST_DEC)
    {
        if (*pSrc == '%')
        {
            char dec1, dec2;
            if (-1 != (dec1 = HEX2DEC[*(pSrc + 1)])
                && -1 != (dec2 = HEX2DEC[*(pSrc + 2)]))
            {
                *pEnd++ = (dec1 << 4) + dec2;
                pSrc += 3;
                continue;
            }
        }

        *pEnd++ = *pSrc++;
    }

    // the last 2- chars
    while (pSrc < SRC_END)
        *pEnd++ = *pSrc++;

    std::string sResult(pStart, pEnd);
    delete [] pStart;
    return sResult;
}

long getFreeDiskSpace(const char* absoluteFilePath, SystemInfoStruct & systemInfo)
{
   struct statfs buf;

   if (!statfs(absoluteFilePath, &buf))
   {
      unsigned long long blksize, blocks, availblks, disk_size, free;
      //unsigned long long freeblks = buf.f_bfree;
      //unsigned long long used = disk_size - free;
      blksize = buf.f_bsize;
      blocks = buf.f_blocks;
      availblks = buf.f_bavail;

      disk_size = blocks*blksize;
      free = availblks*blksize;

      systemInfo.TotalDiskSize = (disk_size)/(1024*1024); // in MB
      systemInfo.TotalFreeSize = (free)/(1024*1024); // in MB
      systemInfo.TotalUsedSize = (disk_size - free)/(1024*1024); // in MB

      //vilslog::printf("\n****blksize[%lu]****\n", blksize);
      //vilslog::printf("\n****blocks[%lu]****\n", blocks);
      //vilslog::printf("\n****freeblks[%lu]****\n", freeblks);
      //vilslog::printf("\n****availblks[%lu]****\n", availblks);
      //vilslog::printf("\n****disk_size[%lu]GB****\n", disk_size/(1024*1024*1024));
      //vilslog::printf("\n****used[%lu]GB****\n", used/(1024*1024*1024));
      //vilslog::printf("\n****free[%lu]GB****\n", free/(1024*1024*1024));

      return free;
   }
   else
   {
      return -1;
   }
}

int getSystemRunTime(SystemInfoStruct & systemInfo, int systemStartTime)
{
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);

    // long sec = usage.ru_stime.tv_sec;
    // sec += usage.ru_stime.tv_usec/1000000;
    long sec = usage.ru_utime.tv_sec;
    sec += usage.ru_utime.tv_usec/1000000;

    int hour = (int)(sec / 3600);
    int min = (int)(sec / 60);
    int hr_min = hour * 60;
    int hr_sec = hour * 3600;
    min -= hr_min;
    int min_sec = min * 60;
    sec -= (hr_sec + min_sec);


    //int hour = sec/(60*60);
    // 1024 * 1024 = 1048576
    //long long ram = CurrentRAMUsed();
    //vilslog::printf("WebUDPParserService::sendSystemStatusToClient() ram[%lu KBytes]\n", ram/(1024));

    time_t cutTime = getCurrentTimeInSec();
    int sys_sec = (int)(cutTime - systemStartTime);

    int sys_hour = (int)(sys_sec / 3600);
    int sys_min = (int)(sys_sec / 60);
    int sys_hr_min = sys_hour * 60;
    int sys_hr_sec = sys_hour * 3600;
    sys_min -= sys_hr_min;
    int sys_min_sec = sys_min * 60;
    sys_sec -= (sys_hr_sec + sys_min_sec);

    systemInfo.SystemRunHour = sys_hour;
    systemInfo.SystemRunMin = sys_min;
    systemInfo.SystemRunSec = sys_sec;

    systemInfo.CPURunHour = hour;
    systemInfo.CPURunMin = min;
    systemInfo.CPURunSec = sec;

#if defined(__APPLE__)
    size_t bytesInUse = 0;
    size_t blocksInUse = 0;
    size_t sizeAllocated = 0;

    heapInUse(&bytesInUse, &blocksInUse, &sizeAllocated);
    //vilslog::printf("in use: %zu KBytes, allocated: %zu, blocks: %zu\n", bytesInUse/(1024), sizeAllocated, blocksInUse);
    //heapStats();
    systemInfo.MemoryUsage = (int)(bytesInUse/(1024*1024)); //  MB

    // vilslog::printf("getSystemRunTime() __APPLE__  RunTime[%02d:%02d:%02d]\n", systemInfo.SystemRunHour, systemInfo.SystemRunMin, systemInfo.SystemRunSec);
    // vilslog::printf("getSystemRunTime() usage.ru_utime.tv_sec[%0ld] usage.ru_utime.tv_usec[%0ld] [%f]\n", usage.ru_utime.tv_sec, usage.ru_utime.tv_usec, (float)usage.ru_utime.tv_usec/1000000.0);
    // vilslog::printf("getSystemRunTime() usage.ru_stime.tv_sec[%0ld] usage.ru_stime.tv_usec[%0ld] [%f]\n\n\n\n", usage.ru_stime.tv_sec, usage.ru_stime.tv_usec, (float)usage.ru_stime.tv_usec/1000000.0);


#else
    double vm, rss;
    process_mem_usage(vm, rss);

    double dmemInMB = vm/(1024.0*1024.0);
    int imemInMB = (int)dmemInMB;
    if (dmemInMB > 0.0 && dmemInMB < 1.0)
    {
        imemInMB = 1;
    }

    // vilslog::printf("getSystemRunTime() vm[%f] rss[%f] imemInMB[%d]\n", vm, rss, imemInMB);

    systemInfo.MemoryUsage = imemInMB; //  MB
#endif

    return 0;
}

std::list<AreaInOutEventStruct*> * merge_inout_event(std::list<AreaInOutEventStruct*> * nodeAreaInOutList)
{
    std::list<AreaInOutEventStruct*> * merge_events = new std::list<AreaInOutEventStruct*>();

    AreaInOutEventStruct * pre_event = NULL;
    for (std::list<AreaInOutEventStruct*>::iterator event = nodeAreaInOutList->begin(); event != nodeAreaInOutList->end(); event++)
    {
        if ( pre_event != NULL )
        {
            // check if same area and same nodeid
            if ((pre_event->action == (*event)->action) &&
                (pre_event->AreaID.compare((*event)->AreaID) == 0) &&
                (pre_event->NodeId.compare((*event)->NodeId) == 0))
            {
                // skip this event
            }
            else
            {
                merge_events->push_back(copyAreaInOutEventStruct(pre_event));
                pre_event = (*event);
            }
        }
        else
        {
            pre_event = (*event);
        }
    }

    if ( pre_event != NULL )
    {
        merge_events->push_back(copyAreaInOutEventStruct(pre_event));
    }

    return merge_events;
}

std::list<Pair_AreaInOutEventStruct*> * pair_inout_event(std::list<AreaInOutEventStruct*> * nodeAreaInOutList)
{
    std::list<Pair_AreaInOutEventStruct*> * pair_events = new std::list<Pair_AreaInOutEventStruct*>();

    for (std::list<AreaInOutEventStruct*>::iterator event = nodeAreaInOutList->begin(); event != nodeAreaInOutList->end(); event++)
    {
        // vilslog::printf ("pair_inout_event() NodeId[%s] AreaID[%s] AreaName[%s] action[%d] DataTime[%s]\n",
        //     (*event)->NodeId.c_str(), (*event)->AreaID.c_str(), (*event)->AreaName.c_str(), (*event)->action, (*event)->DataTime.c_str());
        //if ( (*event)->action == 2 ) // "out" action
        if ( (*event)->action == 1 ) // "in" action
        {
            // find "out" action
            bool bFound = false;
            for (std::list<AreaInOutEventStruct *>::iterator check_event = std::next(event, 1); check_event != nodeAreaInOutList->end();)
            {
                if ( (*event)->AreaID.compare((*check_event)->AreaID) == 0)
                {
                    //if ( (*check_event)->action == 1)
                    if ( (*check_event)->action == 2)
                    {
                        // vilslog::printf("pair_inout_event() NodeId[%s] AreaID[%s] output pair\n", (*event)->NodeId.c_str(), (*event)->AreaID.c_str());

                        Pair_AreaInOutEventStruct * pairEvent = new Pair_AreaInOutEventStruct();
                        pairEvent->type = "pair";
                        pairEvent->event1 = copyAreaInOutEventStruct((*event));
                        pairEvent->event2 = copyAreaInOutEventStruct((*check_event));
                        pair_events->push_back(pairEvent);

                        // remove from list
                        AreaInOutEventStruct * old_event = (*check_event);
                        check_event = nodeAreaInOutList->erase(check_event);
                        delete old_event;

                        bFound = true;
                        break;
                    }
                    else
                    {
                        // not found
                        break;
                    }
                }
                else
                {
                    check_event++;
                }
            }

            if (!bFound)
            {
                // vilslog::printf("pair_inout_event() NodeId[%s] AreaID[%s] output single\n", (*event)->NodeId.c_str(), (*event)->AreaID.c_str());
                Pair_AreaInOutEventStruct * singleEvent = new Pair_AreaInOutEventStruct();
                singleEvent->type = "single";
                singleEvent->event1 = copyAreaInOutEventStruct((*event));
                singleEvent->event2 = NULL;
                pair_events->push_back(singleEvent);
            }
        }
        else
        {
            // vilslog::printf("pair_inout_event() NodeId[%s] AreaID[%s] output single\n", (*event)->NodeId.c_str(), (*event)->AreaID.c_str());
            Pair_AreaInOutEventStruct * singleEvent = new Pair_AreaInOutEventStruct();
            singleEvent->type = "single";
            singleEvent->event1 = copyAreaInOutEventStruct((*event));
            singleEvent->event2 = NULL;
            pair_events->push_back(singleEvent);
            continue;
        }
    }

    return pair_events;
}

std::string getAreaTypeName(int areatype)
{
    std::string areaname = "";
    switch(areatype)
    {
        default:
        case 0:
            areaname = "普通";
        break;
        case 1:
            areaname = "電子圍欄";
        break;
        case 2:
            areaname = "電子點名";
        break;
        case 3:
            areaname = "房間";
        break;
        case 4:
            areaname = "教室";
        break;
        case 5:
            areaname = "病房";
        break;
        case 6:
            areaname = "病床";
        break;
        case 999:
            areaname = "虛擬牆";
        break;
    }
    return areaname;
}

PDFTableHeaderDataStruct * create_pdf_headerdata(std::string header, int showwidth)
{
    PDFTableHeaderDataStruct * headerData = new PDFTableHeaderDataStruct();
    headerData->header = header;
    headerData->ShowWidth = showwidth;
    return headerData;
}

PDFTableColDataStruct * create_pdf_columndata(std::string header, std::string data)
{
    PDFTableColDataStruct * colData = new PDFTableColDataStruct();
    colData->header = header;
    colData->data = data;
    return colData;
}

std::string getEventActionName(std::string action)
{
    std::string ActionName = action;
    if (action.compare("Enter") == 0 || action.compare("enter") == 0)
    {
        ActionName = "進入";
    }
    else
    if (action.compare("Leave") == 0 || action.compare("leave") == 0)
    {
        ActionName = "離開";
    }
    else
    if (action.compare("Stay") == 0)
    {
        ActionName = "停留";
    }
    else
    if (action.compare("Alert") == 0)
    {
        ActionName = "按下警急按鈕";
    }
    else
    if (action.compare("Cancel") == 0 || action.compare("cancel") == 0)
    {
        ActionName = "按下取消按鈕";
    }
    else
    if (action.compare("Finish") == 0 || action.compare("finish") == 0)
    {
        ActionName = "完成";
    }
    else
    if (action.compare("Exceed") == 0 || action.compare("exceed") == 0)
    {
        ActionName = "超過巡檢時間";
    }
    else
    if (action.compare("shorttime") == 0)
    {
        ActionName = "巡檢時間過短";
    }
    else
    if (action.compare("longtime") == 0)
    {
        ActionName = "巡檢時間過長";
    }

    return ActionName;
}

std::string getEventIssueTimeName(std::string issuetime)
{
    std::string IssueTimeName = issuetime;
    if (issuetime.compare("0") == 0)
    {
        IssueTimeName = "立刻";
    }
    else
    if (issuetime.compare("1") == 0)
    {
        IssueTimeName = "1分鐘";
    }
    else
    if (issuetime.compare("5") == 0)
    {
        IssueTimeName = "5分鐘";
    }
    else
    if (issuetime.compare("10") == 0)
    {
        IssueTimeName = "10分鐘";
    }

    return IssueTimeName;
}

std::string convert_usergroup_id_to_name(std::list<UserGroupStruct*> * userGroupList, int groupid)
{
    std::string groupname = "";
    for (std::list<UserGroupStruct*>::iterator userGroup = userGroupList->begin(); userGroup != userGroupList->end(); userGroup++)
    {
        if ( (*userGroup)->groupid == groupid )
        {
            groupname = (*userGroup)->groupname;
            break;
        }
    }
    return groupname;
}

std::string convert_subgroup_id_to_name(std::list<UserSubGroupStruct*> * userSubGroupList, std::list<std::string> * subgroupidList)
{
    std::ostringstream sendstream;
    int count = 0;
    for (std::list<std::string>::iterator subgroupidIt = subgroupidList->begin(); subgroupidIt != subgroupidList->end(); subgroupidIt++)
    {
        //vilslog::printf("common::convert_subgroup_id_to_name() (*subgroupidIt)[%s]\n", (*subgroupidIt).c_str());

        std::string subgroupidstr = (*subgroupidIt);
        if (subgroupidstr.length() == 0)
        {
            continue;
        }
        int subgroupid = convert_string_to_int(subgroupidstr);

        if (subgroupid == 99999)
        {
            if ( count > 0 )
                sendstream<<",";
            sendstream<<"全部";
            count++;
            continue;
        }

        for (std::list<UserSubGroupStruct*>::iterator userSubGroup = userSubGroupList->begin(); userSubGroup != userSubGroupList->end(); userSubGroup++)
        {
            if ( (*userSubGroup)->subgroupid == subgroupid )
            {
                if ( count > 0 )
                    sendstream<<",";
                sendstream<<(*userSubGroup)->subgroupname;
                count++;
            }
        }
    }
    return sendstream.str();
}

std::string convert_maingroup_id_to_name(std::list<UserMainGroupStruct*> * userMainGroupList, std::list<std::string> * maingroupidList)
{
    std::ostringstream sendstream;
    int count = 0;
    for (std::list<std::string>::iterator maingroupidIt = maingroupidList->begin(); maingroupidIt != maingroupidList->end(); maingroupidIt++)
    {
        std::string maingroupidstr = (*maingroupidIt);
        if (maingroupidstr.length() == 0)
        {
            continue;
        }
        int maingroupid = convert_string_to_int(maingroupidstr);

        if (maingroupid == 99999)
        {
            if ( count > 0 )
                sendstream<<",";
            sendstream<<"全部";
            count++;
            continue;
        }

        for (std::list<UserMainGroupStruct*>::iterator userMainGroup = userMainGroupList->begin(); userMainGroup != userMainGroupList->end(); userMainGroup++)
        {
            if ( (*userMainGroup)->id == maingroupid )
            {
                if ( count > 0 )
                    sendstream<<",";
                sendstream<<(*userMainGroup)->name;
                count++;
            }
        }
    }
    return sendstream.str();
}

bool validateID(const std::string& s)
{
    for (const char c : s)
    {
        //if (!isalpha(c) && !isdigit(c) && !isspace(c))
        if (!isalpha(c) && !isdigit(c))
            return false;
    }

    return true;
}

std::string getUserName(int userid, std::list<UserStruct*> * plist)
{
    for (std::list<UserStruct*>::iterator user = plist->begin(); user != plist->end(); user++)
    {
        if ( (*user)->userid == userid )
        {
            return (*user)->name;
        }
    }
    return "";
}

std::list<std::string> * copyStringList(std::list<std::string> * pList)
{
    std::list<std::string> * retList = new std::list<std::string>();

    if (pList != NULL)
    {
        for (std::list<std::string>::iterator value = pList->begin(); value != pList->end(); value++)
        {
            retList->push_back((*value));
        }
    }

    return retList;
}

int fetchArrayValue(Json::Value result, PDFTableDataStruct * pdfDataWeight, const char * datalabel, const char * dataname)
{
    int datacount = 0;
    if ( result[datalabel].isArray() )
    {
        for(int i=0;i<(int)result[datalabel].size();i++)
        {
            Json::Value weightData = result[datalabel][i];

            PDFTableRowDataStruct * newRowData = new PDFTableRowDataStruct();

            std::string datavalue;
            std::string datetime;
            if ( weightData.isMember("data") )
            {
                datavalue = weightData["data"].asString();
            }
            if ( weightData.isMember("datetime") )
            {
                datetime = weightData["datetime"].asString();
            }

            newRowData->coldatalist.push_back(create_pdf_columndata(dataname, datavalue));
            newRowData->coldatalist.push_back(create_pdf_columndata("日期", datetime));
            pdfDataWeight->rowdatalist.push_front(newRowData);

            datacount++;
        }
    }
    return datacount;
}

bool checkCardExisted(std::string cardid, std::list<std::string> * cardlist)
{
    bool bExisted = false;

    for (std::list<std::string>::iterator card = cardlist->begin(); card != cardlist->end(); card++)
    {
        if ( cardid.compare((*card)) == 0 )
        {
            bExisted = true;
            break;
        }
    }

    return bExisted;
}

void copyFireFightingStruct(FireFightingStruct * srcInfo, FireFightingStruct * dstInfo)
{
    dstInfo->id = srcInfo->id;
    dstInfo->maingroupid = srcInfo->maingroupid;
    dstInfo->caseclosed = srcInfo->caseclosed;
    dstInfo->casenumber = srcInfo->casenumber;
    dstInfo->address = srcInfo->address;
    dstInfo->lat = srcInfo->lat;
    dstInfo->lng = srcInfo->lng;
    dstInfo->starttime = srcInfo->starttime;
    dstInfo->closetime = srcInfo->closetime;
    dstInfo->datetime = srcInfo->datetime;

    for (std::list<std::string>::iterator deviceid = srcInfo->StandbyDeviceIds->begin(); deviceid != srcInfo->StandbyDeviceIds->end(); deviceid++)
        dstInfo->StandbyDeviceIds->push_back(*deviceid);
    for (std::list<int>::iterator deviceid = srcInfo->NFCDeviceIdList->begin(); deviceid != srcInfo->NFCDeviceIdList->end(); deviceid++)
        dstInfo->NFCDeviceIdList->push_back(*deviceid);
}

void copyFireFightingUserStruct(FireFightingUserStruct * srcInfo, FireFightingUserStruct * dstInfo)
{
    dstInfo->userid = srcInfo->userid;
    dstInfo->maingroupid = srcInfo->maingroupid;
    dstInfo->areaType = srcInfo->areaType;
    dstInfo->airremain = srcInfo->airremain;
    dstInfo->enterTime = srcInfo->enterTime;
    dstInfo->casenumber = srcInfo->casenumber;
    dstInfo->activity = srcInfo->activity;
    dstInfo->HR = srcInfo->HR;
    dstInfo->SPO2 = srcInfo->SPO2;
    dstInfo->battery = srcInfo->battery;
}

void copyFireFightingAirUsingStruct(FireFightingAirUsingStruct * srcInfo, FireFightingAirUsingStruct * dstInfo)
{
    dstInfo->userid = srcInfo->userid;
    dstInfo->casenumber = srcInfo->casenumber;
    dstInfo->startairremain = srcInfo->startairremain;
    dstInfo->starttime = srcInfo->starttime;
    dstInfo->endairremain = srcInfo->endairremain;
    dstInfo->endtime = srcInfo->endtime;
    dstInfo->airusing = srcInfo->airusing;
    dstInfo->totaltime = srcInfo->totaltime;
}

int rssi_to_distance(int rssi)
{
    int txPower = -59;
    /*
     * RSSI = TxPower - 10 * n * lg(d)
     * n = 2 (in free space)
     *
     * d = 10 ^ ((TxPower - RSSI) / (10 * n))
     */

    return pow(10, ((double) txPower - rssi) / (10 * 2)) * 100.0f;
}
