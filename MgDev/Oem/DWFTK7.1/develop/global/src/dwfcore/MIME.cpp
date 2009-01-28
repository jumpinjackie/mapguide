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


#include "dwfcore/MIME.h"
using namespace DWFCore;


//
// Holds the singleton - will be deleted when this module unloads
//
DWFPointer<DWFMIME>             g_apDWFMIME( NULL, false );
DWFPointer<DWFMIME::_tList>     DWFMIME::_kapTypeMap( NULL, false );
DWFPointer<DWFMIME::_tMultiMap> DWFMIME::_kapExtensionMap( NULL, false );


#if defined(DWFCORE_STATIC) || !defined(_DWFCORE_WIN32_SYSTEM)

const wchar_t* const DWFMIME::kzMIMEType_DWF1 =     L"model/vnd.dwf";
const wchar_t* const DWFMIME::kzMIMEType_DWF2 =     L"application/x-dwf";
const wchar_t* const DWFMIME::kzMIMEType_DWF3 =     L"drawing/x-dwf";
const wchar_t* const DWFMIME::kzMIMEType_W2D =      L"application/x-w2d";
const wchar_t* const DWFMIME::kzMIMEType_W3D =      L"application/x-w3d";
const wchar_t* const DWFMIME::kzMIMEType_DSD =      L"application/x-dsd";
const wchar_t* const DWFMIME::kzMIMEType_PIA =      L"application/x-dwg-state";
const wchar_t* const DWFMIME::kzMIMEType_BMP =      L"image/bmp";
const wchar_t* const DWFMIME::kzMIMEType_CALS =     L"image/cals";
const wchar_t* const DWFMIME::kzMIMEType_GIF =      L"image/gif";
const wchar_t* const DWFMIME::kzMIMEType_FLIC =     L"image/flic";
const wchar_t* const DWFMIME::kzMIMEType_IG4 =      L"image/ig4";
const wchar_t* const DWFMIME::kzMIMEType_JPG =      L"image/jpeg";
const wchar_t* const DWFMIME::kzMIMEType_PCX =      L"image/pcx";
const wchar_t* const DWFMIME::kzMIMEType_PICT =     L"image/pict";
const wchar_t* const DWFMIME::kzMIMEType_PNG =      L"image/png";
const wchar_t* const DWFMIME::kzMIMEType_RLC =      L"image/rlc";
const wchar_t* const DWFMIME::kzMIMEType_TARGA =    L"image/targa";
const wchar_t* const DWFMIME::kzMIMEType_TIFF =     L"image/tiff";
const wchar_t* const DWFMIME::kzMIMEType_TXT =      L"text/plain";
const wchar_t* const DWFMIME::kzMIMEType_XML =      L"text/xml";
const wchar_t* const DWFMIME::kzMIMEType_FONT =     L"application/x-font";

const wchar_t* const DWFMIME::kzExtension_DWF =     L"dwf";
const wchar_t* const DWFMIME::kzExtension_W2D =     L"w2d";
const wchar_t* const DWFMIME::kzExtension_W3D =     L"w3d";
const wchar_t* const DWFMIME::kzExtension_DSD =     L"dsd";
const wchar_t* const DWFMIME::kzExtension_PIA =     L"pia";
const wchar_t* const DWFMIME::kzExtension_BMP =     L"bmp";
const wchar_t* const DWFMIME::kzExtension_CALS =    L"cals";
const wchar_t* const DWFMIME::kzExtension_FLIC =    L"flc";
const wchar_t* const DWFMIME::kzExtension_GIF =     L"gif";
const wchar_t* const DWFMIME::kzExtension_IG4 =     L"ig4";
const wchar_t* const DWFMIME::kzExtension_JPG =     L"jpg";
const wchar_t* const DWFMIME::kzExtension_PCX =     L"pcx";
const wchar_t* const DWFMIME::kzExtension_PICT =    L"pct";
const wchar_t* const DWFMIME::kzExtension_PNG =     L"png";
const wchar_t* const DWFMIME::kzExtension_RLC =     L"rlc";
const wchar_t* const DWFMIME::kzExtension_TARGA =   L"tga";
const wchar_t* const DWFMIME::kzExtension_TIFF =    L"tif";
const wchar_t* const DWFMIME::kzExtension_TXT =     L"txt";
const wchar_t* const DWFMIME::kzExtension_XML =     L"xml";
const wchar_t* const DWFMIME::kzExtension_FONT =    L"ef_";

#endif


DWFMIME::DWFMIME()
throw()
{
    ;
}

_DWFCORE_API
DWFIterator<const wchar_t* const>*
DWFMIME::GetMIMEType( const wchar_t* const zExtension )
throw( DWFException )
{
        //
        // create the map on first access
        //
    if (g_apDWFMIME.isNull())
    {
        _Build();
    }

    return DWFCORE_ALLOC_OBJECT( _Iterator(_kapExtensionMap->lower_bound(zExtension), _kapExtensionMap->upper_bound(zExtension)) );
}

_DWFCORE_API
const wchar_t* const
DWFMIME::GetExtension( const wchar_t* const zMIMEType )
throw( DWFException )
{
        //
        // create the map on first access
        //
    if (g_apDWFMIME.isNull())
    {
        _Build();
    }

    const wchar_t** pzExtension = _kapTypeMap->find( zMIMEType );
    
    return (pzExtension ? *pzExtension : NULL);
}

