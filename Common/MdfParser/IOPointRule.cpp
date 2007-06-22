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
#include "IOPointRule.h"
#include "IOLabel.h"
#include "IOPointSymbolization2D.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, PointRule);
ELEM_MAP_ENTRY(2, PointSymbolization2D);
ELEM_MAP_ENTRY(3, Label);
ELEM_MAP_ENTRY(4, LegendLabel);
ELEM_MAP_ENTRY(5, Filter);


IOPointRule::IOPointRule()
{
    this->m_pointRule = NULL;
    this->m_pointTypeStyle = NULL;
}


IOPointRule::IOPointRule(PointTypeStyle* pointTypeStyle)
{
    this->m_pointRule = NULL;
    this->m_pointTypeStyle = pointTypeStyle;
}


IOPointRule::~IOPointRule()
{
}


void IOPointRule::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case ePointRule:
        this->m_startElemName = name;
        this->m_pointRule = new PointRule();
        break;

    case ePointSymbolization2D:
        {
            IOPointSymbolization2D* IO = new IOPointSymbolization2D(this->m_pointRule);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eLabel:
        {
            IOLabel* IO = new IOLabel(this->m_pointRule);
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


void IOPointRule::ElementChars(const wchar_t* ch)
{
    if (this->m_currElemName == L"LegendLabel") // NOXLATE
        this->m_pointRule->SetLegendLabel(ch);
    else if (this->m_currElemName == L"Filter") // NOXLATE
        this->m_pointRule->SetFilter(ch);
}


void IOPointRule::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_pointRule->SetUnknownXml(this->m_unknownXml);

        this->m_pointTypeStyle->GetRules()->Adopt(this->m_pointRule);
        this->m_pointTypeStyle = NULL;
        this->m_pointRule = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOPointRule::Write(MdfStream& fd, PointRule* pointRule, Version* version)
{
    fd << tab() << "<PointRule>" << std::endl; // NOXLATE
    inctab();

    //Property: LegendLabel
    fd << tab() << "<LegendLabel>"; // NOXLATE
    fd << EncodeString(pointRule->GetLegendLabel());
    fd << "</LegendLabel>" << std::endl; // NOXLATE

    //Property: Filter
    if (pointRule->GetFilter().length() > 0)
    {
        fd << tab() << "<Filter>"; // NOXLATE
        fd << EncodeString(pointRule->GetFilter());
        fd << "</Filter>" << std::endl; // NOXLATE
    }
    //Property: Label
    if (pointRule->GetLabel() != NULL && pointRule->GetLabel()->GetSymbol() != NULL)
        IOLabel::Write(fd, pointRule->GetLabel(), version);

    //Property: Symbolization
    PointSymbolization2D* symbolization2d = static_cast<PointSymbolization2D*>(pointRule->GetSymbolization());
    if (symbolization2d != NULL)
        IOPointSymbolization2D::Write(fd, symbolization2d, version);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, pointRule->GetUnknownXml(), version);

    dectab();
    fd << tab() << "</PointRule>" << std::endl; // NOXLATE
}
