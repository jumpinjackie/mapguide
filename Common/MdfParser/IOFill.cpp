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

#include "stdafx.h"
#include "IOFill.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, Fill);
ELEM_MAP_ENTRY(2, FillPattern);
ELEM_MAP_ENTRY(3, ForegroundColor);
ELEM_MAP_ENTRY(4, BackgroundColor);
ELEM_MAP_ENTRY(5, ExtendedData1);


IOFill::IOFill(Version& version) : SAX2ElementHandler(version)
{
    this->m_fill = NULL;
}


IOFill::IOFill(Fill* fill, Version& version) : SAX2ElementHandler(version)
{
    this->m_fill = fill;
}


IOFill::~IOFill()
{
}


void IOFill::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eFill:
        this->m_startElemName = name;
        break;

    case eExtendedData1:
        this->m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IOFill::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eFillPattern:
        this->m_fill->SetFillPattern(ch);
        break;

    case eForegroundColor:
        this->m_fill->SetForegroundColor(ch);
        break;

    case eBackgroundColor:
        this->m_fill->SetBackgroundColor(ch);
        break;
    }
}


void IOFill::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_fill->SetUnknownXml(this->m_unknownXml);

        this->m_fill = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOFill::Write(MdfStream& fd, Fill* fill, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sFill) << std::endl;
    tab.inctab();

    // Property: FillPattern
    fd << tab.tab() << startStr(sFillPattern);
    fd << EncodeString(fill->GetFillPattern());
    fd << endStr(sFillPattern) << std::endl;

    // Property: ForegroundColor
    fd << tab.tab() << startStr(sForegroundColor);
    fd << EncodeString(fill->GetForegroundColor());
    fd << endStr(sForegroundColor) << std::endl;

    // Property: BackgroundColor
    fd << tab.tab() << startStr(sBackgroundColor);
    fd << EncodeString(fill->GetBackgroundColor());
    fd << endStr(sBackgroundColor) << std::endl;

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, fill->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sFill) << std::endl;
}
