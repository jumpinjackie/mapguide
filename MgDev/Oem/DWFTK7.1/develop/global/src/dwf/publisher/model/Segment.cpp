//
//  Copyright (C) 2004-2008 by Autodesk, Inc.
//
//  By using this code, you are agreeing to the terms and conditions of
//  the License Agreement included in the documentation for this code.
//
//  AUTODESK MAKES NO WARRANTIES, EXPRESSED OR IMPLIED,
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


#ifndef DWFTK_READ_ONLY


#include "dwfcore/Timer.h"
#include "dwfcore/Pointer.h"
using namespace DWFCore;

#include "dwf/publisher/PublishedObject.h"
#include "dwf/publisher/model/Segment.h"
#include "dwf/publisher/model/StyleSegment.h"
#include "dwf/publisher/model/IncludeSegment.h"
using namespace DWFToolkit;



#if defined(DWFTK_STATIC) || !defined(_DWFCORE_WIN32_SYSTEM)

//DNT_Start
const char* const DWFSegment::kz_User_Options_Node                         = "node";
const char* const DWFSegment::kz_User_Options_Edges                        = "edges";
const char* const DWFSegment::kz_User_Options_Hidden                       = "hidden";
const char* const DWFSegment::kz_User_Options_Transparent                  = "transparent";

const char* const DWFSegment::kz_User_Options_Hidden_Node                  = "node,hidden";
const char* const DWFSegment::kz_User_Options_Transparent_Node             = "node,transparent";

const char* const DWFSegment::kz_User_Options_Node_and_Edges               = "node,edges";
const char* const DWFSegment::kz_User_Options_Hidden_Node_and_Edges        = "node,edges,hidden";
const char* const DWFSegment::kz_User_Options_Transparent_Node_and_Edges   = "node,edges,transparent";
//DNT_End

#endif


unsigned int DWFSegment::_knNextKey = 0;


_DWFTK_API
DWFSegment::DWFSegment( DWFSegmentHandlerBuilder&    rSegmentBuilder,
                        DWFGeometryHandlerBuilder&   rGeometryBuilder,
                        DWFFeatureHandlerBuilder&    rFeatureBuilder,
                        DWFAttributeHandlerBuilder&  rAttributeBuilder,
                        DWFPublishedObject::Factory& rObjectFactory,
                        DWFPublishedObject*          pPublishedParent )
throw()
       : _rSegmentBuilder( rSegmentBuilder )
       , _rGeometryBuilder( rGeometryBuilder )
       , _rFeatureBuilder( rFeatureBuilder )
       , _rAttributeBuilder( rAttributeBuilder )
       , _rObjectFactory( rObjectFactory )
       , _pPublishedObject( NULL )
       , _pPublishedParent( pPublishedParent )
       , _pLibrary( NULL )
       , _bOpen( false )
       , _nKey( 0 )
{
    //
    // use sequential numbers for segment keying
    // rather than unique ticks or guids
    // this greatly improves standard LZH compression
    // further, since we name the segments in the W3D stream
    // with the same strings, we get even better compression
    // if the W3D stream itself is written with compression OFF
    //
    _nKey = DWFCore::AtomicIncrement( (int*)&_knNextKey );
}

_DWFTK_API
DWFSegment::DWFSegment( const DWFSegment& rSegment )
throw( DWFException )
       : _rSegmentBuilder( rSegment._rSegmentBuilder )
       , _rGeometryBuilder( rSegment._rGeometryBuilder )
       , _rFeatureBuilder( rSegment._rFeatureBuilder )
       , _rAttributeBuilder( rSegment._rAttributeBuilder )
       , _rObjectFactory( rSegment._rObjectFactory )
       , _pPublishedObject( rSegment._pPublishedObject )
       , _pPublishedParent( rSegment._pPublishedParent )
       , _pLibrary( NULL )
       , _bOpen( rSegment._bOpen )
       , _nKey( rSegment._nKey )
{
    if (rSegment._pLibrary)
    {
        _pLibrary = DWFCORE_ALLOC_OBJECT( DWFString(*(rSegment._pLibrary)) );
    }
}

