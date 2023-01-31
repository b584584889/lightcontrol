/*
 *  CoordMsgParserService.h
 *
 *  Created on: 2019/04/15
 *      Author: ikki
 */
#ifndef __CoordMsgParserService_h__
#define __CoordMsgParserService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../network/wqueue.h"
#include "../network/DataItem.h"
#include "../network/UDPDataItem.h"
#include "../DeviceManager/DeviceManager.h"
#include "../common.h"
#include "../json/json.h"

//#define PRINT_COORD_MSGPARSER_MSG

using namespace std;

class CoordMsgParserService : public Thread
{
public:
    CoordMsgParserService(wqueue<DataItem*> & jobqueue, wqueue<UDPDataItem*> & udpqueue, DeviceManager * pDevManager);
    bool status();
    std::string statusString();
    void updateProject();

    void * run();

private:
    wqueue<DataItem*> & m_jobqueue;
    wqueue<UDPDataItem*> & m_udpqueue;
    DeviceManager * mDeviceManager;

    bool mUpdateProject;
    std::list<int> * mPrjidList;

    bool mInitial;

    std::string mStatus;

    void runloop();

    void send_alive_to_projects(const char * mac, int nodetype, const char * vlt, const char * tem, int count, const char * panid, const char * IP);
    void send_join_to_projects(const char * mac, int nodetype, const char * version, const char * IP);
    void send_alert_to_projects(const char * mac, int nodetype, const char * IP);
    void send_cancel_to_projects(const char * mac, int nodetype, const char * IP);
    void send_rangingdiag_to_projects(int anchoridx, int tagidx, int rangingidx, int distance, int * imu,
                        int TagPollCnt, int RangingAnchorCnt, int TagFinalRecvdBm, int TagFirstPathdBm,
                        int AnchorRecvdBm, int AnchorFirstPathdBm, int RangingCount, int RangingFailCount, float DataRate, const char * IP);
    void send_rangingdiag_1_to_projects(std::string anchor_mac, std::string tag_mac, int rangingidx, int distance, int * imu,
                        int TagPollCnt, int RangingAnchorCnt, int TagFinalRecvdBm, int TagFirstPathdBm,
                        int AnchorRecvdBm, int AnchorFirstPathdBm, int RangingCount, int RangingFailCount, float DataRate, const char * IP);
    void send_list_to_projects(int nodetype, std::list<NodeStruct*> * plist, const char * IP);


    int do_rangingdiag(Json::Value & JSONresult, const char * IP);
    int do_list(Json::Value & JSONresult, const char * IP);
    int getNodeProjectId(const char * mac, int nodetype);
};

#endif
