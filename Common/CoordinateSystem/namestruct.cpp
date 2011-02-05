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
#include "namestruct.h"

//Constructor.  Initializes to a specified string
//(if one is provided) or to all zeroes (if not).
//
TNameStruct::TNameStruct(const char *kpName)
    : name(NULL)
{
    this->Init(kpName);
}

//Copy constructor - needed as we allocate the [name] field dynamically
//
TNameStruct::TNameStruct(const TNameStruct& other)
    : name(NULL)
{
    this->Init(other.name);
}

//Destructor - releases the allocated memory
//
TNameStruct::~TNameStruct()
{
    this->Release();
}

void TNameStruct::Release()
{
    delete[] name;
    name = NULL;
}

void TNameStruct::Init(const char *kpName)
{
    this->Release();

    size_t stringLength = (NULL == kpName) ? 0 : strnlen(kpName, MAX_STRING_LENGTH);

    //cannot be null; throws std::bad_alloc in case of failure; (what about replacing the new operator?)
    //don't catch it here - if we're not even able to allocate about [MAX_STRING_LENGTH] something very serious is going on here
    name = new char[stringLength + 1];

    if (stringLength > 0) //copy the string into the buffer if we've been given a valid, non-empty string
        strncpy(name, kpName, stringLength);

    name[stringLength] = '\0'; // finally, set the terminating NULL char in any case; the buffer is at least 1 char long
}

//Assignment operator; assign based on other TNameStruct
//
TNameStruct&
TNameStruct::operator=(const TNameStruct& other)
{
    this->Init(other.name);
    return *this;
}

//Assignment operator; assign based on string
TNameStruct&
TNameStruct::operator=(const char* newName)
{
    this->Init(newName);
    return *this;
}

//Comparison operator (alphabetic, case-insensitive).
//
bool
TNameStruct::operator<(const TNameStruct& other) const
{
    return (_stricmp(name, other.name) < 0);
}


//Comparison operator (alphabetic, case-insensitive).
//
bool
TNameStruct::operator>(const TNameStruct& other) const
{
    return (_stricmp(name, other.name) > 0);
}


//Equality operator (alphabetic, case-insensitive).
//
bool
TNameStruct::operator==(const TNameStruct& other) const
{
    return (_stricmp(name, other.name) == 0);
}


//Inequality operator (alphabetic, case-insensitive).
//
bool
TNameStruct::operator!=(const TNameStruct& other) const
{
    return (_stricmp(name, other.name) != 0);
}

//Returns this [TNameStruct] current char* pointer
//
const char*
TNameStruct::Name() const
{
    return this->name;
}
