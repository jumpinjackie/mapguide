//
//  Copyright (C) 2003-2007 by Autodesk, Inc.  All rights reserved.
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
// $Header: /Components/Internal/DWF Toolkit/v7.1/develop/global/src/dwf/package/GraphicResource.cpp 7     8/11/05 7:56p Kuk $
//


#include "dwf/package/GraphicResource.h"
#include "dwf/package/Constants.h"
#include "dwf/package/writer/DWFXMLSerializer.h"
using namespace DWFToolkit;


_DWFTK_API
DWFGraphicResource::DWFGraphicResource( DWFPackageReader* pPackageReader )
throw()
                  : DWFResource( pPackageReader )
                  , _bShow( true )
                  , _nZOrder( 0 )
                  , _nEffectiveResolution( 0 )
                  , _eOrientation( eNotSpecified )
{
    _anClip[0] = _anClip[1] = _anClip[2] = _anClip[3] = 0.0;
    _anExtents[0] = _anExtents[1] = _anExtents[2] = _anExtents[3] = 0.0;

    _anTransform[0][0] = _anTransform[1][1] = _anTransform[2][2] = _anTransform[3][3] = 1.0;
    _anTransform[0][1] = _anTransform[0][2] = _anTransform[0][3] = 0.0;
    _anTransform[1][0] = _anTransform[1][2] = _anTransform[1][3] = 0.0;
    _anTransform[2][0] = _anTransform[2][1] = _anTransform[2][3] = 0.0;
    _anTransform[3][0] = _anTransform[3][1] = _anTransform[3][2] = 0.0;
}

_DWFTK_API
DWFGraphicResource::DWFGraphicResource( const DWFString& zTitle,
                                        const DWFString& zRole,
                                        const DWFString& zMIME,
                                        const DWFString& zAuthor,
                                        const DWFString& zDescription,
                                        const DWFString& zCreationTime,
                                        const DWFString& zModificationTime )
throw()
                  : DWFResource( zTitle, zRole, zMIME )
                  , _bShow( true )
                  , _nZOrder( 0 )
                  , _nEffectiveResolution( 0 )
                  , _zAuthor( zAuthor )
                  , _zDescription( zDescription )
                  , _zCreationTime( zCreationTime )
                  , _zModificationTime( zModificationTime )
                  , _eOrientation( eNotSpecified )
{
    _anClip[0] = _anClip[1] = _anClip[2] = _anClip[3] = 0.0f;
    _anExtents[0] = _anExtents[1] = _anExtents[2] = _anExtents[3] = 0.0f;

    _anTransform[0][0] = _anTransform[1][1] = _anTransform[2][2] = _anTransform[3][3] = 1.0f;
    _anTransform[0][1] = _anTransform[0][2] = _anTransform[0][3] = 0.0f;
    _anTransform[1][0] = _anTransform[1][2] = _anTransform[1][3] = 0.0f;
    _anTransform[2][0] = _anTransform[2][1] = _anTransform[2][3] = 0.0f;
    _anTransform[3][0] = _anTransform[3][1] = _anTransform[3][2] = 0.0f;
}

_DWFTK_API
DWFGraphicResource::~DWFGraphicResource()
throw()
{
    ;
}

_DWFTK_API
void
DWFGraphicResource::configureGraphic( const double*   anTransform,
                                      const double*   anExtents,
                                      const double*   anClip,
                                      bool            bShow,
                                      int             nZOrder,
                                      int             nEffectiveResolution,
                                      teOrientation   eOrientation )
