/*
 *  TAFServiceLoopThread.cpp
 *
 *  Created on: 2017/08/03
 *      Author: ikki
 */

#include "TAFServiceLoopThread.h"

using namespace std;

TAFServiceLoopThread::TAFServiceLoopThread(wqueue<WorkItem*> & queue,
        TAFDBServer * pTAFDBServer, VILSConfig * config, DeviceManager * pDevManager)
    : m_queue(queue), mTAFDBServer(pTAFDBServer), mConfig(config), mDeviceManager(pDevManager)
{

}

void * TAFServiceLoopThread::run()
{
    // Remove 1 item at a time and process it. Blocks if no items are
    char type[64];


    // prepare configs
    MySQLDBAccess * mysqldb = new MySQLDBAccess();
    mConfigList = mysqldb->read_all_config();
    delete mysqldb;

    if (mConfigList != NULL)
    {
        vilslog::printf("thread TAFServiceLoopThread, mConfigList->size[%lu]\n", mConfigList->size());
    }

    // available to process.
    while (1)
    {
        //printf("thread %lu, loop %d - waiting for item...\n", (long unsigned int)self(), i);
        WorkItem * item = m_queue.remove();

        //printf("thread %lu, loop %d - item: Type - %s, NodeId - %s\n",
        //      (long unsigned int)self(), i, item->getType(), item->getNodeId());

        sprintf(type, "%s", item->getType());
        if ( strcmp(type, "updateTAF") == 0)
            send_updateTAF(item);
        else
        if ( strcmp(type, "eventTAF") == 0)
            send_event(item);

        //printf("thread %lu, loop %d - got one item\n", (long unsigned int)self(), i);
        delete item;
    }

    if (mConfigList != NULL)
    {
        mConfigList->clear();
        delete mConfigList;
    }

    return NULL;
}

bool TAFServiceLoopThread::getCurrentLngLat(double posX, double posY, double posZ,
                        double & targetLng, double & targetLat, int & buildingID)
{
    bool bFound = false;
    double lng = 0.0;
    double lat = 0.0;
    targetLng = 0.0;
    targetLat = 0.0;
    buildingID = 16;
    //buildingFloor = 1;

    //
    // find building id and floor
    //
    float * p = new float[2];
    if (mConfigList != NULL)
    {
        for (std::list<VILSConfig*>::iterator config = mConfigList->begin(); config != mConfigList->end(); config++)
        {
            //
            // rotate to correct north
            //
            //rotate_point(item->getPosX(), item->getPosY(), 0, 0, (*config)->MAP_North, p);
            rotate_point((float)posX, (float)posY, (float)0, (float)0, (*config)->MAP_North, p);
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
            //if (bFound)
            {
                buildingID = (*config)->BuildingID;
                // if ( posZ >= 400 )
                // {
                //     buildingFloor = 2;
                // }
                //buildingFloor = (*config)->BuildingFloor;
                break;
            }
        }
    }
    delete [] p;

    //printf("TAFServiceLoopThread::getCurrentLngLat() posX[%f] posY[%f] posZ[%f] targetLat[%.012lf] targetLng[%.012lf]\n",
    //           posX, posY, posZ, targetLat, targetLng);


    return bFound;
}

int TAFServiceLoopThread::send_updateTAF(WorkItem * item)
{
    int ret = 0;
    //long long start1 = getSystemTime();

    //printf("TAFMSSQLServiceLoopThread::send_updateTAF() start1[%lld]\n", start1);
    double targetLng = 0.0;
    double targetLat = 0.0;
    int buildingID = 16;
    int buildingFloor = item->getFloor();

    //printf("item->getPosZ()[%f]\n", item->getPosZ());

    //
    // find building id and floor
    //
    bool bFound = getCurrentLngLat(item->getPosX(), item->getPosY(), item->getPosZ(),
                                targetLng, targetLat, buildingID);


    //printf("TAFMSSQLServiceLoopThread::send_updateTAF() (%s) x[%f] y[%f] lat[%f] lat[%f] WGS48_Origin_X[%f] WGS48_Origin_Y[%f]\n",
    //           item->getNodeId(), item->getPosX(), item->getPosY(), lng, lat, mConfig->WGS48_Origin_X, mConfig->WGS48_Origin_Y);
    vilslog::printf("TAFServiceLoopThread::send_updateTAF() bFound[%d] TAFID[%d] BuildingId[%d] Floor[%d] targetLng[%f] targetLat[%f]\n",
         bFound, item->getTAFID(), buildingID, buildingFloor, targetLng, targetLat);

    ret = mTAFDBServer->write_device_footprint(item->getTAFID(), targetLng, targetLat, 4326, buildingID, buildingFloor);

    mDeviceManager->setTagInfoB(item->getTAFID(), targetLng, targetLat, 4326, buildingID, buildingFloor);

    //long long end1 = getSystemTime();
    //long long difft = end1 - start1;

    //printf("TAFServiceLoopThread::send_updateTAF() time: %lld ms\n", difft);
    return ret;
}

int TAFServiceLoopThread::send_event(WorkItem * item)
{
    int ret = 0;

    vilslog::printf("TAFServiceLoopThread::send_event() [%s] [%s] [%s] [%s]\n",
               item->getNodeName(), item->getAction(), item->getAreaName(), item->getIssueTime());

    ret = mTAFDBServer->write_device_event(item->getTAFID(), item->getNodeName(), item->getAction(), item->getAreaName(), item->getIssueTime());

    //mTAFDBServer->get_device_event();

    return ret;
}
