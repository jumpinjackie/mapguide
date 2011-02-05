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

#include "OgcFramework.h"
#include "NameValueCollection.h"

/////////////////////////////////////////////////////////////////////////
// Empties the collection.
// This is the Base class's default implementation of a virtual function.
// It frees memory allocated by SetCount.
//
// Design note: ALL DERIVATIONS that override the Empty method also need
// to call their empty in their destructor.  The Call to Empty in this
// base class implementation's destructor won't reach you, because the
// vtable will have been undone by then.
//
void MgUtilNameValueCollection::Empty()
{
    if(m_ppszNames != NULL) {
        free(m_ppszNames);
        m_ppszNames = NULL;
    }
    if(m_ppvValues != NULL) {
        free(m_ppvValues);
        m_ppvValues = NULL;
    }
    m_iCount = 0;
}


/////////////////////////////////////////////////////////////////////////
// Gets the i'th name of the collection.
CPSZ MgUtilNameValueCollection::Name(int i)
{
    ASSERT(i>=0 && i<Count()); // Debug-level warning
    if(i>=0 && i<Count())      // Release build safety
        return m_ppszNames[i];
    else
        return NULL;
}


/////////////////////////////////////////////////////////////////////////
// Gets the i'th value of the collection.
void* MgUtilNameValueCollection::Value(int i)
{
    ASSERT(i>=0 && i<Count()); // Debug-level warning
    if(i>=0 && i<Count())      // Release build safety
        return m_ppvValues[i];
    else
        return NULL;
}

/////////////////////////////////////////////////////////////////////////
// Given a candidate name, finds the position of the name/value pair
// that matches the candidate name.  Note that CompareSame is virtual,
// and derived implementations define what it means to compare the same
// (such as case insensitivity, subset or wildcard matching, etc.)
int MgUtilNameValueCollection::FindName(CPSZ pszCandidate)
{
    for(int i=0;i<Count();i++) {
        if(CompareSame(m_ppszNames[i],pszCandidate))
            return i;
    }

    return -1;
}

/////////////////////////////////////////////////////////////////////////
// Allocates the storage used to contain the name/value pairs.  Derivations
// can then hang whatever names and/or values appropriate for their implementation.
bool MgUtilNameValueCollection::SetCount(int i)
{
    // Allocate the storage.
    CPSZ*  ppszNames = (CPSZ *) realloc(m_ppszNames,i * sizeof(VPSZ));
    void** ppvValues = (void **)realloc(m_ppvValues,i * sizeof(void*));

    // If both allocations succeeded, we can move forward
    if(ppszNames != NULL && ppvValues != NULL) {
        m_iCount     = i;
        m_ppszNames  = ppszNames;
        m_ppvValues  = ppvValues;
        return true;
    }
    else { // roll-back.
        // clean up anything that did succeed.
        if(ppszNames != NULL)
            free(ppszNames);
        if(ppvValues != NULL)
            free(ppvValues);
        return false;
    }

}

/////////////////////////////////////////////////////////////////////////
// Mutator, corresponding to Name(i) -- sets the name associated with the ith
// name/value pair.
void MgUtilNameValueCollection::SetName(int i,CPSZ pszName)
{
    ASSERT(i>=0 && i<Count());
    if(i>=0 && i<Count())
        m_ppszNames[i] = pszName;
}

/////////////////////////////////////////////////////////////////////////
// Mutator, corresponding to Value(i) -- sets the value associated with the ith
// name/value pair.
void MgUtilNameValueCollection::SetValue(int i,void* pvValue)
{
    ASSERT(i>=0 && i<Count());
    if(i>=0 && i<Count())
        m_ppvValues[i] = pvValue;
}

/////////////////////////////////////////////////////////////////////////
// Compares a name/value pair's name with a candidate name.
// This is the Base class's default implementation of a virtual function.
// It simply performs a case-sensitive string comparison.
bool MgUtilNameValueCollection::CompareSame(CPSZ pszName,CPSZ pszCandidate)
{
    // default implementation: case sensitive names.
    return SZ_EQ(pszName,pszCandidate);
}
