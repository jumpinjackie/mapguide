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
#include "ThemeParameters.h"

static const wchar_t* sEmpty = L"";

extern void ProcessStylizerException(FdoException* exception, int line, wchar_t* file);

//////////////////////////////////////////////////////////////////////////////

ThemeParameters* ThemeParameters::Parse(const wchar_t* expressionString)
{
    // parse the expression and see if it consists of one of our theming functions
    FdoPtr<FdoExpression> expression;
    try
    {
        expression = FdoExpression::Parse(expressionString);
    }
    catch (FdoException* e)
    {
        ProcessStylizerException(e, __LINE__, __WFILE__);
    }

    FdoFunction* function = dynamic_cast<FdoFunction*>(expression.p);
    if (function != NULL)
    {
        FdoString* name = function->GetName();
        FdoPtr<FdoExpressionCollection> exprColl = function->GetArguments();

        if (_wcsicmp(name, L"lookup") == 0)
            return new LookupThemeParameters(exprColl);

        if (_wcsicmp(name, L"range") == 0)
            return new RangeThemeParameters(exprColl);
    }

    // not one of our functions
    return NULL;
}


//////////////////////////////////////////////////////////////////////////////

LookupThemeParameters::LookupThemeParameters(FdoExpressionCollection* arguments)
{
    m_numberOfCategories = 0;

    int numArgs = arguments->GetCount();
    if (numArgs > 0)
    {
        FdoPtr<FdoExpression> exprKey = arguments->GetItem(0);
        m_key = exprKey->ToString();
    }

    if (numArgs > 1)
    {
        FdoPtr<FdoExpression> exprDefVal = arguments->GetItem(1);
        m_defaultValue = exprDefVal->ToString();
    }

    if (numArgs > 2)
    {
        // for now, discard any partial category at the end
        m_numberOfCategories = (numArgs - 2) / 2;

        for (int i=0; i<m_numberOfCategories; ++i)
        {
            FdoPtr<FdoExpression> exprKey = arguments->GetItem(2 + 2*i);
            FdoPtr<FdoExpression> exprVal = arguments->GetItem(3 + 2*i);

            m_keys.push_back(exprKey->ToString());
            m_vals.push_back(exprVal->ToString());
        }
    }
}


const wchar_t* LookupThemeParameters::GetKey()
{
    return m_key.c_str();
}


const wchar_t* LookupThemeParameters::GetDefaultValue()
{
    return m_defaultValue.c_str();
}


int LookupThemeParameters::NumberOfCategories()
{
    return m_numberOfCategories;
}


const wchar_t* LookupThemeParameters::CategoryKey(int i)
{
    if (i >= 0 && i < m_numberOfCategories)
        return m_keys[i].c_str();

    return sEmpty;
}


const wchar_t* LookupThemeParameters::CategoryValue(int i)
{
    if (i >= 0 && i < m_numberOfCategories)
        return m_vals[i].c_str();

    return sEmpty;
}


//////////////////////////////////////////////////////////////////////////////

RangeThemeParameters::RangeThemeParameters(FdoExpressionCollection* arguments)
{
    m_numberOfCategories = 0;

    int numArgs = arguments->GetCount();
    if (numArgs > 0)
    {
        FdoPtr<FdoExpression> exprKey = arguments->GetItem(0);
        m_key = exprKey->ToString();
    }

    if (numArgs > 1)
    {
        FdoPtr<FdoExpression> exprDefVal = arguments->GetItem(1);
        m_defaultValue = exprDefVal->ToString();
    }

    if (numArgs > 2)
    {
        // for now, discard any partial category at the end
        m_numberOfCategories = (numArgs - 2) / 3;

        for (int i=0; i<m_numberOfCategories; ++i)
        {
            FdoPtr<FdoExpression> exprMin = arguments->GetItem(2 + 3*i);
            FdoPtr<FdoExpression> exprMax = arguments->GetItem(3 + 3*i);
            FdoPtr<FdoExpression> exprVal = arguments->GetItem(4 + 3*i);

            m_mins.push_back(exprMin->ToString());
            m_maxs.push_back(exprMax->ToString());
            m_vals.push_back(exprVal->ToString());
        }
    }
}


const wchar_t* RangeThemeParameters::GetKey()
{
    return m_key.c_str();
}


const wchar_t* RangeThemeParameters::GetDefaultValue()
{
    return m_defaultValue.c_str();
}


int RangeThemeParameters::NumberOfCategories()
{
    return m_numberOfCategories;
}


const wchar_t* RangeThemeParameters::CategoryMin(int i)
{
    if (i >= 0 && i < m_numberOfCategories)
        return m_mins[i].c_str();

    return sEmpty;
}


const wchar_t* RangeThemeParameters::CategoryMax(int i)
{
    if (i >= 0 && i < m_numberOfCategories)
        return m_maxs[i].c_str();

    return sEmpty;
}


const wchar_t* RangeThemeParameters::CategoryValue(int i)
{
    if (i >= 0 && i < m_numberOfCategories)
        return m_vals[i].c_str();

    return sEmpty;
}
