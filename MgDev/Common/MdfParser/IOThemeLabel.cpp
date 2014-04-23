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
#include "IOThemeLabel.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOThemeLabel::IOThemeLabel(Override* pOverride, Version& version) : SAX2ElementHandler(version)
{
    this->m_override = pOverride;
    this->m_themeLabel = NULL;
}


IOThemeLabel::~IOThemeLabel()
{
}


void IOThemeLabel::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"ThemeLabel") // NOXLATE
    {
        this->m_startElemName = name;
        this->m_themeLabel = new ThemeLabel();
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


void IOThemeLabel::ElementChars(const wchar_t* ch)
{
         IF_STRING_PROPERTY(this->m_currElemName, this->m_themeLabel, Description, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_themeLabel, CategoryFormat, ch)
}


void IOThemeLabel::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_themeLabel->SetUnknownXml(this->m_unknownXml);

        this->m_override->AdoptThemeLabel(this->m_themeLabel);
        this->m_override = NULL;
        this->m_themeLabel = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (::wcscmp(name, L"ExtendedData1") == 0) // NOXLATE
    {
        this->m_procExtData = false;
    }
}


void IOThemeLabel::Write(MdfStream& fd, ThemeLabel* pThemeLabel, Version* version, MgTab& tab)
{
    fd << tab.tab() << "<ThemeLabel>" << std::endl; // NOXLATE
    tab.inctab();

    EMIT_STRING_PROPERTY(fd, pThemeLabel, Description, false, NULL, tab)
    EMIT_STRING_PROPERTY(fd, pThemeLabel, CategoryFormat, false, NULL, tab)

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, pThemeLabel->GetUnknownXml(), version, tab);

    tab.dectab();
    fd << tab.tab() << "</ThemeLabel>" << std::endl; // NOXLATE
}
