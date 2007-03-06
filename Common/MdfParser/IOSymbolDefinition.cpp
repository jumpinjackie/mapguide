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
#include "IOSymbolDefinition.h"
#include "SimpleSymbolDefinition.h"
#include "CompoundSymbolDefinition.h"
#include "IOSimpleSymbolDefinition.h"
#include "IOCompoundSymbolDefinition.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOSymbolDefinition::IOSymbolDefinition(SymbolInstance* symbolInstance)
{
    this->_symbolInstance = symbolInstance;
    this->_symbolDefinition = NULL;
}

void IOSymbolDefinition::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"SymbolDefinition") // NOXLATE
    {
        m_startElemName = name;
    }
    else if (m_currElemName == L"SimpleSymbolDefinition") // NOXLATE
    {
        SimpleSymbolDefinition* simpleSymbol = new SimpleSymbolDefinition();
        this->_symbolDefinition = simpleSymbol;
        IOSimpleSymbolDefinition* IO = new IOSimpleSymbolDefinition(simpleSymbol);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (m_currElemName == L"CompoundSymbolDefinition") // NOXLATE
    {
        CompoundSymbolDefinition* compoundSymbol = new CompoundSymbolDefinition();
        this->_symbolDefinition = compoundSymbol;
        IOCompoundSymbolDefinition* IO = new IOCompoundSymbolDefinition(compoundSymbol);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
}

void IOSymbolDefinition::ElementChars(const wchar_t *ch)
{
}

void IOSymbolDefinition::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        this->_symbolInstance->AdoptSymbolDefinition(this->_symbolDefinition);
        this->_symbolInstance = NULL;
        this->_symbolDefinition = NULL;
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}

void IOSymbolDefinition::Write(MdfStream &fd, SymbolDefinition* symbolDefinition, bool writeAsRootElement)
{
    if (writeAsRootElement)
        fd << tab() << "<SymbolDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:xsd=\"http://www.w3.org/2001/XMLSchema\" xsi:noNamespaceSchemaLocation=\"SymbolDefinition-1.0.0.xsd\">" << std::endl; // NOXLATE
    else
        fd << tab() << "<SymbolDefinition>" << std::endl; // NOXLATE
    inctab();

    SimpleSymbolDefinition* simpleSymbol = dynamic_cast<SimpleSymbolDefinition*>(symbolDefinition);
    if (simpleSymbol)
        IOSimpleSymbolDefinition::Write(fd, simpleSymbol);

    CompoundSymbolDefinition* compoundSymbol = dynamic_cast<CompoundSymbolDefinition*>(symbolDefinition);
    if (compoundSymbol)
        IOCompoundSymbolDefinition::Write(fd, compoundSymbol);

    dectab();
    fd << tab() << "</SymbolDefinition>" << std::endl; // NOXLATE
}
