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
// FILE: orpplygn.cpp.
//
// PURPOSE: Implemenation of the OrientedPolyPolygon class.
//
//------------------------------------------------------------------------------

#include "Foundation.h"
#include "buffer.h"

//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize an OrientedPolyPolygon object.
//
// PARAMETERS:
//
//     Input:
//
//         nExpectedBoundaries - passes an initial estimate of the number of
//                               boundaries that will be added via calls to
//                               AddBoundary() (default value of 1).
//
//     Output:
//
//        None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insuffient memory
//             available to initialize the object.
//
//------------------------------------------------------------------------------

OrientedPolyPolygon::OrientedPolyPolygon(int nExpectedBoundaries) :
    m_nBoundaries(0),
    m_maxBoundaries(0),
    m_boundaryExt(NULL),
    m_nBoundaryVerts(NULL),
    m_totalVertices(0),
    m_boundaries(NULL)
{
    assert(nExpectedBoundaries > 0);

    MG_TRY()

    m_nBoundaryVerts = new int[nExpectedBoundaries];
    m_boundaryExt = new OpsFloatExtent[nExpectedBoundaries];
    m_boundaries = new OpsFloatPoint *[nExpectedBoundaries];
    m_maxBoundaries = nExpectedBoundaries;

    MG_CATCH(L"OrientedPolyPolygon.OrientedPolyPolygon")

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

OrientedPolyPolygon::~OrientedPolyPolygon()
{
    Cleanup();

} // end: destructor

OrientedPolyPolygon::OrientedPolyPolygon(const OrientedPolyPolygon &orientedPolyPolygon) :
    m_boundaryExt(NULL),
    m_nBoundaryVerts(NULL),
    m_boundaries(NULL)
{
    *this = orientedPolyPolygon;
}

OrientedPolyPolygon & OrientedPolyPolygon::operator=(const OrientedPolyPolygon &orientedPolyPolygon)
{
    if (this != &orientedPolyPolygon)
    {
        Cleanup();

        m_nBoundaries = orientedPolyPolygon.m_nBoundaries;
        m_maxBoundaries = orientedPolyPolygon.m_maxBoundaries;
        m_totalVertices = orientedPolyPolygon.m_totalVertices;
        m_extentOfBoundaries = orientedPolyPolygon.m_extentOfBoundaries;

        MG_TRY()

        m_nBoundaryVerts = new int[m_maxBoundaries];
        m_boundaryExt = new OpsFloatExtent[m_maxBoundaries];
        m_boundaries = new OpsFloatPoint *[m_maxBoundaries];

        for (int i = 0; i < m_nBoundaries; i++)
        {
            m_nBoundaryVerts[i] = orientedPolyPolygon.m_nBoundaryVerts[i];
            m_boundaryExt[i] = orientedPolyPolygon.m_boundaryExt[i];
            m_boundaries[i] = new OpsFloatPoint[m_nBoundaryVerts[i]];

            for (int j = 0; j < m_nBoundaryVerts[i]; j++)
            {
                m_boundaries[i][j] = orientedPolyPolygon.m_boundaries[i][j];
            }
        }

        MG_CATCH(L"OrientedPolyPolygon.operator=")

        if (mgException != 0) // mgException is defined in MG_TRY() macro
        {
            Cleanup();
        }

        MG_THROW()

    }
    return *this;
}


//------------------------------------------------------------------------------
//
// METHOD: GetNBoundaries().
//
// PURPOSE: Determine the number of boundaries in the oriented poly-polygon.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: The number of boundaries in the poly-polygon.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

int OrientedPolyPolygon::GetNBoundaries() const
{
    return m_nBoundaries;

} // end: GetNBoundaries()


//------------------------------------------------------------------------------
//
// METHOD: GetNBoundaryVertices().
//
// PURPOSE: Get the number of vertices in the specified boundary.
//
// PARAMETERS:
//
//     Input:
//
//         boundaryIndex - passes the zero-based index of the boundary.
//
//     Output:
//
//         None.
//
// RETURNS: The number of vertices in the indexed boundary.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

int OrientedPolyPolygon::GetNBoundaryVertices(int boundaryIndex) const
{
    assert(boundaryIndex >= 0);
    assert(boundaryIndex < m_nBoundaries);

    return m_nBoundaryVerts[boundaryIndex];

} // end: GetNBoundaryVertices()


//------------------------------------------------------------------------------
//
// METHOD: GetTotalVertices().
//
// PURPOSE: Get the number of vertices for all the boundaries.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: The total number of vertices.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

int OrientedPolyPolygon::GetTotalVertices() const
{
    return m_totalVertices;

} // end: GetTotalVertices()


//------------------------------------------------------------------------------
//
// METHOD: GetBoundary().
//
// PURPOSE: Get a pointer to the array of vertices for the specified boundary.
//
// PARAMETERS:
//
//     Input:
//
//         boundaryIndex - passes the zero-based index of the boundary.
//
//     Output:
//
//         None.
//
// RETURNS: A pointer to the array of vertices for the indexed boundary.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

const OpsFloatPoint *OrientedPolyPolygon::GetBoundary(int boundaryIndex) const
{
    assert(boundaryIndex >= 0);
    assert(boundaryIndex < m_nBoundaries);

    return m_boundaries[boundaryIndex];

} // end: GetBoundary()


//------------------------------------------------------------------------------
//
// METHOD: GetExtent().
//
// PURPOSE: Get the extent of the oriented poly-polygon.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: An OpsFloatExtent object containing the extent of the poly-polygon.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

OpsFloatExtent OrientedPolyPolygon::GetExtent() const
{
    return m_extentOfBoundaries;

} // end: GetExtent()


//------------------------------------------------------------------------------
//
// METHOD: GetPolyPolygon().
//
// PURPOSE: Retrieve the data defining the poly-polygon.
//
// PARAMETERS:
//
//     Input:
//
//         None.
//
//     Output:
//
//         vertices   - passes a pointer to an array into which the poly-polygon
//                      vertices are to be copied. The array is assumed to be
//                      large enough.
//         nPolyVerts - passes a pointer to an array into which the polygon
//                      vertex counts are to be copied.The array is assumed to
//                      be large enough.
//         nPolygons  - passes a reference to an int. The number of polygons in
//                      the poly-polygon is copied to the referenced int.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void OrientedPolyPolygon::GetPolyPolygon(OpsFloatPoint vertices[],
    int nPolyVerts[], int &nPolygons) const
{
    for (int i = 0, j = 0; i < m_nBoundaries; i++) {
        nPolyVerts[i] = m_nBoundaryVerts[i];

        for (int k = 0; k < m_nBoundaryVerts[i]; k++)
            vertices[j++] = m_boundaries[i][k];

    }

    nPolygons = m_nBoundaries;

} // end: GetPolyPolygon()


