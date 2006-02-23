//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "AceCommon.h"
#include "buffer.h"

const int   POINT_BUFFER_SMOOTHNESS     = 64;
const int   POLYLINE_BUFFER_SMOOTHNESS  = 24;
const int   POLYGON_BUFFER_SMOOTHNESS   = 24;
const float MINIMUM_OFFSET              = 5.0f;

#define VALIDATE_NULL(x) \
    assert(x != NULL); \
    if (x == NULL) { return; }


//-------------------------------------------------------------------------
//
// METHOD: CreateBuffer().
//
// PURPOSE: Creates a buffer around the specified geometries. If the merge
// parameter is true, the resultant AWkbStream array represents a
// single buffer around all of the geometries, and the number of elements in
// the resultant AWkbStream array is 1.  Otherwise, the resultant
// AWkbStream array represents individual buffers around each geometry, and
// number of elements in the resultant AWkbStream array is the same as the
// number of elements in the supplied geometry array.
//
// PARAMETERS:
//
//     Input:
//
//         geometries  - A collection of geometries to buffer.
//         offset      - The offset distance for the buffer calculation
//                       in MapDefinition coordinate system units.  This
//                       offset should be zero or positive.
//         merge       - True if all input geometries are to be merged into
//                       a single BufferFeature, false otherwise.
//
// RETURNS: An AWkbStream array representing the buffer geometry.  Returns
//          null if the buffer cannot be created.
//
//-------------------------------------------------------------------------

MgGeometry* MgBuffer::CreateBuffer(MgGeometry* geometry, double offset, MgMeasure* measure)
{
    m_measure = SAFE_ADDREF((MgMeasure*)measure);

    Ptr<MgGeometryCollection> geomCol = new MgGeometryCollection();
    geomCol->Add(geometry);

    Ptr<MgGeometryCollection> geomCol1 = CreateBuffer(geomCol, offset, false);
    assert(geomCol1->GetCount() == 1);

    return geomCol1->GetItem(0);
}

MgGeometryCollection* MgBuffer::CreateBuffer(MgGeometryCollection* geometries, double offset, bool merge)
{
    Ptr<MgGeometryCollection> geomCol = (MgGeometryCollection*)NULL;

    //  check parameters
    if ( geometries == NULL || offset < 0.0 ||
         IsDoubleNan( offset ) || offset > DoubleMaxValue )
    {
        return NULL;
    }

    // Compute envelope for the entire collection
    Ptr<MgEnvelope> envelope = FindEnvelope(geometries, offset);
    assert(envelope != NULL);

    if ( envelope != NULL && !envelope->IsNull() )
    {
        std::vector<OrientedPolyPolygon*> bufferPolygons;

        // we need to delete the pointer returned at the end
        BufferParams* bufferParams = CreateBufferParams(envelope, offset);
        assert(bufferParams != NULL);

        // Create buffer around all geometries
        CreateBuffer(geometries, bufferParams, bufferPolygons);
        assert(bufferPolygons.size() > 0);

        // Convert oriented polygons to geometries
        geomCol = OrientedPolygonsToGeometries(bufferParams, bufferPolygons, merge);

        delete bufferParams;
        ClearVector(bufferPolygons);
    }

    return SAFE_ADDREF((MgGeometryCollection*)geomCol);
}


