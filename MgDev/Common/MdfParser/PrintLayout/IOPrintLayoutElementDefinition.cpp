//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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
#include "IOPrintLayoutElementDefinition.h"
#include "IOStylizationConfiguration.h"
#include "IODataConfiguration.h"

CREATE_ELEMENT_MAP;
// Start Elements
ELEM_MAP_ENTRY(1, PrintLayoutElementDefinition);
// Inherited Elements
ELEM_MAP_ENTRY(2, Description);
ELEM_MAP_ENTRY(3, ResourceId);
ELEM_MAP_ENTRY(4, Stylization);
ELEM_MAP_ENTRY(5, Data);
ELEM_MAP_ENTRY(6, ExtendedData1);

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
    case ePrintLayoutElementDefinition:
        _ASSERT(false);
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
    case eDescription:
        m_layoutElemDef->SetDescription(ch);
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

void IOPrintLayoutElementDefinition::Write(MdfStream& fd, PrintLayoutElementDefinition* layoutElemDef, Version* version, MgTab& tab)
{
    _ASSERT(NULL != layoutElemDef);

    // Property: Description
    fd << tab.tab() << startStr(sDescription);
    fd << EncodeString(layoutElemDef->GetDescription());
    fd << endStr(sDescription) << std::endl;

    // Property: ResourceId
    fd << tab.tab() << startStr(sResourceId);
    fd << EncodeString(layoutElemDef->GetResourceId());
    fd << endStr(sResourceId) << std::endl;

    // Property: Stylization
    IOStylizationConfiguration::Write(fd, layoutElemDef->GetStylizationConfiguration(), version, tab);

    // Property: Data
    IODataConfiguration::Write(fd, layoutElemDef->GetDataConfiguration(), version, tab);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, layoutElemDef->GetUnknownXml(), version, tab);
}
