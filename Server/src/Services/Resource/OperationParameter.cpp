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
#include "OperationParameter.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgOperationParameter::MgOperationParameter()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Constructs a new MgOperationParameter object and initializes the member
/// variables to values given by a referenced MgOperationParameter object.
/// </summary>
///
/// <param name="opParam">
/// Reference to an MgOperationParameter object.
/// </param>
///
/// <exceptions>
/// MgOutOfMemoryException
/// </exceptions>
///----------------------------------------------------------------------------

MgOperationParameter::MgOperationParameter(const MgOperationParameter& opParam)
{
    *this = opParam;
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs the object.
/// </summary>
///----------------------------------------------------------------------------

MgOperationParameter::~MgOperationParameter()
{
}

///----------------------------------------------------------------------------
/// <summary>
/// Overloaded assignment operator for MgOperationParameter.
/// </summary>
///
/// <param name="opParam">
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

MgOperationParameter& MgOperationParameter::operator=(
    const MgOperationParameter& opParam)
{
    if (&opParam != this)
    {
        m_value = opParam.m_value;
        m_contentType = opParam.m_contentType;
    }

    return *this;
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets the value of the parameter.
/// </summary>
///----------------------------------------------------------------------------

void MgOperationParameter::SetValue(CREFSTRING value)
{
    m_value = value;
}

///----------------------------------------------------------------------------
/// <summary>
/// Sets the content type of the parameter.
/// </summary>
///----------------------------------------------------------------------------

void MgOperationParameter::SetContentType(CREFSTRING contentType)
{
    m_contentType = contentType;
}
