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

#ifndef _MG_READER_H
#define _MG_READER_H

class MgRaster;

/// \defgroup MgReader MgReader
/// \ingroup Feature_Service_classes
/// \{

/////////////////////////////////////////////////////////////////
/// \brief
/// Provides a forward-only, read-only iterator for reading data.
/// You must call ReadNext before you can access any data. This
/// is a base class for MgDataReader, MgFeatureReader, and
/// MgSqlDataReader.
///

class MG_PLATFORMBASE_API  MgReader : public MgSerializable
{
PUBLISHED_API:
    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Advances the reader to the next item and determines whether
    /// there is another object to read.
    ///
    /// \remarks
    /// The initial position of the reader is prior to the first
    /// item. Therefore, you must call ReadNext before you can access
    /// any data.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool ReadNext();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean ReadNext();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool ReadNext();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns true if there is a next item; otherwise returns
    /// false.
    ///
    virtual bool ReadNext() = 0;

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of properties in the feature currently being
    /// read.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetPropertyCount();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetPropertyCount();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetPropertyCount();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the number of properties.
    ///
    virtual INT32 GetPropertyCount() = 0;

    /////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name of the property at the given ordinal position.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual string GetPropertyName(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual String GetPropertyName(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual string GetPropertyName(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// The position of the property in the list of
    /// properties belonging to the feature currently
    /// being read.
    ///
    /// \return
    /// Returns the property name.
    ///
    virtual STRING GetPropertyName(INT32 index) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the type of the property with the specified name. See
    /// MgPropertyType for the list of possible values.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetPropertyType(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetPropertyType(String propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetPropertyType(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param propertyName (String/string)
    /// The property name.
    ///
    /// \return
    /// Returns an MgPropertyType value.
    ///
    virtual INT32 GetPropertyType(CREFSTRING propertyName) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines whether the value of the specified property is
    /// null.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool IsNull(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean IsNull(String propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool IsNull(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param propertyName (String/string)
    /// The property name.
    ///
    /// \return
    /// Returns true if the value is null; otherwise returns false.
    ///
    virtual bool IsNull(CREFSTRING propertyName) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Boolean value of the specified property.
    ///
    /// \remarks
    /// No conversion is performed. Therefore, the property must be of
    /// MgPropertyType::Boolean type or an
    /// MgInvalidPropertyTypeException is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool GetBoolean(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean GetBoolean(String propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool GetBoolean(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param propertyName (String/string)
    /// The property name.
    ///
    /// \return
    /// Returns the Boolean value.
    ///
    /// \exception MgInvalidPropertyTypeException.
    ///
    ///
    virtual bool GetBoolean(CREFSTRING propertyName) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Byte value of the specified property.
    ///
    /// \remarks
    /// No conversion is performed. Therefore, the property must be
    /// of type MgPropertyType::Byte or an
    /// MgInvalidPropertyTypeException is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual unsigned char GetByte(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual signed char GetByte(String propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetByte(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param propertyName (String/string)
    /// The property name.
    ///
    /// \return
    /// Returns the Byte value.
    ///
    /// \exception MgInvalidPropertyTypeException.
    ///
    ///
    virtual BYTE GetByte(CREFSTRING propertyName) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the MgDateTime value of the specified property.
    ///
    /// \remarks
    /// No conversion is performed. Therefore, the property must be
    /// of type MgPropertyType::DateTime or an
    /// MgInvalidPropertyTypeException is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgDateTime GetDateTime(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgDateTime GetDateTime(String propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgDateTime GetDateTime(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param propertyName (String/string)
    /// The property name.
    ///
    /// \return
    /// Returns an MgDateTime object.
    ///
    /// \exception MgInvalidPropertyTypeException.
    ///
    ///
    virtual MgDateTime* GetDateTime(CREFSTRING propertyName) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Single value of the specified property.
    ///
    /// \remarks
    /// No conversion is performed. Therefore, the property must be a
    /// of type MgPropertyType::Single or an
    /// MgInvalidPropertyTypeException is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual float GetSingle(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual float GetSingle(String propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual float GetSingle(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param propertyName (String/string)
    /// The property name.
    ///
    /// \return
    /// Returns the single value.
    ///
    /// \exception MgInvalidPropertyTypeException.
    ///
    ///
    virtual float GetSingle(CREFSTRING propertyName) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Double value of the specified property.
    ///
    /// \remarks
    /// No conversion is performed. Therefore, the property must be a
    /// of type MgPropertyType::Double or an
    /// MgInvalidPropertyTypeException is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual double GetDouble(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual double GetDouble(String propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual double GetDouble(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param propertyName (String/string)
    /// The property name.
    ///
    /// \return
    /// Returns the double value.
    ///
    /// \exception MgInvalidPropertyTypeException.
    ///
    ///
    virtual double GetDouble(CREFSTRING propertyName) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the 16 bit integer value of the specified property.
    ///
    /// \remarks
    /// No conversion is performed. Therefore the property must be a
    /// of type MgPropertyType::Int16 or an
    /// MgInvalidPropertyTypeException is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual short GetInt16(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual short GetInt16(String propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetInt16(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param propertyName (String/string)
    /// The property name.
    ///
    /// \return
    /// Returns the 16 bit integer value.
    ///
    /// \exception MgInvalidPropertyTypeException
    ///
    ///
    virtual INT16 GetInt16(CREFSTRING propertyName) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the 32 bit integer value of the specified property.
    ///
    /// \remarks
    /// No conversion is performed. Therefore, the property must be a
    /// of type MgPropertyType::Int32 or an
    /// MgInvalidPropertyTypeException is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetInt32(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetInt32(String propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetInt32(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param propertyName (String/string)
    /// The property name.
    ///
    /// \return
    /// Returns the integer 32 bits value.
    ///
    /// \exception MgInvalidPropertyTypeException.
    ///
    ///
    virtual INT32 GetInt32(CREFSTRING propertyName) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the 64 bit integer value of the specified property.
    ///
    /// \remarks
    /// No conversion is performed. Therefore, the property must be a
    /// of type MgPropertyType::Int64 or an
    /// MgInvalidPropertyTypeException is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual long GetInt64(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual long GetInt64(String propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual string GetInt64(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param propertyName (String/string)
    /// The property name.
    ///
    /// \return
    /// Returns the integer 64 bits value.
    ///
    /// \exception MgInvalidPropertyTypeException.
    ///
    /// \note
    /// INT64 is actually a pointer to an Integer64 object
    ///
    virtual INT64 GetInt64(CREFSTRING propertyName) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the string value of the specified property.
    ///
    /// \remarks
    /// No conversion is performed. Therefore, the property must be a
    /// of type MgPropertyType::String or an
    /// MgInvalidPropertyTypeException is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual string GetString(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual String GetString(String propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual string GetString(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param propertyName (String/string)
    /// The property name.
    ///
    /// \return
    /// Returns the string value.
    ///
    /// \exception MgInvalidPropertyTypeException.
    ///
    ///
    virtual STRING GetString(CREFSTRING propertyName) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the \link BLOB BLOB \endlink value of the specified property.
    ///
    /// \remarks
    /// No conversion is performed. Therefore, the property must be
    /// of type MgPropertyType::Blob or an
    /// MgInvalidPropertyTypeException is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader GetBLOB(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader GetBLOB(String propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader GetBLOB(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param propertyName (String/string)
    /// The property name.
    ///
    /// \return
    /// Returns the BLOB value in an MgByteReader object.
    ///
    /// \exception MgInvalidPropertyTypeException
    ///
    ///
    virtual MgByteReader* GetBLOB(CREFSTRING propertyName) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the CLOB value of the specified property.
    ///
    /// \remarks
    /// No conversion is performed. Therefore, the property must be a
    /// of type MgPropertyType::Clob or an
    /// MgInvalidPropertyTypeException is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader GetCLOB(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader GetCLOB(String propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader GetCLOB(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param propertyName (String/string)
    /// The property name.
    ///
    /// \return
    /// Returns the CLOB value in an MgByteReader object.
    ///
    /// \exception MgInvalidPropertyTypeException.
    ///
    ///
    virtual MgByteReader* GetCLOB(CREFSTRING propertyName) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Geometry for the specified property.
    ///
    /// \remarks
    /// No conversion is performed. Therefore, the property must be a
    /// of type MgPropertyType::Geometry or an
    /// MgInvalidPropertyTypeException is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader GetGeometry(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader GetGeometry(String propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader GetGeometry(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param propertyName (String/string)
    /// The property name.
    ///
    /// \return
    /// Returns a geometry in AGF binary format in an MgByteReader
    /// object.
    ///
    /// \exception MgInvalidPropertyTypeException
    ///
    ///
    virtual MgByteReader* GetGeometry(CREFSTRING propertyName) = 0;

