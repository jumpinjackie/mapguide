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
// LineSymbolization2D class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "LineSymbolization2D.h"
#include "ISymbolizationVisitor.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the LineSymbolization2D
//          class.
//-------------------------------------------------------------------------
LineSymbolization2D::LineSymbolization2D()
{
    this->m_pstkStroke = new Stroke();
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all adopted objects.
//-------------------------------------------------------------------------
LineSymbolization2D::~LineSymbolization2D()
{
    delete this->m_pstkStroke;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Stroke defined in this LineSymbolization2D.
//          The Stroke is applied to the line geometric primitives contained
//          in the features to which this symbolization is applied.
// RETURNS: pointer to the modifiable Stroke object. May be NULL if it
//          hasn't been defined.
//-------------------------------------------------------------------------
Stroke* LineSymbolization2D::GetStroke()
{
    return this->m_pstkStroke;
}

//-------------------------------------------------------------------------
// PURPOSE: Method to adopt a Stroke object for the Stroke property defined
//          in this symbolization. The Stroke is applied to the line
//          geometric primitives contained in the features to which this
//          symbolization is applied. This object is now responsible for
//          the deletion of the adopted Stoke.
// PARAMETERS:
//      Input:
//          pstkAdoptedStroke - pointer to the Stroke created on the heap.
//                              It may be NULL.
//-------------------------------------------------------------------------
void LineSymbolization2D::AdoptStroke(Stroke* pstkAdoptedStroke)
{
    if (this->m_pstkStroke != pstkAdoptedStroke)
    {
        delete this->m_pstkStroke;
        this->m_pstkStroke = pstkAdoptedStroke;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Method to orphan the Stroke object held by this LineSymbolization2D.
//          The Stroke is applied to the line geometric primitive contained
//          in the features to which this symbolization is applied. The
//          caller is responsible for the deletion of the returned object.
// RETURNS: A pointer to the orphaned Stroke. It may be NULL if it hasn't
//          been defined.
//-------------------------------------------------------------------------
Stroke* LineSymbolization2D::OrphanStroke()
{
    Stroke* pstkRet = this->m_pstkStroke;
    this->m_pstkStroke = NULL;
    return pstkRet;
}

//-------------------------------------------------------------------------
// PURPOSE: Implementation of the Visitor pattern. The pure, virtual method
//          is defined in Symbolization and is implemented by all of its
//          concrete subclasses.
// PARAMETERS:
//      Input:
//          isymVisitor - The ISymbolizationVisitor interface which sports
//                        methods that accept the final concrete type this
//                        LineSymbolization2D represents as an argument.
//-------------------------------------------------------------------------
void LineSymbolization2D::AcceptVisitor(ISymbolizationVisitor& isymVisitor)
{
    isymVisitor.VisitLineSymbolization2D(*this);
}
