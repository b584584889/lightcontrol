/*
 *  CoordMsgParserService.cpp
 *
 *  Created on: 2019/04/15
 *      Author: ikki
 */

#include <string.h>

#include "CoordMsgParserService.h"
#include "../json/json.h"
#include "../json/JSONAgent.h"


using namespace std;

CoordMsgParserService::CoordMsgParserService(wqueue<DataItem*> & jobqueue, wqueue<UDPDataItem*> & udpqueue, DeviceManager * pDevManager)
    : m_jobqueue(jobqueue), m_udpqueue(udpqueue), mDeviceManager(pDevManager)
{
}

bool CoordMsgParserService::status()
{
    return mInitial;
}

std::string CoordMsgParserService::statusString()
{
    return mStatus;
}

void CoordMsgParserService::updateProject()
{
    mUpdateProject = true;
}

void * CoordMsgParserService::run()
{
    while(1)
    {
        try
        {
            mPrjidList = mDeviceManager->getAllProjectConfig();

            runloop();
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("CoordMsgParserService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "CoordMsgParserService()::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void CoordMsgParserService::runloop()
{
    long long start1 = getSystemTime();
    long long end1 = getSystemTime();
    long long difft = end1 - start1;
    long long updatePrjIdTime = 0;
    char type[64];

    mInitial = true;
    mStatus = "CoordMsgParserService()::run() Initial successfully!";
    vilslog::printf ("CoordMsgParserService()::run() Initial successfully!\n");

    JSONAgent * jsonAgent = new JSONAgent();
    // available to process.
    while (1)
    {
        //memset(buf, 0, MAXUDPPACKET);
        //memset(clientIP, 0, 64);

        UDPDataItem * item = m_udpqueue.removetimed(1000);

        if ( item != NULL )
        {
            int length = item->getMsgLength();

            if (length > 0)
            {
                jsonAgent->put(item->getMsg(), length);

                bool parsingSuccessful = true;
                Json::Value JSONresult;
                while(parsingSuccessful)
                {
                    parsingSuccessful = jsonAgent->get(JSONresult);
                    if ( !parsingSuccessful )
                    {
                        // int buflen = jsonAgent->getBufferLen();
                        // if (buflen != 0)
                        // {
                        //     vilslog::printf("CoordMsgParserService()::run() parsing failed, jsonAgent->getBufferLen()[%d]\n", jsonAgent->getBufferLen());
                        // }
                        break;
                    }

                    //sprintf(clientIP, "%s", item->getIPAddress());


                    //
                    // check command
                    //
                    if ( JSONresult.isMember("type") )
                    {
                        sprintf(type, "%s", JSONresult["type"].asCString());
#ifdef PRINT_COORD_MSGPARSER_MSG
                        vilslog::printf("CoordMsgParserService()::run() type=%s\n", type);
#endif
                        if ( strcmp(type, "alive") == 0 )
                        {

                            // anchor return panid
                            std::string panid = "";
                            if ( JSONresult.isMember("pid") )
                            {
                                panid = JSONresult["pid"].asString();
                            }

                            std::string mac = JSONresult["mac"].asString();
                            int nodetype = ::atoi(JSONresult["nodetype"].asCString());
                            std::string vlt = JSONresult["vlt"].asString();
                            std::string tem = JSONresult["tem"].asString();
                            int count = ::atoi(JSONresult["count"].asCString());

                            send_alive_to_projects(mac.c_str(), nodetype, vlt.c_str(), tem.c_str(), count, panid.c_str(), item->getIPAddress());

                            // int projectid = getNodeProjectId(JSONresult["mac"].asCString(), nodetype);
                            //
                            // DataItem * jobitem = new DataItem("alive", projectid, JSONresult["mac"].asCString(), nodetype,
                            //         JSONresult["vlt"].asCString(), JSONresult["tem"].asCString(), ::atoi(JSONresult["count"].asCString()), panid.c_str());
                            // m_jobqueue.add(jobitem);
                        }
                        else
                        if ( strcmp(type, "join") == 0 )
                        {
#ifdef PRINT_COORD_MSGPARSER_MSG
                            printf ("CoordMsgParserService()::run() mac[%s] nodetype[%s] version[%s]\n",
                                JSONresult["mac"].asCString(), JSONresult["nodetype"].asCString(), JSONresult["version"].asCString());
#endif
                            std::string mac = JSONresult["mac"].asString();
                            int nodetype = ::atoi(JSONresult["nodetype"].asCString());
                            std::string version = JSONresult["version"].asString();

                            send_join_to_projects(mac.c_str(), nodetype, version.c_str(), item->getIPAddress());

                            // int projectid = getNodeProjectId(JSONresult["mac"].asCString(), nodetype);
                            //
                            // DataItem * jobitem = new DataItem("join", projectid, JSONresult["mac"].asCString(), ::atoi(JSONresult["nodetype"].asCString()),
                            //         JSONresult["version"].asCString());
                            // m_jobqueue.add(jobitem);
                        }
                        else
                        if ( strcmp(type, "rangingdiag") == 0 )
                        {
                            //printf ("CoordIncomingService()::run() do_rangingdiag length[%d] buf[%s]\n", length, buf);
                            do_rangingdiag(JSONresult, item->getIPAddress());
                        }
                        else
                        if ( strcmp(type, "list") == 0 )
                        {
                            //printf ("CoordIncomingService()::run() length[%d] buf[%s]\n", length, buf);
                            do_list(JSONresult, item->getIPAddress());
                            //list_count++;
                        }
                        else
                        if ( strcmp(type, "alert") == 0 )
                        {
                            vilslog::printf ("CoordMsgParserService()::run() type[%s] mac[%s] nodetype[%s] IPAddress[%s]\n",
                                type, JSONresult["mac"].asCString(), JSONresult["nodetype"].asCString(), item->getIPAddress());

                            std::string mac = JSONresult["mac"].asString();
                            int nodetype = ::atoi(JSONresult["nodetype"].asCString());

                            send_alert_to_projects(mac.c_str(), nodetype, item->getIPAddress());

                            // int projectid = getNodeProjectId(JSONresult["mac"].asCString(), nodetype);
                            //
                            // DataItem * jobitem = new DataItem("alert", projectid, JSONresult["mac"].asCString(), ::atoi(JSONresult["nodetype"].asCString()));
                            // jobitem->setIPAddress(item->getIPAddress());
                            // m_jobqueue.add(jobitem);
                        }
                        else
                        if ( strcmp(type, "cancel") == 0 )
                        {
                            // from 7688 coordinator
                            vilslog::printf ("CoordMsgParserService()::run() type[%s] mac[%s] nodetype[%s] IPAddress[%s]\n",
                                type, JSONresult["mac"].asCString(), JSONresult["nodetype"].asCString(), item->getIPAddress());

                            std::string mac = JSONresult["mac"].asString();
                            int nodetype = ::atoi(JSONresult["nodetype"].asCString());

                            send_cancel_to_projects(mac.c_str(), nodetype, item->getIPAddress());

                            // int projectid = getNodeProjectId(JSONresult["mac"].asCString(), nodetype);
                            //
                            // DataItem * jobitem = new DataItem("cancel", projectid, JSONresult["mac"].asCString(), ::atoi(JSONresult["nodetype"].asCString()));
                            // jobitem->setIPAddress(item->getIPAddress());
                            // m_jobqueue.add(jobitem);
                        }
                        else
                        if ( strcmp(type, "emercancel") == 0 )
                        {
                            // from ethernet coordinator
                            vilslog::printf ("CoordMsgParserService()::run() type[%s] mac[%s] IPAddress[%s]\n",
                                type, JSONresult["mac"].asCString(), item->getIPAddress());

                            std::string mac = JSONresult["mac"].asString();
                            int nodetype = NODE_TYPE_TAG;

                            send_cancel_to_projects(mac.c_str(), nodetype, item->getIPAddress());

                            // int projectid = getNodeProjectId(JSONresult["mac"].asCString(), NODE_TYPE_TAG);
                            //
                            // DataItem * jobitem = new DataItem("cancel", projectid, JSONresult["mac"].asCString(), NODE_TYPE_TAG);
                            // jobitem->setIPAddress(item->getIPAddress());
                            // m_jobqueue.add(jobitem);
                        }
                        else
                        if ( strcmp(type, "m0callreset") == 0 )
                        {
                            //
                            // TODO
                            // {"type":"m0callreset"}
                            vilslog::printf ("CoordMsgParserService()::run() type[%s] [%s]\n", type, item->getMsg());
                        }
                        else
                        if ( strcmp(type, "m0norespond") == 0 )
                        {
                            //
                            // TODO
                            // {"type":"m4reset","failed":"1"} 鞥重試次數(DEC)
                            vilslog::printf ("CoordMsgParserService()::run() type[%s] failed[%s] [%s]\n", type, JSONresult["failed"].asCString(), item->getMsg());
                        }
                        else
                        if ( strcmp(type, "m4reset") == 0 )
                        {
                            //
                            // TODO
                            // {"type":"m4reset","err":"1"} //  重啟觸發位置(DEC)
                            vilslog::printf ("CoordMsgParserService()::run() type[%s] err[%s] [%s]\n", type, JSONresult["err"].asCString(), item->getMsg());
                        }
                        else
                        if ( strcmp(type, "validrang") == 0 )
                        {
                            //
                            // TODO
                            // {"type":"valirang","lv1":"3000","lv2":"5000"}
                            vilslog::printf ("CoordMsgParserService()::run() type[%s] lv1[%s] lv2[%s]\n", type, JSONresult["lv1"].asCString(), JSONresult["lv2"].asCString());
                        }
                        else
                        if ( strcmp(type, "blacklist_r") == 0 )
                        {
                            //
                            // TODO
                            // {"type":"blacklist_r","blackList":"012,014"}
                            // vilslog::printf ("CoordMsgParserService()::run() type[%s] blackList[%s]\n", type, JSONresult["blackList"].asCString());
                        }
                        else
                        if ( strcmp(type, "whitelist_r") == 0 )
                        {
                            //
                            // TODO
                            // {"type":"whitelist_r","whiteList":"012,014"}
                            // vilslog::printf ("CoordMsgParserService()::run() type[%s] whiteList[%s]\n", type, JSONresult["whiteList"].asCString());
                        }
                    }
                }
            }
            delete item;
        }

        end1 = getSystemTime();
        difft = end1 - start1;

        updatePrjIdTime += difft;
        if (updatePrjIdTime > 3600000 || mUpdateProject)
        {
            mPrjidList->clear();
            delete mPrjidList;
            mPrjidList = mDeviceManager->getAllProjectConfig();

            updatePrjIdTime = 0;
            mUpdateProject = false;
        }

        start1 = end1;
    }

    //vilslog::printf ("\n\nCoordMsgParserService()::run() Findished!! \n\n\n");
}

void CoordMsgParserService::send_alive_to_projects(const char * mac, int nodetype, const char * vlt, const char * tem, int count, const char * panid, const char * IP)
{
    bool bFound = false;

    // vilslog::printf ("CoordMsgParserService()::send_alive_to_projects() mac[%s] nodetype[%d]\n", mac, nodetype);

    //
    // check every project has this coordinator
    //
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        bool bPrjFound = false;
        std::string nodeid = mDeviceManager->getCoordinatorIdFromIP(projectid, IP);

        // vilslog::printf ("CoordMsgParserService()::send_alive_to_projects() projectid[%d] nodeid[%s]\n", projectid, nodeid.c_str());

        if (nodeid.length() > 0 )
        {
            // exist
            DataItem * jobitem = new DataItem("alive", projectid, mac, nodetype, vlt, tem, count, panid);
            m_jobqueue.add(jobitem);

            int maplayerid = mDeviceManager->getCoordinatorMapLayerFromIP(projectid, IP);
            mDeviceManager->setNodeInfoMapLayer(projectid, nodetype, mac, maplayerid);

            bPrjFound = true;
        }
        if (!bPrjFound)
        {
            bool bExisted = mDeviceManager->checkNodeExisted(projectid, nodetype, mac);

            // vilslog::printf ("CoordMsgParserService()::send_alive_to_projects() checkNodeExisted projectid[%d] bExisted[%d]\n", projectid, bExisted);

            if ( bExisted )
            {
                DataItem * jobitem = new DataItem("alive", projectid, mac, nodetype, vlt, tem, count, panid);
                m_jobqueue.add(jobitem);
                bPrjFound = true;
            }
        }

        if (bPrjFound)
        {
            mDeviceManager->setNodeInfoIP(projectid, nodetype, mac, IP);
            bFound = true;
        }
    }

    if (!bFound)
    {
        DataItem * jobitem = new DataItem("alive", 1, mac, nodetype, vlt, tem, count, panid);
        m_jobqueue.add(jobitem);
        mDeviceManager->setNodeInfoIP(1, nodetype, mac, IP);
    }
}

void CoordMsgParserService::send_join_to_projects(const char * mac, int nodetype, const char * version, const char * IP)
{
    bool bFound = false;

    // vilslog::printf ("CoordMsgParserService()::send_join_to_projects() mac[%s] nodetype[%d]\n", mac, nodetype);

    //
    // check every project has this coordinator
    //
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        bool bPrjFound = false;
        std::string nodeid =  mDeviceManager->getCoordinatorIdFromIP(projectid, IP);

        // vilslog::printf ("CoordMsgParserService()::send_join_to_projects() projectid[%d] nodeid[%s]\n", projectid, nodeid.c_str());

        if (nodeid.length() > 0 )
        {
            // exist
            DataItem * jobitem = new DataItem("join", projectid, mac, nodetype, version);
            m_jobqueue.add(jobitem);

            int maplayerid = mDeviceManager->getCoordinatorMapLayerFromIP(projectid, IP);
            mDeviceManager->setNodeInfoMapLayer(projectid, nodetype, mac, maplayerid);

            bPrjFound = true;
        }
        if (!bPrjFound)
        {
            bool bExisted = mDeviceManager->checkNodeExisted(projectid, nodetype, mac);

            // vilslog::printf ("CoordMsgParserService()::send_join_to_projects() checkNodeExisted projectid[%d] bExisted[%d]\n", projectid, bExisted);

            if ( bExisted )
            {
                DataItem * jobitem = new DataItem("join", projectid, mac, nodetype, version);
                m_jobqueue.add(jobitem);
                bPrjFound = true;
            }
        }
        if (bPrjFound)
        {
            mDeviceManager->setNodeInfoIP(projectid, nodetype, mac, IP);
            bFound = true;
        }
    }

    if (!bFound)
    {
        DataItem * jobitem = new DataItem("join", 1, mac, nodetype, version);
        m_jobqueue.add(jobitem);
        mDeviceManager->setNodeInfoIP(1, nodetype, mac, IP);
    }
}

void CoordMsgParserService::send_alert_to_projects(const char * mac, int nodetype, const char * IP)
{
    bool bFound = false;
    //
    // check every project has this coordinator
    //
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        bool bPrjFound = false;
        std::string nodeid =  mDeviceManager->getCoordinatorIdFromIP(projectid, IP);
        if (nodeid.length() > 0 )
        {
            // exist
            DataItem * jobitem = new DataItem("alert", projectid, mac, nodetype);
            m_jobqueue.add(jobitem);

            int maplayerid = mDeviceManager->getCoordinatorMapLayerFromIP(projectid, IP);
            mDeviceManager->setNodeInfoMapLayer(projectid, nodetype, mac, maplayerid);

            bPrjFound = true;
        }
        if (!bPrjFound)
        {
            bool bExisted = mDeviceManager->checkNodeExisted(projectid, nodetype, mac);
            if ( bExisted )
            {
                DataItem * jobitem = new DataItem("alert", projectid, mac, nodetype);
                m_jobqueue.add(jobitem);
                bPrjFound = true;
            }
        }
        if (bPrjFound)
        {
            mDeviceManager->setNodeInfoIP(projectid, nodetype, mac, IP);
            bFound = true;
        }
    }

    if (!bFound)
    {
        DataItem * jobitem = new DataItem("alert", 1, mac, nodetype);
        m_jobqueue.add(jobitem);
        mDeviceManager->setNodeInfoIP(1, nodetype, mac, IP);
    }
}

void CoordMsgParserService::send_cancel_to_projects(const char * mac, int nodetype, const char * IP)
{
    bool bFound = false;
    //
    // check every project has this coordinator
    //
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        bool bPrjFound = false;
        std::string nodeid =  mDeviceManager->getCoordinatorIdFromIP(projectid, IP);
        if (nodeid.length() > 0 )
        {
            // exist
            DataItem * jobitem = new DataItem("cancel", projectid, mac, nodetype);
            m_jobqueue.add(jobitem);

            int maplayerid = mDeviceManager->getCoordinatorMapLayerFromIP(projectid, IP);
            mDeviceManager->setNodeInfoMapLayer(projectid, nodetype, mac, maplayerid);

            bPrjFound = true;
        }
        if (!bPrjFound)
        {
            bool bExisted = mDeviceManager->checkNodeExisted(projectid, nodetype, mac);
            if ( bExisted )
            {
                DataItem * jobitem = new DataItem("cancel", projectid, mac, nodetype);
                m_jobqueue.add(jobitem);
                bPrjFound = true;
            }
        }
        if (bPrjFound)
        {
            mDeviceManager->setNodeInfoIP(projectid, nodetype, mac, IP);
            bFound = true;
        }
    }

    if (!bFound)
    {
        DataItem * jobitem = new DataItem("cancel", 1, mac, nodetype);
        m_jobqueue.add(jobitem);
        mDeviceManager->setNodeInfoIP(1, nodetype, mac, IP);
    }
}

