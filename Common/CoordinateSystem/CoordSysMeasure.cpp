//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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
CCoordinateSystemMeasure::CCoordinateSystemMeasure(MgCoordinateSystem* coordSys) :
    m_coordSys(NULL)
{
    if (NULL == coordSys)
    {
        throw new MgNullArgumentException(
            L"MgCoordinateSystemMeasure.CCoordinateSystemMeasure",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }

    m_coordSys = SAFE_ADDREF(coordSys);
}

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

    try
    {
        if(m_coordSys->GetType() == MgCoordinateSystemType::Arbitrary)
        {
            distance = m_coordSys->MeasureEuclideanDistance(x1, y1, x2, y2);
        }
        else
        {
            distance = m_coordSys->MeasureGreatCircleDistance(x1, y1, x2, y2);
        }
    }
    catch (MgException* e)
    {
        e->Raise();
    }
    catch (...)
    {
        throw new MgCoordinateSystemMeasureFailedException(L"MgCoordinateSystem.GetDistance", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemUnexpectedError", NULL);
    }

    return distance;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Computes the angle with respect to the north of a vector formed by two
/// coordinates.
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
/// The azimuth (Angle with respect to the North) of the vector formed by
/// coord1 and coord2.
///</returns>
double CCoordinateSystemMeasure::GetAzimuth(double x1, double y1, double x2, double y2)
{
    double azimuth = 0.0;

    try
    {
        azimuth = m_coordSys->GetAzimuth(x1, y1, x2, y2);
    }
    catch (MgException* e)
    {
        e->Raise();
    }
    catch (...)
    {
        throw new MgCoordinateSystemComputationFailedException(L"MgCoordinateSystemMeasure.GetAzimuth", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemUnexpectedError", NULL);
    }

    return azimuth;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Computes a coordinate a given distance along a vector that is defined
/// by a starting coordinate and an azimuth (Angle with respect to the
/// North).
///</summary>
///<param name="xStart">
/// The x value that represents the start of the vector.
///</param>
///<param name="yStart">
/// The y value that represents the start of the vector.
///</param>
///<param name="azimuth">
/// An azimuth (Angle with respect to the North) that defines the direction
/// of the vector.
///</param>
///<param name="distance">
/// The distance along the vector of the desired coordinate in coordinate system units.
///</param>
///<returns>
/// An CCoordinate that lies the given distance along the vector.
///</returns>
void CCoordinateSystemMeasure::GetCoordinate(double xStart, double yStart, double azimuth, double distance, double& x, double& y)
{
    x = 0.0;
    y = 0.0;

    try
    {
        Ptr<MgCoordinate> coord = m_coordSys->GetCoordinate(xStart, yStart, azimuth, distance);
        x = coord->GetX();
        y = coord->GetY();
    }
    catch (MgException* e)
    {
        e->Raise();
    }
    catch(...)
    {
        throw new MgCoordinateSystemComputationFailedException(L"MgCoordinateSystemMeasure.GetCoordinate", __LINE__, __WFILE__, NULL, L"MgCoordinateSystemUnexpectedError", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////
double CCoordinateSystemMeasure::GetDistance(MgCoordinate* coord1, MgCoordinate* coord2)
{
    double distance = 0.0;
    MG_TRY()
    if((NULL == coord1) || (NULL == coord2))
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemMeasure.GetDistance", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    distance = GetDistance(coord1->GetX(), coord1->GetY(), coord2->GetX(), coord2->GetY());
    MG_CATCH_AND_THROW(L"MgCoordinateSystemMeasure.GetDistance")
    return distance;
}

///////////////////////////////////////////////////////////////////////////
double CCoordinateSystemMeasure::GetAzimuth(MgCoordinate* coord1, MgCoordinate* coord2)
{
    double azimuth = 0.0;
    MG_TRY()
    if((NULL == coord1) || (NULL == coord2))
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemMeasure.GetAzimuth", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    azimuth = GetAzimuth(coord1->GetX(), coord1->GetY(), coord2->GetX(), coord2->GetY());
    MG_CATCH_AND_THROW(L"MgCoordinateSystemMeasure.GetAzimuth")

    return azimuth;
}

MgCoordinate* CCoordinateSystemMeasure::GetCoordinate(double xStart, double yStart, double azimuth, double distance)
{
    MgCoordinate* pCoordinate=NULL;

    MG_TRY()
    pCoordinate=m_coordSys->GetCoordinate(xStart, yStart, azimuth, distance);
    MG_CATCH_AND_THROW(L"MgCoordinateSystemMeasure.GetCoordinate")
    return pCoordinate;
}

///////////////////////////////////////////////////////////////////////////
MgCoordinate* CCoordinateSystemMeasure::GetCoordinate(MgCoordinate* coord, double azimuth, double distance)
{
    if(NULL == coord)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemMeasure.GetCoordinate", __LINE__, __WFILE__, NULL, L"", NULL);
    }
    return GetCoordinate(coord->GetX(), coord->GetY(), azimuth, distance);
}

MgCoordinateSystem* CCoordinateSystemMeasure::GetCoordSys()
{
    return SAFE_ADDREF(m_coordSys.p);
}

MgEnvelope* CCoordinateSystemMeasure::GetEnvelope()
{
    return new MgEnvelope(m_coordSys->GetMinX(), m_coordSys->GetMinY(), m_coordSys->GetMaxX(), m_coordSys->GetMaxY());
}
