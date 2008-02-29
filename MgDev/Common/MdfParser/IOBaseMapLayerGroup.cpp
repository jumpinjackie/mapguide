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

#include "stdafx.h"
#include "IOBaseMapLayerGroup.h"
#include "IOBaseMapLayer.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOBaseMapLayerGroup::IOBaseMapLayerGroup(Version& version) : IOMapLayerGroupCommon(version)
{
}


IOBaseMapLayerGroup::IOBaseMapLayerGroup(MapDefinition* map, Version& version) : IOMapLayerGroupCommon(map, version)
{
}


IOBaseMapLayerGroup::~IOBaseMapLayerGroup()
{
}


void IOBaseMapLayerGroup::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"BaseMapLayerGroup") // NOXLATE
    {
        this->m_startElemName = name;
        this->m_layerGroup = new BaseMapLayerGroup(L"");
    }
    else
    {
        if (this->m_currElemName == L"BaseMapLayer") // NOXLATE
        {
            BaseMapLayerGroup* baseMapLayerGroup = static_cast<BaseMapLayerGroup*>(this->m_layerGroup);
            IOBaseMapLayer* IO = new IOBaseMapLayer(baseMapLayerGroup->GetLayers(), this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
    }
}


void IOBaseMapLayerGroup::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_map->GetBaseMapLayerGroups()->Adopt(static_cast<BaseMapLayerGroup*>(this->m_layerGroup));
        this->m_map = NULL;
        this->m_layerGroup = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOBaseMapLayerGroup::Write(MdfStream& fd, BaseMapLayerGroup* baseMapLayerGroup, Version* version)
{
    fd << tab() << "<BaseMapLayerGroup>" << std::endl; // NOXLATE
    inctab();

    IOMapLayerGroupCommon::Write(fd, baseMapLayerGroup, version);

    // Property: Layers
    BaseMapLayerCollection* baseMapLayers = baseMapLayerGroup->GetLayers();
    for (int i=0; i<baseMapLayers->GetCount(); ++i)
        IOBaseMapLayer::Write(fd, static_cast<BaseMapLayer*>(baseMapLayers->GetAt(i)), version);

    dectab();
    fd << tab() << "</BaseMapLayerGroup>" << std::endl; // NOXLATE
}
