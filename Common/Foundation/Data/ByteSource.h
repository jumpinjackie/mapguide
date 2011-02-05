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

#ifndef MG_BYTE_SOURCE_H
#define MG_BYTE_SOURCE_H

/// \defgroup MgByteSource MgByteSource
/// \ingroup Common_Module
/// \{

/////////////////////////////////////////////////////////////////
/// \brief
/// Represents a source of byte data with a specific content type.
///
/// \remarks
/// The
/// source can be initialized from an array of bytes in memory or a file.
/// You can get a MgByteReader object from the byte source and pass it to
/// an API function where the content will be consumed.
/// \n\n
/// Here are a few scenarios to illustrate how MgByteSink, MgByteSource
/// and MgByteReader interact:
/// \n\n
/// Loading the content of a file and creating a byte reader to be passed to a
/// service API:
/// <ol>
///  <li>Create a MgByteSource passing the file name to its constructor</li>
///  <li>Get a MgByteReader from this byte source using MgByteSource::GetReader()
///     and pass the obtained reference to the service API</li>
/// </ol>
/// \n
/// Receiving a byte reader from a service API and saving the result into a file:
/// <ol>
///  <li>Create a MgByteSink object passing the MgByteReader reference to its constructor</li>
///  <li>Call the method ToFile() on the MgByteSink object.</li>
/// </ol>
/// \n
/// Saving the content of a memory buffer (or of a string) into a file:
/// <ol>
///  <li>Create a MgByteSource object passing to its constructor the buffer and the
///     number of bytes in this buffer</li>
///  <li>Get a MgByteReader from this byte source using MgByteSource::GetReader()</li>
///  <li>Create a MgByteSink object passing the MgByteReader reference to its constructor</li>
///  <li>Call the method ToFile() on the MgByteSink object.</li>
/// </ol>
///
class MG_FOUNDATION_API MgByteSource : public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgByteSource)

PUBLISHED_API:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgByteSource object from the specific file
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgByteSource(string file);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgByteSource(String file);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgByteSource(string file);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param file (String/string)
    /// Full path name of the file.
    ///
    MgByteSource(CREFSTRING file);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgByteSource object from an array of bytes
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgByteSource(Byte[] buffer, int length);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgByteSource(byte[] buffer, int length);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgByteSource(string buffer, int length);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param buffer (byte[]/Byte[]/string)
    /// Buffer containing the data
    /// \param length (int)
    /// Length of the buffer
    ///
    MgByteSource(BYTE_ARRAY_IN buffer, INT32 length);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets the mime type of the data in this reader.  See MgMimeType for list of
    /// supported mime types.
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
    STRING GetMimeType();  /// __get, __set

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the mime type of the data.  See MgMimeType for list of supported
    /// mime types.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void SetMimeType(string mimeType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void SetMimeType(String mimeType);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void SetMimeType(string mimeType);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param mimeType (String/string)
    /// Mime type for byte source
    ///
    /// \return
    /// string representing the type, for example text/xml
    ///
    void SetMimeType(CREFSTRING mimeType);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Gets a reader for this byte source
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgByteReader GetReader();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgByteReader GetReader();
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgByteReader GetReader();
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \return
    /// A MgByteReader object
    ///
    MgByteReader* GetReader(); // _get

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Default constructor for ByteSource object. Needed for Linux build
    ///
    MgByteSource();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destruct a ByteSource object
    ///
    virtual ~MgByteSource();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgByteSource object from the specific file.  If
    /// file is marked as temporary, it will be removed when the
    /// byte source is deleted.
    ///
    /// \param file
    /// Full path name of the file.
    /// \param temporary
    /// If true, file is temporary and should be removed
    ///
    MgByteSource(CREFSTRING file, bool temporary);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgByteSource object from MgByte.  The reader pulls
    /// data directly from MgByte and reference counts it accordingly.
    ///
    /// \param bytes
    /// MgByte object to use
    ///
    MgByteSource(MgByte* bytes);

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructs an MgByteSource object from ByteSourceImpl. The reader pulls
    /// data directly from ByteSourceImpl and reference counts it accordingly.
    ///
    /// \param byteSourceImpl
    /// ByteSourceImpl object to use.
    ///
    MgByteSource(ByteSourceImpl* byteSourceImpl);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the underlying data source
    ///
    /// \return
    /// Information about the underlying data source
    ///
    ByteSourceImpl* GetSourceImpl();

protected:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId(); // { return m_cls_id; }

    //////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    virtual void Dispose();

protected:
    STRING          m_mimeType;
    ByteSourceImpl* m_sourceImpl;

CLASS_ID:
    static const INT32 m_cls_id = Foundation_Data_ByteSource;
};
/// \}

#endif // MG_BYTE_SOURCE_H
