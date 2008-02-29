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

#include "ResourceServiceDefs.h"
#include "TagInfo.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgTagInfo::MgTagInfo()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object based on the specified tag attributes.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

MgTagInfo::MgTagInfo(CREFSTRING storageType, CREFSTRING tokenValue,
    CREFSTRING mimeType)
{
    m_attributes[MgTagInfo::StorageType] = storageType;
    m_attributes[MgTagInfo::TokenValue] = tokenValue;
    m_attributes[MgTagInfo::MimeType] = mimeType;
}

///----------------------------------------------------------------------------
/// <summary>
/// Constructs a new MgTagInfo object and initializes the member variables
/// to values given by a referenced MgTagInfo object.
/// </summary>
///
/// <param name="tagInfo">
/// Reference to an MgTagInfo object.
/// </param>
///
/// <exceptions>
/// MgOutOfMemoryException
/// </exceptions>
///----------------------------------------------------------------------------

MgTagInfo::MgTagInfo(const MgTagInfo& tagInfo)
{
    *this = tagInfo;
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgTagInfo::~MgTagInfo()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Overloaded assignment operator for MgTagInfo.
/// </summary>
///
/// <param name="tagInfo">
/// Reference to the structure that is to be copied.
/// </param>
///
/// <return>
/// Reference to the structure (the one assigned to).
/// </return>
///
/// <exceptions>
/// MgOutOfMemoryException
/// </exceptions>
///----------------------------------------------------------------------------

MgTagInfo& MgTagInfo::operator=(const MgTagInfo& tagInfo)
{
    if (&tagInfo != this)
    {
        for (int i = 0; i < MAX_RESOURCE_TAG_ATTRIBUTE_NUMBER; ++i)
        {
            m_attributes[i] = tagInfo.m_attributes[i];
        }
    }

    return *this;
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets a tag attribute.
/// </summary>
///----------------------------------------------------------------------------

void MgTagInfo::SetAttribute(Attribute name, CREFSTRING value)
{
    m_attributes[name] = value;
}
