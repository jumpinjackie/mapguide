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
// FILE: oppolyln.cpp.
//
// PURPOSE: Implementation of the OpsPolyPolyline object.
//
//------------------------------------------------------------------------------

#include "Foundation.h"
#include "buffer.h"

//------------------------------------------------------------------------------
//
// METHOD: Default Constructor.
//
// PURPOSE: Constructs an empty poly-polyline. The default constructor is
//          implemented to prevent the construction of uninitialized poly-
//          polyline objects.
//
// PARAMETERS:
//
//     None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

OpsPolyPolyline::OpsPolyPolyline() :
    m_vertices(NULL),
    m_nPolyVerts(NULL),
    m_nPolylines(0)
{
} // end: Constructor


//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize an OpsPolyPolyline object from the
//          specified array of vertices, and array of vertex counts.
//
// PARAMETERS:
//
//     Input:
//
//         vertices     - pointer to an array of poly-polyline vertices. A
//                        copy is made of the array.
//         nPolyVerts   - passes an array of the counts of the number of
//                        vertices in each polyline.
//         nPolylines   - passes the number of polylines in the poly-polyline.
//
//     Output:
//
//         polyPlineExt - If the pointer is non-null, then the extent of the
//                        poly-polyline is copied to the location pointed to.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient
//             memory to construct the OpsPolyPolyline object.
//
//------------------------------------------------------------------------------

OpsPolyPolyline::OpsPolyPolyline(const OpsFloatPoint vertices[],
    const int nPolyVerts[], int nPolylines, OpsFloatExtent *polyPlineExt) :
    m_vertices(NULL),
    m_nPolyVerts(NULL),
    m_nPolylines(nPolylines)
{
    // must be at least one polyline

    assert(m_nPolylines > 0);

    // allocate memory for vertex count array, and make a copy

    size_t nBytes = m_nPolylines * sizeof(int);
    m_nPolyVerts = reinterpret_cast<int *>(OpsObject::AllocMem(nBytes));
    int i = 0, totalVerts = 0;
    for (i = 0, totalVerts = 0; i < m_nPolylines; i++) {
        // must be at least 2 points in each polyline

        assert(nPolyVerts[i] > 1);

        totalVerts += nPolyVerts[i];
        m_nPolyVerts[i] = nPolyVerts[i];
    }

    // allocate space for the vertices and copy them

    nBytes = totalVerts * sizeof(OpsFloatPoint);
    m_vertices = reinterpret_cast<OpsFloatPoint *>(OpsObject::AllocMem(nBytes));
    CopyPoints(vertices, totalVerts, m_vertices, polyPlineExt);

} // end: Constructor


//------------------------------------------------------------------------------
//
// METHOD: Copy Constructor.
//
// PURPOSE: Construct and initialize an OpsPolyPolyline object, by making a
//          copy of another OpsPolyPolyline.
//
// PARAMETERS:
//
//     Input:
//
//         polyPolyline - passes a reference to the OpsPolyPolyline to copy.
//
//     Output:
//
//         None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient
//             memory to construct the OpsPolyPolyline object.
//
//------------------------------------------------------------------------------

