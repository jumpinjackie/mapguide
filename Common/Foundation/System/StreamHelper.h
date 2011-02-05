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

#ifndef MG_STREAM_HELPER_H_
#define MG_STREAM_HELPER_H_

class MgStreamHelper;
template class MG_FOUNDATION_API Ptr<MgStreamHelper>;

/// \cond INTERNAL
///////////////////////////////////////////////////////////////////////////
//  The MgStreamHelper class provides an interface to read and write data
//  from an underlying stream.
class MG_FOUNDATION_API MgStreamHelper : public MgGuardDisposable
{
    ///////////////////////////////////////////////////////////////////////
    ///  Constructors/Destructors
    public:
    MgStreamHelper();
    virtual ~MgStreamHelper();

    ///////////////////////////////////////////////////////////////////////
    ///  Enumerations
    public:

    enum MgStreamStatus
    {
        mssError     = 0x1111F901,
        mssDone      = 0x1111F902,
        mssNotDone   = 0x1111F903
    };

    ///////////////////////////////////////////////////////////////////////
    ///  Methods
    public:

    ////////////////////////////////////
    ////////////////////////////////////
    /// Buffered Reader Methods
    virtual MgStreamStatus GetData( void* buffer, size_t size, bool blocking = true, bool peeking = false ) = 0;

    virtual MgStreamStatus GetUINT8( UINT8& data, bool blocking = true, bool peeking = false ) = 0;

    virtual MgStreamStatus GetUINT16( UINT16& data, bool blocking = true, bool peeking = false ) = 0;

    virtual MgStreamStatus GetUINT32( UINT32& data, bool blocking = true, bool peeking = false ) = 0;

    virtual MgStreamStatus GetINT64( INT64& data, bool blocking = true, bool peeking = false ) = 0;

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
    virtual MgStreamStatus WriteUINT8( UINT8 value ) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Writes a TCP/IP 16 bit integer.  This routine should make our
    /// data transfer platform independent
    ///
    /// \param value
    /// 16 bit integer to write
    ///
    virtual MgStreamStatus WriteUINT16( UINT16 value ) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Writes a TCP/IP 32 bit integer.  This routine should make our
    /// data transfer platform independent
    ///
    /// \param value
    /// 32 bit integer to write
    ///
    virtual MgStreamStatus WriteUINT32( UINT32 value ) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Writes a TCP/IP 64 bit integer.  This routine should make our
    /// data transfer platform independent
    ///
    /// \param value
    /// 64 bit integer to write
    ///
    virtual MgStreamStatus WriteINT64( INT64 value ) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Recursive routine to write an arbitrarily large input buffer to the stream
    /// If the input fits in our internal buffer, just use the internal buffer.
    ///
    virtual MgStreamStatus WriteBytes(const unsigned char* buf, int nBytes) = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Flushes internal memory buffer to stream helper.  This method
    /// should be called once an entire packet has been assembled.
    ///
    virtual MgStreamStatus Flush() = 0;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns whether or not stream is connected.
    ///
    virtual bool IsConnected();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Standard protocol handling for strings, via the UTF-16 standard.
    /// Uses WriteBytes internally.
    ///
    virtual MgStreamStatus WriteString(CREFSTRING str);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Standard protocol handling for strings, via the UTF-16 standard.
    /// Uses GetData internally.
    ///
    virtual MgStreamStatus GetString(REFSTRING str);

    protected:
    virtual void Dispose();
};
/// \endcond

#endif  //  MG_STREAM_HELPER_H_
