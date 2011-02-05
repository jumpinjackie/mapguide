//
//  Copyright (C) 2010-2011 by Autodesk, Inc.
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
// The TileWatermarkPosition class implementation.
// TileWatermarkPosition class is one implementation of WatermarkPosition to
// define the position of watermark.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include <cmath>
#include "TileWatermarkPosition.h"

using namespace MDFMODEL_NAMESPACE;

const double TileWatermarkPosition::doubleTolerance = 0.01;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the TileWatermarkPosition class.
//-------------------------------------------------------------------------
TileWatermarkPosition::TileWatermarkPosition()
{
    this->m_tileWidth = 150.0;
    this->m_tileHeight = 150.0;
    this->m_horizontalPosition = NULL;
    this->m_verticalPosition = NULL;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Release any dynamically allocated resources.
//-------------------------------------------------------------------------
TileWatermarkPosition::~TileWatermarkPosition()
{
    delete this->m_horizontalPosition;
    delete this->m_verticalPosition;
}

//-------------------------------------------------------------------------
// Get the tile width
//-------------------------------------------------------------------------
double TileWatermarkPosition::GetTileWidth() const
{
    return this->m_tileWidth;
}

//-------------------------------------------------------------------------
// Set the tile width
//-------------------------------------------------------------------------
void TileWatermarkPosition::SetTileWidth(const double& dTileWidth)
{
    this->m_tileWidth = dTileWidth;
}

//-------------------------------------------------------------------------
// Get the tile height
//-------------------------------------------------------------------------
double TileWatermarkPosition::GetTileHeight() const
{
    return this->m_tileHeight;
}

//-------------------------------------------------------------------------
// Set the tile height
//-------------------------------------------------------------------------
void TileWatermarkPosition::SetTileHeight(const double& dTileHeight)
{
    this->m_tileHeight = dTileHeight;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the HorizontalPosition property.
//          The WatermarkXOffset is the type of offset used in the position of watermark.
// RETURNS: The pointer of WatermarkXOffset.
//-------------------------------------------------------------------------
const WatermarkXOffset* TileWatermarkPosition::GetHorizontalPosition() const
{
    return this->m_horizontalPosition;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the HorizontalPosition property.
//          The WatermarkXOffset is the type of offset used in the position of watermark.
// RETURNS: The pointer of WatermarkXOffset.
//-------------------------------------------------------------------------
WatermarkXOffset* TileWatermarkPosition::GetHorizontalPosition()
{
    return this->m_horizontalPosition;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the HorizontalPosition property.
//          The WatermarkXOffset is the type of offset used in the position of watermark.
// PARAMETERS:
//        Input:
//          pXPosition - Adopted WatermarkXOffset object that is created on the heap.
//                             It may be NULL.
//-------------------------------------------------------------------------
void TileWatermarkPosition::AdoptHorizontalPosition(WatermarkXOffset* pHorizontalPosition)
{
    if (this->m_horizontalPosition != pHorizontalPosition)
    {
        delete this->m_horizontalPosition;
        this->m_horizontalPosition = pHorizontalPosition;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the HorizontalPosition property.
//          The WatermarkXOffset is the type of offset used in the position of watermark.
// RETURNS: The pointer to the orphaned WatermarkXOffset object. It may be NULL.
//-------------------------------------------------------------------------
WatermarkXOffset* TileWatermarkPosition::OrphanHorizontalPosition()
{
    WatermarkXOffset* pRet = this->m_horizontalPosition;
    this->m_horizontalPosition = NULL;
    return pRet;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the VerticalPosition property.
//          The WatermarkYOffset is the type of offset used in the position of watermark.
// RETURNS: The pointer of WatermarkYOffset.
//-------------------------------------------------------------------------
const WatermarkYOffset* TileWatermarkPosition::GetVerticalPosition() const
{
    return this->m_verticalPosition;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the VerticalPosition property.
//          The WatermarkYOffset is the type of offset used in the position of watermark.
// RETURNS: The pointer of WatermarkYOffset.
//-------------------------------------------------------------------------
WatermarkYOffset* TileWatermarkPosition::GetVerticalPosition()
{
    return this->m_verticalPosition;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the VerticalPosition property.
//          The WatermarkYOffset is the type of offset used in the position of watermark.
// PARAMETERS:
//        Input:
//          pYPosition - Adopted WatermarkYOffset object that is created on the heap.
//                             It may be NULL.
//-------------------------------------------------------------------------
void TileWatermarkPosition::AdoptVerticalPosition(WatermarkYOffset* pVerticalPosition)
{
    if (this->m_verticalPosition != pVerticalPosition)
    {
        delete this->m_verticalPosition;
        this->m_verticalPosition = pVerticalPosition;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the VerticalPosition property.
//          The WatermarkYOffset is the type of offset used in the position of watermark.
// RETURNS: The pointer to the orphaned WatermarkYOffset object. It may be NULL.
//-------------------------------------------------------------------------
WatermarkYOffset* TileWatermarkPosition::OrphanVerticalPosition()
{
    WatermarkYOffset* pRet = this->m_verticalPosition;
    this->m_verticalPosition = NULL;
    return pRet;
}

//-------------------------------------------------------------------------
// Determines whether this position is the same as the supplied one.
//-------------------------------------------------------------------------
bool TileWatermarkPosition::Equals(WatermarkPosition* another)
{
    TileWatermarkPosition* anotherPosition = dynamic_cast<TileWatermarkPosition*>(another);
    if (!anotherPosition)
        return false;

    // check width / height
    if (fabs(this->m_tileWidth - anotherPosition->m_tileWidth) > doubleTolerance)
        return false;
    if (fabs(this->m_tileHeight - anotherPosition->m_tileHeight) > doubleTolerance)
        return false;

    // check horizontal position
    if (!this->m_horizontalPosition)
    {
        if (anotherPosition->m_horizontalPosition)
            return false;
    }
    else if (!this->m_horizontalPosition->Equals(anotherPosition->m_horizontalPosition))
        return false;

    // check vertical position
    if (!this->m_verticalPosition)
    {
        if (anotherPosition->m_verticalPosition)
            return false;
    }
    else if (!this->m_verticalPosition->Equals(anotherPosition->m_verticalPosition))
        return false;

    // all checks passed
    return true;
}

#ifdef _WIN32
#ifdef _DEBUG
int TileWatermarkPosition::DumpMemoryLeaks()
{
    return _CrtDumpMemoryLeaks();
}
#endif
#endif
