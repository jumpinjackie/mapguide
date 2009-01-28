//
//  Copyright (C) 2004-2009 by Autodesk, Inc.
//
//  By using this code, you are agreeing to the terms and conditions of
//  the License Agreement included in the documentation for this code.
//
//  AUTODESK MAKES NO WARRANTIES, EXPRESS OR IMPLIED, AS TO THE CORRECTNESS
//  OF THIS CODE OR ANY DERIVATIVE WORKS WHICH INCORPORATE IT. AUTODESK
//  PROVIDES THE CODE ON AN "AS-IS" BASIS AND EXPLICITLY DISCLAIMS ANY
//  LIABILITY, INCLUDING CONSEQUENTIAL AND INCIDENTAL DAMAGES FOR ERRORS,
//  OMISSIONS, AND OTHER PROBLEMS IN THE CODE.
//
//  Use, duplication, or disclosure by the U.S. Government is subject to
//  restrictions set forth in FAR 52.227-19 (Commercial Computer Software
//  Restricted Rights) and DFAR 252.227-7013(c)(1)(ii) (Rights in Technical
//  Data and Computer Software), as applicable.
//




#include "dwfcore/Pointer.h"
#include "dwfcore/MonitoredInputStream.h"
#include "dwfcore/UnzippingInputStream.h"
using namespace DWFCore;

#include "dwf/Version.h"
#include "dwf/package/XML.h"
#include "dwf/package/Manifest.h"
#include "dwf/package/EPlotSection.h"
#include "dwf/package/EModelSection.h"
#include "dwf/package/GlobalSection.h"
#include "dwf/package/SectionBuilder.h"
#include "dwf/package/reader/PackageReader.h"
#include "dwf/package/reader/PackageManifestReader.h"
using namespace DWFToolkit;


_DWFTK_API
DWFPackageReader::DWFPackageReader()
throw()
                : _pDWFStream( NULL )
                , _pPackageManifest( NULL )
                , _pSectionBuilder( NULL )
{
    ;
}

_DWFTK_API
DWFPackageReader::DWFPackageReader( const DWFFile&      zDWFPackageFile,
                                    const DWFString&    zDWFPackagePassword )
throw()
                : _zDWFPackage( zDWFPackageFile )
                , _zDWFPassword( zDWFPackagePassword )
                , _pDWFStream( NULL )
                , _pPackageManifest( NULL )
                , _pSectionBuilder( NULL )
{
    _tPackageInfo.nVersion = 0L;
    _tPackageInfo.eType = eUnknown;
}

_DWFTK_API
DWFPackageReader::DWFPackageReader( DWFInputStream&     rDWFPackageStream,
                                    const DWFString&    zDWFPackagePassword )
throw()
                : _zDWFPassword( zDWFPackagePassword )
                , _pDWFStream( &rDWFPackageStream )
                , _pPackageManifest( NULL )
                , _pSectionBuilder( NULL )
{
    _tPackageInfo.nVersion = 0L;
    _tPackageInfo.eType = eUnknown;
}

_DWFTK_API
DWFPackageReader::~DWFPackageReader()
throw()
{
    if (_pPackageManifest)
    {
        DWFCORE_FREE_OBJECT( _pPackageManifest );
    }

    DWFStringKeySkipList<DWFTempFile*>::Iterator* piCache = _oLocalFileCache.iterator();

    if (piCache)
    {
        for (; piCache->valid(); piCache->next())
        {
            DWFCORE_FREE_OBJECT( piCache->value() );
        }

        DWFCORE_FREE_OBJECT( piCache );
    }

    if (_pSectionBuilder)
    {
        DWFCORE_FREE_OBJECT( _pSectionBuilder );
    }
}

DWFZipFileDescriptor*
DWFPackageReader::_open()
throw( DWFException )
{
    DWFZipFileDescriptor* pPackageDescriptor = NULL;

        if (_pDWFStream)
        {
        pPackageDescriptor = DWFCORE_ALLOC_OBJECT( DWFZipFileDescriptor(*_pDWFStream) );
        }
        else
        {
        pPackageDescriptor = DWFCORE_ALLOC_OBJECT( DWFZipFileDescriptor(_zDWFPackage, DWFZipFileDescriptor::eUnzip) );
        }

    if (pPackageDescriptor == NULL)
        {
            _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate file descriptor" );
    }

    _makeSectionBuilder();

    pPackageDescriptor->openIndexed(&_oZipFileIndex);
    return pPackageDescriptor;
}

