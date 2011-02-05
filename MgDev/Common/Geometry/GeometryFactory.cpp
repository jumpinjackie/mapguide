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

static map<UINT32, MgGeometry*(*)()> createFuncs;

bool MgGeometryFactory::m_factoryInitialized = MgGeometryFactory::InitializeGeometryFactory();

///////////////////////////////////////////////////////////////////////////
// Creates an MgGeometryFactory object
//
MgGeometryFactory::MgGeometryFactory()
{
}

///////////////////////////////////////////////////////////////////////////
// Creates an MgCoordinate object with Dimension = XY, X = x, Y = y.
//
MgCoordinate* MgGeometryFactory::CreateCoordinateXY(double x, double y)
{
    return new MgCoordinateXY(x, y);
}

///////////////////////////////////////////////////////////////////////////
// Creates an MgCoordinate object with Dimension = XYM, X = x, Y = y,
/// M = m.
//
MgCoordinate* MgGeometryFactory::CreateCoordinateXYM(double x, double y, double m)
{
    return new MgCoordinateXYM(x, y, m);
}

///////////////////////////////////////////////////////////////////////////
// Creates an MgCoordinate object with Dimension = XYZ, X = x, Y = y,
// Z = z.
//
MgCoordinate* MgGeometryFactory::CreateCoordinateXYZ(double x, double y, double z)
{
    return new MgCoordinateXYZ(x, y, z);
}

///////////////////////////////////////////////////////////////////////////
// Creates an MgCoordinate object with Dimension = XYZM, X = x, Y = y,
// Z = z, M = m.
//
MgCoordinate* MgGeometryFactory::CreateCoordinateXYZM(double x, double y, double z, double m)
{
    return new MgCoordinateXYZM(x, y, z, m);
}

///////////////////////////////////////////////////////////////////////////
// Creates an arc segment from a start, end, and control coordinates.
//
MgArcSegment* MgGeometryFactory::CreateArcSegment(MgCoordinate* start, MgCoordinate* end, MgCoordinate* control)
{
    return new MgArcSegment(start, end, control);
}

///////////////////////////////////////////////////////////////////////////
// Creates a linear segment from a collection of coordinates.
//
MgLinearSegment* MgGeometryFactory::CreateLinearSegment(MgCoordinateCollection* coordinates)
{
    return new MgLinearSegment(coordinates);
}

///////////////////////////////////////////////////////////////////////////
// Creates a linear ring from a collection of coordinates.
//
MgLinearRing* MgGeometryFactory::CreateLinearRing(MgCoordinateCollection* coordinates)
{
    return new MgLinearRing(coordinates);
}

///////////////////////////////////////////////////////////////////////////
// Creates a curve ring from a collection of curve segments.
//
MgCurveRing* MgGeometryFactory::CreateCurveRing(MgCurveSegmentCollection* curveSegments)
{
    return new MgCurveRing(curveSegments);
}

///////////////////////////////////////////////////////////////////////////
// Creates a point from a coordinate.
//
MgPoint* MgGeometryFactory::CreatePoint(MgCoordinate* coordinate)
{
    return new MgPoint(coordinate);
}

///////////////////////////////////////////////////////////////////////////
// Creates a line string from a collection of coordinates.
//
MgLineString* MgGeometryFactory::CreateLineString(MgCoordinateCollection* coordinates)
{
    return new MgLineString(coordinates);
}

///////////////////////////////////////////////////////////////////////////
// Creates a curve string from a collection of curve segments.
//
MgCurveString* MgGeometryFactory::CreateCurveString(MgCurveSegmentCollection* curveSegments)
{
    return new MgCurveString(curveSegments);
}

///////////////////////////////////////////////////////////////////////////
// Creates a polygon from an outer ring and a collection of inner rings.
//
MgPolygon* MgGeometryFactory::CreatePolygon(MgLinearRing* outerRing, MgLinearRingCollection* innerRings)
{
    return new MgPolygon(outerRing, innerRings);
}

///////////////////////////////////////////////////////////////////////////
// Creates a curve polygon from an outer ring and a collection of inner
// rings.
//
MgCurvePolygon* MgGeometryFactory::CreateCurvePolygon(MgCurveRing* outerRing, MgCurveRingCollection* innerRings)
{
    return new MgCurvePolygon(outerRing, innerRings);
}

///////////////////////////////////////////////////////////////////////////
// Creates a multi point aggregrate geometry from a collection of points.
//
MgMultiPoint* MgGeometryFactory::CreateMultiPoint(MgPointCollection* points)
{
    return new MgMultiPoint(points);
}

///////////////////////////////////////////////////////////////////////////
// Creates a multi line string aggregrate geometry from a collection of
// line strings.
//
MgMultiLineString* MgGeometryFactory::CreateMultiLineString(MgLineStringCollection* lineStrings)
{
    return new MgMultiLineString(lineStrings);
}

///////////////////////////////////////////////////////////////////////////
// Creates a multi curve string aggregrate geometry from a collection of
// curve strings.
//
MgMultiCurveString* MgGeometryFactory::CreateMultiCurveString(MgCurveStringCollection* curveStrings)
{
    return new MgMultiCurveString(curveStrings);
}

