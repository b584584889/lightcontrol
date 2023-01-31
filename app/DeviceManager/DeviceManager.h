/*
 * DeviceManager.h
 *
 *  Created on: 2017/02/20
 *      Author: ikki
 */


#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#include "../config.h"
#include "../common.h"
#include "../network/wqueue.h"
#include "../mysql/DBManager.h"
#include "AreaInOutDataClass.h"

// #define PRINT_AREA_INOUT_MSG

typedef struct
{
    int projectid;
    VILSSystemConfig * mSysConfig;
	std::list<NodeStruct*> * mAnchorNodeList; // node_type = 0
    std::list<NodeStruct*> * mTagNodeList; // node_type = 1
    std::list<NodeStruct*> * mCoordNodeList; // node_type = 2
    std::list<NodeStruct*> * mLocatorNodeList; // node_type = 3
    std::list<LoRaGatewayInfoStruct*> * mLoRaGatewayList;
    // std::vector<AreaInOutDataClass *> mAreaInOutDataVector;
    std::list<NodeGPSInfoStruct*> mNodeGPSInfoList;
    std::list<NodePositionInfoStruct*> mNodePositionInfoList;
	std::list<AoANodeStruct*> * mAoANodeList;

    std::list<NFCDeviceStruct*> * mNFCDeviceList;
    std::list<NFCCardStruct*> * mNFCCardList;
    std::list<NFCCardStruct*> * mMaingroupNFCCardList;
    std::list<RFIDDeviceStruct*> * mRFIDDeviceList;
    std::list<RFIDCardStruct*> * mRFIDCardList;
    // std::list<CARDScanRecordStruct*> * mCARDScanList;
    std::list<EDGEDeviceStruct*> * mEDGEDeviceList;
    std::list<LocatorRSSIInfoStruct*> * mLocatorRSSIInfoList;
    std::list<SensorDeviceStruct*> * mSensorDeviceList;
    std::list<UserStruct*> * mUserList;

} DeviceInfoListStruct;

class DeviceManager
{
public:
	DeviceManager(DBManager * dbManager);
	~DeviceManager();

    int resetAnchorIndexs(int projectid);
    int resetTagIndexs(int projectid);
    int resetCoordIndexs(int projectid);
    int resetLocatorIndexs(int projectid);
    int resetNodeIndexs(std::list<NodeStruct*> * plist);
    //int reloadConfig();
    std::list<int> * getAllProjectConfig();

    bool setTagOperationMode(int projectid, const char * Tag_macaddress, int mode);
    bool setTagOperationModeById(int projectid, const char * TagId, int mode);

    bool setTagRangingList(int projectid, const char * Tag_macaddress, const char * rangingList);

    bool setCoordPanid(int projectid, const char * Coord_macaddress, const char * PANID, const char * version);
    bool setTagIndex(int projectid, const char * Tag_macaddress, int Tag_idx, const char * version);
    bool setTagInfo(int projectid, const char * macaddress, const char * nodeid, int tafid, int buildingid, int buildingfloor, const char * nodename,
                        int idx, bool updatePos, double posX, double posY, double posZ, const char * version);
    bool setTagInfoPos(int projectid, const char * TagId, double posX, double posY, double posZ);
    bool setTagInfoS(int projectid, const char * TagId, double posX, double posY, double posZ, int maplayerid);
    bool setTagInfoB(int projectid, int tafid, double lng, double lat, int EPSG, int buildingID, int buildingFloor);
    bool setTagInfoIP(int projectid, const char * Tag_macaddress, const char * IP);
    bool setTagInfoPANID(int projectid, const char * Tag_macaddress, const char * PANID);
    bool setTagInfoMapLayer(int projectid, const char * Tag_macaddress, int maplayerid);
    bool setTagInfoMapLayerById(int projectid, const char * TagId, int maplayerid);
    bool setTagHeartBeat(int projectid, const char * Tag_macaddress, int HeartBeat);
    bool setTagWeight(int projectid, const char * Tag_macaddress, int Weight);
    bool setTagInfoRangingTarget(int projectid, const char * Tag_macaddress, const char * TargetID);
    bool setTagInfoGatewayID(int projectid, const char * Tag_macaddress, const char * GatewayID);
    bool setTagInfoRangingLoRAmac(int projectid, const char * Tag_macaddress, const char* loramacaddress);
    bool setTagInfoArea(int projectid, const char * TagId, const char * areaid, const char * areaname);
    // bool setTagInfoArea(int projectid, const char * TagId, const char * areaid, const char * areaname, int AreaType0D, int mapAreaType, const char * insideAreaId);
    // bool setTagInfoArea(int projectid, NodeStruct * node, AreaStruct * area, const char * insideAreaId);
    bool setTagInfoArea0D(int projectid, const char * TagId, const char * areaid, int AreaType0D);
    bool setTagInfoAreaType(int projectid, const char * TagId, int AreaType);
    bool setTagInfoAreaType5(int projectid, const char * TagId, const char * areaid, const char * areaname);
    // bool setTagInfoNotInArea(int projectid, const char * TagId, const char * areaid);
    bool setTagRollCallNotInArea(int projectid, const char * TagId, const char * areaid);
    bool setTagRangingResultType(int projectid, const char * TagId, int RangingResultType);