_DWFTK_API
void
DWFPackageReader::setPackagePassword( const DWFString& zDWFPackagePassword )
throw()
{
    _zDWFPassword = zDWFPackagePassword;
}

_DWFTK_API
void
DWFPackageReader::getPackageInfo( tPackageInfo& rInfo )
throw( DWFException )
{
    try
    {
            //
            // open up the package if necessary
            //
        DWFPointer<DWFZipFileDescriptor> apPackageDescriptor( _open(), false );

        //
        // rewind and remember the current position
        //
        off_t iCurrent = apPackageDescriptor->seek( SEEK_SET, 0 );

            //
            // snarf the first 12 bytes
            //
        unsigned char pBuffer[12];
        if (12 == apPackageDescriptor->read(pBuffer, 12))
        {
            GetPackageInfo( pBuffer, _tPackageInfo );
        }
        else
        {
            _DWFCORE_THROW( DWFIOException, L"Invalid DWF package stream" );
        }

            //
            // restore previous position
            //
        if (iCurrent < 0)
        {
            apPackageDescriptor->seek( SEEK_END, iCurrent );
        }
        else
        {
            apPackageDescriptor->seek( SEEK_SET, iCurrent );
        }

            //
            // track down the TYPEINFO
            //
        if (_tPackageInfo.nVersion >= _DWF_FORMAT_VERSION_TYPEINFO)
        {
            //
            // simply ask the zip file if a known filename exists...
            // start with EPlot
            //
            _tPackageInfo.zTypeGUID.assign( DWFInterface::kzEPlot_ID );

            DWFString zTypeFile( _tPackageInfo.zTypeGUID );
                    zTypeFile.append( L".TYPEINFO" );

            if (apPackageDescriptor->locate(zTypeFile) == false)
            {
                _tPackageInfo.zTypeGUID.assign( DWFInterface::kzEModel_ID );
                zTypeFile.assign( _tPackageInfo.zTypeGUID );
                zTypeFile.append( L".TYPEINFO" );

                if (apPackageDescriptor->locate(zTypeFile) == false)
                {
                    _tPackageInfo.zTypeGUID.destroy();
                }
            }
        }

            //
            // DWF 6+ package - we can test for password encryption
            //
        if (_tPackageInfo.eType == eDWFPackage)
        {
            DWFZipFileDescriptor::teEncryption enc = apPackageDescriptor->encryption();

                //
                // check for password encryption using the manifest
                // if it wasn't discovered already with the typeinfo file
                //
            if (enc == DWFZipFileDescriptor::eEncryptionUnknown)
            {
                apPackageDescriptor->locate( /*NOXLATE*/L"manifest.xml" );
                enc = apPackageDescriptor->encryption();
            }

            if (enc == DWFZipFileDescriptor::eEncrypted)
            {
                _tPackageInfo.eType = eDWFPackageEncrypted;
            }
        }

        rInfo = _tPackageInfo;
    }
        //
        // thrown from the package descriptor if the file is not a zip archive
        //
    catch (DWFInvalidTypeException&)
    {
			//
			// reset the stream
			//
        if (_pDWFStream)
        {
            _pDWFStream->seek( SEEK_SET, 0 );

                //
                // snarf the first 12 bytes
                //
            unsigned char pBuffer[12];
            if (12 == _pDWFStream->read(pBuffer, 12))
            {
                GetPackageInfo( pBuffer, rInfo );
            }
            else
            {
                _DWFCORE_THROW( DWFIOException, L"Invalid DWF stream" );
            }            
        }
			//
			// reopen the file
			//
        else
        {
			//
			// open as stream
			//
            DWFStreamFileDescriptor* pStreamDescriptor = DWFCORE_ALLOC_OBJECT( DWFStreamFileDescriptor(_zDWFPackage, L"rb") );
            if (pStreamDescriptor == NULL)
            {
                _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate stream descriptor" );
            }

			//
			// open the file
			//
            pStreamDescriptor->open();

                //
                // snarf the first 12 bytes
                //
            unsigned char pBuffer[12];
            if (12 == pStreamDescriptor->read(pBuffer, 12))
            {
                GetPackageInfo( pBuffer, rInfo );
            }
            else
            {
                DWFCORE_FREE_OBJECT( pStreamDescriptor );
                _DWFCORE_THROW( DWFIOException, L"Invalid DWF file" );
            }   

            DWFCORE_FREE_OBJECT( pStreamDescriptor );
        }
    }
}

