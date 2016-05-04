//
//  Copyright (C) 2004-2015 by Autodesk, Inc.
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

#ifndef MG_RESOURCE_SERVICE_CACHE_TIMEOUT_EVENT_HANDLER_H_
#define MG_RESOURCE_SERVICE_CACHE_TIMEOUT_EVENT_HANDLER_H_

#include "TimedEventHandler.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Derived Event Handler class to clean up inactive resource service cache items.
///
class MgResourceServiceCacheTimeLimitEventHandler : public MgTimedEventHandler
{
/// Constructors/Destructor

public:

    MgResourceServiceCacheTimeLimitEventHandler(MgEventTimer& timer);
    virtual ~MgResourceServiceCacheTimeLimitEventHandler();

private:

    // Unimplemented Constructors/Methods

    MgResourceServiceCacheTimeLimitEventHandler();
    MgResourceServiceCacheTimeLimitEventHandler(const MgResourceServiceCacheTimeLimitEventHandler&);
    MgResourceServiceCacheTimeLimitEventHandler& operator=(const MgResourceServiceCacheTimeLimitEventHandler&);

/// Methods

protected:

    virtual void HandleEvent(long eventId);

/// Data Members

private:

};

/// Inline Methods

#endif