    // bool setTagInfoAreaR(int projectid, const char * TagId, const char * areaid, const char * areaname);
    bool setTagInfoGPSById(int projectid, const char * TagId, double Longitude, double Latitude);
    bool setTagInfoGPS(int projectid, const char * Tag_macaddress, double Longitude, double Latitude, const char * timestamp);
    bool setTagInfoActivity(int projectid, const char * Tag_macaddress, std::string activity);
    bool setLocatorInfoGPS(int projectid, const char * loramacaddress, double Longitude, double Latitude, const char * timestamp);
    int setNodeInfoGPS(NodeStruct * node, double Longitude, double Latitude, const char * timestamp);
    bool setTagSmoothingPos(int projectid, const char * TagId, double posX, double posY, double posZ);
	//bool setTagSectionGroup(int projectid, const char * TagId, SectionGroup * curSectionGroup);
	//bool setTagInWall(int projectid, const char * TagId, double posX, double posY, double posZ, AreaStruct * area);
    bool setTagInWall(int projectid, const char * TagId, double posX, double posY, double posZ, MapLayerAreaStruct * area);
	bool checkTagInWallTimeout(int projectid, const char * TagId, int timeout);

    bool setAnchorIndex(int projectid, const char * Anchor_macaddress, int Tag_idx, const char * version);
    bool setAnchorInfo(int projectid, const char * macaddress, const char * nodeid, int tafid, int buildingid, int buildingfloor, const char * nodename,
                        int idx, bool updatePos, double posX, double posY, double posZ, const char * version);
    bool setAnchorInfoS(int projectid, const char * AnchorId, double posX, double posY, double posZ);
    bool setAnchorInfoPANID(int projectid, const char * Anchor_macaddress, const char * PANID);
    bool setAnchorInfoMapLayer(int projectid, const char * Anchor_macaddress, int maplayerid);
    bool setCoordInfo(int projectid, const char * macaddress, const char * nodeid, int tafid, int buildingid, int buildingfloor, const char * nodename,
                        int idx, bool updatePos, double posX, double posY, double posZ, const char * version);
    bool setCoordInfoMapLayer(int projectid, const char * macaddress, int maplayerid);
    bool setLocatorIndex(int projectid, const char * macaddress, int index, const char * version);
    bool setLocatorInfoS(int projectid, const char * LocatorId, double posX, double posY, double posZ);
    bool setLocatorInfo(int projectid, const char * macaddress, const char * nodeid, int tafid, int buildingid, int buildingfloor, const char * nodename,
        int idx, bool updatePos, double posX, double posY, double posZ, const char * version);
    bool setLocatorInfoLoRA(int projectid, const char * macaddress, const char * loramacaddress);
    bool setLocatorInfoMacAddress(int projectid, const char * loramacaddress, const char * macaddress);
    bool setLocatorInfoMapLayer(int projectid, const char * macaddress, int maplayerid);
    bool setLocatorInfoMapLayerByLoRAmacaddress(int projectid, const char * loramacaddress, int maplayerid);
    bool setLocatorInfoGatewayID(int projectid, const char * loramacaddress, const char * GatewayID);
    bool setLocatorInfoGatewayRSSI(int projectid, const char * loramacaddress, const char * GatewayID, int rssi);
    bool setLocatorInfoBoot(int projectid, const char * loramacaddress, const char * PANID, const char * MODE, const char * VERSION);
    bool setLocatorInfoPowerOff(int projectid, const char * loramacaddress);
    bool setLocatorInfoPowerOn(int projectid, const char * loramacaddress);
    bool setLocatorInfoHeartBeat(int projectid, const char * loramacaddress, const char * HeartBeat);
    bool setLocatorInfoWeight(int projectid, const char * loramacaddress, const char * Weight);
    bool setLocatorInfoAreaType5(int projectid, const char * LocatorId, const char * areaid, const char * areaname);
    bool setLocatorInfoArea(int projectid, const char * LocatorId, const char * areaid, const char * areaname);
    bool setLocatorInfoSensorAlert(int projectid, const char * macaddress, int SensorAlert);

