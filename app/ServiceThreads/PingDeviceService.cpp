/*
 * PingDeviceService.cpp
 *
 *  Created on: 2019/05/21
 *      Author: ikki
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include "../network/SocketPing.h"
#include "PingDeviceService.h"

using namespace std;

PingDeviceService::PingDeviceService()
{
}

bool PingDeviceService::status()
{
    return mInitial;
}

std::string PingDeviceService::statusString()
{
    return mStatus;
}

void * PingDeviceService::run()
{
    while(1)
    {
        try
        {
            runloop();
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("PingDeviceService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "PingDeviceService::run() exception";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}


void PingDeviceService::runloop()
{
    mInitial = false;

    mStatus = "PingDeviceService()::run() Initial PingDeviceService successfully!";
    vilslog::printf ("PingDeviceService()::run() Initial PingDeviceService successfully!\n");

    mInitial = true;

    waitloop();

}

void PingDeviceService::waitloop()
{
    mUpdate = true;
    int waitCount = 0;
    while ( 1 )
    {

        if (mUpdate || waitCount >= 30)
        {
            for (std::list<PingStatusStruct *>::iterator curPing = mIPAddressList.begin(); curPing != mIPAddressList.end(); curPing++)
            {
                CPing ping((*curPing)->IP.c_str(),1000);
                bool pingSuccess = ping.ping(5);

                if (pingSuccess)
                {
                    (*curPing)->pingCount++;
                }
                //vilslog::printf ("PingDeviceService()::waitloop() IP[%s] pingSuccess[%d] pingCount[%d]\n", (*curPing)->IP.c_str(), pingSuccess, (*curPing)->pingCount);
            }

            mUpdate = false;
        }

        if (waitCount >= 30)
        {
            waitCount = 0;
        }

        waitCount++;
        sleep(5);//wait 5 seconds
    }
}

void PingDeviceService::addPingAddress(const char * IPAddress, int alive)
{
    bool bFound = false;
    for (std::list<PingStatusStruct *>::iterator curPing = mIPAddressList.begin(); curPing != mIPAddressList.end(); curPing++)
    {
        if((*curPing)->IP.compare(IPAddress) == 0)
        {
            (*curPing)->aliveCount = alive;
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        PingStatusStruct * newPing = new PingStatusStruct();
        newPing->IP = IPAddress;
        newPing->aliveCount = alive;
        newPing->pingCount = 0;
        mIPAddressList.push_back(newPing);
        mUpdate = true;
    }
}

int PingDeviceService::getPingCount(const char * IPAddress, bool clear)
{
    int pingCount = 0;
    for (std::list<PingStatusStruct *>::iterator curPing = mIPAddressList.begin(); curPing != mIPAddressList.end(); curPing++)
    {
        if((*curPing)->IP.compare(IPAddress) == 0)
        {
            pingCount = (*curPing)->pingCount;
            if(clear)
                (*curPing)->pingCount = 0;
            break;
        }
    }
    return pingCount;
}
