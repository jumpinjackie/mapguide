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

#include "PlatformBase.h"
#include "System/XmlDefs.h"
#include "System/XmlUtil.h"
#include "VectorScaleRange.h"
#include "GridScaleRange.h"
#include "SAX2Parser.h"
#include "VectorLayerDefinition.h"
#include "DrawingLayerDefinition.h"
#include "GridLayerDefinition.h"

MG_IMPL_DYNCREATE(MgLayerBase)

//////////////////////////////////////////////////////////////
// Creates a MgLayerBase object
//
MgLayerBase::MgLayerBase()
    : m_type(MgLayerType::Dynamic),
      m_group((MgLayerGroup*)NULL),
      m_visible(true),
      m_selectable(false),
      m_hasTooltips(false),
      m_displayInLegend(false),
      m_expandInLegend(false),
      m_needRefresh(false),
      m_layers(NULL),
      m_displayOrder(0.0),
      m_resourceContent(L""),
      m_forceReadFromServer(false)
{
}


//////////////////////////////////////////////////////////////
// Creates a MgLayerBase object given a reference to a LayerDefinition
//
MgLayerBase::MgLayerBase(MgResourceIdentifier* layerDefinition, MgResourceService* resourceService)
    : m_type(MgLayerType::Dynamic),
      m_group((MgLayerGroup*)NULL),
      m_visible(true),
      m_selectable(false),
      m_hasTooltips(false),
      m_displayInLegend(false),
      m_expandInLegend(false),
      m_needRefresh(false),
      m_layers(NULL),
      m_displayOrder(0.0),
      m_resourceContent(L""),
      m_forceReadFromServer(false)
{
    m_definition = layerDefinition;
    if(SAFE_ADDREF((MgResourceIdentifier*)m_definition) != NULL)
        m_name = m_definition->GetName();

    GetLayerInfoFromDefinition(resourceService);

    //Generate a unique id for this layer
    MgUtil::GenerateUuid(m_objectId);
}

//////////////////////////////////////////////////////////////
// Creates an MgLayerBase object from a layer definition and init layer definition if required.
//
MgLayerBase::MgLayerBase(MgResourceIdentifier* layerDefinition, MgResourceService* resourceService, bool initLayerDefinition)
    : m_type(MgLayerType::Dynamic),
    m_group((MgLayerGroup*)NULL),
    m_visible(true),
    m_selectable(false),
    m_hasTooltips(false),
    m_displayInLegend(false),
    m_expandInLegend(false),
    m_needRefresh(false),
    m_layers(NULL),
    m_displayOrder(0.0),
    m_resourceContent(L""),
    m_forceReadFromServer(false)
{
    m_definition = layerDefinition;
    if(SAFE_ADDREF((MgResourceIdentifier*)m_definition) != NULL)
        m_name = m_definition->GetName();

    if(initLayerDefinition)
        GetLayerInfoFromDefinition(resourceService);

    //Generate a unique id for this layer
    MgUtil::GenerateUuid(m_objectId);
}


//////////////////////////////////////////////////////////////
// Returns the name of the layer
//
STRING MgLayerBase::GetName()
{
    return m_name;
}


//////////////////////////////////////////////////////////////
// Returns an identifier that can uniquely identify this layer
//
STRING MgLayerBase::GetObjectId()
{
    return m_objectId;  // what is this?
}


//////////////////////////////////////////////////////////////
// Returns the type of the layer
//
INT32 MgLayerBase::GetLayerType()
{
    return m_type;
}


//////////////////////////////////////////////////////////////
// Sets the type of the layer
//
void MgLayerBase::SetLayerType(INT32 layerType)
{
    m_type = layerType;
}


//////////////////////////////////////////////////////////////
// Returns the group this layer is organized within
//
MgLayerGroup* MgLayerBase::GetGroup()
{
    return SAFE_ADDREF((MgLayerGroup*)m_group);
}


//////////////////////////////////////////////////////////////
// Changes the group this layer is organized within.
// Specifying a null LayerGroup indicates the layer should not reside within a group
//
void MgLayerBase::SetGroup(MgLayerGroup* group)
{
    if(m_group != group)
    {
        m_group = SAFE_ADDREF((MgLayerGroup*)group);

        if(m_layers != NULL)
            m_layers->GetMap()->OnLayerParentChanged(this, group != NULL? group->GetObjectId(): L"");
    }
}


