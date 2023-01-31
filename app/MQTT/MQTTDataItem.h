/*
 * MQTTDataItem.h
 *
 *  Created on: 2018/08/06
 *      Author: ikki
 */
#ifndef __MQTTDataItem_h__
#define __MQTTDataItem_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "../common.h"

using namespace std;

class MQTTDataItem
{
private:
    string m_type;
    int m_projectID;
    int m_NodeType;// 0:Anchor 1:Tag 2:Coordinator

    string m_LoRAMacaddress;
    string m_Macaddress;
    string m_TagMacaddress;
    int m_ranging;

    int m_distInterval;

    string m_voltage;
    // string m_imu;
    int m_IMU[6];
    int m_velocity;

    double m_Longitude;
    double m_Latitude;

    float m_angle;

    string m_gatewayid;
    int m_rssi;
    string m_timestamp;

    std::string m_payload;
    int m_payload_length;

    string m_PANID;
    string m_MODE;
    string m_VERSION;

    string m_Data;

    // AOAfence
    string m_AOA_lv;
    string m_AOA_time;
    int m_AOA_gridsize;

public:
    MQTTDataItem(const char* type, int projectID, const char* loramacaddress, const char* tagmacaddress, const char* gatewayid, const char* timestamp);
    MQTTDataItem(const char* type, int projectID, const char* loramacaddress, const char* tagmacaddress, int ranging, const char* voltage, const char* gatewayid, int rssi, const char* timestamp);
    //MQTTDataItem(const char* type, const char* loramacaddress, const char* tagmacaddress, int ranging, const char* voltage, const char * imu);
    // MQTTDataItem(const char* type, const char* loramacaddress, const char* tagmacaddress, int ranging, const char* voltage, int * imu, const char* gatewayid, const char* timestamp);
    MQTTDataItem(const char* type, int projectID, const char* loramacaddress, const char* tagmacaddress, int ranging, const char* voltage, int velocity, const char* gatewayid, int rssi, const char* timestamp);
    MQTTDataItem(const char* type, int projectID, const char* loramacaddress, const char* tagmacaddress, int ranging, const char* voltage, int velocity, float angle, const char* gatewayid, int rssi, const char* timestamp);
    MQTTDataItem(const char* type, int projectID, const char* loramacaddress, const char* tagmacaddress, int distInterval, std::list<int> * distList, const char* voltage, int velocity, const char* gatewayid, int rssi, const char* timestamp);
    MQTTDataItem(const char* type, int projectID, const char* loramacaddress, const char* tagmacaddress, double Longitude, double Latitude, const char* gatewayid, const char* timestamp);
    MQTTDataItem(const char* type, int projectID, LoRaGatewayInfoStruct * gateway);
    MQTTDataItem(const char* type, int projectID, const char* gatewayid, const char* payload, int payload_length);
    MQTTDataItem(const char* type, int projectID, const char* loramacaddress, const char* macaddress, std::string PANID, std::string MODE, std::string VERSION);
    MQTTDataItem(const char* type, int projectID, const char* loramacaddress, const char* macaddress);
    MQTTDataItem(const char* type, int projectID, const char* loramacaddress, std::string data);
    MQTTDataItem(const char* type, int projectID, const char* loramacaddress);
    MQTTDataItem(const char* type, int projectID, RFIDDeviceStruct * device);
    MQTTDataItem(const char* type, int projectID, NFCDeviceStruct * device);
    MQTTDataItem(const char* type, int projectID, RFIDCardStruct * card);
    MQTTDataItem(const char* type, int projectID, NFCCardStruct * card);
    MQTTDataItem(const char* type, int projectID, EDGEDeviceStruct * device);
    MQTTDataItem(const char* type, int projectID, SensorDeviceStruct * device);
    MQTTDataItem(const char* type, int projectID, std::shared_ptr<wentaitek_s3_watch_deviceStruct> device);
    MQTTDataItem(const char* type, int projectID, std::shared_ptr<wentaitek_uw_sensor_1_deviceStruct> device);
    MQTTDataItem(const char* type, int projectID, std::shared_ptr<wentaitek_lightStruct> device);
    MQTTDataItem(const char* type, int projectID, std::shared_ptr<wentaitek_power_meter_deviceStruct> device);
    ~MQTTDataItem();

    void setLoRAMacaddress(const char* loramacaddress);
    void setGatewayid(const char* gatewayid);
    void setTagMacaddress(const char* macaddress);
    void setRSSI(int rssi);
    void setTimestamp(const char* timestamp);
    void setAOA_lv(const char* lv, const char* time);
    void setAOA_gridsize(int gridsize);
    void setIMU(int * IMU);

    const char* getType();
    int getProjectID();
    const char* getLoRAMacaddress();
    const char* getMacaddress();
    const char* getTagMacaddress();
    int getNodeType();
    int getRanging();
    const char* getTagVoltage();
    int getTagVelocity();
    // void getTagIMU(int * imu);
    int getTagRSSI();

    double getLongitude();
    double getLatitude();

    const char* getGatewayid();
    const char* getTimestamp();

    const char* getPayload();
    int getPayloadLength();

    int getDistInterval();
    std::list<int> m_distList;

    const char* getPANID();
    const char* getMODE();
    const char* getVERSION();

    const char* getData();

    float getAngle();

    const char* getAOA_lv();
    const char* getAOA_time();
    int getAOA_gridsize();
    int getIMU(int * IMU);

    LoRaGatewayInfoStruct mLoRaGateway;
    RFIDDeviceStruct m_RFIDDevice;
    NFCDeviceStruct m_NFCDevice;
    RFIDCardStruct m_RFIDCard;
    NFCCardStruct m_NFCCard;
    EDGEDeviceStruct m_EdgeDevice;
    SensorDeviceStruct m_SensorDevice;

    wentaitek_s3_watch_deviceStruct m_wentaitek_s3_watch_device;
    wentaitek_uw_sensor_1_deviceStruct m_wentaitek_uw_sensor_1_device;
    wentaitek_lightStruct m_wentaitek_light;
    wentaitek_power_meter_deviceStruct m_wentaitek_power_meter_device;
};

#endif
