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
// FILE: oppolygn.cpp.
//
// PURPOSE: Implementation of the OpsPolyPolygon object.
//
//------------------------------------------------------------------------------

#include "Foundation.h"
#include "buffer.h"

//------------------------------------------------------------------------------
//
// METHOD: Default Constructor.
//
// PURPOSE: Constructs an empty poly-polygon. The default constructor is
//          implemented to prevent the construction of uninitialized poly-
//          polygon objects.
//
// PARAMETERS:
//
//     None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

OpsPolyPolygon::OpsPolyPolygon() :
    m_vertices(NULL),
    m_nPolyVerts(NULL),
    m_nPolygons(0)
{
} // end: Constructor


//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize an OpsPolyPolygon object from the
//          specified array of vertices, and array of vertex counts. Each
//          polygon must be explicitly closed.
//
// PARAMETERS:
//
//     Input:
//
//         vertices    - pointer to an array of poly-polygon vertices. A
//                       copy is made of the array.
//         nPolyVerts  - passes an array of the counts of the number of
//                       vertices in each polygon.
//         nPolygons   - passes the number of polygons in the poly-polygon.
//
//     Output:
//
//         polyPgonExt - If the pointer is non-null, then the extent of the
//                       poly-polygon is copied to the location pointed to.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient
//             memory to construct the OpsPolyPolygon object.
//
//------------------------------------------------------------------------------

OpsPolyPolygon::OpsPolyPolygon(const OpsFloatPoint vertices[],
    const int nPolyVerts[], int nPolygons, OpsFloatExtent *polyPgonExt) :
    m_vertices(NULL),
    m_nPolyVerts(NULL),
    m_nPolygons(nPolygons)
{
    // must be at least one polygon

    assert(m_nPolygons > 0);

    // allocate memory for vertex count array, and make a copy

    size_t nBytes = m_nPolygons * sizeof(int);
    m_nPolyVerts = reinterpret_cast<int *>(OpsObject::AllocMem(nBytes));

    int i = 0, totalVerts = 0;
    for (i = 0, totalVerts = 0; i < m_nPolygons; i++) {
        // each polygon must be explicitly closed

        assert(nPolyVerts[i] > 2);
        if ( !PERF_ADD_START_POINT_TO_LOOP )
        assert(vertices[totalVerts] == vertices[totalVerts+nPolyVerts[i]-1]);

        totalVerts += nPolyVerts[i];
        m_nPolyVerts[i] = nPolyVerts[i];
    }

    // allocate space for the vertices and copy them

    nBytes = totalVerts * sizeof(OpsFloatPoint);
    m_vertices = reinterpret_cast<OpsFloatPoint *>(OpsObject::AllocMem(nBytes));
    CopyPoints(vertices, totalVerts, m_vertices, polyPgonExt);

} // end: Constructor


//------------------------------------------------------------------------------
//
// METHOD: Copy Constructor.
//
// PURPOSE: Construct and initialize an OpsPolyPolygon object, by making a
//          copy of another OpsPolyPolygon.
//
// PARAMETERS:
//
//     Input:
//
//         polyPolygon - passes a reference to the OpsPolyPolygon to copy.
//
//     Output:
//
//         None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient
//             memory to construct the OpsPolyPolygon object.
//
//------------------------------------------------------------------------------

OpsPolyPolygon::OpsPolyPolygon(const OpsPolyPolygon &polyPolygon) :
    m_vertices(NULL),
    m_nPolyVerts(NULL),
    m_nPolygons(0)
{
    // use operator=() to make the copy

    (*this) = polyPolygon;

} // end: Copy Constructor


//------------------------------------------------------------------------------
//
// METHOD: Destructor.
//
// PURPOSE: Release all dynamically allocated memory.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

OpsPolyPolygon::~OpsPolyPolygon()
{
    OpsObject::FreeMem(m_nPolyVerts);
    OpsObject::FreeMem(m_vertices);

} // end: Destructor


