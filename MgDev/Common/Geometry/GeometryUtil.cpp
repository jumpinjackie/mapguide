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

#include "GeometryCommon.h"

//////////////////////////////////////////////////////////////////
// Read a coordinate object from a stream
//
MgCoordinate* MgGeometryUtil::ReadCoordinate(MgStream* stream, INT32 dimension)
{
    Ptr<MgCoordinate> coord;
    switch(dimension)
    {
        case MgCoordinateDimension::XY:
            coord = new MgCoordinateXY();
            break;
        case MgCoordinateDimension::XYZ:
            coord = new MgCoordinateXYZ();
            break;
        case MgCoordinateDimension::XY|MgCoordinateDimension::M:
            coord = new MgCoordinateXYM();
            break;
        case MgCoordinateDimension::XYZ|MgCoordinateDimension::M:
            coord = new MgCoordinateXYZM();
            break;
    }
    coord->Deserialize(stream);
    return SAFE_ADDREF((MgCoordinate*)coord);
}

//////////////////////////////////////////////////////////////////
// Write an MgCurveSegment object to a stream
//
void MgGeometryUtil::WriteCurveSegment(MgStream* stream, MgCurveSegment* segment)
{
    if(segment->GetComponentType() == MgGeometryComponentType::LinearSegment)
    {
        stream->WriteInt32(MgGeometryComponentType::LinearSegment);  // 130 == linear curve string element in AGF format
        MgLinearSegment* linearSeg = (MgLinearSegment*)&*segment;
        Ptr<MgCoordinateIterator> coords = linearSeg->GetCoordinates();
        stream->WriteInt32(coords->GetCount() - 1);
        coords->MoveNext(); //we already wrote the start position as part of the containing object or part of the previous segment
        while(coords->MoveNext())
        {
            Ptr<MgCoordinate> coord = coords->GetCurrent();
            coord->Serialize(stream);
        }
    }
    else if (segment->GetComponentType() == MgGeometryComponentType::ArcSegment)
    {
        stream->WriteInt32(MgGeometryComponentType::ArcSegment);  // 131 == circular arc curve string element in AGF format
        MgArcSegment* arcSegment = (MgArcSegment*)&*segment;
        Ptr<MgCoordinate> coord = arcSegment->GetControlCoordinate();
        coord->Serialize(stream);
        coord = arcSegment->GetEndCoordinate();
        coord->Serialize(stream);
    }
}

//////////////////////////////////////////////////////////////////
// write an MgCurveRing object to a stream
//
void MgGeometryUtil::WriteCurveRing(MgStream* stream, MgCurveRing* ring)
{
    //Start point
    Ptr<MgCurveSegment> segment = ring->GetSegment(0);
    Ptr<MgCoordinate> startCoord = segment->GetStartCoordinate();
    startCoord->Serialize(stream);

    //Num elements
    INT32 numSegments = ring->GetCount();
    stream->WriteInt32(numSegments);

    for(INT32 i = 0; i < numSegments; i++)
    {
        segment = ring->GetSegment(i);
        WriteCurveSegment(stream, segment);
    }
}

//////////////////////////////////////////////
// Write an MgLinearRing object to a stream
//
void MgGeometryUtil::WriteLinearRing(MgStream* stream, MgLinearRing* ring)
{
    Ptr<MgCoordinateCollection> coords = ring->GetCoordinateCollection();

    //Num coordinates
    INT32 numPoints = coords->GetCount();
    stream->WriteInt32(numPoints);

    //Coorindtaes
    for(INT32 i = 0; i < numPoints; i++)
    {
        Ptr<MgCoordinate> coord = coords->GetItem(i);
        coord->Serialize(stream);
    }
}

