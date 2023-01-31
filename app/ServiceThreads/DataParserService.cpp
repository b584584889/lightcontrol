/*
 *  DataParserService.cpp
 *
 *  Created on: 2018/10/24
 *      Author: ikki
 */

#include <string.h>
#include <sstream>
#include <iostream>

#include "DataParserService.h"

using namespace std;

DataParserService::DataParserService(wqueue<DataItem*> & UDPJobQueue, wqueue<DataItem*> & PosCalJobQueue, wqueue<SubscribeDataItem*> & pWebSocketqueue, wqueue<NotificationItem*> & NotifiactionQueue, wqueue<WorkItem*> & MySQLJobQueue, DeviceManager * pDevManager)
    : mUDPJobQueue(UDPJobQueue), mPosCalJobQueue(PosCalJobQueue), mWebSocketqueue(pWebSocketqueue), mNotifiactionQueue(NotifiactionQueue), mMySQLJobQueue(MySQLJobQueue), mDeviceManager(pDevManager)
{
}

bool DataParserService::status()
{
    return mInitial;
}

std::string DataParserService::statusString()
{
    return mStatus;
}

void * DataParserService::run()
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
            vilslog::printf("DataParserService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "DataParserService()::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void DataParserService::runloop()
{
    mInitial = false;

    mDataParser = new DataParser(mPosCalJobQueue, mWebSocketqueue, mNotifiactionQueue, mMySQLJobQueue, mDeviceManager);

    if (mDataParser != NULL)
    {
        mStatus = "DataParserService()::run() Initial DataParser successfully!";
        vilslog::printf ("DataParserService()::run() Initial DataParser successfully!\n");
    }
    else
    {
        mStatus = "DataParserService()::run() Initial DataParser failed!!";
        vilslog::printf ("DataParserService()::run() Initial DataParser failed!\n");
        return;
    }

    mInitial = true;

    waitloop();
}

void DataParserService::waitloop()
{
    // int ProcessCount = 0;

    while (1)
    {
        DataItem * item = mUDPJobQueue.remove();

        if ( item != NULL )
        {
            // ProcessCount++;
            // if ((ProcessCount % 100) == 0)
            // {
            //     vilslog::out <<currentDateTimeForSQL() << " DataParserService::waitloop() ProcessCount:"<<ProcessCount<<" mUDPJobQueue.size()="<<mUDPJobQueue.size()<<endl;
            //     vilslog::flush() ;
            // }


            //printf("wait_loop() item:Type[%s]\n", item->getType());
            mDataParser->processData(item);

            delete item;
        }
    }
}
