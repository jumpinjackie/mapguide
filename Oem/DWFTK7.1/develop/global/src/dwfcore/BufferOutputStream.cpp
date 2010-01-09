//
//  Copyright (C) 2003-2010 by Autodesk, Inc.
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


#include "dwfcore/BufferOutputStream.h"
using namespace DWFCore;


_DWFCORE_API
DWFBufferOutputStream::DWFBufferOutputStream( size_t    nInitialBufferBytes,
                                              off_t     nMaxBufferBytes )
throw()
        : _pBuffer( NULL )
        , _iBufferPos( 0 )
        , _nBufferBytes( 0 )
        , _nBufferBytesRemaining( 0 )
        , _pOutputStream( NULL )
        , _bOwnStream( false )
        , _nMaxBufferBytes( nMaxBufferBytes )
        , _nInitialBufferBytes( nInitialBufferBytes )
{
    ;
}

_DWFCORE_API
DWFBufferOutputStream::DWFBufferOutputStream( void*     pBuffer,
                                              size_t    nBufferBytes )
throw()
        : _pBuffer( pBuffer )
        , _iBufferPos( 0 )
        , _nBufferBytes( nBufferBytes )
        , _nBufferBytesRemaining( nBufferBytes )
        , _pOutputStream( NULL )
        , _bOwnStream( false )
        , _nMaxBufferBytes( 0 )
        , _nInitialBufferBytes( 0 )
{
    ;
}

_DWFCORE_API
DWFBufferOutputStream::DWFBufferOutputStream( DWFOutputStream*  pOutputStream,
                                              bool              bOwnStream,
                                              size_t            nBufferBytes )
throw()
        : _pBuffer( NULL )
        , _iBufferPos( 0 )
        , _nBufferBytes( 0 )
        , _nBufferBytesRemaining( 0 )
        , _pOutputStream( pOutputStream )
        , _bOwnStream( bOwnStream )
        , _nMaxBufferBytes( 0 )
        , _nInitialBufferBytes( nBufferBytes )
{
    ;
}

_DWFCORE_API
DWFBufferOutputStream::~DWFBufferOutputStream()
throw()
{
    if ((_nInitialBufferBytes > 0) && (_pBuffer != NULL))
    {
        ::free( _pBuffer );
    }

    if (_bOwnStream && _pOutputStream)
    {
        DWFCORE_FREE_OBJECT( _pOutputStream );
    }
}

_DWFCORE_API
size_t
DWFBufferOutputStream::write( const void*   pBuffer,
                              size_t        nBytesToWrite )
throw( DWFException )
{
    size_t nBytesWritten = 0;

        //
        // fixed buffer
        //
    if (_nInitialBufferBytes == 0)
    {
        if (_pBuffer == NULL)
        {
            _DWFCORE_THROW( DWFNullPointerException, L"No buffer provided" );
        }

        nBytesWritten = (nBytesToWrite < _nBufferBytesRemaining)
                        ? nBytesToWrite
                        : _nBufferBytesRemaining;

            //
            //
            //
        if (nBytesWritten > 0)
        {
            char* pDest = (char*)_pBuffer + _iBufferPos;
            DWFCORE_COPY_MEMORY( pDest, pBuffer, nBytesWritten );

            _iBufferPos += nBytesWritten;
            _nBufferBytesRemaining -= nBytesWritten;
        }
            //
            // buffer full
            //
        else
        {
            _DWFCORE_THROW( DWFIOException, L"Buffer full" );
        }
    }
        //
        // auto-size
        //
    else
    {
        if (_pBuffer == NULL)
        {
            _pBuffer = (char*)::malloc( _nInitialBufferBytes );

            if (_pBuffer == NULL)
            {
                _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate buffer" );
            }

            _nBufferBytes = _nBufferBytesRemaining = _nInitialBufferBytes;
        }

            //
            //
            //
        if (nBytesToWrite > _nBufferBytesRemaining)
        {
            //
            // current buffer size
            //
            size_t nBytesUsed = _nBufferBytes - _nBufferBytesRemaining;

                //
                // write current buffer to stream
                // nBytesUsed can be zero if we recursed in here to flush
                // the existing buffer but the number of bytes to write
                // exceeds the initial buffer size, this check will force
                // a buffer reallocation to handle the call
                //
            if (_pOutputStream && (nBytesUsed > 0))
            {
                //
                //
                //
                nBytesWritten = _pOutputStream->write( _pBuffer, _iBufferPos );

                //
                // reset internals
                //
                _iBufferPos = 0;
                _nBufferBytesRemaining = _nBufferBytes;

                //
                // recurse with original call
                //
                return this->DWFBufferOutputStream::write( pBuffer, nBytesToWrite );
            }
                //
                // resize internal buffer
                //
            else
            {
                //
                // double (at least) the buffer
                //
                _nBufferBytes = dwf_max( 2*_nBufferBytes, 2*(nBytesToWrite + nBytesUsed) );

                    //
                    // cast OK since _nMaxBufferBytes checked > 0
                    //
                if ((_nMaxBufferBytes > 0) && (_nBufferBytes > (size_t)_nMaxBufferBytes))
                {
                    _DWFCORE_THROW( DWFIOException, L"Cannot resize stream buffer - exceeded requested maximum" );
                }

                void* pTemp = ::realloc( _pBuffer, _nBufferBytes );
                if (pTemp == NULL)
                {
                    _DWFCORE_THROW( DWFMemoryException, L"Failed to resize buffer" );
                }

                _pBuffer = pTemp;
                _nBufferBytesRemaining = _nBufferBytes - nBytesUsed;
            }
        }

        char* pDest = (char*)_pBuffer + _iBufferPos;
        DWFCORE_COPY_MEMORY( pDest, pBuffer, nBytesToWrite );

        _iBufferPos += nBytesToWrite;
        _nBufferBytesRemaining -= nBytesToWrite;

        nBytesWritten = nBytesToWrite;
    }


    return nBytesWritten;
}

_DWFCORE_API
void
DWFBufferOutputStream::flush()
throw( DWFException )
{
        //
        // only flush a stream
        //
    if (_pOutputStream)
    {
            //
            // write out the remaining bytes
            //
        if (_pBuffer)
        {
            _pOutputStream->write( _pBuffer, _iBufferPos );
            _pOutputStream->flush();

            DWFCORE_ZERO_MEMORY( _pBuffer, _iBufferPos );
            _iBufferPos = 0;
        }
    }
}

_DWFCORE_API
size_t
DWFBufferOutputStream::buffer( void** ppBuffer ) const
throw( DWFException )
{
    if (ppBuffer == NULL)
    {
        _DWFCORE_THROW( DWFInvalidArgumentException, L"No buffer provided" );
    }

    if (_pBuffer)
    {
        *ppBuffer = DWFCORE_ALLOC_MEMORY( char, _iBufferPos );

        if (*ppBuffer == NULL)
        {
            _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate new buffer" );
        }

        DWFCORE_COPY_MEMORY( *ppBuffer, _pBuffer, _iBufferPos );

        return _iBufferPos;
    }
    else
    {
        *ppBuffer = NULL;
        return 0;
    }
}
