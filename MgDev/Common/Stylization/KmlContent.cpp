//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

//using this in contructor
#pragma warning(disable:4355)

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
    WriteString("<Document>");
}

void KmlContent::EndDocument()
{
    WriteString("</Document>");
    WriteString("</kml>");
}

void KmlContent::WriteString(const char* mbString)
{
    m_content.sputn(mbString, (std::streamsize)strlen(mbString));
}

void KmlContent::WriteString(const std::string& mbString)
{
    m_content.sputn(mbString.c_str(), (std::streamsize)mbString.length());
}

void KmlContent::WriteString(const wchar_t* wString)
{
    std::string mbString;
    UnicodeString::WideCharToMultiByte(wString, mbString);
    WriteString(mbString);
}

void KmlContent::WriteString(const std::wstring& wString)
{
    std::string mbString;
    UnicodeString::WideCharToMultiByte(wString.c_str(), mbString);
    WriteString(mbString);
}

std::string KmlContent::GetString()
{
    return m_content.str();
}

