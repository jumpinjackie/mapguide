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

#ifndef MGSERVERCOMMON_H_
#define MGSERVERCOMMON_H_

//////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Enumeration for the idle timer events.
/// </summary>

enum IdleTimerEvent
{
    evtConnection = 0,          /// Timer event that checks for idle connections.
    evtDataConnection,          /// Timer event that checks for idle data connections.
    evtServiceRegistration,     /// Timer event that registers server services.
    evtSessionCleanup,          /// Timer event that cleans up expired sessions.
    evtRepositoryCheckpoints,   /// Timer event that performs repository checkpoints.
};

#endif // MGSERVERCOMMON_H_
