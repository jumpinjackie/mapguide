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

#include "DataValue.h"

class DoubleValue : public DataValue
{

//let only the DataValuePool create and destroy DataValues
friend class DataValuePool;
private:
    DoubleValue(double val);
    virtual ~DoubleValue();

public:
    void Set(double val);

    DataValue* Clone(DataValuePool* pPool);
    DataValue* Add(DataValue& v2, DataValuePool* pPool);
    DataValue* Subtract(DataValue& v2, DataValuePool* pPool);
    DataValue* Multiply(DataValue& v2, DataValuePool* pPool);
    DataValue* Divide(DataValue& v2, DataValuePool* pPool);
    DataValue* Negate(DataValuePool* pPool);

    bool IsEqualTo(DataValue&v2);
    bool IsNotEqualTo(DataValue&v2);
    bool IsGreaterThan(DataValue&v2);
    bool IsGreaterThanOrEqualTo(DataValue&v2);
    bool IsLessThan(DataValue&v2);
    bool IsLessThanOrEqualTo(DataValue&v2);

    double GetAsDouble();
    long long GetAsInt64();
    wchar_t* GetAsString();
    bool GetAsBoolean();
    FdoDateTime GetAsDateTime();

    DataValueType GetType();

private:
    double m_value;
    wchar_t* m_strValue;
};
