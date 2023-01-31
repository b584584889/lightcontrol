/*
 * DataItem.h
 *
 *  Created on: 2017/02/18
 *      Author: ikki
 */
#ifndef __DataItem_h__
#define __DataItem_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "../common.h"

using namespace std;

class DataItem
{
private:
    string m_type;
    int m_projectID;
    int m_NodeType;// 0:Anchor 1:Tag 2:Coordinator
    string m_nodeid;

    // alive
    string m_macaddress;
    string m_voltage;
    string m_temperature;
    int m_count;
    string m_panid;

    string m_Version;

    // ranging
    std::string m_AnchorMac;
    std::string m_TagMac;
    int m_Anchor_idx;
    int m_Tag_idx;
    int m_Ranging_idx;
    int m_Distance;

    int m_distInterval;

    int m_Velocity;
    int m_IMU[6];

    // rangingdiag
    int m_TagPollCnt;       // Tag ranging poll count
    int m_RangingAnchorCnt;     // ranging Anchor count
    int m_TagFinalRecvdBm; // Tag ->  Anchor receive signal power
    int m_TagFirstPathdBm; // Tag ->  Anchor signal power in the first path
    int m_AnchorRecvdBm;   // Anchor -> Coord receive signal power
    int m_AnchorFirstPathdBm; //  Anchor -> Coord signal power in the first path
    ///char m_AnchorRangingTable[256];
    int m_RangingCount;
    int m_RangingFailCount;
    float m_DataRate;

    // valid ranging anchor list
    string validAnchors;

    //AT+STAGSCHDL=10,200\r\n
    int tag_window_time;// 10
    int tag_reset_time;// 200
    int m_timeout;

    // checkpoint using
    string m_tagid;
    string m_anchorid;

    float m_angle;
    string m_LoRAMacaddress;
    string m_gatewayid;
    int m_rssi;

    string m_IPAddress;
    string m_timestamp;
    time_t m_timestamp_t;

    int m_range1;
    int m_range2;

    std::list<NodeStruct*> * mNodeList;

    string m_systemtime;
    string m_atcmd;
    string m_waitkey;
    int m_responsestatus;
    std::string m_message;

    std::string m_recorderid;
    std::string m_weight;
    std::string m_heartbeat;
    std::string m_bloodsugar;
    std::string m_systolicbloodpressure;
    std::string m_diastolicbloodpressure;
    std::string m_bodytemperature;
    std::string m_bodyfat;
    std::string m_bloodoxygen;
    std::string m_datetime;

    int m_GroupID; // InspectionGroup
    int m_PointID; // InspectionPoint

    std::string m_cardid;
    int m_userid;

    // AOAfence
    string m_AOA_lv;
    string m_AOA_time;

    int m_delaytime;// in seconds

public:
    DataItem(const char* type, int projectID, const char* nodeid);
    DataItem(const char* type, int projectID, const char* macaddress, int nodetype);
    DataItem(const char* type, int projectID, const char* macaddress, int nodetype, const char* voltage, const char* temperature, int count, const char* panid);
    DataItem(const char* type, int projectID, const char* macaddress, int nodetype, const char* version);
    DataItem(const char* type, int projectID, const char* macaddress, string anchoridxs);
    DataItem(const char* type, int projectID, const char* macaddress, const char * ipAddress);
    DataItem(const char* type, int projectID, const char* macaddress, const char * ipAddress, string anchoridxs);
    DataItem(const char* type, int projectID, const char* macaddress, const char * ipAddress, int range1, int range2);
    DataItem(const char* type, int projectID, const char* tagid, const char* anchorid, int distance, int dummy, time_t timestamp);
    DataItem(const char* type, int projectID, const char* tagid, const char* anchorid, int distance, int dummy, const char * timestamp);
    DataItem(const char* type, int projectID, const char * LoRAPrefix, const char * LoRA_macaddress, const char * Tag_macaddress, int Distance, int rssi, const char * timestamp);
    DataItem(const char* type, int projectID, const char * LoRAPrefix, const char * LoRA_macaddress, const char * Tag_macaddress, int Distance, int velocity, int rssi, const char * timestamp);
    DataItem(const char* type, int projectID, const char * LoRAPrefix, const char * LoRA_macaddress, const char * Tag_macaddress, int Distance, int velocity, float angle, int rssi, const char * timestamp);
    DataItem(const char* type, int projectID, const char * LoRA_macaddress, const char * Tag_macaddress, int distInterval, std::list<int> * distList, int velocity, int rssi, const char * timestamp);


