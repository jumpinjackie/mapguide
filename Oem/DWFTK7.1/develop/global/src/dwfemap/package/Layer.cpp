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

#include "./Layer.h"
#include "./EMapConstants.h"
#include "./EMapUtility.h"
#include "./UIGraphic.h"
#include "./ScaleRange.h"
using namespace DWFToolkit::DWFEMapExtension;


// constructor used with XMLBuildable
DWFEMapLayer::DWFEMapLayer( DWFPackageReader* pPackageReader )
                           throw() : DWFXMLBuildable()
                                   , _zName("")
                                   , _zObjectID("")
                                   , _zGroupObjectID("")
                                   , _bVisible(true)
                                   , _bSelectable(true)
                                   , _bEditable(true)
                                   , _poUIGraphic(NULL)
                                   , _poPackageReader(pPackageReader)
                                   , _poCallback(NULL)
                                   , _nFlag(0x00)
{
}


// basic constructor
DWFEMapLayer::DWFEMapLayer( const DWFString& zName,
                            const DWFString& zObjectID,
                            const DWFString& zGroupObjectID,
                            bool bVisible,
                            bool bSelectable,
                            bool bEditable,
                            DWFUIGraphic* pUIGraphic )
                            throw() : DWFXMLBuildable()
                                    , _zName(zName)
                                    , _zObjectID(zObjectID)
                                    , _zGroupObjectID(zGroupObjectID)
                                    , _bVisible(bVisible)
                                    , _bSelectable(bSelectable)
                                    , _bEditable(bEditable)
                                    , _poUIGraphic(pUIGraphic)
                                    , _poPackageReader(NULL)
                                    , _poCallback(NULL)
                                    , _nFlag(0x00)
{
}

// destructor
DWFEMapLayer::~DWFEMapLayer() throw()
{
    // delete objects created on the heap
    DWFCORE_FREE_OBJECT( _poUIGraphic );

    // delete all scaleRanges the collection
    for( ScaleRangeCollection::iterator itr = _oScaleRanges.begin();
                        itr != _oScaleRanges.end(); itr++)
    {
        DWFCORE_FREE_OBJECT(*itr);
    }
}


_DWFTK_API void DWFEMapLayer::parseAttributeList( const char** ppAttributeList )
throw( DWFException )
{
    unsigned char nFlag = 0x00;
    size_t iAttrib = 0;
    const char* pAttrib = NULL;

    // loop over the collection of attributes
    for(;ppAttributeList[iAttrib]!= NULL; iAttrib += 2)
    {
        pAttrib = DWFEMapUtility::offsetKnownPrefix(ppAttributeList[iAttrib]);

        if (!(nFlag & 0x01) && DWFCORE_COMPARE_ASCII_STRINGS(pAttrib,
                                    DWFEMAPXML::kzAttribute_ObjectID ) == 0)
        {
            nFlag |= 0x01;
            this->_zObjectID = ppAttributeList[iAttrib + 1];
        }
        else if (!(nFlag & 0x02) && DWFCORE_COMPARE_ASCII_STRINGS(pAttrib,
                               DWFEMAPXML::kzAttribute_GroupObjectID ) == 0)
        {
            nFlag |= 0x02;
            this->_zGroupObjectID = ppAttributeList[iAttrib + 1];
        }
        else if (!(nFlag & 0x04) && DWFCORE_COMPARE_ASCII_STRINGS(pAttrib,
                                  DWFEMAPXML::kzAttribute_Selectable ) == 0)
        {
            nFlag |= 0x04;
            this->_bSelectable = DWFEMapUtility::convertStringToBool
                                            (ppAttributeList[iAttrib + 1]);
        }
        else if (!(nFlag & 0x08) && DWFCORE_COMPARE_ASCII_STRINGS(pAttrib,
                                    DWFEMAPXML::kzAttribute_Visible ) == 0)
        {
            nFlag |= 0x08;
            this->_bVisible = DWFEMapUtility::convertStringToBool
                                            (ppAttributeList[iAttrib + 1]);
        }
        else if (!(nFlag & 0x10) && DWFCORE_COMPARE_ASCII_STRINGS(pAttrib,
                                    DWFEMAPXML::kzAttribute_Editable ) == 0)
        {
            nFlag |= 0x10;
            this->_bEditable = DWFEMapUtility::convertStringToBool
                                            (ppAttributeList[iAttrib + 1]);
        }
    }
}

// property accessors
const DWFString& DWFEMapLayer::name() const throw()
{
    return _zName;
}

const DWFString& DWFEMapLayer::objectID() const throw()
{
    return _zObjectID;
}

