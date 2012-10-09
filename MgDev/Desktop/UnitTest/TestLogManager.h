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

#ifndef _TESTLOGMANAGER_H
#define _TESTLOGMANAGER_H

#include <cppunit/extensions/HelperMacros.h>

class TestLogManager : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestLogManager);
    CPPUNIT_TEST(TestStart); // This must be the very first unit test

    CPPUNIT_TEST(TestCase_ValidLogManager);
    CPPUNIT_TEST(TestCase_GetLogsPath);
    CPPUNIT_TEST(TestCase_TestForDeadLock);

    CPPUNIT_TEST(TestCase_EnumerateLogs);
    CPPUNIT_TEST(TestCase_RenameLog);

    CPPUNIT_TEST(TestCase_SetAccessLogInfo);
    CPPUNIT_TEST(TestCase_ClearAccessLog);
    CPPUNIT_TEST(TestCase_GetAccessLog);
    CPPUNIT_TEST(TestCase_GetAccessLogByDate);
    CPPUNIT_TEST(TestCase_GetAccessLogInvalid);

    CPPUNIT_TEST(TestCase_SetAdminLogInfo);
    CPPUNIT_TEST(TestCase_ClearAdminLog);
    CPPUNIT_TEST(TestCase_GetAdminLog);
    CPPUNIT_TEST(TestCase_GetAdminLogByDate);
    CPPUNIT_TEST(TestCase_GetAdminLogInvalid);

    CPPUNIT_TEST(TestCase_SetAuthenticationLogInfo);
    CPPUNIT_TEST(TestCase_ClearAuthenticationLog);
    CPPUNIT_TEST(TestCase_GetAuthenticationLog);
    CPPUNIT_TEST(TestCase_GetAuthenticationLogByDate);
    CPPUNIT_TEST(TestCase_GetAuthenticationLogInvalid);

    CPPUNIT_TEST(TestCase_SetErrorLogInfo);
    CPPUNIT_TEST(TestCase_ClearErrorLog);
    CPPUNIT_TEST(TestCase_GetErrorLog);
    CPPUNIT_TEST(TestCase_GetErrorLogByDate);
    CPPUNIT_TEST(TestCase_GetErrorLogInvalid);

    CPPUNIT_TEST(TestCase_SetSessionLogInfo);
    CPPUNIT_TEST(TestCase_ClearSessionLog);
    CPPUNIT_TEST(TestCase_GetSessionLog);
    CPPUNIT_TEST(TestCase_GetSessionLogByDate);
    CPPUNIT_TEST(TestCase_GetSessionLogInvalid);

    CPPUNIT_TEST(TestCase_SetTraceLogInfo);
    CPPUNIT_TEST(TestCase_ClearTraceLog);
    CPPUNIT_TEST(TestCase_GetTraceLog);
    CPPUNIT_TEST(TestCase_GetTraceLogByDate);
    CPPUNIT_TEST(TestCase_GetTraceLogInvalid);

    CPPUNIT_TEST(TestCase_LogAccessEntry);
    //CPPUNIT_TEST(TestCase_LogAdminEntry);             //Not applicable for mg-desktop
    //CPPUNIT_TEST(TestCase_LogAuthenticationEntry);    //Not applicable for mg-desktop
    CPPUNIT_TEST(TestCase_LogErrorEntry);
    CPPUNIT_TEST(TestCase_LogTraceEntry);

    CPPUNIT_TEST(TestCase_GetLogFile);

    CPPUNIT_TEST(TestCase_DeleteLog);

    CPPUNIT_TEST(TestEnd); // This must be the very last unit test
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();
    void TestStart();
    void TestEnd();
    bool CreateFile(STRING filename, STRING contents);

    void TestCase_ValidLogManager();
    void TestCase_GetLogsPath();
    void TestCase_TestForDeadLock();

    void TestCase_EnumerateLogs();
    void TestCase_DeleteLog();
    void TestCase_RenameLog();

    void TestCase_SetAccessLogInfo();
    void TestCase_ClearAccessLog();
    void TestCase_GetAccessLog();
    void TestCase_GetAccessLogByDate();
    void TestCase_GetAccessLogInvalid();

    void TestCase_SetAdminLogInfo();
    void TestCase_ClearAdminLog();
    void TestCase_GetAdminLog();
    void TestCase_GetAdminLogByDate();
    void TestCase_GetAdminLogInvalid();

    void TestCase_SetAuthenticationLogInfo();
    void TestCase_ClearAuthenticationLog();
    void TestCase_GetAuthenticationLog();
    void TestCase_GetAuthenticationLogByDate();
    void TestCase_GetAuthenticationLogInvalid();

    void TestCase_SetErrorLogInfo();
    void TestCase_ClearErrorLog();
    void TestCase_GetErrorLog();
    void TestCase_GetErrorLogByDate();
    void TestCase_GetErrorLogInvalid();

    void TestCase_SetSessionLogInfo();
    void TestCase_ClearSessionLog();
    void TestCase_GetSessionLog();
    void TestCase_GetSessionLogByDate();
    void TestCase_GetSessionLogInvalid();

    void TestCase_SetTraceLogInfo();
    void TestCase_ClearTraceLog();
    void TestCase_GetTraceLog();
    void TestCase_GetTraceLogByDate();
    void TestCase_GetTraceLogInvalid();

    void TestCase_LogAccessEntry();
    void TestCase_LogAdminEntry();
    void TestCase_LogAuthenticationEntry();
    void TestCase_LogErrorEntry();
    void TestCase_LogSessionEntry();
    void TestCase_LogTraceEntry();

    void TestCase_GetLogFile();
};

#endif // _TESTLOGMANAGER_H
