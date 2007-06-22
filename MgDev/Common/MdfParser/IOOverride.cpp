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
#include "IOOverride.h"
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOOverride::IOOverride(OverrideCollection* overrideCollection)
{
    this->m_overrideCollection = overrideCollection;
}


void IOOverride::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_currElemName == L"Override") // NOXLATE
    {
        this->m_startElemName = name;
        this->m_override = new Override();
    }
    else if (this->m_currElemName == L"ExtendedData1") // NOXLATE
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
}


void IOOverride::Write(MdfStream& fd, Override* pOverride, Version* version)
{
    fd << tab() << "<Override>" << std::endl; // NOXLATE
    inctab();

    EMIT_STRING_PROPERTY(fd, pOverride, SymbolName, false, NULL)
    EMIT_STRING_PROPERTY(fd, pOverride, ParameterIdentifier, false, NULL)
    EMIT_STRING_PROPERTY(fd, pOverride, ParameterValue, false, NULL)

    // Write any unknown XML / extended data
    IOUnknown::Write(fd, pOverride->GetUnknownXml(), version);

    dectab();
    fd << tab() << "</Override>" << std::endl; // NOXLATE
}
