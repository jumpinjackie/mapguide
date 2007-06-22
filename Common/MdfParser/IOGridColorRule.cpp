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
#include "IOGridColorRule.h"
#include "IOGridColor.h"
#include "IOExtra.h"
#include "IOLabel.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, ColorRule);
ELEM_MAP_ENTRY(2, Color);
ELEM_MAP_ENTRY(3, Label);
ELEM_MAP_ENTRY(4, LegendLabel);
ELEM_MAP_ENTRY(5, Filter);


IOGridColorRule::IOGridColorRule()
{
    this->m_colorStyle = NULL;
    this->m_colorRule = NULL;
}


IOGridColorRule::IOGridColorRule(GridColorStyle* colorStyle)
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

    case eColor:
        {
            IOGridColor* IO = new IOGridColor(this->m_colorRule);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eLabel:
        {
            IOLabel* IO = new IOLabel(this->m_colorRule);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;

    default:
        break;
    }
}


void IOGridColorRule::ElementChars(const wchar_t* ch)
{
    if (this->m_currElemName == L"LegendLabel") // NOXLATE
        this->m_colorRule->SetLegendLabel(ch);
    else if (this->m_currElemName == L"Filter") // NOXLATE
        this->m_colorRule->SetFilter(ch);
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
}


void IOGridColorRule::Write(MdfStream& fd, GridColorRule* colorRule, Version* version)
{
    fd << tab() << "<ColorRule>" << std::endl; // NOXLATE
    inctab();

    //Property: Legend Lable
    fd << tab() << "<LegendLabel>"; // NOXLATE
    fd << EncodeString(colorRule->GetLegendLabel());
    fd << "</LegendLabel>" << std::endl; // NOXLATE

    //Property: Filter
    if (colorRule->GetFilter() != toMdfString(""))
    {
        fd << tab() << "<Filter>"; // NOXLATE
        fd << EncodeString(colorRule->GetFilter());
        fd << "</Filter>" << std::endl; // NOXLATE
    }

    // Property: Label
    if (colorRule->GetLabel() != NULL)
        IOLabel::Write(fd, colorRule->GetLabel(), version);

    // Property: GridColor
    IOGridColor::Write(fd, colorRule->GetGridColor(), version);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, colorRule->GetUnknownXml(), version);

    dectab();
    fd << tab() << "</ColorRule>" << std::endl; // NOXLATE
}
