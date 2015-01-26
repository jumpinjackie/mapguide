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

// disable C4244 of io.h about int64 assigned to unsigned long
#pragma warning(push)
#pragma warning(disable: 4244)
#include "geos.h"
#pragma warning(pop)

#include "GeometryCommon.h"
#include "Buffer/buffer.h"
#include "GeosUtil.h"
#include "GeometryExceptionDef.h"
#include "Spatial/SpatialUtility.h"
#include "GeometricEntityType.h"

using namespace geos;

// Dummy class used to automate initialization/uninitialization of GEOS.
class CInitGeos
{
public:
    CInitGeos()
    {
    }
    ~CInitGeos()
    {
        // free GEOS resources
        Unload::Release();
    }
};

static CInitGeos s_InitGeos;


bool MgGeosUtil::Contains(MgGeometry* geom1, MgGeometry* geom2)
{
    bool contains = false;
    PrecisionModel* pm = NULL;
    GeometryFactory* gf = NULL;
    Geometry* g1 = NULL;
    Geometry* g2 = NULL;

    MG_GEOMETRY_TRY()

    pm = new PrecisionModel();
    gf = new GeometryFactory(pm, 10);
    WKTReader r(gf);

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    Ptr<MgGeometry> tGeom2 = MgSpatialUtility::TesselateCurve(geom2);

    STRING wktGeom1 = tGeom1->ToAwkt(true);
    STRING wktGeom2 = tGeom2->ToAwkt(true);

    g1 = r.read(MgUtil::WideCharToMultiByte(wktGeom1));
    g2 = r.read(MgUtil::WideCharToMultiByte(wktGeom2));

    contains = g1->contains(g2);

    MG_GEOMETRY_CATCH(L"MgGeosUtil.Contains")

    delete g1;
    delete g2;
    delete gf;
    delete pm;

    MG_GEOMETRY_THROW()

    return contains;
}

bool MgGeosUtil::Intersects(MgGeometry* geom1, MgGeometry* geom2)
{
    bool intersects = false;
    PrecisionModel* pm = NULL;
    GeometryFactory* gf = NULL;
    Geometry* g1 = NULL;
    Geometry* g2 = NULL;

    MG_GEOMETRY_TRY()

    pm = new PrecisionModel();
    gf = new GeometryFactory(pm, 10);
    WKTReader r(gf);

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    Ptr<MgGeometry> tGeom2 = MgSpatialUtility::TesselateCurve(geom2);

    STRING wktGeom1 = tGeom1->ToAwkt(true);
    STRING wktGeom2 = tGeom2->ToAwkt(true);

    g1 = r.read(MgUtil::WideCharToMultiByte(wktGeom1));
    g2 = r.read(MgUtil::WideCharToMultiByte(wktGeom2));

    intersects = g1->intersects(g2);

    MG_GEOMETRY_CATCH(L"MgGeosUtil.Intersects")

    delete g1;
    delete g2;
    delete gf;
    delete pm;

    MG_GEOMETRY_THROW()

    return intersects;
}

bool MgGeosUtil::Crosses(MgGeometry* geom1, MgGeometry* geom2)
{
    bool crosses = false;
    PrecisionModel* pm = NULL;
    GeometryFactory* gf = NULL;
    Geometry* g1 = NULL;
    Geometry* g2 = NULL;

    MG_GEOMETRY_TRY()

    pm = new PrecisionModel();
    gf = new GeometryFactory(pm, 10);
    WKTReader r(gf);

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    Ptr<MgGeometry> tGeom2 = MgSpatialUtility::TesselateCurve(geom2);

    STRING wktGeom1 = tGeom1->ToAwkt(true);
    STRING wktGeom2 = tGeom2->ToAwkt(true);

    g1 = r.read(MgUtil::WideCharToMultiByte(wktGeom1));
    g2 = r.read(MgUtil::WideCharToMultiByte(wktGeom2));

    crosses = g1->crosses(g2);

    MG_GEOMETRY_CATCH(L"MgGeosUtil.Crosses")

    delete g1;
    delete g2;
    delete gf;
    delete pm;

    MG_GEOMETRY_THROW()

    return crosses;
}

