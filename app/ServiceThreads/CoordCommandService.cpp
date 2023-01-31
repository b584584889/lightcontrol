/*
 *  CoordCommandService.cpp
 *
 *  Created on: 2019/08/05
 *      Author: ikki
 */

#include <string.h>

#include "CoordCommandService.h"
#include "../json/json.h"
#include "../json/JSONAgent.h"
#include "../network/UDPClient.h"

using namespace std;

CoordCommandService::CoordCommandService(DBManager * dbManager)
    : mDbManager(dbManager)
{

}

bool CoordCommandService::status()
{
    return mUdpInitial;
}

std::string CoordCommandService::statusString()
{
    return mStatus;
}

void * CoordCommandService::run()
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
            vilslog::printf("CoordCommandService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mUdpInitial = false;
            mStatus = "CoordCommandService()::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void CoordCommandService::runloop()
{
    mUdpInitial = false;

    getMyCenterNetworkInfo(mCenterNetworkInfoList);
    vilslog::printf ("CoordCommandService()::runloop() mCenterNetworkInfoList.length()[%lu]\n", mCenterNetworkInfoList.size());

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(1);
    if (baseDb != NULL)
        baseDb->read_sys_config(&m_systemConfig);

    mCoordCMDServerList = createMultipleIfServer(m_systemConfig.center_broadcast_port);
    //mCoordCMDServerList = createMultipleIfServer(12317);

    if ( (int)mCoordCMDServerList->size() > 0 )
    {
        mUdpInitial = true;
        mStatus = "CoordCommandService()::runloop() Initial Coord UDPServer successfully!";
        vilslog::printf ("CoordCommandService()::runloop() Initial Coord CMDServer(%d) port[%d] successfully!\n",
            (int)mCoordCMDServerList->size(), m_systemConfig.center_broadcast_port);
    }
    else
    {
        mStatus = "CoordCommandService()::runloop() Initial Coord UDPServer failed!!";
        vilslog::printf ("CoordCommandService()::runloop() Initial Coord CMDServer failed!\n");
        return;
    }

    waitloop();

}

void CoordCommandService::updateconfig()
{
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(1);
    if (baseDb != NULL)
        baseDb->read_sys_config(&m_systemConfig);
}

std::list<UDPServer *> * CoordCommandService::createMultipleIfServer(int port)
{
    std::list<UDPServer *> * serverList = new std::list<UDPServer *>();

    for (std::list<NetworkInfoStruct * >::iterator netif = mCenterNetworkInfoList.begin(); netif != mCenterNetworkInfoList.end(); netif++)
    {
        UDPServer * udpserver = new UDPServer();
        int UDPretryCount = 0;
        bool udpInitial = false;
        while ( !udpInitial && UDPretryCount < 12 )
        {
            //udpInitial = udpserver->init((*netif)->ip.c_str(), port);
            udpInitial = udpserver->init(port);
            UDPretryCount++;
            if (!udpInitial)
            {
                vilslog::printf("CoordCommandService::createMultipleIfServer(%d) udpInitial = %d, retryCount = %d\n",
                    port, udpInitial, UDPretryCount);
                sleep(3);
            }
        }

        if ( udpInitial )
        {
            vilslog::printf ("CoordCommandService()::createMultipleIfServer(%d) Initial UDPServer successfully! name[%s] IP[%s]\n",
                port, (*netif)->interface.c_str(), (*netif)->ip.c_str());

            serverList->push_back(udpserver);
        }
    }
    return serverList;
}

void CoordCommandService::waitloop()
{
    char buf[MAXUDPPACKET];
    char clientIP[64];

    fd_set rset;
    int maxfdp = -1;

    // clear the descriptor set
    FD_ZERO(&rset);

    for (std::list<UDPServer *>::iterator udpserver = mCoordCMDServerList->begin(); udpserver != mCoordCMDServerList->end(); udpserver++)
    {
        int fd = (*udpserver)->getsocketfd();
        vilslog::printf ("CoordCommandService()::waitloop() mCoordCMDServerList dp[%d]\n", fd);
        if (fd > maxfdp)
        {
            maxfdp = fd;
        }
    }

    vilslog::printf ("CoordCommandService()::waitloop() maxfdp[%d]\n", maxfdp);

    while (1)
    {
        memset(buf, 0, MAXUDPPACKET);
        memset(clientIP, 0, 64);

        //printf ("CoordCommandService()::run() mCoordUDPServer->waitClient()!\n");

        for (std::list<UDPServer *>::iterator udpserver = mCoordCMDServerList->begin(); udpserver != mCoordCMDServerList->end(); udpserver++)
        {
            // set lfd in readset
            int fd = (*udpserver)->getsocketfd();
            FD_SET(fd, &rset);
        }

        int length = 0;

        // wait socket event
        // select the ready descriptor
        //
        int activity = select(maxfdp + 1, &rset, NULL, NULL, NULL);

        if ((activity < 0) && (errno != EINTR))
        {
            vilslog::printf ("CoordCommandService()::waitloop() select error! activity[%d]\n", activity);
            continue;
        }

        //vilslog::printf ("CoordCommandService()::waitloop() activity[%d]\n", activity);

        for (std::list<UDPServer *>::iterator udpserver = mCoordCMDServerList->begin(); udpserver != mCoordCMDServerList->end(); udpserver++)
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
            printf ("CoordCommandService()::run() length[%d]\n", length);
            continue;
        }

        vilslog::printf ("CoordCommandService()::run() clientIP[%s] length[%d] \nbuf[%s]\n", clientIP, length, buf);

        process(clientIP, length, buf);

    }
}

