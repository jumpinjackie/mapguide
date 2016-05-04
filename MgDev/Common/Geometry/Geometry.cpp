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
#include "Buffer/buffer.h"
#include "GeosUtil.h"

///////////////////////////////////////////////////////////////////////////
// Tests whether this geometry spatially contains another geometry.
//
bool MgGeometry::Contains(MgGeometry* other)
{
    bool contains = false;

    contains = MgGeosUtil::Contains(this, other);

    return contains;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this geometry spatially crosses another geometry.
//
bool MgGeometry::Crosses(MgGeometry* other)
{
    bool crosses = false;

    crosses = MgGeosUtil::Crosses(this, other);

    return crosses;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this geometry is spatially disjoint from another
// geometry.
//
bool MgGeometry::Disjoint(MgGeometry* other)
{
    bool disjoint = false;

    disjoint = MgGeosUtil::Disjoint(this, other);

    return disjoint;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this geometry is spatially equal to another geometry.
//
bool MgGeometry::Equals(MgGeometry* other)
{
    bool equals = false;

    equals = MgGeosUtil::Equals(this, other);

    return equals;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this geometry spatially intersects another geometry.
//
bool MgGeometry::Intersects(MgGeometry* other)
{
    bool intersects = false;

    intersects = MgGeosUtil::Intersects(this, other);

    return intersects;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this geometry spatially overlaps another geometry.
//
bool MgGeometry::Overlaps(MgGeometry* other)
{
    bool overlaps = false;

    overlaps = MgGeosUtil::Overlaps(this, other);

    return overlaps;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this geometry spatially touches another geometry.
//
bool MgGeometry::Touches(MgGeometry* other)
{
    bool touches = false;

    touches = MgGeosUtil::Touches(this, other);

    return touches;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this geometry is spatially within another geometry.
//
bool MgGeometry::Within(MgGeometry* other)
{
    bool within = false;

    within = MgGeosUtil::Within(this, other);

    return within;
}

///////////////////////////////////////////////////////////////////////////
// Returns the closure of the combinitorial boundary of this geometric
// entity as a topologically closed geometry.
//
MgGeometry* MgGeometry::Boundary()
{
    return MgGeosUtil::Boundary(this);
}


///////////////////////////////////////////////////////////////////////////
// Returns a geometry that represents all points whose distance from this
// geometric entity is less than or equal to a given distance. By default
// distance computation is done using a linear algorithm unless a custom
// measure is supplied.
//
MgGeometry* MgGeometry::Buffer(double distance, MgMeasure* measure)
{
    if (distance == 0.0)
    {
        STRING buffer;
        MgUtil::DoubleToString(distance, buffer);

        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgGeometry.Buffer",
            __LINE__, __WFILE__, &arguments, L"MgValueCannotBeZero", NULL);
    }

    MgBuffer buffer;
    return buffer.CreateBuffer(this, distance, measure);
}

///////////////////////////////////////////////////////////////////////////
// Returns a geometry that represents the convex hull of this geometric
// entity.
//
MgGeometry* MgGeometry::ConvexHull()
{
    return MgGeosUtil::ConvexHull(this);
}

///////////////////////////////////////////////////////////////////////////
// Returns a geometry that represents a point set difference between this
// geometric entity and another.
//
MgGeometry* MgGeometry::Difference(MgGeometry* other)
{
    return MgGeosUtil::Difference(this, other);
}

///////////////////////////////////////////////////////////////////////////
// Returns the shortest distance between any two points of this geometry
// and another. By default distance computation is done using a linear
// algorithm unless a custom measure is supplied.
//
double MgGeometry::Distance(MgGeometry* other, MgMeasure* measure)
{
    bool started = false;
    double result = 0.0;

    // TODO: when Measure is supplied, utilize measure to find distance
    if (measure == NULL)
    {
        result = MgGeosUtil::Distance(this, other);
    }
    else
    {
        Ptr<MgCoordinateIterator> thisIter = this->GetCoordinates();
        while (thisIter->MoveNext())
        {
            Ptr<MgCoordinateIterator> otherIter = other->GetCoordinates();
            Ptr<MgCoordinate> thisCoord = thisIter->GetCurrent();

            while (otherIter->MoveNext())
            {
                Ptr<MgCoordinate> otherCoord = otherIter->GetCurrent();

                double distance = measure->GetDistance(thisCoord, otherCoord);
                if (distance < result)
                {
                    result = distance;
                }
                else if (!started)
                {
                    started = true;
                    result = distance;
                }
            }
        }
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////
// Returns a geometry that represents the point set intersection of this
// geometry and another.
//
MgGeometry* MgGeometry::Intersection(MgGeometry* other)
{
    return MgGeosUtil::Intersection(this, other);
}

///////////////////////////////////////////////////////////////////////////
// Returns a geometry that represents the point set symmetric difference
// of this geometry with another.
//
MgGeometry* MgGeometry::SymetricDifference(MgGeometry* other)
{
    return MgGeosUtil::SymetricDifference(this, other);
}

///////////////////////////////////////////////////////////////////////////
// Returns a geometry that represents the point set union of this geometry
// with another.
//
MgGeometry* MgGeometry::Union(MgGeometry* other)
{
    return MgGeosUtil::Union(this, other);
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgGeometry::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgGeometry::Dispose()
{
}

INT32 MgGeometry::GetEntityType()
{
    return MgGeometryEntityType::Geometry;
}
