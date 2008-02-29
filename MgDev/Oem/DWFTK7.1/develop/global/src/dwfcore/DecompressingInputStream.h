//
//  Copyright (C) 2003-2008 by Autodesk, Inc.
//
//  By using this code, you are agreeing to the terms and conditions of
//  the License Agreement included in the documentation for this code.
//
//  AUTODESK MAKES NO WARRANTIES, EXPRESS OR IMPLIED,
//  AS TO THE CORRECTNESS OF THIS CODE OR ANY DERIVATIVE
//  WORKS WHICH INCORPORATE IT.
//
//  AUTODESK PROVIDES THE CODE ON AN "AS-IS" BASIS
//  AND EXPLICITLY DISCLAIMS ANY LIABILITY, INCLUDING
//  CONSEQUENTIAL AND INCIDENTAL DAMAGES FOR ERRORS,
//  OMISSIONS, AND OTHER PROBLEMS IN THE CODE.
//
//  Use, duplication, or disclosure by the U.S. Government is subject to
//  restrictions set forth in FAR 52.227-19 (Commercial Computer Software
//  Restricted Rights) and DFAR 252.227-7013(c)(1)(ii) (Rights in Technical
//  Data and Computer Software), as applicable.
//


#ifndef _DWFCORE_DECOMPRESSING_INPUT_STREAM_H
#define _DWFCORE_DECOMPRESSING_INPUT_STREAM_H


///
///\file        dwfcore/DecompressingInputStream.h
///\brief       This header contains the declaration for the DWFDecompressingInputStream class.
///


    //
    // use either the built-in or sytem zlib header
    // depending on the pre-processor configuration
    //
#ifndef _ZLIB_H
#ifdef  DWFCORE_BUILD_ZLIB
#include "dwfcore/zlib/zlib.h"
#else
#include <zlib.h>
#endif
#endif


#include "dwfcore/InputStream.h"



#ifndef DWFCORE_DECOMPRESSING_INPUT_STREAM_DECOMPRESSION_BUFFER_SIZE
///
///\ingroup     ExternalMacros
///\brief       Defines the size of the overrun buffer to allocate if needed.
///
///             Defines the size of the overrun buffer to allocate if needed.
///             This buffer is created if more bytes are decompressed than
///             will fit into the callers read buffer.  These bytes are cached
///             and returned in a subsequent \a read() request.
///
#define DWFCORE_DECOMPRESSING_INPUT_STREAM_DECOMPRESSION_BUFFER_SIZE    16384
#endif



namespace DWFCore
{

///
///\class       DWFDecompressingInputStream     dwfcore/DecompressingInputStream.h  "dwfcore/DecompressingInputStream.h"
///\brief       An input stream object that uses zlib to decompress bytes from
///             from another stream.
///\since       1.0.1
///
///             This implementation of the DWFInputStream is used to provide
///             zlib inflation for compressed bytes sourced by another input stream.
///
class DWFDecompressingInputStream : public DWFCoreMemory
                                  , public DWFInputStream
{

public:

    ///
    ///         Constructor
    ///
    ///\param   pInputStream    An input stream from which to stream the compressed source data.
    ///                         This object should be allocated with the \b DWFCORE_ALLOC_OBJECT macro
    ///                         if ownership will be transferred.
    ///\param   bOwnStream      If \e true, this object will assume ownership of \a pInputStream 
    ///                         and delete it as necessary using \b DWFCORE_FREE_OBJECT.
    ///                         If \e false, the caller retains ownership of \a pInputStream
    ///                         and is responsible for deleting it.
    ///\throw   None
    ///
    _DWFCORE_API
    DWFDecompressingInputStream( DWFInputStream* pInputStream,
                                 bool            bOwnStream )
        throw();

    ///
    ///         Destructor
    ///
    ///\throw   None
    ///
    _DWFCORE_API
    virtual ~DWFDecompressingInputStream()
        throw();

    ///
    ///         Used to determine the availablity of data remaining in the stream.
    ///         The byte count returned by this method will report either
    ///         the number of inflated bytes left in the overrun buffer or
    ///         if that buffer is empty, the number of uncompressed bytes remaining
    ///         in the underlying source stream.  Regardless of the source, if zero
    ///         bytes are returned as available, the stream is exhuasted and thus,
    ///         fully decompressed.
    ///
    ///\return  The number of bytes available to read from the stream.
    ///\throw   DWFException
    ///
    _DWFCORE_API
    size_t available() const
        throw( DWFException );

    ///
    ///         Reads at most \a nBytesToRead uncompressed bytes from
    ///         into the buffer provided.  
    ///         If the overrun buffer contains inflated data, these will be copied
    ///         into \a pBuffer.  This buffer will always be exhausted prior to 
    ///         subsequent inflation of the compressed stream data, even if \a nBytesToRead
    ///         is larger than the number of inflated bytes in the overrun buffer.
    ///         If the overrun buffer is empty, at most \a nBytesToRead bytes will be 
    ///         decompressed from the underlying stream.
    ///         The stream is not required to fill the read buffer but it must always
    ///         report the number of bytes, including zero, that were obtained.  
    ///
    ///\param   pBuffer         A pointer to a block of memory to receive the bytes.
    ///\param   nBytesToRead    The number of bytes to copy into \a pBuffer.
    ///                         This value should not exceed the capacity of the memory block at \a pBuffer.
    ///\return  The number of uncompressed bytes read into \a pBuffer.
    ///\throw   DWFException
    ///
    _DWFCORE_API
    size_t read( void*  pBuffer,
                 size_t nBytesToRead )
        throw( DWFException );

    ///
    ///\warning This method is not implemented and will always throw an exception if invoked.
    ///
    ///\param   eOrigin     One of \b SEEK_SET, \b SEEK_CUR or \b SEEK_END.
    ///\param   nOffset     The number of bytes from \a eOrigin to move the internal cursor.
    ///\return  The previous cursor offset before the seek.
    ///\throw   DWFException
    ///
    _DWFCORE_API
    off_t seek( int     eOrigin,
                off_t   nOffset )
        throw( DWFException );

private:

    DWFInputStream* _pSourceStream;
    bool            _bOwnStream;

    bool            _bDecompressionStreamInit;
    bool            _bDecompressionStreamFinished;
    z_stream        _oDecompressionStream;

    size_t          _nSourceBufferBytes;
    unsigned char*  _pSourceBuffer;

    size_t          _nDecompressedBytes;
    size_t          _nDecompressionBufferSize;
    size_t          _nDecompressionBufferOffset;
    size_t          _nDecompressedBytesBuffered;
    unsigned char*  _pDecompressionBuffer;

    bool            _bPending;

private:

    //
    // Unimplemented methods
    //
    DWFDecompressingInputStream( const DWFDecompressingInputStream& );
    DWFDecompressingInputStream& operator=( const DWFDecompressingInputStream& );
};

}



#endif
