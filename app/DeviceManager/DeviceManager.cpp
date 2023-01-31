/*
 * DeviceManager.cpp
 *
 *  Created on: 2017/02/20
 *      Author: ikki
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <boost/algorithm/string.hpp>

#include <sys/time.h>
#include <time.h>

#include "string.h"
#include "DeviceManager.h"
#include "../json/json.h"

using namespace std ;


DeviceManager::DeviceManager(DBManager * dbManager)
    : mDbManager(dbManager)
{
    if ( !loadDeviceNodeFromDB() )
    {
        vilslog::printf ("DeviceManager::loadDeviceNodeFromDB()  failed!\n");
    }

    initialTagSmoothingPos();
}

DeviceManager::~DeviceManager()
{
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        clear_node_list(mDeviceInfoList[i]->mAnchorNodeList);
        clear_node_list(mDeviceInfoList[i]->mTagNodeList);
        clear_node_list(mDeviceInfoList[i]->mCoordNodeList);
        clear_node_list(mDeviceInfoList[i]->mLocatorNodeList);
        clear_LoRaGateway_list(mDeviceInfoList[i]->mLoRaGatewayList);
        clear_AoANodeStruct_list(mDeviceInfoList[i]->mAoANodeList);
        clear_NFCDeviceStruct_list(mDeviceInfoList[i]->mNFCDeviceList);
        clear_NFCCardStruct_list(mDeviceInfoList[i]->mNFCCardList);
        clear_NFCCardStruct_list(mDeviceInfoList[i]->mMaingroupNFCCardList);
        clear_RFIDDeviceStruct_list(mDeviceInfoList[i]->mRFIDDeviceList);
        clear_RFIDCardStruct_list(mDeviceInfoList[i]->mRFIDCardList);
        clear_EDGEDeviceStruct_list(mDeviceInfoList[i]->mEDGEDeviceList);
        clear_LocatorRSSIInfoStruct_list(mDeviceInfoList[i]->mLocatorRSSIInfoList);
        mDeviceInfoList[i]->mSensorDeviceList->clear();
        clear_UserStruct_list(mDeviceInfoList[i]->mUserList);

        delete mDeviceInfoList[i]->mAnchorNodeList;
        delete mDeviceInfoList[i]->mTagNodeList;
        delete mDeviceInfoList[i]->mCoordNodeList;
        delete mDeviceInfoList[i]->mLocatorNodeList;
        delete mDeviceInfoList[i]->mLoRaGatewayList;
        delete mDeviceInfoList[i]->mSysConfig;
        delete mDeviceInfoList[i]->mAoANodeList;
        delete mDeviceInfoList[i]->mNFCDeviceList;
        delete mDeviceInfoList[i]->mNFCCardList;
        delete mDeviceInfoList[i]->mMaingroupNFCCardList;
        delete mDeviceInfoList[i]->mRFIDDeviceList;
        delete mDeviceInfoList[i]->mRFIDCardList;
        delete mDeviceInfoList[i]->mEDGEDeviceList;
        delete mDeviceInfoList[i]->mLocatorRSSIInfoList;
        delete mDeviceInfoList[i]->mSensorDeviceList;
        delete mDeviceInfoList[i]->mUserList;
    }
}

bool DeviceManager::loadDeviceNodeFromDB()
{
    std::list<int> * prjidList = mDbManager->getAllProjectConfig();
    for (std::list<int>::iterator prj = prjidList->begin(); prj != prjidList->end(); prj++)
    {
        int projectid = (*prj);

        bool bFound = false;
        for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
        {
            if (mDeviceInfoList[i]->projectid == projectid)
            {
                readDeviceInfoFromDB(projectid, mDeviceInfoList[i]);
                bFound = true;
                break;
            }
        }

        if (!bFound)
        {
            DeviceInfoListStruct * newInfo = new DeviceInfoListStruct();
            newInfo->projectid = projectid;
            newInfo->mSysConfig = NULL;
            readDeviceInfoFromDB(projectid, newInfo);

            mDeviceInfoList.push_back(newInfo);

        }
    }

    prjidList->clear();
    delete prjidList;

    return true;
}

int DeviceManager::readDeviceInfoFromDB(int projectid, DeviceInfoListStruct * deviceInfo)
{
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        deviceInfo->mAnchorNodeList = baseDb->read_nodes(NODE_TYPE_ANCHOR);
        deviceInfo->mTagNodeList = baseDb->read_nodes(NODE_TYPE_TAG);
        deviceInfo->mCoordNodeList = baseDb->read_nodes(NODE_TYPE_COORD);
        deviceInfo->mLocatorNodeList = baseDb->read_nodes(NODE_TYPE_LOCATOR);
        deviceInfo->mLoRaGatewayList = baseDb->read_LoRaGateway();
        deviceInfo->mAoANodeList = baseDb->read_AoA_nodes();

        deviceInfo->mNFCDeviceList = baseDb->get_NFCDevices();
        deviceInfo->mNFCCardList = baseDb->get_NFCCards();
        deviceInfo->mMaingroupNFCCardList = baseDb->get_MaingroupNFCCards();
        deviceInfo->mRFIDDeviceList = baseDb->get_RFIDDevices();
        deviceInfo->mRFIDCardList = baseDb->get_RFIDCards();
        // deviceInfo->mCARDScanList = new std::list<CARDScanRecordStruct*>();
        deviceInfo->mEDGEDeviceList = baseDb->get_EDGEDevices();
        deviceInfo->mSensorDeviceList = baseDb->get_SensorDevices();
        deviceInfo->mUserList = baseDb->get_Users();

        //
        // Locator RSSI record
        //
        deviceInfo->mLocatorRSSIInfoList = new std::list<LocatorRSSIInfoStruct*>();
        for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
        {
            LocatorRSSIInfoStruct * newRSSIInfo = new LocatorRSSIInfoStruct();
            newRSSIInfo->LoRAMacaddress = (*existed)->LoRAMacaddress;
            newRSSIInfo->gatewayRecords = new std::list<GatewayRSSIInfoStruct*>();

            deviceInfo->mLocatorRSSIInfoList->push_back(newRSSIInfo);
        }

        if ( deviceInfo->mSysConfig == NULL)
        {
            deviceInfo->mSysConfig = new VILSSystemConfig();
        }
        baseDb->read_sys_config(deviceInfo->mSysConfig);

        int layerid = baseDb->get_first_maplayerid();
        check_all_node_mapleyer(layerid, deviceInfo);

        check_all_node_verifykey(deviceInfo);

        checkTagVoltageLow(deviceInfo->mTagNodeList, deviceInfo->mSysConfig->TagAlertVoltageLow);

        // check_UserSubGroupStruct(baseDb, deviceInfo->mUserList);
        //(baseDb, deviceInfo->mUserList);
        check_UserGroupID(baseDb, deviceInfo->mUserList);

        vilslog::printf("DeviceManager::readDeviceInfoFromDB() [%d] mNFCDeviceList->size[%d]\n", projectid, deviceInfo->mNFCDeviceList->size());
        vilslog::printf("DeviceManager::readDeviceInfoFromDB() [%d] mNFCCardList->size[%d]\n", projectid, deviceInfo->mNFCCardList->size());
        vilslog::printf("DeviceManager::readDeviceInfoFromDB() [%d] mMaingroupNFCCardList->size[%d]\n", projectid, deviceInfo->mMaingroupNFCCardList->size());
        vilslog::printf("DeviceManager::readDeviceInfoFromDB() [%d] mRFIDDeviceList->size[%d]\n", projectid, deviceInfo->mRFIDDeviceList->size());
        vilslog::printf("DeviceManager::readDeviceInfoFromDB() [%d] mRFIDCardList->size[%d]\n", projectid, deviceInfo->mRFIDCardList->size());
        vilslog::printf("DeviceManager::readDeviceInfoFromDB() [%d] mLocatorRSSIInfoList->size[%d]\n", projectid, deviceInfo->mLocatorRSSIInfoList->size());
        vilslog::printf("DeviceManager::readDeviceInfoFromDB() [%d] mEDGEDeviceList->size[%d]\n", projectid, deviceInfo->mEDGEDeviceList->size());
        vilslog::printf("DeviceManager::readDeviceInfoFromDB() [%d] mSensorDeviceList->size[%d]\n", projectid, deviceInfo->mSensorDeviceList->size());
        vilslog::printf("DeviceManager::readDeviceInfoFromDB() [%d] mUserList->size[%d]\n", projectid, deviceInfo->mUserList->size());

    }

    return 0;
}

int DeviceManager::check_all_node_mapleyer(int maplayer, DeviceInfoListStruct * deviceInfo)
{
    check_node_mapleyer(maplayer, deviceInfo->mAnchorNodeList);
    check_node_mapleyer(maplayer, deviceInfo->mTagNodeList);
    check_node_mapleyer(maplayer, deviceInfo->mCoordNodeList);
    check_node_mapleyer(maplayer, deviceInfo->mLocatorNodeList);

    for (std::list<LoRaGatewayInfoStruct *>::iterator gateway = deviceInfo->mLoRaGatewayList->begin(); gateway != deviceInfo->mLoRaGatewayList->end(); gateway++)
    {
        if ((*gateway)->maplayer == 0 )
        {
            (*gateway)->maplayer = maplayer;
        }
    }

    for (std::list<AoANodeStruct *>::iterator aoanode = deviceInfo->mAoANodeList->begin(); aoanode != deviceInfo->mAoANodeList->end(); aoanode++)
    {
        if ((*aoanode)->maplayer == 0 )
        {
            (*aoanode)->maplayer = maplayer;
        }
    }

    return maplayer;
}

int DeviceManager::check_node_mapleyer(int maplayer, std::list<NodeStruct*> * plist)
{
    for (std::list<NodeStruct*>::iterator existed = plist->begin(); existed != plist->end(); existed++)
    {
        if ((*existed)->maplayer == 0 )
        {
            (*existed)->maplayer = maplayer;
        }
    }
    return maplayer;
}

int DeviceManager::check_all_node_verifykey(DeviceInfoListStruct * deviceInfo)
{
    check_node_verifykey(deviceInfo->projectid, deviceInfo->mAnchorNodeList);
    check_node_verifykey(deviceInfo->projectid, deviceInfo->mTagNodeList);
    check_node_verifykey(deviceInfo->projectid, deviceInfo->mCoordNodeList);
    check_node_verifykey(deviceInfo->projectid, deviceInfo->mLocatorNodeList);

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(deviceInfo->projectid);

    for (std::list<LoRaGatewayInfoStruct *>::iterator gateway = deviceInfo->mLoRaGatewayList->begin(); gateway != deviceInfo->mLoRaGatewayList->end(); gateway++)
    {
        if ((*gateway)->verifykey.length() == 0 )
        {
            if (baseDb != NULL)
            {
                (*gateway)->verifykey = baseDb->create_verifykey("LoRaGatewayInfo");

                vilslog::printf("DeviceManager::check_all_node_verifykey() LoRaGatewayInfo [%d] gwid[%s] verifykey[%s]\n",
                    deviceInfo->projectid, (*gateway)->gwid.c_str(), (*gateway)->verifykey.c_str());

                baseDb->write_LoRaGatewayInfo((*gateway));
            }
        }
    }

    for (std::list<AoANodeStruct *>::iterator aoanode = deviceInfo->mAoANodeList->begin(); aoanode != deviceInfo->mAoANodeList->end(); aoanode++)
    {
        if ((*aoanode)->verifykey.length() == 0 )
        {
            if (baseDb != NULL)
            {
                (*aoanode)->verifykey = baseDb->create_verifykey("AoANodeInfo");

                vilslog::printf("DeviceManager::check_all_node_verifykey() AoANodeInfo [%d] nodeid[%s] verifykey[%s]\n",
                    deviceInfo->projectid, (*aoanode)->nodeid, (*aoanode)->verifykey.c_str());

                baseDb->write_AOA_node_info((*aoanode));
            }
        }
    }

    for (std::list<NFCDeviceStruct*>::iterator nfcdevice = deviceInfo->mNFCDeviceList->begin(); nfcdevice != deviceInfo->mNFCDeviceList->end(); nfcdevice++)
    {
        if ((*nfcdevice)->verifykey.length() == 0 )
        {
            if (baseDb != NULL)
            {
                (*nfcdevice)->verifykey = baseDb->create_verifykey("NFCDevice");

                vilslog::printf("DeviceManager::check_all_node_verifykey() NFCDevice [%d] deviceid[%d] verifykey[%s]\n",
                    deviceInfo->projectid, (*nfcdevice)->deviceid, (*nfcdevice)->verifykey.c_str());

                baseDb->update_NFCDevice((*nfcdevice));
            }
        }
    }

    for (std::list<RFIDDeviceStruct*>::iterator rfiddevice = deviceInfo->mRFIDDeviceList->begin(); rfiddevice != deviceInfo->mRFIDDeviceList->end(); rfiddevice++)
    {
        if ((*rfiddevice)->verifykey.length() == 0 )
        {
            if (baseDb != NULL)
            {
                (*rfiddevice)->verifykey = baseDb->create_verifykey("RFIDDevice");

                vilslog::printf("DeviceManager::check_all_node_verifykey() RFIDDevice [%d] deviceid[%d] verifykey[%s]\n",
                    deviceInfo->projectid, (*rfiddevice)->deviceid, (*rfiddevice)->verifykey.c_str());

                baseDb->update_RFIDDevice((*rfiddevice));
            }
        }
    }

    for (std::list<EDGEDeviceStruct*>::iterator edgedevice = deviceInfo->mEDGEDeviceList->begin(); edgedevice != deviceInfo->mEDGEDeviceList->end(); edgedevice++)
    {
        if ((*edgedevice)->verifykey.length() == 0 )
        {
            if (baseDb != NULL)
            {
                (*edgedevice)->verifykey = baseDb->create_verifykey("EDGEDevice");

                vilslog::printf("DeviceManager::check_all_node_verifykey() EDGEDevice [%d] deviceid[%d] verifykey[%s]\n",
                    deviceInfo->projectid, (*edgedevice)->deviceid, (*edgedevice)->verifykey.c_str());

                baseDb->update_EDGEDevice((*edgedevice));
            }
        }
    }

    for (std::list<SensorDeviceStruct*>::iterator sensor = deviceInfo->mSensorDeviceList->begin(); sensor != deviceInfo->mSensorDeviceList->end(); sensor++)
    {
        if ((*sensor)->verifykey.length() == 0 )
        {
            if (baseDb != NULL)
            {
                (*sensor)->verifykey = baseDb->create_verifykey("sensordevice");

                vilslog::printf("DeviceManager::check_all_node_verifykey() sensordevice [%d] id[%s] verifykey[%s]\n",
                    deviceInfo->projectid, (*sensor)->id.c_str(), (*sensor)->verifykey.c_str());

                baseDb->update_SensorDevice((*sensor));
            }
        }
    }

    return 0;
}

int DeviceManager::check_node_verifykey(int projectid, std::list<NodeStruct*> * plist)
{
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);

    for (std::list<NodeStruct*>::iterator existed = plist->begin(); existed != plist->end(); existed++)
    {
        if ((*existed)->verifykey.length() == 0 )
        {
            if (baseDb != NULL)
            {
                (*existed)->verifykey = baseDb->create_verifykey("NodeInfo");

                vilslog::printf("DeviceManager::check_node_verifykey() [%d] nodetype[%d] nodename[%s] verifykey[%s]\n",
                    projectid, (*existed)->nodetype, (*existed)->nodename, (*existed)->verifykey.c_str());

                baseDb->write_node_info((*existed));
            }
        }
    }
    return 0;
}

int DeviceManager::check_UserSubGroupStruct(MySQLDBAccess * baseDb, std::list<UserStruct*> * userList)
{
    std::list<UserSubGroupStruct*> * userSubGroupList = baseDb->get_UserSubGroups();

    for (std::list<UserSubGroupStruct *>::iterator userSubGroup = userSubGroupList->begin(); userSubGroup != userSubGroupList->end(); userSubGroup++)
    {
        int accountidList_size = (int)(*userSubGroup)->accountidList->size();
        int useridList_size = (int)(*userSubGroup)->useridList->size();
        if ( accountidList_size > useridList_size )
        {
            vilslog::printf("DeviceManager::check_UserSubGroupStruct() accountidList_size[%d] useridList_size[%d] start...\n", accountidList_size, useridList_size);

            (*userSubGroup)->useridList->clear();

            for (std::list<std::string>::iterator accountid = (*userSubGroup)->accountidList->begin(); accountid != (*userSubGroup)->accountidList->end(); accountid++)
            {
                // find this user
                for (std::list<UserStruct*>::iterator user = userList->begin(); user != userList->end(); user++)
                {
                    //if ( (*user)->account.compare((*accountid)) == 0 )
                    if( boost::iequals((*user)->account, (*accountid)) )
                    {
                        vilslog::printf("DeviceManager::check_UserSubGroupStruct() found accountid[%s] -> userid[%d]\n", (*accountid).c_str(), (*user)->userid);
                        (*userSubGroup)->useridList->push_back(to_string((*user)->userid));
                        break;
                    }
                }
            }

            accountidList_size = (int)(*userSubGroup)->accountidList->size();
            useridList_size = (int)(*userSubGroup)->useridList->size();
            vilslog::printf("DeviceManager::check_UserSubGroupStruct() accountidList_size[%d] useridList_size[%d] finished.\n", accountidList_size, useridList_size);

            baseDb->update_UserSubGroup((*userSubGroup));
        }
    }

    clear_UserSubGroupStruct_list(userSubGroupList);
    delete userSubGroupList;

    return 0;
}

int DeviceManager::check_UserMainGroupStruct(MySQLDBAccess * baseDb, std::list<UserStruct*> * userList)
{
    std::list<UserMainGroupStruct*> * userMainGroupList = baseDb->get_UserMainGroups();

    for (std::list<UserMainGroupStruct *>::iterator userMainGroup = userMainGroupList->begin(); userMainGroup != userMainGroupList->end(); userMainGroup++)
    {
        int accountidList_size = (int)(*userMainGroup)->accountidList->size();
        int useridList_size = (int)(*userMainGroup)->useridList->size();
        if ( accountidList_size > useridList_size )
        {
            vilslog::printf("DeviceManager::check_UserMainGroupStruct() accountidList_size[%d] useridList_size[%d] start...\n", accountidList_size, useridList_size);

            (*userMainGroup)->useridList->clear();

            for (std::list<std::string>::iterator accountid = (*userMainGroup)->accountidList->begin(); accountid != (*userMainGroup)->accountidList->end(); accountid++)
            {
                // find this user
                for (std::list<UserStruct*>::iterator user = userList->begin(); user != userList->end(); user++)
                {
                    //if ( (*user)->account.compare((*accountid)) == 0 )
                    if( boost::iequals((*user)->account, (*accountid)) )
                    {
                        vilslog::printf("DeviceManager::check_UserMainGroupStruct() found accountid[%s] -> userid[%d]\n", (*accountid).c_str(), (*user)->userid);
                        (*userMainGroup)->useridList->push_back(to_string((*user)->userid));
                        break;
                    }
                }
            }

            accountidList_size = (int)(*userMainGroup)->accountidList->size();
            useridList_size = (int)(*userMainGroup)->useridList->size();
            vilslog::printf("DeviceManager::check_UserMainGroupStruct() accountidList_size[%d] useridList_size[%d] finished.\n", accountidList_size, useridList_size);

            baseDb->update_UserMainGroup((*userMainGroup));
        }
    }

    clear_UserMainGroupStruct_list(userMainGroupList);
    delete userMainGroupList;

    return 0;
}

int DeviceManager::check_UserGroupID(MySQLDBAccess * baseDb, std::list<UserStruct*> * userList)
{
    std::list<UserMainGroupStruct*> * userMainGroupList = baseDb->get_UserMainGroups();

    for (std::list<UserMainGroupStruct *>::iterator userMainGroup = userMainGroupList->begin(); userMainGroup != userMainGroupList->end(); userMainGroup++)
    {
        for (std::list<std::string>::iterator userid = (*userMainGroup)->useridList->begin(); userid != (*userMainGroup)->useridList->end(); userid++)
        {
            int int_userid = convert_string_to_int(*userid);

            // find this user
            for (std::list<UserStruct*>::iterator user = userList->begin(); user != userList->end(); user++)
            {
                if ( int_userid == (*user)->userid )
                {
                    if ( ((*user)->groupid != 1) && ((*user)->groupid != 2) && ((*user)->groupid != 6) )
                    {
                        vilslog::printf("DeviceManager::check_UserGroupID() int_userid[%d] groupid[%d] -> 6\n", int_userid, (*user)->groupid);
                        (*user)->groupid = 6;// 6:主集合管理員
                        baseDb->update_UserStruct((*user));
                    }
                    break;
                }
            }
        }
    }
    return 0;
}

void DeviceManager::reload_sys_config()
{
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        MySQLDBAccess * baseDb = mDbManager->getBaseDB(mDeviceInfoList[i]->projectid);
        if (baseDb != NULL)
        {
            if (mDeviceInfoList[i]->mSysConfig != NULL)
            {
                baseDb->read_sys_config(mDeviceInfoList[i]->mSysConfig);
                checkTagVoltageLow(mDeviceInfoList[i]->mTagNodeList, mDeviceInfoList[i]->mSysConfig->TagAlertVoltageLow);
            }
        }
    }
}

void DeviceManager::reload_RFIDDevice()
{
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        MySQLDBAccess * baseDb = mDbManager->getBaseDB(mDeviceInfoList[i]->projectid);
        if (baseDb != NULL)
        {
            //=================================================
            // 1.先比對是否有增加
            std::list<RFIDDeviceStruct*> * rfidDeviceList = baseDb->get_RFIDDevices();
            for (std::list<RFIDDeviceStruct*>::iterator rfiddevice = rfidDeviceList->begin(); rfiddevice != rfidDeviceList->end(); rfiddevice++)
            {
                bool bFound = false;
                for (std::list<RFIDDeviceStruct*>::iterator existed = mDeviceInfoList[i]->mRFIDDeviceList->begin(); existed != mDeviceInfoList[i]->mRFIDDeviceList->end(); existed++)
                {
                    if ( (*rfiddevice)->macaddress.compare((*existed)->macaddress) == 0 )
                    {
                        (*existed)->deviceid = (*rfiddevice)->deviceid;
                        (*existed)->name = (*rfiddevice)->name;
                        (*existed)->type = (*rfiddevice)->type;
                        (*existed)->ip = (*rfiddevice)->ip;
                        (*existed)->gwid = (*rfiddevice)->gwid;
                        (*existed)->LoRAMacaddress = (*rfiddevice)->LoRAMacaddress;
                        (*existed)->datetime = (*rfiddevice)->datetime;
                        (*existed)->maingroupid = (*rfiddevice)->maingroupid;

                        bFound = true;
                        break;
                    }
                }

                if (!bFound)
                {
                    vilslog::printf("DeviceManager::reload_RFIDDevice() Found new RFIDDevice macaddress[%s] LoRAMacaddress[%s] gwid[%s]\n",
                        (*rfiddevice)->macaddress.c_str(), (*rfiddevice)->LoRAMacaddress.c_str(), (*rfiddevice)->gwid.c_str());

                    RFIDDeviceStruct * record = new RFIDDeviceStruct();
                    record->alive = (*rfiddevice)->alive;
                    record->deviceid = (*rfiddevice)->deviceid;
                    record->macaddress = (*rfiddevice)->macaddress;
                    record->type = (*rfiddevice)->type;
                    record->name = (*rfiddevice)->name;
                    record->ip = (*rfiddevice)->ip;
                    record->gwid = (*rfiddevice)->gwid;
                    record->LoRAMacaddress = (*rfiddevice)->LoRAMacaddress;
                    record->datetime = (*rfiddevice)->datetime;
                    record->maingroupid = (*rfiddevice)->maingroupid;

                    //===================================
                    record->verifykey = baseDb->create_verifykey("RFIDDevice");
                    baseDb->update_RFIDDevice(record);
                    //===================================

                    mDeviceInfoList[i]->mRFIDDeviceList->push_back(record);
                }
            }

            // 2.再比對是否有刪除
            for (std::list<RFIDDeviceStruct*>::iterator existed = mDeviceInfoList[i]->mRFIDDeviceList->begin(); existed != mDeviceInfoList[i]->mRFIDDeviceList->end(); existed++)
            {
                bool bFound = false;
                for (std::list<RFIDDeviceStruct*>::iterator rfiddevice = rfidDeviceList->begin(); rfiddevice != rfidDeviceList->end(); rfiddevice++)
                {
                    if ( (*rfiddevice)->macaddress.compare((*existed)->macaddress) == 0 )
                    {
                        bFound = true;
                        break;
                    }
                }

                if (!bFound)
                {
                    vilslog::printf("DeviceManager::reload_RFIDDevice() Found delete RFIDDevice macaddress[%s] LoRAMacaddress[%s] gwid[%s]\n",
                        (*existed)->macaddress.c_str(), (*existed)->LoRAMacaddress.c_str(), (*existed)->gwid.c_str());

                    // 清空資料
                    (*existed)->alive = 0;
                    (*existed)->deviceid = -1;
                    (*existed)->name = "";
                    (*existed)->type = "";
                    (*existed)->ip = "";
                    (*existed)->gwid = "";
                    (*existed)->LoRAMacaddress = "";
                    (*existed)->datetime = "";
                    (*existed)->aliveTime = 0;
                }
            }
            clear_RFIDDeviceStruct_list(rfidDeviceList);
            delete rfidDeviceList;

            //=================================================
            // if (mDeviceInfoList[i]->mRFIDDeviceList != NULL)
            // {
            //     clear_RFIDDeviceStruct_list(mDeviceInfoList[i]->mRFIDDeviceList);
            //     delete mDeviceInfoList[i]->mRFIDDeviceList;
            // }
            // mDeviceInfoList[i]->mRFIDDeviceList = baseDb->get_RFIDDevices();
            //=================================================
        }
    }
}

void DeviceManager::reload_NFCDevice()
{
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        MySQLDBAccess * baseDb = mDbManager->getBaseDB(mDeviceInfoList[i]->projectid);
        if (baseDb != NULL)
        {
            //=================================================
            // 1.先比對是否有增加
            std::list<NFCDeviceStruct*> * nfcDeviceList = baseDb->get_NFCDevices();
            for (std::list<NFCDeviceStruct*>::iterator nfcdevice = nfcDeviceList->begin(); nfcdevice != nfcDeviceList->end(); nfcdevice++)
            {
                bool bFound = false;
                for (std::list<NFCDeviceStruct*>::iterator existed = mDeviceInfoList[i]->mNFCDeviceList->begin(); existed != mDeviceInfoList[i]->mNFCDeviceList->end(); existed++)
                {
                    if ( (*nfcdevice)->macaddress.compare((*existed)->macaddress) == 0 )
                    {
                        // vilslog::printf("DeviceManager::reload_NFCDevice() Found existed NFCDevice macaddress[%s] LoRAMacaddress[%s] gwid[%s]\n",
                        //     (*existed)->macaddress.c_str(), (*nfcdevice)->LoRAMacaddress.c_str(), (*nfcdevice)->gwid.c_str());

                        (*existed)->deviceid = (*nfcdevice)->deviceid;
                        (*existed)->name = (*nfcdevice)->name;
                        (*existed)->gwid = (*nfcdevice)->gwid;
                        (*existed)->LoRAMacaddress = (*nfcdevice)->LoRAMacaddress;
                        (*existed)->datetime = (*nfcdevice)->datetime;
                        (*existed)->ssid = (*nfcdevice)->ssid;
                        (*existed)->wifimac = (*nfcdevice)->wifimac;
                        (*existed)->maingroupid = (*nfcdevice)->maingroupid;

                        if ( (*nfcdevice)->type.length() >= 0 )
                        {
                            (*existed)->type = (*nfcdevice)->type;
                        }
                        if ( (*nfcdevice)->version.length() >= 0 )
                        {
                            (*existed)->version = (*nfcdevice)->version;
                        }

                        if ( (*nfcdevice)->verifykey.length() == 0 )
                        {
                            if ( (*existed)->verifykey.length() == 0 )
                            {
                                // create new key
                                 (*existed)->verifykey = baseDb->create_verifykey("NFCDevice");
                                 (*nfcdevice)->verifykey = (*existed)->verifykey;

                            }
                            else
                            {
                                (*nfcdevice)->verifykey = (*existed)->verifykey;
                            }
                            baseDb->update_NFCDevice((*nfcdevice));
                        }
                        else
                        {
                            // database data overwrite memory data
                            (*existed)->verifykey = (*nfcdevice)->verifykey;
                        }

                        bFound = true;
                        break;
                    }
                }

                if (!bFound)
                {
                    // vilslog::printf("DeviceManager::reload_NFCDevice() Found new NFCDevice macaddress[%s] LoRAMacaddress[%s] gwid[%s]\n",
                    //     (*nfcdevice)->macaddress.c_str(), (*nfcdevice)->LoRAMacaddress.c_str(), (*nfcdevice)->gwid.c_str());

                    NFCDeviceStruct * record = new NFCDeviceStruct();
                    record->alive = (*nfcdevice)->alive;
                    record->deviceid = (*nfcdevice)->deviceid;
                    record->macaddress = (*nfcdevice)->macaddress;
                    record->type = (*nfcdevice)->type;
                    record->name = (*nfcdevice)->name;
                    record->gwid = (*nfcdevice)->gwid;
                    record->LoRAMacaddress = (*nfcdevice)->LoRAMacaddress;
                    record->datetime = (*nfcdevice)->datetime;
                    record->ssid = (*nfcdevice)->ssid;
                    record->wifimac = (*nfcdevice)->wifimac;
                    record->version = (*nfcdevice)->version;
                    record->maingroupid = (*nfcdevice)->maingroupid;
                    record->verifykey = baseDb->create_verifykey("NFCDevice");

                    baseDb->update_NFCDevice(record);

                    mDeviceInfoList[i]->mNFCDeviceList->push_back(record);
                }
            }

            // 2.再比對是否有刪除
            for (std::list<NFCDeviceStruct*>::iterator existed = mDeviceInfoList[i]->mNFCDeviceList->begin(); existed != mDeviceInfoList[i]->mNFCDeviceList->end(); existed++)
            {
                bool bFound = false;
                for (std::list<NFCDeviceStruct*>::iterator nfcdevice = nfcDeviceList->begin(); nfcdevice != nfcDeviceList->end(); nfcdevice++)
                {
                    if ( (*nfcdevice)->macaddress.compare((*existed)->macaddress) == 0 )
                    {
                        bFound = true;
                        break;
                    }
                }

                if (!bFound)
                {
                    vilslog::printf("DeviceManager::reload_NFCDevice() Found delete NFCDevice macaddress[%s] LoRAMacaddress[%s] gwid[%s]\n",
                        (*existed)->macaddress.c_str(), (*existed)->LoRAMacaddress.c_str(), (*existed)->gwid.c_str());

                    // 清空資料
                    (*existed)->alive = 0;
                    (*existed)->deviceid = -1;
                    (*existed)->name = "";
                    (*existed)->type = "";
                    (*existed)->gwid = "";
                    (*existed)->LoRAMacaddress = "";
                    (*existed)->datetime = "";
                    (*existed)->aliveTime = 0;
                    (*existed)->ssid = "";
                    (*existed)->wifimac = "";
                    (*existed)->version = "";
                    (*existed)->maingroupid = 1;
                    (*existed)->verifykey = "";
                }
            }
            clear_NFCDeviceStruct_list(nfcDeviceList);
            delete nfcDeviceList;

            //=================================================
            // if (mDeviceInfoList[i]->mNFCDeviceList != NULL)
            // {
            //     clear_NFCDeviceStruct_list(mDeviceInfoList[i]->mNFCDeviceList);
            //     delete mDeviceInfoList[i]->mNFCDeviceList;
            // }
            // mDeviceInfoList[i]->mNFCDeviceList = baseDb->get_NFCDevices();
            //=================================================
        }
    }
}

void DeviceManager::reload_EDGEDevice()
{
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        MySQLDBAccess * baseDb = mDbManager->getBaseDB(mDeviceInfoList[i]->projectid);
        if (baseDb != NULL)
        {
            //=================================================
            // 1.先比對是否有增加
            std::list<EDGEDeviceStruct*> * edgeDeviceList = baseDb->get_EDGEDevices();
            for (std::list<EDGEDeviceStruct*>::iterator edgedevice = edgeDeviceList->begin(); edgedevice != edgeDeviceList->end(); edgedevice++)
            {
                bool bFound = false;
                for (std::list<EDGEDeviceStruct*>::iterator existed = mDeviceInfoList[i]->mEDGEDeviceList->begin(); existed != mDeviceInfoList[i]->mEDGEDeviceList->end(); existed++)
                {
                    if ( (*edgedevice)->macaddress.compare((*existed)->macaddress) == 0 )
                    {
                        // vilslog::printf("DeviceManager::reload_EDGEDevice() Found existed EDGEDevice macaddress[%s] LoRAMacaddress[%s] gwid[%s]\n",
                        //     (*existed)->macaddress.c_str(), (*edgedevice)->LoRAMacaddress.c_str(), (*edgedevice)->gwid.c_str());

                        (*existed)->deviceid = (*edgedevice)->deviceid;
                        (*existed)->name = (*edgedevice)->name;
                        (*existed)->type = (*edgedevice)->type;
                        (*existed)->LoRAMacaddress = (*edgedevice)->LoRAMacaddress;
                        (*existed)->gwid = (*edgedevice)->gwid;
                        (*existed)->datetime = (*edgedevice)->datetime;
                        (*existed)->version = (*edgedevice)->version;
                        (*existed)->aliveTime = (*edgedevice)->aliveTime;
                        (*existed)->maingroupid = (*edgedevice)->maingroupid;

                        bFound = true;
                        break;
                    }
                }

                if (!bFound)
                {
                    // vilslog::printf("DeviceManager::reload_EDGEDevice() Found new EDGEDevice macaddress[%s] LoRAMacaddress[%s] gwid[%s]\n",
                    //     (*edgedevice)->macaddress.c_str(), (*edgedevice)->LoRAMacaddress.c_str(), (*edgedevice)->gwid.c_str());

                    EDGEDeviceStruct * record = new EDGEDeviceStruct();
                    record->deviceid = (*edgedevice)->deviceid;
                    record->macaddress = (*edgedevice)->macaddress;
                    record->name = (*edgedevice)->name;
                    record->type = (*edgedevice)->type;
                    record->ip = (*edgedevice)->ip;
                    record->LoRAMacaddress = (*edgedevice)->LoRAMacaddress;
                    record->gwid = (*edgedevice)->gwid;
                    record->datetime = (*edgedevice)->datetime;
                    record->alive = (*edgedevice)->alive;
                    record->aliveTime = (*edgedevice)->aliveTime;
                    record->version = (*edgedevice)->version;
                    record->maingroupid = (*edgedevice)->maingroupid;

                    //===================================
                    record->verifykey = baseDb->create_verifykey("EDGEDevice");
                    baseDb->update_EDGEDevice(record);
                    //===================================

                    mDeviceInfoList[i]->mEDGEDeviceList->push_back(record);
                }
            }

            // 2.再比對是否有刪除
            for (std::list<EDGEDeviceStruct*>::iterator existed = mDeviceInfoList[i]->mEDGEDeviceList->begin(); existed != mDeviceInfoList[i]->mEDGEDeviceList->end(); existed++)
            {
                bool bFound = false;
                for (std::list<EDGEDeviceStruct*>::iterator edgedevice = edgeDeviceList->begin(); edgedevice != edgeDeviceList->end(); edgedevice++)
                {
                    if ( (*edgedevice)->macaddress.compare((*existed)->macaddress) == 0 )
                    {
                        bFound = true;
                        break;
                    }
                }

                if (!bFound)
                {
                    vilslog::printf("DeviceManager::reload_EDGEDevice() Found delete EDGEDevice macaddress[%s] LoRAMacaddress[%s] gwid[%s]\n",
                        (*existed)->macaddress.c_str(), (*existed)->LoRAMacaddress.c_str(), (*existed)->gwid.c_str());

                    // 清空資料
                    (*existed)->macaddress = "";
                    (*existed)->alive = 0;
                    (*existed)->deviceid = -1;
                    (*existed)->name = "";
                    (*existed)->type = "";
                    (*existed)->ip = "";
                    (*existed)->LoRAMacaddress = "";
                    (*existed)->gwid = "";
                    (*existed)->datetime = "";
                    (*existed)->aliveTime = 0;
                    (*existed)->version = "";
                    (*existed)->maingroupid = 1;
                    (*existed)->verifykey = "";
                }
            }
            clear_EDGEDeviceStruct_list(edgeDeviceList);
            delete edgeDeviceList;
        }
    }
}

void DeviceManager::reload_RFIDCard()
{
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        MySQLDBAccess * baseDb = mDbManager->getBaseDB(mDeviceInfoList[i]->projectid);
        if (baseDb != NULL)
        {
            //=================================================
            // 1.先比對是否有增加
            std::list<RFIDCardStruct*> * rfidCardList = baseDb->get_RFIDCards();
            for (std::list<RFIDCardStruct*>::iterator rfidcard = rfidCardList->begin(); rfidcard != rfidCardList->end(); rfidcard++)
            {
                bool bFound = false;
                for (std::list<RFIDCardStruct*>::iterator existed = mDeviceInfoList[i]->mRFIDCardList->begin(); existed != mDeviceInfoList[i]->mRFIDCardList->end(); existed++)
                {
                    if ( (*existed)->cardid.compare((*rfidcard)->cardid) == 0 )
                    {
                        (*existed)->userid = (*rfidcard)->userid;
                        (*existed)->name = (*rfidcard)->name;
                        (*existed)->type = (*rfidcard)->type;
                        (*existed)->maplayer = (*rfidcard)->maplayer;
                        (*existed)->unknown = (*rfidcard)->unknown;
                        (*existed)->deviceid = (*rfidcard)->deviceid;
                        (*existed)->alive = (*rfidcard)->alive;
                        (*existed)->datetime = (*rfidcard)->datetime;
                        (*existed)->updatetime = (*rfidcard)->updatetime;
                        (*existed)->maingroupid = (*rfidcard)->maingroupid;

                        bFound = true;
                        break;
                    }
                }

                if (!bFound)
                {
                    vilslog::printf("DeviceManager::reload_RFIDCard() Found new RFIDCard cardid[%s] deviceid[%d]\n",
                        (*rfidcard)->cardid.c_str(), (*rfidcard)->deviceid);

                    RFIDCardStruct * record = new RFIDCardStruct();
                    record->cardid = (*rfidcard)->cardid;
                    record->userid = (*rfidcard)->userid;
                    record->name = (*rfidcard)->name;
                    record->type = (*rfidcard)->type;
                    record->maplayer = (*rfidcard)->maplayer;
                    record->unknown = (*rfidcard)->unknown;
                    record->deviceid = (*rfidcard)->deviceid;
                    record->alive = (*rfidcard)->alive;
                    record->datetime = (*rfidcard)->datetime;
                    record->updatetime = (*rfidcard)->updatetime;
                    record->maingroupid = (*rfidcard)->maingroupid;

                    mDeviceInfoList[i]->mRFIDCardList->push_back(record);
                }
            }

            // 2.再比對是否有刪除
            for (std::list<RFIDCardStruct*>::iterator existed = mDeviceInfoList[i]->mRFIDCardList->begin(); existed != mDeviceInfoList[i]->mRFIDCardList->end(); existed++)
            {
                bool bFound = false;
                for (std::list<RFIDCardStruct*>::iterator rfidcard = rfidCardList->begin(); rfidcard != rfidCardList->end(); rfidcard++)
                {
                    if ( (*existed)->cardid.compare((*rfidcard)->cardid) == 0 )
                    {
                        bFound = true;
                        break;
                    }
                }

                if (!bFound)
                {
                    vilslog::printf("DeviceManager::reload_RFIDCard() Found delete RFIDCard cardid[%s] deviceid[%d]\n",
                        (*existed)->cardid.c_str(), (*existed)->deviceid);

                    // 清空資料
                    (*existed)->cardid = "";
                    (*existed)->userid = 0;
                    (*existed)->name = "";
                    (*existed)->type = "";
                    (*existed)->maplayer = 0;
                    (*existed)->unknown = 1;
                    (*existed)->deviceid = 0;
                    (*existed)->alive = 0;
                    (*existed)->datetime = "";
                    (*existed)->updatetime = "";
                }
            }
            clear_RFIDCardStruct_list(rfidCardList);
            delete rfidCardList;

            //=================================================
            // if (mDeviceInfoList[i]->mRFIDCardList != NULL)
            // {
            //     clear_RFIDCardStruct_list(mDeviceInfoList[i]->mRFIDCardList);
            //     delete mDeviceInfoList[i]->mRFIDCardList;
            // }
            // mDeviceInfoList[i]->mRFIDCardList = baseDb->get_RFIDCards();
            //=================================================
        }
    }
}

void DeviceManager::reload_NFCCard()
{
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        MySQLDBAccess * baseDb = mDbManager->getBaseDB(mDeviceInfoList[i]->projectid);
        if (baseDb != NULL)
        {
            //=================================================
            // 1.先比對是否有增加
            std::list<NFCCardStruct*> * nfcCardList = baseDb->get_NFCCards();
            for (std::list<NFCCardStruct*>::iterator nfccard = nfcCardList->begin(); nfccard != nfcCardList->end(); nfccard++)
            {
                bool bFound = false;
                for (std::list<NFCCardStruct*>::iterator existed = mDeviceInfoList[i]->mNFCCardList->begin(); existed != mDeviceInfoList[i]->mNFCCardList->end(); existed++)
                {
                    if ( (*existed)->cardid.compare((*nfccard)->cardid) == 0 )
                    {
                        (*existed)->userid = (*nfccard)->userid;
                        (*existed)->name = (*nfccard)->name;
                        (*existed)->type = (*nfccard)->type;
                        (*existed)->maplayer = (*nfccard)->maplayer;
                        (*existed)->unknown = (*nfccard)->unknown;
                        (*existed)->deviceid = (*nfccard)->deviceid;
                        (*existed)->alive = (*nfccard)->alive;
                        (*existed)->datetime = (*nfccard)->datetime;
                        (*existed)->updatetime = (*nfccard)->updatetime;
                        (*existed)->maingroupid = (*nfccard)->maingroupid;

                        bFound = true;
                        break;
                    }
                }

                if (!bFound)
                {
                    vilslog::printf("DeviceManager::reload_NFCCard() Found new NFCCard cardid[%s] deviceid[%d]\n",
                        (*nfccard)->cardid.c_str(), (*nfccard)->deviceid);

                    NFCCardStruct * record = new NFCCardStruct();
                    record->cardid = (*nfccard)->cardid;
                    record->userid = (*nfccard)->userid;
                    record->name = (*nfccard)->name;
                    record->type = (*nfccard)->type;
                    record->maplayer = (*nfccard)->maplayer;
                    record->unknown = (*nfccard)->unknown;
                    record->deviceid = (*nfccard)->deviceid;
                    record->alive = (*nfccard)->alive;
                    record->datetime = (*nfccard)->datetime;
                    record->updatetime = (*nfccard)->updatetime;
                    record->maingroupid = (*nfccard)->maingroupid;

                    mDeviceInfoList[i]->mNFCCardList->push_back(record);
                }
            }

            // 2.再比對是否有刪除
            for (std::list<NFCCardStruct*>::iterator existed = mDeviceInfoList[i]->mNFCCardList->begin(); existed != mDeviceInfoList[i]->mNFCCardList->end(); existed++)
            {
                bool bFound = false;
                for (std::list<NFCCardStruct*>::iterator nfccard = nfcCardList->begin(); nfccard != nfcCardList->end(); nfccard++)
                {
                    if ( (*existed)->cardid.compare((*nfccard)->cardid) == 0 )
                    {
                        bFound = true;
                        break;
                    }
                }

                if (!bFound)
                {
                    vilslog::printf("DeviceManager::reload_RFIDCard() Found delete RFIDCard cardid[%s] deviceid[%d]\n",
                        (*existed)->cardid.c_str(), (*existed)->deviceid);

                    // 清空資料
                    (*existed)->cardid = "";
                    (*existed)->userid = 0;
                    (*existed)->name = "";
                    (*existed)->type = "";
                    (*existed)->maplayer = 0;
                    (*existed)->unknown = 1;
                    (*existed)->deviceid = 0;
                    (*existed)->alive = 0;
                    (*existed)->datetime = "";
                    (*existed)->updatetime = "";
                }
            }
            clear_NFCCardStruct_list(nfcCardList);
            delete nfcCardList;

            //=================================================
            // if (mDeviceInfoList[i]->mNFCCardList != NULL)
            // {
            //     clear_NFCCardStruct_list(mDeviceInfoList[i]->mNFCCardList);
            //     delete mDeviceInfoList[i]->mNFCCardList;
            // }
            // mDeviceInfoList[i]->mNFCCardList = baseDb->get_NFCCards();
            //=================================================

        }
    }
}

void DeviceManager::reload_SensorDevice()
{
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        MySQLDBAccess * baseDb = mDbManager->getBaseDB(mDeviceInfoList[i]->projectid);
        if (baseDb != NULL)
        {
            mDeviceInfoList[i]->mSensorDeviceList->clear();
            mDeviceInfoList[i]->mSensorDeviceList = baseDb->get_SensorDevices();
        }
    }
}

void DeviceManager::reload_User()
{
    DeviceInfoListStruct * deviceInfo = NULL;

    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        deviceInfo = mDeviceInfoList[i];

        MySQLDBAccess * baseDb = mDbManager->getBaseDB(deviceInfo->projectid);
        if (baseDb != NULL)
        {
            std::list<UserStruct*> * userList = baseDb->get_Users();

            //=================================================
            // 1.先比對是否有增加
            bool bFound = false;
            for (std::list<UserStruct*>::iterator user = userList->begin(); user != userList->end(); user++)
            {
                bFound = false;
                for (std::list<UserStruct*>::iterator existed = deviceInfo->mUserList->begin(); existed != deviceInfo->mUserList->end(); existed++)
                {
                    //if( ((*user)->account.compare((*existed)->account) == 0) && ((*user)->userid == (*existed)->userid))
                    if( (boost::iequals((*user)->account, (*existed)->account)) && ((*user)->userid == (*existed)->userid))
                    {
                        clear_UserStruct((*existed));

                        copyUserStruct((*user), (*existed));

                        bFound = true;
                        break;
                    }
                }

                 if(!bFound)
                 {
                     UserStruct * newUser = new UserStruct();

                     copyUserStruct((*user), newUser);

                     deviceInfo->mUserList->push_back(newUser);
                 }
            }

            // 2.再比對是否有刪除
            for (std::list<UserStruct*>::iterator existed = deviceInfo->mUserList->begin(); existed != deviceInfo->mUserList->end(); existed++)
            {
                bFound = false;
                for (std::list<UserStruct*>::iterator user = userList->begin(); user != userList->end(); user++)
                {
                    //if( ((*user)->account.compare((*existed)->account) == 0) && ((*user)->userid == (*existed)->userid))
                    if( (boost::iequals((*user)->account, (*existed)->account)) && ((*user)->userid == (*existed)->userid))
                    {
                        bFound = true;
                        break;
                    }
                }

                if(!bFound)
                {
                    clear_UserStruct((*existed));

                    (*existed)->userid = 0;
                    (*existed)->groupid = 0;
                    (*existed)->type = 0;
                    (*existed)->empid = "";
                    (*existed)->account = "";
                    (*existed)->name = "";
                    (*existed)->unit = "";
                    (*existed)->title = "";
                    (*existed)->gender = "";
                    (*existed)->pwd = "";
                    (*existed)->datetime = "";

                    (*existed)->maingroups = new std::list<std::string>();
                    (*existed)->datagroups = new std::list<std::string>();
                    (*existed)->subgroups = new std::list<std::string>();
                    (*existed)->rfidcards = new std::list<std::string>();
                    (*existed)->nfccards = new std::list<std::string>();
                    (*existed)->uwbcards = new std::list<std::string>();
                    (*existed)->sensors = new std::list<std::string>();
                    (*existed)->imageuid = "";

                    (*existed)->maplayer = 0;
                    (*existed)->posX = 0;
                    (*existed)->posY = 0;
                    (*existed)->posZ = 0;
                    (*existed)->Longitude = "";
                    (*existed)->Latitude = "";
                }
            }
            //=================================================


            clear_UserStruct_list(userList);
            delete userList;
        }
    }
}

void DeviceManager::reload_LoRaGateway()
{
    DeviceInfoListStruct * deviceInfo = NULL;

    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        deviceInfo = mDeviceInfoList[i];
        MySQLDBAccess * baseDb = mDbManager->getBaseDB(deviceInfo->projectid);
        if (baseDb != NULL)
        {
            auto loRaGatewayList = deviceInfo->mLoRaGatewayList;

            deviceInfo->mLoRaGatewayList = baseDb->read_LoRaGateway();

            clear_LoRaGateway_list(loRaGatewayList);
        }
    }
}

int DeviceManager::initialTagSmoothingPos()
{
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        for (std::list<NodeStruct*>::iterator existed = mDeviceInfoList[i]->mTagNodeList->begin(); existed != mDeviceInfoList[i]->mTagNodeList->end(); existed++)
        {
            (*existed)->smoothingPosX = (*existed)->posX;
            (*existed)->smoothingPosY = (*existed)->posY;
            (*existed)->smoothingPosZ = (*existed)->posZ;
        }
    }
    return 0;
}

DeviceInfoListStruct * DeviceManager::getDeviceInfoList(int projectid)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = NULL;
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        if (mDeviceInfoList[i]->projectid == projectid)
        {
            deviceInfo = mDeviceInfoList[i];
            bFound = true;
            break;
        }
    }
    if (!bFound)
    {
        DeviceInfoListStruct * newInfo = new DeviceInfoListStruct();

        readDeviceInfoFromDB(projectid, newInfo);

        // MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        // newInfo->projectid = projectid;
        // newInfo->mAnchorNodeList = baseDb->read_nodes(NODE_TYPE_ANCHOR);
        // newInfo->mTagNodeList = baseDb->read_nodes(NODE_TYPE_TAG);
        // newInfo->mCoordNodeList = baseDb->read_nodes(NODE_TYPE_COORD);
        // newInfo->mLocatorNodeList = baseDb->read_nodes(NODE_TYPE_LOCATOR);
        // newInfo->mLoRaGatewayList = baseDb->read_LoRaGateway();
        // newInfo->mSysConfig = new VILSSystemConfig();
        // baseDb->read_sys_config(newInfo->mSysConfig);
        //
        // checkTagVoltageLow(newInfo->mTagNodeList, newInfo->mSysConfig->TagAlertVoltageLow);

        mDeviceInfoList.push_back(newInfo);

        deviceInfo = newInfo;
    }

    return deviceInfo;
}

std::list<int> * DeviceManager::getAllProjectConfig()
{
    std::list<int> * prjidList = mDbManager->getAllProjectConfig();
    return prjidList;
}

// bool DeviceManager::set_tag_area_change(int projectid, NodeStruct* node)
// {
//     bool bFound = false;
//
//     if ( strlen(node->area_AreaId) == 0 )
//     {
//         return bFound;
//     }
//
//     DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
//
//     bool bNewAction = false;
//     for (int i=0; i< deviceInfo->mAreaInOutDataVector.size(); i++)
//     {
//         // check AreaId
//         if ( deviceInfo->mAreaInOutDataVector[i]->getAreaId().compare(node->area_AreaId) == 0 )
//         {
//             // vilslog::printf ("DeviceManager::set_tag_area_change() index[%d] area_AreaId[%s] area_changed[%d]\n",
//             //     node->index, node->area_AreaId, node->area_changed);
//             bNewAction = deviceInfo->mAreaInOutDataVector[i]->setNewAction(node->nodeid, node->tafid, node->area_changed, node->AreaType0D);
//
//             deviceInfo->mAreaInOutDataVector[i]->updateAreaInfo(node->area_AreaName, node->area_type);
//
//             bFound = true;
//             break;
//         }
//     }
//     if (!bFound)
//     {
//         // vilslog::printf ("DeviceManager::set_tag_area_change() create new Vector nodeid[%s] area_AreaId[%s] area_changed[%d]\n",
//         //     node->nodeid, node->area_AreaId, node->area_changed);
//
//         AreaInOutDataClass * newData = new AreaInOutDataClass(6);
//         newData->init(node->area_AreaId, node->area_AreaName, node->area_type);
//         bNewAction = newData->setNewAction(node->nodeid, node->tafid, node->area_changed, node->AreaType0D);
//         deviceInfo->mAreaInOutDataVector.push_back(newData);
//     }
//
//     if (bNewAction)
//     {
//        // vilslog::printf ("DeviceManager::set_tag_area_change() New Action nodeid[%s] area_AreaId[%s] area_changed[%d]\n",
//        //     node->nodeid, node->area_AreaId, node->area_changed);
//
//         AreaInOutEventStruct * newEvent = new AreaInOutEventStruct();
//         newEvent->projectid = projectid; // area projectid
//         newEvent->action = node->area_changed; //1:in 2:out
//         newEvent->AreaType0D = node->AreaType0D; // 0:normal 1:room 2:danger
//         newEvent->tafid = node->tafid; // 0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師
//         newEvent->MapAreaType = node->area_type;// 0:普通 1:電子圍欄 2:電子點名 3:房間 4:教室 5:病房 6:病床 999:虛擬牆
//         newEvent->NodeId = node->nodeid;
//         newEvent->NodeName = node->nodename;
//         newEvent->NodeMacaddress = node->macaddress;
//         newEvent->AreaID = node->area_AreaId;
//         newEvent->AreaName = node->area_AreaName;
//         newEvent->DataTime = currentDateTimeForSQL();
//
//         mAreaInOutEventQueue.add(newEvent);
//
//         AreaInOutEventStruct * newEventCourse = new AreaInOutEventStruct();
//         newEventCourse->projectid = projectid; // area projectid
//         newEventCourse->action = node->area_changed; //1:in 2:out
//         newEventCourse->AreaType0D = node->AreaType0D; // 0:normal 1:room 2:danger
//         newEventCourse->tafid = node->tafid; // 0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師
//         newEventCourse->MapAreaType = node->area_type;// 0:普通 1:電子圍欄 2:電子點名 3:房間 4:教室 5:病房 6:病床 999:虛擬牆
//         newEventCourse->NodeId = node->nodeid;
//         newEventCourse->NodeName = node->nodename;
//         newEventCourse->NodeMacaddress = node->macaddress;
//         newEventCourse->AreaID = node->area_AreaId;
//         newEventCourse->AreaName = node->area_AreaName;
//         newEventCourse->DataTime = currentDateTimeForSQL();
//
//         mCourseAreaInOutEventQueue.add(newEventCourse);
//     }
//
//     return bNewAction;
// }

// bool DeviceManager::set_tag_area_action(int projectid, std::string nodeid, int action, std::string areaid, std::string areaname, int AreaType0D, int MapAreaType)
// {
//     DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
//     NodeStruct * node = NULL;
//
//     for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
//     {
//         if ( nodeid.compare((*existed)->nodeid) == 0 )
//         {
//             node = (*existed);
//             break;
//         }
//     }
//     if ( node == NULL )
//     {
//         return false;
//     }
//
//     AreaInOutEventStruct * newEvent = new AreaInOutEventStruct();
//     newEvent->projectid = projectid; // area projectid
//     newEvent->action = action; //1:in 2:out
//     newEvent->AreaType0D = AreaType0D; // 0:normal 1:room 2:danger
//     newEvent->MapAreaType = MapAreaType;// 0:普通 1:電子圍欄 2:電子點名 3:房間 4:教室 5:病房 6:病床 999:虛擬牆
//     newEvent->tafid = node->tafid; // 0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師
//     newEvent->NodeId = node->nodeid;
//     newEvent->NodeName = node->nodename;
//     newEvent->NodeMacaddress = node->macaddress;
//     newEvent->AreaID = areaid;
//     newEvent->AreaName = areaname;
//     newEvent->DataTime = currentDateTimeForSQL();
//
//     mAreaInOutEventQueue.add(newEvent);
//
//     AreaInOutEventStruct * newEventCourse = new AreaInOutEventStruct();
//     newEventCourse->projectid = projectid; // area projectid
//     newEventCourse->action = action; //1:in 2:out
//     newEventCourse->AreaType0D = AreaType0D; // 0:normal 1:room 2:danger
//     newEventCourse->MapAreaType = MapAreaType;// 0:普通 1:電子圍欄 2:電子點名 3:房間 4:教室 5:病房 6:病床 999:虛擬牆
//     newEventCourse->tafid = node->tafid; // 0:人員訪客 1:巡檢員 2:設備 3:VIP貴賓 4:工作人員 5:醫師 6:護理師
//     newEventCourse->NodeId = node->nodeid;
//     newEventCourse->NodeName = node->nodename;
//     newEventCourse->NodeMacaddress = node->macaddress;
//     newEventCourse->AreaID = areaid;
//     newEventCourse->AreaName = areaname;
//     newEventCourse->DataTime = currentDateTimeForSQL();
//
//     mCourseAreaInOutEventQueue.add(newEventCourse);
//
//     vilslog::printf("\nDeviceManager::set_tag_area_action() nodeid[%s][%d][%s]\n",
//         node->nodeid, action, areaname.c_str());
//
//     return true;
// }
bool DeviceManager::add_Anchor_node(int projectid, NodeStruct * node)
{
    bool added = false;
    bool bFound = false;

    vilslog::printf("DeviceManager::add_Anchor_node() projectid[%d] nodeid[%s]\n", projectid, node->nodeid);

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mAnchorNodeList->begin(); existed != deviceInfo->mAnchorNodeList->end(); existed++)
    {
        if ( strcmp(node->macaddress, (*existed)->macaddress) == 0 )
        {
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        NodeStruct * newNode = copyNodeStruct(node);
        newNode->index = getEmptyIndex(deviceInfo->mAnchorNodeList);

        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if (baseDb != NULL)
        {
            newNode->verifykey = baseDb->create_verifykey("NodeInfo");
            baseDb->write_node_info(newNode);
        }

        deviceInfo->mAnchorNodeList->push_back(newNode);
        added = true;
    }

    return added;
}

bool DeviceManager::add_Tag_node(int projectid, NodeStruct * node)
{
    bool added = false;
    bool bFound = false;

    vilslog::printf("DeviceManager::add_Tag_node() projectid[%d] nodeid[%s]\n", projectid, node->nodeid);

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(node->macaddress, (*existed)->macaddress) == 0 )
        {
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        NodeStruct * newNode = copyNodeStruct(node);
        newNode->index = getEmptyIndex(deviceInfo->mTagNodeList);

        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if (baseDb != NULL)
        {
            newNode->verifykey = baseDb->create_verifykey("NodeInfo");
            baseDb->write_node_info(newNode);
        }

        deviceInfo->mTagNodeList->push_back(newNode);
        added = true;
    }

    return added;
}

bool DeviceManager::add_Cooridinator_node(int projectid, NodeStruct * node)
{
    bool added = false;
    bool bFound = false;

    vilslog::printf("DeviceManager::add_Cooridinator_node() projectid[%d] nodeid[%s]\n", projectid, node->nodeid);

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mCoordNodeList->begin(); existed != deviceInfo->mCoordNodeList->end(); existed++)
    {
        if ( strcmp(node->macaddress, (*existed)->macaddress) == 0 )
        {
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        NodeStruct * newNode = copyNodeStruct(node);
        newNode->index = getEmptyIndex(deviceInfo->mCoordNodeList);

        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if (baseDb != NULL)
        {
            newNode->verifykey = baseDb->create_verifykey("NodeInfo");
            baseDb->write_node_info(newNode);
        }

        deviceInfo->mCoordNodeList->push_back(newNode);
        added = true;
    }

    return added;
}

bool DeviceManager::add_Locator_node(int projectid, NodeStruct * node)
{
    bool added = false;
    bool bFound = false;

    vilslog::printf("DeviceManager::add_Locator_node() projectid[%d] nodeid[%s]\n", projectid, node->nodeid);

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( strcmp(node->macaddress, (*existed)->macaddress) == 0 )
        {
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        NodeStruct * newNode = copyNodeStruct(node);
        newNode->index = getEmptyIndex(deviceInfo->mLocatorNodeList);

        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if (baseDb != NULL)
        {
            newNode->verifykey = baseDb->create_verifykey("NodeInfo");
            baseDb->write_node_info(newNode);
        }

        deviceInfo->mLocatorNodeList->push_back(newNode);
        added = true;
    }

    return added;
}

bool DeviceManager::add_LoRaGateway_node(int projectid, LoRaGatewayInfoStruct * node)
{
    bool added = false;
    bool bFound = false;

    vilslog::printf("DeviceManager::add_LoRaGateway_node() projectid[%d] nodeid[%s]\n", projectid, node->gwid.c_str());

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<LoRaGatewayInfoStruct*>::iterator existed = deviceInfo->mLoRaGatewayList->begin(); existed != deviceInfo->mLoRaGatewayList->end(); existed++)
    {
        if ( node->gwid.compare((*existed)->gwid) == 0 )
        {
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        LoRaGatewayInfoStruct * newgateway = new LoRaGatewayInfoStruct();
        newgateway->alive = node->alive;
        newgateway->MQTTalive = node->MQTTalive;
        newgateway->aliveTime = node->aliveTime;
        newgateway->pingCount = node->pingCount;
        newgateway->sf = node->sf;
        newgateway->projectid = projectid;
        newgateway->gwip = node->gwip;
        newgateway->gwid = node->gwid;
        newgateway->repeater = node->repeater;
        for (std::list<std::string>::iterator channel = node->channels.begin(); channel != node->channels.end(); channel++)
        {
            newgateway->channels.push_back((*channel));
        }
        newgateway->DataTime = node->DataTime;
        newgateway->maplayer = 1;
        newgateway->posX = 0;
        newgateway->posY = 0;
        newgateway->posZ = 0;
        newgateway->alive = 0;// alive count
        newgateway->MQTTalive = 0;// MQTT alive count
        newgateway->pingCount = 0;
        newgateway->aliveTime = 0;
        newgateway->maingroupid = node->maingroupid;

        //===================================
        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        newgateway->verifykey = baseDb->create_verifykey("LoRaGatewayInfo");
        baseDb->write_LoRaGatewayInfo(newgateway);
        //===================================

        deviceInfo->mLoRaGatewayList->push_back(newgateway);

        added = true;
    }

    return added;
}

bool DeviceManager::updateAoAList(int projectid, const char * LoRA_macaddress)
{
    bool added = false;
    bool bFound = false;

    // vilslog::printf("DeviceManager::updateAoAList() projectid[%d] LoRA_macaddress[%s]\n", projectid, LoRA_macaddress);

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( strcmp(LoRA_macaddress, (*existed)->LoRAMacaddress) == 0 )
        {

            //
            // find exited AoA node
            //
            for (std::list<AoANodeStruct*>::iterator aoaNode = deviceInfo->mAoANodeList->begin(); aoaNode != deviceInfo->mAoANodeList->end(); aoaNode++)
            {
                if ( strcmp((*existed)->nodeid, (*aoaNode)->nodeid) == 0 )
                {
                    bFound = true;
                    break;
                }
            }

            if (!bFound)
            {
                AoANodeStruct * newaoaNode = new AoANodeStruct();

                newaoaNode->projectid = (*existed)->projectid;
                sprintf(newaoaNode->nodeid, "%s", (*existed)->nodeid);
                sprintf(newaoaNode->nodename, "%s", (*existed)->nodename);
                sprintf(newaoaNode->panid, "%s", (*existed)->panid);
                sprintf(newaoaNode->macaddress, "%s", (*existed)->macaddress);
                newaoaNode->maplayer = (*existed)->maplayer;
                newaoaNode->posX =(int)(*existed)->posX;
                newaoaNode->posY = (int)(*existed)->posY;
                newaoaNode->posZ = (int)(*existed)->posZ;
                newaoaNode->pitch = 0;
                newaoaNode->roll = 0;
                newaoaNode->yaw = 0;
                newaoaNode->gridsize = 0;

                //===================================
                MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
                newaoaNode->verifykey = baseDb->create_verifykey("AoANodeInfo");
                baseDb->write_AOA_node_info(newaoaNode);
                //===================================

                deviceInfo->mAoANodeList->push_back(newaoaNode);

                added = true;
            }
            break;
        }
    }

    return added;
}

bool DeviceManager::update_Node_maingroup(const char * macaddress, int maingroupid)
{
    vilslog::printf("DeviceManager::update_Node_maingroup() macaddress[%s] maingroupid[%d]\n", macaddress, maingroupid);

    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        update_Node_maingroup(mDeviceInfoList[i]->mAnchorNodeList, macaddress, maingroupid);
        update_Node_maingroup(mDeviceInfoList[i]->mTagNodeList, macaddress, maingroupid);
        update_Node_maingroup(mDeviceInfoList[i]->mCoordNodeList, macaddress, maingroupid);
        update_Node_maingroup(mDeviceInfoList[i]->mLocatorNodeList, macaddress, maingroupid);
        update_Node_maingroup(mDeviceInfoList[i]->mAoANodeList, macaddress, maingroupid);
    }
    return true;
}

bool DeviceManager::update_Node_maingroup(std::list<NodeStruct*> * nodelist, const char * macaddress, int maingroupid)
{
    for (std::list<NodeStruct*>::iterator existed = nodelist->begin(); existed != nodelist->end(); existed++)
    {
        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            vilslog::printf("DeviceManager::update_Node_maingroup() macaddress[%s] maingroupid[%d] -> [%d]\n",
                macaddress, (*existed)->maingroupid, maingroupid);

            (*existed)->maingroupid = maingroupid;
            break;
        }
    }
    return true;
}

bool DeviceManager::update_Node_maingroup(std::list<AoANodeStruct*> * nodelist, const char * macaddress, int maingroupid)
{
    for (std::list<AoANodeStruct*>::iterator aoaNode = nodelist->begin(); aoaNode != nodelist->end(); aoaNode++)
    {
        if ( strcmp(macaddress, (*aoaNode)->macaddress) == 0 )
        {
            vilslog::printf("DeviceManager::update_Node_maingroup() AoA macaddress[%s] maingroupid[%d] -> [%d]\n",
                macaddress, (*aoaNode)->maingroupid, maingroupid);

            (*aoaNode)->maingroupid = maingroupid;
            break;
        }
    }
    return true;
}

// bool DeviceManager::addCardScanRecord(int projectid, std::string cardid, std::string LoRA_macaddress)
// {
//     DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
//
//     if ( deviceInfo->mCARDScanList == NULL)
//     {
//         // vilslog::printf ("DeviceManager::addCardScanRecord() cardid[%s] deviceInfo->mCARDScanList == NULL\n", cardid.c_str() );
//         deviceInfo->mCARDScanList = new std::list<CARDScanRecordStruct*>();
//     }
//
//     bool bFound = false;
//     for (std::list<CARDScanRecordStruct*>::iterator existed = deviceInfo->mCARDScanList->begin(); existed != deviceInfo->mCARDScanList->end(); existed++)
//     {
//         if ( cardid.compare((*existed)->cardid) == 0 )
//         {
//             // vilslog::printf ("DeviceManager::addCardScanRecord() cardid[%s] (*existed)->scanRecords.size()[%d]\n",
//             //     cardid.c_str(), (int)(*existed)->scanRecords.size() );
//
//             if ( (int)(*existed)->scanRecords.size() > 20)
//             {
//                 // remove old one
//                 std::list<CARDScanStruct*>::iterator oldcard = (*existed)->scanRecords.begin();
//                 (*existed)->scanRecords.remove((*oldcard));
//                 delete (*oldcard);
//
//                 // vilslog::printf ("DeviceManager::addCardScanRecord() remove old one cardid[%s] (*existed)->scanRecords.size()[%d]\n",
//                 //     cardid.c_str(), (int)(*existed)->scanRecords.size() );
//             }
//
//             CARDScanStruct * newCard = new CARDScanStruct();
//             newCard->LoRAMacaddress = LoRA_macaddress;
//             newCard->datetime = currentDateTimeForSQL();
//             (*existed)->scanRecords.push_back(newCard);
//
//             bFound = true;
//             break;
//         }
//     }
//
//     if (!bFound)
//     {
//         // vilslog::printf ("DeviceManager::addCardScanRecord() cardid[%s] new CARDScanRecordStruct\n", cardid.c_str());
//
//         CARDScanRecordStruct * newCardScan = new CARDScanRecordStruct();
//         newCardScan->cardid = cardid;
//         deviceInfo->mCARDScanList->push_back(newCardScan);
//
//         CARDScanStruct * newCard = new CARDScanStruct();
//         newCard->LoRAMacaddress = LoRA_macaddress;
//         newCard->datetime = currentDateTimeForSQL();
//         newCardScan->scanRecords.push_back(newCard);
//     }
//
//     return true;
// }

// std::list<CARDScanRecordStruct*> * DeviceManager::get_CardScan_list(int projectid)
// {
//     DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
//     if ( deviceInfo->mCARDScanList == NULL)
//     {
//         // vilslog::printf ("DeviceManager::addCardScanRecord() cardid[%s] deviceInfo->mCARDScanList == NULL\n", cardid.c_str() );
//         deviceInfo->mCARDScanList = new std::list<CARDScanRecordStruct*>();
//     }
//
//     return deviceInfo->mCARDScanList;
// }

int DeviceManager::resetAnchorIndexs(int projectid)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    int max_index = resetNodeIndexs(deviceInfo->mAnchorNodeList);
    return max_index;
}

int DeviceManager::resetTagIndexs(int projectid)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    int max_index = resetNodeIndexs(deviceInfo->mTagNodeList);
    return max_index;
}

int DeviceManager::resetCoordIndexs(int projectid)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    int max_index = resetNodeIndexs(deviceInfo->mCoordNodeList);
    return max_index;
}

int DeviceManager::resetLocatorIndexs(int projectid)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    int max_index = resetNodeIndexs(deviceInfo->mLocatorNodeList);
    return max_index;
}

int DeviceManager::resetNodeIndexs(std::list<NodeStruct*> * plist)
{
    int max_index = 0;
    for (std::list<NodeStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
    {
        //printf ("PosCalculate::resetNodeIndexs() (*i)->index[%d] -> max_index[%d]\n", (*i)->index, max_index);
        (*i)->index = max_index;
        max_index++;
    }
    return max_index;
}

// int DeviceManager::reloadConfig()
// {
//     bNeedReloadConfig = true;
//     return 0;
// }

bool DeviceManager::setTagOperationMode(int projectid, const char * Tag_macaddress, int mode)
{
    bool bRet = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(Tag_macaddress, (*existed)->macaddress) == 0 )
        {
            (*existed)->Opertaion_Mode = mode;
            bRet = true;
            break;
        }
    }
    return bRet;
}

bool DeviceManager::setTagOperationModeById(int projectid, const char * TagId, int mode)
{
    bool bRet = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(TagId, (*existed)->nodeid) == 0 )
        {
            (*existed)->Opertaion_Mode = mode;
            bRet = true;
            break;
        }
    }
    return bRet;
}

bool DeviceManager::setTagRangingList(int projectid, const char * Tag_macaddress, const char * rangingList)
{
    bool bRet = false;

    if (strlen(rangingList) <= 0 )
    {
        return bRet;
    }
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(Tag_macaddress, (*existed)->macaddress) == 0 )
        {
            strncpy((*existed)->rangingList, rangingList, 128);
            bRet = true;
            break;
        }
    }
    return bRet;
}

int DeviceManager::getEmptyIndex(std::list<NodeStruct*> * plist)
{
    int index = 0;

    //printf ("PosCalculate::getEmptyIndex()\n");

    for(index = 0; index<255; index++)
    {
        bool bFound = false;
        for (std::list<NodeStruct*>::iterator i = plist->begin(); i != plist->end(); i++)
        {
            if ( (*i)->index == index)
            {
                bFound = true;
                //printf ("PosCalculate::getEmptyIndex() bFound index=%d\n",index);
                break;
            }
        }
        if (!bFound)
            break;
    }
    //printf ("PosCalculate::getEmptyIndex() return index=%d\n",index);
    return index;
}

bool DeviceManager::setCoordPanid(int projectid, const char * macaddress, const char * PANID, const char * version)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    bFound = addNodeInfo(deviceInfo->mCoordNodeList, macaddress, PANID, "", -1, -100, -100, "", -1, NODE_TYPE_COORD, false, 0, 0, 0, version);

    return bFound;
}

bool DeviceManager::setTagIndex(int projectid, const char * macaddress, int idx, const char * version)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    bFound = addNodeInfo(deviceInfo->mTagNodeList, macaddress, "", "", -1, -100, -100, "", idx, NODE_TYPE_TAG, false, 0, 0, 0, version);

    return bFound;
}

bool DeviceManager::setTagInfoPos(int projectid, const char * TagId, double posX, double posY, double posZ)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(TagId, (*existed)->nodeid) == 0 )
        {
            //vilslog::printf("setTagInfoPos() projectid[%d] TagId[%s] [%d,%d,%d]\n", projectid, TagId, (int)posX, (int)posY, (int)posZ);

            updateNodeInfoPos((*existed), posX, posY, posZ);

            double diff_X = (*existed)->posX - (*existed)->prePosX;
            double diff_Y = (*existed)->posY - (*existed)->prePosY;
            double diff_Z = (*existed)->posZ - (*existed)->prePosZ;

            (*existed)->momentumX = (*existed)->momentumX * 0.8 + diff_X * 0.2;
            (*existed)->momentumY = (*existed)->momentumY * 0.8 + diff_Y * 0.2;
            (*existed)->momentumZ = (*existed)->momentumZ * 0.8 + diff_Z * 0.2;

            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setTagInfoS(int projectid, const char * TagId, double posX, double posY, double posZ, int maplayerid)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(TagId, (*existed)->nodeid) == 0 )
        {
            double diff_X = (*existed)->posX - (*existed)->prePosX;
            double diff_Y = (*existed)->posY - (*existed)->prePosY;
            double diff_Z = (*existed)->posZ - (*existed)->prePosZ;

            updateNodeInfoPos((*existed), posX, posY, posZ);

            (*existed)->momentumX = (*existed)->momentumX * 0.8 + diff_X * 0.2;
            (*existed)->momentumY = (*existed)->momentumY * 0.8 + diff_Y * 0.2;
            (*existed)->momentumZ = (*existed)->momentumZ * 0.8 + diff_Z * 0.2;
            if (maplayerid >= 0)
                (*existed)->maplayer = maplayerid;


            // vilslog::printf("DeviceManager::setTagInfoS() setTagInfoS TagId[%s] posX[%d] posY[%d] buildingFloor[%d]\n",
            //    (*existed)->nodeid, (int)(*existed)->posX, (int)(*existed)->posY, buildingFloor);


            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setTagInfoB(int projectid, int tafid, double lng, double lat, int EPSG, int buildingID, int buildingFloor)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        // check Tag_idx
        if ( (*existed)->tafid == tafid )
        {
            (*existed)->buildingid = buildingID;
            (*existed)->buildingfloor = buildingFloor;
            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setTagInfoIP(int projectid, const char * Tag_macaddress, const char * IP)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(Tag_macaddress, (*existed)->macaddress) == 0 )
        {
            if (strlen(IP) > 0)
                strncpy((*existed)->IP, IP, 32);

            bFound = true;

            break;
        }
    }
    return bFound;
}

bool DeviceManager::setTagInfoPANID(int projectid, const char * Tag_macaddress, const char * PANID)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(Tag_macaddress, (*existed)->macaddress) == 0 )
        {
            if (strlen(PANID) > 0 )
                sprintf((*existed)->panid, "%s", remove_nonASCII(PANID).c_str());
            bFound = true;

            break;
        }
    }
    return bFound;
}

bool DeviceManager::setTagInfoMapLayer(int projectid, const char * Tag_macaddress, int maplayerid)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(Tag_macaddress, (*existed)->macaddress) == 0 )
        {
            if (maplayerid >= 0)
                (*existed)->maplayer = maplayerid;

            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setTagInfoMapLayerById(int projectid, const char * TagId, int maplayerid)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(TagId, (*existed)->nodeid) == 0 )
        {
            if (maplayerid >= 0)
                (*existed)->maplayer = maplayerid;

            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setTagHeartBeat(int projectid, const char * Tag_macaddress, int HeartBeat)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(Tag_macaddress, (*existed)->macaddress) == 0 )
        {
            (*existed)->heartbeat = HeartBeat;
            (*existed)->updateHeartBeatTimestamp = getCurrentTimeInSec();

            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setTagWeight(int projectid, const char * Tag_macaddress, int Weight)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(Tag_macaddress, (*existed)->macaddress) == 0 )
        {
            (*existed)->weight = Weight;
            (*existed)->updateWeightTimestamp = getCurrentTimeInSec();

            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setTagInfoRangingTarget(int projectid, const char * Tag_macaddress, const char * TargetID)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(Tag_macaddress, (*existed)->macaddress) == 0 )
        {
            (*existed)->rangingTargetid = TargetID;

            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setTagInfoGatewayID(int projectid, const char * Tag_macaddress, const char * GatewayID)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(Tag_macaddress, (*existed)->macaddress) == 0 )
        {
            (*existed)->gwid = GatewayID;

            bFound = true;

            break;
        }
    }
    return bFound;
}

bool DeviceManager::setTagInfoRangingLoRAmac(int projectid, const char * Tag_macaddress, const char* loramacaddress)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(Tag_macaddress, (*existed)->macaddress) == 0 )
        {
            strncpy((*existed)->LoRAMacaddress, loramacaddress, 32);
            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setTagInfoArea(int projectid, const char * TagId, const char * areaid, const char * areaname)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(TagId, (*existed)->nodeid) == 0 )
        {
            // vilslog::printf("DeviceManager::setTagInfoArea() TagId[%s] areaid[%s] areaname[%s]\n",  TagId, areaid, areaname);

            strncpy((*existed)->AreaId, areaid, 32);
            strncpy((*existed)->AreaName, areaname, 256);
            if (strlen(areaid) == 0)
                (*existed)->area_unknown = 1; // 1:unknown area
            else
                (*existed)->area_unknown = 0;// 0:known area
            bFound = true;
            break;
        }
    }
    return bFound;
}

// bool DeviceManager::setTagInfoArea(int projectid, const char * TagId, const char * areaid, const char * areaname, int AreaType0D, int mapAreaType, const char * insideAreaId)
// {
//     bool bFound = false;
//     DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
//
//     for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
//     {
//         if ( strcmp(TagId, (*existed)->nodeid) == 0 )
//         {
//             //
//             // check time stamp
//             //
// //             time_t currentTimestamp = getCurrentTimeInMilliSec();
// //             time_t diffTime = currentTimestamp - (*existed)->area_checkTimestamp;
// //             if ( (int)diffTime <= 3000 && (int)diffTime > 0)
// //             {
// // // #ifdef PRINT_AREA_INOUT_MSG
// // //                 vilslog::printf("DeviceManager::setTagInfoArea() TagId[%s] diffTime[%d]  <= 3000\n", TagId, (int)diffTime);
// // // #endif
// //                 break;
// //             }
// //             (*existed)->area_checkTimestamp = getCurrentTimeInMilliSec();
//
// // #ifdef PRINT_AREA_INOUT_MSG
// //            vilslog::printf("DeviceManager::setTagInfoArea() TagId[%s] areaid[%s] areaname[%s] AreaType[%d] AreaId[%s] AreaName[%s]\n",
// //                            TagId, areaid, areaname, AreaType, (*existed)->AreaId, (*existed)->AreaName);
// // #endif
//
//             if (strlen(areaid) == 0 && strlen((*existed)->AreaId) > 0)
//             {
//                 strncpy((*existed)->area_AreaId, (*existed)->AreaId, 32);
//                 strncpy((*existed)->area_AreaName, (*existed)->AreaName, 256);
//                 (*existed)->area_changed = 2;// 2:out
//
// #ifdef PRINT_AREA_INOUT_MSG
//            vilslog::printf("DeviceManager::setTagInfoArea() TagId[%s] areaid[%s] areaname[%s] AreaType0D[%d] AreaId[%s] AreaName[%s]\n",
//                             TagId, areaid, areaname, AreaType0D, (*existed)->AreaId, (*existed)->AreaName);
//            vilslog::printf("DeviceManager::setTagInfoArea() TagId[%s] AreaId[%s] OUT\n", TagId, (*existed)->AreaId);
// #endif
//
//                 //
//                 // area change
//                 //
//                 set_tag_area_change(projectid, (*existed));
//
//             }
//             else
//             {
//                 if ( strcmp(areaid, (*existed)->AreaId) == 0 )
//                 {
//                     // same area
// // #ifdef PRINT_AREA_INOUT_MSG
// //            vilslog::printf("DeviceManager::setTagInfoArea() TagId[%s] Same Area\n", TagId);
// // #endif
//                 }
//                 else
//                 if ( strcmp(areaid, (*existed)->AreaType5Id) == 0 )
//                 {
//                     // 目前 在病房內, 不進入病房,
//                     // 可 離開目前區域
//                     strncpy((*existed)->area_AreaId, (*existed)->AreaId, 32);
//                     strncpy((*existed)->area_AreaName, (*existed)->AreaName, 256);
//                     (*existed)->area_changed = 2;// 2:out
//
//                     //
//                     // area change
//                     //
//                     set_tag_area_change(projectid, (*existed));
//                 }
//                 else
//                 {
//
// #ifdef PRINT_AREA_INOUT_MSG
//             vilslog::printf("DeviceManager::setTagInfoArea() TagId[%s] areaid[%s] areaname[%s] AreaType[%d] AreaId[%s] AreaName[%s]\n",
//                              TagId, areaid, areaname, AreaType0D, (*existed)->AreaId, (*existed)->AreaName);
// #endif
//
//                     //
//                     // make area_changed = out, first
//                     //
//                     if ( strcmp(insideAreaId, (*existed)->AreaId) == 0)
//                     {
//                         // 同樣還在目前的區域內, 但是不讓他顯示離開區域.
//                         // vilslog::printf("DeviceManager::setTagInfoArea() inside Area TagId[%s] areaid[%s] areaname[%s] insideAreaId[%s] AreaId[%s] AreaName[%s]\n",
//                         //                  TagId, areaid, areaname, insideAreaId, (*existed)->AreaId, (*existed)->AreaName);
//                     }
//                     else
//                     {
//                         if ( mapAreaType == 5 )
//                         {
//                             // 進入 別的病房, 可以離開 目前區域.
//                             strncpy((*existed)->area_AreaId, (*existed)->AreaId, 32);
//                             strncpy((*existed)->area_AreaName, (*existed)->AreaName, 256);
//                             (*existed)->area_changed = 2;// 2:out
//
//                             //
//                             // area change
//                             //
//                             set_tag_area_change(projectid, (*existed));
//
//     #ifdef PRINT_AREA_INOUT_MSG
//                vilslog::printf("DeviceManager::setTagInfoArea() TagId[%s] AreaId[%s] OUT\n", TagId, (*existed)->AreaId);
//     #endif
//
//                         }
//                         else
//                         {
//                             if ( strlen((*existed)->AreaType5Id) > 0 )
//                             {
//                                 // 目前在病房內 不離開病房
//                             }
//                             else
//                             {
//                                 strncpy((*existed)->area_AreaId, (*existed)->AreaId, 32);
//                                 strncpy((*existed)->area_AreaName, (*existed)->AreaName, 256);
//                                 (*existed)->area_changed = 2;// 2:out
//
//                                 //
//                                 // area change
//                                 //
//                                 set_tag_area_change(projectid, (*existed));
//                             }
//                         }
//                     }
//
//                     strncpy((*existed)->area_AreaId, areaid, 32);
//                     strncpy((*existed)->area_AreaName, areaname, 256);
//                     (*existed)->area_changed = 1;// 1:in
//                     (*existed)->area_type = mapAreaType;
//                     (*existed)->AreaType0D = AreaType0D; // 0D using 0:normal 1:room 2:danger
//
//                     //
//                     // area change
//                     //
//                     set_tag_area_change(projectid, (*existed));
//
// #ifdef PRINT_AREA_INOUT_MSG
//        vilslog::printf("DeviceManager::setTagInfoArea() TagId[%s] areaid[%s] IN\n", TagId, areaid);
// #endif
//
//                 }
//             }
//
//                // vilslog::printf("DeviceManager::setTagInfoArea() TagId[%s] areaid[%s] area_changed[%d]\n",
//                //                 TagId, areaid, (*existed)->area_changed);
//
//
//             strncpy((*existed)->AreaId, areaid, 32);
//             strncpy((*existed)->AreaName, areaname, 256);
//             (*existed)->AreaType0D = AreaType0D; // 0D using 0:normal 1:room 2:danger
//             (*existed)->area_unknown = 0; // 1:unknown area
//
//             if ( mapAreaType == 5 ) // 5:病房
//             {
//                 strncpy((*existed)->AreaType5Id, areaid, 32);
//                 strncpy((*existed)->AreaType5Name, areaname, 256);
//             }
//             // else
//             // {
//             //     memset((*existed)->AreaType5Id, 0, 32);
//             //     memset((*existed)->AreaType5Name, 0, 256);
//             // }
//
//             bFound = true;
//
//             break;
//         }
//     }
//     return bFound;
// }

// bool DeviceManager::setTagInfoArea(int projectid, NodeStruct * node, AreaStruct * area, const char * insideAreaId)
// {
//     bool bFound = false;
//     std::string areaid = "";
//     std::string areaname = "";
//     int area_type = 0;
//
//     if ( area != NULL )
//     {
//         areaid = area->areaid;
//         areaname = area->areaname;
//         area_type = area->type;
//     }
//
//     if ( (areaid.length() == 0) && strlen(node->AreaId) > 0)
//     {
//         strncpy(node->area_AreaId, node->AreaId, 32);
//         strncpy(node->area_AreaName, node->AreaName, 256);
//         node->area_changed = 2;// 2:out
//
// #ifdef PRINT_AREA_INOUT_MSG
//            vilslog::printf("DeviceManager::setTagInfoArea() TagId[%s] areaid[%s] areaname[%s] AreaId[%s] AreaName[%s]\n",
//                             node->nodeid, areaid.c_str(), areaname.c_str(), node->AreaId, node->AreaName);
//            vilslog::printf("DeviceManager::setTagInfoArea() TagId[%s] AreaId[%s] AreaName[%s] OUT\n", node->nodeid, node->AreaId, node->AreaName);
// #endif
//
//         //
//         // area change
//         //
//         set_tag_area_change(projectid, node);
//
//     }
//     else
//     {
//         if ( areaid.compare(node->AreaId) == 0 )
//         {
//             // same area
// // #ifdef PRINT_AREA_INOUT_MSG
// //            vilslog::printf("DeviceManager::setTagInfoArea() TagId[%s] Same Area\n", TagId);
// // #endif
//         }
//         else
//         {
//
// #ifdef PRINT_AREA_INOUT_MSG
//     vilslog::printf("DeviceManager::setTagInfoArea() TagId[%s] areaid[%s] areaname[%s] AreaId[%s] AreaName[%s]\n",
//                      node->nodeid, areaid.c_str(), areaname.c_str(), node->AreaId, node->AreaName);
// #endif
//
//             //
//             // make area_changed = out, first
//             //
//             if ( strcmp(insideAreaId,node->AreaId) == 0)
//             {
//                 // 同樣還在目前的區域內, 但是不讓他顯示離開區域.
//                 // vilslog::printf("DeviceManager::setTagInfoArea() inside Area TagId[%s] areaid[%s] areaname[%s] insideAreaId[%s] AreaId[%s] AreaName[%s]\n",
//                 //                  node->nodeid, areaid.c_str(), areaname.c_str(), insideAreaId, node->AreaId, node->AreaName);
//             }
//             else
//             {
//                 strncpy(node->area_AreaId, node->AreaId, 32);
//                 strncpy(node->area_AreaName, node->AreaName, 256);
//                 node->area_changed = 2;// 2:out
//                 //
//                 // area change
//                 //
//                 set_tag_area_change(projectid, node);
//
// #ifdef PRINT_AREA_INOUT_MSG
//    vilslog::printf("DeviceManager::setTagInfoArea() TagId[%s] AreaId[%s] AreaName[%s] OUT\n", node->nodeid, node->AreaId, node->AreaName);
// #endif
//             }
//
//             strncpy(node->area_AreaId, areaid.c_str(), 32);
//             strncpy(node->area_AreaName, areaname.c_str(), 256);
//             node->area_changed = 1;// 1:in
//             node->area_type = area_type;
//
//             //
//             // area change
//             //
//             set_tag_area_change(projectid, node);
//
// #ifdef PRINT_AREA_INOUT_MSG
// vilslog::printf("DeviceManager::setTagInfoArea() TagId[%s] areaid[%s] areaname[%s] IN\n", node->nodeid, areaid.c_str(), areaname.c_str());
// #endif
//
//         }
//     }
//
//        // vilslog::printf("DeviceManager::setTagInfoArea() TagId[%s] areaid[%s] area_changed[%d]\n",
//        //                 TagId, areaid, (*existed)->area_changed);
//
//
//     strncpy(node->AreaId, areaid.c_str(), 32);
//     strncpy(node->AreaName, areaname.c_str(), 256);
//     //(*existed)->AreaType = AreaType; // move to setTagInfoAreaType()
//     node->area_unknown = 0; // 1:unknown area
//
//     bFound = true;
//
//     return bFound;
// }

bool DeviceManager::setTagInfoArea0D(int projectid, const char * TagId, const char * areaid, int AreaType0D)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(TagId, (*existed)->nodeid) == 0 )
        {
            strncpy((*existed)->AreaId0D, areaid, 32);
            (*existed)->AreaType0D = AreaType0D; // 0:normal 1:0D room  2:danger area
            bFound = true;
            break;
        }
    }
    bFound = true;

    return bFound;
}

bool DeviceManager::setTagInfoAreaType(int projectid, const char * TagId, int AreaType0D)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(TagId, (*existed)->nodeid) == 0 )
        {
            (*existed)->AreaType0D = AreaType0D; // 0:normal 1:0D room  2:danger area
            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setTagInfoAreaType5(int projectid, const char * TagId, const char * areaid, const char * areaname)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(TagId, (*existed)->nodeid) == 0 )
        {
            strncpy((*existed)->AreaType5Id, areaid, 32);
            strncpy((*existed)->AreaType5Name, areaname, 256);
            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setTagRollCallNotInArea(int projectid, const char * TagId, const char * areaid)
{
    bool bFound = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(TagId, (*existed)->nodeid) == 0 )
        {
            strcpy((*existed)->exclude_rollcall_AreaId, areaid);

            bFound = true;
            break;
        }
    }
    return bFound;
}

// RangingResultType: -1:unknown 0:0D 1:1D 2:2D 3:3D
bool DeviceManager::setTagRangingResultType(int projectid, const char * TagId, int RangingResultType)
{
    bool bFound = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(TagId, (*existed)->nodeid) == 0 )
        {
            (*existed)->RangingResultType = RangingResultType;

            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setTagInfoGPS(int projectid, const char * Tag_macaddress, double Longitude, double Latitude, const char * timestamp)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        // check Tag_idx
        if ( strcmp(Tag_macaddress, (*existed)->macaddress) == 0 )
        {
            setNodeInfoGPS((*existed), Longitude, Latitude, timestamp);

            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setTagInfoActivity(int projectid, const char * Tag_macaddress, std::string activity)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(Tag_macaddress, (*existed)->macaddress) == 0 )
        {
            (*existed)->activity = activity;

            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setTagInfoGPSById(int projectid, const char * TagId, double Longitude, double Latitude)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(TagId, (*existed)->nodeid) == 0 )
        {
            setNodeInfoGPS((*existed), Longitude, Latitude, "");

            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setLocatorInfoGPS(int projectid, const char * loramacaddress, double Longitude, double Latitude, const char * timestamp)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( strcmp(loramacaddress, (*existed)->LoRAMacaddress) == 0 )
        {
            setNodeInfoGPS((*existed), Longitude, Latitude, timestamp);

            bFound = true;
            break;
        }
    }
    return bFound;
}

int DeviceManager::setNodeInfoGPS(NodeStruct * node, double Longitude, double Latitude, const char * timestamp)
{
    // vilslog::printf("DeviceManager::setNodeInfoGPS() nodeid[%s] Longitude[%f] Latitude[%f]\n",
    // node->nodeid, Longitude, Latitude);

    // int Check_Longitude = (int)Longitude;
    // int Check_Latitude = (int)Latitude;

    // check -1
    if ( Longitude < 0.0 || Latitude < 0.0 )
    {
        // vilslog::printf("DeviceManager::setNodeInfoGPS() 1 nodeid[%s] Longitude[%f] Latitude[%f]\n",  node->nodeid, Longitude, Latitude);

        // unknown GPS info
        node->Longitude = Longitude;
        node->Latitude = Latitude;
        // node->area_unknown = 1;
        return 1;
    }
    else
    if ( !isZero(Longitude, 0.0000001) && !isZero(Latitude, 0.0000001) )
    {
        // vilslog::printf("DeviceManager::setNodeInfoGPS() 2\n");

        node->Longitude = Longitude;
        node->Latitude = Latitude;
        node->area_unknown = 0;
        node->updateTimestamp = getCurrentTimeInMilliSec();

        // clear RollCallNotInArea info
        // strcpy(node->exclude_rollcall_AreaId, "");

        // clear position
        node->prePosX = 0.0;
        node->prePosY = 0.0;
        node->prePosZ = 0.0;
        node->posX = 0.0;
        node->posY = 0.0;
        node->posZ = 0.0;

        //
        // convert to position
        //
        //ConvertGPSToPos(node);

        create_NodeGPSInfo(NODE_TYPE_TAG, node, Longitude, Latitude, timestamp);

        return 0;
    }
    else
    if ( isZero(Longitude, 0.0000001) && isZero(Latitude, 0.0000001) )
    {
        // vilslog::printf("DeviceManager::setNodeInfoGPS() 3\n");
        node->area_unknown = 1;
        return 1;
    }

    return 1;
}

bool DeviceManager::setTagSmoothingPos(int projectid, const char * TagId, double posX, double posY, double posZ)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(TagId, (*existed)->nodeid) == 0 )
        {
            (*existed)->smoothingPosX = posX;
            (*existed)->smoothingPosY = posY;
            (*existed)->smoothingPosZ = posZ;
            bFound = true;

            break;
        }
    }
    return bFound;
}

bool DeviceManager::setTagInWall(int projectid, const char * TagId, double posX, double posY, double posZ, MapLayerAreaStruct * area)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        // check Tag_idx
        if ( strcmp(TagId, (*existed)->nodeid) == 0 )
        {
            bool changed = false;
            if ( (*existed)->wallArea == NULL || (*existed)->wallPosX != posX || (*existed)->wallPosY != posY)
            {
                changed = true;
            }
            (*existed)->wallArea = area;
            (*existed)->wallPosX = posX;
            (*existed)->wallPosY = posY;
            (*existed)->wallPosZ = posZ;
            if (changed)
                (*existed)->wallTimeStamp = getCurrentTimeInMilliSec();

            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::checkTagInWallTimeout(int projectid, const char * TagId, int timeout)
{
    bool bTimeout = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(TagId, (*existed)->nodeid) == 0 )
        {
            if ( (*existed)->wallArea != NULL)
            {
                time_t curtime = getCurrentTimeInMilliSec();
                time_t difftime = curtime - (*existed)->wallTimeStamp;

                //printf ("DeviceManager::checkTagInWallTimeout() difftime[%d]\n", (int)difftime);

                if ( (int)difftime > timeout )
                {
                    bTimeout = true;
                }
            }
            break;
        }
    }
    return bTimeout;
}

bool DeviceManager::deleteGatewayByID(int projectid, std::string gatewayid)
{
    bool bFound = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<LoRaGatewayInfoStruct*>::iterator gateway = deviceInfo->mLoRaGatewayList->begin(); gateway != deviceInfo->mLoRaGatewayList->end(); gateway++)
    {
        if ( (*gateway)->gwid.compare(gatewayid) == 0 )
        {
            deviceInfo->mLoRaGatewayList->remove((*gateway));
            delete (*gateway);
            bFound = true;
            break;
        }
    }
    return bFound;
}

//bool DeviceManager::setLoRaGatewayAlive(std::string gatewayid, std::string channel)
bool DeviceManager::setLoRaGatewayAlive(int projectid, LoRaGatewayInfoStruct * curgateway)
{
    bool bFound = false;

    std::string newChannel = "";
    for (std::list<std::string>::iterator channel = curgateway->channels.begin(); channel != curgateway->channels.end(); channel++)
    {
        newChannel = (*channel);
    }

    std::string gatewayid = curgateway->gwid;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<LoRaGatewayInfoStruct*>::iterator gateway = deviceInfo->mLoRaGatewayList->begin(); gateway != deviceInfo->mLoRaGatewayList->end(); gateway++)
    {
        if ( (*gateway)->gwid.compare(gatewayid) == 0 )
        {
            (*gateway)->alive++;
            (*gateway)->aliveTime = getCurrentTimeInSec();

            bool bFoundChannel = false;
            for (std::list<std::string>::iterator curChannel = (*gateway)->channels.begin(); curChannel != (*gateway)->channels.end(); curChannel++)
            {
                if( newChannel.compare((*curChannel)) == 0 )
                {
                    bFoundChannel = true;
                    break;
                }
            }
            if (!bFoundChannel)
            {
                (*gateway)->channels.push_back(newChannel);
            }

            bFound = true;
            break;
        }
    }

    if(!bFound)
    {
        LoRaGatewayInfoStruct * newgateway = new LoRaGatewayInfoStruct();
        newgateway->alive = 1;
        newgateway->MQTTalive = 1;
        newgateway->aliveTime = getCurrentTimeInSec();
        newgateway->pingCount = 0;
        newgateway->sf = curgateway->sf;
        newgateway->projectid = curgateway->projectid;
        newgateway->gwip = curgateway->gwip;
        newgateway->gwid = curgateway->gwid;
        newgateway->repeater = curgateway->repeater;
        newgateway->channels.push_back(newChannel);
        newgateway->DataTime = currentDateTimeForSQL();
        newgateway->maingroupid = curgateway->maingroupid;

        //===================================
        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        newgateway->verifykey = baseDb->create_verifykey("LoRaGatewayInfo");
        baseDb->write_LoRaGatewayInfo(newgateway);
        //===================================

        deviceInfo->mLoRaGatewayList->push_back(newgateway);
    }

    return bFound;
}

bool DeviceManager::setLoRaGatewayMQTTAlive(int projectid, std::string gatewayid)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<LoRaGatewayInfoStruct*>::iterator gateway = deviceInfo->mLoRaGatewayList->begin(); gateway != deviceInfo->mLoRaGatewayList->end(); gateway++)
    {
        if ( (*gateway)->gwid.compare(gatewayid) == 0 )
        {
            (*gateway)->MQTTalive++;
            (*gateway)->aliveTime = getCurrentTimeInSec();
            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setLoRaGatewayIP(int projectid, std::string gatewayid, std::string gwip)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<LoRaGatewayInfoStruct*>::iterator gateway = deviceInfo->mLoRaGatewayList->begin(); gateway != deviceInfo->mLoRaGatewayList->end(); gateway++)
    {
        if ( (*gateway)->gwid.compare(gatewayid) == 0 )
        {
            (*gateway)->gwip = gwip;
            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setLoRaGatewayMapLayer(int projectid, std::string gatewayid, int maplayerid)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<LoRaGatewayInfoStruct*>::iterator gateway = deviceInfo->mLoRaGatewayList->begin(); gateway != deviceInfo->mLoRaGatewayList->end(); gateway++)
    {
        if ( (*gateway)->gwid.compare(gatewayid) == 0 )
        {
            if (maplayerid >= 0)
                (*gateway)->maplayer = maplayerid;
            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setLoRaGatewayMaingroup(int projectid, std::string gatewayid, int maingroupid)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<LoRaGatewayInfoStruct*>::iterator gateway = deviceInfo->mLoRaGatewayList->begin(); gateway != deviceInfo->mLoRaGatewayList->end(); gateway++)
    {
        if ( (*gateway)->gwid.compare(gatewayid) == 0 )
        {
            (*gateway)->maingroupid = maingroupid;
            bFound = true;
            break;
        }
    }
    return bFound;
}

LoRaGatewayInfoStruct * DeviceManager::getLoRaGateway(int projectid, std::string gwid)
{
    LoRaGatewayInfoStruct * gatway = NULL;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<LoRaGatewayInfoStruct*>::iterator gateway = deviceInfo->mLoRaGatewayList->begin(); gateway != deviceInfo->mLoRaGatewayList->end(); gateway++)
    {
        if ( (*gateway)->gwid.compare(gwid) == 0 )
        {
            gatway = (*gateway);
            break;
        }
    }

    return gatway;
}

int DeviceManager::getLoRaGatewayMapLayer(int projectid, std::string gatewayid)
{
    int maplayerid = 0;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<LoRaGatewayInfoStruct*>::iterator gateway = deviceInfo->mLoRaGatewayList->begin(); gateway != deviceInfo->mLoRaGatewayList->end(); gateway++)
    {
        if ( (*gateway)->gwid.compare(gatewayid) == 0 )
        {
            maplayerid = (*gateway)->maplayer;
            break;
        }
    }
    return maplayerid;
}

bool DeviceManager::setAnchorIndex(int projectid, const char * macaddress, int idx, const char * version)
{
    bool bFound = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    bFound = addNodeInfo(deviceInfo->mAnchorNodeList, macaddress, "", "", -1, -100, -100, "", idx, NODE_TYPE_ANCHOR, false, 0, 0, 0, version);

    return bFound;
}

bool DeviceManager::setAnchorInfo(int projectid, const char * macaddress, const char * nodeid, int tafid, int buildingid, int buildingfloor, const char * nodename,
                    int idx, bool updatePos, double posX, double posY, double posZ, const char * version)
{
    bool bFound = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    bFound = addNodeInfo(deviceInfo->mAnchorNodeList, macaddress, "", nodeid, tafid, buildingid, buildingfloor, nodename, idx, NODE_TYPE_ANCHOR, updatePos, posX, posY, posZ, version);

    return bFound;
}

bool DeviceManager::setAnchorInfoS(int projectid, const char * AnchorId, double posX, double posY, double posZ)
{
    bool bFound = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mAnchorNodeList->begin(); existed != deviceInfo->mAnchorNodeList->end(); existed++)
    {
        if ( strcmp(AnchorId, (*existed)->nodeid) == 0 )
        {
            updateNodeInfoPos((*existed), posX, posY, posZ);

            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setAnchorInfoPANID(int projectid, const char * Anchor_macaddress, const char * PANID)
{
    bool bFound = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mAnchorNodeList->begin(); existed != deviceInfo->mAnchorNodeList->end(); existed++)
    {
        if ( strcmp(Anchor_macaddress, (*existed)->macaddress) == 0 )
        {
            if (strlen(PANID) > 0 )
                sprintf((*existed)->panid, "%s", remove_nonASCII(PANID).c_str());

            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setAnchorInfoMapLayer(int projectid, const char * Anchor_macaddress, int maplayerid)
{
    bool bFound = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mAnchorNodeList->begin(); existed != deviceInfo->mAnchorNodeList->end(); existed++)
    {
        if ( strcmp(Anchor_macaddress, (*existed)->macaddress) == 0 )
        {
            if (maplayerid >= 0)
                (*existed)->maplayer = maplayerid;

            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setTagInfo(int projectid, const char * macaddress, const char * nodeid, int tafid, int buildingid, int buildingfloor, const char * nodename,
                    int idx, bool updatePos, double posX, double posY, double posZ, const char * version)
{
    bool bFound = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    bFound = addNodeInfo(deviceInfo->mTagNodeList, macaddress, "", nodeid, tafid, buildingid, buildingfloor, nodename, idx, NODE_TYPE_TAG, updatePos, posX, posY, posZ, version);

    return bFound;
}

bool DeviceManager::setCoordInfo(int projectid, const char * macaddress, const char * nodeid, int tafid, int buildingid, int buildingfloor, const char * nodename,
                    int idx, bool updatePos, double posX, double posY, double posZ, const char * version)
{
    bool bFound = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    bFound = addNodeInfo(deviceInfo->mCoordNodeList, macaddress, "", nodeid, tafid, buildingid, buildingfloor, nodename, idx, NODE_TYPE_COORD, updatePos, posX, posY, posZ, version);

    return bFound;
}

bool DeviceManager::setCoordInfoMapLayer(int projectid, const char * macaddress, int maplayerid)
{
    bool bFound = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mCoordNodeList->begin(); existed != deviceInfo->mCoordNodeList->end(); existed++)
    {
        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            if (maplayerid >= 0)
                (*existed)->maplayer = maplayerid;

            bFound = true;
            break;
        }
    }
    return bFound;
}


bool DeviceManager::setLocatorIndex(int projectid, const char * macaddress, int index, const char * version)
{
    bool bFound = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    bFound = addNodeInfo(deviceInfo->mLocatorNodeList, macaddress, "", "", -1, -100, -100, "", index, NODE_TYPE_LOCATOR, false, 0, 0, 0, version);

    return bFound;
}

bool DeviceManager::setLocatorInfoS(int projectid, const char * LocatorId, double posX, double posY, double posZ)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( strcmp(LocatorId, (*existed)->nodeid) == 0 )
        {
            updateNodeInfoPos((*existed), posX, posY, posZ);

            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setLocatorInfo(int projectid, const char * macaddress, const char * nodeid, int tafid, int buildingid, int buildingfloor, const char * nodename,
                    int idx, bool updatePos, double posX, double posY, double posZ, const char * version)
{
    bool bFound = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    bFound = addNodeInfo(deviceInfo->mLocatorNodeList, macaddress, "", nodeid, tafid, buildingid, buildingfloor, nodename, idx, NODE_TYPE_LOCATOR, updatePos, posX, posY, posZ, version);

    return bFound;
}

bool DeviceManager::setLocatorInfoLoRA(int projectid, const char * macaddress, const char * loramacaddress)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        // check mac address
        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            strcpy((*existed)->LoRAMacaddress, loramacaddress);
            bFound = true;
            break;
        }
    }

    return bFound;
}

bool DeviceManager::setLocatorInfoMacAddress(int projectid, const char * loramacaddress, const char * macaddress)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        // check mac address
        if ( strcmp(loramacaddress, (*existed)->LoRAMacaddress) == 0 )
        {
            strcpy((*existed)->macaddress, macaddress);
            bFound = true;
            //break;
        }
    }

    return bFound;
}

bool DeviceManager::setLocatorInfoMapLayer(int projectid, const char * macaddress, int maplayerid)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        // check mac address
        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            vilslog::printf("DeviceManager::setLocatorInfoMapLayer() macaddress[%s] maplayer[%d] maplayerid[%d]\n",
                macaddress, (*existed)->maplayer, maplayerid);

            if (maplayerid >= 0)
                (*existed)->maplayer = maplayerid;

            bFound = true;
            break;
        }
    }

    return bFound;
}

bool DeviceManager::setLocatorInfoMapLayerByLoRAmacaddress(int projectid, const char * loramacaddress, int maplayerid)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( strcmp(loramacaddress, (*existed)->LoRAMacaddress) == 0 )
        {
            if (maplayerid >= 0)
                (*existed)->maplayer = maplayerid;

            bFound = true;
            break;
        }
    }

    return bFound;
}

bool DeviceManager::setLocatorInfoGatewayID(int projectid, const char * loramacaddress, const char * GatewayID)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( strcmp(loramacaddress, (*existed)->LoRAMacaddress) == 0 )
        {
            if( strlen(GatewayID) != 0 )
                (*existed)->gwid = GatewayID;
            bFound = true;
            break;
        }
    }

    return bFound;
}

bool DeviceManager::setLocatorInfoGatewayRSSI(int projectid, const char * loramacaddress, const char * GatewayID, int rssi)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    if( strlen(GatewayID) == 0 || rssi >= 0)
    {
        return bFound;
    }

    for (std::list<LocatorRSSIInfoStruct*>::iterator existed = deviceInfo->mLocatorRSSIInfoList->begin(); existed != deviceInfo->mLocatorRSSIInfoList->end(); existed++)
    {
        if ( (*existed)->LoRAMacaddress.compare(loramacaddress) == 0 )
        {
            bool bFoundGateway = false;
            for (std::list<GatewayRSSIInfoStruct*>::iterator gateway = (*existed)->gatewayRecords->begin(); gateway != (*existed)->gatewayRecords->end(); gateway++)
            {
                if ( (*gateway)->gwid.compare(GatewayID) == 0 )
                {
                    RSSIInfoStruct * newInfo = new RSSIInfoStruct();
                    newInfo->rssi = rssi;
                    newInfo->datetime = currentDateTimeForSQL();

                    if ((*gateway)->rssiRecords->size() >= 50)
                    {
                        (*gateway)->rssiRecords->pop_front();
                    }
                    (*gateway)->rssiRecords->push_back(newInfo);

                    bFoundGateway = true;
                    break;
                }
            }

            if (!bFoundGateway)
            {
                GatewayRSSIInfoStruct * newGateway = new GatewayRSSIInfoStruct();
                newGateway->gwid = GatewayID;
                newGateway->rssiRecords = new std::list<RSSIInfoStruct*>();

                RSSIInfoStruct * newInfo = new RSSIInfoStruct();
                newInfo->rssi = rssi;
                newInfo->datetime = currentDateTimeForSQL();
                newGateway->rssiRecords->push_back(newInfo);

                (*existed)->gatewayRecords->push_back(newGateway);
            }

            bFound = true;
            break;
        }
    }

    return bFound;
}

bool DeviceManager::setLocatorInfoBoot(int projectid, const char * loramacaddress, const char * PANID, const char * MODE, const char * VERSION)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( strcmp(loramacaddress, (*existed)->LoRAMacaddress) == 0 )
        {
            vilslog::printf("DeviceManager::setLocatorInfoBoot() loramacaddress[%s] PANID[%s] MODE[%s] VERSION[%s]\n",
                                loramacaddress, PANID, MODE, VERSION);

            if (strlen(PANID) > 0 )
                sprintf((*existed)->panid, "%s", remove_nonASCII(PANID).c_str());

            if (strlen(MODE) > 0 )
                (*existed)->Opertaion_Mode = atoi(MODE);

            if (strlen(VERSION) > 0 )
                sprintf((*existed)->version, "%s", VERSION);

            //(*existed)->poweronTimestamp = getCurrentTimeInSec();

            bFound = true;
            break;
        }
    }

    return bFound;
}

bool DeviceManager::setLocatorInfoPowerOff(int projectid, const char * loramacaddress)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( strcmp(loramacaddress, (*existed)->LoRAMacaddress) == 0 )
        {
            vilslog::printf("DeviceManager::setLocatorInfoPowerOff() loramacaddress[%s]\n", loramacaddress);

            (*existed)->poweroffTimestamp = getCurrentTimeInSec();
            (*existed)->alive = 0;

            bFound = true;
            break;
        }
    }

    return bFound;
}

bool DeviceManager::setLocatorInfoPowerOn(int projectid, const char * loramacaddress)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( strcmp(loramacaddress, (*existed)->LoRAMacaddress) == 0 )
        {
            vilslog::printf("DeviceManager::setLocatorInfoPowerOn() loramacaddress[%s]\n", loramacaddress);

            (*existed)->poweronTimestamp = getCurrentTimeInSec();
            bFound = true;
            break;
        }
    }

    return bFound;
}

bool DeviceManager::setLocatorInfoHeartBeat(int projectid, const char * loramacaddress, const char * HeartBeat)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( strcmp(loramacaddress, (*existed)->LoRAMacaddress) == 0 )
        {
            // vilslog::printf("DeviceManager::setLocatorInfoHeartBeat() loramacaddress[%s] HeartBeat[%s]\n",
            //                     loramacaddress, HeartBeat);
            (*existed)->heartbeat = atoi(HeartBeat);
            (*existed)->updateHeartBeatTimestamp = getCurrentTimeInSec();

            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setLocatorInfoWeight(int projectid, const char * loramacaddress, const char * Weight)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( strcmp(loramacaddress, (*existed)->LoRAMacaddress) == 0 )
        {
            // vilslog::printf("DeviceManager::setLocatorInfoWeight() loramacaddress[%s] Weight[%s]\n",
            //                     loramacaddress, Weight);
            (*existed)->weight = (int)(atof(Weight) * 100.0f);
            (*existed)->updateWeightTimestamp = getCurrentTimeInSec();

            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setLocatorInfoAreaType5(int projectid, const char * LocatorId, const char * areaid, const char * areaname)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( strcmp(LocatorId, (*existed)->nodeid) == 0 )
        {
            strncpy((*existed)->AreaType5Id, areaid, 32);
            strncpy((*existed)->AreaType5Name, areaname, 256);
            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setLocatorInfoArea(int projectid, const char * LocatorId, const char * areaid, const char * areaname)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( strcmp(LocatorId, (*existed)->nodeid) == 0 )
        {
            strncpy((*existed)->AreaId, areaid, 32);
            strncpy((*existed)->AreaName, areaname, 256);
            if (strlen(areaid) == 0)
                (*existed)->area_unknown = 1; // 1:unknown area
            else
                (*existed)->area_unknown = 0;// 0:known area
            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setLocatorInfoSensorAlert(int projectid, const char * macaddress, int SensorAlert)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            (*existed)->enableSensorAlert = SensorAlert;
            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::addNodeInfo(std::list<NodeStruct*> * plist, const char * macaddress, const char * PANID,
                    const char * nodeid, int tafid, int buildingid, int buildingfloor, const char * nodename,
                    int idx, int type, bool updatePos, double posX, double posY, double posZ, const char * version)
{
    bool bFound = false;

    //printf ("DeviceManager::addNodeInfo() plist->size[%d]\n", plist->size());

    for (std::list<NodeStruct*>::iterator existed = plist->begin(); existed != plist->end(); existed++)
    {
        // check mac address
        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            if ( idx < 0)
            {
                if ((*existed)->index < 0)
                {
                    (*existed)->index = getEmptyIndex(plist);
                }
                else
                {
                    //keep same index
                }
            }
            else
            {
                if ((*existed)->index < 0)
                {
                    (*existed)->index = idx;
                }
                else
                {
                    //keep same index
                }
            }

            if (strlen(PANID) > 0 )
                sprintf((*existed)->panid, "%s", remove_nonASCII(PANID).c_str());
            if (strlen(nodeid) > 0 )
                sprintf((*existed)->nodeid, "%s", nodeid);
            if (strlen(nodename) > 0 )
                sprintf((*existed)->nodename, "%s", nodename);
            if (strlen(version) > 0 )
                sprintf((*existed)->version, "%s", version);
            if (tafid >= 0)
                (*existed)->tafid = tafid;
            if (buildingid >= 0)
                (*existed)->buildingid = buildingid;
            if (buildingfloor > -100)
                (*existed)->buildingfloor = buildingfloor;

            if (updatePos)
            {
                updateNodeInfoPos((*existed), posX, posY, posZ);
            }
            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        //
        // find new index
        //
        if ( idx < 0)
        {
            idx = getEmptyIndex(plist);
        }

        NodeStruct * newNode = createNodeStruct();
        newNode->index = idx;
        newNode->alive = 0;
        newNode->nodetype = type;// 0:Anchor 1:Tag 2:Coordinator
        strcpy(newNode->panid, "");
        if (strlen(PANID) > 0 )
            sprintf(newNode->panid, "%s", remove_nonASCII(PANID).c_str());

        char defaulePrefix[32];
        memset(defaulePrefix, 0, 32);
        if(type == NODE_TYPE_ANCHOR)
            snprintf(defaulePrefix, 32, "An");
        else
        if(type == NODE_TYPE_TAG)
            snprintf(defaulePrefix, 32, "Tag");
        else
        if(type == NODE_TYPE_COORD)
            snprintf(defaulePrefix, 32, "Coord");
        else
        if(type == NODE_TYPE_LOCATOR)
        {
            char version = macaddress[1];
            if ( version == '7' )
                snprintf(defaulePrefix, 32, "AOA");
            else
                snprintf(defaulePrefix, 32, "Locator");
        }

        //int maclen = (int)strlen(macaddress);

        if (strlen(nodeid) > 0 )
            sprintf(newNode->nodeid, "%s", nodeid);
        else
        {
            getEmptyNodeId(plist, defaulePrefix, macaddress, newNode->nodeid);

            //sprintf(newNode->nodeid, "%s%03d", defaulePrefix, plist->size());
            // sprintf(newNode->nodeid, "%s%c%c%c%c", defaulePrefix, macaddress[maclen-4], macaddress[maclen-3], macaddress[maclen-2], macaddress[maclen-1]);
        }
        if (strlen(nodename) > 0 )
            sprintf(newNode->nodename, "%s", nodename);
        else
        {
            sprintf(newNode->nodename, "%s", newNode->nodeid);
            //sprintf(newNode->nodename, "%s%c%c%c%c", defaulePrefix, macaddress[maclen-4], macaddress[maclen-3], macaddress[maclen-2], macaddress[maclen-1]);
        }
        if (tafid < 0)
            newNode->tafid = 0;
        else
            newNode->tafid = tafid;
        newNode->buildingid = 1;
        newNode->buildingfloor = 1;
        strcpy(newNode->macaddress, macaddress);
        strcpy(newNode->version, version);
        newNode->posX = posX;
        newNode->posY = posY;
        newNode->posZ = posZ;
        strcpy(newNode->voltage, "");
        strcpy(newNode->temperature, "");
        newNode->aliveTime = 0;
        //newNode->waitRangingCount = 5;

        MySQLDBAccess * baseDb = mDbManager->getBaseDB(1);
        if (baseDb != NULL)
        {
            newNode->verifykey = baseDb->create_verifykey("NodeInfo");
            baseDb->write_node_info(newNode);
        }

        // vilslog::printf ("DeviceManager::addNodeInfo() newNode nodeid[%s] nodename[%s] nodetype[%d] mac[%s] index[%d] version[%s]\n",
        //      newNode->nodeid, newNode->nodename, newNode->nodetype, newNode->macaddress, newNode->index, newNode->version);
        //printf ("DeviceManager::addNodeInfo() newNode mac[%s] index[%d] tafid[%d] buildingid[%d] buildingfloor[%d]\n",
        //  newNode->macaddress, newNode->index, newNode->tafid, newNode->buildingid, newNode->buildingfloor);


        plist->push_back(newNode);
    }

    return bFound;
}

bool DeviceManager::updateNodeInfoPos(NodeStruct * node, double posX, double posY, double posZ)
{
    node->prePosX = node->posX;
    node->prePosY = node->posY;
    node->prePosZ = node->posZ;
    node->posX = posX;
    node->posY = posY;
    node->posZ = posZ;
    node->updateTimestamp = getCurrentTimeInMilliSec();

    // vilslog::printf("DeviceManager::updateNodeInfoPos() macaddress[%s] newX[%d] newY[%d]\n",
    //     node->macaddress, (int)posX, (int)posY);

    // clear RollCallNotInArea info
    // strcpy(node->exclude_rollcall_AreaId, "");

    // cleate GPS info
    node->Latitude = -1;
    node->Longitude = -1;

    node->area_unknown = 0;

    create_NodePOSInfo(node, posX, posY, posZ);

    return true;
}

bool DeviceManager::setNodeInfo(std::list<NodeStruct*> * plist, const char * node_id, double posX, double posY, double posZ)
{
    bool bFound = false;
    for (std::list<NodeStruct*>::iterator existed = plist->begin(); existed != plist->end(); existed++)
    {
        // check nodeid
        if ( strcmp(node_id, (*existed)->nodeid) == 0 )
        {

            updateNodeInfoPos((*existed), posX, posY, posZ);

            (*existed)->smoothingPosX = posX;
            (*existed)->smoothingPosY = posY;
            (*existed)->smoothingPosZ = posZ;

            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setNodeInfoByID(int projectid, const char * node_type, const char * node_id, double posX, double posY, double posZ)
{
    bool bFound = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    if ( strcmp(node_type, "anchor") == 0 )
    {
        bFound = setNodeInfo(deviceInfo->mAnchorNodeList, node_id, posX, posY, posZ);
    }
    else
    if ( strcmp(node_type, "tag") == 0 )
    {
        bFound = setNodeInfo(deviceInfo->mTagNodeList, node_id, posX, posY, posZ);
    }
    else
    if ( strcmp(node_type, "coord") == 0 )
    {
        bFound = setNodeInfo(deviceInfo->mCoordNodeList, node_id, posX, posY, posZ);
    }
    else
    if ( strcmp(node_type, "locator") == 0 )
    {
        bFound = setNodeInfo(deviceInfo->mLocatorNodeList, node_id, posX, posY, posZ);

        if (bFound)
        {
            setAoAInfoById(projectid, node_id, posX, posY, posZ);
        }
    }

    return bFound;
}

bool DeviceManager::setNodeAlertByType(int projectid, const char * macaddress, int node_type)
{
    bool bFound = false;

    vilslog::printf("DeviceManager::setNodeAlertByType() projectid[%d] macaddress[%s] node_type[%d]\n", projectid, macaddress, node_type);

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    switch(node_type)
    {
        default:
        break;
        case NODE_TYPE_ANCHOR: // Anchor
            bFound = setNodeAlert(deviceInfo->mAnchorNodeList, macaddress);
        break;
        case NODE_TYPE_TAG: // Tag
            bFound = setNodeAlert(deviceInfo->mTagNodeList, macaddress);
        break;
        case NODE_TYPE_COORD: // Coordinator
            bFound = setNodeAlert(deviceInfo->mCoordNodeList, macaddress);
        break;
        case NODE_TYPE_LOCATOR: // Locator
            bFound = setNodeAlert(deviceInfo->mLocatorNodeList, macaddress);
        break;
    }
    return bFound;
}

bool DeviceManager::clearNodeAlertByType(int projectid, const char * macaddress, int node_type)
{
    bool bFound = false;

    vilslog::printf("DeviceManager::clearNodeAlertByType() projectid[%d] macaddress[%s] node_type[%d]\n", projectid, macaddress, node_type);

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    switch(node_type)
    {
        default:
        break;
        case NODE_TYPE_ANCHOR: // Anchor
            bFound = clearNodeAlert(deviceInfo->mAnchorNodeList, macaddress);
        break;
        case NODE_TYPE_TAG: // Tag
            bFound = clearNodeAlert(deviceInfo->mTagNodeList, macaddress);
        break;
        case NODE_TYPE_COORD: // Coordinator
            bFound = clearNodeAlert(deviceInfo->mCoordNodeList, macaddress);
        break;
        case NODE_TYPE_LOCATOR: // Locator
            bFound = clearNodeAlert(deviceInfo->mLocatorNodeList, macaddress);
        break;
    }
    return bFound;
}

bool DeviceManager::setNodeAlert(std::list<NodeStruct*> * plist, const char * macaddress)
{
    bool bFound = false;
    for (std::list<NodeStruct*>::iterator existed = plist->begin(); existed != plist->end(); existed++)
    {
        //printf("DeviceManager::setNodeAlert() macaddress[%s] (*existed)->macaddress[%s]\n", macaddress, (*existed)->macaddress);

        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            (*existed)->alert++;
            (*existed)->alertIssuTime = getCurrentTimeInMilliSec();
            bFound = true;

            printf("DeviceManager::setNodeAlert() (*existed)->alert[%d]\n", (*existed)->alert);

            break;
        }
    }
    return bFound;
}

bool DeviceManager::clearNodeAlert(std::list<NodeStruct*> * plist, const char * macaddress)
{
    bool bFound = false;
    for (std::list<NodeStruct*>::iterator existed = plist->begin(); existed != plist->end(); existed++)
    {
        // vilslog::printf("DeviceManager::clearNodeAlert() macaddress[%s] (*existed)->macaddress[%s]\n", macaddress, (*existed)->macaddress);

        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            (*existed)->alert = 0;
            (*existed)->alertIssuTime = getCurrentTimeInMilliSec();
            bFound = true;
            // vilslog::printf("DeviceManager::clearNodeAlert() (*existed)->alert[%d]\n", (*existed)->alert);
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setTag0DAlert(int projectid, const char * macaddress, const char* loramacaddress)
{
    bool bFound = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            vilslog::printf("DeviceManager::setTag0DAlert() macaddress[%s] (*existed)->macaddress[%s] loramacaddress[%s]\n",
                macaddress, (*existed)->macaddress, loramacaddress);

            if ( (*existed)->alert0D > 0 )
                (*existed)->alert0D--;
            else
                (*existed)->alert0D = 10;
            (*existed)->alertIssuTime = getCurrentTimeInMilliSec();
            strncpy((*existed)->LoRAMacaddress, loramacaddress, 32);
            bFound = true;
            vilslog::printf("DeviceManager::setTag0DAlert() (*existed)->alert0D[%d] (*existed)->alert[%d]\n",
                (*existed)->alert0D, (*existed)->alert);
            break;
        }
    }
    return bFound;

}

bool DeviceManager::setLocator0DAlert(int projectid, const char * macaddress, const char* loramacaddress)
{
    bool bFound = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        // vilslog::printf("DeviceManager::setLocator0DAlert() macaddress[%s] loramacaddress[%s] (*existed)->macaddress[%s]\n",
        //        macaddress, loramacaddress, (*existed)->macaddress);

        if ( (strcmp(macaddress, (*existed)->macaddress) == 0) || ( strcmp(loramacaddress, (*existed)->LoRAMacaddress) == 0 ))
        {
            if ( (*existed)->alert0D > 0 )
                (*existed)->alert0D--;
            else
                (*existed)->alert0D = 10;

            // vilslog::printf("DeviceManager::setLocator0DAlert() alert0D[%d]\n", (*existed)->alert0D);

            (*existed)->alertIssuTime = getCurrentTimeInMilliSec();
            bFound = true;
            break;
        }
    }
    return bFound;
}

int DeviceManager::getTag0DAlertList(int projectid, const char * loramacaddress, std::list<std::string> * noideidlist)
{
    int count = 0;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        // if ( strcmp(loramacaddress, (*existed)->LoRAMacaddress) == 0 )
        // {
        //     printf("DeviceManager::getTag0DAlertList() nodeid[%s] loramacaddress[%s] (*existed)->LoRAMacaddress[%s] alert0D[%d]\n",
        //         (*existed)->nodeid, loramacaddress, (*existed)->LoRAMacaddress, (*existed)->alert0D );
        // }

        //if ( strcmp(loramacaddress, (*existed)->LoRAMacaddress) == 0 && (*existed)->alert0D > 0)
        if ( strcmp(loramacaddress, (*existed)->LoRAMacaddress) == 0 )
        {
            noideidlist->push_back((*existed)->nodeid);
            count++;
        }
    }
    return count;
}

int DeviceManager::getTagAlertListFromSameIP(int projectid, const char * IPAddress, std::list<std::string> * noideidlist)
{
    int count = 0;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( (strcmp(IPAddress, (*existed)->IP) == 0) && ((*existed)->alert > 0) )
        {
            noideidlist->push_back((*existed)->nodeid);
            count++;
        }
    }
    return count;
}

bool DeviceManager::clearTag0DAlert(int projectid, const char * loramacaddress)
{
    bool bFound = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        //printf("DeviceManager::clearTag0DAlert() macaddress[%s] (*existed)->macaddress[%s]\n", macaddress, (*existed)->macaddress);

        if ( strcmp(loramacaddress, (*existed)->LoRAMacaddress) == 0 )
        {
            printf("DeviceManager::clearTag0DAlert() (*existed)->alert0D[%d] (*existed)->alert[%d]\n", (*existed)->alert0D, (*existed)->alert);

            (*existed)->alert0D = 0;
            (*existed)->alert = 0;
            (*existed)->alertIssuTime = getCurrentTimeInMilliSec();
            //strncpy((*existed)->LoRAMacaddress, "", 32);
            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::clearTagAlertById(int projectid, const char * node_id)
{
    bool bFound = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(node_id, (*existed)->nodeid) == 0 )
        {
            (*existed)->alert0D = 0;
            (*existed)->alert = 0;
            (*existed)->alertIssuTime = getCurrentTimeInMilliSec();
            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::clearLocator0DAlert(int projectid, const char * loramacaddress)
{
    bool bFound = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        //vilslog::printf("DeviceManager::clearLocator0DAlert() loramacaddress[%s] (*existed)->macaddress[%s]\n", loramacaddress, (*existed)->macaddress);

        if ( strcmp(loramacaddress, (*existed)->LoRAMacaddress) == 0 )
        {
            (*existed)->alert0D = 0;
            (*existed)->alertIssuTime = getCurrentTimeInMilliSec();
            bFound = true;
            vilslog::printf("DeviceManager::clearLocator0DAlert() loramacaddress[%s] nodeid[%s] alert[%d]\n", loramacaddress, (*existed)->nodeid, (*existed)->alert);
            //break;
        }
    }
    return bFound;
}

bool DeviceManager::deleteNodeByMAC(int projectid, const char * macaddress, const char * node_type)
{
    bool bFound = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    if ( strcmp(node_type, "anchor") == 0 )
    {
        bFound = deleteNodeInfo(deviceInfo->mAnchorNodeList, macaddress);
    }
    else
    if ( strcmp(node_type, "tag") == 0 )
    {
        bFound = deleteNodeInfo(deviceInfo->mTagNodeList, macaddress);
    }
    else
    if ( strcmp(node_type, "coord") == 0 )
    {
        bFound = deleteNodeInfo(deviceInfo->mCoordNodeList, macaddress);
    }
    else
    if ( strcmp(node_type, "locator") == 0 )
    {
        bFound = deleteNodeInfo(deviceInfo->mLocatorNodeList, macaddress);
    }
    else
    if ( strcmp(node_type, "aoa") == 0 )
    {
        bFound = deleteAoANodeInfo(deviceInfo->mAoANodeList, macaddress);
    }

    return bFound;
}

bool DeviceManager::deleteNodeInfo(std::list<NodeStruct*> * plist, const char * macaddress)
{
    bool bFound = false;

    //printf("PosCalculate::deleteNodeInfo() macaddress[%s]\n", macaddress);
    int deletedIndex = -1;
    for (std::list<NodeStruct*>::iterator existed = plist->begin(); existed != plist->end(); existed++)
    {
        // check nodeid
        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            //printf("DeviceManager::deleteNodeInfo() macaddress[%s] bFound!\n", macaddress);
            deletedIndex = (*existed)->index;
            plist->remove((*existed));
            delete (*existed);
            bFound = true;
            break;
        }
    }

    //
    // re-arrange  index
    //
    if (bFound && deletedIndex >= 0)
    {
        for (std::list<NodeStruct*>::iterator existed = plist->begin(); existed != plist->end(); existed++)
        {
            if ( (*existed)->index > deletedIndex )
            {
                (*existed)->index--;
            }
        }
    }

    return bFound;
}

bool DeviceManager::deleteAoANodeInfo(std::list<AoANodeStruct*> * plist, const char * macaddress)
{
    bool bFound = false;

    for (std::list<AoANodeStruct*>::iterator existed = plist->begin(); existed != plist->end(); existed++)
    {
        // check nodeid
        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            //printf("DeviceManager::deleteNodeInfo() macaddress[%s] bFound!\n", macaddress);
            plist->remove((*existed));
            delete (*existed);
            bFound = true;
            break;
        }
    }

    return bFound;
}

bool DeviceManager::setCoordinatorAlive(int projectid, const char * Coord_macaddress)
{
    bool bRet = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    int i=0;
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mCoordNodeList->begin(); existed != deviceInfo->mCoordNodeList->end(); existed++)
    {
        // check mac address
        if ( strcmp(Coord_macaddress, (*existed)->macaddress) == 0 )
        {
            if ((*existed)->index < 0)
                (*existed)->index = i;
            (*existed)->alive++;

            (*existed)->aliveTime = getCurrentTimeInSec();

            bRet = true;
            //printf ("DeviceManager::setCoordinatorAlive() [%s] alive[%d]\n", Coord_macaddress, (*existed)->alive);

            break;
        }
        i++;
    }
    return bRet;
}

bool DeviceManager::setCoordinatorAliveCount(int projectid, const char * Coord_macaddress, int count)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mCoordNodeList->begin(); existed != deviceInfo->mCoordNodeList->end(); existed++)
    {
        // check mac address
        if ( strcmp(Coord_macaddress, (*existed)->macaddress) == 0 )
        {
            (*existed)->alive = count;
            if ( count > 0 )
            {
                (*existed)->aliveTime = getCurrentTimeInSec();
            }

            //printf ("DeviceManager::setCoordinatorAlive() [%s] alive[%d]\n", Coord_macaddress, (*existed)->alive);
            break;
        }
    }
    return true;
}

bool DeviceManager::setCoordinatorAliveAll(int projectid)
{
    //printf ("PosCalculate::setCoordinatorAliveAll()[%d]\n", mCoordNodeList->size());
    bool bRet = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mCoordNodeList->begin(); existed != deviceInfo->mCoordNodeList->end(); existed++)
    {
        if ( (*existed)->index >= 0)
        {
            (*existed)->alive++;

            (*existed)->aliveTime = getCurrentTimeInSec();
        }
        bRet = true;
    }
    return bRet;
}

bool DeviceManager::setCoordinatorInfo(int projectid, const char * Coord_macaddress, const char * IP, const char * AppVersion, const char * TIME, const char * PANID)
{
    //printf ("PosCalculate::setCoordinatorAliveAll()[%d]\n", mCoordNodeList->size());
    //printf ("DeviceManager::setCoordinatorInfo() Coord_macaddress[%s]\n", Coord_macaddress);

    bool bRet = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mCoordNodeList->begin(); existed != deviceInfo->mCoordNodeList->end(); existed++)
    {
        // check mac address
        if ( strcmp(Coord_macaddress, (*existed)->macaddress) == 0 )
        {
            if (strlen(IP) > 0)
                strncpy((*existed)->IP, IP, 32);
            if (strlen(AppVersion) > 0)
                strncpy((*existed)->AppVersion, AppVersion, 32);
            if (strlen(TIME) > 0)
                strncpy((*existed)->time, TIME, 32);
            if (strlen(PANID) > 0)
                strncpy((*existed)->panid, remove_nonASCII(PANID).c_str(), 32);
            //printf ("DeviceManager::setCoordinatorInfo() IP[%s]\n", (*existed)->IP);
            //printf ("DeviceManager::setCoordinatorInfo() AppVersion[%s]\n", (*existed)->AppVersion);
            //printf ("DeviceManager::setCoordinatorInfo() PID[%s]\n", (*existed)->PID);

            bRet = true;
            break;
        }
    }
    return bRet;
}

bool DeviceManager::setCoordinatorBlacklist(int projectid, const char * Coord_macaddress, const char * list)
{
    bool bRet = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mCoordNodeList->begin(); existed != deviceInfo->mCoordNodeList->end(); existed++)
    {
        // check mac address
        if ( strcmp(Coord_macaddress, (*existed)->macaddress) == 0 )
        {
            (*existed)->blacklist = list;
        }
    }
    return bRet;
}

bool DeviceManager::setCoordinatorWhitelist(int projectid, const char * Coord_macaddress, const char * list)
{
    bool bRet = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mCoordNodeList->begin(); existed != deviceInfo->mCoordNodeList->end(); existed++)
    {
        // check mac address
        if ( strcmp(Coord_macaddress, (*existed)->macaddress) == 0 )
        {
            (*existed)->whitelist = list;
        }
    }
    return bRet;
}

bool DeviceManager::setCoordinatorjoinRangingRange(int projectid, const char * Coord_macaddress, int joinRangingRange)
{
    bool bRet = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mCoordNodeList->begin(); existed != deviceInfo->mCoordNodeList->end(); existed++)
    {
        // check mac address
        if ( strcmp(Coord_macaddress, (*existed)->macaddress) == 0 )
        {
            (*existed)->joinRangingRange = joinRangingRange;
        }
    }
    return bRet;
}

bool DeviceManager::setCoordinatorjoinNotRangingRange(int projectid, const char * Coord_macaddress, int joinNotRangingRange)
{
    bool bRet = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mCoordNodeList->begin(); existed != deviceInfo->mCoordNodeList->end(); existed++)
    {
        // check mac address
        if ( strcmp(Coord_macaddress, (*existed)->macaddress) == 0 )
        {
            (*existed)->joinNotRangingRange = joinNotRangingRange;
            bRet = true;
            break;
        }
    }
    return bRet;
}

bool DeviceManager::setLocatorAlive(int projectid, const char * locator_macaddress)
{
    //printf ("DeviceManager::setLocatorAlive()[%lu]\n", mLocatorNodeList->size());
    bool bRet = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if (strcmp(locator_macaddress, (*existed)->macaddress) == 0)
        {
            if ( (*existed)->alive == 0 )
            {
                vilslog::printf ("DeviceManager::setLocatorAlive() [%s]\n", (*existed)->LoRAMacaddress);
            }

            (*existed)->alive++;

            (*existed)->aliveTime = getCurrentTimeInSec();
            bRet = true;
            break;
        }
    }
    return bRet;
}

bool DeviceManager::setLocatorAliveCount(int projectid, const char * locator_macaddress, int count)
{
    //printf ("DeviceManager::setLocatorAliveCount()[%lu]\n", mLocatorNodeList->size());
    bool bRet = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if (strcmp(locator_macaddress, (*existed)->macaddress) == 0)
        {
            if ( (*existed)->alive == 0 )
            {
                vilslog::printf ("DeviceManager::setLocatorAliveCount() [%s]\n", (*existed)->LoRAMacaddress);
            }

            (*existed)->alive = count;

            (*existed)->aliveTime = getCurrentTimeInSec();
            bRet = true;
            break;
        }
    }
    return bRet;
}

bool DeviceManager::setLocatorAliveByLoRAmacaddress(int projectid, const char * LoRA_macaddress)
{
    bool bRet = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if (strcmp(LoRA_macaddress, (*existed)->LoRAMacaddress) == 0)
        {
            if ( (*existed)->alive == 0 )
            {
                vilslog::printf ("DeviceManager::setLocatorAliveByLoRAmacaddress() [%s]\n", (*existed)->LoRAMacaddress);
            }

            time_t curTime = getCurrentTimeInSec();
            time_t diffTime = curTime - (*existed)->aliveTime;
            if ( (int)diffTime > 1 )
            {
                (*existed)->alive++;
                (*existed)->aliveTime = curTime;
            }
            bRet = true;
            break;
        }
    }
    return bRet;
}

bool DeviceManager::setLocatorResident(int projectid, const char * locator_macaddress, std::string serno)
{
    bool bRet = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if (strcmp(locator_macaddress, (*existed)->macaddress) == 0)
        {
            //
            // change name
            //
            bRet = setNodeResident(projectid, (*existed), serno);
            break;
        }
    }
    return bRet;
}

bool DeviceManager::setLocatorEmployee(int projectid, const char * locator_macaddress, std::string empid)
{
    bool bRet = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if (strcmp(locator_macaddress, (*existed)->macaddress) == 0)
        {
            setNodeEmployee(projectid, (*existed), empid);
            break;
        }
    }
    return bRet;
}

bool DeviceManager::setLocatorButtonType(int projectid, const char * locator_macaddress, std::string buttontype)
{
    bool bRet = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if (strcmp(locator_macaddress, (*existed)->macaddress) == 0)
        {
            (*existed)->tafid = atoi(buttontype.c_str());
            break;
        }
    }
    return bRet;
}

bool DeviceManager::setLocatorUser(int projectid, const char * locator_macaddress, int userid)
{
    bool bRet = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if (strcmp(locator_macaddress, (*existed)->macaddress) == 0)
        {
            (*existed)->userid = userid;
            break;
        }
    }
    return bRet;
}

bool DeviceManager::setAnchorAliveById(int projectid, const char * AnchorId)
{
    bool bRet = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mAnchorNodeList->begin(); existed != deviceInfo->mAnchorNodeList->end(); existed++)
    {
        if (strcmp(AnchorId, (*existed)->nodeid) == 0)
        {
            (*existed)->alive++;

            (*existed)->aliveTime = getCurrentTimeInSec();

            bRet = true;
            //printf ("PosCalculate::setAnchorAlive() [%d] alive[%d]\n", Anchor_idx, (*existed)->alive);
            break;
        }
    }
    return bRet;
}

bool DeviceManager::setAnchorAlive(int projectid, const char * Anchor_macaddress)
{
    bool bRet = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mAnchorNodeList->begin(); existed != deviceInfo->mAnchorNodeList->end(); existed++)
    {
        if (strcmp(Anchor_macaddress, (*existed)->macaddress) == 0)
        {
            (*existed)->alive++;

            (*existed)->aliveTime = getCurrentTimeInSec();
            bRet = true;
            //printf ("PosCalculate::setAnchorAlive() [%d] alive[%d]\n", Anchor_idx, (*existed)->alive);
            break;
        }
    }
    return bRet;
}

bool DeviceManager::setAnchorAliveCount(int projectid, const char * Anchor_macaddress, int count)
{
    bool bRet = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mAnchorNodeList->begin(); existed != deviceInfo->mAnchorNodeList->end(); existed++)
    {
        if (strcmp(Anchor_macaddress, (*existed)->macaddress) == 0)
        {
            (*existed)->alive = count;
            if (count > 0)
            {
                (*existed)->aliveTime = getCurrentTimeInSec();
            }
            bRet = true;
            break;
        }
    }
    return bRet;
}

bool DeviceManager::setTagAliveById(int projectid, const char * TagId)
{
    bool bRet = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if (strcmp(TagId, (*existed)->nodeid) == 0)
        {
            (*existed)->alive++;

            (*existed)->aliveTime = getCurrentTimeInSec();

            bRet = true;
            //printf ("PosCalculate::setTagAlive() [%d] voltage[%s] temperature[%s] alive[%d]\n", Tag_idx, voltage, temperature, (*existed)->alive);
            break;
        }
    }
    return bRet;
}

bool DeviceManager::setTagAlive(int projectid, const char * Tag_macaddress)
{
    bool bRet = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if (strcmp(Tag_macaddress, (*existed)->macaddress) == 0)
        {
            (*existed)->alive++;

            (*existed)->aliveTime = getCurrentTimeInSec();

            bRet = true;
            //printf ("PosCalculate::setTagAlive() [%d] voltage[%s] temperature[%s] alive[%d]\n", Tag_idx, voltage, temperature, (*existed)->alive);
            break;
        }
    }
    return bRet;
}

bool DeviceManager::setTagAlive(int projectid, const char * Tag_macaddress, const char * voltage, const char * temperature)
{
    bool bRet = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if (strcmp(Tag_macaddress, (*existed)->macaddress) == 0)
        {
            (*existed)->alive++;
            if ( voltage != NULL )
            {
                if((strcmp(voltage, "0.0") != 0) && (strcmp(voltage, "2.9") != 0))
                {
                    if ( strcmp((*existed)->voltage, voltage) != 0)
                    {
                        (*existed)->voltage_changed = 1;
                    }

                    if ( strcmp(voltage, deviceInfo->mSysConfig->TagAlertVoltageLow.c_str()) < 0)
                    {
                        // vilslog::printf ("DeviceManager::setTagAlive() [%s] voltage[%s] temperature[%s] alive[%d] voltage_low[1] TagAlertVoltageLow[%s]\n",
                        //     Tag_macaddress, voltage, temperature, (*existed)->alive, deviceInfo->mSysConfig->TagAlertVoltageLow.c_str());
                        // 電壓 比之前降低才會發出低電量通知.
                        if ( strcmp((*existed)->voltage, voltage) < 0)
                        {
                            (*existed)->voltage_low = 1;
                        }
                    }
                    else
                    {
                        (*existed)->voltage_low = 0;
                    }
                    strcpy((*existed)->voltage, voltage);
                }
            }
            if ( temperature != NULL )
                strcpy((*existed)->temperature, temperature);

            (*existed)->aliveTime = getCurrentTimeInSec();

            //printf ("PosCalculate::setTagAlive() [%d] voltage[%s] temperature[%s] alive[%d]\n", Tag_idx, voltage, temperature, (*existed)->alive);
            bRet = true;
            break;
        }
    }
    return bRet;
}

bool DeviceManager::setTagAliveCount(int projectid, const char * Tag_macaddress, int count)
{
    bool bRet = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if (strcmp(Tag_macaddress, (*existed)->macaddress) == 0)
        {
            (*existed)->alive = count;

            if (count > 0)
            {
                (*existed)->aliveTime = getCurrentTimeInSec();
            }

            break;
        }
    }
    return bRet;
}

bool DeviceManager::setTagVoltage(int projectid, const char * Tag_macaddress, const char * voltage)
{
    bool bRet = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if (strcmp(Tag_macaddress, (*existed)->macaddress) == 0)
        {
            if (strlen(voltage) > 0)
            {
                if((strcmp(voltage, "0.0") != 0) && (strcmp(voltage, "2.9") != 0))
                {
                    if ( strcmp((*existed)->voltage, voltage) != 0)
                    {
                        (*existed)->voltage_changed = 1;
                    }

                    if ( strcmp(voltage, deviceInfo->mSysConfig->TagAlertVoltageLow.c_str()) < 0)
                    {
                        // vilslog::printf ("DeviceManager::setTagVoltage() [%s] voltage[%s] alive[%d] voltage_low[1] TagAlertVoltageLow[%s]\n",
                        //     Tag_macaddress, voltage, (*existed)->alive, deviceInfo->mSysConfig->TagAlertVoltageLow.c_str());
                        (*existed)->voltage_low = 1;
                    }
                    else
                    {
                        (*existed)->voltage_low = 0;
                    }

                    strcpy((*existed)->voltage, voltage);
                    (*existed)->aliveTime = getCurrentTimeInSec();
                }
            }

            bRet = true;
            break;
        }
    }
    return bRet;
}

bool DeviceManager::checkTagVoltageLow(std::list<NodeStruct*> * plist, std::string TagAlertVoltageLow)
{
    for (std::list<NodeStruct*>::iterator existed = plist->begin(); existed != plist->end(); existed++)
    {
        //vilslog::printf("checkTagVoltageLow() nodeid[%s] voltage[%s] TagAlertVoltageLow[%s]\n", (*existed)->nodeid, (*existed)->voltage, TagAlertVoltageLow.c_str());

        if ( strcmp((*existed)->voltage, TagAlertVoltageLow.c_str()) < 0)
        {
            (*existed)->voltage_low = 1;
        }
        else
        {
            (*existed)->voltage_low = 0;
        }
    }
    return true;
}

bool DeviceManager::setTagResident(int projectid, const char * Tag_macaddress, std::string serno)
{
    bool bRet = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if (strcmp(Tag_macaddress, (*existed)->macaddress) == 0)
        {
            //
            // change name
            //
            bRet = setNodeResident(projectid, (*existed), serno);
            break;
        }
    }
    return bRet;
}

bool DeviceManager::setNodeResident(int projectid, NodeStruct * node, std::string serno)
{
    if ( serno.compare("-1") == 0 )
    {
        sprintf(node->nodename, "%s", node->nodeid);

        // change poistion
        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if (baseDb != NULL)
        {
            int unassign_count = baseDb->getUnassignResidentCount(node->nodetype);

            double newX = UNASSIGN_RESIDENT_START_X + unassign_count * 200;
            double newY = UNASSIGN_RESIDENT_START_Y + node->nodetype * 200;

            // vilslog::printf("DeviceManager::setNodeResident() serno[%s] unassign_count[%d] newX[%d] newY[%d]\n",
            //     serno.c_str(), unassign_count, (int)newX, (int)newY);

            updateNodeInfoPos(node, newX, newY, 0.0);

            node->smoothingPosX = newX;
            node->smoothingPosY = newY;
            node->smoothingPosZ = 0.0;
        }
    }
    else
    {
        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if (baseDb != NULL)
        {
            UserResidentStruct * resident = baseDb->getResident(serno.c_str());

            //vilslog::printf("DeviceManager::setTagResident() serno[%s]\n", serno.c_str());

            std::string nodename = resident->name;
            nodename.append(" ");
            nodename.append(resident->roomNo);

            delete resident;

            sprintf(node->nodename, "%s", nodename.c_str());
        }
    }

    //vilslog::printf("DeviceManager::setTagResident() nodename[%s]\n", nodename.c_str());

    node->resident_serno = serno;

    return true;
}

bool DeviceManager::setTagEmployee(int projectid, const char * Tag_macaddress, std::string empid)
{
    bool bRet = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if (strcmp(Tag_macaddress, (*existed)->macaddress) == 0)
        {
            setNodeEmployee(projectid, (*existed), empid);
            break;
        }
    }
    return bRet;
}

bool DeviceManager::setNodeEmployee(int projectid, NodeStruct * node, std::string empid)
{
    if ( empid.compare("-1") == 0 )
    {
        sprintf(node->nodename, "%s", node->nodeid);
    }
    else
    {
        MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
        if (baseDb != NULL)
        {
            UserEmployeeStruct * employee = baseDb->getEmployee(empid);

            std::string nodename = employee->cname;
            nodename.append(" ");
            nodename.append(employee->jobTitle);

            delete employee;

            sprintf(node->nodename, "%s", nodename.c_str());
        }
    }
    node->empid = empid;
    return true;
}

bool DeviceManager::setTagUser(int projectid, const char * Tag_macaddress, int userid)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if (strcmp(Tag_macaddress, (*existed)->macaddress) == 0)
        {
            (*existed)->userid = userid;
            break;
        }
    }
    return true;
}

bool DeviceManager::clearTagUser(int projectid, int userid)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ((*existed)->userid == userid)
        {
            (*existed)->userid = 0;
        }
    }
    return true;
}

bool DeviceManager::setTagVelocity(int projectid, const char * Tag_macaddress, double velocity)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if (strcmp(Tag_macaddress, (*existed)->macaddress) == 0)
        {
            (*existed)->velocity = velocity;
            break;
        }
    }
    return true;
}

bool DeviceManager::setTagVelocityById(int projectid, const char * TagId, double velocity)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if (strcmp(TagId, (*existed)->nodeid) == 0)
        {
            (*existed)->velocity = velocity;
            break;
        }
    }
    return true;
}

bool DeviceManager::setTagSleep(int projectid, const char * TagId, int sleep)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if (strcmp(TagId, (*existed)->nodeid) == 0)
        {
            (*existed)->asleep = sleep;
            break;
        }
    }
    return true;
}

bool DeviceManager::checkAnchorValidNode(int projectid, const char * Anchor_macaddress, int index)
{
    bool valid = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    valid = checkValidNode(deviceInfo->mAnchorNodeList, Anchor_macaddress, index);
    return valid;
}

bool DeviceManager::checkTagValidNode(int projectid, const char * Tag_macaddress, int index)
{
    bool valid = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    valid = checkValidNode(deviceInfo->mTagNodeList, Tag_macaddress, index);
    return valid;
}

bool DeviceManager::checkCoordValidNode(int projectid, const char * Coord_macaddress, int index)
{
    bool valid = true;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    valid = checkValidNode(deviceInfo->mCoordNodeList, Coord_macaddress, index);
    return valid;
}

bool DeviceManager::checkLocatorValidNode(int projectid, const char * Locator_macaddress, int index)
{
    bool valid = true;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    valid = checkValidNode(deviceInfo->mLocatorNodeList, Locator_macaddress, index);
    return valid;
}

bool DeviceManager::checkValidNode(std::list<NodeStruct*> * plist, const char * macaddress, int index)
{
    bool valid = false;

    if (index < 0)
        return true;

    bool bFoundSameIndex = false;
    for (std::list<NodeStruct*>::iterator existed = plist->begin(); existed != plist->end(); existed++)
    {
        if ( (*existed)->index == index)
        {
            bFoundSameIndex = true;
            if ( strcmp((*existed)->macaddress, macaddress) == 0)
            {
                valid = true;
            }
        }
    }

    if (!bFoundSameIndex)
    {
        return false;// maybe we just delete from center, and we do not need this device.
        //return true;
    }

    return valid;
}

bool DeviceManager::checkNodeExisted(int projectid, int node_type, const char * macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    bool bExisted = false;

    switch(node_type)
    {
        default:
        break;
        case NODE_TYPE_ANCHOR: // Anchor
            bExisted = checkNodeExisted(deviceInfo->mAnchorNodeList, macaddress);
        break;
        case NODE_TYPE_TAG: // Tag
            bExisted = checkNodeExisted(deviceInfo->mTagNodeList, macaddress);
        break;
        case NODE_TYPE_COORD: // Coordinator
            bExisted = checkNodeExisted(deviceInfo->mCoordNodeList, macaddress);
        break;
        case NODE_TYPE_LOCATOR: // Locator
            bExisted = checkNodeExisted(deviceInfo->mLocatorNodeList, macaddress);
        break;
    }

    return bExisted;
}

bool DeviceManager::checkNodeExisted(std::list<NodeStruct*> * plist, const char * macaddress)
{
    bool bExisted = false;

    for (std::list<NodeStruct*>::iterator existed = plist->begin(); existed != plist->end(); existed++)
    {
        if ( strcmp((*existed)->macaddress, macaddress) == 0)
        {
            bExisted = true;
            break;
        }
    }

    return bExisted;
}

bool DeviceManager::checkNFCLocatorMainGroup(int projectid, std::list<std::string> * maingroups)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    bool bExisted = false;

    for (std::list<NFCDeviceStruct*>::iterator existed = deviceInfo->mNFCDeviceList->begin(); existed != deviceInfo->mNFCDeviceList->end(); existed++)
    {
        for (std::list<std::string>::iterator maingroupid = maingroups->begin(); maingroupid != maingroups->end(); maingroupid++)
        {
            int int_maingroupid = convert_string_to_int((*maingroupid));
            if ( (*existed)->maingroupid == int_maingroupid )
            {
                bExisted = true;
                break;
            }
        }
        if (bExisted)
        {
            break;
        }
    }

    return bExisted;
}

bool DeviceManager::checkSensorDeviceMainGroup(int projectid, std::list<std::string> * maingroups)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    bool bExisted = false;

    for (std::list<SensorDeviceStruct*>::iterator existed = deviceInfo->mSensorDeviceList->begin(); existed != deviceInfo->mSensorDeviceList->end(); existed++)
    {
        for (std::list<std::string>::iterator maingroupid = maingroups->begin(); maingroupid != maingroups->end(); maingroupid++)
        {
            int int_maingroupid = convert_string_to_int((*maingroupid));
            if ( (*existed)->maingroupid == int_maingroupid )
            {
                bExisted = true;
                break;
            }
        }
        if (bExisted)
        {
            break;
        }
    }

    return bExisted;
}

bool DeviceManager::setNodeInfoIP(int projectid, int node_type, const char * macaddress, const char * IP)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    bool bExisted = false;

    switch(node_type)
    {
        default:
        break;
        case NODE_TYPE_ANCHOR: // Anchor
            bExisted = setNodeInfoIP(deviceInfo->mAnchorNodeList, macaddress, IP);
        break;
        case NODE_TYPE_TAG: // Tag
            bExisted = setNodeInfoIP(deviceInfo->mTagNodeList, macaddress, IP);
        break;
        case NODE_TYPE_COORD: // Coordinator
            bExisted = setNodeInfoIP(deviceInfo->mCoordNodeList, macaddress, IP);
        break;
        case NODE_TYPE_LOCATOR: // Locator
            bExisted = setNodeInfoIP(deviceInfo->mLocatorNodeList, macaddress, IP);
        break;
    }

    return bExisted;
}

bool DeviceManager::setNodeInfoIP(std::list<NodeStruct*> * plist, const char * macaddress, const char * IP)
{
    bool bExisted = false;

    for (std::list<NodeStruct*>::iterator existed = plist->begin(); existed != plist->end(); existed++)
    {
        if ( strcmp((*existed)->macaddress, macaddress) == 0)
        {
            snprintf((*existed)->IP, 32, "%s", IP);
            bExisted = true;
            break;
        }
    }

    return bExisted;
}

int DeviceManager::getAnchorIndex(int projectid, const char * macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    int Anchor_idx = getNodeIndex(deviceInfo->mAnchorNodeList, macaddress);
    return Anchor_idx;
}

int DeviceManager::getAnchorIndexFromId(int projectid, const char * anchorid)
{
    int Anchor_idx = -1;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mAnchorNodeList->begin(); existed != deviceInfo->mAnchorNodeList->end(); existed++)
    {
        if ( strcmp((*existed)->nodeid, anchorid) == 0 )
        {
            return (*existed)->index;
        }
    }
    return Anchor_idx;
}

int DeviceManager::getTagIndex(int projectid, const char * macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    int Tag_idx = getNodeIndex(deviceInfo->mTagNodeList, macaddress);
    return Tag_idx;
}

int DeviceManager::getCoordIndex(int projectid, const char * macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    int Coord_idx = getNodeIndex(deviceInfo->mCoordNodeList, macaddress);
    return Coord_idx;
}

int DeviceManager::getLocatorIndex(int projectid, const char * macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    int Locator_idx = getNodeIndex(deviceInfo->mLocatorNodeList, macaddress);
    return Locator_idx;
}

char * DeviceManager::getTagId(int projectid, int Tag_idx)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( (*existed)->index == Tag_idx )
        {
            return (*existed)->nodeid;
        }
    }
    return (char *)"";
}

char * DeviceManager::getTagId(int projectid, const char * macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            return (*existed)->nodeid;
        }
    }
    return (char *)"";
}

char * DeviceManager::getTagIP(int projectid, const char * macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            return (*existed)->IP;
        }
    }
    return (char *)"";
}

std::string DeviceManager::getTagempid(int projectid, const char * macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            return (*existed)->empid;
        }
    }
    return "";
}

int DeviceManager::getTaguserid(int projectid, const char * macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            return (*existed)->userid;
        }
    }
    return -1;
}

char * DeviceManager::getTagName(int projectid, const char * tagid)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(tagid, (*existed)->nodeid) == 0 )
        {
            return (*existed)->nodename;
        }
    }
    return (char *)"";
}

char * DeviceManager::getTagMacaddress(int projectid, const char * tagid)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(tagid, (*existed)->nodeid) == 0 )
        {
            return (*existed)->macaddress;
        }
    }
    return (char *)"";
}

// int DeviceManager::getTagFloor(int projectid, const char * tagid)
// {
//     DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
//
//     for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
//     {
//         if ( strcmp(tagid, (*existed)->nodeid) == 0 )
//         {
//             return (*existed)->buildingfloor;
//         }
//     }
//     return -1;
// }

NodeStruct* DeviceManager::get_Tag_node(int projectid, const char * tagid)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(tagid, (*existed)->nodeid) == 0 )
        {
            return (*existed);
        }
    }
    return NULL;
}

NodeStruct* DeviceManager::get_Tag_node_ByMacaddress(int projectid, const char * macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            return (*existed);
        }
    }
    return NULL;
}

NodeStruct* DeviceManager::get_Anchor_node_ByMacaddress(int projectid, const char * Anchor_macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mAnchorNodeList->begin(); existed != deviceInfo->mAnchorNodeList->end(); existed++)
    {
        if ( strcmp(Anchor_macaddress, (*existed)->macaddress) == 0 )
        {
            return (*existed);
        }
    }
    return NULL;
}

NodeStruct* DeviceManager::get_Anchor_node(int projectid, const char * anchorid)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mAnchorNodeList->begin(); existed != deviceInfo->mAnchorNodeList->end(); existed++)
    {
        if ( strcmp(anchorid, (*existed)->nodeid) == 0 )
        {
            return (*existed);
        }
    }
    return NULL;
}

NodeStruct* DeviceManager::get_Cooridinator_node_ByMacaddress(int projectid, const char * macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mCoordNodeList->begin(); existed != deviceInfo->mAnchorNodeList->end(); existed++)
    {
        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            return (*existed);
        }
    }
    return NULL;
}

char * DeviceManager::getAnchorId(int projectid, int Anchor_idx)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mAnchorNodeList->begin(); existed != deviceInfo->mAnchorNodeList->end(); existed++)
    {
        if ( (*existed)->index == Anchor_idx )
        {
            return (*existed)->nodeid;
        }
    }
    return (char *)"";
}

char * DeviceManager::getAnchorId(int projectid, const char * Anchor_macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mAnchorNodeList->begin(); existed != deviceInfo->mAnchorNodeList->end(); existed++)
    {
        if ( strcmp(Anchor_macaddress, (*existed)->macaddress) == 0 )
        {
            return (*existed)->nodeid;
        }
    }
    return (char *)"";
}

char * DeviceManager::getAnchorName(int projectid, const char * anchorid)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mAnchorNodeList->begin(); existed != deviceInfo->mAnchorNodeList->end(); existed++)
    {
        if ( strcmp(anchorid, (*existed)->nodeid) == 0 )
        {
            return (*existed)->nodename;
        }
    }
    return (char *)"";
}

char * DeviceManager::getLocatorId(int projectid, const char * locator_macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( strcmp(locator_macaddress, (*existed)->macaddress) == 0 )
        {
            return (*existed)->nodeid;
        }
    }
    return (char *)"";
}

char * DeviceManager::getLocatorIdByLoRAmacaddress(int projectid, const char * LoRA_macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( strcmp(LoRA_macaddress, (*existed)->LoRAMacaddress) == 0 )
        {
            return (*existed)->nodeid;
        }
    }
    return (char *)"";
}

char * DeviceManager::getAoAId(int projectid, const char * macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<AoANodeStruct*>::iterator existed = deviceInfo->mAoANodeList->begin(); existed != deviceInfo->mAoANodeList->end(); existed++)
    {
        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            return (*existed)->nodeid;
        }
    }
    return (char *)"";
}

AoANodeStruct * DeviceManager::get_AoA_node(int projectid, const char * macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<AoANodeStruct*>::iterator existed = deviceInfo->mAoANodeList->begin(); existed != deviceInfo->mAoANodeList->end(); existed++)
    {
        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            return (*existed);
        }
    }
    return NULL;
}

AoANodeStruct * DeviceManager::get_AoA_nodeById(int projectid, const char * nodeid)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<AoANodeStruct*>::iterator existed = deviceInfo->mAoANodeList->begin(); existed != deviceInfo->mAoANodeList->end(); existed++)
    {
        if ( strcmp(nodeid, (*existed)->nodeid) == 0 )
        {
            return (*existed);
        }
    }
    return NULL;
}

bool DeviceManager::setAoAInfo(int projectid, const char * macaddress, int posX, int posY, int posZ)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    bool bFound = false;
    for (std::list<AoANodeStruct*>::iterator existed = deviceInfo->mAoANodeList->begin(); existed != deviceInfo->mAoANodeList->end(); existed++)
    {
        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            vilslog::printf("DeviceManager::setAoAInfo() macaddress[%s] posX[%d] posY[%d] posZ[%d] ->  posX[%d] posY[%d] posZ[%d]\n",
                macaddress, (*existed)->posX, (*existed)->posY, (*existed)->posZ, posX, posY, posZ);

            (*existed)->posX = posX;
            (*existed)->posY = posY;
            (*existed)->posZ = posZ;

            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setAoAInfoById(int projectid, const char * nodeid, int posX, int posY, int posZ)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    bool bFound = false;
    for (std::list<AoANodeStruct*>::iterator existed = deviceInfo->mAoANodeList->begin(); existed != deviceInfo->mAoANodeList->end(); existed++)
    {
        if ( strcmp(nodeid, (*existed)->nodeid) == 0 )
        {
            (*existed)->posX = posX;
            (*existed)->posY = posY;
            (*existed)->posZ = posZ;

            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setAoAInfoS(int projectid, const char * macaddress, int pitch, int roll, int yaw)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    bool bFound = false;
    for (std::list<AoANodeStruct*>::iterator existed = deviceInfo->mAoANodeList->begin(); existed != deviceInfo->mAoANodeList->end(); existed++)
    {
        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            (*existed)->pitch = pitch;
            (*existed)->roll = roll;
            (*existed)->yaw = yaw;

            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setAoAInfoSById(int projectid, const char * nodeid, int pitch, int roll, int yaw)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    bool bFound = false;
    for (std::list<AoANodeStruct*>::iterator existed = deviceInfo->mAoANodeList->begin(); existed != deviceInfo->mAoANodeList->end(); existed++)
    {
        if ( strcmp(nodeid, (*existed)->nodeid) == 0 )
        {
            (*existed)->pitch = pitch;
            (*existed)->roll = roll;
            (*existed)->yaw = yaw;

            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setAoAInfoMacAddress(int projectid, const char * nodeid, const char * macaddress)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<AoANodeStruct*>::iterator existed = deviceInfo->mAoANodeList->begin(); existed != deviceInfo->mAoANodeList->end(); existed++)
    {
        // check mac address
        if ( strcmp(nodeid, (*existed)->nodeid) == 0 )
        {
            strcpy((*existed)->macaddress, macaddress);
            bFound = true;
            //break;
        }
    }

    return bFound;
}

bool DeviceManager::setAoAInfoGridSize(int projectid, const char * nodeid, int AoAGridSize)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    if(AoAGridSize == 0)
    {
        return bFound;
    }

    for (std::list<AoANodeStruct*>::iterator existed = deviceInfo->mAoANodeList->begin(); existed != deviceInfo->mAoANodeList->end(); existed++)
    {
        if ( strcmp(nodeid, (*existed)->nodeid) == 0 )
        {
            (*existed)->gridsize = AoAGridSize;

            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setNodeInfoMapLayer(int projectid, int node_type, const char * macaddress, int maplayer)
{
    bool bFound = false;

    switch(node_type)
    {
        default:
        break;
        case NODE_TYPE_ANCHOR: // Anchor
            bFound = setAnchorInfoMapLayer(projectid, macaddress, maplayer);
        break;
        case NODE_TYPE_TAG: // Tag
            bFound = setTagInfoMapLayer(projectid, macaddress, maplayer);
        break;
        case NODE_TYPE_COORD: // Coordinator
            bFound = setCoordInfoMapLayer(projectid, macaddress, maplayer);
        break;
        case NODE_TYPE_LOCATOR: // Locator
            bFound = setLocatorInfoMapLayer(projectid, macaddress, maplayer);
        break;
    }

    return bFound;
}

char * DeviceManager::getLocatorName(int projectid, const char * locatorid)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( strcmp(locatorid, (*existed)->nodeid) == 0 )
        {
            return (*existed)->nodename;
        }
    }
    return (char *)"";
}

int DeviceManager::getLocatorButtonType(int projectid, const char * locatorid)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( strcmp(locatorid, (*existed)->nodeid) == 0 )
        {
            return (*existed)->tafid;
        }
    }
    return 0;
}

// int DeviceManager::getAnchorFloor(int projectid, const char * Anchor_macaddress)
// {
//     DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
//
//     for (std::list<NodeStruct*>::iterator existed = deviceInfo->mAnchorNodeList->begin(); existed != deviceInfo->mAnchorNodeList->end(); existed++)
//     {
//         if ( strcmp(Anchor_macaddress, (*existed)->macaddress) == 0 )
//         {
//             return (*existed)->buildingfloor;
//         }
//     }
//     return -1;
// }
//
// int DeviceManager::getAnchorFloorById(int projectid, const char * AnchorId)
// {
//     DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
//
//     for (std::list<NodeStruct*>::iterator existed = deviceInfo->mAnchorNodeList->begin(); existed != deviceInfo->mAnchorNodeList->end(); existed++)
//     {
//         if ( strcmp(AnchorId, (*existed)->nodeid) == 0 )
//         {
//             return (*existed)->buildingfloor;
//         }
//     }
//     return -1;
// }

int DeviceManager::getTagMapLayerById(int projectid, const char * nodeid)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(nodeid, (*existed)->nodeid) == 0 )
        {
            return (*existed)->maplayer;
        }
    }
    return 0;
}

int DeviceManager::getAnchorMapLayer(int projectid, const char * Anchor_macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mAnchorNodeList->begin(); existed != deviceInfo->mAnchorNodeList->end(); existed++)
    {
        if ( strcmp(Anchor_macaddress, (*existed)->macaddress) == 0 )
        {
            return (*existed)->maplayer;
        }
    }
    return 0;
}

int DeviceManager::getAnchorMapLayerById(int projectid, const char * AnchorId)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mAnchorNodeList->begin(); existed != deviceInfo->mAnchorNodeList->end(); existed++)
    {
        if ( strcmp(AnchorId, (*existed)->nodeid) == 0 )
        {
            return (*existed)->maplayer;
        }
    }
    return 0;
}

// int DeviceManager::getLocatorFloor(int projectid, const char * locator_macaddress)
// {
//     DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
//
//     for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
//     {
//         if ( strcmp(locator_macaddress, (*existed)->macaddress) == 0 )
//         {
//             return (*existed)->buildingfloor;
//         }
//     }
//     return -1;
// }
//
// int DeviceManager::getLocatorFloorByLoRAmacaddress(int projectid, const char * LoRA_macaddress)
// {
//     DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
//
//     for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
//     {
//         if ( strcmp(LoRA_macaddress, (*existed)->LoRAMacaddress) == 0 )
//         {
//             return (*existed)->buildingfloor;
//         }
//     }
//     return -1;
// }

int DeviceManager::getLocatorMapLayer(int projectid, const char * locator_macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( strcmp(locator_macaddress, (*existed)->macaddress) == 0 )
        {
            return (*existed)->maplayer;
        }
    }
    return 0;
}

int DeviceManager::getLocatorMapLayerByLoRAmacaddress(int projectid, const char * LoRA_macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( strcmp(LoRA_macaddress, (*existed)->LoRAMacaddress) == 0 )
        {
            return (*existed)->maplayer;
        }
    }
    return 0;
}

int DeviceManager::getLocatorHeartBeat(int projectid, const char * LoRA_macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( strcmp(LoRA_macaddress, (*existed)->LoRAMacaddress) == 0 )
        {
            return (*existed)->heartbeat;
        }
    }
    return 0;
}

int DeviceManager::getLocatorWeight(int projectid, const char * LoRA_macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( strcmp(LoRA_macaddress, (*existed)->LoRAMacaddress) == 0 )
        {
            return (*existed)->weight;
        }
    }
    return 0;
}

char * DeviceManager::getLocatorMacaddress(int projectid, const char * locatorid)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( strcmp(locatorid, (*existed)->nodeid) == 0 )
        {
            return (*existed)->macaddress;
        }
    }
    return (char *)"";
}

NodeStruct * DeviceManager::get_Locator_node(int projectid, const char * LoRA_macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( strcmp(LoRA_macaddress, (*existed)->LoRAMacaddress) == 0 )
        {
            return (*existed);
        }
    }
    return NULL;
}

NodeStruct * DeviceManager::get_Locator_node_byid(int projectid, const char * locatorid)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( strcmp(locatorid, (*existed)->nodeid) == 0 )
        {
            return (*existed);
        }
    }
    return NULL;
}

NodeStruct * DeviceManager::get_Locator_node_byUserid(int projectid, int userid)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( userid == (*existed)->userid )
        {
            return (*existed);
        }
    }
    return NULL;
}

NodeStruct * DeviceManager::get_Locator_node_ByMacaddress(int projectid, const char * macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            return (*existed);
        }
    }
    return NULL;
}

int DeviceManager::getNodeIndex(std::list<NodeStruct*> * plist, const char * macaddress)
{
    int index = -1;
    for (std::list<NodeStruct*>::iterator existed = plist->begin(); existed != plist->end(); existed++)
    {
        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            index = (*existed)->index;
            break;
        }
    }
    return index;
}

std::list<NodeStruct*> * DeviceManager::get_Anchor_list(int projectid)
{
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        if (mDeviceInfoList[i]->projectid == projectid)
        {
            return mDeviceInfoList[i]->mAnchorNodeList;
        }
    }
    return NULL;
    // return mAnchorNodeList;
}

std::list<NodeStruct*> * DeviceManager::get_Tag_list(int projectid)
{
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        if (mDeviceInfoList[i]->projectid == projectid)
        {
            return mDeviceInfoList[i]->mTagNodeList;
        }
    }
    return NULL;
}

std::list<NodeStruct*> * DeviceManager::get_Coordinator_list(int projectid)
{
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        if (mDeviceInfoList[i]->projectid == projectid)
        {
            return mDeviceInfoList[i]->mCoordNodeList;
        }
    }
    return NULL;
    // return mCoordNodeList;
}

std::list<NodeStruct*> * DeviceManager::get_Locator_list(int projectid)
{
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        if (mDeviceInfoList[i]->projectid == projectid)
        {
            return mDeviceInfoList[i]->mLocatorNodeList;
        }
    }
    return NULL;
    // return mLocatorNodeList;
}

std::list<LoRaGatewayInfoStruct*> * DeviceManager::get_LoRaGateway_list(int projectid)
{
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        if (mDeviceInfoList[i]->projectid == projectid)
        {
            return mDeviceInfoList[i]->mLoRaGatewayList;
        }
    }
    return NULL;
}

std::list<AoANodeStruct*> * DeviceManager::get_AoA_list(int projectid)
{
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        if (mDeviceInfoList[i]->projectid == projectid)
        {
            return mDeviceInfoList[i]->mAoANodeList;
        }
    }
    return NULL;
}

std::list<RFIDDeviceStruct*> * DeviceManager::get_RFIDDevice_list(int projectid)
{
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        if (mDeviceInfoList[i]->projectid == projectid)
        {
            return mDeviceInfoList[i]->mRFIDDeviceList;
        }
    }
    return NULL;
}

std::list<NFCDeviceStruct*> * DeviceManager::get_NFCDevice_list(int projectid)
{
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        if (mDeviceInfoList[i]->projectid == projectid)
        {
            return mDeviceInfoList[i]->mNFCDeviceList;
        }
    }
    return NULL;
}

std::list<RFIDCardStruct*> * DeviceManager::get_RFIDCard_list(int projectid)
{
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        if (mDeviceInfoList[i]->projectid == projectid)
        {
            return mDeviceInfoList[i]->mRFIDCardList;
        }
    }
    return NULL;
}

std::list<NFCCardStruct*> * DeviceManager::get_NFCCard_list(int projectid)
{
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        if (mDeviceInfoList[i]->projectid == projectid)
        {
            return mDeviceInfoList[i]->mNFCCardList;
        }
    }
    return NULL;
}

std::list<NFCCardStruct*> * DeviceManager::get_MaingroupNFCCard_list(int projectid)
{
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        if (mDeviceInfoList[i]->projectid == projectid)
        {
            return mDeviceInfoList[i]->mMaingroupNFCCardList;
        }
    }
    return NULL;
}

std::list<LocatorRSSIInfoStruct*> * DeviceManager::get_LocatorRSSIInfo_list(int projectid)
{
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        if (mDeviceInfoList[i]->projectid == projectid)
        {
            return mDeviceInfoList[i]->mLocatorRSSIInfoList;
        }
    }
    return NULL;
}

std::list<EDGEDeviceStruct*> * DeviceManager::get_EDGEDevice_list(int projectid)
{
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        if (mDeviceInfoList[i]->projectid == projectid)
        {
            return mDeviceInfoList[i]->mEDGEDeviceList;
        }
    }
    return NULL;
}

std::list<SensorDeviceStruct*> * DeviceManager::get_SensorDevice_list(int projectid)
{
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        if (mDeviceInfoList[i]->projectid == projectid)
        {
            return mDeviceInfoList[i]->mSensorDeviceList;
        }
    }
    return NULL;
}

std::list<UserStruct*> * DeviceManager::get_User_list(int projectid)
{
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        if (mDeviceInfoList[i]->projectid == projectid)
        {
            return mDeviceInfoList[i]->mUserList;
        }
    }
    return NULL;
}

bool DeviceManager::getNodeCoord(std::list<NodeStruct*> * plist, const char * nodeid, double * coord)
{
    bool bFound = false;
    for (std::list<NodeStruct*>::iterator existed = plist->begin(); existed != plist->end(); existed++)
    {
        if ( strcmp(nodeid, (*existed)->nodeid) == 0 )
        {
            coord[0] = (*existed)->posX;
            coord[1] = (*existed)->posY;
            coord[2] = (*existed)->posZ;

            // vilslog::printf ("DeviceManager::getNodeCoord() nodeid[%s] coord[%d,%d,%d]\n", nodeid, (int)coord[0], (int)coord[1], (int)coord[2]);

            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::getAnchorCoord(int projectid, const char * anchorid, double * AnchorCoord)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    return getNodeCoord(deviceInfo->mAnchorNodeList, anchorid, AnchorCoord);
}

bool DeviceManager::getLocatorCoord(int projectid, const char * locatorid, double * LocatorCoord)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    return getNodeCoord(deviceInfo->mLocatorNodeList, locatorid, LocatorCoord);
}

bool DeviceManager::getCoordinatorCoord(int projectid, const char * coordid, double * CoordCoord)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    return getNodeCoord(deviceInfo->mCoordNodeList, coordid, CoordCoord);
}

bool DeviceManager::getTagCoord(int projectid, const char * tagid, double * TagCoord)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    return getNodeCoord(deviceInfo->mTagNodeList, tagid, TagCoord);
}

bool DeviceManager::getTagSmoothingCoord(int projectid, const char * nodeid, double * coord)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(nodeid, (*existed)->nodeid) == 0 )
        {
            coord[0] = (*existed)->smoothingPosX;
            coord[1] = (*existed)->smoothingPosY;
            coord[2] = (*existed)->smoothingPosZ;
            bFound = true;
            break;
        }
    }
    return bFound;
}

int DeviceManager::getTagPrePos(int projectid, const char * TagId, double * TagCoord)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(TagId, (*existed)->nodeid) == 0 )
        {
            TagCoord[0] = (*existed)->prePosX;
            TagCoord[1] = (*existed)->prePosY;
            TagCoord[2] = (*existed)->prePosZ;
            return 3;
        }
    }
    return 0;
}

int DeviceManager::getTagMomentum(int projectid, const char * TagId, double * momentum)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(TagId, (*existed)->nodeid) == 0 )
        {
            momentum[0] = (*existed)->momentumX;
            momentum[1] = (*existed)->momentumY;
            momentum[2] = (*existed)->momentumZ;
            return 3;
        }
    }
    return 0;
}

time_t DeviceManager::getTagDiffTimestamp(int projectid, const char * TagId)
{
     time_t diffTime = 0;
     DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
     for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
     {
         if (strcmp(TagId, (*existed)->nodeid) == 0)
         {
             time_t currentTime = getCurrentTimeInMilliSec();
             diffTime = currentTime -  (*existed)->updateTimestamp;
             break;
         }
     }
     return diffTime;
}

time_t DeviceManager::getTagAlertDiffTimestamp(int projectid, const char * macaddress)
{
    time_t diffTime = 0;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if (strcmp(macaddress, (*existed)->macaddress) == 0)
        {
            if ((*existed)->alertIssuTime == 0)
            {
                  diffTime = (time_t)20000;
            }
            else
            {
                time_t currentTime = getCurrentTimeInMilliSec();
                diffTime = currentTime -  (*existed)->alertIssuTime;
            }
            break;
        }
    }
    return diffTime;
}

time_t DeviceManager::getLocatorAlertDiffTimestamp(int projectid, const char * macaddress)
{
    time_t diffTime = -1;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if (strcmp(macaddress, (*existed)->macaddress) == 0)
        {
            if ((*existed)->alertIssuTime == 0)
            {
                  diffTime = (time_t)20000;
            }
            else
            {
                time_t currentTime = getCurrentTimeInMilliSec();
                diffTime = currentTime -  (*existed)->alertIssuTime;
            }
            break;
        }
    }
    return diffTime;
}

time_t DeviceManager::getLocatorAlertDiffTimestampByLoRAmacaddress(int projectid, const char * LoRAMacaddress)
{
    time_t diffTime = -1;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if (strcmp(LoRAMacaddress, (*existed)->LoRAMacaddress) == 0)
        {
            if ((*existed)->alertIssuTime == 0)
            {
                  diffTime = (time_t)20000;
            }
            else
            {
                time_t currentTime = getCurrentTimeInMilliSec();
                diffTime = currentTime -  (*existed)->alertIssuTime;
            }
            break;
        }
    }
    return diffTime;
}

void DeviceManager::updateLocatorAlertIssuTime(int projectid, const char * macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mLocatorNodeList->begin(); existed != deviceInfo->mLocatorNodeList->end(); existed++)
    {
        if (strcmp(macaddress, (*existed)->macaddress) == 0)
        {
            (*existed)->alertIssuTime = getCurrentTimeInMilliSec();
            break;
        }
    }
}

char * DeviceManager::getTagAreaId(int projectid, const char * TagId)
{
    char * areaid = NULL;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(TagId, (*existed)->nodeid) == 0 )
        {
            areaid = (*existed)->AreaId;
            break;
        }
    }
    if (areaid == NULL)
        return (char *)"";
    else
        return areaid;
}

std::string DeviceManager::getempidFromTagByAliveLoRAranging(int projectid, const char * LoRA_macaddress)
{
    std::string empid = "";
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( (strcmp(LoRA_macaddress, (*existed)->LoRAMacaddress) == 0) && ((*existed)->alive > 0) )
        {
            empid = (*existed)->empid;
            break;
        }
    }
    return empid;
}

int DeviceManager::getAnchorProjectId(const char * Anchor_macaddress)
{
    int projectid = -1;
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        projectid = getNodeProjectId(mDeviceInfoList[i]->mAnchorNodeList, Anchor_macaddress);
        if ( projectid > 0 )
            break;
    }
    return projectid;
    // return getNodeProjectId(mAnchorNodeList, Anchor_macaddress);
}

int DeviceManager::getTagProjectId(const char * Tag_macaddress)
{
    int projectid = -1;
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        projectid = getNodeProjectId(mDeviceInfoList[i]->mTagNodeList, Tag_macaddress);
        if ( projectid > 0 )
            break;
    }
    return projectid;
    // return getNodeProjectId(mTagNodeList, Tag_macaddress);
}

int DeviceManager::getCoordinatorProjectId(const char * Coord_macaddress)
{
    int projectid = -1;
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        projectid = getNodeProjectId(mDeviceInfoList[i]->mCoordNodeList, Coord_macaddress);
        if ( projectid > 0 )
            break;
    }
    return projectid;
    // return getNodeProjectId(mCoordNodeList, Coord_macaddress);
}

int DeviceManager::getLocatorProjectId(const char * Locator_macaddress)
{
    int projectid = -1;
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        projectid = getNodeProjectId(mDeviceInfoList[i]->mLocatorNodeList, Locator_macaddress);
        if ( projectid > 0 )
            break;
    }
    return projectid;
    // return getNodeProjectId(mLocatorNodeList, Locator_macaddress);
}

int DeviceManager::getLoRaGatewayProjectId(std::string gwid)
{
    int projectid = 1;
    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        for (std::list<LoRaGatewayInfoStruct*>::iterator gateway = mDeviceInfoList[i]->mLoRaGatewayList->begin(); gateway != mDeviceInfoList[i]->mLoRaGatewayList->end(); gateway++)
        {
            if ( (*gateway)->gwid.compare(gwid) == 0 )
            {
                projectid = (*gateway)->projectid;
                break;
            }
        }
    }
    return projectid;
}

int DeviceManager::getNodeProjectId(std::list<NodeStruct*> * plist, const char * macaddress)
{
    int projectid = -1;
    for (std::list<NodeStruct*>::iterator existed = plist->begin(); existed != plist->end(); existed++)
    {
        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            projectid = (*existed)->projectid;
            break;
        }
    }
    return projectid;
}

int DeviceManager::getLocatorProjectIdByLoRAmacaddress(const char * LoRA_macaddress)
{
    int projectid = -1;

    for (unsigned int i=0; i< mDeviceInfoList.size(); i++)
    {
        for (std::list<NodeStruct*>::iterator existed = mDeviceInfoList[i]->mLocatorNodeList->begin(); existed != mDeviceInfoList[i]->mLocatorNodeList->end(); existed++)
        {
            if ( strcmp(LoRA_macaddress, (*existed)->LoRAMacaddress) == 0 )
            {
                projectid = (*existed)->projectid;
                break;
            }
        }
    }
    return projectid;
}

char * DeviceManager::getCoordinatorIP(int projectid, const char * macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mCoordNodeList->begin(); existed != deviceInfo->mCoordNodeList->end(); existed++)
    {
        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            return (*existed)->IP;
        }
    }
    return (char *)"";
}

char * DeviceManager::getCoordinatorIdFromIP(int projectid, const char * IPAddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mCoordNodeList->begin(); existed != deviceInfo->mCoordNodeList->end(); existed++)
    {
        if ( strcmp(IPAddress, (*existed)->IP) == 0 )
        {
            return (*existed)->nodeid;
        }
    }
    return (char *)"";
}

int DeviceManager::getCoordinatorMapLayer(int projectid, const char * macaddress)
{
    int maplayerid = 0;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mCoordNodeList->begin(); existed != deviceInfo->mCoordNodeList->end(); existed++)
    {
        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            return (*existed)->maplayer;
        }
    }
    return maplayerid;
}

int DeviceManager::getCoordinatorMapLayerFromIP(int projectid, const char * IPAddress)
{
    int maplayerid = 0;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mCoordNodeList->begin(); existed != deviceInfo->mCoordNodeList->end(); existed++)
    {
        if ( strcmp(IPAddress, (*existed)->IP) == 0 )
        {
            return (*existed)->maplayer;
        }
    }
    return maplayerid;
}

int DeviceManager::getCoordinatorjoinRangingRange(int projectid, const char * macaddress)
{
    int joinRangingRange = 0;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mCoordNodeList->begin(); existed != deviceInfo->mCoordNodeList->end(); existed++)
    {
        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            joinRangingRange = (*existed)->joinRangingRange;
            break;
        }
    }
    return joinRangingRange;
}

int DeviceManager::getCoordinatorjoinNotRangingRange(int projectid, const char * macaddress)
{
    int joinNotRangingRange = 0;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mCoordNodeList->begin(); existed != deviceInfo->mCoordNodeList->end(); existed++)
    {
        if ( strcmp(macaddress, (*existed)->macaddress) == 0 )
        {
            joinNotRangingRange = (*existed)->joinNotRangingRange;
            break;
        }
    }
    return joinNotRangingRange;
}

RFIDDeviceStruct * DeviceManager::get_RFIDDevice(int projectid, const char * macaddress)
{
    RFIDDeviceStruct * rfiddevice = NULL;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<RFIDDeviceStruct*>::iterator existed = deviceInfo->mRFIDDeviceList->begin(); existed != deviceInfo->mRFIDDeviceList->end(); existed++)
    {
        if ((*existed)->macaddress.compare(macaddress) == 0 )
        {
            rfiddevice = (*existed);
            break;
        }
    }
    return rfiddevice;
}

RFIDDeviceStruct * DeviceManager::get_RFIDDeviceByLoRAmacaddress(int projectid, const char * LoRAmacaddress)
{
    RFIDDeviceStruct * rfiddevice = NULL;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<RFIDDeviceStruct*>::iterator existed = deviceInfo->mRFIDDeviceList->begin(); existed != deviceInfo->mRFIDDeviceList->end(); existed++)
    {
        if ((*existed)->LoRAMacaddress.compare(LoRAmacaddress) == 0 )
        {
            rfiddevice = (*existed);
            break;
        }
    }
    return rfiddevice;
}

NFCDeviceStruct * DeviceManager::get_NFCDevice(int projectid, const char * macaddress)
{
    NFCDeviceStruct * nfcdevice = NULL;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NFCDeviceStruct*>::iterator existed = deviceInfo->mNFCDeviceList->begin(); existed != deviceInfo->mNFCDeviceList->end(); existed++)
    {
        if ((*existed)->macaddress.compare(macaddress) == 0 )
        {
            nfcdevice = (*existed);
            break;
        }
    }
    return nfcdevice;
}

NFCDeviceStruct * DeviceManager::get_NFCDeviceByLoRAmacaddress(int projectid, const char * LoRAmacaddress)
{
    NFCDeviceStruct * nfcdevice = NULL;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NFCDeviceStruct*>::iterator existed = deviceInfo->mNFCDeviceList->begin(); existed != deviceInfo->mNFCDeviceList->end(); existed++)
    {
        if ((*existed)->LoRAMacaddress.compare(LoRAmacaddress) == 0 )
        {
            nfcdevice = (*existed);
            break;
        }
    }
    return nfcdevice;
}

EDGEDeviceStruct * DeviceManager::get_EDGEDevice(int projectid, const char * macaddress)
{
    EDGEDeviceStruct * edgedevice = NULL;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<EDGEDeviceStruct*>::iterator existed = deviceInfo->mEDGEDeviceList->begin(); existed != deviceInfo->mEDGEDeviceList->end(); existed++)
    {
        if ((*existed)->macaddress.compare(macaddress) == 0 )
        {
            edgedevice = (*existed);
            break;
        }
    }
    return edgedevice;
}

SensorDeviceStruct * DeviceManager::get_SensorDevice(int projectid, std::string id)
{
    SensorDeviceStruct * device = NULL;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<SensorDeviceStruct*>::iterator existed = deviceInfo->mSensorDeviceList->begin(); existed != deviceInfo->mSensorDeviceList->end(); existed++)
    {
        if ((*existed)->id.compare(id) == 0 )
        {
            device = (*existed);
            break;
        }
    }
    return device;
}

bool DeviceManager::setRFIDDeviceAlive(int projectid, RFIDDeviceStruct * device)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<RFIDDeviceStruct*>::iterator existed = deviceInfo->mRFIDDeviceList->begin(); existed != deviceInfo->mRFIDDeviceList->end(); existed++)
    {
        if ( device->macaddress.compare((*existed)->macaddress) == 0 )
        {
            time_t curTime = getCurrentTimeInSec();
            time_t diffTime = curTime - (*existed)->aliveTime;
            if ( (int)diffTime > 1 )
            {
                (*existed)->alive++;
            }
            (*existed)->aliveTime = curTime;
            return true;
        }
    }

    RFIDDeviceStruct * record = new RFIDDeviceStruct();
    record->deviceid = device->deviceid;
    record->macaddress = device->macaddress;
    record->type = device->type;
    record->name = device->name;
    record->ip = device->ip;
    record->gwid = device->gwid;
    record->LoRAMacaddress = device->LoRAMacaddress;
    record->datetime = device->datetime;
    record->alive = 1;
    record->aliveTime = getCurrentTimeInSec();
    record->maingroupid = device->maingroupid;

    //===================================
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    record->verifykey = baseDb->create_verifykey("RFIDDevice");
    baseDb->update_RFIDDevice(record);
    //===================================

    deviceInfo->mRFIDDeviceList->push_back(record);

    return false;
}

int DeviceManager::setRFIDDeviceAlive(int projectid, const char * Locator_macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<RFIDDeviceStruct*>::iterator existed = deviceInfo->mRFIDDeviceList->begin(); existed != deviceInfo->mRFIDDeviceList->end(); existed++)
    {
        if ((*existed)->LoRAMacaddress.compare(Locator_macaddress) == 0 )
        {
            time_t curTime = getCurrentTimeInSec();
            time_t diffTime = curTime - (*existed)->aliveTime;
            if ( (int)diffTime > 1 )
            {
                (*existed)->alive++;
            }
            (*existed)->aliveTime = curTime;
            return (*existed)->deviceid;
        }
    }
    return 0;
}

bool DeviceManager::setNFCDeviceAlive(int projectid, NFCDeviceStruct * device)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NFCDeviceStruct*>::iterator existed = deviceInfo->mNFCDeviceList->begin(); existed != deviceInfo->mNFCDeviceList->end(); existed++)
    {
        if ( device->macaddress.compare((*existed)->macaddress) == 0 )
        {
            time_t curTime = getCurrentTimeInSec();
            time_t diffTime = curTime - (*existed)->aliveTime;
            if ( (int)diffTime > 1 )
            {
                (*existed)->alive++;
            }
            (*existed)->aliveTime = curTime;

            if ( device->ssid.length() > 0 )
            {
                (*existed)->ssid = device->ssid;
            }
            if ( device->wifimac.length() > 0 )
            {
                (*existed)->wifimac = device->wifimac;
            }

            return true;
        }
    }

    NFCDeviceStruct * record = new NFCDeviceStruct();
    record->deviceid = device->deviceid;
    record->macaddress = device->macaddress;
    record->name = device->name;
    record->type = device->type;
    record->gwid = device->gwid;
    record->LoRAMacaddress = device->LoRAMacaddress;
    record->datetime = device->datetime;
    record->alive = 1;
    record->aliveTime = getCurrentTimeInSec();
    record->ssid = device->ssid;
    record->wifimac = device->wifimac;
    record->maingroupid = device->maingroupid;

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        record->verifykey = baseDb->create_verifykey("NFCDevice");
    }

    baseDb->update_NFCDevice(record);

    deviceInfo->mNFCDeviceList->push_back(record);

    return false;
}

int DeviceManager::setNFCDeviceAlive(int projectid, const char * Locator_macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NFCDeviceStruct*>::iterator existed = deviceInfo->mNFCDeviceList->begin(); existed != deviceInfo->mNFCDeviceList->end(); existed++)
    {
        if ((*existed)->LoRAMacaddress.compare(Locator_macaddress) == 0 )
        {
            time_t curTime = getCurrentTimeInSec();
            time_t diffTime = curTime - (*existed)->aliveTime;
            if ( (int)diffTime > 1 )
            {
                (*existed)->alive++;
            }
            (*existed)->aliveTime = curTime;
            return (*existed)->deviceid;
        }
    }
    return 0;
}

bool DeviceManager::setEDGEDeviceAlive(int projectid, EDGEDeviceStruct * device)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<EDGEDeviceStruct*>::iterator existed = deviceInfo->mEDGEDeviceList->begin(); existed != deviceInfo->mEDGEDeviceList->end(); existed++)
    {
        if ( device->macaddress.compare((*existed)->macaddress) == 0 )
        {
            time_t curTime = getCurrentTimeInSec();
            time_t diffTime = curTime - (*existed)->aliveTime;
            if ( (int)diffTime > 1 )
            {
                (*existed)->alive++;
            }
            (*existed)->aliveTime = curTime;

            if ( device->ip.length() > 0 )
            {
                (*existed)->ip = device->ip;
            }
            if ( device->gwid.length() > 0 )
            {
                (*existed)->gwid = device->gwid;
            }
            if ( device->version.length() > 0 )
            {
                (*existed)->version = device->version;
            }
            (*existed)->maingroupid = device->maingroupid;

            return true;
        }
    }

    EDGEDeviceStruct * record = new EDGEDeviceStruct();
    record->deviceid = device->deviceid;
    record->macaddress = device->macaddress;
    record->name = device->name;
    record->type = device->type;
    record->ip = device->ip;
    record->LoRAMacaddress = device->LoRAMacaddress;
    record->gwid = device->gwid;
    record->datetime = device->datetime;
    record->alive = 1;
    record->aliveTime = getCurrentTimeInSec();
    record->version = device->version;
    record->maingroupid = device->maingroupid;

    //===================================
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    record->verifykey = baseDb->create_verifykey("EDGEDevice");
    baseDb->update_EDGEDevice(record);
    //===================================

    deviceInfo->mEDGEDeviceList->push_back(record);

    return false;
}

bool DeviceManager::setSensorDeviceAlive(int projectid, SensorDeviceStruct * device)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<SensorDeviceStruct*>::iterator existed = deviceInfo->mSensorDeviceList->begin(); existed != deviceInfo->mSensorDeviceList->end(); existed++)
    {
        if ( device->id.compare((*existed)->id) == 0 )
        {
            (*existed)->alive++;
            (*existed)->aliveTime = getCurrentTimeInSec();

            int i_Longitude = 0;
            int i_Latitude = 0;
            if (device->Longitude.length() > 0)
            {
                i_Longitude = std::stoi(device->Longitude);
            }
            if (device->Latitude.length() > 0)
            {
                i_Latitude = std::stoi(device->Latitude);
            }

            // vilslog::printf("DeviceManager::setSensorDeviceAlive() device->id[%s] i_Longitude[%d] i_Latitude[%d] device->Longitude[%s] device->Latitude[%s]\n",
            //     device->id.c_str(), i_Longitude, i_Latitude, device->Longitude.c_str(), device->Latitude.c_str());
            // vilslog::printf("DeviceManager::setSensorDeviceAlive() device->id[%s] existed->Longitude[%s] existed->Latitude[%s]\n",
            //     device->id.c_str(), (*existed)->Longitude.c_str(), (*existed)->Latitude.c_str());

            if ( i_Longitude == 0 || i_Latitude == 0)
            {
                (*existed)->unknownLocation = 1;
            }
            else
            {
                // check same location
                if ( ((*existed)->Longitude.compare(device->Longitude) == 0) && ((*existed)->Latitude.compare(device->Latitude) == 0))
                {
                    int curDiffTime = (int)(getCurrentTimeInSec() - (*existed)->updateLocationTime);

                    // vilslog::printf("DeviceManager::setSensorDeviceAlive() curDiffTime[%d]\n", curDiffTime);


                    if ( curDiffTime >= 60)
                    {
                        (*existed)->unknownLocation = 1;
                    }
                }
                else
                {
                    (*existed)->Longitude = device->Longitude;
                    (*existed)->Latitude = device->Latitude;
                    (*existed)->unknownLocation = 0;
                    (*existed)->updateLocationTime = getCurrentTimeInSec();
                }
                // vilslog::printf("DeviceManager::setSensorDeviceAlive() device->id[%s] existed->Longitude[%s] existed->Latitude[%s]\n",
                //     device->id.c_str(), (*existed)->Longitude.c_str(), (*existed)->Latitude.c_str());
            }


            if ( device->jsonvalue.length() > 0 )
                (*existed)->jsonvalue = device->jsonvalue;
            // if ( device->Longitude.length() > 0 && device->Longitude.compare("0.000000") != 0 && device->Longitude.compare("0.0") != 0)
            //     (*existed)->Longitude = device->Longitude;
            // if ( device->Latitude.length() > 0 && device->Latitude.compare("0.000000") != 0 && device->Latitude.compare("0.0") != 0)
            //     (*existed)->Latitude = device->Latitude;
            if ( device->Battery.length() > 0 )
                (*existed)->Battery = device->Battery;
            if ( device->Version.length() > 0 )
                (*existed)->Version = device->Version;
            (*existed)->type = device->type;
            (*existed)->maingroupid = device->maingroupid;

            return true;
        }
    }

    SensorDeviceStruct * sensorDevice = new SensorDeviceStruct();
    sensorDevice->alive = device->alive;
    sensorDevice->maingroupid = device->maingroupid;
    sensorDevice->id = device->id;
    sensorDevice->name = device->name;
    sensorDevice->type = device->type;
    sensorDevice->jsonvalue = device->jsonvalue;
    sensorDevice->Longitude = device->Longitude;
    sensorDevice->Latitude = device->Latitude;
    sensorDevice->Battery = device->Battery;
    sensorDevice->Version = device->Version;
    sensorDevice->datetime = device->datetime;
    sensorDevice->aliveTime = getCurrentTimeInSec();

    int i_Longitude = 0;
    int i_Latitude = 0;
    if (device->Longitude.length() > 0)
    {
        i_Longitude = std::stoi(device->Longitude);
    }
    if (device->Latitude.length() > 0)
    {
        i_Latitude = std::stoi(device->Latitude);
    }

    if ( i_Longitude == 0 || i_Latitude == 0)
    {
        sensorDevice->unknownLocation = 1;
    }
    else
    {
        sensorDevice->unknownLocation = 0;
    }
    sensorDevice->value_buffer_size = 0;
    sensorDevice->value_buffer_index = 0;
    for(int i=0;i<SENSOR_VALUE_BUFFER_SIZE;i++)
    {
        sensorDevice->value_buffer[i] = "0";
        sensorDevice->value_datetime[i] = "";
    }

    //===================================
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    sensorDevice->verifykey = baseDb->create_verifykey("sensordevice");
    baseDb->update_SensorDevice(sensorDevice);
    //===================================

    deviceInfo->mSensorDeviceList->push_back(sensorDevice);

    return false;
}

SensorDeviceStruct * DeviceManager::setSensorDeviceLocation(int projectid, std::string deviceid, std::string Latitude, std::string Longitude)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<SensorDeviceStruct*>::iterator existed = deviceInfo->mSensorDeviceList->begin(); existed != deviceInfo->mSensorDeviceList->end(); existed++)
    {
        if ( deviceid.compare((*existed)->id) == 0 )
        {
            int i_Longitude = 0;
            int i_Latitude = 0;
            if (Longitude.length() > 0)
            {
                i_Longitude = (int)std::stof(Longitude);
            }
            if (Latitude.length() > 0)
            {
                i_Latitude = (int)std::stof(Latitude);
            }

            // vilslog::printf ("DeviceManager::setSensorDeviceLocation() deviceid[%s] (*existed)->Latitude[%s] (*existed)->Longitude[%s] i_Longitude[%d] i_Latitude[%d]\n",
            //     deviceid.c_str(), (*existed)->Latitude.c_str(), (*existed)->Longitude.c_str(), i_Longitude, i_Latitude);

            if ( i_Longitude != 0 && i_Latitude != 0)
            {
                (*existed)->Latitude = Latitude;
                (*existed)->Longitude = Longitude;
            }

            return (*existed);
        }
    }

    return NULL;
}

bool DeviceManager::setSensorDeviceValue(int projectid, std::string deviceid, std::string value, std::string datetime)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<SensorDeviceStruct*>::iterator existed = deviceInfo->mSensorDeviceList->begin(); existed != deviceInfo->mSensorDeviceList->end(); existed++)
    {
        // vilslog::printf("DeviceManager::setSensorDeviceValue() projectid[%d] deviceid[%s] (*existed)->id[%s] value[%s] datetime[%s]\n",
        //     projectid, deviceid.c_str(), (*existed)->id.c_str(), value.c_str(), datetime.c_str());

        if ( deviceid.compare((*existed)->id) == 0 )
        {
            // vilslog::printf("DeviceManager::setSensorDeviceValue() projectid[%d] deviceid[%s] (*existed)->value_buffer_index[%d] (*existed)->value_buffer_size[%d]\n",
            //     projectid, deviceid.c_str(), (*existed)->value_buffer_index, (*existed)->value_buffer_size);

            if ( (*existed)->value_buffer_size < SENSOR_VALUE_BUFFER_SIZE )
            {
                (*existed)->value_buffer[ (*existed)->value_buffer_index ] = value;
                (*existed)->value_datetime[ (*existed)->value_buffer_index ] = datetime;
                (*existed)->value_buffer_size++;
                (*existed)->value_buffer_index++;
            }
            else
            {
                (*existed)->value_buffer[ (*existed)->value_buffer_index ] = value;
                (*existed)->value_datetime[ (*existed)->value_buffer_index ] = datetime;
                (*existed)->value_buffer_index++;
            }

            if ( (*existed)->value_buffer_index >= SENSOR_VALUE_BUFFER_SIZE )
            {
                (*existed)->value_buffer_index = 0;
            }
            break;
        }
    }
    return true;
}

RFIDCardStruct * DeviceManager::get_RFIDCard(int projectid, const char * cardid)
{
    RFIDCardStruct * rfidcard = NULL;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<RFIDCardStruct*>::iterator existed = deviceInfo->mRFIDCardList->begin(); existed != deviceInfo->mRFIDCardList->end(); existed++)
    {
        if ( (*existed)->cardid.compare(cardid) == 0 )
        {
            rfidcard = (*existed);
            break;
        }
    }
    return rfidcard;
}

NFCCardStruct * DeviceManager::get_NFCCard(int projectid, const char * cardid)
{
    NFCCardStruct * nfccard = NULL;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NFCCardStruct*>::iterator existed = deviceInfo->mNFCCardList->begin(); existed != deviceInfo->mNFCCardList->end(); existed++)
    {
        if ( (*existed)->cardid.compare(cardid) == 0 )
        {
            nfccard = (*existed);
            break;
        }
    }
    return nfccard;
}

bool DeviceManager::setRFIDCardAlive(int projectid, RFIDCardStruct * card)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    // bool bFound = false;
    for (std::list<RFIDCardStruct*>::iterator existed = deviceInfo->mRFIDCardList->begin(); existed != deviceInfo->mRFIDCardList->end(); existed++)
    {
        if ( card->cardid.compare((*existed)->cardid) == 0 )
        {
            (*existed)->alive++;
            (*existed)->aliveTime = getCurrentTimeInSec();
            return true;
        }
    }

    return false;
}

bool DeviceManager::setNFCCardAlive(int projectid, NFCCardStruct * card)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    // bool bFound = false;
    for (std::list<NFCCardStruct*>::iterator existed = deviceInfo->mNFCCardList->begin(); existed != deviceInfo->mNFCCardList->end(); existed++)
    {
        if ( card->cardid.compare((*existed)->cardid) == 0 )
        {
            (*existed)->alive++;
            (*existed)->aliveTime = getCurrentTimeInSec();
            return true;
        }
    }

    return false;
}

bool DeviceManager::setRFIDCardUser(int projectid, std::string cardid, int userid)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    bool bFound = false;
    for (std::list<RFIDCardStruct*>::iterator existed = deviceInfo->mRFIDCardList->begin(); existed != deviceInfo->mRFIDCardList->end(); existed++)
    {
        if ( (*existed)->cardid.compare(cardid) == 0 )
        {
            (*existed)->userid = userid;

            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        RFIDCardStruct * record = new RFIDCardStruct();
        record->cardid = cardid;
        record->userid = userid;
        record->name = cardid;
        record->type = 1;
        record->maplayer = 0;
        record->unknown = 0;
        record->deviceid = 0;
        record->alive = 0;
        record->datetime = currentDateTimeForSQL();
        record->updatetime = currentDateTimeForSQL();
        record->maingroupid = 1;

        deviceInfo->mRFIDCardList->push_back(record);
    }

    return bFound;
}

bool DeviceManager::setNFCCardUser(int projectid, std::string cardid, int userid)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    bool bFound = false;
    for (std::list<NFCCardStruct*>::iterator existed = deviceInfo->mNFCCardList->begin(); existed != deviceInfo->mNFCCardList->end(); existed++)
    {
        if ( (*existed)->cardid.compare(cardid) == 0 )
        {
            (*existed)->userid = userid;

            bFound = true;
            break;
        }
    }

    if (!bFound)
    {
        NFCCardStruct * record = new NFCCardStruct();
        record->cardid = cardid;
        record->userid = userid;
        record->name = cardid;
        record->type = 1;
        record->maplayer = 0;
        record->unknown = 0;
        record->deviceid = 0;
        record->alive = 0;
        record->datetime = currentDateTimeForSQL();
        record->updatetime = currentDateTimeForSQL();
        record->maingroupid = 1;

        deviceInfo->mNFCCardList->push_back(record);
    }

    return bFound;
}

int DeviceManager::debug_printNodeList(const char * name, std::list<NodeStruct*> * plist)
{
    for (std::list<NodeStruct*>::iterator existed = plist->begin(); existed != plist->end(); existed++)
    {
        vilslog::printf ("DeviceManager::INFO %s(%s) MAC[%s] index[%d] buildingid[%d] buildingfloor[%d] AreaId[%s] AreaName[%s]\n",
            name, (*existed)->nodeid, (*existed)->macaddress, (*existed)->index, (*existed)->buildingid, (*existed)->buildingfloor, (*existed)->AreaId, (*existed)->AreaName);
    }
    return 0;
}

int DeviceManager::getSameIPTagCount(int projectid, const char * IP)
{
    int count = 0;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);
    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if ( strcmp(IP, (*existed)->IP) == 0 )
        {
            count++;
        }
    }

    return count;
}

int DeviceManager::getEmptyNodeId(std::list<NodeStruct*> * plist, const char * Prefix, const char * macaddress, char * outnodeid)
{
    char nodeid[512];
    char extra[512];
    int maclen = (int)strlen(macaddress);

    for(int i=0;i<10;i++)
    {
        memset(extra, 0, 512);
        memcpy(extra, macaddress, i);
        extra[i] = '\0';

        memset(nodeid, 0, 512);
        sprintf(nodeid, "%s%s%c%c%c%c", Prefix, extra, macaddress[maclen-4], macaddress[maclen-3], macaddress[maclen-2], macaddress[maclen-1]);

        bool bFound = false;
        for (std::list<NodeStruct*>::iterator existed = plist->begin(); existed != plist->end(); existed++)
        {
            if ( strcmp(nodeid, (*existed)->nodeid) == 0 )
            {
                bFound = true;
                break;
            }
        }

        if (!bFound)
        {
            break;
        }
    }

    //vilslog::printf("DeviceManager::getEmptyNodeId() nodeid[%s] Prefix[%s] macaddress[%s]\n", nodeid, Prefix, macaddress);
    strcpy(outnodeid, nodeid);

    return 0;
}

int DeviceManager::create_NodePOSInfo(NodeStruct * node, double posX, double posY, double posZ)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(node->projectid);

    bool bFound = false;
    for (std::list<NodePositionInfoStruct*>::iterator NodePos = deviceInfo->mNodePositionInfoList.begin(); NodePos != deviceInfo->mNodePositionInfoList.end(); NodePos++)
    {
        if ( (*NodePos)->nodeid.compare(node->nodeid) == 0 )
        {
            // vilslog::printf("DeviceManager::create_NodePOSInfo() found POSInfoQueue.size()[%d]\n", (*NodePos)->POSInfoQueue.size());

            while( (*NodePos)->POSInfoList.size() > MAX_POSINFO_LIST_SIZE )
            {
                // vilslog::printf("DeviceManager::create_NodePOSInfo() POSInfoQueue.size()[%d] > MAX_POSINFO_QUEUE_SIZE[%d]\n",
                //     (*NodePos)->POSInfoQueue.size(), MAX_POSINFO_QUEUE_SIZE);

                PositionInfoStruct * oldPos = (*NodePos)->POSInfoList.front();
                (*NodePos)->POSInfoList.pop_front();
                delete oldPos;
            }

            PositionInfoStruct * newPos = new PositionInfoStruct();
            newPos->posX = (int)posX;
            newPos->posY = (int)posY;
            newPos->posZ = (int)posZ;
            newPos->updateTimestamp = getCurrentTimeInSec();

            (*NodePos)->nodename = node->nodename;
            (*NodePos)->projectid = node->projectid;
            (*NodePos)->POSInfoList.push_back(newPos);

            bFound = true;
            break;
        }
    }

    if(!bFound)
    {
        // vilslog::printf("DeviceManager::create_NodePOSInfo() create new NodePositionInfoStruct nodeid[%s]\n", nodeid);

        PositionInfoStruct * newPos = new PositionInfoStruct();
        newPos->posX = (int)posX;
        newPos->posY = (int)posY;
        newPos->posZ = (int)posZ;
        newPos->updateTimestamp = getCurrentTimeInSec();

        NodePositionInfoStruct * newInfo = new NodePositionInfoStruct();
        newInfo->nodeid = node->nodeid;
        newInfo->nodename = node->nodename;
        newInfo->projectid = node->projectid;
        newInfo->POSInfoList.push_back(newPos);

        deviceInfo->mNodePositionInfoList.push_back(newInfo);
    }

    return 0;
}

int DeviceManager::create_NodeGPSInfo(int nodetype, NodeStruct * node, double Longitude, double Latitude, const char * timestamp)
{
    // vilslog::printf("DeviceManager::create_NodeGPSInfo() nodeid[%s] Longitude[%f] Latitude[%f]\n", nodeid, Longitude, Latitude);

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(node->projectid);

    bool bFound = false;
    for (std::list<NodeGPSInfoStruct*>::iterator NodeGPS = deviceInfo->mNodeGPSInfoList.begin(); NodeGPS != deviceInfo->mNodeGPSInfoList.end(); NodeGPS++)
    {
        if ( ((*NodeGPS)->nodeid.compare(node->nodeid) == 0) && ((*NodeGPS)->nodetype == nodetype))
        {
            // vilslog::printf("DeviceManager::create_NodeGPSInfo() found GPSInfoQueue.size()[%d]\n", (*NodeGPS)->GPSInfoQueue.size());

            while( (*NodeGPS)->GPSInfoList.size() > MAX_GPSINFO_LIST_SIZE )
            {
                // vilslog::printf("DeviceManager::create_NodeGPSInfo() POSInfoQueue.size()[%d] > MAX_GPSINFO_QUEUE_SIZE[%d]\n",
                //     (*NodeGPS)->GPSInfoQueue.size(), MAX_GPSINFO_QUEUE_SIZE);

                GPSInfoStruct * oldGPS = (*NodeGPS)->GPSInfoList.front();
                (*NodeGPS)->GPSInfoList.pop_front();
                delete oldGPS;
            }

            GPSInfoStruct * newGPS = new GPSInfoStruct();
            newGPS->nodetype = nodetype;
            newGPS->nodeid = node->nodeid;
            newGPS->Longitude = Longitude;
            newGPS->Latitude = Latitude;
            newGPS->updateTimestamp = StringTimeZoneDateTimeTotime_t(timestamp);

            // vilslog::printf("DeviceManager::create_NodeGPSInfo() updateTimestamp[%d]\n", newGPS->updateTimestamp);

            if (newGPS->updateTimestamp == 0)
                newGPS->updateTimestamp = StringDateTimeTotime_t(timestamp);

            (*NodeGPS)->nodename = node->nodename;
            (*NodeGPS)->nodetype = node->nodetype;
            (*NodeGPS)->GPSInfoList.push_back(newGPS);

            bFound = true;
            break;
        }
    }

    if(!bFound)
    {
        //vilslog::printf("DeviceManager::create_NodeGPSInfo() create new NodePositionInfoStruct nodeid[%s]\n", nodeid);

        GPSInfoStruct * newGPS = new GPSInfoStruct();
        newGPS->nodetype = nodetype;
        newGPS->nodeid = node->nodeid;
        newGPS->Longitude = Longitude;
        newGPS->Latitude = Latitude;
        newGPS->updateTimestamp = StringTimeZoneDateTimeTotime_t(timestamp);

        NodeGPSInfoStruct * newInfo = new NodeGPSInfoStruct();
        newInfo->nodeid = node->nodeid;
        newInfo->nodename = node->nodename;
        newInfo->nodetype = node->nodetype;
        newInfo->GPSInfoList.push_back(newGPS);

        deviceInfo->mNodeGPSInfoList.push_back(newInfo);
    }

    return 0;
}

int DeviceManager::get_POSNode_list(std::list<NodeItemStruct * > * plist, int projectid)
{
    int count = 0;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodePositionInfoStruct*>::iterator NodePOS = deviceInfo->mNodePositionInfoList.begin(); NodePOS != deviceInfo->mNodePositionInfoList.end(); NodePOS++)
    {
        if( (*NodePOS)->projectid == projectid || projectid == 0)
        {
            NodeItemStruct * item = new NodeItemStruct();
            item->nodeid = (*NodePOS)->nodeid;
            item->nodename = (*NodePOS)->nodename;
            plist->push_back(item);
            count++;
        }
    }

    return count;
}

std::list<PositionInfoStruct *> * DeviceManager::get_POSNodeData_list(int projectid, std::string nodeid)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodePositionInfoStruct*>::iterator NodePOS = deviceInfo->mNodePositionInfoList.begin(); NodePOS != deviceInfo->mNodePositionInfoList.end(); NodePOS++)
    {
        if( (*NodePOS)->nodeid.compare(nodeid) == 0)
        {
            return &((*NodePOS)->POSInfoList);
        }
    }

    return NULL;
}

int DeviceManager::get_GPSNode_list(std::list<NodeItemStruct * > * plist, int projectid)
{
    int count = 0;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeGPSInfoStruct*>::iterator NodeGPS = deviceInfo->mNodeGPSInfoList.begin(); NodeGPS != deviceInfo->mNodeGPSInfoList.end(); NodeGPS++)
    {
        NodeItemStruct * item = new NodeItemStruct();
        item->nodeid = (*NodeGPS)->nodeid;
        item->nodename = (*NodeGPS)->nodename;
        plist->push_back(item);
        count++;
    }

    return count;
}

std::list<GPSInfoStruct *> * DeviceManager::get_GPSNodeData_list(int projectid, std::string nodeid)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeGPSInfoStruct*>::iterator NodeGPS = deviceInfo->mNodeGPSInfoList.begin(); NodeGPS != deviceInfo->mNodeGPSInfoList.end(); NodeGPS++)
    {
        if( (*NodeGPS)->nodeid.compare(nodeid) == 0)
        {
            return &((*NodeGPS)->GPSInfoList);
        }
    }

    return NULL;
}

GPSInfoStruct * DeviceManager::getTagInfoGPS(int projectid, const char * Tag_macaddress)
{
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<NodeStruct*>::iterator existed = deviceInfo->mTagNodeList->begin(); existed != deviceInfo->mTagNodeList->end(); existed++)
    {
        if( strcmp((*existed)->macaddress, Tag_macaddress) == 0)
        {
            for (std::list<NodeGPSInfoStruct*>::iterator NodeGPS = deviceInfo->mNodeGPSInfoList.begin(); NodeGPS != deviceInfo->mNodeGPSInfoList.end(); NodeGPS++)
            {
                if( (*NodeGPS)->nodeid.compare((*existed)->nodeid) == 0)
                {
                    return (*NodeGPS)->GPSInfoList.back();
                }
            }
        }
    }

    return NULL;
}

bool DeviceManager::checkValidUser(int projectid, const char * account, const char * password)
{
    bool valid = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    std::string accountid = std::string(account);

    for (std::list<UserStruct*>::iterator existed = deviceInfo->mUserList->begin(); existed != deviceInfo->mUserList->end(); existed++)
    {
        //if( (*existed)->account.compare(accountid) == 0)
        if(boost::iequals((*existed)->account, accountid))
        {
            std::string pwd = create_MD5_encrypt_salt(std::string(ERP_MD5_KEY), std::string(password));

            if ( pwd.compare((*existed)->pwd) == 0 )
                return true;
            else
                return false;
        }
    }
    vilslog::printf("DeviceManager::checkValidUser() not found user account[%s] password[%s]\n", account, password);

    // not found user , check database
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        UserStruct * UserData = new UserStruct();
        valid = baseDb->check_valid_User(account, password, UserData);

        vilslog::printf("DeviceManager::checkValidUser() not found user account[%s] password[%s] valid[%d]\n",
            account, password, valid);

        if ( valid )
        {
            deviceInfo->mUserList->push_back(UserData);
        }
        else
        {
            clear_UserStruct(UserData);
            delete UserData;
        }
    }
    return valid;
}

bool DeviceManager::checkValidUser(int projectid, const char * account, const char * password, const char * userid)
{
    bool valid = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    std::string accountid = std::string(account);
    int useridInt = atoi(userid);

    for (std::list<UserStruct*>::iterator existed = deviceInfo->mUserList->begin(); existed != deviceInfo->mUserList->end(); existed++)
    {
        //if( ((*existed)->account.compare(accountid) == 0) && ((*existed)->userid == useridInt))
        if( (boost::iequals((*existed)->account, accountid)) && ((*existed)->userid == useridInt))
        {
            std::string pwd = create_MD5_encrypt_salt(std::string(ERP_MD5_KEY), std::string(password));

            if ( pwd.compare((*existed)->pwd) == 0 )
                return true;
            else
                return false;
        }
    }

    vilslog::printf("DeviceManager::checkValidUser() not found user account[%s] password[%s] userid[%s]\n",
        account, password, userid);

    // not found user , check database
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        UserStruct * UserData = new UserStruct();
        valid = baseDb->check_valid_User(account, password, userid, UserData);

        vilslog::printf("DeviceManager::checkValidUser() not found user account[%s] password[%s] userid[%s] valid[%d]\n",
            account, password, userid, valid);

        if ( valid )
        {
            deviceInfo->mUserList->push_back(UserData);
        }
        else
        {
            clear_UserStruct(UserData);
            delete UserData;
        }
    }
    return valid;
}

// bool DeviceManager::checkValidUser(int projectid, const char * account, const char * password, UserStruct * UserData)
// {
//     bool valid = false;
//
//     MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
//     if (baseDb != NULL)
//     {
//         valid = baseDb->check_valid_User(account, password, UserData);
//     }
//     return valid;
// }

bool DeviceManager::checkValidUser(int projectid, const char * account, const char * password, const char * userid, UserStruct * UserData)
{
    bool valid = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    std::string accountid = std::string(account);
    int useridInt = atoi(userid);

    for (std::list<UserStruct*>::iterator existed = deviceInfo->mUserList->begin(); existed != deviceInfo->mUserList->end(); existed++)
    {
        //if( ((*existed)->account.compare(accountid) == 0) && ((*existed)->userid == useridInt))
        if( boost::iequals((*existed)->account, accountid) )
        {
            vilslog::printf("DeviceManager::checkValidUser() account[%s] password[%s] userid[%s] useridInt[%d]\n", account, password, userid, useridInt);
        }

        if( (boost::iequals((*existed)->account, accountid)) && ((*existed)->userid == useridInt))
        {
            std::string pwd = create_MD5_encrypt_salt(std::string(ERP_MD5_KEY), std::string(password));

            vilslog::printf("DeviceManager::checkValidUser() pwd.compare((*existed)->pwd)[%d]\n", pwd.compare((*existed)->pwd));

            if ( pwd.compare((*existed)->pwd) == 0 )
            {
                valid = true;

                //
                // copy user
                //
                UserData->userid = (*existed)->userid;
                UserData->groupid = (*existed)->groupid;
                UserData->type = (*existed)->type;
                UserData->empid = (*existed)->empid;
                UserData->account = (*existed)->account;
                UserData->name = (*existed)->name;
                UserData->unit = (*existed)->unit;
                UserData->title = (*existed)->title;
                UserData->gender = (*existed)->gender;
                UserData->pwd = (*existed)->pwd;
                UserData->datetime = (*existed)->datetime;

                UserData->maingroups = copyStringList((*existed)->maingroups);
                UserData->datagroups = copyStringList((*existed)->datagroups);
                UserData->subgroups = copyStringList((*existed)->subgroups);
                UserData->rfidcards = copyStringList((*existed)->rfidcards);
                UserData->nfccards = copyStringList((*existed)->nfccards);
                UserData->uwbcards = copyStringList((*existed)->uwbcards);
                UserData->sensors = copyStringList((*existed)->sensors);
                UserData->imageuid = (*existed)->imageuid;

                UserData->maplayer = (*existed)->maplayer;
                UserData->posX = (*existed)->posX;
                UserData->posY = (*existed)->posY;
                UserData->posZ = (*existed)->posZ;
                UserData->Longitude = (*existed)->Longitude;
                UserData->Latitude = (*existed)->Latitude;

            }
            break;
        }
    }

    // MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    // if (baseDb != NULL)
    // {
    //     valid = baseDb->check_valid_User(account, password, userid, UserData);
    // }
    return valid;
}

int DeviceManager::checkValidAccountLogin(int projectid, std::string account, std::string key)
{
    int userid = 0;

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        userid = baseDb->check_Account_Login(account, key);
    }
    return userid;
}

std::string DeviceManager::queryLoginKey(int projectid, std::string account, std::string userid, std::string token)
{
    std::string loginkey = "";

    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        loginkey = baseDb->query_Account_LoginKey(account, userid, token);
    }
    return loginkey;
}

// std::string DeviceManager::queryLoginKey(int projectid, std::string account, std::string token)
// {
//     std::string loginkey = "";
//
//     MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
//     if (baseDb != NULL)
//     {
//         loginkey = baseDb->query_Account_LoginKey(account, token);
//     }
//     return loginkey;
// }
//
// bool DeviceManager::update_user_password(int projectid, const char * account, const char * password)
// {
//     bool valid = false;
//
//     MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
//     if (baseDb != NULL)
//     {
//         valid = baseDb->update_user_password(account, password);
//     }
//     return valid;
// }

bool DeviceManager::update_user_password(int projectid, std::string accountid, int userid, std::string password)
{
    bool valid = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<UserStruct*>::iterator existed = deviceInfo->mUserList->begin(); existed != deviceInfo->mUserList->end(); existed++)
    {
        // if( ((*existed)->account.compare(accountid) == 0) && ((*existed)->userid == userid))
        if( (boost::iequals((*existed)->account, accountid)) && ((*existed)->userid == userid))
        {
            std::string pwd = create_MD5_encrypt_salt(std::string(ERP_MD5_KEY), password);

            (*existed)->pwd = pwd;

            valid = true;
            break;
        }
    }

    // MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    // if (baseDb != NULL)
    // {
    //     valid = baseDb->update_user_password(account, userid, password);
    // }
    return valid;
}

UserEmployeeStruct * DeviceManager::getEmployee(int projectid, const char * empid)
{
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        UserEmployeeStruct * user = baseDb->getEmployee(empid);
        return user;
    }
    else
    {
        return NULL;
    }
}

UserStruct * DeviceManager::getUser(int projectid, int userid)
{
    UserStruct * userRet = NULL;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<UserStruct*>::iterator existed = deviceInfo->mUserList->begin(); existed != deviceInfo->mUserList->end(); existed++)
    {
        if((*existed)->userid == userid)
        {
            return (*existed);
        }
    }
    return userRet;
}

bool DeviceManager::setUserInfoArea(int projectid, int userid, std::string areaid, std::string areaname)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<UserStruct*>::iterator existed = deviceInfo->mUserList->begin(); existed != deviceInfo->mUserList->end(); existed++)
    {
        if((*existed)->userid == userid)
        {
            (*existed)->AreaId = areaid;
            (*existed)->AreaName = areaname;
            bFound = true;
            break;
        }
    }
    bFound = true;

    return bFound;
}

bool DeviceManager::setUserInfoPos(int projectid, int userid, int posX, int posY, int posZ)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<UserStruct*>::iterator existed = deviceInfo->mUserList->begin(); existed != deviceInfo->mUserList->end(); existed++)
    {
        if((*existed)->userid == userid)
        {
            (*existed)->posX = posX;
            (*existed)->posY = posY;
            (*existed)->posZ = posZ;
            bFound = true;
            break;
        }
    }
    bFound = true;

    return bFound;
}

bool DeviceManager::setUserInfoArea0D(int projectid, int userid, string areaid, int AreaType0D)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<UserStruct*>::iterator existed = deviceInfo->mUserList->begin(); existed != deviceInfo->mUserList->end(); existed++)
    {
        if((*existed)->userid == userid)
        {
            (*existed)->AreaId0D = areaid;
            (*existed)->AreaType0D = AreaType0D; // 0:normal 1:0D room  2:danger area
            bFound = true;
            break;
        }
    }
    bFound = true;

    return bFound;
}

bool DeviceManager::setUserInfoAreaType5(int projectid, int userid, std::string areaid, std::string areaname)
{
    bool bFound = false;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<UserStruct*>::iterator existed = deviceInfo->mUserList->begin(); existed != deviceInfo->mUserList->end(); existed++)
    {
        if((*existed)->userid == userid)
        {
            (*existed)->AreaType5Id = areaid;
            (*existed)->AreaType5Name = areaname;
            bFound = true;
            break;
        }
    }
    bFound = true;

    return bFound;
}

bool DeviceManager::setUserRollCallNotInArea(int projectid, int userid, std::string areaid)
{
    bool bFound = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<UserStruct*>::iterator existed = deviceInfo->mUserList->begin(); existed != deviceInfo->mUserList->end(); existed++)
    {
        if((*existed)->userid == userid)
        {
            (*existed)->exclude_rollcall_AreaId = areaid;

            bFound = true;
            break;
        }
    }
    return bFound;
}

bool DeviceManager::setUserInfoMaplayer(int projectid, int userid, int maplayerid)
{
    bool bFound = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<UserStruct*>::iterator existed = deviceInfo->mUserList->begin(); existed != deviceInfo->mUserList->end(); existed++)
    {
        if((*existed)->userid == userid)
        {
            (*existed)->maplayer = maplayerid;

            bFound = true;
            break;
        }
    }
    return bFound;
}
// RangingResultType: -1:unknown 0:0D 1:1D 2:2D 3:3D
bool DeviceManager::setUserRangingResultType(int projectid, int userid, int RangingResultType)
{
    bool bFound = false;

    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<UserStruct*>::iterator existed = deviceInfo->mUserList->begin(); existed != deviceInfo->mUserList->end(); existed++)
    {
        if((*existed)->userid == userid)
        {
            (*existed)->RangingResultType = RangingResultType;

            bFound = true;
            break;
        }
    }
    return bFound;
}

TargetTopicStruct * DeviceManager::getUserTopic(int projectid, int userid, std::string accountid)
{
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        TargetTopicStruct * userTopic = baseDb->get_UserTargetTopic(userid, accountid);

        return userTopic;
    }
    else
    {
        return NULL;
    }
}

std::string DeviceManager::getUserName(int projectid, int userid)
{
    std::string username;
    DeviceInfoListStruct * deviceInfo = getDeviceInfoList(projectid);

    for (std::list<UserStruct*>::iterator existed = deviceInfo->mUserList->begin(); existed != deviceInfo->mUserList->end(); existed++)
    {
        if((*existed)->userid == userid)
        {
            username = (*existed)->name;
            break;
        }
    }
    return username;
}

std::list<EmployeeEventReportStruct*> * DeviceManager::getEmployeeEventReport(int projectid, std::string empid, std::string report_start_date, std::string report_end_date)
{
    MySQLDBAccess * baseDb = mDbManager->getBaseDB(projectid);
    if (baseDb != NULL)
    {
        std::list<EmployeeEventReportStruct*> * reportlist = baseDb->getEmployeeEventReport(empid, report_start_date, report_end_date);
        return reportlist;
    }
    else
    {
        return NULL;
    }
}