throw( DWFException )
{
    if (anTransform)
    {
        _anTransform[0][0] = anTransform[0];
        _anTransform[0][1] = anTransform[1];
        _anTransform[0][2] = anTransform[2];
        _anTransform[0][3] = anTransform[3];
        _anTransform[1][0] = anTransform[4];
        _anTransform[1][1] = anTransform[5];
        _anTransform[1][2] = anTransform[6];
        _anTransform[1][3] = anTransform[7];
        _anTransform[2][0] = anTransform[8];
        _anTransform[2][1] = anTransform[9];
        _anTransform[2][2] = anTransform[10];
        _anTransform[2][3] = anTransform[11];
        _anTransform[3][0] = anTransform[12];
        _anTransform[3][1] = anTransform[13];
        _anTransform[3][2] = anTransform[14];
        _anTransform[3][3] = anTransform[15];
    }

    if (anExtents)
    {
        _anExtents[0] = anExtents[0];
        _anExtents[1] = anExtents[1];
        _anExtents[2] = anExtents[2];
        _anExtents[3] = anExtents[3];
    }

    if (anClip)
    {
        _anClip[0] = anClip[0];
        _anClip[1] = anClip[1];
        _anClip[2] = anClip[2];
        _anClip[3] = anClip[3];
    }

    _bShow = bShow;
    _nZOrder = nZOrder;
    _nEffectiveResolution = nEffectiveResolution;
    _eOrientation = eOrientation;
}

_DWFTK_API
void
DWFGraphicResource::parseAttributeList( const char** ppAttributeList )
throw( DWFException )
{
	//DNT_Start

    //
    // parse with base resource first
    //
    DWFResource::parseAttributeList( ppAttributeList );

    unsigned short nFound = 0;
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
            // set the show attribute
            //
        if (!(nFound & 0x01) &&
             (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_Show) == 0))
        {
            nFound |= 0x01;

            if ((ppAttributeList[iAttrib+1][0] == 't') ||      // true
                (ppAttributeList[iAttrib+1][0] == 'T') ||      // True | TRUE
                (ppAttributeList[iAttrib+1][0] == '1') ||      // 1
                (ppAttributeList[iAttrib+1][0] == 'y') ||      // yes
                (ppAttributeList[iAttrib+1][0] == 'Y'))        // Yes | YES
            {
                _bShow = true;
            }
        }
            //
            // set the z-order
            //
        else if (!(nFound & 0x02) &&
                  (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_ZOrder) == 0))
        {
            nFound |= 0x02;
            _nZOrder = ::atoi( ppAttributeList[iAttrib+1] );
        }
            //
            // set the extents
            //
        else if (!(nFound & 0x04) &&
                  (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_Extents) == 0))
        {
            nFound |= 0x04;

            char* pExtents = (char*)ppAttributeList[iAttrib+1];
            char* pToken = ::strtok( pExtents, " " );
            _anExtents[0] = DWFString::StringToDouble( pToken );

            pToken = ::strtok( NULL, " " );
            _anExtents[1] = DWFString::StringToDouble( pToken );

            pToken = ::strtok( NULL, " " );
            _anExtents[2] = DWFString::StringToDouble( pToken );

            pToken = ::strtok( NULL, " " );
            _anExtents[3] = DWFString::StringToDouble( pToken );
        }
            //
            // set the transform matrix
            //
        else if (!(nFound & 0x08) &&
                  (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_Transform) == 0))
        {
            nFound |= 0x08;
            int i, j;

            char* pTransform = (char*)ppAttributeList[iAttrib+1];
            char* pToken = ::strtok( pTransform, " " );

            for (i=0; i<4; i++)
            {
                for (j=0; j<4; j++)
                {
                    _anTransform[i][j] = DWFString::StringToDouble( pToken );
                    pToken = ::strtok( NULL, " " );
                }
            }
        }
            //
            // set the clipping rect
            //
        else if (!(nFound & 0x10) &&
                  (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_Clip) == 0))
        {
            nFound |= 0x10;

            char* pClip = (char*)ppAttributeList[iAttrib+1];
            char* pToken = ::strtok( pClip, " " );
            _anClip[0] = DWFString::StringToDouble( pToken );

            pToken = ::strtok( NULL, " " );
            _anClip[1] = DWFString::StringToDouble( pToken );

            pToken = ::strtok( NULL, " " );
            _anClip[2] = DWFString::StringToDouble( pToken );

            pToken = ::strtok( NULL, " " );
            _anClip[3] = DWFString::StringToDouble( pToken );
        }
            //
            // set the effective resolution
            //
        else if (!(nFound & 0x20) &&
             (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_EffectiveResolution) == 0))
        {
            nFound |= 0x20;

            _nEffectiveResolution = ::atoi( ppAttributeList[iAttrib+1] );
        }
            //
            // set the orientation
            //
        else if (!(nFound & 0x40) &&
             (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_Orientation) == 0))
        {
            nFound |= 0x40;

            if (DWFCORE_COMPARE_ASCII_STRINGS(ppAttributeList[iAttrib+1], "alwaysInSync") == 0)
            {
                _eOrientation = eAlwaysInSync;
            }
            else if (DWFCORE_COMPARE_ASCII_STRINGS(ppAttributeList[iAttrib+1], "alwaysDifferent") == 0)
            {
                _eOrientation = eAlwaysDifferent;
            }
            else if (DWFCORE_COMPARE_ASCII_STRINGS(ppAttributeList[iAttrib+1], "decoupled") == 0)
            {
                _eOrientation = eDecoupled;
            }
        }
            //
            // set the author
            //
        else if (!(nFound & 0x80) &&
             (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_Author) == 0))
        {
            nFound |= 0x80;
            _zAuthor.assign( ppAttributeList[iAttrib+1] );
        }
            //
            // set the description
            //
        else if (!(nFound & 0x0100) &&
             (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_Description) == 0))
        {
            nFound |= 0x0100;
            _zDescription.assign( ppAttributeList[iAttrib+1] );
        }
            //
            // set the creation time
            //
        else if (!(nFound & 0x0200) &&
             (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_CreationTime) == 0))
        {
            nFound |= 0x0200;
            _zCreationTime.assign( ppAttributeList[iAttrib+1] );
        }
            //
            // set the modification time
            //
        else if (!(nFound & 0x0400) &&
             (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_ModificationTime) == 0))
        {
            nFound |= 0x0400;
            _zModificationTime.assign( ppAttributeList[iAttrib+1] );
        }
    }
	//DNT_End
}

