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

IOLineRule::IOLineRule()
{
}

IOLineRule::IOLineRule(LineTypeStyle * lineTypeStyle)
{
    this->lineTypeStyle = lineTypeStyle;
}

IOLineRule::~IOLineRule()
{
}

void IOLineRule::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"LineRule") // NOXLATE
    {
        m_startElemName = name;
        this->_lineRule = new LineRule();
    }
    else if (m_currElemName == L"LineSymbolization2D") // NOXLATE
    {
        IOLineSymbolization2D *IO = new IOLineSymbolization2D(this->_lineRule);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (m_currElemName == L"Label") // NOXLATE
    {
        IOLabel *IO = new IOLabel(this->_lineRule);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
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

    dectab();
    fd << tab() << "</LineRule>" << std::endl; // NOXLATE
}
