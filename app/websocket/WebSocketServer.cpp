/*
 *  WebSocketServer.cpp
 *
 *  Created on: 2018/09/26
 *      Author: ikki
 */

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include <string.h>
#include <list>
#include <ifaddrs.h>

#include "../common.h"
#include "WebSocketServer.h"

WebSocketServer::WebSocketServer(int port)
    : m_next_sessionid(1), m_server_port(port)
{

}

void * WebSocketServer::run()
{
    mInitial = false;
    mHandlering = false;
    mSetting = false;

    //
    // [info] asio listen error: system:98 (Address already in use)
    //
    m_server.set_reuse_addr(true);

    //m_server.set_message_handler(&on_message);
    m_server.set_open_handler(bind(&WebSocketServer::on_open,this,::_1));
    m_server.set_close_handler(bind(&WebSocketServer::on_close,this,::_1));
    m_server.set_message_handler(bind(&WebSocketServer::on_message,this,::_1,::_2));


    //m_server.set_access_channels( websocketpp::log::alevel::all );
    //m_server.set_access_channels( websocketpp::log::alevel::devel );
    //m_server.set_access_channels( websocketpp::log::elevel::rerror );
    m_server.set_access_channels( websocketpp::log::alevel::none );
    m_server.clear_access_channels( websocketpp::log::alevel::frame_payload );

    m_server.init_asio();
    m_server.listen(m_server_port);

    websocketpp::lib::error_code ec;
    m_server.start_accept(ec);

    if (ec)
    {
        mInitial = false;
        std::ostringstream statusstream;
        statusstream<<"error_code:"<<ec;

        mStatus = statusstream.str();
    }
    else
    {
        mInitial = true;
        mStatus = "";
    }

    m_server.run();

    return NULL;
}

int WebSocketServer::setDataItem(SubscribeDataItem * dataItem)
{
    // vilslog::printf("WebSocketServer::setDataItem() m_msgType[%s]\n", dataItem->m_msgType.c_str());

    while(mHandlering)
    {
        vilslog::printf ("WebSocketServer::setDataItem() mHandlering[%d]\n", mHandlering);
        usleep(10000);// 10 ms
    }
    mSetting = true;

    // bool validSend = checkValidSendWebsocket(dataItem);
    // if ( !validSend )
    // {
    //     vilslog::printf ("WebSocketServer::setDataItem() m_msgType[%s] m_tagMacAddress[%s] not valid validSend!\n",
    //         dataItem->m_msgType.c_str(), dataItem->m_tagMacAddress.c_str());
    //     mSetting = false;
    //     return 0;
    // }

    if ( dataItem->m_msgType.compare("sos") == 0 )
    {
        do_send_sos(dataItem);
    }
    else
    if ( dataItem->m_msgType.compare("power") == 0 )
    {
        do_send_power(dataItem);
    }
    else
    if ( dataItem->m_msgType.compare("powerAlert") == 0 )
    {
        do_send_powerAlert(dataItem);
    }
    else
    if ( dataItem->m_msgType.compare("fenceAlert") == 0 )
    {
        do_send_fenceAlert(dataItem);
    }
    else
    if ( dataItem->m_msgType.compare("coord") == 0 )
    {
        do_send_coord(dataItem);
    }
    else
    if ( dataItem->m_msgType.compare("checkpointAlert") == 0 )
    {
        //printf("WebSocketServer::setDataItem() do_send_checkpointAlert!\n");
        do_send_checkpointAlert(dataItem);
    }
    else
    if ( dataItem->m_msgType.compare("event") == 0 )
    {
        //printf("WebSocketServer::setDataItem() do_send_event...\n");
        do_send_event(dataItem);
        //printf("WebSocketServer::setDataItem() do_send_event Finished!\n");
    }
    else
    if ( dataItem->m_msgType.compare("callout") == 0 )
    {
        //printf("WebSocketServer::setDataItem() do_send_event...\n");
        do_send_callout(dataItem);
        //printf("WebSocketServer::setDataItem() do_send_event Finished!\n");
    }
    else
    if ( dataItem->m_msgType.compare("cardrollcallevent") == 0 )
    {
        //printf("WebSocketServer::setDataItem() do_send_event...\n");
        do_send_cardrollcallevent(dataItem);
        //printf("WebSocketServer::setDataItem() do_send_event Finished!\n");
    }
    else
    if ( dataItem->m_msgType.compare("firefightingevent") == 0 )
    {
        //printf("WebSocketServer::setDataItem() do_send_event...\n");
        do_send_firefightingevent(dataItem);
        //printf("WebSocketServer::setDataItem() do_send_event Finished!\n");
    }
    else
    {
        vilslog::printf("WebSocketServer::setDataItem() Do Nothing!!\n");
    }
    //printf("WebSocketServer::setDataItem() Finished!\n");
    mSetting = false;
    return 0;
}


void WebSocketServer::on_open(connection_hdl hdl)
{
    subscribe_data data;
    server::connection_ptr con = m_server.get_con_from_hdl( hdl );

    mHandlering = true;

    data.sessionid = m_next_sessionid++;
    data.msg.clear();
    data.uri = con->get_resource();

    std::string remote_endpoint_info = con->get_remote_endpoint();

    vilslog::printf("WebSocketServer::on_open() Opening connection[%s] with sessionid[%d] endpoint:[%s]\n", data.uri.c_str(), data.sessionid, remote_endpoint_info.c_str());

    bool valid = parse_subscribe_data(data.uri, data);
    if (valid)
    {
        put_to_list(hdl, data);
    }
    else
    {
        vilslog::printf("WebSocketServer::on_open() Not a valid subscribe_data!\n");
    }
    m_connections[ hdl ] = data;

    mHandlering = false;
}

void WebSocketServer::on_close(connection_hdl hdl)
{
    subscribe_data& data = get_data_from_hdl(hdl);

    mHandlering = true;

    vilslog::printf("WebSocketServer::on_open() Closing with sessionid[%d]\n", data.sessionid);

    remove_from_list(data);

    // vilslog::printf("Closing connection: 2 \n");

    m_connections.erase(hdl);

    // vilslog::printf("Closing connection: Finish! \n");

    mHandlering = false;

}

void WebSocketServer::on_message(connection_hdl hdl, server::message_ptr msg)
{
    subscribe_data& data = get_data_from_hdl(hdl);

    mHandlering = true;

    data.msg = msg->get_payload();

    vilslog::printf("WebSocketServer::on_message() Got a message from connection: %s with sessionid: %d msg: %s\n",
        data.uri.c_str(), data.sessionid, data.msg.c_str());

    //
    // test send
    //
    //m_server.send(hdl, "{\"msgType\":\"sos\"}", websocketpp::frame::opcode::text);

    mHandlering = false;
}

