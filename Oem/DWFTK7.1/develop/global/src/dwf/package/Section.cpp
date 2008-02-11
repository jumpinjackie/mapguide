//
//  Copyright (C) 2003-2008 by Autodesk, Inc.  All rights reserved.
//
// Permission to use, copy, modify, and distribute this software in
// object code form for any purpose and without fee is hereby granted,
// provided that the above copyright notice appears in all copies and
// that both that copyright notice and the limited warranty and
// restricted rights notice below appear in all supporting
// documentation.
//
// AUTODESK PROVIDES THIS PROGRAM 'AS IS' AND WITH ALL FAULTS.
// AUTODESK SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTY OF
// MERCHANTABILITY OR FITNESS FOR A PARTICULAR USE.  AUTODESK, INC.
// DOES NOT WARRANT THAT THE OPERATION OF THE PROGRAM WILL BE
// UNINTERRUPTED OR ERROR FREE.
//
// Use, duplication, or disclosure by the U.S. Government is subject to
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//
// $Header: /Components/Internal/DWF Toolkit/v7.1/develop/global/src/dwf/package/Section.cpp 26    8/11/05 8:38p Kuk $
//


#include "dwfcore/Pointer.h"
using namespace DWFCore;

#include "dwf/Version.h"
#include "dwf/package/Source.h"
#include "dwf/package/Section.h"
#include "dwf/package/Constants.h"
#include "dwf/package/reader/PackageManifestReader.h"
#include "dwf/package/reader/EPlotSectionDescriptorReader.h"
#include "dwf/package/reader/EModelSectionDescriptorReader.h"
#include "dwf/package/reader/GlobalSectionDescriptorReader.h"
#include "dwf/package/writer/DWFXMLSerializer.h"
using namespace DWFToolkit;



_DWFTK_API
DWFSection::DWFSection( DWFPackageReader* pPackageReader )
throw()
          : _nVersion( 0.0 )
          , _nPlotOrder( 0.0 )
	      , _pPackageReader( pPackageReader )
{
    ;
}

_DWFTK_API
DWFSection::DWFSection( const DWFString&  zType,
                        const DWFString&  zName,
                        const DWFString&  zTitle,
                        DWFPackageReader* pPackageReader )
throw()
          : _zType( zType )
          , _zName( zName )
          , _zTitle( zTitle )
          , _nVersion( 0.0 )
          , _nPlotOrder( -1 )
	      , _pPackageReader( pPackageReader )
{
    ;
}

_DWFTK_API
DWFSection::DWFSection( const DWFString& zType,
                        const DWFString& zTitle,
                        const DWFString& zObjectID,
                        double           nVersion,
                        double           nPlotOrder,
                        const DWFSource& rSource )
throw()
          : _zType( zType )
          , _zTitle( zTitle )
          , _zObjectID( zObjectID )
          , _nVersion( nVersion )
          , _nPlotOrder( nPlotOrder )
          , _oSource( rSource )
          , _pPackageReader( NULL )
{
    ;
}

_DWFTK_API
DWFSection::DWFSection( const DWFSection& rSection )
throw()
          : _zType( rSection._zType )
          , _zName( rSection._zName )
          , _zTitle( rSection._zTitle )
          , _zObjectID( rSection._zObjectID )
          , _nVersion( rSection._nVersion )
          , _nPlotOrder( rSection._nPlotOrder )
          , _oSource( rSection._oSource )
          , _pPackageReader( rSection._pPackageReader )
{
    ;
}

_DWFTK_API
DWFSection&
DWFSection::operator=( const DWFSection& rSection )
throw()
{
    _zType = rSection._zType;
    _zName = rSection._zName;
    _zTitle = rSection._zTitle;
    _zObjectID = rSection._zObjectID;
    _nVersion = rSection._nVersion;
    _nPlotOrder = rSection._nPlotOrder;
    _oSource = rSection._oSource;
    _pPackageReader = rSection._pPackageReader;

    return *this;
}

_DWFTK_API
DWFSection::~DWFSection()
throw()
{
    //
    // requirement of the DWFOwnable contract
    //
    DWFOwnable::_notifyDelete();
}

_DWFTK_API
void
DWFSection::addSource( const DWFSource& rSource )
throw()
{
    _oSource = rSource;
}

_DWFTK_API
void
DWFSection::rename( const DWFString& zName )
throw()
{
    _zName.assign( _zType );
    _zName.append( /*NOXLATE*/L"_" );
    _zName.append( zName );
}

