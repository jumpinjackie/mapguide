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

#ifndef DESKTOP_JOIN_FEATURE_READER_H
#define DESKTOP_JOIN_FEATURE_READER_H

#include "Fdo.h"

/////////////////////////////////////////////////////////////////
///<summary>
/// This class wraps an FdoIFeatureReader object in order to
/// add custom behavior
///</summary>

class MgdJoinFeatureReader : public FdoIFeatureReader
{
private:

    // Unimplemented Constructors/Methods

    MgdJoinFeatureReader();
    MgdJoinFeatureReader(const MgdJoinFeatureReader&);
    MgdJoinFeatureReader& operator=(const MgdJoinFeatureReader&);

public:

    MgdJoinFeatureReader(MgdGwsFeatureReader* reader);
    virtual ~MgdJoinFeatureReader();

    /// \brief
    /// Gets the definition of the object currently being read. If the user
    /// has requested only a subset of the class properties, the class
    /// definition reflects what the user has asked, rather than the full class
    /// definition.
    ///
    /// \return
    /// Returns the class definition object.
    ///
    virtual FdoClassDefinition* GetClassDefinition();

    /// \brief
    /// Gets a value indicating the depth of nesting for the current reader.
    /// The depth value increases each time GetFeatureObject is called and a new
    /// reader is returned. The outermost reader has a depth of 0.
    ///
    /// \return
    /// Returns the depth
    ///
    virtual FdoInt32 GetDepth();

    /// \brief
    /// Gets the name of the property at the given ordinal position.
    ///
    /// \param index
    /// Input the position of the property.
    ///
    /// \return
    /// Returns the property name
    ///
    virtual FdoString* GetPropertyName(FdoInt32 index);

    /// \brief
    /// Gets the index of the property with the specified name.
    ///
    /// \param propertyName
    /// Input the property name.
    ///
    /// \return
    /// Returns the property index
    ///
    virtual FdoInt32 GetPropertyIndex(FdoString* propertyName);

    /// \brief
    /// Gets the geometry value of the specified property as a byte array in
    /// FGF format. Because no conversion is performed, the property must be
    /// of Geometric type; otherwise, an exception is thrown.
    /// This method is a language-specific performance optimization that returns a
    /// pointer to the array data, rather than to an object that encapsulates
    /// the array.  The array's memory area is only guaranteed to be valid
    /// until a call to ReadNext() or Close(), or the disposal of this reader
    /// object.
    ///
    /// \param propertyName
    /// Input the property name.
    /// \param count
    /// Output the number of bytes in the array.
    ///
    /// \return
    /// Returns a pointer to the byte array in FGF format.
    ///
    virtual const FdoByte * GetGeometry(FdoString* propertyName, FdoInt32 * count);

    /// \brief
    /// Gets the geometry value of the specified property as a byte array in
    /// FGF format. Because no conversion is performed, the property must be
    /// of Geometric type; otherwise, an exception is thrown.
    ///
    /// \param propertyName
    /// Input the property name.
    ///
    /// \return
    /// Returns the byte array in FGF format.
    ///
    virtual FdoByteArray* GetGeometry(FdoString* propertyName);

    /// \brief
    /// Gets a reference to an FdoIFeatureReader to read the data contained in
    /// the object or object collection property. If the property is not an
    /// object property, an exception is thrown.
    ///
    /// \param propertyName
    /// Input the property name.
    ///
    /// \return
    /// Returns the nested feature reader
    ///
    virtual FdoIFeatureReader* GetFeatureObject(FdoString* propertyName);

    /// \brief
    /// Gets the Boolean value of the specified property. No conversion is
    /// performed, thus the property must be FdoDataType_Boolean or an
    /// exception is thrown.
    ///
    /// \param propertyName
    /// Input the property name.
    ///
    /// \return
    /// Returns the Boolean value.
    ///
    virtual bool GetBoolean(FdoString* propertyName);

    /// \brief
    /// Gets the byte value of the specified property. No conversion is
    /// performed, thus the property must be FdoDataType_Byte or an
    /// exception is thrown.
    ///
    /// \param propertyName
    /// Input the property name.
    ///
    /// \return
    /// Returns the byte value.
    ///
    virtual FdoByte GetByte(FdoString* propertyName);

