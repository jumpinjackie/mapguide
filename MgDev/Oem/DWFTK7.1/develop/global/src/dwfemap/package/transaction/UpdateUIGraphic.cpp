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

#include "./UpdateUIGraphic.h"
#include "../EMapConstants.h"
#include "../EMapUtility.h"
using namespace DWFToolkit::DWFEMapExtension;


// Construction , Destruction
DWFEMapUpdateUIGraphicCommand::DWFEMapUpdateUIGraphicCommand()
throw() : DWFEMapCommand()
        , _zObjectID("")
        , _zLabel("")
        , _bShowInLegend(true)
        , _bProcessLabel(false)
{
}

DWFEMapUpdateUIGraphicCommand::DWFEMapUpdateUIGraphicCommand(
                                    const DWFString& zObjectID,
                                    const DWFString& zLabel,
                                    bool bShowInLegend)
throw() : DWFEMapCommand()
        , _zObjectID(zObjectID)
        , _zLabel(zLabel)
        , _bShowInLegend(bShowInLegend)
        , _bProcessLabel(false)
{
}

DWFEMapUpdateUIGraphicCommand::~DWFEMapUpdateUIGraphicCommand(void)
throw()
{
}

// acquire member data from attribute list (from XML parser)
_DWFTK_API void DWFEMapUpdateUIGraphicCommand::parseAttributeList(const char** ppAttributeList)
    throw( DWFException )
{
    unsigned char nFlag = 0;
    size_t iAttrib = 0;
    const char* pAttrib = NULL;

    // loop over the collection of attributes
    for(;ppAttributeList[iAttrib]!= NULL; iAttrib +=2)
    {
        pAttrib = DWFEMapUtility::offsetKnownPrefix(ppAttributeList[iAttrib]);

        if(!(nFlag & 0x01) && DWFCORE_COMPARE_ASCII_STRINGS(pAttrib,
                                    DWFEMAPXML::kzAttribute_ObjectID ) == 0)
        {
            nFlag |= 0x01;
            this->_zObjectID = ppAttributeList[iAttrib + 1];
        }
        else if(!(nFlag & 0x02) && DWFCORE_COMPARE_ASCII_STRINGS(pAttrib,
                               DWFEMAPXML::kzAttribute_ShowInLegend ) == 0)
        {
            nFlag |= 0x02;
            this->_bShowInLegend =
                DWFEMapUtility::convertStringToBool(ppAttributeList[iAttrib + 1]);
        }
    }
}

// operations
const DWFString& DWFEMapUpdateUIGraphicCommand::objectID() const
throw()
{
    return _zObjectID;
}

const DWFString& DWFEMapUpdateUIGraphicCommand::label() const
throw()
{
    return _zLabel;
}



bool DWFEMapUpdateUIGraphicCommand::showInLegend() const
throw()
{
    return _bShowInLegend;
}

// XMLCallback support
void DWFEMapUpdateUIGraphicCommand::notifyStartElement(const char* zName,
                                               const char** ppAttributeList)
                                               throw()
{
    const char* zElement = DWFEMapUtility::offsetKnownPrefix(zName);

    if(DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                    DWFEMAPXML::kzElement_Label ) == 0)
    {
        // clear any existing label
        _zLabel.destroy();
        _bProcessLabel = true;
    }
}

void DWFEMapUpdateUIGraphicCommand::notifyEndElement( const char* zName )
                                                      throw()
{
    // Note this check does not validate the element rather it simply assumes
    // the XML is properly formed
    if( _bProcessLabel)
    {
        _bProcessLabel = false;
    }
}

void DWFEMapUpdateUIGraphicCommand::notifyStartNamespace( const char* zPrefix,
                                                          const char* zURI )
                                                          throw()
{
    // NA
}

void DWFEMapUpdateUIGraphicCommand::notifyEndNamespace( const char* zPrefix )
                                                        throw()
{
    // NA
}


void DWFEMapUpdateUIGraphicCommand::notifyCharacterData( const char* zCData,
                                                         int nLength )
                                                         throw()
{
    if( _bProcessLabel)
    {
        // expat will call this callback multiple times if the
        // original string contains XML-encoded bytes
        this->_zLabel.append( zCData, nLength );
    }
}

// DWFXMLSerializable support
void DWFEMapUpdateUIGraphicCommand::serializeXML(
                            DWFXMLSerializer& rSerializer,
                            unsigned int nFlags )
                            throw( DWFException )
{
    // Element Layer
    rSerializer.startElement(DWFEMAPXML::kzElement_UpdateUIGraphic,
                                                DWFEMAPXML::kzNamespace_EMap);
    {
        // Attributes
        {
            // Attribute ObjectID : Required
            rSerializer.addAttribute(DWFEMAPXML::kzAttribute_ObjectID, _zObjectID,
                                                        DWFEMAPXML::kzNamespace_EMap );

            // Attribute ShowInLegend : Optional
            if( ! _bShowInLegend )
            {
                rSerializer.addAttribute( DWFEMAPXML::kzAttribute_ShowInLegend,
                                        "False" , DWFEMAPXML::kzNamespace_EMap );
            }
        }
        // Element Label
        if( _zLabel != "")
        {
            rSerializer.startElement(DWFEMAPXML::kzElement_Label,
                                                DWFEMAPXML::kzNamespace_EMap);
            {
                rSerializer.addCData( _zLabel);
            }
            rSerializer.endElement();
        }
    }
    rSerializer.endElement(); // End UpdateUIGraphic element
}

// End of file.
