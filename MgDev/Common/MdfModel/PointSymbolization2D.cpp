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
// DESCRIPTION:
// PointSymbolization2D class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "PointSymbolization2D.h"
#include "W2DSymbol.h"
#include "ISymbolizationVisitor.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the PointSymbolization2D
//          class.
//-------------------------------------------------------------------------
PointSymbolization2D::PointSymbolization2D()
{
    this->m_psymSymbol = NULL;

    // TODO: when we handle MarkSymbols, this default symbol should be one
    this->m_psymSymbol = new W2DSymbol(L"", L"");
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all adopted objects.
//-------------------------------------------------------------------------
PointSymbolization2D::~PointSymbolization2D()
{
    delete this->m_psymSymbol;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for Symbol property defined in this Point
//          Symbolization. The Symbol object held by the property is used
//          to describe the appearance of the point feature to which this
//          symbolization has been applied.
// RETURNS: A pointer to the modifiable symbol object. It may be NULL if it
//          hasn't been defined.
//-------------------------------------------------------------------------
Symbol* PointSymbolization2D::GetSymbol()
{
    return this->m_psymSymbol;
}

//-------------------------------------------------------------------------
// PURPOSE: Method to adopt a Symbol object to represent the Symbol property
//          defined in this PointSymbolization2D. The Symbol object held by the
//          property is used to describe the appearance of the point feature
//          to which this symbolization has been applied. If an object has
//          previously been defined it is deleted. This symbolization is now
//          responsible for the deletion of the adopted Symbol.
// PARAMETERS:
//      Input:
//          psymAdoptedSymbol - the pointer to the Symbol object which has been
//                              created on the heap.
//-------------------------------------------------------------------------
void PointSymbolization2D::AdoptSymbol(Symbol* psymAdoptedSymbol)
{
    if (this->m_psymSymbol != psymAdoptedSymbol)
    {
        delete this->m_psymSymbol;
        this->m_psymSymbol = psymAdoptedSymbol;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Method to orphan the Symbol object being held by the Symbol
//          property on this symbolization. The caller is now responsible
//          for the deletion of the returned Symbol. The Symbol property is
//          set to NULL.
// RETURNS: Pointer to the Symbol object. It may be NULL if it hasn't been
//          defined.
//-------------------------------------------------------------------------
Symbol* PointSymbolization2D::OrphanSymbol()
{
    Symbol* psymRet = this->m_psymSymbol;
    this->m_psymSymbol = NULL;
    return psymRet;
}

//-------------------------------------------------------------------------
// PURPOSE: Implementation of the Visitor pattern. The pure, virtual method
//          is defined in Symbolization and is implemented by all of its
//          concrete subclasses.
// PARAMETERS:
//      Input:
//          isymVisitor - The ISymbolizationVisitor interface which sports
//                        methods that accept the final concrete type this
//                        PointSymbolization2D represents as an argument.
//-------------------------------------------------------------------------
void PointSymbolization2D::AcceptVisitor(ISymbolizationVisitor& isymVisitor)
{
    isymVisitor.VisitPointSymbolization2D(*this);
}
