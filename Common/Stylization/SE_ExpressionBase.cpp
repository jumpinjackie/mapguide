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
#include "SE_ExpressionBase.h"
#include "StylizationEngine.h"

#include "SimpleSymbolDefinition.h"

#include <wctype.h>

using namespace MDFMODEL_NAMESPACE;

void SE_ExpressionBase::SetParameterValues(OverrideCollection* overrides)
{
    m_parameters.clear();
    int length = overrides->GetCount();
    for (int i = 0; i < length; i++)
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
    for (int i = 0; i < length; i++)
    {
        Parameter* param = parameters->GetAt(i);
        const wchar_t* name = param->GetIdentifier().c_str();
        const wchar_t* value = param->GetDefaultValue().c_str();
        m_defaults[name] = value;
    }
}

void SE_ExpressionBase::ReplaceParameters(const MdfModel::MdfString& exprstr, const wchar_t* fallback)
{
    MdfString::size_type startIdx, endIdx, count;
    MdfString::iterator beginIter;
    const wchar_t *name, *value;

    m_buffer.assign(exprstr);
    startIdx = endIdx = 0;

    for(;;)
    {
        startIdx = m_buffer.find(L'%', startIdx);
        if (startIdx == MdfString::npos)
            break;

        endIdx = m_buffer.find(L'%', startIdx + 1);
        if (endIdx == MdfString::npos)
            break;

        count = endIdx - startIdx - 1;
        m_param.assign(m_buffer, startIdx + 1, count);
        name = m_param.c_str();

        /* If there is a space in the parameter name, this is not a parameter.  This way,
         * if we add a modulus operator or something, parsing will not be completely screwed up,
         * although we should probably check for a few more illegal characters as well (checking
         * for legal characters doesn't seem viable in the face of localization) */
        bool notParam = false;
        for (size_t i = 0; i < count; i++)
            if (iswspace(name[i]))
                notParam = true;
        if (notParam)
        {
            startIdx = endIdx;
            continue;
        }

        ParameterMap::const_iterator paramIter = m_parameters.find(ParamId(m_symbol, name));

        if (paramIter == m_parameters.end())
        {
            DefaultMap::const_iterator defIter = m_defaults.find(name);
            if (defIter == m_defaults.end())
                value = fallback;
            else
                value = (*defIter).second;
        }
        else
            value = (*paramIter).second;

        beginIter = m_buffer.begin();
        m_buffer.replace(beginIter + startIdx, beginIter + endIdx + 1, value);
        startIdx += wcslen(value);
    }
}

void SE_ExpressionBase::ParseDoubleExpression(const MdfModel::MdfString& exprstr, SE_Double& val)
{
    ReplaceParameters(exprstr, L"0.0");
    const wchar_t* cstr = m_buffer.c_str();
    size_t len = m_buffer.size();
    size_t chars = 0;
    val.expression = NULL;

    if (len == 0)
    {
        val.value = 0;
        return;
    }

    int ret = swscanf(cstr, L"%lf%n", &val.value, &chars);

    if (ret == 1 && chars == len)
        return;

    val.expression = FdoExpression::Parse(cstr);
}

void SE_ExpressionBase::ParseIntegerExpression(const MdfModel::MdfString& exprstr, SE_Integer& val)
{
    ReplaceParameters(exprstr, L"0");
    const wchar_t* cstr = m_buffer.c_str();
    size_t len = m_buffer.size();
    size_t chars = 0;
    val.expression = NULL;

    if (len == 0)
    {
        val.value = 0;
        return;
    }

    int ret = swscanf(cstr, L"%d%n", &val.value, &chars);

    if (ret == 1 && chars == len)
        return;

    val.expression = FdoExpression::Parse(cstr);
}

void SE_ExpressionBase::ParseBooleanExpression(const MdfModel::MdfString& exprstr, SE_Boolean& val)
{
    ReplaceParameters(exprstr, L"false");
    const wchar_t* cstr = m_buffer.c_str();
    size_t len = m_buffer.size();
    val.expression = NULL;

    if (len == 0)
    {
        val.value = false;
        return;
    }
    
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

    val.expression = FdoExpression::Parse(cstr);
}

void SE_ExpressionBase::ParseStringExpression(const MdfModel::MdfString& exprstr, SE_String& val)
{
    ReplaceParameters(exprstr, L"");

    //TODO: we need to add Parse functions that know what the string they are parsing represents.
    //for example ParseHorizontalAlignment. This way we can determine if the value is a constant
    //or an expression that needs to be evaluated all the time.
    if (exprstr.empty())
    {
        val.expression = NULL;
        val.value = L"";
    }
    else
    {
        const wchar_t* str = m_buffer.c_str();
        while (isspace(*str)) 
            str++;
        if (*str++ != '\'')
        {
            val.expression = FdoExpression::Parse(m_buffer.c_str());
            return;
        }

        const wchar_t* begin = str;

        while(*str != NULL && *str != '\'')
            str++;

        if (*str == NULL)
        {
            val.expression = FdoExpression::Parse(m_buffer.c_str());
            return;
        }

        size_t len = str - begin;
        wchar_t* copy = new wchar_t[len + 1];
        memcpy(copy, begin, sizeof(wchar_t)*len);
        copy[len] = L'\0';
        val.value = copy;
    }
}

void SE_ExpressionBase::ParseColorExpression(const MdfModel::MdfString& exprstr, SE_Color& val)
{
    ReplaceParameters(exprstr, L"FF000000");
    const wchar_t* cstr = m_buffer.c_str();
    size_t len = m_buffer.size();
    size_t chars = 0;
    val.expression = NULL;

    if (len == 0)
    {
        val = 0;
        return;
    }

    int ret = swscanf(cstr, L"%X%n", &val.argb(), &chars);

    if (ret == 1 && chars == len)
    {
        return;
    }
    
    val.expression = FdoExpression::Parse(cstr);
}