void
DWFMIME::_Build()
throw( DWFException )
{
        g_apDWFMIME = DWFCORE_ALLOC_OBJECT( DWFMIME );
        if (g_apDWFMIME.isNull())
        {
            _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate object" );
        }

        _kapExtensionMap = DWFCORE_ALLOC_OBJECT( DWFMIME::_tMultiMap );
        if (_kapExtensionMap.isNull())
        {
            _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate object" );
        }

        _kapTypeMap = DWFCORE_ALLOC_OBJECT( DWFMIME::_tList );
        if (_kapTypeMap.isNull())
        {
            _DWFCORE_THROW( DWFMemoryException, L"Failed to allocate object" );
        }

        //
        // extension -> MIME
        //
        _kapExtensionMap->insert( _tMultiMap::value_type(kzExtension_DWF,   kzMIMEType_DWF1) );
        _kapExtensionMap->insert( _tMultiMap::value_type(kzExtension_DWF,   kzMIMEType_DWF2) );
        _kapExtensionMap->insert( _tMultiMap::value_type(kzExtension_DWF,   kzMIMEType_DWF3) );
        _kapExtensionMap->insert( _tMultiMap::value_type(kzExtension_W2D,   kzMIMEType_W2D) );
        _kapExtensionMap->insert( _tMultiMap::value_type(kzExtension_W3D,   kzMIMEType_W3D)  );
        _kapExtensionMap->insert( _tMultiMap::value_type(kzExtension_DSD,   kzMIMEType_DSD)  );
        _kapExtensionMap->insert( _tMultiMap::value_type(kzExtension_PIA,   kzMIMEType_PIA)  );
		_kapExtensionMap->insert( _tMultiMap::value_type(kzExtension_BMP,   kzMIMEType_BMP) );
        _kapExtensionMap->insert( _tMultiMap::value_type(kzExtension_CALS,  kzMIMEType_CALS) );
        _kapExtensionMap->insert( _tMultiMap::value_type(kzExtension_FLIC,  kzMIMEType_FLIC) );
        _kapExtensionMap->insert( _tMultiMap::value_type(kzExtension_GIF,   kzMIMEType_GIF) );
        _kapExtensionMap->insert( _tMultiMap::value_type(kzExtension_IG4,   kzMIMEType_IG4)  );
        _kapExtensionMap->insert( _tMultiMap::value_type(kzExtension_JPG,   kzMIMEType_JPG) );
        _kapExtensionMap->insert( _tMultiMap::value_type(kzExtension_PCX,   kzMIMEType_PCX)  );
        _kapExtensionMap->insert( _tMultiMap::value_type(kzExtension_PICT,  kzMIMEType_PICT)  );
        _kapExtensionMap->insert( _tMultiMap::value_type(kzExtension_PNG,   kzMIMEType_PNG) );
        _kapExtensionMap->insert( _tMultiMap::value_type(kzExtension_RLC,   kzMIMEType_RLC) );
        _kapExtensionMap->insert( _tMultiMap::value_type(kzExtension_TARGA, kzMIMEType_TARGA) );
        _kapExtensionMap->insert( _tMultiMap::value_type(kzExtension_TIFF,  kzMIMEType_TIFF) );
        _kapExtensionMap->insert( _tMultiMap::value_type(kzExtension_TXT,   kzMIMEType_TXT) );
        _kapExtensionMap->insert( _tMultiMap::value_type(kzExtension_XML,   kzMIMEType_XML) );
        _kapExtensionMap->insert( _tMultiMap::value_type(kzExtension_FONT,  kzMIMEType_FONT) );

        //
        // MIME -> extension
        //
        _kapTypeMap->insert( kzMIMEType_DWF1,  kzExtension_DWF );
        _kapTypeMap->insert( kzMIMEType_DWF2,  kzExtension_DWF );
        _kapTypeMap->insert( kzMIMEType_DWF3,  kzExtension_DWF );
        _kapTypeMap->insert( kzMIMEType_W2D,   kzExtension_W2D );
        _kapTypeMap->insert( kzMIMEType_W3D,   kzExtension_W3D );
        _kapTypeMap->insert( kzMIMEType_DSD,   kzExtension_DSD );
        _kapTypeMap->insert( kzMIMEType_PIA,   kzExtension_PIA );
		_kapTypeMap->insert( kzMIMEType_BMP,   kzExtension_BMP );
        _kapTypeMap->insert( kzMIMEType_CALS,  kzExtension_CALS );
        _kapTypeMap->insert( kzMIMEType_FLIC,  kzExtension_FLIC );
        _kapTypeMap->insert( kzMIMEType_GIF,   kzExtension_GIF );
        _kapTypeMap->insert( kzMIMEType_IG4,   kzExtension_IG4 );
        _kapTypeMap->insert( kzMIMEType_JPG,   kzExtension_JPG );
        _kapTypeMap->insert( kzMIMEType_PCX,   kzExtension_PCX );
        _kapTypeMap->insert( kzMIMEType_PICT,  kzExtension_PICT );
        _kapTypeMap->insert( kzMIMEType_PNG,   kzExtension_PNG );
        _kapTypeMap->insert( kzMIMEType_RLC,   kzExtension_RLC );
        _kapTypeMap->insert( kzMIMEType_TARGA, kzExtension_TARGA );
        _kapTypeMap->insert( kzMIMEType_TIFF,  kzExtension_TIFF );
        _kapTypeMap->insert( kzMIMEType_TXT,   kzExtension_TXT );
        _kapTypeMap->insert( kzMIMEType_XML,   kzExtension_XML );
        _kapTypeMap->insert( kzMIMEType_FONT,  kzExtension_FONT );
}
