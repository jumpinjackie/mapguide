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
// The CompoundSymbolDefinition class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "CompoundSymbolDefinition.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the CompoundSymbolDefinition class.
//-------------------------------------------------------------------------
CompoundSymbolDefinition::CompoundSymbolDefinition()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all objects that have been created on the
//          heap or have been adopted.
//-------------------------------------------------------------------------
CompoundSymbolDefinition::~CompoundSymbolDefinition()
{
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
SimpleSymbolCollection* CompoundSymbolDefinition::GetSymbols()
{
    return &this->m_collSymbols;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void CompoundSymbolDefinition::AcceptVisitor(ISymbolDefinitionVisitor& isdVisitor)
{
    isdVisitor.VisitCompoundSymbolDefinition(*this);
}
