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

#ifndef MGACESTREAMHELPER_H_
#define MGACESTREAMHELPER_H_

#include "StreamHelper.h"

class MgAceStreamHelper;
template class MG_FOUNDATION_API Ptr<MgAceStreamHelper>;

/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////
/// \brief
///  The MgAceStreamHelper class provides an interface to read and write data
///  from an underlying ACE data stream.
class MG_FOUNDATION_API MgAceStreamHelper : public MgStreamHelper
{
    DECLARE_CLASSNAME(MgAceStreamHelper)

    ///////////////////////////////////////////////////////////////////////
    ///  Constructors/Destructors
    public:
    MgAceStreamHelper();
    MgAceStreamHelper( ACE_HANDLE handle );
    virtual ~MgAceStreamHelper();

    ///////////////////////////////////////////////////////////////////////
    ///  Methods
    public:

    ////////////////////////////////////
    ////////////////////////////////////
    /// Buffered Reader Methods
    MgStreamStatus GetUINT8( UINT8& data, bool blocking = true, bool peeking = false );

    MgStreamStatus GetUINT16( UINT16& data, bool blocking = true, bool peeking = false );

    MgStreamStatus GetUINT32( UINT32& data, bool blocking = true, bool peeking = false );

    MgStreamStatus GetINT64( INT64& data, bool blocking = true, bool peeking = false );

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
    /// Flushes internal memory buffer to stream helper.  This method
    /// should be called once an entire packet has been assembled.
    ///
    MgStreamStatus Flush();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns whether or not stream is connected.
    ///
    bool IsConnected();

    protected:

    void Dispose();

private:

    ///////////////////////////////////////////////////////////////////////
    /// Private Functions

    MgStreamStatus GetData( void* buffer, size_t size, bool blocking = true, bool peeking = false );

    MgStreamStatus WriteData( void* buffer, size_t size, bool blocking = true, size_t* bytesWritten = NULL );

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Recursive routine to write an arbitrarily large input buffer to the stream
    /// If the input fits in our internal buffer, just use the internal buffer.
    ///
    MgStreamStatus WriteBytes(const unsigned char* buf, int nBytes);

    MgStreamStatus UpdateReadBuffers( void* buffer, size_t size, bool peeking);

    ///////////////////////////////////////////////////////////////////////
    ///  Member Variables

    ///////////////////////////////////////////////////////////////////////
    ///  Member Variables

    /// Read buffer variables
    size_t m_readBufSize;
    UINT8* m_readBuffer;
    size_t m_readBufStart;
    size_t m_readBufEnd;

    /// Write buffer variables
    size_t m_writeBufSize;
    UINT8* m_writeBuffer;
    size_t m_writeBufLength;

    ///  the handle to the stream
    const ACE_HANDLE    m_handle;

    /// TCP/IP packet size
    static INT32 m_tcpPacketSize;
};
/// \endcond

#endif  //  MGACESTREAMHELPER_H_
