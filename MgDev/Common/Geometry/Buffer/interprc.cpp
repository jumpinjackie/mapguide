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

//------------------------------------------------------------------------------
//
// FILE: interprc.cpp.
//
// PURPOSE: Implementation of the IntersectionProcessor class.
//
//------------------------------------------------------------------------------

#include "Foundation.h"
#include "buffer.h"

const int IntersectionProcessor::EdgeRecTableSize = 16;
const double IntersectionProcessor::TwoPi = 2.0 * ::acos(-1.0);


//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Initializes an instance of the IntersectionProcessor class.
//
// PARAMETERS:
//
//     Input:
//
//         vertAlloc - passes a pointer to the vertex allocator object that is
//                     to be used to allocate new edge vertices. The object
//                     pointed to is expected to have a lifetime at least that
//                     of the IntersectionProcessor object.
//         edgeAlloc - passes a pointer to the edge allocator object that is to
//                     be used to allocate new edges. The object pointed to is
//                     expected to have a lifetime at least that of the Inter-
//                     sectionProcessor object.
//
//     Output:
//
//         None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to initialize the object.
//
//------------------------------------------------------------------------------

IntersectionProcessor::IntersectionProcessor(VertexAllocator *vertAlloc,
    WingedEdgeAllocator *edgeAlloc, FloatTransform* transform) :
    m_nEdgeRecs(0),
    m_maxEdgeRecs(EdgeRecTableSize),
    m_edgeRecs(NULL),
    m_vertexAllocator(vertAlloc),
    m_wingedEdgeAllocator(edgeAlloc),
    m_Transform(transform)
{
    m_edgeRecs = new EdgeRecord[m_maxEdgeRecs];

#if defined _DEBUG
    traceFile = NULL;
#endif

} // end: constructor


//------------------------------------------------------------------------------
//
// METHOD: Destructor.
//
// PURPOSE: Release all dynamically allocated resources.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

IntersectionProcessor::~IntersectionProcessor()
{
    delete [] m_edgeRecs;

} // end: destructor


//------------------------------------------------------------------------------
//
// METHOD: ProcessIntersection().
//
// PURPOSE: Split and join a set of WingedEdge's at the specified intersection
//          point.
//
// PARAMETERS:
//
//     Input:
//
//         intersectionPt - passes a reference to the intersection point.
//         edges          - an array of all edges that intersect at the spec-
//                          ified point.
//
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to process the intersecting edges.
//
//------------------------------------------------------------------------------

void IntersectionProcessor::ProcessIntersection(
    const OpsDoublePoint &intersectionPt, WingedEdgeArray &edges)
{
    // loop through the edges generating an EdgeRecord for each of them

    m_nEdgeRecs = 0;

    for (int i = 0; i < edges.GetNEdges(); i++) {
        // if the intersection point is one of the edge endpoints, then
        // just add it

        if (intersectionPt == *edges[i]->m_vert[0])
            AddEdgeRecord(edges[i], 0);
        else if (intersectionPt == *edges[i]->m_vert[1])
            AddEdgeRecord(edges[i], 1);

        // else split the edge and add the two resulting edges

        else {
            WingedEdge *newEdge;
            SplitEdge(intersectionPt, edges[i], newEdge);
            AddEdgeRecord(edges[i], 0);
            AddEdgeRecord(newEdge, 1);
        }
    }

    // if more than two edge records, or the edges are coincident then sort
    // them into polar order before joining them about the intersection point

    if (m_nEdgeRecs > 2 || EdgesCoincident(edges[0], edges[1])) {
        SortEdges();
        JoinEdges(intersectionPt);
    }

    // else must have two non-coincident edges joined at a common endpoint -
    // handle this case directly since it occurs frequently

    else
        edges[0]->Join(edges[1], &intersectionPt);

#if defined _DEBUG
    if (traceFile != NULL)
        TraceIntersection(intersectionPt);
#endif

} // end: ProcessIntersection()


