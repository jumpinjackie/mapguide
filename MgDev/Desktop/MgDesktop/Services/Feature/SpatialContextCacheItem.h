#ifndef DESKTOP_SPATIAL_CONTEXT_CACHE_ITEM_H
#define DESKTOP_SPATIAL_CONTEXT_CACHE_ITEM_H


typedef std::map<STRING, STRING> MgSpatialContextInfo;

class MgSpatialContextCacheItem : public MgGuardDisposable
{
/// Constructors/Destructor

public:

    MgSpatialContextCacheItem();
    explicit MgSpatialContextCacheItem(MgSpatialContextInfo* spatialContextInfo);
    virtual ~MgSpatialContextCacheItem();

private:

    // Unimplemented copy constructor and assignment operator.
    MgSpatialContextCacheItem(const MgSpatialContextCacheItem&);
    MgSpatialContextCacheItem& operator=(const MgSpatialContextCacheItem&);

protected:
    virtual void Dispose() { delete this; }

/// Methods

public:

    MgSpatialContextInfo* Get();
    void Set(MgSpatialContextInfo* spatialContextInfo);

/// Data Members

private:

    auto_ptr<MgSpatialContextInfo> m_spatialContextInfo;
};

/// Inline Methods

inline MgSpatialContextInfo* MgSpatialContextCacheItem::Get()
{
    return m_spatialContextInfo.get();
}

#endif