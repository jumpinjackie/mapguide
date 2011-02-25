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
#include "IOLineRule.h"
#include "IOLabel.h"
#include "IOLineSymbolization2D.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, LineRule);
ELEM_MAP_ENTRY(2, LegendLabel);
ELEM_MAP_ENTRY(3, Filter);
ELEM_MAP_ENTRY(4, Label);
ELEM_MAP_ENTRY(5, LineSymbolization2D);
ELEM_MAP_ENTRY(6, ExtendedData1);


IOLineRule::IOLineRule(Version& version) : SAX2ElementHandler(version)
{
    this->m_lineRule = NULL;
    this->m_lineTypeStyle = NULL;
}


IOLineRule::IOLineRule(LineTypeStyle* lineTypeStyle, Version& version) : SAX2ElementHandler(version)
{
    this->m_lineRule = NULL;
    this->m_lineTypeStyle = lineTypeStyle;
}


IOLineRule::~IOLineRule()
{
}


void IOLineRule::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eLineRule:
        this->m_startElemName = name;
        this->m_lineRule = new LineRule();
        break;

    case eLabel:
        {
            IOLabel* IO = new IOLabel(this->m_lineRule, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eLineSymbolization2D:
        {
            IOLineSymbolization2D* IO = new IOLineSymbolization2D(this->m_lineRule, this->m_version);
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


void IOLineRule::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eLegendLabel:
        this->m_lineRule->SetLegendLabel(ch);
        break;

    case eFilter:
        this->m_lineRule->SetFilter(ch);
        break;
    }
}


void IOLineRule::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_lineRule->SetUnknownXml(this->m_unknownXml);

        this->m_lineTypeStyle->GetRules()->Adopt(this->m_lineRule);
        this->m_lineTypeStyle = NULL;
        this->m_lineRule = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOLineRule::Write(MdfStream& fd, LineRule* lineRule, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sLineRule) << std::endl;
    tab.inctab();

    // Property: LegendLabel
    fd << tab.tab() << startStr(sLegendLabel);
    fd << EncodeString(lineRule->GetLegendLabel());
    fd << endStr(sLegendLabel) << std::endl;

    // Property: Filter
    if (!lineRule->GetFilter().empty())
    {
        fd << tab.tab() << startStr(sFilter);
        fd << EncodeString(lineRule->GetFilter());
        fd << endStr(sFilter) << std::endl;
    }

    // Property: Label
    if (lineRule->GetLabel() && lineRule->GetLabel()->GetSymbol())
        IOLabel::Write(fd, lineRule->GetLabel(), version, tab);

    // Property: Symbolizations
    for (int i=0; i<lineRule->GetSymbolizations()->GetCount(); ++i)
        IOLineSymbolization2D::Write(fd, lineRule->GetSymbolizations()->GetAt(i), version, tab);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, lineRule->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sLineRule) << std::endl;
}