//------------------------------------------------------------------------------
//
// METHOD: SortEdges().
//
// PURPOSE: Sort the edges about the intersection point according to polar
//          order.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void IntersectionProcessor::SortEdges()
{
    // just do a straight selection sort, since the number of intersecting
    // edges will (hopefully) be small enough so that a more sophisticated
    // sorting algorithm is not called for

    for (int i = 0, k; i < m_nEdgeRecs - 1; i++) {
        k = i;

        for (int j = i + 1; j < m_nEdgeRecs; j++) {
            if (m_edgeRecs[j].m_polarAngle < m_edgeRecs[k].m_polarAngle)
                k = j;
        }

        if (i != k) {
            EdgeRecord temp = m_edgeRecs[i];
            m_edgeRecs[i] = m_edgeRecs[k];
            m_edgeRecs[k] = temp;
        }
    }

} // end: SortEdges()


//------------------------------------------------------------------------------
//
// METHOD: JoinEdges().
//
// PURPOSE: Join all the edges referenced by the array of edge records about the
//          specified intersection point (the intersection point coincides with
//          one of the endpoints of each of the edges).
//
// PARAMETERS:
//
//     Input:
//
//         intersectionPt - passes a reference to the intersection point.
//
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void IntersectionProcessor::JoinEdges(const OpsDoublePoint &intersectionPt)
{
    // join the last edge to the first if the intersection points coincides
    // with the first endpoint of the last edge, or if the edges are not
    // coincident (if these conditions are not met, then all edges must be
    // be coincident)

    WingedEdge *edge0 = m_edgeRecs[m_nEdgeRecs-1].m_edge;
    WingedEdge *edge1 = m_edgeRecs[0].m_edge;

    if (intersectionPt == *edge0->m_vert[0] || !EdgesCoincident(edge0, edge1))
        Join(intersectionPt, edge0, edge1);

    // loop through the edges in increasing polar order, joining them in
    // counter-clockwise order

    for (int i = 0; i < (m_nEdgeRecs - 1); ) {
        edge0 = m_edgeRecs[i].m_edge;
        edge1 = m_edgeRecs[i+1].m_edge;

        // join the first edge to the second edge if the intersection point
        // coincides with the first endpoint of the first edge, or if the
        // edges are not coincident

        if (intersectionPt==*edge0->m_vert[0]||!EdgesCoincident(edge0, edge1)){
            Join(intersectionPt, edge0, edge1);
            i++;
        }

        // else the intersection point coincides with the last endpoint of the
        // first edge, and the first and second edges are coincident - must
        // have a sequence of two or more coincident edges. Merge the edges
        // and mark all but the last in the sequence as deleted.

        else {
            int j = i + 1;
            BOOL minVertSideIn = edge0->SideIn(WingedEdge::MinVertSide);
            BOOL maxVertSideIn = edge0->SideIn(WingedEdge::MaxVertSide);
            WingedEdge *firstEdge = NULL, *lastEdge = NULL;

            // Check if edge0 is linked to other non-coincident edges from
            // vert[0]

            if (!EdgesCoincident(edge0, edge0->m_cwEdge[1]))
                firstEdge = edge0;
            else if (!EdgesCoincident(edge0, edge0->m_ccwEdge[0]))
                lastEdge = edge0;

            edge0->SetEdgeDeleted();

            // determine union of side-in flags and mark edges as deleted

            while (j < (m_nEdgeRecs-1) &&
                       EdgesCoincident(edge1, m_edgeRecs[j+1].m_edge)) {
                if (edge1->SideIn(WingedEdge::MinVertSide))
                    minVertSideIn = TRUE;

                if (edge1->SideIn(WingedEdge::MaxVertSide))
                    maxVertSideIn = TRUE;

                // Check if edge1 is linked to other non-coincident edges from
                // vert[0]

                if (!firstEdge && !EdgesCoincident(edge1, edge1->m_cwEdge[1]))
                     firstEdge = edge1;
                else if (!lastEdge&&!EdgesCoincident(edge1,edge1->m_ccwEdge[0]))
                     lastEdge = edge1;

                edge1->SetEdgeDeleted();
                edge1 = m_edgeRecs[++j].m_edge;
            }

            // patch up links to other edges from vert[0]

            if (firstEdge != NULL) {
                edge1->m_cwEdge[1] = firstEdge->m_cwEdge[1];
                edge1->m_cwEdge[1]->ResetWings(firstEdge, edge1);
            }
            else if (edge1->m_cwEdge[1]->EdgeDeleted())
                edge1->m_cwEdge[1] = edge1;

            if (lastEdge != NULL) {
                edge1->m_ccwEdge[0] = lastEdge->m_ccwEdge[0];
                edge1->m_ccwEdge[0]->ResetWings(lastEdge, edge1);
            }
            else if (edge1->m_ccwEdge[0]->EdgeDeleted())
                edge1->m_ccwEdge[0] = edge1;

            // patch up link to edge from vert[1] - the first edge in
            // sequence of coincident edges may have been joined to another
            // non-coincident edge

            if (edge0->m_ccwEdge[1] != edge0) {
                edge1->m_ccwEdge[1] = edge0->m_ccwEdge[1];
                edge1->m_ccwEdge[1]->ResetWings(edge0, edge1);
            }

            // update the side-in flags if necessary

            if (minVertSideIn)
                edge1->SetSideIn(WingedEdge::MinVertSide);

            if (maxVertSideIn)
                edge1->SetSideIn(WingedEdge::MaxVertSide);

            i = j;
        }
    }

} // end: JoinEdges()


