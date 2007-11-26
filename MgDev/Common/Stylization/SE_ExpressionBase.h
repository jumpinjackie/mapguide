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

#ifndef SE_EXPRESSIONBASE_H_
#define SE_EXPRESSIONBASE_H_

#include "Stylization.h"
#include "ExpressionHelper.h"
#include <map>
#include <string>


// Parameters used in symbol definitions must be delimited by the '%' character,
// e.g. <LineWeight>%WEIGHT%</LineWeight>.
static const wchar_t sParameterDelimiter = L'%';


//////////////////////////////////////////////////////////////////////////////
struct SE_Color
{
    union
    {
        struct
        {
            unsigned char b, g, r, a; // argb, but little endian
        } comps;
        unsigned int argb;
    } value; //g++ doesn't like nameless structs, otherwise things would be far cleaner

    FdoExpression* expression;

    SE_INLINE SE_Color() : expression(NULL) { value.argb = 0; }
    ~SE_Color() { if (expression) expression->Release(); }

    // Retrieve argb color
    SE_INLINE unsigned int evaluate(FdoExpressionEngine* exec)
    {
        if (expression)
        {
            try
            {
                FdoPtr<FdoLiteralValue> lval = exec->Evaluate(expression);
                value.argb = (unsigned int)ExpressionHelper::GetAsInt32(lval.p);
            }
            catch (FdoException* e)
            {
                e->Release();
            }
        }

        // return the last set value
        return value.argb;
    }

    SE_INLINE bool empty() { return value.argb == 0 && expression == NULL; }
    SE_INLINE void operator=(unsigned int argb) { value.argb = argb; }
};


//////////////////////////////////////////////////////////////////////////////
struct SE_Double
{
    double value;
    FdoExpression* expression;

    SE_INLINE SE_Double() : value(0.0), expression(NULL) { }
    ~SE_Double() { if (expression) expression->Release(); }

    SE_INLINE double evaluate(FdoExpressionEngine* exec)
    {
        if (expression)
        {
            try
            {
                FdoPtr<FdoLiteralValue> lval = exec->Evaluate(expression);
                value = ExpressionHelper::GetAsDouble(lval.p);
            }
            catch (FdoException* e)
            {
                e->Release();
            }
        }

        // return the last set value
        return value;
    }

    SE_INLINE void operator=(double d) { value = d; }
};


//////////////////////////////////////////////////////////////////////////////
struct SE_Integer
{
    int value;
    FdoExpression* expression;

    SE_INLINE SE_Integer() : value(0), expression(NULL) { }
    ~SE_Integer() { if (expression) expression->Release(); }

    SE_INLINE int evaluate(FdoExpressionEngine* exec)
    {
        if (expression)
        {
            try
            {
                FdoPtr<FdoLiteralValue> lval = exec->Evaluate(expression);
                value = ExpressionHelper::GetAsInt32(lval.p);
            }
            catch (FdoException* e)
            {
                e->Release();
            }
        }

        // return the last set value
        return value;
    }

    SE_INLINE void operator=(int i) { value = i; }
};


//////////////////////////////////////////////////////////////////////////////
struct SE_Boolean
{
    bool value;
    FdoExpression* expression;

    SE_INLINE SE_Boolean() : value(false), expression(NULL) { }
    ~SE_Boolean() { if (expression) expression->Release(); }

    SE_INLINE bool evaluate(FdoExpressionEngine* exec)
    {
        if (expression)
        {
            try
            {
                FdoPtr<FdoLiteralValue> lval = exec->Evaluate(expression);
                value = ExpressionHelper::GetAsBoolean(lval.p);
            }
            catch (FdoException* e)
            {
                e->Release();
            }
        }

        // return the last set value
        return value;
    }

    SE_INLINE void operator=(bool b) { value = b; }
};


//////////////////////////////////////////////////////////////////////////////
struct SE_String
{
    wchar_t* value;
    FdoExpression* expression;

    SE_INLINE SE_String() : value(NULL), expression(NULL) { }
    ~SE_String()
    {
        delete[] value;
        if (expression)
            expression->Release();
    }

    SE_INLINE const wchar_t* evaluate(FdoExpressionEngine* exec)
    {
        static const wchar_t* sEmpty = L"";
        if (expression)
        {
            try
            {
                FdoPtr<FdoLiteralValue> lval = exec->Evaluate(expression);
                const wchar_t* newValue = ExpressionHelper::GetAsString(lval.p);
                if (newValue)
                {
                    // the expression was successfully evaluated - update the value
                    delete[] value;
                    size_t len = wcslen(newValue) + 1;
                    value = new wchar_t[len];
                    wcscpy(value, newValue);
                }
            }
            catch (FdoException* e)
            {
                e->Release();
            }
        }

        // return the last set value
        return value? value : sEmpty;
    }

    SE_INLINE void operator=(const wchar_t* s)
    {
        delete[] value;

        if (s)
        {
            size_t len = wcslen(s) + 1;
            wchar_t* copy = new wchar_t[len];
            value = wcscpy(copy, s);
        }
        else
            value = NULL;
    }
};


//////////////////////////////////////////////////////////////////////////////
typedef std::pair<const wchar_t*, const wchar_t*> ParamId;

struct ParamCmpLess : std::binary_function<ParamId&, ParamId&, bool>
{
public:
    bool operator() (const ParamId& a, const ParamId& b) const
    {
        int symcmp = wcscmp(a.first, b.first);
        if (symcmp == 0)
            return wcscmp(a.second, b.second) < 0;

        return symcmp < 0;
    }
};


struct StrCmpLess : std::binary_function<const wchar_t*, const wchar_t*, bool>
{
public:
    bool operator() (const wchar_t* a, const wchar_t* b) const
    {
        return wcscmp(a, b) < 0;
    }
};


//////////////////////////////////////////////////////////////////////////////
typedef std::map<ParamId, const wchar_t*, ParamCmpLess> ParameterMap;
typedef std::map<const wchar_t*, const wchar_t*, StrCmpLess> DefaultMap;

class SE_ExpressionBase
{
public:
    void ParseDoubleExpression(const MdfModel::MdfString& exprstr, SE_Double& val, const double defaultValue);
    void ParseIntegerExpression(const MdfModel::MdfString& exprstr, SE_Integer& val, const int defaultValue);
    void ParseBooleanExpression(const MdfModel::MdfString& exprstr, SE_Boolean& val, const bool defaultValue);
    void ParseStringExpression(const MdfModel::MdfString& exprstr, SE_String& val, const wchar_t* defaultValue);
    void ParseColorExpression(const MdfModel::MdfString& exprstr, SE_Color& val, const unsigned int defaultValue);

    void SetParameterValues(MdfModel::OverrideCollection* overrides);
    void SetDefaultValues(MdfModel::SimpleSymbolDefinition* definition);

private:
    const wchar_t* ReplaceParameters(const MdfModel::MdfString& exprstr);

    ParameterMap m_parameters;
    DefaultMap m_defaults;
    const wchar_t* m_symbol;
    MdfModel::MdfString m_buffer;
    MdfModel::MdfString m_param;
};

#endif
