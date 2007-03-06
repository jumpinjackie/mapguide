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

#ifndef SE_EXPRESSIONBASE_H
#define SE_EXPRESSIONBASE_H

#include <map>
#include <string>

#include "SimpleSymbolDefinition.h"
#include "CompositeSymbolization.h"

struct SE_Double;
struct SE_Integer;
struct SE_Boolean;
struct SE_String;
struct SE_Color;

typedef std::pair<const wchar_t*, const wchar_t*> ParamId;

struct ParamCmpLess : std::binary_function<ParamId&, ParamId&, bool>
{
public:
    bool operator( ) (const ParamId& a, const ParamId& b) const
    {
        int symcmp = _wcsicmp(a.first, b.first);
        if (symcmp == 0)
            return _wcsicmp(a.second, b.second) < 0;
        
        return symcmp < 0;
    }
};

struct StrCmpLess : std::binary_function<const wchar_t*, const wchar_t*, bool>
{
public:
    bool operator( ) (const wchar_t* a, const wchar_t* b) const
    {
        return _wcsicmp(a, b) < 0;
    }
};

typedef std::map<ParamId, const wchar_t*, ParamCmpLess> ParameterMap;
typedef std::map<const wchar_t*, const wchar_t*, StrCmpLess> DefaultMap; 

class SE_ExpressionBase
{
public:
    void ParseDoubleExpression(const MdfModel::MdfString& exprstr, SE_Double& val);
    void ParseIntegerExpression(const MdfModel::MdfString& exprstr, SE_Integer& val);
    void ParseBooleanExpression(const MdfModel::MdfString& exprstr, SE_Boolean& val);
    void ParseStringExpression(const MdfModel::MdfString& exprstr, SE_String& val);
    void ParseColorExpression(const MdfModel::MdfString& exprstr, SE_Color& val);
    
    void SetParameterValues(MdfModel::OverrideCollection* overrides);
    void SetDefaultValues(MdfModel::SimpleSymbolDefinition* definition);

private:
    void ReplaceParameters(const MdfModel::MdfString& exprstr, const wchar_t* fallback);
    
    ParameterMap m_parameters;
    DefaultMap m_defaults;
    const wchar_t* m_symbol;
    MdfModel::MdfString m_buffer;
    MdfModel::MdfString m_param;
};

#endif // SE_EXPRESSIONBASE_H
