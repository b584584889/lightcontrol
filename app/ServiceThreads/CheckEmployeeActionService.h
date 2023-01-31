/*
 *  CheckEmployeeActionService.h
 *
 *  Created on: 2019/10/15
 *      Author: ikki
 */

#ifndef __CheckEmployeeActionService_h__
#define __CheckEmployeeActionService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../network/WorkItem.h"
#include "../DeviceManager/DeviceManager.h"
#include "../mysql/DBManager.h"
#include "../config.h"

using namespace std;

class CheckEmployeeActionService : public Thread
{
public:
     CheckEmployeeActionService(wqueue<WorkItem*> & MySQLJobQueue, DeviceManager * pDevManager, DBManager * dbManager);
     bool status();
     std::string statusString();
     void updateProject();

     void * run();

private:

     std::string mStatus;
     bool mInitial;

     wqueue<WorkItem*> & mMySQLJobQueue;
     DeviceManager * mDeviceManager;
     DBManager * mDbManager;

     bool mUpdateProject;
     std::list<int> * mPrjidList;

     void runloop();
     void waitloop();

     void checkDeviceTypeAction(int type);
     void checkDeviceTypeAction(int projectid, int type);
     std::list<Pair_AreaInOutEventStruct*> * pair_inout_event(std::list<AreaInOutEventStruct*> * nodeAreaInOutList);
     void output_inout_pairevent(int projectid, std::list<Pair_AreaInOutEventStruct*> * pair_inout_events);
     void create_inout_single_event(int projectid, AreaInOutEventStruct * event);
     void create_inout_pair_event(int projectid, AreaInOutEventStruct * event_1, AreaInOutEventStruct * event_2);

     void checkEmployeeAction();
     void checkEmployeeAction(int projectid);
     std::list<FiredEventStruct*> * merge_same_event(std::list<FiredEventStruct*> * firedEventList);
     std::list<Pair_FiredEventStruct*> * pair_event(std::list<FiredEventStruct*> * merge_events);
     std::list<Pair_FiredEventStruct*> * merge_same_tagaction_event(std::list<Pair_FiredEventStruct*> * pair_events);
     void output_pairevent(int projectid, std::list<Pair_FiredEventStruct*> * pair_events);

     void create_single_event(int projectid, FiredEventStruct * event_Alert);
     void create_pair_event(int projectid, FiredEventStruct * event_Alert, FiredEventStruct * event_Cancel);
     void create_sametag_event(int projectid, FiredEventStruct * event, std::list<FiredEventStruct*> * firedEventList);

     Pair_FiredEventStruct * copyPair_FiredEventStruct(Pair_FiredEventStruct * srcEvent);
};


#endif
