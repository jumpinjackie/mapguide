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
// FILE: oppolyln.h.
//
// PURPOSE: Declaration of the OpsPolyPolyline class. The OpsPolyPolyline
//          class implements a line-type graphic primitive consisting of
//          one or more polylines. Each poly-polyline is characterized
//          by an array of vertices, an array of the number of vertices
//          in each polyline, the number of polylines, and a line pen.
//
//------------------------------------------------------------------------------

#ifndef _OPPOLYLN_H_
#define _OPPOLYLN_H_

class OpsPolyPolyline : public OpsLinePrimitive
{
public:
    /// constructors/destructor

    OpsPolyPolyline();
    OpsPolyPolyline(const OpsFloatPoint vertices[], const int nPolyVerts[],
        int nPolylines, OpsFloatExtent *polyPgonExt = NULL);
    OpsPolyPolyline(const OpsPolyPolyline &polyPolyline);
    virtual ~OpsPolyPolyline();

    /// method to get the extent of the poly-polyline

    virtual OpsFloatExtent *GetExtent(OpsFloatExtent *polyPgonExt) const;
    /// method to compute the length of the poly-polyline
    virtual double GetLength() const;
    /// method to clip the poly-polyline to the specified clip rectangle
    virtual void Clip(const OpsFloatExtent *clipRect, OpsFloatPoint outVerts[],
        int nPolyVerts[], int &nPolylines) const;
    /// methods to access the vertices and vertex counts
    int GetNPolylines() const;
    int GetNPolyVerts(int index) const;
    int GetTotalVertices() const;
    OpsFloatPoint &operator[](int index) const;
    /// method to concatenate a polyline onto the poly-polyline
    void Concatenate(const OpsPolyline *polyline);
    /// overloaded assignment operator
    OpsPolyPolyline &operator=(const OpsPolyPolyline &polyline);
private:
    OpsFloatPoint *m_vertices;  // pointer to array of vertices
    int *m_nPolyVerts;          // array of polyline vertex counts
    int m_nPolylines;           // number of polylines
};

// implementation of inline methods

inline int OpsPolyPolyline::GetNPolylines() const
{
    return m_nPolylines;
}

#endif
