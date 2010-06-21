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

#include "./ScaleRange.h"
#include "./EMapConstants.h"
#include "./EMapUtility.h"
#include "./UIGraphic.h"
using namespace DWFToolkit::DWFEMapExtension;


DWFScaleRange::DWFScaleRange( DWFPackageReader* pPackageReader)
throw() : DWFXMLBuildable()
        , _zObjectID("")
        , _nMinScale(1.0)
        , _nMaxScale(1.0)
        , _poCachedPackageReader(pPackageReader)
        , _poActiveChild(NULL)
        , _bUIGraphics(false)
{
}

// basic constructor
DWFScaleRange::DWFScaleRange( const DWFString& zObjectID,
                        double nMinScale, double nMaxScale )
throw() : DWFXMLBuildable()
        , _zObjectID(zObjectID)
        , _nMinScale(nMinScale)
        , _nMaxScale(nMaxScale)
        , _poCachedPackageReader(NULL)
        , _poActiveChild(NULL)
        , _bUIGraphics(false)
{
}

// destructor
DWFScaleRange::~DWFScaleRange() throw()
{
    //delete the collection of UIGraphics
    for( UIGraphicCollection::iterator itr = _oUIGraphics.begin();
                    itr != _oUIGraphics.end(); itr++)
    {
        DWFCORE_FREE_OBJECT( *itr );
    }
}

_DWFTK_API void DWFScaleRange::parseAttributeList( const char** ppAttributeList )
throw( DWFException )
{
    unsigned char nFlag = 0x00;
    size_t iAttrib = 0;
    const char* pAttrib = NULL;

    // loop over the collection of Attributes
    for(;ppAttributeList[iAttrib]!= NULL; iAttrib +=2)
    {
        pAttrib = DWFEMapUtility::offsetKnownPrefix(ppAttributeList[iAttrib]);

        if(!(nFlag & 0x01) && DWFCORE_COMPARE_ASCII_STRINGS(pAttrib,
                                    DWFEMAPXML::kzAttribute_ObjectID ) == 0)
        {
            nFlag |= 0x01;
            this->_zObjectID = ppAttributeList[iAttrib + 1];
        }
        else if (! (nFlag & 0x02) && DWFCORE_COMPARE_ASCII_STRINGS(pAttrib,
                                    DWFEMAPXML::kzAttribute_MinScale ) == 0)
        {
            nFlag |= 0x02;
            this->_nMinScale = DWFString::StringToDouble(
                                            ppAttributeList[iAttrib + 1]);
        }
        else if (! (nFlag & 0x04) && DWFCORE_COMPARE_ASCII_STRINGS(pAttrib,
                                    DWFEMAPXML::kzAttribute_MaxScale ) == 0)
        {
            nFlag |= 0x04;
            this->_nMaxScale = DWFString::StringToDouble(
                                            ppAttributeList[iAttrib + 1]);
        }
    }
}

// property accessors
const DWFString& DWFScaleRange::objectID() const throw()
{
    return _zObjectID;
}

double DWFScaleRange::minScale() const throw()
{
    return _nMinScale;
}

double DWFScaleRange::maxScale() const throw()
{
    return _nMaxScale;
}

// Returns all UI graphics in the scale range
const DWFUIGraphic* DWFScaleRange::getUIGraphic( size_t index ) const
throw()
{
    DWFUIGraphic* pRetValue = NULL;

    if ( index < _oUIGraphics.size() )
    {
        pRetValue = _oUIGraphics.at( index );
    }

    return pRetValue;
}

        // Returns the index count
size_t DWFScaleRange::uiGraphicCount() const
throw()
{
    return _oUIGraphics.size();
}


// adds a UI graphic to the scale range
void DWFScaleRange::addUIGraphic( DWFUIGraphic* pUIGraphic ) throw( DWFException )
{
    if( pUIGraphic != NULL)
    {
        _oUIGraphics.push_back(pUIGraphic);
    }
}

