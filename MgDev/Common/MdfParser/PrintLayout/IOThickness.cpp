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
#include "IOThickness.h"

CREATE_ELEMENT_MAP;
// Start Elements
ELEM_MAP_ENTRY(1, PaperMargin);
// Local Elements
ELEM_MAP_ENTRY(2, Left);
ELEM_MAP_ENTRY(3, Top);
ELEM_MAP_ENTRY(4, Right);
ELEM_MAP_ENTRY(5, Bottom);

IOThickness::IOThickness(Thickness* thickness, Version& version) :
    SAX2ElementHandler(version),
    m_thickness(thickness)
{
    // The parser will update all the data of the object pointed by the following assigned pointer.
    _ASSERT(NULL != m_thickness);
}

IOThickness::~IOThickness()
{
}

void IOThickness::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case ePaperMargin:
        m_startElemName = name;
        break;
    }
}

void IOThickness::ElementChars(const wchar_t* ch)
{
    switch (m_currElemId)
    {
    case eLeft:
        m_thickness->SetLeft(wstrToDouble(ch));
        break;

    case eTop:
        m_thickness->SetTop(wstrToDouble(ch));
        break;

    case eRight:
        m_thickness->SetRight(wstrToDouble(ch));
        break;

    case eBottom:
        m_thickness->SetBottom(wstrToDouble(ch));
        break;
    }
}

void IOThickness::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}

void IOThickness::Write(MdfStream& fd, Thickness* thickness, Version* version, const std::string& name, MgTab& tab)
{
    _ASSERT(NULL != thickness);

    fd << tab.tab() << startStr(name) << std::endl;
    tab.inctab();

    // Property: Left
    fd << tab.tab() << startStr(sLeft);
    fd << DoubleToStr(thickness->GetLeft());
    fd << endStr(sLeft) << std::endl;

    // Property: Top
    fd << tab.tab() << startStr(sTop);
    fd << DoubleToStr(thickness->GetTop());
    fd << endStr(sTop) << std::endl;

    // Property: Right
    fd << tab.tab() << startStr(sRight);
    fd << DoubleToStr(thickness->GetRight());
    fd << endStr(sRight) << std::endl;

    // Property: Bottom
    fd << tab.tab() << startStr(sBottom);
    fd << DoubleToStr(thickness->GetBottom());
    fd << endStr(sBottom) << std::endl;

    tab.dectab();
    fd << tab.tab() << endStr(name) << std::endl;
}
