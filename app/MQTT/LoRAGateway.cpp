/*
 * LoRAGateway.h
 *
 *  Created on: 2020/02/15
 *      Author: ikki
 */
#include <stdio.h>
#include <stdlib.h>
#include "string.h"
#include <iostream>
#include <sstream>

#include <netdb.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <resolv.h>
#include <arpa/inet.h>
#include "../logger/log.h"

#include "LoRAGateway.h"

LoRAGateway::LoRAGateway(const char * ip)
    : mGatewayIP(ip)
{
}

bool LoRAGateway::resetLoRA()
{
    bool bFound = false;

    char token[0xfff];
    char sysauth[0xfff];
    memset(token, 0, 0xfff);
    memset(sysauth, 0, 0xfff);

    bFound = login_and_get_SysauthToken(sysauth, token);

    if ( bFound )
    {
        bFound = send_reset_cmd("lora_restart", sysauth, token);
        //bFound = send_reset_cmd("lora_mqtt_restart", sysauth, token);
    }

    return bFound;
}

bool LoRAGateway::getPage()
{
    const char *host = mGatewayIP.c_str(); // 目標 URI
    const char *command = "cgi-bin/luci";
    const char *PORT_NUM = "80"; // HTTP port

    char request[4096];
    memset(request, 0, 4096);

    //char request[0xfff], response[4096]; // 請求 與 回應訊息
    const char *requestLineFmt = "GET /%s HTTP/1.1\r\n"; // 請求行
    const char *headerFmt = "Host: %s:%s\r\n"; // Host 表頭欄位
    const char *CRLF = "\r\n";  // 表頭後的 CRLF

    //int cfd; // Socket 檔案描述符 (File Descriptor)
    //int gaiStatus; // getaddrinfo 狀態碼
    //struct addrinfo hints; // hints 參數，設定 getaddrinfo() 的回傳方式
    //struct addrinfo *result; // getaddrinfo() 執行結果的 addrinfo 結構指標

    char buffer[1024];
    int bufferLen = 1024;
    memset(buffer, 0, bufferLen);

    //組裝請求訊息
    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, requestLineFmt, command);
    strcpy(request, buffer);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, headerFmt, host, PORT_NUM);
    strcat(request, buffer);

    strcat(request, CRLF);

    //
    // sen out request
    //
    if ( sendRequest(host, PORT_NUM, request) )
    {
        return true;
    }
    else
    {
        vilslog::printf("LoRAGateway::getPage() failed!\n");
        return false;
    }
}

