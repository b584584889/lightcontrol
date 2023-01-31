/*
 *  DataItem.cpp
 *
 *  Created on: 2017/02/18
 *      Author: ikki
 */
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>

#include "SubscribeDataItem.h"
#include "../common.h"

// coord
SubscribeDataItem::SubscribeDataItem(const char* msgType, int tagIndex, const char* tagName, const char* tagMacAddress, const char* tagId, int projectID,
    int posX, int posY, int posZ,
    const char* voltage, const char* temperature, const char* areaId, const char* areaName)
{
    init();
    m_msgType = msgType;
    m_tagIndex = tagIndex;
    m_tagName = tagName;
    m_tagMacAddress = tagMacAddress;
    m_tagId = tagId;
    m_projectID = projectID;
    m_locatorName = "";
    m_locatorId = "";
    m_posX = posX;
    m_posY = posY;
    m_posZ = posZ;
    m_alertType = -1;
    m_alertAction = -1;
    m_voltage = voltage;
    m_temperature = temperature;
    m_areaId = areaId;
    m_areaName = areaName;
    m_timestamp = std::to_string(getCurrentTimeInMilliSec());

    // for (int i = 0; i < 6; i++)
    //     m_IMU[i] = IMU[i];
}

// sos
SubscribeDataItem::SubscribeDataItem(const char* msgType, int tagIndex, const char* tagName, const char* tagMacAddress, const char* tagId, int projectID,
    const char* locatorName, const char* locatorId,
    int alertType, const char* areaId, const char* areaName)
{
    init();
    m_msgType = msgType;
    m_tagIndex = tagIndex;
    m_tagName = tagName;
    m_tagMacAddress = tagMacAddress;
    m_tagId = tagId;
    m_projectID = projectID;
    m_locatorName = locatorName;
    m_locatorId = locatorId;
    m_posX = 0;
    m_posY = 0;
    m_posZ = 0;
    m_alertType = alertType;
    m_alertAction = -1;
    m_voltage = "0";
    m_temperature = "0";
    m_areaId = areaId;
    m_areaName = areaName;
    m_timestamp = std::to_string(getCurrentTimeInMilliSec());

    // for (int i = 0; i < 6; i++)
    //     m_IMU[i] = 0;
}

// fenceAlert, In, Out
SubscribeDataItem::SubscribeDataItem(const char* msgType, int tagIndex, const char* tagName, const char* tagMacAddress, const char* tagId, int projectID,
    int alertType, int alertAction, const char* areaId, const char* areaName)
{
    init();
    m_msgType = msgType;
    m_tagIndex = tagIndex;
    m_tagName = tagName;
    m_tagMacAddress = tagMacAddress;
    m_tagId = tagId;
    m_projectID = projectID;
    m_locatorName = "";
    m_locatorId = "";
    m_posX = 0;
    m_posY = 0;
    m_posZ = 0;
    m_alertType = alertType;
    m_alertAction = alertAction;
    m_voltage = "0";
    m_temperature = "0";
    m_areaId = areaId;
    m_areaName = areaName;
    m_timestamp = std::to_string(getCurrentTimeInMilliSec());

    // for (int i = 0; i < 6; i++)
    //     m_IMU[i] = 0;
}

// power, powerAlert
SubscribeDataItem::SubscribeDataItem(const char* msgType, int tagIndex, const char* tagName, const char* tagMacAddress, const char* tagId, int projectID,
    const char* voltage, const char* temperature)
{
    init();
    m_msgType = msgType;
    m_tagIndex = tagIndex;
    m_tagName = tagName;
    m_tagMacAddress = tagMacAddress;
    m_tagId = tagId;
    m_projectID = projectID;
    m_locatorName = "";
    m_locatorId = "";
    m_posX = 0;
    m_posY = 0;
    m_posZ = 0;
    m_alertType = -1;
    m_alertAction = -1;
    m_voltage = voltage;
    m_temperature = temperature;
    m_areaId = "";
    m_areaName = "";
    m_timestamp = std::to_string(getCurrentTimeInMilliSec());
    //
    // for (int i = 0; i < 6; i++)
    //     m_IMU[i] = 0;
}

// checkpointAlert
SubscribeDataItem::SubscribeDataItem(const char* msgType, const char* AchorId, const char* ChcekPointName, int alertType)
{
    init();
    m_msgType = msgType;
    m_anchorId = AchorId;
    m_chcekPointName = ChcekPointName;
    m_alertType = alertType;
    m_timestamp = std::to_string(getCurrentTimeInMilliSec());
}

// event
SubscribeDataItem::SubscribeDataItem(const char* msgType, int projectID, int eventid, const char* nodeid, int alertType, int alertAction)
{
    init();
    m_msgType = msgType;
    m_projectID = projectID;
    m_eventid = eventid;
    m_nodeId = nodeid;
    m_alertType = alertType;
    m_alertAction = alertAction;
    m_timestamp = std::to_string(getCurrentTimeInMilliSec());
}

// callout
SubscribeDataItem::SubscribeDataItem(const char* msgType, int projectID, int eventid, const char* nodeid, const char* nodename, int alertType, int alertAction)
{
    init();
    m_msgType = msgType;
    m_projectID = projectID;
    m_eventid = eventid;
    m_nodeId = nodeid;
    m_nodename = nodename;
    m_alertType = alertType;
    m_alertAction = alertAction;
    m_timestamp = std::to_string(getCurrentTimeInMilliSec());
}

// cardrollcallevent
SubscribeDataItem::SubscribeDataItem(const char* msgType, int projectID)
{
    init();
    m_msgType = msgType;
    m_projectID = projectID;
    m_readerid = -1;
    m_cardtype = -1;
    m_cardid = "";
}

void SubscribeDataItem::setMaingroupid(int maingroupid)
{
    m_maingroupid = maingroupid;
}

int SubscribeDataItem::getMaingroupid()
{
    return m_maingroupid;
}

SubscribeDataItem::~SubscribeDataItem()
{
    m_FireFightingInfo.StandbyDeviceIds->clear();
    m_FireFightingInfo.NFCDeviceIdList->clear();

    delete m_FireFightingInfo.StandbyDeviceIds;
    delete m_FireFightingInfo.NFCDeviceIdList;
}

void SubscribeDataItem::init()
{
    m_FireFightingInfo.StandbyDeviceIds = new std::list<std::string>();
    m_FireFightingInfo.NFCDeviceIdList = new std::list<int>();
}