int CoordCommandService::process(const char * clientIP, int length, const char * buf)
{
    Json::Reader * reader = new Json::Reader();
    Json::Value JSONresult;
    bool parsingSuccessful = false;
    char type[64];

    try{
        parsingSuccessful = reader->parse( buf, JSONresult);
    }
    catch(...)
    {
        vilslog::printf ("CoordCommandService::process() reader->parse() catch exception...\n");
    }

    if (parsingSuccessful)
    {
        if ( JSONresult.isMember("type") )
        {
            sprintf(type, "%s", JSONresult["type"].asCString());

            vilslog::printf ("CoordCommandService::process() type[%s]\n", type);
            if ( strcmp(type, "boot") == 0 )
            {
                do_boot(clientIP, JSONresult);
            }
        }
    }

    delete reader;
    return 0;
}

int CoordCommandService::do_boot(const char * clientIP, Json::Value JSONresult)
{
    //
    // {"type":"boot","coord2cen":"18624","cen2coord":"18615"}
    //
    int coord2cen = 0;
    int cen2coord = 0;
    if ( JSONresult.isMember("coord2cen") )
    {
        //vilslog::printf ("CoordCommandService::process() coord2cen[%s]\n", JSONresult["coord2cen"].asCString());
        coord2cen = atoi(JSONresult["coord2cen"].asCString());
    }
    if ( JSONresult.isMember("cen2coord") )
    {
        //vilslog::printf ("CoordCommandService::process() cen2coord[%s]\n", JSONresult["cen2coord"].asCString());
        cen2coord = atoi(JSONresult["cen2coord"].asCString());
    }

    vilslog::printf ("CoordCommandService::process() coord2cen[%d] cen2coord[%d]\n", coord2cen, cen2coord);
    vilslog::printf ("CoordCommandService::process() center_udp_port[%d] coord_udp_port[%d]\n", m_systemConfig.center_udp_port, m_systemConfig.coord_udp_port);

    if ( coord2cen != m_systemConfig.center_udp_port || cen2coord != m_systemConfig.coord_udp_port)
    {
        sleep(2);
        send_back_server_port(clientIP, cen2coord);
    }
    else
    {
        //sleep(2);
        //send_back_server_port(clientIP, cen2coord);
    }

    return 0;
}

int CoordCommandService::send_back_server_port(const char * clientIP, int target_port)
{
    vilslog::printf ("CoordCommandService::send_back_server_port() clientIP[%s] target_port[%d]\n", clientIP, target_port);

    char mBuf[MAXUDPPACKET];
    memset(mBuf, 0, MAXUDPPACKET);
    sprintf(mBuf, "{\"type\":\"setport\",\"param1\":\"%d\",\"param2\":\"%d\"}\r\n",
            m_systemConfig.center_udp_port, m_systemConfig.coord_udp_port);

    vilslog::printf ("CoordCommandService::send_back_server_port() mBuf[%s]\n", mBuf);

    for (std::list<NetworkInfoStruct * >::iterator netif = mCenterNetworkInfoList.begin(); netif != mCenterNetworkInfoList.end(); netif++)
    {
        //UDPClient * udpClient = new UDPClient((*netif)->interface, (*netif)->ip);
        UDPClient * udpClient = new UDPClient((*netif)->interface, "");
        bool udpInitial = udpClient->init(target_port, clientIP);
        if ( udpInitial )
        {
            //
            // send out
            //
            int ret = udpClient->send(mBuf, (int)strlen(mBuf));


            vilslog::printf ("CoordCommandService()::send_back_server_port() name[%s] IP[%s] clientIP[%s] target_port[%d] ret[%d]\n",
                (*netif)->interface.c_str(), (*netif)->ip.c_str(), clientIP, target_port, ret);


        }
        delete udpClient;
    }

    return 0;
}
