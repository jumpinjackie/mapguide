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

#include "Foundation.h"
#include "AceStreamHelper.h"

/////////////////////////////////////////
/// Globals

INT32 MgAceStreamHelper::m_tcpPacketSize = 0;

//-------------------------------------------------------------------------
//  Constructors/Destructors
//-------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  Constructs a MgAceStreamHelper object with the given handle.
//  </summary>
//
//  <param name = "handle">
//  The ACE_HANDLE that this MgAceStreamHelper will be reading and writing.
//  </param>
MgAceStreamHelper::MgAceStreamHelper( ACE_HANDLE handle )
: MgStreamHelper(),
  m_readBufSize(0),
  m_readBuffer(NULL),
  m_readBufStart(0),
  m_readBufEnd(0),
  m_writeBufSize(0),
  m_writeBuffer(NULL),
  m_writeBufLength(0),
  m_handle(handle)
{
ACE_ASSERT( handle != 0 );
    if (m_tcpPacketSize == 0)
    {
        m_tcpPacketSize = 1460;

        MgConfiguration* config = MgConfiguration::GetInstance();

        config->GetIntValue(MgFoundationConfigProperties::GeneralPropertiesSection,
                MgFoundationConfigProperties::GeneralPropertyTcpIpMtu,
                m_tcpPacketSize,
                MgFoundationConfigProperties::DefaultGeneralPropertyTcpIpMtu);

        // MTU is a key parameter in our transmission scheme, make sure
        // the value specified is reasonable.  If it isn't, default to
        // something we know is reasonable.

        if (m_tcpPacketSize < 40 || m_tcpPacketSize > 65000)
        {
            m_tcpPacketSize = MgFoundationConfigProperties::DefaultGeneralPropertyTcpIpMtu;
        }
    }

    // read buffer
    m_readBufSize =  m_tcpPacketSize*10;
    m_readBuffer = new UINT8[m_readBufSize];
    m_readBufStart = 0;
    m_readBufEnd = 0;

    // write buffer
    m_writeBufSize = m_tcpPacketSize*5;
    m_writeBuffer = new UINT8[m_writeBufSize];
    m_writeBufLength = 0;
};

MgAceStreamHelper::MgAceStreamHelper()
: MgStreamHelper(),
  m_readBufSize(0),
  m_readBuffer(NULL),
  m_readBufStart(0),
  m_readBufEnd(0),
  m_writeBufSize(0),
  m_writeBuffer(NULL),
  m_writeBufLength(0),
  m_handle(ACE_INVALID_HANDLE)
{
}

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  The destructor for a MgAceStreamHelper object.
//  </summary>
MgAceStreamHelper::~MgAceStreamHelper()
{
    delete[] m_readBuffer;
    m_readBuffer = NULL;
    m_readBufStart = 0;
    m_readBufEnd = 0;
    m_readBufSize = 0;

    delete [] m_writeBuffer;
    m_writeBuffer = NULL;
    m_writeBufLength = 0;
    m_writeBufSize = 0;
};

