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

#ifndef MG_BYTE_SOURCE_IMPL_H_
#define MG_BYTE_SOURCE_IMPL_H_

/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////////
/// \brief
/// Placeholder for various ByteSourceInfo derived classes, which supply
/// the actual access to the bytes in the source
class MG_FOUNDATION_API ByteSourceImpl
{
public:

    /// Constructor
    ByteSourceImpl() {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Destruct a ByteSourceImpl object
    ///
    virtual ~ByteSourceImpl() {}

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Reads a buffer
    ///
    /// \param buffer
    /// A buffer receiving the data.
    /// \param length
    /// Maximum number of bytes to read
    ///
    /// \return
    /// Actual number of bytes put in the buffer. 0 means end of bytes
    ///
    virtual INT32 Read(BYTE_ARRAY_OUT buffer, INT32 length) = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the remaining length of the byte source.  This length is
    /// adjusted for previous reads.  If the returned length is zero
    /// then the underlying source may be a streaming format and the length
    /// is not known.
    ///
    /// \return
    /// Remaining length of the byte source
    ///
    virtual INT64 GetLength() = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the source is rewindable.
    ///
    /// \return
    /// true if the source is rewindable, false otherwise.
    ///
    virtual bool IsRewindable() = 0;

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Rewinds to the start of the source.  Depending on the source of
    /// the reader, Rewind may not be supported.  Readers sourced from
    /// true streams cannot be rewound.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgInvalidOperationException if source cannot be rewound
    ///
    virtual void Rewind() = 0;
};
/// \endcond

#endif // MG_BYTE_SOURCE_IMPL_H_
