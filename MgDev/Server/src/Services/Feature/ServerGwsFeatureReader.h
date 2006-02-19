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

#ifndef _MG_SERVER_GWS_FEATURE_READER_H_
#define _MG_SERVER_GWS_FEATURE_READER_H_

#include "ServerFeatureDllExport.h"
#include "ServerFeatureServiceDefs.h"
#include "ServerGwsGetFeatures.h"
#include "GwsQueryEngine.h"

typedef std::multimap<STRING, IGWSFeatureIterator*> GwsFeatureIteratorMap;
typedef std::pair<STRING, IGWSFeatureIterator*> GwsFeatureIteratorPair;

/////////////////////////////////////////////////////////////////
/// <summary>
/// The GwsFeatureReader interface provides a forward-only, read-only
/// iterator for reading data from the GwsQueryEngine.
/// The initial position of the GwsFeatureReader interface is prior to the first item.
/// Thus, you must call ReadNext to begin accessing any data..
/// </summary>

class MG_SERVER_FEATURE_API  MgServerGwsFeatureReader : public MgFeatureReader
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

    MgServerGwsFeatureReader();

    MgServerGwsFeatureReader(IGWSFeatureIterator* gwsFeatureIterator);
    ~MgServerGwsFeatureReader();


    void PrepareGwsGetFeatures(CREFSTRING extensionName, GisStringCollection* relationNames);

    void DeterminePropertyFeatureSource(
        CREFSTRING inputPropName,
        IGWSFeatureIterator** gwsFeatureIter,
        STRING& relationName,
        STRING& className,
        STRING& parsedPropName);

    void SetGwsIteratorCopy(IGWSFeatureIterator* iterCopy);

private:

    IGWSFeatureIterator* m_gwsFeatureIterator;
    MgServerGwsGetFeatures* m_gwsGetFeatures;
    GwsFeatureIteratorMap m_secondaryGwsFeatureIteratorMap;
    IGWSFeatureIterator* m_gwsFeatureIteratorCopy;

    void DeterminePropertyFeatureSource(CREFSTRING inputPropName, IGWSFeatureIterator** gwsFeatureIter, STRING& parsedPropName);

CLASS_ID:
    static const INT32 m_cls_id = FeatureService_GwsFeatureReader;

};

#endif
