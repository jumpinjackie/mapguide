//
//  Copyright (C) 2003-2010 by Autodesk, Inc.
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

#include "./CoordinateSpace.h"
#include "./LinearUnit.h"
#include "./Bounds.h"
#include "./View.h"
#include "./EMapConstants.h"
#include "./EMapUtility.h"
using namespace DWFToolkit::DWFEMapExtension;


DWFCoordinateSpace::DWFCoordinateSpace(  )
throw() : DWFXMLBuildable()
        , _zName("")
        , _poUnits(NULL)
        , _poBounds(NULL)
        , _poInitialView(NULL)
        , _poInitialBounds(NULL)
        , _poCallback(NULL)
        , _bIsName(false)
{
}

// Copy Constructor
DWFCoordinateSpace::DWFCoordinateSpace( const DWFCoordinateSpace& copy )
    throw() : DWFXMLBuildable()
            , _zName( copy._zName )
            , _poUnits( DWFCORE_ALLOC_OBJECT( DWFLinearUnit( *copy._poUnits ) ) )
            , _poBounds( DWFCORE_ALLOC_OBJECT( DWFBounds( *copy._poBounds ) ) )
            , _poInitialView( NULL )
            , _poInitialBounds( NULL )
            , _poCallback( NULL )
            , _bIsName( false )
{
    if( copy._poInitialView != NULL )
    {
        _poInitialView = DWFCORE_ALLOC_OBJECT( DWFView( *copy._poInitialView ) );
    }
    if( copy._poInitialBounds != NULL )
    {
        _poInitialBounds = DWFCORE_ALLOC_OBJECT( DWFBounds( *copy._poInitialBounds ) );
    }
}

// basic constructor
DWFCoordinateSpace::DWFCoordinateSpace( const DWFString&     zName,
                                        const DWFLinearUnit& units,
                                        const DWFBounds&     bounds )
    throw() : DWFXMLBuildable()
            , _zName( zName )
            , _poUnits( DWFCORE_ALLOC_OBJECT( DWFLinearUnit( units ) ) )
            , _poBounds( DWFCORE_ALLOC_OBJECT( DWFBounds( bounds ) ) )
            , _poInitialView( NULL )
            , _poInitialBounds( NULL )
            , _poCallback( NULL )
            , _bIsName( false )
{
}

// basic constructor with an initial view
DWFCoordinateSpace::DWFCoordinateSpace( const DWFString&     zName,
                                        const DWFLinearUnit& units,
                                        const DWFBounds&     bounds,
                                        const DWFView*       pInitialView )
    throw() : DWFXMLBuildable()
            , _zName( zName )
            , _poUnits( DWFCORE_ALLOC_OBJECT( DWFLinearUnit( units ) ) )
            , _poBounds( DWFCORE_ALLOC_OBJECT( DWFBounds( bounds ) ) )
            , _poInitialView( NULL )
            , _poInitialBounds( NULL )
            , _poCallback( NULL )
            , _bIsName( false )
{
    if( pInitialView != NULL )
    {
        _poInitialView = DWFCORE_ALLOC_OBJECT( DWFView( *pInitialView ) );
    }
}

// basic constructor with an initial bounds
DWFCoordinateSpace::DWFCoordinateSpace( const DWFString&     zName,
                                        const DWFLinearUnit& units,
                                        const DWFBounds&     bounds,
                                        const DWFBounds*     pInitialBounds )
    throw() : DWFXMLBuildable()
            , _zName( zName )
            , _poUnits( DWFCORE_ALLOC_OBJECT( DWFLinearUnit( units ) ) )
            , _poBounds( DWFCORE_ALLOC_OBJECT( DWFBounds( bounds ) ) )
            , _poInitialView( NULL )
            , _poInitialBounds( NULL )
            , _poCallback( NULL )
            , _bIsName( false )
{
    if( pInitialBounds != NULL )
    {
        _poInitialBounds = DWFCORE_ALLOC_OBJECT( DWFBounds( *pInitialBounds ) );
    }
}

// destructor
DWFCoordinateSpace::~DWFCoordinateSpace() throw()
{
    //delete objects created on the heap
    DWFCORE_FREE_OBJECT( _poUnits );
    DWFCORE_FREE_OBJECT( _poBounds );
    DWFCORE_FREE_OBJECT( _poInitialView );
    DWFCORE_FREE_OBJECT( _poInitialBounds );
}