//////////////////////////////////////////////////////////////////
// Read an MgCurveSegment object from a stream
//
MgCurveSegment* MgGeometryUtil::ReadCurveSegment(MgStream* stream, MgCoordinate* startCoord, INT32 dimension)
{
    Ptr<MgCurveSegment> curveSeg;

    INT32 type;
    ((MgStreamReader*)stream)->GetInt32(type);

    if(type == MgGeometryComponentType::LinearSegment)   // 130 == linear curve string element in AGF format
    {
        INT32 numCoords;
        ((MgStreamReader*)stream)->GetInt32(numCoords);

        Ptr<MgCoordinateCollection> coords = new MgCoordinateCollection();
        coords->Add(startCoord);
        for(INT32 i = 0; i < numCoords; i++)
        {
            Ptr<MgCoordinate> coord = MgGeometryUtil::ReadCoordinate(stream, dimension);
            coords->Add(coord);
        }
        curveSeg = new MgLinearSegment(coords);
    }
    else if (type == MgGeometryComponentType::ArcSegment) // 131
    {
        Ptr<MgCoordinate> controlCoord = MgGeometryUtil::ReadCoordinate(stream, dimension);
        Ptr<MgCoordinate> endCoord = MgGeometryUtil::ReadCoordinate(stream, dimension);

        curveSeg = new MgArcSegment(startCoord, endCoord, controlCoord);
    }

    return SAFE_ADDREF((MgCurveSegment*)curveSeg);
}

//////////////////////////////////////////////////////////////////
// Read an ReadCurveRing object from a stream
//
MgCurveRing* MgGeometryUtil::ReadCurveRing(MgStream* stream, INT32 dimension)
{
    //Start point
    Ptr<MgCoordinate> startCoord = ReadCoordinate(stream, dimension);

    //Num elements
    INT32 numSegments;
    stream->GetInt32(numSegments);

    //segments
    Ptr<MgCurveSegmentCollection> segments = new MgCurveSegmentCollection();
    for(INT32 i = 0; i < numSegments; i++)
    {
        Ptr<MgCurveSegment> segment = ReadCurveSegment(stream, startCoord, dimension);
        segments->Add(segment);

        startCoord = segment->GetEndCoordinate();
    }

    return new MgCurveRing(segments);
}

//////////////////////////////////////////////
// Read an MgCurveSegment object to a stream
//
MgLinearRing* MgGeometryUtil::ReadLinearRing(MgStream* stream, INT32 dimension)
{
    Ptr<MgCoordinateCollection> coordinates = new MgCoordinateCollection;

    //NUm coordinates
    INT32 numPoints;
    stream->GetInt32(numPoints);

    //Coordinates
    for(INT32 i = 0; i < numPoints; i++)
    {
        Ptr<MgCoordinate> coord = ReadCoordinate(stream, dimension);
        coordinates->Add(coord);
    }
    return new MgLinearRing(coordinates);
}

//////////////////////////////////////////////
// Compute the envelope enclosing a line string
//
MgEnvelope* MgGeometryUtil::ComputeLineStringEnvelope(MgCoordinateCollection* coordinates)
{
    CHECKARGUMENTNULL(coordinates, L"MgGeometryUtil.ComputeLineStringEnvelope");

    Ptr<MgCoordinate> coord = coordinates->GetItem(0);
    double minX = coord->GetX();
    double minY = coord->GetY();
    double maxX = minX;
    double maxY = minY;

    for(INT32 i = 1; i < coordinates->GetCount(); i++)
    {
        coord = coordinates->GetItem(i);
        double x = coord->GetX();
        double y = coord->GetY();

        if(x < minX)
            minX = x;
        else if(x > maxX)
            maxX = x;

        if(y < minY)
            minY = y;
        else if(y > maxY)
            maxY = y;
    }

    Ptr<MgCoordinate> min = new MgCoordinateXY(minX, minY);
    Ptr<MgCoordinate> max = new MgCoordinateXY(maxX, maxY);
    return new MgEnvelope(min, max);
}

//////////////////////////////////////////////
// Compute the envelope enclosing a curve segment set
//
MgEnvelope* MgGeometryUtil::ComputeCurveSegmentsEnvelope(MgCurveSegmentCollection* segments)
{
    Ptr<MgCurveSegment> segment = segments->GetItem(0);
    MgEnvelope* envelope = segment->Envelope();

    for(INT32 i = 1; i < segments->GetCount(); i++)
    {
        segment = segments->GetItem(i);
        Ptr<MgEnvelope> segEnvelope = segment->Envelope();
        envelope->ExpandToInclude(segEnvelope);
    }
    return envelope;
}

