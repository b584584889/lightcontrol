/*
 *  CenterWebSocketService.h
 *
 *  Created on: 2018/09/25
 *      Author: ikki
 */
#ifndef __CenterWebSocketService_h__
#define __CenterWebSocketService_h__


#include "Thread.h"
#include "../network/wqueue.h"
#include "../network/inter_thread_pubsub.hpp"
#include "../config.h"
#include "../mysql/DBManager.h"
#include "../websocket/SubscribeDataItem.h"

#include "WebSocketServer.h"

class CenterWebSocketService : public Thread
{
public:
    CenterWebSocketService(wqueue<SubscribeDataItem*> & queue, DBManager * dbManager);
    bool status();
    std::string statusString();

    void * run();
private:
    wqueue<SubscribeDataItem*> & m_queue;
    ITPS::Subscriber<SubscribeDataItem*> * mEventSubscriber;

    DBManager * mDbManager;
    VILSSystemConfig m_systemConfig;

    WebSocketServer * m_server;

    bool mInitial;
    std::string mStatus;

    void initQueue();
    void runloop();
};

#endif
