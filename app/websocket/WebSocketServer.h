/*
 *  WebSocketServer.h
 *
 *  Created on: 2018/09/26
 *      Author: ikki
 */
#ifndef __WebSocketServer_h__
#define __WebSocketServer_h__

#include "../ServiceThreads/Thread.h"
#include "../config.h"
#include "SubscribeDataItem.h"

#include <iostream>
#include <map>
#include <exception>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>

#define WEBSOCKET_VALID_SEND_TIME_OUT 1000

typedef websocketpp::server<websocketpp::config::asio> server;

using websocketpp::connection_hdl;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;


struct subscribe_data
{
    int sessionid;
    std::string msg;
    std::string uri;

    std::string subscribeType; // tag, prj
    std::list<std::string> itemIDs; // tagid1, tagid2
    std::list<std::string> types; // sos, power ,powerAlert, fenceAlert, coord
};

// sos, power ,powerAlert, fenceAlert, coord
struct subscribe_item
{
    int sessionid;
    websocketpp::connection_hdl hdl;
    std::string subscribeType; // tag, prj
    std::list<std::string> itemIDs; // tagid1, tagid2
};

typedef struct
{
    std::string type;
    std::string targetid;
    time_t issuetime;
} WebsocketSendTimeStruct;

//===============
// -std=c++11
typedef std::map<connection_hdl, subscribe_data, std::owner_less<connection_hdl> > connection_hdl_list;
//typedef std::map< connection_hdl, connection_data, std::owner_less<std::shared_ptr<connection_hdl> > > connection_hdl_list;
//typedef std::map< connection_hdl, connection_data, std::owner_less<std::weak_ptr<connection_hdl> > > connection_hdl_list;
//===============
// none c++11
//typedef std::map<connection_hdl, subscribe_data> connection_hdl_list;
//===============

typedef std::map<connection_hdl, subscribe_data>::iterator it_type;

class WebSocketServer : public Thread
{
public:
    WebSocketServer(int port);

    bool mInitial;
    std::string mStatus;

    void * run();
    void on_open(connection_hdl hdl);
    void on_close(connection_hdl hdl);
    void on_message(connection_hdl hdl, server::message_ptr msg);

    int setDataItem(SubscribeDataItem * dataItem);

private:

    bool mHandlering;
    bool mSetting;

    std::list<WebsocketSendTimeStruct*> mWebsocketSendTimeList;

    subscribe_data& get_data_from_hdl(connection_hdl hdl);
    bool parse_subscribe_data(std::string uri, subscribe_data & data);
    int parse_pattern_id(std::string comma_str, std::string pattern, std::list<std::string> & itemIDs);

    int do_send_sos(SubscribeDataItem * dataItem);
    int do_send_power(SubscribeDataItem * dataItem);
    int do_send_powerAlert(SubscribeDataItem * dataItem);
    int do_send_fenceAlert(SubscribeDataItem * dataItem);
    int do_send_coord(SubscribeDataItem * dataItem);
    int do_send_checkpointAlert(SubscribeDataItem * dataItem);
    int do_send_event(SubscribeDataItem * dataItem);
    int do_send_callout(SubscribeDataItem * dataItem);
    int do_send_cardrollcallevent(SubscribeDataItem * dataItem);
    int do_send_firefightingevent(SubscribeDataItem * dataItem);

    std::string convert_to_sos_json(SubscribeDataItem * dataItem);
    std::string convert_to_power_json(SubscribeDataItem * dataItem);
    std::string convert_to_powerAlert_json(SubscribeDataItem * dataItem);
    std::string convert_to_fenceAlert_json(SubscribeDataItem * dataItem);
    std::string convert_to_coord_json(SubscribeDataItem * dataItem);
    std::string convert_to_checkpointAlert_json(SubscribeDataItem * dataItem);
    std::string convert_to_event_json(SubscribeDataItem * dataItem);
    std::string convert_to_callout_json(SubscribeDataItem * dataItem);
    std::string convert_to_cardrollcallevent_json(SubscribeDataItem * dataItem);
    std::string convert_to_FireFightingInfo_json(SubscribeDataItem * dataItem);
    std::string convert_to_FireFightingUser_json(SubscribeDataItem * dataItem);

    int m_next_sessionid;
    server m_server;
    int m_server_port;
    connection_hdl_list m_connections;

    int put_to_list(connection_hdl hdl, subscribe_data data);
    int remove_from_list(subscribe_data data);
    int remove_list_item(std::list<subscribe_item *> * targetList, int sessionid);
    std::list<subscribe_item *> m_sos_List;
    std::list<subscribe_item *> m_power_List;
    std::list<subscribe_item *> m_powerAlert_List;
    std::list<subscribe_item *> m_fenceAlert_List;
    std::list<subscribe_item *> m_coord_List;
    std::list<subscribe_item *> m_checkpointAlert_List;
    std::list<subscribe_item *> m_event_List;
    std::list<subscribe_item *> m_cardrollcallevent_List;
    std::list<subscribe_item *> m_firefightingevent_List;

    subscribe_item * create_subscribe_item(connection_hdl hdl, subscribe_data data);
    bool checkValidSendWebsocket(std::string type, std::string targetid);
};

#endif