const DWFString& DWFEMapLayer::groupObjectID() const throw()
{
    return _zGroupObjectID;
}

bool DWFEMapLayer::visible() const throw()
{
    return _bVisible;
}

bool DWFEMapLayer::selectable() const throw()
{
    return _bSelectable;
}

bool DWFEMapLayer::editable() const throw()
{
    return _bEditable;
}

// returns the UI graphic
const DWFUIGraphic* const DWFEMapLayer::uiGraphic() const throw()
{
    return _poUIGraphic;
}


// finds a scale range by its object ID
const DWFScaleRange* const DWFEMapLayer::findScaleRangeByObjectID(
                    const DWFString& zObjectID ) const throw()
{
    const DWFScaleRange* pRetScale = NULL;
    for( ScaleRangeCollection::const_iterator itr = _oScaleRanges.begin();
                        itr != _oScaleRanges.end(); itr++)
    {
        if ( (*itr)->objectID() == zObjectID)
        {
            pRetScale = *itr;
            break;
        }
    }
    return pRetScale;
}

const DWFScaleRange* DWFEMapLayer::getScaleRange( size_t index ) const
throw()
{
    DWFScaleRange* pRetValue = NULL;

    if ( index < _oScaleRanges.size() )
    {
        pRetValue = _oScaleRanges.at( index );
    }

    return pRetValue;
}

        // returns the ScaleRangeCount
size_t DWFEMapLayer::scaleRangeCount() const
throw()
{
    return _oScaleRanges.size();
}

// adds a scale range to the layer
void DWFEMapLayer::addScaleRange(DWFScaleRange* pScaleRange) throw(DWFException)
{
    if (pScaleRange != NULL)
    {
        _oScaleRanges.push_back(pScaleRange);
    }
}


// XMLCallback support
void DWFEMapLayer::notifyStartElement( const char*  zName,
                                const char** ppAttributeList ) throw()
{
    // Parsing is invalid if PackageReader is NULL
    if (_poPackageReader == NULL)
    {
        return;
    }

    const char* zElement = DWFEMapUtility::offsetKnownPrefix(zName);

    if ( _nFlag == 0x00 )
    {
        if ( DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                    DWFEMAPXML::kzElement_UIGraphic ) == 0)
        {
            _poUIGraphic = new DWFUIGraphic(_poPackageReader);
            _poUIGraphic->parseAttributeList(ppAttributeList);
            _nFlag = 0x01;
        }
        else if ( DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                    DWFEMAPXML::kzElement_ScaleRanges) == 0)
        {
            _nFlag = 0x02;
        }
        else if ( DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                    DWFXML::kzElement_Properties) == 0)
        {
            _nFlag = 0x03;
        }
        else if ( DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                    DWFEMAPXML::kzElement_Name) == 0)
        {
            // clear any existing name
            _zName.destroy();
            _nFlag = 0x04;
        }
    }
    // Pass control to the UIGraphic element
    else if ( _nFlag == 0x01 )
    {
        _poUIGraphic->notifyStartElement(zName,ppAttributeList);
    }
    // In a ScaleRanges Collection
    else if ( _nFlag == 0x02 )
    {
        if ( _poCallback == NULL)
        {
            if ( DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                    DWFEMAPXML::kzElement_ScaleRange) == 0)
            {
                DWFScaleRange* scaleRange = DWFCORE_ALLOC_OBJECT( DWFScaleRange(_poPackageReader));
                scaleRange->parseAttributeList( ppAttributeList );
                _poCallback = scaleRange;
            }
        }
        else // Pass control to the current ScaleRangeObject
        {
           _poCallback->notifyStartElement(zName,ppAttributeList);
        }
    }
    // In a Properties Collection
    else if ( _nFlag == 0x03 )
    {
        if ( DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                    DWFXML::kzElement_Property) == 0)
        {
            // Construct the property and add it to the collection
            DWFProperty* property = DWFCORE_ALLOC_OBJECT( DWFProperty() );
            property->parseAttributeList( ppAttributeList );

            // taking ownership of all the properties constructed
            this->addProperty( property, true );
        }
    }
}

