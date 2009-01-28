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

#ifndef _MG_OBJECT_PROPERTY_DEFINITION_H
#define _MG_OBJECT_PROPERTY_DEFINITION_H

class MgClassDefinition;

/// \defgroup MgObjectPropertyDefinition MgObjectPropertyDefinition
/// \ingroup Feature_Schema_Module
/// \{

/////////////////////////////////////////////////////////////////
/// \brief
/// Defines a feature property which contains one or more objects
/// whose type is a feature class. If there is more than one
/// object, an identity property is defined that is used to
/// select an object from the collection.
///
class MG_PLATFORMBASE_API  MgObjectPropertyDefinition : public MgPropertyDefinition
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgObjectPropertyDefinition)

PUBLISHED_API:
    //////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs a MgObjectPropertyDefinition object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgObjectPropertyDefinition(string name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgObjectPropertyDefinition(String name);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgObjectPropertyDefinition(string name);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param name (String/string)
    /// The name of the property.
    ///
    MgObjectPropertyDefinition(CREFSTRING name);

    /////////////////////////////////////////////////////////////
    /// \brief
    /// Gets an MgClassDefinition object that defines the type of
    /// this property.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgClassDefinition GetClassDefinition();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgClassDefinition GetClassDefinition();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgClassDefinition GetClassDefinition();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns an MgClassDefinition object.
    ///
    MgClassDefinition* GetClassDefinition();   /// __get, __set

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the type of this object property (value, collection, or
    /// ordered collection).
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetObjectType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetObjectType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetObjectType();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns an MgObjectPropertyType value.
    ///
    INT32 GetObjectType();   /// __get, __set

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the MgOrderingOption value (ascending or descending) for
    /// this object property.
    ///
    /// \remarks
    /// This property is only applicable if the value returned by
    /// MgObjectPropertyDefinition::GetOrderType is
    /// MgObjectPropertyType::OrderedCollection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetOrderType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetOrderType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetOrderType();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns an MgOrderingOption value.
    ///
    INT32 GetOrderType();   /// __get, __set

    //////////////////////////////////////////////////////////////
    /// \brief
    /// Gets an MgDataPropertyDefinition object that defines the
    /// property used to identify an object property uniquely.
    ///
    /// \remarks
    /// This method is only used when the property contains a
    /// collection of objects. See
    /// MgObjectPropertyDefinition::GetObjectType.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  MgDataPropertyDefinition GetIdentityProperty();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  MgDataPropertyDefinition GetIdentityProperty();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  MgDataPropertyDefinition GetIdentityProperty();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns an MgDataPropertyDefinition object if the type of
    /// the object property is MgObjectPropertyType::Collection or
    /// MgObjectPropertyType::OrderedCollection.
    ///
    MgDataPropertyDefinition* GetIdentityProperty();   /// __get, __set

    ////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets a reference to the MgClassDefinition that defines the
    /// type of this property.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetClassDefinition(MgClassDefinition classDef);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetClassDefinition(MgClassDefinition classDef);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetClassDefinition(MgClassDefinition classDef);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param classDef (MgClassDefinition)
    /// The class definition used to create the
    /// object or objects that constitute the
    /// property.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetClassDefinition(MgClassDefinition* classDef);

    ///////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the MgObjectPropertyType type (either value, collection,
    /// or ordered collection) of this object property.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetObjectType(int objType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetObjectType(int objType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetObjectType(int objType);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param objType (int)
    /// The MgObjectPropertyType type of this
    /// object property.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetObjectType(INT32 objType);

    ///////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the MgOrderingOption type (either ascending or
    /// descending) for this object property.
    ///
    /// \remarks
    /// This property is only applicable if the value returned by
    /// MgObjectPropertyDefinition::GetOrderType is
    /// MgObjectPropertyType::OrderedCollection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetOrderType(int orderType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetOrderType(int orderType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetOrderType(int orderType);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param orderType (int)
    /// The ordering option type of this object
    /// property.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetOrderType(INT32 orderType);

    //////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets a reference to an MgDataPropertyDefinition used to
    /// create the identity properties that uniquely identify the
    /// objects that constitute the object property.
    ///
    /// \remarks
    /// This identity property definition is only used when the
    /// object property contains an object collection.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    ///  void SetIdentityProperty(MgDataPropertyDefinition propDef);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    ///  void SetIdentityProperty(MgDataPropertyDefinition propDef);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    ///  void SetIdentityProperty(MgDataPropertyDefinition propDef);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param propDef (MgDataPropertyDefinition)
    /// The data property definition used to create
    /// the identity properties for the object
    /// collection in this object property.
    ///
    /// \return
    /// Returns nothing.
    ///
    void SetIdentityProperty(MgDataPropertyDefinition* propDef);

INTERNAL_API:

    MgObjectPropertyDefinition();
    virtual ~MgObjectPropertyDefinition() {}

    void Serialize(MgStream* stream);
    void Deserialize(MgStream* stream);

    virtual INT32 GetClassId()
    {
        return m_cls_id;
    }

    virtual void Dispose()
    {
        delete this;
    }

private:

    void Initialize();

    Ptr<MgClassDefinition> m_clsDef;
    Ptr<MgDataPropertyDefinition> m_dataPropDef;
    INT32 m_objType;
    INT32 m_orderType;

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_FeatureService_ObjectPropertyDefinition;
};
/// \}

#endif
