//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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

#ifndef MGSITECONNECTION_H_
#define MGSITECONNECTION_H_

// Forward declare MgConfiguration to avoid ACE header dependency
class MgConfiguration;
class MG_MAPGUIDE_API MgSiteConnection;
class MG_MAPGUIDE_API MgServerConnectionStack;

#ifdef _WIN32
#undef CreateService
#endif

class MgSite;

/// \defgroup MgSiteConnection MgSiteConnection
/// \ingroup Site_Service_Module
/// \{
//////////////////////////////////////////////////////////////////
/// \brief
/// Site connection.  Provides basic authentication and Service access
/// capabilities for the site.
///
/// \if INTERNAL
/// INTERNAL:
/// An MgSiteHelper derived class is used internally to implement the
/// Authenticate and GetService methods.  The Factory dlls and CreateService
/// should be used to get the appropriate Helpers.  Otherwise, there will be
/// significant undesirable dll interdependencies.
/// \n
/// The site server ip address and ports are contained in a configuration
/// file for the web tier.  The configuration file also defines which services
/// are hosted locally.
/// \endif
///
class MG_MAPGUIDE_API MgSiteConnection  : public MgDisposable
{
    DECLARE_CLASSNAME(MgSiteConnection)

PUBLISHED_API:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a site connection.  The local site defined
    /// for the web tier or server is used.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgSiteConnection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgSiteConnection();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgSiteConnection();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \exception MgConfigurationException
    ///  if config file is missing
    ///
    MgSiteConnection();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Establishes a connection to a site
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void Open(MgUserInformation userInformation);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void Open(MgUserInformation userInformation);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void Open(MgUserInformation userInformation);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param userInformation (MgUserInformation)
    /// User information to authenticate against.  Can either be username/password
    /// or session identifier.  User information also contains client locale.
    /// See MgUserInformation for details.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgAuthenticationFailedException if user information was not supplied
    /// \exception MgConfigurationException if configuration information could not be loaded
    /// \exception MgUnauthorizedAccessException if user is not authorized for site
    /// \exception MgConnectionFailedException if site cannot be connected to
    ///
    /// \cond INTERNAL
    /// INTERNAL:
    /// Open should authenticate with MgSiteConnection::Authenticate which in turn uses the appropriate
    /// helper class.  A connection to the Site Server, if necessary, should be created
    /// and maintained.
    /// \endcond
    ///
    void Open(MgUserInformation* userInformation);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns a new instance of the specified service from the site.  A connection
    /// to the machine hosting the service will be created if required.
    /// Configuration information determines whether the service is hosted locally
    /// or not. The configuration information is stored in a file named webconfig.ini
    /// which is located in the same folder as the map agent. The parameters in this file
    /// can be modified using the web administration application.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgService CreateService(int serviceType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgService CreateService(int serviceType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgService CreateService(int serviceType);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param serviceType (int)
    /// Type of service to create.  See MgServiceType for valid service names.
    ///
    /// \return
    /// Returns the created service
    ///
    /// \exception MgServiceNotSupportedException if service type is not supported
    /// \exception MgConnectionNotOpenException if connection has not been opened
    ///
    /// \cond INTERNAL
    /// INTERNAL:
    /// GetService relies on the implementation of the bound MgSiteHelper class
    /// \endcond
    ///
    MgService* CreateService(INT32 serviceType);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns an instance of the MgSite object.  This object
    /// is responsible for user/group management and session management.
    /// MgSite exposes functionality to enumerate users and groups and to
    /// manage user sessions.  The connection must already be open to
    /// obtain access to the site.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgSite GetSite();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgSite GetSite();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgSite GetSite();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns a reference to the MgSite object.
    ///
    /// \exception MgConnectionNotOpenException if connection has not been opened
    ///
    MgSite* GetSite();

EXTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a site at the given url.
    ///
    /// \param url
    /// HTTP url for a site.
    ///
    /// \cond INTERNAL
    /// INTERNAL:
    /// Since the URL is specified, this method binds to an MgHttpProxySiteHelper.
    /// \endcond
    ///
    MgSiteConnection(CREFSTRING url);

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destructor
    ///
    ~MgSiteConnection();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Authenticates a user.
    ///
    /// \param userInformation
    /// User information to authenticate against.
    ///
    /// \return
    /// Nothing.
    ///
    /// \exception MgNullArgumentException
    /// \exception MgAuthenticationFailedException
    /// \exception MgUnauthorizedAccessException
    ///