subscribe_data& WebSocketServer::get_data_from_hdl(connection_hdl hdl)
{
    //auto it = m_connections.find(hdl);
    std::map<connection_hdl, subscribe_data>::iterator it = m_connections.find(hdl);

    if (it == m_connections.end())
    {
        vilslog::printf("WebSocketServer::get_data_from_hdl(): this connection is not in the list\n");

        // this connection is not in the list. This really shouldn't happen
        // and probably means something else is wrong.
        throw std::invalid_argument("WebSocketServer::get_data_from_hdl(): No data available for session");
    }

    return it->second;
}

bool WebSocketServer::parse_subscribe_data(std::string uri, subscribe_data & data)
{
    bool valid = false;

    std::string URIString = uri;
    replaceAll(URIString, "%7C", "|");
    //printf("CenterWebSocketService::parse_subscribe_data() URIString[%s]\n", URIString.c_str());
    replaceAll(URIString, "%3F", "?");
    // vilslog::printf("WebSocketServer::parse_subscribe_data() URIString[%s]\n", URIString.c_str());

    //=================================
    //
    //  /websocket/tag_tagid_type|sos|power?encoding=text
    //  /websocket/prj_prjid_type|sos|power?encoding=text
    //
    //  /websocket/[projectCode]_[ID]_2D[stamp][api_token]_types
    //  /websocket/tag_[tagMacs]_2D[stamp][api_token]_types
    //
    int start_pos = 0;
    std::size_t tagpos = URIString.find("/websocket/tag_");
    if ( tagpos != std::string::npos )
    {
        // tag subscription
        data.subscribeType = "tag";
        start_pos = strlen("/websocket/tag_");
        // vilslog::printf("WebSocketServer::parse_subscribe_data() tag subscription.\n");
    }
    //std::size_t prjpos = URIString.find("/websocket/prj_");
    std::size_t prjpos = URIString.find("/websocket/xystudy_");
    if ( prjpos != std::string::npos )
    {
        // project subscription
        data.subscribeType = "prj";
        start_pos = strlen("/websocket/xystudy_");
        // vilslog::printf("WebSocketServer::parse_subscribe_data() project subscription.\n");
    }

    std::size_t maingroupspos = URIString.find("/websocket/maingroups_");
    if ( maingroupspos != std::string::npos )
    {
        // project subscription
        data.subscribeType = "maingroups";
        start_pos = strlen("/websocket/maingroups_");
        // vilslog::printf("WebSocketServer::parse_subscribe_data() project subscription.\n");
    }

    if ( tagpos == std::string::npos &&
         prjpos == std::string::npos &&
         maingroupspos == std::string::npos )
    {

        // vilslog::printf("WebSocketServer::parse_subscribe_data() tagpos[%lu] prjpos[%lu]\n", tagpos, prjpos);
        return false;
    }
    //=================================

    std::size_t tagidspos = URIString.find("_", start_pos+1);
    std::size_t typespos = URIString.find("_type");

    if ( typespos != std::string::npos )
    {
        // /websocket/tag_1,2_type|sos|power

        std::string tagids = URIString.substr(start_pos, tagidspos - start_pos);
        // vilslog::printf("WebSocketServer::parse_subscribe_data() tagids[%s]\n", tagids.c_str());

        //int count = parse_pattern_id(tagids, ",",  data.itemIDs);
        parse_pattern_id(tagids, ",",  data.itemIDs);
        // vilslog::printf("WebSocketServer::parse_subscribe_data() tagids[%s] count[%d] itemIDs.size()[%lu]\n",
        //      tagids.c_str(), count, data.itemIDs.size());

        //std::size_t typesStartpos = URIString.find("|", tagidspos + 1);
        std::size_t typesStartpos = URIString.find("|", typespos + 1);
        if (typesStartpos == std::string::npos)
        {
            vilslog::printf("WebSocketServer::parse_subscribe_data() error !! tagids[%s]\n", tagids.c_str());
            return false;
        }

        std::size_t typesEndpos = URIString.find("?", typesStartpos + 1);
        std::string types = "";
        if (typesEndpos != std::string::npos)
        {
            types = URIString.substr(typesStartpos + 1, typesEndpos - typesStartpos - 1);
        }
        else
        {
            types = URIString.substr(typesStartpos + 1, uri.size() - typesStartpos - 1);
        }
        parse_pattern_id(types, "|",  data.types);
        // vilslog::printf("WebSocketServer::parse_subscribe_data() types[%s]\n", types.c_str());

        valid = true;
    }
    else
    {
        //
        // subscribe all types
        //
        // vilslog::printf("WebSocketServer::parse_subscribe_data() subscribe all types, start_pos[%d]\n", start_pos);

        std::string tagids = "";
        std::size_t tagidsendpos = URIString.find("_2D", start_pos+1);
        if (tagidsendpos != std::string::npos)
        {
            // /websocket/tag_0610000000000126,4210000000001303_2D1531973179180j8teuD5A4A
            tagids = URIString.substr(start_pos, tagidsendpos - start_pos);
        }
        else
        {
            vilslog::printf("WebSocketServer::parse_subscribe_data() error subscribe format!! _2D not found!!\n");
            return false;
        }

        // std::size_t tagidsendpos = URIString.find("?", start_pos+1);
        // std::string tagids = "";
        // if (tagidsendpos != std::string::npos)
        // {
        //     // /websocket/tag_1,2?encoding=tex
        //     vilslog::printf("CenterWebSocketService::parse_subscribe_data() tagidsendpos[%lu]\n", tagidsendpos);
        //     tagids = URIString.substr(start_pos, tagidsendpos - start_pos);
        // }
        // else
        // {
        //     // /websocket/tag_1,2
        //     vilslog::printf("CenterWebSocketService::parse_subscribe_data() uri.size()[%lu]\n", uri.size());
        //     tagids = URIString.substr(start_pos, uri.size() - start_pos);
        // }

        //int count = parse_pattern_id(tagids, ",", data.itemIDs);
        parse_pattern_id(tagids, ",", data.itemIDs);
        // vilslog::printf("WebSocketServer::parse_subscribe_data() tagids[%s] count[%d] itemIDs.size()[%lu]\n",
        //     tagids.c_str(), count, data.itemIDs.size());
        valid = true;
    }

    return valid;
}

