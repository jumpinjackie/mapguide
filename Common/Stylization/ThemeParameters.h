/*
 * Copyright (C) 2007-2010 by Autodesk, Inc. All Rights Reserved.
 *
 * By using this code, you are agreeing to the terms and conditions of
 * the License Agreement included in the documentation for this code.
 *
 * AUTODESK MAKES NO WARRANTIES, EXPRESS OR IMPLIED, AS TO THE
 * CORRECTNESS OF THIS CODE OR ANY DERIVATIVE WORKS WHICH INCORPORATE
 * IT. AUTODESK PROVIDES THE CODE ON AN "AS-IS" BASIS AND EXPLICITLY
 * DISCLAIMS ANY LIABILITY, INCLUDING CONSEQUENTIAL AND INCIDENTAL
 * DAMAGES FOR ERRORS, OMISSIONS, AND OTHER PROBLEMS IN THE CODE.
 *
 * Use, duplication, or disclosure by the U.S. Government is subject
 * to restrictions set forth in FAR 52.227-19 (Commercial Computer
 * Software Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
 * (Rights in Technical Data and Computer Software), as applicable.
 */

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