OpsPolyPolyline::OpsPolyPolyline(const OpsPolyPolyline &polyPolyline) :
    m_vertices(NULL),
    m_nPolyVerts(NULL),
    m_nPolylines(0)
{
    // use operator=() to make the copy

    (*this) = polyPolyline;

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

OpsPolyPolyline::~OpsPolyPolyline()
{
    OpsObject::FreeMem(m_nPolyVerts);
    OpsObject::FreeMem(m_vertices);

} // end: Destructor

//------------------------------------------------------------------------------
//
// METHOD: GetExtent().
//
// PURPOSE: Determine the extent (bounding box) of the OpsPolyPolyline
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
//         polyPlineExt - passes a pointer to a OpsFloatExtent structure;
//                        the extent of the poly-polyline is copied to the
//                        structure pointed to.
//
// RETURNS: Returns polyPlineExt.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

OpsFloatExtent *OpsPolyPolyline::GetExtent(OpsFloatExtent *polyPlineExt) const
{
    // take union of the extents of all polylines in the poly-polyline

    for (int i = 0, j = 0; i < m_nPolylines; i++) {
        OpsFloatExtent polylineExt;
        GetExtentOfPoints(&m_vertices[j], m_nPolyVerts[i], &polylineExt);

        if (i > 0)
            polyPlineExt->UnionWith(&polylineExt);
        else
            *polyPlineExt = polylineExt;

        j += m_nPolyVerts[i];
    }

    return polyPlineExt;

} // end: GetExtent()

//------------------------------------------------------------------------------
//
// METHOD: GetLength().
//
// PURPOSE: Compute the length of the poly-polyline.
//
// PARAMETERS:
//
//     None.
//
// RETURNS: Returns the length of the poly-polyline.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

double OpsPolyPolyline::GetLength() const
{
    double totalLength = 0.0;

    for (int i = 0, j = 0; i < m_nPolylines; i++)  {
        totalLength += ::OpsPolylineLength(&m_vertices[j], m_nPolyVerts[i]);
        j += m_nPolyVerts[i];
    }

    return totalLength;

} // end: GetLength()


//------------------------------------------------------------------------------
//
// METHOD: Clip().
//
// PURPOSE: Clip the poly-polyline primitive to the specified clip rectangle.
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
//                      vertices of the clipped poly-polyline are copied to
//                      the array pointed to.
//         nPolyVerts - passes a pointer to an array of ints. The counts of
//                      the number of vertices in each polyline are copied
//                      to the array.
//         nPolylines - passes a reference to an int. The number of polylines
//                      in the clipped result is copied to the referenced
//                      integer.
//
// RETURNS: None.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

void OpsPolyPolyline::Clip(const OpsFloatExtent *clipRect,
    OpsFloatPoint vertices[], int nPolyVerts[], int &nPolylines) const
{
    ::OpsClipPolyPolyline(*clipRect, m_vertices, m_nPolyVerts, m_nPolylines,
        vertices, nPolyVerts, nPolylines);

} // end: Clip()


//------------------------------------------------------------------------------
//
// METHOD: GetNPolyVerts().
//
// PURPOSE: Get the number of vertices in the specified polyline of the
//          poly-polyline.
//
// PARAMETERS:
//
//
//     Input:
//
//         index - passes the index of the polyline.
//
//     Output:
//
//         None.
//
// RETURNS: Returns the number of vertices in the specified polyline.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

int OpsPolyPolyline::GetNPolyVerts(int index) const
{
    // test for a valid index in debug mode

    assert(index >= 0);
    assert(index < m_nPolylines);

    return m_nPolyVerts[index];

} // end: GetNPolyVerts()


//------------------------------------------------------------------------------
//
// METHOD: GetTotalVertices().
//
// PURPOSE: Get the total number of vertices in the OpsPolyPolyline.
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

int OpsPolyPolyline::GetTotalVertices() const
{
    // sum the vertex counts
    int i = 0, totalVerts = 0;

    for (i = 0, totalVerts = 0; i < m_nPolylines; i++)
        totalVerts += m_nPolyVerts[i];

    return totalVerts;

} // end: GetTotalVerts()


//------------------------------------------------------------------------------
//
// METHOD: operator[].
//
// PURPOSE: Return a reference to the indexed vertex of the poly-polyline.
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

OpsFloatPoint & OpsPolyPolyline::operator[](int index) const
{
    // test for valid index in debug mode

    assert(index >= 0);
    assert(index < GetTotalVertices());

    return m_vertices[index];

} // end: operator[]()


//------------------------------------------------------------------------------
//
// METHOD: Concatenate().
//
// PURPOSE: Concatenate an OpsPolyline to the poly-polyline.
//
// PARAMETERS:
//
//     Input:
//
//         polyline - passes a pointer to the polyline to be concatenated.
//
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient
//             memory to resize the vertex array or vertex count array;
//             in this case the poly-polyline is left unchanged.
//
//------------------------------------------------------------------------------

void OpsPolyPolyline::Concatenate(const OpsPolyline *polyline)
{
    int nVerticesToAppend = polyline->GetNVertices();

    if (nVerticesToAppend > 0) {
        // resize vertex count and vertex arrays

        size_t nBytes = (m_nPolylines + 1) * sizeof(int);
        m_nPolyVerts = reinterpret_cast<int *>(OpsObject::ReallocMem(m_nPolyVerts, nBytes));
        int nVertices = GetTotalVertices();
        nBytes = (nVertices + nVerticesToAppend) * sizeof(OpsFloatPoint);
        m_vertices = reinterpret_cast<OpsFloatPoint *>(OpsObject::ReallocMem(m_vertices, nBytes));

        // concatentate the polyline vertices, and update counts

        for (int i = 0; i < nVerticesToAppend; i++, nVertices++)
            m_vertices[nVertices] = (*polyline)[i];

        m_nPolyVerts[m_nPolylines] = nVerticesToAppend;
        m_nPolylines++;
    }

} // end: Concatenate()


//------------------------------------------------------------------------------
//
// METHOD: operator=().
//
// PURPOSE: Overloaded assignment operator to copy an OpsPolyPolyline.
//
// PARAMETERS:
//
//     Input:
//
//         polyPolyline - passes a reference to the OpsPolyPolyline to copy.
//
//     Output:
//
//         None.
//
// RETURNS: Returns a reference to the OpsPolyPolyline (the one assigned
//          to).
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient
//             memory to resize the vertex array or vertex count array;
//             in this case the poly-polyline being assigned to is left
//             unchanged.
//
//------------------------------------------------------------------------------

OpsPolyPolyline &OpsPolyPolyline::operator=(const OpsPolyPolyline &polyPolyline)
{
    // avoid special case of polyPolyline = polyPolyline

    if (this != &polyPolyline) {
        // make the copy provided the poly-polyline being copied is not empty

        int *oldVertCounts = m_nPolyVerts;
        OpsFloatPoint *oldVertices = m_vertices;

        if (polyPolyline.m_nPolylines > 0) {
            // allocate memory for vertex count array, and make a copy

            size_t nBytes = polyPolyline.m_nPolylines * sizeof(int);
            int *nVerts = reinterpret_cast<int *>(OpsObject::AllocMem(nBytes));

            int i=0, totalVerts=0;

            for (i=0, totalVerts=0; i < polyPolyline.m_nPolylines; i++) {
                totalVerts += polyPolyline.m_nPolyVerts[i];
                nVerts[i] = polyPolyline.m_nPolyVerts[i];
            }

            // allocate space for the vertices and copy them - memory
            // exceptions are handled locally first and then passed on

            OpsFloatPoint *verts;

            MG_TRY()

            nBytes = totalVerts * sizeof(OpsFloatPoint);
            verts = reinterpret_cast<OpsFloatPoint *>(OpsObject::AllocMem(nBytes));
            CopyPoints(polyPolyline.m_vertices, totalVerts, verts, NULL);

            MG_CATCH(L"OpsPolyPolyline::operator=")

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

        if (m_nPolylines > 0) {
            OpsObject::FreeMem(oldVertCounts);
            OpsObject::FreeMem(oldVertices);
        }

        m_nPolylines = polyPolyline.m_nPolylines;
    }

    return *this;

} // end: operator=()
