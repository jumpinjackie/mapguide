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
// FILE: interlst.cpp.
//
// PURPOSE: Implementation of the IntersectionList class.
//
//------------------------------------------------------------------------------

#include "Foundation.h"
#include "buffer.h"

#if defined PERF_DUMPFFGF_MAX
#include "DumpFFGF.h"
#endif

static const int IntersectionRecBlockSize = 1024;
static const int IntersectionStepsPerProgressSubInterval = 128;

#if defined _DEBUG
BOOL IntersectionList::traceIntersections = FALSE;
STRING IntersectionList::intersectionTraceFileName;
#endif


//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize an IntersectionList object.
//
// PARAMETERS:
//
//     Input:
//
//         vertAlloc - passes a pointer to the vertex allocator object that is
//                     to be used to allocate new edge vertices when the inter-
//                     sections are processed.
//         edgeAlloc - passes a pointer to the edge allocator object that is
//                     to be used to allocate new edges when the intersections
//                     are processed.
//
//     Output:
//
//         None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to initialize the object.
//
//------------------------------------------------------------------------------

IntersectionList::IntersectionList(VertexAllocator *vertAlloc,
    WingedEdgeAllocator *wingedEdgeAlloc, FloatTransform* transform) :
    m_allocator(IntersectionRecBlockSize),
    m_Transform(transform),
    m_intersectProc(NULL)
{
    m_intersectProc = new IntersectionProcessor(vertAlloc, wingedEdgeAlloc, m_Transform);
} // end: constructor


//------------------------------------------------------------------------------
//
// METHOD: Destructor.
//
// PURPOSE: Releases all dynamically allocated memory.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

IntersectionList::~IntersectionList()
{
    // destroy the intersection processor object

    delete m_intersectProc;

} // end: destructor


//------------------------------------------------------------------------------
//
// METHOD: Add().
//
// PURPOSE: Add an intersection point along with the intersecting edges into
//          the list.
//
// PARAMETERS:
//
//     Input:
//
//         intersectPt - passes a reference to the intersection point. A copy
//                       is made.
//         edge1       - passes a pointer to the first of the intersecting
//                       edges.
//         edge2       - passes a pointer to the second of the intersecting
//                       edges.
//
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available for the insertion.
//
//------------------------------------------------------------------------------

bool IntersectionList::Add(const OpsDoublePoint &intersectPt, WingedEdge *edge1,
    WingedEdge *edge2)
{

    // Avoid duplicates

    if ( PERF_SKIP_DUPLICATE_INTERSECTIONS && this->GetSize() != 0 )
    {
        IntersectionRecord &intersectRec1 = (*this)[this->GetSize() - 1];

        if ((intersectRec1.m_intersectionPt.x == intersectPt.x) &&
            (intersectRec1.m_intersectionPt.y == intersectPt.y))
        {
            return false;
        }
    }

    // get a reference to the next available IntersectionRecord

    IntersectionRecord &intersectionRec = *m_allocator.GetNextObject();

    // initialize the record from the specified parameters

    intersectionRec.m_intersectionPt = intersectPt;
    intersectionRec.m_edge1 = edge1;
    intersectionRec.m_edge2 = edge2;

    return true;

} // end: Add()


//------------------------------------------------------------------------------
//
// METHOD: ProcessIntersections().
//
// PURPOSE: Sort the intersection records on lexicographic order of the points
//          of intersection, and then process all edges with common points of
//          intersection based on the sorted order.
//
// PARAMETERS:
//
//     Input:
//
//         callback   - passes a reference to a ProgressCallback object. The
//                      referenced object is is periodically called to report
//                      progress, and determine whether a cancelation has been
//                      signalled.
//         sortMethod - passes an enumerated value of type SortMethod specifying
//                      which algorithm to use to sort the intersection records
//                      into lexicographic order.
//
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to process the intersections.
//
//------------------------------------------------------------------------------

