//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef _CCOORDINATESYSTEMCOMMON_H_
#define _CCOORDINATESYSTEMCOMMON_H_

#include <limits>
#include <map>
#include <string>
#include <vector>
using namespace std;

typedef wstring STRING;
typedef wstring& REFSTRING;
typedef const wstring& CREFSTRING;
typedef vector<STRING> StringVector;

// Wide character version of __FILE__ macro
#define WIDEN2(x) L ## x
#define WIDEN(x) WIDEN2(x)
#define __WFILE__ WIDEN(__FILE__)

#ifndef _WIN32
//Linux: different naming for string functions
#define _stricmp strcasecmp
#define _wcsicmp  wcscasecmp
#endif

#include <list>

template<int nSize>
struct TNameStruct
{
    //data members
    char name[nSize];
    //member functions
    TNameStruct(const char *kpName = NULL);
    bool operator<(const TNameStruct& other) const;
    bool operator>(const TNameStruct& other) const;
    bool operator==(const TNameStruct&) const;
    bool operator!=(const TNameStruct&) const;
};

//This struct holds a summary of a definition (just name and description,
//not all the other baggage which Mentor structs drag along).  The
//size of the arrays unfortunately must be hard-coded, since Mentor
//doesn't provide named constants.  (An alternative to hard-coded
//static arrays would have been character pointers that are dynamically
//allocated at run time.  However, a survey of the current state of the
//coordinate system dictionary shows that that wouldn't save much space;
//description strings take up, on average, around 2/3 of the space available
//for them, so the savings wouldn't be much.
typedef TNameStruct<24> CSystemName;
typedef TNameStruct<64> CSystemDescription;

//Handy typedef for working with sets of CSystemName objects.
typedef std::list<CSystemName> CSystemNameList;
typedef std::map<CSystemName, CSystemDescription> CSystemNameDescriptionMap;
typedef std::pair<CSystemName, CSystemDescription> CSystemNameDescriptionPair;

const int knMaxCategoryNameLen = 128;

typedef TNameStruct<knMaxCategoryNameLen> CCategoryName;
typedef std::map<CCategoryName, long> CCategoryNameIndexMap;
typedef std::list<CCategoryName> CCategoryNameList;

//Constructor.  Initializes to a specified string
//(if one is provided) or to all zeroes (if not).
//
template<int nSize>
TNameStruct<nSize>::TNameStruct(const char *kpName)
{
    if (NULL == kpName)
    {
        memset(name, 0, sizeof(name));
    }
    else
    {
        strncpy(name, kpName, nSize);
        name[nSize-1] = '\0';
    }
}


//Comparison operator (alphabetic, case-insensitive).
//
template<int nSize>
bool
TNameStruct<nSize>::operator<(const TNameStruct& other) const
{
    return (_stricmp(name, other.name) < 0);
}


//Comparison operator (alphabetic, case-insensitive).
//
template<int nSize>
bool
TNameStruct<nSize>::operator>(const TNameStruct& other) const
{
    return (_stricmp(name, other.name) > 0);
}


//Equality operator (alphabetic, case-insensitive).
//
template<int nSize>
bool
TNameStruct<nSize>::operator==(const TNameStruct& other) const
{
    return (_stricmp(name, other.name) == 0);
}


//Inequality operator (alphabetic, case-insensitive).
//
template<int nSize>
bool
TNameStruct<nSize>::operator!=(const TNameStruct& other) const
{
    return (_stricmp(name, other.name) != 0);
}

#endif //_CCOORDINATESYSTEMCOMMON_H_
