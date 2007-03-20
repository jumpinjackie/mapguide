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
#include "IOSymbolDefinition.h"

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
    else if (m_currElemName == L"SymbolDefinition") // NOXLATE
    {
        IOSymbolDefinition* IO = new IOSymbolDefinition(this->_symbolInstance);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
}

void IOSymbolInstance::ElementChars(const wchar_t *ch)
{
         IF_STRING_PROPERTY(m_currElemName, this->_symbolInstance, SymbolReference, ch)
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
        this->_symbolInstanceCollection->Adopt(this->_symbolInstance);
        this->_symbolInstanceCollection = NULL;
        this->_symbolInstance = NULL;
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}

void IOSymbolInstance::Write(MdfStream &fd, SymbolInstance* symbolInstance)
{
    fd << tab() << "<SymbolInstance>" << std::endl; // NOXLATE
    inctab();

    // we must write either a symbol definition or reference, but not both
    if (symbolInstance->GetSymbolDefinition())
        IOSymbolDefinition::Write(fd, symbolInstance->GetSymbolDefinition(), false);
    else
    {
        EMIT_STRING_PROPERTY(fd, symbolInstance, SymbolReference, false)
    }

    EMIT_STRING_PROPERTY(fd, symbolInstance, ScaleX, true)
    EMIT_STRING_PROPERTY(fd, symbolInstance, ScaleY, true)
    EMIT_STRING_PROPERTY(fd, symbolInstance, InsertionOffsetX, true)
    EMIT_STRING_PROPERTY(fd, symbolInstance, InsertionOffsetY, true)
    EMIT_ENUM_2(fd, symbolInstance, MdfModel, SizeContext, DeviceUnits, MappingUnits, 1) // DeviceUnits is default
    EMIT_STRING_PROPERTY(fd, symbolInstance, DrawLast, true)
    EMIT_STRING_PROPERTY(fd, symbolInstance, CheckExclusionRegion, true)
    EMIT_STRING_PROPERTY(fd, symbolInstance, AddToExclusionRegion, true)
    EMIT_STRING_PROPERTY(fd, symbolInstance, PositioningAlgorithm, true)

    dectab();
    fd << tab() << "</SymbolInstance>" << std::endl; // NOXLATE
}
