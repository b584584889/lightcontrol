/*
 *  ForacareService.cpp
 *
 *  Created on: 2020/02/17
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

#include "../common.h"
#include "../websocket/websocketpp/base64/base64.hpp"

#include "../htmlcxx/html/ParserDom.h"
#include "../htmlcxx/html/wincstring.h"

#include "ForacareService.h"

using namespace std;

ForacareService::ForacareService(wqueue<WorkItem*> & queue)
    : m_queue(queue)
{
}

ForacareService::~ForacareService()
{
}

bool ForacareService::status()
{
    return mInitial;
}

std::string ForacareService::statusString()
{
    return mStatus;
}

void * ForacareService::run()
{
    while (1)
    {
        mDataUpdateTime = 0;
        mLogininfo = new ForacareLoginStruct();

        try
        {
            mInitial = true;
            mStatus = "ForacareService() Initial successfully!";
            vilslog::printf ("ForacareService()::run() Initial successfully!\n");

            runloop();

            vilslog::printf ("ForacareService() Initial failed!\n");
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("ForacareService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");
            mInitial = false;
            mStatus = "ForacareService::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void ForacareService::runloop()
{
    char type[64];
    long long start1 = getSystemTime();
    long long updateTime = 0;
    long long checkTime = 45000;

    // available to process.
    while (1)
    {
        WorkItem * item = m_queue.removetimed(1000);

        long long end1 = getSystemTime();
        long long difft = end1 - start1;
        updateTime += difft;
        checkTime += difft;

        if (checkTime >= 60000)
        {
            //updateInfo();

            checkTime = 0;
        }

        if (item != NULL)
        {
            sprintf(type, "%s", item->getType());

            if ( strcmp(type, "update") == 0)
            {
                updateInfo();
            }

            delete item;
        }

        start1 = end1;
    }
}

std::string ForacareService::getForacareData()
{
    std::ostringstream sendstream;
    sendstream<<"{\"header\":\"";

    for (std::list<std::string>::iterator header = mTableHeaderList.begin(); header != mTableHeaderList.end(); header++)
    {
        if ( header != mTableHeaderList.begin() )
            sendstream<<",";
        sendstream<<(*header).c_str();
    }
    sendstream<<"\",";
    sendstream<<"\"data\":[\"";

    for (std::list<std::string>::iterator data = mTableDataList.begin(); data != mTableDataList.end(); data++)
    {
        if ( data != mTableDataList.begin() )
            sendstream<<"\",\"";
        sendstream<<(*data).c_str();
    }
    sendstream<<"\"]}";

    std::string sendString = sendstream.str();

    return sendString;
}

void ForacareService::updateInfo()
{

    std::string host = "telehealth.foracare.com.tw";
    std::string port = "80";
    // int fd = createConnectSocket(host.c_str(), port.c_str());
    // if( fd == 0 )
    // {
    //     vilslog::printf("ForacareService::updateInfo() createConnectSocket failed!\n");
    //     return ;
    // }

    time_t curTime = getCurrentTimeInSec();
    int diff_t = (int)(curTime - mDataUpdateTime);
    if ( diff_t <= 60 )
    {
        return;
    }

    bool success = fetchData(host.c_str(), port.c_str(), mLogininfo);
    if(success)
    {
        vilslog::printf("ForacareService::updateInfo() fetchData success\n");
        mDataUpdateTime = getCurrentTimeInSec();
    }
    else
    {
        sleep(1);
        success = fetchMainPage(host.c_str(), port.c_str(), mLogininfo);
        if(success)
        {
            sleep(1);
            success = login(host.c_str(), port.c_str(), mLogininfo);
            if(success)
            {
                sleep(1);
                success = fetchPageData(host.c_str(), port.c_str(), mLogininfo);
                if(success)
                {
                    sleep(1);
                    success = fetchData(host.c_str(), port.c_str(), mLogininfo);
                    if(success)
                    {
                        vilslog::printf("ForacareService::updateInfo() fetchData success\n");
                        mDataUpdateTime = getCurrentTimeInSec();
                    }
                    else
                    {
                        vilslog::printf("ForacareService::updateInfo() fetchData failed!\n");
                    }
                }
                else
                {
                    vilslog::printf("ForacareService::updateInfo() fetchPageData failed!\n");
                }
            }
            else
            {
                vilslog::printf("ForacareService::updateInfo() login failed!\n");
            }
        }
        else
        {
            vilslog::printf("ForacareService::updateInfo() fetchMainPage failed!\n");
        }
    }

    //NewAccount();
    //AddMeter();
    //VerifyPatient();
    //QueryPatient();
}

bool ForacareService::fetchMainPage(const char * host, const char * port, ForacareLoginStruct * logininfo)
{
    //const char *host = "telehealth.foracare.com.tw"; // ?????? URI
    const char *command = "NewLogin.aspx?Lang=zh-TW&PageType=";
    //const char *PORT_NUM = "80"; // HTTP port

    // vilslog::printf("-----------------------------------------------------------\n");
    // vilslog::printf("ForacareService::fetchMainPage()\n");

    char request[4096];
    memset(request, 0, 4096);

    //char request[0xfff], response[4096]; // ?????? ??? ????????????
    const char *requestLineFmt = "GET /%s HTTP/1.1\r\n"; // ?????????
    const char *headerFmt = "Host: %s\r\n"; // Host ????????????
    const char *CRLF = "\r\n";  // ???????????? CRLF

    char buffer[1024];
    int bufferLen = 1024;
    memset(buffer, 0, bufferLen);

    //??????????????????
    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, requestLineFmt, command);
    strcpy(request, buffer);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, headerFmt, host, port);
    strcat(request, buffer);

    strcat(request, CRLF);

    //vilslog::printf("ForacareService::fetchMainPage() request=[%s]\n", request);
#ifdef DUMP_WEB_REQUEST
    vilslog::printf("-------------------------------------------\n");
    vilslog::printf("ForacareService::fetchMainPage() request=[%s]\n", request);
    vilslog::printf("-------------------------------------------\n");
#endif

    //
    // sen out request
    //
    std::string http_response_string = sendRequest(host, port, request, logininfo);
    if ( http_response_string.length() == 0 )
    {
        vilslog::printf("ForacareService::fetchMainPage() failed!\n");
        return false;
    }

#ifdef DUMP_WEB_REPONSE
    vilslog::printf("-------------------------------------------\n");
    vilslog::printf("ForacareService::fetchMainPage() http_response_string=[%s]\n", http_response_string.c_str());
    vilslog::printf("-------------------------------------------\n");
#endif

    //vilslog::printf("-----------------------------------------------------------\n");
    //vilslog::printf("Start parse....\n");

    tree<htmlcxx::HTML::Node> tr;
    htmlcxx::HTML::ParserDom parser;
    //parser.parse(content);
    //tr = parser.getTree();
    tree<htmlcxx::HTML::Node> dom = parser.parseTree(http_response_string, 0);
    //cout << tr << endl;

    //Dump all links in the tree
    tree<htmlcxx::HTML::Node>::iterator it = dom.begin();
    tree<htmlcxx::HTML::Node>::iterator end = dom.end();
    for (; it != end; ++it)
    {
        //cout << it->tagName() << endl;

        if (strcasecmp(it->tagName().c_str(), "input") == 0)
        {
            it->parseAttributes();

            std::string type = it->attribute("type").second;
            if ( type.compare("hidden") == 0 )
            {
                // cout << it->attribute("id").second << " : "<< it->attribute("value").second << endl;

                if ( it->attribute("id").second.compare("__VIEWSTATE") == 0 )
                {
                    logininfo->__VIEWSTATE = it->attribute("value").second;
                }
                else
                if ( it->attribute("id").second.compare("__VIEWSTATEGENERATOR") == 0 )
                {
                    logininfo->__VIEWSTATEGENERATOR = it->attribute("value").second;
                }
                else
                if ( it->attribute("id").second.compare("__EVENTVALIDATION") == 0 )
                {
                    logininfo->__EVENTVALIDATION = it->attribute("value").second;
                }
                else
                if ( it->attribute("id").second.compare("HidF_AutoID") == 0 )
                {
                    logininfo->HidF_AutoID = it->attribute("value").second;
                }
                else
                if ( it->attribute("id").second.compare("HidF_QMType") == 0 )
                {
                    logininfo->HidF_QMType = it->attribute("value").second;
                }
                else
                if ( it->attribute("id").second.compare("HidF_PageType") == 0 )
                {
                    logininfo->HidF_PageType = it->attribute("value").second;
                }
                else
                if ( it->attribute("id").second.compare("HidF_LoginDestinationUrl") == 0 )
                {
                    logininfo->HidF_LoginDestinationUrl = it->attribute("value").second;
                }
            }
        }

    }
    //vilslog::printf("-----------------------------------------------------------\n");
    return true;
}

bool ForacareService::login(const char * host, const char * port, ForacareLoginStruct * logininfo)
{
    //const char *host = "telehealth.foracare.com.tw"; // ?????? URI
    const char *command = "NewLogin.aspx?Lang=zh-TW&PageType=";
    //const char *PORT_NUM = "80"; // HTTP port

    // vilslog::printf("-----------------------------------------------------------\n");
    // vilslog::printf("ForacareService::login()\n");

    char request[65536];
    memset(request, 0, 65536);

    const char *requestLineFmt = "POST /%s HTTP/1.1\r\n"; // ?????????
    const char *headerFmt = "Host: %s\r\n"; // Host ????????????
    const char *ContentType = "Content-Type: application/x-www-form-urlencoded; charset=utf-8\r\n";
    const char *ContentLengthFmt = "Content-Length: %d\r\n\r\n";
    // const char *CRLF = "\r\n";  // ???????????? CRLF

    char buffer[4096];
    int bufferLen = 4096;
    memset(buffer, 0, bufferLen);

    //??????????????????
    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, requestLineFmt, command);
    strcpy(request, buffer);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, headerFmt, host);
    strcat(request, buffer);

    strcat(request, ContentType);

    strcat(request, "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:72.0) Gecko/20100101 Firefox/72.0\r\n");
    strcat(request, "Accept: */*\r\n");
    strcat(request, "Accept-Language: zh-TW,zh;q=0.8,en-US;q=0.5,en;q=0.3\r\n");
    strcat(request, "Accept-Encoding: gzip, deflate\r\n");
    strcat(request, "X-Requested-With: XMLHttpRequest\r\n");
    strcat(request, "X-MicrosoftAjax: Delta=true\r\n");
    strcat(request, "Cache-Control: no-cache\r\n");
    strcat(request, "Origin: http://telehealth.foracare.com.tw\r\n");
    strcat(request, "DNT: 1\r\n");
    strcat(request, "Connection: keep-alive\r\n");
    strcat(request, "Referer: http://telehealth.foracare.com.tw/\r\n");
    strcat(request, "Cookie: ASP.NET_SessionId=");
    strcat(request, logininfo->ASP_DOT_NET_SessionId.c_str());
    strcat(request, "; font-size=S\r\n");

    //
    // prepare content
    //
    std::string sendMessage;

    sendMessage.append("ScriptManager1=");
    sendMessage.append(UriEncode((const unsigned char *)"UP_Login|But_Login"));
    sendMessage.append("&__EVENTTARGET=");
    sendMessage.append("&__EVENTARGUMENT=");
    sendMessage.append("&__VIEWSTATE=");
    sendMessage.append(UriEncode((const unsigned char *)logininfo->__VIEWSTATE.c_str()));
    sendMessage.append("&__VIEWSTATEGENERATOR=");
    sendMessage.append(UriEncode((const unsigned char *)logininfo->__VIEWSTATEGENERATOR.c_str()));
    sendMessage.append("&__EVENTVALIDATION=");
    sendMessage.append(UriEncode((const unsigned char *)logininfo->__EVENTVALIDATION.c_str()));
    sendMessage.append("&HidF_AutoID=");
    sendMessage.append("&HidF_QMType=");
    sendMessage.append("&HidF_PageType=");
    sendMessage.append("&DDL_Language=zh-TW");
    sendMessage.append("&TB_LoginName=studioatw");
    sendMessage.append("&TB_LoginPW=studioatw123");
    sendMessage.append("&HidF_LoginDestinationUrl=");
    sendMessage.append("&TBox_ForgotPwd_Acc=");
    sendMessage.append("&TBox_ForgotPwd_Email=");
    sendMessage.append("&__ASYNCPOST=true");
    sendMessage.append("&But_Login=??????");

    // vilslog::printf("ForacareService::login() sendMessage[%s]\n", sendMessage.c_str());

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, ContentLengthFmt, (int)sendMessage.length());
    strcat(request, buffer);

    strcat(request, sendMessage.c_str());

    // vilslog::printf("ForacareService::login() request[%s]\n", request);
