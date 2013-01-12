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

#include "ProxyDataReader.h"

MG_IMPL_DYNCREATE(MgdProxyDataReader);

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct an uninitialized MgdProxyDataReader object
///</summary>
///
MgdProxyDataReader::MgdProxyDataReader()
{
    m_currRecord = 0;
    m_serverDataReader = L"";
    m_service = NULL;
    m_set = NULL;
    m_propDefCol = NULL;
}

MgdProxyDataReader::MgdProxyDataReader(MgBatchPropertyCollection* batchCol, MgPropertyDefinitionCollection* propDefCol)
{
    m_currRecord = 0;
    m_serverDataReader = L"";
    m_service = NULL;
    m_set = SAFE_ADDREF(batchCol);
    m_propDefCol = SAFE_ADDREF(propDefCol);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Destruct a MgdProxyDataReader object
///</summary>
///
MgdProxyDataReader::~MgdProxyDataReader()
{
    Close();
    SAFE_RELEASE(m_service);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Advances the reader to the next item and returns true if there is
/// another object to read or false if reading is complete. The default
/// position of the reader is prior to the first item. Thus you must
/// call ReadNext to begin accessing any data.
/// </summary>
/// <returns>
/// Returns true if there is a next item.
/// </returns>
bool MgdProxyDataReader::ReadNext()
{
    CHECKNULL(m_set, L"MgdProxyDataReader.ReadNext");

    bool foundNextFeature = false;

    INT32 cnt = m_set->GetCount();

    if ( m_currRecord < cnt )
    {
        m_currRecord++;
        foundNextFeature = true;
    }
    else
    {
        // Fetch next set of records from server
        try
        {
            m_currRecord = 0;
            if (m_serverDataReader != L"")
            {
                Ptr<MgBatchPropertyCollection> bpCol = m_service->GetDataRows(m_serverDataReader);

                if ((((MgBatchPropertyCollection*)bpCol) != NULL) && (bpCol->GetCount() > 0))
                {
                    UpdateCurrentSet(bpCol);
                    foundNextFeature = true;
                    m_currRecord++;
                }
            }
        }
        catch (MgException* me)
        {
            SAFE_RELEASE(me);
        }
    }

    return foundNextFeature;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the number of properties in the result set.
/// </summary>
/// <returns>Returns the number of properties.</returns>
INT32 MgdProxyDataReader::GetPropertyCount()
{
    CHECKNULL(m_propDefCol, L"MgdProxyDataReader.GetPropertyCount");

    return m_propDefCol->GetCount();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the name of the property at the given ordinal position.
/// </summary>
/// <param name="index">Input the position of the property.</param>
/// <returns>Returns the property name</returns>
STRING MgdProxyDataReader::GetPropertyName(INT32 index)
{
    CHECKNULL(m_propDefCol, L"MgdProxyDataReader.GetPropertyName");

    Ptr<MgPropertyDefinition> propDef = m_propDefCol->GetItem(index);
    return propDef->GetName();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the index of the property with the specified name.
/// </summary>
/// <param name="propertyName">Input the name of the property.</param>
/// <returns>Returns the property index</returns>
INT32 MgdProxyDataReader::GetPropertyIndex(CREFSTRING propertyName)
{
    CHECKNULL(m_propDefCol, L"MgdProxyDataReader.GetPropertyIndex");

    return m_propDefCol->IndexOf(propertyName);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the data type of the property with the specified name.
/// </summary>
/// <param name="propertyName">Input the property name.</param>
/// <returns>Returns the type of the property.</returns>
INT32 MgdProxyDataReader::GetPropertyType(CREFSTRING propertyName)
{
    CHECKNULL(m_propDefCol, L"MgdProxyDataReader.GetPropertyType");

    Ptr<MgPropertyDefinition> propDef = m_propDefCol->GetItem(propertyName);
    CHECKNULL((MgPropertyDefinition*)propDef, L"MgdProxyDataReader.GetPropertyType");

    return propDef->GetPropertyType();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the data type of the property at the specified index.
/// </summary>
/// <param name="index">Input the property index.</param>
/// <returns>Returns the type of the property.</returns>
INT32 MgdProxyDataReader::GetPropertyType(INT32 index)
{
    CHECKNULL(m_propDefCol, L"MgdProxyDataReader.GetPropertyType");

    Ptr<MgPropertyDefinition> propDef = m_propDefCol->GetItem(index);
    CHECKNULL((MgPropertyDefinition*)propDef, L"MgdProxyDataReader.GetPropertyType");

    return propDef->GetPropertyType();
}

//////////////////////////////////////////////////////////////////
/// <summary>
///  Returns true if the value of the specified property is null.
/// </summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns true if the value is null.</returns>
bool MgdProxyDataReader::IsNull(CREFSTRING propertyName)
{
    bool isNull = false;

    Ptr<MgNullableProperty> ptrProp = (MgNullableProperty*)GetProperty(propertyName);

    if (ptrProp != NULL)
    {
        isNull = ptrProp->IsNull();
    }

    return isNull;
}

//////////////////////////////////////////////////////////////////
/// <summary>
///  Returns true if the value of the specified property is null.
/// </summary>
/// <param name="index">Property index.</param>
/// <returns>Returns true if the value is null.</returns>
bool MgdProxyDataReader::IsNull(INT32 index)
{
    bool isNull = false;

    Ptr<MgNullableProperty> ptrProp = (MgNullableProperty*)GetProperty(index);

    if (ptrProp != NULL)
    {
        isNull = ptrProp->IsNull();
    }

    return isNull;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Boolean value of the specified property. No conversion is
/// performed, thus the property must be a of boolean type the result
/// is undertermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the Boolean value.</returns>
bool MgdProxyDataReader::GetBoolean(CREFSTRING propertyName)
{
    bool retVal = false;

    Ptr<MgBooleanProperty> ptrProp = (MgBooleanProperty*)GetProperty(propertyName, MgPropertyType::Boolean);
    retVal = ptrProp->GetValue();

    return retVal;

}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Boolean value of the specified property. No conversion is
/// performed, thus the property must be a of boolean type the result
/// is undertermined</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the Boolean value.</returns>
bool MgdProxyDataReader::GetBoolean(INT32 index)
{
    bool retVal = false;

    Ptr<MgBooleanProperty> ptrProp = (MgBooleanProperty*)GetProperty(index, MgPropertyType::Boolean);
    retVal = ptrProp->GetValue();

    return retVal;

}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Byte value of the specified property. No conversion is
/// performed, thus the property must be a of byte type or the result
/// is undertermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the Byte value.</returns>
BYTE MgdProxyDataReader::GetByte(CREFSTRING propertyName)
{
    BYTE retVal = 0;

    Ptr<MgByteProperty> ptrProp = (MgByteProperty*)GetProperty(propertyName, MgPropertyType::Byte);
    retVal = ptrProp->GetValue();

    return retVal;

}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Byte value of the specified property. No conversion is
/// performed, thus the property must be a of byte type or the result
/// is undertermined</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the Byte value.</returns>
BYTE MgdProxyDataReader::GetByte(INT32 index)
{
    BYTE retVal = 0;

    Ptr<MgByteProperty> ptrProp = (MgByteProperty*)GetProperty(index, MgPropertyType::Byte);
    retVal = ptrProp->GetValue();

    return retVal;

}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the DTime value of the specified property. No conversion is
/// performed, thus the property must be a of date type or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the DTime value.</returns>
MgDateTime* MgdProxyDataReader::GetDateTime(CREFSTRING propertyName)
{
    Ptr<MgDateTimeProperty> ptrProp = (MgDateTimeProperty*)GetProperty(propertyName, MgPropertyType::DateTime);
    Ptr<MgDateTime> retVal = ptrProp->GetValue();

    return SAFE_ADDREF((MgDateTime*)retVal);

}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the DTime value of the specified property. No conversion is
/// performed, thus the property must be a of date type or the result
/// is NULL</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the DTime value.</returns>
MgDateTime* MgdProxyDataReader::GetDateTime(INT32 index)
{
    Ptr<MgDateTimeProperty> ptrProp = (MgDateTimeProperty*)GetProperty(index, MgPropertyType::DateTime);
    Ptr<MgDateTime> retVal = ptrProp->GetValue();

    return SAFE_ADDREF((MgDateTime*)retVal);

}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Single value of the specified property. No conversion is
/// performed, thus the property must be a of type single or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the single value.</returns>
float MgdProxyDataReader::GetSingle(CREFSTRING propertyName)
{
    float retVal = 0;

    Ptr<MgSingleProperty> ptrProp = (MgSingleProperty*)GetProperty(propertyName, MgPropertyType::Single);
    retVal = ptrProp->GetValue();

    return retVal;

}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Single value of the specified property. No conversion is
/// performed, thus the property must be a of type single or the result
/// is undetermined</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the single value.</returns>
float MgdProxyDataReader::GetSingle(INT32 index)
{
    float retVal = 0;

    Ptr<MgSingleProperty> ptrProp = (MgSingleProperty*)GetProperty(index, MgPropertyType::Single);
    retVal = ptrProp->GetValue();

    return retVal;

}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Double value of the specified property. No conversion is
/// performed, thus the property must be a of type double or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the double value.</returns>
double MgdProxyDataReader::GetDouble(CREFSTRING propertyName)
{
    double retVal = 0;

    Ptr<MgDoubleProperty> ptrProp = (MgDoubleProperty*)GetProperty(propertyName, MgPropertyType::Double);
    retVal = ptrProp->GetValue();

    return retVal;

}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Double value of the specified property. No conversion is
/// performed, thus the property must be a of type double or the result
/// is undetermined</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the double value.</returns>
double MgdProxyDataReader::GetDouble(INT32 index)
{
    double retVal = 0;

    Ptr<MgDoubleProperty> ptrProp = (MgDoubleProperty*)GetProperty(index, MgPropertyType::Double);
    retVal = ptrProp->GetValue();

    return retVal;

}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 16 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 16 bits or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the integer 16 bits value.</returns>
INT16 MgdProxyDataReader::GetInt16(CREFSTRING propertyName)
{
    INT16 retVal = 0;

    Ptr<MgInt16Property> ptrProp = (MgInt16Property*)GetProperty(propertyName, MgPropertyType::Int16);
    retVal = ptrProp->GetValue();

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 16 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 16 bits or the result
/// is undetermined</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the integer 16 bits value.</returns>
INT16 MgdProxyDataReader::GetInt16(INT32 index)
{
    INT16 retVal = 0;

    Ptr<MgInt16Property> ptrProp = (MgInt16Property*)GetProperty(index, MgPropertyType::Int16);
    retVal = ptrProp->GetValue();

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 32 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 32 bits or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the integer 32 bits value.</returns>
INT32 MgdProxyDataReader::GetInt32(CREFSTRING propertyName)
{
    INT32 retVal = 0;

    Ptr<MgInt32Property> ptrProp = (MgInt32Property*)GetProperty(propertyName, MgPropertyType::Int32);
    retVal = ptrProp->GetValue();

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 32 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 32 bits or the result
/// is undetermined</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the integer 32 bits value.</returns>
INT32 MgdProxyDataReader::GetInt32(INT32 index)
{
    INT32 retVal = 0;

    Ptr<MgInt32Property> ptrProp = (MgInt32Property*)GetProperty(index, MgPropertyType::Int32);
    retVal = ptrProp->GetValue();

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 64 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 64 bits or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the integer 64 bits value.
/// Note: INT64 is actually a pointer to an Integer64 object
///</returns>
INT64 MgdProxyDataReader::GetInt64(CREFSTRING propertyName)
{
    INT64 retVal = 0;

    Ptr<MgInt64Property> ptrProp = (MgInt64Property*)GetProperty(propertyName, MgPropertyType::Int64);
    retVal = ptrProp->GetValue();

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 64 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 64 bits or the result
/// is NULL</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the integer 64 bits value.
/// Note: INT64 is actually a pointer to an Integer64 object
///</returns>
INT64 MgdProxyDataReader::GetInt64(INT32 index)
{
    INT64 retVal = 0;

    Ptr<MgInt64Property> ptrProp = (MgInt64Property*)GetProperty(index, MgPropertyType::Int64);
    retVal = ptrProp->GetValue();

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the string value of the specified property. No conversion is
/// performed, thus the property must be a of type string or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the string value.</returns>
STRING MgdProxyDataReader::GetString(CREFSTRING propertyName)
{
    STRING retVal = L"";

    Ptr<MgStringProperty> ptrProp = (MgStringProperty*)GetProperty(propertyName, MgPropertyType::String);
    retVal = ptrProp->GetValue();

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the string value of the specified property. No conversion is
/// performed, thus the property must be a of type string or the result
/// is NULL</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the string value.</returns>
STRING MgdProxyDataReader::GetString(INT32 index)
{
    STRING retVal = L"";

    Ptr<MgStringProperty> ptrProp = (MgStringProperty*)GetProperty(index, MgPropertyType::String);
    retVal = ptrProp->GetValue();

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the BLOB value of the specified property. No conversion is
/// performed, thus the property must be a of type BLOBs or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the BLOB value.</returns>
MgByteReader* MgdProxyDataReader::GetBLOB(CREFSTRING propertyName)
{
    Ptr<MgBlobProperty> ptrProp = (MgBlobProperty*)GetProperty(propertyName, MgPropertyType::Blob);
    return ptrProp->GetValue();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the BLOB value of the specified property. No conversion is
/// performed, thus the property must be a of type BLOBs or the result
/// is NULL</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the BLOB value.</returns>
MgByteReader* MgdProxyDataReader::GetBLOB(INT32 index)
{
    Ptr<MgBlobProperty> ptrProp = (MgBlobProperty*)GetProperty(index, MgPropertyType::Blob);
    return ptrProp->GetValue();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the CLOB value of the specified property. No conversion is
/// performed, thus the property must be a of type CLOB or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the CLOB value.</returns>
MgByteReader* MgdProxyDataReader::GetCLOB(CREFSTRING propertyName)
{
    Ptr<MgClobProperty> ptrProp = (MgClobProperty*)GetProperty(propertyName, MgPropertyType::Clob);
    return ptrProp->GetValue();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the CLOB value of the specified property. No conversion is
/// performed, thus the property must be a of type CLOB or the result
/// is NULL</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the CLOB value.</returns>
MgByteReader* MgdProxyDataReader::GetCLOB(INT32 index)
{
    Ptr<MgClobProperty> ptrProp = (MgClobProperty*)GetProperty(index, MgPropertyType::Clob);
    return ptrProp->GetValue();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Geometry for the specified property. No conversion is
/// performed, thus the property must be a of type Geometry or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns a ByteReader object</returns>
MgByteReader* MgdProxyDataReader::GetGeometry(CREFSTRING propertyName)
{
    Ptr<MgGeometryProperty> ptrProp = (MgGeometryProperty*)GetProperty(propertyName, MgPropertyType::Geometry);
    return ptrProp->GetValue();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Geometry for the specified property. No conversion is
/// performed, thus the property must be a of type Geometry or the result
/// is NULL</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns a ByteReader object</returns>
MgByteReader* MgdProxyDataReader::GetGeometry(INT32 index)
{
    Ptr<MgGeometryProperty> ptrProp = (MgGeometryProperty*)GetProperty(index, MgPropertyType::Geometry);
    return ptrProp->GetValue();
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Serialize data to TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgdProxyDataReader::Serialize(MgStream* stream)
{
    bool operationCompleted = true;
    stream->WriteBoolean(operationCompleted);

    if (operationCompleted)
    {
        stream->WriteString(m_serverDataReader);                        // Write the reader ID so we can retrieve it for later use
        stream->WriteString(m_providerName);
        stream->WriteObject(m_propDefCol);                              // Write the property definition
        stream->WriteObject(m_set);                                     // Write the property data
    }
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserialize data from TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgdProxyDataReader::Deserialize(MgStream* stream)
{
    bool operationCompleted = false;

    stream->GetBoolean(operationCompleted);

    if (operationCompleted)
    {
        stream->GetString(m_serverDataReader);                                  // Get the reader ID so we can retrieve it for later use
        stream->GetString(m_providerName);
        m_propDefCol = (MgPropertyDefinitionCollection*)stream->GetObject();    // Get the property definition
        m_set = (MgBatchPropertyCollection*)stream->GetObject();                // Get the property data
    }
    else
    {
        MgException* exp = (MgException*)stream->GetObject();
        exp->Raise();
    }
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Serializes all features into an XML.
/// XML is serialized from the current position of feature reader in the order
/// data are retrieved.
/// <returns>MgByteReader holding XML.</returns>
MgByteReader* MgdProxyDataReader::ToXml()
{
    string xmlStr;
    this->ToXml(xmlStr);

    Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)xmlStr.c_str(), (INT32)xmlStr.length());
    byteSource->SetMimeType(MgMimeType::Xml);

    return byteSource->GetReader();
}

void MgdProxyDataReader::ToXml(string &str)
{
    CHECKNULL((MgBatchPropertyCollection*)m_set, L"MgdProxyDataReader.ToXml");
    CHECKNULL((MgPropertyDefinitionCollection*)m_propDefCol, L"MgdProxyDataReader.ToXml");

    // this XML follows the SelectAggregate-1.0.0.xsd schema
    str += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    str += "<PropertySet>";
    m_propDefCol->ToXml(str);
    str += "<Properties>";
    while ( this->ReadNext() )
    {
        Ptr<MgPropertyCollection> propCol = m_set->GetItem(m_currRecord-1);
        INT32 cnt = propCol->GetCount();
        if (propCol != NULL && cnt > 0)
        {
            str += "<PropertyCollection>";
            propCol->ToXml(str,false);
            str += "</PropertyCollection>";
        }
    }
    str += "</Properties>";
    str += "</PropertySet>";
}

string MgdProxyDataReader::GetResponseElementName()
{
    return "PropertySet";
}

string MgdProxyDataReader::GetBodyElementName()
{
    return "Properties";
}

void MgdProxyDataReader::ResponseStartUtf8(string& str)
{
    str += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    str += "<";
    str += GetResponseElementName();
    str += ">";
}

void MgdProxyDataReader::ResponseEndUtf8(string& str)
{
    str += "</";
    str += GetResponseElementName();
    str += ">";
}

void MgdProxyDataReader::BodyStartUtf8(string& str)
{
    str += "<";
    str += GetBodyElementName();
    str += ">";
}

void MgdProxyDataReader::BodyEndUtf8(string& str)
{
    str += "</";
    str += GetBodyElementName();
    str += ">";
}

void MgdProxyDataReader::HeaderToStringUtf8(string& str)
{
    if (NULL != (MgPropertyDefinitionCollection*)m_propDefCol)
    {
        m_propDefCol->ToXml(str);
    }
}

void MgdProxyDataReader::CurrentToStringUtf8(string& str)
{
    if (NULL != (MgBatchPropertyCollection*)m_set)
    {
        Ptr<MgPropertyCollection> propCol = m_set->GetItem(m_currRecord-1);
        INT32 cnt = propCol->GetCount();
        if (propCol != NULL && cnt > 0)
        {
            str += "<PropertyCollection>";
            propCol->ToXml(str, false);
            str += "</PropertyCollection>";
        }
    }
}

void MgdProxyDataReader::SetService(MgFeatureService* service)
{
    CHECKNULL(service, L"MgdProxyDataReader.SetService");

    if (m_service == NULL)
    {
        m_service = SAFE_ADDREF(service);
    }
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Releases all the resources of feature reader.
/// This must be called when user is done with Feature Reader
/// <returns>Nothing</returns>
void MgdProxyDataReader::Close()
{
    if (m_serverDataReader != L"")
    {
        MG_TRY()

        m_service->CloseDataReader(m_serverDataReader);
        m_serverDataReader = L"";

        MG_CATCH(L"MgdProxyDataReader.Close")

        // We do not rethrow the exception while destructing the object. Even if we had problem
        // disposing this feature reader, it will automatically get collected after time out.
    }
}


// Get the property for the specified name
MgProperty* MgdProxyDataReader::GetProperty(CREFSTRING propertyName, INT16 expectedType)
{
    Ptr<MgNullableProperty> ptrProp = (MgNullableProperty*)this->GetProperty(propertyName);
    CHECKNULL(ptrProp, L"MgdProxyDataReader.GetProperty");

    if (ptrProp->IsNull())
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgdProxyDataReader.GetProperty",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    INT16 propType = ptrProp->GetPropertyType();
    MG_CHECK_PROPERTY_TYPE(propType, expectedType, L"MgdProxyDataReader.GetProperty");

    return SAFE_ADDREF((MgProperty*)ptrProp);
}

// Get the property for the specified name
MgProperty* MgdProxyDataReader::GetProperty(CREFSTRING propertyName)
{
    CHECKNULL(m_set, L"MgdProxyDataReader.GetProperty");
    MG_CHECK_FEATURE_SET_COUNT(m_set, L"MgdProxyDataReader.GetProperty");

    Ptr<MgPropertyCollection> ptrCol = m_set->GetItem(m_currRecord-1);
    CHECKNULL(ptrCol, L"MgdProxyDataReader.GetProperty");

    Ptr<MgProperty> ptrProp = ptrCol->GetItem(propertyName);
    CHECKNULL(ptrProp, L"MgdProxyDataReader.GetProperty");

    return SAFE_ADDREF((MgProperty*)ptrProp);
}

// Get the property for the specified name
MgProperty* MgdProxyDataReader::GetProperty(INT32 index, INT16 expectedType)
{
    Ptr<MgNullableProperty> ptrProp = (MgNullableProperty*)this->GetProperty(index);
    CHECKNULL(ptrProp, L"MgdProxyDataReader.GetProperty");

    if (ptrProp->IsNull())
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);

        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgdProxyDataReader.GetProperty",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    INT16 propType = ptrProp->GetPropertyType();
    MG_CHECK_PROPERTY_TYPE(propType, expectedType, L"MgdProxyDataReader.GetProperty");

    return SAFE_ADDREF((MgProperty*)ptrProp);
}

// Get the property for the specified index
MgProperty* MgdProxyDataReader::GetProperty(INT32 index)
{
    CHECKNULL(m_set, L"MgdProxyDataReader.GetProperty");
    MG_CHECK_FEATURE_SET_COUNT(m_set, L"MgdProxyDataReader.GetProperty");

    Ptr<MgPropertyCollection> ptrCol = m_set->GetItem(m_currRecord-1);
    CHECKNULL(ptrCol, L"MgdProxyDataReader.GetProperty");

    Ptr<MgProperty> ptrProp = ptrCol->GetItem(index);
    CHECKNULL(ptrProp, L"MgdProxyDataReader.GetProperty");

    return SAFE_ADDREF((MgProperty*)ptrProp);
}

void MgdProxyDataReader::UpdateCurrentSet(MgBatchPropertyCollection* bpCol)
{
    CHECKNULL((MgBatchPropertyCollection*)m_set, L"MgdProxyDataReader.UpdateCurrentSet");
    CHECKNULL((MgBatchPropertyCollection*)bpCol, L"MgdProxyDataReader.UpdateCurrentSet");

    m_set->Clear();

    // It is only reference copy
    INT32 cnt = bpCol->GetCount();
    for (INT32 i=0; i < cnt; i++)
    {
        Ptr<MgPropertyCollection> propCol = bpCol->GetItem(i);
        m_set->Add(propCol);
    }
}

/// <summary>Gets the raster object of the specified property.
/// the property must be of Raster type; otherwise, an exception is thrown.
/// </summary>
/// <param name="propertyName">Input the property name.</param>
/// <returns>Returns the raster object.</returns>
MgRaster* MgdProxyDataReader::GetRaster(CREFSTRING propertyName)
{
    Ptr<MgRasterProperty> ptrProp = (MgRasterProperty*)GetProperty(propertyName, MgPropertyType::Raster);
    Ptr<MgRaster> retVal = ptrProp->GetValue();
    retVal->SetMgService(m_service);
    retVal->SetHandle(m_serverDataReader);

    return SAFE_ADDREF((MgRaster*)retVal);
}

/// <summary>Gets the raster object of the specified property.
/// the property must be of Raster type; otherwise, an exception is thrown.
/// </summary>
/// <param name="index">Input the property index.</param>
/// <returns>Returns the raster object.</returns>
MgRaster* MgdProxyDataReader::GetRaster(INT32 index)
{
    Ptr<MgRasterProperty> ptrProp = (MgRasterProperty*)GetProperty(index, MgPropertyType::Raster);
    Ptr<MgRaster> retVal = ptrProp->GetValue();
    retVal->SetMgService(m_service);
    retVal->SetHandle(m_serverDataReader);

    return SAFE_ADDREF((MgRaster*)retVal);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the string value of the specified property. No conversion is
/// performed, thus the property must be a of type string or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the string value.</returns>
const wchar_t* MgdProxyDataReader::GetString(CREFSTRING propertyName, INT32& length)
{
    STRING str = this->GetString(propertyName);
    length = (INT32)str.size();

    return str.c_str();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the string value of the specified property. No conversion is
/// performed, thus the property must be a of type string or the result
/// is NULL</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the string value.</returns>
const wchar_t* MgdProxyDataReader::GetString(INT32 index, INT32& length)
{
    STRING str = this->GetString(index);
    length = (INT32)str.size();

    return str.c_str();
}
