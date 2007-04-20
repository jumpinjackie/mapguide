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
#include "IOMapLayerCommon.h"
#include "IOExtra.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOMapLayerCommon::IOMapLayerCommon()
{
    this->mapLayerCommon = NULL;
}

IOMapLayerCommon::~IOMapLayerCommon()
{
}

void IOMapLayerCommon::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"Name") // NOXLATE
        (this->mapLayerCommon)->SetName(ch);
    else if(m_currElemName == L"ResourceId") // NOXLATE
        (this->mapLayerCommon)->SetLayerResourceID(ch);
    else if(m_currElemName == L"Selectable") // NOXLATE
        (this->mapLayerCommon)->SetSelectable(wstrToBool(ch));
    else if(m_currElemName == L"ShowInLegend") // NOXLATE
        (this->mapLayerCommon)->SetShowInLegend(wstrToBool(ch));
    else if(m_currElemName == L"LegendLabel") // NOXLATE
        (this->mapLayerCommon)->SetLegendLabel(ch);
    else if(m_currElemName == L"ExpandInLegend") // NOXLATE
        (this->mapLayerCommon)->SetExpandInLegend(wstrToBool(ch));
}

void IOMapLayerCommon::Write(MdfStream &fd, BaseMapLayer * baseMapLayer)
{
    //Property: Name
    fd << tab() << "<Name>"; // NOXLATE
    fd << EncodeString(baseMapLayer->GetName());
    fd << "</Name>" << std::endl; // NOXLATE

    // Property: ResourceId
    fd << tab() << "<ResourceId>"; // NOXLATE
    fd << EncodeString(baseMapLayer->GetLayerResourceID());
    fd << "</ResourceId>" << std::endl; // NOXLATE

    //Property: Selectable
    fd << tab() << "<Selectable>"; // NOXLATE
    fd << (baseMapLayer->IsSelectable()? "true" : "false"); // NOXLATE
    fd << "</Selectable>" << std::endl; // NOXLATE

    //Property: ShowInLegend
    fd << tab() << "<ShowInLegend>"; // NOXLATE
    fd << (baseMapLayer->IsShowInLegend()? "true" : "false"); // NOXLATE
    fd << "</ShowInLegend>" << std::endl; // NOXLATE

    // Property: LegendLabel
    fd << tab() << "<LegendLabel>"; // NOXLATE
    fd << EncodeString(baseMapLayer->GetLegendLabel());
    fd << "</LegendLabel>" << std::endl; // NOXLATE

    //Property: ExpandInLegend
    fd << tab() << "<ExpandInLegend>"; // NOXLATE
    fd << (baseMapLayer->IsExpandInLegend()? "true" : "false"); // NOXLATE
    fd << "</ExpandInLegend>" << std::endl; // NOXLATE
}