bool MgGeosUtil::Disjoint(MgGeometry* geom1, MgGeometry* geom2)
{
    bool disjoint = false;
    PrecisionModel* pm = NULL;
    GeometryFactory* gf = NULL;
    Geometry* g1 = NULL;
    Geometry* g2 = NULL;

    MG_GEOMETRY_TRY()

    pm = new PrecisionModel();
    gf = new GeometryFactory(pm, 10);
    WKTReader r(gf);

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    Ptr<MgGeometry> tGeom2 = MgSpatialUtility::TesselateCurve(geom2);

    STRING wktGeom1 = tGeom1->ToAwkt(true);
    STRING wktGeom2 = tGeom2->ToAwkt(true);

    g1 = r.read(MgUtil::WideCharToMultiByte(wktGeom1));
    g2 = r.read(MgUtil::WideCharToMultiByte(wktGeom2));

    disjoint = g1->disjoint(g2);

    MG_GEOMETRY_CATCH(L"MgGeosUtil.Disjoint")

    delete g1;
    delete g2;
    delete gf;
    delete pm;

    MG_GEOMETRY_THROW()

    return disjoint;
}

bool MgGeosUtil::Equals(MgGeometry* geom1, MgGeometry* geom2)
{
    bool equals = false;
    PrecisionModel* pm = NULL;
    GeometryFactory* gf = NULL;
    Geometry* g1 = NULL;
    Geometry* g2 = NULL;

    MG_GEOMETRY_TRY()

    pm = new PrecisionModel();
    gf = new GeometryFactory(pm, 10);
    WKTReader r(gf);

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    Ptr<MgGeometry> tGeom2 = MgSpatialUtility::TesselateCurve(geom2);

    STRING wktGeom1 = tGeom1->ToAwkt(true);
    STRING wktGeom2 = tGeom2->ToAwkt(true);

    g1 = r.read(MgUtil::WideCharToMultiByte(wktGeom1));
    g2 = r.read(MgUtil::WideCharToMultiByte(wktGeom2));

    equals = g1->equals(g2);

    MG_GEOMETRY_CATCH(L"MgGeosUtil.Equals")

    delete g1;
    delete g2;
    delete gf;
    delete pm;

    MG_GEOMETRY_THROW()

    return equals;
}

bool MgGeosUtil::Overlaps(MgGeometry* geom1, MgGeometry* geom2)
{
    bool overlaps = false;
    PrecisionModel* pm = NULL;
    GeometryFactory* gf = NULL;
    Geometry* g1 = NULL;
    Geometry* g2 = NULL;

    MG_GEOMETRY_TRY()

    pm = new PrecisionModel();
    gf = new GeometryFactory(pm, 10);
    WKTReader r(gf);

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    Ptr<MgGeometry> tGeom2 = MgSpatialUtility::TesselateCurve(geom2);

    STRING wktGeom1 = tGeom1->ToAwkt(true);
    STRING wktGeom2 = tGeom2->ToAwkt(true);

    g1 = r.read(MgUtil::WideCharToMultiByte(wktGeom1));
    g2 = r.read(MgUtil::WideCharToMultiByte(wktGeom2));

    overlaps = g1->overlaps(g2);

    MG_GEOMETRY_CATCH(L"MgGeosUtil.Overlaps")

    delete g1;
    delete g2;
    delete gf;
    delete pm;

    MG_GEOMETRY_THROW()

    return overlaps;
}

bool MgGeosUtil::Touches(MgGeometry* geom1, MgGeometry* geom2)
{
    bool touches = false;
    PrecisionModel* pm = NULL;
    GeometryFactory* gf = NULL;
    Geometry* g1 = NULL;
    Geometry* g2 = NULL;

    MG_GEOMETRY_TRY()

    pm = new PrecisionModel();
    gf = new GeometryFactory(pm, 10);
    WKTReader r(gf);

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    Ptr<MgGeometry> tGeom2 = MgSpatialUtility::TesselateCurve(geom2);

    STRING wktGeom1 = tGeom1->ToAwkt(true);
    STRING wktGeom2 = tGeom2->ToAwkt(true);

    g1 = r.read(MgUtil::WideCharToMultiByte(wktGeom1));
    g2 = r.read(MgUtil::WideCharToMultiByte(wktGeom2));

    touches = g1->touches(g2);

    MG_GEOMETRY_CATCH(L"MgGeosUtil.Touches")

    delete g1;
    delete g2;
    delete gf;
    delete pm;

    MG_GEOMETRY_THROW()

    return touches;
}