_DWFTK_API
DWFSegment&
DWFSegment::operator=( const DWFSegment& rSegment )
throw( DWFException )
{
    _rSegmentBuilder = rSegment._rSegmentBuilder;
    _rGeometryBuilder = rSegment._rGeometryBuilder;
    _rFeatureBuilder = rSegment._rFeatureBuilder;
    _rAttributeBuilder = rSegment._rAttributeBuilder;
    _rObjectFactory = rSegment._rObjectFactory;
    _pPublishedObject = rSegment._pPublishedObject;
    _pPublishedParent = rSegment._pPublishedParent;
    _bOpen = rSegment._bOpen;
    _nKey = rSegment._nKey;

    if (rSegment._pLibrary)
    {
        _pLibrary = DWFCORE_ALLOC_OBJECT( DWFString(*(rSegment._pLibrary)) );
    }
    else
    {
        _pLibrary = NULL;
    }

    return *this;
}

_DWFTK_API
DWFSegment::~DWFSegment()
throw()
{
    if (_pLibrary)
    {
        DWFCORE_FREE_OBJECT( _pLibrary );
    }
}

_DWFTK_API
DWFSegment::tKey
DWFSegment::key() const
throw()
{
    return _nKey;
}

_DWFTK_API
DWFSegment
DWFSegment::openSegment()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return DWFSegment( _rSegmentBuilder,
                      _rGeometryBuilder,
                      _rFeatureBuilder,
                      _rAttributeBuilder,
                      _rObjectFactory,
                    ((_pPublishedObject) ? _pPublishedObject  : _pPublishedParent) );
}

_DWFTK_API
void
DWFSegment::style( DWFStyleSegment& rSegment )
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    TK_Referenced_Segment& rHandler = _rSegmentBuilder.getStyleSegmentHandler();

    //
    //
    //
    const DWFString& zName = rSegment.name();

    if (zName.chars() > 0)
    {
        char* pUTF8Name = NULL;
        zName.getUTF8( &pUTF8Name );

        rHandler.SetSegment( pUTF8Name );
        rHandler.serialize();

        DWFCORE_FREE_MEMORY( pUTF8Name );
    }
    else
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Style segments must be named" );
    }
}

_DWFTK_API
void
DWFSegment::include( DWFIncludeSegment& rSegment )
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    TK_Referenced_Segment& rHandler = _rSegmentBuilder.getIncludeSegmentHandler();

    //
    // get the segment path from the object
    //
    DWFString zIncludePath( DWFIncludeSegment::kz_Include_Library );

    rSegment._pPublishedObject->path( zIncludePath );

    //
    //
    //
    size_t nChars = zIncludePath.chars();

    if (nChars > 0)
    {
        char* pUTF8Name = NULL;
        zIncludePath.getUTF8( &pUTF8Name );

        rHandler.SetSegment( pUTF8Name );
        rHandler.serialize();

        DWFCORE_FREE_MEMORY( pUTF8Name );
    }
    else
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Include segments must be named" );
    }

        //
        // add the included segment as a reference
        //
        // named segments will acquire the geometry of the include
        // segment but not their structure.  this is because the
        // nav tree will end up in a odd looking state with a
        // parent & child essentially being the same segment.
        //
    if (_pPublishedObject == NULL)
    {
        if (_pPublishedParent)
        {
            _pPublishedParent->addReference( rSegment._pPublishedObject, _nKey );

            //
            // mark this as an interesting/selectable segment
            // this is required as with no published object
            // (i.e. the segment was not named) there is no
            // user_options attribute to mark this segment as scene selectable
            //
            TK_User_Options& rOptionsHandler = getUserOptionsHandler();
            rOptionsHandler.SetOptions( kz_User_Options_Node );
            rOptionsHandler.serialize();
        }
            //
            // nothing to attach this reference to
            // so we need to create one...
            //
        else
        {
            //
            // create a published object using this segment key with
            // the name of the inclusion segment
            //
            _pPublishedObject = _rObjectFactory.makePublishedObject( _nKey, rSegment._pPublishedObject->name() );

            //
            // still need to capture the referenced properties
            //
            _pPublishedObject->addReference( rSegment._pPublishedObject, _nKey, NULL, true );

            //
            // mark this as an interesting/selectable segment
            //
            TK_User_Options& rOptionsHandler = getUserOptionsHandler();
            rOptionsHandler.SetOptions( kz_User_Options_Node );
            rOptionsHandler.serialize();
        }
    }
        //
        // still need to capture the referenced properties
        //
    else
    {
        _pPublishedObject->addReference( rSegment._pPublishedObject, _nKey, NULL, true );
    }
}