//////////////////////////////////////////////
// Compute the envelope enclosing a geometry aggregate
//
MgEnvelope* MgGeometryUtil::ComputeAggregateEnvelope(MgAggregateGeometry* aggrGeometry)
{
    Ptr<MgGeometry> geometry = aggrGeometry->GetGeometryAt(0);
    MgEnvelope* envelope = geometry->Envelope();

    for(INT32 i = 1; i < aggrGeometry->GetCount(); i++)
    {
        geometry = aggrGeometry->GetGeometryAt(i);
        Ptr<MgEnvelope> geomEnvelope = geometry->Envelope();
        envelope->ExpandToInclude(geomEnvelope);
    }
    return envelope;
}

//////////////////////////////////////////////
// Compare a set of coordinates
//
bool MgGeometryUtil::CoordinatesEqual(MgCoordinateCollection* coords1, MgCoordinateCollection* coords2)
{
    INT32 count1 = coords1->GetCount();
    INT32 count2 = coords2->GetCount();

    if(count1 != count2)
        return false;

    Ptr<MgCoordinate> coord1;
    Ptr<MgCoordinate> coord2;

    //compare the collections in the same order
    INT32 i;
    for(i = 0; i < count1; i++)
    {
        coord1 = coords1->GetItem(i);
        coord2 = coords2->GetItem(i);
        if(!coord1->Equals(coord2))
            break;
    }

    if(i == count1)
        return true;

    //compare the collections in the reverse order
    INT32 j;
    for(i = 0, j = count2 - 1; i < count1; i++, j--)
    {
        coord1 = coords1->GetItem(i);
        coord2 = coords2->GetItem(2);
        if(!coord1->Equals(coord2))
            break;
    }

    if(i == count1)
        return true;

    return false;
}

//////////////////////////////////////////////
// Compare a set of curve segments
//
bool MgGeometryUtil::CurveSegmentsEqual(MgCurveSegmentCollection* segs1, MgCurveSegmentCollection* segs2)
{
    INT32 count1 = segs1->GetCount();
    INT32 count2 = segs2->GetCount();

    if(count1 != count2)
        return false;

    Ptr<MgCurveSegment> seg1;
    Ptr<MgCurveSegment> seg2;

    //compare the collections in the same order
    INT32 i;
    for(i = 0; i < count1; i++)
    {
        seg1 = segs1->GetItem(i);
        seg2 = segs2->GetItem(i);
        if(!seg1->Equals(seg2))
            break;
    }

    if(i == count1)
        return true;

    //compare the collections in the reverse order
    INT32 j;
    for(i = 0, j = count2 - 1; i < count1; i++, j--)
    {
        seg1 = segs1->GetItem(i);
        seg2 = segs2->GetItem(2);
        if(!seg1->Equals(seg2))
            break;
    }

    if(i == count1)
        return true;

    return false;
}

//////////////////////////////////////////////
// Compare two aggregate geometries
//
bool MgGeometryUtil::AggregatesEqual(MgAggregateGeometry* aggrGeom1, MgAggregateGeometry* aggrGeom2)
{
    if(aggrGeom1->GetGeometryType() != aggrGeom2->GetGeometryType())
        return false;

    int count1 = aggrGeom1->GetCount();
    int count2 = aggrGeom2->GetCount();

    if(count1 != count2)
        return false;

    //compare the geometries in any particular order
    Ptr<MgGeometryCollection> geometries = aggrGeom2->CopyCollection();

    for(INT32 i = 0; i < count1; i++)
    {
        Ptr<MgGeometry> geom1 = aggrGeom1->GetGeometryAt(i);

        INT32 j;
        for(j = 0; j < geometries->GetCount(); j++)
        {
            Ptr<MgGeometry> geom2 = geometries->GetItem(j);
            if(geom1->Equals(geom2))
            {
                //this ring is matched. remove it to avoid unnecessary comparisons
                geometries->RemoveAt(j);
                break;
            }
        }
        if(j == geometries->GetCount())
            break;  //unmatched geometry
    }

    int unmatched = geometries->GetCount();
    return unmatched == 0;
}
