/*
 *  MQTTDataItem.cpp
 *
 *  Created on: 2018/08/06
 *      Author: ikki
 */
 #include <stdio.h>
 #include <stdlib.h>
 #include "string.h"
 #include <iostream>
 #include <sstream>

#include "MQTTDataItem.h"

MQTTDataItem::MQTTDataItem(const char* type, int projectID, const char* loramacaddress, const char* tagmacaddress, const char* gatewayid, const char* timestamp)
    : m_type(type), m_projectID(projectID), m_LoRAMacaddress(loramacaddress), m_TagMacaddress(tagmacaddress), m_gatewayid(gatewayid), m_timestamp(timestamp)
{
    m_NodeType = NODE_TYPE_TAG;
}

// ranging
MQTTDataItem::MQTTDataItem(const char* type, int projectID, const char* loramacaddress, const char* tagmacaddress, int ranging, const char* voltage, const char* gatewayid, int rssi, const char* timestamp)
    : m_type(type), m_projectID(projectID), m_LoRAMacaddress(loramacaddress), m_TagMacaddress(tagmacaddress), m_ranging(ranging), m_voltage(voltage), m_gatewayid(gatewayid), m_rssi(rssi), m_timestamp(timestamp)
{
    m_NodeType = NODE_TYPE_TAG;
}

// MQTTDataItem::MQTTDataItem(const char* type, const char* loramacaddress, const char* tagmacaddress, int ranging, const char* voltage, int * imu, const char* gatewayid, const char* timestamp)
//     : m_type(type), m_LoRAMacaddress(loramacaddress), m_TagMacaddress(tagmacaddress), m_ranging(ranging), m_voltage(voltage), m_gatewayid(gatewayid), m_timestamp(timestamp)
// {
//     m_NodeType = NODE_TYPE_TAG;
//     for(int i=0;i<6;i++)
//     {
//         m_imu[i] = imu[i];
//     }
// }

// rangingcompress
MQTTDataItem::MQTTDataItem(const char* type, int projectID, const char* loramacaddress, const char* tagmacaddress, int ranging, const char* voltage, int velocity, const char* gatewayid, int rssi, const char* timestamp)
    : m_type(type), m_projectID(projectID), m_LoRAMacaddress(loramacaddress), m_TagMacaddress(tagmacaddress), m_ranging(ranging), m_voltage(voltage), m_velocity(velocity), m_gatewayid(gatewayid), m_rssi(rssi), m_timestamp(timestamp)
{
    m_NodeType = NODE_TYPE_TAG;
}

// rangingcompressaoa
MQTTDataItem::MQTTDataItem(const char* type, int projectID, const char* loramacaddress, const char* tagmacaddress, int ranging, const char* voltage, int velocity, float angle, const char* gatewayid, int rssi, const char* timestamp)
    : m_type(type), m_projectID(projectID), m_LoRAMacaddress(loramacaddress), m_TagMacaddress(tagmacaddress), m_ranging(ranging), m_voltage(voltage), m_velocity(velocity), m_angle(angle), m_gatewayid(gatewayid), m_rssi(rssi), m_timestamp(timestamp)
{
    m_NodeType = NODE_TYPE_TAG;
}

MQTTDataItem::MQTTDataItem(const char* type, int projectID, const char* loramacaddress, const char* tagmacaddress, int distInterval, std::list<int> * distList, const char* voltage, int velocity, const char* gatewayid, int rssi, const char* timestamp)
    : m_type(type), m_projectID(projectID), m_LoRAMacaddress(loramacaddress), m_TagMacaddress(tagmacaddress), m_distInterval(distInterval), m_voltage(voltage), m_velocity(velocity), m_gatewayid(gatewayid), m_rssi(rssi), m_timestamp(timestamp)
{
    for (std::list<int>::iterator i = distList->begin(); i != distList->end(); i++)
    {
        m_distList.push_back((*i));
    }
}

MQTTDataItem::MQTTDataItem(const char* type, int projectID, const char* loramacaddress, const char* tagmacaddress, double Longitude, double Latitude, const char* gatewayid, const char* timestamp)
    : m_type(type), m_projectID(projectID), m_LoRAMacaddress(loramacaddress), m_TagMacaddress(tagmacaddress), m_Longitude(Longitude), m_Latitude(Latitude), m_gatewayid(gatewayid), m_timestamp(timestamp)
{
    m_NodeType = NODE_TYPE_TAG;
}

