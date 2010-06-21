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

#ifndef _MG_DATA_PROPERTY_DEFINITION_H
#define _MG_DATA_PROPERTY_DEFINITION_H

/// \defgroup MgDataPropertyDefinition MgDataPropertyDefinition
/// \ingroup Feature_Schema_Module
/// \{

/////////////////////////////////////////////////////////
/// \brief
/// Defines a data property belonging to a feature class.
///
/// \remarks
/// This definition can represent simple value types like
/// integers or strings.
///
class MG_PLATFORMBASE_API MgDataPropertyDefinition : public MgPropertyDefinition
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgDataPropertyDefinition)

PUBLISHED_API:
    //////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgDataPropertyDefinition object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgDataPropertyDefinition(string name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgDataPropertyDefinition(String name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgDataPropertyDefinition(string name);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// The name of the property.
    ///
    MgDataPropertyDefinition(CREFSTRING name);

    ///////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the data type of this property.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetDataType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetDataType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetDataType();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the data type of this property. See MgPropertyType
    /// for the different data types.
    ///
    INT32 GetDataType();  /// __get, __set

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the length of a String, BLOB, or CLOB data property.
    /// This value does not apply to any other data type.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetLength();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetLength();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetLength();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the length of a String, BLOB, or CLOB data property.
    ///
    INT32 GetLength();  /// __get, __set

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the precision (total number of digits) of a decimal data
    /// property.
    ///
    /// \remarks
    /// This value does not apply to any other data type. The decimal
    /// data type is not supported.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetPrecision();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetPrecision();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetPrecision();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the precision.
    ///
    INT32 GetPrecision();  /// __get, __set

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the scale (number of digits to the right of the decimal
    /// point) of a decimal data property.
    ///
    /// \remarks
    /// This value does not apply to any other data type. The decimal
    /// data property is not supported.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetScale();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetScale();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetScale();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the scale.
    ///
    INT32 GetScale();  /// __get, __set

    //////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a Boolean value that indicates whether this is an
    /// auto-generated property.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool IsAutoGenerated();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean IsAutoGenerated();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool IsAutoGenerated();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns true if the property is auto-generated; otherwise
    /// returns false.
    ///
    bool IsAutoGenerated();

    /////////////////////////////////////////////
    /// \brief
    /// Gets the default value for this property.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetDefaultValue();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetDefaultValue();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetDefaultValue();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the default value.
    ///
    STRING GetDefaultValue();  /// __get, __set

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a Boolean value that indicates whether this property's
    /// value can be null.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool GetNullable();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean GetNullable();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool GetNullable();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns true if the property's value can be null; otherwise
    /// returns false.
    ///
    bool GetNullable();  /// __get, __set

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a Boolean value that indicates whether this property is
    /// read-only.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool GetReadOnly();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean GetReadOnly();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool GetReadOnly();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns a Boolean value that indicates whether this property
    /// is read-only.
    ///
    bool GetReadOnly();  /// __get, __set

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the data type of this property. See MgPropertyType for
    /// the different data types.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetDataType(int type);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetDataType(int type);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetDataType(int type);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param type (int)
    /// The data type of this property.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetDataType(INT32 type);

    /////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines whether this property is read-only
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetReadOnly(bool value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetReadOnly(boolean value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetReadOnly(bool value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (boolean/bool)
    /// True configures the property to be read-only,
    /// and false configures it to be user settable.
    ///
    /// \return
    /// Returns nothing
    ///
    void SetReadOnly(bool value);

    ///////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the maximum length of a String, BLOB, or CLOB data
    /// property. This value does not apply to any other data type.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetLength(int len);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetLength(int len);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetLength(int len);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param len (int)
    /// The length of this String, BLOB, or CLOB data
    /// property.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetLength(INT32 len);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the precision (total number of digits) of a decimal data
    /// property. This value does not apply to any other data type.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetPrecision(int precision);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetPrecision(int precision);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetPrecision(int precision);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param precision (int)
    /// Precision of this decimal property.
    ///
    void SetPrecision(INT32 precision);

    ///////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the scale (number of digits to the right of the decimal
    /// point) of a decimal data property. This value does not apply
    /// to any other data type.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetScale(int scale);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetScale(int scale);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetScale(int scale);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param scale (int)
    /// The scale of this decimal property.
    ///
    void SetScale(INT32 scale);

    ///////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines whether this property's value can be null.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetNullable(bool value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetNullable(boolean value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetNullable(bool value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (boolean/bool)
    /// True configures the property to be nullable. That
    /// is, it is possible to set it to a null value and
    /// false configures the property so that it cannot be
    /// set to a null value.
    ///
    /// \return
    /// Returns nothing
    ///
    void SetNullable(bool value);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the default value for this property.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetDefaultValue(string value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetDefaultValue(String value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetDefaultValue(string value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (String/string)
    /// The default value.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetDefaultValue(CREFSTRING value);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets a boolean value that determines whether this is an
    /// auto-generated property.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetAutoGeneration(bool value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetAutoGeneration(boolean value);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetAutoGeneration(bool value);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param value (boolean/bool)
    /// True configures the property to be
    /// auto-generated and read-only and false
    /// configures it to be user settable.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetAutoGeneration(bool value);

INTERNAL_API:

    MgDataPropertyDefinition();
    void Serialize(MgStream* stream);
    void Deserialize(MgStream* stream);
    void Initialize();
    void ToXml(string& xmlStr, bool includeType);
    void ValidateArgument(INT32 value);

    virtual void Dispose()
    {
        delete this;
    }

private:

    INT32       m_dataType;
    bool        m_readOnly;
    INT32       m_length;
    INT32       m_precision;
    INT32       m_scale;
    bool        m_nullable;
    STRING      m_defaultValue;
    bool        m_autogenerated;
    STRING      m_serializedXml;

INTERNAL_API:

    virtual INT32 GetClassId()
    {
        return m_cls_id;
    }

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_DataPropertyDefinition;
};
/// \}

#endif
