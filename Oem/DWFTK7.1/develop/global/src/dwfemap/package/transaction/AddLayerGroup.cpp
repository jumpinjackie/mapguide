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

#include "./AddLayerGroup.h"
#include "../EMapConstants.h"
#include "../EMapUtility.h"
#include "../LayerGroup.h"
using namespace DWFToolkit::DWFEMapExtension;


// Construction , Destruction
DWFEMapAddLayerGroupCommand::DWFEMapAddLayerGroupCommand(DWFPackageReader* poPackageReader)
throw() : DWFEMapCommand()
        , _zInsertAfter("")
        , _poLayerGroup(NULL)
        , _bIsLayerGroup(false)
        , _poPackageReader(poPackageReader)
{
}

DWFEMapAddLayerGroupCommand::DWFEMapAddLayerGroupCommand(DWFEMapLayerGroup* poLayerGroup,
                                   const DWFString& zInsertAfterObjectId)
throw() : DWFEMapCommand()
        , _zInsertAfter(zInsertAfterObjectId)
        , _poLayerGroup(poLayerGroup)
        , _bIsLayerGroup(false)
        , _poPackageReader(NULL)
{
}

DWFEMapAddLayerGroupCommand::~DWFEMapAddLayerGroupCommand(void) throw()
{
    DWFCORE_FREE_OBJECT( _poLayerGroup );
}

// acquire member data from attribute list (from XML parser)
_DWFTK_API void DWFEMapAddLayerGroupCommand::parseAttributeList( const char** ppAttributeList )
    throw( DWFException )
{
    const char* pAttrib = NULL;

    // process attributes
    if(ppAttributeList[0]!= NULL)
    {
        pAttrib = DWFEMapUtility::offsetKnownPrefix(ppAttributeList[0]);

        if (DWFCORE_COMPARE_ASCII_STRINGS(pAttrib, DWFEMAPXML::kzAttribute_InsertAfter ) == 0)
        {
            this->_zInsertAfter = ppAttributeList[1];
        }
    }
}

// Operations
const DWFString& DWFEMapAddLayerGroupCommand::insertAfter() const
throw()
{
    return _zInsertAfter;
}

DWFEMapLayerGroup* DWFEMapAddLayerGroupCommand::layerGroup() throw()
{
    return _poLayerGroup;
}

// XMLCallback support
void DWFEMapAddLayerGroupCommand::notifyStartElement( const char*  zName,
                                const char** ppAttributeList )
                                throw()
{
    if( _poPackageReader == NULL)
    {
        return;
    }

    if(!_bIsLayerGroup)
    {
        const char* zElement = DWFEMapUtility::offsetKnownPrefix(zName);

        if( DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                    DWFEMAPXML::kzElement_LayerGroup) == 0)
        {
            _bIsLayerGroup = true;
            _poLayerGroup = DWFCORE_ALLOC_OBJECT( DWFEMapLayerGroup(_poPackageReader) );
            _poLayerGroup->parseAttributeList(ppAttributeList);
        }
    }
    else
    {
        _poLayerGroup->notifyStartElement(zName,ppAttributeList);
    }
}

void DWFEMapAddLayerGroupCommand::notifyEndElement( const char* zName )
throw()
{
    // Since there is only one instance of a LayerGroup there is no need to test
    // for a subsequent element
    if(_bIsLayerGroup)
    {
        _poLayerGroup->notifyEndElement(zName);
    }
}

void DWFEMapAddLayerGroupCommand::notifyStartNamespace( const char* zPrefix,
                                                        const char* zURI )
throw()
{
    // NA
}

void DWFEMapAddLayerGroupCommand::notifyEndNamespace( const char* zPrefix )
throw()
{
    // NA
}


void DWFEMapAddLayerGroupCommand::notifyCharacterData( const char* zCData,
                                                 int nLength )
throw()
{
    if( _bIsLayerGroup)
    {
        _poLayerGroup->notifyCharacterData(zCData, nLength);
    }
}

// DWFXMLSerializable support
void DWFEMapAddLayerGroupCommand::serializeXML( DWFXMLSerializer& rSerializer,
                                                unsigned int nFlags )
throw( DWFException )
{
    // Element AddLayerGroup
    rSerializer.startElement(DWFEMAPXML::kzElement_AddLayerGroup,
                                                DWFEMAPXML::kzNamespace_EMap);
    {
        // Attributes
        {
            // Attribute InsertAfter : Optional
            if( _zInsertAfter != L"")
            {
                rSerializer.addAttribute(DWFEMAPXML::kzAttribute_InsertAfter,
                                         _zInsertAfter, DWFEMAPXML::kzNamespace_EMap);
            }
        }

        // Element LayerGroup
        if( _poLayerGroup != NULL) // should you throw an exception if the LayerGroup doesn't exist?
        {
            _poLayerGroup->serializeXML(rSerializer,nFlags);
        }
    }
    rSerializer.endElement(); // end AddLayerGroup Element
}

// End of file.
