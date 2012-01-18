#ifndef DESKTOP_FEATURE_SOURCE_CACHE_ITEM_H
#define DESKTOP_FEATURE_SOURCE_CACHE_ITEM_H

class MgFeatureSourceCacheItem : public MgGuardDisposable
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

protected:
    virtual void Dispose() { delete this; }

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