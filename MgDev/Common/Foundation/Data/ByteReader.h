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

#ifndef MG_BYTE_READER_H_
#define MG_BYTE_READER_H_

class MgByteSource;
template class MG_FOUNDATION_API Ptr<MgByteSource>;

class MgByteReader;
template class MG_FOUNDATION_API Ptr<MgByteReader>;

/// \defgroup MgByteReader MgByteReader
/// \ingroup Common_Module
/// \{

////////////////////////////////////////////////////////////////////
/// \brief
/// Reads data from a byte source. Once read, the data in the
/// reader cannot be reread.
///
/// \remarks
/// Here are a few scenarios to illustrate how MgByteSink,
/// MgByteSource and MgByteReader interact:
/// \n\n
/// Loading the content of a file and creating a byte reader to
/// be passed to a service API:
/// \n
/// <ul>
///   <li>Create a MgByteSource passing the file name to its
///     constructor</li>
///   <li>Get a MgByteReader from this byte source using
///     MgByteSource::GetReader() and pass the obtained reference to
///     the service API</li
/// </ul>
/// \n
/// Receiving a byte reader from a service API and saving the
/// result into a file:
/// <ul>
///   <li>Create a MgByteSink object passing the MgByteReader
///     reference to its constructor</li>
///   <li>Call the method ToFile() on the MgByteSink object.</li>
/// </ul>
/// \n
/// Saving the content of a memory buffer (or of a string) into a
/// file:
/// <ul>
///   <li>Create a MgByteSource object passing to its constructor
///     the buffer and the number of bytes in this buffer</li>
///   <li>Get a MgByteReader from this byte source using
///     MgByteSource::GetReader()</li>
///   <li>Create a MgByteSink object passing the MgByteReader
///     reference to its constructor</li>
///   <li>Call the method ToFile() on the MgByteSink object.</li>
/// </ul>
///

class MG_FOUNDATION_API MgByteReader : public MgSerializable
{
    friend class MgByteSource;
    friend class MgByteSink;

    MG_DECL_DYNCREATE();
    DECLARE_CLASSNAME(MgByteReader)

PUBLISHED_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a byte reader from a file
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgByteReader(string fileName, string mimeType, bool removeFile);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgByteReader(String fileName, String mimeType, bool removeFile);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgByteReader(string fileName, string mimeType, bool removeFile);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param fileName (string/String/string)
    /// Name of file on disk.
    ///
    /// \param mimeType (string/String/string)
    /// Mime type for content
    ///
    /// \param removeFile (bool)
    /// Remove file when byte reader is deleted
    ///
    /// \return
    /// Returns nothing
    ///
    MgByteReader(CREFSTRING fileName, CREFSTRING mimeType, bool removeFile);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a byte reader from a string
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgByteReader(string content, string mimeType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgByteReader(String content, String mimeType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgByteReader(string content, string mimeType);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param contents (string/String/string)
    /// String contents for byte reader.
    ///
    /// \param mimeType (string/String/string)
    /// Mime type for content.
    ///
    /// \return
    /// Returns nothing
    ///
    MgByteReader(CREFSTRING contents, CREFSTRING mimeType);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Creates a byte reader from an array of bytes
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgByteReader(Byte[] content, string mimeType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgByteReader(byte[] content, String mimeType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgByteReader(string content, string mimeType);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param contents (Byte[]/byte[]/string)
    /// Binary contents for byte reader.
    ///
    /// \param length (int/int/int)
    /// Length of contents.
    ///
    /// \param mimeType (string/String/string)
    /// Mime type for content.
    ///
    /// \return
    /// Returns nothing
    ///
    MgByteReader(BYTE_ARRAY_IN contents, INT32 length, CREFSTRING mimeType);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Reads a buffer
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// int Read(Byte[] buffer, int length);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// int Read(byte[] buffer, int length);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// int Read(string buffer, int length);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param buffer (byte[]/Byte[]/string)
    /// A buffer receiving the data.
    /// \param length (int)
    /// Maximum number of bytes to read
    ///
    /// \return
    /// Actual number of bytes put in the buffer. 0 means end of bytes
    ///
    INT32 Read(BYTE_ARRAY_OUT buffer, INT32 length);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the mime type of the data in this reader.  See MgMimeType
    /// for list of possible mime types.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string GetMimeType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String GetMimeType();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string GetMimeType();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// string representing the type, for example text/xml
    ///
    ///
    STRING GetMimeType();  /// __get

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the contents of the reader as a string.  The mime
    /// type must be a text type, for example text/xml.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// string ToString();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// String ToString();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// string ToString();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Text string of reader contents
    ///
    /// \exception MgInvalidArgumentException is thrown if the reader does not contain text
    ///
    STRING ToString();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Writes the the contents of the reader to a file.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void ToFile(string fileName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void ToFile(String fileName);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void ToFile(string fileName);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// Returns nothing
    ///
    /// \exception MgFileIoException is thrown if file cannot be written
    ///
    void ToFile(CREFSTRING fileName);

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the remaining length of the underlying byte source.
    /// This length is adjusted for previous reads.  If the returned length is zero
    /// then the underlying source may be a streaming format and the length
    /// is not known.
    ///
    /// \return
    /// Remaining length of underlying byte source
    ///
    virtual INT64 GetLength();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the reader is rewindable.
    ///
    /// \return
    /// true if the source is rewindable, false otherwise.
    ///
    bool IsRewindable();

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Rewinds to the start of the reader.  Depending on the source of
    /// the reader, Rewind may not be supported.  Readers sourced from
    /// true streams cannot be rewound.
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgIoException if reader cannot be rewound
    ///
    void Rewind();

INTERNAL_API:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgByteReader object from a byte source
    ///
    /// \param byteSource
    /// Byte  source object
    ///
    MgByteReader();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Serialize data to TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Serialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Deserialize data from TCP/IP stream
    ///
    /// \param stream
    /// Stream
    ///
    virtual void Deserialize(MgStream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the byte source for a reader
    ///
    /// \return
    /// Byte source for reader
    ///
    virtual MgByteSource* GetByteSource();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Returns the contents of the reader as a UTF-8 string.  The mime
    /// type must be a text type, for example text/xml.
    ///
    /// \param str
    /// Destination string.
    ///
    /// \exception MgInvalidArgumentException is thrown if the reader does not contain text.
    ///
    void ToStringUtf8(string& str);

protected:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgByteReader object from a byte source
    ///
    /// \param byteSource
    /// Byte  source object
    ///
    MgByteReader(MgByteSource* byteSource);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destruct a MgByteReader object
    ///
    virtual ~MgByteReader();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Set the byte source for an empty reader
    ///
    /// \param byteSource
    /// Byte source object
    ///
    virtual void SetByteSource(MgByteSource* byteSource);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId();

    //////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    virtual void Dispose();

protected:
    Ptr<MgByteSource> m_byteSource;

CLASS_ID:
    static const INT32 m_cls_id = Foundation_Data_ByteReader;
};
/// \}

#endif // MG_BYTE_READER_H_
