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
// FILE: opolygon.cpp.
//
// PURPOSE: Implementation of the OpsPolygon object.
//
//------------------------------------------------------------------------------

#include "Foundation.h"
#include "buffer.h"

//------------------------------------------------------------------------------
//
// METHOD: Default Constructor.
//
// PURPOSE: Constructs an empty polygon. The default constructor is
//          provided to prevent construction of uninitialized polygon
//          objects.
//
// PARAMETERS:
//
//     None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

OpsPolygon::OpsPolygon() :
    m_vertices(NULL),
    m_nVertices(0)
{
} // end: Constructor


//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize an OpsPolygon object from the
//          specified array of vertices. The polygon must be explicitly
//          closed.
//
// PARAMETERS:
//
//     Input:
//
//         vertices   - pointer to an array of polygon vertices. A
//                      copy is made of the array.
//         nVertices  - passes the number of vertices in the polygon.
//
//     Output:
//
//         polygonExt - If the pointer is non-null, then the extent of the
//                      polygon is copied to the location pointed to.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient
//             memory to construct the OpsPolygon object.
//
//------------------------------------------------------------------------------

OpsPolygon::OpsPolygon(const OpsFloatPoint vertices[], int nVertices,
    OpsFloatExtent *polygonExt) :
    m_vertices(NULL),
    m_nVertices(nVertices)
{
    // number of vertices must be greater than 3, and the  polygon
    // must be closed

//    assert(nVertices > 2);  // Some maps have degenerate polygons with 2 vertices !!
    assert(m_nVertices > 1);
    assert(vertices[0] == vertices[m_nVertices-1]);

    // allocate memory for vertices and make a copy

    size_t nBytes = m_nVertices * sizeof(OpsFloatPoint);
    m_vertices = reinterpret_cast<OpsFloatPoint *>(OpsObject::AllocMem(nBytes));
    CopyPoints(vertices, m_nVertices, m_vertices, polygonExt);

} // end: Constructor


//------------------------------------------------------------------------------
//
// METHOD: Copy Constructor.
//
// PURPOSE: Construct and initialize an OpsPolygon object, by making a
//          copy of another OpsPolygon.
//
// PARAMETERS:
//
//     Input:
//
//         polygon - passes a reference to the OpsPolygon to copy.
//
//     Output:
//
//         None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient
//             memory to construct the OpsPolygon object.
//
//------------------------------------------------------------------------------

OpsPolygon::OpsPolygon(const OpsPolygon &polygon) :
    m_vertices(NULL),
    m_nVertices(0)
{
    // use operator=() to make the copy

    (*this) = polygon;

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

OpsPolygon::~OpsPolygon()
{
    OpsObject::FreeMem(m_vertices);

} // end: Destructor

//------------------------------------------------------------------------------
//
// METHOD: GetExtent().
//
// PURPOSE: Determine the extent (bounding box) of the OpsPolygon object.
//
// PARAMETERS:
//
//     Input:
//
//         None.
//
//     Output:
//
//         polygonExt - passes a pointer to a OpsFloatExtent structure; the
//                      extent of the polygon is copied to the structure
//                      pointed to.
//
// RETURNS: Returns polygonExt.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

OpsFloatExtent *OpsPolygon::GetExtent(OpsFloatExtent *polygonExt) const
{
    // call method in OpsGraphicPrimitive base class to get the extent

    if (m_nVertices > 0)
        GetExtentOfPoints(m_vertices, m_nVertices, polygonExt);

    return polygonExt;

} // end: GetExtent()


//------------------------------------------------------------------------------
//
// METHOD: GetCentroid().
//
// PURPOSE: Determine the centroid of the OpsPolygon object.
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
//                    coordinates of the polygon centroid are copied to the
//                    structure pointed to.
//
// RETURNS: Returns centroid.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

OpsFloatPoint *OpsPolygon::GetCentroid(OpsFloatPoint *centroid) const
{
    // compute and return the weighted polygon centroid

    ::OpsPolygonCentroid(m_vertices, m_nVertices, centroid);
    return centroid;

} // end: GetCentroid()


//------------------------------------------------------------------------------
//
// METHOD: GetArea()
//
// PURPOSE: Get the area of the polygon.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: Returns the area of the polygon.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

double OpsPolygon::GetArea() const
{
    return ::OpsPolygonArea(m_vertices, m_nVertices);

} // end: GetArea()


//------------------------------------------------------------------------------
//
// METHOD: Clip().
//
// PURPOSE: Clip the polygon to the specified clip rectangle.
//
// PARAMETERS:
//
//     Input:
//
//         None.
//
//     Output:
//
//         clipRect   - passes a pointer to an OpsFloatExtent structure;
//                      the structure pointed to contains the coordinates
//                      of the corners of the clip rectangle.
//         vertices   - passes a pointer to an array of OpsFloatPoints. The
//                      vertices of the clipped polygon are copied to the
//                      array pointed to.
//         nPolyVerts - passes a pointer to an array of ints. The count of
//                      the number of vertices in the clipped result is
//                      copied to the first element of the array.
//         nPolygons  - passes a reference to an int. The referenced int
//                      is set to 1 if the clipped result is non-empty and
//                      0 otherwise.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void OpsPolygon::Clip(const OpsFloatExtent *clipRect, OpsFloatPoint vertices[],
    int nPolyVerts[], int &nPolygons) const
{
    ::OpsClipPolygon(*clipRect, m_vertices, m_nVertices, vertices,
        nPolyVerts[0]);

    // explicitly close the clipped result

    if (nPolyVerts[0] > 2) {
        vertices[nPolyVerts[0]++] = vertices[0];
        nPolygons = 1;
    }
    else
        nPolygons = 0;

} // end: Clip()

//------------------------------------------------------------------------------
//
// METHOD: operator[].
//
// PURPOSE: Return a reference to the indexed vertex of the OpsPolygon.
//          NOTE - if the caller intends to modify the contents of a vertex,
//          then care should be taken to make sure the first and last
//          vertices remain identical to preserve explicit closure.
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

OpsFloatPoint &OpsPolygon::operator[](int index) const
{
    // test for valid index in debug mode

    assert(index >= 0);
    assert(index < m_nVertices);

    return m_vertices[index];

} // end: operator[]()


//------------------------------------------------------------------------------
//
// METHOD: operator=().
//
// PURPOSE: Overloaded assignment operator to copy the specified OpsPolygon.
//
// PARAMETERS:
//
//     Input:
//
//         polygon - passes a reference to the OpsPolygon to copy.
//
//     Output:
//
//         None.
//
// RETURNS: Returns a reference to the OpsPolygon (the one assigned to).
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient
//             memory to resize the vertex array; in this case, the
//             polygon being assigned to is left unchanged.
//
//------------------------------------------------------------------------------

OpsPolygon &OpsPolygon::operator=(const OpsPolygon &polygon)
{
    // avoid special case of polygon = polygon

    if (this != &polygon)  {
        // if polygon being copied is not empty then copy the vertices

        OpsFloatPoint *oldVertices = m_vertices;

        if (polygon.m_nVertices > 0) {
            size_t nBytes = polygon.m_nVertices * sizeof(OpsFloatPoint);
            m_vertices = reinterpret_cast<OpsFloatPoint *>(OpsObject::AllocMem(nBytes));
            m_nVertices = polygon.m_nVertices;
            CopyPoints(polygon.m_vertices, m_nVertices, m_vertices, NULL);
        }
        else {
            m_nVertices = 0;
            m_vertices = NULL;
        }

        // free up the memory occupied by old vertices

        OpsObject::FreeMem(oldVertices);
    }

    return *this;

} // end: operator=()
