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

#include "GeometryCommon.h"
#include "GeosUtil.h"

///////////////////////////////////////////////////////////////////////////
// Returns this geometric entities bounding box.
//
MgEnvelope* MgGeometricEntity::Envelope()
{
    return ComputeEnvelope();
}

///////////////////////////////////////////////////////////////////////////
//
//
MgGeometricPathIterator* MgGeometricEntity::GetPath()
{
    throw new MgNotImplementedException(L"MgGeometricEntityImpl.GetPath", __LINE__, __WFILE__, NULL, L"", NULL);
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgGeometricEntity::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgGeometricEntity::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Tests whether this entity is simple or not. Simple entities do not
/// contain any points of self-tangency or self intersection.
///
/// Note: Optional for MapGuide.
///</summary>
///<returns>
/// True if the geometry is simple, false otherwise.
///</returns>
bool MgGeometricEntity::IsSimple()
{
    return MgGeosUtil::IsSimple(this);
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Tests whether this entity is simple or not. Simple entities do not
/// contain any points of self-tangency or self intersection.
///
/// Note: Optional for MapGuide.
///</summary>
///<returns>
/// True if the geometry is simple, false otherwise.
///</returns>
bool MgGeometricEntity::IsValid()
{
    return MgGeosUtil::IsValid(this);
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Calculates the area for this geometric entity
///</summary>
///<returns>
/// The area of the geometric entity
///</returns>
double MgGeometricEntity::GetArea()
{
    return MgGeosUtil::Area(this);
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Calculates the length for this geometric entity
///</summary>
///<returns>
/// The length of the geometric entity
///</returns>
double MgGeometricEntity::GetLength()
{
    return MgGeosUtil::Length(this);
}


///////////////////////////////////////////////////////////////////////////
///<summary>
/// Calculates the centroid for this geometric entity
///</summary>
///<returns>
/// The ceontroid point of the geometric entity
///</returns>
MgPoint* MgGeometricEntity::GetCentroid()
{
    return MgGeosUtil::Centroid(this);
}
