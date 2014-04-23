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
// The SupplementalSpatialContextInfo class implementation.
//-------------------------------------------------------------------------

#include "stdafx.h"
#include "SupplementalSpatialContextInfo.h"

using namespace MDFMODEL_NAMESPACE;

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the SupplementalSpatialContextInfo
//          class.
// PARAMETERS:
//     Input:
//          strName - the name
//          strCoordinateSystem - the coordinate system
//-------------------------------------------------------------------------
SupplementalSpatialContextInfo::SupplementalSpatialContextInfo(const MdfString& strName, const MdfString& strCoordinateSystem)
: m_strName(strName)
, m_strCoordinateSystem(strCoordinateSystem)
{
}

//-------------------------------------------------------------------------
// PURPOSE: Construct and initialize an instance of the SupplementalSpatialContextInfo
//          class.
// PARAMETERS:
//     Input:
//          strName - the name
//-------------------------------------------------------------------------
SupplementalSpatialContextInfo::SupplementalSpatialContextInfo()
{
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Name property in this SupplementalSpatialContextInfo.
// RETURNS: The name of the spatial content.
//-------------------------------------------------------------------------
const MdfString& SupplementalSpatialContextInfo::GetName() const
{
    return this->m_strName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the Name property in this SupplementalSpatialContextInfo.
// PARAMETERS:
//      Input:
//         strName - The name.
//-------------------------------------------------------------------------
void SupplementalSpatialContextInfo::SetName(const MdfString& strName)
{
    this->m_strName = strName;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the CoordinateSystem property in this SupplementalSpatialContextInfo.
// RETURNS: The value of the coorindate system used for the spatial context
//-------------------------------------------------------------------------
const MdfString& SupplementalSpatialContextInfo::GetCoordinateSystem() const
{
    return this->m_strCoordinateSystem;
}

//-------------------------------------------------------------------------
// PURPOSE: Accessor method to the CoordinateSystem property in this SupplementalSpatialContextInfo.
// PARAMETERS:
//      Input:
//         strCoordinateSystem - The value associate with the name.
//-------------------------------------------------------------------------
void SupplementalSpatialContextInfo::SetCoordinateSystem(const MdfString& strCoordinateSystem)
{
    this->m_strCoordinateSystem = strCoordinateSystem;
}
