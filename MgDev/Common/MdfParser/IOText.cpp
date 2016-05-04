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
#include "IOText.h"
#include "IOTextFrame.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOText::IOText(Text* text, Version& version) : IOGraphicElement(text, version)
{
}


void IOText::StartTextElement(const wchar_t* name, HandlerStack* handlerStack)
{
    // the element is a text with the supplied name
    this->m_currElemName = name;
    this->m_startElemName = name;
}


void IOText::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"Frame") // NOXLATE
    {
        Text* text = static_cast<Text*>(this->m_element);
        IOTextFrame* IO = new IOTextFrame(text, this->m_version);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
    }
    else if (this->m_currElemName == L"Overlined") // NOXLATE
    {
        // value read in ElementChars
    }
    else if (this->m_currElemName == L"ObliqueAngle") // NOXLATE
    {
        // value read in ElementChars
    }
    else if (this->m_currElemName == L"TrackSpacing") // NOXLATE
    {
        // value read in ElementChars
    }
    else if (this->m_currElemName == L"Markup") // NOXLATE
    {
        // value read in ElementChars
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


void IOText::ElementChars(const wchar_t* ch)
{
    Text* text = static_cast<Text*>(this->m_element);

         IF_STRING_PROPERTY(this->m_currElemName, text, Content, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, text, FontName, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, text, Bold, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, text, Italic, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, text, Underlined, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, text, Overlined, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, text, ObliqueAngle, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, text, TrackSpacing, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, text, Height, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, text, HeightScalable, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, text, Angle, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, text, PositionX, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, text, PositionY, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, text, HorizontalAlignment, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, text, VerticalAlignment, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, text, Justification, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, text, LineSpacing, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, text, TextColor, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, text, GhostColor, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, text, Markup, ch)
    else IOGraphicElement::ElementChars(ch);
}


void IOText::Write(MdfStream& fd, Text* text, Version* version, MgTab& tab)
{
    fd << tab.tab() << "<Text>" << std::endl; // NOXLATE
    tab.inctab();

    MdfStringStream fdExtData;

    IOGraphicElement::Write(fd, text, version, tab);

    EMIT_STRING_PROPERTY(fd, text, Content, false, NULL, tab)
    EMIT_STRING_PROPERTY(fd, text, FontName, false, NULL, tab)
    EMIT_BOOL_PROPERTY(fd, text, Bold, true, false, tab)                           // default is false
    EMIT_BOOL_PROPERTY(fd, text, Italic, true, false, tab)                         // default is false
    EMIT_BOOL_PROPERTY(fd, text, Underlined, true, false, tab)                     // default is false

    if (!version || (*version >= Version(1, 1, 0)))
    {
        EMIT_BOOL_PROPERTY(fd, text, Overlined, true, false, tab)                  // default is false
        EMIT_DOUBLE_PROPERTY(fd, text, ObliqueAngle, true, 0.0, tab)               // default is 0.0
        EMIT_DOUBLE_PROPERTY(fd, text, TrackSpacing, true, 1.0, tab)               // default is 1.0
    }
    else if (*version == Version(1, 0, 0))
    {
        // save new properties as extended data for symbol definition version 1.0.0
        tab.inctab();

        EMIT_BOOL_PROPERTY(fdExtData, text, Overlined, true, false, tab)           // default is false
        EMIT_DOUBLE_PROPERTY(fdExtData, text, ObliqueAngle, true, 0.0, tab)        // default is 0.0
        EMIT_DOUBLE_PROPERTY(fdExtData, text, TrackSpacing, true, 1.0, tab)        // default is 1.0

        tab.dectab();
    }

    EMIT_DOUBLE_PROPERTY(fd, text, Height, true, 4.0, tab)                         // default is 4.0
    EMIT_BOOL_PROPERTY(fd, text, HeightScalable, true, true, tab)                  // default is true
    EMIT_DOUBLE_PROPERTY(fd, text, Angle, true, 0.0, tab)                          // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, text, PositionX, true, 0.0, tab)                      // default is 0.0
    EMIT_DOUBLE_PROPERTY(fd, text, PositionY, true, 0.0, tab)                      // default is 0.0
    EMIT_STRING_PROPERTY(fd, text, HorizontalAlignment, true, Text::sHAlignmentDefault, tab)
    EMIT_STRING_PROPERTY(fd, text, VerticalAlignment, true, Text::sVAlignmentDefault, tab)
    EMIT_STRING_PROPERTY(fd, text, Justification, true, Text::sJustificationDefault, tab)
    EMIT_DOUBLE_PROPERTY(fd, text, LineSpacing, true, 1.05, tab)                   // default is 1.05
    EMIT_STRING_PROPERTY(fd, text, TextColor, true, L"ff000000", tab)              // default is ff000000
    EMIT_STRING_PROPERTY(fd, text, GhostColor, true, L"", tab)                     // default is empty string

    if (text->GetFrame())
        IOTextFrame::Write(fd, text->GetFrame(), version, tab);

    if (!version || (*version >= Version(1, 1, 0)))
    {
        EMIT_STRING_PROPERTY(fd, text, Markup, true, Text::sMarkupDefault, tab)
    }
    else if (*version == Version(1, 0, 0))
    {
        // save new property as extended data for symbol definition version 1.0.0
        tab.inctab();

        EMIT_STRING_PROPERTY(fdExtData, text, Markup, true, Text::sMarkupDefault, tab)

        tab.dectab();
    }

    // Write the unknown XML / extended data
    IOUnknown::Write(fd, text->GetUnknownXml(), fdExtData.str(), version, tab);

    tab.dectab();
    fd << tab.tab() << "</Text>" << std::endl; // NOXLATE
}
