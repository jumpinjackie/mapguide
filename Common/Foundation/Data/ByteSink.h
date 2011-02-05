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

#ifndef MG_BYTE_SINK_H
#define MG_BYTE_SINK_H

/// \defgroup MgByteSink MgByteSink
/// \ingroup Common_Module
/// \{

class MgByteReader;

///////////////////////////////////////////////////////////////////////////////
/// \brief
/// MgByteSink is a used to write the content of a MgByteReader object to a file.
/// MgByteSink is, like MgByteReader, inherently read-only.
///
/// \remarks
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
/// \n
/// Copying a file to another file:
/// <ol>
///  <li>Create a MgByteSource object passing to its constructor the pathname
///     of the source file.</li>
///  <li>Get a MgByteReader from this byte source using MgByteSource::GetReader().</li>
///  <li>Create a MgByteSink object passing the MgByteReader reference to its
///     constructor.</li>
///  <li>Call the method ToFile() on the MgByteSink object.</li>
/// </ol>
///
class MG_FOUNDATION_API MgByteSink: public MgGuardDisposable
{
    DECLARE_CLASSNAME(MgByteSink)

PUBLISHED_API:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a MgByteSink object from a source byte reader
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// MgByteSink(MgByteReader reader);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// MgByteSink(MgByteReader reader);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// MgByteSink(MgByteReader reader);
    /// \htmlinclude SyntaxBottom.html
    /// \param reader (MgByteReader)
    /// MgByteReader object
    ///
    MgByteSink(MgByteReader* reader);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Writes the content of the byte reader to a file.  If the
    /// full contents of the reader cannot be written to the file
    /// then the file is deleted and an MgException is raised.
    ///
    /// <!-- Syntax in .Net, Java, and PHP -->
    /// \htmlinclude DotNetSyntaxTop.html
    /// void ToFile(string filename);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude JavaSyntaxTop.html
    /// void ToFile(String filename);
    /// \htmlinclude SyntaxBottom.html
    /// \htmlinclude PHPSyntaxTop.html
    /// void ToFile(string filename);
    /// \htmlinclude SyntaxBottom.html
    ///
    /// \param filename (String/string)
    /// Destination file name
    ///
    void ToFile(CREFSTRING filename);

INTERNAL_API:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destruct a MgByteSource object
    ///
    /// \return
    /// Nothing
    ///
    virtual ~MgByteSink();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Send the content of the byte reader to a string
    ///
    /// \param strDest
    /// Destination string
    ///
    void ToString(REFSTRING strDest);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Send the content of the byte reader to a UTF-8 string
    ///
    /// \param strDest
    /// Destination string
    ///
    void ToStringUtf8(string& strDest);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Send the content of the byte reader to a socket stream
    ///
    /// \param stream
    /// Destination ACE stream
    ///
    void ToStream(ACE_SOCK_Stream* stream);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Sends the content of the byte reader to an MgByte object.
    ///
    /// \return
    /// MgByte object.
    ///
    MgByte* ToBuffer();

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Get the unique identifier for the class
    ///
    /// \return
    /// Class Identifider.
    ///
    virtual INT32 GetClassId() { return m_cls_id; }

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Dispose this object.
    ///
    /// \return
    /// Returns nothing
    ///
    virtual void Dispose()
    {
        delete this;
    }

protected:
    MgByteReader*   m_reader;

CLASS_ID:
    static const INT32 m_cls_id = Foundation_Data_ByteSink;

private:
    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Default Constructor
    ///
    MgByteSink();
};
/// \}

#define BSINK_BUFFER_SIZE       (1*1024*1024)

#endif
