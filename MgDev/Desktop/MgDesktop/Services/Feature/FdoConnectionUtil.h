#ifndef DESKTOP_FDO_CONNECTION_UTIL_H
#define DESKTOP_FDO_CONNECTION_UTIL_H

class MgdFdoConnectionPool;
class FdoIConnection;
class MgResourceIdentifier;
class MgdResourceService;

namespace MdfModel
{
    class FeatureSource;
}

/// \cond INTERNAL
class MgdFdoConnectionUtil
{
    friend class MgdFeatureConnection;
    friend class MgdFdoConnectionPool;

public:
    static MdfModel::FeatureSource* GetFeatureSource(MgResourceIdentifier* resource);
    static void CloseConnection(FdoIConnection* conn);

private:
    static void PerformTagSubstitution(MgdResourceService* resSvc, REFSTRING str, MgResourceIdentifier* resource, CREFSTRING username = L"", CREFSTRING password = L"");
    static STRING ParseNonQualifiedProviderName(CREFSTRING providerName);

    //Creates a connection by feature source ID. Any tag and aliases present in the connection parameters
    //are substituted
    static FdoIConnection* CreateConnection(MgResourceIdentifier* resource);

    //Createa a connection by raw FDO connection string. No tag subsitution is performed.
	static FdoIConnection* CreateConnection(CREFSTRING provider, CREFSTRING connectionString);    
#ifdef DEBUG_FDO_CONNECTION_POOL
    static void CheckCallStats();
#endif

private:
    static INT64 sm_nConnectionsCreated;
    static INT64 sm_nConnectionsClosed;
};
/// \endcond

#endif