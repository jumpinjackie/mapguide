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
#include "IOSymbolInstance.h"
#include "IOOverrideCollection.h"
#include "IOSimpleSymbolDefinition.h"
#include "IOCompoundSymbolDefinition.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOSymbolInstance::IOSymbolInstance(SymbolInstanceCollection* symbolInstanceCollection)
{
    this->_symbolInstanceCollection = symbolInstanceCollection;
}

void IOSymbolInstance::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"SymbolInstance") // NOXLATE
    {
        m_startElemName = name;
        this->_symbolInstance = new SymbolInstance();
    }
    else if (m_currElemName == L"SimpleSymbolDefinition") // NOXLATE
    {
        SimpleSymbolDefinition* simpleSymbol = new SimpleSymbolDefinition();
        this->_symbolInstance->AdoptSymbolDefinition(simpleSymbol);
        IOSimpleSymbolDefinition* IO = new IOSimpleSymbolDefinition(simpleSymbol);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (m_currElemName == L"CompoundSymbolDefinition") // NOXLATE
    {
        CompoundSymbolDefinition* compoundSymbol = new CompoundSymbolDefinition();
        this->_symbolInstance->AdoptSymbolDefinition(compoundSymbol);
        IOCompoundSymbolDefinition* IO = new IOCompoundSymbolDefinition(compoundSymbol);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (m_currElemName == L"ParameterOverrides") // NOXLATE
    {
        IOOverrideCollection* IO = new IOOverrideCollection(this->_symbolInstance->GetParameterOverrides());
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (m_currElemName == L"ExtendedData1") // NOXLATE
    {
        ParseUnknownXml(name, handlerStack);
    }
}

void IOSymbolInstance::ElementChars(const wchar_t *ch)
{
         IF_STRING_PROPERTY(m_currElemName, this->_symbolInstance, ResourceId, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_symbolInstance, ScaleX, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_symbolInstance, ScaleY, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_symbolInstance, InsertionOffsetX, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_symbolInstance, InsertionOffsetY, ch)
    else IF_ENUM_2(m_currElemName, this->_symbolInstance, MdfModel, SizeContext, ch, DeviceUnits, MappingUnits)
    else IF_STRING_PROPERTY(m_currElemName, this->_symbolInstance, DrawLast, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_symbolInstance, CheckExclusionRegion, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_symbolInstance, AddToExclusionRegion, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_symbolInstance, PositioningAlgorithm, ch)
}

void IOSymbolInstance::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        if (!UnknownXml().empty())
            this->_symbolInstance->SetUnknownXml(UnknownXml());

        this->_symbolInstanceCollection->Adopt(this->_symbolInstance);
        this->_symbolInstanceCollection = NULL;
        this->_symbolInstance = NULL;
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}

void IOSymbolInstance::Write(MdfStream &fd, SymbolInstance* symbolInstance, Version* version)
{
    fd << tab() << "<SymbolInstance>" << std::endl; // NOXLATE
    inctab();

    // we must write either a symbol definition or reference, but not both
    SymbolDefinition* pSymbol = symbolInstance->GetSymbolDefinition();
    if (pSymbol)
    {
        SimpleSymbolDefinition* pSimpleSymbol = dynamic_cast<SimpleSymbolDefinition*>(pSymbol);
        CompoundSymbolDefinition* pCompoundSymbol = dynamic_cast<CompoundSymbolDefinition*>(pSymbol);

        if (pSimpleSymbol)
            IOSimpleSymbolDefinition::Write(fd, pSimpleSymbol, false, version);
        else if (pCompoundSymbol)
            IOCompoundSymbolDefinition::Write(fd, pCompoundSymbol, false, version);
    }
    else
    {
        EMIT_STRING_PROPERTY(fd, symbolInstance, ResourceId, false, NULL)
    }

    IOOverrideCollection::Write(fd, symbolInstance->GetParameterOverrides());

    EMIT_DOUBLE_PROPERTY(fd, symbolInstance, ScaleX, true, 1.0)                          // default is 1.0
    EMIT_DOUBLE_PROPERTY(fd, symbolInstance, ScaleY, true, 1.0)                          // default is 1.0
    EMIT_DOUBLE_PROPERTY(fd, symbolInstance, InsertionOffsetX, true, 0.0)                // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, symbolInstance, InsertionOffsetY, true, 0.0)                // default is 0.0
    EMIT_ENUM_2(fd, symbolInstance, MdfModel, SizeContext, DeviceUnits, MappingUnits, 1) // default is DeviceUnits
    EMIT_BOOL_PROPERTY(fd, symbolInstance, DrawLast, true, false)                        // default is false
    EMIT_BOOL_PROPERTY(fd, symbolInstance, CheckExclusionRegion, true, false)            // default is false
    EMIT_BOOL_PROPERTY(fd, symbolInstance, AddToExclusionRegion, true, false)            // default is false
    EMIT_STRING_PROPERTY(fd, symbolInstance, PositioningAlgorithm, true, L"")            // default is empty string

    // write any previously found unknown XML
    if (!symbolInstance->GetUnknownXml().empty())
        fd << tab() << toCString(symbolInstance->GetUnknownXml()) << std::endl;

    dectab();
    fd << tab() << "</SymbolInstance>" << std::endl; // NOXLATE
}
