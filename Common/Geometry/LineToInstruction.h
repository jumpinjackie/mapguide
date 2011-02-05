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

#ifndef _MGLINETOINSTRUCTION_H_
#define _MGLINETOINSTRUCTION_H_

/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// MgLineToInstruction defines a straight line from the last coordinate in the
/// path to the coordinate included with this instruction.
class MgLineToInstruction : public MgGeometricPathInstruction
{
    DECLARE_CLASSNAME(MgLineToInstruction)

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the type of this instruction.
    ///
    /// \return
    /// Returns MgGeometricPathInstructionType.LineTo.
    ///
    virtual INT32 GetType();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate to define a line to.
    ///
    /// \return
    /// The MgCoordinate to define a line to.
    ///
    virtual MgCoordinate* GetCoordinate();
};
/// \endcond

#endif //_MGLINETOINSTRUCTION_H_
