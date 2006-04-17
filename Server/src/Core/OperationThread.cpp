//
//  Copyright (C) 2004-2006  Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#include "AceCommon.h"
#include "OperationThread.h"
#include "ClientHandler.h"
#include "ServiceHandlerFactory.h"
#include "Connection.h"
#include "ServerManager.h"
#include "SessionManager.h"
#include "LogManager.h"

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor
/// </summary>
MgOperationThread::MgOperationThread(ACE_Thread_Manager &tm, INT32 nThreads) :
    MgThreadBase(tm, nThreads)
{
    m_bActive = true;
    m_timer.reset();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// ACE_Task method
/// </summary>
int MgOperationThread::svc(void)
{
    INT32 nResult = 0;

    Ptr<MgException> mgException;
    try
    {
        while (m_bActive)
        {
            // Clear the user information for this operation thread.
            MgUserInformation::SetCurrentUserInfo(NULL);

            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%P|%t) MgOperationThread::svc() Ready\n")));

            ACE_Message_Block* messageBlock = NULL;

            while (getq(messageBlock) == -1)
            {
                INT32 nError = ACE_OS::last_error();

                if(nError == EINTR)
                {
                    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("  (%P|%t) Interrupted while waiting for message\n")));
                    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("  (%P|%t) MgOperationThread - Exiting thread\n")));
                    return 0;
                }
                else
                {
                    // There was an error
                    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("  (%P|%t) MgOperationThread - Exiting thread\n")));
                    ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT("%p\n"), ACE_TEXT("MgOperationThread::svc()")), -1);
                }
            }

            if(messageBlock)
            {
                MgServerStreamData* pData = dynamic_cast<MgServerStreamData*>(messageBlock->data_block());

                if(messageBlock->msg_type() == ACE_Message_Block::MB_STOP)
                {
                    m_bActive = false;

                    ACE_Message_Block* mb = new ACE_Message_Block(4);
                    if(mb)
                    {
                        mb->msg_type(ACE_Message_Block::MB_STOP);
                        putq(mb);
                    }
                }
                else if(messageBlock->msg_type() == ACE_Message_Block::MB_DATA)
                {
                    IMgServiceHandler::MgProcessStatus stat = ProcessMessage( messageBlock );

                    Ptr<MgClientHandler> pClientHandler = pData->GetClientHandler();

                    switch ( stat )
                    {
                        case ( IMgServiceHandler::mpsError ) :
                            //  We hit an error in processing.  An exception should have been
                            //  sent back to the client at some point.  If the client is an
                            //  HTTP agent, it will respond with a close control packet.  We will
                            //  try to process this incoming packet, but since there still may be junk
                            //  data on the wire, we will force the close of the client handler
                            //  if we are already in an error condition.                             
                            if ( pData->GetErrorFlag() )
                            {
                                // Remove the client handler from reactor.  This code will also
                                // be hit if the client handler has been torn down from the connection idle timer
                                // while we were waiting for additional requests from the client.                                   
                                pClientHandler->reactor()->remove_handler(
                                    pClientHandler->get_handle(), ACE_Event_Handler::READ_MASK );

                                //  reset the stream error flag
                                pData->SetErrorFlag( false );
                            }
                            else
                            {
                                //  Set the stream error flag.
                                pData->SetErrorFlag( true );

                                //  We have hit an error so an exception should have been sent back
                                //  to the client.  If the client is an HTTP agent, it will respond
                                //  with a close control packet.  Check to see if we have received
                                //  the packet.  If so, push it back on the queue.  If not, go
                                //  into idle state to wait for the next request.
                                if ( MgStreamHelper::mssDone == CheckStream( pClientHandler->GetStreamHelper() ) )
                                {
                                    //  set client handler status before queueing messageblock
                                    pClientHandler->SetStatus( MgClientHandler::hsQueued );

                                    //  Create a new messageBlock and put it on the queue
                                    MgServerStreamData* pStreamData = new MgServerStreamData( *pData );
                                    if(pStreamData)
                                    {
                                        //  create the message block
                                        ACE_Message_Block* mb = new ACE_Message_Block( pStreamData );
                                        if(mb)
                                        {
                                            putq( mb );
                                        }
                                    }
                                }
                                else
                                {
                                    pClientHandler->SetStatus( MgClientHandler::hsIdle );
                                }
                            }
                            break;

                        case ( IMgServiceHandler::mpsDone ) :
                            
                            // Do we still have a stream?
                            if (NULL == (MgClientHandler*) pClientHandler || MgClientHandler::hsClosed == pClientHandler->GetStatus())
                            {
                                break;
                            }

                            //  is there anything else on the stream?
                            if (MgStreamHelper::mssDone == CheckStream( pClientHandler->GetStreamHelper() ) )
                            {
                                //  set client handler status before queueing messageblock
                                pClientHandler->SetStatus( MgClientHandler::hsQueued );

                                //  Create a new messageBlock and put it on the queue
                                MgServerStreamData* pStreamData = new MgServerStreamData( *pData );
                                if(pStreamData)
                                {
                                    //  create the message block
                                    ACE_Message_Block* mb = new ACE_Message_Block( pStreamData );
                                    if(mb)
                                    {
                                        putq( mb );
                                    }
                                }
                            }
                            else
                            {
                                pClientHandler->SetStatus( MgClientHandler::hsIdle );
                            }
                            break;

                        case ( IMgServiceHandler::mpsClosed ) :
                            // Push an error on the handler and remove it from reactor
                            // to force a timely close
                            if (NULL != (MgClientHandler*) pClientHandler)
                            {
                                pClientHandler->reactor()->remove_handler(
                                pClientHandler->get_handle(), ACE_Event_Handler::READ_MASK);
                            }
                            break;

                        case ( IMgServiceHandler::mpsOther ) :
                            if (NULL != (MgClientHandler*) pClientHandler)
                            {
                                pClientHandler->SetStatus( MgClientHandler::hsBusy );
                            }
                            break;
                    }
                }

                //  Cleanup message block
                messageBlock->release();
                messageBlock = NULL;
            }
        }
    }
    catch (MgException* e)
    {
        MgServerManager* pServerManager = MgServerManager::GetInstance();

        ACE_DEBUG ((LM_ERROR, ACE_TEXT("(%P|%t) %W\n"), e->GetDetails(pServerManager->GetDefaultLocale()).c_str()));
        MG_LOG_EXCEPTION_ENTRY(e->GetMessage(pServerManager->GetDefaultLocale()).c_str(), e->GetStackTrace(pServerManager->GetDefaultLocale()).c_str());

        SAFE_RELEASE(e);

        nResult = -1;
    }
    catch (exception& e)
    {
        MgServerManager* pServerManager = MgServerManager::GetInstance();

        mgException = MgSystemException::Create(e, L"MgOperationThread.svc", __LINE__, __WFILE__);
        ACE_DEBUG ((LM_ERROR, ACE_TEXT("(%P|%t) %W\n"), mgException->GetDetails(pServerManager->GetDefaultLocale()).c_str()));
        MG_LOG_EXCEPTION_ENTRY(mgException->GetMessage(pServerManager->GetDefaultLocale()).c_str(), mgException->GetStackTrace(pServerManager->GetDefaultLocale()).c_str());

        nResult = -1;
    }
    catch (...)
    {
        MgServerManager* pServerManager = MgServerManager::GetInstance();

        mgException = new MgUnclassifiedException(L"MgOperationThread.svc", __LINE__, __WFILE__, NULL, L"", NULL);
        ACE_DEBUG ((LM_ERROR, ACE_TEXT("(%P|%t) %W\n"), mgException->GetDetails(pServerManager->GetDefaultLocale()).c_str()));
        MG_LOG_EXCEPTION_ENTRY(mgException->GetMessage(pServerManager->GetDefaultLocale()).c_str(), mgException->GetStackTrace(pServerManager->GetDefaultLocale()).c_str());

        nResult = -1;
    }

    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%P|%t) MgOperationThread - Exiting thread\n")));
    return nResult;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Processes a Data Message Block.
