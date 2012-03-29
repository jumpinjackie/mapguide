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

/// \ingroup Geometry_Module

//------------------------------------------------------------------------------
//
// FILE: oppolygn.h.
//
// PURPOSE: Declaration of the OpsPolyPolygon class. The OpsPolyPolygon
//          class implements a polygon type graphic primitive consisting
//          of one or more polygon boundaries. Each poly-polygon is char-
//          acterized by an array of boundary vertices, an array of the
//          number of vertices in each polygon, the number of polygons,
//          an edge pen, and a brush.
//
//------------------------------------------------------------------------------

#ifndef _OPPOLYGN_H_
#define _OPPOLYGN_H_

class OpsPolygonPrimitive;

class OpsPolyPolygon : public OpsPolygonPrimitive
{
public:
    /// constructors/destructor

    OpsPolyPolygon();
    OpsPolyPolygon(const OpsFloatPoint vertices[], const int nPolyVerts[],
        int nPolygons, OpsFloatExtent *polyPgonExt = NULL);
    OpsPolyPolygon(const OpsPolyPolygon &polyPolygon);
    virtual ~OpsPolyPolygon();

    /// methods to get the extent, centroid and area of the polygon

    virtual OpsFloatExtent *GetExtent(OpsFloatExtent *polyPgonExt) const;
    virtual OpsFloatPoint *GetCentroid(OpsFloatPoint *centroid) const;
    virtual double GetArea() const;
    virtual void Clip(const OpsFloatExtent *clipRect, OpsFloatPoint vertices[],
            int nPolyVerts[], int &nPolygons) const;

    /// methods to access the vertices and vertex counts

    int GetNPolygons() const;
    int GetNPolyVerts(int index) const;
    int GetTotalVertices() const;
    OpsFloatPoint &operator[](int index) const;

    /// overloaded assignment operator

    OpsPolyPolygon &operator=(const OpsPolyPolygon &polygon);

private:
    OpsFloatPoint *m_vertices;  // pointer to array of vertices
    int *m_nPolyVerts;          // array of polygon vertex counts
    int m_nPolygons;            // number of polygons
};

// implementation of inline methods

inline int OpsPolyPolygon::GetNPolygons() const
{
    return m_nPolygons;
}

#endif
