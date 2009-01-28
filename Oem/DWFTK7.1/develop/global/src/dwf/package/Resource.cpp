//
//  Copyright (C) 1996-2009 by Autodesk, Inc.
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
// restrictions set forth in FAR 52.227-19 (Commercial Computer
// Software - Restricted Rights) and DFAR 252.227-7013(c)(1)(ii)
// (Rights in Technical Data and Computer Software), as applicable.
//
// $Header: /Components/Internal/DWF Toolkit/v7.1/develop/global/src/dwf/package/Resource.cpp 13    8/11/05 7:56p Kuk $
//


#include "dwfcore/MIME.h"
using namespace DWFCore;

#include "dwf/package/Resource.h"
#include "dwf/package/Constants.h"
#include "dwf/package/reader/PackageManifestReader.h"
#include "dwf/package/writer/DWFXMLSerializer.h"
using namespace DWFToolkit;

    
    
_DWFTK_API
DWFResource::DWFResource( DWFPackageReader* pPackageReader )
throw()
           : _nSize( 0 )
           , _pStream( NULL )
           , _pPackageReader( pPackageReader )
{
    ;
}

_DWFTK_API
DWFResource::DWFResource( const DWFString& zTitle,
                          const DWFString& zRole,
                          const DWFString& zMIME,
                          const DWFString& zHREF )
throw()
           : _nSize( 0 )
           , _zTitle( zTitle )
           , _zRole( zRole )
           , _zMIME( zMIME )
           , _zHRef( zHREF )
           , _pStream( NULL )
           , _pPackageReader( NULL )
{
    ;
}

_DWFTK_API
DWFResource::~DWFResource()
throw()
{
    // TODO: when the "set" stuff is fixed, if _pStream is deletable - be sure to do it here

    //
    // requirement of the DWFOwnable contract
    //
    DWFOwnable::_notifyDelete();
}

_DWFTK_API
DWFInputStream*
DWFResource::getInputStream( bool bCache )
throw( DWFException )
{
        //
        // return the "set" stream
        // TODO: fix this so it's not inconsistent
        //
    if (_pStream)
    {
        //
        // since the caller must delete this pointer,
        // we must ensure that it is returned only once
        //
        DWFInputStream* pStream = _pStream;
        _pStream = NULL;

        return pStream;
    }
        //
        // always a new stream
        //
    else if (_pPackageReader)
    {
        return _pPackageReader->extract( _zHRef, bCache );
    }
    else
    {
        return NULL;
    }
}

#ifndef DWFTK_READ_ONLY

_DWFTK_API
void
DWFResource::setInputStream( DWFInputStream* pStream, size_t nBytes )
throw( DWFException )
{
    _nSize = nBytes;
    _pStream = pStream;

    if ((_nSize == 0) && _pStream)
    {
        _nSize = _pStream->available();
    }
}

#endif

_DWFTK_API
void
DWFResource::setPublishedIdentity( const DWFString& zSectionName,
                                   const DWFString& zObjectID )
throw()
{
        //
        // keep the original object ID or create a new one
        // either way, only the incoming ID is used for making the HREF!
        //
    if (_zObjectID.bytes() == 0)
    {
        _zObjectID.assign( zObjectID );
    }

    _zTargetHRef.assign( zSectionName );
    _zTargetHRef.append( /*NOXLATE*/L"\\" );
    _zTargetHRef.append( zObjectID );

    DWFString zExtension = DWFMIME::GetExtension(_zMIME);
    if (zExtension.bytes())
    {
        _zTargetHRef.append( /*NOXLATE*/L"." );
        _zTargetHRef.append( zExtension );
    }
}

_DWFTK_API
void
DWFResource::setParentResource( const DWFResource* pResource )
throw()
{
    if (pResource)
    {
        _zParentObjectID.assign( pResource->objectID() );
    }
    else
    {
        _zParentObjectID.destroy();
    }
}

