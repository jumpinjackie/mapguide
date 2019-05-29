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
#include "GeosInclude.h"
#pragma warning(pop)

#include "GeometryCommon.h"
#include "Buffer/buffer.h"
#include "GeosUtil.h"
#include "GeometryExceptionDef.h"
#include "Spatial/SpatialUtility.h"
#include "GeometricEntityType.h"

using namespace geos;
using namespace geos::io;

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

class GeosWktReader
{
public:
// GEOS 3.6.0 onwards changes the C++ API around GeometryFactory
#if (GEOS_VERSION_MAJOR == 3) && (GEOS_VERSION_MINOR >= 6)
    GeosWktReader() : m_pm(NULL), m_reader(NULL)
    {
        m_pm = new PrecisionModel();
        m_gf = GeometryFactory::create(m_pm, 10);
        m_reader = new WKTReader(m_gf.get());
    }
    ~GeosWktReader()
    {
        delete m_reader;
        delete m_pm;
    }
#else
    GeosWktReader() : m_pm(NULL), m_reader(NULL), m_gf(NULL)
    {
        m_pm = new PrecisionModel();
        m_gf = new GeometryFactory(m_pm, 10);
        m_reader = new WKTReader(m_gf);
    }
    ~GeosWktReader()
    {
        delete m_reader;
        delete m_gf;
        delete m_pm;
    }
#endif
    Geometry* Read(CREFSTRING wkt)
    {
        return m_reader->read(MgUtil::WideCharToMultiByte(wkt));
    }

private:
    PrecisionModel* m_pm;
    WKTReader* m_reader;
// GEOS 3.6.0 onwards changes the C++ API around GeometryFactory
#if (GEOS_VERSION_MAJOR == 3) && (GEOS_VERSION_MINOR >= 6)
    GeometryFactory::unique_ptr m_gf;
#else
    GeometryFactory* m_gf;
#endif
};

bool MgGeosUtil::Contains(MgGeometry* geom1, MgGeometry* geom2)
{
    bool contains = false;

    MG_GEOMETRY_TRY()

    GeosWktReader r;

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    Ptr<MgGeometry> tGeom2 = MgSpatialUtility::TesselateCurve(geom2);

    STRING wktGeom1 = tGeom1->ToAwkt(true);
    STRING wktGeom2 = tGeom2->ToAwkt(true);

    std::unique_ptr<Geometry> g1(r.Read(wktGeom1));
    std::unique_ptr<Geometry> g2(r.Read(wktGeom2));

    contains = g1->contains(g2.get());

    MG_GEOMETRY_CATCH_AND_THROW(L"MgGeosUtil.Contains")

    return contains;
}

bool MgGeosUtil::Intersects(MgGeometry* geom1, MgGeometry* geom2)
{
    bool intersects = false;

    MG_GEOMETRY_TRY()

    GeosWktReader r;

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    Ptr<MgGeometry> tGeom2 = MgSpatialUtility::TesselateCurve(geom2);

    STRING wktGeom1 = tGeom1->ToAwkt(true);
    STRING wktGeom2 = tGeom2->ToAwkt(true);

    std::unique_ptr<Geometry> g1(r.Read(wktGeom1));
    std::unique_ptr<Geometry> g2(r.Read(wktGeom2));

    intersects = g1->intersects(g2.get());

    MG_GEOMETRY_CATCH_AND_THROW(L"MgGeosUtil.Intersects")

    return intersects;
}

bool MgGeosUtil::Crosses(MgGeometry* geom1, MgGeometry* geom2)
{
    bool crosses = false;

    MG_GEOMETRY_TRY()

    GeosWktReader r;

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    Ptr<MgGeometry> tGeom2 = MgSpatialUtility::TesselateCurve(geom2);

    STRING wktGeom1 = tGeom1->ToAwkt(true);
    STRING wktGeom2 = tGeom2->ToAwkt(true);

    std::unique_ptr<Geometry> g1(r.Read(wktGeom1));
    std::unique_ptr<Geometry> g2(r.Read(wktGeom2));

    crosses = g1->crosses(g2.get());

    MG_GEOMETRY_CATCH_AND_THROW(L"MgGeosUtil.Crosses")

    return crosses;
}

