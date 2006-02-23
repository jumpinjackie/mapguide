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
#include "IOBaseMapLayer.h"
#include "IOExtra.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOBaseMapLayer::IOBaseMapLayer()
: IOMapLayerCommon()
{
}

IOBaseMapLayer::IOBaseMapLayer(BaseMapLayerCollection * baseMapLayers)
: IOMapLayerCommon()
{
    this->baseMapLayers = baseMapLayers;
}

IOBaseMapLayer::~IOBaseMapLayer()
{
}

void IOBaseMapLayer::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"BaseMapLayer") // NOXLATE
    {
        m_startElemName = name;
        this->mapLayerCommon = new BaseMapLayer(L"",L"");
    }
}

void IOBaseMapLayer::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        this->baseMapLayers->Adopt(this->mapLayerCommon);
        handlerStack->pop();
        this->baseMapLayers = NULL;
        this->mapLayerCommon = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOBaseMapLayer::Write(MdfStream &fd, BaseMapLayer * baseMapLayer)
{
    fd << tab() << "<BaseMapLayer>" << std::endl; // NOXLATE
    inctab();

    IOMapLayerCommon::Write(fd, baseMapLayer);

    dectab();
    fd << tab() << "</BaseMapLayer>" << std::endl; // NOXLATE
}
