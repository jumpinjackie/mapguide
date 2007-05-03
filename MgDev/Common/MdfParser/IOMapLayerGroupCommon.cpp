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
#include "IOMapLayerGroupCommon.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOMapLayerGroupCommon::IOMapLayerGroupCommon()
{
    this->_layerGroup = NULL;
    this->map = NULL;
}

IOMapLayerGroupCommon::IOMapLayerGroupCommon(MapDefinition * map)
{
    this->_layerGroup = NULL;
    this->map = map;
}

IOMapLayerGroupCommon::~IOMapLayerGroupCommon()
{
}

void IOMapLayerGroupCommon::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
#ifdef _NDEBUG
    _ASSERT(false);
#endif
}

void IOMapLayerGroupCommon::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"Name") // NOXLATE
        (this->_layerGroup)->SetName(ch);
    else if(m_currElemName == L"Visible") // NOXLATE
        (this->_layerGroup)->SetVisible(wstrToBool(ch));
    else if(m_currElemName == L"ShowInLegend") // NOXLATE
        (this->_layerGroup)->SetShowInLegend(wstrToBool(ch));
    else if(m_currElemName == L"ExpandInLegend") // NOXLATE
        (this->_layerGroup)->SetExpandInLegend(wstrToBool(ch));
    else if(m_currElemName == L"LegendLabel") // NOXLATE
        (this->_layerGroup)->SetLegendLabel(ch);
}

void IOMapLayerGroupCommon::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
#ifdef _NDEBUG
    _ASSERT(false);
#endif
}

void IOMapLayerGroupCommon::Write(MdfStream &fd, MapLayerGroupCommon * layerGroup)
{

    //Property: Name
    fd << tab() << "<Name>"; // NOXLATE
    fd << EncodeString(layerGroup->GetName());
    fd << "</Name>" << std::endl; // NOXLATE

    //Property: Visible
    fd << tab() << "<Visible>"; // NOXLATE
    fd << (layerGroup->IsVisible()? "true" : "false"); // NOXLATE
    fd << "</Visible>" << std::endl; // NOXLATE

    //Property: ShowInLegend
    fd << tab() << "<ShowInLegend>"; // NOXLATE
    fd << (layerGroup->IsShowInLegend()? "true" : "false"); // NOXLATE
    fd << "</ShowInLegend>" << std::endl; // NOXLATE

    //Property: ExpandInLegend
    fd << tab() << "<ExpandInLegend>"; // NOXLATE
    fd << (layerGroup->IsExpandInLegend()? "true" : "false"); // NOXLATE
    fd << "</ExpandInLegend>" << std::endl; // NOXLATE

    // Property: LegendLabel
    fd << tab() << "<LegendLabel>"; // NOXLATE
    fd << EncodeString(layerGroup->GetLegendLabel());
    fd << "</LegendLabel>" << std::endl; // NOXLATE
}
