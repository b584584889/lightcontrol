/*
 *  WebSocketClient.cpp
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
#include "WebSocketClient.h"

WebSocketClient::WebSocketClient(std::string vilsuid, wqueue<DataItem*> & GAEJobQueue)
    : m_VILSUID(vilsuid), mGAEJobQueue(GAEJobQueue)
{
}

void * WebSocketClient::run()
{
    mInitial = false;

    char url[4096];
    int bufferLen = 4096;
    memset(url, 0, bufferLen);

    sprintf(url, "ws://%s:%s/vilsapi/endpoint?vilsuid=%s", PSUH_NOTIFY_SERVER, PSUH_NOTIFY_SERVER_PORT, m_VILSUID.c_str());
    std::string uri = url;

    vilslog::printf("WebSocketClient::run() uri[%s] \n", uri.c_str());

    // set logging policy if needed
    m_client.clear_access_channels(websocketpp::log::alevel::frame_header);
    m_client.clear_access_channels(websocketpp::log::alevel::frame_payload);
    //c.set_error_channels(websocketpp::log::elevel::none);

    vilslog::printf("WebSocketClient::run() 1\n\n");

    // Initialize ASIO
    m_client.init_asio();

    // Register our handlers
    m_client.set_open_handler(bind(&WebSocketClient::on_open,this,::_1));
    m_client.set_fail_handler(bind(&WebSocketClient::on_fail,this,::_1));
    m_client.set_message_handler(bind(&WebSocketClient::on_message,this,::_1,::_2));
    m_client.set_close_handler(bind(&WebSocketClient::on_close,this,::_1));

    while(1)
    {
        connect_to_server(uri);

        vilslog::printf("WebSocketClient::run() disconnect from uri[%s]\n\n", uri.c_str());

        sleep(10);

        vilslog::printf("WebSocketClient::run() reconnect to uri[%s]\n\n", uri.c_str());

    }

    return NULL;
}

void WebSocketClient::connect_to_server(std::string uri)
{
    try {
        // Create a connection to the given URI and queue it for connection once
        // the event loop starts
        websocketpp::lib::error_code ec;
        client::connection_ptr con = m_client.get_connection(uri, ec);
        m_client.connect(con);

        vilslog::printf("WebSocketClient::connect_to_server() m_client.connect()\n");

        mInitial = true;
        mStatus = "";

        vilslog::printf("WebSocketClient::connect_to_server() m_client.run() ....... \n");

        // Start the ASIO io_service run loop
        m_client.run();

        vilslog::printf("WebSocketClient::connect_to_server() m_client.run() Finished! \n\n");

    }
    catch (const std::exception & exc)
    {
        vilslog::printf("WebSocketClient::connect_to_server() e.what:");
        vilslog::printf(exc.what());
        vilslog::printf("\n");
    }
    catch (websocketpp::lib::error_code e)
    {
        //std::cout << e.message() << std::endl;
        vilslog::printf("WebSocketClient::connect_to_server() e.message[%s]\n", e.message().c_str());
    }
    catch (...)
    {
        vilslog::printf("WebSocketClient::connect_to_server() other exception\n");
    }
}

void WebSocketClient::parse_message(std::string msg)
{
    Json::Reader * reader = new Json::Reader();
    Json::Value JSONresult;
    bool parsingSuccessful = false;

    try{
        parsingSuccessful = reader->parse( msg, JSONresult);

        if (parsingSuccessful)
        {
            if ( JSONresult.isMember("type") )
            {
                std::string type = JSONresult["type"].asCString();

                // vilslog::printf ("WebSocketClient::parse_message() type[%s]\n", JSONresult["type"].asCString());

                if (type.compare("SurveyResult") == 0)
                {
                    parse_SurveyResult(JSONresult);
                }
                else
                if (type.compare("NotifyMessageStatus") == 0)
                {
                    parse_NotifyMessageStatus(JSONresult);
                }
                else
                {

                }
            }
        }
        else
        {
            vilslog::printf ("WebSocketClient::parse_message() parsingSuccessful[%d]\n", parsingSuccessful);
        }
    }
    catch(exception exc)
    {
        vilslog::printf ("WebSocketClient::parse_message() reader->parse() catch exception[%s]\n", exc.what());
    }

    delete reader;
}

void WebSocketClient::parse_SurveyResult(Json::Value & result)
{
    GAESurveyResultData * surveyData = new GAESurveyResultData();

    if ( result.isMember("topic") )
    {
        // vilslog::printf ("WebSocketClient::parse_SurveyResult() topic[%s]\n", result["topic"].asCString());
        surveyData->topic = result["topic"].asCString();
    }

    if ( result.isMember("messagehashkey") )
    {
        // vilslog::printf ("WebSocketClient::parse_SurveyResult() messagehashkey[%s]\n", result["messagehashkey"].asCString());
        surveyData->messagehashkey = result["messagehashkey"].asCString();
    }

    if ( result.isMember("hash") )
    {
        // vilslog::printf ("WebSocketClient::parse_SurveyResult() hash[%s]\n", result["hash"].asCString());
        surveyData->hashkey = result["hash"].asCString();
    }

    if ( result.isMember("token") )
    {
        // vilslog::printf ("WebSocketClient::parse_SurveyResult() hash[%s]\n", result["hash"].asCString());
        surveyData->token = result["token"].asCString();
    }

    if ( result.isMember("comment") )
    {
        // vilslog::printf ("WebSocketClient::parse_SurveyResult() comment[%s]\n", result["comment"].asCString());
        surveyData->comment = result["comment"].asCString();
    }

    if ( result.isMember("results") )
    {
        // vilslog::printf ("WebSocketClient::parse_SurveyResult() result[results].isArray()[%d]\n", result["results"].isArray());
        if ( result["results"].isArray() )
        {
            for(int i=0;i<(int)result["results"].size();i++)
            {
                Json::Value resultvalue = result["results"][i];
                // vilslog::printf ("WebSocketClient::parse_SurveyResult() results[%d][%s]\n", i, resultvalue.asCString());
                surveyData->results.push_back(resultvalue.asString());
            }
        }
    }

    if ( result.isMember("datetime") )
    {
        // vilslog::printf ("WebSocketClient::parse_SurveyResult() datetime[%s]\n\n", result["datetime"].asCString());
        surveyData->datetime = result["datetime"].asCString();
    }

    DataItem * item = new DataItem("GAESurveyResultData", 1, surveyData);
    mGAEJobQueue.add(item);

    surveyData->results.clear();
    delete surveyData;
}

void WebSocketClient::parse_NotifyMessageStatus(Json::Value & result)
{
    GAEMessageStatusData * msgStatus = new GAEMessageStatusData();
    msgStatus->messageid = "";
    msgStatus->topic = "";
    msgStatus->messagehashkey = "";
    msgStatus->token = "";
    msgStatus->read = "0";
    msgStatus->finished = "0";
    msgStatus->datetime = "";

    if ( result.isMember("vilsuid") )
    {
        // vilslog::printf ("WebSocketClient::parse_NotifyMessageStatus() vilsuid[%s]\n", result["vilsuid"].asCString());
        msgStatus->messageid = result["messageid"].asCString();
    }

    if ( result.isMember("messageid") )
    {
        // vilslog::printf ("WebSocketClient::parse_NotifyMessageStatus() messageid[%s]\n", result["messageid"].asCString());
        msgStatus->messageid = result["messageid"].asCString();
    }

    if ( result.isMember("messagehashkey") )
    {
        // vilslog::printf ("WebSocketClient::parse_NotifyMessageStatus() messagehashkey[%s]\n", result["messagehashkey"].asCString());
        msgStatus->messagehashkey = result["messagehashkey"].asCString();
    }

    if ( result.isMember("topic") )
    {
        // vilslog::printf ("WebSocketClient::parse_NotifyMessageStatus() topic[%s]\n", result["topic"].asCString());
        msgStatus->topic = result["topic"].asCString();
    }

    if ( result.isMember("token") )
    {
        // vilslog::printf ("WebSocketClient::parse_NotifyMessageStatus() token[%s]\n", result["token"].asCString());
        msgStatus->token = result["token"].asCString();
    }

    if ( result.isMember("read") )
    {
        // vilslog::printf ("WebSocketClient::parse_NotifyMessageStatus() read[%s]\n", result["read"].asCString());
        msgStatus->read = result["read"].asCString();
    }

    if ( result.isMember("finished") )
    {
        // vilslog::printf ("WebSocketClient::parse_NotifyMessageStatus() finished[%s]\n", result["finished"].asCString());
        msgStatus->finished = result["finished"].asCString();
    }

    if ( result.isMember("datetime") )
    {
        // vilslog::printf ("WebSocketClient::parse_NotifyMessageStatus() datetime[%s]\n\n", result["datetime"].asCString());
        msgStatus->datetime = result["datetime"].asCString();
    }

    DataItem * item = new DataItem("GAEMessageStatusData", 1, msgStatus);
    mGAEJobQueue.add(item);

    delete msgStatus;
}

// Handlers
void WebSocketClient::on_open(websocketpp::connection_hdl hdl)
{
    vilslog::printf("WebSocketClient::on_open() \n");

    std::string msg = "Hello";
    m_client.send(hdl,msg,websocketpp::frame::opcode::text);
    //m_client.get_alog().write(websocketpp::log::alevel::app, "Sent Message: "+msg);
}

void WebSocketClient::on_fail(websocketpp::connection_hdl hdl)
{
    vilslog::printf("WebSocketClient::on_fail() \n");

    //m_client.get_alog().write(websocketpp::log::alevel::app, "Connection Failed");
}

void WebSocketClient::on_message(websocketpp::connection_hdl hdl, message_ptr msg)
{
    // vilslog::printf("WebSocketClient::on_message() payload[%s]\n", msg->get_payload().c_str());

    parse_message(msg->get_payload());

    // m_client.get_alog().write(websocketpp::log::alevel::app, "Received Reply: "+msg->get_payload());
    //m_client.close(hdl,websocketpp::close::status::normal,"");
}

void WebSocketClient::on_close(websocketpp::connection_hdl hdl)
{
    vilslog::printf("WebSocketClient::on_close() \n");

    m_client.close(hdl,websocketpp::close::status::normal,"");

    //m_client.get_alog().write(websocketpp::log::alevel::app, "Connection Closed");
}
