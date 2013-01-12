//
//  Copyright (C) 2004-2011 by Autodesk, Inc.
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


#include "dwf/publisher/model/IncludeSegment.h"
using namespace DWFToolkit;


#if defined(DWFTK_STATIC) || !defined(_DWFCORE_WIN32_SYSTEM)

const char* const DWFIncludeSegment::kz_Include_Library = /*NOXLATE*/"?Include Library/";

#endif



_DWFTK_API
DWFIncludeSegment::DWFIncludeSegment( DWFSegmentHandlerBuilder&     rSegmentBuilder,
                                      DWFGeometryHandlerBuilder&    rGeometryBuilder,
                                      DWFFeatureHandlerBuilder&     rFeatureBuilder,
                                      DWFAttributeHandlerBuilder&   rAttributeBuilder,
                                      DWFPublishedObject::Factory&  rObjectFactory )
throw()
                 : DWFSegment( rSegmentBuilder,
                               rGeometryBuilder,
                               rFeatureBuilder,
                               rAttributeBuilder,
                               rObjectFactory )
{
    _pLibrary = DWFCORE_ALLOC_OBJECT( DWFString(kz_Include_Library) );
}

_DWFTK_API
DWFIncludeSegment::DWFIncludeSegment( const DWFIncludeSegment& rSegment )
throw()
                 : DWFSegment( rSegment )
{
}

_DWFTK_API
DWFIncludeSegment&
DWFIncludeSegment::operator=( const DWFIncludeSegment& rSegment )
throw()
{
    DWFSegment::operator=( rSegment );

    return *this;
}

_DWFTK_API
DWFIncludeSegment::~DWFIncludeSegment()
throw()
{
    ;
}

_DWFTK_API
void
DWFIncludeSegment::open( const DWFString* zName )
throw( DWFException )
{
    if (zName == NULL)
    {
        _DWFCORE_THROW( DWFInvalidArgumentException, L"Include segments must be named" );
    }

    DWFSegment::open( zName );

    //
    // mark this object as a reference
    //
    _pPublishedObject->referenced();
}

_DWFTK_API
void
DWFIncludeSegment::open( const DWFString& zName )
throw( DWFException )
{
    DWFSegment::open( zName );

    //
    // mark this object as a reference
    //
    _pPublishedObject->referenced();
}


#endif

