/*
 * MulticastClient.cpp
 *
 *  Created on: 2017/06/22
 *      Author: ikki
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <net/if.h>

#include "MulticastClient.h"
#include "../common.h"

MulticastClient::MulticastClient()
{
    m_interface = "";
    m_ip = "";
}

MulticastClient::MulticastClient(std::string interface, std::string ip)
{
    m_interface = interface;
    m_ip = ip;
}

MulticastClient::~MulticastClient()
{
    close(socket_fd);
}

bool MulticastClient::init(int port)
{
    m_port = port;

    //
    // create what looks like an ordinary UDP socket
    //
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
         vilslog::printf("MulticastClient::init() socket failed\n");
         return false;
    }

    /**** MODIFICATION TO ORIGINAL */
    /* allow multiple sockets to use the same PORT number */
    u_int yes=1;            /*** MODIFICATION TO ORIGINAL */
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
    {
        vilslog::printf("MulticastClient::init() Reusing ADDR failed\n");
        return false;
    }
    /*** END OF MODIFICATION TO ORIGINAL */


    //set loop to 1 to enable loopback or 0 to disable it
    u_char loop=1;
    setsockopt(socket_fd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));

    u_char ttl=10;
    setsockopt(socket_fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));

    if (m_interface.length() > 0)
    {
        vilslog::printf("MulticastClient::init() m_interface[%s]\n", m_interface.c_str());
        struct in_addr iaddr;
        // set content of struct saddr and imreq to zero
        memset(&iaddr, 0, sizeof(struct in_addr));

        //iaddr.s_addr = INADDR_ANY; // use DEFAULT interface
        iaddr.s_addr = inet_addr(m_ip.c_str());

        // Set the outgoing interface to interfacce
        if(setsockopt(socket_fd, IPPROTO_IP, IP_MULTICAST_IF, &iaddr, sizeof(in_addr)) < 0)
        {
            perror("MulticastClient::init() Setting local interface error\n");
        }

        // struct ifreq ifr;
        // memset(&ifr, 0, sizeof(ifr));
        // snprintf(ifr.ifr_name, sizeof(ifr.ifr_name), "%s", m_interface.c_str());
        // if (setsockopt(socket_fd, SOL_SOCKET, SO_BINDTODEVICE, (void *)&ifr, sizeof(ifr)) < 0)
        // {
        //     vilslog::printf("MulticastClient::init() SO_BINDTODEVICE failed!\n");
        // }
        // else
        // {
        //     vilslog::printf("MulticastClient::init() SO_BINDTODEVICE success!!\n");
        // }
    }

    //
    //  set up destination address
    //
    bzero ((char *)&server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(MCAST_COORD_SRV_GROUP);
    server_addr.sin_port = htons(m_port);

    serveraddrlength = sizeof(server_addr);

    return true;
}

int MulticastClient::send(const char * content, int length)
{
    // send to client
    int ret = (int)sendto(socket_fd, content, length, 0, (struct sockaddr*)&server_addr, serveraddrlength);
    if ( ret < 0)
    {
         vilslog::printf("MulticastClient::send() Could not send datagram!!\n");
         return 0;
    }

    return length;
}

int MulticastClient::recv(char * buf, int buflen)
{
    int length = (int)recvfrom(socket_fd, buf, buflen, 0, (struct sockaddr*)&server_addr, &serveraddrlength);
    if ( length < 0)
    {
        vilslog::printf("MulticastClient::recv() Could not recvfrom datagram!!\n");
        return 0;
    }

    return length;
}

std::string MulticastClient::getInterfaceName()
{
    return m_interface;
}
