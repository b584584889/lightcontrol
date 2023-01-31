/*
 *  SensorInfoService.h
 *
 *  Created on: 2020/12/30
 *      Author: ikki
 */
#ifndef __SensorInfoService_h__
#define __SensorInfoService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../common.h"
#include "../config.h"
#include "../network/DataItem.h"
#include "../network/WorkItem.h"
#include "../network/NotificationItem.h"
#include "../mysql/DBManager.h"
#include "../DeviceManager/DeviceManager.h"
// #include "../Filters/kalman/Kalman1D.h"
#include "../Filters/SmoothingFilter/SmoothingFilter.h"

using namespace std;

#define MAX_VALUE_SIZE 20
#define EVENT_TRIGGER_COUNT 4
#define EVENT_CHANGE_TIME 300
#define RESET_SENSOR_COUNT 5

#define THRESHOD_SD_ENTER 10
#define THRESHOD_AVG_ENTER 500

#define THRESHOD_SD 100
#define THRESHOD_AVG 1000
#define THRESHOD_DISTANCE 30

typedef struct
{
    std::string name;
    int index;
    int size;
    float average;
    float StandardDeviation;
    int maxDistance;
    int minDistance;
    int value[MAX_VALUE_SIZE];
    // Kalman1D * kalman1d;
    SmoothingFilter * smoothingFilter;
    time_t updateTime;
    time_t processTime;
} LiDARSensorInfoStruct;

typedef struct
{
    std::string sensorname;
    // std::string day;
    std::string eventname;
    std::string gate;
    int type; // 0: enter 1: exit
    int send; // 0: not send , 1: already send  Notification
    int triggerCount;
    time_t triggerTime;
    time_t changeTime;
    time_t startTime;
    std::string pre_airLineCode;
    std::string pre_airLineNum;
} SensorEventStruct;

typedef struct
{
    std::string FDATE;
    std::string STD;
    std::string amhsETD;
    std::string ritETD;
    std::string ATDDate;
    std::string ATD;
    std::string airLineCode;
    std::string airLineNum;
    std::string ArrivalAirport;
    std::string airPlaneType;
    std::string REG_NO;
    std::string APPLY_TYPE;
    int send; // 0: not send , 1: already send  Notification
    std::string airBoardingGate; // FlyScheduleDepartureStruct.airBoardingGate
} FlyInfoDepartureStruct;

typedef struct
{
    std::string FDATE;
    std::string STA;
    std::string amhsETA;
    std::string ritETA;
    std::string ATADate;
    std::string ATA;
    std::string airLineCode;
    std::string airLineNum;
    std::string DepartureAirport;
    std::string airPlaneType;
    std::string REG_NO;
    std::string APPLY_TYPE;
    int send; // 0: not send , 1: already send  Notification
    std::string airBoardingGate; // FlyScheduleArrivalStruct.airBoardingGate
} FlyInfoArrivalStruct;

typedef struct
{
    std::string FDATE;
    std::string expectTime;
    std::string realTime;
    std::string airLineName;
    std::string airLineCode;
    std::string airLineLogo;
    std::string airLineUrl;
    std::string airLineNum;
    std::string goalAirportCode;
    std::string goalAirportName;
    std::string airPlaneType; // ATR72
    std::string airBoardingGate; // 16
    std::string airFlyStatus; // "離站Departed", "取消Cancelled", "登機Boarding",
    std::string airFlyDelayCause;
} FlyScheduleDepartureStruct;

typedef struct
{
    std::string FDATE;
    std::string expectTime;
    std::string realTime;
    std::string airLineName;
    std::string airLineCode;
    std::string airLineLogo;
    std::string airLineUrl;
    std::string airLineNum;
    std::string upAirportCode;
    std::string upAirportName;
    std::string airPlaneType;
    std::string airBoardingGate;
    std::string airFlyStatus; // "準時On Time" "抵達Arrived" "延遲Delayed" "取消Cancelled"
    std::string airFlyDelayCause;
} FlyScheduleArrivalStruct;

