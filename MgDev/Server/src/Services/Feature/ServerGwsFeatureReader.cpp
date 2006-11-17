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
#include "ServerGwsFeatureReader.h"
#include "ServerFeatureUtil.h"

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct a MgServerGwsFeatureReader object from a byte source
///</summary>
///<param name="byteSource">Byte  source object</param>
///
MgServerGwsFeatureReader::MgServerGwsFeatureReader(IGWSFeatureIterator* gwsFeatureIterator)
{
    MG_FEATURE_SERVICE_TRY()

    m_gwsFeatureIterator = FDO_SAFE_ADDREF(gwsFeatureIterator);
    m_gwsGetFeatures = new MgServerGwsGetFeatures(gwsFeatureIterator);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.MgServerGwsFeatureReader")
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct an uninitialized MgServerGwsFeatureReader object
///</summary>
///
MgServerGwsFeatureReader::MgServerGwsFeatureReader()
{
    m_gwsFeatureIterator = NULL;
    m_gwsGetFeatures = NULL;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Destruct a MgServerGwsFeatureReader object
///</summary>
///
MgServerGwsFeatureReader::~MgServerGwsFeatureReader()
{
    Close();
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
bool MgServerGwsFeatureReader::ReadNext()
{
    CHECKNULL(m_gwsFeatureIterator, L"MgServerGwsFeatureReader.ReadNext");

    bool retVal = false;

    MG_FEATURE_SERVICE_TRY()

    // advance the primary feature source iterator
    retVal = m_gwsFeatureIterator->ReadNext();

    if (retVal)
    {
        // retrieve the secondary feature source iterators, advance the iterators and store into a collection
        m_secondaryGwsFeatureIteratorMap.clear();
        FdoPtr<IGWSExtendedFeatureDescription> desc;
        m_gwsFeatureIterator->DescribeFeature(&desc);

        for (int i = 0; i < desc->GetCount(); i++)
        {
            FdoPtr<IGWSFeatureIterator> featureIter = m_gwsFeatureIterator->GetJoinedFeatures(i);
            CHECKNULL(featureIter, L"MgServerGwsFeatureReader.ReadNext");
            if (featureIter->ReadNext())
            {
                m_secondaryGwsFeatureIteratorMap.insert(GwsFeatureIteratorPair(L"secondaryFeatures", featureIter));
            }
        }
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.ReadNext")

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
MgClassDefinition* MgServerGwsFeatureReader::GetClassDefinition()
{
    CHECKNULL(m_gwsGetFeatures, L"MgServerGwsFeatureReader.GetClassDefinition");

    Ptr<MgClassDefinition> classDef;

    MG_FEATURE_SERVICE_TRY()

    Ptr<MgServerGwsGetFeatures> gwsGetFeatures = new MgServerGwsGetFeatures(m_gwsFeatureIteratorCopy);
    gwsGetFeatures->SetRelationNames(FdoPtr<FdoStringCollection>(m_gwsGetFeatures->GetRelationNames()));
    gwsGetFeatures->SetExtensionName(m_gwsGetFeatures->GetExtensionName());
    classDef = gwsGetFeatures->GetMgClassDefinition(false);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetClassDefinition")

    return classDef.Detach();
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
MgClassDefinition* MgServerGwsFeatureReader::GetClassDefinitionNoXml()
{
    CHECKNULL(m_gwsGetFeatures, L"MgServerGwsFeatureReader.GetClassDefinitionNoXml");

    Ptr<MgClassDefinition> classDef;

    MG_FEATURE_SERVICE_TRY()

    Ptr<MgServerGwsGetFeatures> gwsGetFeatures = new MgServerGwsGetFeatures(m_gwsFeatureIteratorCopy);
    gwsGetFeatures->SetRelationNames(FdoPtr<FdoStringCollection>(m_gwsGetFeatures->GetRelationNames()));
    gwsGetFeatures->SetExtensionName(m_gwsGetFeatures->GetExtensionName());
    classDef = gwsGetFeatures->GetMgClassDefinition(false);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetClassDefinitionNoXml")

    return classDef.Detach();
}


//////////////////////////////////////////////////////////////////
/// <summary>
///  Returns true if the value of the specified property is null.
/// </summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns true if the value is null.</returns>
bool MgServerGwsFeatureReader::IsNull(CREFSTRING propertyName)
{
    bool retVal = false;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.IsNull");

    retVal = gwsFeatureIter->IsNull(parsedPropertyName.c_str());

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.IsNull");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Boolean value of the specified property. No conversion is
/// performed, thus the property must be a of boolean type the result
/// is undertermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the Boolean value.</returns>
bool MgServerGwsFeatureReader::GetBoolean(CREFSTRING propertyName)
{
    bool retVal = false;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retreive the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetBoolean");

    retVal = gwsFeatureIter->GetBoolean(parsedPropertyName.c_str());

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetBoolean");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Byte value of the specified property. No conversion is
/// performed, thus the property must be a of byte type or the result
/// is undertermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the Byte value.</returns>
BYTE MgServerGwsFeatureReader::GetByte(CREFSTRING propertyName)
{
    BYTE retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetByte");

    retVal = (BYTE)gwsFeatureIter->GetByte(parsedPropertyName.c_str());

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetByte");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the DTime value of the specified property. No conversion is
/// performed, thus the property must be a of date type or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the DTime value.</returns>
MgDateTime* MgServerGwsFeatureReader::GetDateTime(CREFSTRING propertyName)
{
    Ptr<MgDateTime> retVal;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetDateTime");

    FdoDateTime val = gwsFeatureIter->GetDateTime(parsedPropertyName.c_str());
    retVal = new MgDateTime((INT16)val.year, (INT8)val.month, (INT8)val.day,
                            (INT8)val.hour, (INT8)val.minute, val.seconds);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetDateTime");

    return retVal.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Single value of the specified property. No conversion is
/// performed, thus the property must be a of type single or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the single value.</returns>
float MgServerGwsFeatureReader::GetSingle(CREFSTRING propertyName)
{
    float retVal = 0.0f;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetSingle");

    retVal = gwsFeatureIter->GetSingle(parsedPropertyName.c_str());

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetSingle");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Double value of the specified property. No conversion is
/// performed, thus the property must be a of type double or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the double value.</returns>
double MgServerGwsFeatureReader::GetDouble(CREFSTRING propertyName)
{
    double retVal = 0.0;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetDouble");

    retVal = gwsFeatureIter->GetDouble(parsedPropertyName.c_str());

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetDouble");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 16 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 16 bits or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the integer 16 bits value.</returns>
INT16 MgServerGwsFeatureReader::GetInt16(CREFSTRING propertyName)
{
    INT16 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetInt16");

    retVal = (INT16)gwsFeatureIter->GetInt16(parsedPropertyName.c_str());

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetInt16");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the integer 32 bits value of the specified property. No conversion is
/// performed, thus the property must be a of type integer 32 bits or the result
/// is undetermined</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the integer 32 bits value.</returns>
INT32 MgServerGwsFeatureReader::GetInt32(CREFSTRING propertyName)
{
    INT32 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetInt32");

    retVal = (INT32)gwsFeatureIter->GetInt32(parsedPropertyName.c_str());

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetInt32");

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
INT64 MgServerGwsFeatureReader::GetInt64(CREFSTRING propertyName)
{
    INT64 retVal = 0;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetInt64");

    retVal = (INT64)gwsFeatureIter->GetInt64(parsedPropertyName.c_str());

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetInt64");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the string value of the specified property. No conversion is
/// performed, thus the property must be a of type string or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the string value.</returns>
STRING MgServerGwsFeatureReader::GetString(CREFSTRING propertyName)
{
    STRING retVal = L"";

    MG_FEATURE_SERVICE_TRY()

    INT32 length = 0;
    const wchar_t* str = this->GetString(propertyName.c_str(), length);
    if (str != NULL)
    {
        retVal = str;
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetString");

    return retVal;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the BLOB value of the specified property. No conversion is
/// performed, thus the property must be a of type BLOBs or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the BLOB value.</returns>
MgByteReader* MgServerGwsFeatureReader::GetBLOB(CREFSTRING propertyName)
{
    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetBLOB");

    byteReader = m_gwsGetFeatures->GetLOBFromFdo(parsedPropertyName, gwsFeatureIter);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetBLOB");

    return byteReader.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the CLOB value of the specified property. No conversion is
/// performed, thus the property must be a of type CLOB or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the CLOB value.</returns>
MgByteReader* MgServerGwsFeatureReader::GetCLOB(CREFSTRING propertyName)
{
    Ptr<MgByteReader> byteReader;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetCLOB");

    byteReader = m_gwsGetFeatures->GetLOBFromFdo(parsedPropertyName, gwsFeatureIter);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetCLOB");

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
MgFeatureReader* MgServerGwsFeatureReader::GetFeatureObject(CREFSTRING propertyName)
{
    // TODO: Figure out how to support object properties.
    Ptr<MgServerFeatureReader> featureReader;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetFeatureObject");

    FdoPtr<FdoIFeatureReader> featureObjectReader = gwsFeatureIter->GetFeatureObject(parsedPropertyName.c_str());

    if (featureObjectReader != NULL)
    {
        // Create a feature reader identifier
        Ptr<MgServerFeatureReaderIdentifier> featReaderId = new MgServerFeatureReaderIdentifier(featureObjectReader);
        featureReader = new MgServerFeatureReader(featReaderId);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetFeatureObject");

    return featureReader.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Geometry for the specified property. No conversion is
/// performed, thus the property must be a of type Geometry or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns a ByteReader object</returns>
MgByteReader* MgServerGwsFeatureReader::GetGeometry(CREFSTRING propertyName)
{
    Ptr<MgByteReader> retVal;

    MG_FEATURE_SERVICE_TRY()

    INT32 len;
    BYTE_ARRAY_OUT data = this->GetGeometry(propertyName.c_str(), len);

    if (data != NULL)
    {
        Ptr<MgByte> mgBytes = new MgByte((BYTE_ARRAY_IN)data, len);
        Ptr<MgByteSource> bSource = new MgByteSource(mgBytes);
        retVal = bSource->GetReader();
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetGeometry");

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
MgRaster* MgServerGwsFeatureReader::GetRaster(CREFSTRING propertyName)
{
    Ptr<MgRaster> retVal;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetRaster");

    FdoPtr<FdoIRaster> raster = gwsFeatureIter->GetRaster(parsedPropertyName.c_str());
    CHECKNULL((FdoIRaster*)raster, L"MgServerGwsFeatureReader.GetRaster");

    retVal = MgServerFeatureUtil::GetMgRaster(raster, parsedPropertyName);
    CHECKNULL((MgRaster*)retVal, L"MgServerGwsFeatureReader.GetRaster");

    // Set the service to MgRaster for GetRaster operation called in GetStream method
    MgServiceManager* serviceMan = MgServiceManager::GetInstance();
    assert(NULL != serviceMan);

    // Get the service from service manager
    Ptr<MgFeatureService> featureService = dynamic_cast<MgFeatureService*>(
        serviceMan->RequestService(MgServiceType::FeatureService));
    assert(featureService != NULL);

    retVal->SetMgService(featureService);
    retVal->SetHandle((INT32)m_gwsGetFeatures.p);

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.GetRaster");

    return retVal.Detach();
}

void MgServerGwsFeatureReader::Serialize(MgStream* stream)
{
    INT32 count = 1; // Get value from MgConfiguration
    bool operationCompleted = false;
    Ptr<MgFeatureSet> featureSet;

    MG_FEATURE_SERVICE_TRY()

    // Find out the counts from Configuration
    MgConfiguration* config = MgConfiguration::GetInstance();

    config->GetIntValue(MgConfigProperties::FeatureServicePropertiesSection,
                        MgConfigProperties::FeatureServicePropertyDataCacheSize,
                        count,
                        MgConfigProperties::DefaultFeatureServicePropertyDataCacheSize);

    featureSet = m_gwsGetFeatures->GetFeatures(count);

    operationCompleted = true;

    MG_FEATURE_SERVICE_CATCH(L"MgServerGwsFeatureReader.Serialize");

    // Mark operation is completed successfully
    stream->WriteBoolean(operationCompleted);

    if (operationCompleted && (mgException == 0))
    {
        stream->WriteInt32((INT32)m_gwsGetFeatures.p);  // Write the pointer value so we can retrieve it for later use
        stream->WriteObject((MgFeatureSet*)featureSet); // Write the feature set
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

void MgServerGwsFeatureReader::Deserialize(MgStream* stream)
{
    throw new MgInvalidOperationException(L"MgServerGwsFeatureReader.Deserialize",
        __LINE__, __WFILE__, NULL, L"", NULL);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Serializes all features into an XML.
/// XML is serialized from the current position of feature reader in the order
/// data are retrieved.
/// <returns>MgByteReader holding XML.</returns>
MgByteReader* MgServerGwsFeatureReader::ToXml()
{
    throw new MgInvalidOperationException(L"MgServerGwsFeatureReader.ToXml",
        __LINE__, __WFILE__, NULL, L"", NULL);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Releases all the resources of feature reader.
/// This must be called when user is done with Feature Reader
/// <returns>Nothing</returns>
void MgServerGwsFeatureReader::Close()
{
    CHECKNULL(m_gwsFeatureIterator, L"MgServerGwsFeatureReader.Close");

    MG_FEATURE_SERVICE_TRY()

    if (!m_secondaryGwsFeatureIteratorMap.empty())
    {
        for (GwsFeatureIteratorMap::iterator iter = m_secondaryGwsFeatureIteratorMap.begin();
             iter != m_secondaryGwsFeatureIteratorMap.end();
             iter++)
        {
            IGWSFeatureIterator* pSecondaryFeatureIter = iter->second;
            if (pSecondaryFeatureIter)
            {
                pSecondaryFeatureIter->Close();
            }
        }
    }
    m_gwsFeatureIterator->Close();

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerGwsFeatureReader.Close");
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Geometry for the specified property. No conversion is
/// performed, thus the property must be a of type Geometry or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns a ByteReader object</returns>
BYTE_ARRAY_OUT MgServerGwsFeatureReader::GetGeometry(CREFSTRING propertyName, INT32& length)
{
    const FdoByte* data = NULL;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propertyName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(gwsFeatureIter, L"MgServerGwsFeatureReader.GetGeometry");

    if(!gwsFeatureIter->IsNull(parsedPropertyName.c_str()))
    {
        FdoInt32 len = 0;
        data = gwsFeatureIter->GetGeometry(parsedPropertyName.c_str(), &len);
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
const wchar_t* MgServerGwsFeatureReader::GetString(CREFSTRING propName, INT32& length)
{
    FdoString* retVal;

    MG_FEATURE_SERVICE_TRY()

    // Determine which feature source to retrieve the property from
    IGWSFeatureIterator* gwsFeatureIter = NULL;
    STRING parsedPropertyName;
    DeterminePropertyFeatureSource(propName, &gwsFeatureIter, parsedPropertyName);
    CHECKNULL(m_gwsFeatureIterator, L"MgServerGwsFeatureReader.GetString");

    retVal = gwsFeatureIter->GetString(parsedPropertyName.c_str());
    if (retVal != NULL)
    {
        length = (INT32)wcslen((const wchar_t*)retVal);
    }

    MG_FEATURE_SERVICE_CATCH_AND_THROW(L"MgServerDataReader.GetString");

    return ((const wchar_t*)retVal);
}

void MgServerGwsFeatureReader::DeterminePropertyFeatureSource(CREFSTRING inputPropName,
                                                              IGWSFeatureIterator** gwsFeatureIter,
                                                              STRING& relationName,
                                                              STRING& className,
                                                              STRING& parsedPropName)
{
    CHECKNULL(m_gwsFeatureIterator, L"MgServerGwsFeatureReader.DeterminePropertyFeatureSource");

    IGWSFeatureIterator* secondaryFeatureIter = NULL;

    // Parse the input property name to determine the feature source,
    // and the property name.  The qualified name could
    // look something like this: Join1PropA, where
    // Join1 = relation name
    // PropA = property name

    STRING qualifiedName;

    if (inputPropName.empty())
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(MgResources::BlankArgument);

        throw new MgInvalidArgumentException(L"MgServerGwsFeatureReader.DeterminePropertyFeatureSource",
            __LINE__, __WFILE__, &arguments, L"MgStringEmpty", NULL);
    }

    // Check if the input propName is prefixed with the relationName
    // by comparing with primary feature source property names
    FdoPtr<IGWSExtendedFeatureDescription> primaryDesc;
    m_gwsFeatureIterator->DescribeFeature(&primaryDesc);
    FdoPtr<FdoStringCollection> primaryPropNames = primaryDesc->PropertyNames();

    int primaryIndex = primaryPropNames->IndexOf(inputPropName.c_str());
    if( -1 != primaryIndex)
    {
        // No prefix, but the property name does exist in the primary feature source
        relationName.clear();
        FdoPtr<IGWSExtendedFeatureDescription> primaryDesc;
        m_gwsFeatureIterator->DescribeFeature(&primaryDesc);

        GWSQualifiedName primQualifiedClassName = primaryDesc->ClassName();

        FdoString* fclassName = primQualifiedClassName.Name();
        className = (wchar_t*)fclassName;

        parsedPropName = inputPropName;
        *gwsFeatureIter = m_gwsFeatureIterator;
    }
    else if (!m_secondaryGwsFeatureIteratorMap.empty())
    {
        // cycle thru secondary feature sources and check property names
        for (GwsFeatureIteratorMap::iterator iter = m_secondaryGwsFeatureIteratorMap.begin();
             iter != m_secondaryGwsFeatureIteratorMap.end();
             iter++)
        {
            secondaryFeatureIter = iter->second;
            CHECKNULL(secondaryFeatureIter, L"MgServerGwsFeatureReader.DeterminePropertyFeatureSource");

            FdoPtr<IGWSExtendedFeatureDescription> secondaryDesc;
            secondaryFeatureIter->DescribeFeature(&secondaryDesc);

            GWSQualifiedName secQualifiedClassName = secondaryDesc->ClassName();

            FdoString* featureSource = secQualifiedClassName.FeatureSource();
            FdoString* fclassName = secQualifiedClassName.Name();
            className = (wchar_t*)fclassName;

            FdoPtr<FdoStringCollection> secondaryPropNames = secondaryDesc->PropertyNames();

            // cycle thru secondaryPropNames looking for substring occurrence in inputPropName
            FdoInt32 secPropCnt = secondaryPropNames->GetCount();
            for (FdoInt32 secPropIndex = 0; secPropIndex < secPropCnt; secPropIndex++)
            {
                STRING secondaryProp = (STRING)secondaryPropNames->GetString(secPropIndex);
                STRING::size_type nPropStartIndex = inputPropName.find(secondaryProp);
                if (std::wstring::npos != nPropStartIndex)
                {
                    parsedPropName = inputPropName.substr(nPropStartIndex).c_str();
                    relationName = inputPropName.substr(0, nPropStartIndex).c_str();

                    if ( wcscmp(featureSource, relationName.c_str()) == 0 )
                    {
                        *gwsFeatureIter = secondaryFeatureIter;
                        break;
                    }
                }
            }
        }
    }

    if (NULL == *gwsFeatureIter)
    {
        MgStringCollection arguments;
        arguments.Add(inputPropName);

        throw new MgParameterNotFoundException(
            L"MgServerGwsFeatureReader.DeterminePropertyFeatureSource", __LINE__, __WFILE__, &arguments, L"", NULL);
    }
}


void MgServerGwsFeatureReader::PrepareGwsGetFeatures(CREFSTRING extensionName, FdoStringCollection* relationNames)
{
    m_gwsGetFeatures->SetExtensionName(extensionName);
    m_gwsGetFeatures->SetRelationNames(relationNames);
}

void MgServerGwsFeatureReader::DeterminePropertyFeatureSource(CREFSTRING inputPropName, IGWSFeatureIterator** gwsFeatureIter, STRING& parsedPropName)
{
    STRING relationName;
    STRING className;
    this->DeterminePropertyFeatureSource(inputPropName, gwsFeatureIter, relationName, className, parsedPropName);
}

void MgServerGwsFeatureReader::SetGwsIteratorCopy(IGWSFeatureIterator* iterCopy)
{
    m_gwsFeatureIteratorCopy = FDO_SAFE_ADDREF(iterCopy);
}
