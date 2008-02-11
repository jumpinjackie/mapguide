//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef _MGCLASS_DEFINITION_H
#define _MGCLASS_DEFINITION_H

/// \defgroup MgClassDefinition MgClassDefinition
/// \ingroup Feature_Schema_Module
/// \{

class MgClassDefinition;
template class MG_PLATFORMBASE_API Ptr<MgClassDefinition>;

/////////////////////////////////////////////////////////////////
/// \brief
/// Defines a feature class belonging to a schema.
///
class MG_PLATFORMBASE_API MgClassDefinition : public MgNamedSerializable
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgClassDefinition)

PUBLISHED_API:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgClassDefinition object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgClassDefinition();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgClassDefinition();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgClassDefinition();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgClassDefinition();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a collection containing all the property definitions for
    /// this class including identity property definitions.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgPropertyDefinitionCollection GetProperties();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgPropertyDefinitionCollection GetProperties();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgPropertyDefinitionCollection GetProperties();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns a collection of the property definitions for this
    /// class.
    ///
    MgPropertyDefinitionCollection* GetProperties();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a collection containing the set of data property
    /// definitions that can be used to uniquely identify instances
    /// of this class.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgPropertyDefinitionCollection GetIdentityProperties();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgPropertyDefinitionCollection GetIdentityProperties();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgPropertyDefinitionCollection GetIdentityProperties();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns a collection of data property definitions that can
    /// be used to uniquely identify instances of this class.
    ///
    MgPropertyDefinitionCollection* GetIdentityProperties();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name of the default geometry property in this class.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetDefaultGeometryPropertyName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetDefaultGeometryPropertyName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetDefaultGeometryPropertyName();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the name of the default geometry property.
    ///
    STRING GetDefaultGeometryPropertyName();  /// __get, __set

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the name of the default geometry property in this class.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetDefaultGeometryPropertyName(string name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetDefaultGeometryPropertyName(String name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetDefaultGeometryPropertyName(string name);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// The name of the default geometry
    /// property.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetDefaultGeometryPropertyName(CREFSTRING name);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the name of the feature class.
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
    /// The feature class name.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetName(CREFSTRING name);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name of the feature class.
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
    /// Returns the feature class name.
    ///
    STRING GetName();  /// __get, __set

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the feature class description.
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
    /// The feature class description.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetDescription(CREFSTRING description);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the feature class description
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
    /// Returns the feature class description.
    ///
    STRING GetDescription();  /// __get, __set

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the base class definition.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgClassDefinition GetBaseClassDefinition();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgClassDefinition GetBaseClassDefinition();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgClassDefinition GetBaseClassDefinition();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the feature class definition or NULL if no base
    /// class is defined.
    ///
    MgClassDefinition* GetBaseClassDefinition();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Tests whether a class definition is computed.
    ///
    /// \remarks
    /// A computed class is transient and does not persist in the
    /// datastore. The class definition for a computed property
    /// return by an MgFeatureService::SelectFeatures or an
    /// MgFeatureService SelectAggregate operation is computed.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool IsComputed();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean IsComputed();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool IsComputed();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns true if the class definition is computed; otherwise
    /// returns false.
    ///
    bool IsComputed();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Tests whether a class definition is abstract or not.
    ///
    /// \remarks
    /// If a class definition is abstract it can not be used to set
    /// the class definition of an MgObjectPropertyDefinition object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool IsAbstract();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean IsAbstract();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool IsAbstract();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns true if the class definition is abstract; otherwise
    /// returns false.
    ///
    bool IsAbstract();

EXTERNAL_API:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor
    ///
    ~MgClassDefinition();

INTERNAL_API:
    void Serialize(MgStream* stream);
    void Deserialize(MgStream* stream);
    bool CanSetName();
    INT32 GetClassId();
    void Dispose();
    void ToXml(string &str);
    void SetSerializedXml(CREFSTRING strXml);
    bool HasRasterProperty();
    STRING GetRasterPropertyName();
    void SetBaseClassDefinition(MgClassDefinition* baseClassDef);
    void MakeClassComputed(bool isComputed);
    void MakeClassAbstract(bool isAbstract);
    MgPropertyDefinitionCollection* GetPropertiesIncludingBase();
    MgStringCollection* GetClassesIncludingBase();

private:

    Ptr<MgPropertyDefinitionCollection> m_classProperties;
    Ptr<MgPropertyDefinitionCollection> m_identityProperties;
    Ptr<MgPropertyDefinitionCollection> m_totalProperties;

    STRING m_className;
    STRING m_description;
    STRING m_defaultGeometryPropertyName;
    STRING m_serializedXml;
    bool m_hasRasterProperty;
    STRING m_rasterPropName;
    bool m_isComputed;
    bool m_isAbstract;
    Ptr<MgClassDefinition> m_baseClassDefinition;
    Ptr<MgStringCollection> m_classList;

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_ClassDefinition;
};
/// \}

#endif
