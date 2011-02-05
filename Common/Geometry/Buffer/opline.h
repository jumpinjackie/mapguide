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
// FILE: opline.h.
//
// PURPOSE: Declaration of the OpsPolyline class. The OpsPolyline class
//          implements a line-type primitive consisting of a sequence of
//          line segments, where adjacent segments are connected at a
//          common endpoint. Each polyline is characterized by an array
//          of vertices, and a line pen.
//
//------------------------------------------------------------------------------

#ifndef _OPLINE_H_
#define _OPLINE_H_

class OpsPolyline : public OpsLinePrimitive
{
public:
    /// constructors/destructor

    OpsPolyline();
    OpsPolyline(const OpsFloatPoint vertices[], int nVertices,
        OpsFloatExtent *pLineExt = NULL);
    OpsPolyline(const OpsPolyline &polyline);
    virtual ~OpsPolyline();
    /// method to get the extent of the polyline
    virtual OpsFloatExtent *GetExtent(OpsFloatExtent *pLineExt) const;
    /// method to compute the length of the polyline
    virtual double GetLength() const;
    /// method to clip the polyline to the specified clip rectangle
    virtual void Clip(const OpsFloatExtent *clipRect, OpsFloatPoint outVerts[],
        int nPolyVerts[], int &nPolylines) const;
    /// methods to access the vertices
    int GetNVertices() const;
    OpsFloatPoint &operator[](int index) const;
    /// overloaded assignment operator
    OpsPolyline &operator=(const OpsPolyline &polyline);
private:
    OpsFloatPoint *m_vertices;  // pointer to array of vertices
    int m_nVertices;            // number of vertices
};

// implementation of inline methods

inline int OpsPolyline::GetNVertices() const
{
    return m_nVertices;
}

#endif