#ifdef DUMP_WEB_REQUEST
    vilslog::printf("-------------------------------------------\n");
    vilslog::printf("ForacareService::login() request=[%s]\n", request);
    vilslog::printf("-------------------------------------------\n");
#endif

    //
    // sen out request
    //
    std::string http_response_string = sendRequest(host, port, request, logininfo);
    if ( http_response_string.length() == 0 )
    {
        vilslog::printf("ForacareService::login() failed!\n");
        return false;
    }

#ifdef DUMP_WEB_REPONSE
    vilslog::printf("-------------------------------------------\n");
    vilslog::printf("ForacareService::login() http_response_string=[%s]\n", http_response_string.c_str());
    vilslog::printf("-------------------------------------------\n");
#endif

    // vilslog::printf("ForacareService::login() http_response_string:[%s]\n", http_response_string.c_str());

    // vilslog::printf("-----------------------------------------------------------\n");
    // vilslog::printf("Start parse....\n");

    tree<htmlcxx::HTML::Node> tr;
    htmlcxx::HTML::ParserDom parser;
    tree<htmlcxx::HTML::Node> dom = parser.parseTree(http_response_string, 0);

    //Dump all links in the tree
    tree<htmlcxx::HTML::Node>::iterator it = dom.begin();
    tree<htmlcxx::HTML::Node>::iterator end = dom.end();
    int iFound = 0;

    for (; it != end; ++it)
    {
        //cout << it->tagName() << endl;
        //vilslog::printf("\n----\ntagName[%s]\n----\n", it->tagName().c_str());

        if (strcasecmp(it->tagName().c_str(), "input") == 0)
        {
            it->parseAttributes();

            std::string type = it->attribute("type").second;
            if ( type.compare("hidden") == 0 )
            {
                // cout << it->attribute("id").second << " : "<< it->attribute("value").second << endl;

                if ( it->attribute("id").second.compare("HidF_LoginDestinationUrl") == 0 )
                {
                    logininfo->HidF_LoginDestinationUrl = UriDecode((const unsigned char *)it->attribute("value").second.c_str());
                    iFound++;
                }
            }
        }
    }

    if (iFound == 0)
    {
        vilslog::printf("ForacareService::login() iFound[%d]\n", iFound);
        return false;
    }

    bool ret = parseAJAX(http_response_string, logininfo);

    // vilslog::printf("----------------------------------------------------------ret[%d]\n", ret);

    return ret;
}

