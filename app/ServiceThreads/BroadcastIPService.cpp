/*
 *  BroadcastIPService.cpp
 *
 *  Created on: 2018/10/19
 *      Author: ikki
 */


#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <iostream>
#include "BroadcastIPService.h"


using namespace std;

BroadcastIPService::BroadcastIPService(DBManager * dbManager)
    : mDbManager(dbManager)
{
}

bool BroadcastIPService::status()
{
    return mInitial;
}

std::string BroadcastIPService::statusString()
{
    return mStatus;
}

void * BroadcastIPService::run()
{
    while(1)
    {
        VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(1);
        m_center_broadcast_port = vilssysConfig->center_broadcast_port;

        try
        {
            runloop();

            sleep(10);
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("BroadcastIPService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "BroadcastIPService::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void BroadcastIPService::runloop()
{
    mInitial = false;

    getMyCenterNetworkInfo(mCenterNetworkInfoList);

    for (std::list<NetworkInfoStruct * >::iterator netif = mCenterNetworkInfoList.begin(); netif != mCenterNetworkInfoList.end(); netif++)
    {
        MulticastClient * broadcastIPclient = new MulticastClient((*netif)->interface, (*netif)->ip);
        if ( broadcastIPclient->init(m_center_broadcast_port) )
        {
            vilslog::printf ("BroadcastIPService()::run() Initial MulticastClient successfully! name[%s] IP[%s]\n",
                (*netif)->interface.c_str(), (*netif)->ip.c_str());
            mBroadcastIPclientList.push_back(broadcastIPclient);
        }
        else
        {
            vilslog::printf ("BroadcastIPService()::run() Initial MulticastClient failed! name[%s] IP[%s]\n",
                (*netif)->interface.c_str(), (*netif)->ip.c_str());
        }
    }

    if (mBroadcastIPclientList.size() == 0)
    {
        mStatus = "BroadcastIPService()::run() Initial BroadcastIPService failed!";
        vilslog::printf ("BroadcastIPService() Initial BroadcastIPService failed!\n");
        clear_NetworkInfo_list(&mCenterNetworkInfoList);
        return;
    }
    else
    {
        mStatus = "BroadcastIPService()::run() Initial BroadcastIPService successfully!";
        vilslog::printf ("BroadcastIPService()::run() Initial BroadcastIPService successfully!\n");
    }

    // mBroadcastIPclient = new MulticastClient();
    // if ( mBroadcastIPclient->init(m_center_broadcast_port) )
    // {
    //     mStatus = "BroadcastIPService()::run() Initial BroadcastIPService successfully!";
    //     vilslog::printf ("BroadcastIPService()::run() Initial BroadcastIPService successfully!\n");
    // }
    // else
    // {
    //     mStatus = "BroadcastIPService()::run() Initial BroadcastIPService failed!";
    //     vilslog::printf ("BroadcastIPService() Initial BroadcastIPService failed!\n");
    //     delete mBroadcastIPclient;
    //     return NULL;
    // }


    mInitial = true;

    waitloop();

}

void BroadcastIPService::waitloop()
{
    //char IP[32];
    //getMyCenterIP(IP);
    //vilslog::printf("My Center IP:[%s] center_broadcast_port[%d]\n", IP, m_center_broadcast_port);

    //int count = getMyCenterIPs(mCenterIPList);

    char sendBuf[64];

    while ( 1 )
    {
        for (std::list<MulticastClient *>::iterator broadcastIPclient = mBroadcastIPclientList.begin(); broadcastIPclient != mBroadcastIPclientList.end(); broadcastIPclient++)
        {
            memset(sendBuf, 0, 64);
            std::string interfacename = (*broadcastIPclient)->getInterfaceName();

            bool bFound = false;
            for (std::list<NetworkInfoStruct * >::iterator netif = mCenterNetworkInfoList.begin(); netif != mCenterNetworkInfoList.end(); netif++)
            {
                if ( (*netif)->interface.compare(interfacename) == 0 )
                {
                    sprintf(sendBuf, "%s", (*netif)->ip.c_str());
                    bFound = true;
                    break;
                }
            }
            if (!bFound)
            {
                continue;
            }

            int ret = (*broadcastIPclient)->send(sendBuf, (int)strlen(sendBuf));
            //printf("BroadcastIPService::waitloop interfacename[%s] sendBuf[%s]\n", interfacename.c_str(), sendBuf);
            if (ret < 0)
            {
                vilslog::printf("BroadcastIPService::waitloop send failed!!! interfacename[%s]\n", interfacename.c_str());
            }
            sleep(1);
        }


        // for (std::list<std::string>::iterator ip = mCenterIPList.begin(); ip != mCenterIPList.end(); ip++)
        // {
        //     memset(sendBuf, 0, 64);
        //     sprintf(sendBuf, "%s", (*ip).c_str());
        //
        //     int ret = mBroadcastIPclient->send(sendBuf, (int)strlen(sendBuf));
        //     vilslog::printf("BroadcastIPService::waitloop sendBuf[%s]\n", sendBuf);
        //     if (ret < 0)
        //         break;
        //     sleep(1);
        // }


        sleep(5);//wait 5 second
    }
    //delete mBroadcastIPclient;
//    clear_NetworkInfo_list(&mCenterNetworkInfoList);
//    clear_MulticastClient_list(&mBroadcastIPclientList);
//
}