void MgBuffer::BufferGeometry(BufferParams* bufferParams, MgGeometry* geometry,
                              std::vector<OrientedPolyPolygon*>& bufferPolygons)
{
    VALIDATE_NULL(geometry);

    if (geometry != NULL)
    {
        INT32 type = geometry->GetGeometryType();
        switch (type)
        {
            case MgGeometryType::Point:
            {
                CreatePointBuffer(bufferParams, (MgPoint*)geometry, bufferPolygons);
                break;
            }
            case MgGeometryType::LineString:
            {
                CreateLineStringBuffer(bufferParams, (MgLineString*)geometry, bufferPolygons);
                break;
            }
            case MgGeometryType::Polygon:
            {
                CreatePolygonBuffer(bufferParams, (MgPolygon*)geometry, bufferPolygons);
                break;
            }
            case MgGeometryType::MultiLineString:
            {
                CreateMultiLineStringBuffer(bufferParams, (MgMultiLineString*)geometry, bufferPolygons);
                break;
            }
            case MgGeometryType::MultiPolygon:
            {
                CreateMultiPolygonBuffer(bufferParams, (MgMultiPolygon*)geometry, bufferPolygons);
                break;
            }
            case MgGeometryType::MultiPoint:
            {
                CreateMultiPointBuffer(bufferParams, (MgMultiPoint*)geometry, bufferPolygons);
                break;
            }
            case MgGeometryType::MultiGeometry:
            {
                CreateMultiGeometryBuffer(bufferParams, (MgMultiGeometry*)geometry, bufferPolygons);
                break;
            }
            case MgGeometryType::CurveString:
            {
                CreateCurveStringBuffer(bufferParams, (MgCurveString*)geometry, bufferPolygons);
                break;
            }
            case MgGeometryType::CurvePolygon:
            {
                CreateCurvePolygonBuffer(bufferParams, (MgCurvePolygon*)geometry, bufferPolygons);
                break;
            }
            case MgGeometryType::MultiCurveString:
            {
                CreateMultiCurveStringBuffer(bufferParams, (MgMultiCurveString*)geometry, bufferPolygons);
                break;
            }
            case MgGeometryType::MultiCurvePolygon:
            {
                CreateMultiCurvePolygonBuffer(bufferParams, (MgMultiCurvePolygon*)geometry, bufferPolygons);
                break;
            }
            default:
            {
                STRING buffer;
                MgUtil::Int32ToString(type, buffer);

                MgStringCollection arguments;
                arguments.Add(L"2");
                arguments.Add(buffer);

                throw new MgInvalidArgumentException(L"MgBuffer.BufferGeometry",
                    __LINE__, __WFILE__, &arguments, L"MgInvalidGeometryType", NULL);
            }
        }
    }
}

MgPolygon* MgBuffer::CreatePolygon(BufferParams* bufferParams, const OrientedPolyPolygon& orientedPolygon)
{
    MgGeometryFactory factory;
    Ptr<MgLinearRingCollection> innerRingCol = new MgLinearRingCollection();
    Ptr<MgLinearRing> outerRing = (MgLinearRing*)NULL;

    int numBoundraries = orientedPolygon.GetNBoundaries();
    for (int i = 0; i < numBoundraries; i++)
    {
        Ptr<MgLinearRing> lring = CreateLinearRing(bufferParams, orientedPolygon, i);
        if (i == 0)
        {
            outerRing = SAFE_ADDREF((MgLinearRing*)lring);
        }
        else
        {
            innerRingCol->Add(lring);
        }
    }

    return factory.CreatePolygon(outerRing, innerRingCol);
}

void MgBuffer::CreatePointBuffer(BufferParams* bufferParams, MgPoint* mgPoint,
                                 std::vector<OrientedPolyPolygon*>& bufferPolygons)
{
    VALIDATE_NULL(mgPoint);

    Ptr<MgCoordinate> coord = mgPoint->GetCoordinate();

    // We are 2D for now.
    double x = coord->GetX();
    double y = coord->GetY();
    // double z = is2D? 0.0 : stream.ReadDouble();

    BufferUtility* bufferUtil = NULL;
    BorderWalker* borderWalker = NULL;

    MgCoordinateSystemMeasure* newMeasure = dynamic_cast<MgCoordinateSystemMeasure*>(m_measure.p);
    if ((m_measure == NULL) || (newMeasure == NULL) || ((Ptr<MgCoordinateSystem>(newMeasure->GetCoordSys()))->GetType() == MgCoordinateSystemType::Arbitrary))
    {
        bufferUtil = new BufferUtility(POINT_BUFFER_SMOOTHNESS, bufferParams->offset);
    }
    else
    {
        borderWalker = new LatLonBorderWalker(bufferParams->transform, newMeasure);
        bufferUtil = new GreatCircleBufferUtil(POINT_BUFFER_SMOOTHNESS, bufferParams->offset,
            bufferParams->transform, borderWalker, newMeasure);
    }

    OpsFloatPoint point;
    OrientedPolyPolygon* bufferPolygon = new OrientedPolyPolygon();

    bufferParams->transform->Double2Float(x, y, point);

    bufferUtil->CreatePointBuffer(point, *bufferPolygon);

    bufferPolygons.push_back(bufferPolygon);

    if (bufferUtil != NULL)
        delete bufferUtil;

    if (borderWalker)
        delete borderWalker;

    return;
}


