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
    CCoordinateSystemMeasure(MgCoordinateSystem* coordSys);

    virtual ~CCoordinateSystemMeasure();

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Computes the distance between two coordinates.
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
    /// The distance between coord1 and coord2 as a double in coordinate system units.
    ///</returns>
    virtual double GetDistance(double x1, double y1, double x2, double y2);
    virtual double GetDistance(MgCoordinate* coord1, MgCoordinate* coord2);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Computes the angle with respect to the north of a vector formed by two
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
    /// coord1 and coord2.
    ///</returns>
    virtual double GetAzimuth(double x1, double y1, double x2, double y2);
    virtual double GetAzimuth(MgCoordinate* coord1, MgCoordinate* coord2);

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
    virtual void GetCoordinate(double xStart, double yStart, double azimuth, double distance, double&x, double& y);
    virtual MgCoordinate* GetCoordinate(MgCoordinate* coord, double azimuth, double distance);
    virtual MgCoordinate* GetCoordinate(double xStart, double yStart, double azimuth, double distance);

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Returns the envelope for the coordinate system being used.
    ///</summary>
    ///<returns>
    /// A MgEnvelope representing the coordinate system envelope.
    ///</returns>
    virtual MgEnvelope* GetEnvelope();

    ///////////////////////////////////////////////////////////////////////////
    ///<summary>
    /// Returns the coordinate system being used.
    ///</summary>
    ///<returns>
    /// The coordinate system being used.
    ///</returns>
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

    Ptr<MgCoordinateSystem> m_coordSys;
};

} // End of namespace

#endif //_CCOORDINATESYSTEMMEASURE_H_
