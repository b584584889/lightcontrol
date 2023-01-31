/*
 *  PosCalculateService.cpp
 *
 *  Created on: 2018/10/23
 *      Author: ikki
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <iostream>

#include "../PosCalculate/algorithm.h"
#include "PosCalculateService.h"

using namespace std;

PosCalculateService::PosCalculateService(wqueue<DataItem*> & JobQueue, wqueue<WorkItem*> & MySQLJobQueue, wqueue<WorkItem*> & MySQLTSJobQueue, wqueue<DataItem*> & CheckPointJobQueue, wqueue<WorkItem*> & nodePositionQueue, wqueue<DataItem*> & DeviceEventJobQueue, DeviceManager * pDevManager, DBManager * dbManager)
    : mJobQueue(JobQueue), mMySQLJobQueue(MySQLJobQueue), mMySQLTSJobQueue(MySQLTSJobQueue), mCheckPointJobQueue(CheckPointJobQueue), mNodePositionQueue(nodePositionQueue), mDeviceEventJobQueue(DeviceEventJobQueue), mDeviceManager(pDevManager), mDbManager(dbManager)
{
    mSmoothingResult = NULL;
    mPosCal = NULL;

    mUpdateData = false;
    mUpdateProject = false;
}

bool PosCalculateService::status()
{
    return mInitial;
}

std::string PosCalculateService::statusString()
{
    return mStatus;
}

void PosCalculateService::updateProject()
{
    mUpdateProject = true;
}

void * PosCalculateService::run()
{
    while(1)
    {
        try
        {
            runloop();
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("PosCalculateService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "PosCalculateService()::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void PosCalculateService::runloop()
{
    mInitial = false;

    mSmoothingResult = new SmoothingResult();
    mPosCal = new PosCalculate(mDeviceManager, mSmoothingResult, mCheckPointJobQueue, mDeviceEventJobQueue, mDbManager);
    mPowerTagCal = new PowerTagCalculate(mDeviceManager, mDbManager);

    loadSectionList();

    mInitial = true;
    mStatus = "PosCalculateService()::run() Initial successfully!";
    vilslog::printf ("PosCalculateService()::run() Initial successfully!\n");

    waitloop();

}

int PosCalculateService::solveAllTagPosition(int projectid)
{
    double results[3] = {0.0, 0.0, 0.0};
    double results_smooth[3] = {0.0, 0.0, 0.0};
    double IMU[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    char tagid[64];
    int success_count = 0;

    if (!mInitial)
        return 0;

    VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);

    int tags = mPosCal->getRangingTagCount(projectid);

    for(int i=0;i<tags;i++)
    {
        memset(tagid, 0, 64);
        bool bExisted = mPosCal->getTagInfo(projectid, i, tagid, IMU);

        // snprintf(tagid, 64, "%s", mPosCal->getTagId(projectid, i));
        if (strlen(tagid) == 0 || !bExisted )
        {
            //printf("run_state() failed! tagid=[%s]\n", tagid);
            continue;
        }

        //
        // check if in powerTag
        //
        if (check_powertag_test(projectid, tagid))
        {
            //vilslog::printf("run_state() check_powertag_test tagid=[%s]\n", tagid);
            continue;
        }

        // return 0: found 0D Section
        // return 1: found 1D Section
        // return 2: found 2D/3D position
        // return -1: not found
        int ret = mPosCal->rangingResult(projectid, tagid, results,
            vilssysConfig->EnableIMUVelocity,
            vilssysConfig->IMUVelocityOffset,
            vilssysConfig->SteadyStateOffset,
            vilssysConfig->EnableGDOP,
            vilssysConfig->GDOPThreshold,
            vilssysConfig->ValidDataTime,
            vilssysConfig->EnableFixHeight);

        if ( ret < 0 )
        {
            continue;
        }

        //==========================
        NodeStruct * tagNode = mDeviceManager->get_Tag_node(projectid, tagid);
        if (tagNode == NULL)
        {
            continue;
        }

        // char * macaddress = mDeviceManager->getTagMacaddress(projectid, tagid);
        bool validUpdate = checkTagUpdateValidTime(projectid, tagid, 300);
        if (validUpdate)
        {
            std::string status = getTagStatus(IMU);
            mDeviceManager->setTagInfoActivity(projectid, tagNode->macaddress, status);

           // vilslog::printf ("PosCalculateService::solveAllTagPosition() tagid[%s] validUpdate[%d] status[%s]\n", tagid, validUpdate, status.c_str());

            // const char * areaid = mDeviceManager->getTagAreaId(projectid, tagid);

            for (std::list<SoccerRecordingStruct*>::iterator record = mSoccerRecordingList.begin(); record != mSoccerRecordingList.end(); record++)
            {
                if (((*record)->starttime != 0) && ((*record)->endtime == 0))
                {

                    WorkItem * item1 = new WorkItem("updateM", projectid, tagid, results[0], results[1], results[2], IMU[0], IMU[1], IMU[2], IMU[3], IMU[4], IMU[5], tagNode->AreaId);
                    item1->setRecorderID((*record)->recordnumber);
                    item1->setMaingroups("team");
                    item1->setRole(tagNode->tafid);// Tag role 0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師
                    item1->setMacaddress(tagNode->macaddress);
                    item1->setMaplayer(tagNode->maplayer);
                    item1->setMaingroupid((*record)->maingroupid);
                    item1->setDateTime(getCurrentDateTimeInMilliSec());
                    item1->setStatus(status);
                    mMySQLTSJobQueue.add(item1);
                }
            }

            double rawresult[3];
            mPosCal->getRawResult(projectid, tagid, rawresult);

            double diff_dist = TwoPointDistance(rawresult, results);
           update_diffPositionList(projectid, tagNode->macaddress, diff_dist, rawresult);
        }
        //==========================

        mSmoothingResult->updatResult(projectid, tagid, results);
        mSmoothingResult->getResult(projectid, tagid, results_smooth);
        mDeviceManager->setTagSmoothingPos(projectid, tagid, results_smooth[0], results_smooth[1], results_smooth[2]);

        // if ( strcmp(tagid, "Tag0302") == 0 )
        //     vilslog::printf("solveAllTagPosition() TagId[%s] results[%d, %d]\n", tagid, (int)results[0], (int)results[1]);

        if (ret == 1 || ret == 2)
        {
            // 1D and 2D/3D
            //NodeStruct * node = mDeviceManager->get_Tag_node(projectid, tagid);
            //mPosCal->checkTagArea(projectid, node);
        }
        else
        if (ret == 0)
        {
            // 0D, direct move to position
            mDeviceManager->setTagSmoothingPos(projectid, tagid, results[0], results[1], results[2]);
        }

        WorkItem * item1 = new WorkItem("tagposinfo", projectid, tagid, results[0], results[1], results[2], "");
        mNodePositionQueue.add(item1);

        // int userid = mDeviceManager->getTaguserid(projectid, macaddress);
        if (tagNode->userid > 0 )
        {
            WorkItem * witem = new WorkItem("updateUserPoistion", projectid);
            witem->setUserId(tagNode->userid);
            witem->setPosition(results[0], results[1], results[2]);
            mMySQLJobQueue.add(witem);
        }

        success_count++;

        //printf("%s solveAllTagPosition() %s(%d): (%d, %d, %d)\n",
        //    currentDateTime().c_str(), tagid, Tag_idx, (int)results[0], (int)results[1], (int)results[2]);

    }

    if (mSmoothingResult != NULL)
    {
        mSmoothingResult->setArguments(projectid, vilssysConfig->SmoothingResultOffset,
            vilssysConfig->SmoothingResultStep,
            vilssysConfig->SmoothingResultFirstStateOffsetRatio,
            vilssysConfig->SmoothingResultFirstStateRunTime,
            vilssysConfig->SmoothingResultSecondStateOffsetRatio,
            vilssysConfig->SmoothingResultSecondStateRunTime);

        mSmoothingResult->updatePosition();
    }

    //printf("%s solveAllTagPosition() end\n", currentDateTime().c_str());

    return success_count;
}

int PosCalculateService::solveAllAoAPosition(int projectid)
{
    int success_count = 0;
    for (std::list<PowerTagAoAInfoStruct*>::iterator queue = mPowerTagAoAInfoList.begin(); queue != mPowerTagAoAInfoList.end(); queue++)
    {
        if ( (*queue)->projectid == projectid )
        {
            for (std::list<PowerTagInfoStruct*>::iterator queue = mPowerTagInfoList.begin(); queue != mPowerTagInfoList.end(); queue++)
            {
                if ( (*queue)->projectid == projectid )
                {
                    mPowerTagCal->setAoATag(projectid, (*queue)->tagId);
                }
            }

            success_count += mPowerTagCal->aoaResult(projectid, (*queue)->aoaId);

            time_t diff_t = getCurrentTimeInSec() - (*queue)->issuetime;
            if ( (int)diff_t >= 60)
            {
                vilslog::printf(" PosCalculateService::solveAllAoAPosition() mPowerTagAoAInfoList.remove(%s)\n", (*queue)->aoaId.c_str());

                PowerTagAoAInfoStruct * target = (*queue);
                mPowerTagAoAInfoList.remove(target);
                delete target;
                break;
            }
        }
    }

    return success_count;
}

int PosCalculateService::solveFixPosition(int projectid)
{
    int success_count = 0;
    time_t cut_t = getCurrentTimeInSec();
    char tagid[64];
    double results[3] = {0.0, 0.0, 0.0};
    double IMU[6] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};

    for (std::list<FixPositionInfoStruct*>::iterator info = mFixPositionInfoList.begin(); info != mFixPositionInfoList.end(); info++)
    {
        if ( (*info)->projectid == projectid )
        {
            time_t diff_t = cut_t - (*info)->issuetime;
            if ( (int)diff_t <= 20)
            {
                snprintf(tagid, 64, "%s", mDeviceManager->getTagId(projectid, (*info)->macaddress.c_str()));
                if (strlen(tagid) == 0)
                    continue;

                results[0] = (*info)->posX;
                results[1] = (*info)->posY;
                results[2] = (*info)->posZ;

                // vilslog::printf("PosCalculate::solveFixPosition() tagid[%s] projectid[%d] results[%d, %d, %d]\n",
                //                    tagid, projectid, (int)results[0], (int)results[1], (int)results[2]);

                mPosCal->setRawResult(projectid, tagid, results);

                //==========================
                bool validUpdate = checkTagUpdateValidTime(projectid, tagid, 1000);
                if (validUpdate)
                {
        //            vilslog::printf ("PosCalculateService::solveFixPosition() tagid[%s] validUpdate[%d]\n", tagid, validUpdate);
                    const char * areaid = mDeviceManager->getTagAreaId(projectid, tagid);
                    WorkItem * item1 = new WorkItem("updateM", projectid, tagid, results[0], results[1], results[2], IMU[0], IMU[1], IMU[2], IMU[3], IMU[4], IMU[5], areaid);
                    item1->setMacaddress(mDeviceManager->getTagMacaddress(projectid, tagid));
                    item1->setMaplayer(mDeviceManager->getTagMapLayerById(projectid, tagid));
                    mMySQLTSJobQueue.add(item1);
                }
                //==========================

                mSmoothingResult->updatResult(projectid, tagid, results);
                mSmoothingResult->getResult(projectid, tagid, results);
                mDeviceManager->setTagSmoothingPos(projectid, tagid, results[0], results[1], results[2]);

                WorkItem * item1 = new WorkItem("tagposinfo", projectid, tagid, results[0], results[1], results[2], "");
                mNodePositionQueue.add(item1);
            }
        }
    }

    return success_count;
}

// int PosCalculateService::solveAllTagArea(int projectid)
// {
//     int success_count = 0;
//     std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);
//
//     // double results[3];
//     for (std::list<NodeStruct*>::iterator node = tagNodeList->begin(); node != tagNodeList->end(); node++)
//     {
//         if ( (*node)->alive > 0 )
//         {
//             // if ( (*node)->AreaType == 1 ) // 0D using for event type 0:normal 1:0D room  2:danger area
//             // {
//             //     continue;
//             // }
//
//             //========================================
//             // mDeviceManager->getTagSmoothingCoord(projectid, (*node)->nodeid, results);
//             // mPosCal->checkTagArea(projectid, (*node)->nodeid, results, (*node)->AreaId, (*node)->exclude_rollcall_AreaId);
//             //========================================
//             mPosCal->checkTagArea(projectid, (*node));
//             //========================================
//             success_count++;
//         }
//     }
//
//     return success_count;
// }

int PosCalculateService::solveLocatorArea(int projectid)
{
    int success_count = 0;
    std::list<NodeStruct*> * LocatorNodeList = mDeviceManager->get_Locator_list(projectid);

    // vilslog::printf ("PosCalculateService::solveLocatorArea() projectid[%d]\n", projectid);

    for (std::list<NodeStruct*>::iterator node = LocatorNodeList->begin(); node != LocatorNodeList->end(); node++)
    {
        // vilslog::printf ("PosCalculateService::solveLocatorArea() nodeid[%s]\n", (*node)->nodeid);

        mPosCal->checkLocatorArea(projectid, (*node));
        success_count++;
    }
    return success_count;
}

// bool PosCalculateService::getSmoothingResult(int Tag_idx, int * results)
// {
//     if (mSmoothingResult != NULL)
//     {
//         mSmoothingResult->getResult(Tag_idx, results);
//     }
//     return true;
// }

std::string PosCalculateService::getRangingString(int projectid, int validdatatime)
{
    if (!mInitial)
        return std::string("");

    return mPosCal->getRangingString(projectid, validdatatime);
}

std::string PosCalculateService::getRangingStringEx(int projectid, int validdatatime)
{
    if (!mInitial)
        return std::string("");

    return mPosCal->getRangingStringEx(projectid, validdatatime);
}

std::string PosCalculateService::getRangingDiagnosisString(int projectid, const char * tag_macaddress, int validdatatime)
{
    if (!mInitial)
        return std::string("");

    return mPosCal->getRangingDiagnosisString(projectid, tag_macaddress, validdatatime);
}

void PosCalculateService::setDumpAoACirclePoint()
{
    // vilslog::printf("PosCalculateService::setDumpAoACirclePoint()\n");
    bEnableDumpAoACirclePoint = true;
}

std::string PosCalculateService::getAoARangingCloudPoint(int projectid)
{
    if (!mInitial)
        return std::string("");

    return mPosCal->getAoARangingCloudPoint(projectid);
}

std::string PosCalculateService::getAoAAngleVectorCloudPoint(int projectid)
{
    if (!mInitial)
        return std::string("");

    return mPosCal->getAoAAngleVectorCloudPoint(projectid);
}

std::string PosCalculateService::getAoAAngleCloudPoint(int projectid)
{
    if (!mInitial)
        return std::string("");

    return mPosCal->getAoAAngleCloudPoint(projectid);
}

std::string PosCalculateService::getUnknownPositionTagString(int projectid)
{
    if (!mInitial)
        return std::string("");

    std::string retString;
    retString.append("[");
    int count = 0;
    for (std::list<FixPositionInfoStruct *>::iterator info = mDiffPositionInfoList.begin(); info != mDiffPositionInfoList.end(); info++)
    {
        if ( (*info)->projectid == projectid )
        {
            if (count != 0)
                retString.append(",");

            retString.append("{\"macaddress\":\"");
            retString.append((*info)->macaddress);
            retString.append("\",\"pos\":\"");
            retString.append(std::to_string((*info)->posX));
            retString.append(",");
            retString.append(std::to_string((*info)->posY));
            retString.append(",");
            retString.append(std::to_string((*info)->posZ));
            retString.append("\"}");
            count++;
        }
    }
    retString.append("]");

    return retString;
}
// int PosCalculateService::pushRangingDataToCheckPoint()
// {
//     //TODO
//
//
//     return 0;
// }

void PosCalculateService::waitloop()
{
    long long start1 = getSystemTime();
    long long serviceRunTime = 0;
    long long solveTagTime = 0;
    // long long solveTagAreaTime = 0;
    long long setArgumentsTime = 0;
    long long debugTime = 0;
    long long solveTagMAXTime = 0;
    long long solveTagMINTime = 99999;
    long long solveTagTOTALTime = 0;
    long long updateTagTime = 0;
    long long updatePrjIdTime = 0;
    long long solveLocatorAreaTime = 50000;
    int solveCount = 0;
    int solveTagCount = 0;
    bEnableDumpAoACirclePoint = false;

    mPrjidList = mDbManager->getAllProjectConfig();

    while ( 1 )
    {
        long long end1 = getSystemTime();
        long long difft = end1 - start1;
        //printf("wait_loop() time[%lld ms]\n", difft);

        solveTagTime += difft;
        // solveTagAreaTime += difft;
        setArgumentsTime += difft;
        debugTime += difft;
        serviceRunTime += difft;
        updatePrjIdTime += difft;
        solveLocatorAreaTime += difft;
        updateTagTime += difft;

        //if (solveTagTime >= m_systemConfig.SolveTagPosition_Time) // 100 miliseconds
        if (solveTagTime >= 100) // 100 miliseconds
        {
            //long long solveStart = getSystemTime();
            //printf("wait_loop() solveAllTagPosition() solveTagTime[%lld ms] start...\n", solveTagTime);

            mPosCal->setDumpAoACirclePoint(bEnableDumpAoACirclePoint);

            int tagSuccessCount = 0;
            int aoaSuccessCount = 0;
            int fixSuccessCount = 0;
            for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
            {
                int projectid = (*prj);
                tagSuccessCount += solveAllTagPosition(projectid);

                aoaSuccessCount += solveAllAoAPosition(projectid);

                fixSuccessCount += solveFixPosition(projectid);
                //VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);
                //mPosCal->update(projectid, vilssysConfig->ValidDataTime);
            }
            bEnableDumpAoACirclePoint = false;

            long long solveEnd = getSystemTime();
            //printf("PosCalculateService::wait_loop() solveAllTagPosition()[%lld ms]\n", solveEnd - solveStart);

            //================
            long long solveTime = solveEnd - end1;
            if (solveTagMAXTime < solveTime)
                solveTagMAXTime = solveTime;
            if (solveTagMINTime > solveTime)
                solveTagMINTime = solveTime;

            solveTagTOTALTime += solveTime;
            solveTagCount += tagSuccessCount;
            solveCount++;
            //================

            solveTagTime = 0;
        }

        if (updateTagTime >= 1000) // 1000 miliseconds
        {
            for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
            {
                int projectid = (*prj);
                VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);
                mPosCal->update(projectid, vilssysConfig->ValidDataTime);
            }
            updateTagTime = 0;
        }

        // if (solveTagAreaTime >= 1000 && serviceRunTime >= 30000)
        // {
        //     // for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
        //     // {
        //     //     int projectid = (*prj);
        //     //     solveAllTagArea(projectid);
        //     // }
        //
        //     solveTagAreaTime = 0;
        // }

        if ( solveLocatorAreaTime >= 60000 ) // 1 minites
        {
            for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
            {
                int projectid = (*prj);
                solveLocatorArea(projectid);
            }
            solveLocatorAreaTime = 0;
        }

        if (setArgumentsTime >= 1000)
        {
            for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
            {
                int projectid = (*prj);
                VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);

                mPosCal->setArguments(projectid, vilssysConfig->EnableNoiseMakup,
                    vilssysConfig->NoiseMakupRatio,
                    vilssysConfig->NoiseMakupWeight,
                    vilssysConfig->EnableKalman1D,
                    vilssysConfig->EnableRangingReliability,
                    vilssysConfig->RangingReliabilityMaxDistance,
                    vilssysConfig->RangingReliabilityMinCount,
                    vilssysConfig->RangingReliabilityAverageDiff,
                    vilssysConfig->EnableAnchor1DMapping,
                    vilssysConfig->EnableOneAnchorCalculation,
                    vilssysConfig->CalculationMethod,
                    vilssysConfig->WeightMovingAverageValue1,
                    vilssysConfig->WeightMovingAverageValue2,
                    vilssysConfig->WeightMovingAverageValue3,
                    vilssysConfig->StayInWallTimeout);
            }

            setArgumentsTime = 0;
        }

        if (debugTime >= 3000)
        {
            debugTime = 0;
        }

        //printf("wait_loop() solveAllTagPosition() solveTagTime[%lld ms] solveCount[%d] end1[%lld]\n", solveTagTime, solveCount, end1);

        if (solveCount >= 100)
        {
            // int solveAverageTime = (int)solveTagTOTALTime / solveCount;
            // int solveTagAverageCount = solveTagCount / solveCount;
            //
            // vilslog::printf("Solve Tag Position:TagCount[%d] solveCount[%d] MaxTime[%d] MinTime[%d] AverageTime[%d]\n",
            //       solveTagAverageCount, solveCount, (int)solveTagMAXTime, (int)solveTagMINTime, (int)solveAverageTime);
            //
            solveTagMAXTime = 0;
            solveTagMINTime = 99999;
            solveTagTOTALTime = 0;
            solveTagCount = 0;
            solveCount = 0;
        }

        if (updatePrjIdTime > 3600000 || mUpdateProject)
        {
            mPrjidList->clear();
            delete mPrjidList;
            mPrjidList = mDbManager->getAllProjectConfig();

            updatePrjIdTime = 0;
            mUpdateProject = false;
        }

        checkRangingQueue();

        if (mUpdateData)
        {
            mPosCal->reloadAreaList();
            mUpdateData = false;
        }

        DataItem * item = mJobQueue.removetimed(100);
        if (item == NULL)
        {
            start1 = end1;
            usleep(10000);
            continue;
        }

        // ProcessCount++;
        // if ((ProcessCount % 100) == 0)
        // {
        //     vilslog::out <<currentDateTimeForSQL() << " PosCalculateService::waitloop() ProcessCount:"<<ProcessCount<<" mJobQueue.size()="<<mJobQueue.size()<<endl;
        //     vilslog::flush() ;
        // }

        //printf ("PosCalculateService()::waitloop() type[%s]\n", item->getType());

        // if ( strcmp(item->getType(), "ranging") == 0 )
        // {
        //     do_ranging(item);
        // }
        // else
        if ( strcmp(item->getType(), "rangingdiag") == 0 )
        {
            do_rangingdiag(item);
            //printf ("PosCalculateService()::waitloop() type[%s] finished!\n", item->getType());
        }
        else
        if ( strcmp(item->getType(), "rangingdiag_1") == 0 )
        {
            do_rangingdiag_1(item);
            //printf ("PosCalculateService()::waitloop() type[%s] finished!\n", item->getType());
        }
        else
        if ( strcmp(item->getType(), "loraranging") == 0 )
        {
            //printf ("PosCalculateService()::waitloop() type[%s]\n", item->getType());
            do_loraranging(item);
        }
        else
        if ( strcmp(item->getType(), "lorarangingcompress") == 0 )
        {
            //printf ("PosCalculateService()::waitloop() type[%s]\n", item->getType());
            do_lorarangingcompress(item);
        }
        else
        if ( strcmp(item->getType(), "lorarangingcompressMultiDistance") == 0 )
        {
            //printf ("PosCalculateService()::waitloop() type[%s]\n", item->getType());
            do_lorarangingcompressMultiDistance(item);
        }
        else
        if ( strcmp(item->getType(), "lorarangingcompressaoa") == 0 )
        {
            //printf ("PosCalculateService()::waitloop() type[%s]\n", item->getType());
            do_lorarangingcompressaoa(item);
        }
        else
        if ( strcmp(item->getType(), "updatlocatorposition") == 0 )
        {
            // std::string macaddress = item->getmacaddress();
            // std::string Message = item->getMessage();
            //
            // vilslog::printf ("PosCalculateService()::waitloop() type[%s] macaddress[%s] Message[%s]\n",
            //     item->getType(), macaddress.c_str(), Message.c_str());

            do_updatlocatorposition(item);
        }
        else
        if ( strcmp(item->getType(), "updatesection") == 0 )
        {
            vilslog::printf ("PosCalculateService()::waitloop() type[%s]\n", item->getType());
            loadSectionList();
        }
        else
        if ( strcmp(item->getType(), "updatearea") == 0 )
        {
            //printf ("PosCalculateService()::waitloop() type[%s]\n", item->getType());
            //mPosCal->updateAreaList();
            mUpdateData = true;
        }
        else
        if ( strcmp(item->getType(), "startdumprangingresult") == 0 )
        {
            vilslog::printf ("PosCalculateService()::waitloop() type[%s]\n", item->getType());
            mPosCal->start_dump_ranging_result(item->getTimeout());
        }
        else
        if ( strcmp(item->getType(), "startrecordsoccer") == 0 )
        {
            int maingroupid = item->getProjectID();
            std::string recorderID = item->getRecorderID();

            vilslog::printf ("PosCalculateService()::waitloop() type[%s] maingroupid[%d] recorderID[%s]\n",
                item->getType(), maingroupid, recorderID.c_str());

            SoccerRecordingStruct * record = getCurrentSoccerRecording(maingroupid);

            if (record == NULL)
            {
                record = new SoccerRecordingStruct();
                mSoccerRecordingList.push_back(record);
            }

            record->maingroupid = maingroupid;
            record->recordnumber = recorderID;
            record->starttime = getCurrentTimeInSec();
            record->endtime = 0;

        }
        else
        if ( strcmp(item->getType(), "stoprecordsoccer") == 0 )
        {
            int maingroupid = item->getProjectID();
            std::string recorderID = item->getRecorderID();

            vilslog::printf ("PosCalculateService()::waitloop() type[%s] maingroupid[%d] recorderID[%s]\n",
                item->getType(), maingroupid, recorderID.c_str());

            SoccerRecordingStruct * record = getCurrentSoccerRecording(maingroupid);
            if (record != NULL)
            {
                if (record->endtime == 0)
                {
                    record->endtime = getCurrentTimeInSec();
                }
            }
            WorkItem * item1 = new WorkItem("dbCommit", 1);
            mMySQLTSJobQueue.add(item1);
        }
        else
        if ( strcmp(item->getType(), "PrintAoACirclePoint") == 0 )
        {
            mPosCal->setPrintAoACirclePoint();
        }
        else
        if ( strcmp(item->getType(), "PowerTagCalculateInfo") == 0 )
        {
            do_PowerTagCalculateInfo(item);
        }
        else
        if ( strcmp(item->getType(), "FixPosition") == 0 )
        {
            do_FixPosition(item);
        }
        else
        if ( strcmp(item->getType(), "FixDevice") == 0 )
        {
            do_FixDevice(item);
        }
        else
        if ( strcmp(item->getType(), "AOAfence") == 0 )
        {
            do_AOAfence(item);
        }
        // else
        // if ( strcmp(item->getType(), "DumpAoACirclePoint") == 0 )
        // {
        //     mPosCal->setDumpAoACirclePoint();
        // }
        // else
        // if ( strcmp(item->getType(), "updatesystemconfig") == 0 )
        // {
        //     vilslog::printf ("PosCalculateService()::waitloop() type[%s]\n", item->getType());
        //     read_sys_config(mVILSsysConfig);
        // }
        // else
        // if ( strcmp(item->getType(), "updatealgorithmconfig") == 0 )
        // {
        //     vilslog::printf ("PosCalculateService()::waitloop() type[%s]\n", item->getType());
        //     read_sys_config(mVILSsysConfig);
        // }
        delete item;

        start1 = end1;

        //usleep(10000);
        //printf ("PosCalculateService()::waitloop() finished!\n");
    }
}

void PosCalculateService::checkRangingQueue()
{
    time_t issuetime = getCurrentTimeInMilliSec();
    for (std::list<TagRangingQueueStruct*>::iterator queue = mTagRangingQueueList.begin(); queue != mTagRangingQueueList.end(); queue++)
    {
        //vilslog::printf ("PosCalculateService()::checkRangingQueue() tagId[%s] size[%d]\n", (*queue)->tagId.c_str(), (*queue)->rangingdataQueue.size());

        TagRangingTimeStruct * rangingData = (*queue)->rangingdataQueue.front();
        int projectid = (*queue)->projectid;
        if (rangingData != NULL )
        {
            if (issuetime >= rangingData->issuetime)
            {
                vilslog::printf ("PosCalculateService()::checkRangingQueue() tagId[%s] size[%d] ranging[%d] LoRAMacaddress[%s]\n",
                    (*queue)->tagId.c_str(), (*queue)->rangingdataQueue.size(), rangingData->ranging, rangingData->LoRAMacaddress.c_str());

                mPosCal->setLoRARanging(projectid, rangingData->LoRAMacaddress.c_str(), (*queue)->tagMacaddress.c_str(), rangingData->ranging, rangingData->velocity, rangingData->rssi, rangingData->timestamp.c_str());

                char Locator_id[64];
                sprintf(Locator_id, "%s", mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, rangingData->LoRAMacaddress.c_str()));

                // DataItem * sendItemckp = new DataItem("RangingFiltered", projectid, (*queue)->tagId.c_str(), Locator_id, rangingData->ranging, 1, rangingData->timestamp.c_str());
                // mCheckPointJobQueue.add(sendItemckp);
                //
                // DataItem * sendItemdevEvent = new DataItem("RangingFiltered", projectid, (*queue)->tagId.c_str(), Locator_id, rangingData->ranging, 1, rangingData->timestamp.c_str());
                // mDeviceEventJobQueue.add(sendItemdevEvent);

                (*queue)->rangingdataQueue.remove();
                delete rangingData;
            }
        }
    }
}

int PosCalculateService::do_rangingdiag(DataItem * item)
{
    int IMU[6];
    item->getTagIMU(IMU);
    int projectid = item->getProjectID();
    int Distance = item->getDistance();

    VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);

    if ( Distance <= 0 || Distance > vilssysConfig->MaxRangindDistance)
    {
        vilslog::printf ("PosCalculateService::do_rangingdiag() Ignore, Distance[%d]\n", Distance);
        return 0;
    }

    //vilslog::printf ("PosCalculateService::do_rangingdiag() TagIdx[%d] IPAddress[%s]\n", item->getTagIdx(), item->getIPAddress());

    int tagIdx = item->getTagIdx();
    mDeviceManager->setTagInfoIP(projectid, mDeviceManager->getTagId(projectid, tagIdx), item->getIPAddress());

    mPosCal->setRangingDiag(projectid, item->getAnchorIdx(), item->getTagIdx(), item->getRangingIdx(), item->getDistance(), IMU,
             item->getTagPollCnt(), item->getRangingAnchorCnt(), item->getTagFinalRecvdBm(), item->getTagFirstPathdBm(),
             item->getAnchorRecvdBm(), item->getAnchorFirstPathdBm(), item->getRangingCount(), item->getRangingFailCount(),
             item->getDataRate());
    return 1;
}

int PosCalculateService::do_rangingdiag_1(DataItem * item)
{
    int IMU[6];
    int projectid = item->getProjectID();
    int Distance = item->getDistance();

    item->getTagIMU(IMU);

    VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);

    if ( Distance <= 0 || Distance > vilssysConfig->MaxRangindDistance)
    {
        //vilslog::printf ("PosCalculateService::do_rangingdiag_1() Ignore, Distance[%d]\n", Distance);
        return 0;
    }

    //vilslog::printf ("PosCalculateService::do_rangingdiag_1() TagMac[%s] IPAddress[%s]\n", item->getTagMac().c_str(), item->getIPAddress());

    mDeviceManager->setTagInfoIP(projectid, item->getTagMac().c_str(), item->getIPAddress());

    mPosCal->setRangingDiag(projectid, item->getAnchorMac(), item->getTagMac(), item->getRangingIdx(), item->getDistance(), IMU,
             item->getTagPollCnt(), item->getRangingAnchorCnt(), item->getTagFinalRecvdBm(), item->getTagFirstPathdBm(),
             item->getAnchorRecvdBm(), item->getAnchorFirstPathdBm(), item->getRangingCount(), item->getRangingFailCount(),
             item->getDataRate());
    return 1;
}

int PosCalculateService::do_loraranging(DataItem * item)
{
    int Distance = item->getDistance();
    int projectid = item->getProjectID();

    VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);
    if ( Distance <= 0 || Distance > vilssysConfig->MaxRangindDistance)
    {
        //vilslog::printf ("PosCalculateService::do_loraranging() Ignore, Distance[%d]\n", Distance);
        return 0;
    }

    // vilslog::printf ("PosCalculateService::do_loraranging() LoRAMacaddress[%s] macaddress[%s] Distance[%d]\n", item->getLoRAMacaddress(), item->getmacaddress(), Distance);

    // int imu[6]={0,0,0,0,0,0};
    mPosCal->setLoRARanging(projectid, item->getLoRAMacaddress(), item->getmacaddress(), Distance, 0, item->getRSSI(), item->getTimestamp());

    char Tag_id[64];
    char Locator_id[64];
    sprintf(Tag_id, "%s", mDeviceManager->getTagId(projectid, item->getmacaddress()));
    sprintf(Locator_id, "%s", mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, item->getLoRAMacaddress()));

    // DataItem * sendItemckp = new DataItem("RangingFiltered", projectid, Tag_id, Locator_id, Distance, 1, item->getTimestamp());
    // mCheckPointJobQueue.add(sendItemckp);
    //
    // DataItem * sendItemdevEvent = new DataItem("RangingFiltered", projectid, Tag_id, Locator_id, Distance, 1, item->getTimestamp());
    // mDeviceEventJobQueue.add(sendItemdevEvent);

    return 1;
}

int PosCalculateService::do_lorarangingcompress(DataItem * item)
{
    int Distance = item->getDistance();
    int projectid = item->getProjectID();

    VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);
    if ( Distance <= 0 || Distance > vilssysConfig->MaxRangindDistance)
    {
        vilslog::printf ("PosCalculateService::do_lorarangingcompress() Ignore, LoRAMacaddress[%s] tag[%s] Distance[%d] velocity[%d]\n",
            item->getLoRAMacaddress(), item->getmacaddress(), Distance, item->getVelocity());
        return 0;
    }

    // vilslog::printf ("PosCalculateService::do_lorarangingcompress() LoRAMacaddress[%s] tag[%s] Distance[%d] velocity[%d]\n",
    //     item->getLoRAMacaddress(), item->getmacaddress(), Distance, item->getVelocity());

    mPosCal->setLoRARanging(projectid, item->getLoRAMacaddress(), item->getmacaddress(), Distance, item->getVelocity(), item->getRSSI(), item->getTimestamp());

    char Tag_id[64];
    char Locator_id[64];
    sprintf(Tag_id, "%s", mDeviceManager->getTagId(projectid, item->getmacaddress()));
    sprintf(Locator_id, "%s", mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, item->getLoRAMacaddress()));

    // DataItem * sendItemckp = new DataItem("RangingFiltered", projectid, Tag_id, Locator_id, Distance, 1, item->getTimestamp());
    // mCheckPointJobQueue.add(sendItemckp);
    //
    // DataItem * sendItemdevEvent = new DataItem("RangingFiltered", projectid, Tag_id, Locator_id, Distance, 1, item->getTimestamp());
    // mDeviceEventJobQueue.add(sendItemdevEvent);

    return 1;
}

int PosCalculateService::do_lorarangingcompressMultiDistance(DataItem * item)
{
    // vilslog::printf ("PosCalculateService::do_lorarangingcompressMultiDistance() LoRAMacaddress[%s] tag[%s] distInterval[%d] velocity[%d]\n",
    //     item->getLoRAMacaddress(), item->getmacaddress(), item->getDistInterval(), item->getVelocity());
    int projectid = item->getProjectID();

    TagRangingQueueStruct * tagRangingQueue = getTagRangingQueue(projectid, item->getmacaddress());
    VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);

    int index = 0 ;
    for (std::list<int>::iterator i = item->m_distList.begin(); i != item->m_distList.end(); i++)
    {
        //vilslog::printf("[%d]",(*i));

        int Distance = (*i);
        if (Distance <= 0 || Distance > vilssysConfig->MaxRangindDistance)
        {
            index++;
            continue;
        }

        TagRangingTimeStruct * rangingTime = new TagRangingTimeStruct();
        rangingTime->ranging = Distance;
        rangingTime->velocity = item->getVelocity();
        rangingTime->rssi = item->getRSSI();
        rangingTime->timestamp = item->getTimestamp();
        rangingTime->LoRAMacaddress = item->getLoRAMacaddress();
        rangingTime->issuetime = getCurrentTimeInMilliSec() + (index * item->getDistInterval());
        tagRangingQueue->rangingdataQueue.add(rangingTime);
        index++;
    }
  //  vilslog::printf("\n");

    return 0;
}

int PosCalculateService::do_lorarangingcompressaoa(DataItem * item)
{
    int Distance = item->getDistance();
    int projectid = item->getProjectID();

    VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);
    if ( Distance <= 0 || Distance > vilssysConfig->MaxRangindDistance)
    {
        vilslog::printf ("PosCalculateService::do_lorarangingcompressaoa() Ignore, LoRAMacaddress[%s] tag[%s] Distance[%d] velocity[%d] angle[%f]\n",
            item->getLoRAMacaddress(), item->getmacaddress(), Distance, item->getVelocity(), item->getAngle());
        return 0;
    }

    mPosCal->setLoRARangingAOA(projectid, item->getLoRAMacaddress(), item->getmacaddress(), Distance, item->getVelocity(), item->getAngle(), item->getRSSI(), item->getTimestamp());
    mPowerTagCal->setLoRARangingAOA(projectid, item->getLoRAMacaddress(), item->getmacaddress(), Distance, item->getVelocity(), item->getAngle(), item->getRSSI(), item->getTimestamp());

    char Tag_id[64];
    char Locator_id[64];
    sprintf(Tag_id, "%s", mDeviceManager->getTagId(projectid, item->getmacaddress()));
    sprintf(Locator_id, "%s", mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, item->getLoRAMacaddress()));

    // DataItem * sendItemckp = new DataItem("RangingFiltered", projectid, Tag_id, Locator_id, Distance, 1, item->getTimestamp());
    // mCheckPointJobQueue.add(sendItemckp);
    //
    // DataItem * sendItemdevEvent = new DataItem("RangingFiltered", projectid, Tag_id, Locator_id, Distance, 1, item->getTimestamp());
    // mDeviceEventJobQueue.add(sendItemdevEvent);

    return 1;
}

int PosCalculateService::do_PowerTagCalculateInfo(DataItem * item)
{
    int projectid = item->getProjectID();
    std::string message = item->getMessage();
    //vilslog::printf("PosCalculateService::do_PowerTagCalculateInfo() projectid[%d] message[%s]\n", projectid, message.c_str());

    Json::Value result;
    bool parsingSuccessful = parse_payload_JSON(message.c_str(), result);

    if (parsingSuccessful)
    {
        std::string AoAPowerTag_1;
        std::string AoAPowerTag_2;
        std::string AoAPowerTag_3;
        std::string AoAPowerTag_aoaid;
        if ( result.isMember("AoAPowerTag_1") )
            AoAPowerTag_1 = result["AoAPowerTag_1"].asString();
        if ( result.isMember("AoAPowerTag_2") )
            AoAPowerTag_2 = result["AoAPowerTag_2"].asString();
        if ( result.isMember("AoAPowerTag_3") )
            AoAPowerTag_3 = result["AoAPowerTag_3"].asString();
        if ( result.isMember("AoAPowerTag_aoaid") )
            AoAPowerTag_aoaid = result["AoAPowerTag_aoaid"].asString();

        // vilslog::printf("PosCalculateService::do_PowerTagCalculateInfo() \n\tAoAPowerTag_1[%s] AoAPowerTag_2[%s] AoAPowerTag_3[%s] AoAPowerTag_1[%s]\n",
        //     AoAPowerTag_1.c_str(), AoAPowerTag_2.c_str(), AoAPowerTag_3.c_str(), AoAPowerTag_aoaid.c_str());


        std::vector<std::string> tag1Vector = convert_string_to_vector(AoAPowerTag_1, ':');
        std::vector<std::string> tag2Vector = convert_string_to_vector(AoAPowerTag_2, ':');
        std::vector<std::string> tag3Vector = convert_string_to_vector(AoAPowerTag_3, ':');

        // vilslog::printf("PosCalculateService::do_PowerTagCalculateInfo() tag1Vector.size()[%d] tag2Vector.size()[%d] tag3Vector.size()[%d]\n",
        //     (int)tag1Vector.size(), (int)tag2Vector.size(), (int)tag3Vector.size());

        // vilslog::printf("PosCalculateService::do_PowerTagCalculateInfo() [%s][%s] [%s][%s] [%s][%s]\n",
        //     tag1Vector[0].c_str(), tag1Vector[1].c_str(), tag2Vector[0].c_str(), tag2Vector[1].c_str(), tag3Vector[0].c_str(), tag3Vector[1].c_str());


        if ( (int)tag1Vector.size() >= 1 )
            put_to_PowerTagInfoList(projectid, tag1Vector[0]);
        if ( (int)tag2Vector.size() >= 1 )
            put_to_PowerTagInfoList(projectid, tag2Vector[0]);
        if ( (int)tag3Vector.size() >= 1 )
            put_to_PowerTagInfoList(projectid, tag3Vector[0]);
        put_to_PowerTagAoAInfoList(projectid, AoAPowerTag_aoaid);


        if ( (int)tag1Vector.size() >= 2 )
        {
            std::vector<std::string> tag1Pos = convert_string_to_vector(tag1Vector[1], ',');

            if ( (int)tag1Pos.size() >= 3 )
            {
                vilslog::printf("PosCalculateService::do_PowerTagCalculateInfo() [%s][%s, %s, %s]\n",
                    tag1Vector[0].c_str(), tag1Pos[0].c_str(), tag1Pos[1].c_str(), tag1Pos[2].c_str());

                double posX = atoi(filterValidNumberString(tag1Pos[0].c_str(), tag1Pos[0].length()).c_str());
                double posY = atoi(filterValidNumberString(tag1Pos[1].c_str(), tag1Pos[1].length()).c_str());
                double posZ = atoi(filterValidNumberString(tag1Pos[2].c_str(), tag1Pos[2].length()).c_str());
                mDeviceManager->setTagInfoPos(projectid, tag1Vector[0].c_str(), posX, posY, posZ);
                mDeviceManager->setTagSmoothingPos(projectid, tag1Vector[0].c_str(), posX, posY, posZ);

            }
        }
        if ( (int)tag2Vector.size() >= 2 )
        {
            std::vector<std::string> tag2Pos = convert_string_to_vector(tag2Vector[1], ',');

            if ( (int)tag2Pos.size() >= 3 )
            {
                vilslog::printf("PosCalculateService::do_PowerTagCalculateInfo() [%s][%s, %s, %s]\n",
                    tag2Vector[0].c_str(), tag2Pos[0].c_str(), tag2Pos[1].c_str(), tag2Pos[2].c_str());

                double posX = atoi(filterValidNumberString(tag2Pos[0].c_str(), tag2Pos[0].length()).c_str());
                double posY = atoi(filterValidNumberString(tag2Pos[1].c_str(), tag2Pos[1].length()).c_str());
                double posZ = atoi(filterValidNumberString(tag2Pos[2].c_str(), tag2Pos[2].length()).c_str());
                mDeviceManager->setTagInfoPos(projectid, tag2Vector[0].c_str(), posX, posY, posZ);
                mDeviceManager->setTagSmoothingPos(projectid, tag2Vector[0].c_str(), posX, posY, posZ);

            }
        }
        if ( (int)tag3Vector.size() >= 2 )
        {
            std::vector<std::string> tag3Pos = convert_string_to_vector(tag3Vector[1], ',');

            if ( (int)tag3Pos.size() >= 3 )
            {
                vilslog::printf("PosCalculateService::do_PowerTagCalculateInfo() [%s][%s, %s, %s]\n",
                    tag3Vector[0].c_str(), tag3Pos[0].c_str(), tag3Pos[1].c_str(), tag3Pos[2].c_str());

                double posX = atoi(filterValidNumberString(tag3Pos[0].c_str(), tag3Pos[0].length()).c_str());
                double posY = atoi(filterValidNumberString(tag3Pos[1].c_str(), tag3Pos[1].length()).c_str());
                double posZ = atoi(filterValidNumberString(tag3Pos[2].c_str(), tag3Pos[2].length()).c_str());
                mDeviceManager->setTagInfoPos(projectid, tag3Vector[0].c_str(), posX, posY, posZ);
                mDeviceManager->setTagSmoothingPos(projectid, tag3Vector[0].c_str(), posX, posY, posZ);

            }
        }

        tag1Vector.clear();
        tag2Vector.clear();
        tag3Vector.clear();
    }

    return 1;
}

int PosCalculateService::do_FixPosition(DataItem * item)
{
    int projectid = item->getProjectID();
    std::string macaddress = item->getmacaddress();
    vilslog::printf("PosCalculateService::do_FixPosition() projectid[%d] macaddress[%s]\n", projectid, macaddress.c_str());

    bool bFound = false;
    FixPositionInfoStruct * moveInfo = NULL;
    for (std::list<FixPositionInfoStruct *>::iterator info = mDiffPositionInfoList.begin(); info != mDiffPositionInfoList.end(); info++)
    {
        if ( ((*info)->projectid == projectid) && (macaddress.compare((*info)->macaddress) == 0) )
        {
            moveInfo = (*info);
            bFound = true;
            break;
        }
    }

    if(!bFound)
    {
        return 0;
    }

    //
    // move to Parking Lot
    //
    put_to_fixPositionList(projectid, macaddress, moveInfo->posX, moveInfo->posY, moveInfo->posZ);

    return 0;
}

int PosCalculateService::do_FixDevice(DataItem * item)
{
    int projectid = item->getProjectID();
    std::string macaddress = item->getmacaddress();
    vilslog::printf("PosCalculateService::do_FixDevice() projectid[%d] macaddress[%s]\n", projectid, macaddress.c_str());

    PositionInfoStruct newPos;
    find_empty_parking_lot(projectid, &newPos, macaddress);

    char tagid[64];
    snprintf(tagid, 64, "%s", mDeviceManager->getTagId(projectid, macaddress.c_str()));
    if (strlen(tagid) == 0)
        return 0;

    mDeviceManager->setTagInfoPos(projectid, tagid, newPos.posX, newPos.posY, newPos.posZ);

    //
    // move to Parking Lot
    //
    put_to_fixPositionList(projectid, macaddress, newPos.posX, newPos.posY, newPos.posZ);

    return 0;
}

int PosCalculateService::do_AOAfence(DataItem * item)
{
    int projectid = item->getProjectID();
    std::string macaddress = item->getmacaddress();
    // vilslog::printf("PosCalculateService::do_AOAfence() projectid[%d] macaddress[%s]\n", projectid, macaddress.c_str());

    std::string level = item->getAOA_lv(); // "0.-1"
    std::string time = item->getAOA_time(); // "27"

    std::vector<std::string> levelList = convert_string_to_vector(level, '.');

    if ( levelList.size() > 1 )
    {
        int level_x = convert_string_to_int(levelList[0]);
        int level_y = convert_string_to_int(levelList[1]);
        int level_time = convert_string_to_int(time);
        // vilslog::printf("PosCalculateService::do_AOAfence() projectid[%d] macaddress[%s] level_x[%d] level_y[%d] level_time[%d]\n",
        //     projectid, macaddress.c_str(), level_x, level_y, level_time);

        mPosCal->setLoRAFenceAOA(projectid, item->getLoRAMacaddress(), item->getmacaddress(), level_x, level_y, level_time, item->getRSSI(), item->getTimestamp());
    }

    levelList.clear();

    return 0;
}

bool PosCalculateService::put_to_fixPositionList(int projectid, std::string macaddress, int posX, int posY, int posZ)
{
    bool bFound = false;
    for (std::list<FixPositionInfoStruct *>::iterator info = mFixPositionInfoList.begin(); info != mFixPositionInfoList.end(); info++)
    {
        if ( ((*info)->projectid == projectid) && (macaddress.compare((*info)->macaddress) == 0) )
        {
            (*info)->posX = posX;
            (*info)->posY = posY;
            (*info)->posZ = posZ;
            (*info)->issuetime = getCurrentTimeInSec();
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        FixPositionInfoStruct * newInfo = new FixPositionInfoStruct();
        newInfo->projectid = projectid;
        newInfo->posX = posX;
        newInfo->posY = posY;
        newInfo->posZ = posZ;
        newInfo->macaddress = macaddress;
        newInfo->issuetime = getCurrentTimeInSec();

        mFixPositionInfoList.push_back(newInfo);
    }
    return bFound;
}

bool PosCalculateService::find_empty_parking_lot(int projectid, PositionInfoStruct * newPos, std::string macaddress)
{
    bool bFound = false;
    int startX = 0;
    int startY = -1000;

    int count = 0;
    while (!bFound)
    {
        int startRow = count / 10;
        int startCol = count % 10;
        int curX = startX + startCol * 50; // offset 50 cm
        int curY = startY + startRow * 50; // offset 50 cm

        bool bExist = false;
        std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);
        for (std::list<NodeStruct*>::iterator node = tagNodeList->begin(); node != tagNodeList->end(); node++)
        {
            if ( (curX == (int)(*node)->posX) && (curY == (int)(*node)->posY) && ( macaddress.compare((*node)->macaddress) != 0 ) )
            {
                bExist = true;
                break;
            }
        }

        if (!bExist)
        {
            newPos->posX = curX;
            newPos->posY = curY;
            newPos->posZ = 0;
            bFound = true;

            vilslog::printf("PosCalculateService::find_empty_parking_lot() projectid[%d] count[%d] curX[%d] posY[%d]\n",
                projectid, count, curX, curY);

            break;
        }
        count++;
    }

    return bFound;
}

bool PosCalculateService::update_diffPositionList(int projectid, char * macaddress, double distance, double * rawresult)
{
    bool bFound = false;
    bool bRemove = false;

    if ( distance <= 50 )
    {
        bRemove = true;
        bFound = true;
    }

    for (std::list<FixPositionInfoStruct *>::iterator info = mDiffPositionInfoList.begin(); info != mDiffPositionInfoList.end(); info++)
    {
        if ( ((*info)->projectid == projectid) && ((*info)->macaddress.compare(macaddress) == 0) )
        {
            if (bRemove)
            {
                // vilslog::printf("PosCalculateService::update_diffPositionList() projectid[%d] macaddress[%s] bRemove\n",
                //     projectid, macaddress);

                FixPositionInfoStruct * tmpInfo = (*info);
                mDiffPositionInfoList.remove((*info));
                delete tmpInfo;
            }
            else
            {
                (*info)->posX = rawresult[0];
                (*info)->posY = rawresult[1];
                (*info)->posZ = rawresult[2];
                (*info)->issuetime = getCurrentTimeInSec();
            }
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        FixPositionInfoStruct * newInfo = new FixPositionInfoStruct();
        newInfo->projectid = projectid;
        newInfo->posX = rawresult[0];
        newInfo->posY = rawresult[1];
        newInfo->posZ = rawresult[2];
        newInfo->macaddress = macaddress;
        newInfo->issuetime = getCurrentTimeInSec();

        // vilslog::printf("PosCalculateService::update_diffPositionList() projectid[%d] macaddress[%s] rawresult[%d,%d,%d]\n",
        //     projectid, macaddress, (int)rawresult[0], (int)rawresult[1], (int)rawresult[2]);

        mDiffPositionInfoList.push_back(newInfo);
    }
    return bFound;
}

int PosCalculateService::do_updatlocatorposition(DataItem * item)
{
    int projectid = item->getProjectID();
    std::string Message = item->getMessage();

    mPosCal->updatlocatorposition(projectid, item->getNodeId(), Message);

    return 0;
}

int PosCalculateService::loadSectionList()
{
    std::list<int> * prjidList = mDbManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);
        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if (baseDb != NULL)
        {
            std::list<SectionGroup *> * loSectionGroupList = new std::list<SectionGroup *>();
            baseDb->read_SectionGroupList(-1, loSectionGroupList);

            // vilslog::printf("PosCalculateService::run() projectid[%d] loSectionGroupList->size()[%lu]\n", projectid, loSectionGroupList->size());

            // vilslog::printf("===============\nSectionGroupList projectid[%d]\n", projectid);
            // printout(loSectionGroupList);
            // vilslog::printf("===============\n");

            mPosCal->setSectionGroupList(projectid, loSectionGroupList);

            clear_SectionGroup_list(loSectionGroupList);
            delete loSectionGroupList;
        }
    }

    prjidList->clear();
    delete prjidList;

    return 0;
}

bool PosCalculateService::checkTagUpdateValidTime(int projectid, const char * tagid, int validtime)
{
    bool validUpdate = false;

    bool bFound = false;

    for (std::list<TagUpdateInfoStruct*>::iterator existed = mTagUpdateList.begin(); existed != mTagUpdateList.end(); existed++)
    {
        if ( ((*existed)->projectid == projectid) && ((*existed)->TagId.compare(tagid) == 0) )
        {
            time_t currentTime = getCurrentTimeInMilliSec();
            time_t diffTime = currentTime - (*existed)->issuetime;

            if (diffTime >= validtime)
            {
                validUpdate = true;
                (*existed)->issuetime = currentTime;
            }

            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        TagUpdateInfoStruct * newTagInfo = new TagUpdateInfoStruct();
        newTagInfo->projectid = projectid;
        newTagInfo->TagId = tagid;
        newTagInfo->issuetime = getCurrentTimeInMilliSec();
        mTagUpdateList.push_back(newTagInfo);

        validUpdate = true;
    }

    return validUpdate;
}

TagRangingQueueStruct * PosCalculateService::getTagRangingQueue(int projectid, std::string tagMacaddress)
{
    TagRangingQueueStruct * tagQueue = NULL;
    //vilslog::printf ("PosCalculateService()::getTagRangingQueue() tagMacaddress[%s]\n", tagMacaddress.c_str());

    for (std::list<TagRangingQueueStruct*>::iterator queue = mTagRangingQueueList.begin(); queue != mTagRangingQueueList.end(); queue++)
    {
        //vilslog::printf ("PosCalculateService()::getTagRangingQueue() (*queue)->tagMacaddress[%s]\n", (*queue)->tagMacaddress.c_str());

        if ( ((*queue)->projectid == projectid) && ((*queue)->tagMacaddress.compare(tagMacaddress) == 0) )
        {
            tagQueue = (*queue);
            break;
        }
    }

    if (tagQueue == NULL)
    {
        //vilslog::printf ("PosCalculateService()::getTagRangingQueue() tagQueue == NULL\n");

        tagQueue = new TagRangingQueueStruct();
        tagQueue->projectid = projectid;
        tagQueue->tagMacaddress = tagMacaddress;
        tagQueue->tagId = mDeviceManager->getTagId(projectid, tagMacaddress.c_str());
        mTagRangingQueueList.push_back(tagQueue);
    }
    return tagQueue;
}

bool PosCalculateService::put_to_PowerTagInfoList(int projectid, std::string tagId)
{
    bool bFound = false;

    for (std::list<PowerTagInfoStruct*>::iterator queue = mPowerTagInfoList.begin(); queue != mPowerTagInfoList.end(); queue++)
    {
        if ( (*queue)->projectid == projectid )
        {
            if ( tagId.compare((*queue)->tagId) == 0 )
            {
                bFound = true;
                (*queue)->issuetime = getCurrentTimeInSec();
                break;
            }
        }
    }

    if (!bFound)
    {
        PowerTagInfoStruct * newInfo = new PowerTagInfoStruct();
        newInfo->projectid = projectid;
        newInfo->tagId = tagId;
        newInfo->issuetime = getCurrentTimeInSec();
        mPowerTagInfoList.push_back(newInfo);

        vilslog::printf(" PosCalculateService::put_to_PowerTagInfoList() mPowerTagInfoList.push_back(%s)\n", tagId.c_str());

    }

    return bFound;
}

bool PosCalculateService::put_to_PowerTagAoAInfoList(int projectid, std::string aoaId)
{
    bool bFound = false;

    for (std::list<PowerTagAoAInfoStruct*>::iterator queue = mPowerTagAoAInfoList.begin(); queue != mPowerTagAoAInfoList.end(); queue++)
    {
        if ( (*queue)->projectid == projectid )
        {
            if ( aoaId.compare((*queue)->aoaId) == 0 )
            {
                bFound = true;
                (*queue)->issuetime = getCurrentTimeInSec();
                break;
            }
        }
    }

    if (!bFound)
    {
        PowerTagAoAInfoStruct * newInfo = new PowerTagAoAInfoStruct();
        newInfo->projectid = projectid;
        newInfo->aoaId = aoaId;
        newInfo->issuetime = getCurrentTimeInSec();
        mPowerTagAoAInfoList.push_back(newInfo);

        vilslog::printf(" PosCalculateService::put_to_PowerTagAoAInfoList() mPowerTagInfoList.push_back(%s)\n", aoaId.c_str());

    }

    return bFound;
}

bool PosCalculateService::check_powertag_test(int projectid, const char * tagId)
{
    bool bFound = false;
    for (std::list<PowerTagInfoStruct*>::iterator queue = mPowerTagInfoList.begin(); queue != mPowerTagInfoList.end(); queue++)
    {
        if ( (*queue)->projectid == projectid )
        {
            if ( strcmp((*queue)->tagId.c_str(), tagId) == 0 )
            {
                bFound = true;

                time_t diff_t = getCurrentTimeInSec() - (*queue)->issuetime;
                if ( (int)diff_t >= 60)
                {
                    vilslog::printf(" PosCalculateService::check_powertag_test() mPowerTagInfoList.remove(%s)\n", tagId);

                    PowerTagInfoStruct * target = (*queue);
                    mPowerTagInfoList.remove(target);
                    delete target;
                }
                break;
            }
        }
    }
    return bFound;
}

SoccerRecordingStruct * PosCalculateService::getCurrentSoccerRecording(int maingroupid)
{
    SoccerRecordingStruct * retRecord = NULL;
    for (std::list<SoccerRecordingStruct*>::iterator record = mSoccerRecordingList.begin(); record != mSoccerRecordingList.end(); record++)
    {
        if ((*record)->maingroupid == maingroupid )
        {
            retRecord = (*record);
            break;
        }
    }
    return retRecord;
}

std::string PosCalculateService::getTagStatus(double * IMU)
{
    std::string status = "none";

    double accelX = IMU[0];
    double accelY = IMU[1];
    double accelZ = IMU[2];
    // double gyroX = IMU[3];
    // double gyroY = IMU[4];
    // double gyroZ = IMU[5];

    if( (accelX > 20.0f || accelX < -20.0f) || (accelY > 20.0f || accelY < -20.0f))
    {
        status = "collision";
    }
    else
    if ( ((accelX > 10.0f || accelX < -10.0f) || (accelY > 10.0f || accelY < -10.0f)) && (accelZ < 10.0f || accelZ > -10.0f) )
    {
        status = "fall";
    }
    else
    if( (accelZ > 20.0f || accelZ < -20.0f) )
    {
        status = "sprint";
    }

    return status;
}
