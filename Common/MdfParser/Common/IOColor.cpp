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
#include "IOColor.h"

CREATE_ELEMENT_MAP;
// Start Elements
ELEM_MAP_ENTRY(1, BackgroundColor);
// Local Elements
ELEM_MAP_ENTRY(2, Red);
ELEM_MAP_ENTRY(3, Green);
ELEM_MAP_ENTRY(4, Blue);
ELEM_MAP_ENTRY(5, Alpha);

IOColor::IOColor(Color* color, Version& version) :
    SAX2ElementHandler(version),
    m_color(color)
{
    // The parser will update all the data of the object pointed by the following assigned pointer.
    _ASSERT(NULL != m_color);
}

IOColor::~IOColor()
{
}

void IOColor::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eBackgroundColor:
        m_startElemName = name;
        break;
    }
}

void IOColor::ElementChars(const wchar_t* ch)
{
    switch (m_currElemId)
    {
    case eRed:
        m_color->SetRed(wstrToInt(ch));
        break;

    case eGreen:
        m_color->SetGreen(wstrToInt(ch));
        break;

    case eBlue:
        m_color->SetBlue(wstrToInt(ch));
        break;

    case eAlpha:
        m_color->SetAlpha(wstrToInt(ch));
        break;
    }
}

void IOColor::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}

void IOColor::Write(MdfStream& fd, Color* color, Version* version, const std::string& name, MgTab& tab)
{
    _ASSERT(NULL != color);

    fd << tab.tab() << startStr(name) << std::endl;
    tab.inctab();

    // Property: Red
    fd << tab.tab() << startStr(sRed);
    fd << IntToStr(color->GetRed());
    fd << endStr(sRed) << std::endl;

    // Property: Green
    fd << tab.tab() << startStr(sGreen);
    fd << IntToStr(color->GetGreen());
    fd << endStr(sGreen) << std::endl;

    // Property: Blue
    fd << tab.tab() << startStr(sBlue);
    fd << IntToStr(color->GetBlue());
    fd << endStr(sBlue) << std::endl;

    // Property: Alpha
    fd << tab.tab() << startStr(sAlpha);
    fd << IntToStr(color->GetAlpha());
    fd << endStr(sAlpha) << std::endl;

    tab.dectab();
    fd << tab.tab() << endStr(name) << std::endl;
}
