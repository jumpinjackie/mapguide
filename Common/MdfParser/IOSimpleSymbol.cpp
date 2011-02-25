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
#include "IOSimpleSymbol.h"
#include "IOSimpleSymbolDefinition.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOSimpleSymbol::IOSimpleSymbol(SimpleSymbolCollection* symbolCollection, Version& version) : SAX2ElementHandler(version)
{
    this->m_symbolCollection = symbolCollection;
    this->m_simpleSymbol = NULL;
}


void IOSimpleSymbol::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"SimpleSymbol") // NOXLATE
    {
        this->m_startElemName = name;
        this->m_simpleSymbol = new SimpleSymbol();
    }
    else if (this->m_currElemName == L"SimpleSymbolDefinition") // NOXLATE
    {
        SimpleSymbolDefinition* symbolDefinition = new SimpleSymbolDefinition();
        this->m_simpleSymbol->AdoptSymbolDefinition(symbolDefinition);
        IOSimpleSymbolDefinition* IO = new IOSimpleSymbolDefinition(symbolDefinition, this->m_version);
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


void IOSimpleSymbol::ElementChars(const wchar_t* ch)
{
         IF_STRING_PROPERTY(this->m_currElemName, this->m_simpleSymbol, ResourceId, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_simpleSymbol, RenderingPass, ch)
}


void IOSimpleSymbol::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_simpleSymbol->SetUnknownXml(this->m_unknownXml);

        this->m_symbolCollection->Adopt(this->m_simpleSymbol);
        this->m_symbolCollection = NULL;
        this->m_simpleSymbol = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (::wcscmp(name, L"ExtendedData1") == 0) // NOXLATE
    {
        this->m_procExtData = false;
    }
}


void IOSimpleSymbol::Write(MdfStream& fd, SimpleSymbol* simpleSymbol, Version* version, MgTab& tab)
{
    fd << tab.tab() << "<SimpleSymbol>" << std::endl; // NOXLATE
    tab.inctab();

    // we must write either a symbol definition or reference, but not both
    if (simpleSymbol->GetSymbolDefinition())
        IOSimpleSymbolDefinition::Write(fd, simpleSymbol->GetSymbolDefinition(), false, version, tab);
    else
    {
        EMIT_STRING_PROPERTY(fd, simpleSymbol, ResourceId, false, NULL, tab)
    }

    EMIT_INTEGER_PROPERTY(fd, simpleSymbol, RenderingPass, true, 0, tab) // default is 0

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, simpleSymbol->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << "</SimpleSymbol>" << std::endl; // NOXLATE
}
