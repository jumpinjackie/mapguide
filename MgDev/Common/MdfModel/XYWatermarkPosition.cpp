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
// The XYWatermarkPosition class implementation.
// XYWatermarkPosition class is one implementation of WatermarkPosition to
// define the position of watermark.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "XYWatermarkPosition.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the XYWatermarkPosition class.
//-------------------------------------------------------------------------
XYWatermarkPosition::XYWatermarkPosition()
{
    this->m_XPosition = NULL;
    this->m_YPosition = NULL;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Release any dynamically allocated resources.
//-------------------------------------------------------------------------
XYWatermarkPosition::~XYWatermarkPosition()
{
    delete this->m_XPosition;
    delete this->m_YPosition;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the XPosition property.
//          The WatermarkXOffset is the type of offset used in the position of watermark.
// RETURNS: The pointer of WatermarkXOffset.
//-------------------------------------------------------------------------
const WatermarkXOffset* XYWatermarkPosition::GetXPosition() const
{
    return this->m_XPosition;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the XPosition property.
//          The WatermarkXOffset is the type of offset used in the position of watermark.
// RETURNS: The pointer of WatermarkXOffset.
//-------------------------------------------------------------------------
WatermarkXOffset* XYWatermarkPosition::GetXPosition()
{
    return this->m_XPosition;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the XPosition property.
//          The WatermarkXOffset is the type of offset used in the position of watermark.
// PARAMETERS:
//        Input:
//          pXPosition - Adopted WatermarkXOffset object that is created on the heap.
//                             It may be NULL.
//-------------------------------------------------------------------------
void XYWatermarkPosition::AdoptXPosition(WatermarkXOffset* pXPosition)
{
    if (this->m_XPosition != pXPosition)
    {
        delete this->m_XPosition;
        this->m_XPosition = pXPosition;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the XPosition property.
//          The WatermarkXOffset is the type of offset used in the position of watermark.
// RETURNS: The pointer to the orphaned WatermarkXOffset object. It may be NULL.
//-------------------------------------------------------------------------
WatermarkXOffset* XYWatermarkPosition::OrphanXPosition()
{
    WatermarkXOffset* pRet = this->m_XPosition;
    this->m_XPosition = NULL;
    return pRet;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the YPosition property.
//          The WatermarkYOffset is the type of offset used in the position of watermark.
// RETURNS: The pointer of WatermarkYOffset.
//-------------------------------------------------------------------------
const WatermarkYOffset* XYWatermarkPosition::GetYPosition() const
{
    return this->m_YPosition;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the YPosition property.
//          The WatermarkYOffset is the type of offset used in the position of watermark.
// RETURNS: The pointer of WatermarkYOffset.
//-------------------------------------------------------------------------
WatermarkYOffset* XYWatermarkPosition::GetYPosition()
{
    return this->m_YPosition;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the YPosition property.
//          The WatermarkYOffset is the type of offset used in the position of watermark.
// PARAMETERS:
//        Input:
//          pYPosition - Adopted WatermarkYOffset object that is created on the heap.
//                             It may be NULL.
//-------------------------------------------------------------------------
void XYWatermarkPosition::AdoptYPosition(WatermarkYOffset* pYPosition)
{
    if (this->m_YPosition != pYPosition)
    {
        delete this->m_YPosition;
        this->m_YPosition = pYPosition;
    }
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the YPosition property.
//          The WatermarkYOffset is the type of offset used in the position of watermark.
// RETURNS: The pointer to the orphaned WatermarkYOffset object. It may be NULL.
//-------------------------------------------------------------------------
WatermarkYOffset* XYWatermarkPosition::OrphanYPosition()
{
    WatermarkYOffset* pRet = this->m_YPosition;
    this->m_YPosition = NULL;
    return pRet;
}

//-------------------------------------------------------------------------
// Determines whether this position is the same as the supplied one.
//-------------------------------------------------------------------------
bool XYWatermarkPosition::Equals(WatermarkPosition* another)
{
    XYWatermarkPosition* anotherPosition = dynamic_cast<XYWatermarkPosition*>(another);
    if (!anotherPosition)
        return false;

    // check X position
    if (!this->m_XPosition)
    {
        if (anotherPosition->m_XPosition)
            return false;
    }
    else if (!this->m_XPosition->Equals(anotherPosition->m_XPosition))
        return false;

    // check Y position
    if (!this->m_YPosition)
    {
        if (anotherPosition->m_YPosition)
            return false;
    }
    else if (!this->m_YPosition->Equals(anotherPosition->m_YPosition))
        return false;

    // all checks passed
    return true;
}

#ifdef _WIN32
#ifdef _DEBUG
int XYWatermarkPosition::DumpMemoryLeaks()
{
    return _CrtDumpMemoryLeaks();
}
#endif
#endif
