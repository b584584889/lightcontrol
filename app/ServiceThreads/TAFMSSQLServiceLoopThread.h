/*
 *  TAFMSSQLServiceLoopThread.h
 *
 *  Created on: 2017/06/16
 *      Author: ikki
 */
#ifndef __TAFMSSQLServiceLoopThread_h__
#define __TAFMSSQLServiceLoopThread_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Thread.h"
#include "../network/wqueue.h"
#include "../network/WorkItem.h"
#include "../mysql/MySQLDBAccess.h"
#include "../mysql/TAFMSSQLDBAccess.h"

#include "../config.h"
#include "../DeviceManager/DeviceManager.h"

using namespace std;


class TAFMSSQLServiceLoopThread : public Thread
{
    wqueue<WorkItem*> & m_queue;

public:
    TAFMSSQLServiceLoopThread(wqueue<WorkItem*> & queue, VILSConfig * config, DeviceManager * pDevManager);
    ~TAFMSSQLServiceLoopThread();

    void * run();

private:
    VILSConfig * mConfig;
    TAFMSSQLDBAccess * mssqldb;
    std::list<VILSConfig*> * mConfigList;
    DeviceManager * mDeviceManager;


    int send_updateTAF(WorkItem * item);
};

#endif
