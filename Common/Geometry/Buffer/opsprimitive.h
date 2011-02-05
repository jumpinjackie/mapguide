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

/// \ingroup Geometry_Module

#ifndef _OPSPRIMITIVE_H_
#define _OPSPRIMITIVE_H_

class OpsPrimitive
{
public:
    /// method to clip the polygon primitive to the specified clip rectangle
    virtual void Clip(const OpsFloatExtent *clipRect, OpsFloatPoint vertices[],
       int nPolyVerts[], int &nPolygons) const = 0;

    //------------------------------------------------------------------------------
    //
    /// METHOD: GetExtentOfPoints().
    //
    /// PURPOSE: Determine the extent (bounding box) of a list of points.
    //
    /// PARAMETERS
    //
    ///     Input:
    //
    ///         points  - passes a pointer to an array containing the coordinates
    ///                   of the points.
    ///         nPoints - passes the number of points.
    //
    ///     Output:
    //
    ///         extent  - passes a pointer to an OpsFloatExtent structure; the
    ///                   extent of the points is copied to the location pointed
    ///                   to.
    //
    // RETURNS: Returns extent.
    //
    // EXCEPTIONS: None.
    //------------------------------------------------------------------------------
    OpsFloatExtent* GetExtentOfPoints(const OpsFloatPoint points[],
        int nPoints, OpsFloatExtent *extent) const
    {
    /// must be at least one point

        assert(nPoints > 0);

        extent->Initialize(&points[0]);
        for (int i = 1; i < nPoints; i++)
            extent->Update(&points[i]);

        return extent;

    } // end: GetExtentOfPoints()


    //------------------------------------------------------------------------------
    //
    /// METHOD: CopyPoints().
    //
    /// PURPOSE: Copy an array of OpsFloatPoints to another array, optionally
    ///          deriving the extent of the points.
    //
    /// PARAMETERS
    //
    ///     Input:
    //
    ///         inPoints    - passes the array points to be copied.
    ///         nPoints     - passes the number of points.
    //
    ///     Output:
    //
    ///         outPoints   - passes the array the points are to be copied to
    ///         extOfPoints - passes a pointer to an OpsFloatExtent structure;
    ///                       if the pointer is non-NULL, then the extent of the
    ///                       points is copied to the location pointed to.
    //
    /// RETURNS: None.
    //
    /// EXCEPTIONS: None.
    //------------------------------------------------------------------------------
    void CopyPoints(const OpsFloatPoint inPoints[],
        int nPoints, OpsFloatPoint outPoints[], OpsFloatExtent *extOfPoints) const
    {
        if (extOfPoints) {
            assert(nPoints > 0);

            extOfPoints->Initialize(&inPoints[0]);
            outPoints[0] = inPoints[0];

            for (int i = 1; i < nPoints; i++) {
                outPoints[i] = inPoints[i];
                extOfPoints->Update(&inPoints[i]);
            }
        }
        else
            ::memcpy(outPoints, inPoints, nPoints * sizeof(OpsFloatPoint));

    } // end: CopyPoints()

};

#endif