_DWFTK_API
void
DWFSegment::include( DWFIncludeSegment&  rSegment,
                     const DWFString& rInstanceName )
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    TK_Referenced_Segment& rHandler = _rSegmentBuilder.getIncludeSegmentHandler();

    //
    // get the segment path from the object
    //
    DWFString zIncludePath( DWFIncludeSegment::kz_Include_Library );

    rSegment._pPublishedObject->path( zIncludePath );

    //
    //
    //
    size_t nChars = zIncludePath.chars();

    if (nChars > 0)
    {
        char* pUTF8Name = NULL;
        zIncludePath.getUTF8( &pUTF8Name );

        rHandler.SetSegment( pUTF8Name );
        rHandler.serialize();

        DWFCORE_FREE_MEMORY( pUTF8Name );
    }
    else
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Include segments must be named" );
    }

        //
        // add the included segment as a reference
        //
        // named segments will acquire the geomtry of the include
        // segment but not their structure.  this is because the
        // nav tree will end up in a odd looking state with a
        // parent & child essentially being the same segment.
        //
    if (_pPublishedObject == NULL)
    {
        if (_pPublishedParent)
        {
            _pPublishedParent->addReference( rSegment._pPublishedObject, _nKey, &rInstanceName );

            //
            // mark this as an interesting/selectable segment
            // this is required as with no published object
            // (i.e. the segment was not named) there is no
            // user_options attribute to mark this segment as scene selectable
            //
            TK_User_Options& rOptionsHandler = getUserOptionsHandler();
            rOptionsHandler.SetOptions( kz_User_Options_Node );
            rOptionsHandler.serialize();
        }
            //
            // nothing to attach this reference to
            // so we need to create one...
            //
        else
        {
            //
            // create a published object using this segment key with
            // the name of the inclusion segment
            //
            _pPublishedObject = _rObjectFactory.makePublishedObject( _nKey, rInstanceName );

            //
            // still need to capture the referenced properties
            //
            _pPublishedObject->addReference( rSegment._pPublishedObject, _nKey, NULL, true );

            //
            // mark this as an interesting/selectable segment
            //
            TK_User_Options& rOptionsHandler = getUserOptionsHandler();
            rOptionsHandler.SetOptions( kz_User_Options_Node );
            rOptionsHandler.serialize();
        }
    }
        //
        // still need to capture the referenced properties
        //
    else
    {
        _pPublishedObject->addReference( rSegment._pPublishedObject, _nKey, &rInstanceName, true );
    }
}

_DWFTK_API
void
DWFSegment::include( DWFSegment::tKey nSegmentKey )
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    //
    // locate the desired object
    //
    DWFPublishedObject& rObject = _rObjectFactory.findPublishedObject( nSegmentKey );

        //
        // key must refer to an IncludeSegment
        //
    if (rObject.isReferenced() == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"The key provided doesn't refer to an IncludeSegment" );
    }

    //
    // get the segment path from the object
    //
    DWFString zIncludePath( DWFIncludeSegment::kz_Include_Library );

    rObject.path( zIncludePath );

    TK_Referenced_Segment& rHandler = _rSegmentBuilder.getIncludeSegmentHandler();

    //
    //
    //
    size_t nChars = zIncludePath.chars();

    if (nChars > 0)
    {
        char* pUTF8Name = NULL;
        zIncludePath.getUTF8( &pUTF8Name );

        rHandler.SetSegment( pUTF8Name );
        rHandler.serialize();

        DWFCORE_FREE_MEMORY( pUTF8Name );
    }
    else
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Include segments must be named" );
    }

        //
        // add the included segment as a reference
        //
        // named segments will acquire the geomtry of the include
        // segment but not their structure.  this is because the
        // nav tree will end up in a odd looking state with a
        // parent & child essentially being the same segment.
        //
    if (_pPublishedObject == NULL)
    {
        if (_pPublishedParent)
        {
            _pPublishedParent->addReference( &rObject, _nKey );

            //
            // mark this as an interesting/selectable segment
            // this is required as with no published object
            // (i.e. the segment was not named) there is no
            // user_options attribute to mark this segment as scene selectable
            //
            TK_User_Options& rOptionsHandler = getUserOptionsHandler();
            rOptionsHandler.SetOptions( kz_User_Options_Node );
            rOptionsHandler.serialize();
        }
            //
            // nothing to attach this reference to
            // so we need to create one...
            //
        else
        {
            //
            // create a published object using this segment key with
            // the name of the inclusion segment
            //
            _pPublishedObject = _rObjectFactory.makePublishedObject( nSegmentKey, rObject.name() );

            //
            // still need to capture the referenced properties
            //
            _pPublishedObject->addReference( &rObject, nSegmentKey, NULL, true );

            //
            // mark this as an interesting/selectable segment
            //
            TK_User_Options& rOptionsHandler = getUserOptionsHandler();
            rOptionsHandler.SetOptions( kz_User_Options_Node );
            rOptionsHandler.serialize();
        }
    }
        //
        // still need to capture the referenced properties
        //
    else
    {
        _pPublishedObject->addReference( &rObject, _nKey, NULL, true );
    }
}

