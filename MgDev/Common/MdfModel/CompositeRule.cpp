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
// The CompositeRule class implementation. CompositeRule is a concrete
// subclass of Rule.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "CompositeRule.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the CompositeRule class.
//-------------------------------------------------------------------------
CompositeRule::CompositeRule()
{
    this->m_pSymbolization = NULL;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all objects that have been created on the
//          heap or have been adopted.
//-------------------------------------------------------------------------
CompositeRule::~CompositeRule()
{
    delete this->m_pSymbolization;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor for the CompositeSymbolization in this CompositeRule.
// RETURNS: A pointer to the CompositeSymbolization.
//-------------------------------------------------------------------------
CompositeSymbolization* CompositeRule::GetSymbolization()
{
    return this->m_pSymbolization;
}

//-------------------------------------------------------------------------
// PURPOSE: Method to adopt an CompositeSymbolization object for the Symbolization
//          property defined in this CompositeRule.  The CompositeSymbolization is
//          applied to the geometric primitives of the specified features during
//          stylization.  This object is now responsible for the deletion of the
//          adopted CompositeSymbolization.
// PARAMETERS:
//      Input:
//         compositeSymbolization - A pointer to a CompositeSymbolization object
//                                  created on the heap.
//-------------------------------------------------------------------------
void CompositeRule::AdoptSymbolization(CompositeSymbolization* compositeSymbolization)
{
    if (this->m_pSymbolization != compositeSymbolization)
    {
        delete this->m_pSymbolization;
        this->m_pSymbolization = compositeSymbolization;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Method to orphan the CompositeSymbolization object being held by this
//          rule.  The CompositeSymbolization is applied to the geometric primitives
//          of the specified features during stylization.  The caller is now
//          responsible for the deletion of the returned CompositeSymbolization
//          object.
// RETURNS: Pointer to the orphaned CompositeSymbolization object. It may be NULL
//          if it wasn't defined.
//-------------------------------------------------------------------------
CompositeSymbolization* CompositeRule::OrphanSymbolization()
{
    CompositeSymbolization* pRet = this->m_pSymbolization;
    this->m_pSymbolization = NULL;
    return pRet;
}
