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
#include "DataValuePool.h"
#include "Int64Value.h"
#include "DoubleValue.h"
#include "StringValue.h"
#include "BooleanValue.h"
#include "DateTimeValue.h"


DataValuePool::DataValuePool()
{
}

DataValuePool::~DataValuePool()
{
    while (!m_int64Pool.empty())
        delete m_int64Pool.pop();

    while (!m_doublePool.empty())
        delete m_doublePool.pop();

    while (!m_stringPool.empty())
        delete m_stringPool.pop();

    while (!m_boolPool.empty())
        delete m_boolPool.pop();

    while (!m_datePool.empty())
        delete m_datePool.pop();
}

Int64Value* DataValuePool::ObtainInt64Value(long long val)
{
    //if the pool is empty, create new
    if (m_int64Pool.empty())
        return new Int64Value(val);

    //otherwise get an object from the pool and initialize it
    Int64Value* ret = m_int64Pool.pop();

    ret->Set(val);
    return ret;
}

void DataValuePool::RelinquishInt64Value(Int64Value* dv)
{
    //value is no longer in use, so add to pool
    m_int64Pool.push(dv);
}

DoubleValue* DataValuePool::ObtainDoubleValue(double val)
{
    //if the pool is empty, create new
    if (m_doublePool.empty())
        return new DoubleValue(val);

    //otherwise get an object from the pool and initialize it
    DoubleValue* ret = m_doublePool.pop();

    ret->Set(val);
    return ret;
}


void DataValuePool::RelinquishDoubleValue(DoubleValue* dv)
{
    //value is no longer in use, so add to pool
    m_doublePool.push(dv);
}

StringValue* DataValuePool::ObtainStringValue(wchar_t* val, bool dispose)
{
    //if the pool is empty, create new
    if (m_stringPool.empty())
        return new StringValue(val, dispose);

    //otherwise get an object from the pool and initialize it
    StringValue* ret = m_stringPool.pop();

    ret->Set(val, dispose);
    return ret;
}


void DataValuePool::RelinquishStringValue(StringValue* dv)
{
    //value is no longer in use, so add to pool
    m_stringPool.push(dv);
}

BooleanValue* DataValuePool::ObtainBooleanValue(bool val)
{
    //if the pool is empty, create new
    if (m_boolPool.empty())
        return new BooleanValue(val);

    //otherwise get an object from the pool and initialize it
    BooleanValue* ret = m_boolPool.pop();

    ret->Set(val);
    return ret;
}

void DataValuePool::RelinquishBooleanValue(BooleanValue* dv)
{
    //value is no longer in use, so add to pool
    m_boolPool.push(dv);
}


DateTimeValue* DataValuePool::ObtainDateTimeValue(FdoDateTime val)
{
    //if the pool is empty, create new
    if (m_datePool.empty())
        return new DateTimeValue(val);

    //otherwise get an object from the pool and initialize it
    DateTimeValue* ret = m_datePool.pop();

    ret->Set(val);
    return ret;
}


void DataValuePool::RelinquishDateTimeValue(DateTimeValue* dv)
{
    //value is no longer in use, so add to pool
    m_datePool.push(dv);
}


void DataValuePool::RelinquishDataValue(DataValue* dv)
{
    switch (dv->GetType())
    {
    case Dvt_Int64: RelinquishInt64Value((Int64Value*)dv);
        break;
    case Dvt_Double: RelinquishDoubleValue((DoubleValue*)dv);
        break;
    case Dvt_String: RelinquishStringValue((StringValue*)dv);
        break;
    case Dvt_Boolean: RelinquishBooleanValue((BooleanValue*)dv);
        break;
    case Dvt_DateTime : RelinquishDateTimeValue((DateTimeValue*)dv);
        break;
    default: _ASSERT(false); //should not happen
        break;
    }
}