_DWFTK_API
void
DWFSegment::include( DWFSegment::tKey   nSegmentKey,
                     const DWFString&   rInstanceName )
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    //
    // locate the desired object
    //
    DWFPublishedObject& rObject = _rObjectFactory.findPublishedObject( nSegmentKey );

        //
        // key must refer to an IncludeSegment
        //
    if (rObject.isReferenced() == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"The key provided doesn't refer to an IncludeSegment" );
    }

    //
    // get the segment path from the object
    //
    DWFString zIncludePath( DWFIncludeSegment::kz_Include_Library );

    rObject.path( zIncludePath );

    TK_Referenced_Segment& rHandler = _rSegmentBuilder.getIncludeSegmentHandler();

    //
    //
    //
    size_t nChars = zIncludePath.chars();

    if (nChars > 0)
    {
        char* pUTF8Name = NULL;
        zIncludePath.getUTF8( &pUTF8Name );

        rHandler.SetSegment( pUTF8Name );
        rHandler.serialize();

        DWFCORE_FREE_MEMORY( pUTF8Name );
    }
    else
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Include segments must be named" );
    }

        //
        // add the included segment as a reference
        //
        // named segments will acquire the geomtry of the include
        // segment but not their structure.  this is because the
        // nav tree will end up in a odd looking state with a
        // parent & child essentially being the same segment.
        //
    if (_pPublishedObject == NULL)
    {
        if (_pPublishedParent)
        {
            _pPublishedParent->addReference( &rObject, _nKey, &rInstanceName );

            //
            // mark this as an interesting/selectable segment
            // this is required as with no published object
            // (i.e. the segment was not named) there is no
            // user_options attribute to mark this segment as scene selectable
            //
            TK_User_Options& rOptionsHandler = getUserOptionsHandler();
            rOptionsHandler.SetOptions( kz_User_Options_Node );
            rOptionsHandler.serialize();
        }
            //
            // nothing to attach this reference to
            // so we need to create one...
            //
        else
        {
            //
            // create a published object using this segment key with
            // the name of the inclusion segment
            //
            _pPublishedObject = _rObjectFactory.makePublishedObject( nSegmentKey, rInstanceName );

            //
            // still need to capture the referenced properties
            //
            _pPublishedObject->addReference( &rObject, _nKey, NULL, true );

            //
            // mark this as an interesting/selectable segment
            //
            TK_User_Options& rOptionsHandler = getUserOptionsHandler();
            rOptionsHandler.SetOptions( kz_User_Options_Node );
            rOptionsHandler.serialize();
        }
    }
        //
        // still need to capture the referenced properties
        //
    else
    {
        _pPublishedObject->addReference( &rObject, _nKey, &rInstanceName, true );
    }
}

_DWFTK_API
void
DWFSegment::open( const DWFString* zName,
                  bool             bPublishedEdges )