_DWFTK_API
void
DWFResource::parseAttributeList( const char** ppAttributeList )
throw( DWFException )
{
    if (ppAttributeList == NULL)
    {
        _DWFCORE_THROW( DWFInvalidArgumentException, /*NOXLATE*/L"No attributes provided" );
    }

    unsigned char nFound = 0;
    size_t iAttrib = 0;
    const char* pAttrib = NULL;

    for(; ppAttributeList[iAttrib]; iAttrib += 2)
    {
            //
            // skip over any acceptable prefixes in the element name
            //
        if (DWFCORE_COMPARE_MEMORY(DWFXML::kzNamespace_DWF, ppAttributeList[iAttrib], 4) == 0)
        {
            pAttrib = &ppAttributeList[iAttrib][4];
        }
        else if (DWFCORE_COMPARE_MEMORY(DWFXML::kzNamespace_ECommon, ppAttributeList[iAttrib], 8) == 0)
        {
            pAttrib = &ppAttributeList[iAttrib][8];
        }
        else if (DWFCORE_COMPARE_MEMORY(DWFXML::kzNamespace_EPlot, ppAttributeList[iAttrib], 6) == 0)
        {
            pAttrib = &ppAttributeList[iAttrib][6];
        }
        else if (DWFCORE_COMPARE_MEMORY(DWFXML::kzNamespace_EModel, ppAttributeList[iAttrib], 7) == 0)
        {
            pAttrib = &ppAttributeList[iAttrib][7];
        }
        else
        {
            pAttrib = &ppAttributeList[iAttrib][0];
        }

            //
            // set the role
            //
        if (!(nFound & 0x01) &&
             (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_Role) == 0))
        {
            nFound |= 0x01;

            _zRole.assign( ppAttributeList[iAttrib+1] );
        }
            //
            // set the href
            //
        else if (!(nFound & 0x02) &&
                  (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_HRef) == 0))
        {
            nFound |= 0x02;

            _zHRef.assign( ppAttributeList[iAttrib+1] );
        }
            //
            // set the MIME type
            //
        else if (!(nFound & 0x04) &&
                  (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_MIME) == 0))
        {
            nFound |= 0x04;

            _zMIME.assign( ppAttributeList[iAttrib+1] );
        }
            //
            // set the title
            //
        else if (!(nFound & 0x08) &&
                  (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_Title) == 0))
        {
            nFound |= 0x08;

            _zTitle.assign( ppAttributeList[iAttrib+1] );
        }
            //
            // set the size
            //
        else if (!(nFound & 0x10) &&
                  (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_Size) == 0))
        {
            nFound |= 0x10;

            _nSize = ::atoi( ppAttributeList[iAttrib+1] );
        }
            //
            // set the object ID
            //
        else if (!(nFound & 0x20) &&
                  (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_ObjectID) == 0))
        {
            nFound |= 0x20;

            _zObjectID.assign( ppAttributeList[iAttrib+1] );
        }
            //
            // set the object ID
            //
        else if (!(nFound & 0x40) &&
                  (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_ParentObjectID) == 0))
        {
            nFound |= 0x40;

            _zParentObjectID.assign( ppAttributeList[iAttrib+1] );
        }
    }
}


#ifndef DWFTK_READ_ONLY

_DWFTK_API
void
DWFResource::serializeXML( DWFXMLSerializer& rSerializer, unsigned int nFlags )
throw( DWFException )
{
    DWFString zNamespace;

        //
        // namespace dictated by document and section type
        //
    if (nFlags & DWFPackageWriter::eDescriptor)
    {
        zNamespace.assign( namespaceXML(nFlags) );
    }
    else
    {
        zNamespace.assign( DWFXML::kzNamespace_DWF );
    }

    DWFString zHRef( (_zTargetHRef.bytes() > 0) ? _zTargetHRef : _zHRef );

        //
        // serialize for manifest
        // (this is an abbreviated attribute list)
        //
    if (nFlags & DWFPackageWriter::eManifest)
    {
        //
        // root element
        //
        rSerializer.startElement( DWFXML::kzElement_Resource, zNamespace );
        {
                //
                // attributes
                //
            {
                rSerializer.addAttribute( DWFXML::kzAttribute_Role, _zRole );
                rSerializer.addAttribute( DWFXML::kzAttribute_MIME, _zMIME );
                rSerializer.addAttribute( DWFXML::kzAttribute_HRef,  zHRef );
            }
        }
        rSerializer.endElement();
    }
        //
        // serialize in detail for descriptor
        // unless this resource is the descriptor itself...
        //
    else if ((nFlags & DWFPackageWriter::eDescriptor) &&
             (_zRole != DWFXML::kzRole_Descriptor))
    {

            //
            // if the derived class has already open the element
            // we can only serialize the base attributes here
            //
        if ((nFlags & DWFXMLSerializer::eElementOpen) == 0)
        {
            rSerializer.startElement( DWFXML::kzElement_Resource, zNamespace );
        }

        {
                //
                // common attributes
                //
            {
                rSerializer.addAttribute( DWFXML::kzAttribute_Role, _zRole );
                rSerializer.addAttribute( DWFXML::kzAttribute_MIME, _zMIME );
                rSerializer.addAttribute( DWFXML::kzAttribute_HRef,  zHRef );
                rSerializer.addAttribute( DWFXML::kzAttribute_Title, _zTitle );

                    //
                    // optional anyway, don't bother if zero
                    //
                if (_nSize > 0)
                {
                    wchar_t zTempBuffer[32];
                    _DWFCORE_SWPRINTF( zTempBuffer, 32, /*NOXLATE*/L"%d", _nSize );

                    rSerializer.addAttribute( DWFXML::kzAttribute_Size, zTempBuffer );
                }

                rSerializer.addAttribute( DWFXML::kzAttribute_ObjectID, _zObjectID );
                rSerializer.addAttribute( DWFXML::kzAttribute_ParentObjectID, _zParentObjectID );
            }

                //
                // if the derived class has already open the element
                // we can only serialize the base attributes here,
                // also, object definition resources, shouldn't seriailize their
                // properties here as they will be detailed in those documents.
                //
            if (((nFlags & DWFXMLSerializer::eElementOpen) == 0) &&
                (_zRole != DWFXML::kzRole_ObjectDefinition)      &&
                (_zRole != DWFXML::kzRole_MarkupObjectDefinition))
            {
                //
                // properties
                //
                DWFXMLSerializable& rSerializable = DWFPropertyContainer::getSerializable();
                rSerializable.serializeXML( rSerializer, nFlags );
            }
        }

        if ((nFlags & DWFXMLSerializer::eElementOpen) == 0)
        {
            rSerializer.endElement();
        }
    }
}

#endif