//////////////////////////////////////////////////////////////
// Returns a flag indicating if whether or not the data for this layer
// will be refreshed during the next map update.
//
bool MgLayerBase::NeedsRefresh()
{
    return m_needRefresh;
}


//////////////////////////////////////////////////////////////
// Forces the data for this layer to be refreshed during the next map update
//
void MgLayerBase::ForceRefresh()
{
    ForceRefresh(true);
}


//////////////////////////////////////////////////////////////
// Returns the current state of the visible property.
// Note that this method may return true even though the layer is not visible.
// This will occur if the visible flag of one of the groups this layer
// is organized within is not visible or when the current viewScale property of the map
// is outside the scale ranges defined for this layer.
//
bool MgLayerBase::GetVisible()
{
    return m_visible;
}


//////////////////////////////////////////////////////////////
// Alters the layers potential visibility.
// Note that setting this property to true will not necessarily guarantee the layer will be visible.
// The layer will be visible only when this property is true, all groups this layer
// is organized within are visible, and the current viewScale property of the map is within
// the scale ranges defined for this layer.
// Note also that the visibility of base map layers (those of type MgLayerType::BaseMap)
// cannot be changed.  Attempting to do so will throw an MgInvalidOperationException.
//
void MgLayerBase::SetVisible(bool visible)
{
    if (m_type == MgLayerType::BaseMap)
    {
        throw new MgInvalidOperationException(L"MgLayerBase.SetVisible",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    ForceVisible(visible);
}


//////////////////////////////////////////////////////////////
// Alters the layers potential visibility.  Unlike SetVisible,
// this method allows you to change the visibility of base layers.
//
void MgLayerBase::ForceVisible(bool visible)
{
    if(m_visible != visible)
    {
        m_visible = visible;

        if(m_layers != NULL)
            m_layers->GetMap()->OnLayerVisibilityChanged(this, visible? L"1": L"0");
    }
}


//////////////////////////////////////////////////////////////
// Returns the actual visibility of the layer based upon the visible property of Layer,
// the visible property of each group this layer is organized within, and the current viewScale of the Map
//
bool MgLayerBase::IsVisible()
{
    if(m_layers == NULL)
        return false;       //layer visible only when attached to a map

    return IsVisibleAtScale(m_layers->GetMap()->GetViewScale());
}


//////////////////////////////////////////////////////////////
// Indicates whether features on the layer are selectable.
//
bool MgLayerBase::GetSelectable()
{
    return m_selectable;
}


//////////////////////////////////////////////////////////////
// Specifies whether features on the layer are selectable.
//
void MgLayerBase::SetSelectable(bool selectable)
{
    if(m_selectable != selectable)
    {
        m_selectable = selectable;

        if(m_layers != NULL)
            m_layers->GetMap()->OnLayerSelectabilityChanged(this, selectable? L"1": L"0");
    }
}


//////////////////////////////////////////////////////////////
// Returns the resource identifier that refers to the layer's definition.
//
MgResourceIdentifier* MgLayerBase::GetLayerDefinition()
{
    return SAFE_ADDREF((MgResourceIdentifier*)m_definition);
}

//////////////////////////////////////////////////////////////
// Sets the layer definition for this layer.
// This method can be used to alter the definition of a layer, including the layers
// data source definition or stylization rules. For example to change the Filter
// used to retrieve features on a FeatureLayer, the old definition can be retrieved, altered,
// and stored in the session repository. Then the SetLayerDefinition method can be called
// with the ResourceId of the altered layer definition in the session repository
//
void MgLayerBase::SetLayerDefinition(MgResourceIdentifier* layerDefinition, MgResourceService* resourceService)
{
    if(layerDefinition == NULL)
    {
        throw new MgNullArgumentException(L"MgLayerBase.SetLayerDefinition", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if(m_definition != NULL && m_definition->ToString() == layerDefinition->ToString())
        return;

    m_definition = SAFE_ADDREF((MgResourceIdentifier*)layerDefinition);
    m_name = m_definition->GetName();

    m_forceReadFromServer = true;
    GetLayerInfoFromDefinition(resourceService);
    m_forceReadFromServer = false;

    if(m_layers != NULL)
        m_layers->GetMap()->OnLayerDefinitionChanged(this);
}

//////////////////////////////////////////////////////////////////
/// Gets the layer's resource content.
///
STRING MgLayerBase::GetLayerResourceContent()
{
    return m_resourceContent;
}

//////////////////////////////////////////////////////////////////
/// Sets the resource content for this layer.
///
void MgLayerBase::SetLayerResourceContent(CREFSTRING resourceContent)
{
    if(resourceContent == L"")
    {
        throw new MgNullArgumentException(L"MgLayerBase.SetLayerResourceContent", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if(m_resourceContent == resourceContent)
        return;

    m_resourceContent = resourceContent;

    GetLayerInfoFromDefinition(NULL);

    if(m_layers != NULL)
        m_layers->GetMap()->OnLayerDefinitionChanged(this);
}


//////////////////////////////////////////////////////////////
// Indicates whether an entry for the layer should appear in the legend.
//
bool MgLayerBase::GetDisplayInLegend()
{
    return m_displayInLegend;
}


//////////////////////////////////////////////////////////////
// Specifies whether an entry for the layer should appear in the legend.
//
void MgLayerBase::SetDisplayInLegend(bool displayInLegend)
{
    if(m_displayInLegend != displayInLegend)
    {
        m_displayInLegend = displayInLegend;

        if(m_layers != NULL)
            m_layers->GetMap()->OnLayerDisplayInLegendChanged(this, displayInLegend? L"1": L"0");
    }
}


//////////////////////////////////////////////////////////////
// Indicates whether the entry for the layer should initially be
// expanded in the legend, if possible.
//
bool MgLayerBase::GetExpandInLegend()
{
    return m_expandInLegend;
}


//////////////////////////////////////////////////////////////
// Specifies whether the entry for the layer should initially
// be expanded in the legend, if possible.
//
void MgLayerBase::SetExpandInLegend(bool expandInLegend)
{
    m_expandInLegend = expandInLegend;
}


//////////////////////////////////////////////////////////////
// Returns the label to be displayed for the layer in the legend
//
STRING MgLayerBase::GetLegendLabel()
{
    return m_legendLabel;
}


//////////////////////////////////////////////////////////////
// Sets the label to be displayed for the layer in the legend
//
void MgLayerBase::SetLegendLabel(CREFSTRING legendLabel)
{
    if(m_legendLabel != legendLabel)
    {
        m_legendLabel = legendLabel;

        if(m_layers != NULL)
            m_layers->GetMap()->OnLayerLegendLabelChanged(this, legendLabel);
    }
}


//////////////////////////////////////////////////////////////
// Gets the feature source associated with this layer
//
STRING MgLayerBase::GetFeatureSourceId()
{
    return m_featureSourceId;
}


//////////////////////////////////////////////////////////////
// Gets the feature class name associated with this layer
//
STRING MgLayerBase::GetFeatureClassName()
{
    return m_featureName;
}

//////////////////////////////////////////////////////////////
// Gets the filter associated with this layer
//
STRING MgLayerBase::GetFilter()
{
    return m_filter;
}


//////////////////////////////////////////////////////////////
// Serialize data to a stream
//
void MgLayerBase::Serialize(MgStream* stream)
{
    MG_TRY()
    throw new MgNotImplementedException(L"MgLayerBase.Serialize", __LINE__, __WFILE__, NULL, L"", NULL);
    MG_CATCH_AND_THROW(L"MgLayerBase.Serialize");
}


//////////////////////////////////////////////////////////////
// Deserialize data from a stream
//
void MgLayerBase::Deserialize(MgStream* stream)
{
    MG_TRY()
    throw new MgNotImplementedException(L"MgLayerBase.Deserialize", __LINE__, __WFILE__, NULL, L"", NULL);
    MG_CATCH_AND_THROW(L"MgLayerBase.Deserialize");
}


//////////////////////////////////////////////////////////////
// Does it allow to set the name
//
bool MgLayerBase::CanSetName()
{
    return true;
}


//////////////////////////////////////////////////////////////
// Destruct a MgLayerBase object
//
MgLayerBase::~MgLayerBase()
{
}


//////////////////////////////////////////////////////////////
// Attaches this layer to a set of layers
//
void MgLayerBase::SetContainer(MgLayerCollection* cont)
{
    m_layers = cont;
}


//////////////////////////////////////////////////////////////
// Turn on/off the refresh flag for this layer
//
void MgLayerBase::ForceRefresh(bool on)
{
    if(on && !IsVisible())
        return;
    m_needRefresh = on;
}


//////////////////////////////////////////////////////////////
// Gets the display order value for this layer
//
double MgLayerBase::GetDisplayOrder()
{
    return m_displayOrder;
}


//////////////////////////////////////////////////////////////
// Sets the display order value for this layer
//
void MgLayerBase::SetDisplayOrder(double displayOrder)
{
    m_displayOrder = displayOrder;
}


//////////////////////////////////////////////////////////////
// Sets the name of this layer
//
void MgLayerBase::SetName(CREFSTRING name)
{
    m_name = name;
}


//////////////////////////////////////////////////////////////
// Sets the scale ranges for this layer
//
void MgLayerBase::SetScaleRanges(const SCALERANGES& scaleRanges)
{
    m_scaleRanges.clear();
    m_scaleRanges = scaleRanges;
}


//////////////////////////////////////////////////////////////
// Determine if this layer is visible at the specified scale
//
bool MgLayerBase::IsVisibleAtScale(double scale)
{
    if(!m_visible)
        return false;

    //check visibility flags
    Ptr<MgLayerGroup> group = GetGroup();

    if(group && !group->IsVisible())
        return false;

    //check scale ranges
    if(m_scaleRanges.size() % 2)
        return false;   //something is wrong with the scale ranges...

    for(SCALERANGES::const_iterator it = m_scaleRanges.begin(); it != m_scaleRanges.end(); )
    {
        double minScale = *it ++;
        double maxScale = *it ++;

        if(scale >= minScale && scale < maxScale)
            return true;
    }
    return false;
}

//////////////////////////////////////////////////////////////
// Determine if this layer is visible at the specified scale
//
bool MgLayerBase::HasTooltips()
{
    return m_hasTooltips;
}

//static method to create the layer definition
MdfModel::LayerDefinition* MgLayerBase::GetLayerDefinition(MgResourceService* svcResource, MgResourceIdentifier* resId)
{
    // get and parse the layer definition
    Ptr<MgByteReader> reader = svcResource->GetResourceContent(resId, L"");
    Ptr<MgByteSink> sink = new MgByteSink(reader);
    Ptr<MgByte> bytes = sink->ToBuffer();
 
    assert(bytes->GetLength() > 0);

    MdfParser::SAX2Parser parser;
    parser.ParseString((const char*)bytes->Bytes(), bytes->GetLength());

    if (!parser.GetSucceeded())
    {
        STRING errorMsg = parser.GetErrorMessage();
        MgStringCollection arguments;
        arguments.Add(errorMsg);
        throw new MgInvalidLayerDefinitionException(L"MgLayerBase::GetLayerDefinition", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // detach the feature layer definition from the parser - it's
    // now the caller's responsibility to delete it
    MdfModel::LayerDefinition* ldef = parser.DetachLayerDefinition();
    assert(ldef != NULL);

    return ldef;
}

//static method to create the layer definition
MdfModel::LayerDefinition* MgLayerBase::GetLayerDefinition(CREFSTRING resourceContent)
{
    // get and parse the layer definition
    MdfParser::SAX2Parser parser;
    parser.ParseString(resourceContent.c_str(), resourceContent.length());

    if (!parser.GetSucceeded())
    {
        STRING errorMsg = parser.GetErrorMessage();
        MgStringCollection arguments;
        arguments.Add(errorMsg);
        throw new MgInvalidLayerDefinitionException(L"MgLayerBase::GetLayerDefinition", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // detach the feature layer definition from the parser - it's
    // now the caller's responsibility to delete it
    MdfModel::LayerDefinition* ldef = parser.DetachLayerDefinition();
    assert(ldef != NULL);

    return ldef;
}

//////////////////////////////////////////////////////////////
// Parse the layer definition XML and extracts scale ranges,
// feature source and feature class from it
//
void MgLayerBase::GetLayerInfoFromDefinition(MgResourceService* resourceService)
{
    m_scaleRanges.clear();
    m_featureSourceId = L"";
    m_featureName = L"";
    m_filter = L"";
    m_schemaName = L"";

    MG_TRY()

    auto_ptr<MdfModel::LayerDefinition> ldf;
    if(!m_forceReadFromServer && m_resourceContent != L"")
    {
        ldf.reset(MgLayerBase::GetLayerDefinition(m_resourceContent));
    }
    else
    {
        ldf.reset(MgLayerBase::GetLayerDefinition(resourceService, m_definition));
        //cache the resource content
        MdfParser::SAX2Parser parser;
        std::string content = parser.SerializeToXML(ldf.get(), NULL);
        MgUtil::MultiByteToWideChar(content, m_resourceContent);
    }

    if(ldf.get() != NULL)
    {
        m_featureSourceId = ldf->GetResourceID();
        MdfModel::VectorLayerDefinition* vl = dynamic_cast<MdfModel::VectorLayerDefinition*>(ldf.get());
        MdfModel::DrawingLayerDefinition* dl = dynamic_cast<MdfModel::DrawingLayerDefinition*>(ldf.get());
        MdfModel::GridLayerDefinition* gl = dynamic_cast<MdfModel::GridLayerDefinition*>(ldf.get());

        // Vector Layer
        if(vl != NULL)
        {
            //does the layer have tooltips?
            //we include layers with hyperlinks, since the presence of a hyperlink
            //results in a tooltip
            m_hasTooltips = (!vl->GetToolTip().empty()) || (!vl->GetUrl().empty());

            //store the scale ranges
            MdfModel::VectorScaleRangeCollection* scaleRanges = vl->GetScaleRanges();
            for (int i=0; i < scaleRanges->GetCount(); i++)
            {
                MdfModel::VectorScaleRange* scaleRange = scaleRanges->GetAt(i);
                m_scaleRanges.push_back(scaleRange->GetMinScale());
                m_scaleRanges.push_back(scaleRange->GetMaxScale());
            }

            //get the feature name
            m_featureName = vl->GetFeatureName();

            //get the filter
            m_filter = vl->GetFilter();

            //get the geometry property
            m_geometry = vl->GetGeometry();
        }
        // Drawing Layer
        else if(dl != NULL)
        {
            //drawing layers only have one scale range
            m_scaleRanges.push_back(dl->GetMinScale());
            m_scaleRanges.push_back(dl->GetMaxScale());
        }
        // Grid Layer
        else if(gl != NULL)
        {
            //store the scale ranges
            MdfModel::GridScaleRangeCollection* scaleRanges = gl->GetScaleRanges();
            for (int i=0; i < scaleRanges->GetCount(); i++)
            {
                MdfModel::GridScaleRange* scaleRange = scaleRanges->GetAt(i);
                m_scaleRanges.push_back(scaleRange->GetMinScale());
                m_scaleRanges.push_back(scaleRange->GetMaxScale());
            }

            //get the feature name
            m_featureName = gl->GetFeatureName();

            //get the filter
            m_filter = gl->GetFilter();

            //get the geometry property
            m_geometry = gl->GetGeometry();
        }
    }

    MG_CATCH_AND_THROW(L"MgLayerBase.GetLayerInfoFromDefinition")
}


//////////////////////////////////////////////////////////////
MgLayerBase::IdPropertyList& MgLayerBase::GetIdPropertyList()
{
    return m_idProps;
}


//////////////////////////////////////////////////////////////
STRING MgLayerBase::GetFeatureGeometryName()
{
    return m_geometry;
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Gets the class definition for the feature class of the layer.  If the
/// feature class of the layer is extended with properties from other feature
/// classes, then all those properties are also contained in the returned
/// class definition.
///
MgClassDefinition* MgLayerBase::GetClassDefinition()
{
    throw new MgNotImplementedException(L"MgLayerBase.GetClassDefinition",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Selects features from a feature source according to the
/// criteria set in the MgFeatureQueryOptions argument The
/// criteria are applied to all of the features in the feature
/// source. If you want to apply the criteria to a subset of the
/// features, use the MgFeatureService::SelectAggregate Method.
///
MgFeatureReader* MgLayerBase::SelectFeatures(MgFeatureQueryOptions* options)
{
    throw new MgNotImplementedException(L"MgLayerBase.SelectFeatures",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Selects groups of features from a feature source and applies
/// filters to each of the groups according to the criteria set
/// in the MgFeatureAggregateOptions argument. If you want to
/// apply the criteria to all features without grouping them, use
/// the MgFeatureService::SelectFeatures Method.
///
MgDataReader* MgLayerBase::SelectAggregate(MgFeatureAggregateOptions* options)
{
    throw new MgNotImplementedException(L"MgLayerBase.SelectAggregate",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Executes the MgDeleteFeatures, MgInsertFeatures,
/// MgUpdateFeatures, MgLockFeatures or MgUnlockFeatures commands
/// contained in the given MgFeatureCommandCollection object.
///
MgPropertyCollection* MgLayerBase::UpdateFeatures(MgFeatureCommandCollection* commands)
{
    throw new MgNotImplementedException(L"MgLayerBase.UpdateFeatures",
        __LINE__, __WFILE__, NULL, L"", NULL);
}