//-------------------------------------------------------------------------
//  Public Methods
//-------------------------------------------------------------------------

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  This method reads data from the stream into the given buffer.  The
//  parameters control whether or not the read blocks, whether or not,
//  the caller wants to peek at the data, and if the caller wants to
//  know how many bytes had been read.
//  </summary>
//
//  <param name = "buffer">
//  The buffer that will receive the data read from the stream.
//  </param>
//
//  <param name = "size">
//  The size of the buffer in bytes of the buffer.
//  </param>
//
//  <param name = "blocking">
//  True if the read request should block;  false otherwise.
//  </param>
//
//  <param name = "peeking">
//  True if the read request should not consume the data;  false
//  otherwise.
//  </param>
//
//  <param name = "bytesAvailable">
//  An out parameter that will contain the number of bytes that have been
//  read from the stream.  It will contain -1 if there was an error.
//  </param>
//
//  <returns>
//  Returns a MgStreamStatus value indicating the status of the operation.
//  </returns>
MgStreamHelper::MgStreamStatus MgAceStreamHelper::GetData(void* buffer,
     size_t size, bool blocking, bool peeking)
{
    // Do not attempt reading zero byte from the socket as this could be problematic.
    if (0 == size)
    {
        return MgStreamHelper::mssDone;
    }

    ACE_ASSERT( size > 0 );
    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssError;

    // Is our internal buffer big enough?  If not, expand it.
    if ( m_readBufSize < size )
    {
        m_readBufSize = size;
        UINT8* temp = new UINT8[m_readBufSize];
        memcpy( temp, &m_readBuffer[m_readBufStart], m_readBufEnd - m_readBufStart );
        delete[] m_readBuffer;
        m_readBuffer = temp;
        m_readBufEnd -= m_readBufStart;
        m_readBufStart = 0;
    }

    //  check if requested data is already in buffer
    stat = UpdateReadBuffers( buffer, size, peeking );

    if ( MgStreamHelper::mssDone != stat )
    {
        // We do not have enough data and will need to read.  Shift buffer back
        // and attempt to fill the entire buffer with a non-blocking read first
        memmove(m_readBuffer, &m_readBuffer[m_readBufStart], m_readBufEnd-m_readBufStart);
        m_readBufEnd -= m_readBufStart;
        m_readBufStart = 0;

        ACE_SOCK_Stream stream;
        stream.set_handle( m_handle );

        // Windows has a timing problem. If trying to receive data too fast,
        // it will fail. So, use a timeout to let it catch up.
        // This workaround reduces the number of lockups significantly and
        // eliminates the hanging problem when it takes so long to write a
        // request to the socket.
        const ACE_Time_Value timeout(60);

        // On Linux, use MSG_NOSIGNAL to turn off raising of SIGPIPE on
        // stream sockets when the other end disappears.
        // Note that neither trapping the SIGPIPE signal via an
        // ACE_Event_Handler nor calling ACE_OS::signal(SIGPIPE, SIG_IGN)
        // seems to work.
        ssize_t res = stream.recv(&m_readBuffer[m_readBufEnd], m_readBufSize - m_readBufEnd, MG_MSG_NOSIGNAL, &timeout);

        if ( res < 0 )
        {
            // Check this return value to determine if the socket is closed or
            // if there was simply no data.
#ifdef _WIN32
            int error = ::WSAGetLastError(); // errno doesn't work correctly on Windows
            bool bConnected = ( error == WSAEWOULDBLOCK || error == 0 );
#else
            bool bConnected = ( errno == EWOULDBLOCK || errno == 0 || errno == ETIME );
#endif

            stat = ( bConnected ) ? MgStreamHelper::mssNotDone : MgStreamHelper::mssError;
        }
        else if (res == 0)
        {
            // No longer connected
            stat = MgStreamHelper::mssError;
        }
        else
        {
            m_readBufEnd += res;
            // Now do we have enough data?
            stat = UpdateReadBuffers( buffer, size, peeking );
        }

        if (MgStreamHelper::mssNotDone == stat && blocking)
        {
            // Still not enough data.  Have to block and fill only what was asked for.
            res = stream.recv_n(&m_readBuffer[m_readBufEnd], size - (m_readBufEnd-m_readBufStart), MG_MSG_NOSIGNAL);

            if ( res < 0 )
            {
                // Check this return value to determine if the socket is closed or
                // if there was simply no data.
#ifdef _WIN32
                int error = ::WSAGetLastError(); // errno doesn't work correctly on Windows
                bool bConnected = ( error == WSAEWOULDBLOCK || error == 0 );
#else
                bool bConnected = ( errno == EWOULDBLOCK || errno == 0 || errno == ETIME );
#endif

                stat = ( bConnected ) ? MgStreamHelper::mssNotDone : MgStreamHelper::mssError;
            }
            else if (res == 0)
            {
                // No longer connected
                stat = MgStreamHelper::mssError;
            }
            else
            {
                m_readBufEnd += res;
                // Now we better have enough data...
                stat = UpdateReadBuffers( buffer, size, peeking );
            }
        }
    }

    return stat;
}

MgStreamHelper::MgStreamStatus MgAceStreamHelper::UpdateReadBuffers( void* buffer, size_t size, bool peeking)
{
    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssNotDone;

    if ((m_readBufEnd - m_readBufStart) >= size)
    {
        //  update output data
        memcpy( buffer, &m_readBuffer[m_readBufStart], size );

        if ( !peeking )
        {
            m_readBufStart += size;
            if (m_readBufStart == m_readBufEnd)
            {
                // Empty buffer, reset
                m_readBufStart = 0;
                m_readBufEnd = 0;
            }
        }

        stat = MgStreamHelper::mssDone;
    }
    return stat;
}


