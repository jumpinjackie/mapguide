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

#include "Foundation.h"

MG_IMPL_DYNCREATE(MgStringProperty);

/////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor.
/// </summary>
MgStringProperty::MgStringProperty(CREFSTRING name, CREFSTRING value)
{
    SetName(name);
    m_value = value;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor.
/// </summary>
MgStringProperty::MgStringProperty()
{
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor.
/// </summary>
MgStringProperty::~MgStringProperty()
{
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgStringProperty::GetClassId()
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
INT16 MgStringProperty::GetPropertyType()
{
    return MgPropertyType::String;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the property value
/// </summary>
/// <returns>
/// The property value
/// </returns>
STRING MgStringProperty::GetValue()
{
    CheckNull();
    return m_value;
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the property value
/// </summary>
/// <returns>
/// Nothing
/// </returns>
void MgStringProperty::SetValue(CREFSTRING value)
{
    CheckNull();
    m_value = value;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Self destructing method
/// </summary>
void MgStringProperty::Dispose()
{
    delete this;
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Return XML for the property
/// </summary>
void MgStringProperty::ToXml(string &str, bool includeType, string rootElmName)
{
    ToXml(str,false,includeType,rootElmName);
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Return XML for the property
/// </summary>
void MgStringProperty::ToXml(string &str, bool onlyKey, bool includeType, string rootElmName)
{
    if (!onlyKey)
    {
        str += "<" + rootElmName + ">";
        str += "<Name>";
        str += MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(GetName())) + "</Name>";
        if (includeType)
        {
            str += "<Type>string</Type>";
        }

        if (!this->IsNull())
        {
            str += "<Value>";
            str += MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(this->GetValue()));
            str += "</Value>";
        }

        str += "</" + rootElmName + ">";
    }
    else
    {
        str += "<Item>";
        str += MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(GetName())) + "</Item>";
    }
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Serialize data to TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>
void MgStringProperty::Serialize(MgStream* stream)
{
    MgNullableProperty::Serialize(stream);
    stream->WriteString(GetName());
    stream->WriteString(m_value);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserialize data from TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>
void MgStringProperty::Deserialize(MgStream* stream)
{
    MgNullableProperty::Deserialize(stream);
    STRING str;

    stream->GetString(str);
    SetName(str);

    stream->GetString(m_value);
}
