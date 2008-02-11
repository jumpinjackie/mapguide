//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef MGDATACONNECTIONTIMEOUTEVENTHANDLER_H_
#define MGDATACONNECTIONTIMEOUTEVENTHANDLER_H_

#include "TimedEventHandler.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Derived Event Handler class to clean up inactive FDO connections.
///
class MgDataConnectionTimeoutEventHandler : public MgTimedEventHandler
{
/// Constructors/Destructor

public:

    MgDataConnectionTimeoutEventHandler(MgEventTimer& timer);
    virtual ~MgDataConnectionTimeoutEventHandler();

private:

    // Unimplemented Constructors/Methods

    MgDataConnectionTimeoutEventHandler();
    MgDataConnectionTimeoutEventHandler(const MgDataConnectionTimeoutEventHandler&);
    MgDataConnectionTimeoutEventHandler& operator=(const MgDataConnectionTimeoutEventHandler&);

/// Methods

protected:

    virtual void HandleEvent(long eventId);

/// Data Members

private:

};

/// Inline Methods

#endif
