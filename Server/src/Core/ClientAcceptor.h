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

#ifndef MGCLIENTACCEPTOR_H
#define MGCLIENTACCEPTOR_H

#include "MapGuideCommon.h"
#include "ClientHandler.h"

class MgClientAcceptor : public ACE_Event_Handler
{
    ///////////////////////////////////////////////////////
    /// Methods
public:
    MgClientAcceptor(const ACE_INET_Addr &addr, ACE_Reactor* pReactor, ACE_Message_Queue<ACE_MT_SYNCH>* pMessageQueue);
    virtual ~MgClientAcceptor();

    // ACE_Event_Handler methods
    virtual int handle_input(ACE_HANDLE handle);
    virtual int handle_close(ACE_HANDLE handle, ACE_Reactor_Mask mask);
    virtual ACE_HANDLE get_handle() const { return m_SockAcceptor.get_handle(); }

    // Class methods
    INT32 Initialize();

    ///////////////////////////////////////////////////////
    /// Member data
private:
    const ACE_INET_Addr &m_Addr;
    ACE_Message_Queue<ACE_MT_SYNCH>* m_pMessageQueue;
    SOCK_Acceptor m_SockAcceptor;
};

#endif
