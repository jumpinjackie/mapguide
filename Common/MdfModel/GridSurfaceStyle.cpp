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
#include "GridSurfaceStyle.h"
#include "IFeatureTypeStyleVisitor.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the GridSurfaceStyle class
//-------------------------------------------------------------------------
GridSurfaceStyle::GridSurfaceStyle()
{
    // default values
    this->m_dZeroValue = 0.0;
    this->m_dScaleFactor = 1.0;
}

//-------------------------------------------------------------------------
// PURPOSE: Destructor.
//-------------------------------------------------------------------------
GridSurfaceStyle::~GridSurfaceStyle()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Band property.
//          The Band is a string to specify the band's name.
// RETURNS: The Band
//-------------------------------------------------------------------------
const MdfString& GridSurfaceStyle::GetBand() const
{
    return this->m_strElevationBandName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the Band property.
//          The Band is a string to specify the band's name.
// PARAMETERS:
//      Input:
//          strElevationBandName - The band's name.
//-------------------------------------------------------------------------
void GridSurfaceStyle::SetBand(const MdfString& strElevationBandName)
{
    this->m_strElevationBandName = strElevationBandName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ZeroValue property.
//          The ZeroValue is a double to specify the value that should
//          show as zero elevation.
// RETURNS: The zero value.
//-------------------------------------------------------------------------
double GridSurfaceStyle::GetZeroValue() const
{
    return this->m_dZeroValue;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ZeroValue property.
//          The ZeroValue is a double to specify the value that should
//          show as zero elevation.
// PARAMETERS:
//      Input:
//          dZeroValue - The zero value.
//-------------------------------------------------------------------------
void GridSurfaceStyle::SetZeroValue(double dZeroValue)
{
    this->m_dZeroValue = dZeroValue;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ScaleFactor property.
//          The ScaleFactor is a double to specify how to scale the elevation data.
// RETURNS: The scale factor.
//-------------------------------------------------------------------------
double GridSurfaceStyle::GetScaleFactor() const
{
    return this->m_dScaleFactor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the ScaleFactor property.
//          The ScaleFactor is a double to specify how to scale the elevation data.
// PARAMETERS:
//      Input:
//          dScaleFactor - The scale factor.
//-------------------------------------------------------------------------
void GridSurfaceStyle::SetScaleFactor(double dScaleFactor)
{
    this->m_dScaleFactor = dScaleFactor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the DefaultColor property.
//          The DefaultColor is a string to specify the default color for a 3d surface.
// RETURNS: The default color
//-------------------------------------------------------------------------
const MdfString& GridSurfaceStyle::GetDefaultColor() const
{
    return this->m_strDefaultColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method for the DefaultColor property.
//          The DefaultColor is a string to specify the default color for a 3d surface.
// PARAMETERS:
//      Input:
//          strDefaultColor - The default color.
//-------------------------------------------------------------------------
void GridSurfaceStyle::SetDefaultColor(const MdfString& strDefaultColor)
{
    this->m_strDefaultColor = strDefaultColor;
}

//-------------------------------------------------------------------------
// PURPOSE: Implementation of the Visitor pattern. The pure, virtual method
//          is defined in FeatureTypeStyle and is implemented by all of its
//          concrete subclasses.
// PARAMETERS:
//      Input:
//          iftsVisitor - The IFeatureTypeStyleVisitor interface which sports
//                        methods that accept the final concrete type this
//                        GridSurfaceStyle represents as an argument.
//-------------------------------------------------------------------------
void GridSurfaceStyle::AcceptVisitor(IFeatureTypeStyleVisitor& iftsVisitor)
{
    iftsVisitor.VisitGridSurfaceStyle(*this);
}
