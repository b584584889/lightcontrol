/*
 * SubscribeDataItem.h
 *
 *  Created on: 2018/09/27
 *      Author: ikki
 */
#ifndef __SubscribeDataItem_h__
#define __SubscribeDataItem_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "../common.h"

using namespace std;

class SubscribeDataItem
{
private:
    void init();
public:
    //sos, power ,powerAlert, fenceAlert, coord


    // checkpointAlert
    SubscribeDataItem(const char* msgType, const char* AchorId, const char* ChcekPointName, int alertType);

    // coord
    SubscribeDataItem(const char* msgType, int tagIndex, const char* tagName, const char* tagMacAddress, const char* tagId, int projectID,
        int posX, int posY, int posZ,
        const char* voltage, const char* temperature, const char* areaId, const char* areaName);

    // sos
    SubscribeDataItem(const char* msgType, int tagIndex, const char* tagName, const char* tagMacAddress, const char* tagId, int projectID,
        const char* locatorName, const char* locatorId,
        int alertType, const char* areaId, const char* areaName);

    // fenceAlert, In, Out
    SubscribeDataItem(const char* msgType, int tagIndex, const char* tagName, const char* tagMacAddress, const char* tagId, int projectID,
        int alertType, int alertAction, const char* areaId, const char* areaName);

    // power, powerAlert
    SubscribeDataItem(const char* msgType, int tagIndex, const char* tagName, const char* tagMacAddress, const char* tagId, int projectID,
        const char* voltage, const char* temperature);

    // event
    SubscribeDataItem(const char* msgType, int projectID, int eventid, const char* nodeid, int alertType, int alertAction);

    // callout
    SubscribeDataItem(const char* msgType, int projectID, int eventid, const char* nodeid, const char* nodename, int alertType, int alertAction);

    // cardrollcallevent
    SubscribeDataItem(const char* msgType, int projectID);
    void setMaingroupid(int maingroupid);
    int getMaingroupid();

    ~SubscribeDataItem();

    std::string m_msgType;

    int m_tagIndex;
    std::string m_tagId;
    std::string m_tagName;
    std::string m_tagMacAddress;
    std::string m_voltage;
    std::string m_temperature;
    std::string m_timestamp;

    std::string m_areaId;
    std::string m_areaName;

    std::string m_anchorId;
    std::string m_chcekPointName;

    std::string m_locatorName;
    std::string m_locatorId;

    int m_projectID;
    int m_posX;
    int m_posY;
    int m_posZ;

    // ranging
    //int m_Distance;
    //int m_IMU[6];

    int m_alertType; //1:人員 2:固定裝置
    int m_alertAction;// 1:In 2:Out 3:alert 4:cancel

    int m_eventid;
    std::string m_nodeId;
    std::string m_nodename;

    // cardrollcallevent
    int m_readerid;
    int m_cardtype;
    std::string m_cardid;

    // firefightingevent
    int m_maingroupid;
    std::string m_FireFightingType;
    FireFightingStruct m_FireFightingInfo;
    FireFightingUserStruct m_FireFightingUser;

};

#endif