int WebSocketServer::parse_pattern_id(std::string target_str, std::string pattern, std::list<std::string> & itemIDs)
{
    int itme_count = 0;
    std::string itemid = "";
    std::size_t start_pos = 0;
    std::size_t pattern_pos = target_str.find(pattern);
    while(pattern_pos != std::string::npos)
    {
        itemid = target_str.substr(start_pos, pattern_pos - start_pos);
        itemIDs.push_back(itemid);
        //printf("CenterWebSocketService::parse_pattern_id() itme_count[%d] itemid[%s]\n", itme_count, itemid.c_str());

        start_pos = pattern_pos + 1;
        pattern_pos = target_str.find(pattern, pattern_pos + 1);
        itme_count++;
    }

    itemid = target_str.substr(start_pos, target_str.size() - start_pos);
    itemIDs.push_back(itemid);
    //printf("CenterWebSocketService::parse_pattern_id() itme_count[%d] itemid[%s]\n", itme_count, itemid.c_str());

    itme_count++;

    return itme_count;
}


int WebSocketServer::put_to_list(connection_hdl hdl, subscribe_data data)
{
    if (data.types.size() == 0)
    {
        // subscribe all
        vilslog::printf("CenterWebSocketService::put_to_list() subscribe all!!\n");
        subscribe_item * item1 = create_subscribe_item(hdl, data);
        m_sos_List.push_back(item1);

        subscribe_item * item2 = create_subscribe_item(hdl, data);
        m_power_List.push_back(item2);

        subscribe_item * item3 = create_subscribe_item(hdl, data);
        m_powerAlert_List.push_back(item3);

        subscribe_item * item4 = create_subscribe_item(hdl, data);
        m_fenceAlert_List.push_back(item4);

        subscribe_item * item5 = create_subscribe_item(hdl, data);
        m_coord_List.push_back(item5);

        subscribe_item * item6 = create_subscribe_item(hdl, data);
        m_checkpointAlert_List.push_back(item6);

        subscribe_item * item7 = create_subscribe_item(hdl, data);
        m_event_List.push_back(item7);

        subscribe_item * item8 = create_subscribe_item(hdl, data);
        m_cardrollcallevent_List.push_back(item8);

        subscribe_item * item9 = create_subscribe_item(hdl, data);
        m_firefightingevent_List.push_back(item9);
        return 0;
    }

    for (std::list<std::string>::iterator type = data.types.begin(); type != data.types.end(); type++)
    {
        //printf("CenterWebSocketService::put_to_list() (*type)[%s]\n", (*type).c_str());

        //subscribe_item * item = new subscribe_item();
        //item->sessionid = data.sessionid;
        //item->hdl = hdl;
        //item->subscribeType = data.subscribeType;
        //item->itemIDs = data.itemIDs; // copy list items

        if ( (*type).compare("sos") == 0 )
        {
            subscribe_item * item = create_subscribe_item(hdl, data);
            m_sos_List.push_back(item);
        }
        else
        if ( (*type).compare("power") == 0 )
        {
            subscribe_item * item = create_subscribe_item(hdl, data);
            m_power_List.push_back(item);
        }
        else
        if ( (*type).compare("powerAlert") == 0 )
        {
            subscribe_item * item = create_subscribe_item(hdl, data);
            m_powerAlert_List.push_back(item);
        }
        else
        if ( (*type).compare("fenceAlert") == 0 )
        {
            subscribe_item * item = create_subscribe_item(hdl, data);
            m_fenceAlert_List.push_back(item);
        }
        else
        if ( (*type).compare("coord") == 0 )
        {
            subscribe_item * item = create_subscribe_item(hdl, data);
            m_coord_List.push_back(item);
        }
        else
        if ( (*type).compare("checkpointAlert") == 0 )
        {
            subscribe_item * item = create_subscribe_item(hdl, data);
            m_checkpointAlert_List.push_back(item);
        }
        else
        if ( (*type).compare("event") == 0 )
        {
            subscribe_item * item = create_subscribe_item(hdl, data);
            m_event_List.push_back(item);
        }
        else
        if ( (*type).compare("cardrollcallevent") == 0 )
        {
            subscribe_item * item = create_subscribe_item(hdl, data);
            m_cardrollcallevent_List.push_back(item);
        }
        else
        if ( (*type).compare("firefightingevent") == 0 )
        {
            subscribe_item * item = create_subscribe_item(hdl, data);
            m_firefightingevent_List.push_back(item);
        }
    }

    return 0;
}

int WebSocketServer::remove_from_list(subscribe_data data)
{
    while(mSetting)
    {
        printf ("WebSocketServer::remove_from_list() mSetting[%d]\n", mSetting);
        usleep(10000);// 10 ms
    }

      if (data.types.size() == 0)
    {
        // subscribe all
        //printf("WebSocketServer::remove_from_list() unsubscribe all!!\n");
        remove_list_item(&m_sos_List, data.sessionid);
        remove_list_item(&m_power_List, data.sessionid);
        remove_list_item(&m_powerAlert_List, data.sessionid);
        remove_list_item(&m_fenceAlert_List, data.sessionid);
        remove_list_item(&m_coord_List, data.sessionid);
        remove_list_item(&m_checkpointAlert_List, data.sessionid);
        remove_list_item(&m_event_List, data.sessionid);
        remove_list_item(&m_cardrollcallevent_List, data.sessionid);
        remove_list_item(&m_firefightingevent_List, data.sessionid);
        return 0;
    }
    for (std::list<std::string>::iterator type = data.types.begin(); type != data.types.end(); type++)
    {
        //printf("WebSocketServer::remove_from_list() (*type)[%s]\n", (*type).c_str());
        if ( (*type).compare("sos") == 0 )
        {
            remove_list_item(&m_sos_List, data.sessionid);
        }
        else
        if ( (*type).compare("power") == 0 )
        {
            remove_list_item(&m_power_List, data.sessionid);
        }
        else
        if ( (*type).compare("powerAlert") == 0 )
        {
            remove_list_item(&m_powerAlert_List, data.sessionid);
        }
        else
        if ( (*type).compare("fenceAlert") == 0 )
        {
            remove_list_item(&m_fenceAlert_List, data.sessionid);
        }
        else
        if ( (*type).compare("coord") == 0 )
        {
            remove_list_item(&m_coord_List, data.sessionid);
        }
        else
        if ( (*type).compare("checkpointAlert") == 0 )
        {
            remove_list_item(&m_checkpointAlert_List, data.sessionid);
        }
        else
        if ( (*type).compare("event") == 0 )
        {
            remove_list_item(&m_event_List, data.sessionid);
        }
        else
        if ( (*type).compare("cardrollcallevent") == 0 )
        {
            remove_list_item(&m_cardrollcallevent_List, data.sessionid);
        }
        else
        if ( (*type).compare("firefightingevent") == 0 )
        {
            remove_list_item(&m_firefightingevent_List, data.sessionid);
        }
    }
    return 0;
}

