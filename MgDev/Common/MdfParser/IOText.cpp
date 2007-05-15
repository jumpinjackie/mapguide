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
    else if (m_currElemName == L"ExtendedData1") // NOXLATE
    {
        ParseUnknownXml(name, handlerStack);
    }
}

void IOText::ElementChars(const wchar_t *ch)
{
    Text* text = static_cast<Text*>(this->_element);

         IF_STRING_PROPERTY(m_currElemName, text, Content, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, FontName, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, Bold, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, Italic, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, Underlined, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, Height, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, HeightScalable, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, Angle, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, PositionX, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, PositionY, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, HorizontalAlignment, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, VerticalAlignment, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, Justification, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, LineSpacing, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, TextColor, ch)
    else IF_STRING_PROPERTY(m_currElemName, text, GhostColor, ch)
    else IOGraphicElement::ElementChars(ch);
}

void IOText::Write(MdfStream &fd, Text* text)
{
    fd << tab() << "<Text>" << std::endl; // NOXLATE
    inctab();

    IOGraphicElement::Write(fd, text);

    EMIT_STRING_PROPERTY(fd, text, Content, false, NULL)
    EMIT_STRING_PROPERTY(fd, text, FontName, false, NULL)
    EMIT_BOOL_PROPERTY(fd, text, Bold, true, false)                           // default is false
    EMIT_BOOL_PROPERTY(fd, text, Italic, true, false)                         // default is false
    EMIT_BOOL_PROPERTY(fd, text, Underlined, true, false)                     // default is false
    EMIT_DOUBLE_PROPERTY(fd, text, Height, true, 4.0)                         // default is 4.0
    EMIT_BOOL_PROPERTY(fd, text, HeightScalable, true, true)                  // default is true
    EMIT_DOUBLE_PROPERTY(fd, text, Angle, true, 0.0)                          // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, text, PositionX, true, 0.0)                      // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, text, PositionY, true, 0.0)                      // default is 0.0
    EMIT_STRING_PROPERTY(fd, text, HorizontalAlignment, true, L"\'Center\'")  // default is 'Center'
    EMIT_STRING_PROPERTY(fd, text, VerticalAlignment, true, L"\'Halfline\'")  // default is 'Halfline'
    EMIT_STRING_PROPERTY(fd, text, Justification, true, L"\'FromAlignment\'") // default is 'FromAlignment'
    EMIT_DOUBLE_PROPERTY(fd, text, LineSpacing, true, 1.05)                   // default is 1.05
    EMIT_STRING_PROPERTY(fd, text, TextColor, true, L"ff000000")              // default is ff000000
    EMIT_STRING_PROPERTY(fd, text, GhostColor, true, L"")                     // default is empty string

    if (text->GetFrame() != NULL)
        IOTextFrame::Write(fd, text->GetFrame());

    // write any previously found unknown XML
    if (!text->GetUnknownXml().empty())
        fd << tab() << toCString(text->GetUnknownXml()) << std::endl;

    dectab();
    fd << tab() << "</Text>" << std::endl; // NOXLATE
}