bool LoRAGateway::sendRequest(const char * host, const char * port, const char * request)
{
    int cfd; // Socket 檔案描述符 (File Descriptor)
    int gaiStatus; // getaddrinfo 狀態碼
    struct addrinfo hints; // hints 參數，設定 getaddrinfo() 的回傳方式
    struct addrinfo *result; // getaddrinfo() 執行結果的 addrinfo 結構指標
    char response[0xfff]; // 回應訊息
    size_t n = 0;

    // 以 memset 清空 hints 結構
    memset(&hints, 0, sizeof(struct addrinfo));
    //hints.ai_family = AF_UNSPEC; // 使用 IPv4 or IPv6
    hints.ai_family = AF_INET; // 使用 IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // 串流 Socket
    hints.ai_flags = AI_NUMERICSERV; // 將 getaddrinfo() 第 2 參數 (PORT_NUM) 視為數字

    // 以 getaddrinfo 透過 DNS，取得 addrinfo 鏈結串列 (Linked List)
    // 以從中取得 Host 的 IP 位址
    if ((gaiStatus = getaddrinfo(host, port, &hints, &result)) != 0)
    {
        errPrint((char *) gai_strerror(gaiStatus));
        return false;
    }


    // 分別以 domain, type, protocol 建立 socket 檔案描述符
    cfd = socket(result->ai_family, result->ai_socktype, 0);

    if ( cfd == 0 )
    {
        errPrint((char *) gai_strerror(gaiStatus));
        return false;
    }

    // 以 socket 檔案描述符 (cfd), addr, addrlen 進行連線
    // 其中，result->ai_addr 為 gai 取得之 通用 socket 位址結構 -- sockaddr
    if (connect(cfd, result->ai_addr, result->ai_addrlen) < 0)
    {
        errPrint("Connect");
        return false;
    }


    // 釋放 getaddrinfo (Linked List) 記憶體空間
    freeaddrinfo(result);
    result = NULL;

    // 格式化輸出請求訊息
    printf("----------\nRequest:\n----------\n%s\n", request);

    // 發送請求
    if (send(cfd, request, strlen(request), 0) <= 0)
    {
        errPrint("Send");
        return false;
    }

    // 接收回應
    if ((n = recv(cfd, response, 0xfff, 0)) <= 0)
    {
        errPrint("Receive");
        return false;
    }


    // 格式化輸出回應訊息
    printf("----------\nResponse:\n----------\n%s\n", response);

/*
    ----------
    Response:
    ----------
    HTTP/1.1 200 OK
    Content-Type: text/plain;charset=utf-8
    X-Cloud-Trace-Context: 252abfff9e7faacb4ddeb8eefb54770b;o=1
    Date: Fri, 02 Aug 2019 07:37:59 GMT
    Server: Google Frontend
    Content-Length: 63

    {  "name": "projects/smimsvils/messages/5888366962163071374"}
*/


    int header_end_index = 0;
    std::list<HttpHeader*> * httpHeaderlist = NULL;
    httpHeaderlist = parse_http_header(response, n);

    if (httpHeaderlist != NULL)
    {
        for (std::list<HttpHeader *>::iterator header = httpHeaderlist->begin(); header != httpHeaderlist->end(); header++)
        {
            //vilslog::printf("%s:%s\n", (*header)->parameter.c_str(), (*header)->value.c_str());

            if ((*header)->parameter.compare("header_end_index") == 0)
            {
                header_end_index = atoi((*header)->value.c_str());
            }
        }
    }

    std::string content = response + header_end_index;

    vilslog::printf("content.length[%d] content[%s]\n", content.length(), content.c_str());

    // string::size_type found = content.find("{", 0);
    // if (found != string::npos)
    // {
    //     std::string http_response_json = content.substr(found);
    //
    //     //vilslog::printf ("http_response_json.length[%d]\n", http_response_json.length());
    //     //printf ("\n-------------------\n%s\n-------------------\n", http_response_json.c_str());
    //
    //     Json::Reader * reader = new Json::Reader();
    //     Json::Value JSONresult;
    //     bool parsingSuccessful = false;
    //
    //     try{
    //         parsingSuccessful = reader->parse( http_response_json.c_str(), JSONresult);
    //
    //         if (parsingSuccessful)
    //         {
    //
    //         }
    //     }
    //     catch(exception exc)
    //     {
    //         vilslog::printf ("SyncERPService::sendRequest() reader->parse() catch exception[%s]\n", exc.what());
    //     }
    //
    //     delete reader;
    // }

    if (httpHeaderlist != NULL)
    {
        clear_HttpHeader_list(httpHeaderlist);
        delete httpHeaderlist;
    }

    // 半雙工關閉 TCP Socket 連線
    // (i.e., 關閉寫入)
    shutdown(cfd, SHUT_WR);
    close(cfd);
    return true;
}