void MgBuffer::CreateCurveStringBuffer(BufferParams* bufferParams, MgCurveString* curveString,
                                       std::vector<OrientedPolyPolygon*>& bufferPolygons)
{
    VALIDATE_NULL(curveString);

    INT32 segCount = curveString->GetCount();
    OpsFloatPointArray vertices(0);

    int currIndex = 0;
    for (INT32 i = 0; i < segCount; i++)
    {
        CheckOpsFloatPointArray(vertices,currIndex);
        Ptr<MgCurveSegment> segment =  curveString->GetSegment(i);
        CurveSegmentToOpsFloatPointArray(bufferParams, segment, vertices, currIndex);
    }

    OpsPolyline opsPolyline(vertices.GetArray(), (int)currIndex);

    BufferUtility* bufferUtil = NULL;
    BorderWalker* borderWalker = NULL;

    MgCoordinateSystemMeasure* newMeasure = dynamic_cast<MgCoordinateSystemMeasure*>(m_measure.p);
    if ((m_measure == NULL) || (newMeasure == NULL) || (Ptr<MgCoordinateSystem>(newMeasure->GetCoordSys())->GetType() == MgCoordinateSystemType::Arbitrary))
    {
        bufferUtil = new BufferUtility(POLYLINE_BUFFER_SMOOTHNESS, bufferParams->offset);
    }
    else
    {
        borderWalker = new LatLonBorderWalker(bufferParams->transform, newMeasure);
        bufferUtil = new GreatCircleBufferUtil(POLYLINE_BUFFER_SMOOTHNESS, bufferParams->offset,
            bufferParams->transform, borderWalker, newMeasure);
    }
    PolylineBuffer lineBuffer(opsPolyline, bufferUtil);

    OrientedPolyPolygon* bufferPolygon = new OrientedPolyPolygon();
    lineBuffer.CreateBufferZone(*bufferParams->progressCallback, *bufferPolygon);

    bufferPolygons.push_back(bufferPolygon);

    if (bufferUtil != NULL)
        delete bufferUtil;

    if (borderWalker)
        delete borderWalker;

    return;
}


void MgBuffer::CreateLineStringBuffer(BufferParams* bufferParams, MgLineString* lineString,
                                      std::vector<OrientedPolyPolygon*>& bufferPolygons)
{
    VALIDATE_NULL(lineString);

    Ptr<MgCoordinateIterator> iterator = lineString->GetCoordinates();

    // This is back door entry to determine count
    INT32 nCoords = iterator->GetCount();
    OpsFloatPointArray vertices((int)nCoords);

    int i = 0;
    while (iterator->MoveNext())
    {
        Ptr<MgCoordinate> coord = iterator->GetCurrent();

        double x = coord->GetX();
        double y = coord->GetY();

        OpsFloatPoint floatPoint;
        bufferParams->transform->Double2Float(x, y, floatPoint);

        vertices[i] = floatPoint;
        i++;
    }

    OpsPolyline opsPolyline(vertices.GetArray(), (int)nCoords);

    BufferUtility* bufferUtil = NULL;
    BorderWalker* borderWalker = NULL;

    MgCoordinateSystemMeasure* newMeasure = dynamic_cast<MgCoordinateSystemMeasure*>(m_measure.p);
    if ((m_measure == NULL) || (newMeasure == NULL) || (Ptr<MgCoordinateSystem>(newMeasure->GetCoordSys())->GetType() == MgCoordinateSystemType::Arbitrary))
    {
        bufferUtil = new BufferUtility(POLYLINE_BUFFER_SMOOTHNESS, bufferParams->offset);
    }
    else
    {
        borderWalker = new LatLonBorderWalker(bufferParams->transform, newMeasure);
        bufferUtil = new GreatCircleBufferUtil(POLYLINE_BUFFER_SMOOTHNESS, bufferParams->offset,
            bufferParams->transform, borderWalker, newMeasure);
    }
    PolylineBuffer lineBuffer(opsPolyline, bufferUtil);

    OrientedPolyPolygon* bufferPolygon = new OrientedPolyPolygon();
    lineBuffer.CreateBufferZone(*bufferParams->progressCallback, *bufferPolygon);

    bufferPolygons.push_back(bufferPolygon);

    if (bufferUtil != NULL)
        delete bufferUtil;

    if (borderWalker)
        delete borderWalker;

    return;
}

