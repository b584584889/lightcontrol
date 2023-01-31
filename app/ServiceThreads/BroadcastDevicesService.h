/*
 *  BroadcastDevicesService.h
 *
 *  Created on: 2018/10/19
 *      Author: ikki
 */
#ifndef __BroadcastDevicesService_h__
#define __BroadcastDevicesService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../common.h"
#include "../config.h"
#include "../network/wqueue.h"
#include "../network/DataItem.h"
#include "../DeviceManager/DeviceManager.h"

using namespace std;

class BroadcastDevicesService : public Thread
{
public:
    BroadcastDevicesService(wqueue<DataItem*> & pSendOutJobQueue, DeviceManager * pDevManager, DBManager * dbManager);
    bool status();
    std::string statusString();

    void * run();

private:

    std::string mStatus;
    bool mInitial;

    wqueue<DataItem*> & mSendOutJobQueue;
    DeviceManager * mDeviceManager;
    DBManager * mDbManager;

    std::list<int> * mPrjidList;

    void runloop();
    void waitloop();
    int broadcast_devices();
    int broadcast_devices(int projectid);
    int broadcast_tag_devices();
    int broadcast_tag_devices(int projectid);
    int broadcast_anchor_devices();
    int broadcast_anchor_devices(int projectid);
    int broadcast_coord_devices();
    int broadcast_coord_devices(int projectid);
    int broadcast_alive_tag();
    int broadcast_alive_tag(int projectid);
    int broadcast_systemtime();
    int broadcast_systemtime(int projectid);
};


#endif
