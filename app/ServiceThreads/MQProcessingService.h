/*
 *  MQProcessingService.h
 *
 *  Created on: 2021/02/04
 *      Author: ikki
 */
#ifndef __MQProcessingService_h__
#define __MQProcessingService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../network/wqueue.h"
#include "../network/DataItem.h"
//#include "../MQ/AMQCPPConnectionHandler.h"

using namespace std;

class MQProcessingService : public Thread
{
public:
    MQProcessingService(wqueue<DataItem*> & pMQqueue);
    bool status();
    std::string statusString();

    void * run();

private:

    std::string mStatus;
    bool mInitial;

    wqueue<DataItem*> & mMQJobQueue;

    void runloop();
    void waitloop();
    void test_MQ();

};

#endif
