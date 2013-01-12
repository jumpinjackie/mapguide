#ifndef DESKTOP_FEATURE_CONNECTION_H
#define DESKTOP_FEATURE_CONNECTION_H

class FdoIConnection;
class MgResourceIdentifier;
class MgdFdoConnectionPool;

template class Ptr<MgResourceIdentifier>;
/// \cond INTERNAL
class MgdFeatureConnection : public MgGuardDisposable
{
    friend class MgdFdoConnectionPool;

public:
    MgdFeatureConnection(MgResourceIdentifier* featureSourceIdentifier);
    MgdFeatureConnection(CREFSTRING providerName, CREFSTRING connectionString);
    virtual ~MgdFeatureConnection();

private:
    MgdFeatureConnection();
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

	bool IsSupportedFunction(FdoFunction* function);
    void OwnReader();

private:

    bool m_bIsCreatedFromFeatureSource;
    FdoIConnection* m_fdoConn;
    MgResourceIdentifier* m_resourceId;
    bool m_bCloseConnection;
};
///\endcond
#endif