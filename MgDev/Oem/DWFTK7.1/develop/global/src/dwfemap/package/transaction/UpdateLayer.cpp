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

#include "./UpdateLayer.h"
#include "../EMapConstants.h"
#include "../EMapUtility.h"
using namespace DWFToolkit::DWFEMapExtension;


// Construction , Destruction
DWFEMapUpdateLayerCommand::DWFEMapUpdateLayerCommand()
throw() : DWFEMapCommand()
        , _zObjectID("")
        , _zGroupObjectID("")
        , _bVisible(true)
        , _bSelectable(true)
{
}

DWFEMapUpdateLayerCommand::DWFEMapUpdateLayerCommand(const DWFString& zObjectID,
                                        const DWFString& zGroupObjectID,
                                        bool bVisible,
                                        bool bSelectable)
throw() : DWFEMapCommand()
        , _zObjectID(zObjectID)
        , _zGroupObjectID(zGroupObjectID)
        , _bVisible(bVisible)
        , _bSelectable(bSelectable)
{
}


DWFEMapUpdateLayerCommand::~DWFEMapUpdateLayerCommand(void)
throw()
{
}


// acquire member data from attribute list (from XML parser)
_DWFTK_API void DWFEMapUpdateLayerCommand::parseAttributeList( const char** ppAttributeList )
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
                               DWFEMAPXML::kzAttribute_GroupObjectID ) == 0)
        {
            nFlag |= 0x02;
            this->_zGroupObjectID = ppAttributeList[iAttrib + 1];
        }
        else if(!(nFlag & 0x04) && DWFCORE_COMPARE_ASCII_STRINGS(pAttrib,
                               DWFEMAPXML::kzAttribute_Visible ) == 0)
        {
            nFlag |= 0x04;
            this->_bVisible =
                DWFEMapUtility::convertStringToBool(ppAttributeList[iAttrib + 1]);
        }
        else if(!(nFlag & 0x08) && DWFCORE_COMPARE_ASCII_STRINGS(pAttrib,
                               DWFEMAPXML::kzAttribute_Selectable ) == 0)
        {
            nFlag |= 0x08;
            this->_bSelectable =
                DWFEMapUtility::convertStringToBool(ppAttributeList[iAttrib + 1]);
        }
    }
}

const DWFString& DWFEMapUpdateLayerCommand::objectID() const
throw()
{
    return _zObjectID;
}

const DWFString& DWFEMapUpdateLayerCommand::groupObjectID() const
throw()
{
    return _zGroupObjectID;
}


bool DWFEMapUpdateLayerCommand::visible() const
throw()
{
    return _bVisible;
}


bool DWFEMapUpdateLayerCommand::selectable() const
throw()
{
    return _bSelectable;
}


// XMLCallback support
void DWFEMapUpdateLayerCommand::notifyStartElement( const char*  zName,
                                const char** ppAttributeList )
                                throw()
{
    // NA
}

void DWFEMapUpdateLayerCommand::notifyEndElement( const char* zName )
throw()
{
   // NA
}

void DWFEMapUpdateLayerCommand::notifyStartNamespace( const char* zPrefix,
                                                      const char* zURI )
                                                      throw()
{
    // NA
}

void DWFEMapUpdateLayerCommand::notifyEndNamespace( const char* zPrefix )
throw()
{
    // NA
}


void DWFEMapUpdateLayerCommand::notifyCharacterData( const char* zCData,
                                                    int nLength )
                                                    throw()
{
    // NA
}

// DWFXMLSerializable support
void DWFEMapUpdateLayerCommand::serializeXML(
                    DWFXMLSerializer& rSerializer,
                    unsigned int nFlags )
                    throw( DWFException )
{
    // Element UpdateLayer
    rSerializer.startElement(DWFEMAPXML::kzElement_UpdateLayer,
                                                DWFEMAPXML::kzNamespace_EMap);
    {
        // Attributes
        {
            // Attribute ObjectID : Required
            rSerializer.addAttribute( DWFEMAPXML::kzAttribute_ObjectID,
                                     _zObjectID, DWFEMAPXML::kzNamespace_EMap );

            // Attribute GroupObjectID : Optional
            if( _zGroupObjectID != "")
            {
                rSerializer.addAttribute( DWFEMAPXML::kzAttribute_GroupObjectID,
                                          _zGroupObjectID, DWFEMAPXML::kzNamespace_EMap );
            }

            // Attribute Visible : Optional
            if( !_bVisible)
            {
                rSerializer.addAttribute( DWFEMAPXML::kzAttribute_Visible,
                                "False" , DWFEMAPXML::kzNamespace_EMap );
            }

            // Attribute Selectable : Optional
            if( !_bSelectable)
            {
                rSerializer.addAttribute(DWFEMAPXML::kzAttribute_Selectable,
                               "False" , DWFEMAPXML::kzNamespace_EMap );
            }
        }
    }
    rSerializer.endElement(); // End UpdateLayer element
}

// End of file.
