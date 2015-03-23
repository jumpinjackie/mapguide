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

#ifndef DESKTOP_GWS_FEATURE_READER_H
#define DESKTOP_GWS_FEATURE_READER_H

#include "FeatureDefs.h"
#include "GwsQueryEngine.h"

///\cond INTERNAL
class GwsRightSideIterator
{
public:
    GwsRightSideIterator(IGWSFeatureIterator* iter, bool b):m_iterator(iter), m_bHasData(b){}

    IGWSFeatureIterator * Iterator(){return m_iterator;};
    bool HasData(){return m_bHasData;}

private:
    IGWSFeatureIterator* m_iterator;

    // Fix ticket #1162
    // When we are processing left-outer joins, this field is needed because we need the iterator
    // to describe the right-side result. In the meanwhile, we need a flag to indicate that this
    // iterator does not contains data, and we will return all null value for the right side
    // features
    bool                m_bHasData;
};

typedef std::multimap<STRING, GwsRightSideIterator> GwsFeatureIteratorMap;
typedef std::pair<STRING, GwsRightSideIterator> GwsFeatureIteratorPair;

class MgdJoinFeatureReader;
class FdoExpressionEngine;
class FdoFilter;
class MgdGwsConnectionPool;

/////////////////////////////////////////////////////////////////
/// <summary>
/// The GwsFeatureReader interface provides a forward-only, read-only
/// iterator for reading data from the GwsQueryEngine.
/// The initial position of the GwsFeatureReader interface is prior to the first item.
/// Thus, you must call ReadNext to begin accessing any data..
/// </summary>

class MG_DESKTOP_API MgdGwsFeatureReader : public MgFeatureReader
{
EXTERNAL_API:

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
    bool ReadNext();

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the definition of the object currently being read. If the user
    /// has requested only a subset of the class properties, the class
    /// definition reflects what the user has asked, rather than the full class
    /// definition.
    /// </summary>
    /// <returns>A MgClassDefinition representing the current object
    ///</returns>
    MgClassDefinition* GetClassDefinition();

