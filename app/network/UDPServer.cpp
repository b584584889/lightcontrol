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

#include "UDPServer.h"

UDPServer::UDPServer()
{
    SERVER_PORT = 80;
}

UDPServer::~UDPServer()
{
    close(socket_fd);
}

bool UDPServer::init(int port)
{
    return init("", port);
}

bool UDPServer::init(const char * ip, int port)
{
    snprintf(SERVER_IP, 32, "%s",ip);
    SERVER_PORT = port;

    //
    //      Get a socket into UDP/IP
    //
    if ((socket_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
         vilslog::printf("UDPServer::init() socket failed");
         return false;
    }

    int yes = 1;
    if( setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) < 0 )
    {
        vilslog::printf("UDPServer::init() setsockopt SO_REUSEADDR failed!\n");
        perror("UDPServer::init() ");
        close(socket_fd);
        return false;
    }
    if( setsockopt(socket_fd, SOL_SOCKET, SO_REUSEPORT, &yes, sizeof(yes)) < 0 )
    {
        vilslog::printf("UDPServer::init() setsockopt SO_REUSEPORT failed!\n");
        perror("UDPServer::init() ");
        close(socket_fd);
        return false;
    }

    int maximumBufferSize = 65535;
    if( setsockopt(socket_fd, SOL_SOCKET, SO_SNDBUF, (const char*)&maximumBufferSize, sizeof(int)) < 0 )
    {
        vilslog::printf("UDPServer::init() sError setting send buffer size (setsockopt)\n");
        perror("UDPServer::init() ");
        close(socket_fd);
        return false;
    }

    //char CENTER_IP[32];
    //getMyCenterIP(CENTER_IP);

    //
    //    Set up our address
    //
    bzero ((char *)&myaddr, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    if (strlen(SERVER_IP) == 0)
        myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    else
        myaddr.sin_addr.s_addr = inet_addr(SERVER_IP);
    myaddr.sin_port = htons(SERVER_PORT);

    //
    //     Bind to the address to which the service will be offered
    //
    //int rc = bind(socket_fd, (struct sockaddr *)&myaddr, sizeof(myaddr));
    int rc = ::bind(socket_fd, (struct sockaddr *)&myaddr, sizeof(myaddr));
    if ( rc < 0)
    {
        vilslog::printf("=====================================ERROR==\n");
        vilslog::printf("UDPServer::init() bind failed errno[%d] SERVER_PORT[%d]\n", errno, SERVER_PORT);
        // vilslog::printf("UDPServer::init() ");
        vilslog::printf("=====================================*****==\n");
        return false;
    }
    return true;
}

int UDPServer::getsocketfd()
{
    return socket_fd;
}

int UDPServer::waitClient(char * buf, char * clientIP)
{
    int nbytes = 0; // the number of read

    //printf ("UDPServer::waitClient()\n");

    //
    // Loop continuously, waiting for datagrams
    // and response a message
    //
    addrlength = sizeof(client_addr);
    nbytes = (int)recvfrom(socket_fd, buf, MAXUDPPACKET, 0, (struct sockaddr*)&client_addr, (socklen_t *)&addrlength);
    if (nbytes <= 0)
    {
         // vilslog::printf("UDPServer::waitClient() could not read datagram!!\n");
         return 0;
    }
    //printf ("UDPServer::waitClient() nbytes[%d]\n", nbytes);

    buf[nbytes] = '\0';


    // ip
    //printf("Client IP:%s\n", inet_ntoa(client_addr.sin_addr));
    sprintf(mClientIP, "%s", inet_ntoa(client_addr.sin_addr));
    sprintf(clientIP, "%s", inet_ntoa(client_addr.sin_addr));

    return nbytes;
}

int UDPServer::waitClient(int timeinms, char * buf, char * clientIP)
{
    int nbytes = 0; // the number of read

    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = timeinms * 1000;

    if (setsockopt(socket_fd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv)) < 0)
    {
        vilslog::printf("Error\n");
        return nbytes;
    }

    addrlength = sizeof(client_addr);
    nbytes = (int)recvfrom(socket_fd, buf, MAXUDPPACKET, 0, (struct sockaddr*)&client_addr, (socklen_t *)&addrlength);
    if (nbytes <= 0)
    {
         // vilslog::printf("UDPServer::waitClient() could not read datagram!!\n");
         return 0;
    }
    //printf ("UDPServer::waitClient() nbytes[%d]\n", nbytes);

    buf[nbytes] = '\0';


    // ip
    //printf("Client IP:%s\n", inet_ntoa(client_addr.sin_addr));
    sprintf(mClientIP, "%s", inet_ntoa(client_addr.sin_addr));
    sprintf(clientIP, "%s", inet_ntoa(client_addr.sin_addr));

    return nbytes;
}

int UDPServer::sendToClient(const char * content, int length)
{
    //printf("UDPServer::sendToClient content[%s]\n", content);

    // send to client
    if (sendto(socket_fd, content, length, 0, (struct sockaddr*)&client_addr, addrlength) < 0)
    {
         vilslog::printf("UDPServer::sendToClient() Could not send datagram!! length[%d]\n",  length);
         perror("UDPServer::sendToClient() Could not send datagram!!\n");
         return 0;
    }

    return length;
}

int UDPServer::waitClient(char * buf, struct sockaddr * clientAddr)
{
    int nbytes = 0; // the number of read
    addrlength = sizeof(client_addr);
    nbytes = (int)recvfrom(socket_fd, buf, MAXUDPPACKET, 0, clientAddr, (socklen_t *)&addrlength);
    if (nbytes < 0)
    {
         perror ("UDPServer::waitClient() could not read datagram!!");
         return 0;
    }
    buf[nbytes] = '\0';
    return nbytes;
}

int UDPServer::sendToClient(const char * content, int length, struct sockaddr * clientAddr)
{
    //printf("UDPServer::sendToClient content[%s]\n", content);

    // send to client
    vilslog::printf("UDPServer::sendToClient content->%s length->%d\n", content, length);
    vilslog::printf("UDPServer::sendToClient length->%d addrlangth-> %d\n", length, addrlength);
    int sendlength = sendto(socket_fd, content, length, 0, clientAddr, addrlength);
    // if (sendto(socket_fd, content, length, 0, clientAddr, addrlength) < 0)
    // {
    //      vilslog::printf("UDPServer::sendToClient() Could not send datagram!! length[%d]\n",  length);
    //      perror("UDPServer::sendToClient() Could not send datagram!!\n");
    //      return 0;
    // }

    vilslog::printf("UDPServer::sendToClient sendlangth->%d langth->%d\n", sendlength, length);

    return length;
}
