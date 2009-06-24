//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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
#include "PrintLayoutBase.h"

MG_IMPL_DYNCREATE(MgPrintLayoutBase)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs an MgPrintLayoutBase object.
///
MgPrintLayoutBase::MgPrintLayoutBase()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destroys an MgPrintLayoutBase object.
///
MgPrintLayoutBase::~MgPrintLayoutBase()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the class ID.
///
INT32 MgPrintLayoutBase::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Disposes this object.
///
void MgPrintLayoutBase::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the name of the resource type.
///
const char* MgPrintLayoutBase::GetResourceTypeName()
{
    return "PrintLayout";
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the name of the layout.
///
STRING MgPrintLayoutBase::GetName()
{
    return L"";
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Does it allow to set the name
///
bool MgPrintLayoutBase::CanSetName()
{
    // via Create
    return true;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Serializes data to TCP/IP stream.
///
void MgPrintLayoutBase::Serialize(MgStream* stream)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Deserializes data from TCP/IP stream.
///
void MgPrintLayoutBase::Deserialize(MgStream* stream)
{
}
