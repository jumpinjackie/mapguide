//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

// When a BlockSymbol is successfully parsed by this class, it must be accessed by the
// parent class and then managed appropriately.  It will not be deleted by this class.

CREATE_ELEMENT_MAP;
// Inherited Symbol Elements
ELEM_MAP_ENTRY(1, Unit);
ELEM_MAP_ENTRY(2, SizeContext);
ELEM_MAP_ENTRY(3, SizeX);
ELEM_MAP_ENTRY(4, SizeY);
ELEM_MAP_ENTRY(5, InsertionPointX);
ELEM_MAP_ENTRY(6, InsertionPointY);
ELEM_MAP_ENTRY(7, Rotation);
ELEM_MAP_ENTRY(8, MaintainAspect);
// Local Elements
ELEM_MAP_ENTRY(9, Block);
ELEM_MAP_ENTRY(10, DrawingName);
ELEM_MAP_ENTRY(11, BlockName);
ELEM_MAP_ENTRY(12, BlockColor);
ELEM_MAP_ENTRY(13, LayerColor);

void IOBlockSymbol::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    this->m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eBlock:
        this->m_startElemName = name;
        this->m_symbol = new BlockSymbol();
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;

    default:
        break;
    }
}

void IOBlockSymbol::ElementChars(const wchar_t *ch)
{
    BlockSymbol* symbol = static_cast<BlockSymbol*>(this->m_symbol);
    if (this->m_currElemName == L"DrawingName") // NOXLATE
        symbol->SetDrawingName(ch);
    else if (this->m_currElemName == L"BlockName") // NOXLATE
        symbol->SetBlockName(ch);
    else if (this->m_currElemName == L"BlockColor") // NOXLATE
        symbol->SetBlockColor(ch);
    else if (this->m_currElemName == L"LayerColor") // NOXLATE
        symbol->SetLayerColor(ch);
    else
        IOSymbol::ElementChars(ch);
}

void IOBlockSymbol::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        if (!UnknownXml().empty())
            this->m_symbol->SetUnknownXml(UnknownXml());

        handlerStack->pop();
        this->m_startElemName = L"";
    }
}

void IOBlockSymbol::Write(MdfStream &fd, BlockSymbol *symbol)
{
    fd << tab() << "<Block>" << std::endl; // NOXLATE
    inctab();

    IOSymbol::Write(fd, symbol);

    //Property: SymbolLibrary
    fd << tab() << "<DrawingName>"; // NOXLATE
    fd << EncodeString(symbol->GetDrawingName());
    fd << "</DrawingName>" << std::endl; // NOXLATE

    //Property: SymbolName
    fd << tab() << "<BlockName>"; // NOXLATE
    fd << EncodeString(symbol->GetBlockName());
    fd << "</BlockName>" << std::endl; // NOXLATE

    //Property: BlockColor
    if (symbol->GetBlockColor().length() > 0)
    {
        fd << tab() << "<BlockColor>"; // NOXLATE
        fd << EncodeString(symbol->GetBlockColor());
        fd << "</BlockColor>" << std::endl; // NOXLATE
    }

    //Property: LayerColor
    if (symbol->GetLayerColor().length() > 0)
    {
        fd << tab() << "<LayerColor>"; // NOXLATE
        fd << EncodeString(symbol->GetLayerColor());
        fd << "</LayerColor>" << std::endl; // NOXLATE
    }

    // Write any previously found unknown XML
    if (!symbol->GetUnknownXml().empty())
    {
        fd << toCString(symbol->GetUnknownXml()); 
    }

    dectab();
    fd << tab() << "</Block>" << std::endl; // NOXLATE
}
