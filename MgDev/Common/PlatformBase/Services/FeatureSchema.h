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

#ifndef _MG_FEATURE_SCHEMA_H
#define _MG_FEATURE_SCHEMA_H

/// \defgroup MgFeatureSchema MgFeatureSchema
/// \ingroup Feature_Service_classes
/// \{

class MgClassDefinitionCollection;

/////////////////////////////////////////////////////////////////
/// \brief
/// Contains one or more feature class definitions, which specify
/// the structure of feature data in a datastore.
///
class MG_PLATFORMBASE_API MgFeatureSchema : public MgNamedSerializable
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgFeatureSchema)

PUBLISHED_API:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an empty MgFeatureSchema object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgFeatureSchema();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgFeatureSchema();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgFeatureSchema();
    /// \htmlinclude SyntaxBottom.html
    ///
    MgFeatureSchema();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgFeatureSchema object and sets the schema name
    /// and description.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgFeatureSchema(string name, string description);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgFeatureSchema(String name, String description);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgFeatureSchema(string name, string description);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// The name of the schema.
    /// \param description (String/string)
    /// A description of the schema.
    ///
    MgFeatureSchema(CREFSTRING name, CREFSTRING description);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the schema name.
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
    /// Returns the name of the schema.
    ///
    STRING GetName();  /// __get, __set

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the schema name.
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
    /// The name of the schema.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetName(CREFSTRING name);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the schema description.
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
    /// Returns the description of the schema.
    ///
    STRING GetDescription();  /// __get, __set

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Set the schema description.
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
    /// The description of the schema.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetDescription(CREFSTRING description);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the class definitions.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgClassDefinitionCollection GetClasses();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgClassDefinitionCollection GetClasses();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgClassDefinitionCollection GetClasses();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns a collection of the class definitions.
    ///
    MgClassDefinitionCollection* GetClasses();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Marks the feature schema for deletion.
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
    void Delete();

INTERNAL_API:
    void Dispose();
    void Serialize(MgStream* stream);
    void Deserialize(MgStream* stream);
    bool CanSetName();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks whether the feature schema is marked as deleted.
    ///
    /// \return
    /// Returns true if the feature schema is marked as deleted,
    /// false otherwise.
    ///
    bool IsDeleted();

protected:
    INT32 GetClassId();

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_FeatureSchema;

private:
    STRING m_name;
    STRING m_description;
    bool   m_isDeleted;
    Ptr<MgClassDefinitionCollection> m_collection;
};
/// \}

#endif
