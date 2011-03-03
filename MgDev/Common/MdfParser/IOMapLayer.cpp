//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


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
    if (this->m_currElemName == L"MapLayer") // NOXLATE
    {
        this->m_startElemName = name;
        this->m_mapLayerCommon = new MapLayer(L"", L"");
    }
}


void IOMapLayer::ElementChars(const wchar_t* ch)
{
    MapLayer* mapLayer = static_cast<MapLayer*>(this->m_mapLayerCommon);
    if (this->m_currElemName == L"Group") // NOXLATE
        mapLayer->SetGroup(ch);
    else if (this->m_currElemName == L"Visible") // NOXLATE
        mapLayer->SetVisible(wstrToBool(ch));
    else
        IOMapLayerCommon::ElementChars(ch);
}


void IOMapLayer::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_map->GetLayers()->Adopt(static_cast<MapLayer*>(this->m_mapLayerCommon));
        this->m_map = NULL;
        this->m_mapLayerCommon = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOMapLayer::Write(MdfStream& fd, MapLayer* mapLayer, Version* version, MgTab& tab)
{
    fd << tab.tab() << "<MapLayer>" << std::endl; // NOXLATE
    tab.inctab();

    IOMapLayerCommon::Write(fd, mapLayer, version, tab);

    // Property: Visible
    fd << tab.tab() << "<Visible>"; // NOXLATE
    fd << BoolToStr(mapLayer->IsVisible());
    fd << "</Visible>" << std::endl; // NOXLATE

    // Property: Group
    fd << tab.tab() << "<Group>"; // NOXLATE
    fd << EncodeString(mapLayer->GetGroup());
    fd << "</Group>" << std::endl; // NOXLATE

    tab.dectab();
    fd << tab.tab() << "</MapLayer>" << std::endl; // NOXLATE
}
