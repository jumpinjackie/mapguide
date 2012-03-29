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

#ifndef _MGMOVETOINSTRUCTION_H_
#define _MGMOVETOINSTRUCTION_H_

/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// MgMoveToInstruction defines the beginning coordinate of a new segment in a
/// path.
class MgMoveToInstruction : public MgGeometricPathInstruction
{
    DECLARE_CLASSNAME(MgMoveToInstruction)

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the type of this instruction.
    ///
    /// \return
    /// Returns MgGeometricPathInstructionType.MoveTo.
    ///
    virtual INT32 GetType();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate to move the path to.
    ///
    /// \return
    /// The MgCoordinate to move to.
    ///
    virtual MgCoordinate* GetCoordinate();
};
/// \endcond

#endif //_MGMOVETOINSTRUCTION_H_