///////////////////////////////////////////////////////////////////////////
//  <summary>
//  Reads a UINT8 from the stream.
//  </summary>
//
//  <param name = "data">
//  An out parameter that holds the UINT8 read from the stream.
//  </param>
//
//  <param name = "blocking">
//  True if the read request should block;  false otherwise.
//  </param>
//
//  <param name = "peeking">
//  True if the read request should not consume the data;  false
//  otherwise.
//  </param>
//
//  <returns>
//  Returns a MgStreamStatus value indicating the status of the operation.
//  </returns>
MgStreamHelper::MgStreamStatus MgAceStreamHelper::GetUINT8( UINT8& data, bool blocking, bool peeking )
{
    return GetData( &data, sizeof(data), blocking, peeking );
};

MgStreamHelper::MgStreamStatus MgAceStreamHelper::GetUINT16( UINT16& data, bool blocking, bool peeking )
{
    INT16 nValue = 0;

    MgStreamHelper::MgStreamStatus stat = GetData( &nValue, sizeof(INT16), blocking, peeking );

    data = MG_NTOHS(nValue);

    return stat;
}

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  Reads a UINT32 from the stream.
//  </summary>
//
//  <param name = "data">
//  An out parameters that holds the UINT32 read from the stream.
//  </param>
//
//  <param name = "blocking">
//  True if the read request should block;  false otherwise.
//  </param>
//
//  <param name = "peeking">
//  True if the read request should not consume the data;  false
//  otherwise.
//  </param>
//
//  <returns>
//  Returns a MgStreamStatus value indicating the status of the operation.
//  </returns>
MgStreamHelper::MgStreamStatus MgAceStreamHelper::GetUINT32( UINT32& data, bool blocking, bool peeking )
{
    UINT32 intData = 0;
    MgStreamHelper::MgStreamStatus stat = GetData( &intData, sizeof(intData), blocking, peeking );
    data = MG_NTOHL(intData);
    return stat;
};

MgStreamHelper::MgStreamStatus MgAceStreamHelper::GetINT64( INT64& data, bool blocking, bool peeking )
{
    data = 0;

    UINT32 lowWord;
    UINT32 hiWord;

    MgStreamHelper::MgStreamStatus stat = GetUINT32(lowWord, blocking, peeking);
    if (MgStreamHelper::mssDone == stat) { stat = GetUINT32(hiWord, blocking, peeking); }

    if (MgStreamHelper::mssDone == stat) { data = (((INT64)hiWord) << 32) | lowWord; }

    return stat;
}