int WebSocketServer::remove_list_item(std::list<subscribe_item *> * targetList, int sessionid)
{
    //printf("WebSocketServer::remove_list_item() start sessionid[%d] targetList->size()[%lu] mSetting[%d]\n",
    //        sessionid, targetList->size(), mSetting);

    subscribe_item * targetItem = NULL;

    for (std::list<subscribe_item *>::iterator item = targetList->begin(); item != targetList->end(); item++)
    {
        //printf("WebSocketServer::remove_list_item() (*item)->sessionid[%d]\n", (*item)->sessionid);
        if ( (*item)->sessionid == sessionid )
        {
            targetItem = (*item);
            targetList->remove((*item));
            //printf("WebSocketServer::remove_list_item() targetList->remove((*item)) Done.\n");
            // if((*item) != NULL)
            // {
            //     vilslog::printf("CenterWebSocketService::remove_list_item() (*item) != NULL\n");
            //     try
            //     {
            //         delete (*item);
            //     }
            //     catch(exception e)
            //     {
            //         vilslog::printf("CenterWebSocketService::remove_list_item() catch(exception e)\n");
            //     }
            // }
            // vilslog::printf("CenterWebSocketService::remove_list_item() delete (*item) Done.\n");
            break;
        }
    }

    if ( targetItem != NULL )
    {
        delete targetItem;
        //printf("WebSocketServer::remove_list_item() delete targetItem Done.\n");
    }

    //printf("WebSocketServer::remove_list_item() end sessionid[%d] targetList->size()[%lu]\n", sessionid, targetList->size());
    return 0;
}


int WebSocketServer::do_send_sos(SubscribeDataItem * dataItem)
{
    // dataItem->m_tagId
    // dataItem->m_tagMac
    // dataItem->m_areaId
    // dataItem->m_areaName
    // dataItem->m_alertType
    // dataItem->m_timestamp

    // vilslog::printf("WebSocketServer::do_send_sos() sm_tagId[%s] m_sos_List->size()[%lu]\n",
    //     dataItem->m_tagId.c_str(), m_sos_List.size());

    int index = 0;
    for (std::list<subscribe_item *>::iterator item = m_sos_List.begin(); item != m_sos_List.end(); item++)
    {
        std::string targetID = "-1";

        if ( (*item)->subscribeType.compare("tag") == 0)
        {
            // targetID = dataItem->m_tagId;
            targetID = dataItem->m_tagMacAddress;
        }
        else
        if ( (*item)->subscribeType.compare("prj") == 0)
        {
            // project
            targetID = std::to_string(dataItem->m_projectID);
        }
        else
        {
            continue;
        }

        bool validSend = checkValidSendWebsocket("sos", targetID);
        if (!validSend)
        {
            //vilslog::printf ("WebSocketServer::do_send_sos() targetID[%s] not validSend!\n", targetID.c_str());
            continue;
        }

        for (std::list<std::string>::iterator itemid = (*item)->itemIDs.begin(); itemid != (*item)->itemIDs.end(); itemid++)
        {
            if ( targetID.compare((*itemid)) == 0 || (*itemid).compare("0") == 0)
            {
                //printf("WebSocketServer::do_send_sos() index[%d] Found! targetID[%s]\n", index, targetID.c_str());

                std::string json = convert_to_sos_json(dataItem);
                m_server.send((*item)->hdl, json, websocketpp::frame::opcode::text);

                //printf("WebSocketServer::do_send_sos() index[%d] Send finished!\n", index);
            }
        }
        index++;
    }
    return 0;
}

int WebSocketServer::do_send_power(SubscribeDataItem * dataItem)
{
    for (std::list<subscribe_item *>::iterator item = m_power_List.begin(); item != m_power_List.end(); item++)
    {
        std::string targetID = "-1";

        if ( (*item)->subscribeType.compare("tag") == 0)
        {
            // targetID = dataItem->m_tagId;
            targetID = dataItem->m_tagMacAddress;
        }
        else
        if ( (*item)->subscribeType.compare("prj") == 0)
        {
            // project
            targetID = std::to_string(dataItem->m_projectID);
        }
        else
        {
            continue;
        }

        bool validSend = checkValidSendWebsocket("power", targetID);
        if (!validSend)
        {
            vilslog::printf ("WebSocketServer::do_send_power() targetID[%s] not validSend!\n", targetID.c_str());
            continue;
        }

        for (std::list<std::string>::iterator itemid = (*item)->itemIDs.begin(); itemid != (*item)->itemIDs.end(); itemid++)
        {
            if ( targetID.compare((*itemid)) == 0 || (*itemid).compare("0") == 0)
            {
                std::string json = convert_to_power_json(dataItem);
                m_server.send((*item)->hdl, json, websocketpp::frame::opcode::text);
            }
        }
    }
    return 0;
}

int WebSocketServer::do_send_powerAlert(SubscribeDataItem * dataItem)
{
    for (std::list<subscribe_item *>::iterator item = m_powerAlert_List.begin(); item != m_powerAlert_List.end(); item++)
    {
        std::string targetID = "-1";

        if ( (*item)->subscribeType.compare("tag") == 0)
        {
            // targetID = dataItem->m_tagId;
            targetID = dataItem->m_tagMacAddress;
        }
        else
        if ( (*item)->subscribeType.compare("prj") == 0)
        {
            // project
            targetID = std::to_string(dataItem->m_projectID);
        }
        else
        {
            continue;
        }

        bool validSend = checkValidSendWebsocket("powerAlert", targetID);
        if (!validSend)
        {
            vilslog::printf ("WebSocketServer::do_send_powerAlert() targetID[%s] not validSend!\n", targetID.c_str());
            continue;
        }

        for (std::list<std::string>::iterator itemid = (*item)->itemIDs.begin(); itemid != (*item)->itemIDs.end(); itemid++)
        {
            if ( targetID.compare((*itemid)) == 0 || (*itemid).compare("0") == 0)
            {
                std::string json = convert_to_powerAlert_json(dataItem);
                m_server.send((*item)->hdl, json, websocketpp::frame::opcode::text);
            }
        }
    }
    return 0;
}

