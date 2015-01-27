
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
#include "MathUtility.h"
#include "SpatialUtilityCircularArc.h"
#include "SpatialUtility.h"
#include "SpatialUtilityStatus.h"

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

    CHECKARGUMENTNULL(geometry, L"MgSpatialUtility.ApproximateGeometryWithLineStrings");

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
    CHECKARGUMENTNULL(curveRing, L"MgSpatialUtility.CurveRingToLinearRing")

    MgGeometryFactory factory;

    Ptr<MgCurveSegmentCollection> csc = curveRing->GetCurveSegments();
    Ptr<MgCoordinateCollection> positions = new MgCoordinateCollection();
    AppendLinearizedCurveSegmentsToDistinctCollection(positions, csc, maxSpacing, maxOffset);

    return factory.CreateLinearRing(positions);
}

MgLinearSegment* MgSpatialUtility::ArcSegmentToLinearSegment(MgArcSegment* segment, double maxSpacing, double maxOffset)
{
    CHECKARGUMENTNULL(segment, L"MgSpatialUtility.ArcSegmentToLinearSegment")

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
    CHECKARGUMENTNULL(curve, L"MgSpatialUtility.TesselateCurve")

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

//=============================================================================
// Determines the largest coordinate (2D) magnitude in a coordinate collection.
double MgSpatialUtility::GreatestMagnitude (MgCoordinateIterator* coordinateIterator)
{
    double magnitude (0.0);
    Ptr<MgCoordinate> tmpCoord;

    // We determine the largest magnitude of any coordinate of the polygon boundary.
    coordinateIterator->Reset ();
    while (coordinateIterator->MoveNext ())
    {
        tmpCoord = coordinateIterator->GetCurrent ();
        if (fabs (tmpCoord->GetX()) > magnitude) magnitude = fabs (tmpCoord->GetX ());
        if (fabs (tmpCoord->GetY()) > magnitude) magnitude = fabs (tmpCoord->GetY ());
    }
    return magnitude;
}

//=============================================================================
// Calculates the two dimensional intersection of two line segments.
//
INT32 MgSpatialUtility::SegmentIntersection (MgCoordinate* result,MgCoordinate* seg1From,
                                                                  MgCoordinate* seg1To,
                                                                  MgCoordinate* seg2From,
                                                                  MgCoordinate* seg2To,
                                                                  double magnitude)
{
    bool preciseFrom;
    bool preciseTo;
    int exponent;           // exponent (base two) of the magnitude
    INT32 status;

    double delX1, delY1;
    double delX2, delY2;
    double denom1, denom2;
    double num, denom;          // numerator and denominator
    double tolerance;

    CHECKARGUMENTNULL(result, L"MgSpatialUtility.SegmentIntersection")
    CHECKARGUMENTNULL(seg1From, L"MgSpatialUtility.SegmentIntersection")
    CHECKARGUMENTNULL(seg1To, L"MgSpatialUtility.SegmentIntersection")
    CHECKARGUMENTNULL(seg2From, L"MgSpatialUtility.SegmentIntersection")
    CHECKARGUMENTNULL(seg2To, L"MgSpatialUtility.SegmentIntersection")
    if (magnitude < 1.0E-04)
    {
        throw new MgInvalidArgumentException(L"MgSpatialUtility.SegmentIntersection", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // Compute a tolerance which is appropriate for the data we are working
    // with.  The intent is to derive a value which will ignore differences
    // in the last 6 bits of the mantissa of a real number.  In standard
    // IEEE form, the mantissa of a "double" is 52 bits.  If magnitude is
    /// 1.0, this should produce a tolerance value of about ~1.5E-14.
    frexp (magnitude,&exponent);            // ignore the mantissa
    tolerance = ldexp (0.5,(exponent - 46));

    // Until we know different.
    status = 0;

    // Compute the denominator which also tells us if the lines are collinear.
    delX1 = seg1To->GetX () - seg1From->GetX ();
    delY1 = seg1To->GetY () - seg1From->GetY ();
    delX2 = seg2To->GetX () - seg2From->GetX ();
    delY2 = seg2To->GetY () - seg2From->GetY ();
    if (fabs (delX1) < tolerance && fabs (delY1) < tolerance)
    {
        status |= MgSpatialUtilityStatus::DegenerateSeg1;
    }
    if (fabs (delX2) < tolerance && fabs (delY2) < tolerance)
    {
        status |= MgSpatialUtilityStatus::DegenerateSeg2;
    }
    if (status != 0)
    {
        return status;
    }

    denom1 = delX2 * delY1;
    denom2 = delX1 * delY2;
    if (MgMathUtility::DblCmp (denom1,denom2))
    {
        double intercept1, intercept2;

        // Here if the segments are parallel, possibly colinear.  In either
        // case, there is no intersection.

        // Determine collinear status.  We determine collinearity by calculating
        // the axis intercept value of both segments.  We choose the axis to use
        // based on the relative values of deltaX and deltaY.  Since the two
        // lines are parallel, the delta values of the two lines are proportionate.
        // Necessary to preclude a divide by zero, but also increases the precision
        // of the determination.
        if (fabs (delX1) > fabs (delY1))
        {
            // We'll use the Y axis intercept for this determination.
            intercept1 = (delX1 * (seg1From->GetX () + seg1To->GetX ())) /
                         (delY1 * (seg1From->GetY () + seg1To->GetY ())) * 0.5;
            intercept2 = (delX2 * (seg2From->GetX () + seg2To->GetX ())) /
                         (delY2 * (seg2From->GetY () + seg2To->GetY ())) * 0.5;
        }
        else
        {
            // We'll use the X axis intercept for this determination.
            intercept1 = (delY1 * (seg1From->GetY () + seg1To->GetY ())) /
                         (delX1 * (seg1From->GetX () + seg1To->GetX ())) * 0.5;
            intercept2 = (delY2 * (seg2From->GetY () + seg2To->GetY ())) /
                         (delX2 * (seg2From->GetX () + seg2To->GetX ())) * 0.5;
        }
        bool collinear = MgMathUtility::DblCmp (intercept1,intercept2,tolerance);
        if (collinear)
        {
            status |= MgSpatialUtilityStatus::SegmentsCollinear;

            // Now we determine if the nature of the collinearity.  Could
            // be tuned for performance, but this case is very rare.  Better
            // top have something that is simple, understandable, and works.
            double x1From = seg1From->GetX ();
            double y1From = seg1From->GetY ();
            double x1To   = seg1To->GetX ();
            double y1To   = seg1To->GetX ();
            double x2From = seg2From->GetX ();
            double y2From = seg2From->GetY ();
            double x2To   = seg2To->GetX ();
            double y2To   = seg2To->GetX ();

            // Determine if there is any overlap.  A three bit combination should
            // not be possible. So there are are 12 various combinations which
            // represent valid situations.  For example, when all four bits are
            // set, the two segments are identical.
            if (fabs (delX1) > fabs (delY1))
            {
                // We'll use the X coordinates to determine overlap.
                if (x1From >= (x2From - tolerance) && x1From <= (x2From + tolerance))
                {
                    // The overlap starts at the first point of the first segment.
                    status |= MgSpatialUtilityStatus::CollinearSeg1Start;
                }
                if (x1To >= (x2From - tolerance) && x1To <= (x2From + tolerance))
                {
                    // The overlap ends at the first point of the first segment.
                    status |= MgSpatialUtilityStatus::CollinearSeg1End;
                }
                if (x2From >= (x1From - tolerance) && x2From <= (x1From + tolerance))
                {
                    // The overlap starts at the first point of the second segment.
                    status |= MgSpatialUtilityStatus::CollinearSeg2Start;
                }
                if (x2To >= (x1From - tolerance) && x2To <= (x1From + tolerance))
                {
                    // The overlap endss at the first point of the second segment.
                    status |= MgSpatialUtilityStatus::CollinearSeg2End;
                }
            }
            else
            {
                // We'll use the Y coordinates to determine overlap.
                if (y1From >= (y2From - tolerance) && y1From <= (y2From + tolerance))
                {
                    status |= MgSpatialUtilityStatus::CollinearSeg1Start;
                }
                if (y1To >= (y2From - tolerance) && y1To <= (y2From + tolerance))
                {
                    status |= MgSpatialUtilityStatus::CollinearSeg1End;
                }
                if (y2From >= (y1From - tolerance) && y2From <= (y1From + tolerance))
                {
                    status |= MgSpatialUtilityStatus::CollinearSeg2Start;
                }
                if (y2To >= (y1From - tolerance) && y2To <= (y1From + tolerance))
                {
                    status |= MgSpatialUtilityStatus::CollinearSeg2End;
                }
            }
        }
        else
        {
            status |= MgSpatialUtilityStatus::SegmentsParallel;
        }
    }
    else
    {
        // OK, we have a stable geometry and an intersection of some sort should exist,
        // somewhere.  Compute the intersection point.
        denom = denom1 - denom2;
        num = delX1 * delX2 * (seg2From->GetY() - seg1From->GetY()) +
              delX2 * delY1 *  seg1From->GetX() -
              delX1 * delY2 *  seg2From->GetX();
        result->SetX (num / denom);

        num = delY1 * delY2 * (seg2From->GetX() - seg1From->GetX()) +
              delY2 * delX1 *  seg1From->GetY() -
              delY1 * delX2 *  seg2From->GetY();
        result->SetY (num / -denom);

        // Determine the location of the intersection point relative to the
        // line segments involved.
        if (fabs (delX1) > fabs (delY1))
        {
            // Segment one is more horizontal than vertical. We will use the X
            // value to test if the resulting point is on the segment.
            preciseFrom = MgMathUtility::DblCmp (result->GetX(),seg1From->GetX(),tolerance);
            preciseTo = MgMathUtility::DblCmp (result->GetX(),seg1To->GetX(),tolerance);
            if (delX1 >= 0.0)
            {
                // X increases from the 'from' point to the 'to' point.
                if ((result->GetX() >= seg1From->GetX() || preciseFrom) &&
                    (result->GetX() <= seg1To->GetX()   || preciseTo))
                {
                    // Intersection is on segment one.
                    status |= MgSpatialUtilityStatus::IntersectOnSeg1;
                }
            }
            else
            {
                // X decreases from the 'from' point to the 'to' point.
                if ((result->GetX() <= seg1From->GetX() || preciseFrom) &&
                    (result->GetX() >= seg1To->GetX()   || preciseTo))
                {
                    // Intersection is on segment one.
                    status |= MgSpatialUtilityStatus::IntersectOnSeg1;
                }
            }
            if (preciseFrom)
            {
                // Intersection point is the start point of the segment.
                status |= MgSpatialUtilityStatus::IntersectIsSeg1Start;
            }
            if (preciseTo)
            {
                // Intersection point is the end point of the segment.
                status |= MgSpatialUtilityStatus::IntersectIsSeg1End;
            }
        }
        else
        {
            // First segment is more vertical than horizontal. We will use the Y
            // value to test if the resulting point is on the segment.
            preciseFrom = MgMathUtility::DblCmp (result->GetY(),seg1From->GetY(),tolerance);
            preciseTo = MgMathUtility::DblCmp (result->GetY(),seg1To->GetY(),tolerance);
            if (delY1 >= 0.0)
            {
                // Y increases from the 'from' point to the 'to' point.
                if ((result->GetY() >= seg1From->GetY() || preciseFrom) &&
                    (result->GetY() <= seg1To->GetY()   || preciseTo))
                {
                    // Intersection is on segment one.
                    status |= MgSpatialUtilityStatus::IntersectOnSeg1;
                }
            }
            else
            {
                // Y increases from the 'from' point to the 'to' point.
                if ((result->GetY() <= seg1From->GetY() || preciseFrom) &&
                    (result->GetY() >= seg1To->GetY()   || preciseTo))
                {
                    // Intersection is on segment one.
                    status |= MgSpatialUtilityStatus::IntersectOnSeg1;
                }
            }
            if (preciseFrom)
            {
                // Intersection point is the start point of the segment.
                status |= MgSpatialUtilityStatus::IntersectIsSeg1Start;
            }
            if (preciseTo)
            {
                // Intersection point is the end point of the segment.
                status |= MgSpatialUtilityStatus::IntersectIsSeg1End;
            }
        }

        // Same stuff with the second segment, sans comments.
        if (fabs (delX2) > fabs (delY2))
        {
            preciseFrom = MgMathUtility::DblCmp (result->GetX(),seg2From->GetX(),tolerance);
            preciseTo = MgMathUtility::DblCmp (result->GetX(),seg2To->GetX(),tolerance);
            if (delX2 >= 0.0)
            {
                if ((result->GetX() >= seg2From->GetX() || preciseFrom) &&
                    (result->GetX() <= seg2To->GetX()   || preciseTo))
                {
                    status |= MgSpatialUtilityStatus::IntersectOnSeg2;
                }
            }
            else
            {
                if ((result->GetX() <= seg2From->GetX() || preciseFrom) &&
                    (result->GetX() >= seg2To->GetX()   || preciseTo))
                {
                    status |= MgSpatialUtilityStatus::IntersectOnSeg2;
                }
            }
            if (preciseFrom)
            {
                status |= MgSpatialUtilityStatus::IntersectIsSeg2Start;
            }
            if (preciseTo)
            {
                status |= MgSpatialUtilityStatus::IntersectIsSeg2End;
            }
        }
        else
        {
            preciseFrom = MgMathUtility::DblCmp (result->GetY(),seg2From->GetY(),tolerance);
            preciseTo = MgMathUtility::DblCmp (result->GetY(),seg2To->GetY(),tolerance);
            if (delY2 >= 0.0)
            {
                if ((result->GetY() >= seg2From->GetY() || preciseFrom) &&
                    (result->GetY() <= seg2To->GetY()   || preciseTo))
                {
                    status |= MgSpatialUtilityStatus::IntersectOnSeg2;
                }
            }
            else
            {
                if ((result->GetY() <= seg2From->GetY() || preciseFrom) &&
                    (result->GetY() >= seg2To->GetY()   || preciseTo))
                {
                    status |= MgSpatialUtilityStatus::IntersectOnSeg2;
                }
            }
            if (preciseFrom)
            {
                status |= MgSpatialUtilityStatus::IntersectIsSeg2Start;
            }
            if (preciseTo)
            {
                status |= MgSpatialUtilityStatus::IntersectIsSeg2End;
            }
        }
    }

    return status;
}

//=============================================================================
// Returns a list of coordinates with all of the intersection points of the
// provided segment with the polygon provided by the first argument.  Note
// that the first argument is an MgCoordinateIterator; so it doesn't have to
// be an MgPolygon.
//
// Note that the returned collection is "new", and that it is a collection
// of MgCoordinateXYM objects.  For each intersection discovered and returned,
// the M value of the coordinates in the collection will contain (as a double)
// the status bit map returned by the SegmentIntersection method.   (Note,
// that the integer status value will have 0.01 added to it before being stored
// as a double to eliminate the possibility of a round off error upon
// conversion back to an INT32.)
//
// In order to preclude duplicate intersection points in the returned
// result, we always skip an intersection point which is precisely (see
// MgMathUtility::DblCmp) equal to the 'To' point on a polgon segment,
// confident that we will pick this intersection up on the next iteration
// when such point becomes the "From" point.
//
// TODO: Currently, this method will loop through the provided polygon
// to determine an appropriate value for "magnitude" which is used in
// performing "fuzzy" comparison of 'double's.  An overloaded function
// which accepts a "magnitude" value which can be computed only once
// for a specific polygon would improve performace in many cases.
MgCoordinateCollection* MgSpatialUtility::PolySegIntersection (MgCoordinateIterator* polyItr,
                                                               MgCoordinate* segFrom,
                                                               MgCoordinate* segTo)
{
    int status;
    INT32 statusMask;
    INT32 insertIndex;                      // for test and debug;

    double rStatus;
    double magnitude;

    Ptr<MgCoordinateCollection> intersectionCollection;
    Ptr<MgCoordinate> intersection;
    Ptr<MgCoordinate> polyFrom;
    Ptr<MgCoordinate> polyTo;

    CHECKARGUMENTNULL(polyItr, L"MgSpatialUtility.PolySegIntersection")
    CHECKARGUMENTNULL(segFrom, L"MgSpatialUtility.PolySegIntersection")
    CHECKARGUMENTNULL(segTo, L"MgSpatialUtility.PolySegIntersection")

    // We need this so we can determine a valid value to use for
    // "fuzzy" double comparisons.
    magnitude = GreatestMagnitude (polyItr);

    // Create the return object.  Quite often it gets returned empty, but we
    // will always return a "new" object (i.e. refCount == 1).
    intersectionCollection = new MgCoordinateCollection ();

    // Create a work point we can use fo the result of an intersection.
    intersection = new MgCoordinateXY ();

    // Reset the iterator so we know for sure what its state is.
    polyItr->Reset ();
    polyItr->MoveNext ();

    // Loop through each segment in the polygon.
    polyTo = polyItr->GetCurrent ();
    while (polyItr->MoveNext ())
    {
        polyFrom = polyTo;
        polyTo = polyItr->GetCurrent ();
        status = SegmentIntersection (intersection,polyFrom,polyTo,segFrom,segTo,magnitude);

        // Filter out that which we do nothing with.
        if ((status & MgSpatialUtilityStatus::SegmentStatusMask) != 0)
        {
            // There is no intersection to deal with.
            continue;
        }

        // An intersection point exists.  If the intersection point is precisely
        // that of the end point of the boundary segment, we skip it.  We'll catch
        // it when this point become the start point of the next boundary segment.
        // Thus, we don't end up with duplicate points in the intersection
        // collection.  This selection of skipping the end point means that if
        // the collection we've been given is not closed, this should all still
        // work (famous last words).
        statusMask = MgSpatialUtilityStatus::IntersectOnSeg1 | MgSpatialUtilityStatus::IntersectIsSeg1End;
        if ((status & statusMask) == statusMask)
        {
            continue;
        }

        // OK, if we have an intersection point on both segments, we have something of
        // interest to us.
        statusMask = MgSpatialUtilityStatus::IntersectOnSeg1 |
                     MgSpatialUtilityStatus::IntersectOnSeg2;
        if ((status & statusMask) == statusMask)
        {
            // We have an intersection of interest to us.  That is, an intersection
            // point which resides on both lines, but not precisely on the 'To'
            // point of the polygon segment.  We use a MgCoordinateM here so we
            // can pass back to the calling module the intersection status.
            rStatus = static_cast<double>(status) + 0.01;
            Ptr<MgCoordinate> newPoint = new MgCoordinateXYM (intersection->GetX(),intersection->GetY(),rStatus);
            insertIndex = AddToCoordinateCollection (intersectionCollection,newPoint,segFrom);
            if (insertIndex != 0)
            {
                insertIndex = 0;
            }
            // The line segment can have more than one intersection point with
            // the polygon.  So we keep trucking.
        }
    }

    // We return the generated collection, which will often be empty.
    // We never return a null pointer.
    return intersectionCollection.Detach ();
}

///////////////////////////////////////////////////////////////////////////////
// In the event that the PolySegIntersection function discovers two or more
// intersection points for a single line segment, it is important that they
// get added to the coordinate collection in the proper order.  This is a
// brute force technique, but this situation is rather rare.
INT32 MgSpatialUtility::AddToCoordinateCollection (MgCoordinateCollection* collection,
                                                   MgCoordinate* newPoint,
                                                   MgCoordinate* basePoint)
{
    INT32 index = 0;
    double deltaX, deltaY;
    double newDistance, indexDistance;

    INT32 segCount = collection->GetCount ();
    if (segCount == 0)
    {
        // This case happens so often that its worth having extra code to
        // deal with this case very efficiently.
        collection->Add (newPoint);
    }
    else
    {
        // This happens rather infrequently, but it can happen.  Need to
        // insert the value into the collection such that when we are
        // done, the points represent the same disrection as the original
        // line segment.

        // Compute the distance from the basePoint to the newPoint.  To
        // keep performance up we'll use the square of the distance
        // rather than the distance itself;
        deltaX = newPoint->GetX () - basePoint->GetX ();
        deltaY = newPoint->GetY () - basePoint->GetY ();
        newDistance = /*sqrt*/ (deltaX * deltaX + deltaY * deltaY);

        // Loop through all points in the collection and determine the index
        // value of the first point whose distance from the base point is
        // greater than the distance for the newPoint.
        for (index = 0;index < segCount;index += 1)
        {
            Ptr<MgCoordinate> indexPoint = collection->GetItem (index);
            deltaX = indexPoint->GetX () - basePoint->GetX ();
            deltaY = indexPoint->GetY () - basePoint->GetY ();
            indexDistance = /*sqrt*/ (deltaX * deltaX + deltaY * deltaY);
            if (indexDistance > newDistance)
            {
                break;
            }
        }
        collection->Insert (index,newPoint);
    }
    return index;
}

///////////////////////////////////////////////////////////////////////////////
// This function returns a point which is guaranteed to be outside the polygon
// provided by the argument.  Having a separate function here enables users
// of PointIsInPolygon to determine a point outside the polygon once per
// polygon, rather than each time PointIsInPolygon is called.
MgCoordinate* MgSpatialUtility::PointOutsidePolygon (MgCoordinateIterator* polyItr)
{
    Ptr<MgCoordinate> outPoint;

    double outX, outY;
    double magnitude;

    // Obtain the largest magnitude of any coordinate in the polygon boundary.
    magnitude = GreatestMagnitude (polyItr);

    // Create a from point known to be outside of the polygon from the
    // magnitude.  The choice  here is rather arbitrary.
    outX = -magnitude * 2.0;
    outY = -magnitude * 2.0;
    outPoint = new MgCoordinateXY (outX,outY);

    return outPoint.Detach ();
}

///////////////////////////////////////////////////////////////////////////////
// Use this overload of the PointIsInPolygon function (in conjuction with the
// PointOutsidePolygon function) when several points are to be tested for the
// same polygon.  That is, arrange your code so the outsidePoint argument is
// only calculated once.  Return value indicates if the the queryPoint is
// inside, outside, or precisely the polygon boundary.
INT32 MgSpatialUtility::PointIsInPolygon (MgCoordinateIterator* polyItr,MgCoordinate* outsidePoint,
                                                                        MgCoordinate* queryPoint)
{
    INT32 pointStatus;

    Ptr<MgCoordinateXYM> lastIntersection;
    Ptr<MgCoordinateCollection> intersectionCollection;

    // Until we know different.
    pointStatus = MgSpatialUtilityStatus::PointIsOutside;

    // Get all the intersections between the "outPoint" amd the query Point.
    intersectionCollection = PolySegIntersection (polyItr,outsidePoint,queryPoint);
    INT32 count = intersectionCollection->GetCount ();

    // If the intersection count is zero, the point is definitely not on the
    // polygon, nor is it inside the polygon.
    if (count > 0)
    {
        // Need to extract the intersection type of the last intersection.
        // It will tell us if we are on the polygon or not.
        lastIntersection = dynamic_cast<MgCoordinateXYM*>(intersectionCollection->GetItem (count-1));
        INT32 status = static_cast<INT32>(lastIntersection->GetM ());

        // If the point is on the polygon, the status of the last intersection will
        // have the IntersectIsSeg2End bit set.
        if ((status & MgSpatialUtilityStatus::IntersectIsSeg2End) != 0)
        {
            pointStatus = MgSpatialUtilityStatus::PointIsOnBoundary;
        }
        else
        {
            // The query point is not on the polygon, so its inside/outside
            // status can be determined by the number of intersections.  An
            // odd number of intersections says the point is inside.
            // This rather strange sequence of code is required to overcome
            // a glitch in the Linux build tools.
            INT32 xxx = MgSpatialUtilityStatus::PointIsOutside;
            pointStatus = ((count % 2) == 1) ? MgSpatialUtilityStatus::PointIsInside : xxx;
        }
    }
    return pointStatus;
}

///////////////////////////////////////////////////////////////////////////////
// Use this overload of the PointIsInPolygon function when only a very few
// points are to be tested against a given polygon or performance is not an
// issue.  The outsidePoint argument of the other overload is calculated upon
// each call to this function.  Return value indicates if the the queryPoint is
// inside, outside, or precisely the polygon boundary.
INT32 MgSpatialUtility::PointIsInPolygon (MgCoordinateIterator* polyItr,MgCoordinate* queryPoint)
{
    INT32 pointStatus;
    Ptr<MgCoordinate> outsidePoint;

    outsidePoint = PointOutsidePolygon (polyItr);
    pointStatus = PointIsInPolygon (polyItr,outsidePoint,queryPoint);
    return pointStatus;
}

//=============================================================================
// Returns a collection of line strings which represents those portions of the
// provided line string which are inside of the provided closed ring.  The null
// pointer is returned rather than an empty line string.
//
// To preserve developer sanity, enhance maintainability, and avoid a ton of
// spaghetti code, the process is broken down into four phases.  Data is
// communicated through the four phases via a collection of MgCoordinateXYZM
// objects.  In these objects, the status of each point is communicated in the
// M member of the collection (as a double) and the status of each line segment
// is communicated in the Z member of the collection (as a double).  The
// specific status values used are defined by the following static
// constants.
//
// Values which are stored in the M member of the internally used point
// collection; i.e. the status of each point relative to the clip ploygon.
static const INT32 StringPointNotDetermined = 0;
static const INT32 StringPointOutside       = 1;
static const INT32 StringPointOnBoundary    = 2;
static const INT32 StringPointInside        = 3;
//
// Values which are stored in the Z member of the internally used point
// collection; i.e. the status of each segment relative to the clip
// ploygon.  The value stored refers to the segment which starts at
// the point in which the segment status is stored.
static const INT32 StringSegNotDetermined = 0;
static const INT32 StringSegOutside       = 1;
static const INT32 StringSegCollinear     = 2;
static const INT32 StringSegInside        = 3;

static const double rStringPointNotDetermined = (StringPointNotDetermined + 0.01);
static const double rStringSegNotDetermined = (StringSegNotDetermined + 0.01);

MgLineStringCollection* MgSpatialUtility::ClipStringToPolygon (MgCoordinateIterator* polyItr,
                                                               MgCoordinateIterator* lineItr,
                                                               bool includeCollinear)
{
    Ptr<MgCoordinateCollection> pointCollection;
    Ptr<MgCoordinateIterator> pointIterator;
    Ptr<MgLineStringCollection> lineCollection;

    // Phase One: Expand the line string with all implied transition points,
    // i.e. intersection with the polygon.
    pointCollection = StringClipPhaseOne (lineItr,polyItr);
    pointIterator = pointCollection->GetIterator ();

    // Phase Two: Classify each point in the newly constructed point collection
    // as being outside, on, or inside the clipping boundary.  This status
    // is stored as a double in the M element of the coordinate collection.
    MgSpatialUtility::StringClipPhaseTwo (pointIterator,polyItr);

    // Phase Three: Clssify each segment in the expanded line as to its
    // clipping status relative to the polygon (i.e. inside, outside, or
    // colinear with the boundary).  Changes are made in place.  This status
    // is stored as a double in the Z element of the coordinate collection.
    StringClipPhaseThree (pointIterator,polyItr);

    // Phase Four: Construct a LineStringCollection in which each LineString
    // represents a segment of the original line string which is inside of
    // the provided polygon.
    lineCollection = StringClipPhaseFour (pointIterator,includeCollinear,false);

    // Looks pretty simple, maybe that means it will work good; often does.
    return lineCollection.Detach ();
}



///////////////////////////////////////////////////////////////////////////////
// Phase One: during a process of copying the points of the provided line
// string coordinate collection (in the form of a MgCoordinateIterator)
// insert points as necessary such that a point on the line string exists at
// each and every possible transition point.  A transition point is a location
// on the line string where the state of the line string relative to the
// provided polygon may change from one of three states to another.  The three
// states of interest are: OUSTSIDE the polygon, INSIDE the polygon, or
// ON THE BOUNDARY defined by the polygon.
//
// In this module we do not concern ourselves with the specific state which
// exists at any specific point, but only making sure that a distinct point
// exists at any point where a transition may occur.  Care is taken to make
// sure that degenerate line segments within the returned point collection are
// not created.
//
// To facilitate processing which is to be accomplished in phases two, three,
// and four of the StingClip facility, the collection returned is a collection
// of MgCoordinateXYZM objects.  The Z and M coordinates of the coordinatess in
// the returned collection will be used to convey status information about each
// coordinate in the collection from one phase to another and in this manner
// eliminate duplicate processing.  When returned from this module, the Z and
// M ordinates are set to zero (i.e. NotDetermined).
MgCoordinateCollection* MgSpatialUtility::StringClipPhaseOne (MgCoordinateIterator* lineString,
                                                              MgCoordinateIterator* itrPolygon)
{
    bool collinear;

    INT32 mask;
    INT32 index;
    INT32 count;
    INT32 status;
    INT32 insertIndex;                  // debuggin & testing only
    INT32 segmentStatus;
    INT32 collinearStatus;

    double magnitude;

    Ptr<MgCoordinate> segFromPoint;
    Ptr<MgCoordinate> segToPoint;
    Ptr<MgCoordinate> plyFromPoint;
    Ptr<MgCoordinate> plyToPoint;
    Ptr<MgCoordinate> xyzmPoint;
    Ptr<MgCoordinateCollection> xyzmCollection;     // The return value

    MgGeometryFactory mgFactory;

// TODO: Should throw an invalid argument exception if the provided
// line sting collection is empty.

    // A value indicating the magnitude of the numbers we will be
    // dealing with, SegmentIntersection needs to know this.
    magnitude = GreatestMagnitude (itrPolygon);

    // Construct the collection which we will return.
    xyzmCollection = new MgCoordinateCollection ();

    // Initialize the line string segment loop by getting the first point.
    // This will be the "toPoint" in preparation for the segment loop
    // established below.
    lineString->Reset ();
    lineString->MoveNext ();
    segToPoint = lineString->GetCurrent ();

    // Add the initial point to the collection which we will eventually return.
    xyzmPoint = mgFactory.CreateCoordinateXYZM (segToPoint->GetX(),segToPoint->GetY(),
                                                                   rStringPointNotDetermined,
                                                                   rStringSegNotDetermined);
    xyzmCollection->Add (xyzmPoint);

    // Loop once for each segment in the line string.
    while (lineString->MoveNext ())
    {
        // We need a temporary collection so that we can assure that the
        // intersection points are added to the collection we will be
        // returning in the ciorrect order.  Thus, we stash the intersections
        // in the temporary in the proper order, and then add the ordered
        // temporary to the collection we will return.
        Ptr<MgCoordinateCollection> xyzmTemporary;
        xyzmTemporary = new MgCoordinateCollection ();

        // Get the current segment of the line string.
        segFromPoint = segToPoint;
        segToPoint = lineString->GetCurrent ();

        // Initialize a loop of each segment in the polygon.
        itrPolygon->Reset ();
        itrPolygon->MoveNext ();
        plyToPoint = itrPolygon->GetCurrent ();
        while (itrPolygon->MoveNext ())
        {
            // We need a place for SegementIntersection to place the
            // calculated intersection.  We can't add the transition
            // points directly to the xyzmCollection object as in the
            // case of more than one transition point for a segment,
            // there is no guarantee that this algorithm will find them
            // in the correct order.  Therefore, we use the
            // AddToCoordinateCollection method to add all transition
            // points to the xyzmTemporary collection; and add the
            // points in the temporary collection to the main collection
            // after all transition points for this segment have been
            // determined.  AddToCoordinateCollection assures that
            // transition points are properly ordered.  To reduce the
            // possibilty of memory leaks creeping tino the code, we
            // create a new temporary collection on each iteration of
            // this loop.
            Ptr<MgCoordinate> intersection = new MgCoordinateXY ();

            // We will add the termination point of this segment of the line
            // string below.  Thus, in this loop we are only interested in
            // transition points which are not already points in the original
            // line segment.

            // Determine the end points of the polygon segment.
            plyFromPoint = plyToPoint;
            plyToPoint = itrPolygon->GetCurrent ();

            // Determine the relationship of the current line string segment
            // with the current polygon segment.
            status = SegmentIntersection (intersection,plyFromPoint,plyToPoint,segFromPoint,segToPoint,magnitude);

            // Disect the large amount of information embedded in the
            // status return.
            segmentStatus   = status & MgSpatialUtilityStatus::SegmentStatusMask;
            collinearStatus = status & MgSpatialUtilityStatus::CollinearStatusMask;
            collinear = (segmentStatus == MgSpatialUtilityStatus::SegmentsCollinear) && (collinearStatus != 0);
            if (!collinear && (segmentStatus != 0))
            {
                // There is no intersection to deal with, collinearity excepted.
                continue;
            }

            // If we are still here, we have an intersection point or a collinear
            // segment overlap.  In the normal case (i.e. not collinear) we are only
            // interested in intersections where the intersection point resides on
            // both segments.
            mask = MgSpatialUtilityStatus::IntersectOnSeg1 |  MgSpatialUtilityStatus::IntersectOnSeg2;
            if ((status & mask) == mask && !collinear)
            {
                // If the intersection point is precisely equal (ala DblCmp) to
                // either of the end points of the line string segment, we skip
                // this intersection as the point already exists in the line
                // string, we do not need to add another.
                // If the Intersection point is precisely equal to (ala DblCMp)
                // the "To" point of the polygon segment, we also skip it.
                // We'll catch it when it becomes the "From" point on the next
                // iteration (to prevent adding points which produce degenerate
                // segments).
                mask = MgSpatialUtilityStatus::IntersectIsSeg2Start |
                       MgSpatialUtilityStatus::IntersectIsSeg2End   |
                       MgSpatialUtilityStatus::IntersectIsSeg1End;
                if ((status & mask) == 0)
                {
// TODO: A performance improvement is possible here.  Since this transition
// point is an intersection point, we know it to reside on the boundary.  We
// could set the M value to indicate that and eliminate the need to determine
// this point's status in Phase Two.  We'll do this after we know all this
// stuff to work reliably.

                    // OK, we have a transition point to add to the line string.
                    xyzmPoint = mgFactory.CreateCoordinateXYZM (intersection->GetX(),intersection->GetY(),
                                                                                     rStringPointNotDetermined,
                                                                                     rStringSegNotDetermined);

                    // The resulting point collection must contain the "transition points"
                    // in sequential order otherwise we can get zig-zags or changed directions
                    // in the clipped lines.  This is what AddToCoordinateCollection
                    // does for us.  The returned value is saved only for debugging
                    // convenience, it is the index at which the point was inserted.
                    insertIndex = AddToCoordinateCollection (xyzmTemporary,xyzmPoint,segFromPoint);
                }
            }
            else if (collinear)
            {
                // We need to do something with collinear lines, sometimes.  The rather
                // verbose comments were necessary for the author to keep track of what
                // can/might/could happen and what to do about each possible situation.

                // The sixteen cases are:
                //  0000  -> DO NOTHING!! There is no overlap, the segments are disjoint.
                //  0001  -> DO NOTHING!! Not possible.
                //  0010  -> DO NOTHING!! Not possible.
                //  0011  -> DO NOTHING!! End to end continuous, but no overlap.
                //  0100  -> DO NOTHING!! Not possible.
                //  0101  -> Insert both segment 1 points, segment 1 _IS_ the ovelapping segment.
                //  0110  -> Insert segment 1 end point, overlap ends at segment 1 end point.
                //  0111  -> Insert segment 1 end point, overlap ends at segment 1 end point.
                //  1000  -> DO NOTHING!! Not possible.
                //  1001  -> Insert segment 1 start point, overlap beginns in the middle of segment 2.
                //  1010  -> DO NOTHING!! Segment 2 is the overlap segment.
                //  1011  -> DO NOTHING!! Segment 2 is the overlap segment.
                //  1100  -> DO NOTHING!! End to end continuous, but no overlap.
                //  1101  -> Insert segment 1 start point, overlap begins in the middle of segment 2.
                //  1110  -> DO NOTHING!! Segment 2 is the overlap segment.
                //  1111  -> DO NOTHING!! Segments are identical
                static const INT32 seg1StartA = (MgSpatialUtilityStatus::CollinearSeg2End | MgSpatialUtilityStatus::CollinearSeg1Start); // 1001
                static const INT32 seg1StartB = (MgSpatialUtilityStatus::CollinearSeg2End | MgSpatialUtilityStatus::CollinearSeg1End | MgSpatialUtilityStatus::CollinearSeg1Start); // 1101
                static const INT32 seg1EndA   = (MgSpatialUtilityStatus::CollinearSeg1End | MgSpatialUtilityStatus::CollinearSeg2Start); // 0110
                static const INT32 seg1EndB   = (MgSpatialUtilityStatus::CollinearSeg1End | MgSpatialUtilityStatus::CollinearSeg2Start | MgSpatialUtilityStatus::CollinearSeg1Start); // 0111
                static const INT32 seg1Both   = (MgSpatialUtilityStatus::CollinearSeg1End | MgSpatialUtilityStatus::CollinearSeg1Start); // 0101

                if (collinearStatus == seg1StartA || collinearStatus == seg1StartB || collinearStatus == seg1Both)
                {
                    // Insert the polygon "From" point.
// TODO: A performance improvement is possible here.  Since this transition
// point is an intersection point, we know it to reside on the boundary.  We
// could set the M value to indicate that and eliminate the need to determine
// this point's status in Phase Two.  We'll do this after we know all this
// stuff to work reliably.
                    xyzmPoint = mgFactory.CreateCoordinateXYZM (plyFromPoint->GetX(),plyFromPoint->GetY(),
                                                                                     rStringPointNotDetermined,
                                                                                     rStringSegNotDetermined);
                    insertIndex = AddToCoordinateCollection (xyzmTemporary,xyzmPoint,segFromPoint);
                }
                if (collinearStatus == seg1EndA || collinearStatus == seg1EndB || collinearStatus == seg1Both)
                {
                    // Insert the polygon "To" point.
// TODO: A performance improvement is possible here.  Since this transition
// point is an intersection point, we know it to reside on the boundary.  We
// could set the M value to indicate that and eliminate the need to determine
// this point's status in Phase Two.  We'll do this after we know all this
// stuff to work reliably.
                    xyzmPoint = mgFactory.CreateCoordinateXYZM (plyToPoint->GetX(),plyToPoint->GetY(),
                                                                                   rStringPointNotDetermined,
                                                                                   rStringSegNotDetermined);
                    insertIndex = AddToCoordinateCollection (xyzmTemporary,xyzmPoint,segFromPoint);
                }
            }
            // intersection variable should go out of scope and be deleted here.
        }

        // If we have some transitions, we copy them from the temporary
        // collection to the collection we will return.  They should be
        // in the proper order.
        count = xyzmTemporary->GetCount();
        for (index = 0;index < count;index += 1)
        {
            Ptr<MgCoordinate> xyzmPointTmp = xyzmTemporary->GetItem (index);
            xyzmCollection->Add (xyzmPointTmp);
        }

        // Add the line string segment end point.
        xyzmPoint = mgFactory.CreateCoordinateXYZM (segToPoint->GetX(),segToPoint->GetY(),
                                                                       rStringPointNotDetermined,
                                                                       rStringSegNotDetermined);
        xyzmCollection->Add (xyzmPoint);

        // The xyzmTemporary collection should go out of scope and be deleted
        // here.
    }

    // We're done.  Return the collection.  It should never be empty unless
    // the provided line string collection is empty (or only one point).
    // We never return a null pointer.
    return xyzmCollection.Detach ();
}

// Phase Two:  In this method, we classify each point in the provided
// point collections as being outside, inside, or on the clip boundary.
// The determination is stored in the M element of the internally
// used MgCoordinateXYZM collection.
void MgSpatialUtility::StringClipPhaseTwo (MgCoordinateIterator* lineString,
                                           MgCoordinateIterator* itrPolygon)
{
    INT32 pointStatus;

    double rPointStatus;

    Ptr<MgCoordinate> outPoint;
    Ptr<MgCoordinate> xyzmPoint;

    MgGeometryFactory mgFactory;

    Ptr<MgCoordinateXY> xyPoint = new MgCoordinateXY ();

    // Compute an outpoint.
    outPoint = MgSpatialUtility::PointOutsidePolygon (itrPolygon);

    lineString->Reset ();
    while (lineString->MoveNext ())
    {
        xyzmPoint = lineString->GetCurrent ();
        rPointStatus = xyzmPoint->GetM ();
        pointStatus = static_cast<INT32>(rPointStatus);
        if (pointStatus == StringPointNotDetermined)
        {
            // The status has not been determined yet, so we do that now.
            xyPoint->SetX(xyzmPoint->GetX());
            xyPoint->SetY(xyzmPoint->GetY());
            pointStatus = MgSpatialUtility::PointIsInPolygon (itrPolygon,outPoint,xyPoint);
            switch (pointStatus) {
            case MgSpatialUtilityStatus::PointIsOutside:
                rPointStatus = static_cast<double>(StringPointOutside) + 0.01;
                break;
            case MgSpatialUtilityStatus::PointIsOnBoundary:
                rPointStatus = static_cast<double>(StringPointOnBoundary) + 0.01;
                break;
            case MgSpatialUtilityStatus::PointIsInside:
                rPointStatus = static_cast<double>(StringPointInside) + 0.01;
                break;
            default:            // Shouldn't ever happen, for defensive purposes only.
                rPointStatus = 0.0;
                break;
            }
            xyzmPoint->SetM (rPointStatus);
        }
    }
    return;
}

// Phase Three:  Based on the fact that all possible state transitions have
// been eliminated from each line segemnt, and knowledge of the status of
// each end point of each segment, we can now classify each segment of the
// internally used MgCoordinateXYZM coordinate collection as being inside,
// outside, or colinear with the clip boundary.  The determined status is
// stuffed in the Z member of the provided coordinate collection.  The
// status value so stored refers to the segment which starts at the point
// in which the status is stored.
void MgSpatialUtility::StringClipPhaseThree (MgCoordinateIterator* lineString,
                                             MgCoordinateIterator* itrPolygon)
{
    INT32 segStatus;
    INT32 currentStatus;
    INT32 nextStatus;
    INT32 midStatus;

    double rSegStatus;

    Ptr<MgCoordinate> xyPoint;
    Ptr<MgCoordinate> outPoint;
    Ptr<MgCoordinate> currentPoint;
    Ptr<MgCoordinate> nextPoint;

    xyPoint = new MgCoordinateXY ();

    lineString->Reset ();
    lineString->MoveNext ();
    nextPoint = lineString->GetCurrent ();
    while (lineString->MoveNext ())
    {
        currentPoint = nextPoint;
        nextPoint = lineString->GetCurrent ();

        currentStatus = static_cast<INT32>(currentPoint->GetM());
        nextStatus    = static_cast<INT32>(nextPoint->GetM());

        segStatus = StringSegNotDetermined;
        if (currentStatus == StringPointInside || nextStatus == StringPointInside)
        {
            // Since state transitions have been removed, the entire segment inside
            // the clipping boundary if either end point is inside.
            segStatus = StringSegInside;
        }
        else if (currentStatus == StringPointOutside || nextStatus == StringPointOutside)
        {
            // Since state transitions have been removed, the entire segment outside
            // the clipping boundary if either end point is outside.
            segStatus = StringSegOutside;
        }
        else
        {
            // Oops!!! both end points are on the boundary.  Could be any one
            // of the three states.  Compute the midpoint of the segment and
            // determine if it is outside, inside, or on the boundary.
            double xx = 0.5 * (nextPoint->GetX () + currentPoint->GetX ());
            double yy = 0.5 * (nextPoint->GetY () + currentPoint->GetY ());
            xyPoint->SetX (xx);
            xyPoint->SetY (yy);

            // If we haven't done so already, compute the location of a point
            // guaranteed to be outside the polygon.
            if (!outPoint)
            {
                outPoint = MgSpatialUtility::PointOutsidePolygon (itrPolygon);
            }

            // Now we can determine the status of the midpoint relative to
            // the polygon.
            midStatus = MgSpatialUtility::PointIsInPolygon (itrPolygon,outPoint,xyPoint);
            if (midStatus == MgSpatialUtilityStatus::PointIsInside)
            {
                // The mid point is inside, so the whole segment is inside.
                segStatus = StringSegInside;
            }
            else if (midStatus == MgSpatialUtilityStatus::PointIsOutside)
            {
                // The mid point is outside, so the whole segment is outside.
                segStatus = StringSegOutside;
            }
            else // if (midStatus == MgSpatialUtilityStatus::PointIsOnBoundary)
            {
                // The mid point is on the boundary, so the whole segment is
                // outside.collinear with the boundary.
                segStatus = StringSegCollinear;
            }
        }
         rSegStatus = static_cast<double>(segStatus) + 0.01;
        currentPoint->SetZ (rSegStatus);
    }
}

// Phase Four:  We construct a collection of line strings from the internally
// used MgCoordinateXYZM collection.  We collect as a member line string all
// contiguous segments classified as being inside the clipping boundary.
// Normally, a segment which is collinear with the boundary is considered to
// be outside the clipping boundary and is not included in any linestring.
// This behavior can be changed by providing a 'true' value for the collinear
// argument.
//
// This function normally returns a collection of lines generated from those
// line segments which are inside of the clipping boundary.  Set the outside
// argument to 'true' to reverse this normal behavior.

MgLineStringCollection* MgSpatialUtility::StringClipPhaseFour (MgCoordinateIterator* lineString,
                                                               bool collinear,
                                                               bool outside)
{
    bool copy;          // true if current segment is to be included in result

    INT32 status;

    Ptr<MgCoordinate> currentPoint;
    Ptr<MgCoordinate> nextPoint;
    Ptr<MgCoordinateCollection> curCollection;
    Ptr<MgLineStringCollection> lineCollection;

    MgGeometryFactory mgFactory;

    curCollection = new MgCoordinateCollection ();
    lineCollection = new MgLineStringCollection ();

    lineString->Reset ();
    lineString->MoveNext();
    nextPoint = lineString->GetCurrent ();
    while (lineString->MoveNext ())
    {
        currentPoint = nextPoint;
        nextPoint = lineString->GetCurrent ();
        status = static_cast<INT32>(currentPoint->GetZ());
        copy = outside ? (status == StringSegOutside) : (status == StringSegInside);
        copy |= (collinear && (status == StringSegCollinear));
        if (copy)
        {
            if (curCollection->GetCount () == 0)
            {
                // We haven't started a current line string yet, so we do so
                // now.
                Ptr<MgCoordinate> xyPoint;
                xyPoint = mgFactory.CreateCoordinateXY (currentPoint->GetX (),currentPoint->GetY ());
                curCollection->Add (xyPoint);
            }
            // Add the end point of the current segment to the cuirrent line
            // string.
            Ptr<MgCoordinate> xyPoint;
            xyPoint = mgFactory.CreateCoordinateXY (nextPoint->GetX (),nextPoint->GetY ());
            curCollection->Add (xyPoint);
        }
        else if (curCollection->GetCount () != 0)
        {
            // We are not to copy this segment.  If we have collected a line
            // string with segments to be copied, we finish this line string
            /// and add it to the collection which we will return.
            Ptr<MgLineString> newLineString = new MgLineString (curCollection);
            lineCollection->Add (newLineString);

            // Prepare for the possible creation of yet another line string.
            curCollection->Clear ();
        }
    }

    // If curCollection has some points in it, need to add that
    // partial line to the line string collection.
    if (curCollection->GetCount () != 0)
    {
        Ptr<MgLineString> newLineString = new MgLineString (curCollection);
        lineCollection->Add (newLineString);
        curCollection->Clear ();
    }

    return lineCollection.Detach ();
}
