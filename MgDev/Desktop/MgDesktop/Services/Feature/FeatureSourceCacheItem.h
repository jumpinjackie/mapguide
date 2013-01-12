#ifndef DESKTOP_FEATURE_SOURCE_CACHE_ITEM_H
#define DESKTOP_FEATURE_SOURCE_CACHE_ITEM_H

class MgdFeatureSourceCacheItem : public MgGuardDisposable
{
    /// Constructors/Destructor

public:

    MgdFeatureSourceCacheItem();
    explicit MgdFeatureSourceCacheItem(MdfModel::FeatureSource* featureSource);
    virtual ~MgdFeatureSourceCacheItem();

private:

    // Unimplemented copy constructor and assignment operator.
    MgdFeatureSourceCacheItem(const MgdFeatureSourceCacheItem&);
    MgdFeatureSourceCacheItem& operator=(const MgdFeatureSourceCacheItem&);

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

inline MdfModel::FeatureSource* MgdFeatureSourceCacheItem::Get()
{
    return m_featureSource.get();
}

#endif