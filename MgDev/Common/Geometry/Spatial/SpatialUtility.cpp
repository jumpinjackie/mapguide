//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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
#include "MathUtility.h"
#include "SpatialUtilityCircularArc.h"
#include "SpatialUtility.h"

#define EPSILON 1e-10
#define NEGEPSILON -EPSILON
#define SQREPSILON 1e-20

//relative fraction to be used for curve tesselation approximation
//This fraction is relative to the biggest dimension of the bounding box
//of the curve.
#define CURVE_APPROX_FACTOR 1e-3

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

MgGeometry * MgSpatialUtility::ApproximateGeometryWithLineStrings(
    MgGeometry * geometry,
    double maxSpacing,
    double maxOffset,
    MgGeometryFactory* geometryFactory)
{
    MgGeometry* newGeometry = NULL;

    if ( geometry == NULL )
    {
        throw new MgNullArgumentException(L"MgSpatialUtility.ApproximateGeometryWithLineStrings",
        __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if ( maxSpacing < 0.0 )
    {
        STRING buffer;
        MgUtil::DoubleToString(maxSpacing, buffer);

        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgSpatialUtility.ApproximateGeometryWithLineStrings",
            __LINE__, __WFILE__, &arguments, L"MgValueCannotBeLessThanZero", NULL);
    }

    if ( maxOffset < 0.0 )
    {
        STRING buffer;
        MgUtil::DoubleToString(maxOffset, buffer);

        MgStringCollection arguments;
        arguments.Add(L"3");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgSpatialUtility.ApproximateGeometryWithLineStrings",
            __LINE__, __WFILE__, &arguments, L"MgValueCannotBeLessThanZero", NULL);
    }

    INT32 geomType = geometry->GetGeometryType();

    switch (geomType)
    {
        case MgGeometryType::LineString:
        case MgGeometryType::Point:
        case MgGeometryType::Polygon:
        case MgGeometryType::MultiPoint:
        case MgGeometryType::MultiLineString:
        case MgGeometryType::MultiPolygon:
        {
            // No approximation needed.
            newGeometry = geometry;
            SAFE_ADDREF(geometry);
            break;
        }
        case MgGeometryType::CurveString:
        {
            MgCurveString* curve = static_cast<MgCurveString*>(geometry);
            Ptr<MgCurveSegmentCollection> csc = curve->GetCurveSegments();

            Ptr<MgCoordinateCollection> positions = new MgCoordinateCollection();
            AppendLinearizedCurveSegmentsToDistinctCollection(
                positions, csc, maxSpacing, maxOffset);

            newGeometry = geometryFactory->CreateLineString(positions);
            break;
        }
        case MgGeometryType::CurvePolygon:
        {
            MgCurvePolygon* geom = static_cast<MgCurvePolygon*>(geometry);
            Ptr<MgCurveRing> ring = geom->GetExteriorRing();
            Ptr<MgLinearRing> linearizedExteriorRing = CurveRingToLinearRing(ring,maxSpacing,maxOffset);
            Ptr<MgLinearRingCollection> linearizedInteriorRings = new MgLinearRingCollection();
            for (INT32 i = 0;  i < geom->GetInteriorRingCount();  i++)
            {
                ring = geom->GetInteriorRing(i);
                Ptr<MgLinearRing> linearizedInteriorRing = CurveRingToLinearRing(ring, maxSpacing, maxOffset);
                linearizedInteriorRings->Add(linearizedInteriorRing);
            }
            newGeometry = geometryFactory->CreatePolygon(linearizedExteriorRing, linearizedInteriorRings);
            break;
        }
        case MgGeometryType::MultiGeometry:
        {
            MgMultiGeometry* geom = static_cast<MgMultiGeometry*>(geometry);
            Ptr<MgGeometryCollection> linearizedGeometries = new MgGeometryCollection();

            for (INT32 i = 0;  i < geom->GetCount();  i++)
            {
                Ptr<MgGeometry> geomInside = geom->GetGeometry(i);
                Ptr<MgGeometry> linearizedGeomInside = ApproximateGeometryWithLineStrings(geomInside, maxSpacing, maxOffset, geometryFactory);
                linearizedGeometries->Add(linearizedGeomInside);
            }
            newGeometry = geometryFactory->CreateMultiGeometry(linearizedGeometries);
            break;
        }
        case MgGeometryType::MultiCurveString:
        {
            MgMultiCurveString* geom = static_cast<MgMultiCurveString*>(geometry);
            Ptr<MgLineStringCollection> linearizedGeometries = new MgLineStringCollection();
            for (INT32 i = 0;  i < geom->GetCount();  i++)
            {
                Ptr<MgCurveString> geomInside = geom->GetCurveString(i);
                Ptr<MgGeometry> linearizedGeomInside = ApproximateGeometryWithLineStrings(geomInside, maxSpacing, maxOffset, geometryFactory);
                MgLineString* lls = static_cast<MgLineString*> (linearizedGeomInside.p);
                linearizedGeometries->Add(lls);
            }
            newGeometry = geometryFactory->CreateMultiLineString(linearizedGeometries);
            break;
        }
        case MgGeometryType::MultiCurvePolygon:
        {
            MgMultiCurvePolygon* geom = static_cast<MgMultiCurvePolygon*>(geometry);
            Ptr<MgPolygonCollection> linearizedGeometries = new MgPolygonCollection();

            for (INT32 i = 0;  i < geom->GetCount();  i++)
            {
                Ptr<MgCurvePolygon> geomInside = geom->GetCurvePolygon(i);
                Ptr<MgGeometry> linearizedGeomInside = ApproximateGeometryWithLineStrings(geomInside, maxSpacing, maxOffset, geometryFactory);
                MgPolygon * lp = static_cast<MgPolygon*> (linearizedGeomInside.p);
                linearizedGeometries->Add(lp);
            }
            newGeometry = geometryFactory->CreateMultiPolygon(linearizedGeometries);
            break;
        }
        default:
        {
            STRING buffer;
            MgUtil::Int32ToString(geomType, buffer);

            MgStringCollection arguments;
            arguments.Add(L"1");
            arguments.Add(buffer);

            throw new MgInvalidArgumentException(L"MgSpatialUtility.ApproximateGeometryWithLineStrings",
                __LINE__, __WFILE__, &arguments, L"MgInvalidGeometryType", NULL);
        }
    }

    return newGeometry;
}