// assignment operator.
DWFCoordinateSpace& DWFCoordinateSpace::operator=( const DWFCoordinateSpace& rho )
throw()
{
    // delete all internal objects created on the heap
    DWFCORE_FREE_OBJECT( _poUnits );
    DWFCORE_FREE_OBJECT( _poBounds );
    DWFCORE_FREE_OBJECT( _poInitialView );
    DWFCORE_FREE_OBJECT( _poInitialBounds );

    _poUnits = DWFCORE_ALLOC_OBJECT( DWFLinearUnit( *rho._poUnits ) );
    _poBounds = DWFCORE_ALLOC_OBJECT( DWFBounds( *rho._poBounds ) );

    if( rho._poInitialView != NULL )
    {
        _poInitialView = DWFCORE_ALLOC_OBJECT( DWFView( *rho._poInitialView ) );
    }

    if( rho._poInitialBounds != NULL )
    {
        _poInitialBounds = DWFCORE_ALLOC_OBJECT( DWFBounds( *rho._poInitialBounds ) );
    }

    return *this;
}

void DWFCoordinateSpace::parseAttributeList( const char** ppAttributeList )
    throw( DWFException )
{
}

// property accessors
const DWFString& DWFCoordinateSpace::name() const throw()
{
    return _zName;
}


// returns the units
const DWFLinearUnit* const DWFCoordinateSpace::units() const throw()
{
    return _poUnits;
}
// returns the bounds
const DWFBounds* const DWFCoordinateSpace::bounds() const throw()
{
    return _poBounds;
}

// returns the initial view
const DWFView* const DWFCoordinateSpace::initialView() const throw()
{
    return _poInitialView;
}

// returns the initial bounds
const DWFBounds* const DWFCoordinateSpace::initialBounds() const throw()
{
    return _poInitialBounds;
}

// XMLCallback support
void DWFCoordinateSpace::notifyStartElement( const char*  zName,
                    const char** ppAttributeList ) throw()
{
    const char* zElement = DWFEMapUtility::offsetKnownPrefix(zName);

    if( _poCallback )
    {
        _poCallback->notifyStartElement(zName, ppAttributeList);
    }
    else if( DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                    DWFEMAPXML::kzElement_Name ) == 0)
    {
        // clear any existing name
        _zName.destroy();
        _bIsName = true;
    }
    else if ( DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                    DWFEMAPXML::kzElement_Units ) == 0)
    {
        _poUnits = DWFCORE_ALLOC_OBJECT( DWFLinearUnit() );
        _poCallback = _poUnits;
    }
    else if ( DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                    DWFEMAPXML::kzElement_Bounds ) == 0)
    {
        _poBounds = DWFCORE_ALLOC_OBJECT( DWFBounds() );
        _poCallback = _poBounds;
    }
    else if ( DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                    DWFEMAPXML::kzElement_InitialView ) == 0)
    {
        _poInitialView = DWFCORE_ALLOC_OBJECT( DWFView() );
        _poCallback = _poInitialView;
    }
    else if ( DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                    DWFEMAPXML::kzElement_InitialBounds ) == 0)
    {
        _poInitialBounds = DWFCORE_ALLOC_OBJECT( DWFBounds() );
        _poCallback = _poInitialBounds;
    }
}


void DWFCoordinateSpace::notifyEndElement( const char* zName ) throw()
{
    const char* zElement = DWFEMapUtility::offsetKnownPrefix(zName);

    if(_bIsName && DWFCORE_COMPARE_ASCII_STRINGS(zElement,  // Redundant check
                                    DWFEMAPXML::kzElement_Name) == 0)
    {
        _bIsName = false;
    }
    else if( DWFCORE_COMPARE_ASCII_STRINGS(zElement, DWFEMAPXML::kzElement_Units        ) == 0
          || DWFCORE_COMPARE_ASCII_STRINGS(zElement, DWFEMAPXML::kzElement_Bounds       ) == 0
          || DWFCORE_COMPARE_ASCII_STRINGS(zElement, DWFEMAPXML::kzElement_InitialView  ) == 0
          || DWFCORE_COMPARE_ASCII_STRINGS(zElement, DWFEMAPXML::kzElement_InitialBounds) == 0)
    {
        _poCallback = NULL;
    }
    else if( _poCallback)
    {
        _poCallback->notifyEndElement(zName);
    }
}


