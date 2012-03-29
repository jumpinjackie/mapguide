//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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
// DESCRIPTION:
// The SymbolDefinition class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "SymbolDefinition.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the SymbolDefinition class.
//-------------------------------------------------------------------------
SymbolDefinition::SymbolDefinition()
{
    // default values
    // NOTE: values used in IOSimpleSymbolDefinition::Write and
    //       IOCompoundSymbolDefinition::Write must match these
//  this->m_sName        = L""; // NOXLATE
//  this->m_sDescription = L""; // NOXLATE
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all objects that have been created on the
//          heap or have been adopted.
//-------------------------------------------------------------------------
SymbolDefinition::~SymbolDefinition()
{
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void SymbolDefinition::SetName(const MdfString& name)
{
    this->m_sName = name;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& SymbolDefinition::GetName() const
{
    return this->m_sName;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void SymbolDefinition::SetDescription(const MdfString& description)
{
    this->m_sDescription = description;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& SymbolDefinition::GetDescription() const
{
    return this->m_sDescription;
}
