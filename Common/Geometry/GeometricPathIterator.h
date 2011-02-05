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

#ifndef _MGGEOMETRICPATHITERATOR_H_
#define _MGGEOMETRICPATHITERATOR_H_

/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// The abstract MgGeometricPathIterator class supports iteration over a
/// geometric entities path. A path is a series of instructions, where the
/// instructions define each segment of the geometry. MgGeometricPathIterator
/// supports the following instructions:
///
/// MoveTo - Defines the beginning coordinate of a new segment in a path.
///
/// LineTo - Defines a straight line from the last coordinate in the path to
/// the coordinate included with this instruction.
///
/// QuadTo - Defines a quadratic curve from the last coordinate in the
/// path using the first coordinate of this instruction as the end point and
/// the second coordinate as the control point.
///
/// Close - Defines a line from the current coordinate in the path back to the
/// last MoveTo instruction, which is the beginning of the last segment in the
/// path.
///
/// With each instruction an associated set of coordinates and/or parameters is
/// accessible from the appropriate MgGeometricPathInstruction derived class.
/// MgGeometricPathIterator only supports read access to the underlying
/// geometry data.
///
/// Initially, the iterator is positioned before the first instruction. Reset
/// also brings the iterator back to this position. At this position, calling
/// GetCurrentInstruction throws an exception. Therefore, you must call
/// MoveNext to advance the iterator to the first instruction before calling
/// GetCurrentInstruction.
class MgGeometricPathIterator : public MgDisposable
{
EXTERNAL_API:

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the current instruction. After an iterator is created or after a
    /// Reset, MoveNext must be called to advance the iterator to the first
    /// instruction before calling GetCurrentInstruction; otherwise, the
    /// current position is undefined and an exception is thrown.
    ///
    /// \return
    /// The MgGeometricPathInstruction at the current position.
    ///
    virtual MgGeometricPathInstruction* GetCurrentInstruction() = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Advances the iterator to the next instruction. After an iterator is
    /// created or after a call to Reset, an iterator is positioned before the
    /// first instruction, and the first call to MoveNext moves the iterator
    /// over the first instruction. After the last instruction is passed,
    /// subsequent calls to MoveNext return false until Reset is called.
    ///
    /// \return
    /// True if the iterator was successfully advanced to the next instruction;
    /// false if the iterator has passed the last instruction.
    ///
    virtual bool MoveNext() = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the iterator to its initial position prior to the first
    /// instruction.
    ///
    virtual void Reset() = 0;
};
/// \endcond

#endif //_MGGEOMETRICPATHITERATOR_H_