MgLinearRing* MgSpatialUtility::CurveRingToLinearRing(MgCurveRing* curveRing, double maxSpacing, double maxOffset)
{
    CHECKNULL(curveRing, L"MgSpatialUtility.CurveRingToLinearRing")

    MgGeometryFactory factory;

    Ptr<MgCurveSegmentCollection> csc = curveRing->GetCurveSegments();
    Ptr<MgCoordinateCollection> positions = new MgCoordinateCollection();
    AppendLinearizedCurveSegmentsToDistinctCollection(positions, csc, maxSpacing, maxOffset);

    return factory.CreateLinearRing(positions);
}

MgLinearSegment* MgSpatialUtility::ArcSegmentToLinearSegment(MgArcSegment* segment, double maxSpacing, double maxOffset)
{
    CHECKNULL(segment, L"MgSpatialUtility.ArcSegmentToLinearSegment")

    MgGeometryFactory factory;

    Ptr<MgCoordinate> start = segment->GetStartCoordinate();
    Ptr<MgCoordinate> mid = segment->GetControlCoordinate();
    Ptr<MgCoordinate> end = segment->GetEndCoordinate();

    Ptr<MgCoordinateCollection> positions = new MgCoordinateCollection();

    MgSpatialUtilityCircularArc utilArc(start, mid, end);
    utilArc.Linearize(positions, maxSpacing, maxOffset);

    return factory.CreateLinearSegment(positions);
}

