//
//  Copyright (C) 2004-2006  Autodesk, Inc.
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

#include "CoordSysCommon.h"
#include "CoordSysIncludes.h"
#include "CoordSysUtil.h"

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Initializes a new instance of CCoordinateSystemMeasure given a
/// coordinate system object. If the coordinate system supports it, the
/// ellipsoid definition is used in all computations.
///</summary>
///<param name="coordSys">
/// The coordinate system to use when performing measurement.
///</param>
CCoordinateSystemMeasure::CCoordinateSystemMeasure(const CCoordinateSystem* coordSys) :
    m_coordSys(NULL)
{
    if(NULL == coordSys)
    {
        throw new CNullArgumentException(L"CCoordinateSystemMeasure.CCoordinateSystemMeasure", __LINE__, __WFILE__, L"[1] - CCoordinateSystem pointer.");
    }

    m_coordSys = const_cast<CCoordinateSystem*> (coordSys)->Clone();
    if(NULL == m_coordSys)
    {
        throw new COutOfMemoryException(L"CCoordinateSystemMeasure.CCoordinateSystemMeasure", __LINE__, __WFILE__, L"Could not allocate CCoordinateSystem.");
    }
}

CCoordinateSystemMeasure::~CCoordinateSystemMeasure()
{
    if(m_coordSys)
    {
        delete m_coordSys;
        m_coordSys = NULL;
    }
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
        if(m_coordSys->GetType() == CCoordinateSystemType::Arbitrary)
        {
            distance = m_coordSys->MeasureEuclideanDistance(x1, y1, x2, y2);
        }
        else
        {
            distance = m_coordSys->MeasureGreatCircleDistance(x1, y1, x2, y2);
        }
    }
    catch(CException* e)
    {
        STRING message = e->GetMessage();
        delete e;
        throw new CCoordinateSystemMeasureFailedException(L"CCoordinateSystemMeasure.GetDistance", __LINE__, __WFILE__, message);
    }
    catch(...)
    {
        throw new CCoordinateSystemMeasureFailedException(L"CCoordinateSystemMeasure.GetDistance", __LINE__, __WFILE__, L"Unexpected error.");
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
    catch(CException* e)
    {
        STRING message = e->GetMessage();
        delete e;
        throw new CCoordinateSystemComputationFailedException(L"CCoordinateSystemMeasure.GetAzimuth", __LINE__, __WFILE__, message);
    }
    catch(...)
    {
        throw new CCoordinateSystemComputationFailedException(L"CCoordinateSystemMeasure.GetAzimuth", __LINE__, __WFILE__, L"Unexpected error.");
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
        m_coordSys->GetCoordinate(xStart, yStart, azimuth, distance, x, y);
    }
    catch(CException* e)
    {
        STRING message = e->GetMessage();
        delete e;
        throw new CCoordinateSystemComputationFailedException(L"CCoordinateSystemMeasure.GetCoordinate", __LINE__, __WFILE__, message);
    }
    catch(...)
    {
        throw new CCoordinateSystemComputationFailedException(L"CCoordinateSystemMeasure.GetCoordinate", __LINE__, __WFILE__, L"Unexpected error.");
    }
}

CCoordinateSystem* CCoordinateSystemMeasure::GetCoordSys()
{
    return m_coordSys;
}

CEnvelope* CCoordinateSystemMeasure::GetEnvelope()
{
    return new CEnvelope(m_coordSys->GetMinX(), m_coordSys->GetMinY(), m_coordSys->GetMaxX(), m_coordSys->GetMaxY());
}
