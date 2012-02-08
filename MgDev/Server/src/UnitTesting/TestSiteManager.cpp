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

#include "MapGuideCommon.h"
#include "TestSiteManager.h"
#include "FoundationDefs.h"
const STRING TEST_SESSION_ID = L"48cb0286-0000-1000-8001-005056c00008_en_MTExLjEyMi4xMzMuMTQ0045708AE0D05";
const STRING TEST_TARGET = L"111.122.133.144";
const STRING TEST_SUPPORT_SERVER = L"144.133.122.111";
const INT32 TEST_SITE_PORT = 1111;
const INT32 TEST_CLIENT_PORT = 2222;
const INT32 TEST_ADMIN_PORT = 3333;
const STRING TEST_LOCALE = L"en";

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestSiteManager, "TestSiteManager");

void TestSiteManager::setUp()
{
}

void TestSiteManager::tearDown()
{
}

void TestSiteManager::TestStart()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nRunning Site Manager tests.\n")));
}

void TestSiteManager::TestEnd()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nSite Manager tests completed.\n\n")));
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case checks to see if there is a valid MgSiteManager and that
/// there is only 1 MgSiteManager.
///----------------------------------------------------------------------------
void TestSiteManager::TestCase_ValidSiteManager()
{
    MgSiteManager* pMgSiteManager = MgSiteManager::GetInstance();
    CPPUNIT_ASSERT(pMgSiteManager != NULL);

    MgSiteManager* pMgSiteManager2 = MgSiteManager::GetInstance();
    CPPUNIT_ASSERT(pMgSiteManager == pMgSiteManager2);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// Get the MgConnectionProperties for a site connection
///----------------------------------------------------------------------------
void TestSiteManager::TestCase_GetSiteConnectionProperties()
{
    try
    {
        // Get the site manager instance
        MgSiteManager* pSiteManager = MgSiteManager::GetInstance();

        // Create admin user info
        Ptr<MgUserInformation> userInfo = new MgUserInformation(L"Administrator", L"admin");

        // Get connection properties
        Ptr<MgConnectionProperties> connProps = pSiteManager->GetConnectionProperties(userInfo, MgSiteInfo::Site, false);

        // Verify that we received connection properties
        CPPUNIT_ASSERT(connProps != NULL);

    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// Get the MgConnectionProperties for a site connection when the user info
/// contains a session ID
///----------------------------------------------------------------------------
void TestSiteManager::TestCase_GetSiteConnectionPropertiesWithSession()
{
    try
    {
        // Get the site manager instance
        MgSiteManager* pSiteManager = MgSiteManager::GetInstance();

        // Create admin user info
        Ptr<MgUserInformation> userInfo = new MgUserInformation(L"Administrator", L"admin");
        userInfo->SetMgSessionId(TEST_SESSION_ID);

        // Get connection properties, allowing overrides from the session ID
        Ptr<MgConnectionProperties> connPropsSite = pSiteManager->GetConnectionProperties(userInfo, MgSiteInfo::Site, true);
        Ptr<MgConnectionProperties> connPropsClient = pSiteManager->GetConnectionProperties(userInfo, MgSiteInfo::Client, true);
        Ptr<MgConnectionProperties> connPropsAdmin = pSiteManager->GetConnectionProperties(userInfo, MgSiteInfo::Admin, true);

        // Verify that the connection properties match those in the session ID
        CPPUNIT_ASSERT(connPropsSite != NULL && connPropsClient != NULL && connPropsAdmin != NULL);
        CPPUNIT_ASSERT(connPropsSite->GetTarget() == TEST_TARGET);
        CPPUNIT_ASSERT(connPropsSite->GetPort() == TEST_SITE_PORT);
        CPPUNIT_ASSERT(connPropsClient->GetPort() == TEST_CLIENT_PORT);
        CPPUNIT_ASSERT(connPropsAdmin->GetPort() == TEST_ADMIN_PORT);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// Get the MgConnectionProperties for a site connection when the user info
/// contains a session ID, but that ID is not to be used to override the
/// connection parameters
///----------------------------------------------------------------------------
void TestSiteManager::TestCase_GetSiteConnectionPropertiesIgnoreSession()
{
    try
    {
        // Get the site manager instance
        MgSiteManager* pSiteManager = MgSiteManager::GetInstance();

        // Create admin user info
        Ptr<MgUserInformation> userInfo = new MgUserInformation(L"Administrator", L"admin");
        userInfo->SetMgSessionId(TEST_SESSION_ID);

        // Get connection properties without allowing overrides from the session ID
        Ptr<MgConnectionProperties> connPropsSite = pSiteManager->GetConnectionProperties(userInfo, MgSiteInfo::Site, false);

        CPPUNIT_ASSERT(connPropsSite != NULL);
        CPPUNIT_ASSERT(connPropsSite->GetTarget() != TEST_TARGET);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// Get a connection to a site specified in an MgSiteInfo object
///----------------------------------------------------------------------------
void TestSiteManager::TestCase_GetSpecificSiteConnectionProperties()
{
    try
    {
        // Get the site manager instance
        MgSiteManager* pSiteManager = MgSiteManager::GetInstance();

        // Create admin user info
        Ptr<MgUserInformation> userInfo = new MgUserInformation(L"Administrator", L"admin");

        // Create MgSiteInfo object
        Ptr<MgSiteInfo> siteInfo = new MgSiteInfo(TEST_TARGET, TEST_SITE_PORT, TEST_CLIENT_PORT, TEST_ADMIN_PORT);

        // Get connection properties for the specified site
        Ptr<MgConnectionProperties> connPropsSite = pSiteManager->GetConnectionProperties(userInfo, siteInfo, MgSiteInfo::Admin);

        CPPUNIT_ASSERT(connPropsSite != NULL);
        CPPUNIT_ASSERT(connPropsSite->GetTarget() == TEST_TARGET);
        CPPUNIT_ASSERT(connPropsSite->GetPort() == TEST_ADMIN_PORT);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// Get a connection to a support server. The support server IP is specified
/// in the request, but the port is the one used by the site server.
///----------------------------------------------------------------------------
void TestSiteManager::TestCase_GetSupportServerConnectionProperties()
{
    try
    {
        // Get the site manager instance
        MgSiteManager* pSiteManager = MgSiteManager::GetInstance();

        // Create admin user info
        Ptr<MgUserInformation> userInfo = new MgUserInformation(L"Administrator", L"admin");
        userInfo->SetMgSessionId(TEST_SESSION_ID);

        // Create MgSiteInfo object
        Ptr<MgSiteInfo> siteInfo = new MgSiteInfo(TEST_TARGET, TEST_SITE_PORT, TEST_CLIENT_PORT, TEST_ADMIN_PORT);

        // Get connection properties for the specified site
        Ptr<MgConnectionProperties> connPropsSite = pSiteManager->GetSupportServerConnectionProperties(TEST_SUPPORT_SERVER, userInfo, MgSiteInfo::Site);

        // The port should come from the site stored in the session ID. The target should be the support server IP.
        CPPUNIT_ASSERT(connPropsSite != NULL);
        CPPUNIT_ASSERT(connPropsSite->GetTarget() == TEST_SUPPORT_SERVER);
        CPPUNIT_ASSERT(connPropsSite->GetPort() == TEST_SITE_PORT);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// Get an MgSiteInfo object representing the connection properties for this
/// site server
///----------------------------------------------------------------------------
void TestSiteManager::TestCase_GetSiteInfo()
{
    try
    {
        // Get the site manager instance
        MgSiteManager* pSiteManager = MgSiteManager::GetInstance();

        // Get the number of site servers
        Ptr<MgSiteInfo> siteInfo = pSiteManager->GetSiteInfo(0);

        // Verify that we received a valid site info object
        CPPUNIT_ASSERT(siteInfo != NULL);
        CPPUNIT_ASSERT(siteInfo->GetTarget() != L"");
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// Get the number of site servers configured in the INI file. Should always be
/// 1 for serverconfig.ini.
///----------------------------------------------------------------------------
void TestSiteManager::TestCase_GetSiteCount()
{
    try
    {
        // Get the site manager instance
        MgSiteManager* pSiteManager = MgSiteManager::GetInstance();

        // Get the number of site servers
        INT32 numSites = pSiteManager->GetSiteCount();

        // The number of sites should always be 1 on the server.
        // It can be greater than 1 in the web tier.
        CPPUNIT_ASSERT(numSites == 1);
    }
    catch(MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}
