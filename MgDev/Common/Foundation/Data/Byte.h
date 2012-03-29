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

#ifndef _MG_BYTE
#define _MG_BYTE

/// \cond INTERNAL

/// \brief
/// A utility class to store bytes
class MG_FOUNDATION_API MgByte : public MgDisposable
{
    DECLARE_CLASSNAME(MgByte)

public:
    /// \brief
    /// Maximum size of an MgByte array created by the MgStreamReader.
    ///
    /// \remarks
    /// This value is only used by MgStreamReader to control when a
    /// file based stream should be used instead of a memory based one.
    ///
    /// \todo
    /// This parameter should be tunable from the configuration file
    static const INT64 MaxSize = 64*1024*1024;

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Allocator type for MgByte.  Controls allocation and
    /// deallocation of the bytes managed by MgByte.
    ///
    enum AllocatorType
    {
    /// \brief
    /// Bytes were allocated externally malloc and should be :free()'d
    /// when MgByte destructs.  Append is not legal if this is specified.
    ///
    Malloc,
    /// \brief
    /// Bytes were allocated externally new [] and should be delete []'d
    /// when MgByte destructs.  Append is not legal if this is specified.
    ///
    New,
    /// \brief
    /// Bytes should be allocated internally by MgByte (using new[]) and
    /// delete []'d when MgByte destructs.  Internal must be specified
    /// for Append to function.
    ///
    Internal,
    /// \brief
    /// Bytes allocation and deallocation is being managed externally.
    /// Append is not legal for this allocation type.
    ///
    None
    };

    /// \brief
    /// Constructor
    ///
    MgByte();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Constructor.  Allocates memory if allocate is true.  If allocator
    /// is Internal, memory will be managed internally and freed using
    /// delete [].
    ///
    ///
    /// \remarks
    /// Specifying len > 0 and bytes = NULL will preallocate
    /// a len sized byte array and set the initial length to zero.  Bytes() should
    /// be used to fill in the array and SetLength() should be called to
    /// define the length of the array.
    ///
    /// \param bytes
    /// bytes to be stored
    /// \param len
    /// len of bytes
    /// \param allocator
    /// Allocation style for MgByte
    ///
    MgByte(BYTE_ARRAY_IN bytes, INT32 len, AllocatorType allocator = MgByte::Internal);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Appends bytes to existing buffer
    ///
    /// \param bytes
    /// bytes to be appended
    /// \param len
    /// len of bytes
    ///
    void Append(BYTE_ARRAY_IN bytes, INT32 len);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Reads the specified number of bytes starting at pos from the
    /// byte source.  Returns the number of bytes read.
    ///
    /// \return
    /// returns the number of bytes read.
    ///
    INT32 Read(BYTE_ARRAY_OUT buffer, INT32 pos, INT32 length);

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Retrieve a pointer to the internal byte buffer.  The buffer
    /// should not be deallocated.
    ///
    /// \return
    /// Returns a pointer to the internal buffer
    ///
    BYTE_ARRAY_OUT Bytes();

    /////////////////////////////////////////////////////////////////
    /// \brief
    /// Get length of buffer
    ///
    /// \return
    /// returns the length of buffer
    ///
    INT32 GetLength();

    ////////////////////////////////////////////////////////////////
    /// \brief
    /// Sets the length of buffer.  Buffer must have been
    /// preallocated.  If length is greater than allocated
    /// length, an exception will be thrown
    ///
    /// \param len
    /// Length of buffer
    ///
    /// \return
    /// Returns nothing
    ///
    void SetLength(INT32 len);

EXTERNAL_API:

    /// \brief
    /// Destructor
    ///
    ~MgByte();

protected:
    void Dispose();

private:
    /// Where's the beef?  End of bytes marker to detect buffer
    /// overflow errors.
    static const UINT64 EndBytesMarker = 0xBEEFBEEFBEEFBEEFULL;

    void CheckEnd();

    unsigned char* Allocate(INT32 len);

    unsigned char* m_bytes;
    INT32 m_len;
    INT32 m_allocLen;
    AllocatorType m_allocType;
};
/// \endcond

#endif


