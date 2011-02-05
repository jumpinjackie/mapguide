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

#ifndef _MGGEOMETRICDIMENSION_H_
#define _MGGEOMETRICDIMENSION_H_

/// \defgroup MgGeometricDimension MgGeometricDimension
/// \ingroup Geometry_Module_classes
/// \{

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// MgGeometricDimension defines enumerated values used to specify the
/// dimension of a geometry.
class MgGeometricDimension
{
PUBLISHED_API:
    ///////////////////////////////////////////////////////////
    /// \brief
    /// Specifies that the geometry is represented by points in
    /// space, that is, is zero dimensional.
    ///
    static const INT32 Point = 0;

    ///////////////////////////////////////////////////////////
    /// \brief
    /// Specifies that the geometry is represented by curves in
    /// space, that is, is one dimensional.
    ///
    static const INT32 Curve = 1;

    ////////////////////////////////////////////////////////////
    /// \brief
    /// Specifies that the geometry is represented by regions in
    /// space that is. is two dimensional.
    ///
    static const INT32 Region = 2;

    //////////////////////////////////////////
    /// \brief
    /// Specifies that the geometry is empty.
    ///
    static const INT32 None = 3;

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Specifies that the geometry can be either points, curves, or
    /// regions, that is, either zero, one, or two dimensional.
    ///
    static const INT32 Any = 4;
};
/// \}

#endif //_MGGEOMETRICDIMENSION_H_
