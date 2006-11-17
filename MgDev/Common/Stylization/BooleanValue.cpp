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
#include "BooleanValue.h"
#include "DataValuePool.h"

BooleanValue::BooleanValue(bool val)
{
    m_value = val;
    m_strValue = NULL;
}

BooleanValue::~BooleanValue()
{
    if (m_strValue)
    {
        delete [] m_strValue;
        m_strValue = NULL;
    }
}

void BooleanValue::Set(bool val)
{
    m_value = val;

    if (m_strValue)
    {
        if (m_value)
            swprintf(m_strValue, MAX_STRING_LENGTH-1, L"true");
        else
            swprintf(m_strValue, MAX_STRING_LENGTH-1, L"false");
    }
}

DataValue* BooleanValue::Clone(DataValuePool* pPool)
{
    return pPool->ObtainBooleanValue(m_value);
}

DataValueType BooleanValue::GetType()
{
    return Dvt_Boolean;
}

DataValue* BooleanValue::Negate(DataValuePool* /*pPool*/)
{
    throw FdoException::Create(L"Cannot perform arithmetic operation on boolean");
}


DataValue* BooleanValue::Add(DataValue& /*v2*/, DataValuePool* /*pPool*/)
{
    throw FdoException::Create(L"Cannot perform arithmetic operation on boolean");
}

DataValue* BooleanValue::Subtract(DataValue& /*v2*/, DataValuePool* /*pPool*/)
{
    throw FdoException::Create(L"Cannot perform arithmetic operation on boolean");
}

DataValue* BooleanValue::Multiply(DataValue& /*v2*/, DataValuePool* /*pPool*/)
{
    throw FdoException::Create(L"Cannot perform arithmetic operation on boolean");
}

DataValue* BooleanValue::Divide(DataValue& /*v2*/, DataValuePool* /*pPool*/)
{
    throw FdoException::Create(L"Cannot perform arithmetic operation on boolean");
}

bool BooleanValue::IsEqualTo(DataValue& v2)
{
    return m_value == v2.GetAsBoolean();
}

bool BooleanValue::IsNotEqualTo(DataValue& v2)
{
    _ASSERT(m_value);
    return m_value != v2.GetAsBoolean();
}

bool BooleanValue::IsGreaterThan(DataValue& /*v2*/)
{
    throw FdoException::Create(L"Cannot perform arithmetic comparison on boolean");
}

bool BooleanValue::IsGreaterThanOrEqualTo(DataValue& /*v2*/)
{
    throw FdoException::Create(L"Cannot perform arithmetic comparison on boolean");
}

bool BooleanValue::IsLessThan(DataValue& /*v2*/)
{
    throw FdoException::Create(L"Cannot perform arithmetic comparison on boolean");
}

bool BooleanValue::IsLessThanOrEqualTo(DataValue& /*v2*/)
{
    throw FdoException::Create(L"Cannot perform arithmetic comparison on boolean");
}

double BooleanValue::GetAsDouble()
{
    throw FdoException::Create(L"Cannot convert boolean to double");
}

long long BooleanValue::GetAsInt64()
{
    throw FdoException::Create(L"Cannot convert boolean to Int64");
}

bool BooleanValue::GetAsBoolean()
{
    return m_value;
}

FdoDateTime BooleanValue::GetAsDateTime()
{
    throw FdoException::Create(L"Cannot convert boolean to DateTime");
}

wchar_t* BooleanValue::GetAsString()
{
    if (m_strValue == NULL)
    {
        m_strValue = new wchar_t[6];

        if (m_value)
            swprintf(m_strValue, MAX_STRING_LENGTH-1, L"true");
        else
            swprintf(m_strValue, MAX_STRING_LENGTH-1, L"false");
    }

    return m_strValue;
}
