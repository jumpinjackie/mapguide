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
#ifndef _DWFEMAPTK_EMAPSECTIONDESCRIPTORREADER_H
#define _DWFEMAPTK_EMAPSECTIONDESCRIPTORREADER_H

#include "../EMapToolkit.h"
#include "dwf/package/reader/SectionDescriptorReader.h"
using namespace DWFToolkit;

namespace DWFToolkit { namespace DWFEMapExtension
{
    //--------------------------------------------------------------------
    // DESCRIPTION:
    // The class DWFEMapSectionDescriptorReader... TODO.
    //--------------------------------------------------------------------
    class DWFEMapSectionDescriptorReader : public DWFSectionDescriptorReader
    {
    public:
        typedef enum
        {
            eProvideNone            = 0x000,

            eProvideName            = 0x001,
            eProvideObjectID        = 0x002,
            eProvideUri             = 0x004,

            eProvideBackground      = 0x008,
            eProvideCoordinateSpace = 0x010,
            eProvideProperties      = 0x020,
            eProvideResources       = 0x040,
            eProvideEMapLayers      = 0x080,
            eProvideEMapLayerGroups = 0x100,

            eProvideAttributes      = 0x00F,
            eProvideElements        = 0x7F0,

            eProvideAll             = 0xFFF
        } teProviderType;

        // constructor
        _DWFTK_API DWFEMapSectionDescriptorReader(
                                DWFPackageReader* pPackageReader = NULL,
                                unsigned int nProviderFlags = eProvideAll ) throw();
        // destructor
        _DWFTK_API virtual ~DWFEMapSectionDescriptorReader() throw();
    };
}}

#endif // _DWFEMAPTK_EMAPSECTIONDESCRIPTORREADER_H
