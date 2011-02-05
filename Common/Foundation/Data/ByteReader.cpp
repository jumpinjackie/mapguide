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

MG_IMPL_DYNCREATE(MgByteReader);

//////////////////////////////////////////////////////////////////
/// \brief
/// Creates a byte reader from a file
///
MgByteReader::MgByteReader(CREFSTRING fileName, CREFSTRING mimeType, bool removeFile)
{
    Ptr<MgByteSource> source = new MgByteSource(fileName, removeFile);
    source->SetMimeType(mimeType);
    SetByteSource(SAFE_ADDREF((MgByteSource*)source));
}

//////////////////////////////////////////////////////////////////
/// \brief
/// Creates a byte reader from a string
///
MgByteReader::MgByteReader(CREFSTRING contents, CREFSTRING mimeType)
{
    string utf8 = MgUtil::WideCharToMultiByte(contents);
    Ptr<MgByteSource> source = new MgByteSource((BYTE_ARRAY_IN)utf8.c_str(), (INT32)utf8.length());
    source->SetMimeType(mimeType);
    SetByteSource(SAFE_ADDREF((MgByteSource*)source));
}

//////////////////////////////////////////////////////////////////
/// \brief
/// Creates a byte reader from an array of bytes
///
MgByteReader::MgByteReader(BYTE_ARRAY_IN contents, INT32 length, CREFSTRING mimeType)
{
    Ptr<MgByteSource> source = new MgByteSource(contents, length);
    source->SetMimeType(mimeType);
    SetByteSource(SAFE_ADDREF((MgByteSource*)source));
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct a MgByteReader object from a byte source
///</summary>
///<param name="byteSource">Byte  source object</param>
///
MgByteReader::MgByteReader(MgByteSource* byteSource)
{
    m_byteSource = byteSource;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Construct an uninitialized MgByteReader object
///</summary>
///
MgByteReader::MgByteReader()
{
    m_byteSource = NULL;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Destruct a MgByteReader object
///</summary>
///
MgByteReader::~MgByteReader()
{
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Set the byte source for an empty reader
///</summary>
///<param name="byteSource">Byte source object</param>
///
void MgByteReader::SetByteSource(MgByteSource* byteSource)
{
    m_byteSource = byteSource;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Get the byte source for a reader
///</summary>
///<returns>
///Byte source for reader
///</summary>
///
MgByteSource* MgByteReader::GetByteSource()
{
    return m_byteSource;
}

/////////////////////////////////////////////////////////////////
/// <summary>
/// Returns the classId.
/// </summary>
INT32 MgByteReader::GetClassId()
{
    return m_cls_id;
}

//////////////////////////////////////////////
///<summary>
/// Dispose this object.
///</summary>
void MgByteReader::Dispose()
{
    delete this;
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Reads a buffer of data
///</summary>
///<param name="buffer">
/// A buffer receiving the data.
///</param>
///<param name="length">
///Maximum number of bytes to read
///</param>
///<returns>Actual number of bytes put in the buffer. 0 means end of bytes</returns>
///
INT32 MgByteReader::Read(BYTE_ARRAY_OUT buffer, INT32 length)
{
    CHECKNULL(m_byteSource, L"MgByteReader.Read");
    return m_byteSource->GetSourceImpl()->Read(buffer, length);
}


//////////////////////////////////////////////////////////////////
///<summary>
/// Gets the mime type of the data in this reader
///</summary>
/// <returns>string representing the type, for example text/xml</returns>
///
STRING MgByteReader::GetMimeType()
{
    CHECKNULL(m_byteSource, L"MgByteReader.GetMimeType");
    return m_byteSource->GetMimeType();
}

//////////////////////////////////////////////////////////////////
/// \brief
/// Writes the the contents of the reader to a file.
///
void MgByteReader::ToFile(CREFSTRING fileName)
{
    CHECKNULL(m_byteSource, L"MgByteReader.ToFile");
    Ptr<MgByteSink> sink = new MgByteSink(this);
    sink->ToFile(fileName);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Returns the contents of the reader as a string.  The mime
/// type must be a text type, for example text/xml.
/// </summary>
///
STRING MgByteReader::ToString()
{
    STRING str;

    MgByteSink byteSink(this);

    if (IsRewindable())
    {
        Rewind();
    }

    byteSink.ToString(str);

    if (IsRewindable())
    {
        Rewind();
    }

    return str;
}

///////////////////////////////////////////////////////////////////////////////
///<summary>
/// Returns the contents of the reader as a UTF-8 string.  The mime
/// type must be a text type, for example text/xml.
///</summary>
///
void MgByteReader::ToStringUtf8(string& str)
{
    MgByteSink byteSink(this);

    byteSink.ToStringUtf8(str);
}

INT64 MgByteReader::GetLength()
{
    CHECKNULL(m_byteSource, L"MgByteReader.GetLength");
    return m_byteSource->GetSourceImpl()->GetLength();
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Serialize data to TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>
void MgByteReader::Serialize(MgStream* stream)
{
    // Send Mime Type
    stream->WriteString(GetMimeType());
    // Send the data
    stream->WriteStream(this);
}

//////////////////////////////////////////////////////////////////
///<summary>
/// Deserialize data from TCP/IP stream
///</summary>
///<param name="stream">
/// Stream
///</param>
void MgByteReader::Deserialize(MgStream* stream)
{
    STRING wcStr;
    // Get Mime Type
    stream->GetString(wcStr);
    // Get data
    Ptr<MgByteReader> byteReader = stream->GetStream(true);
    // Set values in byte source
    this->m_byteSource = SAFE_ADDREF((MgByteSource*)byteReader->m_byteSource);
    this->m_byteSource->SetMimeType(wcStr);
}

bool MgByteReader::IsRewindable()
{
    CHECKNULL(m_byteSource, L"MgByteReader.IsRewindable");
    return m_byteSource->GetSourceImpl()->IsRewindable();
}

////////////////////////////////////////////////////////////////
///<summary>
/// Rewinds to the start of the reader.  Depending on the source of
/// the reader, Rewind may not be supported.  Readers sourced from
/// true streams cannot be rewound.
///</summary>
void MgByteReader::Rewind()
{
    CHECKNULL(m_byteSource, L"MgByteReader.Rewind");
    m_byteSource->GetSourceImpl()->Rewind();
}