_DWFTK_API
void
DWFPackageReader::GetPackageInfo( const unsigned char* pBuffer,
                                  tPackageInfo&        rInfo )
throw( DWFException )
{
    rInfo.nVersion = 0L;
    rInfo.eType = eUnknown;
    rInfo.zTypeGUID.destroy();

    bool bDWF = (DWFCORE_COMPARE_MEMORY(pBuffer, "(DWF V", 6) == 0);
    bool bW2D = (bDWF ? false : (DWFCORE_COMPARE_MEMORY(pBuffer, "(W2D V", 6) == 0));

    if (!bDWF && !bW2D)
    {
        const unsigned char pZipHeader[4] = {'P', 'K', 0x03, 0x04};
        rInfo.eType = (DWFCORE_COMPARE_MEMORY(pBuffer, pZipHeader, 4) == 0) ? eZIPFile : eUnknown;
        return;
    }

    unsigned char* pChar = (unsigned char*)&pBuffer[6];

    if (*pChar < '0' || *pChar++ > '9' ||
        *pChar < '0' || *pChar++ > '9' ||
        *pChar++ != '.'                ||
        *pChar < '0' || *pChar++ > '9' ||
        *pChar < '0' || *pChar++ > '9' ||
        *pChar != ')')
    {
        rInfo.eType = eUnknown;
        rInfo.nVersion = 0L;
        rInfo.zTypeGUID.destroy();
        return;
    }

    rInfo.nVersion = ((pBuffer[6] - '0')*10 + (pBuffer[7] - '0'))*100 + ((pBuffer[9] - '0')*10 + (pBuffer[10] - '0'));

    rInfo.eType = (rInfo.nVersion >= _DWF_FORMAT_VERSION_PACKAGE && bW2D) ? eW2DStream :
                    (rInfo.nVersion >= _DWF_FORMAT_VERSION_PACKAGE && bDWF) ? eDWFPackage :
                        (rInfo.nVersion < _DWF_FORMAT_VERSION_PACKAGE && bDWF) ? eDWFStream : eUnknown;
}

_DWFTK_API
DWFToolkit::DWFManifest&
DWFPackageReader::getManifest( DWFManifestReader* pFilter )
throw( DWFException )
{
    if (_pPackageManifest == NULL)
    {
        _pPackageManifest = DWFCORE_ALLOC_OBJECT( DWFManifest(this) );

        if (_pPackageManifest == NULL)
        {
            _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate manifest" );
        }

        if (pFilter)
        {
            _pPackageManifest->setFilter( pFilter );
        }

        getManifest( *_pPackageManifest );
    }

    return *_pPackageManifest;
}