    bool addNodeInfo(std::list<NodeStruct*> * plist, const char * macaddress, const char * PANID,
                        const char * nodeid, int tafid, int buildingid, int buildingfloor, const char * nodename,
                        int idx, int type, bool updatePos, double posX, double posY, double posZ, const char * version);
    bool updateNodeInfoPos(NodeStruct * node, double posX, double posY, double posZ);

    bool setNodeInfo(std::list<NodeStruct*> * plist, const char * node_id, double posX, double posY, double posZ);
    bool setNodeInfoByID(int projectid, const char * node_type, const char * node_id, double posX, double posY, double posZ);
    bool setNodeAlertByType(int projectid, const char * macaddress, int node_type);
    bool clearNodeAlertByType(int projectid, const char * macaddress, int node_type);
    bool setTag0DAlert(int projectid, const char * macaddress, const char* loramacaddress);
    bool setLocator0DAlert(int projectid, const char * macaddress, const char* loramacaddress);
    int getTag0DAlertList(int projectid, const char * loramacaddress, std::list<std::string> * noideidlist);
    int getTagAlertListFromSameIP(int projectid, const char * IPAddress, std::list<std::string> * noideidlist);
    bool clearTag0DAlert(int projectid, const char * loramacaddress);
    bool clearTagAlertById(int projectid, const char * node_id);
    bool clearLocator0DAlert(int projectid, const char * loramacaddress);
    bool setNodeAlert(std::list<NodeStruct*> * plist, const char * macaddress);
    bool clearNodeAlert(std::list<NodeStruct*> * plist, const char * macaddress);
    bool deleteNodeByMAC(int projectid, const char * macaddress, const char * node_type);
    bool deleteNodeInfo(std::list<NodeStruct*> * plist, const char * macaddress);
    bool deleteAoANodeInfo(std::list<AoANodeStruct*> * plist, const char * macaddress);

    bool setCoordinatorAlive(int projectid, const char * Coord_macaddress);
    bool setCoordinatorAliveCount(int projectid, const char * Coord_macaddress, int count);
    bool setCoordinatorAliveAll(int projectid);
    bool setCoordinatorInfo(int projectid, const char * Coord_macaddress, const char * IP, const char * AppVersion, const char * TIME, const char * PANID);
    bool setCoordinatorBlacklist(int projectid, const char * Coord_macaddress, const char * list);
    bool setCoordinatorWhitelist(int projectid, const char * Coord_macaddress, const char * list);
    bool setCoordinatorjoinRangingRange(int projectid, const char * Coord_macaddress, int joinRangingRange);
    bool setCoordinatorjoinNotRangingRange(int projectid, const char * Coord_macaddress, int joinNotRangingRange);

    bool setLocatorAlive(int projectid, const char * locator_macaddress);
    bool setLocatorAliveCount(int projectid, const char * locator_macaddress, int count);
    bool setLocatorAliveByLoRAmacaddress(int projectid, const char * LoRA_macaddress);
    bool setLocatorResident(int projectid, const char * locator_macaddress, std::string serno);
    bool setLocatorEmployee(int projectid, const char * locator_macaddress, std::string empid);
    bool setLocatorButtonType(int projectid, const char * locator_macaddress, std::string buttontype);
    bool setLocatorUser(int projectid, const char * locator_macaddress, int userid);

