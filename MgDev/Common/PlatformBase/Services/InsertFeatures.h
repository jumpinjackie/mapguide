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

#ifndef _MGINSERTFEATURES_H
#define _MGINSERTFEATURES_H

/// \defgroup MgInsertFeatures MgInsertFeatures
/// \ingroup Feature_Service_classes
/// \{

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
    MG_DECL_DYNCREATE();
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
    /// <!-- Example (C#) -->
    /// \htmlinclude CSharpExampleTop.html
    /// \code
    /// using OSGeo.MapGuide;
    /// private MgFeatureService featureService;
    /// private MgResourceIdentifier resourceId;
    /// private MgPropertyCollection insertFeatIdOne;
    /// private MgPropertyCollection insertResults;
    /// private MgInt32Property FeatIdOneProp;
    /// private MgByteReader point11Reader;
    /// private MgGeometryProperty point11Prop;
    /// private MgByteReader Poly0040440400Reader;
    /// private MgGeometryProperty Poly0040440400Prop;
    /// private MgBooleanProperty insertABoolean;
    /// private MgByteProperty aZeroByte;
    /// private MgDateTime Dec312000;
    /// private MgSingleProperty neg5dot1234567Prop);
    /// private MgDoubleProperty neg5Dot123456789012345Prop;
    /// private MgInt16Property minInt16Prop;
    /// private MgInt32Property minInt32Prop;
    /// private MgInt64Property minInt64Prop;
    /// private MgStringProperty point11StrProp;
    /// private MgStringProperty groupNameOneStrProp;
    /// private MgStringProperty notNullStrProp;
    /// private MgInsertFeatures insertFeatures;
    /// private MgFeatureCommandCollection commands;
    /// private Boolean useTransaction = false;
    ///
    /// insertFeatIdOne = new MgPropertyCollection();
    /// FeatIdOneProp = new MgInt32Property("FeatId", 1);
    /// insertFeatIdOne.Add(FeatIdOneProp);
    /// point11Reader = geometryServices.WktTextToMgByteReader("POINT XY (1 1)");
    /// point11Prop = new MgGeometryProperty("theFeatureGeometry", point11Reader);
    /// insertFeatIdOne.Add(point11Prop);
    /// Poly0040440400Reader = geometryServices.WktTextToMgByteReader("POLYGON XY ((0 0, 4 0, 4 4, 0 4, 0 0), (1 1, 1 2, 2 2, 2 1, 1 1))");
    /// Poly0040440400Prop = new MgGeometryProperty("aNonFeatureGeometry", Poly0040440400Reader);
    /// insertFeatIdOne.Add(Poly0040440400Prop);
    /// insertABoolean = new MgBooleanProperty("aBoolean", true);
    /// insertFeatIdOne.Add(insertABoolean);
    /// aZeroByte = new MgByteProperty("aByte", 0);
    /// insertFeatIdOne.Add(aZeroByte);
    /// Dec312000 = new MgDateTime(2000, 12, 31, 0, 0, 0, 0);
    /// insertFeatIdOne.Add(Dec312000Prop);
    /// neg5dot1234567Prop = new MgSingleProperty("aSingle", -5.1234567);
    /// insertFeatIdOne.Add(neg5dot1234567Prop);
    /// neg5Dot123456789012345Prop = new MgDoubleProperty("aDouble", -5.123456789012345);
    /// insertFeatIdOne.Add(neg5Dot123456789012345Prop);
    /// minInt16Prop = new MgInt16Property("anInt16", Int16.MinValue);
    /// insertFeatIdOne.Add(minInt16Prop);
    /// minInt32Prop = new MgInt32Property("anInt32", Int32.MinValue);
    /// insertFeatIdOne.Add(minInt32Prop);
    /// minInt64Prop = new MgInt64Property("anInt64", Int64.MinValue);
    /// insertFeatIdOne.Add(minInt64Prop);
    /// point11StrProp = new MgStringProperty("aString", "POINT XY (1 1)");
    /// insertFeatIdOne.Add(point11StrProp);
    /// insertFeatures = new MgInsertFeatures(className, properties);
    /// commands = new MgFeatureCommandCollection();
    /// commands.Add(insertFeatures);
    /// int commandIndex = commands.IndexOf(insertFeatures);
    /// // this feature source already exists in the repository
    /// resourceId = new MgResourceIdentifier("Library://PlatformApiDocTests/SdfFeatureClass.FeatureSource");
    /// // the sample code for the MgFeatureService class shows how to instantiate it
    /// insertResults = featureService.UpdateFeatures(resourceId, commands, useTransaction);
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
    /// for($i = 0; $i < 2; $i++)
    /// {
    ///    $col = new MgPropertyCollection();
    ///    $featIdProp = new MgInt32Property($i);
    ///    $col->Add($featIdProp);
    ///    $batchPropertyCollection->Add($col);
    /// }
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
    INT32 GetCommandType() const;   /// __get

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
    MgPropertyCollection* GetPropertyValues() const;   /// __get

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
    STRING GetFeatureClassName() const;   /// __get

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
/// \}

#endif // _MGINSERTFEATURES_H
