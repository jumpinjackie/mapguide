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

MG_IMPL_DYNCREATE(MgInt32Property);

/////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor.
/// </summary>
MgInt32Property::MgInt32Property(CREFSTRING name, INT32 value)
{
    SetName(name);
    m_value = value;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor.
/// </summary>
MgInt32Property::MgInt32Property()
{
    m_value = 0;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor.
/// </summary>
MgInt32Property::~MgInt32Property()
{
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgInt32Property::GetClassId()
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
INT16 MgInt32Property::GetPropertyType()
{
    return MgPropertyType::Int32;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the property value
/// </summary>
/// <returns>
/// The property value
/// </returns>
INT32 MgInt32Property::GetValue()
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
void MgInt32Property::SetValue(INT32 value)
{
    CheckNull();
    m_value = value;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Self destructing method
/// </summary>
void MgInt32Property::Dispose()
{
    delete this;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Converts to XML format
/// </summary>
void MgInt32Property::ToXml(string &str, bool includeType, string rootElmName)
{
    str += "<" + rootElmName + ">";

    str += "<Name>";
    str += MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(GetName())) + "</Name>";

    if (includeType)
    {
        str += "<Type>int32</Type>";
    }

    if (!this->IsNull())
    {
        str += "<Value>";
        char buf[128]; buf[0] = 0;
        #ifdef _WIN32
        itoa(this->GetValue(), buf, 10);
        #else
        snprintf(buf, 128, "%li", this->GetValue());
        #endif

        str += std::string(buf);
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
void MgInt32Property::Serialize(MgStream* stream)
{
    MgNullableProperty::Serialize(stream);
    stream->WriteString(GetName());
    stream->WriteInt32(m_value);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserialize data from TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>
void MgInt32Property::Deserialize(MgStream* stream)
{
    MgNullableProperty::Deserialize(stream);
    STRING str;

    stream->GetString(str);
    SetName(str);

    stream->GetInt32(m_value);
}
