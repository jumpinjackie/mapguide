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

#ifndef _CCOORDINATESYSTEMMEASURE_H_
#define _CCOORDINATESYSTEMMEASURE_H_

namespace CSLibrary
{
///////////////////////////////////////////////////////////////////////////////
///<summary>
/// The CCoordinateSystemMeasure class provides operations for performing
/// measurement of coordinates in a particular coordinate system. If
/// applicable the coordinate systems ellipsoid definition is used when
/// computing measurements.
///</summary>

class CCoordinateSystemMeasure : public MgCoordinateSystemMeasure
{
public:
    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Initializes a new instance of CCoordinateSystemMeasure given a
    /// coordinate system object. If the coordinate system supports it, the
    /// ellipsoid definition is used in all computations.
    ///</summary>
    ///<param name="CCoordinateSystem coordSys">
    /// The coordinate system to use when performing measurement.
    ///</param>
    CCoordinateSystemMeasure(MgCoordinateSystem* pCoordSys);
    virtual ~CCoordinateSystemMeasure();

    virtual double GetDistance(MgCoordinate* coord1, MgCoordinate* coord2);
    virtual double GetDistance(double x1, double y1, double x2, double y2);
    virtual double GetAzimuth(MgCoordinate* coord1, MgCoordinate* coord2);
    virtual double GetAzimuth(double x1, double y1, double x2, double y2);
    virtual MgCoordinate* GetCoordinate(MgCoordinate* coord, double azimuth, double distance);
    virtual MgCoordinate* GetCoordinate(double xStart, double yStart, double azimuth, double distance);
    virtual MgEnvelope* GetEnvelope();  /// __get

INTERNAL_API:
    virtual MgCoordinateSystem* GetCoordSys();

protected:
    /// \brief
    /// Dispose this object.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual void Dispose();

private:
    CCoordinateSystemMeasure();

    Ptr<MgCoordinateSystem> m_pCoordSys;
};

} // End of namespace

#endif //_CCOORDINATESYSTEMMEASURE_H_
