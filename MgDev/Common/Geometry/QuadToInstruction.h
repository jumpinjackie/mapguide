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

#ifndef _MGQUADTOINSTRUCTION_H_
#define _MGQUADTOINSTRUCTION_H_

/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// MgQuadToInstruction defines a quadratic curve from the last coordinate in
/// the path using the end coordinate and control coordinate included with this
/// instruction.
class MgQuadToInstruction : public MgGeometricPathInstruction
{
    DECLARE_CLASSNAME(MgQuadToInstruction)

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the type of this instruction.
    ///
    /// \return
    /// Returns MgGeometricPathInstructionType.QuadTo.
    ///
    virtual INT32 GetType();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the ending coordinate of the quadratic curve.
    ///
    /// \return
    /// The MgCoordinate defining the end of the curve.
    ///
    virtual MgCoordinate* GetEndCoordinate();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the control coordinate for the quadratic curve.
    ///
    /// \return
    /// The MgCoordinate to use as the control point.
    ///
    virtual MgCoordinate* GetControlCoordinate();
};
/// \endcond

#endif //_MGQUADTOINSTRUCTION_H_
