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
#include "ExpressionFunctionUrlEncode.h"
#include "ExpressionHelper.h"
#include "Foundation.h"
#include "UnicodeString.h"


ExpressionFunctionUrlEncode::ExpressionFunctionUrlEncode()
{
    m_urlEncodeValue = FdoStringValue::Create();
    m_functionDefinition = NULL;
}


ExpressionFunctionUrlEncode::~ExpressionFunctionUrlEncode()
{
    FDO_SAFE_RELEASE(m_urlEncodeValue);
    FDO_SAFE_RELEASE(m_functionDefinition);
}


ExpressionFunctionUrlEncode* ExpressionFunctionUrlEncode::Create()
{
    return new ExpressionFunctionUrlEncode();
}


FdoFunctionDefinition* ExpressionFunctionUrlEncode::GetFunctionDefinition()
{
    if (!m_functionDefinition)
    {
        STRING funcDesc = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgFunctionURLENCODE_Description");
        STRING sValDesc = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgFunctionURLENCODE_StringValueDescription");

        FdoPtr<FdoArgumentDefinition> arg = FdoArgumentDefinition::Create(L"strValue", sValDesc.c_str(), FdoDataType_String); // NOXLATE

        FdoPtr<FdoArgumentDefinitionCollection> args = FdoArgumentDefinitionCollection::Create();
        args->Add(arg);

        m_functionDefinition = FdoFunctionDefinition::Create(L"URLENCODE", // NOXLATE
                                                             funcDesc.c_str(),
                                                             FdoDataType_String,
                                                             args,
                                                             FdoFunctionCategoryType_String,
                                                             false);
    }

    return FDO_SAFE_ADDREF(m_functionDefinition);
}


// URL encodes a string.
//
// From the HTTP 1.1 specification:
//
//   Many HTTP/1.1 header field values consist of words separated by LWS
//   or special characters. These special characters MUST be in a quoted
//   string to be used within a parameter value (as defined in section 3.6).
//
//       token          = 1*<any CHAR except CTLs or separators>
//       separators     = "(" | ")" | "<" | ">" | "@"
//                      | "," | ";" | ":" | "\" | <">
//                      | "/" | "[" | "]" | "?" | "="
//                      | "{" | "}" | SP | HT
//
//
// 3.6 Transfer Codings
//
//   ...
//
//   Parameters are in the form of attribute/value pairs.
//
//       parameter    = attribute "=" value
//       attribute    = token
//       value        = token | quoted-string
//
FdoLiteralValue* ExpressionFunctionUrlEncode::Evaluate(FdoLiteralValueCollection* literalValues)
{
    // make sure we have one argument
    if (literalValues->GetCount() != 1)
    {
        MgResources* resources = MgResources::GetInstance();
        assert(NULL != resources);

        STRING message = MgUtil::GetResourceMessage(MgResources::Stylization, L"MgIncorrectNumberOfArguments");
        MgStringCollection arguments;
        arguments.Add(L"URLENCODE"); // NOXLATE
        message = resources->FormatMessage(message, &arguments);

        throw FdoExpressionException::Create(message.c_str());
    }

    FdoPtr<FdoLiteralValue> arg = literalValues->GetItem(0);

    const wchar_t* str = ExpressionHelper::GetAsString(arg);

    size_t len = str? wcslen(str) : 0;
    if (len >= 0)
    {
        // must first UTF8 encode
        string sutf8;
        const XMLCh* srcX = W2X(str);
        UnicodeString::UTF16toUTF8(srcX, sutf8);
        size_t utf8lenbytes = sutf8.length();

        // now URL encode the result
        size_t urllenbytes = 3*utf8lenbytes + 1;
        char* sUrl = (char*)alloca(urllenbytes * sizeof(char));
        memset(sUrl, 0, urllenbytes * sizeof(char));

        size_t i, j;
        for (i=j=0; i<utf8lenbytes; ++i)
        {
            unsigned char chr = (unsigned char)sutf8[i];
            if ((chr <= 0x20) || // a space or anything below it in value
                (chr == 0x21) || // a ! (exclamation)
                (chr == 0x22) || // a " (quotation mark)
                (chr == 0x23) || // a # (hash)
                (chr == 0x24) || // a $ (dollar)
                (chr == 0x25) || // a % (percentage)
                (chr == 0x26) || // a & (ampersand)
                (chr == 0x27) || // a ' (apostrophe)
                (chr == 0x28) || // a (
                (chr == 0x29) || // a )
//              (chr == 0x2A) || // a * (asterisk)
                (chr == 0x2B) || // a + (plus)
                (chr == 0x2C) || // a , (comma)
//              (chr == 0x2D) || // a - (hyphen)
//              (chr == 0x2E) || // a . (period)
                (chr == 0x2F) || // a / (forward slash)
                (chr == 0x3A) || // a : (colon)
                (chr == 0x3B) || // a ; (semi-colon)
                (chr == 0x3C) || // a < (less than)
                (chr == 0x3D) || // a = (equals)
                (chr == 0x3E) || // a > (greater than)
                (chr == 0x3F) || // a ? (question mark)
                (chr == 0x40) || // a @ (at)
                (chr == 0x5B) || // a [
                (chr == 0x5C) || // a \ (backslash)
                (chr == 0x5D) || // a ]
                (chr == 0x5E) || // a ^
//              (chr == 0x5F) || // a _ (underscore)
                (chr == 0x60) || // a `
                (chr == 0x7B) || // a { (opening bracket)
                (chr == 0x7C) || // a | (pipe)
                (chr == 0x7D) || // a } (closing bracket)
                (chr == 0x7E) || // a ~ (tilde)
                (chr == 0x7F) || // a control character
                (chr >= 0x80))   // 8-bit (encoded)
                j += sprintf(&sUrl[j], "%%%2X", chr);
            else
                sUrl[j++] = chr;
        }

        // finally, convert to a wide string
        wchar_t* res = (wchar_t*)alloca((j+1) * sizeof(wchar_t));
        for (size_t k=0; k<=j; ++k)
            res[k] = (wchar_t)sUrl[k];

        m_urlEncodeValue->SetString(res);
    }
    else
    {
        m_urlEncodeValue->SetString(L"");
    }

    return FDO_SAFE_ADDREF(m_urlEncodeValue);
}


FdoExpressionEngineIFunction* ExpressionFunctionUrlEncode::CreateObject()
{
    return ExpressionFunctionUrlEncode::Create();
}


void ExpressionFunctionUrlEncode::Dispose()
{
    delete this;
}
