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

#ifndef _MG_PROXY_DATA_READER_H
#define _MG_PROXY_DATA_READER_H

#include "MgDesktop.h"

class MgDataReader;

class MgdProxyDataReader;
template class MG_DESKTOP_API Ptr<MgdProxyDataReader>;

/// \cond INTERNAL
/////////////////////////////////////////////////////////////////
/// \brief
/// The SqlReader interface provides a forward-only, read-only
/// iterator for reading relational table data.
///
/// \remarks
/// The initial position of the SqlReader interface is prior to the first item.
/// Thus, you must call ReadNext to begin accessing any data.
class MG_DESKTOP_API  MgdProxyDataReader : public MgDataReader
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgdProxyDataReader)

EXTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Advances the reader to the next item and returns true if there is
    /// another object to read or false if reading is complete. The default
    /// position of the reader is prior to the first item. Thus you must
    /// call ReadNext to begin accessing any data.
    ///
    /// \return
    /// Returns true if there is a next item.
    ///
    bool ReadNext();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of properties in the result set.
    ///
    /// \return
    /// Returns the number of properties.
    ///
    INT32 GetPropertyCount();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name of the property at the given ordinal position.
    ///
    /// \param index
    /// Input the position of the property.
    ///
    /// \return
    /// Returns the property name
    ///
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
    /// \brief
    /// Gets the data type of the property with the specified name.
    ///
    /// \param propertyName
    /// Input the property name.
    ///
    /// \return
    /// Returns the type of the property.
    ///
    INT32 GetPropertyType(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the data type of the property at the specified index.
    ///
    /// \param index
    /// Input the property index.
    ///
    /// \return
    /// Returns the type of the property.
    ///
    INT32 GetPropertyType(INT32 index);

    //////////////////////////////////////////////////////////////////
    /// \brief
    ///  Returns true if the value of the specified property is null.
    ///
    /// \param propertyName
    /// Property name.
    ///
    /// \return
    /// Returns true if the value is null.
    ///
    bool IsNull(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Boolean value of the specified property. No conversion is
    /// performed, thus the property must be a of boolean type the result
    /// is undetermined
    ///
    /// \param propertyName
    /// Property name.
    ///
    /// \return
    /// Returns the Boolean value.
    ///
    bool GetBoolean(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Byte value of the specified property. No conversion is
    /// performed, thus the property must be a of byte type or the result
    /// is undetermined
    ///
    /// \param propertyName
    /// Property name.
    ///
    /// \return
    /// Returns the Byte value.
    ///
    BYTE GetByte(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the DTime value of the specified property. No conversion is
    /// performed, thus the property must be a of date type or the result
    /// is NULL
    ///
    /// \param propertyName
    /// Property name.
    ///
    /// \return
    /// Returns the DTime value.
    ///
    MgDateTime* GetDateTime(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Single value of the specified property. No conversion is
    /// performed, thus the property must be a of type single or the result
    /// is undetermined
    ///
    /// \param propertyName
    /// Property name.
    ///
    /// \return
    /// Returns the single value.
    ///
    float GetSingle(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Double value of the specified property. No conversion is
    /// performed, thus the property must be a of type double or the result
    /// is undetermined
    ///
    /// \param propertyName
    /// Property name.
    ///
    /// \return
    /// Returns the double value.
    ///
    double GetDouble(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the integer 16 bits value of the specified property. No conversion is
    /// performed, thus the property must be a of type integer 16 bits or the result
    /// is undetermined
    ///
    /// \param propertyName
    /// Property name.
    ///
    /// \return
    /// Returns the integer 16 bits value.
    ///
    INT16 GetInt16(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the integer 32 bits value of the specified property. No conversion is
    /// performed, thus the property must be a of type integer 32 bits or the result
    /// is undetermined
    ///
    /// \param propertyName
    /// Property name.
    ///
    /// \return
    /// Returns the integer 32 bits value.
    ///
    INT32 GetInt32(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the integer 64 bits value of the specified property. No conversion is
    /// performed, thus the property must be a of type integer 64 bits or the result
    /// is NULL
    ///
    /// \param propertyName
    /// Property name.
    ///
    /// \return
    /// Returns the integer 64 bits value.
    /// Note: INT64 is actually a pointer to an Integer64 object
    ///
    INT64 GetInt64(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the string value of the specified property. No conversion is
    /// performed, thus the property must be a of type string or the result
    /// is NULL
    ///
    /// \param propertyName
    /// Property name.
    ///
    /// \return
    /// Returns the string value.
    ///
    STRING GetString(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the BLOB value of the specified property. No conversion is
    /// performed, thus the property must be a of type BLOBs or the result
    /// is NULL
    ///
    /// \param propertyName
    /// Property name.
    ///
    /// \return
    /// Returns the BLOB value.
    ///
    MgByteReader* GetBLOB(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the CLOB value of the specified property. No conversion is
    /// performed, thus the property must be a of type CLOB or the result
    /// is NULL
    ///
    /// \param propertyName
    /// Property name.
    ///
    /// \return
    /// Returns the CLOB value.
    ///
    MgByteReader* GetCLOB(CREFSTRING propertyName);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Geometry for the specified property. No conversion is
    /// performed, thus the property must be a of type Geometry or the result
    /// is NULL
    ///
    /// \param propertyName
    /// Property name.
    ///
    /// \return
    /// Returns a ByteReader object
    ///
    MgByteReader* GetGeometry(CREFSTRING propertyName);

    /// \brief
    /// Gets the raster object of the specified property.
    /// the property must be of Raster type; otherwise, an exception is thrown.
    ///
    /// \param propertyName
    /// Input the property name.
    ///
    /// \return
    /// Returns the raster object.
    ///
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
    /// \brief
    /// Closes the FeatureReader object, freeing any resources it may be holding.
    ///
    void Close();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serializes all features into an XML.
    /// XML is serialized from the current position of feature reader in the order
    /// data are retrieved.
    ///
    /// \return
    /// MgByteReader holding XML.
    ///
    MgByteReader* ToXml();

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
    /// \brief
    /// Gets the string value of the specified property. No conversion is
    /// performed, thus the property must be a of type string or the result
    /// is NULL
    ///
    /// \param propertyName
    /// Property name.
    ///
    /// \return
    /// Returns the string value.
    ///
    const wchar_t* GetString(CREFSTRING propertyName, INT32& length);

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
    const wchar_t* GetString(INT32 index, INT32& length);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serialize data to TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserialize data from TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

    void ToXml(string &str);

    void SetService(MgFeatureService* service);

    virtual INT32 GetClassId()
    {
        return m_cls_id;
    }

    virtual void Dispose()
    {
        delete this;
    }

    MgdProxyDataReader(MgBatchPropertyCollection* batchCol, MgPropertyDefinitionCollection* propDefCol);

    MgdProxyDataReader();
    ~MgdProxyDataReader();

private:

    MgProperty* GetProperty(CREFSTRING propertyName, INT16 expectedType);
    /// Get the property for the specified name
    MgProperty* GetProperty(CREFSTRING propertyName);

    MgProperty* GetProperty(INT32 index, INT16 expectedType);
    /// Get the property for the specified index.
    MgProperty* GetProperty(INT32 index);

    void UpdateCurrentSet(MgBatchPropertyCollection* bpCol);

    Ptr<MgBatchPropertyCollection> m_set;
    Ptr<MgPropertyDefinitionCollection> m_propDefCol;

    STRING m_providerName;
    INT32 m_currRecord;
    STRING m_serverDataReader;
    MgFeatureService* m_service; // Do not want to implement a default constructor

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_DataReader;

};
/// \endcond

#endif
