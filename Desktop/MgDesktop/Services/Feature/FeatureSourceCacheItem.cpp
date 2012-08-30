#include "MgDesktop.h"
#include "FeatureSourceCacheItem.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
MgdFeatureSourceCacheItem::MgdFeatureSourceCacheItem()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
MgdFeatureSourceCacheItem::MgdFeatureSourceCacheItem(MdfModel::FeatureSource* featureSource)
{
    Set(featureSource);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgdFeatureSourceCacheItem::~MgdFeatureSourceCacheItem()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Set the feature source.
///
void MgdFeatureSourceCacheItem::Set(MdfModel::FeatureSource* featureSource)
{
    m_featureSource.reset(featureSource);
}
