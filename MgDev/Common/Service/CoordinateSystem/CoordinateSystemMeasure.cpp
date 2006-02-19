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

#include "CoordinateSystemCommon.h"

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Initializes a new instance of MgCoordinateSystemMeasure given a
/// coordinate system object. If the coordinate system supports it, the
/// ellipsoid definition is used in all computations.
///</summary>
///<param name="coordSys">
/// The coordinate system to use when performing measurement.
///</param>
MgCoordinateSystemMeasure::MgCoordinateSystemMeasure(MgCoordinateSystem* coordSys) :
    m_coordSysMeasure(NULL)
{
    ACE_ASSERT(coordSys);

    if(NULL == coordSys)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemMeasure.MgCoordinateSystemMeasure", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    m_coordSys = SAFE_ADDREF(coordSys);

    m_coordSysMeasure = new CCoordinateSystemMeasure(coordSys->GetInternalCoordinateSystem());
    if(NULL == m_coordSysMeasure)
    {
        throw new MgOutOfMemoryException(L"MgCoordinateSystemMeasure.MgCoordinateSystemMeasure", __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

MgCoordinateSystemMeasure::~MgCoordinateSystemMeasure()
{
    if(m_coordSysMeasure)
    {
        delete m_coordSysMeasure;
        m_coordSysMeasure = NULL;
    }
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Computes the distance between two coordinates.
///</summary>
///<param name="coord1">
/// An MgCoordinate that specifies the first coordinate.
///</param>
///<param name="coord2">
/// An MgCoordinate that specifies the second coordinate.
///</param>
///<returns>
/// The distance between coord1 and coord2 as a double in coordinate system units.
///</returns>
double MgCoordinateSystemMeasure::GetDistance(MgCoordinate* coord1, MgCoordinate* coord2)
{
    double distance = 0.0;

    if((NULL == coord1) || (NULL == coord2))
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemMeasure.GetDistance", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        distance = m_coordSysMeasure->GetDistance(coord1->GetX(), coord1->GetY(), coord2->GetX(), coord2->GetY());
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemMeasureFailedException(L"MgCoordinateSystemMeasure.GetDistance", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemMeasureFailedException(L"MgCoordinateSystemMeasure.GetDistance", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return distance;
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
double MgCoordinateSystemMeasure::GetDistance(double x1, double y1, double x2, double y2)
{
    double distance = 0.0;

    try
    {
        distance = m_coordSysMeasure->GetDistance(x1, y2, x2, y2);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemMeasureFailedException(L"MgCoordinateSystemMeasure.GetDistance", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemMeasureFailedException(L"MgCoordinateSystemMeasure.GetDistance", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return distance;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Computes the angle with respect to the north of a vector formed by two
/// coordinates.
///</summary>
///<param name="coord1">
/// An MgCoordinate that specifies the first coordinate.
///</param>
///<param name="coord2">
/// An MgCoordinate that specifies the second coordinate.
///</param>
///<returns>
/// The azimuth (Angle with respect to the North) of the vector formed by
/// coord1 and coord2.
///</returns>
double MgCoordinateSystemMeasure::GetAzimuth(MgCoordinate* coord1, MgCoordinate* coord2)
{
    double azimuth = 0.0;

    if((NULL == coord1) || (NULL == coord2))
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemMeasure.GetAzimuth", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        azimuth = m_coordSysMeasure->GetAzimuth(coord1->GetX(), coord1->GetY(), coord2->GetX(), coord2->GetY());
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemComputationFailedException(L"MgCoordinateSystemMeasure.GetAzimuth", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemComputationFailedException(L"MgCoordinateSystemMeasure.GetAzimuth", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return azimuth;
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
double MgCoordinateSystemMeasure::GetAzimuth(double x1, double y1, double x2, double y2)
{
    double azimuth = 0.0;

    try
    {
        azimuth = m_coordSysMeasure->GetAzimuth(x1, y1, x2, y2);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemComputationFailedException(L"MgCoordinateSystemMeasure.GetAzimuth", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemComputationFailedException(L"MgCoordinateSystemMeasure.GetAzimuth", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return azimuth;
}

///////////////////////////////////////////////////////////////////////////
///<summary>
/// Computes a coordinate a given distance along a vector that is defined
/// by a starting coordinate and an azimuth (Angle with respect to the
/// North).
///</summary>
///<param name="coord">
/// An MgCoordinate that represents the start of the vector.
///</param>
///<param name="azimuth">
/// An azimuth (Angle with respect to the North) that defines the direction
/// of the vector.
///</param>
///<param name="distance">
/// The distance along the vector of the desired coordinate in coordinate system units.
///</param>
///<returns>
/// An MgCoordinate that lies the given distance along the vector.
///</returns>
MgCoordinate* MgCoordinateSystemMeasure::GetCoordinate(MgCoordinate* coord, double azimuth, double distance)
{
    MgCoordinate* pCoordinate = NULL;

    if(NULL == coord)
    {
        throw new MgNullArgumentException(L"MgCoordinateSystemMeasure.GetCoordinate", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    try
    {
        double x = 0.0;
        double y = 0.0;

        m_coordSysMeasure->GetCoordinate(coord->GetX(), coord->GetY(), azimuth, distance, x, y);

        pCoordinate = new MgCoordinateXY(x, y);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemComputationFailedException(L"MgCoordinateSystemMeasure.GetCoordinate", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemComputationFailedException(L"MgCoordinateSystemMeasure.GetCoordinate", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return pCoordinate;
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
/// An MgCoordinate that lies the given distance along the vector.
///</returns>
MgCoordinate* MgCoordinateSystemMeasure::GetCoordinate(double xStart, double yStart, double azimuth, double distance)
{
    MgCoordinate* pCoordinate = NULL;

    try
    {
        double x = 0.0;
        double y = 0.0;

        m_coordSysMeasure->GetCoordinate(xStart, yStart, azimuth, distance, x, y);

        pCoordinate = new MgCoordinateXY(x, y);
    }
    catch(CException* e)
    {
        MgStringCollection arguments;
        arguments.Add(e->GetMessage());
        delete e;
        throw new MgCoordinateSystemComputationFailedException(L"MgCoordinateSystemMeasure.GetCoordinate", __LINE__, __WFILE__, NULL, L"MgFormatInnerExceptionMessage", &arguments);
    }
    catch(...)
    {
        throw new MgCoordinateSystemComputationFailedException(L"MgCoordinateSystemMeasure.GetCoordinate", __LINE__, __WFILE__, NULL, L"", NULL);
    }

    return pCoordinate;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgCoordinateSystemMeasure::GetClassId()
{
    return m_cls_id;
}

// <summary>Dispose this object.</summary>
// <returns>Returns nothing</returns>
void MgCoordinateSystemMeasure::Dispose()
{
    delete this;
}

MgCoordinateSystem* MgCoordinateSystemMeasure::GetCoordSys()
{
    return SAFE_ADDREF((MgCoordinateSystem*)m_coordSys);
}

MgEnvelope* MgCoordinateSystemMeasure::GetEnvelope()
{
    Ptr<MgCoordinate> lowerLeft = new MgCoordinateXY(m_coordSys->GetMinX(), m_coordSys->GetMinY());
    Ptr<MgCoordinate> upperRight = new MgCoordinateXY(m_coordSys->GetMaxX(), m_coordSys->GetMaxY());

    return new MgEnvelope(lowerLeft, upperRight);
}
