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
#include "IOFontSymbol.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

// When a FontSymbol is successfully parsed by this class, it must be accessed by the
// parent class and then managed appropriately.  It will not be deleted by this class.

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, Font);
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
ELEM_MAP_ENTRY(10, FontName);
ELEM_MAP_ENTRY(11, Character);
ELEM_MAP_ENTRY(12, Bold);
ELEM_MAP_ENTRY(13, Italic);
ELEM_MAP_ENTRY(14, Underlined);
ELEM_MAP_ENTRY(15, ForegroundColor);
ELEM_MAP_ENTRY(16, ExtendedData1);


IOFontSymbol::IOFontSymbol(Version& version) : IOSymbol(version)
{
}


void IOFontSymbol::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eFont:
        this->m_startElemName = name;
        this->m_symbol = new FontSymbol();
        break;

    case eExtendedData1:
        this->m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IOFontSymbol::ElementChars(const wchar_t* ch)
{
    FontSymbol* symbol = static_cast<FontSymbol*>(this->m_symbol);

    switch (this->m_currElemId)
    {
    case eFontName:
        symbol->SetFontName(ch);
        break;

    case eCharacter:
        symbol->SetCharacter(ch[0]);
        break;

    case eBold:
        symbol->SetBold(ch);
        break;

    case eItalic:
        symbol->SetItalic(ch);
        break;

    case eUnderlined:
        symbol->SetUnderlined(ch);
        break;

    case eForegroundColor:
        symbol->SetForegroundColor(ch);
        break;

    default:
        IOSymbol::ElementChars(ch);
        break;
    }
}


void IOFontSymbol::EndElement(const wchar_t* name, HandlerStack* handlerStack)
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


void IOFontSymbol::Write(MdfStream& fd, FontSymbol* symbol, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sFont) << std::endl;
    tab.inctab();

    IOSymbol::Write(fd, symbol, version, tab);

    // Property: FontName
    fd << tab.tab() << startStr(sFontName);
    fd << EncodeString(symbol->GetFontName());
    fd << endStr(sFontName) << std::endl;

    // Property: Character
    fd << tab.tab() << startStr(sCharacter);
    wchar_t c[2];
    c[0] = symbol->GetCharacter();
    c[1] = 0;
    fd << EncodeString(c);
    fd << endStr(sCharacter) << std::endl;

    // Property: Bold
    if (wstrToBool(symbol->GetBold().c_str()))
    {
        fd << tab.tab() << startStr(sBold);
        fd << EncodeString(symbol->GetBold());
        fd << endStr(sBold) << std::endl;
    }

    // Property: Italic
    if (wstrToBool(symbol->GetItalic().c_str()))
    {
        fd << tab.tab() << startStr(sItalic);
        fd << EncodeString(symbol->GetItalic());
        fd << endStr(sItalic) << std::endl;
    }

    // Property: Underlined
    if (wstrToBool(symbol->GetUnderlined().c_str()))
    {
        fd << tab.tab() << startStr(sUnderlined);
        fd << EncodeString(symbol->GetUnderlined());
        fd << endStr(sUnderlined) << std::endl;
    }

    // Property: ForegroundColor
    fd << tab.tab() << startStr(sForegroundColor);
    fd << EncodeString(symbol->GetForegroundColor());
    fd << endStr(sForegroundColor) << std::endl;

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, symbol->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sFont) << std::endl;
}
