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

#ifndef _MG_LAYER_H_
#define _MG_LAYER_H_

class MgLayerGroup;
class MgLayerCollection;
class MgLayer;
template class MG_SERVICE_API Ptr<MgLayer>;

#ifndef TEMPL_INST_LAYER_GROUP
#define TEMPL_INST_LAYER_GROUP
template class MG_SERVICE_API Ptr<MgLayerGroup>;
#endif

#ifdef WIN32
#pragma warning(disable:4251)
#endif

typedef vector<double> SCALERANGES;

/// \defgroup MgLayer MgLayer
/// \ingroup Mapping_Service_Module
/// \{

/////////////////////////////////////////////////////////////////
/// \brief
/// Gets and sets properties of a layer.
/// \todo
class MG_SERVICE_API MgLayer : public MgNamedSerializable
{
    DECLARE_CREATE_OBJECT()
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
    /// See \link MgMap MgMap Class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    STRING GetName();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the name of the layer.
    ///
    /// \remarks
    /// The name must be unique in MgMap. An exception will be thrown
    /// when this layer is added to MgMap if the name already exists.
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
    void SetName(CREFSTRING name);

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
    /// See \link MgMap MgMap Class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    STRING GetObjectId();

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
    INT32 GetLayerType();

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
    /// See \link MgMap MgMap Class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    /// \todo
    /// [[Job for Philip: I need to clarify how groups and layers are
    /// stored internally, versus how they are shown in e.g.
    /// Studio.]]
    ///
    MgLayerGroup* GetGroup();

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
   void SetGroup(MgLayerGroup* group);

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
    /// See \link MgMap MgMap Class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    bool NeedsRefresh();

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
    void ForceRefresh();

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
    /// See \link MgMap MgMap Class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    bool GetVisible();

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
    void SetVisible(bool visible);

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
    /// See \link MgMap MgMap Class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    bool IsVisible();

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
    /// See \link MgMap MgMap Class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    /// \todo
    /// [[Job for Philip: Do I have that in the example????]]
    ///
    bool GetSelectable();

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
    void SetSelectable(bool selectable);

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
    /// See \link MgMap MgMap Class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    MgResourceIdentifier* GetLayerDefinition();

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
    void SetLayerDefinition(MgResourceIdentifier* layerDefinition, MgResourceService* resourceService);

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
    /// See \link MgMap MgMap Class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    bool GetDisplayInLegend();

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
    void SetDisplayInLegend(bool displayInLegend);

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
    /// See \link MgMap MgMap Class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    bool GetExpandInLegend();

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
    /// See \link MgMap MgMap Class \endlink.
    /// \htmlinclude ExampleBottom.html
    ///
    STRING GetLegendLabel();

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
    void SetLegendLabel(CREFSTRING legendLabel);

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
    STRING GetFeatureSourceId();

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
    STRING GetFeatureClassName();

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
    STRING GetFeatureGeometryName();

INTERNAL_API:

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
    /// Creates an empty MgLayer object
    ///
    MgLayer();

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
    void SetContainer(MgLayerCollection* cont);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Specifies whether the entry for the layer should initially
    /// be expanded in the legend, if possible.
    ///
    void SetExpandInLegend(bool expandInLegend);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the type of the layer
    ///
    void SetLayerType(INT32 layerType);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the layer's potential visibility.  Unlike SetVisible,
    /// this method allows you to change the visibility of base
    /// layers.
    ///
    void ForceVisible(bool visible);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Turn on/off the refresh flag for this layer
    ///
    void ForceRefresh(bool on);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the zorder value for this layer
    ///
    double GetDisplayOrder();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the zorder value for this layer
    ///
    void SetDisplayOrder(double zorder);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the scale ranges for this layer
    ///
    void SetScaleRanges(const SCALERANGES& scaleRanges);

    //////////////////////////////////////////////////////////////////
    /// Determine if this layer is visible at the specified scale
    bool IsVisibleAtScale(double scale);

    //////////////////////////////////////////////////////////////////
    /// Parse the layer definition XML and extracts scale ranges,
    /// feature source and feature class from it
    void GetLayerInfoFromDefinition(MgResourceService* resourceService);

    //////////////////////////////////////////////////////////////////
    /// Returns id property list
    IdPropertyList& GetIdPropertyList();

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
    static const INT32 m_cls_id = MapLayer_Layer;

private:
    Ptr<MgResourceIdentifier> m_definition;
    STRING                m_name;
    STRING                m_objectId;
    INT32                 m_type;
    Ptr<MgLayerGroup>     m_group;
    bool                  m_visible;
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
    STRING                m_geometry;
    IdPropertyList        m_idProps;
};
/// \}

#endif