bool MgGeosUtil::Within(MgGeometry* geom1, MgGeometry* geom2)
{
    bool within = false;
    PrecisionModel* pm = NULL;
    GeometryFactory* gf = NULL;
    Geometry* g1 = NULL;
    Geometry* g2 = NULL;

    MG_GEOMETRY_TRY()

    pm = new PrecisionModel();
    gf = new GeometryFactory(pm, 10);
    WKTReader r(gf);

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    Ptr<MgGeometry> tGeom2 = MgSpatialUtility::TesselateCurve(geom2);

    STRING wktGeom1 = tGeom1->ToAwkt(true);
    STRING wktGeom2 = tGeom2->ToAwkt(true);

    g1 = r.read(MgUtil::WideCharToMultiByte(wktGeom1));
    g2 = r.read(MgUtil::WideCharToMultiByte(wktGeom2));

    within = g1->within(g2);

    MG_GEOMETRY_CATCH(L"MgGeosUtil.Within")

    delete g1;
    delete g2;
    delete gf;
    delete pm;

    MG_GEOMETRY_THROW()

    return within;
}

MgGeometry* MgGeosUtil::Boundary(MgGeometry* geom1)
{
    Ptr<MgGeometry> bndGeom;
    PrecisionModel* pm = NULL;
    GeometryFactory* gf = NULL;
    Geometry* g1 = NULL;
    Geometry* g3 = NULL;

    MG_GEOMETRY_TRY()

    pm = new PrecisionModel();
    gf = new GeometryFactory(pm, 10);
    WKTReader r(gf);

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    STRING wktGeom1 = tGeom1->ToAwkt(true);

    g1 = r.read(MgUtil::WideCharToMultiByte(wktGeom1));
    g3 = g1->getBoundary();

    WKTWriter writer;
    string bndWkt = writer.write(g3);

    if (bndWkt.find("EMPTY", 0) == string::npos)
    {
        MgWktReaderWriter rdrWrt;
        bndGeom = rdrWrt.Read(MgUtil::MultiByteToWideChar(bndWkt));
    }

    MG_GEOMETRY_CATCH(L"MgGeosUtil.Boundary")

    delete g1;
    delete g3;
    delete gf;
    delete pm;

    MG_GEOMETRY_THROW()

    return SAFE_ADDREF((MgGeometry*)bndGeom);
}

MgGeometry* MgGeosUtil::ConvexHull(MgGeometry* geom1)
{
    Ptr<MgGeometry> convexHull;
    PrecisionModel* pm = NULL;
    GeometryFactory* gf = NULL;
    Geometry* g1 = NULL;
    Geometry* g3 = NULL;

    MG_GEOMETRY_TRY()

    pm = new PrecisionModel();
    gf = new GeometryFactory(pm, 10);
    WKTReader r(gf);

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    STRING wktGeom1 = tGeom1->ToAwkt(true);

    g1 = r.read(MgUtil::WideCharToMultiByte(wktGeom1));
    g3 = g1->convexHull();

    WKTWriter writer;
    string convexHullWkt = writer.write(g3);

    if (convexHullWkt.find("EMPTY", 0) == string::npos)
    {
        MgWktReaderWriter rdrWrt;
        convexHull = rdrWrt.Read(MgUtil::MultiByteToWideChar(convexHullWkt));
    }

    MG_GEOMETRY_CATCH(L"MgGeosUtil.ConvexHull")

    delete g1;
    delete g3;
    delete gf;
    delete pm;

    MG_GEOMETRY_THROW()

    return SAFE_ADDREF((MgGeometry*)convexHull);
}

