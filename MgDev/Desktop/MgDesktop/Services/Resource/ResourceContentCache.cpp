#include "MgDesktop.h"
#include "ResourceContentCache.h"

Ptr<MgResourceContentCache> MgResourceContentCache::smInstance = (MgResourceContentCache*)NULL;

MgResourceContentCache::MgResourceContentCache() { }

MgResourceContentCache::~MgResourceContentCache()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgResourceContentCache::~MgResourceContentCache()\n")));
}

MgResourceContentCache* MgResourceContentCache::GetInstance()
{
    if (NULL == MgResourceContentCache::smInstance)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance (), 0));
        if (NULL == MgResourceContentCache::smInstance)
        {
            MgResourceContentCache::smInstance = new MgResourceContentCache();
        }
    }
    return smInstance;
}

STRING MgResourceContentCache::GetContentEntry(MgResourceIdentifier* resource)
{
    CHECKARGUMENTNULL(resource, L"MgResourceContentCache::PutContentEntry");
    STRING resId = resource->ToString();

    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex, L""));
    MgResourceContentCacheEntries::iterator i = m_resourceContentCacheEntries.find(resId);

    STRING ret;
    if (m_resourceContentCacheEntries.end() != i)
    {
        ret = i->second;
    }
    return ret;
}

void MgResourceContentCache::RemoveContentEntry(MgResourceIdentifier* resource)
{
    CHECKARGUMENTNULL(resource, L"MgResourceContentCache::PutContentEntry");
    STRING resId = resource->ToString();

    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));
    MgResourceContentCacheEntries::iterator i = m_resourceContentCacheEntries.find(resId);

    if (m_resourceContentCacheEntries.end() != i)
    {
        m_resourceContentCacheEntries.erase(i);
    }
}

void MgResourceContentCache::PutContentEntry(MgResourceIdentifier* resource, CREFSTRING content)
{
    CHECKARGUMENTNULL(resource, L"MgResourceContentCache::PutContentEntry");
    if (content.empty())
    {
        throw new MgInvalidArgumentException(L"MgResourceContentCache::PutContentEntry", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    STRING resId = resource->ToString();
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgResourceContentCache::PutContentEntry - %W\n"), resId.c_str()));
    
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_mutex));
    m_resourceContentCacheEntries[resId] = content;
}