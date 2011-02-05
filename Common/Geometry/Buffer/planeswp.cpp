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
// FILE: planeswp.cpp.
//
// PURPOSE: Implementation of the PlaneSweep object. The implementation is based
//          on concepts introduced in the following papers:
//
//     Meyers, E., W., "An O(E log E + I) Expected Time Algorithm for the
//     Planar Segment Intersection Problem", Siam J. Comput., 14(3), 1985,
//     625-637.
//
//     Weiler, K, "Polygon Comparison using a Graph Representation", ACM
//     SIGGRAPH Conference Proceedings, 1980, 10 -18.
//
//     Milenkovic, V, "Double Precision Geometry: A General Technique for
//     Calculating Line and Segment Intersections Using Rounded Arithmetic",
//     30th Annual Symposium on the Foundations of Computer Science, IEEE,
//     1989, 500 - 505.
//
//------------------------------------------------------------------------------

#include "Foundation.h"
#include "buffer.h"
#include "interlst.h"

#if defined PERF_DUMPFFGF
#include "DumpFFGF.h"
#endif

const int PlaneSweep::ItemsPerProgressSubInterval = 128;
const int PlaneSweep::BoundaryArraySize = 16384;

int PlaneSweep::m_currentFile = 0;

SortMethod PlaneSweep::sortMethod = QUICKSORT;

#if defined(_DEBUG)
BOOL PlaneSweep::traceBoundaryTraversal = FALSE;
STRING PlaneSweep::boundaryTraversalTraceFileName;
FILE *PlaneSweep::boundaryTraversalTraceFile = NULL;
#endif

//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize an instance of the PlaneSweep object.
//
// PARAMETERS:
//
//     None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to initialize the PlaneSweep object.
//
//------------------------------------------------------------------------------

