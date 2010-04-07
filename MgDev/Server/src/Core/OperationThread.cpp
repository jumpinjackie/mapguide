//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#include "MapGuideCommon.h"
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
int MgOperationThread::svc()
{
    INT32 nResult = 0;

    MG_TRY()

    {
        while (m_bActive)
        {
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgOperationThread::svc() Ready\n")));

            ACE_Message_Block* messageBlock = NULL;

            while (getq(messageBlock) == -1)
            {
                INT32 nError = ACE_OS::last_error();

                if(nError == EINTR)
                {
                    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("  (%t) Interrupted while waiting for message\n")));
                    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("  (%t) MgOperationThread - Exiting thread\n")));
                    return 0;
                }
                else
                {
                    // There was an error
                    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("  (%t) MgOperationThread - Exiting thread\n")));
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

                    if (NULL != pClientHandler.p)
                    {
                        switch ( stat )
                        {
                            case ( IMgServiceHandler::mpsDone ) :
                            {
                                break;
                            }

                            case ( IMgServiceHandler::mpsOther ) :
                            {
                                pClientHandler->SetStatus( MgClientHandler::hsBusy );
                                break;
                            }

                            //  We hit an error in processing.  An exception should have been
                            //  sent back to the client at some point.  If the client is an
                            //  HTTP agent, it will respond with a close control packet.  We will NOT
                            //  try to process this incoming packet, but since there still may be junk
                            //  data on the wire, we will force the close of the client handler
                            //  if we are already in an error condition.
                            case ( IMgServiceHandler::mpsError ) :
                            case ( IMgServiceHandler::mpsClosed ) :
                            default:
                            {
                                ACE_Reactor* pReactor = pClientHandler->reactor();
                                ACE_HANDLE handle = pClientHandler->get_handle();
                                pClientHandler = NULL;

                                //  Cleanup message block
                                messageBlock->release();
                                messageBlock = NULL;

                                // Remove the client handler from reactor.  This code will also
                                // be hit if the client handler has been torn down from the connection idle timer
                                // while we were waiting for additional requests from the client.
                                pReactor->remove_handler(handle, ACE_Event_Handler::READ_MASK);
                                break;
                            }
                        }
                    }
                }

                //  Cleanup message block
                if (messageBlock != NULL)
                {
                    messageBlock->release();
                    messageBlock = NULL;
                }
            }
        }
    }

    MG_CATCH(L"MgOperationThread.svc")

    if (mgException != NULL)
    {
        MgServerManager* serverManager = MgServerManager::GetInstance();
        STRING locale = (NULL == serverManager) ?
            MgResources::DefaultMessageLocale : serverManager->GetDefaultMessageLocale();
        STRING message = mgException->GetExceptionMessage(locale);
        STRING details = mgException->GetDetails(locale);
        STRING stackTrace = mgException->GetStackTrace(locale);

        ACE_DEBUG((LM_ERROR, ACE_TEXT("(%t) %W\n"), details.c_str()));

        // Log that the operation thread terminated abnormally
        message += L" - Operation thread has terminated abnormally!";
        MG_LOG_EXCEPTION_ENTRY(message.c_str(), stackTrace.c_str());

        nResult = -1;
    }

    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgOperationThread - Exiting thread\n")));

    return nResult;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Processes a Data Message Block.