void MgBuffer::CreatePolygonBuffer(BufferParams* bufferParams, MgPolygon* polygon,
                                   std::vector<OrientedPolyPolygon*>& bufferPolygons)
{
    VALIDATE_NULL(polygon);

    INT32 outerNumRings = 0;
    Ptr<MgLinearRing> outerRing = polygon->GetExteriorRing();
    if (outerRing != NULL)
        outerNumRings++;

    INT32 innerNumRings = polygon->GetInteriorRingCount();
    INT32 totalRings = outerNumRings + innerNumRings;

    OpsFloatPointArray vertices(0);
    OpsIntArray nPolyVerts((int)totalRings);

    // OuterRing
    int currentIndex = 0;
    Ptr<MgCoordinateIterator> iter = outerRing->GetCoordinates();
    int nCoords = CoordinateIteratorToFloatArray(bufferParams, iter, vertices, currentIndex);
    if (nCoords > 0)
    {
        nPolyVerts[0] = nCoords; // outerRing coordinates
    }

    // Inner ring
    for (int i = 0; i < innerNumRings; i++)
    {
        Ptr<MgLinearRing> innerRing = polygon->GetInteriorRing(i); // Get Ring
        Ptr<MgCoordinateIterator> iter1 = innerRing->GetCoordinates();
        nCoords = CoordinateIteratorToFloatArray(bufferParams, iter1, vertices, currentIndex); // Covert to OpsFloatPoint
        if (nCoords > 0)
        {
            nPolyVerts[i+1] = nCoords;
        }
    }

    if (vertices.GetMaxSize() > 0)
    {
        OpsPolyPolygon opsPolyPolygon(vertices.GetArray(), nPolyVerts.GetArray(), totalRings);

        BufferUtility* bufferUtil = NULL;
        BorderWalker* borderWalker = NULL;

        MgCoordinateSystemMeasure* newMeasure = dynamic_cast<MgCoordinateSystemMeasure*>(m_measure.p);
        if ((m_measure == NULL) || (newMeasure == NULL) || (Ptr<MgCoordinateSystem>(newMeasure->GetCoordSys())->GetType() == MgCoordinateSystemType::Arbitrary))
        {
            bufferUtil = new BufferUtility(POLYGON_BUFFER_SMOOTHNESS, bufferParams->offset);
        }
        else
        {
            borderWalker = new LatLonBorderWalker(bufferParams->transform, newMeasure);
            bufferUtil = new GreatCircleBufferUtil(POLYGON_BUFFER_SMOOTHNESS, bufferParams->offset,
                bufferParams->transform, borderWalker, newMeasure);
        }
        PolygonBuffer polygonBuffer(opsPolyPolygon, bufferUtil);

        OrientedPolyPolygon* bufferPolygon = new OrientedPolyPolygon();
        polygonBuffer.CreateBufferZone(*bufferParams->progressCallback, *bufferPolygon);

        bufferPolygons.push_back(bufferPolygon);

        if (bufferUtil != NULL)
            delete bufferUtil;

        if (borderWalker)
            delete borderWalker;
    }

    return;
}


void MgBuffer::CreateCurvePolygonBuffer(BufferParams* bufferParams, MgCurvePolygon* polygon,
                                        std::vector<OrientedPolyPolygon*>& bufferPolygons)
{
    VALIDATE_NULL(polygon);

    INT32 outerNumRings = 0;
    Ptr<MgCurveRing> outerRing = polygon->GetExteriorRing();
    if (outerRing != NULL)
        outerNumRings++;

    INT32 innerNumRings = polygon->GetInteriorRingCount();
    INT32 totalRings = outerNumRings + innerNumRings;

    OpsFloatPointArray vertices(0);
    OpsIntArray nPolyVerts((int)totalRings);

    // OuterRing
    int currentIndex = 0;
    int nCoords = CurveRingToOpsFloatPointArray(bufferParams, outerRing, vertices, currentIndex);
    if (nCoords > 0)
    {
        nPolyVerts[0] = nCoords; // outerRing coordinates
    }

    // Inner ring
    for (int i = 0; i < innerNumRings; i++)
    {
        Ptr<MgCurveRing> innerRing = polygon->GetInteriorRing(i); // Get Ring
        nCoords = CurveRingToOpsFloatPointArray(bufferParams, innerRing, vertices, currentIndex);
        if (nCoords > 0)
        {
            nPolyVerts[i+1] = nCoords;
        }
    }

    if (vertices.GetMaxSize() > 0)
    {
        OpsPolyPolygon opsPolyPolygon(vertices.GetArray(), nPolyVerts.GetArray(), totalRings);

        BufferUtility* bufferUtil = NULL;
        BorderWalker* borderWalker = NULL;

        MgCoordinateSystemMeasure* newMeasure = dynamic_cast<MgCoordinateSystemMeasure*>(m_measure.p);
        if ((m_measure == NULL) || (newMeasure == NULL) || (Ptr<MgCoordinateSystem>(newMeasure->GetCoordSys())->GetType() == MgCoordinateSystemType::Arbitrary))
        {
            bufferUtil = new BufferUtility(POLYGON_BUFFER_SMOOTHNESS, bufferParams->offset);
        }
        else
        {
            borderWalker = new LatLonBorderWalker(bufferParams->transform, newMeasure);
            bufferUtil = new GreatCircleBufferUtil(POLYGON_BUFFER_SMOOTHNESS, bufferParams->offset,
                bufferParams->transform, borderWalker, newMeasure);
        }
        PolygonBuffer polygonBuffer(opsPolyPolygon, bufferUtil);

        OrientedPolyPolygon* bufferPolygon = new OrientedPolyPolygon();
        polygonBuffer.CreateBufferZone(*bufferParams->progressCallback, *bufferPolygon);

        bufferPolygons.push_back(bufferPolygon);

        if (bufferUtil != NULL)
            delete bufferUtil;

        if (borderWalker)
            delete borderWalker;
    }

    return;
}


