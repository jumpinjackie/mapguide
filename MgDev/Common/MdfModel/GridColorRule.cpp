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

#include "stdafx.h"
#include "GridColorRule.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the GridColorRule class
//-------------------------------------------------------------------------
GridColorRule::GridColorRule()
{
    this->m_spGridColor = NULL;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor.
//-------------------------------------------------------------------------
GridColorRule::~GridColorRule()
{
    delete this->m_spGridColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the GridColor property.
//          The GridColor is a GridColorType to specify how to stylize the pixel
//          of the band.
// RETURNS: The pointer of GridColor.
//-------------------------------------------------------------------------
const GridColor* GridColorRule::GetGridColor() const
{
    return this->m_spGridColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the GridColor property.
//          The GridColor is a GridColorType to specify how to stylize the pixel
//          of the band.
// RETURNS: The pointer of GridColor.
//-------------------------------------------------------------------------
GridColor* GridColorRule::GetGridColor()
{
    return this->m_spGridColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the GridColor property.
//          The GridColor is a GridColorType to specify how to stylize the pixel
//          of the band.
// PARAMETERS:
//        Input:
//          pGridColor - Adopted GridColor object that is created on the heap.
//                             It may be NULL.
//-------------------------------------------------------------------------
void GridColorRule::AdoptGridColor(GridColor *pGridColor)
{
    if (this->m_spGridColor != pGridColor)
    {
        delete this->m_spGridColor;
        this->m_spGridColor = pGridColor;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the GridColor property.
//          The GridColor is a GridColorType to specify how to stylize the pixel
//          of the band.
// RETURNS: The pointer to the orphaned GridColor object. It may be NULL.
//-------------------------------------------------------------------------
GridColor* GridColorRule::OrphanGridColor()
{
    GridColor* pRet = this->m_spGridColor;
    this->m_spGridColor = NULL;
    return pRet;
}
