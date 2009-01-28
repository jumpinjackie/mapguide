//
//  Copyright (C) 2003-2009 by Autodesk, Inc.
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

#include "./EMapSection.h"
#include "./Background.h"
#include "./CoordinateSpace.h"
#include "./Layer.h"
#include "./LayerGroup.h"
#include "./EMapConstants.h"
#include "./EMapUtility.h"
#include "../EMapVersion.h"
using namespace DWFToolkit::DWFEMapExtension;


DWFEMapSection::DWFEMapSection( const DWFString& zName,
                                const DWFString& zTitle,
                                DWFPackageReader* pPackageReader )
throw() : DWFSection( _DWF_FORMAT_EMAP_TYPE_STRING,
                                            zName,
                                            zTitle,
                                            pPackageReader )
         , DWFEMapSectionDescriptorReader(pPackageReader)
         , _poBackground(NULL)
         , _poCoordinateSpace(NULL)
         , _nFlag(0x11)
         , _poPackageReader(pPackageReader)
{
}

DWFEMapSection::DWFEMapSection( const DWFString& zTitle,
                                const DWFString& zObjectID,
                                const DWFSource& rSource,
                                const DWFBackground& background,
                                const DWFCoordinateSpace& space,
                                const DWFString& zUri )
throw() : DWFSection( _DWF_FORMAT_EMAP_TYPE_STRING,
                      zTitle,
                      zObjectID,
                      _DWF_FORMAT_EMAP_VERSION_CURRENT_FLOAT,
                      0,   // Plot order is not needed for EMap
                      rSource )
            , DWFEMapSectionDescriptorReader()
            , _zUri(zUri)
            , _poBackground(DWFCORE_ALLOC_OBJECT(DWFBackground(background)))
            , _poCoordinateSpace(DWFCORE_ALLOC_OBJECT(DWFCoordinateSpace(space)))
            , _nFlag(0x11)
            , _poPackageReader(NULL)
{
}

// Destructor
DWFEMapSection::~DWFEMapSection()
throw()
{
    // delete
    DWFCORE_FREE_OBJECT( _poBackground );
    DWFCORE_FREE_OBJECT( _poCoordinateSpace );

    // delete both the layer and Layergroup Collections
    for( LayerCollection::iterator itr = _oLayers.begin();
                        itr != _oLayers.end(); itr++)
    {
        DWFCORE_FREE_OBJECT(*itr);
    }

    for( LayerGroupCollection::iterator itr = _oLayerGroups.begin();
                        itr != _oLayerGroups.end(); itr++)
    {
        DWFCORE_FREE_OBJECT(*itr);
    }
}


const DWFBackground* const DWFEMapSection::background()
throw()
{
    return _poBackground;
}


const DWFCoordinateSpace* const DWFEMapSection::coordinateSpace()
throw()
{
    return _poCoordinateSpace;
}


const DWFString& DWFEMapSection::uri() const
throw()
{
    return _zUri;
}


const DWFEMapLayer* const DWFEMapSection::findLayerByName(
                                            const DWFString& zName )
throw()
{
    const DWFEMapLayer* pLayer = NULL;
    for( LayerCollection::const_iterator itr = _oLayers.begin();
                            itr != _oLayers.end(); itr++ )
    {

         if( (*itr)->name() == zName )
         {
            pLayer = *itr;
            break;
         }
    }
    return pLayer;
}


// Access a Layer by its ObjectID
const DWFEMapLayer* const DWFEMapSection::findLayerByObjectID(
                                        const DWFString& zObjectID )
throw()
{
    const DWFEMapLayer* pLayer = NULL;
    for( LayerCollection::const_iterator itr = _oLayers.begin();
                            itr != _oLayers.end(); itr++ )
    {

         if ( (*itr)->objectID() == zObjectID )
         {
            pLayer = *itr;
            break;
         }
    }
    return pLayer;
}


const DWFEMapLayer* DWFEMapSection::getLayer( size_t index ) const
throw()
{
    DWFEMapLayer* pRetValue = NULL;

    if( index < _oLayers.size() )
    {
        pRetValue = _oLayers.at( index );
    }

    return pRetValue;
}

// returns the count of the Layers
size_t DWFEMapSection::layerCount() const
throw()
{
    return _oLayers.size();
}


