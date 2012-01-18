#include "MgDesktop.h"
#include "SpatialContextCacheItem.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
MgSpatialContextCacheItem::MgSpatialContextCacheItem()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
MgSpatialContextCacheItem::MgSpatialContextCacheItem(MgSpatialContextInfo* spatialContextInfo)
{
    Set(spatialContextInfo);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgSpatialContextCacheItem::~MgSpatialContextCacheItem()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Set the spatial context info.
///
void MgSpatialContextCacheItem::Set(MgSpatialContextInfo* spatialContextInfo)
{
    m_spatialContextInfo.reset(spatialContextInfo);
}