PlaneSweep::PlaneSweep() :
    m_vertexAlloc(NULL),
    m_wingedEdgeAlloc(NULL),
    m_sweepEdgeAlloc(NULL),
    m_tupleArray(NULL),
    m_nBoundaryVertices(0),
    m_boundaryVertices(NULL),
    m_boundaryArraySize(0)
{
    // create and initialize required data structures

    MG_TRY()

    InitializeMembers();

    MG_CATCH(L"PlaneSweep::PlaneSweep")

    if (mgException != 0) // mgException is defined in MG_TRY() macro
    {
        Cleanup();
    }

    MG_THROW()

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

PlaneSweep::~PlaneSweep()
{
    Cleanup();

} // end: destructor


//------------------------------------------------------------------------------
//
// METHOD: SetSortMethod().
//
// PURPOSE: Set the method that should be used to sort the sweep tuple array
//          during the next call to DoPlaneSweep().
//
//
// PARAMETERS:
//
//     Input:
//
//         method - passes an enumerated value of type SortMethod specifying
//                  the sort method to use.
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

void PlaneSweep::SetSortMethod(SortMethod method)
{
    sortMethod = method;

} // end: SetSortMethod()


#if defined _DEBUG

//------------------------------------------------------------------------------
//
// METHOD: EnableBoundaryTraversalTrace().
//
// PURPOSE: Enable/disable tracing of output boundaries.
//
//
// PARAMETERS:
//
//     Input:
//
//         enableTrace - passes TRUE to enable tracing, and FALSE to disable it.
//         traceFile   - passes the name of the file the trace data is to be
//                       written to. The trace data is appended to the file.
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

void PlaneSweep::EnableBoundaryTraversalTrace(BOOL enableTrace,
    STRING traceFile)
{
    traceBoundaryTraversal = enableTrace;

    if (traceBoundaryTraversal) {
        assert(!traceFile.empty());
        boundaryTraversalTraceFileName = traceFile;
    }

} // end: EnableBoundaryTraversalTrace()

#endif

//------------------------------------------------------------------------------
//
// METHOD: Initialize().
//
// PURPOSE: Initialize (or re-initialize) the PlaneSweep object. This method
//          should be called in between calls to DoPlaneSweep().
//
//
// PARAMETERS:
//
//     None.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to initialize the object.
//
//------------------------------------------------------------------------------

void PlaneSweep::Initialize()
{
    // cleanup and then reinitialize members

    Cleanup();
    InitializeMembers();

} // end: Initialize()

//------------------------------------------------------------------------------
//
// METHOD: AddEdges().
//
// PURPOSE: Add a sequences of edges that are to be swept over in a later call
//          to DoPlaneSweep().
//
// PARAMETERS:
//
//     Input:
//
//         vertices     - passes an array of edge vertices.
//         nVertices    - passes the number of vertices in the sequence (the
//                        number of edges = nVertices - 1).
//         interiorSide - passes an enumerated value of type PlaneSweep::-
//                        InteriorSide specifying whether the interior of the
//                        polygonal region lies to the right or left of the
//                        edges in the sequence.
//
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to add the edges.
//
//------------------------------------------------------------------------------

void PlaneSweep::AddEdges(const OpsFloatPoint vertices[], int nVertices,
    InteriorSide side)
{
    // if the interior is to the left, then add sweep edges starting at the
    // beginning of the sequence of vertices

    OpsDoublePoint *verts[2];

    if (side == Left) {
        verts[0] = m_vertexAlloc->Allocate(vertices[0].x, vertices[0].y);

        for (int i = 1; i < nVertices; i++) {
            verts[1] = m_vertexAlloc->Allocate(vertices[i].x, vertices[i].y);
            AddEdge(verts);
            verts[0] = verts[1];
        }
    }

    // else walk backward through the sequence of vertices, so as to maintain
    // the interior to the left of the edges

    else {
        verts[0] = m_vertexAlloc->Allocate(vertices[nVertices-1].x,
            vertices[nVertices-1].y);

        for (int i = nVertices - 2; i >= 0; i--) {
            verts[1] = m_vertexAlloc->Allocate(vertices[i].x, vertices[i].y);
            AddEdge(verts);
            verts[0] = verts[1];
        }
    }

} // end: AddEdges()


//------------------------------------------------------------------------------
//
// METHOD: DoPlaneSweep().
//
// PURPOSE: Perform the plane sweep over the edges that have been added in prior
//          calls to AddEdges(). The edges are split, and joined at detected
//          points of intersection to form a graph. The algorithm used is essen-
//          tially the following three step process:
//
//          1. Perform Meyer's plane sweep to find all intersections between
//             edges.
//          2. Process the intersections to form the graph of WingedEdges.
//          3. Traverse closed loops in the graph to extract the boundaries
//             of the output polygon.
//
// PARAMETERS:
//
//     Input:
//
//          callback      - passes a pointer to a ProgressCallback object. The
//                          object is periodically notified of progress, and
//                          checked to determine if the plane sweep operation
//                          has been canceled.
//
//     Output:
//
//          outputPolygon - passes a reference to an OrientedPolyPolygon object.
//                          The closed loops in the graph of edges are added
//                          to the object as polygon boundaries.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to peform the plane sweep operation. A PlaneSweep-
//             Exception is thrown if an error is detected during the execution
//             of the plane sweep algorithm.
//
//------------------------------------------------------------------------------

void PlaneSweep::DoPlaneSweep(ProgressCallback &callback, FloatTransform *pTransform,
    OrientedPolyPolygon &outputPolygon)
{
    m_Transform = pTransform;

    DoPlaneSweep(callback, outputPolygon);
}

void PlaneSweep::DoPlaneSweep(ProgressCallback &callback,
    OrientedPolyPolygon &outputPolygon)
{
    // check if there is anything to do

    if (m_nEdgesAdded == 0)
        return;

    // setup to report progress over four subintervals - sorting, plane
    // sweep, processing intersections, and traversing candidate output
    // boundaries

    callback.BeginProgressInterval(4);

    // sort sweep tuples

    m_tupleArray->Sort(callback, sortMethod);
    if (callback.IsCanceled())
        return;

    // initialize pointers to data structures

    EventIntervalArray *eventIntArray = NULL;
    WorkListArray *workListArray = NULL;
    XOrderedTree *xOrdTree = NULL;
    IntersectionList *intersectList = NULL;

    MG_TRY()

    // determine the event intervals, and initialize the worklist array

    eventIntArray = new EventIntervalArray(*m_tupleArray);
    int nEventIntervals = eventIntArray->GetNEventIntervals();
    workListArray = new WorkListArray(nEventIntervals);

    // begin progress interval for the sweep part

    int nSubIntervals = GetNProgressSubIntervals(nEventIntervals);
    callback.BeginProgressInterval(nSubIntervals);

    // create the X-Ordered tree, and the intersection list

    xOrdTree = new XOrderedTree();
    intersectList = new IntersectionList(m_vertexAlloc, m_wingedEdgeAlloc, m_Transform);

    // process the event intervals to determine intersections

    for (int i = 0; i < nEventIntervals; i++) {
        // get a pointer to the next event interval

        const EventInterval &eventInterval = (*eventIntArray)[i];

        // set abscissa on the XOrderedTree

        xOrdTree->SetAbscissa(eventInterval.Abscissa());

        // add segments in BEG(i) and collect their start point
        // intersections

        SweepEdge *e;
        int j = 0;
        for (j = 0; j < eventInterval.BeginLength(); j++) {
            SweepEdge *f;

            e = eventInterval.Begin(j);
            xOrdTree->Insert(e);
            f = xOrdTree->Below(e);

            if (f != NULL) {
                workListArray->Remove(f);
                if (f->XOrder(e, BufferUtility::Min(f->MaxVert()->x, e->MaxVert()->x)) > 0)
                    workListArray->Push(f, GetIntersectionInterval(f, e, eventIntArray));
            }

            f = xOrdTree->Above(e);

            if (f && e->XOrder(f, BufferUtility::Min(e->MaxVert()->x, f->MaxVert()->x)) > 0)
                workListArray->Push(e, GetIntersectionInterval(e, f, eventIntArray));

            // now find all start point intersections and add to
            // intersection list

            FindStartPointIntersections(e, xOrdTree, intersectList);
        }

        // find all intersections with segments in VERT(i)

        for (j = 0; j < eventInterval.VertLength(); j++) {
            e = eventInterval.Vert(j);
            xOrdTree->Insert(e);
            FindVerticalIntersections(e, xOrdTree, intersectList);
        }

        // delete all segments in VERT(i) from the XOrderedTree

        for (j = 0; j < eventInterval.VertLength(); j++)
            xOrdTree->Delete(eventInterval.Vert(j));

        // Delete all segments in END(i)

        for (j = 0; j < eventInterval.EndLength(); j++) {
            SweepEdge *f;

            e = eventInterval.End(j);
            f = xOrdTree->Below(e);
            xOrdTree->Delete(e);
            workListArray->Remove(e);

            if (f != NULL) {
                SweepEdge *g;

                workListArray->Remove(f);
                g = xOrdTree->Above(f);
                if (g && f->XOrder(g, BufferUtility::Min(f->MaxVert()->x, g->MaxVert()->x)) > 0)
                    workListArray->Push(f, GetIntersectionInterval(f, g, eventIntArray));
            }
        }

        // Find all event exchange intersections in the current interval

        e = workListArray->Pop(i);

        while (e != NULL) {
            OpsDoublePoint intersectionPt;
            SweepEdge *f, *g;

            f = xOrdTree->Above(e);
            GetMidpointIntersection(e, f, intersectionPt);
            intersectList->Add(intersectionPt, e->GetWingedEdge(),
                f->GetWingedEdge());

            f = xOrdTree->Below(e);
            xOrdTree->Exchange(e);

            workListArray->Remove(xOrdTree->Below(e));

            if (f != NULL) {
                workListArray->Remove(f);
                g = xOrdTree->Above(f);
                if (g && f->XOrder(g, BufferUtility::Min(f->MaxVert()->x, g->MaxVert()->x)) > 0)
                    workListArray->Push(f, GetIntersectionInterval(f, g, eventIntArray));
            }

            g = xOrdTree->Above(e);
            if (g && e->XOrder(g, BufferUtility::Min(e->MaxVert()->x, g->MaxVert()->x)) > 0)
                workListArray->Push(e, GetIntersectionInterval(e, g, eventIntArray));

            e = workListArray->Pop(i);
        }

        // check if plane sweep operation has been canceled

        if (callback.IsCanceled())
            break;

        // update progress if necessary

        if (i % ItemsPerProgressSubInterval == 0 && i > 0)
            callback.AdvanceOneSubInterval();
    }

    // clean up data structures that are no longer required, and the
    // progress interval for the sweep part

    delete xOrdTree;
    delete workListArray;
    delete eventIntArray;
    callback.EndProgressInterval();

    MG_CATCH(L"PlaneSweep::DoPlaneSweep")

    if (mgException != 0) // mgException is defined in MG_TRY() macro
    {
        delete xOrdTree;
        delete workListArray;
        delete eventIntArray;
        delete intersectList;
    }

    MG_THROW()

    // process all intersections found to generate the graph of WingedEdges

    if (!callback.IsCanceled())
    {
        MG_TRY()

//      intersectList->Dump2FFGF( m_Transform );

        intersectList->ProcessIntersections(callback);

        MG_CATCH(L"...PlaneSweep::DoPlaneSweep")

        if (mgException != 0) // mgException is defined in MG_TRY() macro
        {
            delete intersectList;
        }

        MG_THROW()
    }

    delete intersectList;

    // extract the output poly-polygon from the graph of WingedEdges

    if (!callback.IsCanceled())
    {
        try
        {
            GetOutputPolygon(callback, outputPolygon);
        }
        catch(PlaneSweepException* ex)
        {
            // Clean up. The progress intervals are limited.
            callback.EndProgressInterval();
            throw(ex);
        }
    }

    // end the progress interval started above

    callback.EndProgressInterval();

} // end: DoPlaneSweep()


//------------------------------------------------------------------------------
//
// METHOD: InitializeMembers().
//
// PURPOSE: Initialize member variables.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to initialize the member variables.
//
//------------------------------------------------------------------------------

void PlaneSweep::InitializeMembers()
{
    m_nEdgesAdded = 0;
    m_vertexAlloc = new VertexAllocator;
    m_wingedEdgeAlloc = new WingedEdgeAllocator;
    m_sweepEdgeAlloc = new SweepEdgeAllocator;
    m_tupleArray = new SweepTupleArray;
    m_nBoundaryVertices = 0;
    m_boundaryArraySize = 0;
    m_Transform = 0;

} // end: InitializeMembers()


//------------------------------------------------------------------------------
//
// METHOD: Cleanup().
//
// PURPOSE: Delete all dynamically allocated resources.
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

void PlaneSweep::Cleanup()
{
    delete m_vertexAlloc;
    m_vertexAlloc = NULL;

    delete m_wingedEdgeAlloc;
    m_wingedEdgeAlloc = NULL;

    delete m_sweepEdgeAlloc;
    m_sweepEdgeAlloc = NULL;

    delete m_tupleArray;
    m_tupleArray = NULL;

    delete [] m_boundaryVertices;
    m_boundaryVertices = NULL;

} // end: Cleanup()


//------------------------------------------------------------------------------
//
// METHOD: AddEdge().
//
// PURPOSE: Generate the required WingedEdge, and SweepEdge data structures
//          corresponding to the specified edge; add the the sweep tuples
//          corresponding to the sweep edge to the SweepTupleArray. NOTE that
//          degenerate edges are rejected (this is required to prevent floating
//          point errors and other bizarre behaviour).
//
// PARAMETERS:
//
//     Input:
//
//         vertices - passes an array of pointers to two OpsDoublePoint's. The
//                    OpsDoublePoint's pointed to contain the coordinates of the
//                    endpoints of the edge.
//
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to generate the data structures corresponding to the
//             edge.
//
//------------------------------------------------------------------------------

void PlaneSweep::AddEdge(OpsDoublePoint *vertices[2])
{
    if (*vertices[0] != *vertices[1])
    {
        WingedEdge *wEdge = m_wingedEdgeAlloc->Allocate(vertices);
        m_tupleArray->AddSweepTuples(m_sweepEdgeAlloc->Allocate(wEdge));
        m_nEdgesAdded++;
    }

} // end: AddEdge()


//------------------------------------------------------------------------------
//
// METHOD: FindStartPointIntersections().
//
// PURPOSE: Find all edges in the current X-Order that intersect the specified
//          edge at its start point.
//
// PARAMETERS:
//
//     Input:
//
//         edge          - passes a pointer to the SweepEdge the start point
//                         intersections are to be determined for.
//         xOrdTree      - passes a pointer to the XOrderedTree object defining
//                         the current X-Ordering of edges.
//
//     Output:
//
//         intersectList - passes a pointer to an IntersectionList object. All
//                         start point intersections (if any) are added to the
//                         object pointed to.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to add the start point intersections to the intersect-
//             ion list.
//
//------------------------------------------------------------------------------

void PlaneSweep::FindStartPointIntersections(const SweepEdge *edge,
    const XOrderedTree *xOrdTree, IntersectionList *intersectList) const
{
    // the edge must not be vertical

    assert(edge->MinVert()->x != edge->MaxVert()->x);

    // get the x and y coordinates of the start point

    double x = edge->MinVert()->x;
    double y = edge->MinVert()->y;

    // walk down the X-Order looking for intersecting edges

    double ordinate;
    SweepEdge *edge2 = xOrdTree->Below(edge);
    while (edge2 != NULL)
    {
        ordinate = edge2->Ordinate(x);
        if (y != ordinate)
            break;
        AddStartPointIntersections(edge, edge2, intersectList);
        edge2 = xOrdTree->Below(edge2);
    }

    // walk up the X-Order looking for intersecting edges

    edge2 = xOrdTree->Above(edge);
    while (edge2 != NULL)
    {
        ordinate = edge2->Ordinate(x);
        if (y != ordinate)
            break;
        AddStartPointIntersections(edge, edge2, intersectList);
        edge2 = xOrdTree->Above(edge2);
    }

} // end: FindStartPointIntersections()


//------------------------------------------------------------------------------
//
// METHOD: AddStartPointIntersections().
//
// PURPOSE: Add the points of intersection between edge1 and edge2 to the inter-
//          section list; the start point of edge1 is a definite intersection
//          point, but it is also necessary to test for the case where edge1 and
//          edge2 overlap.
//
// PARAMETERS:
//
//     Input:
//
//          edge1        - passes a pointer to the SweepEdge containing the
//                         start point.
//          edge2        - passes a pointer to the edge intersecting edge1 at
//                         its start point.
//
//     Output:
//
//         intersectList - passes a pointer to an IntersectionList object. The
//                         interesections between edge1 and edge2 are added to
//                         the list.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to add the start point intersections.
//
//------------------------------------------------------------------------------

void PlaneSweep::AddStartPointIntersections(const SweepEdge *edge1,
    const SweepEdge *edge2, IntersectionList *intersectList) const
{
    // Add the start point intersection

    OpsDoublePoint *intersectPt = edge1->MinVert();

    if ( !intersectList->Add(*intersectPt, edge1->GetWingedEdge(),
        edge2->GetWingedEdge()))
        return;

    // if the edges have the same slope, and the start point of edge1 is not
    // the end point of edge2 then the two edges overlap - add the appropriate
    // edge endpoint to the intersection list

    if (*intersectPt != *edge2->MaxVert() && edge1->Slope() == edge2->Slope()) {
        double xMax1 = edge1->MaxVert()->x;
        double xMax2 = edge2->MaxVert()->x;

        if (xMax1 <= xMax2)
            intersectPt = edge1->MaxVert();
        else
            intersectPt = edge2->MaxVert();

        intersectList->Add(*intersectPt, edge1->GetWingedEdge(),
            edge2->GetWingedEdge());
    }

} // end: AddStartPointIntersections()


//------------------------------------------------------------------------------
//
// METHOD: FindVerticalIntersections().
//
// PURPOSE: Find all edges in the current X-Order that intersect the specified
//          vertical edge.
//
// PARAMETERS:
//
//     Input:
//
//         vertEdge      - passes a pointer to the vertical SweepEdge.
//         xOrdTree      - passes a pointer to the XOrderedTree object defining
//                         the current X-Ordering of edges.
//
//     Output:
//
//         intersectList - passes a pointer to an IntersectionList object. All
//                         intersections with the specified vertical edge (if
//                         any) are added to the object pointed to.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to add the vertical intersections to the intersection
//             list.
//
//------------------------------------------------------------------------------

void PlaneSweep::FindVerticalIntersections(const SweepEdge *vertEdge,
    const XOrderedTree *xOrdTree, IntersectionList *intersectList) const
{
    // the edge must be vertical

    assert(vertEdge->MinVert()->x == vertEdge->MaxVert()->x);

    // get the x coordinate of the vertical edge, and it min/max y coordinates

    double x = vertEdge->MinVert()->x;
    double yMin = vertEdge->MinVert()->y;
    double yMax = vertEdge->MaxVert()->y;

    // walk down the X-Order to find looking for intersections

    double ordinate;
    SweepEdge *edge2 = xOrdTree->Below(vertEdge);
    while (edge2 != NULL) {
        ordinate = edge2->Ordinate(x);
        if (ordinate < yMin || ordinate > yMax)
            break;
        AddVerticalIntersections(vertEdge, edge2, intersectList);
        edge2 = xOrdTree->Below(edge2);
    }

    // walk up the X-Order to find looking for intersections

    edge2 = xOrdTree->Above(vertEdge);
    while (edge2 != NULL) {
        ordinate = edge2->Ordinate(x);
        if (ordinate < yMin || ordinate > yMax)
            break;
        AddVerticalIntersections(vertEdge, edge2, intersectList);
        edge2 = xOrdTree->Above(edge2);
    }

} // end: FindVerticalIntersections()


//------------------------------------------------------------------------------
//
// METHOD: AddVerticalIntersections().
//
// PURPOSE: Add the points of intersection between the specified vertical edge,
//          and the specified edge.
//
// PARAMETERS:
//
//     Input:
//
//         vertEdge      - passes a pointer to the vertical SweepEdge.
//         edge2         - passes a pointer to the SweepEdge that intersects
//                         the vertical edge.
//
//     Output:
//
//         intersectList - passes a pointer to an IntersectionList object. The
//                         intersections between the two edges are added to
//                         the intersection list object pointed to.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to add the intersections.
//
//------------------------------------------------------------------------------

void PlaneSweep::AddVerticalIntersections(const SweepEdge *vertEdge,
    const SweepEdge *edge2, IntersectionList *intersectList) const
{
    // get pointers to the underlying WingedEdges

    WingedEdge *wEdge1 = vertEdge->GetWingedEdge();
    WingedEdge *wEdge2 = edge2->GetWingedEdge();

    // if second edge is not vertical then there will only be a single point
    // of intersection

    if (vertEdge->Slope() != edge2->Slope()) {
        // check for case where the second edge intersects the vertical edge
        // at its minimum vertex

        if (vertEdge->MinVert()->x == edge2->MinVert()->x)
            intersectList->Add(*edge2->MinVert(), wEdge1, wEdge2);

        // check for case where the second edge intersects the vertical edge
        // at its maximum vertex

        else if (vertEdge->MinVert()->x == edge2->MaxVert()->x)
            intersectList->Add(*edge2->MaxVert(), wEdge1, wEdge2);

        // else compute the point where the second edge crosses the vertical
        // edge

        else {
            OpsDoublePoint intersectPt;

            intersectPt.x = vertEdge->MinVert()->x;
            intersectPt.y = edge2->Ordinate(vertEdge->MinVert()->x);
            intersectList->Add(intersectPt, wEdge1, wEdge2);
        }
    }

    // else the two edges are vertical and may overlap

    else {
        // check for case where second edge terminates at the minimum
        // vertex of the vertical edge

        if (*vertEdge->MinVert() == *edge2->MaxVert())
            intersectList->Add(*vertEdge->MinVert(), wEdge1, wEdge2);

        // check for case where second edge starts at the maximum
        // vertex of the vertical edge

        else if (*vertEdge->MaxVert() == *edge2->MinVert())
            intersectList->Add(*vertEdge->MaxVert(), wEdge1, wEdge2);

        // else the two edges must overlap - determine the two points of
        // intersection and add to them to the intersection list

        else {
            if (vertEdge->MinVert()->y <= edge2->MinVert()->y)
                intersectList->Add(*edge2->MinVert(), wEdge1, wEdge2);
            else
                intersectList->Add(*vertEdge->MaxVert(), wEdge1, wEdge2);

            if (vertEdge->MaxVert()->y <= edge2->MaxVert()->y)
                intersectList->Add(*vertEdge->MaxVert(), wEdge1, wEdge2);
            else
                intersectList->Add(*edge2->MaxVert(), wEdge1, wEdge2);
        }
    }

} // end: AddVerticalIntersections()


//------------------------------------------------------------------------------
//
// METHOD: GetIntersectionInterval().
//
// PURPOSE: Determine the event interval in which the two specified edges inter-
//          sect; the point of intersection is assumed to be unique (i.e., the
//          two edges cross at the point of intersection).
//
// PARAMETERS:
//
//     Input:
//
//         edge1         - passes a pointer to the first of the two intersect-
//                         ing edges.
//         edge2         - passes a pointer to the second of the two intersect-
//                         ing edges.
//         eventIntArray - passes a pointer to the EventIntervalArray object to
//                         query for the interval containing the point of inter-
//                         section
//
//     Output:
//
//         None.
//
// RETURNS: An integer specifying the event interval containing the point
//          where the two edges intersect.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

int PlaneSweep::GetIntersectionInterval(const SweepEdge *edge1,
    const SweepEdge *edge2, const EventIntervalArray *eventIntArray) const
{
    // call GetMidpointIntersection() to compute the point where the two lines
    // intersect - this means a little extra work computing the y coordinate of
    // the intersection, but it also means that a uniform mechanism is used to
    // compute interesections; this may need to be reconsidered if it turns out
    // to be a performance problem

    OpsDoublePoint intersectionPt;
    GetMidpointIntersection(edge1, edge2, intersectionPt);

    // get the index of the interval containing the x coordinate of the
    // point of intersection

    return eventIntArray->GetContainingInterval(intersectionPt.x);

} // end: GetIntersectionInterval()


//------------------------------------------------------------------------------
//
// METHOD: GetMidpointInteresection().
//
// PURPOSE: Compute the point where the specified edges intersect (the edges are
//          assumed to intersect at precisely one point). The algorithm used to
//          compute the intersection is adapted from the NCGIA line intersection
//          algorithm as described in the "Back to Basics" column of the Dec.
//          1996 issue of GISEurope.
//
// PARAMETERS:
//
//     Input:
//
//         edge1          - passes a pointer to the first of the two intersect-
//                          ing edges.
//         edge2          - passes a pointer to the second of the two intersect-
//                          ing edges.
//
//     Output:
//
//         intersectionPt - passes a reference to a OpsDoublePoint. The coord-
//                          inatesof the intersection point are copied to the
//                          referenced object.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void PlaneSweep::GetMidpointIntersection(const SweepEdge *edge1,
    const SweepEdge *edge2, OpsDoublePoint &intersectionPt) const
{
    // get pointers to the maximum vertices of the two edges

    OpsDoublePoint *pt2 = edge1->MaxVert();
    OpsDoublePoint *pt4 = edge2->MaxVert();

    // check for special case where the two edges intersect at their maximum
    // vertices - this is treated as a special case, so as to avoid any inacc-
    // uracies that might result from computing the end point intersection

    if (*pt2 == *pt4)
        intersectionPt = *pt2;

    // else compute the point of intersection

    else {
        OpsDoublePoint *pt1 = edge1->MinVert();
        OpsDoublePoint *pt3 = edge2->MinVert();

        // first edge vertical and second one not vertical ?

        if (pt1->x == pt2->x) {
            intersectionPt.x = pt1->x;
            double b = (pt4->y - pt3->y) / (pt4->x - pt3->x);
            double a = pt3->y - b * pt3->x;
            intersectionPt.y = a + b * intersectionPt.x;
        }

        // else second edge vertical and first one not vertical ?

        else if (pt3->x == pt4->x) {
            intersectionPt.x = pt3->x;
            double b = (pt2->y - pt1->y) / (pt2->x - pt1->x);
            double a = pt1->y - b * pt1->x;
            intersectionPt.y = a + b * intersectionPt.x;
        }

        // else neither edge is vertical

        else {
            double b1 = (pt2->y - pt1->y) / (pt2->x - pt1->x);
            double b2 = (pt4->y - pt3->y) / (pt4->x - pt3->x);
            double a1 = pt1->y - b1 * pt1->x;
            double a2 = pt3->y - b2 * pt3->x;
            intersectionPt.x = (a2 - a1) / (b1 - b2);
            intersectionPt.y = a1 + b1 * intersectionPt.x;
        }

        // if the point of intersection is very close to one of the edge
        // endpoints, then snap it to the endpoint - this helps prevent
        // degenerate topology from being formed, particularly in cases
        // where buffers are being generated for degenerate polylines that
        // double back and forth on themselves several times (this seems to
        // be the case for some polylines in the US interstates layer - the
        // filter algorithm must have gotten a little carried away!)

        const double SnapTolerance = 2.5e-17;

        if (BufferUtility::DistBetweenPointsSqrd(intersectionPt, *pt1) <
                SnapTolerance)
            intersectionPt = *pt1;
        else if (BufferUtility::DistBetweenPointsSqrd(intersectionPt, *pt2) <
                SnapTolerance)
            intersectionPt = *pt2;
        else if (BufferUtility::DistBetweenPointsSqrd(intersectionPt, *pt3) <
                SnapTolerance)
            intersectionPt = *pt3;
        else if (BufferUtility::DistBetweenPointsSqrd(intersectionPt, *pt4) <
                 SnapTolerance)
            intersectionPt = *pt4;

        // else make sure that the point of intersection is constrained to lie
        // within the intersection of the bounding boxes of the intersecting
        // edges

        else
            ClampIntersection(pt1, pt2, pt3, pt4, intersectionPt);
    }

} // end: GetMidpointIntersection()


