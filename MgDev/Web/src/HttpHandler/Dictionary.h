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

#ifndef _Dictionary_h
#define _Dictionary_h

#include "NameStringValueCollection.h"

// A Nested Dictionary, where if a definition is not found
// at a given scope, the next scope can be examined.
class MgUtilDictionary: public MgUtilNameStringValueCollection
{
public:
    MgUtilDictionary(MgUtilDictionary* pNextScope);
    ~MgUtilDictionary();

    void Empty();

    // May replace an existing definition, if the names compare same.
    bool AddDefinition(CPSZ pszName,CPSZ pszDefinition);
    bool AddDefinition(STRING sName,STRING sDefinition);

    // Virtual override; if not found in this scope, then
    // the next scope is examined.
    CPSZ operator[] (CPSZ pszCandidate);

    MgUtilDictionary* NextScope();

private:
    MgUtilDictionary* m_pNextScope;
};


inline MgUtilDictionary* MgUtilDictionary::NextScope()
{
    return m_pNextScope;
}

#endif//_Dictionary_h
