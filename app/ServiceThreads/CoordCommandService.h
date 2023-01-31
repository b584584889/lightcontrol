/*
 *  CoordCommandService.h
 *
 *  Created on: 2019/08/05
 *      Author: ikki
 */
#ifndef __CoordCommandService_h__
#define __CoordCommandService_h__

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

class CoordCommandService : public Thread
{
public:
    CoordCommandService(DBManager * dbManager);
    bool status();
    std::string statusString();

    void * run();
    void updateconfig();
private:

    DBManager * mDbManager;
    VILSSystemConfig m_systemConfig;

    bool mUdpInitial;

    std::list<UDPServer *> * mCoordCMDServerList;
    std::list<NetworkInfoStruct * > mCenterNetworkInfoList;

    std::string mStatus;

    void runloop();
    std::list<UDPServer *> * createMultipleIfServer(int port);
    void waitloop();

    int process(const char * clientIP, int length, const char * buf);

    int do_boot(const char * clientIP, Json::Value JSONresult);

    int send_back_server_port(const char * clientIP, int target_port);
};

#endif