///////////////////////////////////////////////////////////////////////////
//  <summary>
//  This method writes data from the given buffer to the underlying stream.
//  It can block or not, depending on the value of the blocking parameter.
//  </summary>
//
//  <param name = "buffer">
//  The buffer that contains the data to be written.
//  </param>
//
//  <param name = "size">
//  The size of the buffer in bytes of the buffer.
//  </param>
//
//  <param name = "blocking">
//  True if the write request should block;  false otherwise.
//  </param>
//
//  <param name = "bytesWritten">
//  An out parameter that will contain the number of bytes that have been
//  written to the stream.
//  </param>
//
//  <returns>
//  Returns a MgStreamStatus value indicating the status of the operation.
//  </returns>
MgStreamHelper::MgStreamStatus MgAceStreamHelper::WriteData(void* buffer,
    size_t size, bool blocking, size_t* bytesWritten)
{
    // Do not attempt writing zero byte to the socket as this could be problematic.
    if (0 == size)
    {
        return MgStreamHelper::mssDone;
    }

    ACE_ASSERT( buffer && size > 0 );
    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssError;

    //  check parameters
    if ( buffer && size > 0 )
    {
        //  init out parameter
        if ( bytesWritten != NULL )
            *bytesWritten = 0;

        ACE_SOCK_Stream stream;
        stream.set_handle( m_handle );

        ssize_t res = 0;

        // On Linux, use MSG_NOSIGNAL to request not to send SIGPIPE on
        // errors on stream oriented sockets when the other end breaks
        // the connection. The EPIPE error is still returned.
        // Note that neither trapping the SIGPIPE signal via an
        // ACE_Event_Handler nor calling ACE_OS::signal(SIGPIPE, SIG_IGN)
        // seems to work.

        if ( blocking )
        {
            res = stream.send_n(buffer, size, MG_MSG_NOSIGNAL);
        }
        else
        {
            res = stream.send(buffer, size, MG_MSG_NOSIGNAL);
        }

        //  check for failure
        if ( res >= 0 )
        {
            //  update out parameter
            if ( bytesWritten != NULL )
                *bytesWritten = res;

            if ( res == (ssize_t)size )
            {
                stat = MgStreamHelper::mssDone;
            }
            else
            {
                stat = blocking ? MgStreamHelper::mssError : MgStreamHelper::mssNotDone;
            }
        }
    }

    return stat;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Recursive routine to write an arbitrarily large input buffer to the stream
/// If the input fits in our internal buffer, just use the internal buffer.
///</summary>
MgStreamHelper::MgStreamStatus MgAceStreamHelper::WriteBytes(const unsigned char* buf, int nBytes)
{
    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssDone;

    if (nBytes + m_writeBufLength >= m_writeBufSize)
    {
        // We cannot fit everything in the buffer.
        // Fill the buffer completely and write it to the helper.
        size_t fillBytes = m_writeBufSize - m_writeBufLength;
        memcpy(&m_writeBuffer[m_writeBufLength], buf, fillBytes);
        m_writeBufLength += fillBytes;

        size_t bytesWritten = 0;

        stat = WriteData(m_writeBuffer, m_writeBufLength, true, &bytesWritten);

        // shift output buffer back
        if (bytesWritten > 0)
        {
            memmove(m_writeBuffer, &m_writeBuffer[bytesWritten], m_writeBufLength-bytesWritten);
            m_writeBufLength -= bytesWritten;
        }

        // and recurse to push more data in
        if (MgStreamHelper::mssError != stat)
        {
            stat = WriteBytes(&buf[fillBytes], nBytes-(int)fillBytes);
        }
    }
    else
    {
        // We have enough room in our internal buffer so just append
        memcpy(&m_writeBuffer[m_writeBufLength], buf, nBytes);
        m_writeBufLength += nBytes;
    }

    return stat;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Flushes internal memory buffer to stream helper.  This method
/// should be called once an entire packet has been assembled.
///</summary>
MgStreamHelper::MgStreamStatus MgAceStreamHelper::Flush()
{
    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssNotDone;
    while (m_writeBufLength > 0 && MgStreamHelper::mssError != stat)
    {
        size_t bytesWritten = 0;
        stat = WriteData(m_writeBuffer, m_writeBufLength, true, &bytesWritten);

        // shift output buffer back
        if (bytesWritten > 0)
        {
            memmove(m_writeBuffer, &m_writeBuffer[bytesWritten], m_writeBufLength-bytesWritten);
            m_writeBufLength -= bytesWritten;
        }
    }
    return stat;
}

MgStreamHelper::MgStreamStatus MgAceStreamHelper::WriteUINT8( UINT8 value )
{
    return WriteBytes((unsigned char*)&value, sizeof(UINT8));
}

MgStreamHelper::MgStreamStatus MgAceStreamHelper::WriteUINT32( UINT32 value )
{
    UINT32 tcpValue = MG_HTONL(value);
    return WriteBytes((unsigned char*)&tcpValue, sizeof(UINT32));
}


MgStreamHelper::MgStreamStatus MgAceStreamHelper::WriteUINT16( UINT16 value )
{
    UINT16 tcpValue = MG_HTONS(value);
    return WriteBytes((unsigned char*)&tcpValue, sizeof(UINT16));
}

MgStreamHelper::MgStreamStatus MgAceStreamHelper::WriteINT64( INT64 value )
{
    UINT32 word = (UINT32) (value & 0xFFFFFFFF);
    MgStreamHelper::MgStreamStatus stat = WriteUINT32(word);

    word = (UINT32)((value >> 32) & 0xFFFFFFFF);
    if (MgStreamHelper::mssDone == stat) stat = WriteUINT32(word);

    return stat;
}

void MgAceStreamHelper::Dispose()
{
    delete this;
}

bool MgAceStreamHelper::IsConnected()
{
    bool bConnected = true;
    ACE_SOCK_Stream stream;
    stream.set_handle( m_handle );
    UINT8 dummy;
    ACE_Time_Value val(0, 0);
    ssize_t res = stream.recv_n(&dummy, 1, MSG_PEEK | MG_MSG_NOSIGNAL, &val);

    if ( res < 0 )
    {
        // Error or timeout occured
#ifdef _WIN32
        int error = ::WSAGetLastError(); // errno doesn't work correctly on Windows
        bConnected = ( error == WSAEWOULDBLOCK || error == 0 );
#else
        bConnected = ( errno == EWOULDBLOCK || errno == 0 || errno == ETIME );
#endif
    }
    else if (res == 0)
    {
        // No longer connected
        bConnected = false;
    }

    return bConnected;
}