// XMLCallback support
void DWFScaleRange::notifyStartElement( const char*  zName,
                                        const char** ppAttributeList ) throw()
{
    // Parsing is invalid if PackageReader is NULL
    if (_poCachedPackageReader == NULL)
    {
        return;
    }

    const char* zElement = DWFEMapUtility::offsetKnownPrefix(zName);

    if ( !_bUIGraphics && DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                            DWFEMAPXML::kzElement_UIGraphics ) == 0)
    {
        _bUIGraphics = true; //the collection is already initialized
    }
    // In the UIGraphics Collection
    else if (_bUIGraphics)
    {
        if( _poActiveChild == NULL && DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                DWFEMAPXML::kzElement_UIGraphic ) == 0)
        {
            // Process Current UIGraphics here
            DWFUIGraphic* pNewUIGraphic =
                    DWFCORE_ALLOC_OBJECT ( DWFUIGraphic( _poCachedPackageReader ) );
            pNewUIGraphic->parseAttributeList(ppAttributeList);
            _poActiveChild = pNewUIGraphic;
        }
        else if (_poActiveChild != NULL)
        {
            _poActiveChild->notifyStartElement(zName,ppAttributeList);
        }
    }
}

void DWFScaleRange::notifyEndElement( const char* zName ) throw()
{
    const char* zElement = DWFEMapUtility::offsetKnownPrefix(zName);

    if (_bUIGraphics)
    {
        if( DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                   DWFEMAPXML::kzElement_UIGraphic) == 0)
        {
            // add the finished UIGraphic to the collection
            DWFUIGraphic* pUIGraphic = static_cast<DWFUIGraphic*>(_poActiveChild);
            this->_oUIGraphics.push_back(pUIGraphic);

            // clear the active UIGraphic
            _poActiveChild = NULL;
        }
        else if (DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                   DWFEMAPXML::kzElement_UIGraphics) == 0)
        {
            _bUIGraphics = false; //out of active collection
        }
        else if( _poActiveChild != NULL )
        {
            _poActiveChild->notifyEndElement(zName);
        }
    }
}


void DWFScaleRange::notifyStartNamespace( const char* zPrefix,
                                const char* zURI ) throw()
{
    if(_poActiveChild != NULL)
    {
        _poActiveChild->notifyStartNamespace(zPrefix,zURI);
    }
}


void DWFScaleRange::notifyEndNamespace( const char* zPrefix ) throw()
{
    if( _poActiveChild != NULL)
    {
        _poActiveChild->notifyEndNamespace(zPrefix);
    }
}


void DWFScaleRange::notifyCharacterData( const char* zCData, int nLength )
                                        throw()
{
   if( _bUIGraphics && _poActiveChild != NULL)
   {
        _poActiveChild->notifyCharacterData(zCData, nLength);
   }
}

// DWFXMLSerializable support
void DWFScaleRange::serializeXML( DWFXMLSerializer& rSerializer,
                        unsigned int nFlags ) throw( DWFException )
{
    wchar_t zTempBuffer[_DWFEMAPTK_TEMP_BUFFER_BYTES];

    // Element UIGraphic
    rSerializer.startElement(DWFEMAPXML::kzElement_ScaleRange, DWFEMAPXML::kzNamespace_EMap);
    {
        // Attributes
        {
            // Attribute objectId : Required
            rSerializer.addAttribute( DWFEMAPXML::kzAttribute_ObjectID,
                                      this->_zObjectID, DWFEMAPXML::kzNamespace_EMap );

            // Attribute minScale
            _DWFCORE_SWPRINTF( zTempBuffer, _DWFEMAPTK_TEMP_BUFFER_BYTES, L"%.17g", _nMinScale );
             rSerializer.addAttribute( DWFEMAPXML::kzAttribute_MinScale,
                                       DWFString::RepairDecimalSeparators(zTempBuffer),
                                       DWFEMAPXML::kzNamespace_EMap );

             // Attribute maxScale
            _DWFCORE_SWPRINTF( zTempBuffer, _DWFEMAPTK_TEMP_BUFFER_BYTES, L"%.17g", _nMaxScale );
             rSerializer.addAttribute( DWFEMAPXML::kzAttribute_MaxScale,
                                       DWFString::RepairDecimalSeparators(zTempBuffer),
                                       DWFEMAPXML::kzNamespace_EMap );
        }

        // Element UIGraphics
        if( _oUIGraphics.size() > 0)
        {
            rSerializer.startElement(DWFEMAPXML::kzElement_UIGraphics, DWFEMAPXML::kzNamespace_EMap);
            {
                for(UIGraphicCollection::iterator itr = _oUIGraphics.begin();
                            itr != _oUIGraphics.end(); itr++)
                {
                    (*itr)->serializeXML( rSerializer, nFlags);
                }
            }
            rSerializer.endElement(); // End UIGraphics
        }
    }
    rSerializer.endElement(); // End ScaleRange
}

// End of file.
