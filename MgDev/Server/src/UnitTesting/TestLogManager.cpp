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
#include "TestLogManager.h"
#include "CppUnitExtensions.h"
#include "LogManager.h"
#include "TestLogManagerThread.h"
#include "FoundationDefs.h"
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestLogManager, "TestLogManager");

const wchar_t TestName[] = L"TestName.log";
const wchar_t TestName2[] = L"TestName2.log";
const wchar_t NewTestName[] = L"SuperCoolNewTestName.log";
const wchar_t JunkName[] = L"junkfile.log";
const wchar_t JunkName2[] = L"junkfile2.log";

const wchar_t DynamicTestName[] = L"TestName-%y%m%d.log";
const wchar_t DynamicTestNameDate1[] = L"TestName-050307.log";
const wchar_t DynamicTestNameDate2[] = L"TestName-050308.log";
const wchar_t TestParameters[] =L"TEST PARAMETERS";

const STRING TEST_LOCALE = L"en";

#ifdef _WIN32
const STRING Entry1 = L"<2005-03-07T16:51:13> Entry 1\r\n";
const STRING Entry2 = L"<2005-03-07T16:53:15> Entry 2\r\n";
const STRING Entry3 = L"<2005-03-07T16:54:19> Entry 3\r\n";
const STRING Entry4 = L"<2005-03-07T16:55:10> Entry 4\r\n";
const STRING LastEntry = L"<2005-03-08T00:57:08> Entry 5\r\n";
#else
const STRING Entry1 = L"<2005-03-07T16:51:13> Entry 1\n";
const STRING Entry2 = L"<2005-03-07T16:53:15> Entry 2\n";
const STRING Entry3 = L"<2005-03-07T16:54:19> Entry 3\n";
const STRING Entry4 = L"<2005-03-07T16:55:10> Entry 4\n";
const STRING LastEntry = L"<2005-03-08T00:57:08> Entry 5\n";
#endif

void TestLogManager::setUp()
{
}

void TestLogManager::tearDown()
{
}

void TestLogManager::TestStart()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nRunning Log Manager tests.\n")));
}

void TestLogManager::TestEnd()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nLog Manager tests completed.\n\n")));
}

