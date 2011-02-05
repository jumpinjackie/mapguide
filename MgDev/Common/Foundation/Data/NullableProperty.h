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

#ifndef _MG_NULLABLE_PROPERTY_H
#define _MG_NULLABLE_PROPERTY_H

/// \defgroup MgNullableProperty MgNullableProperty
/// \ingroup Feature_Properties_Module
/// \{

/////////////////////////////////////////////////////////////////
/// \brief
/// This is the base class for all properties which can have NULL
/// values. If a property value is NULL, there is no value, and a
/// call to the GetValue() method throws an exception.
///

class MG_FOUNDATION_API MgNullableProperty : public MgProperty
{
    DECLARE_CLASSNAME(MgNullableProperty)

PUBLISHED_API:
    //////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// If set to true, disables the getting and setting of the
    /// property's value. It does not remove any existing value.
    /// Attempting to get or set the value causes an
    /// MgNullPropertyValueException. If set to false, reenables the
    /// getting and setting of the property's values.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetNull(bool bIsNull);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetNull(boolean bIsNull);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetNull(bool bIsNull);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param bIsNull (boolean/bool)
    /// Set to true to disable the getting and setting of
    /// the property value. Set to false to reenable the
    /// getting and setting of the property value.
    ///
    void SetNull(bool bIsNull);

    ////////////////////////////////////////////////////////////
    /// \brief
    /// Determines whether the GetValue and SetValue methods are
    /// enabled.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool IsNull();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean IsNull();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool IsNull();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns true if the GetValue and SetValue methods are
    /// enabled; otherwise returns false.
    ///
    bool IsNull();

INTERNAL_API:

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

protected:

    /// Constructor
    MgNullableProperty();
    /// Destructor
    virtual ~MgNullableProperty();

    /// Throws exception if property is null
    void CheckNull();

private :

    /// Stores whether a value
    bool m_bIsNull;
};
/// \}

#endif
