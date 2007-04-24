//
//  Copyright (C) 2007 by Autodesk, Inc.
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
#include "IOCompositeRule.h"
#include "IOCompositeSymbolization.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOCompositeRule::IOCompositeRule(CompositeTypeStyle* compositeTypeStyle)
{
    this->_compositeTypeStyle = compositeTypeStyle;
    this->_compositeRule = NULL;
}

IOCompositeRule::~IOCompositeRule()
{
}

void IOCompositeRule::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"CompositeRule") // NOXLATE
    {
        m_startElemName = name;
        this->_compositeRule = new CompositeRule();
    }
    else if (m_currElemName == L"CompositeSymbolization") // NOXLATE
    {
        IOCompositeSymbolization* IO = new IOCompositeSymbolization(this->_compositeRule);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (m_currElemName == L"ExtendedData1") // NOXLATE
    {
        ParseUnknownXml(name, handlerStack);
    }
}

void IOCompositeRule::ElementChars(const wchar_t *ch)
{
         IF_STRING_PROPERTY(m_currElemName, this->_compositeRule, LegendLabel, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_compositeRule, Filter, ch)
}

void IOCompositeRule::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        if (!UnknownXml().empty())
            this->_compositeRule->SetUnknownXml(UnknownXml());

        this->_compositeTypeStyle->GetRules()->Adopt(this->_compositeRule);
        this->_compositeTypeStyle = NULL;
        this->_compositeRule = NULL;
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}

void IOCompositeRule::Write(MdfStream &fd, CompositeRule* compositeRule, Version* version)
{
    fd << tab() << "<CompositeRule>" << std::endl; // NOXLATE
    inctab();

    EMIT_STRING_PROPERTY(fd, compositeRule, LegendLabel, false, NULL)
    EMIT_STRING_PROPERTY(fd, compositeRule, Filter, true, L"") // default is empty string

    IOCompositeSymbolization::Write(fd, compositeRule->GetSymbolization(), version);

    // write any previously found unknown XML
    if (!compositeRule->GetUnknownXml().empty())
        fd << tab() << toCString(compositeRule->GetUnknownXml()) << std::endl;

    dectab();
    fd << tab() << "</CompositeRule>" << std::endl; // NOXLATE
}
