
#ifndef AnchorTagCoupleDataClass_H
#define AnchorTagCoupleDataClass_H

#include "../Filters/Filter.h"
#include "../Math/leastsquares/NonLinearSolver.h"
#include "../Math/leastsquares/ResidualsFunction.h"

#include "AnchorRangingDataClass.h"
#include "../config.h"

typedef struct
{
    int validCount;
    //int AnchorFloor[MAX_RANGING_RESULT];
    int MapLayer[MAX_RANGING_RESULT];
    char AnchorName[MAX_RANGING_RESULT][32];
    int RangingRaw[MAX_RANGING_RESULT];// Anchor with TagName Ranging result
    int RangingFiltered[MAX_RANGING_RESULT];// Anchor with TagName Ranging result
    double TagVelocity[MAX_RANGING_RESULT];
    float TagAngle[MAX_RANGING_RESULT];
    int TagLevel_x[MAX_RANGING_RESULT];
    int TagLevel_y[MAX_RANGING_RESULT];
    int TagLevel_time[MAX_RANGING_RESULT];
    // int PollCnt[MAX_RANGING_RESULT];
    // int FinalCnt[MAX_RANGING_RESULT];
    // int recvdBm[MAX_RANGING_RESULT]; // receive signal power
    // int firstpathdBm[MAX_RANGING_RESULT]; // signal power in the first path
    int TagPollCnt[MAX_RANGING_RESULT];      // Tag ranging poll count
    int RangingAnchorCnt[MAX_RANGING_RESULT];     // ranging Anchor count
    int TagFinalRecvdBm[MAX_RANGING_RESULT]; // Tag ->  Anchor receive signal power
    int TagFirstPathdBm[MAX_RANGING_RESULT]; // Tag ->  Anchor signal power in the first path
    int AnchorRecvdBm[MAX_RANGING_RESULT];   // Anchor -> Coord receive signal power
    int AnchorFirstPathdBm[MAX_RANGING_RESULT]; //  Anchor -> Coord signal power in the first path
    int LostRate[MAX_RANGING_RESULT];
    float DataRate[MAX_RANGING_RESULT];
    int DataCount[MAX_RANGING_RESULT];
    float StandardDeviation[MAX_RANGING_RESULT];
    int AnchorTagIMU[MAX_RANGING_RESULT][6];
    time_t DataTimeInMS[MAX_RANGING_RESULT];
    time_t DataTimeStamp[MAX_RANGING_RESULT];
    float ValidRate[MAX_RANGING_RESULT]; // valid data count in valid time
} RangingResultStruct;

class AnchorTagCoupleDataClass
{
public:
	AnchorTagCoupleDataClass();
	~AnchorTagCoupleDataClass();
    void init(double posX, double posY, double posZ);
    //void putAnchorStatus(char * anchorname, int totalCount, int activeCount);
	void putRanging(const char * anchorname, int MapLayer, int rangingidx, int ranging, int * imu);
    void putRanging(const char * anchorname, int MapLayer, int rangingidx, int ranging, int velocity);
    void putRanging(const char * anchorname, int MapLayer, int rangingidx, int ranging, int velocity, int rssi, const char * timestamp);
    void putRangingAOA(const char * anchorname, int MapLayer, int rangingidx, int ranging, int velocity, float angle, int rssi, const char * timestamp);
    void putFenceAOA(const char * anchorname, int MapLayer, int rangingidx, int level_x, int level_y, int level_time, int rssi, const char * timestamp);
    void putRangingDiag(const char * anchorname, int MapLayer, int rangingidx, int ranging, int * imu,
                        int PollCnt, int RangingAnchorCnt, int TagFinalRecvdBm, int TagFirstPathdBm,
                        int AnchorRecvdBm, int AnchorFirstPathdBm, int RangingCount, int RangingFailCount, float DataRate);
    int prepareRangingData(RangingResultStruct * validRangingData, int validdatatime);
    int prepareAllRangingData(RangingResultStruct * validRangingData, int validdatatime);
    //int getIndex();
	void setTagName(char * tagname);
	char * getTagName();
	void setResult(double * result, int valuecount);
    void setRawResult(double * rawresult, int valuecount);
    //void setIndex(int index);
	void getResult(double * result, int valuecount);
    void getRawResult(double * rawresult, int valuecount);
    void getIMU(double * IMU, int valuecount);
    int getRangingCount();
    int resetRangingCount();
    double getVelocity();
    double getVelocitykm();
    int update(int validdatatime);
    int updateVelocity(int validdatatime);
    RangingResultStruct * getLastRangingData();

    int setArguments(int EnableNoiseMakup,
        float NoiseMakupRatio,
        float NoiseMakupWeight,
        int EnableKalman1D,
        int EnableRangingReliability,
        int RangingReliabilityMaxDistance,
        int RangingReliabilityMinCount,
        int RangingReliabilityAverageDiff,
        int CalculationMethod,
        float WeightMovingAverageValue1,
        float WeightMovingAverageValue2,
        float WeightMovingAverageValue3);

private:
    //int mIndex;
    char TagName[32];//
	double last_results[3]; // 3D location
    double raw_results[3]; // 3D location

    int mLatestRangingIndex;

    int mNotValidRangingCount;
    int mRangingCount;

    RangingResultStruct * mLastRangingResult;

    double mTagVelocity;
    double kmVelocity[1];
    double lastVelocity[1];
    Kalman1D * kalman1dVelocity;
    time_t mVelocityEqualZeroTimeStamp;

    Filter * mCurrentFilter;
    Filter * mKalman3DFilter;
    Filter * mSmoothingFilter;
    Filter * mWeightedMovingAverageFilter;

    int mCalculationMethod;
    int mEnableRangingReliability;
    int mRangingReliabilityMaxDistance;
    int mRangingReliabilityMinCount;
    int mRangingReliabilityAverageDiff;

    //AnchorRangingDataClass * mAnchorData;
    std::vector<AnchorRangingDataClass *> mAnchorData;

    int getLatestAnchorIndex();
    int validRangingIndex(time_t latest_time, int min_count, int validdatatime);
};


#endif