//------------------------------------------------------------------------------
//
// METHOD: AddBoundary().
//
// PURPOSE: Add a polygon boundary to the oriented poly-polygon.
//
// PARAMETERS:
//
//     Input:
//
//         vertices    - passes an array of the boundary vertices. Note that
//                       the boundary is assumed to be explicitly closed. A
//                       copy is made of the vertices in the array.
//         nVertices   - passes the number of vertices in the boundary.
//         extent      - passes a reference to an OpsFloatExtent containing the
//                       extent of the boundary vertices.
//
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to copy the boundary information.
//
//------------------------------------------------------------------------------

void OrientedPolyPolygon::AddBoundary(const OpsFloatPoint vertices[],
    int nVertices, const OpsFloatExtent &extent)
{
    // check for sensible input

    assert(nVertices > 0);
    assert(vertices[0] == vertices[nVertices-1]);

    // resize the arrays if necessary

    if (m_nBoundaries >= m_maxBoundaries)
        ResizeArrays();

    // copy the boundary information

    m_nBoundaryVerts[m_nBoundaries] = nVertices;
    m_totalVertices += nVertices;
    m_boundaryExt[m_nBoundaries] = extent;
    m_boundaries[m_nBoundaries] = new OpsFloatPoint[nVertices];
    size_t nBytes = nVertices * sizeof(OpsFloatPoint);
    ::memcpy(m_boundaries[m_nBoundaries], vertices, nBytes);
    m_nBoundaries++;

    if (m_nBoundaries > 1)
        m_extentOfBoundaries.UnionWith(&extent);
    else
        m_extentOfBoundaries = extent;

} // end: AddBoundary()


