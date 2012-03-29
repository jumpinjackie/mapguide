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
#include "IOMapLayerGroup.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, MapLayerGroup);
ELEM_MAP_ENTRY(2, Name);
ELEM_MAP_ENTRY(3, Visible);
ELEM_MAP_ENTRY(4, ShowInLegend);
ELEM_MAP_ENTRY(5, ExpandInLegend);
ELEM_MAP_ENTRY(6, LegendLabel);
ELEM_MAP_ENTRY(7, Group);
ELEM_MAP_ENTRY(8, ExtendedData1);

IOMapLayerGroup::IOMapLayerGroup(Version& version) : IOMapLayerGroupCommon(version)
{
}


IOMapLayerGroup::IOMapLayerGroup(MapDefinition* map, Version& version) : IOMapLayerGroupCommon(map, version)
{
}


IOMapLayerGroup::~IOMapLayerGroup()
{
}


void IOMapLayerGroup::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eMapLayerGroup:
        this->m_startElemName = name;
        this->m_layerGroup = new MapLayerGroup(L"");
        break;

    case eExtendedData1:
        this->m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IOMapLayerGroup::ElementChars(const wchar_t* ch)
{
    if (this->m_currElemId == eGroup)
        static_cast<MapLayerGroup*>(this->m_layerGroup)->SetGroup(ch);
    else
        IOMapLayerGroupCommon::ElementChars(ch);
}


void IOMapLayerGroup::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_layerGroup->SetUnknownXml(this->m_unknownXml);
        this->m_map->GetLayerGroups()->Adopt(static_cast<MapLayerGroup*>(this->m_layerGroup));
        this->m_map = NULL;
        this->m_layerGroup = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOMapLayerGroup::Write(MdfStream& fd, MapLayerGroup* mapLayerGroup, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sMapLayerGroup) << std::endl;
    tab.inctab();

    IOMapLayerGroupCommon::Write(fd, mapLayerGroup, version, tab);

    // Property: Group
    fd << tab.tab() << startStr(sGroup);
    fd << EncodeString(mapLayerGroup->GetGroup());
    fd << endStr(sGroup) << std::endl;

    // Write any unknown XML / extended data
    if (!version || (*version >= Version(2, 3, 0)))
        IOUnknown::Write(fd, mapLayerGroup->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sMapLayerGroup) << std::endl;
}
