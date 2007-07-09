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
#include "DateTimeValue.h"
#include "DataValuePool.h"

DateTimeValue::DateTimeValue(FdoDateTime& val)
{
    m_value = val;
    m_strValue = NULL;
}

DateTimeValue::~DateTimeValue()
{
    if (m_strValue)
    {
        delete [] m_strValue;
        m_strValue = NULL;
    }
}

void DateTimeValue::Set(FdoDateTime& val)
{
    m_value = val;

    if (m_strValue)
    {
        FdoPtr<FdoDateTimeValue> dt = FdoDateTimeValue::Create(m_value);

        wcscpy(m_strValue, dt->ToString());
    }
}

DataValue* DateTimeValue::Clone(DataValuePool* pPool)
{
    return pPool->ObtainDateTimeValue(m_value);
}

DataValueType DateTimeValue::GetType()
{
    return Dvt_DateTime;
}

DataValue* DateTimeValue::Negate(DataValuePool* /*pPool*/)
{
    throw FdoException::Create(L"Cannot perform arithmetic on DateTime");
}

DataValue* DateTimeValue::Add(DataValue& /*v2*/, DataValuePool* /*pPool*/)
{
    throw FdoException::Create(L"Cannot perform arithmetic on DateTime");
}

DataValue* DateTimeValue::Subtract(DataValue& /*v2*/, DataValuePool* /*pPool*/)
{
    throw FdoException::Create(L"Cannot perform arithmetic on DateTime");
}

DataValue* DateTimeValue::Multiply(DataValue& /*v2*/, DataValuePool* /*pPool*/)
{
    throw FdoException::Create(L"Cannot perform arithmetic on DateTime");
}

DataValue* DateTimeValue::Divide(DataValue& /*v2*/, DataValuePool* /*pPool*/)
{
    throw FdoException::Create(L"Cannot perform arithmetic on DateTime");
}

bool DateTimeValue::IsEqualTo(DataValue& v2)
{
    FdoDateTime d2 = v2.GetAsDateTime();

    if (   d2.year == m_value.year
        && d2.month == m_value.month
        && d2.day == m_value.day
        && d2.hour == m_value.hour
        && d2.minute == m_value.minute
        && d2.seconds == m_value.seconds)
        return true;

    return false;
}

bool DateTimeValue::IsNotEqualTo(DataValue& v2)
{
    return !IsEqualTo(v2);
}

bool DateTimeValue::IsGreaterThan(DataValue& v2)
{
    FdoDateTime d2 = v2.GetAsDateTime();

    if ((m_value.IsDate() && d2.IsDate()) || (m_value.IsDateTime() && d2.IsDateTime()))
    {
        if (m_value.year > d2.year)
            return true;

        if (m_value.year < d2.year)
            return false;

        //if we are here, year value is equal, test month
        if (m_value.month > d2.month)
            return true;

        if (m_value.month < d2.month)
            return false;

        //months are equal too, test days
        if (m_value.day > d2.day)
            return true;

        if (m_value.day < d2.day)
            return false;
    }

    if ((m_value.IsTime() && d2.IsTime()) || (m_value.IsDateTime() && d2.IsDateTime()))
    {
        //test hour
        if (m_value.hour > d2.hour)
            return true;

        if (m_value.hour < d2.hour)
            return false;

        //hours are equal, test minute
        if (m_value.minute > d2.minute)
            return true;

        if (m_value.minute < d2.minute)
            return false;

        //minutes are equal, test seconds
        if (m_value.seconds > d2.seconds)
            return true;

        if (m_value.seconds < d2.seconds)
            return false;
    }

    return false;
}

bool DateTimeValue::IsGreaterThanOrEqualTo(DataValue& v2)
{
    return IsGreaterThan(v2) || IsEqualTo(v2);
}

bool DateTimeValue::IsLessThan(DataValue& v2)
{
    return !IsGreaterThanOrEqualTo(v2);
}

bool DateTimeValue::IsLessThanOrEqualTo(DataValue& v2)
{
    return !IsGreaterThan(v2);
}

double DateTimeValue::GetAsDouble()
{
    throw FdoException::Create(L"Cannot convert DateTime to double");
}

FdoInt64 DateTimeValue::GetAsInt64()
{
    throw FdoException::Create(L"Cannot convert DateTime to Int64");
}

FdoDateTime DateTimeValue::GetAsDateTime()
{
    return m_value;
}

bool DateTimeValue::GetAsBoolean()
{
    throw FdoException::Create(L"Cannot convert DateTime to boolean");
}

wchar_t* DateTimeValue::GetAsString()
{
    if (m_strValue == NULL)
    {
        m_strValue = new wchar_t[MAX_STRING_LENGTH];

        FdoPtr<FdoDateTimeValue> dt = FdoDateTimeValue::Create(m_value);

        wcscpy(m_strValue, dt->ToString());
    }

    return m_strValue;
}
