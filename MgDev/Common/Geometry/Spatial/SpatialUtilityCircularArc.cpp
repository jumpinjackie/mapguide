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
#include "SpatialUtility.h"
#include "SpatialUtilityCircularArc.h"
#include "MathUtility.h"

#include <math.h>

#define MAX_NUM_POSITIONS       (5000)  // Fail-safe maximum # positions in linearised arc.
#define MAX_NUM_LINESEGMENTS    (MAX_NUM_POSITIONS-1)


// Much of this implementation code is adopted from AGDS function su_carc_pts_rad(), by Derek Wang.

MgSpatialUtilityCircularArc::MgSpatialUtilityCircularArc(MgCoordinate* start, MgCoordinate* mid, MgCoordinate* end) :
                                                           m_isValidArc(false), m_hasZ(false), m_isCircle(false)
{
    CHECKARGUMENTNULL(start, L"MgSpatialUtilityCircularArc.MgSpatialUtilityCircularArc");
    CHECKARGUMENTNULL(mid, L"MgSpatialUtilityCircularArc.MgSpatialUtilityCircularArc");
    CHECKARGUMENTNULL(end, L"MgSpatialUtilityCircularArc.MgSpatialUtilityCircularArc");

    MgGeometryFactory factory;

    m_start = start->Copy();
    m_mid = mid->Copy();
    m_end = end->Copy();
    m_radius = MgMathUtility::GetQuietNan();
    m_length = MgMathUtility::GetQuietNan();
    m_hasZ = start->GetDimension() & mid->GetDimension() & end->GetDimension() & MgCoordinateDimension::XYZ;

    m_unitNormalVector = factory.CreateCoordinateXYZ(0.0, 0.0, 0.0);
    m_center = factory.CreateCoordinateXYZ(0.0, 0.0, 0.0);
//  m_center->SetDimensionality(m_start->GetDimension());

    m_startAngle.m_theta = MgMathUtility::GetQuietNan();
    m_startAngle.m_phi   = MgMathUtility::GetQuietNan();
    m_endAngle.m_theta = MgMathUtility::GetQuietNan();
    m_endAngle.m_phi   = MgMathUtility::GetQuietNan();

    // Two main cases are supported by the three-point definition:
    //  - three distinct, non-collinear points (start, end, midpt somewhere in the
    //    curve) define an open arc
    //  - start&end coincide exactly, midpt is diametrically opposite, defining
    //    a circle (closed arc), presumed to be counterclockwise in direction
    //    when viewed from above (even if the circle isn't flat)
    //        - tilted circle: support midpt's Z ordinate not being the same as start/end,
    //          circle is tilted; the diameter that is perpendicular to the start-mid vector
    //          is parallel to XY plane.
    // It is possible to define a vertical circle, but one must assume the direction of the
    // normal (which will be somewhere in the XY plane).  That is, one cannot define an
    // arbitrary vertical circle using three points.

    if (MgSpatialUtility::ArePositionsEqualXYZ(start, end))
    {
        // This might be a supported circle case.
        if (!MgSpatialUtility::ArePositionsEqualXY(start, mid))
        {
            // It is.
            m_isCircle = true;

            // See if we should to lock onto "flat" circle to avoid floating point drift.
            if (MgMathUtility::AreEqualAndMaybeNan(start->GetZ(), mid->GetZ()))
            {
                // Normal always has Z, even if geometry is XY only.
                m_unitNormalVector = factory.CreateCoordinateXYZ(0.0, 0.0, 1.0);
            }
            else
            {
                // Tilted circle case.
                // First, we need a vector that is perpendicular, in the XY plane, to
                // the start-mid vector.
                double dx = mid->GetX() - start->GetX();
                double dy = mid->GetY() - start->GetY();
                double diameterInXY = sqrt (dx*dx + dy*dy);
                double startMidAngle = atan2(dy, dx);
                double perpendicularInXYAngle = startMidAngle + 0.5*MgMathUtility::GetPi();
                double perpendicularX = start->GetX() + diameterInXY * cos(perpendicularInXYAngle);
                double perpendicularY = start->GetX() + diameterInXY * sin(perpendicularInXYAngle);
                Ptr<MgCoordinate> perpendicularInXYPos = factory.CreateCoordinateXYZ(perpendicularX, perpendicularY, start->GetZ());

                // Now compute the unit normal to the circle.
                m_unitNormalVector = MgSpatialUtility::UnitNormalVector(start, mid, perpendicularInXYPos);
            }
        } // Let the case of three identical points fall through.  It will be left marked as invalid.
    }
    else
    {
        // It is not a circle -- treat as a conventional arc.
        // This still could fail, in which case the unit normal is (0,0,0).
        m_unitNormalVector = MgSpatialUtility::UnitNormalVector(start, mid, end);
    }

    if (m_unitNormalVector->GetX() != 0.0 || m_unitNormalVector->GetY() != 0.0 || m_unitNormalVector->GetZ() != 0.0)
    {
        m_isValidArc = true;
        ComputeCenterRadiusAnglesFromThreePositionsAndNormal();
        ComputeLengthFromCenterRadiusAnglesAndNormal();
    }
}


