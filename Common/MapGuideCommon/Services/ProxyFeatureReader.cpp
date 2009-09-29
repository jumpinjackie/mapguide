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

#define CHECK_FEATURESET_COUNT(pointer, methodname)        \
if (0 == pointer->GetCount())                           \
{                                                       \
    throw new MgEmptyFeatureSetException(methodname,    \
                                       __LINE__, __WFILE__, NULL, L"", NULL); \
}

#define CHECK_PROPERTY_TYPE(property, type, methodname) \
if (property != type)                                   \
{                                                       \
    throw new MgInvalidPropertyTypeException(methodname, \
                                       __LINE__, __WFILE__, NULL, L"", NULL); \
}

MG_IMPL_DYNCREATE(MgProxyFeatureReader);

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct a MgProxyFeatureReader object from a byte source
///</summary>
///<param name="byteSource">Byte  source object</param>
///
MgProxyFeatureReader::MgProxyFeatureReader(MgFeatureSet* featureSet)
{
    m_currRecord = 0;
    m_serverfeatReader = L"";
    m_set = SAFE_ADDREF((MgFeatureSet*)featureSet);
    m_service = NULL;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct an uninitialized MgProxyFeatureReader object
///</summary>
///
MgProxyFeatureReader::MgProxyFeatureReader()
{
    m_currRecord = 0;
    m_serverfeatReader = L"";
    m_set = NULL;
    m_service = NULL;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Destruct a MgProxyFeatureReader object
///</summary>
///
MgProxyFeatureReader::~MgProxyFeatureReader()
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
void MgProxyFeatureReader::AssignFeatureSet(MgFeatureSet* featureSet)
{
    m_currRecord = 0;
    m_set = SAFE_ADDREF((MgFeatureSet*)featureSet);
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgProxyFeatureReader::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////
///<summary>
/// Dispose this object.
///</summary>
void MgProxyFeatureReader::Dispose()
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
bool MgProxyFeatureReader::ReadNext()
{
    CHECKNULL(m_set, L"MgProxyFeatureReader.ReadNext");

    bool foundNextFeature = false;

    INT32 cnt = m_set->GetCount();

    if ( m_currRecord < cnt )
    {
        m_currRecord++;
        foundNextFeature = true;
    }
    else
    {
        // Fetch next set of records
        try
        {
            m_currRecord = 0;
            if (m_serverfeatReader != L"")
            {
                Ptr<MgBatchPropertyCollection> bpCol = m_service->GetFeatures(m_serverfeatReader);
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
MgClassDefinition* MgProxyFeatureReader::GetClassDefinition()
{
    CHECKNULL(m_set, L"MgProxyFeatureReader.GetClassDefinition");

    return m_set->GetClassDefinition();
}

MgClassDefinition* MgProxyFeatureReader::GetClassDefinitionNoXml()
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
bool MgProxyFeatureReader::IsNull(CREFSTRING propertyName)
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
bool MgProxyFeatureReader::IsNull(INT32 index)
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
/// EXCEPTIONS:
/// InvalidPropertyType
bool MgProxyFeatureReader::GetBoolean(CREFSTRING propertyName)
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
/// EXCEPTIONS:
/// InvalidPropertyType
bool MgProxyFeatureReader::GetBoolean(INT32 index)
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
/// EXCEPTIONS:
/// InvalidPropertyType
BYTE MgProxyFeatureReader::GetByte(CREFSTRING propertyName)
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
/// EXCEPTIONS:
/// InvalidPropertyType
BYTE MgProxyFeatureReader::GetByte(INT32 index)
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
/// EXCEPTIONS:
/// InvalidPropertyType
MgDateTime* MgProxyFeatureReader::GetDateTime(CREFSTRING propertyName)
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
/// EXCEPTIONS:
/// InvalidPropertyType
MgDateTime* MgProxyFeatureReader::GetDateTime(INT32 index)
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
/// EXCEPTIONS:
/// InvalidPropertyType
float MgProxyFeatureReader::GetSingle(CREFSTRING propertyName)
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
/// EXCEPTIONS:
/// InvalidPropertyType
float MgProxyFeatureReader::GetSingle(INT32 index)
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
/// EXCEPTIONS:
/// InvalidPropertyType
double MgProxyFeatureReader::GetDouble(CREFSTRING propertyName)
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
/// EXCEPTIONS:
/// InvalidPropertyType
double MgProxyFeatureReader::GetDouble(INT32 index)
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
/// EXCEPTIONS:
/// InvalidPropertyType
INT16 MgProxyFeatureReader::GetInt16(CREFSTRING propertyName)
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
/// EXCEPTIONS:
/// InvalidPropertyType
INT16 MgProxyFeatureReader::GetInt16(INT32 index)
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
/// EXCEPTIONS:
/// InvalidPropertyType
INT32 MgProxyFeatureReader::GetInt32(CREFSTRING propertyName)
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
/// EXCEPTIONS:
/// InvalidPropertyType
INT32 MgProxyFeatureReader::GetInt32(INT32 index)
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
/// EXCEPTIONS:
/// InvalidPropertyType
INT64 MgProxyFeatureReader::GetInt64(CREFSTRING propertyName)
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
/// EXCEPTIONS:
/// InvalidPropertyType
INT64 MgProxyFeatureReader::GetInt64(INT32 index)
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
/// EXCEPTIONS:
/// InvalidPropertyType
STRING MgProxyFeatureReader::GetString(CREFSTRING propertyName)
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
/// EXCEPTIONS:
/// InvalidPropertyType
STRING MgProxyFeatureReader::GetString(INT32 index)
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
/// EXCEPTIONS:
/// InvalidPropertyType
MgByteReader* MgProxyFeatureReader::GetBLOB(CREFSTRING propertyName)
{
    Ptr<MgBlobProperty> ptrProp = (MgBlobProperty*)GetProperty(propertyName, MgPropertyType::Blob);
    Ptr<MgByteReader> retVal = ptrProp->GetValue();

    return SAFE_ADDREF((MgByteReader*)retVal);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the BLOB value of the specified property. No conversion is
/// performed, thus the property must be a of type BLOBs or the result
/// is NULL</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the BLOB value.</returns>
/// EXCEPTIONS:
/// InvalidPropertyType
MgByteReader* MgProxyFeatureReader::GetBLOB(INT32 index)
{
    Ptr<MgBlobProperty> ptrProp = (MgBlobProperty*)GetProperty(index, MgPropertyType::Blob);
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
/// EXCEPTIONS:
/// InvalidPropertyType
MgByteReader* MgProxyFeatureReader::GetCLOB(CREFSTRING propertyName)
{
    Ptr<MgClobProperty> ptrProp = (MgClobProperty*)GetProperty(propertyName, MgPropertyType::Clob);
    Ptr<MgByteReader> retVal = ptrProp->GetValue();

    return SAFE_ADDREF((MgByteReader*)retVal);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the CLOB value of the specified property. No conversion is
/// performed, thus the property must be a of type CLOB or the result
/// is NULL</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the CLOB value.</returns>
/// EXCEPTIONS:
/// InvalidPropertyType
MgByteReader* MgProxyFeatureReader::GetCLOB(INT32 index)
{
    Ptr<MgClobProperty> ptrProp = (MgClobProperty*)GetProperty(index, MgPropertyType::Clob);
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
MgFeatureReader* MgProxyFeatureReader::GetFeatureObject(CREFSTRING propertyName)
{
    Ptr<MgFeatureProperty> ptrProp = (MgFeatureProperty*)GetProperty(propertyName, MgPropertyType::Feature);
    Ptr<MgFeatureReader> retVal = ptrProp->GetValue();

    return SAFE_ADDREF((MgFeatureReader*)retVal);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the FeatureReader to access this object value.
/// The property must be of an object type{} otherwise, the result is NULL.
/// </summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the feature reader to access this object.</returns>
/// EXCEPTIONS:
/// InvalidPropertyType
MgFeatureReader* MgProxyFeatureReader::GetFeatureObject(INT32 index)
{
    Ptr<MgFeatureProperty> ptrProp = (MgFeatureProperty*)GetProperty(index, MgPropertyType::Feature);
    Ptr<MgFeatureReader> retVal = ptrProp->GetValue();

    return SAFE_ADDREF((MgFeatureReader*)retVal);
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
MgByteReader* MgProxyFeatureReader::GetGeometry(CREFSTRING propertyName)
{
    Ptr<MgGeometryProperty> ptrProp = (MgGeometryProperty*)GetProperty(propertyName, MgPropertyType::Geometry);
    Ptr<MgByteReader> retVal = ptrProp->GetValue();

    return SAFE_ADDREF((MgByteReader*)retVal);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the geometry value of the specified property as a GeometryReader.
/// Because no conversion is performed, the property must be
/// of Geometric type{} otherwise, the result is NULL.</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns the Geometry object.</returns>
/// EXCEPTIONS:
/// InvalidPropertyType
MgByteReader* MgProxyFeatureReader::GetGeometry(INT32 index)
{
    Ptr<MgGeometryProperty> ptrProp = (MgGeometryProperty*)GetProperty(index, MgPropertyType::Geometry);
    Ptr<MgByteReader> retVal = ptrProp->GetValue();

    return SAFE_ADDREF((MgByteReader*)retVal);
}

// Get the property for the specified name
MgProperty* MgProxyFeatureReader::GetProperty(CREFSTRING propertyName, INT16 expectedType)
{
    Ptr<MgNullableProperty> ptrProp = (MgNullableProperty*)this->GetProperty(propertyName);
    CHECKNULL(ptrProp, L"MgProxyFeatureReader.GetProperty");

    if (ptrProp->IsNull())
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgProxyFeatureReader.GetProperty",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    INT16 propType = ptrProp->GetPropertyType();
    CHECK_PROPERTY_TYPE(propType, expectedType, L"MgProxyFeatureReader.GetProperty");

    return SAFE_ADDREF((MgProperty*)ptrProp);
}

// Get the property for the specified index
MgProperty* MgProxyFeatureReader::GetProperty(INT32 index, INT16 expectedType)
{
    Ptr<MgNullableProperty> ptrProp = (MgNullableProperty*)this->GetProperty(index);
    CHECKNULL(ptrProp, L"MgProxyFeatureReader.GetProperty");

    if (ptrProp->IsNull())
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);
        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgProxyFeatureReader.GetProperty",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    INT16 propType = ptrProp->GetPropertyType();
    CHECK_PROPERTY_TYPE(propType, expectedType, L"MgProxyFeatureReader.GetProperty");

    return SAFE_ADDREF((MgProperty*)ptrProp);
}

// Get the property for the specified name
MgProperty* MgProxyFeatureReader::GetProperty(CREFSTRING propertyName)
{
    CHECKNULL(m_set, L"MgProxyFeatureReader.GetProperty");
    CHECK_FEATURESET_COUNT(m_set, L"MgProxyFeatureReader.GetProperty");

    Ptr<MgPropertyCollection> ptrCol = m_set->GetFeatureAt(m_currRecord-1);
    CHECKNULL(ptrCol, L"MgProxyFeatureReader.GetProperty");

    Ptr<MgProperty> ptrProp = ptrCol->GetItem(propertyName);
    CHECKNULL(ptrProp, L"MgProxyFeatureReader.GetProperty");

    return SAFE_ADDREF((MgProperty*)ptrProp);
}

// Get the property for the specified index
MgProperty* MgProxyFeatureReader::GetProperty(INT32 index)
{
    CHECKNULL(m_set, L"MgProxyFeatureReader.GetProperty");
    CHECK_FEATURESET_COUNT(m_set, L"MgProxyFeatureReader.GetProperty");

    Ptr<MgPropertyCollection> ptrCol = m_set->GetFeatureAt(m_currRecord-1);
    CHECKNULL(ptrCol, L"MgProxyFeatureReader.GetProperty");

    Ptr<MgProperty> ptrProp = ptrCol->GetItem(index);
    CHECKNULL(ptrProp, L"MgProxyFeatureReader.GetProperty");

    return SAFE_ADDREF((MgProperty*)ptrProp);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Serialize data to TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgProxyFeatureReader::Serialize(MgStream* stream)
{
    // This serialize is called from server when Select
    // contains custom computed properties like JENK_CATEGORY, QUANTILE_CATEGORY
    // STDEV_CATEGORY or EQUAL_CATEGORY.
    // This method will never be called in any other case.
    // In this case, there is NO FdoFeatureReader cached on server
    // and therefore m_serverfeatReader will be empty.

    bool operationCompleted = true;

    stream->WriteBoolean(operationCompleted);

    if (operationCompleted)
    {
        stream->WriteString(m_serverfeatReader);
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

void MgProxyFeatureReader::Deserialize(MgStream* stream)
{
    bool operationCompleted = false;

    stream->GetBoolean(operationCompleted);

    if (operationCompleted)
    {
        stream->GetString(m_serverfeatReader);
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
MgByteReader* MgProxyFeatureReader::ToXml()
{
    string xmlStr;
    this->ToXml(xmlStr);

    Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)xmlStr.c_str(), (INT32)xmlStr.length());
    Ptr<MgByteReader> byteReader = byteSource->GetReader();
    return SAFE_ADDREF((MgByteReader*)byteReader);
}

void MgProxyFeatureReader::ToXml(string &str)
{
    CHECKNULL(m_set, L"MgProxyFeatureReader.ToXml");

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

void MgProxyFeatureReader::SetService(MgFeatureService* service)
{
    CHECKNULL(service, L"MgProxyFeatureReader.SetService");

    if (m_service == NULL)
    {
        m_service = SAFE_ADDREF(service);
        SetServiceForFeatureProperties(service);
    }
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Releases all the resources of feature reader.
/// This must be called when user is done with Feature Reader
/// <returns>Nothing</returns>
void MgProxyFeatureReader::Close()
{
    if (m_serverfeatReader != L"")
    {
        MG_TRY()

        bool bClosed = m_service->CloseFeatureReader(m_serverfeatReader);
        m_serverfeatReader = L"";

        MG_CATCH(L"MgProxyFeatureReader.~MgProxyFeatureReader")

        // We do not rethrow the exception while destructing the object. Even if we had problem
        // disposing this feature reader, it will automatically get collected after time out.
    }
}

/// <summary>Gets the raster object of the specified property.
/// the property must be of Raster type; otherwise, an exception is thrown.
/// </summary>
/// <param name="propertyName">Input the property name.</param>
/// <returns>Returns the raster object.</returns>
MgRaster* MgProxyFeatureReader::GetRaster(CREFSTRING propertyName)
{
    Ptr<MgRasterProperty> ptrProp = (MgRasterProperty*)GetProperty(propertyName, MgPropertyType::Raster);
    Ptr<MgRaster> retVal = ptrProp->GetValue();
    retVal->SetMgService(m_service);
    retVal->SetHandle(m_serverfeatReader);

    return SAFE_ADDREF((MgRaster*)retVal);
}

/// <summary>Gets the raster object of the specified property.
/// the property must be of Raster type; otherwise, an exception is thrown.
/// </summary>
/// <param name="index">Input the property index.</param>
/// <returns>Returns the raster object.</returns>
MgRaster* MgProxyFeatureReader::GetRaster(INT32 index)
{
    Ptr<MgRasterProperty> ptrProp = (MgRasterProperty*)GetProperty(index, MgPropertyType::Raster);
    Ptr<MgRaster> retVal = ptrProp->GetValue();
    retVal->SetMgService(m_service);
    retVal->SetHandle(m_serverfeatReader);

    return SAFE_ADDREF((MgRaster*)retVal);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Geometry for the specified property. No conversion is
/// performed, thus the property must be a of type Geometry or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns a ByteReader object</returns>
BYTE_ARRAY_OUT MgProxyFeatureReader::GetGeometry(CREFSTRING propertyName, INT32& length)
{
    throw new MgNotImplementedException(L"MgProxyFeatureReader.GetGeometry",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Geometry for the specified property. No conversion is
/// performed, thus the property must be a of type Geometry or the result
/// is NULL</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns a ByteReader object</returns>
BYTE_ARRAY_OUT MgProxyFeatureReader::GetGeometry(INT32 index, INT32& length)
{
    throw new MgNotImplementedException(L"MgProxyFeatureReader.GetGeometry",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the string value of the specified property. No conversion is
/// performed, thus the property must be a of type string or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the string value.</returns>
const wchar_t* MgProxyFeatureReader::GetString(CREFSTRING propertyName, INT32& length)
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
const wchar_t* MgProxyFeatureReader::GetString(INT32 index, INT32& length)
{
    STRING str = this->GetString(index);
    length = (INT32)str.size();

    return str.c_str();
}

void MgProxyFeatureReader::SetServiceForFeatureProperties(MgFeatureService* service)
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
                        Ptr<MgProxyFeatureReader> reader = (MgProxyFeatureReader*)featProp->GetValue();
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

MgFeatureSet* MgProxyFeatureReader::GetFeatures(INT32 count)
{
    throw new MgNotImplementedException(L"MgProxyFeatureReader.GetFeatures",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

