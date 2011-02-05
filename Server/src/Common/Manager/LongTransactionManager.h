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

#ifndef MGLONGTRANSACTIONMANAGER_H_
#define MGLONGTRANSACTIONMANAGER_H_

typedef struct {
    STRING featureSource;   // feature source
    STRING ltName;          // long transaction name
} LongTransactionNameCacheEntry;

typedef std::multimap<STRING, LongTransactionNameCacheEntry*> LongTransactionNameCache;
typedef std::pair<STRING, LongTransactionNameCacheEntry*> LongTransactionNameCache_Pair;

class MG_SERVER_MANAGER_API MgLongTransactionManager
{
    DECLARE_CLASSNAME(MgLongTransactionManager)

/// Constructors/Destructor

public:
    MgLongTransactionManager();
    virtual ~MgLongTransactionManager();

private:
    // Unimplemented copy constructor and assignment operator.
    MgLongTransactionManager(const MgLongTransactionManager&);
    MgLongTransactionManager& operator=(const MgLongTransactionManager&);

/// Methods

public:
    static bool GetLongTransactionName(MgResourceIdentifier* featureSourceId, REFSTRING longTransactionName);
    static bool GetLongTransactionName(CREFSTRING sessionId, MgResourceIdentifier* featureSourceId, REFSTRING longTransactionName);
    static void SetLongTransactionName(CREFSTRING sessionId, MgResourceIdentifier* featureSourceId, CREFSTRING longTransactionName);
    static void RemoveLongTransactionNames(CREFSTRING sessionId);
    static void RemoveLongTransactionNames(MgStringCollection* expiredSessions);
    static void ClearCache();
    static void Terminate();

private:
    static LongTransactionNameCacheEntry* FindEntry(CREFSTRING sessionId, CREFSTRING resource);

/// Data Members

private:
    static ACE_Recursive_Thread_Mutex sm_mutex;
    static LongTransactionNameCache s_LongTransactionNameCache;
};

#endif