bool LoRAGateway::login_and_get_SysauthToken(char * sysauth, char * token)
{
    const char *host = mGatewayIP.c_str(); // 目標 URI
    const char *command = "cgi-bin/luci";
    const char *PORT_NUM = "80"; // HTTP port

    char request[4096];
    memset(request, 0, 4096);

    //char request[0xfff], response[0xfff]; // 請求 與 回應訊息
    //const char *requestLine = "GET /sendmessage HTTP/1.1\r\n"; // 請求行
    const char *requestLineFmt = "POST /%s HTTP/1.1\r\n"; // 請求行
    const char *headerFmt = "Host: %s:%s\r\n"; // Host 表頭欄位
    const char *ContentType = "Content-Type: application/x-www-form-urlencoded\r\n";
    const char *ContentLengthFmt = "Content-Length: %d\r\n\r\n";
    const char *MessageFmt = "username=%s&password=%s";
    const char *CRLF = "\r\n";  // 表頭後的 CRLF

    // int cfd; // Socket 檔案描述符 (File Descriptor)
    // int gaiStatus; // getaddrinfo 狀態碼
    // struct addrinfo hints; // hints 參數，設定 getaddrinfo() 的回傳方式
    // struct addrinfo *result; // getaddrinfo() 執行結果的 addrinfo 結構指標

    char buffer[4096];
    int bufferLen = 4096;
    memset(buffer, 0, bufferLen);

    //組裝請求訊息
    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, requestLineFmt, command);
    strcpy(request, buffer);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, headerFmt, host, PORT_NUM);
    strcat(request, buffer);

    strcat(request, ContentType);

    char sendmessage[4096];
    memset(sendmessage, 0, 4096);
    snprintf(sendmessage, 4096, MessageFmt, "admin", "admin");

    memset(buffer, 0, 4096);
    snprintf(buffer, bufferLen, ContentLengthFmt, strlen(sendmessage));
    strcat(request, buffer);

    strcat(request, sendmessage);
    strcat(request, CRLF);

    //
    // sen out request
    //

    if ( getSysauthToken(host, PORT_NUM, request, sysauth, token) )
    {
        return true;
    }
    else
    {
        vilslog::printf("LoRAGateway::login_and_get_SysauthToken() failed!\n");
        return false;
    }
}

bool LoRAGateway::send_reset_cmd(const char * function, char * sysauth, char * token)
{
    const char *host = mGatewayIP.c_str(); // 目標 URI
    const char *command = "cgi-bin/luci/";
    const char *PORT_NUM = "80"; // HTTP port

    vilslog::printf("LoRAGateway::send_reset_cmd() function[%s] sysauth[%s] token[%s]\n", function, sysauth, token);

    char request[4096];
    memset(request, 0, 4096);

    //char request[0xfff], response[4096]; // 請求 與 回應訊息
    const char *requestLineFmt = "GET /%s;stok=%s/admin/giot/%s HTTP/1.1\r\n"; // 請求行
    const char *headerFmt = "Host: %s:%s\r\n"; // Host 表頭欄位
    const char *RefererFmt = "Referer: http://192.168.55.1/cgi-bin/luci/;stok=%s/admin/giot/dms\r\n"; // Host 表頭欄位
    const char *cookieFmt = "Cookie: sysauth=%s; sysauth=\r\n"; // Host 表頭欄位
    const char *CRLF = "\r\n";  // 表頭後的 CRLF

    //int cfd; // Socket 檔案描述符 (File Descriptor)
    //int gaiStatus; // getaddrinfo 狀態碼
    //struct addrinfo hints; // hints 參數，設定 getaddrinfo() 的回傳方式
    //struct addrinfo *result; // getaddrinfo() 執行結果的 addrinfo 結構指標

    char buffer[1024];
    int bufferLen = 1024;
    memset(buffer, 0, bufferLen);

    //組裝請求訊息
    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, requestLineFmt, command, token, function);
    strcpy(request, buffer);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, headerFmt, host, PORT_NUM);
    strcat(request, buffer);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, RefererFmt, token);
    strcat(request, buffer);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, cookieFmt, sysauth);
    strcat(request, buffer);

    strcat(request, CRLF);

    //
    // sen out request
    //
    if ( sendRequest(host, PORT_NUM, request) )
    {
        return true;
    }
    else
    {
        vilslog::printf("LoRAGateway::send_reset_cmd() failed!\n");
        return false;
    }
}