_DWFTK_API
const DWFResource&
DWFSection::readDescriptor( DWFSectionDescriptorReader* pSectionDescriptorReader )
const
throw( DWFException )
{
        //
        // we need a reader at this level
        //
    if (pSectionDescriptorReader == NULL)
    {
        _DWFCORE_THROW( DWFInvalidArgumentException, /*NOXLATE*/L"A section descriptor reader interface must be provided" );
    }

    //
    // find the one resource with the role of "descriptor"
    //
    DWFResource::tMultiMap::const_iterator iResource = _oResourcesByRole.find( DWFXML::kzRole_Descriptor );
    if (iResource == _oResourcesByRole.end())
    {
        _DWFCORE_THROW( DWFDoesNotExistException, L"This section does not have a descriptor" );
    }

    //
    // acquire the data feed from the resource directly
    // don't cache since hopefully the descriptor is read
    // only once and any useful data is collected by the
    // implementation of the reader
    //
    DWFPointer<DWFInputStream> apDescriptorStream( (*iResource).second->getInputStream(), false );

    //
    // parse the descriptor
    //
    readDescriptor( *pSectionDescriptorReader, *(DWFInputStream*)apDescriptorStream );

    //
    // return the resource
    //
    return *((*iResource).second);
}

_DWFTK_API
void
DWFSection::readDescriptor( DWFSectionDescriptorReader& rSectionDescriptorReader,
                            DWFResource&                rResource ) const
throw( DWFException )
{
    //
    // acquire the data feed from the resource directly
    // don't cache since hopefully the descriptor is read
    // only once and any useful data is collected by the
    // implementation of the reader
    //
    DWFPointer<DWFInputStream> apDescriptorStream( rResource.getInputStream(), false );

    //
    // parse the descriptor
    //
    readDescriptor( rSectionDescriptorReader, *(DWFInputStream*)apDescriptorStream );
}

_DWFTK_API
void
DWFSection::readDescriptor( DWFSectionDescriptorReader& rSectionDescriptorReader,
                            DWFInputStream&             rSectionDescriptorStream ) const
throw( DWFException )
{
    //
    // parse the descriptor
    //
    _parseDocument( rSectionDescriptorStream, rSectionDescriptorReader );
}

_DWFTK_API
DWFObjectDefinition*
DWFSection::getObjectDefinition()
const
throw( DWFException )
{
    DWFObjectDefinition* pDefs = DWFCORE_ALLOC_OBJECT( DWFObjectDefinition(_pPackageReader) );

    if (pDefs == NULL)
    {
        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate instance tree" );
    }

        //
        // no leaks
        //
    try
    {
        getObjectDefinition( *pDefs, NULL );
        return pDefs;
    }
    catch (...)
    {
        DWFCORE_FREE_OBJECT( pDefs );
        throw;
    }
}

_DWFTK_API
DWFObjectDefinition*
DWFSection::getObjectDefinition( DWFObjectDefinitionReader& rObjectDefinitionFilter,
                                 DWFResource&               rResource )
const
throw( DWFException )
{
    DWFObjectDefinition* pDefs = DWFCORE_ALLOC_OBJECT( DWFObjectDefinition(_pPackageReader) );

    if (pDefs == NULL)
    {
        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate instance tree" );
    }

    pDefs->setFilter(&rObjectDefinitionFilter);

        //
        // no leaks
        //
    try
    {
        getObjectDefinition( *pDefs, &rResource );
        return pDefs;
    }
    catch (...)
    {
        DWFCORE_FREE_OBJECT( pDefs );
        throw;
    }
}

_DWFTK_API
void
DWFSection::getObjectDefinition( DWFObjectDefinitionReader& rObjectDefinitionReader,
                                 DWFResource*               pResource )
const
throw( DWFException )
{
    if (pResource)
    {
        //
        // acquire the data feed from the resource directly
        // don't cache since hopefully the document is read
        // only once and any useful data is collected by the
        // implementation of the reader
        //
        DWFPointer<DWFInputStream> apObjectStream( pResource->getInputStream(), false );

        //
        // parse the descriptor
        //
        getObjectDefinition( rObjectDefinitionReader, *(DWFInputStream*)apObjectStream );
    }
    else
    {
        //
        // find all resources with the role of "object definition"
        //
        DWFResource::tMultiMap::const_iterator iResource = _oResourcesByRole.begin();
        DWFResource::tMultiMap::const_iterator iEnd = _oResourcesByRole.end();

        for (; iResource != iEnd; iResource++)
        {
            const DWFString& zRole = iResource->second->role();
            if (zRole == DWFXML::kzRole_ObjectDefinition
                || zRole == DWFXML::kzRole_MarkupObjectDefinition)
            {
                //
                // acquire the data feed from the resource directly
                // don't cache since hopefully the document is read
                // only once and any useful data is collected by the
                // implementation of the reader
                //
                DWFPointer<DWFInputStream> apObjectStream( iResource->second->getInputStream(), false );

                //
                // parse the object definition
                //
                getObjectDefinition( rObjectDefinitionReader, *(DWFInputStream*)apObjectStream );
            }
        }
    }
}

