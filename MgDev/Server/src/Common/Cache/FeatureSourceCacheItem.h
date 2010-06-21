//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#ifndef MG_FEATURE_SOURCE_CACHE_ITEM_H_
#define MG_FEATURE_SOURCE_CACHE_ITEM_H_

#include "ServerCacheItem.h"
#include "FeatureSource.h"

class MG_SERVER_CACHE_API MgFeatureSourceCacheItem : public MgServerCacheItem
{
/// Constructors/Destructor

public:

    MgFeatureSourceCacheItem();
    explicit MgFeatureSourceCacheItem(MdfModel::FeatureSource* featureSource);
    virtual ~MgFeatureSourceCacheItem();

private:

    // Unimplemented copy constructor and assignment operator.
    MgFeatureSourceCacheItem(const MgFeatureSourceCacheItem&);
    MgFeatureSourceCacheItem& operator=(const MgFeatureSourceCacheItem&);

/// Methods

public:

    MdfModel::FeatureSource* Get();
    void Set(MdfModel::FeatureSource* featureSource);

/// Data Members

private:

    auto_ptr<MdfModel::FeatureSource> m_featureSource;
};

/// Inline Methods

inline MdfModel::FeatureSource* MgFeatureSourceCacheItem::Get()
{
    return m_featureSource.get();
}

#endif
