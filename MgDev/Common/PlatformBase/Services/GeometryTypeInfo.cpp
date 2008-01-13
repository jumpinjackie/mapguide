//
//  Copyright (C) 2007 by Autodesk, Inc.
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

#include "PlatformBase.h"
#include "FeatureService.h"
#include "GeometryTypeInfo.h"

MG_IMPL_DYNCREATE(MgGeometryTypeInfo)

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs a MgGeometryTypeInfo object
/// </summary>
MgGeometryTypeInfo::MgGeometryTypeInfo()
{
    Initialize();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the MgGeometryType value which specifies one of the
/// types of FGF geometry (Point, LineString, CurveString, ...)
/// </summary>
/// <param name="index">The index value of the type to get.
/// This must be in the range of zero to MgGeometryTypeInfo::GetCount()-1.
/// </param>
/// <returns>
/// Returns the geometry type at the given index.
/// </returns>
INT32 MgGeometryTypeInfo::GetType(INT32 index)
{
    return m_types[index];
}

/// <summary>
/// Gets the count of geometric types in the list.
/// </summary>
/// <returns>
/// Returns the count of geometric types in the list..
/// </returns>
INT32 MgGeometryTypeInfo::GetCount()
{
    return m_count;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the geometry type value list.  Elements must be integers
/// as defined by MgGeometryType.
/// <param name="types">The list of types; there must be no more than MG_MAX_GEOMETRY_TYPE_SIZE elements</param>
/// <param name="count">The number of types in the list; the maximum allowed value is MG_MAX_GEOMETRY_TYPE_SIZE</param>
void MgGeometryTypeInfo::SetTypes(INT32 * types, INT32 count)
{
    for (INT32 i = 0;  i < count && i < MG_MAX_GEOMETRY_TYPE_SIZE;  i++)
    {
        m_types[i] = types[i];
    }
}

void MgGeometryTypeInfo::Serialize(MgStream* stream)
{
    stream->WriteInt32(m_count);
    for (INT32 i = 0;  i < m_count && i < MG_MAX_GEOMETRY_TYPE_SIZE;  i++)
    {
        INT32 type = m_types[i];
        stream->WriteInt32(type);
    }
    stream->WriteString(m_serializedXml);
}

void MgGeometryTypeInfo::Deserialize(MgStream* stream)
{
    INT32 type = 0;
    stream->GetInt32(m_count);
    for (INT32 i = 0;  i < m_count && i < MG_MAX_GEOMETRY_TYPE_SIZE;  i++)
    {
        stream->WriteInt32(type);
        m_types[i] = type;
    }
    stream->GetString(m_serializedXml);
}

void MgGeometryTypeInfo::Initialize()
{
    m_count = 0;
    m_serializedXml = L"";
}

void MgGeometryTypeInfo::ToXml(string& xmlStr, bool includeType)
{
}
