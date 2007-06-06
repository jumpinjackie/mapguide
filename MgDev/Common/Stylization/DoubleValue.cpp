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
#include "DoubleValue.h"
#include "DataValuePool.h"
#include "StringValue.h"

DoubleValue::DoubleValue(double val)
{
    m_value = val;
    m_strValue = NULL;
}

DoubleValue::~DoubleValue()
{
    if (m_strValue)
    {
        delete [] m_strValue;
        m_strValue = NULL;
    }
}

void DoubleValue::Set(double val)
{
    m_value = val;

    if (m_strValue)
    {
        swprintf(m_strValue, MAX_STRING_LENGTH-1, L"%lf", m_value);
    }
}

DataValue* DoubleValue::Clone(DataValuePool* pPool)
{
    return pPool->ObtainDoubleValue(m_value);
}

DataValueType DoubleValue::GetType()
{
    return Dvt_Double;
}


DataValue* DoubleValue::Negate(DataValuePool* pPool)
{
    return pPool->ObtainDoubleValue(-m_value);
}

DataValue* DoubleValue::Add(DataValue& v2, DataValuePool* pPool)
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
    return pPool->ObtainDoubleValue(m_value + v2.GetAsDouble());
}

DataValue* DoubleValue::Subtract(DataValue&v2, DataValuePool* pPool)
{
    return pPool->ObtainDoubleValue(m_value - v2.GetAsDouble());
}

DataValue* DoubleValue::Multiply(DataValue&v2, DataValuePool* pPool)
{
    return pPool->ObtainDoubleValue(m_value * v2.GetAsDouble());
}

DataValue* DoubleValue::Divide(DataValue&v2, DataValuePool* pPool)
{
    double divisor = v2.GetAsDouble();

    if (0.0 == divisor)
    {
        throw FdoException::Create(L"Divide by zero");
    }

    return pPool->ObtainDoubleValue(m_value / divisor);
}

bool DoubleValue::IsEqualTo(DataValue&v2)
{
    return m_value == v2.GetAsDouble();
}

bool DoubleValue::IsNotEqualTo(DataValue&v2)
{
    return m_value != v2.GetAsDouble();
}

bool DoubleValue::IsGreaterThan(DataValue&v2)
{
    return m_value > v2.GetAsDouble();
}

bool DoubleValue::IsGreaterThanOrEqualTo(DataValue&v2)
{
    return m_value >= v2.GetAsDouble();
}

bool DoubleValue::IsLessThan(DataValue&v2)
{
    return m_value < v2.GetAsDouble();
}

bool DoubleValue::IsLessThanOrEqualTo(DataValue&v2)
{
    return m_value <= v2.GetAsDouble();
}

double DoubleValue::GetAsDouble()
{
    return m_value;
}

long long DoubleValue::GetAsInt64()
{
    return (long long) m_value;
}

bool DoubleValue::GetAsBoolean()
{
    return (m_value != 0.0);
}

FdoDateTime DoubleValue::GetAsDateTime()
{
    throw FdoException::Create(L"Cannot convert double to DateTime");
}

wchar_t* DoubleValue::GetAsString()
{
    if (m_strValue == NULL)
    {
        m_strValue = new wchar_t[MAX_STRING_LENGTH];

        swprintf(m_strValue, MAX_STRING_LENGTH-1, L"%lf", m_value);
    }

    return m_strValue;
}
