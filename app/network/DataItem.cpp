/*
 *  DataItem.cpp
 *
 *  Created on: 2017/02/18
 *      Author: ikki
 */
 #include <stdio.h>
 #include <stdlib.h>
 #include "string.h"
 #include <iostream>
 #include <sstream>

#include "DataItem.h"

DataItem::DataItem(const char* type, int projectID, const char* macaddress, int nodetype)
    : m_type(type), m_projectID(projectID), m_NodeType(nodetype), m_macaddress(macaddress)
{
    mNodeList = NULL;
}

DataItem::DataItem(const char* type, int projectID, const char* macaddress, int nodetype, const char* voltage, const char* temperature, int count, const char * panid)
    : m_type(type), m_projectID(projectID), m_NodeType(nodetype), m_macaddress(macaddress), m_voltage(voltage), m_temperature(temperature), m_count(count), m_panid(panid)
{
    mNodeList = NULL;
}

DataItem::DataItem(const char* type, int projectID, const char* macaddress, int nodetype, const char* version)
    : m_type(type), m_projectID(projectID), m_NodeType(nodetype), m_macaddress(macaddress), m_Version(version)
{
    mNodeList = NULL;
}

DataItem::DataItem(const char* type, int projectID, const char* macaddress, string anchoridxs)
    : m_type(type), m_projectID(projectID), m_macaddress(macaddress), validAnchors(anchoridxs)
{
    mNodeList = NULL;
}

DataItem::DataItem(const char* type, int projectID, const char* macaddress, const char * ipAddress)
    : m_type(type), m_projectID(projectID), m_macaddress(macaddress), m_IPAddress(ipAddress)
{
    mNodeList = NULL;
}

DataItem::DataItem(const char* type, int projectID, const char* macaddress, const char * ipAddress, string anchoridxs)
    : m_type(type), m_projectID(projectID), m_macaddress(macaddress), validAnchors(anchoridxs), m_IPAddress(ipAddress)
{
    mNodeList = NULL;
}

DataItem::DataItem(const char* type, int projectID, const char* macaddress, const char * ipAddress, int range1, int range2)
    : m_type(type), m_projectID(projectID), m_macaddress(macaddress), m_IPAddress(ipAddress), m_range1(range1), m_range2(range2)
{
    mNodeList = NULL;
}

DataItem::DataItem(const char* type, int projectID, const char* tagid, const char* anchorid, int distance, int dummy, time_t timestamp)
    : m_type(type), m_projectID(projectID), m_Distance(distance), m_tagid(tagid), m_anchorid(anchorid), m_timestamp_t(timestamp)
{
    mNodeList = NULL;
    m_timestamp = "";
}

DataItem::DataItem(const char* type, int projectID, const char* tagid, const char* anchorid, int distance, int dummy, const char * timestamp)
    : m_type(type), m_projectID(projectID), m_Distance(distance), m_tagid(tagid), m_anchorid(anchorid), m_timestamp(timestamp)
{
    mNodeList = NULL;
    m_timestamp_t = 0;
}

// loraranging
DataItem::DataItem(const char* type, int projectID, const char * LoRAPrefix, const char * LoRA_macaddress, const char * Tag_macaddress, int Distance, int rssi, const char * timestamp)
    : m_type(type), m_projectID(projectID), m_macaddress(Tag_macaddress), m_Distance(Distance), m_LoRAMacaddress(LoRA_macaddress), m_rssi(rssi), m_timestamp(timestamp)
{
    mNodeList = NULL;
    m_timestamp_t = 0;
}

// lorarangingcompress
DataItem::DataItem(const char* type, int projectID, const char * LoRAPrefix, const char * LoRA_macaddress, const char * Tag_macaddress, int Distance, int velocity, int rssi, const char * timestamp)
    : m_type(type), m_projectID(projectID), m_macaddress(Tag_macaddress), m_Distance(Distance), m_Velocity(velocity), m_LoRAMacaddress(LoRA_macaddress), m_rssi(rssi), m_timestamp(timestamp)
{
    mNodeList = NULL;
}