// adds a layer to the EMap section
void DWFEMapSection::addLayer( DWFEMapLayer* pLayer )
throw( DWFException )
{
    if ( pLayer != NULL )
    {
        _oLayers.push_back( pLayer );
    }
}


// Access a layer group by its name
const DWFEMapLayerGroup* const DWFEMapSection::findLayerGroupByName(
                                                 const DWFString& zName ) const
throw()
{
    const DWFEMapLayerGroup* pLayerGroup = NULL;
    for( LayerGroupCollection::const_iterator itr = _oLayerGroups.begin();
                                itr != _oLayerGroups.end(); itr++ )
    {
         if ( (*itr)->name() == zName )
         {
            pLayerGroup = *itr;
            break;
         }
    }

    return pLayerGroup;
}


// Access a layer group by its objectID
const DWFEMapLayerGroup* const DWFEMapSection::findLayerGroupByObjectID(
                                    const DWFString& zObjectID )
const throw()
{
    const DWFEMapLayerGroup* pLayerGroup = NULL;
    for( LayerGroupCollection::const_iterator itr = _oLayerGroups.begin();
                            itr != _oLayerGroups.end(); itr++ )
    {
        pLayerGroup = *itr;
        if ( pLayerGroup->objectID() == zObjectID )
        {
           break;
        }
    }

    return pLayerGroup;
}


// access a layer group for the given index in the EMap section.
// returns NULL if the index is out of range.
const DWFEMapLayerGroup* DWFEMapSection::getLayerGroup( size_t index ) const
throw()
{
    DWFEMapLayerGroup* pRetValue = NULL;

    if( index < _oLayerGroups.size() )
    {
        pRetValue = _oLayerGroups.at( index );
    }

    return pRetValue;
}

size_t DWFEMapSection::layerGroupCount() const
throw()
{
    return _oLayerGroups.size();
}

// adds a layer group to the EMap section
void DWFEMapSection::addLayerGroup( DWFEMapLayerGroup* pLayerGroup )
throw( DWFException )
{
    if ( pLayerGroup != NULL )
    {
        _oLayerGroups.push_back( pLayerGroup );
    }
}


// XMLCallback support defined in DWFEMapSectionDescriptorReader
// Note this Callback differs from the other elements as it even
// handles itself
void DWFEMapSection::notifyStartElement( const char*  zName,
                                         const char** ppAttributeList )