#ifndef DWFTK_READ_ONLY

_DWFTK_API
void
DWFGraphicResource::serializeXML( DWFXMLSerializer& rSerializer, unsigned int nFlags )
throw( DWFException )
{
	//DNT_Start
    DWFString zNamespace;

        //
        // namespace dictated by document and section type
        //
    if (nFlags & DWFPackageWriter::eDescriptor)
    {
        zNamespace.assign( namespaceXML(nFlags) );
    }

        //
        // serialize in detail for descriptor
        //
    if (nFlags & DWFPackageWriter::eDescriptor)
    {
            //
            // start element
            //
        if ((nFlags & DWFXMLSerializer::eElementOpen) == 0)
        {
            rSerializer.startElement( DWFXML::kzElement_GraphicResource, zNamespace );

            //
            // let base class know not to start the element
            //
            nFlags |= DWFXMLSerializer::eElementOpen;

            //
            // base attributes
            //
            DWFResource::serializeXML( rSerializer, nFlags );

            //
            // clear this bit
            //
            nFlags &= ~DWFXMLSerializer::eElementOpen;
        }
            //
            // element already started by derived class
            //
        else
        {
            //
            // base attributes
            //
            DWFResource::serializeXML( rSerializer, nFlags );
        }

            //
            // attributes
            //
        {
#define _DWFTK_GFX_RESOURCE_TEMP_BUFFER_BYTES   1024

            wchar_t zTempBuffer[_DWFTK_GFX_RESOURCE_TEMP_BUFFER_BYTES];

            rSerializer.addAttribute( DWFXML::kzAttribute_Author, _zAuthor );
            rSerializer.addAttribute( DWFXML::kzAttribute_Description, _zDescription );
            rSerializer.addAttribute( DWFXML::kzAttribute_CreationTime, _zCreationTime );
            rSerializer.addAttribute( DWFXML::kzAttribute_ModificationTime, _zModificationTime );

            if (_eOrientation != eNotSpecified)
            {
                rSerializer.addAttribute( DWFXML::kzAttribute_Orientation,
                                            (_eOrientation == eAlwaysInSync) ? L"alwaysInSync" :
                                            (_eOrientation == eAlwaysDifferent) ? L"alwaysDifferent" : L"decoupled" );
            }

            if (_bShow == false)
            {
                rSerializer.addAttribute( DWFXML::kzAttribute_Show, L"false" );
            }

            if (_nZOrder != 0)
            {
                _DWFCORE_SWPRINTF( zTempBuffer, _DWFTK_GFX_RESOURCE_TEMP_BUFFER_BYTES, L"%d", _nZOrder );
                rSerializer.addAttribute( DWFXML::kzAttribute_ZOrder, zTempBuffer );
            }

            if ((_anClip[0] != 0.0) ||
                (_anClip[1] != 0.0) ||
                (_anClip[2] != 0.0) ||
                (_anClip[3] != 0.0))
            {
                _DWFCORE_SWPRINTF( zTempBuffer, _DWFTK_GFX_RESOURCE_TEMP_BUFFER_BYTES, L"%.17g %.17g %.17g %.17g", _anClip[0], _anClip[1], _anClip[2], _anClip[3] );

                rSerializer.addAttribute( DWFXML::kzAttribute_Clip, DWFString::RepairDecimalSeparators(zTempBuffer) );
            }

            if ((_anExtents[0] != 0.0) ||
                (_anExtents[1] != 0.0) ||
                (_anExtents[2] != 0.0) ||
                (_anExtents[3] != 0.0))
            {
                _DWFCORE_SWPRINTF( zTempBuffer, _DWFTK_GFX_RESOURCE_TEMP_BUFFER_BYTES, L"%.17g %.17g %.17g %.17g", _anExtents[0], _anExtents[1], _anExtents[2], _anExtents[3] );

                rSerializer.addAttribute( DWFXML::kzAttribute_Extents, DWFString::RepairDecimalSeparators(zTempBuffer) );
            }

                //
                // transform is required
                //
            {
                _DWFCORE_SWPRINTF( zTempBuffer, _DWFTK_GFX_RESOURCE_TEMP_BUFFER_BYTES,
                                   L"%.17g %.17g %.17g %.17g %.17g %.17g %.17g %.17g %.17g %.17g %.17g %.17g %.17g %.17g %.17g %.17g",
                                  _anTransform[0][0], _anTransform[0][1], _anTransform[0][2], _anTransform[0][3],
                                  _anTransform[1][0], _anTransform[1][1], _anTransform[1][2], _anTransform[1][3],
                                  _anTransform[2][0], _anTransform[2][1], _anTransform[2][2], _anTransform[2][3],
                                  _anTransform[3][0], _anTransform[3][1], _anTransform[3][2], _anTransform[3][3] );

                rSerializer.addAttribute( DWFXML::kzAttribute_Transform, DWFString::RepairDecimalSeparators(zTempBuffer) );
            }

            if (_nEffectiveResolution != 0)
            {
                _DWFCORE_SWPRINTF( zTempBuffer, _DWFTK_GFX_RESOURCE_TEMP_BUFFER_BYTES, L"%d", _nEffectiveResolution );
                rSerializer.addAttribute( DWFXML::kzAttribute_EffectiveResolution, zTempBuffer );
            }

        }

            //
            // if the derived class has already open the element
            // we can only serialize the base attributes here
            //
        if ((nFlags & DWFXMLSerializer::eElementOpen) == 0)
        {
            //
            // properties
            //
            DWFXMLSerializable& rSerializable = DWFPropertyContainer::getSerializable();
            rSerializable.serializeXML( rSerializer, nFlags );
        }

            //
            // close element if this bit is not set
            //
        if ((nFlags & DWFXMLSerializer::eElementOpen) == 0)
        {
            rSerializer.endElement();
        }
    }
        //
        // otherwise defer to the base class
        //
    else
    {
        DWFResource::serializeXML( rSerializer, nFlags );
    }
	//DNT_End
}

