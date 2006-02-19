//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

// When a FontSymbol is successfully parsed by this class, it must be accessed by the
// parent class and then managed appropriately.  It will not be deleted by this class.

void IOFontSymbol::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"Font") // NOXLATE
    {
        this->m_startElemName = name;
        this->m_symbol = new FontSymbol();
    }
}

void IOFontSymbol::ElementChars(const wchar_t *ch)
{
    FontSymbol* symbol = static_cast<FontSymbol*>(this->m_symbol);
    if (this->m_currElemName == L"FontName") // NOXLATE
        symbol->SetFontName(ch);
    else if (this->m_currElemName == L"Character") // NOXLATE
        symbol->SetCharacter(ch[0]);
    else if (this->m_currElemName == L"Bold") // NOXLATE
        symbol->SetBold(ch);
    else if (this->m_currElemName == L"Italic") // NOXLATE
        symbol->SetItalic(ch);
    else if (this->m_currElemName == L"Underlined") // NOXLATE
        symbol->SetUnderlined(ch);
    else if (this->m_currElemName == L"ForegroundColor") // NOXLATE
        symbol->SetForegroundColor(ch);
    else
        IOSymbol::ElementChars(ch);
}

void IOFontSymbol::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        handlerStack->pop();
        this->m_startElemName = L"";
    }
}

void IOFontSymbol::Write(MdfStream &fd, FontSymbol *symbol)
{
    fd << tab() << "<Font>" << std::endl; // NOXLATE
    inctab();

    IOSymbol::Write(fd, symbol);

    //Property: FontName
    fd << tab() << "<FontName>"; // NOXLATE
    fd << EncodeString(symbol->GetFontName());
    fd << "</FontName>" << std::endl; // NOXLATE

    //Property: Character
    fd << tab() << "<Character>"; // NOXLATE
    wchar_t c[2];
    c[0] = symbol->GetCharacter();
    c[1] = 0;
    fd << EncodeString(c);
    fd << "</Character>" << std::endl; // NOXLATE

    //Property: Bold
    if (wstrToBool(symbol->GetBold().c_str()))
    {
        fd << tab() << "<Bold>"; // NOXLATE
        fd << EncodeString(symbol->GetBold());
        fd << "</Bold>" << std::endl; // NOXLATE
    }

    //Property: Italic
    if (wstrToBool(symbol->GetItalic().c_str()))
    {
        fd << tab() << "<Italic>"; // NOXLATE
        fd << EncodeString(symbol->GetItalic());
        fd << "</Italic>" << std::endl; // NOXLATE
    }

    //Property: Underlined
    if (wstrToBool(symbol->GetUnderlined().c_str()))
    {
        fd << tab() << "<Underlined>"; // NOXLATE
        fd << EncodeString(symbol->GetUnderlined());
        fd << "</Underlined>" << std::endl; // NOXLATE
    }

    //Property: ForegroundColor
    fd << tab() << "<ForegroundColor>"; // NOXLATE
    fd << EncodeString(symbol->GetForegroundColor());
    fd << "</ForegroundColor>" << std::endl; // NOXLATE

    dectab();
    fd << tab() << "</Font>" << std::endl; // NOXLATE
}
