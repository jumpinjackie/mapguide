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
#include "MemoryStreamHelper.h"
#include "Base64.h"

INT32 MgMemoryStreamHelper::m_blockSize = 4096;

//-------------------------------------------------------------------------
//  Constructors/Destructors
//-------------------------------------------------------------------------

MgMemoryStreamHelper::MgMemoryStreamHelper() : MgStreamHelper()
{
    m_curPtrRead = m_curPtrWrite = m_buffer = new INT8[m_blockSize];
    if(m_buffer == NULL)
        throw new MgOutOfMemoryException(L"MgMemoryStreamHelper.MgMemoryStreamHelper", __LINE__, __WFILE__, NULL, L"", NULL);

    m_endBuffer = m_buffer + m_blockSize;
    m_blockCount = 1;
    m_ownMemory = true;
}

MgMemoryStreamHelper::MgMemoryStreamHelper(INT8* buffer, INT32 length, bool ownMemory)
{
    m_curPtrRead = m_buffer = buffer;
    m_curPtrWrite = m_endBuffer = buffer + length;
    m_ownMemory = ownMemory;
}

///////////////////////////////////////////////////////////////////////////
//  <summary>
//  The destructor for a MgMemoryStreamHelper object.
//  </summary>
MgMemoryStreamHelper::~MgMemoryStreamHelper()
{
    if(m_ownMemory)
        delete [] m_buffer;
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
//  Ignored. A memory stream never blocks.
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
MgStreamHelper::MgStreamStatus MgMemoryStreamHelper::GetData(void* buffer, size_t size,
    bool blocking, bool peeking)
{
    ACE_ASSERT( size > 0 );

    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssError;

    size_t available = m_endBuffer - m_curPtrRead;
    if(available < size)
        stat = MgStreamHelper::mssError;
    else
    {
        stat = MgStreamHelper::mssDone;
        memcpy(buffer, m_curPtrRead, size);
        if(!peeking)
            m_curPtrRead += size;
    }

    return stat;
};

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
//  Ignored. A memory stream never blocks.
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
MgStreamHelper::MgStreamStatus MgMemoryStreamHelper::GetUINT8(UINT8& data, bool blocking, bool peeking)
{
    return GetData(&data, sizeof(INT8), blocking, peeking);
};

MgStreamHelper::MgStreamStatus MgMemoryStreamHelper::GetUINT16(UINT16& data, bool blocking, bool peeking)
{
    INT16 nValue = 0;

    MgStreamHelper::MgStreamStatus stat = GetData(&nValue, sizeof(INT16), blocking, peeking);
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
//  Ignored. A memory stream never blocks.
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
MgStreamHelper::MgStreamStatus MgMemoryStreamHelper::GetUINT32(UINT32& data, bool blocking, bool peeking)
{
    UINT32 intData = 0;

    MgStreamHelper::MgStreamStatus stat = GetData(&intData, sizeof(intData), blocking, peeking);
    data = MG_NTOHL(intData);

    return stat;
};

MgStreamHelper::MgStreamStatus MgMemoryStreamHelper::GetINT64(INT64& data, bool blocking, bool peeking)
{
    UINT32 lowWord;
    UINT32 hiWord;

    MgStreamHelper::MgStreamStatus stat = GetUINT32(lowWord, blocking, peeking);
    if(stat == MgStreamHelper::mssDone)
        if((stat = GetUINT32(hiWord, blocking, peeking)) == MgStreamHelper::mssDone)
            data = ((UINT64)hiWord << 32) | lowWord;

    return stat;
}

MgStreamHelper::MgStreamStatus MgMemoryStreamHelper::GetNullTermString( REFSTRING data, bool blocking, bool peeking)
{
    char buf[1024];
    int len = 1024;
    if (GetLength() < len)
    {
        len = GetLength();
    }

    MgStreamHelper::MgStreamStatus stat = GetData(buf, len, true, true);
    if(stat == MgStreamHelper::mssDone)
    {
        char* endBuf = strchr(buf, '\0');
        if (endBuf != buf)
        {
            string mbBuf = buf;
            data = MgUtil::MultiByteToWideChar(mbBuf);
            stat = GetData(buf, endBuf-buf+1, true, peeking);
        }
    }

    return stat;
}

MgStreamHelper::MgStreamStatus MgMemoryStreamHelper::GetSingle(float& data, bool blocking, bool peeking)
{
    float fData = 0.0;

    MgStreamHelper::MgStreamStatus stat = GetData(&fData, sizeof(fData), blocking, peeking);
    data = (float)MG_NTOHL(fData);

    return stat;
};

MgStreamHelper::MgStreamStatus MgMemoryStreamHelper::GetDouble(double& data, bool blocking, bool peeking)
{
    INT64 intData = 0;

    MgStreamHelper::MgStreamStatus stat = GetINT64(intData, blocking, peeking);
    memcpy((void*)&data, (void*)&intData, sizeof(double));

    return stat;
};

//////////////////////////////////////////////////////////////////
///<summary>
/// Write to the memory buffer, making it grow if necessary.
///</summary>
MgStreamHelper::MgStreamStatus MgMemoryStreamHelper::WriteBytes(const unsigned char* buf, int nBytes)
{
    MgStreamHelper::MgStreamStatus stat = MgStreamHelper::mssDone;

    //check if the data to be written fits in the existing buffer
    size_t available = m_endBuffer - m_curPtrWrite;
    if(available < (size_t)nBytes)
    {
        //need to make the buffer grow.
        //calculate how much growth is needed
        size_t neededBytes = (m_curPtrWrite + nBytes) - m_endBuffer;
        size_t neededBlocks = neededBytes / m_blockSize;
        neededBlocks += (neededBytes % m_blockSize != 0)? 1: 0;
        int newBlockCount = m_blockCount + (int)neededBlocks;

        //allocate the new block
        INT8* newBlock = new INT8[newBlockCount * m_blockSize];
        if(!newBlock)
            throw new MgOutOfMemoryException(L"MgMemoryStreamHelper.WriteBytes", __LINE__, __WFILE__, NULL, L"", NULL);

        //initialize new block with current stream content
        size_t writePos = m_curPtrWrite - m_buffer;
        memcpy(newBlock, m_buffer, writePos);

        //remember the position of the read ptr in the old buffer
        size_t readPos = m_curPtrRead - m_buffer;

        if (m_ownMemory)
            delete [] m_buffer;

        //new block becomes this stream's memory
        m_curPtrWrite = (m_buffer = newBlock) + writePos;
        m_curPtrRead = m_buffer + readPos;
        m_endBuffer = m_buffer + newBlockCount * m_blockSize;
        m_blockCount = newBlockCount;
    }

    //copy bytes into stream
    memcpy(m_curPtrWrite, buf, nBytes);
    m_curPtrWrite += nBytes;

    return stat;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Flushes internal memory buffer to stream helper.
/// This his not applicable to a memory stream
///</summary>
MgStreamHelper::MgStreamStatus MgMemoryStreamHelper::Flush()
{
    return MgStreamHelper::mssDone;
}

MgStreamHelper::MgStreamStatus MgMemoryStreamHelper::WriteUINT8( UINT8 value )
{
    return WriteBytes((unsigned char*)&value, sizeof(UINT8));
}

MgStreamHelper::MgStreamStatus MgMemoryStreamHelper::WriteUINT32( UINT32 value )
{
    UINT32 tcpValue = MG_HTONL(value);
    return WriteBytes((unsigned char*)&tcpValue, sizeof(UINT32));
}


MgStreamHelper::MgStreamStatus MgMemoryStreamHelper::WriteUINT16( UINT16 value )
{
    UINT16 tcpValue = MG_HTONS(value);
    return WriteBytes((unsigned char*)&tcpValue, sizeof(UINT16));
}

MgStreamHelper::MgStreamStatus MgMemoryStreamHelper::WriteINT64( INT64 value )
{
    UINT32 word = (UINT32) (value & 0xFFFFFFFF);
    MgStreamHelper::MgStreamStatus stat = WriteUINT32(word);

    word = (UINT32)((value >> 32) & 0xFFFFFFFF);
    if (MgStreamHelper::mssDone == stat) stat = WriteUINT32(word);

    return stat;
}

MgStreamHelper::MgStreamStatus MgMemoryStreamHelper::WriteSingle( float value )
{
    UINT32 word = 0;
    memcpy((void*)&word, (void*)&value, sizeof(float));
    UINT32 tcpValue = MG_HTONL(word);
    return WriteBytes((unsigned char*)&tcpValue, sizeof(UINT32));
}

MgStreamHelper::MgStreamStatus MgMemoryStreamHelper::WriteDouble( double value )
{
    INT64 intValue;
    memcpy((void*)&intValue, (void*)&value, sizeof(double));
    return WriteINT64(intValue);
}

MgStreamHelper::MgStreamStatus MgMemoryStreamHelper::WriteNullTermString( CREFSTRING data )
{
    string mbStr = MgUtil::WideCharToMultiByte(data);
    MgStreamHelper::MgStreamStatus stat =
        WriteBytes((const unsigned char*)mbStr.c_str(),(int)mbStr.length()+1);
    return stat;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Get a pointer to the data in this memory stream
///</summary>
const INT8* MgMemoryStreamHelper::GetBuffer()
{
    return m_buffer;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Get the length of the data in this stream
///</summary>
INT32 MgMemoryStreamHelper::GetLength()
{
    return (INT32)(m_curPtrWrite - m_buffer);
}

void MgMemoryStreamHelper::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Encode stream as Base64
///</summary>
string MgMemoryStreamHelper::ToBase64()
{
    unsigned long len = Base64::GetEncodedLength(GetLength());
    string b64;
    b64.resize(len, '\0');
    Base64::Encode((char*)b64.c_str(), (const unsigned char*)m_buffer, GetLength());
    return b64;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Decode stream from Base64
///</summary>
void MgMemoryStreamHelper::FromBase64(CREFSTRING uniString)
{
    string b64string;
    UnicodeString::WideCharToMultiByte(uniString.c_str(), b64string);

    Clear();
    unsigned long origLen = (unsigned long) b64string.length();
    unsigned long len = Base64::GetDecodedLength(origLen);
    if (len < 512)
    {
        unsigned char buf[512];
        Base64::Decode(buf, b64string.c_str(), origLen);
        WriteBytes(buf, len);
    }
    else
    {
        unsigned char* buf = new unsigned char[len];
        Base64::Decode(buf, b64string.c_str(), origLen);
        WriteBytes(buf, len);
        delete [] buf;
    }
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Clears internal buffer
///</summary>
void MgMemoryStreamHelper::Clear()
{
    m_curPtrWrite = m_curPtrRead = m_buffer;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Rewind to start of buffer
///</summary>
void MgMemoryStreamHelper::Rewind()
{
    m_curPtrRead = m_buffer;
}
