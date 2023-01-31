/*
 * UDPDataItem.h
 *
 *  Created on: 2019/04/15
 *      Author: ikki
 */
#ifndef __UDPDataItem_h__
#define __UDPDataItem_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "../common.h"
#include "UDPServer.h"

using namespace std;

class UDPDataItem
{
private:
    char m_type[64];
    char m_buf[MAXUDPPACKET];
    int m_length;
    char m_clientIP[64];
    int m_projectid;

    void init();

public:
    UDPDataItem(const char* type);
    UDPDataItem(const char* type, const char* buf, int buf_length, const char* clientIP);

    char * getMsg();
    int getMsgLength();
    char * getIPAddress();
    int getProjectid();

    void setMsg(const char * msg, int length);
    void setProjectid(int projectid);

    UDPServer * mUDPServer;
    struct sockaddr_in m_client_addr; // address of client
};

#endif
