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

#ifndef _WKTFAILURECACHE_H_
#define _WKTFAILURECACHE_H_

namespace CSLibrary
{
typedef std::map<STRING, MgException*> CWktExceptionMap;

class CCoordinateSystemWktFailureCache
{
/// Constructors/Destructor

public:

    CCoordinateSystemWktFailureCache();
    virtual ~CCoordinateSystemWktFailureCache();

private:

    // Unimplemented copy constructor and assignment operator.
    CCoordinateSystemWktFailureCache(const CCoordinateSystemWktFailureCache&);
    CCoordinateSystemWktFailureCache& operator=(const CCoordinateSystemWktFailureCache&);

/// Methods
public:

    static CCoordinateSystemWktFailureCache* GetInstance();

    void Clear();

    bool Has (CREFSTRING key);
    MgException* Get(CREFSTRING key);
    void Set(CREFSTRING key, MgException* mgException);

/// Data Members
private:

    static CCoordinateSystemWktFailureCache sm_wktFailureCache;

    ACE_Recursive_Thread_Mutex m_mutex;
    CWktExceptionMap m_wktExceptionMap;
};

} // namespace CSLibrary
#endif
