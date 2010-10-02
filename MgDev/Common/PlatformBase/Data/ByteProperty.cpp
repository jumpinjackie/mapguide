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

MG_IMPL_DYNCREATE(MgByteProperty);

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
MgByteProperty::MgByteProperty(CREFSTRING name, BYTE value)
{
    SetName(name);
    m_value = value;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor.
/// </summary>
MgByteProperty::MgByteProperty()
{
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor.
/// </summary>
MgByteProperty::~MgByteProperty()
{
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgByteProperty::GetClassId()
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
INT16 MgByteProperty::GetPropertyType()
{
    return MgPropertyType::Byte;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the property value
/// </summary>
/// <returns>
/// The property value
/// </returns>
BYTE MgByteProperty::GetValue()
{
    CheckNull();
    return m_value;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the property value
/// <param name="name">
/// Value of property
/// </param>
/// </summary>
void MgByteProperty::SetValue(BYTE value)
{
    CheckNull();
    m_value = value;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Self destructing method
/// </summary>
void MgByteProperty::Dispose()
{
    delete this;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Converts data into XML format
/// </summary>
void MgByteProperty::ToXml(string &str, bool includeType, string rootElmName)
{
    str += "<" + rootElmName + ">";

    str += "<Name>";
    str += MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(GetName())) + "</Name>";
    if (includeType)
    {
        str += "<Type>byte</Type>";
    }

    if (!this->IsNull())
    {
        str += "<Value>";
        str += MgUtil::Char2Hex(this->GetValue());
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
void MgByteProperty::Serialize(MgStream* stream)
{
    MgNullableProperty::Serialize(stream);
    stream->WriteString(GetName());
    stream->WriteByte(m_value);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserialize data from TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>
void MgByteProperty::Deserialize(MgStream* stream)
{
    MgNullableProperty::Deserialize(stream);
    STRING str;

    stream->GetString(str);
    SetName(str);

    stream->GetByte(m_value);
}
