/*
 *  FootprintDBService.cpp
 *
 *  Created on: 2019/09/04
 *      Author: ikki
 */

#include <string.h>
#include <ctime>
#include "FootprintDBService.h"
#include "../common.h"

using namespace std;

FootprintDBService::FootprintDBService(wqueue<WorkItem*> & queue, DBManager * dbManager)
    : m_queue(queue), mDbManager(dbManager)
{

    for (int i=0;i<MAX_TAG_NUMBER;i++)
    {
        mTagData[i].idx = -1;
        memset(mTagData[i].TagId, 0, 32);
        memset(mTagData[i].voltage, 0, 32);
        memset(mTagData[i].temperature, 0, 32);
    }
}

FootprintDBService::~FootprintDBService()
{
}

bool FootprintDBService::status()
{
    return mInitial;
}

std::string FootprintDBService::statusString()
{
    return mStatus;
}

void FootprintDBService::updateProject()
{
    mUpdateProject = true;
}

bool FootprintDBService::savealldata()
{
    mFinished = false;
    printf("FootprintDBService::savealldata()\n");

    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);

        FootprintDBAccess * fpDb = mDbManager->getFootprintDB(projectid);
        if (fpDb != NULL)
        {
            fpDb->save_to_db();
        }

        TimeSerialsDBAccess * tsDb = mDbManager->getTSDB(projectid);
        if (tsDb != NULL)
        {
            tsDb->Transaction_commit();
        }
    }
    printf("FootprintDBService::savealldata() finished!\n");
    mFinished = true;

    return mFinished;
}

void * FootprintDBService::run()
{
    while (1)
    {
        mInitial = false;
        mFinished = false;
        mStatus = "FootprintDBService() Initial failed!!";

        try
        {
            runloop();
            vilslog::printf ("FootprintDBService() Initial failed!\n");
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("FootprintDBService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");
            mInitial = false;
            mStatus = "FootprintDBService::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void FootprintDBService::runloop()
{
    char type[64];
    long long start1 = getSystemTime();
    long long updateTime = 0;
    long long checkTime = 0;
    long long updatePrjIdTime = 0;
    int updateM_count = 0;

    mInitial = true;
    mStatus = "FootprintDBService() Initial successfully!";
    vilslog::printf ("FootprintDBService() Initial successfully!\n");

    mPrjidList = mDbManager->getAllProjectConfig();

    // available to process.
    while (1)
    {
        WorkItem * item = m_queue.removetimed(1000);

        long long end1 = getSystemTime();
        long long difft = end1 - start1;
        updateTime += difft;
        checkTime += difft;
        updatePrjIdTime += difft;

        if (updateTime >= 60000 && updateM_count > 0)
        {
            save_to_db();

            updateTime = 0;
            updateM_count = 0;
        }

        if (checkTime >= 30000)
        {
            std::string time = currentTimeForSQL();

            if ( time.compare("02:00") == 0 )
            {
                long long t_clear_start = getSystemTime();
                clear_old_data();
                long long t_clear = (int)(getSystemTime() - t_clear_start);
                vilslog::printf("FootprintDBService::runloop() t_clear[%lld]\n", t_clear);
            }

            checkTime = 0;
        }

        if (updatePrjIdTime > 3600000 || mUpdateProject)
        {
            mPrjidList->clear();
            delete mPrjidList;
            mPrjidList = mDbManager->getAllProjectConfig();

            updatePrjIdTime = 0;
            mUpdateProject = false;
        }

        if (item != NULL)
        {
            sprintf(type, "%s", item->getType());

//            printf("FootprintDBService::run() type[%s] difft[%lld]\n", type, difft);

            if ( strcmp(type, "alive") == 0)
                send_alive(item);
            else
            if ( strcmp(type, "updateM") == 0)
            {
                updateM_count++;
                send_updateM(item);
            }
            else
            if ( strcmp(type, "dbCommit") == 0)
            {
                save_to_db();
            }

            delete item;
        }

        start1 = end1;
    }
}

void FootprintDBService::save_to_db()
{
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);

        FootprintDBAccess * fpDb = mDbManager->getFootprintDB(projectid);
        if (fpDb != NULL)
        {
            //fpDb->Transaction_commit();
            fpDb->save_to_db();
        }

        TimeSerialsDBAccess * tsDb = mDbManager->getTSDB(projectid);
        if (tsDb != NULL)
        {
            tsDb->Transaction_commit();
        }
    }
}

void FootprintDBService::clear_old_data()
{
    std::list<int> * prjidList = mDbManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);

        FootprintDBAccess * fpDb = mDbManager->getFootprintDB(projectid);
        if (fpDb != NULL)
        {
            fpDb->clear_old_data();
        }

        TimeSerialsDBAccess * tsDb = mDbManager->getTSDB(projectid);
        if (tsDb != NULL)
        {
            tsDb->clear_old_data();
        }
    }

    prjidList->clear();
    delete prjidList;
}

