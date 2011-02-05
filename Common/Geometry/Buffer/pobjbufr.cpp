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
// FILE: pobjbufr.cpp.
//
// PURPOSE: Implementation of the PolyObjectBuffer class.
//
//------------------------------------------------------------------------------

#include <vector>

using namespace std;

#include "Foundation.h"
#include "buffer.h"

#if defined PERF_DUMPFFGF
#include "DumpFFGF.h"
#endif

#if defined _DEBUG
BOOL PolyObjectBuffer::traceVertices = FALSE;
STRING PolyObjectBuffer::traceFileName;
#endif


//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize an instance of the PolyObjectBuffer class.
//
// PARAMETERS:
//
//     Input:
//
//         nSegsPerCircle - passes the number of segments that would be used in
//                          the polygonization of a complete circle. This para-
//                          meter specifies the smoothness of polygonized offset
//                          arcs.
//         pBufferUtil    - passes a pointer to the BufferUtility object.
//
//     Output:
//
//         None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

PolyObjectBuffer::PolyObjectBuffer(BufferUtility *pBufferUtil) :
    m_pVertices(NULL),
    m_pnPolyVerts(NULL),
    m_nPolyObjects(0),
    m_pBufferUtil(pBufferUtil),
    m_bufferAlreadyGenerated(FALSE)
{
    // m_polyObjExtent (as well as m_pVertices, m_pmPolyVerts, and m_nPolyObjects)
    // are initialized in PolyObjectBuffer::Initialize()

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

PolyObjectBuffer::~PolyObjectBuffer()
{

} // end: destructor


//------------------------------------------------------------------------------
//
// METHOD: GetNSegmentsPerCircle().
//
// PURPOSE: Get the number of segments used to polygonize complete circles.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: An integer specifying the number of segments per circle.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

int PolyObjectBuffer::GetNSegmentsPerCircle() const
{
    return m_pBufferUtil->GetNSegmentsPerCircle();

} // end: GetNSegmentsPerCircle()


//------------------------------------------------------------------------------
//
// METHOD: GetOffsetDistance().
//
// PURPOSE: Get the buffer offset distance.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: A float specifying the buffer offset distance.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

float PolyObjectBuffer::GetOffsetDistance() const
{
    return m_pBufferUtil->GetOffsetDistance();

} // end: GetOffsetDistance()


//------------------------------------------------------------------------------
//
// METHOD: GenerateBufferZone().
//
// PURPOSE: Create the buffer zone polygon for the specified poly-object (where
//          a poly-object is a poly-polyline or poly-polygon).
//
// PARAMETERS:
//
//          vertices      - passes an array of the poly-object vertices.
//          nPolyVerts    - passes an array of the counts of the number of
//                          vertices in each sub-object (polyline or polygon).
//          nPolyObjects  - passes the number of sub-objects.
//          callback      - passes a reference to a ProgressCallback object. The
//                          object is periodically notified of progress, and
//                          checked to determine if the buffer operation has
//                          been canceled.
//
//     Output:
//
//          bufferPolygon - passes a reference to an OrientedPolyPolygon object.
//                          The buffer zone polygon containing for the poly-
//                          object is copied to the referenced object.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to create the buffer zone. A PlaneSweepException is
//             thrown if an error is detected during the execution of the plane
//             sweep algorithm.
//
//------------------------------------------------------------------------------

void PolyObjectBuffer::GenerateBufferZone(const OpsFloatPoint vertices[],
    const int nPolyVerts[], int nPolyObjects, ProgressCallback &callback,
    OrientedPolyPolygon &bufferPolygon)
{
#ifdef _DEBUG
    // trace object vertices to a file if requested to do so

    if (traceVertices)
        TraceObjectVertices(vertices, nPolyVerts, nPolyObjects);
#endif

    // if a buffer has already been generated, then reinitialize the base class

    if (m_bufferAlreadyGenerated)
        PlaneSweep::Initialize();

    m_bufferAlreadyGenerated = TRUE;

    // create convex offset chains around each sub-object

    for (int i = 0, j = 0; i < nPolyObjects; i++) {

        CreateOffsetChains(&vertices[j], nPolyVerts[i]);
        j += nPolyVerts[i];
    }

    // perform the plane sweep, generating the buffer zone polygon in the
    // process

    GreatCircleBufferUtil * util = dynamic_cast<GreatCircleBufferUtil *>(m_pBufferUtil);

    DoPlaneSweep(callback, util? util->GetFloatTransform() : NULL, bufferPolygon);

} // end: GenerateBufferZone()


#if defined _DEBUG

//------------------------------------------------------------------------------
//
// METHOD: EnableObjectVerticesTrace().
//
// PURPOSE: Enable/disable tracing of object vertices.
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

void PolyObjectBuffer::EnableObjectVerticesTrace(BOOL enableTrace,
    STRING traceFile)
{
    traceVertices = enableTrace;

    if (traceVertices) {
        assert(!traceFile.empty());
        traceFileName = traceFile;
    }

} // end: EnableBoundaryTraversalTrace()


//------------------------------------------------------------------------------
//
// METHOD: TraceObjectVertices().
//
// PURPOSE: Write the object vertices to a trace file.
//
// PARAMETERS:
//
//          vertices      - passes an array of the poly-object vertices.
//          nPolyVerts    - passes an array of the counts of the number of
//                          vertices in each sub-object (polyline or polygon).
//          nPolyObjects  - passes the number of sub-objects.
//
//     Output:
//
//          None.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void PolyObjectBuffer::TraceObjectVertices(const OpsFloatPoint vertices[],
    const int nPolyVerts[], int nPolyObjects) const
{
    FILE *traceFile = ::fopen(MgUtil::WideCharToMultiByte(traceFileName).c_str(), "a");
    assert(traceFile != NULL);

    ::fprintf(traceFile, "**** Poly Object Vertices:\n");

    for (int i = 0, j = 0; i < nPolyObjects; i++) {
        ::fprintf(traceFile, "Sub-object %d\n", i);

        for (int k = 0; k < nPolyVerts[i]; k++, j++)
            ::fprintf(traceFile, "    %12.4f, %12.4f\n", vertices[j].x,
                vertices[j].y);
    }

    ::fclose(traceFile);

} // end: TraceObjectVertices()

