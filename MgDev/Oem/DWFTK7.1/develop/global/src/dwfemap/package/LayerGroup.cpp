//
//  Copyright (C) 2003-2008 by Autodesk, Inc.
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

#include "./LayerGroup.h"
#include "./EMapUtility.h"
#include "./EMapConstants.h"
#include "./UIGraphic.h"
using namespace DWFToolkit::DWFEMapExtension;


DWFEMapLayerGroup::DWFEMapLayerGroup( DWFPackageReader* pPackageReader )
                                    throw() : DWFXMLBuildable()
                                            , _zName("")
                                            , _zObjectID("")
                                            , _zGroupObjectID("")
                                            , _bVisible(true)
                                            , _poUIGraphic(NULL)
                                            , _poPackageReader(pPackageReader)
                                            , _nFlag(0x00)
{
}

// basic constructor
DWFEMapLayerGroup::DWFEMapLayerGroup( const DWFString& zName,
                                    const DWFString& zObjectID,
                                    const DWFString& zGroupObjectID,
                                    bool bVisible,
                                    DWFUIGraphic* pUIGraphic)
                    throw() : DWFXMLBuildable()
                            , _zName(zName)
                            , _zObjectID(zObjectID)
                            , _zGroupObjectID(zGroupObjectID)
                            , _bVisible(bVisible)
                            , _poUIGraphic(pUIGraphic)
                            , _poPackageReader(NULL)
                            , _nFlag(0x00)
{
}

// destructor
DWFEMapLayerGroup::~DWFEMapLayerGroup() throw()
{
    DWFCORE_FREE_OBJECT( _poUIGraphic );
}


_DWFTK_API void DWFEMapLayerGroup::parseAttributeList( const char** ppAttributeList )
throw( DWFException )
{
    unsigned char nFlag = 0x00;
    size_t iAttrib = 0;
    const char* pAttrib = NULL;

    // loop over the collection of attributes
    for(;ppAttributeList[iAttrib]!= NULL; iAttrib += 2)
    {
        pAttrib = DWFEMapUtility::offsetKnownPrefix(ppAttributeList[iAttrib]);
        // Attribute objectId
        if (!(nFlag & 0x01) && DWFCORE_COMPARE_ASCII_STRINGS(pAttrib,
                                    DWFEMAPXML::kzAttribute_ObjectID ) == 0)
        {
            nFlag |= 0x01;
            this->_zObjectID = ppAttributeList[iAttrib + 1];
        }
        // Attribute groupObjectId
        else if (!(nFlag & 0x02) && DWFCORE_COMPARE_ASCII_STRINGS(pAttrib,
                                DWFEMAPXML::kzAttribute_GroupObjectID ) == 0)
        {
            nFlag |= 0x02;
            this->_zGroupObjectID = ppAttributeList[iAttrib + 1];
        }
        // Attribute visible
        else if (!(nFlag & 0x04) && DWFCORE_COMPARE_ASCII_STRINGS(pAttrib,
                                    DWFEMAPXML::kzAttribute_Visible ) == 0)
        {
            nFlag |= 0x04;
            this->_bVisible = DWFEMapUtility::convertStringToBool(
                                    ppAttributeList[iAttrib + 1]);
        }
    }
}

// property accessors
const DWFString& DWFEMapLayerGroup::name() const throw()
{
    return _zName;
}

const DWFString& DWFEMapLayerGroup::objectID() const throw()
{
    return _zObjectID;
}

const DWFString& DWFEMapLayerGroup::groupObjectID() const throw()
{
    return _zGroupObjectID;
}

bool DWFEMapLayerGroup::visible() const throw()
{
    return _bVisible;
}

// returns the UI graphic
const DWFUIGraphic* const DWFEMapLayerGroup::uiGraphic() const throw()
{
    return _poUIGraphic;
}

