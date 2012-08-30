#include "ResourceDefs.h"
#include "ByteSourceDwfInputStreamImpl.h"

///----------------------------------------------------------------------------
/// <summary>
/// Constructs an MgdByteSourceDwfInputStreamImpl object.
/// </summary>
///
/// <param name="inputStream">
/// DWFInputStream object to use.
/// </param>
///----------------------------------------------------------------------------

MgdByteSourceDwfInputStreamImpl::MgdByteSourceDwfInputStreamImpl(
    DWFInputStream* inputStream) :
    m_inputStream(inputStream)
{
    if (NULL == m_inputStream)
    {
        throw new MgNullArgumentException(
            L"MgdByteSourceDwfInputStreamImpl.MgdByteSourceDwfInputStreamImpl",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

///----------------------------------------------------------------------------
/// <summary>
/// Destructs an MgdByteSourceDwfInputStreamImpl object.
/// </summary>
///----------------------------------------------------------------------------

MgdByteSourceDwfInputStreamImpl::~MgdByteSourceDwfInputStreamImpl()
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
/// MgdDwfException
/// </exceptions>
///----------------------------------------------------------------------------

INT64 MgdByteSourceDwfInputStreamImpl::GetLength()
{
    INT64 length = 0;

    MG_RESOURCE_SERVICE_TRY()

    length = m_inputStream->available();

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgdByteSourceDwfInputStreamImpl.GetLength")

    return length;
}

bool MgdByteSourceDwfInputStreamImpl::IsRewindable()
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
/// MgdDwfException
/// </exceptions>
///----------------------------------------------------------------------------

void MgdByteSourceDwfInputStreamImpl::Rewind()
{
    MG_RESOURCE_SERVICE_TRY()

    // Currently DWFUnzippingInputStream throws an DWFNotImplementedException.
    m_inputStream->seek(SEEK_SET, 0);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgdByteSourceDwfInputStreamImpl.Rewind")
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
/// MgdDwfException
/// </exceptions>
///----------------------------------------------------------------------------

INT32 MgdByteSourceDwfInputStreamImpl::Read(BYTE_ARRAY_OUT buffer, INT32 length)
{
    INT32 nBytesRead = 0;

    MG_RESOURCE_SERVICE_TRY()

    nBytesRead = (INT32)m_inputStream->read(buffer, length);

    MG_RESOURCE_SERVICE_CATCH_AND_THROW(L"MgdByteSourceDwfInputStreamImpl.Read")

    return nBytesRead;
}