#endif


//------------------------------------------------------------------------------
//
// METHOD: PointWithinOffsetDist().
//
// PURPOSE: Test a point to determine if it is within the offset distance with
//          respect to any line segment in the specified poly-object.
//
// PARAMETERS:
//
//     Input:
//
//         vertices  - passes an array of the poly-object vertices.
//         nVertices - passes an array of the counts of the number of
//                     vertices in each sub-object (polyline or polygon).
//         point     - passes a reference to a OpsDoublePoint containing the
//                     coordinates of the point to test.
//
//     Output:
//
//         None.
//
// RETURNS: TRUE if the point is within a distance of offsetDist of any line
//          segment of the poly-object, and FALSE otherwise.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BOOL PolyObjectBuffer::PointWithinOffsetDist(const OpsFloatPoint vertices[],
    int nVertices, const OpsDoublePoint &point) const
{
    double offsetDist = fabs(m_pBufferUtil->GetOffsetDistance());

    for (int i = 0; i < nVertices - 1; i++) {
        if (m_pBufferUtil->DistFromPointToLineSeg(&vertices[i], point) <=
                offsetDist)
            return TRUE;
    }

    return FALSE;

} // end: PointWithinOffsetDist()


//------------------------------------------------------------------------------
//
// METHOD: CreateOffsetChains().
//
// PURPOSE: Compute a set of "convex offset chains" around the vertices of the
//          specified poly-object (a polyline or polygon boundary). The input
//          poly-object is scanned for adjacent segments that double back on
//          each other, since these are not handled directly by the Create-
//          ConvexOffsetChains() method.
//
// PARAMETERS:
//
//     Input:
//
//         vertices  - passes the array of poly-object vertices. Note that the
//                     array of vertices is assumed to be free of redundancies;
//                     that is, no two adjacent vertices are identical.
//         nVertices - passes the number of vertices.
//
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to add the offset chain edges.
//
//------------------------------------------------------------------------------

void PolyObjectBuffer::CreateOffsetChains(const OpsFloatPoint vertices[],
    int nVertices)
{
    vector<OpsFloatPoint*> vChainEdges;
    vector<int> vChainEdgesCount;

    m_pBufferUtil->CreateOffsetChains(vertices, nVertices, vChainEdges, vChainEdgesCount);

#ifdef PERF_DUMPFFGF_MAX
    GreatCircleBufferUtil * util = dynamic_cast<GreatCircleBufferUtil *>(m_pBufferUtil);
    FILE *ffgfFile = MgDumpFFGF::createFile( "CreateOffsetChains", PlaneSweep::m_currentFile++, "" );
    for (unsigned int i = 0; i < vChainEdges.size(); ++i)
    {
        MgDumpFFGF::writeFile( ffgfFile, util? util->GetFloatTransform() : NULL, i, vChainEdges[i], vChainEdgesCount[i] );
    }
    MgDumpFFGF::closeFile(ffgfFile);
#endif

    for (unsigned int i = 0; i < vChainEdges.size(); ++i)
    {
        AddEdges(vChainEdges[i], vChainEdgesCount[i], Left);
        delete [] vChainEdges[i];
    }
} // end: CreateOffsetChains()

