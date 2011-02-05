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
#ifndef _DWFEMAPTK_COMMAND_H
#define _DWFEMAPTK_COMMAND_H

#include "../../EMapToolkit.h"
#include "dwf/package/reader/PackageReader.h"
#include "dwf/package/writer/DWFXMLSerializer.h"
using namespace DWFToolkit;

namespace DWFToolkit { namespace DWFEMapExtension
{
    //--------------------------------------------------------------------
    // DESCRIPTION:
    // The class DWFEMapCommand... TODO.
    //--------------------------------------------------------------------
    class DWFEMapCommand : public DWFXMLBuildable
                         , public DWFXMLCallback
                         , public DWFXMLSerializable
    {
    public:
        _DWFTK_API static DWFEMapCommand* ConstructCommand(const char* pzCommandName,
                                                           const char** ppAttributeList,
                                                           DWFPackageReader* pPackageReader) throw();
    public:
        // constructor / destructor
        _DWFTK_API DWFEMapCommand() throw();
        _DWFTK_API virtual ~DWFEMapCommand(void) throw();

        // acquire member data from attribute list (from XML parser)
        _DWFTK_API virtual void parseAttributeList(const char** ppAttributeList) throw(DWFException);

        // None of the interfaces are implemented
    };
}}

#endif // _DWFEMAPTK_COMMAND_H
// End of file.
