/*
 *  AMQCPPConnectionHandler.cpp
 *
 *  Created on: 2021/02/04
 *      Author: ikki
 */

#include "AMQCPPConnectionHandler.h"

using namespace std;

/**
 *  Method that is called by the AMQP library when a new connection
 *  is associated with the handler. This is the first call to your handler
 *  @param  connection      The connection that is attached to the handler
 */
void AMQCPPConnectionHandler::onAttached(AMQP::TcpConnection *connection)
{
    // @todo
    //  add your own implementation, for example initialize things
    //  to handle the connection.
    vilslog::printf ("AMQCPPConnectionHandler()::onAttached()\n");
}

/**
 *  Method that is called by the AMQP library when the TCP connection
 *  has been established. After this method has been called, the library
 *  still has take care of setting up the optional TLS layer and of
 *  setting up the AMQP connection on top of the TCP layer., This method
 *  is always paired with a later call to onLost().
 *  @param  connection      The connection that can now be used
 */
void AMQCPPConnectionHandler::onConnected(AMQP::TcpConnection *connection)
{
    // @todo
    //  add your own implementation (probably not needed)
    vilslog::printf ("AMQCPPConnectionHandler()::onConnected()\n");
}

/**
 *  Method that is called when the secure TLS connection has been established.
 *  This is only called for amqps:// connections. It allows you to inspect
 *  whether the connection is secure enough for your liking (you can
 *  for example check the server certicate). The AMQP protocol still has
 *  to be started.
 *  @param  connection      The connection that has been secured
 *  @param  ssl             SSL structure from openssl library
 *  @return bool            True if connection can be used
 */
bool AMQCPPConnectionHandler::onSecured(AMQP::TcpConnection *connection, const SSL *ssl)
{
    // @todo
    //  add your own implementation, for example by reading out the
    //  certificate and check if it is indeed yours
    vilslog::printf ("AMQCPPConnectionHandler()::onSecured()\n");
    return true;
}

/**
 *  Method that is called by the AMQP library when the login attempt
 *  succeeded. After this the connection is ready to use.
 *  @param  connection      The connection that can now be used
 */
void AMQCPPConnectionHandler::onReady(AMQP::TcpConnection *connection)
{
    // @todo
    //  add your own implementation, for example by creating a channel
    //  instance, and start publishing or consuming
    vilslog::printf ("AMQCPPConnectionHandler()::onReady()\n");
}

/**
 *  Method that is called by the AMQP library when a fatal error occurs
 *  on the connection, for example because data received from RabbitMQ
 *  could not be recognized, or the underlying connection is lost. This
 *  call is normally followed by a call to onLost() (if the error occured
 *  after the TCP connection was established) and onDetached().
 *  @param  connection      The connection on which the error occured
 *  @param  message         A human readable error message
 */
void AMQCPPConnectionHandler::onError(AMQP::TcpConnection *connection, const char *message)
{
    // @todo
    //  add your own implementation, for example by reporting the error
    //  to the user of your program and logging the error
    vilslog::printf ("AMQCPPConnectionHandler()::onError()\n");
}

/**
 *  Method that is called when the AMQP protocol is ended. This is the
 *  counter-part of a call to connection.close() to graceful shutdown
 *  the connection. Note that the TCP connection is at this time still
 *  active, and you will also receive calls to onLost() and onDetached()
 *  @param  connection      The connection over which the AMQP protocol ended
 */
void AMQCPPConnectionHandler::onClosed(AMQP::TcpConnection *connection)
{
    // @todo
    //  add your own implementation (probably not necessary, but it could
    //  be useful if you want to do some something immediately after the
    //  amqp connection is over, but do not want to wait for the tcp
    //  connection to shut down
    vilslog::printf ("AMQCPPConnectionHandler()::onClosed()\n");
}

/**
 *  Method that is called when the TCP connection was closed or lost.
 *  This method is always called if there was also a call to onConnected()
 *  @param  connection      The connection that was closed and that is now unusable
 */
void AMQCPPConnectionHandler::onLost(AMQP::TcpConnection *connection)
{
    // @todo
    //  add your own implementation (probably not necessary)
    vilslog::printf ("AMQCPPConnectionHandler()::onLost()\n");
}

/**
 *  Final method that is called. This signals that no further calls to your
 *  handler will be made about the connection.
 *  @param  connection      The connection that can be destructed
 */
void AMQCPPConnectionHandler::onDetached(AMQP::TcpConnection *connection)
{
    // @todo
    //  add your own implementation, like cleanup resources or exit the application
    vilslog::printf ("AMQCPPConnectionHandler()::onDetached()\n");
}

/**
 *  Method that is called by the AMQP-CPP library when it wants to interact
 *  with the main event loop. The AMQP-CPP library is completely non-blocking,
 *  and only make "write()" or "read()" system calls when it knows in advance
 *  that these calls will not block. To register a filedescriptor in the
 *  event loop, it calls this "monitor()" method with a filedescriptor and
 *  flags telling whether the filedescriptor should be checked for readability
 *  or writability.
 *
 *  @param  connection      The connection that wants to interact with the event loop
 *  @param  fd              The filedescriptor that should be checked
 *  @param  flags           Bitwise or of AMQP::readable and/or AMQP::writable
 */
void AMQCPPConnectionHandler::monitor(AMQP::TcpConnection *connection, int fd, int flags)
{
    // @todo
    //  add your own implementation, for example by adding the file
    //  descriptor to the main application event loop (like the select() or
    //  poll() loop). When the event loop reports that the descriptor becomes
    //  readable and/or writable, it is up to you to inform the AMQP-CPP
    //  library that the filedescriptor is active by calling the
    //  connection->process(fd, flags) method.

    vilslog::printf ("AMQCPPConnectionHandler()::monitor() fd[%d] flags[%d]\n", fd, flags);

    // we did not yet have this watcher - but that is ok if no filedescriptor was registered
    if (flags == 0)
        return;

    if (flags & AMQP::readable)
    {
        FD_SET(fd, &m_set);
        m_fd = fd;
        m_flags = flags;
    }
}
