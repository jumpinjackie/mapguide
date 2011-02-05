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

#ifndef _MG_BUFFER_H_
#define _MG_BUFFER_H_


class FloatTransform;
class ProgressCallback;

/// \cond INTERNAL
class MgBuffer
{
public:

    MgGeometry* CreateBuffer(MgGeometry* geometry, double offset, MgMeasure* measure);
    MgGeometryCollection* CreateBuffer(MgGeometryCollection* geometries, double offset, bool merge);

private: // internal

    class BufferParams
    {
    public:
        BufferParams()
        {
            progressCallback = NULL;
            transform = NULL;
            offset = 0.0f;
        }

        ~BufferParams()
        {
            delete progressCallback;
            delete transform;
        }

        ProgressCallback* progressCallback;
        FloatTransform* transform;
        float offset;
    };


    void BufferGeometry(BufferParams* bufferParams, MgGeometry* geometry,
        std::vector<OrientedPolyPolygon*>& bufferPolygons);

    MgPolygon* CreatePolygon(BufferParams * bufferParams,
        const OrientedPolyPolygon& orientedPolygon);

    void CreatePointBuffer(BufferParams* bufferParams, MgPoint* point,
        std::vector<OrientedPolyPolygon*>& bufferPolygons);

    void CreateLineStringBuffer(BufferParams* bufferParams, MgLineString* lineString,
        std::vector<OrientedPolyPolygon*>& bufferPolygons);

    void CreatePolygonBuffer(BufferParams* bufferParams, MgPolygon* polygon,
        std::vector<OrientedPolyPolygon*>& bufferPolygons);

    void CreateLinearRingBuffer(BufferParams* bufferParams, MgLinearRing* ring,
        std::vector<OrientedPolyPolygon*>& bufferPolygons);

    int CoordinateIteratorToFloatArray(BufferParams* bufferParams, MgCoordinateIterator* iter,
        OpsFloatPointArray& vertices, int& currentIndex);

    void CreateMultiLineStringBuffer(BufferParams* bufferParams, MgMultiLineString* multiLineString,
        std::vector<OrientedPolyPolygon*>& bufferPolygons);

    void CreateMultiPolygonBuffer(BufferParams* bufferParams, MgMultiPolygon* multiPolygon,
        std::vector<OrientedPolyPolygon*>& bufferPolygons);

    void CreateMultiGeometryBuffer(BufferParams* bufferParams, MgMultiGeometry* multiGeom,
        std::vector<OrientedPolyPolygon*>& bufferPolygons);

    void CreateMultiPointBuffer(BufferParams* bufferParams, MgMultiPoint* multiPoint,
        std::vector<OrientedPolyPolygon*>& bufferPolygons);

    void CreateCurveStringBuffer(BufferParams* bufferParams, MgCurveString* curveString,
        std::vector<OrientedPolyPolygon*>& bufferPolygons);

    void CreateCurvePolygonBuffer(BufferParams* bufferParams, MgCurvePolygon* polygon,
        std::vector<OrientedPolyPolygon*>& bufferPolygons);

    void CreateMultiCurveStringBuffer(BufferParams* bufferParams, MgMultiCurveString* multiCurveString,
        std::vector<OrientedPolyPolygon*>& bufferPolygons);

    void CreateMultiCurvePolygonBuffer(BufferParams* bufferParams, MgMultiCurvePolygon* multiCurvePolygon,
        std::vector<OrientedPolyPolygon*>& bufferPolygons);

    MgPolygon* CreateMgPolygon(BufferParams* bufferParams,
        std::vector<OrientedPolyPolygon*>& bufferPolygons);

    MgEnvelope* FindEnvelope(MgGeometryCollection* geometries, double offset);

    OrientedPolyPolygon* CreateOrientedPolyPolygon(BufferParams* bufferParams,
        std::vector<OrientedPolyPolygon*>& bufferPolygons);

    MgBuffer::BufferParams* CreateBufferParams(MgEnvelope* envelope, double offset);

    void ClearVector(std::vector<OrientedPolyPolygon*>& bufferPolygons);

    void CreateBuffer(MgGeometryCollection* geometries, BufferParams* bufferParams,
        std::vector<OrientedPolyPolygon*>& bufferPolygons);

    MgGeometryCollection* OrientedPolygonsToGeometries(BufferParams* bufferParams,
        std::vector<OrientedPolyPolygon*>& bufferPolygons, bool merge);

    MgLinearRing* CreateLinearRing(BufferParams* bufferParams,
        const OrientedPolyPolygon& orientedPolygon, INT32 index);

    void CoordinateToOpsFloatPoint(BufferParams* bufferParams,
        MgCoordinate* coord, OpsFloatPoint& floatPoint);

    int CurveSegmentToOpsFloatPointArray(BufferParams* bufferParams, MgCurveSegment* segment,
        OpsFloatPointArray& vertices, int& currIndex);

    int CurveRingToOpsFloatPointArray(BufferParams* bufferParams, MgCurveRing* ring,
        OpsFloatPointArray& vertices, int& currIndex);

    void CheckOpsFloatPointArray(OpsFloatPointArray& vertices, int& currIndex);

private:
    //// static void ExpandExtents(AWkbStream &dataStream, Box2D* extents);
    //static double Area(const OpsFloatPoint boundaryVerts __nogc[], int nVertices);
    //static System::Collections::ArrayList* FindSmallestContainingPolygon(Point2D point, System::Collections::ArrayList* polygonList, double areas __gc []);
    Ptr<MgMeasure> m_measure;
};
/// \endcond

#endif // _MG_BUFFER_H_
