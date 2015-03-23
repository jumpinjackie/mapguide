//
//  Copyright (C) 2004-2013 by Autodesk, Inc.
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
#ifndef READER_BYTE_SOURCE_IMPL
#define READER_BYTE_SOURCE_IMPL

#include "Foundation.h"
#include "XmlJsonConvert.h"

class MgReader;

// MgReaderByteSourceImpl provides an adapter layer to MgReader
// objects, allowing them to be streamed/iterated with a MgByteReader
class MG_MAPAGENT_API MgReaderByteSourceImpl : public ByteSourceImpl
{
    DECLARE_CLASSNAME(MgReaderByteSourceImpl)

public:
    MgReaderByteSourceImpl(MgReader* reader, CREFSTRING format);
    virtual ~MgReaderByteSourceImpl();

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
    virtual INT32 Read(BYTE_ARRAY_OUT buffer, INT32 length);

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
    virtual INT64 GetLength();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the source is rewindable.
    ///
    /// \return
    /// true if the source is rewindable, false otherwise.
    ///
    virtual bool IsRewindable();

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
    virtual void Rewind();

private:
    INT32 ReadInternalBuffer(BYTE_ARRAY_OUT buffer, INT32 fromIndex, INT32 length);

    Ptr<MgReader> m_reader;
    STRING m_format;
    bool m_bReadHeader;
    bool m_bInternalReaderHasMore;
    bool m_bFirstRecord;

    std::string m_buf;
    INT32 m_bufOffset;
};

#endif