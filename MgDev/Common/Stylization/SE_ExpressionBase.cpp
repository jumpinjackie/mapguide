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
#include "SE_ExpressionBase.h"
#include "StylizationEngine.h"
#include "SimpleSymbolDefinition.h"
#include <wctype.h>

using namespace MDFMODEL_NAMESPACE;

extern void ProcessStylizerException(FdoException* exception, int line, wchar_t* file);

void SE_ExpressionBase::SetParameterValues(OverrideCollection* overrides)
{
    m_parameters.clear();
    int length = overrides->GetCount();
    for (int i=0; i<length; ++i)
    {
        Override* over = overrides->GetAt(i);
        const wchar_t* name = over->GetParameterIdentifier().c_str();
        const wchar_t* symbol = over->GetSymbolName().c_str();
        const wchar_t* value = over->GetParameterValue().c_str();
        m_parameters[ParamId(symbol, name)] = value;
    }
}


void SE_ExpressionBase::SetDefaultValues(SimpleSymbolDefinition* definition)
{
    m_defaults.clear();

    ParameterCollection* parameters = definition->GetParameterDefinition();
    m_symbol = definition->GetName().c_str();

    int length = parameters->GetCount();
    for (int i=0; i<length; ++i)
    {
        Parameter* param = parameters->GetAt(i);
        const wchar_t* name = param->GetIdentifier().c_str();
        const wchar_t* value = param->GetDefaultValue().c_str();
        m_defaults[name] = value;
    }
}


const wchar_t* SE_ExpressionBase::ReplaceParameters(const MdfModel::MdfString& exprstr)
{
    static const wchar_t* sEmpty = L"";

    const wchar_t* retDefValue = NULL;

    MdfString::size_type startIdx, endIdx, count;
    const wchar_t *name, *value, *trim;

    // trim whitespace from the beginning
    trim = exprstr.c_str();
    while (iswspace(*trim))
        trim++;

    m_buffer.assign(trim);
    startIdx = endIdx = 0;

    for (;;)
    {
        startIdx = m_buffer.find(sParameterDelimiter, startIdx);
        if (startIdx == MdfString::npos)
            break;

        endIdx = m_buffer.find(sParameterDelimiter, startIdx + 1);
        if (endIdx == MdfString::npos)
            break;

        // we have found matched % characters - it must be a parameter
        count = endIdx - startIdx - 1;
        m_param.assign(m_buffer, startIdx + 1, count);
        name = m_param.c_str();

        // retrieve any default value for the parameter
        const wchar_t* defValue = NULL;
        DefaultMap::const_iterator defIter = m_defaults.find(name);
        if (defIter != m_defaults.end())
        {
            defValue = (*defIter).second;

            // if the expression string is a single parameter then
            // return this default value
            if (startIdx == 0 && endIdx == m_buffer.length()-1)
                retDefValue = defValue;
        }

        // retrieve any override value for the parameter
        ParameterMap::const_iterator paramIter = m_parameters.find(ParamId(m_symbol, name));
        if (paramIter == m_parameters.end())
        {
            // no override found - use the default value
            value = defValue;
        }
        else
            value = (*paramIter).second;

        if (!value)
            value = sEmpty;

        m_buffer.replace(startIdx, count + 2, value);
        startIdx += wcslen(value);
    }

    // trim whitespace from the end
    size_t len = m_buffer.size();
    trim = m_buffer.c_str() + len - 1;
    while (len > 0 && iswspace(*trim--))
        len--;
    m_buffer.resize(len);

    return retDefValue;
}