int MgBuffer::CoordinateIteratorToFloatArray(BufferParams* bufferParams, MgCoordinateIterator* iter,
                                             OpsFloatPointArray& vertices, int& currentIndex)
{
    assert(iter != NULL);
    if (iter == NULL) { return 0; }

    int noOfCoord = 0;
    while( iter->MoveNext() )
    {
        Ptr<MgCoordinate> coord = iter->GetCurrent();

        // Array is incremented in double the size specified
        CheckOpsFloatPointArray(vertices,currentIndex);

        double x = coord->GetX();
        double y = coord->GetY();

        OpsFloatPoint floatPoint;
        bufferParams->transform->Double2Float(x, y, floatPoint);
        vertices[currentIndex++] = floatPoint;
        noOfCoord++;
    }
    return noOfCoord;
}

void MgBuffer::CreateMultiLineStringBuffer(BufferParams* bufferParams, MgMultiLineString* multiLineString,
                                           std::vector<OrientedPolyPolygon*>& bufferPolygons)
{
    VALIDATE_NULL(multiLineString);

    int nPolylines = multiLineString->GetCount();

    OpsFloatPointArray vertices(0);
    OpsIntArray nPolyVerts(nPolylines);

    int currentIndex = 0;
    for (int i = 0; i < nPolylines; i++)
    {
        Ptr<MgLineString> lineString = multiLineString->GetLineString(i);
        Ptr<MgCoordinateIterator> iter = lineString->GetCoordinates();
        int nCoords = CoordinateIteratorToFloatArray(bufferParams, iter, vertices, currentIndex);
        if (nCoords > 0)
        {
            nPolyVerts[i] = nCoords;
        }
    }

    if (vertices.GetMaxSize() > 0)
    {
        OpsPolyPolyline opsPolyPolyline(vertices.GetArray(), nPolyVerts.GetArray(), nPolylines);

        BufferUtility* bufferUtil = NULL;
        BorderWalker* borderWalker = NULL;

        MgCoordinateSystemMeasure* newMeasure = dynamic_cast<MgCoordinateSystemMeasure*>(m_measure.p);
        if ((m_measure == NULL) || (newMeasure == NULL) || (Ptr<MgCoordinateSystem>(newMeasure->GetCoordSys())->GetType() == MgCoordinateSystemType::Arbitrary))
        {
            bufferUtil = new BufferUtility(POLYLINE_BUFFER_SMOOTHNESS, bufferParams->offset);
        }
        else
        {
            borderWalker = new LatLonBorderWalker(bufferParams->transform, newMeasure);
            bufferUtil = new GreatCircleBufferUtil(POLYLINE_BUFFER_SMOOTHNESS, bufferParams->offset,
                bufferParams->transform, borderWalker, newMeasure);
        }
        PolylineBuffer lineBuffer(opsPolyPolyline, bufferUtil);

        OrientedPolyPolygon* bufferPolygon = new OrientedPolyPolygon();
        lineBuffer.CreateBufferZone(*bufferParams->progressCallback, *bufferPolygon);

        bufferPolygons.push_back(bufferPolygon);

        if (bufferUtil != NULL)
            delete bufferUtil;

        if (borderWalker)
            delete borderWalker;
    }

    return;
}