//------------------------------------------------------------------------------
//
// METHOD: ClampIntersection().
//
// PURPOSE: Constrain the coordinates of a computed point of intersection to
//          lie within the intersection of the bounding boxes of the intersect-
//          ing edges. Constraining points of intersection in this manner seems
//          to help promote topological stability in certain instances.
//
// PARAMETERS:
//
//     Input:
//
//         minVert1       - passes a pointer to a OpsDoublePoint containing the
//                          coordinates of the minimum vertex of the first
//                          edge.
//         maxVert1       - passes a pointer to a OpsDoublePoint containing the
//                          coordinates of the maximum vertex of the first
//                          edge.
//         minVert2       - passes a pointer to a OpsDoublePoint containing the
//                          coordinates of the minimum vertex of the second
//                          edge.
//         maxVert2       - passes a pointer to a OpsDoublePoint containing the
//                          coordinates of the maximum vertex of the second
//                          edge.
//
//     Output:
//
//         intersectionPt - passes a reference to a OpsDoublePoint containing
//                          the coordinates of the point of intersection. The
//                          pointis constrained to the intersection of the
//                          bounding boxes of the edges on return.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void PlaneSweep::ClampIntersection(const OpsDoublePoint *minVert1,
    const OpsDoublePoint *maxVert1, const OpsDoublePoint *minVert2,
    const OpsDoublePoint *maxVert2, OpsDoublePoint &intersectionPt) const
{
    // constrain in x

    double xLimit = BufferUtility::Max(minVert1->x, minVert2->x);

    if (intersectionPt.x < xLimit)
        intersectionPt.x = xLimit;

    xLimit = BufferUtility::Min(maxVert1->x, maxVert2->x);

    if (intersectionPt.x > xLimit)
        intersectionPt.x = xLimit;

    // constrain in y

    double yMin1, yMax1;

    if (minVert1->y <= maxVert1->y) {
        yMin1 = minVert1->y;
        yMax1 = maxVert1->y;
    }
    else {
        yMin1 = maxVert1->y;
        yMax1 = minVert1->y;
    }

    double yMin2, yMax2;

    if (minVert2->y <= maxVert2->y) {
        yMin2 = minVert2->y;
        yMax2 = maxVert2->y;
    }
    else {
        yMin2 = maxVert2->y;
        yMax2 = minVert2->y;
    }

    double yLimit = BufferUtility::Max(yMin1, yMin2);

    if (intersectionPt.y < yLimit)
        intersectionPt.y = yLimit;

    yLimit = BufferUtility::Min(yMax1, yMax2);

    if (intersectionPt.y > yLimit)
        intersectionPt.y = yLimit;

} // end: ClampIntersection()