void DWFCoordinateSpace::notifyStartNamespace( const char* zPrefix,
                                               const char* zURI ) throw()
{
    if(_poCallback)
    {
        _poCallback->notifyStartNamespace(zPrefix,zURI);
    }
}


void DWFCoordinateSpace::notifyEndNamespace( const char* zPrefix ) throw()
{
    if( _poCallback)
    {
        _poCallback->notifyEndNamespace(zPrefix);
    }
}


void DWFCoordinateSpace::notifyCharacterData( const char* zCData, int nLength )
                                                  throw()
{
    if( _poCallback)
    {
        _poCallback->notifyCharacterData(zCData,nLength);
    }
    else if ( _bIsName)
    {
        // expat will call this callback multiple times if the
        // original string contains XML-encoded bytes
        this->_zName.append( zCData, nLength );
    }
}


// DWFXMLSerializable support
void DWFCoordinateSpace::serializeXML( DWFXMLSerializer& rSerializer,
            unsigned int nFlags ) throw( DWFException )
{
    // Element Coordinate Space
    rSerializer.startElement(DWFEMAPXML::kzElement_CoordinateSpace, DWFEMAPXML::kzNamespace_EMap);
    {
        // Element Name
        rSerializer.startElement(DWFEMAPXML::kzElement_Name, DWFEMAPXML::kzNamespace_EMap);
        {
            rSerializer.addCData(this->_zName);
        }
        rSerializer.endElement();

        // Element Units
        if( _poUnits != NULL)
        {
            _poUnits->serializeXML(rSerializer, nFlags);
        }

        // Element Bounds
        if( _poBounds != NULL)
        {
            _poBounds->serializeXML(rSerializer, nFlags);
        }

        // Element InitialView
        if( _poInitialView != NULL)
        {
            _poInitialView->serializeXML(rSerializer, nFlags);
        }

        // Element InitialBounds
        if( _poInitialBounds != NULL)
        {
            // TODO: determine the right way to serialize the bounds using a different start element name
            wchar_t zTempBuffer[_DWFEMAPTK_TEMP_BUFFER_BYTES];
            rSerializer.startElement(DWFEMAPXML::kzElement_InitialBounds, DWFEMAPXML::kzNamespace_EMap);
            {
                // MinX element
                rSerializer.startElement(DWFEMAPXML::kzElement_MinX, DWFEMAPXML::kzNamespace_EMap);
                {
                    _DWFCORE_SWPRINTF( zTempBuffer, _DWFEMAPTK_TEMP_BUFFER_BYTES, L"%.17g", _poInitialBounds->minX() );
                    rSerializer.addCData( DWFString::RepairDecimalSeparators(zTempBuffer) );
                }
                rSerializer.endElement();

                // MaxX element
                rSerializer.startElement(DWFEMAPXML::kzElement_MaxX, DWFEMAPXML::kzNamespace_EMap);
                {
                    _DWFCORE_SWPRINTF( zTempBuffer, _DWFEMAPTK_TEMP_BUFFER_BYTES, L"%.17g", _poInitialBounds->maxX() );
                    rSerializer.addCData( DWFString::RepairDecimalSeparators(zTempBuffer) );
                }
                rSerializer.endElement();

                // MinY element
                rSerializer.startElement(DWFEMAPXML::kzElement_MinY, DWFEMAPXML::kzNamespace_EMap);
                {
                    _DWFCORE_SWPRINTF( zTempBuffer, _DWFEMAPTK_TEMP_BUFFER_BYTES, L"%.17g", _poInitialBounds->minY() );
                    rSerializer.addCData( DWFString::RepairDecimalSeparators(zTempBuffer) );
                }
                rSerializer.endElement();

                // MaxY element
                rSerializer.startElement(DWFEMAPXML::kzElement_MaxY, DWFEMAPXML::kzNamespace_EMap);
                {
                    _DWFCORE_SWPRINTF( zTempBuffer, _DWFEMAPTK_TEMP_BUFFER_BYTES, L"%.17g", _poInitialBounds->maxY() );
                    rSerializer.addCData( DWFString::RepairDecimalSeparators(zTempBuffer) );
                }
                rSerializer.endElement();
            }
            rSerializer.endElement(); //end InitialBounds element
        }
    }
    rSerializer.endElement(); // End CoordinateSpace
}

// End of file
