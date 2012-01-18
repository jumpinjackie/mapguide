#include "Fdo.h"
#include "FdoConnectionUtil.h"
#include "FeatureConnection.h"

// Initialize the minimum required memeber variables
MgFeatureConnection::MgFeatureConnection(MgResourceIdentifier* featureSourceIdentifier)
{
    Initialize();
#ifdef DEBUG_FDO_CONNECTION_POOL
    ACE_DEBUG((LM_INFO, ACE_TEXT("MgFeatureConnection::MgFeatureConnection(MgResourceIdentifier*)\n")));
#endif
    //This is a poolable connection
    m_fdoConn = MgFdoConnectionPool::GetConnection(featureSourceIdentifier);
    CHECKNULL(m_fdoConn, L"MgFeatureConnection.MgFeatureConnection()");
    m_fdoConn->Open();

    m_resourceId = SAFE_ADDREF(featureSourceIdentifier);
    m_bIsCreatedFromFeatureSource = true;
}


MgFeatureConnection::MgFeatureConnection(CREFSTRING providerName, CREFSTRING connectionString)
{
    Initialize();
#ifdef DEBUG_FDO_CONNECTION_POOL
    ACE_DEBUG((LM_INFO, ACE_TEXT("MgFeatureConnection::MgFeatureConnection(CREFSTRING, CREFSTRING)\n")));
#endif
    //This is not a poolable connection
    m_fdoConn = MgFdoConnectionUtil::CreateConnection(providerName, connectionString); //pFdoConnectionManager->Open(providerName, connectionString);
    CHECKNULL(m_fdoConn, L"MgFeatureConnection.MgFeatureConnection()");
    if (!connectionString.empty())
        m_fdoConn->Open();

    m_resourceId = NULL;
    m_bIsCreatedFromFeatureSource = false;
}


MgFeatureConnection::~MgFeatureConnection()
{
    MG_TRY()

    if(m_bCloseConnection)
    {
        Close();
    }

    SAFE_RELEASE(m_resourceId);

    MG_CATCH_AND_RELEASE()
}

void MgFeatureConnection::Dispose()
{
    delete this;
}

void MgFeatureConnection::Initialize()
{
    m_fdoConn = NULL;
    m_resourceId = NULL;
    m_bCloseConnection = true;
}

void MgFeatureConnection::Close()
{
    if (NULL != m_fdoConn)
    {
    #ifdef DEBUG_FDO_CONNECTION_POOL
        ACE_DEBUG((LM_INFO, ACE_TEXT("MgFeatureConnection::Close()\n")));
    #endif
        //Ones initialized with a resource id are poolable and thus must be
        //returned
        if (m_bIsCreatedFromFeatureSource)
            MgFdoConnectionPool::ReturnConnection(this);
        else
            m_fdoConn->Close();

        m_fdoConn = NULL;
    }
}

MgResourceIdentifier* MgFeatureConnection::GetFeatureSource()
{
    return SAFE_ADDREF(m_resourceId);
}

FdoIConnection* MgFeatureConnection::GetConnection()
{
    return FDO_SAFE_ADDREF(m_fdoConn);
}


STRING MgFeatureConnection::GetProviderName()
{
    CHECKNULL(m_fdoConn, L"MgFeatureConnection.GetProviderName");

    // Get FdoIConnectionInfo
    FdoPtr<FdoIConnectionInfo> fdoConnInfo = m_fdoConn->GetConnectionInfo();
    CHECKNULL((FdoIConnectionInfo*)fdoConnInfo, L"MgFeatureConnection.GetProviderName");

    return fdoConnInfo->GetProviderName();
}


bool MgFeatureConnection::IsConnectionOpen()
{
    CHECKNULL(m_fdoConn, L"MgFeatureConnection.IsConnectionOpen()");
    FdoConnectionState state = m_fdoConn->GetConnectionState();
    if (FdoConnectionState_Open != state)
        return false;

    return true;
}


bool MgFeatureConnection::IsConnectionPending()
{
    CHECKNULL(m_fdoConn, L"MgFeatureConnection.IsConnectionPending()");
    FdoConnectionState state = m_fdoConn->GetConnectionState();
    if (FdoConnectionState_Pending != state)
        return false;

    return true;
}


bool MgFeatureConnection::IsConnectionBusy()
{
    CHECKNULL(m_fdoConn, L"MgFeatureConnection.IsConnectionBusy()");
    FdoConnectionState state = m_fdoConn->GetConnectionState();
    if (FdoConnectionState_Busy != state)
        return false;

    return true;
}


bool MgFeatureConnection::IsConnectionClosed()
{
    CHECKNULL(m_fdoConn, L"MgFeatureConnection.IsConnectionClosed()");
    FdoConnectionState state = m_fdoConn->GetConnectionState();
    if (FdoConnectionState_Closed != state)
        return false;

    return true;
}

bool MgFeatureConnection::SupportsJoins()
{
    CHECKNULL(m_fdoConn, L"MgFeatureConnection.SupportsJoins");

    FdoPtr<FdoIConnectionCapabilities> connCaps = m_fdoConn->GetConnectionCapabilities();
    CHECKNULL((FdoIConnectionCapabilities*)connCaps, L"MgFeatureConnection.SupportsJoins");
    bool joinsSupported = connCaps->SupportsJoins();
#ifdef DEBUG_FDOJOIN
    if (!joinsSupported)
    {
        ACE_DEBUG((LM_INFO, ACE_TEXT("\n[FDO Join Test]: Connection does not support FDO joins")));
    }
#endif
    return joinsSupported;
}

bool MgFeatureConnection::SupportsSelectOrdering()
{
    CHECKNULL(m_fdoConn, L"MgFeatureConnection.SupportsSelectOrdering");

    FdoPtr<FdoICommandCapabilities> cmdCaps = m_fdoConn->GetCommandCapabilities();
    CHECKNULL((FdoICommandCapabilities*)cmdCaps, L"MgFeatureConnection.SupportsSelectOrdering");

    return cmdCaps->SupportsSelectOrdering();
}

FdoJoinType MgFeatureConnection::GetJoinTypes() const
{
    CHECKNULL(m_fdoConn, L"MgFeatureConnection.GetJoinTypes");

    FdoPtr<FdoIConnectionCapabilities> connCaps = m_fdoConn->GetConnectionCapabilities();
    CHECKNULL((FdoIConnectionCapabilities*)connCaps, L"MgFeatureConnection.GetJoinTypes");

    return (FdoJoinType)connCaps->GetJoinTypes();
}

bool MgFeatureConnection::SupportsCommand(INT32 commandType)
{
    CHECKNULL(m_fdoConn, L"MgFeatureConnection.SupportsCommand");

    FdoPtr<FdoICommandCapabilities> fcc = m_fdoConn->GetCommandCapabilities();
    CHECKNULL((FdoICommandCapabilities*)fcc, L"MgFeatureConnection.SupportsCommand");

    bool supports = false;

    // Find all supported command types
    FdoInt32 cnt = 0;
    FdoInt32* fcmd = fcc->GetCommands(cnt);
    if (cnt > 0 && fcmd != NULL)
    {
        for (FdoInt32 i=0; i < cnt; i++)
        {
            if (fcmd[i] == (FdoInt32)commandType)
            {
                supports = true;
                break;
            }
        }
    }

    return supports;
}

void MgFeatureConnection::OwnReader()
{
    if(! m_bCloseConnection)
        FDO_SAFE_ADDREF(m_fdoConn);
    m_bCloseConnection = false;
}
