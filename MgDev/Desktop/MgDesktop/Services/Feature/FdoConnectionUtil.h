#ifndef DESKTOP_FDO_CONNECTION_UTIL_H
#define DESKTOP_FDO_CONNECTION_UTIL_H

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
    static void PerformTagSubstitution(MgdResourceService* resSvc, REFSTRING str, MgResourceIdentifier* resource);
    static STRING ParseNonQualifiedProviderName(CREFSTRING providerName);

    //Creates a connection by feature source ID. Any tag and aliases present in the connection parameters
    //are substituted
    static FdoIConnection* CreateConnection(MgResourceIdentifier* resource);

    //Createa a connection by raw FDO connection string. No tag subsitution is performed.
	static FdoIConnection* CreateConnection(CREFSTRING provider, CREFSTRING connectionString);    
};

#endif