///////////////////////////////////////////////////////////////////////////
// Creates a multi polygon aggregrate geometry from a collection of
// polygons.
//
MgMultiPolygon* MgGeometryFactory::CreateMultiPolygon(MgPolygonCollection* polygons)
{
    return new MgMultiPolygon(polygons);
}

///////////////////////////////////////////////////////////////////////////
// Creates a multi curve polygon aggregrate geometry from a collection of
// curve polygons.
//
MgMultiCurvePolygon* MgGeometryFactory::CreateMultiCurvePolygon(MgCurvePolygonCollection* polygons)
{
    return new MgMultiCurvePolygon(polygons);
}

///////////////////////////////////////////////////////////////////////////
// Creates an aggregrate geometry from a collection of geometry objects.
//
MgMultiGeometry* MgGeometryFactory::CreateMultiGeometry(MgGeometryCollection* geometries)
{
    return new MgMultiGeometry(geometries);
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgGeometryFactory::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgGeometryFactory::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////////////////////////
// Creates an empty geometry from its type identifier
//
MgGeometry* MgGeometryFactory::CreateGeometry(INT32 geomId)
{
    MgGeometry*(*createFunc)() = createFuncs[geomId];
    if(createFunc == NULL)
        return NULL;
    return createFunc();
}

//////////////////////////////////////////////////////////////////
// Initializes the factory
//
bool MgGeometryFactory::InitializeGeometryFactory()
{
    createFuncs[MgGeometryType::Point] = (MgGeometry*(*)())MgGeometryFactory::CreateEmptyPoint;
    createFuncs[MgGeometryType::LineString] = (MgGeometry*(*)())MgGeometryFactory::CreateEmptyLineString;
    createFuncs[MgGeometryType::CurveString] = (MgGeometry*(*)())MgGeometryFactory::CreateEmptyCurveString;
    createFuncs[MgGeometryType::Polygon] = (MgGeometry*(*)())MgGeometryFactory::CreateEmptyPolygon;
    createFuncs[MgGeometryType::CurvePolygon] = (MgGeometry*(*)())MgGeometryFactory::CreateEmptyCurvePolygon;
    createFuncs[MgGeometryType::MultiPoint] = (MgGeometry*(*)())MgGeometryFactory::CreateEmptyMultiPoint;
    createFuncs[MgGeometryType::MultiLineString] = (MgGeometry*(*)())MgGeometryFactory::CreateEmptyMultiLineString;
    createFuncs[MgGeometryType::MultiCurveString] = (MgGeometry*(*)())MgGeometryFactory::CreateEmptyMultiCurveString;
    createFuncs[MgGeometryType::MultiPolygon] = (MgGeometry*(*)())MgGeometryFactory::CreateEmptyMultiPolygon;
    createFuncs[MgGeometryType::MultiCurvePolygon] = (MgGeometry*(*)())MgGeometryFactory::CreateEmptyMultiCurvePolygon;
    createFuncs[MgGeometryType::MultiGeometry] = (MgGeometry*(*)())MgGeometryFactory::CreateEmptyMultiGeometry;
    return true;
}

///////////////////////////////////////////////////////////////////////////
// Creates an empty point
//
MgPoint* MgGeometryFactory::CreateEmptyPoint()
{
    return new MgPoint();
}

///////////////////////////////////////////////////////////////////////////
// Creates an empty line string
//
MgLineString* MgGeometryFactory::CreateEmptyLineString()
{
    return new MgLineString();
}

///////////////////////////////////////////////////////////////////////////
// Creates an empty curve string
//
MgCurveString* MgGeometryFactory::CreateEmptyCurveString()
{
    return new MgCurveString();
}

///////////////////////////////////////////////////////////////////////////
// Creates an empty polygon
//
MgPolygon* MgGeometryFactory::CreateEmptyPolygon()
{
    return new MgPolygon();
}

///////////////////////////////////////////////////////////////////////////
// Creates an empty curve polygon
//
MgCurvePolygon* MgGeometryFactory::CreateEmptyCurvePolygon()
{
    return new MgCurvePolygon();
}

///////////////////////////////////////////////////////////////////////////
// Creates an empty multi point aggregrate
//
MgMultiPoint* MgGeometryFactory::CreateEmptyMultiPoint()
{
    return new MgMultiPoint();
}

///////////////////////////////////////////////////////////////////////////
// Creates an empty multi line string aggregrate
//
MgMultiLineString* MgGeometryFactory::CreateEmptyMultiLineString()
{
    return new MgMultiLineString();
}

///////////////////////////////////////////////////////////////////////////
// Creates an empty multi curve string aggregrate
//
MgMultiCurveString* MgGeometryFactory::CreateEmptyMultiCurveString()
{
    return new MgMultiCurveString();
}

///////////////////////////////////////////////////////////////////////////
// Creates an empty multi polygon aggregrate
//
MgMultiPolygon* MgGeometryFactory::CreateEmptyMultiPolygon()
{
    return new MgMultiPolygon();
}

///////////////////////////////////////////////////////////////////////////
// Creates an empty multi curve polygon aggregrate
//
MgMultiCurvePolygon* MgGeometryFactory::CreateEmptyMultiCurvePolygon()
{
    return new MgMultiCurvePolygon();
}

///////////////////////////////////////////////////////////////////////////
// Creates an empty aggregrate
//
MgMultiGeometry* MgGeometryFactory::CreateEmptyMultiGeometry()
{
    return new MgMultiGeometry();
}
