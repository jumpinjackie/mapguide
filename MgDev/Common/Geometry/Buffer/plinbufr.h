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
// FILE: plinbufr.h.
//
// PURPOSE: Declaration of the PolylineBuffer class. The PolylineBuffer object
//          is used to generate buffer zones around polyline map objects.
//
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//
// FILE: plinbufr.h.
//
// PURPOSE: Declaration of the PolylineBuffer class. The PolylineBuffer object
//          is used to generate buffer zones around polyline map objects.
//
//------------------------------------------------------------------------------

#ifndef _PLINBUFR_H_
#define _PLINBUFR_H_

#include "pobjbufr.h"

// class PolylineObject;
class OpsPolyline;
class OpsPolyPolyline;

class PolylineBuffer : public PolyObjectBuffer {
public:
    /// constructor/destructor

    //PolylineBuffer(const PolylineObject &polylineObj, BufferUtility *pBufferUtil);
    PolylineBuffer(const OpsPolyline &polyline, BufferUtility *pBufferUtil);
    PolylineBuffer(const OpsPolyPolyline &polyPolyline, BufferUtility *pBufferUtil);
    virtual ~PolylineBuffer();

    /// method to create the buffer zone polygon around the polyline object

    virtual void CreateBufferZone(ProgressCallback &callback,
        OrientedPolyPolygon &bufferPolygon);

protected:
    /// override of method to determine whether or not to accept a buffer zone
    /// polygon boundary

    virtual BOOL AcceptBoundary(const OpsFloatExtent &boundaryExt,
        Orientation boundaryOrient, const OpsFloatPoint &boundaryVert,
        const OpsDoublePoint &interiorPt) const;

    /// declarations of vertex accessor classes - one for each of PolylineObject,
    /// OpsPolyline, and OpsPolyPolyline

    //class PolylineObjVertexAccessor : public PolyObjectBuffer::VertexAccessor {
    //public:
    ///    PolylineObjVertexAccessor(const PolylineObject &polylineObj);

    ///    virtual int GetTotalVertices() const;
    ///    virtual int GetNPolyObjects() const;
    ///    virtual int GetNPolyVerts(int index) const;
    ///    virtual const OpsFloatPoint &operator[](int index) const;
    ///    virtual void GetExtent(OpsFloatExtent &extent) const;
    ///
    //private:
    ///    const PolylineObject &polylineObj;
    //};

    class OpsPolylineVertexAccessor : public PolyObjectBuffer::VertexAccessor {
    public:
        OpsPolylineVertexAccessor(const OpsPolyline &polyline);

        virtual int GetTotalVertices() const;
        virtual int GetNPolyObjects() const;
        virtual int GetNPolyVerts(int index) const;
        virtual const OpsFloatPoint &operator[](int index) const;
        virtual void GetExtent(OpsFloatExtent &extent) const;

    private:
        const OpsPolyline &polyline;

    };

    class OpsPolyPolylineVertexAccessor : public PolyObjectBuffer::VertexAccessor {
    public:
        OpsPolyPolylineVertexAccessor(const OpsPolyPolyline &polyPolyline);

        virtual int GetTotalVertices() const;
        virtual int GetNPolyObjects() const;
        virtual int GetNPolyVerts(int index) const;
        virtual const OpsFloatPoint &operator[](int index) const;
        virtual void GetExtent(OpsFloatExtent &extent) const;

    private:
        const OpsPolyPolyline &polyPolyline;

    };
};

#endif
