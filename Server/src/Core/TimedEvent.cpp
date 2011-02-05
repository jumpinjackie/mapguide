//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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
#include "TimedEvent.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs the object.
///
MgTimedEvent::MgTimedEvent() :
    m_id(0),
    m_timeout(0)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destructs the object.
///
MgTimedEvent::~MgTimedEvent()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets the event ID.
///
void MgTimedEvent::SetId(long id)
{
    m_id = id;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets the event timeout (in seconds).
///
void MgTimedEvent::SetTimeout(INT32 timeout)
{
    m_timeout = timeout;
}
