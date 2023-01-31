/*
 *  CenterOutgoingService.h
 *
 *  Created on: 2017/02/21
 *      Author: ikki
 */
#ifndef __CenterOutgoingService_h__
#define __CenterOutgoingService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../network/wqueue.h"
#include "../network/DataItem.h"
#include "../network/UDPClient.h"
#include "../network/TCPServer.h"
#include "../network/MulticastClient.h"
#include "../mysql/DBManager.h"
#include "../config.h"

using namespace std;

class CenterOutgoingService : public Thread
{
public:
    CenterOutgoingService(wqueue<DataItem*> & queue, DBManager * dbManager);
    bool status();
    std::string statusString();
    void updateProject();


    void * run();

private:

    wqueue<DataItem*> & m_queue;
    DBManager * mDbManager;

    bool mUdpInitial;
    std::list<UDPClient *> mUdpClientList;
    std::list<NetworkInfoStruct * > mCenterNetworkInfoList;

    bool mUpdateProject;
    std::list<int> * mPrjidList;

    std::string mStatus;

    char mBuf[MAXUDPPACKET];

    void runloop();

    int wait_incoming_msg();
    int send_list(DataItem * item, bool bChekAlive);
    // int send_validList(DataItem * item);
    int send_tagsampletime(DataItem * item);
    int send_coordinatorreset(DataItem * item);
    int send_m0reset(DataItem * item);
    int send_setvalidrang(DataItem * item);
    int send_blacklist(DataItem * item);
    int send_whitelist(DataItem * item);
    int send_systemtime(DataItem * item);

    int query_validrang(DataItem * item);
    int query_blacklist(DataItem * item);
    int query_whitelist(DataItem * item);

    int query_validrang();
    int query_blacklist();
    int query_whitelist();

    int send_item_data(DataItem * item, const char * buf, int buf_len);
    int send_out_data(const char * buf, int buf_len);

};

#endif
