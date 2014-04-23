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

#include "MgDesktop.h"
#include "UnitType.h"

///////////////////////////////////////////////////////////////////////////////
/// MgdUnitType defines enumerated values used to determine the type
/// of units for plot decorations (scale bar).
///
const STRING MgdUnitType::USEnglish       = L"US-English";
const STRING MgdUnitType::Metric          = L"Metric";


///////////////////////////////////////////////////////////////////////////////
/// Page size units for Mapping Service.
///
const STRING MgdPageUnitsType::Inches        = L"in";
const STRING MgdPageUnitsType::Millimeters   = L"mm";

///////////////////////////////////////////////////////////////////////////////
/// Coordinate space for geometry returned from QueryFeatureProperties.
///
const STRING MgdCoordinateSpace::None        = L"None";
const STRING MgdCoordinateSpace::Map         = L"Map";
const STRING MgdCoordinateSpace::Display     = L"Display";