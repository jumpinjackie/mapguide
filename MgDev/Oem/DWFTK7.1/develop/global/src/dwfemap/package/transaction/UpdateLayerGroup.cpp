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

#include "./UpdateLayerGroup.h"
#include "../EMapConstants.h"
#include "../EMapUtility.h"
using namespace DWFToolkit::DWFEMapExtension;


// Construction , Destruction
DWFEMapUpdateLayerGroupCommand::DWFEMapUpdateLayerGroupCommand()
throw() : DWFEMapCommand()
        , _zObjectID("")
        , _zGroupObjectID("")
        , _bVisible(true)
{
}

DWFEMapUpdateLayerGroupCommand::DWFEMapUpdateLayerGroupCommand(const DWFString& zObjectID,
                                        const DWFString& zGroupObjectID,
                                        bool bVisible)
throw() : DWFEMapCommand()
        , _zObjectID(zObjectID)
        , _zGroupObjectID(zGroupObjectID)
        , _bVisible(bVisible)
{
}


DWFEMapUpdateLayerGroupCommand::~DWFEMapUpdateLayerGroupCommand(void)
throw()
{
}

// acquire member data from attribute list (from XML parser)
_DWFTK_API void DWFEMapUpdateLayerGroupCommand::parseAttributeList( const char** ppAttributeList )
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
    }
}

const DWFString& DWFEMapUpdateLayerGroupCommand::objectID() const
throw()
{
    return _zObjectID;
}

const DWFString& DWFEMapUpdateLayerGroupCommand::groupObjectID() const
throw()
{
    return _zGroupObjectID;
}


bool DWFEMapUpdateLayerGroupCommand::visible() const
throw()
{
    return _bVisible;
}

// XMLCallback support
void DWFEMapUpdateLayerGroupCommand::notifyStartElement( const char*  zName,
                                const char** ppAttributeList )
                                throw()
{
    // NA
}

void DWFEMapUpdateLayerGroupCommand::notifyEndElement( const char* zName )
throw()
{
   // NA
}

void DWFEMapUpdateLayerGroupCommand::notifyStartNamespace( const char* zPrefix,
                                                      const char* zURI )
                                                      throw()
{
    // NA
}

void DWFEMapUpdateLayerGroupCommand::notifyEndNamespace( const char* zPrefix )
throw()
{
    // NA
}


void DWFEMapUpdateLayerGroupCommand::notifyCharacterData( const char* zCData,
                                                    int nLength )
                                                    throw()
{
    // NA
}

// DWFXMLSerializable support
void DWFEMapUpdateLayerGroupCommand::serializeXML(
                    DWFXMLSerializer& rSerializer,
                    unsigned int nFlags )
                    throw( DWFException )
{
    // Element UpdateLayerGroup
    rSerializer.startElement(DWFEMAPXML::kzElement_UpdateLayerGroup,
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
            if( ! _bVisible )
            {
                rSerializer.addAttribute( DWFEMAPXML::kzAttribute_Visible,
                                       "False" , DWFEMAPXML::kzNamespace_EMap );
            }
        }
    }
    rSerializer.endElement(); // End UpdateLayerGroup element
}

// End of file.