MgGeometry* MgGeosUtil::Difference(MgGeometry* geom1, MgGeometry* geom2)
{
    Ptr<MgGeometry> difference;
    PrecisionModel* pm = NULL;
    GeometryFactory* gf = NULL;
    Geometry* g1 = NULL;
    Geometry* g2 = NULL;
    Geometry* g3 = NULL;

    MG_GEOMETRY_TRY()

    pm = new PrecisionModel();
    gf = new GeometryFactory(pm, 10);
    WKTReader r(gf);

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    Ptr<MgGeometry> tGeom2 = MgSpatialUtility::TesselateCurve(geom2);

    STRING wktGeom1 = tGeom1->ToAwkt(true);
    STRING wktGeom2 = tGeom2->ToAwkt(true);

    g1 = r.read(MgUtil::WideCharToMultiByte(wktGeom1));
    g2 = r.read(MgUtil::WideCharToMultiByte(wktGeom2));
    g3 = g1->difference(g2);

    WKTWriter writer;
    string differenceWkt = writer.write(g3);

    if (differenceWkt.find("EMPTY", 0) == string::npos)
    {
        MgWktReaderWriter rdrWrt;
        difference = rdrWrt.Read(MgUtil::MultiByteToWideChar(differenceWkt));
    }

    MG_GEOMETRY_CATCH(L"MgGeosUtil.Difference")

    delete g1;
    delete g2;
    delete g3;
    delete gf;
    delete pm;

    MG_GEOMETRY_THROW()

    return SAFE_ADDREF((MgGeometry*)difference);
}


double MgGeosUtil::Distance(MgGeometry* geom1, MgGeometry* geom2)
{
    double distance = 0.0;
    PrecisionModel* pm = NULL;
    GeometryFactory* gf = NULL;
    Geometry* g1 = NULL;
    Geometry* g2 = NULL;

    MG_GEOMETRY_TRY()

    pm = new PrecisionModel();
    gf = new GeometryFactory(pm, 10);
    WKTReader r(gf);

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    Ptr<MgGeometry> tGeom2 = MgSpatialUtility::TesselateCurve(geom2);

    STRING wktGeom1 = tGeom1->ToAwkt(true);
    STRING wktGeom2 = tGeom2->ToAwkt(true);

    g1 = r.read(MgUtil::WideCharToMultiByte(wktGeom1));
    g2 = r.read(MgUtil::WideCharToMultiByte(wktGeom2));

    distance = g1->distance(g2);

    MG_GEOMETRY_CATCH(L"MgGeosUtil.Distance")

    delete g1;
    delete g2;
    delete gf;
    delete pm;

    MG_GEOMETRY_THROW()

    return distance;
}

MgGeometry* MgGeosUtil::Intersection(MgGeometry* geom1, MgGeometry* geom2)
{
    Ptr<MgGeometry> intersection;
    PrecisionModel* pm = NULL;
    GeometryFactory* gf = NULL;
    Geometry* g1 = NULL;
    Geometry* g2 = NULL;
    Geometry* g3 = NULL;

    MG_GEOMETRY_TRY()

    pm = new PrecisionModel();
    gf = new GeometryFactory(pm, 10);
    WKTReader r(gf);

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    Ptr<MgGeometry> tGeom2 = MgSpatialUtility::TesselateCurve(geom2);

    STRING wktGeom1 = tGeom1->ToAwkt(true);
    STRING wktGeom2 = tGeom2->ToAwkt(true);

    g1 = r.read(MgUtil::WideCharToMultiByte(wktGeom1));
    g2 = r.read(MgUtil::WideCharToMultiByte(wktGeom2));
    g3 = g1->intersection(g2);

    WKTWriter writer;
    string intersectionWkt = writer.write(g3);

    if (intersectionWkt.find("EMPTY", 0) == string::npos)
    {
        MgWktReaderWriter rdrWrt;
        intersection = rdrWrt.Read(MgUtil::MultiByteToWideChar(intersectionWkt));
    }

    MG_GEOMETRY_CATCH(L"MgGeosUtil.Intersection")

    delete g1;
    delete g2;
    delete g3;
    delete gf;
    delete pm;

    MG_GEOMETRY_THROW()

    return SAFE_ADDREF((MgGeometry*)intersection);
}