    ///////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the raster object of the specified property.
    ///
    /// \remarks
    /// The property must be of type MgPropertyType::Raster, or an
    /// MgInvalidPropertyTypeException is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgRaster GetRaster(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgRaster GetRaster(String propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgRaster GetRaster(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param propertyName (String/string)
    /// The property name.
    ///
    /// \return
    /// Returns an MgRaster object.
    ///
    /// \exception MgConnectionNotOpenException MgNullPropertyValueException
    /// \exception MgFdoException MgInvalidPropertyTypeException
    /// \exception MgInvalidPropertyTypeException.
    ///
    ///
    virtual MgRaster* GetRaster(CREFSTRING propertyName) = 0;


    /////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the index of the property with the specified property name.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetPropertyIndex(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetPropertyIndex(String propertyName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetPropertyIndex(string propertyName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param propertyName(string)
    /// The name of the property in the list of
    /// properties belonging to the feature currently
    /// being read.
    ///
    /// \return
    /// Returns the property index.
    ///
    virtual INT32 GetPropertyIndex(CREFSTRING propertyName) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines whether the value of the specified property is
    /// null.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool IsNull(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean IsNull(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool IsNull(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// The position of the property in the list of
    /// properties belonging to the feature currently
    /// being read.
    ///
    /// \return
    /// Returns true if the value is null; otherwise returns false.
    ///
    virtual bool IsNull(INT32 index) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Boolean value of the specified property.
    ///
    /// \remarks
    /// No conversion is performed. Therefore, the property must be of
    /// MgPropertyType::Boolean type or an
    /// MgInvalidPropertyTypeException is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual bool GetBoolean(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual boolean GetBoolean(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual bool GetBoolean(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// The position of the property in the list of
    /// properties belonging to the feature currently
    /// being read.
    ///
    /// \return
    /// Returns the Boolean value.
    ///
    /// \exception MgInvalidPropertyTypeException.
    ///
    ///
    virtual bool GetBoolean(INT32 index) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Byte value of the specified property.
    ///
    /// \remarks
    /// No conversion is performed. Therefore, the property must be
    /// of type MgPropertyType::Byte or an
    /// MgInvalidPropertyTypeException is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual unsigned char GetByte(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual signed char GetByte(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetByte(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// The position of the property in the list of
    /// properties belonging to the feature currently
    /// being read.
    ///
    /// \return
    /// Returns the Byte value.
    ///
    /// \exception MgInvalidPropertyTypeException.
    ///
    ///
    virtual BYTE GetByte(INT32 index) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the MgDateTime value of the specified property.
    ///
    /// \remarks
    /// No conversion is performed. Therefore, the property must be
    /// of type MgPropertyType::DateTime or an
    /// MgInvalidPropertyTypeException is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgDateTime GetDateTime(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgDateTime GetDateTime(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgDateTime GetDateTime(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// The position of the property in the list of
    /// properties belonging to the feature currently
    /// being read.
    ///
    /// \return
    /// Returns an MgDateTime object.
    ///
    /// \exception MgInvalidPropertyTypeException.
    ///
    ///
    virtual MgDateTime* GetDateTime(INT32 index) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Single value of the specified property.
    ///
    /// \remarks
    /// No conversion is performed. Therefore, the property must be a
    /// of type MgPropertyType::Single or an
    /// MgInvalidPropertyTypeException is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual float GetSingle(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual float GetSingle(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual float GetSingle(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// The position of the property in the list of
    /// properties belonging to the feature currently
    /// being read.
    ///
    /// \return
    /// Returns the single value.
    ///
    /// \exception MgInvalidPropertyTypeException.
    ///
    ///
    virtual float GetSingle(INT32 index) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Double value of the specified property.
    ///
    /// \remarks
    /// No conversion is performed. Therefore, the property must be a
    /// of type MgPropertyType::Double or an
    /// MgInvalidPropertyTypeException is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual double GetDouble(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual double GetDouble(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual double GetDouble(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// The position of the property in the list of
    /// properties belonging to the feature currently
    /// being read.
    ///
    /// \return
    /// Returns the double value.
    ///
    /// \exception MgInvalidPropertyTypeException.
    ///
    ///
    virtual double GetDouble(INT32 index) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the 16 bit integer value of the specified property.
    ///
    /// \remarks
    /// No conversion is performed. Therefore the property must be a
    /// of type MgPropertyType::Int16 or an
    /// MgInvalidPropertyTypeException is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual short GetInt16(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual short GetInt16(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetInt16(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// The position of the property in the list of
    /// properties belonging to the feature currently
    /// being read.
    ///
    /// \return
    /// Returns the 16 bit integer value.
    ///
    /// \exception MgInvalidPropertyTypeException
    ///
    ///
    virtual INT16 GetInt16(INT32 index) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the 32 bit integer value of the specified property.
    ///
    /// \remarks
    /// No conversion is performed. Therefore, the property must be a
    /// of type MgPropertyType::Int32 or an
    /// MgInvalidPropertyTypeException is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetInt32(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetInt32(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetInt32(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// The position of the property in the list of
    /// properties belonging to the feature currently
    /// being read.
    ///
    /// \return
    /// Returns the integer 32 bits value.
    ///
    /// \exception MgInvalidPropertyTypeException.
    ///
    ///
    virtual INT32 GetInt32(INT32 index) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the 64 bit integer value of the specified property.
    ///
    /// \remarks
    /// No conversion is performed. Therefore, the property must be a
    /// of type MgPropertyType::Int64 or an
    /// MgInvalidPropertyTypeException is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  virtual long GetInt64(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  virtual long GetInt64(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  virtual string GetInt64(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// The position of the property in the list of
    /// properties belonging to the feature currently
    /// being read.
    ///
    /// \return
    /// Returns the integer 64 bits value.
    ///
    /// \exception MgInvalidPropertyTypeException.
    ///
    /// \note
    /// INT64 is actually a pointer to an Integer64 object
    ///
    virtual INT64 GetInt64(INT32 index) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the string value of the specified property.
    ///
    /// \remarks
    /// No conversion is performed. Therefore, the property must be a
    /// of type MgPropertyType::String or an
    /// MgInvalidPropertyTypeException is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual string GetString(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual String GetString(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual string GetString(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// The position of the property in the list of
    /// properties belonging to the feature currently
    /// being read.
    ///
    /// \return
    /// Returns the string value.
    ///
    /// \exception MgInvalidPropertyTypeException.
    ///
    ///
    virtual STRING GetString(INT32 index) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the \link BLOB BLOB \endlink value of the specified property.
    ///
    /// \remarks
    /// No conversion is performed. Therefore, the property must be
    /// of type MgPropertyType::Blob or an
    /// MgInvalidPropertyTypeException is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader GetBLOB(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader GetBLOB(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader GetBLOB(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// The position of the property in the list of
    /// properties belonging to the feature currently
    /// being read.
    ///
    /// \return
    /// Returns the BLOB value in an MgByteReader object.
    ///
    /// \exception MgInvalidPropertyTypeException
    ///
    ///
    virtual MgByteReader* GetBLOB(INT32 index) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the CLOB value of the specified property.
    ///
    /// \remarks
    /// No conversion is performed. Therefore, the property must be a
    /// of type MgPropertyType::Clob or an
    /// MgInvalidPropertyTypeException is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader GetCLOB(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader GetCLOB(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader GetCLOB(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// The position of the property in the list of
    /// properties belonging to the feature currently
    /// being read.
    ///
    /// \return
    /// Returns the CLOB value in an MgByteReader object.
    ///
    /// \exception MgInvalidPropertyTypeException.
    ///
    ///
    virtual MgByteReader* GetCLOB(INT32 index) = 0;

