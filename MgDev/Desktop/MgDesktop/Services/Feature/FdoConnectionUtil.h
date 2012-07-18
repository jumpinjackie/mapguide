#ifndef DESKTOP_FDO_CONNECTION_UTIL_H
#define DESKTOP_FDO_CONNECTION_UTIL_H

#define RELEASE_AND_DEBUG_FDO_CONNECTION_REF_COUNT(methodName, fdoConn, expectedRefCount) \
    FdoInt32 iRefCount = fdoConn->Release(); \
    if (iRefCount != expectedRefCount) \
    { \
        ACE_DEBUG((LM_INFO, ACE_TEXT("[%W - WARNING] Expected refcount of %d. Instead, got a refcount of %d\n"), methodName, expectedRefCount, iRefCount)); \
    } \

class MgFdoConnectionPool;
class FdoIConnection;
class MgResourceIdentifier;
class MgdResourceService;

namespace MdfModel
{
    class FeatureSource;
}

class MgFdoConnectionUtil
{
    friend class MgFeatureConnection;
    friend class MgFdoConnectionPool;

public:
    static MdfModel::FeatureSource* GetFeatureSource(MgResourceIdentifier* resource);

private:
    static void PerformTagSubstitution(MgdResourceService* resSvc, REFSTRING str, MgResourceIdentifier* resource, CREFSTRING username = L"", CREFSTRING password = L"");
    static STRING ParseNonQualifiedProviderName(CREFSTRING providerName);

    //Creates a connection by feature source ID. Any tag and aliases present in the connection parameters
    //are substituted
    static FdoIConnection* CreateConnection(MgResourceIdentifier* resource);

    //Createa a connection by raw FDO connection string. No tag subsitution is performed.
	static FdoIConnection* CreateConnection(CREFSTRING provider, CREFSTRING connectionString);    
};

#endif