void SE_ExpressionBase::ParseColorExpression(const MdfModel::MdfString& exprstr, SE_Color& val, unsigned int defaultValue)
{
    // set to schema default
    val.expression = NULL;
    val.defValue.argb = defaultValue;

    // process any parameters in the expression
    const wchar_t* defValue = ReplaceParameters(exprstr);

    // if we got back a default parameter value and can parse it
    // then that becomes our new default value
    if (defValue)
    {
        size_t chars = 0;
        unsigned int tempVal;
        int ret = swscanf(defValue, L"%X%n", &tempVal, &chars);
        if (ret == 1 && chars == wcslen(defValue))
            val.defValue.argb = tempVal;
    }

    // Initially the value is the default
    val.value.argb = val.defValue.argb;

    size_t len = m_buffer.size();
    if (len == 0)
        return;

    // try to parse the string
    const wchar_t* cstr = m_buffer.c_str();
    size_t chars = 0;
    unsigned int tempVal;
    int ret = swscanf(cstr, L"%X%n", &tempVal, &chars);
    if (ret == 1 && chars == len)
    {
        val.value.argb = tempVal;
        return;
    }

    // We have an expression - parse it using FDO.  If the parse
    // call fails then the value stays at the default.
    try
    {
        val.expression = FdoExpression::Parse(cstr);
    }
    catch (FdoException* e)
    {
        ProcessStylizerException(e, __LINE__, __WFILE__);
    }
}


void SE_ExpressionBase::ParseDoubleExpression(const MdfModel::MdfString& exprstr, SE_Double& val, const double defaultValue)
{
    // set to schema default
    val.expression = NULL;
    val.defValue = defaultValue;

    // process any parameters in the expression
    const wchar_t* defValue = ReplaceParameters(exprstr);

    // if we got back a default parameter value and can parse it
    // then that becomes our new default value
    if (defValue)
    {
        size_t chars = 0;
        double tempVal;
        int ret = swscanf(defValue, L"%lf%n", &tempVal, &chars);
        if (ret == 1 && chars == wcslen(defValue))
            val.defValue = tempVal;
    }

    // Initially the value is the default
    val.value = val.defValue;

    size_t len = m_buffer.size();
    if (len == 0)
        return;

    // try to parse the string
    const wchar_t* cstr = m_buffer.c_str();
    size_t chars = 0;
    double tempVal;
    int ret = swscanf(cstr, L"%lf%n", &tempVal, &chars);
    if (ret == 1 && chars == len)
    {
        val.value = tempVal;
        return;
    }

    // We have an expression - parse it using FDO.  If the parse
    // call fails then the value stays at the default.
    try
    {
        val.expression = FdoExpression::Parse(cstr);
    }
    catch (FdoException* e)
    {
        ProcessStylizerException(e, __LINE__, __WFILE__);
    }
}


void SE_ExpressionBase::ParseIntegerExpression(const MdfModel::MdfString& exprstr, SE_Integer& val, const int defaultValue)
{
    // set to schema default
    val.expression = NULL;
    val.defValue = defaultValue;

    // process any parameters in the expression
    const wchar_t* defValue = ReplaceParameters(exprstr);

    // if we got back a default parameter value and can parse it
    // then that becomes our new default value
    if (defValue)
    {
        size_t chars = 0;
        int tempVal;
        int ret = swscanf(defValue, L"%d%n", &tempVal, &chars);
        if (ret == 1 && chars == wcslen(defValue))
            val.defValue = tempVal;
    }

    // Initially the value is the default
    val.value = val.defValue;

    size_t len = m_buffer.size();
    if (len == 0)
        return;

    // try to parse the string
    const wchar_t* cstr = m_buffer.c_str();
    size_t chars = 0;
    int tempVal;
    int ret = swscanf(cstr, L"%d%n", &tempVal, &chars);
    if (ret == 1 && chars == len)
    {
        val.value = tempVal;
        return;
    }

    // We have an expression - parse it using FDO.  If the parse
    // call fails then the value stays at the default.
    try
    {
        val.expression = FdoExpression::Parse(cstr);
    }
    catch (FdoException* e)
    {
        ProcessStylizerException(e, __LINE__, __WFILE__);
    }
}


