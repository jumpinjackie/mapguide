//
//  Copyright (C) 2003-2011 by Autodesk, Inc.
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

#include "./UIGraphic.h"
#include "./EMapConstants.h"
#include "./EMapUtility.h"
using namespace DWFToolkit::DWFEMapExtension;


DWFUIGraphic::DWFUIGraphic( DWFPackageReader* pPackageReader)
throw() : DWFXMLBuildable()
        , _zObjectID("")
        , _zLabel("")
        , _bShowInLegend(true)
        , _bCollapsed(false)
        , _poCachedPackageReader(pPackageReader)
        , _bIsLabel(false)
{
}

// basic constructor
DWFUIGraphic::DWFUIGraphic( const DWFString& zObjectID,
                const DWFString& zLabel,
                bool bShowInLegend,
                bool bCollapsed )
throw() : DWFXMLBuildable()
        , _zObjectID(zObjectID)
        , _zLabel(zLabel)
        , _bShowInLegend(bShowInLegend)
        , _bCollapsed(bCollapsed)
        , _poCachedPackageReader(NULL)
        , _bIsLabel(false)
{
}

// destructor
DWFUIGraphic::~DWFUIGraphic() throw()
{
}


_DWFTK_API void DWFUIGraphic::parseAttributeList( const char** ppAttributeList )
    throw( DWFException )
{
    unsigned char nFlag = 0x00;
    size_t iAttrib = 0;
    const char* pAttrib = NULL;

    // loop over the collection of Attributes
    for(;ppAttributeList[iAttrib]!= NULL; iAttrib += 2)
    {
        pAttrib = DWFEMapUtility::offsetKnownPrefix(ppAttributeList[iAttrib]);

        if(!(nFlag & 0x01) && DWFCORE_COMPARE_ASCII_STRINGS(pAttrib,
                                    DWFEMAPXML::kzAttribute_ObjectID ) == 0)
        {
            nFlag |= 0x01;
            this->_zObjectID = ppAttributeList[iAttrib + 1];
        }
        else if (! (nFlag & 0x02) && DWFCORE_COMPARE_ASCII_STRINGS(pAttrib,
                                    DWFEMAPXML::kzAttribute_ShowInLegend ) == 0)
        {
            nFlag |= 0x02;
            this->_bShowInLegend = DWFEMapUtility::convertStringToBool(
                                            ppAttributeList[iAttrib + 1]);
        }
        else if (! (nFlag & 0x04) && DWFCORE_COMPARE_ASCII_STRINGS(pAttrib,
                                    DWFEMAPXML::kzAttribute_Collapsed ) == 0)
        {
            nFlag |= 0x04;
            this->_bCollapsed = DWFEMapUtility::convertStringToBool(
                                            ppAttributeList[iAttrib + 1]);
        }
    }
}

// property accessors
const DWFString& DWFUIGraphic::objectID() const throw()
{
    return _zObjectID;
}

const DWFString& DWFUIGraphic::label() const throw()
{
    return _zLabel;
}

bool DWFUIGraphic::showInLegend() const throw()
{
    return _bShowInLegend;
}

bool DWFUIGraphic::collapsed() const throw()
{
    return _bCollapsed;
}

// XMLCallback support
void DWFUIGraphic::notifyStartElement( const char*  zName,
                            const char** ppAttributeList ) throw()
{
    // Parsing is invalid if PackageReader is NULL
    if (_poCachedPackageReader == NULL)
    {
        return;
    }

    const char* zElement = DWFEMapUtility::offsetKnownPrefix(zName);

    _bIsLabel = false;
    if(DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                DWFEMAPXML::kzElement_Label ) == 0)
    {
        // clear any existing label
        _zLabel.destroy();
        _bIsLabel = true;
    }
}

void DWFUIGraphic::notifyEndElement( const char* zName ) throw()
{
    const char* zElement = DWFEMapUtility::offsetKnownPrefix(zName);

    if(DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                DWFEMAPXML::kzElement_Label ) == 0)
    {
        _bIsLabel = false;
    }
}


void DWFUIGraphic::notifyStartNamespace( const char* zPrefix,
                            const char* zURI ) throw()
{
}

void DWFUIGraphic::notifyEndNamespace( const char* zPrefix ) throw()
{
}

void DWFUIGraphic::notifyCharacterData( const char* zCData, int nLength )
                                    throw()
{
    if(_bIsLabel)
    {
        // expat will call this callback multiple times if the
        // original string contains XML-encoded bytes
        this->_zLabel.append( zCData, nLength );
    }
}

// DWFXMLSerializable support
void DWFUIGraphic::serializeXML( DWFXMLSerializer& rSerializer,
                    unsigned int nFlags ) throw( DWFException )
{
    // Element UIGraphic
    rSerializer.startElement(DWFEMAPXML::kzElement_UIGraphic, DWFEMAPXML::kzNamespace_EMap);
    {
        // Attributes
        {
            // Attribute ObjectID : Required
            rSerializer.addAttribute( DWFEMAPXML::kzAttribute_ObjectID,
                                      this->_zObjectID, DWFEMAPXML::kzNamespace_EMap );

            // Attribute showInLegend : Optional
            if( !_bShowInLegend ) // default is true;
            {
                rSerializer.addAttribute( DWFEMAPXML::kzAttribute_ShowInLegend,
                                       "False", DWFEMAPXML::kzNamespace_EMap );
            }

            // Attribute collapsed : Optional
            if( _bCollapsed ) // default is false;
            {
                rSerializer.addAttribute( DWFEMAPXML::kzAttribute_Collapsed,
                                       "True", DWFEMAPXML::kzNamespace_EMap );
            }
        }

        // Element Label
        rSerializer.startElement(DWFEMAPXML::kzElement_Label, DWFEMAPXML::kzNamespace_EMap);
        {
            rSerializer.addCData(this->_zLabel);
        }
        rSerializer.endElement();
    }
    rSerializer.endElement(); // end Element UIGraphic
}

// End of file.
