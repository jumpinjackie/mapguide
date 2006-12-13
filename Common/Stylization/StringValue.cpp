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
#include "StringValue.h"
#include "DataValuePool.h"
#include "locale.h"

const wchar_t* EMPTY_STRING = L"";


StringValue::StringValue(wchar_t* val, bool dispose)
{
    if (val)
    {
        m_value = val;
        m_bDisposeValue = dispose;
    }
    else
    {
        m_value = (wchar_t*)EMPTY_STRING;
        m_bDisposeValue = false;
    }
}

StringValue::~StringValue()
{
    if (m_bDisposeValue)
        delete [] m_value;
}

void StringValue::Set(wchar_t* val, bool dispose)
{
    if (m_bDisposeValue && m_value)
        delete [] m_value;

    if (val)
    {
        m_value = val;
        m_bDisposeValue = dispose;
    }
    else
    {
        m_value = (wchar_t*)EMPTY_STRING;
        m_bDisposeValue = false;
    }
}

DataValue* StringValue::Clone(DataValuePool* pPool)
{
    return pPool->ObtainStringValue(m_value);
}

DataValueType StringValue::GetType()
{
    return Dvt_String;
}

DataValue* StringValue::Negate(DataValuePool* /*pPool*/)
{
    throw FdoException::Create(L"Cannot perform arithmetic operation on string");
}

DataValue* StringValue::Add(DataValue& /*v2*/, DataValuePool* /*pPool*/)
{
    throw FdoException::Create(L"Cannot perform arithmetic operation on string");
}

DataValue* StringValue::Subtract(DataValue& /*v2*/, DataValuePool* /*pPool*/)
{
    throw FdoException::Create(L"Cannot perform arithmetic operation on string");
}

DataValue* StringValue::Multiply(DataValue& /*v2*/, DataValuePool* /*pPool*/)
{
    throw FdoException::Create(L"Cannot perform arithmetic operation on string");
}

DataValue* StringValue::Divide(DataValue& /*v2*/, DataValuePool* /*pPool*/)
{
    throw FdoException::Create(L"Cannot perform arithmetic operation on string");
}

bool StringValue::IsEqualTo(DataValue& v2)
{
    _ASSERT(m_value);

    DataValueType type = v2.GetType();

    //handle "casting" to other data types, in case
    //a property is stored as a string, but user wants to
    //treat it as a number or boolean
    switch (type)
    {
    case Dvt_Int64:
        return GetAsInt64() == v2.GetAsInt64();
    case Dvt_Double:
        return GetAsDouble() == v2.GetAsDouble();
    case Dvt_Boolean:
        return GetAsBoolean() == v2.GetAsBoolean();
    default:
        return wcscmp(m_value, v2.GetAsString()) == 0;
    }
}

bool StringValue::IsNotEqualTo(DataValue& v2)
{
    return !IsEqualTo(v2);
}

bool StringValue::IsGreaterThan(DataValue& v2)
{
    _ASSERT(m_value);

    DataValueType type = v2.GetType();

    //handle "casting" to other data types, in case
    //a property is stored as a string, but user wants to
    //treat it as a number or boolean
    switch (type)
    {
    case Dvt_Int64:
        return GetAsInt64() > v2.GetAsInt64();
    case Dvt_Double:
        return GetAsDouble() > v2.GetAsDouble();
    default:
        return wcscmp(m_value, v2.GetAsString()) > 0;
    }
}

bool StringValue::IsGreaterThanOrEqualTo(DataValue& v2)
{
    return IsGreaterThan(v2) || IsEqualTo(v2);
}

bool StringValue::IsLessThan(DataValue& v2)
{
    return !IsGreaterThanOrEqualTo(v2);
}

bool StringValue::IsLessThanOrEqualTo(DataValue& v2)
{
    return !IsGreaterThan(v2);
}

double StringValue::GetAsDouble()
{
    double d = 0.0;

    _ASSERT(m_value);

    //d = _wtof(m_value);

    //check for a number format that uses comma instead of point
    //for decimal place separator -- it is pretty basic way to do
    //this check, and may not work in all cases...? //TODO
    lconv* lconv_info = localeconv();
    char sep = *lconv_info->decimal_point;

    wchar_t* hasInvalidSeparator = NULL;

    if (sep == '.')
        hasInvalidSeparator = wcschr(m_value, L',');
    else
        hasInvalidSeparator = wcschr(m_value, L'.');

    int ret = 0;

    if (!hasInvalidSeparator)
    {
        ret = swscanf(m_value, L"%lf", &d);
        _ASSERT(ret == 1);
    }
    else
    {
        //case where we will need to switch locale.
        //remember the old one
        char* oldloc = setlocale(LC_NUMERIC, NULL);
        char memoldloc[64];
        _ASSERT(strlen(oldloc) < 63);
        strcpy(memoldloc, oldloc);

        //switch to a locale that will likely do a good job
        if (sep == '.')
        {
            //case where string contains a comma but separator
            //in current locale is .
            //switch to a known comma using locale
            setlocale(LC_NUMERIC, "fr_FR");
        }
        else
        {
            //case where string contains a . but separator in current
            //locale is ,
            setlocale(LC_NUMERIC, "C");
        }

        ret = swscanf(m_value, L"%lf", &d);
        _ASSERT(ret == 1);

        //restore the original locale
        setlocale(LC_NUMERIC, memoldloc);
    }

    return d;
}

long long StringValue::GetAsInt64()
{
    long long n = 0;

    _ASSERT(m_value);

    //n = _wtoi64(m_value);

    int ret = 0;
    ret = swscanf(m_value, L"%ld", &n);
    _ASSERT(ret == 1);

    return n;
}

bool StringValue::GetAsBoolean()
{
    _ASSERT(m_value);

    if (_wcsnicmp(m_value, L"true", 4) == 0)
        return true;

    return false;
}

FdoDateTime StringValue::GetAsDateTime()
{
    throw FdoException::Create(L"Cannot convert string to DateTime");
}


wchar_t* StringValue::GetAsString()
{
    return m_value;
}