int WebSocketServer::do_send_fenceAlert(SubscribeDataItem * dataItem)
{
    for (std::list<subscribe_item *>::iterator item = m_fenceAlert_List.begin(); item != m_fenceAlert_List.end(); item++)
    {
        std::string targetID = "-1";

        if ( (*item)->subscribeType.compare("tag") == 0)
        {
            //targetID = dataItem->m_tagId;
            targetID = dataItem->m_tagMacAddress;
        }
        else
        if ( (*item)->subscribeType.compare("prj") == 0)
        {
            // project
            targetID = std::to_string(dataItem->m_projectID);
        }
        else
        {
            continue;
        }

        bool validSend = checkValidSendWebsocket("fenceAlert", targetID);
        if (!validSend)
        {
            vilslog::printf ("WebSocketServer::do_send_fenceAlert() targetID[%s] not validSend!\n", targetID.c_str());
            continue;
        }

        for (std::list<std::string>::iterator itemid = (*item)->itemIDs.begin(); itemid != (*item)->itemIDs.end(); itemid++)
        {
            if ( targetID.compare((*itemid)) == 0 || (*itemid).compare("0") == 0)
            {
                std::string json = convert_to_fenceAlert_json(dataItem);
                m_server.send((*item)->hdl, json, websocketpp::frame::opcode::text);
            }
        }
    }
    return 0;
}

int WebSocketServer::do_send_coord(SubscribeDataItem * dataItem)
{
    for (std::list<subscribe_item *>::iterator item = m_coord_List.begin(); item != m_coord_List.end(); item++)
    {
        std::string targetID = "-1";

        if ( (*item)->subscribeType.compare("tag") == 0)
        {
            //targetID = dataItem->m_tagId;
            targetID = dataItem->m_tagMacAddress;
        }
        else
        if ( (*item)->subscribeType.compare("prj") == 0)
        {
            // project
            targetID = std::to_string(dataItem->m_projectID);
        }
        else
        {
            continue;
        }

        bool validSend = checkValidSendWebsocket("coord", targetID);
        if (!validSend)
        {
            vilslog::printf ("WebSocketServer::do_send_coord() targetID[%s] not validSend!\n", targetID.c_str());
            continue;
        }

        for (std::list<std::string>::iterator itemid = (*item)->itemIDs.begin(); itemid != (*item)->itemIDs.end(); itemid++)
        {
            //printf("CenterWebSocketService::do_send_coord() targetID[%s] itemid[%s]\n", targetID.c_str(), (*itemid).c_str());

            if ( targetID.compare((*itemid)) == 0 || (*itemid).compare("0") == 0)
            {
                std::string json = convert_to_coord_json(dataItem);
                m_server.send((*item)->hdl, json, websocketpp::frame::opcode::text);
            }
        }
    }
    return 0;
}

int WebSocketServer::do_send_checkpointAlert(SubscribeDataItem * dataItem)
{
    for (std::list<subscribe_item *>::iterator item = m_checkpointAlert_List.begin(); item != m_checkpointAlert_List.end(); item++)
    {
        std::string projectID = "-1";

        if ( (*item)->subscribeType.compare("prj") == 0)
        {
            // project
            projectID = std::to_string(dataItem->m_projectID);
        }
        else
        {
            continue;
        }

        bool validSend = checkValidSendWebsocket("checkpointAlert", projectID);
        if (!validSend)
        {
            vilslog::printf ("WebSocketServer::do_send_checkpointAlert() projectID[%s] not validSend!\n", projectID.c_str());
            continue;
        }

        for (std::list<std::string>::iterator itemid = (*item)->itemIDs.begin(); itemid != (*item)->itemIDs.end(); itemid++)
        {
            //printf("WebSocketServer::do_send_checkpointAlert() projectID[%s] (*itemid)[%s]\n", projectID.c_str(), (*itemid).c_str());

            if ( projectID.compare((*itemid)) == 0 || (*itemid).compare("0") == 0)
            {
                std::string json = convert_to_checkpointAlert_json(dataItem);
                m_server.send((*item)->hdl, json, websocketpp::frame::opcode::text);
            }
        }
    }
    return 0;
}

int WebSocketServer::do_send_event(SubscribeDataItem * dataItem)
{
    for (std::list<subscribe_item *>::iterator item = m_event_List.begin(); item != m_event_List.end(); item++)
    {
        std::string projectID = "-1";

        if ( (*item)->subscribeType.compare("prj") == 0)
        {
            // project
            projectID = std::to_string(dataItem->m_projectID);
        }
        else
        {
            continue;
        }

        // bool validSend = checkValidSendWebsocket("event", projectID);
        // if (!validSend)
        // {
        //     vilslog::printf ("WebSocketServer::do_send_event() projectID[%s] not validSend!\n", projectID.c_str());
        //     continue;
        // }

        for (std::list<std::string>::iterator itemid = (*item)->itemIDs.begin(); itemid != (*item)->itemIDs.end(); itemid++)
        {
//            vilslog::printf("WebSocketServer::do_send_event() projectID[%s] (*itemid)[%s]\n", projectID.c_str(), (*itemid).c_str());

            if ( projectID.compare((*itemid)) == 0 || (*itemid).compare("0") == 0)
            {
                std::string json = convert_to_event_json(dataItem);
                m_server.send((*item)->hdl, json, websocketpp::frame::opcode::text);
            }
        }
    }
    return 0;
}

int WebSocketServer::do_send_callout(SubscribeDataItem * dataItem)
{
    for (std::list<subscribe_item *>::iterator item = m_event_List.begin(); item != m_event_List.end(); item++)
    {
        std::string projectID = "-1";

        if ( (*item)->subscribeType.compare("prj") == 0)
        {
            // project
            projectID = std::to_string(dataItem->m_projectID);
        }
        else
        {
            continue;
        }

        for (std::list<std::string>::iterator itemid = (*item)->itemIDs.begin(); itemid != (*item)->itemIDs.end(); itemid++)
        {
            if ( projectID.compare((*itemid)) == 0 || (*itemid).compare("0") == 0)
            {
                std::string json = convert_to_callout_json(dataItem);
                m_server.send((*item)->hdl, json, websocketpp::frame::opcode::text);
            }
        }
    }
    return 0;
}

