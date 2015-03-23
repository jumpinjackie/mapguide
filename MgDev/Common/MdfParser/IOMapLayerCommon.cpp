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
#include "IOMapLayerCommon.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOMapLayerCommon::IOMapLayerCommon(Version& version) : SAX2ElementHandler(version)
{
    this->m_mapLayerCommon = NULL;
}


IOMapLayerCommon::~IOMapLayerCommon()
{
}


void IOMapLayerCommon::ElementChars(const wchar_t* ch)
{
    if (this->m_currElemName == L"Name") // NOXLATE
        this->m_mapLayerCommon->SetName(ch);
    else if (this->m_currElemName == L"ResourceId") // NOXLATE
        this->m_mapLayerCommon->SetLayerResourceID(ch);
    else if (this->m_currElemName == L"Selectable") // NOXLATE
        this->m_mapLayerCommon->SetSelectable(wstrToBool(ch));
    else if (this->m_currElemName == L"ShowInLegend") // NOXLATE
        this->m_mapLayerCommon->SetShowInLegend(wstrToBool(ch));
    else if (this->m_currElemName == L"LegendLabel") // NOXLATE
        this->m_mapLayerCommon->SetLegendLabel(ch);
    else if (this->m_currElemName == L"ExpandInLegend") // NOXLATE
        this->m_mapLayerCommon->SetExpandInLegend(wstrToBool(ch));
}


void IOMapLayerCommon::Write(MdfStream& fd, BaseMapLayer* baseMapLayer, Version* version, MgTab& tab)
{
    // Property: Name
    fd << tab.tab() << "<Name>"; // NOXLATE
    fd << EncodeString(baseMapLayer->GetName());
    fd << "</Name>" << std::endl; // NOXLATE

    // Property: ResourceId
    fd << tab.tab() << "<ResourceId>"; // NOXLATE
    fd << EncodeString(baseMapLayer->GetLayerResourceID());
    fd << "</ResourceId>" << std::endl; // NOXLATE

    // Property: Selectable
    fd << tab.tab() << "<Selectable>"; // NOXLATE
    fd << BoolToStr(baseMapLayer->IsSelectable());
    fd << "</Selectable>" << std::endl; // NOXLATE

    // Property: ShowInLegend
    fd << tab.tab() << "<ShowInLegend>"; // NOXLATE
    fd << BoolToStr(baseMapLayer->IsShowInLegend());
    fd << "</ShowInLegend>" << std::endl; // NOXLATE

    // Property: LegendLabel
    fd << tab.tab() << "<LegendLabel>"; // NOXLATE
    fd << EncodeString(baseMapLayer->GetLegendLabel());
    fd << "</LegendLabel>" << std::endl; // NOXLATE

    // Property: ExpandInLegend
    fd << tab.tab() << "<ExpandInLegend>"; // NOXLATE
    fd << BoolToStr(baseMapLayer->IsExpandInLegend());
    fd << "</ExpandInLegend>" << std::endl; // NOXLATE
}
