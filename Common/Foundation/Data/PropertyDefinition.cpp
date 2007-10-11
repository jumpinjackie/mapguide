//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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
#include "System/JsonDoc.h"

MG_IMPL_DYNCREATE(MgPropertyDefinition);

static std::map<INT16, std::string> sm_PropertyType;
static std::map<INT16, std::string> sm_DataType;

bool MgPropertyDefinition::smb_initialize = MgPropertyDefinition::Initialize();

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
MgPropertyDefinition::MgPropertyDefinition(CREFSTRING name, INT16 type)
{
    SetName(name);

    string propType = sm_PropertyType[type];

    // Property Type has to be MgFeaturePropertyType or MgPropertyType
    if (propType.empty())
    {
        propType = sm_DataType[type];
    }

    if (propType.empty())
    {
        throw new MgInvalidPropertyTypeException(L"MgPropertyDefinition.MgPropertyDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    m_propertyType = type;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor.
/// </summary>
MgPropertyDefinition::MgPropertyDefinition()
{
    m_propertyType = MgPropertyType::Byte;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor.
/// </summary>
MgPropertyDefinition::~MgPropertyDefinition()
{
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgPropertyDefinition::GetClassId()
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
INT16 MgPropertyDefinition::GetPropertyType()
{
    return m_propertyType;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Self destructing method
/// </summary>
void MgPropertyDefinition::Dispose()
{
    delete this;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Converts data into XML format
/// </summary>
void MgPropertyDefinition::ToColumnDefinitionAsXml(string &str, bool includeType)
{
    str += "<Column><Name>";

    str += MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(GetName())) +
           "</Name><Type>" +
           sm_DataType[m_propertyType].c_str() +
           "</Type>";

    str += "</Column>";
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Converts data into JSON format
/// </summary>
void MgPropertyDefinition::ToColumnDefinitionAsJson(MgJsonDoc &jsonDoc, bool includeType)
{
    jsonDoc.Add("Name", MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(GetName())));
    jsonDoc.Add("Type", sm_DataType[m_propertyType].c_str());
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Converts data into XML format
/// </summary>
void MgPropertyDefinition::ToXml(string &str, bool includeType, string rootElmName)
{
    str += "<" + rootElmName + ">";

    str += "<Name>";

    str += MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(GetName())) + "</Name>";

    string propType = "";
    // The type has to be one of the data type or property type
    if (includeType)
    {
        propType = sm_PropertyType[m_propertyType];
        if (propType.empty())
        {
            propType = sm_DataType[m_propertyType];
        }
        str += "<Type>" + propType + "</Type>";
    }
    str += "</" + rootElmName + ">";
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Converts data into JSON format
/// </summary>
void MgPropertyDefinition::ToJson(MgJsonDoc &jsonDoc, bool includeType)
{
    jsonDoc.Add("Name", MgUtil::WideCharToMultiByte(MgUtil::ReplaceEscapeCharInXml(GetName())));    
    
    // The type has to be one of the data type or property type
    if (includeType)
    {
        string propType = sm_PropertyType[m_propertyType];
        if (propType.empty())
        {
            propType = sm_DataType[m_propertyType];
        }        
        jsonDoc.Add("Type", propType);
    }
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Serialize data to TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>
void MgPropertyDefinition::Serialize(MgStream* stream)
{
    stream->WriteString(GetName());
    stream->WriteString(GetDescription());
    stream->WriteString(GetQualifiedName());
    stream->WriteInt16((INT16)m_propertyType);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserialize data from TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>
void MgPropertyDefinition::Deserialize(MgStream* stream)
{
    STRING str;
    INT16 type;

    stream->GetString(str);
    SetName(str);

    stream->GetString(str);
    SetDescription(str);

    stream->GetString(str);
    SetQualifiedName(str);

    stream->GetInt16(type);
    m_propertyType = type;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Static Initializer
/// </summary>
bool MgPropertyDefinition::Initialize()
{
    // Property Type for Features
    sm_PropertyType[MgFeaturePropertyType::AssociationProperty]     =          "AssociationProperty";
    sm_PropertyType[MgFeaturePropertyType::DataProperty]            =          "DataProperty";
    sm_PropertyType[MgFeaturePropertyType::GeometricProperty]       =          "GeometricProperty";
    sm_PropertyType[MgFeaturePropertyType::ObjectProperty]          =          "ObjectProperty";
    sm_PropertyType[MgFeaturePropertyType::RasterProperty]          =          "RasterProperty";

    // Data types for Column definition
    sm_DataType[MgPropertyType::Null]     =             "null";      /// Null property (undefined)
    sm_DataType[MgPropertyType::Boolean]  =          "boolean";      /// Boolean true/false value
    sm_DataType[MgPropertyType::Byte]     =             "byte";      /// Unsigned 8 bit value
    sm_DataType[MgPropertyType::DateTime] =         "datetime";      /// DateTime object
    sm_DataType[MgPropertyType::Single]   =           "single";      /// Single precision floating point value
    sm_DataType[MgPropertyType::Double]   =           "double";      /// Double precision floating point value
    sm_DataType[MgPropertyType::Int16]    =            "int16";      /// 16 bit signed integer value
    sm_DataType[MgPropertyType::Int32]    =            "int32";      /// 32 bit signed integer value
    sm_DataType[MgPropertyType::Int64]    =            "int64";      /// 64 bit signed integer value
    sm_DataType[MgPropertyType::String]   =           "string";      /// String MgProperty
    sm_DataType[MgPropertyType::Blob]     =             "blob";      /// binary BLOB
    sm_DataType[MgPropertyType::Clob]     =             "clob";      /// binary CLOB
    sm_DataType[MgPropertyType::Feature]  =          "feature";      /// Feature object
    sm_DataType[MgPropertyType::Geometry] =         "geometry";      /// Geometry object
    sm_DataType[MgPropertyType::Raster]   =           "raster";      /// Raster object

    return true;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the description of this property.
/// </summary>
/// <param name="description">
/// Description of property
/// </param>
/// <returns>
/// Nothing
/// </returns>
void MgPropertyDefinition::SetDescription(CREFSTRING description)
{
    m_description = description;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the description of this property.
/// </summary>
/// <returns>
/// String detailing description of the property
/// </returns>
STRING MgPropertyDefinition::GetDescription()
{
    return m_description;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Sets the qualified name of this property.
/// </summary>
/// <param name="qualifiedName">
/// Qualified name of property
/// </param>
/// <returns>
/// Nothing
/// </returns>
void MgPropertyDefinition::SetQualifiedName(CREFSTRING qualifiedName)
{
    m_qualifiedName = qualifiedName;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the qualified name of this property.
/// </summary>
/// <returns>
/// String detailing qualified name
/// </returns>
STRING MgPropertyDefinition::GetQualifiedName()
{
    return m_qualifiedName;
}
