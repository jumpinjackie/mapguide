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

#ifndef PROPERTY_H
#define PROPERTY_H

/// \defgroup MgProperty MgProperty
/// \ingroup Feature_Properties_Module
/// \{

#include <map>

#include "PropertyType.h"

class MgPropertyCollection;

////////////////////////////////////////////////////////////////
/// \brief
/// A base class for MgNullableProperty, which is the base class
/// for all of the concrete property classes, for example,
/// MgBooleanProperty and a base class for MgPropertyDefinition,
/// which is the base class for all of the concrete property
/// definition classes, for example, MgDataPropertyDefinition.
/// \remarks
/// Allows for polymorphic manipulation of the concrete property
/// classes in the MgPropertyCollection class.
///
class MG_FOUNDATION_API MgProperty : public MgNamedSerializable
{
PUBLISHED_API:
    ///////////////////////////////////////////////////
    /// \brief
    /// Gets the property type.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual short GetPropertyType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual short GetPropertyType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetPropertyType();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the property type. See MgPropertyType.
    ///
    virtual INT16 GetPropertyType() = 0;  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the property name.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetName();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the property name.
    ///
    STRING GetName();  /// __get, __set

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the property name.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetName(string name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetName(String name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetName(string name);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// The name of property.
    ///
    void SetName(CREFSTRING name);

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Does it allow to set the name
    ///
    /// \return
    /// true - allows to set the name
    /// false - can not set the name
    ///
    virtual bool CanSetName();

protected:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the class Id.
    ///
    /// \return
    /// Returns the integer value of the class Id.
    ///
    INT32 GetClassId();

    /// Constructor
    MgProperty();

    /// Destructor
    virtual ~MgProperty();

    /// Self destructing method
    virtual void Dispose();

    /// Export to XML
    virtual void ToXml(string &str, bool includeType = true, string rootElmName = "Property") = 0;

private:

    friend class MgPropertyCollection;

    STRING m_propertyName;

CLASS_ID:
    static const INT32 m_cls_id = Foundation_Property_Property;
};
/// \}

#endif
