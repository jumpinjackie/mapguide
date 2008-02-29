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

#include "./EMapConstants.h"
#include "./EMapUtility.h"
#include "dwf/package/Constants.h"
using namespace DWFToolkit::DWFEMapExtension;


const char* DWFEMapUtility::offsetKnownPrefix(const char* zString)
{
    const char* zRetStr = zString;
    if (DWFCORE_COMPARE_MEMORY(DWFXML::kzNamespace_DWF, zString, 4) == 0)
    {
        zRetStr = &zString[4];
    }
    else if (DWFCORE_COMPARE_MEMORY(DWFEMAPXML::kzNamespace_EMap, zString, 5) == 0)
    {
        zRetStr = &zString[5];
    }

    return zRetStr;
}

// TODO: Make this more robust;
bool DWFEMapUtility::convertStringToBool(const char* zString)
{
    if ((zString[0] == 't') ||      // true
        (zString[0] == 'T') ||      // True | TRUE
        (zString[0] == '1') ||      // 1
        (zString[0] == 'y') ||      // yes
        (zString[0] == 'Y'))        // Yes | YES
   {
        return true;
   }

   return false;
}

//End of file.