throw()
{
    // Parsing is invalid if PackageReader is NULL
    if ( _poPackageReader == NULL )
    {
        return;
    }

    const char* zElement = DWFEMapUtility::offsetKnownPrefix( zName );

    switch (_nFlag)
    {
    case 0x11:  // default
        {
            if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                                        DWFEMAPXML::kzElement_Map ) == 0  )
            {
                _nFlag = 0x00; // can start processing other elements
                this->_processAttributes( ppAttributeList );
            }
            break;
        }
    case 0x00:  // at the Map element
        {
            if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                                        DWFEMAPXML::kzElement_Name ) == 0 )
            {
                // clear any existing title
                _zTitle.destroy();
                _nFlag = 0x01;
            }
            else if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                                        DWFEMAPXML::kzElement_Background) == 0 )
            {
                _nFlag = 0x02;
                _poBackground = DWFCORE_ALLOC_OBJECT( DWFBackground() );
            }
            else if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                                        DWFEMAPXML::kzElement_CoordinateSpace ) == 0 )
            {
                _nFlag = 0x03;
                _poCoordinateSpace = DWFCORE_ALLOC_OBJECT( DWFCoordinateSpace() );
            }
            else if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                                        DWFXML::kzElement_Properties ) == 0 )
            {
                _nFlag = 0x04;
            }
            else if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                                        DWFEMAPXML::kzElement_Layers ) == 0 )
            {
                _nFlag = 0x05;
            }
            else if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                                        DWFEMAPXML::kzElement_LayerGroups ) == 0 )
            {
                _nFlag = 0x06;
            }
            else if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                                        DWFXML::kzElement_Resources ) == 0 )
            {
                _nFlag = 0x07;
            }
            break;
        }
    case 0x02:  // at the Background element
        {
            _poBackground->notifyStartElement( zName, ppAttributeList );
            break;
        }
    case 0x03:  // at the CoordinateSpace element
        {
            _poCoordinateSpace->notifyStartElement( zName, ppAttributeList );
            break;
        }
    case 0x04:  // in the Properties collection
        {
            if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                                DWFXML::kzElement_Property ) == 0 )
            {
                DWFProperty* pProperty = DWFCORE_ALLOC_OBJECT( DWFProperty() );
                pProperty->parseAttributeList( ppAttributeList );
                this->addProperty( pProperty, true );
            }
            break;
        }
    case 0x05:  // in the Layers collection
        {
            if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                                        DWFEMAPXML::kzElement_Layer ) == 0 )
            {
                DWFEMapLayer* pLayer = DWFCORE_ALLOC_OBJECT(
                                        DWFEMapLayer( _poPackageReader ) );
                pLayer->parseAttributeList( ppAttributeList );
                _poCallback = pLayer;
            }
            else if ( _poCallback ) // redundant check : if false the XML is corrupt
            {
                _poCallback->notifyStartElement( zName, ppAttributeList );
            }
            break;
        }
    case 0x06:  // in the LayerGroups collection
        {
            if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                                        DWFEMAPXML::kzElement_LayerGroup ) == 0 )
            {
                DWFEMapLayerGroup* pLayerGroup = DWFCORE_ALLOC_OBJECT(
                                                DWFEMapLayerGroup( _poPackageReader ) );
                pLayerGroup->parseAttributeList( ppAttributeList );
                _poCallback = pLayerGroup;
            }
            else if ( _poCallback )
            {
                _poCallback->notifyStartElement( zName, ppAttributeList );
            }
            break;
        }
    case 0x07:  // in the Resources collection
        {
            if ( DWFCORE_COMPARE_ASCII_STRINGS(zElement, DWFXML::kzElement_Resource) == 0 )
            {
                DWFResource* pResource = buildResource( ppAttributeList, _poPackageReader );
                this->addResource( pResource, true );
            }
            else if ( DWFCORE_COMPARE_ASCII_STRINGS(zElement, DWFXML::kzElement_GraphicResource) == 0 )
            {
                DWFGraphicResource* pResource = buildGraphicResource( ppAttributeList, _poPackageReader );
                this->addResource( (DWFResource*)pResource, true );
            }
            else if ( DWFCORE_COMPARE_ASCII_STRINGS(zElement, DWFXML::kzElement_ImageResource) == 0 )
            {
                DWFImageResource* pResource = buildImageResource( ppAttributeList,_poPackageReader );
                this->addResource( (DWFResource*)pResource, true );
            }
            break;
        }
    }
}


void DWFEMapSection::notifyEndElement( const char* zName )
throw()
{
    if( _poPackageReader == NULL)
    {
        return;
    }

    const char* zElement = DWFEMapUtility::offsetKnownPrefix( zName );

    switch( _nFlag )
    {
    case 0x00:  // in the Map element
        {
            if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                                            DWFEMAPXML::kzElement_Map ) == 0 )
            {
                _nFlag = 0x11;
            }
            break;
        }
    case 0x01:  // in the Name element
        {
            _nFlag = 0x00;  // reset the flag
            break;
        }
    case 0x02:  // in the Background element
        {
            if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                                            DWFEMAPXML::kzElement_Background ) == 0 )
            {
                _nFlag = 0x00;  // reset the flag
            }
            else
            {
                _poBackground->notifyEndElement( zName );
            }
            break;
        }
    case 0x03:  // in the CoordinateSpace element
        {
            if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                                     DWFEMAPXML::kzElement_CoordinateSpace ) == 0)
            {
                _nFlag = 0x00;  // reset the flag
            }
            else
            {
                _poCoordinateSpace->notifyEndElement( zName );
            }
            break;
        }
    case 0x04:  // in the Properties collection
        {
            if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                            DWFXML::kzElement_Properties ) == 0)
            {
                _nFlag = 0x00;  // reset the flag
            }
            break;
        }
    case 0x05:  // in the Layers collection
        {
            if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                                     DWFEMAPXML::kzElement_Layer ) == 0 )
            {
                DWFEMapLayer* pLayer = static_cast<DWFEMapLayer*>( _poCallback );
                this->_oLayers.push_back( pLayer );
                _poCallback = NULL;
            }
            else if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                                     DWFEMAPXML::kzElement_Layers ) == 0 )
            {
                _nFlag = 0x00;  // reset the flag
            }
            else if ( _poCallback )
            {
                _poCallback->notifyEndElement( zName );
            }
            break;
        }
    case 0x06:  // in the LayerGroups collection
        {
            if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                                     DWFEMAPXML::kzElement_LayerGroup ) == 0)
            {
                DWFEMapLayerGroup* pLayerGroup = static_cast<DWFEMapLayerGroup*>( _poCallback );
                this->_oLayerGroups.push_back( pLayerGroup );
                _poCallback = NULL;
            }
            else if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                                     DWFEMAPXML::kzElement_LayerGroups ) == 0)
            {
                _nFlag = 0x00;  // reset the flag
            }
            else if ( _poCallback )
            {
                _poCallback->notifyEndElement( zName );
            }
            break;
        }
    case 0x07:  // in the Resources collection
        {
            if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                            DWFXML::kzElement_Resources ) == 0)
            {
                _nFlag = 0x00;  // reset the flag
            }
            break;
        }
    }
}

