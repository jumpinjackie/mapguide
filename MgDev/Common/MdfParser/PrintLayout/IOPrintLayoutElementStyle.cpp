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
#include "IOPrintLayoutElementStyle.h"

CREATE_ELEMENT_MAP;
// Start Elements
ELEM_MAP_ENTRY(1, AnnotationStyle);
ELEM_MAP_ENTRY(2, LegendStyle);
ELEM_MAP_ENTRY(3, NorthArrowStyle);
// Inherited Elements
ELEM_MAP_ENTRY(4, Name);
ELEM_MAP_ENTRY(5, ExtendedData1);

IOPrintLayoutElementStyle::IOPrintLayoutElementStyle(PrintLayoutElementStyle* layoutElemStyle, Version& version) :
    SAX2ElementHandler(version),
    m_layoutElemStyle(layoutElemStyle)
{
    // The parser will update all the data of the object pointed by the following assigned pointer.
    _ASSERT(NULL != m_layoutElemStyle);
}

IOPrintLayoutElementStyle::~IOPrintLayoutElementStyle()
{
}

void IOPrintLayoutElementStyle::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eAnnotationStyle:
    case eLegendStyle:
    case eNorthArrowStyle:
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

void IOPrintLayoutElementStyle::ElementChars(const wchar_t* ch)
{
    switch (m_currElemId)
    {
    case eName:
        m_layoutElemStyle->SetName(ch);
        break;
    }
}

void IOPrintLayoutElementStyle::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        m_layoutElemStyle->SetUnknownXml(m_unknownXml);

        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        m_procExtData = false;
    }
}

void IOPrintLayoutElementStyle::Write(MdfStream& fd, PrintLayoutElementStyle* layoutElemStyle, Version* version, const std::string& name, MgTab& tab)
{
    _ASSERT(NULL != layoutElemStyle);

    fd << tab.tab() << startStr(name) << std::endl;
    tab.inctab();

    // Property: Name
    fd << tab.tab() << startStr(sName);
    fd << EncodeString(layoutElemStyle->GetName());
    fd << endStr(sName) << std::endl;

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, layoutElemStyle->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(name) << std::endl;
}
