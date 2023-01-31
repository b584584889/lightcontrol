/*
 *  LLRPController.cpp
 *
 *  Created on: 2020/05/04
 *      Author: ikki
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sstream>
#include <iostream>

#include <netdb.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/errno.h>

#include "../htmlcxx/html/ParserDom.h"
#include "../htmlcxx/html/wincstring.h"

#include "LLRPController.h"

using namespace std;

LLRPController::LLRPController()
{
    mInitial = false;
    m_pConnectionToReader = NULL;
    m_TypeRegistry = NULL;
}

LLRPController::~LLRPController()
{
}

int LLRPController::prepareReader(const char * pReaderHostName, int nDurationMS)
{
    int rc = 0;
    m_ReaderHostName = pReaderHostName;

    //
    // Allocate the type registry. This is needed by the connection to decode.
    //
    m_TypeRegistry = LLRP::getTheTypeRegistry();
    if(NULL == m_TypeRegistry)
    {
        vilslog::printf("LLRPController::prepareReader() ERROR: getTheTypeRegistry failed\n");
        return -1;
    }

    //
    // Construct a connection (LLRP::CConnection).
    // Using a 32kb max frame size for send/recv.
    // The connection object is ready for business
    // but not actually connected to the reader yet.
    //
    m_pConnectionToReader = new LLRP::CConnection(m_TypeRegistry, 32u*1024u);
    if(NULL == m_pConnectionToReader)
    {
        vilslog::printf("LLRPController::prepareReader() ERROR: new CConnection failed\n");
        return -2;
    }

    //
    // Open the connection to the reader
    //
    rc = m_pConnectionToReader->openConnectionToReader(pReaderHostName);
    if(0 != rc)
    {
        vilslog::printf("LLRPController::prepareReader() ERROR: connect: %s (%d)\n", m_pConnectionToReader->getConnectError(), rc);
        //delete m_pConnectionToReader;
        return -3;
    }

    //
    // Record the pointer to the connection object so other
    // routines can use it.
    //
    // vilslog::printf("LLRPController::prepareReader() INFO: Connected, checking status....\n");

    //
    // Commence the sequence and check for errors as we go.
    // See comments for each routine for details.
    // Each routine prints messages.
    //
    rc = 1;
    if(0 == checkConnectionStatus())
    {
        rc = 2;
        if(0 == scrubConfiguration())
        {
            rc = 3;
            if(0 == addROSpec(nDurationMS))
            {
                rc = 4;
                if(0 == enableROSpec())
                {
                    // All ok
                    rc = 0;
                    mInitial = true;
                }
            }
        }
    }

    return rc;
}

std::list<RFIDCardStruct*> * LLRPController::readReader(int nMaxMS)
{
    // vilslog::printf("LLRPController::readReader() INFO: Starting run ================\n");

    if (!mInitial)
    {
        return NULL;
    }

    if(0 != startROSpec())
    {
        // already tattled
        mInitial = true;
        return NULL;
    }

    std::list<RFIDCardStruct*> * pCardList = awaitAndGetCardList(nMaxMS);

    return pCardList;
}

int LLRPController::closeReader()
{
    //
    // After we're done, try to leave the reader
    // in a clean state for next use. This is best
    // effort and no checking of the result is done.
    //
    // vilslog::printf("LLRPController::closeReader() INFO: Clean up reader configuration...\n");


    // vilslog::printf("LLRPController::closeReader() INFO: Finished\n");

    // Close the connection and release its resources
    //
    if (m_pConnectionToReader != NULL)
    {
        scrubConfiguration();

        m_pConnectionToReader->closeConnectionToReader();
        delete m_pConnectionToReader;
        m_pConnectionToReader = NULL;
    }

    //
    // Done with the registry.
    //
    if (m_TypeRegistry != NULL)
    {
        delete m_TypeRegistry;
        m_TypeRegistry = NULL;
    }

    mInitial = false;

    return 0;
}

bool LLRPController::isReady()
{
    return mInitial;
}

bool LLRPController::isSameHost(std::string ReaderHostName)
{
    return (m_ReaderHostName.compare(ReaderHostName) == 0);
}

int LLRPController::disconnectLLRP(const char * pReaderHostName, std::string username, std::string password)
{
    ZebraCookieStruct * zebraCookie = login_and_get_cookie(pReaderHostName, username, password);

    sleep(1);
    send_disconnect_request(pReaderHostName, zebraCookie);

    sleep(1);
    logout(pReaderHostName, zebraCookie);

    delete zebraCookie;
    return 0;
}



int LLRPController::run(const char * pReaderHostName, int nDurationMS, int nMaxMS)
{
    LLRP::CTypeRegistry * pTypeRegistry;
    LLRP::CConnection * pConn;
    int rc;

    //
    // Allocate the type registry. This is needed by the connection to decode.
    //
    pTypeRegistry = LLRP::getTheTypeRegistry();
    if(NULL == pTypeRegistry)
    {
        vilslog::printf("LLRPController::run() ERROR: getTheTypeRegistry failed\n");
        return -1;
    }

    //
    // Construct a connection (LLRP::CConnection).
    // Using a 32kb max frame size for send/recv.
    // The connection object is ready for business
    // but not actually connected to the reader yet.
    //
    pConn = new LLRP::CConnection(pTypeRegistry, 32u*1024u);
    if(NULL == pConn)
    {
        vilslog::printf("LLRPController::run() ERROR: new CConnection failed\n");
        return -2;
    }

    //
    // Open the connection to the reader
    //
    rc = pConn->openConnectionToReader(pReaderHostName);
    if(0 != rc)
    {
        vilslog::printf("LLRPController::run() ERROR: connect: %s (%d)\n", pConn->getConnectError(), rc);
        delete pConn;
        return -3;
    }

    //
    // Record the pointer to the connection object so other
    // routines can use it.
    //
    m_pConnectionToReader = pConn;

    vilslog::printf("LLRPController::run() INFO: Connected, checking status....\n");

    //
    // Commence the sequence and check for errors as we go.
    // See comments for each routine for details.
    // Each routine prints messages.
    //
    rc = 1;
    if(0 == checkConnectionStatus())
    {
        rc = 2;
        if(0 == scrubConfiguration())
        {
            rc = 3;
            if(0 == addROSpec(nDurationMS))
            {
                rc = 4;
                if(0 == enableROSpec())
                {
                    int i;

                    rc = 5;

                    for(i = 1; i <= 5; i++)
                    {
                        vilslog::printf("LLRPController::run() INFO: Starting run %d ================\n", i);
                        if(0 != startROSpec())
                        {
                            // already tattled
                            vilslog::printf("LLRPController::run() 0 != startROSpec()\n");
                            break;
                        }
                        if(0 != awaitAndPrintReport(7000))
                        {
                            // already tattled
                            vilslog::printf("LLRPController::run() 0 != awaitAndPrintReport(7000)\n");
                            break;
                        }
                    }

                    if(5 == i)
                    {
                        rc = 0;
                    }
                }
            }

            //
            // After we're done, try to leave the reader
            // in a clean state for next use. This is best
            // effort and no checking of the result is done.
            //
            vilslog::printf("LLRPController::run() INFO: Clean up reader configuration...\n");

            scrubConfiguration();
        }
    }

    vilslog::printf("LLRPController::run() INFO: Finished\n");


    //
    // Close the connection and release its resources
    //
    pConn->closeConnectionToReader();
    delete pConn;

    //
    // Done with the registry.
    //
    delete pTypeRegistry;

    //
    // When we get here all allocated memory should have been deallocated.
    //

    return rc;

}

//
//
// @brief  Await and check the connection status message from the reader
//
// We are expecting a READER_EVENT_NOTIFICATION message that
// tells us the connection is OK. The reader is suppose to
// send the message promptly upon connection.
//
// If there is already another LLRP connection to the
// reader we'll get a bad Status.
//
// The message should be something like:
//
//     <READER_EVENT_NOTIFICATION MessageID='0'>
//       <ReaderEventNotificationData>
//         <UTCTimestamp>
//           <Microseconds>1184491439614224</Microseconds>
//         </UTCTimestamp>
//         <ConnectionAttemptEvent>
//           <Status>Success</Status>
//         </ConnectionAttemptEvent>
//       </ReaderEventNotificationData>
//     </READER_EVENT_NOTIFICATION>
//
// @return     ==0             Everything OK
//             !=0             Something went wrong
//
int LLRPController::checkConnectionStatus(void)
{
    LLRP::CMessage * pMessage;
    LLRP::CREADER_EVENT_NOTIFICATION * pNtf;
    LLRP::CReaderEventNotificationData * pNtfData;
    LLRP::CConnectionAttemptEvent *pEvent;

    //
    // Expect the notification within 10 seconds.
    // It is suppose to be the very first message sent.
    //
    pMessage = recvMessage(10000);

    //
    // recvMessage() returns NULL if something went wrong.
    //
    if(NULL == pMessage)
    {
        // recvMessage already tattled
        vilslog::printf("LLRPController::checkConnectionStatus() ERROR: recvMessage already tattled\n");
        return -1;
    }

    //
    // Check to make sure the message is of the right type.
    // The type label (pointer) in the message should be
    // the type descriptor for READER_EVENT_NOTIFICATION.
    //
    if(&LLRP::CREADER_EVENT_NOTIFICATION::s_typeDescriptor != pMessage->m_pType)
    {
        vilslog::printf("LLRPController::checkConnectionStatus() ERROR: checkConnectionStatus -2 failed!\n");
        delete pMessage;
        return -2;
    }

    //
    // Now that we are sure it is a READER_EVENT_NOTIFICATION,
    // traverse to the ReaderEventNotificationData parameter.
    //
    pNtf = (LLRP::CREADER_EVENT_NOTIFICATION *) pMessage;
    pNtfData = pNtf->getReaderEventNotificationData();
    if(NULL == pNtfData)
    {
        vilslog::printf("LLRPController::checkConnectionStatus() ERROR: checkConnectionStatus -3 failed!\n");
        delete pMessage;
        return -3;
    }

    //
    // The ConnectionAttemptEvent parameter must be present.
    //
    pEvent = pNtfData->getConnectionAttemptEvent();
    if(NULL == pEvent)
    {
        vilslog::printf("LLRPController::checkConnectionStatus() ERROR: checkConnectionStatus -4 failed!\n");
        delete pMessage;
        return -4;
    }

    //
    // The status in the ConnectionAttemptEvent parameter
    // must indicate connection success.
    //
    int iStatus = pEvent->getStatus();
    if(LLRP::ConnectionAttemptStatusType_Success != iStatus)
    {
        if ( iStatus == LLRP::ConnectionAttemptStatusType_Failed_A_Reader_Initiated_Connection_Already_Exists )
            vilslog::printf("LLRPController::checkConnectionStatus() ERROR: ConnectionAttemptStatusType_Failed_A_Reader_Initiated_Connection_Already_Exists\n");
        else
        if ( iStatus == LLRP::ConnectionAttemptStatusType_Failed_A_Client_Initiated_Connection_Already_Exists )
            vilslog::printf("LLRPController::checkConnectionStatus() ERROR: ConnectionAttemptStatusType_Failed_A_Client_Initiated_Connection_Already_Exists\n");
        else
        if ( iStatus == LLRP::ConnectionAttemptStatusType_Failed_Reason_Other_Than_A_Connection_Already_Exists )
            vilslog::printf("LLRPController::checkConnectionStatus() ERROR: ConnectionAttemptStatusType_Failed_Reason_Other_Than_A_Connection_Already_Exists\n");
        else
        if ( iStatus == LLRP::ConnectionAttemptStatusType_Another_Connection_Attempted )
            vilslog::printf("LLRPController::checkConnectionStatus() ERROR: ConnectionAttemptStatusType_Another_Connection_Attempted\n");
        else
            vilslog::printf("LLRPController::checkConnectionStatus() ERROR: checkConnectionStatus -5 failed!\n");

        delete pMessage;
        return -5;
    }

    //
    // Done with the message
    //
    delete pMessage;

    //
    // Victory.
    //
    // vilslog::printf("LLRPController::checkConnectionStatus() INFO: Connection status OK\n");
    return 0;
}

//
// @brief  Scrub the reader configuration
//
// The steps:
//     - Try to reset configuration to factory defaults,
//       this feature is optional and may not be supported
//       by the reader.
//     - Delete all ROSpecs
//
// @return     ==0             Everything OK
//             !=0             Something went wrong
//
int LLRPController::scrubConfiguration(void)
{
    if(0 != resetConfigurationToFactoryDefaults())
    {
        return -1;
    }

    if(0 != deleteAllROSpecs())
    {
        return -2;
    }

    return 0;
}

//
// @brief  Send a SET_READER_CONFIG message that resets the
//         reader to factory defaults.
//
// NB: The ResetToFactoryDefault semantics vary between readers.
//     It might have no effect because it is optional.
//
// The message is:
//
//     <SET_READER_CONFIG MessageID='101'>
//       <ResetToFactoryDefault>1</ResetToFactoryDefault>
//     </SET_READER_CONFIG>
//
// @return     ==0             Everything OK
//             !=0             Something went wrong
//
int LLRPController::resetConfigurationToFactoryDefaults(void)
{
    LLRP::CSET_READER_CONFIG * pCmd;
    LLRP::CMessage * pRspMsg;
    LLRP::CSET_READER_CONFIG_RESPONSE * pRsp;

    //
    // Compose the command message
    //
    pCmd = new LLRP::CSET_READER_CONFIG();
    pCmd->setMessageID(101);
    pCmd->setResetToFactoryDefault(1);

    //
    // Send the message, expect the response of certain type
    //
    pRspMsg = transact(pCmd);

    //
    // Done with the command message
    //
    delete pCmd;

    //
    // transact() returns NULL if something went wrong.
    //
    if(NULL == pRspMsg)
    {
        // transact already tattled
        return -1;
    }

    //
    // Cast to a SET_READER_CONFIG_RESPONSE message.
    //
    pRsp = (LLRP::CSET_READER_CONFIG_RESPONSE *) pRspMsg;

    //
    // Check the LLRPStatus parameter.
    //
    if(0 != checkLLRPStatus(pRsp->getLLRPStatus(), "resetConfigurationToFactoryDefaults"))
    {
        // checkLLRPStatus already tattled
        delete pRspMsg;
        return -1;
    }

    //
    // Done with the response message.
    //
    delete pRspMsg;

    //
    // Tattle progress, maybe
    //
    // vilslog::printf("LLRPController::resetConfigurationToFactoryDefaults() INFO: Configuration reset to factory defaults\n");

    //
    // Victory.
    //
    return 0;
}

//
// @brief  Delete all ROSpecs using DELETE_ROSPEC message
//
// Per the spec, the DELETE_ROSPEC message contains an ROSpecID
// of 0 to indicate we want all ROSpecs deleted.
//
// The message is
//
//     <DELETE_ROSPEC MessageID='102'>
//       <ROSpecID>0</ROSpecID>
//     </DELETE_ROSPEC>
//
// @return     ==0             Everything OK
//             !=0             Something went wrong
//
int LLRPController::deleteAllROSpecs(void)
{
    LLRP::CDELETE_ROSPEC * pCmd;
    LLRP::CMessage * pRspMsg;
    LLRP::CDELETE_ROSPEC_RESPONSE * pRsp;

    //
    // Compose the command message
    //
    pCmd = new LLRP::CDELETE_ROSPEC();
    pCmd->setMessageID(102);
    pCmd->setROSpecID(0);               // All

    //
    // Send the message, expect the response of certain type
    //
    pRspMsg = transact(pCmd);

    //
    // Done with the command message
    //
    delete pCmd;

    //
    // transact() returns NULL if something went wrong.
    //
    if(NULL == pRspMsg)
    {
        // transact already tattled
        return -1;
    }

    //
    // Cast to a DELETE_ROSPEC_RESPONSE message.
    //
    pRsp = (LLRP::CDELETE_ROSPEC_RESPONSE *) pRspMsg;

    //
    // Check the LLRPStatus parameter.
    //
    if(0 != checkLLRPStatus(pRsp->getLLRPStatus(), "deleteAllROSpecs"))
    {
        // checkLLRPStatus already tattled
        delete pRspMsg;
        return -1;
    }

    //
    // Done with the response message.
    //
    delete pRspMsg;

    //
    // Tattle progress, maybe
    //
    // vilslog::printf("LLRPController::deleteAllROSpecs() INFO: All ROSpecs are deleted\n");

    //
    // Victory.
    //
    return 0;

}

//
// @brief  Add our ROSpec using ADD_ROSPEC message
//
// This ROSpec waits for a START_ROSPEC message,
// then takes inventory on all antennas for 5 seconds.
//
// The tag report is generated after the ROSpec is done.
//
// This example is deliberately streamlined.
// Nothing here configures the antennas, RF, or Gen2.
// The current defaults are used. Remember we just reset
// the reader to factory defaults (above). Normally an
// application would be more precise in configuring the
// reader and in its ROSpecs.
//
// Experience suggests that typical ROSpecs are about
// double this in size.
//
// The message is
//
//     <ADD_ROSPEC MessageID='201'>
//       <ROSpec>
//         <ROSpecID>123</ROSpecID>
//         <Priority>0</Priority>
//         <CurrentState>Disabled</CurrentState>
//         <ROBoundarySpec>
//           <ROSpecStartTrigger>
//             <ROSpecStartTriggerType>Null</ROSpecStartTriggerType>
//           </ROSpecStartTrigger>
//           <ROSpecStopTrigger>
//             <ROSpecStopTriggerType>Null</ROSpecStopTriggerType>
//             <DurationTriggerValue>0</DurationTriggerValue>
//           </ROSpecStopTrigger>
//         </ROBoundarySpec>
//         <AISpec>
//           <AntennaIDs>0</AntennaIDs>
//           <AISpecStopTrigger>
//             <AISpecStopTriggerType>Duration</AISpecStopTriggerType>
//             <DurationTrigger>5000</DurationTrigger>
//           </AISpecStopTrigger>
//           <InventoryParameterSpec>
//             <InventoryParameterSpecID>1234</InventoryParameterSpecID>
//             <ProtocolID>EPCGlobalClass1Gen2</ProtocolID>
//           </InventoryParameterSpec>
//         </AISpec>
//         <ROReportSpec>
//           <ROReportTrigger>Upon_N_Tags_Or_End_Of_ROSpec</ROReportTrigger>
//           <N>0</N>
//           <TagReportContentSelector>
//             <EnableROSpecID>0</EnableROSpecID>
//             <EnableSpecIndex>0</EnableSpecIndex>
//             <EnableInventoryParameterSpecID>0</EnableInventoryParameterSpecID>
//             <EnableAntennaID>0</EnableAntennaID>
//             <EnableChannelIndex>0</EnableChannelIndex>
//             <EnablePeakRSSI>0</EnablePeakRSSI>
//             <EnableFirstSeenTimestamp>0</EnableFirstSeenTimestamp>
//             <EnableLastSeenTimestamp>0</EnableLastSeenTimestamp>
//             <EnableTagSeenCount>0</EnableTagSeenCount>
//             <EnableAccessSpecID>0</EnableAccessSpecID>
//           </TagReportContentSelector>
//         </ROReportSpec>
//       </ROSpec>
//     </ADD_ROSPEC>
//
// @return     ==0             Everything OK
//             !=0             Something went wrong
//
int LLRPController::addROSpec(int nDurationMS)
{
    LLRP::CROSpecStartTrigger * pROSpecStartTrigger = new LLRP::CROSpecStartTrigger();
    pROSpecStartTrigger->setROSpecStartTriggerType(LLRP::ROSpecStartTriggerType_Null);

    LLRP::CROSpecStopTrigger * pROSpecStopTrigger = new LLRP::CROSpecStopTrigger();
    pROSpecStopTrigger->setROSpecStopTriggerType(LLRP::ROSpecStopTriggerType_Null);
    pROSpecStopTrigger->setDurationTriggerValue(0);     // n/a

    LLRP::CROBoundarySpec * pROBoundarySpec = new LLRP::CROBoundarySpec();
    pROBoundarySpec->setROSpecStartTrigger(pROSpecStartTrigger);
    pROBoundarySpec->setROSpecStopTrigger(pROSpecStopTrigger);

    LLRP::CAISpecStopTrigger * pAISpecStopTrigger = new LLRP::CAISpecStopTrigger();
    pAISpecStopTrigger->setAISpecStopTriggerType(LLRP::AISpecStopTriggerType_Duration);
    pAISpecStopTrigger->setDurationTrigger(nDurationMS);

    LLRP::CInventoryParameterSpec * pInventoryParameterSpec = new LLRP::CInventoryParameterSpec();
    pInventoryParameterSpec->setInventoryParameterSpecID(1234);
    pInventoryParameterSpec->setProtocolID(LLRP::AirProtocols_EPCGlobalClass1Gen2);

    LLRP::llrp_u16v_t AntennaIDs = LLRP::llrp_u16v_t(1);
    AntennaIDs.m_pValue[0] = 0;// All

    LLRP::CAISpec * pAISpec = new LLRP::CAISpec();
    pAISpec->setAntennaIDs(AntennaIDs);
    pAISpec->setAISpecStopTrigger(pAISpecStopTrigger);
    pAISpec->addInventoryParameterSpec(pInventoryParameterSpec);

    LLRP::CTagReportContentSelector * pTagReportContentSelector = new LLRP::CTagReportContentSelector();
    pTagReportContentSelector->setEnableROSpecID(FALSE);
    pTagReportContentSelector->setEnableSpecIndex(FALSE);
    pTagReportContentSelector->setEnableInventoryParameterSpecID(FALSE);
    pTagReportContentSelector->setEnableAntennaID(FALSE);
    pTagReportContentSelector->setEnableChannelIndex(FALSE);
    pTagReportContentSelector->setEnablePeakRSSI(FALSE);
    pTagReportContentSelector->setEnableFirstSeenTimestamp(FALSE);
    pTagReportContentSelector->setEnableLastSeenTimestamp(FALSE);
    pTagReportContentSelector->setEnableTagSeenCount(FALSE);
    pTagReportContentSelector->setEnableAccessSpecID(FALSE);

    LLRP::CROReportSpec * pROReportSpec = new LLRP::CROReportSpec();
    pROReportSpec->setROReportTrigger(LLRP::ROReportTriggerType_Upon_N_Tags_Or_End_Of_ROSpec);
    pROReportSpec->setN(0);         // Unlimited
    pROReportSpec->setTagReportContentSelector(pTagReportContentSelector);

    LLRP::CROSpec * pROSpec = new LLRP::CROSpec();
    pROSpec->setROSpecID(123);
    pROSpec->setPriority(0);
    pROSpec->setCurrentState(LLRP::ROSpecState_Disabled);
    pROSpec->setROBoundarySpec(pROBoundarySpec);
    pROSpec->addSpecParameter(pAISpec);
    pROSpec->setROReportSpec(pROReportSpec);

    LLRP::CADD_ROSPEC * pCmd;
    LLRP::CMessage * pRspMsg;
    LLRP::CADD_ROSPEC_RESPONSE * pRsp;

    //
    // Compose the command message.
    // N.B.: After the message is composed, all the parameters
    //       constructed, immediately above, are considered "owned"
    //       by the command message. When it is destructed so
    //       too will the parameters be.
    //
    pCmd = new LLRP::CADD_ROSPEC();
    pCmd->setMessageID(201);
    pCmd->setROSpec(pROSpec);

    //
    // Send the message, expect the response of certain type
    //
    pRspMsg = transact(pCmd);

    //
    // Done with the command message.
    // N.B.: And the parameters
    //
    delete pCmd;

    //
    // transact() returns NULL if something went wrong.
    //
    if(NULL == pRspMsg)
    {
        // transact already tattled
        return -1;
    }

    //
    // Cast to a ADD_ROSPEC_RESPONSE message.
    //
    pRsp = (LLRP::CADD_ROSPEC_RESPONSE *) pRspMsg;

    //
    // Check the LLRPStatus parameter.
     //
    if(0 != checkLLRPStatus(pRsp->getLLRPStatus(), "addROSpec"))
    {
        // checkLLRPStatus already tattled
        delete pRspMsg;
        return -1;
    }

    //
    // Done with the response message.
    //
    delete pRspMsg;

    //
    // Tattle progress, maybe
    //
    // vilslog::printf("LLRPController::addROSpec() INFO: ROSpec added\n");

    //
    // Victory.
    //
    return 0;
}

//
// @brief  Enable our ROSpec using ENABLE_ROSPEC message
//
// Enable the ROSpec that was added above.
//
// The message we send is:
//     <ENABLE_ROSPEC MessageID='202'>
//       <ROSpecID>123</ROSpecID>
//     </ENABLE_ROSPEC>
//
// @return     ==0             Everything OK
//             !=0             Something went wrong
//
int LLRPController::enableROSpec(void)
{
    LLRP::CENABLE_ROSPEC * pCmd;
    LLRP::CMessage * pRspMsg;
    LLRP::CENABLE_ROSPEC_RESPONSE * pRsp;

    //
    // Compose the command message
    //
    pCmd = new LLRP::CENABLE_ROSPEC();
    pCmd->setMessageID(202);
    pCmd->setROSpecID(123);

    //
    // Send the message, expect the response of certain type
    //
    pRspMsg = transact(pCmd);

    //
    // Done with the command message
    //
    delete pCmd;

    //
    // transact() returns NULL if something went wrong.
    //
    if(NULL == pRspMsg)
    {
        // transact already tattled
        return -1;
    }

    //
    // Cast to a ENABLE_ROSPEC_RESPONSE message.
    //
    pRsp = (LLRP::CENABLE_ROSPEC_RESPONSE *) pRspMsg;

    //
    // Check the LLRPStatus parameter.
    //
    if(0 != checkLLRPStatus(pRsp->getLLRPStatus(), "enableROSpec"))
    {
        // checkLLRPStatus already tattled
        delete pRspMsg;
        return -1;
    }

    //
    // Done with the response message.
    //
    delete pRspMsg;

    //
    // Tattle progress, maybe
    //
    // vilslog::printf("LLRPController::enableROSpec() INFO: ROSpec enabled\n");

    //
    // Victory.
    //
    return 0;
}

//
// @brief  Start our ROSpec using START_ROSPEC message
//
// Start the ROSpec that was added above.
//
// The message we send is:
//     <START_ROSPEC MessageID='203'>
//       <ROSpecID>123</ROSpecID>
//     </START_ROSPEC>
//
// @return     ==0             Everything OK
//             !=0             Something went wrong
//
int LLRPController::startROSpec(void)
{
    LLRP::CSTART_ROSPEC * pCmd;
    LLRP::CMessage * pRspMsg;
    LLRP::CSTART_ROSPEC_RESPONSE * pRsp;

    //
    // Compose the command message
    //
    pCmd = new LLRP::CSTART_ROSPEC();
    pCmd->setMessageID(202);
    pCmd->setROSpecID(123);

    //
    // Send the message, expect the response of certain type
    //
    pRspMsg = transact(pCmd);

    //
    // Done with the command message
    //
    delete pCmd;

    //
    // transact() returns NULL if something went wrong.
    //
    if(NULL == pRspMsg)
    {
        // transact already tattled
        return -1;
    }

    //
    // Cast to a START_ROSPEC_RESPONSE message.
    //
    pRsp = (LLRP::CSTART_ROSPEC_RESPONSE *) pRspMsg;

    //
    // Check the LLRPStatus parameter.
    //
    if(0 != checkLLRPStatus(pRsp->getLLRPStatus(), "startROSpec"))
    {
        // checkLLRPStatus already tattled
        delete pRspMsg;
        return -1;
    }

    //
    // Done with the response message.
    //
    delete pRspMsg;

    //
    // Tattle progress
    //
    // vilslog::printf("LLRPController::startROSpec() INFO: ROSpec started\n");

    //
    // Victory.
    //
    return 0;
}

//
// @brief  Receive and print the RO_ACCESS_REPORT
//
// Receive messages until an RO_ACCESS_REPORT is received.
// Time limit is 7 seconds. We expect a report within 5 seconds.
//
// This shows how to determine the type of a received message.
//
// @return     ==0             Everything OK
//             !=0             Something went wrong
//
int LLRPController::awaitAndPrintReport(int nMaxMS)
{
    int bDone = 0;
    int retVal = 0;

    //
    // Keep receiving messages until done or until
    // something bad happens.
    //
    while(!bDone)
    {
        LLRP::CMessage * pMessage;
        const LLRP::CTypeDescriptor * pType;

        //
        // Wait up to 7 seconds for a message. The report
        // should occur within 5 seconds.
        //
        pMessage = recvMessage(nMaxMS);
        if(NULL == pMessage)
        {
            //
            // Did not receive a message within a reasonable
            // amount of time. recvMessage() already tattled
            //
            retVal = -2;
            bDone = 1;

            mInitial = false;

            vilslog::printf("LLRPController::awaitAndPrintReport() NULL == pMessage\n");

            break;;
        }

        //
        // What happens depends on what kind of message
        // received. Use the type label (m_pType) to
        // discriminate message types.
        //
        pType = pMessage->m_pType;

        //
        // Is it a tag report? If so, print it out.
        //
        if(&LLRP::CRO_ACCESS_REPORT::s_typeDescriptor == pType)
        {
            LLRP::CRO_ACCESS_REPORT * pNtf;

            pNtf = (LLRP::CRO_ACCESS_REPORT *) pMessage;

            printTagReportData(pNtf);
            bDone = 1;
            retVal = 0;
        }
        else
        if(&LLRP::CREADER_EVENT_NOTIFICATION::s_typeDescriptor == pType)
        {
            //
            // Is it a reader event? This example only recognizes
            // AntennaEvents.
            //

            LLRP::CREADER_EVENT_NOTIFICATION * pNtf;
            LLRP::CReaderEventNotificationData * pNtfData;

            pNtf = (LLRP::CREADER_EVENT_NOTIFICATION *) pMessage;

            pNtfData = pNtf->getReaderEventNotificationData();
            if(NULL != pNtfData)
            {
                handleReaderEventNotification(pNtfData);
            }
            else
            {
                //
                // This should never happen. Using continue
                // to keep indent depth down.
                //
                vilslog::printf("LLRPController::awaitAndPrintReport() WARNING: READER_EVENT_NOTIFICATION without data\n");
            }
        }
        else
        {
            //
            // Hmmm. Something unexpected. Just tattle and keep going.
            //
            vilslog::printf("LLRPController::awaitAndPrintReport() WARNING: Ignored unexpected message during monitor: %s\n", pType->m_pName);
        }

        //
        // Done with the received message
        //
        delete pMessage;
    }

    return retVal;
}

std::list<RFIDCardStruct*> * LLRPController::awaitAndGetCardList(int nMaxMS)
{
    int bDone = 0;
    std::list<RFIDCardStruct*> * cardList = new std::list<RFIDCardStruct*>();

    //
    // Keep receiving messages until done or until
    // something bad happens.
    //
    while(!bDone)
    {
        LLRP::CMessage * pMessage;
        const LLRP::CTypeDescriptor * pType;

        //
        // Wait up to 7 seconds for a message. The report
        // should occur within 5 seconds.
        //
        pMessage = recvMessage(nMaxMS);
        if(NULL == pMessage)
        {
            //
            // Did not receive a message within a reasonable
            // amount of time. recvMessage() already tattled
            //
            //retVal = -2;
            bDone = 1;

            vilslog::printf("LLRPController::awaitAndGetCardList() NULL == pMessage\n");

            delete cardList;
            cardList = NULL;

            continue;
        }

        //
        // What happens depends on what kind of message
        // received. Use the type label (m_pType) to
        // discriminate message types.
        //
        pType = pMessage->m_pType;

        //
        // Is it a tag report? If so, print it out.
        //
        if(&LLRP::CRO_ACCESS_REPORT::s_typeDescriptor == pType)
        {
            LLRP::CRO_ACCESS_REPORT * pNtf;

            pNtf = (LLRP::CRO_ACCESS_REPORT *) pMessage;

            //printTagReportData(pNtf);

            std::list<RFIDCardStruct*> * msgcardList = getTagReportRFIDCard(pNtf);

            for (std::list<RFIDCardStruct*>::iterator msgcard = msgcardList->begin(); msgcard != msgcardList->end(); msgcard++)
            {
                bool bFound = false;
                for (std::list<RFIDCardStruct*>::iterator card = cardList->begin(); card != cardList->end(); card++)
                {
                    if ( (*msgcard)->cardid.compare((*card)->cardid) == 0 )
                    {
                        bFound = true;
                        break;
                    }
                }
                if (!bFound)
                {
                    RFIDCardStruct * newCard = new RFIDCardStruct();
                    newCard->cardid = (*msgcard)->cardid;
                    cardList->push_back(newCard);
                }
            }

            // vilslog::printf("awaitAndGetCardList() cardList->size[%d] msgcardList->size[%d]\n", (int)cardList->size(), (int)msgcardList->size());

            if (msgcardList != NULL)
            {
                clear_RFIDCardStruct_list(msgcardList);
                delete msgcardList;
            }

            bDone = 1;
            //retVal = 0;
        }
        else
        if(&LLRP::CREADER_EVENT_NOTIFICATION::s_typeDescriptor == pType)
        {
            //
            // Is it a reader event? This example only recognizes
            // AntennaEvents.
            //

            LLRP::CREADER_EVENT_NOTIFICATION * pNtf;
            LLRP::CReaderEventNotificationData * pNtfData;

            pNtf = (LLRP::CREADER_EVENT_NOTIFICATION *) pMessage;

            pNtfData = pNtf->getReaderEventNotificationData();
            if(NULL != pNtfData)
            {
                handleReaderEventNotification(pNtfData);
            }
            else
            {
                //
                // This should never happen. Using continue
                // to keep indent depth down.
                //
                vilslog::printf("LLRPController::awaitAndGetCardList() WARNING: READER_EVENT_NOTIFICATION without data\n");
            }
        }
        else
        {
            //
            // Hmmm. Something unexpected. Just tattle and keep going.
            //
            vilslog::printf("LLRPController::awaitAndGetCardList() WARNING: Ignored unexpected message during monitor: %s\n", pType->m_pName);
        }

        //
        // Done with the received message
        //
        delete pMessage;
    }

    return cardList;
}

std::list<RFIDCardStruct*> * LLRPController::getTagReportRFIDCard(LLRP::CRO_ACCESS_REPORT * pRO_ACCESS_REPORT)
{
    std::list<RFIDCardStruct*> * cardList = new std::list<RFIDCardStruct*>();

    std::list<LLRP::CTagReportData *>::iterator Cur;

    //
    // Loop through again and print each entry.
    //
    for( Cur = pRO_ACCESS_REPORT->beginTagReportData(); Cur != pRO_ACCESS_REPORT->endTagReportData(); Cur++)
    {
        try{
            std::string tagid = getOneTagReportId(*Cur);

            if (tagid.length() > 0)
            {
                RFIDCardStruct * card = new RFIDCardStruct();
                card->cardid = tagid;
                cardList->push_back(card);
            }
        }
        catch ( ... )
        {
            vilslog::printf("LLRPController::getTagReportRFIDCard() Caught Abort\n");
        }
    }
    return cardList;
}

std::string LLRPController::getOneTagReportId(LLRP::CTagReportData * pTagReportData)
{
    const LLRP::CTypeDescriptor * pType;
    char aBuf[64];
    std::string tagid = "";


    //
    // Print the EPC. It could be an 96-bit EPC_96 parameter
    // or an variable length EPCData parameter.
    //
    LLRP::CParameter * pEPCParameter = pTagReportData->getEPCParameter();

    if(NULL != pEPCParameter)
    {
        char * p = aBuf;
        LLRP::llrp_u96_t my_u96;
        LLRP::llrp_u1v_t my_u1v;
        LLRP::llrp_u8_t * pValue = NULL;
        unsigned int n, i;

        pType = pEPCParameter->m_pType;
        if(&LLRP::CEPC_96::s_typeDescriptor == pType)
        {
            LLRP::CEPC_96 * pEPC_96;

            pEPC_96 = (LLRP::CEPC_96 *) pEPCParameter;
            my_u96 = pEPC_96->getEPC();
            pValue = my_u96.m_aValue;
            n = 12u;
        }
        else
        if(&LLRP::CEPCData::s_typeDescriptor == pType)
        {
            LLRP::CEPCData * pEPCData;

            pEPCData = (LLRP::CEPCData *) pEPCParameter;
            my_u1v = pEPCData->getEPC();
            pValue = my_u1v.m_pValue;
            n = (my_u1v.m_nBit + 7u) / 8u;
        }

        if(NULL != pValue)
        {
            for(i = 0; i < n; i++)
            {
                // if(0 < i && i%2 == 0)
                // {
                //     *p++ = '-';
                // }
                sprintf(p, "%02X", pValue[i]);
                while(*p)
                {
                    p++;
                }
            }
        }
        else
        {
            //strcpy(aBuf, "---unknown-epc-data-type---");
        }
    }
    else
    {
        //strcpy(aBuf, "---missing-epc-data---");
    }

    tagid = aBuf;

    //vilslog::printf("%-32s", aBuf);

    //
    // End of line
    //
    //vilslog::printf("\n");
    return tagid;
}


//
// @brief  Helper routine to print a tag report
//
// The report is printed in list order, which is arbitrary.
//
// TODO: It would be cool to sort the list by EPC and antenna,
//       then print it.
//
// @return     void
//
void LLRPController::printTagReportData(LLRP::CRO_ACCESS_REPORT * pRO_ACCESS_REPORT)
{
    std::list<LLRP::CTagReportData *>::iterator Cur;
    unsigned int nEntry = 0;

    //
    // Loop through and count the number of entries
    //
    for( Cur = pRO_ACCESS_REPORT->beginTagReportData(); Cur != pRO_ACCESS_REPORT->endTagReportData(); Cur++)
    {
        nEntry++;
    }

    vilslog::printf("LLRPController::printTagReportData() INFO: %u tag report entries\n", nEntry);

    //
    // Loop through again and print each entry.
    //
    for( Cur = pRO_ACCESS_REPORT->beginTagReportData(); Cur != pRO_ACCESS_REPORT->endTagReportData(); Cur++)
    {
        printOneTagReportData(*Cur);
    }
}

//
// @brief  Helper routine to print one tag report entry on one line
//
// @return     void
//
void LLRPController::printOneTagReportData(LLRP::CTagReportData * pTagReportData)
{
    const LLRP::CTypeDescriptor * pType;
    char aBuf[64];

    //
    // Print the EPC. It could be an 96-bit EPC_96 parameter
    // or an variable length EPCData parameter.
    //
    LLRP::CParameter * pEPCParameter = pTagReportData->getEPCParameter();

    if(NULL != pEPCParameter)
    {
        char * p = aBuf;
        LLRP::llrp_u96_t my_u96;
        LLRP::llrp_u1v_t my_u1v;
        LLRP::llrp_u8_t * pValue = NULL;
        unsigned int n, i;

        pType = pEPCParameter->m_pType;
        if(&LLRP::CEPC_96::s_typeDescriptor == pType)
        {
            LLRP::CEPC_96 * pEPC_96;

            pEPC_96 = (LLRP::CEPC_96 *) pEPCParameter;
            my_u96 = pEPC_96->getEPC();
            pValue = my_u96.m_aValue;
            n = 12u;
        }
        else
        if(&LLRP::CEPCData::s_typeDescriptor == pType)
        {
            LLRP::CEPCData * pEPCData;

            pEPCData = (LLRP::CEPCData *) pEPCParameter;
            my_u1v = pEPCData->getEPC();
            pValue = my_u1v.m_pValue;
            n = (my_u1v.m_nBit + 7u) / 8u;
        }

        if(NULL != pValue)
        {
            for(i = 0; i < n; i++)
            {
                if(0 < i && i%2 == 0)
                {
                    *p++ = '-';
                }
                sprintf(p, "%02X", pValue[i]);
                while(*p)
                {
                    p++;
                }
            }
        }
        else
        {
            strcpy(aBuf, "---unknown-epc-data-type---");
        }
    }
    else
    {
        strcpy(aBuf, "---missing-epc-data---");
    }

    vilslog::printf("%-32s", aBuf);

    //
    // End of line
    //
    vilslog::printf("\n");
}

//
// @brief  Handle a ReaderEventNotification
//
// Handle the payload of a READER_EVENT_NOTIFICATION message.
// This routine simply dispatches to handlers of specific
// event types.
//
// @return     void
//
void LLRPController::handleReaderEventNotification(LLRP::CReaderEventNotificationData * pNtfData)
{
    LLRP::CAntennaEvent * pAntennaEvent;
    LLRP::CReaderExceptionEvent * pReaderExceptionEvent;
    int nReported = 0;

    pAntennaEvent = pNtfData->getAntennaEvent();
    if(NULL != pAntennaEvent)
    {
        handleAntennaEvent(pAntennaEvent);
        nReported++;
    }

    pReaderExceptionEvent = pNtfData->getReaderExceptionEvent();
    if(NULL != pReaderExceptionEvent)
    {
        handleReaderExceptionEvent(pReaderExceptionEvent);
        nReported++;
    }

    //
    // Similarly handle other events here:
    //      HoppingEvent
    //      GPIEvent
    //      ROSpecEvent
    //      ReportBufferLevelWarningEvent
    //      ReportBufferOverflowErrorEvent
    //      RFSurveyEvent
    //      AISpecEvent
    //      ConnectionAttemptEvent
    //      ConnectionCloseEvent
    //      Custom
    //
    if(0 == nReported)
    {
        vilslog::printf("LLRPController::handleReaderEventNotification() NOTICE: Unexpected (unhandled) ReaderEvent\n");
    }
}

//
// @brief  Handle an AntennaEvent
//
// An antenna was disconnected or (re)connected. Tattle.
//
// @return     void
//
void LLRPController::handleAntennaEvent(LLRP::CAntennaEvent * pAntennaEvent)
{
    LLRP::EAntennaEventType eEventType;
    LLRP::llrp_u16_t AntennaID;
    std::string pStateStr;

    eEventType = pAntennaEvent->getEventType();
    AntennaID = pAntennaEvent->getAntennaID();

    switch(eEventType)
    {
    case LLRP::AntennaEventType_Antenna_Disconnected:
        pStateStr = "disconnected";
        break;

    case LLRP::AntennaEventType_Antenna_Connected:
        pStateStr = "connected";
        break;

    default:
        pStateStr = "?unknown-event?";
        break;
    }

    vilslog::printf("LLRPController::handleAntennaEvent() NOTICE: Antenna %d is %s\n", AntennaID, pStateStr.c_str());
}

//
// @brief  Handle a ReaderExceptionEvent
//
// Something has gone wrong. There are lots of details but
// all this does is print the message, if one.
//
// @return     void
//
void LLRPController::handleReaderExceptionEvent(LLRP::CReaderExceptionEvent * pReaderExceptionEvent)
{
    LLRP::llrp_utf8v_t Message;

    Message = pReaderExceptionEvent->getMessage();

    if(0 < Message.m_nValue && NULL != Message.m_pValue)
    {
        vilslog::printf("LLRPController::handleReaderExceptionEvent() NOTICE: ReaderException '%.*s'\n", Message.m_nValue, Message.m_pValue);
    }
    else
    {
        vilslog::printf("LLRPController::handleReaderExceptionEvent() NOTICE: ReaderException but no message\n");
    }
}

//
// @brief  Helper routine to check an LLRPStatus parameter
//         and tattle on errors
//
// Helper routine to interpret the LLRPStatus subparameter
// that is in all responses. It tattles on an error, if one,
// and tries to safely provide details.
//
// This simplifies the code, above, for common check/tattle
// sequences.
//
// @return     ==0             Everything OK
//             !=0             Something went wrong, already tattled
//
int LLRPController::checkLLRPStatus(LLRP::CLLRPStatus * pLLRPStatus, const char * pWhatStr)
{
    //
    // The LLRPStatus parameter is mandatory in all responses.
    // If it is missing there should have been a decode error.
    // This just makes sure (remember, this program is a
    // diagnostic and suppose to catch LTKC mistakes).
    //
    if(NULL == pLLRPStatus)
    {
        vilslog::printf("LLRPController::checkLLRPStatus() ERROR: %s missing LLRP status\n", pWhatStr);
        return -1;
    }

    //
    // Make sure the status is M_Success.
    // If it isn't, print the error string if one.
    // This does not try to pretty-print the status
    // code. To get that, run this program with -vv
    // and examine the XML output.
    //
    if(LLRP::StatusCode_M_Success != pLLRPStatus->getStatusCode())
    {
        LLRP::llrp_utf8v_t ErrorDesc;

        ErrorDesc = pLLRPStatus->getErrorDescription();

        if(0 == ErrorDesc.m_nValue)
        {
            vilslog::printf("LLRPController::checkLLRPStatus() ERROR: %s failed, no error description given\n", pWhatStr);
        }
        else
        {
            vilslog::printf("LLRPController::checkLLRPStatus() ERROR: %s failed, %.*s\n", pWhatStr, ErrorDesc.m_nValue, ErrorDesc.m_pValue);
        }
        return -2;
    }

    //
    // Victory. Everything is fine.
    //
    return 0;
}

//
// @brief  Wrapper routine to do an LLRP transaction
//
// Wrapper to transact a request/resposne.
//     - Print the outbound message in XML if verbose level is at least 2
//     - Send it using the LLRP_Conn_transact()
//     - LLRP_Conn_transact() receives the response or recognizes an error
//     - Tattle on errors, if any
//     - Print the received message in XML if verbose level is at least 2
//     - If the response is ERROR_MESSAGE, the request was sufficiently
//       misunderstood that the reader could not send a proper reply.
//       Deem this an error, free the message.
//
// The message returned resides in allocated memory. It is the
// caller's obligtation to free it.
//
// @return     ==NULL          Something went wrong, already tattled
//             !=NULL          Pointer to a message
//
LLRP::CMessage * LLRPController::transact(LLRP::CMessage * pSendMsg)
{
    LLRP::CConnection * pConn = m_pConnectionToReader;
    LLRP::CMessage * pRspMsg;

    //
    // Print the XML text for the outbound message if
    // verbosity is 2 or higher.
    //
    //vilslog::printf("LLRPController::transact() \n===================================\n");
    //vilslog::printf("LLRPController::transact() INFO: Transact sending\n");
    //printXMLMessage(pSendMsg);

    //
    // Send the message, expect the response of certain type.
    // If LLRP::CConnection::transact() returns NULL then there was
    // an error. In that case we try to print the error details.
    //
    pRspMsg = pConn->transact(pSendMsg, 5000);

    if(NULL == pRspMsg)
    {
        const LLRP::CErrorDetails * pError = pConn->getTransactError();

        vilslog::printf("LLRPController::transact() ERROR: %s transact failed, %s\n", pSendMsg->m_pType->m_pName,
            pError->m_pWhatStr ? pError->m_pWhatStr : "no reason given");

        if(NULL != pError->m_pRefType)
        {
            vilslog::printf("LLRPController::transact() ERROR: ... reference type %s\n", pError->m_pRefType->m_pName);
        }

        if(NULL != pError->m_pRefField)
        {
            vilslog::printf("LLRPController::transact() ERROR: ... reference field %s\n", pError->m_pRefField->m_pName);
        }

        return NULL;
    }

    //
    // Print the XML text for the inbound message if
    // verbosity is 2 or higher.
    //
    //vilslog::printf("LLRPController::transact() \n- - - - - - - - - - - - - - - - - -\n");
    //vilslog::printf("LLRPController::transact() INFO: Transact received response\n");
    //printXMLMessage(pRspMsg);

    //
    // If it is an ERROR_MESSAGE (response from reader
    // when it can't understand the request), tattle
    // and declare defeat.
    //
    if(&LLRP::CERROR_MESSAGE::s_typeDescriptor == pRspMsg->m_pType)
    {
        const LLRP::CTypeDescriptor * pResponseType;

        pResponseType = pSendMsg->m_pType->m_pResponseType;

        vilslog::printf("LLRPController::transact() ERROR: Received ERROR_MESSAGE instead of %s\n", pResponseType->m_pName);
        delete pRspMsg;
        pRspMsg = NULL;
    }

    return pRspMsg;
}

//
// @brief  Wrapper routine to receive a message
//
// This can receive notifications as well as responses.
//     - Recv a message using the LLRP_Conn_recvMessage()
//     - Tattle on errors, if any
//     - Print the message in XML if verbose level is at least 2
//
// The message returned resides in allocated memory. It is the
// caller's obligtation to free it.
//
// @param[in]  nMaxMS          -1 => block indefinitely
//                              0 => just peek at input queue and
//                                   socket queue, return immediately
//                                   no matter what
//                             >0 => ms to await complete frame
//
// @return     ==NULL          Something went wrong, already tattled
//             !=NULL          Pointer to a message
//
LLRP::CMessage * LLRPController::recvMessage(int nMaxMS)
{
    LLRP::CConnection * pConn = m_pConnectionToReader;
    LLRP::CMessage * pMessage;

    //
    // Receive the message subject to a time limit
    //
    pMessage = pConn->recvMessage(nMaxMS);

    //
    // If LLRP::CConnection::recvMessage() returns NULL then there was
    // an error. In that case we try to print the error details.
    //
    if(NULL == pMessage)
    {
        const LLRP::CErrorDetails * pError = pConn->getRecvError();

        vilslog::printf("LLRPController::recvMessage() ERROR: recvMessage failed, %s\n",
            pError->m_pWhatStr ? pError->m_pWhatStr : "no reason given");

        if(NULL != pError->m_pRefType)
        {
            vilslog::printf("LLRPController::recvMessage() ERROR: ... reference type %s\n", pError->m_pRefType->m_pName);
        }

        if(NULL != pError->m_pRefField)
        {
            vilslog::printf("LLRPController::recvMessage() ERROR: ... reference field %s\n", pError->m_pRefField->m_pName);
        }

        return NULL;
    }

    //
    // Print the XML text for the inbound message if
    // verbosity is 2 or higher.
    //
    //vilslog::printf("LLRPController::recvMessage() \n===================================\n");
    //vilslog::printf("LLRPController::recvMessage() INFO: Message received\n");
    // printXMLMessage(pMessage);

    return pMessage;
}

//
// @brief  Wrapper routine to send a message
//
// Wrapper to send a message.
//     - Print the message in XML if verbose level is at least 2
//     - Send it using the LLRP_Conn_sendMessage()
//     - Tattle on errors, if any
//
// @param[in]  pSendMsg        Pointer to message to send
//
// @return     ==0             Everything OK
//             !=0             Something went wrong, already tattled
//
int LLRPController::sendMessage(LLRP::CMessage * pSendMsg)
{
    LLRP::CConnection * pConn = m_pConnectionToReader;

    //
    // Print the XML text for the outbound message if
    // verbosity is 2 or higher.
    //
    vilslog::printf("LLRPController::sendMessage() \n===================================\n");
    vilslog::printf("LLRPController::sendMessage() INFO: Sending\n");
    //printXMLMessage(pSendMsg);

    //
    // If LLRP::CConnection::sendMessage() returns other than RC_OK
    // then there was an error. In that case we try to print
    // the error details.
    //
    if(LLRP::RC_OK != pConn->sendMessage(pSendMsg))
    {
        const LLRP::CErrorDetails * pError = pConn->getSendError();

        vilslog::printf("LLRPController::sendMessage() ERROR: %s sendMessage failed, %s\n", pSendMsg->m_pType->m_pName,
            pError->m_pWhatStr ? pError->m_pWhatStr : "no reason given");

        if(NULL != pError->m_pRefType)
        {
            vilslog::printf("LLRPController::sendMessage() ERROR: ... reference type %s\n", pError->m_pRefType->m_pName);
        }

        if(NULL != pError->m_pRefField)
        {
            vilslog::printf("LLRPController::sendMessage() ERROR: ... reference field %s\n", pError->m_pRefField->m_pName);
        }

        return -1;
    }

    //
    // Victory
    //
    return 0;
}

//
// @brief  Helper to print a message as XML text
//
// Print a LLRP message as XML text
//
// @param[in]  pMessage        Pointer to message to print
//
// @return     void
//
void LLRPController::printXMLMessage(LLRP::CMessage * pMessage)
{
    char aBuf[100*1024];

    //
    // Convert the message to an XML string.
    // This fills the buffer with either the XML string
    // or an error message. The return value could
    // be checked.
    //
    pMessage->toXMLString(aBuf, sizeof aBuf);

    //
    // Print the XML Text to the standard output.
    //
    vilslog::printf("LLRPController::printXMLMessage() %s", aBuf);
}

ZebraCookieStruct * LLRPController::login_and_get_cookie(const char * pReaderHostName, std::string username, std::string password)
{
    ZebraCookieStruct * cookie = new ZebraCookieStruct();

    const char *host = pReaderHostName; // 目標 URI
    const char *command = "control";
    const char *PORT_NUM = "80"; // HTTP port

    char request[4096];
    memset(request, 0, 4096);

    const char *requestLineFmt = "POST /%s HTTP/1.1\r\n"; // 請求行
    const char *headerFmt = "Host: %s:%s\r\n"; // Host 表頭欄位
    const char *OriginFmt = "Origin: http://%s\r\n";
    const char *RefererFmt = "Referer: http://%s/login.html\r\n";
    const char *ContentType = "Content-Type: application/x-www-form-urlencoded\r\n";
    const char *ContentLengthFmt = "Content-Length: %d\r\n\r\n";
    const char *cookieFmt = "Cookie: RMSessionID=84:24:8D:80:16:83:1589792978:A; isReadTagsPage=true; isFullSupport=true; isAdmin=true\r\n";
    const char *MessageXMLVersion = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    const char *MessageCommandStart = "<rm:command epcglobal:creationDate=\"%sZ\" epcglobal:schemaVersion=\"0.0\" xsi:schemaLocation=\"urn:epcglobal:rm:xsd:1 ../../../schemas/RmCommand.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:rm=\"urn:epcglobal:rm:xsd:1\" xmlns:epcglobal=\"urn:epcglobal:xsd:1\" xmlns:motorm=\"urn:motorfid:rm:xsd:1\">";
    const char *MessageId = "<rm:id>104</rm:id>";
    const char *MessageTargetName = "<rm:targetName></rm:targetName>";
    const char *MessageReaderDeviceStart = "<motorm:readerDevice>";
    //const char *MessageLogin1 = "<motorm:sessionID>NotSet</motorm:sessionID>";
    const char *MessageSessionID = "<motorm:sessionID>84:24:8D:80:16:83:1589792978:A</motorm:sessionID>";
    const char *MessageLogin1 = "<motorm:doLogin>";
    const char *MessageLogin2 = "<motorm:username>%s</motorm:username><motorm:password>%s</motorm:password>";
    const char *MessageLogin3 = "<motorm:forceLogin>true</motorm:forceLogin>";
    const char *MessageLogin4 = "</motorm:doLogin>";
    const char *MessageReaderDeviceEnd = "</motorm:readerDevice>";
    const char *MessageCommandEnd = "</rm:command>";
    const char *CRLF = "\r\n";  // 表頭後的 CRLF

    char buffer[4096];
    int bufferLen = 4096;
    memset(buffer, 0, bufferLen);

    //組裝請求訊息
    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, requestLineFmt, command);
    strcpy(request, buffer);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, headerFmt, host, PORT_NUM);
    strcat(request, buffer);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, OriginFmt, host);
    strcat(request, buffer);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, RefererFmt, host);
    strcat(request, buffer);

    strcat(request, ContentType);
    strcat(request, cookieFmt);

    char sendmessage[4096];
    memset(sendmessage, 0, 4096);

    strcat(sendmessage, MessageXMLVersion);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, MessageCommandStart, currentDateTimeISO8601().c_str());
    strcat(sendmessage, buffer);

    strcat(sendmessage, MessageId);
    strcat(sendmessage, MessageTargetName);
    strcat(sendmessage, MessageReaderDeviceStart);

    strcat(sendmessage, MessageSessionID);

    //=========================
    // login
    strcat(sendmessage, MessageLogin1);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, MessageLogin2, username.c_str(), password.c_str());
    strcat(sendmessage, buffer);

    strcat(sendmessage, MessageLogin3);
    strcat(sendmessage, MessageLogin4);
    //=========================

    strcat(sendmessage, MessageReaderDeviceEnd);
    strcat(sendmessage, MessageCommandEnd);

    memset(buffer, 0, 4096);
    snprintf(buffer, bufferLen, ContentLengthFmt, strlen(sendmessage));
    strcat(request, buffer);

    strcat(request, sendmessage);
    strcat(request, CRLF);

    //
    // sen out request
    //
    if ( get_cookie(host, PORT_NUM, request, cookie) )
    {
        return cookie;
    }
    else
    {
        vilslog::printf("LLRPController::login_and_get_cookie() failed!\n");
        return cookie;
    }

    return cookie;
}

bool LLRPController::send_disconnect_request(const char * pReaderHostName, ZebraCookieStruct * cookie)
{
    const char *host = pReaderHostName; // 目標 URI
    const char *command = "control";
    const char *PORT_NUM = "80"; // HTTP port

    char request[4096];
    memset(request, 0, 4096);

    const char *requestLineFmt = "POST /%s HTTP/1.1\r\n"; // 請求行
    const char *headerFmt = "Host: %s:%s\r\n"; // Host 表頭欄位
    const char *OriginFmt = "Origin: http://%s\r\n";
    const char *RefererFmt = "Referer: http://%s/login.html\r\n";
    const char *ContentType = "Content-Type: application/x-www-form-urlencoded\r\n";
    const char *ContentLengthFmt = "Content-Length: %d\r\n\r\n";
    const char *cookieFmt = "Cookie: RMSessionID=NotSet; isReadTagsPage=true; isFullSupport=true; isAdmin=true\r\n";
    const char *MessageXMLVersion = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    const char *MessageCommandStart = "<rm:command epcglobal:creationDate=\"%sZ\" epcglobal:schemaVersion=\"0.0\" xsi:schemaLocation=\"urn:epcglobal:rm:xsd:1 ../../../schemas/RmCommand.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:rm=\"urn:epcglobal:rm:xsd:1\" xmlns:epcglobal=\"urn:epcglobal:xsd:1\" xmlns:motorm=\"urn:motorfid:rm:xsd:1\">";
    const char *MessageId = "<rm:id>104</rm:id>";
    const char *MessageTargetName = "<rm:targetName></rm:targetName>";
    const char *MessageReaderDeviceStart = "<motorm:readerDevice>";
    const char *MessageSessionID = "<motorm:sessionID>%s</motorm:sessionID>";
    const char *MessageConnectLLRP1 = "<motorm:ConnectLLRP>";
    const char *MessageConnectLLRP2 = "<motorm:LLRPConnectAction>false</motorm:LLRPConnectAction>";
    const char *MessageConnectLLRP3 = "</motorm:ConnectLLRP>";
    const char *MessageReaderDeviceEnd = "</motorm:readerDevice>";
    const char *MessageCommandEnd = "</rm:command>";
    const char *CRLF = "\r\n";  // 表頭後的 CRLF

    char buffer[4096];
    int bufferLen = 4096;
    memset(buffer, 0, bufferLen);

    //組裝請求訊息
    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, requestLineFmt, command);
    strcpy(request, buffer);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, headerFmt, host, PORT_NUM);
    strcat(request, buffer);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, OriginFmt, host);
    strcat(request, buffer);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, RefererFmt, host);
    strcat(request, buffer);

    strcat(request, ContentType);
    strcat(request, cookieFmt);

    char sendmessage[4096];
    memset(sendmessage, 0, 4096);

    strcat(sendmessage, MessageXMLVersion);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, MessageCommandStart, currentDateTimeISO8601().c_str());
    strcat(sendmessage, buffer);

    strcat(sendmessage, MessageId);
    strcat(sendmessage, MessageTargetName);
    strcat(sendmessage, MessageReaderDeviceStart);

    memset(buffer, 0, 4096);
    snprintf(buffer, bufferLen, MessageSessionID, cookie->RMSessionID.c_str());
    strcat(sendmessage, buffer);


    //=========================
    strcat(sendmessage, MessageConnectLLRP1);
    strcat(sendmessage, MessageConnectLLRP2);
    strcat(sendmessage, MessageConnectLLRP3);
    //=========================

    strcat(sendmessage, MessageReaderDeviceEnd);
    strcat(sendmessage, MessageCommandEnd);

    memset(buffer, 0, 4096);
    snprintf(buffer, bufferLen, ContentLengthFmt, strlen(sendmessage));
    strcat(request, buffer);

    strcat(request, sendmessage);
    strcat(request, CRLF);

    //
    // sen out request
    //
    sendRequest(host, PORT_NUM, request);
    // std::string http_response_string = sendRequest(host, PORT_NUM, request);
    // vilslog::printf("LLRPController::send_disconnect_request() http_response_string[%s]\n", http_response_string.c_str());

    return true;
}

bool LLRPController::logout(const char * pReaderHostName, ZebraCookieStruct * cookie)
{
    const char *host = pReaderHostName; // 目標 URI
    const char *command = "control";
    const char *PORT_NUM = "80"; // HTTP port

    char request[4096];
    memset(request, 0, 4096);

    const char *requestLineFmt = "POST /%s HTTP/1.1\r\n"; // 請求行
    const char *headerFmt = "Host: %s:%s\r\n"; // Host 表頭欄位
    const char *OriginFmt = "Origin: http://%s\r\n";
    const char *RefererFmt = "Referer: http://%s/login.html\r\n";
    const char *ContentType = "Content-Type: application/x-www-form-urlencoded\r\n";
    const char *ContentLengthFmt = "Content-Length: %d\r\n\r\n";
    const char *cookieFmt = "Cookie: RMSessionID=NotSet; isReadTagsPage=true; isFullSupport=true; isAdmin=true\r\n";
    const char *MessageXMLVersion = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    const char *MessageCommandStart = "<rm:command epcglobal:creationDate=\"%sZ\" epcglobal:schemaVersion=\"0.0\" xsi:schemaLocation=\"urn:epcglobal:rm:xsd:1 ../../../schemas/RmCommand.xsd\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:rm=\"urn:epcglobal:rm:xsd:1\" xmlns:epcglobal=\"urn:epcglobal:xsd:1\" xmlns:motorm=\"urn:motorfid:rm:xsd:1\">";
    const char *MessageId = "<rm:id>104</rm:id>";
    const char *MessageTargetName = "<rm:targetName></rm:targetName>";
    const char *MessageReaderDeviceStart = "<motorm:readerDevice>";
    const char *MessageSessionID = "<motorm:sessionID>%s</motorm:sessionID>";
    const char *MessageLogout = "<motorm:doLogout/>";
    const char *MessageReaderDeviceEnd = "</motorm:readerDevice>";
    const char *MessageCommandEnd = "</rm:command>";
    const char *CRLF = "\r\n";  // 表頭後的 CRLF

    char buffer[4096];
    int bufferLen = 4096;
    memset(buffer, 0, bufferLen);

    //組裝請求訊息
    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, requestLineFmt, command);
    strcpy(request, buffer);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, headerFmt, host, PORT_NUM);
    strcat(request, buffer);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, OriginFmt, host);
    strcat(request, buffer);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, RefererFmt, host);
    strcat(request, buffer);

    strcat(request, ContentType);
    strcat(request, cookieFmt);

    char sendmessage[4096];
    memset(sendmessage, 0, 4096);

    strcat(sendmessage, MessageXMLVersion);

    memset(buffer, 0, bufferLen);
    snprintf(buffer, bufferLen, MessageCommandStart, currentDateTimeISO8601().c_str());
    strcat(sendmessage, buffer);

    strcat(sendmessage, MessageId);
    strcat(sendmessage, MessageTargetName);
    strcat(sendmessage, MessageReaderDeviceStart);

    memset(buffer, 0, 4096);
    snprintf(buffer, bufferLen, MessageSessionID, cookie->RMSessionID.c_str());
    strcat(sendmessage, buffer);

    //=========================
    // logout
    strcat(sendmessage, MessageLogout);
    //=========================

    strcat(sendmessage, MessageReaderDeviceEnd);
    strcat(sendmessage, MessageCommandEnd);

    memset(buffer, 0, 4096);
    snprintf(buffer, bufferLen, ContentLengthFmt, strlen(sendmessage));
    strcat(request, buffer);

    strcat(request, sendmessage);
    strcat(request, CRLF);

    //
    // sen out request
    //
    sendRequest(host, PORT_NUM, request);
    // std::string http_response_string = sendRequest(host, PORT_NUM, request);
    // vilslog::printf("LLRPController::logout() http_response_string[%s]\n", http_response_string.c_str());

    return true;
}

bool LLRPController::get_cookie(const char * host, const char * port, const char * request, ZebraCookieStruct * cookie)
{
    std::string http_response_string = sendRequest(host, port, request);

    //
    //
    // int cfd; // Socket 檔案描述符 (File Descriptor)
    // int gaiStatus; // getaddrinfo 狀態碼
    // struct addrinfo hints; // hints 參數，設定 getaddrinfo() 的回傳方式
    // struct addrinfo *result; // getaddrinfo() 執行結果的 addrinfo 結構指標
    // char response[0xfff]; // 回應訊息
    // size_t n = 0;
    //
    // // 以 memset 清空 hints 結構
    // memset(&hints, 0, sizeof(struct addrinfo));
    // //hints.ai_family = AF_UNSPEC; // 使用 IPv4 or IPv6
    // hints.ai_family = AF_INET; // 使用 IPv4 or IPv6
    // hints.ai_socktype = SOCK_STREAM; // 串流 Socket
    // hints.ai_flags = AI_NUMERICSERV; // 將 getaddrinfo() 第 2 參數 (PORT_NUM) 視為數字
    // //hints.ai_flags = AI_PASSIVE;
    //
    // // 以 getaddrinfo 透過 DNS，取得 addrinfo 鏈結串列 (Linked List)
    // // 以從中取得 Host 的 IP 位址
    // if ((gaiStatus = getaddrinfo(host, port, &hints, &result)) != 0)
    // {
    //     errPrint((char *) gai_strerror(gaiStatus));
    //     return false;
    // }
    //
    // //分別以 domain, type, protocol 建立 socket 檔案描述符
    // cfd = socket(result->ai_family, result->ai_socktype, 0);
    //
    // if ( cfd == -1 )
    // {
    //     errPrint((char *) gai_strerror(gaiStatus));
    //     return false;
    // }
    //
    // //================
    // // // 以 socket 檔案描述符 (cfd), addr, addrlen 進行連線
    // // // 其中，result->ai_addr 為 gai 取得之 通用 socket 位址結構 -- sockaddr
    // if (connect(cfd, result->ai_addr, result->ai_addrlen) < 0)
    // {
    //     errPrint("Connect");
    //     return false;
    // }
    //
    // // 釋放 getaddrinfo (Linked List) 記憶體空間
    // freeaddrinfo(result);
    // result = NULL;
    //
    // // 格式化輸出請求訊息
    // printf("----------\nRequest:\n----------\n%s\n", request);
    //
    // // 發送請求
    // if (send(cfd, request, strlen(request), 0) <= 0)
    // {
    //     errPrint("Send");
    //     return false;
    // }
    //
    // // 接收回應
    // if ((n = recv(cfd, response, 0xfff, 0)) <= 0)
    // {
    //     errPrint("Receive");
    //     return false;
    // }
    //
    // // 格式化輸出回應訊息
    // // vilslog::printf("----------\nResponse:\n----------\n%s\n", response);
    // vilslog::printf("----------\nResponse:\n----------\n");
    //
    // int header_end_index = 0;
    // std::list<HttpHeader*> * httpHeaderlist = NULL;
    // httpHeaderlist = parse_http_header(response, n);
    //
    // if (httpHeaderlist != NULL)
    // {
    //     for (std::list<HttpHeader *>::iterator header = httpHeaderlist->begin(); header != httpHeaderlist->end(); header++)
    //     {
    //         vilslog::printf("%s:%s\n", (*header)->parameter.c_str(), (*header)->value.c_str());
    //
    //         if ((*header)->parameter.compare("header_end_index") == 0)
    //         {
    //             header_end_index = atoi((*header)->value.c_str());
    //         }
    //     }
    // }
    //
    // std::string content = response + header_end_index;

    // vilslog::printf("http_response_string[%s]\n", http_response_string.c_str());


    tree<htmlcxx::HTML::Node> tr;
    htmlcxx::HTML::ParserDom parser;
    //parser.parse(content);
    //tr = parser.getTree();
    tree<htmlcxx::HTML::Node> dom = parser.parseTree(http_response_string, 1);
    //cout << tr << endl;

    //Dump all links in the tree
    tree<htmlcxx::HTML::Node>::iterator it = dom.begin();
    tree<htmlcxx::HTML::Node>::iterator end = dom.end();
    // int iFound = 0;

    // cookie->RMSessionID = "84:24:8D:80:16:83:1589794543:A";

    for (; it != end; ++it)
    {
        if ( it->isTag() )
        {
            // vilslog::printf("tag[%s]\n", it->tagName().c_str());

            if ( it->tagName().compare("g3:sessionID") == 0 )
            {
                ++it;
                // vilslog::printf("g3:sessionID:value[%s]\n", it->tagName().c_str());
                cookie->RMSessionID = it->tagName();
            }
            else
            if ( it->tagName().compare("g1:name") == 0 )
            {
                ++it;
                // vilslog::printf("g1:name:value[%s]\n", it->tagName().c_str());
            }


        }
//        else
//        if ( it->isComment() )
//        {
//            vilslog::printf("Comment[%s]\n", it->tagName().c_str());
//        }
//        else
//        {
//            vilslog::printf("value[%s]\n", it->tagName().c_str());
//        }

        //it->parseAttributes();
        //
        // std::map<std::string, std::string> attributes = it->attributes();
        //
        //
        // for(auto itmap = attributes.cbegin(); itmap != attributes.cend(); ++itmap)
        // {
        //     std::cout << itmap->first << " -=- " << itmap->second << "\n";
        // }

        // if ( it->isTag() )
        // {
        //     vilslog::printf("tag %s\n", it->string().c_str());
        //
        //     //
        //
        //     //cout << it->attribute("g3").second << " : "<< it->attribute("g1").second << endl;
        // }
        // else
//        if ( it->isComment() )
//        {
//            vilslog::printf("Comment %s\n", it->tagName().c_str());
//        }



    }



    // if (httpHeaderlist != NULL)
    // {
    //     clear_HttpHeader_list(httpHeaderlist);
    //     delete httpHeaderlist;
    // }
    //
    // // 半雙工關閉 TCP Socket 連線
    // // (i.e., 關閉寫入)
    // shutdown(cfd, SHUT_WR);
    // close(cfd);

    return true;
}

std::string LLRPController::sendRequest(const char * host, const char * port, const char * request)
{
    int cfd; // Socket 檔案描述符 (File Descriptor)
    int gaiStatus; // getaddrinfo 狀態碼
    struct addrinfo hints; // hints 參數，設定 getaddrinfo() 的回傳方式
    struct addrinfo *result; // getaddrinfo() 執行結果的 addrinfo 結構指標
    char response[0xfff]; // 回應訊息
    size_t n = 0;

    // 以 memset 清空 hints 結構
    memset(&hints, 0, sizeof(struct addrinfo));
    //hints.ai_family = AF_UNSPEC; // 使用 IPv4 or IPv6
    hints.ai_family = AF_INET; // 使用 IPv4 or IPv6
    hints.ai_socktype = SOCK_STREAM; // 串流 Socket
    hints.ai_flags = AI_NUMERICSERV; // 將 getaddrinfo() 第 2 參數 (PORT_NUM) 視為數字

    // 以 getaddrinfo 透過 DNS，取得 addrinfo 鏈結串列 (Linked List)
    // 以從中取得 Host 的 IP 位址
    if ((gaiStatus = getaddrinfo(host, port, &hints, &result)) != 0)
    {
        errPrint((char *) gai_strerror(gaiStatus));
        return "";
    }


    // 分別以 domain, type, protocol 建立 socket 檔案描述符
    cfd = socket(result->ai_family, result->ai_socktype, 0);

    if ( cfd == 0 )
    {
        errPrint((char *) gai_strerror(gaiStatus));
        return "";
    }

    // 以 socket 檔案描述符 (cfd), addr, addrlen 進行連線
    // 其中，result->ai_addr 為 gai 取得之 通用 socket 位址結構 -- sockaddr
    if (connect(cfd, result->ai_addr, result->ai_addrlen) < 0)
    {
        errPrint("Connect");
        return "";
    }


    // 釋放 getaddrinfo (Linked List) 記憶體空間
    freeaddrinfo(result);
    result = NULL;

    // 格式化輸出請求訊息
    // vilslog::printf("----------\nRequest:\n----------\n%s\n", request);

    // 發送請求
    if (send(cfd, request, strlen(request), 0) <= 0)
    {
        errPrint("Send");
        return "";
    }

    // 接收回應
    if ((n = recv(cfd, response, 0xfff, 0)) <= 0)
    {
        errPrint("Receive");
        return "";
    }


    // 格式化輸出回應訊息
    // printf("----------\nResponse:\n----------\n%s\n", response);

/*
    ----------
    Response:
    ----------
    HTTP/1.1 200 OK
    Content-Type: text/plain;charset=utf-8
    X-Cloud-Trace-Context: 252abfff9e7faacb4ddeb8eefb54770b;o=1
    Date: Fri, 02 Aug 2019 07:37:59 GMT
    Server: Google Frontend
    Content-Length: 63

    {  "name": "projects/smimsvils/messages/5888366962163071374"}
*/


    int header_end_index = 0;
    std::list<HttpHeader*> * httpHeaderlist = NULL;
    httpHeaderlist = parse_http_header(response, n);

    if (httpHeaderlist != NULL)
    {
        for (std::list<HttpHeader *>::iterator header = httpHeaderlist->begin(); header != httpHeaderlist->end(); header++)
        {
            //vilslog::printf("%s:%s\n", (*header)->parameter.c_str(), (*header)->value.c_str());

            if ((*header)->parameter.compare("header_end_index") == 0)
            {
                header_end_index = atoi((*header)->value.c_str());
            }
        }
    }

    std::string content = response + header_end_index;

    //vilslog::printf("content.length[%d] content[%s]\n", content.length(), content.c_str());

    // string::size_type found = content.find("{", 0);
    // if (found != string::npos)
    // {
    //     std::string http_response_json = content.substr(found);
    //
    //     //vilslog::printf ("http_response_json.length[%d]\n", http_response_json.length());
    //     //printf ("\n-------------------\n%s\n-------------------\n", http_response_json.c_str());
    //
    //     Json::Reader * reader = new Json::Reader();
    //     Json::Value JSONresult;
    //     bool parsingSuccessful = false;
    //
    //     try{
    //         parsingSuccessful = reader->parse( http_response_json.c_str(), JSONresult);
    //
    //         if (parsingSuccessful)
    //         {
    //
    //         }
    //     }
    //     catch(exception exc)
    //     {
    //         vilslog::printf ("SyncERPService::sendRequest() reader->parse() catch exception[%s]\n", exc.what());
    //     }
    //
    //     delete reader;
    // }

    if (httpHeaderlist != NULL)
    {
        clear_HttpHeader_list(httpHeaderlist);
        delete httpHeaderlist;
    }

    // 半雙工關閉 TCP Socket 連線
    // (i.e., 關閉寫入)
    shutdown(cfd, SHUT_WR);
    close(cfd);
    return content;
}

