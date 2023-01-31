/*
 *  TAFServiceLoopThread.h
 *
 *  Created on: 2017/08/03
 *      Author: ikki
 */
#ifndef __TAFServiceLoopThread_h__
#define __TAFServiceLoopThread_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "wqueue.h"
#include "WorkItem.h"
#include "TAFDBServer.h"

#include "../mysql/MySQLDBAccess.h"
#include "../config.h"
#include "../DeviceManager/DeviceManager.h"

using namespace std;

class TAFServiceLoopThread : public Thread
{
    wqueue<WorkItem*> & m_queue;
    TAFDBServer * mTAFDBServer;

public:
    TAFServiceLoopThread(wqueue<WorkItem*> & queue,
        TAFDBServer * pTAFDBServer, VILSConfig * config, DeviceManager * pDevManager);

    void * run();
    bool getCurrentLngLat(double posX, double posY, double posZ,
                double & targetLng, double & targetLat, int & buildingID);

private:
    VILSConfig * mConfig;
    std::list<VILSConfig*> * mConfigList;
    DeviceManager * mDeviceManager;

    int send_updateTAF(WorkItem * item);
    int send_event(WorkItem * item);
};

#endif
