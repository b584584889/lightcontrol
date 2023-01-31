/*
 * AnchorRangingDataClass.cpp
 *
 *  Created on: 2017/03/09
 *      Author: ikki
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <math.h>

#include <sys/time.h>
#include <sys/timeb.h>//timeb
#include <time.h>
#include "string.h"

#include "algorithm.h"
#include "../common.h"
#include "AnchorRangingDataClass.h"

//
// AnchorRangingDataClass
//
AnchorRangingDataClass::AnchorRangingDataClass()
{
    init();
}

AnchorRangingDataClass::~AnchorRangingDataClass()
{
//#ifdef APPLY_KALMAN_1D
    delete kalman1d;
//#endif
    delete [] RangingData;
    delete mDataRateSmoothingFilter;
}

int AnchorRangingDataClass::init()
{
    // AnchorDataStruct
    memset(AnchorName, 0, 32);
    //AnchorFloor = -1;
    MapLayer = 0;

    mDataCount = 0;
    mCurDataIndex = 0;
    mPreDataIndex = 0;
    initRangingData();
    //mLastDataTime = getCurrentTimeInSec();
    //mLastDataTime = getCurrentTimeInMilliSec();

    //mRangingFailureCount = 5;
    //mTotalAnchorCount = 1;
    //mActiveAnchorCount = 1;

    //mCalLostRateCount = 0;
    mCurLostRate = 0.0f;
    mPreLostRate = 0.0f;
    mStandardDeviation = 0.0f;

    mAverageDiffRanging = 0.0f;
    //char m_AnchorRangingTable[256];
    mRangingCount = 0;
    mRangingFailCount = 0;
    mDataRate = 0.0f;

    mValidRate = 0.0f;

    mEnableNoiseMakup = 1;
    mNoiseMakupRatio = 2.0f;
    mNoiseMakupWeight = 0.8f;
    mEnableKalman1D = 0;

    mPreTagVelocity = 0.0f;
    mVelocityEqualZeroTimeStamp = 0;

//#ifdef APPLY_KALMAN_1D
    AnchorTagRanging = 0;
    kalman1d = new Kalman1D();

    // update first data
    kmresult[0] = 0;
    last_results[0] = AnchorTagRanging;
    kalman1d->update(last_results, kmresult);

//#endif

    mDataRateSmoothingFilter = new SmoothingFilter();

    mDataRate_results[0] = 0;
    mDataRate_results[1] = 0;
    mDataRate_results[2] = 0;
    mDataRateSmoothingFilter->initial(mDataRate_results);
    mDataRateSmoothingFilter->update(mDataRate_results, mDataRate_results);

    for(int i=0; i<StandardDeviationCOUNT;i++)
    {
        mAnchorTagRanging[i] = 0;
    }

    mAoADataExtraDataTimeInMS = 0;

    return 0;
}

int AnchorRangingDataClass::initRangingData()
{
    RangingData = new RangingDataStruct[RANGING_BUFFER_SIZE];

    for (int i=0;i<RANGING_BUFFER_SIZE;i++)
    {
        RangingData[i].AnchorTagRangingRaw = 0;
        RangingData[i].AnchorTagRangingFiltered = 0;
        RangingData[i].AnchorTagRangingIndex = -1;
        for (int j=0;j<6;j++)
            RangingData[i].AnchorTagIMU[j] = 0;
        RangingData[i].TagPollCnt = 0;
        RangingData[i].RangingAnchorCnt = 0;
        RangingData[i].TagFinalRecvdBm = 0;
        RangingData[i].TagFirstPathdBm = 0;
        RangingData[i].AnchorRecvdBm = 0;
        RangingData[i].AnchorFirstPathdBm = 0;
        RangingData[i].TagVelocity = 0.0;
        RangingData[i].TagAngle = 0.0;
        RangingData[i].TagLevel_x = 0;
        RangingData[i].TagLevel_y = 0;
        RangingData[i].TagLevel_time = 0;
        RangingData[i].mDataTime = 0;
        RangingData[i].mDataTimeInMS = 0;
        RangingData[i].mDataTimeStamp = 0;
        RangingData[i].mAverageDiffTime = 0;
    }
    return 0;
}

// void AnchorRangingDataClass::putAnchorStatus(int totalCount, int activeCount)
// {
//     mTotalAnchorCount = totalCount;
//     mActiveAnchorCount = activeCount;
// }

void AnchorRangingDataClass::putRanging(int rangingidx, int ranging, int * imu)
{
    mDataCount++;

    if ( ranging <= 0 || ranging > 50000)
    {
        //printf ("AnchorRangingDataClass::putRanging() Ignore, ranging = %d\n", ranging);
        return;
    }
    //printf ("AnchorRangingDataClass::putRanging() rangingidx[%d] ranging[%d]\n", rangingidx, ranging);

    //struct timeval tv;
    double Velocity = 0.0;
    // first data in
    if (mPreDataIndex == mCurDataIndex)
    {
        Velocity = 0.0;
    }
    else
    {
    	//
    	// using IMU data
    	//
        Velocity = calcVelocity(RangingData[mPreDataIndex].AnchorTagIMU, imu);
    }
    //==========================
    //gettimeofday(&tv, NULL);
    RangingData[mCurDataIndex].AnchorTagRangingRaw = ranging;
    RangingData[mCurDataIndex].AnchorTagRangingFiltered = ranging;
    RangingData[mCurDataIndex].AnchorTagRangingIndex = rangingidx;
    //RangingData[mCurDataIndex].PollCnt = PollCnt;
    //RangingData[mCurDataIndex].recvdBm = recvdBm;
    //RangingData[mCurDataIndex].firstpathdBm = firstpathdBm;
    for(int j=0;j<6;j++)
        RangingData[mCurDataIndex].AnchorTagIMU[j] = imu[j];
    //RangingData[mCurDataIndex].mDataTime = tv.tv_sec;
    RangingData[mCurDataIndex].mDataTime = getCurrentTimeInSec();
    RangingData[mCurDataIndex].mDataTimeInMS = getCurrentTimeInMilliSec();
    RangingData[mCurDataIndex].mDataTimeStamp = getCurrentTimeInSec();
    //RangingData[mCurDataIndex].mDataTime = getCurrentTimeInMilliSec();

    //RangingData[mCurDataIndex].TagVelocity = Velocity;
    RangingData[mCurDataIndex].TagVelocity = updateVelocity(Velocity);

    mPreDataIndex = mCurDataIndex;
    mCurDataIndex++;
    if (mCurDataIndex >= RANGING_BUFFER_SIZE)
        mCurDataIndex = 0;
    //mLastDataTime = RangingData[mCurDataIndex].mDataTime;
    //==========================
    if ( mEnableKalman1D == 1 )
    {
        AnchorTagRanging = ranging;
        last_results[0] = AnchorTagRanging;
    }

    //
    // calculate Lost Rate
    //
    calculateLostRate();
    calculateStandardDeviation(ranging);

    //printf ("AnchorRangingDataClass::putRanging() AnchorName[%s] AnchorTagRanging[%d] mDataTime[%ld]\n", AnchorName, AnchorTagRanging, mDataTime);
}

void AnchorRangingDataClass::putRanging(int rangingidx, int ranging, int velocity)
{
    mDataCount++;

    if ( ranging <= 0 || ranging > 50000)
    {
        //printf ("AnchorRangingDataClass::putRanging() Ignore, ranging = %d\n", ranging);
        return;
    }

    //==========================
    RangingData[mCurDataIndex].AnchorTagRangingRaw = ranging;
    RangingData[mCurDataIndex].AnchorTagRangingFiltered = ranging;
    RangingData[mCurDataIndex].AnchorTagRangingIndex = rangingidx;
    for(int j=0;j<6;j++)
        RangingData[mCurDataIndex].AnchorTagIMU[j] = 0;
    RangingData[mCurDataIndex].mDataTime = getCurrentTimeInSec();
    RangingData[mCurDataIndex].mDataTimeInMS = getCurrentTimeInMilliSec();
    RangingData[mCurDataIndex].mDataTimeStamp = getCurrentTimeInSec();

    double Velocity = (double)velocity / 10.0f;
    //RangingData[mCurDataIndex].TagVelocity = Velocity;
    RangingData[mCurDataIndex].TagVelocity = updateVelocity(Velocity);

    mPreDataIndex = mCurDataIndex;
    mCurDataIndex++;
    if (mCurDataIndex >= RANGING_BUFFER_SIZE)
        mCurDataIndex = 0;
    //==========================
    if ( mEnableKalman1D == 1 )
    {
        AnchorTagRanging = ranging;
        last_results[0] = AnchorTagRanging;
    }

    //
    // calculate Lost Rate
    //
    calculateLostRate();
    calculateStandardDeviation(ranging);

    //printf ("AnchorRangingDataClass::putRanging() AnchorName[%s] AnchorTagRanging[%d] mDataTime[%ld]\n", AnchorName, AnchorTagRanging, mDataTime);
}

void AnchorRangingDataClass::putRanging(int rangingidx, int ranging, int velocity, int rssi, const char * timestamp)
{
    mDataCount++;

    if ( ranging <= 0 || ranging > 50000)
    {
        //printf ("AnchorRangingDataClass::putRanging() Ignore, ranging = %d\n", ranging);
        return;
    }

    //printf ("AnchorRangingDataClass::putRanging() AnchorName[%s] ranging[%d] timestamp[%s]\n", AnchorName, ranging, timestamp);

    //
    // 根據每次 LoRa gateway時間 與 local時間計算時間差, 求出平均秒差.
    //
    time_t oldTime = RangingData[mCurDataIndex].mAverageDiffTime;
    time_t curTimeMS = getCurrentTimeInMilliSec();
    time_t dataTime = StringTimeZoneDateTimeTotime_t(timestamp);
    if (dataTime == 0)
        dataTime = StringDateTimeTotime_t(timestamp);
    time_t diff_t = dataTime * 1000 - curTimeMS;
    time_t newDiffTime = (oldTime * 0.4) + (diff_t * 0.6);

    //time_t fixTimeMS = dataTime * 1000 - newDiffTime;
    //time_t fixTime = (fixTimeMS / 1000);

    // printf ("AnchorRangingDataClass::putRanging() AnchorName[%s] timestamp[%s] diff_t[%ld] newDiffTime[%ld]\n",
    //     AnchorName, time_tToString(dataTime).c_str(), diff_t, newDiffTime);
    // printf ("AnchorRangingDataClass::putRanging() curTime[%s] fixTime[%s] timestamp[%s]\n",
    //     time_tToString(curTimeMS/1000).c_str(), time_tToString(fixTime).c_str(), time_tToString(dataTime).c_str());

    //==========================
    RangingData[mCurDataIndex].AnchorTagRangingRaw = ranging;
    RangingData[mCurDataIndex].AnchorTagRangingFiltered = ranging;
    RangingData[mCurDataIndex].AnchorTagRangingIndex = rangingidx;
    for(int j=0;j<6;j++)
        RangingData[mCurDataIndex].AnchorTagIMU[j] = 0;
    RangingData[mCurDataIndex].TagFinalRecvdBm = rssi * -1;
    RangingData[mCurDataIndex].TagFirstPathdBm = 0;
    RangingData[mCurDataIndex].AnchorRecvdBm = 0;
    RangingData[mCurDataIndex].AnchorFirstPathdBm = 0;

    // RangingData[mCurDataIndex].mDataTime = StringTimeZoneDateTimeTotime_t(timestamp);
    // RangingData[mCurDataIndex].mDataTimeInMS = StringTimeZoneDateTimeTotime_t(timestamp) * 1000;
    RangingData[mCurDataIndex].mDataTime = getCurrentTimeInSec();
    RangingData[mCurDataIndex].mDataTimeInMS = getCurrentTimeInMilliSec();
    RangingData[mCurDataIndex].mDataTimeStamp = StringTimeZoneDateTimeTotime_t(timestamp);
    if (RangingData[mCurDataIndex].mDataTimeStamp == 0)
        RangingData[mCurDataIndex].mDataTimeStamp = StringDateTimeTotime_t(timestamp);
    RangingData[mCurDataIndex].mAverageDiffTime = newDiffTime;

    // printf ("AnchorRangingDataClass::putRanging() AnchorName[%s] ranging[%d] timestamp[%s] -> [%s] InMS[%s]\n",
    //     AnchorName, ranging, timestamp, time_tToString(RangingData[mCurDataIndex].mDataTime).c_str(),
    //     time_tToString(RangingData[mCurDataIndex].mDataTimeInMS / 1000).c_str());

    double Velocity = (double)velocity / 10.0f;
    //RangingData[mCurDataIndex].TagVelocity = Velocity;
    RangingData[mCurDataIndex].TagVelocity = updateVelocity(Velocity);

    mPreDataIndex = mCurDataIndex;
    mCurDataIndex++;
    if (mCurDataIndex >= RANGING_BUFFER_SIZE)
        mCurDataIndex = 0;
    //==========================
    if ( mEnableKalman1D == 1 )
    {
        AnchorTagRanging = ranging;
        last_results[0] = AnchorTagRanging;
    }

    //
    // calculate Lost Rate
    //
    calculateLostRate();
    calculateStandardDeviation(ranging);

    //printf ("AnchorRangingDataClass::putRanging() AnchorName[%s] AnchorTagRanging[%d] mDataTime[%ld]\n", AnchorName, AnchorTagRanging, mDataTime);
}

void AnchorRangingDataClass::putRangingAOA(int rangingidx, int ranging, int velocity, float angle, int rssi, const char * timestamp)
{
    mDataCount++;

    if ( ranging <= 0 || ranging > 50000)
    {
        return;
    }

    //
    // 根據每次 LoRa gateway時間 與 local時間計算時間差, 求出平均秒差.
    //
    time_t oldTime = RangingData[mCurDataIndex].mAverageDiffTime;
    time_t curTimeMS = getCurrentTimeInMilliSec();
    time_t dataTime = StringTimeZoneDateTimeTotime_t(timestamp);
    if (dataTime == 0)
        dataTime = StringDateTimeTotime_t(timestamp);
    time_t diff_t = dataTime * 1000 - curTimeMS;
    time_t newDiffTime = (oldTime * 0.4) + (diff_t * 0.6);

    RangingData[mCurDataIndex].AnchorTagRangingRaw = ranging;
    RangingData[mCurDataIndex].AnchorTagRangingFiltered = ranging;
    RangingData[mCurDataIndex].AnchorTagRangingIndex = rangingidx;
    for(int j=0;j<6;j++)
        RangingData[mCurDataIndex].AnchorTagIMU[j] = 0;
    RangingData[mCurDataIndex].TagFinalRecvdBm = rssi * -1;
    RangingData[mCurDataIndex].TagFirstPathdBm = 0;
    RangingData[mCurDataIndex].AnchorRecvdBm = 0;
    RangingData[mCurDataIndex].AnchorFirstPathdBm = 0;

    RangingData[mCurDataIndex].mDataTime = getCurrentTimeInSec();
    RangingData[mCurDataIndex].mDataTimeInMS = getCurrentTimeInMilliSec();
    RangingData[mCurDataIndex].mDataTimeStamp = StringTimeZoneDateTimeTotime_t(timestamp);
    if (RangingData[mCurDataIndex].mDataTimeStamp == 0)
        RangingData[mCurDataIndex].mDataTimeStamp = StringDateTimeTotime_t(timestamp);
    RangingData[mCurDataIndex].mAverageDiffTime = newDiffTime;

    double Velocity = (double)velocity / 10.0f;
    //RangingData[mCurDataIndex].TagVelocity = Velocity;
    RangingData[mCurDataIndex].TagVelocity = updateVelocity(Velocity);

    // RangingData[mCurDataIndex].TagAngle = angle;
    RangingData[mCurDataIndex].TagAngle = rad_to_angle(angle);

    mPreDataIndex = mCurDataIndex;
    mCurDataIndex++;
    if (mCurDataIndex >= RANGING_BUFFER_SIZE)
        mCurDataIndex = 0;

    if ( mEnableKalman1D == 1 )
    {
        AnchorTagRanging = ranging;
        last_results[0] = AnchorTagRanging;
    }

    //
    // calculate Lost Rate
    //
    calculateLostRate();
    calculateStandardDeviation(ranging);

    mAoADataExtraDataTimeInMS = 10000; //10 seconds
}

void AnchorRangingDataClass::putFenceAOA(int rangingidx, int level_x, int level_y, int level_time, int rssi, const char * timestamp)
{
    mDataCount++;

    //
    // 根據每次 LoRa gateway時間 與 local時間計算時間差, 求出平均秒差.
    //
    time_t oldTime = RangingData[mCurDataIndex].mAverageDiffTime;
    time_t curTimeMS = getCurrentTimeInMilliSec();
    time_t dataTime = StringTimeZoneDateTimeTotime_t(timestamp);
    if (dataTime == 0)
        dataTime = StringDateTimeTotime_t(timestamp);
    time_t diff_t = dataTime * 1000 - curTimeMS;
    time_t newDiffTime = (oldTime * 0.4) + (diff_t * 0.6);

    RangingData[mCurDataIndex].AnchorTagRangingRaw = 0;
    RangingData[mCurDataIndex].AnchorTagRangingFiltered = 0;
    RangingData[mCurDataIndex].AnchorTagRangingIndex = rangingidx;
    for(int j=0;j<6;j++)
        RangingData[mCurDataIndex].AnchorTagIMU[j] = 0;
    RangingData[mCurDataIndex].TagFinalRecvdBm = rssi * -1;
    RangingData[mCurDataIndex].TagFirstPathdBm = 0;
    RangingData[mCurDataIndex].AnchorRecvdBm = 0;
    RangingData[mCurDataIndex].AnchorFirstPathdBm = 0;

    RangingData[mCurDataIndex].mDataTime = getCurrentTimeInSec();
    RangingData[mCurDataIndex].mDataTimeInMS = getCurrentTimeInMilliSec();
    RangingData[mCurDataIndex].mDataTimeStamp = StringTimeZoneDateTimeTotime_t(timestamp);
    if (RangingData[mCurDataIndex].mDataTimeStamp == 0)
        RangingData[mCurDataIndex].mDataTimeStamp = StringDateTimeTotime_t(timestamp);
    RangingData[mCurDataIndex].mAverageDiffTime = newDiffTime;

    RangingData[mCurDataIndex].TagVelocity = 0;
    RangingData[mCurDataIndex].TagAngle = 0;

    RangingData[mCurDataIndex].TagLevel_x = level_x;
    RangingData[mCurDataIndex].TagLevel_y = level_y;
    RangingData[mCurDataIndex].TagLevel_time = level_time;

    mPreDataIndex = mCurDataIndex;
    mCurDataIndex++;
    if (mCurDataIndex >= RANGING_BUFFER_SIZE)
        mCurDataIndex = 0;

    // if ( mEnableKalman1D == 1 )
    // {
    //     AnchorTagRanging = ranging;
    //     last_results[0] = AnchorTagRanging;
    // }
    //
    // //
    // // calculate Lost Rate
    // //
    // calculateLostRate();
    // calculateStandardDeviation(ranging);

    mAoADataExtraDataTimeInMS = 10000; //10 seconds

}

void AnchorRangingDataClass::putRangingDiag(int rangingidx, int ranging, int * imu,
                    int PollCnt, int RangingAnchorCnt, int TagFinalRecvdBm, int TagFirstPathdBm,
                    int AnchorRecvdBm, int AnchorFirstPathdBm, int RangingCount, int RangingFailCount, float DataRate)
{
    mDataCount++;

    // vilslog::printf("AnchorRangingDataClass::putRangingDiag()\n AnchorName[%s] rangingidx[%d] PollCnt[%d] ranging[%d] RangingAnchorCnt[%d] mDataRate[%.02f]\n",
    //             AnchorName, rangingidx, PollCnt, ranging, RangingAnchorCnt, mDataRate);

    if ( ranging <= 0 || ranging > 50000)
    {
        //printf ("AnchorRangingDataClass::putRanging() Ignore AnchorName[%s] ranging = %d\n", AnchorName, ranging);
        return;
    }
    int rangingRaw = ranging;
    mRangingCount = RangingCount;
    mRangingFailCount = RangingFailCount;
    mDataRate = DataRate;

    mDataRate_results[0] = mDataRate;
    mDataRateSmoothingFilter->update(mDataRate_results, mDataRate_results);

    //struct timeval tv;
    double Velocity = 0.0;
    // first data in
    if (mPreDataIndex == mCurDataIndex)
    {
        Velocity = 0.0;
    }
    else
    {
    	//
    	// using IMU data
    	//
        Velocity = calcVelocity(RangingData[mPreDataIndex].AnchorTagIMU, imu);
    }

//#ifdef APPLY_NOISE_MAKEUP_1D
    if (mEnableNoiseMakup == 1)
    {
        //
        // compare previous value, if too big this time, ignore it.
        //
        float diff = (float)ranging - (float)RangingData[mPreDataIndex].AnchorTagRangingFiltered;
        int direction = 0;
        if ( diff < 0.0f )
        {
            direction = 1;
        }
        float diffabs = fabs(diff);

        float ratio = 1.0f;
        if (mAverageDiffRanging != 0.0f)
        {
    		ratio = diffabs / mAverageDiffRanging;
            //printf ("AnchorTagCoupleDataClass::putRangingDiag() ratio[%.02f] diff[%.02f] mAverageDiffRanging[%.02f]\n", ratio, diffabs, mAverageDiffRanging);
        }

        //printf ("AnchorName[%s] ranging[%d] diff[%0.2f] mAverageDiffRanging[%0.2f] ratio[%0.2f] pre ranging[%d]\n",
        //    AnchorName, ranging, diff, mAverageDiffRanging, ratio, RangingData[mPreDataIndex].AnchorTagRanging);

        if ( ratio >= mNoiseMakupRatio && RangingData[mPreDataIndex].AnchorTagRangingFiltered > 50)
    	{
            //printf ("AnchorTagCoupleDataClass::putRangingDiag() ranging[%d] ratio[%f] diff[%f] \n", ranging, ratio, diff);
    		//printf ("AnchorTagCoupleDataClass::putRangingDiag() diff[%f] AnchorTagRanging[mPreDataIndex][%d]\n", diff, RangingData[mPreDataIndex].AnchorTagRanging);
    		//break;

            //
            // replace with old one
            //
            //int oldrangeing = ranging;
            float diffCorrection = diff;

            if (ratio > 100.0f)
            {
                diffCorrection = mAverageDiffRanging * 20.0f;
                //printf ("AnchorTagCoupleDataClass::putRangingDiag() ranging[%d] ratio[%.02f] diff[%.02f] mAverageDiffRanging[%0.2f] diffCorrection[%.02f]\n",
                //        ranging, ratio, diff, mAverageDiffRanging, diffCorrection);
            }


            ranging = RangingData[mPreDataIndex].AnchorTagRangingFiltered;
            if (direction == 0)
            {
                ranging += mAverageDiffRanging;
            }
            else
            {
                diffCorrection *= -1.0f;
                ranging -= mAverageDiffRanging;
            }

            // calculate average diff
            //float w1 = (float)(RANGING_BUFFER_SIZE - 1) /(float)RANGING_BUFFER_SIZE;
            float w1 = mNoiseMakupWeight;
            mAverageDiffRanging = mAverageDiffRanging * w1 + diffCorrection * (1.0f - w1);

            //printf ("AnchorTagCoupleDataClass::putRangingDiag()\n Replace rangingRaw[%d] new ranging[%d] direction[%d] pre ranging[%d] mAverageDiffRanging[%.02f]\n",
           //                 rangingRaw, ranging, direction, RangingData[mPreDataIndex].AnchorTagRangingFiltered, mAverageDiffRanging);
        }
        else
        {
            // calculate average diff
            //float w1 = (float)(RANGING_BUFFER_SIZE - 1) /(float)RANGING_BUFFER_SIZE;
            float w1 = mNoiseMakupWeight;
            mAverageDiffRanging = mAverageDiffRanging * w1 + diff * (1.0f - w1);
        }
    }
//#endif // APPLY_NOISE_MAKEUP_1D


    //==========================
    //gettimeofday(&tv, NULL);
    RangingData[mCurDataIndex].AnchorTagRangingRaw = rangingRaw;
    RangingData[mCurDataIndex].AnchorTagRangingFiltered = ranging;
    RangingData[mCurDataIndex].AnchorTagRangingIndex = rangingidx;
    for(int j=0;j<6;j++)
        RangingData[mCurDataIndex].AnchorTagIMU[j] = imu[j];
    //RangingData[mCurDataIndex].mDataTime = tv.tv_sec;
    RangingData[mCurDataIndex].TagPollCnt = PollCnt;
    RangingData[mCurDataIndex].RangingAnchorCnt = RangingAnchorCnt;
    RangingData[mCurDataIndex].TagFinalRecvdBm = TagFinalRecvdBm;
    RangingData[mCurDataIndex].TagFirstPathdBm = TagFirstPathdBm;
    RangingData[mCurDataIndex].AnchorRecvdBm = AnchorRecvdBm;
    RangingData[mCurDataIndex].AnchorFirstPathdBm = AnchorFirstPathdBm;
    RangingData[mCurDataIndex].mDataTime = getCurrentTimeInSec();
    RangingData[mCurDataIndex].mDataTimeInMS = getCurrentTimeInMilliSec();
    RangingData[mCurDataIndex].mDataTimeStamp = getCurrentTimeInSec();
    //RangingData[mCurDataIndex].mDataTime = getCurrentTimeInMilliSec();

    //RangingData[mCurDataIndex].TagVelocity = Velocity;
    RangingData[mCurDataIndex].TagVelocity = updateVelocity(Velocity);

    //
    // calculate Lost Rate
    //
    calculateLostRate();
    calculateStandardDeviation(ranging);

    // time_t preDataTimeInMS = RangingData[mPreDataIndex].mDataTimeInMS;
    // time_t curDataTimeInMS = RangingData[mCurDataIndex].mDataTimeInMS;
    // int diff = (int)(curDataTimeInMS - preDataTimeInMS);
    // mDataRate = 1000.0f / (float)diff;

    mPreDataIndex = mCurDataIndex;
    mCurDataIndex++;
    if (mCurDataIndex >= RANGING_BUFFER_SIZE)
        mCurDataIndex = 0;
    //mLastDataTime = RangingData[mCurDataIndex].mDataTime;
    //==========================
    if ( mEnableKalman1D == 1 )
    {
        AnchorTagRanging = ranging;
        last_results[0] = AnchorTagRanging;
    }
}

int AnchorRangingDataClass::getLostRate()
{
    return mCurLostRate;
}

float AnchorRangingDataClass::getDataRate()
{
    return mDataRate_results[0];
    //return mDataRate;
}

float AnchorRangingDataClass::getValidRate()
{
    return mValidRate;
}

float AnchorRangingDataClass::getStandardDeviation()
{
    return mStandardDeviation;
}

int AnchorRangingDataClass::getDataCount()
{
    return mDataCount;
}

int AnchorRangingDataClass::update()
{
    if ( mEnableKalman1D == 1 )
    {
        kalman1d->update(last_results, kmresult);
        AnchorTagRanging = kmresult[0];
    }

    return 0;
}

bool AnchorRangingDataClass::checkValidRanging(int validdatatime)
{
    if ( strlen(AnchorName) > 0 )
    {
// #ifdef APPLY_KALMAN_1D
//         return true;
// #else

        // check time
        time_t latest_time;
        //struct timeval tv;
        //gettimeofday(&tv, NULL);
        //latest_time = tv.tv_sec;
        //latest_time = tv.tv_usec / 1000; // micro to milli
        //time_t diff = latest_time - RangingData[mPreDataIndex].mDataTime;
        latest_time = getCurrentTimeInMilliSec();
        time_t diff = latest_time - RangingData[mPreDataIndex].mDataTimeInMS;

        // vilslog::printf ("AnchorRangingDataClass::checkValidRanging() AnchorName[%s] latest_time[%ld] mDataTimeInMS[%ld] diff[%ld]\n",
        // AnchorName, latest_time, RangingData[mPreDataIndex].mDataTimeInMS, diff);
        //
        // vilslog::printf ("AnchorRangingDataClass::checkValidRanging() AnchorName[%s] latest_time[%s] mDataTimeInMS[%s] diff[%s]\n",
        // AnchorName, time_tToString(latest_time).c_str(), time_tToString(RangingData[mPreDataIndex].mDataTimeInMS).c_str(), time_tToString(diff).c_str());


        int i_diff = (int)diff;
        if ( i_diff < (validdatatime + mAoADataExtraDataTimeInMS) && i_diff >= 0)
        {
            return true;
        }
        return false;
// #endif
    }
    return false;
}

bool AnchorRangingDataClass::checkValidRanging(time_t latest_time, int validdatatime)
{
    if ( strlen(AnchorName) > 0 )
    {
        time_t diff = latest_time - RangingData[mPreDataIndex].mDataTimeInMS;
        //printf ("AnchorRangingDataClass::checkValidRanging() AnchorName[%s] latest_time[%ld] mDataTime[%ld] diff[%ld]\n", AnchorName, latest_time, mDataTime, diff);
        int i_diff = (int)diff;
        if ( i_diff < (validdatatime + mAoADataExtraDataTimeInMS) && i_diff >= 0)
        {
            return true;
        }
        return false;
    }
    return false;
}

int AnchorRangingDataClass::ValidRangingCount(int validdatatime)
{
    int count = 0;
    time_t latest_time = getCurrentTimeInMilliSec();

    if ( strlen(AnchorName) == 0 )
    {
        return count;
    }

    for (int i=0;i<RANGING_BUFFER_SIZE;i++)
    {
        //
        // check a range
        //
        time_t diff = latest_time - RangingData[i].mDataTimeInMS;
        int i_diff = (int)diff;
        if ( i_diff < (validdatatime + mAoADataExtraDataTimeInMS) && i_diff >= 0)
        {
            count++;
        }
    }

    //
    // calculate valid rate
    //
    mValidRate = mValidRate * 0.4f + count * 0.6f;

    return count;
}

int AnchorRangingDataClass::getAverageDiffRanging(int validdatatime)
{
    int count = 0;
    int preRanging = -1;
    int totalDiffRanging = 0;
    int averageDiffRanging = -1;
    time_t latest_time = getCurrentTimeInMilliSec();

    if ( strlen(AnchorName) == 0 )
    {
        return averageDiffRanging;
    }

    for (int i=0;i<RANGING_BUFFER_SIZE;i++)
    {
        //
        // check a range
        //
        time_t diff = latest_time - RangingData[i].mDataTimeInMS;
        int i_diff = (int)diff;
        if ( i_diff < (validdatatime + mAoADataExtraDataTimeInMS) && i_diff >= 0)
        {
            count++;

            if ( preRanging < 0 )
            {
                preRanging = RangingData[i].AnchorTagRangingFiltered;
            }
            else
            {
                int rangingDiff = abs(preRanging - RangingData[mPreDataIndex].AnchorTagRangingFiltered);
                totalDiffRanging += rangingDiff;
            }
        }
    }

    if(totalDiffRanging > 0)
    {
        averageDiffRanging = totalDiffRanging / count;
    }

    return averageDiffRanging;
}

int AnchorRangingDataClass::getRangingRaw()
{
    return RangingData[mPreDataIndex].AnchorTagRangingRaw;
}

int AnchorRangingDataClass::getRangingFiltered()
{
//#ifdef APPLY_KALMAN_1D
    if ( mEnableKalman1D == 1 )
    {
        return AnchorTagRanging;
    }
    //#else
    else
    {
        return RangingData[mPreDataIndex].AnchorTagRangingFiltered;
    }
//#endif
}

int AnchorRangingDataClass::getTagPollCnt()
{
    return RangingData[mPreDataIndex].TagPollCnt;
}

int AnchorRangingDataClass::getRangingAnchorCnt()
{
    return RangingData[mPreDataIndex].RangingAnchorCnt;
}

int AnchorRangingDataClass::getTagFinalRecvdBm()
{
    return RangingData[mPreDataIndex].TagFinalRecvdBm;
}

int AnchorRangingDataClass::getTagFirstPathdBm()
{
    return RangingData[mPreDataIndex].TagFirstPathdBm;
}

int AnchorRangingDataClass::getAnchorRecvdBm()
{
    return RangingData[mPreDataIndex].AnchorRecvdBm;
}

int AnchorRangingDataClass::getAnchorFirstPathdBm()
{
    return RangingData[mPreDataIndex].AnchorFirstPathdBm;
}

time_t AnchorRangingDataClass::getDataTimeInMS()
{
    return RangingData[mPreDataIndex].mDataTimeInMS;
}

void AnchorRangingDataClass::getIMU(double * IMU, int valuecount)
{
    for (int i = 0; i < valuecount; i++)
        IMU[i] = RangingData[mPreDataIndex].AnchorTagIMU[i];
}

void AnchorRangingDataClass::getIMUi(int * IMU, int valuecount)
{
    for (int i = 0; i < valuecount; i++)
        IMU[i] = RangingData[mPreDataIndex].AnchorTagIMU[i];
}

double AnchorRangingDataClass::getVelocity()
{
    return RangingData[mPreDataIndex].TagVelocity;
}

float AnchorRangingDataClass::getAngle()
{
    return RangingData[mPreDataIndex].TagAngle;
}

int AnchorRangingDataClass::getLevel_x()
{
    return RangingData[mPreDataIndex].TagLevel_x;
}

int AnchorRangingDataClass::getLevel_y()
{
    return RangingData[mPreDataIndex].TagLevel_y;
}

int AnchorRangingDataClass::getLevel_time()
{
    return RangingData[mPreDataIndex].TagLevel_time;
}

time_t AnchorRangingDataClass::getDataTime()
{
    return RangingData[mPreDataIndex].mDataTime;
}

time_t AnchorRangingDataClass::getDataTimeStamp()
{
    return RangingData[mPreDataIndex].mDataTimeStamp;
}

char * AnchorRangingDataClass::getAnchorName()
{
    return AnchorName;
}


int AnchorRangingDataClass::dump_ranging_data()
{
    if ( strlen(AnchorName) <= 0 )
        return 0;
    vilslog::printf ("dump_ranging_data() %s mCurDataIndex[%d]\n", AnchorName, mCurDataIndex);
    for (int i=0;i<RANGING_BUFFER_SIZE;i++)
    {
        if ( RangingData[i].AnchorTagRangingIndex >= 0 )
        {
            vilslog::printf ("[%02d] [%d,%d,%ld]\n", i, RangingData[i].AnchorTagRangingIndex, RangingData[i].AnchorTagRangingRaw, RangingData[i].mDataTimeInMS);
        }
    }

    return 0;
}

int AnchorRangingDataClass::setArguments(int EnableNoiseMakup,
    float NoiseMakupRatio,
    float NoiseMakupWeight,
    int EnableKalman1D)
{
    mEnableNoiseMakup = EnableNoiseMakup;
    mNoiseMakupRatio = NoiseMakupRatio;
    mNoiseMakupWeight = NoiseMakupWeight;
    mEnableKalman1D = EnableKalman1D;

    return 0;
}

double AnchorRangingDataClass::updateVelocity(double Velocity)
{
    double tagVelocity = Velocity;
    if ( Velocity <= 0.1 )
    {
        if (mVelocityEqualZeroTimeStamp == 0)
        {
            mVelocityEqualZeroTimeStamp = getCurrentTimeInSec();
        }
        else
        {
            time_t curtime = getCurrentTimeInSec();
            int difft = (int)(curtime - mVelocityEqualZeroTimeStamp);
            if ( difft > 10 )
            {
                // vilslog::printf ("AnchorRangingDataClass::updateVelocity() difft[%d] Velocity[%f] tagVelocity[%f] mPreTagVelocity[%f]\n",
                //     difft, Velocity, tagVelocity, mPreTagVelocity);

                tagVelocity = Velocity;

            }
            else
            {
                // keep same Velocity
                tagVelocity = mPreTagVelocity;
            }
        }
    }
    else
    {
        tagVelocity = Velocity;
        mVelocityEqualZeroTimeStamp = 0;
    }

    mPreTagVelocity = tagVelocity;

    return tagVelocity;
}

float AnchorRangingDataClass::calculateLostRate()
{
    //float lostRate = 0.0f;
    mPreLostRate = mCurLostRate;
    if (mRangingCount == 0)
        mCurLostRate = 100;
    else
        mCurLostRate = (100 * mRangingFailCount) / (mRangingCount * 2);

/*
    if (mPreDataIndex == mCurDataIndex)
    {
        return mPreLostRate;
    }

    int diffIndex = RangingData[mCurDataIndex].AnchorTagRangingIndex - RangingData[mPreDataIndex].AnchorTagRangingIndex;
    if (diffIndex < 0)
        diffIndex += 255;


    int LostCount = 0;
    if (RangingData[mCurDataIndex].RangingAnchorCnt > 0)
    {
        LostCount = diffIndex / RangingData[mCurDataIndex].RangingAnchorCnt;
    }

    if (LostCount > 0)
    {
        //printf("calculateLostRate() RangingData mCurDataIndex[%d] mPreDataIndex[%d] diffIndex[%d]\n",
        //            RangingData[mCurDataIndex].AnchorTagRangingIndex, RangingData[mPreDataIndex].AnchorTagRangingIndex, diffIndex);
        //printf("calculateLostRate() AnchorName[%s] Lost LostCount[%d]\n", AnchorName, LostCount);
    }
    mTotalLostRateCount = mTotalLostRateCount * 0.05f + (float)LostCount * 0.95f;

    mPreLostRate = mCurLostRate;
    if (mCalLostRateCount < 20)
    {
        mCalLostRateCount++;
        mCurLostRate = 0.0;
    }
    else
    {
        mCurLostRate = 100.0f * ((float)mTotalLostRateCount / (float)mCalLostRateCount);
        //printf("calculateLostRate() AnchorName[%s] mCurLostRate[%.02f]\n", AnchorName, mCurLostRate);
    }

    if ( mCurLostRate > 100.0f)
        mCurLostRate = 100.0f;


    // float successRate = 100.0f * (float)RangingData[mCurDataIndex].RangingAnchorCnt/(float)RangingData[mCurDataIndex].TagPollCnt;
    // lostRate = 100.0f - successRate;
    // if (lostRate < 0.0f)
    //     lostRate = 0.0f;
    //printf ("calculateLostRate() %s firstPollCnt[%d] lastPollCnt[%d] firstFinalCnt[%d] lastFinalCnt[%d] windowSize[%d]\n",
    //        AnchorName, firstPollCnt, lastPollCnt, firstFinalCnt, lastFinalCnt, windowSize);
    // mPreLostRate = mCurLostRate;
    // mCurLostRate = lostRate;
    //mCurLostRate = (mCurLostRate + lostRate) / 2.0f;


    // vilslog::printf ("%s mTotalAnchorCount[%d] mActiveAnchorCount[%d] mRangingFailureCount[%d]\n",
    //         AnchorName, mTotalAnchorCount, mActiveAnchorCount, mRangingFailureCount);

    // vilslog::printf ("%s diffPollCnt[%d] diffFinalCnt[%d] successCount[%.02f] lostRate[%.02f] mCurLostRate[%.02f]\n",
    //         AnchorName, diffPollCnt, diffFinalCnt, successCount, lostRate, mCurLostRate);
*/
    return mCurLostRate;
}

float AnchorRangingDataClass::calculateStandardDeviation(int ranging)
{
    float total_m = 0.0f;

    for(int i=0; i<StandardDeviationCOUNT - 1;i++)
    {
        // remove oldest data
        mAnchorTagRanging[i] = mAnchorTagRanging[i + 1];
        total_m += (float)mAnchorTagRanging[i];
    }
    mAnchorTagRanging[StandardDeviationCOUNT - 1] = ranging;
    total_m += (float)ranging;

    float meaning_value = (float)(total_m / (float)StandardDeviationCOUNT);

    float total = 0.0f;
    for(int i=0; i<StandardDeviationCOUNT - 1;i++)
    {
        total += pow((float)mAnchorTagRanging[i] - meaning_value, 2);
    }

    mStandardDeviation = sqrt((float)(total / (float)StandardDeviationCOUNT));

    // if (mStandardDeviation >= 50.0)
    // {
    //     vilslog::printf ("calculateStandardDeviation AnchorName[%s], ranging[%d] total_m[%.02f] meaning_value[%.02f] total[%.02f] mStandardDeviation[%.02f]\n",
    //             AnchorName, ranging, total_m, meaning_value, total, mStandardDeviation);
    // }

    return mStandardDeviation;
}
