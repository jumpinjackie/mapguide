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

#include "ResourceServiceDefs.h"
#include "ByteSourceDwfInputStreamImpl.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs an MgByteSourceDwfInputStreamImpl object.
/// </summary>
///
/// <param name="inputStream">
/// DWFInputStream object to use.
/// </param>
///----------------------------------------------------------------------------

MgByteSourceDwfInputStreamImpl::MgByteSourceDwfInputStreamImpl(
    DWFInputStream* inputStream) :
    m_inputStream(inputStream)
{
    if (NULL == m_inputStream)
    {
        throw new MgNullArgumentException(
            L"MgByteSourceDwfInputStreamImpl.MgByteSourceDwfInputStreamImpl",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs an MgByteSourceDwfInputStreamImpl object.
/// </summary>
///----------------------------------------------------------------------------

MgByteSourceDwfInputStreamImpl::~MgByteSourceDwfInputStreamImpl()
{
    DWFCORE_FREE_OBJECT(m_inputStream);
}

///----------------------------------------------------------------------------
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
/// <exceptions>
/// MgDwfException
/// </exceptions>
///----------------------------------------------------------------------------

INT64 MgByteSourceDwfInputStreamImpl::GetLength()
{
    INT64 length = 0;

    MG_RESOURCE_SERVICE_TRY()

    length = m_inputStream->available();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgByteSourceDwfInputStreamImpl.GetLength")

    return length;
}

bool MgByteSourceDwfInputStreamImpl::IsRewindable()
{
    // Currently DWFUnzippingInputStream does not support rewinding.
    return false;
}

///----------------------------------------------------------------------------
/// <summary>
/// Rewinds to the start of memory.  Length is reset.
/// </summary>
///
/// <returns>
/// Nothing.
/// </returns>
///
/// <exceptions>
/// MgDwfException
/// </exceptions>
///----------------------------------------------------------------------------

void MgByteSourceDwfInputStreamImpl::Rewind()
{
    MG_RESOURCE_SERVICE_TRY()

    // Currently DWFUnzippingInputStream throws an DWFNotImplementedException.
    m_inputStream->seek(SEEK_SET, 0);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgByteSourceDwfInputStreamImpl.Rewind")
}

///----------------------------------------------------------------------------
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
/// <exceptions>
/// MgDwfException
/// </exceptions>
///----------------------------------------------------------------------------

INT32 MgByteSourceDwfInputStreamImpl::Read(BYTE_ARRAY_OUT buffer, INT32 length)
{
    INT32 nBytesRead = 0;

    MG_RESOURCE_SERVICE_TRY()

    nBytesRead = (INT32)m_inputStream->read(buffer, length);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgByteSourceDwfInputStreamImpl.Read")

    return nBytesRead;
}
