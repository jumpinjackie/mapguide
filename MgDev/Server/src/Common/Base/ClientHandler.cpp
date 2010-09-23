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
#include "ClientHandler.h"
#include "Connection.h"
#include "LogManager.h"
#include "ServerManager.h"
#include "ServerStreamData.h"

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor
/// </summary>
MgClientHandler::MgClientHandler(const ACE_SOCK_Stream &stream, ACE_Reactor* pReactor, ACE_Message_Queue<ACE_MT_SYNCH>* pMessageQueue) :
    m_SockStream(stream),
    m_pMessageQueue(pMessageQueue),
    m_Status(hsIdle)
{
    // Set the reactor
    reactor(pReactor);

    // Increment self-refcount to account for ACE reactor.  Refcount is decremented in handle_close
    SAFE_ADDREF(this);

    m_pConnection = new MgConnection();

    MgServerManager* pServerManager = MgServerManager::GetInstance();
    if(pServerManager)
    {
        pServerManager->IncrementActiveConnections();
    }
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Default constructor for Ptr<> template
/// </summary>
MgClientHandler::MgClientHandler()
:
    m_SockStream(ACE_INVALID_HANDLE),
    m_pMessageQueue(NULL),
    m_Status(hsIdle),
    m_pConnection(NULL)
{
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor
/// </summary>
MgClientHandler::~MgClientHandler()
{
    m_pMessageQueue = NULL;

    if (m_pConnection != NULL)
    {
        MgConnection::SetCurrentConnection(NULL);
        delete m_pConnection;
        m_pConnection = NULL;
    }

    MgServerManager* pServerManager = MgServerManager::GetInstance();
    if(pServerManager)
    {
        pServerManager->DecrementActiveConnections();
    }
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// This method registers the handler for the READ_MASK.
/// </summary>
INT32 MgClientHandler::Initialize()
{
    INT32 nResult = 0;

    MgLogManager* pMan = MgLogManager::GetInstance();
    if(pMan->IsTraceLogEnabled())
    {
        ACE_INET_Addr addr;
        m_SockStream.get_local_addr(addr);

        ACE_TCHAR buffer[255];
        addr.addr_to_string(buffer, 255);
        ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgClientHandler::Initialize() - Address: %s\n"), buffer));

        STRING temp;
        temp = L"MgClientHandler::Initialize() - Address: ";
        temp += MG_TCHAR_TO_WCHAR(buffer);
        MG_LOG_TRACE_ENTRY(temp);
    }

    nResult = reactor()->register_handler(this, ACE_Event_Handler::READ_MASK);

    return nResult;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// ACE_Event_Handler method that is called to handle registered events.
/// </summary>
int MgClientHandler::handle_input(ACE_HANDLE handle)
{
    HandlerStatus currentStatus = MgClientHandler::hsError;

    // Switch to "pending" if the current state is "idle".
    {
        ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, -1));

        currentStatus = m_Status;

        if (MgClientHandler::hsIdle == m_Status)
        {
            m_Status = MgClientHandler::hsPending;
        }
    }

    INT32 ret = -1;

//    ACE_DEBUG ((LM_DEBUG, "(%t) MgClientHandler::handle_input()\n"));

    MgServerManager* pServerManager = MgServerManager::GetInstance();

    // Determine if data is coming from the client port
    ACE_INET_Addr addr;
    m_SockStream.get_local_addr(addr);
    bool bFromClientPort = ( addr.get_port_number() == pServerManager->GetClientPort() );

    if ( pServerManager->IsOnline() || !bFromClientPort )
    {
        switch ( currentStatus )
        {
            case ( MgClientHandler::hsIdle ) :
                ret = ProcessInput(handle);
                break;

            case ( MgClientHandler::hsBusy ) :
            case ( MgClientHandler::hsQueued ) :
            case ( MgClientHandler::hsPending ) :
                //  alright, just return because we assume the workerthreads will handle the new input
                ret = 0;
                break;

            case ( MgClientHandler::hsClosed ) :    //  client has dropped connection so we should close
            case ( MgClientHandler::hsError ) :     //  this is bad, so we should just close
            default :                               //  just close if we get an unknown status
                ret = -1;
                break;
        };
    }
    else
    {
        Ptr<MgException> mgException;
        mgException = new  MgServerNotOnlineException(L"MgClientHandler.handle_input", __LINE__, __WFILE__, NULL, L"", NULL);

        Ptr<MgStream> stream;

        if ( (MgStreamHelper*)m_pStreamHelper == NULL )
        {
            m_pStreamHelper = new MgAceStreamHelper( handle );
        }

        stream = new MgMapGuideStream( m_pStreamHelper );

        // TODO: use locale from client connection
        mgException->GetExceptionMessage(MgResources::DefaultMessageLocale);
        mgException->GetDetails(MgResources::DefaultMessageLocale);
        mgException->GetStackTrace(MgResources::DefaultMessageLocale);

        stream->WriteResponseHeader(MgPacketParser::mecFailure, 1);
        stream->WriteObject(mgException);
        stream->WriteStreamEnd();

        m_SockStream.close_writer();
        m_SockStream.close_reader();
        m_SockStream.close();
    }

    return ret;
};

//////////////////////////////////////////////////////////////////
/// <summary>
/// Processes input on the given handle.
/// </summary>
INT32 MgClientHandler::ProcessInput(ACE_HANDLE handle)
{
    ACE_DEBUG((LM_DEBUG, "(%t) MgClientHandler::ProcessInput()\n"));
    INT32 ret = -1;

    MG_TRY()

    //  check for streamhelper
    if ( (MgStreamHelper*)m_pStreamHelper == NULL )
    {
        m_pStreamHelper = new MgAceStreamHelper( handle );
    }

    //  create a MgStreamData Object for our message queue
    MgStreamData* pData = NULL;
    ACE_Allocator* allocator = ACE_Allocator::instance();
    ACE_NEW_MALLOC_RETURN( pData,
        static_cast<MgServerStreamData*>(allocator->malloc(sizeof(MgServerStreamData))),
        MgServerStreamData( this, handle, m_pStreamHelper ), -1 );

    //ACE_NEW_RETURN( pData, MgServerStreamData( this, handle, m_pStreamHelper ), -1 );

    //  create the message block
    ACE_Message_Block* mb = NULL;
    ACE_NEW_RETURN( mb, ACE_Message_Block( pData ), -1 );

    //  set status so that other input callbacks are postponed
    this->SetStatus( MgClientHandler::hsQueued );

    //  queue the message
    m_pMessageQueue->enqueue( mb );

    //  all is well
    ret = 0;

    MG_CATCH_AND_RELEASE()

    return ret;
};

//////////////////////////////////////////////////////////////////
/// <summary>
/// ACE_Event_Handler method that is called when the handler is closed.
/// </summary>
int MgClientHandler::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask mask)
{
    if (m_Status == MgClientHandler::hsClosed)
        return 0;

    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgClientHandler::handle_close()\n")));
    MG_LOG_TRACE_ENTRY(L"MgClientHandler::handle_close()");

    mask = ACE_Event_Handler::ALL_EVENTS_MASK | ACE_Event_Handler::DONT_CALL;
    reactor()->remove_handler(this, mask);
    m_SockStream.close_writer();
    m_SockStream.close_reader();
    m_SockStream.close();

    MgServerManager* pServerManager = MgServerManager::GetInstance();
    if(pServerManager)
    {
        pServerManager->RemoveClientHandle(handle);
    }

    // Flag this handler as closed
    m_Status = MgClientHandler::hsClosed;

    // Decrement self refcount
    MgClientHandler* pThis = this;
    SAFE_RELEASE(pThis);

    return 0;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the current status of the MgClientHandler.
/// </summary>
MgClientHandler::HandlerStatus MgClientHandler::GetStatus()
{
    return m_Status;
};

//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the current status of the MgClientHandler.
/// </summary>
void MgClientHandler::SetStatus( MgClientHandler::HandlerStatus status )
{
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));
    m_Status = status;
};

//////////////////////////////////////////////////////////////////
/// <summary>
/// This method returns a pointer to the MgConnection associated with this handler.
/// </summary>
MgConnection* MgClientHandler::GetConnection()
{
    return m_pConnection;
}


MgStreamHelper* MgClientHandler::GetStreamHelper()
{
    return (MgStreamHelper*) m_pStreamHelper;
}


///----------------------------------------------------------------------------
/// <summary>
/// Disposes this object.
/// </summary>
///----------------------------------------------------------------------------
void MgClientHandler::Dispose()
{
    delete this;
}