//------------------------------------------------------------------------------
//
// METHOD: Join().
//
// PURPOSE: Join two edges in order about the given intersection point (the
//          intersection points coincides with an endpoint of each edge).
//
// PARAMETERS:
//
//     Input:
//
//         intersectionPt - passes a reference the intersection point.
//         edge0          - passes a pointer to the first edge to be joined.
//         edge1          - passes a pointer to the second edge to be joined.
//
//     Output:
//
//         edge0, edge1   - are joined at intersectionPt upon return.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void IntersectionProcessor::Join(const OpsDoublePoint &intersectionPt,
    WingedEdge *edge0, WingedEdge *edge1)
{
    if (intersectionPt == *edge0->m_vert[0]) {
        edge0->m_cwEdge[1] = edge1;
        if (intersectionPt == *edge1->m_vert[0])
            edge1->m_ccwEdge[0] = edge0;
        else
            edge1->m_ccwEdge[1] = edge0;
    }
    else {
        edge0->m_cwEdge[0] = edge1;
        if (intersectionPt == *edge1->m_vert[0])
            edge1->m_ccwEdge[0] = edge0;
        else
            edge1->m_ccwEdge[1] = edge0;
    }

} // end: Join()


//------------------------------------------------------------------------------
//
// METHOD: SplitEdge().
//
// PURPOSE: Split an edge at the indicated point. The edge is split so that the
//          intersection point becomes the minimum vertex of the original edge.
//
//          NOTE - During testing it was found that if the split point does not
//                 lie within the bounding box of the edge, topological instab-
//                 ility can result. For example, if the split point lies to the
//                 right of the bounding box (but constrained to be within the
//                 y extents of the box), vert[0] and vert[1] for the second
//                 half of the split edge are not properly oriented, i.e.,
//                 vert[1].x < vert[0].x. Hopefully this situation will not
//                 occur in practice.
//
// PARAMETERS:
//
//     Input:
//
//         splitPt - passes a reference to the point at which the edge is to
//                   be split.
//         edge    - passes a pointer to the edge to be split.
//
//     Output:
//
//         edge    - the edge is split into two at the specified point upon
//                   return.
//         newEdge - reference to a WingedEdge pointer. The referenced pointer
//                   is assigned to point to the new half of the split edge.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to split the edge.
//
//------------------------------------------------------------------------------

