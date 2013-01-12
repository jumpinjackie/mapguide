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

#ifndef _MG_PROPERTY_DEFINITION_H_
#define _MG_PROPERTY_DEFINITION_H_

/// \defgroup MgPropertyDefinition MgPropertyDefinition
/// \ingroup Feature_Schema_Module
/// \{

/////////////////////////////////////////////////////////////////
/// \brief
/// Defines the base class for the concrete property definition
/// classes which are used to create data, geometric, object, and
/// raster property definitions.
/// \remarks
/// It is also used in the definition of
/// MgPropertyDefinitionCollection so that the latter can be used
/// to store objects constructed from the concrete property
/// definition classes.
class MG_FOUNDATION_API  MgPropertyDefinition : public MgProperty
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgPropertyDefinition)

PUBLISHED_API:
    //////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgPropertyDefinition object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgPropertyDefinition(string name, short type);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgPropertyDefinition(String name, short type);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgPropertyDefinition(string name, int type);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// The name of the property.
    /// \param type (short/int)
    /// The type of the property.
    ///
    /// \return
    /// Nothing
    ///
    MgPropertyDefinition(CREFSTRING name, INT16 type);

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the definition type (either data, geometric, object, or
    /// raster) of this property.
    ///
    /// \remarks
    /// After retrieving a property definition object from an
    /// MgPropertyDefinitionCollection, you can use this method to
    /// determine the concrete type of the property definition
    /// object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// short GetPropertyType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// short GetPropertyType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetPropertyType();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns one of the MgFeaturePropertyType values that
    /// identifies the concrete type.
    ///
    ///
    INT16 GetPropertyType();  /// __get, __inherited

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the description of this property.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetDescription(string description);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetDescription(String description);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetDescription(string description);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param description (String/string)
    /// The description of the
    /// property.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetDescription(CREFSTRING description);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the description of this property.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetDescription();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetDescription();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetDescription();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns a String containing the description of the property.
    ///
    STRING GetDescription();  /// __get, __set

    ///////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the qualified name of this property. The format of the
    /// qualified name is \<schemaName\>:\<className\>.\<propertyName\>.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetQualifiedName(string qualifiedName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetQualifiedName(String qualifiedName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetQualifiedName(string qualifiedName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param qualifiedName (String/string)
    /// The qualified name of the
    /// property.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetQualifiedName(CREFSTRING qualifiedName);

    ////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the qualified name of this property whose format is
    /// \<schemaName\>:\<className\>.\<propertyName\>.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetQualifiedName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetQualifiedName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetQualifiedName();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns a String containing the qualified name.
    ///
    STRING GetQualifiedName();  /// __get, __set

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Marks the property definition for deletion.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Delete();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Delete();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Delete();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \since 2.2
	///
    void Delete();

EXTERNAL_API:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor.  Deletes property and any contained information
    ///
    /// \return
    /// Nothing
    ///
    virtual ~MgPropertyDefinition();

INTERNAL_API:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructor.
    ///
    MgPropertyDefinition();

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

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks whether the property definition is marked as deleted.
    ///
    /// \return
    /// Returns true if the property definition is marked as deleted,
    /// false otherwise.
    ///
    bool IsDeleted();

protected:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// The integer value
    ///
    INT32 GetClassId();

    friend class MgPropertyDefinitionCollection;

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Converts property name and value to XML
    ///
    /// \return
    /// ByteReader for the XML
    ///
    void ToXml(string &str, bool includeType = true, string rootElmName = "PropertyDefinition");
    void ToColumnDefinition(string &str, bool includeType = true);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Self destructing method
    ///
    virtual void Dispose();

private:
    static bool Initialize();
    static bool smb_initialize;

    STRING          m_qualifiedName;
    STRING          m_description;
    INT16           m_propertyType;
    bool            m_isDeleted;

CLASS_ID:
    static const INT32 m_cls_id = Foundation_Property_PropertyDefinition;
};
/// \}

#endif
