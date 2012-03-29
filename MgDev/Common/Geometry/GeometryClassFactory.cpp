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

static bool InitializeStaticData();

static bool initStatic = InitializeStaticData();

#define EXCEPTION_CLASS_CREATOR(className) \
    fact->Register(Geometry_Exception_##className, className::CreateObject);

bool InitializeStaticData()
{
    MgClassFactory* fact = MgClassFactory::GetInstance();

    // Put in the map any class that can be serialized
    // Geometry API objects
    fact->Register(Geometry_Point, MgPoint::CreateObject);
    fact->Register(Geometry_Envelope, MgEnvelope::CreateObject);
    fact->Register(Geometry_CurvePolygon, MgCurvePolygon::CreateObject);
    fact->Register(Geometry_CurveString, MgCurveString::CreateObject);
    fact->Register(Geometry_MultiCurvePolygon, MgMultiCurvePolygon::CreateObject);
    fact->Register(Geometry_MultiCurveString, MgMultiCurveString::CreateObject);
    fact->Register(Geometry_MultiGeometry, MgMultiGeometry::CreateObject);
    fact->Register(Geometry_MultiLineString, MgMultiLineString::CreateObject);
    fact->Register(Geometry_MultiPoint, MgMultiPoint::CreateObject);
    fact->Register(Geometry_MultiPolygon, MgMultiPolygon::CreateObject);
    fact->Register(Geometry_Polygon, MgPolygon::CreateObject);
    fact->Register(Geometry_WktReaderWriter, MgWktReaderWriter::CreateObject);
    fact->Register(Geometry_LineString, MgLineString::CreateObject);
    fact->Register(Geometry_CoordinateXY, MgCoordinateXY::CreateObject);
    fact->Register(Geometry_CoordinateXYZ, MgCoordinateXYZ::CreateObject);
    fact->Register(Geometry_CoordinateXYZM, MgCoordinateXYZM::CreateObject);
    fact->Register(Geometry_CoordinateXYM, MgCoordinateXYM::CreateObject);

    EXCEPTION_CLASS_CREATOR(MgCoordinateSystemComputationFailedException)
    EXCEPTION_CLASS_CREATOR(MgCoordinateSystemConversionFailedException)
    EXCEPTION_CLASS_CREATOR(MgCoordinateSystemInitializationFailedException)
    EXCEPTION_CLASS_CREATOR(MgCoordinateSystemLoadFailedException)
    EXCEPTION_CLASS_CREATOR(MgCoordinateSystemMeasureFailedException)
    EXCEPTION_CLASS_CREATOR(MgCoordinateSystemTransformFailedException)
    EXCEPTION_CLASS_CREATOR(MgGeometryException)
    EXCEPTION_CLASS_CREATOR(MgInvalidCoordinateSystemException)
    EXCEPTION_CLASS_CREATOR(MgInvalidCoordinateSystemTypeException)
    EXCEPTION_CLASS_CREATOR(MgInvalidCoordinateSystemUnitsException)

    return true;
}

