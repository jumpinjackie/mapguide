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
// Implementation of the Label class.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "Label.h"
#include "Symbol.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the Label class.
//-------------------------------------------------------------------------
Label::Label()
{
    this->m_Symbol = NULL;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all adopted objects.
//-------------------------------------------------------------------------
Label::~Label()
{
    delete this->m_Symbol;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the Symbol in this Label.
// RETURNS: A pointer to the Symbol.
//-------------------------------------------------------------------------
TextSymbol* Label::GetSymbol()
{
    return this->m_Symbol;
}

//-------------------------------------------------------------------------
// PURPOSE: Method to adopt a Symbol object for the Symbolization property defined
//          in this AreaRule. The Symbol is applied to the point
//          geometric primitives of the specified features during stylization.
//          This object is now responsible for the deletion of the adopted
//          Symbol.
// PARAMETERS:
//      Input:
//         pflAdoptedSymbolization - A pointer to a Symbol subclass object created
//                          on the heap.
//-------------------------------------------------------------------------
void Label::AdoptSymbol(TextSymbol* pflAdoptedSymbol)
{
    if (this->m_Symbol != pflAdoptedSymbol)
    {
        delete this->m_Symbol;
        this->m_Symbol = pflAdoptedSymbol;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Method to orphan the Symbol object being held by this rule.
//          The Symbol is applied to label the geometric primitives of the
//          specified features during stylization. The caller is now
//          responsible for the deletion of the returned Symbol object.
// RETURNS: Pointer to the orphaned Symbol object. It may be NULL if it wasn't
//          defined.
//-------------------------------------------------------------------------
TextSymbol* Label::OrphanSymbol()
{
    TextSymbol* pRet = this->m_Symbol;
    this->m_Symbol = NULL;
    return pRet;
}
