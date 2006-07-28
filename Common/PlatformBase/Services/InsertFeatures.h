//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#ifndef _MGINSERTFEATURES_H
#define _MGINSERTFEATURES_H

/// \ingroup Feature_Service_classes

class MgInsertFeatures;
template class MG_PLATFORMBASE_API Ptr<MgInsertFeatures>;

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// \brief
/// Creates an insert feature data command. You must add this
/// command to an MgFeatureCommandCollection object and pass the
/// latter in a call to MgFeatureService::UpdateFeatures.
///
/// \remarks
/// If you inserting one feature, you can create the command
/// using the \link MgInsertFeatures::MgInsertFeatures MgInsertFeatures::MgInsertFeatures Constructor (CREFSTRING, MgPropertyCollection*) \endlink.
/// If you are inserting many features, you can create the
/// command using the \link MgInsertFeatures::MgInsertFeatures MgInsertFeatures::MgInsertFeatures Constructor (CREFSTRING, MgBatchPropertyCollection*) \endlink.
/// \n
/// <b>Procedure</b>
///   <ol>
///     <li>Get the property definitions for the class. See \link MgClassDefinition::GetProperties MgClassDefinition::GetProperties Method \endlink.
///       As a minimum, you must create property values for each of the
///       properties defined as mandatory. See \link MgDataPropertyDefinition::GetNullable MgDataPropertyDefinition::GetNullable Method \endlink
///       and \link MgRasterPropertyDefinition::GetNullable MgRasterPropertyDefinition::GetNullable Method \endlink.</li>
///     <li>Create a property object for each property value
///       appropriate to the type of the value; for example, use the
///       MgGeometryProperty class to create an object containing a
///       geometry value and use the MgBooleanProperty class to create
///       an object containing a boolean value; the property class
///       constructor is passed the name of the property and its value.
///       See \link Feature_Properties_Module Feature Properties \endlink.</li>
///     <li>Add each property object to an MgPropertyCollection
///       object </li>
///     <li>Create the insert command</li>
///     <ol>
///       <li>Pass the class name and the property collection to
///         the \link MgInsertFeatures::MgInsertFeatures MgInsertFeatures::MgInsertFeatures Constructor (CREFSTRING, MgPropertyCollection*) \endlink
///         and go on to step 5, or</li>
///       <li>Add the MgPropertyCollection object to an
///         MgBatchPropertyCollection object, and</li>
///       <li>Create and add MgPropertyCollection objects to the
///         batch property collection for all of the features that you
///         want to add, and</li>
///       <li>Pass the class name and batch property collection to
///         the \link MgInsertFeatures::MgInsertFeatures MgInsertFeatures::MgInsertFeatures Constructor (CREFSTRING, MgBatchPropertyCollection*) \endlink
///         and go on to step 5</li>
///     </ol>
///     <li>Add the MgInsertFeatures object to an
///       MgFeatureCommandCollection object.</li>
///     <li>Execute the MgFeatureServices::UpdateFeatures() method;
///       the arguments are a resource identifier for the feature
///       source and the MgFeatureCommandCollection object.</li>
///     <li>Check the result. The result is an MgPropertyCollection
///       object.</li>
///     1. For each insert command constructed with an
///     MgPropertyCollection object argument, there will be one
///     property of type MgPropertyType::Feature in the collection.
///     This property is an MgFeatureReader object containing the
///     properties that were inserted.
///   </ol>
/// \n
/// The following illustration shows the relationships among the
/// classes used in creating the arguments for a call to
/// MgFeatureService::UpdateFeatures(), which contains one
/// insertion command:
/// \image html InsertFeatures.png
///
class MG_PLATFORMBASE_API  MgInsertFeatures : public MgFeatureCommand
{
    DECLARE_CREATE_OBJECT();
    DECLARE_CLASSNAME(MgInsertFeatures)

PUBLISHED_API:
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an insert command containing one feature.
    ///
    /// \remarks
    /// Do not clear or close the MgPropertyCollection object until
    /// after you have called MgFeatureService::UpdateFeatures. The
    /// constructor does not do a deep copy of the collection object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgInsertFeatures(string className, MgPropertyCollection propertyValues);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgInsertFeatures(String className, MgPropertyCollection propertyValues);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgInsertFeatures(string className, MgPropertyCollection propertyValues);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param className (String/string)
    /// The name of the class in the feature
    /// source into which the collection of
    /// property values is to be inserted.
    /// \param propertyValues (MgPropertyCollection)
    /// The collection of property values to be
    /// inserted into the feature source.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// <?php
    /// $propertyCollection = new MgPropertyCollection();
    /// $featIdProp = new MgInt32Property(0);
    /// $propertyCollection->Add($featIdProp);
    /// $insertFeatures = new MgInsertFeatures("featClass", $propertyCollection);
    /// $featCmdCollection = new MgFeatureCommandCollection();
    /// $featCmdCollection->Add($insertFeatures);
    /// $useTransaction = false;
    /// $featureService->UpdateFeatures($featureSrcResourceId, $featCmdCollection, $useTransaction);
    /// ?>
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    MgInsertFeatures( CREFSTRING className,
                      MgPropertyCollection* propertyValues );

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an insert command containing many features.
    ///
    /// \remarks
    /// Do not clear or close the MgBatchPropertyCollection object
    /// until after you have called MgFeatureService::UpdateFeatures.
    /// The constructor does not do a deep copy of the collection
    /// object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgInsertFeatures(string className, MgBatchPropertyCollection propertyValueCollection);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgInsertFeatures(String className, MgBatchPropertyCollection propertyValueCollection);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgInsertFeatures(string className, MgBatchPropertyCollection propertyValueCollection);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param className (String/string)
    /// The name of the class in the schema
    /// definition in the feature source used to
    /// create the feature instances to be inserted.
    /// \param propertyValueCollection (MgBatchPropertyCollection)
    /// The collection of property collections to be
    /// inserted into the feature source. Each
    /// collection contains the property values for
    /// one feature.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// \code
    /// <?php
    /// $batchPropertyCollection = new MgBatchPropertyCollection();
    /// $featCmdCollection = new MgFeatureCommandCollection();
    /// for($i = 0; $i < 2; $i++) {
    ///    $col = new MgPropertyCollection();
    ///    $featIdProp = new MgInt32Property($i);
    ///    $col->Add($featIdProp);
    ///    $batchPropertyCollection->Add($col);
    /// \}
    /// $insertFeatures = new MgInsertFeatures("featClass", $batchPropertyCollection);
    /// $featCmdCollection->Add($insertFeatures);
    /// $useTransaction = false;
    /// $featureService->UpdateFeatures($featureSrcResourceId, $featCmdCollection, $useTransaction);
    /// ?>
    /// \endcode
    /// \htmlinclude ExampleBottom.html
    ///
    MgInsertFeatures( CREFSTRING className,
                      MgBatchPropertyCollection* propertyValueCollection );

