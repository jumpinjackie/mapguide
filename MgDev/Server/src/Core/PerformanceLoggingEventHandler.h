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

#ifndef MGPERFORMANCELOGGINGEVENTHANDLER_H_
#define MGPERFORMANCELOGGINGEVENTHANDLER_H_

#include "TimedEventHandler.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Derived Event Handler class to perform performance logging.
///
class MgPerformanceLoggingEventHandler : public MgTimedEventHandler
{
/// Constructors/Destructor

public:

    MgPerformanceLoggingEventHandler(MgEventTimer& timer);
    virtual ~MgPerformanceLoggingEventHandler();

private:

    // Unimplemented Constructors/Methods

    MgPerformanceLoggingEventHandler();
    MgPerformanceLoggingEventHandler(const MgPerformanceLoggingEventHandler&);
    MgPerformanceLoggingEventHandler& operator=(const MgPerformanceLoggingEventHandler&);

/// Methods

protected:

    virtual void HandleEvent(long eventId);

/// Data Members

private:
    bool m_bPerformanceLogEnabled;

};

/// Inline Methods

#endif