bool TestLogManager::CreateFile(STRING filename, STRING contents)
{
    FILE* file = NULL;
    file = ::fopen(MG_WCHAR_TO_CHAR(filename), "wb");

    if (file)
    {
        ::fputs(MG_WCHAR_TO_CHAR(contents), file);
        fclose(file);
        return true;
    }
    else
    {
        return false;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case checks to see if there is a valid MgLogManager and that
/// there is only 1 MgLogManager.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_ValidLogManager()
{
    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    CPPUNIT_ASSERT(pMgLogManager != NULL);

    MgLogManager* pMgLogManager2 = MgLogManager::GetInstance();
    CPPUNIT_ASSERT(pMgLogManager == pMgLogManager2);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case checks the logs path.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_GetLogsPath()
{
    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    CREFSTRING path = pMgLogManager->GetLogsPath();

    CPPUNIT_ASSERT(path.length() > 0);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case hits the log manager with multiple threads to ensure that
/// it encounters no deadlocking.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_TestForDeadLock()
{
    try
    {
        ACE_Thread_Manager manager;
        TestLogManagerThread test(manager);
        manager.wait();
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case enumerates the log files that are currently located in the
/// logs directory
///----------------------------------------------------------------------------
void TestLogManager::TestCase_EnumerateLogs()
{
    try
    {
        MgLogManager* pMgLogManager = MgLogManager::GetInstance();

        // Create a few files
        STRING path = pMgLogManager->GetLogsPath();
        CreateFile(path + TestName, L"");
        CreateFile(path + JunkName, L"");
        CreateFile(path + TestName2, L"");
        CreateFile(path + JunkName2, L"");

        Ptr<MgPropertyCollection> logs = pMgLogManager->EnumerateLogs();

        // Make sure the files show up in the enumeration
        Ptr<MgStringProperty> testNameProp = new MgStringProperty(L"LogNameProperty", TestName);
        Ptr<MgStringProperty> junkNameProp = new MgStringProperty(L"LogNameProperty", JunkName);
        Ptr<MgStringProperty> testName2Prop = new MgStringProperty(L"LogNameProperty", TestName2);
        Ptr<MgStringProperty> junkName2Prop = new MgStringProperty(L"LogNameProperty", JunkName2);

        CPPUNIT_ASSERT(logs->Contains(junkNameProp));
        CPPUNIT_ASSERT(logs->Contains(testNameProp));
        CPPUNIT_ASSERT(logs->Contains(junkName2Prop));
        CPPUNIT_ASSERT(logs->Contains(testName2Prop));
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case deletes log files that were created during the running
/// of the log manager tests
///----------------------------------------------------------------------------
void TestLogManager::TestCase_DeleteLog()
{
    try
    {
        MgLogManager* pMgLogManager = MgLogManager::GetInstance();

        pMgLogManager->DeleteLog(TestName);
        pMgLogManager->DeleteLog(NewTestName);
        pMgLogManager->DeleteLog(JunkName);
        pMgLogManager->DeleteLog(TestName2);
        pMgLogManager->DeleteLog(JunkName2);

        Ptr<MgPropertyCollection> logs = pMgLogManager->EnumerateLogs();

        CPPUNIT_ASSERT(!logs->Contains(TestName));
        CPPUNIT_ASSERT(!logs->Contains(NewTestName));
        CPPUNIT_ASSERT(!logs->Contains(JunkName));
        CPPUNIT_ASSERT(!logs->Contains(TestName2));
        CPPUNIT_ASSERT(!logs->Contains(JunkName2));
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}
///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case creates a file in the logs folder and then uses the api
/// to rename it.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_RenameLog()
{
    try
    {
        MgLogManager* pMgLogManager = MgLogManager::GetInstance();

        // Create test file
        STRING path = pMgLogManager->GetLogsPath();
        CreateFile(path + TestName, L"");

        // Rename the test file
        pMgLogManager->RenameLog(TestName, NewTestName);

        Ptr<MgPropertyCollection> logs = pMgLogManager->EnumerateLogs();

        Ptr<MgStringProperty> newTestNameProp = new MgStringProperty(L"LogNameProperty", NewTestName);
        CPPUNIT_ASSERT(logs->Contains(newTestNameProp));

        CPPUNIT_ASSERT_THROW_MG(pMgLogManager->RenameLog(L"", NewTestName), MgNullArgumentException*);
        CPPUNIT_ASSERT_THROW_MG(pMgLogManager->RenameLog(NewTestName, L""), MgNullArgumentException*);
        CPPUNIT_ASSERT_THROW_MG(pMgLogManager->RenameLog(TestName, TestName), MgDuplicateFileException*);
        CPPUNIT_ASSERT_THROW_MG(pMgLogManager->RenameLog(L"DoesNotExist.log", L"NewDoesNotExist.log"), MgFileNotFoundException*);
#ifdef _WIN32
        CPPUNIT_ASSERT_THROW_MG(pMgLogManager->RenameLog(NewTestName, L"?"), MgInvalidArgumentException*);
#endif
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to change the log info.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_SetAccessLogInfo()
{
    try
    {
        MgLogManager* pMgLogManager = MgLogManager::GetInstance();

        // Save original information
        bool bOriginalEnabled = pMgLogManager->IsAccessLogEnabled();
        STRING originalName = pMgLogManager->GetAccessLogFileName();
        STRING originalParams = pMgLogManager->GetAccessLogParameters();

        pMgLogManager->SetAccessLogInfo(false, TestName, TestParameters);

        bool bEnabled = pMgLogManager->IsAccessLogEnabled();
        STRING name = pMgLogManager->GetAccessLogFileName();
        STRING params = pMgLogManager->GetAccessLogParameters();

        // Restore original info
        pMgLogManager->SetAccessLogInfo(bOriginalEnabled, originalName, originalParams);

        CPPUNIT_ASSERT(bEnabled == false);
        CPPUNIT_ASSERT(wcscmp(name.c_str(), TestName) == 0);
        CPPUNIT_ASSERT(wcscmp(params.c_str(), TestParameters) == 0);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to clear the log.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_ClearAccessLog()
{
    try
    {
        MgLogManager* pMgLogManager = MgLogManager::GetInstance();
        bool bResult = pMgLogManager->ClearAccessLog();
        CPPUNIT_ASSERT(bResult);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to get the contents of the log.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_GetAccessLog()
{
    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    STRING path = pMgLogManager->GetLogsPath();
    STRING originalName = pMgLogManager->GetAccessLogFileName();
    pMgLogManager->SetAccessLogFileName(TestName);

    STRING contents = Entry1 + Entry2 + Entry3 + Entry4 + LastEntry;
    CreateFile(path + TestName, contents);

    STRING logContents;
    STRING lastLogEntry;

    try
    {
        Ptr<MgByteReader> byteReader;
        byteReader = pMgLogManager->GetAccessLog();
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        byteReader = pMgLogManager->GetAccessLog(1);
        lastLogEntry = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));

        pMgLogManager->SetAccessLogFileName(originalName);
    }
    catch (MgException* e)
    {
        pMgLogManager->SetAccessLogFileName(originalName);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        pMgLogManager->SetAccessLogFileName(originalName);
        throw;
    }

    CPPUNIT_ASSERT(logContents == contents);
    CPPUNIT_ASSERT(lastLogEntry == LastEntry);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to get the contents of the log between two dates.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_GetAccessLogByDate()
{
    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    STRING path = pMgLogManager->GetLogsPath();
    STRING originalName = pMgLogManager->GetAccessLogFileName();
    pMgLogManager->SetAccessLogFileName(DynamicTestName);

    STRING contents = Entry1 + Entry2 + Entry3 + Entry4;
    CreateFile(path + DynamicTestNameDate1, contents);
    contents = LastEntry;
    CreateFile(path + DynamicTestNameDate2, contents);

    STRING logContents;

    try
    {
        Ptr<MgByteReader> byteReader;
        Ptr<MgDateTime> fromDate;
        Ptr<MgDateTime> toDate;

        // from & to dates are at exact time an entry was made
        fromDate = new MgDateTime(2005, 3, 7, 16, 51, 13, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 54, 19, 0);
        byteReader = pMgLogManager->GetAccessLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry1 + Entry2 + Entry3));

        // from & to date are the same and at exact time an entry was made
        fromDate = new MgDateTime(2005, 3, 7, 16, 51, 13, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 51, 13, 0);
        byteReader = pMgLogManager->GetAccessLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == Entry1);

        // from & to date are the same and not at exact time an entry was made
        fromDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        byteReader = pMgLogManager->GetAccessLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents.length() == 0);

        // from & to dates are at not at exact times an entry was made
        fromDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 56, 0, 0);
        byteReader = pMgLogManager->GetAccessLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry2 + Entry3 + Entry4));

        // spans two different files
        fromDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        toDate = new MgDateTime(2005, 3, 8, 1, 0, 0, 0);
        byteReader = pMgLogManager->GetAccessLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry2 + Entry3 + Entry4 + LastEntry));

        // spans two different files, the first of which doesn't exist
        fromDate = new MgDateTime(2005, 3, 6, 23, 59, 59, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        byteReader = pMgLogManager->GetAccessLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == Entry1);

        // spans two different files, the second of which doesn't exist
        fromDate = new MgDateTime(2005, 3, 8, 0, 0, 0, 0);
        toDate = new MgDateTime(2005, 3, 9, 0, 0, 0, 0);
        byteReader = pMgLogManager->GetAccessLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == LastEntry);

        // from date is after the latest entry in the log files
        fromDate = new MgDateTime(2006, 1, 1, 1, 0, 0, 0);
        toDate = new MgDateTime(2006, 1, 1, 1, 0, 0, 0);
        byteReader = pMgLogManager->GetAccessLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents.length() == 0);

        // Use a null value for the date
        CPPUNIT_ASSERT_THROW_MG(pMgLogManager->GetAccessLog(NULL, toDate), MgNullArgumentException*);

        // Use dates more than 24 hours apart
        fromDate = new MgDateTime(2005, 2, 18, 14, 0, 0, 0);
        toDate = new MgDateTime(2005, 2, 19, 18, 0, 0, 0);
        CPPUNIT_ASSERT_THROW_MG(pMgLogManager->GetAccessLog(fromDate, toDate), MgInvalidArgumentException*);

        // Search a log file with an invalid log entry as the first entry
        CreateFile(path + JunkName, L"asdfasdfasdf");
        pMgLogManager->SetAccessLogFileName(JunkName);
        fromDate = new MgDateTime(2005, 3, 8, 0, 0, 0, 0);
        toDate = new MgDateTime(2005, 3, 9, 0, 0, 0, 0);
        CPPUNIT_ASSERT_THROW_MG(pMgLogManager->GetAccessLog(fromDate, toDate), MgInvalidLogEntryException*);

        pMgLogManager->SetAccessLogFileName(originalName);
    }
    catch (MgException* e)
    {
        pMgLogManager->SetAccessLogFileName(originalName);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        pMgLogManager->SetAccessLogFileName(originalName);
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to get the contents of the log using an invalid
/// argument.  An exception should be thrown.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_GetAccessLogInvalid()
{
    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    CPPUNIT_ASSERT_THROW_MG(pMgLogManager->GetAccessLog(-1), MgArgumentOutOfRangeException*);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to change the log info.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_SetAdminLogInfo()
{
    try
    {
        MgLogManager* pMgLogManager = MgLogManager::GetInstance();

        // Save original information
        bool bOriginalEnabled = pMgLogManager->IsAdminLogEnabled();
        STRING originalName = pMgLogManager->GetAdminLogFileName();
        STRING originalParams = pMgLogManager->GetAdminLogParameters();

        pMgLogManager->SetAdminLogInfo(false, TestName, TestParameters);

        bool bEnabled = pMgLogManager->IsAdminLogEnabled();
        STRING name = pMgLogManager->GetAdminLogFileName();
        STRING params = pMgLogManager->GetAdminLogParameters();

        // Restore original info
        pMgLogManager->SetAdminLogInfo(bOriginalEnabled, originalName, originalParams);

        CPPUNIT_ASSERT(bEnabled == false);
        CPPUNIT_ASSERT(wcscmp(name.c_str(), TestName) == 0);
        CPPUNIT_ASSERT(wcscmp(params.c_str(), TestParameters) == 0);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to clear the log.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_ClearAdminLog()
{
    try
    {
        MgLogManager* pMgLogManager = MgLogManager::GetInstance();
        bool bResult = pMgLogManager->ClearAdminLog();
        CPPUNIT_ASSERT(bResult);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to get the contents of the log.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_GetAdminLog()
{
    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    STRING path = pMgLogManager->GetLogsPath();
    STRING originalName = pMgLogManager->GetAdminLogFileName();
    pMgLogManager->SetAdminLogFileName(TestName);

    STRING contents = Entry1 + Entry2 + Entry3 + Entry4 + LastEntry;
    CreateFile(path + TestName, contents);

    STRING logContents;
    STRING lastLogEntry;

    try
    {
        Ptr<MgByteReader> byteReader = pMgLogManager->GetAdminLog();
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        byteReader = pMgLogManager->GetAdminLog(1);
        lastLogEntry = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        pMgLogManager->SetAdminLogFileName(originalName);
    }
    catch (MgException* e)
    {
        pMgLogManager->SetAdminLogFileName(originalName);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        pMgLogManager->SetAdminLogFileName(originalName);
        throw;
    }

    CPPUNIT_ASSERT(logContents == contents);
    CPPUNIT_ASSERT(lastLogEntry == LastEntry);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to get the contents of the log between two dates.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_GetAdminLogByDate()
{
    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    STRING path = pMgLogManager->GetLogsPath();
    STRING originalName = pMgLogManager->GetAdminLogFileName();
    pMgLogManager->SetAdminLogFileName(DynamicTestName);

    STRING contents = Entry1 + Entry2 + Entry3 + Entry4;
    CreateFile(path + DynamicTestNameDate1, contents);
    contents = LastEntry;
    CreateFile(path + DynamicTestNameDate2, contents);

    STRING logContents;

    try
    {
        Ptr<MgByteReader> byteReader;
        Ptr<MgDateTime> fromDate;
        Ptr<MgDateTime> toDate;

        // from & to dates are at exact time an entry was made
        fromDate = new MgDateTime(2005, 3, 7, 16, 51, 13, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 54, 19, 0);
        byteReader = pMgLogManager->GetAdminLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry1 + Entry2 + Entry3));

        // from & to date are the same and at exact time an entry was made
        fromDate = new MgDateTime(2005, 3, 7, 16, 51, 13, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 51, 13, 0);
        byteReader = pMgLogManager->GetAdminLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == Entry1);

        // from & to date are the same and not at exact time an entry was made
        fromDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        byteReader = pMgLogManager->GetAdminLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents.length() == 0);

        // from & to dates are at not at exact times an entry was made
        fromDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 56, 0, 0);
        byteReader = pMgLogManager->GetAdminLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry2 + Entry3 + Entry4));

        // spans two different files, the second of which doesn't exist
        fromDate = new MgDateTime(2005, 3, 8, 0, 0, 0, 0);
        toDate = new MgDateTime(2005, 3, 9, 0, 0, 0, 0);
        byteReader = pMgLogManager->GetAdminLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == LastEntry);

        // from date is after the latest entry in the log files
        fromDate = new MgDateTime(2006, 1, 1, 1, 0, 0, 0);
        toDate = new MgDateTime(2006, 1, 1, 1, 0, 0, 0);
        byteReader = pMgLogManager->GetAdminLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents.length() == 0);

        // Use a null value for the date
        CPPUNIT_ASSERT_THROW_MG(pMgLogManager->GetAdminLog(NULL, toDate), MgNullArgumentException*);

        // Use dates more than 24 hours apart
        fromDate = new MgDateTime(2005, 2, 18, 14, 0, 0, 0);
        toDate = new MgDateTime(2005, 2, 19, 18, 0, 0, 0);
        CPPUNIT_ASSERT_THROW_MG(pMgLogManager->GetAdminLog(fromDate, toDate), MgInvalidArgumentException*);

        pMgLogManager->SetAdminLogFileName(originalName);
    }
    catch (MgException* e)
    {
        pMgLogManager->SetAdminLogFileName(originalName);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        pMgLogManager->SetAdminLogFileName(originalName);
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to get the contents of the log using an invalid
/// argument.  An exception should be thrown.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_GetAdminLogInvalid()
{
    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    CPPUNIT_ASSERT_THROW_MG(pMgLogManager->GetAdminLog(-1), MgArgumentOutOfRangeException*);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to change the log info.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_SetAuthenticationLogInfo()
{
    try
    {
        MgLogManager* pMgLogManager = MgLogManager::GetInstance();

        // Save original information
        bool bOriginalEnabled = pMgLogManager->IsAuthenticationLogEnabled();
        STRING originalName = pMgLogManager->GetAuthenticationLogFileName();
        STRING originalParams = pMgLogManager->GetAuthenticationLogParameters();

        pMgLogManager->SetAuthenticationLogInfo(false, TestName, TestParameters);

        bool bEnabled = pMgLogManager->IsAuthenticationLogEnabled();
        STRING name = pMgLogManager->GetAuthenticationLogFileName();
        STRING params = pMgLogManager->GetAuthenticationLogParameters();

        // Restore original info
        pMgLogManager->SetAuthenticationLogInfo(bOriginalEnabled, originalName, originalParams);

        CPPUNIT_ASSERT(bEnabled == false);
        CPPUNIT_ASSERT(wcscmp(name.c_str(), TestName) == 0);
        CPPUNIT_ASSERT(wcscmp(params.c_str(), TestParameters) == 0);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to clear the log.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_ClearAuthenticationLog()
{
    try
    {
        MgLogManager* pMgLogManager = MgLogManager::GetInstance();
        bool bResult = pMgLogManager->ClearAuthenticationLog();
        CPPUNIT_ASSERT(bResult);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to get the contents of the log.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_GetAuthenticationLog()
{
    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    STRING path = pMgLogManager->GetLogsPath();
    STRING originalName = pMgLogManager->GetAuthenticationLogFileName();
    pMgLogManager->SetAuthenticationLogFileName(TestName);

    STRING contents = Entry1 + Entry2 + Entry3 + Entry4 + LastEntry;
    CreateFile(path + TestName, contents);

    STRING logContents;
    STRING lastLogEntry;

    try
    {
        Ptr<MgByteReader> byteReader = pMgLogManager->GetAuthenticationLog();
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        byteReader = pMgLogManager->GetAuthenticationLog(1);
        lastLogEntry = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        pMgLogManager->SetAuthenticationLogFileName(originalName);
    }
    catch (MgException* e)
    {
        pMgLogManager->SetAuthenticationLogFileName(originalName);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        pMgLogManager->SetAuthenticationLogFileName(originalName);
        throw;
    }

    CPPUNIT_ASSERT(logContents == contents);
    CPPUNIT_ASSERT(lastLogEntry == LastEntry);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to get the contents of the log between two dates.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_GetAuthenticationLogByDate()
{
    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    STRING path = pMgLogManager->GetLogsPath();
    STRING originalName = pMgLogManager->GetAuthenticationLogFileName();
    pMgLogManager->SetAuthenticationLogFileName(DynamicTestName);

    STRING contents = Entry1 + Entry2 + Entry3 + Entry4;
    CreateFile(path + DynamicTestNameDate1, contents);
    contents = LastEntry;
    CreateFile(path + DynamicTestNameDate2, contents);

    STRING logContents;

    try
    {
        Ptr<MgByteReader> byteReader;
        Ptr<MgDateTime> fromDate;
        Ptr<MgDateTime> toDate;

        // from & to dates are at exact time an entry was made
        fromDate = new MgDateTime(2005, 3, 7, 16, 51, 13, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 54, 19, 0);
        byteReader = pMgLogManager->GetAuthenticationLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry1 + Entry2 + Entry3));

        // from & to dates are at not at exact times an entry was made
        fromDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 56, 0, 0);
        byteReader = pMgLogManager->GetAuthenticationLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry2 + Entry3 + Entry4));

        // spans two different files
        fromDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        toDate = new MgDateTime(2005, 3, 8, 1, 0, 0, 0);
        byteReader = pMgLogManager->GetAuthenticationLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry2 + Entry3 + Entry4 + LastEntry));

        // spans two different files, the first of which doesn't exist
        fromDate = new MgDateTime(2005, 3, 6, 23, 59, 59, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        byteReader = pMgLogManager->GetAuthenticationLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == Entry1);

        // spans two different files, the second of which doesn't exist
        fromDate = new MgDateTime(2005, 3, 8, 0, 0, 0, 0);
        toDate = new MgDateTime(2005, 3, 9, 0, 0, 0, 0);
        byteReader = pMgLogManager->GetAuthenticationLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == LastEntry);

        // from date is after the latest entry in the log files
        fromDate = new MgDateTime(2006, 1, 1, 1, 0, 0, 0);
        toDate = new MgDateTime(2006, 1, 1, 1, 0, 0, 0);
        byteReader = pMgLogManager->GetAuthenticationLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents.length() == 0);

        // Use a null value for the date
        CPPUNIT_ASSERT_THROW_MG(pMgLogManager->GetAuthenticationLog(NULL, toDate), MgNullArgumentException*);

        // Use dates more than 24 hours apart
        fromDate = new MgDateTime(2005, 2, 18, 14, 0, 0, 0);
        toDate = new MgDateTime(2005, 2, 19, 18, 0, 0, 0);
        CPPUNIT_ASSERT_THROW_MG(pMgLogManager->GetAuthenticationLog(fromDate, toDate), MgInvalidArgumentException*);

        pMgLogManager->SetAuthenticationLogFileName(originalName);
    }
    catch (MgException* e)
    {
        pMgLogManager->SetAuthenticationLogFileName(originalName);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        pMgLogManager->SetAuthenticationLogFileName(originalName);
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to get the contents of the log using an invalid
/// argument.  An exception should be thrown.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_GetAuthenticationLogInvalid()
{
    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    CPPUNIT_ASSERT_THROW_MG(pMgLogManager->GetAuthenticationLog(-1), MgArgumentOutOfRangeException*);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to change the log info.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_SetErrorLogInfo()
{
    try
    {
        MgLogManager* pMgLogManager = MgLogManager::GetInstance();

        // Save original information
        bool bOriginalEnabled = pMgLogManager->IsErrorLogEnabled();
        STRING originalName = pMgLogManager->GetErrorLogFileName();
        STRING originalParams = pMgLogManager->GetErrorLogParameters();

        pMgLogManager->SetErrorLogInfo(false, TestName, TestParameters);

        bool bEnabled = pMgLogManager->IsErrorLogEnabled();
        STRING name = pMgLogManager->GetErrorLogFileName();
        STRING params = pMgLogManager->GetErrorLogParameters();

        // Restore original info
        pMgLogManager->SetErrorLogInfo(bOriginalEnabled, originalName, originalParams);

        CPPUNIT_ASSERT(bEnabled == false);
        CPPUNIT_ASSERT(wcscmp(name.c_str(), TestName) == 0);
        CPPUNIT_ASSERT(wcscmp(params.c_str(), TestParameters) == 0);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to clear the log.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_ClearErrorLog()
{
    try
    {
        MgLogManager* pMgLogManager = MgLogManager::GetInstance();
        bool bResult = pMgLogManager->ClearErrorLog();
        CPPUNIT_ASSERT(bResult);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to get the contents of the log.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_GetErrorLog()
{
    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    STRING path = pMgLogManager->GetLogsPath();
    STRING originalName = pMgLogManager->GetErrorLogFileName();
    pMgLogManager->SetErrorLogFileName(TestName);

    STRING contents = Entry1 + Entry2 + Entry3 + Entry4 + LastEntry;
    CreateFile(path + TestName, contents);

    STRING logContents;
    STRING lastLogEntry;

    try
    {
        Ptr<MgByteReader> byteReader = pMgLogManager->GetErrorLog();
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        byteReader = pMgLogManager->GetErrorLog(1);
        lastLogEntry = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        pMgLogManager->SetErrorLogFileName(originalName);
    }
    catch (MgException* e)
    {
        pMgLogManager->SetErrorLogFileName(originalName);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        pMgLogManager->SetErrorLogFileName(originalName);
        throw;
    }

    CPPUNIT_ASSERT(logContents == contents);
    CPPUNIT_ASSERT(lastLogEntry == LastEntry);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to get the contents of the log between two dates.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_GetErrorLogByDate()
{
    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    STRING path = pMgLogManager->GetLogsPath();
    STRING originalName = pMgLogManager->GetErrorLogFileName();
    pMgLogManager->SetErrorLogFileName(DynamicTestName);

    STRING contents = Entry1 + Entry2 + Entry3 + Entry4;
    CreateFile(path + DynamicTestNameDate1, contents);
    contents = LastEntry;
    CreateFile(path + DynamicTestNameDate2, contents);

    STRING logContents;

    try
    {
        Ptr<MgByteReader> byteReader;
        Ptr<MgDateTime> fromDate;
        Ptr<MgDateTime> toDate;

        // from & to dates are at exact time an entry was made
        fromDate = new MgDateTime(2005, 3, 7, 16, 51, 13, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 54, 19, 0);
        byteReader = pMgLogManager->GetErrorLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry1 + Entry2 + Entry3));

        // from & to dates are at not at exact times an entry was made
        fromDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 56, 0, 0);
        byteReader = pMgLogManager->GetErrorLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry2 + Entry3 + Entry4));

        // spans two different files
        fromDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        toDate = new MgDateTime(2005, 3, 8, 1, 0, 0, 0);
        byteReader = pMgLogManager->GetErrorLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry2 + Entry3 + Entry4 + LastEntry));

        // spans two different files, the first of which doesn't exist
        fromDate = new MgDateTime(2005, 3, 6, 23, 59, 59, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        byteReader = pMgLogManager->GetErrorLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == Entry1);

        // spans two different files, the second of which doesn't exist
        fromDate = new MgDateTime(2005, 3, 8, 0, 0, 0, 0);
        toDate = new MgDateTime(2005, 3, 9, 0, 0, 0, 0);
        byteReader = pMgLogManager->GetErrorLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == LastEntry);

        // from date is after the latest entry in the log files
        fromDate = new MgDateTime(2006, 1, 1, 1, 0, 0, 0);
        toDate = new MgDateTime(2006, 1, 1, 1, 0, 0, 0);
        byteReader = pMgLogManager->GetErrorLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents.length() == 0);

        // Use a null value for the date
        CPPUNIT_ASSERT_THROW_MG(pMgLogManager->GetErrorLog(NULL, toDate), MgNullArgumentException*);

        // Use dates more than 24 hours apart
        fromDate = new MgDateTime(2005, 2, 18, 14, 0, 0, 0);
        toDate = new MgDateTime(2005, 2, 19, 18, 0, 0, 0);
        CPPUNIT_ASSERT_THROW_MG(pMgLogManager->GetErrorLog(fromDate, toDate), MgInvalidArgumentException*);

        pMgLogManager->SetErrorLogFileName(originalName);
    }
    catch (MgException* e)
    {
        pMgLogManager->SetErrorLogFileName(originalName);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        pMgLogManager->SetErrorLogFileName(originalName);
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to get the contents of the log using an invalid
/// argument.  An exception should be thrown.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_GetErrorLogInvalid()
{
    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    CPPUNIT_ASSERT_THROW_MG(pMgLogManager->GetErrorLog(-1), MgArgumentOutOfRangeException*);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to change the log info.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_SetSessionLogInfo()
{
    try
    {
        MgLogManager* pMgLogManager = MgLogManager::GetInstance();

        // Save original information
        bool bOriginalEnabled = pMgLogManager->IsSessionLogEnabled();
        STRING originalName = pMgLogManager->GetSessionLogFileName();
        STRING originalParams = pMgLogManager->GetSessionLogParameters();

        pMgLogManager->SetSessionLogInfo(false, TestName, TestParameters);

        bool bEnabled = pMgLogManager->IsSessionLogEnabled();
        STRING name = pMgLogManager->GetSessionLogFileName();
        STRING params = pMgLogManager->GetSessionLogParameters();

        // Restore original info
        pMgLogManager->SetSessionLogInfo(bOriginalEnabled, originalName, originalParams);

        CPPUNIT_ASSERT(bEnabled == false);
        CPPUNIT_ASSERT(wcscmp(name.c_str(), TestName) == 0);
        CPPUNIT_ASSERT(wcscmp(params.c_str(), TestParameters) == 0);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to clear the log.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_ClearSessionLog()
{
    try
    {
        MgLogManager* pMgLogManager = MgLogManager::GetInstance();
        bool bResult = pMgLogManager->ClearSessionLog();
        CPPUNIT_ASSERT(bResult);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to get the contents of the log.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_GetSessionLog()
{
    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    STRING path = pMgLogManager->GetLogsPath();
    STRING originalName = pMgLogManager->GetSessionLogFileName();
    pMgLogManager->SetSessionLogFileName(TestName);

    STRING contents = Entry1 + Entry2 + Entry3 + Entry4 + LastEntry;
    CreateFile(path + TestName, contents);

    STRING logContents;
    STRING lastLogEntry;

    try
    {
        Ptr<MgByteReader> byteReader = pMgLogManager->GetSessionLog();
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        byteReader = pMgLogManager->GetSessionLog(1);
        lastLogEntry = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        pMgLogManager->SetSessionLogFileName(originalName);
    }
    catch (MgException* e)
    {
        pMgLogManager->SetSessionLogFileName(originalName);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        pMgLogManager->SetSessionLogFileName(originalName);
        throw;
    }

    CPPUNIT_ASSERT(logContents == contents);
    CPPUNIT_ASSERT(lastLogEntry == LastEntry);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to get the contents of the log between two dates.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_GetSessionLogByDate()
{
    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    STRING path = pMgLogManager->GetLogsPath();
    STRING originalName = pMgLogManager->GetSessionLogFileName();
    pMgLogManager->SetSessionLogFileName(DynamicTestName);

    STRING contents = Entry1 + Entry2 + Entry3 + Entry4;
    CreateFile(path + DynamicTestNameDate1, contents);
    contents = LastEntry;
    CreateFile(path + DynamicTestNameDate2, contents);

    STRING logContents;

    try
    {
        Ptr<MgByteReader> byteReader;
        Ptr<MgDateTime> fromDate;
        Ptr<MgDateTime> toDate;

        // from & to dates are at exact time an entry was made
        fromDate = new MgDateTime(2005, 3, 7, 16, 51, 13, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 54, 19, 0);
        byteReader = pMgLogManager->GetSessionLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry1 + Entry2 + Entry3));

        // from & to dates are at not at exact times an entry was made
        fromDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 56, 0, 0);
        byteReader = pMgLogManager->GetSessionLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry2 + Entry3 + Entry4));

        // spans two different files
        fromDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        toDate = new MgDateTime(2005, 3, 8, 1, 0, 0, 0);
        byteReader = pMgLogManager->GetSessionLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry2 + Entry3 + Entry4 + LastEntry));

        // spans two different files, the first of which doesn't exist
        fromDate = new MgDateTime(2005, 3, 6, 23, 59, 59, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        byteReader = pMgLogManager->GetSessionLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == Entry1);

        // spans two different files, the second of which doesn't exist
        fromDate = new MgDateTime(2005, 3, 8, 0, 0, 0, 0);
        toDate = new MgDateTime(2005, 3, 9, 0, 0, 0, 0);
        byteReader = pMgLogManager->GetSessionLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == LastEntry);

        // from date is after the latest entry in the log files
        fromDate = new MgDateTime(2006, 1, 1, 1, 0, 0, 0);
        toDate = new MgDateTime(2006, 1, 1, 1, 0, 0, 0);
        byteReader = pMgLogManager->GetSessionLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents.length() == 0);

        // Use a null value for the date
        CPPUNIT_ASSERT_THROW_MG(pMgLogManager->GetSessionLog(NULL, toDate), MgNullArgumentException*);

        // Use dates more than 24 hours apart
        fromDate = new MgDateTime(2005, 2, 18, 14, 0, 0, 0);
        toDate = new MgDateTime(2005, 2, 19, 18, 0, 0, 0);
        CPPUNIT_ASSERT_THROW_MG(pMgLogManager->GetSessionLog(fromDate, toDate), MgInvalidArgumentException*);

        pMgLogManager->SetSessionLogFileName(originalName);
    }
    catch (MgException* e)
    {
        pMgLogManager->SetSessionLogFileName(originalName);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        pMgLogManager->SetSessionLogFileName(originalName);
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to get the contents of the log using an invalid
/// argument.  An exception should be thrown.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_GetSessionLogInvalid()
{
    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    CPPUNIT_ASSERT_THROW_MG(pMgLogManager->GetSessionLog(-1), MgArgumentOutOfRangeException*);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to change the log info.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_SetTraceLogInfo()
{
    try
    {
        MgLogManager* pMgLogManager = MgLogManager::GetInstance();

        // Save original information
        bool bOriginalEnabled = pMgLogManager->IsTraceLogEnabled();
        STRING originalName = pMgLogManager->GetTraceLogFileName();
        STRING originalParams = pMgLogManager->GetTraceLogParameters();

        pMgLogManager->SetTraceLogInfo(false, TestName, TestParameters);

        bool bEnabled = pMgLogManager->IsTraceLogEnabled();
        STRING name = pMgLogManager->GetTraceLogFileName();
        STRING params = pMgLogManager->GetTraceLogParameters();

        // Restore original info
        pMgLogManager->SetTraceLogInfo(bOriginalEnabled, originalName, originalParams);

        CPPUNIT_ASSERT(bEnabled == false);
        CPPUNIT_ASSERT(wcscmp(name.c_str(), TestName) == 0);
        CPPUNIT_ASSERT(wcscmp(params.c_str(), TestParameters) == 0);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to clear the log.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_ClearTraceLog()
{
    try
    {
        MgLogManager* pMgLogManager = MgLogManager::GetInstance();
        bool bResult = pMgLogManager->ClearTraceLog();
        CPPUNIT_ASSERT(bResult);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to get the contents of the log.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_GetTraceLog()
{
    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    STRING path = pMgLogManager->GetLogsPath();
    STRING originalName = pMgLogManager->GetTraceLogFileName();
    pMgLogManager->SetTraceLogFileName(TestName);

    STRING contents = Entry1 + Entry2 + Entry3 + Entry4 + LastEntry;
    CreateFile(path + TestName, contents);

    STRING logContents;
    STRING lastLogEntry;

    try
    {
        Ptr<MgByteReader> byteReader = pMgLogManager->GetTraceLog();
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        byteReader = pMgLogManager->GetTraceLog(1);
        lastLogEntry = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        pMgLogManager->SetTraceLogFileName(originalName);
    }
    catch (MgException* e)
    {
        pMgLogManager->SetTraceLogFileName(originalName);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        pMgLogManager->SetTraceLogFileName(originalName);
        throw;
    }

    CPPUNIT_ASSERT(logContents == contents);
    CPPUNIT_ASSERT(lastLogEntry == LastEntry);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to get the contents of the log between two dates.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_GetTraceLogByDate()
{
    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    STRING path = pMgLogManager->GetLogsPath();
    STRING originalName = pMgLogManager->GetTraceLogFileName();
    pMgLogManager->SetTraceLogFileName(DynamicTestName);

    STRING contents = Entry1 + Entry2 + Entry3 + Entry4;
    CreateFile(path + DynamicTestNameDate1, contents);
    contents = LastEntry;
    CreateFile(path + DynamicTestNameDate2, contents);

    STRING logContents;

    try
    {
        Ptr<MgByteReader> byteReader;
        Ptr<MgDateTime> fromDate;
        Ptr<MgDateTime> toDate;

        // from & to dates are at exact time an entry was made
        fromDate = new MgDateTime(2005, 3, 7, 16, 51, 13, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 54, 19, 0);
        byteReader = pMgLogManager->GetTraceLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry1 + Entry2 + Entry3));

        // from & to dates are at not at exact times an entry was made
        fromDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 56, 0, 0);
        byteReader = pMgLogManager->GetTraceLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry2 + Entry3 + Entry4));

        // spans two different files
        fromDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        toDate = new MgDateTime(2005, 3, 8, 1, 0, 0, 0);
        byteReader = pMgLogManager->GetTraceLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry2 + Entry3 + Entry4 + LastEntry));

        // spans two different files, the first of which doesn't exist
        fromDate = new MgDateTime(2005, 3, 6, 23, 59, 59, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        byteReader = pMgLogManager->GetTraceLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == Entry1);

        // spans two different files, the second of which doesn't exist
        fromDate = new MgDateTime(2005, 3, 8, 0, 0, 0, 0);
        toDate = new MgDateTime(2005, 3, 9, 0, 0, 0, 0);
        byteReader = pMgLogManager->GetTraceLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == LastEntry);

        // from date is after the latest entry in the log files
        fromDate = new MgDateTime(2006, 1, 1, 1, 0, 0, 0);
        toDate = new MgDateTime(2006, 1, 1, 1, 0, 0, 0);
        byteReader = pMgLogManager->GetTraceLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents.length() == 0);

        // Use a null value for the date
        CPPUNIT_ASSERT_THROW_MG(pMgLogManager->GetTraceLog(NULL, toDate), MgNullArgumentException*);

        // Use dates more than 24 hours apart
        fromDate = new MgDateTime(2005, 2, 18, 14, 0, 0, 0);
        toDate = new MgDateTime(2005, 2, 19, 18, 0, 0, 0);
        CPPUNIT_ASSERT_THROW_MG(pMgLogManager->GetTraceLog(fromDate, toDate), MgInvalidArgumentException*);

        pMgLogManager->SetTraceLogFileName(originalName);
    }
    catch (MgException* e)
    {
        pMgLogManager->SetTraceLogFileName(originalName);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        pMgLogManager->SetTraceLogFileName(originalName);
        throw;
    }
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to get the contents of the log using an invalid
/// argument.  An exception should be thrown.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_GetTraceLogInvalid()
{
    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    CPPUNIT_ASSERT_THROW_MG(pMgLogManager->GetTraceLog(-1), MgArgumentOutOfRangeException*);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case writes an entry to the log.  (Assumes that getlog works)
///----------------------------------------------------------------------------
void TestLogManager::TestCase_LogAccessEntry()
{
    Ptr<MgByteReader> byteReader;
    STRING logEntry;
    STRING::size_type pos;

    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    bool bOriginalEnabled = pMgLogManager->IsAccessLogEnabled();
    pMgLogManager->SetAccessLogEnabled(true);

    STRING entry = L"TestAccessEntry";
    MG_LOG_ACCESS_ENTRY(entry, L"TestClient", L"TestClientIp", L"TestUser");

    // Give the server time to write out the entry as it is on a different thread
    ACE_OS::sleep(2);

    pMgLogManager->SetAccessLogEnabled(bOriginalEnabled);

    try
    {
        byteReader = pMgLogManager->GetAccessLog(1);
        logEntry = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));

    }
    catch (...)
    {
        throw;
    }

    pos = logEntry.find(entry);
    CPPUNIT_ASSERT(pos != string::npos);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case writes an entry to the log.  (Assumes that getlog works)