bool LoRAGateway::getSysauthToken(const char * host, const char * port, const char * request, char * sysauth, char * token)
{
    bool bFound = false;
    int cfd; // Socket 檔案描述符 (File Descriptor)
    int gaiStatus; // getaddrinfo 狀態碼
    struct addrinfo hints; // hints 參數，設定 getaddrinfo() 的回傳方式
    struct addrinfo *result; // getaddrinfo() 執行結果的 addrinfo 結構指標
    char response[0xfff]; // 回應訊息
    size_t n = 0;

    // 以 memset 清空 hints 結構
    memset(&hints, 0, sizeof(struct addrinfo));
    //hints.ai_family = AF_UNSPEC; // 使用 IPv4 or IPv6
    hints.ai_family = AF_INET; // 使用 IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // 串流 Socket
    hints.ai_flags = AI_NUMERICSERV; // 將 getaddrinfo() 第 2 參數 (PORT_NUM) 視為數字
    //hints.ai_flags = AI_PASSIVE;

    // 以 getaddrinfo 透過 DNS，取得 addrinfo 鏈結串列 (Linked List)
    // 以從中取得 Host 的 IP 位址
    if ((gaiStatus = getaddrinfo(host, port, &hints, &result)) != 0)
    {
        errPrint((char *) gai_strerror(gaiStatus));
        return false;
    }

    //分別以 domain, type, protocol 建立 socket 檔案描述符
    cfd = socket(result->ai_family, result->ai_socktype, 0);

    if ( cfd == -1 )
    {
        errPrint((char *) gai_strerror(gaiStatus));
        return false;
    }

    //================
    // // 以 socket 檔案描述符 (cfd), addr, addrlen 進行連線
    // // 其中，result->ai_addr 為 gai 取得之 通用 socket 位址結構 -- sockaddr
    if (connect(cfd, result->ai_addr, result->ai_addrlen) < 0)
    {
        errPrint("Connect");
        return false;
    }

    // 釋放 getaddrinfo (Linked List) 記憶體空間
    freeaddrinfo(result);
    result = NULL;

    // 格式化輸出請求訊息
    printf("----------\nRequest:\n----------\n%s\n", request);

    // 發送請求
    if (send(cfd, request, strlen(request), 0) <= 0)
    {
        errPrint("Send");
        return false;
    }

    // 接收回應
    if ((n = recv(cfd, response, 0xfff, 0)) <= 0)
    {
        errPrint("Receive");
        return false;
    }

    // 格式化輸出回應訊息
    //vilslog::printf("----------\nResponse:\n----------\n%s\n", response);
    vilslog::printf("----------\nResponse:\n----------\n");

    int header_end_index = 0;
    std::list<HttpHeader*> * httpHeaderlist = NULL;
    httpHeaderlist = parse_http_header(response, n);

    if (httpHeaderlist != NULL)
    {
        for (std::list<HttpHeader *>::iterator header = httpHeaderlist->begin(); header != httpHeaderlist->end(); header++)
        {
            vilslog::printf("%s:%s\n", (*header)->parameter.c_str(), (*header)->value.c_str());

            if ((*header)->parameter.compare("header_end_index") == 0)
            {
                header_end_index = atoi((*header)->value.c_str());
            }
            else
            if ((*header)->parameter.compare("Set-Cookie") == 0)
            {
                bFound = parse_SysauthToken((*header)->value, sysauth, token);
            }
        }
    }

    std::string content = response + header_end_index;

    //vilslog::printf("content[%s]\n", content.c_str());

    // string::size_type found = content.find("{", 0);
    // if (found != string::npos)
    // {
    //     std::string http_response_json = content.substr(found);
    //
    //     //vilslog::printf ("http_response_json.length[%d]\n", http_response_json.length());
    //     //printf ("\n-------------------\n%s\n-------------------\n", http_response_json.c_str());
    //
    //     Json::Reader * reader = new Json::Reader();
    //     Json::Value JSONresult;
    //     bool parsingSuccessful = false;
    //
    //     try{
    //         parsingSuccessful = reader->parse( http_response_json.c_str(), JSONresult);
    //
    //         if (parsingSuccessful)
    //         {
    //
    //         }
    //     }
    //     catch(exception exc)
    //     {
    //         vilslog::printf ("SyncERPService::sendRequest() type[%d] reader->parse() catch exception[%s]\n", type, exc.what());
    //         bSuccess = false;
    //     }
    //
    //     delete reader;
    // }

    if (httpHeaderlist != NULL)
    {
        clear_HttpHeader_list(httpHeaderlist);
        delete httpHeaderlist;
    }

    // 半雙工關閉 TCP Socket 連線
    // (i.e., 關閉寫入)
    shutdown(cfd, SHUT_WR);
    close(cfd);
    return bFound;
}