    DataItem(const char* type, int projectID, int nodetype, std::list<NodeStruct*> * plist);
    // DataItem(const char* type, int projectID, int anchoridx, int tagidx, int rangingidx, int distance, int * imu);
    DataItem(const char* type, int projectID, int anchoridx, int tagidx, int rangingidx, int distance, int * imu,
                        int TagPollCnt, int RangingAnchorCnt, int TagFinalRecvdBm, int TagFirstPathdBm,
                        int AnchorRecvdBm, int AnchorFirstPathdBm, int RangingCount, int RangingFailCount, float DataRate, const char * IP);
    DataItem(const char* type, int projectID, std::string anchor_mac, std::string tag_mac, int rangingidx, int distance, int * imu,
                        int TagPollCnt, int RangingAnchorCnt, int TagFinalRecvdBm, int TagFirstPathdBm,
                        int AnchorRecvdBm, int AnchorFirstPathdBm, int RangingCount, int RangingFailCount, float DataRate, const char * IP);
    DataItem(const char* type, int projectID, int dummy, const char* systemtime);
    DataItem(const char* type, int projectID, int window_time, int reset_time);
    // DataItem(const char* type, int projectID, int tagidx, const char* anchorid, int distance);
    // DataItem(const char* type, int projectID, int tagidx, string anchoridxs);
    DataItem(const char* type, int projectID, int time_out);
    DataItem(const char* type, int projectID, std::list<std::string> * stringList);
    DataItem(const char* type, int projectID, SendNotifyStatusStruct * status);
    DataItem(const char* type, int projectID, CARDRollCallRecordStruct * record);
    // DataItem(const char* type, int projectID, CARDScanRecordStruct * record);
    DataItem(const char* type, int projectID, GAESurveyResultData * record);
    DataItem(const char* type, int projectID, GAEMessageStatusData * record);
    DataItem(const char* type, int projectID);
    ~DataItem();

    void setIPAddress(const char * IP);
    void setATcommand(std::string cmd);
    void setWaitkey(std::string waitkey);
    void setResponseStatus(int status);
    void setMacaddress(const char * macaddress);
    void setLoRAMacaddress(const char * LoRA_macaddress);
    void setGatewayid(const char* gatewayid);
    void setMessage(std::string Message);
    void setGroupID(int GroupID);
    void setPointID(int PointID);
    void setCardid(std::string cardid);

    void setRecorderID(std::string recorderid);
    void setWeight(std::string weight);
    void setHeartBeat(std::string heartbeat);
    void setBloodSugar(std::string bloodsugar);
    void setSystolicBloodPressure(std::string systolicbloodpressure);
    void setDiastolicBloodPressure(std::string diastolicbloodpressure);
    void setBodyTemperature(std::string bodytemperature);
    void setBodyFat(std::string bodyfat);
    void setBloodOxygen(std::string bloodoxygen);
    void setDateTime(std::string datetime);
    void setUserId(int userid);
    void setAOA_lv(std::string lv, std::string time);
    void setDelayTime(int delaytime);
    void setIMU(int * IMU);
    
    const char* getType();
    int getProjectID();
    const char* getNodeId();

    // alive
    const char* getmacaddress();
    const char* getVoltage();
    const char* getTemperature();
    int getCount();
    const char* getPANID();

    // join
    int getNodeType();
    const char* getVersion();

    // ranging
    std::string getAnchorMac();
    std::string getTagMac();
    int getAnchorIdx();
    int getTagIdx();
    int getRangingIdx();
    int getDistance();
    int getVelocity();
    int getTagIMU(int * IMU);

    // rangingdiag
    int getTagPollCnt();      // Tag ranging poll count
    int getRangingAnchorCnt();     // ranging Anchor count
    int getTagFinalRecvdBm(); // Tag ->  Anchor receive signal power
    int getTagFirstPathdBm(); // Tag ->  Anchor signal power in the first path
    int getAnchorRecvdBm();   // Anchor -> Coord receive signal power
    int getAnchorFirstPathdBm(); //  Anchor -> Coord signal power in the first path
    //char * getAnchorRangingTable();
    int getRangingCount();
    int getRangingFailCount();
    float getDataRate();
    int getRSSI();

    // valid ranging anchor list
    string getValidAnchors();
    //AT+STAGSCHDL=10,200\r\n
    int getTagWindowTime();// 10
    int getTagResetTime();// 200
    int getTimeout();

    // checkpoint using
    const char* getTagId();
    const char* getAnchorId();

    const char* getLoRAMacaddress();
    const char* getGatewayid();
    const char* getIPAddress();

    const char* getTimestamp();
    time_t getTimestamp_t();

    const char* getSystemtime();

    int getRange1();
    int getRange2();

    std::list<NodeStruct*> * get_Node_list();
    std::list<int> m_distList;
    std::list<std::string> m_stringList;
    int getDistInterval();
    float getAngle();

    SendNotifyStatusStruct m_status;
    CARDRollCallRecordStruct mCARDRollCallRecord;
    // CARDScanRecordStruct mCARDScanRecord;
    GAESurveyResultData mGAESurveyResultData;
    GAEMessageStatusData mGAEMessageStatusData;

    std::string getATcommand();
    std::string getWaitkey();
    int getResponseStatus();
    std::string getMessage();

    std::string getRecorderID();
    std::string getWeight();
    std::string getHeartBeat();
    std::string getBloodSugar();
    std::string getSystolicBloodPressure();
    std::string getDiastolicBloodPressure();
    std::string getBodyTemperature();
    std::string getBodyFat();
    std::string getBloodOxygen();
    std::string getDateTime();

    int getGroupID();
    int getPointID();

    std::string getCardid();
    int getUserId();

    std::string getAOA_lv();
    std::string getAOA_time();

    int getDelayTime();
};

#endif
