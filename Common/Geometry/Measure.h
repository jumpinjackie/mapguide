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

#ifndef _MGMEASURE_H_
#define _MGMEASURE_H_

/// \defgroup MgMeasure MgMeasure
/// \ingroup Geometry_Module_classes
/// \{

///////////////////////////////////////////////////////////////
/// \brief
/// MgMeasure is an abstract class that can be implemented to
/// provide non-linear measurement for geometry operations.
///
/// \remarks
/// This is a base class, not designed to be used directly. Use
/// the derived class.
///
class MgMeasure : public MgGuardDisposable
{
EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the distance between two coordinates.
    ///
    /// \param coord1
    /// An MgCoordinate that specifies the first coordinate.
    /// \param coord2
    /// An MgCoordinate that specifies the second coordinate.
    ///
    /// \return
    /// The distance between coord1 and coord2 as a double in Meters.
    ///
    virtual double GetDistance(MgCoordinate* coord1, MgCoordinate* coord2) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the angle with respect to the north of a vector formed by two
    /// coordinates.
    ///
    /// \param coord1
    /// An MgCoordinate that specifies the first coordinate.
    /// \param coord2
    /// An MgCoordinate that specifies the second coordinate.
    ///
    /// \return
    /// The azimuth (Angle with respect to the North) of the vector formed by
    /// coord1 and coord2.
    ///
    virtual double GetAzimuth(MgCoordinate* coord1, MgCoordinate* coord2) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Computes a coordinate a given distance along a vector that is defined
    /// by a starting coordinate and an azimuth (Angle with respect to the
    /// North).
    ///
    /// \param coord
    /// An MgCoordinate that represents the start of the vector.
    /// \param azimuth
    /// An azimuth (Angle with respect to the North) that defines the direction
    /// of the vector.
    /// \param distance
    /// The distance along the vector of the desired coordinate in Meters.
    ///
    /// \return
    /// An MgCoordinate that lies the given distance along the vector.
    ///
    virtual MgCoordinate* GetCoordinate(MgCoordinate* coord, double azimuth, double distance) = 0;
};
/// \}

#endif //_MGMEASURE_H_
