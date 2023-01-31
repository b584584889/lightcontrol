/*
 *  WebUDPAPIService.h
 *
 *  Created on: 2018/10/18
 *      Author: ikki
 */
#ifndef __WebUDPAPIService_h__
#define __WebUDPAPIService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../network/wqueue.h"
#include "../network/UDPDataItem.h"
#include "../network/UDPServer.h"
#include "../mysql/DBManager.h"
#include "../config.h"

using namespace std;

class WebUDPAPIService : public Thread
{
public:
    WebUDPAPIService(wqueue<UDPDataItem*> & udpqueue, DBManager * dbManager);
    bool status();
    std::string statusString();

    void * run();
private:
    bool mUdpInitial;
    UDPServer * mWebUDPServer;
    std::string mStatus;

    wqueue<UDPDataItem*> & m_udpqueue;
    DBManager * mDbManager;
    //VILSSystemConfig m_systemConfig;

    void runloop();
    void waitloop();
};


#endif