// lorarangingcompressaoa
DataItem::DataItem(const char* type, int projectID, const char * LoRAPrefix, const char * LoRA_macaddress, const char * Tag_macaddress, int Distance, int velocity, float angle, int rssi, const char * timestamp)
    : m_type(type), m_projectID(projectID), m_macaddress(Tag_macaddress), m_Distance(Distance), m_Velocity(velocity), m_angle(angle), m_LoRAMacaddress(LoRA_macaddress), m_rssi(rssi), m_timestamp(timestamp)
{
    mNodeList = NULL;
}

// lorarangingcompressMultiDistance
DataItem::DataItem(const char* type, int projectID, const char * LoRA_macaddress, const char * Tag_macaddress, int distInterval, std::list<int> * distList, int velocity, int rssi, const char * timestamp)
    : m_type(type), m_projectID(projectID), m_macaddress(Tag_macaddress), m_distInterval(distInterval), m_Velocity(velocity), m_LoRAMacaddress(LoRA_macaddress), m_rssi(rssi), m_timestamp(timestamp)
{
    mNodeList = NULL;
    for (std::list<int>::iterator i = distList->begin(); i != distList->end(); i++)
    {
        m_distList.push_back((*i));
    }
}


DataItem::DataItem(const char* type, int projectID, int nodetype, std::list<NodeStruct*> * pSourcelist)
    : m_type(type), m_projectID(projectID), m_NodeType(nodetype)
{
    //clear_node_list(mNodeList);

    mNodeList = new std::list<NodeStruct*>();

    // copy list
    for (std::list<NodeStruct*>::iterator i = pSourcelist->begin(); i != pSourcelist->end(); i++)
    {
        NodeStruct * newNode = createNodeStruct();
        newNode->index = (*i)->index;
        newNode->alive = (*i)->alive;
        newNode->nodetype = (*i)->nodetype;
        strcpy(newNode->nodeid, (*i)->nodeid);
        strcpy(newNode->nodename, (*i)->nodename);
        strcpy(newNode->panid, (*i)->panid);
        strcpy(newNode->macaddress, (*i)->macaddress);
        newNode->posX = (*i)->posX;
        newNode->posY = (*i)->posY;
        newNode->posZ = (*i)->posZ;
        newNode->buildingid = (*i)->buildingid;
        newNode->buildingfloor = (*i)->buildingfloor;
        newNode->maplayer = (*i)->maplayer;

        strcpy(newNode->voltage, (*i)->voltage);
        strcpy(newNode->temperature, (*i)->temperature);
        strcpy(newNode->version, (*i)->version);
        strcpy(newNode->rangingList, (*i)->rangingList);

        strcpy(newNode->IP, (*i)->IP);
        strcpy(newNode->AppVersion, (*i)->AppVersion);
        strcpy(newNode->PID, (*i)->PID);
        strcpy(newNode->time, (*i)->time);

        newNode->blacklist = (*i)->blacklist;
        newNode->whitelist = (*i)->whitelist;
        newNode->joinRangingRange = (*i)->joinRangingRange;
        newNode->joinNotRangingRange = (*i)->joinNotRangingRange;

        newNode->poweronTimestamp = (*i)->poweronTimestamp;
        newNode->poweroffTimestamp = (*i)->poweroffTimestamp;
        //printf("DataItem::DataItem() macaddress[%s] index[%d]\n", newNode->macaddress, newNode->index);

        newNode->maingroupid = (*i)->maingroupid;
        newNode->verifykey = (*i)->verifykey;

        mNodeList->push_back(newNode);
    }
}

// DataItem::DataItem(const char* type, int projectID, int anchoridx, int tagidx, int rangingidx, int distance, int * imu)
//     : m_type(type), m_projectID(projectID), m_Anchor_idx(anchoridx), m_Tag_idx(tagidx), m_Ranging_idx(rangingidx), m_Distance(distance)
// {
//     m_TagPollCnt = 0;
//     m_RangingAnchorCnt = 0;
//     m_TagFinalRecvdBm = 0;
//     m_TagFirstPathdBm = 0;
//     m_AnchorRecvdBm = 0;
//     m_AnchorFirstPathdBm = 0;
//     mNodeList = NULL;
//
//     for (int i = 0; i < 6; i++)
//         m_IMU[i] = imu[i];
// }

