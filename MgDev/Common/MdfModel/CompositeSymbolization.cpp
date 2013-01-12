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
// The CompositeSymbolization class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "CompositeSymbolization.h"
#include "ISymbolizationVisitor.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the CompositeSymbolization class.
//-------------------------------------------------------------------------
CompositeSymbolization::CompositeSymbolization()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all objects that have been created on the
//          heap or have been adopted.
//-------------------------------------------------------------------------
CompositeSymbolization::~CompositeSymbolization()
{
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
SymbolInstanceCollection* CompositeSymbolization::GetSymbolCollection()
{
    return &this->m_collInstances;
}

//-------------------------------------------------------------------------
// PURPOSE: Implementation of the Visitor pattern. The pure, virtual method
//          is defined in Symbol and is implemented by all of its concrete
//          subclasses.
// PARAMETERS:
//      Input:
//          isymVisitor - The ISymbolizationVisitor interface which sports
//                        methods that accept the final concrete type this
//                        CompositeSymbolization represents as an argument.
//-------------------------------------------------------------------------
void CompositeSymbolization::AcceptVisitor(ISymbolizationVisitor& isymVisitor)
{
    isymVisitor.VisitCompositeSymbolization(*this);
}