bool MgGeosUtil::Disjoint(MgGeometry* geom1, MgGeometry* geom2)
{
    bool disjoint = false;

    MG_GEOMETRY_TRY()

    GeosWktReader r;

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    Ptr<MgGeometry> tGeom2 = MgSpatialUtility::TesselateCurve(geom2);

    STRING wktGeom1 = tGeom1->ToAwkt(true);
    STRING wktGeom2 = tGeom2->ToAwkt(true);

    std::unique_ptr<Geometry> g1(r.Read(wktGeom1));
    std::unique_ptr<Geometry> g2(r.Read(wktGeom2));

    disjoint = g1->disjoint(g2.get());

    MG_GEOMETRY_CATCH_AND_THROW(L"MgGeosUtil.Disjoint")

    return disjoint;
}

bool MgGeosUtil::Equals(MgGeometry* geom1, MgGeometry* geom2)
{
    bool equals = false;

    MG_GEOMETRY_TRY()

    GeosWktReader r;

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    Ptr<MgGeometry> tGeom2 = MgSpatialUtility::TesselateCurve(geom2);

    STRING wktGeom1 = tGeom1->ToAwkt(true);
    STRING wktGeom2 = tGeom2->ToAwkt(true);

    std::unique_ptr<Geometry> g1(r.Read(wktGeom1));
    std::unique_ptr<Geometry> g2(r.Read(wktGeom2));

    equals = g1->equals(g2.get());

    MG_GEOMETRY_CATCH_AND_THROW(L"MgGeosUtil.Equals")

    return equals;
}

bool MgGeosUtil::Overlaps(MgGeometry* geom1, MgGeometry* geom2)
{
    bool overlaps = false;

    MG_GEOMETRY_TRY()

    GeosWktReader r;

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    Ptr<MgGeometry> tGeom2 = MgSpatialUtility::TesselateCurve(geom2);

    STRING wktGeom1 = tGeom1->ToAwkt(true);
    STRING wktGeom2 = tGeom2->ToAwkt(true);

    std::unique_ptr<Geometry> g1(r.Read(wktGeom1));
    std::unique_ptr<Geometry> g2(r.Read(wktGeom2));

    overlaps = g1->overlaps(g2.get());

    MG_GEOMETRY_CATCH_AND_THROW(L"MgGeosUtil.Overlaps")

    return overlaps;
}

bool MgGeosUtil::Touches(MgGeometry* geom1, MgGeometry* geom2)
{
    bool touches = false;

    MG_GEOMETRY_TRY()

    GeosWktReader r;

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    Ptr<MgGeometry> tGeom2 = MgSpatialUtility::TesselateCurve(geom2);

    STRING wktGeom1 = tGeom1->ToAwkt(true);
    STRING wktGeom2 = tGeom2->ToAwkt(true);

    std::unique_ptr<Geometry> g1(r.Read(wktGeom1));
    std::unique_ptr<Geometry> g2(r.Read(wktGeom2));

    touches = g1->touches(g2.get());

    MG_GEOMETRY_CATCH_AND_THROW(L"MgGeosUtil.Touches")

    return touches;
}

bool MgGeosUtil::Within(MgGeometry* geom1, MgGeometry* geom2)
{
    bool within = false;

    MG_GEOMETRY_TRY()

    GeosWktReader r;

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    Ptr<MgGeometry> tGeom2 = MgSpatialUtility::TesselateCurve(geom2);

    STRING wktGeom1 = tGeom1->ToAwkt(true);
    STRING wktGeom2 = tGeom2->ToAwkt(true);

    std::unique_ptr<Geometry> g1(r.Read(wktGeom1));
    std::unique_ptr<Geometry> g2(r.Read(wktGeom2));

    within = g1->within(g2.get());

    MG_GEOMETRY_CATCH_AND_THROW(L"MgGeosUtil.Within")

    return within;
}

