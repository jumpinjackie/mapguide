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
#include <typeinfo>

//////////////////////////////////////////////////////////////////
///<summary>
/// Default Constructor
///</summary>
///
MgByteSink::MgByteSink()
{
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct a MgByteSource object from a source byte reader
///</summary>
///<param name="reader">MgByteReader object</param>
///
MgByteSink::MgByteSink(MgByteReader *reader)
{
    m_reader = reader;
    SAFE_ADDREF(m_reader);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Destruct a MgByteSource object
///</summary>
///
MgByteSink::~MgByteSink()
{
    SAFE_RELEASE(m_reader);
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Sends the content of the byte reader to a string.
/// </summary>
/// <param name="strDest">Destination string</param>
///
void MgByteSink::ToString(REFSTRING strDest)
{
    string strSource;

    ToStringUtf8(strSource);

    MgUtil::MultiByteToWideChar(strSource, strDest);
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Sends the content of the byte reader to a UTF-8 string.
/// </summary>
/// <param name="strDest">Destination string</param>
///
void MgByteSink::ToStringUtf8(string& strDest)
{
    MG_TRY()

    CHECKARGUMENTNULL(m_reader, L"MgByteSink.ToStringUtf8");

    STRING mimeType = m_reader->GetMimeType();

    if (!mimeType.empty()
        && mimeType.find(L"text") == STRING::npos
        && MgMimeType::Binary != mimeType)
    {
        // Note that the mime type of resource data is determined by the
        // extension of their names. If the extension is unknown or
        // unspecified, then the mime type is set to "application/octet-stream"
        // by default. Therefore, we need to include checking for the
        // MgMimeType::Binary because the resource data in this case could be
        // in either text or binary format.

        MgStringCollection arguments;
        arguments.Add(mimeType);

        throw new MgInvalidMimeTypeException(L"MgByteSink.ToStringUtf8",
            __LINE__,__WFILE__, &arguments, L"", NULL);
    }

    unsigned char buf[2048];
    INT32 len;

    strDest.clear();

    while ((len = m_reader->Read(buf, 2047)) > 0)
    {
        buf[len] = '\0';
        strDest.append((char*)buf);
    }

    MG_CATCH_AND_THROW(L"MgByteSink.ToStringUtf8");
}

///<summary>
/// Send the content of the byte reader to a socket stream
///</summary>
///<param name="stream">Destination ACE stream</param>
///
void MgByteSink::ToStream(ACE_SOCK_Stream* stream)
{
    unsigned char bytes[4096];

    size_t bytesReceived, bytesSent;

    while((bytesReceived = (size_t)m_reader->Read(bytes, 4096)) > 0)
    {
        if ((bytesSent = stream->send_n(bytes, bytesReceived, MG_MSG_NOSIGNAL)) != bytesReceived)
        {
            //should get the error from ACE and put it in the exception
            throw new MgStreamIoException(L"MgByteSink.ToStream",
                __LINE__, __WFILE__, NULL, L"", NULL);
        }
    }

}

/// <summary>
/// Sends the content of the byte reader to an MgByte object.
/// </summary>
/// <returns>
/// MgByte object.
/// </returns>
///
MgByte* MgByteSink::ToBuffer()
{
    CHECKNULL(m_reader, L"MgByteSink.ToBuffer");

    Ptr<MgByte> bytes;

    // See if we are sourced from an MgByte.  If we are, just pass it back.
    ByteSourceMemoryImpl* memImpl = dynamic_cast<ByteSourceMemoryImpl*>(m_reader->m_byteSource->GetSourceImpl());
    if (memImpl != NULL)
    {
        bytes = memImpl->Bytes();
    }
    else
    {
        bytes = new MgByte(NULL, (INT32)m_reader->GetLength());
        char temp[4096];
        temp[0] = 0;
        INT32 dataRead = 0;

        do
        {
            // Read the data
            dataRead = m_reader->Read((BYTE_ARRAY_OUT)(&temp[0]), 4096);
            bytes->Append((BYTE_ARRAY_IN)temp, dataRead);

        } while ( dataRead > 0 );
    }

    return SAFE_ADDREF((MgByte*)bytes);
}

///<summary>
/// Send the content of the byte reader to a file
///</summary>
///<param name="filename">Destination file name</param>
///
void MgByteSink::ToFile(CREFSTRING filename)
{
    // assigned inside try-catch block
    unsigned char* bytes = NULL;
    FILE* f = NULL;

    MG_TRY()

    // See if we are sourced from a temporary file byte source.  If we are, just
    // rename the file.  This should be a significant performance optimization for
    // large file transfers
    ByteSourceFileImpl* fileImpl = dynamic_cast<ByteSourceFileImpl*>(m_reader->m_byteSource->GetSourceImpl());
    if (fileImpl != NULL && fileImpl->IsTemporary())
    {
        fileImpl->Rename(filename);
    }
    else
    {
        // Do it the hard way.  Create a new file and write it a block at a time.
        // Use ACE_OS::fopen() for Linux compatibility
        f = ACE_OS::fopen(MG_WCHAR_TO_TCHAR(filename), ACE_TEXT("wb"));

        if (f == NULL)
        {
            MgStringCollection arguments;
            arguments.Add(filename);
            if (errno == EEXIST)
            {
                MG_FILE_IO_EXCEPTION_ADD_ERRNO();
                throw new MgFileNotFoundException(L"MgByteSink.ToFile",
                    __LINE__, __WFILE__, &arguments, L"MgFileIoErrNo", &errNoArgs);
            }
            else
            {
                MG_FILE_IO_EXCEPTION_ADD_ERRNO();
                throw new MgFileIoException(L"MgByteSink.ToFile",
                    __LINE__, __WFILE__, &arguments, L"MgFileIoErrNo", &errNoArgs);
            }
        }

        // allocate a huge buffer (usually 1MB) at first
        int bSinkBufferSize = BSINK_BUFFER_SIZE;
        do
        {
            // retry allocation with half the size until we give up at 16kB
            try
            {
                bytes = new unsigned char[bSinkBufferSize];
            }
            catch (exception& e)
            {
                if (typeid(e) == typeid(bad_alloc) && bSinkBufferSize > 1024*16)
                    bSinkBufferSize >>= 1;
                else
                    throw e;
            }
        }
        while (bytes == NULL);

        size_t bytesReceived, bytesWritten;

        while ((bytesReceived = (size_t)m_reader->Read(bytes, bSinkBufferSize)) > 0)
        {
            if((bytesWritten = ACE_OS::fwrite(bytes, 1, bytesReceived, f)) != bytesReceived)
            {
                ACE_OS::fclose(f);

                MgStringCollection arguments;
                arguments.Add(filename);

                throw new MgFileIoException(L"MgByteSink.ToFile",
                    __LINE__, __WFILE__, &arguments, L"", NULL);
            }
        }
    }

    MG_CATCH(L"MgByteSink.ToFile")

    delete [] bytes;

    if (NULL != f)
    {
        // close the file if one was created
        ACE_OS::fclose(f);

        // also delete the file if there was an exception - this is safe
        // because we know the new file was created (i.e. we're not deleting
        // a pre-existing file)
        if (NULL != mgException)
            MgFileUtil::DeleteFile(filename);
    }

    MG_THROW()
}
