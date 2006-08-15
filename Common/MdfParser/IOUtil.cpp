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
#include "IOUtil.h"
#include "UnicodeString.h"

BEGIN_NAMESPACE_MDFPARSER

int tabcount = 0;

std::string tab()
{
    std::string ret;
    for(int x = 0; x < tabcount; x++)
    {
        ret += "  ";
    }
    return ret;
}

void inctab()
{
    tabcount++;
}
void dectab()
{
    tabcount--;
}
void zerotab()
{
    tabcount = 0;
}

void UP(char ** str)
{
    int x = 0;
    while((*str)[x] != 0)
    {
        if ((*str)[x] >= 97 && (*str)[x] <= 122)
            (*str)[x] -= 32;
        x++;
    }
}

std::wstring toMdfString(char * str)
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

std::string toCString(std::wstring str)
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

std::string EncodeString(std::wstring str)
{
    std::string out;
    std::string s = toCString(str);
    out.reserve(s.length());
    const char* ret = s.c_str();
    size_t length = s.length();

    for(size_t i = 0; i < length; i++)
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

bool wstrToBool(const wchar_t *ch)
{
    return (::wcscmp(ch, L"true") == 0);
}

double wstrToDouble(const wchar_t *ch)
{
    #ifdef _WIN32
        return _wtof(ch);
    #else
        return (double)wcstold(ch, NULL);  // radix is always 10
    #endif
}

int wstrToInt(const wchar_t *ch)
{
    #ifdef _WIN32
        return _wtoi(ch);
    #else
        return (int)wcstol(ch, NULL, 10);  // radix is always 10
    #endif

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
    if(b)
        return std::string("true"); // NOXLATE
    return std::string("false"); // NOXLATE
}

END_NAMESPACE_MDFPARSER