bool ForacareService::fetchPageData(const char * host, const char * port, ForacareLoginStruct * logininfo)
{
    //const char *host = "telehealth.foracare.com.tw"; // ?????? URI
    //const char *PORT_NUM = "80"; // HTTP port

    char request[4096];
    memset(request, 0, 4096);

    // vilslog::printf("-----------------------------------------------------------\n");
    // vilslog::printf("ForacareService::fetchPageData()\n");


    //char request[0xfff], response[4096]; // ?????? ??? ????????????
    const char *requestLineFmt = "GET /%s HTTP/1.1\r\n"; // ?????????
    const char *headerFmt = "Host: %s\r\n"; // Host ????????????
    const char *CookieFmt = "Cookie: ASP.NET_SessionId=%s; font-size=S; .ASPXAUTH=%s\r\n"; // Host ????????????
    const char *CRLF = "\r\n";  // ???????????? CRLF

    char buffer[1024];
    int bufferLen = 1024;
    memset(buffer, 0, bufferLen);

    //??????????????????
    memset(buffer, 0, bufferLen);
    //snprintf(buffer, bufferLen, requestLineFmt, logininfo->HidF_LoginDestinationUrl.c_str());
    //snprintf(buffer, bufferLen, requestLineFmt, "CareWork/CareWorkDashboard.aspx#Menu=UL12800&BMenu=IC12000");
    //snprintf(buffer, bufferLen, requestLineFmt, "CareWork/CareWorkDashboard.aspx");
    snprintf(buffer, bufferLen, requestLineFmt, "CareWork/RealTimeMResult.aspx");
    //snprintf(buffer, bufferLen, requestLineFmt, "MeasureData/CaseDashboard.aspx?PageAutoID=22829#Menu=UL21D00&amp;BMenu=IC21000");
    //snprintf(buffer, bufferLen, requestLineFmt, "MeasureData/CaseDashboard.aspx?PageAutoID=22829");

    strcpy(request, buffer);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, headerFmt, host);
    strcat(request, buffer);

    strcat(request, "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:72.0) Gecko/20100101 Firefox/72.0\r\n");
    strcat(request, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n");
    strcat(request, "Accept-Language: zh-TW,zh;q=0.8,en-US;q=0.5,en;q=0.3\r\n");
    strcat(request, "Accept-Encoding: gzip, deflate\r\n");
    strcat(request, "DNT: 1\r\n");
    strcat(request, "Connection: keep-alive\r\n");
    //strcat(request, "Referer: http://telehealth.foracare.com.tw/NewLogin.aspx?Lang=zh-TW&PageType=\r\n");
    strcat(request, "Referer: http://telehealth.foracare.com.tw/CareWork/CareWorkDashboard.aspx\r\n");
    strcat(request, "Upgrade-Insecure-Requests: 1\r\n");



    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, CookieFmt, logininfo->ASP_DOT_NET_SessionId.c_str(), logininfo->DOT_ASPXAUTH.c_str());
    //snprintf(buffer, bufferLen, CookieFmt, "pucr53v0jbuvtdsey4oqiss3", "49CB1535381EC3EAE2D42B53A6CF7481EA0EDF2122650B3FAFC4324ACAD0A50AB4C1190B7901AE76F10987636F937EFDD528A8563566760E8FA80D0610BFD83CDA36599B887232B28094AC251FED2EED16C19536BD5E42BFCEDFF094822C28F895AAD52AE0D61657FC7F6472AFD44D8777DA2013B2936650D43F4050CE60FD1B23EFC693CDF550EBE56C51EF831027C8");
    strcat(request, buffer);

    strcat(request, CRLF);

    //vilslog::printf("request[%s]\n", request);
#ifdef DUMP_WEB_REQUEST
    vilslog::printf("-------------------------------------------\n");
    vilslog::printf("ForacareService::fetchPageData() request=[%s]\n", request);
    vilslog::printf("-------------------------------------------\n");
#endif
    //
    // sen out request
    //
    std::string http_response_string = sendRequest(host, port, request, logininfo);
    if ( http_response_string.length() == 0 )
    {
        vilslog::printf("ForacareService::fetchPageData() failed!\n");
        return false;
    }

#ifdef DUMP_WEB_REPONSE
    vilslog::printf("-------------------------------------------\n");
    vilslog::printf("ForacareService::fetchPageData() http_response_string=[%s]\n", http_response_string.c_str());
    vilslog::printf("-------------------------------------------\n");
#endif
    // vilslog::printf("http_response_string[%s]\n", http_response_string.c_str());

    //vilslog::printf("-----------------------------------------------------------\n");
    //vilslog::printf("Start parse....\n");

    tree<htmlcxx::HTML::Node> tr;
    htmlcxx::HTML::ParserDom parser;
    //parser.parse(content);
    //tr = parser.getTree();
    tree<htmlcxx::HTML::Node> dom = parser.parseTree(http_response_string, 0);
    //cout << tr << endl;

    //Dump all links in the tree
    tree<htmlcxx::HTML::Node>::iterator it = dom.begin();
    tree<htmlcxx::HTML::Node>::iterator end = dom.end();
    int iFound = 0;

    for (; it != end; ++it)
    {
        //cout << it->tagName() << endl;

        if (strcasecmp(it->tagName().c_str(), "input") == 0)
        {
            it->parseAttributes();

            std::string type = it->attribute("type").second;
            if ( type.compare("hidden") == 0 )
            {
                // cout << it->attribute("id").second << " : "<< it->attribute("value").second << endl;

                if ( it->attribute("id").second.compare("__VIEWSTATE") == 0 )
                {
                    logininfo->__VIEWSTATE = it->attribute("value").second;
                    iFound++;
                }
                else
                if ( it->attribute("id").second.compare("__VIEWSTATEGENERATOR") == 0 )
                {
                    logininfo->__VIEWSTATEGENERATOR = it->attribute("value").second;
                    iFound++;
                }
                else
                if ( it->attribute("id").second.compare("__EVENTVALIDATION") == 0 )
                {
                    logininfo->__EVENTVALIDATION = it->attribute("value").second;
                    iFound++;
                }
                else
                if ( it->attribute("id").second.compare("HidF_AutoID") == 0 )
                {
                    logininfo->HidF_AutoID = it->attribute("value").second;
                    iFound++;
                }
                else
                if ( it->attribute("id").second.compare("HidF_QMType") == 0 )
                {
                    logininfo->HidF_QMType = it->attribute("value").second;
                    iFound++;
                }
                else
                if ( it->attribute("id").second.compare("HidF_PageType") == 0 )
                {
                    logininfo->HidF_PageType = it->attribute("value").second;
                    iFound++;
                }
                else
                if ( it->attribute("id").second.compare("HidF_LoginDestinationUrl") == 0 )
                {
                    logininfo->HidF_LoginDestinationUrl = it->attribute("value").second;
                    iFound++;
                }
            }
        }
    }

    if (iFound == 0)
    {
        vilslog::printf("ForacareService::fetchPageData()  iFound[%d]\n", iFound);
        return false;
    }
    else
        return true;
}