    void Authenticate(MgUserInformation* userInformation);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates service on specific machine.  Works for local and proxy
    /// services based on passed in connection properties
    ///
    /// \param serviceType
    /// Service type to create
    /// \param connProp
    /// Connection properties defining endpoint of deries connection
    ///
    /// \return
    /// An appropriately constructed and initialized service object
    ///
    static MgService* CreateService(INT32 serviceType, MgConnectionProperties* connProp);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Return previously opened site server connection
    ///
    /// \return
    /// Connection to Site Server
    ///
    /// \cond INTERNAL
    /// Open must be called before this method will work.  If we are the Site Server
    /// then no connection will be returned.
    /// \endcond
    ///
    MgServerConnection* GetSiteConnection();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Looks in config.ini to find out whether it is a site server.
    /// This is currently determined based on availablity of resource
    /// service.
    ///
    /// \return
    /// true -  resource service is hosted
    /// false - resource service is not hosted
    ///
    bool IsSiteServer();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines whether server is hosting a particular service.
    /// This is based on the config.ini properties.
    ///
    /// \return
    /// true - hosts the service
    /// false - does not host the service
    ///
    bool IsServiceLocal(INT32 serviceType);

    ///////////////////////////////
    /// \brief
    /// Sets the site connection for the current thread.  This
    /// function uses thread local storage.
    ///
    /// \param siteConnection
    /// Site connection to assign to the current thread
    ///
    /// \return
    /// Nothing
    ///
    static void SetCurrentConnection(MgSiteConnection* siteConnection);

    ///////////////////////////////
    /// \brief
    /// Gets the site connection for the current thread which was
    /// set previously using SetCurrentConnection.  This
    /// function uses thread local storage.
    ///
    /// \return
    /// Site connection for currently executing thread
    ///
    /// \exception MgConnectionNotOpenException if the site connection has not been set
    ///
    static MgSiteConnection* GetCurrentConnection();


    ///////////////////////////////
    /// \brief
    /// Gets the user info for this site
    ///
    /// \return
    /// User info
    MgUserInformation* GetUserInfo();

protected:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// The integer value
    ///
     INT32 GetClassId();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Self Destructor
    ///
    /// \return
    /// Nothing
    ///
     virtual void Dispose();

private:

    static MgService* CreateService(ServerConnectionType connType, INT32 serviceType, MgConnectionProperties* connProp);

    void Initialize();

    void AuthenticateWithHttpServer(MgUserInformation* userInformation);
    void AuthenticateWithSiteServer(MgUserInformation* userInformation);

    MgConnectionProperties* GetSiteConnectionProperties(MgUserInformation* userInformation);
    MgConnectionProperties* GetConnectionProperties(INT32 serviceType);
    MgConnectionProperties* GetConnectionPropertiesFromSiteServer(INT32 serviceType);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines whether it is a HTTP connection or TCP/IP connection
    ///
    /// \return
    /// true - HTTP connection
    /// false - TCP/IP connection
    ///
    bool IsHttpConnection();
    bool IsServer();
    bool IsWebTier();

private:

    Ptr<MgConnectionProperties> m_connProp;
    MgConfiguration* m_config;

    bool m_isServer;
    bool m_isSiteServer;
    bool m_isWebTier;
    bool m_http;

    STRING m_url;

CLASS_ID:

    static const INT32 m_cls_id = MapGuide_Service_SiteConnection;
};
/// \}

#endif  //  MGSITECONNECTION_H_