#endif


//
//
//

_DWFTK_API
DWFImageResource::DWFImageResource( DWFPackageReader* pPackageReader )
throw()
                : DWFGraphicResource( pPackageReader )
                , _bScanned( false )
                , _bInvertColors ( false )
                , _nColorDepth( 0 )
                , _nScannedResolution( 0 )
{
    _anOriginalExtents[0] = _anOriginalExtents[1] = _anOriginalExtents[2] = _anOriginalExtents[3] = 0.0f;
}

_DWFTK_API
DWFImageResource::DWFImageResource( const DWFString& zTitle,
                                    const DWFString& zRole,
                                    const DWFString& zMIME,
                                    const DWFString& zAuthor,
                                    const DWFString& zDescription,
                                    const DWFString& zCreationTime,
                                    const DWFString& zModificationTime )
throw()
                : DWFGraphicResource( zTitle, zRole, zMIME, zAuthor, zDescription, zCreationTime, zModificationTime )
{
    _anOriginalExtents[0] = _anOriginalExtents[1] = _anOriginalExtents[2] = _anOriginalExtents[3] = 0.0f;
}

_DWFTK_API
DWFImageResource::~DWFImageResource()
throw()
{
    ;
}

_DWFTK_API
void
DWFImageResource::configureImage( unsigned char  nColorDepth,
                                  bool           bInvertColors,
                                  bool           bScannedImage,
                                  int            nScannedResolution,
                                  const double*  anOriginalExtents )
