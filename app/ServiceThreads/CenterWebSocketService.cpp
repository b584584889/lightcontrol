/*
 *  CenterWebSocketService.cpp
 *
 *  Created on: 2018/09/25
 *      Author: ikki
 */

#include "CenterWebSocketService.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <string.h>
#include <list>
#include <ifaddrs.h>


CenterWebSocketService::CenterWebSocketService(wqueue<SubscribeDataItem*> & queue, DBManager * dbManager)
    : m_queue(queue), mDbManager(dbManager)
{

}

bool CenterWebSocketService::status()
{
    return mInitial;
}

std::string CenterWebSocketService::statusString()
{
    return mStatus;
}

void * CenterWebSocketService::run()
{
    int error_count = 0;
    while (1)
    {
        try
        {
            initQueue();
            runloop();
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            error_count++;
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("CenterWebSocketService::run() exception error_count[%d]\n", error_count);
            std::string error_string = exc.what();
            error_string.append("\n");
            vilslog::printf(error_string.c_str());
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "CenterWebSocketService()::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void CenterWebSocketService::initQueue()
{
    std::string dft_rtn = "none";
    int queue_size1 = 256;

    mEventSubscriber = new ITPS::Subscriber<SubscribeDataItem*>("WebSocket", "event", queue_size1);

    while(!mEventSubscriber->subscribe() )
    {
        vilslog::printf ("CenterWebSocketService()::waitloop() subscribe()\n");
        sleep(1);
    }
}

void CenterWebSocketService::runloop()
{
    mInitial = true;
    mStatus = "CenterWebSocketService()::runloop() Initial CenterWebSocketService successfully!";
    SubscribeDataItem * dft_rtn = NULL;
    int count = 0;

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(1);
    if (baseDb != NULL)
        baseDb->read_sys_config(&m_systemConfig);

    vilslog::printf ("CenterWebSocketService()::run() Initial CenterWebSocketService successfully! web_socket_port[%d]\n", m_systemConfig.web_socket_port);
    m_server = new WebSocketServer(m_systemConfig.web_socket_port);
    m_server->start();

    // available to process.
    while (1)
    {
        mInitial = m_server->mInitial;
        mStatus = m_server->mStatus;

        //printf("thread %lu, - waiting for item...\n", (long unsigned int)self());
        SubscribeDataItem * item = m_queue.removetimed(1);
        if( item != NULL )
        {
            //printf("CenterWebSocketService::run() item:m_msgType[%s]\n", item->m_msgType.c_str());
            count++;
            m_server->setDataItem(item);
            delete item;
        }

        SubscribeDataItem * item1 = mEventSubscriber->pop_msg(1, dft_rtn);
        if ( item1 != NULL )
        {
            count++;
            // vilslog::printf("CenterWebSocketService::run() mEventSubscriber item1:m_msgType[%s]\n", item1->m_msgType.c_str());
            m_server->setDataItem(item1);
            delete item1;
        }

        if (count == 0)
        {
            usleep(100000);
        }
        count = 0;
        //printf("thread %lu, loop %d - item: Type - %s, NodeId - %s\n",
        //      (long unsigned int)self(), i, item->getType(), item->getNodeId());
    }
}
