#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "string.h"

#include "../common.h"
#include "AnchorTagCoupleDataClass.h"
#include "../Filters/kalman/Kalman3D.h"
#include "../Filters/SmoothingFilter/SmoothingFilter.h"
#include "../Filters/WeightedMovingAverageFilter/WeightedMovingAverageFilter.h"
//
// AnchorTagCoupleDataClass
//
AnchorTagCoupleDataClass::AnchorTagCoupleDataClass()
{

}

AnchorTagCoupleDataClass::~AnchorTagCoupleDataClass()
{
    delete mKalman3DFilter;
    delete mSmoothingFilter;
    delete mWeightedMovingAverageFilter;
    delete mLastRangingResult;

    //delete [] mAnchorData;
    clear_AnchorRangingDataClass_vector(&mAnchorData);
}

void AnchorTagCoupleDataClass::init(double posX, double posY, double posZ)
{
    // initial

	last_results[0] = posX;
	last_results[1] = posY;
	last_results[2] = posZ;
    raw_results[0] = posX;
	raw_results[1] = posY;
	raw_results[2] = posZ;

    mKalman3DFilter = new Kalman3D();
    mSmoothingFilter = new SmoothingFilter();
    mWeightedMovingAverageFilter = new WeightedMovingAverageFilter(0.01, 0.02, 0.97);


    double tmpresult[3];
    mKalman3DFilter->initial(last_results);
    mSmoothingFilter->initial(last_results);
    mWeightedMovingAverageFilter->initial(last_results);

    mKalman3DFilter->update(last_results, tmpresult);
    mSmoothingFilter->update(last_results, tmpresult);
    mWeightedMovingAverageFilter->update(last_results, tmpresult);

    mCurrentFilter = mKalman3DFilter;

    mTagVelocity = 0.0;
    kalman1dVelocity = new Kalman1D();
    kmVelocity[0] = 0.0;
    lastVelocity[0] = mTagVelocity;
    kalman1dVelocity->update(lastVelocity, kmVelocity);
    mVelocityEqualZeroTimeStamp = 0;

    mLatestRangingIndex = -1;
    mRangingCount = 0;
    mNotValidRangingCount = 0;

    mCalculationMethod = 0;
    mEnableRangingReliability = 0;
    mRangingReliabilityMaxDistance = 3000;
    mRangingReliabilityMinCount = 1;
    mRangingReliabilityAverageDiff = 50;

	memset(TagName, 0, 32);

    // mAnchorData = new AnchorRangingDataClass[MAX_ANCHOR_NUMBER];
	// for (int i=0;i<MAX_ANCHOR_NUMBER;i++)
	// {
    //     mAnchorData[i].init();
	// }

    mLastRangingResult = new RangingResultStruct();
}

void AnchorTagCoupleDataClass::putRanging(const char * anchorname, int MapLayer, int rangingidx, int ranging, int * imu)
{

    mLatestRangingIndex = rangingidx;

    //===================================================
    bool bFound = false;
    for (unsigned int i=0; i< mAnchorData.size(); i++)
    {
        if ( strcmp(anchorname, mAnchorData[i]->AnchorName) == 0)
    	{
            mAnchorData[i]->MapLayer = MapLayer;
            mAnchorData[i]->putRanging(rangingidx, ranging, imu);

            bFound = true;
            break;
        }
    }
    if (!bFound)
    {
        AnchorRangingDataClass * newData = new AnchorRangingDataClass();
        strcpy(newData->AnchorName, anchorname);
        newData->MapLayer = MapLayer;
        newData->putRanging(rangingidx, ranging, imu);

        mAnchorData.push_back(newData);
    }
    //===================================================
    // for(int i=0;i<MAX_ANCHOR_NUMBER;i++)
    // {
    // 	if ( strcmp(anchorname, mAnchorData[i].AnchorName) == 0)
    // 	{
    //         // vilslog::printf("AnchorTagCoupleDataClass::putputRangingRanging() imu TagName[%s] anchorname[%s] MapLayer[%d]\n", TagName, anchorname, MapLayer);
    //
    //         mAnchorData[i].MapLayer = MapLayer;
    //         mAnchorData[i].putRanging(rangingidx, ranging, imu);
    //         break;
    // 	}
    // 	else
    // 	if (strlen(mAnchorData[i].AnchorName) == 0)
    // 	{
    // 		strcpy(mAnchorData[i].AnchorName, anchorname);
    //         mAnchorData[i].MapLayer = MapLayer;
    //         mAnchorData[i].putRanging(rangingidx, ranging, imu);
    // 		break;
    // 	}
    // }
    //===================================================
}

