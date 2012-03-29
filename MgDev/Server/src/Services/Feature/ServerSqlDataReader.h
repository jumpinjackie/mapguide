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

#ifndef _MG_SERVER_SQL_DATA_READER_H_
#define _MG_SERVER_SQL_DATA_READER_H_

class MgStream;
class MgByteReader;
class FdoISQLDataReader;
class MgDateTime;
class MgSqlDataReader;

#include "ServerFeatureDllExport.h"
#include "ServerFeatureConnection.h"

/////////////////////////////////////////////////////////////////
/// <summary>
/// The SqlReader interface provides a forward-only, read-only
/// iterator for reading relational table data.
/// The initial position of the SqlReader interface is prior to the first item.
/// Thus, you must call ReadNext to begin accessing any data..
/// </summary>

class MG_SERVER_FEATURE_API  MgServerSqlDataReader : public MgSqlDataReader
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
    /// Gets the number of properties in the result set.
    /// </summary>
    /// <returns>Returns the number of properties.</returns>
    INT32 GetPropertyCount();

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the name of the property at the given ordinal position.
    /// </summary>
    /// <param name="index">Input the position of the property.</param>
    /// <returns>Returns the property name</returns>
    STRING GetPropertyName(INT32 index);

    /////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the index of the property with the specified property name.
    ///
    /// \param propertyName(string)
    /// The name of the property in the list of
    /// properties belonging to the feature currently
    /// being read.
    ///
    /// \return
    /// Returns the property index.
    ///
    INT32 GetPropertyIndex(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the data type of the property with the specified name.
    /// </summary>
    /// <param name="propertyName">Input the property name.</param>
    /// <returns>Returns the type of the property.</returns>
    INT32 GetPropertyType(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the data type of the property at the specified index.
    /// </summary>
    /// <param name="index">Input the property index.</param>
    /// <returns>Returns the type of the property.</returns>
    INT32 GetPropertyType(INT32 index);

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

    virtual INT32 GetClassId()
    {
        return m_cls_id;
    }

    MgServerSqlDataReader(MgServerFeatureConnection* connection, FdoISQLDataReader* sqlReader, CREFSTRING providerName);
    MgServerSqlDataReader();
    ~MgServerSqlDataReader();
    void Deserialize(MgStream* stream);
    void Serialize(MgStream* stream);
    MgByteReader* GetLOB(CREFSTRING propertyName);
    MgByteReader* GetLOB(INT32 index);
    MgBatchPropertyCollection* GetRows(INT32 count = 0); // If zero means feature all records
    void AddRows(INT32 count);
    void AddRow(MgPropertyDefinitionCollection* propDefCol);
    MgPropertyDefinitionCollection* GetColumnDefinitions();

private:

    Ptr<MgServerFeatureConnection> m_connection;
    FdoISQLDataReader* m_sqlReader;
    STRING m_providerName;
    Ptr<MgBatchPropertyCollection>       m_bpCol;
    Ptr<MgPropertyDefinitionCollection>  m_propDefCol;
    void AddCurrentRow();

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_SqlDataReader;

};

#endif
