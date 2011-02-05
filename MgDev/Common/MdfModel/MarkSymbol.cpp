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
// MarkSymbol class implementation. MarkSymbol is a concrete subclass of
// the FlatSymbol class.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "MarkSymbol.h"
#include "Fill.h"
#include "Stroke.h"
#include "ISymbolVisitor.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the MarkSymbol class.
//-------------------------------------------------------------------------
MarkSymbol::MarkSymbol(const Shape shape)
: m_shape(shape)
{
    this->m_pstkEdge = new Stroke();
    this->m_pflFill = new Fill();
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all adopted objects.
//-------------------------------------------------------------------------
MarkSymbol::~MarkSymbol()
{
    delete this->m_pflFill;
    delete this->m_pstkEdge;
}

//-------------------------------------------------------------------------
// PURPOSE: Access the shape of this symbol.
// RETURNS: The shape enumeration.
//-------------------------------------------------------------------------
const MarkSymbol::Shape MarkSymbol::GetShape() const
{
    return this->m_shape;
}

//-------------------------------------------------------------------------
// PURPOSE: Set the shape of this symbol.
// PARAMETERS:
//      Input:
//          shape - The shape enumeration.
//-------------------------------------------------------------------------
void MarkSymbol::SetShape(const MarkSymbol::Shape shape)
{
    this->m_shape = shape;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Fill property in this MarkSymbol.
//          The Fill is applied to the polygon geometric primitives of the
//          specified features during stylization.
// RETURNS: A pointer to a modifiable Fill object. It may be NULL if it hasn't
//          been defined.
//-------------------------------------------------------------------------
Fill* MarkSymbol::GetFill()
{
    return this->m_pflFill;
}

//-------------------------------------------------------------------------
// PURPOSE: Method to adopt a Fill object for the Fill property defined
//          in this MarkSymbol. The Fill is applied to the polygon
//          geometric primitives of the specified features during stylization.
//          This object is now responsible for the deletion of the adopted
//          Fill.
// PARAMETERS:
//      Input:
//         pflAdoptedFill - A pointer to a Fill subclass object created
//                          on the heap.
//-------------------------------------------------------------------------
void MarkSymbol::AdoptFill(Fill* pflAdoptedFill)
{
    if (this->m_pflFill != pflAdoptedFill)
    {
        delete this->m_pflFill;
        this->m_pflFill = pflAdoptedFill;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Method to orphan the Fill object being held by this symbolization.
//          The Fill is applied to the polygon geometric primitives of the
//          specified features during stylization. The caller is now
//          responsible for the deletion of the returned Fill object.
// RETURNS: Pointer to the orphaned Fill object. It may be NULL if it wasn't
//          defined.
//-------------------------------------------------------------------------
Fill* MarkSymbol::OrphanFill()
{
    Fill* pflRet = this->m_pflFill;
    this->m_pflFill = NULL;
    return pflRet;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Edge property in this MarkSymbol.
//          The Edge is applied to the polygon geometric primitives of the
//          specified features during stylization.
// RETURNS: A pointer to a modifiable Stroke object. It may be NULL if it
//          hasn't been defined.
//-------------------------------------------------------------------------
Stroke* MarkSymbol::GetEdge()
{
    return this->m_pstkEdge;
}

//-------------------------------------------------------------------------
// PURPOSE: Method to adopt a Stroke object for the Edge property defined
//          in this MarkSymbol. The Stroke is applied to the polygon
//          geometric primitives of the specified features during stylization.
//          This object is now responsible for the deletion of the adopted
//          Stroke.
// PARAMETERS:
//      Input:
//         pstkAdoptedEdge - A pointer to a Stroke subclass object created
//                             on the heap.
//-------------------------------------------------------------------------
void MarkSymbol::AdoptEdge(Stroke* pstkAdoptedEdge)
{
    if (this->m_pstkEdge != pstkAdoptedEdge)
    {
        delete this->m_pstkEdge;
        this->m_pstkEdge = pstkAdoptedEdge;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Method to orphan the Stroke object being held by this
//          symbolization. The Stroke is applied to the polygon geometric
//          primitives of the specified features during stylization. The
//          caller is now responsible for the deletion of the returned Stroke
//          object.
// RETURNS: Pointer to the orphaned Stroke object. It may be NULL if it wasn't
//          defined.
//-------------------------------------------------------------------------
Stroke* MarkSymbol::OrphanEdge()
{
    Stroke* pstkRet = this->m_pstkEdge;
    this->m_pstkEdge = NULL;
    return pstkRet;
}

//-------------------------------------------------------------------------
// PURPOSE: Implementation of the Visitor pattern. The pure, virtual method
//          is defined in Symbol and is implemented by all of its concrete
//          subclasses.
// PARAMETERS:
//      Input:
//          isymVisitor - The ISymbolVisitor interface which sports methods
//                        that accept the final concrete type this MarkSymbol
//                        represents as an argument.
//-------------------------------------------------------------------------
void MarkSymbol::AcceptVisitor(ISymbolVisitor& isymVisitor)
{
    isymVisitor.VisitMarkSymbol(*this);
}
