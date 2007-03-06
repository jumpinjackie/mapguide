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
#include "IOTextFrame.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOTextFrame::IOTextFrame(Text* text)
{
    this->_text = text;
    this->_textFrame = NULL;
}

IOTextFrame::~IOTextFrame()
{
}

void IOTextFrame::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"Frame") // NOXLATE
    {
        m_startElemName = name;
        this->_textFrame = new TextFrame();
    }
}

void IOTextFrame::ElementChars(const wchar_t *ch)
{
         IF_STRING_PROPERTY(m_currElemName, this->_textFrame, LineColor, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_textFrame, FillColor, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_textFrame, OffsetX, ch)
    else IF_STRING_PROPERTY(m_currElemName, this->_textFrame, OffsetY, ch)
}

void IOTextFrame::EndElement(const wchar_t *name, HandlerStack *handlerStack)
{
    if (m_startElemName == name)
    {
        this->_text->AdoptFrame(this->_textFrame);
        this->_text = NULL;
        this->_textFrame = NULL;
        m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
}

void IOTextFrame::Write(MdfStream &fd, TextFrame* textFrame)
{
    fd << tab() << "<Frame>" << std::endl; // NOXLATE
    inctab();

    EMIT_STRING_PROPERTY(fd, textFrame, LineColor, true)
    EMIT_STRING_PROPERTY(fd, textFrame, FillColor, true)
    EMIT_STRING_PROPERTY(fd, textFrame, OffsetX, true)
    EMIT_STRING_PROPERTY(fd, textFrame, OffsetY, true)

    dectab();
    fd << tab() << "</Frame>" << std::endl; // NOXLATE
}
