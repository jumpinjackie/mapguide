#ifndef MG_FDO_CONNECTION_POOL
#define MG_FDO_CONNECTION_POOL

#include "Fdo.h"

/// \cond INTERNAL

class MgdFeatureConnection;
class MgResourceIdentifier;
class MgConfiguration;

struct PoolCacheEntry
{
    PoolCacheEntry(CREFSTRING resourceId, CREFSTRING providerName, INT32 open, INT32 closed) 
    {
        ResourceId = resourceId;
        ProviderName = providerName;
        OpenCount = open;
        ClosedCount = closed;
    }

    STRING ResourceId;
    INT32 OpenCount;
    INT32 ClosedCount;
    STRING ProviderName;
};

class MG_DESKTOP_API MgdFdoConnectionPool //Needs to be exported for unit testing code
{
public:
    static void Initialize(MgConfiguration* config);
    static FdoIConnection* GetConnection(MgResourceIdentifier* resId);
    static FdoIConnection* GetConnection(CREFSTRING provider, CREFSTRING connectionString);
    static void ReturnConnection(MgdFeatureConnection* conn);
    static void Cleanup();
    static void PurgeCachedConnections(MgResourceIdentifier* resId);
    static void PurgeCachedConnectionsUnderFolder(MgResourceIdentifier* resId);
    static void GetCacheInfo(std::vector<PoolCacheEntry*>& entries); //Caller responsible for cleanup of contents

private:
    static bool StringStartsWith(CREFSTRING haystack, CREFSTRING needle);
};
/// \endcond

#endif