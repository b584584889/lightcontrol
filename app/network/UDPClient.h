/*
 * UDPClient.h
 *
 *  Created on: 2017/02/20
 *      Author: ikki
 */
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#ifndef __UDP_CLIENT_H__
#define __UDP_CLIENT_H__

#include "../common.h"

class UDPClient
{
public:
    UDPClient();
    UDPClient(std::string interface, std::string ip);
    ~UDPClient();
    bool init(int port, const char * ip);
    int send(const char * content, int length);
    int recv(char * buf, int buflen);
    int getPort();

private:
    int SERVER_PORT;
    char SERVER_IP[32];
    //struct hostent * hp;   // holds IP address of server
    std::string m_interface;
    std::string m_ip;

    int socket_fd;   // file description into transport
    struct sockaddr_in myaddr; // address of this service
    //struct sockaddr_in client_addr; // address of client
    struct sockaddr_in server_addr; // address of server
    //int recfd; // file descriptor to accept
    socklen_t serveraddrlength; // length of address structure
};


#endif