bool MgSpatialUtilityCircularArc::IsFlat()
{
    // Note that the calculations for the normal vector latch to zero
    // as a special case, so these exact comparisons are valid.
    return m_unitNormalVector->GetX() == 0.0 && m_unitNormalVector->GetY() == 0.0;
}


bool MgSpatialUtilityCircularArc::IsDirectionCounterClockWise()
{
    return m_unitNormalVector->GetZ() > 0;
}


void MgSpatialUtilityCircularArc::ComputeLengthFromCenterRadiusAnglesAndNormal()
{
    double pi = MgMathUtility::GetPi();
    double angleDiff = 0.0;

    if (m_isCircle)
    {
        angleDiff = 2.0 * pi;
    }
    else if (IsFlat())
    {
        // In the 2D case, simply use difference in "theta" around the Z axis.

        double startAngle = m_startAngle.m_theta;
        double endAngle = m_endAngle.m_theta;

        // Make sure that angles' magnitudes reflect direction.
        if (IsDirectionCounterClockWise() && startAngle > endAngle)
            endAngle += 2.0 * pi;
        else if (!IsDirectionCounterClockWise() && startAngle < endAngle)
            endAngle -= 2.0 * pi;

        angleDiff = fabs(endAngle - startAngle);
    }
    else
    {
        // In the 3D case, use spherical trigonometry.  To avoid ill-conditioning
        // of the arccosine  and arcsine functions, this will be the two-parameter
        // arctangent form of the Haversine formula (e.g. rather than the Law of Cosines).
        //
        // The ill-conditioning of certain angles stretch
        // the accuracy of the trig functions.  For example, the arcosine
        // function breaks down for values near 1.0...
        //       cos (5 degrees) = 0.996194698
        //       cos (1 degree) = 0.999847695
        //       cos (1 minute) = 0.9999999577
        //       cos (1 second) = 0.9999999999882
        //       cos (0.05 sec) = 0.999999999999971
        // Credits:
        //       http://www.census.gov/cgi-bin/geo/gisfaq?Q5.1
        //       http://mathforum.org/library/drmath/view/51879.html
        //       http://mathforum.org/library/drmath/view/51711.html
        //       http://mathforum.org/library/drmath/view/54680.html

        double lon1 = m_startAngle.m_theta;
        double lon2 = m_endAngle.m_theta;
        double lat1 = m_startAngle.m_phi;
        double lat2 = m_endAngle.m_phi;

        // Haversine formula.
        double dLon = lon2 - lon1;
        double dLat = lat2 - lat1;
        double sinDLat2 = sin(dLat/2);
        double sinDLon2 = sin(dLon/2);
        double a = (sinDLat2*sinDLat2) + cos(lat1) * cos(lat2) * (sinDLon2*sinDLon2);
        angleDiff = 2.0 * atan2(sqrt(a), sqrt(1-a));

        // We now know the angle of minimum distance between the positions, but this might
        // be in the wrong direction.  Try rotating the start position about the normal
        // in both directions.  Whichever gets us closer to the end position is the
        // direction we want.  It should be an exact match, but we don't have a tolerance
        // available to guard against floating point drift.
        // We could use a fraction of the length and check proximity to the circle's
        // "mid" intermediate position to guard against ambiguity for semi-circles, but
        // we don't need that for a length calculation (the length is the same in either
        // direction).

        Ptr<MgCoordinate> rotatedForward = MgSpatialUtility::RotatePosition(
            m_start, angleDiff, m_unitNormalVector, m_center, false, 0);

        Ptr<MgCoordinate> rotatedBackward = MgSpatialUtility::RotatePosition(
            m_start, -angleDiff, m_unitNormalVector, m_center, false, 0);

        double forwardDistance = MgSpatialUtility::DistanceBetweenPositionsXYZ(rotatedForward, m_end);
        double backwardDistance = MgSpatialUtility::DistanceBetweenPositionsXYZ(rotatedBackward, m_end);

        if (forwardDistance > backwardDistance)
            angleDiff = (2.0*pi) - angleDiff;
    }
    m_length = m_radius * angleDiff;
}


