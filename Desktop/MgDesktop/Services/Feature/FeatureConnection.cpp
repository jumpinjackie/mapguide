#include "Fdo.h"
#include "FdoConnectionUtil.h"
#include "FeatureConnection.h"
#ifdef DEBUG_FDO_CONNECTION_POOL
#include <WinBase.h>
#endif

// Initialize the minimum required memeber variables
MgdFeatureConnection::MgdFeatureConnection(MgResourceIdentifier* featureSourceIdentifier)
{
    Initialize();
    //This is a potentially poolable connection
    m_fdoConn = MgdFdoConnectionPool::GetConnection(featureSourceIdentifier);
    CHECKNULL(m_fdoConn, L"MgdFeatureConnection.MgdFeatureConnection()");

#ifdef DEBUG_FDO_CONNECTION_POOL
    ACE_DEBUG((LM_INFO, ACE_TEXT("MgdFeatureConnection::MgdFeatureConnection(MgResourceIdentifier*) - refcount: %d\n"), m_fdoConn->GetRefCount()));
#endif

    m_resourceId = SAFE_ADDREF(featureSourceIdentifier);
    m_bIsCreatedFromFeatureSource = true;
}


MgdFeatureConnection::MgdFeatureConnection(CREFSTRING providerName, CREFSTRING connectionString)
{
    Initialize();
    //This is not a poolable connection
    m_fdoConn = MgdFdoConnectionPool::GetConnection(providerName, connectionString);
    CHECKNULL(m_fdoConn, L"MgdFeatureConnection.MgdFeatureConnection()");
    if (!connectionString.empty())
        m_fdoConn->Open();

#ifdef DEBUG_FDO_CONNECTION_POOL
    ACE_DEBUG((LM_INFO, ACE_TEXT("MgdFeatureConnection::MgdFeatureConnection(CREFSTRING, CREFSTRING) - refcount: %d\n"), m_fdoConn->GetRefCount()));
#endif

    m_resourceId = NULL;
    m_bIsCreatedFromFeatureSource = false;
}


MgdFeatureConnection::~MgdFeatureConnection()
{
    MG_TRY()

    if(m_bCloseConnection)
    {
        Close();
    }

    SAFE_RELEASE(m_resourceId);

    MG_CATCH_AND_RELEASE()
}

void MgdFeatureConnection::Dispose()
{
    delete this;
}

void MgdFeatureConnection::Initialize()
{
    m_fdoConn = NULL;
    m_resourceId = NULL;
    m_bCloseConnection = true;
}

void MgdFeatureConnection::Close()
{
    if (NULL != m_fdoConn)
    {
    #ifdef DEBUG_FDO_CONNECTION_POOL
        FdoInt32 iRefCount = m_fdoConn->GetRefCount();
        ACE_DEBUG((LM_INFO, ACE_TEXT("MgdFeatureConnection::Close() - refcount %d\n"), iRefCount));
    #endif
        MgdFdoConnectionPool::ReturnConnection(this);
        m_fdoConn = NULL;
    }
}

MgResourceIdentifier* MgdFeatureConnection::GetFeatureSource()
{
    return SAFE_ADDREF(m_resourceId);
}

FdoIConnection* MgdFeatureConnection::GetConnection()
{
    return FDO_SAFE_ADDREF(m_fdoConn);
}


STRING MgdFeatureConnection::GetProviderName()
{
    CHECKNULL(m_fdoConn, L"MgdFeatureConnection.GetProviderName");

    // Get FdoIConnectionInfo
    FdoPtr<FdoIConnectionInfo> fdoConnInfo = m_fdoConn->GetConnectionInfo();
    CHECKNULL((FdoIConnectionInfo*)fdoConnInfo, L"MgdFeatureConnection.GetProviderName");

    return fdoConnInfo->GetProviderName();
}


bool MgdFeatureConnection::IsConnectionOpen()
{
    CHECKNULL(m_fdoConn, L"MgdFeatureConnection.IsConnectionOpen()");
    FdoConnectionState state = m_fdoConn->GetConnectionState();
    if (FdoConnectionState_Open != state)
        return false;

    return true;
}


bool MgdFeatureConnection::IsConnectionPending()
{
    CHECKNULL(m_fdoConn, L"MgdFeatureConnection.IsConnectionPending()");
    FdoConnectionState state = m_fdoConn->GetConnectionState();
    if (FdoConnectionState_Pending != state)
        return false;

    return true;
}