/// </summary>
IMgServiceHandler::MgProcessStatus MgOperationThread::ProcessMessage( ACE_Message_Block* pMB )
{
    ACE_DEBUG(( LM_DEBUG, ACE_TEXT( "  (%P|%t) MgOperationThread::ProcessMessage\n" )));

    IMgServiceHandler::MgProcessStatus stat = IMgServiceHandler::mpsError;

    Ptr<MgException> mgException;
    MgServerStreamData* pData = 0;
    try
    {
        ACE_ASSERT( pMB );
        if ( pMB )
        {
            pData = (MgServerStreamData*) pMB->data_block();
            Ptr<MgClientHandler> pHandler = pData->GetClientHandler();
            MgStreamHelper* pStreamHelper = pData->GetStreamHelper();

            // For pooled TCP/IP connections, the idle timeout handler may have removed
            // the client handler for this message block while we were waiting for
            // another request.  If the client handler is closed then
            // we should bail out of the processing loop because there is no longer an
            // active connection to read an operation from.
            //
            // Note:  No exception is thrown and no error is logged because stale connection
            // removal is considered standard operational behaviour.
            if (NULL == pData || NULL == pStreamHelper || MgClientHandler::hsClosed == pHandler->GetStatus())
            {
                if (NULL != pData)
                {
                    pData->SetErrorFlag(true);
                }

                return IMgServiceHandler::mpsError;
            }

            ACE_ASSERT ( pData && pStreamHelper && (MgClientHandler*) pHandler );

            if(pHandler->GetStatus() == MgClientHandler::hsQueued)
            {
                //  we're about to start processing, so set the handler to busy
                pHandler->SetStatus( MgClientHandler::hsBusy );

                //  get stream information
                if ( MgStreamParser::ParseStreamHeader( pData ) )
                {
                    //  get data information
                    if ( MgStreamParser::ParseDataHeader( pData ) )
                    {
                        bool keepParsing = true;

                        //  process data stream
                        while ( keepParsing )
                        {
                            MgPacketParser::MgPacketHeader pt = MgPacketParser::GetPacketHeader( pData );

                            switch ( pt )
                            {
                                case ( MgPacketParser::mphOperation ) :
                                    ACE_DEBUG( ( LM_DEBUG, ACE_TEXT( "  (%P|%t) OPERATION PACKET\n" )));
                                    stat = ProcessOperation( pData );
                                    keepParsing = false; // TODO: Remove this line when support for zero data size is added
                                    break;

                                case ( MgPacketParser::mphOperationResponse ) :
                                    ACE_DEBUG( ( LM_DEBUG, ACE_TEXT( "  (%P|%t) ERROR... OPERATION RESPONSE PACKET\n" )));
                                    stat = IMgServiceHandler::mpsError;
                                    keepParsing = false;
                                    break;

                                case ( MgPacketParser::mphArgumentSimple ) :
                                    ACE_DEBUG( ( LM_DEBUG, ACE_TEXT( "  (%P|%t) ERROR... SIMPLE ARGUMENT PACKET\n" )));
                                    stat = IMgServiceHandler::mpsError;
                                    keepParsing = false;
                                    break;

                                case ( MgPacketParser::mphArgumentCollection ):
                                    ACE_DEBUG( ( LM_DEBUG, ACE_TEXT( "  (%P|%t) ERROR... COLLECTION ARGUMENT PACKET\n" )));
                                    stat = IMgServiceHandler::mpsError;
                                    keepParsing = false;
                                    break;

                                case ( MgPacketParser::mphArgumentBinaryStream ) :
                                    ACE_DEBUG( ( LM_DEBUG, ACE_TEXT( "  (%P|%t) ERROR... BINARY STREAM ARGUMENT PACKET\n" )));
                                    stat = IMgServiceHandler::mpsError;
                                    keepParsing = false;
                                    break;

                                case ( MgPacketParser::mphControl ):
                                    {
                                    ACE_DEBUG( ( LM_DEBUG, ACE_TEXT( "  (%P|%t) CONTROL PACKET\n" )));
                                    // Check for close packet
                                    MgControlPacket packet;
                                    MgPacketParser::GetControlPacket( pData, &packet );
                                    if (MgPacketParser::mciClose == packet.m_ControlID)
                                    {
                                        MgStreamParser::ParseEndHeader( pData );
                                        stat = IMgServiceHandler::mpsClosed;
                                    }
                                    keepParsing = false;
                                    }
                                    break;

                                case ( MgPacketParser::mphError ):
                                    ACE_DEBUG( ( LM_DEBUG, ACE_TEXT( "  (%P|%t) ERROR... ERROR PACKET\n" )));
                                    stat = IMgServiceHandler::mpsError;
                                    keepParsing = false;
                                    break;

                                default :
                                    ACE_DEBUG( ( LM_DEBUG, ACE_TEXT( "  (%P|%t) ERROR... Unknown Packet Type\n" )));
                                    stat = IMgServiceHandler::mpsError;
                                    keepParsing = false;
                                    break;
                            }
                        }

                        if ( stat != IMgServiceHandler::mpsError &&
                            stat != IMgServiceHandler::mpsClosed)
                        {
                            //  check for EndStream
                            if ( !MgStreamParser::ParseEndHeader( pData ) )
                            {
                                stat = IMgServiceHandler::mpsError;
                            }
                            else
                            {
                                stat = IMgServiceHandler::mpsDone;
                            }
                        }
                    }
                }
            }
        }
    }
    catch (MgException* e)
    {
        MgServerManager* pServerManager = MgServerManager::GetInstance();

        ACE_DEBUG ((LM_ERROR, ACE_TEXT("(%P|%t) %W\n"), e->GetDetails(pServerManager->GetDefaultLocale()).c_str()));
        MG_LOG_EXCEPTION_ENTRY(e->GetMessage(pServerManager->GetDefaultLocale()).c_str(), e->GetStackTrace(pServerManager->GetDefaultLocale()).c_str());

        SAFE_RELEASE(e);

        stat = IMgServiceHandler::mpsError;
    }
    catch (exception& e)
    {
        MgServerManager* pServerManager = MgServerManager::GetInstance();

        mgException = MgSystemException::Create(e, L"MgOperationThread.ProcessMessage", __LINE__, __WFILE__);
        ACE_DEBUG ((LM_ERROR, ACE_TEXT("(%P|%t) %W\n"), mgException->GetDetails(pServerManager->GetDefaultLocale()).c_str()));
        MG_LOG_EXCEPTION_ENTRY(mgException->GetMessage(pServerManager->GetDefaultLocale()).c_str(), mgException->GetStackTrace(pServerManager->GetDefaultLocale()).c_str());

        stat = IMgServiceHandler::mpsError;
    }
    catch (...)
    {
        MgServerManager* pServerManager = MgServerManager::GetInstance();

        mgException = new MgUnclassifiedException(L"MgOperationThread.ProcessMessage", __LINE__, __WFILE__, NULL, L"", NULL);
        ACE_DEBUG ((LM_ERROR, ACE_TEXT("(%P|%t) %W\n"), mgException->GetDetails(pServerManager->GetDefaultLocale()).c_str()));
        MG_LOG_EXCEPTION_ENTRY(mgException->GetMessage(pServerManager->GetDefaultLocale()).c_str(), mgException->GetStackTrace(pServerManager->GetDefaultLocale()).c_str());

        stat = IMgServiceHandler::mpsError;
    }

    //  if any message was processed without error, make sure the stream's
    //  error flag is cleared
    if ( pData != 0 && stat != IMgServiceHandler::mpsError )
        pData->SetErrorFlag( false );

    return stat;
};