void MgSpatialUtilityCircularArc::ComputeCenterRadiusAnglesFromThreePositionsAndNormal()
{
    MgGeometryFactory factory;

    //Compute center.
    if (m_isCircle)
    {
        // This assumes that the three-point constructor was used.
        // The start and end positions coincide, while the mid position is
        // diametrically opposite.
        double x = 0.5 * (m_mid->GetX() + m_start->GetX());
        double y = 0.5 * (m_mid->GetY() + m_start->GetY());

        if (m_hasZ)
            m_center = factory.CreateCoordinateXYZ(x, y, 0.5 * (m_mid->GetZ() + m_start->GetZ()));
        else
            m_center = factory.CreateCoordinateXY(x, y);
    }
    else // It is an unclosed arc.
    {
        ComputeCenterFromThreePositions();
    }

    // Compute radius.
    m_radius = MgSpatialUtility::DistanceBetweenPositionsXYZ(m_start, m_center);

    // Compute start_angle and end_angle

    m_startAngle.m_theta = atan2(m_start->GetY() - m_center->GetY(),
                                 m_start->GetX() - m_center->GetX());

    if (m_startAngle.m_theta == -MgMathUtility::GetPi())
        m_startAngle.m_theta = MgMathUtility::GetPi();

    m_startAngle.m_phi = m_hasZ? asin((m_start->GetZ() - m_center->GetZ()) / m_radius) : 0.0;

    m_endAngle.m_theta = atan2(m_end->GetY() - m_center->GetY(),
                               m_end->GetX() - m_center->GetX());

    if (m_endAngle.m_theta == -MgMathUtility::GetPi())
        m_endAngle.m_theta = MgMathUtility::GetPi();

    m_endAngle.m_phi = m_hasZ? asin((m_end->GetZ() - m_center->GetZ()) / m_radius) : 0.0;
}