void AnchorTagCoupleDataClass::putRanging(const char * anchorname, int MapLayer, int rangingidx, int ranging, int velocity)
{

    mLatestRangingIndex = rangingidx;

    //===================================================
    bool bFound = false;
    for (unsigned int i=0; i< mAnchorData.size(); i++)
    {
        if ( strcmp(anchorname, mAnchorData[i]->AnchorName) == 0)
    	{
            mAnchorData[i]->MapLayer = MapLayer;
            mAnchorData[i]->putRanging(rangingidx, ranging, velocity);

            bFound = true;
            break;
        }
    }
    if (!bFound)
    {
        AnchorRangingDataClass * newData = new AnchorRangingDataClass();
        strcpy(newData->AnchorName, anchorname);
        newData->MapLayer = MapLayer;
        newData->putRanging(rangingidx, ranging, velocity);

        mAnchorData.push_back(newData);
    }
    //===================================================
    // for(int i=0;i<MAX_ANCHOR_NUMBER;i++)
    // {
    // 	if ( strcmp(anchorname, mAnchorData[i].AnchorName) == 0)
    // 	{
    //         // vilslog::printf("AnchorTagCoupleDataClass::putputRangingRanging() velocity TagName[%s] anchorname[%s] MapLayer[%d]\n", TagName, anchorname, MapLayer);
    //
    //         mAnchorData[i].MapLayer = MapLayer;
    //         mAnchorData[i].putRanging(rangingidx, ranging, velocity);
    //         break;
    // 	}
    // 	else
    // 	if (strlen(mAnchorData[i].AnchorName) == 0)
    // 	{
    // 		strcpy(mAnchorData[i].AnchorName, anchorname);
    //         mAnchorData[i].MapLayer = MapLayer;
    //         mAnchorData[i].putRanging(rangingidx, ranging, velocity);
    // 		break;
    // 	}
    // }
    //===================================================
}

void AnchorTagCoupleDataClass::putRanging(const char * anchorname, int MapLayer, int rangingidx, int ranging, int velocity, int rssi, const char * timestamp)
{

    mLatestRangingIndex = rangingidx;

    //===================================================
    bool bFound = false;
    for (unsigned int i=0; i< mAnchorData.size(); i++)
    {
        if ( strcmp(anchorname, mAnchorData[i]->AnchorName) == 0)
    	{
            mAnchorData[i]->MapLayer = MapLayer;
            mAnchorData[i]->putRanging(rangingidx, ranging, velocity, rssi, timestamp);

            bFound = true;
            break;
        }
    }
    if (!bFound)
    {
        AnchorRangingDataClass * newData = new AnchorRangingDataClass();
        strcpy(newData->AnchorName, anchorname);
        newData->MapLayer = MapLayer;
        newData->putRanging(rangingidx, ranging, velocity, rssi, timestamp);

        mAnchorData.push_back(newData);
    }
    //===================================================
    // for(int i=0;i<MAX_ANCHOR_NUMBER;i++)
    // {
    // 	if ( strcmp(anchorname, mAnchorData[i].AnchorName) == 0)
    // 	{
    //         // vilslog::printf("AnchorTagCoupleDataClass::putRanging() rssi TagName[%s] anchorname[%s] MapLayer[%d]\n", TagName, anchorname, MapLayer);
    //
    //         mAnchorData[i].MapLayer = MapLayer;
    //         mAnchorData[i].putRanging(rangingidx, ranging, velocity, rssi, timestamp);
    //         break;
    // 	}
    // 	else
    // 	if (strlen(mAnchorData[i].AnchorName) == 0)
    // 	{
    // 		strcpy(mAnchorData[i].AnchorName, anchorname);
    //         mAnchorData[i].MapLayer = MapLayer;
    //         mAnchorData[i].putRanging(rangingidx, ranging, velocity, rssi, timestamp);
    // 		break;
    // 	}
    // }
    //===================================================
}

