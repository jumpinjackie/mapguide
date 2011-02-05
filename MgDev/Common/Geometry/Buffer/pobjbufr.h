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
// FILE: pobjbufr.h.
//
// PURPOSE: Declaration of the PolyObjectBuffer class. The PolyObjectBuffer
//          acts as a base to the derived classes used to generate buffer zones
//          for polyline and polygon objects, providing methods common to both.
//
//------------------------------------------------------------------------------

#ifndef _POBJBUFR_H_
#define _POBJBUFR_H_

class PolyObjectBuffer : public PlaneSweep {

public:
    virtual ~PolyObjectBuffer();

    virtual void CreateBufferZone(ProgressCallback &callback,
        OrientedPolyPolygon &bufferPolygon);

    /// declaration of vertex accessor class
    class VertexAccessor
    {
    public:
        virtual int GetTotalVertices() const = 0;
        virtual int GetNPolyObjects() const = 0;
        virtual int GetNPolyVerts(int index) const = 0;
        virtual const OpsFloatPoint &operator[](int index) const = 0;
        virtual void GetExtent(OpsFloatExtent &extent) const = 0;
    };

#ifdef _DEBUG
public:
    static void EnableObjectVerticesTrace(BOOL enable,
        STRING traceFile = L"objverts.trc");
#endif

protected:
    OpsFloatPoint *m_pVertices;
    int *m_pnPolyVerts;
    int m_nPolyObjects;
    OpsFloatExtent m_polyObjExt;
    BufferUtility *m_pBufferUtil;

    /// constructor/destructor

    PolyObjectBuffer(BufferUtility *pBufferUtil);

    /// methods to query buffering parameters

    int GetNSegmentsPerCircle() const;
    float GetOffsetDistance() const;

    /// method to generate the buffer zone

    virtual void GenerateBufferZone(const OpsFloatPoint vertices[],
        const int nPolyVerts[], int nPolyObjects, ProgressCallback &callback,
        OrientedPolyPolygon &bufferZone);

    /// method to test whether a point lies within the offset distance of a
    /// polyline or polygon boundary

    BOOL PointWithinOffsetDist(const OpsFloatPoint vertices[], int nVertices,
        const OpsDoublePoint &point) const;

    void Initialize(const VertexAccessor &vertAccessor);
    void Cleanup();

private:
    BOOL m_bufferAlreadyGenerated;

#if defined _DEBUG
    static BOOL traceVertices;
    static STRING traceFileName;
    void TraceObjectVertices(const OpsFloatPoint vertices[],
        const int nPolyVerts[], int nPolyObjects) const;
#endif

    /// methods used to compute convex offset chains around a polyline or
    /// polygon boundary

    void CreateOffsetChains(const OpsFloatPoint vertices[], int nVertices);
};

#endif