void DWFEMapSection::notifyCharacterData( const char* zCData, int nLength )
throw()
{
    switch ( _nFlag )
    {
    case 0x01:  // in the Name element
        {
            // although it's the name element, it's stored in the title variable

            // expat will call this callback multiple times if the
            // original string contains XML-encoded bytes
            this->_zTitle.append( zCData, nLength );
            break;
        }
    case 0x02:  // in the Background element
        {
            _poBackground->notifyCharacterData( zCData, nLength );
            break;
        }
    case 0x03:  // in the CoordinateSpace element
        {
            _poCoordinateSpace->notifyCharacterData( zCData, nLength );
            break;
        }
    case 0x05:  // in the Layers collection
    case 0x06:  // in the LayerGroups collection
        {
            _poCallback->notifyCharacterData( zCData, nLength );
        }
    }
}

void DWFEMapSection::notifyStartNamespace( const char* zPrefix,
                                           const char* zURI )
throw()
{
    // NA
}

void DWFEMapSection::notifyEndNamespace( const char* zPrefix )
throw()
{
    // NA
}

const DWFResource&
DWFEMapSection::readDescriptor( DWFSectionDescriptorReader*
                                pSectionDescriptorReader ) const
throw( DWFException )
{
    return DWFSection::readDescriptor( ( pSectionDescriptorReader
                                       ? pSectionDescriptorReader
                                       : ( DWFEMapSectionDescriptorReader* )this ) );
}

// DWFXMLSerializable support
void DWFEMapSection::serializeXML(  DWFXMLSerializer& rSerializer,
                                    unsigned int nFlags )
