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
#include "IOGridColorRule.h"
#include "IOGridColor.h"
#include "IOLabel.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, ColorRule);
ELEM_MAP_ENTRY(2, LegendLabel);
ELEM_MAP_ENTRY(3, Filter);
ELEM_MAP_ENTRY(4, Label);
ELEM_MAP_ENTRY(5, Color);
ELEM_MAP_ENTRY(6, ExtendedData1);


IOGridColorRule::IOGridColorRule(Version& version) : SAX2ElementHandler(version)
{
    this->m_colorStyle = NULL;
    this->m_colorRule = NULL;
}


IOGridColorRule::IOGridColorRule(GridColorStyle* colorStyle, Version& version) : SAX2ElementHandler(version)
{
    this->m_colorStyle = colorStyle;
    this->m_colorRule = NULL;
}


IOGridColorRule::~IOGridColorRule()
{
}


void IOGridColorRule::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eColorRule:
        this->m_startElemName = name;
        this->m_colorRule = new GridColorRule();
        break;

    case eLabel:
        {
            IOLabel* IO = new IOLabel(this->m_colorRule, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eColor:
        {
            IOGridColor* IO = new IOGridColor(this->m_colorRule, this->m_version);
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


void IOGridColorRule::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eLegendLabel:
        this->m_colorRule->SetLegendLabel(ch);
        break;

    case eFilter:
        this->m_colorRule->SetFilter(ch);
        break;
    }
}


void IOGridColorRule::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_colorRule->SetUnknownXml(this->m_unknownXml);

        this->m_colorStyle->GetRules()->Adopt(this->m_colorRule);
        this->m_colorStyle = NULL;
        this->m_colorRule = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOGridColorRule::Write(MdfStream& fd, GridColorRule* colorRule, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sColorRule) << std::endl;
    tab.inctab();

    // Property: Legend Lable
    fd << tab.tab() << startStr(sLegendLabel);
    fd << EncodeString(colorRule->GetLegendLabel());
    fd << endStr(sLegendLabel) << std::endl;

    // Property: Filter
    if (!colorRule->GetFilter().empty())
    {
        fd << tab.tab() << startStr(sFilter);
        fd << EncodeString(colorRule->GetFilter());
        fd << endStr(sFilter) << std::endl;
    }

    // Property: Label
    if (colorRule->GetLabel())
        IOLabel::Write(fd, colorRule->GetLabel(), version, tab);

    // Property: GridColor
    IOGridColor::Write(fd, colorRule->GetGridColor(), version, tab);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, colorRule->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sColorRule) << std::endl;
}
