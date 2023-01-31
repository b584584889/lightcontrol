/*
 *  AnalyzeNodePosOffsetService.cpp
 *
 *  Created on: 2019/0904
 *      Author: ikki
 */

#include <string.h>
#include <ctime>
#include <math.h>
#include "AnalyzeNodePosOffsetService.h"
#include "../common.h"
#include "../mysql/struct.h"

using namespace std;

AnalyzeNodePosOffsetService::AnalyzeNodePosOffsetService(wqueue<WorkItem*> & trackdbqueue, wqueue<WorkItem*> & nodePositionQueue, DeviceManager * pDevManager)
    : mTrackDBqueue(trackdbqueue), mNodePositionQueue(nodePositionQueue), mDeviceManager(pDevManager)
{
}

AnalyzeNodePosOffsetService::~AnalyzeNodePosOffsetService()
{
}

bool AnalyzeNodePosOffsetService::status()
{
    return mInitial;
}

std::string AnalyzeNodePosOffsetService::statusString()
{
    return mStatus;
}

void * AnalyzeNodePosOffsetService::run()
{
    while (1)
    {
        mInitial = false;
        mStatus = "AnalyzeNodePosOffsetService() Initial failed!!";

        try
        {
            runloop();
            vilslog::printf ("AnalyzeNodePosOffsetService() Initial failed!\n");
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("AnalyzeNodePosOffsetService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");
            mInitial = false;
            mStatus = "AnalyzeNodePosOffsetService::run() exception:";
            mStatus.append(exc.what());
        }
    }

    return NULL;
}

void AnalyzeNodePosOffsetService::runloop()
{
    //long long start1 = getSystemTime();
    //long long updateTime = 0;

    mInitial = true;
    mStatus = "AnalyzeNodePosOffsetService() Initial successfully!";
    vilslog::printf ("AnalyzeNodePosOffsetService() Initial successfully!\n");

    // available to process.
    while (1)
    {
        //long long end1 = getSystemTime();
        //long long difft = end1 - start1;
        //updateTime += difft;

        WorkItem * item = mNodePositionQueue.removetimed(1000);
        if (item != NULL)
        {
            std::string type = item->getType();
            int projectid = item->getProjectId();

            if ( type.compare("taggpsinfo") == 0 )
            {
                if ( !isZero(item->mGPSInfo.Latitude, 0.0000001) && !isZero(item->mGPSInfo.Longitude, 0.0000001) )
                {
                    double posX = LatitudeToCentimeter(item->mGPSInfo.Latitude);
                    double posY = LongitudeToCentimeter(item->mGPSInfo.Latitude, item->mGPSInfo.Longitude);

                    analyze_tag_pos(projectid, item->mGPSInfo.nodeid.c_str(), posX, posY, 0);
                }
            }
            else
            if ( type.compare("tagposinfo") == 0 )
            {
                analyze_tag_pos(projectid, item->getNodeId(), item->getPosX(), item->getPosY(), item->getPosZ());
            }

            delete item;
        }

        //start1 = end1;
    }
}

bool AnalyzeNodePosOffsetService::analyze_tag_pos(int projectid, const char * nodeid, double posX, double posY, double posZ)
{
    // vilslog::printf("AnalyzeNodePosOffsetService::analyze_tag_pos() nodeid[%s] [%f, %f, %f]\n",
    //     item->getNodeId(), item->getPosX(), item->getPosY(), item->getPosZ());

    bool bFound = false;
    for (std::list<TagMoveOffsetInfoStruct*>::iterator i = mTagMoveOffsetList.begin(); i != mTagMoveOffsetList.end(); i++)
    {
        if( ((*i)->projectid == projectid) && ((*i)->TagId.compare(nodeid) == 0) )
        {
            // offset
            double x_diff = (*i)->prePosX - posX;
            double y_diff = (*i)->prePosY - posY;
            //double z_diff = (*i)->prePosZ - posZ;
            //double distance =  sqrt(pow(x_diff, 2) + pow(y_diff, 2) + pow(z_diff, 2));
            double distance =  sqrt(pow(x_diff, 2) + pow(y_diff, 2));
            (*i)->moveoffset += (int)distance;
            (*i)->prePosX = posX;
            (*i)->prePosY = posY;
            (*i)->prePosZ = posZ;

            // check time
            time_t cur_time_t = getCurrentTimeInSec();
            double diifTime = std::difftime(cur_time_t, (*i)->issuetime);

            if ((int)diifTime > 60)
            {
                // vilslog::printf("AnalyzeNodePosOffsetService::analyze_tag_pos() [%s] nodeid[%s] moveoffset[%d]\n",
                //     time_tToString(cur_time_t).c_str(), nodeid, (*i)->moveoffset);

                WorkItem * writem = new WorkItem("TagMoveOffsetInfoRecord", projectid, (*i));
                mTrackDBqueue.add(writem);

                // clear
                (*i)->moveoffset = 0;
                (*i)->issuetime = cur_time_t;
            }

            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        vilslog::printf("AnalyzeNodePosOffsetService::analyze_tag_pos() nodeid[%s] Create New record!\n", nodeid);

        TagMoveOffsetInfoStruct * newTag = new TagMoveOffsetInfoStruct();
        newTag->projectid = projectid;
        newTag->TagId = nodeid;
        newTag->prePosX = posX;
        newTag->prePosY = posY;
        newTag->prePosZ = posZ;
        newTag->moveoffset = 0;
        newTag->issuetime = getCurrentTimeInSec();
        mTagMoveOffsetList.push_back(newTag);
    }

    return true;
}
