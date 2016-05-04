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

//------------------------------------------------------------------------------
//
// FILE: oextent.h.
//
// PURPOSE: Declaration for the OpsExtent<> template. Typedefs are provided for
//          OpsFloatExtent, OpsDoubleExtent, and OpsIntegerExtent.
//
//------------------------------------------------------------------------------

#ifndef _OEXTENT_H_
#define _OEXTENT_H_

//------------------------------------------------------------------------------
//
// Template: OpsExtent<>.
//
// Purpose - Stores coordinates of a lower-left and upper-right corners of
//     a bounding box using the specified numeric (scalar) type.
//
//------------------------------------------------------------------------------

template <class ScalarType> struct OpsExtent {
    ScalarType xMin;
    ScalarType yMin;
    ScalarType xMax;
    ScalarType yMax;

    OpsExtent();
    OpsExtent(ScalarType x1, ScalarType y1, ScalarType x2, ScalarType y2);
    void Initialize(ScalarType x, ScalarType y);
    void Initialize(const OpsPoint<ScalarType> *point);
    void Update(ScalarType x, ScalarType y);
    void Update(const OpsPoint<ScalarType> *point);
    void Enlarge(ScalarType xSize, ScalarType ySize);
    BOOL Intersects(const OpsExtent<ScalarType> *box) const;
    BOOL Contains(const OpsPoint<ScalarType> *point) const;
    BOOL Contains(const OpsExtent<ScalarType> *box,
        BOOL strictContainment=FALSE) const;
    BOOL IntersectWith(const OpsExtent<ScalarType> *box);
    void UnionWith(const OpsExtent<ScalarType> *box);
    ScalarType Width() const;
    ScalarType Height() const;
    double Area() const;
};


//------------------------------------------------------------------------------
//
// OpsExtent<>::OpsExtent() - Construct and initialize an OpsExtent<> to all
//     zeros.
//
//------------------------------------------------------------------------------

template <class ScalarType> OpsExtent<ScalarType>::OpsExtent() :
    xMin(0),
    yMin(0),
    xMax(0),
    yMax(0)
{
}


//------------------------------------------------------------------------------
//
// OpsExtent<>::OpsExtent() - Construct and initialize an OpsExtent from the
//     coordinates of diagonally opposite corners.
//
//------------------------------------------------------------------------------

template <class ScalarType>
inline OpsExtent<ScalarType>::OpsExtent(ScalarType x1, ScalarType y1,
    ScalarType x2, ScalarType y2)
{
    if (x1 <= x2) {
        xMin = x1;
        xMax = x2;
    }
    else {
        xMin = x2;
        xMax = x1;
    }

    if (y1 <= y2) {
        yMin = y1;
        yMax = y2;
    }
    else {
        yMin = y2;
        yMax = y1;
    }
}


//------------------------------------------------------------------------------
//
// OpsExtent<>::Initialize() - Initialize the coordinates of the lower-left and
//     upper-right corners of the OpsExtent with the given coordinates.
//
//------------------------------------------------------------------------------

template <class ScalarType> inline void OpsExtent<ScalarType>::Initialize(
    ScalarType x, ScalarType y)
{
    xMin = xMax = x;
    yMin = yMax = y;
}


//------------------------------------------------------------------------------
//
// OpsExtent<>::Initialize() - Initialize the coordinates of the lower-left and
//     upper-right corners of the OpsExtent with the coordinates of the given
//     point.
//
//------------------------------------------------------------------------------

template <class ScalarType> inline void OpsExtent<ScalarType>::Initialize(
    const OpsPoint<ScalarType> *point)
{
    Initialize(point->x, point->y);
}


//------------------------------------------------------------------------------
//
// OpsExtent<>::Update() - Update the coordinates lower-left and upper-right
//     corners of the OpsExtent<> with the given coordinates.
//
//------------------------------------------------------------------------------

template <class ScalarType> inline void OpsExtent<ScalarType>::Update(
    ScalarType x, ScalarType y)
{
    if (x < xMin)
        xMin = x;
    else if (x > xMax)
        xMax = x;

    if (y < yMin)
        yMin = y;
    else if (y > yMax)
        yMax = y;
}


//------------------------------------------------------------------------------
//
// OpsExtent<>::Update() - Update the coordinates lower-left and upper-right
//     corners of the OpsExtent<> with the coordinates of the given point.
//
//------------------------------------------------------------------------------

template <class ScalarType> inline void OpsExtent<ScalarType>::Update(
    const OpsPoint<ScalarType> *point)
{
    Update(point->x, point->y);
}


//------------------------------------------------------------------------------
//
// OpsExtent<>::Enlarge() - Enlarge an extent in the x and y directions by the
//     specified amounts.
//
//------------------------------------------------------------------------------

