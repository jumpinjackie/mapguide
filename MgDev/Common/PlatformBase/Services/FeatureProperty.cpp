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
#include "FeatureProperty.h"
#include "FeatureService.h"

MG_IMPL_DYNCREATE(MgFeatureProperty);

/////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor.
/// </summary>
MgFeatureProperty::MgFeatureProperty(CREFSTRING name, MgFeatureReader* value)
{
    SetName(name);
    m_value = SAFE_ADDREF((MgFeatureReader*)value);
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor.
/// </summary>
MgFeatureProperty::MgFeatureProperty()
{
    m_value = NULL;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor.
/// </summary>
MgFeatureProperty::~MgFeatureProperty() {}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
 INT32 MgFeatureProperty::GetClassId()
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
 INT16 MgFeatureProperty::GetPropertyType()
{
    return MgPropertyType::Feature;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the property value
/// </summary>
/// <returns>
/// The property value
/// </returns>
MgFeatureReader* MgFeatureProperty::GetValue()
{
    return SAFE_ADDREF((MgFeatureReader*)m_value);
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the property value
/// <param name="name">
/// Value of property
/// </param>
/// </summary>
void MgFeatureProperty::SetValue(MgFeatureReader* value)
{
    m_value = SAFE_ADDREF((MgFeatureReader*)value);
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Self destructing method
/// </summary>
 void MgFeatureProperty::Dispose()
 {
     delete this;
 }

/////////////////////////////////////////////////////////////////
/// <summary>
/// Converts data into XML
/// </summary>
void MgFeatureProperty::ToXml(string &str, bool includeType, string rootElmName)
{
    str += "<" + rootElmName + ">";

    str += "<Name>";
    str += MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(GetName())) + "</Name>";
    if (includeType)
    {
        str += "<Type>feature</Type>";
    }

    if (!this->IsNull())
    {
        str += "<Value>";
        std::string featureXml;

        Ptr<MgByteReader> reader = m_value->ToXml();
        MgByteSink sink(reader);
        sink.ToStringUtf8(featureXml);

        size_t idx = featureXml.find("?>");
        if (idx >= 0)
        {
            featureXml = featureXml.substr(idx+2);
        }

        str += featureXml;
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

void MgFeatureProperty::Serialize(MgStream* stream)
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

void MgFeatureProperty::Deserialize(MgStream* stream)
{
    MgNullableProperty::Deserialize(stream);
    STRING str;

    stream->GetString(str);
    SetName(str);

    m_value = (MgFeatureReader*)stream->GetObject();
}
