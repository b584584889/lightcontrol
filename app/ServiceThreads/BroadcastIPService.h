/*
 *  BroadcastIPService.h
 *
 *  Created on: 2018/10/19
 *      Author: ikki
 */
#ifndef __BroadcastIPService_h__
#define __BroadcastIPService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../config.h"
#include "../network/MulticastClient.h"
#include "../mysql/DBManager.h"
#include "../common.h"

using namespace std;



class BroadcastIPService : public Thread
{
public:
    BroadcastIPService(DBManager * dbManager);
    bool status();
    std::string statusString();

    void * run();

private:

    std::string mStatus;
    bool mInitial;

    DBManager * mDbManager;

    int m_center_broadcast_port;
    std::list<std::string> mCenterIPList;
    std::list<NetworkInfoStruct * > mCenterNetworkInfoList;
    std::list<MulticastClient * > mBroadcastIPclientList;

    //MulticastClient * mBroadcastIPclient;

    void runloop();
    void waitloop();
};


#endif
