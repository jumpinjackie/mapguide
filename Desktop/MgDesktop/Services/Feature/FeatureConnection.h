#ifndef DESKTOP_FEATURE_CONNECTION_H
#define DESKTOP_FEATURE_CONNECTION_H

class FdoIConnection;
class MgResourceIdentifier;
class MgFdoConnectionPool;

template class Ptr<MgResourceIdentifier>;

class MgFeatureConnection : public MgGuardDisposable
{
public:
    MgFeatureConnection(MgResourceIdentifier* featureSourceIdentifier);
    MgFeatureConnection(CREFSTRING providerName, CREFSTRING connectionString);
    virtual ~MgFeatureConnection();

private:
    MgFeatureConnection();
    void Initialize();

public:
    void Close();
    virtual void Dispose();

    MgResourceIdentifier* GetFeatureSource();

    FdoIConnection* GetConnection();
    STRING GetProviderName();
    
    bool IsConnectionOpen();
    bool IsConnectionBusy();
    bool IsConnectionClosed();
    bool IsConnectionPending();

    bool SupportsCommand(INT32 commandType);
    bool SupportsJoins();
    bool SupportsSelectOrdering();
    FdoJoinType GetJoinTypes() const;

    void OwnReader();

private:

    bool m_bIsCreatedFromFeatureSource;
    FdoIConnection* m_fdoConn;
    MgResourceIdentifier* m_resourceId;
    bool m_bCloseConnection;
};

#endif