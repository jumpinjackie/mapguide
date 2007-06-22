//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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
#include "IOUnknown.h"

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFPARSER_NAMESPACE;


IOUnknown::IOUnknown(std::wstring* xml, bool writeStartElement)
{
    this->m_xml = xml;
    this->m_writeStartElement = writeStartElement;
    this->m_nesting_level = 0;
    this->m_startElemName.clear();
}


IOUnknown::~IOUnknown()
{
}

void IOUnknown::StartElement(const wchar_t* name, HandlerStack* handlerStack)
{
    this->m_currElemName = name;
    if (this->m_startElemName.empty())
    {
        this->m_startElemName = name;

        // only store the start element name if requested
        if (this->m_writeStartElement)
        {
            this->m_xml->append(L"<"); // NOXLATE
            this->m_xml->append(name);
            this->m_xml->append(L">"); // NOXLATE
        }

        this->m_nesting_level = 0;
    }
    else
    {
        this->m_xml->append(L"<"); // NOXLATE
        this->m_xml->append(name);
        this->m_xml->append(L">"); // NOXLATE

        ++this->m_nesting_level;
    }
}


void IOUnknown::ElementChars(const wchar_t* ch)
{
    this->m_xml->append(ch);
}


void IOUnknown::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    // we may encounter nested extended data elements, hence the level check
    if (this->m_startElemName == name && this->m_nesting_level == 0)
    {
        // only store the start element name if requested
        if (this->m_writeStartElement)
        {
            this->m_xml->append(L"</"); // NOXLATE
            this->m_xml->append(name);
            this->m_xml->append(L">"); // NOXLATE
        }

        handlerStack->pop();
        delete this;
    }
    else
    {
        this->m_xml->append(L"</"); // NOXLATE
        this->m_xml->append(name);
        this->m_xml->append(L">"); // NOXLATE

        --this->m_nesting_level;
    }
}


// The supplied data is assumed to already be tabbed.
void IOUnknown::WriteRaw(MdfStream& fd, const std::string& data, Version* version)
{
    if (data.empty())
        return;

    if (!version || (*version >= Version(1, 0, 0)))
    {
        fd << tab() << "<ExtendedData1>" << std::endl; // NOXLATE
        fd << data << std::endl;
        fd << tab() << "</ExtendedData1>" << std::endl; // NOXLATE
    }
}


// The supplied data is assumed to be untabbed.
void IOUnknown::Write(MdfStream& fd, const std::wstring& data, Version* version)
{
    if (data.empty())
        return;

    if (!version || (*version >= Version(1, 0, 0)))
    {
        fd << tab() << "<ExtendedData1>" << std::endl; // NOXLATE
        inctab();
        fd << tab() << toCString(data) << std::endl;
        dectab();
        fd << tab() << "</ExtendedData1>" << std::endl; // NOXLATE
    }
}