MgGeometry* MgGeosUtil::SymetricDifference(MgGeometry* geom1, MgGeometry* geom2)
{
    Ptr<MgGeometry> symetricDifference;
    PrecisionModel* pm = NULL;
    GeometryFactory* gf = NULL;
    Geometry* g1 = NULL;
    Geometry* g2 = NULL;
    Geometry* g3 = NULL;

    MG_GEOMETRY_TRY()

    pm = new PrecisionModel();
    gf = new GeometryFactory(pm, 10);
    WKTReader r(gf);

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    Ptr<MgGeometry> tGeom2 = MgSpatialUtility::TesselateCurve(geom2);

    STRING wktGeom1 = tGeom1->ToAwkt(true);
    STRING wktGeom2 = tGeom2->ToAwkt(true);

    g1 = r.read(MgUtil::WideCharToMultiByte(wktGeom1));
    g2 = r.read(MgUtil::WideCharToMultiByte(wktGeom2));
    g3 = g1->symDifference(g2);

    WKTWriter writer;
    string symetricDifferenceWkt = writer.write(g3);

    if (symetricDifferenceWkt.find("EMPTY", 0) == string::npos)
    {
        MgWktReaderWriter rdrWrt;
        symetricDifference = rdrWrt.Read(MgUtil::MultiByteToWideChar(symetricDifferenceWkt));
    }

    MG_GEOMETRY_CATCH(L"MgGeosUtil.SymetricDifference")

    delete g1;
    delete g2;
    delete g3;
    delete gf;
    delete pm;

    MG_GEOMETRY_THROW()

    return SAFE_ADDREF((MgGeometry*)symetricDifference);
}

MgGeometry* MgGeosUtil::Union(MgGeometry* geom1, MgGeometry* geom2)
{
    Ptr<MgGeometry> unionGeom;
    PrecisionModel* pm = NULL;
    GeometryFactory* gf = NULL;
    Geometry* g1 = NULL;
    Geometry* g2 = NULL;
    Geometry* g3 = NULL;

    MG_GEOMETRY_TRY()

    pm = new PrecisionModel();
    gf = new GeometryFactory(pm, 10);
    WKTReader r(gf);

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    Ptr<MgGeometry> tGeom2 = MgSpatialUtility::TesselateCurve(geom2);

    STRING wktGeom1 = tGeom1->ToAwkt(true);
    STRING wktGeom2 = tGeom2->ToAwkt(true);

    g1 = r.read(MgUtil::WideCharToMultiByte(wktGeom1));
    g2 = r.read(MgUtil::WideCharToMultiByte(wktGeom2));
    g3 = g1->Union(g2);

    WKTWriter writer;
    string unionGeomWkt = writer.write(g3);

    if (unionGeomWkt.find("EMPTY", 0) == string::npos)
    {
        MgWktReaderWriter rdrWrt;
        unionGeom = rdrWrt.Read(MgUtil::MultiByteToWideChar(unionGeomWkt));
    }

    MG_GEOMETRY_CATCH(L"MgGeosUtil.Union")

    delete g1;
    delete g2;
    delete g3;
    delete gf;
    delete pm;

    MG_GEOMETRY_THROW()

    return SAFE_ADDREF((MgGeometry*)unionGeom);
}

bool MgGeosUtil::IsValid(MgGeometricEntity* geom1)
{
    bool isValid = false;
    PrecisionModel* pm = NULL;
    GeometryFactory* gf = NULL;
    Geometry* g1 = NULL;

    MG_GEOMETRY_TRY()

    pm = new PrecisionModel();
    gf = new GeometryFactory(pm, 10);
    WKTReader r(gf);

    STRING wktGeom1 = ToAwkt(geom1);

    g1 = r.read(MgUtil::WideCharToMultiByte(wktGeom1));
    isValid = g1->isValid();

    MG_GEOMETRY_CATCH(L"MgGeosUtil.IsValid")

    delete g1;
    delete gf;
    delete pm;

    MG_GEOMETRY_THROW()

    return isValid;
}

