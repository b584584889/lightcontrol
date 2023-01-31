/*
 * AnchorRangingDataClass.h
 *
 *  Created on: 2017/03/09
 *      Author: ikki
 */


#ifndef AnchorRangingDataClass_H
#define AnchorRangingDataClass_H

#include "../config.h"
#include "../Filters/kalman/Kalman1D.h"
#include "../Filters/SmoothingFilter/SmoothingFilter.h"

#define StandardDeviationCOUNT 20

typedef struct
{
    int AnchorTagRangingRaw;// Anchor with TagName Ranging result
    int AnchorTagRangingFiltered;// Anchor with TagName Ranging result Filtered
    int AnchorTagRangingIndex;// Anchor with TagName Ranging index
    // int PollCnt;
    // int recvdBm; // receive signal power
    // int firstpathdBm; // signal power in the first path
	int AnchorTagIMU[6];
    int TagPollCnt;      // Tag ranging poll count
    int RangingAnchorCnt;     // ranging Anchor count
    int TagFinalRecvdBm; // Tag ->  Anchor receive signal power
    int TagFirstPathdBm; // Tag ->  Anchor signal power in the first path
    int AnchorRecvdBm;   // Anchor -> Coord receive signal power
    int AnchorFirstPathdBm; //  Anchor -> Coord signal power in the first path
	double TagVelocity;
    float TagAngle;
    int TagLevel_x; // AOA Level
    int TagLevel_y; // AOA Level
    int TagLevel_time; // AOA Level
    time_t mDataTime;
    time_t mDataTimeInMS; // millisecoond
    time_t mDataTimeStamp; // From LoRa gateway
    time_t mAverageDiffTime; // LoRa gateway 傳送過來的 timestamp 可能與Server有時間差, 取平均再扣回去.
} RangingDataStruct;

class AnchorRangingDataClass
{
public:
	AnchorRangingDataClass();
	~AnchorRangingDataClass();

    char AnchorName[32];
    //int AnchorFloor;
    int MapLayer;

    int init();
    //void putAnchorStatus(int totalCount, int activeCount);
    void putRanging(int rangingidx, int ranging, int * imu);
    void putRanging(int rangingidx, int ranging, int velocity);
    void putRanging(int rangingidx, int ranging, int velocity, int rssi, const char * timestamp);
    void putRangingAOA(int rangingidx, int ranging, int velocity, float angle, int rssi, const char * timestamp);
    void putFenceAOA(int rangingidx, int level_x, int level_y, int level_time, int rssi, const char * timestamp);
    void putRangingDiag(int rangingidx, int ranging, int * imu,
                        int PollCnt, int RangingAnchorCnt, int TagFinalRecvdBm, int TagFirstPathdBm,
                        int AnchorRecvdBm, int AnchorFirstPathdBm, int RangingCount, int RangingFailCount, float DataRate);

    //int getRanging();
    int getRangingRaw();
    int getRangingFiltered();
    // int getFinalCnt();
    // int getPollCnt();
    // int getrecvdBm();
    // int getfirstpathdBm();
    // rangingdiag
    int getTagPollCnt();      // Tag ranging poll count
    int getRangingAnchorCnt();     // ranging Anchor count
    int getTagFinalRecvdBm(); // Tag ->  Anchor receive signal power
    int getTagFirstPathdBm(); // Tag ->  Anchor signal power in the first path
    int getAnchorRecvdBm();   // Anchor -> Coord receive signal power
    int getAnchorFirstPathdBm(); //  Anchor -> Coord signal power in the first path
    time_t getDataTimeInMS();

    int getLostRate();
    float getDataRate();
    float getValidRate();
    float getStandardDeviation();
    int getDataCount();

    // int getRangingRawFromRangingIndex(int index);
    // int getRangingFilteredFromRangingIndex(int index);
    // double getVelocityFromRangingIndex(int index);
    // int getTagPollCntFromRangingIndex(int index);      // Tag ranging poll count
    // int getRangingAnchorCntFromRangingIndex(int index);     // ranging Anchor count
    // int getTagFinalRecvdBmFromRangingIndex(int index); // Tag ->  Anchor receive signal power
    // int getTagFirstPathdBmFromRangingIndex(int index); // Tag ->  Anchor signal power in the first path
    // int getAnchorRecvdBmFromRangingIndex(int index);   // Anchor -> Coord receive signal power
    // int getAnchorFirstPathdBmFromRangingIndex(int index); //  Anchor -> Coord signal power in the first path
    //int getFinalCntFromRangingIndex(int index);
    //int getPollCntFromRangingIndex(int index);
    //int getrecvdBmFromRangingIndex(int index);
    //int getfirstpathdBmFromRangingIndex(int index);
    // int getRangingDataIndexFromRangingIndex(int index);
    // int getDataTimeInMSFromRangingIndex(int index);

    time_t getDataTime();
    time_t getDataTimeStamp();
    bool checkValidRanging(int validdatatime);
    bool checkValidRanging(time_t latest_time, int validdatatime);
    int ValidRangingCount(int validdatatime);
    int getAverageDiffRanging(int validdatatime);

    void getIMU(double * IMU, int valuecount);
    void getIMUi(int * IMU, int valuecount);
    double getVelocity();
    float getAngle();
    int getLevel_x();
    int getLevel_y();
    int getLevel_time();

    char * getAnchorName();
    int update();

    int dump_ranging_data();

    int setArguments(int EnableNoiseMakup,
        float NoiseMakupRatio,
        float NoiseMakupWeight,
        int EnableKalman1D);
private:

//#ifdef APPLY_KALMAN_1D
    int AnchorTagRanging;
    Kalman1D * kalman1d;
    double kmresult[1];
    double last_results[1];

//#endif
    //time_t mLastDataTime;
    int mDataCount;
    int mCurDataIndex;
    int mPreDataIndex;

    //int mRangingFailureCount;
    //int mTotalAnchorCount;
    //int mActiveAnchorCount;

    //int mTotalLostRateCount;
    //int mCalLostRateCount;
    float mCurLostRate;
    float mPreLostRate;
    int mRangingCount;
    int mRangingFailCount;
    float mDataRate;

    float mValidRate;

    double mDataRate_results[3];
    SmoothingFilter * mDataRateSmoothingFilter;

    float mStandardDeviation;
    int mAnchorTagRanging[StandardDeviationCOUNT];
    //RangingDataStruct RangingData[RANGING_BUFFER_SIZE];
    RangingDataStruct * RangingData;

    float mAverageDiffRanging;

    int mEnableNoiseMakup;
    float mNoiseMakupRatio;
    float mNoiseMakupWeight;
    int mEnableKalman1D;

    double mPreTagVelocity;
    time_t mVelocityEqualZeroTimeStamp;

    time_t mAoADataExtraDataTimeInMS; // millisecoond

    int initRangingData();

    double updateVelocity(double Velocity);
    float calculateLostRate();
    float calculateStandardDeviation(int ranging);

};


#endif