DataItem::DataItem(const char* type, int projectID, int anchoridx, int tagidx, int rangingidx, int distance, int * imu,
                    int TagPollCnt, int RangingAnchorCnt, int TagFinalRecvdBm, int TagFirstPathdBm,
                    int AnchorRecvdBm, int AnchorFirstPathdBm, int RangingCount, int RangingFailCount, float DataRate, const char * IP)
{
    m_type = type;
    m_projectID = projectID;
    m_Anchor_idx = anchoridx;
    m_Tag_idx = tagidx;
    m_Ranging_idx = rangingidx;
    m_Distance = distance;
    m_TagPollCnt = TagPollCnt;
    m_RangingAnchorCnt = RangingAnchorCnt;
    m_TagFinalRecvdBm = TagFinalRecvdBm;
    m_TagFirstPathdBm = TagFirstPathdBm;
    m_AnchorRecvdBm = AnchorRecvdBm;
    m_AnchorFirstPathdBm = AnchorFirstPathdBm;
    //strncpy(m_AnchorRangingTable, AnchorRangingTable, 256);
    m_RangingCount = RangingCount;
    m_RangingFailCount = RangingFailCount;
    m_DataRate = DataRate;
    m_IPAddress = IP;
    mNodeList = NULL;

    for (int i = 0; i < 6; i++)
        m_IMU[i] = imu[i];
}

DataItem::DataItem(const char* type, int projectID, std::string anchor_mac, std::string tag_mac, int rangingidx, int distance, int * imu,
                    int TagPollCnt, int RangingAnchorCnt, int TagFinalRecvdBm, int TagFirstPathdBm,
                    int AnchorRecvdBm, int AnchorFirstPathdBm, int RangingCount, int RangingFailCount, float DataRate, const char * IP)
{
    m_type = type;
    m_projectID = projectID;
    m_AnchorMac = anchor_mac;
    m_TagMac = tag_mac;
    m_Ranging_idx = rangingidx;
    m_Distance = distance;
    m_TagPollCnt = TagPollCnt;
    m_RangingAnchorCnt = RangingAnchorCnt;
    m_TagFinalRecvdBm = TagFinalRecvdBm;
    m_TagFirstPathdBm = TagFirstPathdBm;
    m_AnchorRecvdBm = AnchorRecvdBm;
    m_AnchorFirstPathdBm = AnchorFirstPathdBm;
    //strncpy(m_AnchorRangingTable, AnchorRangingTable, 256);
    m_RangingCount = RangingCount;
    m_RangingFailCount = RangingFailCount;
    m_DataRate = DataRate;
    m_IPAddress = IP;
    mNodeList = NULL;

    for (int i = 0; i < 6; i++)
        m_IMU[i] = imu[i];
}

// DataItem::DataItem(const char* type, int projectID, int tagidx, string anchoridxs)
//     : m_type(type), m_projectID(projectID), m_Tag_idx(tagidx), validAnchors(anchoridxs)
// {
//     mNodeList = NULL;
// }

DataItem::DataItem(const char* type, int projectID, int dummy, const char* systemtime)
    : m_type(type), m_projectID(projectID), m_systemtime(systemtime)
{
    mNodeList = NULL;
}

DataItem::DataItem(const char* type, int projectID, int window_time, int reset_time)
    : m_type(type), m_projectID(projectID), tag_window_time(window_time), tag_reset_time(reset_time)
{
    mNodeList = NULL;
}

// DataItem::DataItem(const char* type, int projectID, int tagidx, const char* anchorid, int distance)
//     : m_type(type), m_projectID(projectID), m_Tag_idx(tagidx), m_Distance(distance), m_anchorid(anchorid)
// {
//     mNodeList = NULL;
// }

DataItem::DataItem(const char* type, int projectID, int time_out)
    : m_type(type), m_projectID(projectID), m_timeout(time_out)
{
    mNodeList = NULL;
}

DataItem::DataItem(const char* type, int projectID, const char* nodeid)
    : m_type(type), m_projectID(projectID), m_nodeid(nodeid)
{
    mNodeList = NULL;
}

