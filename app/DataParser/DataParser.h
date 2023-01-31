/*
 * DataParser.h
 *
 *  Created on: 2017/02/21
 *      Author: ikki
 */


#ifndef DATA_PARSER_H
#define DATA_PARSER_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <list>

#include "../config.h"
#include "../network/DataItem.h"
#include "../network/WorkItem.h"
#include "../websocket/SubscribeDataItem.h"
#include "../network/NotificationItem.h"
#include "../DeviceManager/DeviceManager.h"

#define BUF_MAX 1024

#define NODE_ACTION_NONE 0
#define NODE_ACTION_ANCHOR_INDEX 1
#define NODE_ACTION_TAG_INDEX 2
#define NODE_ACTION_SEND_NODE_ALIVE 3

typedef struct
{
    int index;
    int nodetype;
    char panid[32];
    char version[32];
    char macaddress[32];
    char voltage[32];
    char temperature[32];
} DeviceJoinStruct;

class DataParser
{
public:
	DataParser(wqueue<DataItem*> & PosCalJobQueue, wqueue<SubscribeDataItem*> & pWebSocketqueue, wqueue<NotificationItem*> & NotifiactionQueue, wqueue<WorkItem*> & MySQLJobQueue, DeviceManager * pDevManager);
	~DataParser();
    int processData(DataItem * item);
private:

    wqueue<DataItem*> & mPosCalJobQueue;
    wqueue<SubscribeDataItem*> & mWebSocketqueue;
    wqueue<NotificationItem*> & mNotifiactionQueue;
    wqueue<WorkItem*> & mMySQLJobQueue;
    DeviceManager * mDeviceManager;

    int do_join(DataItem * item);
    int do_alive(DataItem * item);
    // int do_ranging(DataItem * item);
    // int do_rangingdiag(DataItem * item);
    int do_rangingdiag_1(DataItem * item);

    int do_ranging(std::list<DeviceJoinStruct*> * plist, const char * macaddress);
    int do_list(DataItem * item);
    int do_alert(DataItem * item);
    int do_cancel(DataItem * item);
};

#endif
