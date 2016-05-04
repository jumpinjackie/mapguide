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

#ifndef _MG_LAYER_H_
#define _MG_LAYER_H_

class MgLayer;
template class MG_MAPGUIDE_API Ptr<MgLayer>;

#ifdef _WIN32
#pragma warning(disable:4251)
#endif

/// \defgroup MgLayer MgLayer
/// \ingroup Maps_and_Layers_Module
/// \{

/////////////////////////////////////////////////////////////////
/// \brief
/// Gets and sets properties of a layer.
/// \todo
class MG_MAPGUIDE_API MgLayer : public MgLayerBase
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgLayer)

PUBLISHED_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a MgLayer object from a layer definition.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgLayer(MgResourceIdentifier layerDefinition, MgResourceService resourceService);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgLayer(MgResourceIdentifier layerDefinition, MgResourceService resourceService);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgLayer(MgResourceIdentifier layerDefinition, MgResourceService resourceService);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param layerDefinition (MgResourceIdentifier)
    /// Identifier of the layer definition in a
    /// repository.
    /// \param resourceService (MgResourceService)
    /// An MgResourceService that can be used to
    /// retrieve the layer definition.
    ///
    /// \todo
    ///   * [[Job for Philip: I need a code sample here.]]
    ///
    MgLayer(MgResourceIdentifier* layerDefinition, MgResourceService* resourceService);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the class definition for the feature class of the layer.  If the
    /// feature class of the layer is extended with properties from other feature
    /// classes, then all those properties are also contained in the returned
    /// class definition. See \link Feature_Schema_Module Feature Schema \endlink.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgClassDefinition GetClassDefinition(string schemaName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgClassDefinition GetClassDefinition(String schemaName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgClassDefinition GetClassDefinition(string schemaName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns an MgClassDefinition instance for the feature class of the layer.
    ///
    /// \exception MgFeatureServiceException
    /// \exception MgInvalidArgumentException
    /// \exception MgFdoException
    ///
    virtual MgClassDefinition* GetClassDefinition();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Selects features from a feature source according to the
    /// criteria set in the MgFeatureQueryOptions argument The
    /// criteria are applied to all of the features in the feature
    /// source. If you want to apply the criteria to a subset of the
    /// features, use the \link MgFeatureService::SelectAggregate MgFeatureService::SelectAggregate Method \endlink.
    /// See \link FiltersAndExpressions Filters and expressions \endlink.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgFeatureReader SelectFeatures(MgFeatureQueryOptions options);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgFeatureReader SelectFeatures(MgFeatureQueryOptions options);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgFeatureReader SelectFeatures(MgFeatureQueryOptions options);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param options (MgFeatureQueryOptions)
    /// MgFeatureQueryOptions instance
    /// containing all required filters for this
    /// select operation.
    ///
    /// \return
    /// Returns an MgFeatureReader containing the set of selected
    /// features.
    ///
    /// \exception MgFeatureServiceException
    /// \exception MgInvalidArgumentException
    /// \exception MgFdoException
    ///
    virtual MgFeatureReader* SelectFeatures(MgFeatureQueryOptions* options);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Selects features from a feature source according to the
    /// criteria set in the MgFeatureQueryOptions argument The
    /// criteria are applied to all of the features in the feature
    /// source. If you want to apply the criteria to a subset of the
    /// features, use the \link MgFeatureService::SelectAggregate MgFeatureService::SelectAggregate Method \endlink.
    /// See \link FiltersAndExpressions Filters and expressions \endlink.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgFeatureReader SelectFeatures(MgFeatureQueryOptions options);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgFeatureReader SelectFeatures(MgFeatureQueryOptions options);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgFeatureReader SelectFeatures(MgFeatureQueryOptions options);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param options (MgFeatureQueryOptions)
    /// MgFeatureQueryOptions instance
    /// containing all required filters for this
    /// select operation.
    /// \param coordinateSystem (String/string)
    /// The WKT string of the coordinate system to transform features to
    ///
    /// \return
    /// Returns an MgFeatureReader containing the set of selected
    /// features.
    ///
    /// \exception MgFeatureServiceException
    /// \exception MgInvalidArgumentException
    /// \exception MgFdoException
    ///
    /// \since 3.0
    virtual MgFeatureReader* SelectFeatures(MgFeatureQueryOptions* options, CREFSTRING coordinateSystem);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Selects groups of features from a feature source and applies
    /// filters to each of the groups according to the criteria set
    /// in the MgFeatureAggregateOptions argument. If you want to
    /// apply the criteria to all features without grouping them, use
    /// the \link MgFeatureService::SelectFeatures MgFeatureService::SelectFeatures Method \endlink.
    /// See \link FiltersAndExpressions Filters and expressions \endlink.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgDataReader SelectAggregate(MgFeatureAggregateOptions options);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgDataReader SelectAggregate(MgFeatureAggregateOptions options);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgDataReader SelectAggregate(MgFeatureAggregateOptions options);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param options (MgFeatureAggregateOptions)
    /// An MgFeatureAggregateOptions instance
    /// containing all the criteria and filters
    /// required for this select operation.
    ///
    /// \return
    /// Returns an MgDataReader containing the group values.
    ///
    /// \exception MgFeatureServiceException
    /// \exception MgInvalidArgumentException
    /// \exception MgFdoException
    ///
    virtual MgDataReader* SelectAggregate(MgFeatureAggregateOptions* options);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Executes the MgDeleteFeatures, MgInsertFeatures,
    /// MgUpdateFeatures, MgLockFeatures or MgUnlockFeatures commands
    /// contained in the given MgFeatureCommandCollection object.
    ///
    /// \remarks
    /// The XML returned by MgFeatureService::GetCapabilities says
    /// whether a provider supports SQL commands. See \link ProviderCapabilities Provider Capabilities \endlink.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgPropertyCollection UpdateFeatures(MgFeatureCommandCollection commands);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgPropertyCollection UpdateFeatures(MgFeatureCommandCollection commands);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgPropertyCollection UpdateFeatures(MgFeatureCommandCollection commands);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param commands (MgFeatureCommandCollection)
    /// A collection of feature commands to be
    /// executed.
    ///
    /// \return
    /// Returns an MgPropertyCollection object. Each property in the
    /// collection corresponds to a command in the
    /// MgFeatureCommandCollection argument. The property name is the
    /// index of the command in the feature command collection.
    /// <ul>
    ///   <li>If the command is of type MgDeleteFeatures, the property
    ///     type is an MgPropertyType::Int32, and its value is the number
    ///     of features deleted.</li>
    ///   <li>If the command is of type MgInsertFeatures, the property
    ///     type is an MgPropertyType::Feature, and its value is a
    ///     MgFeatureReader object. The feature reader object contains
    ///     the set of properties inserted into the datastore by the
    ///     insert command.</li>
    ///   <li>If the command is of type MgUpdateFeatures, the property
    ///     type is MgPropertyType::Int32, and its value is the number of
    ///     features updated.</li>
    ///   <li>If the command is of type MgLockFeatures, the property
    ///     type is MgPropertyType::Feature, and its value is the number
    ///     of features locked.</li>
    ///   <li>If the command is of type MgUnLockFeatures, the property
    ///     type is MgPropertyType::Int32, and its value is the number of
    ///     features unlocked.</li>
    ///   <li>For any of the above commands, if the property type is 
    ///     MgPropertyType::String, it indicates an exception had occurred
    ///     for that particular command, and its value is the exception message.
    ///     This only applies when the operation is not using a transaction</li>
    /// </ul>
    ///
    /// \exception MgFeatureServiceException
    /// \exception MgInvalidArgumentException
    /// \exception MgInvalidOperationException
    /// \exception MgFdoException
    ///
    virtual MgPropertyCollection* UpdateFeatures(MgFeatureCommandCollection* commands);

    ////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Starts a transaction on the this layer. The FDO provider indicated by the layer's 
    /// Feature Source must support transactions.
    ///
    /// \remarks
    /// The XML returned by MgFeatureService::GetCapabilities says
    /// whether a provider supports transactions. See \link ProviderCapabilities Provider Capabilities \endlink.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgTransaction BeginTransaction();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgTransaction BeginTransaction();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgTransaction BeginTransaction();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns an MgTransaction instance (or NULL).
    ///
    /// \exception MgFeatureServiceException
    /// \exception MgInvalidArgumentException
    /// \exception MgInvalidOperationException
    /// \exception MgFdoException
    ///
    /// \since 3.0
    virtual MgTransaction* BeginTransaction();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Executes the MgDeleteFeatures, MgInsertFeatures,
    /// MgUpdateFeatures, MgLockFeatures or MgUnlockFeatures commands
    /// contained in the given MgFeatureCommandCollection object.
    ///
    /// \remarks
    /// The XML returned by MgFeatureService::GetCapabilities says
    /// whether a provider supports SQL commands. See \link ProviderCapabilities Provider Capabilities \endlink.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgPropertyCollection UpdateFeatures(MgFeatureCommandCollection commands);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgPropertyCollection UpdateFeatures(MgFeatureCommandCollection commands);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgPropertyCollection UpdateFeatures(MgFeatureCommandCollection commands);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param commands (MgFeatureCommandCollection)
    /// A collection of feature commands to be
    /// executed.
    /// \param transaction (MgTransaction)
    /// An optional transaction to execute this operation under
    ///
    /// \return
    /// Returns an MgPropertyCollection object. Each property in the
    /// collection corresponds to a command in the
    /// MgFeatureCommandCollection argument. The property name is the
    /// index of the command in the feature command collection.
    /// <ul>
    ///   <li>If the command is of type MgDeleteFeatures, the property
    ///     type is an MgPropertyType::Int32, and its value is the number
    ///     of features deleted.</li>
    ///   <li>If the command is of type MgInsertFeatures, the property
    ///     type is an MgPropertyType::Feature, and its value is a
    ///     MgFeatureReader object. The feature reader object contains
    ///     the set of properties inserted into the datastore by the
    ///     insert command.</li>
    ///   <li>If the command is of type MgUpdateFeatures, the property
    ///     type is MgPropertyType::Int32, and its value is the number of
    ///     features updated.</li>
    ///   <li>If the command is of type MgLockFeatures, the property
    ///     type is MgPropertyType::Feature, and its value is the number
    ///     of features locked.</li>
    ///   <li>If the command is of type MgUnLockFeatures, the property
    ///     type is MgPropertyType::Int32, and its value is the number of
    ///     features unlocked.</li>
    /// </ul>
    ///
    /// \exception MgFeatureServiceException
    /// \exception MgInvalidArgumentException
    /// \exception MgInvalidOperationException
    /// \exception MgFdoException
    ///
    /// \since 3.0
    virtual MgPropertyCollection* UpdateFeatures(MgFeatureCommandCollection* commands, MgTransaction* transaction);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Inserts a new feature into the specified feature class of the specified Feature Source
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgFeatureReader InsertFeatures(MgPropertyCollection properties);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgFeatureReader InsertFeatures(MgPropertyCollection properties);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgFeatureReader InsertFeatures(MgPropertyCollection properties);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param properties (MgPropertyCollection)
    /// The collection of property values to insert
    /// \param transaction (MgTransaction)
    /// An optional transaction to execute this operation under
    ///
    /// \return
    /// Returns a feature reader object that contains the set of properties 
    /// inserted into the datastore by the insert command.
    ///
    /// \remarks
    /// Remember to close any feature readers returned by this method, even if you don't intend
    /// to do anything with them
    ///
    /// \exception MgFeatureServiceException
    /// \exception MgInvalidArgumentException
    /// \exception MgInvalidOperationException
    /// \exception MgFdoException
    ///
    /// \since 3.0
    virtual MgFeatureReader* InsertFeatures(MgPropertyCollection* properties, MgTransaction* transaction);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Inserts a set of new features into the specified feature class of the specified Feature Source
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgFeatureReader InsertFeatures(MgBatchPropertyCollection properties);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgFeatureReader InsertFeatures(MgBatchPropertyCollection properties);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgFeatureReader InsertFeatures(MgBatchPropertyCollection properties);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param properties (MgBatchPropertyCollection)
    /// The collection of property values to insert. Each MgPropertyCollection within 
    /// this collection represents property values for a single feature to insert
    /// \param transaction (MgTransaction)
    /// An optional transaction to execute this operation under
    ///
    /// \return
    /// Returns a feature reader object that contains the set of properties 
    /// inserted into the datastore by the insert command.
    ///
    /// \remarks
    /// Remember to close any feature readers returned by this method, even if you don't intend
    /// to do anything with them
    ///
    /// \exception MgFeatureServiceException
    /// \exception MgInvalidArgumentException
    /// \exception MgInvalidOperationException
    /// \exception MgFdoException
    ///
    /// \since 3.0
    virtual MgFeatureReader* InsertFeatures(MgBatchPropertyCollection* properties, MgTransaction* transaction);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Updates all features that match the given filter with the specified property values
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int UpdateMatchingFeatures(MgPropertyCollection properties, string filter);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int UpdateMatchingFeatures(MgPropertyCollection properties, String filter);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int UpdateMatchingFeatures(MgPropertyCollection properties, String filter);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param properties (MgBatchPropertyCollection)
    /// The property values to update matching features with
    /// \param filter (String/string)
    /// The FDO filter string that detemines what features will be updated
    /// \param transaction (MgTransaction)
    /// An optional transaction to execute this operation under
    ///
    /// \return
    /// Returns the number of features updated by this operation
    ///
    /// \exception MgFeatureServiceException
    /// \exception MgInvalidArgumentException
    /// \exception MgInvalidOperationException
    /// \exception MgFdoException
    ///
    /// \since 3.0
    virtual INT32 UpdateMatchingFeatures(MgPropertyCollection* properties, CREFSTRING filter, MgTransaction* transaction);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Deletes all features that match the given filter
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int DeleteFeatures(string filter);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int DeleteFeatures(String filter);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int DeleteFeatures(String filter);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param filter (String/string)
    /// The FDO filter string that detemines what features will be deleted
    /// \param transaction (MgTransaction)
    /// An optional transaction to execute this operation under
    ///
    /// \return
    /// Returns the number of features deleted by this operation
    ///
    /// \exception MgFeatureServiceException
    /// \exception MgInvalidArgumentException
    /// \exception MgInvalidOperationException
    /// \exception MgFdoException
    ///
    /// \since 3.0
    virtual INT32 DeleteFeatures(CREFSTRING filter, MgTransaction* transaction);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the list of geometry type styles for this layer at the map's current scale. Returns NULL if there are no applicable geometry types
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgIntCollection GetGeometryTypeStyles();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgIntCollection GetGeometryTypeStyles();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgIntCollection GetGeometryTypeStyles();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \remarks
    /// The map's current scale is used to determine what scale range in the layer definition to search for.
    /// For a scale range with multiple composite styles, multiple instances of (4 = composite) will be in the resulting collection
    ///
    /// \return
    /// The list of geometry type styles for this layer at the map's current scale. Returns NULL if there are no applicable geometry types
    ///
    /// \since 3.0
    MgIntCollection* GetGeometryTypeStyles();

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of theme categories for this layer at the map's current scale for the given geometry type style. A count greater than 1 indicates a themed layer. Returns -1 if there are no applicable styles at the current scale
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetThemeCategoryCount(int geomType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetThemeCategoryCount(int geomType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetThemeCategoryCount(int geomType);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param geomType (int)
    /// The geometry type
    ///
    /// \remarks
    /// The map's current scale is used to determine what scale range in the layer definition to search for.
    /// When geomType = 4, it will only count the number of the theme categories for the first composite style it finds. For a scale range
    /// with multiple composite type styles, you should use GetCompositeThemeCategoryCount() instead
    ///
    /// \return
    /// The number of theme categories for this layer at the map's current scale for the given geometry type style. A count greater than 1 indicates a themed layer. Returns -1 if there are no applicable styles at the current scale
    ///
    /// \since 3.0
    INT32 GetThemeCategoryCount(INT32 geomType);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of composite theme categories for this layer at the map's current scale for the given composite style. A count greater than 1 indicates a themed layer. Returns -1 if there are no applicable styles at the current scale
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetThemeCategoryCount(double scale, int geomType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetThemeCategoryCount(double scale, int geomType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetThemeCategoryCount(double scale, int geomType);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param compositeOffset (int)
    /// The zero-based index denoting the particular composite style to count from. 0 = 1st composite style, 1 = 2nd composite style
    ///
    /// \return
    /// The number of theme categories for this layer at the map's current scale for the given composite style. A count greater than 1 indicates a themed layer. Returns -1 if there are no applicable styles at the current scale
    ///
    /// \since 3.0
    INT32 GetCompositeThemeCategoryCount(INT32 compositeOffset);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the legend image for the specified geometry type and theme category
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader GenerateLegendImage(int width, int height, string format, int geomType, int themeCategory);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader GenerateLegendImage(int width, int height, String format, int geomType, int themeCategory);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader GenerateLegendImage(int width, int height, string format, int geomType, int themeCategory);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Input
    /// MgResourceIdentifier object identifying the layer definition resource.
    /// \param scale (double)
    /// Input
    /// The scale at which the symbolization is requested.
    /// \param width (int)
    /// Input
    /// The requested image width in pixels.
    /// \param height (int)
    /// Input
    /// The requested image height in pixels.
    /// \param format (String/string)
    /// Input
    /// Image format, from MgImageFormats. Example: PNG, JPG, PNG8, etc 
    /// \param geomType (int)
    /// Input
    /// The type of symbolization required: 1=Point, 2=Line, 3=Area, 4=Composite
    /// \param themeCategory (int)
    /// Input
    /// The value indicating which theme category swatch to return.
    /// Used when there is a theme defined at this scale. An exception will be
    /// thrown if a requested them category doesn't exist.
    ///
    /// \remarks
    /// The map's current scale is used to determine what scale range in the layer definition to search for
    ///
    /// \return
    /// Returns a stream representing the legend image.
    /// The default returned image format will be PNG8 unless a different supported
    /// format is requested. An exception will be thrown if an unsupported image
    /// format is requested.
    ///
    /// \exception MgArgumentOutOfRangeException
    /// \exception MgInvalidResourceTypeException
    /// \exception MgNullArgumentException
    /// \exception MgInvalidImageFormatException
    ///
    /// \since 3.0
    MgByteReader* GenerateLegendImage(INT32 width, INT32 height, CREFSTRING format, INT32 geomType, INT32 themeCategory);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the list of geometry type styles for this layer at the map's current scale. Returns NULL if there are no applicable geometry types
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgIntCollection GetGeometryTypeStyles(double scale);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgIntCollection GetGeometryTypeStyles(double scale);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgIntCollection GetGeometryTypeStyles(double scale);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param scale (double)
    /// The scale at which to retrive the list of applicable geometry types
    ///
    /// \remarks
    /// For a scale range with multiple composite styles, multiple instances of (4 = composite) will be in the resulting collection
    ///
    /// \return
    /// The list of geometry type styles for this layer at the map's current scale. Returns NULL if there are no applicable geometry types
    ///
    /// \since 3.0
    MgIntCollection* GetGeometryTypeStyles(double scale);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of theme categories for this layer at the map's current scale for the given geometry type style. A count greater than 1 indicates a themed layer. Returns -1 if there are no applicable styles at the current scale
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetThemeCategoryCount(double scale, int geomType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetThemeCategoryCount(double scale, int geomType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetThemeCategoryCount(double scale, int geomType);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param scale (double)
    /// The scale at which to count the number of applicable theme categories
    /// \param geomType (int)
    /// The geometry type
    ///
    /// \remarks
    /// When geomType = 4, it will only count the number of the theme categories for the first composite style it finds. For a scale range
    /// with multiple composite type styles, you should use GetCompositeThemeCategoryCount() instead
    ///
    /// \return
    /// The number of theme categories for this layer at the map's current scale for the given geometry type style. A count greater than 1 indicates a themed layer. Returns -1 if there are no applicable styles at the current scale
    ///
    /// \since 3.0
    INT32 GetThemeCategoryCount(double scale, INT32 geomType);

    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the number of composite theme categories for this layer at the map's current scale for the given composite style. A count greater than 1 indicates a themed layer. Returns -1 if there are no applicable styles at the current scale
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual int GetThemeCategoryCount(double scale, int geomType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual int GetThemeCategoryCount(double scale, int geomType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual int GetThemeCategoryCount(double scale, int geomType);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param scale (double)
    /// The scale at which to count the number of applicable theme categories
    /// \param compositeOffset (int)
    /// The zero-based index denoting the particular composite style to count from. 0 = 1st composite style, 1 = 2nd composite style
    ///
    /// \return
    /// The number of theme categories for this layer at the map's current scale for the given composite style. A count greater than 1 indicates a themed layer. Returns -1 if there are no applicable styles at the current scale
    ///
    /// \since 3.0
    INT32 GetCompositeThemeCategoryCount(double scale, INT32 compositeOffset);

    ////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the legend image for the specified geometry type and theme category
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgByteReader GenerateLegendImage(double scale, int width, int height, string format, int geomType, int themeCategory);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgByteReader GenerateLegendImage(double scale, int width, int height, String format, int geomType, int themeCategory);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgByteReader GenerateLegendImage(double scale, int width, int height, string format, int geomType, int themeCategory);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param resource (MgResourceIdentifier)
    /// Input
    /// MgResourceIdentifier object identifying the layer definition resource.
    /// \param scale (double)
    /// Input
    /// The scale at which the symbolization is requested.
    /// \param width (int)
    /// Input
    /// The requested image width in pixels.
    /// \param height (int)
    /// Input
    /// The requested image height in pixels.
    /// \param format (String/string)
    /// Input
    /// Image format, from MgImageFormats. Example: PNG, JPG, PNG8, etc 
    /// \param geomType (int)
    /// Input
    /// The type of symbolization required: 1=Point, 2=Line, 3=Area, 4=Composite
    /// \param themeCategory (int)
    /// Input
    /// The value indicating which theme category swatch to return.
    /// Used when there is a theme defined at this scale. An exception will be
    /// thrown if a requested them category doesn't exist.
    ///
    /// \return
    /// Returns a stream representing the legend image.
    /// The default returned image format will be PNG8 unless a different supported
    /// format is requested. An exception will be thrown if an unsupported image
    /// format is requested.
    ///
    /// \exception MgArgumentOutOfRangeException
    /// \exception MgInvalidResourceTypeException
    /// \exception MgNullArgumentException
    /// \exception MgInvalidImageFormatException
    ///
    /// \since 3.0
    MgByteReader* GenerateLegendImage(double scale, INT32 width, INT32 height, CREFSTRING format, INT32 geomType, INT32 themeCategory);

    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets all of the spatial contexts available in the layer or just the active one
    ///
    /// \remarks
    /// The \link FdoSpatialContextList_schema FdoSpatialContextList \endlink XML schema contains
    /// a specification of the content of the spatial context
    /// information returned in the MgSpatialContextReader object.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// virtual MgSpatialContextReader GetSpatialContexts(bool bActiveOnly);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// virtual MgSpatialContextReader GetSpatialContexts(boolean bActiveOnly);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// virtual MgSpatialContextReader GetSpatialContexts(bool bActiveOnly);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param bActiveOnly (boolean/bool)
    /// This flag is obsolete and no longer used.
    ///
    /// \return
    /// Returns an MgSpatialContextReader object.
    ///
    /// \exception MgFeatureServiceException
    /// \exception MgInvalidArgumentException
    /// \exception MgInvalidOperationException
    /// \exception MgFdoException
    ///
    /// \since 3.0
    virtual MgSpatialContextReader* GetSpatialContexts(bool bActiveOnly);

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates an empty MgLayer object
    ///
    MgLayer();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates an MgLayer object and pulls identity properties if required
    ///
    MgLayer(MgResourceIdentifier* layerDefinition, MgResourceService* resourceService, bool initIdProps);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates an MgLayerBase object from a layer definition, pulls identity properties if required, and init layer definition if required.
    ///
    MgLayer(MgResourceIdentifier* layerDefinition, MgResourceService* resourceService, bool initIdProps, bool initLayerDefinition);

    //////////////////////////////////////////////////////////////////
    /// Parse the layer definition XML and extracts scale ranges,
    /// feature source and feature class from it
    void GetLayerInfoFromDefinition(MgResourceService* resourceService);

    //////////////////////////////////////////////////////////////////
    /// Populate identity properties using a class definition collection
    void PopulateIdentityProperties(MgClassDefinition* classDef);

    ////////////////////////////////////////////////////////////////////
    /// Parse feature name to populate schema name and return class name
    void ParseFeatureName(MgFeatureService* featureService, REFSTRING className, REFSTRING schemaName);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serialize data to a stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserialize data from a stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

protected:
    /// \brief
    /// Destruct a MgLayer object
    ///
    /// \return
    /// Nothing
    ///
    virtual ~MgLayer();

    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifier.
    ///
    virtual INT32 GetClassId()
    {
        return m_cls_id;
    }


    /// \brief
    /// Dispose this object.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual void Dispose()
    {
        delete this;
    }

private:
    STRING GetProviderName();
    bool UseTransaction(MgFeatureService* svcFeature, CREFSTRING providerName);

    MgMapBase* GetMap();

    bool m_initIdProps;

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_MapLayer_Layer;

};
/// \}

#endif