throw( DWFException )
{
    _nColorDepth = nColorDepth;
    _bInvertColors = bInvertColors;
    _bScanned = bScannedImage;
    _nScannedResolution = nScannedResolution;

    if (anOriginalExtents)
    {
        _anOriginalExtents[0] = anOriginalExtents[0];
        _anOriginalExtents[1] = anOriginalExtents[1];
        _anOriginalExtents[2] = anOriginalExtents[2];
        _anOriginalExtents[3] = anOriginalExtents[3];
    }
}

_DWFTK_API
void
DWFImageResource::parseAttributeList( const char** ppAttributeList )
throw( DWFException )
{
    //
    // parse with base resource first
    //
    DWFGraphicResource::parseAttributeList( ppAttributeList );

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
            // set the invert colors attribute
            //
        if (!(nFound & 0x01) &&
             (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_InvertColors) == 0))
        {
            nFound |= 0x01;

            if ((ppAttributeList[iAttrib+1][0] == 't') ||      // true
                (ppAttributeList[iAttrib+1][0] == 'T') ||      // True | TRUE
                (ppAttributeList[iAttrib+1][0] == '1') ||      // 1
                (ppAttributeList[iAttrib+1][0] == 'y') ||      // yes
                (ppAttributeList[iAttrib+1][0] == 'Y'))        // Yes | YES
            {
                _bInvertColors = true;
            }
        }
            //
            // set the scanned attribute
            //
        else if (!(nFound & 0x02) &&
             (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_Scanned) == 0))
        {
            nFound |= 0x02;

            if ((ppAttributeList[iAttrib+1][0] == 't') ||      // true
                (ppAttributeList[iAttrib+1][0] == 'T') ||      // True | TRUE
                (ppAttributeList[iAttrib+1][0] == '1') ||      // 1
                (ppAttributeList[iAttrib+1][0] == 'y') ||      // yes
                (ppAttributeList[iAttrib+1][0] == 'Y'))        // Yes | YES
            {
                _bScanned = true;
            }
        }
            //
            // set the color depth
            //
        else if (!(nFound & 0x04) &&
                  (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_ColorDepth) == 0))
        {
            nFound |= 0x04;
            _nColorDepth = (((unsigned char)::atoi(ppAttributeList[iAttrib+1])) >> 5);
        }
            //
            // set the scanned res
            //
        else if (!(nFound & 0x08) &&
                  (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_ScannedResolution) == 0))
        {
            nFound |= 0x04;
            _nScannedResolution = ::atoi( ppAttributeList[iAttrib+1] );
        }
            //
            // set the original extents
            //
        else if (!(nFound & 0x08) &&
                  (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFXML::kzAttribute_OriginalExtents) == 0))
        {
            nFound |= 0x08;

            char* pExtents = (char*)ppAttributeList[iAttrib+1];
            char* pToken = ::strtok( pExtents, /*NOXLATE*/" " );
            _anOriginalExtents[0] = DWFString::StringToDouble( pToken );

            pToken = ::strtok( NULL, /*NOXLATE*/" " );
            _anOriginalExtents[1] = DWFString::StringToDouble( pToken );

            pToken = ::strtok( NULL, /*NOXLATE*/" " );
            _anOriginalExtents[2] = DWFString::StringToDouble( pToken );

            pToken = ::strtok( NULL, /*NOXLATE*/" " );
            _anOriginalExtents[3] = DWFString::StringToDouble( pToken );
        }
    }
}

