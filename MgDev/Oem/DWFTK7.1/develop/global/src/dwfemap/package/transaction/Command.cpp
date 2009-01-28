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

#include "./Command.h"
#include "./AddLayer.h"
#include "./RemoveLayer.h"
#include "./UpdateLayer.h"
#include "./AddLayerGroup.h"
#include "./RemoveLayerGroup.h"
#include "./UpdateLayerGroup.h"
#include "./UpdateUIGraphic.h"
#include "../EMapConstants.h"
using namespace DWFToolkit::DWFEMapExtension;


DWFEMapCommand::DWFEMapCommand()
throw() : DWFXMLBuildable()
{
}

DWFEMapCommand::~DWFEMapCommand(void) throw()
{
}


_DWFTK_API void DWFEMapCommand::parseAttributeList( const char** ppAttributeList )
    throw( DWFException )
{
}

DWFEMapCommand* DWFEMapCommand::ConstructCommand( const char* pzCommandName,
                                                  const char** ppAttributeList,
                                                  DWFPackageReader* pPackageReader)
                                                  throw()
{
    DWFEMapCommand* pRetCommand = NULL;

    if (pPackageReader != NULL && DWFCORE_COMPARE_ASCII_STRINGS(pzCommandName,
                                                                DWFEMAPXML::kzElement_AddLayer) == 0)
    {
        pRetCommand = DWFCORE_ALLOC_OBJECT( DWFEMapAddLayerCommand(pPackageReader) );
        pRetCommand->parseAttributeList( ppAttributeList );
    }
    else if (DWFCORE_COMPARE_ASCII_STRINGS(pzCommandName,
                                           DWFEMAPXML::kzElement_RemoveLayer) == 0)
    {
        pRetCommand = DWFCORE_ALLOC_OBJECT( DWFEMapRemoveLayerCommand() );
        pRetCommand->parseAttributeList( ppAttributeList );
    }
    else if (DWFCORE_COMPARE_ASCII_STRINGS(pzCommandName,
                                           DWFEMAPXML::kzElement_UpdateLayer) == 0)
    {
        pRetCommand = DWFCORE_ALLOC_OBJECT( DWFEMapUpdateLayerCommand());
        pRetCommand->parseAttributeList( ppAttributeList );
    }
    else if (pPackageReader != NULL && DWFCORE_COMPARE_ASCII_STRINGS(pzCommandName,
                                                                     DWFEMAPXML::kzElement_AddLayerGroup) == 0)
    {
        pRetCommand = DWFCORE_ALLOC_OBJECT(DWFEMapAddLayerGroupCommand(pPackageReader));
        pRetCommand->parseAttributeList( ppAttributeList );
    }
    else if (DWFCORE_COMPARE_ASCII_STRINGS(pzCommandName,
                                           DWFEMAPXML::kzElement_RemoveLayerGroup) == 0)
    {
        pRetCommand = DWFCORE_ALLOC_OBJECT(DWFEMapRemoveLayerGroupCommand());
        pRetCommand->parseAttributeList( ppAttributeList );
    }
    else if (DWFCORE_COMPARE_ASCII_STRINGS(pzCommandName,
                                           DWFEMAPXML::kzElement_UpdateLayerGroup) == 0)
    {
        pRetCommand = DWFCORE_ALLOC_OBJECT(DWFEMapUpdateLayerGroupCommand());
        pRetCommand->parseAttributeList( ppAttributeList );
    }
    else if (DWFCORE_COMPARE_ASCII_STRINGS(pzCommandName,
                                           DWFEMAPXML::kzElement_UpdateUIGraphic) == 0)
    {
        pRetCommand = DWFCORE_ALLOC_OBJECT(DWFEMapUpdateUIGraphicCommand());
        pRetCommand->parseAttributeList( ppAttributeList );
    }
    return pRetCommand;
}

//End of file.
