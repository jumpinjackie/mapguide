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
#include "IOAreaRule.h"
#include "IOLabel.h"
#include "IOAreaSymbolization2D.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, AreaRule);
ELEM_MAP_ENTRY(2, LegendLabel);
ELEM_MAP_ENTRY(3, Filter);
ELEM_MAP_ENTRY(4, Label);
ELEM_MAP_ENTRY(5, AreaSymbolization2D);
ELEM_MAP_ENTRY(6, ExtendedData1);


IOAreaRule::IOAreaRule(Version& version) : SAX2ElementHandler(version)
{
    this->m_areaRule = NULL;
    this->m_areaTypeStyle = NULL;
}


IOAreaRule::IOAreaRule(AreaTypeStyle* areaTypeStyle, Version& version) : SAX2ElementHandler(version)
{
    this->m_areaRule = NULL;
    this->m_areaTypeStyle = areaTypeStyle;
}


IOAreaRule::~IOAreaRule()
{
}


void IOAreaRule::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eAreaRule:
        this->m_startElemName = name;
        this->m_areaRule = new AreaRule();
        break;

    case eLabel:
        {
            IOLabel* IO = new IOLabel(this->m_areaRule, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eAreaSymbolization2D:
        {
            IOAreaSymbolization2D* IO = new IOAreaSymbolization2D(this->m_areaRule, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eExtendedData1:
        this->m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IOAreaRule::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eLegendLabel:
        this->m_areaRule->SetLegendLabel(ch);
        break;

    case eFilter:
        this->m_areaRule->SetFilter(ch);
        break;
    }
}


void IOAreaRule::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_areaRule->SetUnknownXml(this->m_unknownXml);

        this->m_areaTypeStyle->GetRules()->Adopt(this->m_areaRule);
        this->m_areaTypeStyle = NULL;
        this->m_areaRule = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOAreaRule::Write(MdfStream& fd, AreaRule* areaRule, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sAreaRule) << std::endl;
    tab.inctab();

    // Property: LegendLabel
    fd << tab.tab() << startStr(sLegendLabel);
    fd << EncodeString(areaRule->GetLegendLabel());
    fd << endStr(sLegendLabel) << std::endl;

    // Property: Filter
    if (!areaRule->GetFilter().empty())
    {
        fd << tab.tab() << startStr(sFilter);
        fd << EncodeString(areaRule->GetFilter());
        fd << endStr(sFilter) << std::endl;
    }

    // Property: Label
    if (areaRule->GetLabel() && areaRule->GetLabel()->GetSymbol())
        IOLabel::Write(fd, areaRule->GetLabel(), version, tab);

    // Property: Symbolization
    IOAreaSymbolization2D::Write(fd, areaRule->GetSymbolization(), version, tab);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, areaRule->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sAreaRule) << std::endl;
}
