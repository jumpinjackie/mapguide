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

MG_IMPL_DYNCREATE(MgMultiCurveString)

//////////////////////////////////////////////////////////////////
// Construct a MgMultiCurveString object
//
MgMultiCurveString::MgMultiCurveString(MgCurveStringCollection* curveStrings)
{
    CHECKARGUMENTNULL(curveStrings, L"MgMultiCurveString.MgMultiCurveString");

    m_curveStrings = curveStrings->Copy();
}

//////////////////////////////////////////////////////////////////
// Construct an empty MgMultiCurveString object for deserialization
//
MgMultiCurveString::MgMultiCurveString()
{
}

///////////////////////////////////////////////////////////////////////////
// Gets the number of geometries in this aggregate.
//
INT32 MgMultiCurveString::GetCount()
{
    return m_curveStrings->GetCount();
}

///////////////////////////////////////////////////////////////////////////
// Gets the curve string at the specified index. The indexing starts at
// zero.
//
MgCurveString* MgMultiCurveString::GetCurveString(INT32 index)
{
    return m_curveStrings->GetItem(index);
}

///////////////////////////////////////////////////////////////////////////
// Gets the type of this geometry.
//
INT32 MgMultiCurveString::GetGeometryType()
{
    return MgGeometryType::MultiCurveString;
}

///////////////////////////////////////////////////////////////////////////
// Gets the geometric dimension of this entity, indicating whether the
// entity is zero, one, or two dimensional (defined by points, defined
// by curves, or defined by regions).
//
INT32 MgMultiCurveString::GetDimension()
{
    return MgGeometricDimension::Curve;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether this entity is empty or not.
//
bool MgMultiCurveString::IsEmpty()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
// Tests whether the entities boundary encloses an area.
//
bool MgMultiCurveString::IsClosed()
{
    return false;
}

///////////////////////////////////////////////////////////////////////////
// Returns a copy of this geometric entity.
//
MgGeometricEntity* MgMultiCurveString::Copy()
{
    Ptr<MgCurveStringCollection> curveStrings = m_curveStrings->Copy();
    return new MgMultiCurveString(curveStrings);
}

///////////////////////////////////////////////////////////////////////////
// Returns a transformed copy of this geometric entity.
//
MgGeometricEntity* MgMultiCurveString::Transform(MgTransform* transform)
{
    Ptr<MgCurveStringCollection> curveStrings = new MgCurveStringCollection();

    INT32 count = m_curveStrings->GetCount();
    for (int i = 0; i < count; i++)
    {
        Ptr<MgCurveString> curveString = m_curveStrings->GetItem(i);
        Ptr<MgCurveString> newCurveString = (MgCurveString*)curveString->Transform(transform);
        curveStrings->Add(newCurveString);
    }

    return new MgMultiCurveString(curveStrings);
}

//////////////////////////////////////////////////////////////////
// Serialize data to a stream
//
void MgMultiCurveString::Serialize(MgStream* stream)
{
    //Serialize to AGF format
    stream->WriteInt32(MgGeometryType::MultiCurveString);

    INT32 numCurveStrings = m_curveStrings->GetCount();
    stream->WriteInt32(numCurveStrings);
    for(INT32 i = 0; i < numCurveStrings; i++)
    {
        Ptr<MgCurveString> curveString = m_curveStrings->GetItem(i);
        curveString->Serialize(stream);
    }
}

//////////////////////////////////////////////////////////////////
// Deserialize data from a stream
//
void MgMultiCurveString::Deserialize(MgStream* stream)
{
    MgStreamReader* reader = (MgStreamReader*)stream;

    //Deserialize from AGF format
    INT32 type;
    reader->GetInt32(type);
    assert(type == MgGeometryType::MultiCurveString);

    INT32 numCurveStrings;
    reader->GetInt32(numCurveStrings);

    m_curveStrings = new MgCurveStringCollection();
    for(INT32 i = 0; i < numCurveStrings; i++)
    {
        Ptr<MgCurveString> curveString = new MgCurveString;
        curveString->Deserialize(stream);
        m_curveStrings->Add(curveString);
    }
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgMultiCurveString::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////////////////////////
// Calculate the envelope for this geometry
//
MgEnvelope* MgMultiCurveString::ComputeEnvelope()
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
void MgMultiCurveString::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////
// Gets the geometry at the specified position
//
MgGeometry* MgMultiCurveString::GetGeometryAt(INT32 index)
{
    return GetCurveString(index);
}


STRING MgMultiCurveString::ToAwkt(bool is2dOnly)
{
    STRING tempAwkt, coordDim;

    ToAwkt(tempAwkt, coordDim, is2dOnly);

    return L"MULTICURVESTRING " + coordDim + tempAwkt;
}

void MgMultiCurveString::ToAwkt(REFSTRING awktStr, REFSTRING coordDim, bool is2dOnly)
{
    awktStr += L"(";

    m_curveStrings->ToAwkt(awktStr, coordDim, is2dOnly);

    awktStr += L")";
}

MgCoordinateIterator* MgMultiCurveString::GetCoordinates()
{
    Ptr<MgCoordinateCollection> coords = new MgCoordinateCollection();
    INT32 count = m_curveStrings->GetCount();

    for (INT32 i = 0; i < count; i++)
    {
        Ptr<MgCurveString> curveString = m_curveStrings->GetItem(i);
        Ptr<MgCoordinateIterator> stringIterator = curveString->GetCoordinates();

        while (stringIterator->MoveNext())
        {
            coords->Add(Ptr<MgCoordinate>(stringIterator->GetCurrent()));
        }
    }

    return new MgCoordinateIterator(coords);
}
