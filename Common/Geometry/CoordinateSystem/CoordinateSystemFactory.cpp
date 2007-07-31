//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#include "GeometryCommon.h"
#include "CoordinateSystemCommon.h"

MgCoordinateSystemCache MgCoordinateSystemFactory::sm_coordinateSystemCache;

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Constructs and intializes a coordinate system factory.
///</summary>
MgCoordinateSystemFactory::MgCoordinateSystemFactory()
{
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Destructor.
///</summary>
MgCoordinateSystemFactory::~MgCoordinateSystemFactory()
{
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Creates a coordinate system instance given a definition of the
/// coordinate system in OpenGIS Well-Known Text (WKT) format.
///</summary>
///<param name="srsWkt">
/// A string defining the coordinate system in OpenGIS WKT format.
///</param>
///<returns>
/// An MgCoordinateSystem instance that corresponds to the specified WKT
/// definition.
///</returns>
MgCoordinateSystem* MgCoordinateSystemFactory::Create(CREFSTRING srsWkt)
{
    return sm_coordinateSystemCache.GetCoordinateSystem(srsWkt);
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgCoordinateSystemFactory::GetClassId()
{
    return m_cls_id;
}

// <summary>Dispose this object.</summary>
// <returns>Returns nothing</returns>
void MgCoordinateSystemFactory::Dispose()
{
    delete this;
}
