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
#include "IOW2DSymbol.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

// When a W2DSymbol is successfully parsed by this class, it must be accessed by the
// parent class and then managed appropriately.  It will not be deleted by this class.

IOW2DSymbol::IOW2DSymbol() : IOSymbol()
{
    this->m_ioResourceRef = NULL;
}

void IOW2DSymbol::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"W2D") // NOXLATE
    {
        this->m_startElemName = name;
        this->m_symbol = new W2DSymbol(L"", L"");
    }
    else if (this->m_currElemName == L"W2DSymbol") // NOXLATE
    {
        this->m_ioResourceRef = new IOResourceRef(name);
        handlerStack->push(this->m_ioResourceRef);
        this->m_ioResourceRef->StartElement(name, handlerStack);
    }
}

void IOW2DSymbol::ElementChars(const wchar_t *ch)
{
    W2DSymbol* symbol = static_cast<W2DSymbol*>(this->m_symbol);
    if (this->m_currElemName == L"FillColor") // NOXLATE
        symbol->SetAreaColor(ch);
    else if (this->m_currElemName == L"LineColor") // NOXLATE
        symbol->SetLineColor(ch);
    else if (this->m_currElemName == L"TextColor") // NOXLATE
        symbol->SetTextColor(ch);
    else
        IOSymbol::ElementChars(ch);
}

void IOW2DSymbol::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (this->m_startElemName == name)
    {
        // copy the values found by the IOResourceRef into our symbol
        W2DSymbol* symbol = static_cast<W2DSymbol*>(this->m_symbol);
        if (this->m_ioResourceRef)
        {
            symbol->SetSymbolLibrary(this->m_ioResourceRef->GetResourceId());
            symbol->SetSymbolName(this->m_ioResourceRef->GetItemName());
        }

        // delete this here - a new one is created in each call to StartElement
        delete this->m_ioResourceRef;
        handlerStack->pop();
        this->m_startElemName = L"";
    }
}

void IOW2DSymbol::Write(MdfStream &fd, W2DSymbol *symbol)
{
    fd << tab() << "<W2D>" << std::endl; // NOXLATE
    inctab();

    IOSymbol::Write(fd, symbol);

    //Property: W2DSymbol
    IOResourceRef::Write(fd, "W2DSymbol", symbol->GetSymbolLibrary(), symbol->GetSymbolName(), true);

    //Property: FillColor
    if (symbol->GetAreaColor().length() > 0)
    {
        fd << tab() << "<FillColor>"; // NOXLATE
        fd << EncodeString(symbol->GetAreaColor());
        fd << "</FillColor>" << std::endl; // NOXLATE
    }

    //Property: LineColor
    if (symbol->GetLineColor().length() > 0)
    {
        fd << tab() << "<LineColor>"; // NOXLATE
        fd << EncodeString(symbol->GetLineColor());
        fd << "</LineColor>" << std::endl; // NOXLATE
    }

    //Property: TextColor
    if (symbol->GetTextColor().length() > 0)
    {
        fd << tab() << "<TextColor>"; // NOXLATE
        fd << EncodeString(symbol->GetTextColor());
        fd << "</TextColor>" << std::endl; // NOXLATE
    }

    dectab();
    fd << tab() << "</W2D>" << std::endl; // NOXLATE
}
