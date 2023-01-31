/*
 * MulticastClient.h
 *
 *  Created on: 2017/06/22
 *      Author: ikki
 */
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#ifndef __MULTICAST_CLIENT_H__
#define __MULTICAST_CLIENT_H__

#include "../config.h"

#define MCAST_COORD_SRV_GROUP "225.0.0.15"

class MulticastClient
{
public:
    MulticastClient();
    MulticastClient(std::string interface, std::string ip);
    ~MulticastClient();
    bool init(int port);
    int send(const char * content, int length);
    int recv(char * buf, int buflen);
    std::string getInterfaceName();
private:
    int m_port;
    int socket_fd;   // file description into transport
    std::string m_interface;
    std::string m_ip;
    struct sockaddr_in server_addr; // address of server
    socklen_t serveraddrlength; // length of address structure
};


#endif