MQTTDataItem::MQTTDataItem(const char* type, int projectID, LoRaGatewayInfoStruct * gateway)
    : m_type(type), m_projectID(projectID)
{
    mLoRaGateway.sf = gateway->sf;
    mLoRaGateway.projectid = gateway->projectid;
    mLoRaGateway.gwip = gateway->gwip;
    mLoRaGateway.gwid = gateway->gwid;
    mLoRaGateway.repeater = gateway->repeater;
    mLoRaGateway.maplayer = gateway->maplayer;
    mLoRaGateway.posX = gateway->posX;
    mLoRaGateway.posY = gateway->posY;
    mLoRaGateway.posZ = gateway->posZ;
    mLoRaGateway.alive = gateway->alive;// alive count
    mLoRaGateway.MQTTalive = gateway->MQTTalive;// MQTT alive count
    mLoRaGateway.pingCount = gateway->pingCount;
    mLoRaGateway.aliveTime = gateway->aliveTime;
    mLoRaGateway.maingroupid = gateway->maingroupid;

    //mLoRaGateway.channel = gateway->channel;
    // copy list
    for (std::list<std::string>::iterator channel = gateway->channels.begin(); channel != gateway->channels.end(); channel++)
    {
        mLoRaGateway.channels.push_back((*channel));
    }
}

MQTTDataItem::MQTTDataItem(const char* type, int projectID, const char* gatewayid, const char* payload, int payload_length)
    : m_type(type), m_projectID(projectID), m_gatewayid(gatewayid), m_payload(payload), m_payload_length(payload_length)
{

}

// Locator boot msg
MQTTDataItem::MQTTDataItem(const char* type, int projectID, const char* loramacaddress, const char* macaddress, std::string PANID, std::string MODE, std::string VERSION)
    : m_type(type), m_projectID(projectID), m_LoRAMacaddress(loramacaddress), m_Macaddress(macaddress), m_PANID(PANID), m_MODE(MODE), m_VERSION(VERSION)
{
    m_NodeType = NODE_TYPE_LOCATOR;
}

MQTTDataItem::MQTTDataItem(const char* type, int projectID, const char* loramacaddress, const char* macaddress)
    : m_type(type), m_projectID(projectID), m_LoRAMacaddress(loramacaddress), m_Macaddress(macaddress)
{
    m_NodeType = NODE_TYPE_LOCATOR;
}

MQTTDataItem::MQTTDataItem(const char* type, int projectID, const char* loramacaddress, std::string data)
    : m_type(type), m_projectID(projectID), m_LoRAMacaddress(loramacaddress), m_Data(data)
{

}


MQTTDataItem::MQTTDataItem(const char* type, int projectID, const char* loramacaddress)
    : m_type(type), m_projectID(projectID), m_LoRAMacaddress(loramacaddress)
{
    m_NodeType = NODE_TYPE_LOCATOR;
}

MQTTDataItem::MQTTDataItem(const char* type, int projectID, RFIDDeviceStruct * device)
    : m_type(type), m_projectID(projectID)
{
    m_RFIDDevice.alive = device->alive;
    m_RFIDDevice.deviceid = device->deviceid;
    m_RFIDDevice.macaddress = device->macaddress;
    m_RFIDDevice.name = device->name;
    m_RFIDDevice.type = device->type;
    m_RFIDDevice.ip = device->ip;
    m_RFIDDevice.gwid = device->gwid;
    m_RFIDDevice.LoRAMacaddress = device->LoRAMacaddress;
    m_RFIDDevice.datetime = device->datetime;
    m_RFIDDevice.maingroupid = device->maingroupid;
}

MQTTDataItem::MQTTDataItem(const char* type, int projectID, NFCDeviceStruct * device)
    : m_type(type), m_projectID(projectID)
{
    m_NFCDevice.alive = device->alive;
    m_NFCDevice.deviceid = device->deviceid;
    m_NFCDevice.macaddress = device->macaddress;
    m_NFCDevice.name = device->name;
    m_NFCDevice.type = device->type;
    m_NFCDevice.gwid = device->gwid;
    m_NFCDevice.LoRAMacaddress = device->LoRAMacaddress;
    m_NFCDevice.version = device->version;
    m_NFCDevice.datetime = device->datetime;
    m_NFCDevice.ssid = device->ssid;
    m_NFCDevice.wifimac = device->wifimac;
    m_NFCDevice.verifykey = device->verifykey;
    m_NFCDevice.maingroupid = device->maingroupid;
}

MQTTDataItem::MQTTDataItem(const char* type, int projectID, RFIDCardStruct * card)
    : m_type(type), m_projectID(projectID)
{
    m_RFIDCard.cardid = card->cardid;
    m_RFIDCard.userid = card->userid;
    m_RFIDCard.datetime = card->datetime;
    m_RFIDCard.updatetime = card->updatetime;
    m_RFIDCard.maingroupid = card->maingroupid;
}