void CoordMsgParserService::send_rangingdiag_to_projects(int anchoridx, int tagidx, int rangingidx, int distance, int * imu,
                    int TagPollCnt, int RangingAnchorCnt, int TagFinalRecvdBm, int TagFirstPathdBm,
                    int AnchorRecvdBm, int AnchorFirstPathdBm, int RangingCount, int RangingFailCount, float DataRate, const char * IP)

{
    bool bFound = false;
    //
    // check every project has this coordinator
    //
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::string nodeid =  mDeviceManager->getCoordinatorIdFromIP(projectid, IP);
        if (nodeid.length() > 0 )
        {
            // exist
            DataItem * item = new DataItem("rangingdiag", projectid, anchoridx, tagidx, rangingidx, distance, imu, TagPollCnt, RangingAnchorCnt,
                            TagFinalRecvdBm, TagFirstPathdBm, AnchorRecvdBm, AnchorFirstPathdBm, RangingCount, RangingFailCount, DataRate, IP);
            m_jobqueue.add(item);

            bFound = true;
        }
    }

    if (!bFound)
    {
        DataItem * item = new DataItem("rangingdiag", 1, anchoridx, tagidx, rangingidx, distance, imu, TagPollCnt, RangingAnchorCnt,
                        TagFinalRecvdBm, TagFirstPathdBm, AnchorRecvdBm, AnchorFirstPathdBm, RangingCount, RangingFailCount, DataRate, IP);
        m_jobqueue.add(item);
    }
}

