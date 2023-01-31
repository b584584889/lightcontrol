/*
 * TCPServer.h
 *
 *  Created on: 2017/07/04
 *      Author: ikki
 */
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <unistd.h>   //close
#include <arpa/inet.h>    //close


#ifndef __TCP_SERVER_H__
#define __TCP_SERVER_H__

#include "../common.h"

#define MAX_TCP_CLIENT 30

class TCPServer
{
public:
    TCPServer();
    ~TCPServer();
    bool init(int port);
    int waitClient(int timeinms, char * buf, int buflen);
    int sendToClient(const char * content, int length);
private:
    int SERVER_PORT;
    //set of socket descriptors
    //fd_set m_readfds;

    int m_client_socket[MAX_TCP_CLIENT];
    int m_master_socket;
    struct sockaddr_in m_myaddr; // address of this service
    socklen_t m_addrlength; // length of address structure

    //int socket_fd;   // file description into transport
    //struct sockaddr_in myaddr; // address of this service
    //struct sockaddr_in client_addr; // address of client
    //int recfd; // file descriptor to accept
    //socklen_t addrlength; // length of address structure
    //char mClientIP[64];
};


#endif
