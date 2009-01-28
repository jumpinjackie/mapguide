//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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

#include "HttpHandler.h"

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor. Create a new MgHttpPrimitiveValue object initialized
/// with a boolean value
/// </summary>
MgHttpPrimitiveValue::MgHttpPrimitiveValue(bool value)
{
    m_value.b = value;
    m_type = MgHttpPrimitiveValueTypes::HTTP_PRIMITIVE_TYPE_BOOL;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor. Create a new MgHttpPrimitiveValue object initialized
/// with an integer value
/// </summary>
MgHttpPrimitiveValue::MgHttpPrimitiveValue(INT32 value)
{
    m_value.i = value;
    m_type = MgHttpPrimitiveValueTypes::HTTP_PRIMITIVE_TYPE_INT;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor. Create a new MgHttpPrimitiveValue object initialized
/// with a string value
/// </summary>
MgHttpPrimitiveValue::MgHttpPrimitiveValue(CREFSTRING value)
{
    if(!(m_value.pstr = new STRING(value)))
        throw new MgOutOfMemoryException(L"MgHttpPrimitiveValue", __LINE__, __WFILE__, NULL, L"", NULL);
    m_type = MgHttpPrimitiveValueTypes::HTTP_PRIMITIVE_TYPE_STRING;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor. This will clean up the value.
/// </summary>
MgHttpPrimitiveValue::~MgHttpPrimitiveValue()
{
    if(m_type == MgHttpPrimitiveValueTypes::HTTP_PRIMITIVE_TYPE_STRING)
        delete m_value.pstr;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Get the type of the data held by this object
/// </summary>
int MgHttpPrimitiveValue::GetType()
{
    return m_type;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Get the boolean primitive value
/// </summary>
bool MgHttpPrimitiveValue::GetBoolValue()
{
    return m_value.b;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Get the integer primitive value
/// </summary>
INT32 MgHttpPrimitiveValue::GetIntegerValue()
{
    return m_value.i;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Get the string primitive value
/// </summary>
STRING MgHttpPrimitiveValue::GetStringValue()
{
    return *m_value.pstr;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Get a string representation of the primitive value
/// </summary>
STRING MgHttpPrimitiveValue::ToString()
{

    switch(m_type)
    {
        case MgHttpPrimitiveValueTypes::HTTP_PRIMITIVE_TYPE_BOOL:
            return m_value.b? L"True": L"False";

        case MgHttpPrimitiveValueTypes::HTTP_PRIMITIVE_TYPE_INT:
        {
            wchar_t buf[30];
            swprintf(buf, 30, L"%d", m_value.i);
            return buf;
        }
        case MgHttpPrimitiveValueTypes::HTTP_PRIMITIVE_TYPE_STRING:
            return *m_value.pstr;
    }

    assert(false);
    return L"";
}

void MgHttpPrimitiveValue::Dispose()
{
    delete this;
}

INT32 MgHttpPrimitiveValue::GetClassId()
{
    return m_cls_id;
}
