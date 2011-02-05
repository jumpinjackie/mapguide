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
// The AreaRule class implementation. AreaRule is a concrete subclass of
// Rule.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "AreaRule.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the AreaRule class.
//-------------------------------------------------------------------------
AreaRule::AreaRule()
{
    this->m_Symbolization = NULL;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Destroy all adopted objects.
//-------------------------------------------------------------------------
AreaRule::~AreaRule()
{
    delete this->m_Symbolization;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the AreaSymbolization2D in this AreaRule.
// RETURNS: A pointer to the AreaSymbolization2D.
//-------------------------------------------------------------------------
AreaSymbolization2D* AreaRule::GetSymbolization()
{
    return this->m_Symbolization;
}

//-------------------------------------------------------------------------
// PURPOSE: Method to adopt an AreaSymbolization2D object for the Symbolization property defined
//          in this AreaRule. The AreaSymbolization2D is applied to the polygon
//          geometric primitives of the specified features during stylization.
//          This object is now responsible for the deletion of the adopted
//          AreaSymbolization2D.
// PARAMETERS:
//      Input:
//         pflAdoptedSymbolization - A pointer to a AreaSymbolization2D subclass object created
//                          on the heap.
//-------------------------------------------------------------------------
void AreaRule::AdoptSymbolization(AreaSymbolization2D* pflAdoptedSymbolization)
{
    if (this->m_Symbolization != pflAdoptedSymbolization)
    {
        delete this->m_Symbolization;
        this->m_Symbolization = pflAdoptedSymbolization;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Method to orphan the AreaSymbolization2D object being held by this rule.
//          The AreaSymbolization2D is applied to the polygon geometric primitives of the
//          specified features during stylization. The caller is now
//          responsible for the deletion of the returned AreaSymbolization2D object.
// RETURNS: Pointer to the orphaned AreaSymbolization2D object. It may be NULL if it wasn't
//          defined.
//-------------------------------------------------------------------------
AreaSymbolization2D* AreaRule::OrphanSymbolization()
{
    AreaSymbolization2D* pRet = this->m_Symbolization;
    this->m_Symbolization = NULL;
    return pRet;
}
