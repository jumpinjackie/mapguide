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
#include "IOBaseMapLayerGroup.h"
#include "IOBaseMapLayer.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, BaseMapLayerGroup);
ELEM_MAP_ENTRY(2, Name);
ELEM_MAP_ENTRY(3, Visible);
ELEM_MAP_ENTRY(4, ShowInLegend);
ELEM_MAP_ENTRY(5, ExpandInLegend);
ELEM_MAP_ENTRY(6, LegendLabel);
ELEM_MAP_ENTRY(7, BaseMapLayer);
ELEM_MAP_ENTRY(8, ExtendedData1);


IOBaseMapLayerGroup::IOBaseMapLayerGroup(Version& version) : IOMapLayerGroupCommon(version)
{
}


IOBaseMapLayerGroup::IOBaseMapLayerGroup(MapDefinition* map, Version& version) : IOMapLayerGroupCommon(map, version)
{
}

IOBaseMapLayerGroup::IOBaseMapLayerGroup(TileSetDefinition* tileset, Version& version) : IOMapLayerGroupCommon(tileset, version)
{
}


IOBaseMapLayerGroup::~IOBaseMapLayerGroup()
{
}


void IOBaseMapLayerGroup::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eBaseMapLayerGroup:
        this->m_startElemName = name;
        this->m_layerGroup = new BaseMapLayerGroup(L"");
        break;

    case eBaseMapLayer:
        {
            BaseMapLayerGroup* baseMapLayerGroup = static_cast<BaseMapLayerGroup*>(this->m_layerGroup);
            IOBaseMapLayer* IO = new IOBaseMapLayer(baseMapLayerGroup->GetLayers(), this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eExtendedData1:
        this->m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IOBaseMapLayerGroup::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_layerGroup->SetUnknownXml(this->m_unknownXml);
        if (NULL != this->m_map)
        {
            this->m_map->GetBaseMapLayerGroups()->Adopt(static_cast<BaseMapLayerGroup*>(this->m_layerGroup));
        }
        else if (NULL != this->m_tileset)
        {
            this->m_tileset->GetBaseMapLayerGroups()->Adopt(static_cast<BaseMapLayerGroup*>(this->m_layerGroup));
        }
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


void IOBaseMapLayerGroup::Write(MdfStream& fd, BaseMapLayerGroup* baseMapLayerGroup, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sBaseMapLayerGroup) << std::endl;
    tab.inctab();

    IOMapLayerGroupCommon::Write(fd, baseMapLayerGroup, version, tab);

    // Property: Layers
    BaseMapLayerCollection* baseMapLayers = baseMapLayerGroup->GetLayers();
    for (int i=0; i<baseMapLayers->GetCount(); ++i)
        IOBaseMapLayer::Write(fd, static_cast<BaseMapLayer*>(baseMapLayers->GetAt(i)), version, tab);

    // Write any unknown XML / extended data
    if (!version || (*version >= Version(2, 3, 0)))
        IOUnknown::Write(fd, baseMapLayerGroup->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sBaseMapLayerGroup) << std::endl;
}
