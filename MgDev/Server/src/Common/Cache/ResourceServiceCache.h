//
//  Copyright (C) 2004-2015 by Autodesk, Inc.
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

#ifndef MG_RESOURCE_SERVICE_CACHE_H_
#define MG_RESOURCE_SERVICE_CACHE_H_

#include "ServerCache.h"
#include "ResourceServiceCacheEntry.h"

class MG_SERVER_CACHE_API MgResourceServiceCache : public MgServerCache
{
    DECLARE_CLASSNAME(MgResourceServiceCache)

/// Constructors/Destructor

public:
    MgResourceServiceCache(void);
    ~MgResourceServiceCache(void);

private:

    // Unimplemented copy constructor and assignment operator.
    MgResourceServiceCache(const MgResourceServiceCache&);
    MgResourceServiceCache& operator=(const MgResourceServiceCache&);

/// Methods

public:

    virtual void Clear();

    void RemoveEntry(CREFSTRING resource);
    void RemoveEntry(MgResourceIdentifier* resource);
    void RemoveExpiredEntries();

    // gets/sets layer definitions
    void SetLayerDefinition(MgResourceIdentifier* resource, MgResourceLayerDefinitionCacheItem* layerDefinition);
    MgResourceLayerDefinitionCacheItem* GetLayerDefinition(MgResourceIdentifier* source);

    INT32 GetCacheSize();
    INT32 GetDroppedEntriesCount();

protected:

    void Compact();

    MgResourceServiceCacheEntry* SetEntry(MgResourceIdentifier* resource);
    MgResourceServiceCacheEntry* GetEntry(MgResourceIdentifier* resource);
    void RemoveOldEntry();

/// Data Members

private:

    friend class MgCacheManager;

    typedef std::map<STRING, MgResourceServiceCacheEntry*> MgResourceServiceCacheEntries;
    MgResourceServiceCacheEntries m_resourceServiceCacheEntries;
    INT32 m_nDroppedEntries;
};

#endif
