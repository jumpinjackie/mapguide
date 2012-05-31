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
#include "TestMisc.h"
#include "../UnitTesting/CppUnitExtensions.h"
#include "FoundationDefs.h"
const STRING TEST_LOCALE = L"en";

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestMisc, "TestMisc");

TestMisc::TestMisc()
{
}


TestMisc::~TestMisc()
{
}


void TestMisc::setUp()
{
}


void TestMisc::tearDown()
{
}


void TestMisc::TestStart()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nRunning Miscellaneous tests.\n")));
}


void TestMisc::TestEnd()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nMiscellaneous tests completed.\n\n")));
}

void TestMisc::TestCase_CommonExceptionMessages()
{
    try
    {
        MgObject* ptr = NULL;
        CHECKNULL(ptr, L"TestMisc::TestCase_CommonExceptionMessages");
    }
    catch(MgException* ex)
    {
        STRING msg = ex->GetExceptionMessage(TEST_LOCALE);
        //ACE_DEBUG((LM_INFO, ACE_TEXT("Exception: %W\n"), msg.c_str()));
        std::string assertMsg = "Expected identifier 'ptr' in exception message: ";
        assertMsg += MgUtil::WideCharToMultiByte(msg);
        CPPUNIT_ASSERT_MESSAGE(assertMsg, msg.find(L"ptr") != STRING::npos);
        ex->Release();
    }

    try
    {
        MgObject* ptr2 = NULL;
        CHECKNULL((void*)ptr2, L"TestMisc::TestCase_CommonExceptionMessages");
    }
    catch(MgException* ex)
    {
        STRING msg = ex->GetExceptionMessage(TEST_LOCALE);
        //ACE_DEBUG((LM_INFO, ACE_TEXT("Exception: %W\n"), msg.c_str()));
        std::string assertMsg = "Expected identifier 'ptr2' in exception message: ";
        assertMsg += MgUtil::WideCharToMultiByte(msg);
        CPPUNIT_ASSERT_MESSAGE(assertMsg, msg.find(L"ptr2") != STRING::npos);
        ex->Release();
    }

    try
    {
        MgObject* ptr3 = NULL;
        CHECKARGUMENTNULL(ptr3, L"TestMisc::TestCase_CommonExceptionMessages");
    }
    catch(MgException* ex)
    {
        STRING msg = ex->GetExceptionMessage(TEST_LOCALE);
        //ACE_DEBUG((LM_INFO, ACE_TEXT("Exception: %W\n"), msg.c_str()));
        std::string assertMsg = "Expected identifier 'ptr3' in exception message: ";
        assertMsg += MgUtil::WideCharToMultiByte(msg);
        CPPUNIT_ASSERT_MESSAGE(assertMsg, msg.find(L"ptr3") != STRING::npos);
        ex->Release();
    }

    try
    {
        MgObject* ptr4 = NULL;
        CHECKARGUMENTNULL((void*)ptr4, L"TestMisc::TestCase_CommonExceptionMessages");
    }
    catch(MgException* ex)
    {
        STRING msg = ex->GetExceptionMessage(TEST_LOCALE);
        //ACE_DEBUG((LM_INFO, ACE_TEXT("Exception: %W\n"), msg.c_str()));
        std::string assertMsg = "Expected identifier 'ptr4' in exception message: ";
        assertMsg += MgUtil::WideCharToMultiByte(msg);
        CPPUNIT_ASSERT_MESSAGE(assertMsg, msg.find(L"ptr4") != STRING::npos);
        ex->Release();
    }

    try
    {
        INT32 val = 1;
        MG_CHECK_RANGE(val, 2, 3, L"TestMisc::TestCase_CommonExceptionMessages");
    }
    catch(MgException* ex)
    {
        STRING msg = ex->GetExceptionMessage(TEST_LOCALE);
        //ACE_DEBUG((LM_INFO, ACE_TEXT("Exception: %W\n"), msg.c_str()));
        std::string assertMsg = "Expected identifier 'val' in exception message: ";
        assertMsg += MgUtil::WideCharToMultiByte(msg);
        CPPUNIT_ASSERT_MESSAGE(assertMsg, msg.find(L"val") != STRING::npos);
        ex->Release();
    }

    try
    {
        INT32 val = 1;
        INT32 min = 2;
        INT32 max = 3;
        MG_CHECK_RANGE(val, min, max, L"TestMisc::TestCase_CommonExceptionMessages");
    }
    catch(MgException* ex)
    {
        STRING msg = ex->GetExceptionMessage(TEST_LOCALE);
        //ACE_DEBUG((LM_INFO, ACE_TEXT("Exception: %W\n"), msg.c_str()));
        std::string assertMsg = "Expected identifier '' in exception message: ";
        assertMsg += MgUtil::WideCharToMultiByte(msg);
        CPPUNIT_ASSERT_MESSAGE(assertMsg, msg.find(L"") != STRING::npos);
        ex->Release();
    }

    try
    {
        INT64 val = 1L;
        MG_CHECK_RANGE(val, 2L, 3L, L"TestMisc::TestCase_CommonExceptionMessages");
    }
    catch(MgException* ex)
    {
        STRING msg = ex->GetExceptionMessage(TEST_LOCALE);
        //ACE_DEBUG((LM_INFO, ACE_TEXT("Exception: %W\n"), msg.c_str()));
        std::string assertMsg = "Expected identifier '' in exception message: ";
        assertMsg += MgUtil::WideCharToMultiByte(msg);
        CPPUNIT_ASSERT_MESSAGE(assertMsg, msg.find(L"") != STRING::npos);
        ex->Release();
    }

    try
    {
        INT64 val = 1L;
        INT64 min = 2L;
        INT64 max = 3L;
        MG_CHECK_RANGE(val, min, max, L"TestMisc::TestCase_CommonExceptionMessages");
    }
    catch(MgException* ex)
    {
        STRING msg = ex->GetExceptionMessage(TEST_LOCALE);
        //ACE_DEBUG((LM_INFO, ACE_TEXT("Exception: %W\n"), msg.c_str()));
        std::string assertMsg = "Expected identifier '' in exception message: ";
        assertMsg += MgUtil::WideCharToMultiByte(msg);
        CPPUNIT_ASSERT_MESSAGE(assertMsg, msg.find(L"") != STRING::npos);
        ex->Release();
    }

    try
    {
        float val = 1.0f;
        MG_CHECK_RANGE(val, 2.0f, 3.0f, L"TestMisc::TestCase_CommonExceptionMessages");
    }
    catch(MgException* ex)
    {
        STRING msg = ex->GetExceptionMessage(TEST_LOCALE);
        //ACE_DEBUG((LM_INFO, ACE_TEXT("Exception: %W\n"), msg.c_str()));
        std::string assertMsg = "Expected identifier '' in exception message: ";
        assertMsg += MgUtil::WideCharToMultiByte(msg);
        CPPUNIT_ASSERT_MESSAGE(assertMsg, msg.find(L"") != STRING::npos);
        ex->Release();
    }

    try
    {
        float val = 1.0f;
        float min = 2.0f;
        float max = 3.0f;
        MG_CHECK_RANGE(val, min, max, L"TestMisc::TestCase_CommonExceptionMessages");
    }
    catch(MgException* ex)
    {
        STRING msg = ex->GetExceptionMessage(TEST_LOCALE);
        //ACE_DEBUG((LM_INFO, ACE_TEXT("Exception: %W\n"), msg.c_str()));
        std::string assertMsg = "Expected identifier '' in exception message: ";
        assertMsg += MgUtil::WideCharToMultiByte(msg);
        CPPUNIT_ASSERT_MESSAGE(assertMsg, msg.find(L"") != STRING::npos);
        ex->Release();
    }

    try
    {
        double val = 1.0;
        MG_CHECK_RANGE(val, 2.0, 3.0, L"TestMisc::TestCase_CommonExceptionMessages");
    }
    catch(MgException* ex)
    {
        STRING msg = ex->GetExceptionMessage(TEST_LOCALE);
        //ACE_DEBUG((LM_INFO, ACE_TEXT("Exception: %W\n"), msg.c_str()));
        std::string assertMsg = "Expected identifier '' in exception message: ";
        assertMsg += MgUtil::WideCharToMultiByte(msg);
        CPPUNIT_ASSERT_MESSAGE(assertMsg, msg.find(L"") != STRING::npos);
        ex->Release();
    }

    try
    {
        double val = 1.0;
        double min = 2.0;
        double max = 3.0;
        MG_CHECK_RANGE(val, min, max, L"TestMisc::TestCase_CommonExceptionMessages");
    }
    catch(MgException* ex)
    {
        STRING msg = ex->GetExceptionMessage(TEST_LOCALE);
        //ACE_DEBUG((LM_INFO, ACE_TEXT("Exception: %W\n"), msg.c_str()));
        std::string assertMsg = "Expected identifier '' in exception message: ";
        assertMsg += MgUtil::WideCharToMultiByte(msg);
        CPPUNIT_ASSERT_MESSAGE(assertMsg, msg.find(L"") != STRING::npos);
        ex->Release();
    }

    try
    {
        Ptr<MgNamedCollection> coll = new MgNamedCollection(false);
        coll->GetItem(L"Foo");
    }
    catch(MgException* ex)
    {
        STRING msg = ex->GetExceptionMessage(TEST_LOCALE);
        //ACE_DEBUG((LM_INFO, ACE_TEXT("Exception: %W\n"), msg.c_str()));
        std::string assertMsg = "Expected string 'Foo' in exception message: ";
        assertMsg += MgUtil::WideCharToMultiByte(msg);
        CPPUNIT_ASSERT_MESSAGE(assertMsg, msg.find(L"Foo") != STRING::npos);
        ex->Release();
    }
}