void MgBuffer::CreateMultiPolygonBuffer(BufferParams* bufferParams, MgMultiPolygon* multiPolygon,
                                        std::vector<OrientedPolyPolygon*>& bufferPolygons)
{
    VALIDATE_NULL(multiPolygon)

    INT32 nPolygons = multiPolygon->GetCount();
    for (INT32 i = 0; i < nPolygons; i++)
    {
        Ptr<MgPolygon> polygon = multiPolygon->GetPolygon(i);
        CreatePolygonBuffer(bufferParams, polygon, bufferPolygons);
    }
}

void MgBuffer::CreateMultiCurveStringBuffer(BufferParams* bufferParams, MgMultiCurveString* multiCurveString,
                                            std::vector<OrientedPolyPolygon*>& bufferPolygons)
{
    VALIDATE_NULL(multiCurveString)

    INT32 nCurveStrings = multiCurveString->GetCount();
    for (INT32 i = 0; i < nCurveStrings; i++)
    {
        Ptr<MgCurveString> curveString = multiCurveString->GetCurveString(i);
        CreateCurveStringBuffer(bufferParams, curveString, bufferPolygons);
    }
}

void MgBuffer::CreateMultiCurvePolygonBuffer(BufferParams* bufferParams, MgMultiCurvePolygon* multiCurvePolygon,
                                             std::vector<OrientedPolyPolygon*>& bufferPolygons)
{
    VALIDATE_NULL(multiCurvePolygon)

    INT32 nCurvePolygons = multiCurvePolygon->GetCount();
    for (INT32 i = 0; i < nCurvePolygons; i++)
    {
        Ptr<MgCurvePolygon> curvePolygon = multiCurvePolygon->GetCurvePolygon(i);
        CreateCurvePolygonBuffer(bufferParams, curvePolygon, bufferPolygons);
    }
}


void MgBuffer::CreateMultiGeometryBuffer(BufferParams* bufferParams, MgMultiGeometry* multiGeom,
                                        std::vector<OrientedPolyPolygon*>& bufferPolygons)
{
    VALIDATE_NULL(multiGeom)

    INT32 num = multiGeom->GetCount();
    for (INT32 i = 0; i < num; i++)
    {
        Ptr<MgGeometry> currGeom = multiGeom->GetGeometry(i);
        BufferGeometry(bufferParams, currGeom, bufferPolygons);
    }
    return;
}

void MgBuffer::CreateMultiPointBuffer(BufferParams* bufferParams, MgMultiPoint* multiPoint,
                                      std::vector<OrientedPolyPolygon*>& bufferPolygons)
{
    VALIDATE_NULL(multiPoint)

    INT32 num = multiPoint->GetCount();
    for (INT32 i = 0; i < num; i++)
    {
        Ptr<MgPoint> currPoint = multiPoint->GetPoint(i);
        CreatePointBuffer(bufferParams, currPoint, bufferPolygons);
    }
    return;
}


MgEnvelope* MgBuffer::FindEnvelope(MgGeometryCollection* geometries, double offset)
{
    assert( geometries != NULL );
    if (geometries == NULL) { return NULL; }

    Ptr<MgEnvelope> envelope = new MgEnvelope();
    INT32 geomCnt = geometries->GetCount();

    for ( INT32 i = 0; i < geomCnt; i++ )
    {
        Ptr<MgGeometry> geometry = geometries->GetItem(i);
        if ( geometry != NULL )
        {
            Ptr<MgEnvelope> env = geometry->Envelope();
            envelope->ExpandToInclude(env);
        }
    }

    // Check to see if any feature geometry was added to the extents.
    if ( envelope->IsNull() )
    {
        return NULL;
    }

    // Expand extents by offset!!
    envelope->Grow( offset );

    return SAFE_ADDREF((MgEnvelope*)envelope);
}


MgPolygon* MgBuffer::CreateMgPolygon(BufferParams* bufferParams, std::vector<OrientedPolyPolygon*>& bufferPolygons)
{
    OrientedPolyPolygon* featurePolygon = CreateOrientedPolyPolygon(bufferParams, bufferPolygons);
    Ptr<MgPolygon> polygon = CreatePolygon(bufferParams, *featurePolygon);

    delete featurePolygon;
    return SAFE_ADDREF((MgPolygon*)polygon);
}

