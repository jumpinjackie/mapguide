//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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

#ifndef MG_USER_INFORMATION_H
#define MG_USER_INFORMATION_H

#undef GetUserName

/// Locale codes are 2 letter language codes defined by ISO 639-1. Please refer
/// to http://www.loc.gov/standards/iso639-2/php/English_list.php
#define MG_LOCALE_LENGTH    2
#define MG_EXTENDED_LOCALE_LENGTH    5

#define MG_API_VERSION(major,minor,phase) ((major<<16) + (minor<<8) + phase)

///////////////////////////////////
/// \brief
/// Type of user information
/// INTERNAL API: Do not document
///
/// \internal
///
enum UserInformationType
{
    uitNone = 0, /// No credentials
    uitMg,  /// Internally managed MapGuide style credentials
    uitMgSession,  /// Internally managed MapGuide session
};

class MgUserInformation;
template class MG_MAPGUIDE_API Ptr<MgUserInformation>;

class MgSiteInfo;


/// \defgroup MgUserInformation MgUserInformation
/// \ingroup Common_Module
/// \{

///////////////////////////////////
/// \brief
/// This class manages user information.
///
/// \remarks
/// Information includes the
/// user's locale, session information, and credentials.
/// Credentials are based on MapGuide authentication only.
class MG_MAPGUIDE_API MgUserInformation : public MgSerializable
{
    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgUserInformation)

PUBLISHED_API:
    ///////////////////////////////
    /// \brief
    /// Constructs an unintialized information object
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgUserInformation();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgUserInformation();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgUserInformation();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns nothing
    ///
    MgUserInformation();

    ///////////////////////////////
    /// \brief
    /// Constructs an information object from a session identifier
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgUserInformation(string sessionId);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgUserInformation(String sessionId);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgUserInformation(string sessionId);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param sessionId (String/string)
    /// MapGuide session identifier
    ///
    /// \return
    /// Returns nothing
    ///
    MgUserInformation(CREFSTRING sessionId);

    ///////////////////////////////
    /// \brief
    /// Constructs information object with a MapGuide user name and password.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgUserInformation(string userName, string password);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgUserInformation(String userName, String password);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgUserInformation(string userName, string password);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param userName (String/string)
    /// MapGuide user name
    /// \param password (String/string)
    /// user password
    ///
    /// \return
    /// Returns nothing
    ///
    MgUserInformation(CREFSTRING userName, CREFSTRING password);

    ///////////////////////////////
    /// \brief
    /// Initializes credentials with a MapGuide user name and password.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetMgUsernamePassword(string userName, string password);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetMgUsernamePassword(String userName, String password);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetMgUsernamePassword(string userName, string password);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param userName (String/string)
    /// MapGuide user name
    /// \param password (String/string)
    /// user password
    ///
    /// \return
    /// Returns nothing
    ///
     void SetMgUsernamePassword(CREFSTRING userName, CREFSTRING password);

    ///////////////////////////////
    /// \brief
    /// Initializes user information with a MapGuide session identifier.  A
    /// session identifier can be created for the current user via
    /// MgSite::CreateSession.  In order to obtain a session identifier,
    /// MgSiteConnection must be opened with valid credentials.  Once opened,
    /// GetSite can be called to obtain MgSite which can then be used to
    /// create a session identifier for the currently logged in user.
    ///
    /// Session identifiers contain internally recognized authentication and locale information.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetMgSessionId(string sessionId);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetMgSessionId(String sessionId);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetMgSessionId(string sessionId);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param sessionId (String/string)
    /// MapGuide session identifier
    ///
    /// \return
    /// Returns nothing
    ///
     void SetMgSessionId(CREFSTRING sessionId);

    ///////////////////////////////
    /// \brief
    /// Sets the desired locale for the user.
    ///
    /// \remarks
    /// The locale code can be either a 2 letter language code defined by ISO 639-1, for example 'en',
    /// or a [language]-[country] code as allowed by the HTTP Accept-Language header, for example 'en-us' or 'en-gb'.
    /// Please refer
    /// to http://www.loc.gov/standards/iso639-2/php/English_list.php and
    /// http://www.w3.org/International/questions/qa-accept-lang-locales
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetLocale(string locale);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetLocale(String locale);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetLocale(string locale);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param locale (String/string)
    /// Input
    /// Locale code
    ///
    /// \return
    /// Returns nothing
    ///
     void SetLocale(CREFSTRING locale);

    ///////////////////////////////
    /// \brief
    /// Retrieves the locale for the user.
    ///
    /// \remarks
    /// The locale code can be either a 2 letter language code defined by ISO 639-1, for example 'en',
    /// or a [language]-[country] code as allowed by the HTTP Accept-Language header, for example 'en-us' or 'en-gb'.
    /// Please refer
    /// to http://www.loc.gov/standards/iso639-2/php/English_list.php and
    /// http://www.w3.org/International/questions/qa-accept-lang-locales
    /// \n
    /// The locale will only be available if SetLocale has been called on the user information
    /// object or if the user information was initialized with a session identifier.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetLocale();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetLocale();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetLocale();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Locale code
    ///
    STRING GetLocale();

    ///////////////////////////////
    /// \brief
    /// Returns an encrypted MgByteReader for the given credentials.  This byte reader
    /// can then be used to populate credentials into any resource which requires default
    /// credentials.  Some feature sources (Oracle Databases) and some drawing sources
    /// (password protected DWFs) may require default credentials to access them.
    /// See MgResourceService::SetResourceData for more information.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgByteReader GetReader();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgByteReader GetReader();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgByteReader GetReader();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Encrypted byte reader for credentials
    ///
    MgByteReader* GetReader();

