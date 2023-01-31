/*
 *  CenterOutgoingService.cpp
 *
 *  Created on: 2017/02/21
 *      Author: ikki
 */

#include "CenterOutgoingService.h"

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <string.h>
#include <iostream>
#include <list>
#include <ifaddrs.h>

using namespace std;

CenterOutgoingService::CenterOutgoingService(wqueue<DataItem*> & queue, DBManager * dbManager)
    : m_queue(queue), mDbManager(dbManager)
{
    //mUdpClientList = new std::list<UDPClient *>();
}

bool CenterOutgoingService::status()
{
    return mUdpInitial;

    // if (m_systemConfig->UsingProtocol == UDP)
    //     return mUdpInitial;
    // if (m_systemConfig->UsingProtocol == MULTICAST)
    //     return mMcastInitial;
    // if (m_systemConfig->UsingProtocol == TCP)
    //     return mTcpInitial;
    //return false;
}

std::string CenterOutgoingService::statusString()
{
    return mStatus;
}

void CenterOutgoingService::updateProject()
{
    mUpdateProject = true;
}

void * CenterOutgoingService::run()
{
    while(1)
    {
        try
        {
            runloop();
            sleep(10);
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("CenterOutgoingService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mUdpInitial = false;
            mStatus = "CenterOutgoingService()::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void CenterOutgoingService::runloop()
{
    mUdpInitial = false;

    mPrjidList = mDbManager->getAllProjectConfig();

    getMyCenterNetworkInfo(mCenterNetworkInfoList);

    vilslog::printf ("CenterOutgoingService()::run() mCenterNetworkInfoList.length()[%lu]\n", mCenterNetworkInfoList.size());

    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);

        //
        // check same port
        //
        bool bFound = false;
        for (std::list<UDPClient *>::iterator udpclient = mUdpClientList.begin(); udpclient != mUdpClientList.end(); udpclient++)
        {
            int usedPort = (*udpclient)->getPort();
            if ( vilssysConfig->coord_udp_port == usedPort )
            {
                bFound = true;
                break;
            }
        }

        if (bFound)
        {
            continue;
        }

        for (std::list<NetworkInfoStruct * >::iterator netif = mCenterNetworkInfoList.begin(); netif != mCenterNetworkInfoList.end(); netif++)
        {
            UDPClient * udpClient = new UDPClient((*netif)->interface, (*netif)->ip);
            bool udpInitial = udpClient->init(vilssysConfig->coord_udp_port, (*netif)->broadcastAddr.c_str());
            if ( udpInitial )
            {
                vilslog::printf ("CenterOutgoingService()::run() Initial UDPClient successfully! name[%s] IP[%s] port[%d] broadcastAddr[%s]\n",
                    (*netif)->interface.c_str(), (*netif)->ip.c_str(), vilssysConfig->coord_udp_port, (*netif)->broadcastAddr.c_str());


                mUdpClientList.push_back(udpClient);
            }
        }
    }

    if (mUdpClientList.size() == 0)
    {
        mStatus = "CenterOutgoingService()::run() Initial CenterOutgoingService failed!";
        vilslog::printf ("CenterOutgoingService() Initial CenterOutgoingService failed!\n");
        return;
    }
    else
    {
        mStatus = "CenterOutgoingService()::run() Initial CenterOutgoingService successfully!";
        vilslog::printf ("CenterOutgoingService()::run() Initial CenterOutgoingService successfully!\n");
    }
    mUdpInitial = true;

    wait_incoming_msg();

}

int CenterOutgoingService::wait_incoming_msg()
{
    // Remove 1 item at a time and process it. Blocks if no items are
    char type[64];
    long long start1 = getSystemTime();
    long long queryBlackListTime = 0;
    long long queryWhiteListTime = 0;
    long long queryValidrangTime = 0;
    long long updatePrjIdTime = 0;

    // available to process.
    while (1)
    {
        //printf("thread %lu, - waiting for item...\n", (long unsigned int)self());
        //DataItem * item = m_queue.remove();
        DataItem * item = m_queue.removetimed(1000);

        //printf("thread %lu, loop %d - item: Type - %s, NodeId - %s\n",
        //      (long unsigned int)self(), i, item->getType(), item->getNodeId());

        if ( item != NULL )
        {
            sprintf(type, "%s", item->getType());

            if ( strcmp(type, "list") == 0)
                send_list(item, false);
            else
            if ( strcmp(type, "alivetag") == 0)
                send_list(item, true);
            else
            // if ( strcmp(type, "validranginglist") == 0)
            //     send_validList(item);
            // else
            if ( strcmp(type, "tagsampletime") == 0)
                send_tagsampletime(item);
            else
            if ( strcmp(type, "coordinatorreset") == 0)
                send_coordinatorreset(item);
            else
            if ( strcmp(type, "m0reset") == 0)
                send_m0reset(item);
            else
            if ( strcmp(type, "setvalidrang") == 0)
                send_setvalidrang(item);
            else
            if ( strcmp(type, "setblacklist") == 0)
                send_blacklist(item);
            else
            if ( strcmp(type, "setwhitelist") == 0)
                send_whitelist(item);
            else
            if ( strcmp(type, "systemtime") == 0)
                send_systemtime(item);
            else
            if ( strcmp(type, "queryvalidrang") == 0)
                query_validrang(item);
            else
            if ( strcmp(type, "queryblacklist") == 0)
                query_blacklist(item);
            else
            if ( strcmp(type, "querywhitelist") == 0)
                query_whitelist(item);

            /*
            if ( strcmp(type, "alive") == 0)
                send_alive(item);
            else
            if ( strcmp(type, "updateS") == 0)
                send_updateS(item);
            else
            if ( strcmp(type, "updateM") == 0)
                send_updateM(item);
                    */
            //printf("thread %lu, loop %d - got one item\n", (long unsigned int)self(), i);
            delete item;
        }

        long long end1 = getSystemTime();
        long long difft = end1 - start1;

        queryBlackListTime += difft;
        queryWhiteListTime += difft;
        queryValidrangTime += difft;
        updatePrjIdTime += difft;

        // if ( difft >= m_systemConfig.center_broadcast_time )
        // {
        //     //send_MYIP();
        //
        // }

        if ( queryBlackListTime >= 60000 )
        {
            query_blacklist();
            queryBlackListTime = 0;
        }

        if ( queryWhiteListTime >= 70000 )
        {
            query_whitelist();
            queryWhiteListTime = 0;
        }

        if ( queryValidrangTime >= 80000 )
        {
            query_validrang();
            queryValidrangTime = 0;
        }

        if (updatePrjIdTime > 3600000 || mUpdateProject)
        {
            mPrjidList->clear();
            delete mPrjidList;
            mPrjidList = mDbManager->getAllProjectConfig();

            updatePrjIdTime = 0;
            mUpdateProject = false;
        }

        start1 = end1;
        //usleep(10000);//wait 10 ms
    }

    return 0;
}

int CenterOutgoingService::send_list(DataItem * item, bool bChekAlive)
{
    int ret = 0;
    char nodemsg[256];
    //static int list_count = 0;

    /*
    "nodelist" : [
        {"nodetype":"0","mac":"1234567890","index":"0","version":"2.1.3"}
        ,{"nodetype":"0","mac":"1234567890","index":"1","version":"2.1.3"}
    ]
    */
    //
    // send 10 node each time.
    //

    memset(mBuf, 0, MAXUDPPACKET);
    sprintf(mBuf, "{\"%s\":\"%s\",\"%s\":\"%d\",\"%s\":[", "type",
            item->getType(), "nodetype", item->getNodeType(), "nodelist");
    //sprintf(msg, "{\"%s\":\"%s\",\"%s\":\"%d\"}\r\n", "type", item->getType(), "nodetype", item->getNodeType());
    int send_cnt = 0;
    int not_send_cnt = 0;
    std::list<NodeStruct*> * nodeList = item->get_Node_list();


    // vilslog::printf ("CenterOutgoingService()::send_list() item->getType[%s] NodeType[%d] nodeList->size[%lu] bChekAlive[%d]\n",
    //                             item->getType(), item->getNodeType(), nodeList->size(), bChekAlive);


    for (std::list<NodeStruct*>::iterator node = nodeList->begin(); node != nodeList->end(); node++)
    {
        // vilslog::printf ("CenterOutgoingService()::send_list() nodetype[%d] (*node)->alive[%d] index[%d]\n",
        //     (*node)->nodetype, (*node)->alive, (*node)->index);

        if (bChekAlive && (*node)->alive <= 0)
        {
            continue;
        }

        memset(nodemsg, 0, 256);
        if (not_send_cnt == 0)
            sprintf(nodemsg,"{");
        else
           sprintf(nodemsg,",{");
        strcat(mBuf, nodemsg);

        sprintf(nodemsg, "\"%s\":\"%d\"", "nodetype", (*node)->nodetype);
        strcat(mBuf, nodemsg);
        sprintf(nodemsg, ",\"%s\":\"%s\"", "mac", (*node)->macaddress);
        strcat(mBuf, nodemsg);
        sprintf(nodemsg, ",\"%s\":\"%d\"", "index", (*node)->index);
        strcat(mBuf, nodemsg);
        sprintf(nodemsg, ",\"%s\":\"%d\"", "posX", (int)(*node)->posX);
        strcat(mBuf, nodemsg);
        sprintf(nodemsg, ",\"%s\":\"%d\"", "posY", (int)(*node)->posY);
        strcat(mBuf, nodemsg);
        sprintf(nodemsg, ",\"%s\":\"%d\"", "posZ", (int)(*node)->posZ);
        strcat(mBuf, nodemsg);
        sprintf(nodemsg, ",\"%s\":\"%d\"", "buildingid", (int)(*node)->buildingid);
        strcat(mBuf, nodemsg);
        sprintf(nodemsg, ",\"%s\":\"%d\"", "buildingfloor", (int)(*node)->buildingfloor);
        strcat(mBuf, nodemsg);
        sprintf(nodemsg, ",\"%s\":\"%s\"", "version", (*node)->version);
        strcat(mBuf, nodemsg);
        strcat(mBuf, "}");

        not_send_cnt++;
        send_cnt++;
        if ( not_send_cnt >= 8 )
        {
            strcat(mBuf, "]}\r\n");

            //printf("CenterOutgoingService::send_list() mBuf[%s]\n", mBuf);
            send_out_data(mBuf, (int)strlen(mBuf));
            //list_count++;

            memset(mBuf, 0, MAXUDPPACKET);
            sprintf(mBuf, "{\"%s\":\"%s\",\"%s\":\"%d\",\"%s\":[", "type",
                    item->getType(), "nodetype", item->getNodeType(), "nodelist");
            not_send_cnt = 0;

            usleep(100000);
        }

    }
    if ( not_send_cnt > 0 )
    {
       strcat(mBuf, "]}\r\n");
       //printf("CenterOutgoingService::send_list() mBuf[%s]\n", mBuf);
       send_out_data(mBuf, (int)strlen(mBuf));
       //list_count++;
       not_send_cnt = 0;
    }

    // if (bChekAlive)
    // {
    //     vilslog::printf ("CenterOutgoingService()::run() send_cnt[%d] nodeList->size[%lu]\n", send_cnt, nodeList->size());
    // }

    return ret;
}

// int CenterOutgoingService::send_validList(DataItem * item)
// {
//     int ret = 0;
//     memset(mBuf, 0, MAXUDPPACKET);
//     //sprintf(mBuf, "{\"%s\":\"%s\",\"%s\":\"%s\",\"%s\":\"%s\"}\r\n",
//     //        "type", item->getType(), "tagmac", item->getmacaddress(), "anchors", item->getValidAnchors().c_str());
//     sprintf(mBuf, "{\"%s\":\"%s\",\"%s\":\"%d\",\"%s\":\"%s\"}\r\n",
//             "type", item->getType(), "tagidx", item->getTagIdx(), "anchors", item->getValidAnchors().c_str());
//
//     //printf("CenterOutgoingService::send_validList() mBuf[%s]\n", mBuf);
//
//     ret = send_out_data(mBuf, (int)strlen(mBuf));
//
//     return ret;
// }

int CenterOutgoingService::send_tagsampletime(DataItem * item)
{
    int ret = 0;
    memset(mBuf, 0, MAXUDPPACKET);
    //sprintf(mBuf, "{\"%s\":\"%s\",\"%s\":\"%s\",\"%s\":\"%s\"}\r\n",
    //        "type", item->getType(), "tagmac", item->getmacaddress(), "anchors", item->getValidAnchors().c_str());
    sprintf(mBuf, "{\"%s\":\"%s\",\"%s\":\"%d\",\"%s\":\"%d\"}\r\n",
            "type", item->getType(), "windowtime", item->getTagWindowTime(), "resettime", item->getTagResetTime());

    //printf("CenterOutgoingService::send_validList() mBuf[%s]\n", mBuf);

    ret = send_out_data(mBuf, (int)strlen(mBuf));

    return ret;
}

int CenterOutgoingService::send_coordinatorreset(DataItem * item)
{
    int ret = 0;
    memset(mBuf, 0, MAXUDPPACKET);
    sprintf(mBuf, "{\"type\":\"coordinatorreset\"}\r\n");

    ret = send_item_data(item, mBuf, (int)strlen(mBuf));

    return ret;
}

int CenterOutgoingService::send_m0reset(DataItem * item)
{
    int ret = 0;
    memset(mBuf, 0, MAXUDPPACKET);
    sprintf(mBuf, "{\"type\":\"m0reset\"}\r\n");

    ret = send_item_data(item, mBuf, (int)strlen(mBuf));

    return ret;
}

int CenterOutgoingService::send_setvalidrang(DataItem * item)
{
    int ret = 0;
    memset(mBuf, 0, MAXUDPPACKET);

    // int param1_range = 20000; // join且納入測距範圍(cm)
    // int param2_range = 30000; // join但不納入測距範圍(cm)
    int param1_range = item->getRange1(); // join且納入測距範圍(cm)
    int param2_range = item->getRange2(); // join但不納入測距範圍(cm)

    sprintf(mBuf, "{\"type\":\"setvalidrang\",\"param1\":\"%d\",\"param2\":\"%d\"}\r\n", param1_range,  param2_range);

    ret = send_item_data(item, mBuf, (int)strlen(mBuf));

    return ret;
}

int CenterOutgoingService::send_blacklist(DataItem * item)
{
    int ret = 0;
    memset(mBuf, 0, MAXUDPPACKET);

    std::string param1_blacklist = item->getValidAnchors(); // ex:001,124,024

    replaceAll(param1_blacklist, ":", ",");

    sprintf(mBuf, "{\"type\":\"blacklist\",\"param1\":\"%s\"}\r\n", param1_blacklist.c_str());

    ret = send_item_data(item, mBuf, (int)strlen(mBuf));

    return ret;
}

int CenterOutgoingService::send_whitelist(DataItem * item)
{
    int ret = 0;
    memset(mBuf, 0, MAXUDPPACKET);

    std::string param1_whitelist = item->getValidAnchors(); // ex:001,124,024

    //vilslog::printf("CenterOutgoingService::send_whitelist() param1_whitelist[%s]\n", param1_whitelist.c_str());

    replaceAll(param1_whitelist, ":", ",");

    sprintf(mBuf, "{\"type\":\"whitelist\",\"param1\":\"%s\"}\r\n", param1_whitelist.c_str());

    //vilslog::printf("CenterOutgoingService::send_whitelist() mBuf[%s]\n", mBuf);

    ret = send_item_data(item, mBuf, (int)strlen(mBuf));

    return ret;
}

int CenterOutgoingService::send_systemtime(DataItem * item)
{
    int ret = 0;
    memset(mBuf, 0, MAXUDPPACKET);
    //vilslog::printf("CenterOutgoingService::send_systemtime() systemtime[%s]\n", item->getSystemtime());
    sprintf(mBuf, "{\"type\":\"systemtime\",\"time\":\"%s\"}\r\n", item->getSystemtime());

    // vilslog::printf("CenterOutgoingService::send_systemtime() mBuf[%s]\n", mBuf);

    ret = send_item_data(item, mBuf, (int)strlen(mBuf));

    return ret;
}

int CenterOutgoingService::query_validrang(DataItem * item)
{
    int ret = 0;
    memset(mBuf, 0, MAXUDPPACKET);
    sprintf(mBuf, "{\"type\":\"setvalidrang\",\"param1\":\"?\"}\r\n");

    // return {"type":"valirang","lv1":"3000","lv2":"5000"}

    ret = send_item_data(item, mBuf, (int)strlen(mBuf));

    return ret;
}

int CenterOutgoingService::query_blacklist(DataItem * item)
{
    int ret = 0;
    memset(mBuf, 0, MAXUDPPACKET);
    sprintf(mBuf, "{\"type\":\"blacklist\",\"param1\":\"?\"}\r\n");

    ret = send_item_data(item, mBuf, (int)strlen(mBuf));

    return ret;
}

int CenterOutgoingService::query_whitelist(DataItem * item)
{
    int ret = 0;
    memset(mBuf, 0, MAXUDPPACKET);
    sprintf(mBuf, "{\"type\":\"whitelist\",\"param1\":\"?\"}\r\n");

    ret = send_item_data(item, mBuf, (int)strlen(mBuf));

    return ret;
}

int CenterOutgoingService::query_validrang()
{
    int ret = 0;
    memset(mBuf, 0, MAXUDPPACKET);
    sprintf(mBuf, "{\"type\":\"setvalidrang\",\"param1\":\"?\"}\r\n");

    // return {"type":"valirang","lv1":"3000","lv2":"5000"}

    // send to all coordinator
    ret = send_out_data(mBuf, (int)strlen(mBuf));

    return ret;
}

int CenterOutgoingService::query_blacklist()
{
    int ret = 0;
    memset(mBuf, 0, MAXUDPPACKET);
    sprintf(mBuf, "{\"type\":\"blacklist\",\"param1\":\"?\"}\r\n");

    // send to all coordinator
    ret = send_out_data(mBuf, (int)strlen(mBuf));

    return ret;
}

int CenterOutgoingService::query_whitelist()
{
    int ret = 0;
    memset(mBuf, 0, MAXUDPPACKET);
    sprintf(mBuf, "{\"type\":\"whitelist\",\"param1\":\"?\"}\r\n");

    // send to all coordinator
    ret = send_out_data(mBuf, (int)strlen(mBuf));

    return ret;
}

int CenterOutgoingService::send_item_data(DataItem * item, const char * buf, int buf_len)
{
    int ret = 0;

    std::string macaddress = item->getmacaddress();

    if ( macaddress.length() > 0)
    {
        std::string IPAddress = item->getIPAddress();
        if ( IPAddress.length() > 0)
        {
            //vilslog::printf("CenterOutgoingService::send_item_data() macaddress[%s] IPAddress[%s]\n", macaddress.c_str(), IPAddress.c_str());

            // for (std::list<NetworkInfoStruct * >::iterator netif = mCenterNetworkInfoList.begin(); netif != mCenterNetworkInfoList.end(); netif++)
            // {
            //     UDPClient * udpClient = new UDPClient((*netif)->interface, (*netif)->ip);
            //
            //     bool udpInitial = udpClient->init(m_systemConfig->coord_udp_port, IPAddress.c_str());
            //     if ( udpInitial )
            //     {
            //         ret = udpClient->send(buf, buf_len);
            //         vilslog::printf("CenterOutgoingService::send_item_data() udpClient->send() ret[%d] \nbuf[%s]\n", ret, buf);
            //     }
            //
            //     delete udpClient;
            // }
            int projectid = item->getProjectID();
            VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(projectid);

            UDPClient * udpClient = new UDPClient();
            bool udpInitial = udpClient->init(vilssysConfig->coord_udp_port, IPAddress.c_str());
            if ( udpInitial )
            {
                //vilslog::printf("CenterOutgoingService::send_item_data() udpClient->send() coord_udp_port[%d] IPAddress[%s]\n", systemConfig.coord_udp_port, IPAddress.c_str());
                ret = udpClient->send(buf, buf_len);
                //vilslog::printf("CenterOutgoingService::send_item_data() udpClient->send() ret[%d] \nbuf[%s]\n", ret, buf);
            }
            delete udpClient;
        }
    }
    else
    {
        // send to all coordinator
        ret = send_out_data(buf, buf_len);
    }

    return ret;
}

int CenterOutgoingService::send_out_data(const char * buf, int buf_len)
{
    int ret = 0;
    if (mUdpInitial)
    {
        for (std::list<UDPClient *>::iterator udpclient = mUdpClientList.begin(); udpclient != mUdpClientList.end(); udpclient++)
        {
            ret = (*udpclient)->send(buf, buf_len);
        }
    }
    return ret;
}
