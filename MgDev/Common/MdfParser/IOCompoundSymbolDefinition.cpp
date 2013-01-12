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
#include "IOCompoundSymbolDefinition.h"
#include "IOSimpleSymbol.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOCompoundSymbolDefinition::IOCompoundSymbolDefinition(CompoundSymbolDefinition* symbol, Version& version) : SAX2ElementHandler(version)
{
    this->m_symbolDefinition = symbol;
}


void IOCompoundSymbolDefinition::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"CompoundSymbolDefinition") // NOXLATE
    {
        this->m_startElemName = name;
    }
    else if (this->m_currElemName == L"SimpleSymbol") // NOXLATE
    {
        IOSimpleSymbol* IO = new IOSimpleSymbol(this->m_symbolDefinition->GetSymbols(), this->m_version);
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


void IOCompoundSymbolDefinition::ElementChars(const wchar_t* ch)
{
         IF_STRING_PROPERTY(this->m_currElemName, this->m_symbolDefinition, Name, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_symbolDefinition, Description, ch)
}


void IOCompoundSymbolDefinition::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_symbolDefinition->SetUnknownXml(this->m_unknownXml);

        this->m_symbolDefinition = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (::wcscmp(name, L"ExtendedData1") == 0) // NOXLATE
    {
        this->m_procExtData = false;
    }
}


void IOCompoundSymbolDefinition::Write(MdfStream& fd, CompoundSymbolDefinition* symbolDefinition, bool writeAsRootElement, Version* version, MgTab& tab)
{
    if (writeAsRootElement)
    {
        // verify the SymbolDefinition version
        MdfString strVersion;
        if (version)
        {
            if ((*version >= Version(1, 0, 0)) && (*version <= Version(2, 4, 0)))
            {
                // SymbolDefinition in MapGuide 2008 - current
                strVersion = version->ToString();
            }
            else
            {
                // unsupported SymbolDefinition version
                // TODO - need a way to return error information
                _ASSERT(false);
                return;
            }
        }
        else
        {
            // use the current highest version
            strVersion = L"2.4.0";
        }

        fd << tab.tab() << "<CompoundSymbolDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"SymbolDefinition-" << EncodeString(strVersion) << ".xsd\" version=\"" << EncodeString(strVersion) << "\">" << std::endl; // NOXLATE
    }
    else
        fd << tab.tab() << "<CompoundSymbolDefinition>" << std::endl; // NOXLATE
    tab.inctab();

    EMIT_STRING_PROPERTY(fd, symbolDefinition, Name, false, NULL, tab)
    EMIT_STRING_PROPERTY(fd, symbolDefinition, Description, true, L"", tab) // default is empty string

    SimpleSymbolCollection* symbolCollection = symbolDefinition->GetSymbols();
    int numElements = symbolCollection->GetCount();
    for (int i=0; i<numElements; ++i)
        IOSimpleSymbol::Write(fd, symbolCollection->GetAt(i), version, tab);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, symbolDefinition->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << "</CompoundSymbolDefinition>" << std::endl; // NOXLATE
}