OrientedPolyPolygon* MgBuffer::CreateOrientedPolyPolygon(BufferParams* bufferParams, std::vector<OrientedPolyPolygon*>& bufferPolygons)
{
    OrientedPolyPolygon* featurePolygon = NULL;

    int cnt = (int)bufferPolygons.size();

    if (cnt > 0)
    {
        featurePolygon = new OrientedPolyPolygon();

        OrientedPolyPolygonUnion featureUnion;
        featureUnion.CreateUnion(bufferPolygons, *bufferParams->progressCallback, *featurePolygon);
    }

    return featurePolygon;
}


MgBuffer::BufferParams* MgBuffer::CreateBufferParams(MgEnvelope* envelope, double offset)
{
    BufferParams* bufferParams = new BufferParams();

    bufferParams->progressCallback = new ProgressCallback();
    bufferParams->transform = new FloatTransform(envelope);

    bufferParams->offset = bufferParams->transform->Double2Float(offset);
    bufferParams->offset = (bufferParams->offset >= MINIMUM_OFFSET) ? bufferParams->offset : MINIMUM_OFFSET;

    return bufferParams;
}

void MgBuffer::ClearVector(std::vector<OrientedPolyPolygon*>& bufferPolygons)
{
    int cnt =  (int)bufferPolygons.size();

    for (int i = 0; i < cnt; i++)
    {
        OrientedPolyPolygon* poly =  bufferPolygons[i];
        delete poly;
    }
    bufferPolygons.clear();
}


void MgBuffer::CreateBuffer(MgGeometryCollection* geometries, BufferParams* bufferParams, std::vector<OrientedPolyPolygon*>& bufferPolygons)
{
    VALIDATE_NULL(geometries)

    INT32 geomCnt = geometries->GetCount();
    for (INT32 i = 0; i < geomCnt; i++ )
    {
        Ptr<MgGeometry> geometry = geometries->GetItem(i);
        // Geometry is NULL, nothing to do
        if ( geometry == NULL) { continue; }

        // Geometry Type Point and offset is zero, nothing to do
        INT32 type = geometry->GetGeometryType();
        if ((type == MgGeometryType::Point) && (bufferParams->offset == 0))
        {
            continue;
        }

        // Calculate buffer for each geometry
        std::vector<OrientedPolyPolygon*> geometryPolygons;
        BufferGeometry(bufferParams, geometry, geometryPolygons);

        // Calculate combined PolyPolygon for the geometry ( A geometry can have multiple geometries )
        OrientedPolyPolygon* featurePolygon = CreateOrientedPolyPolygon(bufferParams, geometryPolygons);
        ClearVector(geometryPolygons); // remove all oriented poly instances

        bufferPolygons.push_back(featurePolygon);
    }
}

MgGeometryCollection* MgBuffer::OrientedPolygonsToGeometries(BufferParams* bufferParams, std::vector<OrientedPolyPolygon*>& bufferPolygons, bool merge)
{
    int cnt = (int)bufferPolygons.size();

    assert(cnt != 0);
    if (cnt == 0)  { return NULL; }

    // Create collection to add MgMultiPolygon objects
    Ptr<MgGeometryCollection> geomCol = new MgGeometryCollection();

    if (merge)
    {
        Ptr<MgPolygon> polygon = CreateMgPolygon(bufferParams, bufferPolygons);
        geomCol->Add(polygon);
    }
    else
    {
        for (int i = 0; i < cnt; i++)
        {
            OrientedPolyPolygon* bufferPolygon = bufferPolygons[i];
            if (bufferPolygon != NULL)
            {
                Ptr<MgPolygon> polygon = CreatePolygon(bufferParams, *bufferPolygon);
                geomCol->Add(polygon);
            }
        }
    }

    return SAFE_ADDREF((MgGeometryCollection*)geomCol);
}

MgLinearRing* MgBuffer::CreateLinearRing(BufferParams* bufferParams, const OrientedPolyPolygon& orientedPolygon, INT32 index)
{
    assert(index >= 0);

    MgGeometryFactory factory;

    int numVerts = orientedPolygon.GetNBoundaryVertices(index);
    const OpsFloatPoint* pts = orientedPolygon.GetBoundary(index);

    Ptr<MgCoordinateCollection> coordCol = new MgCoordinateCollection();

    for (int i = 0; i < numVerts; i++)
    {
        Ptr<MgCoordinate> coord = bufferParams->transform->Float2Double(pts[i]);
        coordCol->Add(coord);
    }

    return factory.CreateLinearRing(coordCol);
}