// XMLCallback support
void DWFEMapLayerGroup::notifyStartElement( const char*  zName,
                            const char** ppAttributeList ) throw()
{
    if ( _poPackageReader == NULL)
    {
        return;
    }

    const char* zElement = DWFEMapUtility::offsetKnownPrefix(zName);

    if ( _nFlag == 0x00 )
    {
        if (DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                DWFEMAPXML::kzElement_Name ) == 0)
        {
            // clear any existing name
            _zName.destroy();
            _nFlag = 0x01;
        }
        else if (DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                DWFEMAPXML::kzElement_UIGraphic ) == 0)
        {
            _nFlag = 0x02;
            _poUIGraphic = DWFCORE_ALLOC_OBJECT(DWFUIGraphic(_poPackageReader));
            _poUIGraphic->parseAttributeList(ppAttributeList);

        }
        else if ( DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                DWFXML::kzElement_Properties ) == 0)
        {
            _nFlag = 0x03;
        }
    }
    // pass through for the UIGraphic element
    else if ( _nFlag == 0x02) 
    {
        _poUIGraphic->notifyStartElement(zName,ppAttributeList);
    }
    // In the Properties collection
    else if ( _nFlag == 0x03 )
    {
        if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                                DWFXML::kzElement_Property ) == 0)
        {
            DWFProperty* pProperty = DWFCORE_ALLOC_OBJECT( DWFProperty() );
            pProperty->parseAttributeList( ppAttributeList );

            // taking ownership of all the properties constructed.
            this->addProperty( pProperty, true );
        }
    }
}

void DWFEMapLayerGroup::notifyEndElement( const char* zName ) throw()
{
    const char* zElement = DWFEMapUtility::offsetKnownPrefix( zName );

    if ( _nFlag == 0x01 )   // should be end of Name element
    {
        _nFlag = 0x00;  // reset the flag
    }
    else if ( _nFlag == 0x02 )
    {
        if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                                DWFEMAPXML::kzElement_UIGraphic ) == 0)
        {
            _nFlag = 0x00;  // reset the flag
        }
        else // pass through to the UIGraphic
        {
            _poUIGraphic->notifyEndElement( zName );
        }
    }
    else if ( _nFlag == 0x03 )
    {
        if ( DWFCORE_COMPARE_ASCII_STRINGS( zElement,
                                DWFXML::kzElement_Properties ) == 0)
        {
            _nFlag = 0x00;  // reset the flag
        }
    }
}

void DWFEMapLayerGroup::notifyStartNamespace( const char* zPrefix,
                            const char* zURI ) throw()
{
    //NA
}

void DWFEMapLayerGroup::notifyEndNamespace( const char* zPrefix ) throw()
{
    // NA
}

void DWFEMapLayerGroup::notifyCharacterData( const char* zCData, int nLength )
                                    throw()
{
    if ( _nFlag == 0x01 )
    {
        // expat will call this callback multiple times if the
        // original string contains XML-encoded bytes
        this->_zName.append( zCData, nLength );
    }
    else if ( _nFlag == 0x02 )
    {
        this->_poUIGraphic->notifyCharacterData( zCData, nLength );
    }
}

// DWFXMLSerializable support
void DWFEMapLayerGroup::serializeXML( DWFXMLSerializer& rSerializer,
                    unsigned int nFlags ) throw( DWFException )
{
    // Element LayerGroup
    rSerializer.startElement( DWFEMAPXML::kzElement_LayerGroup,
                              DWFEMAPXML::kzNamespace_EMap );
    {
        // Attributes
        {
            // Attribute objectId : Required
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
            if ( !_bVisible )   // if its not the default write it out.
            {
                rSerializer.addAttribute( DWFEMAPXML::kzAttribute_Visible,
                                          "False",
                                          DWFEMAPXML::kzNamespace_EMap );
            }
        }

        // Element Name
        rSerializer.startElement( DWFEMAPXML::kzElement_Name,
                                  DWFEMAPXML::kzNamespace_EMap );
        {
            rSerializer.addCData( _zName );
        }
        rSerializer.endElement();

        // Element UIGraphic
        if ( _poUIGraphic != NULL )
        {
             _poUIGraphic->serializeXML( rSerializer, nFlags );
        }

        // Element Properties
        DWFXMLSerializable& xmlSrlzable = DWFPropertyContainer::getSerializable();
        xmlSrlzable.serializeXML( rSerializer, nFlags );
    }

    rSerializer.endElement(); // End LayerGroups
}

// End of file.
