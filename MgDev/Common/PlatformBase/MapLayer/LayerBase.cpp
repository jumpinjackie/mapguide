//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

IMPLEMENT_CREATE_OBJECT(MgLayerBase)

//////////////////////////////////////////////////////////////
// Creates a MgLayerBase object
//
MgLayerBase::MgLayerBase()
    : m_type(MgLayerType::Dynamic),
      m_group((MgLayerGroup*)NULL),
      m_visible(true),
      m_selectable(false),
      m_displayInLegend(false),
      m_expandInLegend(false),
      m_needRefresh(false),
      m_layers(NULL),
      m_displayOrder(0.0)
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
      m_displayInLegend(false),
      m_expandInLegend(false),
      m_needRefresh(false),
      m_layers(NULL),
      m_displayOrder(0.)
{
    m_definition = layerDefinition;
    if(SAFE_ADDREF((MgResourceIdentifier*)m_definition) != NULL)
        m_name = m_definition->GetName();

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
        throw new MgInvalidOperationException(L"MgLayer.SetVisible", __LINE__, __WFILE__, NULL, L"", NULL);
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

    GetLayerInfoFromDefinition(resourceService);

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
// Parse the layer definition XML and extracts scale ranges,
// feature source and feature class from it
//
void MgLayerBase::GetLayerInfoFromDefinition(MgResourceService* resourceService)
{
    m_scaleRanges.clear();
    m_featureSourceId = L"";
    m_featureName = L"";

    MG_TRY()

    //get the layer definition from the resource repository
    Ptr<MgByteReader> layerContent = resourceService->GetResourceContent(m_definition);

    //TODO: This code should be converted to use the MDF parser instead!!!
    //It would simpler and faster and would not get out of date as easily.  

    //parse the layer definition and extract all the scale ranges
    MgXmlUtil xmlUtil;
    xmlUtil.ParseString(MgUtil::GetTextFromReader(layerContent).c_str());

    wstring strval;

    DOMElement* root = xmlUtil.GetRootNode();
    DOMNode* child = MgXmlUtil::GetFirstChild(root);
    while(0 != child)
    {
        if(MgXmlUtil::GetNodeType(child) == DOMNode::ELEMENT_NODE)
        {
            DOMElement* elt = (DOMElement*)child;
            wstring strName = MgXmlUtil::GetTagName(elt);

            if(strName == L"VectorLayerDefinition")     //NOXLATE
            {
                DOMNode* child = MgXmlUtil::GetFirstChild(elt);
                while(0 != child)
                {
                    if(MgXmlUtil::GetNodeType(child) == DOMNode::ELEMENT_NODE)
                    {
                        elt = (DOMElement*)child;
                        strName = MgXmlUtil::GetTagName(elt);

                        if(strName == L"VectorScaleRange")  //NOXLATE
                        {
                            double minScale = 0.0;
                            double maxScale = MdfModel::VectorScaleRange::MAX_MAP_SCALE;

                            DOMNode* ichild = MgXmlUtil::GetFirstChild(elt);
                            while(0 != ichild)
                            {
                                if(MgXmlUtil::GetNodeType(ichild) == DOMNode::ELEMENT_NODE)
                                {
                                    DOMElement* ielt = (DOMElement*)ichild;
                                    strName = MgXmlUtil::GetTagName(ielt);

                                    if(strName == L"MinScale")  //NOXLATE
                                    {
                                        MgXmlUtil::GetTextFromElement(ielt, strval);
                                        minScale = MgUtil::StringToDouble(strval);
                                    }
                                    else if(strName == L"MaxScale")  //NOXLATE
                                    {
                                        MgXmlUtil::GetTextFromElement(ielt, strval);
                                        maxScale = MgUtil::StringToDouble(strval);
                                    }
                                }
                                ichild = MgXmlUtil::GetNextSibling(ichild);
                            }

                            m_scaleRanges.push_back(minScale);
                            m_scaleRanges.push_back(maxScale);
                        }
                        else if(strName == L"ResourceId")  //NOXLATE
                        {
                            MgXmlUtil::GetTextFromElement(elt, m_featureSourceId);
                            m_featureSourceId = MgUtil::Trim(m_featureSourceId);
                        }
                        else if(strName == L"FeatureName")  //NOXLATE
                        {
                            MgXmlUtil::GetTextFromElement(elt, m_featureName);
                            m_featureName = MgUtil::Trim(m_featureName);
                        }
                        else if(strName == L"Geometry")  //NOXLATE
                        {
                            MgXmlUtil::GetTextFromElement(elt, m_geometry);
                            m_geometry = MgUtil::Trim(m_geometry);
                        }
                    }
                    child = MgXmlUtil::GetNextSibling(child);
                }
                break;
            }
            else if(strName == L"DrawingLayerDefinition")    //NOXLATE
            {
                double minScale = 0.0;
                double maxScale = MdfModel::VectorScaleRange::MAX_MAP_SCALE;
                DOMNode* ichild = MgXmlUtil::GetFirstChild(elt);
                while(0 != ichild)
                {
                    if(MgXmlUtil::GetNodeType(ichild) == DOMNode::ELEMENT_NODE)
                    {
                        DOMElement* ielt = (DOMElement*)ichild;
                        strName = MgXmlUtil::GetTagName(ielt);

                        if(strName == L"MinScale")  //NOXLATE
                        {
                            MgXmlUtil::GetTextFromElement(ielt, strval);
                            minScale = MgUtil::StringToDouble(strval);
                        }
                        else if(strName == L"MaxScale")  //NOXLATE
                        {
                            MgXmlUtil::GetTextFromElement(ielt, strval);
                            maxScale = MgUtil::StringToDouble(strval);
                        }
                    }
                    ichild = MgXmlUtil::GetNextSibling(ichild);
                }

                m_scaleRanges.push_back(minScale);
                m_scaleRanges.push_back(maxScale);

                break;
            }
            else if(strName == L"GridLayerDefinition")    //NOXLATE
            {
                DOMNode* child = MgXmlUtil::GetFirstChild(elt);
                while(0 != child)
                {
                    if(MgXmlUtil::GetNodeType(child) == DOMNode::ELEMENT_NODE)
                    {
                        elt = (DOMElement*)child;
                        strName = MgXmlUtil::GetTagName(elt);

                        if(strName == L"GridScaleRange")  //NOXLATE
                        {
                            double minScale = 0.0;
                            double maxScale = MdfModel::GridScaleRange::MAX_MAP_SCALE;

                            DOMNode* ichild = MgXmlUtil::GetFirstChild(elt);
                            while(0 != ichild)
                            {
                                if(MgXmlUtil::GetNodeType(ichild) == DOMNode::ELEMENT_NODE)
                                {
                                    DOMElement* ielt = (DOMElement*)ichild;
                                    strName = MgXmlUtil::GetTagName(ielt);

                                    if(strName == L"MinScale")  //NOXLATE
                                    {
                                        MgXmlUtil::GetTextFromElement(ielt, strval);
                                        minScale = MgUtil::StringToDouble(strval);
                                    }
                                    else if(strName == L"MaxScale")  //NOXLATE
                                    {
                                        MgXmlUtil::GetTextFromElement(ielt, strval);
                                        maxScale = MgUtil::StringToDouble(strval);
                                    }
                                }
                                ichild = MgXmlUtil::GetNextSibling(ichild);
                            }

                            m_scaleRanges.push_back(minScale);
                            m_scaleRanges.push_back(maxScale);
                        }
                        else if(strName == L"ResourceId")  //NOXLATE
                        {
                            MgXmlUtil::GetTextFromElement(elt, m_featureSourceId);
                            m_featureSourceId = MgUtil::Trim(m_featureSourceId);
                        }
                        else if(strName == L"FeatureName")  //NOXLATE
                        {
                            MgXmlUtil::GetTextFromElement(elt, m_featureName);
                            m_featureName = MgUtil::Trim(m_featureName);
                        }
                        else if(strName == L"Geometry")  //NOXLATE
                        {
                            MgXmlUtil::GetTextFromElement(elt, m_geometry);
                            m_geometry = MgUtil::Trim(m_geometry);
                        }
                    }
                    child = MgXmlUtil::GetNextSibling(child);
                }
                break;
            }
        }
        child = MgXmlUtil::GetNextSibling(child);
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
