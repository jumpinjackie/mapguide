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

#include "./RemoveLayerGroup.h"
#include "../EMapConstants.h"
#include "../EMapUtility.h"
using namespace DWFToolkit::DWFEMapExtension;


// Construction , Destruction
DWFEMapRemoveLayerGroupCommand::DWFEMapRemoveLayerGroupCommand()
throw() : DWFEMapCommand()
        , _zObjectID("")
{
}

DWFEMapRemoveLayerGroupCommand::DWFEMapRemoveLayerGroupCommand(const DWFString& zObjectID)
throw() : DWFEMapCommand()
        , _zObjectID(zObjectID)
{
}


DWFEMapRemoveLayerGroupCommand::~DWFEMapRemoveLayerGroupCommand(void)
throw()
{
}

// acquire member data from attribute list (from XML parser)
_DWFTK_API void DWFEMapRemoveLayerGroupCommand::parseAttributeList( const char** ppAttributeList )
    throw( DWFException )
{
    const char* pAttrib = NULL;

    // process attributes
    if(ppAttributeList[0]!= NULL)
    {
        pAttrib = DWFEMapUtility::offsetKnownPrefix(ppAttributeList[0]);

        if(DWFCORE_COMPARE_ASCII_STRINGS(pAttrib,
                                    DWFEMAPXML::kzAttribute_ObjectID ) == 0)
        {
            this->_zObjectID = ppAttributeList[1];
        }
    }
}


const DWFString& DWFEMapRemoveLayerGroupCommand::objectID()const
{
    return _zObjectID;
}


void DWFEMapRemoveLayerGroupCommand::notifyStartElement( const char*  zName,
                                            const char** ppAttributeList )
                                            throw()
{
}

void DWFEMapRemoveLayerGroupCommand::notifyEndElement( const char* zName ) throw()
{
}

void DWFEMapRemoveLayerGroupCommand::notifyStartNamespace( const char* zPrefix,
                                                      const char* zURI )
                                                      throw()
{
}

void DWFEMapRemoveLayerGroupCommand::notifyEndNamespace( const char* zPrefix ) throw()
{
}


void DWFEMapRemoveLayerGroupCommand::notifyCharacterData( const char* zCData,
                                                    int nLength )
                                                    throw()
{
}

// DWFXMLSerializable support
void DWFEMapRemoveLayerGroupCommand::serializeXML(
                            DWFXMLSerializer& rSerializer,
                            unsigned int nFlags )
                            throw( DWFException )
{
        // Element UpdateLayerGroup
    rSerializer.startElement(DWFEMAPXML::kzElement_RemoveLayerGroup,
                                                DWFEMAPXML::kzNamespace_EMap);
    {
        // Attributes
        {
            // Attribute ObjectID : Required
            rSerializer.addAttribute(DWFEMAPXML::kzAttribute_ObjectID,
                                      _zObjectID, DWFEMAPXML::kzNamespace_EMap);
        }
    }
    rSerializer.endElement(); // End RemoveLayerGroup element
}

// End of file.