template <class ScalarType> inline void OpsExtent<ScalarType>::Enlarge(
    ScalarType xSize, ScalarType ySize)
{
    xMin -= xSize;
    yMin -= ySize;
    xMax += xSize;
    yMax += ySize;
}


//------------------------------------------------------------------------------
//
// OpsExtent<>::Intersects() - Determine whether this OpsExtent<> intersects the
//     specified OpsExtent<>, returning TRUE if so, and FALSE otherwise.
//
//------------------------------------------------------------------------------

template <class ScalarType> inline BOOL OpsExtent<ScalarType>::Intersects(
    const OpsExtent<ScalarType> *box) const
{
    return xMin <= box->xMax && xMax >= box->xMin &&
           yMin <= box->yMax && yMax >= box->yMin;
}


//------------------------------------------------------------------------------
//
// OpsExtent<>::Contains() - Determine whether this OpsExtent<> contains the
//     specified OpsPoint<>, returning TRUE if so, and FALSE otherwise.
//
//------------------------------------------------------------------------------

template <class ScalarType> inline BOOL OpsExtent<ScalarType>::Contains(
    const OpsPoint<ScalarType> *point) const
{
    return point->x >= xMin && point->x <= xMax &&
           point->y >= yMin && point->y <= yMax;
}


//------------------------------------------------------------------------------
//
// OpsExtent<>::Contains() - Determine whether this OpsExtent<> contains the
//     specified OpsExtent<>, returning TRUE if so, and FALSE otherwise.
//
//------------------------------------------------------------------------------

template <class ScalarType> inline BOOL OpsExtent<ScalarType>::Contains(
    const OpsExtent<ScalarType> *box, BOOL strictContainment) const
{
    if (strictContainment)
        return xMin < box->xMin && xMax > box->xMax &&
               yMin < box->yMin && yMax > box->yMax;
    else
        return xMin <= box->xMin && xMax >= box->xMax &&
               yMin <= box->yMin && yMax >= box->yMax;
}


//------------------------------------------------------------------------------
//
// OpsExtent<>::IntersectWith() - Intersect this OpsExtent<> with the specified
//     OpsExtent<> and store the result back to this OpsExtent<>. Returns TRUE
//     if the intersection is non-empty and FALSE otherwise.
//
//------------------------------------------------------------------------------

template <class ScalarType> inline BOOL OpsExtent<ScalarType>::IntersectWith(
    const OpsExtent<ScalarType> *box)
{
    BOOL intersects = Intersects(box);

    if (intersects) {
        if (xMin < box->xMin)
            xMin = box->xMin;

        if (xMax > box->xMax)
            xMax = box->xMax;

        if (yMin < box->yMin)
            yMin = box->yMin;

        if (yMax > box->yMax)
            yMax = box->yMax;
    }

    return intersects;
}


//------------------------------------------------------------------------------
//
// OpsExtent<>::UnionWith() - Generate the "union" of this OpsExtent<> and the
//     specified OpsExtent<> and store the result back to this OpsExtent<>. The
//     union of the two bounding boxes is the smallest bounding box that
//     encloses both of the original bounding boxes.
//
//------------------------------------------------------------------------------

template <class ScalarType> inline void OpsExtent<ScalarType>::UnionWith(
    const OpsExtent<ScalarType> *box)
{
    if (xMin > box->xMin)
        xMin = box->xMin;

    if (xMax < box->xMax)
        xMax = box->xMax;

    if (yMin > box->yMin)
        yMin = box->yMin;

    if (yMax < box->yMax)
        yMax = box->yMax;
}


//------------------------------------------------------------------------------
//
// OpsExtent<>::Width() - Compute the width of the bounding box.
//
//------------------------------------------------------------------------------

template <class ScalarType> inline ScalarType OpsExtent<ScalarType>::Width()
    const
{
    return xMax - xMin;
}


//------------------------------------------------------------------------------
//
// OpsExtent<>::Height() - Compute the height of the bounding box.
//
//------------------------------------------------------------------------------

template <class ScalarType> inline ScalarType OpsExtent<ScalarType>::Height()
    const
{
    return yMax - yMin;
}


//------------------------------------------------------------------------------
//
// OpsExtent<>::Area() - Compute the area enclosed by the bounding box.
//
//------------------------------------------------------------------------------

template <class ScalarType> inline double OpsExtent<ScalarType>::Area() const
{
    return static_cast<double>(Width()) * static_cast<double>(Height());
}


// typedefs for commonly used extent types

typedef OpsExtent<float> OpsFloatExtent;
typedef OpsExtent<double> OpsDoubleExtent;
typedef OpsExtent<int> OpsIntegerExtent;

#endif