throw( DWFException )
{
    if (_bOpen)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment already open" );
    }

        //
        // open named segment
        //
    if (zName)
    {
        DWFSegment::open( *zName, bPublishedEdges );
    }
    else
    {
        //
        // write to stream and request this segment be tagged
        //
        _rSegmentBuilder.getOpenSegmentHandler().serialize( (const void*)(uint64_t)(_nKey) );

        //
        // must be set before handlers are used below
        //
        _bOpen = true;

            //
            // this segment will contain published edges
            // so add a reference to a known style segment
            //
        if (bPublishedEdges)
        {
            TK_Referenced_Segment& rEdges = _rSegmentBuilder.getStyleSegmentHandler();
            rEdges.SetSegment( DWFStyleSegment::kz_StyleSegment_PublishedEdges );
            rEdges.serialize();

            //
            // mark this as a published edge segment
            //
            TK_User_Options& rOptionsHandler = getUserOptionsHandler();
            rOptionsHandler.SetOptions( kz_User_Options_Edges );
            rOptionsHandler.serialize();
        }
    }
}

_DWFTK_API
void
DWFSegment::open( const DWFString& zName,
                  bool             bPublishedEdges )
throw( DWFException )
{
    if (_bOpen)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment already open" );
    }

    TK_Open_Segment& rHandler = _rSegmentBuilder.getOpenSegmentHandler();

        //
        // this segment will contain published edges
        // so add a reference to a known style segment
        //
    if (bPublishedEdges)
    {
        TK_Referenced_Segment& rEdges = _rSegmentBuilder.getStyleSegmentHandler();
        rEdges.SetSegment( DWFStyleSegment::kz_StyleSegment_PublishedEdges );
        rEdges.serialize();
    }

    //
    // create a published object keyed to this segment
    // hold onto the new object so we can set properties on it
    //
    _pPublishedObject = _rObjectFactory.makePublishedObject( _nKey, zName );

        //
        // tell the published object who his parent is
        //
    if (_pPublishedParent)
    {
        _pPublishedObject->setParent( _pPublishedParent );
    }

    //
    // "name" segment with the key
    //
    char zSegmentID[12] = {0};
    ::sprintf( zSegmentID, /*NOXLATE*/"%lu", _nKey );

        //
        // special case
        //
    if (_pLibrary)
    {
        DWFString zSegmentPath( *_pLibrary );
                  zSegmentPath.append( zSegmentID );

        //
        // abbreviated buffer since this will be a 7-bit ASCII string
        //
        size_t nChars = zSegmentPath.chars() + 1;
        DWFPointer<ASCII_char_t> apBuffer( DWFCORE_ALLOC_MEMORY(ASCII_char_t, nChars), true );
        zSegmentPath.getUTF8( apBuffer, nChars );

        rHandler.SetSegment( (const char*)apBuffer );
    }
    else
    {
        rHandler.SetSegment( zSegmentID );
    }

    //
    // write to stream and request this segment be tagged
    //
    rHandler.serialize( (const void*)(uintptr_t)_nKey );

    //
    // mark as open as soon as the opcode is written
    //
    _bOpen = true;

    //
    // mark this as an interesting/selectable segment
    //
    /*
    const char* zOptions = NULL;

    if (bPublishedEdges)
    {
        zOptions = (eInitialState == eDefault) ? kz_User_Options_Node_and_Edges :
                   (eInitialState == eHidden)  ? kz_User_Options_Hidden_Node_and_Edges :
                                                 kz_User_Options_Transparent_Node_and_Edges;
    }
    else
    {
        zOptions = (eInitialState == eDefault) ? kz_User_Options_Node :
                   (eInitialState == eHidden)  ? kz_User_Options_Hidden_Node :
                                                 kz_User_Options_Transparent_Node;
    }
    */

    TK_User_Options& rOptionsHandler = getUserOptionsHandler();
    rOptionsHandler.SetOptions( bPublishedEdges ? kz_User_Options_Node_and_Edges : kz_User_Options_Node );
    rOptionsHandler.serialize();
}

_DWFTK_API
void
DWFSegment::close()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    _rSegmentBuilder.getCloseSegmentHandler().serialize();
}

_DWFTK_API
const DWFProperty* const
DWFSegment::findProperty( const DWFString& zName,
                          const DWFString& zCategory )
const
throw()
{
        //
        // pass along to the published object
        // and ignore the default implementation
        //
    if (_pPublishedObject)
    {
        return _pPublishedObject->findProperty( zName, zCategory );
    }
        //
        // this case for unnamed segments
        //
    else if (_pPublishedParent)
    {
        return _pPublishedParent->findProperty( zName, zCategory );
    }

    return NULL;
}

