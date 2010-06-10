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

MG_IMPL_DYNCREATE(MgDateTimeProperty);

/////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor.
/// </summary>
MgDateTimeProperty::MgDateTimeProperty(CREFSTRING name, MgDateTime* value)
{
    SetName(name);
    m_value = SAFE_ADDREF(value);
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor.
/// </summary>
MgDateTimeProperty::MgDateTimeProperty()
{
    m_value = (MgDateTime*)NULL;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor.
/// </summary>
MgDateTimeProperty::~MgDateTimeProperty()
{
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgDateTimeProperty::GetClassId()
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
INT16 MgDateTimeProperty::GetPropertyType()
{
    return MgPropertyType::DateTime;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the property value
/// </summary>
/// <returns>
/// The property value
/// </returns>
MgDateTime* MgDateTimeProperty::GetValue()
{
    CheckNull();
    return SAFE_ADDREF((MgDateTime*)m_value);
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the property value
/// <param name="name">
/// Value of property
/// </param>
/// </summary>
void MgDateTimeProperty::SetValue(MgDateTime* value)
{
    CheckNull();
    m_value = SAFE_ADDREF(value);
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Self destructing method
/// </summary>
void MgDateTimeProperty::Dispose()
{
    delete this;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Converts to XML format
/// </summary>
void MgDateTimeProperty::ToXml(string &str, bool includeType, string rootElmName)
{
    str += "<" + rootElmName + ">";

    str += "<Name>";
    str += MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(GetName())) + "</Name>";

    if (includeType)
    {
        str += "<Type>datetime</Type>";
    }

    if (!this->IsNull())
    {
        str += "<Value>";
        char buf[128]; buf[0] = 0;
        Ptr<MgDateTime> dtPtr = this->GetValue();
        if (dtPtr != NULL)
        {
            STRING dateStr = dtPtr->ToString();
            str += MgUtil::WideCharToMultiByte(dateStr);
        }
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
void MgDateTimeProperty::Serialize(MgStream* stream)
{
    MgNullableProperty::Serialize(stream);
    stream->WriteString(GetName());
    stream->WriteObject(m_value);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserialize data from TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>
void MgDateTimeProperty::Deserialize(MgStream* stream)
{
    MgNullableProperty::Deserialize(stream);
    STRING str;

    stream->GetString(str);
    SetName(str);

    m_value = (MgDateTime*)stream->GetObject();
}
