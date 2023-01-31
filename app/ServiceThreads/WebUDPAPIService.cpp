/*
 *  WebUDPAPIService.cpp
 *
 *  Created on: 2018/10/18
 *      Author: ikki
 */

#include <string.h>
#include <sstream>
#include <iostream>

#include "WebUDPAPIService.h"

using namespace std;

WebUDPAPIService::WebUDPAPIService(wqueue<UDPDataItem*> & udpqueue, DBManager * dbManager)
    : m_udpqueue(udpqueue), mDbManager(dbManager)
{
}

bool WebUDPAPIService::status()
{
    return mUdpInitial;
}

std::string WebUDPAPIService::statusString()
{
    return mStatus;
}

void * WebUDPAPIService::run()
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
            vilslog::printf("WebUDPAPIService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mUdpInitial = false;
            mStatus = "WebUDPAPIService::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void WebUDPAPIService::runloop()
{
    mUdpInitial = false;
    mWebUDPServer = new UDPServer();

    VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(1);

    int UDPretryCount = 0;
    while ( !mUdpInitial && UDPretryCount < 12 )
    {
        mUdpInitial = mWebUDPServer->init(vilssysConfig->web_udp_port);
        UDPretryCount++;
        if (!mUdpInitial)
        {
            vilslog::printf("WebUDPAPIService::run() mUdpInitial = %d, retryCount = %d\n", mUdpInitial, UDPretryCount);
            sleep(3);
        }
    }

    if ( mUdpInitial )
    {
        mStatus = "WebUDPAPIService()::run() Initial UDPServer successfully!";
        vilslog::printf ("WebUDPAPIService()::run() Initial UDPServer successfully! web_udp_port[%d]\n", vilssysConfig->web_udp_port);
    }
    else
    {
        mStatus = "WebUDPAPIService()::run() Initial UDPServer failed!!";
        vilslog::printf ("WebUDPAPIService()::run() Initial UDPServer failed!\n");
        delete mWebUDPServer;
        return;
    }

    waitloop();

}

void WebUDPAPIService::waitloop()
{
    char buf[MAXUDPPACKET];
    char clientIP[64];
    int length = 0;
    // int ProcessCount = 0;

    if (mWebUDPServer == NULL)
        return ;

    while ( 1 )
    {
        memset(buf, 0, MAXUDPPACKET);
        memset(clientIP, 0, 64);

        UDPDataItem * item = new UDPDataItem("webudpapi");

        // length = mWebUDPServer->waitClient(buf, &clientIP[0]);
        length = mWebUDPServer->waitClient(buf, (struct sockaddr*)&item->m_client_addr);
        if (length < 0)
        {
            delete item;
            continue;
        }

        // UDPDataItem * item = new UDPDataItem("webudpapi", buf, length, clientIP);

        //vilslog::printf ("WebUDPAPIService()::run() buf[%s]\n", buf);

        item->setMsg(buf, length);
        item->mUDPServer = mWebUDPServer;
        m_udpqueue.add(item);

    }
    //delete mWebUDPServer;

    //vilslog::printf ("\n\nWebUDPAPIService()::run() Findished!! \n\n\n");

}
