/*
 *  CoordIncomingService.h
 *
 *  Created on: 2017/02/21
 *      Author: ikki
 */
#ifndef __CoordIncomingService_h__
#define __CoordIncomingService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../network/wqueue.h"
#include "../network/UDPDataItem.h"
#include "../common.h"
#include "../json/json.h"
#include "../network/UDPServer.h"
#include "../mysql/DBManager.h"

using namespace std;

class CoordIncomingService : public Thread
{
public:
    CoordIncomingService(wqueue<UDPDataItem*> & udpqueue, DBManager * dbManager);
    bool status();
    std::string statusString();

    void * run();

private:

    wqueue<UDPDataItem*> & m_udpqueue;
    DBManager * mDbManager;
    VILSSystemConfig m_systemConfig;

    bool mUdpInitial;

    std::list<UDPServer *> * mUdpServerList;
    std::list<NetworkInfoStruct * > mCenterNetworkInfoList;

    std::string mStatus;

    void runloop();
    std::list<UDPServer *> * createMultipleIfServer(int port);
    void waitloop();
};

#endif
