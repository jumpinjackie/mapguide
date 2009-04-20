//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#include "MapGuideCommon.h"

MG_IMPL_DYNCREATE(MgProxyGwsFeatureReader);

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct a MgProxyGwsFeatureReader object from a byte source
///</summary>
///<param name="byteSource">Byte  source object</param>
///
MgProxyGwsFeatureReader::MgProxyGwsFeatureReader(MgFeatureSet* featureSet)
{
    m_currRecord = 0;
    m_serverGwsFeatureReader = L"";
    m_set = SAFE_ADDREF((MgFeatureSet*)featureSet);
    m_service = NULL;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct an uninitialized MgProxyGwsFeatureReader object
///</summary>
///
MgProxyGwsFeatureReader::MgProxyGwsFeatureReader()
{
    m_currRecord = 0;
    m_serverGwsFeatureReader = L"";
    m_set = NULL;
    m_service = NULL;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Destruct a MgProxyGwsFeatureReader object
///</summary>
///
MgProxyGwsFeatureReader::~MgProxyGwsFeatureReader()
{
    Close();
    SAFE_RELEASE(m_service);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Set the byte source for an empty reader
///</summary>
///<param name="byteSource">Byte source object</param>
///
void MgProxyGwsFeatureReader::AssignFeatureSet(MgFeatureSet* featureSet)
{
    m_currRecord = 0;
    m_set = SAFE_ADDREF((MgFeatureSet*)featureSet);
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
bool MgProxyGwsFeatureReader::ReadNext()
{
    CHECKNULL(m_set, L"MgProxyGwsFeatureReader.ReadNext");

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
            if (m_serverGwsFeatureReader != L"")
            {
                Ptr<MgBatchPropertyCollection> bpCol = m_service->GetFeatures(m_serverGwsFeatureReader);
                m_set->ClearFeatures();
                m_set->AddFeatures(bpCol);
                if (m_set->GetCount() > 0)
                {
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
/// Gets the definition of the object currently being read. If the user
/// has requested only a subset of the class properties, the class
/// definition reflects what the user has asked, rather than the full class
/// definition.
/// </summary>
/// <returns>A MgClassDefinition representing the current object
///</returns>
MgClassDefinition* MgProxyGwsFeatureReader::GetClassDefinition()
{
    CHECKNULL(m_set, L"MgProxyGwsFeatureReader.GetClassDefinition");

    return m_set->GetClassDefinition();
}

MgClassDefinition* MgProxyGwsFeatureReader::GetClassDefinitionNoXml()
{
    //fall back to regular public call in this case
    return GetClassDefinition();
}


//////////////////////////////////////////////////////////////////
/// <summary>
///  Returns true if the value of the specified property is null.
/// </summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns true if the value is null.</returns>
bool MgProxyGwsFeatureReader::IsNull(CREFSTRING propertyName)
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
/// Gets the Boolean value of the specified property. No conversion is
/// performed, thus the property must be a of boolean type the result
/// is undertermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the Boolean value.</returns>
bool MgProxyGwsFeatureReader::GetBoolean(CREFSTRING propertyName)
{
    bool retVal = false;

    Ptr<MgBooleanProperty> ptrProp = (MgBooleanProperty*)GetProperty(propertyName, MgPropertyType::Boolean);
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
BYTE MgProxyGwsFeatureReader::GetByte(CREFSTRING propertyName)
{
    BYTE retVal = 0;

    Ptr<MgByteProperty> ptrProp = (MgByteProperty*)GetProperty(propertyName, MgPropertyType::Byte);
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
MgDateTime* MgProxyGwsFeatureReader::GetDateTime(CREFSTRING propertyName)
{
    Ptr<MgDateTimeProperty> ptrProp = (MgDateTimeProperty*)GetProperty(propertyName, MgPropertyType::DateTime);
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
float MgProxyGwsFeatureReader::GetSingle(CREFSTRING propertyName)
{
    float retVal = 0;

    Ptr<MgSingleProperty> ptrProp = (MgSingleProperty*)GetProperty(propertyName, MgPropertyType::Single);
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
double MgProxyGwsFeatureReader::GetDouble(CREFSTRING propertyName)
{
    double retVal = 0;

    Ptr<MgDoubleProperty> ptrProp = (MgDoubleProperty*)GetProperty(propertyName, MgPropertyType::Double);
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
INT16 MgProxyGwsFeatureReader::GetInt16(CREFSTRING propertyName)
{
    INT16 retVal = 0;

    Ptr<MgInt16Property> ptrProp = (MgInt16Property*)GetProperty(propertyName, MgPropertyType::Int16);
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
INT32 MgProxyGwsFeatureReader::GetInt32(CREFSTRING propertyName)
{
    INT32 retVal = 0;

    Ptr<MgInt32Property> ptrProp = (MgInt32Property*)GetProperty(propertyName, MgPropertyType::Int32);
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
INT64 MgProxyGwsFeatureReader::GetInt64(CREFSTRING propertyName)
{
    INT64 retVal = 0;

    Ptr<MgInt64Property> ptrProp = (MgInt64Property*)GetProperty(propertyName, MgPropertyType::Int64);
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
STRING MgProxyGwsFeatureReader::GetString(CREFSTRING propertyName)
{
    STRING retVal = L"";

    Ptr<MgStringProperty> ptrProp = (MgStringProperty*)GetProperty(propertyName, MgPropertyType::String);
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
MgByteReader* MgProxyGwsFeatureReader::GetBLOB(CREFSTRING propertyName)
{
    Ptr<MgBlobProperty> ptrProp = (MgBlobProperty*)GetProperty(propertyName, MgPropertyType::Blob);
    Ptr<MgByteReader> retVal = ptrProp->GetValue();

    return SAFE_ADDREF((MgByteReader*)retVal);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the CLOB value of the specified property. No conversion is
/// performed, thus the property must be a of type CLOB or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the CLOB value.</returns>
MgByteReader* MgProxyGwsFeatureReader::GetCLOB(CREFSTRING propertyName)
{
    Ptr<MgClobProperty> ptrProp = (MgClobProperty*)GetProperty(propertyName, MgPropertyType::Clob);
    Ptr<MgByteReader> retVal = ptrProp->GetValue();

    return SAFE_ADDREF((MgByteReader*)retVal);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the FeatureReader to access this object value.
/// The property must be of an object type{} otherwise, the result is NULL.
/// </summary>
/// <param name="propertyName">Input the property name.</param>
/// <returns>Returns the feature reader to access this object.</returns>
/// EXCEPTIONS:
/// InvalidPropertyType
MgFeatureReader* MgProxyGwsFeatureReader::GetFeatureObject(CREFSTRING propertyName)
{
    Ptr<MgFeatureProperty> ptrProp = (MgFeatureProperty*)GetProperty(propertyName, MgPropertyType::Feature);
    Ptr<MgFeatureReader> retVal = ptrProp->GetValue();

    return SAFE_ADDREF((MgFeatureReader*)retVal);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Geometry for the specified property. No conversion is
/// performed, thus the property must be a of type Geometry or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns a ByteReader object</returns>
MgByteReader* MgProxyGwsFeatureReader::GetGeometry(CREFSTRING propertyName)
{
    Ptr<MgGeometryProperty> ptrProp = (MgGeometryProperty*)GetProperty(propertyName, MgPropertyType::Geometry);
    Ptr<MgByteReader> retVal = ptrProp->GetValue();

    return SAFE_ADDREF((MgByteReader*)retVal);
}

// Get the property for the specified name
MgProperty* MgProxyGwsFeatureReader::GetProperty(CREFSTRING propertyName, INT16 expectedType)
{
    Ptr<MgNullableProperty> ptrProp = (MgNullableProperty*)this->GetProperty(propertyName);
    CHECKNULL(ptrProp, L"MgProxyGwsFeatureReader.GetProperty");

    if (ptrProp->IsNull())
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgProxyGwsFeatureReader.GetProperty",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    INT16 propType = ptrProp->GetPropertyType();
    MG_CHECK_PROPERTY_TYPE(propType, expectedType, L"MgProxyGwsFeatureReader.GetProperty");

    return SAFE_ADDREF((MgProperty*)ptrProp);
}

// Get the property for the specified name
MgProperty* MgProxyGwsFeatureReader::GetProperty(CREFSTRING propertyName)
{
    CHECKNULL(m_set, L"MgProxyGwsFeatureReader.GetProperty");
    MG_CHECK_FEATURE_SET_COUNT(m_set, L"MgProxyGwsFeatureReader.GetProperty");

    Ptr<MgPropertyCollection> ptrCol = m_set->GetFeatureAt(m_currRecord-1);
    CHECKNULL(ptrCol, L"MgProxyGwsFeatureReader.GetProperty");

    Ptr<MgProperty> ptrProp = ptrCol->GetItem(propertyName);
    CHECKNULL(ptrProp, L"MgProxyGwsFeatureReader.GetProperty");

    return SAFE_ADDREF((MgProperty*)ptrProp);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Serialize data to TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgProxyGwsFeatureReader::Serialize(MgStream* stream)
{
    // This serialize is called from server when Select
    // contains custom computed properties like JENK_CATEGORY, QUANTILE_CATEGORY
    // STDEV_CATEGORY or EQUAL_CATEGORY.
    // This method will never be called in any other case.
    // In this case, there is NO FdoFeatureReader cached on server
    // and therefore m_serverGwsFeatureReader will be empty.

    bool operationCompleted = true;

    stream->WriteBoolean(operationCompleted);

    if (operationCompleted)
    {
        stream->WriteString(m_serverGwsFeatureReader);
        stream->WriteObject((MgFeatureSet*)m_set); // Write the feature set
    }
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserialize data from TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgProxyGwsFeatureReader::Deserialize(MgStream* stream)
{
    bool operationCompleted = false;

    stream->GetBoolean(operationCompleted);

    if (operationCompleted)
    {
        stream->GetString(m_serverGwsFeatureReader);                      // Get the reader ID so we can retrieve it for later use
        m_set = (MgFeatureSet*)stream->GetObject();
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
MgByteReader* MgProxyGwsFeatureReader::ToXml()
{
    string xmlStr;
    this->ToXml(xmlStr);

    Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)xmlStr.c_str(), (INT32)xmlStr.length());
    byteSource->SetMimeType(MgMimeType::Xml);

    Ptr<MgByteReader> byteReader = byteSource->GetReader();
    return SAFE_ADDREF((MgByteReader*)byteReader);
}

void MgProxyGwsFeatureReader::ToXml(string &str)
{
    CHECKNULL(m_set, L"MgProxyGwsFeatureReader.ToXml");

    Ptr<MgClassDefinition> classDef = this->GetClassDefinition();

    if (classDef != NULL)
    {
        // TODO: define a schema for this XML
        // TODO: rename FeatureSet element to avoid conflict with FeatureSet-1.0.0.xsd?
        str += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
        str += "<FeatureSet>";
        classDef->ToXml(str);
        str += "<Features>";
        while ( this->ReadNext() )
        {
            Ptr<MgPropertyCollection> propCol = m_set->GetFeatureAt(m_currRecord-1);
            INT32 cnt = propCol->GetCount();
            if (propCol != NULL && cnt > 0)
            {
                propCol->ToFeature(str);
            }
        }
        str += "</Features>";
        str += "</FeatureSet>";
    }
}

void MgProxyGwsFeatureReader::SetService(MgFeatureService* service)
{
    CHECKNULL(service, L"MgProxyGwsFeatureReader.SetService");

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
void MgProxyGwsFeatureReader::Close()
{
    if (m_serverGwsFeatureReader != L"")
    {
        MG_TRY()

        m_service->CloseFeatureReader(m_serverGwsFeatureReader);
        m_serverGwsFeatureReader = L"";

        MG_CATCH(L"MgProxyGwsFeatureReader.Close")

        // We do not rethrow the exception while destructing the object. Even if we had problem
        // disposing this feature reader, it will automatically get collected after time out.
    }
}


/// <summary>Gets the raster object of the specified property.
/// the property must be of Raster type; otherwise, an exception is thrown.
/// </summary>
/// <param name="propertyName">Input the property name.</param>
/// <returns>Returns the raster object.</returns>
/// EXCEPTIONS:
/// MgConnectionNotOpenException
/// MgNullPropertyValueException
/// MgFdoException
MgRaster* MgProxyGwsFeatureReader::GetRaster(CREFSTRING propertyName)
{
    Ptr<MgRasterProperty> ptrProp = (MgRasterProperty*)GetProperty(propertyName, MgPropertyType::Raster);
    Ptr<MgRaster> retVal = ptrProp->GetValue();
    retVal->SetMgService(m_service);
    retVal->SetHandle(m_serverGwsFeatureReader);

    return SAFE_ADDREF((MgRaster*)retVal);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Geometry for the specified property. No conversion is
/// performed, thus the property must be a of type Geometry or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns a ByteReader object</returns>
BYTE_ARRAY_OUT MgProxyGwsFeatureReader::GetGeometry(CREFSTRING propertyName, INT32& length)
{
    throw new MgNotImplementedException(L"MgProxyGwsFeatureReader.GetGeometry",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the string value of the specified property. No conversion is
/// performed, thus the property must be a of type string or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the string value.</returns>
const wchar_t* MgProxyGwsFeatureReader::GetString(CREFSTRING propertyName, INT32& length)
{
    STRING str = this->GetString(propertyName);
    length = (INT32)str.size();

    return str.c_str();
}


void MgProxyGwsFeatureReader::SetServiceForFeatureProperties(MgFeatureService* service)
{
    if (m_set != NULL)
    {
        INT32 cnt = m_set->GetCount();
        for (INT32 i = 0; i < cnt; i++)
        {
            Ptr<MgPropertyCollection> propCol = m_set->GetFeatureAt(i);
            if (propCol != NULL)
            {
                INT32 cnt = propCol->GetCount();
                for (INT32 i=0; i < cnt; i++)
                {
                    Ptr<MgProperty> prop = propCol->GetItem(i);
                    INT32 propType = prop->GetPropertyType();
                    if (propType == MgPropertyType::Feature)
                    {
                        MgFeatureProperty* featProp = ((MgFeatureProperty*)((MgProperty*)prop));
                        Ptr<MgProxyGwsFeatureReader> reader = (MgProxyGwsFeatureReader*)featProp->GetValue();
                        if (reader != NULL)
                        {
                            reader->SetService(service);
                        }
                    }
                }
            }
        }
    }
}

MgFeatureSet* MgProxyGwsFeatureReader::GetFeatures(INT32 count)
{
    throw new MgNotImplementedException(L"MgProxyGwsFeatureReader.GetFeatures",
        __LINE__, __WFILE__, NULL, L"", NULL);
}
