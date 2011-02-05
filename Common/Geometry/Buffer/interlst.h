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
// FILE: interlst.h.
//
// PURPOSE: Declaration of the IntersectionList class. An IntersectionList
//          object is used to store the points of intersection detected during
//          the plane sweep along with lists of the intersecting edges. When
//          the points of intersection are processed, they first are sorted into
//          lexicographic order on (x, y), and then all edges intersecting at
//          common points are processed based on the sorted order.
//
//------------------------------------------------------------------------------

#ifndef _INTERLST_H_
#define _INTERLST_H_

#include "FloatTransform.h"

class IntersectionList {
public:
    /// constructor/destructor

    IntersectionList(VertexAllocator *vertAlloc,
        WingedEdgeAllocator *wingedEdgeAlloc, FloatTransform* transform);
    virtual ~IntersectionList();

    /// method to add two edges that share a common point of intersection

    bool Add(const OpsDoublePoint &intersectPt, WingedEdge *edge1,
        WingedEdge *edge2);

    /// method to traverse the list, processing intersections in lexicographic
    /// order

    void ProcessIntersections(ProgressCallback &callback,
        SortMethod sortMethod = QUICKSORT);

    int GetSize() { return m_allocator.GetNObjects(); };

    /// debug only method to trace intersections to a specified file - Note that
    /// this method should be called before calling ProcessIntersections()

#if defined _DEBUG
    static void EnableIntersectionTrace(BOOL enableTrace,
        STRING traceFile = L"intersec.trc");
#endif

    /// debug only method to dump the intersection points to file
#if defined PERF_DUMPFFGF_MAX
    void Dump2FFGF( FloatTransform* transform );
#endif

private:
    struct IntersectionRecord {
        OpsDoublePoint m_intersectionPt;
        WingedEdge *m_edge1;
        WingedEdge *m_edge2;

        int operator<(const IntersectionRecord &intersectionRecord) const;
    };

    BufferAllocator<IntersectionRecord> m_allocator;
    IntersectionProcessor *m_intersectProc;

    FloatTransform* m_Transform;

    IntersectionRecord &operator[](int index) const;
    void Sort(ProgressCallback &callback, SortMethod sortMethod);
    int GetNProgressSubIntervals(int nSteps) const;

#if defined _DEBUG
    static BOOL traceIntersections;
    static STRING intersectionTraceFileName;
#endif
};


//------------------------------------------------------------------------------
//
// METHOD: IntersectionRecord::operator<().
//
// PURPOSE: Overloaded "less-than" operator to compare two IntersectionRecords
//          based on the lexicographic order of the intersection point each
//          record contains.
//
// PARAMETERS:
//
//     Input:
//
//         intersectionRec - reference to the IntersectionRecord object to
//                           compare against.
//
//     Output:
//
//         None.
//
// RETURNS: The method returns 1 or 0 depending on whether this record is less
//          than or greater than or equal to intersectionRec respectively.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline int IntersectionList::IntersectionRecord::operator<(
    const IntersectionList::IntersectionRecord &intersectionRec) const
{
    if (m_intersectionPt.x < intersectionRec.m_intersectionPt.x)
        return 1;
    else if (m_intersectionPt.x > intersectionRec.m_intersectionPt.x)
        return 0;
    else if (m_intersectionPt.y < intersectionRec.m_intersectionPt.y)
        return 1;
    else
        return 0;

} // end: operator<()


//------------------------------------------------------------------------------
//
// METHOD: operator[]().
//
// PURPOSE: Operator used to return a reference to the indexed Intersection-
//          Record object.
//
// PARAMETERS:
//
//     Input:
//
//         index - passes the index of the IntersectionRecord.
//
//     Output:
//
//         None.
//
// RETURNS: A reference to the indexed IntersectionRecord.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline IntersectionList::IntersectionRecord &IntersectionList::operator[](
    int index) const
{
    /// check for valid index in debug mode

    assert(index >= 0);
    assert(index < m_allocator.GetNObjects());

    return *m_allocator[index];

} // end: operator[]()

#endif
