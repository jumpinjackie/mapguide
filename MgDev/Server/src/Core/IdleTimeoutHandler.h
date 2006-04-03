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

#ifndef MGIDLETIMEOUTHANDLER_H_
#define MGIDLETIMEOUTHANDLER_H_

#include "ace/Reactor.h"
#include "ace/Timer_Queue_Adapters.h"
#include "ace/Timer_Heap.h"

class MgServer;

// IdleTimeoutHandler
// This class handles timer events for searching for and removing idle connections and data connections.
class MgIdleTimeoutHandler : public ACE_Event_Handler
{
    ///////////////////////////////////////////////////////
    /// Methods
public:
    // Constructor/destructor
    MgIdleTimeoutHandler(MgServer* server, INT32 nConnectionTimeout,
        INT32 nSessionTimeout);
    ~MgIdleTimeoutHandler();

    // ACE_Event_Handler methods
    virtual int handle_timeout(const ACE_Time_Value &currentTime, const void *arg);

private:

    // Unimplemented Constructors/Methods

    MgIdleTimeoutHandler();
    MgIdleTimeoutHandler(const MgIdleTimeoutHandler&);
    MgIdleTimeoutHandler& operator=(const MgIdleTimeoutHandler&);

    // Helper method to search and process inactive connections.
    void FindAndCloseIdleConnections();

    // Helper method to search and process inactive data connections.
    void FindAndCloseIdleDataConnections();

    // Helper method to perform the service registration if this has not been done yet.
    void RegisterServices();

    // Helper method to clean up expired sessions.
    void CleanUpSessions();

    // Helper method to perform checkpoints for all repositories.
    void PerformRepositoryCheckpoints();

    ///////////////////////////////////////////////////////
    /// Member data
private:

    MgServer* m_server;
    INT32 m_nConnectionTimeout;
    INT32 m_nSessionTimeout;
    Ptr<MgStringCollection> m_expiredSessions;
};

#endif