/// </summary>
IMgServiceHandler::MgProcessStatus MgOperationThread::ProcessMessage( ACE_Message_Block* pMB )
{
    ACE_DEBUG(( LM_DEBUG, ACE_TEXT( "  (%t) MgOperationThread::ProcessMessage\n" )));

    IMgServiceHandler::MgProcessStatus stat = IMgServiceHandler::mpsError;
    MgServerStreamData* pData = NULL;

    MG_TRY()

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
                        //  process data stream
                        MgPacketParser::MgPacketHeader pt = MgPacketParser::GetPacketHeader( pData );

                        switch ( pt )
                        {
                            case ( MgPacketParser::mphOperation ) :
                                ACE_DEBUG( ( LM_DEBUG, ACE_TEXT( "  (%t) OPERATION PACKET\n" )));
                                stat = ProcessOperation( pData );
                                break;

                            case ( MgPacketParser::mphControl ):
                            {
                                ACE_DEBUG( ( LM_DEBUG, ACE_TEXT( "  (%t) CONTROL PACKET\n" )));
                                // Check for close packet
                                MgControlPacket packet;
                                MgPacketParser::GetControlPacket( pData, &packet );
                                if (MgPacketParser::mciClose == packet.m_ControlID)
                                {
                                    MgStreamParser::ParseEndHeader( pData );
                                    stat = IMgServiceHandler::mpsClosed;
                                }
                                break;
                            }

                            case ( MgPacketParser::mphOperationResponse ) :
                                ACE_DEBUG( ( LM_DEBUG, ACE_TEXT( "  (%t) ERROR... OPERATION RESPONSE PACKET\n" )));
                                break;

                            case ( MgPacketParser::mphArgumentSimple ) :
                                ACE_DEBUG( ( LM_DEBUG, ACE_TEXT( "  (%t) ERROR... SIMPLE ARGUMENT PACKET\n" )));
                                break;

                            case ( MgPacketParser::mphArgumentCollection ):
                                ACE_DEBUG( ( LM_DEBUG, ACE_TEXT( "  (%t) ERROR... COLLECTION ARGUMENT PACKET\n" )));
                                break;

                            case ( MgPacketParser::mphArgumentBinaryStream ) :
                                ACE_DEBUG( ( LM_DEBUG, ACE_TEXT( "  (%t) ERROR... BINARY STREAM ARGUMENT PACKET\n" )));
                                break;

                            case ( MgPacketParser::mphError ):
                                ACE_DEBUG( ( LM_DEBUG, ACE_TEXT( "  (%t) ERROR... ERROR PACKET\n" )));
                                break;

                            default :
                                ACE_DEBUG( ( LM_DEBUG, ACE_TEXT( "  (%t) ERROR... Unknown Packet Type\n" )));
                                break;
                        }
                    }
                }
            }
        }
    }

    MG_CATCH(L"MgOperationThread.ProcessMessage")

    if (NULL != mgException)
    {
        // The stream may contain garbage when the connection is dropped for
        // some reason. Suppress this exception to reduce noise in the log file.
        if (!mgException->IsOfClass(Foundation_Exception_MgInvalidStreamHeaderException))
        {
            MgServerManager* serverManager = MgServerManager::GetInstance();
            STRING locale = (NULL == serverManager) ?
                MgResources::DefaultMessageLocale : serverManager->GetDefaultMessageLocale();
            STRING message = mgException->GetExceptionMessage(locale);
            STRING details = mgException->GetDetails(locale);
            STRING stackTrace = mgException->GetStackTrace(locale);

            ACE_DEBUG((LM_ERROR, ACE_TEXT("(%t) %W\n"), details.c_str()));
            MG_LOG_EXCEPTION_ENTRY(message.c_str(), stackTrace.c_str());
        }

        stat = IMgServiceHandler::mpsError;
    }

    return stat;
};

//////////////////////////////////////////////////////////////////
/// <summary>
/// Processes an Operation.
/// </summary>
IMgServiceHandler::MgProcessStatus MgOperationThread::ProcessOperation( MgServerStreamData* pData )
{
    ACE_DEBUG( ( LM_DEBUG, ACE_TEXT( "  (%t) MgOperationThread::ProcessOperation\n" )));

    IMgServiceHandler::MgProcessStatus stat = IMgServiceHandler::mpsError;
    MgConnection* pConnection = NULL;

    MG_TRY()

    {
        ACE_Time_Value operationTime(0);
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
                time_t opTime = operationTime.sec();

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

    MG_CATCH(L"MgOperationThread.ProcessOperation")

    if (mgException != NULL)
    {
        MgServerManager* serverManager = MgServerManager::GetInstance();
        STRING locale = (NULL == serverManager) ?
            MgResources::DefaultMessageLocale : serverManager->GetDefaultMessageLocale();
        STRING message = mgException->GetExceptionMessage(locale);
        STRING details = mgException->GetDetails(locale);
        STRING stackTrace = mgException->GetStackTrace(locale);

        ACE_DEBUG((LM_ERROR, ACE_TEXT("(%t) %W\n"), details.c_str()));
        MG_LOG_EXCEPTION_ENTRY(message.c_str(), stackTrace.c_str());

        // Clear the connection busy state so that our idle thread can remove it
        if (NULL != pConnection)
        {
            // Mark the connection as no longer busy
            pConnection->ClearBusy();
        }

        stat = IMgServiceHandler::mpsError;
    }

    return stat;
}