bool ForacareService::fetchData(const char * host, const char * port, ForacareLoginStruct * logininfo)
{
    //const char *host = "telehealth.foracare.com.tw"; // ?????? URI
    //const char *PORT_NUM = "80"; // HTTP port

    char request[65536];
    memset(request, 0, 65536);

    // vilslog::printf("-----------------------------------------------------------\n");
    // vilslog::printf("ForacareService::fetchData()\n");


    //char request[0xfff], response[4096]; // ?????? ??? ????????????
    const char *requestLineFmt = "POST /%s HTTP/1.1\r\n"; // ?????????
    const char *headerFmt = "Host: %s\r\n"; // Host ????????????
    const char *ContentType = "Content-Type: application/x-www-form-urlencoded; charset=utf-8\r\n";
    const char *ContentLengthFmt = "Content-Length: %d\r\n\r\n";
    const char *CookieFmt = "Cookie: ASP.NET_SessionId=%s; font-size=S; .ASPXAUTH=%s\r\n"; // Host ????????????
    const char *CRLF = "\r\n";  // ???????????? CRLF

    char buffer[1024];
    int bufferLen = 1024;
    memset(buffer, 0, bufferLen);

    //??????????????????
    memset(buffer, 0, bufferLen);
    //snprintf(buffer, bufferLen, requestLineFmt, "CareWork/CareWorkDashboard.aspx");
    snprintf(buffer, bufferLen, requestLineFmt, "CareWork/RealTimeMResult.aspx");

    strcpy(request, buffer);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, headerFmt, host);
    strcat(request, buffer);

    strcat(request, "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:72.0) Gecko/20100101 Firefox/72.0\r\n");
    strcat(request, "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8\r\n");
    strcat(request, "Accept-Language: zh-TW,zh;q=0.8,en-US;q=0.5,en;q=0.3\r\n");
    strcat(request, "Accept-Encoding: gzip, deflate\r\n");
    strcat(request, "DNT: 1\r\n");
    strcat(request, "Connection: keep-alive\r\n");
    strcat(request, "Referer: http://telehealth.foracare.com.tw/CareWork/CareWork/RealTimeMResult.aspx\r\n");
    strcat(request, "Upgrade-Insecure-Requests: 1\r\n");

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, CookieFmt, logininfo->ASP_DOT_NET_SessionId.c_str(), logininfo->DOT_ASPXAUTH.c_str());
    //snprintf(buffer, bufferLen, CookieFmt, "pucr53v0jbuvtdsey4oqiss3", "49CB1535381EC3EAE2D42B53A6CF7481EA0EDF2122650B3FAFC4324ACAD0A50AB4C1190B7901AE76F10987636F937EFDD528A8563566760E8FA80D0610BFD83CDA36599B887232B28094AC251FED2EED16C19536BD5E42BFCEDFF094822C28F895AAD52AE0D61657FC7F6472AFD44D8777DA2013B2936650D43F4050CE60FD1B23EFC693CDF550EBE56C51EF831027C8");
    strcat(request, buffer);

    //
    // prepare content
    //
    std::string sendMessage;
    sendMessage.append(UriEncode((const unsigned char *)"ctl00$ScriptManager1"));
    sendMessage.append("=");
    sendMessage.append(UriEncode((const unsigned char *)"ctl00$ContentPlaceHolder1$UpPanel_LastData|ctl00$ContentPlaceHolder1$ImgBut_Refresh"));
    //sendMessage.append(UriEncode((const unsigned char *)"ctl00$ScriptManager1|ctl00$ContentPlaceHolder1$Timer1"));

    // //sendMessage.append("&ctl00$ContentPlaceHolder1$DDL_QueryLastData=72"); // 74 hours
    //sendMessage.append("&ctl00$ContentPlaceHolder1$DDL_QueryLastData=24"); // 24 hours
    sendMessage.append("&");
    sendMessage.append(UriEncode((const unsigned char *)"ctl00$ContentPlaceHolder1$DDL_QueryLastData"));
    sendMessage.append("=24"); // 24 hours
    sendMessage.append("&__EVENTTARGET=");
    sendMessage.append(UriEncode((const unsigned char *)"ctl00$ContentPlaceHolder1$Timer1"));
    sendMessage.append("&__EVENTARGUMENT=");
    //sendMessage.append("&__EVENTARGUMENT=Page$3"); // Page$3
    sendMessage.append("&__VIEWSTATE=");
    sendMessage.append(UriEncode((const unsigned char *)logininfo->__VIEWSTATE.c_str()));
    sendMessage.append("&__VIEWSTATEGENERATOR=");
    sendMessage.append(UriEncode((const unsigned char *)logininfo->__VIEWSTATEGENERATOR.c_str()));
    // sendMessage.append("&__VIEWSTATEGENERATOR=8083930E");
    sendMessage.append("&__EVENTVALIDATION=");
    sendMessage.append(UriEncode((const unsigned char *)logininfo->__EVENTVALIDATION.c_str()));
    sendMessage.append("&ctl00$ContentPlaceHolder1$HidF_DateFormat=");
    sendMessage.append(UriEncode((const unsigned char *)"CY/MM/DD"));
    sendMessage.append("&ctl00$ContentPlaceHolder1$HidF_TimeFormat=24H");
    sendMessage.append("&ctl00$ContentPlaceHolder1$HidF_Lang=zh-TW");
    sendMessage.append("&ctl00$ContentPlaceHolder1$HidF_LoginGID=CH023");
    sendMessage.append("&ctl00$ContentPlaceHolder1$HidF_LoginPID=CH023");
    sendMessage.append("&ctl00$HidF_JStr_NumSymbol=.");
    sendMessage.append("&ctl00$HidF_JStr_WarningWords=");
    sendMessage.append("&ctl00$HidF_JStr_WarningURL=");
    sendMessage.append("&ctl00$HidF_ConnectErrorStatusCode=");
    sendMessage.append("&ctl00$HidF_SysTimeOutSec=1800");
    sendMessage.append("&ctl00$HidF_JSStr_DPTitle1=??????");
    sendMessage.append("&ctl00$HidF_JSStr_DPTitle2=???");
    sendMessage.append("&ctl00$HidF_JSStr_DPTitle3=???");
    sendMessage.append("&ctl00$HidF_JSStr_DTPLang=zh-TW");
    sendMessage.append("&ctl00$HidF_JSStr_Close=??????");
    sendMessage.append("&ctl00$HidF_JSStr_ROCYear=??????");
    sendMessage.append("&ctl00$HidF_JSStr_DTP_LastYear=??????");
    sendMessage.append("&ctl00$HidF_JSStr_DTP_LastMonth=??????");
    sendMessage.append("&ctl00$HidF_JSStr_DTP_NextYear=??????");
    sendMessage.append("&ctl00$HidF_JSStr_DTP_NextMonth=??????");
    sendMessage.append("&ctl00$HidF_JSStr_DTP_W0=???");
    sendMessage.append("&ctl00$HidF_JSStr_DTP_W1=???");
    sendMessage.append("&ctl00$HidF_JSStr_DTP_W2=???");
    sendMessage.append("&ctl00$HidF_JSStr_DTP_W3=???");
    sendMessage.append("&ctl00$HidF_JSStr_DTP_W4=???");
    sendMessage.append("&ctl00$HidF_JSStr_DTP_W5=???");
    sendMessage.append("&ctl00$HidF_JSStr_DTP_W6=???");
    sendMessage.append("&ctl00$HidF_JSStr_DTP_L=[");
    sendMessage.append("&ctl00$HidF_JSStr_DTP_R=]");
    sendMessage.append("&ctl00$HidF_JSStr_DTP_PlsKeyInYM=???????????????");
    sendMessage.append("&ctl00$HidF_JSStr_DTP_Hour=??????");
    sendMessage.append("&ctl00$HidF_JSStr_DTP_Minute=??????");
    sendMessage.append("&ctl00$HidF_JSStr_DTP_Back=??????");
    sendMessage.append("&ctl00$HidF_IconPath=");
    sendMessage.append(UriEncode((const unsigned char *)"../Image/Icon_Other/"));
    sendMessage.append("&__ASYNCPOST=true");
    sendMessage.append("&");

    // vilslog::printf("ForacareService::fetchData() sendMessage[%s]\n sendMessage.length()[%d]\n", sendMessage.c_str(), (int)sendMessage.length());

    strcat(request, ContentType);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, ContentLengthFmt, (int)sendMessage.length());
    strcat(request, buffer);

    strcat(request, sendMessage.c_str());

    strcat(request, CRLF);

    //vilslog::printf("request[%s]\n", request);

