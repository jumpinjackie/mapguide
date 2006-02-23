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

#include "AceCommon.h"

//////////////////////////////////////////////////////////////////
/// <summary>
/// Construct a Service object.
/// </summary>
MgService::MgService(MgConnectionProperties* connProp)
{
    m_connProp = SAFE_ADDREF(connProp);

    // One warning object per service instance
    m_warning = new MgWarnings();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Default Constructor
/// </summary>
MgService::MgService()
{
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor. This will clean up the service.
/// </summary>
/// <returns>
/// Nothing
/// </returns>
MgService::~MgService()
{
}

/// <summary>
/// GetWarningsObject() method returns all the warning messages.
/// </summary>
///
/// <returns>
/// Returns a pointer to the MgWarnings object.
/// </returns>
MgWarnings* MgService::GetWarningsObject()
{
    return SAFE_ADDREF((MgWarnings*)m_warning);
}

/// <summary>
/// HasWarnings() method reports if there are warnings.
/// </summary>
///
/// <returns>
/// Returns true or false
/// </returns>
bool MgService::HasWarnings()
{
    return m_warning->GetCount() > 0 ? true : false;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgService::GetClassId()
{
    return m_cls_id;
}
