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

#ifndef MG_BYTE_SOURCE_FILE_IMPL_H_
#define MG_BYTE_SOURCE_FILE_IMPL_H_

/// \cond INTERNAL
/////////////////////////////////////////////////////////////////
/// \brief
/// ByteSourceMemoryImpl provides access to the bytes on a file source
///
/// INTERNAL_CLASS - NOT EXPOSED
///
class ByteSourceFileImpl : public ByteSourceImpl
{
    DECLARE_CLASSNAME(ByteSourceFileImpl)

public:

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Construct a ByteSourceFileImpl object
    ///
    /// \param filename
    /// Name of the file
    ///
    ByteSourceFileImpl(CREFSTRING filename, bool temporary = false);

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Destruct a ByteSourceFileImpl object
    ///
    virtual ~ByteSourceFileImpl();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines whether or not the byte source is from a temporary file.
    ///
    /// \return
    /// true if the byte source is from a temporary file, false otherwise.
    ///
    bool IsTemporary() const;

    //////////////////////////////////////////////////////////////////
    /// \brief
    /// Reads a buffer
    ///
    /// \param buffer
    /// A buffer receiving the data.
    /// \param length
    /// Maximum number of bytes to read
    ///
    /// \return
    /// Actual number of bytes put in the buffer. 0 means end of bytes
    ///
    INT32 Read(BYTE_ARRAY_OUT buffer, INT32 length);

    INT64 GetLength();

    ///////////////////////////////////////////////////////////////////////////
    /// \brief
    /// Determines if the source is rewindable.
    ///
    /// \return
    /// true if the source is rewindable, false otherwise.
    ///
    bool IsRewindable();

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Rewinds to the start of file.  File length is reset.
    ///
    /// \return
    /// Nothing
    ///
    void Rewind();

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Renames the file.  File is renamed to the new name and
    /// rewound to start of file.  Rename is only allowed on
    /// temporary files (see constructor).
    ///
    /// \param newName
    /// New name for file
    ///
    /// \return
    /// Nothing
    ///
    /// \exception MgInvalidOperationException if file is not temporary
    ///
    void Rename(CREFSTRING newName);

protected:

    void LoadFile(CREFSTRING name);

    STRING      m_name;
    ACE_HANDLE  m_file;
    INT64       m_size;
    bool        m_temporary;
};
/// \endcond


/// Inline Methods
/// \cond INTERNAL
inline bool ByteSourceFileImpl::IsTemporary() const
{
    return m_temporary;
}
/// \endcond

#endif // MG_BYTE_SOURCE_FILE_IMPL_H_