#ifdef DUMP_WEB_REQUEST
    vilslog::printf("-------------------------------------------\n");
    vilslog::printf("ForacareService::fetchData() request=[%s]\n", request);
    vilslog::printf("-------------------------------------------\n");
#endif

    //
    // sen out request
    //
    std::string http_response_string = sendRequest(host, port, request, logininfo);
    if ( http_response_string.length() == 0 )
    {
        vilslog::printf("ForacareService::fetchData()  failed!\n");
        return false;
    }

#ifdef DUMP_WEB_REPONSE
    vilslog::printf("-------------------------------------------\n");
    vilslog::printf("ForacareService::fetchData() http_response_string=[%s]\n", http_response_string.c_str());
    vilslog::printf("-------------------------------------------\n");
#endif
    // vilslog::printf("http_response_string[%s]\n", http_response_string.c_str());
    //
    // vilslog::printf("-----------------------------------------------------------\n");
    // vilslog::printf("Start parse....\n");

    tree<htmlcxx::HTML::Node> tr;
    htmlcxx::HTML::ParserDom parser;
    //parser.parse(content);
    //tr = parser.getTree();
    tree<htmlcxx::HTML::Node> dom = parser.parseTree(http_response_string, 0);
    //cout << tr << endl;

    //Dump all links in the tree
    tree<htmlcxx::HTML::Node>::iterator it = dom.begin();
    tree<htmlcxx::HTML::Node>::iterator end = dom.end();

    if (it == NULL || end == NULL )
    {
        vilslog::printf("ForacareService::fetchData() it[%x] end[%x]\n", it, end);
        return false;
    }

    bool bFoundTable = false;
    bool bFoundTableHeaderRow = false;
    bool bFoundTableHeaderCol = false;
    bool bFoundTableDataRow = false;
    bool bFoundTableDataCol = false;

    int colIndex = 0;

    std::list<std::string> tmpTableHeaderList;
    std::list<std::string> tmpTableDataList;
    std::list<std::string> tmpDataList;

    for (; it != end; ++it)
    {
        // cout << it->tagName() << endl;

        if (strcasecmp(it->tagName().c_str(), "table") == 0)
        {
            it->parseAttributes();

            if (strcasecmp(it->attribute("id").second.c_str(), "ContentPlaceHolder1_GV_LastData") == 0)
            {
                bFoundTable = true;
            }

            // cout << it->attribute("id").second << " : "<< it->attribute("value").second << endl;

            // std::string type = it->attribute("type").second;
            //
            // if ( type.compare("hidden") == 0 )
            // {
            //     cout << it->attribute("id").second << " : "<< it->attribute("value").second << endl;
            // }
        }
        else
        {
            if (bFoundTable)
            {
                if (bFoundTableDataRow)
                {
                    if (strcasecmp(it->tagName().c_str(), "tr") == 0)
                    {
                        it->parseAttributes();

                        std::string classType = it->attribute("class").second;
                        if ( classType.compare("S_GridView_Pager") == 0 )
                        {
                            // cout << "End Data Row" << endl;
                            bFoundTable = false;
                        }
                        else
                        {
                            // cout << "New Data Row" << endl;
                            colIndex = 0;

                            std::string dataString = convertListToString(&tmpDataList, ",");
                            tmpTableDataList.push_back(dataString);
                            tmpDataList.clear();
                        }
                    }
                    else
                    if (bFoundTableDataCol)
                    {
                        if ( it->isComment() )
                        {
                            // cout << "isComment" << endl;
                        }
                        else
                        if ( it->isTag() )
                        {
                            if (strcasecmp(it->tagName().c_str(), "a") == 0)
                            {
                                bFoundTableDataCol = true;
                                // cout << it->tagName() << endl;
                            }
                            else
                            if (strcasecmp(it->tagName().c_str(), "div") == 0)
                            {
                                bFoundTableDataCol = true;
                                // cout << it->tagName() << endl;
                            }
                            else
                            if (strcasecmp(it->tagName().c_str(), "span") == 0)
                            {
                                bFoundTableDataCol = true;
                                // cout << it->tagName() << endl;
                            }
                            else
                            {
                                colIndex++;
                                // cout <<"[" << colIndex <<"] " << endl;
                                tmpDataList.push_back("");
                            }
                        }
                        else
                        {
                            // cout <<"[" << colIndex <<"] " << it->tagName() << endl;
                            tmpDataList.push_back(it->tagName());
                            bFoundTableDataCol = false;
                        }
                    }
                    else
                    if (strcasecmp(it->tagName().c_str(), "td") == 0)
                    {
                        bFoundTableDataCol = true;
                        colIndex++;
                    }
                }
                else
                if (bFoundTableHeaderRow)
                {
                    if (strcasecmp(it->tagName().c_str(), "tr") == 0)
                    {
                        bFoundTableDataRow = true;
                        // cout << "End Header Row" << endl;
                        // cout << "New Data bRow" << endl;
                    }
                    else
                    if (bFoundTableHeaderCol)
                    {
                        if ( it->isComment() )
                        {
                            //cout << it->tagName() << endl;
                        }
                        else
                        if ( it->isTag() )
                        {
                            //cout << it->tagName() << endl;
                        }
                        else
                        {
                            // cout << it->tagName() << endl;
                            tmpTableHeaderList.push_back(it->tagName());
                            bFoundTableHeaderCol = false;
                        }
                    }
                    else
                    {
                        if (strcasecmp(it->tagName().c_str(), "th") == 0)
                        {
                            bFoundTableHeaderCol = true;
                        }
                    }
                }
                else
                {
                    if (strcasecmp(it->tagName().c_str(), "tr") == 0)
                    {
                        it->parseAttributes();

                        std::string classType = it->attribute("class").second;
                        if ( classType.compare("S_GridView_Header") == 0 )
                        {
                            bFoundTableHeaderRow = true;
                            // cout << "New Header Row" << endl;
                        }
                    }
                }
            }
        }

    }
    // vilslog::printf("-----------------------------------------------------------\n");
    //
    // vilslog::printf("ForacareService::fetchData() mTableHeaderList.size()[%d] mTableDataList.size()[%d]\n",
    //     (int)mTableHeaderList.size(), (int)mTableDataList.size());
    // vilslog::printf("ForacareService::fetchData() tmpTableHeaderList.size()[%d] tmpTableDataList.size()[%d]\n",
    //     (int)tmpTableHeaderList.size(), (int)tmpTableDataList.size());

    if ( (int)tmpTableHeaderList.size() > 0 )
        mTableHeaderList.clear();

    for (std::list<std::string>::iterator header = tmpTableHeaderList.begin(); header != tmpTableHeaderList.end(); header++)
    {
        //vilslog::printf("[%s] ", (*header).c_str());
        mTableHeaderList.push_back((*header));
    }

    if ( (int)tmpTableDataList.size() > 0 )
        mTableDataList.clear();

    for (std::list<std::string>::iterator data = tmpTableDataList.begin(); data != tmpTableDataList.end(); data++)
    {
        //vilslog::printf("%s\n", (*data).c_str());
        mTableDataList.push_back((*data));
    }

    tmpTableHeaderList.clear();
    tmpTableDataList.clear();
    tmpDataList.clear();

    bool ret = parseAJAX(http_response_string, logininfo);

    // vilslog::printf("ForacareService::fetchData() mTableHeaderList.size()[%d] mTableDataList.size()[%d] parseAJAX[%d]\n",
    //     (int)mTableHeaderList.size(), (int)mTableDataList.size(), ret);

    return ret;
}