DataItem::DataItem(const char* type, int projectID, std::list<std::string> * stringList)
    : m_type(type), m_projectID(projectID)
{
    mNodeList = NULL;
    // copy list
    for (std::list<std::string>::iterator i = stringList->begin(); i != stringList->end(); i++)
    {
        m_stringList.push_back((*i));
    }
}

DataItem::DataItem(const char* type, int projectID, SendNotifyStatusStruct * status)
    : m_type(type), m_projectID(projectID)
{
    mNodeList = NULL;
    m_status.projectid = projectID;
    m_status.status = status->status;// 0:wait finish 1:success 2:failed!
    m_status.waitkey = status->waitkey;
    m_status.message = status->message;
    m_status.createtime = status->createtime;
}

DataItem::DataItem(const char* type, int projectID, CARDRollCallRecordStruct * record)
    : m_type(type), m_projectID(projectID)
{
    mNodeList = NULL;
    mCARDRollCallRecord.rollcallid = record->rollcallid;
    mCARDRollCallRecord.rollcalleventid = record->rollcalleventid;
    mCARDRollCallRecord.cardid = record->cardid;
    mCARDRollCallRecord.userid = record->userid;
    mCARDRollCallRecord.nodeid = record->nodeid;

    for (std::list<int>::iterator value = record->rfiddeviceids.begin(); value != record->rfiddeviceids.end(); value++)
    {
        mCARDRollCallRecord.rfiddeviceids.push_back((*value));
    }

    for (std::list<std::string>::iterator value = record->nfclocators.begin(); value != record->nfclocators.end(); value++)
    {
        mCARDRollCallRecord.nfclocators.push_back((*value));
    }

    //mCARDRollCallRecord.rfiddeviceid = record->rfiddeviceid;
    //mCARDRollCallRecord.nfclocator = record->nfclocator;

    mCARDRollCallRecord.present = record->present;
    mCARDRollCallRecord.datetime = record->datetime;
    mCARDRollCallRecord.updatetime = record->updatetime;
    mCARDRollCallRecord.maingroups = record->maingroups;
}

// DataItem::DataItem(const char* type, int projectID, CARDScanRecordStruct * record)
//     : m_type(type), m_projectID(projectID)
// {
//     mNodeList = NULL;
//     mCARDScanRecord.cardid = record->cardid;
//     mCARDScanRecord.deviceid = record->deviceid;
//     mCARDScanRecord.LoRAMacaddress = record->LoRAMacaddress;
//     mCARDScanRecord.datetime = record->datetime;
// }

DataItem::DataItem(const char* type, int projectID, GAESurveyResultData * record)
    : m_type(type), m_projectID(projectID)
{
    mNodeList = NULL;
    mGAESurveyResultData.hashkey = record->hashkey;
    mGAESurveyResultData.token = record->token;
    mGAESurveyResultData.topic = record->topic;
    mGAESurveyResultData.messagehashkey = record->messagehashkey;
    mGAESurveyResultData.comment = record->comment;
    mGAESurveyResultData.datetime = record->datetime;

    for (std::list<std::string>::iterator result = record->results.begin(); result != record->results.end(); result++)
    {
        mGAESurveyResultData.results.push_back((*result));
    }
}

DataItem::DataItem(const char* type, int projectID, GAEMessageStatusData * record)
    : m_type(type), m_projectID(projectID)
{
    mNodeList = NULL;
    mGAEMessageStatusData.messageid = record->messageid;
    mGAEMessageStatusData.topic = record->topic;
    mGAEMessageStatusData.messagehashkey = record->messagehashkey;
    mGAEMessageStatusData.token = record->token;
    mGAEMessageStatusData.read = record->read;
    mGAEMessageStatusData.finished = record->finished;
    mGAEMessageStatusData.datetime = record->datetime;
}

DataItem::DataItem(const char* type, int projectID)
    : m_type(type), m_projectID(projectID)
{
    mNodeList = NULL;
}

