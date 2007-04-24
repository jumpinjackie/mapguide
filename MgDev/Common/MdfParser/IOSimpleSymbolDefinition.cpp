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
#include "IOSimpleSymbolDefinition.h"
#include "IOGraphicElementCollection.h"
#include "IOResizeBox.h"
#include "IOPointUsage.h"
#include "IOLineUsage.h"
#include "IOAreaUsage.h"
#include "IOParameterCollection.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOSimpleSymbolDefinition::IOSimpleSymbolDefinition(SimpleSymbolDefinition* symbolDefinition)
{
    this->_symbolDefinition = symbolDefinition;
}

void IOSimpleSymbolDefinition::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"SimpleSymbolDefinition") // NOXLATE
    {
        m_startElemName = name;
    }
    else if (m_currElemName == L"Graphics") // NOXLATE
    {
        IOGraphicElementCollection* IO = new IOGraphicElementCollection(this->_symbolDefinition->GetGraphics());
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (m_currElemName == L"ResizeBox") // NOXLATE
    {
        IOResizeBox* IO = new IOResizeBox(this->_symbolDefinition);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (m_currElemName == L"PointUsage") // NOXLATE
    {
        IOPointUsage* IO = new IOPointUsage(this->_symbolDefinition);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (m_currElemName == L"LineUsage") // NOXLATE
    {
        IOLineUsage* IO = new IOLineUsage(this->_symbolDefinition);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (m_currElemName == L"AreaUsage") // NOXLATE
    {
        IOAreaUsage* IO = new IOAreaUsage(this->_symbolDefinition);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (m_currElemName == L"ParameterDefinition") // NOXLATE
    {
        IOParameterCollection* IO = new IOParameterCollection(this->_symbolDefinition->GetParameterDefinition());
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (m_currElemName == L"ExtendedData1") // NOXLATE
    {
        ParseUnknownXml(name, handlerStack);
    }
}

void IOSimpleSymbolDefinition::ElementChars(const wchar_t *ch)
{
         IF_STRING_PROPERTY(m_currElemName, this->_symbolDefinition, Name, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_symbolDefinition, Description, ch)
}

void IOSimpleSymbolDefinition::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        if (!UnknownXml().empty())
            this->_symbolDefinition->SetUnknownXml(UnknownXml());

        this->_symbolDefinition = NULL;
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}

void IOSimpleSymbolDefinition::Write(MdfStream &fd, SimpleSymbolDefinition* symbolDefinition, bool writeAsRootElement, Version* version)
{
    if (writeAsRootElement)
    {
        MdfString strVersion = version? version->ToString() : L"1.0.0";
        fd << tab() << "<SimpleSymbolDefinition xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"SymbolDefinition-" << EncodeString(strVersion) << ".xsd\" version=\"" << EncodeString(strVersion) << "\">" << std::endl; // NOXLATE
    }
    else
        fd << tab() << "<SimpleSymbolDefinition>" << std::endl; // NOXLATE
    inctab();

    EMIT_STRING_PROPERTY(fd, symbolDefinition, Name, false, NULL)
    EMIT_STRING_PROPERTY(fd, symbolDefinition, Description, true, L"") // default is empty string

    IOGraphicElementCollection::Write(fd, symbolDefinition->GetGraphics());

    if (symbolDefinition->GetResizeBox())
        IOResizeBox::Write(fd, symbolDefinition->GetResizeBox());

    if (symbolDefinition->GetPointUsage())
        IOPointUsage::Write(fd, symbolDefinition->GetPointUsage());

    if (symbolDefinition->GetLineUsage())
        IOLineUsage::Write(fd, symbolDefinition->GetLineUsage());

    if (symbolDefinition->GetAreaUsage())
        IOAreaUsage::Write(fd, symbolDefinition->GetAreaUsage());

    IOParameterCollection::Write(fd, symbolDefinition->GetParameterDefinition());

    // write any previously found unknown XML
    if (!symbolDefinition->GetUnknownXml().empty())
        fd << tab() << toCString(symbolDefinition->GetUnknownXml()) << std::endl;

    dectab();
    fd << tab() << "</SimpleSymbolDefinition>" << std::endl; // NOXLATE
}
