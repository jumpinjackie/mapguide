//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

#include "Foundation.h"

/////////////////////////////////////////////////////////////////
/// <summary>
/// Constructor.  Initializes an empty property
/// </summary>
MgNullableProperty::MgNullableProperty()
{
    m_bIsNull = false;
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor.  Deletes property and any contained information
/// </summary>
MgNullableProperty::~MgNullableProperty()
{
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Set whether this property has no value. In other words, it contains
/// NULL value.
/// </summary>
/// <param name="bIsNull">
/// Specify TRUE or FALSE whether this property value is NULL
/// </param>
void MgNullableProperty::SetNull(bool bIsNull)
{
    m_bIsNull = bIsNull;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Determines whether property has null value
/// </summary>
/// <returns>
/// TRUE - if property is null
/// FALSE - if property has a value
/// </returns>
bool MgNullableProperty::IsNull()
{
    return m_bIsNull;
}

void MgNullableProperty::CheckNull()
{
    if (this->IsNull())
    {
        MgStringCollection arguments;
        arguments.Add(this->GetName());

        throw new MgNullPropertyValueException(L"MgNullableProperty.CheckNull",
            __LINE__, __WFILE__, &arguments, L"", NULL);
    }
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Serialize data to TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>
void MgNullableProperty::Serialize(MgStream* stream)
{
    stream->WriteString(GetName());
    stream->WriteBoolean(m_bIsNull);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserialize data from TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>
void MgNullableProperty::Deserialize(MgStream* stream)
{
    STRING str;

    stream->GetString(str);
    SetName(str);

    stream->GetBoolean(m_bIsNull);
}