void AnchorTagCoupleDataClass::putRangingAOA(const char * anchorname, int MapLayer, int rangingidx, int ranging, int velocity, float angle, int rssi, const char * timestamp)
{

    mLatestRangingIndex = rangingidx;

    //===================================================
    bool bFound = false;
    for (unsigned int i=0; i< mAnchorData.size(); i++)
    {
        if ( strcmp(anchorname, mAnchorData[i]->AnchorName) == 0)
    	{
            mAnchorData[i]->MapLayer = MapLayer;
            mAnchorData[i]->putRangingAOA(rangingidx, ranging, velocity, angle, rssi, timestamp);

            bFound = true;
            break;
        }
    }
    if (!bFound)
    {
        AnchorRangingDataClass * newData = new AnchorRangingDataClass();
        strcpy(newData->AnchorName, anchorname);
        newData->MapLayer = MapLayer;
        newData->putRangingAOA(rangingidx, ranging, velocity, angle, rssi, timestamp);

        mAnchorData.push_back(newData);
    }
    //===================================================
    // for(int i=0;i<MAX_ANCHOR_NUMBER;i++)
    // {
    // 	if ( strcmp(anchorname, mAnchorData[i].AnchorName) == 0)
    // 	{
    //
    //         // vilslog::printf("AnchorTagCoupleDataClass::putRangingAOA() TagName[%s] anchorname[%s] MapLayer[%d]\n", TagName, anchorname, MapLayer);
    //
    //         mAnchorData[i].MapLayer = MapLayer;
    //         mAnchorData[i].putRangingAOA(rangingidx, ranging, velocity, angle, rssi, timestamp);
    //         break;
    // 	}
    // 	else
    // 	if (strlen(mAnchorData[i].AnchorName) == 0)
    // 	{
    // 		strcpy(mAnchorData[i].AnchorName, anchorname);
    //         mAnchorData[i].MapLayer = MapLayer;
    //         mAnchorData[i].putRangingAOA(rangingidx, ranging, velocity, angle, rssi, timestamp);
    // 		break;
    // 	}
    // }
    //===================================================
}

void AnchorTagCoupleDataClass::putFenceAOA(const char * anchorname, int MapLayer, int rangingidx, int level_x, int level_y, int level_time, int rssi, const char * timestamp)
{
    mLatestRangingIndex = rangingidx;

    //===================================================
    bool bFound = false;
    for (unsigned int i=0; i< mAnchorData.size(); i++)
    {
        if ( strcmp(anchorname, mAnchorData[i]->AnchorName) == 0)
    	{
            mAnchorData[i]->MapLayer = MapLayer;
            mAnchorData[i]->putFenceAOA(rangingidx, level_x, level_y, level_time, rssi, timestamp);

            bFound = true;
            break;
        }
    }
    if (!bFound)
    {
        AnchorRangingDataClass * newData = new AnchorRangingDataClass();
        strcpy(newData->AnchorName, anchorname);
        newData->MapLayer = MapLayer;
        newData->putFenceAOA(rangingidx, level_x, level_y, level_time, rssi, timestamp);

        mAnchorData.push_back(newData);
    }
}

