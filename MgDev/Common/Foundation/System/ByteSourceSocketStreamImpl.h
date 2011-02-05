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

#ifndef MG_BYTE_SOURCE_SOCKET_STREAM_IMPL_H_
#define MG_BYTE_SOURCE_SOCKET_STREAM_IMPL_H_

/// INTERNAL_CLASS - NOT EXPOSED
/// \cond INTERNAL

/////////////////////////////////////////////////////////////////
/// \brief
/// ByteSourceSocketStreamImpl provides access to the bytes on a file source
class ByteSourceSocketStreamImpl : public ByteSourceImpl
{
    DECLARE_CLASSNAME(ByteSourceSocketStreamImpl)

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a ByteSourceSocketStreamImpl object
    ///
    /// \param serverConn
    /// server connection
    ///
    ByteSourceSocketStreamImpl(INT64 len, MgServerConnectionBase* serverConn);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destroy ByteSourceSocketStreamImpl object
    ///
    virtual ~ByteSourceSocketStreamImpl();

    //////////////////////////////////////////////////////////////////
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
    INT32 Read(BYTE_ARRAY_OUT buffer, INT32 length);

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the remaining length of the byte source.  This length is
    /// adjusted for previous reads.  If the returned length is zero
    /// then the underlying source may be a streaming format and the length
    /// is not known.
    ///
    /// \return
    /// Remaining length of the byte source
    ///
    INT64 GetLength();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the source is rewindable.
    ///
    /// \return
    /// true if the source is rewindable, false otherwise.
    ///
    bool IsRewindable();

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Rewinds to the start of the source.  This implementation
    /// does not support rewind.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgInvalidOperationException will be thrown since socket stream cannot be rewound
    ///
    void Rewind();

private:

    void Close();

protected:
    Ptr<MgServerConnectionBase> m_conn;
    INT64     m_len;
    INT64     m_pos;
    INT32     m_blockSize;
    INT32     m_blockRead;
};

/// \endcond

#endif // MG_BYTE_SOURCE_SOCKET_STREAM_IMPL_H_
