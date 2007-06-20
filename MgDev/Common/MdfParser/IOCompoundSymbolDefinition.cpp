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
#include "IOCompoundSymbolDefinition.h"
#include "IOSimpleSymbol.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOCompoundSymbolDefinition::IOCompoundSymbolDefinition(CompoundSymbolDefinition* symbol)
{
    this->_symbolDefinition = symbol;
}


void IOCompoundSymbolDefinition::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"CompoundSymbolDefinition") // NOXLATE
    {
        m_startElemName = name;
    }
    else if (m_currElemName == L"SimpleSymbol") // NOXLATE
    {
        IOSimpleSymbol* IO = new IOSimpleSymbol(this->_symbolDefinition->GetSymbols());
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (m_currElemName == L"ExtendedData1") // NOXLATE
    {
        ParseUnknownXml(name, handlerStack);
    }
}


void IOCompoundSymbolDefinition::ElementChars(const wchar_t* ch)
{
         IF_STRING_PROPERTY(m_currElemName, this->_symbolDefinition, Name, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_symbolDefinition, Description, ch)
}


void IOCompoundSymbolDefinition::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        this->_symbolDefinition->SetUnknownXml(UnknownXml());

        this->_symbolDefinition = NULL;
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOCompoundSymbolDefinition::Write(MdfStream& fd, CompoundSymbolDefinition* symbolDefinition, bool writeAsRootElement, Version* version)
{
    if (writeAsRootElement)
    {
        // we currently only support symbol definition version 1.0.0
        if (version && (*version != Version(1, 0, 0)))
        {
            // TODO - need a way to return error information
            _ASSERT(false);
            return;
        }

        fd << tab() << "<CompoundSymbolDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"SymbolDefinition-1.0.0.xsd\" version=\"1.0.0\">" << std::endl; // NOXLATE
    }
    else
        fd << tab() << "<CompoundSymbolDefinition>" << std::endl; // NOXLATE
    inctab();

    EMIT_STRING_PROPERTY(fd, symbolDefinition, Name, false, NULL)
    EMIT_STRING_PROPERTY(fd, symbolDefinition, Description, true, L"") // default is empty string

    SimpleSymbolCollection* symbolCollection = symbolDefinition->GetSymbols();
    int numElements = symbolCollection->GetCount();
    for (int i=0; i<numElements; ++i)
        IOSimpleSymbol::Write(fd, symbolCollection->GetAt(i), version);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, symbolDefinition->GetUnknownXml(), version);

    dectab();
    fd << tab() << "</CompoundSymbolDefinition>" << std::endl; // NOXLATE
}