void AnchorTagCoupleDataClass::putRangingDiag(const char * anchorname, int MapLayer, int rangingidx, int ranging, int * imu,
                    int PollCnt, int RangingAnchorCnt, int TagFinalRecvdBm, int TagFirstPathdBm,
                    int AnchorRecvdBm, int AnchorFirstPathdBm, int RangingCount, int RangingFailCount, float DataRate)
{
    mLatestRangingIndex = rangingidx;

    //
    // decode AnchorRangingTable
    // "0000=0:003a=2:0019=2:001a=2:0008=2:003b=2:0003=2:0001=2"
    // std::list<AnchorRangingCountStruct *> * AnchorRangingList = parse_anchor_ranging_table(AnchorRangingTable);
    //
    //
    // clear_AnchorRangingCount_list(AnchorRangingList);
    // delete AnchorRangingList;

    //===================================================
    bool bFound = false;
    for (unsigned int i=0; i< mAnchorData.size(); i++)
    {
        if ( strcmp(anchorname, mAnchorData[i]->AnchorName) == 0)
    	{
            mAnchorData[i]->MapLayer = MapLayer;
            mAnchorData[i]->putRangingDiag(rangingidx, ranging, imu,
                    PollCnt, RangingAnchorCnt, TagFinalRecvdBm, TagFirstPathdBm, AnchorRecvdBm, AnchorFirstPathdBm, RangingCount,
                    RangingFailCount, DataRate);

            bFound = true;
            break;
        }
    }
    if (!bFound)
    {
        AnchorRangingDataClass * newData = new AnchorRangingDataClass();
        strcpy(newData->AnchorName, anchorname);
        newData->MapLayer = MapLayer;
        newData->putRangingDiag(rangingidx, ranging, imu,
                PollCnt, RangingAnchorCnt, TagFinalRecvdBm, TagFirstPathdBm, AnchorRecvdBm, AnchorFirstPathdBm, RangingCount,
                RangingFailCount, DataRate);

        mAnchorData.push_back(newData);
    }
    //===================================================
    // for(int i=0;i<MAX_ANCHOR_NUMBER;i++)
    // {
    // 	if ( strcmp(anchorname, mAnchorData[i].AnchorName) == 0)
    // 	{
    //         //printf("AnchorTagCoupleDataClass::putRangingDiag() 1 anchorname[%s] ranging[%d]\n", anchorname, ranging);
    //         // vilslog::printf("AnchorTagCoupleDataClass::putRangingDiag() TagName[%s] anchorname[%s] MapLayer[%d]\n", TagName, anchorname, MapLayer);
    //
    //         mAnchorData[i].MapLayer = MapLayer;
    //         mAnchorData[i].putRangingDiag(rangingidx, ranging, imu,
    //                 PollCnt, RangingAnchorCnt, TagFinalRecvdBm, TagFirstPathdBm, AnchorRecvdBm, AnchorFirstPathdBm, RangingCount,
    //                 RangingFailCount, DataRate);
    //         break;
    // 	}
    // 	else
    // 	if (strlen(mAnchorData[i].AnchorName) == 0)
    // 	{
    //         //printf("AnchorTagCoupleDataClass::putRangingDiag() 2 anchorname[%s] ranging[%d]\n", anchorname, ranging);
    //
    // 		strcpy(mAnchorData[i].AnchorName, anchorname);
    //         mAnchorData[i].MapLayer = MapLayer;
    //         mAnchorData[i].putRangingDiag(rangingidx, ranging, imu,
    //                 PollCnt, RangingAnchorCnt, TagFinalRecvdBm, TagFirstPathdBm, AnchorRecvdBm, AnchorFirstPathdBm,
    //                 RangingCount, RangingFailCount, DataRate);
    // 		break;
    // 	}
    // }
    //===================================================
}