throw( DWFException )
{
    if( nFlags & DWFPackageWriter::eDescriptor )
    {
        wchar_t zTempBuffer[_DWFEMAPTK_TEMP_BUFFER_BYTES];
        DWFString zTempString;
        rSerializer.startElement( DWFEMAPXML::kzElement_Map,
                                  DWFEMAPXML::kzNamespace_EMap );
        {
            // attributes
            {
                _DWFCORE_SWPRINTF( zTempBuffer, _DWFEMAPTK_TEMP_BUFFER_BYTES, L"%#0.2g", _nVersion );

                // EMap namespace
                zTempString.assign( L"DWF-eMap:" );
                zTempString.append( DWFString::RepairDecimalSeparators(zTempBuffer) );

                rSerializer.addAttribute( L"eMap", zTempString, L"xmlns:" );

                // DWF namespace addition
                zTempString.assign( L"dwf:" );
                zTempString.append( _DWF_FORMAT_VERSION_CURRENT_WIDE_STRING );

                rSerializer.addAttribute( L"dwf", zTempString, L"xmlns:" );

                // Attribute version    :Optional
                rSerializer.addAttribute( DWFXML::kzAttribute_Version, zTempBuffer );

                // Attribute objectId   :Required
                rSerializer.addAttribute( DWFEMAPXML::kzAttribute_ObjectID, _zObjectID );

                // Attribute uri        :Optional
                if( _zUri != L"" )
                {
                    rSerializer.addAttribute( DWFEMAPXML::kzAttribute_Uri, _zUri );
                }
            }

            //
            // Name element
            // : do it here
            rSerializer.startElement( DWFEMAPXML::kzElement_Name,
                                      DWFEMAPXML::kzNamespace_EMap );
            {
                // although it's the name element, it's stored in the title variable
                rSerializer.addCData( this->_zTitle );
            }

            rSerializer.endElement();

            //
            // Background element
            //
            if( _poBackground != NULL )
            {
                _poBackground->serializeXML( rSerializer, nFlags );
            }

            //
            // CoordinateSpace element
            // : delegate to the CoordinateSpace element
            if( _poCoordinateSpace != NULL )
            {
                _poCoordinateSpace->serializeXML( rSerializer, nFlags );
            }

            //
            // Layers element
            //
            if( _oLayers.size() > 0)
            {
                rSerializer.startElement( DWFEMAPXML::kzElement_Layers,
                                          DWFEMAPXML::kzNamespace_EMap );
                {
                    for( LayerCollection::const_iterator itr = _oLayers.begin();
                            itr != _oLayers.end(); itr++ )
                    {
                        (*itr)->serializeXML( rSerializer, nFlags );
                    }
                }
                rSerializer.endElement();
            }

            //
            // LayerGroups element
            //
            if( _oLayerGroups.size() > 0)
            {
                rSerializer.startElement( DWFEMAPXML::kzElement_LayerGroups,
                                          DWFEMAPXML::kzNamespace_EMap );
                {
                    for( LayerGroupCollection::const_iterator itr = _oLayerGroups.begin();
                            itr != _oLayerGroups.end(); itr++ )
                    {
                        (*itr)->serializeXML( rSerializer, nFlags );
                    }
                }
                rSerializer.endElement();
            }

            //
            // base implementation
            //
            DWFSection::serializeXML( rSerializer, nFlags );
        }
        rSerializer.endElement(); // End Map element
    }
    else if( nFlags & DWFPackageWriter::eManifest )// manifest generation
    {
        DWFSection::serializeXML( rSerializer, nFlags );
    }
}

void DWFEMapSection::_processAttributes( const char** ppAttributeList )
throw()
{
    unsigned char nFlag = 0;
    size_t iAttrib = 0;
    const char* pAttrib = NULL;

    // process attributes
    for( ; ppAttributeList[iAttrib]!= NULL; iAttrib +=2 )
    {
        pAttrib = DWFEMapUtility::offsetKnownPrefix( ppAttributeList[iAttrib] );

        if( !( nFlag & 0x01 ) && DWFCORE_COMPARE_ASCII_STRINGS( pAttrib,
                                    DWFEMAPXML::kzAttribute_ObjectID ) == 0 )
        {
            nFlag |= 0x01;
            this->_zObjectID = ppAttributeList[iAttrib + 1];
        }
        else if( !( nFlag & 0x02 ) && DWFCORE_COMPARE_ASCII_STRINGS( pAttrib,
                               DWFEMAPXML::kzAttribute_Uri ) == 0 )
        {
            nFlag |= 0x02;
            this->_zUri = ppAttributeList[iAttrib + 1];
        }
        else if( !( nFlag & 0x04 ) && DWFCORE_COMPARE_ASCII_STRINGS( pAttrib,
                                        DWFXML::kzAttribute_Version ) == 0 )
        {
            nFlag |= 0x04;
            this->_nVersion = DWFString::StringToDouble( ppAttributeList[iAttrib + 1] );
        }
    }
}


DWFEMapSection::Factory::Factory()
throw() : DWFSection::Factory( _DWF_FORMAT_EMAP_TYPE_STRING )
{
}


DWFEMapSection::Factory::~Factory()
throw()
{
}


DWFSection* DWFEMapSection::Factory::build( const DWFString&  zName,
                                            const DWFString&  zTitle,
                                            DWFPackageReader* pPackageReader )
throw( DWFException )
{
    DWFSection* pSection = DWFCORE_ALLOC_OBJECT(
                                        DWFEMapSection( zName,
                                                        zTitle,
                                                        pPackageReader ) );
    return pSection;
}

// End of file.
