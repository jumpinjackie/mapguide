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

#ifndef MG_RESOURCE_SERVICE_CACHE_ENTRY_H_
#define MG_RESOURCE_SERVICE_CACHE_ENTRY_H_

#include "ServerCacheEntry.h"
#include "ResourceLayerDefinitionCacheItem.h"

class MG_SERVER_CACHE_API MgResourceServiceCacheEntry : public MgServerCacheEntry
{
/// Constructors/Destructor
public:
    MgResourceServiceCacheEntry(void);
    virtual ~MgResourceServiceCacheEntry(void);

private:

    // Unimplemented copy constructor and assignment operator.
    MgResourceServiceCacheEntry(const MgResourceServiceCacheEntry&);
    MgResourceServiceCacheEntry& operator=(const MgResourceServiceCacheEntry&);

/// Methods

public:
    void SetLayerDefinition(MgResourceLayerDefinitionCacheItem* layerDefinition);
    MgResourceLayerDefinitionCacheItem* GetLayerDefinition();

private:
    Ptr<MgResourceLayerDefinitionCacheItem> m_layerDefinition;
};

#endif