void MgSpatialUtility::AppendLinearizedCurveSegmentsToDistinctCollection(
    MgCoordinateCollection* distinctPositions,
    MgCurveSegmentCollection* curveSegments,
    double maxSpacing, double maxOffset)
{
    for (INT32 i = 0;  i < curveSegments->GetCount();  i++)
    {
        Ptr<MgCurveSegment> cs = curveSegments->GetItem(i);
        INT32 gct = cs->GetComponentType();

        switch (gct)
        {
            case MgGeometryComponentType::LinearSegment:
            {
                MgLinearSegment* ls = static_cast<MgLinearSegment*>((MgCurveSegment*)cs);
                Ptr<MgCoordinateCollection> segmentPositions = ls->GetCoordinateCollection();
                AppendPositionsToDistinctCollection(distinctPositions, segmentPositions);
                break;
            }
            case MgGeometryComponentType::ArcSegment:
            {
                MgArcSegment* arc = static_cast<MgArcSegment*>((MgCurveSegment*)cs);
                Ptr<MgCoordinate> start = arc->GetStartCoordinate();
                Ptr<MgCoordinate> mid = arc->GetControlCoordinate();
                Ptr<MgCoordinate> end = arc->GetEndCoordinate();
                MgSpatialUtilityCircularArc utilArc(start, mid, end);
                utilArc.Linearize(distinctPositions, maxSpacing, maxOffset);
                break;
            }
            default:
            {
                STRING buffer;
                MgUtil::Int32ToString(gct, buffer);

                MgStringCollection arguments;
                arguments.Add(L"2");
                arguments.Add(buffer);

                throw new MgInvalidArgumentException(L"MgSpatialUtility.AppendLinearizedCurveSegmentsToDistinctCollection",
                    __LINE__, __WFILE__, &arguments, L"MgInvalidGeometryComponentType", NULL);
            }
        }
    }
}

void MgSpatialUtility::AppendPositionToDistinctCollection(
    MgCoordinateCollection* distinctPositions,
    MgCoordinate* positionToAppend)
{
    bool isDistinct = true;
    INT32 count = distinctPositions->GetCount();

    if (count > 0)
    {
        Ptr<MgCoordinate> pLastPos = distinctPositions->GetItem(count-1);
        if (ArePositionsEqualXYZ(pLastPos, positionToAppend))
            isDistinct = false;
    }

    if (isDistinct)
        distinctPositions->Add(positionToAppend);
}

void MgSpatialUtility::AppendPositionsToDistinctCollection(
    MgCoordinateCollection* distinctPositions,
    MgCoordinateCollection* positionsToAppend)
{
    // Slight performance might be gained here by assuming that source points
    // are themselves distinct.
    for (INT32 i = 0;  i < positionsToAppend->GetCount();  i++)
    {
        Ptr<MgCoordinate> pos = positionsToAppend->GetItem(i);
        AppendPositionToDistinctCollection(distinctPositions, pos);
    }
}

bool MgSpatialUtility::ArePositionsEqualXYZ(
    MgCoordinate * p1,
    MgCoordinate * p2)
{
    bool areEqual =
        MgMathUtility::AreEqualAndMaybeNan(p1->GetX(), p2->GetX()) &&
        MgMathUtility::AreEqualAndMaybeNan(p1->GetY(), p2->GetY()) &&
        MgMathUtility::AreEqualAndMaybeNan(p1->GetZ(), p2->GetZ());
    return areEqual;
}

bool MgSpatialUtility::ArePositionsEqualXY(
    MgCoordinate * p1,
    MgCoordinate * p2)
{
    bool areEqual =
        MgMathUtility::AreEqualAndMaybeNan(p1->GetX(), p2->GetX()) &&
        MgMathUtility::AreEqualAndMaybeNan(p1->GetY(), p2->GetY());
    return areEqual;
}

