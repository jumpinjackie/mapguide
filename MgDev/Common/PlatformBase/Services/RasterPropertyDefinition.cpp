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
#include "RasterPropertyDefinition.h"

MG_IMPL_DYNCREATE(MgRasterPropertyDefinition)

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs a MgRasterPropertyDefinition object
/// </summary>
/// <param name="name">Name of the property</param>
MgRasterPropertyDefinition::MgRasterPropertyDefinition(CREFSTRING name) : MgPropertyDefinition(name, MgFeaturePropertyType::RasterProperty)
{
    Initialize();
}

/// <summary>
/// Returns a Boolean value that indicates if this property is read-only.
/// </summary>
/// <returns>
/// Returns a Boolean value that specifies whether this property is read-only.
/// </returns>
bool MgRasterPropertyDefinition::GetReadOnly()
{
    return m_readOnly;
}

/// <summary>
/// Returns a Boolean value that indicates if this property's value can be null.
/// </summary>
/// <returns>Returns a Boolean value</returns>
bool MgRasterPropertyDefinition::GetNullable()
{
    return m_nullable;
}


/// <summary>Gets the default size of image file in the horizontal
/// direction in pixels (number of columns).</summary>
/// <returns>Returns the current default horizontal image size in pixels
/// (number of columns).</returns>
INT32 MgRasterPropertyDefinition::GetDefaultImageXSize()
{
    return m_sizeX;
}

/// <summary>
/// Gets the default size of image file in the vertical direction in pixels (number of rows).
/// </summary>
/// <returns>
/// Returns the current default vertical image size in pixels (number of rows).
/// </returns>
INT32 MgRasterPropertyDefinition::GetDefaultImageYSize()
{
    return m_sizeY;
}

/// <summary>
/// Gets the Spatial Context name associated to this raster property.
/// </summary>
/// <returns>
/// Returns a String value representing the Spatial Context name.
/// If empty then the raster property is associated with the active Spatial Context
/// in the datastore.
/// </returns>
STRING MgRasterPropertyDefinition::GetSpatialContextAssociation()
{
    return m_associatedSCName;
}

/// <summary>
/// Sets a Boolean value that specifies whether this geometric property is read-only.
/// </summary>
/// <param name="value">
/// Input a Boolean value that specifies whether this geometric property is read-only
/// </param>
/// <returns>Returns nothing</returns>
void MgRasterPropertyDefinition::SetReadOnly(bool value)
{
    m_readOnly = value;
}

/// <summary>Sets a Boolean value that indicates if this property's value can be null.</summary>
/// <param name="value">Input a Boolean value that indicates if this property's value can be
/// null</param>
/// <returns>Returns nothing</returns>
void MgRasterPropertyDefinition::SetNullable(bool value)
{
    m_nullable = value;
}

/// <summary>
/// Sets the default size of image file in the horizontal direction in pixels (number of columns).
/// </summary>
/// <param name="size">
/// The desired default horizontal image size in pixels (number of columns)
/// </param>
void MgRasterPropertyDefinition::SetDefaultImageXSize (INT32 size)
{
    m_sizeX = size;
}

/// <summary>
/// Sets the default size of image file in the vertical direction in pixels (number of rows).
/// </summary>
/// <param name="size">
/// The desired default vertical image size in pixels (number of rows).
/// </param>
void MgRasterPropertyDefinition::SetDefaultImageYSize(INT32 size)
{
    m_sizeY = size;
}

/// <summary>
/// Sets/add a Spatial Context association to this raster property.
/// </summary>
/// <param name="value">
/// Input the Spatial Context name to be added/set.
/// Defaults to the active Spatial Context.
/// </param>
/// <returns>Returns nothing</returns>
void MgRasterPropertyDefinition::SetSpatialContextAssociation(CREFSTRING spatialContextName)
{
    m_associatedSCName = spatialContextName;
}

MgRasterPropertyDefinition::MgRasterPropertyDefinition()
{
    Initialize();
}

void MgRasterPropertyDefinition::Serialize(MgStream* stream)
{
    MgPropertyDefinition::Serialize(stream);
    stream->WriteBoolean(m_readOnly);
    stream->WriteBoolean(m_nullable);
    stream->WriteInt32(m_sizeX);
    stream->WriteInt32(m_sizeY);
    stream->WriteString(m_serializedXml);
    stream->WriteString(m_associatedSCName);
}

void MgRasterPropertyDefinition::Deserialize(MgStream* stream)
{
    MgPropertyDefinition::Deserialize(stream);
    stream->GetBoolean(m_readOnly);
    stream->GetBoolean(m_nullable);
    stream->GetInt32(m_sizeX);
    stream->GetInt32(m_sizeY);
    stream->GetString(m_serializedXml);
    stream->GetString(m_associatedSCName);
}

void MgRasterPropertyDefinition::Initialize()
{
    m_readOnly = false;
    m_nullable = false;
    m_sizeX = 0;
    m_sizeY = 0;
    m_serializedXml = L"";
    m_associatedSCName = L"";
}

void MgRasterPropertyDefinition::ToXml(string& xmlStr, bool includeType)
{
}
