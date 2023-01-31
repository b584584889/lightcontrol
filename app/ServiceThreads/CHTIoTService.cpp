/*
 *  CHTIoTService.h
 *
 *  Created on: 2020/05/07
 *      Author: ikki
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <iostream>

#include <netdb.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <openssl/engine.h>
#include <openssl/hmac.h>
#include <openssl/evp.h>

#include "CHTIoTService.h"

using namespace std;

CHTIoTService::CHTIoTService(wqueue<DataItem*> & CHTIoTJobQueue)
    : mCHTIoTJobQueue(CHTIoTJobQueue)
{
}

CHTIoTService::~CHTIoTService()
{
}

bool CHTIoTService::status()
{
    return mInitial;
}

std::string CHTIoTService::statusString()
{
    return mStatus;
}

void * CHTIoTService::run()
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
            vilslog::printf("CHTIoTService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "CHTIoTService()::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void CHTIoTService::runloop()
{
    long long start1 = getSystemTime();
    long long end1 = getSystemTime();
    long long difft = end1 - start1;
    long long syncTime = 3590000;
    long long heartbeatTime = 600000;

    mInitial = true;

    mStatus = "CHTIoTService()::run() Initial Courses successfully!";
    vilslog::printf ("CHTIoTService()::run() Initial Courses successfully!\n");

    while (1)
    {
        DataItem * item = mCHTIoTJobQueue.removetimed(10000);
        if( item != NULL )
        {
            processDataItem(item);
            delete item;
        }

        end1 = getSystemTime();
        difft = end1 - start1;

        syncTime += difft;
        heartbeatTime += difft;

        if ( syncTime >= 3600000) //  1 hour
        {
            std::string DeviceID = CHT_IOT_DeviceID;
            std::string DeviceKey = CHT_IOT_DeviceKey;
            std::string SensorID = CHT_IOT_SensorID;
            std::string dataValue = "1";

            upload_cht_iot(DeviceID, DeviceKey, SensorID, dataValue);
            syncTime = 0;
        }

        if ( heartbeatTime >= 600000) //  10 mins
        {
            std::string DeviceID = CHT_IOT_DeviceID;
            std::string DeviceKey = CHT_IOT_DeviceKey;
            std::string pulseValue = "600000";
            upload_cht_iot_heartbeat(DeviceID, DeviceKey, pulseValue);
            heartbeatTime = 0;
        }

        start1 = end1;
    }
}

int CHTIoTService::processDataItem(DataItem * dataItem)
{
    std::string DeviceID = CHT_IOT_DeviceID;
    std::string DeviceKey = CHT_IOT_DeviceKey;

    if ( strcmp(dataItem->getType(), "enterCounter") == 0 )
    {
        std::string SensorID = "enterCounter";
        std::string dataValue = dataItem->getMessage();

        upload_cht_iot(DeviceID, DeviceKey, SensorID, dataValue);
    }
    else
    if ( strcmp(dataItem->getType(), "exitCounter") == 0 )
    {
        std::string SensorID = "exitCounter";
        std::string dataValue = dataItem->getMessage();

        upload_cht_iot(DeviceID, DeviceKey, SensorID, dataValue);
    }

    return 0;
}

bool CHTIoTService::upload_cht_iot(std::string DeviceID, std::string DeviceKey, std::string SensorID, std::string dataValue)
{
    const char *host = "iot.cht.com.tw"; // 目標 URI
    const char *PORT_NUM = "80"; // HTTP port

    char request[4096];
    memset(request, 0, 4096);

    const char *requestLineFmt = "POST /iot/v1/device/%s/rawdata HTTP/1.1\r\n"; // 請求行
    const char *headerFmt = "Host: %s\r\n"; // Host 表頭欄位
    const char *apiKeyFmt = "CK:%s\r\n";
    //const char *ContentType = "Content-Type: application/x-www-form-urlencoded\r\n";
    const char *ContentLengthFmt = "Content-Length: %d\r\n\r\n";
    const char *MessageFmt = "[{\"id\":\"%s\",\"time\":\"%s\",\"save\":true,\"value\":[\"%s\"]}]\r\n";
    //const char *CRLF = "\r\n";  // 表頭後的 CRLF

    char buffer[4096];
    int bufferLen = 4096;
    memset(buffer, 0, bufferLen);

    //組裝請求訊息
    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, requestLineFmt, DeviceID.c_str());
    strcpy(request, buffer);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, headerFmt, host);
    strcat(request, buffer);

    memset(buffer, 0, bufferLen);
    // snprintf(buffer, bufferLen, apiKeyFmt, "PKRTXRFZ5WW2X93U9M"); // project key
    snprintf(buffer, bufferLen, apiKeyFmt, DeviceKey.c_str()); //device key
    strcat(request, buffer);

    // strcat(request, ContentType);

    char sendmessage[4096];
    memset(sendmessage, 0, 4096);
    snprintf(sendmessage, 4096, MessageFmt, SensorID.c_str(), currentDateTimeISO8601().c_str(), dataValue.c_str());

    memset(buffer, 0, 4096);
    snprintf(buffer, bufferLen, ContentLengthFmt, strlen(sendmessage));
    strcat(request, buffer);

    strcat(request, sendmessage);

    //strcat(request, CRLF);

    // vilslog::printf("CHTIoTService::upload_cht_iot() request[%s]\n", request);

    //
    // sen out request
    //
    std::string http_response_string = sendRequest(host, PORT_NUM, request);

    // vilslog::printf("CHTIoTService::upload_cht_iot() [%s]\n", http_response_string.c_str());

    return true;
}

bool CHTIoTService::upload_cht_iot_heartbeat(std::string DeviceID, std::string DeviceKey, std::string pulseValue)
{
    const char *host = "iot.cht.com.tw"; // 目標 URI
    const char *PORT_NUM = "80"; // HTTP port

    char request[4096];
    memset(request, 0, 4096);

    const char *requestLineFmt = "POST /iot/v1/device/%s/heartbeat HTTP/1.1\r\n"; // 請求行
    const char *headerFmt = "Host: %s\r\n"; // Host 表頭欄位
    const char *apiKeyFmt = "CK:%s\r\n";
    const char *ContentLengthFmt = "Content-Length: %d\r\n\r\n";
    const char *MessageFmt = "{\"pulse\":\"%s\"}\r\n";

    char buffer[4096];
    int bufferLen = 4096;
    memset(buffer, 0, bufferLen);

    //組裝請求訊息
    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, requestLineFmt, DeviceID.c_str());
    strcpy(request, buffer);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, headerFmt, host);
    strcat(request, buffer);

    memset(buffer, 0, bufferLen);
    // snprintf(buffer, bufferLen, apiKeyFmt, "PKRTXRFZ5WW2X93U9M"); // project key
    snprintf(buffer, bufferLen, apiKeyFmt, DeviceKey.c_str()); //device key
    strcat(request, buffer);

    // strcat(request, ContentType);

    char sendmessage[4096];
    memset(sendmessage, 0, 4096);
    snprintf(sendmessage, 4096, MessageFmt, pulseValue.c_str());

    memset(buffer, 0, 4096);
    snprintf(buffer, bufferLen, ContentLengthFmt, strlen(sendmessage));
    strcat(request, buffer);

    strcat(request, sendmessage);

    //
    // sen out request
    //
    std::string http_response_string = sendRequest(host, PORT_NUM, request);

    // vilslog::printf("CHTIoTService::upload_cht_iot_heartbeat() [%s]\n", http_response_string.c_str());

    return true;
}

std::string CHTIoTService::sendRequest(const char * host, const char * port, const char * request)
{
    int cfd; // Socket 檔案描述符 (File Descriptor)
    int gaiStatus; // getaddrinfo 狀態碼
    struct addrinfo hints; // hints 參數，設定 getaddrinfo() 的回傳方式
    struct addrinfo *result; // getaddrinfo() 執行結果的 addrinfo 結構指標
    char response[4096]; // 回應訊息
    std::string http_response_string = "";

    // 以 memset 清空 hints 結構
    //memset(&hints, 0, sizeof(struct addrinfo));
    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_UNSPEC; // 使用 IPv4 or IPv6
    //hints.ai_family = AF_INET; // 使用 IPv4
    hints.ai_socktype = SOCK_STREAM; // 串流 Socket
    hints.ai_flags = AI_NUMERICSERV; // 將 getaddrinfo() 第 2 參數 (PORT_NUM) 視為數字

    // 以 getaddrinfo 透過 DNS，取得 addrinfo 鏈結串列 (Linked List)
    // 以從中取得 Host 的 IP 位址
    //if ((gaiStatus = getaddrinfo(host, "http", &hints, &result)) != 0)
    if ((gaiStatus = getaddrinfo(host, port, &hints, &result)) != 0)
    {
        vilslog::printf("CHTIoTService::sendRequest() getaddrinfo() failed!\n");
        errPrint((char *) gai_strerror(gaiStatus));
        return http_response_string;
    }

    // 分別以 domain, type, protocol 建立 socket 檔案描述符
    cfd = socket(result->ai_family, result->ai_socktype, 0);
    if ( cfd < 0)
    {
        vilslog::printf("CHTIoTService::sendRequest() socket() failed!\n");
        errPrint((char *) gai_strerror(gaiStatus));
        return http_response_string;
    }

    // 以 socket 檔案描述符 (cfd), addr, addrlen 進行連線
    // 其中，result->ai_addr 為 gai 取得之 通用 socket 位址結構 -- sockaddr
    if (connect(cfd, result->ai_addr, result->ai_addrlen) < 0)
    {
        vilslog::printf("CHTIoTService::sendRequest() connect() failed!\n");
        errPrint((char *) gai_strerror(gaiStatus));
        return http_response_string;
    }

    // 釋放 getaddrinfo (Linked List) 記憶體空間
    freeaddrinfo(result);
    result = NULL;

    // 格式化輸出請求訊息
    //vilslog::printf("----------\nRequest:\n----------\n%s\n", request);

    // 發送請求
    if (send(cfd, request, strlen(request), 0) < 0)
    {
        vilslog::printf("CHTIoTService::sendRequest() send() failed!\n");
        errPrint("Send");
        return http_response_string;
    }

    // 接收回應
    size_t size = 4096;
    size_t total = 0, n = 0;

    memset(response, 0, size);

    // 格式化輸出回應訊息
    // vilslog::printf ("----------\nResponse:\n");

    bool parse_header = true;
    bool chunked = false;
    int header_end_index = 0;
    std::list<HttpHeader*> * httpHeaderlist = NULL;

    // Transfer-Encoding: chunked\r\n
    // 如果是chunked, 則主體內容實際長度並不同Content-Length (最終長度是相同). 這種格式是將內容分成多個chunked, 每個chunked格式為:
    // chunked byte數\r\n
    // 實際資料\r\n
    // 注意都有\r\n喔. chunked byte數的格式為16進位字串

    while((n = recv(cfd, response, size, 0)) > 0)
    {
        total += n;

        // vilslog::printf ("\n-------\ntotal[%d] n[%d]\n", total, n);
        // vilslog::printf ("response[%c][%c][%c][%c][%c][%c][%c][%c]\n",
        // response[0], response[1], response[2], response[3], response[4], response[5], response[6], response[7]);
        // vilslog::printf ("response[%d][%d][%d][%d][%d][%d][%d][%d]\n",
        // response[0], response[1], response[2], response[3], response[4], response[5], response[6], response[7]);

        if (parse_header)
        {
            httpHeaderlist = parse_http_header(response, n);
            if (httpHeaderlist != NULL)
            {
                //
                // check if content chunked
                //
                for (std::list<HttpHeader *>::iterator header = httpHeaderlist->begin(); header != httpHeaderlist->end(); header++)
                {
                    if ((*header)->parameter.compare("Transfer-Encoding") == 0)
                    {
                        string::size_type found = (*header)->value.find("chunked", 0);
                        if (found != string::npos)
                        {
                            chunked = true;
                        }
                    }
                    else
                    if ((*header)->parameter.compare("header_end_index") == 0)
                    {
                        header_end_index = atoi((*header)->value.c_str());
                    }
                }
                parse_header = false;
            }
        }
        else
        {
            header_end_index = 0;
        }

        std::string content = "";
        if (chunked)
        {
            // f66\r\n
            std::string tmpStr = response + header_end_index;
            string::size_type chunkedEnd = tmpStr.find("\r\n", 0);

            if (chunkedEnd != string::npos)
            {
                //printf ("chunkedEnd[%d] != string::npos\n", (int)chunkedEnd);
                if (chunkedEnd > 6)
                {
                    // maybe not content  chunked size info
                    response[(int)chunkedEnd] = '\0';

                    content = response + header_end_index;
                }
                else
                {
                    content = response + header_end_index + chunkedEnd + 2;
                }
            }
            else
            {
                //printf ("chunkedEnd == string::npos\n");
                content = response + header_end_index;
            }
        }
        else
        {
            //printf ("!chunkedEnd\n");
            content = response + header_end_index;
        }

        // vilslog::printf ("\n-------\ncontent:%s\n---------------\n", content.c_str());

        http_response_string.append(content);

        //printf ("\n-------------------\nresponse:%d\n%s\n------------------\n",
        //   (int)http_response_string.length(), http_response_string.c_str());

        if (n >= 4)
        {
            // read not use data
            //
            // find http end \r\n\r\n
            //
            //if (response[n-4] == '\r' && response[n-3] == '\n' &&response[n-2] == '\r' &&response[n-1] == '\n')
            if (response[n-2] == '\r' &&response[n-1] == '\n')
            {
                //vilslog::printf ("Found Http end!!!\n");
                break;
            }
        }

        memset(response, 0, size);
    }

    if (httpHeaderlist != NULL)
    {
        clear_HttpHeader_list(httpHeaderlist);
        delete httpHeaderlist;
    }

    // 半雙工關閉 TCP Socket 連線
    // (i.e., 關閉寫入)
    shutdown(cfd, SHUT_WR);
    close(cfd);
    return http_response_string;

}


std::list<HttpHeader*> * CHTIoTService::parse_http_header(const char * response, size_t n)
{
    // HTTP/1.1 200 OK\r\n
    // Server: nginx/1.10.3 (Ubuntu)\r\n
    // Date: Fri, 16 Aug 2019 01:36:41 GMT\r\n
    // Content-Type: application/json; charset=utf-8\r\n
    // Transfer-Encoding: chunked\r\n
    // Connection: keep-alive\r\n
    // Access-Control-Allow-Origin: *\r\n
    // \r\n
    // f66\r\n
    // {"success"...

    char buffer[4096];
    std::list<HttpHeader*> * httpHeaderlist = NULL;

    memset(buffer, 0, 4096);
    int datalen = get_line(response, (int)n, 0, buffer, 4096);
    buffer[datalen] = 0;

    if (response[0] == 'H' && response[1] == 'T' && response[2] == 'T' && response[3] == 'P')
    {
        httpHeaderlist = new std::list<HttpHeader*>();
    }
    else
    {
        return NULL;
    }

    int start_idx = 0;
    bool bFinished = false;
    while(!bFinished)
    {
        memset(buffer, 0, 4096);
        int datalen = get_line(response, (int)n, start_idx, buffer, 4096);
        buffer[datalen] = 0;

        // pair 屬性名稱: 屬性值\r\n
        std::string bufString = buffer;
        string::size_type loc = bufString.find(":", 0);
        if (loc != string::npos)
        {
            HttpHeader * header = new HttpHeader();

            header->parameter = bufString.substr(0, loc);
            header->value = bufString.substr(loc+2, bufString.length() - 2 - 2);

            // vilslog::printf ("CHTIoTService::parse_http_header() parameter[%s] value[%s]\n",
            //     header->parameter.c_str(), header->value.c_str());

            httpHeaderlist->push_back(header);
        }

        start_idx += datalen;

        if(datalen == 0)
        {
            HttpHeader * header = new HttpHeader();
            header->parameter = "header_end_index";
            header->value = std::to_string(start_idx);
            httpHeaderlist->push_back(header);

            // vilslog::printf ("CHTIoTService::parse_http_header() parameter[%s] value[%s]\n",
            //     header->parameter.c_str(), header->value.c_str());

            bFinished = true;
        }
        else
        if(datalen == 2)
        {
            // header end \r\n\r\n
            HttpHeader * header = new HttpHeader();
            header->parameter = "header_end_index";
            header->value = std::to_string(start_idx);
            httpHeaderlist->push_back(header);

            // vilslog::printf ("CHTIoTService::parse_http_header() parameter[%s] value[%s]\n",
            //     header->parameter.c_str(), header->value.c_str());

            bFinished = true;
        }
        else
        {
            //vilslog::printf ("CHTIoTService::parse_http_header() buffer[%s]\n", buffer);
        }
    }
    return httpHeaderlist;
}

int CHTIoTService::get_line(const char * databuffer, int data_len, int start_idx, char * linebuffer, int buffer_len)
{
    int length = 0;
    bool bFound = false;
    // find 0x10 0x13
    for (int i = start_idx; i < data_len; i++)
    {
        if(length < buffer_len)
        {
            linebuffer[length] = databuffer[i];
            length++;
        }

        if (databuffer[i] == '\r')
        {
            if ((i+1) < (data_len))
            {
                if (databuffer[i+1] == '\n')
                {
                    linebuffer[length] = databuffer[i];
                    length++;
                    bFound = true;
                }
            }
        }

        if(bFound)
            break;

    }

    return length;
}

void CHTIoTService::errExit(const char *reason)
{
    std::string buff = reason ? reason : strerror(errno);
    printf("CHTIoTService::errExit() Error: %s\n", buff.c_str());
    exit(EXIT_FAILURE);
}

void CHTIoTService::errPrint(const char *reason)
{
    std::string buff = reason ? reason : strerror(errno);
    vilslog::printf("CHTIoTService::errPrint() Error: %s\n", buff.c_str());
}
