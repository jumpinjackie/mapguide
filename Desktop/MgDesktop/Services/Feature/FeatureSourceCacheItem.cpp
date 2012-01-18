#include "MgDesktop.h"
#include "FeatureSourceCacheItem.h"

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
MgFeatureSourceCacheItem::MgFeatureSourceCacheItem()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Construct the object.
///
MgFeatureSourceCacheItem::MgFeatureSourceCacheItem(MdfModel::FeatureSource* featureSource)
{
    Set(featureSource);
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Destruct the object.
///
MgFeatureSourceCacheItem::~MgFeatureSourceCacheItem()
{
}

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Set the feature source.
///
void MgFeatureSourceCacheItem::Set(MdfModel::FeatureSource* featureSource)
{
    m_featureSource.reset(featureSource);
}
