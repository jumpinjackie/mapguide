//
//  Copyright (C) 2004-2017 by Autodesk, Inc.
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

#include "GeometryCommon.h"
#include "GeosInclude.h"
#include <geos/geom/prep/PreparedGeometry.h>
#include <geos/geom/prep/PreparedGeometryFactory.h>

using namespace geos::geom::prep;

class MgPreparedGeometry::PreparedGeometryImpl 
{
public:
// GEOS 3.6.0 onwards changes the C++ API around GeometryFactory
#if (GEOS_VERSION_MAJOR == 3) && (GEOS_VERSION_MINOR >= 6)
    PreparedGeometryImpl() 
      : m_pg(NULL), 
        m_pm(new PrecisionModel()), 
        m_geom(NULL)
    {
        m_gf = GeometryFactory::create(m_pm.get(), 10);
    }
#else
    PreparedGeometryImpl() 
      : m_pg(NULL), 
        m_pm(new PrecisionModel()), 
        m_gf(NULL),
        m_geom(NULL)
    {
        m_gf.reset(new GeometryFactory(m_pm.get(), 10));
    }
#endif
    ~PreparedGeometryImpl() 
    {
        PreparedGeometryFactory::destroy(m_pg);
    }
    Geometry* Convert(MgGeometry* geom)
    {
        WKTReader r(m_gf.get());
        Ptr<MgGeometry> tGeom = MgSpatialUtility::TesselateCurve(geom);
        STRING wktGeom = tGeom->ToAwkt(true);

        return r.read(MgUtil::WideCharToMultiByte(wktGeom));
    }
    void Prepare(MgGeometry* geom)
    {
        WKTReader r(m_gf.get());
        Ptr<MgGeometry> tGeom = MgSpatialUtility::TesselateCurve(geom);
        STRING wktGeom = tGeom->ToAwkt(true);

        m_geom.reset(r.read(MgUtil::WideCharToMultiByte(wktGeom)));
        m_pg = PreparedGeometryFactory::prepare(m_geom.get());
    }

    const PreparedGeometry* m_pg;

private:
    std::auto_ptr<Geometry> m_geom;
    std::auto_ptr<PrecisionModel> m_pm;
// GEOS 3.6.0 onwards changes the C++ API around GeometryFactory
#if (GEOS_VERSION_MAJOR == 3) && (GEOS_VERSION_MINOR >= 6)
    GeometryFactory::unique_ptr m_gf;
#else
    std::auto_ptr<GeometryFactory> m_gf;
#endif
};

MgPreparedGeometry* MgPreparedGeometry::Create(MgGeometry* geom)
{
    Ptr<MgPreparedGeometry> pGeom;

    MG_GEOMETRY_TRY()

    PreparedGeometryImpl* impl = new PreparedGeometryImpl();
    impl->Prepare(geom);

    pGeom = new MgPreparedGeometry(impl);

    MG_GEOMETRY_CATCH_AND_THROW(L"MgPreparedGeometry.Create")

    return SAFE_ADDREF((MgPreparedGeometry*)pGeom);
}

MgPreparedGeometry::MgPreparedGeometry(PreparedGeometryImpl* impl)
    : d_ptr(impl)
{

}

MgPreparedGeometry::~MgPreparedGeometry()
{

}

bool MgPreparedGeometry::Contains(MgGeometry * other)
{
    std::auto_ptr<Geometry> gOther(d_ptr->Convert(other));
    return d_ptr->m_pg->contains(gOther.get());
}

bool MgPreparedGeometry::Crosses(MgGeometry * other)
{
    std::auto_ptr<Geometry> gOther(d_ptr->Convert(other));
    return d_ptr->m_pg->crosses(gOther.get());
}

bool MgPreparedGeometry::Disjoint(MgGeometry * other)
{
    std::auto_ptr<Geometry> gOther(d_ptr->Convert(other));
    return d_ptr->m_pg->disjoint(gOther.get());
}

bool MgPreparedGeometry::Intersects(MgGeometry * other)
{
    std::auto_ptr<Geometry> gOther(d_ptr->Convert(other));
    return d_ptr->m_pg->intersects(gOther.get());
}

bool MgPreparedGeometry::Overlaps(MgGeometry * other)
{
    std::auto_ptr<Geometry> gOther(d_ptr->Convert(other));
    return d_ptr->m_pg->overlaps(gOther.get());
}

bool MgPreparedGeometry::Touches(MgGeometry * other)
{
    std::auto_ptr<Geometry> gOther(d_ptr->Convert(other));
    return d_ptr->m_pg->touches(gOther.get());
}

bool MgPreparedGeometry::Within(MgGeometry * other)
{
    std::auto_ptr<Geometry> gOther(d_ptr->Convert(other));
    return d_ptr->m_pg->within(gOther.get());
}

INT32 MgPreparedGeometry::GetClassId() 
{ 
    return m_cls_id; 
}

void MgPreparedGeometry::Dispose()
{
    delete this;
}