//------------------------------------------------------------------------------
//
// METHOD: GetOutputPolygon().
//
// PURPOSE: Extract the output polygon by traversing the graph of WingedEdges
//          formed during the plane sweep operation.
//
// PARAMETERS:
//
//     Input:
//
//         callback      - passes a pointer to a ProgressCallback object. The
//                         object pointed to is periodically queried to deter-
//                         mine whether the operation has been canceled, and to
//                         report progress.
//
//     Output:
//
//         outputPolygon - passes a reference to an OrientedPolyPolygon object.
//                         The vertices of the polygon boundaries in the output
//                         are added to the referenced object.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to construct the output polygon.
//             A PlaneSweepException is thrown if the resulting output polygon
//             has zero boundaries.
//
//------------------------------------------------------------------------------

void PlaneSweep::GetOutputPolygon(ProgressCallback &callback,
    OrientedPolyPolygon &outputPolygon)
{
    // open boundary traversal trace file if necessary (debug mode only)

#if defined _DEBUG
    if (traceBoundaryTraversal) {
        boundaryTraversalTraceFile =
            ::fopen(MgUtil::WideCharToMultiByte(boundaryTraversalTraceFileName).c_str(), "a");

        assert(boundaryTraversalTraceFile != NULL);

        ::fprintf(boundaryTraversalTraceFile,
            "\n**** Getting output polygon ****\n");
    }
#endif

    // loop through the edges allocated by the WingedEdgeAllocator. The edges
    // are traversed so that the polygon interior is to the left. This ensures
    // that exterior polygon boundaries are traced out counter-clockwise, while
    // interior (hole) boundaries are traced out clockwise

    int totalEdges = m_wingedEdgeAlloc->GetNObjects();

    // start a progress interval for the traversal phase

    callback.BeginProgressInterval(GetNProgressSubIntervals(totalEdges));

#if defined PERF_DUMPFFGF
    FILE *ffgfFile = MgDumpFFGF::createFile( "wingedEdges_after_correction", PlaneSweep::m_currentFile++, "" );
    for (int i = 0; i < totalEdges; i++)
    {
        WingedEdge *wEdge = (*m_wingedEdgeAlloc)[i];
        if (!wEdge->EdgeDeleted())
            MgDumpFFGF::writeFile( ffgfFile, m_Transform, i, (float)wEdge->m_vert[0]->x, (float)wEdge->m_vert[0]->y,
                                                              (float)wEdge->m_vert[1]->x, (float)wEdge->m_vert[1]->y);
    }
    MgDumpFFGF::closeFile(ffgfFile);
#endif

    for (int i = 0; i < totalEdges; i++) {
        // check if a cancelation has been signalled

        if (callback.IsCanceled())
            break;

        // get a pointer to the next WingedEdge object

        WingedEdge *wEdge = (*m_wingedEdgeAlloc)[i];

        // ignore if the edge has been marked as deleted

        if (!wEdge->EdgeDeleted()) {
            // if the minimum vertex side is marked as interior, then just mark
            // the edge as having been visited

            WingedEdge::EdgeSide edgeSide = WingedEdge::MinVertSide;

            if (wEdge->SideIn(edgeSide))
                wEdge->SetSideVisited(edgeSide);

            // else have a potential output boundary - call TraverseBoundary()
            // to determine whether this is the case, and add the boundary to
            // the output polygon if so

            else if (!wEdge->SideVisited(edgeSide)) {
                OpsFloatExtent extent;

                if (TraverseBoundary(wEdge, edgeSide, extent)) {
                    outputPolygon.AddBoundary(m_boundaryVertices,
                        m_nBoundaryVertices, extent);
                }
            }

            // do the same with the maximum vertex side of the edge

            edgeSide = WingedEdge::MaxVertSide;

            if (wEdge->SideIn(edgeSide))
                wEdge->SetSideVisited(edgeSide);
            else if (!wEdge->SideVisited(edgeSide)) {
                OpsFloatExtent extent;

                if (TraverseBoundary(wEdge, edgeSide, extent)) {
                    outputPolygon.AddBoundary(m_boundaryVertices,
                        m_nBoundaryVertices, extent);
                }
            }
        }

        // update progress if necessary

        if (i % ItemsPerProgressSubInterval == 0 && i > 0)
            callback.AdvanceOneSubInterval();
    }

    // end the current progress interval

    callback.EndProgressInterval();

#if defined _DEBUG
    if (boundaryTraversalTraceFile != NULL) {
        ::fclose(boundaryTraversalTraceFile);
        boundaryTraversalTraceFile = NULL;
    }
#endif

    // if no points in the output polygon and no cancelation was signalled,
    // then some sort of topological error must have occurred - throw a Plane-
    // SweepException to indicate the error
    // NOTE: The caller will ignore this error in the case of negative offset.

    if (outputPolygon.GetNBoundaries() == 0 && !callback.IsCanceled())
        throw new PlaneSweepException(PlaneSweepException::TopologicalError);

} // end: GetOutputPolyPolygon()


