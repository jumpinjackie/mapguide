//
//  Copyright (C) 2004-2007 by Autodesk, Inc.
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

#ifndef TESTTRANSFORMMESH_H_
#define TESTTRANSFORMMESH_H_

#include <cppunit/extensions/HelperMacros.h>

#ifndef _WIN32
//Linux: different naming for string functions
#define stricmp strcasecmp
#endif

class TransformMesh;

class TestTransformMesh : public CppUnit::TestFixture
{
    CPPUNIT_TEST_SUITE(TestTransformMesh);
    CPPUNIT_TEST(TestStart); // This must be the very first unit test

    CPPUNIT_TEST(TestCase_TransformMesh_DefaultConstructor);
    CPPUNIT_TEST(TestCase_TransformMesh_LL84_to_WAG);
    CPPUNIT_TEST(TestCase_TransformMesh_LL84_to_LL84);
    CPPUNIT_TEST(TestCase_TransformMesh_ArbXYM_to_ArbXYM);
    CPPUNIT_TEST(TestCase_TransformMesh_ArbXYKM_to_ArbXYM);
    CPPUNIT_TEST(TestCase_TransformMesh_Small_Image_01);

    CPPUNIT_TEST(TestEnd); // This must be the very last unit test
    CPPUNIT_TEST_SUITE_END();

public:
    TestTransformMesh();

    void setUp();
    void tearDown();
    void TestStart();
    void TestEnd();

    MgCoordinateSystem* CreateCoordinateSystemArbitrary();
    MgCoordinateSystem* CreateCoordinateSystemGeographic();
    MgCoordinateSystem* CreateCoordinateSystemProjected();
    MgCoordinateSystem* CreateCoordinateSystem(const STRING wkt);

    bool CheckTransformMeshSize(int srcWidth, int srcHeight, TransformMesh* xform);

    void TestCase_TransformMesh_DefaultConstructor();
    void TestCase_TransformMesh_LL84_to_WAG();
    void TestCase_TransformMesh_LL84_to_LL84();
    void TestCase_TransformMesh_ArbXYM_to_ArbXYM();
    void TestCase_TransformMesh_ArbXYKM_to_ArbXYM();
    void TestCase_TransformMesh_Small_Image_01();


};

#endif
