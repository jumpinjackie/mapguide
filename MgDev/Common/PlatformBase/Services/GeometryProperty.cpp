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
#include "GeometryProperty.h"
#include "GeometryCommon.h"

MG_IMPL_DYNCREATE(MgGeometryProperty);

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
MgGeometryProperty::MgGeometryProperty(CREFSTRING name, MgByteReader* value)
{
    SetName(name);
    m_value = SAFE_ADDREF(value);
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor.
/// </summary>
MgGeometryProperty::MgGeometryProperty()
{
    m_value = NULL;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor.
/// </summary>
MgGeometryProperty::~MgGeometryProperty() {}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
 INT32 MgGeometryProperty::GetClassId() { return m_cls_id; }

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the property type
/// </summary>
/// <returns>
/// The property type
/// </returns>
 INT16 MgGeometryProperty::GetPropertyType() { return MgPropertyType::Geometry; }

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the property value
/// </summary>
/// <returns>
/// The property value
/// </returns>
MgByteReader* MgGeometryProperty::GetValue()
{
    if (m_value != NULL)
        m_value->Rewind();

    return SAFE_ADDREF((MgByteReader*)m_value);
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the property value
/// <param name="value">
/// Value of property
/// </param>
/// </summary>
 void MgGeometryProperty::SetValue(MgByteReader* value) { m_value = SAFE_ADDREF((MgByteReader*)value); }


/////////////////////////////////////////////////////////////////
/// <summary>
/// Self destructing method
/// </summary>
 void MgGeometryProperty::Dispose() { delete this; }

/////////////////////////////////////////////////////////////////
/// <summary>
/// Converts data into XML format
/// </summary>
/// <returns>
/// The ByteReader
/// </returns>
void MgGeometryProperty::ToXml(string &str, bool includeType, string rootElmName)
{
    str += "<" + rootElmName + ">";

    str += "<Name>";
    str += MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(GetName())) + "</Name>";
    if (includeType)
    {
        str += "<Type>geometry</Type>";
    }

    if (!this->IsNull())
    {
        string valueXml = "";
        try 
        {
            valueXml += "<Value>";
            Ptr<MgByteReader> byteReader = this->GetValue();

            if (byteReader != NULL)
            {
                MgAgfReaderWriter agfReader;
                Ptr<MgGeometry> geom = agfReader.Read(byteReader);

                // geom->ToXml(str); // TODO: we need this method
                STRING awktStr = L"";
                if (geom != NULL)
                {
                    awktStr = geom->ToAwkt(false);
                    assert(!awktStr.empty());
                }

                if (!awktStr.empty())
                {
                    valueXml += MgUtil::WideCharToMultiByte(awktStr);
                }
            }
            valueXml += "</Value>";
        }
        catch(MgException* ex) //Can happen if the geometry is invalid
        {
            SAFE_RELEASE(ex);
            valueXml = ""; //Treat an invalid geometry as a NULL geometry
        }
        str += valueXml;
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

void MgGeometryProperty::Serialize(MgStream* stream)
{
    MgNullableProperty::Serialize(stream);
    stream->WriteString(GetName());
    Ptr<MgByteReader> byteReader = this->GetValue();
    stream->WriteStream(byteReader);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserialize data from TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgGeometryProperty::Deserialize(MgStream* stream)
{
    MgNullableProperty::Deserialize(stream);
    STRING str;

    stream->GetString(str);
    SetName(str);

    m_value = stream->GetStream();
}