bool MgdFeatureConnection::IsConnectionBusy()
{
    CHECKNULL(m_fdoConn, L"MgdFeatureConnection.IsConnectionBusy()");
    FdoConnectionState state = m_fdoConn->GetConnectionState();
    if (FdoConnectionState_Busy != state)
        return false;

    return true;
}


bool MgdFeatureConnection::IsConnectionClosed()
{
    CHECKNULL(m_fdoConn, L"MgdFeatureConnection.IsConnectionClosed()");
    FdoConnectionState state = m_fdoConn->GetConnectionState();
    if (FdoConnectionState_Closed != state)
        return false;

    return true;
}

bool MgdFeatureConnection::SupportsJoins()
{
    CHECKNULL(m_fdoConn, L"MgdFeatureConnection.SupportsJoins");

    FdoPtr<FdoIConnectionCapabilities> connCaps = m_fdoConn->GetConnectionCapabilities();
    CHECKNULL((FdoIConnectionCapabilities*)connCaps, L"MgdFeatureConnection.SupportsJoins");
    bool joinsSupported = connCaps->SupportsJoins();
#ifdef DEBUG_FDOJOIN
    if (!joinsSupported)
    {
        ACE_DEBUG((LM_INFO, ACE_TEXT("\n[FDO Join Test]: Connection does not support FDO joins")));
    }
#endif
    return joinsSupported;
}

bool MgdFeatureConnection::SupportsSelectOrdering()
{
    CHECKNULL(m_fdoConn, L"MgdFeatureConnection.SupportsSelectOrdering");

    FdoPtr<FdoICommandCapabilities> cmdCaps = m_fdoConn->GetCommandCapabilities();
    CHECKNULL((FdoICommandCapabilities*)cmdCaps, L"MgdFeatureConnection.SupportsSelectOrdering");

    return cmdCaps->SupportsSelectOrdering();
}

FdoJoinType MgdFeatureConnection::GetJoinTypes() const
{
    CHECKNULL(m_fdoConn, L"MgdFeatureConnection.GetJoinTypes");

    FdoPtr<FdoIConnectionCapabilities> connCaps = m_fdoConn->GetConnectionCapabilities();
    CHECKNULL((FdoIConnectionCapabilities*)connCaps, L"MgdFeatureConnection.GetJoinTypes");

    return (FdoJoinType)connCaps->GetJoinTypes();
}

bool MgdFeatureConnection::SupportsCommand(INT32 commandType)
{
    CHECKNULL(m_fdoConn, L"MgdFeatureConnection.SupportsCommand");

    FdoPtr<FdoICommandCapabilities> fcc = m_fdoConn->GetCommandCapabilities();
    CHECKNULL((FdoICommandCapabilities*)fcc, L"MgdFeatureConnection.SupportsCommand");

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

void MgdFeatureConnection::OwnReader()
{
    if(! m_bCloseConnection)
        FDO_SAFE_ADDREF(m_fdoConn);
    m_bCloseConnection = false;
}

bool MgdFeatureConnection::IsSupportedFunction(FdoFunction* fdoFunc)
{
	CHECKNULL(m_fdoConn, L"MgdFeatureConnection.SupportsFunction");

    FdoPtr<FdoIExpressionCapabilities> fec = m_fdoConn->GetExpressionCapabilities();
    CHECKNULL((FdoIExpressionCapabilities*)fec, L"MgdFeatureConnection.SupportsFunction");

    bool supports = false;

    FdoPtr<FdoFunctionDefinitionCollection> ffdc = fec->GetFunctions();
    if (NULL != (FdoFunctionDefinitionCollection*)ffdc)
    {
        FdoInt32 funcCnt = ffdc->GetCount();
        for (FdoInt32 i=0; i < funcCnt; i++)
        {
            FdoPtr<FdoFunctionDefinition> ffd = ffdc->GetItem(i);
            CHECKNULL((FdoFunctionDefinition*)ffd, L"MgdFeatureConnection.SupportsFunction");

            // TODO: Just comparing name is enough?
            // TODO: I think, NOT, because there can be overloaded functions like one function
            // with multiple arguments, differnet datatypes etc.
            //
            // Comparing argument count is not sufficient because, there can be optional arguments
            // as well. Therefore, we should just restrict to name comparision only
            FdoString* funcNameAllowed = ffd->GetName();
            FdoString* funcNameSupplied = fdoFunc->GetName();
            size_t cmp = _wcsicmp(funcNameAllowed, funcNameSupplied);
            if (cmp == 0)
            {
                supports = true;
                break;
            }
        }
    }
    return supports;
}