//////////////////////////////////////////////////////////////////
/// <summary>
/// Processes an Operation.
/// </summary>
IMgServiceHandler::MgProcessStatus MgOperationThread::ProcessOperation( MgServerStreamData* pData )
{
    ACE_DEBUG( ( LM_DEBUG, ACE_TEXT( "  (%P|%t) MgOperationThread::ProcessOperation\n" )));

    ACE_Time_Value operationTime = 0;
    IMgServiceHandler::MgProcessStatus stat = IMgServiceHandler::mpsError;
    MgConnection* pConnection = NULL;

    Ptr<MgException> mgException;
    try
    {
        ACE_ASSERT( pData );
        if ( pData )
        {
            Ptr<MgClientHandler> pHandler;
            MgServerManager* serverManager = NULL;
            MgOperationPacket op;
            MgStreamHelper* pHelper = NULL;
            MgStreamHelper::MgStreamStatus streamStat = MgStreamHelper::mssDone;
            int nBytesRead = 0;

            // Get client handler
            pHandler = pData->GetClientHandler();
            ACE_ASSERT ( (MgClientHandler*) pHandler );

            // Get the Connection
            pConnection = pHandler->GetConnection();

            // Mark the connection as busy
            pConnection->SetBusy();

            // Reset the session ID.
            pConnection->SetSessionId(L"");

            // Update the Operations received and last usage time (Connection)
            pConnection->IncrementReceivedOperations();
            pConnection->UpdateLastUsageTime();

            // Update the operations received (Entire process)
            serverManager = MgServerManager::GetInstance();
            serverManager->IncrementReceivedOperations();

            // Get the stream helper
            pHelper = pData->GetStreamHelper();

            // Parse the stream
            if (MgStreamHelper::mssDone == streamStat)
            {
                streamStat = pHelper->GetUINT32(op.m_PacketHeader);
            }

            if (MgStreamHelper::mssDone == streamStat)
            {
                streamStat = pHelper->GetUINT32(op.m_PacketVersion);
            }

            if (MgStreamHelper::mssDone == streamStat)
            {
                nBytesRead += sizeof(UINT32);
                streamStat = pHelper->GetUINT32(op.m_ServiceID);
            }

            if (MgStreamHelper::mssDone == streamStat)
            {
                nBytesRead += sizeof(UINT32);
                streamStat = pHelper->GetUINT32(op.m_OperationID);
            }

            if (MgStreamHelper::mssDone == streamStat)
            {
                nBytesRead += sizeof(UINT32);
                streamStat = pHelper->GetUINT32(op.m_OperationVersion);
            }

            if (MgStreamHelper::mssDone == streamStat)
            {
                nBytesRead += sizeof(UINT32);
                streamStat = pHelper->GetUINT32(op.m_NumArguments);
            }

            if ( MgStreamHelper::mssDone == streamStat )
            {
                nBytesRead += sizeof(UINT32);

                pData->SetDataRead( pData->GetDataRead() + nBytesRead );

                IMgServiceHandler* pServiceHandler = MgServiceHandlerFactory::Instance()->GetHandler( op.m_ServiceID, pData, op );
                if(pServiceHandler)
                {
                    MgConnection::SetCurrentConnection(pConnection);

                    m_timer.reset();
                    m_timer.start();

                    stat = pServiceHandler->ProcessOperation();

                    m_timer.stop();
                    m_timer.elapsed_time(operationTime);

                    delete pServiceHandler;
                    pServiceHandler = NULL;

                    MgConnection::SetCurrentConnection(NULL);
                }
            }

            // We increment operations processed for successful operations only.
            if (IMgServiceHandler::mpsDone == stat)
            {
                INT32 opTime = operationTime.msec();

                pConnection->IncrementProcessedOperations();
                pConnection->SetCurrentOperationTime(opTime);

                serverManager->IncrementProcessedOperations();
                serverManager->IncrementOperationTime(opTime);
            }

            // Update the session with the current operation information if necessary.
            if (!pConnection->GetSessionId().empty())
            {
                MgSessionManager::UpdateCurrentOperationInfo(*pConnection);
            }

            // Mark the connection as no longer busy
            pConnection->ClearBusy();
        }
    }
    catch (MgException* e)
    {
        MgServerManager* pServerManager = MgServerManager::GetInstance();

        ACE_DEBUG ((LM_ERROR, ACE_TEXT("(%P|%t) %W\n"), e->GetDetails(pServerManager->GetDefaultLocale()).c_str()));
        MG_LOG_EXCEPTION_ENTRY(e->GetMessage(pServerManager->GetDefaultLocale()).c_str(), e->GetStackTrace(pServerManager->GetDefaultLocale()).c_str());

        SAFE_RELEASE(e);

        // Clear the connection busy state so that our idle thread can remove it
        if(pConnection)
        {
            // Mark the connection as no longer busy
            pConnection->ClearBusy();
        }

        stat = IMgServiceHandler::mpsError;
    }
    catch (exception& e)
    {
        MgServerManager* pServerManager = MgServerManager::GetInstance();

        mgException = MgSystemException::Create(e, L"MgOperationThread.ProcessOperation", __LINE__, __WFILE__);
        ACE_DEBUG ((LM_ERROR, ACE_TEXT("(%P|%t) %W\n"), mgException->GetDetails(pServerManager->GetDefaultLocale()).c_str()));
        MG_LOG_EXCEPTION_ENTRY(mgException->GetMessage(pServerManager->GetDefaultLocale()).c_str(), mgException->GetStackTrace(pServerManager->GetDefaultLocale()).c_str());

        // Clear the connection busy state so that our idle thread can remove it
        if(pConnection)
        {
            // Mark the connection as no longer busy
            pConnection->ClearBusy();
        }

        stat = IMgServiceHandler::mpsError;
    }
    catch (...)
    {
        MgServerManager* pServerManager = MgServerManager::GetInstance();

        mgException = new MgUnclassifiedException(L"MgOperationThread.ProcessOperation", __LINE__, __WFILE__, NULL, L"", NULL);
        ACE_DEBUG ((LM_ERROR, ACE_TEXT("(%P|%t) %W\n"), mgException->GetDetails(pServerManager->GetDefaultLocale()).c_str()));
        MG_LOG_EXCEPTION_ENTRY(mgException->GetMessage(pServerManager->GetDefaultLocale()).c_str(), mgException->GetStackTrace(pServerManager->GetDefaultLocale()).c_str());

        // Clear the connection busy state so that our idle thread can remove it
        if(pConnection)
        {
            // Mark the connection as no longer busy
            pConnection->ClearBusy();
        }

        stat = IMgServiceHandler::mpsError;
    }

    return stat;
};

