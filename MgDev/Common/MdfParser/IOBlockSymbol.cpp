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
#include "IOBlockSymbol.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

// When a BlockSymbol is successfully parsed by this class, it must be accessed by the
// parent class and then managed appropriately.  It will not be deleted by this class.

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, Block);
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
ELEM_MAP_ENTRY(10, DrawingName);
ELEM_MAP_ENTRY(11, BlockName);
ELEM_MAP_ENTRY(12, BlockColor);
ELEM_MAP_ENTRY(13, LayerColor);
ELEM_MAP_ENTRY(14, ExtendedData1);


IOBlockSymbol::IOBlockSymbol(Version& version) : IOSymbol(version)
{
}


void IOBlockSymbol::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eBlock:
        this->m_startElemName = name;
        this->m_symbol = new BlockSymbol();
        break;

    case eExtendedData1:
        this->m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IOBlockSymbol::ElementChars(const wchar_t* ch)
{
    BlockSymbol* symbol = static_cast<BlockSymbol*>(this->m_symbol);

    switch (this->m_currElemId)
    {
    case eDrawingName:
        symbol->SetDrawingName(ch);
        break;

    case eBlockName:
        symbol->SetBlockName(ch);
        break;

    case eBlockColor:
        symbol->SetBlockColor(ch);
        break;

    case eLayerColor:
        symbol->SetLayerColor(ch);
        break;

    default:
        IOSymbol::ElementChars(ch);
        break;
    }
}


void IOBlockSymbol::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_symbol->SetUnknownXml(this->m_unknownXml);

        this->m_startElemName = L"";
        handlerStack->pop();
    }
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOBlockSymbol::Write(MdfStream& fd, BlockSymbol* symbol, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sBlock) << std::endl;
    tab.inctab();

    IOSymbol::Write(fd, symbol, version, tab);

    // Property: SymbolLibrary
    fd << tab.tab() << startStr(sDrawingName);
    fd << EncodeString(symbol->GetDrawingName());
    fd << endStr(sDrawingName) << std::endl;

    // Property: SymbolName
    fd << tab.tab() << startStr(sBlockName);
    fd << EncodeString(symbol->GetBlockName());
    fd << endStr(sBlockName) << std::endl;

    // Property: BlockColor
    if (!symbol->GetBlockColor().empty())
    {
        fd << tab.tab() << startStr(sBlockColor);
        fd << EncodeString(symbol->GetBlockColor());
        fd << endStr(sBlockColor) << std::endl;
    }

    // Property: LayerColor
    if (!symbol->GetLayerColor().empty())
    {
        fd << tab.tab() << startStr(sLayerColor);
        fd << EncodeString(symbol->GetLayerColor());
        fd << endStr(sLayerColor) << std::endl;
    }

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, symbol->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sBlock) << std::endl;
}
