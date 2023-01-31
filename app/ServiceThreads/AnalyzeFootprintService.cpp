/*
 *  AnalyzeFootprintService.cpp
 *
 *  Created on: 2019/04/17
 *      Author: ikki
 */

#include <string.h>
#include "AnalyzeFootprintService.h"
#include "../common.h"

using namespace std;

AnalyzeFootprintService::AnalyzeFootprintService(DeviceManager * pDevManager, DBManager * dbManager)
    : mDeviceManager(pDevManager), mDbManager(dbManager)
{
}

AnalyzeFootprintService::~AnalyzeFootprintService()
{
}

bool AnalyzeFootprintService::status()
{
    return mInitial;
}

std::string AnalyzeFootprintService::statusString()
{
    return mStatus;
}

void AnalyzeFootprintService::updateProject()
{
    mUpdateProject = true;
}

void * AnalyzeFootprintService::run()
{
    while (1)
    {
        mInitial = false;
        mStatus = "AnalyzeFootprintService() Initial failed!!";
        //vilslog::printf ("AnalyzeFootprintService() Initial failed!\n");

        try
        {
            runloop();
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("AnalyzeFootprintService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");
            mInitial = false;
            mStatus = "AnalyzeFootprintService::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void AnalyzeFootprintService::runloop()
{
    long long start1 = getSystemTime();
    long long analyzeFootprintTime = 0;
    long long checkNULLDateTime = 0; // check at first start
    long long updatePrjIdTime = 0;
    //long long totlatime_analyzeFootprint = 0;

    mPrjidList = mDbManager->getAllProjectConfig();

    mInitial = true;
    mStatus = "AnalyzeFootprintService() Initial successfully!";
    vilslog::printf ("AnalyzeFootprintService() Initial successfully!\n");


    // available to process.
    while (1)
    {
        long long end1 = getSystemTime();
        long long difft = end1 - start1;
        analyzeFootprintTime += difft;
        checkNULLDateTime += difft;
        updatePrjIdTime += difft;

        if(analyzeFootprintTime >= 600000) // 10 minutes
        //if(analyzeFootprintTime >= 60000) // 1 minutes
        {
            //long long analyzeFootprintStart = getSystemTime();

#ifdef ANALYZE_FOOTPRINT_PRINT_MSG
            vilslog::printf("AnalyzeFootprintService::run() analyzeFootprintTime[%lld]\n", analyzeFootprintTime);
#endif

            //bool checkNULLDate = false;
            if (checkNULLDateTime >= 86400000)// 24 hours
            {
                //checkNULLDate = true;
                checkNULLDateTime = 0;
            }

            for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
            {
                //int projectid = (*prj);
                //analyzeFootprint(projectid, checkNULLDate);
            }

            //totlatime_analyzeFootprint = getSystemTime() - analyzeFootprintStart;
            //vilslog::printf("AnalyzeFootprintService::run() totlatime_analyzeFootprint[%d]\n", (int)totlatime_analyzeFootprint);

            analyzeFootprintTime = 0;
        }

        if (updatePrjIdTime > 3600000 || mUpdateProject)
        {
            mPrjidList->clear();
            delete mPrjidList;
            mPrjidList = mDbManager->getAllProjectConfig();

            updatePrjIdTime = 0;
            mUpdateProject = false;
        }

        start1 = end1;
        sleep(10);
    }
}

// void AnalyzeFootprintService::analyzeFootprint(int projectid, bool checkNULLDate)
// {
//     std::list<NodeStruct*> * TagNodeList = mDeviceManager->get_Tag_list(projectid);
//     TimeSerialsDBAccess * tsDb = mDbManager->getTSDB(projectid);
//     if (tsDb == NULL)
//         return ;
//
// #ifdef ANALYZE_FOOTPRINT_PRINT_MSG
//     int totaltag = TagNodeList->size();
// #endif
//     int totalNnullDateCount = 0;
//     int index = 1;
//     for (std::list<NodeStruct*>::iterator i = TagNodeList->begin(); i != TagNodeList->end(); i++)
//     {
// #ifdef ANALYZE_FOOTPRINT_PRINT_MSG
//         vilslog::printf("AnalyzeFootprintService::run() analyzeFootprintall [%d/%d] nodeid[%s]\n",
//             index, totaltag, (*i)->nodeid);
// #endif
//
//         int nullDateCount = tsDb->analyzeFootprintall(NODE_TYPE_TAG, (*i)->nodeid, checkNULLDate);
//
//         totalNnullDateCount += nullDateCount;
//
// #ifdef ANALYZE_FOOTPRINT_PRINT_MSG
//         vilslog::printf("AnalyzeFootprintService::run() analyzeFootprintall [%d/%d] nodeid[%s] nullDateCount[%d]\n",
//             index, totaltag, (*i)->nodeid, nullDateCount);
// #endif
//         index++;
//     }
//
// #ifdef ANALYZE_FOOTPRINT_PRINT_MSG
//     vilslog::printf("AnalyzeFootprintService::run() analyzeFootprintall totalNnullDateCount[%d]\n", totalNnullDateCount);
// #endif
// }
