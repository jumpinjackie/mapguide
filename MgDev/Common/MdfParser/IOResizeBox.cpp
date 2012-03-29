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
#include "IOResizeBox.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOResizeBox::IOResizeBox(SimpleSymbolDefinition* symbolDefinition, Version& version) : SAX2ElementHandler(version)
{
    this->m_symbolDefinition = symbolDefinition;
    this->m_resizeBox = NULL;
}


IOResizeBox::~IOResizeBox()
{
}


void IOResizeBox::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"ResizeBox") // NOXLATE
    {
        this->m_startElemName = name;
        this->m_resizeBox = new ResizeBox();
    }
    else if (this->m_currElemName == L"ExtendedData1") // NOXLATE
    {
        this->m_procExtData = true;
    }
    else
    {
        ParseUnknownXml(name, handlerStack);
    }
}


void IOResizeBox::ElementChars(const wchar_t* ch)
{
         IF_STRING_PROPERTY(this->m_currElemName, this->m_resizeBox, SizeX, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_resizeBox, SizeY, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_resizeBox, PositionX, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_resizeBox, PositionY, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_resizeBox, GrowControl, ch)
}


void IOResizeBox::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_resizeBox->SetUnknownXml(this->m_unknownXml);

        this->m_symbolDefinition->AdoptResizeBox(this->m_resizeBox);
        this->m_symbolDefinition = NULL;
        this->m_resizeBox = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (::wcscmp(name, L"ExtendedData1") == 0) // NOXLATE
    {
        this->m_procExtData = false;
    }
}


void IOResizeBox::Write(MdfStream& fd, ResizeBox* resizeBox, Version* version, MgTab& tab)
{
    fd << tab.tab() << "<ResizeBox>" << std::endl; // NOXLATE
    tab.inctab();

    EMIT_DOUBLE_PROPERTY(fd, resizeBox, SizeX, false, 1.0, tab)
    EMIT_DOUBLE_PROPERTY(fd, resizeBox, SizeY, false, 1.0, tab)
    EMIT_DOUBLE_PROPERTY(fd, resizeBox, PositionX, false, 0.0, tab)
    EMIT_DOUBLE_PROPERTY(fd, resizeBox, PositionY, false, 0.0, tab)
    EMIT_STRING_PROPERTY(fd, resizeBox, GrowControl, false, NULL, tab)

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, resizeBox->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << "</ResizeBox>" << std::endl; // NOXLATE
}
