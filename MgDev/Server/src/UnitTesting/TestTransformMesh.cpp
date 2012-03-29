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
#include "TestTransformMesh.h"
#include "CppUnitExtensions.h"

#include "TransformMesh.h"
#include "MgCSTrans.h"
#include "Bounds.h"
#include "FoundationDefs.h"


const STRING GeographicWkt      = L"GEOGCS[\"LL84\",DATUM[\"WGS84\",SPHEROID[\"WGS84\",6378137.000,298.25722293]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.01745329251994]]";
const STRING ProjectedWkt       = L"PROJCS[\"GA-W\",GEOGCS[\"LL27\",DATUM[\"NAD27\",SPHEROID[\"CLRK66\",6378206.400,294.97869821]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"false_easting\",500000.000],PARAMETER[\"false_northing\",0.000],PARAMETER[\"scale_factor\",0.999900000000],PARAMETER[\"central_meridian\",-84.16666666666670],PARAMETER[\"latitude_of_origin\",30.00000000000000],UNIT[\"Foot_US\",0.30480060960122]]";
const STRING ArbitraryWkt       = L"LOCAL_CS [ \"Non-Earth (Meter)\", LOCAL_DATUM [\"Local Datum\", 0], UNIT [\"Meter\", 1.0], AXIS [\"X\", EAST], AXIS[\"Y\", NORTH]]";

const STRING ArbitraryWkt_Feet  = L"LOCAL_CS [ \"Non-Earth (U.S. Foot)\", LOCAL_DATUM [\"Local Datum\", 0], UNIT [\"U.S. Foot\", 3.048006096012e-01], AXIS [\"X\", EAST], AXIS[\"Y\", NORTH]]";
const STRING ArbitraryWkt_Meter = L"LOCAL_CS [ \"Non-Earth (Meter)\", LOCAL_DATUM [\"Local Datum\", 0], UNIT [\"Meter\", 1.0], AXIS [\"X\", EAST], AXIS[\"Y\", NORTH]]";
const STRING ArbitraryWkt_KM    = L"LOCAL_CS [ \"Non-Earth (Kilometer)\", LOCAL_DATUM [\"Local Datum\", 0], UNIT [\"Kilometer\", 1.0], AXIS [\"X\", EAST], AXIS[\"Y\", NORTH]]";

const STRING GeographicWkt_LL84 = L"GEOGCS[\"LL84\",DATUM[\"WGS84\",SPHEROID[\"WGS84\",6378137.000,298.25722293]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]]";
const STRING ProjectedWkt_GAW   = L"PROJCS[\"GA-W\",GEOGCS[\"LL27\",DATUM[\"NAD27\",SPHEROID[\"CLRK66\",6378206.400,294.97869821]],PRIMEM[\"Greenwich\",0],UNIT[\"Degree\",0.017453292519943295]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"false_easting\",500000.000],PARAMETER[\"false_northing\",0.000],PARAMETER[\"scale_factor\",0.999900000000],PARAMETER[\"central_meridian\",-84.16666666666670],PARAMETER[\"latitude_of_origin\",30.00000000000000],UNIT[\"Foot_US\",0.30480060960122]]";



const STRING TEST_LOCALE = L"en";

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(TestTransformMesh, "TestTransformMesh");

void TestTransformMesh::setUp()
{
}


void TestTransformMesh::tearDown()
{
}


void TestTransformMesh::TestStart()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nRunning Transform Mesh tests.\n")));
}


void TestTransformMesh::TestEnd()
{
    ACE_DEBUG((LM_INFO, ACE_TEXT("\nTransform Mesh tests completed.\n\n")));
}


TestTransformMesh::TestTransformMesh()
{
}


MgCoordinateSystem* TestTransformMesh::CreateCoordinateSystemArbitrary()
{
    return CreateCoordinateSystem(ArbitraryWkt);
}

MgCoordinateSystem* TestTransformMesh::CreateCoordinateSystemGeographic()
{
    return CreateCoordinateSystem(GeographicWkt);
}


MgCoordinateSystem* TestTransformMesh::CreateCoordinateSystemProjected()
{
    return CreateCoordinateSystem(ProjectedWkt);
}

MgCoordinateSystem* TestTransformMesh::CreateCoordinateSystem(const STRING wkt)
{
    MgCoordinateSystemFactory factory;
    return factory.Create(wkt);
}

