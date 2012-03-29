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
#include "IOTextFrame.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOTextFrame::IOTextFrame(Text* text, Version& version) : SAX2ElementHandler(version)
{
    this->m_text = text;
    this->m_textFrame = NULL;
}


IOTextFrame::~IOTextFrame()
{
}


void IOTextFrame::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"Frame") // NOXLATE
    {
        this->m_startElemName = name;
        this->m_textFrame = new TextFrame();
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


void IOTextFrame::ElementChars(const wchar_t* ch)
{
         IF_STRING_PROPERTY(this->m_currElemName, this->m_textFrame, LineColor, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_textFrame, FillColor, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_textFrame, OffsetX, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_textFrame, OffsetY, ch)
}


void IOTextFrame::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_textFrame->SetUnknownXml(this->m_unknownXml);

        this->m_text->AdoptFrame(this->m_textFrame);
        this->m_text = NULL;
        this->m_textFrame = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (::wcscmp(name, L"ExtendedData1") == 0) // NOXLATE
    {
        this->m_procExtData = false;
    }
}


void IOTextFrame::Write(MdfStream& fd, TextFrame* textFrame, Version* version, MgTab& tab)
{
    fd << tab.tab() << "<Frame>" << std::endl; // NOXLATE
    tab.inctab();

    EMIT_STRING_PROPERTY(fd, textFrame, LineColor, true, L"", tab)   // empty string is default
    EMIT_STRING_PROPERTY(fd, textFrame, FillColor, true, L"", tab)   // empty string is default
    EMIT_DOUBLE_PROPERTY(fd, textFrame, OffsetX, true, 0.0, tab)     // 0.0 is default
    EMIT_DOUBLE_PROPERTY(fd, textFrame, OffsetY, true, 0.0, tab)     // 0.0 is default

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, textFrame->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << "</Frame>" << std::endl; // NOXLATE
}
