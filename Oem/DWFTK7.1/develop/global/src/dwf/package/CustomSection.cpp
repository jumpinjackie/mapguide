//
//  Copyright (C) 2005-2009 by Autodesk, Inc.  All rights reserved.
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
// $Header: /Components/Internal/DWF Toolkit/v7/develop/global/src/dwf/package/CustomSection.cpp 1     1/25/05 5:30p Evansg $
//


#include "dwf/package/CustomSection.h"
using namespace DWFToolkit;



_DWFTK_API
DWFCustomSection::DWFCustomSection( DWFPackageReader* pPackageReader )
throw()
                : DWFSection( pPackageReader )
{
    _tBehavior.bRenameOnPublish = true;
    _tBehavior.bPublishDescriptor = false;
    _tBehavior.bPublishResourcesToManifest = false;
}

_DWFTK_API
DWFCustomSection::DWFCustomSection( const DWFString&  zType,
                                    const DWFString&  zName,
                                    const DWFString&  zTitle,
                                    DWFPackageReader* pPackageReader )
throw()
                : DWFSection( zType, zName, zTitle, pPackageReader )
{
    _tBehavior.bRenameOnPublish = true;
    _tBehavior.bPublishDescriptor = false;
    _tBehavior.bPublishResourcesToManifest = false;
}

_DWFTK_API
DWFCustomSection::DWFCustomSection( const DWFString& zType,
                                    const DWFString& zTitle,
                                    const DWFString& zObjectID,
                                    double           nVersion,
                                    double           nPlotOrder,
                                    const DWFSource& rSource )
throw()
                : DWFSection( zType, 
                              zTitle, 
                              zObjectID, 
                              nVersion, 
                              nPlotOrder, 
                              rSource )
{
    _tBehavior.bRenameOnPublish = true;
    _tBehavior.bPublishDescriptor = false;
    _tBehavior.bPublishResourcesToManifest = false;
}

_DWFTK_API
DWFCustomSection::DWFCustomSection( const DWFCustomSection& rSection )
throw()
                : DWFSection( rSection )
{
    DWFCORE_COPY_MEMORY( &_tBehavior, &(rSection._tBehavior), sizeof(DWFCustomSection::tBehavior) );
}

_DWFTK_API
DWFCustomSection&
DWFCustomSection::operator=( const DWFCustomSection& rSection )
throw()
{
    DWFSection::operator=( rSection );
    DWFCORE_COPY_MEMORY( &_tBehavior, &(rSection._tBehavior), sizeof(DWFCustomSection::tBehavior) );

    return *this;
}

_DWFTK_API
DWFCustomSection::~DWFCustomSection()
throw()
{
    ;
}

_DWFTK_API
DWFCustomSection::tBehavior
DWFCustomSection::behavior() const
throw()
{
    return _tBehavior;
}

_DWFTK_API
void
DWFCustomSection::applyBehavior( const DWFCustomSection::tBehavior& rBehavior )
throw()
{
    DWFCORE_COPY_MEMORY( &_tBehavior, &rBehavior, sizeof(DWFCustomSection::tBehavior) );
}

#ifndef DWFTK_READ_ONLY

_DWFTK_API
void
DWFCustomSection::serializeXML( DWFXMLSerializer& rSerializer, unsigned int nFlags )
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
            if (_tBehavior.bPublishResourcesToManifest)
            {
                DWFResourceContainer::getSerializable().serializeXML( rSerializer, nFlags );
            }
        }
        rSerializer.endElement();
    }
    else if (_tBehavior.bPublishDescriptor && (nFlags & DWFPackageWriter::eDescriptor))
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
DWFCustomSection::Factory::Factory( const DWFString& zType )
throw()
                   : _zType( zType )
{
    ;
}

_DWFTK_API
DWFCustomSection::Factory::~Factory()
throw()
{
    ;
}

_DWFTK_API
const DWFString&
DWFCustomSection::Factory::type()
const
throw()
{
    return _zType;
}

_DWFTK_API
DWFCustomSection*
DWFCustomSection::Factory::build( const DWFString&  zName,
                                  const DWFString&  zTitle,
                                  DWFPackageReader* pPackageReader )
throw( DWFException )
{
    DWFCustomSection* pSection = DWFCORE_ALLOC_OBJECT( DWFCustomSection(_zType, zName, zTitle, pPackageReader) );

    if (pSection == NULL)
    {
        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate new section" );
    }

    return pSection;
}

_DWFTK_API
DWFCustomSection*
DWFCustomSection::Factory::build( const DWFString&     zType,
                            const DWFString&     zName,
                            const DWFString&     zTitle,
                            DWFPackageReader*    pPackageReader )
throw( DWFException )
{
    DWFCustomSection* pSection = DWFCORE_ALLOC_OBJECT( DWFCustomSection(zType, zName, zTitle, pPackageReader) );

    if (pSection == NULL)
    {
        _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate new section" );
    }

    return pSection;
}

