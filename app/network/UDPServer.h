/*
 * UDPServer.h
 *
 *  Created on: 2017/02/03
 *      Author: ikki
 */
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#ifndef __UDP_SERVER_H__
#define __UDP_SERVER_H__

#include "../common.h"

class UDPServer
{
public:
    UDPServer();
    ~UDPServer();
    bool init(int port);
    bool init(const char * ip, int port);
    int getsocketfd();
    int waitClient(char * buf, char * clientIP);
    int waitClient(int timeinms, char * buf, char * clientIP);
    int sendToClient(const char * content, int length);

    int waitClient(char * buf, struct sockaddr * client_addr);
    int sendToClient(const char * content, int length, struct sockaddr * client_addr);
private:
    char SERVER_IP[32];
    int SERVER_PORT;

    int socket_fd;   // file description into transport
    struct sockaddr_in myaddr; // address of this service
    struct sockaddr_in client_addr; // address of client
    //int recfd; // file descriptor to accept
    socklen_t addrlength; // length of address structure
    char mClientIP[64];
};


#endif