DataItem::~DataItem()
{
    if (mNodeList != NULL)
    {
        clear_node_list(mNodeList);
        delete mNodeList;
    }
    m_distList.clear();
    m_stringList.clear();
    mGAESurveyResultData.results.clear();
}

void DataItem::setIPAddress(const char * IP)
{
    m_IPAddress = IP;
}

void DataItem::setATcommand(std::string cmd)
{
    m_atcmd = cmd;
}

void DataItem::setWaitkey(std::string waitkey)
{
    m_waitkey = waitkey;
}

void DataItem::setResponseStatus(int status)
{
    m_responsestatus = status;
}

void  DataItem::setMacaddress(const char * macaddress)
{
    m_macaddress = macaddress;
}

void DataItem::setLoRAMacaddress(const char * LoRA_macaddress)
{
    m_LoRAMacaddress = LoRA_macaddress;
}

void DataItem::setGatewayid(const char* gatewayid)
{
    m_gatewayid = gatewayid;
}

void DataItem::setMessage(std::string Message)
{
    m_message = Message;
}

void DataItem::setGroupID(int GroupID)
{
    m_GroupID = GroupID;
}

void DataItem::setPointID(int PointID)
{
    m_PointID = PointID;
}

void DataItem::setCardid(std::string cardid)
{
    m_cardid = cardid;
}

void DataItem::setRecorderID(std::string recorderid)
{
    m_recorderid = recorderid;
}

void DataItem::setWeight(std::string weight)
{
    m_weight = weight;
}

void DataItem::setHeartBeat(std::string heartbeat)
{
    m_heartbeat = heartbeat;
}

void DataItem::setBloodSugar(std::string bloodsugar)
{
    m_bloodsugar = bloodsugar;
}

void DataItem::setSystolicBloodPressure(std::string systolicbloodpressure)
{
    m_systolicbloodpressure = systolicbloodpressure;
}

void DataItem::setDiastolicBloodPressure(std::string diastolicbloodpressure)
{
    m_diastolicbloodpressure = diastolicbloodpressure;
}

void DataItem::setBodyTemperature(std::string bodytemperature)
{
    m_bodytemperature = bodytemperature;
}

void DataItem::setBodyFat(std::string bodyfat)
{
    m_bodyfat = bodyfat;
}

void DataItem::setBloodOxygen(std::string bloodoxygen)
{
    m_bloodoxygen = bloodoxygen;
}

void DataItem::setDateTime(std::string datetime)
{
    m_datetime = datetime;
}

void DataItem::setUserId(int userid)
{
    m_userid = userid;
}

void DataItem::setAOA_lv(std::string lv, std::string time)
{
    m_AOA_lv = lv;
    m_AOA_time = time;
}

void DataItem::setDelayTime(int delaytime)
{
    m_delaytime = delaytime;
}

void DataItem::setIMU(int * IMU)
{
    for (int i = 0; i < 6; i++)
        m_IMU[i] = IMU[i];
}

const char* DataItem::getType()
{
    return m_type.c_str();
}

int DataItem::getProjectID()
{
    return m_projectID;
}

const char* DataItem::getNodeId()
{
    return m_nodeid.c_str();
}

// alive
const char* DataItem::getmacaddress()
{
    return m_macaddress.c_str();
}

const char* DataItem::getVoltage()
{
    return m_voltage.c_str();
}

const char* DataItem::getTemperature()
{
    return m_temperature.c_str();
}

int DataItem::getCount()
{
    return m_count;
}

const char* DataItem::getPANID()
{
    return m_panid.c_str();
}

// join
int DataItem::getNodeType()
{
    return m_NodeType;
}

const char* DataItem::getVersion()
{
    return m_Version.c_str();
}

// ranging
std::string DataItem::getAnchorMac()
{
    return m_AnchorMac;
}

std::string DataItem::getTagMac()
{
    return m_TagMac;
}

int DataItem::getAnchorIdx()
{
    return m_Anchor_idx;
}

int DataItem::getTagIdx()
{
    return m_Tag_idx;
}

int DataItem::getRangingIdx()
{
    return m_Ranging_idx;
}

int DataItem::getDistance()
{
    return m_Distance;
}

int DataItem::getVelocity()
{
    return m_Velocity;
}

