/*
 * TCPServer.cpp
 *
 *  Created on: 2017/07/04
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

#include "TCPServer.h"

TCPServer::TCPServer()
{
    SERVER_PORT = 80;
}

TCPServer::~TCPServer()
{
    close(m_master_socket);
}

bool TCPServer::init(int port)
{
    SERVER_PORT = port;
    int opt = 1;

    //initialise all client_socket[] to 0 so not checked
    for (int i = 0; i < MAX_TCP_CLIENT; i++)
    {
        m_client_socket[i] = 0;
    }

    //create a master socket
    if( (m_master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
    {
        vilslog::printf("TCPServer::init() socket failed\n");
        return false;
    }

    //set master socket to allow multiple connections ,
    //this is just a good habit, it will work without this
    if( setsockopt(m_master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, sizeof(opt)) < 0 )
    {
        vilslog::printf("TCPServer::init() setsockopt failed!\n");
        return false;
    }

    //type of socket created
    bzero ((char *)&m_myaddr, sizeof(m_myaddr));
    m_myaddr.sin_family = AF_INET;
    m_myaddr.sin_addr.s_addr = INADDR_ANY;
    m_myaddr.sin_port = htons( SERVER_PORT );

    //bind the socket to localhost SERVER_PORT
    if (::bind(m_master_socket, (struct sockaddr *)&m_myaddr, sizeof(m_myaddr)) < 0)
    {
        vilslog::printf("TCPServer::init() bind failed\n");
        return false;
    }
    vilslog::printf("TCPServer::init() Listener on port %d \n", SERVER_PORT);

    //try to specify maximum of 3 pending connections for the master socket
    if (listen(m_master_socket, 3) < 0)
    {
        vilslog::printf("TCPServer::init() listen error!\n");
        return false;
    }

    m_addrlength = sizeof(m_myaddr);


    //clear the socket set
    //FD_ZERO(&m_readfds);
    //add master socket to set
    //FD_SET(m_master_socket, &m_readfds);



    //accept the incoming connection
    vilslog::printf("TCPServer::init() Waiting for connections ...\n");

    return true;
}

int TCPServer::waitClient(int timeinms, char * buf, int buflen)
{
    //int ret = 0; // the number of client
    int max_sd;
    int client_sd;
    struct timeval tv;
    int new_socket;
    int addrlen;
    struct sockaddr_in clinet_address;
    int valread = 0;
    //char buffer[1025];  //data buffer of 1K

    tv.tv_sec = 0;
    tv.tv_usec = timeinms * 1000;

    //set of socket descriptors
    fd_set readfds;

    //clear the socket set
    FD_ZERO(&readfds);

    //add master socket to set
    FD_SET(m_master_socket, &readfds);

    max_sd = m_master_socket;

    //add child sockets to set
    for (int i=0; i<MAX_TCP_CLIENT; i++)
    {
        //socket descriptor
        client_sd = m_client_socket[i];

        //if valid socket descriptor then add to read list
        if(client_sd > 0)
            FD_SET( client_sd , &readfds);

        //highest file descriptor number, need it for the select function
        if(client_sd > max_sd)
            max_sd = client_sd;
    }

    //wait for an activity on one of the sockets , timeout is NULL ,
    //so wait indefinitely
    int activity = select( max_sd + 1, &readfds, NULL, NULL, &tv);

    if ((activity < 0) && (errno!=EINTR))
    {
        vilslog::printf("TCPServer::init() select error.\n");
        return 0;
    }
    if ( activity == 0 )
    {
        //printf("TCPServer::init() select Timeout.\n");
        return 0;
    }

    //If something happened on the master socket ,
    //then its an incoming connection
    if (FD_ISSET(m_master_socket, &readfds))
    {
        if ((new_socket = accept(m_master_socket, (struct sockaddr *)&clinet_address, (socklen_t*)&clinet_address))<0)
        {
            vilslog::printf("TCPServer::init() accept error\n");
            return 0;
        }

        //inform user of socket number - used in send and receive commands
        vilslog::printf("TCPServer::init() New connection , socket fd is %d , ip is : %s , port : %d\n",
            new_socket , inet_ntoa(clinet_address.sin_addr) , ntohs(clinet_address.sin_port));
        //add new socket to array of sockets
        for (int i=0; i<MAX_TCP_CLIENT; i++)
        {
            //if position is empty
            if( m_client_socket[i] == 0 )
            {
                m_client_socket[i] = new_socket;
                vilslog::printf("TCPServer::init() Adding to list of sockets as %d\n" , i);
                break;
            }
        }
    }

    //else its some IO operation on some other socket
    //int client_count = 0;
    for (int i=0; i<MAX_TCP_CLIENT; i++)
    {
        client_sd = m_client_socket[i];

        if (FD_ISSET(client_sd , &readfds))
        {
            //Check if it was for closing , and also read the
            //incoming message
            if ((valread = (int)read(client_sd , buf, buflen)) == 0)
            {
                //Somebody disconnected , get his details and print
                addrlen = sizeof(clinet_address);
                getpeername(client_sd , (struct sockaddr*)&clinet_address , (socklen_t*)&addrlen);
                vilslog::printf("TCPServer::init() Host disconnected , ip %s , port %d \n" , inet_ntoa(clinet_address.sin_addr) , ntohs(clinet_address.sin_port));

                //Close the socket and mark as 0 in list for reuse
                close( client_sd );
                m_client_socket[i] = 0;
            }
            else
            {
                if (valread > buflen)
                    valread = buflen - 1;
                buf[valread] = '\0';
                //printf("TCPServer::init() incoming message valread[%d] buf[%s]\n", valread, buf);
            }
            break;
        }
        //if ( m_client_socket[i] != 0 )
        //    client_count++;
    }
    return valread;
}

int TCPServer::sendToClient(const char * content, int length)
{
    int client_sd;

    //printf("TCPServer::sendToClient content[%s]\n", content);

   // send to all client
    for (int i=0; i<MAX_TCP_CLIENT; i++)
    {
        client_sd = m_client_socket[i];
        if (client_sd == 0)
            continue;
        if (send(client_sd, content, length, 0) != length)
        {
             vilslog::printf("TCPServer::sendToClient Could not send datagram!!\n");
             return 0;
        }
    }
    return length;
}