//------------------------------------------------------------------------------
//
// METHOD: CreateBufferZone().
//
// PURPOSE: Create a buffer zone around the polygon object specified at
//          construction time.
//
// PARAMETERS:
//
//     Input:
//
//         callback       - passes a pointer to a ProgressCallback object. The
//                          object is periodically notified of progress, and
//                          checked to determine if the buffer operation has
//                          been canceled.
//
//     Output:
//
//         bufferPolygon  - passes a reference to an OrientedPolyPolygon object.
//                          The boundaries of the buffer zone polygon are copied
//                          to the referenced object.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to create the buffer zone. A PlaneSweepException is
//             thrown if an error is detected during the execution of the plane
//             sweep algorithm.
//
//------------------------------------------------------------------------------

void PolyObjectBuffer::CreateBufferZone(ProgressCallback &callback,
    OrientedPolyPolygon &bufferPolygon)
{
    m_pBufferUtil->InitPolyObject();

    MG_TRY()

    // forward call to base class method

    GenerateBufferZone(m_pVertices, m_pnPolyVerts, m_nPolyObjects,
        callback, bufferPolygon);

    MG_CATCH(L"PolyObjectBuffer.CreateBufferZone")

    // If the default CreateOffsetChain algorithm failed, try using
    // the CreateOffsetChain line-by-line algorithm.

    // Flag the line-by-line algorithm and also check
    // to see if this is indeed a GreatCircleBufferUtil.
    if (mgException != 0) // mgException is defined in MG_TRY() macro
    {
        if ( !PERF_RUN_LBL_ALGORITHM_ON_FAILURE )
        {
            MG_THROW();
        }
        else
        {
            mgException.p->Release();
            mgException.p = 0;
            if (m_pBufferUtil->CreateOffsetChainLBL(TRUE))
            {
                MG_TRY()

                // forward call to base class method
                GenerateBufferZone(m_pVertices, m_pnPolyVerts, m_nPolyObjects,
                    callback, bufferPolygon);

                MG_CATCH_AND_THROW(L"PolyObjectBuffer.CreateBufferZone")
            }
        }
    }

} // end: CreateBufferZone()

//------------------------------------------------------------------------------
//
// METHOD: Initialize().
//
// PURPOSE: Initialize a PolygonBuffer object using the vertices available from
//          the specified VertexAccessor object.
//
// PARAMETERS:
//
//     Input:
//
//         vertAccessor   - passes a reference to the VertexAccessor object
//                          from which the polygon vertices will be obtained.
//
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to complete initialization.
//
//------------------------------------------------------------------------------

void PolyObjectBuffer::Initialize(const VertexAccessor &vertAccessor)
{
    // initialize ...

    m_pVertices = NULL;
    m_pnPolyVerts = NULL;
    m_nPolyObjects = 0;

    MG_TRY()

    // allocate arrays to contain a copy of the vertices and vertex counts

    m_pVertices = new OpsFloatPoint[vertAccessor.GetTotalVertices()];
    m_pnPolyVerts = new int[vertAccessor.GetNPolyObjects()];

    // copy the vertices, culling redundant vertices in the process

    for (int i = 0, j = 0, k = 0; i < vertAccessor.GetNPolyObjects(); i++) {
        m_pVertices[j++] = vertAccessor[k++];
        m_pnPolyVerts[m_nPolyObjects] = 1;

        for (int l = 1; l < vertAccessor.GetNPolyVerts(i); l++) {
            m_pVertices[j] = vertAccessor[k++];

            if (m_pVertices[j] != m_pVertices[j-1]) {
                j++;
                m_pnPolyVerts[m_nPolyObjects]++;
            }
        }

        m_nPolyObjects++;
    }

    // get the extent of the poly object

    vertAccessor.GetExtent(m_polyObjExt);

    MG_CATCH(L"PolyObjectBuffer.Initialize")

    if (mgException != 0) // mgException is defined in MG_TRY() macro
    {
        Cleanup();
    }

    MG_THROW()

} // end: Initialize()

//------------------------------------------------------------------------------
//
// METHOD: Cleanup().
//
// PURPOSE: Release all dynamically allocated memory.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: None.
//
// EXCEPTIONS: None,
//
//------------------------------------------------------------------------------

void PolyObjectBuffer::Cleanup()
{
    delete [] m_pVertices;
    delete [] m_pnPolyVerts;

} // end: Cleanup()
