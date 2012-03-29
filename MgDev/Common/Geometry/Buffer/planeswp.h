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
// FILE: planeswp.h.
//
// PURPOSE: Declaration of the PlaneSweep and PlaneSweepException classes. The
//          PlaneSweep class implements a framework allowing a client (where the
//          client is a derived class) to add edges bounding polygonal regions,
//          that are later "swept over" using a plane sweep algorithm to form a
//          graph of connected edges. The graph is traversed to extract exterior
//          polygon boundaries yielding a poly-polygon as output. The Plane-
//          SweepException class implements an MFC exception based means of
//          reporting exceptional conditions that may arise during execution of
//          the plane sweep algorithm.
//
//------------------------------------------------------------------------------

#ifndef _PLANESWP_H_
#define _PLANESWP_H_

#include "FloatTransform.h"

class PlaneSweep
{
public:
    static void SetSortMethod(SortMethod method);

    static int m_currentFile;

#if defined _DEBUG
    static void EnableBoundaryTraversalTrace(BOOL enableTrace,
        STRING traceFile = L"traverse.trc");
#endif

protected:
    /// constructor/destructor

    PlaneSweep();
    virtual ~PlaneSweep();

    /// method to (re)initialize the object

    void Initialize();

    /// methods to add a sequence of edges, and perform the plane sweep

    enum InteriorSide{Left, Right};
    void AddEdges(const OpsFloatPoint vertices[], int nVertices,
        InteriorSide side);
    void DoPlaneSweep(ProgressCallback &callback,
        OrientedPolyPolygon &outputPolygon);
    void DoPlaneSweep(ProgressCallback &callback, FloatTransform *pTransform,
       OrientedPolyPolygon &outputPolygon);

    /// boundary orientation enumeration

    enum Orientation {Clockwise, CounterClockwise};

    /// pure virtual method each derived class must implement, to test whether a
    /// boundary should be included as part of the output

    virtual BOOL AcceptBoundary(const OpsFloatExtent &boundaryExt,
        Orientation boundaryOrient, const OpsFloatPoint &boundaryVert,
        const OpsDoublePoint &interiorPt) const = 0;

    BOOL EdgesAreDegenerate(const OpsFloatPoint &vertex1,
        const OpsFloatPoint &vertex2, const OpsFloatPoint &vertex3) const;

protected:
    BufferUtility  *m_pBufferUtil;
    FloatTransform *m_Transform;

private:
    int m_nEdgesAdded;
    VertexAllocator *m_vertexAlloc;
    WingedEdgeAllocator *m_wingedEdgeAlloc;
    SweepEdgeAllocator *m_sweepEdgeAlloc;
    SweepTupleArray *m_tupleArray;
    int m_nBoundaryVertices;
    OpsFloatPoint *m_boundaryVertices;
    int m_boundaryArraySize;
    static const int ItemsPerProgressSubInterval;
    static SortMethod sortMethod;
    static const int BoundaryArraySize;

#if defined _DEBUG
    static BOOL traceBoundaryTraversal;
    static STRING boundaryTraversalTraceFileName;
    static FILE *boundaryTraversalTraceFile;
#endif

    void InitializeMembers();
    void Cleanup();
    void AddEdge(OpsDoublePoint *verts[2]);
    void FindStartPointIntersections(const SweepEdge *edge,
        const XOrderedTree *xOrdTree, IntersectionList *intersectList) const;
    void AddStartPointIntersections(const SweepEdge *edge1,
        const SweepEdge *edge2, IntersectionList *intersectList) const;
    void FindVerticalIntersections(const SweepEdge *vertEdge,
        const XOrderedTree *xOrdTree, IntersectionList *intersectList) const;
    void AddVerticalIntersections(const SweepEdge *vertEdge,
        const SweepEdge *edge2, IntersectionList *intersectList) const;
    int GetIntersectionInterval(const SweepEdge *edge1,
        const SweepEdge *edge2, const EventIntervalArray *eventIntArray) const;
    void GetMidpointIntersection(const SweepEdge *edge1,
        const SweepEdge *edge2, OpsDoublePoint &intersectPt) const;
    void ClampIntersection(const OpsDoublePoint *minVert1,
        const OpsDoublePoint *maxVert1, const OpsDoublePoint *minVert2,
        const OpsDoublePoint *maxVert2, OpsDoublePoint &intersectionPt) const;
    void GetOutputPolygon(ProgressCallback &callback,
        OrientedPolyPolygon &polygon);
    BOOL TraverseBoundary(WingedEdge *wEdge, WingedEdge::EdgeSide side,
        OpsFloatExtent &boundaryExt);
    void AddBoundaryVertex(const OpsFloatPoint &vertex);
    void CloseBoundary();
    void ResizeBoundaryArray();
    BOOL EdgeCrossesCutLine(const OpsFloatPoint &endPt1,
        const OpsFloatPoint &endPt2, double xCut) const;
    double Ordinate(const OpsFloatPoint &endPt1, const OpsFloatPoint &endPt2,
        double x) const;
    BOOL GetBoundaryInfo(const OpsFloatPoint boundaryVerts[], int nVertices,
        const OpsFloatExtent &boundaryExt, Orientation &boundaryOrient,
        OpsDoublePoint &interiorPt) const;
    int GetNProgressSubIntervals(int nItems) const;
};


class PlaneSweepException
{
public:
    /// possible reasons for the exception (old max points error is gone)

    enum ErrorCode {TopologicalError};

    /// constructor

    PlaneSweepException(ErrorCode errorCode) : m_errorCode(errorCode) {}

    /// method to determine the reason for the exception

    ErrorCode GetErrorCode() const {return m_errorCode;}

private:
    ErrorCode m_errorCode;
};

#endif
