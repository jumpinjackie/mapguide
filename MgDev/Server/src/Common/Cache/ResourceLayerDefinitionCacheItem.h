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

#ifndef MG_RESOURCE_LAYER_DEFINITION_CACHE_ITEM_H_
#define MG_RESOURCE_LAYER_DEFINITION_CACHE_ITEM_H_

#include "ServerCacheItem.h"
#include "LayerDefinition.h"
#include <memory>

using namespace std;

class MG_SERVER_CACHE_API MgResourceLayerDefinitionCacheItem : public MgServerCacheItem
{
/// Constructors/Destructor

public:
    MgResourceLayerDefinitionCacheItem(void);
    explicit MgResourceLayerDefinitionCacheItem(MdfModel::LayerDefinition* LayerDefinition);
    virtual ~MgResourceLayerDefinitionCacheItem(void);

private:

    // Unimplemented copy constructor and assignment operator.
    MgResourceLayerDefinitionCacheItem(const MgResourceLayerDefinitionCacheItem&);
    MgResourceLayerDefinitionCacheItem& operator=(const MgResourceLayerDefinitionCacheItem&);

/// Methods

public:

    MdfModel::LayerDefinition* Get();
    void Set(MdfModel::LayerDefinition* layerDefinition);

/// Data Members

private:

    auto_ptr<MdfModel::LayerDefinition> m_layerDefinition;
};

/// Inline Methods

inline MdfModel::LayerDefinition* MgResourceLayerDefinitionCacheItem::Get()
{
    return m_layerDefinition.get();
}

#endif