void CoordMsgParserService::send_rangingdiag_1_to_projects(std::string anchor_mac, std::string tag_mac, int rangingidx, int distance, int * imu,
                    int TagPollCnt, int RangingAnchorCnt, int TagFinalRecvdBm, int TagFirstPathdBm,
                    int AnchorRecvdBm, int AnchorFirstPathdBm, int RangingCount, int RangingFailCount, float DataRate, const char * IP)
{
    bool bFound = false;
    //
    // check every project has this coordinator
    //
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::string nodeid =  mDeviceManager->getCoordinatorIdFromIP(projectid, IP);
        if (nodeid.length() > 0 )
        {
            // exist
            DataItem * item = new DataItem("rangingdiag_1", projectid, anchor_mac, tag_mac, rangingidx, distance, imu, TagPollCnt, RangingAnchorCnt,
                            TagFinalRecvdBm, TagFirstPathdBm, AnchorRecvdBm, AnchorFirstPathdBm, RangingCount, RangingFailCount, DataRate, IP);
            m_jobqueue.add(item);
            bFound = true;
        }
    }

    if (!bFound)
    {
        DataItem * item = new DataItem("rangingdiag_1", 1, anchor_mac, tag_mac, rangingidx, distance, imu, TagPollCnt, RangingAnchorCnt,
                        TagFinalRecvdBm, TagFirstPathdBm, AnchorRecvdBm, AnchorFirstPathdBm, RangingCount, RangingFailCount, DataRate, IP);
        m_jobqueue.add(item);
    }
}

