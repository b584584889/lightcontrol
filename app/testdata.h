
/*
 *  testdata.h
 *
 *  Created on: 2018/10/19
 *      Author: ikki
 */

#ifndef TESTDATA_H
#define TESTDATA_H

#include <stdio.h>

#include "mysql/MySQLDBAccess.h"

void save_iHome_test_node_to_MySQL();
void save_iHome_base_node_to_MySQL();
void save_TAF_base_node_to_MySQL();
void save_test_node_to_MySQL();
void save_KAOOFFICE_node_to_MySQL();
void save_AUO_node_to_MySQL();
//void update_test_node_to_MySQL();
void save_node_to_MySQL(MySQLDBAccess * mysqldb, int index, int nodetype,
        const char * nodeid, const char *macaddress, const char *LoRA_macaddress,
        double posX, double posY, double posZ, const char * version,
        int tafid, int buildingid, int buildingfloor);
void save_node_to_MySQL(MySQLDBAccess * mysqldb, int index, int nodetype,
        const char * nodeid, const char *macaddress,
        double posX, double posY, double posZ, const char * version,
        int tafid, int buildingid, int buildingfloor);
void save_nodelist_info_MySQL(std::list<NodeStruct*> * nodeList, MySQLDBAccess * mysqldb);
void save_node_info_MySQL(NodeStruct * node, MySQLDBAccess * mysqldb);

#endif // TESTDATA_H