MQTTDataItem::MQTTDataItem(const char* type, int projectID, NFCCardStruct * card)
    : m_type(type), m_projectID(projectID)
{
    m_NFCCard.cardid = card->cardid;
    m_NFCCard.userid = card->userid;
    m_NFCCard.datetime = card->datetime;
    m_NFCCard.updatetime = card->updatetime;
    m_NFCCard.maingroupid = card->maingroupid;
}

MQTTDataItem::MQTTDataItem(const char* type, int projectID, EDGEDeviceStruct * device)
    : m_type(type), m_projectID(projectID)
{
    m_EdgeDevice.alive = device->alive;
    m_EdgeDevice.deviceid = device->deviceid;
    m_EdgeDevice.macaddress = device->macaddress;
    m_EdgeDevice.name = device->name;
    m_EdgeDevice.type = device->type;
    m_EdgeDevice.ip = device->ip;
    m_EdgeDevice.LoRAMacaddress = device->LoRAMacaddress;
    m_EdgeDevice.gwid = device->gwid;
    m_EdgeDevice.version = device->version;
    m_EdgeDevice.datetime = device->datetime;
    m_EdgeDevice.aliveTime = device->aliveTime;
    m_EdgeDevice.maingroupid = device->maingroupid;
}

MQTTDataItem::MQTTDataItem(const char* type, int projectID, SensorDeviceStruct * device)
    : m_type(type), m_projectID(projectID)
{
    m_SensorDevice.alive = device->alive;
    m_SensorDevice.maingroupid = device->maingroupid;
    m_SensorDevice.id = device->id;
    m_SensorDevice.name = device->name;
    m_SensorDevice.type = device->type;
    m_SensorDevice.jsonvalue = device->jsonvalue;
    m_SensorDevice.Longitude = device->Longitude;
    m_SensorDevice.Latitude = device->Latitude;
    m_SensorDevice.Battery = device->Battery;
    m_SensorDevice.Version = device->Version;
    m_SensorDevice.datetime = device->datetime;
}

MQTTDataItem::MQTTDataItem(const char* type, int projectID, std::shared_ptr<wentaitek_s3_watch_deviceStruct> device)
    : m_type(type), m_projectID(projectID)
{
    m_wentaitek_s3_watch_device.Device_ID = device->Device_ID;
    m_wentaitek_s3_watch_device.Blood_Oxygen = device->Blood_Oxygen;
    m_wentaitek_s3_watch_device.DBP = device->DBP;
    m_wentaitek_s3_watch_device.Heart_Rate = device->Heart_Rate;
    m_wentaitek_s3_watch_device.Rssi = device->Rssi;
    m_wentaitek_s3_watch_device.SBP = device->SBP;
    m_wentaitek_s3_watch_device.SOS = device->SOS;
    m_wentaitek_s3_watch_device.Temperature = device->Temperature;
    m_wentaitek_s3_watch_device.repeater_ip = device->repeater_ip;
    m_wentaitek_s3_watch_device.repeater_mac = device->repeater_mac;
}

MQTTDataItem::MQTTDataItem(const char* type, int projectID, std::shared_ptr<wentaitek_uw_sensor_1_deviceStruct> device)
    : m_type(type), m_projectID(projectID)
{
    m_wentaitek_uw_sensor_1_device.Device_ID = device->Device_ID;
    m_wentaitek_uw_sensor_1_device.Rssi = device->Rssi;
    m_wentaitek_uw_sensor_1_device.repeater_ip = device->repeater_ip;
    m_wentaitek_uw_sensor_1_device.repeater_mac = device->repeater_mac;
}

MQTTDataItem::MQTTDataItem(const char* type, int projectID, std::shared_ptr<wentaitek_lightStruct> device)
    : m_type(type), m_projectID(projectID)
{
    m_wentaitek_light.id = device->id;
    m_wentaitek_light.name = device->name;
    m_wentaitek_light.uniAddress = device->uniAddress;
    m_wentaitek_light.state_onOff = device->state_onOff;
    m_wentaitek_light.state_level = device->state_level;
    m_wentaitek_light.state_cct = device->state_cct;
}

