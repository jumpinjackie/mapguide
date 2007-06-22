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

#include "stdafx.h"
#include "IOMapLayerGroup.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOMapLayerGroup::IOMapLayerGroup() : IOMapLayerGroupCommon()
{
}


IOMapLayerGroup::IOMapLayerGroup(MapDefinition* map) : IOMapLayerGroupCommon(map)
{
}


IOMapLayerGroup::~IOMapLayerGroup()
{
}


void IOMapLayerGroup::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"MapLayerGroup") // NOXLATE
    {
        this->m_startElemName = name;
        this->m_layerGroup = new MapLayerGroup(L"");
    }
}


void IOMapLayerGroup::ElementChars(const wchar_t* ch)
{
    if (this->m_currElemName == L"Group") // NOXLATE
        static_cast<MapLayerGroup*>(this->m_layerGroup)->SetGroup(ch);
    else
        IOMapLayerGroupCommon::ElementChars(ch);
}


void IOMapLayerGroup::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_map->GetLayerGroups()->Adopt(static_cast<MapLayerGroup*>(this->m_layerGroup));
        this->m_map = NULL;
        this->m_layerGroup = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOMapLayerGroup::Write(MdfStream& fd, MapLayerGroup* mapLayerGroup, Version* version)
{
    fd << tab() << "<MapLayerGroup>" << std::endl; // NOXLATE
    inctab();

    IOMapLayerGroupCommon::Write(fd, mapLayerGroup, version);

    //Property: Group
    fd << tab() << "<Group>"; // NOXLATE
    fd << EncodeString(mapLayerGroup->GetGroup());
    fd << "</Group>" << std::endl; // NOXLATE

    dectab();
    fd << tab() << "</MapLayerGroup>" << std::endl; // NOXLATE
}
