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
// The WatermarkXOffset class implementation.
// WatermarkXOffset class is to define the offset of watermark at one dimension.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include <cmath>
#include "WatermarkXOffset.h"

using namespace MDFMODEL_NAMESPACE;

const double WatermarkXOffset::doubleTolerance = 0.01;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the WatermarkXOffset class.
//-------------------------------------------------------------------------
WatermarkXOffset::WatermarkXOffset()
{
    this->m_offset = 0.0;
    this->m_unit = WatermarkOffset::Points;
    this->m_alignment = Center;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Release any dynamically allocated resources.
//-------------------------------------------------------------------------
WatermarkXOffset::~WatermarkXOffset()
{
}

//-------------------------------------------------------------------------
// Get the offset length.
//-------------------------------------------------------------------------
double WatermarkXOffset::GetOffset() const
{
    return this->m_offset;
}

//-------------------------------------------------------------------------
// Set the offset length.
//-------------------------------------------------------------------------
void WatermarkXOffset::SetOffset(const double& dOffset)
{
    this->m_offset = dOffset;
}

//-------------------------------------------------------------------------
// PURPOSE: Gets a value determining the meaning of the Unit property value.
// RETURNS: The type of value stored in the Unit property.
//-------------------------------------------------------------------------
WatermarkOffset::WatermarkOffsetUnit WatermarkXOffset::GetUnit() const
{
    return this->m_unit;
}

//-------------------------------------------------------------------------
// PURPOSE: Sets a value determining the meaning of the Unit property value.
// PARAMETERS:
//          unit - The type of value stored in the Unit property.
//-------------------------------------------------------------------------
void WatermarkXOffset::SetUnit(WatermarkOffset::WatermarkOffsetUnit unit)
{
    this->m_unit = unit;
}

//-------------------------------------------------------------------------
// PURPOSE: Gets a value determining the meaning of the Alignment property value.
// RETURNS: The type of value stored in the Alignment property.
//-------------------------------------------------------------------------
WatermarkXOffset::HorizontalAlignment WatermarkXOffset::GetAlignment() const
{
    return this->m_alignment;
}

//-------------------------------------------------------------------------
// PURPOSE: Sets a value determining the meaning of the Alignment property value.
// PARAMETERS:
//          alignment - The type of value stored in the Alignment property.
//-------------------------------------------------------------------------
void WatermarkXOffset::SetAlignment(WatermarkXOffset::HorizontalAlignment alignment)
{
    this->m_alignment = alignment;
}

//-------------------------------------------------------------------------
// Determines whether this offset is the same as the supplied one.
//-------------------------------------------------------------------------
bool WatermarkXOffset::Equals(WatermarkXOffset* another)
{
    return another &&
           fabs(this->m_offset - another->m_offset) < doubleTolerance &&
           this->m_unit == another->m_unit &&
           this->m_alignment == another->m_alignment;
}

#ifdef _WIN32
#ifdef _DEBUG
int WatermarkXOffset::DumpMemoryLeaks()
{
    return _CrtDumpMemoryLeaks();
}
#endif
#endif