_DWFTK_API
void
DWFPackageReader::getManifest( DWFManifestReader& rManifestReader )
throw( DWFException )
{
    //
    // acquire a stream to the file and don't bother caching it
    // since this will probably (a) be a one-time event and
    // (b) it probably won't be very large
    //
    DWFPointer<DWFInputStream> apManifestStream( extract(L"manifest.xml", false), false );

    //
    // setup a work buffer
    //
    DWFPointer<char> apParseBuffer( DWFCORE_ALLOC_MEMORY(char, (size_t)DWFTK_XML_PARSER_BUFFER_BYTES), true );

    if (apParseBuffer.isNull())
    {
        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate work buffer" );
    }

    //
    // create a new parser
    //
    XML_Parser pManifestParser = XML_ParserCreate( NULL );

    //
    // setup the callback
    //
    XML_SetUserData( pManifestParser, (void*)&rManifestReader );

    //
    // use the static handler hooks from this class
    //
    XML_SetElementHandler( pManifestParser, StartElementHandler, EndElementHandler );
    //XML_SetNamespaceDeclHandler( pManifestParser, _StartNamespaceDeclHandler, _EndNamespaceDeclHandler );

        //
        // The reader may have a filter associated with it, if it does,
        // we need to attach the document stream so the filter can process
        // the initial bytes before Expat gets to look at them.
        // Use this mechanism to handle malformed XML from bad publishers...
        //
    DWFBufferInputStream* pFilterStream = rManifestReader.getStreamFilter();
    if (pFilterStream)
    {
        pFilterStream->chainInputStream( apManifestStream, false );
    }

    //
    // with which stream are we finally going to feed Expat?
    //
    DWFInputStream* pReadStream = (pFilterStream ? pFilterStream : (DWFInputStream*)apManifestStream);

        //
        // process the manifest stream, parsing each chunk as it's read
        //
    while (pReadStream->available() > 0)
    {
        //
        // extract next chunk from stream; try to fill our read buffer
        //
        size_t nBytesRead = 0;
        try
        {            
            nBytesRead = pReadStream->read( apParseBuffer, (size_t)DWFTK_XML_PARSER_BUFFER_BYTES );
        }
        catch(...)
        {

            if (_pPackageManifest)
            {
                // Free and NULL it out. This allows the next call to the getManifest( DWFManifestReader* pFilter )
                // overload to try again. Helpfull when re-trying due to a bad password.
                DWFCORE_FREE_OBJECT( _pPackageManifest );
            }
            //
            // release the parser
            //
            XML_ParserFree( pManifestParser );	  

            //
            // Throw the exception to the next handler. A common exception is the DWFInvalidPasswordException
            // exception. We re-throw here so higher level code can deal with it.  
            //
            throw;
        }

            //
            // there seems to be a problem with the parser's EOS
            // logic if this condition exists. the overall byte
            // count appears to have the requirement that it
            // cover the data itself and must exclude any terminating
            // marker if one exists.
            //
        if ((nBytesRead > 0) && (((char*)apParseBuffer)[nBytesRead-1] == 0))
        {
            nBytesRead--;
        }

        if (XML_Parse(pManifestParser, apParseBuffer, (int)nBytesRead, 0) == XML_STATUS_ERROR)
        {
            //
            // capture the error information
            //
            DWFString zParseError( XML_ErrorString( XML_GetErrorCode(pManifestParser)) );

            wchar_t zBuffer[32] = {0};
            _DWFCORE_SWPRINTF( zBuffer, 32, L" [line: %d]", XML_GetCurrentLineNumber(pManifestParser) );

            zParseError.append( zBuffer );

            //
            // release the parser
            //
            XML_ParserFree( pManifestParser );

            _DWFCORE_THROW( DWFIOException, zParseError );
        }
    }

    //
    // release the parser
    //
    XML_ParserFree( pManifestParser );
}

_DWFTK_API
DWFInputStream*
DWFPackageReader::extract( const DWFString& zFilename,
                           bool             bCache )
