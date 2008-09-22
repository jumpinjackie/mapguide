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

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the definitions of one or more schemas contained in the
/// feature source for particular classes. If the specified schema name or
/// a class name does not exist, this method will throw an exception.
///
MgFeatureSchemaCollection* MgFeatureService::DescribeSchema(MgResourceIdentifier* resource,
    CREFSTRING schemaName, MgStringCollection* classNames)
{
    throw new MgNotImplementedException(L"MgFeatureService.DescribeSchema",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the definitions in XML format of one or more schemas contained in the
/// feature source for particular classes. If the specified schema name or
/// a class name does not exist, this method will throw an exception.
///
STRING MgFeatureService::DescribeSchemaAsXml(MgResourceIdentifier* resource,
    CREFSTRING schemaName, MgStringCollection* classNames)
{
    throw new MgNotImplementedException(L"MgFeatureService.DescribeSchemaAsXml",
        __LINE__, __WFILE__, NULL, L"", NULL);
}