int DataItem::getTagIMU(int * IMU)
{
    for (int i = 0; i < 6; i++)
        IMU[i] = m_IMU[i];
    return 6;
}

int DataItem::getTagPollCnt()
{
    return m_TagPollCnt;
}

int DataItem::getRangingAnchorCnt()
{
    return m_RangingAnchorCnt;
}

int DataItem::getTagFinalRecvdBm()
{
    return m_TagFinalRecvdBm;
}

int DataItem::getTagFirstPathdBm()
{
    return m_TagFirstPathdBm;
}

int DataItem::getAnchorRecvdBm()
{
    return m_AnchorRecvdBm;
}

int DataItem::getAnchorFirstPathdBm()
{
    return m_AnchorFirstPathdBm;
}

int DataItem::getRangingCount()
{
    return m_RangingCount;
}

int DataItem::getRangingFailCount()
{
    return m_RangingFailCount;
}

float DataItem::getDataRate()
{
    return m_DataRate;
}

int DataItem::getRSSI()
{
    return m_rssi;
}

// char * DataItem::getAnchorRangingTable()
// {
//     return m_AnchorRangingTable;
// }

std::list<NodeStruct*> * DataItem::get_Node_list()
{
    return mNodeList;
}

// valid ranging anchor list
string DataItem::getValidAnchors()
{
    return validAnchors;
}

int DataItem::getTagWindowTime()
{
    return tag_window_time;
}

int DataItem::getTagResetTime()
{
    return tag_reset_time;
}

int DataItem::getTimeout()
{
    return m_timeout;
}

const char* DataItem::getTagId()
{
    return m_tagid.c_str();
}

const char* DataItem::getAnchorId()
{
    return m_anchorid.c_str();
}

const char * DataItem::getLoRAMacaddress()
{
    return m_LoRAMacaddress.c_str();
}

const char * DataItem::getGatewayid()
{
    return m_gatewayid.c_str();
}

const char * DataItem::getIPAddress()
{
    return m_IPAddress.c_str();
}

const char * DataItem::getTimestamp()
{
    return m_timestamp.c_str();
}

time_t DataItem::getTimestamp_t()
{
    return m_timestamp_t;
}

const char* DataItem::getSystemtime()
{
    return m_systemtime.c_str();
}

int DataItem::getRange1()
{
    return m_range1;
}

int DataItem::getRange2()
{
    return m_range2;
}

int DataItem::getDistInterval()
{
    return m_distInterval;
}

float DataItem::getAngle()
{
    return m_angle;
}

std::string DataItem::getATcommand()
{
    return m_atcmd;
}

std::string DataItem::getWaitkey()
{
    return m_waitkey;
}

int DataItem::getResponseStatus()
{
    return m_responsestatus;
}

std::string DataItem::getMessage()
{
    return m_message;
}

std::string DataItem::getRecorderID()
{
    return m_recorderid;
}

std::string DataItem::getWeight()
{
    return m_weight;
}

std::string DataItem::getHeartBeat()
{
    return m_heartbeat;
}

std::string DataItem::getBloodSugar()
{
    return m_bloodsugar;
}

std::string DataItem::getSystolicBloodPressure()
{
    return m_systolicbloodpressure;
}

std::string DataItem::getDiastolicBloodPressure()
{
    return m_diastolicbloodpressure;
}

std::string DataItem::getBodyTemperature()
{
    return m_bodytemperature;
}

std::string DataItem::getBodyFat()
{
    return m_bodyfat;
}

std::string DataItem::getBloodOxygen()
{
    return m_bloodoxygen;
}

std::string DataItem::getDateTime()
{
    return m_datetime;
}

int DataItem::getGroupID()
{
    return m_GroupID;
}

int DataItem::getPointID()
{
    return m_PointID;
}

std::string DataItem::getCardid()
{
    return m_cardid;
}

int DataItem::getUserId()
{
    return m_userid;
}

std::string DataItem::getAOA_lv()
{
    return m_AOA_lv;
}

std::string DataItem::getAOA_time()
{
    return m_AOA_time;
}

int DataItem::getDelayTime()
{
    return m_delaytime;
}
