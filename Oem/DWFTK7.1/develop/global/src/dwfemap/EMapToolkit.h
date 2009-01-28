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
#ifndef _DWFEMAPTK_EMAPTOOLKIT_H
#define _DWFEMAPTK_EMAPTOOLKIT_H

#include "dwfcore/Core.h"
#include "dwf/Toolkit.h"
#include "dwf/package/XML.h"

#ifdef _DWFCORE_WIN32_SYSTEM
    #ifndef DWFTK_STATIC
        #ifdef  DWFEMAPTK_EXPORT_API
            #define _DWFEMAPTK_API          __declspec( dllexport )
        #else
            #define _DWFEMAPTK_API          __declspec( dllimport )
        #endif
    #else
        #define _DWFEMAPTK_API
    #endif
#else
    #define _DWFEMAPTK_API
#endif

#ifndef EXPAT_STATIC
    #define EXPAT_STATIC
#endif

#ifndef BSTREAM_STATIC_LIB
    #define BSTREAM_STATIC_LIB
#endif

#endif //_DWFEMAPTK_EMAPTOOLKIT_H

