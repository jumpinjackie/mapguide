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
#include "IOMarkSymbol.h"
#include "IOFill.h"
#include "IOStroke.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

// When a MarkSymbol is successfully parsed by this class, it must be accessed by the
// parent class and then managed appropriately.  It will not be deleted by this class.

void IOMarkSymbol::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"Mark") // NOXLATE
    {
        this->m_startElemName = name;
        this->m_symbol = new MarkSymbol(MarkSymbol::Square);
    }
    else if (this->m_currElemName == L"Fill") // NOXLATE
    {
        MarkSymbol* symbol = static_cast<MarkSymbol*>(this->m_symbol);
        symbol->AdoptFill(new Fill());
        this->m_ioFill = new IOFill(symbol->GetFill());
        handlerStack->push(this->m_ioFill);
        this->m_ioFill->StartElement(name, handlerStack);
    }
    else if (this->m_currElemName == L"Edge") // NOXLATE
    {
        MarkSymbol* symbol = static_cast<MarkSymbol*>(this->m_symbol);
        symbol->AdoptEdge(new Stroke());
        this->m_ioStroke = new IOStroke(symbol->GetEdge(), this->m_currElemName);
        handlerStack->push(this->m_ioStroke);
        this->m_ioStroke->StartElement(name, handlerStack);
    }
}

void IOMarkSymbol::ElementChars(const wchar_t *ch)
{
    if (m_currElemName == L"Shape") // NOXLATE
    {
        MarkSymbol* symbol = static_cast<MarkSymbol*>(this->m_symbol);
        if (!wcscmp(ch, L"square") || !wcscmp(ch, L"Square")) // NOXLATE
            symbol->SetShape(MarkSymbol::Square);
        else if (!wcscmp(ch, L"circle") || !wcscmp(ch, L"Circle")) // NOXLATE
            symbol->SetShape(MarkSymbol::Circle);
        else if (!wcscmp(ch, L"triangle") || !wcscmp(ch, L"Triangle")) // NOXLATE
            symbol->SetShape(MarkSymbol::Triangle);
        else if (!wcscmp(ch, L"star") || !wcscmp(ch, L"Star")) // NOXLATE
            symbol->SetShape(MarkSymbol::Star);
        else if (!wcscmp(ch, L"cross") || !wcscmp(ch, L"Cross")) // NOXLATE
            symbol->SetShape(MarkSymbol::Cross);
        else if (!wcscmp(ch, L"x") || !wcscmp(ch, L"X")) // NOXLATE
            symbol->SetShape(MarkSymbol::X);
    }
    else
        IOSymbol::ElementChars(ch);
}

void IOMarkSymbol::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (this->m_startElemName == name)
    {
        handlerStack->pop();
        this->m_startElemName = L"";
    }
}

void IOMarkSymbol::Write(MdfStream &fd, MarkSymbol *symbol)
{
    fd << tab() << "<Mark>" << std::endl; // NOXLATE
    inctab();

    IOSymbol::Write(fd, symbol);

    //Property: Shape
    fd << tab() << "<Shape>"; // NOXLATE
    if (symbol->GetShape() == MarkSymbol::Square)
        fd << EncodeString(L"Square"); // NOXLATE
    else if (symbol->GetShape() == MarkSymbol::Circle)
        fd << EncodeString(L"Circle"); // NOXLATE
    else if (symbol->GetShape() == MarkSymbol::Triangle)
        fd << EncodeString(L"Triangle"); // NOXLATE
    else if (symbol->GetShape() == MarkSymbol::Star)
        fd << EncodeString(L"Star"); // NOXLATE
    else if (symbol->GetShape() == MarkSymbol::Cross)
        fd << EncodeString(L"Cross"); // NOXLATE
    else if (symbol->GetShape() == MarkSymbol::X)
        fd << EncodeString(L"X"); // NOXLATE
    fd << "</Shape>" << std::endl; // NOXLATE

    //Property: Fill
    if (symbol->GetFill() != NULL)
        IOFill::Write(fd, symbol->GetFill());

    //Property: Edge
    if (symbol->GetEdge() != NULL)
        IOStroke::Write(fd, symbol->GetEdge(), "Edge"); // NOXLATE

    dectab();
    fd << tab() << "</Mark>" << std::endl; // NOXLATE
}
