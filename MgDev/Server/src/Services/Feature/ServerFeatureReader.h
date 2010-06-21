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

#ifndef _MG_SERVER_FEATURE_READER_H
#define _MG_SERVER_FEATURE_READER_H

#include "ServerFeatureDllExport.h"
#include "ServerFeatureServiceDefs.h"
#include "ServerFeatureConnection.h"

/////////////////////////////////////////////////////////////////
///<summary>
/// The FeatureReader class provides a one way iterator for reading feature data.
/// The initial position of the FeatureReader is prior to
/// the first item.  ReadNext should be called to begin accessing the data.
/// GetClassDefintion returns the feature class definition for
/// the feature data.  The Get* methods operate on the current feature.  The
/// next feature is obtained by calling ReadNext.
///</summary>

class MG_SERVER_FEATURE_API MgServerFeatureReader : public MgFeatureReader
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
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not boolean
    bool GetBoolean(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the Byte value of the specified property. No conversion is
    /// performed, thus the property must be a of byte type or the result
    /// is undertermined</summary>
    /// <param name="propertyName">Property name.</param>
    /// <returns>Returns the Byte value.</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not byte
    BYTE GetByte(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the DTime value of the specified property. No conversion is
    /// performed, thus the property must be a of date type or the result
    /// is NULL</summary>
    /// <param name="propertyName">Property name.</param>
    /// <returns>Returns the DTime value.</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not date/time
    MgDateTime* GetDateTime(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the Single value of the specified property. No conversion is
    /// performed, thus the property must be a of type single or the result
    /// is undetermined</summary>
    /// <param name="propertyName">Property name.</param>
    /// <returns>Returns the single value.</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not float
    float GetSingle(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the Double value of the specified property. No conversion is
    /// performed, thus the property must be a of type double or the result
    /// is undetermined</summary>
    /// <param name="propertyName">Property name.</param>
    /// <returns>Returns the double value.</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not double
    double GetDouble(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the integer 16 bits value of the specified property. No conversion is
    /// performed, thus the property must be a of type integer 16 bits or the result
    /// is undetermined</summary>
    /// <param name="propertyName">Property name.</param>
    /// <returns>Returns the integer 16 bits value.</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not int16
    INT16 GetInt16(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the integer 32 bits value of the specified property. No conversion is
    /// performed, thus the property must be a of type integer 32 bits or the result
    /// is undetermined</summary>
    /// <param name="propertyName">Property name.</param>
    /// <returns>Returns the integer 32 bits value.</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not int32
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
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not int64
    INT64 GetInt64(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the string value of the specified property. No conversion is
    /// performed, thus the property must be a of type string or the result
    /// is NULL</summary>
    /// <param name="propertyName">Property name.</param>
    /// <returns>Returns the string value.</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not a string
    STRING GetString(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the BLOB value of the specified property. No conversion is
    /// performed, thus the property must be a of type BLOBs or the result
    /// is NULL</summary>
    /// <param name="propertyName">Property name.</param>
    /// <returns>Returns the BLOB value.</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not a BLOB
    MgByteReader* GetBLOB(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the CLOB value of the specified property. No conversion is
    /// performed, thus the property must be a of type CLOB or the result
    /// is NULL</summary>
    /// <param name="propertyName">Property name.</param>
    /// <returns>Returns the CLOB value.</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not a CLOB
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
    /// Gets the geometry value of the specified property as a GeometryReader.
    /// Because no conversion is performed, the property must be
    /// of Geometric type; otherwise, the result is NULL.</summary>
    /// <param name="propertyName">Input the property name.</param>
    /// <returns>Returns the Geometry object.</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not geometry
    MgByteReader* GetGeometry(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// </summary>
    /// <param name="propertyName">Input the property name.</param>
    /// <returns>Returns the Raster object.</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not geometry
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
    /// Serializes all features into an XML.
    /// XML is serialized from the current position of feature reader in the order
    /// data are retrieved.
    /// <returns>MgByteReader holding XML.</returns>
    MgByteReader* ToXml();

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Releases all the resources of feature reader.
    /// This must be called when user is done with Feature Reader
    /// <returns>Nothing</returns>
    void Close();

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
    /// Gets the Geometry for the property at the specified index. No conversion is
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

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the string value of the specified property. No conversion is
    /// performed, thus the property must be a of type string or the result
    /// is NULL</summary>
    /// <param name="index">Property index.</param>
    /// <returns>Returns the string value.</returns>
    const wchar_t* GetString(INT32 index, INT32& length);

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

    MgServerFeatureReader();

    MgServerFeatureReader(MgServerFeatureConnection* connection, FdoIFeatureReader* fdoReader);
    ~MgServerFeatureReader();

    virtual void Dispose();

    virtual INT32 GetClassId() { return m_cls_id; }

    FdoIFeatureReader* GetInternalReader() { return FDO_SAFE_ADDREF(m_fdoReader); }
    void AddFeatures(INT32 count);
    void AddFeature(MgPropertyDefinitionCollection* propDefCol);
    MgByteReader* GetRaster(STRING rasterPropName, INT32 xSize, INT32 ySize);

private:

    Ptr<MgClassDefinition>  m_classDef;
    Ptr<MgFeatureSet>       m_featureSet;
    Ptr<MgServerFeatureConnection> m_connection;
    FdoIFeatureReader* m_fdoReader;
    bool m_removeFromPoolOnDestruction;

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_FeatureReader;
};
#endif
