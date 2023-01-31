/*
 * UDPServer.cpp
 *
 *  Created on: 2017/02/03
 *      Author: ikki
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "MulticastServer.h"
#include "../common.h"

MulticastServer::MulticastServer()
{
}

MulticastServer::~MulticastServer()
{
    close(socket_fd);
}

bool MulticastServer::init(int port)
{
    m_port = port;
    //
    // create what looks like an ordinary UDP socket
    //
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
         vilslog::printf("MulticastServer::init() socket failed");
         return false;
    }

    /**** MODIFICATION TO ORIGINAL */
    /* allow multiple sockets to use the same PORT number */
    u_int yes=1;            /*** MODIFICATION TO ORIGINAL */
    if (setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0)
    {
        vilslog::printf("MulticastServer::init() Reusing ADDR failed");
        return false;
    }
    /*** END OF MODIFICATION TO ORIGINAL */


    //
    //    Set up our address
    //
    bzero ((char *)&myaddr, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY); /* N.B.: differs from sender */
    myaddr.sin_port = htons(m_port);

    /* bind to receive address */
    if (::bind(socket_fd, (struct sockaddr *) &myaddr, sizeof(myaddr)) < 0)
    {
        vilslog::printf("MulticastServer::init() bind error!!");
        return false;
    }

    //set loop to 1 to enable loopback or 0 to disable it
    u_char loop=1;
    setsockopt(socket_fd, IPPROTO_IP, IP_MULTICAST_LOOP, &loop, sizeof(loop));

    u_char ttl=10;
    setsockopt(socket_fd, IPPROTO_IP, IP_MULTICAST_TTL, &ttl, sizeof(ttl));


    /* use setsockopt() to request that the kernel join a multicast group */
    mreq.imr_multiaddr.s_addr = inet_addr(MCAST_CENTER_SRV_GROUP);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);

    int status;
    status = setsockopt(socket_fd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&mreq.imr_interface.s_addr,
    sizeof(mreq.imr_interface.s_addr));
    if (status < 0)
    {
        vilslog::printf("MulticastServer::init() setsockopt IP_MULTICAST_IF failed!!!!\n");
        return false;
    }
    status = setsockopt(socket_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    if (status < 0)
    {
        vilslog::printf("MulticastServer::init() setsockopt IP_ADD_MEMBERSHIP failed!!!\n");
        return false;
    }

    // set receive UDP message timeout
    struct timeval timeout={2,0}; //set timeout for 2 seconds
    setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, (char*)&timeout, sizeof(struct timeval));
    addrlength = sizeof(client_addr);

//    if (setsockopt(socket_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq)) < 0)
 //   {
 //       vilslog::printf("MulticastServer::init() setsockopt");
 //       return false;
 //   }

    return true;
}


bool MulticastServer::reJoin()
{
    int status = setsockopt(socket_fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
    if (status < 0)
    {
        vilslog::printf("MulticastServer::reJoin() setsockopt IP_DROP_MEMBERSHIP failed!!! errno=%d\n", errno);
    }
    status = setsockopt(socket_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    if (status < 0)
    {
        vilslog::printf("MulticastServer::reJoin() setsockopt IP_ADD_MEMBERSHIP failed!!! errno=%d\n", errno);
    }
    return true;
}


int MulticastServer::waitClient(char * buf)
{
    // static int count = 0;
    // if (count >= 10)
    // {
    //     int status = setsockopt(socket_fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
    //     if (status < 0)
    //     {
    //         vilslog::printf("MulticastServer::waitClient() setsockopt IP_DROP_MEMBERSHIP failed!!! errno=%d\n", errno);
    //     }
    //     status = setsockopt(socket_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
    //     if (status < 0)
    //     {
    //         vilslog::printf("MulticastServer::waitClient() setsockopt IP_ADD_MEMBERSHIP failed!!! errno=%d\n", errno);
    //     }
    //     count =0;
    // }
    // count++;


    //
    // Loop continuously, waiting for datagrams
    // and response a message
    //
    int nbytes = (int)recvfrom(socket_fd, buf, MAXUDPPACKET, 0, (struct sockaddr *)&client_addr, (socklen_t *)&addrlength);
    if (nbytes >= 0)
    {
        //Message Received
        buf[nbytes] = '\0';
    }
    else
    {
        //Message Receive Timeout or other error
        return 0;
    }

    // if ((nbytes = recvfrom(socket_fd, buf, MAXUDPPACKET, 0, (struct sockaddr*)&client_addr, (socklen_t *)&addrlength)) <0)
    // {
    //      vilslog::printf("MulticastServer::waitClient() could not read datagram!!");
    //      return 0;
    // }
    // buf[nbytes] = '\0';

    //printf("MulticastServer::waitClient() buf[%s]\n", buf);

    return nbytes;
}

int MulticastServer::sendToClient(const char * content, int length)
{
    static int count = 0;
    if (count >=10)
    {
        int status = setsockopt(socket_fd, IPPROTO_IP, IP_DROP_MEMBERSHIP, &mreq, sizeof(mreq));
        if (status < 0)
        {
            vilslog::printf("MulticastServer::sendToClient() setsockopt IP_DROP_MEMBERSHIP failed!!! errno=%d\n", errno);
        }
        status = setsockopt(socket_fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(mreq));
        if (status < 0)
        {
            vilslog::printf("MulticastServer::sendToClient() setsockopt IP_ADD_MEMBERSHIP failed!!! errno=%d\n", errno);
        }
        count =0;
    }
    count++;

    // send to client
    if (sendto(socket_fd, content, length, 0, (struct sockaddr*)&client_addr, addrlength) < 0)
    {
         vilslog::printf("MulticastServer::sendToClient() Could not send datagram!!\n");
         return 0;
    }
    vilslog::printf("MulticastServer::sendToClient() length->%s",length);
    return length;
}