_DWFTK_API
void
DWFSection::getObjectDefinition( DWFObjectDefinitionReader& rObjectDefinitionReader,
                                 DWFInputStream&            rObjectDefinitionStream ) const
throw( DWFException )
{
    //
    // parse the object definition
    //
    _parseDocument( rObjectDefinitionStream, rObjectDefinitionReader );
}

void
DWFSection::_parseDocument( DWFInputStream& rDocumentStream,
                            DWFXMLCallback& rDocumentReader )
const
throw( DWFException )
{
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
    XML_Parser pDescriptorParser = XML_ParserCreate( NULL );

    //
    // setup the callback
    //
    XML_SetUserData( pDescriptorParser, (void*)&rDocumentReader );

    //
    // use the static handler hooks from the DWFPackageReader
    //
    XML_SetElementHandler( pDescriptorParser, DWFPackageReader::StartElementHandler, DWFPackageReader::EndElementHandler );

    //
    // use the static handler hooks from the DWFPackageReader
    // this is unused (to date) by the standard DWF sections,
    // however, it is provided nonetheless for extensibility
    //
    XML_SetCharacterDataHandler( pDescriptorParser, DWFPackageReader::CharacterDataHandler );

        //
        // The reader may have a filter associated with it, if it does,
        // we need to attach the document stream so the filter can process
        // the initial bytes before Expat gets to look at them.
        // Use this mechanism to handle malformed XML from bad publishers...
        //
    DWFBufferInputStream* pFilterStream = rDocumentReader.getStreamFilter();
    if (pFilterStream)
    {
        pFilterStream->chainInputStream( &rDocumentStream, false );
    }

    //
    // with which stream are we finally going to feed Expat?
    //
    DWFInputStream* pReadStream = (pFilterStream ? pFilterStream : &rDocumentStream);

        //
        // process the manifest stream, parsing each chunk as it's read
        //
    while (pReadStream->available() > 0)
    {
        //
        // extract next chunk from stream; try to fill the entire read buffer
        //
        size_t nBytesRead = pReadStream->read( apParseBuffer, (size_t)DWFTK_XML_PARSER_BUFFER_BYTES );

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

        if (XML_Parse(pDescriptorParser, apParseBuffer, (int)nBytesRead, 0) == XML_STATUS_ERROR)
        {
            //
            // capture the error information
            //
            DWFString zParseError( XML_ErrorString( XML_GetErrorCode(pDescriptorParser)) );

            wchar_t zBuffer[32] = {0};
            _DWFCORE_SWPRINTF( zBuffer, 32, /*NOXLATE*/L" [line: %d]", XML_GetCurrentLineNumber(pDescriptorParser) );

            zParseError.append( zBuffer );

            //
            // release the parser
            //
            XML_ParserFree( pDescriptorParser );

            _DWFCORE_THROW( DWFIOException, zParseError );
        }
    }

    //
    // release the parser
    //
    XML_ParserFree( pDescriptorParser );
}

_DWFTK_API
void
DWFSection::parseAttributeList( const char** ppAttributeList )
throw( DWFException )
{
    if (ppAttributeList == NULL)
    {
        _DWFCORE_THROW( DWFInvalidArgumentException, L"No attributes provided" );
    }

    unsigned char nFound = 0;
    size_t iAttrib = 0;
    const char* pAttrib = NULL;

    for(; ppAttributeList[iAttrib]; iAttrib += 2)
    {
        //
        // skip over any "dwf:" in the attribute name
        //
        pAttrib = (DWFCORE_COMPARE_MEMORY(DWFXML::kzNamespace_DWF, ppAttributeList[iAttrib], 4) == 0) ?
                  &ppAttributeList[iAttrib][4] :
                  &ppAttributeList[iAttrib][0];

            //
            // set the name
            //
        if (!(nFound & 0x01) &&
             (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_Name) == 0))
        {
            nFound |= 0x01;

            _zName.assign( ppAttributeList[iAttrib+1] );
        }
            //
            // set the type
            //
        else if (!(nFound & 0x02) &&
                  (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_Type) == 0))
        {
            nFound |= 0x02;

            _zType.assign( ppAttributeList[iAttrib+1] );
        }
            //
            // set the title
            //
        else if (!(nFound & 0x04) &&
                  (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_Title) == 0))
        {
            nFound |= 0x04;

            _zTitle.assign( ppAttributeList[iAttrib+1] );
        }
    }
}

#ifndef DWFTK_READ_ONLY

