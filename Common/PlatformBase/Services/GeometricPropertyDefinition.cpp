//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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
    stream->GetBoolean(m_readOnly);
    stream->GetBoolean(m_hasElevation);
    stream->GetBoolean(m_hasMeasure);
    stream->GetString(m_associatedSCName);
    stream->GetString(m_serializedXml);
}

void MgGeometricPropertyDefinition::Initialize()
{
    m_geometricTypes = 0;
    m_readOnly = false;
    m_hasElevation = false;
    m_hasMeasure = false;
    m_associatedSCName = L"";
    m_serializedXml = L"";
}

void MgGeometricPropertyDefinition::ToXml(string& xmlStr, bool includeType)
{
}