int AnchorTagCoupleDataClass::prepareRangingData(RangingResultStruct * validRangingData, int validdatatime)
{
    int validCount = 0;
    validRangingData->validCount = 0;
    mLastRangingResult->validCount = 0;
    //time_t latest_time = getCurrentTimeInSec();
    time_t latest_time = getCurrentTimeInMilliSec();

    // for(int i=0;i<MAX_ANCHOR_NUMBER;i++)
    // {
    for (unsigned int i=0; i< mAnchorData.size(); i++)
    {
        if ( mAnchorData[i]->checkValidRanging(latest_time, validdatatime) )
        {
            int ValidRangingCount = mAnchorData[i]->ValidRangingCount(validdatatime);

            if (mEnableRangingReliability == 1)
            {
//#ifdef APPLY_RANGING_REALIABLITY

                int averageDiffRanging = mAnchorData[i]->getAverageDiffRanging(validdatatime);

                if ( ValidRangingCount <= mRangingReliabilityMinCount )
                {
                    // vilslog::printf("AnchorTagCoupleDataClass::prepareRangingData() i[%d] validdatatime[%d]\n", i, validdatatime);
                    // vilslog::printf("\t\tTagName[%s] AnchorName[%s] RangingRaw[%d] ValidCount[%d] AverageDiff[%d]\n",
                    //       TagName, mAnchorData[i].AnchorName, mAnchorData[i].getRangingRaw(), ValidRangingCount, averageDiffRanging);

                    continue;
                }

                if ( ValidRangingCount > mRangingReliabilityMinCount && averageDiffRanging > mRangingReliabilityAverageDiff )
                {
                    // vilslog::printf("AnchorTagCoupleDataClass::prepareRangingData() i[%d] validdatatime[%d]\n", i, validdatatime);
                    // vilslog::printf("\t\tTagName[%s] AnchorName[%s] RangingRaw[%d] ValidCount[%d] AverageDiff[%d]\n",
                    //       TagName, mAnchorData[i].AnchorName, mAnchorData[i].getRangingRaw(), ValidRangingCount, averageDiffRanging);

                    continue;
                }

                if ( mAnchorData[i]->getRangingFiltered() >= mRangingReliabilityMaxDistance)
                {
                    // vilslog::printf("AnchorTagCoupleDataClass::prepareRangingData() i[%d] validdatatime[%d]\n", i, validdatatime);
                    // vilslog::printf("\t\tTagName[%s] AnchorName[%s] RangingFiltered[%d] >= %d SKIP!!!!!!!!\n",
                    //       TagName, mAnchorData[i].AnchorName, mAnchorData[i].getRangingFiltered(), RANGING_REALIABLITY_MAX_DISTANCE);
                    continue;
                }
            }
//#endif
            //printf("AnchorTagCoupleDataClass::prepareRangingData() \n");
            //printf("\t\tTagName[%s] AnchorName[%s] ValidCount[%d] AverageDiff[%d]\n",
            //      TagName, mAnchorData[i].AnchorName, mAnchorData[i].ValidRangingCount(validdatatime), mAnchorData[i].getAverageDiffRanging(validdatatime));

            validRangingData->RangingRaw[validCount] = mAnchorData[i]->getRangingRaw();
            validRangingData->RangingFiltered[validCount] = mAnchorData[i]->getRangingFiltered();
            validRangingData->TagVelocity[validCount] = mTagVelocity;
            validRangingData->TagAngle[validCount] = mAnchorData[i]->getAngle();
            validRangingData->TagLevel_x[validCount] = mAnchorData[i]->getLevel_x();
            validRangingData->TagLevel_y[validCount] = mAnchorData[i]->getLevel_y();
            validRangingData->TagLevel_time[validCount] = mAnchorData[i]->getLevel_time();
            validRangingData->TagPollCnt[validCount] = mAnchorData[i]->getTagPollCnt();
            validRangingData->RangingAnchorCnt[validCount] = mAnchorData[i]->getRangingAnchorCnt();
            validRangingData->TagFinalRecvdBm[validCount] = mAnchorData[i]->getTagFinalRecvdBm();
            validRangingData->TagFirstPathdBm[validCount] = mAnchorData[i]->getTagFirstPathdBm();
            validRangingData->AnchorRecvdBm[validCount] = mAnchorData[i]->getAnchorRecvdBm();
            validRangingData->AnchorFirstPathdBm[validCount] = mAnchorData[i]->getAnchorFirstPathdBm();
            validRangingData->LostRate[validCount] = mAnchorData[i]->getLostRate();
            validRangingData->DataRate[validCount] = mAnchorData[i]->getDataRate();
            validRangingData->DataCount[validCount] = mAnchorData[i]->getDataCount();
            validRangingData->StandardDeviation[validCount] = mAnchorData[i]->getStandardDeviation();
            mAnchorData[i]->getIMUi(validRangingData->AnchorTagIMU[validCount], 6);
            memset(validRangingData->AnchorName[validCount], 0, 32);
            strncpy(validRangingData->AnchorName[validCount], mAnchorData[i]->AnchorName, 32);
            validRangingData->MapLayer[validCount] = mAnchorData[i]->MapLayer;
            validRangingData->DataTimeInMS[validCount] = mAnchorData[i]->getDataTimeInMS();
            validRangingData->DataTimeStamp[validCount] = mAnchorData[i]->getDataTimeStamp();
            validRangingData->ValidRate[validCount] = mAnchorData[i]->getValidRate();

            // keep as last
            mLastRangingResult->RangingRaw[validCount] = mAnchorData[i]->getRangingRaw();
            mLastRangingResult->RangingFiltered[validCount] = mAnchorData[i]->getRangingFiltered();
            mLastRangingResult->TagVelocity[validCount] = mTagVelocity;
            mLastRangingResult->TagAngle[validCount] = mAnchorData[i]->getAngle();
            mLastRangingResult->TagLevel_x[validCount] = mAnchorData[i]->getLevel_x();
            mLastRangingResult->TagLevel_y[validCount] = mAnchorData[i]->getLevel_y();
            mLastRangingResult->TagLevel_time[validCount] = mAnchorData[i]->getLevel_time();
            mLastRangingResult->TagPollCnt[validCount] = mAnchorData[i]->getTagPollCnt();
            mLastRangingResult->RangingAnchorCnt[validCount] = mAnchorData[i]->getRangingAnchorCnt();
            mLastRangingResult->TagFinalRecvdBm[validCount] = mAnchorData[i]->getTagFinalRecvdBm();
            mLastRangingResult->TagFirstPathdBm[validCount] = mAnchorData[i]->getTagFirstPathdBm();
            mLastRangingResult->AnchorRecvdBm[validCount] = mAnchorData[i]->getAnchorRecvdBm();
            mLastRangingResult->AnchorFirstPathdBm[validCount] = mAnchorData[i]->getAnchorFirstPathdBm();
            mLastRangingResult->LostRate[validCount] = mAnchorData[i]->getLostRate();
            mLastRangingResult->DataRate[validCount] = mAnchorData[i]->getDataRate();
            mLastRangingResult->DataCount[validCount] = mAnchorData[i]->getDataCount();
            mLastRangingResult->StandardDeviation[validCount] = mAnchorData[i]->getStandardDeviation();
            mAnchorData[i]->getIMUi(mLastRangingResult->AnchorTagIMU[validCount], 6);
            memset(mLastRangingResult->AnchorName[validCount], 0, 32);
            strncpy(mLastRangingResult->AnchorName[validCount], mAnchorData[i]->AnchorName, 32);
            mLastRangingResult->DataTimeInMS[validCount] = mAnchorData[i]->getDataTimeInMS();
            mLastRangingResult->DataTimeStamp[validCount] = mAnchorData[i]->getDataTimeStamp();
            mLastRangingResult->ValidRate[validCount] = mAnchorData[i]->getValidRate();


            //printf("\t AnchorName[%s] RangingRaw[%d] RangingFiltered[%d]\n",
            //        validRangingData->AnchorName[validCount], validRangingData->RangingRaw[validCount], validRangingData->RangingFiltered[validCount]);


            validCount++;
        }

        if (validCount >= MAX_RANGING_RESULT)
            break;
    }
    validRangingData->validCount = validCount;
    mLastRangingResult->validCount = validCount;

    //printf("AnchorTagCoupleDataClass::prepareRangingData() mIndex[%d] validCount[%d]\n", mIndex, validCount);

    return validCount;
}


