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

MG_IMPL_DYNCREATE(MgMultiLineString)

//////////////////////////////////////////////////////////////////
// Construct a MgMultiLineString object
//
MgMultiLineString::MgMultiLineString(MgLineStringCollection* lineStrings)
{
    CHECKARGUMENTNULL(lineStrings, L"MgMultiLineString.MgMultiLineString");

    m_lineStrings = lineStrings->Copy();
}

//////////////////////////////////////////////////////////////////
// Construct an empty MgMultiLineString object for deserialization
//
MgMultiLineString::MgMultiLineString()
{
}

///////////////////////////////////////////////////////////////////////////
// Gets the number of geometries in this aggregate.
//
INT32 MgMultiLineString::GetCount()
{
    return m_lineStrings->GetCount();
}

///////////////////////////////////////////////////////////////////////////
// Gets the line string at the specified index. The indexing starts at
// zero.
//
MgLineString* MgMultiLineString::GetLineString(INT32 index)
{
    return m_lineStrings->GetItem(index);
}

///////////////////////////////////////////////////////////////////////////
// Gets the type of this geometry.
//
INT32 MgMultiLineString::GetGeometryType()
{
    return MgGeometryType::MultiLineString;
}

///////////////////////////////////////////////////////////////////////////
// Gets the geometric dimension of this entity, indicating whether the
// entity is zero, one, or two dimensional (defined by points, defined
// by curves, or defined by regions).
//
INT32 MgMultiLineString::GetDimension()
{
    return MgGeometricDimension::Curve;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this entity is empty or not.
//
bool MgMultiLineString::IsEmpty()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether the entities boundary encloses an area.
//
bool MgMultiLineString::IsClosed()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
// Returns a copy of this geometric entity.
//
MgGeometricEntity* MgMultiLineString::Copy()
{
    Ptr<MgLineStringCollection> lineStrings = m_lineStrings->Copy();
    return new MgMultiLineString(lineStrings);
}

///////////////////////////////////////////////////////////////////////////
// Returns a transformed copy of this geometric entity.
//
MgGeometricEntity* MgMultiLineString::Transform(MgTransform* transform)
{
    Ptr<MgLineStringCollection> lineStrings = new MgLineStringCollection();

    INT32 count = m_lineStrings->GetCount();
    for (int i = 0; i < count; i++)
    {
        Ptr<MgLineString> lineString = m_lineStrings->GetItem(i);
        Ptr<MgLineString> newLineString = (MgLineString*)lineString->Transform(transform);
        lineStrings->Add(newLineString);
    }

    return new MgMultiLineString(lineStrings);
}

//////////////////////////////////////////////////////////////////
// Serialize data to a stream
//
void MgMultiLineString::Serialize(MgStream* stream)
{
    //Serialize to AGF format
    stream->WriteInt32(MgGeometryType::MultiLineString);

    INT32 numLineStrings = m_lineStrings->GetCount();
    stream->WriteInt32(numLineStrings);
    for(INT32 i = 0; i < numLineStrings; i++)
    {
        Ptr<MgLineString> lineString = m_lineStrings->GetItem(i);
        lineString->Serialize(stream);
    }
}

//////////////////////////////////////////////////////////////////
// Deserialize data from a stream
//
void MgMultiLineString::Deserialize(MgStream* stream)
{
    MgStreamReader* reader = (MgStreamReader*)stream;

    //Deserialize from AGF format
    INT32 type;
    reader->GetInt32(type);
    assert(type == MgGeometryType::MultiLineString);

    INT32 numLineStrings;
    reader->GetInt32(numLineStrings);

    m_lineStrings = new MgLineStringCollection();
    for(INT32 i = 0; i < numLineStrings; i++)
    {
        Ptr<MgLineString> lineString = new MgLineString;
        lineString->Deserialize(stream);
        m_lineStrings->Add(lineString);
    }
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgMultiLineString::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////////////////////////
// Calculate the envelope for this geometry
//
MgEnvelope* MgMultiLineString::ComputeEnvelope()
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
void MgMultiLineString::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////
// Gets the geometry at the specified position
//
MgGeometry* MgMultiLineString::GetGeometryAt(INT32 index)
{
    return GetLineString(index);
}

STRING MgMultiLineString::ToAwkt(bool is2dOnly)
{
    STRING tempAwkt, coordDim;

    ToAwkt(tempAwkt, coordDim, is2dOnly);

    return L"MULTILINESTRING " + coordDim + tempAwkt;
}

void MgMultiLineString::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    awktStr += L"(";

    m_lineStrings->ToAwkt(awktStr, coordDim, is2dOnly);

    awktStr += L")";
}

MgCoordinateIterator* MgMultiLineString::GetCoordinates()
{
    Ptr<MgCoordinateCollection> coords = new MgCoordinateCollection();
    INT32 count = m_lineStrings->GetCount();

    for (int i = 0; i < count; i++)
    {
        Ptr<MgLineString> lineString = m_lineStrings->GetItem(i);
        Ptr<MgCoordinateIterator> lineIterator = lineString->GetCoordinates();

        while (lineIterator->MoveNext())
        {
            coords->Add(Ptr<MgCoordinate>(lineIterator->GetCurrent()));
        }
    }

    return new MgCoordinateIterator(coords);
}
