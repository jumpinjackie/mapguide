//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
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
#include "ServiceManager.h"
#include "ServerManager.h"
#include "ServerAdminService.h"
#include "TestServerAdminService.h"
#include "CppUnitExtensions.h"

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestServerAdminService, "TestServerAdminService");

const STRING adminName = L"Administrator";
const STRING adminPass = L"admin";
const STRING userLocale = L"en";
const STRING TEST_LOCALE = L"en";

const wchar_t TestName[] = L"TestName.log";
const wchar_t TestParameters[] =L"TEST PARAMETERS";

const STRING TestServerAdminService::PackageName  = L"World.mgp";
const STRING TestServerAdminService::PackageName2 = L"World2.mgp";

void TestServerAdminService::setUp()
{
}

void TestServerAdminService::tearDown()
{
}

void TestServerAdminService::TestStart()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nRunning ServerAdmin Service tests.\n")));
}

void TestServerAdminService::TestEnd()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nServerAdmin Service tests completed.\n\n")));
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tests taking the server offline.
///----------------------------------------------------------------------------
void TestServerAdminService::TestCase_TakeOffline()
{
    try
    {
        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
        if(serviceMan == 0)
        {
            throw new MgNullReferenceException(L"TestServerAdminService.TestCase_TakeOffline", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerAdminService> pService = dynamic_cast<MgServerAdminService*>(serviceMan->RequestService(MgServiceType::ServerAdminService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestServerAdminService.TestCase_TakeOffline", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        pService->TakeOffline();
        bool bOnline = pService->IsOnline();
        CPPUNIT_ASSERT(bOnline == false);
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
/// This test case tests bringing the server online.
///----------------------------------------------------------------------------
void TestServerAdminService::TestCase_BringOnline()
{
    try
    {
        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
        if(serviceMan == 0)
        {
            throw new MgNullReferenceException(L"TestServerAdminService.TestCase_BringOnline", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerAdminService> pService = dynamic_cast<MgServerAdminService*>(serviceMan->RequestService(MgServiceType::ServerAdminService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestServerAdminService.TestCase_BringOnline", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        pService->BringOnline();
        bool bOnline = pService->IsOnline();
        CPPUNIT_ASSERT(bOnline == true);
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
/// This test case tries to get the server configuration properties
///----------------------------------------------------------------------------
void TestServerAdminService::TestCase_GetConfigurationProperties()
{
    try
    {
        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
        if(serviceMan == 0)
        {
            throw new MgNullReferenceException(L"TestServerAdminService::TestCase_GetConfigurationProperties", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerAdminService> pService = dynamic_cast<MgServerAdminService*>(serviceMan->RequestService(MgServiceType::ServerAdminService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestServerAdminService::TestCase_GetConfigurationProperties", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgPropertyCollection> pPropertyCollection1 = MgServerManager::GetInstance()->GetConfigurationProperties(L"GeneralProperties");
        Ptr<MgPropertyCollection> pPropertyCollection2 = pService->GetConfigurationProperties(L"GeneralProperties");
        CPPUNIT_ASSERT(pPropertyCollection1->GetCount() == pPropertyCollection2->GetCount());

        Ptr<MgStringProperty> pProperty1 = (MgStringProperty*)pPropertyCollection1->GetItem(L"DefaultMessageLocale");
        Ptr<MgStringProperty> pProperty2 = (MgStringProperty*)pPropertyCollection2->GetItem(L"DefaultMessageLocale");

        CPPUNIT_ASSERT(pProperty1->GetValue() == pProperty2->GetValue());
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
/// This test case tries to set the server configuration properties
///----------------------------------------------------------------------------
void TestServerAdminService::TestCase_SetConfigurationProperties()
{
    try
    {
        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
        if(serviceMan == 0)
        {
            throw new MgNullReferenceException(L"TestServerAdminService::TestCase_SetConfigurationProperties", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerAdminService> pService = dynamic_cast<MgServerAdminService*>(serviceMan->RequestService(MgServiceType::ServerAdminService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestServerAdminService::TestCase_SetConfigurationProperties", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgPropertyCollection> pPropertyCollection1 = pService->GetConfigurationProperties(L"GeneralProperties");
        Ptr<MgStringProperty> pProperty1 = (MgStringProperty*)pPropertyCollection1->GetItem(L"DefaultMessageLocale");
        STRING valueOriginal = pProperty1->GetValue();

        pProperty1->SetValue(L"fr");
        pService->SetConfigurationProperties(L"GeneralProperties", pPropertyCollection1);

        Ptr<MgPropertyCollection> pPropertyCollection2 = pService->GetConfigurationProperties(L"GeneralProperties");
        Ptr<MgStringProperty> pProperty2 = (MgStringProperty*)pPropertyCollection1->GetItem(L"DefaultMessageLocale");

        CPPUNIT_ASSERT(pProperty1->GetValue() == pProperty2->GetValue());

        // Restore original value
        pProperty1->SetValue(valueOriginal);
        pService->SetConfigurationProperties(L"GeneralProperties", pPropertyCollection1);
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
/// This test case tries to remove the server configuration properties
///----------------------------------------------------------------------------
void TestServerAdminService::TestCase_RemoveConfigurationProperties()
{
    try
    {
        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
        if(serviceMan == 0)
        {
            throw new MgNullReferenceException(L"TestServerAdminService::TestCase_RemoveConfigurationProperties", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerAdminService> pService = dynamic_cast<MgServerAdminService*>(serviceMan->RequestService(MgServiceType::ServerAdminService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestServerAdminService::TestCase_RemoveConfigurationProperties", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // add a property to unmanaged data mappings
        Ptr<MgPropertyCollection> pPropertyCollection1 = pService->GetConfigurationProperties(L"UnmanagedDataMappings");
        Ptr<MgStringProperty> pProperty1 = new MgStringProperty(L"TestCase_RemoveConfigurationProperties", L"c:\\temp");
        pPropertyCollection1->Add(pProperty1);

        pService->SetConfigurationProperties(L"UnmanagedDataMappings", pPropertyCollection1);

        // retrieve newly added property
        Ptr<MgPropertyCollection> pPropertyCollection2 = pService->GetConfigurationProperties(L"UnmanagedDataMappings");
        Ptr<MgStringProperty> pProperty2 = (MgStringProperty*)pPropertyCollection2->GetItem(L"TestCase_RemoveConfigurationProperties");

        CPPUNIT_ASSERT(pProperty2->GetValue().compare(L"c:\\temp") == 0);

        // remove newly added property
        Ptr<MgPropertyCollection> pPropertyCollectionRemove = new MgPropertyCollection();
        Ptr<MgStringProperty> pPropertyRemove = new MgStringProperty(L"TestCase_RemoveConfigurationProperties", L"");
        pPropertyCollectionRemove->Add(pPropertyRemove);

        pService->RemoveConfigurationProperties(L"UnmanagedDataMappings", pPropertyCollectionRemove);

        // check for the removed property
        Ptr<MgPropertyCollection> pPropertyCollection3 = pService->GetConfigurationProperties(L"UnmanagedDataMappings");

        CPPUNIT_ASSERT(!pPropertyCollection3->Contains(L"TestCase_RemoveConfigurationProperties"));
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
/// This test case clears all of the logs
///----------------------------------------------------------------------------
void TestServerAdminService::TestCase_ClearLog()
{
    try
    {
        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
        if (serviceMan == 0)
        {
            throw new MgNullReferenceException(L"TestServerAdminService.TestCase_ClearLog", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerAdminService> pService = dynamic_cast<MgServerAdminService*>(serviceMan->RequestService(MgServiceType::ServerAdminService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestServerAdminService.TestCase_ClearLog", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        CPPUNIT_ASSERT(pService->ClearLog(MgLogFileType::Access));
        CPPUNIT_ASSERT(pService->ClearLog(MgLogFileType::Admin));
        CPPUNIT_ASSERT(pService->ClearLog(MgLogFileType::Authentication));
        CPPUNIT_ASSERT(pService->ClearLog(MgLogFileType::Error));
        CPPUNIT_ASSERT(pService->ClearLog(MgLogFileType::Session));
        CPPUNIT_ASSERT(pService->ClearLog(MgLogFileType::Trace));
        CPPUNIT_ASSERT_THROW_MG(pService->ClearLog(L"Invalid"), MgInvalidArgumentException*);
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
/// This test case gets the entire contents of each of the logs.  This checks
/// whether an MgByteReader was returned or not
///----------------------------------------------------------------------------
void TestServerAdminService::TestCase_GetLog()
{
    try
    {
        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
        if (serviceMan == 0)
        {
            throw new MgNullReferenceException(L"TestServerAdminService.TestCase_GetLog", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerAdminService> pService = dynamic_cast<MgServerAdminService*>(serviceMan->RequestService(MgServiceType::ServerAdminService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestServerAdminService.TestCase_GetLog", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgByteReader> byteReader;

        byteReader = pService->GetLog(MgLogFileType::Access);
        CPPUNIT_ASSERT(byteReader != NULL);

        byteReader = pService->GetLog(MgLogFileType::Admin);
        CPPUNIT_ASSERT(byteReader != NULL);

        byteReader = pService->GetLog(MgLogFileType::Authentication);
        CPPUNIT_ASSERT(byteReader != NULL);

        byteReader = pService->GetLog(MgLogFileType::Error);
        CPPUNIT_ASSERT(byteReader != NULL);

        byteReader = pService->GetLog(MgLogFileType::Session);
        CPPUNIT_ASSERT(byteReader != NULL);

        byteReader = pService->GetLog(MgLogFileType::Trace);
        CPPUNIT_ASSERT(byteReader != NULL);

        CPPUNIT_ASSERT_THROW_MG(pService->GetLog(L"Invalid"), MgInvalidArgumentException*);
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
/// This test case gets the last 3 entries of each of the logs.  Checks to make
/// sure an MgByteReader is returned.
///----------------------------------------------------------------------------
void TestServerAdminService::TestCase_GetLogEntries()
{
    try
    {
        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
        if(serviceMan == 0)
        {
            throw new MgNullReferenceException(L"TestServerAdminService.TestCase_GetLogEntries", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerAdminService> pService = dynamic_cast<MgServerAdminService*>(serviceMan->RequestService(MgServiceType::ServerAdminService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestServerAdminService.TestCase_GetLogEntries", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgByteReader> byteReader;

        byteReader = pService->GetLog(MgLogFileType::Access, 3);
        CPPUNIT_ASSERT(byteReader != NULL);

        byteReader = pService->GetLog(MgLogFileType::Admin, 3);
        CPPUNIT_ASSERT(byteReader != NULL);

        byteReader = pService->GetLog(MgLogFileType::Authentication, 3);
        CPPUNIT_ASSERT(byteReader != NULL);

        byteReader = pService->GetLog(MgLogFileType::Error, 3);
        CPPUNIT_ASSERT(byteReader != NULL);

        byteReader = pService->GetLog(MgLogFileType::Session, 3);
        CPPUNIT_ASSERT(byteReader != NULL);

        byteReader = pService->GetLog(MgLogFileType::Trace, 3);
        CPPUNIT_ASSERT(byteReader != NULL);

        CPPUNIT_ASSERT_THROW_MG(pService->GetLog(MgLogFileType::Access, -1), MgArgumentOutOfRangeException*);
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
/// This test case gets the entries of each log between 2 dates.  Checks to make
/// sure an MgByteReader is returned.
///----------------------------------------------------------------------------
void TestServerAdminService::TestCase_GetLogByDate()
{
    try
    {
        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
        if(serviceMan == 0)
        {
            throw new MgNullReferenceException(L"TestServerAdminService.TestCase_GetLogEntries", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerAdminService> pService = dynamic_cast<MgServerAdminService*>(serviceMan->RequestService(MgServiceType::ServerAdminService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestServerAdminService.TestCase_GetLogEntries", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgByteReader> byteReader;
        Ptr<MgDateTime> dateTime1 = new MgDateTime(2005, 3, 10, 10, 0, 0, 0);
        Ptr<MgDateTime> dateTime2 = new MgDateTime(2005, 3, 10, 15, 0, 0, 0);
        Ptr<MgDateTime> dateTime3 = new MgDateTime(2005, 3, 11, 10, 0, 0, 1);
        Ptr<MgDateTime> invalidDateTime = new MgDateTime(2005, 3, 11);

        byteReader = pService->GetLog(MgLogFileType::Access, dateTime1, dateTime2);
        CPPUNIT_ASSERT(byteReader != NULL);

        byteReader = pService->GetLog(MgLogFileType::Admin, dateTime1, dateTime2);
        CPPUNIT_ASSERT(byteReader != NULL);

        byteReader = pService->GetLog(MgLogFileType::Authentication, dateTime1, dateTime2);
        CPPUNIT_ASSERT(byteReader != NULL);

        byteReader = pService->GetLog(MgLogFileType::Error, dateTime1, dateTime2);
        CPPUNIT_ASSERT(byteReader != NULL);

        byteReader = pService->GetLog(MgLogFileType::Session, dateTime1, dateTime2);
        CPPUNIT_ASSERT(byteReader != NULL);

        byteReader = pService->GetLog(MgLogFileType::Trace, dateTime1, dateTime2);
        CPPUNIT_ASSERT(byteReader != NULL);

        CPPUNIT_ASSERT_THROW_MG(pService->GetLog(MgLogFileType::Access, dateTime1, NULL), MgNullArgumentException*);
        CPPUNIT_ASSERT_THROW_MG(pService->GetLog(MgLogFileType::Access, dateTime2, dateTime1), MgInvalidArgumentException*);
        CPPUNIT_ASSERT_THROW_MG(pService->GetLog(MgLogFileType::Access, dateTime1, dateTime3), MgInvalidArgumentException*);
        CPPUNIT_ASSERT_THROW_MG(pService->GetLog(MgLogFileType::Access, invalidDateTime, dateTime2), MgInvalidArgumentException*);
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
/// This test case tries to get the server information properties
///----------------------------------------------------------------------------
void TestServerAdminService::TestCase_GetInformationProperties()
{
    try
    {
        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
        if(serviceMan == 0)
        {
            throw new MgNullReferenceException(L"TestServerAdminService::TestCase_GetInformationProperties", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerAdminService> pService = dynamic_cast<MgServerAdminService*>(serviceMan->RequestService(MgServiceType::ServerAdminService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestServerAdminService::TestCase_GetInformationProperties", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgPropertyCollection> pPropertyCollection = MgServerManager::GetInstance()->GetInformationProperties();
        CPPUNIT_ASSERT(0 < pPropertyCollection->GetCount());
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
/// This test case enumerates the packages in the specified package folder
///----------------------------------------------------------------------------
void TestServerAdminService::TestCase_EnumeratePackages()
{
    try
    {
        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
        if(serviceMan == 0)
        {
            throw new MgNullReferenceException(L"TestServerAdminService::TestCase_EnumeratePackages", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerAdminService> pService = dynamic_cast<MgServerAdminService*>(serviceMan->RequestService(MgServiceType::ServerAdminService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestServerAdminService::TestCase_EnumeratePackages", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgPropertyCollection> pPropertyCollection1 = pService->GetConfigurationProperties(L"ResourceServiceProperties");
        Ptr<MgStringProperty> pProperty1 = (MgStringProperty*)pPropertyCollection1->GetItem(L"PackagesPath");
        STRING valueOriginal = pProperty1->GetValue();

        pProperty1->SetValue(L"../UnitTestFiles/");
        pService->SetConfigurationProperties(L"ResourceServiceProperties", pPropertyCollection1);

        Ptr<MgPropertyCollection> pPropertyCollection2 = pService->GetConfigurationProperties(L"ResourceServiceProperties");
        Ptr<MgStringProperty> pProperty2 = (MgStringProperty*)pPropertyCollection1->GetItem(L"PackagesPath");

        Ptr<MgStringCollection> packages = pService->EnumeratePackages();

        // Restore original value
        pProperty1->SetValue(valueOriginal);
        pService->SetConfigurationProperties(L"ResourceServiceProperties", pPropertyCollection1);

        CPPUNIT_ASSERT(packages->Contains(TestServerAdminService::PackageName));
    }
    catch(MgException* e)
    {
        STRING message = e->GetMessage(TEST_LOCALE);
        message += e->GetStackTrace(TEST_LOCALE);
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
/// This test case loads a resource package
///----------------------------------------------------------------------------
void TestServerAdminService::TestCase_LoadPackage()
{
    try
    {
        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
        if(serviceMan == 0)
        {
            throw new MgNullReferenceException(L"TestServerAdminService::TestCase_LoadPackage", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerAdminService> pService = dynamic_cast<MgServerAdminService*>(serviceMan->RequestService(MgServiceType::ServerAdminService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestServerAdminService::TestCase_LoadPackage", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgPropertyCollection> pPropertyCollection1 = pService->GetConfigurationProperties(L"ResourceServiceProperties");
        Ptr<MgStringProperty> pProperty1 = (MgStringProperty*)pPropertyCollection1->GetItem(L"PackagesPath");
        STRING valueOriginal = pProperty1->GetValue();

        pProperty1->SetValue(L"../UnitTestFiles/");
        pService->SetConfigurationProperties(L"ResourceServiceProperties", pPropertyCollection1);

        Ptr<MgPropertyCollection> pPropertyCollection2 = pService->GetConfigurationProperties(L"ResourceServiceProperties");
        Ptr<MgStringProperty> pProperty2 = (MgStringProperty*)pPropertyCollection1->GetItem(L"PackagesPath");

        Ptr<MgUserInformation> userInfo;

        //Sets the user information for the current thread to be administrator
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(adminName, adminPass);
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);

            pService->LoadPackage(TestServerAdminService::PackageName);
        }

        // Restore original value
        pProperty1->SetValue(valueOriginal);
        pService->SetConfigurationProperties(L"ResourceServiceProperties", pPropertyCollection1);
    }
    catch(MgException* e)
    {
        STRING message = e->GetMessage(TEST_LOCALE);
        message += e->GetStackTrace(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}

///////////////////////////////////////////////////////////////////////////////
/// Test Case Description:
///
/// Creates a package from the specified resource, and then saves it into
/// the specified name.
///
void TestServerAdminService::TestCase_MakePackage()
{
    try
    {
        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
        if(serviceMan == 0)
        {
            throw new MgNullReferenceException(L"TestServerAdminService::TestCase_MakePackage", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerAdminService> pService = dynamic_cast<MgServerAdminService*>(serviceMan->RequestService(MgServiceType::ServerAdminService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestServerAdminService::TestCase_MakePackage", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgPropertyCollection> pPropertyCollection1 = pService->GetConfigurationProperties(L"ResourceServiceProperties");
        Ptr<MgStringProperty> pProperty1 = (MgStringProperty*)pPropertyCollection1->GetItem(L"PackagesPath");
        STRING valueOriginal = pProperty1->GetValue();

        pProperty1->SetValue(L"../UnitTestFiles/");
        pService->SetConfigurationProperties(L"ResourceServiceProperties", pPropertyCollection1);

        Ptr<MgPropertyCollection> pPropertyCollection2 = pService->GetConfigurationProperties(L"ResourceServiceProperties");
        Ptr<MgStringProperty> pProperty2 = (MgStringProperty*)pPropertyCollection1->GetItem(L"PackagesPath");

        Ptr<MgUserInformation> userInfo;

        //Sets the user information for the current thread to be administrator
        MgUserInformation::SetCurrentUserInfo(NULL);
        userInfo = new MgUserInformation(adminName, adminPass);
        if (userInfo != NULL)
        {
            userInfo->SetLocale(userLocale);
            MgUserInformation::SetCurrentUserInfo(userInfo);

            MgResourceIdentifier resource(L"Library://UnitTests/");

            pService->MakePackage(&resource,
                TestServerAdminService::PackageName2, L"Unit Test Package");
        }

        Ptr<MgStringCollection> packages = pService->EnumeratePackages();

        // Restore original value
        pProperty1->SetValue(valueOriginal);
        pService->SetConfigurationProperties(L"ResourceServiceProperties", pPropertyCollection1);

        CPPUNIT_ASSERT(packages->Contains(TestServerAdminService::PackageName2));
    }
    catch(MgFileIoException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        ACE_DEBUG((LM_INFO, ACE_TEXT("\nMgFileIoException - Possible file permission error.\nError: %W\n"), message.c_str()));
    }
    catch(MgException* e)
    {
        STRING message = e->GetMessage(TEST_LOCALE);
        message += e->GetStackTrace(TEST_LOCALE);
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
/// This test case gets a log associated with a resource package
///----------------------------------------------------------------------------
void TestServerAdminService::TestCase_GetPackageLog()
{
    try
    {
        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
        if(serviceMan == 0)
        {
            throw new MgNullReferenceException(L"TestServerAdminService::TestCase_GetPackageLog", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerAdminService> pService = dynamic_cast<MgServerAdminService*>(serviceMan->RequestService(MgServiceType::ServerAdminService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestServerAdminService::TestCase_GetPackageLog", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgPropertyCollection> pPropertyCollection1 = pService->GetConfigurationProperties(L"ResourceServiceProperties");
        Ptr<MgStringProperty> pProperty1 = (MgStringProperty*)pPropertyCollection1->GetItem(L"PackagesPath");
        STRING valueOriginal = pProperty1->GetValue();

        pProperty1->SetValue(L"../UnitTestFiles/");
        pService->SetConfigurationProperties(L"ResourceServiceProperties", pPropertyCollection1);

        Ptr<MgPropertyCollection> pPropertyCollection2 = pService->GetConfigurationProperties(L"ResourceServiceProperties");
        Ptr<MgStringProperty> pProperty2 = (MgStringProperty*)pPropertyCollection1->GetItem(L"PackagesPath");

        Ptr<MgByteReader> byteReader = pService->GetPackageLog(TestServerAdminService::PackageName);
        STRING contents = byteReader->ToString();

        // Restore original value
        pProperty1->SetValue(valueOriginal);
        pService->SetConfigurationProperties(L"ResourceServiceProperties", pPropertyCollection1);

        CPPUNIT_ASSERT(!contents.empty());
    }
    catch(MgException* e)
    {
        STRING message = e->GetMessage(TEST_LOCALE);
        message += e->GetStackTrace(TEST_LOCALE);
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
/// This test case gets a status associated with a resource package
///----------------------------------------------------------------------------
void TestServerAdminService::TestCase_GetPackageStatus()
{
    try
    {
        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
        if(serviceMan == 0)
        {
            throw new MgNullReferenceException(L"TestServerAdminService::TestCase_GetPackageStatus", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerAdminService> pService = dynamic_cast<MgServerAdminService*>(serviceMan->RequestService(MgServiceType::ServerAdminService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestServerAdminService::TestCase_GetPackageStatus", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgPropertyCollection> pPropertyCollection1 = pService->GetConfigurationProperties(L"ResourceServiceProperties");
        Ptr<MgStringProperty> pProperty1 = (MgStringProperty*)pPropertyCollection1->GetItem(L"PackagesPath");
        STRING valueOriginal = pProperty1->GetValue();

        pProperty1->SetValue(L"../UnitTestFiles/");
        pService->SetConfigurationProperties(L"ResourceServiceProperties", pPropertyCollection1);

        Ptr<MgPropertyCollection> pPropertyCollection2 = pService->GetConfigurationProperties(L"ResourceServiceProperties");
        Ptr<MgStringProperty> pProperty2 = (MgStringProperty*)pPropertyCollection1->GetItem(L"PackagesPath");

        Ptr<MgPackageStatusInformation> statusInfo = pService->GetPackageStatus(TestServerAdminService::PackageName);
        STRING statusCode = statusInfo->GetStatusCode();

        // Restore original value
        pProperty1->SetValue(valueOriginal);
        pService->SetConfigurationProperties(L"ResourceServiceProperties", pPropertyCollection1);

        CPPUNIT_ASSERT(statusCode == MgPackageStatusCode::Succeeded);
    }
    catch(MgException* e)
    {
        STRING message = e->GetMessage(TEST_LOCALE);
        message += e->GetStackTrace(TEST_LOCALE);
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
/// This test case gets deletes a package and its associated log
///----------------------------------------------------------------------------
void TestServerAdminService::TestCase_DeletePackage()
{
    try
    {
        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
        if(serviceMan == 0)
        {
            throw new MgNullReferenceException(L"TestServerAdminService::TestCase_DeletePackage", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerAdminService> pService = dynamic_cast<MgServerAdminService*>(serviceMan->RequestService(MgServiceType::ServerAdminService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestServerAdminService::TestCase_DeletePackage", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgPropertyCollection> pPropertyCollection1 = pService->GetConfigurationProperties(L"ResourceServiceProperties");
        Ptr<MgStringProperty> pProperty1 = (MgStringProperty*)pPropertyCollection1->GetItem(L"PackagesPath");
        STRING valueOriginal = pProperty1->GetValue();

        pProperty1->SetValue(L"../UnitTestFiles/");
        pService->SetConfigurationProperties(L"ResourceServiceProperties", pPropertyCollection1);

        Ptr<MgPropertyCollection> pPropertyCollection2 = pService->GetConfigurationProperties(L"ResourceServiceProperties");
        Ptr<MgStringProperty> pProperty2 = (MgStringProperty*)pPropertyCollection1->GetItem(L"PackagesPath");

        pService->DeletePackage(TestServerAdminService::PackageName2);

        Ptr<MgStringCollection> packages = pService->EnumeratePackages();

        // Restore original value
        pProperty1->SetValue(valueOriginal);
        pService->SetConfigurationProperties(L"ResourceServiceProperties", pPropertyCollection1);

        CPPUNIT_ASSERT(!packages->Contains(TestServerAdminService::PackageName2));
    }
    catch(MgException* e)
    {
        STRING message = e->GetMessage(TEST_LOCALE);
        message += e->GetStackTrace(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch(...)
    {
        throw;
    }
}


// TODO: add these tests once methods implemented

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case registers a service on a server
///----------------------------------------------------------------------------
//void TestServerAdminService::TestCase_RegisterServicesOnServer()
//{
//    try
//    {
//        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
//        if(serviceMan == 0)
//        {
//            throw new MgNullReferenceException(L"TestServerAdminService::TestCase_GetInformationProperties", __LINE__, __WFILE__, NULL, L"", NULL);
//        }
//
//        Ptr<MgServerAdminService> pService = dynamic_cast<MgServerAdminService*>(serviceMan->RequestService(MgServiceType::ServerAdminService));
//        if (pService == 0)
//        {
//            throw new MgServiceNotAvailableException(L"TestServerAdminService::TestCase_GetInformationProperties", __LINE__, __WFILE__, NULL, L"", NULL);
//        }
//
//    }
//    catch(MgException* e)
//    {
//        STRING message = e->GetDetails(TEST_LOCALE);
//        SAFE_RELEASE(e);
//        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
//    }
//    catch(...)
//    {
//        throw;
//    }
//}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case unregisters a services on a server
///----------------------------------------------------------------------------
//void TestServerAdminService::TestCase_UnregisterServicesOnServer()
//{
//        try
//    {
//        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
//        if(serviceMan == 0)
//        {
//            throw new MgNullReferenceException(L"TestServerAdminService::TestCase_GetInformationProperties", __LINE__, __WFILE__, NULL, L"", NULL);
//        }
//
//        Ptr<MgServerAdminService> pService = dynamic_cast<MgServerAdminService*>(serviceMan->RequestService(MgServiceType::ServerAdminService));
//        if (pService == 0)
//        {
//            throw new MgServiceNotAvailableException(L"TestServerAdminService::TestCase_GetInformationProperties", __LINE__, __WFILE__, NULL, L"", NULL);
//        }
//    }
//    catch(MgException* e)
//    {
//        STRING message = e->GetDetails(TEST_LOCALE);
//        SAFE_RELEASE(e);
//        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
//    }
//    catch(...)
//    {
//        throw;
//    }
//}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case gets the entire contents of a document.  This checks
/// whether an MgByteReader was returned or not
///----------------------------------------------------------------------------
void TestServerAdminService::TestCase_GetDocument()
{
    try
    {
        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
        if (serviceMan == 0)
        {
            throw new MgNullReferenceException(L"TestServerAdminService.TestCase_GetDocument", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerAdminService> pService = dynamic_cast<MgServerAdminService*>(serviceMan->RequestService(MgServiceType::ServerAdminService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestServerAdminService.TestCase_GetDocument", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgByteReader> byteReader;

        // Give invalid document
        CPPUNIT_ASSERT_THROW_MG(pService->GetDocument(L""), MgInvalidArgumentException*);
        CPPUNIT_ASSERT_THROW_MG(pService->GetDocument(L":test"), MgInvalidArgumentException*);
        CPPUNIT_ASSERT_THROW_MG(pService->GetDocument(L"Wms:..test"), MgInvalidArgumentException*);
        CPPUNIT_ASSERT_THROW_MG(pService->GetDocument(L"Wms:test\\"), MgInvalidArgumentException*);
        CPPUNIT_ASSERT_THROW_MG(pService->GetDocument(L"Wms:/test"), MgInvalidArgumentException*);

        // Check to see if the path exists before trying to get document
        STRING path;

        MgConfiguration* pConfiguration = MgConfiguration::GetInstance();
        pConfiguration->GetStringValue(MgConfigProperties::GeneralPropertiesSection, L"WmsDocumentPath", path, L"");
        if(!path.empty())
        {
            MgFileUtil::AppendSlashToEndOfPath(path);

            if(MgFileUtil::PathnameExists(path))
            {
                byteReader = pService->GetDocument(L"Wms:1.1.1");
                CPPUNIT_ASSERT(byteReader != NULL);

                string test = MgUtil::GetTextFromReader(byteReader);
                CPPUNIT_ASSERT(test.length() > 0);
            }
        }
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
/// This test case sets the entire contents of a document.
///----------------------------------------------------------------------------
void TestServerAdminService::TestCase_SetDocument()
{
    try
    {
        MgServiceManager* serviceMan = MgServiceManager::GetInstance();
        if (serviceMan == 0)
        {
            throw new MgNullReferenceException(L"TestServerAdminService.TestCase_SetDocument", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        Ptr<MgServerAdminService> pService = dynamic_cast<MgServerAdminService*>(serviceMan->RequestService(MgServiceType::ServerAdminService));
        if (pService == 0)
        {
            throw new MgServiceNotAvailableException(L"TestServerAdminService.TestCase_SetDocument", __LINE__, __WFILE__, NULL, L"", NULL);
        }

        // Check to see if the path exists before trying to set document
        STRING path;

        MgConfiguration* pConfiguration = MgConfiguration::GetInstance();
        pConfiguration->GetStringValue(MgConfigProperties::GeneralPropertiesSection, L"WmsDocumentPath", path, L"");
        if(!path.empty())
        {
            MgFileUtil::AppendSlashToEndOfPath(path);

            if(MgFileUtil::PathnameExists(path))
            {
                STRING document = L"../UnitTestFiles/1.1.1.mgd";
                Ptr<MgByteSource> ptrSource = new MgByteSource(document);
                Ptr<MgByteReader> byteReader = ptrSource->GetReader();

                pService->SetDocument(L"Wms:1.1.1", byteReader);
            }
        }
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
