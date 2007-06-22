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
#include "IOImageSymbol.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

// When a ImageSymbol is successfully parsed by this class, it must be accessed by the
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
ELEM_MAP_ENTRY(9, Image);
ELEM_MAP_ENTRY(10, Content);


IOImageSymbol::IOImageSymbol() : IOSymbol()
{
    this->m_ioResourceRef = NULL;
}


void IOImageSymbol::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    // it's a pain to have the "Image" element contain an "Image" element
    if (this->m_currElemName == L"Image" && this->m_startElemName != L"Image") // NOXLATE
    {
        this->m_startElemName = name;
        this->m_symbol = new ImageSymbol();
    }
    else if (this->m_currElemName == L"Image") // NOXLATE
    {
        this->m_ioResourceRef = new IOResourceRef(name);
        handlerStack->push(this->m_ioResourceRef);
        this->m_ioResourceRef->StartElement(name, handlerStack);
    }
    else if (eUnknown == this->m_currElemId)
    {
        ParseUnknownXml(name, handlerStack);
    }
}


void IOImageSymbol::ElementChars(const wchar_t* ch)
{
    ImageSymbol* symbol = static_cast<ImageSymbol*>(this->m_symbol);
    if (this->m_currElemName == L"Content") // NOXLATE
        symbol->SetContent(ch);
    else
        IOSymbol::ElementChars(ch);
}


void IOImageSymbol::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_symbol->SetUnknownXml(this->m_unknownXml);

        // copy the values found by the IOResourceRef into our symbol
        ImageSymbol* symbol = static_cast<ImageSymbol*>(this->m_symbol);
        if (this->m_ioResourceRef != NULL)
        {
            symbol->SetImageLibrary(this->m_ioResourceRef->GetResourceId());
            symbol->SetImageName(this->m_ioResourceRef->GetItemName());

            // delete this here - a new one is created in each call to StartElement
            delete this->m_ioResourceRef;
            this->m_ioResourceRef = NULL;
        }

        this->m_startElemName = L"";
        handlerStack->pop();
    }
}


void IOImageSymbol::Write(MdfStream& fd, ImageSymbol* symbol, Version* version)
{
    fd << tab() << "<Image>" << std::endl; // NOXLATE
    inctab();

    IOSymbol::Write(fd, symbol, version);

    if (symbol->GetImageLibrary().length() > 0)
    {
        //Property: Image
        IOResourceRef::Write(fd, "Image", symbol->GetImageLibrary(), symbol->GetImageName(), false, version);
    }
    else
    {
        //Property: Content
        fd << tab() << "<Content>"; // NOXLATE
        fd << EncodeString(symbol->GetContent());
        fd << "</Content>" << std::endl; // NOXLATE
    }

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, symbol->GetUnknownXml(), version);

    dectab();
    fd << tab() << "</Image>" << std::endl; // NOXLATE
}