//------------------------------------------------------------------------------
//
// METHOD: GetExtent().
//
// PURPOSE: Determine the extent (bounding box) of the OpsPolyPolygon
//          object.
//
// PARAMETERS:
//
//     Input:
//
//         None.
//
//     Output:
//
//         polyPgonExt - passes a pointer to a OpsFloatExtent structure;
//                       the extent of the poly-polygon is copied to the
//                       structure pointed to.
//
// RETURNS: Returns polyPgonExt.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

OpsFloatExtent *OpsPolyPolygon::GetExtent(OpsFloatExtent *polyPgonExt) const
{
    // take union of the extents of all polygons in the poly-polygon

    for (int i = 0, j = 0; i < m_nPolygons; i++) {
        OpsFloatExtent polygonExt;
        GetExtentOfPoints(&m_vertices[j], m_nPolyVerts[i], &polygonExt);
        if (i > 0)
            polyPgonExt->UnionWith(&polygonExt);
        else
            *polyPgonExt = polygonExt;
        j += m_nPolyVerts[i];
    }

    return polyPgonExt;

} // end: GetExtent()


//------------------------------------------------------------------------------
//
// METHOD: GetCentroid().
//
// PURPOSE: Determine the centroid of the OpsPolyPolygon object.
//
// PARAMETERS:
//
//     Input:
//
//         None.
//
//     Output:
//
//         centroid - passes a pointer to a OpsFloatPoint structure; the
//                    coordinates of the poly-polygon centroid are copied
//                    to the structure pointed to.
//
// RETURNS: Returns centroid.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

OpsFloatPoint *OpsPolyPolygon::GetCentroid(OpsFloatPoint *centroid) const
{
    // compute and return the poly-polygon centroid

    ::OpsPolyPolygonCentroid(m_vertices, m_nPolyVerts, m_nPolygons, centroid);
    return centroid;

} // end: GetCentroid()


//------------------------------------------------------------------------------
//
// METHOD: GetArea()
//
// PURPOSE: Get the area of the poly-polygon.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: Returns the area of the poly-polygon.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

double OpsPolyPolygon::GetArea() const
{
    return ::OpsPolyPolygonArea(m_vertices, m_nPolyVerts, m_nPolygons);

} // end: GetArea()


//------------------------------------------------------------------------------
//
// METHOD: Clip().
//
// PURPOSE: Clip the poly-polygon primitive to the specified clip rectangle.
//
// PARAMETERS:
//
//     Input:
//
//         clipRect   - passes a pointer to an OpsFloatExtent structure;
//                      the structure pointed to contains the coordinates
//                      of the corners of the clip rectangle.
//
//     Output:
//
//         vertices   - passes a pointer to an array of OpsFloatPoints. The
//                      vertices of the clipped polygon are copied to the
//                      array pointed to.
//         nPolyVerts - passes a pointer to an array of ints. The counts of
//                      the number of vertices in each polygon are copied
//                      to the array.
//         nPolygons  - passes a reference to an int. The number of polygons
//                      in the clipped result is copied to the referenced
//                      integer.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void OpsPolyPolygon::Clip(const OpsFloatExtent *clipRect,
    OpsFloatPoint vertices[], int nPolyVerts[], int &nPolygons) const
{
    ::OpsClipPolyPolygon(*clipRect, m_vertices, m_nPolyVerts, m_nPolygons,
        vertices, nPolyVerts, nPolygons);

} // end: Clip()

//------------------------------------------------------------------------------
//
// METHOD: GetNPolyVerts().
//
// PURPOSE: Get the number of vertices in the specified polygon of the
//          poly-polygon.
//
// PARAMETERS:
//
//
//     Input:
//
//         index - passes the index of the polygon.
//
//     Output:
//
//         None.
//
// RETURNS: Returns the number of vertices in the specified polygon.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

int OpsPolyPolygon::GetNPolyVerts(int index) const
{
    // test for a valid index in debug mode

    assert(index >= 0);
    assert(index < m_nPolygons);

    return m_nPolyVerts[index];

} // end: GetNPolyVerts()


