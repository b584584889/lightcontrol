/*
 *  CoordIncomingService.cpp
 *
 *  Created on: 2017/02/21
 *      Author: ikki
 */

#include <string.h>

#include "CoordIncomingService.h"
#include "../json/json.h"
#include "../json/JSONAgent.h"


using namespace std;

CoordIncomingService::CoordIncomingService(wqueue<UDPDataItem*> & udpqueue, DBManager * dbManager)
    : m_udpqueue(udpqueue), mDbManager(dbManager)
{

}

bool CoordIncomingService::status()
{
    return mUdpInitial;
}

std::string CoordIncomingService::statusString()
{
    return mStatus;
}

void * CoordIncomingService::run()
{
    while (1)
    {
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
            vilslog::printf("CoordIncomingService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mUdpInitial = false;
            mStatus = "CoordIncomingService()::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void CoordIncomingService::runloop()
{
    mUdpInitial = false;

    getMyCenterNetworkInfo(mCenterNetworkInfoList);
    vilslog::printf ("CoordIncomingService()::runloop() mCenterNetworkInfoList.length()[%lu]\n", mCenterNetworkInfoList.size());

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(1);
    if (baseDb != NULL)
        baseDb->read_sys_config(&m_systemConfig);

    mUdpServerList = createMultipleIfServer(m_systemConfig.center_udp_port);

    if ( (int)mUdpServerList->size() > 0 )
    {
        mUdpInitial = true;
        mStatus = "CoordIncomingService()::runloop() Initial Coord UDPServer successfully!";
        vilslog::printf ("CoordIncomingService()::runloop() Initial Coord UDPServer(%d) successfully!\n", (int)mUdpServerList->size());
    }
    else
    {
        mStatus = "CoordIncomingService()::runloop() Initial Coord UDPServer failed!!";
        vilslog::printf ("CoordIncomingService()::runloop() Initial Coord UDPServer failed!\n");
        return;
    }

    waitloop();

}

std::list<UDPServer *> * CoordIncomingService::createMultipleIfServer(int port)
{
    std::list<UDPServer *> * serverList = new std::list<UDPServer *>();

    for (std::list<NetworkInfoStruct * >::iterator netif = mCenterNetworkInfoList.begin(); netif != mCenterNetworkInfoList.end(); netif++)
    {
        UDPServer * udpserver = new UDPServer();
        int UDPretryCount = 0;
        bool udpInitial = false;
        while ( !udpInitial && UDPretryCount < 12 )
        {
            udpInitial = udpserver->init((*netif)->ip.c_str(), port);
            UDPretryCount++;
            if (!udpInitial)
            {
                vilslog::printf("CoordIncomingService::createMultipleIfServer(%d) udpInitial = %d, retryCount = %d\n",
                    port, udpInitial, UDPretryCount);
                sleep(3);
            }
        }

        if ( udpInitial )
        {
            vilslog::printf ("CoordIncomingService()::createMultipleIfServer(%d) Initial UDPServer successfully! name[%s] IP[%s]\n",
                port, (*netif)->interface.c_str(), (*netif)->ip.c_str());

            serverList->push_back(udpserver);
        }
    }
    return serverList;
}

void CoordIncomingService::waitloop()
{
    char buf[MAXUDPPACKET];
    char clientIP[64];

    fd_set rset;
    int maxfdp = -1;

    // clear the descriptor set
    FD_ZERO(&rset);

    for (std::list<UDPServer *>::iterator udpserver = mUdpServerList->begin(); udpserver != mUdpServerList->end(); udpserver++)
    {
        int fd = (*udpserver)->getsocketfd();
        vilslog::printf ("CoordIncomingService()::waitloop() mUdpServerList dp[%d]\n", fd);
        if (fd > maxfdp)
        {
            maxfdp = fd;
        }
    }

    vilslog::printf ("CoordIncomingService()::waitloop() maxfdp[%d]\n", maxfdp);

    while (1)
    {
        memset(buf, 0, MAXUDPPACKET);
        memset(clientIP, 0, 64);

        //printf ("CoordIncomingService()::run() mCoordUDPServer->waitClient()!\n");

        for (std::list<UDPServer *>::iterator udpserver = mUdpServerList->begin(); udpserver != mUdpServerList->end(); udpserver++)
        {
            // set lfd in readset
            int fd = (*udpserver)->getsocketfd();
            FD_SET(fd, &rset);
        }

        int length = 0;

        // select the ready descriptor
        int activity = select(maxfdp + 1, &rset, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR))
        {
            vilslog::printf ("CoordIncomingService()::waitloop() select error! activity[%d]\n", activity);
            continue;
        }

        // vilslog::printf ("CoordIncomingService()::waitloop() activity[%d]\n", activity);

        for (std::list<UDPServer *>::iterator udpserver = mUdpServerList->begin(); udpserver != mUdpServerList->end(); udpserver++)
        {
            // if udp socket is readable receive the message.
            int fd = (*udpserver)->getsocketfd();
            if (FD_ISSET(fd, &rset))
            {
                length = (*udpserver)->waitClient(buf, &clientIP[0]);
                break;
            }
        }

        if (length <= 0)
        {
            continue;
        }

        //vilslog::printf ("CoordIncomingService()::run() length[%d] \nbuf[%s]\n", length, buf);

        // long long end1 = getSystemTime();
        // long long difft = end1 - start1;
        // vilslog::printf("CoordIncomingService::wait_loop() time[%lld ms]\n", difft);

        UDPDataItem * item = new UDPDataItem("coordmsg", buf, length, clientIP);
        m_udpqueue.add(item);
    }
}
