/*
 *  BroadcastDevicesService.cpp
 *
 *  Created on: 2018/10/19
 *      Author: ikki
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <iostream>

#include "BroadcastDevicesService.h"

using namespace std;

BroadcastDevicesService::BroadcastDevicesService(wqueue<DataItem*> & pSendOutJobQueue, DeviceManager * pDevManager, DBManager * dbManager)
    : mSendOutJobQueue(pSendOutJobQueue), mDeviceManager(pDevManager), mDbManager(dbManager)
{
}

bool BroadcastDevicesService::status()
{
    return mInitial;
}

std::string BroadcastDevicesService::statusString()
{
    return mStatus;
}

void * BroadcastDevicesService::run()
{
    runloop();

    return NULL;
}

void BroadcastDevicesService::runloop()
{
    while(1)
    {
        try
        {
            waitloop();
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("BroadcastDevicesService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "BroadcastDevicesService::run() exception:";
            mStatus.append(exc.what());
        }
    }
}

void BroadcastDevicesService::waitloop()
{
    long long t_start = getSystemTime();
    //long long t_all_device = 0;
    long long t_tag_device = 0;
    long long t_anchor_device = 0;
    long long t_coord_device = 0;
    long long t_tag_alive_device = 0;
    long long t_systemtime = 3590000;
    long long updatePrjIdTime = 0;

    mPrjidList = mDbManager->getAllProjectConfig();

    mInitial = true;
    mStatus = "BroadcastDevicesService()::run() Initial successfully!";
    vilslog::printf ("BroadcastDevicesService()::run() Initial successfully!\n");

    while ( 1 )
    {
        long long t_end = getSystemTime();
        long long t_diff = t_end - t_start;

        //t_all_device += t_diff;
        t_tag_device += t_diff;
        t_anchor_device += t_diff;
        t_coord_device += t_diff;
        t_tag_alive_device += t_diff;
        t_systemtime += t_diff;

        // if ( t_all_device >= 10000 ) // 10 seconds
        // {
        //     broadcast_devices();
        //     t_all_device = 0;
        // }
        //
        if ( t_tag_device >= 9000 ) // 9 seconds
        {
            broadcast_tag_devices();
            t_tag_device = 0;
        }

        if ( t_anchor_device >= 10000 ) // 10 seconds
        {
            broadcast_anchor_devices();
            t_anchor_device = 0;
        }

        if ( t_coord_device >= 10000 ) // 13 seconds
        {
            broadcast_coord_devices();
            t_coord_device = 0;
        }

        if ( t_tag_alive_device >= 1000 ) // 1 seconds
        {
            broadcast_alive_tag();
            t_tag_alive_device = 0;
        }

        if ( t_systemtime >= 3600000 ) // 1 hour
        {
            broadcast_systemtime();
            t_systemtime = 0;
        }

        if (updatePrjIdTime > 3600000)
        {
            mPrjidList->clear();
            delete mPrjidList;
            mPrjidList = mDbManager->getAllProjectConfig();

            updatePrjIdTime = 0;
        }

        //sleep(10);
        t_start = t_end;

        usleep(10000);
    }
}

int BroadcastDevicesService::broadcast_devices()
{
    int count = 0;
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        count += broadcast_devices(projectid);
    }
    return count;
}

int BroadcastDevicesService::broadcast_devices(int projectid)
{
    //printf("broadcast_devices()\n");

    // local connected node list
    std::list<NodeStruct*> * anchorNodeList = mDeviceManager->get_Anchor_list(projectid);
    std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);
    std::list<NodeStruct*> * coordNodeList = mDeviceManager->get_Coordinator_list(projectid);
    //std::list<NodeStruct*> * locatorNodeList = mDeviceManager->get_Locator_list(1);

    //printf("BroadcastDevicesService::broadcast_devices() coordNodeList->size()[%lu]\n", coordNodeList->size());
    //printf("BroadcastDevicesService::broadcast_devices() anchorNodeList->size()[%lu]\n", anchorNodeList->size());
    //printf("BroadcastDevicesService::broadcast_devices() tagNodeList->size()[%lu]\n", tagNodeList->size());

    if (anchorNodeList->size() > 0)
    {
        sleep(1);
        DataItem * itemA = new DataItem("list", projectid, NODE_TYPE_ANCHOR, anchorNodeList);
        mSendOutJobQueue.add(itemA);
    }

    if (tagNodeList->size() > 0)
    {
        sleep(1);
        DataItem * itemT = new DataItem("list", projectid, NODE_TYPE_TAG, tagNodeList);
        mSendOutJobQueue.add(itemT);
    }

    if (coordNodeList->size() > 0)
    {
        sleep(1);
        DataItem * itemC = new DataItem("list", projectid, NODE_TYPE_COORD, coordNodeList);
        mSendOutJobQueue.add(itemC);
    }

    // if (locatorNodeList->size() > 0)
    // {
    //     sleep(1);
    //     DataItem * itemC = new DataItem("list", NODE_TYPE_LOCATOR, locatorNodeList);
    //     mSendOutJobQueue.add(itemC);
    // }

    return 0;
}

int BroadcastDevicesService::broadcast_tag_devices()
{
    int count = 0;
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        count += broadcast_tag_devices(projectid);
    }
    return count;
}

int BroadcastDevicesService::broadcast_tag_devices(int projectid)
{
    std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);

    if (tagNodeList->size() > 0)
    {
        sleep(1);
        DataItem * itemT = new DataItem("list", projectid, NODE_TYPE_TAG, tagNodeList);
        mSendOutJobQueue.add(itemT);
    }

    return 0;
}

int BroadcastDevicesService::broadcast_anchor_devices()
{
    int count = 0;
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        count += broadcast_anchor_devices(projectid);
    }
    return count;
}

int BroadcastDevicesService::broadcast_anchor_devices(int projectid)
{
    std::list<NodeStruct*> * anchorNodeList = mDeviceManager->get_Anchor_list(projectid);

    if (anchorNodeList->size() > 0)
    {
        sleep(1);
        DataItem * itemA = new DataItem("list", projectid, NODE_TYPE_ANCHOR, anchorNodeList);
        mSendOutJobQueue.add(itemA);
    }

    return 0;
}

int BroadcastDevicesService::broadcast_coord_devices()
{
    int count = 0;
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        count += broadcast_coord_devices(projectid);
    }
    return count;
}

int BroadcastDevicesService::broadcast_coord_devices(int projectid)
{
    std::list<NodeStruct*> * coordNodeList = mDeviceManager->get_Coordinator_list(projectid);

    if (coordNodeList->size() > 0)
    {
        sleep(1);
        DataItem * itemC = new DataItem("list", projectid, NODE_TYPE_COORD, coordNodeList);
        mSendOutJobQueue.add(itemC);
    }

    return 0;
}

int BroadcastDevicesService::broadcast_alive_tag()
{
    int count = 0;
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        count += broadcast_alive_tag(projectid);
    }
    return count;
}

int BroadcastDevicesService::broadcast_alive_tag(int projectid)
{
    std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);
    if (tagNodeList->size() > 0)
    {
        DataItem * itemT = new DataItem("alivetag", projectid, NODE_TYPE_TAG, tagNodeList);
        mSendOutJobQueue.add(itemT);
    }
    return 0;
}

int BroadcastDevicesService::broadcast_systemtime()
{
    int count = 0;
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        count += broadcast_systemtime(projectid);
    }
    return count;
}

int BroadcastDevicesService::broadcast_systemtime(int projectid)
{
    DataItem * itemT = new DataItem("systemtime", projectid, 1, currentDateTimeForSQL().c_str());
    mSendOutJobQueue.add(itemT);
    return 0;
}