std::list<HttpHeader*> * LLRPController::parse_http_header(const char * response, size_t n)
{
    // HTTP/1.1 200 OK
    // Date: Mon, 18 May 2020 06:54:00 GMT
    // Server: Apache/2.4.3 (Unix) mod_wsgi/3.4 Python/2.7.3
    // X-Frame-Options: SAMEORIGIN
    // Content-Length: 348
    // Content-Type: text/xml

    char buffer[4096];
    std::list<HttpHeader*> * httpHeaderlist = NULL;

    memset(buffer, 0, 4096);
    int datalen = get_line(response, (int)n, 0, buffer, 4096);
    buffer[datalen] = 0;

    if (response[0] == 'H' && response[1] == 'T' && response[2] == 'T' && response[3] == 'P')
    {
        httpHeaderlist = new std::list<HttpHeader*>();
    }
    else
    {
        return NULL;
    }

    int start_idx = 0;
    bool bFinished = false;
    while(!bFinished)
    {
        memset(buffer, 0, 4096);
        int datalen = get_line(response, (int)n, start_idx, buffer, 4096);
        buffer[datalen] = 0;

        // pair 屬性名稱: 屬性值\r\n
        std::string bufString = buffer;
        string::size_type loc = bufString.find(":", 0);
        if (loc != string::npos)
        {
            HttpHeader * header = new HttpHeader();

            header->parameter = bufString.substr(0, loc);
            header->value = bufString.substr(loc+2, bufString.length() - 2 - 2);

            // vilslog::printf ("PushNotificationService::parse_http_header() parameter[%s] value[%s]\n",
            //     header->parameter.c_str(), header->value.c_str());

            httpHeaderlist->push_back(header);
        }

        start_idx += datalen;

        if(datalen == 0)
        {
            HttpHeader * header = new HttpHeader();
            header->parameter = "header_end_index";
            header->value = std::to_string(start_idx);
            httpHeaderlist->push_back(header);

            // vilslog::printf ("PushNotificationService::parse_http_header() parameter[%s] value[%s]\n",
            //     header->parameter.c_str(), header->value.c_str());

            bFinished = true;
        }
        else
        if(datalen == 2)
        {
            // header end \r\n\r\n
            HttpHeader * header = new HttpHeader();
            header->parameter = "header_end_index";
            header->value = std::to_string(start_idx);
            httpHeaderlist->push_back(header);

            // vilslog::printf ("PushNotificationService::parse_http_header() parameter[%s] value[%s]\n",
            //     header->parameter.c_str(), header->value.c_str());

            bFinished = true;
        }
        else
        {
            //vilslog::printf ("SyncERPService::parse_http_header() buffer[%s]\n", buffer);
        }
    }
    return httpHeaderlist;
}

int LLRPController::get_line(const char * databuffer, int data_len, int start_idx, char * linebuffer, int buffer_len)
{
    int length = 0;
    bool bFound = false;
    // find 0x10 0x13
    for (int i = start_idx; i < data_len; i++)
    {
        if(length < buffer_len)
        {
            linebuffer[length] = databuffer[i];
            length++;
        }

        if (databuffer[i] == '\r')
        {
            if ((i+1) < (data_len))
            {
                if (databuffer[i+1] == '\n')
                {
                    linebuffer[length] = databuffer[i];
                    length++;
                    bFound = true;
                }
            }
        }

        if(bFound)
            break;

    }

    return length;
}

void LLRPController::errPrint(const char *reason)
{
    std::string buff = reason ? reason : strerror(errno);
    printf("Error: %s", buff.c_str());
}
