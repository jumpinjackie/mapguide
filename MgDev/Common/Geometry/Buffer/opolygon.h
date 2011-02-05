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
// FILE: opolygon.h.
//
// PURPOSE: Declaration of the OpsPolygon class. The OpsPolygon class
//          implements a polygon type primitive consisting of a single
//          closed polygonal boundary. Each polygon is characterized by
//          an array of boundary vertices, a count of the number of
//          vertices, an edge pen, and a brush.
//
//------------------------------------------------------------------------------

#ifndef _OPOLYGON_H_
#define _OPOLYGON_H_

class OpsPolygonPrimitive;

class OpsPolygon : public OpsPolygonPrimitive
{
public:
    /// constructors/destructor

    OpsPolygon();
    OpsPolygon(const OpsFloatPoint vertices[], int nVertices,
        OpsFloatExtent *polygonExt = NULL);
    OpsPolygon(const OpsPolygon &polygon);
    virtual ~OpsPolygon();

    /// methods to get the extent, centroid and area of the polygon

    virtual OpsFloatExtent *GetExtent(OpsFloatExtent *polygonExt) const;
    virtual OpsFloatPoint *GetCentroid(OpsFloatPoint *centroid) const;
    virtual double GetArea() const;

    /// method to clip the polygon to the specified clip rectangle

    virtual void Clip(const OpsFloatExtent *clipRect, OpsFloatPoint vertices[],
       int nPolyVerts[], int &nPolygons) const;

    /// methods to get the number of vertices, and to access the vertices
    int GetNVertices() const;
    OpsFloatPoint &operator[](int index) const;

    /// overloaded assignment operator
    OpsPolygon &operator=(const OpsPolygon &polygon);

private:
    OpsFloatPoint *m_vertices;  // pointer to array of vertices
    int m_nVertices;            // number of vertices
};

// implementation of inline methods

inline int OpsPolygon::GetNVertices() const
{
    return m_nVertices;
}

#endif
