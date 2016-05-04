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
#include "IOW2DSymbol.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

// When a W2DSymbol is successfully parsed by this class, it must be accessed by the
// parent class and then managed appropriately.  It will not be deleted by this class.

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, W2D);
// Inherited Symbol Elements
ELEM_MAP_ENTRY(2, Unit);
ELEM_MAP_ENTRY(3, SizeContext);
ELEM_MAP_ENTRY(4, SizeX);
ELEM_MAP_ENTRY(5, SizeY);
ELEM_MAP_ENTRY(6, Rotation);
ELEM_MAP_ENTRY(7, MaintainAspect);
ELEM_MAP_ENTRY(8, InsertionPointX);
ELEM_MAP_ENTRY(9, InsertionPointY);
// Local Elements
ELEM_MAP_ENTRY(10, W2DSymbol);
ELEM_MAP_ENTRY(11, FillColor);
ELEM_MAP_ENTRY(12, LineColor);
ELEM_MAP_ENTRY(13, TextColor);
ELEM_MAP_ENTRY(14, ExtendedData1);


IOW2DSymbol::IOW2DSymbol(Version& version) : IOSymbol(version)
{
    this->m_ioResourceRef = NULL;
}


void IOW2DSymbol::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eW2D:
        this->m_startElemName = name;
        this->m_symbol = new W2DSymbol(L"", L"");
        break;

    case eW2DSymbol:
        {
            this->m_ioResourceRef = new IOResourceRef(name, this->m_version);
            handlerStack->push(this->m_ioResourceRef);
            this->m_ioResourceRef->StartElement(name, handlerStack);
        }
        break;

    case eExtendedData1:
        this->m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IOW2DSymbol::ElementChars(const wchar_t* ch)
{
    W2DSymbol* symbol = static_cast<W2DSymbol*>(this->m_symbol);

    switch (this->m_currElemId)
    {
    case eFillColor:
        symbol->SetFillColor(ch);
        break;

    case eLineColor:
        symbol->SetLineColor(ch);
        break;

    case eTextColor:
        symbol->SetTextColor(ch);
        break;

    default:
        IOSymbol::ElementChars(ch);
        break;
    }
}


void IOW2DSymbol::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_symbol->SetUnknownXml(this->m_unknownXml);

        // copy the values found by the IOResourceRef into our symbol
        W2DSymbol* symbol = static_cast<W2DSymbol*>(this->m_symbol);
        if (this->m_ioResourceRef)
        {
            symbol->SetSymbolLibrary(this->m_ioResourceRef->GetResourceId());
            symbol->SetSymbolName(this->m_ioResourceRef->GetItemName());

            // delete this here - a new one is created in each call to StartElement
            delete this->m_ioResourceRef;
            this->m_ioResourceRef = NULL;
        }

        this->m_startElemName = L"";
        handlerStack->pop();
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOW2DSymbol::Write(MdfStream& fd, W2DSymbol* symbol, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sW2D) << std::endl;
    tab.inctab();

    IOSymbol::Write(fd, symbol, version, tab);

    // Property: W2DSymbol
    IOResourceRef::Write(fd, sW2DSymbol, symbol->GetSymbolLibrary(), symbol->GetSymbolName(), true, version, tab);

    // Property: FillColor
    if (!symbol->GetFillColor().empty())
    {
        fd << tab.tab() << startStr(sFillColor);
        fd << EncodeString(symbol->GetFillColor());
        fd << endStr(sFillColor) << std::endl;
    }

    // Property: LineColor
    if (!symbol->GetLineColor().empty())
    {
        fd << tab.tab() << startStr(sLineColor);
        fd << EncodeString(symbol->GetLineColor());
        fd << endStr(sLineColor) << std::endl;
    }

    // Property: TextColor
    if (!symbol->GetTextColor().empty())
    {
        fd << tab.tab() << startStr(sTextColor);
        fd << EncodeString(symbol->GetTextColor());
        fd << endStr(sTextColor) << std::endl;
    }

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, symbol->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sW2D) << std::endl;
}
