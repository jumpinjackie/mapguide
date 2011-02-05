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

#ifndef MG_BYTE_SOURCE_DWF_INPUT_STREAM_IMPL_H_
#define MG_BYTE_SOURCE_DWF_INPUT_STREAM_IMPL_H_

#include "ServerResourceDllExport.h"

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// MgByteSourceDwfInputStreamImpl provides access to the bytes on a DWF input
/// stream source.
/// </summary>
///
class MG_SERVER_RESOURCE_API MgByteSourceDwfInputStreamImpl : public ByteSourceImpl
{
public:

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Constructs an MgByteSourceDwfInputStreamImpl object.
    /// </summary>
    ///
    /// <param name="inputStream">
    /// DWFInputStream object to use.
    /// </param>
    ///
    MgByteSourceDwfInputStreamImpl(DWFInputStream* inputStream);

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Destructs an MgByteSourceDwfInputStreamImpl object.
    /// </summary>
    ///
    virtual ~MgByteSourceDwfInputStreamImpl();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Returns the remaining length of the byte source.  This length is
    /// adjusted for previous reads.  If the returned length is zero
    /// then the underlying source may be a streaming format and the length
    /// is not known.
    /// </summary>
    ///
    /// <returns>
    /// Remaining length of the byte source.
    /// </returns>
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

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Rewinds to the start of memory.  Length is reset.
    /// </summary>
    ///
    /// <returns>
    /// Nothing.
    /// </returns>
    ///
    void Rewind();

    ///////////////////////////////////////////////////////////////////////////
    /// <summary>
    /// Reads a buffer.
    /// </summary>
    ///
    /// <param name="buffer">
    /// A buffer receiving the data.
    /// </param>
    /// <param name="length">
    /// Maximum number of bytes to read.
    /// </param>
    ///
    /// <returns>
    /// Actual number of bytes put in the buffer. 0 means end of bytes.
    /// </returns>
    ///
    INT32 Read(BYTE_ARRAY_OUT buffer, INT32 length);

private:

    // Unimplemented Methods

    MgByteSourceDwfInputStreamImpl();
    MgByteSourceDwfInputStreamImpl(const MgByteSourceDwfInputStreamImpl&);
    MgByteSourceDwfInputStreamImpl& operator=(
        const MgByteSourceDwfInputStreamImpl&);

protected:

    DWFInputStream* m_inputStream;
};

#endif
