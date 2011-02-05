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

//-----------------------------------------------------------------------------
//
// FILE: ppgunion.cpp.
//
// PURPOSE: Implementation of the OrientedPolyPolygonUnion class.
//
//-----------------------------------------------------------------------------

#include "Foundation.h"
#include "buffer.h"

//------------------------------------------------------------------------------
//
// METHOD: Constructor.
//
// PURPOSE: Construct and initialize an OrientedPolyPolygonUnion object.
//
// PARAMETERS:
//
//     None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to complete initialization.
//
//------------------------------------------------------------------------------

OrientedPolyPolygonUnion::OrientedPolyPolygonUnion(FloatTransform *transform) :
    m_polyPolygonRTree(NULL),
    m_unionAlreadyGenerated(FALSE)
{
    m_polyPolygonRTree = new OrientedPolyPolygonRTree;
    m_Transform = transform;

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

OrientedPolyPolygonUnion::~OrientedPolyPolygonUnion()
{
    delete m_polyPolygonRTree;

} // end: destructor


//------------------------------------------------------------------------------
//
// METHOD: CreateUnion().
//
// PURPOSE: Create the union of a list of input OrientedPolyPolygons.
//
// PARAMETERS:
//
//     Input:
//
//         inputPolyPolygons - passes a reference to a CObArray containing
//                             pointers to the list of input poly-polygons the
//                             union is to be derived for.
//         callback          - passes a pointer to a ProgressCallback object.
//                             The object is periodically notified of progress,
//                             and checked to determine if the operation has
//                             been canceled.
//
//     Output:
//
//         unionPolyPolygon  - passes a reference to an OrientedPolyPolygon
//                             object. The boundaries of the union poly-polygon
//                             are copied to the referenced object.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to create the union poly-polygon. A PlaneSweepException
//             is thrown if an error is detected during the execution of the
//             plane sweep algorithm.
//
//------------------------------------------------------------------------------

void OrientedPolyPolygonUnion::CreateUnion(const std::vector<OrientedPolyPolygon*> &inputPolyPolygons,
    ProgressCallback &callback, OrientedPolyPolygon &unionPolyPolygon)
{
    // re-initialize if necessary

    if (m_unionAlreadyGenerated) {
        PlaneSweep::Initialize();
        m_polyPolygonRTree->Initialize();
    }

    m_unionAlreadyGenerated = TRUE;

    // add the poly-polygon edges as input to the plane sweep operation, and
    // insert poly-polygon into the R-Tree
    int size = (int)inputPolyPolygons.size();
    for (int i = 0; i < size; i++)
    {
        const OrientedPolyPolygon *polyPolygon = inputPolyPolygons[i];
        AddPolyPolygonEdges(polyPolygon);
        OpsFloatExtent polyPolygonExt = polyPolygon->GetExtent();
        m_polyPolygonRTree->Insert(polyPolygon, &polyPolygonExt);
    }

    // perform the plane sweep operation to derive the union poly-polygon

    DoPlaneSweep(callback, unionPolyPolygon);

} // CreateUnion()

//------------------------------------------------------------------------------
//
// METHOD: AcceptBoundary().
//
// PURPOSE: Determine whether a potential polygon boundary should be included
//          as part of the union poly-polygon.
//
// PARAMETERS:
//
//     Input:
//
//         boundaryExt    - passes a reference to an OpsFloatExtent containing
//                          the extent of the boundary.
//         boundaryOrient - passes an enumerated value of type PlaneSweep::
//                          Orientation, specifying the orientation of the
//                          boundary.
//         boundaryVert   - passes a reference to an OpsFloatPoint. The refer-
//                          enced object contains the coordinates of a point
//                          that lies on the polygon boundary.
//         interiorPt     - passes a reference to a OpsDoublePoint containing
//                          the coordinates of a point that is strictly interior
//                          to the boundary.
//
//     Output:
//
//         None.
//
// RETURNS: TRUE if the polygon boundary should be accepted as part of the
//          output and FALSE otherwise.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BOOL OrientedPolyPolygonUnion::AcceptBoundary(const OpsFloatExtent &boundaryExt,
    Orientation boundaryOrient, const OpsFloatPoint &,
    const OpsDoublePoint &interiorPt) const
{
    // accept the boundary if the interior point is not contained within any of
    // the input boundaries

    BOOL strictContainment = boundaryOrient == CounterClockwise;
    return !m_polyPolygonRTree->BoundaryContained(boundaryExt, interiorPt,
        strictContainment);

} // end: AcceptBoundary()


