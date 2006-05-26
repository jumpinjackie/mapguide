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
/// Default Constructor
/// </summary>
MgMappingService::MgMappingService() : MgService(NULL)
{
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Construct an MgMappingService object
/// </summary>
MgMappingService::MgMappingService(MgConnectionProperties* connection) : MgService(connection)
{
}

//////////////////////////////////////////////////////////////////
///TODO: Get rid of this stub once user info is passed to the server
MgByteReader* MgMappingService::GenerateMap(MgMap* map, CREFSTRING mapAgentUri, MgDwfVersion* dwfVersion)
{
    STRING sessionId;

    if (m_connProp.p != NULL)
    {
        Ptr<MgUserInformation> userInfo = m_connProp->GetUserInfo();
        if (userInfo.p != NULL)
        {
            sessionId = userInfo->GetMgSessionId();
        }
    }

    if (sessionId.empty())
    {
        return NULL;
    }
    else
    {
        return GenerateMap(map, sessionId, mapAgentUri, dwfVersion);
    }
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Get the class Id
/// </summary>
/// <returns>
/// The integer value
/// </returns>
INT32 MgMappingService::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Dispose this object
/// </summary>
/// <returns>
/// Nothing
/// </returns>
void MgMappingService::Dispose()
{
    delete this;
}