//------------------------------------------------------------------------------
//
// METHOD: BoundaryContained().
//
// PURPOSE: Determine whether a boundary of another poly-polygon is contained
//          by this oriented poly-polygon. The boundary to be tested is charact-
//          erized by an interior point, and its extent. The test assumes that
//          the boundary to be tested does not intersect any of the boundaries
//          of the oriented poly-polygon (such as is the case after a plane
//          sweep operation has been performed and the boundary being tested is
//          a potential output boundary).
//
// PARAMETERS:
//
//     Input:
//
//         boundaryExt       - passes a reference to an OpsFloatExtent contain-
//                             ing the coordinates of the boundary extent.
//         interiorPt        - passes a reference to a OpsDoublePoint containing
//                             the coordinates of a point strictly interior to
//                             the boundary.
//         strictContainment - passes TRUE if the boundary must be strictly
//                             contained within any of the boundaries of the
//                             oriented poly-polygon.
//
//     Output:
//
//         None.
//
// RETURNS: TRUE if the point is contained in the poly-polygon and FALSE
//          otherwise.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BOOL OrientedPolyPolygon::BoundaryContained(const OpsFloatExtent &boundaryExt,
    const OpsDoublePoint &interiorPt, BOOL strictContainment) const
{
    int windingNumber = 0;

    for (int i = 0; i < m_nBoundaries; i++) {
        if (m_boundaryExt[i].Contains(&boundaryExt, strictContainment)) {
            windingNumber += BufferUtility::WindingNumber(m_boundaries[i],
                m_nBoundaryVerts[i], interiorPt);
        }
    }

    return static_cast<BOOL>(windingNumber % 2);

} // end: PointInPolygon()


//------------------------------------------------------------------------------
//
// METHOD: ReverseBoundaries().
//
// PURPOSE: Reverses the orientation of all boundaries in this oriented
//          poly-polygon.
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

void OrientedPolyPolygon::ReverseBoundaries() const
{
    OpsFloatPoint tmp;
    for (int i = 0; i < m_nBoundaries; i++)
    {
        int numPoints = m_nBoundaryVerts[i];
        int numToSwap = numPoints / 2;

        for (int k = 0; k < numToSwap; k++)
        {
            tmp = m_boundaries[i][k];
            m_boundaries[i][k] = m_boundaries[i][numPoints - 1 - k];
            m_boundaries[i][numPoints - 1 - k] = tmp;
        }
    }
}


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
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void OrientedPolyPolygon::Cleanup()
{
    delete [] m_nBoundaryVerts;
    delete [] m_boundaryExt;

    for (int i = 0; i < m_nBoundaries; i++)
        delete [] m_boundaries[i];

    delete [] m_boundaries;

} // end: Cleanup()


//------------------------------------------------------------------------------
//
// METHOD: ResizeArrays().
//
// PURPOSE: Resize the internal arrays to accomodate the addition of a boundary.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to resize the arrays.
//
//------------------------------------------------------------------------------

void OrientedPolyPolygon::ResizeArrays()
{
    // double the current size of the arrays, reallocate, and copy

    int newMaxBoundaries = m_maxBoundaries * 2;
    int *temp1 = NULL;
    OpsFloatExtent *temp2 = NULL;
    OpsFloatPoint **temp3 = NULL;

    MG_TRY()

    temp1 = new int[newMaxBoundaries];
    temp2 = new OpsFloatExtent[newMaxBoundaries];
    temp3 = new OpsFloatPoint *[newMaxBoundaries];

    MG_CATCH(L"OrientedPolyPolygon.ResizeArrays")

    if (mgException != 0) // mgException is defined in MG_TRY() macro
    {
        delete [] temp1;
        delete [] temp2;
        delete [] temp3;
    }

    MG_THROW()

    for (int i = 0; i < m_nBoundaries; i++)
    {
        temp1[i] = m_nBoundaryVerts[i];
        temp2[i] = m_boundaryExt[i];
        temp3[i] = m_boundaries[i];
    }

    delete [] m_nBoundaryVerts;
    delete [] m_boundaryExt;
    delete [] m_boundaries;

    m_nBoundaryVerts = temp1;
    m_boundaryExt = temp2;
    m_boundaries = temp3;
    m_maxBoundaries = newMaxBoundaries;

} // end: ResizeArrays()
