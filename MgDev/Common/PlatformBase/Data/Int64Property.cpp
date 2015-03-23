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

#include "PlatformBase.h"

MG_IMPL_DYNCREATE(MgInt64Property);

/////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor.
/// </summary>
MgInt64Property::MgInt64Property(CREFSTRING name, INT64 value)
{
    SetName(name);
    m_value = value;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor.
/// </summary>
MgInt64Property::MgInt64Property()
{
    m_value = 0;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor.
/// </summary>
MgInt64Property::~MgInt64Property()
{
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgInt64Property::GetClassId()
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
INT16 MgInt64Property::GetPropertyType()
{
    return MgPropertyType::Int64;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the property value
/// </summary>
/// <returns>
/// The property value
/// </returns>
INT64 MgInt64Property::GetValue()
{
    CheckNull();
    return m_value;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the property value
/// </summary>
/// <param name="name">
/// Value of property
/// </param>
/// <returns>
/// Nothing
/// </returns>
void MgInt64Property::SetValue(INT64 value)
{
    CheckNull();
    m_value = value;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Converts data into XML
/// </summary>
void MgInt64Property::ToXml(string &str, bool includeType, string rootElmName)
{
    str += "<" + rootElmName + ">";

    str += "<Name>";
    str += MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(GetName())) + "</Name>";

    if (includeType)
    {
        str += "<Type>int64</Type>";
    }

    if (!this->IsNull())
    {
        str += "<Value>";
        std::string tmp = "";
        MgUtil::Int64ToString(this->GetValue(), tmp);

        str += tmp;
        str += "</Value>";
    }

    str += "</" + rootElmName + ">";
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Self destructing method
/// </summary>
void MgInt64Property::Dispose()
{
    delete this;
}


//////////////////////////////////////////////////////////////////
///<summary>
/// Serialize data to TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>
void MgInt64Property::Serialize(MgStream* stream)
{
    MgNullableProperty::Serialize(stream);
    stream->WriteString(GetName());
    stream->WriteInt64(m_value);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserialize data from TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>
void MgInt64Property::Deserialize(MgStream* stream)
{
    MgNullableProperty::Deserialize(stream);
    STRING str;

    stream->GetString(str);
    SetName(str);

    stream->GetInt64(m_value);
}
