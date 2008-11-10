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
//
// file namestruct.h:  Declaration of TNameStruct template class
//
// Several places in the public API code require working with STL sets, lists, or
// maps of strings, wherein the strings have two important properties:
//
//	1.	They're fixed-length (can't be over a certain fixed size)
//
//	2.	They need to be sorted in a particular way (alphabetically,
//		case-insensitive.
//
// For this reason, instead of trying to work with char *'s that are
// dynamically allocated and freed, it's much more convenient to work
// with structs that have a particular size, and which know how to
// order themselves (by defining operator<, which STL uses for
// sorting).  Thus TNameStruct, which is templatized on a single
// integer parameter which specifies the size, in bytes, of the buffer.
//
// Note that in the public API, only operator< is ever used.  The operator> is
// provided for symmetry, since I couldn't bear the lopsidedness of
// defining only operator<.  I have deliberately unimplemented
// operator==, since STL never uses it and the default implementation
// which the compiler provides is unhelpful and misleading.

#ifndef MG_NAMESTRUCT_H
#define MG_NAMESTRUCT_H
#pragma once

#include <list>
#include <map>
#include "cs_map.h"	//for cs_KEYNM_DEF

//Disable silly warning that comes when we use template stuff
#pragma warning(disable: 4786)

#ifndef _WIN32
//Linux: different naming for string functions
#define stricmp strcasecmp
#ifndef _stricmp
#define _stricmp stricmp
#endif
#endif

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
typedef TNameStruct<cs_KEYNM_DEF> CSystemName;
typedef TNameStruct<64> CSystemDescription;

//Handy typedef for working with sets of CSystemName objects.
typedef std::list<CSystemName> CSystemNameList;
typedef std::map<CSystemName, CSystemDescription> CSystemNameDescriptionMap;
typedef std::pair<CSystemName, CSystemDescription> CSystemNameDescriptionPair;

//Max length of a category "name" (really, description).
//The value chosen here is admittedly arbitrary.  This is also
//a new constraint, as ADE.PRO allowed arbitrarily long category
//names.  However I scanned the ADE.PRO which shipped with Map 3,
//and the longest description therein was only 29 characters, so
//this size should be more than ample.
//
//Note:  Be careful about changing this number!  It determines the
//format of the category dictionary file, so changing this number
//will render the public API unable to access any category dictionaries
//which were created using a different number.
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

#endif //MG_NAMESTRUCT_H


//End of file.
