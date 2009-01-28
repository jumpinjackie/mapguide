//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#ifndef _IOUTIL_H
#define _IOUTIL_H

#include "MdfModel.h"
#include "MdfParser.h"
#include <sstream>

using namespace XERCES_CPP_NAMESPACE;
using namespace MDFMODEL_NAMESPACE;

BEGIN_NAMESPACE_MDFPARSER

    // TODO: The streams below should really be changed to std::w* versions to
    // output unicode strings but MapGuide explicitly supports UTF-8 characters in the
    // output for some reason; If we have time, we should come back to this and fix
    // it.  -- Reva
    typedef std::ostream MdfStream;
    typedef std::ostringstream MdfStringStream;

    void UP(char** str);

    std::wstring toMdfString(char* str);
    std::string EncodeString(const std::wstring& str);
    MDFPARSER_API std::string toCString(const std::wstring& str);

    bool wstrToBool(const wchar_t* ch);
    double wstrToDouble(const wchar_t* ch);
    int wstrToInt(const wchar_t* ch);

    bool wstrToBool(const MdfString& str, bool& result);
    bool wstrToDouble(const MdfString& str, double& result);
    bool wstrToInt(const MdfString& str, int& result);

    std::string IntToStr(int i);
    std::string DoubleToStr(double d);
    std::string BoolToStr(bool b);

    std::string tab();

    int gettabcount();
    void inctab();
    void dectab();
    void zerotab();
    void disableTabs();

    std::string startStr(const std::string& elementName);
    std::string endStr(const std::string& elementName);

END_NAMESPACE_MDFPARSER
#endif // _IOUTIL_H
