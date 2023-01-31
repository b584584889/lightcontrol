/*
 *  WebUDPAPIService_DMATEK.h
 *
 *  Created on: 2018/10/18
 *      Author: ikki
 */
#ifndef __WebUDPAPIService_DMATEK_h__
#define __WebUDPAPIService_DMATEK_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "Thread.h"
#include "../network/wqueue.h"
#include "../network/DataItem.h"
#include "../network/UDPDataItem.h"
#include "../network/UDPServer.h"
#include "../mysql/DBManager.h"
#include "../config.h"

using namespace std;

//Anchor
#define _ANCHORID 0
#define _ANCHOR_FIRMWARE_TYPE 1
#define _ANCHOR_FIRMWARE_VERSION 2
#define _ANCHOR_HARDWARE_VERSION 3
#define _ANCHOR_REPORT_ID_INTERVAL 4
#define _ANCHOR_TAG_BIN_UPDATE_EN 5
#define _ANCHOR_TAG_PARM_UPDATE_EN 6
#define _ANCHOR_HEIGHT 7
#define _ANCHOR_TEMP 8
#define _ANCHOR_TX_POWER 9

//Tag
#define _TAGID 40
#define _TAG_FW_TYPE 41
#define _TAG_FW_VERSION 42
#define _TAG_HARDWARE_VERSION 43
#define _TAG_BATTERY 44
#define _TAG_GSENSOR_TIME 45
#define _GSENSORX 46
#define _GSENSORY 47
#define _GSENSORZ 48
#define _TAG_HEIGHT 49
#define _HEART_RATE 50
#define _TAG_SLEEP_TIME 51
#define _TAG_STATIC_SLEEP_TIME 52
#define _SENSITIVITY 53
#define _AREA_SEARCH_EN 54
#define _ATEA_SEARCH_TX_POWER 55
#define _LED1_CONTROL 56
#define _BUZZER_CONTROL 57
#define _TAG_TX_POWER 58
#define _TAG_MOVE_DURATION 59
#define _TAG_LOCATE_RESET_EN 60
#define _MOTOR_CONTROL 61
#define _HEART_RATE_DETECT_TIME 62
#define _HEART_RATE_STATIC_DETECT_TIME 63

//Other
#define _SERIALNUM 100
#define _LOCATE_TYEP 101
#define _LOCATE_ANCHORID1 102
#define _LOCATE_DISTANCE1 103
#define _LOCATE_ANCHOR_HEIGHT1 104
#define _LOCATE_ANCHORID2 105
#define _LOCATE_DISTANCE2 106
#define _LOCATE_ANCHOR_HEIGHT2 107
#define _LOCATE_ANCHORID3 108
#define _LOCATE_DISTANCE3 109
#define _LOCATE_ANCHOR_HEIGHT3 110
#define _LOCATE_ANCHORID4 111
#define _LOCATE_DISTANCE4 112
#define _LOCATE_ANCHOR_HEIGHT4 113
#define _LOCATE_ANCHORID5 114
#define _LOCATE_DISTANCE5 115
#define _LOCATE_ANCHOR_HEIGHT5 116
#define _LOCATE_ANCHORID6 117
#define _LOCATE_DISTANCE6 118
#define _LOCATE_ANCHOR_HEIGHT6 119
#define _TAG_ON_LINE_SET_STATUS 120

#define _ENDCODE 0xff

typedef struct{
    uint8_t anchorid[3];
    uint8_t serialnum;
    uint8_t anchorReportIdInv[2];
    uint8_t anchorFwType[4];
    uint8_t anchorFwVersion[4];
    uint8_t anchorHwVersion;
    uint8_t tagBinUpdateEn;
    uint8_t tagParmUpdateEn;
}AnchorReportId;

typedef struct{
    uint8_t anchorid[3];
    uint8_t tagid[3];
    uint8_t serialNum;
    uint8_t locateType;
    uint8_t tagBattery;
    uint8_t tagGSensorTime[2];
    uint8_t tagSleepTime[2];
    uint8_t gsensorX[2];
    uint8_t gsensorY[2];
    uint8_t gsensorZ[2];
    uint8_t tagHeight[3];
    uint8_t tagFwType[4];
    uint8_t tagFwVersion[4];
    uint8_t tagHwVersion;
    bool anchor1Exist = false;
    uint8_t locateAnchorId1[3];
    uint8_t locateAnchorDis1[2];
    uint8_t locateAnchorHeight1[3];
    bool anchor2Exist = false;
    uint8_t locateAnchorId2[3];
    uint8_t locateAnchorDis2[2];
    uint8_t locateAnchorHeight2[3];
    bool anchor3Exist = false;
    uint8_t locateAnchorId3[3];
    uint8_t locateAnchorDis3[2];
    uint8_t locateAnchorHeight3[3];
    bool anchor4Exist = false;
    uint8_t locateAnchorId4[3];
    uint8_t locateAnchorDis4[2];
    uint8_t locateAnchorHeight4[3];
    bool anchor5Exist = false;
    uint8_t locateAnchorId5[3];
    uint8_t locateAnchorDis5[2];
    uint8_t locateAnchorHeight5[3];
    uint8_t heartRate[3];
}TagReport;

typedef struct{
    int role;   // 0 = anchor, 1 = tag
    TagReport tagreport;
    AnchorReportId anchorreportid;
}ReportInf;

class WebUDPAPIService_DMATEK : public Thread
{
public:
    WebUDPAPIService_DMATEK(wqueue<UDPDataItem*> & udpqueue, wqueue<DataItem*> & WebDMATEKJobQueue);
    bool status();
    std::string statusString();
    ReportInf *decodeDMATEK_UDP(const char *buf, int size);
    AnchorReportId *analyAnchorReport(uint8_t *data, int dataSize);
    TagReport *analyTagReport(uint8_t *data, int dataSize);
    bool checkCRC(uint8_t *data, int size);

    void * run();
private:
    bool mUdpInitial;
    UDPServer * mWebUDPServer;
    std::string mStatus;

    wqueue<UDPDataItem*> & m_udpqueue;
    wqueue<DataItem*> & m_WebDMATEKJobQueue;

    void convert2hex(char* _buffer, unsigned int data);

    void runloop();
    void waitloop();
    int parseData( const char * clientIP, const char * buf, int length);
    int send_anchor_join(std::string anchorid_s, const char * clientIP);
};


#endif