void MgBuffer::CoordinateToOpsFloatPoint(BufferParams* bufferParams, MgCoordinate* coord, OpsFloatPoint& floatPoint)
{
    VALIDATE_NULL(coord)

    double x = coord->GetX();
    double y = coord->GetY();

    bufferParams->transform->Double2Float(x, y, floatPoint);
}

int MgBuffer::CurveSegmentToOpsFloatPointArray(BufferParams* bufferParams, MgCurveSegment* segment,
                                                OpsFloatPointArray& vertices, int& currIndex)
{
    int noOfCoords = 0;
    if (segment == NULL) { return noOfCoords; }

    INT32 type = segment->GetComponentType();
    assert(( type == MgGeometryComponentType::ArcSegment )  ||
            ( type == MgGeometryComponentType::LinearSegment ) );

    // Strip arc into two linear segments
    if (type == MgGeometryComponentType::ArcSegment)
    {
        Ptr<MgArcSegment> arc = (MgArcSegment*)SAFE_ADDREF(segment);
        Ptr<MgCoordinate> coord1 = arc->GetStartCoordinate();
        OpsFloatPoint floatPoint1;
        CoordinateToOpsFloatPoint(bufferParams, coord1, floatPoint1);
        CheckOpsFloatPointArray(vertices,currIndex);
        vertices[currIndex++] = floatPoint1;
        noOfCoords++;

        Ptr<MgCoordinate> coord2 = arc->GetControlCoordinate();
        OpsFloatPoint floatPoint2;
        CoordinateToOpsFloatPoint(bufferParams, coord2, floatPoint2);
        CheckOpsFloatPointArray(vertices,currIndex);
        vertices[currIndex++] = floatPoint2;
        noOfCoords++;

        OpsFloatPoint floatPoint3;
        CoordinateToOpsFloatPoint(bufferParams, coord2, floatPoint3);
        CheckOpsFloatPointArray(vertices,currIndex);
        vertices[currIndex++] = floatPoint3;
        noOfCoords++;

        Ptr<MgCoordinate> coord4 = arc->GetEndCoordinate();
        OpsFloatPoint floatPoint4;
        CoordinateToOpsFloatPoint(bufferParams, coord4, floatPoint4);
        CheckOpsFloatPointArray(vertices,currIndex);
        vertices[currIndex++] = floatPoint4;
        noOfCoords++;
    }

    if (type == MgGeometryComponentType::LinearSegment)
    {
        Ptr<MgLinearSegment> arc = (MgLinearSegment*)SAFE_ADDREF(segment);
        Ptr<MgCoordinate> coord1 = arc->GetStartCoordinate();
        OpsFloatPoint floatPoint1;
        CoordinateToOpsFloatPoint(bufferParams, coord1, floatPoint1);
        CheckOpsFloatPointArray(vertices,currIndex);
        vertices[currIndex++] = floatPoint1;
        noOfCoords++;

        Ptr<MgCoordinate> coord4 = arc->GetEndCoordinate();
        OpsFloatPoint floatPoint4;
        CoordinateToOpsFloatPoint(bufferParams, coord4, floatPoint4);
        CheckOpsFloatPointArray(vertices,currIndex);
        vertices[currIndex++] = floatPoint4;
        noOfCoords++;
    }

    return noOfCoords;
}

int MgBuffer::CurveRingToOpsFloatPointArray(BufferParams* bufferParams, MgCurveRing* ring,
                                            OpsFloatPointArray& vertices, int& currIndex)
{
    int noOfCoords = 0;
    if (ring == NULL) { return noOfCoords; }

    INT32 cnt = ring->GetCount();

    for (INT32 i = 0; i < cnt; i++)
    {
        CheckOpsFloatPointArray(vertices,currIndex);
        Ptr<MgCurveSegment> segment = ring->GetSegment(i);
        noOfCoords += CurveSegmentToOpsFloatPointArray(bufferParams,segment,vertices,currIndex);
    }

    return noOfCoords; // no of co-ordinates in a ring
}

void MgBuffer::CheckOpsFloatPointArray(OpsFloatPointArray& vertices, int& currIndex)
{
    if (currIndex >= vertices.GetMaxSize())
    {
        // In case of arc segment we might have two line strings
        vertices.ResizeArray(currIndex+4);
    }
}