double MgSpatialUtility::DistanceBetweenPositionsXYZ(
    MgCoordinate * p1,
    MgCoordinate * p2)
{
    double x1 = p1->GetX();
    double y1 = p1->GetY();
    double z1 = p1->GetZ();
    double x2 = p2->GetX();
    double y2 = p2->GetY();
    double z2 = p2->GetZ();

    double dx = x2 - x1;
    double dy = y2 - y1;
    bool zIsNan = ( MgMathUtility::IsNan(z1) || MgMathUtility::IsNan(z2) );
    double dz = ( zIsNan ? 0.0 : z2 - z1 );

    return sqrt(dx*dx + dy*dy + dz*dz);
}


double MgSpatialUtility::DistanceBetweenPositionsXY(
    MgCoordinate * p1,
    MgCoordinate * p2)
{
    double x1 = p1->GetX();
    double y1 = p1->GetY();
    double x2 = p2->GetX();
    double y2 = p2->GetY();

    double dx = x2 - x1;
    double dy = y2 - y1;

    return sqrt(dx*dx + dy*dy);
}

MgCoordinate* MgSpatialUtility::UnitNormalVector(MgCoordinate* p1, MgCoordinate* p2, MgCoordinate* p3)
{
    // Take the cross product = (p2-p1) X (p3-p1);
    MgGeometryFactory factory;

    bool hasZ = p1->GetDimension() & p2->GetDimension() & p3->GetDimension() & MgCoordinateDimension::XYZ;

    double v21[3], v31[3];

    v21[0] =  p2->GetX() - p1->GetX();
    v21[1] =  p2->GetY() - p1->GetY();
    v21[2] =  hasZ ? ( p2->GetZ() - p1->GetZ() ) : 0.0;

    v31[0] =  p3->GetX() - p1->GetX();
    v31[1] =  p3->GetY() - p1->GetY();
    v31[2] =  hasZ ? ( p3->GetZ() - p1->GetZ() ) : 0.0;

    double normalX = MgMathUtility::SnapToZero(v21[1] * v31[2] - v31[1] * v21[2]);
    double normalY = MgMathUtility::SnapToZero(v21[2] * v31[0] - v31[2] * v21[0]);
    double normalZ = MgMathUtility::SnapToZero(v21[0] * v31[1] - v31[0] * v21[1]);

    // Normalize

    double a;
    a = (normalX*normalX) + (normalY*normalY) + (normalZ*normalZ);
    a = MgMathUtility::SnapToZero(a);
    if (0.0 != a)
        a = 1 / sqrt ( a );

    return factory.CreateCoordinateXYZ(a * normalX, a * normalY, a * normalZ);
}