void IntersectionList::ProcessIntersections(ProgressCallback &callback,
    SortMethod sortMethod)
{
    int nRecords = m_allocator.GetNObjects();

#if defined _DEBUG
    // trace intersections in debug mode if requested to do so

    FILE *traceFile = NULL;

    if (traceIntersections) {
        traceFile = ::fopen(MgUtil::WideCharToMultiByte(intersectionTraceFileName).c_str(), "a");
        assert(traceFile != NULL);
        ::fprintf(traceFile,
            "\n**** Start intersection trace, # of intersections = %d\n\n",
            nRecords);
        m_intersectProc->TraceIntersectionsTo(traceFile);
    }
#endif

    // setup two progress subintervals - one for the sort phase, and the other
    // for the intersection processing phase

    callback.BeginProgressInterval(2);

    // sort the intersection records

    Sort(callback, sortMethod);

    // process the intersections - note that adjacent intersection records
    // with the same point of intersection are processed as a group

    WingedEdgeArray edgeArray;
    callback.BeginProgressInterval(GetNProgressSubIntervals(nRecords));

    int i = 0;
    int j = 0;
    for (i = 0; i < nRecords;) {
        IntersectionRecord &intersectRec1 = (*this)[i];

        edgeArray.AddEdge(intersectRec1.m_edge1);
        edgeArray.AddEdge(intersectRec1.m_edge2);

        for (j = i + 1; j < nRecords; j++) {
            if (j % IntersectionStepsPerProgressSubInterval == 0)
                callback.AdvanceOneSubInterval();

            IntersectionRecord &intersectRec2 = (*this)[j];

            if (intersectRec1.m_intersectionPt!=intersectRec2.m_intersectionPt)
                break;

            edgeArray.AddEdge(intersectRec2.m_edge1);
            edgeArray.AddEdge(intersectRec2.m_edge2);
        }

        m_intersectProc->ProcessIntersection(intersectRec1.m_intersectionPt,
            edgeArray);

        i = j;
        edgeArray.Reset();
    }

    // end the progress intervals for the processing and sort phases

    callback.EndProgressInterval();
    callback.EndProgressInterval();

#if defined _DEBUG
    // close trace file

    if (traceIntersections) {
        ::fclose(traceFile);
        traceFile = NULL;
        m_intersectProc->TraceIntersectionsTo(NULL);
    }
#endif

} // end: ProcessIntersections()


#if defined _DEBUG

//------------------------------------------------------------------------------
//
// METHOD: EnableIntersectionTrace().
//
// PURPOSE: Enable/disable tracing of computed points of intersection.
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

void IntersectionList::EnableIntersectionTrace(BOOL enableTrace,
    STRING traceFile)
{
    traceIntersections = enableTrace;

    if (traceIntersections) {
        assert(!traceFile.empty());
        intersectionTraceFileName = traceFile;
    }

} // end: EnableIntersectionTrace()

#endif


//------------------------------------------------------------------------------
//
// METHOD: Sort().
//
// PURPOSE: Sort the list of IntersectionRecord objects into lexicographic
//          order on the points of intersection.
//
// PARAMETERS:
//
//     Input:
//
//         callback   - passes a reference to a ProgressCallback object. The
//                      referenced callback object is periodically queried to
//                      determine whether or not to cancel processing, and to
//                      report progress.
//         sortMethod - passes an enumerated value of type SortMethod specifying
//                      which algorithm to use to sort the intersection records
//                      into lexicographic order.
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

void IntersectionList::Sort(ProgressCallback &callback, SortMethod sortMethod)
{
    BufferSort<IntersectionRecord> sortRecords;
    sortRecords.Sort(sortMethod, m_allocator, IntersectionStepsPerProgressSubInterval,
        callback);

} // end: Sort()


//------------------------------------------------------------------------------
//
// METHOD: GetNProgressSubIntervals().
//
// PURPOSE: Determine how many subintervals of progress should be used for the
//          specified number of steps. The number subintervals is computed to
//          be ceiling(nSteps / IntersectionStepsPerProgressSubInterval).
//
// PARAMETERS:
//
//     Input:
//
//         nSteps - passes the number of "steps" to report progress for.
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

int IntersectionList::GetNProgressSubIntervals(int nSteps) const
{
    if (nSteps % IntersectionStepsPerProgressSubInterval)
        return nSteps / IntersectionStepsPerProgressSubInterval + 1;
    else
        return nSteps / IntersectionStepsPerProgressSubInterval;

} // end: GetNProgressSubIntervals()


#if defined PERF_DUMPFFGF_MAX
void IntersectionList::Dump2FFGF( FloatTransform* transform )
{
    FILE *ffgfFile = MgDumpFFGF::createFile( "intersections", PlaneSweep::m_currentFile++, "" );

    for (int i = 0; i < this->GetSize(); i++) {
        IntersectionRecord &intersectRec1 = (*this)[i];

        MgDumpFFGF::writeFile( ffgfFile, transform, i, (float)intersectRec1.m_intersectionPt.x, (float)intersectRec1.m_intersectionPt.y );
    }
    MgDumpFFGF::closeFile(ffgfFile);
}
#endif
