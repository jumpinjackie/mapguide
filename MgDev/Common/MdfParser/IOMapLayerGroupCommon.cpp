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

#include "stdafx.h"
#include "IOMapLayerGroupCommon.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOMapLayerGroupCommon::IOMapLayerGroupCommon(Version& version) : SAX2ElementHandler(version)
{
    this->m_layerGroup = NULL;
    this->m_map = NULL;
    this->m_tileset = NULL;
}


IOMapLayerGroupCommon::IOMapLayerGroupCommon(MapDefinition* map, Version& version) : SAX2ElementHandler(version)
{
    this->m_layerGroup = NULL;
    this->m_map = map;
    this->m_tileset = NULL;
}


IOMapLayerGroupCommon::IOMapLayerGroupCommon(TileSetDefinition* tileset, Version& version) : SAX2ElementHandler(version)
{
    this->m_layerGroup = NULL;
    this->m_map = NULL;
    this->m_tileset = tileset;
}


IOMapLayerGroupCommon::~IOMapLayerGroupCommon()
{
}


void IOMapLayerGroupCommon::ElementChars(const wchar_t* ch)
{
    if (this->m_currElemName == L"Name") // NOXLATE
        this->m_layerGroup->SetName(ch);
    else if (this->m_currElemName == L"Visible") // NOXLATE
        this->m_layerGroup->SetVisible(wstrToBool(ch));
    else if (this->m_currElemName == L"ShowInLegend") // NOXLATE
        this->m_layerGroup->SetShowInLegend(wstrToBool(ch));
    else if (this->m_currElemName == L"ExpandInLegend") // NOXLATE
        this->m_layerGroup->SetExpandInLegend(wstrToBool(ch));
    else if (this->m_currElemName == L"LegendLabel") // NOXLATE
        this->m_layerGroup->SetLegendLabel(ch);
}


void IOMapLayerGroupCommon::Write(MdfStream& fd, MapLayerGroupCommon* layerGroup, Version* version, MgTab& tab)
{
    // Property: Name
    fd << tab.tab() << "<Name>"; // NOXLATE
    fd << EncodeString(layerGroup->GetName());
    fd << "</Name>" << std::endl; // NOXLATE

    // Property: Visible
    fd << tab.tab() << "<Visible>"; // NOXLATE
    fd << BoolToStr(layerGroup->IsVisible());
    fd << "</Visible>" << std::endl; // NOXLATE

    // Property: ShowInLegend
    fd << tab.tab() << "<ShowInLegend>"; // NOXLATE
    fd << BoolToStr(layerGroup->IsShowInLegend());
    fd << "</ShowInLegend>" << std::endl; // NOXLATE

    // Property: ExpandInLegend
    fd << tab.tab() << "<ExpandInLegend>"; // NOXLATE
    fd << BoolToStr(layerGroup->IsExpandInLegend());
    fd << "</ExpandInLegend>" << std::endl; // NOXLATE

    // Property: LegendLabel
    fd << tab.tab() << "<LegendLabel>"; // NOXLATE
    fd << EncodeString(layerGroup->GetLegendLabel());
    fd << "</LegendLabel>" << std::endl; // NOXLATE
}