    bool setAnchorAliveById(int projectid, const char * AnchorId);
    bool setAnchorAlive(int projectid, const char * Anchor_macaddress);
    bool setAnchorAliveCount(int projectid, const char * Anchor_macaddress, int count);
    bool setTagAliveById(int projectid, const char * TagId);
    bool setTagAlive(int projectid, const char * Tag_macaddress);
    bool setTagAlive(int projectid, const char * Tag_macaddress, const char * voltage, const char * temperature);
    bool setTagAliveCount(int projectid, const char * Tag_macaddress, int count);
    bool setTagVoltage(int projectid, const char * Tag_macaddress, const char * voltage);
    bool checkTagVoltageLow(std::list<NodeStruct*> * plist, std::string TagAlertVoltageLow);
    bool setTagResident(int projectid, const char * Tag_macaddress, std::string serno);
    bool setNodeResident(int projectid, NodeStruct * node, std::string serno);
    bool setTagEmployee(int projectid, const char * Tag_macaddress, std::string empid);
    bool setNodeEmployee(int projectid, NodeStruct * node, std::string empid);
    bool setTagUser(int projectid, const char * Tag_macaddress, int userid);
    bool clearTagUser(int projectid, int userid);
    bool setTagVelocity(int projectid, const char * Tag_macaddress, double velocity);
    bool setTagVelocityById(int projectid, const char * TagId, double velocity);
    bool setTagSleep(int projectid, const char * TagId, int sleep);

    bool checkAnchorValidNode(int projectid, const char * Anchor_macaddress, int index);
    bool checkTagValidNode(int projectid, const char * Anchor_macaddress, int index);
    bool checkCoordValidNode(int projectid, const char * Anchor_macaddress, int index);
    bool checkLocatorValidNode(int projectid, const char * Anchor_macaddress, int index);
    bool checkValidNode(std::list<NodeStruct*> * plist, const char * macaddress, int index);
    bool checkNodeExisted(int projectid, int node_type, const char * macaddress);
    bool checkNodeExisted(std::list<NodeStruct*> * plist, const char * macaddress);

    bool checkNFCLocatorMainGroup(int projectid, std::list<std::string> * maingroups);
    bool checkSensorDeviceMainGroup(int projectid, std::list<std::string> * maingroups);

    bool setNodeInfoIP(int projectid, int node_type, const char * macaddress, const char * IP);
    bool setNodeInfoIP(std::list<NodeStruct*> * plist, const char * macaddress, const char * IP);

    bool deleteGatewayByID(int projectid, std::string gatewayid);
    bool setLoRaGatewayAlive(int projectid, LoRaGatewayInfoStruct * gateway);
    bool setLoRaGatewayMQTTAlive(int projectid, std::string gatewayid);
    bool setLoRaGatewayIP(int projectid, std::string gatewayid, std::string gwip);
    bool setLoRaGatewayMapLayer(int projectid, std::string gatewayid, int maplayerid);
    bool setLoRaGatewayMaingroup(int projectid, std::string gatewayid, int maingroupid);
    LoRaGatewayInfoStruct * getLoRaGateway(int projectid, std::string gwid);
    int getLoRaGatewayMapLayer(int projectid, std::string gatewayid);

    int getAnchorIndex(int projectid, const char * macaddress);
	int getAnchorIndexFromId(int projectid, const char * anchorid);
    int getTagIndex(int projectid, const char * macaddress);
    int getCoordIndex(int projectid, const char * macaddress);
    int getLocatorIndex(int projectid, const char * macaddress);
    char * getTagId(int projectid, int Tag_idx);
    char * getTagId(int projectid, const char * macaddress);
    char * getTagIP(int projectid, const char * macaddress);
    std::string getTagempid(int projectid, const char * macaddress);
    int getTaguserid(int projectid, const char * macaddress);
    char * getTagName(int projectid, const char * tagid);
    char * getTagMacaddress(int projectid, const char * tagid);
    //int getTagFloor(int projectid, const char * tagid);
    NodeStruct * get_Tag_node(int projectid, const char * tagid);
    NodeStruct * get_Tag_node_ByMacaddress(int projectid, const char * macaddress);
    NodeStruct * get_Anchor_node(int projectid, const char * anchorid);
    NodeStruct * get_Anchor_node_ByMacaddress(int projectid, const char * Anchor_macaddress);
    NodeStruct * get_Cooridinator_node_ByMacaddress(int projectid, const char * macaddress);
    char * getAnchorId(int projectid, int Anchor_idx);
    char * getAnchorId(int projectid, const char * Anchor_macaddress);
    char * getAnchorName(int projectid, const char * anchorid);
    char * getLocatorId(int projectid, const char * locator_macaddress);
    char * getLocatorIdByLoRAmacaddress(int projectid, const char * LoRA_macaddress);
	char * getLocatorName(int projectid, const char * locatorid);
    int getLocatorButtonType(int projectid, const char * locatorid);

