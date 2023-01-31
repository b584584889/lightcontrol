/*
 * EventChecker.h
 *
 *  Created on: 2019/04/23
 *      Author: ikki
 */

#ifndef EventChecker_H
#define EventChecker_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../common.h"
#include "../DeviceManager/DeviceManager.h"
#include "../mysql/MySQLDBAccess.h"
#include "../network/wqueue.h"
#include "../network/WorkItem.h"
#include "../websocket/SubscribeDataItem.h"
#include "../network/NotificationItem.h"

using namespace std;

#define AreaInOutData_BUFFER_SIZE 32

#define SAME_FENCE_EVENT_TIMEOUT 30000

typedef struct
{
    int projectid;
    std::list<EventStruct*> * mEventList;
    std::vector<AreaInOutDataClass *> mAreaInOutDataVector;
} ProjectEventInfoListStruct;

class EventChecker
{
public:
    EventChecker(wqueue<SubscribeDataItem*> & pWebSocketqueue, wqueue<NotificationItem*> & NotifiactionQueue, wqueue<WorkItem*> & MySQLJobQueue, DeviceManager * pDevManager, DBManager * dbManager);
    ~EventChecker();
    int setUpdateEventList();

    bool addNewEvent(AreaInOutEventStruct * event);
    bool getTagAreaLastAction(int projectid, std::string NodeId, std::string AreaID, AreaInOutStruct & data);

    int check();

private:

    wqueue<SubscribeDataItem*> & mWebSocketqueue;
    wqueue<NotificationItem*> & mNotifiactionQueue;
    wqueue<WorkItem*> & mMySQLJobQueue;
    DeviceManager * mDeviceManager;
    DBManager * mDbManager;

    std::vector<ProjectEventInfoListStruct *> mProjectEventInfoVector;
    std::list<NodeEventTimeStruct*> mNodeEventTimeList;
    std::vector<AreaInOutDataClass *> mAreaInOutDataVector;

    bool mUpdateEventList;

    int updateEventList();
    int updateEventList(ProjectEventInfoListStruct * eventInfo);

    int init();
    ProjectEventInfoListStruct * createProjectEventInfo(int projectid);
	ProjectEventInfoListStruct * getProjectEventInfo(int projectid);
    void check_allProject();
    void check_Project(ProjectEventInfoListStruct * eventInfo);

    FiredEventStruct * do_check_action(ProjectEventInfoListStruct * eventInfo, EventStruct* event, int action);
    FiredEventStruct * check_event_mach(EventStruct * event, AreaInOutStruct * data);

    std::string convert_event_to_json_str(EventStruct * event);
    std::string convert_event_to_show_string(NodeStruct * tagnode, EventStruct * event);

    std::string get_Action_Name(std::string action);
    std::string get_Issue_Name(std::string issuetime);

    bool check_area_fenceAlert(AreaInOutEventStruct * event);
    int check_node_AlertTime(int projectid, const char * nodeid, const char * area_AreaId, int alertAction); // in ms
    int update_node_AlertTime(int projectid, const char * nodeid, const char * area_AreaId, int alertAction); // in ms

    void clear_ProjectEventInfoListStruct_vector(std::vector<ProjectEventInfoListStruct*> * plist);
};


#endif
