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

#ifndef _TESTPERFORMANCE_H
#define _TESTPERFORMANCE_H

#include <cppunit/extensions/HelperMacros.h>

class TestPerformance : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestPerformance);
    CPPUNIT_TEST(TestStart); // This must be the very first unit test

    CPPUNIT_TEST(TestCase_BenchmarkSelectFeatures);
    CPPUNIT_TEST(TestCase_BenchmarkJoinFeatures);

    CPPUNIT_TEST(TestEnd); // This must be the very last unit test
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();
    void TestStart();
    void TestEnd();

    void TestCase_BenchmarkSelectFeatures();
    void TestCase_BenchmarkJoinFeatures();
};

#endif // _TESTPERFORMANCE_H
