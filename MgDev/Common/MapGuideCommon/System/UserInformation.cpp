//
//  Copyright (C) 2004-2010 by Autodesk, Inc.
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

#include "MapGuideCommon.h"
#include "CryptographyUtil.h"

#undef GetUserName

static ACE_thread_key_t g_threadLocalUserInformation = 0;

// for locale
static const int ExtendedLocaleSecondSepLocation = 5;
static const int ExtendedLocaleDashLocation      = 2;

MG_IMPL_DYNCREATE(MgUserInformation);

///////////////////////////////
///<summary>
///Constructs an unintialized user information object
///</summary>
///<returns>
///Nothing
///</returns>
MgUserInformation::MgUserInformation()
{
    m_type = uitNone;
    m_apiVersion = MG_API_VERSION(1,0,0);
}

///////////////////////////////
///<summary>
///Constructs an information object from a session identifier
///</summary>
MgUserInformation::MgUserInformation(CREFSTRING sessionId)
{
    SetMgSessionId(sessionId.c_str());
    m_type = uitMgSession;
    m_apiVersion = MG_API_VERSION(1,0,0);
}

///////////////////////////////
///<summary>
///Constructs credential object with a Mg user name and password.
///</summary>
///<param name="userName">
///Mg user name
///</param>
///<param name="password">
///user password
///</param>
///<returns>
///Nothing
///</returns>
MgUserInformation::MgUserInformation(CREFSTRING userName, CREFSTRING password)
{
    MgUtil::CheckXss(userName);

    m_username = userName.c_str();
    m_password = password.c_str();
    m_type = uitMg;
    m_apiVersion = MG_API_VERSION(1,0,0);
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Copy constructor.
/// </summary>
MgUserInformation::MgUserInformation(const MgUserInformation& userInfo)
{
    *this = userInfo;
}

///////////////////////////////
///<summary>
///Destructor
///</summary>
///<returns>
///Nothing
///</returns>
MgUserInformation::~MgUserInformation()
{
}

//////////////////////////////////////////////////////////////
/// <summary>
/// Assignment operator.
/// </summary>
MgUserInformation& MgUserInformation::operator=(const MgUserInformation& userInfo)
{
    if (&userInfo != this)
    {
        m_username = userInfo.m_username.c_str();
        m_password = userInfo.m_password.c_str();
        m_sessionId = userInfo.m_sessionId.c_str();
        m_locale = userInfo.m_locale.c_str();
        m_type = userInfo.m_type;
        m_clientAgent = userInfo.m_clientAgent.c_str();
        m_clientIp = userInfo.m_clientIp.c_str();
        m_apiVersion = userInfo.m_apiVersion;
    }

    return *this;
}

///////////////////////////////
///<summary>
///Initializes userInformation with a Mg user name and password.  The
///userInformation type is set to uitMg
///</summary>
///<param name="userName">
///Mg user name
///</param>
///<param name="password">
///user password
///</param>
///<returns>
///Nothing
///</returns>
void MgUserInformation::SetMgUsernamePassword(CREFSTRING userName, CREFSTRING password)
{
    MgUtil::CheckXss(userName);

    m_username = userName.c_str();
    m_password = password.c_str();
    m_type = uitMg;
}


///////////////////////////////
///<summary>
///Returns the type of supplied userInformation
///</summary>
///<returns>
///The type of supplied userInformation
///</returns>
 UserInformationType MgUserInformation::GetType() { return m_type; }

///////////////////////////////
///<summary>
/// Returns the username stored
///</summary>
///<returns>
/// Username stored
///</returns>
STRING MgUserInformation::GetUserName() { return (STRING)m_username.c_str(); }

///////////////////////////////
///<summary>
/// Returns the password stored
///</summary>
///<returns>
/// password stored
///</returns>
STRING MgUserInformation::GetPassword() { return (STRING)m_password.c_str(); }

///////////////////////////////
///<summary>
/// Returns the session identifier stored
///</summary>
STRING MgUserInformation::GetMgSessionId()
{
    return (STRING)m_sessionId.c_str();
}

///////////////////////////////
///<summary>
///Initializes user information with a Mg session identifier.
///</summary>
void MgUserInformation::SetMgSessionId(CREFSTRING sessionId)
{
    MgUtil::CheckXss(sessionId);

    size_t sepChar = sessionId.find(L"_");

    if (sepChar > 0 && sepChar < sessionId.length())
    {
        // the locale can be either in the form "en" or "en-US"
        STRING subStr = sessionId.substr(sepChar+1);
        size_t dashChar = subStr.find(L"-");
        size_t secondSepChar = subStr.find(L"_");
        if (ExtendedLocaleDashLocation == dashChar && ExtendedLocaleSecondSepLocation == secondSepChar)
        {
            SetLocale(sessionId.substr(sepChar+1, MG_EXTENDED_LOCALE_LENGTH));
        }
        else
        {
            SetLocale(sessionId.substr(sepChar+1, MG_LOCALE_LENGTH));
        }
        m_sessionId = sessionId.c_str();
    }
    else
    {
        MgStringCollection arguments;
        arguments.Add(L"1");
        arguments.Add(sessionId);

        throw new MgInvalidArgumentException(L"MgUserInformation.SetMgSessionId",
            __LINE__, __WFILE__, &arguments, L"MgInvalidSessionsId", NULL);
    }
}

///////////////////////////////
///<summary>
///Sets the desired locale for the user.
///Locale codes are 2 letters language codes defined by ISO 639-1. Please refer
///to  http://www.loc.gov/standards/iso639-2/
///</summary>
void MgUserInformation::SetLocale(CREFSTRING locale)
{
    // the locale can be either in the form "en" or "en-US"
    if (MG_LOCALE_LENGTH != locale.length() && MG_EXTENDED_LOCALE_LENGTH != locale.length())
    {
        throw new MgLengthException(L"MgUserInformation.SetLocale",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    if (MG_EXTENDED_LOCALE_LENGTH == locale.length())
    {
        size_t dashChar = locale.find(L"-");
        if (ExtendedLocaleDashLocation != dashChar)
        {
            throw new MgLengthException(L"MgUserInformation.SetLocale",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
        else
        {
            // convert the locale to lower-upper form
            m_locale = MgUtil::ToLower(locale.substr(0, 3)) + MgUtil::ToUpper(locale.substr(3));
        }
    }
    else
    {
        // two-character locales remain case insensitive for legacy behavior
        m_locale = locale.c_str();
    }
}


///////////////////////////////
///<summary>
///Retrieves the locale for the user.
///</summary>
STRING MgUserInformation::GetLocale()
{
    return (STRING)m_locale.c_str();
}

void MgUserInformation::SetClientAgent(CREFSTRING agent)
{
    m_clientAgent = agent.c_str();
}

STRING MgUserInformation::GetClientAgent()
{
    return (STRING)m_clientAgent.c_str();
}

void MgUserInformation::SetClientIp(CREFSTRING ip)
{
    // Perform Cross Site Scripting Attack validations.
    // Note that MgIpUtil::ValidateAddress is not used here because
    // an IP look up will affect peformance.
    MgUtil::CheckXss(ip);
    m_clientIp = ip.c_str();
}

STRING MgUserInformation::GetClientIp()
{
    return (STRING)m_clientIp.c_str();
}

///////////////////////////////
///<summary>
///Create a sessionid
///</summary>
STRING MgUserInformation::CreateMgSessionId()
{
    STRING uuid;
    MgUtil::GenerateUuid(uuid);

    // TODO: Pull default locale from MgConfiguration within a try/catch
    STRING locale = m_locale.empty() ? MgResources::DefaultMessageLocale : m_locale;

    uuid.append(L"_");
    uuid.append(locale);

    return uuid;
}

///////////////////////////////
///<summary>
///Create a sessionid, and encode the IP address of the site server that
///created it into the ID
///</summary>
STRING MgUserInformation::CreateMgSessionId(MgSiteInfo* siteInfo)
{
    STRING id = CreateMgSessionId();

    STRING hexAddress;
    if(siteInfo != NULL)
    {
        hexAddress = siteInfo->ToHexString();
        if(!hexAddress.empty())
        {
            id.append(L"_");
            id.append(hexAddress);
        }
    }

    return id;
}

///////////////////////////////
///<summary>
///Clears the current sessionid
///</summary>
void MgUserInformation::ClearMgSessionId()
{
    m_sessionId = L"";
}

/// <summary>
/// Self destructor
/// </summary>
/// <returns> Nothing
/// </returns>
void MgUserInformation::Dispose() { delete this; }

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgUserInformation::GetClassId() { return m_cls_id; }

MgByteReader* MgUserInformation::GetReader()
{
    throw new MgNotImplementedException(L"MgUserInformation.GetReader", __LINE__, __WFILE__, NULL, L"", NULL);
}

///////////////////////////////
///<summary>
///Sets the user information for the current thread.  This
///function uses thread local storage.
///</summary>
void MgUserInformation::SetCurrentUserInfo(MgUserInformation* userInformation)
{
    if (0 == g_threadLocalUserInformation)
    {
        // Perform Double-Checked Locking Optimization.
        ACE_MT (ACE_GUARD(ACE_Recursive_Thread_Mutex, ace_mon, *ACE_Static_Object_Lock::instance ()));
        if (0 == g_threadLocalUserInformation)
        {
            if (ACE_OS::thr_keycreate(&g_threadLocalUserInformation, NULL) < 0)
            {
                g_threadLocalUserInformation = 0;
            }
            else
            {
                ACE_OS::thr_setspecific(g_threadLocalUserInformation, NULL);
            }
        }
    }

    if (0 != g_threadLocalUserInformation)
    {
        // Clean up old one if applicable
        MgUserInformation* oldInfo = NULL;
        ACE_OS::thr_getspecific(g_threadLocalUserInformation, (void**) &oldInfo);
        SAFE_RELEASE(oldInfo);

        SAFE_ADDREF(userInformation);
        ACE_OS::thr_setspecific(g_threadLocalUserInformation, userInformation);
    }
}

///////////////////////////////
///<summary>
///Gets the user information for the current thread which was
///set previously using SetCurrentUserInfo.  This
///function uses thread local storage.
///</summary>
MgUserInformation* MgUserInformation::GetCurrentUserInfo()
{
    MgUserInformation* userInfo = NULL;
    if (0 != g_threadLocalUserInformation)
    {
        ACE_OS::thr_getspecific(g_threadLocalUserInformation, (void**) &userInfo);
    }

    if (NULL == userInfo)
    {
        // No user information should mean that we have opened the connection to the site server yet.
        throw new MgConnectionNotOpenException(L"MgSiteConnection.GetCurrentUserInfo", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    SAFE_ADDREF(userInfo);

    return userInfo;
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Serialize data to TCP/IP stream.
/// </summary>

void MgUserInformation::Serialize(MgStream* stream)
{
    STRING credentials;

    if (!m_username.empty() || !m_password.empty())
    {
        //TODO:  Add a general configuration parameter for user info encryption
        if (1)
        {
        MgCryptographyManager cryptoManager;

        credentials = cryptoManager.EncryptCredentials(m_username, m_password);
        }
        else
        {
            credentials = m_username;
            credentials.append(L"\t");
            credentials.append(m_password);
        }
    }

    Ptr<MgStreamHelper> helper = stream->GetStreamHelper();

    helper->WriteUINT32(m_type);
    helper->WriteString(credentials);
    helper->WriteString(m_sessionId);
    helper->WriteString(m_locale);
    helper->WriteString(m_clientAgent);
    helper->WriteString(m_clientIp);
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Deserialize data from TCP/IP stream.
/// </summary>

void MgUserInformation::Deserialize(MgStream* stream)
{
    UINT32 type;
    STRING credentials;

    Ptr<MgStreamHelper> helper = stream->GetStreamHelper();

    helper->GetUINT32(type);
    helper->GetString(credentials);
    helper->GetString(m_sessionId);
    helper->GetString(m_locale);
    helper->GetString(m_clientAgent);
    helper->GetString(m_clientIp);

    m_type = (UserInformationType) type;

    if (credentials.empty())
    {
        m_username = m_password = L"";
    }
    else
    {
        //TODO:  Add a general configuration parameter for user info encryption
        if (1)
        {
        MG_CRYPTOGRAPHY_TRY()

        MgCryptographyUtil cryptoUtil;
        string username, password;

        string cred = MgUtil::WideCharToMultiByte(credentials);
        cryptoUtil.DecryptCredentials(cred, username, password);

        MgUtil::MultiByteToWideChar(username, m_username);
        MgUtil::MultiByteToWideChar(password, m_password);

        MG_CRYPTOGRAPHY_CATCH_AND_THROW(L"MgUserInformation.Deserialize")
        }
        else
        {
            STRING::size_type sep = credentials.find(L"\t");
            m_username = credentials.substr(0, sep);
            m_password = credentials.substr(sep+1);
        }
    }
}


void MgUserInformation::SetApiVersion(INT32 apiVersion)
{
    m_apiVersion = apiVersion;
}

INT32 MgUserInformation::GetApiVersion()
{
    return m_apiVersion;
}
