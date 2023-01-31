/*
 * MulticastServer.h
 *
 *  Created on: 2017/06/22
 *      Author: ikki
 */
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#ifndef __MULTICAST_SERVER_H__
#define __MULTICAST_SERVER_H__

#include "../config.h"

#define MCAST_CENTER_SRV_GROUP "225.0.0.14"

class MulticastServer
{
public:
    MulticastServer();
    ~MulticastServer();
    bool init(int port);
    bool reJoin();
    int waitClient(char * buf);
    int sendToClient(const char * content, int length);
private:
    int m_port;
    int socket_fd;   // file description into transport
    struct ip_mreq mreq;
    struct sockaddr_in myaddr; // address of this service

    struct sockaddr_in client_addr; // address of client
    //int recfd; // file descriptor to accept
    socklen_t addrlength; // length of address structure
};


#endif