int AnchorTagCoupleDataClass::prepareAllRangingData(RangingResultStruct * validRangingData, int validdatatime)
{
    int validCount = 0;
    validRangingData->validCount = 0;

    // for(int i=0;i<MAX_ANCHOR_NUMBER;i++)
    // {
    for (unsigned int i=0; i< mAnchorData.size(); i++)
    {
        if ( mAnchorData[i]->checkValidRanging(validdatatime) )
        {
            // int ValidRangingCount = mAnchorData[i]->ValidRangingCount(validdatatime);
            mAnchorData[i]->ValidRangingCount(validdatatime);

            validRangingData->RangingRaw[validCount] = mAnchorData[i]->getRangingRaw();
            validRangingData->RangingFiltered[validCount] = mAnchorData[i]->getRangingFiltered();
            validRangingData->TagVelocity[validCount] = mTagVelocity;
            validRangingData->TagAngle[validCount] = mAnchorData[i]->getAngle();
            validRangingData->TagLevel_x[validCount] = mAnchorData[i]->getLevel_x();
            validRangingData->TagLevel_y[validCount] = mAnchorData[i]->getLevel_y();
            validRangingData->TagLevel_time[validCount] = mAnchorData[i]->getLevel_time();
            validRangingData->TagPollCnt[validCount] = mAnchorData[i]->getTagPollCnt();
            validRangingData->RangingAnchorCnt[validCount] = mAnchorData[i]->getRangingAnchorCnt();
            validRangingData->TagFinalRecvdBm[validCount] = mAnchorData[i]->getTagFinalRecvdBm();
            validRangingData->TagFirstPathdBm[validCount] = mAnchorData[i]->getTagFirstPathdBm();
            validRangingData->AnchorRecvdBm[validCount] = mAnchorData[i]->getAnchorRecvdBm();
            validRangingData->AnchorFirstPathdBm[validCount] = mAnchorData[i]->getAnchorFirstPathdBm();
            validRangingData->LostRate[validCount] = mAnchorData[i]->getLostRate();
            validRangingData->DataRate[validCount] = mAnchorData[i]->getDataRate();
            validRangingData->DataCount[validCount] = mAnchorData[i]->getDataCount();
            validRangingData->StandardDeviation[validCount] = mAnchorData[i]->getStandardDeviation();
            mAnchorData[i]->getIMUi(validRangingData->AnchorTagIMU[validCount], 6);
            memset(validRangingData->AnchorName[validCount], 0, 32);
            strncpy(validRangingData->AnchorName[validCount], mAnchorData[i]->AnchorName, 32);
            validRangingData->MapLayer[validCount] = mAnchorData[i]->MapLayer;
            validRangingData->DataTimeInMS[validCount] = mAnchorData[i]->getDataTimeInMS();
            validRangingData->DataTimeStamp[validCount] = mAnchorData[i]->getDataTimeStamp();
            validRangingData->ValidRate[validCount] = mAnchorData[i]->getValidRate();
            validCount++;
        }

        if (validCount >= MAX_RANGING_RESULT)
            break;
    }
    validRangingData->validCount = validCount;
    return validCount;
}
//
// int AnchorTagCoupleDataClass::getIndex()
// {
//     return mIndex;
// }