    /// \brief
    ///  Gets the date and time value of the specified property. No conversion is
    /// performed, thus the property must be FdoDataType_DateTime or an
    /// exception is thrown.
    ///
    /// \param propertyName
    /// Input the property name.
    ///
    /// \return
    /// Returns the date and time value.
    ///
    virtual FdoDateTime GetDateTime(FdoString* propertyName);

    /// \brief
    /// Gets the double-precision floating point value of the specified property. No
    /// conversion is performed, thus the property must be FdoDataType_Double
    /// or an exception is thrown.
    ///
    /// \param propertyName
    /// Input the property name.
    ///
    /// \return
    /// Returns the double floating point value
    ///
    virtual double GetDouble(FdoString* propertyName);

    /// \brief
    /// Gets the 16-bit integer value of the specified property. No conversion is
    /// performed, thus the property must be FdoDataType_Int16 or an exception
    /// is thrown.
    ///
    /// \param propertyName
    /// Input the property name.
    ///
    /// \return
    /// Returns the FdoInt16 value.
    ///
    virtual FdoInt16 GetInt16(FdoString* propertyName);

    /// \brief
    /// Gets the 32-bit integer value of the specified property. No conversion is
    /// performed, thus the property must be FdoDataType_Int32 or an exception
    /// is thrown.
    ///
    /// \param propertyName
    /// Input the property name.
    ///
    /// \return
    /// Returns the FdoInt32 value
    ///
    virtual FdoInt32 GetInt32(FdoString* propertyName);

    /// \brief
    /// Gets the 64-bit integer value of the specified property. No conversion is
    /// performed, thus the property must be FdoDataType_Int64 or an exception
    /// is thrown.
    ///
    /// \param propertyName
    /// Input the property name.
    ///
    /// \return
    /// Returns the FdoInt64 value.
    ///
    virtual FdoInt64 GetInt64(FdoString* propertyName);

    /// \brief
    /// Gets the Single floating point value of the specified property. No
    /// conversion is performed, thus the property must be FdoDataType_Single
    /// or an exception is thrown.
    ///
    /// \param propertyName
    /// Input the property name.
    ///
    /// \return
    /// Returns the single value
    ///
    virtual float GetSingle(FdoString* propertyName);

    /// \brief
    /// Gets the string value of the specified property. No conversion is
    /// performed, thus the property must be FdoDataType_String or an exception
    /// is thrown.
    ///
    /// \param propertyName
    /// Input the property name.
    ///
    /// \return
    /// Returns the string value
    ///
    virtual FdoString* GetString(FdoString* propertyName);

    /// \brief
    /// Gets a LOBValue reference. The LOB is fully read in and data available.
    /// Because no conversion is performed, the property must be FdoDataType_BLOB or
    /// FdoDataType_CLOB etc. (a LOB type)
    ///
    /// \param propertyName
    /// Input the property name.
    ///
    /// \return
    /// Returns the reference to LOBValue
    ///
    virtual FdoLOBValue* GetLOB(FdoString* propertyName);

    /// \brief
    /// Gets a reference of the specified LOB property as a FdoBLOBStreamReader or
    /// FdoCLOBStreamReader etc. to allow reading in blocks of data.
    /// Because no conversion is performed, the property must be FdoDataType_BLOB
    /// or FdoDataType_CLOB etc. (a LOB type)
    /// Cast the FdoIStreamReader to the appropiate LOB Stream Reader.
    ///
    /// \param propertyName
    /// Input the property name.
    ///
    /// \return
    /// Returns a reference to a LOB stream reader
    ///
    virtual FdoIStreamReader* GetLOBStreamReader(const wchar_t* propertyName );

    /// \brief
    /// Returns true if the value of the specified property is null.
    ///
    /// \param propertyName
    /// Input the property name.
    ///
    /// \return
    /// Returns true if the value is null.
    ///
    virtual bool IsNull(FdoString* propertyName);

    /// \brief
    /// Gets the raster object of the specified property.
    /// Because no conversion is performed, the property must be
    /// of Raster type; otherwise, an exception is thrown.
    ///
    /// \param propertyName
    /// Input the property name.
    ///
    /// \return
    /// Returns the raster object.
    ///
    virtual FdoIRaster* GetRaster(FdoString* propertyName);

