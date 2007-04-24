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
#include "IOSimpleSymbol.h"
#include "IOSimpleSymbolDefinition.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOSimpleSymbol::IOSimpleSymbol(SimpleSymbolCollection* symbolCollection)
{
    this->_symbolCollection = symbolCollection;
    this->_simpleSymbol = NULL;
}

void IOSimpleSymbol::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"SimpleSymbol") // NOXLATE
    {
        m_startElemName = name;
        this->_simpleSymbol = new SimpleSymbol();
    }
    else if (m_currElemName == L"SimpleSymbolDefinition") // NOXLATE
    {
        SimpleSymbolDefinition* symbolDefinition = new SimpleSymbolDefinition();
        this->_simpleSymbol->AdoptSymbolDefinition(symbolDefinition);
        IOSimpleSymbolDefinition* IO = new IOSimpleSymbolDefinition(symbolDefinition);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
}

void IOSimpleSymbol::ElementChars(const wchar_t *ch)
{
         IF_STRING_PROPERTY(m_currElemName, this->_simpleSymbol, ResourceId, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_simpleSymbol, RenderingPass, ch)
}

void IOSimpleSymbol::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        this->_symbolCollection->Adopt(this->_simpleSymbol);
        this->_symbolCollection = NULL;
        this->_simpleSymbol = NULL;
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}

void IOSimpleSymbol::Write(MdfStream &fd, SimpleSymbol* simpleSymbol, Version* version)
{
    fd << tab() << "<SimpleSymbol>" << std::endl; // NOXLATE
    inctab();

    // we must write either a symbol definition or reference, but not both
    if (simpleSymbol->GetSymbolDefinition())
        IOSimpleSymbolDefinition::Write(fd, simpleSymbol->GetSymbolDefinition(), false, version);
    else
    {
        EMIT_STRING_PROPERTY(fd, simpleSymbol, ResourceId, false, NULL)
    }

    EMIT_INTEGER_PROPERTY(fd, simpleSymbol, RenderingPass, true, 0) // default is 0

    dectab();
    fd << tab() << "</SimpleSymbol>" << std::endl; // NOXLATE
}
