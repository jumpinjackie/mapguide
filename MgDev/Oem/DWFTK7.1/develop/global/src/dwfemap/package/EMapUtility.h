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
#ifndef _DWFEMAPTK_EMAPUTILITY_H
#define _DWFEMAPTK_EMAPUTILITY_H

#include "../EMapToolkit.h"

namespace DWFToolkit { namespace DWFEMapExtension
{
// utility definitions
#define _DWFEMAPTK_TEMP_BUFFER_BYTES  1024

    //--------------------------------------------------------------------
    // DESCRIPTION:
    // Class DWFEMapUtility is a helper class for the DWF EMap library.
    // Common routines that deal with parsing and are not supplied by the
    // DWFCore or DWF Toolkit libraries are added here.
    //--------------------------------------------------------------------
    class DWFEMapUtility
    {
    public:
        static const char* offsetKnownPrefix(const char* zString);
        static bool convertStringToBool(const char* zString);
    };
}}

#endif // _DWFEMAPTK_EMAPUTILITY_H
// End of file.