_DWFTK_API
DWFProperty::tMap::Iterator*
DWFSegment::getProperties( const DWFString& zCategory )
throw()
{
        //
        // pass along to the published object
        // and ignore the default implementation
        //
    if (_pPublishedObject)
    {
        return _pPublishedObject->getProperties( zCategory );
    }
        //
        // this case for unnamed segments
        //
    else if (_pPublishedParent)
    {
        return _pPublishedParent->getProperties( zCategory );
    }
    else
    {
        return DWFPropertyContainer::getProperties( zCategory );
    }
}

_DWFTK_API
void
DWFSegment::addProperty( DWFProperty* pProperty )
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

        //
        // pass along to the published object
        // and ignore the default implementation
        //
    if (_pPublishedObject)
    {
        _pPublishedObject->addProperty( pProperty, true );
    }
        //
        // this case for unnamed segments
        //
    else if (_pPublishedParent)
    {
        _pPublishedParent->addProperty( pProperty, true );
    }
        //
        // this means that there are no named segments above this one
        // as such, there is nothing to add this property to
        //
    else
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Cannot add property to non-rooted segment" );
    }
}

_DWFTK_API
void
DWFSegment::addProperty( const DWFProperty& rProperty )
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

        //
        // pass along to the published object
        // and ignore the default implementation
        //
    if (_pPublishedObject)
    {
        _pPublishedObject->addProperty( const_cast<DWFProperty*>(&rProperty), false );
    }
        //
        // this case for unnamed segments
        //
    else if (_pPublishedParent)
    {
        _pPublishedParent->addProperty( const_cast<DWFProperty*>(&rProperty), false );
    }
        //
        // this means that there are no named segments above this one
        // as such, there is nothing to add this property to
        //
    else
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Cannot add property to non-rooted segment" );
    }
}

_DWFTK_API
void
DWFSegment::addPropertyContainer( DWFPropertyContainer* pContainer )
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

        //
        // pass along to the published object
        // and ignore the default implementation
        //
    if (_pPublishedObject)
    {
        _pPublishedObject->addPropertyContainer( pContainer );
    }
        //
        // this case for unnamed segments
        //
    else if (_pPublishedParent)
    {
        _pPublishedParent->addPropertyContainer( pContainer );
    }
        //
        // this means that there are no named segments above this one
        // as such, there is nothing to add this property to
        //
    else
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Cannot add property to non-rooted segment" );
    }
}

_DWFTK_API
void
DWFSegment::referencePropertyContainer( const DWFPropertyContainer& rContainer )
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

        //
        // pass along to the published object
        // and ignore the default implementation
        //
    if (_pPublishedObject)
    {
        _pPublishedObject->referencePropertyContainer( rContainer );
    }
        //
        // this case for unnamed segments
        //
    else if (_pPublishedParent)
    {
        _pPublishedParent->referencePropertyContainer( rContainer );
    }
        //
        // this means that there are no named segments above this one
        // as such, there is nothing to add this property to
        //
    else
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Cannot add property to non-rooted segment" );
    }
}

_DWFTK_API
void
DWFSegment::removeOwnedPropertyContainers( DWFPropertyContainer::tList& rContainerList,
                                           bool                         bMakeReferences )
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

        //
        // pass along to the published object
        // and ignore the default implementation
        //
    if (_pPublishedObject)
    {
        _pPublishedObject->removeOwnedPropertyContainers( rContainerList, bMakeReferences );
    }
        //
        // this case for unnamed segments
        //
    else if (_pPublishedParent)
    {
        _pPublishedParent->removeOwnedPropertyContainers( rContainerList, bMakeReferences );
    }
        //
        // this means that there are no named segments above this one
        //
    else
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Cannot remove property from non-rooted segment" );
    }
}

_DWFTK_API
void
DWFSegment::removeReferencedPropertyContainers( DWFPropertyContainer::tList& rContainerList )
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

        //
        // pass along to the published object
        // and ignore the default implementation
        //
    if (_pPublishedObject)
    {
        _pPublishedObject->removeReferencedPropertyContainers( rContainerList );
    }
        //
        // this case for unnamed segments
        //
    else if (_pPublishedParent)
    {
        _pPublishedParent->removeReferencedPropertyContainers( rContainerList );
    }
        //
        // this means that there are no named segments above this one
        //
    else
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Cannot remove property from non-rooted segment" );
    }
}