//------------------------------------------------------------------------------
//
// METHOD: GetTotalVertices().
//
// PURPOSE: Get the total number of vertices in the OpsPolyPolygon.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: Returns the total number of vertices.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

int OpsPolyPolygon::GetTotalVertices() const
{
    // sum the vertex counts
    int i = 0, totalVerts = 0;
    for (i = 0, totalVerts = 0; i < m_nPolygons; i++)
        totalVerts += m_nPolyVerts[i];

    return totalVerts;

} // end: GetTotalVerts()


//------------------------------------------------------------------------------
//
// METHOD: operator[].
//
// PURPOSE: Return a reference to the indexed vertex of the poly-polygon.
//          NOTE - if the caller intends to modify the contents of a vertex,
//          then care should be taken to make sure polygon boundaries remain
//          explicitly closed.
//
// PARAMETERS:
//
//     Input:
//
//         index - passes the index of the vertex.
//
//     Output:
//
//         None.
//
// RETURNS: Returns a reference to the indexed vertex.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

OpsFloatPoint & OpsPolyPolygon::operator[](int index) const
{
    // test for valid index in debug mode

    assert(index >= 0);
    assert(index < GetTotalVertices());

    return m_vertices[index];

} // end: operator[]()


//------------------------------------------------------------------------------
//
// METHOD: operator=().
//
// PURPOSE: Overloaded assignment operator to copy an OpsPolyPolygon.
//
// PARAMETERS:
//
//     Input:
//
//         polyPolygon - passes a reference to the OpsPolyPolygon to copy.
//
//     Output:
//
//         None.
//
// RETURNS: Returns a reference to the OpsPolyPolygon (the one assigned to).
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient
//             memory to resize the vertex array or vertex count array;
//             in this case the poly-polygon being assigned to is left
//             unchanged.
//
//------------------------------------------------------------------------------

OpsPolyPolygon &OpsPolyPolygon::operator=(const OpsPolyPolygon &polyPolygon)
{
    // avoid special case of polyPolygon = polyPolygon

    if (this != &polyPolygon) {
        // make the copy provided the poly-polygon being copied is not empty

        int *oldVertCounts = m_nPolyVerts;
        OpsFloatPoint *oldVertices = m_vertices;

        if (polyPolygon.m_nPolygons > 0) {
            // allocate memory for vertex count array, and make a copy

            size_t nBytes = polyPolygon.m_nPolygons * sizeof(int);
            int *nVerts = reinterpret_cast<int *>(OpsObject::AllocMem(nBytes));

            int i = 0, totalVerts = 0;

            for (i = 0, totalVerts = 0; i < polyPolygon.m_nPolygons; i++) {
                totalVerts += polyPolygon.m_nPolyVerts[i];
                nVerts[i] = polyPolygon.m_nPolyVerts[i];
            }

            // allocate space for the vertices and copy them - memory
            // exceptions are handled locally first and then passed on

            OpsFloatPoint *verts;

            MG_TRY()

            nBytes = totalVerts * sizeof(OpsFloatPoint);
            verts = reinterpret_cast<OpsFloatPoint *>(OpsObject::AllocMem(nBytes));
            CopyPoints(polyPolygon.m_vertices, totalVerts, verts, NULL);

            MG_CATCH(L"OpsPolyPolygon.operator=")

            if (mgException != 0) // mgException is defined in MG_TRY() macro
            {
                OpsObject::FreeMem(nVerts);
            }

            MG_THROW();

            m_vertices = verts;
            m_nPolyVerts = nVerts;
        }
        else {
            m_vertices = NULL;
            m_nPolyVerts = NULL;
        }

        // free up the old vertex count and vertex arrays

        if (m_nPolygons > 0) {
            OpsObject::FreeMem(oldVertCounts);
            OpsObject::FreeMem(oldVertices);
        }

        m_nPolygons = polyPolygon.m_nPolygons;
    }

    return *this;

} // end: operator=()
