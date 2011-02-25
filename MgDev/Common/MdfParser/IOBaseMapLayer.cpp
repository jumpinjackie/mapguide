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
#include "IOBaseMapLayer.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, BaseMapLayer);
ELEM_MAP_ENTRY(2, Name);
ELEM_MAP_ENTRY(3, ResourceId);
ELEM_MAP_ENTRY(4, Selectable);
ELEM_MAP_ENTRY(5, ShowInLegend);
ELEM_MAP_ENTRY(6, LegendLabel);
ELEM_MAP_ENTRY(7, ExpandInLegend);
ELEM_MAP_ENTRY(8, ExtendedData1);

IOBaseMapLayer::IOBaseMapLayer(Version& version) : IOMapLayerCommon(version)
{
    this->m_baseMapLayers = NULL;
}


IOBaseMapLayer::IOBaseMapLayer(BaseMapLayerCollection* baseMapLayers, Version& version) : IOMapLayerCommon(version)
{
    this->m_baseMapLayers = baseMapLayers;
}


IOBaseMapLayer::~IOBaseMapLayer()
{
}

void IOBaseMapLayer::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eBaseMapLayer:
        this->m_startElemName = name;
        this->m_mapLayerCommon = new BaseMapLayer(L"", L"");
        break;

    case eExtendedData1:
        this->m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IOBaseMapLayer::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_mapLayerCommon->SetUnknownXml(this->m_unknownXml);
        this->m_baseMapLayers->Adopt(this->m_mapLayerCommon);
        this->m_baseMapLayers = NULL;
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


void IOBaseMapLayer::Write(MdfStream& fd, BaseMapLayer* baseMapLayer, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sBaseMapLayer) << std::endl;
    tab.inctab();

    IOMapLayerCommon::Write(fd, baseMapLayer, version, tab);

    // Write any unknown XML / extended data
    if (!version || (*version >= Version(2, 3, 0)))
        IOUnknown::Write(fd, baseMapLayer->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sBaseMapLayer) << std::endl;
}
