/*
 * LoRAGateway.h
 *
 *  Created on: 2020/02/15
 *      Author: ikki
 */
#ifndef __LORAGATEWAY_h__
#define __LORAGATEWAY_h__

#include <stdio.h>
#include <stdlib.h>
#include <string>

#include "../common.h"

using namespace std;

class LoRAGateway
{
private:
    std::string mGatewayIP;

public:
    LoRAGateway(const char * ip);

    bool resetLoRA();
    bool getPage();

    bool getSysauthToken(const char * host, const char * port, const char * request, char * sysauth, char * token);
    bool login_and_get_SysauthToken(char * sysauth, char * token);
    bool send_reset_cmd(const char * function, char * sysauth, char * token);
    bool sendRequest(const char * host, const char * port, const char * request);
    bool parse_SysauthToken(std::string value, char * sysauth, char * token);

    std::list<HttpHeader*> * parse_http_header(const char * response, size_t n);
    int get_line(const char * databuffer, int data_len, int start_idx, char * linebuffer, int buffer_len);
    void errExit(const char *reason);
    void errPrint(const char *reason);

};
#endif
