//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

MG_IMPL_DYNCREATE(MgBlobProperty);

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
MgBlobProperty::MgBlobProperty(CREFSTRING name, MgByteReader* value)
{
    SetName(name);
    m_value = SAFE_ADDREF(value);
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor.
/// </summary>
MgBlobProperty::MgBlobProperty()
{
    m_value = NULL;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor.
/// </summary>
MgBlobProperty::~MgBlobProperty()
{
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgBlobProperty::GetClassId()
{
    return m_cls_id;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the property type
/// </summary>
/// <returns>
/// The property type
/// </returns>
INT16 MgBlobProperty::GetPropertyType()
{
    return MgPropertyType::Blob;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the property value
/// </summary>
/// <returns>
/// The property value
/// </returns>
MgByteReader* MgBlobProperty::GetValue()
{
    CheckNull();
    return SAFE_ADDREF((MgByteReader*)m_value);
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the property value
/// <param name="value">
/// Value of property
/// </param>
/// </summary>
void MgBlobProperty::SetValue(MgByteReader* value)
{
    CheckNull();
    m_value = SAFE_ADDREF(value);
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Self destructing method
/// </summary>
void MgBlobProperty::Dispose()
{
    delete this;
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Converts data into XML format
/// </summary>
/// <returns>
/// The ByteReader
/// </returns>
void MgBlobProperty::ToXml(string &str, bool includeType, string rootElmName)
{
    str += "<" + rootElmName + ">";

    str += "<Name>";
    str += MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(GetName())) + "</Name>";

    if (includeType)
    {
        str += "<Type>blob</Type>";
    }

    if (m_value != NULL || !this->IsNull())
    {
        str += "<Value>";
        Ptr<MgByteReader> reader = this->GetValue();
        str += MgUtil::GetStringFromReader(reader);
        str += "</Value>";
    }

    str += "</" + rootElmName + ">";
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Serialize data to TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>
void MgBlobProperty::Serialize(MgStream* stream)
{
    MgNullableProperty::Serialize(stream);
    stream->WriteString(GetName());
    stream->WriteStream(m_value);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserialize data from TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>
void MgBlobProperty::Deserialize(MgStream* stream)
{
    MgNullableProperty::Deserialize(stream);
    STRING str;

    stream->GetString(str);
    SetName(str);

    m_value = stream->GetStream();
}