//////////////////////////////////////////////////////////////////
/// <summary>
/// Checks for data in the given MgStreamHelper
/// </summary>
MgStreamHelper::MgStreamStatus MgOperationThread::CheckStream( MgStreamHelper* pHelper )
{
    UINT8 dummy = 0;

    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssError;

    //  check to see if socket is alive...  TODO:  may be a better way to determine if the
    //  socket has not been closed
    Ptr<MgException> mgException;
    try
    {
        stat = pHelper->GetUINT8( dummy, false, true );
    }
    catch (MgException* e)
    {
        MgServerManager* pServerManager = MgServerManager::GetInstance();

        ACE_DEBUG ((LM_ERROR, ACE_TEXT("(%P|%t) %W\n"), e->GetDetails(pServerManager->GetDefaultLocale()).c_str()));
        MG_LOG_EXCEPTION_ENTRY(e->GetMessage(pServerManager->GetDefaultLocale()).c_str(), e->GetStackTrace(pServerManager->GetDefaultLocale()).c_str());

        SAFE_RELEASE(e);

        stat = MgStreamHelper::mssError;
    }
    catch (exception& e)
    {
        MgServerManager* pServerManager = MgServerManager::GetInstance();

        mgException = MgSystemException::Create(e, L"MgOperationThread.CheckStream", __LINE__, __WFILE__);
        ACE_DEBUG ((LM_ERROR, ACE_TEXT("(%P|%t) %W\n"), mgException->GetDetails(pServerManager->GetDefaultLocale()).c_str()));
        MG_LOG_EXCEPTION_ENTRY(mgException->GetMessage(pServerManager->GetDefaultLocale()).c_str(), mgException->GetStackTrace(pServerManager->GetDefaultLocale()).c_str());

        stat = MgStreamHelper::mssError;
    }
    catch (...)
    {
        MgServerManager* pServerManager = MgServerManager::GetInstance();

        mgException = new MgUnclassifiedException(L"MgOperationThread.CheckStream", __LINE__, __WFILE__, NULL, L"", NULL);
        ACE_DEBUG ((LM_ERROR, ACE_TEXT("(%P|%t) %W\n"), mgException->GetDetails(pServerManager->GetDefaultLocale()).c_str()));
        MG_LOG_EXCEPTION_ENTRY(mgException->GetMessage(pServerManager->GetDefaultLocale()).c_str(), mgException->GetStackTrace(pServerManager->GetDefaultLocale()).c_str());

        stat = MgStreamHelper::mssError;
    }

    return stat;
}
