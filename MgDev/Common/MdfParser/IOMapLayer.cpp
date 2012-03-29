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
#include "IOMapLayer.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, MapLayer);
ELEM_MAP_ENTRY(2, Name);
ELEM_MAP_ENTRY(3, ResourceId);
ELEM_MAP_ENTRY(4, Selectable);
ELEM_MAP_ENTRY(5, ShowInLegend);
ELEM_MAP_ENTRY(6, LegendLabel);
ELEM_MAP_ENTRY(7, ExpandInLegend);
ELEM_MAP_ENTRY(8, Visible);
ELEM_MAP_ENTRY(9, Group);
ELEM_MAP_ENTRY(10, ExtendedData1);

IOMapLayer::IOMapLayer(Version& version) : IOMapLayerCommon(version)
{
    this->m_map = NULL;
}


IOMapLayer::IOMapLayer(MapDefinition* map, Version& version) : IOMapLayerCommon(version)
{
    this->m_map = map;
}


IOMapLayer::~IOMapLayer()
{
}


void IOMapLayer::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eMapLayer:
        this->m_startElemName = name;
        this->m_mapLayerCommon = new MapLayer(L"", L"");
        break;

    case eExtendedData1:
        this->m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IOMapLayer::ElementChars(const wchar_t* ch)
{
    MapLayer* mapLayer = static_cast<MapLayer*>(this->m_mapLayerCommon);
    if (this->m_currElemId == eGroup)
        mapLayer->SetGroup(ch);
    else if (this->m_currElemId == eVisible)
        mapLayer->SetVisible(wstrToBool(ch));
    else
        IOMapLayerCommon::ElementChars(ch);
}


void IOMapLayer::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_mapLayerCommon->SetUnknownXml(this->m_unknownXml);
        this->m_map->GetLayers()->Adopt(static_cast<MapLayer*>(this->m_mapLayerCommon));
        this->m_map = NULL;
        this->m_mapLayerCommon = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOMapLayer::Write(MdfStream& fd, MapLayer* mapLayer, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sMapLayer) << std::endl;
    tab.inctab();

    IOMapLayerCommon::Write(fd, mapLayer, version, tab);

    // Property: Visible
    fd << tab.tab() << startStr(sVisible);
    fd << BoolToStr(mapLayer->IsVisible());
    fd << endStr(sVisible) << std::endl;

    // Property: Group
    fd << tab.tab() << startStr(sGroup);
    fd << EncodeString(mapLayer->GetGroup());
    fd << endStr(sGroup) << std::endl;

    // Write any unknown XML / extended data
    if (!version || (*version >= Version(2, 3, 0)))
        IOUnknown::Write(fd, mapLayer->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sMapLayer) << std::endl; // NOXLATE
}
