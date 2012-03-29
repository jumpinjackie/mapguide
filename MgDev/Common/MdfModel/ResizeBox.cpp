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
// The ResizeBox class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "ResizeBox.h"

using namespace MDFMODEL_NAMESPACE;

// initialize values for string properties
const wchar_t* ResizeBox::sGrowControlDefault = L"'GrowInXYMaintainAspect'";                        // NOXLATE
const wchar_t* ResizeBox::sGrowControlValues  = L"GrowInX|GrowInY|GrowInXY|GrowInXYMaintainAspect"; // NOXLATE


//-------------------------------------------------------------------------
// PURPOSE: Initialize an instance of the ResizeBox class.
//-------------------------------------------------------------------------
ResizeBox::ResizeBox()
{
    // default values
    // NOTE: values used in IOResizeBox::Write must match these
    this->m_sSizeX       = L"1.0"; // NOXLATE
    this->m_sSizeY       = L"1.0"; // NOXLATE
    this->m_sPositionX   = L"0.0"; // NOXLATE
    this->m_sPositionY   = L"0.0"; // NOXLATE
    this->m_sGrowControl = ResizeBox::sGrowControlDefault;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor. Delete all objects that have been created on the
//          heap or have been adopted.
//-------------------------------------------------------------------------
ResizeBox::~ResizeBox()
{
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& ResizeBox::GetSizeX() const
{
    return this->m_sSizeX;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void ResizeBox::SetSizeX(const MdfString& sizeX)
{
    this->m_sSizeX = sizeX;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& ResizeBox::GetSizeY() const
{
    return this->m_sSizeY;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void ResizeBox::SetSizeY(const MdfString& sizeY)
{
    this->m_sSizeY = sizeY;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& ResizeBox::GetPositionX() const
{
    return this->m_sPositionX;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void ResizeBox::SetPositionX(const MdfString& positionX)
{
    this->m_sPositionX = positionX;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& ResizeBox::GetPositionY() const
{
    return this->m_sPositionY;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void ResizeBox::SetPositionY(const MdfString& positionY)
{
    this->m_sPositionY = positionY;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
const MdfString& ResizeBox::GetGrowControl() const
{
    return this->m_sGrowControl;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void ResizeBox::SetGrowControl(const MdfString& growControl)
{
    this->m_sGrowControl = growControl;
}
