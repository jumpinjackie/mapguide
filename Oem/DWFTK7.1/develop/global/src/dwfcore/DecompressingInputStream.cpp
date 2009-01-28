//
//  Copyright (C) 2003-2009 by Autodesk, Inc.
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


#include "dwfcore/DecompressingInputStream.h"
using namespace DWFCore;



DWFDecompressingInputStream::DWFDecompressingInputStream( DWFInputStream* pInputStream,
                                                          bool            bOwnStream )
throw()
        : DWFInputStream()
        , _pSourceStream( pInputStream )
        , _bOwnStream( bOwnStream )
        , _bDecompressionStreamInit( false )
        , _bDecompressionStreamFinished( false )
        , _nSourceBufferBytes( 0 )
        , _pSourceBuffer( NULL )
        , _nDecompressedBytes( 0 )
        , _nDecompressionBufferSize( 0 )
        , _nDecompressionBufferOffset( 0 )
        , _nDecompressedBytesBuffered( 0 )
        , _pDecompressionBuffer( NULL )
        , _bPending( false )
{
    _oDecompressionStream.zalloc = NULL;
    _oDecompressionStream.zfree = NULL;
    _oDecompressionStream.opaque = NULL;

    _oDecompressionStream.next_in = NULL;
    _oDecompressionStream.next_out = NULL;

    _oDecompressionStream.avail_in = 0;
    _oDecompressionStream.avail_out = 0;

    if (_pSourceStream)
    {
        _bDecompressionStreamInit = (inflateInit2(&_oDecompressionStream, -MAX_WBITS) == Z_OK);
    }
}

DWFDecompressingInputStream::~DWFDecompressingInputStream()
throw()
{
    if (_bDecompressionStreamInit)
    {
        inflateEnd( &_oDecompressionStream );
    }

    if (_pDecompressionBuffer)
    {
        DWFCORE_FREE_MEMORY( _pDecompressionBuffer );
    }

    if (_pSourceBuffer)
    {
        ::free( _pSourceBuffer );
    }

    if (_pSourceStream && _bOwnStream)
    {
        DWFCORE_FREE_OBJECT( _pSourceStream );
    }
}

size_t
DWFDecompressingInputStream::available() const
throw( DWFException )
{
    if (_bDecompressionStreamInit == false)
    {
        _DWFCORE_THROW( DWFIOException, L"Decompression stream failed to initialize" );
    }

        //
        // next read can get this many pre-inflated bytes
        //
    if (_nDecompressedBytesBuffered > 0)
    {
        return _nDecompressedBytesBuffered;
    }
        //
        // source stream and decompression buffer fully exhausted
        //
    else if (_bDecompressionStreamFinished)
    {
        return 0;
    }
        //
        // this will reflect the number of compressed bytes remaining
        //
    else
    {
            //
            // if there are more bytes left to decompress in the zlib stream
            // return that number before resorting to the underlying file.
            // this is actually fairly important since for some smaller files
            // (or highly compressed ones) with big read buffers, the underlying
            // file bytes will be consumed before the zlib stream clears.
            //
        if (_bPending && (_oDecompressionStream.avail_in > 0))
        {
            return _oDecompressionStream.avail_in;
        }

            //
            // anything left in the source file?
            //
        size_t nAvailable = _pSourceStream->available();
        if (nAvailable == 0)
        {
            //
            // we can get here if the source has been depleted,
            // the zlib stream has bytes remaining BUT is basically
            // finished and the next call will complete the decompression.
            // at this point, we really have no idea how much is left
            // but it's safe to assume it must be less than
            // DWFCORE_DECOMPRESSING_INPUT_STREAM_DECOMPRESSION_BUFFER_SIZE.
            // so we return this, so the process can finish.
            //
            return DWFCORE_DECOMPRESSING_INPUT_STREAM_DECOMPRESSION_BUFFER_SIZE;
        }

        return nAvailable;
    }
}

size_t
DWFDecompressingInputStream::read( void*    pBuffer,
                                   size_t   nBytesToRead )