void AnchorTagCoupleDataClass::setTagName(char * tagname)
{
	strncpy(TagName, tagname, 32);
}

char * AnchorTagCoupleDataClass::getTagName()
{
	return TagName;
}

void AnchorTagCoupleDataClass::setResult(double * result, int valuecount)
{
    for (int i = 0; i < valuecount && i < 3; i++)
        last_results[i] = result[i];
    mRangingCount++;
    if (mRangingCount >= 10000000 )
    {
        resetRangingCount();
    }
}

void AnchorTagCoupleDataClass::setRawResult(double * rawresult, int valuecount)
{
    for (int i = 0; i < valuecount && i < 3; i++)
        raw_results[i] = rawresult[i];
}

// void AnchorTagCoupleDataClass::setIndex(int index)
// {
//     mIndex = index;
// }

void AnchorTagCoupleDataClass::getResult(double * result, int valuecount)
{
    for (int i = 0; i < valuecount && i < 3; i++)
        result[i] = last_results[i];
}

void AnchorTagCoupleDataClass::getRawResult(double * rawresult, int valuecount)
{
    for (int i = 0; i < valuecount && i < 3; i++)
        rawresult[i] = raw_results[i];
}


void AnchorTagCoupleDataClass::getIMU(double * IMU, int valuecount)
{
    //
    // find latest anchor ranging data
    //
    int anchorIdx = getLatestAnchorIndex();
    mAnchorData[anchorIdx]->getIMU(IMU, valuecount);
}


int AnchorTagCoupleDataClass::getRangingCount()
{
    return mRangingCount;
}

int AnchorTagCoupleDataClass::resetRangingCount()
{
    mRangingCount = 0;
    return mRangingCount;
}

double AnchorTagCoupleDataClass::getVelocity()
{
    return mTagVelocity;
}

double AnchorTagCoupleDataClass::getVelocitykm()
{
    return kmVelocity[0];
}

int AnchorTagCoupleDataClass::update(int validdatatime)
{
    //printf ("PosCalculate::update() [%d] [%f.2,%.2f,%.2f] -> ", tagIndexInVector, results[0], results[1], results[2]);
    mCurrentFilter->update(last_results, last_results);

    // for(int i=0;i<MAX_ANCHOR_NUMBER;i++)
    // {
    for (unsigned int i=0; i< mAnchorData.size(); i++)
    {
    	if ( mAnchorData[i]->checkValidRanging(validdatatime) )
    	{
    		mAnchorData[i]->update();
    	}
    }
    return 0;
}

