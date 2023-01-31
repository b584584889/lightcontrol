/*
 * JSONAgent.h
 *
 *  Created on: 2017/07/05
 *      Author: ikki
 */

#ifndef __JSON_AGENT_H__
#define __JSON_AGENT_H__

#include "../json/json.h"
#include "../common.h"


#define JSON_BUF_MAX 12000
#define JSON_LINE_MAX 3000



class JSONAgent
{
public:
    JSONAgent();
    ~JSONAgent();
    int put(const char * content, int length);
    bool get(Json::Value & JSONresult);
    int getBufferLen();
private:
    int mReceiveBufLen;
    char mReceiveBuffer[JSON_BUF_MAX];
    char line[JSON_LINE_MAX];

    Json::Reader * mReader;

    int get_line(char * linebuffer, int start_idx);
};


#endif
