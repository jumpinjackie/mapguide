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

//////////////////////////////////////////////////////////////////
///<summary>
/// Default constructor for ByteSource object. Needed for Linux build
///</summary>
MgByteSource::MgByteSource() :
    m_sourceImpl( NULL)
{
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct a MgByteSource object from the specific file
///</summary>
///<param name="file">
/// Name of the file.
///</param>
/// EXCEPTIONS:
///   MgFileNotFoundException, MgFileIoException, MgOutOfMemoryException
///
MgByteSource::MgByteSource(CREFSTRING file)
{
    if ((m_sourceImpl = new ByteSourceFileImpl(file)) == NULL)
    {
        throw new MgOutOfMemoryException(L"MgByteSource.MgByteSource",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

MgByteSource::MgByteSource(CREFSTRING file, bool temporary)
{
    if ((m_sourceImpl = new ByteSourceFileImpl(file, temporary)) == NULL)
    {
        throw new MgOutOfMemoryException(L"MgByteSource.MgByteSource",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct a ByteSource object from an array of bytes
///</summary>
///<param name="stream">
/// Stream content
///</param>
///<param name="length">
/// Length of the stream
///</param>
///
MgByteSource::MgByteSource(BYTE_ARRAY_IN stream, INT32 length)
{
    if ((m_sourceImpl = new ByteSourceMemoryImpl(stream, length)) == NULL)
    {
        throw new MgOutOfMemoryException(L"MgByteSource.MgByteSource",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Destruct a MgByteSource object
///</summary>
///
MgByteSource::~MgByteSource()
{
    delete m_sourceImpl;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgByteSource::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////
///<summary>
/// Dispose this object.
///</summary>
void MgByteSource::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Gets the mime type of the data in this reader
///</summary>
/// <returns>string representing the type, for example text/xml</returns>
///
STRING MgByteSource::GetMimeType()
{
    return m_mimeType;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Sets the mime type of the data
///</summary>
///<param name="mimeType">
///Mime type for byte source
///</param>
/// <returns>string representing the type, for example text/xml</returns>
///
void MgByteSource::SetMimeType(CREFSTRING mimeType)
{
    m_mimeType = mimeType;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Gets a reader for this byte source
///</summary>
///<returns>A ByteReader object</returns>
MgByteReader* MgByteSource::GetReader()
{
    return new MgByteReader(SAFE_ADDREF(this));
}

MgByteSource::MgByteSource(MgByte* bytes)
{
    if ((m_sourceImpl = new ByteSourceMemoryImpl(bytes)) == NULL)
    {
        throw new MgOutOfMemoryException(L"MgByteSource.MgByteSource",
            __LINE__, __WFILE__, NULL, L"", NULL);
    }
}

///////////////////////////////////////////////////////////////////////////////
/// <summary>
/// Constructs an MgByteSource object from ByteSourceImpl. The reader pulls
/// data directly from ByteSourceImpl and reference counts it accordingly.
/// </summary>
///
/// <param name="byteSourceImpl">
/// ByteSourceImpl object to use.
/// </param>
///
MgByteSource::MgByteSource(ByteSourceImpl* byteSourceImpl) :
    m_sourceImpl(byteSourceImpl)
{
    CHECKARGUMENTNULL(m_sourceImpl, L"MgByteSource.MgByteSource");
}

/////////////////////////////////////////////////////////////////
///<summary>
/// Get the underlying data source
///<returns>Information about the underlying data source</returns>
///
ByteSourceImpl* MgByteSource::GetSourceImpl()
{
    return m_sourceImpl;
}
