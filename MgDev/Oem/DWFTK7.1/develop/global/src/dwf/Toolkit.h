//
//  Copyright (C) 1996-2011 by Autodesk, Inc.
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

#ifndef _DWFTK_TOOLKIT_H
#define _DWFTK_TOOLKIT_H


///
///\file        dwf/Toolkit.h
///\brief       This is the main configuration header for the DWF Toolkit library.
///

#include "dwfcore/Core.h"

#ifdef	HAVE_CONFIG_H
#include "dwf/config.h"
#endif


#ifndef DWFTK_STATIC

#ifdef  _DWFCORE_WIN32_SYSTEM
#ifdef  DWFTK_EXPORT_API
#define _DWFTK_API          __declspec( dllexport )
#else
#define _DWFTK_API          __declspec( dllimport )
#endif
#else
#define _DWFTK_API
#endif

#else
#define _DWFTK_API

#ifndef BSTREAM_STATIC_LIB
#define BSTREAM_STATIC_LIB
#endif

#endif

//
// we don't expose expat
//
#ifndef EXPAT_STATIC
#define EXPAT_STATIC
#endif

///
///\brief       DWF Toolkit class library.
/// 
///             This namespace is used for every function, interface, class 
///             and enumeration defined and exposed by this library.
///
#define _DWFTK_EXPORT_API_MEMORY_CONTROL_BASE
#define _DWFTK_EXPORT_API_MEMORY_CONTROL_BASE_MEMBER

#ifndef DWFTK_STATIC

#undef  _DWFTK_EXPORT_API_MEMORY_CONTROL_BASE
#undef  _DWFTK_EXPORT_API_MEMORY_CONTROL_BASE_MEMBER

#define _DWFTK_EXPORT_API_MEMORY_CONTROL_BASE           : virtual public _DWFToolkitMemory
#define _DWFTK_EXPORT_API_MEMORY_CONTROL_BASE_MEMBER    , virtual public _DWFToolkitMemory


namespace DWFToolkit
{
    ///
    ///\class   DWFToolkitMemory      dwf/Toolkit.h  "dwf/Toolkit.h"
    ///\brief   Base class for all toolkit objects.
    ///\since   7.0.1
    ///
    ///\warning Do not derive abstract interfaces from this base.
    ///
    ///         If configured with the \b DWFTK_ENABLE_MEMORY_BASE
    ///         macro, \a new() and \a delete() operators are defined
    ///         for every concrete class in library.  Their implementation
    ///         may be customized or the global operators can be automatically
    ///         provided by also defining the \b DWFTK_USE_DEFAULT_MEMORY_BASE_IMPL macro.
    ///
    class _DWFTK_API _DWFToolkitMemory
    {
        public:

            ///
            ///     Allocation Operator
            ///
            void* operator new( size_t );

            ///
            ///     Placement Allocation Operator
            ///
            void* operator new( size_t, void* );

            ///
            ///     Deallocation Operator
            ///
            void  operator delete( void* );

            ///
            ///     Placement Deallocation Operator
            ///
            void  operator delete( void*, void* );
    };
}

#endif

#endif