void CoordMsgParserService::send_list_to_projects(int nodetype, std::list<NodeStruct*> * plist, const char * IP)
{
    bool bFound = false;
    //
    // check every project has this coordinator
    //
    for (std::list<int>::iterator prj = mPrjidList->begin(); prj != mPrjidList->end(); prj++)
    {
        int projectid = (*prj);
        std::string nodeid =  mDeviceManager->getCoordinatorIdFromIP(projectid, IP);
        if (nodeid.length() > 0 )
        {
            // exist
            DataItem * item = new DataItem("list", projectid, nodetype, plist);
            m_jobqueue.add(item);
            bFound = true;
        }
    }

    if (!bFound)
    {
        DataItem * item = new DataItem("list", 1, nodetype, plist);
        m_jobqueue.add(item);
    }
}

int CoordMsgParserService::do_rangingdiag(Json::Value & JSONresult, const char * IP)
{
    int IMU[6] = {0,0,0,0,0,0};
    char linebuffer[64];

    //vilslog::printf ("CoordMsgParserService()::do_rangingdiag() IP[%s]\n", IP);
    memset(linebuffer, 0, 64);

    if ( JSONresult.isMember("imu") )
    {
        sprintf(linebuffer, "%s", JSONresult["imu"].asCString());
    }

    //printf ("CoordIncomingService()::run() length[%d] buf[%s]\n", length, buf);

    // MPU9250 IMU data
    int i = 0;
    int length = (int)strlen(linebuffer);
	int imuidx = 0;
	char numstr[10];
	while( i < length )
	{
		int index = 0;
		while(linebuffer[i] != ':' && i < length)
		{
			numstr[index++] = linebuffer[i++];
		}
		numstr[index] = '\0';
		IMU[imuidx++] = atoi(numstr);
        i++;
	}

    //printf ("CoordIncomingService()::do_rangingdiag() AnRnTbl[%s]\n", JSONresult["AnRnTbl"].asCString());
    std::string anchor_mac = "";
    std::string tag_mac = "";
    int anchor_idx = -1;
    int tag_idx = -1;
    int ranging_idx = -1;
    int distance = -1;
    int PCnt = -1;
    int AnCnt = -1;
    int TagRecv = -1;
    int TagFP = -1;
    int AnRecv = -1;
    int AnFP = -1;
    int RnCnt = -1;
    int RnFCnt = -1;
    float DaRate = -1;
    //char AnchorRangingTable[128];
    //memset(AnchorRangingTable, 0, 128);

    if ( JSONresult.isMember("anchor") )
    {
       std::string anchor = JSONresult["anchor"].asString();
       if (anchor.length() > 3)
       {
           anchor_mac = anchor;
       }
       else
       {
           anchor_idx = std::atoi(JSONresult["anchor"].asCString());
       }
    }

    if ( JSONresult.isMember("tag") )
    {
        std::string tag = JSONresult["tag"].asString();
        if (tag.length() > 3)
        {
            tag_mac = tag;
        }
        else
        {
            tag_idx = std::atoi(JSONresult["tag"].asCString());
        }
    }

    if ( JSONresult.isMember("idx") )
    {
        ranging_idx = std::atoi(JSONresult["idx"].asCString());
    }

    if ( JSONresult.isMember("dist") )
    {
        distance = std::atoi(JSONresult["dist"].asCString());
    }

    if ( JSONresult.isMember("PCnt") )
    {
        PCnt = std::atoi(JSONresult["PCnt"].asCString());
    }

    if ( JSONresult.isMember("AnCnt") )
    {
        AnCnt = std::atoi(JSONresult["AnCnt"].asCString());
    }

    if ( JSONresult.isMember("TagRecv") )
    {
        TagRecv = std::atoi(JSONresult["TagRecv"].asCString());
    }

    if ( JSONresult.isMember("TagFP") )
    {
        TagFP = std::atoi(JSONresult["TagFP"].asCString());
    }

    if ( JSONresult.isMember("AnRecv") )
    {
        AnRecv = std::atoi(JSONresult["AnRecv"].asCString());
    }

    if ( JSONresult.isMember("AnFP") )
    {
        AnFP = std::atoi(JSONresult["AnFP"].asCString());
    }

    if ( JSONresult.isMember("RnCnt") )
    {
        RnCnt = std::atoi(JSONresult["RnCnt"].asCString());
    }

    if ( JSONresult.isMember("RnFCnt") )
    {
        RnFCnt = std::atoi(JSONresult["RnFCnt"].asCString());
    }

    if ( JSONresult.isMember("DaRate") )
    {
        DaRate = std::atof(JSONresult["DaRate"].asCString());
    }

    // if ( JSONresult.isMember("AnRnTbl") )
    // {
    //     sprintf(AnchorRangingTable, "%s", JSONresult["AnRnTbl"].asCString());
    // }

    if ( anchor_mac.length() > 0 || tag_mac.length() > 0 )
    {
        // vilslog::printf ("CoordMsgParserService::do_rangingdiag() rangingdiag_1 anchor_mac[%s] tag_mac[%s]\n",
        //     anchor_mac.c_str(), tag_mac.c_str());

        send_rangingdiag_1_to_projects(anchor_mac, tag_mac, ranging_idx, distance, IMU, PCnt, AnCnt,
                        TagRecv, TagFP, AnRecv, AnFP, RnCnt, RnFCnt, DaRate, IP);

        // int projectid = getNodeProjectId(tag_mac.c_str(), NODE_TYPE_TAG);
        //
        // DataItem * item = new DataItem("rangingdiag_1", projectid, anchor_mac, tag_mac, ranging_idx, distance, IMU, PCnt, AnCnt,
        //                 TagRecv, TagFP, AnRecv, AnFP, RnCnt, RnFCnt, DaRate, IP);
        // m_jobqueue.add(item);
    }
    else
    {

        send_rangingdiag_to_projects(anchor_idx, tag_idx, ranging_idx, distance, IMU, PCnt, AnCnt,
                        TagRecv, TagFP, AnRecv, AnFP, RnCnt, RnFCnt, DaRate, IP);

        // DataItem * item = new DataItem("rangingdiag", 1, anchor_idx, tag_idx, ranging_idx, distance, IMU, PCnt, AnCnt,
        //                 TagRecv, TagFP, AnRecv, AnFP, RnCnt, RnFCnt, DaRate, IP);
        // m_jobqueue.add(item);
    }

    return 0;
}

