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

//////////////////////////////////////////////
///<summary>
/// Calculates the two dimensional intersection of two line segments.
/// 
///</summary>

///////////////////////////////////////////////////////////////////////////////
//
INT32 MgSpatialUtility::SegmentIntersection (MgCoordinate* result,MgCoordinate* seg1From,
                                                                  MgCoordinate* seg1To,
                                                                  MgCoordinate* seg2From,
                                                                  MgCoordinate* seg2To)
{
    INT32 status (-7);          // until we know different.

    double delX1, delY1;
    double delX2, delY2;
    double denom1, denom2;
    double num, denom;          // numerator and denominator

    CHECKNULL(result,   L"MgSpatialUtility.SegmentIntersection")
    CHECKNULL(seg1From, L"MgSpatialUtility.SegmentIntersection")
    CHECKNULL(seg1To,   L"MgSpatialUtility.SegmentIntersection")
    CHECKNULL(seg2From, L"MgSpatialUtility.SegmentIntersection")
    CHECKNULL(seg2To,   L"MgSpatialUtility.SegmentIntersection")

    // Compute the denominator which also tells us if the lines are collinear.
    delX1 = seg1To->GetX () - seg1From->GetX ();
    delY1 = seg1To->GetY () - seg1From->GetY ();
    delX2 = seg2To->GetX () - seg2From->GetX ();
    delY2 = seg2To->GetY () - seg2From->GetY ();
    denom1 = delX2 * delY1;
    denom2 = delX1 * delY2;
    if (MgMathUtility::DblCmp (denom1,denom2))
    {
        // Collinear or parallel, there is no intersection.
        status = -1;
    }
    else
    {
        // OK, we have a stable geometry and an intersection of some sort should exist.
        // Compute the intersection point.
        status = 0;
        denom = denom1 - denom2;
        num = delX1 * delX2 * (seg2From->GetY() - seg1From->GetY()) +
              delX2 * delY1 *  seg1From->GetX() -
              delX1 * delY2 *  seg2From->GetX();
        result->SetX (num / denom);

        num = delY1 * delY2 * (seg2From->GetX() - seg1From->GetX()) +
              delY2 * delX1 *  seg1From->GetY() -
              delY1 * delX2 *  seg2From->GetY();
        result->SetY (num / -denom);

        // Now to compute the location of the intersection point relative to the
        // line segments involved. This is often very important.
        if (fabs (delX1) > fabs (delY1))
        {
            // Segment one is more horizontal than vertical. We will use the X
            // value to test if the resulting point is on the segment.  Notice
            // that an interscetion point which resides (relatively) precisely
            // on the 'to' point is not considered to be on the segment.
            if (delX1 >= 0.0)
            {
                // X increases from the 'from' point to the 'to' point.
                if (result->GetX() > seg1From->GetX() && result->GetX() <= seg1To->GetX())
                {
                    status |= 1;
                }
            }
            else
            {
                // X decreases from the 'from' point to the 'to' point.
                if (result->GetX() < seg1From->GetX() && result->GetX() >= seg1To->GetX())
                {
                    status |= 1;
                }
            }
        }
        else
        {
            // First segment is more verical than horizontal. We will use the y
            // value to test if the resulting point is on the segment.
            if (delY1 >= 0.0)
            {
                // Y increases from the 'from' point to the 'to' point.
                if (result->GetY() > seg1From->GetY() && result->GetY() <= seg1To->GetY())
                {
                    status |= 1;
                }
            }
            else
            {
                // Y increases from the 'from' point to the 'to' point.
                if (result->GetY() < seg1From->GetY() && result->GetY() >= seg1To->GetY())
                {
                    status |= 1;
                }
            }
        }

        // Same stuff with the second segment, sans comments.
        if (fabs (delX2) > fabs (delY2))
        {
            if (delX2 >= 0.0)
            {
                if (result->GetX() > seg2From->GetX() && result->GetX() <= seg2To->GetX())
                {
                    status |= 2;
                }
            }
            else
            {
                if (result->GetX() < seg2From->GetX() && result->GetX() >= seg2To->GetX())
                {
                    status |= 2;
                }
            }
        }
        else
        {
            if (delY2 >= 0.0)
            {
                if (result->GetY() > seg2From->GetY() && result->GetY() <= seg2To->GetY())
                {
                    status |= 2;
                }
            }
            else
            {
                if (result->GetY() < seg2From->GetY() && result->GetY() >= seg2To->GetY())
                {
                    status |= 2;
                }
            }
        }
    }
    return status;
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Returns a list of coordinates with all of the intersection points of the
/// provided segment with the polygon provided by the first argument.  Note
/// that the first argument is a CoordinateIterator; so it doesn't have to be
/// an MgPolygon.  However, this function ASSUMES that the iterator does indeed
/// point to a closed ring.
/// </summary>
MgCoordinateCollection* MgSpatialUtility::PolySegIntersection (MgCoordinateIterator* polyItr,MgCoordinate* segFrom,
                                                                                             MgCoordinate* segTo)
{
    int status;
    INT32 insertIndex;                      // for test and debug;
    Ptr<MgCoordinateCollection> coordinateCollection;
    Ptr<MgCoordinate> intersection;
    Ptr<MgCoordinate> polyFrom;
    Ptr<MgCoordinate> polyTo;
    
    CHECKNULL(polyItr, L"MgSpatialUtility.PolySegIntersection")
    CHECKNULL(segFrom, L"MgSpatialUtility.PolySegIntersection")
    CHECKNULL(segTo,   L"MgSpatialUtility.PolySegIntersection")
  
    // Create the return object.  Quite often it gets returned empty, but we
    // will always return a "new" object (i.e. refCount == 1).  Create a work
    // point we can use.
    coordinateCollection = new MgCoordinateCollection ();
    intersection = new MgCoordinateXY ();

    // Reset the iterator so we know for sure what its state is.
    polyItr->Reset ();
    polyItr->MoveNext ();

    // Loop through each segment in the polygon.  If its an exterior ring,
    // as is usually the case, it should be in counterclockwise order.
    polyTo = polyItr->GetCurrent ();
    while (polyItr->MoveNext ())
    {
        polyFrom = polyTo;
        polyTo = polyItr->GetCurrent ();
        status = SegmentIntersection (intersection,polyFrom,polyTo,segFrom,segTo);
        if (status == 3)
        {
            // We have an intersection of interest to us.  We need to add a
            // point which will still be on the heap when we are done, so we
            // definitely do not want to add the intersection point.
            Ptr<MgCoordinate> newPoint = new MgCoordinateXY (intersection->GetX(),intersection->GetY());
            insertIndex = AddToCoordinateCollection (coordinateCollection,newPoint,segFrom);
            if (insertIndex != 0)
            {
                insertIndex = 0;
            }

            // The line segment can have more than one intersection point with
            // the polygon.  So we keep trucking.
        }
    }

    // We return the generated collection, which will often be empty.
    return coordinateCollection.Detach ();
}

// In the event that the PolySegIntersection function discovers two or more
// intersection points for a single line segment, it is important that they
// get added to the coordinate collection in the proper order.  This is a
// brute force technique, but this situation is rather rare.
INT32 MgSpatialUtility::AddToCoordinateCollection (MgCoordinateCollection* collection,MgCoordinate* newPoint,MgCoordinate* basePoint)
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
        // This happens rather in frequently, but it can happen.  Need to
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
/// <summary>
/// Returns true if the provided query point is inside of, or on, the closed
/// ring provided by the polyItr argument.
/// </summary>
bool MgSpatialUtility::PointIsInPolygon (MgCoordinateIterator* polyItr,MgCoordinate* queryPoint)
{
    bool isInside;

    INT32 count;

    double minX, maxX;
    double minY, maxY;
    double outX, outY;

    Ptr<MgCoordinate> tmpCoord;
    Ptr<MgCoordinate> outPoint;
    Ptr<MgCoordinateCollection> coordinateCollection;

    // Accunulate min/max;
    polyItr->Reset ();
    minX = minY = +9.9E+100;
    maxX = maxY = -9.9E+100;
    while (polyItr->MoveNext ())
    {
        tmpCoord = polyItr->GetCurrent ();
        if (tmpCoord->GetX() < minX) minX = tmpCoord->GetX ();
        if (tmpCoord->GetY() < minY) minY = tmpCoord->GetY ();
        if (tmpCoord->GetX() > maxX) maxX = tmpCoord->GetX ();
        if (tmpCoord->GetY() > maxY) maxY = tmpCoord->GetY ();
    }

    // Create a from point known to be outside of the polygon.
    outX = (minX >= 0.0) ? -minX : minX * 2.0;
    outY = (minY >= 0.0) ? -minY : minY * 2.0;
    outPoint = new MgCoordinateXY (outX,outY);

    // Get all the intersections between the "outPoint" amd the query Point.
    coordinateCollection = PolySegIntersection (polyItr,outPoint,queryPoint);
    count = coordinateCollection->GetCount ();

    // If the number of intersections is odd, the query point is inside.
    // Otherwise it is outside. (Zero is an even number, isn't it?)
    isInside = ((count & 1) != 0);
    
    return isInside;
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Returns a collection of line strings which represents those portions of the
/// provided line string which are inside of the provided closed ring.  The null
/// pointer is returned rather than an empty line string.
/// </summary>
MgLineStringCollection* MgSpatialUtility::ClipStringToPolygon (MgCoordinateIterator* polyItr,
                                                               MgCoordinateIterator* lineItr)
{
    bool inside;

    INT32 index;

    Ptr<MgCoordinate> lineFrom;
    Ptr<MgCoordinate> lineTo;
    Ptr<MgCoordinateCollection> curCollection;
    Ptr<MgLineStringCollection> lineCollection;

    // Need to create an empty collections, the factory doesn't do this.
    curCollection = new MgCoordinateCollection ();
    lineCollection = new MgLineStringCollection ();

    // Set up the line string iterator.  It is the primary control loop
    // for this operation.
    lineItr->Reset ();
    lineItr->MoveNext ();
    lineTo = lineItr->GetCurrent ();
    
    // We need to know in what state we are starting in.  lineTo currently
    // has the coordinates of the first point, which will become the
    // lineFrom point once the main loop gets started.
    inside = PointIsInPolygon (polyItr,lineTo);
    
    // If we are starting inside the polgon, we push the first point
    // onto the current point collection.
    if (inside)
    {
        curCollection->Add (lineTo);
    }

    while (lineItr->MoveNext ())
    {
        lineFrom = lineTo;
        lineTo = lineItr->GetCurrent ();
        
        // Intersect this segment with the polygon.
        Ptr<MgCoordinateCollection> segCollection;
        segCollection = PolySegIntersection (polyItr,lineFrom,lineTo);
        INT32 segCount = segCollection->GetCount ();

        // If the segment count is zero, there were no intersections.  If we were
        // inside, we're still inside.
        if (segCount == 0)
        {
            if (inside)
            {
                curCollection->Add (lineTo);
            }
        }
        else
        {
            // There is at least one intersection, so there will some state changing going on here.
            for (index = 0;index < segCount;index += 1)
            {
                Ptr<MgCoordinate> newPoint = segCollection->GetItem (index);

                // Terminate the current state, and initate the new state.
                // At this point, the state switches for each point in the
                // intersection list.
                if (inside)
                {
                    // We're inside.  Switch to outside state.  sewPoint is the
                    // location where we left the inside of the polygon.
                    curCollection->Add (newPoint);

                    // Make a line string out of the current point collection,
                    // and add it to the line string collection which contains
                    // the clipped segments.
                    Ptr<MgLineString> newLineString = new MgLineString (curCollection);
                    lineCollection->Add (newLineString);

                    // Clear curCollection inpreparation for the nex segment if
                    // there is one.
                    curCollection->Clear ();

                    // We're outside of the polygon now.
                    inside = false;
                }
                else
                {
                    // We were outside the polgon.  The intersection point
                    // is the point at which we reenterd the polygon.
                    curCollection->Add (newPoint);

                    // We're back inside now.
                    inside = true;
                }
            }

            // At this point, if we end up inside, there will be a point in
            // curCollection which will be the point at which we re-entered
            // the last time.  Since we are inside, we need to add the
            // lineTo point to the collection.
            if (inside)
            {
                curCollection->Add (lineTo);
            }

            // Now we do something which might appear strange: we leave the
            // curCollection object alone.  In this manner, we should eliminate
            // a lot of contiguous line strings in the line string collection.
            // Hope it works out that way.  It should, as if we are inside now,
            // the first point of the segment from the line string should also
            // be inside.
        }
    }

    // If we ended this mess in the inside state, curCollection should _NOT_ be
    // empty.  In this case, we need to add this final point collection as the
    // final line string in the line string collection.
    if (inside)
    {
        Ptr<MgLineString> newLineString = new MgLineString (curCollection);
        lineCollection->Add (newLineString);
    }

    // Release the LineStringCollection object if it is empty.
    if (lineCollection->GetCount () == 0)
    {
        lineCollection = 0;
    }

    // Looks pretty simple, maybe that means it will work good; often does.
    return lineCollection.Detach ();
}
