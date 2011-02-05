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

#ifndef _MGCOORDINATESYSTEMGEODETICTRANSFORMDEFPARAMS_H_
#define _MGCOORDINATESYSTEMGEODETICTRANSFORMDEFPARAMS_H_

/// \defgroup MgCoordinateSystemGeodeticTransformDefParams MgCoordinateSystemGeodeticTransformDefParams
/// \ingroup Coordinate_System_classes
/// \{

////////////////////////////////////////////////////////////////
/// \brief
/// Common base class for all geodetic transformation parameters.
///
class MG_GEOMETRY_API MgCoordinateSystemGeodeticTransformDefParams : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgCoordinateSystemGeodeticTransformDefParams)

PUBLISHED_API:

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks, whether this transformation parameter object's content is considered (technically) valid.
    /// That is, even if this method returns true, the transformation might still fail due to
    /// other reasons, e.g. a grid file does not exist.
    ///
    /// \return
    /// Returns true, if this parameter object is considered valid. Otherwise false.
    virtual bool IsValid() = 0;

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Checks, whether this transformation object is protected, i.e. any attempts to change its
    /// content will fail. All definitions contained in the dictionaries at compile time
    /// are protected by default.
    ///
    /// \return
    /// Returns true, if this parameter object is read-only. Otherwise false.
    virtual bool IsProtected() = 0;

};

/// \}

#endif //_MGCOORDINATESYSTEMGEODETICTRANSFORMDEFPARAMS_H_
