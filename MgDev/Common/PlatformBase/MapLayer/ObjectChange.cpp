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
// Construct an MgObjectChange object
//
MgObjectChange::MgObjectChange()
{
}

MgObjectChange::MgObjectChange(MgObjectChange::ChangeType type)
{
    m_type = type;
}

//////////////////////////////////////////////////////////////////
// Get the type of change at the specified index
//
MgObjectChange::ChangeType MgObjectChange::GetType()
{
    return m_type;
}

//////////////////////////////////////////////////////////////////
// Get the parameter value for this change
//
STRING MgObjectChange::GetParam()
{
    return m_param;
}

//////////////////////////////////////////////////////////////////
// Set the parameter value for this change
//
void MgObjectChange::SetParam(CREFSTRING param)
{
    m_param = param;
}

//////////////////////////////////////////////////////////////////
// Dispose this object
//
void MgObjectChange::Dispose()
{
    delete this;
}
