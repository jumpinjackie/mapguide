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

#include "PlatformBase.h"

//////////////////////////////////////////////////////////////////
/// <summary>
/// Default Constructor
/// </summary>
MgService::MgService()
{
    // One warning object per service instance
    m_warning = new MgWarnings();
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
/// Placeholder to associate user information with a service.
/// Not implemented in the base Service class.
/// </summary>
MgUserInformation* MgService::GetUserInfo()
{
    MG_TRY()
        throw new MgNotImplementedException(L"MgService.GetUserInfo", __LINE__, __WFILE__, NULL, L"", NULL);
    MG_CATCH_AND_THROW(L"MgService.GetUserInfo");
    return NULL;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Placeholder to associate user information with a service.
/// Not implemented in the base Service class.
/// </summary>
void MgService::SetConnectionProperties(MgConnectionProperties* connProp)
{
    MG_TRY()
        throw new MgNotImplementedException(L"MgService.SetConnectionProperties", __LINE__, __WFILE__, NULL, L"", NULL);
    MG_CATCH_AND_THROW(L"MgService.SetConnectionProperties");
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgService::GetClassId()
{
    return m_cls_id;
}