//------------------------------------------------------------------------------
//
// METHOD: TraverseBoundary().
//
// PURPOSE: Traverse a (potential) polygon boundary starting at the specified
//          WingedEdge and EdgeSide. The boundary vertices are copied to the
//          m_boundaryVertices member, and the number of vertices is copied to
//          the m_nBoundaryVertices member.
//
// PARAMETERS:
//
//     Input:
//
//         wEdge          - passes a pointer to the WingedEdge to start the
//                          boundary traversal at.
//         side           - passes the EdgeSide of the WingedEdge.
//
//     Output:
//
//         boundaryExt    - passes a reference to an OpsFloatExtent structure;
//                          the extent of the vertices in the boundary is
//                          copied to the referenced structure.
//
// RETURNS: TRUE if a boundary was successfully traversed, and FALSE otherwise.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to accumulate the boundary vertices.
//
//------------------------------------------------------------------------------

BOOL PlaneSweep::TraverseBoundary(WingedEdge *wEdge, WingedEdge::EdgeSide side,
    OpsFloatExtent &boundaryExt)
{
#if defined _DEBUG
    if (traceBoundaryTraversal)
        ::fprintf(boundaryTraversalTraceFile, "Traversing boundary:\n");
#endif

    // setup to traverse the boundary (the boundary is a closed loop in
    // the graph of WingedEdges)

    WingedEdge *startEdge = wEdge;
    WingedEdge::EdgeSide startSide = side;
    OpsDoublePoint *vert = wEdge->m_vert[side];
    OpsFloatPoint floatPoint(static_cast<float>(vert->x), static_cast<float>(vert->y));
    boundaryExt.Initialize(&floatPoint);
    BOOL abandonBoundary = FALSE;
    m_nBoundaryVertices = 0;

    // traverse the edges of the boundary, abandoning traversal once an edge
    // is encountered whose edge-side has either already been visited or is
    // marked as interior (all the edge-sides must be exterior in order for
    // the boundary to qualify for output)

    do {
        assert(!wEdge->EdgeDeleted());

        if (wEdge->SideVisited(side)) {

#if defined _DEBUG
            if (traceBoundaryTraversal && PERF_SHOW_TRACE_BOUNDARY)
                ::fprintf(boundaryTraversalTraceFile, "id: %d - already visited\n", wEdge->m_id);
#endif

            abandonBoundary = TRUE;
            break;
        }

        wEdge->SetSideVisited(side);

        if (!wEdge->SideIn(side))
            AddBoundaryVertex(floatPoint);
        else if (!abandonBoundary) {
#if defined _DEBUG
            if (traceBoundaryTraversal && PERF_SHOW_TRACE_BOUNDARY)
                ::fprintf(boundaryTraversalTraceFile, "id: %d - incorrect side\n", wEdge->m_id);
#endif
            abandonBoundary = TRUE;
            break;
        }

        if (PERF_SHOW_TRACE_BOUNDARY)
            printf("id: %d\n", wEdge->m_id);

        vert = wEdge->m_vert[(side + 1) % 2];
        floatPoint.x = static_cast<float>(vert->x);
        floatPoint.y = static_cast<float>(vert->y);
        boundaryExt.Update(&floatPoint);
        wEdge = wEdge->m_cwEdge[side];
        side = wEdge->GetSide(vert);

    } while ( wEdge != startEdge || side != startSide);

    // if no interior edges were encountered, then close the boundary and
    // call internal AcceptBoundary() method to determine whether or not to
    // accept the boundary

    if ( m_nBoundaryVertices < 2 )
        abandonBoundary = true;

    if (!abandonBoundary) {
        CloseBoundary();

        Orientation boundaryOrient;
        OpsDoublePoint interiorPt;

        if (!GetBoundaryInfo(m_boundaryVertices, m_nBoundaryVertices,
                boundaryExt, boundaryOrient, interiorPt))
            abandonBoundary = TRUE;
        else
            abandonBoundary = !AcceptBoundary(boundaryExt, boundaryOrient,
                m_boundaryVertices[0], interiorPt);
    }

    // return TRUE or FALSE depending on whether a boundary was detected

    return !abandonBoundary;

} // end: TraverseBoundary()


