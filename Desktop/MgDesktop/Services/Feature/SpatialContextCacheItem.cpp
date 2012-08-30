#include "MgDesktop.h"
#include "SpatialContextCacheItem.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
MgdSpatialContextCacheItem::MgdSpatialContextCacheItem()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
MgdSpatialContextCacheItem::MgdSpatialContextCacheItem(MgSpatialContextInfo* spatialContextInfo)
{
    Set(spatialContextInfo);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgdSpatialContextCacheItem::~MgdSpatialContextCacheItem()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Set the spatial context info.
///
void MgdSpatialContextCacheItem::Set(MgSpatialContextInfo* spatialContextInfo)
{
    m_spatialContextInfo.reset(spatialContextInfo);
}