class SensorInfoService : public Thread
{
public:
    SensorInfoService(wqueue<WorkItem*> & SensorInfoJobQueue, wqueue<WorkItem*> & MySQLJobQueue, wqueue<DataItem*> & MainJobQueue, DeviceManager * pDevManager, DBManager * dbManager);
    ~SensorInfoService();
    bool status();
    std::string statusString();

    void * run();

private:
    bool mInitial;
    std::string mStatus;

    wqueue<WorkItem*> & mSensorInfoJobQueue;
    wqueue<WorkItem*> & mMySQLJobQueue;
    wqueue<DataItem*> & mMainJobQueue;
    DeviceManager * mDeviceManager;
    DBManager * mDbManager;

    double mResult[3];
    double mInput[3];

    std::list<LiDARSensorInfoStruct*> * mLiDARInfoList;
    std::list<SensorEventStruct*> * mSensorEventList;
    std::list<FlyInfoDepartureStruct*> * mFlyInfoDepartureList;
    std::list<FlyInfoArrivalStruct*> * mFlyInfoArrivalList;
    std::list<FlyScheduleDepartureStruct*> * mFlyScheduleDepartureList;
    std::list<FlyScheduleArrivalStruct*> * mFlyScheduleArrivalList;
    std::list<FlyGateInfoStruct*> * mFlyGateInfoList;


    void runloop();
    void update_last_trigger_event();
    void updateGateInfo();

    int processDataItem(WorkItem * workItem);
    // void test_data();
    int put_to_LiDAR_list(std::string name, int distance, time_t timestamp);
    void calculate_LiDAR_value();
    int check_LiDAR_event(LiDARSensorInfoStruct * lidar, int curType);
    void check_Fly_event(LiDARSensorInfoStruct * lidar, int type, SensorEventStruct * event, time_t updateTime);

    FlyInfoDepartureStruct * getFlyInfoDeparture(std::string day, std::string curTime, std::string gate);
    std::list<FlyInfoDepartureStruct*> * getFlyInfoDeparturePossible(std::string day, std::string curTime);

    FlyInfoArrivalStruct * getFlyInfoArrival(std::string day, std::string curTime, std::string gate);
    std::list<FlyInfoArrivalStruct*> * getFlyInfoArrivalPossible(std::string day, std::string curTime);

    int checkFlyDepartured(std::string STD, std::string airLineCode, std::string airLineNum);
    int checkFlyArrivaled(std::string STA, std::string airLineCode, std::string airLineNum);

    SensorEventStruct * getEvent(std::string sensorname, std::string eventname);
    std::string getFlyBoardingGate(std::string airLineCode, std::string airLineNum);

    void updateFlyInfo();
    void updateFlyInfoArr(std::string content);
    void updateFlyInfoDep(std::string content);

    void updateFlySchedule();
    void updateFlyScheduleArr(std::string content);
    void updateFlyScheduleDep(std::string content);

    void putToFlyInfoDepartureList(FlyInfoDepartureStruct * newInfo);
    void putToFlyInfoArrivalList(FlyInfoArrivalStruct * newInfo);

    void putToFlyScheduleDepartureList(FlyScheduleDepartureStruct * newInfo);
    void putToFlyScheduleArrivalList(FlyScheduleArrivalStruct * newInfo);

    std::string convert_to_AirLineName(std::string airLineCode);
    std::string convert_to_fly_json_str(FlyInfoDepartureStruct * info, std::string triggerTime);
    std::string convert_to_fly_json_str(FlyInfoArrivalStruct * info, std::string triggerTime);

    void putFlyInfoToDB(FlyInfoDepartureStruct * newInfo, std::string triggerTime, std::string sensorid);
    void putFlyInfoToDB(FlyInfoArrivalStruct * newInfo, std::string triggerTime, std::string sensorid);

    void updateDepartureBoardingGate(FlyScheduleDepartureStruct * newInfo);
    void updateArrivalBoardingGate(FlyScheduleArrivalStruct * newInfo);
    void updateArrivalBoardingGate(std::string FDATE, std::string STD, std::string airLineCode, std::string airLineNum, std::string airBoardingGate);

    void release_old_info();
    void reset_sensor_device();

    float averageCalc(int * x, int len, bool skipOne);
    float varianceCalc(float average, int * x, int len, bool skipOne);
};

#endif
