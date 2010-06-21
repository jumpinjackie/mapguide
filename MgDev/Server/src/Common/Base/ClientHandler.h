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

#ifndef MGCLIENTHANDLER_H_
#define MGCLIENTHANDLER_H_

#include "ServerBaseDllExport.h"
#include "MapGuideCommon.h"

class MgConnection;

class MG_SERVER_BASE_API MgClientHandler : public ACE_Event_Handler, public MgGuardDisposable
{
    ///////////////////////////////////////////////////////
    /// Methods
public:
    MgClientHandler (const ACE_SOCK_Stream &stream, ACE_Reactor* pReactor, ACE_Message_Queue<ACE_MT_SYNCH>* pMessageQueue);
    MgClientHandler();
    virtual ~MgClientHandler();

    // ACE_Event_Handler methods
    virtual int handle_input(ACE_HANDLE handle);
    virtual int handle_close (ACE_HANDLE handle, ACE_Reactor_Mask mask);
    virtual ACE_HANDLE get_handle() const { return m_SockStream.get_handle(); }

    // Class methods
    INT32 Initialize();

    // enumerations
    enum HandlerStatus
    {
        hsError = -1,
        hsIdle,
        hsPending,
        hsQueued,
        hsBusy,
        hsClosed
    };

    // accessors
    MgConnection* GetConnection();
    HandlerStatus GetStatus();
    void SetStatus( HandlerStatus status );
    MgStreamHelper* GetStreamHelper();

protected:

     virtual void Dispose();


private:
    INT32 ProcessInput(ACE_HANDLE handle);

    ///////////////////////////////////////////////////////
    /// Member data
private:
    friend class MgServiceOperation;

    ACE_Recursive_Thread_Mutex          m_mutex;
    ACE_SOCK_Stream                     m_SockStream;
    ACE_Message_Queue<ACE_MT_SYNCH>*    m_pMessageQueue;
    Ptr<MgStreamHelper>                 m_pStreamHelper;
    HandlerStatus                       m_Status;
    MgConnection*                       m_pConnection;
};

#endif
