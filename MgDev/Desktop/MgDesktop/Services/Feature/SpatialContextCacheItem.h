#ifndef DESKTOP_SPATIAL_CONTEXT_CACHE_ITEM_H
#define DESKTOP_SPATIAL_CONTEXT_CACHE_ITEM_H


typedef std::map<STRING, STRING> MgSpatialContextInfo;

class MgdSpatialContextCacheItem : public MgGuardDisposable
{
/// Constructors/Destructor

public:

    MgdSpatialContextCacheItem();
    explicit MgdSpatialContextCacheItem(MgSpatialContextInfo* spatialContextInfo);
    virtual ~MgdSpatialContextCacheItem();

private:

    // Unimplemented copy constructor and assignment operator.
    MgdSpatialContextCacheItem(const MgdSpatialContextCacheItem&);
    MgdSpatialContextCacheItem& operator=(const MgdSpatialContextCacheItem&);

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

inline MgSpatialContextInfo* MgdSpatialContextCacheItem::Get()
{
    return m_spatialContextInfo.get();
}

#endif