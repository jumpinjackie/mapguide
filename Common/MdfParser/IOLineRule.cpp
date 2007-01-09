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
#include "IOLineRule.h"
#include "IOLabel.h"
#include "IOLineSymbolization2D.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, LineRule);
ELEM_MAP_ENTRY(2, LineSymbolization2D);
ELEM_MAP_ENTRY(3, Label);
ELEM_MAP_ENTRY(4, LegendLabel);
ELEM_MAP_ENTRY(5, Filter);

IOLineRule::IOLineRule()
{
    this->_lineRule = NULL;
    this->lineTypeStyle = NULL;
}

IOLineRule::IOLineRule(LineTypeStyle * lineTypeStyle)
{
    this->_lineRule = NULL;
    this->lineTypeStyle = lineTypeStyle;
}

IOLineRule::~IOLineRule()
{
}

void IOLineRule::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eLineRule:
        m_startElemName = name;
        this->_lineRule = new LineRule();
        break;

    case eLineSymbolization2D:
        {
            IOLineSymbolization2D *IO = new IOLineSymbolization2D(this->_lineRule);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eLabel:
        {
            IOLabel *IO = new IOLabel(this->_lineRule);
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

void IOLineRule::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"LegendLabel") // NOXLATE
        (this->_lineRule)->SetLegendLabel(ch);
    else if (m_currElemName == L"Filter") // NOXLATE
        (this->_lineRule)->SetFilter(ch);
}

void IOLineRule::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        if (!UnknownXml().empty())
            this->_lineRule->SetUnknownXml(UnknownXml());

        this->lineTypeStyle->GetRules()->Adopt(this->_lineRule);
        handlerStack->pop();
        this->lineTypeStyle= NULL;
        this->_lineRule = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOLineRule::Write(MdfStream &fd, LineRule *lineRule)
{
    fd << tab() << "<LineRule>" << std::endl; // NOXLATE
    inctab();

    fd << tab() << "<LegendLabel>"; // NOXLATE
    fd << EncodeString(lineRule->GetLegendLabel());
    fd << "</LegendLabel>" << std::endl; // NOXLATE

    //Property: Filter
    if (lineRule->GetFilter().length() > 0)
    {
        fd << tab() << "<Filter>"; // NOXLATE
        fd << EncodeString(lineRule->GetFilter());
        fd << "</Filter>" << std::endl; // NOXLATE
    }

    //Property: Label
    if (lineRule->GetLabel() != NULL && lineRule->GetLabel()->GetSymbol() != NULL)
    {
        IOLabel *IO2 = new IOLabel();
        IO2->Write(fd, lineRule->GetLabel());
        delete IO2;
    }

    //Property: Symbolizations
    for (int x = 0; x < lineRule->GetSymbolizations()->GetCount(); x++)
    {
        IOLineSymbolization2D *IO = new IOLineSymbolization2D();
        IO->Write(fd, lineRule->GetSymbolizations()->GetAt(x));
        delete IO;
    }

    // Write any previously found unknown XML
    if (!lineRule->GetUnknownXml().empty())
    {
        fd << toCString(lineRule->GetUnknownXml()); 
    }

    dectab();
    fd << tab() << "</LineRule>" << std::endl; // NOXLATE
}