void DWFEMapLayer::notifyEndElement( const char* zName ) throw()
{
    const char* zElement = DWFEMapUtility::offsetKnownPrefix( zName );

    if ( _nFlag == 0x01 )   // processing a UIGraphic
    {
        if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                                    DWFEMAPXML::kzElement_UIGraphic ) == 0)
        {
            _nFlag = 0x00;  // reset the flag
        }
        else
        {
            _poUIGraphic->notifyEndElement( zName );
        }
    }
    else if ( _nFlag == 0x02 )  // in the ScaleRanges collection
    {
        if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                                DWFEMAPXML::kzElement_ScaleRange ) == 0)
        {
            DWFScaleRange* scalerange = static_cast<DWFScaleRange*>( _poCallback );
            _oScaleRanges.push_back( scalerange );
            _poCallback = NULL;
        }
        else if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                                DWFEMAPXML::kzElement_ScaleRanges ) == 0)
        {
            _nFlag = 0x00;  // reset the flag
        }
        else if ( _poCallback != NULL )
        {
            _poCallback->notifyEndElement( zName );
        }
    }
    else if ( _nFlag == 0x03 )  // in the Properties collection
    {
        if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                                   DWFXML::kzElement_Properties ) == 0 )
        {
            _nFlag = 0x00;  // reset the flag
        }
    }
    else if ( _nFlag == 0x04 )
    {
        _nFlag = 0x00;  // reset the flag
    }
}

void DWFEMapLayer::notifyStartNamespace( const char* zPrefix,
                                const char* zURI ) throw()
{
    //NA
}

void DWFEMapLayer::notifyEndNamespace( const char* zPrefix ) throw()
{
    //NA
}


void DWFEMapLayer::notifyCharacterData( const char* zCData, int nLength )
                                        throw()
{
    if ( _nFlag == 0x01 && _poUIGraphic != NULL)
    {
        _poUIGraphic->notifyCharacterData(zCData, nLength);
    }
    else if ( _nFlag == 0x02 && _poCallback != NULL)
    {
        _poCallback->notifyCharacterData(zCData, nLength);
    }
    else if ( _nFlag == 0x04) // Name element
    {
        // expat will call this callback multiple times if the
        // original string contains XML-encoded bytes
        this->_zName.append( zCData, nLength );
    }
}

// DWFXMLSerializable support
void DWFEMapLayer::serializeXML( DWFXMLSerializer& rSerializer,
                                unsigned int nFlags ) throw( DWFException )
{
    // Element Layer
    rSerializer.startElement( DWFEMAPXML::kzElement_Layer,
                              DWFEMAPXML::kzNamespace_EMap );
    {
        // Attributes
        {
            // Attribute ObjectID : Required
            rSerializer.addAttribute( DWFEMAPXML::kzAttribute_ObjectID,
                                      _zObjectID,
                                      DWFEMAPXML::kzNamespace_EMap );

            // Attribute groupObjectId : Optional
            if ( _zGroupObjectID != "" )
            {
                rSerializer.addAttribute( DWFEMAPXML::kzAttribute_GroupObjectID,
                                          _zGroupObjectID,
                                          DWFEMAPXML::kzNamespace_EMap );
            }

            // Attribute visible : Optional
            if ( !_bVisible )   // if it's not the default write it out.
            {
                rSerializer.addAttribute( DWFEMAPXML::kzAttribute_Visible,
                                         "False",
                                         DWFEMAPXML::kzNamespace_EMap );
            }

            // Attribute selectable : Optional
            if ( !_bSelectable )    // if it's not the default value
            {
                rSerializer.addAttribute( DWFEMAPXML::kzAttribute_Selectable,
                                         "False",
                                         DWFEMAPXML::kzNamespace_EMap );
            }

            // Attribute editable : Optional
            if ( !_bEditable )    // if it's not the default value
            {
                rSerializer.addAttribute( DWFEMAPXML::kzAttribute_Editable,
                                         "False",
                                         DWFEMAPXML::kzNamespace_EMap );
            }
        }

        // Name Element
        rSerializer.startElement( DWFEMAPXML::kzElement_Name,
                                  DWFEMAPXML::kzNamespace_EMap );
        {
            rSerializer.addCData( _zName );
        }
        rSerializer.endElement();

        // Element ScaleRanges
        if ( _oScaleRanges.size() > 0 )
        {
            rSerializer.startElement( DWFEMAPXML::kzElement_ScaleRanges,
                                      DWFEMAPXML::kzNamespace_EMap );
            {
                for( ScaleRangeCollection::iterator itr = _oScaleRanges.begin();
                            itr != _oScaleRanges.end(); itr++ )
                {
                    ( *itr )->serializeXML( rSerializer, nFlags );
                }
            }

            rSerializer.endElement();
        }

        // Element UIGraphic
        if ( _poUIGraphic != NULL )
        {
             _poUIGraphic->serializeXML( rSerializer, nFlags );
        }

        // Element Properties
        DWFXMLSerializable& xmlSrlzable = DWFPropertyContainer::getSerializable();
        xmlSrlzable.serializeXML( rSerializer, nFlags );

    }
    rSerializer.endElement(); // End element Layer
}

// End of file.
