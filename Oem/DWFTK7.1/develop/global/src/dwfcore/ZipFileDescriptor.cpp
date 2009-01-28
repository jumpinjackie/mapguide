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


#include "dwfcore/ZipFileDescriptor.h"
#include "dwfcore/ZippingOutputStream.h"
#include "dwfcore/UnzippingInputStream.h"
using namespace DWFCore;


_DWFCORE_API
DWFZipFileDescriptor::DWFZipFileDescriptor( const DWFFile& rFile,
                                            teFileMode     eMode )
throw()
                    : DWFFileDescriptor( rFile )
                    , _eMode( eMode )
                    , _eEncryption( eEncryptionUnknown )
                    , _pZipStream( NULL )
                    , _pUnzipStream( NULL )
                    , _pInputStream( NULL )
{
    ;
}

_DWFCORE_API
DWFZipFileDescriptor::DWFZipFileDescriptor( DWFInputStream& rStream )
throw()
                    : DWFFileDescriptor( DWFFile(L"") )
                    , _eMode( eUnzip )
                    , _eEncryption( eEncryptionUnknown )
                    , _pZipStream( NULL )
                    , _pUnzipStream( NULL )
                    , _pInputStream( &rStream )
{
    ;
}

_DWFCORE_API
DWFZipFileDescriptor::~DWFZipFileDescriptor()
throw()
{
    try { DWFZipFileDescriptor::close(); } catch (...) {;}
}

_DWFCORE_API
void
DWFZipFileDescriptor::open()
throw( DWFException )
{
    return openIndexed(&_oZipFileIndex);
}

_DWFCORE_API
void
DWFZipFileDescriptor::openIndexed(DWFZipFileIndex* pIndex)
throw( DWFException )
{
    if (_eMode == eUnzip)
    {
        if (_pUnzipStream)
        {
            _DWFCORE_THROW( DWFIOException, L"Zip archive already open for unzipping" );
        }

        if (_pInputStream)
        {
            _pUnzipStream = unzOpenStream( *_pInputStream, pIndex->index() );
        }
        else
        {
            _pUnzipStream = unzOpenFile( _oFile.name(), pIndex->index() );
        }

        if (_pUnzipStream == NULL)
        {
            _DWFCORE_THROW( DWFIOException, L"Failed to open zip archive for unzipping" );
        }
    }
    else
    {
        if (_pZipStream)
        {
            _DWFCORE_THROW( DWFIOException, L"Zip archive already open for zipping" );
        }

        _pZipStream = zipOpen( _oFile.name(), 0, 0 );

        if (_pZipStream == NULL)
        {
            _DWFCORE_THROW( DWFIOException, L"Failed to open zip archive for zipping" );
        }
    }
}

_DWFCORE_API
void
DWFZipFileDescriptor::close()
throw( DWFException )
{
    if (_pUnzipStream)
    {
        unzClose( _pUnzipStream );
        _pUnzipStream = NULL;
    }
    else if (_pZipStream)
    {
        zipClose( _pZipStream );
        _pZipStream = NULL;
    }
}

_DWFCORE_API
size_t
DWFZipFileDescriptor::size()
throw( DWFException )
{
    _DWFCORE_THROW( DWFNotImplementedException, L"Not implemented yet" );
}

_DWFCORE_API
DWFZipFileDescriptor::teEncryption
DWFZipFileDescriptor::encryption()
throw( DWFException )
{
    return _eEncryption;
}

_DWFCORE_API
size_t
DWFZipFileDescriptor::read( void* pBuffer, size_t nBytesToRead )
throw( DWFException )
{
    if (_eMode == eUnzip)
    {
        if (_pUnzipStream == NULL)
        {
            _DWFCORE_THROW( DWFIOException, L"No archive open for unzipping" );
        }

        //
        // get the underlying stream
        //
        DWFInputStream* pUnzipStream = unzGetFilePointer( _pUnzipStream );

        //
        // perform read
        //
        return pUnzipStream->read( pBuffer, nBytesToRead );
    }
    else
    {
        _DWFCORE_THROW( DWFIOException, L"No archive open for unzipping" );
    }
}

_DWFCORE_API
off_t
DWFZipFileDescriptor::seek( int eOrigin, off_t nOffset )
throw( DWFException )
{
    if (_eMode == eUnzip)
    {
        if (_pUnzipStream == NULL)
        {
            _DWFCORE_THROW( DWFIOException, L"No archive open for unzipping" );
        }

        //
        // get the underlying file descriptor
        //
        DWFInputStream* pUnzipStream = unzGetFilePointer( _pUnzipStream );

        //
        // the current offset
        //
        off_t nPrevOffset = unztell( _pUnzipStream );

        //
        // perform seek
        //
        pUnzipStream->seek( eOrigin, nOffset );

        //
        // return previous offset
        //
        return nPrevOffset;
    }
    else
    {
        _DWFCORE_THROW( DWFIOException, L"No archive open for unzipping" );
    }
}

_DWFCORE_API
bool
DWFZipFileDescriptor::locate( const DWFString& rArchivedFile )
throw( DWFException )
{
    if (_pUnzipStream == NULL)
    {
        _DWFCORE_THROW( DWFIOException, L"No archive open for unzipping" );
    }

    if (unzLocateFile(_pUnzipStream, rArchivedFile, 1) == UNZ_OK)
    {
        int nEnc = unzIsEncrypted( _pUnzipStream );
        _eEncryption = ((nEnc == 1) ? eEncrypted : ((nEnc == 0) ? eNotEncrypted : eEncryptionUnknown));

        return true;
    }
    else
    {
        return false;
    }
}

_DWFCORE_API
DWFInputStream*
DWFZipFileDescriptor::unzip( const DWFString& zArchivedFile,
                             const DWFString& zPassword )
throw( DWFException )
{
    if (_pUnzipStream == NULL)
    {
        _DWFCORE_THROW( DWFIOException, L"No archive open for unzipping" );
    }

    //
    // to unzip a file from the archive,
    // simply create an unzipping stream
    // and open the corresponding file.
    // the caller can then stream the file
    // wherever he likes
    //

    DWFUnzippingInputStream* pUnzipper = DWFCORE_ALLOC_OBJECT( DWFUnzippingInputStream(_pUnzipStream) );

    if (pUnzipper == NULL)
    {
        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate unzipping reader stream" );
    }

    //
    // locates and readies the file for unzipping
    //
    pUnzipper->open( zArchivedFile, zPassword );

    //
    // the caller now has control of the stream
    // and must destroy it with DWFCORE_FREE_OBJECT
    //
    return pUnzipper;
}

_DWFCORE_API
DWFOutputStream*
DWFZipFileDescriptor::zip( const DWFString& zArchiveFile,
                           const DWFString& zPassword )
throw( DWFException )
{
    if (_pZipStream == NULL)
    {
        _DWFCORE_THROW( DWFIOException, L"No archive open for zipping" );
    }

    //
    //
    //
    DWFZippingOutputStream* pZipper = DWFCORE_ALLOC_OBJECT( DWFZippingOutputStream(_pZipStream, _eMode) );

    //
    // creates the file block in the archive
    //
    pZipper->open( zArchiveFile, zPassword );

    //
    // the caller now has control of the stream
    // and must destroy it with DWFCORE_FREE_OBJECT
    // when this stream is deleted, the archive will be finalized
    //
    return pZipper;
}


