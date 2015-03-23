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

#include "GeometryCommon.h"
#include "CoordSysCommon.h"

#include "CoordSys.h"          //for CCoordinateSystem
#include "CoordSysMeasure.h"   //for CCoordinateSystemMeasure
#include "CoordSysUtil.h"      //for CsDictionaryOpenMode
#include "MentorUtil.h"        //for ProjectionFromString

using namespace CSLibrary;

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Initializes a new instance of CCoordinateSystemMeasure given a
/// coordinate system object. If the coordinate system supports it, the
/// ellipsoid definition is used in all computations.
///</summary>
///<param name="coordSys">
/// The coordinate system to use when performing measurement.
///</param>
CCoordinateSystemMeasure::CCoordinateSystemMeasure(MgCoordinateSystem* pCoordSys)
{
    CHECKARGUMENTNULL(pCoordSys, L"MgCoordinateSystemMeasure.MgCoordinateSystemMeasure");

    m_pCoordSys = SAFE_ADDREF(pCoordSys);
}

///////////////////////////////////////////////////////////////////////////
CCoordinateSystemMeasure::~CCoordinateSystemMeasure()
{
}

// <summary>Dispose this object.</summary>
// <returns>Returns nothing</returns>
void CCoordinateSystemMeasure::Dispose()
{
    delete this;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Computes the distance between two coordinates.
///</summary>
///<param name="x1">
/// The x value that defines the first coordinate.
///</param>
///<param name="y1">
/// The y value that defines the first coordinate.
///</param>
///<param name="x2">
/// The x value that defines the second coordinate.
///</param>
///<param name="y2">
/// The y value that defines the second coordinate.
///</param>
///<returns>
/// The distance between coord1 and coord2 as a double in coordinate system units.
///</returns>
double CCoordinateSystemMeasure::GetDistance(double x1, double y1, double x2, double y2)
{
    double distance = 0.0;

    MG_TRY()
    if(m_pCoordSys->GetType() == MgCoordinateSystemType::Arbitrary)
    {
        distance = m_pCoordSys->MeasureEuclideanDistance(x1, y1, x2, y2);
    }
    else
    {
        distance = m_pCoordSys->MeasureGreatCircleDistance(x1, y1, x2, y2);
    }
    MG_CATCH_AND_THROW(L"MgCoordinateSystemMeasure.GetDistance")

    return distance;
}

///////////////////////////////////////////////////////////////////////////
double CCoordinateSystemMeasure::GetDistance(MgCoordinate* coord1, MgCoordinate* coord2)
{
    double distance = 0.0;
    MG_TRY()
    CHECKARGUMENTNULL(coord1, L"MgCoordinateSystemMeasure.GetDistance");
    CHECKARGUMENTNULL(coord2, L"MgCoordinateSystemMeasure.GetDistance");

    distance = GetDistance(coord1->GetX(), coord1->GetY(), coord2->GetX(), coord2->GetY());
    MG_CATCH_AND_THROW(L"MgCoordinateSystemMeasure.GetDistance")
    return distance;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Gets the angle with respect to the north of a vector formed by two
/// coordinates.
///</summary>
///<param name="double x1">
/// The x value that defines the first coordinate.
///</param>
///<param name="double y1">
/// The y value that defines the first coordinate.
///</param>
///<param name="double x2">
/// The x value that defines the second coordinate.
///</param>
///<param name="double y2">
/// The y value that defines the second coordinate.
///</param>
///<returns>
/// The azimuth (Angle with respect to the North) of the vector formed by
/// the 2 specified coordinates.
///</returns>
double CCoordinateSystemMeasure::GetAzimuth(double x1, double y1, double x2, double y2)
{
    double azimuth = 0.0;

    MG_TRY()
    azimuth = m_pCoordSys->GetAzimuth(x1, y1, x2, y2);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemMeasure.GetAzimuth")

    return azimuth;
}

///////////////////////////////////////////////////////////////////////////
double CCoordinateSystemMeasure::GetAzimuth(MgCoordinate* coord1, MgCoordinate* coord2)
{
    double azimuth = 0.0;
    MG_TRY()
    CHECKARGUMENTNULL(coord1, L"MgCoordinateSystemMeasure.GetAzimuth");
    CHECKARGUMENTNULL(coord2, L"MgCoordinateSystemMeasure.GetAzimuth");
    azimuth = GetAzimuth(coord1->GetX(), coord1->GetY(), coord2->GetX(), coord2->GetY());
    MG_CATCH_AND_THROW(L"MgCoordinateSystemMeasure.GetAzimuth")

    return azimuth;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Computes a coordinate a given distance along a vector that is defined
/// by a starting coordinate and an azimuth (Angle with respect to the
/// North).
///</summary>
///<param name="double xStart">
/// The x value that represents the start of the vector.
///</param>
///<param name="double yStart">
/// The y value that represents the start of the vector.
///</param>
///<param name="double azimuth">
/// An azimuth (Angle with respect to the North) that defines the direction
/// of the vector.
///</param>
///<param name="double distance">
/// The distance along the vector of the desired coordinate in coordinate system units.
///</param>
///<param name="double& x">
/// The x value that represents the resultant coordinate.
///</param>
///<param name="double& y">
/// The y value that represents the resultant coordinate.
///</param>
///<returns>
/// Nothing.
///</returns>
MgCoordinate* CCoordinateSystemMeasure::GetCoordinate(double xStart, double yStart, double azimuth, double distance)
{
    MgCoordinate* pCoordinate=NULL;

    MG_TRY()
    pCoordinate=m_pCoordSys->GetCoordinate(xStart, yStart, azimuth, distance);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemMeasure.GetCoordinate")
    return pCoordinate;
}

///////////////////////////////////////////////////////////////////////////
MgCoordinate* CCoordinateSystemMeasure::GetCoordinate(MgCoordinate* coord, double azimuth, double distance)
{
    CHECKARGUMENTNULL(coord, L"MgCoordinateSystemMeasure.GetCoordinate");
    return GetCoordinate(coord->GetX(), coord->GetY(), azimuth, distance);
}

///////////////////////////////////////////////////////////////////////////
MgEnvelope* CCoordinateSystemMeasure::GetEnvelope()
{
    MgEnvelope* pNew=NULL;
    pNew=new MgEnvelope(m_pCoordSys->GetMinX(), m_pCoordSys->GetMinY(), m_pCoordSys->GetMaxX(), m_pCoordSys->GetMaxY());
    if (NULL == pNew)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemMeasure.GetEnvelope", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    return pNew;
}

///////////////////////////////////////////////////////////////////////////
MgCoordinateSystem* CCoordinateSystemMeasure::GetCoordSys()
{
    return SAFE_ADDREF(m_pCoordSys.p);
}
