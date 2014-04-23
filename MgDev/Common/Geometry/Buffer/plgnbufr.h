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
// FILE: plgnbufr.h.
//
// PURPOSE: Declaration of the PolygonBuffer class. The PolygonBuffer object is
//          used to generate buffer zones around polygon map objects, and poly-
//          gon primitives.
//
//------------------------------------------------------------------------------

#ifndef _PLGNBUFR_H_
#define _PLGNBUFR_H_

#include "pobjbufr.h"

// class PolygonObject;
class OpsPolygon;
class OpsPolyPolygon;

class PolygonBuffer : public PolyObjectBuffer {
public:
    /// constructors/destructor

    //PolygonBuffer(const PolygonObject &polygonObj, BufferUtility *pBufferUtil);
    PolygonBuffer(const OpsPolygon &polygon, BufferUtility *pBufferUtil);
    PolygonBuffer(const OpsPolyPolygon &polyPolygon, BufferUtility *pBufferUtil);
    virtual ~PolygonBuffer();

protected:
    /// implementation methods

    virtual BOOL AcceptBoundary(const OpsFloatExtent &boundaryExt,
        Orientation boundaryOrient, const OpsFloatPoint &boundaryVert,
        const OpsDoublePoint &interiorPt) const;
    BOOL PointInPolygon(const OpsDoublePoint &point) const;

    /// declarations of vertex accessor classes - one for each of PolygonObject,
    /// OpsPolygon, and OpsPolyPolygon

    //class PolygonObjVertexAccessor : public PolyObjectBuffer::VertexAccessor {
    //public:
    ///    PolygonObjVertexAccessor(const PolygonObject &polygonObj);

    ///    virtual int GetTotalVertices() const;
    ///    virtual int GetNPolyObjects() const;
    ///    virtual int GetNPolyVerts(int index) const;
    ///    virtual const OpsFloatPoint &operator[](int index) const;
    ///    virtual void GetExtent(OpsFloatExtent &extent) const;
    ///
    //private:
    ///    const PolygonObject &polygonObj;
    //};

    class OpsPolygonVertexAccessor : public PolyObjectBuffer::VertexAccessor {
    public:
        OpsPolygonVertexAccessor(const OpsPolygon &polygon);

        virtual int GetTotalVertices() const;
        virtual int GetNPolyObjects() const;
        virtual int GetNPolyVerts(int index) const;
        virtual const OpsFloatPoint &operator[](int index) const;
        virtual void GetExtent(OpsFloatExtent &extent) const;

    private:
        const OpsPolygon &polygon;

    };

    class OpsPolyPolygonVertexAccessor : public PolyObjectBuffer::VertexAccessor {
    public:
        OpsPolyPolygonVertexAccessor(const OpsPolyPolygon &polyPolygon);

        virtual int GetTotalVertices() const;
        virtual int GetNPolyObjects() const;
        virtual int GetNPolyVerts(int index) const;
        virtual const OpsFloatPoint &operator[](int index) const;
        virtual void GetExtent(OpsFloatExtent &extent) const;

    private:
        const OpsPolyPolygon &polyPolygon;

    };

};

#endif
