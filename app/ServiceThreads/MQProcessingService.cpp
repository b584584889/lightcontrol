/*
 *  MQProcessingService.cpp
 *  https://github.com/CopernicaMarketingSoftware/AMQP-CPP
 *  Created on: 2021/02/04
 *      Author: ikki
 */

#include <string.h>
#include <sstream>
#include <iostream>
// #include <event2/event.h>

#include "MQProcessingService.h"
// #include "amqpcpp/libevent.h"
//#include "../MQ/TestBase.h"

using namespace std;

MQProcessingService::MQProcessingService(wqueue<DataItem*> & pMQqueue)
    :mMQJobQueue(pMQqueue)
{
}

bool MQProcessingService::status()
{
    return mInitial;
}

std::string MQProcessingService::statusString()
{
    return mStatus;
}

void * MQProcessingService::run()
{
    while(1)
    {
        try
        {
            runloop();
            vilslog::printf("MQProcessingService::run() escape from runloop()\n");
            sleep(10);
        }
        catch(const std::exception &exc)
        {
            // catch anything thrown within try block that derives from std::exception
            std::cerr << exc.what();
            vilslog::printf("*********************************************\n");
            vilslog::printf("MQProcessingService::run() exception \n");
            vilslog::printf(exc.what());
            vilslog::printf("\n");
            vilslog::printf("*********************************************\n");

            mInitial = false;
            mStatus = "MQProcessingService()::run() exception:";
            mStatus.append(exc.what());
        }
    }
    return NULL;
}

void MQProcessingService::runloop()
{
    mInitial = false;
    mStatus = "MQProcessingService()::run() Initial successfully!";
    vilslog::printf ("MQProcessingService()::run() Initial successfully!\n");

    mInitial = true;

    waitloop();

}

void MQProcessingService::waitloop()
{
    // MqInfo mq;
    // mq.ip = "127.0.0.1";
    // mq.port = 5672;
    // mq.loginName = "guest";
    // mq.loginPwd = "guest";
    // mq.exchangeName = "my-exchange1";
    // mq.exchangeType = "fanout";
    // mq.queueName = "my-queue1";
    // mq.routingKey = "my-routing-key1";
    // string peerQueueName = "";
    // mq.bindingKey = "";
    //
    // TestBase rmq;
    // if (rmq.Init(mq))
    // {
    //     string err;
    //
    //     sleep(5);
    //
    //     rmq.PublishMsg(std::string("My Message"), err);
    // }



    //==========================================
    // // access to the event loop
    // auto evbase = event_base_new();
    //
    // // handler for libevent
    // AMQP::LibEventHandler handler(evbase);
    //
    // // make a connection
    // AMQP::TcpConnection connection(&handler, AMQP::Address("amqp://guest:guest@localhost/"));
    //
    // // we need a channel too
    // AMQP::TcpChannel channel(&connection);
    //
    // // create a temporary queue
    // channel.declareQueue(AMQP::exclusive).onSuccess([&connection](const std::string &name, uint32_t messagecount, uint32_t consumercount) {
    //
    //     // report the name of the temporary queue
    //     std::cout << "declared queue " << name << std::endl;
    //
    //     // now we can close the connection
    //     connection.close();
    // });
    //
    // vilslog::printf("MQProcessingService::waitloop() event_base_dispatch(evbase)\n");
    //
    // // run the loop
    // event_base_dispatch(evbase);
    //
    // vilslog::printf("MQProcessingService::waitloop() event_base_free(evbase)\n");
    //
    // event_base_free(evbase);
    //
    // vilslog::printf("MQProcessingService::waitloop() while ( 1 )\n");
    //==========================================



    //==========================================
    // // create an instance of your own connection handler
    // AMQCPPConnectionHandler myHandler;
    //
    // // address of the server
    // AMQP::Address address("amqp://guest:guest@localhost:5672");
    //
    // // create a AMQP connection object
    // AMQP::TcpConnection connection(&myHandler, address);
    //
    // // and create a channel
    // AMQP::TcpChannel channel(&connection);
    //
    // channel.onReady([&channel]() {
    //         // channel.declareQueue(queueName);
    //         // channel.declareExchange(exchangeName, direct).onSuccess([]();
    //         // channel.bindQueue(exchangeName, queueName, routingKey);
    //         // channel.publish(exchangeName, routingKey, msg, msg.size());
    //         channel.bindQueue("my-exchange", "my-queue", "my-routing-key");
    //
    //         vilslog::printf("MQProcessingService::channel.onReady() \n");
    //
    //         std::string msg = "123123123";
    //         channel.publish("my-exchange", "my-routing-key", msg, msg.size());
    //     });
    //
    //
    // sleep(2);
    //
    // struct timeval tTime;
	// tTime.tv_sec = 10;
	// tTime.tv_usec = 0;
	// select(0, &myHandler.m_set, NULL, NULL, &tTime);
    //
    // int isset = FD_ISSET(myHandler.m_fd, &myHandler.m_set);
    // vilslog::printf("MQProcessingService() After select, isset[%d]\n", isset);

    // connection.process(myHandler.m_fd, myHandler.m_flags);


    // use the channel object to call the AMQP method you like
    // channel.declareExchange("my-exchange", AMQP::fanout);
    // channel.declareQueue("my-queue");
    //channel.bindQueue("my-exchange", "my-queue", "my-routing-key");
    //==========================================


    while ( 1 )
    {
        DataItem * item = mMQJobQueue.removetimed(1000);

        if ( item != NULL )
        {
            delete item;
        }

    }
}
