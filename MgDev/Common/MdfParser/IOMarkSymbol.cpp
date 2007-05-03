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
#include "IOMarkSymbol.h"
#include "IOFill.h"
#include "IOStroke.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

// When a MarkSymbol is successfully parsed by this class, it must be accessed by the
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
ELEM_MAP_ENTRY(9, Mark);
ELEM_MAP_ENTRY(10, Fill);
ELEM_MAP_ENTRY(11, Edge);
ELEM_MAP_ENTRY(12, Shape);


IOMarkSymbol::IOMarkSymbol() : IOSymbol()
{
    this->m_ioStroke = NULL;
    this->m_ioFill = NULL;
}

void IOMarkSymbol::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    this->m_currElemName = name;
    m_currElemId = _ElementIdFromName(name);

    switch (m_currElemId)
    {
    case eMark:
        {
            this->m_startElemName = name;
            MarkSymbol* markSymbol = new MarkSymbol(MarkSymbol::Square);
            delete markSymbol->OrphanFill();
            delete markSymbol->OrphanEdge();
            this->m_symbol = markSymbol;
        }
        break;

    case eFill:
        {
            MarkSymbol* symbol = static_cast<MarkSymbol*>(this->m_symbol);
            symbol->AdoptFill(new Fill());
            this->m_ioFill = new IOFill(symbol->GetFill());
            handlerStack->push(this->m_ioFill);
            this->m_ioFill->StartElement(name, handlerStack);
        }
        break;

    case eEdge:
        {
            MarkSymbol* symbol = static_cast<MarkSymbol*>(this->m_symbol);
            symbol->AdoptEdge(new Stroke());
            this->m_ioStroke = new IOStroke(symbol->GetEdge(), this->m_currElemName);
            handlerStack->push(this->m_ioStroke);
            this->m_ioStroke->StartElement(name, handlerStack);
        }
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;

    default:
        break;
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
        if (!UnknownXml().empty())
            this->m_symbol->SetUnknownXml(UnknownXml());

        handlerStack->pop();
        this->m_startElemName = L"";
    }
}

void IOMarkSymbol::Write(MdfStream &fd, MarkSymbol *markSymbol, Version *version)
{
    fd << tab() << "<Mark>" << std::endl; // NOXLATE
    inctab();

    IOSymbol::Write(fd, markSymbol);

    //Property: Shape
    fd << tab() << "<Shape>"; // NOXLATE
    if (markSymbol->GetShape() == MarkSymbol::Square)
        fd << EncodeString(L"Square"); // NOXLATE
    else if (markSymbol->GetShape() == MarkSymbol::Circle)
        fd << EncodeString(L"Circle"); // NOXLATE
    else if (markSymbol->GetShape() == MarkSymbol::Triangle)
        fd << EncodeString(L"Triangle"); // NOXLATE
    else if (markSymbol->GetShape() == MarkSymbol::Star)
        fd << EncodeString(L"Star"); // NOXLATE
    else if (markSymbol->GetShape() == MarkSymbol::Cross)
        fd << EncodeString(L"Cross"); // NOXLATE
    else if (markSymbol->GetShape() == MarkSymbol::X)
        fd << EncodeString(L"X"); // NOXLATE
    fd << "</Shape>" << std::endl; // NOXLATE

    //Property: Fill
    if (markSymbol->GetFill() != NULL)
        IOFill::Write(fd, markSymbol->GetFill());

    //Property: Edge
    if (markSymbol->GetEdge() != NULL)
        IOStroke::Write(fd, markSymbol->GetEdge(), "Edge", version); // NOXLATE

    // Write any previously found unknown XML
    if (!markSymbol->GetUnknownXml().empty())
        fd << tab() << toCString(markSymbol->GetUnknownXml()) << std::endl;

    dectab();
    fd << tab() << "</Mark>" << std::endl; // NOXLATE
}
