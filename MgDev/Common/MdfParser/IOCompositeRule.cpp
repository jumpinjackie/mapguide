//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOCompositeRule::IOCompositeRule(CompositeTypeStyle* compositeTypeStyle, Version& version) : SAX2ElementHandler(version)
{
    this->m_compositeTypeStyle = compositeTypeStyle;
    this->m_compositeRule = NULL;
}


IOCompositeRule::~IOCompositeRule()
{
}


void IOCompositeRule::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"CompositeRule") // NOXLATE
    {
        this->m_startElemName = name;
        this->m_compositeRule = new CompositeRule();
    }
    else if (this->m_currElemName == L"CompositeSymbolization") // NOXLATE
    {
        IOCompositeSymbolization* IO = new IOCompositeSymbolization(this->m_compositeRule, this->m_version);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (this->m_currElemName == L"ExtendedData1") // NOXLATE
    {
        this->m_procExtData = true;
    }
    else
    {
        ParseUnknownXml(name, handlerStack);
    }
}


void IOCompositeRule::ElementChars(const wchar_t* ch)
{
         IF_STRING_PROPERTY(this->m_currElemName, this->m_compositeRule, LegendLabel, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_compositeRule, Filter, ch)
}


void IOCompositeRule::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_compositeRule->SetUnknownXml(this->m_unknownXml);

        this->m_compositeTypeStyle->GetRules()->Adopt(this->m_compositeRule);
        this->m_compositeTypeStyle = NULL;
        this->m_compositeRule = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (::wcscmp(name, L"ExtendedData1") == 0) // NOXLATE
    {
        this->m_procExtData = false;
    }
}


void IOCompositeRule::Write(MdfStream& fd, CompositeRule* compositeRule, Version* version, MgTab& tab)
{
    fd << tab.tab() << "<CompositeRule>" << std::endl; // NOXLATE
    tab.inctab();

    EMIT_STRING_PROPERTY(fd, compositeRule, LegendLabel, false, NULL, tab)
    EMIT_STRING_PROPERTY(fd, compositeRule, Filter, true, L"", tab) // default is empty string

    IOCompositeSymbolization::Write(fd, compositeRule->GetSymbolization(), version, tab);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, compositeRule->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << "</CompositeRule>" << std::endl; // NOXLATE
}
