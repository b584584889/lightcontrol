/*
 * DataParser.cpp
 *
 *  Created on: 2017/02/21
 *      Author: ikki
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "string.h"

#include <sys/time.h>
#include <time.h>

#include "../common.h"
#include "../json/json.h"

#include "DataParser.h"

DataParser::DataParser(wqueue<DataItem*> & PosCalJobQueue, wqueue<SubscribeDataItem*> & pWebSocketqueue, wqueue<NotificationItem*> & NotifiactionQueue, wqueue<WorkItem*> & MySQLJobQueue, DeviceManager * pDevManager)
    :mPosCalJobQueue(PosCalJobQueue), mWebSocketqueue(pWebSocketqueue), mNotifiactionQueue(NotifiactionQueue), mMySQLJobQueue(MySQLJobQueue), mDeviceManager(pDevManager)
{
}

DataParser::~DataParser()
{
}

int DataParser::processData(DataItem * item)
{
    bool vailed_ranging = false;
    int VaildCount = 0;

    if ( strcmp(item->getType(), "join") == 0 )
    {
        do_join(item);
    }
    else
    if ( strcmp(item->getType(), "alive") == 0 )
    {
        do_alive(item);
    }
    // else
    // if ( strcmp(item->getType(), "ranging") == 0 )
    // {
    //     vailed_ranging = true;
    //     VaildCount = do_ranging(item);
    // }
    // else
    // if ( strcmp(item->getType(), "rangingdiag") == 0 )
    // {
    //     vailed_ranging = true;
    //     VaildCount = do_rangingdiag(item);
    // }
    else
    if ( strcmp(item->getType(), "rangingdiag_1") == 0 )
    {
        vailed_ranging = true;
        VaildCount = do_rangingdiag_1(item);
    }
    else
    if ( strcmp(item->getType(), "list") == 0 )
    {
        //printf ("DataParser::processData() list\n");
        do_list(item);
    }
    else
    if ( strcmp(item->getType(), "alert") == 0 )
    {
        //printf ("DataParser::processData() alert\n");
        do_alert(item);
    }
    else
    if ( strcmp(item->getType(), "cancel") == 0 )
    {
        //printf ("DataParser::processData() alert\n");
        do_cancel(item);
    }

    if (vailed_ranging)
        return VaildCount;
    else
        return 0;
}

int DataParser::do_join(DataItem * item)
{
    int nodetype = item->getNodeType();
    int projectid = item->getProjectID();

    switch(nodetype)
    {
        default:
        break;
        case NODE_TYPE_ANCHOR: // Anchor
            //printf("DataParser::do_join() NODE_TYPE_ANCHOR [%s] [%s]\n", item->getmacaddress(), item->getVersion());
            // update index
            mDeviceManager->setAnchorIndex(projectid, item->getmacaddress(), -1, item->getVersion());
            mDeviceManager->setAnchorAlive(projectid, item->getmacaddress());

            // alert alive
            //mPosCal->setAnchorAlive(newDevice->index);
            //mPosCal->setCoordinatorAliveAll();
        break;
        case NODE_TYPE_TAG: // Tag
            //vilslog::printf("DataParser::do_join() NODE_TYPE_TAG [%s] [%s]\n", item->getmacaddress(), item->getVersion());
            // update index
            //num = mPosCal->get_Tag_num();
            if(mDeviceManager->setTagIndex(projectid, item->getmacaddress(), -1, item->getVersion()))
            {
                //int tag_index = mDeviceManager->getTagIndex(item->getmacaddress());
                //mPosCal->setTagJoin(tag_index);
            }


        break;
        case NODE_TYPE_COORD: // Coordinator
            //printf("DataParser::do_join() NODE_TYPE_COORD [%s] [%s]\n", item->getmacaddress(), item->getVersion());
            mDeviceManager->setCoordPanid(projectid, item->getmacaddress(), "", item->getVersion());

            // alert alive
            mDeviceManager->setCoordinatorAlive(projectid, item->getmacaddress());
        break;
        case NODE_TYPE_LOCATOR: // Locator
            vilslog::printf("DataParser::do_join() NODE_TYPE_LOCATOR [%s] [%s]\n", item->getmacaddress(), item->getVersion());
        break;
    }


    return 0;
}

int DataParser::do_alive(DataItem * item)
{
    int nodetype = item->getNodeType();
    int projectid = item->getProjectID();

    switch(nodetype)
    {
        default:
        break;
        case NODE_TYPE_ANCHOR: // Anchor
            //printf("DataParser::do_alive() NODE_TYPE_ANCHOR\n");
            // alert alive
            mDeviceManager->setAnchorAlive(projectid, item->getmacaddress());
            mDeviceManager->setAnchorInfoPANID(projectid, item->getmacaddress(), item->getPANID());


            //vilslog::printf(" DataParser::do_alive() NODE_TYPE_ANCHOR macaddress[%s] PANID[%s]\n", item->getmacaddress(), item->getPANID());

            //mPosCal->setCoordinatorAliveAll();
        break;
        case NODE_TYPE_TAG: // Tag
            //printf("DataParser::do_alive() NODE_TYPE_TAG\n");
            // alert alive
            // mDeviceManager->setTagAlive(item->getmacaddress(), item->getVoltage(), item->getTemperature());
            //mPosCal->setCoordinatorAliveAll();
        break;
        case NODE_TYPE_COORD: // Coordinator
            //printf("DataParser::do_alive() NODE_TYPE_COORD\n");
            // alert alive
            mDeviceManager->setCoordinatorAlive(projectid, item->getmacaddress());
        break;
        case NODE_TYPE_LOCATOR: // Locator
            //printf("DataParser::do_alive() NODE_TYPE_LOCATOR [%s]\n", item->getmacaddress());
            //mDeviceManager->setLocatorAlive(item->getmacaddress());
        break;
    }
    return 0;
}

// int DataParser::do_ranging(DataItem * item)
// {
//     int IMU[6];
//     item->getTagIMU(IMU);
//
//     //
//     // send to PosCalculate
//     //
//     DataItem * senditem = new DataItem("ranging", item->getProjectID(), item->getAnchorIdx(), item->getTagIdx(), item->getRangingIdx(), item->getDistance(), IMU);
//     mPosCalJobQueue.add(senditem);
//
//     //mPosCal->setRanging(item->getAnchorIdx(), item->getTagIdx(), item->getRangingIdx(), item->getDistance(), IMU);
//     return 1;
// }

// int DataParser::do_rangingdiag(DataItem * item)
// {
//     int IMU[6];
//     item->getTagIMU(IMU);
//
//     //
//     // set to PosCalculate
//     //
//     DataItem * senditem = new DataItem("rangingdiag", item->getProjectID(), item->getAnchorIdx(), item->getTagIdx(), item->getRangingIdx(), item->getDistance(), IMU,
//             item->getTagPollCnt(), item->getRangingAnchorCnt(), item->getTagFinalRecvdBm(), item->getTagFirstPathdBm(),
//             item->getAnchorRecvdBm(), item->getAnchorFirstPathdBm(), item->getRangingCount(), item->getRangingFailCount(),
//             item->getDataRate(), item->getIPAddress());
//
//     mPosCalJobQueue.add(senditem);
//     return 1;
// }

int DataParser::do_rangingdiag_1(DataItem * item)
{
    int IMU[6];
    item->getTagIMU(IMU);

    //vilslog::printf ("DataParser::do_rangingdiag_1() projectid[%d]\n", item->getProjectID());

    //
    // set to PosCalculate
    //
    DataItem * senditem = new DataItem("rangingdiag_1", item->getProjectID(), item->getAnchorMac(), item->getTagMac(), item->getRangingIdx(), item->getDistance(), IMU,
            item->getTagPollCnt(), item->getRangingAnchorCnt(), item->getTagFinalRecvdBm(), item->getTagFirstPathdBm(),
            item->getAnchorRecvdBm(), item->getAnchorFirstPathdBm(), item->getRangingCount(), item->getRangingFailCount(),
            item->getDataRate(), item->getIPAddress());

    mPosCalJobQueue.add(senditem);
    return 1;
}

int DataParser::do_list(DataItem * item)
{
    int succces_count = 0;
    int projectid = item->getProjectID();
    std::list<NodeStruct*> * nodeList = item->get_Node_list();

    //printf ("DataParser::do_list() nodelist size[%d]\n", nodeList->size());

    for (std::list<NodeStruct*>::iterator node = nodeList->begin(); node != nodeList->end(); node++)
    {
        //printf ("DataParser::do_list() nodetype[%d] mac[%s] index[%d] version[%s]\n",
        //    (*node)->nodetype, (*node)->macaddress, (*node)->index, (*node)->version);

        if ( (*node)->nodetype == NODE_TYPE_ANCHOR )
        {
            //
            // check if have same index but different MAC address.
            //
            bool valid = mDeviceManager->checkAnchorValidNode(projectid, (*node)->macaddress, (*node)->index);

            if (valid)
            {
                //printf ("DataParser::do_list() setAnchorInfo[%s]\n", (*node)->macaddress);
                //mDeviceManager->setAnchorInfo(projectid, (*node)->macaddress, "", -1, -100, -100, "", (*node)->index, false, 0, 0, 0, (*node)->version);
                mDeviceManager->setAnchorInfo(projectid, (*node)->macaddress, "", -1, -100, -100, "", (*node)->index, false, 0, 0, 0, "");
                if ((*node)->alive > 0)
                {
                    // we does not set 0 here, maybe this list is not hold all device info.
                    mDeviceManager->setAnchorAliveCount(projectid, (*node)->macaddress, (*node)->alive);
                }
            }
        }
        else
        if ( (*node)->nodetype == NODE_TYPE_TAG )
        {
            bool valid = mDeviceManager->checkTagValidNode(projectid, (*node)->macaddress, (*node)->index);
            if (valid)
            {
                //printf ("DataParser::do_list() setTagInfo[%s] rangingList[%s]\n", (*node)->macaddress, (*node)->rangingList);

                mDeviceManager->setTagInfo(projectid, (*node)->macaddress, "", -1, -100, -100, "", (*node)->index, false, 0, 0, 0, (*node)->version);
                //mDeviceManager->setTagInfo(projectid, (*node)->macaddress, "", -1, -100, -100, "", (*node)->index, false, 0, 0, 0, "");
                mDeviceManager->setTagRangingList(projectid, (*node)->macaddress, (*node)->rangingList);
                mDeviceManager->setTagInfoPANID(projectid, (*node)->macaddress, (*node)->panid);

                if ((*node)->alive > 0)
                {
                    // we does not set 0 here, maybe this list is not hold all device info.
                    mDeviceManager->setTagAliveCount(projectid, (*node)->macaddress, (*node)->alive);
                    mDeviceManager->setTagAlive(projectid, (*node)->macaddress, (*node)->voltage, (*node)->temperature);
                }
            }
            else
            {
                if ((*node)->alive > 0)
                {
                    printf ("DataParser::do_list() setTagInfo[%s] valid[%d] alive[%d]\n", (*node)->macaddress, valid, (*node)->alive);

                    mDeviceManager->setTagInfo(projectid, (*node)->macaddress, "", -1, -100, -100, "", -1, false, 0, 0, 0, "");
                    mDeviceManager->setTagRangingList(projectid, (*node)->macaddress, (*node)->rangingList);
                    mDeviceManager->setTagInfoPANID(projectid, (*node)->macaddress, (*node)->panid);
                    // we does not set 0 here, maybe this list is not hold all device info.
                    mDeviceManager->setTagAliveCount(projectid, (*node)->macaddress, (*node)->alive);
                    mDeviceManager->setTagAlive(projectid, (*node)->macaddress, (*node)->voltage, (*node)->temperature);
                }
            }
        }
        else
        if ( (*node)->nodetype == NODE_TYPE_COORD )
        {
            bool valid = mDeviceManager->checkCoordValidNode(projectid, (*node)->macaddress, (*node)->index);
            if (valid)
            {
                // vilslog::printf ("DataParser::do_list() setCoordInfo projectid[%d] macaddress[%s] IP[%s]\n", projectid, (*node)->macaddress, (*node)->IP);
                //mDeviceManager->setCoordInfo(projectid, (*node)->macaddress, "", -1, -100, -100, "", (*node)->index, false, 0, 0, 0, (*node)->version);
                mDeviceManager->setCoordInfo(projectid, (*node)->macaddress, "", -1, -100, -100, "", (*node)->index, false, 0, 0, 0, "");
                mDeviceManager->setCoordinatorInfo(projectid, (*node)->macaddress, (*node)->IP, (*node)->AppVersion, (*node)->time, (*node)->panid);

                if ((*node)->alive > 0)
                {
                    // we does not set 0 here, maybe this list is not hold all device info.
                    mDeviceManager->setCoordinatorAliveCount(projectid, (*node)->macaddress, (*node)->alive);
                }
            }
        }
        else
        if ( (*node)->nodetype == NODE_TYPE_LOCATOR )
        {
            bool valid = mDeviceManager->checkLocatorValidNode(projectid, (*node)->macaddress, (*node)->index);
            if (valid)
            {
                mDeviceManager->setLocatorInfo(projectid, (*node)->macaddress, "", -1, -100, -100, "", (*node)->index, false, 0, 0, 0, (*node)->version);

                if ((*node)->alive > 0)
                {
                    // we does not set 0 here, maybe this list is not hold all device info.
                    mDeviceManager->setLocatorAliveCount(projectid, (*node)->macaddress, (*node)->alive);
                }
            }
        }
        succces_count++;
    }

    return succces_count;
}

int DataParser::do_alert(DataItem * item)
{
    int nodetype = item->getNodeType();
    int projectid = item->getProjectID();
    const char * macaddress = item->getmacaddress();
    const char * IPAddress = item->getIPAddress();
    std::string CoordinatorId = mDeviceManager->getCoordinatorIdFromIP(projectid, IPAddress);

    vilslog::printf ("DataParser::do_alert() nodetype[%d] macaddress[%s] IPAddress[%s] CoordinatorId[%s]\n",
        nodetype, macaddress, IPAddress, CoordinatorId.c_str());

    if ( nodetype == NODE_TYPE_TAG )
    {
        projectid = mDeviceManager->getTagProjectId(macaddress);
        mDeviceManager->setTagInfoIP(projectid, macaddress, IPAddress);
    }
    else
    if ( nodetype == NODE_TYPE_LOCATOR )
    {
        projectid = mDeviceManager->getLocatorProjectId(macaddress);
    }

    // check previous alert time
    time_t diffTime = mDeviceManager->getTagAlertDiffTimestamp(projectid, macaddress);
    int alertTimeDiff = (int)diffTime; // in ms

    vilslog::printf ("DataParser::do_alert() alertTimeDiff[%d]\n", alertTimeDiff);

    if ( alertTimeDiff > 10000 )
    {
        mDeviceManager->setNodeAlertByType(projectid, macaddress, nodetype);

        int maingroupid = 1;
        std::string nodeid = "";
        std::string nodename = "";
        int functiontype = TAG_PUSHNOTIFY_FUNCTION_2;
        if ( nodetype == NODE_TYPE_TAG )
        {
            //projectid = mDeviceManager->getTagProjectId(macaddress);
            nodeid = mDeviceManager->getTagId(projectid, macaddress);
            nodename = mDeviceManager->getTagName(projectid, nodeid.c_str());
            functiontype = TAG_PUSHNOTIFY_FUNCTION_2;
        }
        else
        if ( nodetype == NODE_TYPE_LOCATOR )
        {
            //projectid = mDeviceManager->getLocatorProjectId(macaddress);
            nodeid = mDeviceManager->getLocatorId(projectid, macaddress);
            nodename = mDeviceManager->getLocatorName(projectid, nodeid.c_str());
            functiontype = LOCATOR_PUSHNOTIFY_FUNCTION_2;
        }

        vilslog::printf ("DataParser::do_alert() projectid[%d] nodetype[%d] macaddress[%s]\n", projectid, nodetype, macaddress);

        //std::string nodeid = mDeviceManager->getTagId(macaddress);

        std::string eventjson = convert_to_json_str(projectid, 9999, nodeid, "Alert", "", "0", CoordinatorId);

        WorkItem * witem = new WorkItem("alert", projectid, nodetype, macaddress, eventjson.c_str());
        witem->setMaingroupid(maingroupid);
        mMySQLJobQueue.add(witem);

        std::string message = nodename;
        message.append(" 按下緊急按鈕(UWB)");

        NotificationItem * item = new NotificationItem(functiontype, projectid, nodetype, nodeid, nodename, "0", "1", macaddress, "SMIMSVILS 緊急求助通知", message);
        mNotifiactionQueue.add(item);
    }

    return 0;
}

int DataParser::do_cancel(DataItem * item)
{
    int nodetype = item->getNodeType();
    int projectid = item->getProjectID();
    int maingroupid = 1;

    const char * macaddress = item->getmacaddress();
    const char * IPAddress = item->getIPAddress();
    std::string TagId = mDeviceManager->getTagId(projectid, macaddress);

    vilslog::printf ("DataParser::do_cancel() projectid[%d] nodetype[%d] macaddress[%s] IPAddress[%s]\n", projectid, nodetype, macaddress, IPAddress);

    if ( nodetype == NODE_TYPE_TAG )
    {
        mDeviceManager->setTagInfoIP(projectid, macaddress, IPAddress);
    }


    //============================================================
    // 判斷 透過同一個Coorinator發出的SOS所有tag都取消.
    //
    std::list<std::string> * noideidlist = new std::list<std::string>();
    mDeviceManager->getTagAlertListFromSameIP(projectid, IPAddress, noideidlist);

    vilslog::printf ("DataParser::do_cancel() noideidlist->size()[%d]\n", (int)noideidlist->size());
    for (std::list<std::string>::iterator nodeid = noideidlist->begin(); nodeid != noideidlist->end(); nodeid++)
    {
        int alertType = 1; //1:人員 2:固定裝置
        int alertAction = 4;// 1:In 2:Out 3:alert 4:cancel
        SubscribeDataItem * sitem = new SubscribeDataItem("event", projectid, 999, (*nodeid).c_str(), alertType, alertAction);
        mWebSocketqueue.add(sitem);
        vilslog::printf("DataParser::do_cancel() cancel nodeid[%s]\n", (*nodeid).c_str());
        mDeviceManager->clearTagAlertById(projectid, (*nodeid).c_str());
    }

    noideidlist->clear();
    delete noideidlist;
    //============================================================


    std::string CoordinatorId = mDeviceManager->getCoordinatorIdFromIP(projectid, IPAddress);
    //std::string empid = mDeviceManager->getTagempid(projectid, macaddress);
    int userid = mDeviceManager->getTaguserid(projectid, macaddress);
    std::string empid = std::to_string(userid);

    // 工作人員 simple tag 按下取消鈕
    std::string eventjson = convert_to_json_str(projectid, 9999, TagId, "Cancel", "", "0", CoordinatorId);
    WorkItem * witem = new WorkItem("alertcancel", projectid, NODE_TYPE_TAG, macaddress, eventjson.c_str(), empid);
    witem->setMaingroupid(maingroupid);
    mMySQLJobQueue.add(witem);

    //============================================================
    // 判斷 10 公尺內所有 tag 的 SOS 都取消.
    //
    if ( CoordinatorId.length() > 0 )
    {
        double Coord[3];
        mDeviceManager->getCoordinatorCoord(projectid, CoordinatorId.c_str(), Coord);

        vilslog::printf("DataParser::do_cancel() CoordinatorId[%s] Coord[%d, %d, %d]\n", CoordinatorId.c_str(), (int)Coord[0], (int)Coord[1], (int)Coord[2]);

        std::list<NodeStruct*> * tagNodeList = mDeviceManager->get_Tag_list(projectid);
        for (std::list<NodeStruct*>::iterator existed = tagNodeList->begin(); existed != tagNodeList->end(); existed++)
        {
            if ( (*existed)->alert0D > 0 || (*existed)->alert > 0)
            {
                int distance = sqrt(pow((*existed)->posX - Coord[0], 2) + pow((*existed)->posY - Coord[1], 2));
                if ( distance < 10000 ) // 10 meters
                {
                    int alertType = 1; //1:人員 2:固定裝置
                    int alertAction = 4;// 1:In 2:Out 3:alert 4:cancel
                    SubscribeDataItem * sitem = new SubscribeDataItem("event", projectid, 999, (*existed)->nodeid, alertType, alertAction);
                    mWebSocketqueue.add(sitem);

                    if ( (*existed)->alert > 0 )
                    {
                        //================================================================
                        // 設定工作人員 simple tag 在 Coordinator 按下取消鈕
                        std::string CoordinatorId = mDeviceManager->getCoordinatorIdFromIP(projectid, (*existed)->IP);
                        std::string eventjson = convert_to_json_str(projectid, 9999, TagId, "Cancel", "", "0", CoordinatorId);
                        WorkItem * witem = new WorkItem("alertcancel", projectid, NODE_TYPE_TAG, macaddress, eventjson.c_str(), empid);
                        witem->setMaingroupid((*existed)->maingroupid);
                        mMySQLJobQueue.add(witem);
                        //================================================================
                    }
                    else
                    {
                        // Find tag alert locator info
                        std::string curLocatorID =  mDeviceManager->getLocatorIdByLoRAmacaddress(projectid, (*existed)->LoRAMacaddress);
                        std::string eventjson = convert_to_json_str(projectid, 9999, (*existed)->nodeid, "Cancel", "", "0", curLocatorID);
                        WorkItem * witem = new WorkItem("alertcancel", projectid, NODE_TYPE_TAG, macaddress, eventjson.c_str(), empid);
                        witem->setMaingroupid((*existed)->maingroupid);
                        mMySQLJobQueue.add(witem);
                    }

                    vilslog::printf("DataParser::do_cancel() cancel distance[%d] nodeid[%s]\n", distance, (*existed)->nodeid);
                    mDeviceManager->clearTagAlertById(projectid, (*existed)->nodeid);
                    mDeviceManager->clearNodeAlertByType(projectid, (*existed)->macaddress, NODE_TYPE_TAG);
                }
            }
        }
    }
    //============================================================

    return 0;
}