    /////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the Geometry for the specified property.
    ///
    /// \remarks
    /// No conversion is performed. Therefore, the property must be a
    /// of type MgPropertyType::Geometry or an
    /// MgInvalidPropertyTypeException is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader GetGeometry(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader GetGeometry(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader GetGeometry(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// The position of the property in the list of
    /// properties belonging to the feature currently
    /// being read.
    ///
    /// \return
    /// Returns a geometry in AGF binary format in an MgByteReader
    /// object.
    ///
    /// \exception MgInvalidPropertyTypeException
    ///
    ///
    virtual MgByteReader* GetGeometry(INT32 index) = 0;

    ///////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the raster object of the specified property.
    ///
    /// \remarks
    /// The property must be of type MgPropertyType::Raster, or an
    /// MgInvalidPropertyTypeException is thrown.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgRaster GetRaster(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgRaster GetRaster(int index);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgRaster GetRaster(int index);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param index (int)
    /// The position of the property in the list of
    /// properties belonging to the feature currently
    /// being read.
    ///
    /// \return
    /// Returns an MgRaster object.
    ///
    /// \exception MgConnectionNotOpenException MgNullPropertyValueException
    /// \exception MgFdoException MgInvalidPropertyTypeException
    /// \exception MgInvalidPropertyTypeException.
    ///
    ///
    virtual MgRaster* GetRaster(INT32 index) = 0;


    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Closes the object, freeing any resources it may be holding.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual void Close();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual void Close();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual void Close();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns nothing.
    ///
    virtual void Close() = 0;

    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Get the type of this reader. See MgReaderType for a list of
    /// the possible values.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetReaderType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetReaderType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetReaderType();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns an MgReaderType value.
    ///
    virtual INT32 GetReaderType() = 0;

EXTERNAL_API:

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Serializes the unread features in the reader into an XML format.
    /// The XML format of the output is governed by the MgFeatureSet
    /// schema.
    ///
    /// \remarks
    /// XML is serialized from the current position of feature reader
    /// in the order data is retrieved.
    ///
    /// \return
    /// MgByteReader holding XML.
    ///
    virtual MgByteReader* ToXml() = 0;

INTERNAL_API:

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
    virtual const wchar_t* GetString(CREFSTRING propertyName, INT32& length) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serializes data to a TCP/IP stream.
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserializes data from a TCP/IP stream.
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream) = 0;
};
/// \}

#endif