//------------------------------------------------------------------------------
//
// METHOD: AddBoundaryVertex().
//
// PURPOSE: Add a boundary vertex to the m_boundaryVertices member, and
//          increment m_nBoundaryVertices. The boundary vertices array is
//          resized if necessary to handle the new vertex.
//
// PARAMETERS:
//
//     Input:
//
//         vertex - reference to the new boundary vertex.
//
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to add the boundary vertex.
//
//------------------------------------------------------------------------------

void PlaneSweep::AddBoundaryVertex(const OpsFloatPoint &vertex)
{
    // resize the array of boundary vertices if necessary

    ResizeBoundaryArray();

    // if this is the first vertex then just add it to the array of boundary
    // vertices

    if (m_nBoundaryVertices == 0)
        m_boundaryVertices[m_nBoundaryVertices++] = vertex;

    // else if the new vertex does not induce a degeneracy then add the vertex
    // to the end of the array

    else {
        // first check that the last vertex in the array is not the same as the
        // new vertex (otherwise have a zero length degenerate edge)

        if (m_boundaryVertices[m_nBoundaryVertices-1] == vertex)
            return;

        // if no degeneracy with the last vertex, then check for degeneracy
        // with the last edge

        if (m_nBoundaryVertices > 1) {
            const OpsFloatPoint &vertex1 = m_boundaryVertices[m_nBoundaryVertices-2];
            const OpsFloatPoint &vertex2 = m_boundaryVertices[m_nBoundaryVertices-1];

            if (EdgesAreDegenerate(vertex1, vertex2, vertex))
                return;
        }

        // if we got here, then no local degeneracy was detected, add the new
        // vertex to the end of the array

        m_boundaryVertices[m_nBoundaryVertices++] = vertex;
    }

} // end: AddBoundaryVertex()


