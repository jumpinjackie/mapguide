//
//  Copyright (C) 2004-2015 by Autodesk, Inc.
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

#include "GeosInclude.h"
#include "geos/simplify/DouglasPeuckerSimplifier.h"
#include "geos/simplify/TopologyPreservingSimplifier.h"

#include "GeometryCommon.h"
#include "GeosUtil.h"
#include "GeometryExceptionDef.h"

///////////////////////////////////////////////////////////////////////////
// Creates an MgGeometrySimplifier object
//
MgGeometrySimplifier::MgGeometrySimplifier()
{
}

MgGeometry* MgGeometrySimplifier::Simplify(MgGeometry* geom, double tolerance, INT32 algorithm)
{
    Ptr<MgGeometry> simplified;
    std::unique_ptr<Geometry> gInput;
    std::unique_ptr<Geometry> gOutput;
    MG_GEOMETRY_TRY()

    CHECKARGUMENTNULL(geom, L"MgGeometrySimplifier.Simplify");
    if (algorithm != MgGeometrySimplificationAlgorithmType::DouglasPeucker &&
        algorithm != MgGeometrySimplificationAlgorithmType::TopologyPreserving)
    {
        throw new MgInvalidArgumentException(L"MgGeometrySimplifier.Simplify", __LINE__, __WFILE__, NULL, L"MgInvalidGeometrySimplficationAlgorithm", NULL);
    }

    STRING inputWKt = geom->ToAwkt(true);
    PrecisionModel pm;
// GEOS 3.6.0 onwards changes the C++ API around GeometryFactory
#if (GEOS_VERSION_MAJOR == 3) && (GEOS_VERSION_MINOR >= 6)
    GeometryFactory::unique_ptr gf = GeometryFactory::create(&pm, 10);
    WKTReader r(gf.get());
#else
    GeometryFactory gf(&pm, 10);
    WKTReader r(&gf);
#endif
    WKTWriter w;

    gInput.reset(r.read(MgUtil::WideCharToMultiByte(inputWKt)));

    switch (algorithm)
    {
    case MgGeometrySimplificationAlgorithmType::DouglasPeucker:
        gOutput = geos::simplify::DouglasPeuckerSimplifier::simplify(gInput.get(), tolerance);
        break;
    case MgGeometrySimplificationAlgorithmType::TopologyPreserving:
        gOutput = geos::simplify::TopologyPreservingSimplifier::simplify(gInput.get(), tolerance);
        break;
    }

    Geometry* gSimp = gOutput.get();
    std::string mbSimpWkt = w.write(gSimp);

    if (mbSimpWkt.find("EMPTY", 0) == string::npos)
    {
        MgWktReaderWriter rdrWrt;
        simplified = rdrWrt.Read(MgUtil::MultiByteToWideChar(mbSimpWkt));
    }
    
    MG_GEOMETRY_CATCH_AND_THROW(L"MgGeometrySimplifier.Simplify")

    return simplified.Detach();
}

//////////////////////////////////////////////
// Dispose this object.
//
void MgGeometrySimplifier::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////////
// Get the unique identifier for the class
//
INT32 MgGeometrySimplifier::GetClassId()
{
    return m_cls_id;
}