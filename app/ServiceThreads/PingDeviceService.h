/*
 * PingDeviceService.h
 *
 *  Created on: 2019/05/21
 *      Author: ikki
 */

#ifndef __PingDeviceService_h__
#define __PingDeviceService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../config.h"
#include "../common.h"

using namespace std;

class PingDeviceService : public Thread
{
public:
    PingDeviceService();
    bool status();
    std::string statusString();

    void * run();
    void addPingAddress(const char * IPAddress, int alive);
    int getPingCount(const char * IPAddress, bool clear);

private:

    std::string mStatus;
    bool mInitial;

    bool mUpdate;
    std::list<PingStatusStruct *> mIPAddressList;

    void runloop();
    void waitloop();
};


#endif
