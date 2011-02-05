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

#ifndef GEOMETRYALGORITHMS_H_
#define GEOMETRYALGORITHMS_H_

#include "MdfModel.h"
#include "Vector3D.h"

///<summary>
/// Helper class for geometry related algorithms.
///</summary>
class GeometryAlgorithms
{
public:
    ///<summary>
    /// Calculate aspect value base on the nearest four grids and the grid's interval.
    ///</summary>
    static double CalculateAspect(double center, double top, double bottom, double left, double right, double cellX, double cellY);

    ///<summary>
    /// Calculate aspect value base on the grid's normal.
    ///</summary>
    static double CalculateAspect(const Vector3D &normal);

    ///<summary>
    /// Calculate slope value base on the nearest four grids and the grid's interval distances.
    ///</summary>
    static double CalculateSlope(double center, double top, double bottom, double left, double right, double cellX, double cellY);

    ///<summary>
    /// Calculate slope value base on the grid's normal.
    ///</summary>
    static double CalculateSlope(const Vector3D &normal);

    ///<summary>
    /// Calculate hillshade value. The value is the dot product of the normal and sun vector.
    /// Assumes buth Sun and Normal are normalized vectors
    ///</summary>
    static double CalculateHillShadeNormalized(const Vector3D &normal, const Vector3D &sun);

    ///<summary>
    /// Calculate vector (x,y,z) value from the azimuth and altitude.
    ///</summary>
    static void CalculateVector(Vector3D &outVector, double dAzimuth, double dAltitude);
};

#endif
