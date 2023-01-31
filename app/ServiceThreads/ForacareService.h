/*
 *  ForacareService.h
 *
 *  Created on: 2020/02/17
 *      Author: ikki
 */
#ifndef __ForacareService_h__
#define __ForacareService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "Thread.h"
#include "../network/WorkItem.h"

using namespace std;

// #define DUMP_WEB_REQUEST
// #define DUMP_WEB_REPONSE

typedef struct
{
    std::string __VIEWSTATE;
    std::string __VIEWSTATEGENERATOR;
    std::string __EVENTVALIDATION;
    std::string HidF_AutoID;
    std::string HidF_QMType;
    std::string HidF_PageType;
    std::string HidF_LoginDestinationUrl;

    std::string ASP_DOT_NET_SessionId;
    std::string DOT_ASPXAUTH;

} ForacareLoginStruct;

class ForacareService : public Thread
{
public:
    ForacareService(wqueue<WorkItem*> & queue);
    ~ForacareService();
    bool status();
    std::string statusString();

    void * run();
    std::string getForacareData();

private:

    std::string mStatus;
    bool mInitial;

    wqueue<WorkItem*> & m_queue;

    ForacareLoginStruct * mLogininfo;
    std::list<std::string> mTableHeaderList;
    std::list<std::string> mTableDataList;
    time_t mDataUpdateTime;

    void runloop();
    void updateInfo();
    int createConnectSocket(const char * host, const char * port);
    bool fetchMainPage(const char * host, const char * port, ForacareLoginStruct * logininfo);
    bool login(const char * host, const char * port, ForacareLoginStruct * logininfo);
    bool fetchPageData(const char * host, const char * port, ForacareLoginStruct * logininfo);
    bool fetchData(const char * host, const char * port, ForacareLoginStruct * logininfo);

    bool NewAccount();
    bool AddMeter();
    bool QueryPatient();
    bool VerifyPatient();
    bool postService(std::string FuncID, std::string CDATA, std::string user, std::string passwd);
    bool postServiceSOP12(std::string FuncID, std::string CDATA, std::string user, std::string passwd);

    std::string sendRequest(const char * host, const char * port, const char * request, ForacareLoginStruct * logininfo);
    bool send_POST_cmd(std::string TEMP_TELEHEALTH_URL, std::string METHOD_NAME, std::string mContent);

    std::string parse_Cookie(std::string value, const char * pattern);
    std::list<HttpHeader*> * parse_http_header(const char * response, size_t n);
    int get_line(const char * databuffer, int data_len, int start_idx, char * linebuffer, int buffer_len);
    void errExit(const char *reason);
    void errPrint(const char *reason);
    bool XMLTest (const char* testString, const char* expected, const char* found, bool echo, bool extraNL);
    bool XMLTest(const char* testString, bool expected, bool found);
    bool parseAJAX(std::string http_response_string, ForacareLoginStruct * logininfo);

};
#endif
