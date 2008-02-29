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

#include "./AddLayer.h"
#include "../EMapConstants.h"
#include "../EMapUtility.h"
#include "../Layer.h"
using namespace DWFToolkit::DWFEMapExtension;


// Construction , Destruction
DWFEMapAddLayerCommand::DWFEMapAddLayerCommand(DWFPackageReader* poPackageReader)
throw() : DWFEMapCommand()
        , _poLayer(NULL)
        , _zInsertAfter("")
        , _bIsLayer(false)
        , _poPackageReader(poPackageReader)
{
}

DWFEMapAddLayerCommand::DWFEMapAddLayerCommand(DWFEMapLayer* poLayer,
                                   const DWFString& zInsertAfterObjectId)
throw() : DWFEMapCommand()
        , _poLayer(poLayer)
        , _zInsertAfter(zInsertAfterObjectId)
        , _bIsLayer(false)
        , _poPackageReader(NULL)
{
}

DWFEMapAddLayerCommand::~DWFEMapAddLayerCommand(void) throw()
{
    DWFCORE_FREE_OBJECT( _poLayer );
}

// acquire member data from attribute list (from XML parser)
_DWFTK_API void DWFEMapAddLayerCommand::parseAttributeList( const char** ppAttributeList )
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
const DWFString& DWFEMapAddLayerCommand::insertAfter() const
throw()
{
    return _zInsertAfter;
}

DWFEMapLayer* DWFEMapAddLayerCommand::layer() throw()
{
    return _poLayer;
}

// XMLCallback support
void DWFEMapAddLayerCommand::notifyStartElement( const char*  zName,
                                        const char** ppAttributeList )
throw()
{
    if( _poPackageReader == NULL)
    {
        return;
    }

    if(!_bIsLayer)
    {
        const char* zElement = DWFEMapUtility::offsetKnownPrefix(zName);

        if( DWFCORE_COMPARE_ASCII_STRINGS(zElement,
                                    DWFEMAPXML::kzElement_Layer) == 0)
        {
            _bIsLayer = true;
            _poLayer = DWFCORE_ALLOC_OBJECT(DWFEMapLayer(_poPackageReader));
            _poLayer->parseAttributeList( ppAttributeList );
        }
    }
    else
    {
        _poLayer->notifyStartElement(zName,ppAttributeList);
    }
}

void DWFEMapAddLayerCommand::notifyEndElement( const char* zName )
throw()
{
    // Since there is only one instance of a Layer there is no need to test
    // for a subsequent element
    if(_bIsLayer)
    {
        _poLayer->notifyEndElement(zName);
    }
}

void DWFEMapAddLayerCommand::notifyStartNamespace( const char* zPrefix,
                                                   const char* zURI )
throw()
{
    // NA
}

void DWFEMapAddLayerCommand::notifyEndNamespace( const char* zPrefix )
throw()
{
    // NA
}


void DWFEMapAddLayerCommand::notifyCharacterData( const char* zCData,
                                                 int nLength )
throw()
{
    if( _bIsLayer)
    {
        _poLayer->notifyCharacterData(zCData, nLength);
    }
}

// DWFXMLSerializable support
void DWFEMapAddLayerCommand::serializeXML( DWFXMLSerializer& rSerializer,
                                           unsigned int nFlags )
throw( DWFException )
{
    // Element AddLayer
    rSerializer.startElement(DWFEMAPXML::kzElement_AddLayer,
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

        // Element Layer
        if( _poLayer != NULL) // should you throw an exception if the Layer doesn't exist?
        {
            _poLayer->serializeXML(rSerializer,nFlags);
        }
    }
    rSerializer.endElement(); // end AddLayer Element
}

//End of file