// This implementation is a collapsed version of logic taken from Autodesk's AcGe package:
//     - AcGeImpEntity3d::rotateBy()
//     - AcGeMatrix3d::setToRotation()
//     - AcGeMatrix3d::preMultBy()
//     - AcGeQuaternion::setToAngleAndAxis()
//     - AcGeQuaternion::getMatrix()
//     - AcGePoint3d::transformBy()
MgCoordinate* MgSpatialUtility::RotatePosition(
    MgCoordinate * position,
    double angle,
    MgCoordinate * axisVector,
    MgCoordinate * center,
    bool includeM,
    double mm)
{
    MgGeometryFactory factory;

    // Translate the rotation center to the origin.
    double tpx = position->GetX() - center->GetX(); // Translated Position's X ordinate
    double tpy = position->GetY() - center->GetY();
    double tpz = position->GetZ() - center->GetZ();

    // Create the Quaternion, with w, x, y, z dimensions;
    double halfAngle = angle / 2.0;
    double qw = cos(halfAngle);
    double qx = sin(halfAngle) * axisVector->GetX();
    double qy = sin(halfAngle) * axisVector->GetY();
    double qz = sin(halfAngle) * axisVector->GetZ();

    // Create a 3D transformation matrix from the Quaternion.
    // In a full implementation of a transformation matrix, its size would
    // be 4x4.  However, with no scaling needed, we just fill out a 3x3 size.

    double twoX = qx + qx;
    double twoY = qy + qy;
    double twoZ = qz + qz;

    double twoWX = qw * twoX;
    double twoXX = qx * twoX;

    double twoWY = qw * twoY;
    double twoXY = qx * twoY;
    double twoYY = qy * twoY;

    double twoWZ = qw * twoZ;
    double twoXZ = qx * twoZ;
    double twoYZ = qy * twoZ;
    double twoZZ = qz * twoZ;

    double m[3][3]; // Transformation matrix.
    m[0][0] = 1.0 - (twoYY + twoZZ);
    m[0][1] = twoXY - twoWZ;
    m[0][2] = twoXZ + twoWY;
    m[1][0] = twoXY + twoWZ;
    m[1][1] = 1.0 - (twoXX + twoZZ);
    m[1][2] = twoYZ - twoWX;
    m[2][0] = twoXZ - twoWY;
    m[2][1] = twoYZ + twoWX;
    m[2][2] = 1.0 - (twoXX + twoYY);

    // Rotate about the origin..
    double trpx = m[0][0] * tpx     // Translated, Rotated position's X ordinate.
                + m[0][1] * tpy
                + m[0][2] * tpz;

    double trpy = m[1][0] * tpx
                + m[1][1] * tpy
                + m[1][2] * tpz;

    double trpz = m[2][0] * tpx
                + m[2][1] * tpy
                + m[2][2] * tpz;

    // Translate back to respect the translation center.
    double trtpx = trpx + center->GetX();   // Translated, Rotated, Translated position's X ordinate
    double trtpy = trpy + center->GetY();
    double trtpz = trpz + center->GetZ();

    MgCoordinate* trtp = NULL;

    if (includeM)
        trtp = factory.CreateCoordinateXYZM(trtpx, trtpy, trtpz, mm);
    else
        trtp = factory.CreateCoordinateXYZ(trtpx, trtpy, trtpz);

    CHECKNULL(trtp, L"MgSpatialUtility.RotatePosition")

    return trtp;
}


///////////////////////////////////////////////////////////////////////////////
MgGeometry* MgSpatialUtility::TesselateCurve(MgGeometry* curve)
{
    Ptr<MgEnvelope> env = curve->Envelope();
    double maxdim = max(env->GetWidth(), env->GetHeight());

    Ptr<MgGeometryFactory> factory = new MgGeometryFactory();
    return MgSpatialUtility::ApproximateGeometryWithLineStrings(curve, maxdim, maxdim * CURVE_APPROX_FACTOR, factory);
}

///////////////////////////////////////////////////////////////////////////////
MgGeometryComponent* MgSpatialUtility::TesselateGeometryComponent(MgGeometryComponent* curve)
{
    CHECKNULL(curve, L"MgSpatialUtility.TesselateCurve")

    Ptr<MgGeometryComponent> retGeomComp;
    Ptr<MgEnvelope> env = curve->Envelope();
    double maxdim = max(env->GetWidth(), env->GetHeight());

    INT32 comp = curve->GetComponentType();
    switch(comp)
    {
        case MgGeometryComponentType::ArcSegment:
        {
            MgArcSegment* arcSeg = static_cast<MgArcSegment*>(curve);
            retGeomComp = ArcSegmentToLinearSegment(arcSeg, maxdim, maxdim * CURVE_APPROX_FACTOR);
            break;
        }
        case MgGeometryComponentType::CurveRing:
        {
            MgCurveRing* curveRing = static_cast<MgCurveRing*>(curve);
            retGeomComp = CurveRingToLinearRing(curveRing, maxdim, maxdim * CURVE_APPROX_FACTOR);
            break;
        }
        case MgGeometryComponentType::LinearSegment:
        {
            retGeomComp = SAFE_ADDREF(curve);
            break;
        }
        case MgGeometryComponentType::LinearRing:
        {
            retGeomComp = SAFE_ADDREF(curve);
            break;
        }
    }

    return retGeomComp.Detach();
}