void IntersectionProcessor::SplitEdge(const OpsDoublePoint &splitPt,
    WingedEdge *edge, WingedEdge *&newEdge)
{
#if defined _DEBUG
    // make sure the split point lies within the bounding box of the edge

    if (edge->m_vert[0]->x <= edge->m_vert[1]->x)
        assert(edge->m_vert[0]->x - 1.0e-9 <= splitPt.x &&
               edge->m_vert[1]->x + 1.0e-9 >= splitPt.x);
    else
        assert(edge->m_vert[1]->x - 1.0e-9 <= splitPt.x &&
               edge->m_vert[0]->x + 1.0e-9 >= splitPt.x);

    if (edge->m_vert[0]->y <= edge->m_vert[1]->y)
        assert(edge->m_vert[0]->y - 1.0e-9 <= splitPt.y &&
               edge->m_vert[1]->y + 1.0e-9 >= splitPt.y);
    else
        assert(edge->m_vert[1]->y - 1.0e-9 <= splitPt.y &&
               edge->m_vert[0]->y + 1.0e-9 >= splitPt.y);

    // check for two special case where the split may result in incorrect
    // topology. The conditions test for two cases where one of the edges
    // resulting from the split is effectively rotated clockwise to the
    // vertical (it is easier to see this by negating the logical conditions
    // of the assert tests). If either of these cases occurs, then one of
    // the edges resulting from the split will have m_vert[0] and m_vert[1]
    // reversed, and the inside/outside relationships will also be incorrectly
    // assigned. (The reason code has not been added to explicitly handle
    // these two cases is that so far they have never been observed to
    // happen in practice - they could only occur as a result of unusual
    // rounding errors in computed points of intersection)

    if (edge->m_vert[0]->x != edge->m_vert[1]->x &&
        edge->m_vert[0]->y > edge->m_vert[1]->y) {
        assert(edge->m_vert[0]->x != splitPt.x ||
               edge->m_vert[0]->y <= splitPt.y);
        assert(edge->m_vert[1]->x != splitPt.x ||
               edge->m_vert[1]->y <= splitPt.y);
    }

#endif

    // get a pointer to a new WingedEdge and initialize the edge endpoints

    newEdge = m_wingedEdgeAllocator->Allocate();
    newEdge->m_vert[0] = edge->m_vert[0];
    newEdge->m_vert[1] = m_vertexAllocator->Allocate(splitPt.x, splitPt.y);
    edge->m_vert[0] = newEdge->m_vert[1];

    assert(*edge->m_vert[0] != *edge->m_vert[1]);
    assert(*newEdge->m_vert[0] != *newEdge->m_vert[1]);

    // set side-in flags to be the same as the old edge

    if (edge->SideIn(WingedEdge::MinVertSide))
        newEdge->SetSideIn(WingedEdge::MinVertSide);
    if (edge->SideIn(WingedEdge::MaxVertSide))
        newEdge->SetSideIn(WingedEdge::MaxVertSide);

    // patch links to other edges if the original edge does not loop
    // back on itself from vert[0]

    if (edge->m_ccwEdge[0] != edge) {
        edge->m_ccwEdge[0]->ResetWings(edge, newEdge);
        newEdge->m_ccwEdge[0] = edge->m_ccwEdge[0];
        edge->m_ccwEdge[0] = edge;
    }

    if (edge->m_cwEdge[1] != edge) {
        edge->m_cwEdge[1]->ResetWings(edge, newEdge);
        newEdge->m_cwEdge[1] = edge->m_cwEdge[1];
        edge->m_cwEdge[1] = edge;
    }

} // end: SplitEdge()


//------------------------------------------------------------------------------
//
// METHOD: AddEdgeRecord().
//
// PURPOSE: Internal function to create an EdgeRecord for the specified Winged-
//          Edge, and add to the internal array of EdgeRecords.
//
// PARAMETERS:
//
//     Input:
//
//         edge       - passes a pointer to the edge the record is to be
//                      added for.
//         endPtIndex - specifies which endpoint should be translated to the
//                      origin when computing the polar angle of the edge.
//
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to add the edge record.
//
//------------------------------------------------------------------------------

