//
//  Copyright (C) 2007 by Autodesk, Inc.
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
// The PointUsage class implementation.
//-------------------------------------------------------------------------

#include "PointUsage.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the PointUsage class.
//-------------------------------------------------------------------------
PointUsage::PointUsage()
{
    // NOTE: if you change any defaults then you must also update
    //       IOPointUsage::Write to use the correct default values.
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all objects that have been created on the
//          heap or have been adopted.
//-------------------------------------------------------------------------
PointUsage::~PointUsage()
{
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& PointUsage::GetAngleControl() const
{
    return this->m_sAngleControl;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void PointUsage::SetAngleControl(const MdfString& angleControl)
{
    this->m_sAngleControl = angleControl;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& PointUsage::GetAngle() const
{
    return this->m_sAngle;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void PointUsage::SetAngle(const MdfString& angle)
{
    this->m_sAngle = angle;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& PointUsage::GetOriginOffsetX() const
{
    return this->m_sOriginOffsetX;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void PointUsage::SetOriginOffsetX(const MdfString& originOffsetX)
{
    this->m_sOriginOffsetX = originOffsetX;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& PointUsage::GetOriginOffsetY() const
{
    return this->m_sOriginOffsetY;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void PointUsage::SetOriginOffsetY(const MdfString& originOffsetY)
{
    this->m_sOriginOffsetY = originOffsetY;
}
