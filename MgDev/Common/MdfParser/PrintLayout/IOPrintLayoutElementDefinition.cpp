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
#include "IOPrintLayoutElementDefinition.h"
#include "IOUnknown.h"

CREATE_ELEMENT_MAP;
// Start Elements
ELEM_MAP_ENTRY(1, AnnotationDefinition);
ELEM_MAP_ENTRY(2, LegendDefinition);
ELEM_MAP_ENTRY(3, NorthArrowDefinition);
// Inherited Elements
ELEM_MAP_ENTRY(4, Name);
ELEM_MAP_ENTRY(5, ExtendedData1);

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
    case eAnnotationDefinition:
    case eLegendDefinition:
    case eNorthArrowDefinition:
        m_startElemName = name;
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

void IOPrintLayoutElementDefinition::Write(MdfStream& fd, PrintLayoutElementDefinition* layoutElemDef, Version& version, const std::string& name)
{
    _ASSERT(NULL != layoutElemDef);

    fd << tab() << startStr(name) << std::endl;
    inctab();

    // Property: Name
    fd << tab() << startStr(sName);
    fd << EncodeString(layoutElemDef->GetName());
    fd << endStr(sName) << std::endl;

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, layoutElemDef->GetUnknownXml(), &version);

    dectab();
    fd << tab() << endStr(name) << std::endl;
}
