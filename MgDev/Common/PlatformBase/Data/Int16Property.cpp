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

MG_IMPL_DYNCREATE(MgInt16Property);

/////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor.
/// </summary>
MgInt16Property::MgInt16Property(CREFSTRING name, INT16 value)
{
    SetName(name);
    m_value = value;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor.
/// </summary>
MgInt16Property::MgInt16Property()
{
    m_value = 0;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor.
/// </summary>
MgInt16Property::~MgInt16Property()
{
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgInt16Property::GetClassId()
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
INT16 MgInt16Property::GetPropertyType()
{
    return MgPropertyType::Int16;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the property value
/// </summary>
/// <returns>
/// The property value
/// </returns>
INT16 MgInt16Property::GetValue()
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
void MgInt16Property::SetValue(INT16 value)
{
    CheckNull();
    m_value = value;
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Self destructing method
/// </summary>
void MgInt16Property::Dispose()
{
    delete this;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Converts data into XML format
/// </summary>
void MgInt16Property::ToXml(string &str, bool includeType, string rootElmName)
{
    str += "<" + rootElmName + ">";

    str += "<Name>";
    str += MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(GetName())) + "</Name>";

    if (includeType)
    {
        str += "<Type>int16</Type>";
    }

    if (!this->IsNull())
    {
        str += "<Value>";
        char buf[128]; buf[0] = 0;
        #ifdef _WIN32
        itoa(this->GetValue(), buf, 10);
        #else
        snprintf(buf, 128, "%d", this->GetValue());
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
void MgInt16Property::Serialize(MgStream* stream)
{
    MgNullableProperty::Serialize(stream);
    stream->WriteString(GetName());
    stream->WriteInt16(m_value);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserialize data from TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>
void MgInt16Property::Deserialize(MgStream* stream)
{
    MgNullableProperty::Deserialize(stream);
    STRING str;

    stream->GetString(str);
    SetName(str);

    stream->GetInt16(m_value);
}
