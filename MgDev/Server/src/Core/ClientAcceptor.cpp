//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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
#include "ClientAcceptor.h"
#include "ClientHandler.h"
#include "LogManager.h"

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor
/// </summary>
MgClientAcceptor::MgClientAcceptor(const ACE_INET_Addr &addr, ACE_Reactor* pReactor,
                                   ACE_Message_Queue<ACE_MT_SYNCH>* pMessageQueue) :
    m_Addr(addr),
    m_pMessageQueue(pMessageQueue)
{
    // Set the reactor
    reactor(pReactor);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor
/// </summary>
MgClientAcceptor::~MgClientAcceptor()
{
    handle_close(ACE_INVALID_HANDLE, 0);

    m_pMessageQueue = NULL;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// This method registers the handler for the ACCEPT_MASK.
/// </summary>
INT32 MgClientAcceptor::Initialize()
{
    INT32 nResult = 0;

    // Set a large backlog so INET will not block connections
    // so often.  It takes time to clean the port up and
    // make it available for other connections.
    if (m_SockAcceptor.open(m_Addr, 0, PF_INET, 32) != -1)
    {
        MgLogManager* pMan = MgLogManager::GetInstance();
        if(pMan->IsTraceLogEnabled())
        {
            ACE_TCHAR buffer[255];
            m_Addr.addr_to_string(buffer, 255);
            ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgClientAcceptor::Initialize() - Address: %s\n"), buffer));

            STRING temp;
            temp = L"MgClientAcceptor::Initialize() - Address: ";
            temp += MG_TCHAR_TO_WCHAR(buffer);
            MG_LOG_TRACE_ENTRY(temp);
        }

        nResult = reactor()->register_handler(this, ACE_Event_Handler::ACCEPT_MASK);
    }
    else
    {
        ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgClientAcceptor::Initialize() - FAILED\n")));
        nResult = -1;
    }

    return nResult;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// ACE_Event_Handler method that is called to handle registered events.
/// </summary>
int MgClientAcceptor::handle_input(ACE_HANDLE handle)
{
    ACE_UNUSED_ARG(handle);

    INT32 nResult = 0;

    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgClientAcceptor::handle_input()\n")));
    ACE_SOCK_Stream clientConnection;

    nResult = m_SockAcceptor.accept(clientConnection);

    if (nResult != -1)
    {
        Ptr<MgClientHandler> handler = new MgClientHandler(clientConnection, reactor(), m_pMessageQueue);
        if(NULL != (MgClientHandler*) handler)
        {
            nResult = handler->Initialize();
            if(-1 != nResult)
            {
                ACE_HANDLE clientHandle = handler->get_handle();

                MgServerManager* pServerManager = MgServerManager::GetInstance();
                if(pServerManager)
                {
                    pServerManager->AddClientHandle(clientHandle);
                }
            }
            else
            {
                handler->handle_close(ACE_INVALID_HANDLE, 0);
            }
        }
        else
        {
            nResult = -1;
        }
    }
    else
    {
        // Failed to accept socket connection
        MG_LOG_ERROR_ENTRY(L"Failed to accept socket connection.");
    }

    return nResult;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// ACE_Event_Handler method that is called when the handler is closed.
/// </summary>
int MgClientAcceptor::handle_close(ACE_HANDLE handle, ACE_Reactor_Mask mask)
{
    ACE_UNUSED_ARG(handle);
    ACE_UNUSED_ARG(mask);

    ACE_DEBUG ((LM_DEBUG, ACE_TEXT("(%t) MgClientAcceptor::handle_close()\n")));

    if(m_SockAcceptor.get_handle() != ACE_INVALID_HANDLE)
    {
        ACE_Reactor_Mask m = ACE_Event_Handler::ACCEPT_MASK | ACE_Event_Handler::DONT_CALL;
        reactor()->remove_handler(this, m);
        m_SockAcceptor.close();
    }

    return 0;
}