throw( DWFException )
{
    if (_bDecompressionStreamInit == false)
    {
        _DWFCORE_THROW( DWFIOException, L"Decompression stream failed to initialize" );
    }

        //
        // flush the existing buffer before decompressing new bytes
        //
    if (_nDecompressedBytesBuffered > 0)
    {
        size_t nBytesToFlush = dwf_min( nBytesToRead, _nDecompressedBytesBuffered );

        DWFCORE_COPY_MEMORY( pBuffer, (_pDecompressionBuffer + _nDecompressionBufferOffset), nBytesToFlush );

        _nDecompressedBytesBuffered -= nBytesToFlush;
        _nDecompressionBufferOffset += nBytesToFlush;

        return nBytesToFlush;
    }

    if (_bPending == false)
    {
            //
            //
            // Choose a size for our new buffer.
            //
            // We wish to reasonably maximize read buffer size, and minimize reallocs.
            //
        size_t nDesiredBufferBytes = _nSourceBufferBytes;
        if(_nSourceBufferBytes < nBytesToRead)
        {
                //
                // If we'd previously allocated a buffer, but we need
                // more space, the new buffer should be at least twice
                // as large as the previous one...
                //
            nDesiredBufferBytes = 2 * _nSourceBufferBytes;

                //
                // And its size should be at least 1024. Don't mess
                // with too small a buffer, it just causes trouble
                // for the decompressor...
                //
            nDesiredBufferBytes = dwf_max( nDesiredBufferBytes, 1024 );

                //
                // It should also be at least as large as the number 
                // of *decompressed* bytes we were asked for...
                //
                //      [This condition is debatable.]
                //
            nDesiredBufferBytes = dwf_max( nDesiredBufferBytes, nBytesToRead );

        }

            //
            // resize existing buffer?
            //
        if (_pSourceBuffer && (nDesiredBufferBytes > _nSourceBufferBytes))
        {
            void* pTemp = ::realloc( _pSourceBuffer, nDesiredBufferBytes );

            if (pTemp)
            {
                _pSourceBuffer = (unsigned char*)pTemp;
                _nSourceBufferBytes = nDesiredBufferBytes;
            }
        }

            //
            // need new buffer?
            //
        if (_pSourceBuffer == NULL)
        {
            _pSourceBuffer = (unsigned char*)::malloc( nDesiredBufferBytes );

            if (_pSourceBuffer == NULL)
            {
                _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate input buffer" );
            }

            _nSourceBufferBytes = nDesiredBufferBytes;
        }

        //
        // Read compressed bytes from underlying source stream.
        //
        // Note: Attempt to fill the buffer. Don't just read the size
        // that our caller requested. They might be asking for an amount
        // that's too small to decompress. In this case, even if they
        // request repeatedly, it won't help.
        //
        _oDecompressionStream.avail_in = (uInt)_pSourceStream->read( _pSourceBuffer, _nSourceBufferBytes /* was: nBytesToRead */ );
        _oDecompressionStream.next_in = _pSourceBuffer;
    }

    //
    // decompress directly to the caller's buffer
    //
    _oDecompressionStream.next_out = (Bytef*)pBuffer;
    _oDecompressionStream.avail_out = (uInt)nBytesToRead;

        //
        // if there are no pending input bytes then there's nothing to inflate
        //
    if (_oDecompressionStream.avail_in == 0 && _bPending == false)
    {
        _nDecompressedBytes = 0;
        _bDecompressionStreamFinished = true;

        return _nDecompressedBytes;
    }

    //
    // perform decompression
    //
    int eResult = inflate( &_oDecompressionStream, Z_SYNC_FLUSH );

    switch (eResult)
    {
        case Z_STREAM_END:
        case Z_FINISH:
        {
            _bDecompressionStreamFinished = true;

            //
            // no break - fall through
            //
        }

        case Z_OK:
        {
            //
            // avail_out is modified during inflate() to reflect the number of
            // bytes remaining in the output buffer that we provided.
            //
            _nDecompressedBytes = (nBytesToRead - _oDecompressionStream.avail_out);

                //
                // are there decompressed bytes leftover that couldn't
                // fit into the caller's buffer? if so, cache them.
                //
            if ((eResult == Z_OK) && (_oDecompressionStream.avail_out == 0))
            {
                if (_pDecompressionBuffer == NULL)
                {
                    _pDecompressionBuffer = DWFCORE_ALLOC_MEMORY( unsigned char, DWFCORE_DECOMPRESSING_INPUT_STREAM_DECOMPRESSION_BUFFER_SIZE );

                    if (_pDecompressionBuffer == NULL)
                    {
                        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate buffer" );
                    }
                }

                //
                // capture whatever we can
                //
                _oDecompressionStream.next_out = (Bytef*)_pDecompressionBuffer;
                _oDecompressionStream.avail_out = DWFCORE_DECOMPRESSING_INPUT_STREAM_DECOMPRESSION_BUFFER_SIZE;

                eResult = inflate( &_oDecompressionStream, Z_SYNC_FLUSH );

                    //
                    // did this clear the source?
                    //
                if ((eResult == Z_STREAM_END) ||
                    (eResult == Z_FINISH))
                {
                    _bDecompressionStreamFinished = true;
                }
                else
                {
                        //
                        // still more? we have to wait until these bytes are consumed
                        // then we continue inflating; only then do we read again from the stream
                        //
                    _bPending = (_oDecompressionStream.avail_out == 0);
                }

                _nDecompressionBufferOffset = 0;
                _nDecompressedBytesBuffered = (DWFCORE_DECOMPRESSING_INPUT_STREAM_DECOMPRESSION_BUFFER_SIZE - _oDecompressionStream.avail_out);
            }
            else
            {
                _bPending = false;
            }

            break;
        }

        case Z_STREAM_ERROR:
        {
            _DWFCORE_THROW( DWFIOException, L"ZLIB stream error occured" );
        }

        case Z_DATA_ERROR:
        {
            _DWFCORE_THROW( DWFIOException, L"ZLIB data error occured" );
        }

        case Z_MEM_ERROR:
        {
            _DWFCORE_THROW( DWFIOException, L"ZLIB memory error occured" );
        }

        case Z_BUF_ERROR:
        {
            _DWFCORE_THROW( DWFIOException, L"ZLIB buffer error occured" );
        }

        default:
        {
            _DWFCORE_THROW( DWFIOException, L"Unexpected ZLIB error occured" );
        }
    }

    //
    // return the number of bytes that zlib filled into the calling buffer.
    //
    return _nDecompressedBytes;
}

off_t
DWFDecompressingInputStream::seek( int      /*eOrigin*/,
                                   off_t    /*nOffset*/ )
throw( DWFException )
{
    _DWFCORE_THROW( DWFNotImplementedException, L"Seeking not permitted on this stream" );
}
