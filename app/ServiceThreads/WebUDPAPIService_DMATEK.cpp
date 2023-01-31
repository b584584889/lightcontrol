/*
 *  WebUDPAPIService_DMATEK.cpp
 *
 *  Created on: 2018/10/18
 *      Author: ikki
 */

#include <string.h>
#include <sstream>
#include <iostream>

#include "WebUDPAPIService_DMATEK.h"

using namespace std;

WebUDPAPIService_DMATEK::WebUDPAPIService_DMATEK(wqueue<UDPDataItem*> & udpqueue, wqueue<DataItem*> & WebDMATEKJobQueue)
    : m_udpqueue(udpqueue), m_WebDMATEKJobQueue(WebDMATEKJobQueue)
{
}

bool WebUDPAPIService_DMATEK::status()
{
    return mUdpInitial;
}

std::string WebUDPAPIService_DMATEK::statusString()
{
    return mStatus;
}

void * WebUDPAPIService_DMATEK::run()
{
    while(1)
    {
        try
        {
            runloop();
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("WebUDPAPIService_DMATEK::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mUdpInitial = false;
            mStatus = "WebUDPAPIService_DMATEK::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void WebUDPAPIService_DMATEK::runloop()
{
    mUdpInitial = false;
    mWebUDPServer = new UDPServer();

    // VILSSystemConfig * vilssysConfig = mDbManager->get_sys_config(1);

    int web_udp_port = 51234;
    int UDPretryCount = 0;
    while ( !mUdpInitial && UDPretryCount < 12 )
    {
        mUdpInitial = mWebUDPServer->init(web_udp_port);
        UDPretryCount++;
        if (!mUdpInitial)
        {
            vilslog::printf("WebUDPAPIService_DMATEK::run() mUdpInitial = %d, retryCount = %d\n", mUdpInitial, UDPretryCount);
            sleep(3);
        }
    }

    if ( mUdpInitial )
    {
        mStatus = "WebUDPAPIService_DMATEK()::run() Initial UDPServer successfully!";
        vilslog::printf ("WebUDPAPIService_DMATEK()::run() Initial UDPServer successfully! web_udp_port[%d]\n", web_udp_port);
    }
    else
    {
        mStatus = "WebUDPAPIService_DMATEK()::run() Initial UDPServer failed!!";
        vilslog::printf ("WebUDPAPIService_DMATEK()::run() Initial UDPServer failed!\n");
        delete mWebUDPServer;
        return;
    }

    waitloop();

}

void WebUDPAPIService_DMATEK::waitloop()
{
    char buf[MAXUDPPACKET];
    char clientIP[64];
    int length = 0;
    // int ProcessCount = 0;

    if (mWebUDPServer == NULL)
        return ;

    while ( 1 )
    {
        memset(buf, 0, MAXUDPPACKET);
        memset(clientIP, 0, 64);

        length = mWebUDPServer->waitClient(100, buf, &clientIP[0]);
        //length = mWebUDPServer->waitClient(buf, (struct sockaddr*)&item->m_client_addr);

        if (length > 0)
        {
            parseData(clientIP, buf, length);
        }

        memset(buf, 0, MAXUDPPACKET);

        DataItem * item = m_WebDMATEKJobQueue.removetimed(10);
        if( item != NULL )
        {
            std::string message = item->getMessage();
            length = HEXStringToBuffer(message.c_str(), message.length(), buf);
            parseData(clientIP, buf, length);
            delete item;
        }
    }
}

int WebUDPAPIService_DMATEK::parseData( const char * clientIP, const char * buf, int length)
{
    Json::Value json;
    // vilslog::printf("WebUDPAPIService_DMATEK::parseData() DMATKE udp in :[%s] length[%d]\n", buf, length);

    ReportInf *reportinf = decodeDMATEK_UDP(buf, length);
    // vilslog::printf("WebUDPAPIService_DMATEK::parseData() done:%d\n ", reportinf->role);

    if(reportinf == NULL)
    {
        return 0;;
    }


    if(reportinf->role == 0)
    {
        unsigned int anchorid = (reportinf->anchorreportid.anchorid[0] & 0xff) << 16 |
        (reportinf->anchorreportid.anchorid[1] & 0xff)<< 8 |
        (reportinf->anchorreportid.anchorid[2] & 0xff);
        char anchorid_c[16];
        memset(anchorid_c, '\0', sizeof(char) * 16);
        convert2hex(anchorid_c, anchorid);
        std::string anchorid_s;
        anchorid_s.append("0020000000");
        anchorid_s.append(anchorid_c);
        //std::string mystring(reportinf->anchorreportid.anchorid, 3);

/*jsonEncodeObj(jsonFormatStorage, 256, 7,"type", type,"mac", mac,
    	    "nodetype", nodeType_SStorage, "vlt", voltage_SStroage,
    	    "tem", temperature_SStroage, "count", aliveCount_SStroage,
			"pid", pid_SStorage);*/

        // json["type"] = "join";
        // json["mac"] = anchorid_s.c_str();
        // json["nodetype"] = "0";
        // json["version"] = "1.0";//reportinf->anchorreportid.anchorFwVersion;
        //
        // UDPDataItem * item = new UDPDataItem("coordmsg", "", 0, clientIP);
        // item->setMsg(json.toStyledString().append("\r\n").c_str(), json.toStyledString().size()+2);
        // item->mUDPServer = mWebUDPServer;
        //
        // m_udpqueue.add(item);

        send_anchor_join(anchorid_s, clientIP);
    }
    else
    {

        unsigned int tagid = (reportinf->tagreport.tagid[0] & 0xff) << 16 |
        (reportinf->tagreport.tagid[1] & 0xff)<< 8 |
        (reportinf->tagreport.tagid[2] & 0xff);
        char tagid_c[16];
        memset(tagid_c, '\0', sizeof(char) * 16);
        convert2hex(tagid_c, tagid);
        std::string tagid_s;
        tagid_s.append("0010000000");
        tagid_s.append(tagid_c);

        //std::string mystring(reportinf->anchorreportid.tagid, 3);

        json["type"] = "join";
        json["mac"] = tagid_s.c_str();
        json["nodetype"] = "1";
        json["version"] = "1.0";//reportinf->anchorreportid.anchorFwVersion;

        UDPDataItem * item = new UDPDataItem("coordmsg", "", 0, clientIP);
        item->setMsg(json.toStyledString().append("\r\n").c_str(), json.toStyledString().size()+2);
        item->mUDPServer = mWebUDPServer;
        m_udpqueue.add(item);

        if(reportinf->tagreport.locateType == 1)
        {
            UDPDataItem * item_dist = new UDPDataItem("coordmsg", "", 0, clientIP);
            unsigned int anchorid = (reportinf->tagreport.locateAnchorId1[0] & 0xff) << 16 |
            (reportinf->tagreport.locateAnchorId1[1] & 0xff)<< 8 |
            (reportinf->tagreport.locateAnchorId1[2] & 0xff);
            char anchorid_c[16];
            memset(anchorid_c, '\0', sizeof(char) * 16);
            convert2hex(anchorid_c, anchorid);
            std::string anchorid_s;
            anchorid_s.append("0020000000");
            anchorid_s.append(anchorid_c);

            send_anchor_join(anchorid_s, clientIP);

            int dist = ((reportinf->tagreport.locateAnchorDis1[0]&0xff)<<8) | ((reportinf->tagreport.locateAnchorDis1[1]&0xff));

            std::string dist_s = std::to_string(dist);
            std::string imu_s;
            signed short imux = ((reportinf->tagreport.gsensorX[0] & 0xff) << 8) | (reportinf->tagreport.gsensorX[1] & 0xff);
            signed short imuy = ((reportinf->tagreport.gsensorY[0] & 0xff) << 8) | (reportinf->tagreport.gsensorY[1] & 0xff);
            signed short imuz = ((reportinf->tagreport.gsensorZ[0] & 0xff) << 8) | (reportinf->tagreport.gsensorZ[1] & 0xff);
            imu_s.append(std::to_string(imux/10));
            imu_s.append(":");
            imu_s.append(std::to_string(imuy/10));
            imu_s.append(":");
            imu_s.append(std::to_string(imuz/10));
            imu_s.append(":0:0:0");

            json["type"] = "alert";
            json["anchor"] = anchorid_s.c_str();
            json["tag"] = tagid_s.c_str();//tagid_s.c_str();
            json["idx"] = "0";
            json["dist"] = dist_s.c_str();
            json["imu"] = imu_s.c_str();
            json["PCnt"] = "0";
            json["AnCnt"] = "0";
            json["TagRecv"] = "0";
            json["AnFP"] = "0";
            json["RnCnt"] = "0";
            json["RnFCnt"] = "0";
            json["DaRate"] = "0";
            json["nodetype"] = "1";
            json["version"] = "1.0";//reportinf->anchorreportid.anchorFwVersion;

            //vilslog::printf("decoded 1 : %s\n", json.toStyledString().c_str());

            item_dist->setMsg(json.toStyledString().append("\r\n").c_str(), json.toStyledString().size()+2);
            item_dist->mUDPServer = mWebUDPServer;

            m_udpqueue.add(item_dist);
        }

/*mBuf = "{\"type\":\"";
mBuf += itemObj.getType(); rangingdiag
mBuf +="\",\"anchor\":\"";
mBuf += anchorMac;
mBuf += "\",\"tag\":\"";
mBuf += itemObj.getmacaddress();
mBuf += "\",\"idx\":\"";
mBuf += idx;
mBuf += "\",\"dist\":\"";
mBuf += dist;
mBuf += "\",\"imu\":\"";
mBuf += IMUString;
mBuf += "\",\"PCnt\":\"";
mBuf += pCnt;
mBuf += "\",\"AnCnt\":\"";
mBuf += anCnt;
mBuf += "\",\"TagRecv\":\"";
mBuf += tagRdbm;
mBuf += "\",\"TagFP\":\"";
mBuf += tagFPdbm;
mBuf += "\",\"AnRecv\":\"";
mBuf += anRdbm;
mBuf += "\",\"AnFP\":\"";
mBuf += anFPdbm;
mBuf += "\",\"RnCnt\":\"";
mBuf += tagHandler.tagList[itemObj.getTagIdx()].sendCnt_update[itemObj.getAnchorIdx()];
mBuf += "\",\"RnFCnt\":\"";
mBuf += tagHandler.tagList[itemObj.getTagIdx()].lostCnt_update[itemObj.getAnchorIdx()];
mBuf += "\",\"DaRate\":\"";
mBuf += dataRate.string();
mBuf +="\"}\r\n";*/


        // vilslog::printf("WebUDPAPIService_DMATEK::parseData() TAG : %s\n", tagid_s.c_str());

        for(int i = 0; i < 5; i++)
        {

            switch(i)
            {
                case 0:
                {
                    if(reportinf->tagreport.anchor1Exist)
                    {
                        UDPDataItem * item_dist = new UDPDataItem("coordmsg", "", 0, clientIP);
                        unsigned int anchorid = (reportinf->tagreport.locateAnchorId1[0] & 0xff) << 16 |
                        (reportinf->tagreport.locateAnchorId1[1] & 0xff)<< 8 |
                        (reportinf->tagreport.locateAnchorId1[2] & 0xff);
                        char anchorid_c[16];
                        memset(anchorid_c, '\0', sizeof(char) * 16);
                        convert2hex(anchorid_c, anchorid);
                        std::string anchorid_s;
                        anchorid_s.append("0020000000");
                        anchorid_s.append(anchorid_c);

                        send_anchor_join(anchorid_s, clientIP);

                        int dist = ((reportinf->tagreport.locateAnchorDis1[0]&0xff)<<8) | ((reportinf->tagreport.locateAnchorDis1[1]&0xff));

                        std::string dist_s = std::to_string(dist);
                        std::string imu_s;
                        signed short imux = ((reportinf->tagreport.gsensorX[0] & 0xff) << 8) | (reportinf->tagreport.gsensorX[1] & 0xff);
                        signed short imuy = ((reportinf->tagreport.gsensorY[0] & 0xff) << 8) | (reportinf->tagreport.gsensorY[1] & 0xff);
                        signed short imuz = ((reportinf->tagreport.gsensorZ[0] & 0xff) << 8) | (reportinf->tagreport.gsensorZ[1] & 0xff);
                        imu_s.append(std::to_string(imux/10));
                        imu_s.append(":");
                        imu_s.append(std::to_string(imuy/10));
                        imu_s.append(":");
                        imu_s.append(std::to_string(imuz/10));
                        imu_s.append(":0:0:0");

                        json["type"] = "rangingdiag";
                        json["anchor"] = anchorid_s.c_str();
                        json["tag"] = tagid_s.c_str();//tagid_s.c_str();
                        json["idx"] = "0";
                        json["dist"] = dist_s.c_str();
                        json["imu"] = imu_s.c_str();
                        json["PCnt"] = "0";
                        json["AnCnt"] = "0";
                        json["TagRecv"] = "0";
                        json["AnFP"] = "0";
                        json["RnCnt"] = "0";
                        json["RnFCnt"] = "0";
                        json["DaRate"] = "0";
                        json["nodetype"] = "1";
                        json["version"] = "1.0";//reportinf->anchorreportid.anchorFwVersion;

                        // vilslog::printf("decoded 1 : %s\n", json.toStyledString().c_str());

                        item_dist->setMsg(json.toStyledString().append("\r\n").c_str(), json.toStyledString().size()+2);
                        item_dist->mUDPServer = mWebUDPServer;

                        m_udpqueue.add(item_dist);
                    }
                }
                case 1:
                {
                    if(reportinf->tagreport.anchor2Exist)
                    {
                        UDPDataItem * item_dist = new UDPDataItem("coordmsg", "", 0, clientIP);
                        unsigned int anchorid = (reportinf->tagreport.locateAnchorId2[0] & 0xff) << 16 |
                        (reportinf->tagreport.locateAnchorId2[1] & 0xff)<< 8 |
                        (reportinf->tagreport.locateAnchorId2[2] & 0xff);
                        char anchorid_c[16];
                        memset(anchorid_c, '\0', sizeof(char) * 16);
                        convert2hex(anchorid_c, anchorid);
                        std::string anchorid_s;
                        anchorid_s.append("0020000000");
                        anchorid_s.append(anchorid_c);

                        send_anchor_join(anchorid_s, clientIP);

                        int dist = ((reportinf->tagreport.locateAnchorDis2[0]&0xff)<<8) | ((reportinf->tagreport.locateAnchorDis2[1]&0xff));

                        std::string dist_s = std::to_string(dist);
                        std::string imu_s;
                        signed short imux = ((reportinf->tagreport.gsensorX[0] & 0xff) << 8) | (reportinf->tagreport.gsensorX[1] & 0xff);
                        signed short imuy = ((reportinf->tagreport.gsensorY[0] & 0xff) << 8) | (reportinf->tagreport.gsensorY[1] & 0xff);
                        signed short imuz = ((reportinf->tagreport.gsensorZ[0] & 0xff) << 8) | (reportinf->tagreport.gsensorZ[1] & 0xff);
                        imu_s.append(std::to_string(imux/10));
                        imu_s.append(":");
                        imu_s.append(std::to_string(imuy/10));
                        imu_s.append(":");
                        imu_s.append(std::to_string(imuz/10));
                        imu_s.append(":0:0:0");

                        json["type"] = "rangingdiag";
                        json["anchor"] = anchorid_s.c_str();
                        json["tag"] = tagid_s.c_str();
                        json["idx"] = "0";
                        json["dist"] = dist_s.c_str();
                        json["imu"] = imu_s.c_str();
                        json["PCnt"] = "0";
                        json["AnCnt"] = "0";
                        json["TagRecv"] = "0";
                        json["AnFP"] = "0";
                        json["RnCnt"] = "0";
                        json["RnFCnt"] = "0";
                        json["DaRate"] = "0";
                        json["nodetype"] = "1";
                        json["version"] = "1.0";//reportinf->anchorreportid.anchorFwVersion;

                        // vilslog::printf("decoded 2 : %s\n", json.toStyledString().c_str());

                        item_dist->setMsg(json.toStyledString().append("\r\n").c_str(), json.toStyledString().size()+2);
                        item_dist->mUDPServer = mWebUDPServer;

                        m_udpqueue.add(item_dist);
                    }
                }
                case 2:
                {
                    if(reportinf->tagreport.anchor3Exist)
                    {
                        UDPDataItem * item_dist = new UDPDataItem("coordmsg", "", 0, clientIP);
                        unsigned int anchorid = (reportinf->tagreport.locateAnchorId3[0] & 0xff) << 16 |
                        (reportinf->tagreport.locateAnchorId3[1] & 0xff)<< 8 |
                        (reportinf->tagreport.locateAnchorId3[2] & 0xff);
                        char anchorid_c[16];
                        memset(anchorid_c, '\0', sizeof(char) * 16);
                        convert2hex(anchorid_c, anchorid);
                        std::string anchorid_s;
                        anchorid_s.append("0020000000");
                        anchorid_s.append(anchorid_c);

                        send_anchor_join(anchorid_s, clientIP);

                        int dist = ((reportinf->tagreport.locateAnchorDis3[0]&0xff)<<8) | ((reportinf->tagreport.locateAnchorDis3[1]&0xff));

                        std::string dist_s = std::to_string(dist);
                        std::string imu_s;
                        signed short imux = ((reportinf->tagreport.gsensorX[0] & 0xff) << 8) | (reportinf->tagreport.gsensorX[1] & 0xff);
                        signed short imuy = ((reportinf->tagreport.gsensorY[0] & 0xff) << 8) | (reportinf->tagreport.gsensorY[1] & 0xff);
                        signed short imuz = ((reportinf->tagreport.gsensorZ[0] & 0xff) << 8) | (reportinf->tagreport.gsensorZ[1] & 0xff);
                        imu_s.append(std::to_string(imux/10));
                        imu_s.append(":");
                        imu_s.append(std::to_string(imuy/10));
                        imu_s.append(":");
                        imu_s.append(std::to_string(imuz/10));
                        imu_s.append(":0:0:0");

                        json["type"] = "rangingdiag";
                        json["anchor"] = anchorid_s.c_str();
                        json["tag"] = tagid_s.c_str();
                        json["idx"] = "0";
                        json["dist"] = dist_s.c_str();
                        json["imu"] = imu_s.c_str();
                        json["PCnt"] = "0";
                        json["AnCnt"] = "0";
                        json["TagRecv"] = "0";
                        json["AnFP"] = "0";
                        json["RnCnt"] = "0";
                        json["RnFCnt"] = "0";
                        json["DaRate"] = "0";
                        json["nodetype"] = "1";
                        json["version"] = "1.0";//reportinf->anchorreportid.anchorFwVersion;
                        // vilslog::printf("decoded 3 : %s\n", json.toStyledString().c_str());
                        item_dist->setMsg(json.toStyledString().append("\r\n").c_str(), json.toStyledString().size()+2);
                        item_dist->mUDPServer = mWebUDPServer;

                        m_udpqueue.add(item_dist);
                    }
                }
                case 3:
                {
                    if(reportinf->tagreport.anchor4Exist)
                    {
                        UDPDataItem * item_dist = new UDPDataItem("coordmsg", "", 0, clientIP);
                        unsigned int anchorid = (reportinf->tagreport.locateAnchorId4[0] & 0xff) << 16 |
                        (reportinf->tagreport.locateAnchorId4[1] & 0xff)<< 8 |
                        (reportinf->tagreport.locateAnchorId4[2] & 0xff);
                        char anchorid_c[16];
                        memset(anchorid_c, '\0', sizeof(char) * 16);
                        convert2hex(anchorid_c, anchorid);
                        std::string anchorid_s;
                        anchorid_s.append("0020000000");
                        anchorid_s.append(anchorid_c);

                        send_anchor_join(anchorid_s, clientIP);

                        int dist = ((reportinf->tagreport.locateAnchorDis4[0]&0xff)<<8) | ((reportinf->tagreport.locateAnchorDis4[1]&0xff));

                        std::string dist_s = std::to_string(dist);
                        std::string imu_s;
                        signed short imux = ((reportinf->tagreport.gsensorX[0] & 0xff) << 8) | (reportinf->tagreport.gsensorX[1] & 0xff);
                        signed short imuy = ((reportinf->tagreport.gsensorY[0] & 0xff) << 8) | (reportinf->tagreport.gsensorY[1] & 0xff);
                        signed short imuz = ((reportinf->tagreport.gsensorZ[0] & 0xff) << 8) | (reportinf->tagreport.gsensorZ[1] & 0xff);
                        imu_s.append(std::to_string(imux/10));
                        imu_s.append(":");
                        imu_s.append(std::to_string(imuy/10));
                        imu_s.append(":");
                        imu_s.append(std::to_string(imuz/10));
                        imu_s.append(":0:0:0");

                        json["type"] = "rangingdiag";
                        json["anchor"] = anchorid_s.c_str();
                        json["tag"] = tagid_s.c_str();
                        json["idx"] = "0";
                        json["dist"] = dist_s.c_str();
                        json["imu"] = imu_s.c_str();
                        json["PCnt"] = "0";
                        json["AnCnt"] = "0";
                        json["TagRecv"] = "0";
                        json["AnFP"] = "0";
                        json["RnCnt"] = "0";
                        json["RnFCnt"] = "0";
                        json["DaRate"] = "0";
                        json["nodetype"] = "1";
                        json["version"] = "1.0";//reportinf->anchorreportid.anchorFwVersion;
                        // vilslog::printf("decoded 4 : %s\n", json.toStyledString().c_str());
                        item_dist->setMsg(json.toStyledString().append("\r\n").c_str(), json.toStyledString().size()+2);
                        item_dist->mUDPServer = mWebUDPServer;

                        m_udpqueue.add(item_dist);
                    }
                }
                case 4:
                {
                    if(reportinf->tagreport.anchor5Exist)
                    {
                        UDPDataItem * item_dist = new UDPDataItem("coordmsg", "", 0, clientIP);
                        unsigned int anchorid = (reportinf->tagreport.locateAnchorId5[0] & 0xff) << 16 |
                        (reportinf->tagreport.locateAnchorId5[1] & 0xff)<< 8 |
                        (reportinf->tagreport.locateAnchorId5[2] & 0xff);
                        char anchorid_c[16];
                        memset(anchorid_c, '\0', sizeof(char) * 16);
                        convert2hex(anchorid_c, anchorid);
                        std::string anchorid_s;
                        anchorid_s.append("0020000000");
                        anchorid_s.append(anchorid_c);

                        send_anchor_join(anchorid_s, clientIP);

                        int dist = ((reportinf->tagreport.locateAnchorDis5[0]&0xff)<<8) | ((reportinf->tagreport.locateAnchorDis5[1]&0xff));

                        std::string dist_s = std::to_string(dist);
                        std::string imu_s;
                        signed short imux = ((reportinf->tagreport.gsensorX[0] & 0xff) << 8) | (reportinf->tagreport.gsensorX[1] & 0xff);
                        signed short imuy = ((reportinf->tagreport.gsensorY[0] & 0xff) << 8) | (reportinf->tagreport.gsensorY[1] & 0xff);
                        signed short imuz = ((reportinf->tagreport.gsensorZ[0] & 0xff) << 8) | (reportinf->tagreport.gsensorZ[1] & 0xff);
                        imu_s.append(std::to_string(imux/10));
                        imu_s.append(":");
                        imu_s.append(std::to_string(imuy/10));
                        imu_s.append(":");
                        imu_s.append(std::to_string(imuz/10));
                        imu_s.append(":0:0:0");

                        json["type"] = "rangingdiag";
                        json["anchor"] = anchorid_s.c_str();
                        json["tag"] = tagid_s.c_str();
                        json["idx"] = "0";
                        json["dist"] = dist_s.c_str();
                        json["imu"] = imu_s.c_str();
                        json["PCnt"] = "0";
                        json["AnCnt"] = "0";
                        json["TagRecv"] = "0";
                        json["AnFP"] = "0";
                        json["RnCnt"] = "0";
                        json["RnFCnt"] = "0";
                        json["DaRate"] = "0";
                        json["nodetype"] = "1";
                        json["version"] = "1.0";//reportinf->anchorreportid.anchorFwVersion;
                        // vilslog::printf("decoded 5 : %s\n", json.toStyledString().c_str());
                        item_dist->setMsg(json.toStyledString().append("\r\n").c_str(), json.toStyledString().size()+2);
                        item_dist->mUDPServer = mWebUDPServer;

                        m_udpqueue.add(item_dist);
                    }
                }

            }
        }
    }
    return 1;
}

int WebUDPAPIService_DMATEK::send_anchor_join(std::string anchorid_s, const char * clientIP)
{
    Json::Value json;
    json["type"] = "join";
    json["mac"] = anchorid_s.c_str();
    json["nodetype"] = "0";
    json["version"] = "1.0";//reportinf->anchorreportid.anchorFwVersion;

    UDPDataItem * item = new UDPDataItem("coordmsg", "", 0, clientIP);
    item->setMsg(json.toStyledString().append("\r\n").c_str(), json.toStyledString().size()+2);
    item->mUDPServer = mWebUDPServer;

    m_udpqueue.add(item);

    return 0;
}

ReportInf *WebUDPAPIService_DMATEK::decodeDMATEK_UDP(const char *buf, int size)
{

//F0 01 27 00 03 01 71 52 64 01 1C 04 02 00 3C 01 04 13 13 13 13 02 04 95 B5 28 10 03 01 02 05 01 00 06 01 00 FF 8D 0F



    const char *start;//strchr(buf, '0xF0');
    const char *end;// = strrchr(buf, '0x0F');
    //vilslog::printf("decode : %d-%d\n", (long)start, (long)end);

    if((uint8_t)buf[0] == 0xf0)
    {
        start = &buf[0];
    }

    for(int i = 0 ; i < size; i++)
    {
        // vilslog::printf("decode : %x\n", buf[i]);
        //vilslog::printf("decode : %c\n", buf[i]);
        if(buf[i] == 0x0f)
        {
            end = &buf[i];
        }
        else
        {
            end = NULL;
        }
    }
    if(start != NULL && end != NULL)
    {
        int dataSize = ((long)end - (long)start) + 1;
        char data[dataSize];

        static ReportInf reportinf;

        AnchorReportId *anchorreportid;
        TagReport *tagreport;
        memcpy(data, buf, dataSize);
        if(data[2] != dataSize)//check size
        {
            return NULL;
        }
        if(checkCRC((uint8_t *)data, dataSize))//check crc
        {
            switch(data[1])
            {
                case 0x01:
                {
                    // vilslog::printf("decode role : anchor");
                    anchorreportid = analyAnchorReport((uint8_t *)data + 3, dataSize-3);
                    reportinf.role = 0;
                    memcpy(&reportinf.anchorreportid, anchorreportid, sizeof(AnchorReportId));
                }
                break;
                case 0x02:
                {
                    // vilslog::printf("decode role : tag\n");
                    tagreport = analyTagReport((uint8_t *)data + 3, dataSize-3);
                    reportinf.role = 1;
                    memcpy(&reportinf.tagreport, tagreport, sizeof(TagReport));
                }
                break;

            }
        }
        return &reportinf;
    }
    else
    {
        return NULL;
    }
}

AnchorReportId *WebUDPAPIService_DMATEK::analyAnchorReport(uint8_t *data, int dataSize)
{
    static AnchorReportId anchorreportid;
    int offset = 0;

    while(offset < dataSize)
    {
        // vilslog::printf("analy : %x\n", data[offset]);
        switch(data[offset])
        {
            case _ANCHORID:
            {
                //int parmSize = data[offset+1];

                anchorreportid.anchorid[0] = data[offset+2];
                anchorreportid.anchorid[1] = data[offset+3];
                anchorreportid.anchorid[2] = data[offset+4];
                offset += 3;
            }
            break;
            case _SERIALNUM:
            {
                //int parmSize = data[offset+1];
                anchorreportid.serialnum = data[offset+2];
                offset++;
            }
            break;
            case _ANCHOR_REPORT_ID_INTERVAL:
            {
                //int parmSize = data[offset+1];
                anchorreportid.anchorReportIdInv[0] = data[offset+2];
                anchorreportid.anchorReportIdInv[1] = data[offset+3];
                offset += 2;
            }
            break;
            case _ANCHOR_FIRMWARE_TYPE:
            {
                //int parmSize = data[offset+1];
                anchorreportid.anchorFwType[0] = data[offset+2];
                anchorreportid.anchorFwType[1] = data[offset+3];
                anchorreportid.anchorFwType[2] = data[offset+4];
                anchorreportid.anchorFwType[3] = data[offset+5];
                offset += 4;
            }
            break;
            case _ANCHOR_FIRMWARE_VERSION:
            {
                //int parmSize = data[offset+1];
                anchorreportid.anchorFwVersion[0] = data[offset + 2];
                anchorreportid.anchorFwVersion[1] = data[offset + 3];
                anchorreportid.anchorFwVersion[2] = data[offset + 4];
                anchorreportid.anchorFwVersion[3] = data[offset + 5];
                offset += 4;
            }
            break;
            case _ANCHOR_HARDWARE_VERSION:
            {
                //int parmSize = data[offset+1];
                anchorreportid.anchorHwVersion = data[offset + 2];
                offset++;
            }
            break;
            case _ANCHOR_TAG_BIN_UPDATE_EN:
            {
                //int parmSize = data[offset+1];
                anchorreportid.tagBinUpdateEn = data[offset + 2];
                offset++;
            }
            break;
            case _ANCHOR_TAG_PARM_UPDATE_EN:
            {
                //int parmSize = data[offset+1];
                anchorreportid.tagParmUpdateEn = data[offset + 2];
                offset++;
            }
            break;
            case _ENDCODE:
            {
                return &anchorreportid;
            }
            break;
            default:
            {
                return NULL;
            }
            break;
        }
        offset+=2;
    }
    return &anchorreportid;
}

TagReport *WebUDPAPIService_DMATEK::analyTagReport(uint8_t *data, int dataSize)
{
    static TagReport tagReport;
    int offset = 0;
    while(offset < dataSize)
    {

        // vilslog::printf("analy Tag : %x\n", data[offset]);

        switch(data[offset])
        {
            case _ANCHORID:
            {
                //int parmSize = data[offset+1];
                tagReport.anchorid[0] = data[offset + 2];
                tagReport.anchorid[1] = data[offset + 3];
                tagReport.anchorid[2] = data[offset + 4];
                offset+=3;
            }
            break;
            case _TAGID:
            {
                // vilslog::printf("_TAGID\n");
                //int parmSize = data[offset+1];
                tagReport.tagid[0] = data[offset + 2];
                tagReport.tagid[1] = data[offset + 3];
                tagReport.tagid[2] = data[offset + 4];
                offset+=3;
            }
            break;
            case _SERIALNUM:
            {
                //int parmSize = data[offset+1];
                tagReport.serialNum = data[offset + 2];
                offset++;
            }
            break;
            case _LOCATE_TYEP:
            {
                //int parmSize = data[offset+1];
                tagReport.locateType = data[offset + 2];
                offset++;
            }
            break;
            case _TAG_BATTERY:
            {
                //int parmSize = data[offset+1];
                tagReport.tagBattery = data[offset + 2];
                offset++;
            }
            break;
            case _TAG_GSENSOR_TIME:
            {
                //int parmSize = data[offset+1];
                tagReport.tagGSensorTime[0] = data[offset + 2];
                tagReport.tagGSensorTime[1] = data[offset + 3];
                offset+=2;
            }
            break;
            case _TAG_SLEEP_TIME:
            {
                //int parmSize = data[offset+1];
                tagReport.tagSleepTime[0] = data[offset + 2];
                tagReport.tagSleepTime[1] = data[offset + 3];
                offset+=2;
            }
            break;
            case _GSENSORX:
            {
                //int parmSize = data[offset+1];
                tagReport.gsensorX[0] = data[offset + 2];
                tagReport.gsensorX[1] = data[offset + 3];
                offset+=2;
            }
            break;
            case _GSENSORY:
            {
                //int parmSize = data[offset+1];
                tagReport.gsensorY[0] = data[offset + 2];
                tagReport.gsensorY[1] = data[offset + 3];
                offset+=2;
            }
            break;
            case _GSENSORZ:
            {
                //int parmSize = data[offset+1];
                tagReport.gsensorZ[0] = data[offset + 2];
                tagReport.gsensorZ[1] = data[offset + 3];
                offset+=2;
            }
            break;
            case _TAG_HEIGHT:
            {
                //int parmSize = data[offset+1];
                tagReport.tagHeight[0] = data[offset + 2];
                tagReport.tagHeight[1] = data[offset + 3];
                tagReport.tagHeight[2] = data[offset + 4];
                offset+=3;
            }
            break;
            case _TAG_FW_TYPE:
            {
                //int parmSize = data[offset+1];
                tagReport.tagFwType[0] = data[offset + 2];
                tagReport.tagFwType[1] = data[offset + 3];
                tagReport.tagFwType[2] = data[offset + 4];
                tagReport.tagFwType[3] = data[offset + 5];
                offset+=4;
            }
            break;
            case _TAG_FW_VERSION:
            {
                //int parmSize = data[offset+1];
                tagReport.tagFwVersion[0] = data[offset + 2];
                tagReport.tagFwVersion[1] = data[offset + 3];
                tagReport.tagFwVersion[2] = data[offset + 4];
                tagReport.tagFwVersion[3] = data[offset + 5];
                offset+=4;
            }
            break;
            case _TAG_HARDWARE_VERSION:
            {
                //int parmSize = data[offset+1];
                tagReport.tagHwVersion = data[offset + 2];
                offset++;
            }
            break;
            case _LOCATE_ANCHORID1:
            {
                tagReport.anchor1Exist = true;
                //int parmSize = data[offset+1];
                tagReport.locateAnchorId1[0] = data[offset + 2];
                tagReport.locateAnchorId1[1] = data[offset + 3];
                tagReport.locateAnchorId1[2] = data[offset + 4];
                offset+=3;
            }
            break;
            case _LOCATE_DISTANCE1:
            {
                //int parmSize = data[offset+1];
                tagReport.locateAnchorDis1[0] = data[offset + 2];
                tagReport.locateAnchorDis1[1] = data[offset + 3];
                offset+=2;
            }
            break;
            case _LOCATE_ANCHOR_HEIGHT1:
            {
                //int parmSize = data[offset+1];
                tagReport.locateAnchorHeight1[0] = data[offset + 2];
                tagReport.locateAnchorHeight1[1] = data[offset + 3];
                tagReport.locateAnchorHeight1[2] = data[offset + 4];
                offset+=3;
            }
            break;
            case _LOCATE_ANCHORID2:
            {
                tagReport.anchor2Exist = true;
                //int parmSize = data[offset+1];
                tagReport.locateAnchorId2[0] = data[offset + 2];
                tagReport.locateAnchorId2[1] = data[offset + 3];
                tagReport.locateAnchorId2[2] = data[offset + 4];
                offset+=3;
            }
            break;
            case _LOCATE_DISTANCE2:
            {
                //int parmSize = data[offset+1];
                tagReport.locateAnchorDis2[0] = data[offset + 2];
                tagReport.locateAnchorDis2[1] = data[offset + 3];
                offset+=2;
            }
            break;
            case _LOCATE_ANCHOR_HEIGHT2:
            {
                //int parmSize = data[offset+1];
                tagReport.locateAnchorHeight2[0] = data[offset + 2];
                tagReport.locateAnchorHeight2[1] = data[offset + 3];
                tagReport.locateAnchorHeight2[2] = data[offset + 4];
                offset+=3;
            }
            break;
            case _LOCATE_ANCHORID3:
            {
                tagReport.anchor3Exist = true;
                //int parmSize = data[offset+1];
                tagReport.locateAnchorId3[0] = data[offset + 2];
                tagReport.locateAnchorId3[1] = data[offset + 3];
                tagReport.locateAnchorId3[2] = data[offset + 4];
                offset+=3;
            }
            break;
            case _LOCATE_DISTANCE3:
            {
                //int parmSize = data[offset+1];
                tagReport.locateAnchorDis3[0] = data[offset + 2];
                tagReport.locateAnchorDis3[1] = data[offset + 3];
                offset+=2;
            }
            break;
            case _LOCATE_ANCHOR_HEIGHT3:
            {
                //int parmSize = data[offset+1];
                tagReport.locateAnchorHeight3[0] = data[offset + 2];
                tagReport.locateAnchorHeight3[1] = data[offset + 3];
                tagReport.locateAnchorHeight3[2] = data[offset + 4];
                offset+=3;
            }
            break;
            case _LOCATE_ANCHORID4:
            {
                tagReport.anchor4Exist = true;
                //int parmSize = data[offset+1];
                tagReport.locateAnchorId4[0] = data[offset + 2];
                tagReport.locateAnchorId4[1] = data[offset + 3];
                tagReport.locateAnchorId4[2] = data[offset + 4];
                offset+=3;
            }
            break;
            case _LOCATE_DISTANCE4:
            {
                //int parmSize = data[offset+1];
                tagReport.locateAnchorDis4[0] = data[offset + 2];
                tagReport.locateAnchorDis4[1] = data[offset + 3];
                offset+=2;
            }
            break;
            case _LOCATE_ANCHOR_HEIGHT4:
            {
                //int parmSize = data[offset+1];
                tagReport.locateAnchorHeight4[0] = data[offset + 2];
                tagReport.locateAnchorHeight4[1] = data[offset + 3];
                tagReport.locateAnchorHeight4[2] = data[offset + 4];
                offset+=3;
            }
            break;
            case _LOCATE_ANCHORID5:
            {
                tagReport.anchor5Exist = true;
                //int parmSize = data[offset+1];
                tagReport.locateAnchorId5[0] = data[offset + 2];
                tagReport.locateAnchorId5[1] = data[offset + 3];
                tagReport.locateAnchorId5[2] = data[offset + 4];
                offset+=3;
            }
            break;
            case _LOCATE_DISTANCE5:
            {
                //int parmSize = data[offset+1];
                tagReport.locateAnchorDis5[0] = data[offset + 2];
                tagReport.locateAnchorDis5[1] = data[offset + 3];
                offset+=2;
            }
            break;
            case _LOCATE_ANCHOR_HEIGHT5:
            {
                //int parmSize = data[offset+1];
                tagReport.locateAnchorHeight5[0] = data[offset + 2];
                tagReport.locateAnchorHeight5[1] = data[offset + 3];
                tagReport.locateAnchorHeight5[2] = data[offset + 4];
                offset+=3;
            }
            break;
            case _HEART_RATE:
            {
                //int parmSize = data[offset+1];
                tagReport.heartRate[0] = data[offset + 2];
                tagReport.heartRate[1] = data[offset + 3];
                tagReport.heartRate[2] = data[offset + 4];
                offset+=3;
            }
            break;
            case _ENDCODE:
            {
                return &tagReport;
            }
            break;
            default:
            {
                return NULL;
            }
            break;
        }
        offset+=2;
    }
    return &tagReport;
}

bool WebUDPAPIService_DMATEK::checkCRC(uint8_t *data, int size)
{
    uint8_t crc = 0;
    for(int i = 0; i < size; i++)
    {
        if(data[i] != 0xff)
        {
            crc += data[i];
        }
        else
        {

            crc += 0xff;
            // vilslog::printf("CRC : %d ~ %d\n", crc, data[i+1]);
            if(crc == data[i+1])
            {
                return true;
            }
        }
    }
    return false;

}

void WebUDPAPIService_DMATEK::convert2hex(char* _buffer, unsigned int data)
{
    char hexchars[] = "0123456789ABCDEF";

    *_buffer++ = hexchars[data >> 20 & 0x0f];
    *_buffer++ = hexchars[data >> 16 & 0x0f];
    *_buffer++ = hexchars[data >> 12 & 0x0f];
    *_buffer++ = hexchars[data >> 8 & 0x0f];
    *_buffer++ = hexchars[data >> 4 & 0x0f];
    *_buffer++ = hexchars[data & 0x0f];
    *_buffer = 0;
}
