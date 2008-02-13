//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
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


#ifndef DWFTK_READ_ONLY


#include "dwf/package/writer/DWFXMLSerializer.h"
using namespace DWFToolkit;


#define _DWFTK_DWFXMLSERIALIZER_WRITE( s )                              \
    _pStream->encode( false );                                          \
    _pStream->writeXML( s, DWFCORE_WIDE_STRING_LENGTH_IN_BYTES(s) );    \
    _pStream->encode( true );

#define _DWFTK_DWFXMLSERIALIZER_WRITE_DWFSTRING( s )                    \
    {                                                                   \
        size_t n = s.bytes();                                           \
        if (n > 0)                                                      \
        {                                                               \
            _pStream->writeXML( s, n );                                 \
        }                                                               \
    }


_DWFTK_API
DWFXMLSerializer::DWFXMLSerializer( DWFUUID& rUUID )
throw()
                : _rUUID( rUUID )
                , _pStream( NULL )
                , _bLastElementOpen( false )
                , _pLastElement( NULL )
{
    ;
}

_DWFTK_API
DWFXMLSerializer::~DWFXMLSerializer()
throw()
{
    detach();
}

_DWFTK_API
const DWFString&
DWFXMLSerializer::nextUUID( bool bSquash )
throw()
{
    return _rUUID.next( bSquash );
}

_DWFTK_API
void
DWFXMLSerializer::attach( DWFOutputStream& rStream )
throw( DWFException )
{
    detach();

    _pStream = DWFCORE_ALLOC_OBJECT( XMLOutputStream(rStream) );

    if (_pStream == NULL)
    {
        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate buffered stream" );
    }
}

_DWFTK_API
void
DWFXMLSerializer::detach()
throw( DWFException )
{
    if (_pStream)
    {
        _pStream->flush();
        DWFCORE_FREE_OBJECT( _pStream );
    }

    _pStream = NULL;
}

_DWFTK_API
void
DWFXMLSerializer::startElement( const DWFString& zName, const DWFString& zNamespace )
throw( DWFException )
{
    if (_pStream == NULL)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"No output stream attached" );
    }

        //
        // close off the previously starting element
        //
    if (_bLastElementOpen)
    {
        _DWFTK_DWFXMLSERIALIZER_WRITE( /*NOXLATE*/L">" );
    }

    //
    // this string is needed in the case that the element
    // has to be explicitly closed off
    //
    DWFString zFull( zNamespace );
              zFull.append( zName );

    _DWFTK_DWFXMLSERIALIZER_WRITE( /*NOXLATE*/L"<" );
    _DWFTK_DWFXMLSERIALIZER_WRITE_DWFSTRING( zFull );

    _oDepthChart.push_back( zFull );
    _pLastElement = &(_oDepthChart.back());

    //
    // will close this element with ">" before opening new one
    //
    _bLastElementOpen = true;
}

_DWFTK_API
void
DWFXMLSerializer::addAttribute( const DWFString& zName, const DWFString& zValue, const DWFString& zNamespace )
throw( DWFException )
{
    if (_pStream == NULL)
    {
        _DWFCORE_THROW( DWFUnexpectedException, /*NOXLATE*/L"No output stream attached" );
    }

        //
        // skip it if there is no value string,
        // don't add any extra bytes to the file
        //
    if (zValue.chars() > 0)
    {
        _DWFTK_DWFXMLSERIALIZER_WRITE( /*NOXLATE*/L" " );
        _DWFTK_DWFXMLSERIALIZER_WRITE_DWFSTRING( zNamespace );
        _DWFTK_DWFXMLSERIALIZER_WRITE_DWFSTRING( zName );
        _DWFTK_DWFXMLSERIALIZER_WRITE( /*NOXLATE*/L"=\"" );
        _DWFTK_DWFXMLSERIALIZER_WRITE_DWFSTRING( zValue );
        _DWFTK_DWFXMLSERIALIZER_WRITE( /*NOXLATE*/L"\"" );
    }
}

_DWFTK_API
 void DWFXMLSerializer::addCData( const DWFString& zCData )
 throw( DWFException )
{
    if (_pStream == NULL)
    {
        _DWFCORE_THROW( DWFUnexpectedException, /*NOXLATE*/L"No output stream attached" );
    }
    
        //
        // close off the previously starting element
        //
    if (_bLastElementOpen)
    {
        _DWFTK_DWFXMLSERIALIZER_WRITE( /*NOXLATE*/L">" );
        _pLastElement = NULL;

        _DWFTK_DWFXMLSERIALIZER_WRITE_DWFSTRING( zCData );
        
        _bLastElementOpen = false;
    }
    else 
    {
        _DWFCORE_THROW( DWFUnexpectedException, /*NOXLATE*/L"No open element" );
    }
}

_DWFTK_API
void
DWFXMLSerializer::endElement()
throw( DWFException )
{
    if (_pStream == NULL)
    {
        _DWFCORE_THROW( DWFUnexpectedException, /*NOXLATE*/L"No output stream attached" );
    }

        //
        // closing off the same element, use simple bracket
        //
    const DWFString& rElement = _oDepthChart.back();
    if (_pLastElement && (rElement == *_pLastElement))
    {
        _pLastElement = NULL;
        _DWFTK_DWFXMLSERIALIZER_WRITE( /*NOXLATE*/L"/>" );
    }
        //
        // otherwise, we need to explicitly declare the element
        //
    else
    {
        _DWFTK_DWFXMLSERIALIZER_WRITE( /*NOXLATE*/L"</" );
        _DWFTK_DWFXMLSERIALIZER_WRITE_DWFSTRING( rElement );
        _DWFTK_DWFXMLSERIALIZER_WRITE( /*NOXLATE*/L">" );
    }

    //
    // remove last element
    //
    _oDepthChart.pop_back();

    //
    // prevents extra ">"
    //
    _bLastElementOpen = false;
}

///
///
///

DWFXMLSerializer::XMLOutputStream::XMLOutputStream( DWFOutputStream& rOutputStream )
throw()
                : DWFBufferOutputStream( &rOutputStream, false, DWFTK_XMLSERIALIZER_WRITE_BUFFER_BYTES )
                , _bXML( true )
                , _oXMLStream( NULL, false, true )
{
    ;
}

DWFXMLSerializer::XMLOutputStream::~XMLOutputStream()
throw()
{
    ;
}

size_t
DWFXMLSerializer::XMLOutputStream::writeXML( const void*   pBuffer,
                                             size_t        nBytesToWrite )
throw( DWFException )
{
        //
        // this set up will perform a simple XML special character
        // encoding and then send the result through the UTF-8 encoder
        //
    if (_bXML)
    {
        _oUTF8Stream.attach( this, false );
        _oXMLStream.attach( &_oUTF8Stream, false );

        return _oXMLStream.write( pBuffer, nBytesToWrite );
    }
        //
        // UTF-8 encode only
        //
    else
    {
        _oUTF8Stream.attach( this, false );

        return _oUTF8Stream.write( pBuffer, nBytesToWrite );
    }
}

size_t
DWFXMLSerializer::XMLOutputStream::write( const void*   pBuffer,
                                          size_t        nBytesToWrite )
throw( DWFException )
{
    return DWFBufferOutputStream::write( pBuffer, nBytesToWrite );
}


#endif

