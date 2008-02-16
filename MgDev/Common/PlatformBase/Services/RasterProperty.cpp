//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

MG_IMPL_DYNCREATE(MgRasterProperty);

/////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor.
/// </summary>
/// <param name="name">
/// Name of property
/// </param>
/// <param name="value">
/// Value of property
/// </param>
/// <returns>
/// Nothing
/// </returns>
MgRasterProperty::MgRasterProperty(CREFSTRING name, MgRaster* value)
{
    SetName(name);
    m_value = SAFE_ADDREF(value);
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor.
/// </summary>
MgRasterProperty::MgRasterProperty()
{
    m_value = NULL;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor.
/// </summary>
MgRasterProperty::~MgRasterProperty() {}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
 INT32 MgRasterProperty::GetClassId() { return m_cls_id; }

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the property type
/// </summary>
/// <returns>
/// The property type
/// </returns>
 INT16 MgRasterProperty::GetPropertyType() { return MgPropertyType::Raster; }

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the property value
/// </summary>
/// <returns>
/// The property value
/// </returns>
MgRaster* MgRasterProperty::GetValue() { return SAFE_ADDREF((MgRaster*)m_value); }

/////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the property value
/// <param name="value">
/// Value of property
/// </param>
/// </summary>
 void MgRasterProperty::SetValue(MgRaster* value) { m_value = value; }


/////////////////////////////////////////////////////////////////
/// <summary>
/// Self destructing method
/// </summary>
 void MgRasterProperty::Dispose() { delete this; }

/////////////////////////////////////////////////////////////////
/// <summary>
/// Converts data into XML format
/// </summary>
/// <returns>
/// The Raster
/// </returns>
void MgRasterProperty::ToXml(string &str, bool includeType, string rootElmName)
{
    str += "<Property><Name>";
    str += MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(GetName())) + "</Name>";
    if (includeType)
    {
        str += "<Type>raster</Type>";
    }
    str += "<Value>";
//  Ptr<MgByteReader> reader = this->GetValue();
    str += /* MgUtil::GetStringFromReader(reader) +*/  "</Value></Property>";
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Serialize data to TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgRasterProperty::Serialize(MgStream* stream)
{
    MgNullableProperty::Serialize(stream);
    stream->WriteString(GetName());
    stream->WriteObject((MgRaster*)m_value);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserialize data from TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgRasterProperty::Deserialize(MgStream* stream)
{
    MgNullableProperty::Deserialize(stream);
    STRING str;

    stream->GetString(str);
    SetName(str);

    m_value = (MgRaster*)stream->GetObject();
}
