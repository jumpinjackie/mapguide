//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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
#include "IOPrintLayoutElement.h"
#include "IOStringObjectCollection.h"
#include "IOExtent3D.h"
#include "IOStylizationConfiguration.h"
#include "IODataConfiguration.h"
#include "IOUnknown.h"

CREATE_ELEMENT_MAP;
// Inherited Elements
ELEM_MAP_ENTRY(2, References);
ELEM_MAP_ENTRY(3, Description);
ELEM_MAP_ENTRY(4, Units);
ELEM_MAP_ENTRY(5, Definition);
ELEM_MAP_ENTRY(6, Extents);
ELEM_MAP_ENTRY(7, Stylization);
ELEM_MAP_ENTRY(8, Data);
ELEM_MAP_ENTRY(9, ExtendedData1);
// Sub Elements
ELEM_MAP_ENTRY(10, ResourceId);

IOPrintLayoutElement::IOPrintLayoutElement(PrintLayoutElement* printLayoutElement, Version& version) :
    SAX2ElementHandler(version),
    m_printLayoutElement(printLayoutElement)
{
    // The parser will update all the data of the object pointed by the following assigned pointer.
    _ASSERT(NULL != m_printLayoutElement);
}

IOPrintLayoutElement::~IOPrintLayoutElement()
{
}

void IOPrintLayoutElement::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eReferences:
        {
            IOStringObjectCollection* IO = new IOStringObjectCollection(&m_printLayoutElement->GetReferences(), m_version, sReferences, sResourceId);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;
    
    case eExtents:
        {
            IOExtent3D* IO = new IOExtent3D(&m_printLayoutElement->GetExtents(), m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;
    
    case eStylization:
        {
            IOStylizationConfiguration* IO = new IOStylizationConfiguration(&m_printLayoutElement->GetStylizationConfiguration(), m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;
    
    case eData:
        {
            IODataConfiguration* IO = new IODataConfiguration(&m_printLayoutElement->GetDataConfiguration(), m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eExtendedData1:
        m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}

void IOPrintLayoutElement::ElementChars(const wchar_t* ch)
{
    switch (m_currElemId)
    {
    case eDescription:
        m_printLayoutElement->SetDescription(ch);
        break;

    case eUnits:
        m_printLayoutElement->SetUnits(ch);
        break;

    case eDefinition:
        m_printLayoutElement->SetDefinition(ch);
        break;
    }
}

void IOPrintLayoutElement::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        m_printLayoutElement->SetUnknownXml(m_unknownXml);

        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        m_procExtData = false;
    }
}

void IOPrintLayoutElement::Write(MdfStream& fd, PrintLayoutElement* printLayoutElement, Version& version)
{
    _ASSERT(NULL != printLayoutElement);

    // Property: References
    IOStringObjectCollection::Write(fd, &printLayoutElement->GetReferences(), version, sReferences, sResourceId);
    
    // Property: Description
    fd << tab() << startStr(sDescription);
    fd << EncodeString(printLayoutElement->GetDescription());
    fd << endStr(sDescription) << std::endl;

    // Property: Units
    fd << tab() << startStr(sUnits);
    fd << EncodeString(printLayoutElement->GetUnits());
    fd << endStr(sUnits) << std::endl;

    // Property: Definition
    fd << tab() << startStr(sDefinition);
    fd << EncodeString(printLayoutElement->GetDefinition());
    fd << endStr(sDefinition) << std::endl;

    // Property: Extents
    IOExtent3D::Write(fd, &printLayoutElement->GetExtents(), version, sExtents);

    // Property: Stylization
    IOStylizationConfiguration::Write(fd, &printLayoutElement->GetStylizationConfiguration(), version, sStylization);

    // Property: Data
    IODataConfiguration::Write(fd, &printLayoutElement->GetDataConfiguration(), version, sData);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, printLayoutElement->GetUnknownXml(), &version);
}