int FootprintDBService::send_alive(WorkItem * item)
{
    int ret = 0;
    bool bFound = false;

    int projectid = item->getProjectId();

    for (int i=0;i<MAX_TAG_NUMBER;i++)
    {
        if( (mTagData[i].projectid == projectid) && (strcmp(item->getNodeId(), mTagData[i].TagId) == 0) )
        {
            snprintf(mTagData[i].voltage, 32, "%0.2f", item->getVoltage());
            snprintf(mTagData[i].temperature, 32, "%0.2f", item->getTemperature());

            //printf("thread MySQLServiceLoopThread, send_alive %s(%s,%s)\n",
            //     mTagData[i].TagId, mTagData[i].voltage, mTagData[i].temperature);

            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        for (int i=0;i<MAX_TAG_NUMBER;i++)
        {
            if(strlen(mTagData[i].TagId) == 0)
            {
                mTagData[i].projectid = projectid;
                strcpy(mTagData[i].TagId, item->getNodeId());
                snprintf(mTagData[i].voltage, 32, "%0.2f", item->getVoltage());
                snprintf(mTagData[i].temperature, 32, "%0.2f", item->getTemperature());
                mTagData[i].prePosX = 0;
                mTagData[i].prePosY = 0;
                mTagData[i].prePosZ = 0;
                mTagData[i].moveoffset = 0;
                mTagData[i].issuetime = 0;

                vilslog::printf("thread FootprintDBService, new data send_alive %s(%s,%s)\n",
                                mTagData[i].TagId, mTagData[i].voltage, mTagData[i].temperature);

                break;
            }
        }
    }
    return ret;
}

int FootprintDBService::send_updateM(WorkItem * item)
{
    int ret = 0;
    double curIMU[6];
    int projectid = item->getProjectId();

    TagIndexDataClass * curidx = get_current_idx(projectid, item->getNodeId());
    // const char * areaid = item->getAreaId();

    //printf("FootprintDBService::send_updateM() ()%s) area[%s]\n", item->getNodeId(), areaid.c_str());

    const char * macaddress = item->getMacaddress();
    const char * areaid = item->getAreaId();
    int maplayer = item->getMaplayer();
    int PosX = item->getPosX();
    int PosY = item->getPosY();
    int PosZ = item->getPosZ();

    TimeSerialsDBAccess * tsDb = mDbManager->getTSDB(projectid);
    if (tsDb != NULL)
    {
        std::string recordnumber = item->getRecorderID();
        // std::string team = item->getMaingroups();
        int role = item->getRole();
        std::string nodeid = item->getNodeId();
        std::string datetime = item->getDateTime();
        int maingroupid = item->getMaingroupid();
        std::string status = item->getStatus();

        curIMU[0] = item->getAccelX();
        curIMU[1] = item->getAccelY();
        curIMU[2] = item->getAccelZ();
        curIMU[3] = item->getGyroX();
        curIMU[4] = item->getGyroY();
        curIMU[5] = item->getGyroZ();

        double accelX = item->getAccelX() * 100;// scale 0.01
        double accelY = item->getAccelY() * 100;// scale 0.01
        double accelZ = item->getAccelZ() * 100;// scale 0.01
        double gyroX = item->getGyroX() * 100;// scale 0.01
        double gyroY = item->getGyroY() * 100;// scale 0.01
        double gyroZ = item->getGyroZ() * 100;// scale 0.01
        char IMU[64];
        memset(IMU, 0, 64);
        sprintf(IMU, "%d:%d:%d:%d:%d:%d", (int)accelX, (int)accelY, (int)accelZ, (int)gyroX, (int)gyroY, (int)gyroZ);

        std::string team = std::to_string(role);

        // check time
        time_t cur_time_t = getCurrentTimeInMilliSec();

        if ( recordnumber.compare(curidx->recordnumber) != 0 )
        {
            curidx->recordnumber = recordnumber;
            curidx->prePosX = PosX;
            curidx->prePosY = PosY;
            curidx->prePosZ = PosZ;
            curidx->preIMU[0] = curIMU[0];
            curidx->preIMU[1] = curIMU[1];
            curidx->preIMU[2] = curIMU[2];
            curidx->preIMU[3] = curIMU[3];
            curidx->preIMU[4] = curIMU[4];
            curidx->preIMU[5] = curIMU[5];
            curidx->velocity = 0;
            curidx->diffacceleration = 0;
            curidx->acceleration = 0;
            curidx->moveoffset = 0;
            curidx->issuetime = 0;
            curidx->status = status;
        }
        else
        {
            // offset
            int x_diff = curidx->prePosX - PosX;
            int y_diff = curidx->prePosY - PosY;

            curidx->prePosX = PosX;
            curidx->prePosY = PosY;
            curidx->prePosZ = PosZ;

            double distance =  sqrt(pow(x_diff, 2) + pow(y_diff, 2));
            curidx->moveoffset += (int)distance;

            double diifTime = std::difftime(cur_time_t, curidx->issuetime);
            curidx->issuetime = cur_time_t;
            curidx->velocity = ((distance / 100.0f) / (diifTime / 1000.0f));

            double accel = calcAcceleration(curidx->preIMU, curIMU);

            curidx->diffacceleration = accel - curidx->acceleration;
            curidx->acceleration = accel;

            curidx->preIMU[0] = curIMU[0];
            curidx->preIMU[1] = curIMU[1];
            curidx->preIMU[2] = curIMU[2];
            curidx->preIMU[3] = curIMU[3];
            curidx->preIMU[4] = curIMU[4];
            curidx->preIMU[5] = curIMU[5];

            curidx->status = status;
            // vilslog::printf("FootprintDBService::send_updateM() [%s] distance[%f] diifTime[%f] velocity[%f] moveoffset[%d] diffacceleration[%f] accel[%f]\n",
            //     curidx->TagId, distance, diifTime, curidx->velocity, curidx->moveoffset, curidx->diffacceleration, accel);

            // vilslog::printf("FootprintDBService::send_updateM() [%s] accel[%f] IMU[%.02f,%.02f,%.02f,%.02f,%.02f,%.02f]\n",
            //     curidx->TagId, accel, curIMU[0], curIMU[1], curIMU[2], curIMU[3], curIMU[4], curIMU[5]);

            // vilslog::printf("FootprintDBService::send_updateM() [%s] distance[%f] diifTime[%f] velocity[%f] IMU[%.02f,%.02f,%.02f,%.02f,%.02f,%.02f]\n",
            //     curidx->TagId, curidx->distance, diifTime, curIMU[0], curIMU[1], curIMU[2], curIMU[3], curIMU[4], curIMU[5]);
        }

        tsDb->Transaction_write_node_SoccerTag(recordnumber, team, nodeid, areaid,
            maplayer, maingroupid, PosX, PosY, PosZ, curidx->voltage, curidx->moveoffset, curidx->velocity, curidx->acceleration, curidx->diffacceleration, IMU, curidx->status, datetime);
    }

    FootprintDBAccess * fpDb = mDbManager->getFootprintDB(projectid);
    if (fpDb != NULL)
    {
        std::string datetime_str = item->getDateTime();
        std::string date_str = TodayDateType1();
        std::string time_str = currentTimeWithSecForSQL();

        // vilslog::printf("FootprintDBService::send_updateM() macaddress[%s] maplayer[%d] areaid[%s]\n", item->getNodeId(), maplayer, areaid);

        // fpDb->Transaction_write_node_footprint(macaddress, date_str.c_str(), time_str.c_str(), areaid,
        //     maplayer, item->getPosX(), item->getPosY(), item->getPosZ(), datetime_str.c_str());
        fpDb->write_node_footprint(macaddress, date_str.c_str(), time_str.c_str(), areaid,
            maplayer, PosX, PosY, PosZ, datetime_str.c_str());
    }

    return ret;
}

TagIndexDataClass * FootprintDBService::get_current_idx(int projectid, const char * tagid)
{
    TagIndexDataClass * curidx = NULL;

    bool bFound = false;
    for (int i=0;i<MAX_TAG_NUMBER;i++)
    {
        if( (mTagData[i].projectid == projectid) && (strcmp(tagid, mTagData[i].TagId) == 0) )
        {
            curidx = &mTagData[i];
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        for (int i=0;i<MAX_TAG_NUMBER;i++)
        {
            if(strlen(mTagData[i].TagId) == 0)
            {
                mTagData[i].projectid = projectid;
                strcpy(mTagData[i].TagId, tagid);
                memset(mTagData[i].voltage, 0, 32);
                memset(mTagData[i].temperature, 0, 32);
                mTagData[i].idx = -1;
                mTagData[i].recordnumber = "";
                mTagData[i].prePosX = 0;
                mTagData[i].prePosY = 0;
                mTagData[i].prePosZ = 0;
                mTagData[i].preIMU[0] = 0;
                mTagData[i].preIMU[1] = 0;
                mTagData[i].preIMU[2] = 0;
                mTagData[i].preIMU[3] = 0;
                mTagData[i].preIMU[4] = 0;
                mTagData[i].preIMU[5] = 0;
                mTagData[i].velocity = 0;
                mTagData[i].diffacceleration = 0;
                mTagData[i].acceleration = 0;
                mTagData[i].moveoffset = 0;
                mTagData[i].status = "";
                mTagData[i].issuetime = 0;

                curidx = &mTagData[i];
                break;
            }
        }
    }

    return curidx;
}

double FootprintDBService::calcAcceleration(double * oldIMU, double * newIMU)
{
    double accel = 0;
    double IMUdiff[6];
	for(int j=0;j<6;j++)
		IMUdiff[j] = (newIMU[j] - oldIMU[j]) / 10.0f;

    double vxyz = IMUdiff[0] * IMUdiff[0] + IMUdiff[1] * IMUdiff[1] + IMUdiff[2] * IMUdiff[2];

    accel = sqrt(vxyz);

    // vilslog::printf("FootprintDBService::calcAcceleration() vxyz[%f] accel[%f] IMUdiff[%.02f,%.02f,%.02f,%.02f,%.02f,%.02f]\n",
    //     vxyz, accel, IMUdiff[0], IMUdiff[1], IMUdiff[2], IMUdiff[3], IMUdiff[4], IMUdiff[5]);

    return accel;
}

double FootprintDBService::calcVelocity(double * oldIMU, double * newIMU)
{
    double ret = 0;
	float Velocity = 0.0f;
	double IMUdiff[6];
	for(int j=0;j<6;j++)
		IMUdiff[j] = (newIMU[j] - oldIMU[j]) / 10.0f;

	// double caaX = IMUdiff[3] * IMUdiff[3] * (1.0f) ; // Centripetal Acceleration = Angular Acceleration * radius(1.0f)
	// double caaY = IMUdiff[4] * IMUdiff[4] * (1.0f) ; // Centripetal Acceleration = Angular Acceleration * radius(1.0f)
	// double caaZ = IMUdiff[5] * IMUdiff[5] * (1.0f) ; // Centripetal Acceleration = Angular Acceleration * radius(1.0f)

    double caaX = IMUdiff[3] * IMUdiff[3] * (1.0f) * 0.1f ; // Centripetal Acceleration = Angular Acceleration * radius(1.0f)
	double caaY = IMUdiff[4] * IMUdiff[4] * (1.0f) * 0.1f ; // Centripetal Acceleration = Angular Acceleration * radius(1.0f)
	double caaZ = IMUdiff[5] * IMUdiff[5] * (1.0f) * 0.1f; // Centripetal Acceleration = Angular Acceleration * radius(1.0f)

	double vxyz = IMUdiff[0] * IMUdiff[0] + IMUdiff[1] * IMUdiff[1] + IMUdiff[2] * IMUdiff[2];
	double ca = (caaY + caaZ) * (caaY + caaZ) + (caaX + caaZ) * (caaX + caaZ) + (caaX + caaY) * (caaX + caaY);

	//printf ("AnchorTagCoupleDataClass::putRanging() vxyz = %f, ca = %f\n", vxyz, ca);

	if ( vxyz - ca  > 0.0f)
	{
		Velocity = sqrt(vxyz - ca);
	}
	else
	{
		Velocity = 0.0f; // rotating
	}
    ret = (double)Velocity;
	return ret;
}
