/*
 *  WebSocketClient.h
 *
 *  Created on: 2020/08/21
 *      Author: ikki
 */
#ifndef __WebSocketClient_h__
#define __WebSocketClient_h__

#include "../ServiceThreads/Thread.h"
#include "../network/DataItem.h"
#include "../config.h"

#include <iostream>
#include <map>
#include <exception>
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/client.hpp>

typedef websocketpp::client<websocketpp::config::asio> client;

// pull out the type of messages sent by our config
typedef websocketpp::config::asio::message_type::ptr message_ptr;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;


class WebSocketClient : public Thread
{
public:
    WebSocketClient(std::string vilsuid, wqueue<DataItem*> & GAEJobQueue);

    bool mInitial;
    std::string mStatus;

    void * run();

    void on_open(websocketpp::connection_hdl hdl);
    void on_fail(websocketpp::connection_hdl hdl);
    void on_message(websocketpp::connection_hdl hdl, message_ptr msg);
    void on_close(websocketpp::connection_hdl hdl);

private:

    client m_client;

    std::string m_VILSUID;
    wqueue<DataItem*> & mGAEJobQueue;

    void connect_to_server(std::string uri);
    void parse_message(std::string msg);
    void parse_SurveyResult(Json::Value & JSONresult);
    void parse_NotifyMessageStatus(Json::Value & JSONresult);

};

#endif