MgGeometry* MgGeosUtil::Boundary(MgGeometry* geom1)
{
    Ptr<MgGeometry> bndGeom;

    MG_GEOMETRY_TRY()

    GeosWktReader r;

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    STRING wktGeom1 = tGeom1->ToAwkt(true);

    std::unique_ptr<Geometry> g1(r.Read(wktGeom1));
    std::unique_ptr<Geometry> g3(g1->getBoundary());

    WKTWriter writer;
    string bndWkt = writer.write(g3.get());

    if (bndWkt.find("EMPTY", 0) == string::npos)
    {
        MgWktReaderWriter rdrWrt;
        bndGeom = rdrWrt.Read(MgUtil::MultiByteToWideChar(bndWkt));
    }

    MG_GEOMETRY_CATCH_AND_THROW(L"MgGeosUtil.Boundary")

    return SAFE_ADDREF((MgGeometry*)bndGeom);
}

MgGeometry* MgGeosUtil::ConvexHull(MgGeometry* geom1)
{
    Ptr<MgGeometry> convexHull;

    MG_GEOMETRY_TRY()

    GeosWktReader r;

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    STRING wktGeom1 = tGeom1->ToAwkt(true);

    std::unique_ptr<Geometry> g1(r.Read(wktGeom1));
    std::unique_ptr<Geometry> g3(g1->convexHull());

    WKTWriter writer;
    string convexHullWkt = writer.write(g3.get());

    if (convexHullWkt.find("EMPTY", 0) == string::npos)
    {
        MgWktReaderWriter rdrWrt;
        convexHull = rdrWrt.Read(MgUtil::MultiByteToWideChar(convexHullWkt));
    }

    MG_GEOMETRY_CATCH_AND_THROW(L"MgGeosUtil.ConvexHull")

    return SAFE_ADDREF((MgGeometry*)convexHull);
}

MgGeometry* MgGeosUtil::Difference(MgGeometry* geom1, MgGeometry* geom2)
{
    Ptr<MgGeometry> difference;

    MG_GEOMETRY_TRY()

    GeosWktReader r;

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    Ptr<MgGeometry> tGeom2 = MgSpatialUtility::TesselateCurve(geom2);

    STRING wktGeom1 = tGeom1->ToAwkt(true);
    STRING wktGeom2 = tGeom2->ToAwkt(true);

    std::unique_ptr<Geometry> g1(r.Read(wktGeom1));
    std::unique_ptr<Geometry> g2(r.Read(wktGeom2));
    std::unique_ptr<Geometry> g3(g1->difference(g2.get()));

    WKTWriter writer;
    string differenceWkt = writer.write(g3.get());

    if (differenceWkt.find("EMPTY", 0) == string::npos)
    {
        MgWktReaderWriter rdrWrt;
        difference = rdrWrt.Read(MgUtil::MultiByteToWideChar(differenceWkt));
    }

    MG_GEOMETRY_CATCH_AND_THROW(L"MgGeosUtil.Difference")

    return SAFE_ADDREF((MgGeometry*)difference);
}


double MgGeosUtil::Distance(MgGeometry* geom1, MgGeometry* geom2)
{
    double distance = 0.0;

    MG_GEOMETRY_TRY()

    GeosWktReader r;

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    Ptr<MgGeometry> tGeom2 = MgSpatialUtility::TesselateCurve(geom2);

    STRING wktGeom1 = tGeom1->ToAwkt(true);
    STRING wktGeom2 = tGeom2->ToAwkt(true);

    std::unique_ptr<Geometry> g1(r.Read(wktGeom1));
    std::unique_ptr<Geometry> g2(r.Read(wktGeom2));

    distance = g1->distance(g2.get());

    MG_GEOMETRY_CATCH_AND_THROW(L"MgGeosUtil.Distance")

    return distance;
}

MgGeometry* MgGeosUtil::Intersection(MgGeometry* geom1, MgGeometry* geom2)
{
    Ptr<MgGeometry> intersection;

    MG_GEOMETRY_TRY()

    GeosWktReader r;

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    Ptr<MgGeometry> tGeom2 = MgSpatialUtility::TesselateCurve(geom2);

    STRING wktGeom1 = tGeom1->ToAwkt(true);
    STRING wktGeom2 = tGeom2->ToAwkt(true);

    std::unique_ptr<Geometry> g1(r.Read(wktGeom1));
    std::unique_ptr<Geometry> g2(r.Read(wktGeom2));
    std::unique_ptr<Geometry> g3(g1->intersection(g2.get()));

    WKTWriter writer;
    string intersectionWkt = writer.write(g3.get());

    if (intersectionWkt.find("EMPTY", 0) == string::npos)
    {
        MgWktReaderWriter rdrWrt;
        intersection = rdrWrt.Read(MgUtil::MultiByteToWideChar(intersectionWkt));
    }

    MG_GEOMETRY_CATCH_AND_THROW(L"MgGeosUtil.Intersection")

    return SAFE_ADDREF((MgGeometry*)intersection);
}