bool MgGeosUtil::IsSimple(MgGeometricEntity* geom1)
{
    bool isSimple = false;
    PrecisionModel* pm = NULL;
    GeometryFactory* gf = NULL;
    Geometry* g1 = NULL;

    MG_GEOMETRY_TRY()

    pm = new PrecisionModel();
    gf = new GeometryFactory(pm, 10);
    WKTReader r(gf);

    STRING wktGeom1 = ToAwkt(geom1);

    g1 = r.read(MgUtil::WideCharToMultiByte(wktGeom1));
    isSimple = g1->isSimple();

    MG_GEOMETRY_CATCH(L"MgGeosUtil.IsSimple")

    delete g1;
    delete gf;
    delete pm;

    MG_GEOMETRY_THROW()

    return isSimple;
}

bool MgGeosUtil::IsEmpty(MgGeometricEntity* geom1)
{
    bool isEmpty = false;
    PrecisionModel* pm = NULL;
    GeometryFactory* gf = NULL;
    Geometry* g1 = NULL;

    MG_GEOMETRY_TRY()

    pm = new PrecisionModel();
    gf = new GeometryFactory(pm, 10);
    WKTReader r(gf);

    STRING wktGeom1 = ToAwkt(geom1);

    g1 = r.read(MgUtil::WideCharToMultiByte(wktGeom1));
    isEmpty = g1->isEmpty();

    MG_GEOMETRY_CATCH(L"MgGeosUtil.IsEmpty")

    delete g1;
    delete gf;
    delete pm;

    MG_GEOMETRY_THROW()

    return isEmpty;
}

bool MgGeosUtil::IsClosed(MgGeometricEntity* geom1)
{
    bool isClosed = false;
    PrecisionModel* pm = NULL;
    GeometryFactory* gf = NULL;
    Geometry* g1 = NULL;

    MG_GEOMETRY_TRY()

    pm = new PrecisionModel();
    gf = new GeometryFactory(pm, 10);
    WKTReader r(gf);

    STRING wktGeom1 = ToAwkt(geom1);

    g1 = r.read(MgUtil::WideCharToMultiByte(wktGeom1));
    // isClosed = g1->isClosed();

    MG_GEOMETRY_CATCH(L"MgGeosUtil.IsClosed")

    delete g1;
    delete gf;
    delete pm;

    MG_GEOMETRY_THROW()

    return isClosed;
}

double MgGeosUtil::Area(MgGeometricEntity* geom1)
{
    double area = 0.0;
    PrecisionModel* pm = NULL;
    GeometryFactory* gf = NULL;
    Geometry* g1 = NULL;

    MG_GEOMETRY_TRY()

    pm = new PrecisionModel();
    gf = new GeometryFactory(pm, 10);
    WKTReader r(gf);

    STRING wktGeom1 = ToAwkt(geom1);

    g1 = r.read(MgUtil::WideCharToMultiByte(wktGeom1));
    area = g1->getArea();

    MG_GEOMETRY_CATCH(L"MgGeosUtil.Area")

    delete g1;
    delete gf;
    delete pm;

    MG_GEOMETRY_THROW()

    return area;
}

double MgGeosUtil::Length(MgGeometricEntity* geom1)
{
    double length = 0.0;
    PrecisionModel* pm = NULL;
    GeometryFactory* gf = NULL;
    Geometry* g1 = NULL;

    MG_GEOMETRY_TRY()

    pm = new PrecisionModel();
    gf = new GeometryFactory(pm, 10);
    WKTReader r(gf);

    STRING wktGeom1 = ToAwkt(geom1);

    g1 = r.read(MgUtil::WideCharToMultiByte(wktGeom1));
    length = g1->getLength();

    MG_GEOMETRY_CATCH(L"MgGeosUtil.Length")

    delete g1;
    delete gf;
    delete pm;

    MG_GEOMETRY_THROW()

    return length;
}