throw( DWFException )
{
        //
        // check the cache first
        //
    DWFTempFile** ppFile = _oLocalFileCache.find( zFilename );
    if (ppFile)
    {
        return (*ppFile)->getInputStream();
    }

        //
        // open up the package if necessary
        //
    DWFZipFileDescriptor* pPackageDescriptor = _open();

        //
        // make sure the file exists...
        //
    if (pPackageDescriptor->locate(zFilename) == false)
    {
        DWFCORE_FREE_OBJECT( pPackageDescriptor );

        _DWFCORE_THROW( DWFDoesNotExistException, L"Requested archive not found in DWF package" );
    }

    //
    // acquire stream for reading archive
    //
    DWFInputStream* pArchiveStream = pPackageDescriptor->unzip( zFilename, _zDWFPassword );

        //
    {
        DWFUnzippingInputStream* pUnzipStream = (DWFUnzippingInputStream*)pArchiveStream;
        pUnzipStream->attach( pPackageDescriptor, true );
    }
        // disk cache requested
        //
    if (bCache)
    {
        //
        // self-managed temp file
        //
        DWFString zTemplate( /*NOXLATE*/L"_dwfpkgreader_" );
        DWFTempFile* pCache = DWFTempFile::Create( zTemplate, true );

        //
        // create a monitor around the archive stream
        // this wrapper provides two useful bits of functionality:
        //  1. as the caller reads from his stream to extract the archive,
        //      this monitor will stream the same bytes into the temp file.
        //  2. when the caller deletes the stream pointer,
        //      the underlying archive stream will go along for the ride
        //
        DWFMonitoredInputStream* pMonitor = DWFCORE_ALLOC_OBJECT( DWFMonitoredInputStream(pArchiveStream, true) );

        if (pMonitor == NULL)
        {
            DWFCORE_FREE_OBJECT( pCache );

            _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate stream monitor" );
        }

        //
        // map the requested archive to the temp file for future lookups
        //
        _oLocalFileCache.insert( zFilename, pCache );

        //
        // attach the stream to populate the temp file
        // and don't delete the temp file's output stream...
        //
        pMonitor->attach( &(pCache->getOutputStream()), false );

        //
        // assign the outbound pointer
        //
        pArchiveStream = pMonitor;
    }

    return pArchiveStream;
}

DWFSectionBuilder&
DWFPackageReader::getSectionBuilder()
throw( DWFException )
{
    _makeSectionBuilder();

    return *_pSectionBuilder;
}

void
DWFPackageReader::_makeSectionBuilder()
throw( DWFException )
{
    if (_pSectionBuilder == NULL)
    {
        _pSectionBuilder = DWFCORE_ALLOC_OBJECT( DWFSectionBuilder );

        if (_pSectionBuilder == NULL)
        {
            _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate section builder" );
        }

        //
        // add known section factories
        //
        _pSectionBuilder->addFactory( DWFCORE_ALLOC_OBJECT(DWFEPlotSection::Factory) );
        _pSectionBuilder->addFactory( DWFCORE_ALLOC_OBJECT(DWFEModelSection::Factory) );
        _pSectionBuilder->addFactory( DWFCORE_ALLOC_OBJECT(DWFEPlotGlobalSection::Factory) );
        _pSectionBuilder->addFactory( DWFCORE_ALLOC_OBJECT(DWFEModelGlobalSection::Factory) );
    }
}

//
//
// Expat Handlers
//
//

void XMLCALL
DWFPackageReader::StartElementHandler( void*           pCallback,
                                       const char*     zName,
                                       const char**    ppAttributeList )
throw()
{
    DWFXMLCallback* pXMLCallback = static_cast<DWFXMLCallback*>(pCallback);
    pXMLCallback->notifyStartElement( zName, ppAttributeList );
}

void XMLCALL
DWFPackageReader::EndElementHandler( void*             pCallback,
                                     const char*       zName )
throw()
{
    DWFXMLCallback* pXMLCallback = static_cast<DWFXMLCallback*>(pCallback);
    pXMLCallback->notifyEndElement( zName );
}

void XMLCALL
DWFPackageReader::StartNamespaceDeclHandler( void*       pCallback,
                                             const char* zPrefix,
                                             const char* zURI )
throw()
{
    DWFXMLCallback* pXMLCallback = static_cast<DWFXMLCallback*>(pCallback);
    pXMLCallback->notifyStartNamespace( zPrefix, zURI );
}

void XMLCALL
DWFPackageReader::EndNamespaceDeclHandler( void*         pCallback,
                                           const char*   zPrefix )
throw()
{
    DWFXMLCallback* pXMLCallback = static_cast<DWFXMLCallback*>(pCallback);
    pXMLCallback->notifyEndNamespace( zPrefix );
}

void XMLCALL 
DWFPackageReader::CharacterDataHandler ( void*         pCallback,
                                         const char*   zCData,
                                         int           nLength ) 
throw()
{
     DWFXMLCallback* pXMLCallback = static_cast<DWFXMLCallback*>(pCallback);
     pXMLCallback->notifyCharacterData( zCData, nLength ); 
}