//------------------------------------------------------------------------------
//
// METHOD: CloseBoundary().
//
// PURPOSE: Close the current boundary by adding the first vertex to the end of
//          the array of vertices.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to add the first boundary vertex.
//
//------------------------------------------------------------------------------

void PlaneSweep::CloseBoundary()
{
    // check for a degeneracy with the first edge, and shift the array left by
    // one vertex if a degeneracy is reported (shifting left removes the degen-
    // erate edge)

    assert(m_nBoundaryVertices > 1);

    if (m_nBoundaryVertices > 1) {
        const OpsFloatPoint &vertex1 = m_boundaryVertices[1];
        const OpsFloatPoint &vertex2 = m_boundaryVertices[0];
        const OpsFloatPoint &vertex3 = m_boundaryVertices[m_nBoundaryVertices-1];

        if (EdgesAreDegenerate(vertex1, vertex2, vertex3)) {
            size_t nBytes = (m_nBoundaryVertices - 1) * sizeof(OpsFloatPoint);
            ::memmove(&m_boundaryVertices[0], &m_boundaryVertices[1], nBytes);
            m_nBoundaryVertices--;
        }
    }

    // resize the array of boundary vertices if necessary

    ResizeBoundaryArray();

    // add the first vertex to the end of the array to close the boundary

    m_boundaryVertices[m_nBoundaryVertices++] = m_boundaryVertices[0];

} // end: CloseBoundary()


//------------------------------------------------------------------------------
//
// METHOD: ResizeBoundaryArray().
//
// PURPOSE: Resize the boundary vertex array if necessary to accomodate a new
//          vertex.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to resize the array.
//
//------------------------------------------------------------------------------

void PlaneSweep::ResizeBoundaryArray()
{
    if (m_nBoundaryVertices + 1 > m_boundaryArraySize)
    {
        size_t newSize = m_boundaryArraySize + BoundaryArraySize;
        OpsFloatPoint *tmp = new OpsFloatPoint[newSize];

        if (m_nBoundaryVertices > 0)
        {
            size_t nBytesToCopy = m_nBoundaryVertices * sizeof(OpsFloatPoint);
            ::memcpy(tmp, m_boundaryVertices, nBytesToCopy);
        }

        delete [] m_boundaryVertices;
        m_boundaryVertices = tmp;
        m_boundaryArraySize = (int)newSize;
    }

} // end: ResizeBoundaryArray()


//------------------------------------------------------------------------------
//
// METHOD: EdgesAreDegenerate().
//
// PURPOSE: Test two edges sharing a common vertex to determine if they are
//          degenerate. The edges are considered degenerate if the second edge
//          doubles back on the first.
//
// PARAMETERS:
//
//     Input:
//
//         vertex1 - passes a reference to the OpsFloatPoint containing the
//                   coordinates of the first vertex of the first edge.
//         vertex2 - passes a reference to the OpsFloatPoint containing the
//                   shared vertex (the second vertex of the first edge, and
//                   the first vertex of the second edge).
//         vertex3 - passes a reference to the OpsFloatPoint containing the
//                   coordinates of the second vertex of the second edge.
//
//     Output:
//
//         None.
//
// RETURNS: TRUE if the egdes are degenerate, and FALSE otherwise.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BOOL PlaneSweep::EdgesAreDegenerate(const OpsFloatPoint &vertex1,
    const OpsFloatPoint &vertex2, const OpsFloatPoint &vertex3) const
{
    return BufferUtility::GetTurnType(vertex1, vertex2, vertex3) ==
        BufferUtility::DoublesBack;

} // end: EdgesAreDegenerate()


