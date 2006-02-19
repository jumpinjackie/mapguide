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

#include "ResourceServiceDefs.h"
#include "OperationInfo.h"

const MgOperationParameter MgOperationInfo::m_defaultParameter;

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgOperationInfo::MgOperationInfo()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Constructs a new MgOperationInfo object and initializes the member variables
/// to values given by a referenced MgOperationInfo object.
/// </summary>
///
/// <param name="opInfo">
/// Reference to an MgOperationInfo object.
/// </param>
///
/// <exceptions>
/// MgOutOfMemoryException
/// </exceptions>
///----------------------------------------------------------------------------

MgOperationInfo::MgOperationInfo(const MgOperationInfo& opInfo)
{
    *this = opInfo;
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgOperationInfo::~MgOperationInfo()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Overloaded assignment operator for MgOperationInfo.
/// </summary>
///
/// <param name="opInfo">
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

MgOperationInfo& MgOperationInfo::operator=(const MgOperationInfo& opInfo)
{
    if (&opInfo != this)
    {
        m_name = opInfo.m_name;
        m_version = opInfo.m_version;

        m_parameters.clear();
        MgOpParamMap::const_iterator i;

        for (i = opInfo.m_parameters.begin(); i != opInfo.m_parameters.end(); ++i)
        {
            m_parameters.insert(MgOpParamMap::value_type(
                (*i).first, (*i).second));
        }
    }

    return *this;
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets the name of the operation.
/// </summary>
///----------------------------------------------------------------------------

void MgOperationInfo::SetName(CREFSTRING name)
{
    m_name = name;
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets the version of the operation.
/// </summary>
///----------------------------------------------------------------------------

void MgOperationInfo::SetVersion(CREFSTRING version)
{
    m_version = version;
}

///----------------------------------------------------------------------------
/// <summary>
/// Gets the specified parameter.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

const MgOperationParameter& MgOperationInfo::GetParameter(CREFSTRING name,
    bool required) const
{
    MgOpParamMap::const_iterator i = m_parameters.find(name);

    if (m_parameters.end() != i)
    {
        return (*i).second;
    }
    else if (required)
    {
        MgStringCollection arguments;
        arguments.Add(name);

        throw new MgParameterNotFoundException(
            L"MgOperationInfo.GetParameter", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    return m_defaultParameter;
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets the specified parameter.
/// </summary>
///
/// <exceptions>
/// MgException
/// </exceptions>
///----------------------------------------------------------------------------

void MgOperationInfo::SetParameter(CREFSTRING name,
    const MgOperationParameter& opParam)
{
    MgOpParamMap::const_iterator i = m_parameters.find(name);

    if (m_parameters.end() == i)
    {
        m_parameters.insert(MgOpParamMap::value_type(name, opParam));
    }
    else
    {
        MgStringCollection arguments;
        arguments.Add(name);

        throw new MgDuplicateParameterException(
            L"MgOperationInfo.SetParameter", __LINE__, __WFILE__, &arguments, L"", NULL);
    }
}
