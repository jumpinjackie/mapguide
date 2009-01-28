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

#include "MapGuideCommon.h"

MG_IMPL_DYNCREATE(MgFeatureInformation)

/////////////////////////////////////////
// Get the selection set the described features are part of.
//
MgSelection* MgFeatureInformation::GetSelection()
{
    return SAFE_ADDREF((MgSelection*)m_selection);
}

/////////////////////////////////////////
// Get the tooltip text
//
STRING MgFeatureInformation::GetTooltip()
{
    return m_tooltip;
}

/////////////////////////////////////////
// Get the hyperlink URL text
//
STRING MgFeatureInformation::GetHyperlink()
{
    return m_hyperlink;
}

/////////////////////////////////////////
// Get the collection of properties for the described feature.
//
MgPropertyCollection* MgFeatureInformation::GetProperties()
{
    return SAFE_ADDREF((MgPropertyCollection*)m_properties);
}

/////////////////////////////////////////
// Write feature information as XML document.
//
MgByteReader* MgFeatureInformation::ToXml()
{
    STRING xml;
    STRING xmlSelection = m_selection? m_selection->ToXml(false): L"";

    // TODO: define a schema for this XML
    xml.append(L"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<FeatureInformation>\n");

    size_t len = xmlSelection.length();
    if(len > 0)
    {
        xml.reserve(len + 2048);
        xml.append(xmlSelection);
    }
    else
    {
        xml.reserve(2048);
        xml.append(L"<FeatureSet />\n");
    }

    if(m_tooltip.length() > 0)
    {
        xml.append(L"<Tooltip>");
        xml.append(MgUtil::ReplaceEscapeCharInXml(m_tooltip));
        xml.append(L"</Tooltip>\n");
    }
    else
        xml.append(L"<Tooltip />\n");

    if(m_hyperlink.length() > 0)
    {
        xml.append(L"<Hyperlink>");
        xml.append(MgUtil::ReplaceEscapeCharInXml(m_hyperlink));
        xml.append(L"</Hyperlink>\n");
    }
    else
        xml.append(L"<Hyperlink />\n");

    if(m_properties != NULL)
    {
        for(int i = 0; i < m_properties->GetCount(); i++)
        {
            Ptr<MgStringProperty> prop = (MgStringProperty*)m_properties->GetItem(i);
            xml.append(L"<Property name=\"");
            xml.append(MgUtil::ReplaceEscapeCharInXml(prop->GetName()));
            xml.append(L"\" value=\"");
            xml.append(MgUtil::ReplaceEscapeCharInXml(prop->GetValue()));
            xml.append(L"\" />\n");
        }
    }
    xml.append(L"</FeatureInformation>\n");

    string xmlDoc = MgUtil::WideCharToMultiByte(xml);
    STRING mimeType = L"text/xml";
    return MgUtil::GetByteReader(xmlDoc, &mimeType);
}

/////////////////////////////////////////
// Construct an empty MgFeatureInformation object
//
MgFeatureInformation::MgFeatureInformation()
{
}

/////////////////////////////////////////
// Set the selection
//
void MgFeatureInformation::SetSelection(MgSelection* selection)
{
    m_selection = SAFE_ADDREF((MgSelection*)selection);
}

/////////////////////////////////////////
// Set the properties
//
void MgFeatureInformation::SetProperties(MgPropertyCollection* properties)
{
    m_properties = SAFE_ADDREF((MgPropertyCollection*)properties);
}

/////////////////////////////////////////
// Set the tooltip text
//
void MgFeatureInformation::SetTooltip(CREFSTRING tooltip)
{
    m_tooltip = tooltip;
}

/////////////////////////////////////////
// Set the hyperlink URL
//
void MgFeatureInformation::SetHyperlink(CREFSTRING hyperlink)
{
    m_hyperlink = hyperlink;
}

/////////////////////////////////////////
// Serialize to a stream
//
void MgFeatureInformation::Serialize(MgStream* stream)
{
    stream->WriteString(m_selection->ToXml());
    stream->WriteString(m_tooltip);
    stream->WriteString(m_hyperlink);
    stream->WriteObject(m_properties);
}

/////////////////////////////////////////
// Deserialize from a stream
//
void MgFeatureInformation::Deserialize(MgStream* stream)
{
    STRING xml;
    stream->GetString(xml);
    if(xml.length() > 0)
    {
        m_selection = new MgSelection();
        m_selection->FromXml(xml);
    }
    stream->GetString(m_tooltip);
    stream->GetString(m_hyperlink);
    m_properties = (MgPropertyCollection*)stream->GetObject();
}
