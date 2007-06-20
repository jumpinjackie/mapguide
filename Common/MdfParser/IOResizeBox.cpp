//
//  Copyright (C) 2007 by Autodesk, Inc.
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


IOResizeBox::IOResizeBox(SimpleSymbolDefinition* symbolDefinition)
{
    this->_symbolDefinition = symbolDefinition;
    this->_resizeBox = NULL;
}


IOResizeBox::~IOResizeBox()
{
}


void IOResizeBox::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"ResizeBox") // NOXLATE
    {
        m_startElemName = name;
        this->_resizeBox = new ResizeBox();
    }
    else if (m_currElemName == L"ExtendedData1") // NOXLATE
    {
        ParseUnknownXml(name, handlerStack);
    }
}


void IOResizeBox::ElementChars(const wchar_t* ch)
{
         IF_STRING_PROPERTY(m_currElemName, this->_resizeBox, SizeX, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_resizeBox, SizeY, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_resizeBox, PositionX, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_resizeBox, PositionY, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_resizeBox, GrowControl, ch)
}


void IOResizeBox::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (m_startElemName == name)
    {
        this->_resizeBox->SetUnknownXml(UnknownXml());

        this->_symbolDefinition->AdoptResizeBox(this->_resizeBox);
        this->_symbolDefinition = NULL;
        this->_resizeBox = NULL;
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}


void IOResizeBox::Write(MdfStream& fd, ResizeBox* resizeBox, Version* version)
{
    fd << tab() << "<ResizeBox>" << std::endl; // NOXLATE
    inctab();

    EMIT_DOUBLE_PROPERTY(fd, resizeBox, SizeX, false, 1.0)
    EMIT_DOUBLE_PROPERTY(fd, resizeBox, SizeY, false, 1.0)
    EMIT_DOUBLE_PROPERTY(fd, resizeBox, PositionX, false, 0.0)
    EMIT_DOUBLE_PROPERTY(fd, resizeBox, PositionY, false, 0.0)
    EMIT_STRING_PROPERTY(fd, resizeBox, GrowControl, false, NULL)

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, resizeBox->GetUnknownXml(), version);

    dectab();
    fd << tab() << "</ResizeBox>" << std::endl; // NOXLATE
}
