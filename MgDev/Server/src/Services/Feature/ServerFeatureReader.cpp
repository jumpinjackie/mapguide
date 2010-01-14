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

#include "ServerFeatureServiceDefs.h"
#include "ServerFeatureReader.h"
#include "ServerFeatureReaderPool.h"
#include "Envelope.h"
#include "ServiceManager.h"
#include "ServerFeatureUtil.h"

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct a MgServerFeatureReader object from a byte source
///</summary>
///<param name="byteSource">Byte  source object</param>
///
MgServerFeatureReader::MgServerFeatureReader(MgServerFeatureConnection* connection, FdoIFeatureReader* fdoReader)
{
    MG_FEATURE_SERVICE_TRY()

    m_classDef = NULL;
    m_featureSet = NULL;
    m_connection = SAFE_ADDREF(connection);
    m_fdoReader = FDO_SAFE_ADDREF(fdoReader);
    m_removeFromPoolOnDestruction = false;

    // The reader takes ownership of the FDO connection
    m_connection->OwnReader();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureReader.MgServerFeatureReader")
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct an uninitialized MgServerFeatureReader object
///</summary>
///
MgServerFeatureReader::MgServerFeatureReader()
{
    m_classDef = NULL;
    m_featureSet = NULL;
    m_connection = NULL;
    m_fdoReader = NULL;
    m_removeFromPoolOnDestruction = false;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Destruct a MgServerFeatureReader object
///</summary>
///
MgServerFeatureReader::~MgServerFeatureReader()
{
    // DO NOT Close() the FDO reader from here or free the reader resources because
    // we may be reading incrementally from the web tier and the ServerFeatureInstance
    // will live much shorter than the Proxy reader on the web tier which needs to
    // keep reading from the underlying FDO reader.
}

//////////////////////////////////////////////
///<summary>
/// Dispose this object.
///</summary>
void MgServerFeatureReader::Dispose()
{
    delete this;
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
bool MgServerFeatureReader::ReadNext()
{
    CHECKNULL(m_fdoReader, L"MgServerFeatureReader.ReadNext");

    bool retVal = false;

    MG_FEATURE_SERVICE_TRY()

    retVal = m_fdoReader->ReadNext();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureReader.ReadNext")

    return retVal;
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
MgClassDefinition* MgServerFeatureReader::GetClassDefinition()
{
    CHECKNULL(m_fdoReader, L"MgServerFeatureReader.GetClassDefinition");

    MG_FEATURE_SERVICE_TRY()

    // Check to see if the class definition cached contains the XML information
    bool bGetClassDefinition = true;
    if (NULL != (MgClassDefinition*)m_classDef)
    {
        if(m_classDef->HasSerializedXml())
        {
            // We have the full object so don't update it
            bGetClassDefinition = false;
        }
    }

    if (bGetClassDefinition)
    {
        // Retrieve FdoClassDefinition
        FdoPtr<FdoClassDefinition> fdoClassDefinition = m_fdoReader->GetClassDefinition();

        // Convert FdoClassDefinition to MgClassDefinition
        m_classDef = MgServerFeatureUtil::GetMgClassDefinition(fdoClassDefinition, true);
        CHECKNULL(m_classDef.p, L"MgServerGetFeatures.GetFeatures");
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureReader.GetClassDefinition")

    return SAFE_ADDREF((MgClassDefinition*)m_classDef);
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
/// NOTE: This is internal API used by the mapping service
/// in the case where we do not want to spend time
/// serializing the class definition to XML
MgClassDefinition* MgServerFeatureReader::GetClassDefinitionNoXml()
{
    CHECKNULL(m_fdoReader, L"MgServerFeatureReader.GetClassDefinitionNoXml");

    Ptr<MgClassDefinition> classDef;

    MG_FEATURE_SERVICE_TRY()

    if (NULL == (MgClassDefinition*)m_classDef)
    {
        // Retrieve FdoClassDefinition
        FdoPtr<FdoClassDefinition> fdoClassDefinition = m_fdoReader->GetClassDefinition();

        // Convert FdoClassDefinition to MgClassDefinition
        m_classDef = MgServerFeatureUtil::GetMgClassDefinition(fdoClassDefinition, false);
        CHECKNULL(m_classDef.p, L"MgServerGetFeatures.GetFeatures");
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureReader.GetClassDefinitionNoXml")

    return SAFE_ADDREF((MgClassDefinition*)m_classDef);
}


//////////////////////////////////////////////////////////////////
/// <summary>
///  Returns true if the value of the specified property is null.
/// </summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns true if the value is null.</returns>
bool MgServerFeatureReader::IsNull(CREFSTRING propertyName)
{
    CHECKNULL(m_fdoReader, L"MgServerFeatureReader.IsNull");

    bool retVal = false;

    MG_FEATURE_SERVICE_TRY()

    retVal = m_fdoReader->IsNull(propertyName.c_str());

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureReader.IsNull");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Boolean value of the specified property. No conversion is
/// performed, thus the property must be a of boolean type the result
/// is undertermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the Boolean value.</returns>
/// EXCEPTIONS:
/// InvalidPropertyType
bool MgServerFeatureReader::GetBoolean(CREFSTRING propertyName)
{
    CHECKNULL(m_fdoReader, L"MgServerFeatureReader.GetBoolean");

    bool retVal = false;

    MG_FEATURE_SERVICE_TRY()

    if(m_fdoReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetBoolean",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = m_fdoReader->GetBoolean(propertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureReader.GetBoolean");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Byte value of the specified property. No conversion is
/// performed, thus the property must be a of byte type or the result
/// is undertermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the Byte value.</returns>
/// EXCEPTIONS:
/// InvalidPropertyType
BYTE MgServerFeatureReader::GetByte(CREFSTRING propertyName)
{
    CHECKNULL(m_fdoReader, L"MgServerFeatureReader.GetByte");

    BYTE retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    if(m_fdoReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetByte",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (BYTE)m_fdoReader->GetByte(propertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureReader.GetByte");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the DTime value of the specified property. No conversion is
/// performed, thus the property must be a of date type or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the DTime value.</returns>
/// EXCEPTIONS:
/// InvalidPropertyType
MgDateTime* MgServerFeatureReader::GetDateTime(CREFSTRING propertyName)
{
    Ptr<MgDateTime> retVal;

    CHECKNULL(m_fdoReader, L"MgServerFeatureReader.GetDateTime");

    MG_FEATURE_SERVICE_TRY()

    if(m_fdoReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetDateTime",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        FdoDateTime val = m_fdoReader->GetDateTime(propertyName.c_str());
        retVal = new MgDateTime((INT16)val.year, (INT8)val.month, (INT8)val.day,
                                (INT8)val.hour, (INT8)val.minute, val.seconds);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureReader.GetDateTime");

    return retVal.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Single value of the specified property. No conversion is
/// performed, thus the property must be a of type single or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the single value.</returns>
/// EXCEPTIONS:
/// InvalidPropertyType
float MgServerFeatureReader::GetSingle(CREFSTRING propertyName)
{
    CHECKNULL(m_fdoReader, L"MgServerFeatureReader.GetSingle");

    float retVal = 0.0f;

    MG_FEATURE_SERVICE_TRY()

    if(m_fdoReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetSingle",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = m_fdoReader->GetSingle(propertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureReader.GetSingle");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Double value of the specified property. No conversion is
/// performed, thus the property must be a of type double or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the double value.</returns>
/// EXCEPTIONS:
/// InvalidPropertyType
double MgServerFeatureReader::GetDouble(CREFSTRING propertyName)
{
    CHECKNULL(m_fdoReader, L"MgServerFeatureReader.GetDouble");

    double retVal = 0.0;

    MG_FEATURE_SERVICE_TRY()

    if(m_fdoReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetDouble",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = m_fdoReader->GetDouble(propertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureReader.GetDouble");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 16 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 16 bits or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the integer 16 bits value.</returns>
/// EXCEPTIONS:
/// InvalidPropertyType
INT16 MgServerFeatureReader::GetInt16(CREFSTRING propertyName)
{
    CHECKNULL(m_fdoReader, L"MgServerFeatureReader.GetInt16");

    INT16 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    if(m_fdoReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetInt16",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (INT16)m_fdoReader->GetInt16(propertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureReader.GetInt16");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 32 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 32 bits or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the integer 32 bits value.</returns>
/// EXCEPTIONS:
/// InvalidPropertyType
INT32 MgServerFeatureReader::GetInt32(CREFSTRING propertyName)
{
    CHECKNULL(m_fdoReader, L"MgServerFeatureReader.GetInt32");

    INT32 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    if(m_fdoReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetInt32",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (INT32)m_fdoReader->GetInt32(propertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureReader.GetInt32");

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
/// EXCEPTIONS:
/// InvalidPropertyType
INT64 MgServerFeatureReader::GetInt64(CREFSTRING propertyName)
{
    CHECKNULL(m_fdoReader, L"MgServerFeatureReader.GetInt64");

    INT64 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    if(m_fdoReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetInt64",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = (INT64)m_fdoReader->GetInt64(propertyName.c_str());
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureReader.GetInt64");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the string value of the specified property. No conversion is
/// performed, thus the property must be a of type string or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the string value.</returns>
/// EXCEPTIONS:
/// InvalidPropertyType
STRING MgServerFeatureReader::GetString(CREFSTRING propertyName)
{
    CHECKNULL(m_fdoReader, L"MgServerFeatureReader.GetString");

    STRING retVal = L"";

    MG_FEATURE_SERVICE_TRY()

    if(m_fdoReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetString",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        INT32 length = 0;
        const wchar_t* str = this->GetString(propertyName.c_str(), length);
        if (str != NULL)
        {
            retVal = str;
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureReader.GetString");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the BLOB value of the specified property. No conversion is
/// performed, thus the property must be a of type BLOBs or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the BLOB value.</returns>
/// EXCEPTIONS:
/// InvalidPropertyType
MgByteReader* MgServerFeatureReader::GetBLOB(CREFSTRING propertyName)
{
    CHECKNULL(m_fdoReader, L"MgServerFeatureReader.GetBLOB");

    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    if(m_fdoReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetBLOB",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        FdoPtr<FdoLOBValue> fdoVal = m_fdoReader->GetLOB(propertyName.c_str());
        if (fdoVal != NULL)
        {
            FdoPtr<FdoByteArray> byteArray = fdoVal->GetData();
            if (byteArray != NULL)
            {
                FdoByte* bytes = byteArray->GetData();
                FdoInt32 len = byteArray->GetCount();
                Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)bytes,(INT32)len);
                byteReader = byteSource->GetReader();
            }
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureReader.GetBLOB");

    return byteReader.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the CLOB value of the specified property. No conversion is
/// performed, thus the property must be a of type CLOB or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the CLOB value.</returns>
/// EXCEPTIONS:
/// InvalidPropertyType
MgByteReader* MgServerFeatureReader::GetCLOB(CREFSTRING propertyName)
{
    CHECKNULL(m_fdoReader, L"MgServerFeatureReader.GetCLOB");

    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    if(m_fdoReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetCLOB",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        FdoPtr<FdoLOBValue> fdoVal = m_fdoReader->GetLOB(propertyName.c_str());
        if (fdoVal != NULL)
        {
            FdoPtr<FdoByteArray> byteArray = fdoVal->GetData();
            if (byteArray != NULL)
            {
                FdoByte* bytes = byteArray->GetData();
                FdoInt32 len = byteArray->GetCount();
                Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)bytes,(INT32)len);
                byteReader = byteSource->GetReader();
            }
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureReader.GetCLOB");

    return byteReader.Detach();
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
MgFeatureReader* MgServerFeatureReader::GetFeatureObject(CREFSTRING propertyName)
{
    // TODO: Figure out how to support object properties.
    CHECKNULL(m_fdoReader, L"MgServerFeatureReader.GetFeatureObject");

    Ptr<MgServerFeatureReader> featureReader;

    MG_FEATURE_SERVICE_TRY()

    if(m_fdoReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFdoFeatureReader.GetFeatureObject",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        FdoPtr<FdoIFeatureReader> featureObjectReader = m_fdoReader->GetFeatureObject(propertyName.c_str());

        if (featureObjectReader != NULL)
        {
            // Create a feature reader identifier
            featureReader = new MgServerFeatureReader(m_connection, featureObjectReader);
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureReader.GetFeatureObject");

    return featureReader.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the geometry value of the specified property as a GeometryReader.
/// Because no conversion is performed, the property must be
/// of Geometric type{} otherwise, the result is NULL.</summary>
/// <param name="propertyName">Input the property name.</param>
/// <returns>Returns the Geometry object.</returns>
/// EXCEPTIONS:
/// InvalidPropertyType
MgByteReader* MgServerFeatureReader::GetGeometry(CREFSTRING propertyName)
{
    CHECKNULL(m_fdoReader, L"MgServerFeatureReader.GetGeometry");

    Ptr<MgByteReader> retVal;

    MG_FEATURE_SERVICE_TRY()

    if(m_fdoReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFeatureReader.GetGeometry",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        FdoInt32 len = 0;
        const FdoByte* data = m_fdoReader->GetGeometry(propertyName.c_str(), &len);

        if (data != NULL)
        {
            Ptr<MgByte> mgBytes = new MgByte((BYTE_ARRAY_IN)data, len);
            Ptr<MgByteSource> bSource = new MgByteSource(mgBytes);
            bSource->SetMimeType(MgMimeType::Agf);
            retVal = bSource->GetReader();
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureReader.GetGeometry");

    return retVal.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the geometry value of the specified property as a GeometryReader.
/// Because no conversion is performed, the property must be
/// of Geometric type{} otherwise, the result is NULL.</summary>
/// <param name="propertyName">Input the property name.</param>
/// <returns>Returns the Geometry object.</returns>
/// EXCEPTIONS:
/// InvalidPropertyType
MgRaster* MgServerFeatureReader::GetRaster(CREFSTRING propertyName)
{
    CHECKNULL(m_fdoReader, L"MgServerFeatureReader.GetRaster");

    Ptr<MgRaster> retVal;

    MG_FEATURE_SERVICE_TRY()

    // TODO: The IsNull() check is returning true for WFS/WMS FDO providers when there is valid data.
    //       In this case it should really be returning false so that the data can be retrieved.
    //       For now the check will be commented out until this can be resolved in FDO.
    if(false) // TODO: Force the exception throwing code to be skipped until issue is resolved.
//    if(m_fdoReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFeatureReader.GetRaster",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        FdoPtr<FdoIRaster> raster = m_fdoReader->GetRaster(propertyName.c_str());
        CHECKNULL((FdoIRaster*)raster, L"MgServerFeatureReader.GetRaster");

        retVal = MgServerFeatureUtil::GetMgRaster(raster, propertyName);
        CHECKNULL((MgRaster*)retVal, L"MgServerFeatureReader.GetRaster");

        // Set the service to MgRaster for GetRaster operation called in GetStream method
        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
        assert(NULL != serviceMan);

        // Get the service from service manager
        Ptr<MgFeatureService> featureService = dynamic_cast<MgFeatureService*>(
            serviceMan->RequestService(MgServiceType::FeatureService));
        assert(featureService != NULL);

        retVal->SetMgService(featureService);

        // Collect the feature reader into a pool for GetRaster operation
        MgServerFeatureReaderPool* featPool = MgServerFeatureReaderPool::GetInstance();
        CHECKNULL(featPool, L"MgServerFeatureReader.GetRaster");

        STRING featureReader = featPool->GetReaderId(this);
        if (L"" == featureReader)
        {
            featureReader = featPool->Add(this); // Add the reference
            m_removeFromPoolOnDestruction = true;
        }

        // Set the handle
        retVal->SetHandle(featureReader);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureReader.GetRaster");

    return retVal.Detach();
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Serialize data to TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgServerFeatureReader::Serialize(MgStream* stream)
{
    INT32 count = 1; // Get value from MgConfiguration
    bool operationCompleted = false;
    Ptr<MgFeatureSet> featureSet;
    STRING featureReader = L"";

    MG_FEATURE_SERVICE_TRY()

    // Find out the counts from Configuration
    MgConfiguration* config = MgConfiguration::GetInstance();

    config->GetIntValue(MgConfigProperties::FeatureServicePropertiesSection,
                        MgConfigProperties::FeatureServicePropertyDataCacheSize,
                        count,
                        MgConfigProperties::DefaultFeatureServicePropertyDataCacheSize);

    // Collect the feature reader into a pool for ReadNext operation
    MgServerFeatureReaderPool* featPool = MgServerFeatureReaderPool::GetInstance();
    CHECKNULL(featPool, L"MgServerFeatureReader.Serialize");

    featureReader = featPool->GetReaderId(this);
    if (L"" == featureReader)
    {
        // The feature reader is not in the pool
        featureReader = featPool->Add(this); // Add the reference
    }

    featureSet = GetFeatures(count);

    operationCompleted = true;

    MG_FEATURE_SERVICE_CATCH(L"MgServerFeatureReader.Serialize");

    // Mark operation is completed successfully
    stream->WriteBoolean(operationCompleted);

    if (operationCompleted && (mgException == 0))
    {
        stream->WriteString(featureReader); // Write the reader ID so we can retrieve it for later use
        stream->WriteObject((MgFeatureSet*)featureSet); // Write the feature set
    }
    else
    {
        stream->WriteObject((MgException*)mgException);
    }

    // Serialize method must not THROW the exception because it has already been written to the stream
    // and in doing so would only end up writing the same exception to the stream twice.
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserialize data from TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgServerFeatureReader::Deserialize(MgStream* stream)
{
    throw new MgInvalidOperationException(L"MgServerFeatureReader.Deserialize",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Serializes all features into an XML.
/// XML is serialized from the current position of feature reader in the order
/// data are retrieved.
/// <returns>MgByteReader holding XML.</returns>
MgByteReader* MgServerFeatureReader::ToXml()
{
    throw new MgInvalidOperationException(L"MgServerFeatureReader.ToXml",
        __LINE__, __WFILE__, NULL, L"", NULL);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Releases all the resources of feature reader.
/// This must be called when user is done with Feature Reader
/// <returns>Nothing</returns>
void MgServerFeatureReader::Close()
{
    CHECKNULL(m_fdoReader, L"MgServerFeatureReader.Close");

    MG_FEATURE_SERVICE_TRY()

    // If m_getFeatures was added to pool by the local service
    // this flag will be set to true. In this case we need to
    // remove this from pool on ServerFeatureReader close operation
    if (m_removeFromPoolOnDestruction)
    {
        MgServerFeatureReaderPool* featPool = MgServerFeatureReaderPool::GetInstance();
        if(NULL != featPool)
        {
            STRING featureReader = featPool->GetReaderId(this);
            if (L"" != featureReader)
                featPool->Remove(featureReader);
        }
    }

    m_fdoReader->Close();

    FDO_SAFE_RELEASE(m_fdoReader);

    // Get the FDO connection
    FdoPtr<FdoIConnection> fdoConnection = m_connection->GetConnection();

    // Release the connection.
    m_connection = NULL;

    // Let the FDO Connection Manager know that we are no longer using a FDO provider connection.
    MgFdoConnectionManager* fdoConnectionManager = MgFdoConnectionManager::GetInstance();
    ACE_ASSERT(NULL != fdoConnectionManager);

    if (NULL != fdoConnectionManager)
    {
        fdoConnectionManager->Close(fdoConnection);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureReader.Close");
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Geometry for the specified property. No conversion is
/// performed, thus the property must be a of type Geometry or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns a ByteReader object</returns>
BYTE_ARRAY_OUT MgServerFeatureReader::GetGeometry(CREFSTRING propertyName, INT32& length)
{
    CHECKNULL(m_fdoReader, L"MgServerFeatureReader.GetGeometry");

    const FdoByte* data = NULL;

    MG_FEATURE_SERVICE_TRY()

    if(m_fdoReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFeatureReader.GetGeometry",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        FdoInt32 len = 0;
        data = m_fdoReader->GetGeometry(propertyName.c_str(), &len);
        length = len;
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureReader.GetGeometry");

    return (BYTE_ARRAY_OUT)data;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the string value of the specified property. No conversion is
/// performed, thus the property must be a of type string or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the string value.</returns>
const wchar_t* MgServerFeatureReader::GetString(CREFSTRING propertyName, INT32& length)
{
    CHECKNULL(m_fdoReader, L"MgServerFeatureReader.GetString");

    FdoString* retVal = NULL;

    MG_FEATURE_SERVICE_TRY()

    if(m_fdoReader->IsNull(propertyName.c_str()))
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgServerFeatureReader.GetString",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
    else
    {
        retVal = m_fdoReader->GetString(propertyName.c_str());
        if (retVal != NULL)
        {
            length = (INT32)wcslen((const wchar_t*)retVal);
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureReader.GetString");

    return ((const wchar_t*)retVal);
}

MgFeatureSet* MgServerFeatureReader::GetFeatures(INT32 count)
{
    CHECKNULL((FdoIFeatureReader*)m_fdoReader, L"MgServerFeatureReader.GetFeatures");

    MG_FEATURE_SERVICE_TRY()

    INT32 featCount = count;

    if (NULL == (MgClassDefinition*)m_classDef)
    {
        // Get MgClassDefinition
        m_classDef = GetClassDefinition();
        CHECKNULL(m_classDef.p, L"MgServerFeatureReader.GetFeatures");
    }

    if (NULL == (MgFeatureSet*)m_featureSet)
    {
        // Create a feature set for a pool of features
        m_featureSet = new MgFeatureSet();
        CHECKNULL((MgFeatureSet*)m_featureSet, L"MgServerFeatureReader.GetFeatures");

        // Assign feature class definition to Feature Set
        m_featureSet->SetClassDefinition(m_classDef);
    }
    else
    {
        m_featureSet->ClearFeatures();
    }

    // If class definition contains raster property
    // we can only retrieve one feature at a time
    // because, user would need to supply x,y and extents before
    // they could fetch the raster data. Therefore we can not
    // advance FdoIReader.
    if (m_classDef->HasRasterProperty())
    {
        featCount = 1;
    }

    // Add all features to feature set
    AddFeatures(featCount);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureReader.GetFeatures")

    return SAFE_ADDREF((MgFeatureSet*)m_featureSet);
}

void MgServerFeatureReader::AddFeatures(INT32 count)
{
    CHECKNULL((FdoIFeatureReader*)m_fdoReader, L"MgServerFeatureReader.AddFeatures");
    CHECKNULL((MgFeatureSet*)m_featureSet, L"MgServerFeatureReader.AddFeatures");

    INT32 desiredFeatures = 0;

    // Access the class definition
    Ptr<MgClassDefinition> classDef = m_featureSet->GetClassDefinition();
    CHECKNULL((MgClassDefinition*)classDef, L"MgServerFeatureReader.AddFeatures");

    // Access the property definition collection
    // Ptr<MgPropertyDefinitionCollection> propDefCol = classDef->GetProperties();
    Ptr<MgPropertyDefinitionCollection> propDefCol = classDef->GetPropertiesIncludingBase();
    CHECKNULL((MgPropertyDefinitionCollection*)propDefCol, L"MgServerFeatureReader.AddFeatures");

    // How many properties are we fetching, it should be atleast one
    INT32 cnt = propDefCol->GetCount();

    // We only read if there is atleast one property requested
    if (cnt > 0 && count > 0)
    {
        try
        {
            while (m_fdoReader->ReadNext())
            {
                AddFeature((MgPropertyDefinitionCollection*)propDefCol);

                // Collected required features therefore do not process more
                if (++desiredFeatures == count)
                    break;
            }
        }
        //some providers will throw if ReadNext is called more than once
        catch (FdoException* e)
        {
            // Note: VB 05/10/06 The assert has been commented out as
            // Linux does not remove them from a release build. The assert
            // will cause the server to crash on Linux. The Oracle provider will throw
            // an exception if the ReadNext() method is called after it returns false.
            // This is a known problem and it is safe to ignore the exception.
            //assert(false);
            e->Release();
        }
        catch(...)
        {
        }
    }
}

void MgServerFeatureReader::AddFeature(MgPropertyDefinitionCollection* propDefCol)
{
    CHECKNULL((FdoIFeatureReader*)m_fdoReader, L"MgServerFeatureReader.AddFeature");
    CHECKNULL((MgPropertyDefinitionCollection*)propDefCol, L"MgServerFeatureReader.AddFeature");

    //intentionally turn off duplicate checking for better performance
    Ptr<MgPropertyCollection> propCol = new MgPropertyCollection(true, true);
    INT32 cnt = propDefCol->GetCount();

    for (INT32 i=0; i < cnt; i++)
    {
        // Access the property definition
        Ptr<MgPropertyDefinition> propDef = propDefCol->GetItem(i);

        // Get the name of property
        STRING propName = propDef->GetName();

        INT16 type = MgServerFeatureUtil::GetMgPropertyType(propDef);
        Ptr<MgProperty> prop = MgServerFeatureUtil::GetMgProperty(this, propName, type);
        if (prop != NULL)
        {
            propCol->Add(prop);
        }
    }

    m_featureSet->AddFeature(propCol);
}

MgByteReader* MgServerFeatureReader::GetRaster(STRING rasterPropName, INT32 xSize, INT32 ySize)
{
    CHECKNULL(m_fdoReader, L"MgServerFeatureReader.GetRaster");
    CHECKNULL(m_classDef, L"MgServerFeatureReader.GetRaster");

    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    // If there is no raster property, GetRaster should not be called
    if (!m_classDef->HasRasterProperty())
    {
        // TODO: specify which argument and message, once we have the mechanism
        STRING message = MgServerFeatureUtil::GetMessage(L"MgMissingRasterProperty");
        throw new MgInvalidOperationException(L"MgServerFeatureReader.GetRaster", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    // There can be more than one Raster property
    if (rasterPropName.empty())
    {
        rasterPropName = m_classDef->GetRasterPropertyName();
    }

    // If this property is requested then we fetch the raster data
    byteReader = MgServerFeatureUtil::GetRaster(m_fdoReader, rasterPropName, xSize, ySize);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerFeatureReader.GetRaster")

    return byteReader.Detach();
}

// getter by index
BYTE_ARRAY_OUT MgServerFeatureReader::GetGeometry(INT32 index, INT32& length)
{
    STRING propertyName = GetPropertyName(index);
    return GetGeometry(propertyName, length);
}

MgByteReader* MgServerFeatureReader::GetGeometry(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetGeometry(propertyName);
}

MgFeatureReader*  MgServerFeatureReader::GetFeatureObject(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetFeatureObject(propertyName);
}

bool MgServerFeatureReader::GetBoolean(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetBoolean(propertyName);
}

BYTE MgServerFeatureReader::GetByte(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetByte(propertyName);
}

MgDateTime* MgServerFeatureReader::GetDateTime(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetDateTime(propertyName);
}

double MgServerFeatureReader::GetDouble(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetDouble(propertyName);
}

INT16 MgServerFeatureReader::GetInt16(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetInt16(propertyName);
}

INT32 MgServerFeatureReader::GetInt32(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetInt32(propertyName);
}

INT64 MgServerFeatureReader::GetInt64(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetInt64(propertyName);
}

float MgServerFeatureReader::GetSingle(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetSingle(propertyName);
}

STRING MgServerFeatureReader::GetString(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetString(propertyName);
}

MgByteReader* MgServerFeatureReader::GetBLOB(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetBLOB(propertyName);
}

MgByteReader* MgServerFeatureReader::GetCLOB(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetCLOB(propertyName);
}

bool MgServerFeatureReader::IsNull(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return IsNull(propertyName);
}

 MgRaster* MgServerFeatureReader::GetRaster(INT32 index)
{
    STRING propertyName = GetPropertyName(index);
    return GetRaster(propertyName);
}
