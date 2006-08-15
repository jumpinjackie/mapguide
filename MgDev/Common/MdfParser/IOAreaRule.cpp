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
#include "IOAreaRule.h"
#include "IOLabel.h"
#include "IOAreaSymbolization2D.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOAreaRule::IOAreaRule()
{
    this->_areaRule = NULL;
    this->areaTypeStyle = NULL;
}

IOAreaRule::IOAreaRule(AreaTypeStyle * areaTypeStyle)
{
    this->_areaRule = NULL;
    this->areaTypeStyle = areaTypeStyle;
}

IOAreaRule::~IOAreaRule()
{
}

void IOAreaRule::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"AreaRule") // NOXLATE
    {
        m_startElemName = name;
        this->_areaRule = new AreaRule();

    }
    else if (m_currElemName == L"AreaSymbolization2D") // NOXLATE
    {
        IOAreaSymbolization2D *IO = new IOAreaSymbolization2D(this->_areaRule);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (m_currElemName == L"Label") // NOXLATE
    {
        IOLabel *IO = new IOLabel(this->_areaRule);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
}

void IOAreaRule::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"LegendLabel") // NOXLATE
        (this->_areaRule)->SetLegendLabel(ch);
    else if (m_currElemName == L"Filter") // NOXLATE
        (this->_areaRule)->SetFilter(ch);
}

void IOAreaRule::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        this->areaTypeStyle->GetRules()->Adopt(this->_areaRule);
        handlerStack->pop();
        this->areaTypeStyle= NULL;
        this->_areaRule = NULL;
        m_startElemName = L"";
        delete this;
    }
}

void IOAreaRule::Write(MdfStream &fd, AreaRule *areaRule)
{
    fd << tab() << "<AreaRule>" << std::endl;  // NOXLATE
    inctab();

    //Property: LegendLabel
    fd << tab() << "<LegendLabel>";  // NOXLATE
    fd << EncodeString(areaRule->GetLegendLabel());
    fd << "</LegendLabel>" << std::endl; // NOXLATE

    //Property: Filter
    if (areaRule->GetFilter().length() > 0)
    {
        fd << tab() << "<Filter>"; // NOXLATE
        fd << EncodeString(areaRule->GetFilter());
        fd << "</Filter>" << std::endl; // NOXLATE
    }

    //Property: Label
    if (areaRule->GetLabel() != NULL && areaRule->GetLabel()->GetSymbol() != NULL)
    {
        IOLabel *IO2 = new IOLabel();
        IO2->Write(fd, areaRule->GetLabel());
        delete IO2;
    }

    //Property: Symbolization
    AreaSymbolization2D *symbolization = areaRule->GetSymbolization();
    IOAreaSymbolization2D *IO = new IOAreaSymbolization2D();
    IO->Write(fd, symbolization);
    delete IO;

    dectab();
    fd << tab() << "</AreaRule>" << std::endl; // NOXLATE
}
