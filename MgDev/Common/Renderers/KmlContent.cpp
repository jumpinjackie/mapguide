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

#include "KmlContent.h"
#include "UnicodeString.h"


//default constructor
KmlContent::KmlContent()
{
}


KmlContent::~KmlContent()
{
}


void KmlContent::StartDocument()
{
    WriteString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>");
    WriteString("<kml xmlns=\"http://earth.google.com/kml/2.1\">");
    WriteString("<NetworkLinkControl><minRefreshPeriod>2</minRefreshPeriod></NetworkLinkControl>");
    WriteString("<Document>");
}


void KmlContent::EndDocument()
{
    WriteString("</Document>");
    WriteString("</kml>");
}


void KmlContent::WriteString(const char* szString, streamsize length, bool lineBreak)
{
    m_content.sputn(szString, length);
    if (lineBreak)
    {
        m_content.sputc('\n');
    }
}


void KmlContent::WriteString(const char* szString, bool lineBreak /*= true*/)
{
    WriteString(szString, (std::streamsize)strlen(szString), lineBreak);
}


void KmlContent::WriteString(const std::string& mbString, bool lineBreak /*= true*/)
{
    WriteString(mbString.c_str(), (std::streamsize)mbString.length(), lineBreak);
}


void KmlContent::WriteString(const wchar_t* wszString, bool lineBreak /*= true*/)
{
    std::string mbString;
    UnicodeString::WideCharToMultiByte(wszString, mbString);
    WriteString(mbString, lineBreak);
}


void KmlContent::WriteString(const std::wstring& wString, bool lineBreak /*= true*/)
{
    std::string mbString;
    UnicodeString::WideCharToMultiByte(wString.c_str(), mbString);
    WriteString(mbString, lineBreak);
}


std::string KmlContent::GetString()
{
    return m_content.str();
}
