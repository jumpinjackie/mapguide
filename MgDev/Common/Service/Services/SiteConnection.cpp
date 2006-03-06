//
//  Copyright (C) 2004-2006  Autodesk, Inc.
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of version 2.1 of the GNU Lesser
//  General Public License as published by the Free Software Foundation.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
//

#include "AceCommon.h"
#include "ServiceFactory.h"

#ifdef WIN32
#undef CreateService
#endif

static ACE_thread_key_t g_threadLocalSiteConnection = 0;

//////////////////////////////////////////////////////////////////
/// <summary>
/// Creates a site connection.  The local site defined
/// for the web tier or server is used.
/// EXCEPTION:
/// MgConfigurationException, if config file is missing
/// </summary>

MgSiteConnection::MgSiteConnection()
{
    Initialize();
}


MgSiteConnection::MgSiteConnection(CREFSTRING url)
{
    m_url = url;
    Initialize();
}

/// <summary>
/// Initializes and determine the machine type
/// </summary>

void MgSiteConnection::Initialize()
{
    MG_TRY()

    // get instanced MgConfiguration for local use
    m_config = MgConfiguration::GetInstance();

    m_isServer     = false;
    m_isSiteServer = false;
    m_isWebTier    = false;
    m_http         = IsHttpConnection();

    if (!m_http)
    {
        m_isServer     = IsServer();
        m_isSiteServer = IsSiteServer();
        m_isWebTier    = IsWebTier();
    }

    // Set up current connection for thread
    MgSiteConnection::SetCurrentConnection(this);

    MG_CATCH_AND_THROW(L"MgSiteConnection::Initialize")
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Destructor
/// </summary>

MgSiteConnection::~MgSiteConnection()
{
    // Drop current connection for thread
    MgSiteConnection::SetCurrentConnection(NULL);
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Establishes a connection to a site.
/// </summary>
void MgSiteConnection::Open(MgUserInformation* userInformation)
{
    // We can not have null userInformation
    if (NULL == userInformation)
    {
        throw new MgAuthenticationFailedException(L"MgSiteConnection.Open", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    MgUserInformation::SetCurrentUserInfo(userInformation);

    m_connProp = NULL;

    // If none of them is valid, means we have a problem.
    if (!m_http && !m_isWebTier && !m_isServer && !m_isSiteServer)
    {
        MgStringCollection arguments;
        arguments.Add(MgConfigProperties::DefaultConfigurationFilename);

        throw new MgConfigurationException(L"MgSiteConnection.Open", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    // Get the service IP address from the site server
    if (m_http)
    {
        // Connection property for HTTP request
         m_connProp = new MgConnectionProperties(m_url);
    }
    // Site connection properties for local site
    // We need to first determine whether it is site server.
    else if (m_isSiteServer)
    {
        m_connProp = new MgConnectionProperties(L"", 0);
    }
    // Site connection properties for web-tier or server-2-server interaction
    else if (m_isWebTier || m_isServer)
    {
        m_connProp = GetSiteConnectionProperties();
    }

    if (m_connProp == NULL)
    {
        throw new MgLogicException(L"MgSiteConnection.Open",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (!m_connProp->IsLocal())
    {
        Authenticate(userInformation);
    }
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the specified service from the site.  A connection
/// to the machine hosting the service will be created if required.
/// Configuration information determines whether the service is hosted locally
/// or not.
/// </summary>
/// <param name="serviceType">
/// Type of service to create, a value from MgServiceType
/// <returns>
/// Returns the created service
/// </returns>
/// EXCEPTIONS:
/// MgServiceNotSupported
/// MgConnectionNotOpen

MgService* MgSiteConnection::CreateService(INT32 serviceType)
{
    Ptr<MgConnectionProperties> connProp;

    if (IsServiceLocal(serviceType))
    {
        // Service is hosted by this site itself
        // Therefore, no connection needed to any other site.
        // In otherwords, empty connection properties.
        // This is based on assumption that a machine hosting
        // a particular service can be directly used without going
        // to site server.
        connProp = new MgConnectionProperties(L"", 0);
    }
    else
    {
        // Get the service IP address from the site server
        connProp = GetConnectionProperties(serviceType);
    }

    return CreateService(serviceType, connProp);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Creates service on specific machine.  Works for local and proxy
/// services based on passed in connection properties
/// </summary>
MgService* MgSiteConnection::CreateService(INT32 serviceType,
    MgConnectionProperties* connProp)
{
    assert(NULL != connProp);
    ServerConnectionType connType;

    if (connProp->IsLocal())
    {
        connType = sctLocalInProc;
    }
    else if (connProp->GetUrl().empty())
    {
        connType = sctRemoteServerToServer;
    }
    else
    {
        connType = sctServerToHttp;
    }

    Ptr<MgService> service = CreateService(connType, serviceType, connProp);

    if (service == NULL)
    {
        throw new MgServiceNotSupportedException(L"CreateService",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return service.Detach();
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Create the appropriate service based on the connection type
/// and desired service
/// </summary>
/// <returns>
/// MgService instance or NULL
/// </returns>
MgService* MgSiteConnection::CreateService(ServerConnectionType connType,
    INT32 serviceType, MgConnectionProperties* connProp)
{
    MgService* service = (MgService*) CreateMgService(connType, serviceType, connProp);

    return service;
}


/////////////////////////////////////////////////////////////////
/// <summary>
/// Get the class Id
/// </summary>
/// <returns>
/// The integer value
/// </returns>
INT32 MgSiteConnection::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Self Destructor
/// </summary>
/// <returns>
/// Nothing
/// </returns>
void MgSiteConnection::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Looks in config.ini to find out whether it is a site server.
/// This is currently determined based on availablity of resource
/// service.
/// </summary>
/// <returns>
/// true -  resource service is hosted
/// false - resource service is not hosted
/// </returns>
bool MgSiteConnection::IsSiteServer()
{
    // NOTE: We are assuming that no more than one machine can host site service.
    return IsServiceLocal(MgServiceType::SiteService);
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Determines whether it is a HTTP connection or TCP/IP connection
/// </summary>
/// <returns>
/// true - HTTP connection
/// false - TCP/IP connection
/// </returns>
bool MgSiteConnection::IsHttpConnection()
{
    return (m_url.empty() ? false : true);
}

/// <summary>
/// Determines whether it hosts any service. If so, it is the server.
/// </summary>

bool MgSiteConnection::IsServer()
{
    bool isServer = false;

    // If any one of the service is available means it is a server
    if ( IsServiceLocal(MgServiceType::DrawingService)   ||
         IsServiceLocal(MgServiceType::FeatureService)   ||
         IsServiceLocal(MgServiceType::MappingService)   ||
         IsServiceLocal(MgServiceType::RenderingService) ||
         IsServiceLocal(MgServiceType::ResourceService)  ||
         IsServiceLocal(MgServiceType::TileService) )
    {
        isServer = true;
    }

    return isServer;
}


/// <summary>
/// Determines whether it is instantiated on webtier
/// </summary>

bool MgSiteConnection::IsWebTier()
{
    bool isWebTier = false;

    if ( m_config->IsFileLoaded() && !IsServer() )
    {
        isWebTier = true;
    }

    return isWebTier;
}


//////////////////////////////////////////////////////////////////
/// <summary>
/// Determines whether server is hosting a particular service.
/// This is based on the config.ini properties.
/// </summary>
/// <returns>
/// true - hosts the service
/// false - does not host the service
/// </returns>
bool MgSiteConnection::IsServiceLocal(INT32 serviceType)
{
    bool isHosting = false;

    try
    {
        switch(serviceType)
        {
            case MgServiceType::DrawingService:
                m_config->GetBoolValue(MgConfigProperties::HostPropertiesSection,
                                    MgConfigProperties::HostPropertyDrawingService,
                                    isHosting,
                                    MgConfigProperties::DefaultHostPropertyDrawingService);
                break;
            case MgServiceType::FeatureService:
                m_config->GetBoolValue(MgConfigProperties::HostPropertiesSection,
                                    MgConfigProperties::HostPropertyFeatureService,
                                    isHosting,
                                    MgConfigProperties::DefaultHostPropertyFeatureService);
                break;
            case MgServiceType::MappingService:
                m_config->GetBoolValue(MgConfigProperties::HostPropertiesSection,
                                    MgConfigProperties::HostPropertyMappingService,
                                    isHosting,
                                    MgConfigProperties::DefaultHostPropertyMappingService);
                break;
            case MgServiceType::RenderingService:
                m_config->GetBoolValue(MgConfigProperties::HostPropertiesSection,
                                    MgConfigProperties::HostPropertyRenderingService,
                                    isHosting,
                                    MgConfigProperties::DefaultHostPropertyRenderingService);
                break;
            case MgServiceType::ResourceService:
                // TODO: We should find a better way to determine the location of site server.
                // If an entry in serverconfig.ini is missing, it would end up in infinite
                // TCP/IP calls or until it times out
                m_config->GetBoolValue(MgConfigProperties::HostPropertiesSection,
                                    MgConfigProperties::HostPropertyResourceService,
                                    isHosting,
                                    MgConfigProperties::DefaultHostPropertyResourceService);
                break;
            case MgServiceType::SiteService:
                // TODO: We should find a better way to determine the location of site server.
                // If an entry in serverconfig.ini is missing, it would end up in infinite
                // TCP/IP calls or until it times out
                m_config->GetBoolValue(MgConfigProperties::HostPropertiesSection,
                                    MgConfigProperties::HostPropertySiteService,
                                    isHosting,
                                    MgConfigProperties::DefaultHostPropertySiteService);
                break;
            case MgServiceType::TileService:
                m_config->GetBoolValue(MgConfigProperties::HostPropertiesSection,
                                    MgConfigProperties::HostPropertyTileService,
                                    isHosting,
                                    MgConfigProperties::DefaultHostPropertyTileService);
                break;
            case MgServiceType::ServerAdminService:
                if (IsServer())
                {
                    isHosting = true;
                }
                break;
            default:
                break;
        }
    }
    catch (MgException* me)
    {
        // Object not found or invalid property value
        // means service does not exist
        SAFE_RELEASE(me);
    }

    return isHosting;
}

void MgSiteConnection::AuthenticateWithHttpServer(MgUserInformation* userInformation)
{
    assert(NULL != userInformation);

    // Send HTTP request to the URL specified for authentication.

    throw new MgNotImplementedException(L"MgSiteConnection.AuthenticateWithHttpServer",
        __LINE__, __WFILE__, NULL, L"", NULL);
}

void MgSiteConnection::AuthenticateWithSiteServer(MgUserInformation* userInformation)
{
    assert(NULL != userInformation);
    Ptr<MgSite> site = new MgSite();

    site->Open(userInformation); // Authentication is implicitly performed.
}

/// <summary>
/// Determines connection properties for http/site server/webtier,server-2-server interaction
/// </summary>

MgConnectionProperties* MgSiteConnection::GetConnectionProperties(INT32 serviceType)
{
    Ptr<MgConnectionProperties> connProp;

    // Send HTTP request to the URL specified for authentication
    if (m_http)
    {
        connProp = new MgConnectionProperties(m_url);
    }
    else if (m_isWebTier)
    {
        connProp = GetConnectionPropertiesFromSiteServer(serviceType);
    }
    else
    {
        // THIS SHOULD NEVER BE CALLED!!
        // Servers should be going through the service manager and not MgSiteConnection
        // to get their services.
        throw new MgNotImplementedException(L"MgSiteConnection.GetConnectionProperties", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return connProp.Detach();
}

/// <summary>
/// Determines connection properties from site server for the services
/// </summary>

MgConnectionProperties* MgSiteConnection::GetConnectionPropertiesFromSiteServer(INT32 serviceType)
{
    CHECKARGUMENTNULL((MgConnectionProperties*)m_connProp, L"MgSiteConnection.GetConnectionPropertiesFromSiteServer");

    Ptr<MgConnectionProperties> connProp;

    if (m_connProp->IsLocal())
    {
        throw new MgInvalidOperationException(
            L"MgSiteConnection.GetConnectionPropertiesFromSiteServer",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }
    else
    {
        // TODO: The following overhead could be eliminated if additional load
        //       balancing is implemented on the Web Tier. Also, the
        //       RequestServer API could be optimized to allow the Web Tier to
        //       notify the site server when a support server is down.
        MgUserInformation* userInformation = MgUserInformation::GetCurrentUserInfo();
        assert(NULL != userInformation);
        Ptr<MgSite> site = new MgSite();

        site->Open(userInformation);

        STRING target = site->RequestServer(serviceType);

        connProp = new MgConnectionProperties(target, m_connProp->GetPort());
    }

    return connProp.Detach();
}

/// <summary>
/// Gets the site ipaddress and port# for connection
/// </summary>

MgConnectionProperties* MgSiteConnection::GetSiteConnectionProperties()
{
    Ptr<MgConnectionProperties> connProp;
    STRING target = L"";
    INT32 port = 0;

    try
    {
        // Get ip address for site server
        m_config->GetStringValue(   MgConfigProperties::SiteConnectionPropertiesSection,
                                    MgConfigProperties::SiteConnectionPropertyIpAddress,
                                    target,
                                    MgConfigProperties::DefaultSiteConnectionPropertyIpAddress);

        // Get the client port
        m_config->GetIntValue(  MgConfigProperties::ClientConnectionPropertiesSection,
                                MgConfigProperties::ClientConnectionPropertyPort,
                                port,
                                MgConfigProperties::DefaultClientConnectionPropertyPort);

        // Use the client port for Server/Web Tier communications.
        connProp = new MgConnectionProperties(target, port);

    }
    catch (MgException* me)
    {
        // Site property address/client port not found, invalid config.ini
        SAFE_RELEASE(me);

        MgStringCollection arguments;
        arguments.Add(m_config->GetFileName());

        throw new MgConfigurationException(L"MgSiteConnection.GetSiteConnectionProperties", __LINE__, __WFILE__, &arguments, L"", NULL);
    }

    return connProp.Detach();
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Authenticates a user.
/// </summary>
///
/// <param name="userInformation">
/// User information to authenticate against.
/// </param>
///
/// <returns>
/// Nothing.
/// </returns>
///
/// EXCEPTIONS:
/// MgNullArgumentException
/// MgAuthenticationFailedException
/// MgUnauthorizedAccessException

void MgSiteConnection::Authenticate(MgUserInformation* userInformation)
{
    if (IsHttpConnection())
    {
        AuthenticateWithHttpServer(userInformation);
    }
    else if (!IsSiteServer())
    {
        AuthenticateWithSiteServer(userInformation);
    }
}

//////////////////////////////////////////////////////////////////
/// <summary>
/// Returns an instance of the MgSite object.  This object
/// is responsible for user/group management and session management.
/// MgSite exposes functionality to enumerate users and groups and to
/// manage user sessions.  The connection must already be open to
/// obtain access to the site.
/// </summary>
MgSite* MgSiteConnection::GetSite()
{
    //TODO: We need to move MgSite into MgBaseService so we can implement this method!!!!
    throw new MgNotImplementedException(L"MgSiteConnection.GetSite", __LINE__, __WFILE__, NULL, L"", NULL);
}

///////////////////////////////
///<summary>
///Sets the site connection for the current thread.  This
///function uses thread local storage.
///</summary>
void MgSiteConnection::SetCurrentConnection(MgSiteConnection* siteConnection)
{
    if (0 == g_threadLocalSiteConnection)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance ()));
        if (0 == g_threadLocalSiteConnection)
        {
            if (ACE_OS::thr_keycreate(&g_threadLocalSiteConnection, NULL) < 0)
            {
                g_threadLocalSiteConnection = 0;
            }
        }
    }

    if (0 != g_threadLocalSiteConnection)
    {
        ACE_OS::thr_setspecific(g_threadLocalSiteConnection, siteConnection);
    }
}

///////////////////////////////
///<summary>
///Gets the site connection for the current thread which was
///set previously using SetCurrentConnection.  This
///function uses thread local storage.
///</summary>
MgSiteConnection* MgSiteConnection::GetCurrentConnection()
{
    MgSiteConnection* connection = NULL;
    if (0 != g_threadLocalSiteConnection)
    {
        ACE_OS::thr_getspecific(g_threadLocalSiteConnection, (void**) &connection);
    }

    if (NULL == connection)
    {
        // No site connection means that we not have opened the connection to the site server yet.
        throw new MgConnectionNotOpenException(L"MgSiteConnection.GetCurrentConnection", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return connection;
}
