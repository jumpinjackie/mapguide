//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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
#include "IOWatermarkXOffset.h"
#include "IOWatermarkOffsetUnit.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, XPosition);
ELEM_MAP_ENTRY(2, HorizontalPosition);
ELEM_MAP_ENTRY(3, Offset);
ELEM_MAP_ENTRY(4, Unit);
ELEM_MAP_ENTRY(5, Alignment);

IOWatermarkXOffset::IOWatermarkXOffset(Version& version)
: SAX2ElementHandler(version), m_XOffset(NULL)
{
}


IOWatermarkXOffset::IOWatermarkXOffset(WatermarkXOffset* xOffset, Version& version)
: SAX2ElementHandler(version), m_XOffset(xOffset)
{
}


IOWatermarkXOffset::~IOWatermarkXOffset()
{
    delete this->m_XOffset;
}


void IOWatermarkXOffset::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eXPosition:
        this->m_startElemName = name;
        break;
    case eHorizontalPosition:
        this->m_startElemName = name;
        break;
    }
}


void IOWatermarkXOffset::ElementChars(const wchar_t* ch)
{
    switch (this->m_currElemId)
    {
    case eOffset:
        this->m_XOffset->SetLength(wstrToDouble(ch));
        break;

    case eAlignment:
        if (::wcscmp(ch, L"Left") == 0) // NOXLATE
            this->m_XOffset->SetAlignment(WatermarkXOffset::Left);
        else if (::wcscmp(ch, L"Right") == 0) // NOXLATE
            this->m_XOffset->SetAlignment(WatermarkXOffset::Right);
        else
            this->m_XOffset->SetAlignment(WatermarkXOffset::Center);  //Treat as "Center" if string is incorrect
        break;
    
    case eUnit:
        this->m_XOffset->SetUnit(IOWatermarkOffsetUnit::ParseWatermarkOffsetUnit(ch));
        break;
    }
}


void IOWatermarkXOffset::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_XOffset = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOWatermarkXOffset::Write(MdfStream& fd, WatermarkXOffset* xOffset, Version* version, const std::string& name)
{
    fd << tab() << startStr(name) << std::endl;
    inctab();

    fd << tab() << startStr(sOffset);
    fd << DoubleToStr(xOffset->GetLength());
    fd << endStr(sOffset) << std::endl;

    IOWatermarkOffsetUnit::Write(fd, xOffset->GetUnit());

    fd << tab() << startStr(sAlignment);
    WatermarkXOffset::HorizontalAlignment alignment = xOffset->GetAlignment();
    if(alignment == WatermarkXOffset::Left)
        fd << "Left"; // NOXLATE
    else if(alignment == WatermarkXOffset::Right)
        fd << "Right"; // NOXLATE
    else    //Treat "Center" as default value
        fd << "Center";      // NOXLATE
    fd << endStr(sAlignment) << std::endl;

    dectab();
    fd << tab() << endStr(name) << std::endl;
}
