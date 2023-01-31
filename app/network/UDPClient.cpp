/*
 * UDPClient.cpp
 *
 *  Created on: 2017/02/20
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

#include "UDPClient.h"

UDPClient::UDPClient()
{
    m_interface = "";
    m_ip = "";
}

UDPClient::UDPClient(std::string interface, std::string ip)
{
    m_interface = interface;
    m_ip = ip;
}

UDPClient::~UDPClient()
{
    close(socket_fd);
}

int UDPClient::getPort()
{
    return SERVER_PORT;
}

bool UDPClient::init(int port, const char * ip)
{
    SERVER_PORT = port;
    snprintf(SERVER_IP, 32, "%s", ip);

    //
    //      Get a socket into UDP/IP
    //
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
         vilslog::printf("UDPClient::init() socket failed");
         return false;
    }

    //
    //  Set up our address
    //
    bzero ((char *)&myaddr, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    if (m_ip.length() == 0)
        myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    else
    {
        myaddr.sin_addr.s_addr = inet_addr(m_ip.c_str());
    }
    myaddr.sin_port = htons(0);

    //
    //  Bind to an arbitrary return address.
    //
    int rc;
    if (( rc = ::bind(socket_fd, (struct sockaddr *)&myaddr, sizeof(myaddr))) < 0)
    {
        vilslog::printf("bind failed\n");
        return false;
    }

    int broadcastEnable = 1;
    setsockopt(socket_fd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable, sizeof(broadcastEnable));

    //
    // Fill in the server's UDP/IP address
    //
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    //server_addr.sin_addr.s_addr = inet_addr("192.168.1.45");
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    //server_addr.sin_addr.s_addr = inet_addr(serverIP);
    //server_addr.sin_addr.s_addr = htonl(INADDR_BROADCAST);

    serveraddrlength = sizeof(server_addr);

/*
    bzero((char *) &server_addr, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    hp = gethostbyname(hostname);

    if (hp == 0)
    {
        vilslog::printf("could not obtain address of %s\n", hostname);
        return false;
    }
    bcopy(hp->h_addr_list[0], (caddr_t)&server_addr.sin_addr, hp->h_length);
    serveraddrlength = sizeof(server_addr);
*/

    return true;
}

int UDPClient::send(const char * content, int length)
{
    // send to client
    if (sendto(socket_fd, content, length, 0, (struct sockaddr*)&server_addr, serveraddrlength) < 0)
    {
         //vilslog::printf("UDPClient::send() Could not send datagram!! SERVER_PORT[%d] SERVER_IP[%s]\n", SERVER_PORT, SERVER_IP);
         return 0;
    }

    return length;
}

int UDPClient::recv(char * buf, int buflen)
{
    int length = (int)recvfrom(socket_fd, buf, buflen, 0, (struct sockaddr*)&server_addr, &serveraddrlength);
    if ( length < 0)
    {
        vilslog::printf("Could not recvfrom datagram!!\n");
        return 0;
    }

    return length;
}