#ifndef DWFTK_READ_ONLY

_DWFTK_API
void
DWFImageResource::serializeXML( DWFXMLSerializer& rSerializer, unsigned int nFlags )
throw( DWFException )
{
        //
        // serialize in detail for descriptor
        //
    if (nFlags & DWFPackageWriter::eDescriptor)
    {
        DWFString zNamespace( namespaceXML(nFlags) );


        rSerializer.startElement( DWFXML::kzElement_ImageResource, zNamespace );

        //
        // let base class know not to start the element
        //
        nFlags |= DWFXMLSerializer::eElementOpen;

        //
        // base attributes
        //
        DWFGraphicResource::serializeXML( rSerializer, nFlags );

            //
            // attributes
            //
        {
            wchar_t zTempBuffer[_DWFTK_GFX_RESOURCE_TEMP_BUFFER_BYTES];


            if (_bScanned)
            {
                rSerializer.addAttribute( DWFXML::kzAttribute_Scanned, /*NOXLATE*/L"true" );
            }

            if (_bInvertColors)
            {
                rSerializer.addAttribute( DWFXML::kzAttribute_InvertColors, /*NOXLATE*/L"true" );
            }

            if (_nScannedResolution > 0)
            {
                _DWFCORE_SWPRINTF( zTempBuffer, _DWFTK_GFX_RESOURCE_TEMP_BUFFER_BYTES, /*NOXLATE*/L"%d", _nScannedResolution );
                rSerializer.addAttribute( DWFXML::kzAttribute_ScannedResolution, zTempBuffer );
            }

            if (_nColorDepth > 0)
            {
                _DWFCORE_SWPRINTF( zTempBuffer, _DWFTK_GFX_RESOURCE_TEMP_BUFFER_BYTES, /*NOXLATE*/L"%d", _nColorDepth );
                rSerializer.addAttribute( DWFXML::kzAttribute_ColorDepth, zTempBuffer );
            }

            if ((_anOriginalExtents[0] != 0.0) ||
                (_anOriginalExtents[1] != 0.0) ||
                (_anOriginalExtents[2] != 0.0) ||
                (_anOriginalExtents[3] != 0.0))
            {
                _DWFCORE_SWPRINTF( zTempBuffer, _DWFTK_GFX_RESOURCE_TEMP_BUFFER_BYTES, /*NOXLATE*/L"%.17g %.17g %.17g %.17g",
                                  _anOriginalExtents[0], _anOriginalExtents[1], _anOriginalExtents[2], _anOriginalExtents[3] );

                rSerializer.addAttribute( DWFXML::kzAttribute_OriginalExtents, DWFString::RepairDecimalSeparators(zTempBuffer) );
            }
        }

        //
        // properties - as the most derived resource, we must invoke the
        // property serialization ourselves or we will end up with properties
        // in the middle of our resource element
        //
        DWFXMLSerializable& rSerializable = DWFPropertyContainer::getSerializable();
        rSerializable.serializeXML( rSerializer, nFlags );

        rSerializer.endElement();
    }
        //
        // otherwise defer to the base class
        //
    else
    {
        DWFResource::serializeXML( rSerializer, nFlags );
    }
}

#endif

