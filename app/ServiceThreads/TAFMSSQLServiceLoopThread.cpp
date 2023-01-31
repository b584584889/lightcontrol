/*
 *  TAFMSSQLServiceLoopThread.cpp
 *
 *  Created on: 2017/06/15
 *      Author: ikki
 */

#include <string.h>
#include <ctime>


#include "TAFMSSQLServiceLoopThread.h"
#include "../common.h"

using namespace std;

TAFMSSQLServiceLoopThread::TAFMSSQLServiceLoopThread(wqueue<WorkItem*> & queue, VILSConfig * config, DeviceManager * pDevManager)
    : m_queue(queue), mConfig(config), mDeviceManager(pDevManager)
{
    mssqldb = new TAFMSSQLDBAccess();
}

TAFMSSQLServiceLoopThread::~TAFMSSQLServiceLoopThread()
{
   delete mssqldb;
}

void * TAFMSSQLServiceLoopThread::run()
{
    // Remove 1 item at a time and process it. Blocks if no items are
    char type[64];

    // prepare configs
    MySQLDBAccess * mysqldb = new MySQLDBAccess();
    mConfigList = mysqldb->read_all_config();
    delete mysqldb;

    if (mConfigList != NULL)
    {
        vilslog::printf("thread TAFMSSQLServiceLoopThread, mConfigList->size[%lu]\n", mConfigList->size());
    }

    // available to process.
    while (1)
    {
        //printf("thread TAFMSSQLServiceLoopThread, waiting for item...\n");
        WorkItem * item = m_queue.remove();

        sprintf(type, "%s", item->getType());
        if ( strcmp(type, "updateTAF") == 0)
            send_updateTAF(item);

        delete item;
    }

    if (mConfigList != NULL)
    {
        mConfigList->clear();
        delete mConfigList;
    }
    return NULL;
}

//  Room 106
//  Origin WGS48 X value : 121.53861237
//  Origin WGS48 Y value : 25.04019547
//  Building Origin X value : 121.53845978
//  Building Origin Y value : 25.04019356
//
//  TAF enterance
//  Building Origin X value : 121.538111
//  Building Origin Y value : 25.040074
//  X Length : 0.0005
//  Y Length : 0.0001
//
int TAFMSSQLServiceLoopThread::send_updateTAF(WorkItem * item)
{
    int ret = 0;
    long long start1 = getSystemTime();

    //printf("TAFMSSQLServiceLoopThread::send_updateTAF() start1[%lld]\n", start1);


    //double lng = centimeterTolatLng(item->getPosX());
    //double lat = centimeterTolatLng(item->getPosY());
    //double targetLng = mConfig->WGS48_Origin_X + lng;
    //double targetLat = mConfig->WGS48_Origin_Y + lat;
    double lng = 0.0;
    double lat = 0.0;
    double targetLng = 0.0;
    double targetLat = 0.0;
    int buildingID = 99;
    int buildingFloor = 1;

    //printf("item->getPosZ()[%f]\n", item->getPosZ());

    //
    // find building id and floor
    //
    bool bFound = false;
    float * p = new float[2];
    if (mConfigList != NULL)
    {
        for (std::list<VILSConfig*>::iterator config = mConfigList->begin(); config != mConfigList->end(); config++)
        {
            //
            // rotate to correct north
            //
            rotate_point(item->getPosX(), item->getPosY(), 0, 0, (*config)->MAP_North, p);
            //printf("rotate_point (%f,%f) --> [%f] --> (%f,%f)\n", item->getPosX(), item->getPosY(), (*config)->MAP_North, p[0], p[1]);

            double top_edge = (*config)->Building_Origin_Y + (*config)->Building_Y_Length;
            double down_edge = (*config)->Building_Origin_Y;
            double right_edge = (*config)->Building_Origin_X + (*config)->Building_X_Length;
            double left_edge = (*config)->Building_Origin_X;
            lng = centimeterTolatLng((int)p[0]);
            lat = centimeterTolatLng((int)p[1]);
            targetLng = mConfig->WGS48_Origin_X + lng;
            targetLat = mConfig->WGS48_Origin_Y + lat;

            // vilslog::printf("left_edge[%f] right_edge[%f] MAP_North[%f]\n", left_edge, right_edge, (*config)->MAP_North);
            // vilslog::printf("down_edge[%f] top_edge[%f]\n", down_edge, top_edge);
            // vilslog::printf("targetLng[%f] targetLat[%f] \n", targetLng, targetLat);

            if (targetLng >= left_edge && targetLng <= right_edge)
            {
                if (targetLat >= down_edge && targetLat <= top_edge)
                {
                    bFound = true;
                }
            }
            if (bFound)
            {
                buildingID = (*config)->BuildingID;
                if ( item->getPosZ() >= 300 )
                {
                    buildingFloor = 2;
                }
                //buildingFloor = (*config)->BuildingFloor;
                break;
            }
        }
    }
    delete [] p;
    //printf("TAFMSSQLServiceLoopThread::send_updateTAF() (%s) x[%f] y[%f] lat[%f] lat[%f] WGS48_Origin_X[%f] WGS48_Origin_Y[%f]\n",
    //           item->getNodeId(), item->getPosX(), item->getPosY(), lng, lat, mConfig->WGS48_Origin_X, mConfig->WGS48_Origin_Y);
    // vilslog::printf("TAFMSSQLServiceLoopThread::send_updateTAF() bFound[%d] TAFID[%d] BuildingId[%d] Floor[%d] targetLng[%f] targetLat[%f]\n",
    //     bFound, item->getTAFID(), buildingID, buildingFloor, targetLng, targetLat);

    //mssqldb->write_device_footprint(100, 121.5379560 + lng, 25.0401453 + lat, 4326, 99, 1);
    mssqldb->write_device_footprint(item->getTAFID(), targetLng, targetLat, 4326, buildingID, buildingFloor);

    mDeviceManager->setTagInfoB(item->getTAFID(), targetLng, targetLat, 4326, buildingID, buildingFloor);

    long long end1 = getSystemTime();
    long long difft = end1 - start1;
    vilslog::printf("TAFMSSQLServiceLoopThread::send_updateTAF() time: %lld ms\n", difft);

    return ret;
}
