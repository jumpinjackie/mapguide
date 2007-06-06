//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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
#include "Int64Value.h"
#include "DataValuePool.h"
#include "StringValue.h"

Int64Value::Int64Value(long long val)
{
    m_value = val;
    m_strValue = NULL;
}

Int64Value::~Int64Value()
{
    if (m_strValue)
    {
        delete [] m_strValue;
        m_strValue = NULL;
    }
}


void Int64Value::Set(long long val)
{
    m_value = val;

    if (m_strValue)
    {
        //TODO: is %d right for int64?
        swprintf(m_strValue, MAX_STRING_LENGTH-1, L"%d", m_value);
    }
}

DataValue* Int64Value::Clone(DataValuePool* pPool)
{
    return pPool->ObtainInt64Value(m_value);
}

DataValueType Int64Value::GetType()
{
    return Dvt_Int64;
}

DataValue* Int64Value::Negate(DataValuePool* pPool)
{
    return pPool->ObtainInt64Value(-m_value);
}

DataValue* Int64Value::Add(DataValue& v2, DataValuePool* pPool)
{
    //special case
    //support concatenation of strings if second arg is a string
    if (v2.GetType() == Dvt_String)
    {
        wchar_t* sval = GetAsString();
        wchar_t* sval2 = v2.GetAsString();

        size_t len = wcslen(sval) + wcslen(sval2) + 1;

        wchar_t* ret = new wchar_t[len];
        wcscpy(ret, sval);
        wcscat(ret, sval2);

        return pPool->ObtainStringValue(ret, true);
    }

    //generic case
    return pPool->ObtainInt64Value(m_value + v2.GetAsInt64());
}

DataValue* Int64Value::Subtract(DataValue& v2, DataValuePool* pPool)
{
    return pPool->ObtainInt64Value(m_value - v2.GetAsInt64());
}

DataValue* Int64Value::Multiply(DataValue& v2, DataValuePool* pPool)
{
    return pPool->ObtainInt64Value(m_value * v2.GetAsInt64());
}

DataValue* Int64Value::Divide(DataValue& v2, DataValuePool* pPool)
{
    long long divisor = v2.GetAsInt64();

    if (0 == divisor)
    {
        throw FdoException::Create(L"Divide by zero");
    }

    return pPool->ObtainInt64Value(m_value / divisor);
}

bool Int64Value::IsEqualTo(DataValue& v2)
{
    return m_value == v2.GetAsInt64();
}

bool Int64Value::IsNotEqualTo(DataValue&v2)
{
    return m_value != v2.GetAsInt64();
}

bool Int64Value::IsGreaterThan(DataValue&v2)
{
    return m_value > v2.GetAsInt64();
}

bool Int64Value::IsGreaterThanOrEqualTo(DataValue&v2)
{
    return m_value >= v2.GetAsInt64();
}

bool Int64Value::IsLessThan(DataValue&v2)
{
    return m_value < v2.GetAsInt64();
}

bool Int64Value::IsLessThanOrEqualTo(DataValue&v2)
{
    return m_value <= v2.GetAsInt64();
}

double Int64Value::GetAsDouble()
{
    return (double)m_value;
}

long long Int64Value::GetAsInt64()
{
    return m_value;
}

bool Int64Value::GetAsBoolean()
{
    return (m_value != 0);
}

FdoDateTime Int64Value::GetAsDateTime()
{
    throw FdoException::Create(L"Cannot convert Int64 to DateTime");
}

wchar_t* Int64Value::GetAsString()
{
    if (m_strValue == NULL)
    {
        m_strValue = new wchar_t[MAX_STRING_LENGTH];

        //TODO: is %d right for int64?
        swprintf(m_strValue, MAX_STRING_LENGTH-1, L"%d", m_value);
    }

    return m_strValue;
}