    char * getAoAId(int projectid, const char * macaddress);
    AoANodeStruct * get_AoA_node(int projectid, const char * macaddress);
    AoANodeStruct * get_AoA_nodeById(int projectid, const char * nodeid);
    bool setAoAInfo(int projectid, const char * macaddress, int posX, int posY, int posZ);
    bool setAoAInfoById(int projectid, const char * nodeid, int posX, int posY, int posZ);
    bool setAoAInfoS(int projectid, const char * macaddress, int pitch, int roll, int yaw);
    bool setAoAInfoSById(int projectid, const char * nodeid, int pitch, int roll, int yaw);
    bool setAoAInfoMacAddress(int projectid, const char * nodeid, const char * macaddress);
    bool setAoAInfoGridSize(int projectid, const char * nodeid, int AoAGridSize);

    bool setNodeInfoMapLayer(int projectid, int node_type, const char * macaddress, int maplayer);

    int getTagMapLayerById(int projectid, const char * nodeid);
    // int getAnchorFloor(int projectid, const char * Anchor_macaddress);
    // int getAnchorFloorById(int projectid, const char * AnchorId);
    int getAnchorMapLayer(int projectid, const char * Anchor_macaddress);
    int getAnchorMapLayerById(int projectid, const char * AnchorId);
    //int getLocatorFloor(int projectid, const char * locator_macaddress);
    //int getLocatorFloorByLoRAmacaddress(int projectid, const char * LoRA_macaddress);
    int getLocatorMapLayer(int projectid, const char * locator_macaddress);
    int getLocatorMapLayerByLoRAmacaddress(int projectid, const char * LoRA_macaddress);
    int getLocatorHeartBeat(int projectid, const char * LoRA_macaddress);
    int getLocatorWeight(int projectid, const char * LoRA_macaddress);
    char * getLocatorMacaddress(int projectid, const char * locatorid);
    NodeStruct * get_Locator_node(int projectid, const char * LoRA_macaddress);
    NodeStruct * get_Locator_node_byid(int projectid, const char * locatorid);
    NodeStruct * get_Locator_node_byUserid(int projectid, int userid);
    NodeStruct * get_Locator_node_ByMacaddress(int projectid, const char * macaddress);
    int getNodeIndex(std::list<NodeStruct*> * plist, const char * macaddress);
    bool getAnchorCoord(int projectid, const char * anchorid, double * AnchorCoord);
    bool getLocatorCoord(int projectid, const char * locatorid, double * LocatorCoord);
    bool getCoordinatorCoord(int projectid, const char * coordid, double * CoordCoord);
    bool getTagCoord(int projectid, const char * TagId, double * TagCoord);
    bool getTagSmoothingCoord(int projectid, const char * TagId, double * TagCoord);
    int getTagPrePos(int projectid, const char * TagId, double * TagCoord);
    int getTagMomentum(int projectid, const char * TagId, double * momentum);
	time_t getTagDiffTimestamp(int projectid, const char * TagId);
    time_t getTagAlertDiffTimestamp(int projectid, const char * macaddress);
    time_t getLocatorAlertDiffTimestamp(int projectid, const char * macaddress);
    time_t getLocatorAlertDiffTimestampByLoRAmacaddress(int projectid, const char * LoRAMacaddress);
    void updateLocatorAlertIssuTime(int projectid, const char * macaddress);
    char * getTagAreaId(int projectid, const char * TagId);
    std::string getempidFromTagByAliveLoRAranging(int projectid, const char * LoRA_macaddress);

    int getAnchorProjectId(const char * Anchor_macaddress);
    int getTagProjectId(const char * Tag_macaddress);
    int getCoordinatorProjectId(const char * Coord_macaddress);
    int getLocatorProjectId(const char * Locator_macaddress);
    int getLoRaGatewayProjectId(std::string gwid);
    int getNodeProjectId(std::list<NodeStruct*> * plist, const char * macaddress);
    int getLocatorProjectIdByLoRAmacaddress(const char * Locator_macaddress);

    char * getCoordinatorIP(int projectid, const char * macaddress);
    char * getCoordinatorIdFromIP(int projectid, const char * IPAddress);
    int getCoordinatorMapLayer(int projectid, const char * macaddress);
    int getCoordinatorMapLayerFromIP(int projectid, const char * IPAddress);
    int getCoordinatorjoinRangingRange(int projectid, const char * macaddress);
    int getCoordinatorjoinNotRangingRange(int projectid, const char * macaddress);

