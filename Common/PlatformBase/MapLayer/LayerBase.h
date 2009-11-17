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

#ifndef _MG_LAYER_BASE_H_
#define _MG_LAYER_BASE_H_

BEGIN_NAMESPACE_MDFMODEL
class LayerDefinition;
END_NAMESPACE_MDFMODEL

class MgLayerGroup;
class MgLayerCollection;
class MgLayerBase;
template class MG_PLATFORMBASE_API Ptr<MgLayerBase>;

#ifndef TEMPL_INST_LAYER_GROUP
#define TEMPL_INST_LAYER_GROUP
template class MG_PLATFORMBASE_API Ptr<MgLayerGroup>;
#endif

#ifdef _WIN32
#pragma warning(disable:4251)
#endif

typedef vector<double> SCALERANGES;

/// \defgroup MgLayerBase MgLayerBase
/// \ingroup Maps_and_Layers_Module
/// \{

/////////////////////////////////////////////////////////////////
/// \brief
/// Gets and sets properties of a layer.
/// \todo
class MG_PLATFORMBASE_API MgLayerBase : public MgNamedSerializable
{
    MG_DECL_DYNCREATE()
    DECLARE_CLASSNAME(MgLayerBase)

PUBLISHED_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates an MgLayerBase object from a layer definition.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgLayerBase(MgResourceIdentifier layerDefinition, MgResourceService resourceService);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgLayerBase(MgResourceIdentifier layerDefinition, MgResourceService resourceService);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgLayerBase(MgResourceIdentifier layerDefinition, MgResourceService resourceService);
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
    MgLayerBase(MgResourceIdentifier* layerDefinition, MgResourceService* resourceService);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the name of the layer.
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
    /// Returns the name of the layer as a string.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See \link MgMapBase MgMapBase class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    virtual STRING GetName();  /// __get, __set

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the name of the layer.
    ///
    /// \remarks
    /// The name must be unique in MgMapBase. An exception will be thrown
    /// when this layer is added to MgMapBase if the name already exists.
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
    /// Layer name.
    ///
    virtual void SetName(CREFSTRING name);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a unique identifier for this layer.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetObjectId();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetObjectId();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetObjectId();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the unique identifier of this map as a string.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See \link MgMapBase MgMapBase class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    virtual STRING GetObjectId();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the type of the layer.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int GetLayerType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int GetLayerType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int GetLayerType();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the type of the layer.
    ///
    virtual INT32 GetLayerType();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the group this layer is organized within.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgLayerGroup GetGroup();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgLayerGroup GetGroup();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgLayerGroup GetGroup();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns an MgLayerGroup representing the parent group, or
    /// NULL if the layer does not belong to a group.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See \link MgMapBase MgMapBase class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    /// \todo
    /// [[Job for Philip: I need to clarify how groups and layers are
    /// stored internally, versus how they are shown in e.g.
    /// Studio.]]
    ///
    virtual MgLayerGroup* GetGroup();  /// __get, __set

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Specifies which group this layer is organized within.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetGroup(MgLayerGroup group);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetGroup(MgLayerGroup group);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetGroup(MgLayerGroup group);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param group (MgLayerGroup)
    /// Group this layer is organized within, or NULL to
    /// have the layer not belong to any group.
    ///
    /// \return
    /// Returns nothing.
    ///
   virtual void SetGroup(MgLayerGroup* group);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines whether the data for this layer will be refreshed
    /// during the next map update.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool NeedsRefresh();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean NeedsRefresh();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool NeedsRefresh();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns true if the layer will be refreshed, or false
    /// otherwise.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See \link MgMapBase MgMapBase class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    virtual bool NeedsRefresh();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Forces the data for this layer to be refreshed during the
    /// next map update.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void ForceRefresh();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void ForceRefresh();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void ForceRefresh();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns nothing.
    ///
    virtual void ForceRefresh();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines whether the layer is potentially visible.
    ///
    /// \remarks
    /// Note that this method may return true even though the layer
    /// is not visible. This will occur if the visible flag of one of
    /// the groups this layer is organized within is not visible or
    /// when the current viewScale property of the map is outside the
    /// scale ranges defined for this layer.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool GetVisible();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean GetVisible();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool GetVisible();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns true if the layer is potentially visible, or false
    /// otherwise.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See \link MgMapBase MgMapBase class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    virtual bool GetVisible();  /// __get, __set

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the layer's potential visibility.
    ///
    /// \remarks
    /// Note that setting this property to true will not necessarily
    /// guarantee the layer will be visible. The layer will be
    /// visible only when this property is true, all groups this
    /// layer is organized within are visible, and the current
    /// viewScale property of the map is within the scale ranges
    /// defined for this layer.
    /// Note also that the visibility of base map layers (those of
    /// type MgLayerType::BaseMap) cannot be changed.  Attempting to
    /// do so will throw an MgInvalidOperationException.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetVisible(bool visible);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetVisible(boolean visible);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetVisible(bool visible);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param visible (boolean/bool)
    /// True if the layer should be visible, false
    /// otherwise.
    ///
    /// \return
    /// Returns nothing.
    ///
    virtual void SetVisible(bool visible);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the actual visibility of the layer.
    ///
    /// \remarks
    /// The visibility depends on the visible property of the layer,
    /// the visible property of each group this layer is organized
    /// within, and the current viewScale of the map.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool IsVisible();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean IsVisible();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool IsVisible();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns true if the layer is actually visible, or false
    /// otherwise.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See \link MgMapBase MgMapBase class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    virtual bool IsVisible();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines whether features on the layer are selectable.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool GetSelectable();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean GetSelectable();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool GetSelectable();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns true if features are selectable on this layer, or
    /// false otherwise.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See \link MgMapBase MgMapBase class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    /// \todo
    /// [[Job for Philip: Do I have that in the example????]]
    ///
    virtual bool GetSelectable();  /// __get, __set

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Specifies whether features on the layer are selectable.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetSelectable(bool selectable);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetSelectable(boolean selectable);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetSelectable(bool selectable);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param selectable (boolean/bool)
    /// True if features should be selectable on this
    /// layer, false otherwise.
    ///
    /// \return
    /// Returns nothing.
    ///
    virtual void SetSelectable(bool selectable);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the layer's definition.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgResourceIdentifier GetLayerDefinition();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgResourceIdentifier GetLayerDefinition();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgResourceIdentifier GetLayerDefinition();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the resource identifier of the layer's definition.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See \link MgMapBase MgMapBase class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    virtual MgResourceIdentifier* GetLayerDefinition();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the the layer definition for this layer.
    ///
    /// \remarks
    /// This method can be used to alter the definition of a layer,
    /// including the layer's data source definition or stylization
    /// rules. For example to change the Filter used to retrieve
    /// features on a FeatureLayer, the old definition can be
    /// retrieved, altered, and stored in the session repository.
    /// Then the SetLayerDefinition method can be called with the
    /// ResourceId of the altered layer definition in the session
    /// repository.
    ///
    /// \note
    /// If you change the layer's definition, you must also
    /// change the cached scale ranges and force an update of any
    /// layer metadata held in the DWF Viewer's eMap.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetLayerDefinition(MgResourceIdentifier layerDefinition, MgResourceService resourceService);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetLayerDefinition(MgResourceIdentifier layerDefinition, MgResourceService resourceService);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetLayerDefinition(MgResourceIdentifier layerDefinition, MgResourceService resourceService);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param layerDefinition (MgResourceIdentifier)
    /// Identifier of the layer definition in a
    /// resource repository
    /// \param resourceService (MgResourceService)
    /// An MgResourceService that can be used to
    /// retrieve the layer definition.
    ///
    /// \return
    /// Returns nothing.
    ///
    /// \todo
    ///   * [[Job for Philip: I need an example showing that.]]
    ///
    virtual void SetLayerDefinition(MgResourceIdentifier* layerDefinition, MgResourceService* resourceService);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines whether an entry for the layer should appear in
    /// the legend.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool GetDisplayInLegend();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean GetDisplayInLegend();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool GetDisplayInLegend();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns true if the layer should appear in the legend, or
    /// false otherwise.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See \link MgMapBase MgMapBase class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    virtual bool GetDisplayInLegend();  /// __get, __set

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Specifies whether an entry for the layer should appear in the
    /// legend.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetDisplayInLegend(bool displayInLegend);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetDisplayInLegend(boolean displayInLegend);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetDisplayInLegend(bool displayInLegend);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param displayInLegend (boolean/bool)
    /// True if the layer should appear in the
    /// legend, false otherwise.
    ///
    /// \return
    /// Returns nothing.
    ///
    virtual void SetDisplayInLegend(bool displayInLegend);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines whether the entry for the layer should initially
    /// be expanded in the legend, if possible.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// bool GetExpandInLegend();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// boolean GetExpandInLegend();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// bool GetExpandInLegend();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns true if the layer should initially be expanded in the legend, or
    /// false otherwise.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See \link MgMapBase MgMapBase class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    virtual bool GetExpandInLegend();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the label to be displayed for the layer in the legend.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetLegendLabel();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetLegendLabel();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetLegendLabel();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the legend label as a string.
    ///
    /// <!-- Example (PHP) -->
    /// \htmlinclude PHPExampleTop.html
    /// See \link MgMapBase MgMapBase class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    virtual STRING GetLegendLabel();  /// __get, __set

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the label to be displayed for the layer in the legend.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetLegendLabel(string legendLabel);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetLegendLabel(String legendLabel);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetLegendLabel(string legendLabel);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param legendLabel (String/string)
    /// Legend label string.
    ///
    /// \return
    /// Returns nothing.
    ///
    virtual void SetLegendLabel(CREFSTRING legendLabel);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the feature source associated with this layer
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetFeatureSourceId();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetFeatureSourceId();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetFeatureSourceId();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the feature source or an empty string if this layer is not a feature layer
    /// false otherwise.
    ///
    virtual STRING GetFeatureSourceId();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the feature class name associated with this layer
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetFeatureClassName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetFeatureClassName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetFeatureClassName();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the feature class name or an empty string if this layer is not a feature layer
    /// false otherwise.
    ///
    virtual STRING GetFeatureClassName();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the filter associated with this layer
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetFilter();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetFilter();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetFilter();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the filter or an empty string if this layer is not a feature layer or doesn't have filter.
    ///
    virtual STRING GetFilter();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the geometry name associated with this layer which is part of
    /// class definition
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetFeatureGeometryName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetFeatureGeometryName();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetFeatureGeometryName();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns the feature geometry name or an empty string if this layer is not a feature layer
    ///
    virtual STRING GetFeatureGeometryName();  /// _get

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
    /// </ul>
    ///
    /// \exception MgFeatureServiceException
    /// \exception MgInvalidArgumentException
    /// \exception MgInvalidOperationException
    /// \exception MgFdoException
    ///
    virtual MgPropertyCollection* UpdateFeatures(MgFeatureCommandCollection* commands);

INTERNAL_API:

