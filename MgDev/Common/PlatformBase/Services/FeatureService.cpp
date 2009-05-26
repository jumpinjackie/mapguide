//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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
MgFeatureService::MgFeatureService()
{
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Get the class Id
/// </summary>
/// <returns>
/// The integer value
/// </returns>
INT32 MgFeatureService::GetClassId()
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
void MgFeatureService::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Retrieve identity properties for specified classe
/// </summary>
/// <returns>
/// Property definitions for identity properties
/// </returns>
MgPropertyDefinitionCollection* MgFeatureService::GetIdentityProperties(MgResourceIdentifier* resource,
                                                                              CREFSTRING schemaName,
                                                                              CREFSTRING className)
{
    throw new MgNotImplementedException(L"MgFeatureService.GetIdentityProperties", __LINE__, __WFILE__, NULL, L"", NULL);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Retrieve identity properties for specified classes
/// </summary>
/// <returns>
/// Class definitions with identity properties
/// </returns>
MgClassDefinitionCollection* MgFeatureService::GetIdentityProperties(MgResourceIdentifier* resource,
                                                                     CREFSTRING schemaName,
                                                                     MgStringCollection* classNames)
{
    throw new MgNotImplementedException(L"MgFeatureService.GetIdentityProperties", __LINE__, __WFILE__, NULL, L"", NULL);
}

