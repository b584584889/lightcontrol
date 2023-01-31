/*
 *  CHTIoTService.h
 *
 *  Created on: 2020/05/07
 *      Author: ikki
 */
#ifndef __CHTIoTService_h__
#define __CHTIoTService_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../common.h"
#include "../config.h"
#include "../network/DataItem.h"
#include "../network/WorkItem.h"
#include "../mysql/DBManager.h"
#include "../DeviceManager/DeviceManager.h"

using namespace std;

class CHTIoTService : public Thread
{
public:
    CHTIoTService(wqueue<DataItem*> & CHTIoTJobQueue);
    ~CHTIoTService();
    bool status();
    std::string statusString();

    void * run();

private:
    bool mInitial;
    std::string mStatus;

    wqueue<DataItem*> & mCHTIoTJobQueue;

    void runloop();
    int processDataItem(DataItem * dataItem);

    bool upload_cht_iot(std::string DeviceID, std::string DeviceKey, std::string SensorID, std::string dataValue);
    bool upload_cht_iot_heartbeat(std::string DeviceID, std::string DeviceKey, std::string pulseValue);

    std::string sendRequest(const char * host, const char * port, const char * request);

    std::list<HttpHeader*> * parse_http_header(const char * response, size_t n);
    int get_line(const char * databuffer, int data_len, int start_idx, char * linebuffer, int buffer_len);
    void errExit(const char * reason);
    void errPrint(const char *reason);
};

#endif
