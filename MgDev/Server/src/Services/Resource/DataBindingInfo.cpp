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

#include "ResourceServiceDefs.h"
#include "DataBindingInfo.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgDataBindingInfo::MgDataBindingInfo()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Constructs a new MgDataBindingInfo object and initializes the member
/// variables to values given by a referenced MgDataBindingInfo object.
/// </summary>
///
/// <param name="dataBindingInfo">
/// Reference to an MgDataBindingInfo object.
/// </param>
///
/// <exceptions>
/// MgOutOfMemoryException
/// </exceptions>
///----------------------------------------------------------------------------

MgDataBindingInfo::MgDataBindingInfo(const MgDataBindingInfo& dataBindingInfo)
{
    *this = dataBindingInfo;
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgDataBindingInfo::~MgDataBindingInfo()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Overloaded assignment operator for MgDataBindingInfo.
/// </summary>
///
/// <param name="dataBindingInfo">
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

MgDataBindingInfo& MgDataBindingInfo::operator=(
    const MgDataBindingInfo& dataBindingInfo)
{
    if (&dataBindingInfo != this)
    {
        m_preProcessTags = dataBindingInfo.m_preProcessTags;
        m_resourceDataFilePath = dataBindingInfo.m_resourceDataFilePath;
        m_loginUsername = dataBindingInfo.m_loginUsername;
        m_loginPassword = dataBindingInfo.m_loginPassword;
        m_substituteUnmanagedDataMappings = dataBindingInfo.m_substituteUnmanagedDataMappings;
    }

    return *this;
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets the pre-process tags.
/// </summary>
///----------------------------------------------------------------------------

void MgDataBindingInfo::SetPreProcessTags(CREFSTRING preProcessTags)
{
    m_preProcessTags = preProcessTags;
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets the data file path.
/// </summary>
///----------------------------------------------------------------------------

void MgDataBindingInfo::SetResourceDataFilePath(CREFSTRING resourceDataFilePath)
{
    m_resourceDataFilePath = resourceDataFilePath;
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets the login username.
/// </summary>
///----------------------------------------------------------------------------

void MgDataBindingInfo::SetLoginUsername(CREFSTRING loginUsername)
{
    m_loginUsername = loginUsername;
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets the login password.
/// </summary>
///----------------------------------------------------------------------------

void MgDataBindingInfo::SetLoginPassword(CREFSTRING loginPassword)
{
    m_loginPassword = loginPassword;
}

///----------------------------------------------------------------------------
/// <summary>
/// Enable unmanaged data mapping substitution
/// </summary>
///----------------------------------------------------------------------------

void MgDataBindingInfo::SetSubstituteUnmanagedDataMappings(bool value)
{
    m_substituteUnmanagedDataMappings = value;
}
