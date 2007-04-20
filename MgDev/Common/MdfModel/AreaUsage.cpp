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
// The AreaUsage class implementation.
//-------------------------------------------------------------------------

#include "AreaUsage.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the AreaUsage class.
//-------------------------------------------------------------------------
AreaUsage::AreaUsage()
{
    // default values
    // NOTE: update IOAreaUsage::Write if you change these values
    this->m_sAngleControl = L"\'FromGeometry\'";
    this->m_sOriginControl = L"\'Global\'";
    this->m_sClippingControl = L"\'Clip\'";
    this->m_sAngle = L"0.0";
    this->m_sOriginX = L"0.0";
    this->m_sOriginY = L"0.0";
    this->m_sRepeatX = L"0.0";
    this->m_sRepeatY = L"0.0";
    this->m_sBufferWidth = L"0.0";
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all objects that have been created on the
//          heap or have been adopted.
//-------------------------------------------------------------------------
AreaUsage::~AreaUsage()
{
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& AreaUsage::GetAngleControl() const
{
    return this->m_sAngleControl;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void AreaUsage::SetAngleControl(const MdfString& angleControl)
{
    this->m_sAngleControl = angleControl;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& AreaUsage::GetOriginControl() const
{
    return this->m_sOriginControl;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void AreaUsage::SetOriginControl(const MdfString& originControl)
{
    this->m_sOriginControl = originControl;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& AreaUsage::GetClippingControl() const
{
    return this->m_sClippingControl;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void AreaUsage::SetClippingControl(const MdfString& clippingControl)
{
    this->m_sClippingControl = clippingControl;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& AreaUsage::GetAngle() const
{
    return this->m_sAngle;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void AreaUsage::SetAngle(const MdfString& angle)
{
    this->m_sAngle = angle;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& AreaUsage::GetOriginX() const
{
    return this->m_sOriginX;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void AreaUsage::SetOriginX(const MdfString& originX)
{
    this->m_sOriginX = originX;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& AreaUsage::GetOriginY() const
{
    return this->m_sOriginY;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void AreaUsage::SetOriginY(const MdfString& originY)
{
    this->m_sOriginY = originY;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& AreaUsage::GetRepeatX() const
{
    return this->m_sRepeatX;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void AreaUsage::SetRepeatX(const MdfString& repeatX)
{
    this->m_sRepeatX = repeatX;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& AreaUsage::GetRepeatY() const
{
    return this->m_sRepeatY;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void AreaUsage::SetRepeatY(const MdfString& repeatY)
{
    this->m_sRepeatY = repeatY;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& AreaUsage::GetBufferWidth() const
{
    return this->m_sBufferWidth;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void AreaUsage::SetBufferWidth(const MdfString& bufferWidth)
{
    this->m_sBufferWidth = bufferWidth;
}