//------------------------------------------------------------------------------
//
// METHOD: EdgeCrossesCutLine().
//
// PURPOSE: Determine if an edge of an output boundary crosses a vertical
//          cut-line.
//
// PARAMETERS:
//
//     Input:
//
//         endPt1 - passes a reference to the first endpoint of the edge.
//         endPt2 - passes a reference to the second endpoint of the edge.
//         xCut   - passes the x coordinate of the vertical cutline.
//
//     Output:
//
//         None.
//
// RETURNS: TRUE if the edge crosses the cutline, and FALSE otherwise.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline BOOL PlaneSweep::EdgeCrossesCutLine(const OpsFloatPoint &endPt1,
    const OpsFloatPoint &endPt2, double xCut) const
{
    return (endPt1.x < xCut && endPt2.x > xCut) ||
           (endPt1.x > xCut && endPt2.x < xCut);

} // end: EdgeCrossesCutline()


//------------------------------------------------------------------------------
//
// METHOD: Ordinate().
//
// PURPOSE: Compute the ordinate of the line at the specified abscissa; the line
//          is specified by its endpoints, and is assumed to be non-vertical.
//
// PARAMETERS:
//
//     Input:
//
//         endPt1 - passes a reference to the first endpoint of the edge.
//         endPt2 - passes a reference to the second endpoint of the edge.
//         x      - passes the abscissa at which the oridinate is to be
//                  computed.
//
//     Output:
//
//         None.
//
// RETURNS: The ordinate of the edge at the specified abscissa.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

inline double PlaneSweep::Ordinate(const OpsFloatPoint &endPt1,
    const OpsFloatPoint &endPt2, double x) const
{
    assert(static_cast<double>(endPt2.x) - static_cast<double>(endPt1.x) != 0.0);

    double b = (static_cast<double>(endPt2.y) - static_cast<double>(endPt1.y)) /
               (static_cast<double>(endPt2.x) - static_cast<double>(endPt1.x));
    double a = static_cast<double>(endPt1.y) - b * static_cast<double>(endPt1.x);

    return  a + b * x;

} // end: Ordinate()


//------------------------------------------------------------------------------
//
// METHOD: GetBoundaryInfo().
//
// PURPOSE: Determine the orientation of, and a point strictly interior to
//          a boundary.
//
// PARAMETERS:
//
//     Input:
//
//         boundaryVerts  - passes an array of the boundary vertices. The bound-
//                          ary is assumed to be explicitly closed.
//         nVertices      - passes the number of boundary vertices.
//         boundaryExt    - passes a reference to an OpsFloatExtent containing
//                          the extent of the boundary.
//
//     Output:
//
//         boundaryOrient - reference to an enumerated value of type Orienta-
//                          tion. The orientation of the boundary is copied to
//                          the referenced location.
//         interiorPt     - reference to a OpsDoublePoint. The coordinates of a
//                          point strictly interior to the boundary are copied
//                          to the referenced structure.
//
// RETURNS: TRUE if the boundary information was successfully obtained, and
//          FALSE otherwise (in the latter case, the boundary was found to be
//          degenerate, and a reliable characterization was not possible).
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BOOL PlaneSweep::GetBoundaryInfo(const OpsFloatPoint boundaryVerts[],
    int nVertices, const OpsFloatExtent &boundaryExt,
    Orientation &boundaryOrient, OpsDoublePoint &interiorPt) const
{
    // determine the boundary orientation, along with an open interval centered
    // about the center of the boundary extent such that the no x coordinate of
    // a boundary vertex lies within the interval

    double xMidpoint = static_cast<double>(boundaryExt.xMax + boundaryExt.xMin) / 2.0;
    double xLower = -HUGE_VAL;
    double xUpper = HUGE_VAL;
    double signedArea = 0.0;
    BOOL xMidpointHit = FALSE;

    int i;

    for (i = 0; i < nVertices - 1; i++) {
        signedArea += (static_cast<double>(boundaryVerts[i].x) *
                       static_cast<double>(boundaryVerts[i+1].y)) -
                      (static_cast<double>(boundaryVerts[i].y) *
                       static_cast<double>(boundaryVerts[i+1].x));

        if (boundaryVerts[i].x < xMidpoint) {
            if (boundaryVerts[i].x > xLower)
                xLower = boundaryVerts[i].x;
        }
        else if (boundaryVerts[i].x > xMidpoint) {
            if (boundaryVerts[i].x < xUpper)
                xUpper = boundaryVerts[i].x;
        }
        else
            xMidpointHit = TRUE;
    }

    // set the boundary orientation depending on the signed area of the
    // boundary

    if (signedArea > 0.0)
        boundaryOrient = CounterClockwise;
    else if (signedArea < 0.0)
        boundaryOrient = Clockwise;
    else
        return FALSE;

    // determine a point that is strictly interior to the boundary; from
    // the above we can find a vertical line that cuts the boundary but
    // contains no boundary vertices, and using a "scan-line" technique
    // the point can be found

    if (xLower == -HUGE_VAL || xUpper == HUGE_VAL)
        return FALSE;

    double xCut;

    if (!xMidpointHit)
        xCut = xMidpoint;
    else if ((xUpper - xMidpoint) > (xMidpoint - xLower))
        xCut = (xUpper + xMidpoint) / 2.0;
    else
        xCut = (xLower + xMidpoint) / 2.0;

    double yCross[2];
    yCross[0] = yCross[1] = HUGE_VAL;

    for (i = 0; i < nVertices - 1; i++) {
        if (EdgeCrossesCutLine(boundaryVerts[i], boundaryVerts[i+1], xCut)) {
            double y = Ordinate(boundaryVerts[i], boundaryVerts[i+1], xCut);

            if (y < yCross[0]) {
                yCross[1] = yCross[0];
                yCross[0] = y;
            }
            else if (y < yCross[1])
                yCross[1] = y;
        }
    }

    if (yCross[0] == HUGE_VAL || yCross[1] == HUGE_VAL)
        return FALSE;

    // compute a point strictly interior to the boundary

    interiorPt.x = xCut;
    interiorPt.y = (yCross[0] + yCross[1]) / 2.0;

    return TRUE;

} // end: GetBoundaryInfo()


//------------------------------------------------------------------------------
//
// METHOD: GetNProgressSubIntervals().
//
// PURPOSE: Determine how many subintervals of progress should be used for the
//          specified number of items. The number subintervals is computed to
//          be ceiling(nItems / m_ItemsPerProgressSubInterval).
//
// PARAMETERS:
//
//     Input:
//
//         nItems - passes the number of "items" to report progress for.
//
//     Output:
//
//         None.
//
// RETURNS: The number of subintervals of progress to use.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

int PlaneSweep::GetNProgressSubIntervals(int nItems) const
{
    if (nItems % ItemsPerProgressSubInterval)
        return static_cast<int>(nItems / ItemsPerProgressSubInterval + 1);
    else
        return static_cast<int>(nItems / ItemsPerProgressSubInterval);

} // end: GetNProgressSubIntervals()
