/*
 *  LLRPController.h
 *
 *  Created on: 2020/05/04
 *      Author: ikki
 */
#ifndef __LLRPController_h__
#define __LLRPController_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "../common.h"
#include "../config.h"
#include "ltkcpp.h"

using namespace std;

typedef struct
{
    std::string isAdmin;
    std::string isFullSupport;
    std::string isReadTagsPage;
    std::string RMSessionID;
}ZebraCookieStruct;


class LLRPController
{
public:
    LLRPController();
    ~LLRPController();

    int run(const char * pReaderHostName, int nDurationMS, int nMaxMS);

    int prepareReader(const char * pReaderHostName, int nDurationMS);
    std::list<RFIDCardStruct*> * readReader(int nMaxMS);
    int closeReader();
    bool isReady();
    bool isSameHost(std::string ReaderHostName);
    int disconnectLLRP(const char * pReaderHostName, std::string username, std::string password);

private:
    //LLRP::CConnection * m_pConnectionToReader;

    std::string m_ReaderHostName;
    bool mInitial;
    LLRP::CConnection * m_pConnectionToReader;
    LLRP::CTypeRegistry * m_TypeRegistry;

    int checkConnectionStatus(void);
    int scrubConfiguration(void);
    int resetConfigurationToFactoryDefaults(void);
    int deleteAllROSpecs(void);
    int addROSpec(int nDurationMS);
    int enableROSpec(void);
    int startROSpec(void);
    int awaitAndPrintReport(int nMaxMS);
    std::list<RFIDCardStruct*> * awaitAndGetCardList(int nMaxMS);
    std::list<RFIDCardStruct*> * getTagReportRFIDCard(LLRP::CRO_ACCESS_REPORT * pRO_ACCESS_REPORT);
    std::string getOneTagReportId(LLRP::CTagReportData * pTagReportData);

    void printTagReportData(LLRP::CRO_ACCESS_REPORT * pRO_ACCESS_REPORT);
    void printOneTagReportData(LLRP::CTagReportData * pTagReportData);
    void handleReaderEventNotification(LLRP::CReaderEventNotificationData * pNtfData);
    void handleAntennaEvent(LLRP::CAntennaEvent * pAntennaEvent);
    void handleReaderExceptionEvent(LLRP::CReaderExceptionEvent * pReaderExceptionEvent);
    int checkLLRPStatus(LLRP::CLLRPStatus * pLLRPStatus, const char * pWhatStr);
    LLRP::CMessage * transact(LLRP::CMessage * pSendMsg);
    LLRP::CMessage * recvMessage(int nMaxMS);
    int sendMessage(LLRP::CMessage * pSendMsg);
    void printXMLMessage(LLRP::CMessage * pMessage);

    ZebraCookieStruct * login_and_get_cookie(const char * pReaderHostName, std::string username, std::string password);
    bool get_cookie(const char * host, const char * port, const char * request, ZebraCookieStruct * cookie);
    bool send_disconnect_request(const char * pReaderHostName, ZebraCookieStruct * cookie);
    bool logout(const char * pReaderHostName, ZebraCookieStruct * cookie);
    std::string sendRequest(const char * host, const char * port, const char * request);
    std::list<HttpHeader*> * parse_http_header(const char * response, size_t n);
    int get_line(const char * databuffer, int data_len, int start_idx, char * linebuffer, int buffer_len);
    void errPrint(const char *reason);
};

#endif