int ForacareService::createConnectSocket(const char * host, const char * port)
{
    int cfd; // Socket ??????????????? (File Descriptor)
    int gaiStatus; // getaddrinfo ?????????
    struct addrinfo hints; // hints ??????????????? getaddrinfo() ???????????????
    struct addrinfo *result; // getaddrinfo() ??????????????? addrinfo ????????????

    // ??? memset ?????? hints ??????
    memset(&hints, 0, sizeof(struct addrinfo));
    //hints.ai_family = AF_UNSPEC; // ?????? IPv4 or IPv6
    hints.ai_family = AF_INET; // ?????? IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // ?????? Socket
    hints.ai_flags = AI_NUMERICSERV; // ??? getaddrinfo() ??? 2 ?????? (PORT_NUM) ????????????

    // ??? getaddrinfo ?????? DNS????????? addrinfo ???????????? (Linked List)
    // ??????????????? Host ??? IP ??????
    if ((gaiStatus = getaddrinfo(host, port, &hints, &result)) != 0)
    {
        errPrint((char *) gai_strerror(gaiStatus));
        return 0;
    }

    // ????????? domain, type, protocol ?????? socket ???????????????
    cfd = socket(result->ai_family, result->ai_socktype, 0);

    if ( cfd == -1 )
    {
        errPrint((char *) gai_strerror(gaiStatus));
        return 0;
    }

    // ??? socket ??????????????? (cfd), addr, addrlen ????????????
    // ?????????result->ai_addr ??? gai ????????? ?????? socket ???????????? -- sockaddr
    if (connect(cfd, result->ai_addr, result->ai_addrlen) < 0)
    {
        errPrint("Connect");
        // ?????? getaddrinfo (Linked List) ???????????????
        freeaddrinfo(result);
        return 0;
    }

    // ?????? getaddrinfo (Linked List) ???????????????
    freeaddrinfo(result);

    return cfd;
}