MQTTDataItem::MQTTDataItem(const char* type, int projectID, std::shared_ptr<wentaitek_power_meter_deviceStruct> device)
    : m_type(type), m_projectID(projectID)
{
    vilslog::printf("MQTTDataItem::PowerMeter IN");
    m_wentaitek_power_meter_device.Device_ID = device->Device_ID;
    m_wentaitek_power_meter_device.BT_version = device->BT_version;
    m_wentaitek_power_meter_device.Rssi = device->Rssi;
    m_wentaitek_power_meter_device.Active_Energy = device->Active_Energy;
    m_wentaitek_power_meter_device.Active_Power = device->Active_Power;
    m_wentaitek_power_meter_device.Current_RMS = device->Current_RMS;
    m_wentaitek_power_meter_device.Frequency = device->Frequency;
    m_wentaitek_power_meter_device.Voltage_RMS = device->Voltage_RMS;
    m_wentaitek_power_meter_device.relay_state = device->relay_state;
    m_wentaitek_power_meter_device.repeater_ip = device->repeater_ip;
    m_wentaitek_power_meter_device.repeater_mac = device->repeater_mac;
    vilslog::printf("MQTTDataItem::PowerMeter OUT");

}

MQTTDataItem::~MQTTDataItem()
{
    m_distList.clear();
}

void MQTTDataItem::setLoRAMacaddress(const char* loramacaddress)
{
    m_LoRAMacaddress = loramacaddress;
}

void MQTTDataItem::setGatewayid(const char* gatewayid)
{
    m_gatewayid = gatewayid;
}

void MQTTDataItem::setTagMacaddress(const char* macaddress)
{
    m_TagMacaddress = macaddress;
}

void MQTTDataItem::setRSSI(int rssi)
{
    m_rssi = rssi;
}

void MQTTDataItem::setTimestamp(const char* timestamp)
{
    m_timestamp = timestamp;
}

void MQTTDataItem::setAOA_lv(const char* lv, const char* time)
{
    m_AOA_lv = lv;
    m_AOA_time = time;
}

void MQTTDataItem::setAOA_gridsize(int gridsize)
{
    m_AOA_gridsize = gridsize;
}

void MQTTDataItem::setIMU(int * IMU)
{
    for (int i = 0; i < 6; i++)
        m_IMU[i] = IMU[i];
}

const char* MQTTDataItem::getType()
{
    return m_type.c_str();
}

int MQTTDataItem::getProjectID()
{
    return m_projectID;
}

const char* MQTTDataItem::getLoRAMacaddress()
{
    return m_LoRAMacaddress.c_str();
}

const char* MQTTDataItem::getMacaddress()
{
    return m_Macaddress.c_str();
}

const char* MQTTDataItem::getTagMacaddress()
{
    return m_TagMacaddress.c_str();
}

int MQTTDataItem::getNodeType()
{
    return m_NodeType;
}

int MQTTDataItem::getRanging()
{
    return m_ranging;
}

const char* MQTTDataItem::getTagVoltage()
{
    return m_voltage.c_str();
}

int MQTTDataItem::getTagVelocity()
{
    return m_velocity;
}

// void MQTTDataItem::getTagIMU(int * imu)
// {
//     for(int i=0;i<6;i++)
//     {
//         imu[i] = m_imu[i];
//     }
// }

int MQTTDataItem::getTagRSSI()
{
    return m_rssi;
}

double MQTTDataItem::getLongitude()
{
    return m_Longitude;
}

double MQTTDataItem::getLatitude()
{
    return m_Latitude;
}

const char* MQTTDataItem::getGatewayid()
{
    return m_gatewayid.c_str();
}

const char* MQTTDataItem::getTimestamp()
{
    return m_timestamp.c_str();
}

const char* MQTTDataItem::getPayload()
{
    return m_payload.c_str();
}

int MQTTDataItem::getPayloadLength()
{
    return m_payload_length;
}

int MQTTDataItem::getDistInterval()
{
    return m_distInterval;
}

const char* MQTTDataItem::getPANID()
{
    return m_PANID.c_str();
}

const char* MQTTDataItem::getMODE()
{
    return m_MODE.c_str();
}

const char* MQTTDataItem::getVERSION()
{
    return m_VERSION.c_str();
}

const char* MQTTDataItem::getData()
{
    return m_Data.c_str();
}

float MQTTDataItem::getAngle()
{
    return m_angle;
}

const char* MQTTDataItem::getAOA_lv()
{
    return m_AOA_lv.c_str();
}

const char* MQTTDataItem::getAOA_time()
{
    return m_AOA_time.c_str();
}

int MQTTDataItem::getAOA_gridsize()
{
    return m_AOA_gridsize;
}

int MQTTDataItem::getIMU(int * IMU)
{
    for (int i = 0; i < 6; i++)
        IMU[i] = m_IMU[i];
    return 6;
}
