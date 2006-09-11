//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "ServerFeatureServiceDefs.h"
#include "AceCommon.h"
#include "FeatureService.h"
#include "ServerSqlDataReader.h"
#include "ServerSqlDataReaderPool.h"
#include "ServerFeatureUtil.h"
#include "ServerSqlProcessor.h"

MgServerSqlDataReader::MgServerSqlDataReader(FdoISQLDataReader* sqlReader, CREFSTRING providerName)
{
    MG_FEATURE_SERVICE_TRY()

    m_sqlReader = GIS_SAFE_ADDREF(sqlReader);
    m_providerName = providerName;

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.MgServerSqlDataReader")
}

MgServerSqlDataReader::MgServerSqlDataReader()
{
    m_sqlReader = NULL;
    m_providerName = L"";
}

MgServerSqlDataReader::~MgServerSqlDataReader()
{
    GIS_SAFE_RELEASE(m_sqlReader);
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
bool MgServerSqlDataReader::ReadNext()
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.ReadNext");

    bool retVal = false;

    MG_FEATURE_SERVICE_TRY()

    retVal = m_sqlReader->ReadNext();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.ReadNext")

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the number of properties in the result set.
/// </summary>
/// <returns>Returns the number of properties.</returns>
INT32 MgServerSqlDataReader::GetPropertyCount()
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetPropertyCount");

    INT32 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    retVal = (INT32)m_sqlReader->GetColumnCount();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetPropertyCount")

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the name of the property at the given ordinal position.
/// </summary>
/// <param name="index">Input the position of the property.</param>
/// <returns>Returns the property name</returns>
STRING MgServerSqlDataReader::GetPropertyName(INT32 index)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetPropertyName");

    STRING retVal;

    MG_FEATURE_SERVICE_TRY()

    GisString* str = m_sqlReader->GetColumnName(index);
    if (str != NULL)
    {
        retVal = (wchar_t*)str;
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetPropertyName")

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the data type of the property with the specified name.
/// </summary>
/// <param name="propertyName">Input the property name.</param>
/// <returns>Returns the type of the property.</returns>
INT32 MgServerSqlDataReader::GetPropertyType(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetPropertyType");

    INT32 type = MgPropertyType::Null;

    MG_FEATURE_SERVICE_TRY()

    FdoPropertyType propType = m_sqlReader->GetPropertyType(propertyName.c_str());

    switch(propType)
    {
        // If geometric property, return geometric data type.
        case FdoPropertyType_GeometricProperty:
        {
            type = MgPropertyType::Geometry;
            break;
        }
        case FdoPropertyType_DataProperty:
        {
            FdoDataType dataType = m_sqlReader->GetColumnType(propertyName.c_str());
            type = MgServerFeatureUtil::GetMgPropertyType(dataType);
            break;
        }
        case FdoPropertyType_RasterProperty:
        {
            type = MgPropertyType::Raster;
            break;
        }
        default:
        {
            throw new MgInvalidPropertyTypeException(L"MgServerSqlDataReader.GetPropertyType", __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetPropertyType")

    return type;
}

//////////////////////////////////////////////////////////////////
/// <summary>
///  Returns true if the value of the specified property is null.
/// </summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns true if the value is null.</returns>
bool MgServerSqlDataReader::IsNull(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.IsNull");

    bool retVal = false;

    MG_FEATURE_SERVICE_TRY()

    retVal = m_sqlReader->IsNull(propertyName.c_str());

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.IsNull");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Boolean value of the specified property. No conversion is
/// performed, thus the property must be a of boolean type the result
/// is undertermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the Boolean value.</returns>
bool MgServerSqlDataReader::GetBoolean(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetBoolean");

    bool retVal = false;

    MG_FEATURE_SERVICE_TRY()

    retVal = m_sqlReader->GetBoolean(propertyName.c_str());

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetBoolean");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Byte value of the specified property. No conversion is
/// performed, thus the property must be a of byte type or the result
/// is undertermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the Byte value.</returns>
BYTE MgServerSqlDataReader::GetByte(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetByte");

    BYTE retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    retVal = (BYTE)m_sqlReader->GetByte(propertyName.c_str());

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetByte");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the DTime value of the specified property. No conversion is
/// performed, thus the property must be a of date type or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the DTime value.</returns>
MgDateTime* MgServerSqlDataReader::GetDateTime(CREFSTRING propertyName)
{
    Ptr<MgDateTime> retVal;

    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetDateTime");

    MG_FEATURE_SERVICE_TRY()

    GisDateTime val = m_sqlReader->GetDateTime(propertyName.c_str());
    retVal = new MgDateTime((INT16)val.year, (INT8)val.month, (INT8)val.day,
                            (INT8)val.hour, (INT8)val.minute, val.seconds);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetDateTime");

    return retVal.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Single value of the specified property. No conversion is
/// performed, thus the property must be a of type single or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the single value.</returns>
float MgServerSqlDataReader::GetSingle(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetSingle");

    float retVal = 0.0f;

    MG_FEATURE_SERVICE_TRY()

    retVal = m_sqlReader->GetSingle(propertyName.c_str());

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetSingle");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Double value of the specified property. No conversion is
/// performed, thus the property must be a of type double or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the double value.</returns>
double MgServerSqlDataReader::GetDouble(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetDouble");

    double retVal = 0.0;

    MG_FEATURE_SERVICE_TRY()

    retVal = m_sqlReader->GetDouble(propertyName.c_str());

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetDouble");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 16 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 16 bits or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the integer 16 bits value.</returns>
INT16 MgServerSqlDataReader::GetInt16(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetInt16");

    INT16 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    retVal = (INT16)m_sqlReader->GetInt16(propertyName.c_str());

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetInt16");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 32 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 32 bits or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the integer 32 bits value.</returns>
INT32 MgServerSqlDataReader::GetInt32(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetInt32");

    INT32 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    retVal = (INT32)m_sqlReader->GetInt32(propertyName.c_str());

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetInt32");

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
INT64 MgServerSqlDataReader::GetInt64(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetInt64");

    INT64 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    retVal = (INT64)m_sqlReader->GetInt64(propertyName.c_str());

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetInt64");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the string value of the specified property. No conversion is
/// performed, thus the property must be a of type string or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the string value.</returns>
STRING MgServerSqlDataReader::GetString(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetString");

    STRING retVal = L"";

    MG_FEATURE_SERVICE_TRY()

    INT32 length = 0;
    const wchar_t* str = this->GetString(propertyName.c_str(), length);
    if (str != NULL)
    {
        retVal = str;
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetString");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the BLOB value of the specified property. No conversion is
/// performed, thus the property must be a of type BLOBs or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the BLOB value.</returns>
MgByteReader* MgServerSqlDataReader::GetBLOB(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetBLOB");

    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    byteReader = this->GetLOB(propertyName);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetBLOB");

    return byteReader.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the CLOB value of the specified property. No conversion is
/// performed, thus the property must be a of type CLOB or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the CLOB value.</returns>
MgByteReader* MgServerSqlDataReader::GetCLOB(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetCLOB");

    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    byteReader = this->GetLOB(propertyName);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetCLOB");

    return byteReader.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Geometry for the specified property. No conversion is
/// performed, thus the property must be a of type Geometry or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns a ByteReader object</returns>
MgByteReader* MgServerSqlDataReader::GetGeometry(CREFSTRING propertyName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetGeometry");

    Ptr<MgByteReader> retVal;

    MG_FEATURE_SERVICE_TRY()

    INT32 len;
    BYTE_ARRAY_OUT data = this->GetGeometry(propertyName.c_str(), len);

    if (data != NULL)
    {
        Ptr<MgByte> mgBytes = new MgByte((BYTE_ARRAY_IN)data, len);
        Ptr<MgByteSource> bSource = new MgByteSource(mgBytes);
        bSource->SetMimeType(MgMimeType::Agf);
        retVal = bSource->GetReader();
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.GetGeometry");

    return retVal.Detach();
}


MgByteReader* MgServerSqlDataReader::GetLOB(CREFSTRING propName)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetLOB");

    Ptr<MgByteReader> byteReader;

    // TODO: We need to switch to GisIStreamReader when we have streaming capability in MgByteReader
    GisPtr<FdoLOBValue> fdoVal = m_sqlReader->GetLOB(propName.c_str());
    if (fdoVal != NULL)
    {
        GisPtr<GisByteArray> byteArray = fdoVal->GetData();
        if (byteArray != NULL)
        {
            GisByte* bytes = byteArray->GetData();
            GisInt32 len = byteArray->GetCount();
            Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)bytes,(INT32)len);
            // TODO: We need to differentiate between CLOB and BLOB
            // TODO: How do we fine the MimeType of data for CLOB
            byteSource->SetMimeType(MgMimeType::Binary);
            byteReader = byteSource->GetReader();
        }
    }
    return byteReader.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Releases all the resources of feature reader.
/// This must be called when user is done with Feature Reader
/// <returns>Nothing</returns>
void MgServerSqlDataReader::Close()
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.Close");

    MG_FEATURE_SERVICE_TRY()

    m_sqlReader->Close();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerSqlDataReader.Close");
}

void MgServerSqlDataReader::Serialize(MgStream* stream)
{
    INT32 count = 1; // Get value from MgConfiguration

    Ptr<MgPropertyDefinitionCollection> propDefCol;
    Ptr<MgBatchPropertyCollection> bpCol;
    Ptr<MgServerSqlProcessor> sqlProcessor;
    bool operationCompleted = false;

    MG_FEATURE_SERVICE_TRY()

    // Find out the counts from Configuration
    MgConfiguration* config = MgConfiguration::GetInstance();

    config->GetIntValue(MgConfigProperties::FeatureServicePropertiesSection,
                        MgConfigProperties::FeatureServicePropertyDataCacheSize,
                        count,
                        MgConfigProperties::DefaultFeatureServicePropertyDataCacheSize);

    // Collect the sql reader into a pool for ReadNext operation
    MgServerSqlDataReaderPool* dataReaderPool = MgServerSqlDataReaderPool::GetInstance();
    CHECKNULL((MgServerSqlDataReaderPool*)dataReaderPool, L"MgServerSqlDataReader.Serialize");

    sqlProcessor = new MgServerSqlProcessor(this);
    dataReaderPool->Add(sqlProcessor);                  // Add the reference to pool

    propDefCol = sqlProcessor->GetColumnDefinitions();  // Get column definitions
    bpCol = sqlProcessor->GetRows(count);               // Get rows

    operationCompleted = true;

    MG_FEATURE_SERVICE_CATCH(L"MgServerSqlDataReader.Serialize");

    // Mark operation is completed successfully
    stream->WriteBoolean(operationCompleted);

    if (operationCompleted && (mgException == 0))
    {
        stream->WriteInt32((INT32)sqlProcessor.p);                              // Write the pointer value so we can retrieve it for later use
        stream->WriteString(m_providerName);                                    // Provider Name for XML
        stream->WriteObject((MgPropertyDefinitionCollection*)propDefCol);       // Write the property definition
        stream->WriteObject((MgBatchPropertyCollection*)bpCol);                 // Write the property data
    }
    else
    {
        stream->WriteObject((MgException*)mgException);
    }

    MG_FEATURE_SERVICE_THROW();
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserialize data from TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgServerSqlDataReader::Deserialize(MgStream* stream)
{
    throw new MgInvalidOperationException(L"MgServerSqlDataReader.Deserialize",__LINE__, __WFILE__, NULL, L"", NULL);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Serializes all features into an XML.
/// XML is serialized from the current position of feature reader in the order
/// data are retrieved.
/// <returns>MgByteReader holding XML.</returns>
MgByteReader* MgServerSqlDataReader::ToXml()
{
    throw new MgInvalidOperationException(L"MgServerSqlDataReader.ToXml",__LINE__, __WFILE__, NULL, L"", NULL);
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
MgRaster* MgServerSqlDataReader::GetRaster(CREFSTRING propertyName)
{
    throw new MgInvalidPropertyTypeException(L"MgProxySqlDataReader.GetRaster", __LINE__, __WFILE__, NULL, L"", NULL);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Geometry for the specified property. No conversion is
/// performed, thus the property must be a of type Geometry or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns a ByteReader object</returns>
BYTE_ARRAY_OUT MgServerSqlDataReader::GetGeometry(CREFSTRING propertyName, INT32& length)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetGeometry");

    // TODO: Can we have an equivalent method as we have in FeatureReader to get
    // TODO: direct pointer on geometry
    GisByte* data = NULL;

    if(!m_sqlReader->IsNull(propertyName.c_str()))
    {
        GisPtr<GisByteArray> byteArray = m_sqlReader->GetGeometry(propertyName.c_str());
        length = (INT32)byteArray->GetCount();
        GisByte* data = byteArray->GetData();
    }

    return (BYTE_ARRAY_OUT)data;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the string value of the specified property. No conversion is
/// performed, thus the property must be a of type string or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the string value.</returns>
const wchar_t* MgServerSqlDataReader::GetString(CREFSTRING propName, INT32& length)
{
    CHECKNULL(m_sqlReader, L"MgServerSqlDataReader.GetString");

    GisString* retVal;

    MG_FEATURE_SERVICE_TRY()

    retVal = m_sqlReader->GetString(propName.c_str());
    if (retVal != NULL)
    {
        length = (INT32)wcslen((const wchar_t*)retVal);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetString");

    return ((const wchar_t*)retVal);
}
