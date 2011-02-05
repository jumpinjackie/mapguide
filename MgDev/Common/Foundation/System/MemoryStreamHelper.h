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

#ifndef MGMEMORYSTREAMHELPER_H_MG
#define MGMEMORYSTREAMHELPER_H_MG

#include "StreamHelper.h"

class MgMemoryStreamHelper;
template class MG_FOUNDATION_API Ptr<MgMemoryStreamHelper>;

/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////
//  The MgMemoryStreamHelper class provides an interface to read and write data
//  from an underlying memory data stream.
class MG_FOUNDATION_API MgMemoryStreamHelper : public MgStreamHelper
{
    DECLARE_CLASSNAME(MgMemoryStreamHelper)

    ///////////////////////////////////////////////////////////////////////
    ///  Constructors/Destructors
    public:
    MgMemoryStreamHelper();
    MgMemoryStreamHelper(INT8* buffer, INT32 length, bool ownMemory = false);
    virtual ~MgMemoryStreamHelper();

    ///////////////////////////////////////////////////////////////////////
    ///  Methods
    public:

    ////////////////////////////////////
    ////////////////////////////////////
    /// Buffered Reader Methods
    MgStreamStatus GetData( void* buffer, size_t size, bool blocking = true, bool peeking = false );

    MgStreamStatus GetUINT8( UINT8& data, bool blocking = true, bool peeking = false );

    MgStreamStatus GetUINT16( UINT16& data, bool blocking = true, bool peeking = false );

    MgStreamStatus GetUINT32( UINT32& data, bool blocking = true, bool peeking = false );

    MgStreamStatus GetINT64( INT64& data, bool blocking = true, bool peeking = false );

    /// Warning:  only works for strings up to 1k in size
    MgStreamStatus GetNullTermString( REFSTRING data, bool blocking = true, bool peeking = false );

    MgStreamStatus GetSingle(float& data, bool blocking = true, bool peeking = false);

    MgStreamStatus GetDouble(double& data, bool blocking = true, bool peeking = false);

    ////////////////////////////////////
    ////////////////////////////////////
    /// Cached Writing Methods

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Writes a byte.
    ///
    /// \param value
    /// 8 bit byte to write
    ///
    MgStreamStatus WriteUINT8( UINT8 value );

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Writes a TCP/IP 16 bit integer.  This routine should make our
    /// data transfer platform independent
    ///
    /// \param value
    /// 16 bit integer to write
    ///
    MgStreamStatus WriteUINT16( UINT16 value );

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Writes a TCP/IP 32 bit integer.  This routine should make our
    /// data transfer platform independent
    ///
    /// \param value
    /// 32 bit integer to write
    ///
    MgStreamStatus WriteUINT32( UINT32 value );

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Writes a TCP/IP 64 bit integer.  This routine should make our
    /// data transfer platform independent
    ///
    /// \param value
    /// 64 bit integer to write
    ///
    MgStreamStatus WriteINT64( INT64 value );

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Writes a double using the INT64 method above.  This routine should
    /// be platform independant
    ///
    /// \param value
    /// double to write
    ///
    MgStreamStatus WriteDouble( double value );

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Writes a single. This routine should be platform independant
    ///
    /// \param value
    /// single to write
    ///
    MgStreamStatus WriteSingle( float value );

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Writes a null terminated string.  Only works for strings up to
    /// 1k in size.
    ///
    /// \param value
    /// String to write
    ///
    MgStreamStatus WriteNullTermString( CREFSTRING value );

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Recursive routine to write an arbitrarily large input buffer to the stream
    /// If the input fits in our internal buffer, just use the internal buffer.
    ///
    MgStreamStatus WriteBytes(const unsigned char* buf, int nBytes);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Flushes internal memory buffer to stream helper.  This method
    /// should be called once an entire packet has been assembled.
    ///
    MgStreamStatus Flush();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get a pointer to the data in this memory stream
    ///
    const INT8* GetBuffer();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the length of the data in this stream
    ///
    INT32 GetLength();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Encode stream as Base64
    ///
    string ToBase64();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Decode stream from Base64
    ///
    void FromBase64(CREFSTRING b64string);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Clears internal buffer
    ///
    void Clear();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Resets reading to start of buffer
    ///
    void Rewind();

    protected:

    void Dispose();

    private:

    INT8*   m_buffer;
    INT8*   m_endBuffer;
    INT8*   m_curPtrWrite;
    INT8*   m_curPtrRead;
    INT32   m_blockCount;
    bool    m_ownMemory;

    /// minimum size of a memory bloc
    static INT32 m_blockSize;
};
/// \endcond

#endif  //  MGMEMORYSTREAMHELPER_H_MG