/************************************************************************
*   Find the center point of the circular arc determined by three       *
*   given points                                                        *
*                                                                       *
*   Denote V, V1, V2, V3 the vectors of center, start, mid, end         *
*       respectively,                                                   *
*   then V = (x,y,z) satisfies the following equtions:                  *
*   ( V - ( V1 + V2 ) / 2 ) * ( V2 - V1 ) = 0                           *
*   ( V - ( V1 + V3 ) / 2 ) * ( V3 - V1 ) = 0                           *
*   ( V - V1 ) * ( ( V2 - V1 ) X ( V3 - V1 ) ) = 0                      *
*                                                                       *
*   i.e.,                                                               *
*                                                                       *
*   x21 * x + y21 * y + z21 * z = ( ||V2||^2 - ||V1||^2 ) / 2           *
*   x31 * x + y31 * y + z31 * z = ( ||V3||^2 - ||V1||^2 ) / 2           *
*   Dyz * x + Dzx * y + Dxy * z = Dxyz                                  *
*                                                                       *
*   where                                                               *
*   ( x21, y21, z21 ) = V2 - V1,                                        *
*   ( x31, y31, z31 ) = V3 - V1,                                        *
*   Dyz = y21 * z31 - y31 * z21,                                        *
*   Dzx = z21 * x31 - z31 * x21,                                        *
*   Dxy = x21 * y31 - x31 * y21,                                        *
*                                                                       *
*   Dxyz = V1 * ( V2 X V3 )                                             *
*                                                                       *
*         | x1  y1  z1  |                                               *
*       = | x21 y21 z21 | = x1 * Dyz + y1 * Dzx + z1 * Dxy              *
*         | x31 y31 z31 |                                               *
*                                                                       *
*   ||Vi||^2 = xi*xi + yi*yi + zi*zi                                    *
*                                                                       *
************************************************************************/
void MgSpatialUtilityCircularArc::ComputeCenterFromThreePositions()
{
    double a[9], b[3], tmp;
    int eindex[3];
    int i;

    MgGeometryFactory factory;

    a[0] = m_mid->GetX() - m_start->GetX(); /* x21 */
    a[1] = m_mid->GetY() - m_start->GetY(); /* y21 */
    a[2] = m_hasZ? (m_mid->GetZ() - m_start->GetZ()) : 0.0; /* z21 */

    a[3] = m_end->GetX() - m_start->GetX(); /* x31 */
    a[4] = m_end->GetY() - m_start->GetY(); /* y31 */
    a[5] = m_hasZ? (m_end->GetZ() - m_start->GetZ()) : 0.0; /* z31 */

              /* y21 * z31 - y31 * z21 */
    a[6] = a[1] * a[5] - a[4] * a[2]; /* Dyz */
              /* z21 * x31 - z31 * x21 */
    a[7] = a[2] * a[3] - a[5] * a[0]; /* Dzx */
              /* x21 * y31 - x31 * y21 */
    a[8] = a[0] * a[4] - a[3] * a[1]; /* Dxy */

    tmp = m_start->GetX() * m_start->GetX() +
          m_start->GetY() * m_start->GetY() +
          (m_hasZ? (m_start->GetZ() * m_start->GetZ()) : 0.0);

    b[0] = m_mid->GetX() * m_mid->GetX() +
           m_mid->GetY() * m_mid->GetY() +
           (m_hasZ? (m_mid->GetZ() * m_mid->GetZ()) : 0.0);

    b[1] = m_end->GetX() * m_end->GetX() +
           m_end->GetY() * m_end->GetY() +
           (m_hasZ? (m_end->GetZ() * m_end->GetZ()) : 0.0);

    b[0] -= tmp;
    b[1] -= tmp;
    b[0] *= 0.5;
    b[1] *= 0.5;
    /* Dxyz = x1 * Dyz + y1 * Dzx + z1 * Dxy */
    b[2] = m_start->GetX() * a[6] +
           m_start->GetY() * a[7] +
           (m_hasZ? (m_start->GetZ() * a[8]) : 0.0);

    /* Decompose a into LU */
    if (MgMathUtility::LUDecompose(3, a, eindex ) == 0)
    {
        // Zero result means that points were collinear.
        throw new MgInvalidArgumentException(L"MgSpatialUtilityCircularArc.ComputeCenterFromThreePositions", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    /* Perform row exchanges for b */
    for (i=0; i<3; ++i)
    {
        if (eindex[i] != i)
        {
            tmp = b[i];
            b[i] = b[eindex[i]];
            b[eindex[i]] = tmp;
        }
    }

    /* Use L (UV) = b to solve UV */
    b[1] -= a[3] * b[0];
    b[2] -= a[6] * b[0] + a[7] * b[1];

    /* Use UV = b to solve V, the center */
    double zz = m_hasZ? (b[2] / a[8]) : 0.0;
    double yy = (b[1] - a[5] * (m_hasZ? zz : 0.0)) / a[4];
    double xx = (b[0] - a[1] * yy - a[2] * (m_hasZ? zz : 0.0)) / a[0];

    m_center = factory.CreateCoordinateXYZ(xx, yy, zz);
}


// This implementation is adopted from the AGDS function lc_feat_:interp_arc(),
// originally by Gavin Cramer, and the ut_carc_ip_() package, by Derek Wang.
// The original logic is 2D, with an extension for 3D added here by calling
// MgSpatialUtility::RotatePosition().
void MgSpatialUtilityCircularArc::Linearize(
    MgCoordinateCollection* distinctPositions,
    double maxSpacing,
    double maxOffset)
{
    MgGeometryFactory factory;

    bool isFlat = IsFlat();

    if (maxSpacing < 0.0)
    {
        STRING buffer;
        MgUtil::DoubleToString(maxSpacing, buffer);

        MgStringCollection arguments;
        arguments.Add(L"2");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgSpatialUtilityCircularArc.Linearize",
            __LINE__, __WFILE__, &arguments, L"MgValueCannotBeLessThanZero", NULL);
    }

    if (maxOffset < 0.0)
    {
        STRING buffer;
        MgUtil::DoubleToString(maxOffset, buffer);

        MgStringCollection arguments;
        arguments.Add(L"3");
        arguments.Add(buffer);

        throw new MgInvalidArgumentException(L"MgSpatialUtilityCircularArc.Linearize",
            __LINE__, __WFILE__, &arguments, L"MgValueCannotBeLessThanZero", NULL);
    }

    // Avoid mathematical exceptions later.
    maxSpacing = MgMathUtility::SnapToZero(maxSpacing);
    maxOffset = MgMathUtility::SnapToZero(maxOffset);

    double spacing = 0.0;
    bool maxSpacingIsUseful = (maxSpacing > 0.0 && maxSpacing <= 0.5*m_length);
    bool maxOffsetIsUseful = (maxOffset > 0.0);

    if (m_isValidArc && (maxSpacingIsUseful || maxOffsetIsUseful))
    {
        if (maxOffsetIsUseful)
            spacing = BestArcStep(m_radius, m_length, maxOffset);
        if (spacing <= 0.0)
            maxOffsetIsUseful = false;

        if (maxSpacingIsUseful && (spacing > maxSpacing || !maxOffsetIsUseful))
            spacing = maxSpacing;

        if (maxOffsetIsUseful)
        {
            double resolution = maxOffset * 2.0;
            if (spacing < resolution)
                spacing = resolution;
        }
    }

    if (spacing <= 0.0)
    {
        // Interpolation is not possible.  Just append known positions.
        MgSpatialUtility::AppendPositionToDistinctCollection(distinctPositions, m_start);
        MgSpatialUtility::AppendPositionToDistinctCollection(distinctPositions, m_mid);
        MgSpatialUtility::AppendPositionToDistinctCollection(distinctPositions, m_end);
    }
    else
    {
        INT32 numLineSegments = (INT32) ceil(m_length / spacing);

        if (numLineSegments > MAX_NUM_LINESEGMENTS)
            numLineSegments = MAX_NUM_LINESEGMENTS;

        // Slightly adjust for even spacing of positions along curve.
        spacing = m_length / numLineSegments;

        double angleIncrement = spacing / m_radius;

        // These get carried along, even if they are NaN.
        double z = m_start->GetZ();
        double mStart = m_start->GetM();
        double mEnd = m_end->GetM();

        // Use start position exactly.
        MgSpatialUtility::AppendPositionToDistinctCollection(distinctPositions, m_start);

        // Compute intermediate positions.
        for (INT32 i=1; i<numLineSegments; ++i)
        {
            double angleOffset = i * angleIncrement;
            Ptr<MgCoordinate> pos;
            double proportion = (double)i / (double)numLineSegments;
            double m = MgMathUtility::LinearInterpolate(mStart, mEnd, proportion);

            if (isFlat)
            {
                // Use simplest possible calculations in 2D case.
                if (!IsDirectionCounterClockWise())
                    angleOffset *= -1.0; // Adjust for arc orientation about Z axis.

                double angle = m_startAngle.m_theta + angleOffset;
                double x = m_center->GetX() + m_radius * cos(angle);
                double y = m_center->GetY() + m_radius * sin(angle);

                pos = factory.CreateCoordinateXYZM(x, y, z, m);

                CHECKNULL(pos, L"MgSpatialUtilityCircularArc.Linearize")
            }
            else
            {
                // Use 3D rotation of start point about the normal vector to trace the arc.
                // This is better than applying spherical math and "great circle" geographic
                // travelling logic with range and bearing, which fails at the "poles", where
                // the bearing is undefined.
                // Great circle logic might be applicable if this arc linearization were to be
                // enhanced to take geographic ellipsoids into account in the future.
                pos = MgSpatialUtility::RotatePosition(m_start, angleOffset, m_unitNormalVector, m_center, true, m);
//              pos->SetM(m);
            }
//          pos->SetDimensionality(m_start->GetDimension());
            MgSpatialUtility::AppendPositionToDistinctCollection(distinctPositions, pos);
        }

        // Use end position exactly.
        MgSpatialUtility::AppendPositionToDistinctCollection(distinctPositions, m_end);
    }
}


/************************************************************************
*    BestArcStep - Compute best interval for interpolating arc          *
*                                                                       *
*    Consider a small arc from a circle centered at the origin.         *
*    Consider approximating the arc with a chord between the arc's      *
*    endpoints.  Position the arc so that its mid-point lies on         *
*    the positive x-axis and the chord is vertical.                     *
*    The maximum error will be between the arc's and chord's midpoints, *
*    which both lie on the x-axis.  For an arc of length C              *
*    depicting an angle of THETA from a circle of radius R, the         *
*    chord will be at:                                                  *
*        X = R cos( THETA/2 )                                           *
*    and the difference, DX, from R is:                                 *
*        DX = R - X                                                     *
*           = R ( 1 - cos( THETA/2 ) )                                  *
*           = R ( 1 - cos( C / 2R ) )                                   *
*    DX is given as max_err.  Solving for C, we get:                    *
*                                                                       *
*        C = 2R arccos( 1 - DX/R )                                      *
*                                                                       *
*    This implementation is adopted from the AGDS function              *
*    su_carc_best_step, originally by Gavin Cramer                      *
*    NOTE: this formula should be reviewed for ill-conditioning. The    *
*    use of arccosine may be inappropriate for small angles, and an     *
*    alternative may be needed, similar to the use of Haversine         *
*    formula instead of Law of Cosines in                               *
*    ComputeLengthFromRadiusAngles().                                   *
*                                                                       *
************************************************************************/
double MgSpatialUtilityCircularArc::BestArcStep(double radius, double arcLength, double maximumError)
{
    // Avoid mathematical exceptions.
    if (MgMathUtility::SnapToZero(radius) <= 0.0)
    {
        STRING buffer;
        MgUtil::DoubleToString(radius, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);
        throw new MgInvalidArgumentException(L"MgSpatialUtilityCircularArc.BestArcStep", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    if (MgMathUtility::SnapToZero(maximumError) <= 0.0)
    {
        STRING buffer;
        MgUtil::DoubleToString(maximumError, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);
        throw new MgInvalidArgumentException(L"MgSpatialUtilityCircularArc.BestArcStep", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    double arcStep = 0.0;
    double resolution = maximumError * 2.0;

    double portion = 1.0 - maximumError / radius;

    if (portion < 1.0 && portion > 0.0)
    {
        arcStep = 2.0 * radius * acos(portion);

        // Avoid step length that is too short for the resolution.
        if (arcStep < resolution)
            arcStep = resolution;

        // Arc step is too long for interpolation.
        if (arcStep > 0.5*arcLength)
            arcStep = 0.0;
    }
    else
    {
        // Arc step is too long for interpolation.
        // Leave as default 0.0.
    }

    return arcStep;
}