    RFIDDeviceStruct * get_RFIDDevice(int projectid, const char * macaddress);
    RFIDDeviceStruct * get_RFIDDeviceByLoRAmacaddress(int projectid, const char * LoRAMacaddress);
    NFCDeviceStruct * get_NFCDevice(int projectid, const char * macaddress);
    NFCDeviceStruct * get_NFCDeviceByLoRAmacaddress(int projectid, const char * LoRAMacaddress);
    EDGEDeviceStruct * get_EDGEDevice(int projectid, const char * macaddress);
    SensorDeviceStruct * get_SensorDevice(int projectid, std::string id);
    bool setRFIDDeviceAlive(int projectid, RFIDDeviceStruct * device);
    int setRFIDDeviceAlive(int projectid, const char * Locator_macaddress);
    bool setNFCDeviceAlive(int projectid, NFCDeviceStruct * device);
    int setNFCDeviceAlive(int projectid, const char * Locator_macaddress);
    bool setEDGEDeviceAlive(int projectid, EDGEDeviceStruct * device);
    bool setSensorDeviceAlive(int projectid, SensorDeviceStruct * device);
    SensorDeviceStruct * setSensorDeviceLocation(int projectid, std::string deviceid, std::string Latitude, std::string Longitude);
    bool setSensorDeviceValue(int projectid, std::string deviceid, std::string value, std::string datetime);

    RFIDCardStruct * get_RFIDCard(int projectid, const char * cardid);
    NFCCardStruct * get_NFCCard(int projectid, const char * cardid);
    bool setRFIDCardAlive(int projectid, RFIDCardStruct * card);
    bool setNFCCardAlive(int projectid, NFCCardStruct * card);
    bool setRFIDCardUser(int projectid, std::string cardid, int userid);
    bool setNFCCardUser(int projectid, std::string cardid, int userid);

    std::list<NodeStruct*> * get_Anchor_list(int projectid);
    std::list<NodeStruct*> * get_Tag_list(int projectid);
    std::list<NodeStruct*> * get_Coordinator_list(int projectid);
    std::list<NodeStruct*> * get_Locator_list(int projectid);
    std::list<LoRaGatewayInfoStruct*> * get_LoRaGateway_list(int projectid);
    std::list<AoANodeStruct*> * get_AoA_list(int projectid);
    std::list<RFIDDeviceStruct*> * get_RFIDDevice_list(int projectid);
    std::list<NFCDeviceStruct*> * get_NFCDevice_list(int projectid);
    std::list<RFIDCardStruct*> * get_RFIDCard_list(int projectid);
    std::list<NFCCardStruct*> * get_NFCCard_list(int projectid);
    std::list<NFCCardStruct*> * get_MaingroupNFCCard_list(int projectid);
    std::list<LocatorRSSIInfoStruct*> * get_LocatorRSSIInfo_list(int projectid);
    std::list<EDGEDeviceStruct*> * get_EDGEDevice_list(int projectid);
    std::list<SensorDeviceStruct*> * get_SensorDevice_list(int projectid);
    std::list<UserStruct*> * get_User_list(int projectid);

    //void dump_tag_area_inout_action();
    //bool getAreaTagLastAction(int projectid, std::string AreaId, std::string TagId, AreaInOutStruct & data);

    int getSameIPTagCount(int projectid, const char * IP);

    int get_POSNode_list(std::list<NodeItemStruct *> * plist, int projectid);
    std::list<PositionInfoStruct *> * get_POSNodeData_list(int projectid, std::string nodeid);
    int get_GPSNode_list(std::list<NodeItemStruct *> * plist, int projectid);
    std::list<GPSInfoStruct *> * get_GPSNodeData_list(int projectid, std::string nodeid);
    GPSInfoStruct * getTagInfoGPS(int projectid, const char * Tag_macaddress);

    bool checkValidUser(int projectid, const char * account, const char * password);
    bool checkValidUser(int projectid, const char * account, const char * password, const char * userid);
    // bool checkValidUser(int projectid, const char * account, const char * password, UserStruct * UserData);
    bool checkValidUser(int projectid, const char * account, const char * password, const char * userid, UserStruct * UserData);
    int checkValidAccountLogin(int projectid, std::string account, std::string key);
    std::string queryLoginKey(int projectid, std::string account, std::string userid, std::string token);
    // std::string queryLoginKey(int projectid, std::string account, std::string token);

