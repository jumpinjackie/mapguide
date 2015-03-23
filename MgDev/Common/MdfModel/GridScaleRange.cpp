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
// The GridScaleRange class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "GridScaleRange.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// Contants : Represents the maximum value for the map scale.
//-------------------------------------------------------------------------
const double GridScaleRange::MAX_MAP_SCALE = 1000000000000.0; // one trillion

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the GridScaleRange class.
//-------------------------------------------------------------------------
GridScaleRange::GridScaleRange()
{
    // default values
    this->m_dMinScale = 0.0;
    this->m_dMaxScale = MAX_MAP_SCALE;
    this->m_dRebuildFactor = 1.0;
    this->m_spSurfaceStyle = NULL;
    this->m_spColorStyle = NULL;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor.
//-------------------------------------------------------------------------
GridScaleRange::~GridScaleRange()
{
    delete this->m_spSurfaceStyle;
    delete this->m_spColorStyle;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the minimum value component of this GridScaleRange.
//          The MinScale value is inclusive i.e. If the Map's scale falls on
//          the MinScale value it is deemed to be within the GridScaleRange.
//          GridScaleRange is used to define a range of values for the map's
//          scale range to fall within to make it "active".
// RETURNS: The minimum scale.
//-------------------------------------------------------------------------
double GridScaleRange::GetMinScale() const
{
    return this->m_dMinScale;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the minimum value component of this GridScaleRange.
//          The MinScale value is inclusive i.e. If the Map's scale falls on
//          the MinScale value it is deemed to be within the GridScaleRange.
//          GridScaleRange is used to define a range of values for the map's
//          scale range to fall within to make it "active".
// PARAMETERS:
//      Input:
//          dMinScale - the minimum scale.
//-------------------------------------------------------------------------
void GridScaleRange::SetMinScale(const double& dMinScale)
{
    this->m_dMinScale = dMinScale;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the maximum value component of this GridScaleRange.
//          GridScaleRange is used to define a range of values for the map's
//          scale range to fall within to make it "active".
// RETURNS: The maximum scale.
//-------------------------------------------------------------------------
double GridScaleRange::GetMaxScale() const
{
    return this->m_dMaxScale;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the maximum value component of this GridScaleRange.
//          GridScaleRange is used to define a range of values for the map's
//          scale range to fall within to make it "active".
// PARAMETERS:
//      Input:
//          dMaxScale - The maximum scale.
//-------------------------------------------------------------------------
void GridScaleRange::SetMaxScale(const double& dMaxScale)
{
    this->m_dMaxScale = dMaxScale;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the surface style component of this GridScaleRange.
//          GridScaleRange is used to define a range of values for the map's
//          scale range to fall within to make it "active".
// RETURNS: The surface style pointer.
//-------------------------------------------------------------------------
const MdfModel::GridSurfaceStyle* GridScaleRange::GetSurfaceStyle() const
{
    return this->m_spSurfaceStyle;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the surface style component of this GridScaleRange.
//          GridScaleRange is used to define a range of values for the map's
//          scale range to fall within to make it "active".
// RETURNS: The surface style pointer.
//-------------------------------------------------------------------------
MdfModel::GridSurfaceStyle* GridScaleRange::GetSurfaceStyle()
{
    return this->m_spSurfaceStyle;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the surface style component of this GridScaleRange.
//          GridScaleRange is used to define a range of values for the map's
//          scale range to fall within to make it "active".
// PARAMETERS:
//        Input:
//          pSurfaceStyle - Adopted GridSurfaceStyle object that is created on the heap.
//                             It may be NULL.
//-------------------------------------------------------------------------
void GridScaleRange::AdoptSurfaceStyle(MdfModel::GridSurfaceStyle *pSurfaceStyle)
{
    if (this->m_spSurfaceStyle != pSurfaceStyle)
    {
        delete this->m_spSurfaceStyle;
        this->m_spSurfaceStyle = pSurfaceStyle;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the surface style component of this GridScaleRange.
//          GridScaleRange is used to define a range of values for the map's
//          scale range to fall within to make it "active".
// RETURNS: The pointer to the orphaned GridSurfaceStyle object. It may be NULL.
//-------------------------------------------------------------------------
MdfModel::GridSurfaceStyle* GridScaleRange::OrphanSurfaceStyle()
{
    GridSurfaceStyle* pRet = this->m_spSurfaceStyle;
    this->m_spSurfaceStyle = NULL;
    return pRet;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the color style component of this GridScaleRange.
//          GridScaleRange is used to define a range of values for the map's
//          scale range to fall within to make it "active".
// RETURNS: The color style pointer.
//-------------------------------------------------------------------------
const MdfModel::GridColorStyle* GridScaleRange::GetColorStyle() const
{
    return this->m_spColorStyle;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the color style component of this GridScaleRange.
//          GridScaleRange is used to define a range of values for the map's
//          scale range to fall within to make it "active".
// RETURNS: The color style pointer.
//-------------------------------------------------------------------------
MdfModel::GridColorStyle* GridScaleRange::GetColorStyle()
{
    return this->m_spColorStyle;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the color style component of this GridScaleRange.
//          GridScaleRange is used to define a range of values for the map's
//          scale range to fall within to make it "active".
// PARAMETERS:
//        Input:
//          pColorStyle - Adopted GridColorStyle object that is created on the heap.
//                             It may be NULL.
//-------------------------------------------------------------------------
void GridScaleRange::AdoptColorStyle(MdfModel::GridColorStyle *pColorStyle)
{
    if (this->m_spColorStyle != pColorStyle)
    {
        delete this->m_spColorStyle;
        this->m_spColorStyle = pColorStyle;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the color style component of this GridScaleRange.
//          GridScaleRange is used to define a range of values for the map's
//          scale range to fall within to make it "active".
// RETURNS: The pointer to the orphaned GridColorStyle object. It may be NULL.
//-------------------------------------------------------------------------
MdfModel::GridColorStyle* GridScaleRange::OrphanColorStyle()
{
    GridColorStyle* pRet = this->m_spColorStyle;
    this->m_spColorStyle = NULL;
    return pRet;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the rebuild factor component of this GridScaleRange.
//          GridScaleRange is used to define a range of values for the map's
//          scale range to fall within to make it "active".
// RETURNS: The rebuild factor.
//-------------------------------------------------------------------------
double GridScaleRange::GetRebuildFactor()
{
    return this->m_dRebuildFactor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the rebuild factor component of this GridScaleRange.
//          GridScaleRange is used to define a range of values for the map's
//          scale range to fall within to make it "active".
// PARAMETERS:
//      Input:
//          dRebuildFactor - The rebuild factor.
//-------------------------------------------------------------------------
void GridScaleRange::SetRebuildFactor(double dRebuildFactor)
{
    this->m_dRebuildFactor = dRebuildFactor;
}
