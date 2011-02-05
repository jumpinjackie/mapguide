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

#ifndef THEME_PARAMETERS_H_
#define THEME_PARAMETERS_H_

#include "Stylization.h"
#include "RendererStyles.h"


//////////////////////////////////////////////////////////////////////////////
class ThemeParameters
{
public:
    STYLIZATION_API static ThemeParameters* Parse(const wchar_t* expressionString);
};


//////////////////////////////////////////////////////////////////////////////
class LookupThemeParameters : public ThemeParameters
{
public:
    STYLIZATION_API LookupThemeParameters(FdoExpressionCollection* arguments);

    STYLIZATION_API const wchar_t* GetKey();
    STYLIZATION_API const wchar_t* GetDefaultValue();

    STYLIZATION_API int NumberOfCategories();
    STYLIZATION_API const wchar_t* CategoryKey(int i);
    STYLIZATION_API const wchar_t* CategoryValue(int i);

private:
    RS_String m_key;
    RS_String m_defaultValue;

    int m_numberOfCategories;
    std::vector<RS_String> m_keys;
    std::vector<RS_String> m_vals;
};


//////////////////////////////////////////////////////////////////////////////
class RangeThemeParameters : public ThemeParameters
{
public:
    STYLIZATION_API RangeThemeParameters(FdoExpressionCollection* arguments);

    STYLIZATION_API const wchar_t* GetKey();
    STYLIZATION_API const wchar_t* GetDefaultValue();

    STYLIZATION_API int NumberOfCategories();
    STYLIZATION_API const wchar_t* CategoryMin(int i);
    STYLIZATION_API const wchar_t* CategoryMax(int i);
    STYLIZATION_API const wchar_t* CategoryValue(int i);

private:
    RS_String m_key;
    RS_String m_defaultValue;

    int m_numberOfCategories;
    std::vector<RS_String> m_mins;
    std::vector<RS_String> m_maxs;
    std::vector<RS_String> m_vals;
};

#endif
