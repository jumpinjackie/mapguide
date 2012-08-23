#ifndef DESKTOP_RESOURCE_CONTENT_CACHE_H
#define DESKTOP_RESOURCE_CONTENT_CACHE_H

#include "MgDesktop.h"

class MgResourceContentCache : public MgGuardDisposable
{
private:
    static Ptr<MgResourceContentCache> smInstance;
    MgResourceContentCache();

public:
    static MgResourceContentCache* GetInstance();
    virtual ~MgResourceContentCache();

    STRING GetContentEntry(MgResourceIdentifier* resource);
    void RemoveContentEntry(MgResourceIdentifier* resource);
    void PutContentEntry(MgResourceIdentifier* resource, CREFSTRING content);

protected:
    virtual void Dispose() { delete this; }

private:
    /// Needed for thread-safety
    ACE_Recursive_Thread_Mutex m_mutex;

    typedef std::map<STRING, STRING> MgResourceContentCacheEntries;
    MgResourceContentCacheEntries m_resourceContentCacheEntries;
};

#endif