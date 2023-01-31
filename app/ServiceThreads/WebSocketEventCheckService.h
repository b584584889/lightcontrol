/*
 *  WebSocketEventCheckService.h
 *
 *  Created on: 2018/10/31
 *      Author: ikki
 */
#ifndef __WebSocketEventCheckService_h__
#define __WebSocketEventCheckService_h__


#include "Thread.h"
#include "../network/wqueue.h"
#include "../network/WorkItem.h"
#include "../config.h"
#include "../websocket/SubscribeDataItem.h"
#include "../network/NotificationItem.h"
#include "../DeviceManager/DeviceManager.h"
#include "../mysql/DBManager.h"
#include "WebSocketServer.h"

#define SAME_EVENT_TIMEOUT 30000

// typedef struct
// {
//     int projectid;
//     std::list<AreaStruct*> * mAreaType1List; // 電子圍欄
//     std::list<AreaStruct*> * mAreaType2List; // 電子點名
//     std::list<AreaStruct*> * mAreaType5List; // 病房
//     std::list<AreaStruct*> * mAreaType6List; // 病床
// } ProjectAreaTypeStruct;

class WebSocketEventCheckService : public Thread
{
public:
    WebSocketEventCheckService(wqueue<SubscribeDataItem*> & pWebSocketqueue, wqueue<AreaInOutEventStruct*> & pWebSocketAreaInOutEventQueue, wqueue<NotificationItem*> & NotifiactionQueue, wqueue<WorkItem*> & MySQLJobQueue, DeviceManager * pDevManager, DBManager * dbManager);
    bool status();
    std::string statusString();
    void updateProject();

    void * run();
private:
    wqueue<SubscribeDataItem*> & mWebSocketqueue;
    wqueue<AreaInOutEventStruct*> & mWebSocketAreaInOutEventQueue;
    wqueue<NotificationItem*> & mNotifiactionQueue;
    wqueue<WorkItem*> & mMySQLJobQueue;

    DeviceManager * mDeviceManager;
    DBManager * mDbManager;
    // MySQLDBAccess * mMysqlDB;

    // VILSSystemConfig m_systemConfig;

	// std::list<VILSConfig*> * mAllConfigList;

    // std::list<AreaStruct*> * mAreaType1List;
    // std::list<AreaStruct*> * mAreaType2List;

    // std::vector<ProjectAreaTypeStruct *> mProjectAreaTypeVector;

    std::list<NodeEventTimeStruct*> mNodeEventTimeList;

    bool mUpdateProject;
    std::list<int> * mPrjidList;

    bool mInitial;
    std::string mStatus;

    void runloop();
    void CheckEvent(AreaInOutEventStruct * event);
    void send_fenceAlert_event(AreaInOutEventStruct * event);

    // void initProjectAreaType();
    // ProjectAreaTypeStruct * createProjectAreaType(int projectid);
	// ProjectAreaTypeStruct * getProjectAreaType(int projectid);

    void sendTagEventToWebSocket();
    void sendTagEventToWebSocket(int projectid);
    void sendLocatorEventToWebSocket();
    void sendLocatorEventToWebSocket(int projectid);
    // void reloadAreaList();
    // void reloadAreaList(ProjectAreaTypeStruct * areaTypeInfo);
    // bool check_area_fenceAlert(int projectid, NodeStruct * node);
    // bool check_area_doctorInOut(int projectid, NodeStruct * node);

    // bool checkAreaTypeAlert(int projectid, int areaType, const char * area_AreaId);
    // int resolveProjectId(int floor);
    int check_node_AlertTime(int projectid, const char * nodeid, const char * area_AreaId, int alertAction); // in ms
    int update_node_AlertTime(int projectid, const char * nodeid, const char * area_AreaId, int alertAction); // in ms

    // void clear_ProjectAreaTypeStruct_vector(std::vector<ProjectAreaTypeStruct*> * plist);
};

#endif