MgPoint* MgGeosUtil::Centroid(MgGeometricEntity* geom1)
{
    Ptr<MgPoint> point;
    PrecisionModel* pm = NULL;
    GeometryFactory* gf = NULL;
    Geometry* g1 = NULL;
    Point* g2 = NULL;

    MG_GEOMETRY_TRY()

    pm = new PrecisionModel();
    gf = new GeometryFactory(pm, 10);
    WKTReader r(gf);

    STRING wktGeom1 = ToAwkt(geom1);

    g1 = r.read(MgUtil::WideCharToMultiByte(wktGeom1));
    g2 = g1->getCentroid();

    double x = g2->getX();
    double y = g2->getY();

    MgGeometryFactory geomFactory;
    Ptr<MgCoordinate> coord = geomFactory.CreateCoordinateXY(x,y);
    point = geomFactory.CreatePoint(coord);

    MG_GEOMETRY_CATCH(L"MgGeosUtil.Centroid")

    delete g1;
    delete g2;
    delete gf;
    delete pm;

    MG_GEOMETRY_THROW()

    return SAFE_ADDREF((MgPoint*)point);
}

MgPoint*  MgGeosUtil::GetPointInRegion(MgGeometry* geom1)
{
    Ptr<MgPoint> point;
    PrecisionModel* pm = NULL;
    GeometryFactory* gf = NULL;
    Geometry* g1 = NULL;
    Point* g2 = NULL;

    MG_GEOMETRY_TRY()

    pm = new PrecisionModel();
    gf = new GeometryFactory(pm, 10);
    WKTReader r(gf);

    STRING wktGeom1 = ToAwkt(geom1);

    g1 = r.read(MgUtil::WideCharToMultiByte(wktGeom1));
    g2 = g1->getInteriorPoint();

    double x = g2->getX();
    double y = g2->getY();

    MgGeometryFactory geomFactory;
    Ptr<MgCoordinate> coord = geomFactory.CreateCoordinateXY(x,y);
    point = geomFactory.CreatePoint(coord);

    MG_GEOMETRY_CATCH(L"MgGeosUtil.GetInteriorPoint")

    delete g1;
    delete g2;
    delete gf;
    delete pm;

    MG_GEOMETRY_THROW()

    return SAFE_ADDREF((MgPoint*)point);
}

MgPoint*  MgGeosUtil::GetPointInRing(MgGeometryComponent* geom1)
{
    Ptr<MgPoint> point;
    PrecisionModel* pm = NULL;
    GeometryFactory* gf = NULL;
    Geometry* g1 = NULL;
    Point* g2 = NULL;

    MG_GEOMETRY_TRY()

    pm = new PrecisionModel();
    gf = new GeometryFactory(pm, 10);
    WKTReader r(gf);

    STRING wktGeom1 = geom1->ToAwkt(true);

    g1 = r.read(MgUtil::WideCharToMultiByte(wktGeom1));
    g2 = g1->getInteriorPoint();

    double x = g2->getX();
    double y = g2->getY();

    MgGeometryFactory geomFactory;
    Ptr<MgCoordinate> coord = geomFactory.CreateCoordinateXY(x,y);
    point = geomFactory.CreatePoint(coord);

    MG_GEOMETRY_CATCH(L"MgGeosUtil.GetInteriorPoint")

    delete g1;
    delete g2;
    delete gf;
    delete pm;

    MG_GEOMETRY_THROW()

    return SAFE_ADDREF((MgPoint*)point);
}

STRING MgGeosUtil::ToAwkt(MgGeometricEntity* geom1)
{
    CHECKARGUMENTNULL(geom1, L"MgGeosUtil::ToAwkt")

    // Increment the refcount
    SAFE_ADDREF((MgGeometricEntity*) geom1);

    STRING wktStr = L"";
    INT32 type = geom1->GetEntityType();
    switch (type)
    {
        case MgGeometryEntityType::Geometry:
        {
            Ptr<MgGeometry> geom = static_cast<MgGeometry*> ((MgGeometricEntity*)geom1);
            Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom);
            wktStr = tGeom1->ToAwkt(true);
            break;
        }
        case MgGeometryEntityType::GeometryComponent:
        {
            Ptr<MgGeometryComponent> geomComp = static_cast<MgGeometryComponent*> ((MgGeometricEntity*)geom1);
            Ptr<MgGeometryComponent> tGeomComp = MgSpatialUtility::TesselateGeometryComponent(geomComp);
            wktStr = tGeomComp->ToAwkt(true);
            break;
        }
    }
    return wktStr;
}