    //////////////////////////////////////////////////
    /// \brief
    /// Gets the type of the command, that is,
    /// MgFeatureCommandType::InsertFeatures.
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
    /// Returns MgFeatureCommandType::InsertFeatures.
    ///
    INT32 GetCommandType() const;

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the collection of property values specified in \link MgInsertFeatures::MgInsertFeatures MgInsertFeatures::MgInsertFeatures Constructor (CREFSTRING, MgPropertyCollection*) \endlink.
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
    /// Returns the collection of property values to be inserted.
    ///
    MgPropertyCollection* GetPropertyValues() const;

    /////////////////////////////////////////////////////////
    /// \brief
    /// Gets the feature class name specified in the constructor.
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
    STRING GetFeatureClassName() const;

EXTERNAL_API:

    /// \brief
    /// Destructor
    ///
    ~MgInsertFeatures();

INTERNAL_API:

    MgBatchPropertyCollection* GetBatchPropertyValues() const;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serialize data to TCP/IP stream
    ///
    /// \param stream
    /// Stream
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

    MgInsertFeatures() {}

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

    static const INT32 m_cls_id = PlatformBase_FeatureService_InsertFeatures;

private:
    STRING m_className;
    Ptr<MgBatchPropertyCollection> m_properties;
};

#endif // _MGINSERTFEATURES_H