bool TestTransformMesh::CheckTransformMeshSize(int srcWidth, int srcHeight, TransformMesh* xformMesh)
{
    bool result = false;

    if (xformMesh->GetTotalPoints() == ((int)(srcHeight / xformMesh->GetGridSizeHeight()) + 1) * ((int)(srcWidth / xformMesh->GetGridSizeWidth()) + 1)
        && xformMesh->GetTotalVerticalPoints() == (int)(srcHeight / xformMesh->GetGridSizeHeight()) + 1
        && xformMesh->GetTotalHorizontalPoints() == (int)(srcWidth / xformMesh->GetGridSizeWidth()) + 1)
    {
        result = true;
    }

    return result;
}

void TestTransformMesh::TestCase_TransformMesh_DefaultConstructor()
{
    try
    {
        TransformMesh xformMesh;

        CPPUNIT_ASSERT(xformMesh.GetTotalPoints() == 0);
        CPPUNIT_ASSERT(xformMesh.GetTotalVerticalPoints() == 0);
        CPPUNIT_ASSERT(xformMesh.GetTotalHorizontalPoints() == 0);
        CPPUNIT_ASSERT(xformMesh.IsYAxisInverted() == true);

        CPPUNIT_ASSERT(true);
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

void TestTransformMesh::TestCase_TransformMesh_LL84_to_WAG()
{
    try
    {
        Ptr<MgCoordinateSystem> src = CreateCoordinateSystem(GeographicWkt_LL84);
        Ptr<MgCoordinateSystem> dst = CreateCoordinateSystem(ProjectedWkt_GAW);

        MgCSTrans xformer(src, dst);

        int gridSize = 100;
        int minGridSize = 10;
        double gridSizeOverride = 1;
        int srcWidth = 800;
        int srcHeight = 600;
        int destWidth = 800;
        int destHeight = 600;

        RS_Bounds srcExt(-86, 29.25, -80.3, 34.59); // centered around georgia, usa in LL84
        RS_Bounds destExt(-44705, -267000, 1661134, 1692341); // centered around georgia, usa in GA-W

        TransformMesh xformMesh(gridSize, minGridSize, gridSizeOverride, srcExt, srcWidth, srcHeight, destExt, destWidth, destHeight, &xformer);

        CPPUNIT_ASSERT(CheckTransformMeshSize(srcWidth, srcHeight, &xformMesh));
        CPPUNIT_ASSERT(xformMesh.IsYAxisInverted() == true);

        CPPUNIT_ASSERT(true);
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

void TestTransformMesh::TestCase_TransformMesh_LL84_to_LL84()
{
    try
    {
        // source and dest CS are the same
        Ptr<MgCoordinateSystem> src = CreateCoordinateSystem(GeographicWkt_LL84);
        Ptr<MgCoordinateSystem> dst = CreateCoordinateSystem(GeographicWkt_LL84);

        MgCSTrans xformer(src, dst);
        CPPUNIT_ASSERT(xformer.GetLinearScale() == 1);

        int gridSize = 100;
        int minGridSize = 10;
        double gridSizeOverride = 1;
        int srcWidth = gridSize;
        int srcHeight = gridSize;
        int destWidth = gridSize;
        int destHeight = gridSize;

        RS_Bounds srcExt(-140, -50, 140, 50);

        TransformMesh xformMesh(gridSize, minGridSize, gridSizeOverride, srcExt, srcWidth, srcHeight, srcExt, destWidth, destHeight, &xformer, false);

        CPPUNIT_ASSERT(CheckTransformMeshSize(srcWidth, srcHeight, &xformMesh));
        CPPUNIT_ASSERT(xformMesh.IsYAxisInverted() == false);

        // check to see that the source pt and dest pt are the same
        CPPUNIT_ASSERT(xformMesh.GetMeshPoint(0).pt_src.x == xformMesh.GetMeshPoint(0).pt_dest.x);
        CPPUNIT_ASSERT(xformMesh.GetMeshPoint(1).pt_src.x == xformMesh.GetMeshPoint(1).pt_dest.x);
        CPPUNIT_ASSERT(xformMesh.GetMeshPoint(2).pt_src.x == xformMesh.GetMeshPoint(2).pt_dest.x);
        CPPUNIT_ASSERT(xformMesh.GetMeshPoint(3).pt_src.x == xformMesh.GetMeshPoint(3).pt_dest.x);
        CPPUNIT_ASSERT(xformMesh.GetMeshPoint(0).pt_src.y == xformMesh.GetMeshPoint(0).pt_dest.y);
        CPPUNIT_ASSERT(xformMesh.GetMeshPoint(1).pt_src.y == xformMesh.GetMeshPoint(1).pt_dest.y);
        CPPUNIT_ASSERT(xformMesh.GetMeshPoint(2).pt_src.y == xformMesh.GetMeshPoint(2).pt_dest.y);
        CPPUNIT_ASSERT(xformMesh.GetMeshPoint(3).pt_src.y == xformMesh.GetMeshPoint(3).pt_dest.y);

        CPPUNIT_ASSERT(true);
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

void TestTransformMesh::TestCase_TransformMesh_ArbXYM_to_ArbXYM()
{
    try
    {
        // source and dest CS are the same
        Ptr<MgCoordinateSystem> src = CreateCoordinateSystem(ArbitraryWkt_Meter);
        Ptr<MgCoordinateSystem> dst = CreateCoordinateSystem(ArbitraryWkt_Meter);

        MgCSTrans xformer(src, dst);
        CPPUNIT_ASSERT(xformer.GetLinearScale() == 1);

        int gridSize = 100;
        int minGridSize = 10;
        double gridSizeOverride = 1;
        int srcWidth = gridSize;
        int srcHeight = gridSize;
        int destWidth = gridSize;
        int destHeight = gridSize;

        RS_Bounds srcExt(0, 0, gridSize, gridSize);

        TransformMesh xformMesh(gridSize, minGridSize, gridSizeOverride, srcExt, srcWidth, srcHeight, srcExt, destWidth, destHeight, &xformer, false);

        CPPUNIT_ASSERT(CheckTransformMeshSize(srcWidth, srcHeight, &xformMesh));
        CPPUNIT_ASSERT(xformMesh.IsYAxisInverted() == false);

        // check to see that the source pt and dest pt are the same
        CPPUNIT_ASSERT(xformMesh.GetMeshPoint(0).pt_src.x == xformMesh.GetMeshPoint(0).pt_dest.x);
        CPPUNIT_ASSERT(xformMesh.GetMeshPoint(1).pt_src.x == xformMesh.GetMeshPoint(1).pt_dest.x);
        CPPUNIT_ASSERT(xformMesh.GetMeshPoint(2).pt_src.x == xformMesh.GetMeshPoint(2).pt_dest.x);
        CPPUNIT_ASSERT(xformMesh.GetMeshPoint(3).pt_src.x == xformMesh.GetMeshPoint(3).pt_dest.x);
        CPPUNIT_ASSERT(xformMesh.GetMeshPoint(0).pt_src.y == xformMesh.GetMeshPoint(0).pt_dest.y);
        CPPUNIT_ASSERT(xformMesh.GetMeshPoint(1).pt_src.y == xformMesh.GetMeshPoint(1).pt_dest.y);
        CPPUNIT_ASSERT(xformMesh.GetMeshPoint(2).pt_src.y == xformMesh.GetMeshPoint(2).pt_dest.y);
        CPPUNIT_ASSERT(xformMesh.GetMeshPoint(3).pt_src.y == xformMesh.GetMeshPoint(3).pt_dest.y);

        CPPUNIT_ASSERT(true);
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

void TestTransformMesh::TestCase_TransformMesh_ArbXYKM_to_ArbXYM()
{
    try
    {
        Ptr<MgCoordinateSystem> src = CreateCoordinateSystem(ArbitraryWkt_KM);
        Ptr<MgCoordinateSystem> dst = CreateCoordinateSystem(ArbitraryWkt_Meter);

        MgCSTrans xformer(src, dst);
        double dLinearScale = xformer.GetLinearScale();
        CPPUNIT_ASSERT(dLinearScale == 1000);

        int gridSize = 100;
        int minGridSize = 10;
        double gridSizeOverride = 1;
        int srcWidth = gridSize;
        int srcHeight = gridSize;
        int destWidth = gridSize;
        int destHeight = gridSize;

        RS_Bounds srcExt(0, 0, gridSize, gridSize);

        TransformMesh xformMesh(gridSize, minGridSize, gridSizeOverride, srcExt, srcWidth, srcHeight, srcExt, destWidth, destHeight, &xformer, false);

        CPPUNIT_ASSERT(CheckTransformMeshSize(srcWidth, srcHeight, &xformMesh));
        CPPUNIT_ASSERT(xformMesh.IsYAxisInverted() == false);

        // pt1----pt2
        // |        |
        // |        |
        // |        |
        // pt0----pt3

        // pt0 should be the same
        CPPUNIT_ASSERT(xformMesh.GetMeshPoint(0).pt_src.x == xformMesh.GetMeshPoint(0).pt_dest.x);
        CPPUNIT_ASSERT(xformMesh.GetMeshPoint(0).pt_src.y == xformMesh.GetMeshPoint(0).pt_dest.y);

        // the pt1, pt2, pt3 should be scaled by dLinearScale (1000)
        CPPUNIT_ASSERT(xformMesh.GetMeshPoint(1).pt_src.x == xformMesh.GetMeshPoint(1).pt_dest.x);
        CPPUNIT_ASSERT(dLinearScale * xformMesh.GetMeshPoint(1).pt_src.y == xformMesh.GetMeshPoint(1).pt_dest.y);

        CPPUNIT_ASSERT(dLinearScale * xformMesh.GetMeshPoint(2).pt_src.x == xformMesh.GetMeshPoint(2).pt_dest.x);
        CPPUNIT_ASSERT(xformMesh.GetMeshPoint(2).pt_src.y == xformMesh.GetMeshPoint(2).pt_dest.y);

        CPPUNIT_ASSERT(dLinearScale * xformMesh.GetMeshPoint(3).pt_src.x == xformMesh.GetMeshPoint(3).pt_dest.x);
        CPPUNIT_ASSERT(dLinearScale * xformMesh.GetMeshPoint(3).pt_src.y == xformMesh.GetMeshPoint(3).pt_dest.y);

        CPPUNIT_ASSERT(true);
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

void TestTransformMesh::TestCase_TransformMesh_Small_Image_01()
{
    try
    {
        Ptr<MgCoordinateSystem> src = CreateCoordinateSystem(ArbitraryWkt_KM);
        Ptr<MgCoordinateSystem> dst = CreateCoordinateSystem(ArbitraryWkt_Meter);

        MgCSTrans xformer(src, dst);
        double dLinearScale = xformer.GetLinearScale();
        CPPUNIT_ASSERT(dLinearScale == 1000);

        int gridSize = 100;
        int minGridSize = 10;
        double gridSizeOverride = 1;
        int srcWidth = 5;
        int srcHeight = 5;
        int destWidth = 5;
        int destHeight = 5;

        RS_Bounds srcExt(0, 0, srcWidth, srcHeight);

        TransformMesh xformMesh(gridSize, minGridSize, gridSizeOverride, srcExt, srcWidth, srcHeight, srcExt, destWidth, destHeight, &xformer, false);

        CPPUNIT_ASSERT(CheckTransformMeshSize(srcWidth, srcHeight, &xformMesh));
        CPPUNIT_ASSERT(xformMesh.IsYAxisInverted() == false);

        // pt1----pt2
        // |        |
        // |        |
        // |        |
        // pt0----pt3

        // pt0 should be the same
        CPPUNIT_ASSERT(xformMesh.GetMeshPoint(0).pt_src.x == xformMesh.GetMeshPoint(0).pt_dest.x);
        CPPUNIT_ASSERT(xformMesh.GetMeshPoint(0).pt_src.y == xformMesh.GetMeshPoint(0).pt_dest.y);

        // the pt1, pt2, pt3 should be scaled by dLinearScale (1000)
        CPPUNIT_ASSERT(xformMesh.GetMeshPoint(1).pt_src.x == xformMesh.GetMeshPoint(1).pt_dest.x);
        CPPUNIT_ASSERT(dLinearScale * xformMesh.GetMeshPoint(1).pt_src.y == xformMesh.GetMeshPoint(1).pt_dest.y);

        CPPUNIT_ASSERT(dLinearScale * xformMesh.GetMeshPoint(2).pt_src.x == xformMesh.GetMeshPoint(2).pt_dest.x);
        CPPUNIT_ASSERT(xformMesh.GetMeshPoint(2).pt_src.y == xformMesh.GetMeshPoint(2).pt_dest.y);

        CPPUNIT_ASSERT(dLinearScale * xformMesh.GetMeshPoint(3).pt_src.x == xformMesh.GetMeshPoint(3).pt_dest.x);
        CPPUNIT_ASSERT(dLinearScale * xformMesh.GetMeshPoint(3).pt_src.y == xformMesh.GetMeshPoint(3).pt_dest.y);

        CPPUNIT_ASSERT(true);
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
