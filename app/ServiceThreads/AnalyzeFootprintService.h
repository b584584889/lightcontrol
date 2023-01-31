/*
 *  AnalyzeFootprintService.h
 *
 *  Created on: 2019/04/17
 *      Author: ikki
 */
#ifndef __AnalyzeFootprintService_h__
#define __AnalyzeFootprintService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Thread.h"
#include "../mysql/DBManager.h"
#include "../DeviceManager/DeviceManager.h"

#include "../config.h"

//#define ANALYZE_FOOTPRINT_PRINT_MSG

using namespace std;

class AnalyzeFootprintService : public Thread
{
public:
    AnalyzeFootprintService(DeviceManager * pDevManager, DBManager * dbManager);
    AnalyzeFootprintService();
    ~AnalyzeFootprintService();
    bool status();
    std::string statusString();
    void updateProject();

    void * run();

private:

    std::string mStatus;
    bool mInitial;

    DeviceManager * mDeviceManager;
    DBManager * mDbManager;

    bool mUpdateProject;
    std::list<int> * mPrjidList;

    void runloop();
    // void analyzeFootprint(int projectid, bool checkNULLDate);
};

#endif
