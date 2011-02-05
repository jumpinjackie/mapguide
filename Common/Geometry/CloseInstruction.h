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

#ifndef _MGCLOSEINSTRUCTION_H_
#define _MGCLOSEINSTRUCTION_H_

class MgGeometricPathInstruction;

/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// MgCloseInstruction defines a line from the current coordinate in the path
/// back to the last MgMoveToInstruction, which is the beginning of the
/// last segment in the path.
class MgCloseInstruction : public MgGeometricPathInstruction
{
    DECLARE_CLASSNAME(MgCloseInstruction)

EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the type of this instruction.
    ///
    /// \return
    /// Returns MgGeometricPathInstructionType.Close.
    ///
    virtual INT32 GetType();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the coordinate that started the segment.
    ///
    /// \return
    /// The MgCoordinate included with the most recent MoveTo instruction.
    ///
    virtual MgCoordinate* GetCoordinate();
};
/// \endcond

#endif //_MGCLOSEINSTRUCTION_H_