int AnchorTagCoupleDataClass::updateVelocity(int validdatatime)
{
    // for(int i=0;i<MAX_ANCHOR_NUMBER;i++)
    // {
    for (unsigned int i=0; i< mAnchorData.size(); i++)
    {
    	if ( mAnchorData[i]->checkValidRanging(validdatatime) )
        {
            lastVelocity[0] = mAnchorData[i]->getVelocity();
            kalman1dVelocity->update(lastVelocity, kmVelocity);
            if ( kmVelocity[0] <= 0.1 )
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
                        mTagVelocity = kmVelocity[0];
                    }
                    else
                    {
                        // keep same Velocity
                    }
                    // vilslog::printf ("PosCalculate::updateVelocity() [%d] lastVelocity[%f] mTagVelocity[%f] difft[%d]\n",
                    //     i, lastVelocity[0], mTagVelocity, difft);
                }
                //
                // vilslog::printf ("PosCalculate::updateVelocity() [%d] lastVelocity[%f] mTagVelocity[%f] difft\n",
                //     i, lastVelocity[0], mTagVelocity, difft);
            }
            else
            {
                mTagVelocity = kmVelocity[0];
                mVelocityEqualZeroTimeStamp = 0;

                // vilslog::printf ("PosCalculate::updateVelocity() [%d] lastVelocity[%f] mTagVelocity[%f]\n",
                //     i, lastVelocity[0], mTagVelocity);
            }
        }
    }

    return 0;
}

int AnchorTagCoupleDataClass::getLatestAnchorIndex()
{
    time_t latest_time = 0;
    time_t tv = 0;
    int an_idx = 0;
    // for (int i=0;i<MAX_ANCHOR_NUMBER;i++)
    // {
    for (unsigned int i=0; i< mAnchorData.size(); i++)
    {
        if (i == 0)
            latest_time = mAnchorData[i]->getDataTime();
        else
        {
            tv = mAnchorData[i]->getDataTime();
            if ( tv > latest_time)
            {
                latest_time = tv;
                an_idx = i;
            }
        }
        //printf ("getLatestAnchorIndex() [%d] latest_time[%ld] tv[%ld]\n", i, latest_time, tv);
    }
    //printf ("getLatestAnchorIndex() an_idx[%d] latest_time[%ld]\n", an_idx, latest_time);
    return an_idx;
}

RangingResultStruct * AnchorTagCoupleDataClass::getLastRangingData()
{
    return mLastRangingResult;
}

int AnchorTagCoupleDataClass::setArguments(int EnableNoiseMakup,
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
    float WeightMovingAverageValue3)
{
    int ret = 0;
    mEnableRangingReliability = EnableRangingReliability;
    mRangingReliabilityMaxDistance = RangingReliabilityMaxDistance;
    mRangingReliabilityMinCount = RangingReliabilityMinCount;
    mRangingReliabilityAverageDiff = RangingReliabilityAverageDiff;
    mCalculationMethod = CalculationMethod;

    if (mCalculationMethod == 0)
        mCurrentFilter = mKalman3DFilter;
    else
    if (mCalculationMethod == 1)
        mCurrentFilter = mSmoothingFilter;
    else
    if (mCalculationMethod == 2)
    {
        WeightedMovingAverageFilter * wmaFilter = (WeightedMovingAverageFilter *)mWeightedMovingAverageFilter;
        wmaFilter->setArguments(WeightMovingAverageValue1, WeightMovingAverageValue2, WeightMovingAverageValue3);

        mCurrentFilter = mWeightedMovingAverageFilter;
    }

    // for(int i=0;i<MAX_ANCHOR_NUMBER;i++)
    // {
    for (unsigned int i=0; i< mAnchorData.size(); i++)
    {
        if (strlen(mAnchorData[i]->AnchorName) != 0)
        {
            mAnchorData[i]->setArguments(EnableNoiseMakup,
                NoiseMakupRatio,
                NoiseMakupWeight,
                EnableKalman1D);
        }
    }
    return ret;
}