MgGeometry* MgGeosUtil::SymetricDifference(MgGeometry* geom1, MgGeometry* geom2)
{
    Ptr<MgGeometry> symetricDifference;

    MG_GEOMETRY_TRY()

    GeosWktReader r;

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    Ptr<MgGeometry> tGeom2 = MgSpatialUtility::TesselateCurve(geom2);

    STRING wktGeom1 = tGeom1->ToAwkt(true);
    STRING wktGeom2 = tGeom2->ToAwkt(true);

    std::unique_ptr<Geometry> g1(r.Read(wktGeom1));
    std::unique_ptr<Geometry> g2(r.Read(wktGeom2));
    std::unique_ptr<Geometry> g3(g1->symDifference(g2.get()));

    WKTWriter writer;
    string symetricDifferenceWkt = writer.write(g3.get());

    if (symetricDifferenceWkt.find("EMPTY", 0) == string::npos)
    {
        MgWktReaderWriter rdrWrt;
        symetricDifference = rdrWrt.Read(MgUtil::MultiByteToWideChar(symetricDifferenceWkt));
    }

    MG_GEOMETRY_CATCH_AND_THROW(L"MgGeosUtil.SymetricDifference")

    return SAFE_ADDREF((MgGeometry*)symetricDifference);
}

MgGeometry* MgGeosUtil::Union(MgGeometry* geom1, MgGeometry* geom2)
{
    Ptr<MgGeometry> unionGeom;

    MG_GEOMETRY_TRY()

    GeosWktReader r;

    Ptr<MgGeometry> tGeom1 = MgSpatialUtility::TesselateCurve(geom1);
    Ptr<MgGeometry> tGeom2 = MgSpatialUtility::TesselateCurve(geom2);

    STRING wktGeom1 = tGeom1->ToAwkt(true);
    STRING wktGeom2 = tGeom2->ToAwkt(true);

    std::unique_ptr<Geometry> g1(r.Read(wktGeom1));
    std::unique_ptr<Geometry> g2(r.Read(wktGeom2));
    std::unique_ptr<Geometry> g3(g1->Union(g2.get()));

    WKTWriter writer;
    string unionGeomWkt = writer.write(g3.get());

    if (unionGeomWkt.find("EMPTY", 0) == string::npos)
    {
        MgWktReaderWriter rdrWrt;
        unionGeom = rdrWrt.Read(MgUtil::MultiByteToWideChar(unionGeomWkt));
    }

    MG_GEOMETRY_CATCH_AND_THROW(L"MgGeosUtil.Union")

    return SAFE_ADDREF((MgGeometry*)unionGeom);
}

bool MgGeosUtil::IsValid(MgGeometricEntity* geom1)
{
    bool isValid = false;

    MG_GEOMETRY_TRY()

    GeosWktReader r;

    STRING wktGeom1 = ToAwkt(geom1);

    std::unique_ptr<Geometry> g1(r.Read(wktGeom1));
    isValid = g1->isValid();

    MG_GEOMETRY_CATCH_AND_THROW(L"MgGeosUtil.IsValid")

    return isValid;
}

bool MgGeosUtil::IsSimple(MgGeometricEntity* geom1)
{
    bool isSimple = false;

    MG_GEOMETRY_TRY()

    GeosWktReader r;

    STRING wktGeom1 = ToAwkt(geom1);

    std::unique_ptr<Geometry> g1(r.Read(wktGeom1));
    isSimple = g1->isSimple();

    MG_GEOMETRY_CATCH_AND_THROW(L"MgGeosUtil.IsSimple")

    return isSimple;
}

