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

#ifndef _MG_SERVER_DATA_READER_H_
#define _MG_SERVER_DATA_READER_H_

class MgStream;
class MgByteReader;
class FdoIDataReader;
class MgDateTime;
class MgDataReader;

#include "ServerFeatureDllExport.h"

/////////////////////////////////////////////////////////////////
/// <summary>
/// The SqlReader interface provides a forward-only, read-only
/// iterator for reading relational table data.
/// The initial position of the SqlReader interface is prior to the first item.
/// Thus, you must call ReadNext to begin accessing any data..
/// </summary>

class MG_SERVER_FEATURE_API  MgServerDataReader : public MgDataReader
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
    /// Gets the number of Propertys in the result set.
    /// </summary>
    /// <returns>Returns the number of Propertys.</returns>
    INT32 GetPropertyCount();

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the name of the Property at the given ordinal position.
    /// </summary>
    /// <param name="index">Input the position of the Property.</param>
    /// <returns>Returns the Property name</returns>
    STRING GetPropertyName(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the index of the property with the specified name.
    ///
    /// \param propertyName
    /// Input the name of the property.
    ///
    /// \return
    /// Returns the property index
    ///
    INT32 GetPropertyIndex(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the data type of the Property with the specified name.
    /// </summary>
    /// <param name="propertyName">Input the Property name.</param>
    /// <returns>Returns the type of the Property.</returns>
    INT32 GetPropertyType(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// Gets the data type of the Property at the specified index.
    /// </summary>
    /// <param name="index">Input the Property index.</param>
    /// <returns>Returns the type of the Property.</returns>
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

    //////////////////////////////////////////////////////////////////
    /// <summary>
    /// </summary>
    /// <param name="propertyName">Input the property name.</param>
    /// <returns>Returns the Raster object.</returns>
    /// EXCEPTIONS:
    /// MgInvalidArgumentException if the property type is not geometry
    MgRaster* GetRaster(CREFSTRING propertyName);


    //////////////////////////////////////////////////////////////////
    /// \brief
    ///  Returns true if the value of the specified property is null.
    ///
    /// \param index
    /// Property index.
    ///
    /// \return
    /// Returns true if the value is null.
    ///
    bool IsNull(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Boolean value of the specified property. No conversion is
    /// performed, thus the property must be a of boolean type the result
    /// is undetermined
    ///
    /// \param index
    /// Property index.
    ///
    /// \return
    /// Returns the Boolean value.
    ///
    bool GetBoolean(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Byte value of the specified property. No conversion is
    /// performed, thus the property must be a of byte type or the result
    /// is undetermined
    ///
    /// \param index
    /// Property index.
    ///
    /// \return
    /// Returns the Byte value.
    ///
    BYTE GetByte(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the DTime value of the specified property. No conversion is
    /// performed, thus the property must be a of date type or the result
    /// is NULL
    ///
    /// \param index
    /// Property index.
    ///
    /// \return
    /// Returns the DTime value.
    ///
    MgDateTime* GetDateTime(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Single value of the specified property. No conversion is
    /// performed, thus the property must be a of type single or the result
    /// is undetermined
    ///
    /// \param index
    /// Property index.
    ///
    /// \return
    /// Returns the single value.
    ///
    float GetSingle(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Double value of the specified property. No conversion is
    /// performed, thus the property must be a of type double or the result
    /// is undetermined
    ///
    /// \param index
    /// Property index.
    ///
    /// \return
    /// Returns the double value.
    ///
    double GetDouble(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the integer 16 bits value of the specified property. No conversion is
    /// performed, thus the property must be a of type integer 16 bits or the result
    /// is undetermined
    ///
    /// \param index
    /// Property index.
    ///
    /// \return
    /// Returns the integer 16 bits value.
    ///
    INT16 GetInt16(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the integer 32 bits value of the specified property. No conversion is
    /// performed, thus the property must be a of type integer 32 bits or the result
    /// is undetermined
    ///
    /// \param index
    /// Property index.
    ///
    /// \return
    /// Returns the integer 32 bits value.
    ///
    INT32 GetInt32(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the integer 64 bits value of the specified property. No conversion is
    /// performed, thus the property must be a of type integer 64 bits or the result
    /// is NULL
    ///
    /// \param index
    /// Property index.
    ///
    /// \return
    /// Returns the integer 64 bits value.
    /// Note: INT64 is actually a pointer to an Integer64 object
    ///
    INT64 GetInt64(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the string value of the specified property. No conversion is
    /// performed, thus the property must be a of type string or the result
    /// is NULL
    ///
    /// \param index
    /// Property index.
    ///
    /// \return
    /// Returns the string value.
    ///
    STRING GetString(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the BLOB value of the specified property. No conversion is
    /// performed, thus the property must be a of type BLOBs or the result
    /// is NULL
    ///
    /// \param index
    /// Property index.
    ///
    /// \return
    /// Returns the BLOB value.
    ///
    MgByteReader* GetBLOB(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the CLOB value of the specified property. No conversion is
    /// performed, thus the property must be a of type CLOB or the result
    /// is NULL
    ///
    /// \param index
    /// Property index.
    ///
    /// \return
    /// Returns the CLOB value.
    ///
    MgByteReader* GetCLOB(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Geometry for the specified property. No conversion is
    /// performed, thus the property must be a of type Geometry or the result
    /// is NULL
    ///
    /// \param index
    /// Property index.
    ///
    /// \return
    /// Returns a ByteReader object
    ///
    MgByteReader* GetGeometry(INT32 index);

    /// \brief
    /// Gets the raster object of the specified property.
    /// the property must be of Raster type; otherwise, an exception is thrown.
    ///
    /// \param index
    /// Property index.
    ///
    /// \return
    /// Returns the raster object.
    ///
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

    MgServerDataReader(MgServerFeatureConnection* connection, FdoIDataReader* dataReader, CREFSTRING providerName);
    MgServerDataReader();
    ~MgServerDataReader();
    void Deserialize(MgStream* stream);
    void Serialize(MgStream* stream);
    MgByteReader* GetLOB(CREFSTRING propertyName);
    MgByteReader* GetLOB(INT32 index);
    STRING GetRasterPropertyName();
    MgByteReader* GetRaster(INT32 xSize, INT32 ySize, STRING rasterPropName);
    MgBatchPropertyCollection* GetRows(INT32 count = 0); // If zero means feature all records
    MgPropertyDefinitionCollection* GetColumnDefinitions();
    void AddRows(INT32 count);
    void AddRow(MgPropertyDefinitionCollection* propDefCol);

private:

    Ptr<MgServerFeatureConnection> m_connection;
    FdoIDataReader* m_dataReader;
    STRING m_providerName;
    bool m_removeFromPoolOnDestruction;
    Ptr<MgBatchPropertyCollection>       m_bpCol;
    Ptr<MgPropertyDefinitionCollection>  m_propDefCol;

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_DataReader;

};

#endif
