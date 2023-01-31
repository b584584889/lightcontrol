/*
* UDPDataItem.cpp
*
*  Created on: 2019/04/15
 *      Author: ikki
 */
 #include <stdio.h>
 #include <stdlib.h>
 #include "string.h"

#include "UDPDataItem.h"

UDPDataItem::UDPDataItem(const char* type)
{
    init();
    strncpy(m_type, type, 64);
}

UDPDataItem::UDPDataItem(const char* type, const char* buf, int length, const char* clientIP)
{
    init();

    strncpy(m_type, type, 64);
    strncpy(m_buf, buf, MAXUDPPACKET);
    m_length = length;
    strncpy(m_clientIP, clientIP, 64);
}

void UDPDataItem::init()
{
    memset(m_type, 0, 64);
    memset(m_buf, 0, MAXUDPPACKET);
    memset(m_clientIP, 0, 64);
    m_length = 0;
}

void UDPDataItem::setMsg(const char * msg, int length)
{
    strncpy(m_buf, msg, MAXUDPPACKET);
    m_length = length;
}

void UDPDataItem::setProjectid(int projectid)
{
    m_projectid = projectid;
}

char * UDPDataItem::getMsg()
{
    return m_buf;
}

char * UDPDataItem::getIPAddress()
{
    return m_clientIP;
}

int UDPDataItem::getMsgLength()
{
    return m_length;
}

int UDPDataItem::getProjectid()
{
    return m_projectid;
}
