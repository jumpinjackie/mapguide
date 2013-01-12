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
#include "IOUtil.h"
#include "UnicodeString.h"

BEGIN_NAMESPACE_MDFPARSER

bool MgTab::m_tabsEnabled = true;


std::wstring toMdfString(const char* str)
{
    std::wstring s;
    try
    {
        UnicodeString::MultiByteToWideChar(str, s);
    }
    catch (int)
    {
        // TODO:  How do we handle exceptions from here?
    }
    return s;
}


std::string toCString(const std::wstring& str)
{
    std::string s;
    try
    {
        UnicodeString::WideCharToMultiByte(str.c_str(), s);
    }
    catch (int)
    {
        // TODO:  How do we handle exceptions from here?
    }

    return s;
}


std::string EncodeString(const std::wstring& str)
{
    std::string out;
    std::string s = toCString(str);
    out.reserve(s.length());
    const char* ret = s.c_str();
    size_t length = s.length();

    for (size_t i=0; i<length; ++i)
    {
        if (ret[i] == '&')          out.append("&amp;"); // NOXLATE
        else if (ret[i] == '<')     out.append("&lt;"); // NOXLATE
        else if (ret[i] == '>')     out.append("&gt;"); // NOXLATE
        else if (ret[i] == '"')     out.append("&quot;"); // NOXLATE
        else if (ret[i] == '\'')    out.append("&apos;"); // NOXLATE
        else                        out.append(1, ret[i]);
    }

    return out;
}


bool wstrToBool(const wchar_t* ch)
{
    return (_wcsnicmp(ch, L"true", 5) == 0);
}


double wstrToDouble(const wchar_t* ch)
{
    #ifdef _WIN32
        return _wtof(ch);
    #else
        return (double)wcstold(ch, NULL);  // radix is always 10
    #endif
}


int wstrToInt(const wchar_t* ch)
{
    #ifdef _WIN32
        return _wtoi(ch);
    #else
        return (int)wcstol(ch, NULL, 10);  // radix is always 10
    #endif
}


// Returns true if the string was successfully converted.
bool wstrToBool(const MdfString& str, bool& result)
{
    size_t len = str.size();
    if (len == 0)
        return false;

    const wchar_t* cstr = str.c_str();

    if (_wcsnicmp(cstr, L"true", 5) == 0)
    {
        result = true;
        return true;
    }

    if (_wcsnicmp(cstr, L"false", 6) == 0)
    {
        result = false;
        return true;
    }

    return false;
}


// Returns true if the string was successfully converted.
bool wstrToDouble(const MdfString& str, double& result)
{
    size_t len = str.size();
    if (len == 0)
        return false;

    const wchar_t* cstr = str.c_str();
    size_t chars = 0;
    int ret = swscanf(cstr, L"%lf%n", &result, &chars);
    if (ret == 1 && chars == len)
        return true;

    return false;
}


// Returns true if the string was successfully converted.
bool wstrToInt(const MdfString& str, int& result)
{
    size_t len = str.size();
    if (len == 0)
        return false;

    const wchar_t* cstr = str.c_str();
    size_t chars = 0;
    int ret = swscanf(cstr, L"%d%n", &result, &chars);
    if (ret == 1 && chars == len)
        return true;

    return false;
}


std::string IntToStr(int i)
{
    char buffer[32];
    sprintf(buffer, "%d", i);
    return buffer;
}


std::string DoubleToStr(double d)
{
    char buffer[64];
    sprintf(buffer, "%.17g", d);
    return buffer;
}


std::string BoolToStr(bool b)
{
    if (b)
        return std::string("true"); // NOXLATE
    return std::string("false"); // NOXLATE
}


std::string startStr(const std::string& elementName)
{
    std::string out("<"); // NOXLATE
    out.append(elementName);
    out.append(">"); // NOXLATE
    return out;
}


std::string endStr(const std::string& elementName)
{
    std::string out("</"); // NOXLATE
    out.append(elementName);
    out.append(">"); // NOXLATE
    return out;
}

END_NAMESPACE_MDFPARSER
