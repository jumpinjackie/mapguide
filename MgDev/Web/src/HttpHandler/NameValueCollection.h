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

#ifndef _MgUtilNameValueCollection_h
#define _MgUtilNameValueCollection_h

// Defines an interface for a pool of "name-value pairs" used
// throughout the system.  Derivations impart greater semantics
// (Note: inherited from standalone implementation.)
class MgUtilNameValueCollection
{
protected:
    MgUtilNameValueCollection();
    ~MgUtilNameValueCollection();

public:
    // Empties the collection
    virtual void Empty();

    // How many pairs are in the collection?
    int Count()
    {
        return m_iCount;
    }

    CPSZ Name(int i);
    void* Value(int i);

    int  FindName(CPSZ pszCandidate);

protected:
    bool SetCount(int i);

    void SetName( int i,CPSZ pszName);
    void SetValue(int i,void  * pvValue);

    virtual bool CompareSame(CPSZ pszName,CPSZ pszCandidate);

private:
    CPSZ * m_ppszNames;
    void   ** m_ppvValues;
    int m_iCount;
};


inline MgUtilNameValueCollection::MgUtilNameValueCollection()
: m_ppszNames(NULL)
, m_ppvValues(NULL)
, m_iCount(0)
{
}

inline MgUtilNameValueCollection::~MgUtilNameValueCollection()
{
    Empty();
}

#endif//_MgUtilNameValueCollection_h