    static MdfModel::LayerDefinition* GetLayerDefinition(MgResourceService* svcResource, MgResourceIdentifier* resId);
    static MdfModel::LayerDefinition* GetLayerDefinition(CREFSTRING resourceContent);

    //////////////////////////////////////////////////////////////////
    /// Internal mapping of identifiers for selection purposes
    /// feature source and feature class from it
    struct IdProperty
    {
        INT16 type;
        STRING name;
    };

    typedef std::list<IdProperty> IdPropertyList;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates an empty MgLayerBase object
    ///
    MgLayerBase();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates an MgLayerBase object from a layer definition and init layer definition if required.
    ///
    MgLayerBase(MgResourceIdentifier* layerDefinition, MgResourceService* resourceService, bool initLayerDefinition);

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

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Does it allow to set the name
    ///
    /// \return
    /// true - allows to set the name
    /// false - can not set the name
    ///
    virtual bool CanSetName();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Attaches this layer to a set of layers
    ///
    virtual void SetContainer(MgLayerCollection* cont);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Specifies whether the entry for the layer should initially
    /// be expanded in the legend, if possible.
    ///
    virtual void SetExpandInLegend(bool expandInLegend);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the type of the layer
    ///
    virtual void SetLayerType(INT32 layerType);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the layer's potential visibility.  Unlike SetVisible,
    /// this method allows you to change the visibility of base
    /// layers.
    ///
    virtual void ForceVisible(bool visible);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Turn on/off the refresh flag for this layer
    ///
    virtual void ForceRefresh(bool on);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the zorder value for this layer
    ///
    virtual double GetDisplayOrder();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the zorder value for this layer
    ///
    virtual void SetDisplayOrder(double zorder);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the scale ranges for this layer
    ///
    virtual void SetScaleRanges(const SCALERANGES& scaleRanges);