//------------------------------------------------------------------------------
//
// METHOD: AddPolyPolygonEdges().
//
// PURPOSE: Add the edges of an OrientedPolyPolygon as input into the plane
//          sweep.
//
// PARAMETERS:
//
//     Input:
//
//         polyPolygon - passes a reference to the OrientedPolyPolygon object
//                       whose edges are to be added.
//
//     Output:
//
//         None.
//
// RETURNS: None.
//
// EXCEPTIONS: A CMemoryException is thrown if there is insufficient memory
//             available to add the edges of the poly-polygon.
//
//------------------------------------------------------------------------------

void OrientedPolyPolygonUnion::AddPolyPolygonEdges(
    const OrientedPolyPolygon *polyPolygon)
{
    for (int i = 0; i < polyPolygon->GetNBoundaries(); i++) {
        const OpsFloatPoint *vertices = polyPolygon->GetBoundary(i);
        int nVertices = polyPolygon->GetNBoundaryVertices(i);
        AddEdges(vertices, nVertices, Left);
    }

} // AddPolyPolygonEdges()


//------------------------------------------------------------------------------
//
// METHOD: OrientedPolyPolygonRTree::BoundaryContained().
//
// PURPOSE: Test whether a potential output boundary is contained by any of the
//          input poly-polygons.
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
//                             contained within any of the input polygons, and
//                             FALSE otherwise.
//
//     Output:
//
//         None.
//
// RETURNS: TRUE if the boundary is contained by one of the input poly-polgyons
//          and FALSE otherwise.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BOOL OrientedPolyPolygonUnion::OrientedPolyPolygonRTree::BoundaryContained(
    const OpsFloatExtent &boundaryExt, const OpsDoublePoint &interiorPt,
    BOOL strictContainment)
{
    // search the R-Tree, testing the interior point for containment within any
    // of the input polygons that contain the boundary extent

    BOOL boundaryContained = FALSE;

    m_extContainsExt.Initialize(boundaryExt, strictContainment);
    BeginSearch(&m_extContainsExt);

    OrientedPolyPolygon *polyPolygon = (OrientedPolyPolygon *)GetNextItem();

    while (polyPolygon != NULL) {
        if (polyPolygon->BoundaryContained(boundaryExt, interiorPt,
                strictContainment)) {
            boundaryContained = TRUE;
            break;
        }

        polyPolygon = (OrientedPolyPolygon *)GetNextItem();
    }

    return boundaryContained;

} // end: BoundaryContained()

//------------------------------------------------------------------------------
//
// METHOD: Initialize().
//
// PURPOSE: Initialize a "extent contains extent" predicate object. The pred-
//          icate tests an item extent to determine whether it contains the
//          boundary extent specified.
//
// PARAMETERS:
//
//     Input:
//
//         point     - pass the application coordinates of the point the ray
//                     emanates from.
//         rayToLeft - passes a boolean which is TRUE if the ray is from (x,y)
//                     to (-infinity,y); otherwise it is FALSE, and the ray is
//                     from (x,y) to (+infinity,y).
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

void OrientedPolyPolygonUnion::OrientedPolyPolygonRTree::ExtContainsExt::Initialize(
    const OpsFloatExtent &boundaryExt, BOOL strictContainment)
{
    m_boundaryExt = boundaryExt;
    m_strictContainment = strictContainment;

} // end: Initialize()


//------------------------------------------------------------------------------
//
// METHOD: Predicate().
//
// PURPOSE: Implementation of the predicate. The method returns TRUE if the
//          item extent (for either a node or a leaf item) contains the
//          boundary extent specified in the last call to Initialize().
//
// PARAMETERS:
//
//     Input:
//
//         itemExtent - passes a reference to the item extent to test.
//
//     Output:
//
//
// RETURNS: TRUE if the item extent contains the boundary extent.
//
// EXCEPTIONS: None.
//
//------------------------------------------------------------------------------

BOOL OrientedPolyPolygonUnion::OrientedPolyPolygonRTree::ExtContainsExt::Predicate(
    const OpsFloatExtent &itemExtent) const
{
    return itemExtent.Contains(&m_boundaryExt, m_strictContainment);

} // end: Predicate()