bool LoRAGateway::parse_SysauthToken(std::string value, char * sysauth, char * token)
{
    bool bFound = false;
    std::string nex_string = value;
    std::string::size_type pos = value.find(';');
    while (pos != std::string::npos)
    {
        std::string sub_string = nex_string.substr(0, pos);

        std::string::size_type pos_e = sub_string.find('=');
        if ( pos_e != std::string::npos )
        {
            std::string head = sub_string.substr(0, pos_e);
            std::string value = sub_string.substr(pos_e + 1);

            //vilslog::printf("LoRAGateway::parse_token() head[%s] value[%s]\n", head.c_str(), value.c_str());

            if ( head.compare("stok") == 0 )
            {
                std::string newvalue = remove_newline(value);
                vilslog::printf("LoRAGateway::parse_SysauthToken() stok[%s] value[%s]\n", head.c_str(), newvalue.c_str());
                sprintf(token, "%s", newvalue.c_str());
                bFound = true;
            }
            else
            if ( head.compare("sysauth") == 0 )
            {
                std::string newvalue = remove_newline(value);
                vilslog::printf("LoRAGateway::parse_SysauthToken() sysauth[%s] value[%s]\n", head.c_str(), newvalue.c_str());
                sprintf(sysauth, "%s", newvalue.c_str());
                bFound = true;
            }
        }

        //vilslog::printf("LoRAGateway::parse_token() nex_string[%s] sub_string[%s]\n", nex_string.c_str(), sub_string.c_str());
        std::string new_string = nex_string.substr(pos + 1);
        //vilslog::printf("LoRAGateway::parse_token() new_string[%s] sub_string[%s]\n", new_string.c_str(), sub_string.c_str());

        nex_string = new_string;
        pos = nex_string.find(';');
    }

    std::string::size_type pos_e = nex_string.find('=');
    if ( pos_e != std::string::npos )
    {
        std::string head = nex_string.substr(0, pos_e);
        std::string value = nex_string.substr(pos_e + 1);

        //vilslog::printf("LoRAGateway::parse_token() head[%s] value[%s]\n", head.c_str(), value.c_str());

        if ( head.compare("stok") == 0 )
        {
            std::string newvalue = remove_newline(value);
            vilslog::printf("LoRAGateway::parse_SysauthToken() stok[%s] value[%s]\n", head.c_str(), newvalue.c_str());
            sprintf(token, "%s", newvalue.c_str());
            bFound = true;
        }
    }

    return bFound;
}

std::list<HttpHeader*> * LoRAGateway::parse_http_header(const char * response, size_t n)
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

            // vilslog::printf ("PushNotificationService::parse_http_header() parameter[%s] value[%s]\n",
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

            // vilslog::printf ("PushNotificationService::parse_http_header() parameter[%s] value[%s]\n",
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

            // vilslog::printf ("PushNotificationService::parse_http_header() parameter[%s] value[%s]\n",
            //     header->parameter.c_str(), header->value.c_str());

            bFinished = true;
        }
        else
        {
            //vilslog::printf ("SyncERPService::parse_http_header() buffer[%s]\n", buffer);
        }
    }
    return httpHeaderlist;
}

int LoRAGateway::get_line(const char * databuffer, int data_len, int start_idx, char * linebuffer, int buffer_len)
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

void LoRAGateway::errExit(const char *reason)
{
    std::string buff = reason ? reason : strerror(errno);
    printf("Error: %s", buff.c_str());
    exit(EXIT_FAILURE);
}

void LoRAGateway::errPrint(const char *reason)
{
    std::string buff = reason ? reason : strerror(errno);
    printf("Error: %s", buff.c_str());
}
