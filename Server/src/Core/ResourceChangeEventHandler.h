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

#ifndef MGRESOURCECHANGEEVENTHANDLER_H_
#define MGRESOURCECHANGEEVENTHANDLER_H_

#include "TimedEventHandler.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Derived Event Handler class to dispatch resource change notifications.
///
class MgResourceChangeEventHandler : public MgTimedEventHandler
{
/// Constructors/Destructor

public:

    MgResourceChangeEventHandler(MgEventTimer& timer);
    virtual ~MgResourceChangeEventHandler();

private:

    // Unimplemented Constructors/Methods

    MgResourceChangeEventHandler();
    MgResourceChangeEventHandler(const MgResourceChangeEventHandler&);
    MgResourceChangeEventHandler& operator=(const MgResourceChangeEventHandler&);

/// Methods

protected:

    virtual void HandleEvent(long eventId);

/// Data Members

private:

    MgServiceManager* m_serviceManager;
};

/// Inline Methods

#endif
