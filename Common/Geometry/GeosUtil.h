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

#ifndef _GEOUTIL_H_
#define _GEOUTIL_H_

/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// The MgWktReaderWriter class supports reading geometry objects from a
/// string in OpenGIS Well-Known Text Format (WKT) and writing geometry
/// objects to a string in WKT format.
class MgGeosUtil
{
public:
    static bool Contains(MgGeometry* geom1, MgGeometry* geom2);
    static bool Intersects(MgGeometry* geom1, MgGeometry* geom2);
    static bool Crosses(MgGeometry* geom1, MgGeometry* geom2);
    static bool Disjoint(MgGeometry* geom1, MgGeometry* geom2);
    static bool Equals(MgGeometry* geom1, MgGeometry* geom2);
    static bool Overlaps(MgGeometry* geom1, MgGeometry* geom2);
    static bool Touches(MgGeometry* geom1, MgGeometry* geom2);
    static bool Within(MgGeometry* geom1, MgGeometry* geom2);
    static MgGeometry* Boundary(MgGeometry* geom1);
    static MgGeometry* ConvexHull(MgGeometry* geom1);
    static MgGeometry* Difference(MgGeometry* geom1, MgGeometry* geom2);
    static double Distance(MgGeometry* geom1, MgGeometry* geom2);
    static MgGeometry* Intersection(MgGeometry* geom1, MgGeometry* geom2);
    static MgGeometry* SymetricDifference(MgGeometry* geom1, MgGeometry* geom2);
    static MgGeometry* Union(MgGeometry* geom1, MgGeometry* geom2);
    static bool IsValid(MgGeometricEntity* geom1);
    static bool IsSimple(MgGeometricEntity* geom1);
    static bool IsEmpty(MgGeometricEntity* geom1);
    static bool IsClosed(MgGeometricEntity* geom1);
    static double Area(MgGeometricEntity* geom1);
    static double Length(MgGeometricEntity* geom1);
    static MgPoint* Centroid(MgGeometricEntity* geom1);
    static MgPoint* GetPointInRegion(MgGeometry* geom1);
    static MgPoint* GetPointInRing(MgGeometryComponent* geom1);
private:
    static STRING ToAwkt(MgGeometricEntity* geom1);
};
/// \endcond

#endif
