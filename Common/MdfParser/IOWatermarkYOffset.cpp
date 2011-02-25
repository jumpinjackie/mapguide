//
//  Copyright (C) 2010-2011 by Autodesk, Inc.
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
#include "IOWatermarkYOffset.h"
#include "IOWatermarkOffsetUnit.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, YPosition);
ELEM_MAP_ENTRY(2, VerticalPosition);
ELEM_MAP_ENTRY(3, Offset);
ELEM_MAP_ENTRY(4, Unit);
ELEM_MAP_ENTRY(5, Alignment);


IOWatermarkYOffset::IOWatermarkYOffset(Version& version) : SAX2ElementHandler(version)
{
    this->m_YOffset = NULL;
}


IOWatermarkYOffset::IOWatermarkYOffset(WatermarkYOffset* yOffset, Version& version) : SAX2ElementHandler(version)
{
    this->m_YOffset = yOffset;
}


IOWatermarkYOffset::~IOWatermarkYOffset()
{
}


void IOWatermarkYOffset::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eYPosition:
        this->m_startElemName = name;
        break;

    case eVerticalPosition:
        this->m_startElemName = name;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IOWatermarkYOffset::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eOffset:
        this->m_YOffset->SetOffset(wstrToDouble(ch));
        break;

    case eUnit:
        this->m_YOffset->SetUnit(IOWatermarkOffsetUnit::ParseWatermarkOffsetUnit(ch));
        break;

    case eAlignment:
        if (::wcscmp(ch, L"Top") == 0) // NOXLATE
            this->m_YOffset->SetAlignment(WatermarkYOffset::Top);
        else if (::wcscmp(ch, L"Bottom") == 0) // NOXLATE
            this->m_YOffset->SetAlignment(WatermarkYOffset::Bottom);
        else
            this->m_YOffset->SetAlignment(WatermarkYOffset::Center); // treat as "Center" if string is incorrect
        break;
    }
}


void IOWatermarkYOffset::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_YOffset->SetUnknownXml(this->m_unknownXml);

        this->m_YOffset = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOWatermarkYOffset::Write(MdfStream& fd, WatermarkYOffset* yOffset, const std::string& name, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(name) << std::endl;
    tab.inctab();

    // Property: Offset
    fd << tab.tab() << startStr(sOffset);
    fd << DoubleToStr(yOffset->GetOffset());
    fd << endStr(sOffset) << std::endl;

    // Property: Unit
    IOWatermarkOffsetUnit::Write(fd, yOffset->GetUnit(), tab);

    // Property: Alignment
    fd << tab.tab() << startStr(sAlignment);
    WatermarkYOffset::VerticalAlignment alignment = yOffset->GetAlignment();
    if (alignment == WatermarkYOffset::Top)
        fd << "Top"; // NOXLATE
    else if (alignment == WatermarkYOffset::Bottom)
        fd << "Bottom"; // NOXLATE
    else // treat "Center" as default value
        fd << "Center"; // NOXLATE
    fd << endStr(sAlignment) << std::endl;

    tab.dectab();
    fd << tab.tab() << endStr(name) << std::endl;
}
