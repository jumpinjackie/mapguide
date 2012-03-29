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
#include "IOImageSymbol.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

// When a ImageSymbol is successfully parsed by this class, it must be accessed by the
// parent class and then managed appropriately.  It will not be deleted by this class.

CREATE_ELEMENT_MAP;
ELEM_MAP_ENTRY(1, Image);
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
ELEM_MAP_ENTRY(10, Content);
ELEM_MAP_ENTRY(11, ExtendedData1);


IOImageSymbol::IOImageSymbol(Version& version) : IOSymbol(version)
{
    this->m_ioResourceRef = NULL;
}


void IOImageSymbol::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    this->m_currElemId = _ElementIdFromName(name);

    switch (this->m_currElemId)
    {
    case eImage:
        {
            // it's a pain to have the "Image" element contain an "Image" element
            if (this->m_startElemName.empty())
            {
                this->m_startElemName = name;
                this->m_symbol = new ImageSymbol();
            }
            else
            {
                this->m_ioResourceRef = new IOResourceRef(name, this->m_version);
                handlerStack->push(this->m_ioResourceRef);
                this->m_ioResourceRef->StartElement(name, handlerStack);
            }
        }

    case eExtendedData1:
        this->m_procExtData = true;
        break;

    case eUnknown:
        ParseUnknownXml(name, handlerStack);
        break;
    }
}


void IOImageSymbol::ElementChars(const wchar_t* ch)
{
    ImageSymbol* symbol = static_cast<ImageSymbol*>(this->m_symbol);

    switch (this->m_currElemId)
    {
    case eContent:
        symbol->SetContent(ch);
        break;

    default:
        IOSymbol::ElementChars(ch);
        break;
    }
}


void IOImageSymbol::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_symbol->SetUnknownXml(this->m_unknownXml);

        // copy the values found by the IOResourceRef into our symbol
        ImageSymbol* symbol = static_cast<ImageSymbol*>(this->m_symbol);
        if (this->m_ioResourceRef)
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
    else if (eExtendedData1 == _ElementIdFromName(name))
    {
        this->m_procExtData = false;
    }
}


void IOImageSymbol::Write(MdfStream& fd, ImageSymbol* symbol, Version* version, MgTab& tab)
{
    fd << tab.tab() << startStr(sImage) << std::endl;
    tab.inctab();

    IOSymbol::Write(fd, symbol, version, tab);

    if (!symbol->GetImageLibrary().empty())
    {
        // Property: Image
        IOResourceRef::Write(fd, sImage, symbol->GetImageLibrary(), symbol->GetImageName(), false, version, tab);
    }
    else
    {
        // Property: Content
        fd << tab.tab() << startStr(sContent);
        fd << EncodeString(symbol->GetContent());
        fd << endStr(sContent) << std::endl;
    }

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, symbol->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << endStr(sImage) << std::endl;
}
