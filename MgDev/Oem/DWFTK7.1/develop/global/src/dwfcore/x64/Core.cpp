//
//  Copyright (C) 2003-2011 by Autodesk, Inc.
//
//  By using this code, you are agreeing to the terms and conditions of
//  the License Agreement included in the documentation for this code.
//
//  AUTODESK MAKES NO WARRANTIES, EXPRESS OR IMPLIED,
//  AS TO THE CORRECTNESS OF THIS CODE OR ANY DERIVATIVE
//  WORKS WHICH INCORPORATE IT.
//
//  AUTODESK PROVIDES THE CODE ON AN "AS-IS" BASIS
//  AND EXPLICITLY DISCLAIMS ANY LIABILITY, INCLUDING
//  CONSEQUENTIAL AND INCIDENTAL DAMAGES FOR ERRORS,
//  OMISSIONS, AND OTHER PROBLEMS IN THE CODE.
//
//  Use, duplication, or disclosure by the U.S. Government is subject to
//  restrictions set forth in FAR 52.227-19 (Commercial Computer Software
//  Restricted Rights) and DFAR 252.227-7013(c)(1)(ii) (Rights in Technical
//  Data and Computer Software), as applicable.
//


#include "dwfcore/Core.h"

//
// Intel and AMD x64 architecture implementation
//
#ifdef  _DWFCORE_X64_SYSTEM

#ifdef  _DWFCORE_WIN64_SYSTEM

_DWFCORE_API
int
DWFCore::AtomicIncrement( volatile int* pValue )
throw()
{
   //
   // An assertion failure results in error C2118: negative subscript.
   //
   C_ASSERT(sizeof(int) == sizeof(long));

   return InterlockedIncrement((volatile long*)pValue);
}

_DWFCORE_API
int
DWFCore::AtomicDecrement( volatile int* pValue )
throw()
{
   return InterlockedDecrement((volatile long*)pValue);
}

#elif _DWFCORE_LINUX_SYSTEM

//
//  TODO: Need 64-bit Linux
//  for AtomicIncrement and AtomicDecrement
//

#endif

#endif


