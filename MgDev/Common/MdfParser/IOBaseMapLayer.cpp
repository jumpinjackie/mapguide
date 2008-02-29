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
#include "IOBaseMapLayer.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


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
    if (this->m_currElemName == L"BaseMapLayer") // NOXLATE
    {
        this->m_startElemName = name;
        this->m_mapLayerCommon = new BaseMapLayer(L"", L"");
    }
}


void IOBaseMapLayer::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_baseMapLayers->Adopt(this->m_mapLayerCommon);
        this->m_baseMapLayers = NULL;
        this->m_mapLayerCommon = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOBaseMapLayer::Write(MdfStream& fd, BaseMapLayer* baseMapLayer, Version* version)
{
    fd << tab() << "<BaseMapLayer>" << std::endl; // NOXLATE
    inctab();

    IOMapLayerCommon::Write(fd, baseMapLayer, version);

    dectab();
    fd << tab() << "</BaseMapLayer>" << std::endl; // NOXLATE
}