int WebSocketServer::do_send_cardrollcallevent(SubscribeDataItem * dataItem)
{
    for (std::list<subscribe_item *>::iterator item = m_cardrollcallevent_List.begin(); item != m_cardrollcallevent_List.end(); item++)
    {
        std::string projectID = "-1";

        if ( (*item)->subscribeType.compare("prj") == 0)
        {
            // project
            projectID = std::to_string(dataItem->m_projectID);
        }
        else
        {
            continue;
        }

        for (std::list<std::string>::iterator itemid = (*item)->itemIDs.begin(); itemid != (*item)->itemIDs.end(); itemid++)
        {
            if ( projectID.compare((*itemid)) == 0 || (*itemid).compare("0") == 0)
            {
                std::string json = convert_to_cardrollcallevent_json(dataItem);
                m_server.send((*item)->hdl, json, websocketpp::frame::opcode::text);
            }
        }
    }
    return 0;
}

int WebSocketServer::do_send_firefightingevent(SubscribeDataItem * dataItem)
{
    // vilslog::printf("WebSocketServer::do_send_firefightingevent() m_firefightingevent_List.size()[%d]\n", (int)m_firefightingevent_List.size());

    for (std::list<subscribe_item *>::iterator item = m_firefightingevent_List.begin(); item != m_firefightingevent_List.end(); item++)
    {
        // std::string projectID = "-1";

        if ( (*item)->subscribeType.compare("maingroups") == 0)
        {
            // project
            // projectID = std::to_string(dataItem->m_projectID);
            std::string m_maingroupid = std::to_string(dataItem->m_maingroupid);

            // vilslog::printf("WebSocketServer::do_send_firefightingevent() m_FireFightingType[%s]\n", dataItem->m_FireFightingType.c_str());
            // vilslog::printf("WebSocketServer::do_send_firefightingevent() m_FireFightingInfo.maingroupid[%d]\n", dataItem->m_FireFightingInfo.maingroupid);
            // vilslog::printf("WebSocketServer::do_send_firefightingevent() (*item)->itemIDs.size()[%d]\n", (int)(*item)->itemIDs.size());

            for (std::list<std::string>::iterator itemid = (*item)->itemIDs.begin(); itemid != (*item)->itemIDs.end(); itemid++)
            {
                // vilslog::printf("WebSocketServer::do_send_firefightingevent() maingroupid[%s]\n", (*itemid).c_str());

                if ( m_maingroupid.compare((*itemid)) == 0 || (*itemid).compare("0") == 0)
                {
                    if (dataItem->m_FireFightingType.compare("FireFightingInfo") == 0)
                    {
                        std::string json = convert_to_FireFightingInfo_json(dataItem);
                        m_server.send((*item)->hdl, json, websocketpp::frame::opcode::text);
                    }
                    else
                    if (dataItem->m_FireFightingType.compare("FireFightingUser") == 0)
                    {
                        std::string json = convert_to_FireFightingUser_json(dataItem);
                        m_server.send((*item)->hdl, json, websocketpp::frame::opcode::text);
                    }
                }
            }
        }
    }
    return 0;
}

std::string WebSocketServer::convert_to_sos_json(SubscribeDataItem * dataItem)
{
    char sendString[512];
    memset(sendString, 0, 512);
    sprintf(sendString, "{\"msgType\":\"%s\",\"anchor\":\"%s\",\"mapId\":%d,\"type\":%d,\"id\":%d,\"mac\":\"%s\",\"code\":\"%s\",\"tagid\":\"%s\",\"tagname\":\"%s\",\"locatorid\":\"%s\",\"locatorname\":\"%s\",\"areaid\":\"%s\",\"areaname\":\"%s\",\"time\":\"%s\"}",
            "sos", dataItem->m_locatorId.c_str(), dataItem->m_projectID, dataItem->m_alertType,
            dataItem->m_tagIndex, dataItem->m_tagMacAddress.c_str(),
            dataItem->m_tagId.c_str(), dataItem->m_tagId.c_str(), dataItem->m_tagName.c_str(),
            dataItem->m_locatorId.c_str(), dataItem->m_locatorName.c_str(),
            dataItem->m_areaId.c_str(), dataItem->m_areaName.c_str(), dataItem->m_timestamp.c_str());

    // sprintf(sendString, "{\"msgType\":\"%s\",\"prjid\":%d,\"id\":\"%s\",\"name\":\"%s\",\"locatorid\":\"%s\",\"locatorname\":\"%s\",\"type\":%d,\"areaid\":\"%s\",\"areaname\":\"%s\",\"time\":\"%s\"}",
    //         "sos", dataItem->m_projectID, dataItem->m_tagId.c_str(), dataItem->m_tagName.c_str(),
    //         dataItem->m_locatorId.c_str(), dataItem->m_locatorName.c_str(),
    //         dataItem->m_alertType, dataItem->m_areaId.c_str(), dataItem->m_areaName.c_str(), dataItem->m_timestamp.c_str());

    // sprintf(sendString, "{\"msgType\":\"%s\",\"prjid\":%d,\"id\":\"%s\",\"mac\":\"%s\",\"type\":%d,\"areaid\":\"%s\",\"areaname\":\"%s\",\"time\":\"%s\"}",
    //         "sos", dataItem->m_projectID, dataItem->m_tagId.c_str(), dataItem->m_tagMac.c_str(),
    //         dataItem->m_alertType, dataItem->m_areaId.c_str(), dataItem->m_areaName.c_str(), dataItem->m_timestamp.c_str());

    std::string json = sendString;
    return json;
}

std::string WebSocketServer::convert_to_power_json(SubscribeDataItem * dataItem)
{
    char sendString[512];
    memset(sendString, 0, 512);

    float fpower = atof(dataItem->m_voltage.c_str());
    int power = (int)(fpower * 1000.0f);

    sprintf(sendString, "{\"msgType\":\"%s\",\"anchor\":\"%s\",\"mapId\":%d,\"id\":%d,\"mac\":\"%s\",\"map\":\"xystudy_%d\",\"power\":%d,\"time\":\"%s\"}",
            "power", dataItem->m_locatorId.c_str(), dataItem->m_projectID,
            dataItem->m_tagIndex, dataItem->m_tagMacAddress.c_str(),
            dataItem->m_projectID, power, dataItem->m_timestamp.c_str());

    // sprintf(sendString, "{\"msgType\":\"%s\",\"prjid\":%d,\"id\":\"%s\",\"name\":\"%s\",\"voltage\":\"%s\",\"temperature\":\"%s\",\"time\":\"%s\"}",
    //         "power", dataItem->m_projectID, dataItem->m_tagId.c_str(), dataItem->m_tagName.c_str(),
    //         dataItem->m_voltage.c_str(), dataItem->m_temperature.c_str(), dataItem->m_timestamp.c_str());

    std::string json = sendString;
    return json;
}