    /// \brief
    /// Gets the geometry value of the property, at the specified index,
    /// as a byte array in FGF format. Because no conversion is performed,
    /// the property must be of Geometric type; otherwise, an exception is thrown.
    /// This method is a language-specific performance optimization that returns a
    /// pointer to the array data, rather than to an object that encapsulates
    /// the array.  The array's memory area is only guaranteed to be valid
    /// until a call to ReadNext() or Close(), or the disposal of this reader
    /// object.
    ///
    /// \param index
    /// Input the index of the property.
    /// \param count
    /// Output the number of bytes in the array.
    ///
    /// \return
    /// Returns a pointer to the byte array in FGF format.
    ///
    virtual const FdoByte * GetGeometry(FdoInt32 index, FdoInt32* count);

    /// \brief
    /// Gets the geometry value of the specified property, at the specified index,
    /// as a byte array in FGF format. Because no conversion is performed, the property
    /// must be of Geometric type; otherwise, an exception is thrown.
    ///
    /// \param index
    /// Input the index of the property.
    ///
    /// \return
    /// Returns the byte array in FGF format.
    ///
    virtual FdoByteArray* GetGeometry(FdoInt32 index);

    /// \brief
    /// Gets a reference to an FdoIFeatureReader to read the data contained in
    /// the object or object collection property defined at the specified index
    /// position. If the property is not an object property, an exception is thrown.
    ///
    /// \param index
    /// Input the index of the property.
    ///
    /// \return
    /// Returns the nested feature reader
    ///
    virtual FdoIFeatureReader* GetFeatureObject(FdoInt32 index);

    /// \brief
    /// Gets the Boolean value of the property specified at the index position.
    /// No conversion is performed, thus the property must be FdoDataType_Boolean
    /// or an exception is thrown.
    ///
    /// \param index
    /// Input the index of the property.
    ///
    /// \return
    /// Returns the Boolean value.
    ///
    virtual FdoBoolean GetBoolean(FdoInt32 index);

    /// \brief
    /// Gets the Byte value of the property specified at the index position.
    /// No conversion is performed, thus the property must be FdoDataType_Byte
    /// or an exception is thrown.
    ///
    /// \param index
    /// Input the index of the property.
    ///
    /// \return
    /// Returns the byte value.
    ///
    virtual FdoByte GetByte(FdoInt32 index);

    /// \brief
    /// Gets the date and time value of the of the property specified at
    /// the index position. No conversion is performed, thus the property
    /// must be FdoDataType_DateTime or an exception is thrown.
    ///
    /// \param index
    /// Input the index of the property.
    ///
    /// \return
    /// Returns the date and time value.
    ///
    virtual FdoDateTime GetDateTime(FdoInt32 index);

    /// \brief
    /// Gets the double-precision floating point value of the property specified at
    /// the index position. No conversion is performed, thus the property must be
    /// FdoDataType_Double or an exception is thrown.
    ///
    /// \param index
    /// Input the index of the property.
    ///
    /// \return
    /// Returns the double floating point value
    ///
    virtual FdoDouble GetDouble(FdoInt32 index);

    /// \brief
    /// Gets the 16-bit integer value of the property specified at
    /// the index position. No conversion is performed, thus the
    /// property must be FdoDataType_Int16 or an exception is thrown.
    ///
    /// \param index
    /// Input the index of the property.
    ///
    /// \return
    /// Returns the FdoInt16 value.
    ///
    virtual FdoInt16 GetInt16(FdoInt32 index);

    /// \brief
    /// Gets the 32-bit integer value of the property specified at
    /// the index position. No conversion is performed, thus the
    /// property must be FdoDataType_Int32 or an exception is thrown.
    ///
    /// \param index
    /// Input the index of the property.
    ///
    /// \return
    /// Returns the FdoInt32 value
    ///
    virtual FdoInt32 GetInt32(FdoInt32 index);

    /// \brief
    /// Gets the 64-bit integer value of the property specified at
    /// the index position. No conversion is performed, thus the
    /// property must be FdoDataType_Int64 or an exception is thrown.
    ///
    /// \param index
    /// Input the index of the property.
    ///
    /// \return
    /// Returns the FdoInt64 value.
    ///
    virtual FdoInt64 GetInt64(FdoInt32 index);

    /// \brief
    /// Gets the Single floating point value of the property specified at
    /// the index position. No conversion is performed, thus the property
    /// must be FdoDataType_Single or an exception is thrown.
    ///
    /// \param index
    /// Input the index of the property.
    ///
    /// \return
    /// Returns the single value
    ///
    virtual FdoFloat GetSingle(FdoInt32 index);

