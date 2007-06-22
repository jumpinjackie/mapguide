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
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOSymbolInstance::IOSymbolInstance(SymbolInstanceCollection* symbolInstanceCollection)
{
    this->m_symbolInstanceCollection = symbolInstanceCollection;
}


void IOSymbolInstance::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"SymbolInstance") // NOXLATE
    {
        this->m_startElemName = name;
        this->m_symbolInstance = new SymbolInstance();
    }
    else if (this->m_currElemName == L"SimpleSymbolDefinition") // NOXLATE
    {
        SimpleSymbolDefinition* simpleSymbol = new SimpleSymbolDefinition();
        this->m_symbolInstance->AdoptSymbolDefinition(simpleSymbol);
        IOSimpleSymbolDefinition* IO = new IOSimpleSymbolDefinition(simpleSymbol);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (this->m_currElemName == L"CompoundSymbolDefinition") // NOXLATE
    {
        CompoundSymbolDefinition* compoundSymbol = new CompoundSymbolDefinition();
        this->m_symbolInstance->AdoptSymbolDefinition(compoundSymbol);
        IOCompoundSymbolDefinition* IO = new IOCompoundSymbolDefinition(compoundSymbol);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (this->m_currElemName == L"ParameterOverrides") // NOXLATE
    {
        IOOverrideCollection* IO = new IOOverrideCollection(this->m_symbolInstance->GetParameterOverrides());
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (this->m_currElemName == L"ExtendedData1") // NOXLATE
    {
        ParseUnknownXml(name, handlerStack);
    }
}


void IOSymbolInstance::ElementChars(const wchar_t* ch)
{
         IF_STRING_PROPERTY(this->m_currElemName, this->m_symbolInstance, ResourceId, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_symbolInstance, ScaleX, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_symbolInstance, ScaleY, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_symbolInstance, InsertionOffsetX, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_symbolInstance, InsertionOffsetY, ch)
    else IF_ENUM_2(this->m_currElemName, this->m_symbolInstance, MdfModel, SizeContext, ch, DeviceUnits, MappingUnits)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_symbolInstance, DrawLast, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_symbolInstance, CheckExclusionRegion, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_symbolInstance, AddToExclusionRegion, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_symbolInstance, PositioningAlgorithm, ch)
}


void IOSymbolInstance::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_symbolInstance->SetUnknownXml(this->m_unknownXml);

        this->m_symbolInstanceCollection->Adopt(this->m_symbolInstance);
        this->m_symbolInstanceCollection = NULL;
        this->m_symbolInstance = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOSymbolInstance::Write(MdfStream& fd, SymbolInstance* symbolInstance, Version* version)
{
    fd << tab() << "<SymbolInstance>" << std::endl; // NOXLATE
    inctab();

    // we must write either a symbol definition or reference, but not both
    SymbolDefinition* symbol = symbolInstance->GetSymbolDefinition();
    if (symbol)
    {
        SimpleSymbolDefinition* simpleSymbol = dynamic_cast<SimpleSymbolDefinition*>(symbol);
        CompoundSymbolDefinition* compoundSymbol = dynamic_cast<CompoundSymbolDefinition*>(symbol);

        if (simpleSymbol)
            IOSimpleSymbolDefinition::Write(fd, simpleSymbol, false, NULL);
        else if (compoundSymbol)
            IOCompoundSymbolDefinition::Write(fd, compoundSymbol, false, NULL);
    }
    else
    {
        EMIT_STRING_PROPERTY(fd, symbolInstance, ResourceId, false, NULL)
    }

    IOOverrideCollection::Write(fd, symbolInstance->GetParameterOverrides(), version);

    EMIT_DOUBLE_PROPERTY(fd, symbolInstance, ScaleX, true, 1.0)                          // default is 1.0
    EMIT_DOUBLE_PROPERTY(fd, symbolInstance, ScaleY, true, 1.0)                          // default is 1.0
    EMIT_DOUBLE_PROPERTY(fd, symbolInstance, InsertionOffsetX, true, 0.0)                // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, symbolInstance, InsertionOffsetY, true, 0.0)                // default is 0.0
    EMIT_ENUM_2(fd, symbolInstance, MdfModel, SizeContext, DeviceUnits, MappingUnits, 1) // default is DeviceUnits
    EMIT_BOOL_PROPERTY(fd, symbolInstance, DrawLast, true, false)                        // default is false
    EMIT_BOOL_PROPERTY(fd, symbolInstance, CheckExclusionRegion, true, false)            // default is false
    EMIT_BOOL_PROPERTY(fd, symbolInstance, AddToExclusionRegion, true, false)            // default is false
    EMIT_STRING_PROPERTY(fd, symbolInstance, PositioningAlgorithm, true, L"")            // default is empty string

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, symbolInstance->GetUnknownXml(), version);

    dectab();
    fd << tab() << "</SymbolInstance>" << std::endl; // NOXLATE
}
