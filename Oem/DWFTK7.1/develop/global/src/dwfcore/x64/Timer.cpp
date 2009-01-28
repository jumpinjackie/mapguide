//
//  Copyright (C) 2003-2009 by Autodesk, Inc.
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
#include "dwfcore/Timer.h"
using namespace DWFCore;

//
// Intel and AMD x64 architecture implementation
//
#ifdef  _DWFCORE_X64_SYSTEM

#ifdef  _DWFCORE_WIN64_SYSTEM

_DWFCORE_API
DWFTimer::DWFTimer()
throw()
        : _nTick( 0 )
{
    ;
}

_DWFCORE_API
DWFTimer::DWFTimer( const DWFTimer& rTimer )
throw()
        : _nTick( rTimer._nTick )
{
    ;
}

_DWFCORE_API
DWFTimer&
DWFTimer::operator=( const DWFTimer& rTimer )
throw()
{
    _nTick = rTimer._nTick;
    return *this;
}

_DWFCORE_API
DWFTimer::~DWFTimer()
throw()
{
}

_DWFCORE_API
unsigned long
DWFTimer::Tick32()
throw()
{
   LARGE_INTEGER  l_Counter;
   QueryPerformanceCounter(&l_Counter);

   return l_Counter.LowPart;
}

_DWFCORE_API
uint64_t
DWFTimer::Tick64()
throw()
{
   LARGE_INTEGER  l_Counter;
   QueryPerformanceCounter(&l_Counter);

   return l_Counter.QuadPart;
}

_DWFCORE_API
void
DWFTimer::start()
throw()
{
    _nTick = Tick64();
}

_DWFCORE_API
unsigned long
DWFTimer::tick32()
throw()
{
    return (Tick32() - (unsigned long)(0x00000000ffffffff & _nTick));
}

_DWFCORE_API
uint64_t
DWFTimer::tick64()
throw()
{
    return (Tick64() - _nTick);
}

_DWFCORE_API
void
DWFTimer::stop()
throw()
{
    _nTick = (Tick64() - _nTick);
}

_DWFCORE_API
unsigned long
DWFTimer::timer32()
throw()
{
    return (unsigned long)(0x00000000ffffffff & _nTick);
}

_DWFCORE_API
uint64_t
DWFTimer::timer64()
throw()
{
    return _nTick;
}

#elif _DWFCORE_LINUX_SYSTEM

//
//  TODO: Need 64-bit Linux
//

#endif


#endif