    /// \brief
    /// Gets the string value of the property specified at the index
    /// position. No conversion is performed, thus the property must
    /// be FdoDataType_String or an exception is thrown.
    ///
    /// \param index
    /// Input the index of the property.
    ///
    /// \return
    /// Returns the string value
    ///
    virtual FdoString* GetString(FdoInt32 index);

    /// \brief
    /// Gets a LOBValue pointer to the property specified at the index
    /// position. The LOB is fully read in and data available.
    /// Because no conversion is performed, the property must be
    /// FdoDataType_BLOB or FdoDataType_CLOB etc. (a LOB type)
    ///
    /// \param index
    /// Input the index of the property.
    ///
    /// \return
    /// Returns the pointer to LOBValue
    ///
    virtual FdoLOBValue* GetLOB(FdoInt32 index);

    /// \brief
    /// Gets a pointer to the specified LOB property, specified at the index
    /// position. The reference is returned as an FdoBLOBStreamReader or an
    /// FdoCLOBStreamReader, to allow reading in blocks of data. Because
    /// no conversion is performed, the property must be FdoDataType_BLOB
    /// or FdoDataType_CLOB etc. (a LOB type) Cast the FdoIStreamReader
    /// to the appropriate LOB Stream Reader.
    ///
    /// \param index
    /// Input the index of the property.
    ///
    /// \return
    /// Returns a pointer to a LOB stream reader
    ///
    virtual FdoIStreamReader* GetLOBStreamReader(FdoInt32 index);

    /// \brief
    /// Returns true if the value of the property at the specified
    /// index is null.
    ///
    /// \param index
    /// Input the index of the property.
    ///
    /// \return
    /// Returns true if the value is null.
    ///
    virtual FdoBoolean IsNull(FdoInt32 index);

    /// \brief
    /// Gets the raster object of the property at the specified index.
    /// Because no conversion is performed, the property must be
    /// of Raster type; otherwise, an exception is thrown.
    ///
    /// \param index
    /// Input the index of the property.
    ///
    /// \return
    /// Returns the raster object.
    ///
    virtual FdoIRaster* GetRaster(FdoInt32 index);

    /// \brief
    /// Advances the reader to the next item and returns true if there is
    /// another object to read or false if reading is complete. The default
    /// position of the reader is prior to the first item. Thus you must
    /// call ReadNext to begin accessing any data.
    ///
    /// \return
    /// Returns true if there is a next item.
    ///
    virtual bool ReadNext();

    /// \brief
    /// Closes the FdoIFeatureReader object, freeing any resources it may be holding.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual void Close();

    /// \brief
    /// Dispose this object.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual void Dispose();

private:
    void InitializePropertyNames();
    void FillProperties(FdoClassDefinition* classDef);

private:
    FdoClassDefinition* GetFdoClassDefinition(MgClassDefinition* mgClassDef);
    void GetClassProperties(FdoPropertyDefinitionCollection* fdoPropDefCol, MgPropertyDefinitionCollection* propDefCol);
    void GetClassProperties(FdoDataPropertyDefinitionCollection* fdoPropDefCol, MgPropertyDefinitionCollection* mgPropDefCol);
    FdoPropertyDefinition* GetFdoPropertyDefinition(MgPropertyDefinition* mgPropDef);

    FdoDataPropertyDefinition* GetDataPropertyDefinition(MgDataPropertyDefinition* mgPropDef);
    FdoObjectPropertyDefinition* GetObjectPropertyDefinition(MgObjectPropertyDefinition* objPropDef);
    FdoGeometricPropertyDefinition* GetGeometricPropertyDefinition(MgGeometricPropertyDefinition* mgPropDef);
    FdoRasterPropertyDefinition* GetRasterPropertyDefinition(MgRasterPropertyDefinition* mgPropDef);
    FdoDataType GetFdoDataType(INT32 awPropType);
    FdoObjectType MgObjectPropertyTypeToFdoObjectType(INT32 type);
    FdoOrderType MgOrderingOptionToFdoOrderType(INT32 type);

    Ptr<MgdGwsFeatureReader> m_reader;

    // Used for access by index readers
    FdoStringsP m_propertyNames;
    bool m_propertyNamesInitialized;
};
#endif
