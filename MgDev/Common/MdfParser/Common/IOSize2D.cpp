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
#include "IOSize2D.h"

CREATE_ELEMENT_MAP;
// Start Elements
ELEM_MAP_ENTRY(1, PaperSize);
// Local Elements
ELEM_MAP_ENTRY(2, Width);
ELEM_MAP_ENTRY(3, Height);

IOSize2D::IOSize2D(Size2D* size, Version& version) :
    SAX2ElementHandler(version),
    m_size(size)
{
    // The parser will update all the data of the object pointed by the following assigned pointer.
    _ASSERT(NULL != m_size);
}

IOSize2D::~IOSize2D()
{
}

void IOSize2D::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case ePaperSize:
        m_startElemName = name;
        break;
    }
}

void IOSize2D::ElementChars(const wchar_t* ch)
{
    switch (m_currElemId)
    {
    case eWidth:
        m_size->SetWidth(wstrToDouble(ch));
        break;

    case eHeight:
        m_size->SetHeight(wstrToDouble(ch));
        break;
    }
}

void IOSize2D::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}

void IOSize2D::Write(MdfStream& fd, Size2D* size, Version* version, const std::string& name, MgTab& tab)
{
    _ASSERT(NULL != size);

    fd << tab.tab() << startStr(name) << std::endl;
    tab.inctab();

    // Property: Width
    fd << tab.tab() << startStr(sWidth);
    fd << DoubleToStr(size->GetWidth());
    fd << endStr(sWidth) << std::endl;

    // Property: Height
    fd << tab.tab() << startStr(sHeight);
    fd << DoubleToStr(size->GetHeight());
    fd << endStr(sHeight) << std::endl;

    tab.dectab();
    fd << tab.tab() << endStr(name) << std::endl;
}