EXTERNAL_API:

    ///////////////////////////////
    /// \brief
    /// Destructor
    ///
    /// \return
    /// Returns nothing
    ///
     ~MgUserInformation();

    void SetClientAgent(CREFSTRING agent);
    STRING GetClientAgent();

    void SetClientIp(CREFSTRING ip);
    STRING GetClientIp();

INTERNAL_API:

    ///////////////////////////////
    /// \brief
    /// Returns the session identifier stored
    ///
    /// \return
    /// session identifier stored
    ///
    STRING GetMgSessionId();

    ///////////////////////////////
    /// \brief
    /// Returns the username stored
    ///
    /// \return
    /// Username stored
    ///
    STRING GetUserName();

    ///////////////////////////////
    /// \brief
    /// Returns the password stored
    ///
    /// \return
    /// password stored
    ///
    STRING GetPassword();

    ///////////////////////////////
    /// \brief
    /// Returns the type of supplied credentials
    ///
    /// \return
    /// The type of supplied credentials
    ///
    UserInformationType GetType();

    ///////////////////////////////
    /// \brief
    /// Create a sessionid
    ///
    /// \return
    /// New session id
    ///
    STRING CreateMgSessionId();
    STRING CreateMgSessionId(MgSiteInfo* siteInfo);

    ///////////////////////////////
    /// \brief
    /// Clears the current sessionid
    ///
    void ClearMgSessionId();

    ///////////////////////////////
    /// \brief
    /// Get/Set API version
    /// Encoding is (Major << 16) + (Minor << 8) + Phase
    /// 1.0.0 is encoded as 0x00010000
    /// 2.1.0 is encoded as 0x00020100
    ///
    void SetApiVersion(INT32 apiVersion);
    INT32 GetApiVersion();

    ///////////////////////////////
    /// \brief
    /// Sets the user information for the current thread.  This
    /// function uses thread local storage.
    ///
    /// \param userInformation
    /// User Information to assign to the current thread
    ///
    /// \return
    /// Returns nothing
    ///
    static void SetCurrentUserInfo(MgUserInformation* userInformation);

    ///////////////////////////////
    /// \brief
    /// Gets the user information for the current thread which was
    /// set previously using SetCurrentUserInfo.  This
    /// function uses thread local storage.
    ///
    /// \return
    /// User information for currently executing thread
    ///
    /// \exception MgConnectionNotOpenException if the user information is not initialized
    ///
    static MgUserInformation* GetCurrentUserInfo();
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serialize data to TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserialize data from TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Copy constructor.
    ///
    MgUserInformation(const MgUserInformation& userInfo);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Assignment operator.
    ///
    MgUserInformation& operator=(const MgUserInformation& userInfo);

protected:

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the class Id
    ///
    /// \return
    /// The integer value
    ///
     INT32 GetClassId();

    virtual void Dispose();

private:

    STRING m_username;
    STRING m_password;
    STRING m_sessionId;
    STRING m_locale;
    UserInformationType m_type;

    STRING m_clientAgent;
    STRING m_clientIp;

    INT32 m_apiVersion;

CLASS_ID:
    static const INT32 m_cls_id = MapGuide_Service_UserInformation;
};
/// \}

#endif
