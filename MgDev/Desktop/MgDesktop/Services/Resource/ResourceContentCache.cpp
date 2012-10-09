#include "MgDesktop.h"
#include "ResourceContentCache.h"

Ptr<MgdResourceContentCache> MgdResourceContentCache::smInstance = (MgdResourceContentCache*)NULL;

MgdResourceContentCache::MgdResourceContentCache() { }

MgdResourceContentCache::~MgdResourceContentCache()
{
    ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgdResourceContentCache::~MgdResourceContentCache()\n")));
}

MgdResourceContentCache* MgdResourceContentCache::GetInstance()
{
    if (NULL == MgdResourceContentCache::smInstance)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD_RETURN (ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance (), 0));
        if (NULL == MgdResourceContentCache::smInstance)
        {
            MgdResourceContentCache::smInstance = new MgdResourceContentCache();
        }
    }
    return smInstance;
}

STRING MgdResourceContentCache::GetContentEntry(MgResourceIdentifier* resource)
{
    CHECKARGUMENTNULL(resource, L"MgdResourceContentCache::PutContentEntry");
    STRING resId = resource->ToString();

    ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, L""));
    MgdResourceContentCacheEntries::iterator i = m_resourceContentCacheEntries.find(resId);

    STRING ret;
    if (m_resourceContentCacheEntries.end() != i)
    {
        ret = i->second;
    }
    return ret;
}

INT32 MgdResourceContentCache::GetCacheSize()
{
	ACE_MT(ACE_GUARD_RETURN(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex, 0));
	return m_resourceContentCacheEntries.size();
}

void MgdResourceContentCache::Clear()
{
	ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));
	m_resourceContentCacheEntries.clear();
}

void MgdResourceContentCache::RemoveContentEntry(MgResourceIdentifier* resource)
{
    CHECKARGUMENTNULL(resource, L"MgdResourceContentCache::PutContentEntry");
    STRING resId = resource->ToString();

    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));
    MgdResourceContentCacheEntries::iterator i = m_resourceContentCacheEntries.find(resId);

    if (m_resourceContentCacheEntries.end() != i)
    {
        m_resourceContentCacheEntries.erase(i);
    }
}

void MgdResourceContentCache::PutContentEntry(MgResourceIdentifier* resource, CREFSTRING content)
{
    CHECKARGUMENTNULL(resource, L"MgdResourceContentCache::PutContentEntry");
    if (content.empty())
    {
        throw new MgInvalidArgumentException(L"MgdResourceContentCache::PutContentEntry", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    STRING resId = resource->ToString();
    //ACE_DEBUG((LM_DEBUG, ACE_TEXT("(%t) MgdResourceContentCache::PutContentEntry - %W\n"), resId.c_str()));
    
    ACE_MT(ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, m_MgdMutex));
    m_resourceContentCacheEntries[resId] = content;
}