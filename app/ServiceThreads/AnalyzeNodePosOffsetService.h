/*
 *  AnalyzeNodePosOffsetService.h
 *
 *  Created on: 2019/09/04
 *      Author: ikki
 */
#ifndef __AnalyzeNodePosOffsetService_h__
#define __AnalyzeNodePosOffsetService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Thread.h"
#include "../network/WorkItem.h"
#include "../DeviceManager/DeviceManager.h"
#include "../config.h"

using namespace std;

class AnalyzeNodePosOffsetService : public Thread
{
public:
    AnalyzeNodePosOffsetService(wqueue<WorkItem*> & trackdbqueue, wqueue<WorkItem*> & nodePositionQueue, DeviceManager * pDevManager);
    AnalyzeNodePosOffsetService();
    ~AnalyzeNodePosOffsetService();
    bool status();
    std::string statusString();

    void * run();

private:

    std::string mStatus;
    bool mInitial;

    wqueue<WorkItem*> & mTrackDBqueue;
    wqueue<WorkItem*> & mNodePositionQueue;
    DeviceManager * mDeviceManager;

    std::list<TagMoveOffsetInfoStruct *> mTagMoveOffsetList;

    void runloop();
    bool analyze_tag_pos(int projectid, const char * nodeid, double posX, double posY, double posZ);
};

#endif
