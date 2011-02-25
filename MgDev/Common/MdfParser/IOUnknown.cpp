//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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


IOUnknown::IOUnknown(std::wstring* xml, Version& version) : SAX2ElementHandler(version)
{
    this->m_xml = xml;
    this->m_nesting_level = 0;
    this->m_startElemName.clear();
    this->m_endedElement = false;
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
        this->m_nesting_level = 0;
    }
    else
    {
        ++this->m_nesting_level;
    }

    // here we're starting a new element - we need to add a newline if:
    // - this is a nested starting element (level 1+)
    // - we previously just ended an element
    // - this is a level-0 element being added to a non-empty unknown XML string
    if (this->m_nesting_level > 0 || this->m_endedElement || (this->m_nesting_level == 0 && this->m_xml->length() > 0))
        this->m_xml->append(L"\n");

    if (this->m_nesting_level > 0)
    {
        std::wstring wtt;
        wtt.reserve(this->m_nesting_level);
        wtt.append(this->m_nesting_level, L' ');
        this->m_xml->append(wtt);
    }

    this->m_xml->append(L"<"); // NOXLATE
    this->m_xml->append(name);
    this->m_xml->append(L">"); // NOXLATE

    this->m_endedElement = false;
}


void IOUnknown::ElementChars(const wchar_t* ch)
{
    this->m_xml->append(ch);
    this->m_endedElement = false;
}


void IOUnknown::EndElement(const wchar_t* name, HandlerStack* handlerStack)
{
    if (this->m_endedElement)
    {
        // we're ending multiple elements in a row - we need a newline
        this->m_xml->append(L"\n");

        if (this->m_nesting_level > 0)
        {
            std::wstring wtt;
            wtt.reserve(this->m_nesting_level);
            wtt.append(this->m_nesting_level, L' ');
            this->m_xml->append(wtt);
        }
    }

    this->m_xml->append(L"</"); // NOXLATE
    this->m_xml->append(name);
    this->m_xml->append(L">"); // NOXLATE
    this->m_endedElement = true;

    // we may encounter nested extended data elements, hence the level check
    if (this->m_startElemName == name && this->m_nesting_level == 0)
    {
        handlerStack->pop();
        delete this;
    }
    else
    {
        --this->m_nesting_level;
    }
}


// The supplied unknown data is assumed to not be tabbed.
void IOUnknown::Write(MdfStream& fd, const std::wstring& unkData, Version* version, MgTab& tab)
{
    if (unkData.empty())
        return;

    if (!version || (*version >= Version(1, 0, 0)))
    {
        fd << tab.tab() << "<ExtendedData1>" << std::endl; // NOXLATE

        WriteUnknown(fd, unkData, tab);

        fd << tab.tab() << "</ExtendedData1>" << std::endl; // NOXLATE
    }
}


// The supplied unknown data is assumed to be unformatted,
// while the raw data is assumed to be formatted.
void IOUnknown::Write(MdfStream& fd, const std::wstring& unkData, const std::string& rawData, Version* version, MgTab& tab)
{
    if (unkData.empty() && rawData.empty())
        return;

    if (!version || (*version >= Version(1, 0, 0)))
    {
        fd << tab.tab() << "<ExtendedData1>" << std::endl; // NOXLATE

        WriteRaw(fd, rawData);
        WriteUnknown(fd, unkData, tab);

        fd << tab.tab() << "</ExtendedData1>" << std::endl; // NOXLATE
    }
}


// The supplied data is assumed to be formatted.
void IOUnknown::WriteRaw(MdfStream& fd, const std::string& rawData)
{
    if (rawData.empty())
        return;

    // write the raw data
    fd << rawData;

    // only write a newline if the data doesn't already have one
    size_t i1 = rawData.find_last_of("\n");
    size_t i2 = rawData.length()-1;
    if (i1 != i2)
        fd << std::endl;
}


// The supplied data is assumed to be unformatted.
void IOUnknown::WriteUnknown(MdfStream& fd, const std::wstring& unkData, MgTab& tab)
{
    if (unkData.empty())
        return;

    // get the current tab count
    int tabCount = tab.gettabcount() + 1;
    std::wstring tabstr;
    tabstr.reserve(tabCount);
    tabstr.append(tabCount, L' ');

    wstring copy(unkData);

    // append a newline at the end if there isn't one
    size_t i1 = copy.find_last_of(L"\n");
    size_t i2 = copy.length()-1;
    if (i1 != i2)
        copy.append(L"\n");

    wstring newData;

    size_t start_pos = 0;
    size_t end_pos = copy.find_first_of(L"\n", start_pos);
    while (end_pos != wstring::npos)
    {
        newData.append(tabstr);
        newData.append(copy, start_pos, end_pos + 1 - start_pos);
        start_pos = end_pos + 1;
        end_pos = copy.find_first_of(L"\n", start_pos);
    }

    fd << toCString(newData);
}
