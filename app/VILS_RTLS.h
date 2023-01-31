

#ifndef VILS_RTLS_H
#define VILS_RTLS_H

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>
#include <fstream>

#include "config.h"
#include "common.h"
#include "testdata.h"
#include "network/DataItem.h"
#include "mysql/MySQLDBAccess.h"

#define STATE_CAL_ANCHOR_COORD	0
#define STATE_CAL_TAG_COORD		1
#define STATE_CAL_VILS_MPU9250	2

//
// Functions
//
void runloop();
void wait_loop();

int wait_system_finished();
int processData(DataItem * item);
int CheckResidentMoveOut(DataItem * item);
int CheckEmployeeMoveOut(DataItem * item);
int sendATcmd(DataItem * item);

void update_service_status();
void update_service_WebAPI();
void check_coordinator_alive();
void check_queue_status();

void clear_anchor_alive();
void clear_tag_alive();
void clear_coord_alive();
void clear_locator_alive();
void clear_node_alive(std::list<NodeStruct*> * nodeList, int nodetype);
void clear_LoRaGateway_alive();
void clear_NFCDevice_alive();
void clear_RFIDDevice_alive();
void clear_EDGEDevice_alive();
void clear_NFCCard_alive();
void clear_MaingroupNFCCard_alive();
void clear_RFIDCard_alive();
void clear_unknown_NFCCard_alive();
void clear_unknown_RFIDCard_alive();
void clear_SensorDevice_alive();

void clear_tag_BodyInfo();
void clear_locator_BodyInfo();
void clear_node_BodyInfo(std::list<NodeStruct*> * nodeList);

void updateERPServerInfo();

int check_alert_timeout();
void update_ping_status();
//void update_node_ProjectId();
//void resolve_nodelist_projectid(std::list<NodeStruct*> * nodeList, std::list<VILSConfig*> * allConfigList);
//int resolveProjectId(std::list<VILSConfig*> * allConfigList, int floor);

bool dump_device_status_to_MySQL();
bool dump_tag_status_to_MySQL();
bool dump_anchor_status_to_MySQL();
bool dump_coord_status_to_MySQL();
bool dump_locator_status_to_MySQL();
bool dump_NFC_RFID_status_to_MySQL();
bool dump_LoRaGateway_status_to_MySQL();
void save_nodelist_info(int projectid, std::list<NodeStruct*> * nodeList);
void save_aoanodelist_info(int projectid, std::list<AoANodeStruct*> * nodeList);
// void dump_tag_area_inout_action();
void dump_queue_status();
void dump_memory_usage();
void printTagInfo();

void send_all_BlackList_info();
void send_all_WhiteList_info();
void send_all_Validrang_info();
void send_BlackList_info(std::list<NodeStruct*> * nodeList);
void send_WhiteList_info(std::list<NodeStruct*> * nodeList);
void send_Validrang_info(std::list<NodeStruct*> * nodeList);
void sendSystemTimeToDevice();
bool send_MQTT_cmd_todevice(int projectid, std::string cmd, NFCDeviceStruct * device);
bool send_MQTT_cmd_wentaitek(std::string payload);
void clear_unknown_NFCCard_alive();
void clear_unknown_RFIDCard_alive();
std::string prepareLocatoRSSIInfoString(LocatorRSSIInfoStruct * rssiInfo);
static void s_signal_handler (int signal_value);
static void s_catch_signals (void);

// int demo_XLSX();

#endif // VILS_RTLS_H
