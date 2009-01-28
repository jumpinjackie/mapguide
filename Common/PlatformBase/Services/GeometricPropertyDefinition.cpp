//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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
#include "GeometricPropertyDefinition.h"

MG_IMPL_DYNCREATE(MgGeometricPropertyDefinition)

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs a MgGeometricPropertyDefinition object
/// </summary>
/// <param name="name">Name of the property</param>
MgGeometricPropertyDefinition::MgGeometricPropertyDefinition(CREFSTRING name) : MgPropertyDefinition(name, MgFeaturePropertyType::GeometricProperty)
{
    Initialize();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Geometric Types that can be stored in this geometric property.
/// The returned value may be any combination of the values from the
/// MgFeatureGeometricType enumeration combined via a bit-wise or operation.</summary>
/// <returns>
/// Returns the MgFeatureGeometricType that can be stored in this geometric property
/// </returns>
INT32 MgGeometricPropertyDefinition::GetGeometryTypes()
{
    return m_geometricTypes;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the specific geometry types that can be stored in this geometric
/// property. The returned value is a list of geometry types that are
/// supported.  The caller does NOT own the array of types and should not free its memory.
/// <returns>
/// Returns a list of geometry types that are supported.
/// </returns>
MgGeometryTypeInfo * MgGeometricPropertyDefinition::GetSpecificGeometryTypes()
{
    return SAFE_ADDREF(m_geometryTypeInfo.p);
}

/// <summary>
/// Returns a Boolean value that indicates if this geometric property is read-only.
/// </summary>
/// <returns>
/// Returns a Boolean value that specifies whether this geometric property is read-only.
/// </returns>
bool MgGeometricPropertyDefinition::GetReadOnly()
{
    return m_readOnly;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets a Boolean value that indicates if the geometry of this property
/// include elevation values.
/// </summary>
/// <returns>
/// Returns a Boolean value that determines if the geometry of this property
/// includes elevation values
/// </returns>
bool MgGeometricPropertyDefinition::GetHasElevation()
{
    return m_hasElevation;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets a Boolean value that indicates if the geometry of this property
/// includes measurement values that can be used for dynamic segmentation.
/// </summary>
/// <returns>
/// Returns a Boolean value that indicates if the geometry of this property
/// includes measurement values
/// </returns>
bool MgGeometricPropertyDefinition::GetHasMeasure()
{
    return m_hasMeasure;
}

/// <summary>
/// Gets the Spatial Context name associated to this geometric property.
/// </summary>
/// <returns>
/// Returns a String value representing the Spatial Context name.
/// If empty then the geometric property is associated with all the Spatial Contexts
/// in the datastore.
/// </returns>
STRING MgGeometricPropertyDefinition::GetSpatialContextAssociation()
{
    return m_associatedSCName;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the Geometric Types that can be stored in this geometric property.
/// This value may be any combination of the values from the
/// GeometricType enumeration combined via a bit-wise or operation.</summary>
/// <param name="types">GeometricTypes that can be stored in this geometric property</param>
void MgGeometricPropertyDefinition::SetGeometryTypes(INT32 types)
{
    m_geometricTypes = types;

    // Set the specific types in case SetSpecificGeometryTypes() is never called.
    INT32 geomTypeList[MG_MAX_GEOMETRY_TYPE_SIZE];
    INT32 geomTypeCount = 0;
    // The MgFeatureGeometricType definition has no "All" symbol, so we'll make the equivalent based on FdoGeometricType_All.
    INT32 allTypes = MgFeatureGeometricType::Point|MgFeatureGeometricType::Curve|MgFeatureGeometricType::Surface|MgFeatureGeometricType::Solid;
    if (types == allTypes)
    {
        geomTypeList[geomTypeCount++] = MgGeometryType::Point;
        geomTypeList[geomTypeCount++] = MgGeometryType::MultiPoint;
        geomTypeList[geomTypeCount++] = MgGeometryType::LineString;
        geomTypeList[geomTypeCount++] = MgGeometryType::MultiLineString;
        geomTypeList[geomTypeCount++] = MgGeometryType::CurveString;
        geomTypeList[geomTypeCount++] = MgGeometryType::MultiCurveString;
        geomTypeList[geomTypeCount++] = MgGeometryType::Polygon;
        geomTypeList[geomTypeCount++] = MgGeometryType::MultiPolygon;
        geomTypeList[geomTypeCount++] = MgGeometryType::CurvePolygon;
        geomTypeList[geomTypeCount++] = MgGeometryType::MultiCurvePolygon;
        geomTypeList[geomTypeCount++] = MgGeometryType::MultiGeometry;
    }
    else // Deal with types by bit-mask value.
    {
        if ((types & MgFeatureGeometricType::Point) != 0)
        {
            geomTypeList[geomTypeCount++] = MgGeometryType::Point;
            geomTypeList[geomTypeCount++] = MgGeometryType::MultiPoint;
        }
        if ((types & MgFeatureGeometricType::Curve) != 0)
        {
            geomTypeList[geomTypeCount++] = MgGeometryType::LineString;
            geomTypeList[geomTypeCount++] = MgGeometryType::MultiLineString;
            geomTypeList[geomTypeCount++] = MgGeometryType::CurveString;
            geomTypeList[geomTypeCount++] = MgGeometryType::MultiCurveString;
        }
        if ((types & MgFeatureGeometricType::Surface) != 0)
        {
            geomTypeList[geomTypeCount++] = MgGeometryType::Polygon;
            geomTypeList[geomTypeCount++] = MgGeometryType::MultiPolygon;
            geomTypeList[geomTypeCount++] = MgGeometryType::CurvePolygon;
            geomTypeList[geomTypeCount++] = MgGeometryType::MultiCurvePolygon;
        }
    }
    Ptr<MgGeometryTypeInfo> geomTypeInfo = new MgGeometryTypeInfo;
    geomTypeInfo->SetTypes(geomTypeList, geomTypeCount);
    this->SetSpecificGeometryTypes(geomTypeInfo);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the specific list of geometry types that can be stored in this
/// geometric property. The provided value is a list of geometry types
/// that are supported.
/// <param name="typeInfo">The specific set of geometry types that can be stored in this geometric property</param>
void MgGeometricPropertyDefinition::SetSpecificGeometryTypes(MgGeometryTypeInfo * typeInfo)
{
    m_geometryTypeInfo = SAFE_ADDREF(typeInfo);

    // Just in case of a mis-match in the caller's code, where it sets
    // specific geometry types but later asks for non-specific ones, we'll update
    // the non-specific ones here.  We'll only turn them on, not fully
    // reset them, in case SetGeometryTypes is also called.
    INT32 geomTypeCount = m_geometryTypeInfo->GetCount();
    // The MgFeatureGeometricType definition has no "All" symbol, so we'll make the equivalent based on FdoGeometricType_All.
    INT32 allTypes = MgFeatureGeometricType::Point|MgFeatureGeometricType::Curve|MgFeatureGeometricType::Surface|MgFeatureGeometricType::Solid;
    INT32 allSpecificTypes =
        ( 1 << MgGeometryType::Point ) |
        ( 1 << MgGeometryType::MultiPoint ) |
        ( 1 << MgGeometryType::LineString ) |
        ( 1 << MgGeometryType::MultiLineString ) |
        ( 1 << MgGeometryType::CurveString ) |
        ( 1 << MgGeometryType::MultiCurveString ) |
        ( 1 << MgGeometryType::Polygon ) |
        ( 1 << MgGeometryType::MultiPolygon ) |
        ( 1 << MgGeometryType::CurvePolygon ) |
        ( 1 << MgGeometryType::MultiCurvePolygon ) |
        ( 1 << MgGeometryType::MultiGeometry );
    INT32 specificTypesMask = 0;

    for (INT32 i = 0;  i < geomTypeCount && i < MG_MAX_GEOMETRY_TYPE_SIZE;  i++)
    {
        INT32 type = m_geometryTypeInfo->GetType(i);
        specificTypesMask |= 1 << type;
        switch(type)
        {
        case MgGeometryType::Point:
        case MgGeometryType::MultiPoint:
            m_geometricTypes |= MgFeatureGeometricType::Point;
            break;
        case MgGeometryType::LineString:
        case MgGeometryType::MultiLineString:
        case MgGeometryType::CurveString:
        case MgGeometryType::MultiCurveString:
            m_geometricTypes |= MgFeatureGeometricType::Curve;
            break;
        case MgGeometryType::Polygon:
        case MgGeometryType::MultiPolygon:
        case MgGeometryType::CurvePolygon:
        case MgGeometryType::MultiCurvePolygon:
            m_geometricTypes |= MgFeatureGeometricType::Surface;
            break;
        }
    }
    if (specificTypesMask == allSpecificTypes)
        m_geometricTypes |= allTypes;
}

/// <summary>
/// Sets a Boolean value that specifies whether this geometric property is read-only.
/// </summary>
/// <param name="value">
/// Input a Boolean value that specifies whether this geometric property is read-only
/// </param>
/// <returns>Returns nothing</returns>
void MgGeometricPropertyDefinition::SetReadOnly(bool value)
{
    m_readOnly = value;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets a Boolean value that indicates if the geometry of this property
/// include elevation values.
/// </summary>
/// <param name="hasElevation">Boolean value that determines if the geometry of this property
/// includes elevation values/param>
void MgGeometricPropertyDefinition::SetHasElevation(bool hasElevation)
{
    m_hasElevation = hasElevation;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Sets a Boolean value that indicates if the geometry of this property
/// includes measurement values that can be used for dynamic segmentation.
/// <returns>
/// <param name="hasMeasure">Boolean value that indicates if the geometry of this property
/// includes measurement values</param>
/// </returns>
void MgGeometricPropertyDefinition::SetHasMeasure(bool hasMeasure)
{
    m_hasMeasure = hasMeasure;
}

/// <summary>
/// Sets/add a Spatial Context association to this geometric property.
/// </summary>
/// <param name="value">
/// Input the Spatial Context name to be added/set.
/// Defaults to the active Spatial Context.
/// </param>
/// <returns>Returns nothing</returns>
void MgGeometricPropertyDefinition::SetSpatialContextAssociation(CREFSTRING spatialContextName)
{
    m_associatedSCName = spatialContextName;
}

MgGeometricPropertyDefinition::MgGeometricPropertyDefinition()
{
    Initialize();
}

void MgGeometricPropertyDefinition::Serialize(MgStream* stream)
{
    MgPropertyDefinition::Serialize(stream);
    stream->WriteInt32(m_geometricTypes);
    m_geometryTypeInfo->Serialize(stream);
    stream->WriteBoolean(m_readOnly);
    stream->WriteBoolean(m_hasElevation);
    stream->WriteBoolean(m_hasMeasure);
    stream->WriteString(m_associatedSCName);
    stream->WriteString(m_serializedXml);
}

void MgGeometricPropertyDefinition::Deserialize(MgStream* stream)
{
    MgPropertyDefinition::Deserialize(stream);
    stream->GetInt32(m_geometricTypes);
    m_geometryTypeInfo->Deserialize(stream);
    stream->GetBoolean(m_readOnly);
    stream->GetBoolean(m_hasElevation);
    stream->GetBoolean(m_hasMeasure);
    stream->GetString(m_associatedSCName);
    stream->GetString(m_serializedXml);
}

void MgGeometricPropertyDefinition::Initialize()
{
    m_geometricTypes = 0;
    m_geometryTypeInfo = new MgGeometryTypeInfo();
    m_readOnly = false;
    m_hasElevation = false;
    m_hasMeasure = false;
    m_associatedSCName = L"";
    m_serializedXml = L"";
}

void MgGeometricPropertyDefinition::ToXml(string& xmlStr, bool includeType)
{
}
