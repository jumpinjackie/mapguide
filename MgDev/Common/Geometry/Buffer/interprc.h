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
// FILE: interprc.h.
//
// PURPOSE: Declaration of the IntersectionProcessor class. This class is resp-
//          onsible for splitting and joining WingedEdges at detected intersec-
//          tion points. The edges are split and joined in polar order around
//          the common intersection point, and coincident edges are merged into
//          one (with one edge surviving and the other edges being marked as
//          deleted).
//
//------------------------------------------------------------------------------

#ifndef _INTERPRC_H_
#define _INTERPRC_H_

class WingedEdgeArray;

#include "FloatTransform.h"

class IntersectionProcessor {
public:
    /// constructor/destructor

    IntersectionProcessor(VertexAllocator *vertAlloc,
        WingedEdgeAllocator *edgeAlloc, FloatTransform* transform);
    virtual ~IntersectionProcessor();

    /// method to process a list of edges that share a common point of
    /// intersection

    void ProcessIntersection(const OpsDoublePoint &intersectionPt,
        WingedEdgeArray &edges);

#if defined _DEBUG
    /// debug only method to set the file to dump intersection trace data to

    void TraceIntersectionsTo(FILE *trcFile);
#endif

private:
    struct EdgeRecord{
        double m_polarAngle;
        WingedEdge *m_edge;
    };

    int m_nEdgeRecs;
    static const int EdgeRecTableSize;
    int m_maxEdgeRecs;
    EdgeRecord *m_edgeRecs;
    VertexAllocator *m_vertexAllocator;
    WingedEdgeAllocator *m_wingedEdgeAllocator;
    static const double TwoPi;

    FloatTransform *m_Transform;

#if defined _DEBUG
    FILE *traceFile;
#endif

    void SortEdges();
    void JoinEdges(const OpsDoublePoint &intersectionPt);
    BOOL EdgesCoincident(const WingedEdge *edge0,const WingedEdge *edge1) const;
    void Join(const OpsDoublePoint &intersectionPt, WingedEdge *edge0,
        WingedEdge *edge1);
    void SplitEdge(const OpsDoublePoint &splitPt, WingedEdge *edge,
        WingedEdge *&newEdge);
    void AddEdgeRecord(WingedEdge *edge, int endPtIndex);

#if defined _DEBUG
    void TraceIntersection(const OpsDoublePoint &intersectionPt) const;
#endif
};


//------------------------------------------------------------------------------
//
// METHOD: EdgesCoincident().
//
// PURPOSE: Test whether two edges have identical endpoints. Edges are
//          considered to be coincident when they share the same endpoints.
//
// PARAMETERS:
//
//     Input:
//
//         edge0 - passes a pointer to the first edge.
//         edge1 - passes a pointer to the second edge.
//
//     Output:
//
//         None.
//
// RETURNS: TRUE if the edges are coincident, and FALSE otherwise.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline BOOL IntersectionProcessor::EdgesCoincident(const WingedEdge *edge0,
    const WingedEdge *edge1) const
{
    return *edge0->m_vert[0] == *edge1->m_vert[0] &&
           *edge0->m_vert[1] == *edge1->m_vert[1];

} // end: EdgesCoincident()

#endif