std::string WebSocketServer::convert_to_powerAlert_json(SubscribeDataItem * dataItem)
{
    char sendString[512];
    memset(sendString, 0, 512);

    float fpower = atof(dataItem->m_voltage.c_str());
    int power = (int)(fpower * 1000.0f);

    sprintf(sendString, "{\"msgType\":\"%s\",\"anchor\":\"%s\",\"mapId\":%d,\"id\":%d,\"mac\":\"%s\",\"map\":\"xystudy_%d\",\"power\":%d,\"time\":\"%s\"}",
            "powerAlert", dataItem->m_locatorId.c_str(), dataItem->m_projectID,
            dataItem->m_tagIndex, dataItem->m_tagMacAddress.c_str(),
            dataItem->m_projectID, power, dataItem->m_timestamp.c_str());


    // sprintf(sendString, "{\"msgType\":\"%s\",\"prjid\":%d,\"id\":\"%s\",\"name\":\"%s\",\"voltage\":\"%s\",\"temperature\":\"%s\",\"time\":\"%s\"}",
    //         "powerAlert", dataItem->m_projectID, dataItem->m_tagId.c_str(), dataItem->m_tagName.c_str(),
    //         dataItem->m_voltage.c_str(), dataItem->m_temperature.c_str(), dataItem->m_timestamp.c_str());

    std::string json = sendString;
    return json;
}

std::string WebSocketServer::convert_to_fenceAlert_json(SubscribeDataItem * dataItem)
{
    char sendString[512];
    memset(sendString, 0, 512);

    std::string isIn = "false";
    if (dataItem->m_alertAction == 1)
    {
        isIn = "true";
    }

    sprintf(sendString, "{\"msgType\":\"%s\",\"mapId\":%d,\"mac\":\"%s\",\"code\":\"%s\",\"fenceId\":\"%s\",\"fenceCname\":\"%s\",\"tagSourceId\":%d,\"isIn\":%s,\"tagid\":\"%s\",\"tagname\":\"%s\",\"type\":%d,\"action\":%d,\"areaid\":\"%s\",\"areaname\":\"%s\",\"time\":\"%s\"}",
            "fenceAlert", dataItem->m_projectID, dataItem->m_tagMacAddress.c_str(),
            dataItem->m_tagId.c_str(), dataItem->m_areaId.c_str(), dataItem->m_areaName.c_str(),
            dataItem->m_tagIndex, isIn.c_str(), dataItem->m_tagId.c_str(), dataItem->m_tagName.c_str(),
            dataItem->m_alertType, dataItem->m_alertAction, dataItem->m_areaId.c_str(),
            dataItem->m_areaName.c_str(), dataItem->m_timestamp.c_str());

    // sprintf(sendString, "{\"msgType\":\"%s\",\"prjid\":%d,\"id\":\"%s\",\"name\":\"%s\",\"type\":%d,\"action\":%d,\"areaid\":\"%s\",\"areaname\":\"%s\",\"time\":\"%s\"}",
    //         "fenceAlert", dataItem->m_projectID, dataItem->m_tagId.c_str(), dataItem->m_tagName.c_str(),
    //         dataItem->m_alertType, dataItem->m_alertAction, dataItem->m_areaId.c_str(), dataItem->m_areaName.c_str(), dataItem->m_timestamp.c_str());

    std::string json = sendString;
    return json;
}

std::string WebSocketServer::convert_to_coord_json(SubscribeDataItem * dataItem)
{
    char sendString[512];
    memset(sendString, 0, 512);

    float fpower = atof(dataItem->m_voltage.c_str());
    float base_v = 4.2f - 3.6f;
    float power_v = fpower - 3.6f;

    int power = (int)((power_v/base_v) * 100.0f);
    if (power > 100)
        power = 100;
    if (power < 0)
        power = 0;

    sprintf(sendString, "{\"msgType\":\"%s\",\"id\":%d,\"mac\":\"%s\",\"code\":\"%s\",\"mapId\":%d,\"power\":%d,\"x\":%d,\"y\":%d,\"z\":%d,\"time\":\"%s\"}",
        "coord", dataItem->m_tagIndex, dataItem->m_tagMacAddress.c_str(),
        dataItem->m_tagId.c_str(), dataItem->m_projectID, power,
        dataItem->m_posX, dataItem->m_posY, dataItem->m_posZ, dataItem->m_timestamp.c_str());


    // sprintf(sendString, "{\"msgType\":\"%s\",\"prjid\":%d,\"id\":\"%s\",\"name\":\"%s\",\"x\":%d,\"y\":%d,\"z\":%d,\"voltage\":\"%s\",\"temperature\":\"%s\",\"time\":\"%s\"}",
    //         "coord", dataItem->m_projectID, dataItem->m_tagId.c_str(), dataItem->m_tagName.c_str(),
    //         dataItem->m_posX, dataItem->m_posY, dataItem->m_posZ, dataItem->m_voltage.c_str(), dataItem->m_temperature.c_str(), dataItem->m_timestamp.c_str());

    std::string json = sendString;
    return json;
}

std::string WebSocketServer::convert_to_checkpointAlert_json(SubscribeDataItem * dataItem)
{
    char sendString[512];
    memset(sendString, 0, 512);
    sprintf(sendString, "{\"msgType\":\"%s\",\"prjid\":%d,\"anchorId\":\"%s\",\"chcekPointName\":\"%s\",\"alertType\":%d,\"time\":\"%s\"}",
            "checkpointAlert", dataItem->m_projectID, dataItem->m_anchorId.c_str(), dataItem->m_chcekPointName.c_str(),
            dataItem->m_alertType, dataItem->m_timestamp.c_str());

    std::string json = sendString;

    //printf("WebSocketServer::convert_to_checkpointAlert_json() json[%s]\n", json.c_str());

    return json;
}

std::string WebSocketServer::convert_to_event_json(SubscribeDataItem * dataItem)
{
    char sendString[512];
    memset(sendString, 0, 512);
    sprintf(sendString, "{\"msgType\":\"%s\",\"prjid\":%d,\"alertType\":%d,\"alertAction\":%d,\"time\":\"%s\",\"nodeid\":\"%s\"}",
            "event", dataItem->m_projectID, dataItem->m_alertType, dataItem->m_alertAction, dataItem->m_timestamp.c_str(), dataItem->m_nodeId.c_str());

    std::string json = sendString;

    //printf("WebSocketServer::convert_to_event_json() json[%s]\n", json.c_str());

    return json;
}

