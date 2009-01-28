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

#include "OgcFramework.h"
#include "Dictionary.h"

MgUtilDictionary::MgUtilDictionary(MgUtilDictionary* pNextScope)
: m_pNextScope(pNextScope)
{
}

MgUtilDictionary::~MgUtilDictionary()
{
    Empty();
}


void MgUtilDictionary::Empty()
{
    for(int i=0; i<Count(); i++) {
        CPSZ pszName = Name(i);
        if(pszName != NULL) {
            free((void*)pszName);
            SetName(i,NULL);
        }
        void* pvValue = Value(i);
        if(pvValue != NULL) {
            free(pvValue);
            SetValue(i,NULL);
        }
    }
    MgUtilNameStringValueCollection::Empty();
}

bool MgUtilDictionary::AddDefinition(CPSZ pszName,CPSZ pszDefinition)
{
    // Point to duplicates of ourselves, so we can own that memory.
    pszName = szdup(pszName);
    pszDefinition = szdup(pszDefinition);

    bool bSpaceAvailable = true;
    // Let's find out if we're replacing, or growing.
    int iIndex = FindName(pszName);
    if(iIndex == -1) {
        iIndex = Count();
        bSpaceAvailable = SetCount(iIndex+1);
    }
    else { // we're replacing the old definition
        CPSZ pszOldName = Name(iIndex);
        if(pszOldName != NULL)
            free((void*)pszOldName);
        void* pvOldValue = Value(iIndex);
        if(pvOldValue != NULL)
            free(pvOldValue);
    }

    // All successful?
    if(pszName && pszDefinition && bSpaceAvailable) {
        SetName(iIndex,pszName);
        SetValue(iIndex,(void*)pszDefinition);
        return true;
    }
    else { // roll-back incomplete work.
        if(pszName != NULL)
            free((void*)pszName);
        if(pszDefinition != NULL)
            free((void*)pszDefinition);
        return false;
    }
}

bool MgUtilDictionary::AddDefinition(STRING sName,STRING sDefinition)
{
    return AddDefinition(sName.c_str(),sDefinition.c_str());
}


CPSZ MgUtilDictionary::operator[] (CPSZ pszCandidate)
{
    // See if we have the definition (as implemented by our base class)
    CPSZ pszRet = MgUtilNameStringValueCollection::operator[](pszCandidate);

    // If not here, and there's another scope beneath us, try it.
    if(pszRet == NULL && NextScope() != NULL)
        pszRet = NextScope()->operator [](pszCandidate);

    // Either way, return what we learn.
    return pszRet;
}
