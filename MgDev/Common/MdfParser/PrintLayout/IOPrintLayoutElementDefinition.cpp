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

#include "../stdafx.h"
#include "../IOUnknown.h"
#include "../Common/IOStringObjectCollection.h"
#include "../Common/IOExtent3D.h"
#include "IOPrintLayoutElementDefinition.h"
#include "IOStylizationConfiguration.h"
#include "IODataConfiguration.h"

CREATE_ELEMENT_MAP;
// Inherited Elements
ELEM_MAP_ENTRY(1, Name);
ELEM_MAP_ENTRY(2, References);
ELEM_MAP_ENTRY(3, Description);
ELEM_MAP_ENTRY(4, Units);
ELEM_MAP_ENTRY(5, ResourceId);
ELEM_MAP_ENTRY(6, Extent);
ELEM_MAP_ENTRY(7, Stylization);
ELEM_MAP_ENTRY(8, Data);
ELEM_MAP_ENTRY(9, ExtendedData1);

IOPrintLayoutElementDefinition::IOPrintLayoutElementDefinition(PrintLayoutElementDefinition* layoutElemDef, Version& version) :
    SAX2ElementHandler(version),
    m_layoutElemDef(layoutElemDef)
{
    // The parser will update all the data of the object pointed by the following assigned pointer.
    _ASSERT(NULL != m_layoutElemDef);
}

IOPrintLayoutElementDefinition::~IOPrintLayoutElementDefinition()
{
}

void IOPrintLayoutElementDefinition::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eReferences:
        {
            IOStringObjectCollection* IO = new IOStringObjectCollection(m_layoutElemDef->GetReferences(), m_version, sReferences, sResourceId);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;
    
    case eExtent:
        {
            IOExtent3D* IO = new IOExtent3D(m_layoutElemDef->GetExtent(), m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;
    
    case eStylization:
        {
            IOStylizationConfiguration* IO = new IOStylizationConfiguration(m_layoutElemDef->GetStylizationConfiguration(), m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;
    
    case eData:
        {
            IODataConfiguration* IO = new IODataConfiguration(m_layoutElemDef->GetDataConfiguration(), m_version);
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

void IOPrintLayoutElementDefinition::ElementChars(const wchar_t* ch)
{
    switch (m_currElemId)
    {
    case eName:
        m_layoutElemDef->SetName(ch);
        break;

    case eDescription:
        m_layoutElemDef->SetDescription(ch);
        break;

    case eUnits:
        m_layoutElemDef->SetUnits(ch);
        break;

    case eResourceId:
        m_layoutElemDef->SetResourceId(ch);
        break;
    }
}

void IOPrintLayoutElementDefinition::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        m_layoutElemDef->SetUnknownXml(m_unknownXml);

        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        m_procExtData = false;
    }
}

void IOPrintLayoutElementDefinition::Write(MdfStream& fd, PrintLayoutElementDefinition* layoutElemDef, Version* version)
{
    _ASSERT(NULL != layoutElemDef);

    // Property: Name
    fd << tab() << startStr(sName);
    fd << EncodeString(layoutElemDef->GetName());
    fd << endStr(sName) << std::endl;

    // Property: References
    IOStringObjectCollection::Write(fd, layoutElemDef->GetReferences(), version, sReferences, sResourceId);
    
    // Property: Description
    fd << tab() << startStr(sDescription);
    fd << EncodeString(layoutElemDef->GetDescription());
    fd << endStr(sDescription) << std::endl;

    // Property: Units
    fd << tab() << startStr(sUnits);
    fd << EncodeString(layoutElemDef->GetUnits());
    fd << endStr(sUnits) << std::endl;

    // Property: ResourceId
    fd << tab() << startStr(sResourceId);
    fd << EncodeString(layoutElemDef->GetResourceId());
    fd << endStr(sResourceId) << std::endl;

    // Property: Extent
    IOExtent3D::Write(fd, layoutElemDef->GetExtent(), version);

    // Property: Stylization
    IOStylizationConfiguration::Write(fd, layoutElemDef->GetStylizationConfiguration(), version);

    // Property: Data
    IODataConfiguration::Write(fd, layoutElemDef->GetDataConfiguration(), version);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, layoutElemDef->GetUnknownXml(), version);
}
