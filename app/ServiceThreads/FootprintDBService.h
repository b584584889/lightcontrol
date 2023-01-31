/*
 *  FootprintDBService.h
 *
 *  Created on: 2019/09/04
 *      Author: ikki
 */
#ifndef __FootprintDBService_h__
#define __FootprintDBService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Thread.h"
#include "../network/WorkItem.h"
#include "../mysql/DBManager.h"
#include "../config.h"

using namespace std;

class FootprintDBService : public Thread
{
public:
    FootprintDBService(wqueue<WorkItem*> & queue, DBManager * dbManager);
    FootprintDBService();
    ~FootprintDBService();
    bool status();
    std::string statusString();
    void updateProject();
    bool savealldata();

    void * run();

private:

    std::string mStatus;
    bool mInitial;
    bool mFinished; // before system exit(1);

    wqueue<WorkItem*> & m_queue;
    DBManager * mDbManager;

    bool mUpdateProject;
    std::list<int> * mPrjidList;

    TagIndexDataClass mTagData[MAX_TAG_NUMBER];

    void runloop();
    void save_to_db();
    void clear_old_data();

    int send_alive(WorkItem * item);
    int send_updateM(WorkItem * item);
    TagIndexDataClass * get_current_idx(int projectid, const char * tagid);

    double calcAcceleration(double * oldIMU, double * newIMU);
    double calcVelocity(double * oldIMU, double * newIMU);
};

#endif