int CoordMsgParserService::do_list(Json::Value & JSONresult, const char * IP)
{
    if ( !JSONresult.isMember("nodelist") )
    {
        vilslog::printf ("CoordIncomingService()::do_list() nodelist not exist!\n");
        return 0;
    }
    if ( !JSONresult["nodelist"].isArray() )
    {
        vilslog::printf ("CoordIncomingService()::do_list() nodelist not a array\n");
        return 0;
    }
    //printf ("CenterIncomingService()::do_list() nodelist size[%d]\n", JSONresult["nodelist"].size());

    int listprojectid = -1;
    std::list<NodeStruct*> nodeList;
    int listlen = JSONresult["nodelist"].size();
    for(int i=0;i<listlen;i++)
    {
        Json::Value node = JSONresult["nodelist"][i];

        //printf ("CenterIncomingService()::do_list() [%d] nodetype[%s] mac[%s] index[%s]\n", i,
        //                    node["nodetype"].asCString(), node["mac"].asCString(), node["index"].asCString());

        int index = -1;
        int nodetype = -1;
        int alive = -1;
        char version[32];
        char macaddress[32];
        memset(version, 0, 32);
        memset(macaddress, 0, 32);

        if ( node.isMember("index") )
        {
            index = std::atoi(node["index"].asCString());
        }

        if ( node.isMember("nodetype") )
        {
            nodetype = std::atoi(node["nodetype"].asCString());
        }

        if ( node.isMember("alive") )
        {
            alive = std::atoi(node["alive"].asCString());
        }

        if ( node.isMember("version") )
        {
            snprintf(version, 32, "%s", node["version"].asCString());
        }

        if ( node.isMember("mac") )
        {
            snprintf(macaddress, 32, "%s", node["mac"].asCString());
        }

        NodeStruct * newNode = createNodeStruct();
        newNode->index = index;
        newNode->nodetype = nodetype;// 0:Anchor 1:Tag 2:Coordinator
        newNode->alive = alive;
        snprintf(newNode->version, 32, "%s", version);
        snprintf(newNode->macaddress, 32,"%s", macaddress);

        int projectid = getNodeProjectId(macaddress, nodetype);
        newNode->projectid = projectid;

        if (listprojectid == -1)
        {
            listprojectid = projectid;
        }

#ifdef PRINT_COORD_MSGPARSER_MSG
        printf ("CoordMsgParserService()::do_list() index[%d] nodetype[%d] alive[%d] version[%s] macaddress[%s]\n",
            newNode->index, newNode->nodetype, newNode->alive, newNode->version, newNode->macaddress);
#endif
        if ( newNode->nodetype == NODE_TYPE_TAG )
        {
            if ( node.isMember("v") )
            {
                sprintf(newNode->voltage, "%s", node["v"].asCString());
            }
            if ( node.isMember("t") )
            {
                sprintf(newNode->temperature, "%s", node["t"].asCString());
            }
            if ( node.isMember("rl") )
            {
                sprintf(newNode->rangingList, "%s", node["rl"].asCString());
                //vilslog::printf ("CoordIncomingService()::do_list() [%d] mac[%s] rl[%s]\n", i, newNode->macaddress, node["rl"].asCString());
            }
            if ( node.isMember("p") )
            {
                sprintf(newNode->panid, "%s", node["p"].asCString());
            }

#ifdef PRINT_COORD_MSGPARSER_MSG
            printf ("CoordMsgParserService()::do_list() voltage[%s] temperature[%s] rangingList[%s] panid[%s]\n",
                newNode->voltage, newNode->temperature, newNode->rangingList, newNode->panid);
#endif
        }
        else
        if ( newNode->nodetype == NODE_TYPE_COORD )
        {
            // app version, ip address, process id
            if ( node.isMember("ip") )
            {
                //printf ("CoordIncomingService()::do_list() [%d] mac[%s] ip[%s]\n", i, newNode->macaddress, node["ip"].asCString());
                sprintf(newNode->IP, "%s", node["ip"].asCString());
            }
            if ( node.isMember("app") )
            {
                //printf ("CoordIncomingService()::do_list() [%d] mac[%s] app[%s]\n", i, newNode->macaddress, node["app"].asCString());
                sprintf(newNode->AppVersion, "%s", node["app"].asCString());
            }
            if ( node.isMember("pid") )
            {
                //printf ("CoordIncomingService()::do_list() [%d] mac[%s] pid[%s]\n", i, newNode->macaddress, node["pid"].asCString());
                sprintf(newNode->PID, "%s", node["pid"].asCString());
            }
            if ( node.isMember("panid") )
            {
                //printf ("CoordIncomingService()::do_list() [%d] mac[%s] panid[%s]\n", i, newNode->macaddress, node["panid"].asCString());
                sprintf(newNode->panid, "%s", node["panid"].asCString());
            }
            if ( node.isMember("time") )
            {
                sprintf(newNode->time, "%s", node["time"].asCString());
            }

#ifdef PRINT_COORD_MSGPARSER_MSG
            printf ("CoordMsgParserService()::do_list() IP[%s] AppVersion[%s] PID[%s] panid[%s] time[%s]\n",
                newNode->IP, newNode->AppVersion, newNode->PID, newNode->panid, newNode->time);
#endif
        }

        nodeList.push_back(newNode);
    }
    int nodetyoe = -1;
    if ( JSONresult.isMember("nodetype") )
    {
        nodetyoe = std::atoi(JSONresult["nodetype"].asCString());
    }

    send_list_to_projects(nodetyoe, &nodeList, IP);

    // DataItem * item = new DataItem("list", listprojectid, nodetyoe, &nodeList);
    // m_jobqueue.add(item);

    clear_node_list(&nodeList);

    return 0;
}

int CoordMsgParserService::getNodeProjectId(const char * mac, int nodetype)
{
    int projectid = 1;

    if (nodetype == NODE_TYPE_ANCHOR )
    {
        projectid = mDeviceManager->getAnchorProjectId(mac);
    }
    if (nodetype == NODE_TYPE_TAG )
    {
        projectid = mDeviceManager->getTagProjectId(mac);
    }
    if (nodetype == NODE_TYPE_COORD )
    {
        projectid = mDeviceManager->getCoordinatorProjectId(mac);
    }
    if (nodetype == NODE_TYPE_LOCATOR )
    {
        projectid = mDeviceManager->getLocatorProjectId(mac);
    }

    return projectid;
}
