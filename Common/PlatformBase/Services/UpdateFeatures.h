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

#ifndef _MGUPDATEFEATURES_H
#define _MGUPDATEFEATURES_H

/// \defgroup MgUpdateFeatures MgUpdateFeatures
/// \ingroup Feature_Service_classes
/// \{

class MgUpdateFeatures;
template class MG_PLATFORMBASE_API Ptr<MgUpdateFeatures>;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// Creates an update feature data command. You must add this
/// command to an MgFeatureCommandCollection object and pass the
/// latter in a call to MgFeatureService::UpdateFeatures.
///
/// \remarks
/// You can change the values of the properties of one feature by
/// setting the filter to select that feature, or you can do a
/// global change on the values of the properties of many
/// features by setting the filter to select those features. See \link FiltersAndExpressions filter and expressions \endlink
/// for instruction on how to set the filter. If you want to
/// change the properties of many features so that each of those
/// properties has a different value, then you must create one
/// update command for each feature.
/// <h2>Procedure</h2>
///   <ol>
///       <li>Get the property definitions for the class. See \link MgClassDefinition::GetProperties MgClassDefinition::GetProperties Method \endlink.
///               As a minimum, you must create property values for each of the
///               properties defined as mandatory. See \link MgDataPropertyDefinition::GetNullable MgDataPropertyDefinition::GetNullable Method \endlink
///               and \link MgRasterPropertyDefinition::GetNullable MgRasterPropertyDefinition::GetNullable Method \endlink.
///       </li>
///       <li>Create a property object for each property value
///               appropriate to the type of the value; for example, use the
///               MgGeometryProperty class to create an object containing a
///               geometry value and use the MgBooleanProperty class to create
///               an object containing a boolean value; the property class
///               constructor is passed the name of the property and its value.
///               See \link Feature_Properties_Module Feature Properties \endlink.
///       </li>
///       <li>Add each property object to an MgPropertyCollection
///               object
///       </li>
///       <li>Create the update command. Pass the class name, the
///               property collection, and the filter text expression to the \link MgUpdateFeatures::MgUpdateFeatures MgUpdateFeatures::MgUpdateFeatures Constructor (CREFSTRING, MgPropertyCollection*, CREFSTRING) \endlink.
///       </li>
///       <li>Add the MgInsertFeatures object to an
///               MgFeatureCommandCollection object.
///       </li>
///       <li>Execute the MgFeatureServices::UpdateFeatures() method;
///               the arguments are a resource identifier for the feature
///               source and the MgFeatureCommandCollection object.
///       </li>
///       <li>Check the result. The result is an MgPropertyCollection
///               object. For each update command, there will be one property
///                  of type MgPropertyType::Int32 in the returned collection.
///                  The index of this property matches the index of the update
///                  command in the feature command collection passed to
///                  MgFeatureService::UpdateFeatures.
///       </li>
///   </ol>
///
///

class MG_PLATFORMBASE_API  MgUpdateFeatures : public MgFeatureCommand
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgUpdateFeatures)

PUBLISHED_API:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgUpdateFeatures object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgUpdateFeatures(string className, MgPropertyCollection propertyValues, string filterText);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgUpdateFeatures(String className, MgPropertyCollection propertyValues, String filterText);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgUpdateFeatures(string className, MgPropertyCollection propertyValues, string filterText);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param className (String/string)
    /// The name of class in the schema
    /// definition in the feature source whose
    /// feature instances are to be updated
    /// \param propertyValues (MgPropertyCollection)
    /// The collection of properties to be
    /// updated.
    /// \param filterText (String/string)
    /// The filter used to select the feature
    /// instances to be updated.
    ///
    /// \return
    /// Returns nothing.
    ///
    MgUpdateFeatures(   CREFSTRING className,
                        MgPropertyCollection* propertyValues,
                        CREFSTRING filterText   );

    //////////////////////////////////////////////////
    /// \brief
    /// Gets the type of the command, that is,
    /// MgFeatureCommandType::UpdateFeatures.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetCommandType() const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetCommandType() const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetCommandType() const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns MgFeatureCommandType::UpdateFeatures.
    ///
    INT32 GetCommandType() const;   /// __get

    //////////////////////////////////////////////////////////
    /// \brief
    /// Get the property values collection specified in the
    /// constructor.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgPropertyCollection GetPropertyValues() const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgPropertyCollection GetPropertyValues() const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgPropertyCollection GetPropertyValues() const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the collection of properties specified in the
    /// constructor.
    ///
    MgPropertyCollection* GetPropertyValues() const;   /// __get

    /////////////////////////////////////////////////////////
    /// \brief
    /// Get the feature class name specified in the constructor.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetFeatureClassName() const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetFeatureClassName() const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetFeatureClassName() const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the feature class name specified in the constructor.
    ///
    STRING GetFeatureClassName() const;   /// __get

    //////////////////////////////////////////////////////////////
    /// \brief
    /// Get the filter specified in the constructor.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetFilterText() const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetFilterText() const;
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetFilterText() const;
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns he filter specified specified in the constructor.
    ///
    STRING GetFilterText() const;   /// __get

EXTERNAL_API:

    /// \brief
    /// Destructor
    ///
    ~MgUpdateFeatures();

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serialize data to TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    ///

    void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserialize data from TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    void Deserialize(MgStream* stream);

protected:

    MgUpdateFeatures() {}

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// Returns the integer value.
    ///
    INT32 GetClassId();

    virtual void Dispose();

CLASS_ID:

    static const INT32 m_cls_id = PlatformBase_FeatureService_UpdateFeatures;


private:

    STRING m_className;
    STRING m_filterText;
    Ptr<MgPropertyCollection> m_properties;

};
/// \}

#endif // _MGUPDATEFEATURES_H
