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
#include "IOMarkSymbol.h"
#include "IOFill.h"
#include "IOStroke.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

// When a MarkSymbol is successfully parsed by this class, it must be accessed by the
// parent class and then managed appropriately.  It will not be deleted by this class.

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, Mark);
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
ELEM_MAP_ENTRY(10, Shape);
ELEM_MAP_ENTRY(11, Fill);
ELEM_MAP_ENTRY(12, Edge);
ELEM_MAP_ENTRY(13, ExtendedData1);


IOMarkSymbol::IOMarkSymbol(Version& version) : IOSymbol(version)
{
}


void IOMarkSymbol::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
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
            IOFill* IO = new IOFill(symbol->GetFill(), this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
        }
        break;

    case eEdge:
        {
            MarkSymbol* symbol = static_cast<MarkSymbol*>(this->m_symbol);
            symbol->AdoptEdge(new Stroke());
            IOStroke* IO = new IOStroke(symbol->GetEdge(), this->m_currElemName, this->m_version);
            handlerStack->push(IO);
            IO->StartElement(name, handlerStack);
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


void IOMarkSymbol::ElementChars(const wchar_t* ch)
{
    MarkSymbol* symbol = static_cast<MarkSymbol*>(this->m_symbol);

    switch (this->m_currElemId)
    {
    case eShape:
        if (wcscmp(ch, L"Square") == 0) // NOXLATE
            symbol->SetShape(MarkSymbol::Square);
        else if (wcscmp(ch, L"Circle") == 0) // NOXLATE
            symbol->SetShape(MarkSymbol::Circle);
        else if (wcscmp(ch, L"Triangle") == 0) // NOXLATE
            symbol->SetShape(MarkSymbol::Triangle);
        else if (wcscmp(ch, L"Star") == 0) // NOXLATE
            symbol->SetShape(MarkSymbol::Star);
        else if (wcscmp(ch, L"Cross") == 0) // NOXLATE
            symbol->SetShape(MarkSymbol::Cross);
        else if (wcscmp(ch, L"X") == 0) // NOXLATE
            symbol->SetShape(MarkSymbol::X);
        break;

    default:
        IOSymbol::ElementChars(ch);
        break;
    }
}


void IOMarkSymbol::EndElement(const wchar_t* name, HandlerStack* handlerStack)
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


void IOMarkSymbol::Write(MdfStream& fd, MarkSymbol* markSymbol, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sMark) << std::endl;
    tab.inctab();

    IOSymbol::Write(fd, markSymbol, version, tab);

    // Property: Shape
    fd << tab.tab() << startStr(sShape);
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
    fd << endStr(sShape) << std::endl;

    // Property: Fill
    if (markSymbol->GetFill())
        IOFill::Write(fd, markSymbol->GetFill(), version, tab);

    // Property: Edge
    if (markSymbol->GetEdge())
        IOStroke::Write(fd, markSymbol->GetEdge(), sEdge, version, tab);

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, markSymbol->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sMark) << std::endl;
}
