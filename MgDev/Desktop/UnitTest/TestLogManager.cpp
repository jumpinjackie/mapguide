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

#include "MgDesktop.h"
#include "TestLogManager.h"
#include "CppUnitExtensions.h"
#include "Log/LogManager.h"
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
/// This test case checks to see if there is a valid MgdLogManager and that
/// there is only 1 MgdLogManager.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_ValidLogManager()
{
    MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
    CPPUNIT_ASSERT(pMgdLogManager != NULL);

    MgdLogManager* pMgdLogManager2 = MgdLogManager::GetInstance();
    CPPUNIT_ASSERT(pMgdLogManager == pMgdLogManager2);
}

///----------------------------------------------------------------------------
/// Test Case Description:
///
/// This test case checks the logs path.
///----------------------------------------------------------------------------
void TestLogManager::TestCase_GetLogsPath()
{
    MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
    CREFSTRING path = pMgdLogManager->GetLogsPath();

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
        MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();

        // Create a few files
        STRING path = pMgdLogManager->GetLogsPath();
        CreateFile(path + TestName, L"");
        CreateFile(path + JunkName, L"");
        CreateFile(path + TestName2, L"");
        CreateFile(path + JunkName2, L"");

        Ptr<MgPropertyCollection> logs = pMgdLogManager->EnumerateLogs();

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
        MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();

        pMgdLogManager->DeleteLog(TestName);
        pMgdLogManager->DeleteLog(NewTestName);
        pMgdLogManager->DeleteLog(JunkName);
        pMgdLogManager->DeleteLog(TestName2);
        pMgdLogManager->DeleteLog(JunkName2);

        Ptr<MgPropertyCollection> logs = pMgdLogManager->EnumerateLogs();

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
        MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();

        // Create test file
        STRING path = pMgdLogManager->GetLogsPath();
        CreateFile(path + TestName, L"");

        // Rename the test file
        pMgdLogManager->RenameLog(TestName, NewTestName);

        Ptr<MgPropertyCollection> logs = pMgdLogManager->EnumerateLogs();

        Ptr<MgStringProperty> newTestNameProp = new MgStringProperty(L"LogNameProperty", NewTestName);
        CPPUNIT_ASSERT(logs->Contains(newTestNameProp));

        CPPUNIT_ASSERT_THROW_MG(pMgdLogManager->RenameLog(L"", NewTestName), MgNullArgumentException*);
        CPPUNIT_ASSERT_THROW_MG(pMgdLogManager->RenameLog(NewTestName, L""), MgNullArgumentException*);
        CPPUNIT_ASSERT_THROW_MG(pMgdLogManager->RenameLog(TestName, TestName), MgDuplicateFileException*);
        CPPUNIT_ASSERT_THROW_MG(pMgdLogManager->RenameLog(L"DoesNotExist.log", L"NewDoesNotExist.log"), MgFileNotFoundException*);
#ifdef _WIN32
        CPPUNIT_ASSERT_THROW_MG(pMgdLogManager->RenameLog(NewTestName, L"?"), MgInvalidArgumentException*);
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
        MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();

        // Save original information
        bool bOriginalEnabled = pMgdLogManager->IsAccessLogEnabled();
        STRING originalName = pMgdLogManager->GetAccessLogFileName();
        STRING originalParams = pMgdLogManager->GetAccessLogParameters();

        pMgdLogManager->SetAccessLogInfo(false, TestName, TestParameters);

        bool bEnabled = pMgdLogManager->IsAccessLogEnabled();
        STRING name = pMgdLogManager->GetAccessLogFileName();
        STRING params = pMgdLogManager->GetAccessLogParameters();

        // Restore original info
        pMgdLogManager->SetAccessLogInfo(bOriginalEnabled, originalName, originalParams);

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
        MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
        bool bResult = pMgdLogManager->ClearAccessLog();
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
    MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
    STRING path = pMgdLogManager->GetLogsPath();
    STRING originalName = pMgdLogManager->GetAccessLogFileName();
    pMgdLogManager->SetAccessLogFileName(TestName);

    STRING contents = Entry1 + Entry2 + Entry3 + Entry4 + LastEntry;
    CreateFile(path + TestName, contents);

    STRING logContents;
    STRING lastLogEntry;

    try
    {
        Ptr<MgByteReader> byteReader;
        byteReader = pMgdLogManager->GetAccessLog();
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        byteReader = pMgdLogManager->GetAccessLog(1);
        lastLogEntry = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));

        pMgdLogManager->SetAccessLogFileName(originalName);
    }
    catch (MgException* e)
    {
        pMgdLogManager->SetAccessLogFileName(originalName);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        pMgdLogManager->SetAccessLogFileName(originalName);
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
    MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
    STRING path = pMgdLogManager->GetLogsPath();
    STRING originalName = pMgdLogManager->GetAccessLogFileName();
    pMgdLogManager->SetAccessLogFileName(DynamicTestName);

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
        byteReader = pMgdLogManager->GetAccessLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry1 + Entry2 + Entry3));

        // from & to date are the same and at exact time an entry was made
        fromDate = new MgDateTime(2005, 3, 7, 16, 51, 13, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 51, 13, 0);
        byteReader = pMgdLogManager->GetAccessLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == Entry1);

        // from & to date are the same and not at exact time an entry was made
        fromDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        byteReader = pMgdLogManager->GetAccessLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents.length() == 0);

        // from & to dates are at not at exact times an entry was made
        fromDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 56, 0, 0);
        byteReader = pMgdLogManager->GetAccessLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry2 + Entry3 + Entry4));

        // spans two different files
        fromDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        toDate = new MgDateTime(2005, 3, 8, 1, 0, 0, 0);
        byteReader = pMgdLogManager->GetAccessLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry2 + Entry3 + Entry4 + LastEntry));

        // spans two different files, the first of which doesn't exist
        fromDate = new MgDateTime(2005, 3, 6, 23, 59, 59, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        byteReader = pMgdLogManager->GetAccessLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == Entry1);

        // spans two different files, the second of which doesn't exist
        fromDate = new MgDateTime(2005, 3, 8, 0, 0, 0, 0);
        toDate = new MgDateTime(2005, 3, 9, 0, 0, 0, 0);
        byteReader = pMgdLogManager->GetAccessLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == LastEntry);

        // from date is after the latest entry in the log files
        fromDate = new MgDateTime(2006, 1, 1, 1, 0, 0, 0);
        toDate = new MgDateTime(2006, 1, 1, 1, 0, 0, 0);
        byteReader = pMgdLogManager->GetAccessLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents.length() == 0);

        // Use a null value for the date
        CPPUNIT_ASSERT_THROW_MG(pMgdLogManager->GetAccessLog(NULL, toDate), MgNullArgumentException*);

        // Use dates more than 24 hours apart
        fromDate = new MgDateTime(2005, 2, 18, 14, 0, 0, 0);
        toDate = new MgDateTime(2005, 2, 19, 18, 0, 0, 0);
        CPPUNIT_ASSERT_THROW_MG(pMgdLogManager->GetAccessLog(fromDate, toDate), MgInvalidArgumentException*);

        // Search a log file with an invalid log entry as the first entry
        CreateFile(path + JunkName, L"asdfasdfasdf");
        pMgdLogManager->SetAccessLogFileName(JunkName);
        fromDate = new MgDateTime(2005, 3, 8, 0, 0, 0, 0);
        toDate = new MgDateTime(2005, 3, 9, 0, 0, 0, 0);
        CPPUNIT_ASSERT_THROW_MG(pMgdLogManager->GetAccessLog(fromDate, toDate), MgdInvalidLogEntryException*);

        pMgdLogManager->SetAccessLogFileName(originalName);
    }
    catch (MgException* e)
    {
        pMgdLogManager->SetAccessLogFileName(originalName);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        pMgdLogManager->SetAccessLogFileName(originalName);
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
    MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
    CPPUNIT_ASSERT_THROW_MG(pMgdLogManager->GetAccessLog(-1), MgArgumentOutOfRangeException*);
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
        MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();

        // Save original information
        bool bOriginalEnabled = pMgdLogManager->IsAdminLogEnabled();
        STRING originalName = pMgdLogManager->GetAdminLogFileName();
        STRING originalParams = pMgdLogManager->GetAdminLogParameters();

        pMgdLogManager->SetAdminLogInfo(false, TestName, TestParameters);

        bool bEnabled = pMgdLogManager->IsAdminLogEnabled();
        STRING name = pMgdLogManager->GetAdminLogFileName();
        STRING params = pMgdLogManager->GetAdminLogParameters();

        // Restore original info
        pMgdLogManager->SetAdminLogInfo(bOriginalEnabled, originalName, originalParams);

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
        MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
        bool bResult = pMgdLogManager->ClearAdminLog();
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
    MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
    STRING path = pMgdLogManager->GetLogsPath();
    STRING originalName = pMgdLogManager->GetAdminLogFileName();
    pMgdLogManager->SetAdminLogFileName(TestName);

    STRING contents = Entry1 + Entry2 + Entry3 + Entry4 + LastEntry;
    CreateFile(path + TestName, contents);

    STRING logContents;
    STRING lastLogEntry;

    try
    {
        Ptr<MgByteReader> byteReader = pMgdLogManager->GetAdminLog();
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        byteReader = pMgdLogManager->GetAdminLog(1);
        lastLogEntry = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        pMgdLogManager->SetAdminLogFileName(originalName);
    }
    catch (MgException* e)
    {
        pMgdLogManager->SetAdminLogFileName(originalName);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        pMgdLogManager->SetAdminLogFileName(originalName);
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
    MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
    STRING path = pMgdLogManager->GetLogsPath();
    STRING originalName = pMgdLogManager->GetAdminLogFileName();
    pMgdLogManager->SetAdminLogFileName(DynamicTestName);

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
        byteReader = pMgdLogManager->GetAdminLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry1 + Entry2 + Entry3));

        // from & to date are the same and at exact time an entry was made
        fromDate = new MgDateTime(2005, 3, 7, 16, 51, 13, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 51, 13, 0);
        byteReader = pMgdLogManager->GetAdminLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == Entry1);

        // from & to date are the same and not at exact time an entry was made
        fromDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        byteReader = pMgdLogManager->GetAdminLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents.length() == 0);

        // from & to dates are at not at exact times an entry was made
        fromDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 56, 0, 0);
        byteReader = pMgdLogManager->GetAdminLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry2 + Entry3 + Entry4));

        // spans two different files, the second of which doesn't exist
        fromDate = new MgDateTime(2005, 3, 8, 0, 0, 0, 0);
        toDate = new MgDateTime(2005, 3, 9, 0, 0, 0, 0);
        byteReader = pMgdLogManager->GetAdminLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == LastEntry);

        // from date is after the latest entry in the log files
        fromDate = new MgDateTime(2006, 1, 1, 1, 0, 0, 0);
        toDate = new MgDateTime(2006, 1, 1, 1, 0, 0, 0);
        byteReader = pMgdLogManager->GetAdminLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents.length() == 0);

        // Use a null value for the date
        CPPUNIT_ASSERT_THROW_MG(pMgdLogManager->GetAdminLog(NULL, toDate), MgNullArgumentException*);

        // Use dates more than 24 hours apart
        fromDate = new MgDateTime(2005, 2, 18, 14, 0, 0, 0);
        toDate = new MgDateTime(2005, 2, 19, 18, 0, 0, 0);
        CPPUNIT_ASSERT_THROW_MG(pMgdLogManager->GetAdminLog(fromDate, toDate), MgInvalidArgumentException*);

        pMgdLogManager->SetAdminLogFileName(originalName);
    }
    catch (MgException* e)
    {
        pMgdLogManager->SetAdminLogFileName(originalName);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        pMgdLogManager->SetAdminLogFileName(originalName);
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
    MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
    CPPUNIT_ASSERT_THROW_MG(pMgdLogManager->GetAdminLog(-1), MgArgumentOutOfRangeException*);
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
        MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();

        // Save original information
        bool bOriginalEnabled = pMgdLogManager->IsAuthenticationLogEnabled();
        STRING originalName = pMgdLogManager->GetAuthenticationLogFileName();
        STRING originalParams = pMgdLogManager->GetAuthenticationLogParameters();

        pMgdLogManager->SetAuthenticationLogInfo(false, TestName, TestParameters);

        bool bEnabled = pMgdLogManager->IsAuthenticationLogEnabled();
        STRING name = pMgdLogManager->GetAuthenticationLogFileName();
        STRING params = pMgdLogManager->GetAuthenticationLogParameters();

        // Restore original info
        pMgdLogManager->SetAuthenticationLogInfo(bOriginalEnabled, originalName, originalParams);

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
        MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
        bool bResult = pMgdLogManager->ClearAuthenticationLog();
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
    MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
    STRING path = pMgdLogManager->GetLogsPath();
    STRING originalName = pMgdLogManager->GetAuthenticationLogFileName();
    pMgdLogManager->SetAuthenticationLogFileName(TestName);

    STRING contents = Entry1 + Entry2 + Entry3 + Entry4 + LastEntry;
    CreateFile(path + TestName, contents);

    STRING logContents;
    STRING lastLogEntry;

    try
    {
        Ptr<MgByteReader> byteReader = pMgdLogManager->GetAuthenticationLog();
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        byteReader = pMgdLogManager->GetAuthenticationLog(1);
        lastLogEntry = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        pMgdLogManager->SetAuthenticationLogFileName(originalName);
    }
    catch (MgException* e)
    {
        pMgdLogManager->SetAuthenticationLogFileName(originalName);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        pMgdLogManager->SetAuthenticationLogFileName(originalName);
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
    MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
    STRING path = pMgdLogManager->GetLogsPath();
    STRING originalName = pMgdLogManager->GetAuthenticationLogFileName();
    pMgdLogManager->SetAuthenticationLogFileName(DynamicTestName);

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
        byteReader = pMgdLogManager->GetAuthenticationLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry1 + Entry2 + Entry3));

        // from & to dates are at not at exact times an entry was made
        fromDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 56, 0, 0);
        byteReader = pMgdLogManager->GetAuthenticationLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry2 + Entry3 + Entry4));

        // spans two different files
        fromDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        toDate = new MgDateTime(2005, 3, 8, 1, 0, 0, 0);
        byteReader = pMgdLogManager->GetAuthenticationLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry2 + Entry3 + Entry4 + LastEntry));

        // spans two different files, the first of which doesn't exist
        fromDate = new MgDateTime(2005, 3, 6, 23, 59, 59, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        byteReader = pMgdLogManager->GetAuthenticationLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == Entry1);

        // spans two different files, the second of which doesn't exist
        fromDate = new MgDateTime(2005, 3, 8, 0, 0, 0, 0);
        toDate = new MgDateTime(2005, 3, 9, 0, 0, 0, 0);
        byteReader = pMgdLogManager->GetAuthenticationLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == LastEntry);

        // from date is after the latest entry in the log files
        fromDate = new MgDateTime(2006, 1, 1, 1, 0, 0, 0);
        toDate = new MgDateTime(2006, 1, 1, 1, 0, 0, 0);
        byteReader = pMgdLogManager->GetAuthenticationLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents.length() == 0);

        // Use a null value for the date
        CPPUNIT_ASSERT_THROW_MG(pMgdLogManager->GetAuthenticationLog(NULL, toDate), MgNullArgumentException*);

        // Use dates more than 24 hours apart
        fromDate = new MgDateTime(2005, 2, 18, 14, 0, 0, 0);
        toDate = new MgDateTime(2005, 2, 19, 18, 0, 0, 0);
        CPPUNIT_ASSERT_THROW_MG(pMgdLogManager->GetAuthenticationLog(fromDate, toDate), MgInvalidArgumentException*);

        pMgdLogManager->SetAuthenticationLogFileName(originalName);
    }
    catch (MgException* e)
    {
        pMgdLogManager->SetAuthenticationLogFileName(originalName);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        pMgdLogManager->SetAuthenticationLogFileName(originalName);
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
    MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
    CPPUNIT_ASSERT_THROW_MG(pMgdLogManager->GetAuthenticationLog(-1), MgArgumentOutOfRangeException*);
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
        MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();

        // Save original information
        bool bOriginalEnabled = pMgdLogManager->IsErrorLogEnabled();
        STRING originalName = pMgdLogManager->GetErrorLogFileName();
        STRING originalParams = pMgdLogManager->GetErrorLogParameters();

        pMgdLogManager->SetErrorLogInfo(false, TestName, TestParameters);

        bool bEnabled = pMgdLogManager->IsErrorLogEnabled();
        STRING name = pMgdLogManager->GetErrorLogFileName();
        STRING params = pMgdLogManager->GetErrorLogParameters();

        // Restore original info
        pMgdLogManager->SetErrorLogInfo(bOriginalEnabled, originalName, originalParams);

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
        MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
        bool bResult = pMgdLogManager->ClearErrorLog();
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
    MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
    STRING path = pMgdLogManager->GetLogsPath();
    STRING originalName = pMgdLogManager->GetErrorLogFileName();
    pMgdLogManager->SetErrorLogFileName(TestName);

    STRING contents = Entry1 + Entry2 + Entry3 + Entry4 + LastEntry;
    CreateFile(path + TestName, contents);

    STRING logContents;
    STRING lastLogEntry;

    try
    {
        Ptr<MgByteReader> byteReader = pMgdLogManager->GetErrorLog();
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        byteReader = pMgdLogManager->GetErrorLog(1);
        lastLogEntry = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        pMgdLogManager->SetErrorLogFileName(originalName);
    }
    catch (MgException* e)
    {
        pMgdLogManager->SetErrorLogFileName(originalName);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        pMgdLogManager->SetErrorLogFileName(originalName);
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
    MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
    STRING path = pMgdLogManager->GetLogsPath();
    STRING originalName = pMgdLogManager->GetErrorLogFileName();
    pMgdLogManager->SetErrorLogFileName(DynamicTestName);

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
        byteReader = pMgdLogManager->GetErrorLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry1 + Entry2 + Entry3));

        // from & to dates are at not at exact times an entry was made
        fromDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 56, 0, 0);
        byteReader = pMgdLogManager->GetErrorLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry2 + Entry3 + Entry4));

        // spans two different files
        fromDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        toDate = new MgDateTime(2005, 3, 8, 1, 0, 0, 0);
        byteReader = pMgdLogManager->GetErrorLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry2 + Entry3 + Entry4 + LastEntry));

        // spans two different files, the first of which doesn't exist
        fromDate = new MgDateTime(2005, 3, 6, 23, 59, 59, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        byteReader = pMgdLogManager->GetErrorLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == Entry1);

        // spans two different files, the second of which doesn't exist
        fromDate = new MgDateTime(2005, 3, 8, 0, 0, 0, 0);
        toDate = new MgDateTime(2005, 3, 9, 0, 0, 0, 0);
        byteReader = pMgdLogManager->GetErrorLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == LastEntry);

        // from date is after the latest entry in the log files
        fromDate = new MgDateTime(2006, 1, 1, 1, 0, 0, 0);
        toDate = new MgDateTime(2006, 1, 1, 1, 0, 0, 0);
        byteReader = pMgdLogManager->GetErrorLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents.length() == 0);

        // Use a null value for the date
        CPPUNIT_ASSERT_THROW_MG(pMgdLogManager->GetErrorLog(NULL, toDate), MgNullArgumentException*);

        // Use dates more than 24 hours apart
        fromDate = new MgDateTime(2005, 2, 18, 14, 0, 0, 0);
        toDate = new MgDateTime(2005, 2, 19, 18, 0, 0, 0);
        CPPUNIT_ASSERT_THROW_MG(pMgdLogManager->GetErrorLog(fromDate, toDate), MgInvalidArgumentException*);

        pMgdLogManager->SetErrorLogFileName(originalName);
    }
    catch (MgException* e)
    {
        pMgdLogManager->SetErrorLogFileName(originalName);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        pMgdLogManager->SetErrorLogFileName(originalName);
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
    MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
    CPPUNIT_ASSERT_THROW_MG(pMgdLogManager->GetErrorLog(-1), MgArgumentOutOfRangeException*);
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
        MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();

        // Save original information
        bool bOriginalEnabled = pMgdLogManager->IsSessionLogEnabled();
        STRING originalName = pMgdLogManager->GetSessionLogFileName();
        STRING originalParams = pMgdLogManager->GetSessionLogParameters();

        pMgdLogManager->SetSessionLogInfo(false, TestName, TestParameters);

        bool bEnabled = pMgdLogManager->IsSessionLogEnabled();
        STRING name = pMgdLogManager->GetSessionLogFileName();
        STRING params = pMgdLogManager->GetSessionLogParameters();

        // Restore original info
        pMgdLogManager->SetSessionLogInfo(bOriginalEnabled, originalName, originalParams);

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
        MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
        bool bResult = pMgdLogManager->ClearSessionLog();
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
    MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
    STRING path = pMgdLogManager->GetLogsPath();
    STRING originalName = pMgdLogManager->GetSessionLogFileName();
    pMgdLogManager->SetSessionLogFileName(TestName);

    STRING contents = Entry1 + Entry2 + Entry3 + Entry4 + LastEntry;
    CreateFile(path + TestName, contents);

    STRING logContents;
    STRING lastLogEntry;

    try
    {
        Ptr<MgByteReader> byteReader = pMgdLogManager->GetSessionLog();
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        byteReader = pMgdLogManager->GetSessionLog(1);
        lastLogEntry = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        pMgdLogManager->SetSessionLogFileName(originalName);
    }
    catch (MgException* e)
    {
        pMgdLogManager->SetSessionLogFileName(originalName);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        pMgdLogManager->SetSessionLogFileName(originalName);
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
    MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
    STRING path = pMgdLogManager->GetLogsPath();
    STRING originalName = pMgdLogManager->GetSessionLogFileName();
    pMgdLogManager->SetSessionLogFileName(DynamicTestName);

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
        byteReader = pMgdLogManager->GetSessionLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry1 + Entry2 + Entry3));

        // from & to dates are at not at exact times an entry was made
        fromDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 56, 0, 0);
        byteReader = pMgdLogManager->GetSessionLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry2 + Entry3 + Entry4));

        // spans two different files
        fromDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        toDate = new MgDateTime(2005, 3, 8, 1, 0, 0, 0);
        byteReader = pMgdLogManager->GetSessionLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry2 + Entry3 + Entry4 + LastEntry));

        // spans two different files, the first of which doesn't exist
        fromDate = new MgDateTime(2005, 3, 6, 23, 59, 59, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        byteReader = pMgdLogManager->GetSessionLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == Entry1);

        // spans two different files, the second of which doesn't exist
        fromDate = new MgDateTime(2005, 3, 8, 0, 0, 0, 0);
        toDate = new MgDateTime(2005, 3, 9, 0, 0, 0, 0);
        byteReader = pMgdLogManager->GetSessionLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == LastEntry);

        // from date is after the latest entry in the log files
        fromDate = new MgDateTime(2006, 1, 1, 1, 0, 0, 0);
        toDate = new MgDateTime(2006, 1, 1, 1, 0, 0, 0);
        byteReader = pMgdLogManager->GetSessionLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents.length() == 0);

        // Use a null value for the date
        CPPUNIT_ASSERT_THROW_MG(pMgdLogManager->GetSessionLog(NULL, toDate), MgNullArgumentException*);

        // Use dates more than 24 hours apart
        fromDate = new MgDateTime(2005, 2, 18, 14, 0, 0, 0);
        toDate = new MgDateTime(2005, 2, 19, 18, 0, 0, 0);
        CPPUNIT_ASSERT_THROW_MG(pMgdLogManager->GetSessionLog(fromDate, toDate), MgInvalidArgumentException*);

        pMgdLogManager->SetSessionLogFileName(originalName);
    }
    catch (MgException* e)
    {
        pMgdLogManager->SetSessionLogFileName(originalName);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        pMgdLogManager->SetSessionLogFileName(originalName);
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
    MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
    CPPUNIT_ASSERT_THROW_MG(pMgdLogManager->GetSessionLog(-1), MgArgumentOutOfRangeException*);
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
        MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();

        // Save original information
        bool bOriginalEnabled = pMgdLogManager->IsTraceLogEnabled();
        STRING originalName = pMgdLogManager->GetTraceLogFileName();
        STRING originalParams = pMgdLogManager->GetTraceLogParameters();

        pMgdLogManager->SetTraceLogInfo(false, TestName, TestParameters);

        bool bEnabled = pMgdLogManager->IsTraceLogEnabled();
        STRING name = pMgdLogManager->GetTraceLogFileName();
        STRING params = pMgdLogManager->GetTraceLogParameters();

        // Restore original info
        pMgdLogManager->SetTraceLogInfo(bOriginalEnabled, originalName, originalParams);

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
        MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
        bool bResult = pMgdLogManager->ClearTraceLog();
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
    MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
    STRING path = pMgdLogManager->GetLogsPath();
    STRING originalName = pMgdLogManager->GetTraceLogFileName();
    pMgdLogManager->SetTraceLogFileName(TestName);

    STRING contents = Entry1 + Entry2 + Entry3 + Entry4 + LastEntry;
    CreateFile(path + TestName, contents);

    STRING logContents;
    STRING lastLogEntry;

    try
    {
        Ptr<MgByteReader> byteReader = pMgdLogManager->GetTraceLog();
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        byteReader = pMgdLogManager->GetTraceLog(1);
        lastLogEntry = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        pMgdLogManager->SetTraceLogFileName(originalName);
    }
    catch (MgException* e)
    {
        pMgdLogManager->SetTraceLogFileName(originalName);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        pMgdLogManager->SetTraceLogFileName(originalName);
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
    MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
    STRING path = pMgdLogManager->GetLogsPath();
    STRING originalName = pMgdLogManager->GetTraceLogFileName();
    pMgdLogManager->SetTraceLogFileName(DynamicTestName);

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
        byteReader = pMgdLogManager->GetTraceLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry1 + Entry2 + Entry3));

        // from & to dates are at not at exact times an entry was made
        fromDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 56, 0, 0);
        byteReader = pMgdLogManager->GetTraceLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry2 + Entry3 + Entry4));

        // spans two different files
        fromDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        toDate = new MgDateTime(2005, 3, 8, 1, 0, 0, 0);
        byteReader = pMgdLogManager->GetTraceLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == (Entry2 + Entry3 + Entry4 + LastEntry));

        // spans two different files, the first of which doesn't exist
        fromDate = new MgDateTime(2005, 3, 6, 23, 59, 59, 0);
        toDate = new MgDateTime(2005, 3, 7, 16, 52, 0, 0);
        byteReader = pMgdLogManager->GetTraceLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == Entry1);

        // spans two different files, the second of which doesn't exist
        fromDate = new MgDateTime(2005, 3, 8, 0, 0, 0, 0);
        toDate = new MgDateTime(2005, 3, 9, 0, 0, 0, 0);
        byteReader = pMgdLogManager->GetTraceLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents == LastEntry);

        // from date is after the latest entry in the log files
        fromDate = new MgDateTime(2006, 1, 1, 1, 0, 0, 0);
        toDate = new MgDateTime(2006, 1, 1, 1, 0, 0, 0);
        byteReader = pMgdLogManager->GetTraceLog(fromDate, toDate);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));
        CPPUNIT_ASSERT(logContents.length() == 0);

        // Use a null value for the date
        CPPUNIT_ASSERT_THROW_MG(pMgdLogManager->GetTraceLog(NULL, toDate), MgNullArgumentException*);

        // Use dates more than 24 hours apart
        fromDate = new MgDateTime(2005, 2, 18, 14, 0, 0, 0);
        toDate = new MgDateTime(2005, 2, 19, 18, 0, 0, 0);
        CPPUNIT_ASSERT_THROW_MG(pMgdLogManager->GetTraceLog(fromDate, toDate), MgInvalidArgumentException*);

        pMgdLogManager->SetTraceLogFileName(originalName);
    }
    catch (MgException* e)
    {
        pMgdLogManager->SetTraceLogFileName(originalName);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        pMgdLogManager->SetTraceLogFileName(originalName);
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
    MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
    CPPUNIT_ASSERT_THROW_MG(pMgdLogManager->GetTraceLog(-1), MgArgumentOutOfRangeException*);
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

    MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
    bool bOriginalEnabled = pMgdLogManager->IsAccessLogEnabled();
    pMgdLogManager->SetAccessLogEnabled(true);

    STRING entry = L"TestAccessEntry";
    MG_LOG_ACCESS_ENTRY(entry, L"TestClient", L"TestClientIp", L"TestUser");

    // Give the server time to write out the entry as it is on a different thread
    ACE_OS::sleep(2);

    pMgdLogManager->SetAccessLogEnabled(bOriginalEnabled);

    try
    {
        byteReader = pMgdLogManager->GetAccessLog(1);
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

    MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
    bool bOriginalEnabled = pMgdLogManager->IsAdminLogEnabled();
    pMgdLogManager->SetAdminLogEnabled(true);

    STRING entry = L"TestAdminEntry";
    MG_LOG_ADMIN_ENTRY(entry, L"TestClient", L"TestClientIp", L"TestUser");

    // Give the server time to write out the entry as it is on a different thread
    ACE_OS::sleep(2);

    pMgdLogManager->SetAdminLogEnabled(bOriginalEnabled);

    try
    {
        byteReader = pMgdLogManager->GetAdminLog(1);
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

    MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
    bool bOriginalEnabled = pMgdLogManager->IsAuthenticationLogEnabled();
    pMgdLogManager->SetAuthenticationLogEnabled(true);

    STRING entry = L"TestAuthenticationEntry";
    MG_LOG_AUTHENTICATION_ENTRY(entry);

    // Give the server time to write out the entry as it is on a different thread
    ACE_OS::sleep(2);

    pMgdLogManager->SetAdminLogEnabled(bOriginalEnabled);

    try
    {
        byteReader = pMgdLogManager->GetAuthenticationLog(1);
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

    MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
    bool bOriginalEnabled = pMgdLogManager->IsErrorLogEnabled();
    pMgdLogManager->SetErrorLogEnabled(true);

    STRING entry = L"TestErrorEntry";
    MG_LOG_ERROR_ENTRY(entry);

    // Give the server time to write out the entry as it is on a different thread
    ACE_OS::sleep(2);

    pMgdLogManager->SetErrorLogEnabled(bOriginalEnabled);

    try
    {
        byteReader = pMgdLogManager->GetErrorLog(1);
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
void TestLogManager::TestCase_LogTraceEntry()
{
    Ptr<MgByteReader> byteReader;
    STRING logEntry;
    STRING::size_type pos;

    MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
    bool bOriginalEnabled = pMgdLogManager->IsTraceLogEnabled();
    pMgdLogManager->SetTraceLogEnabled(true);

    STRING entry = L"TestTraceEntry";
    MG_LOG_TRACE_ENTRY(entry);

    // Give the server time to write out the entry as it is on a different thread
    ACE_OS::sleep(2);

    pMgdLogManager->SetTraceLogEnabled(bOriginalEnabled);

    try
    {
        byteReader = pMgdLogManager->GetTraceLog(1);
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
    MgdLogManager* pMgdLogManager = MgdLogManager::GetInstance();
    STRING path = pMgdLogManager->GetLogsPath();
    STRING originalName = pMgdLogManager->GetAccessLogFileName();
    pMgdLogManager->SetAccessLogFileName(TestName);

    STRING contents = Entry1 + Entry2 + Entry3 + Entry4 + LastEntry;
    CreateFile(path + TestName, contents);

    STRING logContents;
    STRING lastLogEntry;

    try
    {
        Ptr<MgByteReader> byteReader;
        byteReader = pMgdLogManager->GetLogFile(TestName);
        logContents = MgUtil::MultiByteToWideChar(MgUtil::GetTextFromReader(byteReader));

        pMgdLogManager->SetAccessLogFileName(originalName);
    }
    catch (MgException* e)
    {
        pMgdLogManager->SetAccessLogFileName(originalName);
        STRING message = e->GetDetails(TEST_LOCALE);
        SAFE_RELEASE(e);
        CPPUNIT_FAIL(MG_WCHAR_TO_CHAR(message.c_str()));
    }
    catch (...)
    {
        pMgdLogManager->SetAccessLogFileName(originalName);
        throw;
    }

    CPPUNIT_ASSERT(logContents == contents);
}
