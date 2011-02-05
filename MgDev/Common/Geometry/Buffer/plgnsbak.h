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
// FILE: plgnsbak.h.
//
// PURPOSE: Declaration of the PolygonSetback class. The PolygonSetback object
//          is used to generate setbacks inside polygon map objects, and poly-
//          gon primitives.
//
//------------------------------------------------------------------------------

#ifndef _PLGNSBAK_H_
#define _PLGNSBAK_H_

#include "plgnbufr.h"

/// \cond INTERNAL
class PolygonSetback : public PolygonBuffer {
public:
    /// constructors (explicit destructor not required)

    //PolygonSetback(const PolygonObject &polygonObj, BufferUtility *pBufferUtil);
    PolygonSetback(const OpsPolygon &polygon, BufferUtility *pBufferUtil);
    PolygonSetback(const OpsPolyPolygon &polyPolygon, BufferUtility *pBufferUtil);

    /// method to generate the setback polygon around the object specified at
    /// construction time

    void CreateSetback(ProgressCallback &callback,
        OrientedPolyPolygon &setbackPolygon);

protected:
    /// override of method to determine whether or not to accept a setback
    /// polygon boundary

    virtual BOOL AcceptBoundary(const OpsFloatExtent &boundaryExt,
        Orientation boundaryOrient, const OpsFloatPoint &boundaryVert,
        const OpsDoublePoint &interiorPt) const;

    /// override of method to generate the buffer zone

    virtual void GenerateBufferZone(const OpsFloatPoint vertices[],
        const int nPolyVerts[], int nPolyObjects, ProgressCallback &callback,
        OrientedPolyPolygon &bufferZone);
};
/// \endcond

#endif
