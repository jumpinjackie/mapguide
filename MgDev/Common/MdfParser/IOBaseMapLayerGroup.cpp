//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

IOBaseMapLayerGroup::IOBaseMapLayerGroup()
: IOMapLayerGroupCommon()
{
}

IOBaseMapLayerGroup::IOBaseMapLayerGroup(MapDefinition * map)
: IOMapLayerGroupCommon(map)
{
}

IOBaseMapLayerGroup::~IOBaseMapLayerGroup()
{
}

void IOBaseMapLayerGroup::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"BaseMapLayerGroup") // NOXLATE
    {
        m_startElemName = name;
        this->_layerGroup = new BaseMapLayerGroup(L"");
    }
    else
    {
        if (m_currElemName == L"BaseMapLayer") // NOXLATE
        {
            BaseMapLayerGroup * baseMapLayerGroup = static_cast<BaseMapLayerGroup*>(this->_layerGroup);
            IOBaseMapLayer * IO = new IOBaseMapLayer(baseMapLayerGroup->GetLayers());
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
    }
}


void IOBaseMapLayerGroup::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        this->map->GetBaseMapLayerGroups()->Adopt(static_cast<BaseMapLayerGroup*>(this->_layerGroup));
        handlerStack->pop();
        this->map = NULL;
        this->_layerGroup = NULL;
        m_startElemName = L"";
        delete this;
    }
}


void IOBaseMapLayerGroup::Write(MdfStream &fd, MapLayerGroupCommon * layerGroup)
{
    BaseMapLayerGroup * baseMapLayerGroup = static_cast<BaseMapLayerGroup *>(layerGroup);
    fd << tab() << "<BaseMapLayerGroup>" << std::endl; // NOXLATE
    inctab();

    IOMapLayerGroupCommon::Write(fd, baseMapLayerGroup);

    //Property: Layers
    BaseMapLayerCollection * baseMapLayers = baseMapLayerGroup->GetLayers();
    IOBaseMapLayer * ioBaseMapLayer = new IOBaseMapLayer();
    for (int i = 0; i < baseMapLayers->GetCount(); i++)
    {
        ioBaseMapLayer->Write(fd, static_cast<BaseMapLayer*>(baseMapLayers->GetAt(i)));
    }
    delete ioBaseMapLayer;

    dectab();
    fd << tab() << "</BaseMapLayerGroup>" << std::endl; // NOXLATE
}