void SE_ExpressionBase::ParseBooleanExpression(const MdfModel::MdfString& exprstr, SE_Boolean& val, const bool defaultValue)
{
    // set to schema default
    val.expression = NULL;
    val.defValue = defaultValue;

    // process any parameters in the expression
    const wchar_t* defValue = ReplaceParameters(exprstr);

    // if we got back a default parameter value and can parse it
    // then that becomes our new default value
    if (defValue)
    {
        if (_wcsnicmp(defValue, L"true", 5) == 0)
            val.defValue = true;
        else if (_wcsnicmp(defValue, L"false", 6) == 0)
            val.defValue = false;
    }

    // Initially the value is the default
    val.value = val.defValue;

    if (m_buffer.empty())
        return;

    // try to parse the string
    const wchar_t* cstr = m_buffer.c_str();
    if (_wcsnicmp(cstr, L"true", 5) == 0)
    {
        val.value = true;
        return;
    }
    else if (_wcsnicmp(cstr, L"false", 6) == 0)
    {
        val.value = false;
        return;
    }

    // We have an expression - parse it using FDO.  If the parse
    // call fails then the value stays at the default.
    try
    {
        val.expression = FdoExpression::Parse(cstr);
    }
    catch (FdoException* e)
    {
        ProcessStylizerException(e, __LINE__, __WFILE__);
    }
}


// Returns whether the supplied string is a literal of the form 'Value'
// (i.e. it's surrounded by a matching pair of single quotes)
bool IsLiteral(const wchar_t* str)
{
    // string must start with a single quote
    if (*str++ == sExprSingleQuote)
    {
        // make sure we have at least one more character
        if (*str++ == L'\0')
            return false;

        // move to the end of the string
        while (*str != L'\0')
            str++;

        // back up one character
        str--;

        // string must have a matching closing single quote
        if (*str == sExprSingleQuote)
            return true;
    }

    return false;
}


// Copies a string using new.  Note that wcsdup allocates using malloc.
wchar_t* DuplicateString(const wchar_t* str)
{
    size_t len = wcslen(str) + 1;
    wchar_t* copy = new wchar_t[len];
    wcscpy(copy, str);
    return copy;
}


// Returns a copy of the supplied string, with any outer pair of
// matching single quotes removed.
wchar_t* UnquoteLiteral(const wchar_t* str)
{
    if (!IsLiteral(str))
        return DuplicateString(str);

    size_t len = wcslen(str) - 2;
    wchar_t* copy = new wchar_t[len + 1];
    memcpy(copy, str + 1, sizeof(wchar_t)*len);
    copy[len] = L'\0';
    return copy;
}


void SE_ExpressionBase::ParseStringExpression(const MdfModel::MdfString& exprstr, SE_String& val, const wchar_t* defaultValue, const wchar_t* allowedValues)
{
    // set to default - we set the value later on
    val.expression = NULL;

    // process any parameters in the expression
    const wchar_t* defValue = ReplaceParameters(exprstr);
    val.setDefValue(UnquoteLiteral(defValue? defValue : defaultValue));

    if (m_buffer.empty())
        return;  // Nothing more to parse.

    // try to parse the string
    const wchar_t* str = m_buffer.c_str();
    if (IsLiteral(str))
    {
        // set the value to the literal
        size_t len = m_buffer.size() - 2;
        wchar_t* copy = new wchar_t[len + 1];
        memcpy(copy, str + 1, sizeof(wchar_t)*len);
        copy[len] = L'\0';

        // modify val.value directly to avoid doing another copy
        val.setValue(copy);

        return;
    }

    // check if it's one of the allowed constant values
    if (allowedValues && wcsstr(allowedValues, str))
    {
        // found it - set the value to a copy
        val.setValue(DuplicateString(str));

        return;
    }

    // We have an expression - parse it using FDO.  If the parse
    // call fails then the value stays at the default.
    try
    {
        val.expression = FdoExpression::Parse(str);
    }
    catch (FdoException* e)
    {
        ProcessStylizerException(e, __LINE__, __WFILE__);
    }
}