bool MgGeosUtil::IsEmpty(MgGeometricEntity* geom1)
{
    bool isEmpty = false;

    MG_GEOMETRY_TRY()

    GeosWktReader r;

    STRING wktGeom1 = ToAwkt(geom1);

    std::unique_ptr<Geometry> g1(r.Read(wktGeom1));
    isEmpty = g1->isEmpty();

    MG_GEOMETRY_CATCH_AND_THROW(L"MgGeosUtil.IsEmpty")

    return isEmpty;
}

bool MgGeosUtil::IsClosed(MgGeometricEntity* geom1)
{
    bool isClosed = false;

    MG_GEOMETRY_TRY()

    GeosWktReader r;

    STRING wktGeom1 = ToAwkt(geom1);

    std::unique_ptr<Geometry> g1(r.Read(wktGeom1));
    //isClosed = g1->isClosed();

    MG_GEOMETRY_CATCH_AND_THROW(L"MgGeosUtil.IsClosed")

    return isClosed;
}

double MgGeosUtil::Area(MgGeometricEntity* geom1)
{
    double area = 0.0;

    MG_GEOMETRY_TRY()

    GeosWktReader r;

    STRING wktGeom1 = ToAwkt(geom1);

    std::unique_ptr<Geometry> g1(r.Read(wktGeom1));
    area = g1->getArea();

    MG_GEOMETRY_CATCH_AND_THROW(L"MgGeosUtil.Area")

    return area;
}

double MgGeosUtil::Length(MgGeometricEntity* geom1)
{
    double length = 0.0;

    MG_GEOMETRY_TRY()

    GeosWktReader r;

    STRING wktGeom1 = ToAwkt(geom1);

    std::unique_ptr<Geometry> g1(r.Read(wktGeom1));
    length = g1->getLength();

    MG_GEOMETRY_CATCH_AND_THROW(L"MgGeosUtil.Length")

    return length;
}

MgPoint* MgGeosUtil::Centroid(MgGeometricEntity* geom1)
{
    Ptr<MgPoint> point;

    MG_GEOMETRY_TRY()

    GeosWktReader r;

    STRING wktGeom1 = ToAwkt(geom1);

    std::unique_ptr<Geometry> g1(r.Read(wktGeom1));
    std::unique_ptr<Point> g2(g1->getCentroid());

    double x = g2->getX();
    double y = g2->getY();

    MgGeometryFactory geomFactory;
    Ptr<MgCoordinate> coord = geomFactory.CreateCoordinateXY(x,y);
    point = geomFactory.CreatePoint(coord);

    MG_GEOMETRY_CATCH_AND_THROW(L"MgGeosUtil.Centroid")

    return SAFE_ADDREF((MgPoint*)point);
}

MgPoint*  MgGeosUtil::GetPointInRegion(MgGeometry* geom1)
{
    Ptr<MgPoint> point;

    MG_GEOMETRY_TRY()

    GeosWktReader r;

    STRING wktGeom1 = ToAwkt(geom1);

    std::unique_ptr<Geometry> g1(r.Read(wktGeom1));
    std::unique_ptr<Point> g2(g1->getInteriorPoint());

    double x = g2->getX();
    double y = g2->getY();

    MgGeometryFactory geomFactory;
    Ptr<MgCoordinate> coord = geomFactory.CreateCoordinateXY(x, y);
    point = geomFactory.CreatePoint(coord);

    MG_GEOMETRY_CATCH_AND_THROW(L"MgGeosUtil.GetInteriorPoint")

    return SAFE_ADDREF((MgPoint*)point);
}

MgPoint*  MgGeosUtil::GetPointInRing(MgGeometryComponent* geom1)
{
    Ptr<MgPoint> point;

    MG_GEOMETRY_TRY()

    GeosWktReader r;

    STRING wktGeom1 = ToAwkt(geom1);

    std::unique_ptr<Geometry> g1(r.Read(wktGeom1));
    std::unique_ptr<Point> g2(g1->getInteriorPoint());

    double x = g2->getX();
    double y = g2->getY();

    MgGeometryFactory geomFactory;
    Ptr<MgCoordinate> coord = geomFactory.CreateCoordinateXY(x,y);
    point = geomFactory.CreatePoint(coord);

    MG_GEOMETRY_CATCH_AND_THROW(L"MgGeosUtil.GetInteriorPoint")

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