///----------------------------------------------------------------------------
void TestLogManager::TestCase_LogAdminEntry()
{
    Ptr<MgByteReader> byteReader;
    STRING logEntry;
    STRING::size_type pos;

    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    bool bOriginalEnabled = pMgLogManager->IsAdminLogEnabled();
    pMgLogManager->SetAdminLogEnabled(true);

    STRING entry = L"TestAdminEntry";
    MG_LOG_ADMIN_ENTRY(entry, L"TestClient", L"TestClientIp", L"TestUser");

    // Give the server time to write out the entry as it is on a different thread
    ACE_OS::sleep(2);

    pMgLogManager->SetAdminLogEnabled(bOriginalEnabled);

    try
    {
        byteReader = pMgLogManager->GetAdminLog(1);
        logEntry = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }

    pos = logEntry.find(entry);
    CPPUNIT_ASSERT(pos != string::npos);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case writes an entry to the log.  (Assumes that getlog works)
///----------------------------------------------------------------------------
void TestLogManager::TestCase_LogAuthenticationEntry()
{
    Ptr<MgByteReader> byteReader;
    STRING logEntry;
    STRING::size_type pos;

    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    bool bOriginalEnabled = pMgLogManager->IsAuthenticationLogEnabled();
    pMgLogManager->SetAuthenticationLogEnabled(true);

    STRING entry = L"TestAuthenticationEntry";
    MG_LOG_AUTHENTICATION_ENTRY(entry);

    // Give the server time to write out the entry as it is on a different thread
    ACE_OS::sleep(2);

    pMgLogManager->SetAdminLogEnabled(bOriginalEnabled);

    try
    {
        byteReader = pMgLogManager->GetAuthenticationLog(1);
        logEntry = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }

    pos = logEntry.find(entry);
    CPPUNIT_ASSERT(pos != string::npos);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case writes an entry to the log.  (Assumes that getlog works)
///----------------------------------------------------------------------------
void TestLogManager::TestCase_LogErrorEntry()
{
    Ptr<MgByteReader> byteReader;
    STRING logEntry;
    STRING::size_type pos;

    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    bool bOriginalEnabled = pMgLogManager->IsErrorLogEnabled();
    pMgLogManager->SetErrorLogEnabled(true);

    STRING entry = L"TestErrorEntry";
    MG_LOG_ERROR_ENTRY(entry);

    // Give the server time to write out the entry as it is on a different thread
    ACE_OS::sleep(2);

    pMgLogManager->SetErrorLogEnabled(bOriginalEnabled);

    try
    {
        byteReader = pMgLogManager->GetErrorLog(1);
        logEntry = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }

    pos = logEntry.find(entry);
    CPPUNIT_ASSERT(pos != string::npos);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case writes an entry to the log.  (Assumes that getlog works)
///----------------------------------------------------------------------------
void TestLogManager::TestCase_LogSessionEntry()
{
    STRING logEntry;
    MgSessionInfo sessionInfo;
    MgLogManager* logMan = MgLogManager::GetInstance();
    bool originalValue = logMan->IsSessionLogEnabled();

    logMan->SetSessionLogEnabled(true);
    MG_LOG_SESSION_ENTRY(sessionInfo);

    // Give the server time to write out the entry as it is on a different thread
    ACE_OS::sleep(2);

    logMan->SetSessionLogEnabled(originalValue);

    try
    {
        Ptr<MgByteReader> byteReader = logMan->GetSessionLog(1);

        MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader), logEntry);
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }

    // Find the values for ops failed and received, 0 and 0 respectively,
    // since no session object has been created.

    CPPUNIT_ASSERT(STRING::npos != logEntry.find(L"\t0\t0"));
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case writes an entry to the log.  (Assumes that getlog works)
///----------------------------------------------------------------------------
void TestLogManager::TestCase_LogTraceEntry()
{
    Ptr<MgByteReader> byteReader;
    STRING logEntry;
    STRING::size_type pos;

    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    bool bOriginalEnabled = pMgLogManager->IsTraceLogEnabled();
    pMgLogManager->SetTraceLogEnabled(true);

    STRING entry = L"TestTraceEntry";
    MG_LOG_TRACE_ENTRY(entry);

    // Give the server time to write out the entry as it is on a different thread
    ACE_OS::sleep(2);

    pMgLogManager->SetTraceLogEnabled(bOriginalEnabled);

    try
    {
        byteReader = pMgLogManager->GetTraceLog(1);
        logEntry = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
    }
    catch (MgException* e)
    {
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        throw;
    }

    pos = logEntry.find(entry);
    CPPUNIT_ASSERT(pos != string::npos);
}


///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case tries to get the contents of the specified log.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_GetLogFile()
{
    MgLogManager* pMgLogManager = MgLogManager::GetInstance();
    STRING path = pMgLogManager->GetLogsPath();
    STRING originalName = pMgLogManager->GetAccessLogFileName();
    pMgLogManager->SetAccessLogFileName(TestName);

    STRING contents = Entry1 + Entry2 + Entry3 + Entry4 + LastEntry;
    CreateFile(path + TestName, contents);

    STRING logContents;
    STRING lastLogEntry;

    try
    {
        Ptr<MgByteReader> byteReader;
        byteReader = pMgLogManager->GetLogFile(TestName);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));

        pMgLogManager->SetAccessLogFileName(originalName);
    }
    catch (MgException* e)
    {
        pMgLogManager->SetAccessLogFileName(originalName);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        pMgLogManager->SetAccessLogFileName(originalName);
        throw;
    }

    CPPUNIT_ASSERT(logContents == contents);
}
