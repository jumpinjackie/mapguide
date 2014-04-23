//
//  Copyright (C) 2007-2011 by Autodesk, Inc.
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
#include "IOGraphicElement.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOGraphicElement::IOGraphicElement(GraphicElement* element, Version& version) : SAX2ElementHandler(version)
{
    this->m_element = element;
}


IOGraphicElement::~IOGraphicElement()
{
}


void IOGraphicElement::ElementChars(const wchar_t* ch)
{
    IF_STRING_PROPERTY(this->m_currElemName, this->m_element, ResizeControl, ch)
}


void IOGraphicElement::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_element->SetUnknownXml(this->m_unknownXml);

        this->m_element = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (::wcscmp(name, L"ExtendedData1") == 0) // NOXLATE
    {
        this->m_procExtData = false;
    }
}


void IOGraphicElement::Write(MdfStream& fd, GraphicElement* element, Version* version, MgTab& tab)
{
    EMIT_STRING_PROPERTY(fd, element, ResizeControl, true, GraphicElement::sResizeControlDefault, tab)
}