_DWFTK_API
void
DWFSegment::removeAllPropertyContainers( DWFPropertyContainer::tList& rContainerList )
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

        //
        // pass along to the published object
        // and ignore the default implementation
        //
    if (_pPublishedObject)
    {
        _pPublishedObject->removeAllPropertyContainers( rContainerList );
    }
        //
        // this case for unnamed segments
        //
    else if (_pPublishedParent)
    {
        _pPublishedParent->removeAllPropertyContainers( rContainerList );
    }
        //
        // this means that there are no named segments above this one
        //
    else
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Cannot remove property from non-rooted segment" );
    }
}

_DWFTK_API
void
DWFSegment::copyProperties( DWFPropertyContainer& rContainer,
                            bool                  bRemoveOwnership,
                            bool                  bMakeReferences )
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

        //
        // pass along to the published object
        // and ignore the default implementation
        //
    if (_pPublishedObject)
    {
        _pPublishedObject->copyProperties( rContainer, bRemoveOwnership, bMakeReferences );
    }
        //
        // this case for unnamed segments
        //
    else if (_pPublishedParent)
    {
        _pPublishedParent->copyProperties( rContainer, bRemoveOwnership, bMakeReferences );
    }
        //
        // this means that there are no named segments above this one
        //
    else
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Cannot copy property to non-rooted segment" );
    }
}

        //
        //
        // GeometryHandlerBuilder Interface
        //
        //

_DWFTK_API
TK_Area_Light&
DWFSegment::getAreaLightHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rGeometryBuilder.getAreaLightHandler();
}

_DWFTK_API
TK_Circle&
DWFSegment::getCircleHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rGeometryBuilder.getCircleHandler();
}

_DWFTK_API
TK_Circle&
DWFSegment::getCircularArcHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rGeometryBuilder.getCircularArcHandler();
}

_DWFTK_API
TK_Circle&
DWFSegment::getCircularChordHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rGeometryBuilder.getCircularChordHandler();
}

_DWFTK_API
TK_Circle&
DWFSegment::getCircularWedgeHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rGeometryBuilder.getCircularWedgeHandler();
}

_DWFTK_API
TK_Cutting_Plane&
DWFSegment::getCuttingPlaneHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rGeometryBuilder.getCuttingPlaneHandler();
}

_DWFTK_API
TK_Point&
DWFSegment::getDistantLightHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rGeometryBuilder.getDistantLightHandler();
}

_DWFTK_API
TK_Ellipse&
DWFSegment::getEllipseHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rGeometryBuilder.getEllipseHandler();
}

_DWFTK_API
TK_Ellipse&
DWFSegment::getEllipticalArcHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rGeometryBuilder.getEllipticalArcHandler();
}

_DWFTK_API
TK_Glyph_Definition&
DWFSegment::getGlyphDefinitionHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rGeometryBuilder.getGlyphDefinitionHandler();
}

_DWFTK_API
TK_Image&
DWFSegment::getImageHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rGeometryBuilder.getImageHandler();
}

_DWFTK_API
TK_Line&
DWFSegment::getLineHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rGeometryBuilder.getLineHandler();
}

_DWFTK_API
TK_Point&
DWFSegment::getLocalLightHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rGeometryBuilder.getLocalLightHandler();
}

_DWFTK_API
TK_NURBS_Curve&
DWFSegment::getNURBSCurveHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rGeometryBuilder.getNURBSCurveHandler();
}


_DWFTK_API
TK_NURBS_Surface&
DWFSegment::getNURBSSurfaceHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rGeometryBuilder.getNURBSSurfaceHandler();
}

_DWFTK_API
TK_PolyCylinder&
DWFSegment::getPolyCylinderHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rGeometryBuilder.getPolyCylinderHandler();
}

_DWFTK_API
TK_Polypoint&
DWFSegment::getPolygonHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rGeometryBuilder.getPolygonHandler();
}

_DWFTK_API
TK_Polypoint&
DWFSegment::getPolylineHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rGeometryBuilder.getPolylineHandler();
}

_DWFTK_API
TK_Shell&
DWFSegment::getShellHandler( bool bTriStripsOnly,
                             bool bDisableOptimization )
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rGeometryBuilder.getShellHandler( bTriStripsOnly, bDisableOptimization );
}

