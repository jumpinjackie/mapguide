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
#ifndef _DWFEMAPTK_EMAPINTERFACE_H
#define _DWFEMAPTK_EMAPINTERFACE_H

#include "dwf/package/Interface.h"
using namespace DWFToolkit;

namespace DWFToolkit { namespace DWFEMapExtension
{
    //--------------------------------------------------------------------
    // DESCRIPTION:
    // Class DWFEMapInterface represents a document descriptor that is used for
    // describing an EMap section (Map) or an EMapTransaction section
    // (MapTransaction) to the DWF Manifest.
    // It is used while reading or writing a section with a DWFPackageReader
    // or DWFPackageWriter.
    //--------------------------------------------------------------------
    class DWFEMapInterface : public DWFInterface
    {
    public:
        static const wchar_t* const kzEMap_Name;
        static const wchar_t* const kzEMap_HRef;
        static const wchar_t* const kzEMap_ID;

        static const wchar_t* const kzEMapTransaction_Name;
        static const wchar_t* const kzEMapTransaction_HRef;
        static const wchar_t* const kzEMapTransaction_ID;

    public:
        // constructors
        _DWFTK_API DWFEMapInterface() throw();
        _DWFTK_API DWFEMapInterface( const DWFString& zName,
                                     const DWFString& zHRef,
                                     const DWFString& zObjectID ) throw();

        // destructor
        _DWFTK_API  virtual ~DWFEMapInterface() throw();
    };
}}

#endif // _DWFEMAPTK_EMAPINTERFACE_H
// End of file.
