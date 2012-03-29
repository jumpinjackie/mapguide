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
// Symbol class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "Symbol.h"
#include "LengthConverter.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the Symbol class.
//-------------------------------------------------------------------------
Symbol::Symbol()
{
    // default values
    // 3 millimeters in default space (screen), and default units (centimeters)
    this->m_strSizeX = L"0.3"; // NOXLATE
    this->m_strSizeY = L"0.3"; // NOXLATE
    this->m_strInsertionPointX = L"0.5"; // NOXLATE
    this->m_strInsertionPointY = L"0.5"; // NOXLATE
    this->m_bMaintainAspect = true;
    this->m_eSizeContext = MdfModel::DeviceUnits;
    this->m_eUnit = MdfModel::Centimeters;
    this->m_strRotation = L"0.0";
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor.
//-------------------------------------------------------------------------
Symbol::~Symbol()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the X size property on this Symbol.  The
//          X size is stored in the specified units.
// RETURNS: The string representation of the Expression:Double.
//-------------------------------------------------------------------------
const MdfString& Symbol::GetSizeX() const
{
    return this->m_strSizeX;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the S size property on this Symbol.  The
//          X size is stored in the specified units.
// PARAMETERS:
//      Input:
//          strSizeX - The string representation of the Expression:Double.
//-------------------------------------------------------------------------
void Symbol::SetSizeX(const MdfString& strSizeX)
{
    this->m_strSizeX = strSizeX;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Y size property on this Symbol.  The
//          Y size is stored in the specified units.
// RETURNS: The string representation of the Expression:Double.
//-------------------------------------------------------------------------
const MdfString& Symbol::GetSizeY() const
{
    return this->m_strSizeY;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Y size property on this Symbol.  The
//          Y size is stored in the specified units.
// PARAMETERS:
//      Input:
//          strSizeY - The string representation of the Expression:Double.
//-------------------------------------------------------------------------
void Symbol::SetSizeY(const MdfString& strSizeY)
{
    this->m_strSizeY = strSizeY;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the X Insertion Point property on this Symbol.  The
//          X Insertion Point value ranges from 0 to 1 horizontally across the symbol.
// RETURNS: The string representation of the Expression:Double.
//-------------------------------------------------------------------------
const MdfString& Symbol::GetInsertionPointX() const
{
    return this->m_strInsertionPointX;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the S Insertion Point property on this Symbol.  The
//          X Insertion Point value ranges from 0 to 1 horizontally across the symbol.
// PARAMETERS:
//      Input:
//          strInsertionPointX - The string representation of the Expression:Double.
//-------------------------------------------------------------------------
void Symbol::SetInsertionPointX(const MdfString& strInsertionPointX)
{
    this->m_strInsertionPointX = strInsertionPointX;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Y Insertion Point property on this Symbol.  The
//          Y Insertion Point value ranges from 0 to 1 vertically across the symbol.
// RETURNS: The string representation of the Expression:Double.
//-------------------------------------------------------------------------
const MdfString& Symbol::GetInsertionPointY() const
{
    return this->m_strInsertionPointY;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Y Insertion Point property on this Symbol.  The
//          Y Insertion Point value ranges from 0 to 1 vertically across the symbol.
// PARAMETERS:
//      Input:
//          strInsertionPointY - The string representation of the Expression:Double.
//-------------------------------------------------------------------------
void Symbol::SetInsertionPointY(const MdfString& strInsertionPointY)
{
    this->m_strInsertionPointY = strInsertionPointY;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the MaintainAspect property on this Symbol.
//          The property defines whether the width and height should maintain
//          their current aspect ratio while being edited by the UI.
// RETURNS: Boolean. Default is true.
//-------------------------------------------------------------------------
bool Symbol::GetMaintainAspect() const
{
    return this->m_bMaintainAspect;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the MaintainAspect property on this Symbol.
//          The property defines whether the width and height should maintain
//          their current aspect ratio while being edited by the UI.
// PARAMETERS:
//      Input:
//          bMaintainAspect - Boolean. Default is true.
//-------------------------------------------------------------------------
void Symbol::SetMaintainAspect(bool bMaintainAspect)
{
    this->m_bMaintainAspect = bMaintainAspect;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Rotation property of this Symbol.
//          The Rotation is a Expression:Double expression, which
//          evaluates to a double value for a given feature. This value
//          is the Rotation applied to the Symbol for that feature.
// RETURNS: A string representation of the Rotation expression.
//-------------------------------------------------------------------------
const MdfString& Symbol::GetRotation() const
{
    return this->m_strRotation;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Rotation property of this Symbol.
//          The Rotation is a Expression:Double expression, which
//          evaluates to a double value for a given feature. This value
//          is the Rotation applied to the Symbol for that feature.
// PARAMETERS:
//      Input:
//          strRotationExpr - the string representation of the expression.
//-------------------------------------------------------------------------
void Symbol::SetRotation(const MdfString& strRotationExpr)
{
    this->m_strRotation = strRotationExpr;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the SizeContext property in this Symbol.
//          The SizeContext enum defines whether the SizeX and SizeY properties
//          are in screen space or map space.
// RETURNS: The SizeContext enum value.
//-------------------------------------------------------------------------
SizeContext Symbol::GetSizeContext() const
{
    return this->m_eSizeContext;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the SizeContext property in this Symbol.
//          The SizeContext enum defines whether the SizeX and SizeY properties
//          are in screen space or map space.
// PARAMETERS:
//      Input:
//          eSizeContext - the new context for the SizeX and SizeY.
//-------------------------------------------------------------------------
void Symbol::SetSizeContext(SizeContext eSizeContext)
{
    this->m_eSizeContext = eSizeContext;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Unit property in this Symbol.
//          The Unit enum defines the measurement unit for the SizeX and SizeY properties.
// RETURNS: The Unit enum value.
//-------------------------------------------------------------------------
LengthUnit Symbol::GetUnit() const
{
    return this->m_eUnit;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Unit property in this Symbol.
//          The Unit enum defines the measurement unit for the SizeX and SizeY properties.
// PARAMETERS:
//      Input:
//          eUnit - the measurement unit for the SizeX and SizeY.
//-------------------------------------------------------------------------
void Symbol::SetUnit(LengthUnit eUnit)
{
    this->m_eUnit = eUnit;
}