    //////////////////////////////////////////////////////////////////
    /// <summary>
    ///  Returns true if the value of the specified property is null.
    /// </summary>
    /// <param name="propertyName">Property name.</param>
    /// <returns>Returns true if the value is null.</returns>
    bool IsNull(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the Boolean value of the specified property. No conversion is
    /// performed, thus the property must be a of boolean type the result
    /// is undertermined</summary>
    /// <param name="propertyName">Property name.</param>
    /// <returns>Returns the Boolean value.</returns>
    bool GetBoolean(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the Byte value of the specified property. No conversion is
    /// performed, thus the property must be a of byte type or the result
    /// is undertermined</summary>
    /// <param name="propertyName">Property name.</param>
    /// <returns>Returns the Byte value.</returns>
    BYTE GetByte(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the DTime value of the specified property. No conversion is
    /// performed, thus the property must be a of date type or the result
    /// is NULL</summary>
    /// <param name="propertyName">Property name.</param>
    /// <returns>Returns the DTime value.</returns>
    MgDateTime* GetDateTime(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the Single value of the specified property. No conversion is
    /// performed, thus the property must be a of type single or the result
    /// is undetermined</summary>
    /// <param name="propertyName">Property name.</param>
    /// <returns>Returns the single value.</returns>
    float GetSingle(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the Double value of the specified property. No conversion is
    /// performed, thus the property must be a of type double or the result
    /// is undetermined</summary>
    /// <param name="propertyName">Property name.</param>
    /// <returns>Returns the double value.</returns>
    double GetDouble(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the integer 16 bits value of the specified property. No conversion is
    /// performed, thus the property must be a of type integer 16 bits or the result
    /// is undetermined</summary>
    /// <param name="propertyName">Property name.</param>
    /// <returns>Returns the integer 16 bits value.</returns>
    INT16 GetInt16(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the integer 32 bits value of the specified property. No conversion is
    /// performed, thus the property must be a of type integer 32 bits or the result
    /// is undetermined</summary>
    /// <param name="propertyName">Property name.</param>
    /// <returns>Returns the integer 32 bits value.</returns>
    INT32 GetInt32(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the integer 64 bits value of the specified property. No conversion is
    /// performed, thus the property must be a of type integer 64 bits or the result
    /// is NULL</summary>
    /// <param name="propertyName">Property name.</param>
    /// <returns>Returns the integer 64 bits value.
    /// Note: INT64 is actually a pointer to an Integer64 object
    ///</returns>
    INT64 GetInt64(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the string value of the specified property. No conversion is
    /// performed, thus the property must be a of type string or the result
    /// is NULL</summary>
    /// <param name="propertyName">Property name.</param>
    /// <returns>Returns the string value.</returns>
    STRING GetString(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the BLOB value of the specified property. No conversion is
    /// performed, thus the property must be a of type BLOBs or the result
    /// is NULL</summary>
    /// <param name="propertyName">Property name.</param>
    /// <returns>Returns the BLOB value.</returns>
    MgByteReader* GetBLOB(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the CLOB value of the specified property. No conversion is
    /// performed, thus the property must be a of type CLOB or the result
    /// is NULL</summary>
    /// <param name="propertyName">Property name.</param>
    /// <returns>Returns the CLOB value.</returns>
    MgByteReader* GetCLOB(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the FeatureReader to access this object value.
    /// The property must be of an object type; otherwise, the result is NULL.
    /// </summary>
    /// <param name="propertyName">Input the property name.</param>
    /// <returns>Returns the feature reader to access this object.</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not a feature
    MgFeatureReader* GetFeatureObject(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the Geometry for the specified property. No conversion is
    /// performed, thus the property must be a of type Geometry or the result
    /// is NULL</summary>
    /// <param name="propertyName">Property name.</param>
    /// <returns>Returns a ByteReader object</returns>
    MgByteReader* GetGeometry(CREFSTRING propertyName);

    /// <summary>Gets the raster object of the specified property.
    /// the property must be of Raster type; otherwise, an exception is thrown.
    /// </summary>
    /// <param name="propertyName">Input the property name.</param>
    /// <returns>Returns the raster object.</returns>
    /// EXCEPTIONS:
    /// MgConnectionNotOpenException
    /// MgNullPropertyValueException
    /// MgFdoException
    MgRaster* GetRaster(CREFSTRING propertyName);


    //////////////////////////////////////////////////////////////////
    /// <summary>
    ///  Returns true if the value of the property at the specified index is null.
    /// </summary>
    /// <param name="index">Property index.</param>
    /// <returns>Returns true if the value is null.</returns>
    bool IsNull(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the Boolean value of the property at the specified index. No conversion is
    /// performed, thus the property must be a of boolean type the result
    /// is undertermined</summary>
    /// <param name="index">Property index.</param>
    /// <returns>Returns the Boolean value.</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not boolean
    bool GetBoolean(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the Byte value of the property at the specified index. No conversion is
    /// performed, thus the property must be a of byte type or the result
    /// is undertermined</summary>
    /// <param name="index">Property index.</param>
    /// <returns>Returns the Byte value.</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not byte
    BYTE GetByte(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the DTime value of the property at the specified index. No conversion is
    /// performed, thus the property must be a of date type or the result
    /// is NULL</summary>
    /// <param name="index">Property index.</param>
    /// <returns>Returns the DTime value.</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not date/time
    MgDateTime* GetDateTime(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the Single value of the property at the specified index. No conversion is
    /// performed, thus the property must be a of type single or the result
    /// is undetermined</summary>
    /// <param name="index">Property index.</param>
    /// <returns>Returns the single value.</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not float
    float GetSingle(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the Double value of the property at the specified index. No conversion is
    /// performed, thus the property must be a of type double or the result
    /// is undetermined</summary>
    /// <param name="index">Property index.</param>
    /// <returns>Returns the double value.</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not double
    double GetDouble(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the integer 16 bits value of the property at the specified index. No conversion is
    /// performed, thus the property must be a of type integer 16 bits or the result
    /// is undetermined</summary>
    /// <param name="index">Property index.</param>
    /// <returns>Returns the integer 16 bits value.</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not int16
    INT16 GetInt16(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the integer 32 bits value of the property at the specified index. No conversion is
    /// performed, thus the property must be a of type integer 32 bits or the result
    /// is undetermined</summary>
    /// <param name="index">Property index.</param>
    /// <returns>Returns the integer 32 bits value.</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not int32
    INT32 GetInt32(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the integer 64 bits value of the property at the specified index. No conversion is
    /// performed, thus the property must be a of type integer 64 bits or the result
    /// is NULL</summary>
    /// <param name="index">Property index.</param>
    /// <returns>Returns the integer 64 bits value.
    /// Note: INT64 is actually a pointer to an Integer64 object
    ///</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not int64
    INT64 GetInt64(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the string value of the property at the specified index. No conversion is
    /// performed, thus the property must be a of type string or the result
    /// is NULL</summary>
    /// <param name="index">Property index.</param>
    /// <returns>Returns the string value.</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not a string
    STRING GetString(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the BLOB value of the property at the specified index. No conversion is
    /// performed, thus the property must be a of type BLOBs or the result
    /// is NULL</summary>
    /// <param name="index">Property index.</param>
    /// <returns>Returns the BLOB value.</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not a BLOB
    MgByteReader* GetBLOB(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the CLOB value of the property at the specified index. No conversion is
    /// performed, thus the property must be a of type CLOB or the result
    /// is NULL</summary>
    /// <param name="index">Property index.</param>
    /// <returns>Returns the CLOB value.</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not a CLOB
    MgByteReader* GetCLOB(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the FeatureReader to access this object value.
    /// The property must be of an object type; otherwise, the result is NULL.
    /// </summary>
    /// <param name="index">Property index.</param>
    /// <returns>Returns the feature reader to access this object.</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not a feature
    MgFeatureReader* GetFeatureObject(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the geometry value of the property at the specified index as a GeometryReader.
    /// Because no conversion is performed, the property must be
    /// of Geometric type; otherwise, the result is NULL.</summary>
    /// <param name="index">Property index.</param>
    /// <returns>Returns the Geometry object.</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not geometry
    MgByteReader* GetGeometry(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// </summary>
    /// <param name="index">Property index.</param>
    /// <returns>Returns the Raster object.</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not geometry
    MgRaster* GetRaster(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Closes the FeatureReader object, freeing any resources it may be holding.
    /// </summary>
    void Close();

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Serializes all features into an XML.
    /// XML is serialized from the current position of feature reader in the order
    /// data are retrieved.
    /// <returns>MgByteReader holding XML.</returns>
    MgByteReader* ToXml();

protected:

    virtual void Dispose()
    {
        delete this;
    }

INTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the starting element name as a UTF-8 string. The mime
    /// type must be a text type, for example text/xml.
    ///
    /// \param str
    /// Destination string.
    ///
    virtual string GetResponseElementName();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the body starting element name as a UTF-8 string. The mime
    /// type must be a text type, for example text/xml.
    ///
    /// \param str
    /// Destination string.
    ///
    virtual string GetBodyElementName();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the start of the response as a UTF-8 string. The mime
    /// type must be a text type, for example text/xml.
    ///
    /// \param str
    /// Destination string.
    ///
    virtual void ResponseStartUtf8(string& str);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the end of the response as a UTF-8 string. The mime
    /// type must be a text type, for example text/xml.
    ///
    /// \param str
    /// Destination string.
    ///
    virtual void ResponseEndUtf8(string& str);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the start of the response body as a UTF-8 string. The mime
    /// type must be a text type, for example text/xml.
    ///
    /// \param str
    /// Destination string.
    ///
    virtual void BodyStartUtf8(string& str);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the end of the response body as a UTF-8 string. The mime
    /// type must be a text type, for example text/xml.
    ///
    /// \param str
    /// Destination string.
    ///
    virtual void BodyEndUtf8(string& str);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the contents of the header in this reader as a UTF-8 string.  The mime
    /// type must be a text type, for example text/xml.
    ///
    /// \param str
    /// Destination string.
    ///
    virtual void HeaderToStringUtf8(string& str);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the contents of the current record/feature in the reader as a UTF-8 string.  The mime
    /// type must be a text type, for example text/xml.
    ///
    /// \param str
    /// Destination string.
    ///
    virtual void CurrentToStringUtf8(string& str);

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
    MgClassDefinition* GetClassDefinitionNoXml();

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the Geometry for the specified property. No conversion is
    /// performed, thus the property must be a of type Geometry or the result
    /// is NULL</summary>
    /// <param name="propertyName">Property name.</param>
    /// <returns>Returns a ByteReader object</returns>
    BYTE_ARRAY_OUT GetGeometry(CREFSTRING propertyName, INT32& length);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the Geometry for the specified property. No conversion is
    /// performed, thus the property must be a of type Geometry or the result
    /// is NULL</summary>
    /// <param name="index">Property index.</param>
    /// <returns>Returns a ByteReader object</returns>
    BYTE_ARRAY_OUT GetGeometry(INT32 index, INT32& length);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the string value of the specified property. No conversion is
    /// performed, thus the property must be a of type string or the result
    /// is NULL</summary>
    /// <param name="propertyName">Property name.</param>
    /// <returns>Returns the string value.</returns>
    const wchar_t* GetString(CREFSTRING propertyName, INT32& length);

    virtual INT32 GetClassId()
    {
        return m_cls_id;
    }

    //////////////////////////////////////////////////////////////////
    ///<summary>
    /// Serialize data to TCP/IP stream
    ///</summary>
    ///<param name="stream">
    /// Stream
    ///</param>

    virtual void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    ///<summary>
    /// Deserialize data from TCP/IP stream
    ///</summary>
    ///<param name="stream">
    /// Stream
    ///</param>

    virtual void Deserialize(MgStream* stream);

    virtual MgFeatureSet* GetFeatures(INT32 count);

    MgdGwsFeatureReader();

    MgdGwsFeatureReader(MgdGwsConnectionPool* pool,
                        IGWSFeatureIterator* gwsFeatureIterator,
                        IGWSFeatureIterator* gwsFeatureIteratorCopy,
                        CREFSTRING extensionName,
                        FdoStringCollection* relationNames,
                        bool bForceOneToOne = true,
                        MgStringCollection* attributeNameDelimiters = NULL);
    ~MgdGwsFeatureReader();


    void PrepareGwsGetFeatures(CREFSTRING extensionName, FdoStringCollection* relationNames);

    void DeterminePropertyFeatureSource(
        CREFSTRING inputPropName,
        IGWSFeatureIterator** gwsFeatureIter,
        STRING& relationName,
        STRING& className,
        STRING& parsedPropName);

    GwsFeatureIteratorMap GetSecondaryGwsFeatureIteratorMap();
    void SetGwsFeatureIteratorMap(GwsFeatureIteratorMap featureIterMap);

    void SetAdvancePrimaryIterator(bool bAdvancePrimary);

    MgStringCollection* GetAttributeNameDelimiters();
    bool IsForceOneToOne();

    IGWSFeatureIterator* GetFeatureIterator();
    void DeterminePropertyFeatureSource(CREFSTRING inputPropName, IGWSFeatureIterator** gwsFeatureIter, STRING& parsedPropName);
    void SetFilter(FdoFilter* filter);
    FdoIFeatureReader* GetJoinFeatureReader();
    void AddFeatures(INT32 count);
    void AddFeature(MgPropertyDefinitionCollection* propDefCol);
    MgClassDefinition* GetMgClassDefinition(bool bSerialize);
    MgByteReader* SerializeToXml(FdoClassDefinition* classDef);

    void OwnsConnections();
    MgByteReader* GetRaster(STRING rasterPropName, INT32 xSize, INT32 ySize);

private:

    FdoPtr<IGWSFeatureIterator> m_gwsFeatureIterator;
    GwsFeatureIteratorMap m_secondaryGwsFeatureIteratorMap;
    FdoPtr<IGWSFeatureIterator> m_gwsFeatureIteratorCopy;
    FdoPtr<IGWSExtendedFeatureDescription> m_primaryExtendedFeatureDescription;

    bool m_bAdvancePrimaryIterator;
    bool m_bForceOneToOne;
    bool m_removeFromPoolOnDestruction;
    bool m_bNoMoreData;
    Ptr<MgStringCollection> m_attributeNameDelimiters;

    FdoPtr<MgdJoinFeatureReader> m_joinReader;
    FdoPtr<FdoExpressionEngine> m_expressionEngine;
    FdoPtr<FdoFilter> m_filter;

    Ptr<MgClassDefinition> m_classDef;
    Ptr<MgFeatureSet> m_featureSet;
    FdoPtr<FdoStringCollection> m_relationNames;
    STRING m_extensionName;
    FdoPtr<MgdGwsConnectionPool> m_pool;

    void ParseSecondaryPropertyName(CREFSTRING inputPropName, CREFSTRING delimiter, CREFSTRING secondaryProp, STRING& relationName, STRING& parsedPropName);

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_GwsFeatureReader;
};
///\endcond
#endif