std::string WebSocketServer::convert_to_cardrollcallevent_json(SubscribeDataItem * dataItem)
{
    char sendString[512];
    memset(sendString, 0, 512);
    sprintf(sendString, "{\"msgType\":\"%s\",\"prjid\":%d,\"readerid\":%d,\"cardtype\":%d,\"cardid\":\"%s\"}",
            "cardrollcallevent", dataItem->m_projectID, dataItem->m_readerid, dataItem->m_cardtype, dataItem->m_cardid.c_str());

    std::string json = sendString;

    //printf("WebSocketServer::convert_to_event_json() json[%s]\n", json.c_str());

    return json;
}

std::string WebSocketServer::convert_to_FireFightingInfo_json(SubscribeDataItem * dataItem)
{
    std::ostringstream sendstream;

    std::string StandbyDeviceIds = convert_list_to_string(dataItem->m_FireFightingInfo.StandbyDeviceIds);
    std::string NFCDeviceIds = convert_listint_to_string(dataItem->m_FireFightingInfo.NFCDeviceIdList);

    sendstream << "{\"msgType\":\"FireFightingInfo\"";
    sendstream << ",\"id\":"<< dataItem->m_FireFightingInfo.id;
    sendstream << ",\"maingroupid\":"<< dataItem->m_FireFightingInfo.maingroupid;
    sendstream << ",\"caseclosed\":"<< dataItem->m_FireFightingInfo.caseclosed;
    sendstream << ",\"casenumber\":\""<< dataItem->m_FireFightingInfo.casenumber<<"\"";
    sendstream << ",\"address\":\""<< dataItem->m_FireFightingInfo.address<<"\"";
    sendstream << ",\"lat\":\""<< dataItem->m_FireFightingInfo.lat<<"\"";
    sendstream << ",\"lng\":\""<< dataItem->m_FireFightingInfo.lng<<"\"";
    sendstream << ",\"starttime\":\""<< dataItem->m_FireFightingInfo.starttime<<"\"";
    sendstream << ",\"closetime\":\""<< dataItem->m_FireFightingInfo.closetime<<"\"";
    sendstream << ",\"StandbyDeviceIds\":\""<< StandbyDeviceIds<<"\"";
    sendstream << ",\"NFCDeviceIds\":\""<< NFCDeviceIds<<"\"";
    sendstream << "}" << endl;

    std::string json = sendstream.str();

    // printf("WebSocketServer::convert_to_firefightingevent_json() json[%s]\n", json.c_str());

    return json;
}

std::string WebSocketServer::convert_to_FireFightingUser_json(SubscribeDataItem * dataItem)
{
    std::ostringstream sendstream;

    std::string enterTime = time_tToString(dataItem->m_FireFightingUser.enterTime);

    sendstream << "{\"msgType\":\"FireFightingUser\"";
    sendstream << ",\"userid\":"<< dataItem->m_FireFightingUser.userid;
    sendstream << ",\"maingroupid\":"<< dataItem->m_FireFightingUser.maingroupid;
    sendstream << ",\"areaType\":"<< dataItem->m_FireFightingUser.areaType;
    sendstream << ",\"airremain\":\""<< dataItem->m_FireFightingUser.airremain<<"\"";
    sendstream << ",\"enterTime\":\""<< enterTime<<"\"";
    sendstream << ",\"casenumber\":\""<< dataItem->m_FireFightingUser.casenumber<<"\"";
    sendstream << ",\"activity\":\""<< dataItem->m_FireFightingUser.activity<<"\"";
    sendstream << ",\"HR\":\""<< dataItem->m_FireFightingUser.HR<<"\"";
    sendstream << ",\"SPO2\":\""<< dataItem->m_FireFightingUser.SPO2<<"\"";
    sendstream << ",\"battery\":\""<< dataItem->m_FireFightingUser.battery<<"\"";
    sendstream << "}" << endl;

    std::string json = sendstream.str();
    return json;
}

std::string WebSocketServer::convert_to_callout_json(SubscribeDataItem * dataItem)
{
    char sendString[512];
    memset(sendString, 0, 512);
    sprintf(sendString, "{\"msgType\":\"%s\",\"prjid\":%d,\"alertType\":%d,\"alertAction\":%d,\"time\":\"%s\",\"nodeid\":\"%s\",\"nodename\":\"%s\"}",
            "callout", dataItem->m_projectID, dataItem->m_alertType, dataItem->m_alertAction, dataItem->m_timestamp.c_str(), dataItem->m_nodeId.c_str(), dataItem->m_nodename.c_str());

    std::string json = sendString;

    //printf("WebSocketServer::convert_to_event_json() json[%s]\n", json.c_str());

    return json;
}

subscribe_item * WebSocketServer::create_subscribe_item(connection_hdl hdl, subscribe_data data)
{
    subscribe_item * item = new subscribe_item();
    item->sessionid = data.sessionid;
    item->hdl = hdl;
    item->subscribeType = data.subscribeType;
    item->itemIDs = data.itemIDs; // copy list items
    return item;
}

bool WebSocketServer::checkValidSendWebsocket(std::string type, std::string targetid)
{
    bool validSend = true;

    bool bfound = false;
    for (std::list<WebsocketSendTimeStruct *>::iterator preTime = mWebsocketSendTimeList.begin(); preTime != mWebsocketSendTimeList.end(); preTime++)
    {
        if ( ((*preTime)->type.compare(type) == 0) && ((*preTime)->targetid.compare(targetid) == 0) )
        {
            time_t currentTimestamp = getCurrentTimeInMilliSec();
            time_t diffTime = currentTimestamp - (*preTime)->issuetime;

            int valid_send_time_out = WEBSOCKET_VALID_SEND_TIME_OUT;

            if ( ((int)diffTime <= valid_send_time_out) && ((int)diffTime > 0) )
            {
                validSend = false;
            }
            else
            {
                (*preTime)->issuetime = currentTimestamp;
            }

            bfound = true;
            break;
        }

    }

    if (!bfound)
    {
        WebsocketSendTimeStruct * preTime = new WebsocketSendTimeStruct();
        preTime->type = type;
        preTime->targetid = targetid;
        preTime->issuetime = getCurrentTimeInMilliSec();
        mWebsocketSendTimeList.push_back(preTime);

        vilslog::printf ("WebSocketServer::checkValidSendWebsocket() type[%s] targetid[%s] new WebsocketSendTimeStruct()\n",
            type.c_str(), targetid.c_str());
    }

    return validSend;
}