std::string ForacareService::sendRequest(const char * host, const char * port, const char * request, ForacareLoginStruct * logininfo)
{
    int cfd; // Socket ??????????????? (File Descriptor)
    int gaiStatus; // getaddrinfo ?????????
    struct addrinfo hints; // hints ??????????????? getaddrinfo() ???????????????
    struct addrinfo *result; // getaddrinfo() ??????????????? addrinfo ????????????
    std::string http_response_string = "";

    // ??? memset ?????? hints ??????
    memset(&hints, 0, sizeof(struct addrinfo));
    //hints.ai_family = AF_UNSPEC; // ?????? IPv4 or IPv6
    hints.ai_family = AF_INET; // ?????? IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // ?????? Socket
    hints.ai_flags = AI_NUMERICSERV; // ??? getaddrinfo() ??? 2 ?????? (PORT_NUM) ????????????

    // ??? getaddrinfo ?????? DNS????????? addrinfo ???????????? (Linked List)
    // ??????????????? Host ??? IP ??????
    // vilslog::printf("ForacareService::sendRequest() \n");
    if ((gaiStatus = getaddrinfo(host, port, &hints, &result)) != 0)
    {
        vilslog::printf("ForacareService::sendRequest() gaiStatus[%d]\n", gaiStatus);
        errPrint((char *) gai_strerror(gaiStatus));
        return http_response_string;
    }
    // vilslog::printf("ForacareService::sendRequest() gaiStatus[%d]\n", gaiStatus);


    // ????????? domain, type, protocol ?????? socket ???????????????
    cfd = socket(result->ai_family, result->ai_socktype, 0);

    if ( cfd == -1 )
    {
        errPrint((char *) gai_strerror(gaiStatus));
        return http_response_string;
    }

    // ??? socket ??????????????? (cfd), addr, addrlen ????????????
    // ?????????result->ai_addr ??? gai ????????? ?????? socket ???????????? -- sockaddr
    if (connect(cfd, result->ai_addr, result->ai_addrlen) < 0)
    {
        errPrint("Connect");
        return http_response_string;
    }


    // ?????? getaddrinfo (Linked List) ???????????????
    freeaddrinfo(result);
    result = NULL;

    // ???????????????????????????
    // printf("----------\nRequest:\n----------\n%s\n", request);

    // ????????????
    if (send(cfd, request, strlen(request), 0) <= 0)
    {
        errPrint("Send");
        return http_response_string;
    }

    // // ???????????????????????????
    // //printf("----------\nResponse:\n----------\n%s\n", response);
    // printf("----------\nResponse:\n----------\n");


    // ????????????
    size_t size = 4096;
    size_t total = 0, n = 0;
    char response[size]; // ????????????

    memset(response, 0, size);

    bool parse_header = true;
    bool chunked = false;
    int header_end_index = 0;
    int Content_Length = 0;
    std::list<HttpHeader*> * httpHeaderlist = NULL;

    // Transfer-Encoding: chunked\r\n
    // ?????????chunked, ????????????????????????????????????Content-Length (?????????????????????). ????????????????????????????????????chunked, ??????chunked?????????:
    // chunked byte???\r\n
    // ????????????\r\n
    // ????????????\r\n???. chunked byte???????????????16????????????

    while((n = recv(cfd, response, size, 0)) > 0)
    {
        //printf("total[%lu] n[%lu]\n", total, n);

        total += n;

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
                    // vilslog::printf ("%s:%s\n", (*header)->parameter.c_str(), (*header)->value.c_str());

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
                    else
                    if ((*header)->parameter.compare("Content-Length") == 0)
                    {
                        Content_Length = std::atoi(remove_nonASCII((*header)->value.c_str()).c_str());
                    }
                    else
                    if ((*header)->parameter.compare("Set-Cookie") == 0)
                    {
                        //printf("Set-Cookie:%s\n", (*header)->value.c_str());

                        std::string ASP_DOT_NET_SessionId = parse_Cookie((*header)->value, "ASP.NET_SessionId");
                        if ( ASP_DOT_NET_SessionId.length() > 0 )
                            logininfo->ASP_DOT_NET_SessionId = ASP_DOT_NET_SessionId;

                        std::string DOT_ASPXAUTH = parse_Cookie((*header)->value, ".ASPXAUTH");
                        if ( DOT_ASPXAUTH.length() > 0 )
                            logininfo->DOT_ASPXAUTH = DOT_ASPXAUTH;
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

        if (n >= 4)
        {
            //
            // find http end \r\n\r\n
            //
            if (response[n-4] == '\r' && response[n-3] == '\n' &&response[n-2] == '\r' &&response[n-1] == '\n')
            {
                // vilslog::printf ("Found Http end!!!\n");
                break;
            }
        }

        memset(response, 0, size);

        if ( (int)total >= Content_Length )
        {
            //printf("total[%lu] Content_Length[%d]\n", total, Content_Length);

            break;
        }
    }

    //vilslog::printf("http_response_string.length[%d] content[%s]\n", http_response_string.length(), http_response_string.c_str());
    //vilslog::printf("http_response_string.length[%d]\n", http_response_string.length());

    if (httpHeaderlist != NULL)
    {
        clear_HttpHeader_list(httpHeaderlist);
        delete httpHeaderlist;
    }

    // ??????????????? TCP Socket ??????
    // (i.e., ????????????)
    shutdown(cfd, SHUT_WR);
    close(cfd);
    return http_response_string;
}

std::string ForacareService::parse_Cookie(std::string value, const char * pattern)
{
    //bool bFound = false;
    std::string return_string = "";

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

            if ( head.compare(pattern) == 0 )
            {
                std::string newvalue = remove_newline(value);
                //vilslog::printf("ForacareService::parse_Cookie() pattern[%s] value[%s]\n", head.c_str(), newvalue.c_str());
                return_string = newvalue;
                //bFound = true;
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

        if ( head.compare(pattern) == 0 )
        {
            std::string newvalue = remove_newline(value);
            //vilslog::printf("ForacareService::parse_Cookie() pattern[%s] value[%s]\n", head.c_str(), newvalue.c_str());
            return_string = newvalue;
            //bFound = true;
        }
    }

    return return_string;
}

std::list<HttpHeader*> * ForacareService::parse_http_header(const char * response, size_t n)
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

        // pair ????????????: ?????????\r\n
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

int ForacareService::get_line(const char * databuffer, int data_len, int start_idx, char * linebuffer, int buffer_len)
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

void ForacareService::errExit(const char *reason)
{
    std::string buff = reason ? reason : strerror(errno);
    printf("Error: %s", buff.c_str());
    exit(EXIT_FAILURE);
}

void ForacareService::errPrint(const char *reason)
{
    std::string buff = reason ? reason : strerror(errno);
    vilslog::printf("ForacareService::errPrint() Error: %s\n", buff.c_str());
}

bool ForacareService::XMLTest (const char* testString, const char* expected, const char* found, bool echo, bool extraNL )
{
	bool pass;
	if ( !expected && !found )
		pass = true;
	else if ( !expected || !found )
		pass = false;
	else
		pass = !strcmp( expected, found );
	if ( pass )
		vilslog::printf ("[pass]");
	else
		vilslog::printf ("[fail]");

	if ( !echo ) {
		vilslog::printf (" %s\n", testString);
	}
	else {
		if ( extraNL ) {
			vilslog::printf( " %s\n", testString );
			vilslog::printf( "%s\n", expected );
			vilslog::printf( "%s\n", found );
		}
		else {
			vilslog::printf (" %s [%s][%s]\n", testString, expected, found);
		}
	}

	// if ( pass )
	// 	++gPass;
	// else
	// 	++gFail;
	return pass;
}

bool ForacareService::XMLTest(const char* testString, bool expected, bool found)
{
    bool echo = true;
    bool extraNL = false;
    return XMLTest(testString, expected ? "true" : "false", found ? "true" : "false", echo, extraNL);
}

bool ForacareService::NewAccount()
{
    std::string IDataNewAccount;
    IDataNewAccount.append("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    IDataNewAccount.append("<AddNewData>");
    IDataNewAccount.append("<Account>iforas</Account>");
    IDataNewAccount.append("<Password>iforas123</Password>");
    IDataNewAccount.append("<ACase>");
    IDataNewAccount.append("<Case>");
    IDataNewAccount.append("<PID>TD123</PID>");
    IDataNewAccount.append("<PAcct>john</PAcct>");
    IDataNewAccount.append("<PPwd>john123</PPwd>");
    IDataNewAccount.append("<PTZ></PTZ>");
    IDataNewAccount.append("<PName></PName>");
    IDataNewAccount.append("<PFName>john</PFName>");
    IDataNewAccount.append("<PLName>john</PLName>");
    IDataNewAccount.append("<PBirthday>2020/01/12</PBirthday>");
    IDataNewAccount.append("<PSex>mail</PSex>");
    IDataNewAccount.append("<PHeight>100</PHeight>");
    IDataNewAccount.append("<PWeight></PWeight>");
    IDataNewAccount.append("<PTel1></PTel1>");
    IDataNewAccount.append("<PTel2></PTel2>");
    IDataNewAccount.append("<PEMail>g123@mail.com</PEMail>");
    IDataNewAccount.append("<PTGetUp>06:00</PTGetUp>");
    IDataNewAccount.append("<PTBreakfast>07:00</PTBreakfast>");
    IDataNewAccount.append("<PTLunch>12:00</PTLunch>");
    IDataNewAccount.append("<PTDinner>17:00</PTDinner>");
    IDataNewAccount.append("<PTSleep>22:00</PTSleep>");
    IDataNewAccount.append("<PAddr></PAddr>");
    IDataNewAccount.append("<PRaceNo RaceNote=\"\"></PRaceNo>");
    IDataNewAccount.append("</Case>");
    IDataNewAccount.append("</ACase>");
    IDataNewAccount.append("</AddNewData>");

    postService("A0001", IDataNewAccount, "iforasmartgm", "iforasmartgm123"); // ????????????????????????????????????????????????
    //postService("A0001", IDataNewAccount, "studioatw", "studioatw123"); // ????????????????????????????????????????????????
    //postService("A0001", IDataNewAccount, "ifora", "ifora"); // ????????????????????????????????????????????????
    //postService("A0001", IDataNewAccount, "iforasmartap", "iforasmartap123"); // ????????????????????????????????????????????????
    return false;
}

bool ForacareService::AddMeter()
{
    std::string MeterData;
    MeterData.append("<Meter>");
    MeterData.append("<MeterType>TD4272</MeterType>");
    MeterData.append("<MeterExID>User 1</MeterExID>");
    MeterData.append("<MeterID>4272215370000214</MeterID>");
    MeterData.append("<GatewayID>1122334455667788</GatewayID>");
    MeterData.append("</Meter>");

    std::string IDataAddMeter;
    IDataAddMeter.append("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    IDataAddMeter.append("<AddNewData>");
    IDataAddMeter.append("<Account>studioatw</Account>");
    IDataAddMeter.append("<Password>studioatw123</Password>");
    IDataAddMeter.append("<ACase>");
    IDataAddMeter.append("<CaseMeterData PID=\"0000366\">");
    IDataAddMeter.append(MeterData);
    IDataAddMeter.append("</CaseMeterData>");
    IDataAddMeter.append("/ACase>");
    IDataAddMeter.append("</AddNewData>");

    postService("A0002", IDataAddMeter, "ifora", "ifora"); // XML ??????????????????????????? Tag???
    //postService("A0002", IDataAddMeter, "studioatw", "studioatw123"); // ????????????????????????????????????????????????
    return false;
}

bool ForacareService::QueryPatient()
{
    std::string IDataQueryPatient;
    IDataQueryPatient.append("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    IDataQueryPatient.append("<QueryData>");
    IDataQueryPatient.append("<Account>studioatw</Account>");
    IDataQueryPatient.append("<Password>studioatw123</Password>");
    IDataQueryPatient.append("<QAcct>John</QAcct>");
    IDataQueryPatient.append("<QPwd>ad12345678</QPwd>");
    IDataQueryPatient.append("</QueryData>");

    postService("Q0005", IDataQueryPatient, "studioatw", "studioatw123");
    return false;
}

bool ForacareService::VerifyPatient()
{
    std::string IDataVerifyPatient;
    IDataVerifyPatient.append("<?xml version=\"1.0\" encoding=\"utf-8\"?>");
    IDataVerifyPatient.append("<QueryData>");
    IDataVerifyPatient.append("<Account>iforasmartgm</Account>");
    IDataVerifyPatient.append("<Password>iforasmartgm123</Password>");
    IDataVerifyPatient.append("</QueryData>");

    //postService("Q0009", IDataVerifyPatient, "iforasmartap", "iforasmartap123");
    //postService("Q0009", IDataVerifyPatient, "studioatw", "studioatw123");
    //postService("Q0009", IDataVerifyPatient, "iforasmartgm", "iforasmartgm123");
    postService("Q0009", IDataVerifyPatient, "ifora", "ifora"); // ???????????????????????????????????????????????????
    return false;
}

bool ForacareService::postService(std::string FuncID, std::string CDATA, std::string user, std::string passwd)
{
    std::string WebServiceRegister;
    WebServiceRegister.append("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
    WebServiceRegister.append("<soap:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" ");
    WebServiceRegister.append("xmlns:soap=\"http://www.w3.org/2003/05/soap-envelope\">\n");
    WebServiceRegister.append("<soap:Header>\n");
    WebServiceRegister.append("<sValidationSoapHeader xmlns=\"http://www.tdcare.com/\">\n");
    WebServiceRegister.append("<Username>");
    WebServiceRegister.append(user);
    WebServiceRegister.append("</Username>\n");
    WebServiceRegister.append("<Password>");
    WebServiceRegister.append(passwd);
    WebServiceRegister.append("</Password>\n");
    WebServiceRegister.append("</sValidationSoapHeader>\n</soap:Header>\n");
    WebServiceRegister.append("<soap:Body>\n");
    WebServiceRegister.append("<DataInterchange xmlns=\"http://www.tdcare.com/\">\n");
    WebServiceRegister.append("<iCMDType>");
    WebServiceRegister.append(FuncID);
    WebServiceRegister.append("</iCMDType>\n");
    //WebServiceRegister.append("<iCMDType>Q0009</iCMDType>\n");
    //WebServiceRegister.append("<iData><![CDATA[%s]]></iData>\n");
    WebServiceRegister.append("<iData><![CDATA[");
    WebServiceRegister.append(CDATA);
    WebServiceRegister.append("]]></iData>\n");
    WebServiceRegister.append("</DataInterchange>\n");
    WebServiceRegister.append("</soap:Body>\n");
    WebServiceRegister.append("</soap:Envelope>");

    std::string TELEHEALTH_URL = "telehealth.foracare.com.tw";
    //std::string TELEHEALTH_URL = "care.99jkom.com";
    std::string METHOD_NAME = "DataInterchange";
    std::string mContent = WebServiceRegister;

    send_POST_cmd(TELEHEALTH_URL, METHOD_NAME, mContent);

    return false;
}

bool ForacareService::postServiceSOP12(std::string FuncID, std::string CDATA, std::string user, std::string passwd)
{
    std::string WebServiceRegister;
    WebServiceRegister.append("<?xml version=\"1.0\" encoding=\"utf-8\"?>\n");
    WebServiceRegister.append("<soap12:Envelope xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" ");
    WebServiceRegister.append("xmlns:soap12=\"http://www.w3.org/2003/05/soap-envelope\">\n");
    WebServiceRegister.append("<soap12:Header>\n");
    WebServiceRegister.append("<sValidationSoapHeader xmlns=\"http://www.tdcare.com/\">\n");
    WebServiceRegister.append("<Username>");
    WebServiceRegister.append(user);
    WebServiceRegister.append("</Username>\n");
    WebServiceRegister.append("<Password>");
    WebServiceRegister.append(passwd);
    WebServiceRegister.append("</Password>\n");
    WebServiceRegister.append("</sValidationSoapHeader>\n</soap12:Header>\n");
    WebServiceRegister.append("<soap12:Body>\n");
    WebServiceRegister.append("<DataInterchange xmlns=\"http://www.tdcare.com/\">\n");
    WebServiceRegister.append("<iCMDType>");
    WebServiceRegister.append(FuncID);
    WebServiceRegister.append("</iCMDType>\n");
    //WebServiceRegister.append("<iCMDType>Q0009</iCMDType>\n");
    //WebServiceRegister.append("<iData><![CDATA[%s]]></iData>\n");
    WebServiceRegister.append("<iData><![CDATA[");
    WebServiceRegister.append(CDATA);
    WebServiceRegister.append("]]></iData>\n");
    WebServiceRegister.append("</DataInterchange>\n");
    WebServiceRegister.append("</soap12:Body>\n");
    WebServiceRegister.append("</soap12:Envelope>");

    std::string TELEHEALTH_URL = "telehealth.foracare.com.tw";
    //std::string TELEHEALTH_URL = "care.99jkom.com";
    std::string METHOD_NAME = "DataInterchange";
    std::string mContent = WebServiceRegister;

    send_POST_cmd(TELEHEALTH_URL, METHOD_NAME, mContent);

    return false;
}

bool ForacareService::send_POST_cmd(std::string TELEHEALTH_URL, std::string METHOD_NAME, std::string mContent)
{
    const char *host = TELEHEALTH_URL.c_str(); // ?????? URI
    const char *command = METHOD_NAME.c_str(); // HTTP port
    const char *PORT_NUM = "80"; // HTTP port

    char request[4096];
    memset(request, 0, 4096);

    const char *requestLineFmt = "POST /WebService/WS_DataInterchangeService.asmx?op=%s HTTP/1.1\r\n"; // ?????????
    const char *headerFmt = "Host: %s\r\n"; // Host ????????????
    const char *ContentType = "Content-Type: application/soap+xml; charset=utf-8\r\n";
    const char *ContentLengthFmt = "Content-Length: %d\r\nSOAPAction: \"http://www.tdcare.com/DataInterchange\"\r\n\r\n";
    const char *MessageFmt = "%s";
    //const char *CRLF = "\r\n";  // ???????????? CRLF

    char buffer[4096];
    int bufferLen = 4096;
    memset(buffer, 0, bufferLen);

    //??????????????????
    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, requestLineFmt, command);
    strcpy(request, buffer);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, headerFmt, host);
    strcat(request, buffer);

    strcat(request, ContentType);

    char sendmessage[4096];
    memset(sendmessage, 0, 4096);
    snprintf(sendmessage, 4096, MessageFmt, mContent.c_str());

    memset(buffer, 0, 4096);
    snprintf(buffer, bufferLen, ContentLengthFmt, strlen(sendmessage));
    strcat(request, buffer);

    strcat(request, sendmessage);

    //strcat(request, CRLF);

    //vilslog::printf("ForacareService::send_POST_cmd() request[%s]", request);

    //
    // sen out request
    //
    //int fd = createConnectSocket(host, PORT_NUM);

    ForacareLoginStruct logininfo;
    std::string http_response_string = sendRequest(host, PORT_NUM, request, &logininfo);
    if ( http_response_string.length() == 0 )
    {
        vilslog::printf("send_POST_cmd() failed!\n");
        return false;
    }

    return false;
}

bool ForacareService::parseAJAX(std::string http_response_string, ForacareLoginStruct * logininfo)
{
    std::vector<std::string> * itemList = new std::vector<std::string>();

    istringstream f(http_response_string.c_str());

    string s;
    while (getline(f, s, '|'))
    {
        itemList->push_back(s);
    }

    int total_length = (int)itemList->size();

    //vilslog::printf("total_length[%d]\n", total_length);
    int iFound = 0;
    int index = 0;
    while ((index + 4) < total_length)
    {
        // find item length
        std::string dataLen = itemList->at(index++);
        std::string dataFeature = itemList->at(index++);
        std::string dataName = itemList->at(index++);
        std::string dataValue = itemList->at(index++);

        if ( dataName.compare("__VIEWSTATE") == 0 )
        {
            logininfo->__VIEWSTATE = dataValue;
            iFound++;
            // vilslog::printf("%s|%s|%s|%s\n", dataLen.c_str(), dataFeature.c_str(), dataName.c_str(), dataValue.c_str());
        }
        else
        if ( dataName.compare("__VIEWSTATEGENERATOR") == 0 )
        {
            logininfo->__VIEWSTATEGENERATOR = dataValue;
            iFound++;
            // vilslog::printf("%s|%s|%s|%s\n", dataLen.c_str(), dataFeature.c_str(), dataName.c_str(), dataValue.c_str());
        }
        else
        if ( dataName.compare("__EVENTVALIDATION") == 0 )
        {
            logininfo->__EVENTVALIDATION = dataValue;
            iFound++;
            // vilslog::printf("%s|%s|%s|%s\n", dataLen.c_str(), dataFeature.c_str(), dataName.c_str(), dataValue.c_str());
        }

    }

    itemList->clear();
    delete itemList;

    if (iFound == 0)
        return false;
    else
        return true;
}
