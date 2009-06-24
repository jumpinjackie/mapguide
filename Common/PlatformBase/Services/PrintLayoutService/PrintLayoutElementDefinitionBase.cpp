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
#include "PrintLayoutElementDefinitionBase.h"

MG_IMPL_DYNCREATE(MgPrintLayoutElementDefinitionBase)

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Constructs an MgPrintLayoutElementDefinitionBase object.
///
MgPrintLayoutElementDefinitionBase::MgPrintLayoutElementDefinitionBase()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destroys an MgPrintLayoutElementDefinitionBase object.
///
MgPrintLayoutElementDefinitionBase::~MgPrintLayoutElementDefinitionBase()
{
    //TODO Default initializations
}


///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the name.
///
STRING MgPrintLayoutElementDefinitionBase::GetName()
{
    return m_name;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Sets the name.
///
void MgPrintLayoutElementDefinitionBase::SetName(CREFSTRING name)
{
    m_name = name;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the class ID.
///
INT32 MgPrintLayoutElementDefinitionBase::GetClassId()
{
    return m_cls_id;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Disposes this object.
///
void MgPrintLayoutElementDefinitionBase::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Returns the name of the resource type.
///
const char* MgPrintLayoutElementDefinitionBase::GetResourceTypeName()
{
    return "PrintLayoutElementDefinition";
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Does it allow to set the name
///
bool MgPrintLayoutElementDefinitionBase::CanSetName()
{
    // via Create
    return true;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Serializes data to TCP/IP stream.
///
void MgPrintLayoutElementDefinitionBase::Serialize(MgStream* stream)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Deserializes data from TCP/IP stream.
///
void MgPrintLayoutElementDefinitionBase::Deserialize(MgStream* stream)
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Initializes this object from the information in the resource XML string
///
void MgPrintLayoutElementDefinitionBase::PopulateFromResource(CREFSTRING resourceXml)
{
    //TODO
}
