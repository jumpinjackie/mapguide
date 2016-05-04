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
#include <typeinfo>

MG_IMPL_DYNCREATE(MgMultiGeometry)

//////////////////////////////////////////////////////////////////
// Construct a MgMultiGeometry object
//
MgMultiGeometry::MgMultiGeometry(MgGeometryCollection* geometries)
{
    CHECKARGUMENTNULL(geometries, L"MgMultiGeometry.MgMultiGeometry");

    m_geometries = geometries->Copy();
}

//////////////////////////////////////////////////////////////////
// Construct an empty MgMultiGeometry object for deserialization
//
MgMultiGeometry::MgMultiGeometry()
{
}

///////////////////////////////////////////////////////////////////////////
// Gets the number of geometries in this aggregate.
//
INT32 MgMultiGeometry::GetCount()
{
    return m_geometries->GetCount();
}

///////////////////////////////////////////////////////////////////////////
// Gets the geometry at the specified index. The indexing starts at
// zero.
//
MgGeometry* MgMultiGeometry::GetGeometry(INT32 index)
{
    return m_geometries->GetItem(index);
}

///////////////////////////////////////////////////////////////////////////
// Gets the type of this geometry.
//
INT32 MgMultiGeometry::GetGeometryType()
{
    return MgGeometryType::MultiGeometry;
}

///////////////////////////////////////////////////////////////////////////
// Gets the geometric dimension of this entity, indicating whether the
// entity is zero, one, or two dimensional (defined by points, defined
// by curves, or defined by regions).
//
INT32 MgMultiGeometry::GetDimension()
{
    return MgGeometricDimension::Any;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this entity is empty or not.
//
bool MgMultiGeometry::IsEmpty()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether the entities boundary encloses an area.
//
bool MgMultiGeometry::IsClosed()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
// Returns a copy of this geometric entity.
//
MgGeometricEntity* MgMultiGeometry::Copy()
{
    Ptr<MgGeometryCollection> geometries = m_geometries->Copy();
    return new MgMultiGeometry(geometries);
}

///////////////////////////////////////////////////////////////////////////
// Returns a transformed copy of this geometric entity.
//
MgGeometricEntity* MgMultiGeometry::Transform(MgTransform* transform)
{
    Ptr<MgGeometryCollection> geometries = new MgGeometryCollection();

    INT32 count = m_geometries->GetCount();
    for (int i = 0; i < count; i++)
    {
        Ptr<MgGeometry> geometry = m_geometries->GetItem(i);
        Ptr<MgGeometry> newGeometry = (MgGeometry*)geometry->Transform(transform);
        geometries->Add(newGeometry);
    }

    return new MgMultiGeometry(geometries);
}

//////////////////////////////////////////////////////////////////
// Serialize data to a stream
//
void MgMultiGeometry::Serialize(MgStream* stream)
{
    //Serialize to AGF format

    Ptr<MgStreamHelper> streamHelper = stream->GetStreamHelper();
    bool tcpipStream = (typeid(*streamHelper.p) == typeid(MgAceStreamHelper));

    //Type
    stream->WriteInt32(MgGeometryType::MultiGeometry);

    //Num geometries
    INT32 numGeometries = m_geometries->GetCount();
    stream->WriteInt32(numGeometries);

    //Geometries
    for (INT32 i = 0; i < numGeometries; i++)
    {
        Ptr<MgGeometry> geometry = m_geometries->GetItem(i);
        if (tcpipStream)
        {
            //The geometry type is duplicated here, only in case of a TCP/IP stream
            stream->WriteInt32(geometry->GetGeometryType());
        }
        geometry->Serialize(stream);
    }
}

//////////////////////////////////////////////////////////////////
// Deserialize data from a stream
//
void MgMultiGeometry::Deserialize(MgStream* stream)
{
    //Deserialize from AGF format

    //Type
    INT32 type;
    stream->GetInt32(type);
    assert(type == MgGeometryType::MultiGeometry);

    //Num geometries
    INT32 numGeometries;
    stream->GetInt32(numGeometries);

    //Geometries
    Ptr<MgStreamHelper> streamHelper = stream->GetStreamHelper();
    bool tcpipStream = (typeid(*streamHelper.p) == typeid(MgAceStreamHelper));
    const type_info& streamType = typeid(*stream);

    m_geometries = new MgGeometryCollection();
    for (INT32 i = 0; i < numGeometries; i++)
    {
        INT32 geomType;
        if (tcpipStream)
            stream->GetInt32(geomType);
        else
        {
            // HACK!
            // get the geometry type without removing it from the stream
            if (streamType == typeid(MgStream))
            {
                // NOTE: in the case of MgStream we have to also read the packet
                //       header and argument type to get to the geometry type 
                UINT32 buffer[3];
//              UINT32 packetHeader = buffer[0];
//              UINT32 argumentType = buffer[1];
//              UINT32 data         = buffer[2];
                streamHelper->GetData(buffer, 3*sizeof(UINT32), true, true);
                assert(buffer[1] == (UINT32)MgPacketParser::matINT32);
                geomType = (INT32)buffer[2];
            }
            else
            {
                streamHelper->GetUINT32((UINT32&)geomType, true, true);
            }
        }

        // create the geometry based on its type and deserialize
        Ptr<MgGeometry> geometry = MgGeometryFactory::CreateGeometry(geomType);
        geometry->Deserialize(stream);
        m_geometries->Add(geometry);
    }
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgMultiGeometry::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////////////////////////
// Calculate the envelope for this geometry
//
MgEnvelope* MgMultiGeometry::ComputeEnvelope()
{
    if(m_envelope == NULL)
    {
        m_envelope = MgGeometryUtil::ComputeAggregateEnvelope(this);
    }
    return new MgEnvelope(m_envelope);
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgMultiGeometry::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////
// Gets the geometry at the specified position
//
MgGeometry* MgMultiGeometry::GetGeometryAt(INT32 index)
{
    return GetGeometry(index);
}

STRING MgMultiGeometry::ToAwkt(bool is2dOnly)
{
    STRING tempAwkt, coordDim;

    ToAwkt(tempAwkt, coordDim, is2dOnly);

    return L"GEOMETRYCOLLECTION " + tempAwkt;
}

void MgMultiGeometry::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    awktStr += L"(";

    m_geometries->ToAwkt(awktStr, coordDim, is2dOnly);

    awktStr += L")";
}

MgCoordinateIterator* MgMultiGeometry::GetCoordinates()
{
    Ptr<MgCoordinateCollection> coords = new MgCoordinateCollection();
    INT32 count = m_geometries->GetCount();

    for (INT32 i = 0; i < count; i++)
    {
        Ptr<MgGeometry> geom = m_geometries->GetItem(i);
        Ptr<MgCoordinateIterator> geomIterator = geom->GetCoordinates();

        while (geomIterator->MoveNext())
        {
            coords->Add(Ptr<MgCoordinate>(geomIterator->GetCurrent()));
        }
    }

    return new MgCoordinateIterator(coords);
}
