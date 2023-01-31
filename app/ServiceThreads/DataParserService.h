/*
 *  DataParserService.h
 *
 *  Created on: 2018/10/24
 *      Author: ikki
 */
#ifndef __DataParserService_h__
#define __DataParserService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../config.h"
#include "../DataParser/DataParser.h"

using namespace std;

class DataParserService : public Thread
{
public:
    DataParserService(wqueue<DataItem*> & UDPJobQueue, wqueue<DataItem*> & PosCalJobQueue, wqueue<SubscribeDataItem*> & pWebSocketqueue, wqueue<NotificationItem*> & NotifiactionQueue, wqueue<WorkItem*> & MySQLJobQueue, DeviceManager * pDevManager);
    bool status();
    std::string statusString();

    void * run();

private:

    std::string mStatus;
    bool mInitial;

    wqueue<DataItem*> & mUDPJobQueue;
    wqueue<DataItem*> & mPosCalJobQueue;
    wqueue<SubscribeDataItem*> & mWebSocketqueue;    
    wqueue<NotificationItem*> & mNotifiactionQueue;
    wqueue<WorkItem*> & mMySQLJobQueue;
    DeviceManager * mDeviceManager;

    DataParser * mDataParser;

    void runloop();
    void waitloop();

};


#endif