    //////////////////////////////////////////////////////////////////
    /// Determine if this layer is visible at the specified scale
    virtual bool IsVisibleAtScale(double scale);

    //////////////////////////////////////////////////////////////////
    /// Determine if this layer has tooltips configured
    virtual bool HasTooltips();

    //////////////////////////////////////////////////////////////////
    /// Parse the layer definition XML and extracts scale ranges,
    /// feature source and feature class from it
    virtual void GetLayerInfoFromDefinition(MgResourceService* resourceService);

    //////////////////////////////////////////////////////////////////
    /// Returns id property list
    virtual IdPropertyList& GetIdPropertyList();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the layer's resource content.
    ///
    virtual STRING GetLayerResourceContent();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the resource content for this layer.
    ///
    virtual void SetLayerResourceContent(CREFSTRING resourceContent);

protected:
    /// \brief
    /// Destruct a MgLayerBase object
    ///
    /// \return
    /// Nothing
    ///
    virtual ~MgLayerBase();

    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
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

CLASS_ID:
    static const INT32 m_cls_id = PlatformBase_MapLayer_LayerBase;

protected:
    Ptr<MgResourceIdentifier> m_definition;
    STRING                m_resourceContent;
    STRING                m_name;
    STRING                m_objectId;
    INT32                 m_type;
    Ptr<MgLayerGroup>     m_group;
    bool                  m_visible;
    bool                  m_hasTooltips;
    bool                  m_selectable;
    bool                  m_displayInLegend;
    bool                  m_expandInLegend;
    STRING                m_legendLabel;
    bool                  m_needRefresh;
    MgLayerCollection*    m_layers;     //no need for ref counting here.
    double                m_displayOrder;
    SCALERANGES           m_scaleRanges;
    STRING                m_featureSourceId;
    STRING                m_featureName;
    STRING                m_schemaName;
    STRING                m_geometry;
    IdPropertyList        m_idProps;
    bool                  m_forceReadFromServer;
    STRING                m_filter;
};
/// \}

#endif
