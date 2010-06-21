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

#ifndef MGSESSIONTIMEOUTEVENTHANDLER_H_
#define MGSESSIONTIMEOUTEVENTHANDLER_H_

#include "TimedEventHandler.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Derived Event Handler class to clean up expired sessions.
///
class MgSessionTimeoutEventHandler : public MgTimedEventHandler
{
/// Constructors/Destructor

public:

    MgSessionTimeoutEventHandler(MgEventTimer& timer);
    virtual ~MgSessionTimeoutEventHandler();

private:

    // Unimplemented Constructors/Methods

    MgSessionTimeoutEventHandler();
    MgSessionTimeoutEventHandler(const MgSessionTimeoutEventHandler&);
    MgSessionTimeoutEventHandler& operator=(const MgSessionTimeoutEventHandler&);

/// Methods

protected:

    virtual void HandleEvent(long eventId);

/// Data Members

private:

    Ptr<MgStringCollection> m_expiredSessions;
};

/// Inline Methods

#endif