    // bool update_user_password(int projectid, const char * account, const char * password);
    bool update_user_password(int projectid, std::string accountid, int userid, std::string password);
    UserEmployeeStruct * getEmployee(int projectid, const char * empid);

    // UserStruct * getUser(int projectid, std::string account);
    UserStruct * getUser(int projectid, int userid);
    bool setUserInfoPos(int projectid, int userid, int posX, int posY, int posZ);
    bool setUserInfoArea(int projectid, int userid, std::string areaid, std::string areaname);
    bool setUserInfoArea0D(int projectid, int userid, std::string areaid, int AreaType0D);
    bool setUserInfoAreaType5(int projectid, int userid, std::string areaid, std::string areaname);
    bool setUserRollCallNotInArea(int projectid, int userid, std::string areaid);
    bool setUserInfoMaplayer(int projectid, int userid, int maplayerid);
    bool setUserRangingResultType(int projectid, int userid, int RangingResultType);

    TargetTopicStruct * getUserTopic(int projectid, int userid, std::string accountid);
    std::string getUserName(int projectid, int userid);
    std::list<EmployeeEventReportStruct*> * getEmployeeEventReport(int projectid, std::string empid, std::string report_start_date, std::string report_end_date);

    void reload_sys_config();
    void reload_RFIDDevice();
    void reload_NFCDevice();
    void reload_EDGEDevice();
    void reload_RFIDCard();
    void reload_NFCCard();
    void reload_SensorDevice();
    void reload_User();
    void reload_LoRaGateway();

    bool add_Anchor_node(int projectid, NodeStruct * node);
    bool add_Tag_node(int projectid, NodeStruct * node);
    bool add_Cooridinator_node(int projectid, NodeStruct * node);
    bool add_Locator_node(int projectid, NodeStruct * node);
    bool add_LoRaGateway_node(int projectid, LoRaGatewayInfoStruct * node);

    bool updateAoAList(int projectid, const char * LoRA_macaddress);
    bool update_Node_maingroup(const char * macaddress, int maingroupid);
    bool update_Node_maingroup(std::list<NodeStruct*> * nodelist, const char * macaddress, int maingroupid);
    bool update_Node_maingroup(std::list<AoANodeStruct*> * nodelist, const char * macaddress, int maingroupid);


    bool addCardScanRecord(int projectid, std::string cardid, std::string LoRA_macaddress);
    // std::list<CARDScanRecordStruct*> * get_CardScan_list(int projectid);

private:

	std::vector<DeviceInfoListStruct*> mDeviceInfoList;

	DBManager * mDbManager;

    bool loadDeviceNodeFromDB();
    int readDeviceInfoFromDB(int projectid, DeviceInfoListStruct * deviceInfo);
    int check_all_node_mapleyer(int maplayer, DeviceInfoListStruct * deviceInfo);
    int check_node_mapleyer(int maplayer, std::list<NodeStruct*> * plist);
    int check_UserSubGroupStruct(MySQLDBAccess * baseDb, std::list<UserStruct*> * userList);
    int check_UserMainGroupStruct(MySQLDBAccess * baseDb, std::list<UserStruct*> * userList);
    int check_UserGroupID(MySQLDBAccess * baseDb, std::list<UserStruct*> * userList);

    int check_all_node_verifykey(DeviceInfoListStruct * deviceInfo);
    int check_node_verifykey(int projectid, std::list<NodeStruct*> * plist);

    int initialTagSmoothingPos();
	DeviceInfoListStruct * getDeviceInfoList(int projectid);

    int getEmptyIndex(std::list<NodeStruct*> * plist);

    bool getNodeCoord(std::list<NodeStruct*> * plist, const char * nodeid, double * coord);

    int debug_printNodeList(const char * name, std::list<NodeStruct*> * plist);
    // int ConvertGPSToPos(NodeStruct * node);
    int getEmptyNodeId(std::list<NodeStruct*> * plist, const char * Prefix, const char * macaddress, char * outnodeid);

    int create_NodePOSInfo(NodeStruct * node, double posX, double posY, double posZ);
    int create_NodeGPSInfo(int type, NodeStruct * node, double Longitude, double Latitude, const char * timestamp);
};
#endif
