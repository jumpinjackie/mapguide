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
#include "IOText.h"
#include "IOTextFrame.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;

IOText::IOText(Text* text) : IOGraphicElement(text)
{
}

void IOText::StartTextElement(const wchar_t *name, HandlerStack *handlerStack)
{
    // the element is a text with the supplied name
    m_currElemName = name;
    m_startElemName = name;
}

void IOText::StartElement(const wchar_t *name, HandlerStack *handlerStack)
{
    m_currElemName = name;
    if (m_currElemName == L"Frame") // NOXLATE
    {
        Text* text = static_cast<Text*>(this->_element);
        IOTextFrame* IO = new IOTextFrame(text);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
}

void IOText::ElementChars(const wchar_t *ch)
{
    Text* text = static_cast<Text*>(this->_element);

         IF_STRING_PROPERTY(m_currElemName, text, String, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, FontName, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, Bold, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, Italic, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, Underlined, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, Height, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, HeightScalable, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, Angle, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, PositionX, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, PositionY, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, LineSpacing, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, TextColor, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, GhostColor, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, HorizontalAlignment, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, VerticalAlignment, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, Justification, ch)
    else IOGraphicElement::ElementChars(ch);
}

void IOText::Write(MdfStream &fd, Text* text)
{
    fd << tab() << "<Text>" << std::endl; // NOXLATE
    inctab();

    IOGraphicElement::Write(fd, text);

    EMIT_STRING_PROPERTY(fd, text, String, false)
    EMIT_STRING_PROPERTY(fd, text, FontName, false)
    EMIT_STRING_PROPERTY(fd, text, Bold, true)
    EMIT_STRING_PROPERTY(fd, text, Italic, true)
    EMIT_STRING_PROPERTY(fd, text, Underlined, true)
    EMIT_STRING_PROPERTY(fd, text, Height, true)
    EMIT_STRING_PROPERTY(fd, text, HeightScalable, true)
    EMIT_STRING_PROPERTY(fd, text, Angle, true)
    EMIT_STRING_PROPERTY(fd, text, PositionX, true)
    EMIT_STRING_PROPERTY(fd, text, PositionY, true)
    EMIT_STRING_PROPERTY(fd, text, HorizontalAlignment, true)
    EMIT_STRING_PROPERTY(fd, text, VerticalAlignment, true)
    EMIT_STRING_PROPERTY(fd, text, Justification, true)
    EMIT_STRING_PROPERTY(fd, text, LineSpacing, true)
    EMIT_STRING_PROPERTY(fd, text, TextColor, true)
    EMIT_STRING_PROPERTY(fd, text, GhostColor, true)

    if (text->GetFrame() != NULL)
        IOTextFrame::Write(fd, text->GetFrame());

    dectab();
    fd << tab() << "</Text>" << std::endl; // NOXLATE
}