_DWFTK_API
TK_Spot_Light&
DWFSegment::getSpotLightHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rGeometryBuilder.getSpotLightHandler();
}

_DWFTK_API
TK_Text&
DWFSegment::getTextHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rGeometryBuilder.getTextHandler();
}

_DWFTK_API
TK_Text&
DWFSegment::getTextWithEncodingHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rGeometryBuilder.getTextWithEncodingHandler();
}

        //
        //
        // FeatureHandlerBuilder Interface
        //
        //

_DWFTK_API
TK_Texture&
DWFSegment::getTextureHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rFeatureBuilder.getTextureHandler();
}

_DWFTK_API
TK_Matrix&
DWFSegment::getTextureMatrixHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rFeatureBuilder.getTextureMatrixHandler();
}
        //
        //
        // AttributeHandlerBuilder Interface
        //
        //

_DWFTK_API
TK_Color&
DWFSegment::getColorHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rAttributeBuilder.getColorHandler();
}

_DWFTK_API
TK_Color_Map&
DWFSegment::getColorMapHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rAttributeBuilder.getColorMapHandler();
}

_DWFTK_API
TK_Color_RGB&
DWFSegment::getColorRGBHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rAttributeBuilder.getColorRGBHandler();
}

_DWFTK_API
TK_Named&
DWFSegment::getEdgePatternHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rAttributeBuilder.getEdgePatternHandler();
}

_DWFTK_API
TK_Size&
DWFSegment::getEdgeWeightHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rAttributeBuilder.getEdgeWeightHandler();
}

_DWFTK_API
TK_Enumerated&
DWFSegment::getFacePatternHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rAttributeBuilder.getFacePatternHandler();
}

_DWFTK_API
TK_Enumerated&
DWFSegment::getHandednessHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rAttributeBuilder.getHandednessHandler();
}

_DWFTK_API
TK_Heuristics&
DWFSegment::getHeuristicsHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rAttributeBuilder.getHeuristicsHandler();
}

_DWFTK_API
TK_Named&
DWFSegment::getLinePatternHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rAttributeBuilder.getLinePatternHandler();
}

_DWFTK_API
TK_Line_Style&
DWFSegment::getLineStyleHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rAttributeBuilder.getLineStyleHandler();
}

_DWFTK_API
TK_Size&
DWFSegment::getLineWeightHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rAttributeBuilder.getLineWeightHandler();
}

_DWFTK_API
TK_Size&
DWFSegment::getMarkerSizeHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rAttributeBuilder.getMarkerSizeHandler();
}

_DWFTK_API
TK_Enumerated&
DWFSegment::getMarkerSymbolHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rAttributeBuilder.getMarkerSymbolHandler();
}

_DWFTK_API
TK_Matrix&
DWFSegment::getModellingMatrixHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rAttributeBuilder.getModellingMatrixHandler();
}

_DWFTK_API
TK_Rendering_Options&
DWFSegment::getRenderingOptionsHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rAttributeBuilder.getRenderingOptionsHandler();
}

_DWFTK_API
TK_Selectability&
DWFSegment::getSelectabilityHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rAttributeBuilder.getSelectabilityHandler();
}

_DWFTK_API
TK_Enumerated&
DWFSegment::getTextAlignmentHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rAttributeBuilder.getTextAlignmentHandler();
}

_DWFTK_API
TK_Text_Font&
DWFSegment::getTextFontHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rAttributeBuilder.getTextFontHandler();
}

_DWFTK_API
TK_Point&
DWFSegment::getTextPathHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rAttributeBuilder.getTextPathHandler();
}

_DWFTK_API
TK_Size&
DWFSegment::getTextSpacingHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rAttributeBuilder.getTextSpacingHandler();
}

_DWFTK_API
TK_User_Options&
DWFSegment::getUserOptionsHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rAttributeBuilder.getUserOptionsHandler();
}

_DWFTK_API
TK_Unicode_Options&
DWFSegment::getUnicodeOptionsHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rAttributeBuilder.getUnicodeOptionsHandler();
}

_DWFTK_API
TK_Visibility&
DWFSegment::getVisibilityHandler()
throw( DWFException )
{
    if (_bOpen == false)
    {
        _DWFCORE_THROW( DWFUnexpectedException, L"Segment must be open" );
    }

    return _rAttributeBuilder.getVisibilityHandler();
}

#endif

