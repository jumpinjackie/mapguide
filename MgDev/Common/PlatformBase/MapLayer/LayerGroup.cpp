//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

MG_IMPL_DYNCREATE(MgLayerGroup)

//////////////////////////////////////////////////////////////
// Construct an empty MgLayerGroup object
//
MgLayerGroup::MgLayerGroup()
    : m_type(MgLayerGroupType::Normal),
      m_name(L""),
      m_group((MgLayerGroup*)NULL),
      m_visible(true),
      m_displayInLegend(false),
      m_expandInLegend(false),
      m_groups(NULL)
{
    //Generate a unique id for this group
    MgUtil::GenerateUuid(m_objectId);
}


//////////////////////////////////////////////////////////////
// Construct a MgLayerGroup object with the specified name
//
MgLayerGroup::MgLayerGroup(CREFSTRING name)
    : m_type(MgLayerGroupType::Normal),
      m_name(name),
      m_group((MgLayerGroup*)NULL),
      m_visible(true),
      m_displayInLegend(false),
      m_expandInLegend(false),
      m_groups(NULL)
{
    //Generate a unique id for this group
    MgUtil::GenerateUuid(m_objectId);
}


//////////////////////////////////////////////////////////////
// Returns the group name
//
STRING MgLayerGroup::GetName()
{
    return m_name;
}


//////////////////////////////////////////////////////////////
// Returns an identifier that can uniquely identify this layer group
//
STRING MgLayerGroup::GetObjectId()
{
    return m_objectId;
}


/////////////////////////////////////////////////////////////////
// Returns the type of the layer group
//
INT32 MgLayerGroup::GetLayerGroupType()
{
    return m_type;
}


/////////////////////////////////////////////////////////////////
// Sets the type of the layer group
//
void MgLayerGroup::SetLayerGroupType(INT32 layerGroupType)
{
    m_type = layerGroupType;
}


//////////////////////////////////////////////////////////////
// Returns the group this group is organized within
//
MgLayerGroup* MgLayerGroup::GetGroup()
{
    return SAFE_ADDREF((MgLayerGroup*)m_group);
}


//////////////////////////////////////////////////////////////
// Changes the group this layer is organized within.
// Specifying a null LayerGroup indicates the group should not reside within a group
//
void MgLayerGroup::SetGroup(MgLayerGroup* group)
{
    if(m_group != group)
    {
        m_group = group;
        SAFE_ADDREF((MgLayerGroup*)m_group);

        if(m_groups != NULL)
            m_groups->GetMap()->OnGroupParentChanged(this, group != NULL? group->GetObjectId(): L"");
    }
}


//////////////////////////////////////////////////////////////
// Returns the current state of the visible property.
// Note that this method may return true even though the group is not visible.
// This will occur if the visible flag of one of the groups this group
// is organized within is not visible.
//
bool MgLayerGroup::GetVisible()
{
    return m_visible;
}


//////////////////////////////////////////////////////////////
// Alters the group potential visibility.
// Note that setting this property to true will not necessarily guarantee the group will be visible.
// The group will be visible only when this property is true, all groups this group
// is organized within are visible.
//
void MgLayerGroup::SetVisible(bool visible)
{
    if(m_visible != visible)
    {
        m_visible = visible;

        if(m_groups != NULL)
            m_groups->GetMap()->OnGroupVisibilityChanged(this, visible? L"1": L"0");
    }
}


//////////////////////////////////////////////////////////////
// Returns the actual visibility of the group based upon the visible property of the LayerGroup,
// the visible property of each group this group is organized within
//
bool MgLayerGroup::IsVisible()
{
    Ptr<MgLayerGroup> group = m_group;

    if(group == NULL)
        return m_visible;
    else
        return m_visible && group->IsVisible();
}


//////////////////////////////////////////////////////////////
// Indicates whether an entry for the group should appear in the legend.
//
bool MgLayerGroup::GetDisplayInLegend()
{
    return m_displayInLegend;
}


//////////////////////////////////////////////////////////////
// Specifies whether an entry for the group should appear in the legend.
//
void MgLayerGroup::SetDisplayInLegend(bool displayInLegend)
{
    if(m_displayInLegend != displayInLegend)
    {
        m_displayInLegend = displayInLegend;

        if(m_groups != NULL)
            m_groups->GetMap()->OnGroupDisplayInLegendChanged(this, displayInLegend? L"1": L"0");
    }
}


//////////////////////////////////////////////////////////////
// Indicates whether the entry for the group should initially be
// expanded in the legend, if possible.
//
bool MgLayerGroup::GetExpandInLegend()
{
    return m_expandInLegend;
}


//////////////////////////////////////////////////////////////
// Specifies whether the entry for the layer should initially
// be expanded in the legend, if possible.
//
void MgLayerGroup::SetExpandInLegend(bool expandInLegend)
{
    m_expandInLegend = expandInLegend;
}


//////////////////////////////////////////////////////////////
// Returns the label to be displayed for the group in the legend
//
STRING MgLayerGroup::GetLegendLabel()
{
    return m_legendLabel;
}


//////////////////////////////////////////////////////////////
// Sets the label to be displayed for the group in the legend
//
void MgLayerGroup::SetLegendLabel(CREFSTRING legendLabel)
{
    if(m_legendLabel != legendLabel)
    {
        m_legendLabel = legendLabel;

        if(m_groups != NULL)
            m_groups->GetMap()->OnGroupLegendLabelChanged(this, legendLabel);
    }
}


//////////////////////////////////////////////////////////////
// Serialize data to a stream
//
void MgLayerGroup::Serialize(MgStream* stream)
{
    stream->WriteString(m_name);
    stream->WriteString(m_objectId);
    stream->WriteInt32(m_type);
    stream->WriteBoolean(m_visible);
    stream->WriteBoolean(m_displayInLegend);
    stream->WriteBoolean(m_expandInLegend);
    stream->WriteString(m_legendLabel);
}


//////////////////////////////////////////////////////////////
// Deserialize data from a stream
//
void MgLayerGroup::Deserialize(MgStream* stream)
{
    stream->GetString(m_name);
    stream->GetString(m_objectId);
    stream->GetInt32(m_type);
    stream->GetBoolean(m_visible);
    stream->GetBoolean(m_displayInLegend);
    stream->GetBoolean(m_expandInLegend);
    stream->GetString(m_legendLabel);
}


//////////////////////////////////////////////////////////////
// Does it allow to set the name
//
bool MgLayerGroup::CanSetName()
{
    return false;
}


//////////////////////////////////////////////////////////////
// Destruct a MgLayerGroup object
//
MgLayerGroup::~MgLayerGroup()
{
}


//////////////////////////////////////////////////////////////
// Attaches this group to a set of groups
//
void MgLayerGroup::SetContainer(MgLayerGroupCollection* cont)
{
    m_groups = cont;
}
