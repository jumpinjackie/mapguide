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

//-------------------------------------------------------------------------
// DESCRIPTION: The PrintLayoutElementDefinition class implementation.
//-------------------------------------------------------------------------

#include "../stdafx.h"
#include "PrintLayoutElementDefinition.h"

///////////////////////////////////////////////////////////////////////////////
/// Print Layout Element Definition types
///
const MdfString PrintLayoutElementDefinitionType::MapViewportDefinition = L"MapViewportDefinition";

//-------------------------------------------------------------------------
// PURPOSE: Default constructor of the PrintLayoutElementDefinition class.
//-------------------------------------------------------------------------
PrintLayoutElementDefinition::PrintLayoutElementDefinition()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all adopted objects.
//-------------------------------------------------------------------------
PrintLayoutElementDefinition::~PrintLayoutElementDefinition()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Manage properties of this PrintLayoutElementDefinition object.
//-------------------------------------------------------------------------
void PrintLayoutElementDefinition::SetDescription(const MdfString& description)
{
    m_description = description;
}

void PrintLayoutElementDefinition::SetResourceId(const MdfString& resourceId)
{
    m_resourceId = resourceId;
}
