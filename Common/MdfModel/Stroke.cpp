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
// The Stroke class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "Stroke.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the Stroke class.
//-------------------------------------------------------------------------
Stroke::Stroke()
{
    // default values
    this->m_eUnit = MdfModel::Centimeters;
    this->m_strLineStyle = L"Solid"; // NOXLATE
    this->m_strThickness = L"0.0"; // NOXLATE
    this->m_strColor = L"ff000000"; // NOXLATE
    this->m_eSizeContext = MdfModel::DeviceUnits;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor.
//-------------------------------------------------------------------------
Stroke::~Stroke()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the LineStyle property defined in this
//          FlatStroke. The LineSyle is stored internally and referenced
//          via a name.  Each LineStyle will contain atleast a specification
//          for dash/space sequences, plus decorations.
// RETURNS: The LineStyle string.
//-------------------------------------------------------------------------
const MdfString& Stroke::GetLineStyle() const
{
    return this->m_strLineStyle;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the LineStyle property defined in this
//          FlatStroke. The LineSyle is stored internally and referenced
//          via a name.  Each LineStyle will contain atleast a specification
//          for dash/space sequences, plus decorations.
// PARAMETERS:
//      Input:
//          strLineStyle - The LineStyle string.
//-------------------------------------------------------------------------
void Stroke::SetLineStyle(const MdfString& strLineStyle)
{
    this->m_strLineStyle = strLineStyle;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Unit property in this Symbol.
//          The Unit enum defines the measurement unit for the SizeX and SizeY properties.
// RETURNS: The Unit enum value.
//-------------------------------------------------------------------------
LengthUnit Stroke::GetUnit() const
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
void Stroke::SetUnit(LengthUnit eUnit)
{
    this->m_eUnit = eUnit;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Thickness property defined in this
//          FlatStroke. The Thickness property is a Expression:Double, which is
//          an expression that evaluates to a double for a given feature.
//          It is used for defining the thickness in meters of the LineStyle
//          to be displayed.
// RETURNS: The string representation of the Thickness expression.
//-------------------------------------------------------------------------
const MdfString& Stroke::GetThickness() const
{
    return this->m_strThickness;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Thickness property defined in this
//          FlatStroke. The Thickness property is a Expression:Double, which is
//          an expression that evaluates to an integer for a given feature.
//          It is used for defining the thickness of the LineStyle to be
//          displayed in meters.
// PARAMETERS:
//      Input:
//       strThicknessExpr - The string representation of the Thickness
//                          expression.
//-------------------------------------------------------------------------
void Stroke::SetThickness(const MdfString& strThicknessExpr)
{
    this->m_strThickness = strThicknessExpr;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Color property in this Fill.
//          Color is of type Expression:Color, which is an expression
//          that evaluates to a color. This property represents the color that
//          will be applied to the Hatch pattern.
// RETURNS: The string representation of the Expression:Color.
//-------------------------------------------------------------------------
const MdfString& Stroke::GetColor() const
{
    return this->m_strColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Color property in this Fill.
//          Color is of type Expression:Color, which is an expression
//          that evaluates to a color. This property represents the color that
//          will be applied to the Hatch pattern. This method does not test
//          the argument to be a valid expression.
// PARAMETERS:
//      Input:
//         strColor - The string representation of the Expression:Color.
//-------------------------------------------------------------------------
void Stroke::SetColor(const MdfString& strColor)
{
    this->m_strColor = strColor;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
SizeContext Stroke::GetSizeContext() const
{
    return this->m_eSizeContext;
}

//-------------------------------------------------------------------------
// PURPOSE:
// PARAMETERS:
//-------------------------------------------------------------------------
void Stroke::SetSizeContext(SizeContext sizeContext)
{
    this->m_eSizeContext = sizeContext;
}
