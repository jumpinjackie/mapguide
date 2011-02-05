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
// FILE: opline.cpp.
//
// PURPOSE: Implementation of the OpsPolyline class.
//
//------------------------------------------------------------------------------

#include "Foundation.h"
#include "buffer.h"

//------------------------------------------------------------------------------
//
// METHOD: Default Constructor.
//
// PURPOSE: Construct an empty polyline. The default constructor is
//          implemented to prevent the construction of uninitialized
//          polyline objects.
//
// PARAMETERS:
//
//     None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

OpsPolyline::OpsPolyline() :
    m_vertices(NULL),
    m_nVertices(0)
{
} // end: Constructor


//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize an OpsPolyline object from the
//          specified array of vertices.
//
// PARAMETERS:
//
//     Input:
//
//         vertices  - passes the array of polyline vertices. A copy
//                     is made of the array.
//         nVertices - passes the number of vertices in the polyline.
//
//     Output:
//
//         pLineExt  - If the pointer is non-null, then the extent of the
//                     polyline is copied to the location pointed to.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient
//             memory to construct the OpsPolyline object.
//
//------------------------------------------------------------------------------

OpsPolyline::OpsPolyline(const OpsFloatPoint vertices[], int nVertices,
    OpsFloatExtent *pLineExt) :
    m_vertices(NULL),
    m_nVertices(nVertices)
{
    // number of vertices must be greater than 1

    assert(m_nVertices > 1);

    // allocate memory for vertices and make a copy

    size_t nBytes = nVertices * sizeof(OpsFloatPoint);
    m_vertices = reinterpret_cast<OpsFloatPoint *>(OpsObject::AllocMem(nBytes));
    CopyPoints(vertices, nVertices, m_vertices, pLineExt);

} // end: Constructor


//------------------------------------------------------------------------------
//
// METHOD: Copy Constructor.
//
// PURPOSE: Construct and initialize an OpsPolyline object, by making a
//          copy of another OpsPolyline.
//
// PARAMETERS:
//
//     Input:
//
//         polyline - passes a reference to the OpsPolyline to copy.
//
//     Output:
//
//         None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient
//             memory to construct the OpsPolyline object.
//
//------------------------------------------------------------------------------

OpsPolyline::OpsPolyline(const OpsPolyline &polyline) :
    m_vertices(NULL),
    m_nVertices(0)
{
    // use operator=() to make the copy

    (*this) = polyline;

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

OpsPolyline::~OpsPolyline()
{
    OpsObject::FreeMem(m_vertices);

} // end: Destructor

//------------------------------------------------------------------------------
//
// METHOD: GetExtent().
//
// PURPOSE: Determine the extent (bounding box) of the OpsPolyline object.
//
// PARAMETERS:
//
//     Input:
//
//         None.
//
//     Output:
//
//         pLineExt - passes a pointer to a OpsFloatExtent structure; the
//                    extent of the polyline is copied to the structure
//                    pointed to.
//
// RETURNS: Returns objExtent.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

OpsFloatExtent *OpsPolyline::GetExtent(OpsFloatExtent *pLineExt) const
{
    // call method in OpsGraphicPrimitive base class to get the extent

    if (m_nVertices > 0)
        GetExtentOfPoints(m_vertices, m_nVertices, pLineExt);

    return pLineExt;

} // end: GetExtent()

//------------------------------------------------------------------------------
//
// METHOD: GetLength().
//
// PURPOSE: Compute the length of the polyline.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: Returns the length of the polyline.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

double OpsPolyline::GetLength() const
{
    if (m_nVertices > 0)
        return ::OpsPolylineLength(m_vertices, m_nVertices);
    else
        return 0.0;

} // end: GetLength()


//------------------------------------------------------------------------------
//
// METHOD: Clip().
//
// PURPOSE: Clip the polyline to  the specified clip rectangle. The clipped
//          result is returned as a poly-polyline containing zero or more
//          polylines.
//
// PARAMETERS:
//
//     Input:
//
//         clipRect    - passes a pointer to an OpsFloatExtent structure; the
//                       structure pointed to contains the coordinates of the
//                       corners of the clip rectangle.
//
//     Output:
//
//         outVertices - pointer to an array of OpsFloatPoint structures;
//                       the vertices of the clipped result are copied to
//                       the array pointed to
//         nPolyVerts  - pointer to an array of integers. The number of
//                       vertices in each polyline of the poly-polyline are
//                       copied to the array elements.
//         nPolylines  - reference to an integer. The number of polylines
//                       in the clipped result is copied to the referenced
//                       integer.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void OpsPolyline::Clip(const OpsFloatExtent *clipRect, OpsFloatPoint outVerts[],
    int nPolyVerts[], int &nPolylines) const
{
    if (m_nVertices > 0)
        ::OpsClipPolyline(*clipRect, m_vertices, m_nVertices, outVerts,
            nPolylines, nPolyVerts);
    else
        nPolylines = 0;

} // end: Clip()


//------------------------------------------------------------------------------
//
// METHOD: operator[].
//
// PURPOSE: Return a reference to the indexed vertex of the OpsPolyline.
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

OpsFloatPoint &OpsPolyline::operator[](int index) const
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
// PURPOSE: Overloaded assignment operator to copy another OpsPolyline.
//
// PARAMETERS:
//
//     Input:
//
//         polyline - passes reference to the OpsPolyline to copy.
//
//     Output:
//
//         None.
//
// RETURNS:
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient
//             memory; in this case, the OpsPolyline is left unchanged
//
//------------------------------------------------------------------------------

OpsPolyline &OpsPolyline::operator=(const OpsPolyline &polyline)
{
    // avoid special case of polyline = polyline

    if (this != &polyline)  {
        // copy the vertices of the polyline if there are any

        OpsFloatPoint *oldVertices = m_vertices;

        if (polyline.m_nVertices > 0) {
            size_t nBytes = polyline.m_nVertices * sizeof(OpsFloatPoint);
            m_vertices = reinterpret_cast<OpsFloatPoint *>(
                OpsObject::AllocMem(nBytes));
            m_nVertices = polyline.m_nVertices;
            CopyPoints(polyline.m_vertices, m_nVertices, m_vertices, NULL);
        }
        else {
            m_nVertices = 0;
            m_vertices = NULL;
        }

        // free up old vertices (if any)

        OpsObject::FreeMem(oldVertices);
    }

    return *this;

} // end: operator=()
