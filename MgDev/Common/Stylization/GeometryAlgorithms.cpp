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

#include "stdafx.h"
#include "GeometryAlgorithms.h"
#include "MathHelper.h"

static const double PI = 3.14159265359;

//
// GeometryAlgorithms
//

double GeometryAlgorithms::CalculateAspect(double /*center*/, double top, double bottom, double left, double right, double cellX, double cellY)
{
    assert(cellX > 0 && cellY > 0);

    double slopeX = (left - right) / (2 * cellX);
    double slopeY = (bottom - top) / (2 * cellY);
    double angle = atan2(slopeX, slopeY) * 180 / PI;

    if (slopeX < 0)
    {
        angle += 360;
    }

    return angle;
}

double GeometryAlgorithms::CalculateAspect(const Vector3D &normal)
{
    double x = normal.x;
    double y = normal.y;
    double angle = atan2(x, y) * 180 / PI; // -180 ~ 180.

    if (x < 0)
    {
        angle += 360;
    }

    return angle;
}

double GeometryAlgorithms::CalculateSlope(double /*center*/, double top, double bottom, double left, double right, double cellX, double cellY)
{
    assert(cellX > 0 && cellY > 0);

    double slopeX = (left - right) / (2 * cellX);
    double slopeY = (bottom - top) / (2 * cellY);
    double angle = atan(sqrt(slopeX * slopeX + slopeY * slopeY)) * (180 / PI); // radian --> angle
    return angle;
}

double GeometryAlgorithms::CalculateSlope(const Vector3D &normal)
{
    double vertical = fabs(normal.z);
    double horizon  = sqrt(normal.x * normal.x + normal.y * normal.y);
    double angle = atan2(horizon, vertical) * (180 / PI); // radian --> angle
    return angle;
}

double GeometryAlgorithms::CalculateHillShadeNormalized(const Vector3D &normal, const Vector3D &sun)
{
    // The dot product of two vectors.
    double dotProduct = normal.x * sun.x + normal.y * sun.y + normal.z * sun.z;

    if (dotProduct <= 0)
        return 0.0;

    return dotProduct;
}

void GeometryAlgorithms::CalculateVector(Vector3D &outVector, double dAzimuth, double dAltitude)
{
    dAzimuth *= (PI / 180.0);
    dAltitude *= (PI / 180.0);
    double cosa = cos(dAltitude);

    outVector.x = cosa * sin(dAzimuth);
    outVector.y = cosa * cos(dAzimuth);
    outVector.z = sin(dAltitude);
}
