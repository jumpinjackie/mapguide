#ifndef DESKTOP_RESOURCE_CONTENT_CACHE_H
#define DESKTOP_RESOURCE_CONTENT_CACHE_H

#include "MgDesktop.h"

class MgdResourceContentCache : public MgGuardDisposable
{
private:
    static Ptr<MgdResourceContentCache> smInstance;
    MgdResourceContentCache();

public:
    static MgdResourceContentCache* GetInstance();
    virtual ~MgdResourceContentCache();

    STRING GetContentEntry(MgResourceIdentifier* resource);
    void RemoveContentEntry(MgResourceIdentifier* resource);
    void PutContentEntry(MgResourceIdentifier* resource, CREFSTRING content);

protected:
    virtual void Dispose() { delete this; }

private:
    /// Needed for thread-safety
    ACE_Recursive_Thread_Mutex m_MgdMutex;

    typedef std::map<STRING, STRING> MgdResourceContentCacheEntries;
    MgdResourceContentCacheEntries m_resourceContentCacheEntries;
};

#endif