_DWFTK_API
void
DWFSection::serializeXML( DWFXMLSerializer& rSerializer, unsigned int nFlags )
throw( DWFException )
{
    //
    // ensure that the resource container knows the section name
    // so that our resources have the correct HREFs
    //
    DWFResourceContainer::rename( _zName );

        //
        // detail the section in the manifest
        //
    if (nFlags & DWFPackageWriter::eManifest)
    {
        //
        // root element
        //
        rSerializer.startElement( DWFXML::kzElement_Section, DWFXML::kzNamespace_DWF );
        {
                //
                // attributes
                //
            {
                rSerializer.addAttribute( DWFXML::kzAttribute_Type, _zType );
                rSerializer.addAttribute( DWFXML::kzAttribute_Name, _zName );
                rSerializer.addAttribute( DWFXML::kzAttribute_Title, _zTitle );
            }

            //
            // source
            //
            _oSource.serializeXML( rSerializer, nFlags );

            //
            // resources
            //
            DWFResourceContainer::getSerializable().serializeXML( rSerializer, nFlags );
        }
        rSerializer.endElement();
    }
    else if (nFlags & DWFPackageWriter::eDescriptor)
    {
        //
        // properties
        //
        DWFPropertyContainer::getSerializable().serializeXML( rSerializer, nFlags );

        //
        // resources
        //
        DWFResourceContainer::getSerializable().serializeXML( rSerializer, nFlags );
    }
}

#endif


///
///
///

_DWFTK_API
DWFSection::Factory::Factory( const DWFString& zType )
throw()
                   : _zType( zType )
{
    ;
}

_DWFTK_API
DWFSection::Factory::~Factory()
throw()
{
    ;
}

_DWFTK_API
const DWFString&
DWFSection::Factory::type()
const
throw()
{
    return _zType;
}

_DWFTK_API
DWFSection*
DWFSection::Factory::build( const DWFString&     zName,
                            const DWFString&     zTitle,
                            DWFPackageReader*    pPackageReader )
throw( DWFException )
{
    DWFSection* pSection = DWFCORE_ALLOC_OBJECT( DWFSection(_zType, zName, zTitle, pPackageReader) );

    if (pSection == NULL)
    {
        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate new section" );
    }

    return pSection;
}

_DWFTK_API
DWFSection*
DWFSection::Factory::build( const DWFString&     zType,
                            const DWFString&     zName,
                            const DWFString&     zTitle,
                            DWFPackageReader*    pPackageReader )
throw( DWFException )
{
    DWFSection* pSection = DWFCORE_ALLOC_OBJECT( DWFSection(zType, zName, zTitle, pPackageReader) );

    if (pSection == NULL)
    {
        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate new section" );
    }

    return pSection;
}

///
///
///

DWFResource*
DWFSection::buildResource( const char**      ppAttributeList,
                           DWFPackageReader* pPackageReader )
throw( DWFException )
{
    DWFResource* pResource = DWFXMLElementBuilder::buildResource( ppAttributeList, pPackageReader );

    //
    // see if this resource matches one we already have
    //
    DWFResource* pContainerResource = findResourceByHREF( pResource->href() );

        //
        // remove the old one and have it deleted
        //
    if (pContainerResource)
    {
        removeResource( *pContainerResource, true );
    }

    return pResource;
}

DWFFontResource*
DWFSection::buildFontResource( const char**      ppAttributeList,
                               DWFPackageReader* pPackageReader )
throw( DWFException )
{
    DWFFontResource* pResource = DWFXMLElementBuilder::buildFontResource( ppAttributeList, pPackageReader );

    //
    // see if this resource matches one we already have
    //
    DWFResource* pContainerResource = findResourceByHREF( pResource->href() );

        //
        // remove the old one and have it deleted
        //
    if (pContainerResource)
    {
        removeResource( *pContainerResource, true );
    }

    return pResource;
}

DWFImageResource*
DWFSection::buildImageResource( const char**      ppAttributeList,
                                DWFPackageReader* pPackageReader )
throw( DWFException )
{
    DWFImageResource* pResource = DWFXMLElementBuilder::buildImageResource( ppAttributeList, pPackageReader );

    //
    // see if this resource matches one we already have
    //
    DWFResource* pContainerResource = findResourceByHREF( pResource->href() );

        //
        // remove the old one and have it deleted
        //
    if (pContainerResource)
    {
        removeResource( *pContainerResource, true );
    }

    return pResource;
}

DWFGraphicResource*
DWFSection::buildGraphicResource( const char**      ppAttributeList,
                                  DWFPackageReader* pPackageReader )
throw( DWFException )
{
    DWFGraphicResource* pResource = DWFXMLElementBuilder::buildGraphicResource( ppAttributeList, pPackageReader );

    //
    // see if this resource matches one we already have
    //
    DWFResource* pContainerResource = findResourceByHREF( pResource->href() );

        //
        // remove the old one and have it deleted
        //
    if (pContainerResource)
    {
        removeResource( *pContainerResource, true );
    }

    return pResource;
}

