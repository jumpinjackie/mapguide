#include "MgDesktop.h"
#include "FeatureSetReader.h"

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

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct a MgdFeatureSetReader object from a byte source
///</summary>
///<param name="byteSource">Byte  source object</param>
///
MgdFeatureSetReader::MgdFeatureSetReader(MgFeatureSet* featureSet)
{
    m_currRecord = 0;
    m_set = SAFE_ADDREF(featureSet);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Destruct a MgdFeatureSetReader object
///</summary>
///
MgdFeatureSetReader::~MgdFeatureSetReader()
{
    Close();
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Set the byte source for an empty reader
///</summary>
///<param name="byteSource">Byte source object</param>
///
void MgdFeatureSetReader::AssignFeatureSet(MgFeatureSet* featureSet)
{
    m_currRecord = 0;
    m_set = SAFE_ADDREF(featureSet);
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
bool MgdFeatureSetReader::ReadNext()
{
    CHECKNULL(m_set, L"MgdFeatureSetReader.ReadNext");

    bool foundNextFeature = false;

    INT32 cnt = m_set->GetCount();

    if ( m_currRecord < cnt )
    {
        m_currRecord++;
        foundNextFeature = true;
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
MgClassDefinition* MgdFeatureSetReader::GetClassDefinition()
{
    CHECKNULL(m_set, L"MgdFeatureSetReader.GetClassDefinition");

    return m_set->GetClassDefinition();
}

MgClassDefinition* MgdFeatureSetReader::GetClassDefinitionNoXml()
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
bool MgdFeatureSetReader::IsNull(CREFSTRING propertyName)
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
bool MgdFeatureSetReader::IsNull(INT32 index)
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
bool MgdFeatureSetReader::GetBoolean(CREFSTRING propertyName)
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
bool MgdFeatureSetReader::GetBoolean(INT32 index)
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
BYTE MgdFeatureSetReader::GetByte(CREFSTRING propertyName)
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
BYTE MgdFeatureSetReader::GetByte(INT32 index)
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
MgDateTime* MgdFeatureSetReader::GetDateTime(CREFSTRING propertyName)
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
MgDateTime* MgdFeatureSetReader::GetDateTime(INT32 index)
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
float MgdFeatureSetReader::GetSingle(CREFSTRING propertyName)
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
float MgdFeatureSetReader::GetSingle(INT32 index)
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
double MgdFeatureSetReader::GetDouble(CREFSTRING propertyName)
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
double MgdFeatureSetReader::GetDouble(INT32 index)
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
INT16 MgdFeatureSetReader::GetInt16(CREFSTRING propertyName)
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
INT16 MgdFeatureSetReader::GetInt16(INT32 index)
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
INT32 MgdFeatureSetReader::GetInt32(CREFSTRING propertyName)
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
INT32 MgdFeatureSetReader::GetInt32(INT32 index)
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
INT64 MgdFeatureSetReader::GetInt64(CREFSTRING propertyName)
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
INT64 MgdFeatureSetReader::GetInt64(INT32 index)
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
STRING MgdFeatureSetReader::GetString(CREFSTRING propertyName)
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
STRING MgdFeatureSetReader::GetString(INT32 index)
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
MgByteReader* MgdFeatureSetReader::GetBLOB(CREFSTRING propertyName)
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
/// EXCEPTIONS:
/// InvalidPropertyType
MgByteReader* MgdFeatureSetReader::GetBLOB(INT32 index)
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
/// EXCEPTIONS:
/// InvalidPropertyType
MgByteReader* MgdFeatureSetReader::GetCLOB(CREFSTRING propertyName)
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
/// EXCEPTIONS:
/// InvalidPropertyType
MgByteReader* MgdFeatureSetReader::GetCLOB(INT32 index)
{
    Ptr<MgClobProperty> ptrProp = (MgClobProperty*)GetProperty(index, MgPropertyType::Clob);
    return ptrProp->GetValue();
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
MgFeatureReader* MgdFeatureSetReader::GetFeatureObject(CREFSTRING propertyName)
{
    Ptr<MgFeatureProperty> ptrProp = (MgFeatureProperty*)GetProperty(propertyName, MgPropertyType::Feature);
    return ptrProp->GetValue();
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
MgFeatureReader* MgdFeatureSetReader::GetFeatureObject(INT32 index)
{
    Ptr<MgFeatureProperty> ptrProp = (MgFeatureProperty*)GetProperty(index, MgPropertyType::Feature);
    return ptrProp->GetValue();
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
MgByteReader* MgdFeatureSetReader::GetGeometry(CREFSTRING propertyName)
{
    Ptr<MgGeometryProperty> ptrProp = (MgGeometryProperty*)GetProperty(propertyName, MgPropertyType::Geometry);
    return ptrProp->GetValue();
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
MgByteReader* MgdFeatureSetReader::GetGeometry(INT32 index)
{
    Ptr<MgGeometryProperty> ptrProp = (MgGeometryProperty*)GetProperty(index, MgPropertyType::Geometry);
    return ptrProp->GetValue();
}

// Get the property for the specified name
MgProperty* MgdFeatureSetReader::GetProperty(CREFSTRING propertyName, INT16 expectedType)
{
    Ptr<MgNullableProperty> ptrProp = (MgNullableProperty*)this->GetProperty(propertyName);
    CHECKNULL(ptrProp, L"MgdFeatureSetReader.GetProperty");

    if (ptrProp->IsNull())
    {
        MgStringCollection arguments;
        arguments.Add(propertyName);

        throw new MgNullPropertyValueException(L"MgdFeatureSetReader.GetProperty",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    INT16 propType = ptrProp->GetPropertyType();
    CHECK_PROPERTY_TYPE(propType, expectedType, L"MgdFeatureSetReader.GetProperty");

    return SAFE_ADDREF(ptrProp.p);
}

// Get the property for the specified index
MgProperty* MgdFeatureSetReader::GetProperty(INT32 index, INT16 expectedType)
{
    Ptr<MgNullableProperty> ptrProp = (MgNullableProperty*)this->GetProperty(index);
    CHECKNULL(ptrProp, L"MgdFeatureSetReader.GetProperty");

    if (ptrProp->IsNull())
    {
        STRING buffer;
        MgUtil::Int32ToString(index, buffer);
        MgStringCollection arguments;
        arguments.Add(buffer);

        throw new MgNullPropertyValueException(L"MgdFeatureSetReader.GetProperty",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    INT16 propType = ptrProp->GetPropertyType();
    CHECK_PROPERTY_TYPE(propType, expectedType, L"MgdFeatureSetReader.GetProperty");

    return SAFE_ADDREF(ptrProp.p);
}

// Get the property for the specified name
MgProperty* MgdFeatureSetReader::GetProperty(CREFSTRING propertyName)
{
    CHECKNULL(m_set, L"MgdFeatureSetReader.GetProperty");
    CHECK_FEATURESET_COUNT(m_set, L"MgdFeatureSetReader.GetProperty");

    Ptr<MgPropertyCollection> ptrCol = m_set->GetFeatureAt(m_currRecord-1);
    CHECKNULL(ptrCol, L"MgdFeatureSetReader.GetProperty");

    Ptr<MgProperty> ptrProp = ptrCol->GetItem(propertyName);
    CHECKNULL(ptrProp, L"MgdFeatureSetReader.GetProperty");

    return SAFE_ADDREF(ptrProp.p);
}

// Get the property for the specified index
MgProperty* MgdFeatureSetReader::GetProperty(INT32 index)
{
    CHECKNULL(m_set, L"MgdFeatureSetReader.GetProperty");
    CHECK_FEATURESET_COUNT(m_set, L"MgdFeatureSetReader.GetProperty");

    Ptr<MgPropertyCollection> ptrCol = m_set->GetFeatureAt(m_currRecord-1);
    CHECKNULL(ptrCol, L"MgdFeatureSetReader.GetProperty");

    Ptr<MgProperty> ptrProp = ptrCol->GetItem(index);
    CHECKNULL(ptrProp, L"MgdFeatureSetReader.GetProperty");

    return SAFE_ADDREF(ptrProp.p);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Serialize data to TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgdFeatureSetReader::Serialize(MgStream* stream)
{
    NOT_IMPLEMENTED(L"MgdFeatureSetReader::Serialize");
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserialize data from TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>

void MgdFeatureSetReader::Deserialize(MgStream* stream)
{
    NOT_IMPLEMENTED(L"MgdFeatureSetReader::Deserialize");
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Serializes all features into an XML.
/// XML is serialized from the current position of feature reader in the order
/// data are retrieved.
/// <returns>MgByteReader holding XML.</returns>
MgByteReader* MgdFeatureSetReader::ToXml()
{
    string xmlStr;
    this->ToXml(xmlStr);

    Ptr<MgByteSource> byteSource = new MgByteSource((BYTE_ARRAY_IN)xmlStr.c_str(), (INT32)xmlStr.length());
    byteSource->SetMimeType(MgMimeType::Xml);

    return byteSource->GetReader();
}

void MgdFeatureSetReader::ToXml(string &str)
{
    CHECKNULL(m_set, L"MgdFeatureSetReader.ToXml");

    Ptr<MgClassDefinition> classDef = this->GetClassDefinition();

    if (classDef != NULL)
    {
        // TODO: define a schema for this XML
        // TODO: rename FeatureSet element to avoid conflict with FeatureSet-1.0.0.xsd?
        ResponseStartUtf8(str);
        HeaderToStringUtf8(str);
        BodyStartUtf8(str);
        while ( this->ReadNext() )
        {
            CurrentToStringUtf8(str);
        }
        BodyEndUtf8(str);
        ResponseEndUtf8(str);
    }
}

string MgdFeatureSetReader::GetResponseElementName()
{
    return "FeatureSet";
}

string MgdFeatureSetReader::GetBodyElementName()
{
    return "Features";
}

void MgdFeatureSetReader::ResponseStartUtf8(string& str)
{
    str += "<?xml version=\"1.0\" encoding=\"UTF-8\"?>";
    str += "<";
    str += GetResponseElementName();
    str += ">";
}

void MgdFeatureSetReader::ResponseEndUtf8(string& str)
{
    str += "</";
    str += GetResponseElementName();
    str += ">";
}

void MgdFeatureSetReader::BodyStartUtf8(string& str)
{
    str += "<";
    str += GetBodyElementName();
    str += ">";
}

void MgdFeatureSetReader::BodyEndUtf8(string& str)
{
    str += "</";
    str += GetBodyElementName();
    str += ">";
}

void MgdFeatureSetReader::HeaderToStringUtf8(string& str)
{
    Ptr<MgClassDefinition> classDef = this->GetClassDefinition();
    if (classDef != NULL)
    {
        classDef->ToXml(str);
    }
}

void MgdFeatureSetReader::CurrentToStringUtf8(string& str)
{
    if (NULL != (MgFeatureSet*)m_set)
    {
        Ptr<MgPropertyCollection> propCol = m_set->GetFeatureAt(m_currRecord-1);
        INT32 cnt = propCol->GetCount();
        if (propCol != NULL && cnt > 0)
        {
            propCol->ToFeature(str);
        }
    }
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Releases all the resources of feature reader.
/// This must be called when user is done with Feature Reader
/// <returns>Nothing</returns>
void MgdFeatureSetReader::Close()
{
}

/// <summary>Gets the raster object of the specified property.
/// the property must be of Raster type; otherwise, an exception is thrown.
/// </summary>
/// <param name="propertyName">Input the property name.</param>
/// <returns>Returns the raster object.</returns>
MgRaster* MgdFeatureSetReader::GetRaster(CREFSTRING propertyName)
{
    NOT_IMPLEMENTED(L"MgdFeatureSetReader::GetRaster");
}

/// <summary>Gets the raster object of the specified property.
/// the property must be of Raster type; otherwise, an exception is thrown.
/// </summary>
/// <param name="index">Input the property index.</param>
/// <returns>Returns the raster object.</returns>
MgRaster* MgdFeatureSetReader::GetRaster(INT32 index)
{
    NOT_IMPLEMENTED(L"MgdFeatureSetReader::GetRaster");
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Geometry for the specified property. No conversion is
/// performed, thus the property must be a of type Geometry or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns a ByteReader object</returns>
BYTE_ARRAY_OUT MgdFeatureSetReader::GetGeometry(CREFSTRING propertyName, INT32& length)
{
    throw new MgNotImplementedException(L"MgdFeatureSetReader.GetGeometry",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the Geometry for the specified property. No conversion is
/// performed, thus the property must be a of type Geometry or the result
/// is NULL</summary>
/// <param name="index">Property index.</param>
/// <returns>Returns a ByteReader object</returns>
BYTE_ARRAY_OUT MgdFeatureSetReader::GetGeometry(INT32 index, INT32& length)
{
    throw new MgNotImplementedException(L"MgdFeatureSetReader.GetGeometry",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Gets the string value of the specified property. No conversion is
/// performed, thus the property must be a of type string or the result
/// is NULL</summary>
/// <param name="propertyName">Property name.</param>
/// <returns>Returns the string value.</returns>
const wchar_t* MgdFeatureSetReader::GetString(CREFSTRING propertyName, INT32& length)
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
const wchar_t* MgdFeatureSetReader::GetString(INT32 index, INT32& length)
{
    STRING str = this->GetString(index);
    length = (INT32)str.size();

    return str.c_str();
}

MgFeatureSet* MgdFeatureSetReader::GetFeatures(INT32 count)
{
    throw new MgNotImplementedException(L"MgdFeatureSetReader.GetFeatures",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

INT32 MgdFeatureSetReader::GetReaderType() { return MgReaderType::FeatureReader; }