void IntersectionProcessor::AddEdgeRecord(WingedEdge *edge, int endPtIndex)
{
    // reallocate the array of EdgeRecords if necessary

    if (m_nEdgeRecs >= m_maxEdgeRecs) {
        EdgeRecord *temp = new EdgeRecord[m_maxEdgeRecs + EdgeRecTableSize];

        if (m_nEdgeRecs > 0) {
            for (int i = 0; i < m_nEdgeRecs; i++)
                temp[i] = m_edgeRecs[i];
            delete [] m_edgeRecs;
        }

        m_edgeRecs = temp;
        m_maxEdgeRecs += EdgeRecTableSize;
    }

    // compute the polar angle of the edge (in the range 0 .. TwoPi) relative
    // to the specified endpoint

    double deltaX, deltaY;

    if (endPtIndex == 0) {
        deltaX = edge->m_vert[1]->x - edge->m_vert[0]->x;
        deltaY = edge->m_vert[1]->y - edge->m_vert[0]->y;
    }
    else {
        deltaX = edge->m_vert[0]->x - edge->m_vert[1]->x;
        deltaY = edge->m_vert[0]->y - edge->m_vert[1]->y;
    }

    assert(deltaX != 0.0 || deltaY != 0.0);

    m_edgeRecs[m_nEdgeRecs].m_polarAngle = ::atan2(deltaY, deltaX);

    if (m_edgeRecs[m_nEdgeRecs].m_polarAngle < 0.0)
        m_edgeRecs[m_nEdgeRecs].m_polarAngle += TwoPi;

    // add the record to the EdgeRec array

    m_edgeRecs[m_nEdgeRecs++].m_edge = edge;

} // end: AddEdgeRecord()


#if defined _DEBUG

//------------------------------------------------------------------------------
//
// METHOD: TraceIntersectionsTo().
//
// PURPOSE: Set the FILE intersection data is to be traced to. A NULL FILE
//          pointer disables tracing.
//
// PARAMETERS:
//
//     Input:
//
//         trcFile - passes a pointer to the FILE the trace data is to be
//                   written to; a NULL pointer should be passed to disable
//                   tracing.
//
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void IntersectionProcessor::TraceIntersectionsTo(FILE *trcFile)
{
    traceFile = trcFile;

} // end: TraceIntersections()


//------------------------------------------------------------------------------
//
// METHOD: TraceIntersection().
//
// PURPOSE: Trace the data for the most recently processed intersection to
//          to the specified file.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void IntersectionProcessor::TraceIntersection(
    const OpsDoublePoint &intersectionPt) const
{
    ::fprintf(traceFile, "Intersection: %20.12f %20.12f\n", intersectionPt.x,
        intersectionPt.y);

    for (int i = 0; i < m_nEdgeRecs; i++) {
        // write the coordinates of the edge endpoints

        OpsDoublePoint *vert0 = m_edgeRecs[i].m_edge->m_vert[0];
        OpsDoublePoint *vert1 = m_edgeRecs[i].m_edge->m_vert[1];

        ::fprintf(traceFile, "    ");
        ::fprintf(traceFile, "%20.12f, %20.12f, ", vert0->x, vert0->y);
        ::fprintf(traceFile, "%20.12f, %20.12f, ", vert1->x, vert1->y);

        // write out the edge flags - note that very short edges are flagged
        // with an asterisk

        STRING flags;
        const double ShortEdgeLength = 5.0e-9;

        if (BufferUtility::DistBetweenPoints(*vert0, *vert1) <= ShortEdgeLength)
            flags += '*';

        if (m_edgeRecs[i].m_edge->EdgeDeleted())
            flags += 'D';

        if (m_edgeRecs[i].m_edge->SideIn(WingedEdge::MaxVertSide))
            flags += 'L';

        if (m_edgeRecs[i].m_edge->SideIn(WingedEdge::MinVertSide))
            flags += 'R';

        ::fprintf(traceFile, "%s\n", MgUtil::WideCharToMultiByte(flags).c_str());
    }

} // end: TraceIntersection()

#endif
