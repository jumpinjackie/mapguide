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
#ifndef _DWFEMAPTK_TRANSACTIONSECTIONDESCRIPTORREADER_H
#define _DWFEMAPTK_TRANSACTIONSECTIONDESCRIPTORREADER_H

#include "../../EMapToolkit.h"
#include "dwf/package/reader/SectionDescriptorReader.h"
using namespace DWFToolkit;

namespace DWFToolkit { namespace DWFEMapExtension
{
    //--------------------------------------------------------------------
    // DESCRIPTION:
    // The class DWFEMapTransactionSectionDescriptorReader... TODO.
    //--------------------------------------------------------------------
    class DWFEMapTransactionSectionDescriptorReader : public DWFSectionDescriptorReader
    {
    public:
        // constructor
        _DWFTK_API DWFEMapTransactionSectionDescriptorReader(DWFPackageReader* pPackageReader = NULL,
                                                             unsigned int nProviderFlags = 0xFF) throw();

        // destructor
        _DWFTK_API virtual ~DWFEMapTransactionSectionDescriptorReader() throw();
    };
}}

#endif // _DWFEMAPTK_TRANSACTIONSECTIONDESCRIPTORREADER_H
// End of file
