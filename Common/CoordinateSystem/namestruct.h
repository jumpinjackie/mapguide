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
//
// file namestruct.h:  Declaration of TNameStruct template class
//
// Several places in the public API code require working with STL sets, lists, or
// maps of strings, wherein the strings have two important properties:
//
//  1.  They're fixed-length (can't be over a certain fixed size)
//
//  2.  They need to be sorted in a particular way (alphabetically,
//      case-insensitive.
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
#include <vector>
#include <map>
#include "cs_map.h" //for cs_KEYNM_DEF

//Disable silly warning that comes when we use template stuff
#pragma warning(disable: 4786)

#ifndef _WIN32
//Linux: different naming for string functions
#define stricmp strcasecmp
#ifndef _stricmp
#define _stricmp stricmp
#endif
#endif

#define MAX_STRING_LENGTH 256

struct TNameStruct
{
public:
    // construction / destruction
    TNameStruct(const char *kpName = NULL);
    TNameStruct(const TNameStruct& other);

    ~TNameStruct();

    //comparison
    bool operator<(const TNameStruct& other) const;
    bool operator>(const TNameStruct& other) const;
    bool operator==(const TNameStruct&) const;
    bool operator!=(const TNameStruct&) const;

    //assignment
    TNameStruct& operator=(const TNameStruct&);

    //assignment
    TNameStruct& operator=(const char* newName);

    //misc. methods
    const char* Name() const;
    void Reset();

private:
    //methods
    void Init(const char *kpName = NULL);
    void Release();

private:
    //data members
    char* name;
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
typedef TNameStruct CSystemName;
typedef TNameStruct CSystemDescription;

//Handy typedef for working with sets of CSystemName objects.
typedef std::list<CSystemName> CSystemNameList;
typedef std::map<CSystemName, CSystemDescription> CSystemNameDescriptionMap;
typedef std::pair<CSystemName, CSystemDescription> CSystemNameDescriptionPair;

typedef TNameStruct CCategoryName;
typedef std::map<CCategoryName, long> CCategoryNameIndexMap;
typedef std::vector<CCategoryName> CCategoryNameList;

#endif //MG_NAMESTRUCT_H
