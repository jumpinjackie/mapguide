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
#include "ServerManager.h"
#include "TestServerManager.h"

const STRING TEST_LOCALE = L"en";

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestServerManager, "TestServerManager");

void TestServerManager::setUp()
{
}

void TestServerManager::tearDown()
{
}

void TestServerManager::TestStart()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nRunning Server Manager tests.\n")));
}

void TestServerManager::TestEnd()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nServer Manager tests completed.\n\n")));
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case checks to see if there is a valid MgServerManager and that
/// there is only 1 MgServerManager.
///----------------------------------------------------------------------------
void TestServerManager::TestCase_ValidServerManager()
{
    MgServerManager* pMgServerManager = MgServerManager::GetInstance();
    CPPUNIT_ASSERT(pMgServerManager != NULL);

    MgServerManager* pMgServerManager2 = MgServerManager::GetInstance();
    CPPUNIT_ASSERT(pMgServerManager == pMgServerManager2);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case checks the default locale.
///----------------------------------------------------------------------------
void TestServerManager::TestCase_GetDefaultLocale()
{
    MgServerManager* pMgServerManager = MgServerManager::GetInstance();
    CREFSTRING locale = pMgServerManager->GetDefaultLocale();

    CPPUNIT_ASSERT(wcscmp(locale.c_str(), TEST_LOCALE.c_str()) == 0);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to get the admin port.
///----------------------------------------------------------------------------
void TestServerManager::TestCase_GetAdminPort()
{
    MgServerManager* pMgServerManager = MgServerManager::GetInstance();
    INT32 port = pMgServerManager->GetAdminPort();

    // One may have/need to use a port number different than 2800 for testings.
    CPPUNIT_ASSERT(port == 2800 || port > 0);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to get the client port.
///----------------------------------------------------------------------------
void TestServerManager::TestCase_GetClientPort()
{
    MgServerManager* pMgServerManager = MgServerManager::GetInstance();
    INT32 port = pMgServerManager->GetClientPort();

    // One may have/need to use a port number different than 2801 for testings.
    CPPUNIT_ASSERT(port == 2801 || port > 0);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to get the site port.
///----------------------------------------------------------------------------
void TestServerManager::TestCase_GetSitePort()
{
    MgServerManager* pMgServerManager = MgServerManager::GetInstance();
    INT32 port = pMgServerManager->GetSitePort();

    // One may have/need to use a port number different than 2802 for testings.
    CPPUNIT_ASSERT(port == 2802 || port > 0);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to get the admin threads.
///----------------------------------------------------------------------------
void TestServerManager::TestCase_GetAdminThreads()
{
    MgServerManager* pMgServerManager = MgServerManager::GetInstance();
    INT32 threads = pMgServerManager->GetAdminThreads();

    CPPUNIT_ASSERT(threads == 5);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to get the client threads.
///----------------------------------------------------------------------------
void TestServerManager::TestCase_GetClientThreads()
{
    MgServerManager* pMgServerManager = MgServerManager::GetInstance();
    INT32 threads = pMgServerManager->GetClientThreads();

    CPPUNIT_ASSERT(threads == 10);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to get the site threads.
///----------------------------------------------------------------------------
void TestServerManager::TestCase_GetSiteThreads()
{
    MgServerManager* pMgServerManager = MgServerManager::GetInstance();
    INT32 threads = pMgServerManager->GetSiteThreads();

    CPPUNIT_ASSERT(threads == 5);
}
