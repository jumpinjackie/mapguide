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
#include "IOMapLayer.h"
#include "IOExtra.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOMapLayer::IOMapLayer()
{
}

IOMapLayer::IOMapLayer(MapDefinition *map)
{
    this->map = map;
}

IOMapLayer::~IOMapLayer()
{
}

void IOMapLayer::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"MapLayer") // NOXLATE
    {
        m_startElemName = name;
        this->mapLayerCommon = new MapLayer(L"",L"");
    }
}

void IOMapLayer::ElementChars(const wchar_t *ch)
{
    MapLayer * mapLayer = static_cast<MapLayer *>(this->mapLayerCommon);
    if(m_currElemName == L"Group") // NOXLATE
        mapLayer->SetGroup(ch);
    else if(m_currElemName == L"Visible") // NOXLATE
        mapLayer->SetVisible(wstrToBool(ch));
    else
        IOMapLayerCommon::ElementChars(ch);
}

void IOMapLayer::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        this->map->GetLayers()->Adopt(static_cast<MapLayer*>(this->mapLayerCommon));
        handlerStack->pop();
        this->map = NULL;
        this->mapLayerCommon = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOMapLayer::Write(MdfStream &fd, MapLayer *mapLayer)
{
    fd << tab() << "<MapLayer>" << std::endl; // NOXLATE
    inctab();

    //Property: Name
    fd << tab() << "<Name>"; // NOXLATE
    fd << EncodeString(mapLayer->GetName());
    fd << "</Name>" << std::endl; // NOXLATE

    // Property: ResourceId
    fd << tab() << "<ResourceId>";  // NOXLATE
    fd << EncodeString(mapLayer->GetLayerResourceID());
    fd << "</ResourceId>" << std::endl; // NOXLATE

    //Property: Selectable
    fd << tab() << "<Selectable>"; // NOXLATE
    fd << (BoolToStr(mapLayer->IsSelectable())) ? ("true") : ("false"); // NOXLATE
    fd << "</Selectable>" << std::endl; // NOXLATE

    //Property: ShowInLegend
    fd << tab() << "<ShowInLegend>"; // NOXLATE
    fd << (BoolToStr(mapLayer->IsShowInLegend())) ? ("true") : ("false"); // NOXLATE
    fd << "</ShowInLegend>" << std::endl; // NOXLATE

    // Property: LegendLabel
    fd << tab() << "<LegendLabel>"; // NOXLATE
    fd << EncodeString(mapLayer->GetLegendLabel());
    fd << "</LegendLabel>" << std::endl; // NOXLATE

    //Property: ExpandInLegend
    fd << tab() << "<ExpandInLegend>"; // NOXLATE
    fd << (BoolToStr(mapLayer->IsExpandInLegend())) ? ("true") : ("false"); // NOXLATE
    fd << "</ExpandInLegend>" << std::endl; // NOXLATE

    //Property: Visible
    fd << tab() << "<Visible>"; // NOXLATE
    fd << (BoolToStr(mapLayer->IsVisible())) ? ("true") : ("false"); // NOXLATE
    fd << "</Visible>" << std::endl; // NOXLATE

    //Property: Group
    fd << tab() << "<Group>"; // NOXLATE
    fd << EncodeString(mapLayer->GetGroup());
    fd << "</Group>" << std::endl; // NOXLATE

    dectab();
    fd << tab() << "</MapLayer>" << std::endl; // NOXLATE
}
