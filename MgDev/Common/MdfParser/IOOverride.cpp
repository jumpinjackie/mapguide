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
#include "IOOverride.h"
#include "IOThemeLabel.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOOverride::IOOverride(OverrideCollection* overrideCollection, Version& version) : SAX2ElementHandler(version)
{
    this->m_overrideCollection = overrideCollection;
    this->m_override = NULL;
}


IOOverride::~IOOverride()
{
}


void IOOverride::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"Override") // NOXLATE
    {
        this->m_startElemName = name;
        this->m_override = new Override();
    }
    else if (this->m_currElemName == L"ThemeLabel") // NOXLATE
    {
        IOThemeLabel* IO = new IOThemeLabel(this->m_override, this->m_version);
        handlerStack->push(IO);
        IO->StartElement(name, handlerStack);
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


void IOOverride::ElementChars(const wchar_t* ch)
{
         IF_STRING_PROPERTY(this->m_currElemName, this->m_override, SymbolName, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_override, ParameterIdentifier, ch)
    else IF_STRING_PROPERTY(this->m_currElemName, this->m_override, ParameterValue, ch)
}


void IOOverride::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_startElemName == name)
    {
        this->m_override->SetUnknownXml(this->m_unknownXml);

        this->m_overrideCollection->Adopt(this->m_override);
        this->m_overrideCollection = NULL;
        this->m_override = NULL;
        this->m_startElemName = L"";
        handlerStack->pop();
        delete this;
    }
    else if (::wcscmp(name, L"ExtendedData1") == 0) // NOXLATE
    {
        this->m_procExtData = false;
    }
}


void IOOverride::Write(MdfStream& fd, Override* pOverride, Version* version, MgTab& tab)
{
    fd << tab.tab() << "<Override>" << std::endl; // NOXLATE
    tab.inctab();

    MdfStringStream fdExtData;

    EMIT_STRING_PROPERTY(fd, pOverride, SymbolName, false, NULL, tab)
    EMIT_STRING_PROPERTY(fd, pOverride, ParameterIdentifier, false, NULL, tab)
    EMIT_STRING_PROPERTY(fd, pOverride, ParameterValue, false, NULL, tab)

    ThemeLabel* themeLabel = pOverride->GetThemeLabel();
    if (themeLabel)
    {
        // only write ThemeLabel if the LDF version is 1.2.0 or greater
        if (!version || (*version >= Version(1, 2, 0)))
        {
            IOThemeLabel::Write(fd, themeLabel, version, tab);
        }
        else if (*version >= Version(1, 0, 0))
        {
            // save ThemeLabel as extended data for LDF versions 1.0.0 and 1.1.0
            tab.inctab();
            IOThemeLabel::Write(fdExtData, themeLabel, version, tab);
            tab.dectab();
        }
    }

    // Write the unknown XML / extended data
    IOUnknown::Write(fd, pOverride->GetUnknownXml(), fdExtData.str(), version, tab);

    tab.dectab();
    fd << tab.tab() << "</Override>" << std::endl; // NOXLATE
}
